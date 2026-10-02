#include "StdInc.h"

#include "TaskSimpleDieInCar.h"
#include "TaskSimpleDie.h"

void CTaskSimpleDieInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDieInCar, 0x86DDE0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62FC20);
    RH_ScopedInstall(Destructor, 0x62FC70);

    RH_ScopedVMTInstall(Clone, 0x635EF0);
    RH_ScopedVMTInstall(GetTaskType, 0x62FC60);
    RH_ScopedVMTInstall(ProcessPed, 0x6398F0);
}

// 0x62FC20
CTaskSimpleDieInCar::CTaskSimpleDieInCar(AssocGroupId groupId, AnimationId animId) : CTaskSimpleDie(groupId, animId, 4.0f, 0.0f) {
    // NOP
}

// 0x6398F0
bool CTaskSimpleDieInCar::ProcessPed(CPed* ped) {
    return CTaskSimpleDie::ProcessPed(ped);
}
