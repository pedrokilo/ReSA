/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEPedAudioEntity
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEPedAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

void CAEPedAudioEntity::InjectHooks() {
    // Como el original: vtable 0x86C2A8 con una sola entrada (UpdateParameters 0x4E1180).
    RH_ScopedVirtualClass(CAEPedAudioEntity, 0x86C2A8, 1);
    RH_ScopedCategory("Audio/Entities");

    //RH_ScopedInstall(Constructor, 0x5DE8D0, { .reversed = false });

    RH_ScopedInstall(Initialise, 0x4E0E80);
    RH_ScopedInstall(StaticInitialise, 0x5B98A0);
    RH_ScopedInstall(Terminate, 0x4E1360);
    RH_ScopedInstall(AddAudioEvent, 0x4E2BB0);
    RH_ScopedInstall(TurnOnJetPack, 0x4E28A0);
    RH_ScopedInstall(TurnOffJetPack, 0x4E2A70);
    RH_ScopedInstall(StopJetPackSound, 0x4E1120);
    RH_ScopedInstall(UpdateJetPack, 0x4E0EE0);
    RH_ScopedInstall(PlayWindRush, 0x4E1170);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E1180); // virtual: un hook plano entraría en bucle infinito
    RH_ScopedInstall(HandleFootstepEvent, 0x4E13A0);
    RH_ScopedInstall(HandleSkateEvent, 0x4E17E0);
    RH_ScopedInstall(HandleLandingEvent, 0x4E18E0);
    RH_ScopedInstall(HandlePedSwing, 0x4E1A40);
    RH_ScopedInstall(HandlePedHit, 0x4E1CC0);
    RH_ScopedInstall(HandlePedJacked, 0x4E2350);
    RH_ScopedInstall(HandleSwimSplash, 0x4E26A0);
    RH_ScopedInstall(HandleSwimWake, 0x4E2790);
    RH_ScopedInstall(PlayShirtFlap, 0x4E2A90);
    RH_ScopedInstall(Service, 0x4E2EE0);
}

// 0x5DE8D0
CAEPedAudioEntity::CAEPedAudioEntity() : CAEAudioEntity() {
    m_pPed = nullptr;
    m_bCanAddEvent = false;

    m_JetPackSound0 = nullptr;
    m_JetPackSound1 = nullptr;
    m_JetPackSound2 = nullptr;
}

// (CEntity* entity)
// 0x4E0E80
void CAEPedAudioEntity::Initialise(CPed* ped) {
    m_pPed = ped;
    m_nSfxId = 0;
    m_LastSwimWakeTriggerTimeMs = 0;

    m_bJetPackPlaying = false;
    m_JetPackSound0 = nullptr;
    m_JetPackSound1 = nullptr;
    m_fVolume1 = -100.0f;
    m_fVolume2 = -100.0f;

    field_150 = nullptr;
    field_154 = -100.0f;
    field_158 = -100.0f;
    m_bCanAddEvent = true;
}

// 0x5B98A0
void CAEPedAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING);
}

// 0x4E1360
void CAEPedAudioEntity::Terminate() {
    m_bCanAddEvent = false;
    m_pPed   = nullptr;
    StopJetPackSound();
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    if (m_sTwinLoopSoundEntity.IsActive()) {
        m_sTwinLoopSoundEntity.StopSoundAndForget();
    }
}

