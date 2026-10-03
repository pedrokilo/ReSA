/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CShotInfo
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
#include "ShotInfo.h"
#include <algorithm>

constexpr auto      MAX_SHOT_INFOS = 100u;
static inline auto& aShotInfos = StaticRef<std::array<CShotInfo, MAX_SHOT_INFOS>>(0xC89690);

// Originally CShotInfo::ms_afRandTable
static inline auto& RandTable = StaticRef<std::array<float, 20>>(0xC89628);

void CShotInfo::InjectHooks() {
    RH_ScopedClass(CShotInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x739B60);
    RH_ScopedInstall(Shutdown, 0x739C20);
    RH_ScopedInstall(AddShot, 0x739C30);
    RH_ScopedInstall(GetFlameThrowerShotPosn, 0x739DE0);
    RH_ScopedInstall(Update, 0x739E60);
}

// 0x739B60
void CShotInfo::Initialise() {
    // Como el original: campo a campo (sin tocar el relleno) y la tabla acumulando 0.005 en la pila x87.
    for (auto& shot : aShotInfos) {
        shot.m_vecOrigin       = CVector{ 0.0f, 0.0f, 0.0f };
        shot.m_vecTargetOffset = CVector{ 0.0f, 0.0f, 0.0f };
        shot.m_bExist          = false;
        shot.m_bExecuted       = false;
        shot.m_nWeaponType     = WEAPON_PISTOL;
        shot.m_fRange          = 1.0f;
        shot.m_pCreator        = nullptr;
        shot.m_DestroyTime     = 0.0f;
    }
    float value = -0.05f;
    for (auto& rd : RandTable) {
        rd = value;
        value = value + std::bit_cast<float>(0x3BA3D70Au); // 0.005f
    }
}

// 0x739C20
void CShotInfo::Shutdown() {
    // NOP
}

// 0x739C30
bool CShotInfo::AddShot(CEntity* creator, eWeaponType weaponType, CVector origin, CVector target) {
    const auto weapInfo = CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD);
    const auto shot = rng::find_if_not(aShotInfos, &CShotInfo::m_bExist);
    if (shot == aShotInfos.end()) {
        return false;
    }

    shot->m_vecOrigin       = origin;
    shot->m_nWeaponType     = weaponType;
    shot->m_vecTargetOffset = CVector{ target.x - origin.x, target.y - origin.y, target.z - origin.z };
    shot->m_bExist          = true;
    shot->m_bExecuted       = false;
    shot->m_fRange          = weapInfo->m_fRadius;
    if (weapInfo->m_fSpread != 0.0f) {
        // Como el original: rand() % 20 sobre la tabla, sin dispersion en z.
        shot->m_vecTargetOffset.x = RandTable[CGeneral::GetRandomNumber() % 20] * weapInfo->m_fSpread + shot->m_vecTargetOffset.x;
        shot->m_vecTargetOffset.y = RandTable[CGeneral::GetRandomNumber() % 20] * weapInfo->m_fSpread + shot->m_vecTargetOffset.y;
        shot->m_vecTargetOffset.z = RandTable[CGeneral::GetRandomNumber() % 20] + shot->m_vecTargetOffset.z;
    }
    shot->m_vecTargetOffset.Normalise();

    const float speed = weapInfo->flags.bRandSpeed ? weapInfo->m_Speed + RandTable[CGeneral::GetRandomNumber() % 20] : weapInfo->m_Speed;
    shot->m_vecTargetOffset.x = speed * shot->m_vecTargetOffset.x;
    shot->m_vecTargetOffset.y = speed * shot->m_vecTargetOffset.y;
    shot->m_vecTargetOffset.z = speed * shot->m_vecTargetOffset.z;
    shot->m_pCreator = creator;

    // Como el original: vida + tiempo (entero sin signo) truncado a entero y vuelto a float.
    const float  lifespan = weapInfo->m_fLifespan, twoPow32 = 4294967296.f;
    const uint32 now = CTimer::GetTimeInMS();
    float sum;
    __asm {
        fld lifespan
        mov eax, now
        fild now
        test eax, eax
        jge shot_now_ok
        fadd twoPow32
    shot_now_ok:
        faddp st(1), st
        fstp sum
    }
    const int32 destroy = (int32)(int64)sum;
    shot->m_DestroyTime = (float)destroy;
    return true;
}

// 0x739DE0
bool CShotInfo::GetFlameThrowerShotPosn(uint8 shotId, CVector& outPos) {
    if (const auto& s = aShotInfos[shotId]; s.m_bExist && s.m_nWeaponType == WEAPON_FLAMETHROWER) {
        outPos = s.m_vecOrigin;
        return true;
    }
    outPos.Reset();
    return false;
}

