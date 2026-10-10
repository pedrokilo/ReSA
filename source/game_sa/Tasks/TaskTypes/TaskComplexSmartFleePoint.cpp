/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSmartFleePoint
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexWanderFlee.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleTired.h"
#include "TaskSimpleStandStill.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexSmartFleePoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSmartFleePoint, 0x86f744, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65BD20);
    RH_ScopedInstall(Destructor, 0x65BDB0);

    RH_ScopedInstall(SetDefaultTaskWanderDir, 0x65BE00);
    RH_ScopedInstall(ComputeFleeDir, 0x65BE40);
    RH_ScopedInstall(CreateSubTask, 0x65BE80);
    RH_ScopedInstall(SetFleePosition, 0x65C3C0);

    RH_ScopedVMTInstall(Clone, 0x65CED0);
    RH_ScopedVMTInstall(GetTaskType, 0x65BDA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x65BDC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65C0C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65C140);
    RH_ScopedVMTInstall(ControlSubTask, 0x65C1E0);
}

// 0x65BD20
CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(CVector const& fleePos, bool doScream, float safeDist, int32 fleeTimeMs) :
    m_fleePoint{fleePos},
    m_doScream{doScream},
    m_safeDist{safeDist},
    m_fleeTimeMs{fleeTimeMs}
{
    if (m_fleeTimeMs != -1) {
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval = m_fleeTimeMs;
        m_timer.m_bStarted = true;
    }
}

CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(const CTaskComplexSmartFleePoint& o)
    : CTaskComplexSmartFleePoint{o.m_fleePoint, o.m_doScream, o.m_safeDist, o.m_fleeTimeMs}
{
}

// 0x65BE00
void CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir(CPed* ped) {
    auto* task = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT);
    if (task && task->GetTaskType() == static_cast<eTaskType>(0x390)) {
        auto* wander = static_cast<CTaskComplexWander*>(task);
        auto* sub = static_cast<CTaskComplexWander*>(m_pSubTask);
        if (wander->m_LastNode != sub->m_LastNode || wander->m_NextNode != sub->m_NextNode) {
            wander->m_LastNode = sub->m_LastNode;
            wander->m_NextNode = sub->m_NextNode;
            wander->m_nDir = sub->m_nDir;
            wander->m_bNewNodes = true;
        }
    }
}

// 0x65BE40
uint32 CTaskComplexSmartFleePoint::ComputeFleeDir(CPed* ped) {
    const auto& pos = ped->GetPosition();
    return CGeneral::GetNodeHeadingFromVector(pos.x - m_fleePoint.x, pos.y - m_fleePoint.y);
}

// 0x65BE80
CTask* CTaskComplexSmartFleePoint::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case static_cast<eTaskType>(0x2CA):
        return new CTaskComplexLeaveAnyCar(0, false, true);
    case static_cast<eTaskType>(0x390):
        return new CTaskComplexWanderFlee(m_moveState, m_fleeDir);
    case static_cast<eTaskType>(0xCB):
        SetDefaultTaskWanderDir(ped);
        return new CTaskSimpleStandStill(0, false, false, 8.0f);
    case static_cast<eTaskType>(0xF4):
        if (m_moveState == static_cast<eMoveState>(6)) {
            auto* sequence = new CTaskComplexSequence;
            sequence->AddTask(new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0), static_cast<AnimationId>(0xA5), 4.0f, false));
            sequence->AddTask(new CTaskSimpleTired(2000));
            return sequence;
        }
        [[fallthrough]];
    default:
        return nullptr;
    }
}

// 0x65C3C0
void CTaskComplexSmartFleePoint::SetFleePosition(CVector const& point, float safeDist, bool scream) {
    if (m_fleePoint.x != point.x || m_fleePoint.y != point.y || m_fleePoint.z != point.z || m_safeDist != safeDist) {
        m_fleePoint = point;
        m_safeDist = safeDist;
        m_hasFleePointChanged = true;
    }
    m_doScream = scream;
}

// 0x65BDC0
bool CTaskComplexSmartFleePoint::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_LEISURE) {
        m_fleeTimeMs = -1;
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval = -1;
        m_timer.m_bStarted = true;
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65C0C0
CTask* CTaskComplexSmartFleePoint::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case static_cast<eTaskType>(0x2CA):
        m_fleeDir = uint8(ComputeFleeDir(ped));
        return CreateSubTask(static_cast<eTaskType>(0x390), ped);
    case static_cast<eTaskType>(0xCB):
        return CreateSubTask(static_cast<eTaskType>(0xF4), ped);
    case static_cast<eTaskType>(0xF4):
        return CreateSubTask(static_cast<eTaskType>(0x516), ped);
    default:
        return nullptr;
    }
}

// 0x65C140
CTask* CTaskComplexSmartFleePoint::CreateFirstSubTask(CPed* ped) {
    m_initalPos = ped->GetPosition();
    if (ped->bInVehicle && ped->m_pVehicle) {
        return CreateSubTask(static_cast<eTaskType>(0x2CA), ped);
    }
    m_fleeDir = uint8(ComputeFleeDir(ped));
    return CreateSubTask(static_cast<eTaskType>(0x390), ped);
}

// 0x65C1E0
CTask* CTaskComplexSmartFleePoint::ControlSubTask(CPed* ped) {
    auto* result = m_pSubTask;
    if (result->GetTaskType() != static_cast<eTaskType>(0x390)) {
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        return m_pSubTask;
    }
    if (m_doScream) {
        ped->Say(static_cast<eGlobalSpeechContext>(0x15B), 0, 0.1f, false, false, false);
    }
    auto* wander = static_cast<CTaskComplexWander*>(m_pSubTask);
    wander->m_nMoveState = m_moveState;
    if (m_hasFleePointChanged) {
        m_hasFleePointChanged = false;
        if (m_timer.m_bStarted) {
            m_timer.m_nStartTime = CTimer::GetTimeInMS();
            m_timer.m_nInterval = m_fleeTimeMs;
            m_timer.m_bStarted = true;
        }
        const auto dir = ComputeFleeDir(ped);
        if (m_fleeDir != uint8(dir)) {
            m_fleeDir = uint8(dir);
            if (uint32(wander->m_nDir) != uint32(m_fleeDir)) {
                wander->m_nDir = m_fleeDir;
                wander->m_bNewDir = true;
            }
        }
    } else {
        if (m_timer.m_bStarted && m_timer.IsOutOfTime()) {
            result = CreateSubTask(static_cast<eTaskType>(0xCB), ped);
        } else {
            const auto& pos = ped->GetPosition();
            const auto distance = [](const CVector& a, const CVector& b) {
                const float x = a.x - b.x, y = a.y - b.y, z = a.z - b.z;
                return (z*z + y*y) + x*x;
            };
            const float safeSq = m_safeDist*m_safeDist;
            if (distance(m_fleePoint, pos) > safeSq && distance(m_initalPos, pos) > safeSq) {
                result = CreateSubTask(static_cast<eTaskType>(0xCB), ped);
            }
        }
    }
    if (m_moveState == static_cast<eMoveState>(6) && !g_ikChainMan.IsLooking(ped) && CGeneral::GetRandomIntInRange(0, 100) <= 5) {
        g_ikChainMan.LookAt("TaskSmartFleePoint", ped, nullptr, 2000, static_cast<eBoneTag>(-1), &m_fleePoint, false, 0.25f, 500, 3, false);
    }
    return result;
}
