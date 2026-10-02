/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkAlongsidePed
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWalkAlongsidePed.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"

void CTaskComplexWalkAlongsidePed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkAlongsidePed, 0x8706cc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x683240);
    RH_ScopedInstall(Destructor, 0x6832C0);

    RH_ScopedInstall(CreateSubTask, 0x685000);
    RH_ScopedVMTInstall(Clone, 0x6838E0);
    RH_ScopedVMTInstall(GetTaskType, 0x6832B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x683320);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x685680);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x685630);
    RH_ScopedVMTInstall(ControlSubTask, 0x685170);
}

// 0x683240
CTaskComplexWalkAlongsidePed::CTaskComplexWalkAlongsidePed(CPed* ped, float giveUpDist) :
    m_TargetPed{ped},
    m_GiveUpDist{giveUpDist}
{
}

// 0x6838E0
CTaskComplexWalkAlongsidePed::CTaskComplexWalkAlongsidePed(const CTaskComplexWalkAlongsidePed& o) :
    CTaskComplexWalkAlongsidePed{o.m_TargetPed, o.m_GiveUpDist}
{
}

// 0x685000
CTask* CTaskComplexWalkAlongsidePed::CreateSubTask(eTaskType tt, CPed* ped) {
    switch (tt) {
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ped->m_pVehicle, 0, 0, true, false};
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{10};
    case TASK_COMPLEX_SEEK_ENTITY: {
        const auto t = new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>{
            m_TargetPed,
            50'000,
            1'000,
            0.5f,
            5.f,
            2.f,
            false,
            false,
            CEntitySeekPosCalculatorXYOffset{m_Offset}
        };
        t->SetMoveState(PEDMOVE_SPRINT);
        t->SetIsTrackingEntity(true);
        return t;
    }
    default: // Como el original: cualquier otro tipo devuelve nullptr
        return nullptr;
    }
}

// 0x685680
CTask* CTaskComplexWalkAlongsidePed::CreateNextSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    case TASK_COMPLEX_SEEK_ENTITY:
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }
    return nullptr;
}

