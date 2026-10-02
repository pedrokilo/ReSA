#include "StdInc.h"

#include "TaskSimpleScratchHead.h"

void CTaskSimpleScratchHead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleScratchHead, 0x85A100, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x48DF60);
    RH_ScopedVMTInstall(IsInterruptable, 0x48DFD0);
}
