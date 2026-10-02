/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAECollisionAudioEntity
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <extensions/utility.hpp>
#include <reversiblebugfixes/Bugs.hpp>

#include "AECollisionAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

#include "AECollisionAudioEntity.CollisionLookup.h"

namespace {
// CAESound::Initialise (0x4EFE50) deja m_ClientVariable = -1.0 y el original de
// esta clase solo lo cambia en algunos sonidos. tSoundPlayParams usa 0.0 por
// defecto, así que aquí se pasa explícitamente el valor original.
constexpr float NO_CLIENT_VARIABLE = -1.f;

// Como el original: log10 con fldlg2/fyl2x a precisión interna completa del FPU y
// producto por 20 redondeado una sola vez (sin el umbral de CAEAudioUtility::AudioLog10).
float Log10Times20(float x) {
    float k20 = 20.f; // 0x858BA4
    float result;
    __asm {
        fldlg2
        fld   x
        fyl2x
        fmul  k20
        fstp  result
    }
    return result;
}

// Como el original: limpia la entrada sin tocar LoopStopTimeMs (Reset 0x4DA320 y Service 0x4DA2C0).
void ClearCollisionSoundEntry(tCollisionSound& entry) {
    entry.EntityA  = nullptr;
    entry.EntityB  = nullptr;
    entry.SurfaceA = TOTAL_NUM_COLLISION_SURFACE_TYPES;
    entry.SurfaceB = TOTAL_NUM_COLLISION_SURFACE_TYPES;
    entry.Sound    = nullptr;
    entry.Status   = COLLISION_SOUND_INACTIVE;
}

// Como el original: |v|² y |t|² se comparan y se usa el vector mayor (t si |t|² >= |v|² o NaN).
// zFirst: el par coche-coche de UpdateLoopingCollisionSound suma z² + y² + x²; el resto x² + y² + z².
float GetLoopingCollisionMagnitude(const CVector& v, const CVector& t, bool zFirst) {
    const auto SqMag = [zFirst](const CVector& a) {
        return zFirst
            ? a.z * a.z + a.y * a.y + a.x * a.x
            : a.x * a.x + a.y * a.y + a.z * a.z;
    };
    return std::sqrt(SqMag(SqMag(t) < SqMag(v) ? v : t));
}
} // namespace

void CAECollisionAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAECollisionAudioEntity, 0x862E64, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x5B9BD0);
    RH_ScopedInstall(InitialisePostLoading, 0x4DA050);
    RH_ScopedInstall(AddCollisionSoundToList, 0x4DAAC0);
    RH_ScopedInstall(Reset, 0x4DA320);
    RH_ScopedInstall(ReportGlassCollisionEvent, 0x4DA070);
    RH_ScopedInstall(UpdateLoopingCollisionSound, 0x4DA540);
    RH_ScopedInstall(GetCollisionSoundStatus, 0x4DA830);
    RH_ScopedInstall(ReportObjectDestruction, 0x4DAB60);
    RH_ScopedInstall(PlayOneShotCollisionSound, 0x4DB150);
    RH_ScopedInstall(PlayLoopingCollisionSound, 0x4DB450);
    RH_ScopedInstall(PlayBulletHitCollisionSound, 0x4DB7C0);
    RH_ScopedInstall(ReportCollision, 0x4DBA10);
    RH_ScopedInstall(ReportBulletHit, 0x4DBDF0);
    RH_ScopedInstall(Service, 0x4DA2C0);
    RH_ScopedInstall(ChooseCollisionSoundID, 0x4DAA50);
    RH_ScopedOverloadedInstall(ReportWaterSplash, "at-position", 0x4DA190, void(CAECollisionAudioEntity::*)(CVector, float));
    RH_ScopedOverloadedInstall(ReportWaterSplash, "for-physical", 0x4DAE40, void(CAECollisionAudioEntity::*)(CPhysical*, float, bool));
}

// 0x5B9BD0
void CAECollisionAudioEntity::Initialise() {
    // Como el original: no toca los campos de CAEAudioEntity y deja todo el historial a -1.
    m_NumActiveCollisionSounds = 0;
    for (auto& entry : m_CollisionSoundList) {
        entry = {};
    }
    rng::fill(m_CollisionSoundIDHistory, (int16)(-1));
    m_LastBulletHitSoundID = -1;
}

// 0x4DA050
void CAECollisionAudioEntity::InitialisePostLoading() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS);
}

// 0x4DA320
void CAECollisionAudioEntity::Reset() {
    for (auto& entry : m_CollisionSoundList) {
        if (entry.Status != COLLISION_SOUND_LOOPING) {
            continue;
        }

        if (entry.Sound) {
            entry.Sound->StopSoundAndForget();
        }

        ClearCollisionSoundEntry(entry);
        --m_NumActiveCollisionSounds; // Como el original: Reset también descuenta la entrada
    }
}

