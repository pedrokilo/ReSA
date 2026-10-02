/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderProstitute
 * Reconstrucción: INCOMPLETA — 4/5 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   ScanForStuff | 0x00672700 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ScanForStuff | 0x00672700 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderProstitute.h"

void CTaskComplexWanderProstitute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderProstitute, 0x870148, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x672690);

    RH_ScopedVMTInstall(Clone, 0x673C80);
    RH_ScopedVMTInstall(GetWanderType, 0x6726C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x674920);
    RH_ScopedVMTInstall(ScanForStuff, 0x672700, { .reversed = false });
}

// 0x672690
CTaskComplexWanderProstitute::CTaskComplexWanderProstitute(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWanderStandard(MoveState, Dir, bWanderSensibly),
    m_nStartTimeInMs{ 0 }
{
}

// 0x672700
void CTaskComplexWanderProstitute::ScanForStuff(CPed* ped) {
    plugin::CallMethod<0x672700, CTaskComplexWanderProstitute*, CPed*>(this, ped);
}
