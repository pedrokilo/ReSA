/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitUntilLeaderAreaCodesMatch
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleWaitUntilLeaderAreaCodesMatch.h"

void CTaskSimpleWaitUntilLeaderAreaCodesMatch::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilLeaderAreaCodesMatch, 0x86c7e8, 9);
    RH_ScopedCategory("Tasks/TaskTypes"); 

    RH_ScopedVMTInstall(Clone, 0x5F67F0);
    RH_ScopedVMTInstall(GetTaskType, 0x5F6870);
    RH_ScopedVMTInstall(MakeAbortable, 0x6356A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6356C0);
}

// 0x6356A0
bool CTaskSimpleWaitUntilLeaderAreaCodesMatch::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    } else {
        ped->SetUsesCollision(true);
        return true;
    }
}

// 0x6356C0
bool CTaskSimpleWaitUntilLeaderAreaCodesMatch::ProcessPed(CPed* ped) {
    if (CTaskSimpleWaitUntilAreaCodesMatch::ProcessPed(ped)) {
        ped->SetUsesCollision(true);
        return true;
    }
    if (!m_b) {
        if (ped->GetTaskManager().Find<Type>()) {
            m_b = true;
            ped->SetUsesCollision(false);
        }
    }
    return false;
}