// 0x685630
CTask* CTaskComplexWalkAlongsidePed::CreateFirstSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return nullptr;
    }
    return CreateSubTask(ped->IsInVehicle() ? TASK_COMPLEX_LEAVE_CAR : TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x685170
CTask* CTaskComplexWalkAlongsidePed::ControlSubTask(CPed* ped) {
    if (!m_TargetPed) {
        return m_pSubTask->MakeAbortable(ped)
            ? nullptr
            : m_pSubTask;
    }

    const auto pedWalkAnim       = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_WALK);
    const auto targetPedWalkAnim = RpAnimBlendClumpGetAssociation(m_TargetPed->GetRpClump(), ANIM_ID_WALK);

    const auto isTargetMoving = notsa::contains({PEDMOVE_WALK, PEDMOVE_RUN, PEDMOVE_SPRINT}, m_TargetPed->GetIntelligence()->GetMoveStateFromGoToTask());

    const auto pedToTarget         = m_TargetPed->GetPosition2D() - ped->GetPosition2D();
    const auto targetToPedDist2DSq = pedToTarget.SquaredMagnitude();

    // Como el original (0x685278): solo se rinde si dist² > giveUp² (con igualdad o NaN sigue)
    if (targetToPedDist2DSq > sq(m_GiveUpDist)) {
        return nullptr;
    }

    // Como el original: banderas separadas para el ped y el objetivo ([esp+4Ch] y [esp+0Dh])
    bool bPedChangedAnimSpeed    = false;
    bool bTargetChangedAnimSpeed = false;

    const auto tSubTaskSeekEntity = notsa::dyn_cast_if_present<CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>>(m_pSubTask);
    if (!tSubTaskSeekEntity) { // 0x68528B
        ped->bMoveAnimSpeedHasBeenSetByTask         = false;
        m_TargetPed->bMoveAnimSpeedHasBeenSetByTask = false;
        return m_pSubTask;
    }

    // Como el original: el contador se lee antes de tocar la subtarea y las animaciones
    const bool bStaticForLong = ped->GetIntelligence()->m_AnotherStaticCounter > 8;

    tSubTaskSeekEntity->GetSeekPosCalculator().SetOffset(m_Offset);
    tSubTaskSeekEntity->SetEntityMinDist2D(3.f);
    
    const auto pedTaskFollowNodeRoute = ped->GetTaskManager().Find<CTaskComplexFollowNodeRoute>();
    const auto pedTaskGoToPoint       = ped->GetTaskManager().Find<CTaskSimpleGoToPoint>();

    if (pedTaskGoToPoint && !pedTaskFollowNodeRoute) { // 0x685308 (Inverted)
        const auto& tm = m_TargetPed->GetMatrix();

        // Como el original (0x59C890): CMatrix * CVector con su orden de sumas
        CVector targetPtForPed{
            ((tm.GetUp().x * m_Offset.z + tm.GetForward().x * m_Offset.y) + tm.GetRight().x * m_Offset.x) + tm.GetPosition().x,
            ((tm.GetUp().y * m_Offset.z + tm.GetRight().y * m_Offset.x) + tm.GetForward().y * m_Offset.y) + tm.GetPosition().y,
            ((tm.GetUp().z * m_Offset.z + tm.GetRight().z * m_Offset.x) + tm.GetForward().z * m_Offset.y) + tm.GetPosition().z,
        };
        const auto& pedPos = ped->GetPosition();
        const CVector2D pedToTargetPt{ targetPtForPed.x - pedPos.x, targetPtForPed.y - pedPos.y };

        if (isTargetMoving) { // 0x68535C
            const auto& fwd = tm.GetForward();

            // Como el original: -(pt·fwd) con orden (y + z) + x, más DotProduct(posición del PED, fwd) (0x40FDB0: (z + y) + x)
            const float negPtDotFwd  = -((targetPtForPed.y * fwd.y + targetPtForPed.z * fwd.z) + targetPtForPed.x * fwd.x);
            const float pedPosDotFwd = (pedPos.z * fwd.z + pedPos.y * fwd.y) + pedPos.x * fwd.x;
            if (pedPosDotFwd + negPtDotFwd < 0.f) {
                const auto pedToTargetPtDist2DSq = pedToTargetPt.SquaredMagnitude();

                if (pedWalkAnim) { // 0x6853C6
                    if (pedToTargetPtDist2DSq < 2.f) { // 0x6853DB
                        targetPtForPed += fwd * 2.f;
                    }

                    // Como el original: 0x404330 / 0x420800 (con NaN devuelven el segundo argumento) y escritura directa de m_Speed
                    const auto IncreaseWalkAnimSpeed = [](CAnimBlendAssociation* walkAnim) {
                        const float s = walkAnim->m_Speed + 0.0125f;
                        walkAnim->m_Speed = (1.25f < s) ? 1.25f : s;
                    };
                    const auto DecreaseWalkAnimSpeed = [](CAnimBlendAssociation* walkAnim) {
                        const float s = walkAnim->m_Speed - 0.0125f;
                        walkAnim->m_Speed = (0.85f > s) ? 0.85f : s;
                    };
                    // Como el original (0x6854BC): se acerca a la velocidad de SetMoveAnimSpeed en pasos de 0.0125
                    const auto ApproachMoveAnimSpeed = [](CPed* p, CAnimBlendAssociation* walkAnim) {
                        const float oldSpeed = walkAnim->m_Speed;
                        p->SetMoveAnimSpeed(walkAnim);
                        const float newSpeed = walkAnim->m_Speed;
                        if (approxEqual(oldSpeed, newSpeed, 0.013f)) {
                            walkAnim->m_Speed = oldSpeed;
                        } else {
                            walkAnim->m_Speed = (oldSpeed > newSpeed) ? oldSpeed - 0.0125f : oldSpeed + 0.0125f;
                        }
                    };

                    if (pedToTargetPtDist2DSq < sq(0.25f)) {
                        DecreaseWalkAnimSpeed(pedWalkAnim);
                        bPedChangedAnimSpeed = true;
                        if (targetPedWalkAnim) {
                            IncreaseWalkAnimSpeed(targetPedWalkAnim);
                            bTargetChangedAnimSpeed = true;
                        }
                    } else if (pedToTargetPtDist2DSq > sq(0.25f)) {
                        IncreaseWalkAnimSpeed(pedWalkAnim);
                        bPedChangedAnimSpeed = true;
                        if (targetPedWalkAnim) {
                            DecreaseWalkAnimSpeed(targetPedWalkAnim);
                            bTargetChangedAnimSpeed = true;
                        }
                    } else { // Igualdad exacta o NaN
                        ApproachMoveAnimSpeed(ped, pedWalkAnim);
                        bPedChangedAnimSpeed = true;
                        if (targetPedWalkAnim) {
                            ApproachMoveAnimSpeed(m_TargetPed, targetPedWalkAnim);
                            bTargetChangedAnimSpeed = true;
                        }
                    }
                }
            }
        }

        pedTaskGoToPoint->UpdatePoint(targetPtForPed); // 0x685586
    }

    // Como el original (0x685598): dist² < 4 estricto
    if (bStaticForLong && targetToPedDist2DSq < sq(2.f) && m_pSubTask->MakeAbortable(ped)) { // 0x6855C2
        m_Offset = CVector{CVector2D{pedToTarget}};
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }

    ped->bMoveAnimSpeedHasBeenSetByTask         = bPedChangedAnimSpeed;
    m_TargetPed->bMoveAnimSpeedHasBeenSetByTask = bTargetChangedAnimSpeed;

    return m_pSubTask;
}
