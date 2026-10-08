/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWeapon
 * Reconstrucción: COMPLETA — 41/41 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 6.
 * Validación registrada: 6/41 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   CheckForShootingVehicleOccupant | 0x0073F480 | S
 *   DoDriveByAutoAiming | 0x0073D720 | S
 *   DoTankDoomAiming | 0x0073D1E0 | S
 *   FindNearestTargetEntityWithScreenCoors | 0x0073E240 | S
 *   FireInstantHit | 0x0073FB10 | S
 *   FireInstantHitFromCar | 0x0073EC40 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Weapon.h"

#include "Glass.h"
#include "CollisionData.h"
#include "Collision/ColLine.h"
#include "WeaponInfo.h"
#include "CreepingFire.h"
#include "BulletInfo.h"
#include "InterestingEvents.h"
#include "Shadows.h"
#include "Birds.h"

//float& PELLET_COL_SCALE_RATIO_MULT = *(float*)0x8D6128; // 1.3

void CWeapon::InjectHooks() {
    RH_ScopedClass(CWeapon);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x73B430);

    RH_ScopedInstall(Shutdown, 0x73A380);
    RH_ScopedInstall(Reload, 0x73AEB0);
    RH_ScopedInstall(IsTypeMelee, 0x73B1C0);
    RH_ScopedInstall(IsType2Handed, 0x73B1E0);
    RH_ScopedInstall(IsTypeProjectile, 0x73B210);
    RH_ScopedInstall(HasWeaponAmmoToBeUsed, 0x73B2A0);
    RH_ScopedInstall(InitialiseWeapons, 0x73A300);
    RH_ScopedInstall(ShutdownWeapons, 0x73A330);
    RH_ScopedInstall(UpdateWeapons, 0x73A360);
    RH_ScopedInstall(AddGunshell, 0x73A3E0);
    RH_ScopedInstall(ProcessLineOfSight, 0x73B300);
    RH_ScopedInstall(StopWeaponEffect, 0x73B360);
    RH_ScopedInstall(TargetWeaponRangeMultiplier, 0x73B380);
    RH_ScopedInstall(Initialise, 0x73B4A0);
    RH_ScopedInstall(DoWeaponEffect, 0x73E690);
    RH_ScopedInstall(FireSniper, 0x73AAC0);
    RH_ScopedInstall(TakePhotograph, 0x73C1F0);
    RH_ScopedInstall(DoDoomAiming, 0x73CDC0);
    RH_ScopedInstall(GenerateDamageEvent, 0x73A530);
    RH_ScopedInstall(FireInstantHitFromCar2, 0x73CBA0);
    RH_ScopedInstall(Update, 0x73DB40);
    RH_ScopedInstall(SetUpPelletCol, 0x73C710);
    RH_ScopedInstall(FireAreaEffect, 0x73E800);
    RH_ScopedInstall(FireInstantHitFromCar, 0x73EC40);
    RH_ScopedInstall(FireFromCar, 0x73FA20);
    RH_ScopedInstall(FireInstantHit, 0x73FB10);
    RH_ScopedInstall(FireProjectile, 0x741360);
    RH_ScopedInstall(DoBulletImpact, 0x73B550);
    RH_ScopedInstall(LaserScopeDot, 0x73A8D0);
    RH_ScopedInstall(FireM16_1stPerson, 0x741C00);
    RH_ScopedInstall(Fire, 0x742300);
    RH_ScopedGlobalInstall(DoTankDoomAiming, 0x73D1E0);
    RH_ScopedGlobalInstall(DoDriveByAutoAiming, 0x73D720);
    RH_ScopedGlobalInstall(FindNearestTargetEntityWithScreenCoors, 0x73E240);
    RH_ScopedGlobalInstall(EvaluateTargetForHeatSeekingMissile, 0x73E560);
    RH_ScopedGlobalInstall(CheckForShootingVehicleOccupant, 0x73F480);
    RH_ScopedGlobalInstall(PickTargetForHeatSeekingMissile, 0x73F910);
    RH_ScopedOverloadedInstall(CanBeUsedFor2Player, "Static", 0x73B240, bool(*)(eWeaponType));
    RH_ScopedOverloadedInstall(CanBeUsedFor2Player, "Method", 0x73DEF0, bool(CWeapon::*)());
    RH_ScopedGlobalInstall(FireOneInstantHitRound, 0x73AF00);
}

// 0x73B430
CWeapon::CWeapon(eWeaponType weaponType, uint32 ammo) :
    m_Type{ weaponType },
    m_TotalAmmo{ std::min<uint32>(ammo, 99'999) }
{
    Reload();
}

// 0x73B4A0
void CWeapon::Initialise(eWeaponType weaponType, int32 ammo, CPed* owner) {
    m_Type = weaponType;
    m_State = eWeaponState::WEAPONSTATE_READY;
    m_AmmoInClip = 0;
    m_TotalAmmo = std::min(ammo, 99999);

    Reload(owner);

    m_TimeForNextShotMs = 0;

    for (const auto m : CWeaponInfo::GetWeaponInfo(m_Type)->GetModels()) {
        if (m != MODEL_INVALID) {
            CModelInfo::GetModelInfo(m)->AddRef();
        }
    }

    m_FxSystem = nullptr;
    m_DontPlaceInHand = false;
}

// 0x73A300
void CWeapon::InitialiseWeapons() {
    CWeaponInfo::Initialise();
    CShotInfo::Initialise();
    CExplosion::Initialise();
    CProjectileInfo::Initialise();
    CBulletInfo::Initialise();

    bPhotographHasBeenTaken = false;
    ms_bTakePhoto = false;
}

// 0x73A330
void CWeapon::ShutdownWeapons() {
    CWeaponInfo::Shutdown();
    CShotInfo::Shutdown();
    CExplosion::Shutdown();
    CProjectileInfo::Shutdown();
    CBulletInfo::Shutdown();

    ms_PelletTestCol.RemoveCollisionVolumes();
}

// 0x73A380
void CWeapon::Shutdown() {
    for (const auto m : CWeaponInfo::GetWeaponInfo(m_Type)->GetModels()) {
        if (m != MODEL_INVALID) {
            CModelInfo::GetModelInfo(m)->RemoveRef();
        }
    }

    m_Type            = eWeaponType::WEAPON_UNARMED;
    m_State           = eWeaponState::WEAPONSTATE_READY;
    m_TotalAmmo       = 0;
    m_AmmoInClip      = 0;
    m_TimeForNextShotMs = 0;
}

// 0x73A3E0
void CWeapon::AddGunshell(CEntity* creator, CVector& position, const CVector2D& direction, float size) {
    if (!creator || !creator->GetIsOnScreen()) {
        return;
    }

    // originally squared
    if ((creator->GetPosition() - TheCamera.GetPosition()).SquaredMagnitude() > sq(10.0f)) {
        return;
    }

    // Add gunshell fx particle
    FxPrtMult_c fxprt(0.5f, 0.5f, 0.5f, 1.0f, size, 1.0f, 1.0f);
    switch (m_Type) {
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SHOTGUN:
        fxprt.SetColor(0.6f, 0.1f, 0.1f);
    }
    g_fx.m_GunShell->AddParticle(position, { direction.x, direction.y, CGeneral::GetRandomNumberInRange(0.4f, 1.6f) }, 0.0f, fxprt);
}

// 0x73A530
bool CWeapon::GenerateDamageEvent(CPed* victim, CEntity* creator, eWeaponType weaponType, int32 damageFactor, ePedPieceTypes pedPiece, uint8 direction) {
    CPedDamageResponseCalculator pedDmgRespCalc{
        creator,
        (float)damageFactor,
        weaponType,
        pedPiece,
        false
    };

    CEventDamage eventDmg{
        creator,
        CTimer::GetTimeInMS(),
        weaponType,
        pedPiece,
        direction,
        false,
        victim->bInVehicle
    };

    if (   victim->m_fHealth <= 0.f
        && CLocalisation::Blood()
        && CLocalisation::KickingWhenDown()
        && victim->GetTaskManager().GetSimplestActiveTask()->GetTaskType() == TASK_SIMPLE_DEAD
    ) {
        const auto floorHitAnim = CAnimManager::BlendAnimation(
            victim->GetRpClump(),
            ANIM_GROUP_DEFAULT,
            RpAnimBlendClumpGetFirstAssociation(victim->GetRpClump(), ANIMATION_IS_FRONT)
                ? ANIM_ID_FLOOR_HIT_F
                : ANIM_ID_FLOOR_HIT
        );
        if (floorHitAnim) {
            floorHitAnim->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE, false);
            floorHitAnim->Start();
        }
        return true;
    }

    if (!victim->IsAlive()) {
        return true;
    }

    if (!eventDmg.AffectsPed(victim)) { // 0x73A687
        return false;
    }

    if (creator == FindPlayerPed()) {
        CCrime::ReportCrime(CRIME_DAMAGED_PED, victim, static_cast<CPed*>(creator));
    }

    pedDmgRespCalc.ComputeDamageResponse(
        victim,
        eventDmg.m_damageResponse,
        true
    );

    bool ret = true;
    if (!victim->bInVehicle && (
           (!notsa::IsFixBugs() || CWeaponInfo::TypeIsWeapon(weaponType)) && CWeaponInfo::GetWeaponInfo(weaponType)->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE
        || weaponType == WEAPON_FALL && creator && creator->GetIsTypeObject()
    )) { // 0x73A6F1
        eventDmg.ComputeAnim(victim, true);
        switch (eventDmg.m_nAnimID) {
        case ANIM_ID_SHOT_PARTIAL:
        case ANIM_ID_SHOT_LEFTP:
        case ANIM_ID_SHOT_PARTIAL_B:
        case ANIM_ID_SHOT_RIGHTP: { //> 0x73A769 - Inverted
            auto anim = RpAnimBlendClumpGetAssociation(victim->GetRpClump(), eventDmg.m_nAnimID);
            if (!anim) {
                anim = CAnimManager::AddAnimation(
                    victim->GetRpClump(),
                    (AssocGroupId)eventDmg.m_nAnimGroup,
                    (AnimationId)eventDmg.m_nAnimID
                );
            }
            anim->m_BlendAmount = 0.f;
            anim->m_BlendDelta = eventDmg.m_fAnimBlend;
            anim->m_Speed = eventDmg.m_fAnimSpeed;
            anim->Start();
            break;
        }
        case ANIM_ID_NO_ANIMATION_SET:
            break;
        case ANIM_ID_DOOR_LHINGE_O:
            ret = false;
            break;
        default: { //< 0x73A7B5
            const auto a = CAnimManager::BlendAnimation(
                victim->GetRpClump(),
                (AssocGroupId)eventDmg.m_nAnimGroup,
                (AnimationId)eventDmg.m_nAnimID,
                eventDmg.m_fAnimBlend
            );
            a->m_Speed = eventDmg.m_fAnimSpeed;
            a->SetFlag(ANIMATION_IS_PLAYING);
            break;
        }
        }
    }

    // 0x73A828
    eventDmg.m_bStealthMode =
           creator
        && creator->GetIsTypePed()
        && (weaponType == WEAPON_PISTOL_SILENCED || creator->AsPed()->GetTaskManager().GetActiveTask()->GetTaskType() == TASK_SIMPLE_STEALTH_KILL);

    if (!victim->bInVehicle || victim->m_fHealth <= 0.f || !victim->GetTaskManager().GetActiveTask() || victim->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_SIMPLE_GANG_DRIVEBY) {
        victim->GetEventGroup().Add(eventDmg);
    }

    return ret;

}

// 0x73A8D0
bool CWeapon::LaserScopeDot(CVector* outCoord, float* outSize) {
    /* UNUSED */
    NOTSA_UNREACHABLE();
}

// 0x73AAC0
bool CWeapon::FireSniper(CPed* shooter, CEntity* victim, CVector* target) {
    const CCam& activeCam = CCamera::GetActiveCamera();

    if (FindPlayerPed() == shooter) {
        switch (activeCam.m_nMode) {
        case MODE_M16_1STPERSON:
        case MODE_SNIPER:
        case MODE_CAMERA:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON_RUNABOUT:
        case MODE_SNIPER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
            break;
        default:
            return false;
        }
    }

    // todo: make sense of literals.
    float vecFrontZ_Y = activeCam.m_vecFront.z * 0.145f - activeCam.m_vecFront.y * 0.98940003f;

    if (vecFrontZ_Y > 0.99699998f)
        CCoronas::MoonSize = (CCoronas::MoonSize + 1) % 8;

    CVector velocity = activeCam.m_vecFront;
    velocity.Normalise();
    velocity *= 16.0f;

    CBulletInfo::AddBullet(shooter, m_Type, activeCam.m_vecSource, velocity);

    // recoil effect for players
    if (shooter->IsPlayer()) {
        CVector creatorPos = FindPlayerCoors();
        CPad* creatorPad = CPad::GetPad(shooter->m_nPedType);

        creatorPad->StartShake_Distance(240, 128, creatorPos);
        CamShakeNoPos(&TheCamera, 0.2f);
    }

    if (shooter->GetIsTypePed()) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, shooter, shooter);
    } else if (shooter->GetIsTypeVehicle() && shooter->m_roadRageWith) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, shooter, shooter->m_roadRageWith);
    }

    CVector targetPoint = velocity * 40.0f + activeCam.m_vecSource;
    bool hasNoSound = m_Type == eWeaponType::WEAPON_PISTOL_SILENCED || m_Type == eWeaponType::WEAPON_TEARGAS;
    CEventGroup* eventGroup = GetEventGlobalGroup();

    CEventGunShot gs(shooter, activeCam.m_vecSource, targetPoint, hasNoSound);
    eventGroup->Add(static_cast<CEvent*>(&gs), false);

    CEventGunShotWhizzedBy gsw(shooter, activeCam.m_vecSource, targetPoint, hasNoSound);
    eventGroup->Add(static_cast<CEvent*>(&gsw), false);

    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_22, shooter);

    return true;
}