// 0x4DAAC0
void CAECollisionAudioEntity::AddCollisionSoundToList(
    CEntity* entityA,
    CEntity* entityB,
    eSurfaceType surfaceA,
    eSurfaceType surfaceB,
    CAESound* sound,
    eCollisionSoundStatus status
) {
    // Find an entry with no sound.
    const auto e = rng::find_if_not(m_CollisionSoundList, &tCollisionSound::Sound);
    if (e == m_CollisionSoundList.end()) {
        NOTSA_LOG_WARN("Collision sound list is full");
        return;
    }

    e->EntityA  = entityA;
    e->EntityB  = entityB;

    e->SurfaceA = surfaceA;
    e->SurfaceB = surfaceB;

    e->Sound          = sound;
    e->Status         = status;
    e->LoopStopTimeMs = status == COLLISION_SOUND_LOOPING
        ? CTimer::GetTimeInMS() + 100
        : 0;

    m_NumActiveCollisionSounds++;
}

// 0x4DA830
eCollisionSoundStatus CAECollisionAudioEntity::GetCollisionSoundStatus(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, int32& outIndex) {
    auto status = COLLISION_SOUND_INACTIVE;
    for (auto&& [i, v] : rngv::enumerate(m_CollisionSoundList)) {
        if (v.EntityA == entityA && v.EntityB == entityB || v.EntityA == entityB && v.EntityB == entityA) {
            if ((status = v.Status) == COLLISION_SOUND_LOOPING) {
                outIndex = (int32)(i);
                return v.Status;
            }
        }
    }
    outIndex = (int32)(NUM_ENTRIES); // Como el original: sin entrada en bucle deja el índice en 300, no -1
    return status;
}

// 0x4DB150
void CAECollisionAudioEntity::PlayOneShotCollisionSound(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, float impulseMagnitude, const CVector& posn) {
    // Como el original: primera pasada con el sonido de surfaceA (entidades A, B) y
    // segunda con el de surfaceB (entidades B, A); la lista recibe siempre (surfaceA, surfaceB).
    for (auto pass = 0; pass < 2; pass++) {
        CEntity* const     eA = pass == 0 ? entityA : entityB;
        CEntity* const     eB = pass == 0 ? entityB : entityA;
        const eSurfaceType sB = pass == 0 ? surfaceA : surfaceB; // Superficie cuyo sonido se elige
        const eSurfaceType sA = pass == 0 ? surfaceB : surfaceA; // La otra superficie

        if (sB >= TOTAL_NUM_COLLISION_SURFACE_TYPES) {
            continue; // Como el original: pasa a la otra superficie
        }
        auto slot = SND_BANK_SLOT_COLLISIONS;
        if (sB == AE_SURFACE_TYPE_POOL_BALL) {
            if (sA != AE_SURFACE_TYPE_POOL_BALL) {
                return;
            }
            slot = SND_BANK_SLOT_MISSION4;
            if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_SCRIPT_POOL_MINIGAME, slot)) {
                return;
            }
            impulseMagnitude *= 5.f; // Como el original: modifica el argumento (se acumula en la segunda pasada)
        } else {
            if (sA == AE_SURFACE_TYPE_POOL_BALL) {
                return;
            }
            if (sB == AE_SURFACE_TYPE_BASKETBALL || sB == AE_SURFACE_TYPE_PUNCHBAG) { // 0x4DB23E
                slot = SND_BANK_SLOT_MISSION4;
                if (!AEAudioHardware.IsSoundBankLoaded(sB == AE_SURFACE_TYPE_BASKETBALL ? SND_BANK_SCRIPT_BASKETBALL : SND_BANK_SCRIPT_GYM, slot)) {
                    return;
                }
            }
        }
        const auto soundID = ChooseCollisionSoundID(sB); // 0x4DB280
        if (soundID == -1) {
            continue;
        }
        // Como el original: (ParamD * impulso) * 0.01, no una división entre 100.
        auto offset = (float)(gCollisionLookup[sA].ParamD) * impulseMagnitude * 0.01f;
        if (sB == AE_SURFACE_TYPE_BMX && sA == SURFACE_PED) { // 0x4DB2A6
            offset *= 10.f;
        }
        offset *= 500.f;
        // Como el original: floor en double, _ftol y comparación en 16 bits.
        const auto floored = (int16)((int64)(std::floor((double)(offset))));
        const auto startAt = (int16)(100 - (floored > 100 ? 100 : floored)); // 0x4DB2FB
        if (startAt >= 100) {
            continue;
        }
        const auto maxStartAt = gCollisionLookup[sB].MaxStartAtPercentage;
        auto       volume     = GetDefaultVolume(AE_GENERAL_COLLISION) - 3.f;
        auto       playTime   = startAt;
        if (startAt > maxStartAt) { // 0x4DB32D
            volume   += Log10Times20((float)(100 - startAt) / (float)(100 - maxStartAt));
            playTime  = (int16)(maxStartAt);
        }
        if (volume <= -100.f) { // 0x4DB37E
            continue;
        }
        auto* const sound = AESoundManager.PlaySound({
            .BankSlotID        = slot,
            .SoundID           = soundID,
            .AudioEntity       = this,
            .Pos               = posn,
            .Volume            = volume,
            .RollOffFactor     = 2.f,
            .Flags             = SOUND_ROLLED_OFF | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES,
            .FrequencyVariance = 0.02f,
            .PlayTime          = playTime,
            .ClientVariable    = NO_CLIENT_VARIABLE,
        });
        m_CollisionSoundIDHistory[sB] = soundID;
        if (sound) { // 0x4DB3E7
            AddCollisionSoundToList(eA, eB, surfaceA, surfaceB, sound, COLLISION_SOUND_ONE_SHOT);
        }
    }
}

