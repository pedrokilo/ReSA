/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGunControl
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
#include "X87Math.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleDuck.h"

void CTaskSimpleGunControl::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGunControl, 0x86d76c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61F3F0);

    RH_ScopedInstall(Destructor, 0x61F4D0);

    RH_ScopedVMTInstall(Clone, 0x622FB0);
    RH_ScopedVMTInstall(GetTaskType, 0x61F4C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x61F530);
    RH_ScopedVMTInstall(ProcessPed, 0x625270);
}

// Not the actual ctor used originally
// because that took pointers instead for the CVector's, but that's stupid, because
// they just 0 inited it otherwise, thus we can just default init the args
// instead of passing nullptr, and it's going to be the same effect.
// so, tldr, instead of `nullptr` for the CVector&'s pass in `{}`
CTaskSimpleGunControl::CTaskSimpleGunControl(CEntity* targetEntity, CVector const& targetPos, CVector const& moveTarget, eGunCommand firingTask, int16 burstAmmoCnt, int32 leisureDurMs) :
    m_targetPos{targetPos},
    m_moveTarget{moveTarget},
    m_targetEntity{targetEntity},
    m_leisureDurMs{leisureDurMs},
    m_firingTask{firingTask},
    m_burstAmmoCnt{burstAmmoCnt},
    m_attackIntervalMult{1.f}
{
    CEntity::SafeRegisterRef(m_targetEntity);
}

CTaskSimpleGunControl::CTaskSimpleGunControl(const CTaskSimpleGunControl& o) :
    CTaskSimpleGunControl{o.m_targetEntity, o.m_targetPos, o.m_moveTarget, o.m_firingTask, o.m_burstAmmoCnt, o.m_leisureDurMs}
{
    m_aimImmidiately = o.m_aimImmidiately;
}

// 0x61F4D0
CTaskSimpleGunControl::~CTaskSimpleGunControl() {
    CEntity::SafeCleanUpRef(m_targetEntity);
}

// atan2(-x, y) con fpatan como el EXE (precision completa del FPU, redondeo al guardar)
static float GunControlHeading(float x, float y) {
    return TrigAtan2Cpp(-x, y); // X87Math.cpp
}

// 0x61F530
bool CTaskSimpleGunControl::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_URGENT && event) {
        if (event->GetEventPriority() < 22) {
            return false;
        }
        // Como el original: GetEventType se llama dos veces (EVENT_VEHICLE_COLLISION..EVENT_BUILDING_COLLISION)
        if (event->GetEventType() >= EVENT_VEHICLE_COLLISION && event->GetEventType() <= EVENT_BUILDING_COLLISION) {
            return false;
        }
    }

    if (ped->GetIntelligence()->GetTaskUseGun()) {
        // Como el original: la tarea se vuelve a leer del hueco secundario de ataque
        const auto useGun = static_cast<CTaskSimpleUseGun*>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK));
        useGun->m_CountDownFrames = priority == ABORT_PRIORITY_URGENT ? 2 : 10;
        useGun->m_HasFiredGun = false;
    }

    ForceStopDuckingMove(ped);

    return true;
}