// 0x73AEB0
void CWeapon::Reload(CPed* owner) {
    if (!m_TotalAmmo) {
        return;
    }

    uint32 ammo = GetWeaponInfo(owner).m_nAmmoClip;
    m_AmmoInClip = std::min(ammo, m_TotalAmmo);
}

// 0x73B1C0
bool CWeapon::IsTypeMelee() {
    return GetWeaponInfo().m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE;
}

// 0x73B1E0
bool CWeapon::IsType2Handed() {
    switch (m_Type) {
    case eWeaponType::WEAPON_M4:
    case eWeaponType::WEAPON_AK47:
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SHOTGUN:
    case eWeaponType::WEAPON_SNIPERRIFLE:
    case eWeaponType::WEAPON_FLAMETHROWER:
    case eWeaponType::WEAPON_COUNTRYRIFLE:
        return true;
    }
    return false;
}

// 0x73B210
bool CWeapon::IsTypeProjectile() {
    switch (m_Type) {
    case eWeaponType::WEAPON_GRENADE:
    case eWeaponType::WEAPON_REMOTE_SATCHEL_CHARGE:
    case eWeaponType::WEAPON_TEARGAS:
    case eWeaponType::WEAPON_MOLOTOV:
    case eWeaponType::WEAPON_FREEFALL_BOMB:
        return true;
    }
    return false;
}

// 0x73B240
bool CWeapon::CanBeUsedFor2Player(eWeaponType weaponType) {
    switch (weaponType) {
    case eWeaponType::WEAPON_CHAINSAW:
    case eWeaponType::WEAPON_SNIPERRIFLE:
    case eWeaponType::WEAPON_RLAUNCHER:
    case eWeaponType::WEAPON_PARACHUTE:
        return false;
    }
    return true;
}

// 0x73B2A0
bool CWeapon::HasWeaponAmmoToBeUsed() {
    switch (m_Type) {
    case eWeaponType::WEAPON_UNARMED:
    case eWeaponType::WEAPON_BRASSKNUCKLE:
    case eWeaponType::WEAPON_GOLFCLUB:
    case eWeaponType::WEAPON_NIGHTSTICK:
    case eWeaponType::WEAPON_KNIFE:
    case eWeaponType::WEAPON_BASEBALLBAT:
    case eWeaponType::WEAPON_KATANA:
    case eWeaponType::WEAPON_CHAINSAW:
    case eWeaponType::WEAPON_DILDO1:
    case eWeaponType::WEAPON_DILDO2:
    case eWeaponType::WEAPON_VIBE1:
    case eWeaponType::WEAPON_VIBE2:
    case eWeaponType::WEAPON_FLOWERS:
    case eWeaponType::WEAPON_PARACHUTE:
        return true;
    }
    return m_TotalAmmo != 0;
}

// 0x73B300
bool CWeapon::ProcessLineOfSight(const CVector& startPoint, const CVector& endPoint, CColPoint& outColPoint, CEntity*& outEntity, eWeaponType weaponType, CEntity* arg5,
                                 bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool arg11, bool doIgnoreCameraCheck) {
    CBirds::HandleGunShot(&startPoint, &endPoint);
    CShadows::GunShotSetsOilOnFire(startPoint, endPoint);
    return CWorld::ProcessLineOfSight(startPoint, endPoint, outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, false, doIgnoreCameraCheck, true);
}

// 0x73B360
void CWeapon::StopWeaponEffect() {
    if (m_FxSystem && m_Type != WEAPON_MOLOTOV) {
        m_FxSystem->Kill();
        m_FxSystem = nullptr;
    }
}

// 0x73B380
float CWeapon::TargetWeaponRangeMultiplier(CEntity* target, CEntity* weaponOwner) {
    if (!target || !weaponOwner) {
        return 1.0f;
    }

    switch (target->GetType()) {
    case ENTITY_TYPE_VEHICLE: {
        if (!target->AsVehicle()->IsBike()) {
            return 3.0f;
        }
        break;
    }
    case ENTITY_TYPE_PED: {
        CPed* pedVictim = target->AsPed();

        if (pedVictim->m_pVehicle && !pedVictim->m_pVehicle->IsBike()) {
            return 3.0f;
        }

        if (CEntity* attachedTo = pedVictim->m_pAttachedTo) {
            if (attachedTo->GetIsTypeVehicle() && !attachedTo->AsVehicle()->IsBike()) {
                return 3.0f;
            }
        }

        break;
    }
    }

    if (!weaponOwner->GetIsTypePed() || !weaponOwner->AsPed()->IsPlayer()) {
        return 1.0f;
    }

    switch (CCamera::GetActiveCamera().m_nMode) {
    case MODE_TWOPLAYER_IN_CAR_AND_SHOOTING:
        return 2.0f;
    case MODE_HELICANNON_1STPERSON:
        return 3.0f;
    }

    return 1.0f;
}

// 0x73B550
void CWeapon::DoBulletImpact(CEntity* firedBy, CEntity* victim, const CVector& startPoint, const CVector& endPoint, const CColPoint& hitCP, int32 incrementalHit) {
    const auto firedByPed = firedBy->GetIsTypePed()
        ? firedBy->AsPed()
        : nullptr;
    const auto firedByPlayer = firedByPed && firedByPed->IsPlayer()
        ? firedByPed->AsPlayer()
        : nullptr;

    const auto wi = &GetWeaponInfo(firedByPed);

    if (firedByPed && firedByPed->IsPlayer()) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, victim, firedByPed);
    }

    if (victim) { // Inverted
        CBulletTraces::AddTrace( // 0x73B60C
            incrementalHit
                ? startPoint + (hitCP.m_vecPoint - startPoint) * 0.4f
                : startPoint,
            hitCP.m_vecPoint,
            GetType(),
            firedBy
        );

        const auto DoBulletHitFx = [&] {
            if (incrementalHit <= 0) {
                const auto angle = (endPoint - startPoint).Normalized().Dot(hitCP.m_vecNormal);
                if (angle < 0.f) { // Normal is opposite to that of the bullet's direction
                    AudioEngine.ReportBulletHit(
                        victim,
                        hitCP.m_nSurfaceTypeB,
                        hitCP.m_vecPoint,
                        RWRAD2DEG(std::asin(-angle))
                    );
                }
            }
        };

#ifdef FIX_BUGS
        if (firedByPlayer && (firedByPlayer != victim || firedBy->GetStatus() == STATUS_PLAYER)) { // 0x73B6D0
#else
        if (firedByPlayer && firedByPlayer != victim || firedBy->GetStatus() == STATUS_PLAYER) { // 0x73B6D0
#endif
            if (notsa::contains({ ENTITY_TYPE_PED, ENTITY_TYPE_VEHICLE, ENTITY_TYPE_OBJECT }, victim->GetType())) {
                if (CStats::GetStatValue(STAT_BULLETS_FIRED) >= CStats::GetStatValue(STAT_BULLETS_THAT_HIT)) {
                    CStats::IncrementStat(STAT_BULLETS_THAT_HIT);
                }
            }
            if (CWeaponInfo::TypeHasSkillStats(GetType())) { // 0x73B738
                // Redundant `if` check here was removed
                if ([&]{
                    const auto victimEntity = notsa::coalesce(firedByPlayer->m_pTargetedObject, victim);

                    // NOTE: The code is written upside down to make the controlflow easier

                    if (victimEntity->GetIsTypePed() && CPedGroups::AreInSameGroup(victimEntity->AsPed(), firedByPed)) {
                        return false;
                    }

                    switch (victimEntity->GetType()) {
                    case eEntityType::ENTITY_TYPE_PED: {
                        const auto victimPed = victimEntity->AsPed();
                        return !CPedGroups::AreInSameGroup(victimPed, firedByPed) && victimPed->m_fHealth > 0.f;
                    }
                    case eEntityType::ENTITY_TYPE_VEHICLE: {
                        const auto victimVeh = victimEntity->AsVehicle();
                        if (notsa::contains(eCarPiece_WheelPieces, (eCarPiece)hitCP.m_nPieceTypeB)) {
                            if (!victimVeh->BurstTyre(hitCP.m_nPieceTypeB, true)) {
                                return false;
                            }
                        }
                        if (victimVeh->physicalFlags.bBulletProof || !victimVeh->vehicleFlags.bCanBeDamaged) {
                            return false;
                        }
                        if (victimVeh->m_fHealth <= 0.f || victimVeh->GetStatus() == STATUS_WRECKED) {
                            return false;
                        }
                        return true;
                    }
                    case eEntityType::ENTITY_TYPE_OBJECT: {
                        const auto victimObj = victimEntity->AsObject();
                        return victimObj->m_fHealth > 0.f && victimObj->m_nColDamageEffect && victimObj->m_pObjectInfo->m_fColDamageMultiplier >= 0.5f;
                    }
                    }
                    return false;
                }()) {
                    CStats::UpdateStatsWhenWeaponHit(GetType());
                }
                
            }
        }

        if (!victim->GetIsTypePed()) { // 0x73B85B
            CGlass::WasGlassHitByBullet(victim, hitCP.m_vecPoint);

            const auto DoBulletImpactFx = [&] {
                if (TheCamera.IsSphereVisible(hitCP.m_vecPoint, 1.f)) {
                    g_fx.AddBulletImpact(
                        hitCP.m_vecPoint,
                        hitCP.m_vecNormal,
                        hitCP.m_nSurfaceTypeB,
                        incrementalHit ? 2 : 8,
                        hitCP.m_nLightingA.GetCurrentLighting()
                    );
                }
            };

            switch (victim->GetType()) {
            case eEntityType::ENTITY_TYPE_BUILDING: { // 0x73C014
                DoBulletImpactFx();
                if (firedByPlayer) {
                    firedByPlayer->GetPlayerData()->m_nModelIndexOfLastBuildingShot = victim->m_nModelIndex;
                }
                break;
            }
            case eEntityType::ENTITY_TYPE_VEHICLE: { // 0x73BD2A
                const auto victimVeh = victim->AsVehicle();
                if (!notsa::contains(eCarPiece_WheelPieces, (eCarPiece)hitCP.m_nPieceTypeB)) {
                    victimVeh->InflictDamage(
                        firedBy,
                        GetType(),
                        (float)(firedByPlayer && TheCamera.GetActiveCam().m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING ? 2 * wi->m_nDamage : wi->m_nDamage),
                        hitCP.m_vecPoint
                    );
                    DoBulletImpactFx();
                    if (g_LoadMonitor.GetProcLevel() != eProcessingLevel::HIGH) { // 0x73BF6C - NOTE/TODO: Useless, remove
                        const auto wepForceMult = [this]{
                            switch (GetType()) {
                            case WEAPON_DESERT_EAGLE:
                            case WEAPON_MINIGUN:
                                return -20.f;
                            case WEAPON_SHOTGUN:
                            case WEAPON_SPAS12_SHOTGUN:
                                return -4.0f;
                            default:
                                return -10.f;
                            }
                        }();
                        victimVeh->ApplyForce(
                            hitCP.m_vecNormal * (wepForceMult * std::min(1.f, victimVeh->m_fMass / 1000.f)),
                            hitCP.m_vecPoint - victimVeh->GetPosition(),
                            true
                        );
                    }
                } else { // 0x73BD3F
                    victimVeh->BurstTyre(hitCP.m_nPieceTypeB, true);
                    g_fx.AddTyreBurst(hitCP.m_vecPoint, hitCP.m_vecNormal);
                    if (firedByPed) { // Add event to occupants
                        const auto AddEventVehicleDamageWeapon = [&](CPed* ped) {
                            if (ped) {
                                ped->GetEventGroup().Add(CEventVehicleDamageWeapon{
                                    victimVeh,
                                    firedBy,
                                    GetType()
                                });
                            }
                        };
                        AddEventVehicleDamageWeapon(victimVeh->m_pDriver);
                        rng::for_each(victimVeh->GetPassengers(), AddEventVehicleDamageWeapon);
                    }
                }
                break;
            }
            case eEntityType::ENTITY_TYPE_OBJECT: { // 0x73BB4F
                const auto victimObj = victim->AsObject();
                const auto oinfo     = victimObj->m_pObjectInfo;

                DoBulletImpactFx();
                if (victimObj->m_nColDamageEffect < 200) {
                    if (!victimObj->physicalFlags.bDisableCollisionForce && oinfo->m_fColDamageMultiplier < 99.9f) {
                        if (victimObj->GetIsStatic() && oinfo->m_fUprootLimit <= 0.f) {
                            victimObj->SetIsStatic(false);
                            victimObj->AddToMovingList();
                        }
                        if (!victimObj->GetIsStatic()) { // 0x73BC6B - Move the object a little
                            float force = -2.f;
                            if (victimObj->physicalFlags.bDisableZ || victimObj->physicalFlags.bDisableMoveForce) {
                                force *= 0.1f;
                            }
                            if (incrementalHit) {
                                force *= 0.2f;
                            }
                            victimObj->ApplyForce(
                                hitCP.m_vecNormal * force,
                                hitCP.m_vecPoint - victimObj->GetPosition(),
                                true
                            );
                        }
                    }
                } else { // 0x73BB94
                    victimObj->ObjectDamage(
                        [&]{
                            switch (oinfo->m_nGunBreakMode) {
                            case 1:  return 151.f;
                            case 2:  return 151.f * oinfo->m_fSmashMultiplier;
                            default: return 50.f;
                            }
                        }(),
                        &hitCP.m_vecPoint,
                        &hitCP.m_vecNormal,
                        firedBy,
                        GetType()
                    );
                }

                break;
            }
            }
            DoBulletHitFx();
        } else if (victim != firedBy) {
            const auto victimPed = victim->AsPed();
            if (   !firedByPed
                || firedByPed->m_nPedType != victimPed->m_nPedType
                || notsa::contains({ PED_TYPE_CIVMALE, PED_TYPE_CIVFEMALE }, victimPed->m_nPedType)
                || firedByPlayer
            ) {
                const auto bAddBloodFx = [&]{
                    if (incrementalHit > 0) { // 0x73B8B8
                        return false;
                    }
                    DoBulletHitFx();
                    return GenerateDamageEvent(
                        victimPed,
                        firedBy,
                        GetType(),
                        [&] {
                            if (firedByPlayer
                                && (victim->GetPosition() - startPoint).SquaredMagnitude() <= 1.f
                                && !victimPed->bNoCriticalHits
                                && !notsa::contains({ WEAPON_SHOTGUN, WEAPON_SPAS12_SHOTGUN }, GetType())
                            ) {
                                return 150;
                            }
                            return incrementalHit < 0
                                ? -(incrementalHit * (int32)wi->m_nDamage)
                                : (int32)wi->m_nDamage;
                        }(),
                        (ePedPieceTypes)hitCP.m_nPieceTypeB,
                        victimPed->GetLocalDirection(startPoint - victimPed->GetPosition2D())
                    );
                }();
                if (firedByPlayer) { // 0x73BA79
                    CCrime::ReportCrime(CRIME_DAMAGE_CAR, victim, firedByPed);
                }
                if (CLocalisation::Blood() && bAddBloodFx) { // 0x73BA81
                    g_fx.AddBlood(
                        hitCP.m_vecPoint,
                        hitCP.m_vecNormal,
                        hitCP.m_nPieceTypeB == ePedPieceTypes::PED_PIECE_HEAD
                                ? victimPed->m_fHealth <= 0.f ? 32 : 16
                                : incrementalHit ? 4 : 8,
                        victimPed->m_fContactSurfaceBrightness
                    );
                }
            }
        }
    } else {
        CBulletTraces::AddTrace(startPoint, endPoint, GetType(), firedBy);
    }

    // 0x73C11B [Moved down here]
    if (firedByPed && firedByPed->IsPlayer()) { // 0x73C14B
        firedByPed->AsPlayer()->GetPadFromPlayer()->StartShake_Distance(
            240,
            128,
            FindPlayerPed()->GetPosition()
        );
    }
}