// Como el original (0x4DB450): volumen y velocidad final (ya multiplicada por 0.8) del
// sonido en bucle. Sin vehículo o con |v| == 0 queda en -100 dB y 0.75 * 0.8.
std::pair<float, float> CAECollisionAudioEntity::GetLoopingCollisionSoundVolumeAndSpeed(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, bool isForceLooping) {
    auto volume    = -100.f;
    auto baseSpeed = 0.75f;

    std::optional<float> mag{};
    if (surfaceA == SURFACE_CAR && surfaceB == SURFACE_CAR) {
        const auto vehA = entityA->AsVehicle(),
                   vehB = entityB->AsVehicle();
        mag = GetLoopingCollisionMagnitude(vehA->GetMoveSpeed() - vehB->GetMoveSpeed(), vehA->GetTurnSpeed() - vehB->GetTurnSpeed(), false);
    } else if ((surfaceB != SURFACE_PED && surfaceA == SURFACE_CAR) || (surfaceA != SURFACE_PED && surfaceB == SURFACE_CAR)) {
        assert(entityA->GetIsTypePhysical());

        const auto* const physicalA = entityA->AsPhysical(); // Como el original: siempre entityA
        mag = GetLoopingCollisionMagnitude(physicalA->GetMoveSpeed(), physicalA->GetTurnSpeed(), false);
    }

    if (mag && *mag != 0.f) { // NaN sigue adelante, como en el original
        auto speed = std::sqrt(*mag * (float)(gCollisionLookup[surfaceA].ParamD * gCollisionLookup[surfaceB].ParamD) * 0.0001f) * 3.f;
        if (!(speed < 0.3f)) {
            speed = 0.3f;
        }
        if (isForceLooping) {
            speed *= 0.16666667f; // 0x85F0A0, no una división entre 6
        }
        auto ratio = 1.3333334f * speed; // 0x85F09C
        if (!(ratio < 1.f)) {
            ratio = 1.f;
        }
        volume = Log10Times20(ratio) + GetDefaultVolume(AE_GENERAL_COLLISION);
        if (speed > 0.75f) {
            baseSpeed = speed;
        }
    }
    return { volume, baseSpeed * 0.8f };
}

