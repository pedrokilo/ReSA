/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorSitAtDesk
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskInteriorSitAtDesk.h"
#include "Interior/InteriorInfo_t.h"
#include "Interior/InteriorManager_c.h"

void CTaskInteriorSitAtDesk::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorSitAtDesk, 0x87035c, 9);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x676010);
    RH_ScopedInstall(Destructor, 0x676080);

    RH_ScopedInstall(FinishAnimCB, 0x676190);
    RH_ScopedInstall(StartRandomLoopAnim, 0x677780);
    RH_ScopedInstall(StartRandomOneOffAnim, 0x677880);
    RH_ScopedVMTInstall(Clone, 0x6760E0);
    RH_ScopedVMTInstall(GetTaskType, 0x676070);
    RH_ScopedVMTInstall(MakeAbortable, 0x676150);
    RH_ScopedVMTInstall(ProcessPed, 0x677920);
}

// 0x676010
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(int32 duration, InteriorInfo_t* interiorInfo, bool bDoInstantly) :
    m_bDoInstantly{bDoInstantly},
    m_InteriorInfo{interiorInfo},
    m_Duration{duration}
{
}

// 0x6760E0
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(const CTaskInteriorSitAtDesk& o) :
    CTaskInteriorSitAtDesk{o.m_Duration, o.m_InteriorInfo, o.m_bDoInstantly}
{
}

// 0x676080
CTaskInteriorSitAtDesk::~CTaskInteriorSitAtDesk() {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x676190
void CTaskInteriorSitAtDesk::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskInteriorSitAtDesk>(static_cast<CTask*>(data));

    assert(self && anim);
    assert(anim == self->m_Anim);

    self->m_PrevAnimId = static_cast<AnimationId>(anim->m_AnimId);
    if (anim->m_AnimId == ANIM_ID_OFF_SIT_2IDLE_180 || self->m_bTaskAborting && self->m_PrevAnimId == ANIM_ID_OFF_SIT_IN) {
        anim->SetBlendDelta(-1000.f);
        self->m_bTaskFinished = true;
    }

    self->m_Anim = nullptr;
}

// 0x677780
void CTaskInteriorSitAtDesk::StartRandomLoopAnim(CPed* ped, float blendDelta) {
    using CGeneral::GetRandomNumberInRange;

    const auto chance = GetRandomNumberInRange(100u);
    StartAnim(
        ped,
        chance > 40
            ? ANIM_ID_OFF_SIT_TYPE_LOOP
            : chance > 10
                ? ANIM_ID_OFF_SIT_BORED_LOOP
                : ANIM_ID_OFF_SIT_IDLE_LOOP,
        blendDelta
    );
    m_AnimTimer.Start(chance > 40 ? GetRandomNumberInRange(2000, 5000) : GetRandomNumberInRange(7000, 12000));
}

// 0x677880
void CTaskInteriorSitAtDesk::StartRandomOneOffAnim(CPed* ped) {
    const auto chance = CGeneral::GetRandomNumberInRange(100u);
    StartAnim(
        ped,
        chance > 60
            ? ANIM_ID_OFF_SIT_READ
            : chance > 35
                ? ANIM_ID_OFF_SIT_DRINK
                : chance > 10
                    ? ANIM_ID_OFF_SIT_WATCH
                    : ANIM_ID_OFF_SIT_CRASH,
        4.f
    );
}

// 0x676150
bool CTaskInteriorSitAtDesk::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetBlendDelta(-1000.f);
            m_Anim->SetDefaultFinishCallback();
            m_Anim = nullptr;
        }
        return true;
    }
    m_bTaskAborting = true;
    return false;
}

