#include "StdInc.h"

#include "TaskInteriorSitInChair.h"

void CTaskInteriorSitInChair::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorSitInChair, 0x870314, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    // TODO: instalar las funciones conforme se reconstruyan.
}
