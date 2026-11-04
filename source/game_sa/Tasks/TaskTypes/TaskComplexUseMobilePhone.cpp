#include "StdInc.h"

#include "TaskComplexUseMobilePhone.h"

void CTaskComplexUseMobilePhone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseMobilePhone, 0x86E454, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Quit, 0x634A40);
    // El constructor esta declarado pero no definido: no hay mas funciones reconstruidas todavia.
}

// 0x634A40
void CTaskComplexUseMobilePhone::Quit(CPed* ped) {
    // Como el original: el byte +0x1D (m_bQuit) se prueba entero y se marca antes de abortar; la ranura 6 de la vtable es MakeAbortable (prioridad 0 = leisure, sin evento)
    if (!m_bQuit) {
        m_bQuit = true;
        MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }
}
