/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleUseGun
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 19/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Ragdoll/IKChainManager.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleFight.h"

void CTaskSimpleUseGun::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleUseGun, 0x86D724, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    //RH_ScopedInstall(Constructor, 0x61DE60);
    //RH_ScopedInstall(Destructor, 0x61DF30);

    RH_ScopedGlobalInstall(FinishGunAnimCB, 0x61F3A0);
    RH_ScopedGlobalInstall(RequirePistolWhip, 0x61E200);
    RH_ScopedInstall(Reset, 0x624DC0);
    RH_ScopedInstall(AimGun, 0x61ED10);
    RH_ScopedInstall(FireGun, 0x61EB10);
    RH_ScopedInstall(StartCountDown, 0x61E160);
    RH_ScopedInstall(ClearAnim, 0x61E190);
    RH_ScopedInstall(PlayerPassiveControlGun, 0x61E0A0);
    RH_ScopedInstall(ControlGun, 0x61E040);
    RH_ScopedInstall(AbortIK, 0x61DFA0);
    RH_ScopedInstall(ControlGunMove, 0x61E0C0);
    RH_ScopedInstall(RemoveStanceAnims, 0x61E8E0);
    RH_ScopedInstall(StartAnim, 0x624F30);
    RH_ScopedInstall(SetMoveAnim, 0x61E3F0);
    RH_ScopedVMTInstall(Clone, 0x622F20);
    RH_ScopedVMTInstall(GetTaskType, 0x61DF20);
    RH_ScopedVMTInstall(MakeAbortable, 0x624E30);
    RH_ScopedVMTInstall(ProcessPed, 0x62A380);
    RH_ScopedVMTInstall(SetPedPosition, 0x624ED0);
}

// 0x61DE60
CTaskSimpleUseGun::CTaskSimpleUseGun(CEntity* targetEntity, CVector vecTarget, eGunCommand firstCmd, uint16 burstLength, bool isAimImmediate) :
    m_NextCmd{ firstCmd },
    m_TargetPos{ vecTarget },
    m_TargetEntity{ targetEntity },
    m_BurstLength{ burstLength }, // Como el original: el EXE guarda la rafaga en 0x34 y pone 0x36 a cero
    m_IsAimImmediate{ isAimImmediate } {
    reinterpret_cast<uint8*>(this)[0xD] = 0; // Como el original: el byte de los bits de disparo se escribe entero
}

// 0x61DF30
CTaskSimpleUseGun::~CTaskSimpleUseGun() {
    if (m_Anim) {
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x61E0C0
bool CTaskSimpleUseGun::ControlGunMove(const CVector2D& moveDir) {
    const float maxDelta = CTimer::GetTimeStep() * 0.07f;
    // Como el original: compara la diferencia con el maximo (no el destino con los limites redondeados),
    // procesa primero la Y y, si la diferencia no supera el maximo en ningun sentido (o es NaN), copia el destino
    const auto Approach = [maxDelta](float& cur, float dest) {
        const float diff = dest - cur;
        if (diff > maxDelta) {
            cur = maxDelta + cur;
        } else if (diff < -maxDelta) {
            cur = cur - maxDelta;
        } else {
            cur = dest;
        }
    };
    Approach(m_MoveCmd.y, moveDir.y);
    Approach(m_MoveCmd.x, moveDir.x);
    m_HasMoveControl = true;
    return true;
}

// 0x61F3A0
void CTaskSimpleUseGun::FinishGunAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleUseGun*>(data); // Como el original: sin consultar el tipo de la tarea

    // Como el original: el contador de la rafaga se compara con signo
    if (int16(self->m_BurstShots) > 0 && self->m_LastCmd == eGunCommand::RELOAD && self->m_NextCmd < eGunCommand::END_LEISURE) {
        switch (anim->GetAnimId()) {
        case ANIM_ID_RELOAD:
        case ANIM_ID_CROUCHRELOAD: {
            self->m_NextCmd = eGunCommand::RELOAD;
        }
        }
    }

    if (self->m_Anim == anim) {
        self->m_Anim = nullptr;
    }
}

// 0x61EB10
void CTaskSimpleUseGun::FireGun(CPed* ped, bool isLeftHand) {
    CVector origin{};
    CVector barrelPos = m_WeaponInfo->m_vecFireOffset;
    if (ped->bCalledPreRender) { // If pre-render was called, the bone positions are up-to-date
        const auto handBoneId = isLeftHand ? BONE_L_HAND : BONE_R_HAND;
        ped->GetBonePosition(&origin, handBoneId, false); // Como el original: la sobrecarga del EXE (0x5E4280)
        origin.z = (0.15f + barrelPos.z) + origin.z;
        ped->GetTransformedBonePosition(barrelPos, handBoneId, false);
        if (ped->m_pedIK.bUseArm) { // Como el original: solo con el brazo en uso (no al reves)
            ped->m_pedIK.bGunReachedTarget = g_ikChainMan.IsFacingTarget(ped, isLeftHand ? eIKChainSlot::LEFT_ARM : eIKChainSlot::RIGHT_ARM);
        }
    } else { // Otherwise ped is probably not on screen anyways, so just do something good enough
        origin = ped->GetPosition();
        origin.z += 0.7f;
        barrelPos = origin;
        if (ped->m_pedIK.bUseArm) { // Como el original: sin huesos actualizados da el arma por orientada
            ped->m_pedIK.bGunReachedTarget = true;
        }
    }
    ped->GetActiveWeapon().Fire(
        ped,
        &origin,
        &barrelPos,
        m_TargetEntity,
        m_TargetPos.x == 0.f || m_TargetPos.y == 0.f ? nullptr : &m_TargetPos,
        nullptr
    );

    if (ped->GetActiveWeapon().GetState() == WEAPONSTATE_FIRING) {
        ped->DoGunFlash(250, isLeftHand);
    }
}

// 0x61E0A0
bool CTaskSimpleUseGun::PlayerPassiveControlGun() {
    m_IsInControl = true;
    // Como el original: comparacion con signo (< FIRE), cualquier orden negativa cuenta como pasiva
    if (m_LastCmd < eGunCommand::FIRE && m_NextCmd < eGunCommand::FIRE) {
        m_NextCmd = eGunCommand::END_LEISURE;
        return true;
    }
    return false;
}

// 0x61E8E0
void CTaskSimpleUseGun::RemoveStanceAnims(CPed* ped, float x) {
    // Como el original: compara la duracion total con el tiempo actual (sin dividir)
    if (m_Anim && !(m_Anim->m_Flags & ANIMATION_IS_PLAYING) && m_Anim->m_BlendHier->m_fTotalTime > m_Anim->m_CurrentTime) {
        if (m_WeaponInfo && m_WeaponInfo->GetAnimLoopStart() == m_Anim->GetCurrentTime()) {
            m_Anim->SetCurrentTime(m_WeaponInfo->GetAnimLoopEnd());
        }
        m_Anim->SetFlag(ANIMATION_IS_PLAYING);
    }

    bool pedHasStanceAnims = false;
    for (const auto animId : {
        ANIM_ID_GUN_STAND,
        ANIM_ID_GUNMOVE_FWD,
        ANIM_ID_GUNMOVE_L,
        ANIM_ID_GUNMOVE_BWD,
        ANIM_ID_GUNMOVE_R,
    }) {
        if (const auto a = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animId)) {
            a->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
            pedHasStanceAnims = true;
        }
    }

    ped->SetMoveState(PEDMOVE_STILL);
    ped->m_nSwimmingMoveState = PEDMOVE_STILL;

    if (!pedHasStanceAnims) {
        return;
    }

    const auto DoBlendAnimAndStart = [ped](AnimationId animId) {
        const auto animWalk = CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, animId);
        animWalk->SetFlag(ANIMATION_IS_PLAYING);
    };

    if (ped->IsPlayer()) {
        if (const auto pad = ped->AsPlayer()->GetPadFromPlayer()) {
            // Como el original: cada eje se lee dos veces (> 50 y < -50)
            if (pad->GetPedWalkUpDown() > 50 || pad->GetPedWalkUpDown() < -50 || pad->GetPedWalkLeftRight() > 50 || pad->GetPedWalkLeftRight() < -50) {
                DoBlendAnimAndStart(ANIM_ID_WALK);

                ped->SetMoveState(PEDMOVE_WALK);
                ped->m_nSwimmingMoveState = PEDMOVE_WALK;

                if (const auto pd = ped->GetPlayerData()) {
                    pd->m_fMoveBlendRatio = 1.f;
                }
                return; // Como el original: al caminar no mezcla la animacion de reposo
            }
        }
    }

    DoBlendAnimAndStart(ANIM_ID_IDLE);

    if (m_NextCmd == eGunCommand::END_LEISURE) {
        switch (ped->m_nAnimGroup) {
        case ANIM_GROUP_PLAYERROCKET:
        case ANIM_GROUP_PLAYERROCKETF:
        case ANIM_GROUP_PLAYERROCKETM:
            break;
        default:
            CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GUN_2_IDLE, 8.f);
        }
    }
}

