#include "StdInc.h"

#include "TaskComplexOpenPassengerDoor.h"

void CTaskComplexOpenPassengerDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenPassengerDoor, 0x86EB3C, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6403E0);
    RH_ScopedInstall(Destructor, 0x640420);

    // El original (0x6438E0) también copia m_MoveState a la tarea nueva; el Clone inline no lo hace.
    RH_ScopedVMTInstall(Clone, 0x6438E0, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x640410);
}
