#include "StdInc.h"

#include "TaskSimpleHailTaxi.h"

void CTaskSimpleHailTaxi::InjectHooks() {
    // TODO: vtable sin localizar en gta_sa.exe.
    // RH_ScopedVirtualClass(CTaskSimpleHailTaxi, 0x0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: instalar las funciones conforme se reconstruyan.
}
