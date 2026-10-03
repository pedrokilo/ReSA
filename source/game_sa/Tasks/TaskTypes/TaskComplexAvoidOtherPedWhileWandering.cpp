/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexAvoidOtherPedWhileWandering
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 15/15 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <Ragdoll/IKChainManager.h>
#include <PedGeometryAnalyser.h>
#include "TaskSimpleStandStill.h"
#include "TaskComplexGangFollower.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexAvoidOtherPedWhileWandering.h"

void CTaskComplexAvoidOtherPedWhileWandering::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexAvoidOtherPedWhileWandering, 0x86FEC4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66A100);
    RH_ScopedInstall(Destructor, 0x66A1D0);

    RH_ScopedInstall(QuitIK, 0x66A230);
    RH_ScopedInstall(ComputeSphere, 0x66A320);
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x66A7B0);
    RH_ScopedInstall(SetUpIK, 0x66A850);
    RH_ScopedInstall(NearbyPedsInSphere, 0x671FE0);
    RH_ScopedInstall(ComputeAvoidSphere, 0x672080);
    RH_ScopedInstall(ComputeDetourTarget, 0x672180);
    RH_ScopedVMTInstall(Clone, 0x66D050);
    RH_ScopedVMTInstall(GetTaskType, 0x66A1C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x66A260);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66A2C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x674610);
    RH_ScopedVMTInstall(ControlSubTask, 0x6721B0);
}

// 0x66A100
CTaskComplexAvoidOtherPedWhileWandering::CTaskComplexAvoidOtherPedWhileWandering(
    CPed* pedToAvoid,
    const CVector& targetPoint,
    eMoveState moveState,

    //VVV NOTSA ARGS VVV//
    bool isMovingTarget
) :
    m_PedToAvoid{ pedToAvoid },
    m_TargetPt{ targetPoint },
    m_DetourTargetPt{ targetPoint },
    m_MoveState{ moveState },
    m_IsMovingTarget{ isMovingTarget } // Como el original: el EXE limpia los tres bits; el argumento es NOTSA (por defecto false)
{
}

// 0x66D050
CTask* CTaskComplexAvoidOtherPedWhileWandering::Clone() const {
    // Como el original: constructor con (peatón, objetivo, estado de movimiento) y después copia el bit de objetivo móvil
    const auto task = new CTaskComplexAvoidOtherPedWhileWandering{ m_PedToAvoid, m_TargetPt, m_MoveState };
    if (m_IsMovingTarget) {
        task->m_IsMovingTarget = true;
    }
    return task;
}

// 0x6721B0
CTask* CTaskComplexAvoidOtherPedWhileWandering::ControlSubTask(CPed* ped) {
    const auto DoQuit = [&](bool stopIK = true) -> CTask* {
        if (stopIK) {
            QuitIK(ped);
        }
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
        return nullptr;
    };

    if (!m_PedToAvoid) {
        return DoQuit();
    }

    // Como el original: si la subtarea se deja abortar sale sin QuitIK
    if (m_WantsToQuit && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return DoQuit(false);
    }

    m_Timer.StartIfNotAlready(200);

    SetUpIK(ped);

    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return m_pSubTask;
    }

    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {
        QuitIK(ped);
        // Como el original: GetRandomNumberInRange(0, 2000) (0x407180) + 500
        return new CTaskSimpleStandStill{ CGeneral::GetRandomIntInRange(0, 2000) + 500 };
    }

    const auto &pedPos        = ped->GetPosition(),
               &pedToAvoidPos = m_PedToAvoid->GetPosition();
    if (std::abs(pedPos.z - pedToAvoidPos.z) > 3.f) { // Como el original: NaN y 3 exacto siguen
        return DoQuit();
    }

    // Como el original: dy*dy + dx*dx
    const float dx = pedPos.x - pedToAvoidPos.x, dy = pedPos.y - pedToAvoidPos.y;
    const float pedToAvoidDist2DSq = dy * dy + dx * dx;
    // Como el original: la tarea más simple es la del peatón a evitar, y el temporizador se consulta antes que IsGoToTask
    if (const auto pedSimplestTask = m_PedToAvoid->GetTaskManager().GetSimplestActiveTask()) {
        if (m_DontQuitYetTimer.IsOutOfTime() && CTask::IsGoToTask(pedSimplestTask)) {
            // Como el original: DotProduct (0x40FDB0, orden (z + y) + x) contra 0.923f
            const auto &fwd = ped->GetMatrix().GetForward(), &avoidFwd = m_PedToAvoid->GetMatrix().GetForward();
            const float dot = fwd.z * avoidFwd.z + fwd.y * avoidFwd.y + fwd.x * avoidFwd.x;
            if (dot >= 0.923f) {
                CVector pedSpeed   = ped->GetMoveSpeed() * 50.f;
                CVector avoidSpeed = m_PedToAvoid->GetMoveSpeed() * 50.f;
                pedSpeed.z = avoidSpeed.z = 0.f;
                if (pedSpeed.SquaredMagnitude() <= avoidSpeed.SquaredMagnitude() + 0.25f && pedToAvoidDist2DSq > 1.f) {
                    return DoQuit();
                }
            }
        }
    }

    static const float s_QuitDistSq = 2.5f * 2.5f + 1.f; // Como el original: estático inicializado en la primera llamada
    if (pedToAvoidDist2DSq > s_QuitDistSq) {
        return DoQuit();
    }

    if (!m_Timer.IsOutOfTime()) {
        return m_pSubTask;
    }
    m_Timer.Start(200);

    // Como el original: sin objetivo móvil no se sale; se recalcula el desvío igualmente
    if (m_IsMovingTarget) {
        if (const auto tGangFollower = static_cast<CTaskComplexGangFollower*>(ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_GANG_FOLLOWER))) {
            if (tGangFollower->m_Leader) {
                m_TargetPt = tGangFollower->m_Leader->GetPosition() + tGangFollower->CalculateOffsetPosition();
            }
        }
    }

    if (ComputeDetourTarget(ped)) {
        // Como el original: UpdatePoint(m_DetourTargetPt, 0.5f, false) sobre la subtarea sin comprobar el tipo
        static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(m_DetourTargetPt, 0.5f, false);
        return m_pSubTask;
    }

    return DoQuit();
}

