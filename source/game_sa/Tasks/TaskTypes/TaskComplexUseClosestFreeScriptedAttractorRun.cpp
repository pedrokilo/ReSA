/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseClosestFreeScriptedAttractorRun
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractorRun.h"

void CTaskComplexUseClosestFreeScriptedAttractorRun::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractorRun, 0x85b904, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8790);
    RH_ScopedInstall(Destructor, 0x4B8840);

    RH_ScopedVMTInstall(Clone, 0x4B87B0);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8810);
}

// 0x4B8790
CTaskComplexUseClosestFreeScriptedAttractorRun::CTaskComplexUseClosestFreeScriptedAttractorRun() :
    CTaskComplexUseClosestFreeScriptedAttractor{PEDMOVE_RUN}
{
}
