/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveCarAndDie
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexLeaveCarAndDie.h"

void CTaskComplexLeaveCarAndDie::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndDie, 0x86dd00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x62F1D0);
    RH_ScopedVMTInstall(GetTaskType, 0x62F240);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642280);
}

// NOTSA
CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie(const CTaskComplexLeaveCarAndDie& o) :
    CTaskComplexLeaveCar{o.m_pTargetVehicle, o.m_nTargetDoor, o.m_nDelayTime, false, true}
{
    m_bDie = true;
}

// 0x642280
CTask* CTaskComplexLeaveCarAndDie::CreateFirstSubTask(CPed* ped) {
    ped->m_nPedState = PEDSTATE_DIE; // Don't use `SetPedState` because it has side-effects
    return CTaskComplexLeaveCar::CreateFirstSubTask(ped);
}
