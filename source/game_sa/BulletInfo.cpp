/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBulletInfo
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "BulletInfo.h"

#include "FireManager.h"
#include "AudioEngine.h"
#include "Glass.h"

// Note: This class is only used by CWeapon::FireSniper

void CBulletInfo::InjectHooks() {
    RH_ScopedClass(CBulletInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x735FD0);
    RH_ScopedInstall(Shutdown, 0x736000);
    RH_ScopedInstall(AddBullet, 0x736010);
    RH_ScopedInstall(Update, 0x7360D0);
}

// 0x735FD0
void CBulletInfo::Initialise() {
    for (auto& info : aBulletInfos) {
        info.m_bExists = 0;
        info.m_nWeaponType = WEAPON_PISTOL;
        info.m_nDestroyTime = 0.0f;
        info.m_pCreator = 0;
    }
}

// 0x736000
void CBulletInfo::Shutdown() {
    // NOP
}

CBulletInfo* CBulletInfo::GetFree() {
    for (auto& info : aBulletInfos) {
        if (!info.m_bExists) {
            return &info;
        }
    }
    return nullptr;
}

// Como el original: fild del tiempo (exacto) y, si es negativo como int32, se le suma 2^32
// con el FPU a 24 bits; la comparación es contra el valor exacto, no contra un float redondeado.
bool CBulletInfo::IsTimeToBeDestroyed() const noexcept {
    const auto timeMs = (int32)CTimer::GetTimeInMS();
    double now = (double)timeMs;
    if (timeMs < 0) {
        now = (double)(float)(now + 4294967296.0);
    }
    return now > (double)m_nDestroyTime;
}

// asin del CRT del juego (0x821E70): fpatan(x, sqrt((1 + x)(1 - x))) con la palabra de control
// (anterior & 0x300) | 0x7F (0x828F05), así que fadd/fsub/fmul/fsqrt redondean como el juego; después
// el llamador multiplica en el FPU por 57.29578f (0x859878 = 0x42652EE0, no es el float más cercano a 180/PI).
static const float s_RadToDegBullet = std::bit_cast<float>(0x42652EE0u);
static float __declspec(naked) __cdecl AsinDegreesX87(float x) {
    __asm {
        sub    esp, 4
        fnstcw word ptr [esp]
        mov    ax, word ptr [esp]
        and    ax, 300h
        or     ax, 7Fh
        mov    word ptr [esp + 2], ax
        fldcw  word ptr [esp + 2]
        fld    dword ptr [esp + 8]
        fld1
        fadd   st, st(1)
        fld1
        fsub   st, st(2)
        fmulp  st(1), st
        fsqrt
        fpatan
        fldcw  word ptr [esp]
        fmul   dword ptr [s_RadToDegBullet]
        add    esp, 4
        ret
    }
}

// 0x736010
bool CBulletInfo::AddBullet(CEntity* creator, eWeaponType weaponType, CVector posn, CVector velocity) {
    auto info = GetFree();
    if (!info) {
        return false;
    }
    info->m_pCreator     = creator;
    info->m_nWeaponType  = weaponType;
    info->m_nDamage      = CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nDamage;
    info->m_vecPosition  = posn;
    info->m_vecVelocity  = velocity;
    // Como el original: fild de (tiempo + 1000) como int32 con signo
    info->m_nDestroyTime = (float)(int32)(CTimer::GetTimeInMS() + 1000);
    info->m_bExists      = true;
    return true;
}