// 0x61E200
bool CTaskSimpleUseGun::RequirePistolWhip(CPed* ped, CEntity* targetEntity) {
    auto* const intel = ped->GetIntelligence();

    // Como el original: con un objetivo peaton solo se prueba ese (indice 15); si no, las 16 entradas del escaner de peatones
    CPed* other = nullptr;
    int32 i     = 0;
    if (targetEntity && targetEntity->GetIsTypePed()) {
        other = targetEntity->AsPed();
        i     = 15;
    }

    if (const auto tUseGun = intel->GetTaskUseGun()) {
        if (tUseGun->GetLastGunCommand() == eGunCommand::PISTOLWHIP) {
            return false;
        }
    }

    for (; i < 16; i++, other = nullptr) {
        if (!other) {
            other = static_cast<CPed*>(intel->GetPedEntities()[i]);
            if (!other) {
                continue;
            }
        }

        // Como el original: la posicion de la cabeza se pide antes de comprobar si esta vivo
        CVector headPos;
        other->GetBonePosition(&headPos, BONE_HEAD, false);
        if (!other->IsAlive()) { // Ped is dead :D
            continue;
        }
        if (!(headPos.z > ped->GetPosition().z) || !(ped->GetPosition().z + 1.5f > headPos.z)) { // Ped is below/above us (NaN: no)
            continue;
        }
        // Como el original: la tarea mas simple activa del peaton se usa sin comprobar nulo
        if (other->bFallenDown && ped->GetTaskManager().GetSimplestActiveTask()->GetTaskType() != TASK_SIMPLE_GET_UP) { // Fell, but hasn't got up yet?
            continue;
        }

        // Como el original: productos escalares en orden z, y, x con la matriz del peaton
        const CVector  d   = other->GetPosition() - ped->GetPosition();
        const CMatrix& mat = *ped->m_matrix;
        const float    fwdDot = d.z * mat.GetForward().z + d.y * mat.GetForward().y + d.x * mat.GetForward().x;
        if (!(fwdDot > 0.f)) { // Is behind?
            continue;
        }
        if (!(std::sqrt(d.y * d.y + d.x * d.x) < 1.f)) {
            continue;
        }
        const float rightDot = d.z * mat.GetRight().z + d.y * mat.GetRight().y + d.x * mat.GetRight().x;
        if (std::abs(rightDot) / fwdDot < std::bit_cast<float>(0x3F13C9EFu)) { // 0.5773 (0x8D2E6C)
            return true;
        }
    }
    return false;
}

// 0x61E040
bool CTaskSimpleUseGun::ControlGun(CPed* ped, CEntity* target, eGunCommand cmd) {
    m_IsInControl = true;
    if (target != m_TargetEntity) { // New target?
        m_TargetEntity  = target;
        AbortIK(ped); // Como el original: al cambiar de objetivo aborta la IK
        m_IsArmIKInUse  = false;
        m_IsLookIKInUse = false;
    }
    m_NextCmd = std::max(cmd, m_NextCmd);
    return true;
}

