/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEWeaponAudioEntity
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 19/19 entradas.
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

#include "AEWeaponAudioEntity.h"
#include "X87Math.h"
#include "AEAudioEnvironment.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

namespace {
// Como el original: la tabla de variaciones de los aviones se lee del EXE (0x8CC29C = {1.08, 1.0}); con un índice
// -1 (módulo con signo de un byte >= 0x80) el original lee 0x8CC298.
auto& gfWeaponPlaneFrequencyVariations = StaticRef<std::array<float, 2>>(0x8CC29C);

// Atenuación que usa el original (0x3F28F5C3 = 0.66f, no 2/3).
constexpr float WEAPON_ROLL_OFF = 0.66f;
} // namespace

void CAEWeaponAudioEntity::InjectHooks() {
    // Como el original: vtable propia 0x862E68 con una sola entrada (UpdateParameters, 0x504B70); la escribe el
    // constructor 0x507560. 0x86C2AC es la de CAEPedWeaponAudioEntity.
    RH_ScopedVirtualClass(CAEWeaponAudioEntity, 0x862E68, 1);
    RH_ScopedCategory("Audio/Entities");

    // 0x507560 es el constructor de esta clase (vtable 0x862E68). 0x5DE990 es el de
    // CAEPedWeaponAudioEntity (se registra allí). El EXE no tiene destructor propio de esta
    // clase (está en línea), así que no se registra ninguno.
    RH_ScopedInstall(Constructor, 0x507560);
    RH_ScopedInstall(Initialise, 0x503450);
    RH_ScopedInstall(Reset, 0x503490);
    RH_ScopedInstall(Terminate, 0x503480);
    RH_ScopedInstall(WeaponFire, 0x504F80);
    RH_ScopedInstall(WeaponReload, 0x503690);
    RH_ScopedInstall(PlayChainsawStopSound, 0x504AA0);
    RH_ScopedInstall(PlayMiniGunStopSound, 0x504960);
    RH_ScopedInstall(PlayMiniGunFireSounds, 0x5047C0);
    RH_ScopedInstall(PlayCameraSound, 0x5046F0);
    RH_ScopedInstall(PlayWeaponLoopSound, 0x504610);
    RH_ScopedInstall(PlayFlameThrowerSounds, 0x504470);
    RH_ScopedInstall(PlayGunSounds, 0x503CE0);
    RH_ScopedInstall(ReportStealthKill, 0x503B20);
    RH_ScopedInstall(ReportChainsawEvent, 0x503910);
    RH_ScopedInstall(PlayFlameThrowerIdleGasLoop, 0x503870);
    RH_ScopedInstall(PlayGoggleSound, 0x503500);
    RH_ScopedInstall(StopFlameThrowerIdleGasLoop, 0x5034E0);
    RH_ScopedVMTInstall(UpdateParameters, 0x504B70);
}

// 0x503450
void CAEWeaponAudioEntity::Initialise() {
    m_MiniGunState  = eMiniGunState::STOPPED;
    m_ChainsawState = eChainsawState::STOPPED;
    if (!AudioEngine.IsLoadingTuneActive()) {
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN);
    }
}

// 0x503480
void CAEWeaponAudioEntity::Terminate() {
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
}

// 0x503490
void CAEWeaponAudioEntity::Reset() {
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    Clear();
}

