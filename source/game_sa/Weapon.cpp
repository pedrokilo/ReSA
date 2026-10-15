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
 * Funciones bloqueadas: 0.
 * Validación registrada: 41/41 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
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
#include "X87Math.h"
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
    // Como el original: 0x73B43E/0x73B44B "cmp eax,1869Fh; jle" es una comparacion CON signo (un uint32 con el bit alto no se recorta)
    m_TotalAmmo{ (int32)ammo > 99'999 ? 99'999u : ammo }
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

    // Como el original (0x73A422..0x73A453): distancia al cuadrado sumada como (dz*dz + dx*dx) + dy*dy; sale si es > 100.
    {
        const CVector& creatorPos = creator->GetPosition();
        const CVector& cameraPos = TheCamera.GetPosition();
        const float dx = cameraPos.x - creatorPos.x, dy = cameraPos.y - creatorPos.y, dz = cameraPos.z - creatorPos.z;
        if (((dz * dz + dx * dx) + dy * dy) > 100.0f) {
            return;
        }
    }

    // Como el original (0x73A476..0x73A49F): z = rand() * 0x858C7C * 0x859934 * 20 + 0x862D50 (constantes leidas del EXE, no son
    // 1/32767, 0.06 ni 0.4 redondeados) y despues una segunda consulta 0x407180(-20, 20) cuyo resultado se descarta pero consume rand().
    const float z = (((float)CGeneral::GetRandomNumber() * std::bit_cast<float>(0x38000100u)) * std::bit_cast<float>(0x3D75C28Fu)) * 20.0f + std::bit_cast<float>(0x3ECCCCCCu);
    (void)CGeneral::GetRandomIntInRange(-20, 20);
    const CVector velocity{ direction.x * 1.0f, direction.y * 1.0f, z };

    // Add gunshell fx particle
    FxPrtMult_c fxprt(0.5f, 0.5f, 0.5f, 1.0f, size, 1.0f, 1.0f);
    switch (m_Type) {
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SHOTGUN:
        fxprt.SetColor(0.6f, 0.1f, 0.1f);
    }
    g_fx.m_GunShell->AddParticle(position, velocity, 0.0f, fxprt);
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

    // Como el original (0x73A5C7..0x73A60E): sale de la rama si GetSimplestActiveTask() devuelve nulo (se consulta dos veces)
    if (   victim->m_fHealth <= 0.f
        && CLocalisation::Blood()
        && CLocalisation::KickingWhenDown()
        && victim->GetTaskManager().GetSimplestActiveTask()
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
            // Como el original (0x73A653..0x73A66A): primero SetCurrentTime(0) (0x4CEA80, NO Start 0x4CEB70) y despues flags = (flags & 0xFFF7) | 1
            floorHitAnim->SetCurrentTime(0.0f);
            floorHitAnim->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE, false);
            floorHitAnim->SetFlag(ANIMATION_IS_PLAYING, true);
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
    // Como el original (0x73A6E5..0x73A72A): GetWeaponInfo(tipo, STD) se consulta SIEMPRE (la desviacion FIX_BUGS con TypeIsWeapon se quito: el
    // EXE lee la entrada del tipo tambien para WEAPON_RAMMEDBYCAR..WEAPON_FLARE, que existen en la tabla)
    if (!victim->bInVehicle && (
           CWeaponInfo::GetWeaponInfo(weaponType)->m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE
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
            anim->SetCurrentTime(0.0f); // Como el original (0x73A7A4): "push 0; call 0x4CEA80" = SetCurrentTime; Start() ademas activaba el flag ANIMATION_IS_PLAYING
            eventDmg.m_bAnimAdded = true; // Como el original: 0x73A7A9 or byte ptr [esp+4Dh],4 (bit 2 del byte 0x25 del evento)
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
            eventDmg.m_bAnimAdded = true; // Como el original: 0x73A7E0 or al,4 (y no modifica los flags de la animacion)
            break;
        }
        }
    }

    // 0x73A828
    // Como el original (0x73A7ED..0x73A828): primero la tarea activa del creador (con comprobacion de nulo) y despues el arma
    {
        bool stealth = false;
        if (creator && creator->GetIsTypePed()) {
            const auto* const activeTask = creator->AsPed()->GetTaskManager().GetActiveTask();
            if (activeTask && activeTask->GetTaskType() == TASK_SIMPLE_STEALTH_KILL) {
                stealth = true;
            } else if (weaponType == WEAPON_PISTOL_SILENCED) {
                stealth = true;
            }
        }
        eventDmg.m_bStealthMode = stealth;
    }

    // Como el original (0x73A83C..0x73A895): "jne" con salud <= 0 o NaN anade el evento; se anade con Add(&evento, false)
    if (!victim->bInVehicle || !(victim->m_fHealth > 0.f) || !victim->GetTaskManager().GetActiveTask() || victim->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_SIMPLE_GANG_DRIVEBY) {
        victim->GetEventGroup().Add(&eventDmg, false);
    }

    return ret;

}

// 0x73A8D0
bool CWeapon::LaserScopeDot(CVector* outCoord, float* outSize) {
    // Como el original (0x73A8D0..0x73AAB9): la funcion existe en el EXE (sin llamadas conocidas, pero es codigo real). Traza el rayo
    // desde la camara activa: inicio = origen + frente * 0.5; fin = inicio + frente_normalizado * alcance; si golpea algo y el punto se
    // proyecta en pantalla devuelve las coordenadas, el tamano (ancho * 0.05) y registra una corona roja.
    const auto wi = CWeaponInfo::GetWeaponInfo(m_Type, eWeaponSkill::STD);
    const float range = wi->m_fWeaponRange;
    const auto& cam = TheCamera.GetActiveCam();

    const CVector start{
        cam.m_vecFront.x * 0.5f + cam.m_vecSource.x,
        cam.m_vecFront.y * 0.5f + cam.m_vecSource.y,
        cam.m_vecFront.z * 0.5f + cam.m_vecSource.z
    };
    CVector unitFront = cam.m_vecFront;
    unitFront.Normalise(); // 0x73A981 (0x59C910)
    const CVector end{
        start.x + unitFront.x * range,
        start.y + unitFront.y * range,
        start.z + unitFront.z * range
    };

    CColPoint colPoint;
    CEntity*  hitEntity{};
    if (!CWorld::ProcessLineOfSight(start, end, colPoint, hitEntity, true, true, true, true, false, false, false, false)) { // 0x73A9EA
        return false;
    }

    CVector screen = colPoint.m_vecPoint; // 0x73AA08: la salida se inicializa con el punto de impacto
    float width, height;
    if (!CSprite::CalcScreenCoors(colPoint.m_vecPoint, &screen, &width, &height, true, true)) { // 0x73AA2A
        return false;
    }

    *outCoord = screen;
    *outSize  = width * std::bit_cast<float>(0x3D4CCCCDu); // 0x73AA58: fmul [0x858C28] (0.05f)
    CCoronas::RegisterCorona(
        (uint32)(uintptr_t)this + 7, // 0x73AA9E: add esi,7
        nullptr,
        0x80, 0, 0, 0xFF,
        colPoint.m_vecPoint,
        1.2f,  // 0x3F99999A
        50.0f, // 0x42480000
        (eCoronaType)0,
        FLARETYPE_NONE,
        CORREFL_SIMPLE,
        LOSCHECK_OFF,
        TRAIL_ON,
        0.0f,
        false,
        1.5f,
        false,
        15.0f,
        false,
        false
    );
    return true;
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
    // Como el original (0x73AB63..0x73AB9E): (z * 0.145 - y * 0.9894) + x * 0.0 (0x858B50), en ese orden
    float vecFrontZ_Y = (activeCam.m_vecFront.z * 0.145f - activeCam.m_vecFront.y * 0.98940003f) + activeCam.m_vecFront.x * 0.0f;

    if (vecFrontZ_Y > 0.99699998f)
        CCoronas::MoonSize = (CCoronas::MoonSize + 1) % 8;

    // Como el original (0x73ABE1..0x73AC4B): el vector normalizado se conserva SIN multiplicar; a AddBullet solo se le pasa una copia * 16
    CVector direction = activeCam.m_vecFront;
    direction.Normalise();

    // Como el original (0x73ABEA..0x73AC30): los fstp [esp+28h/2Ch/30h] escriben dir*16 SOBRE el propio vector; el primer evento usa ese vector
    // ya multiplicado (x16 y despues x40) y solo el segundo lo renormaliza (0x73ADC7).
    direction = CVector{ direction.x * 16.0f, direction.y * 16.0f, direction.z * 16.0f };
    CBulletInfo::AddBullet(shooter, m_Type, activeCam.m_vecSource, direction);

    // recoil effect for players
    if (shooter->IsPlayer()) {
        // Como el original (0x73AC66..0x73ACD3): tres consultas a FindPlayerPed(-1): z de la primera, y de la segunda, x de la tercera
        const float creatorZ = FindPlayerPed()->GetPosition().z;
        const float creatorY = FindPlayerPed()->GetPosition().y;
        const float creatorX = FindPlayerPed()->GetPosition().x;
        CVector creatorPos{ creatorX, creatorY, creatorZ };
        CPad* creatorPad = CPad::GetPad(shooter->m_nPedType);

        creatorPad->StartShake_Distance(240, 128, creatorPos);
        CamShakeNoPos(&TheCamera, 0.2f);
    }

    if (shooter->GetIsTypePed()) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, shooter, shooter);
    } else if (shooter->GetIsTypeVehicle() && shooter->m_roadRageWith) {
        CCrime::ReportCrime(CRIME_FIRE_WEAPON, shooter, shooter->m_roadRageWith);
    }

    // Como el original (0x73AD23..0x73AE59): punto = direccion normalizada * 40 + origen; el vector se normaliza OTRA VEZ
    // (0x73ADC7) antes del segundo evento, que usa su propio punto.
    bool hasNoSound = m_Type == eWeaponType::WEAPON_PISTOL_SILENCED || m_Type == eWeaponType::WEAPON_TEARGAS;

    const CVector targetPoint = direction * 40.0f + activeCam.m_vecSource;
    CEventGunShot gs(shooter, activeCam.m_vecSource, targetPoint, hasNoSound);
    GetEventGlobalGroup()->Add(static_cast<CEvent*>(&gs), false);

    direction.Normalise();
    const CVector targetPoint2 = direction * 40.0f + activeCam.m_vecSource;
    CEventGunShotWhizzedBy gsw(shooter, activeCam.m_vecSource, targetPoint2, hasNoSound);
    GetEventGlobalGroup()->Add(static_cast<CEvent*>(&gsw), false);

    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_22, shooter);

    return true;
}

