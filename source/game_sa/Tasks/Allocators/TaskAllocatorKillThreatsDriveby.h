#pragma once

#include "TaskAllocatorKillThreatsBasic.h"

// Como el original: no hereda de `CTaskAllocatorKillThreatsBasic` (0x14 bytes: vtable, amenaza y temporizador).
// Antes heredaba y volvía a declarar m_Threat/m_Timer: el objeto medía 0x24 y AllocateTasks leía la amenaza de +0x14.
class NOTSA_EXPORT_VTABLE CTaskAllocatorKillThreatsDriveby final : public CTaskAllocator {
public:
    constexpr static inline auto Type = eTaskAllocatorType::KILL_THREATS_DRIVEBY;

    static void InjectHooks();

    explicit CTaskAllocatorKillThreatsDriveby(CPed* threat);
    ~CTaskAllocatorKillThreatsDriveby() override = default;

    eTaskAllocatorType GetType() override { return eTaskAllocatorType::KILL_THREATS_DRIVEBY; }; // 0x69CB50
    void               AllocateTasks(CPedGroupIntelligence* intel) override;
    CTaskAllocator*    ProcessGroup(CPedGroupIntelligence* intel) override;

public:
    CPed::Ref  m_Threat;
    CTaskTimer m_Timer; // Como el original: sin `{}` (la inicialización por valor pondría a cero el relleno 0x12..0x13)

private: // Wrappers for hooks
    // 0x69CAF0
    CTaskAllocatorKillThreatsDriveby* Constructor(CPed* a) {
        this->CTaskAllocatorKillThreatsDriveby::CTaskAllocatorKillThreatsDriveby(a);
        return this;
    }

    // 0x69CB60
    CTaskAllocatorKillThreatsDriveby* Destructor() {
        this->CTaskAllocatorKillThreatsDriveby::~CTaskAllocatorKillThreatsDriveby();
        return this;
    }
};
VALIDATE_SIZE(CTaskAllocatorKillThreatsDriveby, 0x14);