// 0x73C1F0
bool CWeapon::TakePhotograph(CEntity* owner, const CVector* point) {
    UNUSED(owner);

    if (point) {
        if (const auto fx = g_fxMan.CreateFxSystem("camflash", *point, nullptr, false)) {
            fx->PlayAndKill();
        }
    }

    if (CCamera::GetActiveCamera().m_nMode != MODE_CAMERA) {
        return false;
    }

    CPickups::PictureTaken();
    bPhotographHasBeenTaken = true;
    ms_bTakePhoto = true;
    CStats::IncrementStat(STAT_PHOTOGRAPHS_TAKEN, 1.0f);

    // NOTSA - Optimization
    const auto& camMat = TheCamera.GetMatrix();
    const auto& camPos = camMat.GetPosition();

    const auto IsPosInRange = [&](const CVector& worldPos) {
        return (camPos - worldPos).SquaredMagnitude() >= sq(125.f); // NOTSA: Using squared mag.
    };

    // Check is in position in camera's frame
    const auto IsPosInCamFrame = [](const CVector& worldPos) {
        CVector pedHeadPos_Screen;
        if (float _w, _h; !CSprite::CalcScreenCoors(worldPos, &pedHeadPos_Screen, &_w, &_h, false, true)) {
            return false;
        }

        // TODO/BUG: Possibly buggy on bigger screens, because the border becomes too big (because of the relative multiplier) maybe?
        if (   (SCREEN_WIDTH * 0.1f >= pedHeadPos_Screen.x || pedHeadPos_Screen.x >= SCREEN_WIDTH * 0.9f)
            || (SCREEN_HEIGHT * 0.1f >= pedHeadPos_Screen.y || pedHeadPos_Screen.y >= SCREEN_HEIGHT * 0.9f)
        ) {
            return false;
        }

        return true;
    };

    const auto CheckIsLOSBlocked = [&, camFwd = camMat.GetForward()](const CVector& target, CEntity* ignore) {
        CColPoint _cp; // Unused
        CEntity* hitEntity{};
        if (!CWorld::ProcessLineOfSight(
            camPos + camFwd * 2.f,
            target,
            _cp,
            hitEntity,
            true,
            true,
            true,
            true,
            true,
            true,
            false,
            false
        ) || hitEntity == ignore) { // TODO: Here we could set CWorld::pIgnoreEntity to `&ped`, instead of this check.
            return false;
        }
        return true;
    };

    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (IsPosInRange(ped.GetPosition())) {
            continue;
        }

        const auto pedHeadPos = ped.GetBonePosition(BONE_HEAD);

        if (!IsPosInCamFrame(pedHeadPos)) {
            continue;
        }

        if (!CheckIsLOSBlocked(
            pedHeadPos + Normalized(camPos - pedHeadPos) * 1.5f, // Point from ped's head towards camera
            &ped
        )) {
            ped.bHasBeenPhotographed = true;
        }
    }

    for (auto& obj : GetObjectPool()->GetAllValid()) {
        const auto& objPos = obj.GetPosition();

        if (!IsPosInRange(objPos) || !IsPosInCamFrame(objPos)) {
            continue;
        }

        if (!CheckIsLOSBlocked(objPos, &obj)) {
            obj.objectFlags.bIsPhotographed = true;
        }
    }

    return true;
}

// 0x73C710
void CWeapon::SetUpPelletCol(int32 numPellets, CEntity* owner, CEntity* victim, CVector& point, CColPoint& colPoint, CMatrix& outMat) {
    constexpr int32 MAX_NUM_PELLETS = 15;

    assert(numPellets <= MAX_NUM_PELLETS);

    auto* const cm = &ms_PelletTestCol;
    if (!cm->GetData()) {
        cm->AllocateData(0, 0, MAX_NUM_PELLETS, 0, 0, false);
        cm->GetBoundingSphere().Set(1.f, {0.f, 0.f, 0.f});
        cm->m_nColSlot = 0;
    }
    auto* const cd = ms_PelletTestCol.GetData();

    auto hitDir = (colPoint.m_vecPoint - point);
    const float depth = hitDir.NormaliseAndMag() * CWorld::fWeaponSpreadRate * 1.3f;

    //> 0x73C806 - Create pellet lines
    cd->m_nNumLines = (uint8)numPellets;
    const auto lines = cd->GetLines();
    lines[0].Set(
        { 0.f, -depth, 0.f },
        { 0.f,  depth, 0.f }
    );
    for (int32 i = 1; i < numPellets; i++) {
        const auto angle  = CGeneral::GetRandomNumberInRange(-PI, PI);
        const auto spread = CGeneral::GetRandomNumberInRange(0.f, depth * 0.8f);

        const auto oX = std::cos(angle) * spread;
        const auto oZ = std::sin(angle) * spread;

        lines[i].Set(
            { oX, -depth * 2.f, oZ },
            { oX,  depth * 2.f, oZ }
        );
    }

    //> 0x73C923 -  Calculate bounding volumes
    cm->GetBoundingBox().Set(
        { -depth, -depth * 2.f, -depth },
        {  depth,  depth * 2.f,  depth }
    );
    cm->GetBoundingSphere().Set(
        depth * 2.5f,
        {0.f, 0.f, 0.f}
    );

    const auto CalculateMatrixRotation = [&](CVector fwd, CVector zaxis) {
        const auto r = zaxis.Cross(fwd).Normalized();
        outMat.GetForward() = fwd;
        outMat.GetRight()   = r;
        outMat.GetUp()      = r.Cross(fwd);
    };

    if (victim->GetIsTypeBuilding()) { // 0x73C98E
        const auto& n = colPoint.m_vecNormal;
        CalculateMatrixRotation(
            -n,
            std::abs(n.z) >= 0.9f
                ? CVector{0.f, 1.f, 0.f}
                : CVector{1.f, 0.f, 0.f}
        );

    } else  if (std::abs(hitDir.z) <= 0.9f) { // 0x73CA4C
        CalculateMatrixRotation(
            hitDir,
            {0.f, 0.f, 1.f}
        );
    } else if (!owner->GetIsTypePed()) { // 0x73CA59
        CalculateMatrixRotation(
            hitDir,
            {1.f, 0.f, 0.f}
        );
    } else { // 0x73CA5B
        CalculateMatrixRotation(
            hitDir,
            owner->GetForward()
        );
    }

    // 0x73CAFF
    outMat.GetPosition() = colPoint.m_vecPoint;

    // 0x73CB1A
    if (!victim->GetIsTypeBuilding()) {
        outMat.GetPosition() -= colPoint.m_vecNormal.ProjectOnToNormal(outMat.GetForward()) * depth;
    }
}

// 0x73CBA0
void CWeapon::FireInstantHitFromCar2(CVector startPoint, CVector endPoint, CVehicle* vehicle, CEntity* owner) {
    CCrime::ReportCrime(CRIME_FIRE_WEAPON, FindPlayerPed(), FindPlayerPed());

    GetEventGlobalGroup()->Add(CEventGunShot{
        notsa::coalesce<CEntity*>(owner, vehicle),
        startPoint,
        endPoint,
        notsa::contains({WEAPON_PISTOL_SILENCED, WEAPON_TEARGAS}, GetType())
    });
    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_22, owner);

    CPointLights::AddLight(PLTYPE_POINTLIGHT, startPoint, {}, 3.0f, 0.25f, 0.22f, 0.0f, 0, false, nullptr);
    CWorld::bIncludeBikers = true;
    CWorld::pIgnoreEntity = vehicle;
    CBirds::HandleGunShot(&startPoint, &endPoint);
    CShadows::GunShotSetsOilOnFire(startPoint, endPoint);

    CEntity* victim{};
    CColPoint cpImpact{};
    CWorld::ProcessLineOfSight(startPoint, endPoint, cpImpact, victim, true, true, true, true, true, false, false, true);
    CWorld::ResetLineTestOptions();
    DoBulletImpact(owner, victim, startPoint, endPoint, cpImpact, 0);
}

