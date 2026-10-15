/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePlayerOnFoot
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

#include "TaskSimplePlayerOnFoot.h"

#include "TaskSimpleUseGun.h"
#include "TaskSimpleThrowProjectile.h"

#include "TaskComplexJump.h"
#include "TaskComplexUseGoggles.h"
#include "TaskSimpleStealthKill.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleFight.h"
#include "TaskSimpleDuck.h"
#include "X87Math.h"

auto& gLastRandomNumberForIdleAnimationID = StaticRef<int32>(0x8D2FEC);
auto& gLastTouchTimeDelta = StaticRef<uint32>(0xC19664);
auto& gDuckAnimBlendData = StaticRef<float>(0x8D2FF0); // 4.0f
auto& gbUnknown_8D2FE8 = StaticRef<bool>(0x8D2FE8);     // default value true; also always true

void CTaskSimplePlayerOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayerOnFoot, 0x8708ec, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x685750);
    RH_ScopedInstall(Destructor, 0x6857D0);

    RH_ScopedInstall(PlayerControlZeldaWeapon, 0x687C20);
    RH_ScopedInstall(PlayerControlDucked, 0x687F30);

    RH_ScopedInstall(ProcessPlayerWeapon, 0x6859A0);
    RH_ScopedInstall(PlayIdleAnimations, 0x6872C0);
    RH_ScopedInstall(PlayerControlFighter, 0x687530);
    RH_ScopedInstall(PlayerControlZelda, 0x6883D0);

    RH_ScopedVMTInstall(Clone, 0x68AFF0);
    RH_ScopedVMTInstall(GetTaskType, 0x6857C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6857E0);
    RH_ScopedVMTInstall(ProcessPed, 0x688810);
}

// 0x685750
CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot() : CTaskSimple() {
    m_PlayerIdlesAnimBlockId = CAnimManager::GetAnimationBlockIndex("playidles");
    dword_14 = 0;
    m_nTimer = 0;
    m_pLookingAtEntity = nullptr;
    m_nFrameCounter = CTimer::GetFrameCounter();
}

// 0x6857E0
bool CTaskSimplePlayerOnFoot::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    auto eventDamage = static_cast<const CEventDamage*>(event);
    bool abortable = false;
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        ped->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        abortable = true;
    }
    else if (priority == ABORT_PRIORITY_URGENT) {
        if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            abortable = true;
        } else {
            if (event) {
                if (event->GetEventPriority() < 61)
                    return false;
                if (event->GetEventType() == EVENT_DAMAGE) {
                    auto secondary = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK);
                    if (secondary->GetTaskType() == TASK_SIMPLE_THROW_PROJECTILE || eventDamage->m_bFallDown) {
                        ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
                    }
                    abortable = true;
                }
            }
            if (!abortable) {
                if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                    abortable = true;
                } else {
                    abortable = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
                }
            }
        }
    }
    if (abortable) {
        if ((ped->m_pTargetedObject || (ped->GetPlayerData()->m_nPlayerFlags & 8) != 0 || TheCamera.Using1stPersonWeaponMode()) && event &&
            (event->GetEventType() == EVENT_DAMAGE || event->GetEventType() == EVENT_IN_WATER)) {
            if (event->GetEventType() != EVENT_DAMAGE || eventDamage->m_damageResponse.m_bHealthZero && eventDamage->m_bAddToEventGroup ||
                !ped->m_pAttachedTo && (eventDamage->m_bFallDown || eventDamage->m_weaponType > WEAPON_LAST_WEAPON && eventDamage->m_weaponType != WEAPON_UZI_DRIVEBY)) {
                TheCamera.ClearPlayerWeaponMode();
                CWeaponEffects::ClearCrossHair(ped->m_nPedType);
                CEntity::ClearReference(ped->m_pTargetedObject);
            }
        }
    }
    return abortable;
}

// 0x688810
bool CTaskSimplePlayerOnFoot::ProcessPed(CPed* ped) {
    auto player = static_cast<CPlayerPed*>(ped);

    if (player->GetPadFromPlayer()) {
        CPedIntelligence* intelligence = player->GetIntelligence();
        bool bPedMoving = player->m_nMoveState >= PEDMOVE_WALK;
        if (player->GetActiveWeapon().m_Type == WEAPON_CHAINSAW) {
            if (const auto fightingTask = intelligence->GetTaskFighting()) {
                if (fightingTask->m_nCurrentMove == FIGHT_ATTACK_FIGHTIDLE) {
                    bPedMoving = true;
                }
            }
        }
        player->SetMoveState(PEDMOVE_STILL);
        if (player->bIsDucking) {
            PlayerControlDucked(player);
        } else if (!intelligence->GetTaskFighting() || bPedMoving) {
            CTaskSimpleUseGun* simpleTaskUseGun = intelligence->GetTaskUseGun();
            if (simpleTaskUseGun && simpleTaskUseGun->m_WeaponInfo && !simpleTaskUseGun->m_WeaponInfo->flags.bAimWithArm) {
                PlayerControlZeldaWeapon(player);
            } else {
                PlayerControlZelda(player, false);
            }
        } else {
            PlayerControlFighter(player);
        }

        ProcessPlayerWeapon(player);
    }

    m_nFrameCounter = CTimer::GetFrameCounter();
    return false;
}

namespace {
// Como el original: la entrada del banco de armas se indexa con movsx del byte de la ranura (sin comprobar el rango).
CWeapon& ActiveWeaponOf(CPlayerPed* player) {
    return player->m_aWeapons.data()[static_cast<int8>(player->m_nActiveWeaponSlot)];
}

// Como el original (0x59C890, MultiplyMatrixWithVector): el orden de las sumas es el del EXE (x: up, forward, right; y y z: up, right, forward) y luego la posicion.
// CMatrix::TransformPoint suma right, forward, up y puede redondear distinto.
CVector MultiplyMatrixWithVectorX87(const CMatrix& m, const CVector& v) {
    const CVector& r = m.GetRight();
    const CVector& f = m.GetForward();
    const CVector& u = m.GetUp();
    const CVector& p = m.GetPosition();
    CVector result;
    result.x = ((u.x * v.z + f.x * v.y) + r.x * v.x) + p.x;
    result.y = ((u.y * v.z + r.y * v.x) + f.y * v.y) + p.y;
    result.z = ((u.z * v.z + r.z * v.x) + f.z * v.y) + p.z;
    return result;
}
} // namespace

