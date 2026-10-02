#include "StdInc.h"

#include "TaskSimpleWaitForPizza.h"

void CTaskSimpleWaitForPizza::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitForPizza, 0x86E1DC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x632B50);
    // TODO: instalar las funciones conforme se reconstruyan.
}