// 0x73CDC0
void CWeapon::DoDoomAiming(CEntity* owner, CVector* start, CVector* end) {
    int16 inRangeCount{};
    std::array<CEntity*, 16> objInRange{};
    CWorld::FindObjectsInRange(*start, (*start - *end).Magnitude(), true, &inRangeCount, (int16)objInRange.size(), objInRange.data(), false, true, true, false, false);

    CEntity* closestEntity{};
    float    closestDist{ 10'000 };
    for (auto entity : std::span{ objInRange.begin(), (size_t)inRangeCount }) {
        if (entity == owner || owner->AsPed()->CanSeeEntity(entity, PI / 8.f)) { // todo: add check owner->GetIsTypePed() NOTSA
            continue;
        }

        switch (entity->GetStatus()) {
        case STATUS_TRAIN_MOVING:
        case STATUS_TRAIN_NOT_MOVING:
        case STATUS_WRECKED:
            continue;
        }

        const auto dir = entity->GetPosition() - owner->GetPosition();
        if (const auto dist2D = dir.Magnitude2D(); std::abs(dir.z) * 1.5f < dist2D) {
            const auto dist3D = std::hypot(dist2D, dir.z);
            if (dist3D < closestDist) {
                closestEntity = entity;
                closestDist = dist3D;
            }
        }
    }

    if (closestDist < 9000.f) {
        // assert(closestEntity); // We should have one, because by default `closestDist` is FLT_MAX (originally 10 000)

        {
            CEntity*  _hitEntity{}; // Unused
            CColPoint _cp;          // Unused
            if (CWorld::ProcessLineOfSight(*start, closestEntity->GetPosition(), _cp, _hitEntity, true, false, false, false, false, false, false, true)) {
                return;
            }
        }

        float targetZ = closestEntity->GetPosition().z + 0.3f;
        if (closestEntity->GetIsTypePed() && closestEntity->AsPed()->bIsDucking) {
            targetZ -= 0.8f; // Effectively only -0.5 relative to the original Z
        }
        const auto t = (*start - *end).Magnitude2D() / (*start - closestEntity->GetPosition()).Magnitude2D();
        end->z = start->z + (targetZ - start->z) * t; // Re-ordered a little
    }
}

// 0x73D1E0
void CWeapon::DoTankDoomAiming(CEntity* vehicle, CEntity* owner, CVector* startPoint, CVector* endPoint) {
    const auto ray=*endPoint-*startPoint;
    const float length=std::sqrt((ray.y*ray.y+ray.z*ray.z)+ray.x*ray.x);
    CEntity* entities[16];int16 count=0;
    CWorld::FindObjectsInRange(*startPoint,length,true,&count,15,entities,false,true,false,false,false);
    const float slope=ray.z/std::sqrt((ray.z*ray.z+ray.y*ray.y)+ray.x*ray.x);
    float closest=10000.0f;int16 selected=0;
    const CVector flatStart{startPoint->x,startPoint->y,0},flatEnd{endPoint->x,endPoint->y,0};
    for(int16 i=0;i<count;++i){
        auto* entity=entities[i];if(entity==vehicle||entity==owner)continue;
        const auto status=uint32(entity->GetStatus());
        if(status==6||status==7||(entity->GetIsTypeVehicle()&&(static_cast<CPhysical*>(entity)->m_nPhysicalFlags&0x20000000u)))continue;
        const auto& origin=vehicle->GetMatrix().GetPosition();const auto& target=entity->GetMatrix().GetPosition();
        const auto delta=origin-target;
        const float planar=std::sqrt(delta.y*delta.y+delta.x*delta.x);
        const float difference=origin.z-(planar*slope+target.z);
        const float vertical=difference>=0.0f?difference:-difference;
        if(!(vertical*3.0f<planar))continue;
        const CVector flatTarget{entity->GetPosition().x,entity->GetPosition().y,0};
        const auto* model=CModelInfo::GetModelInfo(int16(entity->GetModelIndex()));
        const float radius=model->GetColModel()->m_boundSphere.m_fRadius;
        if(!(CCollision::DistToLine(flatStart,flatEnd,flatTarget)<radius*3.0f))continue;
        const float distance=std::sqrt(planar*planar+vertical*vertical);
        if(distance<closest){closest=distance;selected=i;}
    }
    if(closest<9000.0f){
        const auto& target=entities[selected]->GetPosition();
        const float planarRay=std::sqrt(ray.y*ray.y+ray.x*ray.x);
        const auto delta=target-*startPoint;
        const float planarTarget=std::sqrt(delta.y*delta.y+delta.x*delta.x);
        endPoint->z=(planarRay/planarTarget)*((target.z+0.3f)-startPoint->z)+startPoint->z;
    }
}

// 0x73D720
void CWeapon::DoDriveByAutoAiming(CEntity* owner, CVehicle* vehicle, CVector* startPoint, CVector* endPoint, bool canAimVehicles) {
    const auto magnitude = [](const CVector& v) {
        return std::sqrt((v.z * v.z + v.y * v.y) + v.x * v.x);
    };
    const auto ray = *endPoint - *startPoint;
    const auto rayLength = magnitude(ray);
    if (!owner) return;
    CEntity* entities[32];
    int16 numPeds = 0, numVehicles = 0;
    CWorld::FindObjectsInRange(*startPoint, rayLength, true, &numPeds, 16, entities, false, false, true, false, false);
    if (canAimVehicles) {
        CWorld::FindObjectsInRange(*startPoint, rayLength, true, &numVehicles, 16, entities + numPeds, false, true, false, false, false);
    }
    const auto count = int16(numPeds + numVehicles);
    float bestScore = 10000.0f;
    int16 best = 0;
    for (int16 i = 0; i < count; ++i) {
        auto* entity = entities[i];
        if (entity == owner) continue;
        if (entity->GetIsTypePed()) {
            auto* ped = static_cast<CPed*>(entity);
            if (ped->m_nPedState == PEDSTATE_DEAD || ped->m_nPedState == PEDSTATE_DIE || ped->m_pAttachedTo == vehicle) continue;
        }
        float score = CCollision::DistToLine(*startPoint, *endPoint, entity->GetPosition());
        if (vehicle->m_nVehicleSubType == VEHICLE_TYPE_HELI || vehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE) {
            const auto distance = magnitude(entity->GetPosition() - vehicle->GetPosition());
            score /= distance < 5.0f ? 5.0f : distance;
        } else {
            score += magnitude(entity->GetPosition() - owner->GetPosition()) * 0.15f;
        }
        const auto delta = entity->GetPosition() - *startPoint;
        const auto forward = (ray.z * delta.z + ray.y * delta.y) + ray.x * delta.x;
        if (forward > 0.0f && score < bestScore) {
            bestScore = score;
            best = i;
        }
    }
    const auto aimAngle = vehicle->GetPlaneGunsAutoAimAngle();
    float threshold = 2.5f;
    if (aimAngle > 0.5f) {
        const float radians = aimAngle * std::bit_cast<float>(0x3C8EFA35u);
        __asm {
            fld radians
            fptan
            fstp st(0)
            fstp threshold
        }
    }
    if (bestScore < threshold) {
        const auto delta = entities[best]->GetPosition() - *startPoint;
        const auto scale = rayLength / magnitude(delta);
        *endPoint = *startPoint + delta * scale;
    }
}

// 0x73DB40
void CWeapon::Update(CPed* owner) {
    const auto wi = &GetWeaponInfo(owner);
    const auto ao = &wi->GetAimingOffset();

    const auto ProcessReloadAudioIf = [&](auto Pred) {
        const auto ProcessOne = [&](uint32 delay, eAudioEvents ae) {
            if (Pred(delay, ae)) {
                owner->GetWeaponAE().AddAudioEvent(ae);
            }
        };
        ProcessOne(owner->bIsDucking ? ao->CrouchRLoadA : ao->RLoadA, AE_WEAPON_RELOAD_A);
        ProcessOne(owner->bIsDucking ? ao->CrouchRLoadB : ao->RLoadB, AE_WEAPON_RELOAD_B);
    };

    switch (m_State) {
    case WEAPONSTATE_FIRING: {
        if (owner && notsa::contains({ WEAPON_SPAS12_SHOTGUN, WEAPON_SHOTGUN }, m_Type)) { // 0x73DBA5    
            ProcessReloadAudioIf([&](uint32 rload, eAudioEvents ae) {
                if (!rload) {
                    return false;
                }
                const auto nextShotEnd = m_TimeForNextShotMs + rload;
                return CTimer::GetPreviousTimeInMS() < nextShotEnd && CTimer::GetTimeInMS() >= nextShotEnd;
            });
        }
        if (CTimer::GetTimeInMS() > m_TimeForNextShotMs) {
            m_State = wi->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE || m_TotalAmmo != 0
                ? eWeaponState::WEAPONSTATE_READY
                : eWeaponState::WEAPONSTATE_OUT_OF_AMMO;
        }
        break;
    }
    case WEAPONSTATE_RELOADING: {
        if (owner && m_Type < WEAPON_LAST_WEAPON) {
            const auto DoPlayAnimlessReloadAudio = [&] {
                ProcessReloadAudioIf([
                    &,
                    shootDelta = m_TimeForNextShotMs - wi->GetWeaponReloadTime()
                ](uint32 rload, eAudioEvents ae) {
                    const auto audioTimeMs = rload + shootDelta;
                    return CTimer::GetPreviousTimeInMS() < audioTimeMs && CTimer::GetTimeInMS() >= audioTimeMs;
                });
            };
            if (wi->flags.bReload && (!owner->IsPlayer() || !FindPlayerInfo().m_bFastReload)) { // 0x73DCCE
                auto animRLoad = RpAnimBlendClumpGetAssociation(
                    owner->GetRpClump(),
                    ANIM_ID_RELOAD //(wi->m_Flags & 0x1000) != 0 ? ANIM_ID_RELOAD : ANIM_ID_WALK // Always going to be `ANIM_ID_RELOAD`
                );
                if (!animRLoad) {
                    animRLoad = RpAnimBlendClumpGetAssociation(owner->GetRpClump(), wi->GetCrouchReloadAnimationID());
                }
                if (animRLoad) { // 0x73DD30
                    ProcessReloadAudioIf([&](uint32 rloadMs, eAudioEvents ae) {
                        const auto rloadS = (float)rloadMs / 1000.f;
                        return rloadS <= animRLoad->m_CurrentTime && animRLoad->m_CurrentTime - animRLoad->m_TimeStep < rloadS;
                    });
                    if (CTimer::GetTimeInMS() > m_TimeForNextShotMs) {
                        if (animRLoad->GetTimeProgress() < 0.9f) {
                            m_TimeForNextShotMs = CTimer::GetTimeInMS();
                        }
                    }
                } else if (owner->GetIntelligence()->GetTaskUseGun()) { // 0x73DDF9
                    if (CTimer::GetTimeInMS() > m_TimeForNextShotMs) {
                        m_TimeForNextShotMs = CTimer::GetTimeInMS();
                    }
                } else { // 0x73DE16
                    DoPlayAnimlessReloadAudio();
                }
            } else {
                DoPlayAnimlessReloadAudio();
            }
        }
        //> 0x73DEA4
        if (CTimer::GetTimeInMS() > m_TimeForNextShotMs) {
            Reload(owner);
            m_State = WEAPONSTATE_READY;
        }
        StopWeaponEffect();
        break;
    }
    case WEAPONSTATE_MELEE_MADECONTACT: {
        m_State = WEAPONSTATE_READY;
        StopWeaponEffect();
        break;
    }
    default: {
        StopWeaponEffect();
        break;
    }
    }
}

// 0x73A360
void CWeapon::UpdateWeapons() {
    ZoneScoped;

    CShotInfo::Update();
    CExplosion::Update();
    CProjectileInfo::Update();
    CBulletInfo::Update();
}

// 0x73DEF0
bool CWeapon::CanBeUsedFor2Player() {
    return CanBeUsedFor2Player(m_Type);
}

// 0x73E240
CEntity* CWeapon::FindNearestTargetEntityWithScreenCoors(float screenX, float screenY, float range, CVector point, float* outScrX, float* outScrY) {
    const float width=float(RsGlobal.maximumWidth),height=float(RsGlobal.maximumHeight);
    float nearest=width*std::bit_cast<float>(0x3D888889u);
    screenX=(screenX+1.0f)*width*0.5f;
    screenY=(screenY+1.0f)*height*0.5f;
    CEntity* closest=nullptr;
    const auto process=[&](CEntity* entity){
        CVector screen;
        float sizeX,sizeY;
        if(!CSprite::CalcScreenCoors(entity->GetPosition(),&screen,&sizeX,&sizeY,true,true))return;
        const float x=screen.x-screenX,y=screen.y-screenY;
        const float distance=std::sqrt(x*x+y*y);
        if(!(distance<nearest))return;
        const auto& pos=entity->GetPosition();
        const float px=pos.x-point.x,py=pos.y-point.y,pz=pos.z-point.z;
        if(!(px*px+py*py+pz*pz<range*range))return;
        nearest=distance;closest=entity;
        if(outScrX){*outScrX=screen.x/(width*0.5f)-1.0f;*outScrY=screen.y/(height*0.5f)-1.0f;}
    };
    auto* peds=GetPedPool();
    for(int32 i=peds->GetSize()-1;i>=0;--i){
        auto* ped=peds->GetAt(size_t(i));
        if(ped&&ped->m_nPedState!=PEDSTATE_DEAD&&!ped->bInVehicle&&CDarkel::ThisPedShouldBeKilledForFrenzy(*ped))process(ped);
    }
    auto* vehicles=GetVehiclePool();
    for(int32 i=vehicles->GetSize()-1;i>=0;--i){
        auto* vehicle=vehicles->GetAt(size_t(i));
        if(vehicle&&vehicle!=FindPlayerVehicle(-1,false)&&CDarkel::ThisVehicleShouldBeKilledForFrenzy(*vehicle))process(vehicle);
    }
    return closest;
}

// notsa
auto CWeapon::GetProjectileType() {
    switch (GetType()) {
    case eWeaponType::WEAPON_RLAUNCHER:
        return eWeaponType::WEAPON_ROCKET;
    case eWeaponType::WEAPON_RLAUNCHER_HS:
        return eWeaponType::WEAPON_ROCKET_HS;
    case WEAPON_GRENADE:
    case WEAPON_TEARGAS:
    case WEAPON_MOLOTOV:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
        return GetType();
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x73E560
float CWeapon::EvaluateTargetForHeatSeekingMissile(CEntity* potentialTarget, const CVector& origin, const CVector& aimingDir, float tolerance, bool arePlanesPriority, CEntity* preferredExistingTarget) {
    const auto potentialTargetDist = (origin - potentialTarget->GetPosition()).Magnitude();

    const auto potentialTargetDistToLine = CCollision::DistToLine(origin, origin + aimingDir * 250.f, potentialTarget->GetPosition());
    auto ret = std::sqrt(potentialTargetDist) / 10.f + potentialTargetDistToLine / potentialTargetDist;

    if (potentialTargetDistToLine * tolerance >= potentialTargetDist) {
        return -1.f;
    }

    if (arePlanesPriority) {
        if (potentialTarget->GetIsTypeVehicle() && notsa::contains({ VEHICLE_TYPE_PLANE, VEHICLE_TYPE_HELI }, potentialTarget->AsVehicle()->m_nVehicleSubType)) {
            ret *= 0.25f;
        }
    }

    if (preferredExistingTarget && preferredExistingTarget == potentialTarget) {
        ret *= 0.25f;
    }

    return ret;
}

// 0x73E690
void CWeapon::DoWeaponEffect(CVector origin, CVector dir) {
    const char* fxName{};
    switch (m_Type) {
    case eWeaponType::WEAPON_FLAMETHROWER: fxName = "flamethrower"; break;
    case eWeaponType::WEAPON_EXTINGUISHER: fxName = "extinguisher"; break;
    case eWeaponType::WEAPON_SPRAYCAN:     fxName = "spraycan";     break;
    default:                               StopWeaponEffect();      return;
    }

    const auto mat = RwMatrixCreate();
    g_fx.CreateMatFromVec(mat, &origin, &dir);

    if (m_FxSystem) {
        m_FxSystem->SetMatrix(mat);
    } else {
        m_FxSystem = g_fxMan.CreateFxSystem(fxName, CVector{}, mat, false);

        if (!m_FxSystem) {
            RwMatrixDestroy(mat);
            return;
        }

        m_FxSystem->CopyParentMatrix();
        m_FxSystem->Play();
        m_FxSystem->SetMustCreatePrts(true);
    }
    m_FxSystem->SetConstTime(1, 1.0f);

    RwMatrixDestroy(mat);
}

// 0x73E800
bool CWeapon::FireAreaEffect(CEntity* firingEntity, const CVector& origin, CEntity* targetEntity, CVector* target) {
    const auto wi = &GetWeaponInfo(); // TODO/NOTE: Why not `GetWeaponInfo(firingEntity)`?
    const auto [shotDir, shotPt] = [&]() -> std::pair<CVector, CVector> {
        if (!targetEntity && !target) {
            if (firingEntity == FindPlayerPed() && TheCamera.m_aCams[0].Using3rdPersonMouseCam()) {
                CVector camPos, camTargetPos;
                TheCamera.Find3rdPersonCamTargetVector(wi->m_fWeaponRange, origin, camPos, camTargetPos);
                return {
                    (camTargetPos - camPos) / wi->m_fWeaponRange, // Scale to a unit vector
                    camTargetPos
                }; 
            } else {
                // NOTE: Moved here from `0x73E83F`
                // NOTE: Original code used degs instead of radians
                //       and then converted back to radians... So we're gonna stick to radians only ;)
                const auto heading = [&] { // 0x73E83F
                    if (targetEntity) {
                        return (targetEntity->GetPosition() - origin).Heading();
                    }
                    if (target) {
                        return (*target - origin).Heading();
                    }
                    return firingEntity->GetHeading();
                }();
                CVector dir{
                    -std::sin(heading),
                    std::cos(heading),
                    0.f
                };
                if (firingEntity->GetIsTypePed()) {
                    if (const auto pd = firingEntity->AsPed()->GetPlayerData()) {
                        dir.z = -std::tan(pd->m_fLookPitch);
                    }
                }
                return { dir, origin + dir };
            }
        } else {
            const auto ptTarget = target
                ? *target
                : targetEntity->GetIsTypePed()
                    ? targetEntity->AsPed()->GetBonePosition(BONE_SPINE1)
                    : targetEntity->GetPosition();
            return { (ptTarget - origin).Normalized(), ptTarget };
        }
    }();
    CShotInfo::AddShot(firingEntity, m_Type, origin, shotPt);
    DoWeaponEffect(origin, shotDir);
    if (m_Type == WEAPON_FLAMETHROWER && CGeneral::RandomBool(1.f / 3.f * 100.f)) {
        if (CCreepingFire::TryToStartFireAtCoors(
            shotDir * CVector::Random(3.5f, 6.f) + origin + CVector{0.f, 0.f, 0.5f},
            0,
            true,
            false,
            2.3f)
        ) {
            CStats::IncrementStat(STAT_FIRES_STARTED);
        }
    }
    CCrime::ReportCrime(CRIME_FIRE_WEAPON, nullptr, firingEntity->AsPed());
    return true;
}

// 0x73EC40
bool CWeapon::FireInstantHitFromCar(CVehicle* vehicle, bool leftSide, bool rightSide) {
    auto* weaponInfo = CWeaponInfo::GetWeaponInfo(m_Type, static_cast<eWeaponSkill>(1));
    auto* model = CModelInfo::GetModelInfo(static_cast<int16>(vehicle->m_nModelIndex));
    const auto constant = [](uintptr_t address) { return StaticRef<float>(address); };
    const bool bike = *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(vehicle) + 0x590) == 9;
    CVector start, target;
    if (!bike || vehicle->m_pDriver) {
        start = weaponInfo->m_vecFireOffset;
        if (!bike && rightSide) { start *= constant(0x859BB4); start.z -= constant(0x858B1C); }
        auto* driver = vehicle->m_pDriver;
        auto* hierarchy = GetAnimHierarchyFromSkinClump(driver->GetRpClump());
        auto* hand = *reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(driver) + 0x4A0);
        const int32 bone = *reinterpret_cast<int32*>(hand + 0x14);
        const int32 index = RpHAnimIDGetIndex(hierarchy, bone);
        auto* matrices = RpHAnimHierarchyGetMatrixArray(hierarchy);
        RwV3dTransformPoints(&start, &start, 1, &matrices[index]);
        start.x = CTimer::GetTimeStep() * vehicle->m_vecMoveSpeed.x + start.x;
        start.y = CTimer::GetTimeStep() * vehicle->m_vecMoveSpeed.y + start.y;
        start.z = CTimer::GetTimeStep() * vehicle->m_vecMoveSpeed.z + start.z;
        const float range = weaponInfo->m_fWeaponRange;
        const auto& matrix = vehicle->GetMatrix();
        if (leftSide) target = start - matrix.GetRight() * range;
        else target = matrix.GetRight() * range + start;
        if (!leftSide && !rightSide) target = matrix.GetForward() * range + start;
    } else {
        const auto seat = [model] {
            auto* vehicleData = *reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(model) + 0x5C);
            const int32 kind = *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(model) + 0x3C);
            return *reinterpret_cast<CVector*>(vehicleData + (kind == 5 ? 0 : 0x30));
        };
        CVector offset, targetOffset;
        if (!leftSide && !rightSide) {
            const auto seatPosition = seat();
            auto* collision = vehicle->GetColModel();
            const uint32 random = uint32(std::rand());
            offset.x = float(random & 0xFF) * constant(0x858CDC) - constant(0x858EE8);
            offset.y = collision->m_boundBox.m_vecMax.y + seatPosition.y + constant(0x858CC4);
            offset.z = seatPosition.z + constant(0x866CAC);
            targetOffset = CVector{0.f, weaponInfo->m_fWeaponRange, seat().z + constant(0x858B8C)};
        } else if (!leftSide) {
            const auto seatPosition = seat();
            const uint32 random = uint32(std::rand()); auto* collision = vehicle->GetColModel();
            offset.x = collision->m_boundBox.m_vecMax.x + constant(0x858C84);
            offset.y = (seatPosition.y - constant(0x872C68)) + float(random & 0xFF) * constant(0x858CDC);
            offset.z = seatPosition.z + constant(0x8665BC);
            const auto again = seat(); targetOffset = CVector{weaponInfo->m_fWeaponRange, again.y, again.z + constant(0x858B8C)};
        } else {
            const auto seatPosition = seat();
            const uint32 random = uint32(std::rand()); auto* collision = vehicle->GetColModel();
            offset.x = -collision->m_boundBox.m_vecMax.x - constant(0x858C84);
            offset.y = (seatPosition.y - constant(0x858C28)) + float(random & 0xFF) * constant(0x858CDC);
            offset.z = seatPosition.z + constant(0x872C6C);
            const auto again = seat(); targetOffset = CVector{-weaponInfo->m_fWeaponRange, again.y, again.z + constant(0x858CC8)};
        }
        start = vehicle->GetMatrix().TransformPoint(offset);
        start.x = CTimer::GetTimeStep() * vehicle->m_vecMoveSpeed.x + start.x;
        start.y = CTimer::GetTimeStep() * vehicle->m_vecMoveSpeed.y + start.y;
        start.z = CTimer::GetTimeStep() * vehicle->m_vecMoveSpeed.z + start.z;
        target = vehicle->GetMatrix().TransformPoint(targetOffset);
    }
    const uint32 randomY = uint32(std::rand()), randomX = uint32(std::rand());
    target.x = (float(randomX & 0xFF) * constant(0x858C58) - constant(0x872C64)) + target.x;
    target.y = target.y + (float(randomY & 0xFF) * constant(0x858C58) - constant(0x872C64));
    const uint32 randomZ = uint32(std::rand());
    target.z += float(randomZ & 0xFF) * constant(0x858C58) - constant(0x872C64);
    DoDriveByAutoAiming(FindPlayerPed(-1), vehicle, &start, &target, false);
    FireInstantHitFromCar2(start, target, vehicle, vehicle->m_pDriver);
    return true;
}

