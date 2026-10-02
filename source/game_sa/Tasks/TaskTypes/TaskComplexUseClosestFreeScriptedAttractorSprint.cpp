/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseClosestFreeScriptedAttractorSprint
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractorSprint.h"

void CTaskComplexUseClosestFreeScriptedAttractorSprint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractorSprint, 0x85b930, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8850);
    RH_ScopedInstall(Destructor, 0x4B8900);

    RH_ScopedVMTInstall(Clone, 0x4B8870);
    RH_ScopedVMTInstall(GetTaskType, 0x4B88D0);
}

// 0x4B8850
CTaskComplexUseClosestFreeScriptedAttractorSprint::CTaskComplexUseClosestFreeScriptedAttractorSprint() :
    CTaskComplexUseClosestFreeScriptedAttractor{PEDMOVE_SPRINT}
{
}

// 0x4B8870
CTask* CTaskComplexUseClosestFreeScriptedAttractorSprint::Clone() const {
    // A fresh sprint task, independent of the source's subtask and move state.
    return new CTaskComplexUseClosestFreeScriptedAttractorSprint{};
}

// 0x4B88D0
eTaskType CTaskComplexUseClosestFreeScriptedAttractorSprint::GetTaskType() const {
    return Type;
}