// 0x4DB450
void CAECollisionAudioEntity::PlayLoopingCollisionSound(CEntity* entityA, CEntity* entityB, eSurfaceType surfaceA, eSurfaceType surfaceB, float force, const CVector& pos, bool isForceLooping) {
    const auto [volume, speed] = GetLoopingCollisionSoundVolumeAndSpeed(entityA, entityB, surfaceA, surfaceB, isForceLooping);

    const auto PlaySound = [&](eSoundID soundID) {
        if (auto* const sound = AESoundManager.PlaySound({
            .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
            .SoundID        = soundID,
            .AudioEntity    = this,
            .Pos            = pos,
            .Volume         = volume,
            .RollOffFactor  = 2.f,
            .Speed          = speed,
            .Flags          = SOUND_REQUEST_UPDATES,
            .ClientVariable = NO_CLIENT_VARIABLE,
            })
        ) {
            AddCollisionSoundToList(entityA, entityB, surfaceA, surfaceB, sound, COLLISION_SOUND_LOOPING);
        }
    };

    // FIX(#1406):
    // This fixes an OOB access in the original code,
    // Namely, originally for BMX `AE_SURFACE_TYPE_BMX` was passed in,
    // but it's not an actual surface type, but a special case for sound selection.

    // If both surfaces are valid we preserve the original logic...
    if (!notsa::bugfixes::CAECollisionAudioEntity_PlayLoopingCollisionSound_InvalidSurfaceType || (surfaceA < TOTAL_NUM_SURFACE_TYPES && surfaceB < TOTAL_NUM_SURFACE_TYPES)) {
        if (g_surfaceInfos.IsAudioGrass(surfaceA) || g_surfaceInfos.IsAudioGrass(surfaceB)) {
            PlaySound(SND_GENRL_COLLISIONS_GRASS_SKID);
        } else if (g_surfaceInfos.IsAudioWater(surfaceA) || g_surfaceInfos.IsAudioWater(surfaceB)) {
            PlaySound(SND_GENRL_COLLISIONS_WATER_LOOP);
        } else if (g_surfaceInfos.IsAudioMetal(surfaceA) || g_surfaceInfos.IsAudioMetal(surfaceB)) {
            PlaySound(SND_GENRL_COLLISIONS_METAL_SCRAPE);
        } else {
            PlaySound(SND_GENRL_COLLISIONS_GRAVEL_SKID);
        }
    } else {
        const auto GetSoundIDForSurface = [&](eSurfaceType surface) -> std::optional<eSoundID> { // 0x4DB6AF
            assert(surface < TOTAL_NUM_COLLISION_SURFACE_TYPES && "Expected `AE_SURFACE_TYPE_*` - Any other values are a bug");
            if (surface < TOTAL_NUM_SURFACE_TYPES) {
                if (g_surfaceInfos.IsAudioGrass(surface)) {
                    return SND_GENRL_COLLISIONS_GRASS_SKID;
                } else if (g_surfaceInfos.IsAudioWater(surface)) {
                    return SND_GENRL_COLLISIONS_WATER_LOOP;
                } else if (g_surfaceInfos.IsAudioMetal(surface)) {
                    return SND_GENRL_COLLISIONS_METAL_SCRAPE;
                }
            }
            return std::nullopt;
        };
        if (const auto soundOfSurfaceA = GetSoundIDForSurface(surfaceA)) {
            PlaySound(*soundOfSurfaceA);
        } else if (const auto soundOfSurfaceB = GetSoundIDForSurface(surfaceB)) {
            PlaySound(*soundOfSurfaceB);
        } else {
            PlaySound(SND_GENRL_COLLISIONS_GRAVEL_SKID);
        }
    }
}

// 0x4DA540
void CAECollisionAudioEntity::UpdateLoopingCollisionSound(
    CAESound*      sound,
    CEntity*       entityA,
    CEntity*       entityB,
    eSurfaceType   surfaceA,
    eSurfaceType   surfaceB,
    float          impulseForce,
    const CVector& pos,
    bool           isForceLooping
) {
    // Como el original: a diferencia de PlayLoopingCollisionSound no hay raíz del producto,
    // el tope es 1.0, no se comprueba |v| == 0 y sin vehículo la magnitud es 0.
    auto mag = 0.f;
    if (surfaceA == SURFACE_CAR && surfaceB == SURFACE_CAR) {
        const auto vehA = entityA->AsVehicle(),
                   vehB = entityB->AsVehicle();
        mag = GetLoopingCollisionMagnitude(vehA->GetMoveSpeed() - vehB->GetMoveSpeed(), vehA->GetTurnSpeed() - vehB->GetTurnSpeed(), true);
    } else if ((surfaceB != SURFACE_PED && surfaceA == SURFACE_CAR) || (surfaceA != SURFACE_PED && surfaceB == SURFACE_CAR)) {
        const auto* const physicalA = entityA->AsPhysical(); // Como el original: siempre entityA
        mag = GetLoopingCollisionMagnitude(physicalA->GetMoveSpeed(), physicalA->GetTurnSpeed(), false);
    }

    auto speed = mag * (float)(gCollisionLookup[surfaceA].ParamD * gCollisionLookup[surfaceB].ParamD) * 0.0001f * 3.f;
    if (!(speed < 1.f)) {
        speed = 1.f;
    }
    if (isForceLooping) {
        speed *= 0.16666667f; // 0x85F0A0
    }
    auto ratio = 1.3333334f * speed; // 0x85F09C
    if (!(ratio < 1.f)) {
        ratio = 1.f;
    }
    const auto volume      = Log10Times20(ratio) + GetDefaultVolume(AE_GENERAL_COLLISION);
    const auto targetSpeed = (speed > 0.75f ? speed : 0.75f) * 0.8f;

    // Como el original: acerca el valor al objetivo con el paso dado; si son iguales (o NaN) no escribe.
    const auto StepTowards = [](float cur, float target, float step) -> std::optional<float> {
        if (target > cur) {
            const auto next = cur + step;
            return next < target ? next : target;
        }
        if (target < cur) {
            const auto next = cur - step;
            return next > target ? next : target;
        }
        return std::nullopt;
    };
    if (const auto v = StepTowards(sound->GetVolume(), volume, 1.f)) {
        sound->SetVolume(*v);
    }
    if (const auto s = StepTowards(sound->GetSpeed(), targetSpeed, 0.1f)) {
        sound->SetSpeed(*s);
    }
    sound->SetPosition(pos);
}

