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
 * Validación registrada: 0/8 entradas.
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

    RH_ScopedGlobalInstall(CreateSubTask, 0x655390);

    RH_ScopedVMTInstall(Clone, 0x655C80);
    RH_ScopedVMTInstall(GetTaskType, 0x655360);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657770);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6577C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x655380);
}

// 0x655390
CTask* CTaskComplexMoveBackAndJump::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_JUMP:
        return new CTaskComplexJump{};
    case TASK_SIMPLE_HIT_WALL:
        return new CTaskSimpleHitWall{};
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
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
        default:
            NOTSA_UNREACHABLE();
        }
    }());
}

// 0x6577C0
CTask* CTaskComplexMoveBackAndJump::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_HIT_WALL);
}
