/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDuck
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
#include "TaskSimpleDuck.h"
#include "AnimBlendAssociation.h"
#include "TaskSimpleUseGun.h"

void CTaskSimpleDuck::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDuck, 0x870bc4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x691FC0);
    RH_ScopedInstall(Destructor, 0x692030);

    RH_ScopedInstall(DeleteDuckAnimCB, 0x692550);
    RH_ScopedInstall(CanPedDuck, 0x692610);
    RH_ScopedInstall(IsTaskInUseByOtherTasks, 0x61C3D0);
    RH_ScopedInstall(RestartTask, 0x692390);
    RH_ScopedInstall(ControlDuckMove, 0x6923F0);
    RH_ScopedInstall(SetMoveAnim, 0x6939F0);
    RH_ScopedInstall(ForceStopMove, 0x6924B0);
    RH_ScopedInstall(SetDuckTimer, 0x692530);
    RH_ScopedInstall(SetControlType, 0x6924F0);
    RH_ScopedVMTInstall(Clone, 0x692CF0);
    RH_ScopedVMTInstall(GetTaskType, 0x692020);
    RH_ScopedVMTInstall(MakeAbortable, 0x692100);
    RH_ScopedVMTInstall(ProcessPed, 0x694390);
}

// 0x691FC0
CTaskSimpleDuck::CTaskSimpleDuck(eDuckControlType duckControlType, uint16 lengthOfDuck, int16 shotWhizzingCounter) :
    m_DuckControlType{duckControlType},
    m_LengthOfDuck{lengthOfDuck},
    m_ShotWhizzingCounter{shotWhizzingCounter},
    m_StartTime{CTimer::GetTimeInMS()}
{
    // Rest set in the header!
}

// notsa
CTaskSimpleDuck::CTaskSimpleDuck(const CTaskSimpleDuck& o) :
    CTaskSimpleDuck{o.m_DuckControlType, o.m_LengthOfDuck, o.m_ShotWhizzingCounter}
{
}

// 0x692030
CTaskSimpleDuck::~CTaskSimpleDuck() {
    if (m_DuckAnim) {
        m_DuckAnim->SetDefaultFinishCallback();
        if (m_DuckAnim->m_BlendAmount > 0.f && m_DuckAnim->m_BlendDelta >= 0.f && (m_DuckAnim->m_Flags & ANIMATION_IS_PARTIAL)) {
            m_DuckAnim->m_BlendDelta = -8.f;
        }
    }

    if (m_MoveAnim) {
        m_MoveAnim->SetDefaultFinishCallback();
        if (m_MoveAnim->m_BlendAmount > 0.f && m_MoveAnim->m_BlendDelta >= 0.f) {
            m_MoveAnim->m_BlendDelta = -8.f;
        }
    }
}

// 0x692550
void CTaskSimpleDuck::DeleteDuckAnimCB(CAnimBlendAssociation* assoc, void* data) {
    const auto self = static_cast<CTaskSimpleDuck*>(data); // aka `this`

    // Como el original: sin tarea o sin animación no hace nada
    if (!self || !assoc) {
        return;
    }

    switch (assoc->m_AnimId) {
    case ANIM_ID_WEAPON_CROUCH:
    case ANIM_ID_DUCK_COWER: {
        self->m_DuckAnim = nullptr;
        if (!self->m_MoveAnim || !self->m_bIsAborting) {
            self->m_bIsFinished = true;
        }
        break;
    }
    case ANIM_ID_CROUCH_ROLL_L:
    case ANIM_ID_CROUCH_ROLL_R: {
        self->m_MoveCmd.x = 0.0f;
        [[fallthrough]];
    }
    case ANIM_ID_GUNCROUCHFWD:
    case ANIM_ID_GUNCROUCHBWD: {
        self->m_MoveAnim = nullptr;
        if (self->m_bIsAborting) {
            self->m_bIsFinished = true;
        }
        break;
    }
    }
}

