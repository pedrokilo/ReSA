#pragma once

#include "TaskComplex.h"

class CTaskComplexSequence;

enum ePartnerState : int8 {
    PARTNER_STATE_UNK_1 = 1
};

class NOTSA_EXPORT_VTABLE CTaskComplexPartner : public CTaskComplex {
public:
    int32         field_C;
    int32         field_10;
    char          m_commandName[32];
    int32         m_taskId;
    CPed*         m_partner;
    float         m_distanceMultiplier;
    CVector       m_point;
    CVector       m_targetPoint;
    bool          m_leadSpeaker;
    ePartnerState m_partnerState;
    int8          m_firstToTargetFlag;
    int8          m_updateDirectionCount;
    bool          m_taskCompleted;
    bool          m_makePedAlwaysFacePartner;
    char          m_animBlockName[16];
    bool          m_requiredAnimsStreamedIn;

public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER;

    CTaskComplexPartner(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point);
    ~CTaskComplexPartner() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*       CreateNextSubTask(CPed* ped) override;
    CTask*       CreateFirstSubTask(CPed* ped) override;
    CTask*       ControlSubTask(CPed* ped) override;
    virtual void StreamRequiredAnims();
    virtual void RemoveStreamedAnims();
    virtual CTaskComplexSequence* GetPartnerSequence() = 0; // ranura 13 (en la vtable de la base 0x870664 es _purecall)

    // 0x681FE0: calcula los dos puntos de encuentro (outPoint = centro - mitad de la direccion, outTargetPoint = centro + mitad); sin convergencia marca m_taskCompleted
    void ComputeTargetPoints(CPed* ped, CVector* outPoint, CVector* outTargetPoint);

    // 0x6822B0 (sin simbolo en el EXE): estado (con signo) de la tarea equivalente del companero, 0 si no la tiene
    int32 GetStateOfPartnerTask() const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexPartner* Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point);
};

VALIDATE_SIZE(CTaskComplexPartner, 0x70);

namespace CTaskComplexPartnerBody {
// 0x683A40: cuerpo no virtual de ~CTaskComplexPartner del EXE (lo llaman los cuerpos de destructor de las subclases)
void __fastcall Destructor(CTaskComplexPartner* self, void* edx);
}