// 0x73F480
void CWeapon::CheckForShootingVehicleOccupant(CEntity** pCarEntity, CColPoint* colPoint, eWeaponType, const CVector& origin, const CVector& target) {
    auto* entity=*pCarEntity;
    if(entity->GetType()!=ENTITY_TYPE_VEHICLE)return;
    auto* vehicle=static_cast<CVehicle*>(entity);
    CColPoint saved;
    const auto copyPoint=[](CColPoint& out,const CColPoint& in){out.m_vecPoint=in.m_vecPoint;out.m_vecNormal=in.m_vecNormal;out.m_nSurfaceTypeA=in.m_nSurfaceTypeA;out.m_nPieceTypeA=in.m_nPieceTypeA;out.m_nLightingA=in.m_nLightingA;out.m_nSurfaceTypeB=in.m_nSurfaceTypeB;out.m_nPieceTypeB=in.m_nPieceTypeB;out.m_nLightingB=in.m_nLightingB;out.m_fDepth=in.m_fDepth;};
    copyPoint(saved,*colPoint);
    CColLine line{origin,target};float depth=1.0f;bool found=false;
    const auto testPed=[&](CPed* ped){
        if(!ped||!(*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(ped)+0x470)&0x4000000u))return;
        CVector center{0,0,0};
        auto* hierarchy=GetAnimHierarchyFromSkinClump(ped->GetRpClump());
        const auto boneId=*reinterpret_cast<const int32*>(*reinterpret_cast<const uint8* const*>(reinterpret_cast<const uint8*>(ped)+0x490)+0x14);
        const int32 index=RpHAnimIDGetIndex(hierarchy,boneId);
        auto* matrices=RpHAnimHierarchyGetMatrixArray(hierarchy);
        RwV3dTransformPoints(&center,&center,1,&matrices[index]);
        center.z+=.1f;CColSphere sphere;sphere.Set(.2f,center,static_cast<eSurfaceType>(0),9,tColLighting{0xFF});
        if(CCollision::ProcessLineSphere(line,sphere,*colPoint,depth)){*pCarEntity=ped;found=true;}
    };
    testPed(vehicle->m_pDriver);
    for(int32 i=0;i<8;++i)testPed(vehicle->m_apPassengers[i]);
    if(vehicle->m_nVehicleType==VEHICLE_TYPE_AUTOMOBILE){
        const auto& matrix=vehicle->GetMatrix();const auto ray=target-origin;
        const float along=(ray.x*matrix.GetForward().x+ray.z*matrix.GetForward().z)+ray.y*matrix.GetForward().y;
        const float vertical=(ray.x*matrix.GetUp().x+ray.z*matrix.GetUp().z)+ray.y*matrix.GetUp().y;
        if(along<0&&(vertical<=0||(*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(vehicle)+0x429)&4))){
            auto* model=vehicle->GetColModel();auto* data=model->m_pColData;
            if(int16(data->m_nNumTriangles)>0){
                const auto inverse=Invert(matrix);
                CColLine localLine=line;localLine.m_vecStart=inverse.TransformPoint(line.m_vecStart);localLine.m_vecEnd=inverse.TransformPoint(line.m_vecEnd);
                CCollision::CalculateTrianglePlanes(model);
                for(int32 i=0;i<int16(data->m_nNumTriangles);++i){
                    if(g_surfaceInfos.IsGlass(data->m_pTriangles[i].m_nMaterial)&&CCollision::TestLineTriangle(localLine,data->m_pVertices,data->m_pTriangles[i],data->m_pTrianglePlanes[i])){
                        auto* automobile=static_cast<CAutomobile*>(vehicle);
                        if(automobile->m_damageManager.ProgressPanelDamage(WINDSCREEN_PANEL)){
                            if(automobile->m_damageManager.GetPanelStatus(WINDSCREEN_PANEL)==static_cast<ePanelDamageState>(2))automobile->m_damageManager.ProgressPanelDamage(WINDSCREEN_PANEL);
                            automobile->SetPanelDamage(WINDSCREEN_PANEL,true);
                        }
                        break;
                    }
                }
            }
        }
    }
    if(!found){*pCarEntity=entity;copyPoint(*colPoint,saved);}
}

// 0x73F910
CEntity* CWeapon::PickTargetForHeatSeekingMissile(CVector origin, CVector direction, float distanceMultiplier, CEntity* ignoreEntity, bool arePlanesPriority, CEntity* preferredExistingTarget) {
    float minRating  = FLT_MAX;
    CEntity* minRated{};
    const auto point = origin + direction * 5.f;
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        if (&veh == ignoreEntity) {
            continue;
        }
        if (!veh.vehicleFlags.bVehicleCanBeTargettedByHS) {
            continue;
        }
        if (veh.m_fHealth <= 0.f) {
            continue;
        }
        const auto rating = EvaluateTargetForHeatSeekingMissile(&veh, point, direction, distanceMultiplier, arePlanesPriority, preferredExistingTarget);
        if (rating >= 0.f && rating <= minRating) {
            minRating = rating;
            minRated  = &veh;
        }
    }
    return minRated;
}

// 0x73FA20
bool CWeapon::FireFromCar(CVehicle* vehicle, bool leftSide, bool rightSide) {
    if (m_State != WEAPONSTATE_READY || m_AmmoInClip <= 0) {
        return false;
    }
    if (!CWeapon::FireInstantHitFromCar(vehicle, leftSide, rightSide)) {
        return notsa::IsFixBugs() ? false : true;
    }
    if (const auto d = vehicle->m_pDriver) {
        d->GetWeaponAE().AddAudioEvent(AE_WEAPON_FIRE);
    }
    if (!CCheat::IsActive(CHEAT_INFINITE_AMMO)) {
        if (m_AmmoInClip) { // NOTE: I'm pretty sure this is redundant
            m_AmmoInClip--;
        }
        if (  m_TotalAmmo < 25'000 && m_TotalAmmo > 0
            && (vehicle->GetStatus() != STATUS_PLAYER || CStats::GetPercentageProgress() < 100.f)
        ) {
            m_TotalAmmo--;
        }
    }
    m_State = WEAPONSTATE_FIRING;
    if (m_AmmoInClip) {
        m_TimeForNextShotMs = CTimer::GetTimeInMS() + 1000; // NOTE: Shoot delay can be adjusted here
    } else if (m_TotalAmmo) {
        m_State = WEAPONSTATE_RELOADING;
        m_TimeForNextShotMs = CTimer::GetTimeInMS() + GetWeaponInfo().GetWeaponReloadTime();
    }
    return true;
}

