/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexTrackEntity
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexTrackEntity.h"
#include "TaskSimpleGoToPointFine.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFollowNodeRoute.h"

void CTaskComplexTrackEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexTrackEntity, 0x86F998, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65F3B0);
    RH_ScopedInstall(Destructor, 0x65F460);

    RH_ScopedInstall(SetOffsetPos, 0x65F760);
    RH_ScopedInstall(CalcTargetPos, 0x65F780);
    RH_ScopedInstall(CalcMoveRatio, 0x65F930);

    RH_ScopedVMTInstall(Clone, 0x65F4E0);
    RH_ScopedVMTInstall(GetTaskType, 0x65F450);
    RH_ScopedVMTInstall(MakeAbortable, 0x65F4C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65F590);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65F700);
    RH_ScopedVMTInstall(ControlSubTask, 0x663640);
}

// 0x65F3B0
CTaskComplexTrackEntity::CTaskComplexTrackEntity(CEntity* entity, CVector offsetPos, uint8 a6, int32 a7, float rangeMin, float rangeMax, uint8 a10) :
    m_rangeMax{rangeMax},
    m_rangeMin{rangeMin},
    m_toTrack{entity},
    m_offsetPosn{offsetPos},
    a{a6},
    b{a7},
    f{a10},
    m_fMoveRatio{-1.f} // 0x65F438: negative = take it from the ped's move state in CreateFirstSubTask
{
    CEntity::RegisterReference(m_toTrack);
}

// NOTSA
// 0x65F4E0: 0x30..0x39 work as a timer; while it runs, the clone gets the time left instead of `b`
CTaskComplexTrackEntity::CTaskComplexTrackEntity(const CTaskComplexTrackEntity& o) :
    CTaskComplexTrackEntity{
        o.m_toTrack, o.m_offsetPosn, o.a,
        o.gap2 ? static_cast<int32>(o.m_someStartTimeMs + o.m_expirationInterval - CTimer::GetTimeInMS()) : o.b,
        o.m_rangeMin, o.m_rangeMax, o.f
    }
{
    m_fMoveRatio = o.m_fMoveRatio;
}

// 0x65F460
CTaskComplexTrackEntity::~CTaskComplexTrackEntity() {
    CEntity::CleanUpOldReference(m_toTrack);
}

// 0x65F760
void CTaskComplexTrackEntity::SetOffsetPos(CVector posn) {
    m_offsetPosn = posn;
}

// 0x65F780
void CTaskComplexTrackEntity::CalcTargetPos(CPed* ped) {
    m_goToPos = m_toTrack->GetPosition();
    if (f) {
        m_toTrack->GetMatrix();
        m_goToPos += m_toTrack->GetRight() * m_offsetPosn.x;
        m_goToPos += m_toTrack->GetForward() * m_offsetPosn.y;
    } else {
        m_goToPos.x += m_offsetPosn.x;
        m_goToPos.y += m_offsetPosn.y;
    }
    if (m_toTrack->GetIsTypeVehicle() || m_toTrack->GetIsTypePed()) {
        m_goToPos += static_cast<CPhysical*>(m_toTrack)->m_vecMoveSpeed * CTimer::GetTimeStep();
    }
    const auto delta = m_goToPos - ped->GetPosition();
    m_distToTargetSq = delta.y * delta.y + delta.x * delta.x;
}

// 0x65F930
void CTaskComplexTrackEntity::CalcMoveRatio(CPed* ped) {
    if (m_distToTargetSq < sq(0.2f)) {
        float40 = 0.0f;
    } else if (m_distToTargetSq > sq(5.0f)) {
        float40 = 1.0f;
    } else if (m_distToTargetSq < sq(1.0f)) {
        float40 = (std::sqrt(m_distToTargetSq) - 0.2f) * (1.0f / (1.0f - 0.2f)) * 0.5f;
    } else {
        float40 = (std::sqrt(m_distToTargetSq) - 1.0f) * (1.0f / (5.0f - 1.0f)) * 0.5f + 0.5f;
    }
    float40 = std::sqrt(float40) * 3.0f;
    if (!a && float40 > 2.0f) {
        float40 = 2.0f;
    }
    m_fMoveRatio = float40 - m_fMoveRatio > 0.2f ? m_fMoveRatio + 0.2f : float40;
}

// 0x65F4C0
bool CTaskComplexTrackEntity::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65F590
CTask* CTaskComplexTrackEntity::CreateNextSubTask(CPed* ped) {
    if (!m_toTrack) {
        return nullptr;
    }

    if (!m_pSubTask || m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
        return new CTaskSimpleStandStill{ 50 }; // 0x65F6C7
    }

    if (m_distToTargetSq < sq(m_rangeMin)) {
        return new CTaskSimpleGoToPointFine{ m_fMoveRatio, m_goToPos, 0.25f, nullptr };
    }

    return new CTaskComplexFollowNodeRoute{
        PEDMOVE_RUN,
        m_toTrack->GetPosition(),
        0.5f,
        0.2f,
        2.0f,
        false,
        -1,
        true
    };
}

// 0x65F700
CTask* CTaskComplexTrackEntity::CreateFirstSubTask(CPed* ped) {
    if (m_fMoveRatio < 0.0) {
        m_fMoveRatio = [ped] {
            switch (ped->m_nMoveState) {
            case 1: return 0.0f;
            case 4: return 1.0f;
            case 6: return 2.0f;
            default: return 3.0f;
            }
        }();
    }
    return CreateNextSubTask(ped);
}

// 0x663640
CTask* CTaskComplexTrackEntity::ControlSubTask(CPed* ped) {
    const auto TryAbort = [this, ped] {
        return MakeAbortable(ped);
    };

    const auto TryAbortGetTask = [&, this] {
        return TryAbort() ? nullptr : m_pSubTask;
    };

    if (!m_toTrack) {
        return TryAbortGetTask();
    }

    if (gap2) {
        if (gap3) {
            m_someStartTimeMs = CTimer::GetTimeInMS();
            gap3 = false;
        }
        if (CTimer::GetTimeInMS() >= m_someStartTimeMs + m_expirationInterval) {
            return TryAbortGetTask();
        }
    }

    CalcTargetPos(ped);

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_NODE_ROUTE) {
        // If we're totally out of range...
        if (m_distToTargetSq > sq(m_rangeMax)) {
            return nullptr;
        }

        // If we're now in range be a little more precise and create `TASK_SIMPLE_GO_TO_POINT_FINE`
        if (m_distToTargetSq < sq(m_rangeMin) && TryAbort()) {
            return CreateNextSubTask(ped);
        }

    } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
        // Check if we're still in range, if not, abort and create `TASK_COMPLEX_FOLLOW_NODE_ROUTE`
        if (m_distToTargetSq >= sq(m_rangeMin) && TryAbort()) {
            return CreateNextSubTask(ped);
        }

        CalcMoveRatio(ped);

        const auto gotoTask = static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask);
        gotoTask->SetTargetPos(m_goToPos);
        gotoTask->SetMoveRatio(m_fMoveRatio);

    }

    return m_pSubTask;
}
