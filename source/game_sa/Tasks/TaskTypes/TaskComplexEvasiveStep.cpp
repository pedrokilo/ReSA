/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEvasiveStep
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

#include "TaskComplexEvasiveStep.h"
#include "TaskSimpleEvasiveStep.h"
#include "TaskSimpleAchieveHeading.h"

void CTaskComplexEvasiveStep::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEvasiveStep, 0x86F218, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6532D0);
    RH_ScopedInstall(Destructor, 0x653350);

    RH_ScopedInstall(CreateSubTask, 0x6533E0);

    RH_ScopedVMTInstall(Clone, 0x655870);
    RH_ScopedVMTInstall(GetTaskType, 0x653340);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6534F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x653540);
    RH_ScopedVMTInstall(ControlSubTask, 0x653550);
}

// 0x6532D0
CTaskComplexEvasiveStep::CTaskComplexEvasiveStep(CEntity* entity, const CVector& pos) : CTaskComplex() {
    m_Pos = pos;
    m_Entity = entity;
    CEntity::SafeRegisterRef(m_Entity);
}

// 0x653350
CTaskComplexEvasiveStep::~CTaskComplexEvasiveStep() {
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x6533E0
CTask* CTaskComplexEvasiveStep::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_EVASIVE_STEP:
        return new CTaskSimpleEvasiveStep(m_Entity);
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        // Como el original: tolerancia 0.2f (0x86FC80)
        const auto angle = CGeneral::GetRadianAngleBetweenPoints(-m_Pos.x, -m_Pos.y, 0.0f, 0.0f);
        return new CTaskSimpleAchieveHeading(angle, 2.0f, 0.2f);
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x6534F0
CTask* CTaskComplexEvasiveStep::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_EVASIVE_STEP:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_EVASIVE_STEP);
    default:
        return nullptr;
    }
}

// 0x653540
CTask* CTaskComplexEvasiveStep::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
}
