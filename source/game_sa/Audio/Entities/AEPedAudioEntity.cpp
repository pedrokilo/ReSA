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
 * Validación registrada: 20/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEPedAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "X87Math.h"
#include <bit>

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
            .FrequencyVariance = std::bit_cast<float>(0x3D75C290u), // literal del EXE (0x4E2D8C/0x4E2E29): 1 ulp sobre 0.06f (0x3D75C28F)
            .PlayTime          = 0,
            .EventID           = AE_PED_CRUNCH,
            .ClientVariable    = -1.0f // Como el original: CAESound::Initialise deja m_ClientVariable = -1 (0x4E2D72)
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
            .FrequencyVariance = std::bit_cast<float>(0x3D75C290u), // literal del EXE (0x4E2D8C/0x4E2E29): 1 ulp sobre 0.06f (0x3D75C28F)
            .PlayTime          = 0,
            .EventID           = AE_PED_KNOCK_DOWN,
            .ClientVariable    = -1.0f // Como el original (0x4E2E0F)
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

    // Como el original (0x4E28EA..0x4E2919): los valores iniciales salen de la tabla 0x8AE5F0/0x8AE600/0x8AE610/0x8AE618
    // (-100, -100, 2.0, 0.4) y se escriben antes de m_bJetPackPlaying.
    m_fVolume2 = StaticRef<float>(0x8AE5F0);
    m_fVolume1 = StaticRef<float>(0x8AE600);
    m_fVolume3 = StaticRef<float>(0x8AE610);
    m_JetPackSoundSpeedMult = StaticRef<float>(0x8AE618);

    m_bJetPackPlaying = true;

    // Como el original: usa el miembro m_tempSound (no un CAESound local ni AESoundManager.PlaySound), con
    // Initialise(slot, sfx, this, pos, -100, 1, 1, 1, 0, 0, 0, 0), speed = 1.0 y flags = REQUEST_UPDATES (4).
    const auto PlayJetPackSound = [&](eSoundBankSlot slot, eSoundID soundID) {
        m_tempSound.Initialise(slot, soundID, this, m_pPed->GetPosition(), -100.f, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        m_tempSound.m_Speed = 1.0f;
        m_tempSound.m_Flags = SOUND_REQUEST_UPDATES;
        return AESoundManager.RequestNewSound(&m_tempSound);
    };
    m_JetPackSound0 = PlayJetPackSound(SND_BANK_SLOT_VEHICLE_GEN, static_cast<eSoundID>(26));
    m_JetPackSound1 = PlayJetPackSound(SND_BANK_SLOT_WEAPON_GEN, static_cast<eSoundID>(10));
    m_JetPackSound2 = PlayJetPackSound(SND_BANK_SLOT_FRONTEND_GAME, static_cast<eSoundID>(0));
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

    // Como el original (fcom + test ah,41h / test ah,5): con NaN gana siempre el limite.
    const auto ClampMin = [](float v, float lim) { return v > lim ? v : lim; };   // test ah,41h / je
    const auto ClampMax = [](float v, float lim) { return v < lim ? v : lim; };   // test ah,5 / jnp

    if (!(thrustFwd > 0.5f)) { // 0x4E0F1C: fcomp qword [859060]=0.5, test ah,41h / jne -> bloque 0x4E0FCC (tambien con NaN)
        m_fVolume1 = ClampMin(m_fVolume1 - 5.0f, -100.0f);
        m_fVolume2 = ClampMax(m_fVolume2 + 6.0f, -17.0f);
        m_fVolume3 = ClampMin(m_fVolume3 - 0.3f, 2.0f);
        m_JetPackSoundSpeedMult = ClampMin(m_JetPackSoundSpeedMult - 0.031f, 0.4f);
    } else {
        m_fVolume1 = ClampMax(15.0f + m_fVolume1, -15.0f);
        m_fVolume2 = ClampMin(m_fVolume2 - 7.1f, -100.0f);
        m_fVolume3 = ClampMax(0.3f + m_fVolume3, 11.0f);
        m_JetPackSoundSpeedMult = ClampMax(0.031f + m_JetPackSoundSpeedMult, 0.71f);
    }

    // Como el original (0x4E1068..0x4E1081): fsin x87 de precision completa; ST0 conserva el seno sin redondear
    // para la velocidad del sonido 0 y el float guardado (fst [esp+4]) para la del sonido 2.
    const float zero = StaticRef<float>(0xB61384); // 0x4E10A4: global sin inicializar del EXE (0.0)
    const float k    = StaticRef<float>(0xB613B0); // 0.93f - 1.0f, calculado por el inicializador 0x84C7E0
    // 0x4E1068..0x4E10A2: el seno de fsin queda sin redondear en ST0 para |sin|·k + 1.0 (velocidad del sonido 0); el float guardado (fst) es sinF.
    float sinF{};
    const float speed0 = PedAudioJetPackSinCpp(thrustAngle, k, StaticRef<float>(0x8AE608), &sinF);
    const float absSin = sinF < 0.0f ? -sinF : sinF; // fcomp 0 / test ah,5 / jp / fchs (NaN y 0 sin cambio)

    m_JetPackSound0->m_Volume = zero + m_fVolume1;
    m_JetPackSound0->m_Speed  = speed0;

    m_JetPackSound1->m_Volume = zero + m_fVolume2;
    m_JetPackSound1->m_Speed  = StaticRef<float>(0x8AE5F8); // mov [eax+1Ch],[8AE5F8] = 0.56

    m_JetPackSound2->m_Volume = zero + m_fVolume3;
    m_JetPackSound2->m_Speed  = (absSin * StaticRef<float>(0x8AE620) + StaticRef<float>(0x858624)) * m_JetPackSoundSpeedMult;
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
    if (m_pPed->bIsInTheAir) { // test ch,2 sobre [ped+46Ch] (0x4E13B3)
        return;
    }

    // Como el original (0x4E13C1..0x4E13F2): (tabla[event] + volume) en float y luego + rand(-3,3), ANTES de FindPlayerPed.
    float vol = GetDefaultVolume(event) + volume;
    vol = static_cast<float>(CAEAudioUtility::GetRandomNumberInRange(-3, 3)) + vol;

    // 0x4E175F..0x4E17CE: siempre usa el miembro m_tempSound.
    const auto PlaySoundNow = [&](eSoundBankSlot slot, int32 sfx, int16 playTime) {
        m_tempSound.Initialise(slot, static_cast<eSoundID>(sfx), this, m_pPed->GetPosition(), vol, 1.0f, speed, 1.0f, 0, 0xA0, std::bit_cast<float>(0x3D70D845u), playTime);
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        AESoundManager.RequestNewSound(&m_tempSound);
    };

    // 0x4E168B (jugador) / 0x4E172D (resto de peds, con vol -= [858B3C] = 3.0)
    const auto PlayGeneric = [&](bool reduceVolume) {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
            AEAudioHardware.LoadSoundBank(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC);
            return;
        }
        if (reduceVolume) {
            vol = vol - StaticRef<float>(0x858B3C);
        }
        PlaySoundNow(SND_BANK_SLOT_FOOTSTEPS_GENERIC, CAEAudioUtility::GetRandomNumberInRange(1, 5), 0);
    };

    if (FindPlayerPed(-1) != m_pPed) {
        PlayGeneric(true);
        return;
    }

    const auto contact = m_pPed->m_nContactSurface;

    // Agua: IsAudioWater(forzada) || IsAudioWater(contacto) (0x4E141C..0x4E16F7)
    if (g_surfaceInfos.IsAudioWater(forcedSurfaceType) || g_surfaceInfos.IsAudioWater(contact)) {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
            AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING); // 0x4E167C
            PlayGeneric(false);
            return;
        }
        // Como el original: el aleatorio se pide DESPUES de comprobar el banco y el playTime es 0x32
        PlaySoundNow(SND_BANK_SLOT_SWIMMING, CAEAudioUtility::GetRandomNumberInRange(0, 4), 50);
        return;
    }

    // Superficies del jugador (0x4E145E..0x4E168F)
    const auto TryPlayerBank = [&](eSoundBank bank, int32 rndMax) {
        const auto slot = SND_BANK_SLOT_FOOTSTEPS_PLAYER;
        if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
            PlaySoundNow(slot, CAEAudioUtility::GetRandomNumberInRange(0, rndMax), 0);
            return;
        }
        // Como el original: movsx eax,ax / dec / je / dec / jne -> solo 1 o 2 (no "<= 1" con signo)
        const auto playing = AESoundManager.AreSoundsPlayingInBankSlot(slot);
        if (playing == 1 || playing == 2) {
            AESoundManager.CancelSoundsInBankSlot(slot, false);
        }
        AEAudioHardware.LoadSoundBank(bank, slot);
        PlayGeneric(false);
    };

    if (g_surfaceInfos.IsAudioConcrete(contact)) {
        PlayGeneric(false);
    } else if (g_surfaceInfos.IsAudioGrass(contact) || g_surfaceInfos.IsAudioLongGrass(contact)) {
        TryPlayerBank(SND_BANK_FEET_GRASS, 4);
    } else if (g_surfaceInfos.IsAudioSand(contact)) {
        TryPlayerBank(SND_BANK_FEET_SAND, 3);
    } else if (g_surfaceInfos.IsAudioGravel(contact)) {
        TryPlayerBank(SND_BANK_FEET_GRAVEL, 4);
    } else if (g_surfaceInfos.IsAudioWood(contact)) {
        TryPlayerBank(SND_BANK_FEET_WOOD, 4);
    } else if (g_surfaceInfos.IsAudioTile(contact)) {
        TryPlayerBank(SND_BANK_FEET_TILE, 4);
    } else if (g_surfaceInfos.IsAudioMetal(contact)) {
        TryPlayerBank(SND_BANK_FEET_METAL, 4);
    } else {
        PlayGeneric(false);
    }
}

