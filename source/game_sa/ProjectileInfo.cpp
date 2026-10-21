/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CProjectileInfo
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

#include "ProjectileInfo.h"
#include "X87Math.h"

#include "Entity/Object/Projectile.h"
#include "Radar.h"
#include "World.h"
#include "Pools/Pools.h"
#include "Collision/Box.h"
#include "Fx/FxPrtMult.h"
#include "Fx/FxSystem.h"
#include "AudioEngine.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Camera.h"
#include "Streaming.h"
#include "MemoryMgr.h"
#include "Fx/FxManager.h"

void CProjectileInfo::InjectHooks() {
    RH_ScopedClass(CProjectileInfo);
    RH_ScopedCategoryGlobal();

    // Install("CProjectileInfo", "", , &CProjectileInfo::);
    RH_ScopedInstall(Initialise, 0x737B40);
    RH_ScopedInstall(Shutdown, 0x737BC0);
    RH_ScopedInstall(GetProjectileInfo, 0x737BF0);
    RH_ScopedInstall(RemoveNotAdd, 0x737C00);
    RH_ScopedInstall(AddProjectile, 0x737C80);
    RH_ScopedInstall(RemoveDetonatorProjectiles, 0x738860);
    RH_ScopedInstall(RemoveProjectile, 0x7388F0);
    RH_ScopedInstall(Update, 0x738B20);
    RH_ScopedInstall(IsProjectileInRange, 0x739860);
    RH_ScopedInstall(RemoveAllProjectiles, 0x7399B0);
    RH_ScopedInstall(RemoveIfThisIsAProjectile, 0x739A40);
    RH_ScopedInstall(RemoveFXSystem, 0x737B80);
}

// 0x737B40
void CProjectileInfo::Initialise() {
    ms_apProjectile.fill(nullptr);
    for (auto& info : gaProjectileInfo) {
        info.m_nWeaponType = WEAPON_GRENADE;
        info.m_pCreator = nullptr;
        info.m_nDestroyTime = 0;
        info.m_bActive = false;
        info.m_pFxSystem = nullptr;
    }
}

// 0x737BC0
void CProjectileInfo::Shutdown() {
    for (auto& info : gaProjectileInfo) {
        info.RemoveFXSystem(true);
    }
}

// 0x737BF0
CProjectileInfo* CProjectileInfo::GetProjectileInfo(int32 infoId) {
    return gaProjectileInfo.data() + infoId;
}

// 0x737C00
void CProjectileInfo::RemoveNotAdd(CEntity* creator, eWeaponType weaponType, CVector pos) {
    eExplosionType type;
    switch (weaponType) {
    case WEAPON_GRENADE:
    case WEAPON_REMOTE_SATCHEL_CHARGE: type = EXPLOSION_GRENADE; break;
    case WEAPON_MOLOTOV: type = EXPLOSION_MOLOTOV; break;
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS: type = EXPLOSION_ROCKET; break;
    default: return;
    }
    CExplosion::AddExplosion(nullptr, creator, type, pos, 0, 1, -1.0f, 0);
}

