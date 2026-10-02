#include "StdInc.h"

#include "TaskComplexUseEntryExit.h"

void CTaskComplexUseEntryExit::InjectHooks() {
    // TODO: vtable sin localizar en gta_sa.exe.
    // RH_ScopedVirtualClass(CTaskComplexUseEntryExit, 0x0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: instalar las funciones conforme se reconstruyan.
}
