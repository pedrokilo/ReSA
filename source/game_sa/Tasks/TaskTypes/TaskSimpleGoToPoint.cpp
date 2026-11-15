/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGoToPoint
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGoToPoint.h"

#include "TaskSimpleStandStill.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"

namespace CTaskSimpleGoToBody {
void __fastcall GoToDestructorBody(CTaskSimpleGoTo* self, void* edx); // TaskSimpleGoTo.cpp: 0x667D50 / 0x667A00
}

void CTaskSimpleGoToPoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGoToPoint, 0x86fd50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x667CD0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x66D6F0); // lote 195A (sin validar; el listado lo atribuia a CTaskComplexInvestigateDisturbance)

    RH_ScopedInstall(UpdatePoint, 0x645700);
    RH_ScopedVMTInstall(Clone, 0x66CC60);
    RH_ScopedVMTInstall(GetTaskType, 0x667D40);
    RH_ScopedVMTInstall(MakeAbortable, 0x667D60);
    RH_ScopedVMTInstall(ProcessPed, 0x66D710);
}

// 0x66D6F0 (lote 195A, sin validar): destructor con borrado de CTaskSimpleGoToPoint del EXE (ranura 0 de su vtable 0x86FD50; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x667D50; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo 0x667D50 es el mismo de CTaskSimpleGoTo
// (deja la vtable 0x86FD1C y sigue con CTask::Destructor; la clase no escribe nada propio). Libera `this` con CTask::operator delete (0x61A5B0) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleGoToPoint* CTaskSimpleGoToPoint::ScalarDeletingDestructor(uint8 flags) {
    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x667CD0
CTaskSimpleGoToPoint::CTaskSimpleGoToPoint(eMoveState moveState, const CVector& targetPoint, float fRadius, bool bMoveTowardsTargetPoint, bool a6) :
    CTaskSimpleGoTo(moveState, targetPoint, fRadius)
{
    m_GoToPointFlags &= 0xE0; // Como el original: and dl,0E0h (conserva los bits 5-7)
    gotoPointFlags.m_bMoveTowardsTargetPoint = bMoveTowardsTargetPoint;
    gotoPointFlags.m_b04 = a6;
}

// 0x667D60
bool CTaskSimpleGoToPoint::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (gotoFlags.m_bIsIKChainSet) {
        if (g_ikChainMan.IsLooking(ped))
            g_ikChainMan.AbortLookAt(ped, 250);
    }
    if (priority != ABORT_PRIORITY_URGENT) {
        if (priority != ABORT_PRIORITY_IMMEDIATE) {
            gotoPointFlags.m_b02 = true;
            return false;
        }
        CTaskSimpleStandStill standStillTask(0, false, false, 8.0f);
        standStillTask.ProcessPed(ped);
    }
    return true;
}