// 0x4E2BB0
void CAEPedAudioEntity::AddAudioEvent(eAudioEvents event, float volume, float speed, CPhysical* ped, eSurfaceType surfaceId, int32 a7, uint32 maxVol) {
    if (!m_bCanAddEvent)
        return;

    if (!m_pPed)
        return;

    switch (event) {
    case AE_PED_FOOTSTEP_LEFT:
    case AE_PED_FOOTSTEP_RIGHT:
        HandleFootstepEvent(event, volume, speed, surfaceId);
        break;
    case AE_PED_SKATE_LEFT:
    case AE_PED_SKATE_RIGHT:
        HandleSkateEvent(event, volume, speed);
        break;
    case AE_PED_LAND_ON_FEET_AFTER_FALL:
    case AE_PED_COLLAPSE_AFTER_FALL:
        HandleLandingEvent(event);
        break;
    case AE_PED_SWING:
        HandlePedSwing(event, a7, maxVol);
        break;
    case AE_PED_HIT_HIGH:
    case AE_PED_HIT_LOW:
    case AE_PED_HIT_GROUND:
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK:
        HandlePedHit(event, ped, surfaceId, volume, maxVol);
        break;
    case AE_PED_JACKED_CAR_PUNCH:
    case AE_PED_JACKED_CAR_HEAD_BANG:
    case AE_PED_JACKED_CAR_KICK:
    case AE_PED_JACKED_BIKE:
    case AE_PED_JACKED_DOZER:
        HandlePedJacked(event);
        break;
    case AE_PED_SWIM_STROKE_SPLASH:
    case AE_PED_SWIM_DIVE_SPLASH:
        HandleSwimSplash(event);
        break;
    case AE_PED_SWIM_WAKE:
        HandleSwimWake(event);
        break;
    case AE_PED_CRUNCH: {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }

        volume += GetDefaultVolume(AE_PED_CRUNCH);

        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_CRUNCH, this)) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = 29,
            .AudioEntity       = this,
            .Pos               = ped->GetPosition(),
            .Volume            = volume,
            .RollOffFactor     = 1.5f,
            .Speed             = speed,
            .Doppler           = 1.0f,
            .FrameDelay        = 0,
            .Flags             = SOUND_DEFAULT,
            .FrequencyVariance = 0.06f,
            .PlayTime          = 0,
            .EventID           = AE_PED_CRUNCH
        });

        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_KNOCK_DOWN, this) != 0) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(47, 49)),
            .AudioEntity       = this,
            .Pos               = ped->GetPosition(),
            .Volume            = volume,
            .RollOffFactor     = 1.5f,
            .Speed             = speed,
            .Doppler           = 1.0f,
            .FrameDelay        = 0,
            .Flags             = SOUND_DEFAULT,
            .FrequencyVariance = 0.06f,
            .PlayTime          = 0,
            .EventID           = AE_PED_KNOCK_DOWN
        });
        break;
    }
    default:
        break;
    }
}

// 0x4E28A0
void CAEPedAudioEntity::TurnOnJetPack() {
    if (!m_pPed)
        return;

    if (m_bJetPackPlaying || m_JetPackSound1 || m_JetPackSound0 || m_JetPackSound2)
        return;

    m_fVolume1 = -100.0f;
    m_fVolume2 = -100.0f;
    m_fVolume3 = +2.000f;
    m_JetPackSoundSpeedMult = 0.400f;

    m_bJetPackPlaying = true;

    const auto PlayJetPackSound = [&](eSoundBankSlot slot, eSoundID soundID) {
        return AESoundManager.PlaySound({
            .BankSlotID  = slot,
            .SoundID     = soundID,
            .AudioEntity = this,
            .Pos         = m_pPed->GetPosition(),
            .Volume      = -100.f,
            .Flags       = SOUND_REQUEST_UPDATES,
        });
    };
    m_JetPackSound0 = PlayJetPackSound(SND_BANK_SLOT_VEHICLE_GEN, 26);
    m_JetPackSound1 = PlayJetPackSound(SND_BANK_SLOT_WEAPON_GEN, 10);
    m_JetPackSound2 = PlayJetPackSound(SND_BANK_SLOT_FRONTEND_GAME, 0);
}

// 0x4E2A70
void CAEPedAudioEntity::TurnOffJetPack() {
    StopJetPackSound();
    m_bJetPackPlaying = false;
}

// 0x4E1120
void CAEPedAudioEntity::StopJetPackSound() {
    if (m_JetPackSound0) {
        m_JetPackSound0->StopSoundAndForget();
        m_JetPackSound0 = nullptr;
    }

    if (m_JetPackSound1) {
        m_JetPackSound1->StopSoundAndForget();
        m_JetPackSound1 = nullptr;
    }

    if (m_JetPackSound2) {
        m_JetPackSound2->StopSoundAndForget();
        m_JetPackSound2 = nullptr;
    }
}

