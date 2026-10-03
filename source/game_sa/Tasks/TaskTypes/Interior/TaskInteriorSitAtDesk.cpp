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
 * Validación registrada: 9/9 entradas.
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
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); // Como el original: 0x4CEBC0 (tipo borrado)
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

// Como el original: (int)((rand() & 0xFFFF) * (1/32768) * escala) con _ftol2
static int32 SitAtDeskRandom(float scale) {
    return static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber() & 0xFFFF) * 3.0517578125e-05f * scale);
}

// 0x677780
void CTaskInteriorSitAtDesk::StartRandomLoopAnim(CPed* ped, float blendDelta) {
    // Como el original: se suelta la animación y se sacan los dos aleatorios antes de mezclar la nueva;
    // escribir (>40) dura 7000..12000 ms y las otras dos 2000..5000 ms
    if (m_Anim) {
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    const auto chance = SitAtDeskRandom(100.f);
    AnimationId animId;
    int32       duration;
    if (chance > 40) {
        animId   = ANIM_ID_OFF_SIT_TYPE_LOOP;
        duration = 7000 - SitAtDeskRandom(-5000.f);
    } else {
        animId   = chance > 10 ? ANIM_ID_OFF_SIT_IDLE_LOOP : ANIM_ID_OFF_SIT_BORED_LOOP;
        duration = 2000 - SitAtDeskRandom(-3000.f);
    }
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, animId, blendDelta);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
    m_AnimTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_AnimTimer.m_nInterval  = duration;
    m_AnimTimer.m_bStarted   = true;
}

// 0x677880
void CTaskInteriorSitAtDesk::StartRandomOneOffAnim(CPed* ped) {
    // Como el original (0x677880): se suelta la animación antes de sacar el aleatorio
    if (m_Anim) {
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    const auto chance = SitAtDeskRandom(100.f);
    const auto animId = chance > 60
        ? ANIM_ID_OFF_SIT_READ
        : chance > 35
            ? ANIM_ID_OFF_SIT_DRINK
            : chance > 10
                ? ANIM_ID_OFF_SIT_WATCH
                : ANIM_ID_OFF_SIT_CRASH;
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, animId, 4.f);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
}

// 0x676150
bool CTaskInteriorSitAtDesk::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetBlendDelta(-1000.f);
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); // Como el original: 0x4CEBC0
            m_Anim = nullptr;
        }
        return true;
    }
    m_bTaskAborting = true;
    return false;
}

// 0x59C890 con su orden de sumas (CMatrix * CVector)
static CVector SitAtDeskMatrixTimesPoint(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// 0x677920
bool CTaskInteriorSitAtDesk::ProcessPed(CPed* ped) {
    const auto currentAnimId = m_Anim ? static_cast<AnimationId>(m_Anim->m_AnimId) : ANIM_ID_UNDEFINED;
    ped->SetMoveState(PEDMOVE_STILL);

    if (m_bTaskFinished && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_OFF_SIT_2IDLE_180)) {
        // Como el original: pi exacto del EXE (0x858CB8), no el PI impreciso de common.h
        ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation + std::bit_cast<float>(0x40490FDBu));
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
            pos = SitAtDeskMatrixTimesPoint(*ped->m_matrix, CCarEnterExit::ms_vecPedDeskAnimOffset);
        } else if (currentAnimId == ANIM_ID_OFF_SIT_2IDLE_180) {
            pos = SitAtDeskMatrixTimesPoint(*ped->m_matrix, -CCarEnterExit::ms_vecPedDeskAnimOffset);
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
        // Como el original (0x677CB8): |d| = sqrt((z² + y²) + x²), d · (1/|d|) · min y productos (y + z) + x
        const auto delta = m_InteriorInfo->Pos - ped->GetPosition();
        const float distance = std::sqrt(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x);
        const float step = distance < 0.02f ? distance : 0.02f; // Como el original: NaN usa 0.02
        const float invDistance = 1.f / distance;
        const CVector shift{ delta.x * invDistance * step, delta.y * invDistance * step, delta.z * invDistance * step };
        const auto& right   = ped->GetRight();
        const auto& forward = ped->GetForward();
        ped->m_vecAnimMovingShiftLocal = {
            (right.y * shift.y + right.z * shift.z) + shift.x * right.x,
            (forward.y * shift.y + forward.z * shift.z) + shift.x * forward.x
        };
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(m_InteriorInfo->Dir.x, m_InteriorInfo->Dir.y, 0.f, 0.f));
    }
    return false;
}

void CTaskInteriorSitAtDesk::StartAnim(CPed* ped, AnimationId animId, float blendDelta) {
    if (m_Anim) {
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); // Como el original: 0x4CEBC0 (tipo borrado)
    }
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_OFFICE, animId, blendDelta);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
}