// 0x61DFA0
void CTaskSimpleUseGun::AbortIK(CPed* ped) {
    if (m_IsArmIKInUse) {
        g_ikChainMan.AbortPointArmIfPointing(eIKArm::IK_ARM_RIGHT, ped);
        g_ikChainMan.AbortPointArmIfPointing(eIKArm::IK_ARM_LEFT, ped);
        m_IsArmIKInUse = false;
    }
    if (m_IsLookIKInUse) {
        g_ikChainMan.AbortLookAtIfLooking(ped);
        m_IsLookIKInUse = false;
    }
    ped->m_pedIK.bEverythingUsed = false;
}

// 0x61ED10
void CTaskSimpleUseGun::AimGun(CPed* ped) {
    // Como el original: velocidad y tiempo de mezcla de la IK salen de variables del EXE;
    // apuntar inmediato los sustituye una sola vez (el indicador se consume)
    float ikSpeed     = StaticRef<float>(0x8D2E70);
    int32 ikBlendTime = StaticRef<int32>(0x8D2E74);
    if (m_IsAimImmediate) {
        ikBlendTime      = 0;
        ikSpeed          = 1.f;
        m_IsAimImmediate = false;
    }

    if (!ped->bIsDucking && m_WeaponInfo->flags.bAimWithArm) {
        ped->m_pedIK.bUseArm         = true;
        ped->m_pedIK.bEverythingUsed = false;
    } else {
        if (!ped->m_pedIK.bEverythingUsed) {
            AbortIK(ped);
        }
        ped->m_pedIK.bUseArm         = false;
        ped->m_pedIK.bEverythingUsed = true;
    }

    const auto DoLookAt = [ped](CEntity* lookAtEntity, eBoneTag lookAtBone, CVector* lookAtPos) {
        g_ikChainMan.LookAt("TaskUseGun", ped, lookAtEntity, 9'999'999, lookAtBone, lookAtPos, true, 0.25f, 250, 3, false);
    };
    const auto DoPointArms = [&, ped](CEntity* lookAtEntity, eBoneTag armBone, CVector* lookAtPos) {
        g_ikChainMan.PointArm("CTaskSimpleUseGun", eIKArm::IK_ARM_RIGHT, ped, lookAtEntity, armBone, lookAtPos, ikSpeed, ikBlendTime, 100.f);
        if (m_WeaponInfo->flags.bTwinPistol && !ped->bLeftArmBlocked) {
            g_ikChainMan.PointArm("CTaskSimpleUseGun", eIKArm::IK_ARM_LEFT, ped, lookAtEntity, armBone, lookAtPos, ikSpeed, ikBlendTime, 100.f);
        }
    };

    if (m_TargetEntity) { // If there's a target entity, aim at them
        // Como el original: objetivo peaton -> si el que apunta tiene datos de jugador, su hueso y desplazamiento objetivo
        // transformados en el objetivo; si no, la posicion de la columna del objetivo; otra entidad -> su posicion
        CVector lookAtPos;
        if (m_TargetEntity->GetIsTypePed()) {
            const auto targetPed = m_TargetEntity->AsPed();
            if (const auto pd = ped->GetPlayerData()) {
                lookAtPos = pd->m_vecTargetBoneOffset;
                targetPed->GetTransformedBonePosition(lookAtPos, (eBoneTag)pd->m_nTargetBone, false);
            } else {
                targetPed->GetBonePosition(&lookAtPos, BONE_SPINE1, false);
            }
        } else {
            lookAtPos = m_TargetEntity->GetPosition();
        }

        if (!ped->IsPlayer()) {
            ped->Say(CTX_GLOBAL_TARGET);
        }

        if (ped->m_pedIK.bUseArm) {
            if (!m_IsArmIKInUse || !m_IsLookIKInUse) {
                // Como el original: con objetivo peaton el hueso es el del jugador que apunta (columna si no es jugador); -1 si no es peaton
                eBoneTag lookAtBone = BONE_UNKNOWN;
                if (m_TargetEntity->GetIsTypePed()) {
                    lookAtBone = ped->IsPlayer() ? (eBoneTag)ped->GetPlayerData()->m_nTargetBone : BONE_SPINE1;
                }
                if (!m_IsLookIKInUse && m_Anim->m_BlendAmount > 0.98f) {
                    DoLookAt(m_TargetEntity, lookAtBone, nullptr);
                    m_IsLookIKInUse = true;
                }
                if (!m_IsArmIKInUse) {
                    DoPointArms(m_TargetEntity, lookAtBone, nullptr);
                    m_IsArmIKInUse = true;
                }
            }
            ped->m_pedIK.RotateTorsoForArm(m_TargetEntity->GetPosition());
        } else {
            ped->m_pedIK.PointGunAtPosition(lookAtPos, m_Anim->m_BlendAmount);
        }
    } else if (ped->m_pedIK.bUseArm) { // 0x61EFB4 - Process player free-aiming
        const auto pd      = ped->GetPlayerData();
        const auto camMode = TheCamera.m_aCams[0].m_nMode;
        if (pd && pd->m_bFreeAiming && m_TargetPos.x == 0.f && m_TargetPos.y == 0.f && (camMode == MODE_AIMWEAPON || camMode == MODE_AIMWEAPON_ATTACHED)) { // Aim to in front of us
            CVector origin, target{ 0.f, 0.f, 0.7f };
            TheCamera.Find3rdPersonCamTargetVector(20.f, ped->GetPosition() + target, origin, target);

            if (m_Anim->m_BlendAmount > 0.98f) {
                DoLookAt(nullptr, BONE_UNKNOWN, &target);
                m_IsLookIKInUse = true;
            }
            DoPointArms(nullptr, BONE_UNKNOWN, &target);
            m_IsArmIKInUse = true;

            ped->m_pedIK.RotateTorsoForArm(target);
        } else if (!m_IsArmIKInUse || !m_IsLookIKInUse) {
            if (m_TargetPos.x != 0.f && m_TargetPos.y != 0.f) { // 0x61F16F
                if (m_Anim->m_BlendAmount > 0.98f) {
                    DoLookAt(nullptr, BONE_UNKNOWN, &m_TargetPos); // Como el original: no marca la IK de mirada como activa
                }
                DoPointArms(nullptr, BONE_UNKNOWN, &m_TargetPos);
            } else { // 0x61F205
                // Como el original: mira y apunta con ambos brazos relativo al hombro derecho del propio peaton
                CVector target{ 0.f, 2.f, 0.f };
                if (m_Anim->m_BlendAmount > 0.98f && !m_IsLookIKInUse) {
                    DoLookAt(ped, BONE_R_UPPER_ARM, &target);
                    m_IsLookIKInUse = true;
                }
                DoPointArms(ped, BONE_R_UPPER_ARM, &target);
            }
            m_IsArmIKInUse = true;
        }
    } else if (m_TargetPos.x != 0.f && m_TargetPos.y != 0.f) { // 0x61F2F4
        ped->m_pedIK.PointGunAtPosition(m_TargetPos, m_Anim->m_BlendAmount);
    } else if (ped->IsPlayer()) { // 0x61F317
        ped->m_pedIK.PointGunInDirection(ped->m_fCurrentRotation, ped->GetPlayerData()->m_fLookPitch, false, m_Anim->m_BlendAmount);
    } else { // Como el original: los que no son jugador usan la direccion de mirada
        ped->m_pedIK.PointGunInDirection(ped->m_fLookDirection, 0.f, false, m_Anim->m_BlendAmount);
    }

    if (m_WeaponInfo->flags.bTwinPistol && ped->bLeftArmBlocked) {
        g_ikChainMan.AbortPointArmIfPointing(eIKArm::IK_ARM_LEFT, ped); // Como el original: el brazo izquierdo (hueco 1)
    }
}