// 0x692610
bool CTaskSimpleDuck::CanPedDuck(CPed* ped) {
    if (ped->IsPlayer() && ped->m_nMoveState >= PEDMOVE_RUN) { // Como el original: comparación con signo >= 6
        return false;
    }

    // Como el original: tipo del arma activa, GetWeaponSkill() (0x5E6580) y GetWeaponInfo(tipo, habilidad) (0x743C60)
    const auto& activeWep = ped->GetActiveWeapon();
    const auto  wepType = activeWep.GetType();
    const auto& pedActiveWepWI = *CWeaponInfo::GetWeaponInfo(wepType, ped->GetWeaponSkill());

    switch (pedActiveWepWI.m_nWeaponFire) {
    case eWeaponFire::WEAPON_FIRE_MELEE:
    case eWeaponFire::WEAPON_FIRE_USE:
        return true;
    }

    if (ped->GetActiveWeapon().GetType() == WEAPON_SPRAYCAN) {
        return true;
    }

    if (pedActiveWepWI.flags.bCrouchFire) {
        return true;
    }

    return false;
}

// 0x61C3D0
bool CTaskSimpleDuck::IsTaskInUseByOtherTasks() const {
    if (m_MoveCmd.IsZero()) {
        if (m_DuckAnim) {
            if (!(m_DuckAnim->m_BlendAmount < 1.f) && !m_bIsAborting && m_ShotWhizzingCounter <= 0) { // Como el original: NaN cuenta como mezclada
                return false;
            }
        }
    }
    return true;
}

// 0x692340
void CTaskSimpleDuck::AbortBecauseOfOtherDuck(CPed* ped) {
    if (m_DuckAnim) {
        m_DuckAnim->SetDefaultFinishCallback();
        m_DuckAnim = nullptr;
    }

    if (m_MoveAnim) {
        m_MoveAnim->m_BlendDelta = -8.0;
        m_MoveAnim->SetDefaultFinishCallback(); // Como el original (0x692340): la de movimiento
        m_MoveAnim = nullptr;
    }

    m_bNeedToSetDuckFlag = true;
    m_bIsFinished        = true;
}

// 0x692390
void CTaskSimpleDuck::RestartTask(CPed* ped) {
    if (m_bNeedToSetDuckFlag) {
        ped->bIsDucking = true;
        m_bNeedToSetDuckFlag = false;
    }

    m_StartTime = CTimer::GetTimeInMS(); // Como el original: siempre reinicia la hora de inicio

    if (m_ShotWhizzingCounter >= 0) {
        // Como el original: 1000 - (int)((rand() & 0xFFFF) * (1 / 32768) * -1500), truncado con _ftol2
        const float r = (float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.f / 32768.f) * -1500.f;
        m_ShotWhizzingCounter = (int16)(1000 - (int32)(int64)r);
    }
}

// 0x6923F0
// @arg moveDir The desired move direction. The `y` axis being fwd/bwd, and `x` being left/right.
bool CTaskSimpleDuck::ControlDuckMove(CVector2D moveDir) {
    m_bIsInControl = true;

    // If going full fwd/bwd ignore left/right commands
    if (std::abs(m_MoveCmd.x) == 1.f) { // Originally checked if either -1 or 1 (this achieves the same thing)
        return true;
    }

    // Change direction, but limit maximum change to `maxUnits`
    const auto maxUnits = CTimer::GetTimeStep() * 0.07f;
    const auto moveDeltaY = moveDir.y - m_MoveCmd.y;
    if (moveDeltaY > maxUnits) {
        m_MoveCmd.y += maxUnits;
    } else if (moveDeltaY < -maxUnits) {
        m_MoveCmd.y -= maxUnits;
    } else { // Como el original: NaN copia la dirección pedida
        m_MoveCmd.y = moveDir.y;
    }

    if (std::abs(moveDir.y) < 0.1f && std::abs(moveDir.x) > 0.9f) { // Como el original: 0.9 exacto no cuenta
        m_MoveCmd.y = 0.f;
        m_MoveCmd.x = moveDir.x <= 0.f ? -1.f : 1.f;
    }
    return true; // Como el original: devuelve siempre 1
}