// 0x4E0EE0
void CAEPedAudioEntity::UpdateJetPack(float thrustFwd, float thrustAngle) {
    if (!m_bJetPackPlaying || !m_JetPackSound1 || !m_JetPackSound0 || !m_JetPackSound2) {
        return;
    }

    if (thrustFwd <= 0.5f) { // flying
        m_fVolume1 = std::max(m_fVolume1 - 5.0f, -100.0f);
        m_fVolume2 = std::min(m_fVolume2 + 6.0f, -17.0f);
        m_fVolume3 = std::max(m_fVolume3 - 0.3f, 2.0f);
        m_JetPackSoundSpeedMult = std::max(m_JetPackSoundSpeedMult - 0.031f, 0.4f);
    } else { // idle
        m_fVolume1 = std::min(m_fVolume1 + 15.0f, -15.0f);
        m_fVolume2 = std::max(m_fVolume2 - 7.1f, -100.0f);
        m_fVolume3 = std::min(m_fVolume3 + 0.3f, 11.0f);
        m_JetPackSoundSpeedMult = std::min(m_JetPackSoundSpeedMult + 0.031f, 0.71f);
    }

    const auto angle = std::sin(thrustAngle);
    const float speed = angle < 0.0f ? -angle : angle; // maybe wrong

    // 0.0f == 0xB61384 (uninitialized)
    m_JetPackSound0->m_Volume = m_fVolume1 + 0.0f;
    m_JetPackSound0->m_Speed = speed * -0.07f + 1.0f;

    m_JetPackSound1->m_Volume = m_fVolume2 + 0.0f;
    m_JetPackSound1->m_Speed = 0.56f;

    m_JetPackSound2->m_Volume = m_fVolume3 + 0.0f;
    m_JetPackSound2->m_Speed = (speed / 5.0f + 1.0f) * m_JetPackSoundSpeedMult;
}

// 0x4E1170
void CAEPedAudioEntity::PlayWindRush(float, float) {
    // NOP
}

// 0x4E1180
void CAEPedAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (sound == m_JetPackSound1) {
        if (curPlayPos == -1) {
            m_JetPackSound1 = nullptr;
            return;
        }
        if (m_pPed) {
            sound->SetPosition(m_pPed->GetPosition());
        }
        return;
    }

    if (sound == m_JetPackSound0) {
        if (curPlayPos == -1) {
            m_JetPackSound0 = nullptr;
            return;
        }
        if (m_pPed) {
            sound->SetPosition(m_pPed->GetPosition());
        }
        return;
    }

    if (sound == m_JetPackSound2) {
        if (curPlayPos == -1) {
            m_JetPackSound2 = nullptr;
            return;
        }
        if (m_pPed) {
            sound->SetPosition(m_pPed->GetPosition());
        }
        return;
    }

    // shit
    if (sound == field_150) {
        if (curPlayPos == -1) {
            field_150 = nullptr;
        }
        return;
    }

    if (curPlayPos < 0)
        return;

    switch (sound->m_Event) {
    case AE_PED_SWING:
    case AE_PED_HIT_HIGH:
    case AE_PED_HIT_LOW:
    case AE_PED_HIT_GROUND:
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK:
    case AE_PED_JACKED_CAR_PUNCH:
    case AE_PED_JACKED_CAR_HEAD_BANG:
    case AE_PED_JACKED_CAR_KICK:
    case AE_PED_JACKED_BIKE:
    case AE_PED_JACKED_DOZER:
        // Como el original: _ftol2 (64 bits, parte baja) y comparación sin signo.
        if (CTimer::GetTimeInMS() < static_cast<uint32>(static_cast<int64>(sound->m_ClientVariable)))
            return;
        sound->m_Speed = 1.0f;
        return;
    case AE_PED_SWIM_WAKE: {
        const auto volume = GetDefaultVolume(AE_PED_SWIM_WAKE);

        if (CTimer::GetTimeInMS() <= m_LastSwimWakeTriggerTimeMs + 100) {
            // Como el original: sube 0.6 dB por llamada hasta el volumen por defecto (NaN o ya alcanzado: no toca nada).
            if (!(sound->m_Volume < volume)) {
                return;
            }
            const float up = sound->m_Volume + 0.6f;
            sound->m_Volume = up < volume ? up : volume;
        } else {
            // Como el original: por debajo (o igual, o NaN) de volumen - 20 se detiene; si no, baja 0.6 dB sin pasar de ahí.
            const float vol = volume - 20.0f;
            if (!(sound->m_Volume > vol)) {
                sound->StopSoundAndForget();
                m_LastSwimWakeTriggerTimeMs = 0;
                return;
            }
            const float down = sound->m_Volume - 0.6f;
            sound->m_Volume = down > vol ? down : vol;
        }

        return;
    }
    default:
        return;
    }
}