// 0x6859A0
void CTaskSimplePlayerOnFoot::ProcessPlayerWeapon(CPlayerPed* player) {
    CPlayerPedData* playerData = player->GetPlayerData();
    CPedIntelligence* intelligence = player->GetIntelligence();
    CTaskManager* taskManager = &player->GetTaskManager();
    CPad* pad = player->GetPadFromPlayer();

    eWeaponType weaponType = ActiveWeaponOf(player).m_Type;
    eWeaponSkill weaponSkill = player->GetWeaponSkill();
    CWeaponInfo* weaponInfo = CWeaponInfo::GetWeaponInfo(weaponType, weaponSkill);

    if (playerData->m_bHaveTargetSelected && !player->m_pTargetedObject) {
        TheCamera.ClearPlayerWeaponMode();
        CWeaponEffects::ClearCrossHair(player->m_nPedType);
    }

    player->m_nWeaponAccuracy = weaponInfo->flags.bCanAim ? 95 : 100;

    if (pad->WeaponJustDown(player) && (player->m_pTargetedObject || CCamera::m_bUseMouse3rdPerson && player->m_p3rdPersonMouseTarget)) {
        player->PlayerHasJustAttackedSomeone();
    }

    if (player->m_pFire || !weaponInfo->flags.b1stPerson) {
        // Como el original (0x685BAA): el tipo del arma se vuelve a leer de la ranura activa
        if (!pad->GetTarget() && ActiveWeaponOf(player).m_Type == WEAPON_RLAUNCHER_HS) {
            playerData->m_nFireHSMissilePressedTime = 0;
            playerData->m_LastHSMissileTarget = nullptr;
        }
    } else {
        if (pad->GetEnterTargeting() || TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget || pad->GetTarget() && m_nFrameCounter < (CTimer::GetFrameCounter() - 1)) {
            auto weaponMode = eCamMode::MODE_NONE;
            switch (ActiveWeaponOf(player).m_Type) {
            case WEAPON_RLAUNCHER: {
                weaponMode = MODE_ROCKETLAUNCHER;
                break;
            }
            case WEAPON_RLAUNCHER_HS: {
                playerData->m_nFireHSMissilePressedTime = CTimer::GetTimeInMS();
                playerData->m_LastHSMissileTarget = nullptr;
                weaponMode = MODE_ROCKETLAUNCHER_HS;
                break;
            }
            case WEAPON_SNIPERRIFLE: {
                weaponMode = MODE_SNIPER;
                break;
            }
            case WEAPON_CAMERA: {
                weaponMode = MODE_CAMERA;
                break;
            }
            default: {
                weaponMode = MODE_M16_1STPERSON;
                break;
            }
            }
            TheCamera.SetNewPlayerWeaponMode(weaponMode, 0, 0);
            player->SetPedState(PEDSTATE_SNIPER_MODE);
            return;
        }

        if (!TheCamera.Using1stPersonWeaponMode()) {
            weaponType = ActiveWeaponOf(player).m_Type;
            if (weaponType == WEAPON_RLAUNCHER || weaponType == WEAPON_RLAUNCHER_HS || weaponType == WEAPON_SNIPERRIFLE || weaponType == WEAPON_CAMERA) {
                // Como el original (0x685B0B..0x685B25): dos consultas de GetTaskUseGun
                if (intelligence->GetTaskUseGun()) {
                    intelligence->GetTaskUseGun()->PlayerPassiveControlGun();
                }
                player->GetPlayerData()->m_bHaveTargetSelected = 0;
                return;
            }
        }
    }

    bool handleFighting = false;
    CTaskSimpleUseGun* newSimpleUseGunTask = nullptr;
    if (weaponInfo->m_nWeaponFire == WEAPON_FIRE_MELEE) {
        int32 fightCommand = 0;
        bool executeMeleeAttack = false;
        if (!player->m_pTargetedObject && !pad->GetTarget() && !taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            if (pad->MeleeAttackJustDown(false)) {
                fightCommand = 11;
            }

            CAnimBlendAssociation* animAssoc = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_KILL_PARTIAL);
            if (animAssoc) {
                animAssoc->m_BlendAmount = -2.0f;
            }

            if (fightCommand == 0) {
                handleFighting = true;
            } // fightCommand cannot be 19 here, so we don't need the code here.
            else if (fightCommand == 19) {
                // Just in case, if this executes somehow, then we probably need to add the code.
                // But It won't.
                assert(fightCommand != 19);
                // LAB_00685c62:
            } else {
                executeMeleeAttack = true;
            }
        } else {
            CPed* targetEntity = nullptr;
            if (!player->m_pTargetedObject) {
                if (CCamera::m_bUseMouse3rdPerson && player->m_p3rdPersonMouseTarget) {
                    targetEntity = player->m_p3rdPersonMouseTarget;
                }
            } else {
                if (player->m_pTargetedObject->GetIsTypePed()) {
                    targetEntity = reinterpret_cast<CPed*>(player->m_pTargetedObject);
                }
            }

            CAnimBlendAssociation* animAssociation = nullptr;
            AssocGroupId animGroupID = weaponInfo->m_eAnimGroup;
            if (targetEntity && pad->GetTarget() && playerData->m_fMoveBlendRatio < 1.9f && player->m_nMoveState != PEDMOVE_SPRINT &&
                !taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK) && animGroupID != ANIM_GROUP_DEFAULT && CAnimManager::GetAnimationBlock(animGroupID) &&
                CAnimManager::GetAnimationBlock(animGroupID)->IsLoaded && intelligence->TestForStealthKill(targetEntity, false)) {
                if (player->bIsDucking) {
                    CTaskSimpleDuck* duckTask = intelligence->GetTaskDuck(true);
                    if (duckTask && intelligence->GetTaskDuck(true)->IsTaskInUseByOtherTasks()) {
                        animAssociation = RpAnimBlendClumpGetFirstAssociation(player->GetRpClump());
                    } else {
                        animAssociation = CAnimManager::BlendAnimation(player->GetRpClump(), weaponInfo->m_eAnimGroup, ANIM_ID_KILL_PARTIAL, 8.0f);
                    }
                } else {
                    animAssociation = CAnimManager::BlendAnimation(player->GetRpClump(), weaponInfo->m_eAnimGroup, ANIM_ID_KILL_PARTIAL, 8.0f);
                }
            } else {
                animAssociation = RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_KILL_PARTIAL);
                if (animAssociation) {
                    animAssociation->m_BlendAmount = -2.0f;
                }
            }

            bool bCheckButtonCircleStateOnly = false;
            if (player->m_pTargetedObject || pad->GetTarget()) {
                bCheckButtonCircleStateOnly = true;
            }
            uint8 meleeAttackJustDown = pad->MeleeAttackJustDown(bCheckButtonCircleStateOnly);
            if (meleeAttackJustDown && animAssociation && animAssociation->m_BlendAmount > 0.5f && targetEntity && intelligence->TestForStealthKill(targetEntity, true)) {
                auto* pTaskSimpleStealthKill = new CTaskSimpleStealthKill(true, targetEntity, weaponInfo->m_eAnimGroup);
                taskManager->SetTask(pTaskSimpleStealthKill, TASK_PRIMARY_PRIMARY, false);

                eWeaponType activeWeaponType = ActiveWeaponOf(player).m_Type;
                CPedDamageResponseCalculator damageCalculator(player, 0.0f, activeWeaponType, PED_PIECE_TORSO, false);
                CEventDamage eventDamage(player, CTimer::GetTimeInMS(), activeWeaponType, PED_PIECE_TORSO, 0, false, targetEntity->bInVehicle);
                if (eventDamage.AffectsPed(targetEntity)) {
                    damageCalculator.ComputeDamageResponse(targetEntity, eventDamage.m_damageResponse, false);
                    targetEntity->GetEventGroup().Add(&eventDamage, false);
                    CCrime::ReportCrime(eCrimeType::CRIME_STAB_PED, targetEntity, player);
                    player->GetWeaponAE().AddAudioEvent(AE_WEAPON_STEALTH_KILL);
                }
                player->ClearWeaponTarget();
            } else {
                switch (meleeAttackJustDown) {
                case 1: {
                    fightCommand = 11;
                    break;
                }
                case 4: {
                    if (!CWeaponInfo::GetWeaponInfo(ActiveWeaponOf(player).m_Type, eWeaponSkill::STD)->flags.bHeavy) {
                        fightCommand = 12;
                    } else {
                        fightCommand = 11;
                    }
                    break;
                }
                case 3: {
                    fightCommand = 2;
                    break;
                }
                default: {
                    if (pad->GetMeleeAttack(false) && ActiveWeaponOf(player).m_Type == WEAPON_CHAINSAW && taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                        fightCommand = 11;
                    } else {
                        handleFighting = true;
                    }
                }
                };
                if (!handleFighting)
                    executeMeleeAttack = true;
            }
        }

        if (executeMeleeAttack) {
            if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                if (auto taskSimpleFight = intelligence->GetTaskFighting())
                    taskSimpleFight->ControlFight(player->m_pTargetedObject, fightCommand);
            } else {
                auto taskSimpleFight = new CTaskSimpleFight(player->m_pTargetedObject, fightCommand, 2000u);
                taskManager->SetTaskSecondary(taskSimpleFight, TASK_SECONDARY_ATTACK);
            }
        }
    } else {
        if (weaponInfo->m_nWeaponFire == WEAPON_FIRE_USE) {
            if (pad->WeaponJustDown(nullptr)) {
                CWeapon* playerWeapon = &ActiveWeaponOf(player);
                weaponType = playerWeapon->m_Type;
                if (weaponType == WEAPON_DETONATOR) {
                    playerWeapon->Fire(player, &player->GetPosition(), &player->GetPosition(), nullptr, nullptr, nullptr);
                } else if (weaponType > WEAPON_CAMERA && weaponType <= WEAPON_INFRARED && !taskManager->GetTaskPrimary(TASK_PRIMARY_PRIMARY)) {
                    auto* pCTaskComplexUseGoggles = new CTaskComplexUseGoggles();
                    taskManager->SetTask(pCTaskComplexUseGoggles, TASK_PRIMARY_PRIMARY, false);
                    player->GetPlayerData()->m_bDontAllowWeaponChange = true;
                }
            }
        } else {
            if (!pad->GetWeapon(player)) {
                if (intelligence->GetTaskThrow()) {
                    auto pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK));
                    pTaskSimpleThrowProjectile->ControlThrow(true, nullptr, nullptr);
                }
            } else {
                uint32 nWeaponFire = weaponInfo->m_nWeaponFire;
                if (player->m_nMoveState != PEDMOVE_SPRINT && playerData->m_nChosenWeapon == player->m_nActiveWeaponSlot && (nWeaponFire - 1) < 4) {
                    switch (nWeaponFire) {
                    case WEAPON_FIRE_INSTANT_HIT:
                    case WEAPON_FIRE_AREA_EFFECT: {
                        CEntity* targetedObject = player->m_pTargetedObject;
                        // Como el original (0x6862D4..0x68632D): con el arma en estado 2 solo se apunta si hay boton de apuntar, objetivo o apuntado libre;
                        // en otro caso se salta todo el control del arma (0x68631D `je 6860AD`).
                        eGunCommand gunCmd = eGunCommand::FIRE;
                        bool skipGunControl = false;
                        if (CTaskSimpleUseGun::RequirePistolWhip(player, targetedObject)) {
                            gunCmd = eGunCommand::PISTOLWHIP;
                        } else if (ActiveWeaponOf(player).m_State == 2) {
                            if (pad->GetTarget() || player->m_pTargetedObject || playerData->m_bFreeAiming) {
                                gunCmd = eGunCommand::AIM;
                            } else {
                                skipGunControl = true;
                            }
                        }
                        if (!skipGunControl) {
                            CTask* secondaryTask = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                            if (secondaryTask) {
                                if (secondaryTask->GetTaskType() == TASK_SIMPLE_USE_GUN) {
                                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                                    if (taskUseGun) {
                                        taskUseGun->ControlGun(player, targetedObject, gunCmd);
                                    }
                                } else {
                                    secondaryTask->MakeAbortable(player);
                                }
                            } else {
                                auto* taskUseGun = new CTaskSimpleUseGun(targetedObject, CVector(0.0f, 0.0f, 0.f), gunCmd, 1, false);
                                taskManager->SetTaskSecondary(taskUseGun, TASK_SECONDARY_ATTACK);
                                player->GetPlayerData()->m_fAttackButtonCounter = 0;
                            }
                            if (!pad->GetTarget()) {
                                if (ActiveWeaponOf(player).m_Type == WEAPON_EXTINGUISHER) {
                                    playerData->m_fLookPitch = -CWeapon::ms_fExtinguisherAimAngle;
                                } else {
                                    playerData->m_fLookPitch = 0.0f;
                                }
                            }
                        }
                        break;
                    }
                    case WEAPON_FIRE_PROJECTILE: {
                        CWeapon* activeWeapon = &ActiveWeaponOf(player);
                        if (activeWeapon->m_Type == WEAPON_RLAUNCHER || activeWeapon->m_Type == WEAPON_RLAUNCHER_HS) {
                            const auto gunCmd = activeWeapon->m_State == WEAPONSTATE_RELOADING
                                ? eGunCommand::AIM
                                : eGunCommand::FIRE;
                            CTask* secondaryTask = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                            if (secondaryTask) {
                                if (secondaryTask->GetTaskType() == TASK_SIMPLE_USE_GUN) {
                                    if (intelligence->GetTaskUseGun()) {
                                        auto* taskUseGun = static_cast<CTaskSimpleUseGun*>(secondaryTask);
                                        taskUseGun->ControlGun(player, player->m_pTargetedObject, gunCmd);
                                    }
                                } else {
                                    taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(player);
                                }
                            } else {
                                auto* taskUseGun = new CTaskSimpleUseGun(player->m_pTargetedObject, CVector(0.0f, 0.0f, 0.0f), gunCmd, 1, false);
                                taskManager->SetTaskSecondary(taskUseGun, TASK_SECONDARY_ATTACK);
                            }
                        } else {
                            if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK) || !pad->WeaponJustDown(player)) {
                                CTask* secondaryTask = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                                if (secondaryTask && secondaryTask->GetTaskType() != TASK_SIMPLE_THROW_PROJECTILE) {
                                    secondaryTask->MakeAbortable(player);
                                } else if (intelligence->GetTaskThrow()) {
                                    auto pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(secondaryTask);
                                    pTaskSimpleThrowProjectile->ControlThrow(pad->WeaponJustDown(player), nullptr, nullptr);
                                }
                            } else {
                                auto* pTaskSimpleThrowProjectile = new CTaskSimpleThrowProjectile(nullptr, CVector(0.0f, 0.0f, 0.0f));
                                taskManager->SetTaskSecondary(pTaskSimpleThrowProjectile, TASK_SECONDARY_ATTACK);
                            }
                        }
                        break;
                    }
                    case WEAPON_FIRE_CAMERA: {
                        CWeapon* activeWeapon = &ActiveWeaponOf(player);
                        if (CCamera::GetActiveCamera().m_nMode == MODE_CAMERA && CTimer::GetTimeInMS() > activeWeapon->m_TimeForNextShotMs) {
                            CVector firingPoint(0.0f, 0.0f, 0.6f);
                            CVector outputFiringPoint = MultiplyMatrixWithVectorX87(*player->m_matrix, firingPoint); // Como el original (0x68674C: call 0x59C890)
                            activeWeapon->Fire(player, &outputFiringPoint, nullptr, nullptr, nullptr, nullptr);
                        }
                        break;
                    }
                    };
                }
            }
        }
    }

    if (handleFighting) {
        if (intelligence->GetTaskFighting()) {
            auto taskSimpleFight = static_cast<CTaskSimpleFight*>(taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK));
            if (player->m_nMoveState == PEDMOVE_STILL && pad->GetSprint()) {
                taskSimpleFight->ControlFight(player->m_pTargetedObject, 15);
            } else {
                if (playerData->m_nChosenWeapon == player->m_nActiveWeaponSlot)
                    taskSimpleFight->ControlFight(player->m_pTargetedObject, 0);
                else
                    taskSimpleFight->ControlFight(player->m_pTargetedObject, 1);
            }
        }
    }
    CVector firingPoint(0.0f, 0.0f, 0.0f);
    CVector upVector(0.0f, 0.0f, 0.0f);

    if (ActiveWeaponOf(player).m_State == WEAPONSTATE_RELOADING && weaponInfo->flags.bReload) {
        if (!intelligence->GetTaskUseGun()) {
            AssocGroupId animGroupId = weaponInfo->m_eAnimGroup;
            AnimationId crouchReloadAnimID = weaponInfo->flags.bReload ? ANIM_ID_RELOAD : ANIM_ID_WALK;
            if (!player->bIsDucking) {
                if (!RpAnimBlendClumpGetAssociation(player->GetRpClump(), crouchReloadAnimID)) {
                    CAnimManager::BlendAnimation(player->GetRpClump(), animGroupId, crouchReloadAnimID, 4.0f);
                }
            } else {
                // Como el original (0x6867D2..0x686848): primero el id de recarga agachado y la bandera, y solo despues las tareas de agacharse
                if (weaponInfo->GetCrouchReloadAnimationID() && gbUnknown_8D2FE8 && intelligence->GetTaskDuck(true)) {
                    if (!intelligence->GetTaskDuck(true)->IsTaskInUseByOtherTasks()) {
                        crouchReloadAnimID = weaponInfo->GetCrouchReloadAnimationID();
                        if (!RpAnimBlendClumpGetAssociation(player->GetRpClump(), crouchReloadAnimID)) {
                            crouchReloadAnimID = weaponInfo->GetCrouchReloadAnimationID();
                            CAnimManager::BlendAnimation(player->GetRpClump(), animGroupId, crouchReloadAnimID, 4.0f);
                        }
                    }
                }
            }
        } else {
            auto taskUseGun = (CTaskSimpleUseGun*)taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
            taskUseGun->ControlGun(player, player->m_pTargetedObject, eGunCommand::RELOAD);
        }
    }

    bool lookAtTarget = false;
    int32 fightCommand = 0;
    if (!pad->GetTarget() || player->GetPlayerData()->m_nChosenWeapon != player->m_nActiveWeaponSlot || player->m_nMoveState == PEDMOVE_SPRINT && weaponInfo->m_nWeaponFire ||
        TheCamera.Using1stPersonWeaponMode()) {
        if (!pad->GetTarget() && !player->m_pAttachedTo || player->GetPlayerData()->m_nChosenWeapon != player->m_nActiveWeaponSlot || player->m_nMoveState == PEDMOVE_SPRINT ||
            !TheCamera.Using1stPersonWeaponMode()) {
            if ((player->m_pTargetedObject || player->GetPlayerData()->m_bFreeAiming) && intelligence->GetTaskFighting()) {
                // Como el original (0x686F81..0x686F97): `fcomp -0.5 / test ah,5 / jp`; con NaN o igualdad se usa 15
                if (!(playerData->m_vecFightMovement.y < -0.5f)) {
                    fightCommand = 15;
                } else {
                    fightCommand = 16;
                }
                CTaskSimpleFight* taskSimpleFight = intelligence->GetTaskFighting();
                taskSimpleFight->ControlFight(nullptr, fightCommand);
            }
            if (intelligence->GetTaskUseGun()) {
                if (pad->GetWeapon(nullptr) || (float)pad->GetPedWalkUpDown() <= 50.0f && (float)pad->GetPedWalkUpDown() >= -50.0f && (float)pad->GetPedWalkLeftRight() <= 50.0f &&
                                                   (float)pad->GetPedWalkLeftRight() >= -50.0f) {
                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                    taskUseGun->PlayerPassiveControlGun();
                } else {
                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                    taskUseGun->ControlGun(player, player->m_pTargetedObject, eGunCommand::END_NOW);
                }

                CCam* camera = &CCamera::GetActiveCamera();
                if (!weaponInfo->flags.bAimWithArm && camera->m_nMode == MODE_FOLLOWPED) {
                    // Como el original (0x6870A1..0x6870AB): fchs + fpatan del x87
                    player->m_fAimingRotation = TrigAtan2Cpp(-camera->m_vecFront.x, camera->m_vecFront.y);
                }
                if (player->m_pTargetedObject || playerData->m_bFreeAiming) {
                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                    taskUseGun->AbortIK(player);
                }
            }
            if (player->m_pTargetedObject) {
                player->ClearWeaponTarget();
            }
            player->Clear3rdPersonMouseTarget();
            playerData->m_bFreeAiming = 0;
        } else if (!taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            newSimpleUseGunTask = new CTaskSimpleUseGun(player->m_pTargetedObject);
            firingPoint.Set(0.0f, 0.0f, 0.0f);

            taskManager->SetTaskSecondary(newSimpleUseGunTask, TASK_SECONDARY_ATTACK);
        } else if (intelligence->GetTaskUseGun()) {
            auto taskUseGun = (CTaskSimpleUseGun*)taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
            taskUseGun->ControlGun(player, player->m_pTargetedObject, eGunCommand::AIM);
        }
        lookAtTarget = true;
    }
    if (!lookAtTarget) {
        if (!weaponInfo->flags.bCanAim || playerData->m_bFreeAiming) {
            if (playerData->m_bFreeAiming && weaponInfo->flags.bCanAim && (pad->ShiftTargetLeftJustDown() || pad->ShiftTargetRightJustDown()) && !CCamera::m_bUseMouse3rdPerson) {
                if (player->m_pTargetedObject) {
                    if (pad->ShiftTargetLeftJustDown()) {
                        player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, true);
                    }
                    if (pad->ShiftTargetRightJustDown()) {
                        player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, false);
                    }
                } else {
                    bool shiftTargetLeftJustDown = pad->ShiftTargetLeftJustDown();
                    player->FindNextWeaponLockOnTarget(nullptr, shiftTargetLeftJustDown);
                }
            } else if (!weaponInfo->flags.bOnlyFreeAim || player->m_pTargetedObject || playerData->m_bFreeAiming) {
                if (CCamera::m_bUseMouse3rdPerson && playerData->m_bFreeAiming) {
                    bool bWeaponIsNotMelee = true;
                    if (!weaponInfo->m_nWeaponFire) {
                        bWeaponIsNotMelee = false;
                    }
                    player->Compute3rdPersonMouseTarget(bWeaponIsNotMelee);
                }
            } else {
                playerData->m_bFreeAiming = 1;
            }
        } else {
            if (player->m_pTargetedObject) {
                CPed* targetedEntity = (CPed*)player->m_pTargetedObject;
                CWeapon* activeWeapon = &ActiveWeaponOf(player);
                auto weaponSkill = eWeaponSkill::POOR;
                int32 pedState = 0;
                if ((fabs((float)pad->AimWeaponLeftRight(player)) > 100.0f || fabs((float)pad->AimWeaponUpDown(player)) > 100.0f) && !CGameLogic::IsCoopGameGoingOn() ||
                    targetedEntity == (CPed*)nullptr || CCamera::m_bUseMouse3rdPerson == true ||
                    targetedEntity && targetedEntity->GetIsTypePed() &&
                        (!CPlayerPed::PedCanBeTargettedVehicleWise(targetedEntity) ||
                         !CLocalisation::KickingWhenDown() && ((pedState = targetedEntity->m_nPedState, pedState == PEDSTATE_DIE) || pedState == PEDSTATE_DEAD)) ||
                    player->DoesTargetHaveToBeBroken(player->m_pTargetedObject, activeWeapon) ||
                    !player->bCanPointGunAtTarget &&
                        (activeWeapon->m_Type, weaponSkill = player->GetWeaponSkill(), !(CWeaponInfo::GetWeaponInfo(activeWeapon->m_Type, weaponSkill)->flags.bCanAim))) {
                    player->ClearWeaponTarget();
                    playerData->m_bFreeAiming = 1;
                }

                if (player->m_pTargetedObject) {
                    if (pad->ShiftTargetLeftJustDown()) {
                        player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, true);
                    }
                    if (pad->ShiftTargetRightJustDown()) {
                        player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, false);
                    }
                }

                if (CWeaponInfo::GetWeaponInfo(ActiveWeaponOf(player).m_Type, eWeaponSkill::STD)->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT) {
                    targetedEntity = (CPed*)player->m_pTargetedObject;
                    // Como el original (0x686AF9..0x686B0D): IsInSeeingRange y GetActivePrimaryTask se piden a la inteligencia del OBJETIVO ([edi+0x47C])
                    if (targetedEntity && targetedEntity->GetIsTypePed() && targetedEntity->GetIntelligence()->IsInSeeingRange(player->GetPosition())) {
                        CTask* activePrimaryTask = targetedEntity->GetIntelligence()->GetActivePrimaryTask();
                        if (!activePrimaryTask || activePrimaryTask->GetTaskType() != TASK_COMPLEX_REACT_TO_GUN_AIMED_AT) {
                            if (ActiveWeaponOf(player).m_Type != WEAPON_PISTOL_SILENCED) {
                                player->Say(CTX_GLOBAL_PULL_GUN);
                            }
                            CPedGroup* pedGroup = CPedGroups::GetPedsGroup(targetedEntity);
                            if (pedGroup) {
                                if (!CPedGroups::AreInSameGroup(targetedEntity, player)) {
                                    auto* eventGunAimedAt = new CEventGunAimedAt(player);
                                    CEventGroupEvent eventGroupEvent(targetedEntity, eventGunAimedAt);
                                    pedGroup->m_groupIntelligence.AddEvent(&eventGroupEvent);
                                }
                            } else {
                                // Como el original (0x686BED..0x686C08): el evento se anade al grupo de eventos del OBJETIVO (edi = [objetivo+0x47C])
                                CEventGunAimedAt eventGunAimedAt(player);
                                targetedEntity->GetIntelligence()->m_eventGroup.Add(&eventGunAimedAt, false);
                            }
                        }
                    }
                }
            } else if (CCamera::m_bUseMouse3rdPerson) {
                player->ClearWeaponTarget();
            } else if (pad->GetEnterTargeting() || TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget || m_nFrameCounter < (uint32)(CTimer::GetFrameCounter() - 1)) {
                player->FindWeaponLockOnTarget();
            }
            if (!player->m_pTargetedObject) {
                playerData->m_bFreeAiming = 1;
            }
        }
        if (player->m_pTargetedObject) {
            if (weaponInfo->m_nWeaponFire || player->bIsDucking) {
                playerData->m_bFreeAiming = 0;
            }
        } else {
            uint32 nWeaponFire = weaponInfo->m_nWeaponFire;
            if (!nWeaponFire || nWeaponFire == WEAPON_FIRE_PROJECTILE || nWeaponFire == WEAPON_FIRE_USE)
                lookAtTarget = true;
        }
        if (!lookAtTarget) {
            TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON, 0, 0);

            if (player->m_pTargetedObject) {
                TheCamera.UpdateAimingCoors(player->m_pTargetedObject->GetPosition());
            } else {
                CMatrix* playerMatrix = player->m_matrix;
                firingPoint = playerMatrix->GetForward();

                // Como el original (0x686DB2..0x686DE1): seno x87 sin redondear sumado a z; todos los productos por 5.0 en float
                const float aimZ = OnFootSinPlusCpp(player->GetPlayerData()->m_fLookPitch, firingPoint.z);
                firingPoint.x = firingPoint.x * 5.0f;
                firingPoint.y = firingPoint.y * 5.0f;
                firingPoint.z = aimZ * 5.0f;

                firingPoint += player->GetPosition();
                TheCamera.UpdateAimingCoors(firingPoint);
            }

            if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                if (intelligence->GetTaskUseGun()) {
                    auto taskUseGun = (CTaskSimpleUseGun*)taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                    taskUseGun->ControlGun(player, player->m_pTargetedObject, eGunCommand::AIM);
                }
            } else {
                newSimpleUseGunTask = new CTaskSimpleUseGun(player->m_pTargetedObject, CVector(0.0f, 0.0f, 0.0f), eGunCommand::AIM, 1, false);
                firingPoint.Set(0.0f, 0.0f, 0.0f);
                taskManager->SetTaskSecondary(newSimpleUseGunTask, TASK_SECONDARY_ATTACK);
            }
        }
    }
    playerData->m_bHaveTargetSelected = player->m_pTargetedObject ? true : false;
    CPed* targetedObject = player->m_pTargetedObject->AsPed();
    const auto AbortLookingIfPossible = [&]() {
        if (m_pLookingAtEntity && g_ikChainMan.IsLooking(player) && g_ikChainMan.GetLookAtEntity(player) == m_pLookingAtEntity) {
            g_ikChainMan.AbortLookAt(player, 250);
        }
        m_pLookingAtEntity = targetedObject;
    };

    if (!targetedObject)
        return AbortLookingIfPossible();

    bool bTargetedPedDead = false;
    if (targetedObject->GetIsTypePed() && (targetedObject->bFallenDown || targetedObject->m_nPedState == PEDSTATE_DEAD)) {
        bTargetedPedDead = true;
    }

    if ((weaponInfo->m_nWeaponFire || intelligence->GetTaskFighting() && !bTargetedPedDead) && (player->bIsDucking || !weaponInfo->flags.bAimWithArm)) {
        return AbortLookingIfPossible();
    }

    CVector distance = targetedObject->GetPosition() - player->GetPosition();
    // Como el original (0x6871EA..0x6871F5): `fcomp 0.0 / test ah,41h / jne`; con NaN tambien se aborta la mirada
    if (!(DotProduct(distance, player->GetForwardVector()) > 0.0f)) {
        return AbortLookingIfPossible();
    } else if (!g_ikChainMan.IsLooking(player) || (g_ikChainMan.GetLookAtEntity(player) != targetedObject)) {
        auto pedBoneId = BONE_UNKNOWN;
        if (targetedObject->GetIsTypePed()) {
            pedBoneId = BONE_HEAD;
        }
        g_ikChainMan.LookAt("ProcPlyrWeapon", player, targetedObject, gDefaultTaskTime, pedBoneId, nullptr, false, 0.25f, 500, 3, false);
    }

    m_pLookingAtEntity = targetedObject;
}