// 0x61E190
void CTaskSimpleUseGun::ClearAnim(CPed* ped) {
    if (m_Anim) {
        if (m_Anim->GetBlendAmount() > 0.f && m_Anim->GetBlendDelta() >= 0.f) {
            m_Anim->SetBlendDelta(-8.f);
        }
        m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
    }
    if (m_LastCmd < eGunCommand::END_LEISURE) {
        m_LastCmd = eGunCommand::NONE;
    }
    AbortIK(ped);
}

// 0x624E30
bool CTaskSimpleUseGun::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_URGENT && priority != ABORT_PRIORITY_IMMEDIATE) { // Como el original: cualquier otra prioridad cuenta como ociosa
        m_NextCmd = eGunCommand::END_LEISURE;
        return false;
    }
    RemoveStanceAnims(ped, -4.f);
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.f);
    }
    if (m_Anim) {
        if (priority == ABORT_PRIORITY_IMMEDIATE) {
            m_Anim->SetBlendDelta(-1000.f);
        }
        m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
    }
    m_IsFinished = true;
    AbortIK(ped);
    if (const auto pd = ped->GetPlayerData()) {
        pd->m_fAttackButtonCounter = 0.f;
    }
    return true;
}

// 0x62A380
bool CTaskSimpleUseGun::ProcessPed(CPed* ped) {
    using Cmd = eGunCommand;
    if (m_IsLookIKInUse && !g_ikChainMan.IsLooking(ped)) m_IsLookIKInUse = false;
    if (m_IsArmIKInUse && !g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_RIGHT, ped)) m_IsArmIKInUse = false;
    m_IsFiringGunRightHandThisFrame = m_IsFiringGunLeftHandThisFrame = false;

    const auto Finish = [&] {
        AbortIK(ped);
        if (auto* data = ped->GetPlayerData()) data->m_fAttackButtonCounter = 0.f;
        return true;
    };
    if (!m_WeaponInfo) {
        if (auto* data = ped->GetPlayerData(); data && data->m_nChosenWeapon != ped->m_nActiveWeaponSlot) return false;
        m_WeaponInfo = &ped->GetActiveWeapon().GetWeaponInfo(ped);
        if (m_WeaponInfo->m_nWeaponFire == WEAPON_FIRE_MELEE || m_WeaponInfo->flags.bThrow) {
            m_IsFinished = true;
            m_WeaponInfo = nullptr;
            return Finish();
        }
        m_MoveCmd = {};
    } else if (m_WeaponInfo != &ped->GetActiveWeapon().GetWeaponInfo(ped)) {
        MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
    }
    if (!m_WeaponInfo->flags.b1stPerson || !ped->GetPlayerData()) ped->bTestForBlockedPositions = true;
    if (m_IsFinished) {
        RemoveStanceAnims(ped, -4.f);
        return Finish();
    }
    if (auto* data = ped->GetPlayerData(); data && data->m_fAttackButtonCounter > 0.f) {
        data->m_fAttackButtonCounter *= std::pow(0.96f, CTimer::GetTimeStep());
    }
    if (!m_IsInControl) {
        m_HasFiredGun = false;
        m_MoveCmd = {};
        if (m_CountDownFrames-- != 0) return false;
        m_IsFinished = true;
        RemoveStanceAnims(ped, -4.f);
        return Finish();
    }

    m_CountDownFrames = 255;
    const auto EndControl = [&] {
        if (!m_WeaponInfo->flags.bAimWithArm || ped->bIsDucking || m_HasMoveControl) SetMoveAnim(ped);
        if (m_NextCmd < Cmd::END_LEISURE && (m_NextCmd != Cmd::RELOAD || (m_LastCmd != Cmd::FIRE && m_LastCmd != Cmd::FIREBURST))) m_NextCmd = Cmd::NONE;
        m_IsInControl = false;
        return false;
    };
    const auto IsFire = [](Cmd cmd) { return cmd == Cmd::FIRE || cmd == Cmd::FIREBURST; };
    const bool blocked = (ped->bIsDucking && ped->bDuckRightArmBlocked)
        || (!ped->bIsDucking && ped->bRightArmBlocked && !m_WeaponInfo->flags.bAimWithArm);
    if (blocked) {
        if (m_LastCmd != Cmd::PISTOLWHIP && (m_NextCmd != Cmd::PISTOLWHIP || m_Anim)) {
            if (m_Anim || (m_NextCmd >= Cmd::AIM && m_NextCmd <= Cmd::FIREBURST)) {
                if (m_Anim) {
                    m_Anim->SetBlendDelta(-4.f);
                    m_Anim->SetFlag(ANIMATION_IS_PLAYING, false);
                    m_Anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
                }
                AbortIK(ped);
                m_IsLOSBlocked = true;
                if (m_LastCmd == Cmd::FIREBURST) m_LastCmd = Cmd::AIM;
                return EndControl();
            }
        }
    } else if (m_NextCmd == Cmd::PISTOLWHIP && m_LastCmd <= Cmd::FIREBURST) {
        if (m_Anim) m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
        AbortIK(ped);
    }
    if (!m_Anim) {
        StartAnim(ped);
        if (!m_Anim) AbortIK(ped);
        return EndControl();
    }

    auto* anim = m_Anim;
    if (m_LastCmd == Cmd::RELOAD) {
        if (anim->GetAnimId() != ANIM_ID_RELOAD && anim->GetAnimId() != ANIM_ID_CROUCHRELOAD && anim->m_BlendDelta >= 0.f) anim->m_BlendDelta = -4.f;
    } else if (m_LastCmd == Cmd::PISTOLWHIP) {
        const auto crouched = size_t(ped->bIsDucking);
        const auto hitTime = StaticRef<std::array<float, 2>>(0xC17738)[crouched];
        if (anim->m_BlendAmount > 0.9f && anim->m_BlendDelta >= 0.f && anim->m_CurrentTime > hitTime && anim->m_CurrentTime - anim->m_TimeStep <= hitTime) {
            const auto offsetId = StaticRef<std::array<uint8, 2>>(0xC17780)[crouched];
            CVector strike = ped->GetMatrix().TransformPoint(StaticRef<CVector[12]>(0xC177D0)[offsetId]);
            CTaskSimpleFight fight{m_TargetEntity, 11, 20000};
            fight.m_nComboSet = 16;
            fight.m_nCurrentMove = eFightAttackType(crouched);
            fight.m_nLastCommand = 11;
            fight.m_pAnim = m_Anim;
            plugin::CallMethod<0x6240B0, CTaskSimpleFight*, CPed*, CVector&>(&fight, ped, strike);
            fight.m_pAnim = nullptr;
        }
        if (m_TargetEntity) ped->m_fAimingRotation = (m_TargetEntity->GetPosition() - ped->GetPosition()).Heading();
    } else if (m_LastCmd > Cmd::NONE && m_LastCmd < Cmd::RELOAD && anim->m_BlendDelta >= 0.f) {
        const bool crouched = ped->bIsDucking;
        const float loopStart = m_WeaponInfo->GetAnimLoopStart(crouched);
        const float loopEnd = m_WeaponInfo->GetAnimLoopEnd(crouched);
        const float fireTime = crouched ? m_WeaponInfo->m_fAnimLoop2Fire : m_WeaponInfo->m_fAnimLoopFire;
        const float leftFireTime = (loopEnd - loopStart) * 0.5f + fireTime;
        auto& weapon = ped->GetActiveWeapon();
        if (IsFire(m_LastCmd)) {
            if (anim->m_BlendAmount < 0.99f || weapon.GetState() == WEAPONSTATE_RELOADING) {
                if (anim->IsPlaying() && anim->m_CurrentTime >= loopStart && anim->m_CurrentTime - anim->m_TimeStep < loopStart) {
                    anim->SetFlag(ANIMATION_IS_PLAYING, false);
                    anim->SetCurrentTime(loopStart);
                }
            } else if (!anim->IsPlaying() && anim->m_CurrentTime == loopStart) {
                anim->SetFlag(ANIMATION_IS_PLAYING);
            }
        }
        if (m_WeaponInfo->flags.bContinuosFire) {
            if (IsFire(m_LastCmd) && anim->m_CurrentTime > loopStart && anim->m_CurrentTime < loopEnd && anim->IsPlaying()) {
                if (m_HasFiredGun && !(m_LastCmd == Cmd::FIREBURST && int16(m_BurstShots) > 0) && !IsFire(m_NextCmd)) {
                    anim->SetFlag(ANIMATION_IS_PLAYING, false);
                    anim->m_BlendDelta = -4.f;
                } else {
                    m_IsFiringGunRightHandThisFrame = m_HasFiredGun = true;
                    m_LastCmd = std::max(m_LastCmd, m_NextCmd);
                    m_NextCmd = Cmd::NONE;
                    if (m_LastCmd == Cmd::FIREBURST && int16(m_BurstShots) > 0) --m_BurstShots;
                    else m_BurstShots = 0;
                }
            }
        } else {
            const auto Crossed = [&](float time) { return anim->IsPlaying() && anim->m_CurrentTime > time && anim->m_CurrentTime - anim->m_TimeStep <= time; };
            if (Crossed(fireTime) && IsFire(m_LastCmd)) {
                m_IsFiringGunRightHandThisFrame = m_HasFiredGun = true;
                if (int16(m_BurstShots) > 0) --m_BurstShots;
            }
            if (m_WeaponInfo->flags.bTwinPistol && !ped->bLeftArmBlocked && Crossed(leftFireTime) && IsFire(m_LastCmd)) {
                m_IsFiringGunLeftHandThisFrame = m_HasFiredGun = true;
                if (int16(m_BurstShots) > 0) --m_BurstShots;
            }
        }

        if (!anim->IsPlaying() && (m_LastCmd != Cmd::AIM || m_NextCmd > Cmd::AIM)
            && (anim->m_BlendHier->m_fTotalTime > anim->m_CurrentTime || anim->m_BlendDelta >= 0.f)) {
            if (m_LastCmd <= Cmd::RELOAD && m_NextCmd <= Cmd::RELOAD) {
                if (anim->m_BlendAmount > 0.f && anim->m_BlendDelta >= 0.f && weapon.GetState() != WEAPONSTATE_RELOADING) anim->SetFlag(ANIMATION_IS_PLAYING);
            } else if (!m_WeaponInfo->flags.bExpands) {
                anim->m_BlendDelta = -4.f;
            } else {
                anim->SetFlag(ANIMATION_IS_PLAYING);
                if (anim->m_CurrentTime <= loopStart) anim->SetCurrentTime(loopEnd);
            }
            if (IsFire(m_NextCmd)) {
                m_LastCmd = m_NextCmd;
                m_NextCmd = Cmd::NONE;
                if (m_LastCmd == Cmd::FIREBURST) m_BurstShots = m_BurstLength;
            } else if (m_LastCmd == Cmd::AIM && m_NextCmd != Cmd::AIM) {
                m_LastCmd = Cmd::NONE;
            }
        } else if (m_LastCmd == Cmd::AIM && anim->IsPlaying()) {
            if ((anim->m_CurrentTime >= loopStart && anim->m_CurrentTime - anim->m_TimeStep < loopStart) || anim->m_CurrentTime + anim->m_TimeStep >= loopStart) {
                anim->SetFlag(ANIMATION_IS_PLAYING, false);
                anim->SetCurrentTime(loopStart);
            }
        }
        if (anim->m_CurrentTime > loopEnd && anim->m_CurrentTime - anim->m_TimeStep <= loopEnd) {
            if (IsFire(m_NextCmd) || (m_LastCmd == Cmd::FIREBURST && int16(m_BurstShots) > 0 && m_NextCmd != Cmd::RELOAD)) {
                anim->SetCurrentTime(loopStart);
                anim->SetFlag(ANIMATION_IS_PLAYING, weapon.GetState() != WEAPONSTATE_RELOADING);
                if (IsFire(m_NextCmd)) {
                    m_LastCmd = std::max(m_LastCmd, m_NextCmd);
                    if (m_NextCmd == Cmd::FIREBURST && m_BurstShots == 0) m_BurstShots = m_BurstLength;
                    m_NextCmd = Cmd::NONE;
                }
            } else if (m_NextCmd == Cmd::AIM) {
                anim->SetCurrentTime(loopStart);
                anim->SetFlag(ANIMATION_IS_PLAYING, false);
                m_LastCmd = Cmd::AIM;
                m_NextCmd = Cmd::NONE;
            }
        }
        if (anim->m_CurrentTime > m_WeaponInfo->m_fBreakoutTime && m_NextCmd == Cmd::END_NOW) {
            m_IsFinished = true;
            anim->m_BlendDelta = anim->HasFlag(ANIMATION_DONT_ADD_TO_PARTIAL_BLEND) ? -1.f : -4.f;
        }
    } else if (m_LastCmd == Cmd::NONE && anim->m_BlendAmount > 0.f && anim->m_BlendDelta >= 0.f) {
        anim->m_BlendDelta = -4.f;
    }

    m_SkipAim = false;
    if (ped->m_pedIK.bUseArm) {
        auto target = GetAimTargetPosition(ped);
        const bool deadTarget = m_TargetEntity && m_TargetEntity->GetIsTypePed() && m_TargetEntity->AsPed()->m_fHealth <= 0.f;
        if (target.x != 0.f || target.y != 0.f) {
            target -= ped->GetPosition();
            target.Normalise();
            auto angle = target.Heading() - ped->m_fCurrentRotation;
            if (angle > PI) angle -= TWO_PI;
            else if (angle < -PI) angle += TWO_PI;
            const float adjustment = deadTarget ? DegreesToRadians(40.f) : 0.f;
            if (angle > DegreesToRadians(115.f) - adjustment || angle < -DegreesToRadians(130.f) + adjustment) m_SkipAim = true;
            if (deadTarget && target.Dot(ped->GetUpVector()) < -0.8f) m_SkipAim = true;
        }
        if (!ped->bIsDucking && ped->bRightArmBlocked) m_SkipAim = true;
    }
    if (m_LastCmd == Cmd::NONE || m_LastCmd >= Cmd::RELOAD || m_SkipAim) AbortIK(ped);
    else AimGun(ped);
    return EndControl();
}

