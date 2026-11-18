#pragma once

#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "PedTaskPair.h"
#include "SArray.h"
#include <Enums/eMoveState.h>

class CEntity;
class CPed;
class CTask;

class NOTSA_EXPORT_VTABLE CPedAttractor {
    inline static auto& ms_tasks = StaticRef<SArray<CTask*>>(0xC0985C);

public:
    static void InjectHooks();

    void* operator new(unsigned size);
    void  operator delete(void* object);

    CPedAttractor(
        C2dEffectPedAttractor* fx,
        CEntity*               entity,
        eMoveState             moveState,
        size_t                 maxNoOfPeds,
        float                  spacing,
        float                  achieveQueueTime,
        float                  achieveQueueShuffleTime,
        float                  arriveRange,
        float                  headingRange,
        float                  deltaPos,
        float                  deltaHeading
    );

    // Orden de la vtable del EXE (0x86C538): GetType (virtual pura) va antes que el destructor.
    virtual int32 GetType() const { return PED_ATTRACTOR_UNDEFINED; } // int32 (no el enum de 8 bits): los llamadores del EXE comparan EAX completo (cmp eax, 9)

    virtual ~CPedAttractor() {}

    static void Shutdown();

    void SetTaskForPed(CPed* ped, CTask* task);
    bool RegisterPed(CPed* ped);
    bool DeRegisterPed(CPed* ped);

    bool IsRegisteredWithPed(const CPed* ped) const;

    virtual bool IsAtHeadOfQueue(CPed* ped);

    CTask*   GetTaskForPed(CPed* ped);
    size_t   GetNoOfRegisteredPeds() const { return m_AttractPeds.size() + m_ArrivedPeds.size(); }
    int32    GetQueueSlot(const CPed*);
    CPed*    GetHeadOfQueue() const;
    CPed*    GetTailOfQueue() const;
    auto&    GetArrivedPeds() { return m_ArrivedPeds; }
    size_t   GetSizeOfQueue() const { return m_ArrivedPeds.size(); }
    auto&    GetAttractPeds() { return m_AttractPeds; }
    auto     GetEffect() const { return m_Fx; }
    CEntity* GetEntity() const { return m_Entity; }

    // 0x5EAF10
    bool         HasEmptySlot() const { return (int32)GetNoOfRegisteredPeds() < (int32)m_MaxNumPeds; } // Como el original: comparación con signo (setl)
    int32        ComputeFreeSlot();

    float        ComputeDeltaPos() const;
    float        ComputeDeltaHeading() const;

    void         ComputeAttractTime(int32 slotIdx, bool hasArrived, float& outTime) const;

    virtual void ComputeAttractPos(int32 pedId, CVector& outPos);
    CVector      ComputeAttractPos(int32 pedId) { CVector pos;  ComputeAttractPos(pedId, pos); return pos; }

    virtual void ComputeAttractHeading(int32 bQueue, float& heading);
    float        ComputeAttractHeading(int32 bQueue) { float heading; ComputeAttractHeading(bQueue, heading); return heading; }

    virtual bool BroadcastDeparture(CPed* ped);
    bool         BroadcastArrival(CPed* ped);

    void AbortPedTasks();

public:
    // Casting.hpp support //
    template<typename From, typename Self>
    static constexpr bool classof(const From* f) { return f->GetType() == Self::Type; }

private:
    // 0x5EDFB0
    auto* Constructor(C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState, size_t maxNoOfPeds, float spacing, float achieveQueueTime, float achieveQueueShuffleTime, float arriveRange, float headingRange, float deltaPos, float deltaHeading) {
        return std::construct_at(this,fx, entity, moveState, maxNoOfPeds, spacing, achieveQueueTime, achieveQueueShuffleTime, arriveRange, headingRange, deltaPos, deltaHeading);
    }

public:
    // 0x5EC410 (enganchado): cuerpo del destructor. Publico desde el lote 206A: lo llama el destructor con borrado 0x5EC4B0 (PedAttractor.cpp) como el EXE (que llama a 0x5EC410), no el destructor de C++ directamente.
    auto* Destructor() {
        // Como el original: llamada no virtual; std::destroy_at llamaba al destructor por la vtable del EXE,
        // que vuelve a 0x5EC410 (enganchado aquí) -> bucle infinito
        this->CPedAttractor::~CPedAttractor();
        return this;
    }
    C2dEffectPedAttractor* m_Fx{};
    notsa::EntityRef<>     m_Entity{};
    SArray<CPed*>          m_AttractPeds;  // Como el original: sin inicializar por valor (el constructor no escribe el allocator)
    SArray<CPed*>          m_ArrivedPeds;  // AKA queue
    SArray<CPedTaskPair>   m_PedTaskPairs;
    size_t                 m_MaxNumPeds{}; //!< Maximum number of registered peds (ArrivedPeds + AttractPeds, see `GetNoOfRegisteredPeds()`)
    float                  m_Spacing{};
    float                  m_AchieveQueueTime{};
    float                  m_AchieveQueueShuffleTime{};
    float                  m_ArriveRange{};
    float                  m_HeadingRange{};
    float                  m_DeltaPos{};
    float                  m_DeltaHeading{};
    CVector                m_Pos{};
    CVector                m_QueueDir{};
    CVector                m_UseDir{};
    eMoveState             m_MoveState{};
    char                   m_ScriptName[8]; // Como el original: solo lo escribe strcpy
};
VALIDATE_SIZE(CPedAttractor, 0x8C);