// 0x4E13A0
void CAEPedAudioEntity::HandleFootstepEvent(eAudioEvents event, float volume, float speed, eSurfaceType forcedSurfaceType) {
    volume += GetDefaultVolume(event);

    if (m_pPed->bIsInTheAir) {
        return;
    }

    const auto PlayFootstepSound = [&] (eSoundBank bank, eSoundBankSlot slot, eSoundID sfx, int16 playTime = 0) {
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_FEET_GENERIC, slot)) {
            AESoundManager.PlaySound({
                .BankSlotID         = slot,
                .SoundID            = sfx,
                .AudioEntity        = this,
                .Pos                = m_pPed->GetPosition(),
                .Volume             = volume,
                .Speed              = speed,
                .Flags              = SOUND_START_PERCENTAGE,
                .FrequencyVariance  = 0.0588f,
                .PlayTime           = playTime,
                .RegisterWithEntity = m_pPed,
            });
        } else {
            AEAudioHardware.LoadSoundBank(bank, slot);
        }
    };

    const auto PlayRandomGenericFootstepSound = [&] () {
        PlayFootstepSound(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(1, 5)));
    };

    if (FindPlayerPed(-1) != m_pPed) {
        PlayRandomGenericFootstepSound();
    } else {
        const auto TryPlayPlayerFootstepSound = [&] (eSoundBank bank, eSoundBankSlot slot, eSoundID sfx, bool needCancelSoundsInSlot = false, int16 playTime = 0) {
            if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
                PlayFootstepSound(bank, slot, sfx);
                return true;
            } else {
                if (needCancelSoundsInSlot) {
                    if (AESoundManager.AreSoundsPlayingInBankSlot(slot) - 1 <= 1) {
                        AESoundManager.CancelSoundsInBankSlot(slot, false);
                    }
                }
                AEAudioHardware.LoadSoundBank(bank, slot);
                PlayRandomGenericFootstepSound();
                return false;
            }
        };
        if (g_surfaceInfos.IsAudioWater(forcedSurfaceType) || g_surfaceInfos.IsAudioWater(m_pPed->m_nContactSurface)) { // 0x4E1703
            TryPlayPlayerFootstepSound(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), false, 50);
        } else if (g_surfaceInfos.IsAudioConcrete(m_pPed->m_nContactSurface)) { // 0x4E145E
            PlayRandomGenericFootstepSound();
        } else if (g_surfaceInfos.IsAudioGrass(m_pPed->m_nContactSurface) || g_surfaceInfos.IsAudioLongGrass(m_pPed->m_nContactSurface)) { // 0x4E16BA
            TryPlayPlayerFootstepSound(SND_BANK_FEET_GRASS, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioSand(m_pPed->m_nContactSurface)) { // 0x4E14B7
            TryPlayPlayerFootstepSound(SND_BANK_FEET_SAND, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 3)), true);
        } else if (g_surfaceInfos.IsAudioGravel(m_pPed->m_nContactSurface)) { // 0x4E1531
            TryPlayPlayerFootstepSound(SND_BANK_FEET_GRAVEL, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioWood(m_pPed->m_nContactSurface)) { // 0x4E157D
            TryPlayPlayerFootstepSound(SND_BANK_FEET_WOOD, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioTile(m_pPed->m_nContactSurface)) { // 0x4E15DB
            TryPlayPlayerFootstepSound(SND_BANK_FEET_TILE, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioMetal(m_pPed->m_nContactSurface)) { // 0x4E1636
            TryPlayPlayerFootstepSound(SND_BANK_FEET_METAL, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else { // 0x4E168F
            PlayRandomGenericFootstepSound();
        }
    }
}

// 0x4E17E0
void CAEPedAudioEntity::HandleSkateEvent(eAudioEvents event, float volume, float speed) {
    if (m_pPed->bIsInTheAir) {
        return;
    }
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
        return;
    }
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_FOOTSTEPS_GENERIC,
        .SoundID            = (eSoundID)(event == AE_PED_SKATE_LEFT ? 7 : 8),
        .AudioEntity        = this,
        .Pos                = m_pPed->GetPosition(),
        .Volume             = GetDefaultVolume(event) + (float)(CAEAudioUtility::GetRandomNumberInRange(-3, 3)) + volume,
        .Speed              = speed,
        .RegisterWithEntity = m_pPed,
    });
}

