/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekEntityRadiusAngleOffset
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexSeekEntityRadiusAngleOffset.h"

void CTaskComplexSeekEntityRadiusAngleOffset::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityRadiusAngleOffset, 0x0085A388, 11);
    RH_ScopedCategory("Tasks/TaskTypes/SeekEntity");

    RH_ScopedInstall(Constructor, 0x493730);
    RH_ScopedInstall(Destructor, 0x493890);

    RH_ScopedInstall(CreateSubTask, 0x496270);

    RH_ScopedVMTInstall(Clone, 0x495FB0);
    RH_ScopedVMTInstall(GetTaskType, 0x493800);
    RH_ScopedVMTInstall(MakeAbortable, 0x493810);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x496080);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x496600);
    RH_ScopedVMTInstall(ControlSubTask, 0x496760);
}