// 0x504F80
void CAEWeaponAudioEntity::WeaponFire(eWeaponType weaponType, CPhysical* parent, eAudioEvents audioEvent) {
    if (!parent) {
        return;
    }

    switch (weaponType) {
    case WEAPON_PISTOL:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_EAGLE_SHOT_L,
            SND_GENRL_WEAPONS_EAGLE_SHOT_R,
            SND_GENRL_WEAPONS_EAGLE_TAIL_L,
            audioEvent,
            0.0f,
            1.41421f
        );
    case WEAPON_PISTOL_SILENCED:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_SILENCED_DRYFIRE,
            SND_GENRL_WEAPONS_SILENCED_LOWS,
            SND_GENRL_WEAPONS_SILENCED_SHOT_L,
            SND_GENRL_WEAPONS_SILENCED_SHOT_L,
            SND_GENRL_WEAPONS_UNK,
            audioEvent,
            -7.f
        );
    case WEAPON_DESERT_EAGLE:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_EAGLE_SHOT_L,
            SND_GENRL_WEAPONS_EAGLE_SHOT_R,
            SND_GENRL_WEAPONS_EAGLE_TAIL_L,
            audioEvent,
            0.f,
            0.94387001f
        );
    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_SHOTGUN_DRYFIRE,
            SND_GENRL_WEAPONS_SHOTGUN_LOWS,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_L,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent,
            0.f
        );
    case WEAPON_SAWNOFF_SHOTGUN:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_SHOTGUN_DRYFIRE,
            SND_GENRL_WEAPONS_SHOTGUN_LOWS,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_L,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent,
            0.0f,
            0.79369998f,
            0.93f
        );
    case WEAPON_MICRO_UZI:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_9MM1DRY,
            SND_GENRL_WEAPONS_9MM1LOWS,
            SND_GENRL_WEAPONS_9MM1SHOT_L,
            SND_GENRL_WEAPONS_9MM1SHOT_R,
            SND_GENRL_WEAPONS_9MM1TAIL,
            audioEvent
        );
    case WEAPON_MP5:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_9MM1DRY,
            SND_GENRL_WEAPONS_9MM1LOWS,
            SND_GENRL_WEAPONS_MP5SHOT_L,
            SND_GENRL_WEAPONS_MP5SHOT_R,
            SND_GENRL_WEAPONS_9MM1TAIL,
            audioEvent
        );
    case WEAPON_AK47:
    case WEAPON_M4:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_AK_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_AK_SHOT_L,
            SND_GENRL_WEAPONS_AK_SHOT_R,
            SND_GENRL_WEAPONS_AK_TAIL_L,
            audioEvent
        );
    case WEAPON_TEC9:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_9MM1DRY,
            SND_GENRL_WEAPONS_9MM1LOWS,
            SND_GENRL_WEAPONS_9MM1SHOT_L,
            SND_GENRL_WEAPONS_9MM1SHOT_R,
            SND_GENRL_WEAPONS_9MM1TAIL,
            audioEvent,
            0.0f,
            1.25992f
        );
    case WEAPON_COUNTRYRIFLE:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_SNIPER_SHOT_L,
            SND_GENRL_WEAPONS_SNIPER_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent,
            0.0f,
            0.89f
        );
    case WEAPON_SNIPERRIFLE:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_SNIPER_SHOT_L,
            SND_GENRL_WEAPONS_SNIPER_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent
        );
    case WEAPON_FLAMETHROWER: {
        if (!std::exchange(m_LastFlameThrowerFireTimeMs, CTimer::GetTimeInMS())) {
            PlayFlameThrowerSounds(
                parent,
                SND_GENRL_WEAPONS_TRIGGER,
                SND_GENRL_WEAPONS_SNIPER_SHOT_L,
                audioEvent,
                -14.f,
                1.f
            );
        }
        break;
    }
    case WEAPON_MINIGUN:
        return PlayMiniGunFireSounds(parent, audioEvent);
    case WEAPON_DETONATOR:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_DETONATOR_BEEP,
            SND_GENRL_WEAPONS_UNK,
            SND_GENRL_WEAPONS_UNK,
            SND_GENRL_WEAPONS_UNK,
            SND_GENRL_WEAPONS_UNK,
            audioEvent,
            -14.0f
        );
    case WEAPON_SPRAYCAN:
        if (!std::exchange(m_LastSprayCanFireTimeMs, CTimer::GetTimeInMS())) {
            PlayWeaponLoopSound(
                parent,
                SND_GENRL_WEAPONS_SPRAY_PAINT,
                audioEvent,
                -20.0,
                1.0f,
                AE_WEAPON_SOUND_CAT_SPRAY
            );
        }
        break;
    case WEAPON_EXTINGUISHER:
        if (!std::exchange(m_LastFireExtFireTimeMs, CTimer::GetTimeInMS())) {
            PlayWeaponLoopSound(
                parent,
                SND_GENRL_WEAPONS_FIRE_EXT,
                audioEvent,
                -20.0,
                0.79369998f,
                AE_WEAPON_SOUND_CAT_EXT
            );
        }
        break;
    case WEAPON_CAMERA:
        return PlayCameraSound(parent, audioEvent, -14.0);
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
        return PlayGoggleSound(SND_GENRL_WEAPONS_NIGHT_VISION, audioEvent);
    default:
        // 0x504F96: el original ignora cualquier tipo fuera de 22..45 y los de la
        // tabla sin sonido (cohetes, satchel).
        return;
    }
}

// 0x503690
void CAEWeaponAudioEntity::WeaponReload(eWeaponType type, CPhysical* entity, eAudioEvents event) {
    if (!entity) {
        return;
    }

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }

    // Como el original: usa m_tempSound (Initialise + RegisterWithPhysicalEntity + RequestNewSound).
    const auto PlaySound = [&](eSoundID soundID, float volumeOffsetdB = 0.f) {
        m_tempSound.Initialise(
            SND_BANK_SLOT_WEAPON_GEN,
            soundID,
            this,
            entity->GetPosition(),
            GetDefaultVolume(event) + volumeOffsetdB,
            WEAPON_ROLL_OFF,
            1.f,
            1.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            0.f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(entity);
        AESoundManager.RequestNewSound(&m_tempSound);
    };

    // Tabla 0x503838 (tipos 22..34): con AE_WEAPON_RELOAD_A (0x92) las escopetas,
    // subfusiles y AK/M4 usan el menor de cada par.
    const bool isA = event == AE_WEAPON_RELOAD_A;
    switch (type) {
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
        return PlaySound(isA ? 55 : 66);
    case WEAPON_DESERT_EAGLE:
        return PlaySound(isA ? 55 : 51);
    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
        return PlaySound(isA ? 71 : 72);
    case WEAPON_SAWNOFF_SHOTGUN:
        return PlaySound(isA ? 69 : 70);
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_TEC9:
        return PlaySound(isA ? 84 : 85);
    case WEAPON_AK47:
    case WEAPON_M4:
        return PlaySound(isA ? 31 : 32);
    case WEAPON_COUNTRYRIFLE:
        if (isA) {
            return PlaySound(32, -6.f);
        }
        break;
    case WEAPON_SNIPERRIFLE:
        return PlaySound(isA ? 55 : 32);
    }
}

