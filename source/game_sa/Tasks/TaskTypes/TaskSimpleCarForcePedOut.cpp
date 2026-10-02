#include "StdInc.h"

#include "TaskSimpleCarForcePedOut.h"

void CTaskSimpleCarForcePedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarForcePedOut, 0x86EE94, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x647770);
    // TODO: instalar las funciones conforme se reconstruyan.
}