// Exact-address data is retained because REA's propagated float constants differ from the EXE.
namespace {
template<class T> T& InstantHitField(void* p, size_t offset) { return *reinterpret_cast<T*>(static_cast<uint8*>(p) + offset); }
float InstantHitConstant(uintptr_t address) { return *reinterpret_cast<const float*>(address); }
int32 InstantHitAnimationDuration(CWeaponInfo* wi) {
    const float animationStart = wi->m_fAnimLoopStart, animationEnd = wi->m_fAnimLoopEnd;
    const float scale = InstantHitConstant(0x858978);
    int32 result;
    __asm {
        fld animationEnd
        fsub animationStart
        fmul scale
        mov eax, 0x821B40
        call eax
        mov result, eax
    }
    return result;
}
static void InstantHitSwayAccumulate355(CVector* endpoint,const CVector* sineBasis,const CVector* cosineBasis,float amplitude,float sine,float cosine) {
    float productX,productY;
    __asm {
        mov ecx,sineBasis
        mov edx,endpoint
        fld dword ptr [ecx+8]
        fmul amplitude
        fld dword ptr [ecx+4]
        fmul amplitude
        fstp productY
        fld dword ptr [ecx]
        fmul amplitude
        fstp productX
        fmul sine
        fld productY
        fmul sine
        fld productX
        fmul sine
        fadd dword ptr [edx]
        fstp dword ptr [edx]
        fld dword ptr [edx+4]
        fadd st,st(1)
        fstp dword ptr [edx+4]
        fstp st(0)
        fadd dword ptr [edx+8]
        mov ecx,cosineBasis
        fld dword ptr [ecx+8]
        fmul amplitude
        fld dword ptr [ecx+4]
        fmul amplitude
        fstp productY
        fld dword ptr [ecx]
        fmul amplitude
        fstp productX
        fmul cosine
        fld productY
        fmul cosine
        fld productX
        fmul cosine
        fadd dword ptr [edx]
        fstp dword ptr [edx]
        fld dword ptr [edx+4]
        fadd st,st(1)
        fstp dword ptr [edx+4]
        fstp st(0)
        fadd st,st(1)
        fstp dword ptr [edx+8]
        fstp st(0)
    }
}

void InstantHitOscillation(uint32 milliseconds, float& sine, float& cosine) {
    float sineResult,cosineResult;
    const double time = double(milliseconds);
    const float frequency = InstantHitConstant(0x8D6118);
    __asm {
        fld time
        fmul frequency
        fld st(0)
        fsin
        fstp sineResult
        fcos
        fstp cosineResult
    }
    sine=sineResult;cosine=cosineResult;
}
}

// 0x73FB10
bool CWeapon::FireInstantHit(CEntity* firingEntity, CVector* origin, CVector* muzzlePosn, CEntity* targetEntity, CVector* target, CVector* originForDriveBy, bool arg6, bool muzzle) {
    auto* ped = firingEntity && firingEntity->GetIsTypePed() ? firingEntity->AsPed() : nullptr;
    auto* wi = CWeaponInfo::GetWeaponInfo(m_Type, ped ? ped->GetWeaponSkill(m_Type) : eWeaponSkill::STD);
    const CVector muzzlePosition = *muzzlePosn;
    CVector start = originForDriveBy ? *originForDriveBy : *origin;
    CVector end{0.f, 0.f, 0.f}, direction;
    CColPoint hit;
    CEntity* victim = nullptr;
    float spread = 0.f;
    CTaskSimpleUseGun* useGun = nullptr;
    if (ped) {
        spread = (100.f - float(InstantHitField<uint8>(ped, 0x71A))) / wi->m_fAccuracy;
        if (InstantHitField<void*>(ped, 0x480) && (InstantHitField<uint32>(ped, 0x46C) & 0x4000000)) spread *= .5f;
        useGun = ped->GetIntelligence()->GetTaskUseGun();
    }
    if (m_Type == WEAPON_SHOTGUN || m_Type == WEAPON_SAWNOFF_SHOTGUN || m_Type == WEAPON_SPAS12_SHOTGUN) {
        spread = 0.f;
        CWorld::fWeaponSpreadRate = InstantHitConstant(0x8D611C) / wi->m_fAccuracy;
    }
    const auto ignorePedVehicle = [&](bool ignoreFireproof) {
        CEntity* entity = nullptr;
        if ((InstantHitField<uint32>(ped, 0x46C) & 0x100) != 0) {
            entity = InstantHitField<CEntity*>(ped, 0x58C);
            if (entity && ignoreFireproof && (InstantHitField<uint8>(entity, 0x42D) & 4)) entity = nullptr;
        }
        if (!entity) {
            auto* attached = InstantHitField<CEntity*>(ped, 0xFC);
            if (attached && attached->GetIsTypeVehicle()) {
                entity = attached;
                if (ignoreFireproof && (InstantHitField<uint8>(entity, 0x42D) & 4)) entity = nullptr;
            }
        }
        return entity;
    };
    const auto cast = [&](const CVector& from) {
        CBirds::HandleGunShot(&from, &end);
        CShadows::GunShotSetsOilOnFire(from, end);
        CWorld::ProcessLineOfSight(from, end, hit, victim, true, true, true, true, true, false, false, true);
    };
    const auto randomSpread = [&] {
        const float first = float(std::rand()) * InstantHitConstant(0x858C7C);
        const float second = float(std::rand()) * InstantHitConstant(0x858C7C);
        const float third = float(std::rand()) * InstantHitConstant(0x858C7C);
        const float zOffset = (first * InstantHitConstant(0x858CC4) - InstantHitConstant(0x858B1C)) * spread;
        end.x += (third * InstantHitConstant(0x858EE8) - InstantHitConstant(0x858CC4)) * spread;
        end.y += (second * InstantHitConstant(0x858EE8) - InstantHitConstant(0x858CC4)) * spread;
        end.z += zOffset;
    };
    const auto sway = [&](bool cameraRay) {
        float factor = InstantHitConstant(0x8D6114) / wi->m_fWeaponRange;
        if (cameraRay) factor *= 3.f;
        if (factor > 1.f) factor = 1.f;
        auto* playerData = InstantHitField<void*>(ped, 0x480);
        spread = factor * spread * InstantHitField<float>(playerData, 0x2C) * InstantHitConstant(0x8D6110);
        CVector first, second;
        const auto cameraMode = StaticRef<uint16>(0xB6F1A8);
        if (cameraMode == 53 || cameraMode == 55 || (cameraRay && cameraMode == 49)) {
            if (cameraRay && cameraMode == 49) { CVector temporary; TheCamera.m_aCams[0].Get_TwoPlayer_AimVector(temporary); }
            const CVector up = StaticRef<CVector>(0xB6F350);
            if (cameraRay) { second = up; first = CrossProduct(StaticRef<CVector>(0xB6F32C), second); first.Normalise(); }
            else { first = up; second = CrossProduct(StaticRef<CVector>(0xB6F32C), first); second.Normalise(); }
        } else {
            const CVector vertical{0.f, 0.f, 1.f};
            if (cameraRay) { first = CrossProduct(end, vertical); first.Normalise(); second = CrossProduct(first, end); second.Normalise(); }
            else { second = CrossProduct(end, vertical); second.Normalise(); first = CrossProduct(second, end); first.Normalise(); }
        }
        float sine, cosine;
        float amplitude = spread;
        if (!cameraRay) {
            const float limit = std::min(spread * .5f, .2f);
            amplitude = CGeneral::GetRandomNumberInRange(limit, spread);
        }
        InstantHitOscillation(CTimer::GetTimeInMS(), sine, cosine);
        InstantHitSwayAccumulate355(&end, cameraRay ? &first : &second, cameraRay ? &second : &first, amplitude, sine, cosine);
        InstantHitField<float>(playerData, 0x2C) += float(wi->m_nDamage) * .04f;
    };
    if (useGun && InstantHitField<uint8>(useGun, 0xE)) {
        end = CVector{wi->m_fWeaponRange, 0.f, 0.f};
        auto* hierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
        auto* frame = InstantHitField<void*>(ped, 0x4A0);
        const int32 bone = InstantHitField<int32>(frame, 0x14);
        const int32 index = RpHAnimIDGetIndex(hierarchy, bone);
        auto* matrix = RpHAnimHierarchyGetMatrixArray(hierarchy) + index;
        RwV3dTransformPoints(&end, &end, 1, matrix);
        direction = end - start; direction.Normalise();
        if (auto* ignored = ignorePedVehicle(false)) CWorld::pIgnoreEntity = ignored;
        CWorld::bIncludeBikers = true;
        cast(start);
    } else {
        const auto activeMode = TheCamera.GetActiveCam().m_nMode;
        const bool playerRay = ped && !targetEntity && !target && ped->IsPlayer()
            && (int(activeMode) == 53 || int(activeMode) == 55 || int(activeMode) == 65 || int(activeMode) == 49);
        if (playerRay) {
            CVector cameraStart;
            TheCamera.Find3rdPersonCamTargetVector(wi->m_fWeaponRange * 3.f, start, cameraStart, end);
            direction = end - start; direction.Normalise();
            if (spread != 0.f) sway(true);
            CWorld::pIgnoreEntity = ignorePedVehicle(true);
            if (!CWorld::pIgnoreEntity) CWorld::pIgnoreEntity = firingEntity;
            CWorld::bIncludeDeadPeds = true;
            CWorld::bIncludeCarTyres = true;
            CWorld::bIncludeBikers = true;
            cast(cameraStart);
            if (victim) {
                const float dx = hit.m_vecPoint.x - cameraStart.x, dy = hit.m_vecPoint.y - cameraStart.y;
                if (std::sqrt(dx * dx + dy * dy) <= TargetWeaponRangeMultiplier(victim, ped) * wi->m_fWeaponRange) {
                    CheckForShootingVehicleOccupant(&victim, &hit, m_Type, cameraStart, end);
                } else victim = nullptr;
            }
        } else if (ped && (targetEntity || target) && ((InstantHitField<uint8>(ped, 0x528) & 1) || arg6)) {
            if (target) end = *target;
            else if (!targetEntity->GetIsTypePed()) end = targetEntity->GetPosition();
            else if (auto* playerData = InstantHitField<void*>(ped, 0x480)) {
                end = InstantHitField<CVector>(playerData, 0x74);
                targetEntity->AsPed()->GetTransformedBonePosition(end, static_cast<eBoneTag>(InstantHitField<uint32>(playerData, 0x70)), false);
            } else targetEntity->AsPed()->GetBonePosition(&end, static_cast<eBoneTag>(3), false);
            direction = end - start;
            const float length = std::max(.01f, std::sqrt(direction.z * direction.z + direction.y * direction.y + direction.x * direction.x));
            direction *= 1.f / length;
            end = start + direction * (TargetWeaponRangeMultiplier(targetEntity, ped) * wi->m_fWeaponRange);
            if (InstantHitField<void*>(ped, 0x480) && spread != 0.f) sway(false);
            else if (spread > 0.f) {
                if (targetEntity && targetEntity->GetIsTypePed() && targetEntity->AsPed()->IsPlayer()) {
                    float speed = targetEntity->AsPhysical()->m_vecMoveSpeed.Magnitude();
                    if (speed <= .33f) speed = targetEntity->AsPhysical()->m_vecMoveSpeed.Magnitude(); else speed = .33f;
                    spread = (speed * .90909094f + .8f) * spread;
                }
                randomSpread();
            }
            if (auto* ignored = ignorePedVehicle(true)) CWorld::pIgnoreEntity = ignored;
            if (ped->IsPlayer()) CWorld::bIncludeDeadPeds = true;
            CWorld::bIncludeBikers = true;
            cast(start);
        } else if (!ped || (!targetEntity && !target)) {
            if (!firingEntity->GetIsTypeVehicle()) {
                end.x = firingEntity->GetForward().x * wi->m_fWeaponRange + muzzlePosition.x;
                end.y = muzzlePosition.y + wi->m_fWeaponRange * firingEntity->GetForward().y;
                end.z = firingEntity->GetForward().z * wi->m_fWeaponRange + muzzlePosition.z;
                direction = firingEntity->GetForward();
                if (ped) {
                    if ((InstantHitField<uint32>(ped, 0x470) & 0x2000000) && (!ped->IsPlayer() || !wi->flags.bCanAim)) DoDoomAiming(firingEntity, &start, &end);
                    CWorld::pIgnoreEntity = ignorePedVehicle(false);
                }
                if (!CWorld::pIgnoreEntity) CWorld::pIgnoreEntity = firingEntity;
            } else {
                auto* vehicle = firingEntity->AsVehicle();
                spread = .6f; direction = vehicle->GetForward(); end = direction;
                if (vehicle->GetStatus() == STATUS_PLAYER || int(vehicle->GetStatus()) == 8) {
                    end = start + direction * wi->m_fWeaponRange;
                    const int32 appearance = InstantHitField<int32>(vehicle, 0x594);
                    CEntity* owner = int(vehicle->GetStatus()) == 8 ? static_cast<CEntity*>(FindPlayerPed()) : static_cast<CEntity*>(vehicle->m_pDriver);
                    DoDriveByAutoAiming(owner, vehicle, &start, &end, appearance == 3 || appearance == 4);
                    end -= start; end.Normalise();
                    const int16 model = vehicle->m_nModelIndex;
                    spread = model == 447 || model == 469 || model == 564 ? .1f : .3f;
                }
                randomSpread(); end.Normalise(); end = start + end * wi->m_fWeaponRange;
                CWorld::pIgnoreEntity = vehicle;
            }
            if (!firingEntity->GetIsTypeVehicle()) CWorld::bIncludeBikers = true;
            cast(start);
        }
    }
    CEventGunShot shot{firingEntity, start, end, m_Type == WEAPON_PISTOL_SILENCED || m_Type == WEAPON_TEARGAS};
    GetEventGlobalGroup()->Add(&shot, false);
    CEventGunShotWhizzedBy whizz{firingEntity, start, end, m_Type == WEAPON_PISTOL_SILENCED};
    GetEventGlobalGroup()->Add(&whizz, false);
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_22, firingEntity);
    if (ped) ped->IsPlayer();
    start = *origin;
    if (muzzle) {
        float flashLength = 0.f, shellSize = 0.f;
        switch (int(m_Type)) {
        case 22: case 23: case 24: case 34: flashLength = .2f; shellSize = .25f; break;
        case 25: case 26: case 27: flashLength = .3f; shellSize = .45f; break;
        case 28: case 29: case 32: flashLength = .2f; shellSize = .3f; break;
        case 30: case 31: case 38:
            if (InstantHitAnimationDuration(wi) <= 49 && (++StaticRef<uint8>(0xC8A80C) & 1)) break;
            flashLength = .65f; shellSize = .25f; break;
        }
        if (flashLength != 0.f) {
            CPointLights::AddLight(PLTYPE_POINTLIGHT, muzzlePosition, CVector{0.f,0.f,0.f}, 3.f, .25f, .22f, 0.f, 0, false, nullptr);
            g_fx.TriggerGunshot(firingEntity, muzzlePosition, direction, !(ped && InstantHitField<void*>(ped,0x4F8)));
            CVector shellPosition = muzzlePosition - direction * flashLength;
            const auto& right = firingEntity->GetRight();
            const CVector2D shellDirection{right.x, right.y};
            AddGunshell(firingEntity, shellPosition, shellDirection, shellSize);
        }
    }
    const bool canSplash = targetEntity
        ? (targetEntity->GetIsTypeVehicle() || targetEntity->GetIsTypePed() || targetEntity->GetIsTypeObject()) && (InstantHitField<uint32>(targetEntity,0x40) & 0x100)
        : ((ped && ped->IsPlayer()) || (firingEntity->GetIsTypeVehicle() && (int(firingEntity->GetStatus())==0 || int(firingEntity->GetStatus())==8))) && end.z < start.z;
    if (canSplash) {
        CVector splash;
        const bool water = CWaterLevel::TestLineAgainstWater(start, victim ? hit.m_vecPoint : end, &splash);
        if (water) {
            g_fx.TriggerBulletSplash(splash);
            AudioEngine.ReportBulletHit(nullptr, static_cast<eSurfaceType>(39), splash, 0.f);
        }
    }
    start = muzzlePosition;
    const auto excludedTyre = [&] {return victim && victim->GetIsTypeVehicle() && hit.m_nPieceTypeB >= 13 && hit.m_nPieceTypeB <=16;};
    if (!(CWorld::fWeaponSpreadRate > 0.f) || !victim || excludedTyre()) {
        DoBulletImpact(firingEntity, victim, start, end, hit, false);
    } else {
        CVector pelletOrigin=*origin;
        int32 attempts = 0;
        do {
            ++attempts;
            const int32 count = m_Type == WEAPON_SPAS12_SHOTGUN ? StaticRef<int32>(0x8D6124) : StaticRef<int32>(0x8D6120);
            CMatrix pelletMatrix;
            SetUpPelletCol(count, firingEntity, victim, pelletOrigin, hit, pelletMatrix);
            std::array<float,15> distances; distances.fill(1.f);
            CColModel* model;
            if (victim->GetIsTypePed()) {
                auto* pedModel = reinterpret_cast<CPedModelInfo*>(CModelInfo::GetModelInfo(victim->m_nModelIndex));
                model = pedModel->AnimatePedColModelSkinned(victim->GetRpClump());
            } else model = victim->GetColModel();
            auto& points = StaticRef<std::array<CColPoint,32>>(0xB9ACD0);
            CCollision::ProcessColModels(pelletMatrix, ms_PelletTestCol, victim->GetMatrix(), *model, points, points.data(), distances.data(), false);
            int32 touched = 0,lastTouched = 0;
            for (int32 index = 0; index < count; ++index) if (distances[size_t(index)] < 1.f) {++touched;lastTouched=index;}
            for (int32 index = 0; index < count; ++index) if (distances[size_t(index)] < 1.f) {
                const auto& pelletPoint=points[size_t(index)];
                DoBulletImpact(firingEntity,victim,start,pelletPoint.m_vecPoint,pelletPoint,index==lastTouched?-touched:1);
            }
            if (victim->GetIsTypePed() || victim->GetIsTypeVehicle()) {
                if ((distances[0] != 1.f && float(touched) / float(count) >= .5f) || attempts > 1) victim = nullptr;
                else {
                    CWorld::pIgnoreEntity = victim;
                    start = hit.m_vecPoint; victim = nullptr;
                    cast(start);
                }
            } else {
                DoBulletImpact(firingEntity, victim, start, end, hit, false);
                victim = nullptr;
            }
        } while (victim);
    }
    CWorld::ResetLineTestOptions();
    return true;
}

