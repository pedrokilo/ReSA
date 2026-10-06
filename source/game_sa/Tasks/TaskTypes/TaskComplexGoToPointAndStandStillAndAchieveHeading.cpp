/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointAndStandStillAndAchieveHeading
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGoToPoint.h"

void CTaskComplexGoToPointAndStandStillAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStillAndAchieveHeading, 0x86FE58, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668CD0);
    RH_ScopedInstall(Destructor, 0x668D40);

    RH_ScopedVMTInstall(Clone, 0x66CFD0);
    RH_ScopedVMTInstall(GetTaskType, 0x668D30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DFD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66E030);
    RH_ScopedVMTInstall(ControlSubTask, 0x668E80);
}

// 0x668CD0
CTaskComplexGoToPointAndStandStillAndAchieveHeading::CTaskComplexGoToPointAndStandStillAndAchieveHeading(eMoveState moveState, const CVector& targetPos, float angle, float radius, float changeRateMult, float maxHeading)
    : CTaskComplex()
{
    m_MoveState = moveState;
    m_TargetPos = targetPos;
    m_Radius = radius;
    m_MaxHeading = maxHeading;
    m_nFlags = m_nFlags | 1; // todo: flags
    m_Angle = angle;
    m_ChangeRateMult = changeRateMult;
}

// 0x66DFD0
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT:
        return CreateSubTask(TASK_SIMPLE_STAND_STILL);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
    default:
        return nullptr;
    }
}

// 0x668D50
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(m_Angle, m_ChangeRateMult, m_MaxHeading);
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{};
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint(m_MoveState, m_TargetPos, m_Radius, false, false);
    default:
        return nullptr;
    }
}

// 0x66E030
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    // Como el original: apaga el bit 0 de las banderas y crea el desplazamiento
    m_nFlags &= 0xFE; // todo: flags
    return CreateSubTask(TASK_SIMPLE_GO_TO_POINT);
}

// 0x668E80
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::ControlSubTask(CPed* ped) {
    // Como el original: con el bit 0 pendiente se crea la primera subtarea (hueco 9 de la vtable)
    if (m_nFlags & 1) { // todo: flags
        return CreateFirstSubTask(ped);
    }
    // Como el original: la subtarea de desplazamiento sigue el estado de movimiento de esta tarea
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
        static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->m_moveState = m_MoveState;
    }
    return m_pSubTask;
}
