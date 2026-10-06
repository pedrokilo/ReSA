/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexOpenPassengerDoor
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

#include "TaskComplexOpenPassengerDoor.h"

void CTaskComplexOpenPassengerDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenPassengerDoor, 0x86EB3C, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6403E0);
    RH_ScopedInstall(Destructor, 0x640420);

    RH_ScopedVMTInstall(Clone, 0x6438E0);
    RH_ScopedVMTInstall(GetTaskType, 0x640410);
}

// 0x6438E0
CTask* CTaskComplexOpenPassengerDoor::Clone() const {
    // Como el original: tarea nueva con (vehículo, asiento) y copia del estado de movimiento.
    auto* task = new CTaskComplexOpenPassengerDoor(m_Car, m_TargetSeat);
    task->m_MoveState = m_MoveState;
    return task;
}
