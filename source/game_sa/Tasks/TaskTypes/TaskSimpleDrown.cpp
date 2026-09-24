/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDrown
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

#include "TaskSimpleDrown.h"
#include "TaskSimpleDie.h"

void CTaskSimpleDrown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrown, 0x86DE30, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedVMTInstall(ProcessPed, 0x639900);
}

// 0x62FEC0
CTaskSimpleDrown::CTaskSimpleDrown() : CTaskSimpleDie(ANIM_GROUP_DEFAULT, ANIM_ID_DROWN, 4.0f, 0.0f) {
    m_bShouldExecute = true;
}

// 0x639900

// 0x0
bool CTaskSimpleDrown::ProcessPed(CPed* ped) {
    if (m_bShouldExecute) {
        ped->SetPedState(PEDSTATE_DIE);
        ped->bIsInTheAir = false; 
        if (ped == FindPlayerPed()) {
            CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
        }
        m_bShouldExecute = false;
    }
    return CTaskSimpleDie::ProcessPed(ped);
}