// 0x625270
bool CTaskSimpleGunControl::ProcessPed(CPed* ped) {
    using enum eGunCommand;

    if (m_isFinished) {
        if (const auto useGun = ped->GetIntelligence()->GetTaskUseGun()) {
            useGun->m_CountDownFrames = std::min<uint8>(useGun->m_CountDownFrames, 4u);
            useGun->m_HasFiredGun = false;
        }
        ForceStopDuckingMove(ped);
        return true;
    }

    ped->SetMoveState(PEDMOVE_STILL); // Como el original

    int32 nextGunCmd = (int32)NONE;
    bool  dontCheckLeisureDur = false;

    if ((int8)m_firingTask < (int8)END_LEISURE) {
        // Como el original: cada acceso vuelve a leer el hueco secundario de ataque
        auto& tm = ped->GetTaskManager();
        if (!tm.GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            const auto useGun = new CTaskSimpleUseGun{
                m_targetEntity,
                m_targetPos,
                AIM,
                (uint16)m_burstAmmoCnt,
                m_aimImmidiately
            };
            tm.SetTaskSecondary(useGun, TASK_SECONDARY_ATTACK);
            m_nextAtkTimeMs  = 0;
            m_isFirstTime    = false;
            m_aimImmidiately = false;
        } else if (tm.GetTaskSecondary(TASK_SECONDARY_ATTACK)->GetTaskType() != TASK_SIMPLE_USE_GUN) {
            tm.GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
            return false;
        } else if (m_isFirstTime) {
            m_nextAtkTimeMs = 0;
            m_isFirstTime   = false;
            static_cast<CTaskSimpleUseGun*>(tm.GetTaskSecondary(TASK_SECONDARY_ATTACK))->Reset(ped, m_targetEntity, m_targetPos, AIM, m_burstAmmoCnt);
        }
    }

    const auto useGunTask = ped->GetIntelligence()->GetTaskUseGun();

    // Como el original: animacion de pistola de banda (tipos 7..15, o tipo 6 quieto con arma que apunta con el brazo)
    const auto pedType = (int32)ped->m_nPedType;
    const bool gunStand = (pedType >= 7 && pedType <= 15)
        || (pedType == 6 && (int32)ped->m_nMoveState == 1 && ped->m_nSwimmingMoveState == 1
            && useGunTask && useGunTask->m_WeaponInfo && useGunTask->m_WeaponInfo->flags.bAimWithArm);
    if (gunStand) {
        const auto anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_GANG_GUNSTAND);
        if (!anim || (anim->m_BlendAmount < 1.f && anim->m_BlendDelta <= 0.f)) {
            CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GANG_GUNSTAND, 4.f);
        }
    }

    CWeaponInfo* winfo = nullptr;
    if (useGunTask) {
        m_isLOSBlocked = useGunTask->m_IsLOSBlocked; // Como el original
        winfo = useGunTask->m_WeaponInfo;
    }
    if (!winfo) {
        const auto& weapon = ped->m_aWeapons.data()[(int8)ped->m_nActiveWeaponSlot];
        winfo = CWeaponInfo::GetWeaponInfo(weapon.m_Type, ped->GetWeaponSkill());
    }

    // Calcular el siguiente comando
    if (ped->m_aWeapons.data()[(int8)ped->m_nActiveWeaponSlot].m_State == WEAPONSTATE_RELOADING && winfo->flags.bReload) {
        nextGunCmd = (int32)RELOAD;
    } else {
        switch ((int8)m_firingTask) {
        case (int8)NONE:
            nextGunCmd = (int32)AIM;
            break;
        case (int8)AIM:
        case (int8)FIRE:
            if (!m_nextAtkTimeMs) {
                nextGunCmd = (int32)FIREBURST;
            } else {
                nextGunCmd = (int32)AIM;
                if (m_firingTask == AIM) {
                    dontCheckLeisureDur = true;
                }
            }
            break;
        case (int8)FIREBURST: {
            if (CTimer::GetTimeInMS() < m_nextAtkTimeMs) {
                if (m_nextAtkTimeMs == (uint32)-1 && useGunTask && useGunTask->m_LastCmd != FIREBURST) {
                    m_nextAtkTimeMs = 0;
                }
                nextGunCmd = (int32)AIM;
                break;
            }
            nextGunCmd      = (int32)FIREBURST;
            m_nextAtkTimeMs = (uint32)-1;
            const auto clip = (int16)winfo->m_nAmmoClip;
            m_burstAmmoCnt  = clip;
            if (clip > 1 && ped->m_nWeaponShootingRate < 100u) {
                const auto byRate = (int32)ped->m_nWeaponShootingRate * 4 / 30;
                m_burstAmmoCnt = (int16)((int32)clip < byRate ? (int32)clip : byRate);
                // Como el original: el max del EXE vuelve a pedir el numero aleatorio
                if ((int32)m_burstAmmoCnt - CGeneral::GetRandomIntInRange(0, 2) < 1) {
                    m_burstAmmoCnt = 1;
                } else {
                    m_burstAmmoCnt = (int16)((int32)m_burstAmmoCnt - CGeneral::GetRandomIntInRange(0, 2));
                }
            }
            if (useGunTask) {
                useGunTask->m_BurstLength = (uint16)m_burstAmmoCnt;
            }
            break;
        }
        case (int8)RELOAD:
            if (!useGunTask || useGunTask->m_LastCmd != RELOAD) {
                nextGunCmd = (int32)FIREBURST;
            } else {
                nextGunCmd   = (int32)END_LEISURE;
                m_firingTask = END_LEISURE;
            }
            break;
        case (int8)PISTOLWHIP:
            nextGunCmd = (int32)FIREBURST;
            break;
        case (int8)END_LEISURE:
            nextGunCmd          = (int32)END_LEISURE;
            dontCheckLeisureDur = true;
            break;
        default: // Como el original: END_NOW y valores fuera de rango dejan NONE
            break;
        }
    }

    if (m_leisureDurMs > -1) {
        const auto stepMs = (int32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.f); // Como el original: _ftol2
        m_leisureDurMs = std::max(m_leisureDurMs - stepMs, 0);
    }

    if (dontCheckLeisureDur || !m_leisureDurMs) { // 0x625681
        if (!useGunTask || (m_firingTask == END_LEISURE && useGunTask->m_IsFinished)) {
            m_isFinished = true;
        } else if (useGunTask->m_HasFiredGun && useGunTask->m_LastCmd != FIRE && useGunTask->m_LastCmd != FIREBURST) {
            useGunTask->m_CountDownFrames = 2;
            useGunTask->m_HasFiredGun     = false;
            m_isFinished = true;
        }

        if (m_isFinished) {
            ForceStopDuckingMove(ped);
            return true;
        }

        if (!m_leisureDurMs && useGunTask) {
            m_firingTask = END_LEISURE;
            useGunTask->ControlGun(ped, m_targetEntity, END_LEISURE);
        }
    }

    // Calculate next attack time - 0x625719
    if (!m_nextAtkTimeMs) {
        m_nextAtkTimeMs = DEFAULT_GUN_ATTACK_PERIOD;
        if (winfo->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_PROJECTILE) {
            m_nextAtkTimeMs = DEFAULT_GUN_ATTACK_PERIOD * 2;
        }
        // Como el original: rand() del EXE escalado en linea y periodo leido como entero sin signo
        const float rnd    = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL * 0.5f + 0.75f;
        const float period = (float)m_nextAtkTimeMs * rnd / ((float)ped->m_nWeaponShootingRate * m_attackIntervalMult * 0.04f);
        m_nextAtkTimeMs = (uint32)(int64)period + CTimer::GetTimeInMS();
    }

    bool isRightArmBlocked = false;
    if ((int16)nextGunCmd == (int16)FIREBURST && CTaskSimpleUseGun::RequirePistolWhip(ped, m_targetEntity)) { // 0x6257A1
        nextGunCmd = (int32)PISTOLWHIP;
    } else if (ped->bIsDucking ? ped->bDuckRightArmBlocked : ped->bRightArmBlocked) { // 0x6257BB (IsRightArmBlockedNow en linea)
        nextGunCmd = (int32)AIM;
        isRightArmBlocked = true;
    }

    if (useGunTask) {
        useGunTask->ControlGun(ped, m_targetEntity, (eGunCommand)nextGunCmd);
    }

    CVector pedToTarget;
    if (m_targetEntity) {
        pedToTarget = m_targetEntity->GetPosition() - ped->GetPosition();
    } else if (m_targetPos.x != 0.f || m_targetPos.y != 0.f) {
        pedToTarget = m_targetPos - ped->GetPosition();
    } else {
        return false;
    }

    ped->m_fAimingRotation = GunControlHeading(pedToTarget.x, pedToTarget.y);
    const float distSq = pedToTarget.z * pedToTarget.z + pedToTarget.x * pedToTarget.x + pedToTarget.y * pedToTarget.y; // Como el original: z primero

    if (useGunTask) {
        // Como el original: se mueve si el brazo esta bloqueado o si esta cerca y no es una rafaga sin poder moverse
        const bool wantsMove = isRightArmBlocked
            || (distSq < 6.f && (useGunTask->m_LastCmd != FIREBURST || winfo->flags.bMoveFire));
        if (wantsMove && !ped->bStayInSamePlace) {
            useGunTask->ControlGunMove({ 0.f, 1.f });
            return false;
        }
    }

    if (m_targetEntity && !ped->bIgnoreWeaponRange) {
        if (distSq > winfo->m_fWeaponRange * winfo->m_fWeaponRange) {
            m_leisureDurMs = 0;
        }
    }

    return false;
}

void CTaskSimpleGunControl::ForceStopDuckingMove(CPed* const ped) {
    // Como el original: GetTaskDuck se llama dos veces
    if (ped->bIsDucking && ped->GetIntelligence()->GetTaskDuck(true)) {
        ped->GetIntelligence()->GetTaskDuck(true)->ForceStopMove();
    }
}

bool CTaskSimpleGunControl::GetPedToTarget(const CPed* ped, CVector& out) {
    if (m_targetEntity) {
        out = m_targetEntity->GetPosition() - ped->GetPosition();
        return true;
    }

    if (!CVector2D{ m_targetPos }.IsZero()) {
        out = m_targetPos - ped->GetPosition();
        return true;
    }

    return false;
}