// 0x737C80
static float ProjectileCreatorHeading(CEntity* creator) {
    if (!creator->m_matrix) return *reinterpret_cast<float*>(reinterpret_cast<uint8*>(creator) + 0x10);
    const auto forward = creator->GetMatrix().GetForward();
    // fld y; fld x; fchs; fpatan -> atan2(-x, y), guardado como float (0x737D32).
    const float heading = TrigAtan2Cpp(-forward.x, forward.y);
    return heading;
}
static CVector ProjectileThrownVelocity(float heading, float speed, float height) {
    const float negative = StaticRef<float>(0x858C1C);
    // fsin; fmul speed; fmul negative; fstp x. fcos; fmul speed; fstp y (0x737DA6).
    const float outputX = TrigSimpleSinMulCpp(heading, speed) * negative;
    const float outputY = TrigSimpleCosMulCpp(heading, speed);
    return CVector{outputX, outputY, height};
}
bool CProjectileInfo::AddProjectile(CEntity* creator, eWeaponType projectileType, CVector origin, float force, const CVector* dir, CEntity* target) {
    const uint32 type = uint32(projectileType);
    if (type != 16 && type != 17 && type != 18 && type != 19 && type != 20 && type != 21 && type != 39 && type != 58) return false;
    CMatrix matrix; CVector velocity;
    uint32 destroyTime; bool gravity = true; uint8 objectInfo = 0; float elasticity = .75f;
    const auto constant = [](uintptr_t address) { return StaticRef<float>(address); };
    if (type == 16 || type == 17 || type == 18 || type == 39) {
        destroyTime = CTimer::GetTimeInMS() + (type == 17 ? 20000u : 2000u);
        float speed = force == 0.f && type != 18 ? 0.f : force * constant(0x8717AC) + constant(0x858FCC);
        if (type == 39) speed *= constant(0x858B8C);
        if (type == 18 && speed < constant(0x858CC4)) speed = constant(0x858CC4);
        float heading = ProjectileCreatorHeading(creator);
        if ((type == 16 || type == 39) && creator->GetIsTypeVehicle()) heading = CGeneral::LimitRadianAngle(heading + constant(0x858CB8));
        matrix.SetTranslate(CVector{0.f, 0.f, 0.f}); matrix.RotateZ(heading); matrix.GetPosition() += origin;
        const float height = type == 18 ? (force * constant(0x858CC4) + constant(0x858EE8)) * speed : (force + 1.f) * constant(0x858EE8) * speed;
        velocity = ProjectileThrownVelocity(heading, speed, height);
        if ((type == 16 || type == 39) && creator->m_nModelIndex == 405) velocity += creator->AsPhysical()->m_vecMoveSpeed;
        if (type != 18) { objectInfo = 5; elasticity = type == 39 ? std::bit_cast<float>(0x3CF5C28Fu) : .5f; }
    } else if (type == 19 || type == 20) {
        destroyTime = CTimer::GetTimeInMS() + (type == 19 ? 3000u : 10000u);
        float speed = type == 19 ? .4f : .2f;
        if (creator->GetIsTypeVehicle()) {
            matrix = creator->GetMatrix(); matrix.GetPosition() = origin;
            speed = creator->AsPhysical()->m_vecMoveSpeed.Magnitude() + speed;
        } else if (creator->GetIsTypePed() && creator->AsPed()->IsPlayer()) {
            auto& camera = TheCamera.GetActiveCam(); matrix.GetForward() = camera.m_vecFront; matrix.GetUp() = camera.m_vecUp;
            matrix.GetRight() = CrossProduct(camera.m_vecUp, camera.m_vecFront); matrix.GetPosition() = origin;
        } else if (!dir) {
            matrix = creator->GetMatrix();
        } else {
            matrix.GetForward() = *dir; matrix.GetRight() = creator->GetMatrix().GetRight();
            matrix.GetUp() = CrossProduct(matrix.GetRight(), matrix.GetForward()); matrix.GetPosition() = origin;
        }
        velocity = matrix.TransformVector(CVector{0.f, speed, 0.f}); gravity = false;
    } else {
        if (type == 58) CStreaming::RequestModel(StaticRef<int32>(0x8CD75C), 0);
        destroyTime = CTimer::GetTimeInMS() + (type == 58 ? 10000u : 2000000u);
        const auto entityType = creator->GetType();
        velocity = entityType >= ENTITY_TYPE_VEHICLE && entityType <= ENTITY_TYPE_OBJECT ? creator->AsPhysical()->m_vecMoveSpeed : CVector{0.f, 0.f, 0.f};
        matrix = creator->GetMatrix(); matrix.GetPosition() = origin;
    }
    size_t index = 0; while (index < MAX_PROJECTILES && gaProjectileInfo[index].m_bActive) ++index;
    if (index == MAX_PROJECTILES) return false;
    void* memory = CObject::operator new(sizeof(CProjectile)); CProjectile* projectile = nullptr;
    if (memory) {
        const int32 model = type == 58 ? StaticRef<int32>(0x8CD75C) : CWeaponInfo::GetWeaponInfo(projectileType, static_cast<eWeaponSkill>(1))->m_nModelId1;
        // Como el original: el pool devolvio memoria sin comprobar el constructor (CProjectile::CProjectile, 0x5A4030, ya en C++).
        projectile = ::new(memory) CProjectile(model);
    }
    ms_apProjectile[index] = projectile;
    if (!projectile) return false;
    if (type == 16 || type == 17 || type == 18 || type == 39) {
        auto* collision = CModelInfo::GetModelInfo(static_cast<int16>(projectile->m_nModelIndex))->GetColModel();
        auto* data = collision->m_pColData;
        if (!data) {
            collision->AllocateData(1, 0, 0, 0, 0, false);
            collision->m_pColData->m_pSpheres[0].Set(collision->m_boundSphere.m_fRadius * constant(0x858F34), collision->m_boundSphere.m_vecCenter, static_cast<eSurfaceType>(56), 0, tColLighting{0xFF});
        } else if (!data->m_nNumSpheres && !data->m_pSpheres) {
            data->m_nNumSpheres = 1; data->m_pSpheres = static_cast<CColSphere*>(CMemoryMgr::Malloc(20, 0));
            data->m_pSpheres[0].Set(collision->m_boundSphere.m_fRadius * constant(0x858F34), collision->m_boundSphere.m_vecCenter, static_cast<eSurfaceType>(56), 0, tColLighting{0xFF});
        }
    }
    if (type == 58) projectile->m_fAirResistance = .9f;
    auto& info = gaProjectileInfo[index]; info.m_nWeaponType = type; info.m_pCreator = creator;
    creator->RegisterReference(&info.m_pCreator); projectile->SetMatrix(matrix); projectile->m_vecMoveSpeed = velocity;
    projectile->physicalFlags.bApplyGravity = gravity; info.m_nDestroyTime = int32(destroyTime); projectile->m_fElasticity = elasticity;
    if (objectInfo == 5) *reinterpret_cast<void**>(reinterpret_cast<uint8*>(projectile) + 0x160) = reinterpret_cast<void*>(0xBB4BD0);
    info.m_pVictim = target; if (target) target->RegisterReference(&info.m_pVictim);
    info.m_bActive = true; CWorld::Add(projectile); projectile->RegisterReference(reinterpret_cast<CEntity**>(&ms_apProjectile[index])); info.m_vecLastPosn = projectile->GetPosition();
    if (type == 17) {
        if (auto* modelling = projectile->GetModellingMatrix()) {
            info.m_pFxSystem = g_fxMan.CreateFxSystem("teargasAD", CVector{0.f, 0.f, 0.f}, modelling, false);
            if (info.m_pFxSystem) info.m_pFxSystem->Play();
        }
    }
    *reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(projectile) + 0x128) = creator;
    projectile->physicalFlags.bCanBeCollidedWith = true;
    if (creator) {
        const auto entityType = creator->GetType(); auto*& damage = *reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(creator) + 0x128);
        if ((entityType == ENTITY_TYPE_VEHICLE || entityType == ENTITY_TYPE_PED || entityType == ENTITY_TYPE_OBJECT) && !damage) damage = creator;
    }
    if (type == 20) {
        const auto blip = CRadar::SetEntityBlip(static_cast<eBlipType>(3), GetObjectPool()->GetRef(projectile), 0xFF0000FF, static_cast<eBlipDisplay>(2));
        CRadar::ChangeBlipScale(blip, 1);
        const bool player = creator == FindPlayerPed(-1) || creator == FindPlayerVehicle(-1, false);
        CRadar::ChangeBlipColour(blip, static_cast<eBlipColour>(player ? 0xFFFFFFFF : 0xFF0000FF));
    }
    AudioEngine.ReportWeaponEvent(148, projectileType, projectile);
    return true;
}

