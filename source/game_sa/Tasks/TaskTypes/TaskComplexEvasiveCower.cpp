/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEvasiveCower
 * Reconstrucción: INCOMPLETA — 7/8 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Validación registrada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   CreateSubTask | 0x00655570 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   CreateSubTask | 0x00655570 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexEvasiveCower.h"

void CTaskComplexEvasiveCower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEvasiveCower, 0x86F3BC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655460);
    RH_ScopedInstall(Destructor, 0x6554E0);

    RH_ScopedInstall(CreateSubTask, 0x655570, { .reversed = false }); // Stub plugin::Call: activarlo provocaría recursión infinita

    RH_ScopedVMTInstall(Clone, 0x655CD0);
    RH_ScopedVMTInstall(GetTaskType, 0x6554D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6556A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6556F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x655700);
}

// 0x655460
CTaskComplexEvasiveCower::CTaskComplexEvasiveCower(CEntity* entity, const CVector& pos) :
    m_Pos{pos},
    m_Entity{entity}
{
    CEntity::SafeRegisterRef(m_Entity);
}

// 0x6554E0
CTaskComplexEvasiveCower::~CTaskComplexEvasiveCower() {
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x6556A0
CTask* CTaskComplexEvasiveCower::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_COWER:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_COWER);
    default:
        return nullptr;
    }
}

// 0x6556F0
CTask* CTaskComplexEvasiveCower::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}

// 0x655570
CTask* CTaskComplexEvasiveCower::CreateSubTask(eTaskType taskType) {
    return plugin::CallMethodAndReturn<CTask*, 0x655570, CTaskComplexEvasiveCower*, eTaskType>(this, taskType);
}
