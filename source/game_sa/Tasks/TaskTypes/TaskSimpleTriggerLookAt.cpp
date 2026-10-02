#include "StdInc.h"

#include "TaskSimpleTriggerLookAt.h"

void CTaskSimpleTriggerLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTriggerLookAt, 0x86E3CC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: no hay funciones reconstruidas todavía (el constructor está declarado pero no definido).
}
