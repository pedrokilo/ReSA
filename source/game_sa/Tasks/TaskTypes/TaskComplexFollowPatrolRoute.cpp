#include "StdInc.h"

#include "TaskComplexFollowPatrolRoute.h"

void CTaskComplexFollowPatrolRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPatrolRoute, 0x870114, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x670A60);
    // TODO: instalar las funciones conforme se reconstruyan.
}
