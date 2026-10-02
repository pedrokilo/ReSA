#include "StdInc.h"

#include "TaskSimpleDetonate.h"

void CTaskSimpleDetonate::InjectHooks() {
    // TODO: vtable sin localizar en gta_sa.exe.
    // RH_ScopedVirtualClass(CTaskSimpleDetonate, 0x0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: instalar las funciones conforme se reconstruyan.
}
