#include "StdInc.h"

#include "TaskSimpleTriggerEvent.h"

void CTaskSimpleTriggerEvent::InjectHooks() {
    // TODO: vtable sin localizar en gta_sa.exe.
    // RH_ScopedVirtualClass(CTaskSimpleTriggerEvent, 0x0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: instalar las funciones conforme se reconstruyan.
}