// 0x6872C0
void CTaskSimplePlayerOnFoot::PlayIdleAnimations(CPlayerPed* player) {
    // Como el original (0x6872CE): el pad se pide antes de mirar el modo cooperativo
    CPad* pad = player->GetPadFromPlayer();

    if (CGameLogic::IsCoopGameGoingOn()) {
        return;
    }

    AssocGroupId animGroupID = ANIM_GROUP_DEFAULT;
    // Como el original (0x687300 `test byte [ebp+478h],2`): la bandera del ped es bUsingMobilePhone, no bCrouchWhenShooting
    if (TheCamera.m_bWideScreenOn || player->bIsDucking || player->bUsingMobilePhone || player->GetIntelligence()->GetTaskHold(false) || pad->DisablePlayerControls ||
        static_cast<int32>(player->m_nMoveState) > static_cast<int32>(PEDMOVE_STILL) || // Como el original (0x687323 `cmp [ebp+534h],1 / jg`): comparacion con signo
        (animGroupID = player->m_nAnimGroup, animGroupID != ANIM_GROUP_PLAYER) && animGroupID != ANIM_GROUP_FAT && animGroupID != ANIM_GROUP_MUSCULAR // todo: bad
    ) {
        pad->SetTouched();
    }

    CAnimBlock* playerIdlesAnimBlock = &CAnimManager::GetAnimBlocks()[m_PlayerIdlesAnimBlockId];
    // Como el original (0x687393 `cmp ebx,2710h / jle`): comparacion con signo
    const auto touchTimeDelta = static_cast<int32>(pad->GetTouchedTimeDelta());
    if (touchTimeDelta <= 10000) {
        if (playerIdlesAnimBlock->IsLoaded) {
            CStreaming::SetModelIsDeletable(IFPToModelId(m_PlayerIdlesAnimBlockId));
            // Como el original (0x6874E3..0x68750C): recorrido con RpAnimBlendClumpGetFirstAssociation / RpAnimBlendGetNextAssociation
            for (auto* animAssoc = RpAnimBlendClumpGetFirstAssociation(player->GetRpClump()); animAssoc; animAssoc = RpAnimBlendGetNextAssociation(animAssoc)) {
                if (animAssoc->m_Flags & ANIMATION_200) {
                    animAssoc->m_BlendDelta = -8.0f;
                }
            }
        }
        gLastTouchTimeDelta = 0;
        return;
    }

    // Make sure block is loaded
    CStreaming::RequestModel(IFPToModelId(m_PlayerIdlesAnimBlockId), STREAMING_GAME_REQUIRED);
    if (!playerIdlesAnimBlock->IsLoaded) {
        return;
    }

    // Como el original (0x6873BD..0x68740B): primero se buscan las animaciones del bloque (indice del EXE: first <= idx < first + num, con signo)
    for (auto* animAssoc = RpAnimBlendClumpGetFirstAssociation(player->GetRpClump()); animAssoc; animAssoc = RpAnimBlendGetNextAssociation(animAssoc)) {
        const auto animIdx = static_cast<int32>(animAssoc->GetHier()->GetIndex());
        if (animIdx >= playerIdlesAnimBlock->FirstAnimIdx && animIdx < playerIdlesAnimBlock->FirstAnimIdx + static_cast<int32>(playerIdlesAnimBlock->NumAnims)) {
            return;
        }
    }

    // Como el original (0x68740D `test byte [ebp+46Ch],0Ch / jne`): basta una de las dos banderas
    if (player->bIsLooking || player->bIsRestoringLook) {
        return;
    }

    // Como el original (0x687424 `cmp edx,4E20h / jle`): resta y comparacion con signo
    if (static_cast<int32>(static_cast<uint32>(touchTimeDelta) - gLastTouchTimeDelta) <= 20000) {
        return;
    }

    // Como el original (0x687430: call 0x407180): [0, 4) con CGeneral::GetRandomIntInRange, no la plantilla entera
    int32 randomNumber = 0;
    do {
        randomNumber = CGeneral::GetRandomIntInRange(0, 4);
    } while (gLastRandomNumberForIdleAnimationID == randomNumber);

    static constexpr struct {
        AnimationId animId;
        AssocGroupId assoc;
    } animations[] = {
        {ANIM_ID_STRETCH, ANIM_GROUP_PLAYIDLES},
        {ANIM_ID_TIME, ANIM_GROUP_PLAYIDLES},
        {ANIM_ID_SHLDR, ANIM_GROUP_PLAYIDLES},
        {ANIM_ID_STRLEG, ANIM_GROUP_PLAYIDLES},
    };
    auto animation = animations[randomNumber];
    CAnimBlendAssociation* animNewAssoc = CAnimManager::BlendAnimation(player->GetRpClump(), animation.assoc, animation.animId, 8.0f);
    animNewAssoc->m_Flags |= ANIMATION_200;
    gLastTouchTimeDelta = touchTimeDelta;
    gLastRandomNumberForIdleAnimationID = randomNumber;
    if (CStats::GetStatValue(STAT_MANAGEMENT_ISSUES_MISSION_ACCOMPLISHED) != 0.0f && CTimer::GetTimeInMS() > 1200000) {
        player->Say(CTX_GLOBAL_SINGING, 0, 0.2f, 0, 0, 0);
    }
}

