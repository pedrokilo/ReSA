/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointAiming
 * Reconstrucción: INCOMPLETA — 5/8 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Validación registrada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateSubTask | 0x006688D0 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateSubTask | 0x006688D0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToPointAiming.h"

void CTaskComplexGoToPointAiming::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAiming, 0x86fe00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668790);
    RH_ScopedInstall(Destructor, 0x668870);
    RH_ScopedInstall(CreateSubTask, 0x6688D0, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x66CD80);
    RH_ScopedVMTInstall(GetTaskType, 0x668860);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DD70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66DDB0, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x6689E0, { .reversed = false });
}

// 0x668790
CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(
    eMoveState moveState,
    const CVector& movePos,
    CEntity* aimAtEntity,
    CVector aimPos,
    const float targetRadius,
    const float slowDownDist
) :
    m_aimPos{aimPos},
    m_aimAtEntity{aimAtEntity},
    m_moveState{moveState}
{
    CEntity::SafeRegisterRef(m_aimAtEntity);
    // The original updates the destination in-place, preserving the flag when
    // the pool slot already contains exactly the same destination and distances.
    if (m_movePos != movePos || m_moveTargetRadius != targetRadius || m_slowDownDistance != slowDownDist) {
        m_movePos = movePos;
        m_moveTargetRadius = targetRadius;
        m_slowDownDistance = slowDownDist;
        m_newTargetSet = true;
    }
}

CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(const CTaskComplexGoToPointAiming& o) :
    CTaskComplexGoToPointAiming{
        o.m_moveState,
        o.m_movePos,
        o.m_aimAtEntity,
        o.m_aimPos,
        o.m_moveTargetRadius,
        o.m_slowDownDistance
    }
{
}

// 0x668870
CTaskComplexGoToPointAiming::~CTaskComplexGoToPointAiming() {
    CEntity::SafeCleanUpRef(m_aimAtEntity);
}

// 0x6688D0
CTask* CTaskComplexGoToPointAiming::CreateSubTask(eTaskType taskType) {
    return plugin::CallMethodAndReturn<CTask*, 0x6688D0, CTaskComplexGoToPointAiming*, int32>(this, taskType);
}

// 0x66DD70
CTask* CTaskComplexGoToPointAiming::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
    case TASK_SIMPLE_GUN_CTRL:
        return CreateSubTask(TASK_FINISHED);
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x66DDB0
CTask* CTaskComplexGoToPointAiming::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x66DDB0, CTaskComplexGoToPointAiming*, CPed*>(this, ped);
}

// 0x6689E0
CTask* CTaskComplexGoToPointAiming::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6689E0, CTaskComplexGoToPointAiming*, CPed*>(this, ped);
}
