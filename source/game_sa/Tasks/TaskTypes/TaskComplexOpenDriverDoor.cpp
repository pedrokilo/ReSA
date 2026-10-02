#include "StdInc.h"

#include "TaskComplexOpenDriverDoor.h"

void CTaskComplexOpenDriverDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenDriverDoor, 0x86EB0C, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x640390);
    RH_ScopedInstall(Destructor, 0x6403D0);

    RH_ScopedVMTInstall(Clone, 0x643870);
    RH_ScopedVMTInstall(GetTaskType, 0x6403C0);
}
