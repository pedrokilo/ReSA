/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFinishBrain
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleFinishBrain.h"

void CTaskSimpleFinishBrain::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFinishBrain, 0x859d20, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x463660);
    RH_ScopedVMTInstall(GetTaskType, 0x4636B0);
}

// 0x463660
CTaskSimpleFinishBrain::CTaskSimpleFinishBrain(const CTaskSimpleFinishBrain&) :
    CTaskSimpleFinishBrain{/* nothing to copy */}
{
}