// 0x503CE0
void CAEWeaponAudioEntity::PlayGunSounds(
    CPhysical*   entity,
    int16        dryFireSfxID,
    int16        subSfxID,
    int16        mainLeftSfxID,
    int16        mainRightSfxID,
    int16        tailSfxID,
    eAudioEvents audioEvent,
    float        volumeOffsetdB,
    float        primarySpeed,
    float        tailFrequencyScalingFactor
) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }

    if (CTimer::GetTimeInMS() < m_LastGunFireTimeMs + 25) {
        return;
    }
    m_LastGunFireTimeMs = CTimer::GetTimeInMS();

    const float baseVolume = GetDefaultVolume(audioEvent) + volumeOffsetdB;
    float       rollOff    = 1.f;
    float       speed      = primarySpeed;
    if (audioEvent == AE_WEAPON_FIRE_PLANE) { // 0x503DA5
        // Como el original: movsx + módulo con signo (and 80000001h), el índice se guarda como byte.
        const int8 idx = static_cast<int8>((static_cast<int8>(m_LastWeaponPlaneFrequencyIndex) + 1) % 2);
        m_LastWeaponPlaneFrequencyIndex = static_cast<uint8>(idx);
        rollOff = 1.6f;
        speed   = primarySpeed * gfWeaponPlaneFrequencyVariations.data()[idx] * 0.7937f;
    } else if (audioEvent == AE_WEAPON_FIRE_MINIGUN_PLANE) { // 0x503DD1
        rollOff = 1.8f;
        speed   = primarySpeed * 0.7937f;
    }
    const bool isPlane = audioEvent == AE_WEAPON_FIRE_PLANE || audioEvent == AE_WEAPON_FIRE_MINIGUN_PLANE;

    // El original solo sobrescribe m_Event (6) para estos dos eventos; si no, queda el -1 de Initialise.
    const bool isMiniGunEvent = audioEvent == AE_WEAPON_FIRE_MINIGUN_AMMO || audioEvent == AE_WEAPON_FIRE_MINIGUN_PLANE;

    // Como el original: cada sonido se construye en m_tempSound (doppler 1.0) y se pide con RequestNewSound.
    const auto PlaySound = [&](int16 sfxID, CVector pos, float volume, float rollOffFactor, float sfxSpeed, uint16 flags) {
        m_tempSound.Initialise(SND_BANK_SLOT_WEAPON_GEN, sfxID, this, pos, volume, rollOffFactor, sfxSpeed, 1.f, 0, flags, 0.f, 0);
        if (flags & SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY) {
            m_tempSound.RegisterWithPhysicalEntity(entity);
        }
        if (isMiniGunEvent) {
            m_tempSound.m_Event = AE_FRONTEND_PICKUP_WEAPON;
        }
        AESoundManager.RequestNewSound(&m_tempSound);
    };
    constexpr uint16 POSITIONAL_FLAGS = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES;                // 0x84
    constexpr uint16 FRONT_FLAGS      = SOUND_FORCED_FRONT | SOUND_REQUEST_UPDATES | SOUND_FRONT_END;                    // 0x1005
    constexpr uint16 TAIL_FLAGS       = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_REQUEST_UPDATES | SOUND_FRONT_END; // 0x1405

    if (dryFireSfxID != -1) { // 0x503DFF
        PlaySound(dryFireSfxID, entity->GetPosition(), baseVolume, rollOff * WEAPON_ROLL_OFF, speed, POSITIONAL_FLAGS);
    }
    if (subSfxID != -1) { // 0x503E93
        PlaySound(subSfxID, entity->GetPosition(), baseVolume, rollOff * 0.9f, speed, POSITIONAL_FLAGS);
    }

    // 0x503F1B: volumen del par frontal y del par posicional. `baseVolume` no se
    // modifica: la cola (más abajo) usa el valor sin ajustar.
    float frontEndVolume = -100.f;
    float mainVolume     = baseVolume;
    if (!isPlane) {
        const auto  rel     = CAEAudioEnvironment::GetPositionRelativeToCamera(entity);
        const float dist    = std::sqrt(rel.z * rel.z + rel.y * rel.y + rel.x * rel.x) / (rollOff * 1.25f);
        const float invRoll = 1.f / rollOff;
        const float nearLim = 5.f * invRoll;
        if (dist <= nearLim) { // 0x503F94
            mainVolume     = baseVolume - 3.f;
            frontEndVolume = CAEAudioEnvironment::GetDistanceAttenuation(dist) + mainVolume;
        } else {
            const float farLim = 12.f * invRoll;
            // Como el original: `fcomp; test ah,1; jne` también interpola si la distancia es NaN.
            if (!(dist >= farLim)) { // 0x503FE9
                // Interpolación entre ambos límites: t = 1 en nearLim y 0 en farLim.
                const float t     = (farLim - dist) / (farLim - nearLim);
                const float atten = CAEAudioEnvironment::GetDistanceAttenuation(dist);
                frontEndVolume    = (atten + AudioLog10Times20Cpp(t * 0.7071f)) + baseVolume;
                // Como el original: 0x862E14 = 0x3E95F6FE (no es el float más cercano a 0.2929).
                mainVolume        = AudioLog10Times20Cpp((1.f - t) * std::bit_cast<float>(0x3E95F6FEu) + 0.7071f) + baseVolume;
            }
        }
    }

    // 0x504068: la variación aleatoria se consulta siempre, haya o no sonidos principales.
    const float mainSpeed = (CAEAudioUtility::GetRandomNumberInRange(-0.02f, 0.02f) + 1.f) * speed;

    // Orden original: frontal izq., frontal der., posicional izq., posicional der.
    if (mainLeftSfxID != -1) { // 0x504094
        PlaySound(mainLeftSfxID, CVector{ -1.f, 0.f, 0.f }, frontEndVolume, rollOff * 1.25f, mainSpeed, FRONT_FLAGS);
    }
    if (mainRightSfxID != -1) { // 0x504103
        PlaySound(mainRightSfxID, CVector{ 1.f, 0.f, 0.f }, frontEndVolume, rollOff * 1.25f, mainSpeed, FRONT_FLAGS);
    }
    if (mainLeftSfxID != -1) { // 0x50417A
        PlaySound(mainLeftSfxID, entity->GetPosition(), mainVolume, rollOff * 1.25f, mainSpeed, POSITIONAL_FLAGS);
    }
    if (mainRightSfxID != -1) { // 0x504212
        PlaySound(mainRightSfxID, entity->GetPosition(), mainVolume, rollOff * 1.25f, mainSpeed, POSITIONAL_FLAGS);
    }

    if (tailSfxID == -1 || isPlane) { // 0x5042AA
        return;
    }
    const float scaledTail = tailFrequencyScalingFactor * 1.1892101f;
    float       leftSpeed  = tailFrequencyScalingFactor;
    float       rightSpeed = scaledTail;
    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        leftSpeed  = scaledTail;
        rightSpeed = tailFrequencyScalingFactor;
    }
    const float tailRollOff = rollOff * 3.5f;
    const float tailDist    = CAEAudioEnvironment::GetPositionRelativeToCamera(entity).Magnitude() / tailRollOff;
    const float tailRaw     = CAEAudioEnvironment::GetDistanceAttenuation(tailDist) + (baseVolume - 20.f);
    const float tailVolume  = tailRaw < 0.f ? tailRaw : 0.f; // 0x504351: nunca por encima de 0 dB (NaN -> 0)
    const int32 tailEventID = audioEvent == AE_WEAPON_FIRE_MINIGUN_AMMO
        ? AE_FRONTEND_PICKUP_MONEY
        : AE_FRONTEND_SELECT;
    const auto PlayTail = [&](CVector pos, float tailSpeed) {
        m_tempSound.Initialise(SND_BANK_SLOT_WEAPON_GEN, tailSfxID, this, pos, tailVolume, tailRollOff, tailSpeed, 1.f, 0, TAIL_FLAGS, 0.f, 0);
        m_tempSound.m_Event = tailEventID;
        AESoundManager.RequestNewSound(&m_tempSound);
    };
    PlayTail(CVector{ -1.f, 0.f, 0.f }, leftSpeed);
    PlayTail(CVector{ 1.f, 0.f, 0.f }, rightSpeed);
}

