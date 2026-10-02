#include "StdInc.h"

#include "TaskComplexKillAllThreats.h"

void CTaskComplexKillAllThreats::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillAllThreats, 0x86DA1C, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x6221B0);
    // TODO: instalar las funciones conforme se reconstruyan.
}