// 0x739E60
void CShotInfo::Update() {
    for (auto&& [i, shot] : rngv::enumerate(aShotInfos)) {
        if (shot.m_pCreator) {
            if (shot.m_pCreator->GetIsTypePed() && !shot.m_pCreator->AsPed()->IsPointerValid()) {
                shot.m_pCreator = nullptr;
            }
        }

        if (!shot.m_bExist) {
            continue;
        }

        const auto& weaponInfo = *CWeaponInfo::GetWeaponInfo(shot.m_nWeaponType, eWeaponSkill::STD);
        // Como el original: el tiempo se carga exacto con fild (sin redondear a float) antes de comparar.
        // (por encima de 2^31 el fadd 2^32 redondea a 24 bits, es decir, a float)
        const uint32 nowMs = CTimer::GetTimeInMS();
        const double now = (nowMs & 0x80000000u) ? (double)(float)nowMs : (double)nowMs;
        if (now > (double)shot.m_DestroyTime) {
            shot.m_bExist = false;
        }

        if (weaponInfo.flags.bSlowdown) {
            // Como el original: pow(0.96, paso) con el _CIpow del EXE (0x822130) y ST0 completo en los productos.
            const double base = 0.9599999785423279;
            const float  step = CTimer::GetTimeStep();
            float* const offPtr = &shot.m_vecTargetOffset.x;
            __asm {
                fld base
                fld step
                mov eax, 0x822130
                call eax
                mov edx, offPtr
                fld st(0)
                fmul dword ptr [edx]
                fstp dword ptr [edx]
                fld st(0)
                fmul dword ptr [edx+4]
                fstp dword ptr [edx+4]
                fmul dword ptr [edx+8]
                fstp dword ptr [edx+8]
            }
        }

        if (weaponInfo.flags.bRangeIncreasesOverTime) {
            shot.m_fRange = CTimer::GetTimeStep() * std::bit_cast<float>(0x3D99999Au) + shot.m_fRange; // 0.075f
        }

        shot.m_vecOrigin.x = CTimer::GetTimeStep() * shot.m_vecTargetOffset.x + shot.m_vecOrigin.x;
        shot.m_vecOrigin.y = CTimer::GetTimeStep() * shot.m_vecTargetOffset.y + shot.m_vecOrigin.y;
        shot.m_vecOrigin.z = CTimer::GetTimeStep() * shot.m_vecTargetOffset.z + shot.m_vecOrigin.z;

        if (shot.m_pCreator) {
            const auto range = shot.m_fRange < 1.0f ? 1.0f : shot.m_fRange;
            // Como el original: recorre los indices 16..1 de la lista de entidades del escaner (el 16 se sale de la
            // lista y lee el campo siguiente, 0x170).
            auto* const intel = *reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(shot.m_pCreator) + 0x47C);
            for (auto idx = 16; idx > 0; idx--) {
                auto* const ped = *reinterpret_cast<CPed**>(intel + 0x130 + idx * 4);
                if (!ped || !ped->IsPointerValid() || ped->bInVehicle) {
                    continue;
                }
                const CVector& pedPos = ped->GetPosition();
                const CVector d{ pedPos.x - shot.m_vecOrigin.x, pedPos.y - shot.m_vecOrigin.y, pedPos.z - shot.m_vecOrigin.z };
                if (ped->physicalFlags.bFireProof) {
                    continue;
                }
                if (!ped->IsPedInControl()) {
                    continue;
                }
                if (!(d.z * d.z + d.y * d.y + d.x * d.x < range)) { // Como el original: z + y + x
                    continue;
                }

                if (shot.m_nWeaponType != eWeaponType::WEAPON_SPRAYCAN && shot.m_nWeaponType != eWeaponType::WEAPON_EXTINGUISHER) {
                    gFireManager.StartFire(ped, shot.m_pCreator, 0.8f, 1, 7'000u, 100);
                    continue;
                }

                if (shot.m_bExecuted) {
                    continue;
                }

                const CVector& creatorPos = shot.m_pCreator->GetPosition();
                const CVector2D localDir{ creatorPos.x - pedPos.x, creatorPos.y - pedPos.y };
                const auto localDirection = ped->GetLocalDirection(localDir);
                CWeapon::GenerateDamageEvent(
                    ped,
                    shot.m_pCreator,
                    shot.m_nWeaponType,
                    (int16)weaponInfo.m_nDamage, // Como el original: movsx
                    ePedPieceTypes::PED_PIECE_TORSO,
                    localDirection
                );
                shot.m_bExecuted = true;
            }
        }

        switch (shot.m_nWeaponType) {
        case eWeaponType::WEAPON_SPRAYCAN: {
            if (shot.m_bExecuted) {
                break;
            }
            CVector sprayDir;
            const auto sprayState = CWorld::SprayPaintWorld(shot.m_vecOrigin, sprayDir, shot.m_fRange, true);
            if (sprayState != eSprayPaintState::NOT_FOUND) {
                shot.m_bExecuted = true;

                auto& off = shot.m_vecTargetOffset;
                const float dot = sprayDir.y * off.y + sprayDir.z * off.z + sprayDir.x * off.x; // Como el original: y + z + x
                off.x = off.x - sprayDir.x * dot;
                off.y = off.y - sprayDir.y * dot;
                off.z = off.z - sprayDir.z * dot;
                off.x = off.x + off.x;
                off.y = off.y + off.y;
                off.z = off.z + off.z;
                if (sprayState == eSprayPaintState::PAINTED && shot.m_pCreator == FindPlayerPed()) {
                    AudioEngine.ReportFrontendAudioEvent(eAudioEvents::AE_FRONTEND_PART_MISSION_COMPLETE, 0.f, 1.f);
                }
            }
            break;
        }
        case eWeaponType::WEAPON_EXTINGUISHER: {
            if (!shot.m_bExecuted && gFireManager.ExtinguishPointWithWater(shot.m_vecOrigin, shot.m_fRange, 2.f)) {
                shot.m_bExecuted = true;
            }
            break;
        }
        default: {
            if (((CTimer::GetFrameCounter() + (uint32)i) & 3) == 0) {
                CWorld::SetCarsOnFire(shot.m_vecOrigin, 4.f, shot.m_pCreator);
            }
            CWorld::SetWorldOnFire(shot.m_vecOrigin, 0.1f, shot.m_pCreator);
        }
        }
    }
}
