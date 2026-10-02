#include "StdInc.h"

#include "TaskSimpleBeHitWhileMoving.h"

void CTaskSimpleBeHitWhileMoving::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleBeHitWhileMoving, 0x86D7FC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x61FE60);
    // TODO: instalar las funciones conforme se reconstruyan.
}