// 0x4DB7C0
void CAECollisionAudioEntity::PlayBulletHitCollisionSound(eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    // ReportBulletHit uses a collision-audio ID for BMX (188), not a surface
    // index. Bullet impacts on its frame use vehicle material, just like other
    // vehicles. Never index the 179-entry surface table with an audio-only ID.
    if (notsa::bugfixes::GenericOOB) {
        if (surface == AE_SURFACE_TYPE_BMX) {
            surface = SURFACE_CAR;
        }
        if (surface >= TOTAL_NUM_SURFACE_TYPES) {
            return;
        }
    } else if (surface >= TOTAL_NUM_COLLISION_SURFACE_TYPES) { // Como el original: solo descarta >= 195
        return;
    }
    const auto PlayRandomSound = [&](int32 minID, int32 maxID, float volumeOffset = 0.f, float rollOff = 1.5f) { // 0x4DB9BF
        //! Find a new random sound ID that is not the same as the last one.
        const auto GetNewRandomSoundID = [&]{
            while (true) {
                const auto id = (int16)(CAEAudioUtility::GetRandomNumberInRange(minID, maxID));
                if (id != m_LastBulletHitSoundID) {
                    return id;
                }
            }
        };
        const auto soundID = GetNewRandomSoundID();
        AESoundManager.PlaySound({
            .BankSlotID        = SND_BANK_SLOT_BULLET_HITS,
            .SoundID           = soundID,
            .AudioEntity       = this,
            .Pos               = posn,
            .Volume            = GetDefaultVolume(AE_BULLET_HIT) + volumeOffset,
            .RollOffFactor     = rollOff,
            .FrequencyVariance = 0.02f,
            .ClientVariable    = NO_CLIENT_VARIABLE,
        });
        m_LastBulletHitSoundID = soundID; // Como el original: se guarda después de RequestNewSound
    };
    if (surface == SURFACE_PED) {
        PlayRandomSound(7, 9);
    } else if (g_surfaceInfos.IsAudioWater(surface)) {
        PlayRandomSound(16, 18, 6.f, 2.f);
    } else if (g_surfaceInfos.IsAudioWood(surface)) {
        PlayRandomSound(19, 21);
    } else if (g_surfaceInfos.IsAudioMetal(surface)) {
        // Como el original: (90 - ángulo) * (1/180) con la constante 0x85F0AC, no una división.
        if (CAEAudioUtility::ResolveProbability((90.0f - angleWithColPointNorm) * (1.f / 180.f))) { // see BoneNode_c::EulerToQuat
            PlayRandomSound(10, 12);
        } else {
            PlayRandomSound(4, 6);
        }
    } else if (g_surfaceInfos.IsAudioConcrete(surface) || g_surfaceInfos.IsAudioGravel(surface) || g_surfaceInfos.IsAudioTile(surface)) { // Como el original: concreto, grava, baldosa
        PlayRandomSound(13, 15);
    } else {
        PlayRandomSound(1, 3);
    }
}

// 0x4DA070
void CAECollisionAudioEntity::ReportGlassCollisionEvent(eAudioEvents event, const CVector& posn, uint32 time) {
    const auto PlayCollisionSound = [&](eSoundID soundID, float speed) {
        if (time) {
            AESoundManager.PlaySound({
                .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
                .SoundID        = soundID,
                .AudioEntity    = this,
                .Pos            = posn,
                .Volume         = GetDefaultVolume(event),
                .RollOffFactor  = 1.5f,
                .Speed          = 0.f,
                .Flags          = SOUND_REQUEST_UPDATES,
                .EventID        = event,
                .ClientVariable = (float)(time + CTimer::GetTimeInMS()),
            });
        } else {
            AESoundManager.PlaySound({
                .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
                .SoundID        = soundID,
                .AudioEntity    = this,
                .Pos            = posn,
                .Volume         = GetDefaultVolume(event),
                .RollOffFactor  = 1.5f,
                .Speed          = speed,
                .ClientVariable = NO_CLIENT_VARIABLE,
            });
        }
    };
    switch (event) {
    case AE_GLASS_HIT:             PlayCollisionSound(51, 1.f);                                                break;
    case AE_GLASS_CRACK:           PlayCollisionSound(68, 1.f);                                                break;
    case AE_GLASS_BREAK_SLOW:      PlayCollisionSound(56, 0.75f);                                              break;
    case AE_GLASS_BREAK_FAST:      PlayCollisionSound(56, 1.f);                                                break;
    case AE_GLASS_HIT_GROUND:      PlayCollisionSound(CAEAudioUtility::GetRandomNumberInRange(15, 18), 1.f);   break;
    case AE_GLASS_HIT_GROUND_SLOW: PlayCollisionSound(CAEAudioUtility::GetRandomNumberInRange(15, 18), 0.56f); break;
    default:                       break; // Como el original: otros eventos no hacen nada
    }
}