// 0x677920
bool CTaskInteriorSitAtDesk::ProcessPed(CPed* ped) {
    const auto currentAnimId = m_Anim ? static_cast<AnimationId>(m_Anim->m_AnimId) : ANIM_ID_UNDEFINED;
    ped->SetMoveState(PEDMOVE_STILL);

    if (m_bTaskFinished && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_OFF_SIT_2IDLE_180)) {
        ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation + PI);
        ped->SetHeading(ped->m_fCurrentRotation);
        return true;
    }

    const auto StartGettingUp = [&] {
        StartAnim(ped, ANIM_ID_OFF_SIT_2IDLE_180, 1000.f);
        m_bUpdatePedPos = true;
    };
    if (m_bTaskAborting) {
        if (!g_interiorMan.AreAnimsLoaded(2)) {
            return true;
        }
        if (currentAnimId == ANIM_ID_OFF_SIT_IN) {
            m_Anim->SetBlendDelta(-8.f);
        } else if (currentAnimId >= ANIM_ID_OFF_SIT_IDLE_LOOP && currentAnimId <= ANIM_ID_OFF_SIT_WATCH) {
            if (!m_bUpdatePedPos) {
                StartGettingUp();
                return false;
            }
        } else if (currentAnimId == ANIM_ID_OFF_SIT_2IDLE_180) {
            m_Anim->m_Speed = 2.f;
        }
    }

    if (!m_Anim) {
        if (!g_interiorMan.AreAnimsLoaded(2)) {
            return false;
        }
        if (m_PrevAnimId == ANIM_ID_UNDEFINED) {
            if (!m_bDoInstantly) {
                StartAnim(ped, ANIM_ID_OFF_SIT_IN, 4.f);
                return false;
            }
        } else if (m_PrevAnimId != ANIM_ID_OFF_SIT_IN) {
            if (m_PrevAnimId >= ANIM_ID_OFF_SIT_CRASH && m_PrevAnimId <= ANIM_ID_OFF_SIT_WATCH) {
                StartRandomLoopAnim(ped, 4.f);
            }
            return false;
        }
        m_TaskTimer.Start(m_Duration);
        StartRandomLoopAnim(ped, 1000.f);
        m_bUpdatePedPos = true;
        return false;
    }

    if (m_bUpdatePedPos) {
        auto pos = ped->GetPosition();
        const auto z = pos.z;
        if (currentAnimId >= ANIM_ID_OFF_SIT_IDLE_LOOP && currentAnimId <= ANIM_ID_OFF_SIT_BORED_LOOP) {
            pos = ped->m_matrix->TransformPoint(CCarEnterExit::ms_vecPedDeskAnimOffset);
        } else if (currentAnimId == ANIM_ID_OFF_SIT_2IDLE_180) {
            pos = ped->m_matrix->TransformPoint(-CCarEnterExit::ms_vecPedDeskAnimOffset);
        }
        pos.z = z;
        ped->SetPosn(pos);
        m_bUpdatePedPos = false;
    }

    if (m_TaskTimer.IsStarted() && m_TaskTimer.IsOutOfTime()) {
        if (m_Anim->m_AnimId != ANIM_ID_OFF_SIT_2IDLE_180) {
            StartGettingUp();
        }
    } else if (m_AnimTimer.IsStarted() && m_AnimTimer.IsOutOfTime()) {
        m_AnimTimer.Stop();
        if (CGeneral::GetRandomNumber() < 0x3FFF) {
            StartRandomOneOffAnim(ped);
        } else {
            StartRandomLoopAnim(ped, 4.f);
        }
    }

    if (m_Anim->m_AnimId == ANIM_ID_OFF_SIT_IN) {
        const auto delta = m_InteriorInfo->Pos - ped->GetPosition();
        const auto distance = delta.Magnitude();
        const auto shift = (delta / distance) * std::min(distance, 0.02f);
        ped->m_vecAnimMovingShiftLocal = {shift.Dot(ped->GetRight()), shift.Dot(ped->GetForward())};
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(m_InteriorInfo->Dir.x, m_InteriorInfo->Dir.y, 0.f, 0.f));
    }
    return false;
}

void CTaskInteriorSitAtDesk::StartAnim(CPed* ped, AnimationId animId, float blendDelta) {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, animId, blendDelta);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
}