// 0x6939F0
void CTaskSimpleDuck::SetMoveAnim(CPed* ped) {
    const auto SetMoveAnimTo = [this, ped](AnimationId to) {
        m_MoveAnim = CAnimManager::BlendAnimation(
            ped->GetRpClump(),
            ANIM_GROUP_DEFAULT,
            to,
            8.f
        );
    };

    const auto SetMoveAnimSpeed = [this](float spd) {
        m_MoveAnim->m_Speed = 0.6f > spd ? 0.6f : spd; // Como el original: NaN se conserva
    };

    const auto IsCurrentMoveAnimGunCrouch = [this] {
        return notsa::contains<AnimationId>({ ANIM_ID_GUNCROUCHFWD, ANIM_ID_GUNCROUCHBWD }, m_MoveAnim->m_AnimId);
    };

    if (m_MoveCmd.x == 0.f) {
        const auto ChangeAnimToIfIts = [&, this, ped](AnimationId toAnimId, AnimationId ifItsAnimId) {
            if (m_MoveAnim && m_MoveAnim->m_AnimId != ifItsAnimId) {
                return;
            }
            if (m_MoveAnim) {
                m_MoveAnim->SetDefaultFinishCallback();
            }
            SetMoveAnimTo(toAnimId);
            m_MoveAnim->SetDeleteCallback(DeleteDuckAnimCB, this);
        };
        if (m_MoveCmd.y > 0.f) { //> 0x693A79 - Crouch backwards (???)
            ChangeAnimToIfIts(ANIM_ID_GUNCROUCHBWD, ANIM_ID_GUNCROUCHFWD);
            SetMoveAnimSpeed(m_MoveCmd.y);
        } else if (!(m_MoveCmd.y < 0.f)) { //> 0x693AF4 - Blend out the animation out if no more movement (Como el original: también con NaN)
            if (m_MoveAnim) {
                if (IsCurrentMoveAnimGunCrouch()) {
                    m_MoveAnim->SetFlag(ANIMATION_IS_PLAYING, false);
                    m_MoveAnim->SetBlendDelta(-4.f);
                }
            }
        } else /*if (m_MoveCmd.y <= 0.f)*/ { //> 0x693B09
            ChangeAnimToIfIts(ANIM_ID_GUNCROUCHFWD, ANIM_ID_GUNCROUCHBWD);
            SetMoveAnimSpeed(-m_MoveCmd.y);
        }
    } else {
        if (!m_MoveAnim || IsCurrentMoveAnimGunCrouch()) {
            if (m_MoveAnim) {
                m_MoveAnim->SetDefaultFinishCallback();
            }
            SetMoveAnimTo(m_MoveCmd.x > 0.f ? ANIM_ID_CROUCH_ROLL_R : ANIM_ID_CROUCH_ROLL_L);
            m_MoveAnim->SetFinishCallback(DeleteDuckAnimCB, this);
        }
    }
}

// 0x6924B0
void CTaskSimpleDuck::ForceStopMove() {
    m_bIsInControl = true;
    m_MoveCmd.y = 0.f;
}

// 0x61C420
bool CTaskSimpleDuck::StopFireGun() const {
    return m_MoveCmd.x != 0.f || !m_DuckAnim || m_DuckAnim->GetBlendAmount() < 1.f || m_bIsAborting || m_ShotWhizzingCounter > 0;
}

// 0x692530
void CTaskSimpleDuck::SetDuckTimer(uint16 time) {
    if (m_DuckControlType != DUCK_SCRIPT_CONTROLLED) {
        m_StartTime    = CTimer::GetTimeInMS(); // Como el original: también reinicia la hora de inicio
        m_LengthOfDuck = time;
    }
}

