#include "StdInc.h"

#include "TaskComplexWanderGang.h"

void CTaskComplexWanderGang::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderGang, 0x8700D4, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66F5C0);

    RH_ScopedVMTInstall(Clone, 0x671470);
    RH_ScopedVMTInstall(GetWanderType, 0x66F630);
    RH_ScopedVMTInstall(ScanForStuff, 0x66F640);
}

// 0x66F5C0
CTaskComplexWanderGang::CTaskComplexWanderGang(eMoveState moveState, uint8 dir, uint32 scanTime, bool bWanderSensibly, float fTargetRadius)
    : CTaskComplexWander(moveState, dir, bWanderSensibly, fTargetRadius)
{
    m_NextScanTime = scanTime;
    m_TaskTimer.m_nInterval = scanTime;
    m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_TaskTimer.m_bStarted = true;
}

// 0x66F640
void CTaskComplexWanderGang::ScanForStuff(CPed* ped) {
    // NOP
}