namespace {
// Como el original: fsin/fcos x87, con redondeo a float al guardar (-sin y cos).
CVector FighterDirection(float angle) {
    const float trigSin = -TrigSimpleSinCpp(angle);
    const float trigCos = TrigSimpleCosCpp(angle);
    return CVector{trigSin, trigCos, 0.0f};
}

// Como el original: seno/coseno permanecen en x87 en la rama no cooperativa;
// productos en orden y, x, z, incluida la multiplicación por cero del eje z (ver OnFootFighterProjectCpp).
CVector2D FighterProject(CMatrix* matrix, float angle, float magnitude, bool roundTrig) {
    float projected[2];
    OnFootFighterProjectCpp(&matrix->GetRight().x, &matrix->GetForward().x, angle, magnitude, roundTrig, projected);
    return CVector2D{projected[0], projected[1]};
}
} // namespace

// 0x687530
void CTaskSimplePlayerOnFoot::PlayerControlFighter(CPlayerPed* player) {
    auto* fight = player->GetIntelligence()->GetTaskFighting();
    if (!fight) {
        return;
    }
    const bool nonMelee = CWeaponInfo::GetWeaponInfo(player->m_aWeapons.data()[static_cast<int8>(player->m_nActiveWeaponSlot)].m_Type, eWeaponSkill::STD)->m_nWeaponFire != WEAPON_FIRE_MELEE;
    auto* pad = player->GetPadFromPlayer();
    const float walkX = static_cast<float>(pad->GetPedWalkLeftRight());
    const float walkY = static_cast<float>(pad->GetPedWalkUpDown());
    CVector2D movement{walkX / 128.f, walkY / 128.f};
    const float step = CTimer::GetTimeStep() * .07f;
    const bool coop = CGameLogic::IsPlayerUse2PlayerControls(player);

    // Como el original: cooperativo consulta la dirección antes de proyectar los ejes locales.
    if (coop || player->m_pTargetedObject || (CCamera::m_bUseMouse3rdPerson && pad->GetTarget())) {
        float magnitude = movement.Magnitude();
        if (magnitude > 0.f) {
            if (!coop && magnitude > 1.f) {
                magnitude = 1.f;
            }
            const float angle = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -movement.x, movement.y) - TheCamera.m_fOrientation);
            const CVector direction = FighterDirection(angle);
            if (!coop || CGameLogic::IsPlayerAllowedToGoInThisDirection(player, direction, 0.f)) {
                movement = FighterProject(player->m_matrix, angle, magnitude, coop);
            } else {
                movement = {0.f, 0.f};
            }
        }
    }

    auto& fightMovement = player->GetPlayerData()->m_vecFightMovement;
    const auto approach = [step](float current, float wanted) {
        const float delta = wanted - current;
        return std::abs(delta) > step ? (delta > 0.f ? current + step : current - step) : wanted;
    };
    if (!player->m_pTargetedObject && !(CCamera::m_bUseMouse3rdPerson && pad->GetTarget())) {
        float angle = CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -walkX, walkY);
        float magnitude = movement.Magnitude();
        if (magnitude > 1.f) {
            magnitude = 1.f;
        }
        if (magnitude == 0.f) {
            fightMovement = {0.f, 0.f};
        } else if (movement.y <= 0.f) {
            fightMovement.y = approach(fightMovement.y, -magnitude);
            fightMovement.x = 0.f;
            player->m_fAimingRotation = CGeneral::LimitRadianAngle(angle - TheCamera.m_fOrientation);
        }

        if (!player->m_pTargetedObject && movement.y <= 0.f && static_cast<int8>(fight->m_nLastCommand) < 11) {
            m_nTimer = static_cast<int32>(static_cast<uint32>(m_nTimer) + static_cast<uint32>(CTimer::GetTimeStep() * .02f * 1000.f));
        } else {
            m_nTimer = 0;
        }
        if (pad->GetSprint() || pad->GetDuck() || nonMelee || movement.y > 0.f || static_cast<float>(static_cast<uint32>(m_nTimer)) >= 2000.f) {
            (void)pad->GetSprint(); // Como el original: segunda consulta antes de escoger comando.
            uint8 command = magnitude > .5f ? 16 : 15;
            if (pad->DuckJustDown() && CTaskSimpleDuck::CanPedDuck(player)) {
                player->GetIntelligence()->SetTaskDuckSecondary(0);
                command = 18;
            }
            fight->ControlFight(nullptr, command);
            return;
        }
    } else {
        fightMovement.y = approach(fightMovement.y, movement.y);
        fightMovement.x = approach(fightMovement.x, movement.x);
        const bool farTarget = player->m_pTargetedObject
            && [&] {
                const CVector d = player->m_pTargetedObject->GetPosition() - player->GetPosition();
                return (d.z * d.z + d.y * d.y) + d.x * d.x > 16.f;
            }()
            && static_cast<int8>(fight->m_nLastCommand) < 11;
        if (farTarget || pad->GetSprint() || nonMelee) {
            const uint8 command = pad->GetSprint() ? 17 : movement.y < -.5f ? 16 : 15;
            fight->ControlFight(nullptr, command);
            return;
        }
    }
    if (std::abs(fightMovement.y) > 0.f && std::abs(fightMovement.y) > std::abs(fightMovement.x)) {
        fight->ControlFight(nullptr, fightMovement.y < 0.f ? 3 : 5);
    } else if (std::abs(fightMovement.x) > 0.f) {
        fight->ControlFight(nullptr, fightMovement.x > 0.f ? 6 : 4);
    }
}

