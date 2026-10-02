#include "StdInc.h"

#include "TaskComplexAvoidEntity.h"

void CTaskComplexAvoidEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexAvoidEntity, 0x86FF00, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]; tras [10] empieza una cadena)
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x66AAD0);
    // TODO: instalar las funciones conforme se reconstruyan.
}