// 0x4E18E0
void CAEPedAudioEntity::HandleLandingEvent(eAudioEvents event) {
    if (m_pPed->bIsInTheAir) {
        return;
    }

    const auto PlayLandingSound = [&](eSoundBankSlot slot, eSoundID soundID, float volume, int16 playPos) {
        AESoundManager.PlaySound({
            .BankSlotID = slot,
            .SoundID            = soundID,
            .AudioEntity        = this,
            .Pos                = m_pPed->GetPosition(),
            .Volume             = volume,
            .Flags              = SOUND_START_PERCENTAGE,
            .FrequencyVariance  = 0.0588f,
            .PlayTime           = playPos,
            .RegisterWithEntity = m_pPed
        });
    };
    const auto volume = GetDefaultVolume(event);
    if (g_surfaceInfos.IsAudioWater(m_pPed->m_nContactSurface)) {
        if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
            PlayLandingSound(
                SND_BANK_SLOT_SWIMMING,
                CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_SWIMMING_SWIM1, SND_GENRL_SWIMMING_SWIM5),
                std::max(0.f, volume),
                50
            );
        }
    } else {
        if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
            PlayLandingSound(
                SND_BANK_SLOT_FOOTSTEPS_GENERIC,
                event == AE_PED_LAND_ON_FEET_AFTER_FALL ? 6 : 0,
                volume,
                0
            );
        }
    }
}