// 0x66A260
bool CTaskComplexAvoidOtherPedWhileWandering::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    const bool subTaskAborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (subTaskAborted) {
        QuitIK(ped);
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
    }
    return subTaskAborted;
}

// 0x66A2C0
CTask* CTaskComplexAvoidOtherPedWhileWandering::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return CreateFirstSubTask(ped);
    }
    QuitIK(ped);
    ped->bIgnoreHeightCheckOnGotoPointTask = false;
    return nullptr;
}

// 0x674610
CTask* CTaskComplexAvoidOtherPedWhileWandering::CreateFirstSubTask(CPed* ped) {
    if (m_PedToAvoid) {
        m_StartPt = ped->GetPosition();
        // Como el original: ComputeDetourTarget en línea (ComputeAvoidSphere + ComputeRouteRoundSphere)
        CColSphere sp;
        ComputeAvoidSphere(ped, sp);
        if (ComputeRouteRoundSphere(ped, sp)) {
            ped->bIgnoreHeightCheckOnGotoPointTask = true;
            const auto task = new CTaskSimpleGoToPoint{ m_MoveState, m_DetourTargetPt };
            m_DontQuitYetTimer.Start(2000);
            return task;
        } else {
            QuitIK(ped);
        }
    }
    ped->bIgnoreHeightCheckOnGotoPointTask = false;
    return nullptr;
}

// 0x66A230
void CTaskComplexAvoidOtherPedWhileWandering::QuitIK(CPed* ped) {
    if (m_IsDoingIK && g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped);
    }
}

// 0x66A320
CColSphere* CTaskComplexAvoidOtherPedWhileWandering::ComputeSphere(CColSphere* out, PedsToAvoidArray& accountedPeds) {
    // Step 1: Find center
    CVector center{};
    int32 numPeds{};
    for (CPed* ped : accountedPeds) {
        if (!ped) {
            continue;
        }
        numPeds++;
        const auto& pos = ped->GetPosition();
        center.x += pos.x;
        center.y += pos.y;
        center.z += pos.z;
    }
    // Como el original: se multiplica por 1 / n
    const float invNum = 1.f / (float)numPeds;
    center.x *= invNum;
    center.y *= invNum;
    center.z *= invNum;

    // Step 2: Find max 2D[!] distance [this is going to be the radius]
    float maxDist2DSq{ 0.f };
    for (CPed* ped : accountedPeds) {
        if (!ped) {
            continue;
        }
        const auto& pos = ped->GetPosition();
        const float dx = pos.x - center.x, dy = pos.y - center.y;
        const float dist2DSq = dy * dy + dx * dx;
        if (!(dist2DSq > maxDist2DSq)) { // Como el original: NaN no cuenta
            continue;
        }
        maxDist2DSq = dist2DSq;
    }

    // Step 3: Construct sphere from it (Como el original: radio = sqrt + 1.05)
    out->Set(std::sqrt(maxDist2DSq) + 1.05f, center, SURFACE_DEFAULT, 0, tColLighting(0xFF));
    return out;
}