// 0x4DA190
void CAECollisionAudioEntity::ReportWaterSplash(CVector posn, float volume) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
        return;
    }
    AESoundManager.PlaySound({
        .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
        .SoundID        = 67,
        .AudioEntity    = this,
        .Pos            = posn,
        .Volume         = GetDefaultVolume(AE_WATER_SPLASH) + volume,
        .RollOffFactor  = 2.5f,
        .Speed          = 1.26f,
        .Flags          = SOUND_REQUEST_UPDATES,
        .EventID        = 1,
        .ClientVariable = NO_CLIENT_VARIABLE,
    });
    AESoundManager.PlaySound({
        .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
        .SoundID        = 66,
        .AudioEntity    = this,
        .Pos            = posn,
        .Volume         = GetDefaultVolume(AE_WATER_SPLASH) + volume,
        .RollOffFactor  = 2.5f,
        .Speed          = 0.f,
        .Flags          = SOUND_REQUEST_UPDATES,
        .EventID        = 2,
        .ClientVariable = (float)(CTimer::GetTimeInMS() + 166)
    });
}

// 0x4DAE40
void CAECollisionAudioEntity::ReportWaterSplash(CPhysical* physical, float volume, bool isForceSplash) {
    assert(physical->GetIsTypePhysical());

    if (!isForceSplash) {
        if (physical->GetMoveSpeed().z > -0.1f && volume == -100.f) {
            return;
        }
        if (rng::any_of(std::to_array({ 1, 2, 3 }), [&](int32 event) {
            return AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, physical) != 0;
        })) {
            return;
        }
    }
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) { // 0x4DAED2
        return;
    }
    const auto PlaySound = [&](int32 eventID, float speed, float volumeOffset, uint32 interval) {
        if (!(volume > -100.f)) { // 0x4DAF90 - Como el original: también con NaN
            // Como el original: log10(min(|v|, 0.6) * 1.6666666) * 20 en línea, sin el umbral de AudioLog10.
            const auto mag      = physical->GetMoveSpeed().Magnitude();
            const auto speedLog = Log10Times20((mag < 0.6f ? mag : 0.6f) * 1.6666666f);
            volume = speedLog > -18.f ? speedLog : -18.f;
        }
        volume += GetDefaultVolume(AE_WATER_SPLASH) + volumeOffset;

        // Como el original: los dos sonidos se registran con la entidad (flags 0x84) y el segundo arranca con velocidad 0.
        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 67,
            .AudioEntity        = this,
            .Pos                = physical->GetPosition(),
            .Volume             = volume,
            .RollOffFactor      = 2.5f,
            .Speed              = speed,
            .Flags              = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .RegisterWithEntity = physical,
            .EventID            = 1,
            .ClientVariable     = NO_CLIENT_VARIABLE,
        });
        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 66,
            .AudioEntity        = this,
            .Pos                = physical->GetPosition(),
            .Volume             = volume,
            .RollOffFactor      = 2.5f,
            .Speed              = 0.f,
            .Flags              = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .RegisterWithEntity = physical,
            .EventID            = eventID,
            .ClientVariable     = (float)(CTimer::GetTimeInMS() + interval),
        });
    };
    if (physical->GetIsTypePed()) { // 0x4DAF2B - Inverted
        if (isForceSplash && !physical->AsPed()->GetIntelligence()->GetTaskSwim()) {
            PlaySound(2, 1.26f, -6.f, 166);
        }
    } else if (physical->GetIsTypeVehicle()) { // 0x4DAF52
        PlaySound(3, 0.94f, 0.f, 248);
    } else { // 0x4DAF70
        PlaySound(2, 1.26f, -12.f, 166);
    }
}