// 0x503B20
void CAEWeaponAudioEntity::ReportStealthKill(eWeaponType type, CPhysical* entity, eAudioEvents event) {
    if (type != WEAPON_KNIFE) {
        return;
    }

    // Como el original: m_tempSound con velocidad 0, evento propio y la hora como variable de cliente.
    const auto PlayStealthKillSound = [&](eSoundBankSlot bankSlotID, eSoundID soundID, float volumeOffsetdB, int32 eventID) {
        m_tempSound.Initialise(
            bankSlotID,
            soundID,
            this,
            entity->GetPosition(),
            GetDefaultVolume(event) + volumeOffsetdB,
            1.f,
            0.f,
            1.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            0.f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(entity);
        m_tempSound.m_Event          = eventID;
        m_tempSound.m_ClientVariable = (float)CTimer::GetTimeInMS();
        AESoundManager.RequestNewSound(&m_tempSound);
    };

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }
    PlayStealthKillSound(SND_BANK_SLOT_WEAPON_GEN, 81, -6.f, AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_IN);

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
        return;
    }
    PlayStealthKillSound(SND_BANK_SLOT_COLLISIONS, 47, 0.f, AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_OUT); // 0x503C8D: push 2Fh
}

// 0x503910
void CAEWeaponAudioEntity::ReportChainsawEvent(CPhysical* entity, eAudioEvents audioEvent) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_CHAINSAW_P, SND_BANK_SLOT_PLAYER_ENGINE_P, false, true)) {
        return;
    }

    const auto PlaySoundIfNotPlaying = [&](int16 sfxID, float rollOffFactor, eWeaponSoundCategories soundCategory) {
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(soundCategory, this) != SOUND_NOT_PLAYING) {
            return;
        }
        // Como el original: m_tempSound con doppler 0 y la categoría como evento.
        m_tempSound.Initialise(
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            sfxID,
            this,
            entity->GetPosition(),
            GetDefaultVolume(audioEvent),
            rollOffFactor,
            1.f,
            0.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            0.f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(entity);
        m_tempSound.m_Event = soundCategory;
        AESoundManager.RequestNewSound(&m_tempSound);
    };

    switch (audioEvent) {
    case AE_WEAPON_CHAINSAW_IDLE: { // 0x503A6E
        switch (m_ChainsawState) {
        case eChainsawState::STOPPED:
        case eChainsawState::IDLE: {
            PlaySoundIfNotPlaying(1, WEAPON_ROLL_OFF, AE_WEAPON_SOUND_CAT_CHAINSAW_IDLE);

            m_ChainsawState           = eChainsawState::IDLE;
            m_LastChainsawEventTimeMs = CTimer::GetTimeInMS();
        }
        }
        break;
    }
    case AE_WEAPON_CHAINSAW_ACTIVE: { // 0x5039C0
        switch (m_ChainsawState) {
        case eChainsawState::STOPPING:
        case eChainsawState::ACTIVE:
        case eChainsawState::IDLE: {
            PlaySoundIfNotPlaying(0, 1.25f, AE_WEAPON_SOUND_CAT_CHAINSAW_ACTIVE);

            m_ChainsawState           = eChainsawState::ACTIVE;
            m_LastChainsawEventTimeMs = CTimer::GetTimeInMS();
        }
        }
        break;
    }
    case AE_WEAPON_CHAINSAW_CUTTING: { // 0x503996
        switch (m_ChainsawState) {
        case eChainsawState::ACTIVE:
        case eChainsawState::CUTTING: {
            m_ChainsawState           = eChainsawState::CUTTING;
            m_LastChainsawEventTimeMs = CTimer::GetTimeInMS();
        }
        }
        break;
    }
    }
}