// 0x6924F0
bool CTaskSimpleDuck::SetControlType(eDuckControlType controlType) {
    if (m_bIsFinished || m_bIsAborting) {
        return false;
    }
    if (controlType == DUCK_SCRIPT_CONTROLLED) {
        m_LengthOfDuck = 0;
    }
    m_DuckControlType = controlType;
    m_StartTime       = CTimer::GetTimeInMS();
    m_bIsInControl    = true;
    return true;
}

// 0x692100
bool CTaskSimpleDuck::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: { // 0x69210C
        // Replace/blend duck anim with idle anim
        if (m_DuckAnim) {
            if (m_DuckAnim->m_Flags & ANIMATION_IS_PARTIAL) {
                m_DuckAnim->m_BlendDelta = -1000.f;
            } else {
                CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.f);
            }
            m_DuckAnim->SetDefaultFinishCallback();
            m_DuckAnim = nullptr;
        }

        if (m_MoveAnim) {
            m_MoveAnim->m_BlendDelta = -1000.f;
            m_MoveAnim->SetDefaultFinishCallback();
            m_MoveAnim = nullptr;
        }

        SetPedIsDucking(ped, false);

        return true;
    }
    case ABORT_PRIORITY_URGENT: { // 0x69219B
        if (m_ShotWhizzingCounter < 0) { // Como el original: 0 también sigue
            break;
        }
        if (const auto eShotFired = notsa::dyn_cast_if_present<const CEventGunShotWhizzedBy>(event)) {
            if (eShotFired->m_TaskId == TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING && event->GetSourceEntity()) {
                SetPedIsDucking(ped, false);
                return true;
            }
        }
        break;
    }
    }

    if (m_MoveAnim) { // 0x6921E5
        switch (m_MoveAnim->m_AnimId) {
        case ANIM_ID_CROUCH_ROLL_L:
        case ANIM_ID_CROUCH_ROLL_R:
            return false;
        }
    }

    const auto blendDelta = (priority == ABORT_PRIORITY_URGENT) ? -8.f : -4.f;

    if (m_DuckAnim) {
        if (m_DuckAnim->m_BlendAmount > 0.f && m_DuckAnim->m_BlendDelta >= 0.f) {
            if (m_DuckAnim->m_Flags & ANIMATION_IS_PARTIAL) {
                m_DuckAnim->m_BlendDelta = blendDelta;
            }
            CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, -blendDelta);
            ped->m_nSwimmingMoveState = eMoveState::PEDMOVE_STILL;
        }

        if (priority == ABORT_PRIORITY_URGENT) {
            m_DuckAnim->SetDefaultFinishCallback();
            m_DuckAnim = nullptr;
        }
    }

    if (m_MoveAnim) {
        if (m_MoveAnim->m_BlendAmount > 0.f && m_MoveAnim->m_BlendDelta >= 0.f) {
            if (priority == ABORT_PRIORITY_URGENT || notsa::contains({ ANIM_ID_GUNCROUCHFWD, ANIM_ID_GUNCROUCHBWD }, m_MoveAnim->GetAnimId())) {
                m_MoveAnim->m_BlendDelta = blendDelta;
                m_MoveAnim->SetFlag(ANIMATION_IS_PLAYING, false);
            }                
        }

        if (priority == ABORT_PRIORITY_URGENT) {
            m_MoveAnim->SetDefaultFinishCallback();
            m_MoveAnim = nullptr;
        }
    }

    if (ped->GetIntelligence()->GetTaskUseGun()) { // Como el original: se pide dos veces (0x6922FF / 0x694422)
        ped->GetIntelligence()->GetTaskUseGun()->ClearAnim(ped);
    }

    if (priority == ABORT_PRIORITY_URGENT) {
        m_bIsFinished = true;
        SetPedIsDucking(ped, false);
        return true;
    }

    m_bIsAborting = true;

    return false;
}