// 0x66D710
bool CTaskSimpleGoToPoint::ProcessPed(CPed* ped) {
    CPlayerPed* player = ped->AsPlayer();
    ped->m_pedIK.bSlopePitch = true;
    if (HasCircledTarget(ped)) {
        if (!gotoPointFlags.m_b05) {
            gotoPointFlags.m_b03 = true;
            if (gotoFlags.m_bIsIKChainSet) {
                if (g_ikChainMan.IsLooking(ped))
                    g_ikChainMan.AbortLookAt(ped, 250);
            }
            return true;
        }
    }
    if (ped->bHasJustLeftCar) {
        ped->GetIntelligence()->GetEventScanner().ScanForEventsNow(*ped, false);
        ped->bHasJustLeftCar = false;
    }
    else if (ped->bHasJustSoughtCover) {
        ped->GetIntelligence()->GetEventScanner().ScanForEventsNow(*ped, true);
        ped->bHasJustSoughtCover = false;
    }
    else {
        // Como el original: la distancia, la diferencia de altura y la direccion se calculan antes de mover al peaton
        const CVector& pedPos = ped->GetPosition();
        const CVector2D vecDistance = m_vecTargetPoint - pedPos;
        const float fHeightDiff = std::fabs(pedPos.z - m_vecTargetPoint.z);
        const float fDistSq = vecDistance.SquaredMagnitude();
        CVector2D vecDirection(vecDistance);
        vecDirection.Normalise();

        CTaskSimpleDuck* pDuckTask = ped->GetIntelligence()->GetTaskDuck(false);
        if (pDuckTask) {
            float fMoveSpeedY = 1.1f;
            if (m_moveState == PEDMOVE_WALK)
                fMoveSpeedY = 0.75f;
            pDuckTask->ControlDuckMove({ 0.0f, -fMoveSpeedY });
        }
        else {
            ped->SetMoveState(static_cast<eMoveState>(m_moveState));
            if (ped->IsPlayer()) {
                if (CPad::GetPad(0)->DisablePlayerControls) {
                    player->SetPlayerMoveBlendRatio(nullptr);
                    player->SetRealMoveAnim();
                }
                else {
                    bool bSprinting = false;
                    CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, eWeaponSkill::STD);
                    if (!pWeaponInfo->flags.bHeavy) {
                        auto* task = static_cast<CTaskSimpleHoldEntity*>(ped->GetIntelligence()->GetTaskHold(false));
                        if (!task || !task->m_pAnimBlendAssociation) {
                            CAnimBlendAssocGroup* animGroup = &CAnimManager::GetAssocGroups()[ped->m_nAnimGroup];
                            if (!ped->GetPlayerData()->m_bPlayerSprintDisabled && !g_surfaceInfos.CantSprintOn(ped->m_nContactSurface)) {
                                auto assoc1 = animGroup->GetAnimation(ANIM_ID_RUN);
                                auto assoc2 = animGroup->GetAnimation(ANIM_ID_SPRINT);
                                if (assoc1->m_BlendHier != assoc2->m_BlendHier && player->ControlButtonSprint(SPRINT_GROUND) >= 1.0f)
                                {
                                    ped->SetMoveState(PEDMOVE_SPRINT);
                                    bSprinting = true;
                                }
                            }
                        }
                    }
                    if (!bSprinting && ped->m_nMoveState == PEDMOVE_SPRINT)
                        ped->SetMoveState(PEDMOVE_RUN);
                    if (gotoPointFlags.m_bMoveTowardsTargetPoint)
                        player->SetPlayerMoveBlendRatio(&m_vecTargetPoint);
                    else
                        player->SetPlayerMoveBlendRatio(nullptr);
                    player->SetRealMoveAnim();
                }
            }
            else {
                ped->SetMoveAnim();
            }
        }
        // Como el original: cada comparacion conserva la semantica NaN del EXE (fcomp + test ah)
        const bool bInsideRadius = fDistSq < m_fRadius * m_fRadius;
        const CVector2D vecDistanceToTargetFromLastPoint = m_vecTargetPoint - m_vecLastPedPos;
        const bool bPassedFromLastPoint = DotProduct2D(vecDistance, vecDistanceToTargetFromLastPoint) < 0.0f;
        m_vecLastPedPos = pedPos;

        bool bReached = false;
        if (bInsideRadius && !gotoPointFlags.m_b05
            && (ped->bIgnoreHeightCheckOnGotoPointTask || fHeightDiff < 2.0f)
            && (!gotoPointFlags.m_b04 || bPassedFromLastPoint)) {
            bReached = true;
        } else if (!gotoPointFlags.m_b04) {
            const CVector2D vecDistanceToTargetFromPoint = m_vecTargetPoint - (CTimer::GetTimeStep() * ped->m_vecMoveSpeed + pedPos);
            if (DotProduct2D(vecDistance, vecDistanceToTargetFromPoint) <= 0.0f && !gotoPointFlags.m_b05) {
                bReached = true;
            }
        }
        if (bReached) {
            gotoPointFlags.m_b03 = true;
            QuitIK(ped);
            return true;
        }
        if (gotoPointFlags.m_b02) {
            QuitIK(ped);
            return true;
        }
        const float fAngleInRadians = CGeneral::GetRadianAngleBetweenPoints(vecDirection.x, vecDirection.y, 0.0f, 0.0f);
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(fAngleInRadians);
    }
    if (gotoFlags.m_bTargetPointUpdated) {
        if (gotoFlags.m_bIsIKChainSet && g_ikChainMan.IsLooking(ped))
            g_ikChainMan.AbortLookAt(ped, 250);
        gotoFlags.m_bTargetPointUpdated = false;
    }
    SetUpIK(ped);
    gotoPointFlags.m_b05 = false;
    return false;
}

// 0x645700
void CTaskSimpleGoToPoint::UpdatePoint(const CVector& targetPosition, float fRadius, bool bDontCheckRadius)
{
    if (bDontCheckRadius || m_vecTargetPoint != targetPosition || m_fRadius != fRadius)
    {
        m_vecTargetPoint = targetPosition;
        m_fRadius = fRadius;

        gotoFlags.m_targetCircledFlags = 0;
        gotoFlags.m_bTargetPointUpdated = true;
    }
}

bool CTaskSimpleGoToPoint::WasTaskSuccessful(CPed* ped) const {
    return gotoPointFlags.m_b03 && std::abs(m_vecTargetPoint.z - ped->GetPosition().z) < 3.f;
}
