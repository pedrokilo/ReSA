#include "StdInc.h"

#include "TaskComplexWaitForPed.h"

void CTaskComplexWaitForPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForPed, 0x8706F8, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: no hay funciones reconstruidas todavía.
}
