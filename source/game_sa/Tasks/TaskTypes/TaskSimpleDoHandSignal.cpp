/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDoHandSignal
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskComplexPlayHandSignalAnim.h"

void CTaskSimpleDoHandSignal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDoHandSignal, 0x86fa68, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660880);
    RH_ScopedInstall(Destructor, 0x6608B0);

    RH_ScopedVMTInstall(Clone, 0x6608C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6608A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x660930);
    RH_ScopedVMTInstall(ProcessPed, 0x660940);
}

CTaskSimpleDoHandSignal::CTaskSimpleDoHandSignal(const CTaskSimpleDoHandSignal& o) :
    m_Initialized{ o.m_Initialized }
{
}

// 0x660940
bool CTaskSimpleDoHandSignal::ProcessPed(CPed* ped) {
    if (!ped->GetIsOnScreen()) {
        return true;
    }

    const auto animTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);

    if (m_Initialized) {
        return !animTask || !notsa::isa<CTaskComplexPlayHandSignalAnim>(animTask);
    }

    if (animTask) {
        if (notsa::isa<CTaskComplexPlayHandSignalAnim>(animTask)) {
            return true;
        }
        animTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        return false;
    }

    ped->GetTaskManager().SetTaskSecondary(
        new CTaskComplexPlayHandSignalAnim{},
        TASK_SECONDARY_PARTIAL_ANIM
    );
    m_Initialized = true;
    return false;
}