// 0x738860
void CProjectileInfo::RemoveDetonatorProjectiles() {
    for (size_t i = 0; i < MAX_PROJECTILES; ++i) {
        auto& info = gaProjectileInfo[i];
        if (!info.m_bActive || info.m_nWeaponType != WEAPON_REMOTE_SATCHEL_CHARGE) {
            continue;
        }
        auto* projectile = ms_apProjectile[i];
        CExplosion::AddExplosion(nullptr, info.m_pCreator, EXPLOSION_GRENADE, projectile->GetPosition(), 0, 1, -1.0f, 0);
        info.m_bActive = false;
        info.RemoveFXSystem(false);
        projectile->m_bRemoveFromWorld = true;
    }
}

// 0x7388F0
void CProjectileInfo::RemoveProjectile(CProjectileInfo* info, CProjectile* object) {
    auto* creator = info->m_pCreator;
    eExplosionType type = EXPLOSION_UNDEFINED;
    switch (info->m_nWeaponType) {
    case WEAPON_GRENADE:
    case WEAPON_FREEFALL_BOMB: type = EXPLOSION_GRENADE; break;
    case WEAPON_MOLOTOV: type = EXPLOSION_MOLOTOV; break;
    case WEAPON_ROCKET:
        type = EXPLOSION_ROCKET;
        if (creator && creator->GetIsTypeVehicle()) creator = creator->AsVehicle()->m_pDriver;
        break;
    case WEAPON_ROCKET_HS:
        type = creator == FindPlayerPed() ? EXPLOSION_ROCKET : EXPLOSION_WEAK_ROCKET;
        break;
    }
    if (type != EXPLOSION_UNDEFINED) {
        CExplosion::AddExplosion(nullptr, creator, type, object->GetPosition(), 0, 1, -1.0f, 0);
        if (info->m_nWeaponType == WEAPON_MOLOTOV) AudioEngine.ReportObjectDestruction(object);
    }
    info->m_bActive = false;
    info->RemoveFXSystem(false);
    CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(object));
    CWorld::Remove(object);
    delete object;
}

