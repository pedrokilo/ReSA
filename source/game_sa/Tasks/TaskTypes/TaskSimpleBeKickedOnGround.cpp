#include "StdInc.h"

#include "TaskSimpleBeKickedOnGround.h"

void CTaskSimpleBeKickedOnGround::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleBeKickedOnGround, 0x86D7D8, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x61FC40);
    // TODO: instalar las funciones conforme se reconstruyan.
}
