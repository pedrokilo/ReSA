/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCrossRoadLookAndAchieveHeading
 * Reconstrucción: INCOMPLETA — 8/9 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Validación registrada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Constructor | 0x00631AE0 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Constructor | 0x00631AE0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleAchieveHeading.h"

void CTaskComplexCrossRoadLookAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCrossRoadLookAndAchieveHeading, 0x86DFF0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    // El constructor todavía es un stub que llama al original.
    RH_ScopedInstall(Constructor, 0x631AE0, { .reversed = false });
    RH_ScopedInstall(Destructor, 0x631B50);
    RH_ScopedInstall(CreateSubTask, 0x631B60);

    RH_ScopedVMTInstall(Clone, 0x636500);
    RH_ScopedVMTInstall(GetTaskType, 0x631B10);
    RH_ScopedVMTInstall(MakeAbortable, 0x631B20);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631C40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631C90);
    RH_ScopedVMTInstall(ControlSubTask, 0x631CB0);
}

// 0x631AE0
CTaskComplexCrossRoadLookAndAchieveHeading::CTaskComplexCrossRoadLookAndAchieveHeading(int32 durationInMs, float fTargetHeading) : CTaskComplex() {
    plugin::CallMethodAndReturn<CTaskComplexCrossRoadLookAndAchieveHeading*, 0x631AE0, CTaskComplexCrossRoadLookAndAchieveHeading*, int32, float>(this, durationInMs, fTargetHeading);
}

// 0x631B50
CTaskComplexCrossRoadLookAndAchieveHeading::~CTaskComplexCrossRoadLookAndAchieveHeading() {
    // NOP
}

// 0x631C40
CTask* CTaskComplexCrossRoadLookAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ANIM:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_ANIM, ped);
    default:
        return nullptr;
    }
}

// 0x631B60
CTask* CTaskComplexCrossRoadLookAndAchieveHeading::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ANIM:
        return new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0f, false);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(m_fTargetHeading, 0.5f, 0.2f);
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x631B20
bool CTaskComplexCrossRoadLookAndAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE)
        return m_pSubTask->MakeAbortable(ped, priority, event);

    return false;
}

CTaskComplexCrossRoadLookAndAchieveHeading* CTaskComplexCrossRoadLookAndAchieveHeading::Constructor(int32 durationInMs, float fTargetHeading) {
    this->CTaskComplexCrossRoadLookAndAchieveHeading::CTaskComplexCrossRoadLookAndAchieveHeading(durationInMs, fTargetHeading);
    return this;
}