// 0x624ED0
bool CTaskSimpleUseGun::SetPedPosition(CPed* ped) {
    if (m_WeaponInfo == &ped->GetActiveWeapon().GetWeaponInfo(ped)) {
        if (m_IsFiringGunRightHandThisFrame) { // Right hand
            FireGun(ped, false);
        }
        if (m_IsFiringGunLeftHandThisFrame) { // Left hand
            FireGun(ped, true);
        }
        reinterpret_cast<uint8*>(this)[0xD] = 0; // Como el original: se pone a cero el byte entero de los bits de disparo
    }
    return false;
}

// notsa
CVector CTaskSimpleUseGun::GetAimTargetPosition(CPed* ped) const {
    if (m_TargetEntity) {
        if (m_TargetEntity->GetIsTypePed()) {
            return m_TargetEntity->AsPed()->GetBonePosition(BONE_SPINE1);
        }
        return m_TargetEntity->GetPosition();
    }
    if (ped->IsPlayer() && TheCamera.m_aCams[0].m_nMode == MODE_AIMWEAPON) {
        CVector src, target;
        TheCamera.Find3rdPersonCamTargetVector(20.f, ped->GetPosition() + CVector{0.f, 0.f, 0.7f}, src, target);
        return target;
    }
    return m_TargetPos;
}

