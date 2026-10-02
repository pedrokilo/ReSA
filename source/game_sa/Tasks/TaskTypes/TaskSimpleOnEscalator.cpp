#include "StdInc.h"

#include "TaskSimpleOnEscalator.h"

void CTaskSimpleOnEscalator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleOnEscalator, 0x85B8DC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x4B8720);
    // TODO: instalar las funciones conforme se reconstruyan.
}