// 0x738B20
namespace {
template<typename T> T& ProjectileField(CEntity* entity, size_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(entity) + offset);
}
float ProjectileDamping(float timestep) {
    const float base = StaticRef<float>(0x8D6104);
    // fld base; fld timestep; call _CIpow (0x822130); fstp result (0x7395CF).
    const float result = float(ExeCompatiblePowCpp(double(base), double(timestep)));
    return result;
}
// Preserve the missile's x87 factor until each component product is formed.
void RemoveBackwardProjectileSteering(CVector& steering,const CVector& direction) {
    ProjInfoRemoveBackwardSteeringCpp(&steering.x, &direction.x);
}
void AddProjectileSteering(CVector& velocity, const CVector& steering, float acceleration, float timestep, bool fast) {
    const float speedMultiplier = StaticRef<float>(0x858F08);
    ProjInfoAddSteeringCpp(&velocity.x, &steering.x, acceleration, timestep, fast, speedMultiplier);
}
void LimitProjectileSpeed(CProjectile* projectile) {
    ProjInfoLimitSpeedCpp(&projectile->m_vecMoveSpeed.x, StaticRef<float>(0x872C00));
}
}
void CProjectileInfo::Update() {
    if (StaticRef<uint8>(0xA43088) == 1) return;
    const uint32 now = CTimer::GetTimeInMS(); const float timestep = CTimer::GetTimeStep();
    for (size_t index = 0; index < MAX_PROJECTILES; ++index) {
        auto& info = gaProjectileInfo[index]; auto* projectile = ms_apProjectile[index];
        if (!info.m_bActive) continue;
        auto& physicalFlags = ProjectileField<uint32>(projectile, 0x40);
        if ((physicalFlags & 0x100) && info.m_pFxSystem) { info.m_pFxSystem->Kill(); info.m_pFxSystem = nullptr; }
        if (info.m_pCreator && info.m_pCreator->GetIsTypePed() && !info.m_pCreator->AsPed()->IsPointerValid()) info.m_pCreator = nullptr;
        const uint32 weapon = info.m_nWeaponType;
        auto& velocity = projectile->m_vecMoveSpeed;
        if (weapon == 39 || weapon == 16 || weapon == 17) {
            if (ProjectileField<float>(projectile, 0x9C) > .1f && std::abs(velocity.x) < .05f && std::abs(velocity.y) < .05f && std::abs(velocity.z) < .05f)
                ProjectileField<float>(projectile, 0x9C) = .03f;
            if (weapon == 17 && uint32(info.m_nDestroyTime - 17500) < now && CGeneral::GetRandomIntInRange(0, 100) < 10) {
                const auto position = projectile->GetPosition(); CWorld::SetPedsOnFire(position.x, position.y, position.z, 6.f, info.m_pCreator);
            }
        }
        if (weapon == 19 || weapon == 20) {
            FxPrtMult_c multiplier{.3f, .3f, .3f, .3f, .5f, 1.f, .08f};
            const CVector travel = velocity * timestep;
            const int32 count = std::max(1, static_cast<int32>(static_cast<int64>(std::sqrt(travel.x * travel.x + travel.z * travel.z + travel.y * travel.y))));
            const float inverseRandomMax = std::bit_cast<float>(0x38000100u);
            for (int32 particle = 0; particle < count; ++particle) {
                const float intensity = float(std::rand()) * inverseRandomMax * .25f + .25f;
                multiplier.m_Color.red = multiplier.m_Color.green = multiplier.m_Color.blue = intensity;
                multiplier.m_fLife = float(std::rand()) * inverseRandomMax * .04f + .08f;
                const float ratio = 1.f - float(particle) / float(count);
                const CVector position = projectile->GetPosition() - travel * ratio;
                const auto randomAxis = [inverseRandomMax] { const float random = float(std::rand()) * inverseRandomMax; return random + random - 1.f; };
                CVector randomDirection{randomAxis(), randomAxis(), randomAxis()}; randomDirection.Normalise();
                CVector heading = velocity; heading.Normalise();
                const CVector particleVelocity = CrossProduct(heading, randomDirection) * 1.5f;
                StaticRef<FxSystem_c*>(0xA9AE20)->AddParticle(position, particleVelocity, 0.f, multiplier, -1.f, 1.2f, .6f, false);
            }
        }
        if (uint32(info.m_nDestroyTime) < now && info.m_nDestroyTime != 0) {
            if (weapon == 39) {
                if (info.m_pCreator->GetIsTypePed() && info.m_pCreator->AsPed()->IsPlayer()) {
                    auto* ped = info.m_pCreator->AsPed();
                    const int32 slot = ped->GetWeaponSlot(static_cast<eWeaponType>(40));
                    if (ProjectileField<int32>(ped, 0x5A0 + slot * 0x1C) != 40 ||
                        ProjectileField<int32>(ped, 0x5AC + ped->GetWeaponSlot(static_cast<eWeaponType>(40)) * 0x1C) == 0)
                        info.m_nDestroyTime = 0;
                }
            } else RemoveProjectile(&info, projectile);
        } else {
            const auto missileCollision = [&] {
                if (!(physicalFlags & 0x200)) {
                    CWorld::pIgnoreEntity = info.m_pCreator;
                    ProjectileField<uint32>(projectile, 0x1C) &= ~1u;
                    const bool clear = CWorld::GetIsLineOfSightClear(info.m_vecLastPosn, projectile->GetPosition(), true, true, true, true, false, false, false);
                    CWorld::pIgnoreEntity = nullptr; ProjectileField<uint32>(projectile, 0x1C) |= 1u;
                    ProjectileField<CEntity*>(projectile, 0x128) = info.m_pCreator;
                    if (clear) return;
                }
                auto* collided = ProjectileField<CEntity*>(projectile, 0xBC);
                if (!ProjectileField<uint8>(projectile, 0xB9) || !collided || (collided != info.m_pCreator && collided->m_nModelIndex != 345))
                    RemoveProjectile(&info, projectile);
            };
            if (weapon == 19) {
                velocity += projectile->GetMatrix().GetForward() * (timestep * .008f); LimitProjectileSpeed(projectile); missileCollision();
            } else if (weapon == 58) {
                ProjectileField<uint32>(projectile, 0x1C) &= ~1u; CWorld::pIgnoreEntity = info.m_pCreator;
                const bool clear = CWorld::GetIsLineOfSightClear(info.m_vecLastPosn, projectile->GetPosition(), true, true, true, true, false, false, false);
                ProjectileField<uint32>(projectile, 0x1C) |= 1u; CWorld::pIgnoreEntity = nullptr;
                if (!clear) { velocity = CVector{0.f, 0.f, 0.f}; projectile->GetPosition() = info.m_vecLastPosn; }
            } else if (weapon == 18 || weapon == 21) {
                const auto position = projectile->GetPosition(); CWorld::pIgnoreEntity = info.m_pCreator; ProjectileField<uint32>(projectile, 0x1C) &= ~1u;
                bool check = !info.m_pCreator;
                if (info.m_pCreator) {
                    const auto delta = info.m_vecLastPosn - info.m_pCreator->GetPosition();
                    check = delta.y * delta.y + delta.z * delta.z + delta.x * delta.x >= 2.f;
                }
                if (check && ((physicalFlags & 0x200) || !CWorld::GetIsLineOfSightClear(info.m_vecLastPosn, position, true, true, true, true, false, false, false))) RemoveProjectile(&info, projectile);
                CWorld::pIgnoreEntity = nullptr; ProjectileField<uint32>(projectile, 0x1C) |= 1u;
            } else if (weapon == 20) {
                if (info.m_pVictim) {
                    if (info.m_pVictim == FindPlayerVehicle(-1, false)) AudioEngine.ReportFrontendAudioEvent(static_cast<eAudioEvents>(101), 0.f, 1.f);
                    const CVector aim = projectile->GetMatrix().GetForward(); const CVector origin = aim + projectile->GetPosition();
                    const float victimScore = CWeapon::EvaluateTargetForHeatSeekingMissile(info.m_pVictim, origin, aim, 1.2f, true, nullptr);
                    float bestScore = 0.f; CEntity* best = nullptr;
                    for (size_t decoy = 0; decoy < MAX_PROJECTILES; ++decoy) {
                        auto& candidate = gaProjectileInfo[decoy]; auto* object = ms_apProjectile[decoy];
                        if (candidate.m_nWeaponType == 58 && candidate.m_bActive) {
                            const float score = CWeapon::EvaluateTargetForHeatSeekingMissile(object, origin, aim, 1.2f, true, nullptr);
                            if (bestScore <= score) { bestScore = score; best = object; }
                        }
                    }
                    if (!best || bestScore <= victimScore) best = info.m_pVictim;
                    bool hunter = false;
                    if (best->GetIsTypeVehicle() && (info.m_pCreator == FindPlayerPed(-1) || info.m_pCreator == FindPlayerVehicle(-1, false)) && ProjectileField<int32>(best, 0x594) == 4) hunter = true;
                    CVector oldVelocity = velocity;
                    const CVector projected = hunter ? projectile->GetPosition() : velocity * 100.f + projectile->GetPosition();
                    const auto delta = projectile->GetPosition() - best->GetPosition();
                    const float distance = std::sqrt(delta.y * delta.y + delta.z * delta.z + delta.x * delta.x);
                    const float horizon = std::min(distance, hunter ? 1.5f : 50.f);
                    CVector steering = best->AsPhysical()->m_vecMoveSpeed * horizon + best->GetPosition() - projected; oldVelocity.Normalise();
                    RemoveBackwardProjectileSteering(steering,oldVelocity);
                    steering.Normalise();
                    float acceleration = (info.m_pCreator == FindPlayerPed(-1) || info.m_pCreator == FindPlayerVehicle(-1, false)) ? .011699999f : .009f;
                    const auto targetVelocity = best->AsPhysical()->m_vecMoveSpeed;
                    const bool fast = std::sqrt(targetVelocity.z * targetVelocity.z + targetVelocity.y * targetVelocity.y + targetVelocity.x * targetVelocity.x) > .8f;
                    float damping = 1.f;
                    if (hunter) { damping = ProjectileDamping(timestep); acceleration = StaticRef<float>(0x8D6108); }
                    velocity *= damping; AddProjectileSteering(velocity, steering, acceleration, timestep, fast && !hunter); LimitProjectileSpeed(projectile);
                    projectile->GetMatrix().GetForward() = oldVelocity;
                }
                missileCollision();
            } else if (weapon == 39 && ProjectileField<float>(projectile, 0xD8) > 0.f && ProjectileField<CPhysical*>(projectile, 0xDC) && !ProjectileField<CPhysical*>(projectile, 0xFC)) {
                projectile->AttachEntityToEntity(ProjectileField<CPhysical*>(projectile, 0xDC), static_cast<CVector*>(nullptr), static_cast<CQuaternion*>(nullptr));
                ProjectileField<uint32>(projectile, 0x1C) &= ~1u;
            }
        }
        info.m_vecLastPosn = projectile->GetPosition();
    }
}

