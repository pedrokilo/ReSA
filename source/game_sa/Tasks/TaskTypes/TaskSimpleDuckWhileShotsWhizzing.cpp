#include "StdInc.h"

#include "TaskSimpleDuckWhileShotsWhizzing.h"

void CTaskSimpleDuckWhileShotsWhizzing::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDuckWhileShotsWhizzing, 0x870BEC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x6926E0);
    // TODO: instalar las funciones conforme se reconstruyan.
}