// 0x687C20
void CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon(CPlayerPed* player) {
    CTaskSimpleUseGun* taskUseGun = player->GetIntelligence()->GetTaskUseGun();
    if (!taskUseGun)
        return;

    if (player->m_pAttachedTo != nullptr)
        return;

    CVector2D moveSpeed(0.0f, 0.0f);
    CPad* pad = player->GetPadFromPlayer();
    float pedWalkUpDown = moveSpeed.y;
    float pedWalkLeftRight = moveSpeed.x;
    if (!taskUseGun->m_WeaponInfo->flags.b1stPerson || CGameLogic::IsPlayerUse2PlayerControls(player)) {
        pedWalkUpDown = pad->GetPedWalkUpDown();
        pedWalkLeftRight = pad->GetPedWalkLeftRight();
    } else if (TheCamera.Using1stPersonWeaponMode()) {
        pedWalkUpDown = pad->GetPedWalkUpDown(player);
        pedWalkLeftRight = pad->GetPedWalkLeftRight(player);
    }

    moveSpeed.x = pedWalkLeftRight / 128.0f;
    moveSpeed.y = pedWalkUpDown / 128.0f;

    if (CGameLogic::IsPlayerUse2PlayerControls(player)) {
        float moveBlendRatio = sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y);
        CEntity* targetedObject = player->m_pTargetedObject;

        if (moveBlendRatio > 0.0f) {
            float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -moveSpeed.x, moveSpeed.y) - TheCamera.m_fOrientation;
            float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
            // Como el original (0x687D71..0x687D9B): -sin y cos con fsin/fcos del x87 sobre el angulo ya redondeado a float
            const CVector direction = FighterDirection(limitedRadianAngle);
            const float negativeSinRadian = direction.x;
            const float cosRadian = direction.y;
            if (targetedObject) {
                if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(player, {negativeSinRadian, cosRadian, 0.0f}, 0.0f)) {
                    moveBlendRatio = 0.0f;
                }
                CMatrixLink* pedMatrix = player->m_matrix;
                moveSpeed.x = (cosRadian * pedMatrix->GetRight().y + negativeSinRadian * pedMatrix->GetRight().x + pedMatrix->GetRight().z * 0.0f) * moveBlendRatio;
                moveSpeed.y = -((cosRadian * pedMatrix->GetForward().y + negativeSinRadian * pedMatrix->GetForward().x + pedMatrix->GetForward().z * 0.0f) * moveBlendRatio);
            } else {
                player->m_fAimingRotation = limitedRadianAngle;
                float moveSpeedY = 0.0f;
                if (CGameLogic::IsPlayerAllowedToGoInThisDirection(player, {negativeSinRadian, cosRadian, 0.0f}, 0.0f)) {
                    moveSpeedY = moveBlendRatio;
                }
                moveSpeed.x = 0.0f;
                moveSpeed.y = -moveSpeedY;
            }
        }

        if (targetedObject) {
            CVector2D distance = targetedObject->GetPosition() - player->GetPosition();
            // Como el original (0x687EA3..0x687EAB): fchs + fpatan del x87
            player->m_fAimingRotation = TrigAtan2Cpp(-distance.x, distance.y);
        }
    }

    float moveBlendRatio = sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y);
    if (moveBlendRatio > 1.0) {
        float moveSpeedMultiplier = 1.0f / moveBlendRatio;
        moveSpeed.x = moveSpeed.x * moveSpeedMultiplier;
        moveSpeed.y = moveSpeedMultiplier * moveSpeed.y;
    }

    taskUseGun->ControlGunMove(moveSpeed);
    if (pad->DuckJustDown()) {
        if (CTaskSimpleDuck::CanPedDuck(player)) {
            player->GetIntelligence()->SetTaskDuckSecondary(0);
        }
    }
}

