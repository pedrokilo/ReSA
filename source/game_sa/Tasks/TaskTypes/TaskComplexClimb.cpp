#include "StdInc.h"

#include "TaskComplexClimb.h"

void CTaskComplexClimb::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexClimb, 0x859DCC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x46A630);
    RH_ScopedInstall(Destructor, 0x46A6E0);

    RH_ScopedVMTInstall(Clone, 0x46A650);
    RH_ScopedVMTInstall(GetTaskType, 0x46A6B0);
}
