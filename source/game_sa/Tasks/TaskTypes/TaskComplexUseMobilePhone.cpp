#include "StdInc.h"

#include "TaskComplexUseMobilePhone.h"

void CTaskComplexUseMobilePhone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseMobilePhone, 0x86E454, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    // TODO: no hay funciones reconstruidas todavía (el constructor está declarado pero no definido).
}