// notsa
std::pair<CVector, eBoneTag> CTaskSimpleUseGun::GetAimLookAtInfo() const {
    if (m_TargetEntity->GetIsTypePed()) {
        const auto targetPed = m_TargetEntity->AsPed();
        if (const auto pd = targetPed->GetPlayerData()) {
            CVector ret = pd->m_vecTargetBoneOffset;
            targetPed->GetTransformedBonePosition(ret, (eBoneTag)pd->m_nTargetBone);
            return {ret, (eBoneTag)pd->m_nTargetBone};
        }
    }
    return {m_TargetEntity->GetPosition(), BONE_SPINE1};
}

// 0x624DC0
void CTaskSimpleUseGun::Reset(CPed* ped, CEntity* targetEntity, CVector targetPos, eGunCommand, int16 burstLength) {
    AbortIK(ped);

    m_TargetPos       = targetPos;
    m_BurstLength     = burstLength;
    m_BurstShots      = 0;
    m_CountDownFrames = -1;
    m_IsLOSBlocked    = false;
    m_IsArmIKInUse    = false;
    m_IsLookIKInUse   = false;
    m_TargetEntity    = targetEntity;
}

// 0x61E3F0
void CTaskSimpleUseGun::SetMoveAnim(CPed* ped) {
    const auto
        animGunStand   = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_GUN_STAND),
        animGunMoveFwd = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_GUNMOVE_FWD),
        animGunMoveL   = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_GUNMOVE_L),
        animGunMoveBwd = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_GUNMOVE_BWD),
        animGunMoveR   = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_GUNMOVE_R);

    // 0x61E444
    if (ped->bIsDucking) {
        if (const auto tDuck = ped->GetIntelligence()->GetTaskDuck()) {
            switch (m_LastCmd) {
            case eGunCommand::FIRE:
            case eGunCommand::FIREBURST:
            case eGunCommand::RELOAD:
                tDuck->ForceStopMove();
            }
        }
        m_MoveCmd = {0.f, 0.f};
        m_HasMoveControl = false;
        return;
    }

    // 0x61E4A5
    const auto isInMoveControl = m_HasMoveControl && (m_LastCmd != eGunCommand::FIRE || m_WeaponInfo->flags.bMoveFire);
    const auto moveCmdDist = isInMoveControl
        ? m_MoveCmd.Magnitude()
        : 0.f;
    const auto moveCmdAbsSum = isInMoveControl
        ? std::abs(m_MoveCmd.x) + std::abs(m_MoveCmd.y)
        : 0.f;

    const auto DoBlendAnim = [ped](AnimationId animId, float blendDelta) {
        return CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, animId, blendDelta);
    };

    // 0x61E505
    if (moveCmdAbsSum < 0.1f) {
        CAnimBlendAssociation* moveAnim;
        if (m_WeaponInfo->flags.bAimWithArm) {
            ped->SetMoveState(PEDMOVE_STILL); // Como el original
            // Como el original: banda = tipos 7..15 (GANG1..GANG9, no GANG10) o policia; el reposo usa el grupo del peaton
            const auto pedType = (int32)ped->m_nPedType;
            moveAnim = (pedType >= (int32)PED_TYPE_GANG1 && pedType <= (int32)PED_TYPE_GANG9) || pedType == (int32)PED_TYPE_COP
                ? DoBlendAnim(ANIM_ID_GANG_GUNSTAND, 4.f) // Why is this used for cops? (0x61E529, inverted)
                : CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 8.f);
        } else {
            moveAnim = DoBlendAnim(ANIM_ID_GUN_STAND, 8.f);
        }

        // Como el original: primero se paran las cuatro (adelante, izquierda, atras, derecha) y despues se reinicia su tiempo
        const std::array<CAnimBlendAssociation*, 4> moveAnims{ animGunMoveFwd, animGunMoveL, animGunMoveBwd, animGunMoveR };
        for (const auto anim : moveAnims) {
            if (anim) {
                anim->SetFlag(ANIMATION_IS_PLAYING, false);
            }
        }
        if (moveAnim && moveAnim->m_BlendAmount > 0.95f) {
            for (const auto anim : moveAnims) {
                if (anim) {
                    anim->SetCurrentTime(0.f);
                }
            }
        }

        m_MoveCmd = { 0.f, 0.f }; // Como el original (a muchos FPS el mando no llega a 0.1 y no se puede mover apuntando)
        m_HasMoveControl = false;

        return; // 0x61E5F3
    }

    // 0x61E5F6 - Blend in new move anim according to the move command
    if (m_WeaponInfo->flags.bAimWithArm) {
        const auto moveAnim = DoBlendAnim(
            m_MoveCmd.x > 0.75f
                ? ANIM_ID_GUNMOVE_R
                : m_MoveCmd.x < -0.75f // Como el original: estrictamente menor
                    ? ANIM_ID_GUNMOVE_L
                    : m_MoveCmd.y > 0.f // Como el original: con NaN, adelante
                        ? ANIM_ID_GUNMOVE_BWD
                        : ANIM_ID_GUNMOVE_FWD,
            8.f
        );
        moveAnim->SetFlag(ANIMATION_IS_PLAYING, true);
        ped->SetMoveState(PEDMOVE_NONE);
        return; // 0x61E675
    }

    //
    // Handle anim transition defined by the current move command
    //

    // 0x61E67C - Calculate lerp progress
    float t = 1.f;
    if (const auto aGS = animGunStand) {
        // 0x61E682 - Adjust blend amount
        if (aGS->GetBlendDelta() >= 0.f) {
            aGS->SetBlendDelta(0.f);
            // Como el original: solo se satura a 0 si es menor (NaN y -0 se conservan)
            const float amount = aGS->m_BlendAmount - CTimer::GetTimeStep() * 0.16f;
            aGS->SetBlendAmount(0.f > amount ? 0.f : amount);
        }

        // Calculate lerp t
        t = 1.f - std::clamp(CTimer::GetTimeStep() * aGS->GetBlendDelta() * 0.02f + aGS->GetBlendAmount(), 0.f, 1.f);
    }

    // Process transition from one gun move to another
    const auto DoTransitionGunMoveAnim = [&, ped, this](CAnimBlendAssociation* fromAnim, CAnimBlendAssociation* toAnim, AnimationId toAnimId, float toAnimBlendAmnt) {
        // If previous anim exists, blend it out
        if (fromAnim) {
            fromAnim->SetBlendAmount(0.f);
            fromAnim->SetBlendDelta(0.f);
        }

        // If new anim doesn't exists, create it
        if (!toAnim) {
            toAnim = CAnimManager::AddAnimation(ped->GetRpClump(),ANIM_GROUP_DEFAULT, toAnimId);
        }

        // Set params of the anim we're transitioning to
        toAnim->SetBlendDelta(0.f);
        toAnim->SetBlendAmount(toAnimBlendAmnt);
        toAnim->SetFlag(ANIMATION_IS_PLAYING, true);
        toAnim->SetSpeed(m_WeaponInfo->m_fMoveSpeed * moveCmdDist);
    };

    const auto moveCmdLerp = m_MoveCmd * (1.f / moveCmdAbsSum) * t;

    // 0x61E73F - Process X axis (left, right)
    if (moveCmdLerp.x < 0.f) { // right => left
        DoTransitionGunMoveAnim(animGunMoveR, animGunMoveL, ANIM_ID_GUNMOVE_L, std::abs(moveCmdLerp.x));
    } else if (moveCmdLerp.x > 0.f) { // left => right
        DoTransitionGunMoveAnim(animGunMoveL, animGunMoveR, ANIM_ID_GUNMOVE_R, std::abs(moveCmdLerp.x));
    }

    // 0x61E7E5 - Process Y axis (fwd, bwd)
    if (moveCmdLerp.y < 0.f) { // bwd => fwd
        DoTransitionGunMoveAnim(animGunMoveBwd, animGunMoveFwd, ANIM_ID_GUNMOVE_FWD, std::abs(moveCmdLerp.y));
    } else if (moveCmdLerp.y > 0.f) { // fwd => bwd
        DoTransitionGunMoveAnim(animGunMoveFwd, animGunMoveBwd, ANIM_ID_GUNMOVE_BWD, std::abs(moveCmdLerp.y));
    }

    // 0x61E88B
    if (m_LastCmd == eGunCommand::AIM) {
        if (m_Anim && m_Anim->GetBlendAmount() > 0.f && m_Anim->GetBlendDelta() >= 0.f && !m_WeaponInfo->flags.bMoveAim) {
            m_Anim->SetBlendDelta(-4.f);
        }
    }

    m_HasMoveControl = false;
}