// 0x741360
bool CWeapon::FireProjectile(CEntity* firedBy, const CVector& origin, CEntity* targetEntity, const CVector* targetPos, float force) {
    assert(firedBy);

    const auto firedByPed = firedBy->GetIsTypePed()
        ? firedBy->AsPed()
        : nullptr;
    auto projOrigin     = origin;
    auto losCheckTarget = origin;
    auto losCheckOrigin = origin;
    auto projType = GetProjectileType();
    if (notsa::contains({ WEAPON_RLAUNCHER, WEAPON_RLAUNCHER_HS }, GetType())) {
        if (firedByPed && firedByPed->IsPlayer()) {
            switch (TheCamera.GetActiveCam().m_nMode) {
            case MODE_M16_1STPERSON:
            case MODE_SNIPER:
            case MODE_ROCKETLAUNCHER:
            case MODE_ROCKETLAUNCHER_HS:
            case MODE_M16_1STPERSON_RUNABOUT:
            case MODE_SNIPER_RUNABOUT:
            case MODE_ROCKETLAUNCHER_RUNABOUT:
            case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
                break;
            default:
                return false;
            }
            projOrigin = origin + TheCamera.GetActiveCam().m_vecFront;
        } else {
            projOrigin = origin + firedBy->GetForward();
        }
        if (firedByPed) {
            if (firedByPed->IsPlayer()) { // 0x7416DC
                CEntity* hsMissleTarget{};
                if (GetType() == WEAPON_RLAUNCHER_HS && CWeaponEffects::IsLockedOn(WEAPONEFFECTS_LOCK_ON)) {
                    const auto pd = firedByPed->GetPlayerData();
                    if (pd->m_nFireHSMissilePressedTime) {
                        hsMissleTarget = PickTargetForHeatSeekingMissile(
                            firedBy->GetPosition(),
                            firedBy->GetForward(),
                            1.2f,
                            firedBy,
                            false,
                            pd->m_LastHSMissileTarget
                        );
                        if (hsMissleTarget == pd->m_LastHSMissileTarget && CTimer::GetTimeInMS() - pd->m_nFireHSMissilePressedTime > 1500) { // 0x74178B
                            const auto ch = &gCrossHair[0];
                            ch->m_color                 = { 255, 0, 0, 255 };
                            ch->m_fRotation             = 1.f;
                            ch->m_nTimeWhenToDeactivate = 0;
                        }
                    }
                }
                if (hsMissleTarget) { // 0x7417BB
                    targetEntity = hsMissleTarget;
                } else {
                    targetEntity = nullptr;
                    projType     = WEAPON_ROCKET;
                }
            } else { // 0x7418A3
                if (targetEntity || targetPos) {
                    CWorld::pIgnoreEntity = firedBy;
                    const auto losClear = CWorld::GetIsLineOfSightClear(
                        projOrigin,
                        projOrigin + ((targetEntity ? targetEntity->GetPosition() : *targetPos) - projOrigin).Normalized() * 8.f,
                        true,
                        false,
                        false,
                        false
                    );
                    CWorld::pIgnoreEntity = nullptr;
                    if (!losClear) {
                        return false;
                    }
                }
            }
        }
    } else { // 0x74139B
        if (const auto t = (origin - firedBy->GetPosition()).Dot(firedBy->GetForward()); t < 0.3f) { // 0x7413FC
            projOrigin += (0.3f - t) * firedBy->GetForward();
        }
        losCheckTarget = projOrigin;
        if (projOrigin.z - firedBy->GetPosition().z > 0.f) {
            losCheckTarget += firedBy->GetForward() * 0.6f;
        }
        losCheckOrigin = projOrigin - (projOrigin - firedBy->GetPosition()).ProjectOnToNormal(firedBy->GetForward()); // 0x7415A2
    }

    // 0x7418F5
    CWorld::pIgnoreEntity = firedBy;
    if (CWorld::GetIsLineOfSightClear(
        losCheckOrigin,
        losCheckTarget,
        true,
        true,
        false,
        true
    )) {
        if (projType == WEAPON_ROCKET && targetEntity && targetPos) {
            const auto projTargetPos = targetEntity
                ? targetEntity->GetPosition()
                : *targetPos;
            const auto projDir = (projTargetPos - losCheckOrigin).Normalized();
            CProjectileInfo::AddProjectile( // 0x741AF9
                firedBy,
                WEAPON_ROCKET,
                projOrigin,
                force,
                &projDir,
                targetEntity
            );
        } else {
            CProjectileInfo::AddProjectile(
                firedBy,
                projType,
                projOrigin,
                force,
                nullptr,
                targetEntity
            );
        }

    } else if (notsa::contains({ WEAPON_GRENADE, WEAPON_REMOTE_SATCHEL_CHARGE }, GetType()) && firedBy->GetIsTypePed()) { // 0x74193B
        const auto thorwableProjOrigin = firedBy->GetPosition() - firedBy->GetForward() - CVector{0.f, 0.f, 0.4f};
        if (CWorld::TestSphereAgainstWorld(thorwableProjOrigin, 0.3f, nullptr, false, false, true, false, false, false)) { // 0x7419CE
            CProjectileInfo::AddProjectile(
                firedBy,
                projType,
                thorwableProjOrigin,
                force,
                nullptr,
                targetEntity
            );
        } else {
            CProjectileInfo::RemoveNotAdd(firedBy, projType, projOrigin);
        }
    } else {
        CProjectileInfo::RemoveNotAdd(firedBy, projType, projOrigin);
    }
    CWorld::pIgnoreEntity = nullptr;

    if (firedByPed) { // 0x741A74
        CCrime::ReportCrime(CRIME_EXPLOSION, firedByPed, firedByPed);
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_22, firedBy);
    } else if (firedBy->GetIsTypeVehicle()) { // 0x741B10
        if (const auto drvr = firedBy->AsVehicle()->m_pDriver) {
            CCrime::ReportCrime(CRIME_FIRE_WEAPON, firedBy, drvr);
            g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_22, drvr);
        }
    }

    GetEventGlobalGroup()->Add(
        CEventGunShot{
            firedBy,
            projOrigin,
            targetEntity
                ? targetEntity->GetPosition()
                : targetPos
                    ? *targetPos
                    : projOrigin,
            notsa::contains({WEAPON_PISTOL_SILENCED, WEAPON_TEARGAS}, GetType())
        }
    );

    return true;
}

