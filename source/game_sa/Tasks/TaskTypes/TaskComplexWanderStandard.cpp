#include "StdInc.h"

#include "TaskComplexWanderStandard.h"

void CTaskComplexWanderStandard::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderStandard, 0x85A200, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E4F0);
    RH_ScopedInstall(WillChat, 0x66AED0, { .reversed = false });
    RH_ScopedInstall(SetNextMinScanTime, 0x66AF60, { .reversed = false });
    RH_ScopedInstall(LookForSexyCars, 0x66AFD0, { .reversed = false });
    RH_ScopedInstall(LookForChatPartners, 0x66FDA0, { .reversed = false });
    RH_ScopedInstall(LookForGangMembers, 0x670100, { .reversed = false });

    // El original (0x48E530) pasa también m_bWanderSensibly; el Clone inline usa el valor por defecto.
    RH_ScopedVMTInstall(Clone, 0x48E530, { .reversed = false });
    RH_ScopedVMTInstall(GetWanderType, 0x48E5D0);
    RH_ScopedVMTInstall(ScanForStuff, 0x672600);
}

// 0x48E4F0
CTaskComplexWanderStandard::CTaskComplexWanderStandard(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWander(MoveState, Dir, bWanderSensibly),
    m_nMinNextScanTime{ 0 }
{
}

// 0x672600
void CTaskComplexWanderStandard::ScanForStuff(CPed* ped) {
    if (!m_TaskTimer.m_bStarted) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval = 50;
        m_TaskTimer.m_bStarted = true;
    }

    if (CTimer::GetTimeInMS() < m_nMinNextScanTime)
        return;

    if (m_TaskTimer.m_bStarted) { // V547 Expression 'm_TaskTimer.m_bStarted' is always true.
        if (m_TaskTimer.m_bStopped) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStopped = false;
        }

        if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + m_TaskTimer.m_nInterval) {
            m_TaskTimer.m_nInterval = 50;
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStarted = true;
            if (!LookForGangMembers(ped) && !LookForChatPartners(ped)) {
                CTaskComplexWanderStandard::LookForSexyCars(ped);
            }
        }
    }
}

// 0x66AED0
bool CTaskComplexWanderStandard::WillChat(const CPed& first, const CPed& second) {
    return plugin::CallMethodAndReturn<bool, 0x66AED0, CTaskComplexWanderStandard*, const CPed&, const CPed&>(this, first, second);
}

// 0x66AF60
void CTaskComplexWanderStandard::SetNextMinScanTime(CPed* ped) {
    plugin::CallMethod<0x66AF60, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

// 0x66AFD0
bool CTaskComplexWanderStandard::LookForSexyCars(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x66AFD0, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

// 0x66FDA0
bool CTaskComplexWanderStandard::LookForChatPartners(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x66FDA0, CTaskComplexWanderStandard*, CPed*>(this, ped);
}

// 0x670100
bool CTaskComplexWanderStandard::LookForGangMembers(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x670100, CTaskComplexWanderStandard*, CPed*>(this, ped);
}