// 0x66A850
void CTaskComplexAvoidOtherPedWhileWandering::SetUpIK(CPed* ped) {
    const auto pad = CPad::GetPad(0);
    if (ped == FindPlayerPed() && !pad->DisablePlayerControls) {
        return;
    }
    // Como el original: GetIsOnScreen antes de mirar m_IsDoingIK
    if (!ped->GetIsOnScreen()) {
        return;
    }
    if (m_IsDoingIK) {
        return;
    }
    // Como el original: sale si ya mira a una entidad o si ya hay tarea secundaria de IK
    if (g_ikChainMan.GetLookAtEntity(ped)) {
        return;
    }
    if (ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK)) {
        return;
    }
    // Como el original: sin padre sigue; el tipo del padre se pide dos veces (una por comparación)
    if (m_Parent) {
        if (m_Parent->GetTaskType() == TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING) {
            return;
        }
        if (m_Parent->GetTaskType() == TASK_COMPLEX_AVOID_ENTITY) {
            return;
        }
    }
    const auto& pedPos = ped->GetPosition();
    CVector pedToTarget = m_TargetPt - pedPos;
    if (!(pedToTarget.SquaredMagnitude() > 9.f)) { // Como el original: NaN también sale
        return;
    }
    pedToTarget.Normalise(); // Como el original: CVector::Normalise (0x59C910)
    const auto& fwd = ped->GetMatrix().GetForward();
    // Como el original: orden (z + y) + x; NaN también sale
    if (!(pedToTarget.z * fwd.z + pedToTarget.y * fwd.y + pedToTarget.x * fwd.x < CTaskSimpleGoTo::ms_fLookAtThresholdDotProduct)) {
        return;
    }
    // Como el original: z = (2 * dir.z + z del peatón) + 0.61
    CVector lookAt{
        pedToTarget.x * 2.f + m_TargetPt.x,
        pedToTarget.y * 2.f + m_TargetPt.y,
        pedToTarget.z * 2.f + pedPos.z + 0.61f
    };
    g_ikChainMan.LookAt("TaskAvoidOthPed", ped, 0, 5000, BONE_UNKNOWN, &lookAt, false, 0.25f, 500, 3, false);
    m_IsDoingIK = true;
}

// 0x671FE0
bool CTaskComplexAvoidOtherPedWhileWandering::NearbyPedsInSphere(CPed* ped, const CColSphere& colSphere, PedsToAvoidArray& pedsToCheck, PedsToAvoidArray& pedsInSphere) {
    bool anyInSphere = false;
    for (auto& pedToCheck : pedsToCheck) {
        if (!pedToCheck) {
            continue;
        }

        CColSphere pedSphere;
        pedSphere.Set(1.05f, pedToCheck->GetPosition(), SURFACE_DEFAULT, 0, tColLighting(0xFF));
        if (!colSphere.IntersectSphere(pedSphere)) {
            continue;
        }

        // Como el original: sin hueco libre se pasa al siguiente peatón
        const auto insertAt = rng::find(pedsInSphere, nullptr);
        if (insertAt == pedsInSphere.end()) {
            continue;
        }

        *insertAt   = pedToCheck; // Como el original: se guarda el peatón y después se anula la entrada
        pedToCheck  = nullptr;
        anyInSphere = true;
    }
    return anyInSphere;
}

// 0x672080
void CTaskComplexAvoidOtherPedWhileWandering::ComputeAvoidSphere(CPed* ped, CColSphere& outSp) {
    // Como el original: la esfera inicial se escribe antes de recorrer el escáner
    outSp.Set(1.05f, m_PedToAvoid->GetPosition(), SURFACE_DEFAULT, 0, tColLighting(0xFF));

    PedsToAvoidArray pedsInSphere{};
    PedsToAvoidArray pedsToCheck;
    const auto& entities = ped->GetIntelligence()->GetPedScanner().m_apEntities; // Can't use GetEntities<CPed>() because it filters null entries
    for (size_t i = 0; i < pedsToCheck.size(); i++) {
        const auto pedToCheck = static_cast<CPed*>(entities[i]);
        pedsToCheck[i] = pedToCheck != m_PedToAvoid && !CPedGroups::AreInSameGroup(ped, pedToCheck) // Inverted
            ? pedToCheck
            : nullptr;
    }

    pedsInSphere[0] = m_PedToAvoid;
    while (NearbyPedsInSphere(ped, outSp, pedsToCheck, pedsInSphere)) {
        ComputeSphere(&outSp, pedsInSphere);
    }
}

// 0x66A7B0
bool CTaskComplexAvoidOtherPedWhileWandering::ComputeRouteRoundSphere(CPed* ped, CColSphere& spToAvoid) {
    CVector start = m_StartPt, target = m_TargetPt;
    start.z = target.z = spToAvoid.m_vecCenter.z = ped->GetPosition().z; // NOTE: Changes sphere's center.z!
    return CPedGeometryAnalyser::ComputeRouteRoundSphere(
        *ped,
        spToAvoid,
        start,
        target,
        m_NewTargetPt,
        m_DetourTargetPt
    );
}

// 0x672180
bool CTaskComplexAvoidOtherPedWhileWandering::ComputeDetourTarget(CPed* ped) {
    CColSphere sp;
    ComputeAvoidSphere(ped, sp);
    return ComputeRouteRoundSphere(ped, sp);
}