// 0x4E1A40
void CAEPedAudioEntity::HandlePedSwing(eAudioEvents event, int32 a3, uint32 volume) {
    const auto* weapon = reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(m_pPed) + 0x5A0 + int32(*reinterpret_cast<const int8*>(reinterpret_cast<const uint8*>(m_pPed) + 0x718)) * 0x1C);
    if (!weapon) return;
    if (!AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(143), static_cast<eSoundBankSlot>(5))) {
        if (!AudioEngine.IsLoadingTuneActive()) AEAudioHardware.LoadSoundBank(static_cast<eSoundBank>(143), static_cast<eSoundBankSlot>(5));
        return;
    }
    int id = -1; float speed = 1.f, offset = 0.f;
    switch (*weapon) {
    case 2: case 15: id = 47; break;
    case 3: case 7: id = 86; speed = std::bit_cast<float>(0x3F570A3Du); break;
    case 4: case 8: id = 88; offset = -3.f; break;
    case 5: case 6: id = 86; speed = std::bit_cast<float>(0x3F2B851Fu); break;
    case 10: case 11: case 12: case 13: id = 86; offset = -6.f; speed = std::bit_cast<float>(0x3F570A3Du); break;
    case 0: case 1: case 9: case 14: case 16: case 17: case 18:
    case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34:
    case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45:
        id = 87; offset = -6.f; speed = std::bit_cast<float>(0x3F570A3Du); break;
    default: break;
    }
    if (a3 >= 64 && a3 <= 68) { id = 87; offset = a3 >= 67 ? -2.f : -6.f; speed = std::bit_cast<float>(0x3F570A3Du); }
    if (id < 0) return;
    m_tempSound.Initialise(static_cast<eSoundBankSlot>(5), static_cast<eSoundID>(id), this, m_pPed->GetPosition(), GetDefaultVolume(event) + offset, 1.f, speed, 1.f, 0, 0x80, std::bit_cast<float>(0x3D23D70Au), 0);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    if (volume) {
        m_tempSound.m_SpeedVariance = 0.f;
        m_tempSound.m_Speed = 0.f;
        m_tempSound.m_ClientVariable = static_cast<float>(CTimer::GetTimeInMS() + volume);
        m_tempSound.m_Event = event;
        m_tempSound.SetFlags(SOUND_REQUEST_UPDATES, true);
    }
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E1CC0
void CAEPedAudioEntity::HandlePedHit(eAudioEvents event, CPhysical* physical, uint8 surfaceId, float volume, uint32 maxVol) {
    const auto* weapon = reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(m_pPed) + 0x5A0 + int32(*reinterpret_cast<const int8*>(reinterpret_cast<const uint8*>(m_pPed) + 0x718)) * 0x1C);
    if (!weapon) return;
    if (!AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(143), static_cast<eSoundBankSlot>(5))) {
        if (!AudioEngine.IsLoadingTuneActive()) AEAudioHardware.LoadSoundBank(static_cast<eSoundBank>(143), static_cast<eSoundBankSlot>(5));
        return;
    }
    const uint32 type = *weapon;
    const float baseVolume = GetDefaultVolume(event) + volume;
    int mainID = -1, secondID = -1, secondSlot = 5;
    float mainOffset = 0.f, secondOffset = -3.f, variance = std::bit_cast<float>(0x3D70D845u);
    switch (type) {
    case 0: case 44: case 45: mainID = 58; break;
    case 1: case 16: case 17: case 18: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34:
    case 37: case 38: case 39: case 40: case 41: case 42: case 43: mainID = 44; break;
    case 2: mainID = 48; break;
    case 3: mainID = 35; break;
    case 4: mainID = 57; variance = std::bit_cast<float>(0x3CF5C28Fu); break;
    case 5: mainID = 34; break;
    case 6: mainID = 75; break;
    case 7: mainID = 67; break;
    case 8: mainID = 56; break;
    case 10: case 11: case 12: case 13: mainID = 50; break;
    case 14: mainID = 54; variance = std::bit_cast<float>(0x3D8F5C29u); break;
    case 15: mainID = 46; break;
    default: break;
    }
    bool onlySecond = false;
    if (physical && physical->GetType() == ENTITY_TYPE_PED) {
        switch (event) {
        case 61: case 62: case 63:
            if (type == 4 || type == 8) { secondID = 81; secondOffset = -9.f; onlySecond = true; }
            else if (type == 9) {
                if (!AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(27), static_cast<eSoundBankSlot>(3))) return;
                secondSlot = 3; secondID = CAEAudioUtility::GetRandomNumberInRange<int32>(7, 9); onlySecond = true;
            } else if (type != 14) {
                mainOffset = type == 1 && event != 63 ? 1.f : type == 5 ? -10.f : -15.f;
                if (event == 63) { secondID = 82; secondOffset = -6.f; }
                else secondID = CAEAudioUtility::GetRandomNumberInRange<int32>(event == 61 ? 40 : 36, event == 61 ? 43 : 39);
            }
            break;
        case 64: secondID = 82; secondOffset = -6.f; onlySecond = true; break;
        case 65: case 67: mainID = 58; secondID = CAEAudioUtility::GetRandomNumberInRange<int32>(40, 43); break;
        case 66: mainID = 58; secondID = CAEAudioUtility::GetRandomNumberInRange<int32>(36, 39); break;
        case 68: secondID = CAEAudioUtility::GetRandomNumberInRange<int32>(59, 62); onlySecond = true; break;
        default: break;
        }
    } else {
        const auto collision = [&] { AudioEngine.ReportCollision(m_pPed, physical, static_cast<eSurfaceType>(62), static_cast<eSurfaceType>(surfaceId), m_pPed->GetPosition(), nullptr, 1.f, 1.f, true, false); };
        switch (event) {
        case 64: case 68: collision(); return;
        case 61: case 62: case 63: case 65: case 66: case 67: {
            const float value = static_cast<float>(reinterpret_cast<const int32*>(0x8AD914)[size_t(surfaceId) * 4]) * StaticRef<float>(0x858C58);
            float result; const float twenty = 20.f;
            __asm {
                fld value
                fldlg2
                fxch st(1)
                fyl2x
                fmul twenty
                fstp result
            }
            mainOffset = result;
            if (event >= 65) mainID = 58;
            collision(); break;
        }
        default: break;
        }
    }
    const auto play = [&](int slot, int id, float offset, float randomVariance) {
        m_tempSound.Initialise(static_cast<eSoundBankSlot>(slot), static_cast<eSoundID>(id), this, m_pPed->GetPosition(), baseVolume + offset, 1.f, 1.f, 1.f, 0, 0x80, randomVariance, 0);
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        if (maxVol) {
            m_tempSound.m_SpeedVariance = 0.f; m_tempSound.m_Speed = 0.f;
            m_tempSound.m_ClientVariable = static_cast<float>(CTimer::GetTimeInMS() + maxVol);
            m_tempSound.m_Event = event; m_tempSound.SetFlags(SOUND_REQUEST_UPDATES, true);
        }
        AESoundManager.RequestNewSound(&m_tempSound);
    };
    if (!onlySecond && mainID >= 0) play(5, mainID, mainOffset, variance);
    if (secondID >= 0) play(secondSlot, secondID, secondOffset, std::bit_cast<float>(0x3D70D845u));
}

