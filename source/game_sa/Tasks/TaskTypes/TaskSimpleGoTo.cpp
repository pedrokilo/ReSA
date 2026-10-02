/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGoTo
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGoTo.h"

#include "Ragdoll/IKChainManager.h"

void CTaskSimpleGoTo::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGoTo, 0x86fd1c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6679C0);

    RH_ScopedInstall(HasCircledTarget, 0x667A10);
    RH_ScopedInstall(SetUpIK, 0x667AD0);
    RH_ScopedInstall(QuitIK, 0x667CA0);
}

CTaskSimpleGoTo::CTaskSimpleGoTo(eMoveState moveState, const CVector& targetPoint, float fRadius) {
    m_moveState = moveState;
    m_vecTargetPoint = targetPoint;
    m_fRadius = fRadius;
    m_GoToFlags = 0;
}

// 0x667A10
bool CTaskSimpleGoTo::HasCircledTarget(CPed* ped) 
{
    const CVector& pedPos = ped->GetPosition();
    if (!ped->bIgnoreHeightCheckOnGotoPointTask && fabs(pedPos.z - m_vecTargetPoint.z) >= 2.0f)
        return false;

    if (m_vecTargetPoint.x > pedPos.x)
        gotoFlags.m_targetCircledFlags |= 1;
    else if (m_vecTargetPoint.x < pedPos.x)
        gotoFlags.m_targetCircledFlags |= 1 << 1;

    if (m_vecTargetPoint.y > pedPos.y)
        gotoFlags.m_targetCircledFlags |= 1 << 2;
    else if (m_vecTargetPoint.y < pedPos.y) 
        gotoFlags.m_targetCircledFlags |= 1 << 3;

    return gotoFlags.m_targetCircledFlags == 0b1111;
}

// 0x667AD0
void CTaskSimpleGoTo::SetUpIK(CPed* ped)
{
    if (ped->GetIsOnScreen() && !gotoFlags.m_bIsIKChainSet
        && !g_ikChainMan.GetLookAtEntity(ped)
        && !ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK)
        && (ped != FindPlayerPed() || CPad::GetPad(0)->DisablePlayerControls)) {
        if (!m_Parent || m_Parent->GetTaskType() != TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING && m_Parent->GetTaskType() != TASK_COMPLEX_AVOID_ENTITY) {
            CVector vecDistance = m_vecTargetPoint - ped->GetPosition();
            if (vecDistance.SquaredMagnitude() > 9.0f) {
                CVector direction(vecDistance);
                direction.Normalise();
                if (DotProduct(&direction, &ped->GetForward()) < ms_fLookAtThresholdDotProduct) {
                    CVector position = direction + direction;
                    position.x += m_vecTargetPoint.x;
                    position.y += m_vecTargetPoint.y;
                    position.z += ped->GetPosition().z + 0.61f;
                    g_ikChainMan.LookAt("TaskGoTo", ped, nullptr, 5000, BONE_UNKNOWN, &position, false, 0.25f, 500, 3, false);
                    gotoFlags.m_bIsIKChainSet = true;
                }
            }
        }
    }
}

// 0x667CA0
void CTaskSimpleGoTo::QuitIK(CPed* ped)
{
    if (gotoFlags.m_bIsIKChainSet && g_ikChainMan.IsLooking(ped))
        g_ikChainMan.AbortLookAt(ped, 250);
}
