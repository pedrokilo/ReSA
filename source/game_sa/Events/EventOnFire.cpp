/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventOnFire
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventOnFire.h"


void CEventOnFire::InjectHooks() {
    RH_ScopedVirtualClass(CEventOnFire, 0x86CCC0, 16);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(AffectsPed, 0x4B1050);
}

// 0x4B1050
bool CEventOnFire::AffectsPed(CPed* ped) {
    if (ped->m_pFire && !ped->physicalFlags.bFireProof) {
        CTask* activeTask = ped->GetTaskManager().GetActiveTask();
        if (activeTask && activeTask->GetTaskType() == TASK_COMPLEX_ON_FIRE)
            return false;

        CTask* partialAnimTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
        if (partialAnimTask && partialAnimTask->GetTaskType() == TASK_SIMPLE_PLAYER_ON_FIRE)
            return false;

        return ped->IsAlive();
    }
    return false;
}