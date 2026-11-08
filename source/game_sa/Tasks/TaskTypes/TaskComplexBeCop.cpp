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
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4994C0);

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

// 0x4994C0 (lote 125A, banco Fn125A) : destructor con borrado de CTaskComplexBeCop del EXE (ranura 0 de su vtable 0x85A428; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4994E0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x4994E0; aqui su envoltura Destructor()), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. 
CTaskComplexBeCop* CTaskComplexBeCop::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
