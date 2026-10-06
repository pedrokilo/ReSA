/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskLeanOnVehicle
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskLeanOnVehicle.h"

void CTaskLeanOnVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskLeanOnVehicle, 0x86FAF4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660F90);
    RH_ScopedInstall(Destructor, 0x661030);

    RH_ScopedInstall(FinishAnimCB, 0x661160);

    RH_ScopedVMTInstall(MakeAbortable, 0x661110);
    RH_ScopedVMTInstall(ProcessPed, 0x6648C0);
}

// 0x660F90
CTaskLeanOnVehicle::CTaskLeanOnVehicle(CEntity* vehicle, int32 leanAnimDurationInMs, uint8 a4) {
    m_LeanAnimDurationInMs = leanAnimDurationInMs;
    m_Vehicle = vehicle->AsVehicle(); // todo: strange
    field_10 = a4;
    m_LeanAnim = nullptr;
    m_LastAnimId = ANIM_ID_UNDEFINED;
    m_bFinished = 0;
    m_LeanAnimId = 0;
    m_StopLeaning = 0;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x661030
CTaskLeanOnVehicle::~CTaskLeanOnVehicle() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    if (m_LeanAnim) {
        m_LeanAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x661160
void CTaskLeanOnVehicle::FinishAnimCB(CAnimBlendAssociation* assoc, void* data) {
    const auto self = notsa::cast<CTaskLeanOnVehicle>(static_cast<CTask*>(data));

    self->m_LastAnimId = assoc->m_AnimId;

    if (assoc->m_AnimId == ANIM_ID_LEANOUT) {
        assoc->m_BlendDelta = -1000.0f;
        self->m_bFinished = true;
    }

    if (self->m_LeanAnimId) {
        if (assoc->m_AnimId == ANIM_ID_LEANIN) {
            assoc->m_BlendDelta = -1000.0f;
            self->m_bFinished = true;
        }
    }

    self->m_LeanAnim = nullptr;
}

// 0x661110
bool CTaskLeanOnVehicle::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_LeanAnim) {
            m_LeanAnim->m_BlendDelta = -1000.0f;
            m_LeanAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_LeanAnim = nullptr;
        }
        m_StopLeaning = true;
        ped->m_pEntityIgnoredCollision = nullptr;
        return true;
    }

    m_LeanAnimId = 1;
    return false;
}

// 0x6648C0
bool CTaskLeanOnVehicle::ProcessPed(CPed* ped) {
    if (!m_Vehicle) {
        m_StopLeaning = true;
    }
    ped->m_pEntityIgnoredCollision = m_Vehicle;

    // Como el original: id de la animación activa (-1 si no hay)
    const int32 curAnimId = m_LeanAnim ? (int32)(int16)m_LeanAnim->m_AnimId : -1;

    if (m_StopLeaning) {
        ped->m_pEntityIgnoredCollision = nullptr;
        return true;
    }

    ped->SetMoveState(PEDMOVE_STILL);

    if (m_bFinished && !RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (uint32)ANIM_ID_LEANOUT)) {
        ped->m_pEntityIgnoredCollision = nullptr;
        return true;
    }

    // Como el original: grupo de animaciones 0x34 (ANIM_GROUP_GANGS) para entrar, estar y salir de la postura.
    constexpr auto LeanGroup = static_cast<AssocGroupId>(0x34);
    const auto PlayAndTrack = [&](AnimationId animId, float blend) {
        const auto anim = CAnimManager::BlendAnimation(ped->GetRpClump(), LeanGroup, animId, blend);
        m_LeanAnim = anim;
        anim->SetFinishCallback(FinishAnimCB, this);
    };

    if (m_LeanAnimId) {
        if (curAnimId == ANIM_ID_LEANIN) {
            m_LeanAnim->m_BlendDelta = -8.0f;
        } else if (curAnimId == ANIM_ID_LEANIDLE) {
            m_LeanAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            PlayAndTrack(ANIM_ID_LEANOUT, 1000.0f);
            return false;
        } else if (curAnimId == ANIM_ID_LEANOUT) {
            m_LeanAnim->m_Speed = 3.0f;
        }
    }

    if (!m_LeanAnim) {
        if (m_LastAnimId == -1) {
            if (!field_10) {
                PlayAndTrack(ANIM_ID_LEANIN, 4.0f);
                return false;
            }
        } else if (m_LastAnimId != ANIM_ID_LEANIN) {
            return false;
        }

        if (m_LeanAnimDurationInMs >= 0) {
            m_LeanTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_LeanTimer.m_nInterval  = m_LeanAnimDurationInMs;
            m_LeanTimer.m_bStarted   = true;
        }
        PlayAndTrack(ANIM_ID_LEANIDLE, 1000.0f);
    } else if (m_LeanTimer.m_bStarted && m_LeanTimer.IsOutOfTime() && m_LeanAnim->m_AnimId != ANIM_ID_LEANOUT) {
        m_LeanAnim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        PlayAndTrack(ANIM_ID_LEANOUT, 1000.0f);
    }
    return false;
}
