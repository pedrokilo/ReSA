/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexMoveBackAndJump
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexMoveBackAndJump.h"
#include "TaskComplexJump.h"
#include "TaskSimpleHitWall.h"

void CTaskComplexMoveBackAndJump::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexMoveBackAndJump, 0x86f390, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655340);
    RH_ScopedInstall(Destructor, 0x655370);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x657750); // lote 182A (sin validar)

    RH_ScopedInstall(CreateSubTask, 0x655390);

    RH_ScopedVMTInstall(Clone, 0x655C80);
    RH_ScopedVMTInstall(GetTaskType, 0x655360);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657770);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6577C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x655380);
}

// 0x657750 (lote 182A, sin validar; el listado decia CTaskComplexWalkRoundBuildingAttempt): destructor con borrado de CTaskComplexMoveBackAndJump del EXE (ranura 0 de su vtable 0x86F390; thiscall, un argumento de pila, `ret 4`,
// 30 bytes: `push esi; mov esi,ecx; call 0x655370; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x655370, con hook), libera
// `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexMoveBackAndJump* CTaskComplexMoveBackAndJump::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x655390
CTask* CTaskComplexMoveBackAndJump::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_JUMP:
        return new CTaskComplexJump{};
    case TASK_SIMPLE_HIT_WALL:
        return new CTaskSimpleHitWall{};
    default: // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nullptr
        return nullptr;
    }
}

// 0x657770
CTask* CTaskComplexMoveBackAndJump::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_HIT_WALL:
            return TASK_COMPLEX_JUMP;
        case TASK_COMPLEX_JUMP:
            return TASK_FINISHED;
        default: // Como el original: subtarea desconocida -> nullptr
            return TASK_NONE;
        }
    }());
}

// 0x6577C0
CTask* CTaskComplexMoveBackAndJump::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_HIT_WALL);
}
