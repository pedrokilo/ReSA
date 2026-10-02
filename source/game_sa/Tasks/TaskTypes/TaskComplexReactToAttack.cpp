#include "StdInc.h"

#include "TaskComplexReactToAttack.h"

void CTaskComplexReactToAttack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexReactToAttack, 0x86D868, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x620B80);
    // TODO: instalar las funciones conforme se reconstruyan.
}