// 0x624F30
void CTaskSimpleUseGun::StartAnim(CPed* ped) {
    const auto tDuck = ped->bIsDucking
        ? ped->GetIntelligence()->GetTaskDuck()
        : nullptr;

    if (m_Anim) {
        if (m_NextCmd == eGunCommand::END_NOW && m_LastCmd <= eGunCommand::FIREBURST) {
            if (m_Anim->GetBlendDelta() > -8.f && m_Anim->GetBlendAmount() > 0.f) {
                m_Anim->SetBlendDelta(-8.f);
            }
        }
        m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
    }

    switch (m_NextCmd) {                      
    case eGunCommand::AIM:
    case eGunCommand::FIRE:
    case eGunCommand::FIREBURST: { // 0x624FC1
        if (m_NextCmd == eGunCommand::AIM) {
            if (!m_MoveCmd.IsZero() && !m_WeaponInfo->flags.bMoveFire || tDuck && tDuck->IsTaskInUseByOtherTasks()) { // 0x624FC3 - Check if we can fire right now
                switch (m_LastCmd) {
                case eGunCommand::FIRE:
                case eGunCommand::FIREBURST:
                    m_LastCmd = eGunCommand::AIM;
                }
                return; // Nope, we can't fire right now, so continue aiming
            }
        } else if (tDuck && tDuck->StopFireGun()) {
            return;
        }

        if (m_NextCmd == eGunCommand::FIREBURST) {
            m_BurstShots = m_BurstLength;
        }

        m_Anim = CAnimManager::BlendAnimation( // 0x62503A
            ped->GetRpClump(),
            m_WeaponInfo->m_eAnimGroup,
            ped->bIsDucking && m_WeaponInfo->flags.bCrouchFire
                ? ANIM_ID_CROUCHFIRE
                : ANIM_ID_FIRE
        );

        if (m_LastCmd == eGunCommand::RELOAD) {
            if (m_WeaponInfo->flags.bReload2Start) {
                m_Anim->SetCurrentTime(m_WeaponInfo->GetAnimLoopStart(ped->bIsDucking));
                m_Anim->SetFlag(ANIMATION_IS_PLAYING, false);
            }
        }

        m_Anim->SetFinishCallback(FinishGunAnimCB, this);

        break;
    }
    case eGunCommand::RELOAD: { // 0x6250E5
        if (tDuck && tDuck->StopFireGun()) {
            return;
        }

        if (m_LastCmd != eGunCommand::RELOAD) {
            m_BurstShots = m_WeaponInfo->flags.bTwinPistol ? 2 : 1;
        }

        if (int16(m_BurstShots) > 0) { // 0x625196 - Inverted (Como el original: con signo)
            m_Anim = CAnimManager::BlendAnimation( // 0x62511C
                ped->GetRpClump(),
                m_WeaponInfo->m_eAnimGroup,
                ped->bIsDucking && m_WeaponInfo->flags.bCrouchFire
                    ? ANIM_ID_CROUCHRELOAD
                    : ANIM_ID_RELOAD
            );
            m_Anim->Start();
            m_Anim->SetFinishCallback(FinishGunAnimCB, this);

            m_BurstShots--;
        } else {
            m_NextCmd = eGunCommand::NONE;
        }

        break;
    }
    case eGunCommand::PISTOLWHIP: {
        if (tDuck && tDuck->IsTaskInUseByOtherTasks()) { // Como el original: 0x61C3D0, no StopFireGun
            return;
        }
        m_Anim = CAnimManager::BlendAnimation( // 0x6251DC
            ped->GetRpClump(),
            CTaskSimpleFight::m_aComboData.data()[12].m_nAnimGroup, // Como el original: lee justo despues de la tabla (0xC17730)
            ped->bIsDucking
                ? ANIM_ID_FIGHT_2
                : ANIM_ID_FIGHT_1
        );
        m_Anim->SetFinishCallback(FinishGunAnimCB, this);
        break;
    }
    case eGunCommand::NONE:
    case eGunCommand::END_LEISURE: { // 0x62520A [Moved here for readability]
        RemoveStanceAnims(ped, 4.f);
        m_IsFinished = true;
        break;
    }
    case eGunCommand::END_NOW: { // 0x625214
        RemoveStanceAnims(ped, 8.f);
        m_IsFinished = true;
        break;
    }
    }

    m_LastCmd = std::exchange(m_NextCmd, eGunCommand::NONE);
}

// 0x0x61E160
void CTaskSimpleUseGun::StartCountDown(uint8 numIdleFrames, char isMax) {
    m_CountDownFrames = isMax ? std::min(numIdleFrames, m_CountDownFrames) : numIdleFrames;
    m_HasFiredGun     = false;
}