// 0x4E2350
void CAEPedAudioEntity::HandlePedJacked(eAudioEvents event) {
    if (!AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(143), static_cast<eSoundBankSlot>(5))) {
        if (!AudioEngine.IsLoadingTuneActive()) AEAudioHardware.LoadSoundBank(static_cast<eSoundBank>(143), static_cast<eSoundBankSlot>(5));
        return;
    }
    const float volume = GetDefaultVolume(event);
    uint32 firstTime = CTimer::GetTimeInMS(), collisionTime = firstTime;
    int first = -1, second = -1;
    switch (event) {
    case 69: first = 58; second = CAEAudioUtility::GetRandomNumberInRange<int32>(40, 43); firstTime += 500; collisionTime += 2133; break;
    case 70: first = 50; second = CAEAudioUtility::GetRandomNumberInRange<int32>(36, 39); firstTime += 933; collisionTime += 2633; break;
    case 71: second = CAEAudioUtility::GetRandomNumberInRange<int32>(40, 43); firstTime += 900; collisionTime += 2800; break;
    case 72: first = 58; second = CAEAudioUtility::GetRandomNumberInRange<int32>(40, 43); firstTime += 33; collisionTime += 366; break;
    case 73: second = CAEAudioUtility::GetRandomNumberInRange<int32>(36, 39); firstTime += 266; collisionTime += 833; break;
    default: break;
    }
    const auto play = [&](int slot, int id, uint32 deadline) {
        m_tempSound.Initialise(static_cast<eSoundBankSlot>(slot), static_cast<eSoundID>(id), this, m_pPed->GetPosition(), volume, 2.f, 0.f, 1.f, 0, 0x84, 0.f, 0);
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        m_tempSound.m_Event = event; m_tempSound.m_ClientVariable = static_cast<float>(deadline);
        AESoundManager.RequestNewSound(&m_tempSound);
    };
    if (first >= 0) play(5, first, firstTime);
    if (second >= 0) play(5, second, firstTime);
    if (AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(0), static_cast<eSoundBankSlot>(41))) play(41, 0, collisionTime);
    else AEAudioHardware.LoadSoundBank(static_cast<eSoundBank>(0), static_cast<eSoundBankSlot>(41));
}

// 0x4E26A0
void CAEPedAudioEntity::HandleSwimSplash(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
        return;
    }
    m_nSfxId = std::max(0, m_nSfxId + 1);
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_SWIMMING,
        .SoundID            = (eSoundID)(m_nSfxId),
        .AudioEntity        = this,
        .Pos                = m_pPed->GetPosition(),
        .Volume             = GetDefaultVolume(event),
        .Flags              = SOUND_PLAY_PHYSICALLY | SOUND_START_PERCENTAGE | SOUND_IS_DUCKABLE,
        .FrequencyVariance  = 0.0588f,
        .RegisterWithEntity = m_pPed,
    });
}

// 0x4E2790
void CAEPedAudioEntity::HandleSwimWake(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
        return;
    }
    if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, m_pPed)) {
        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 3,
            .AudioEntity        = this,
            .Pos                = m_pPed->GetPosition(),
            .Volume             = GetDefaultVolume(event) - 20.0f,
            .RollOffFactor      = 1.f,
            .Speed              = 0.75f,
            .Doppler            = 1.f,
            .FrameDelay         = 0,
            .Flags              = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .RegisterWithEntity = m_pPed,
            .EventID            = event,
        });
    }
    m_LastSwimWakeTriggerTimeMs = CTimer::GetTimeInMS();
}

// 0x4E2A90
void CAEPedAudioEntity::PlayShirtFlap(float volume, float speed) {
    if (m_sTwinLoopSoundEntity.IsActive()) {
        m_sTwinLoopSoundEntity.UpdateTwinLoopSound(m_pPed->GetPosition(), volume, speed);
    } else {
        m_sTwinLoopSoundEntity.Initialise(SND_BANK_SLOT_WEAPON_GEN, 19, 20, this, 200, 1000, -1, -1);
        m_sTwinLoopSoundEntity.PlayTwinLoopSound(m_pPed->GetPosition(), volume, speed, 2.0f, 1.0f, SOUND_DEFAULT);
    }
}