// 0x694390
bool CTaskSimpleDuck::ProcessPed(CPed* ped) {
    if (m_bIsFinished || ped->m_fHealth < 1.f) {
        if (!m_bIsAborting) {
            MakeAbortable(ped);
        }
        if (!m_bNeedToSetDuckFlag) {
            ped->bIsDucking = false;
        }
        return true;
    }

    // 0x6943D5
    if (m_bNeedToSetDuckFlag) {
        const auto tSimplestActiveDuck = ped->GetTaskManager().GetSimplestActiveTaskAs<CTaskSimpleDuck>();
        const auto tSecondaryDuck      = ped->GetIntelligence()->GetTaskSecondaryDuck();

        if (tSecondaryDuck == this && tSimplestActiveDuck) { // 0x694414
            tSimplestActiveDuck->AbortBecauseOfOtherDuck(ped);
        } else if (tSimplestActiveDuck == this && tSecondaryDuck) {
            MakeAbortable(ped);
            return false;
        }

        if (ped->GetIntelligence()->GetTaskUseGun()) { // Como el original: se pide dos veces (0x6922FF / 0x694422)
            ped->GetIntelligence()->GetTaskUseGun()->ClearAnim(ped);
        }

        SetPedIsDucking(ped, true);
    } else if (!ped->bIsDucking) {
        m_bIsFinished = true;
    }

    if (m_bIsAborting) {
        return false;
    }

    if (m_bIsInControl) { // 0x69448C
        m_CountDownFrames = 4;

        if (m_LengthOfDuck && CTimer::GetTimeInMS() > m_StartTime + m_LengthOfDuck) { // 0x6944A2 (Como el original: estrictamente mayor, prioridad LEISURE)
            MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }

        if (m_ShotWhizzingCounter > 0) { // 0x6944C1
            // Como el original: duración - (int)(paso * 0.02 * 1000) truncado con _ftol2
            m_ShotWhizzingCounter = (int16)(m_LengthOfDuck - (uint16)(int32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.f));
        }

        if (m_DuckAnim) {
            if (m_DuckAnim->GetBlendAmount() > 0.99f) {
                switch (m_DuckControlType) {
                case DUCK_TASK_CONTROLLED:
                case DUCK_SCRIPT_CONTROLLED:
                    SetMoveAnim(ped); // 0x6945C6
                }
            }
        } else {
            // Como el original: la animación de cansancio se desvanece antes de agacharse
            if (const auto tiredAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_IDLE_TIRED)) {
                tiredAnim->m_BlendDelta = -2.f;
            }

            const auto duckAnimId  = m_DuckControlType == DUCK_STANDALONE ? ANIM_ID_DUCK_COWER : ANIM_ID_WEAPON_CROUCH;
            const auto otherAnimId = m_DuckControlType == DUCK_STANDALONE ? ANIM_ID_WEAPON_CROUCH : ANIM_ID_DUCK_COWER;
            m_DuckAnim = CAnimManager::BlendAnimation(
                ped->GetRpClump(),
                ANIM_GROUP_DEFAULT,
                duckAnimId,
                4.f
            );
            m_DuckAnim->SetDeleteCallback(DeleteDuckAnimCB, this); // Como el original: SetDeleteCallback (0x4CEBC0)

            // Como el original: se desvanece la otra animación de agacharse
            if (const auto weaponCruchAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), otherAnimId)) { // 0x69454F
                if (weaponCruchAnim->m_BlendAmount > 0 && weaponCruchAnim->m_BlendDelta >= 0.f) {
                    if (weaponCruchAnim->m_Flags & ANIMATION_IS_PARTIAL) {
                        weaponCruchAnim->m_BlendDelta = -4.f;
                    } else {
                        CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_IDLE, 4.f);
                    }
                }
            }
        }

        if (m_DuckControlType == DUCK_TASK_CONTROLLED) {
            m_bIsInControl = false;
        }

        return false;
    }

    // Como el original: se comprueba el valor anterior a la resta
    if (m_CountDownFrames-- != 0 || !MakeAbortable(ped)) { // 0x6945EA
        return false;
    }

    ped->bIsDucking = false;
    return true;
}

void CTaskSimpleDuck::SetPedIsDucking(CPed* ped, bool value) {
    ped->bIsDucking      = value;
    m_bNeedToSetDuckFlag = !value;
}