// 0x504610
void CAEWeaponAudioEntity::PlayWeaponLoopSound(CPhysical* entity, int16 sfxId, eAudioEvents startEvent, float volumeOffsetdB, float speed, eWeaponSoundCategories soundCat) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }
    // Como el original: m_tempSound (flags 0xA4) con la categoría como evento.
    m_tempSound.Initialise(
        SND_BANK_SLOT_WEAPON_GEN,
        sfxId,
        this,
        entity->GetPosition(),
        GetDefaultVolume(startEvent) + volumeOffsetdB,
        1.f,
        speed,
        1.f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES,
        0.f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_tempSound.m_Event = soundCat;
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x504960
void CAEWeaponAudioEntity::PlayMiniGunStopSound(CPhysical* entity) {
    if (!entity) {
        m_MiniGunState = eMiniGunState::STOPPED;
        return;
    }

    if (m_MiniGunState == eMiniGunState::STOPPING) {
        return;
    }

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }

    // 0x5049EE: en aviones la atenuación base es 1.8 y la velocidad 0.7937.
    const bool  isPlane = entity->GetIsTypeVehicle() && entity->AsVehicle()->IsSubPlane();
    const float rollOff = (isPlane ? 1.8f : 1.f) * WEAPON_ROLL_OFF;
    const float speed   = isPlane ? 0.7937f : 1.f;
    m_tempSound.Initialise(
        SND_BANK_SLOT_WEAPON_GEN,
        SND_GENRL_WEAPONS_MINIGUNOFF,
        this,
        entity->GetPosition(),
        GetDefaultVolume(AE_WEAPON_FIRE_MINIGUN_STOP),
        rollOff,
        speed,
        1.f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
        0.f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_tempSound.m_Event = AE_WEAPON_SOUND_CAT_MINIGUN_STOP;
    AESoundManager.RequestNewSound(&m_tempSound);
    m_MiniGunState = eMiniGunState::STOPPING;
}

// 0x5047C0
void CAEWeaponAudioEntity::PlayMiniGunFireSounds(CPhysical* entity, eAudioEvents audioEvent) {
    // Como el original: PlayGunSounds se llama antes de marcar el disparo activo y el estado.
    const auto PlayMiniGunFireSound = [&](eAudioEvents gunFireAE) {
        if (!m_IsMiniGunFireActive) {
            PlayGunSounds(entity, 15, 16, 11, 12, 13, gunFireAE, 0.f, 1.f, 1.f);
            m_IsMiniGunFireActive = true;
        }
        m_MiniGunState = eMiniGunState::FIRING;
    };
    switch (audioEvent) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_MINIGUN_AMMO: { // 0x504912
        PlayMiniGunFireSound(AE_WEAPON_FIRE_MINIGUN_AMMO);
        break;
    }
    case AE_WEAPON_FIRE_PLANE: { // 0x5047E6
        PlayMiniGunFireSound(AE_WEAPON_FIRE_MINIGUN_PLANE);
        break;
    }
    case AE_WEAPON_FIRE_MINIGUN_NO_AMMO: { // 0x504823 - Minigun is just spinning
        if (!m_IsMiniGunSpinActive) {
            if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
                if (!AudioEngine.IsLoadingTuneActive()) {
                    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN);
                }
                return; // 0x50484F/0x50486D: sale sin tocar el estado ni m_LastMiniGunFireTimeMs.
            }
            m_tempSound.Initialise(
                SND_BANK_SLOT_WEAPON_GEN,
                SND_GENRL_WEAPONS_MINIGUNEMPTY,
                this,
                entity->GetPosition(),
                GetDefaultVolume(AE_WEAPON_FIRE_MINIGUN_NO_AMMO),
                WEAPON_ROLL_OFF,
                1.f,
                0.f,
                0,
                SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
                0.f,
                0
            );
            m_tempSound.RegisterWithPhysicalEntity(entity);
            m_tempSound.m_Event = AE_WEAPON_SOUND_CAT_MINIGUN_SPIN;
            AESoundManager.RequestNewSound(&m_tempSound);
            m_IsMiniGunSpinActive = true;
        }
        m_MiniGunState = eMiniGunState::SPINNING;
        break;
    }
    }
    m_LastMiniGunFireTimeMs = CTimer::GetTimeInMS();
}

