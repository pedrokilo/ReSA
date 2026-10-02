#include "StdInc.h"

#include "TaskComplexFallAndStayDown.h"

void CTaskComplexFallAndStayDown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFallAndStayDown, 0x870480, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x678A00);
    // TODO: instalar las funciones conforme se reconstruyan.
}