// 0x4DAB60
void CAECollisionAudioEntity::ReportObjectDestruction(CEntity* entity) {
    if (!entity) {
        return;
    }

    const auto PlaySoundForSurface = [&](eSurfaceType surface) {
        const auto PlaySound = [&](eSoundID soundID) {
            AESoundManager.PlaySound({
                .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
                .SoundID           = soundID,
                .AudioEntity       = this,
                .Pos               = entity->GetPosition(),
                .Volume            = GetDefaultVolume(AE_GENERAL_COLLISION),
                .RollOffFactor     = 2.f,
                .FrequencyVariance = 0.0588f,
                .ClientVariable    = NO_CLIENT_VARIABLE,
            });
        };
        switch (surface) {
        case SURFACE_WOOD_CRATES:
        case SURFACE_WOOD_BENCH:
        case SURFACE_WOOD_PICKET_FENCE:
        case SURFACE_WOOD_SLATTED_FENCE:
        case SURFACE_WOOD_RANCH_FENCE:   PlaySound(CAEAudioUtility::GetRandomNumberInRange(41, 44)); break; // 0x4DAD14
        case SURFACE_GLASS:              PlaySound(11); break;                                              // 0x4DAD43
        default:                         break;
        }
    };
    if (entity->GetModelId() == MODEL_MOLOTOV) { // 0x4DAB7B - Como el original: se comprueba antes que MI_GRASSHOUSE
        PlaySoundForSurface(SURFACE_GLASS);
        return;
    }

    if (entity->GetModelId() == ModelIndices::MI_GRASSHOUSE) { // 0x4DAB94
        AESoundManager.PlaySound({ // 0x4DABEF
            .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
            .SoundID        = (eSoundID)(56),
            .AudioEntity    = this,
            .Pos            = entity->GetPosition(),
            .Volume         = GetDefaultVolume(AE_GLASS_BREAK_SLOW),
            .RollOffFactor  = 1.5f,
            .Speed          = 0.75f,
            .ClientVariable = NO_CLIENT_VARIABLE,
        });
        AESoundManager.PlaySound({ // 0x4DAC65
            .BankSlotID     = SND_BANK_SLOT_COLLISIONS,
            .SoundID        = (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(15, 18)),
            .AudioEntity    = this,
            .Pos            = entity->GetPosition(),
            .Volume         = GetDefaultVolume(AE_GLASS_HIT_GROUND_SLOW),
            .RollOffFactor  = 1.5f,
            .Speed          = 0.0f,
            .Flags          = SOUND_REQUEST_UPDATES,
            .EventID        = AE_GLASS_HIT_GROUND_SLOW,
            .ClientVariable = (float)(CTimer::GetTimeInMS() + 600),
        });
        return;
    }

    // Como el original: colisión del model info (sin el caso especial de vehículos de
    // CEntity::GetColModel) y contadores comparados con signo.
    if (const auto cd = entity->GetModelInfo()->GetColModel()->m_pColData) { // 0x4DAB89
        if ((int16)(cd->m_nNumSpheres) > 0) {
            PlaySoundForSurface(cd->m_pSpheres[0].GetSurfaceType());
        } else if ((int16)(cd->m_nNumBoxes) > 0) {
            PlaySoundForSurface(cd->m_pBoxes[0].GetSurfaceType());
        } else if ((int16)(cd->m_nNumTriangles) > 0) {
            PlaySoundForSurface(cd->m_pTriangles[0].GetSurfaceType());
        }
    }
}