// 0x687F30
void CTaskSimplePlayerOnFoot::PlayerControlDucked(CPlayerPed* player) {
    auto* duckTask = static_cast<CTaskSimpleDuck*>(player->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK));
    if (!duckTask)
        return;

    CPad* pad = player->GetPadFromPlayer();

    // Como el original (0x687F5F..0x687FB1): primero arriba/abajo (0x53FD30) y despues izquierda/derecha (0x53FC90)
    const float pedWalkUpDown = pad->GetPedWalkUpDown();
    const float pedWalkLeftRight = pad->GetPedWalkLeftRight();
    CVector2D moveSpeed;
    moveSpeed.x = pedWalkLeftRight / 128.0f;
    moveSpeed.y = pedWalkUpDown / 128.0f;

    float pedMoveBlendRatio = sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y);
    if (player->m_pAttachedTo) {
        pedMoveBlendRatio = 0.0f;
    } else if (pedMoveBlendRatio > 1.0f) {
        pedMoveBlendRatio = 1.0f;
    }

    // Como el original (0x687FE2..0x687FF0): las banderas de la tarea se miran DESPUES de leer el pad
    if (duckTask->m_bIsFinished || duckTask->m_bIsAborting)
        return;

    if (pad->DuckJustDown() || pad->GetSprint() || pad->JumpJustDown() || pad->ExitVehicleJustDown() || !CTaskSimpleDuck::CanPedDuck(player)) {
        player->GetIntelligence()->ClearTaskDuckSecondary();
        // Como el original (0x688290..0x6882B3): dos consultas de GetTaskUseGun
        auto useGunTask = player->GetIntelligence()->GetTaskUseGun();
        if (!useGunTask || player->GetIntelligence()->GetTaskUseGun()->m_WeaponInfo->flags.bAimWithArm) {
            auto pedMoveState = PEDMOVE_NONE;
            if (pad->GetSprint()) {
                // Como el original (0x6882C3 `fcomp 0.5 / test ah,41h / jne`): con NaN tambien se sale
                if (!(pedMoveBlendRatio > 0.5f)) {
                    return;
                }
                auto pNewAnimation = CAnimManager::BlendAnimation(player->GetRpClump(), player->m_nAnimGroup, ANIM_ID_RUN, gDuckAnimBlendData);
                pNewAnimation->m_Flags |= ANIMATION_IS_PLAYING;
                player->GetPlayerData()->m_fMoveBlendRatio = 1.5f;
                pedMoveState = PEDMOVE_RUN;
            } else {
                if (!(pedMoveBlendRatio > 0.5f)) {
                    return;
                }
                auto pNewAnimation = CAnimManager::BlendAnimation(player->GetRpClump(), player->m_nAnimGroup, ANIM_ID_WALK, gDuckAnimBlendData);
                pNewAnimation->m_Flags |= ANIMATION_IS_PLAYING;
                player->GetPlayerData()->m_fMoveBlendRatio = 1.5f;
                pedMoveState = PEDMOVE_WALK;
            }
            player->m_nMoveState = pedMoveState;
            player->m_nSwimmingMoveState = pedMoveState;
        } else if (pedMoveBlendRatio > 0.5f) {
            auto pNewAnimation = CAnimManager::BlendAnimation(player->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GUNMOVE_FWD, gDuckAnimBlendData);
            pNewAnimation->m_Flags |= ANIMATION_IS_PLAYING;
            player->GetPlayerData()->m_fMoveBlendRatio = 1.0f;
            moveSpeed.x = 1.0f;
            moveSpeed.y = 0.0f;
            CTaskSimpleUseGun* pTaskSimpleUseGun = player->GetIntelligence()->GetTaskUseGun();
            pTaskSimpleUseGun->ControlGunMove(moveSpeed);
        }
    } else if (!pad->GetTarget() || ActiveWeaponOf(player).IsTypeMelee()) {
        if (pedMoveBlendRatio > 0.0f) {
            float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -moveSpeed.x, moveSpeed.y) - TheCamera.m_fOrientation;
            float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
            player->m_fAimingRotation = limitedRadianAngle;
            // Como el original (0x6881F6..0x68823D): la direccion es (-sin, cos, 0), no (0, -sin, 0)
            if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(player, FighterDirection(limitedRadianAngle), 0.0f)) {
                pedMoveBlendRatio = 0.0f;
            }
        }
        moveSpeed.y = -pedMoveBlendRatio;

        player->GetPlayerData()->m_fMoveBlendRatio = pedMoveBlendRatio;
        moveSpeed.x = 0.0f;
        duckTask->ControlDuckMove({ 0.0f, moveSpeed.y });
    } else {
        if (CGameLogic::IsPlayerUse2PlayerControls(player)) {
            float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -moveSpeed.x, moveSpeed.y) - TheCamera.m_fOrientation;
            float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
            // Como el original (0x68807B..0x6880F1): direccion (-sin, cos, 0) del x87
            const CVector moveDirection = FighterDirection(limitedRadianAngle);
            if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(player, moveDirection, 0.0f)) {
                pedMoveBlendRatio = 0.0f;
            }
            CMatrix* matrix = player->m_matrix;
            CEntity* targetedObject = player->m_pTargetedObject;
            moveSpeed.x = (moveDirection.y * matrix->GetRight().y + moveDirection.x * matrix->GetRight().x + matrix->GetRight().z * 0.0f) * pedMoveBlendRatio;
            moveSpeed.y = -((moveDirection.y * matrix->GetForward().y + matrix->GetForward().z * 0.0f + moveDirection.x * matrix->GetForward().x) * pedMoveBlendRatio);
            if (targetedObject) {
                CVector distance = targetedObject->GetPosition() - player->GetPosition();
                player->m_fAimingRotation = TrigAtan2Cpp(-distance.x, distance.y);
            } else {
                player->m_fAimingRotation = limitedRadianAngle;
            }
        }
        duckTask->ControlDuckMove(moveSpeed);
        player->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
    }
}

