/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDrownInCar
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

#include "TaskSimpleDrownInCar.h"

void CTaskSimpleDrownInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrownInCar, 0x86DE54, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedVMTInstall(ProcessPed, 0x62FF70);
}

// 0x62FF70

// 0x0
bool CTaskSimpleDrownInCar::ProcessPed(CPed* ped) {
    ped->SetPedState(PEDSTATE_DIE);
    ped->bIsInTheAir = false;

    if (ped == FindPlayerPed()) {
        CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
        ped->m_pVehicle->SetStatus(STATUS_FORCED_STOP);
    }

    CEventDeath event(true);
    ped->GetEventGroup().Add(&event, false);
    return true;
}
