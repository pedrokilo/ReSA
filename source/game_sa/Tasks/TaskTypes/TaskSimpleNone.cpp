/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleNone
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleNone.h"

void CTaskSimpleNone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleNone, 0x859CFC, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    // Destructor 0x463560: el EXE comparte esta función con CTaskSimpleLeaveGroup (plegado
    // de funciones idénticas) y ya se instala allí; instalarla dos veces dispara el assert
    // de dirección duplicada en ReversibleHooks.

    RH_ScopedVMTInstall(Clone, 0x463570);
    RH_ScopedVMTInstall(GetTaskType, 0x4635C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x4635D0);
    RH_ScopedVMTInstall(ProcessPed, 0x4635E0);
}
