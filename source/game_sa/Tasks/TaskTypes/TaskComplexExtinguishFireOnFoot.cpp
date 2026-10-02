#include "StdInc.h"

#include "TaskComplexExtinguishFireOnFoot.h"

void CTaskComplexExtinguishFireOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexExtinguishFireOnFoot, 0x86F5FC, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x6598A0);
    // TODO: instalar las funciones conforme se reconstruyan.
}
