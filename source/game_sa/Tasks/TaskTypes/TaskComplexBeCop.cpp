/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexBeCop
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexBeCop.h"

void CTaskComplexBeCop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeCop, 0x85A428, 15); // vtable verificado en gta_sa.exe (GetTaskType en [4]); [11]..[14] heredadas de CTaskComplexWander
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Destructor, 0x4994E0);

    RH_ScopedVMTInstall(GetTaskType, 0x499430);
    RH_ScopedVMTInstall(Clone, 0x499440);
}

// 0x499440
CTask* CTaskComplexBeCop::Clone() const {
    // Como el original: primero clona la subtarea (solo si no es nula, llamada virtual [vtable+4]) y despues
    // reserva la tarea (CTask::operator new, 0x50 bytes); m_nDir se pasa como byte (movzx)
    CTask* const clonedTask = m_pGoToPointAndStandStillTask ? m_pGoToPointAndStandStillTask->Clone() : nullptr;
    return new CTaskComplexBeCop(m_nMoveState, m_nDir, clonedTask);
}
