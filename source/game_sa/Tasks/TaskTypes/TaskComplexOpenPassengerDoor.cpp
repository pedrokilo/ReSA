/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexOpenPassengerDoor
 * Reconstrucción: INCOMPLETA — 3/4 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x006438E0 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x006438E0 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexOpenPassengerDoor.h"

void CTaskComplexOpenPassengerDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenPassengerDoor, 0x86EB3C, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6403E0);
    RH_ScopedInstall(Destructor, 0x640420);

    // El original (0x6438E0) también copia m_MoveState a la tarea nueva; el Clone inline no lo hace.
    RH_ScopedVMTInstall(Clone, 0x6438E0, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x640410);
}