// 0x6883D0
int32 CTaskSimplePlayerOnFoot::PlayerControlZelda(CPlayerPed* player, bool bAvoidJumpingAndDucking) {
    CPlayerPedData* playerData = player->GetPlayerData();
    playerData->m_vecFightMovement = CVector2D();

    CPad* pad = player->GetPadFromPlayer();
    float pedWalkLeftRight = pad->GetPedWalkLeftRight();
    float pedWalkUpDown = pad->GetPedWalkUpDown();
    float pedMoveBlendRatio = sqrt(pedWalkUpDown * pedWalkUpDown + pedWalkLeftRight * pedWalkLeftRight) * (1.0f / 60.0f);
    if (player->m_pAttachedTo)
        pedMoveBlendRatio = 0.0f;

    bool updateMoveBlendRatio = true;
    if (pad->NewState.m_bPedWalk && pedMoveBlendRatio > 1.0f) {
        pedMoveBlendRatio = 1.0f;
    } else if (!(pedMoveBlendRatio > 0.0f)) { // Como el original (0x688491 `test ah,41h / jne`): con NaN tambien se pone a cero
        player->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
        updateMoveBlendRatio = false;
    }
    if (updateMoveBlendRatio) {
        const float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -pedWalkLeftRight, pedWalkUpDown) - TheCamera.m_fOrientation;
        const float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
        player->m_fAimingRotation = limitedRadianAngle;
        // Como el original (0x6884C4..0x688508): (-sin, cos, 0) con fsin/fcos del x87
        if (CGameLogic::IsPlayerAllowedToGoInThisDirection(player, FighterDirection(limitedRadianAngle), 0.0f)) {
            float fMaximumMoveBlendRatio = CTimer::GetTimeStep() * 0.07f;
            // Como el original (0x68852D..0x688564): `fcomp / test ah,41h / jne`; con NaN se toma la rama central (m_fMoveBlendRatio = pedMoveBlendRatio)
            if (pedMoveBlendRatio - playerData->m_fMoveBlendRatio > fMaximumMoveBlendRatio) {
                playerData->m_fMoveBlendRatio = fMaximumMoveBlendRatio + playerData->m_fMoveBlendRatio;
            } else if (!(-fMaximumMoveBlendRatio > pedMoveBlendRatio - playerData->m_fMoveBlendRatio)) {
                playerData->m_fMoveBlendRatio = pedMoveBlendRatio;
            } else {
                playerData->m_fMoveBlendRatio -= fMaximumMoveBlendRatio;
            }
        } else {
            player->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
        }
    }
    if (!(CWeaponInfo::GetWeaponInfo(ActiveWeaponOf(player).m_Type, eWeaponSkill::STD)->flags.bHeavy)) {
        // Como el original (0x6885A7..0x6885B0): banderas fisicas del suelo (+0x40): bit 2 (bDisableCollisionForce) activo y bit 3 (bCollidable) apagado lo impide
        const uint32 standingFlags = player->m_standingOnEntity ? *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(player->m_standingOnEntity) + 0x40) : 0u;
        if (!player->m_standingOnEntity || !(standingFlags & 4) || (standingFlags & 8)) {
            if (!player->GetIntelligence()->GetTaskHold(false) || !((CTaskSimpleHoldEntity*)player->GetIntelligence()->GetTaskHold(false))->m_pAnimBlendAssociation) {
                CAnimBlendHierarchy* animHierarchy = nullptr;
                CAnimBlendAssocGroup* animGroup = &CAnimManager::GetAssocGroups()[player->m_nAnimGroup];

                if (player->GetPlayerData()->m_bPlayerSprintDisabled || g_surfaceInfos.CantSprintOn(player->m_nContactSurface) ||
                    (animHierarchy = animGroup->GetAnimation(ANIM_ID_RUN)->m_BlendHier, animHierarchy == animGroup->GetAnimation(ANIM_ID_SPRINT)->m_BlendHier)) {
                    if (pad->GetSprint()) {
                        player->m_nMoveState = PEDMOVE_RUN;
                    }
                } else if (player->ControlButtonSprint(SPRINT_GROUND) >= 1.0f) {
                    player->m_nMoveState = PEDMOVE_SPRINT;
                }
            }
        }
    }

    player->SetRealMoveAnim();
    // What is the point of calling RpAnimBlendClumpGetAssociation here?
    // Nvm, let's keep it.
    RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_IDLE_ARMED);
    RpAnimBlendClumpGetAssociation(player->GetRpClump(), ANIM_ID_IDLE_CHAT);

    // Como el original (0x688687..0x6886C5): DuckJustDown y CanPedDuck se consultan siempre; solo SetTaskDuckSecondary depende de bAvoidJumpingAndDucking
    if (pad->DuckJustDown() && CTaskSimpleDuck::CanPedDuck(player)) {
        if (!bAvoidJumpingAndDucking) {
            player->GetIntelligence()->SetTaskDuckSecondary(0);
        }
    }

    if (!bAvoidJumpingAndDucking && !player->bIsInTheAir && !(CWeaponInfo::GetWeaponInfo(ActiveWeaponOf(player).m_Type, eWeaponSkill::STD)->flags.bHeavy) && pad->JumpJustDown() && !pad->GetTarget() &&
        !player->m_pAttachedTo) {
        // Possibly inlined code? Like CCamera::IsInSniperMode()
        switch (CCamera::GetActiveCamera().m_nMode) {
        case MODE_SNIPER:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON:
        case MODE_HELICANNON_1STPERSON:
        case MODE_CAMERA:
            break;
        default: {
            player->ClearWeaponTarget();
            if (const auto task = player->GetTaskManager().GetActiveTask()) {
                if (!notsa::isa<CTaskComplexJump>(task)) {
                    player->GetTaskManager().SetTask(new CTaskComplexJump{}, TASK_PRIMARY_PRIMARY, false);
                }
            }
        }
        }
    }

    PlayIdleAnimations(player);
    player->m_pedIK.bSlopePitch = true;
    return player->m_pedIK.m_nFlags;
}