// 0x739860
bool CProjectileInfo::IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool bDestroy) {
    const CBox bb{
        CVector{ x1, y1, z1 },
        CVector{ x2, y2, z2 }
    };
    bool found = false;
    for (auto&& [info, proj] : rngv::zip(gaProjectileInfo, ms_apProjectile)) {
        if (!info.m_bActive) {
            continue;
        }

        if (!IsWeaponTypeProjectile(static_cast<eWeaponType>(info.m_nWeaponType))) {
            continue;
        }

        if (!bb.IsPointInside(proj->GetPosition())) {
            continue;
        }

        found = true;
        if (bDestroy) {
            info.m_bActive = false;
            info.RemoveFXSystem(false);
            CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(proj));
            CWorld::Remove(proj);
            delete proj;
        }
    }
    return found;
}

// 0x7399B0
void CProjectileInfo::RemoveAllProjectiles() {
    for (size_t i = 0; i < MAX_PROJECTILES; ++i) {
        auto& info = gaProjectileInfo[i];
        auto* projectile = ms_apProjectile[i];
        if (info.m_bActive) {
            info.m_bActive = false;
            info.RemoveFXSystem(true);
            CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(projectile));
            CWorld::Remove(projectile);
            delete projectile;
        }
    }
}

// 0x739A40
bool CProjectileInfo::RemoveIfThisIsAProjectile(CObject* object) {
    for (size_t i = 0; i < MAX_PROJECTILES; ++i) {
        if (ms_apProjectile[i] == object && gaProjectileInfo[i].m_bActive) {
            gaProjectileInfo[i].m_bActive = false;
            gaProjectileInfo[i].RemoveFXSystem(false);
            CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(ms_apProjectile[i]));
            CWorld::Remove(ms_apProjectile[i]);
            delete ms_apProjectile[i];
            ms_apProjectile[i] = nullptr;
            return true;
        }
    }
    return false;
}

// 0x737B80
void CProjectileInfo::RemoveFXSystem(bool bInstantly) {
    if (!m_pFxSystem) {
        return;
    }
    if (bInstantly) {
        g_fxMan.DestroyFxSystem(m_pFxSystem);
    } else {
        m_pFxSystem->Kill();
    }
    m_pFxSystem = nullptr;
}