// 0x4E17E0
void CAEPedAudioEntity::HandleSkateEvent(eAudioEvents event, float volume, float speed) {
    if (m_pPed->bIsInTheAir) {
        return;
    }

    // Como el original (0x4E1804..0x4E1839): (tabla[event] + volume) y despues + rand(-3,3), ANTES de comprobar el banco.
    float vol = GetDefaultVolume(event) + volume;
    vol = static_cast<float>(CAEAudioUtility::GetRandomNumberInRange(-3, 3)) + vol;

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
        return;
    }
    // 0x4E1865: sfx = (event != 0x38) + 7; flags 0x80, varianza 0x3D70D845, m_tempSound
    m_tempSound.Initialise(SND_BANK_SLOT_FOOTSTEPS_GENERIC, static_cast<eSoundID>(event != AE_PED_SKATE_LEFT ? 8 : 7), this, m_pPed->GetPosition(), vol, 1.0f, speed, 1.0f, 0, 0x80, std::bit_cast<float>(0x3D70D845u), 0);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E18E0
void CAEPedAudioEntity::HandleLandingEvent(eAudioEvents event) {
    if (m_pPed->bIsInTheAir) {
        return;
    }

    float volume = GetDefaultVolume(event);
    eSoundBankSlot slot;
    int32 sfx;
    int16 playTime = 0;
    if (g_surfaceInfos.IsAudioWater(m_pPed->m_nContactSurface)) {
        if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
            return;
        }
        slot = SND_BANK_SLOT_SWIMMING;
        sfx = CAEAudioUtility::GetRandomNumberInRange(0, 4);
        playTime = 50;
        if (!(volume > 0.0f)) { // 0x4E1968: fcomp 0 / test ah,41h / je (NaN y <= 0 -> 0)
            volume = 0.0f;
        }
    } else {
        if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
            return;
        }
        slot = SND_BANK_SLOT_FOOTSTEPS_GENERIC;
        sfx = event == AE_PED_LAND_ON_FEET_AFTER_FALL ? 6 : 0;
    }
    // 0x4E19C0..0x4E1A2C: miembro m_tempSound, flags 0xA0, varianza 0x3D70D845
    m_tempSound.Initialise(slot, static_cast<eSoundID>(sfx), this, m_pPed->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, 0xA0, std::bit_cast<float>(0x3D70D845u), playTime);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    AESoundManager.RequestNewSound(&m_tempSound);
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
            // 0x4DA510: fild * 0.01f, fldlg2/fyl2x y fmul 20.0f con un solo redondeo a float (fstp) = AudioLog10Times20Cpp.
            const float result = AudioLog10Times20Cpp(value);
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
    const float volume = GetDefaultVolume(event);
    // Como el original (0x4E26DF..0x4E26FF): inc cx / cmp cx,4 / jle -> si pasa de 4 vuelve a 0 (no un std::max)
    int16 id = static_cast<int16>(m_nSfxId + 1);
    if (id > 4) {
        id = 0;
    }
    m_nSfxId = id;
    // 0x4E2718..0x4E2777: m_tempSound; flags 0 en Initialise y luego SetFlags(0x80, 1); varianza 0x3D70D845
    m_tempSound.Initialise(SND_BANK_SLOT_SWIMMING, static_cast<eSoundID>(id), this, m_pPed->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
    m_tempSound.m_SpeedVariance = std::bit_cast<float>(0x3D70D845u);
    m_tempSound.SetFlags(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY, 1);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E2790
void CAEPedAudioEntity::HandleSwimWake(eAudioEvents event) {
    // Como el original: primero AreSoundsOfThisEventPlayingForThisEntityAndPhysical (0x4E27A7); si suena, solo actualiza la hora.
    if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, m_pPed)) {
        if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
            return; // sin tocar m_LastSwimWakeTriggerTimeMs (salta a 0x4E2893)
        }
        const float volume = GetDefaultVolume(event) - StaticRef<float>(0x858BA4); // fild / fsub [858BA4] = 20.0
        // 0x4E283C..0x4E2877: m_tempSound; speed 0.75, flags 0x84 y evento escritos a mano tras Initialise
        m_tempSound.Initialise(SND_BANK_SLOT_COLLISIONS, static_cast<eSoundID>(3), this, m_pPed->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
        m_tempSound.m_Speed = 0.75f;
        m_tempSound.m_Flags = 0x84;
        m_tempSound.m_Event = event;
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        AESoundManager.RequestNewSound(&m_tempSound);
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
    // 0x4D9E50 + 0x4E303A: p < 1e-5 (o NaN: fcomp + test ah,1 / jne) devuelve -5.0; si no, log10(p) sin redondear en ST0.
    // Luego fmul 20.0f (-5 · 20 = -100 exacto), fadd baseVolume y fstp: el producto se redondea a 24 bits y la suma otra vez.
    const float scaled = (value >= 0.00001f) ? AudioLog10Times20Cpp(value) : -100.0f;
    return scaled + baseVolume;
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
