/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekEntityStandard
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexSeekEntityStandard.h"

void CTaskComplexSeekEntityStandard::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityStandard, 0x00859DF8, 11);
    RH_ScopedCategory("Tasks/TaskTypes/SeekEntity");

    RH_ScopedInstall(Constructor, 0x46AC10);
    RH_ScopedInstall(Destructor, 0x46AD70); // 0x46AD50 is the scalar deleting wrapper (ret 4).

    RH_ScopedInstall(CreateSubTask, 0x46F1E0);

    RH_ScopedVMTInstall(Clone, 0x46EF50);
    RH_ScopedVMTInstall(GetTaskType, 0x46ACE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x46ACF0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x46EFF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x46F6A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x46F800);
}