// 0x503500
void CAEWeaponAudioEntity::PlayGoggleSound(int16 sfxId, eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }
    const auto r = CAEAudioUtility::ResolveProbability(0.5f);
    // Como el original: m_tempSound con flags 0 en Initialise y después m_Flags = 0x1001 (mismo resultado).
    const auto PlaySound = [&](float offsetX, float speed) {
        m_tempSound.Initialise(
            SND_BANK_SLOT_WEAPON_GEN,
            sfxId,
            this,
            CVector{ offsetX, 0.0f, 0.0f },
            GetDefaultVolume(event) - 9.f,
            1.f,
            speed,
            1.f,
            0,
            SOUND_FORCED_FRONT | SOUND_FRONT_END,
            0.f,
            0
        );
        AESoundManager.RequestNewSound(&m_tempSound);
    };
    PlaySound(-1.0f, r ? 1.1892101f : 1.f);
    PlaySound(+1.0f, r ? 1.f : 1.1892101f);
}

// 0x504470
void CAEWeaponAudioEntity::PlayFlameThrowerSounds(CPhysical* entity, eSoundID dryFireSfxID, eSoundID flameSfxID, eAudioEvents audioEvent, float volumeOffsetdB, float speed) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }
    const float volume = GetDefaultVolume(audioEvent) + volumeOffsetdB;
    if (dryFireSfxID != -1) { // 0x5044E1: sin evento (queda el -1 de Initialise)
        m_tempSound.Initialise(
            SND_BANK_SLOT_WEAPON_GEN,
            dryFireSfxID,
            this,
            entity->GetPosition(),
            volume - 6.f,
            WEAPON_ROLL_OFF,
            speed,
            1.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            0.02f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(entity);
        AESoundManager.RequestNewSound(&m_tempSound);
    }

    // 0x504551: el banco de vehículos no mira la melodía de carga.
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }
    if (flameSfxID != -1) { // 0x504588
        m_tempSound.Initialise(
            SND_BANK_SLOT_VEHICLE_GEN,
            flameSfxID,
            this,
            entity->GetPosition(),
            volume - 20.f,
            2.f,
            speed * 0.79369998f,
            1.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            0.f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(entity);
        m_tempSound.m_Event = AE_WEAPON_SOUND_CAT_FLAME;
        AESoundManager.RequestNewSound(&m_tempSound);
    }
}