// 0x741C00
bool CWeapon::FireM16_1stPerson(CPed* owner) {
    const auto cam = &TheCamera.GetActiveCam();

    switch (cam->m_nMode) {
    case MODE_M16_1STPERSON:
    case MODE_SNIPER:
    case MODE_CAMERA:
    case MODE_ROCKETLAUNCHER:
    case MODE_ROCKETLAUNCHER_HS:
    case MODE_M16_1STPERSON_RUNABOUT:
    case MODE_SNIPER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT:
    case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
    case MODE_HELICANNON_1STPERSON:
        break;
    default:
        return false;
    }

    const auto wi = &GetWeaponInfo(); // NOTE: Why not `GetWeaponInfo(owner)`

    CWorld::bIncludeDeadPeds = true;
    CWorld::bIncludeCarTyres = true;
    CWorld::bIncludeBikers   = true;

    const auto camOriginPos = cam->m_vecSource;
    const auto camTargetPos = camOriginPos + cam->m_vecFront * 3.f;

    CBirds::HandleGunShot(&camOriginPos, &camTargetPos);
    CShadows::GunShotSetsOilOnFire(camOriginPos, camTargetPos);

    CColPoint shotCP;
    CEntity*  shotHitEntity;
    if (CWorld::ProcessLineOfSight(camOriginPos, camTargetPos, shotCP, shotHitEntity, true, true, true, true, true, false, false, true)) {
        CheckForShootingVehicleOccupant(&shotHitEntity, &shotCP, m_Type, camOriginPos, camTargetPos);
    }

    CWorld::bIncludeDeadPeds = false;
    CWorld::bIncludeCarTyres = false;
    CWorld::bIncludeBikers   = false;
    CWorld::pIgnoreEntity    = nullptr;

    //> 0x741DC4 - Check if hit entity is within range
    if (shotHitEntity) {
        if (TargetWeaponRangeMultiplier(shotHitEntity, owner) * wi->m_fWeaponRange >= (camOriginPos - shotCP.m_vecPoint).SquaredMagnitude2D()) {
            shotHitEntity = nullptr;
        }
    }

    DoBulletImpact(owner, shotHitEntity, camOriginPos, camTargetPos, shotCP, false);

    //> 0x741E48 - Visual/physical feedback for the player(s)
    if (owner->IsPlayer()) {
        auto intensity = [&]{
            switch (m_Type) {
            case WEAPON_AK47:
                return 0.00015f;
            case WEAPON_M4:
                return 0.0003f;
            default:
                return 0.0002f;
            }
        }();
        if (FindPlayerPed()->bIsDucking || FindPlayerPed()->m_pAttachedTo) {
            intensity *= 0.3f;
        }

        // Move the camera around a little
        cam->m_fHorizontalAngle += (float)CGeneral::GetRandomNumberInRange(-64, 64) * intensity;
        cam->m_fVerticalAngle += (float)CGeneral::GetRandomNumberInRange(-64, 64) * intensity;

        // Do pad shaking
        const auto shakeFreq = (uint8)lerp(130.f, 210.f, std::clamp((20.f - (wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.f) / 80.f, 0.f, 1.f));
        CPad::GetPad(owner->GetPadNumber())->StartShake(
            (int16)(CTimer::GetTimeStep() * 20'000.f / (float)shakeFreq),
            shakeFreq,
            0
        );
    }

    return true;
}

// 0x742300
bool CWeapon::Fire(CEntity* firedBy, CVector* startPosn, CVector* barrelPosn, CEntity* targetEnt, CVector* targetPosn, CVector* altPosn) {
    const auto firedByPed = firedBy && firedBy->GetIsTypePed()
        ? firedBy->AsPed()
        : nullptr;
    const auto wi = &GetWeaponInfo(firedByPed);

    CVector point{ 0.f, 0.f, 0.6f };

    const auto fxPos = startPosn
        ? startPosn
        : &point;
    const auto shotOrigin = startPosn
        ? barrelPosn
        : &point;
    if (!startPosn) {
        point     = firedBy->GetMatrix().TransformPoint(point);
        startPosn = &point;
    }

    if (m_IsFirstPersonWeaponModeSelected) {
        const auto r = 0.15f;

        const auto h = firedBy->GetHeading();
        fxPos->x -= std::sin(h) * r;
        fxPos->y += std::cos(h) * r;
    }

    switch (m_State) {
    case WEAPONSTATE_READY:
    case WEAPONSTATE_FIRING:
        break;
    default:
        return false;
    }

    if (!m_AmmoInClip) {
        if (!m_TotalAmmo) {
            return false;
        }
        m_AmmoInClip = std::min<uint32>(m_TotalAmmo, wi->m_nAmmoClip);
    }

    const auto [hasFired, delayNextShot] = [&]() -> std::pair<bool, bool> {
        switch (m_Type) {
        case WEAPON_GRENADE:
        case WEAPON_TEARGAS:
        case WEAPON_MOLOTOV:
        case WEAPON_REMOTE_SATCHEL_CHARGE: { // 0x74268B
            if (targetPosn) {
                return {
                    FireProjectile( // 0x742705
                        firedBy,
                        *shotOrigin,
                        targetEnt,
                        targetPosn,
                        std::clamp(((firedBy->GetPosition() - *targetPosn).Magnitude() - 10.f) / 10.f, 0.2f, 1.f)
                    ),
                    true
                };
            } else if (firedBy == FindPlayerPed()) { // 0x74271F
                return {
                    FireProjectile(
                        firedBy,
                        *shotOrigin,
                        targetEnt,
                        nullptr,
                        firedBy->AsPed()->GetPlayerData()->m_fAttackButtonCounter * 0.0375f
                    ),
                    true
                };
            }
            return {
                FireProjectile( // 0x74274E
                    firedBy,
                    *shotOrigin,
                    targetEnt,
                    nullptr,
                    0.3f
                ),
                true
            };
        }
        case WEAPON_PISTOL:
        case WEAPON_PISTOL_SILENCED:
        case WEAPON_DESERT_EAGLE:
        case WEAPON_MICRO_UZI:
        case WEAPON_MP5:
        case WEAPON_AK47:
        case WEAPON_M4:
        case WEAPON_TEC9:
        case WEAPON_COUNTRYRIFLE:
        case WEAPON_MINIGUN: { // 0x7424FE
            if (   firedByPed
                && firedByPed->m_nPedType == PED_TYPE_PLAYER1
                && notsa::contains({ MODE_M16_1STPERSON, MODE_HELICANNON_1STPERSON }, (eCamMode)TheCamera.m_PlayerWeaponMode.m_nMode)
            ) {
                return { FireM16_1stPerson(firedByPed), true };
            }
            const auto fired = FireInstantHit(firedBy, startPosn, shotOrigin, targetEnt, targetPosn, altPosn, false, true);
            if (firedByPed) { // 0x74255B
                if (!firedByPed->bInVehicle) {
                    return { fired, false };
                }
                if (const auto t = firedByPed->GetTaskManager().GetActiveTask()) {
                    return { fired, t->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY };
                }
            }
            return { fired, true };
        }
        case WEAPON_SHOTGUN:
        case WEAPON_SAWNOFF_SHOTGUN:
        case WEAPON_SPAS12_SHOTGUN:
            return {
                FireInstantHit( // 0x742495
                    firedBy,
                    startPosn,
                    shotOrigin,
                    targetEnt,
                    targetPosn,
                    altPosn,
                    false,
                    true
                ),
                true
            };
        case WEAPON_SNIPERRIFLE: { // 0x7424AC
            if (firedByPed && firedByPed->m_nPedType == PED_TYPE_PLAYER1 && TheCamera.m_PlayerWeaponMode.m_nMode == MODE_SNIPER) {
                return {
                    FireSniper(firedByPed, targetEnt, targetPosn),
                    true
                }; 
            }
            return {
                FireInstantHit(
                    firedBy,
                    startPosn,
                    shotOrigin,
                    targetEnt,
                    targetPosn,
                    nullptr,
                    false,
                    true
                ),
                true
            };
        }
        case WEAPON_RLAUNCHER:
        case WEAPON_RLAUNCHER_HS: { // 0x7425B3
            if (firedByPed) {
                const auto CanFire = [&](CVector origin, CVector end) {
                    return (origin - end).SquaredMagnitude() <= sq(8.f) && !firedBy->GetIsTypePed();
                };
                if (   targetEnt  && !CanFire(firedBy->GetPosition(), targetEnt->GetPosition())
                    || targetPosn && !CanFire(firedBy->GetPosition(), *targetPosn)
                ) {
                    return { false, true };
                }
            }
            return {
                FireProjectile(
                    firedBy,
                    *shotOrigin,
                    targetEnt,
                    targetPosn
                ),
                true
            };
        }
        case WEAPON_FLAMETHROWER:
        case WEAPON_SPRAYCAN:
        case WEAPON_EXTINGUISHER:
            return {
                FireAreaEffect(
                    firedBy,
                    *shotOrigin,
                    targetEnt,
                    targetPosn
                ),
                true
            };
        case WEAPON_DETONATOR: {
            assert(firedByPed);
            CWorld::UseDetonator(firedByPed);
            m_AmmoInClip = m_TotalAmmo  = 1;
            return { true, true };
        }
        case WEAPON_CAMERA:
            return {
                TakePhotograph(firedBy, shotOrigin),
                true
            };
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    // 0x74279A
    if (hasFired) {
        // 0x7427B3
        const bool isPlayerFiring = firedByPed && m_Type != WEAPON_CAMERA && firedByPed->IsPlayer();
        if (firedByPed) {
            if (m_Type != WEAPON_CAMERA) {
                firedByPed->bFiringWeapon = true;
            }
            firedByPed->GetWeaponAE().AddAudioEvent(AE_WEAPON_FIRE);
            if (isPlayerFiring && targetEnt && targetEnt->GetIsTypePed() && m_Type != WEAPON_PISTOL_SILENCED) {
                firedByPed->Say(CTX_GLOBAL_SHOOT, 200); // 0x74280E
            }
        }

        // 0x74282C
        if (m_Type == WEAPON_REMOTE_SATCHEL_CHARGE) {
            firedByPed->GiveWeapon(WEAPON_DETONATOR, true, true);
            if (firedByPed->GetWeapon(WEAPON_REMOTE_SATCHEL_CHARGE).m_TotalAmmo <= 1) {
                firedByPed->GetWeapon(WEAPON_DETONATOR).m_State = eWeaponState::WEAPONSTATE_READY;
                firedByPed->SetCurrentWeapon(WEAPON_DETONATOR);
            }
        }

        //> 0x74286D - Increase stats
        if (isPlayerFiring) {
            switch (m_Type)
            {
            case WEAPON_GRENADE:
            case WEAPON_MOLOTOV:
            case WEAPON_ROCKET:
            case WEAPON_RLAUNCHER:
            case WEAPON_RLAUNCHER_HS:
            case WEAPON_REMOTE_SATCHEL_CHARGE:
            case WEAPON_DETONATOR:
                CStats::IncrementStat(STAT_KGS_OF_EXPLOSIVES_USED);
                break;
            case WEAPON_PISTOL:
            case WEAPON_PISTOL_SILENCED:
            case WEAPON_DESERT_EAGLE:
            case WEAPON_SHOTGUN:
            case WEAPON_SAWNOFF_SHOTGUN:
            case WEAPON_SPAS12_SHOTGUN:
            case WEAPON_MICRO_UZI:
            case WEAPON_MP5:
            case WEAPON_AK47:
            case WEAPON_M4:
            case WEAPON_TEC9:
            case WEAPON_COUNTRYRIFLE:
            case WEAPON_SNIPERRIFLE:
            case WEAPON_MINIGUN:
                CStats::IncrementStat(STAT_BULLETS_FIRED);
                break;
            default:
                break;
            }
        }

        // 0x7428A6
        if (!CCheat::IsActive(CHEAT_INFINITE_AMMO)) {
            if (m_AmmoInClip) {
                m_AmmoInClip--;
            }
            if (m_TotalAmmo > 0) {
                if (isPlayerFiring
                        ? m_Type == WEAPON_DETONATOR || CStats::GetPercentageProgress() < 100.f
                        : m_TotalAmmo < 25'000
                ) {
                    m_TotalAmmo--;    
                }
            }
        }

        m_State = WEAPONSTATE_FIRING;

        if (!m_AmmoInClip) { // 0x7428FB
            if (m_TotalAmmo) {
                m_State = WEAPONSTATE_RELOADING;
                m_TimeForNextShotMs = s_DebugSettings.NoShotDelay
                    ? 0
                    : firedBy == FindPlayerPed() && FindPlayerInfo().m_bFastReload
                        ? wi->GetWeaponReloadTime() / 4
                        : wi->GetWeaponReloadTime();
                m_TimeForNextShotMs += CTimer::GetTimeInMS();
            } else if (TheCamera.GetActiveCam().m_nMode == MODE_CAMERA) {
                CPad::GetPad()->Clear(false, true);
            }
            return true;
        }

        m_TimeForNextShotMs = s_DebugSettings.NoShotDelay
            ? 0
            : delayNextShot
                ? m_Type == WEAPON_CAMERA
                    ? 1100
                    : (uint32)((wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.f)
                : 0;
        m_TimeForNextShotMs += CTimer::GetTimeInMS();
    }
    // 0x7429F2
    if (m_Type == WEAPON_UNARMED || m_Type == WEAPON_BASEBALLBAT) {
        return true;
    }
    return hasFired;
}

CWeaponInfo& CWeapon::GetWeaponInfo(CPed* owner) const {
    return GetWeaponInfo(owner ? owner->GetWeaponSkill(GetType()) : eWeaponSkill::STD);
}

CWeaponInfo& CWeapon::GetWeaponInfo(eWeaponSkill skill) const {
    return *CWeaponInfo::GetWeaponInfo(GetType(), skill);
}

// 0x73AF00
void FireOneInstantHitRound(const CVector& startPoint, const CVector& endPoint, int32 intensity) {
    CPointLights::AddLight(
        PLTYPE_POINTLIGHT,
        startPoint,
        CVector{0.f, 0.f, 0.f},
        3.f,
        0.25f,
        0.22f,
        0.0f
    );

    CColPoint hitCP;
    CEntity* hitEntity;
    CWorld::ProcessLineOfSight(
        startPoint,
        endPoint,
        hitCP,
        hitEntity,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        false
    );

    CBulletTraces::AddTrace(
        startPoint,
        hitEntity ? hitCP.m_vecPoint : endPoint,
        0.02f,
        750,
        150
    );

    if (hitEntity) {
        switch (hitEntity->GetType()) {
        case ENTITY_TYPE_PED: {
            const auto hitPed = hitEntity->AsPed();

            if (!notsa::contains({ PEDSTATE_DIE, PEDSTATE_DEAD }, hitPed->GetPedState())) {
                const auto pedHitDir = hitPed->GetLocalDirection(startPoint - hitPed->GetPosition2D());
                CAnimManager::AddAnimation(
                    hitPed->GetRpClump(),
                    ANIM_GROUP_DEFAULT,
                    std::to_array({ANIM_ID_SHOT_PARTIAL, ANIM_ID_SHOT_LEFTP, ANIM_ID_SHOT_PARTIAL_B, ANIM_ID_SHOT_RIGHTP})[pedHitDir]
                );
                CWeapon::GenerateDamageEvent(
                    hitPed,
                    nullptr,
                    WEAPON_UZI_DRIVEBY,
                    intensity,
                    (ePedPieceTypes)hitCP.m_nPieceTypeB,
                    pedHitDir
                );
            }
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            const auto hitVeh = hitEntity->AsVehicle();

            hitVeh->InflictDamage(
                nullptr,
                WEAPON_MICRO_UZI,
                (float)intensity,
                CVector{0.f, 0.f, 0.f}
            );
            break;
        }
        }

        const auto angleOfIncidenceCos = (endPoint - startPoint).Normalized().Dot(hitCP.m_vecNormal); // 0x73B0CC
        if (angleOfIncidenceCos < 0.f) {
            AudioEngine.ReportBulletHit(
                hitEntity,
                hitCP.m_nSurfaceTypeB,
                hitCP.m_vecPoint,
                RWRAD2DEG(std::asin(-angleOfIncidenceCos)) // Really should've used `acos + PI / 2` here to make this cleaner
            );
        }
    } else { // no hit entity
        float waterZ;
        if (CWaterLevel::GetWaterLevel(endPoint.x, endPoint.y, endPoint.z + 10.f, waterZ, true, nullptr)) {
            AudioEngine.ReportBulletHit(
                nullptr,
                SURFACE_WATER_SHALLOW,
                {endPoint.x, endPoint.y, waterZ},
                0.f
            );
        }
    }
}

float CWeapon::GetWeaponRange(CPed* owner, CEntity* target) const noexcept {
    const auto r = GetWeaponInfo(owner).m_fTargetRange;
    if (target) {
        return r * TargetWeaponRangeMultiplier(target, owner);
    }
    return r;
}