// 0x4DBA10
void CAECollisionAudioEntity::ReportCollision(
    CEntity*       entityA,
    CEntity*       entityB,
    eSurfaceType   surfaceA,
    eSurfaceType   surfaceB,
    const CVector& pos,
    const CVector* normal,
    float          impulseForce,
    float          relVelSq,
    bool           isForceOneShot,
    bool           isForceLooping
) {
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
        return;
    }
    if (m_NumActiveCollisionSounds == (int32)(NUM_ENTRIES)) { // Como el original: solo descarta con exactamente 300
        return;
    }
    if (relVelSq <= 0.f) {
        return;
    }

    const auto GetSurfaceToUse = [&](
        CEntity* eA, eSurfaceType& sA,
        CEntity* eB, eSurfaceType& sB
    ) {
        if (!eA->GetIsTypeVehicle()) {      
            switch (eA->GetModelId()) {
            case MODEL_MOLOTOV:
                return SURFACE_GLASS;
            case MODEL_SATCHEL:
                return AE_SURFACE_TYPE_SATCHEL_CHARGE;
            case MODEL_GRENADE:
            case MODEL_BOMB:
            case MODEL_TEARGAS:
                return AE_SURFACE_TYPE_GRENADE;
            }
            const auto eAModelId = eA->GetModelId();
            if (eAModelId == ModelIndices::MI_BASKETBALL) {
                return AE_SURFACE_TYPE_BASKETBALL;
            }
            if (eAModelId == ModelIndices::MI_PUNCHBAG) {
                return AE_SURFACE_TYPE_PUNCHBAG;
            }
            if (eAModelId == ModelIndices::MI_GRASSHOUSE) {
                return SURFACE_UNBREAKABLE_GLASS;
            }
            if (eAModelId == ModelIndices::MI_IMY_GRAY_CRATE) {
                return SURFACE_WOOD_SOLID;
            }
            // Como el original: prueba el bit 7 del byte +0x40 (test al,al / jns), no el bit 0.
            if (eA->GetIsTypePhysical() && eA->AsPhysical()->physicalFlags.bDisableZ) {
                return AE_SURFACE_TYPE_POOL_BALL;
            }
            return sA;        
        }
        if (eB && eB->GetIsTypeBuilding() && normal && eA->GetUp().Dot(*normal) > 0.6f) {
            if (eA->AsVehicle()->IsSubBMX()) {
                return AE_SURFACE_TYPE_BMX;
            }
            if (g_surfaceInfos.GetFrictionEffect(sB) != FRICTION_EFFECT_SPARKS) {
                return SURFACE_RUBBER;
            }
        }
        if (eA->AsVehicle()->IsSubBMX()) {
            return AE_SURFACE_TYPE_BMX;
        }
        return SURFACE_CAR;
    };

    surfaceA = GetSurfaceToUse(entityA, surfaceA, entityB, surfaceB);
    surfaceB = GetSurfaceToUse(entityB, surfaceB, entityA, surfaceA);

    if (isForceOneShot) { // 0x4DBC68
        return PlayOneShotCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos);
    }
    // 0x4DBC83
    int32      entryID;
    const auto soundStatus = CAECollisionAudioEntity::GetCollisionSoundStatus(entityA, entityB, surfaceA, surfaceB, entryID);
    if (soundStatus == COLLISION_SOUND_LOOPING) { // 0x4DBCA3, 0x4DBD44
        const auto e = &m_CollisionSoundList[entryID];
        e->LoopStopTimeMs = CTimer::GetTimeInMS() + 100;
        if (e->Sound) {
            UpdateLoopingCollisionSound(e->Sound, e->EntityA, e->EntityB, e->SurfaceA, e->SurfaceB, impulseForce, pos, isForceLooping);
        }
        return;
    }
    // Como el original: cualquier otro estado con isForceLooping, o ONE_SHOT sin él, pasa a bucle; el resto, sonido puntual.
    if (isForceLooping || soundStatus == COLLISION_SOUND_ONE_SHOT) { // 0x4DBD1C, 0x4DBDE0
        return PlayLoopingCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos, isForceLooping);
    }
    PlayOneShotCollisionSound(entityA, entityB, surfaceA, surfaceB, impulseForce, pos); // 0x4DBD5A
}

// 0x4DBDF0
void CAECollisionAudioEntity::ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
        return;
    }
    if (entity && entity->GetIsTypeVehicle()) {
        PlayBulletHitCollisionSound(
            entity->AsVehicle()->IsSubBMX()
                ? AE_SURFACE_TYPE_BMX
                : SURFACE_CAR,
            posn,
            angleWithColPointNorm
        );
    } else {
        PlayBulletHitCollisionSound(surface, posn, angleWithColPointNorm);
    }
}

// 0x4DA2C0
void CAECollisionAudioEntity::Service() {
    const auto time = CTimer::GetTimeInMS();
    for (auto& entry : m_CollisionSoundList) {
        if (entry.Status != COLLISION_SOUND_LOOPING || time < entry.LoopStopTimeMs)
            continue;

        if (entry.Sound) {
            entry.Sound->StopSoundAndForget();
        }

        ClearCollisionSoundEntry(entry);
        --m_NumActiveCollisionSounds;
    }
}

// 0x4DAA50
eSoundID CAECollisionAudioEntity::ChooseCollisionSoundID(eSurfaceType surface) {
    const auto* const l = &gCollisionLookup[surface];
    if (l->MinSoundID == l->MaxSoundID) {
        return l->MinSoundID;
    }
    for (auto retry = 0; retry < 100; retry++) { // NOTSA: Handle very unlikely infinite loop
        const auto soundID = CAEAudioUtility::GetRandomNumberInRange(l->MinSoundID, l->MaxSoundID);
        if (soundID != m_CollisionSoundIDHistory[surface]) {
            return soundID;
        }
    }
    NOTSA_LOG_WARN("Failed to generate collision sound ID not in history!"); // NOTSA
    return l->MinSoundID; // NOTSA
}