// 0x503870
void CAEWeaponAudioEntity::PlayFlameThrowerIdleGasLoop(CPhysical* entity) {
    if (m_FlameThrowerIdleGasLoopSound != nullptr) {
        return;
    }
    // Como el original: m_tempSound sin evento; se guarda la ranura que devuelve RequestNewSound.
    m_tempSound.Initialise(
        SND_BANK_SLOT_WEAPON_GEN,
        SND_GENRL_WEAPONS_GAS_LOOP,
        this,
        entity->GetPosition(),
        GetDefaultVolume(AE_FLAMETHROWER_IDLE),
        WEAPON_ROLL_OFF,
        1.f,
        1.f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
        0.f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_FlameThrowerIdleGasLoopSound = AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x5034E0
void CAEWeaponAudioEntity::StopFlameThrowerIdleGasLoop() {
    if (auto* const s = m_FlameThrowerIdleGasLoopSound) {
        s->StopSoundAndForget();
        m_FlameThrowerIdleGasLoopSound = nullptr;
    }
}

// 0x504AA0
void CAEWeaponAudioEntity::PlayChainsawStopSound(CPhysical* entity) {
    if (entity && AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_CHAINSAW_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        if (m_ChainsawState != eChainsawState::STOPPING) {
            m_tempSound.Initialise(
                SND_BANK_SLOT_PLAYER_ENGINE_P,
                2,
                this,
                entity->GetPosition(),
                GetDefaultVolume(AE_WEAPON_CHAINSAW_ACTIVE),
                WEAPON_ROLL_OFF,
                1.f,
                1.f,
                0,
                SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
                0.f,
                0
            );
            m_tempSound.RegisterWithPhysicalEntity(entity);
            m_tempSound.m_Event = AE_WEAPON_SOUND_CAT_CHAINSAW_STOP;
            AESoundManager.RequestNewSound(&m_tempSound);
        }
        m_ChainsawState = eChainsawState::STOPPING;
    } else {
        m_ChainsawState = eChainsawState::STOPPED;
    }
}

// 0x5046F0
void CAEWeaponAudioEntity::PlayCameraSound(CPhysical* entity, eAudioEvents event, float audability) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }
    // 0x504752: el original suma `audability` y no asigna m_Event (queda el -1 de Initialise).
    m_tempSound.Initialise(
        SND_BANK_SLOT_WEAPON_GEN,
        SND_GENRL_WEAPONS_CAMERA,
        this,
        entity->GetPosition(),
        GetDefaultVolume(event) + audability,
        WEAPON_ROLL_OFF,
        1.f,
        1.f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_IS_PAUSABLE, // NOTE/BUG: `UNPAUSABLE` typo?
        0.f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(entity);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x504B70
void CAEWeaponAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    const auto now = CTimer::GetTimeInMS(); // 0x504B72: se lee una sola vez al entrar.
    if (curPlayPos < 0) { // 0x504B85: cualquier negativo sale; solo -1 tiene efectos.
        if (curPlayPos == -1) {
            if (sound == m_FlameThrowerIdleGasLoopSound) {
                m_FlameThrowerIdleGasLoopSound = nullptr;
            } else if (sound->m_Event == AE_WEAPON_SOUND_CAT_MINIGUN_STOP && m_MiniGunState == eMiniGunState::STOPPING) {
                m_MiniGunState = eMiniGunState::STOPPED;
            }
        }
        return;
    }

    // Los límites solo se aplican si el valor aún no los ha alcanzado (fcom + salto):
    // un valor que ya estaba fuera del límite se conserva, como en el original.
    switch (sound->m_Event) {
    case AE_WEAPON_SOUND_CAT_TAIL: { // 0x504BA4
        if (!CGame::CanSeeOutSideFromCurrArea()) {
            sound->m_Volume -= 1.f;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_FLAME: { // 0x504BC3
        if (m_LastFlameThrowerFireTimeMs + 300 < now) {
            sound->StopSoundAndForget();
            m_LastFlameThrowerFireTimeMs = 0;
        } else {
            const float target = GetDefaultVolume(AE_WEAPON_FIRE) - 14.f;
            if (sound->m_Volume < target) {
                const float v   = sound->m_Volume + 2.f; // TODO: Use TimeStep
                sound->m_Volume = v < target ? v : target;
            }
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_SPRAY: { // 0x504C38
        if (m_LastSprayCanFireTimeMs + 300 < now) {
            sound->StopSoundAndForget();
            m_LastSprayCanFireTimeMs = 0;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_EXT: { // 0x504C5F
        if (m_LastFireExtFireTimeMs + 300 < now) {
            sound->StopSoundAndForget();
            m_LastFireExtFireTimeMs = 0;
        } else if (sound->m_Speed < 0.85f) {
            const float s  = sound->m_Speed + 0.01f; // TODO: Use TimeStep
            sound->m_Speed = s < 0.85f ? s : 0.85f;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_MINIGUN_SPIN: { // 0x504CBC
        if (m_LastMiniGunFireTimeMs + 300 < now) {
            PlayMiniGunStopSound(sound->m_PhysicalEntity->AsPhysical());
        }
        // Como el original: cualquier estado distinto de SPINNING/FIRING (no solo STOPPING/STOPPED) para el sonido.
        if (m_MiniGunState != eMiniGunState::SPINNING && m_MiniGunState != eMiniGunState::FIRING) {
            sound->StopSoundAndForget();
            m_IsMiniGunSpinActive = false;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_MINIGUN_FIRE: { // 0x504CFB
        if (m_LastMiniGunFireTimeMs + 300 < now) {
            PlayMiniGunStopSound(sound->m_PhysicalEntity->AsPhysical());
        }
        if (m_MiniGunState != eMiniGunState::FIRING) {
            sound->StopSoundAndForget();
            m_IsMiniGunFireActive = false;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_MINIGUN_TAIL: { // 0x504D31
        if (m_MiniGunState != eMiniGunState::FIRING) {
            // Como el original: `test ah,41h; jne` también para el sonido con volumen NaN.
            if (!(sound->m_Volume > -30.0f)) {
                sound->StopSoundAndForget();
            } else {
                sound->m_Volume -= 1.5f;
            }
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_CHAINSAW_IDLE: { // 0x504D6B
        if (m_LastChainsawEventTimeMs + 300 < now) {
            m_ChainsawState = eChainsawState::STOPPED;
        }
        if (m_ChainsawState != eChainsawState::IDLE) {
            sound->StopSoundAndForget();
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_CHAINSAW_ACTIVE: { // 0x504D9B
        switch (m_ChainsawState) {
        case eChainsawState::CUTTING: { // 0x504DA5
            if (m_LastChainsawEventTimeMs + 400 < now) {
                m_LastChainsawEventTimeMs = now;
                m_ChainsawState           = eChainsawState::ACTIVE;
            } else if (sound->m_Speed > 0.85f) {
                const float s  = sound->m_Speed - 0.15f; // TODO: Use TimeStep
                sound->m_Speed = s > 0.85f ? s : 0.85f;
            }
            break;
        }
        case eChainsawState::ACTIVE: { // 0x504E0E
            if (sound->m_Speed < 1.f) {
                const float s  = sound->m_Speed + 0.03f; // TODO: Use TimeStep
                sound->m_Speed = s < 1.f ? s : 1.f;
            }
            if (m_LastChainsawEventTimeMs + 300 < now) {
                PlayChainsawStopSound(sound->m_PhysicalEntity->AsPhysical());
            }
            break;
        }
        default: {
            sound->StopSoundAndForget();
            break;
        }
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_CHAINSAW_STOP: { // 0x504E66
        if (m_ChainsawState == eChainsawState::STOPPING && curPlayPos > 1'000) {
            m_ChainsawState = eChainsawState::IDLE;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_IN: { // 0x504E8B: _ftol2 (64 bits, parte baja) y comparación con signo
        if (static_cast<int32>(static_cast<uint32>(static_cast<int64>(sound->m_ClientVariable)) + 820u) < static_cast<int32>(now)) {
            sound->m_Speed = 0.84f;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_OUT: { // 0x504EAE
        if (static_cast<int32>(static_cast<uint32>(static_cast<int64>(sound->m_ClientVariable)) + 2200u) < static_cast<int32>(now)) {
            sound->m_Speed = 1.f;
        }
        break;
    }
    default: { // 0x504ECD
        if (sound->m_Volume > 0.0f) {
            sound->m_Volume = std::max(0.0f, sound->m_Volume - 2.5f);
        }
        break;
    }
    }
}

// 0x5034A0: el original solo pone a cero estos campos. Conserva m_MiniGunState,
// m_ChainsawState y toda la parte CAEAudioEntity (la versión anterior reasignaba el
// objeto completo con `*this = CAEWeaponAudioEntity{}`).
void CAEWeaponAudioEntity::Clear() {
    m_LastFlameThrowerFireTimeMs    = 0;
    m_LastSprayCanFireTimeMs        = 0;
    m_LastFireExtFireTimeMs         = 0;
    m_FlameThrowerIdleGasLoopSound  = nullptr;
    m_LastWeaponPlaneFrequencyIndex = 0;
    m_LastMiniGunFireTimeMs         = 0;
    m_IsMiniGunSpinActive           = false;
    m_IsMiniGunFireActive           = false;
    m_LastChainsawEventTimeMs       = 0;
    m_LastGunFireTimeMs             = 0;
}

// 0x507560
CAEWeaponAudioEntity::CAEWeaponAudioEntity() {
    // Como el original: solo pone a cero los campos de Clear(); m_MiniGunState y m_ChainsawState
    // quedan sin escribir. (La parte CAEAudioEntity de la DLL sigue inicializando m_Entity y todo
    // m_tempSound; el EXE solo escribe m_tempSound.m_PhysicalEntity: ver Constructor().)
    Clear();
}

// 0x507560: envoltorio del hook. Construye en el sitio exactamente como el EXE, sin pasar por los
// inicializadores de CAEAudioEntity/CAESound: m_tempSound.m_PhysicalEntity (+0x10) = nullptr, la
// vtable del EXE (0x862E68) y los campos de Clear(). El resto de bytes no se tocan.
CAEWeaponAudioEntity* CAEWeaponAudioEntity::Constructor() {
    auto* const self = reinterpret_cast<uint8*>(this);
    *reinterpret_cast<uint32*>(self + 0x10) = 0;
    *reinterpret_cast<uintptr_t*>(self) = 0x862E68;
    Clear();
    return this;
}
