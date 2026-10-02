#include "StdInc.h"

#include "TaskComplexWanderMedic.h"

void CTaskComplexWanderMedic::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderMedic, 0x86F48C, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x658770);

    RH_ScopedVMTInstall(Clone, 0x6587A0);
    RH_ScopedVMTInstall(GetWanderType, 0x658810);
    RH_ScopedVMTInstall(ScanForStuff, 0x658820);
}

// 0x658770
CTaskComplexWanderMedic::CTaskComplexWanderMedic(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) : CTaskComplexWander(MoveState, Dir, bWanderSensibly) {
    // NOP
}
