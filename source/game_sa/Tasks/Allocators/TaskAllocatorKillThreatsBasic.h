#pragma once

#include "TaskAllocator.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorKillThreatsBasic : public CTaskAllocator {
public:
    constexpr static inline auto Type = eTaskAllocatorType::KILL_THREATS_BASIC;

    static void InjectHooks();

    explicit CTaskAllocatorKillThreatsBasic(CPed* threat);
    ~CTaskAllocatorKillThreatsBasic() override = default;

    eTaskAllocatorType GetType() override { return eTaskAllocatorType::KILL_THREATS_BASIC; }; // 0x69C2C0
    void               AllocateTasks(CPedGroupIntelligence* intel) override;
    CTaskAllocator*    ProcessGroup(CPedGroupIntelligence* intel) final override;

protected:
    static void ComputeClosestPeds(CPedGroup& group1, CPedGroup& group2, CPed** peds);

protected:
    CPed::Ref  m_Threat{};
    CTaskTimer m_Timer; // Como el original: sin inicializacion por valor (los bytes de relleno 0x12..0x13 no se escriben)

public: // Envoltura del cuerpo 0x69C780 (con hook): la llama el cuerpo reconstruido de CTaskAllocatorKillThreatsBasicRandomGroup (0x5F6920; lote 141A, banco Fn141A), sin expansion en linea
    // 0x69C780
    __declspec(noinline) CTaskAllocatorKillThreatsBasic* Destructor() {
        this->CTaskAllocatorKillThreatsBasic::~CTaskAllocatorKillThreatsBasic();
        return this;
    }

private: // Wrappers for hooks
    // 0x69C710
    CTaskAllocatorKillThreatsBasic* Constructor(CPed * ped) {
        this->CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic(ped);
        return this;
    }
};