static float PedShirtLogarithmicVolume(float value, float baseVolume) {
    float result;
    const float scale = 20.f;
    auto logFunction = &CAEAudioUtility::AudioLog10;
    // Preserve the transcendental result in ST(0) through both operations.
    __asm {
        push value
        call logFunction
        add esp, 4
        fmul scale
        fadd baseVolume
        fstp result
    }
    return result;
}

// 0x4E2EE0
void CAEPedAudioEntity::Service() {
    if (!m_pPed->IsPlayer()) return;
    const auto readFloat = [](uintptr_t address) { return StaticRef<float>(address); };
    const auto clamp = [](float value) { if (value > 1.f) return 1.f; if (value < 0.f) return 0.f; return value; };
    const auto speedOf = [](const CPhysical* body) {
        const auto& v = body->m_vecMoveSpeed;
        return std::sqrt((v.x * v.x + v.y * v.y) + v.z * v.z);
    };
    float volume = -100.f, speed = 1.f, falling = 0.f, decel = 0.f;
    const bool onFoot = !(m_pPed->m_pAttachedTo && m_pPed->m_pAttachedTo->GetType() == ENTITY_TYPE_VEHICLE) && !m_pPed->bInVehicle;
    auto* vehicle = m_pPed->m_pVehicle;
    if (static_cast<int32>(m_pPed->m_nPedState) == 50 && vehicle) {
        const int32 vehicleType = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(vehicle) + 0x590);
        bool movingVehicle = vehicleType == 9;
        if (vehicleType == 0) {
            const auto* vtable = *reinterpret_cast<const uintptr_t* const*>(vehicle);
            movingVehicle = reinterpret_cast<bool(__thiscall*)(CVehicle*)>(vtable[39])(vehicle);
        }
        if (movingVehicle) {
            const float magnitude = std::abs(speedOf(vehicle));
            if (magnitude > 0.f) {
                speed = clamp(magnitude);
                volume = PedShirtLogarithmicVolume(speed, readFloat(*(reinterpret_cast<const uint8*>(vehicle) + 0x1B8) == 2 ? 0x8AE654 : 0x8AE650));
                speed = readFloat(0xB613BC) * speed + readFloat(0x8AE658);
            }
        }
    }
    const bool parachute = m_pPed->GetIntelligence() && m_pPed->GetIntelligence()->GetUsingParachute();
    if (onFoot) {
        speed = speedOf(m_pPed) / readFloat(0x8AE6A4);
        float offset;
        if (!parachute) {
            offset = readFloat(0x8AE660);
            if (!(speed < readFloat(0x8AE69C))) speed = clamp((speed - readFloat(0x8AE69C)) / readFloat(0xB613CC));
            else speed = 0.f;
        } else {
            offset = readFloat(0x8AE664);
            if (auto* anim = RpAnimBlendClumpGetAssociation(m_pPed->GetRpClump(), "FALL_SkyDive_accel")) falling = clamp(anim->m_BlendAmount);
            if (auto* anim = RpAnimBlendClumpGetAssociation(m_pPed->GetRpClump(), "Para_decel")) decel = clamp(anim->m_BlendAmount);
            speed = clamp(speed);
        }
        volume = PedShirtLogarithmicVolume(speed, offset);
        speed = readFloat(0xB613C0) * speed + readFloat(0x8AE668);
    }
    speed = (readFloat(0x8AE644) * falling + 1.f) * speed;
    const float target = (readFloat(0x8AE648) * falling + volume) + readFloat(0x8AE64C) * decel;
    if (target > field_158) {
        const float up = readFloat(0x8AE670) + field_158;
        field_158 = up < target ? up : target;
    } else if (target < field_158) {
        const float down = field_158 - readFloat(0x8AE674);
        field_158 = down > target ? down : target;
    }
    if (target < readFloat(0x8AE6A0) && field_158 < readFloat(0x8AE6A0)) field_158 = target;
    if (field_158 > -100.f || std::isnan(field_158)) PlayShirtFlap(field_158, speed);
    else m_sTwinLoopSoundEntity.StopSoundAndForget();
}