// 0x7360D0
void CBulletInfo::Update() {
    // Como el original: la entidad alcanzada vive fuera del bucle (ProcessLineOfSight la pone a nulo)
    CEntity* hitEntity = nullptr;
    for (auto& info : aBulletInfos) {
        // Como el original: un creador peatón que ya no es válido se olvida
        if (info.m_pCreator && info.m_pCreator->GetIsTypePed() && !info.m_pCreator->AsPed()->IsPointerValid()) {
            info.m_pCreator = nullptr;
        }
        if (!info.m_bExists)
            continue;

        // Como el original: al caducar se marca como inexistente pero se procesa este frame
        if (info.IsTimeToBeDestroyed())
            info.m_bExists = false;

        CVector newPosition = info.m_vecPosition + info.m_vecVelocity * (CTimer::GetTimeStep() * 0.5f);
        if (!CWorld::IsInWorldBounds(newPosition)) {
            info.m_bExists = false;
            continue;
        }
        const CVector origin = info.m_vecPosition;

        CWorld::pIgnoreEntity    = info.m_pCreator;
        CWorld::bIncludeDeadPeds = true;
        CWorld::bIncludeCarTyres = true;
        CWorld::bIncludeBikers   = true;

        CColPoint colPoint;
        if (CWorld::ProcessLineOfSight(origin, newPosition, colPoint, hitEntity, true, true, true, true, true, false, false, true)) {
            CWeapon::CheckForShootingVehicleOccupant(&hitEntity, &colPoint, info.m_nWeaponType, origin, newPosition);

            switch (hitEntity->GetType()) {
            case ENTITY_TYPE_PED: {
                auto hitPed = hitEntity->AsPed();

                if (hitPed->m_nPedState != PEDSTATE_DIE && hitPed->m_nPedState != PEDSTATE_DEAD && hitEntity != info.m_pCreator) {
                    CWeapon::GenerateDamageEvent(
                        hitPed,
                        info.m_pCreator,
                        info.m_nWeaponType,
                        info.m_nDamage,
                        (ePedPieceTypes)colPoint.m_nPieceTypeB,
                        hitPed->GetLocalDirection(CVector2D{ hitPed->GetPosition() - colPoint.m_vecPoint })
                    );
                    CCrime::ReportCrime(
                        (hitPed->m_nPedType == PED_TYPE_COP) ? eCrimeType::CRIME_DAMAGE_COP_CAR : eCrimeType::CRIME_DAMAGE_CAR,
                        hitPed,
                        info.m_pCreator->AsPed()
                    );
                    newPosition = colPoint.m_vecPoint;
                }

                if (CLocalisation::Blood()) {
                    g_fx.AddBlood(colPoint.m_vecPoint, colPoint.m_vecNormal, 8, hitPed->m_fContactSurfaceBrightness);
                    if (hitPed->m_nPedState == PEDSTATE_DEAD) {
                        const auto anim = RpAnimBlendClumpGetFirstAssociation(hitPed->GetRpClump(), ANIMATION_IS_FRONT) ? ANIM_ID_FLOOR_HIT_F : ANIM_ID_FLOOR_HIT;

                        if (auto assoc = CAnimManager::BlendAnimation(hitPed->GetRpClump(), ANIM_GROUP_DEFAULT, anim, 8.0f)) {
                            assoc->SetCurrentTime(0.0f);
                            // Como el original: quita FINISH_AUTO_REMOVE y activa IS_PLAYING
                            assoc->SetFlag(ANIMATION_IS_FINISH_AUTO_REMOVE, false);
                            assoc->SetFlag(ANIMATION_IS_PLAYING, true);
                        }
                    }
                    newPosition = colPoint.m_vecPoint;
                }
                break;
            }
            case ENTITY_TYPE_VEHICLE: {
                auto hitVehicle = hitEntity->AsVehicle();

                if (info.m_pCreator && info.m_pCreator->GetIsTypePed())
                    if (info.m_pCreator->AsPed()->m_pAttachedTo == hitVehicle)
                        break;

                // Originally: if (colPoint.m_nPieceTypeB < 13u || colPoint.m_nPieceTypeB > 16u)
                switch ((eCarPiece)colPoint.m_nPieceTypeB) {
                default: /* originally `if` body */
                {
                    hitVehicle->InflictDamage(info.m_pCreator, info.m_nWeaponType, (float)info.m_nDamage, colPoint.m_vecPoint);
                    if (info.m_nWeaponType == eWeaponType::WEAPON_FLAMETHROWER) {
                        gFireManager.StartFire(hitVehicle, info.m_pCreator, 0.8f, true, 7000, 100);
                    } else if (TheCamera.IsSphereVisible(colPoint.m_vecPoint, 1.0f)) { // Como el original: la esfera se centra en el punto
                        g_fx.AddBulletImpact(colPoint.m_vecPoint, colPoint.m_vecNormal, colPoint.m_nSurfaceTypeB, 8, colPoint.m_nLightingB.GetCurrentLighting());
                    }
                    break;
                }
                case eCarPiece::CAR_PIECE_WHEEL_LF:
                case eCarPiece::CAR_PIECE_WHEEL_RF:
                case eCarPiece::CAR_PIECE_WHEEL_RL:
                case eCarPiece::CAR_PIECE_WHEEL_RR: { /* originally `else` body */
                    hitVehicle->BurstTyre(colPoint.m_nPieceTypeB, true);
                    g_fx.AddTyreBurst(colPoint.m_vecPoint, colPoint.m_vecNormal);
                    break;
                }
                }

                break;
            }
            default: {
                if (TheCamera.IsSphereVisible(colPoint.m_vecPoint, 1.0f)) // Como el original: la esfera se centra en el punto
                    g_fx.AddBulletImpact(colPoint.m_vecPoint, colPoint.m_vecNormal, colPoint.m_nSurfaceTypeB, 8, colPoint.m_nLightingB.GetCurrentLighting());

                if (info.m_pCreator && info.m_pCreator->GetIsTypePed())
                    if (info.m_pCreator->AsPed()->m_pAttachedTo == hitEntity)
                        break;

                switch (hitEntity->GetType()) {
                case ENTITY_TYPE_OBJECT: {
                    auto hitObject = hitEntity->AsObject();

                    const auto DoDamageToObject = [&](float dmg) {
                        hitObject->ObjectDamage(dmg, &colPoint.m_vecPoint, &colPoint.m_vecNormal, info.m_pCreator, info.m_nWeaponType);
                    };

                    if (hitObject->m_nColDamageEffect < 200u) {
                        // Como el original: un multiplicador NaN también se salta el empuje
                        if (hitObject->physicalFlags.bDisableCollisionForce || !(hitObject->m_pObjectInfo->m_fColDamageMultiplier < 99.9f)) {
                            /* empty */
                        } else {
                            if (hitObject->GetIsStatic() && hitObject->m_pObjectInfo->m_fUprootLimit <= 0.0f) {
                                hitObject->SetIsStatic(false);
                                hitObject->AddToMovingList();
                            }
                            if (!hitObject->GetIsStatic()) {
                                hitObject->ApplyMoveForce(colPoint.m_vecNormal * -7.5f);
                            }
                        }
                    } else {
                        switch (hitObject->m_pObjectInfo->m_nGunBreakMode) {
                        case eObjectBreakMode::BY_GUN: {
                            DoDamageToObject(151.0f);
                            break;
                        }
                        case eObjectBreakMode::SMASHABLE: {
                            DoDamageToObject(hitObject->m_pObjectInfo->m_fSmashMultiplier * 151.0f);
                            break;
                        }
                        }
                    }
                    DoDamageToObject(50.0f);
                    break;
                }
                case ENTITY_TYPE_BUILDING: {
                    if (info.m_pCreator && info.m_pCreator->GetIsTypePed()) {
                        if (auto playerData = info.m_pCreator->AsPed()->GetPlayerData()) {
                            // Como el original: el indice de modelo se extiende con signo (movsx)
                            playerData->m_nModelIndexOfLastBuildingShot = (uint32)(int32)(int16)hitEntity->GetModelIndex();
                        }
                    }
                    break;
                }
                }
            }
            }

            if (info.m_nWeaponType == eWeaponType::WEAPON_SNIPERRIFLE) {
                CVector dir = newPosition - origin;
                dir.Normalise();
                // Como el original: suma x + z + y
                const auto& n = colPoint.m_vecNormal;
                const float dirDotColPointNorm = n.x * dir.x + n.z * dir.z + n.y * dir.y;
                if (dirDotColPointNorm < 0.0f) {
                    const float angle = AsinDegreesX87(-dirDotColPointNorm);
                    AudioEngine.ReportBulletHit(hitEntity, colPoint.m_nSurfaceTypeB, colPoint.m_vecPoint, angle);
                }
            }
            CGlass::WasGlassHitByBullet(hitEntity, colPoint.m_vecPoint);
        }

        CWorld::bIncludeDeadPeds = false;
        CWorld::bIncludeCarTyres = false;
        CWorld::bIncludeBikers   = false;
        CWorld::pIgnoreEntity    = nullptr;

        if (info.m_nWeaponType == eWeaponType::WEAPON_SNIPERRIFLE) {
            PlayerSniperBulletStart = info.m_vecPosition;
            PlayerSniperBulletEnd = newPosition;
            CHeli::TestSniperCollision(&PlayerSniperBulletStart, &PlayerSniperBulletEnd);
        }

        info.m_vecPosition = newPosition;
        // Como el original: la bala desaparece si alcanzó algo
        if (hitEntity) {
            info.m_bExists = false;
        }
    }
}