// 0x73AEB0
void CWeapon::Reload(CPed* owner) {
    if (!m_TotalAmmo) {
        return;
    }

    // Como el original: 0x73AEDA "movsx eax,word ptr [eax+20h]" (el tamano del cargador se extiende con signo)
    const uint32 ammo = (uint32)(int32)(int16)GetWeaponInfo(owner).m_nAmmoClip;
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

// Como el original (CRT 0x821E70): asin(x) = fpatan(x, fsqrt((1+x)*(1-x))) con las sumas, el producto y la raiz redondeados a float (x87 a 24 bits)
// y el fpatan a precision completa; despues "fmul 57.29578" (0x42652EE0) redondeado a float.
static float AsinDegreesX87(float x) {
    if (!(x < 1.0f)) {
        return (float)(std::asin((double)x) * (double)std::bit_cast<float>(0x42652EE0u));
    }
    return ExeAsinDegreesCpp(x); // _CIasin (0x821E70) por 57.29578f (0x859878), mismo cuerpo que usa CBulletInfo::Update
}

// 0x73B550
void CWeapon::DoBulletImpact(CEntity* firedBy, CEntity* victim, const CVector& startPoint, const CVector& endPoint, const CColPoint& hitCP, int32 incrementalHit) {
    // Como el original (0x73B586..0x73B597): el tirador puede ser nulo ("test ebx,ebx; je"); solo la rama sin victima lo tolera
    const auto firedByPed = firedBy && firedBy->GetIsTypePed()
        ? firedBy->AsPed()
        : nullptr;
    const auto firedByPlayer = firedByPed && firedByPed->IsPlayer()
        ? firedByPed->AsPlayer()
        : nullptr;

    // Como el original (0x73B59B): el nivel de habilidad sale de CPed::GetWeaponSkill() sin argumentos (0x5E6580, arma ACTIVA del
    // tirador), no de GetWeaponSkill(m_Type) (0x5E3B60).
    const auto wi = CWeaponInfo::GetWeaponInfo(m_Type, firedByPed ? firedByPed->GetWeaponSkill() : eWeaponSkill::STD);

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
                    // Como el original (0x73B91F..0x73B93E): asin del CRT (0x821E70, precision completa) y despues "fmul 57.29578" (0x859878 = 0x42652EE0,
                    // que NO es el float mas cercano a 180/pi); RWRAD2DEG(std::asin(float)) daba otra constante y un asin de simple precision.
                    AudioEngine.ReportBulletHit(
                        victim,
                        hitCP.m_nSurfaceTypeB,
                        hitCP.m_vecPoint,
                        AsinDegreesX87(-angle)
                    );
                }
            }
        };

        // Como el original (0x73B6BB..0x73B6CD): "(jugador && victima != tirador) || estado del tirador == STATUS_PLAYER". La rama FIX_BUGS
        // (jugador && (victima != tirador || estado == STATUS_PLAYER)) se quito: el EXE no la tiene.
        if (firedByPlayer && firedByPlayer != victim || firedBy->GetStatus() == STATUS_PLAYER) { // 0x73B6D0
            if (notsa::contains({ ENTITY_TYPE_PED, ENTITY_TYPE_VEHICLE, ENTITY_TYPE_OBJECT }, victim->GetType())) {
                // Como el original (0x73B6DE..0x73B716): se consulta primero 0x80 (BULLETS_THAT_HIT) y despues 0x7E (BULLETS_FIRED);
                // "fcomp" ordenado: con NaN no se incrementa
                const float bulletsThatHit = CStats::GetStatValue(STAT_BULLETS_THAT_HIT);
                const float bulletsFired = CStats::GetStatValue(STAT_BULLETS_FIRED);
                if (bulletsFired >= bulletsThatHit) {
                    CStats::IncrementStat(STAT_BULLETS_THAT_HIT);
                }
            }
            if (firedByPed && CWeaponInfo::TypeHasSkillStats(GetType())) { // 0x73B738 (0x73B731 exige que el tirador sea un ped)
                // Redundant `if` check here was removed
                // Como el original (0x73B731..0x73B858): el orden de las comprobaciones y de las llamadas es el del EXE.
                if ([&]{
                    const auto victimEntity = notsa::coalesce(firedByPed->m_pTargetedObject, victim);
                    bool hits = false;

                    switch (victimEntity->GetType()) {
                    case eEntityType::ENTITY_TYPE_PED: { // 0x73B75F: fcomp + "test ah,41h; je" => salud > 0 (ordenada)
                        hits = victimEntity->AsPed()->m_fHealth > 0.f;
                        break;
                    }
                    case eEntityType::ENTITY_TYPE_VEHICLE: { // 0x73B772: salud > 0 (ordenada) y estado != STATUS_WRECKED
                        const auto victimVeh = victimEntity->AsVehicle();
                        if (victimVeh->m_fHealth > 0.f && victimVeh->GetStatus() != STATUS_WRECKED) {
                            hits = true;
                        }
                        // 0x73B7D7: despues, antibalas o no danable => falso
                        if (victimVeh->physicalFlags.bBulletProof || !victimVeh->vehicleFlags.bCanBeDamaged) {
                            hits = false;
                        }
                        // 0x73B7F6: ruedas => BurstTyre(pieza, true) (ranura 0x2C) siempre; si devuelve falso => falso
                        if (notsa::contains(eCarPiece_WheelPieces, (eCarPiece)hitCP.m_nPieceTypeB)) {
                            if (!victimVeh->BurstTyre(hitCP.m_nPieceTypeB, true)) {
                                hits = false;
                            }
                        }
                        break;
                    }
                    case eEntityType::ENTITY_TYPE_OBJECT: { // 0x73B797
                        const auto victimObj = victimEntity->AsObject();
                        if (victimObj->m_fHealth > 0.f && victimObj->m_nColDamageEffect && victimObj->m_pObjectInfo->m_fColDamageMultiplier >= 0.5f) {
                            hits = true;
                        }
                        break;
                    }
                    }

                    // 0x73B828..0x73B83F: con un ped (UNA sola consulta, siempre) => si estan en el mismo grupo no cuenta
                    if (victimEntity->GetIsTypePed() && CPedGroups::AreInSameGroup(victimEntity->AsPed(), firedByPed)) {
                        return false;
                    }
                    return hits;
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
                        // Como el original (0x73BB5A, 0x73BEF8, 0x73C01F): la iluminacion es la del lado B (byte 0x25 del CColPoint), no la A (0x22)
                        hitCP.m_nLightingB.GetCurrentLighting()
                    );
                }
            };

            switch (victim->GetType()) {
            case eEntityType::ENTITY_TYPE_BUILDING: { // 0x73C014
                DoBulletImpactFx();
                if (firedByPlayer) {
                    // Como el original (0x73C050..0x73C066): los datos de jugador pueden ser nulos y el indice de modelo se extiende con signo (movsx word)
                    if (const auto pd = firedByPlayer->GetPlayerData()) {
                        pd->m_nModelIndexOfLastBuildingShot = (uint32)(int32)(int16)victim->m_nModelIndex;
                    }
                }
                break;
            }
            case eEntityType::ENTITY_TYPE_VEHICLE: { // 0x73BD2A
                const auto victimVeh = victim->AsVehicle();
                if (!notsa::contains(eCarPiece_WheelPieces, (eCarPiece)hitCP.m_nPieceTypeB)) {
                    victimVeh->InflictDamage(
                        firedBy,
                        GetType(),
                        // Como el original (0x73BE6D..0x73BEAA): movsx word + fild; con dos jugadores en coche el dano se duplica DESPUES de pasar a float
                        (float)(firedByPlayer && TheCamera.GetActiveCam().m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING ? 2 * (int32)(int16)wi->m_nDamage : (int32)(int16)wi->m_nDamage),
                        hitCP.m_vecPoint
                    );
                    DoBulletImpactFx();
                    if ((int32)g_LoadMonitor.GetProcLevel() < 2) { // 0x73BF6C "cmp [0xB7298C],2; jge" (comparacion con signo) - NOTE/TODO: Useless, remove
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
                            // Como el original (0x73BF72..0x73BF8C): masa * 0x858CDC (0.001f) y min(1.0, x) = (1.0 < x) ? 1.0 : x (0x404330)
                            hitCP.m_vecNormal * (wepForceMult * [&]{ const float x = victimVeh->m_fMass * 0.001f; return (1.0f < x) ? 1.0f : x; }()),
                            hitCP.m_vecPoint - victimVeh->GetPosition(),
                            true
                        );
                    }
                } else { // 0x73BD3F
                    victimVeh->BurstTyre(hitCP.m_nPieceTypeB, true);
                    g_fx.AddTyreBurst(hitCP.m_vecPoint, hitCP.m_vecNormal);
                    if (firedByPed) { // Add event to occupants
                        // Como el original (0x73BD84..0x73BE64): el evento se construye UNA vez en la pila y se anade con Add(&evento, false)
                        // (la plantilla Add(T) por valor hacia una copia del evento y registraba/liberaba referencias de mas)
                        const auto AddEventVehicleDamageWeapon = [&](CPed* ped) {
                            if (ped) {
                                CEventVehicleDamageWeapon event{
                                    victimVeh,
                                    firedBy,
                                    GetType()
                                };
                                ped->GetEventGroup().Add(&event, false);
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
                    // Como el original (0x73BD0C): con ColDamageEffect < 200 SIEMPRE se llama a ObjectDamage(50.0, ...) al final
                    victimObj->ObjectDamage(50.0f, &hitCP.m_vecPoint, &hitCP.m_vecNormal, firedBy, GetType());
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
        } else if (victim == firedBy) {
            return; // Como el original: 0x73B89F "cmp esi,ebp; je 0x73C1BB" salta tambien la sacudida del mando
        } else {
            const auto victimPed = victim->AsPed();
            if (   !firedByPed
                || firedByPed->m_nPedType != victimPed->m_nPedType
                || notsa::contains({ PED_TYPE_CIVMALE, PED_TYPE_CIVFEMALE }, victimPed->m_nPedType)
                || firedByPlayer
            ) {
                const auto bAddBloodFx = [&]{
                    if (incrementalHit > 0) { // 0x73B8B0: mov byte ptr [esp+8Ch],1 ; jg 0x73BA5F -> el flag queda en TRUE (como el original)
                        return true;
                    }
                    DoBulletHitFx();
                    // Como el original (0x73B949..0x73BA58): primero la direccion local (0x5DEF60), despues el dano (movsx word) y al final
                    // GenerateDamageEvent; el orden de evaluacion de los argumentos de la llamada no esta definido, asi que va en sentencias.
                    const int32 localDirection = victimPed->GetLocalDirection(startPoint - victimPed->GetPosition2D());
                    int32 damage = (int32)(int16)wi->m_nDamage;
                    if (incrementalHit < 0) {
                        damage = -(damage * incrementalHit);
                    }
                    if (firedByPlayer
                        && (victim->GetPosition() - startPoint).Magnitude() < 1.0f // 0x73B9F8: Magnitude() (4082C0) y "mag < 1.0"
                        && !victimPed->bNoCriticalHits
                        && !notsa::contains({ WEAPON_SHOTGUN, WEAPON_SPAS12_SHOTGUN }, GetType())
                    ) {
                        damage = 150;
                    }
                    return GenerateDamageEvent(
                        victimPed,
                        firedBy,
                        GetType(),
                        damage,
                        (ePedPieceTypes)hitCP.m_nPieceTypeB,
                        (uint8)localDirection
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
                                ? victimPed->m_fHealth < 0.f ? 32 : 16 // 0x73BABA: test ah,5 + jp -> 0x20 solo con salud < 0 estricta
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
        // Como el original (0x73C149..0x73C1B6): tres consultas a FindPlayerPed(-1) (z de la primera, y de la segunda, x de la tercera)
        const float shakeZ = FindPlayerPed()->GetPosition().z;
        const float shakeY = FindPlayerPed()->GetPosition().y;
        const float shakeX = FindPlayerPed()->GetPosition().x;
        firedByPed->AsPlayer()->GetPadFromPlayer()->StartShake_Distance(
            240,
            128,
            CVector{ shakeX, shakeY, shakeZ }
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

    // Como el original (0x73C2A1..0x73C2EC y 0x73C54C..0x73C599): distancia entidad - camara con (dz*dz + dy*dy) + dx*dx y fsqrt;
    // solo se procesa la entidad si distancia < 125 (0x865038). El EXE recorre los pools del ultimo indice al primero.
    const CVector camPos = TheCamera.GetPosition();
    const auto IsPosInRange = [&](const CVector& worldPos) {
        const float dx = worldPos.x - camPos.x, dy = worldPos.y - camPos.y, dz = worldPos.z - camPos.z;
        return std::sqrt((dz * dz + dy * dy) + dx * dx) < 125.0f;
    };

    // Check is in position in camera's frame
    // Como el original (0x73C32D..0x73C39F): todas las comparaciones son "ordenadas" (con NaN se descarta); el ancho/alto salen del
    // entero RsGlobal.maximumWidth/Height convertido a float.
    const auto IsPosInCamFrame = [](const CVector& worldPos) {
        CVector screen;
        float w, h;
        if (!CSprite::CalcScreenCoors(worldPos, &screen, &w, &h, false, true)) {
            return false;
        }

        const float width = (float)RsGlobal.maximumWidth, height = (float)RsGlobal.maximumHeight;
        if (!(width * 0.1f < screen.x)) return false;
        if (!(screen.x < width * 0.9f)) return false;
        if (!(height * 0.1f < screen.y)) return false;
        if (!(screen.y < height * 0.9f)) return false;
        return true;
    };

    // Como el original (0x73C443..0x73C4C8): el origen es camara + 2 * (frente de m_mCameraMatrix, 0xB6F9AC..0xB6F9B4), no el de
    // la matriz del placeable; "2 *" se hace como x + x.
    const CVector camFwd = TheCamera.m_mCameraMatrix.GetForward();
    const CVector losStart{ (camFwd.x + camFwd.x) + camPos.x, (camFwd.y + camFwd.y) + camPos.y, (camFwd.z + camFwd.z) + camPos.z };
    const auto CheckIsLOSBlocked = [&](const CVector& target, CEntity* ignore) {
        CColPoint _cp; // Unused
        CEntity* hitEntity{};
        if (!CWorld::ProcessLineOfSight(
            losStart,
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

    auto* const pedPool = GetPedPool();
    for (size_t i = pedPool->GetSize(); i-- > 0;) {
        auto* const pedPtr = pedPool->GetAt(i);
        if (!pedPtr) {
            continue;
        }
        auto& ped = *pedPtr;
        if (!IsPosInRange(ped.GetPosition())) {
            continue;
        }

        const auto pedHeadPos = ped.GetBonePosition(BONE_HEAD);

        if (!IsPosInCamFrame(pedHeadPos)) {
            continue;
        }

        // Como el original (0x73C3A5..0x73C43F): camara - cabeza normalizado (0x59C910) * 1.5 (0x858CE8) + cabeza
        CVector towardsCamera{ camPos.x - pedHeadPos.x, camPos.y - pedHeadPos.y, camPos.z - pedHeadPos.z };
        towardsCamera.Normalise();
        const CVector target{
            pedHeadPos.x + towardsCamera.x * 1.5f,
            pedHeadPos.y + towardsCamera.y * 1.5f,
            pedHeadPos.z + towardsCamera.z * 1.5f
        };
        if (!CheckIsLOSBlocked(target, &ped)) {
            ped.bHasBeenPhotographed = true;
        }
    }

    auto* const objPool = GetObjectPool();
    for (size_t i = objPool->GetSize(); i-- > 0;) {
        auto* const objPtr = objPool->GetAt(i);
        if (!objPtr) {
            continue;
        }
        auto& obj = *objPtr;
        const CVector objPos = obj.GetPosition();

        // Como el original (0x73C599..0x73C5A4): los objetos tambien se procesan SOLO si estan a menos de 125 (la version anterior
        // los descartaba dentro del rango).
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

    // Como el original (0x73C77E..0x73C7E8): hitDir = punto - origen; |hitDir| = sqrt((y*y + x*x) + z*z); depth = (|hitDir| * spread) * 1.3
    CVector hitDir{ colPoint.m_vecPoint.x - point.x, colPoint.m_vecPoint.y - point.y, colPoint.m_vecPoint.z - point.z };
    const float depth = (std::sqrt((hitDir.y * hitDir.y + hitDir.x * hitDir.x) + hitDir.z * hitDir.z) * CWorld::fWeaponSpreadRate) * 1.3f;

    //> 0x73C806 - Create pellet lines
    CColLine* const lines = cd->m_pLines; // Como el original: puntero directo (GetLines() es un span con m_nNumLines, que el EXE escribe DESPUES de rellenar las lineas: indexar el span vacio hace fail-fast 0xC0000409)
    lines[0].Set(
        { 0.f, -depth, 0.f },
        { 0.f,  depth, 0.f }
    );
    // Como el original (0x73C835..0x73C904): minimo = depth * 0.2 (0x858CC4), rango = depth - minimo, y = depth * -2.0 (0x858B18),
    // angulo = rand * 0x858C7C * 6.2831855 - 3.1415927 (0x858CBC/0x858CB8) y radio = rand * 0x858C7C * rango + minimo; seno/coseno x87.
    const float minSpread = depth * 0.2f;
    const float spreadRange = depth - minSpread;
    const float lineY = depth * -2.0f;
    const float lineY2 = depth + depth;
    const float randScale = std::bit_cast<float>(0x38000100u);
    const float twoPi = std::bit_cast<float>(0x40C90FDBu), onePi = std::bit_cast<float>(0x40490FDBu);
    for (int32 i = 1; i < numPellets; i++) {
        const int32 randomAngle = CGeneral::GetRandomNumber();
        const float angle = ((float)randomAngle * randScale) * twoPi - onePi;
        const int32 randomSpread = CGeneral::GetRandomNumber();
        const float spread = (((float)randomSpread * randScale) * spreadRange) + minSpread;

        // fcos/fsin sin redondear por spread y un solo redondeo a float (0x73C8BB..0x73C8D6)
        const float oX = TrigSimpleCosMulCpp(angle, spread);
        const float oZ = TrigSimpleSinMulCpp(angle, spread);

        lines[i].Set(
            { oX, lineY, oZ },
            { oX, lineY2, oZ }
        );
    }

    //> 0x73C91C - Como el original, el numero de lineas se escribe despues de generarlas
    cd->m_nNumLines = (uint8)numPellets;

    //> 0x73C923 -  Calculate bounding volumes
    cm->GetBoundingBox().Set(
        { -depth, lineY, -depth },
        {  depth, lineY2,  depth }
    );
    cm->GetBoundingSphere().m_fRadius = depth * 2.5f; // 0x73C923: solo se escribe el radio (el centro queda como esta)

    // Como el original (0x73C98E..0x73CAFF): el orden de los factores del producto vectorial depende de la rama:
    // edificio: normal x ayudante; hitDir z <= 0.9: hitDir x (0,0,1); sin ped: hitDir x (1,0,0); ped: frente-del-propietario x hitDir.
    CVector forward, right;
    if (victim->GetIsTypeBuilding()) { // 0x73C98E
        const auto& n = colPoint.m_vecNormal;
        forward = CVector{ -n.x, -n.y, -n.z };
        // 0x73C9CF: se examina |normal.x| (no z): < 0.9 -> (1,0,0); si no (o NaN) -> (0,1,0)
        const CVector helper = (std::abs(n.x) < 0.9f) ? CVector{ 1.f, 0.f, 0.f } : CVector{ 0.f, 1.f, 0.f };
        right = n.Cross(helper);
    } else {
        hitDir.Normalise(); // 0x73CA19 (0x59C910)
        forward = hitDir;
        if (!(std::abs(hitDir.z) > 0.9f)) { // 0x73CA4C: test ah,41h + jne
            right = hitDir.Cross(CVector{ 0.f, 0.f, 1.f });
        } else if (!owner->GetIsTypePed()) { // 0x73CA59
            right = hitDir.Cross(CVector{ 1.f, 0.f, 0.f });
        } else { // 0x73CA5B (si el propietario no tiene matriz, el EXE la crea: 0x54F560 + 0x54F1B0)
            right = owner->GetMatrix().GetForward().Cross(hitDir);
        }
    }
    right.Normalise(); // 0x73CADB (0x59C910)
    outMat.GetForward() = forward;
    outMat.GetRight()   = right;
    outMat.GetUp()      = right.Cross(forward);

    // 0x73CAFF
    outMat.GetPosition() = colPoint.m_vecPoint;

    // Como el original (0x73CB28..0x73CB86): dot = (nz*fz + ny*fy) + nx*fx; pos += ((-depth * f) * dot)
    if (!victim->GetIsTypeBuilding()) {
        const CVector& f = outMat.GetForward();
        const CVector& n = colPoint.m_vecNormal;
        const float negDepth = -depth;
        const float dot = (n.z * f.z + n.y * f.y) + n.x * f.x;
        outMat.GetPosition().x += (negDepth * f.x) * dot;
        outMat.GetPosition().y += (negDepth * f.y) * dot;
        outMat.GetPosition().z += (negDepth * f.z) * dot;
    }
}

// 0x73CBA0
void CWeapon::FireInstantHitFromCar2(CVector startPoint, CVector endPoint, CVehicle* vehicle, CEntity* owner) {
    CCrime::ReportCrime(CRIME_FIRE_WEAPON, FindPlayerPed(), FindPlayerPed());

    // Como el original (0x73CC5D..0x73CC7F): evento construido una vez y anadido con Add(&evento, false)
    CEventGunShot gunShotEvent{
        notsa::coalesce<CEntity*>(owner, vehicle),
        startPoint,
        endPoint,
        notsa::contains({WEAPON_PISTOL_SILENCED, WEAPON_TEARGAS}, GetType())
    };
    GetEventGlobalGroup()->Add(&gunShotEvent, false);
    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_22, owner);

    CPointLights::AddLight(PLTYPE_POINTLIGHT, startPoint, {}, 3.0f, 0.25f, 0.22f, 0.0f, 0, false, nullptr);
    CWorld::bIncludeBikers = true;
    CWorld::pIgnoreEntity = vehicle;
    CBirds::HandleGunShot(&startPoint, &endPoint);
    CShadows::GunShotSetsOilOnFire(startPoint, endPoint);

    CEntity* victim{};
    CColPoint cpImpact; // Como el original: sin inicializar (el EXE solo lo escribe si el rayo golpea algo)
    CWorld::ProcessLineOfSight(startPoint, endPoint, cpImpact, victim, true, true, true, true, true, false, false, true);
    CWorld::ResetLineTestOptions();
    DoBulletImpact(owner, victim, startPoint, endPoint, cpImpact, 0);
}

// 0x73CDC0
void CWeapon::DoDoomAiming(CEntity* owner, CVector* start, CVector* end) {
    int16 inRangeCount{};
    std::array<CEntity*, 16> objInRange{};
    // Como el original (0x73CDD0..0x73CE1A): radio = |end - start| sumado como (dz*dz + dy*dy) + dx*dx; el maximo de objetos es 15 (push 0Fh)
    const float rangeDX = end->x - start->x, rangeDY = end->y - start->y, rangeDZ = end->z - start->z;
    const float searchRadius = std::sqrt((rangeDZ * rangeDZ + rangeDY * rangeDY) + rangeDX * rangeDX);
    CWorld::FindObjectsInRange(*start, searchRadius, true, &inRangeCount, 15, objInRange.data(), false, true, true, false, false);

    CEntity* closestEntity{};
    float    closestDist{ 10'000 };
    for (auto entity : std::span{ objInRange.begin(), (size_t)std::max<int16>(inRangeCount, 0) }) {
        // Como el original (0x73CE58..0x73CE67): "call 5E0730; test al,al; je" -> solo se consideran las entidades que el propietario VE
        // dentro de +-0x3EC90FDB (pi/8 exacto), no las que no ve.
        if (entity == owner || !owner->AsPed()->CanSeeEntity(entity, std::bit_cast<float>(0x3EC90FDBu))) { // todo: add check owner->GetIsTypePed() NOTSA
            continue;
        }

        switch (entity->GetStatus()) {
        case STATUS_TRAIN_MOVING:
        case STATUS_TRAIN_NOT_MOVING:
        case STATUS_WRECKED:
            continue;
        }

        // Como el original (0x73CF92..0x73D091): dx, dy con la matriz (owner - entidad), dist2D = sqrt(dx*dx + dy*dy),
        // |dz| = fchs de (owner.z - entidad.z) si es < 0, y dist3D = sqrt(dz*dz + dist2D*dist2D) (no std::hypot).
        const float ex = owner->GetPosition().x - entity->GetPosition().x, ey = owner->GetPosition().y - entity->GetPosition().y;
        const float dist2D = std::sqrt(ex * ex + ey * ey);
        const float dzSigned = owner->GetPosition().z - entity->GetPosition().z;
        const float absDz = (dzSigned < 0.f) ? -dzSigned : dzSigned; // jp: dz >= 0 o NaN conservan el signo
        if (1.5f * absDz < dist2D) {
            const float dist3D = std::sqrt(absDz * absDz + dist2D * dist2D);
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
    // Como el original (0x73DA19..0x73DA48): el umbral es tan(angulo * 0x8595EC) SIN redondear a float (queda en ST0 y se compara con la
    // puntuacion), o 2.5 (0x858FA0) si el angulo no supera 0.5 (ni es NaN).
    const bool inRange = aimAngle > 0.5f
        ? WeaponLessThanTanCpp(bestScore, aimAngle * std::bit_cast<float>(0x3C8EFA35u))
        : bestScore < 2.5f;
    if (inRange) {
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
        // Como el original (0x73A280/0x73A2B0): el retraso se lee como palabra SIN signo (mov ax / movzx), no como int16 con signo
        ProcessOne((uint16)(owner->bIsDucking ? ao->CrouchRLoadA : ao->RLoadA), AE_WEAPON_RELOAD_A);
        ProcessOne((uint16)(owner->bIsDucking ? ao->CrouchRLoadB : ao->RLoadB), AE_WEAPON_RELOAD_B);
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
                    if (!rload) { // Como el original: 0x73DE2C "test ax,ax; jbe" (retraso 0 -> sin evento)
                        return false;
                    }
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
                // Como el original (0x73DCF2..0x73DCF8): GetTaskUseGun se consulta SIEMPRE tras buscar la animacion, aunque exista
                const auto useGunTask = owner->GetIntelligence()->GetTaskUseGun();
                if (animRLoad) { // 0x73DD30
                    ProcessReloadAudioIf([&](uint32 rloadMs, eAudioEvents ae) {
                        const auto rloadS = (float)rloadMs * 0.001f; // Como el original: 0x73DD30 fmul [0x858CDC] (0.001f), no una division
                        return rloadS <= animRLoad->m_CurrentTime && animRLoad->m_CurrentTime - animRLoad->m_TimeStep < rloadS;
                    });
                    if (CTimer::GetTimeInMS() > m_TimeForNextShotMs) {
                        if (animRLoad->GetTimeProgress() < 0.9f) {
                            m_TimeForNextShotMs = CTimer::GetTimeInMS();
                        }
                    }
                } else if (useGunTask) { // 0x73DDF9
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
    // Como el original (0x73E575..0x73E5A7): distancia = sqrt((dz*dz + dy*dy) + dx*dx) con dx = objetivo - origen
    const CVector targetPos = potentialTarget->GetPosition();
    const float dx = targetPos.x - origin.x, dy = targetPos.y - origin.y, dz = targetPos.z - origin.z;
    const float potentialTargetDist = std::sqrt((dz * dz + dy * dy) + dx * dx);

    const auto potentialTargetDistToLine = CCollision::DistToLine(origin, origin + aimingDir * 250.f, potentialTarget->GetPosition());
    // Como el original (0x73E61C): sqrt(dist) * 0x858B1C (0.1f), no una division entre 10
    auto ret = std::sqrt(potentialTargetDist) * 0.1f + potentialTargetDistToLine / potentialTargetDist;

    // Como el original (0x73E634..0x73E63D): fcomp + test ah,5 + jp -> -1 salvo que "dist_linea * tolerancia < dist" (con NaN tambien -1)
    if (!(potentialTargetDistToLine * tolerance < potentialTargetDist)) {
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
    CVector shotDir, shotPt;
    if (targetEntity || target) { // 0x73EA1E
        // Como el original (0x73EA68..0x73EAA4): con un ped se llama a la variante con puntero de salida (0x5E4280: out, bone 3, false)
        CVector ptTarget;
        if (target) {
            ptTarget = *target;
        } else if (targetEntity->GetIsTypePed()) {
            targetEntity->AsPed()->GetBonePosition(&ptTarget, BONE_SPINE1, false);
        } else {
            ptTarget = targetEntity->GetPosition();
        }
        shotPt  = ptTarget;
        shotDir = CVector{ ptTarget.x - origin.x, ptTarget.y - origin.y, ptTarget.z - origin.z };
        shotDir.Normalise(); // 0x73EAE8 (0x59C910)
    } else if (firingEntity == FindPlayerPed() && TheCamera.m_aCams[0].Using3rdPersonMouseCam()) { // 0x73E892
        CVector camPos, camTargetPos;
        TheCamera.Find3rdPersonCamTargetVector(wi->m_fWeaponRange, origin, camPos, camTargetPos);
        // Como el original (0x73E921 -> 0x4119D0): v * (1.0 / rango), no v / rango; el punto es camTargetPos
        const float inverseRange = 1.0f / wi->m_fWeaponRange;
        shotDir = CVector{ (camTargetPos.x - camPos.x) * inverseRange, (camTargetPos.y - camPos.y) * inverseRange, (camTargetPos.z - camPos.z) * inverseRange };
        shotPt  = camTargetPos;
    } else { // 0x73E95A
        // Como el original: el rumbo se calcula en GRADOS (fpatan * 0x859878) y se vuelve a radianes con 0x872C60 (0.017453216f,
        // que NO es pi/180); el seno/coseno son de x87 a precision completa.
        float headingDegrees;
        if (firingEntity->m_matrix) { // 0x73E965: atan2(-fwd.x, fwd.y)
            const float forwardX = firingEntity->m_matrix->GetForward().x, forwardY = firingEntity->m_matrix->GetForward().y;
            headingDegrees = WeaponHeadingDegreesCpp(forwardX, forwardY); // fpatan sin redondear por 0x859878, un solo redondeo
        } else { // 0x73E97E: heading del placeable (esi+0x10)
            headingDegrees = firingEntity->m_placement.m_fHeading * std::bit_cast<float>(0x42652EE0u);
        }

        const float headingRadians = headingDegrees * std::bit_cast<float>(0x3C8EFA0Cu); // 0x73E997 fmul [0x872C60] (leido del EXE)
        const float sinValue = -TrigSimpleSinCpp(headingRadians); // fsin; fchs; fstp
        const float cosValue = TrigSimpleCosCpp(headingRadians);
        shotDir = CVector{ sinValue, cosValue, 0.f };
        if (firingEntity->GetIsTypePed()) {
            if (const auto pd = firingEntity->AsPed()->GetPlayerData()) {
                const float pitch = pd->m_fLookPitch;
                const float negTan = -WeaponTanCpp(pitch); // fptan; fstp st(0); fchs; fstp
                shotDir.z = negTan;
                shotDir.Normalise(); // 0x73E9D8 (0x59C910): solo se normaliza dentro de esta rama
            }
        }
        shotPt = CVector{ shotDir.x + origin.x, shotDir.y + origin.y, shotDir.z + origin.z };
    }
    CShotInfo::AddShot(firingEntity, m_Type, origin, shotPt);
    DoWeaponEffect(origin, shotDir);
    // Como el original (0x73EB52..0x73EBFE): flamethrower y (rand() & 3) == 2; escala = rand * 0x858C7C * 2.5 + 3.5 (UN escalar, no un
    // vector aleatorio); posicion = dir * escala + origen, y despues z + 0.5
    if (m_Type == WEAPON_FLAMETHROWER && (CGeneral::GetRandomNumber() & 3) == 2) {
        const float scale = ((float)CGeneral::GetRandomNumber() * std::bit_cast<float>(0x38000100u)) * 2.5f + 3.5f;
        const float x = shotDir.x * scale + origin.x;
        const float y = shotDir.y * scale + origin.y;
        const float z = shotDir.z * scale + origin.z;
        if (CCreepingFire::TryToStartFireAtCoors(
            CVector{ x, y, z + 0.5f },
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
    // Como el original: el minimo inicial es 0x47C34FF3 (~99999.9), no FLT_MAX (0x73F93F); el EXE recorre el pool del ultimo al primero
    float minRating  = std::bit_cast<float>(0x47C34FF3u);
    CEntity* minRated{};
    const auto point = origin + direction * 5.f;
    auto* const vehiclePool = GetVehiclePool();
    for (size_t i = vehiclePool->GetSize(); i-- > 0;) {
        auto* const vehPtr = vehiclePool->GetAt(i);
        if (!vehPtr) {
            continue;
        }
        auto& veh = *vehPtr;
        if (&veh == ignoreEntity) {
            continue;
        }
        if (!veh.vehicleFlags.bVehicleCanBeTargettedByHS) {
            continue;
        }
        // 0x73F9B0: fcomp + test ah,41h + jne -> se descarta con salud <= 0 o NaN
        if (!(veh.m_fHealth > 0.f)) {
            continue;
        }
        const auto rating = EvaluateTargetForHeatSeekingMissile(&veh, point, direction, distanceMultiplier, arePlanesPriority, preferredExistingTarget);
        // 0x73F9DC..0x73F9F5: rating >= 0 (test ah,1) y rating < minRating ESTRICTO (test ah,5 + jp)
        if (!(rating < 0.f) && rating < minRating) {
            minRating = rating;
            minRated  = &veh;
        }
    }
    return minRated;
}

// 0x73FA20
bool CWeapon::FireFromCar(CVehicle* vehicle, bool leftSide, bool rightSide) {
    // Como el original (0x73FA23..0x73FA39): el estado debe ser READY (0) o FIRING (1); la municion del cargador se compara con signo
    if ((m_State != WEAPONSTATE_READY && m_State != WEAPONSTATE_FIRING) || (int32)m_AmmoInClip <= 0) {
        return false;
    }
    if (!CWeapon::FireInstantHitFromCar(vehicle, leftSide, rightSide)) {
        return true; // Como el original (0x73FB01/0x73FB02): mov al,1 aunque FireInstantHitFromCar falle
    }
    if (const auto d = vehicle->m_pDriver) {
        d->GetWeaponAE().AddAudioEvent(AE_WEAPON_FIRE);
    }
    if (!CCheat::IsActive(CHEAT_INFINITE_AMMO)) {
        if ((int32)m_AmmoInClip > 0) { // 0x73FA7E: test + jle (con signo)
            m_AmmoInClip--;
        }
        if (  (int32)m_TotalAmmo < 25'000 && (int32)m_TotalAmmo > 0 // 0x73FA89: cmp 61A8h + jge / jle (con signo)
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
    // (fin - inicio) * escala a 24 bits por operacion y truncado como _ftol2 (0x821B40): conversion a 64 bits y 32 bits bajos
    return static_cast<int32>(static_cast<long long>((animationEnd - animationStart) * scale));
}
static void InstantHitSwayAccumulate355(CVector* endpoint,const CVector* sineBasis,const CVector* cosineBasis,float amplitude,float sine,float cosine) {
    // 0x740103..0x7401B7: ver WeaponSwayAccumulateCpp (X87Math)
    WeaponSwayAccumulateCpp(&endpoint->x, &sineBasis->x, &cosineBasis->x, amplitude, sine, cosine);
}

void InstantHitOscillation(uint32 milliseconds, float& sine, float& cosine) {
    // 0x7400E7..0x74012F: fild del tiempo sin signo (+2^32 si el bit alto esta puesto) por 0x8D6118, redondeado a 24 bits; fsin/fcos de ese valor
    const float time = UnsignedTimesFloatCpp(milliseconds, InstantHitConstant(0x8D6118));
    sine = TrigSimpleSinCpp(time);
    cosine = TrigSimpleCosCpp(time);
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

    // Como el original: el EXE modifica `origin` EN EL SITIO (edi = puntero del llamador: 0x741434, 0x741665, 0x74169D...), asi que
    // el vector del llamador queda desplazado. Todas las lecturas posteriores usan el valor ya modificado.
    CVector& projOrigin = const_cast<CVector&>(origin);

    const auto firedByPed = firedBy->GetIsTypePed()
        ? firedBy->AsPed()
        : nullptr;
    CVector losCheckTarget;
    CVector losCheckOrigin;
    // Como el original (0x741601): proyectil = 0x13 (ROCKET) + (tipo == 0x24); el resto de armas conserva su tipo
    auto projType = GetType();
    if (notsa::contains({ WEAPON_RLAUNCHER, WEAPON_RLAUNCHER_HS }, GetType())) {
        projType = (GetType() == WEAPON_RLAUNCHER_HS) ? WEAPON_ROCKET_HS : WEAPON_ROCKET;
        losCheckOrigin = projOrigin; // 0x7415DF: copia del origen ANTES de modificarlo ([esp+14h])
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
            const auto& front = TheCamera.GetActiveCam().m_vecFront;
            projOrigin.x = projOrigin.x + front.x;
            projOrigin.y = front.y + projOrigin.y;
            projOrigin.z = front.z + projOrigin.z;
        } else {
            const CVector forward = firedBy->GetMatrix().GetForward(); // 0x741683: crea la matriz si no existe
            projOrigin.x = projOrigin.x + forward.x;
            projOrigin.y = forward.y + projOrigin.y;
            projOrigin.z = forward.z + projOrigin.z;
        }
        if (firedByPed) {
            if (firedByPed->IsPlayer()) { // 0x7416DC
                const auto pd = firedByPed->GetPlayerData();
                // Como el original (0x7416D9..0x7417BB): el objetivo HS solo se conserva si es el mismo que el ultimo y lleva > 1500 ms
                // pulsado; en cualquier otro caso se dispara un cohete normal (WEAPON_ROCKET) sin objetivo.
                CEntity* hsMissleTarget{};
                if (GetType() == WEAPON_RLAUNCHER_HS && CWeaponEffects::IsLockedOn(WEAPONEFFECTS_LOCK_ON) && pd->m_nFireHSMissilePressedTime) {
                    const auto picked = PickTargetForHeatSeekingMissile(
                        firedBy->GetPosition(),
                        firedBy->GetMatrix().GetForward(),
                        1.2f,
                        firedBy,
                        false,
                        pd->m_LastHSMissileTarget
                    );
                    if (picked && picked == pd->m_LastHSMissileTarget && CTimer::GetTimeInMS() - pd->m_nFireHSMissilePressedTime > 1500) { // 0x74178B
                        const auto ch = &gCrossHair[0];
                        // Como el original (0x74178B..0x7417AA): solo se escriben R, G y B (el alfa no se toca)
                        ch->m_color.r               = 255;
                        ch->m_color.g               = 0;
                        ch->m_color.b               = 0;
                        ch->m_fRotation             = 1.f;
                        ch->m_nTimeWhenToDeactivate = 0;
                        hsMissleTarget = picked;
                    }
                }
                if (hsMissleTarget) { // 0x7417B4
                    targetEntity = hsMissleTarget;
                } else {              // 0x7417BB
                    targetEntity = nullptr;
                    projType     = WEAPON_ROCKET;
                }
            } else { // 0x7417C7
                if (targetEntity || targetPos) {
                    // Como el original (0x7417E5..0x74188E): la direccion y el rayo parten del origen SIN desplazar (losCheckOrigin)
                    const CVector aimed = targetEntity ? targetEntity->GetPosition() : *targetPos;
                    CVector aimDir{ aimed.x - losCheckOrigin.x, aimed.y - losCheckOrigin.y, aimed.z - losCheckOrigin.z };
                    aimDir.Normalise();
                    const CVector rayEnd{ aimDir.x * 8.0f + losCheckOrigin.x, aimDir.y * 8.0f + losCheckOrigin.y, aimDir.z * 8.0f + losCheckOrigin.z };
                    CWorld::pIgnoreEntity = firedBy;
                    const auto losClear = CWorld::GetIsLineOfSightClear(
                        losCheckOrigin,
                        rayEnd,
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
        losCheckTarget = projOrigin; // 0x7418C1: copia del origen YA modificado
    } else { // 0x74139B
        const CVector forward = firedBy->GetMatrix().GetForward();
        const CVector pos = firedBy->GetPosition();
        // Como el original (0x7413C9..0x7413EA): t = (dy*fy + dz*fz) + dx*fx
        const float t = ((projOrigin.y - pos.y) * forward.y + (projOrigin.z - pos.z) * forward.z) + (projOrigin.x - pos.x) * forward.x;
        if (t < 0.3f) { // 0x7413F1: fcomp + test ah,5 + jp
            const float s = 0.3f - t;
            projOrigin.x = s * forward.x + projOrigin.x;
            projOrigin.y = s * forward.y + projOrigin.y;
            projOrigin.z = s * forward.z + projOrigin.z;
        }
        losCheckTarget = projOrigin;
        if (projOrigin.z - firedBy->GetPosition().z > 0.f) { // 0x74146F: fcomp + test ah,41h + jne
            losCheckTarget.x = forward.x * 0.6f + losCheckTarget.x;
            losCheckTarget.y = losCheckTarget.y + forward.y * 0.6f;
            losCheckTarget.z = losCheckTarget.z + forward.z * 0.6f;
        }
        // Como el original (0x7414D8..0x7415A6): v = origen - pos; d = (vz*fz + vy*fy) + vx*fx; origen_rayo = origen - f * d
        const CVector fwd2 = firedBy->GetMatrix().GetForward();
        const CVector pos2 = firedBy->GetPosition();
        const float vx = projOrigin.x - pos2.x, vy = projOrigin.y - pos2.y, vz = projOrigin.z - pos2.z;
        const float d = (vz * fwd2.z + vy * fwd2.y) + vx * fwd2.x;
        losCheckOrigin = CVector{ projOrigin.x - fwd2.x * d, projOrigin.y - fwd2.y * d, projOrigin.z - fwd2.z * d };
        // 0x741585: el objetivo es el parametro tal cual
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
        // Como el original (0x741A1B..0x741A91): con ROCKET (no HS) y objetivo OR posicion -> cohete dirigido (no AND)
        if (projType == WEAPON_ROCKET && (targetEntity || targetPos)) {
            const auto projTargetPos = targetEntity
                ? targetEntity->GetPosition()
                : *targetPos;
            CVector projDir{ projTargetPos.x - losCheckOrigin.x, projTargetPos.y - losCheckOrigin.y, projTargetPos.z - losCheckOrigin.z };
            projDir.Normalise();
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

    } else if (projType == WEAPON_GRENADE || projType == WEAPON_REMOTE_SATCHEL_CHARGE) { // 0x741905
        // Como el original: si el tirador NO es un ped no se hace nada (ni RemoveNotAdd) -> 0x74193B "jne 0x741A5B"
        if (firedBy->GetIsTypePed()) {
            const CVector firedByForward = firedBy->GetMatrix().GetForward();
            const CVector firedByPos = firedBy->GetPosition();
            const CVector thorwableProjOrigin{
                firedByPos.x - firedByForward.x,
                firedByPos.y - firedByForward.y,
                (firedByPos.z - firedByForward.z) - 0.4f
            };
            // Como el original (0x7419CE..0x7419F8): si la esfera NO choca (resultado nulo) se anade el proyectil con fuerza 0 (eax == 0);
            // si choca, RemoveNotAdd. La version anterior lo tenia invertido y pasaba `force`.
            if (!CWorld::TestSphereAgainstWorld(thorwableProjOrigin, 0.3f, nullptr, false, false, true, false, false, false)) {
                CProjectileInfo::AddProjectile(
                    firedBy,
                    projType,
                    thorwableProjOrigin,
                    0.0f,
                    nullptr,
                    targetEntity
                );
            } else {
                CProjectileInfo::RemoveNotAdd(firedBy, projType, projOrigin);
            }
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

    // Como el original (0x741B2B..0x741BC6): evento construido una vez y anadido con Add(&evento, false)
    CEventGunShot gunShotEvent{
        firedBy,
        projOrigin,
        targetEntity
            ? targetEntity->GetPosition()
            : targetPos
                ? *targetPos
                : projOrigin,
        notsa::contains({WEAPON_PISTOL_SILENCED, WEAPON_TEARGAS}, GetType())
    };
    GetEventGlobalGroup()->Add(&gunShotEvent, false);

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
    CWorld::pIgnoreEntity    = owner; // Como el original: 0x741CC7 mov [0xB7CD68],edi

    // Como el original (0x741C9D..0x741D3B): destino = origen + (frente * 3.0) * alcance del arma (no solo frente * 3)
    const auto camOriginPos = cam->m_vecSource;
    const float range = wi->m_fWeaponRange;
    const float frontX3 = cam->m_vecFront.x * 3.0f, frontY3 = cam->m_vecFront.y * 3.0f, frontZ3 = cam->m_vecFront.z * 3.0f;
    const CVector camTargetPos{ frontX3 * range + camOriginPos.x, camOriginPos.y + frontY3 * range, camOriginPos.z + frontZ3 * range };

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
    // Como el original (0x741DC6..0x741E16): distancia 2D = sqrt(dy*dy + dx*dx); se descarta la entidad si multiplicador*alcance < distancia
    if (shotHitEntity) {
        const float dx = shotCP.m_vecPoint.x - camOriginPos.x, dy = shotCP.m_vecPoint.y - camOriginPos.y;
        const float dist2D = std::sqrt(dy * dy + dx * dx);
        if (TargetWeaponRangeMultiplier(shotHitEntity, owner) * range < dist2D) {
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
        // Como el original (0x741EA9..0x741F03): (rand() & 0x7F) - 0x40 como entero, no GetRandomNumberInRange(-64, 64)
        cam->m_fHorizontalAngle += (float)((int32)(CGeneral::GetRandomNumber() & 0x7F) - 0x40) * intensity;
        cam->m_fVerticalAngle += (float)((int32)(CGeneral::GetRandomNumber() & 0x7F) - 0x40) * intensity;

        // Do pad shaking
        // Como el original (0x741F05..0x741F8A): (fin - inicio) * 900 se trunca a ENTERO (_ftol2); t = (20 - n) * 0.0125 con x87 a 24 bits
        // (cada operacion se redondea a float), limitado a [0,1]; frecuencia = (uint8)_ftol2(t * 80 + 130).
        const int32 loopLength = (int32)(int64)((wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * 900.0f);
        float t = (float)((double)(float)(20 - loopLength) * 0.0125);
        if (t < 0.0f) {
            t = 0.0f;
        } else if (1.0f < t) {
            t = 1.0f;
        }
        const float scaled = (float)((double)t * 80.0);
        const uint8 shakeFreq = (uint8)(int64)(float)((double)scaled + 130.0);
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

        // Como el original (0x7423CD..0x74240F): el rumbo sale de atan2(-fwd.x, fwd.y) (fpatan) o del heading del placeable, se
        // multiplica por 57.2957764 (0x859878) y luego por 0.017453216 (0x872C60) y el seno/coseno son de x87 a precision completa.
        float angle;
        const float radiansScale = std::bit_cast<float>(0x3C8EFA0Cu);
        if (firedBy->m_matrix) {
            const float forwardX = firedBy->m_matrix->GetForward().x, forwardY = firedBy->m_matrix->GetForward().y;
            angle = WeaponHeadingDegreesCpp(forwardX, forwardY) * radiansScale; // fpatan * 0x859878 (un redondeo) * 0x872C60 (otro)
        } else {
            const float heading = firedBy->m_placement.m_fHeading;
            angle = (heading * std::bit_cast<float>(0x42652EE0u)) * radiansScale;
        }
        const float sinPart = TrigSimpleSinMulCpp(angle, r); // fsin sin redondear por 0.15 y un redondeo
        const float cosPart = TrigSimpleCosMulCpp(angle, r);
        fxPos->x = fxPos->x - sinPart;
        fxPos->y = cosPart + fxPos->y;
    }

    switch (m_State) {
    case WEAPONSTATE_READY:
    case WEAPONSTATE_FIRING:
        break;
    default:
        return false;
    }

    // Como el original (0x742429..0x742459): comparaciones CON signo, el tamano del cargador sale de GetWeaponInfo(tipo, STD) (nivel 1,
    // no el nivel del tirador) y se extiende con signo (movsx)
    if ((int32)m_AmmoInClip <= 0) {
        if ((int32)m_TotalAmmo <= 0) {
            return false;
        }
        const uint32 stdAmmoClip = (uint32)(int32)(int16)CWeaponInfo::GetWeaponInfo(m_Type, eWeaponSkill::STD)->m_nAmmoClip;
        m_AmmoInClip = std::min<uint32>(m_TotalAmmo, stdAmmoClip);
    }

    const auto [hasFired, delayNextShot] = [&]() -> std::pair<bool, bool> {
        switch (m_Type) {
        case WEAPON_GRENADE:
        case WEAPON_TEARGAS:
        case WEAPON_MOLOTOV:
        case WEAPON_REMOTE_SATCHEL_CHARGE: { // 0x74268B
            if (targetPosn) {
                // Como el original (0x7426AD..0x7426F6): (|objetivo - tirador| - 10) * 0.1 (0x858B1C, no una division), limitado a [0.2, 1.0]
                const CVector shooterPos = firedBy->GetPosition();
                const CVector toTarget{ targetPosn->x - shooterPos.x, targetPosn->y - shooterPos.y, targetPosn->z - shooterPos.z };
                const float distance = toTarget.Magnitude();
                float force = (distance - 10.0f) * 0.1f;
                if (force < 0.2f) {
                    force = 0.2f;
                } else if (1.0f < force) {
                    force = 1.0f;
                }
                return {
                    FireProjectile( // 0x742705
                        firedBy,
                        *shotOrigin,
                        targetEnt,
                        targetPosn,
                        force
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
                // Como el original (0x742571..0x74259C): se consulta GetActiveTask DOS veces; si el tipo es GANG_DRIVEBY (0x3FE)
                // se anula el retardo (0x7425A2 mov [esp+13h],0), si no (o sin tarea) el retardo se mantiene
                if (firedByPed->GetTaskManager().GetActiveTask()) {
                    return { fired, firedByPed->GetTaskManager().GetActiveTask()->GetTaskType() != TASK_SIMPLE_GANG_DRIVEBY };
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
            // Como el original (0x7425AC..0x742667): un ped con objetivo solo dispara si |objetivo - tirador| > 8 (0x859000) o es el
            // jugador; si no, no dispara ni ejecuta el procesado posterior (salta a 0x7429F2).
            if (firedBy->GetIsTypePed()) {
                const CVector* targetPoint = nullptr;
                CVector targetEntPos;
                if (targetEnt) {
                    targetEntPos = targetEnt->GetPosition();
                    targetPoint = &targetEntPos;
                } else if (targetPosn) {
                    targetPoint = targetPosn;
                }
                if (targetPoint) {
                    const CVector shooterPos = firedBy->GetPosition();
                    const CVector toTarget{ targetPoint->x - shooterPos.x, targetPoint->y - shooterPos.y, targetPoint->z - shooterPos.z };
                    const float distance = toTarget.Magnitude();
                    if (!(distance > 8.0f) && !firedBy->AsPed()->IsPlayer()) {
                        return { false, true };
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
                false // Como el original: 0x742768 mov byte ptr [esp+13h],0 (sin retraso para el siguiente disparo)
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
            // Como el original (0x742794): "mov bl,byte ptr [esp+60h]" -> dispara si el byte bajo del puntero targetPosn no es 0
            return { (reinterpret_cast<uintptr_t>(targetPosn) & 0xFF) != 0, true };
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
            if ((int32)firedByPed->GetWeapon(WEAPON_REMOTE_SATCHEL_CHARGE).m_TotalAmmo <= 1) { // 0x74283D: cmp ,1 + jg (con signo)
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
            if ((int32)m_AmmoInClip > 0) {
                m_AmmoInClip--;
            }
            if ((int32)m_TotalAmmo > 0) {
                // Como el original (0x7428BA..0x7428E6): para el jugador se consulta primero el progreso (GetPercentageProgress) y despues
                // el detonador; para el resto basta con total < 25000
                bool decrement;
                if (isPlayerFiring) {
                    const bool progressBelow100 = CStats::GetPercentageProgress() < 100.f;
                    decrement = progressBelow100 || m_Type == WEAPON_DETONATOR;
                } else {
                    decrement = (int32)m_TotalAmmo < 25'000;
                }
                if (decrement) {
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
                        ? (uint32)((int32)wi->GetWeaponReloadTime() / 4)
                        : wi->GetWeaponReloadTime();
                m_TimeForNextShotMs += CTimer::GetTimeInMS();
            } else if (TheCamera.GetActiveCam().m_nMode == MODE_CAMERA) {
                CPad::GetPad()->Clear(false, true);
            }
            return true;
        }

        // Como el original (0x7429AB..0x7429E7): tiempo = ahora - _ftol2((fin - inicio) * -900.0) (0x86C2FC), 1100 para la camara
        m_TimeForNextShotMs = s_DebugSettings.NoShotDelay
            ? 0
            : delayNextShot
                ? m_Type == WEAPON_CAMERA
                    ? 1100
                    : (uint32)(0 - (uint32)(int64)((wi->m_fAnimLoopEnd - wi->m_fAnimLoopStart) * -900.0f))
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
                auto* const hitAnim = CAnimManager::AddAnimation(
                    hitPed->GetRpClump(),
                    ANIM_GROUP_DEFAULT,
                    (AnimationId)(ANIM_ID_SHOT_PARTIAL + pedHitDir) // Como el original: 0x73B04D lea eax,[edi+1Ch]
                );
                // Como el original (0x73B055..0x73B05C): BlendAmount = 0 y BlendDelta = 8.0 (41000000h)
                hitAnim->m_BlendAmount = 0.f;
                hitAnim->m_BlendDelta = 8.0f;
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

        // Como el original (0x73B0C4..0x73B143): end - start normalizado (0x59C910) y producto escalar sumado en orden z, y, x;
        // asin con el CRT de precision completa (0x821E70) y multiplicado por 57.29578 (0x859878 = 0x42652EE0).
        CVector incidence = endPoint - startPoint;
        incidence.Normalise();
        const float angleOfIncidenceCos = (hitCP.m_vecNormal.z * incidence.z + hitCP.m_vecNormal.y * incidence.y) + hitCP.m_vecNormal.x * incidence.x;
        if (angleOfIncidenceCos < 0.f) {
            AudioEngine.ReportBulletHit(
                hitEntity,
                hitCP.m_nSurfaceTypeB,
                hitCP.m_vecPoint,
                AsinDegreesX87(-angleOfIncidenceCos)
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
