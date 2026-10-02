/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedTaskPair
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedTaskPair.h"

void CPedTaskPair::InjectHooks() {
    RH_ScopedClass(CPedTaskPair);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Flush, 0x5E95B0);
}

// 0x5E95B0
void CPedTaskPair::Flush() {
    Ped = nullptr;
    delete Task;
    Task = nullptr;
    Slot = TASK_SECONDARY_INVALID;
}
