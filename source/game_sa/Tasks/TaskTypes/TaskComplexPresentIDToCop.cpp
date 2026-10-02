#include "StdInc.h"

#include "TaskComplexPresentIDToCop.h"

void CTaskComplexPresentIDToCop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPresentIDToCop, 0x86F544, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x659230);
    // TODO: instalar las funciones conforme se reconstruyan.
}
