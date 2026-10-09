/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEFrontendAudioEntity
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEFrontendAudioEntity.h"

#include "AETwinLoopSoundEntity.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "AESound.h"

// 0x5B9AB0
void CAEFrontendAudioEntity::Initialise() {
    // Como el original (0x5B9AB1..0x5B9AF6): se escriben todos los campos en este orden.
    m_nLastFrameGeneral_or_nFrameCount = 0;  // 0x84
    m_nLastFrameMissionComplete        = 0;  // 0x88
    m_nLastFrameBulletPass             = 0;  // 0x8C
    m_BulletPassCount                  = 0;  // 0x80 (word)
    m_BulletPassBank                   = SND_BANK_GENRL_BULLET_PASS_1; // 0x82 (word 0x1C)
    m_f7E                              = -1; // 0x7E
    m_nLastTimeCarRespray              = 0;  // 0x90
    m_nLatestTimerCount                = 0;  // 0x94
    m_bAmplifierWakeUp                 = false; // 0x7C
    m_pAmplifierWakeUp                 = nullptr; // 0x98

    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_BULLET_PASS_1, SND_BANK_SLOT_BULLET_PASS);

    m_nbLoadingTuneSeed = CAEAudioUtility::GetRandomNumberInRange(0, 3);
    // Como el original (0x5B9B24..0x5B9B52): el byte se extiende con signo (movsx) y el id se trunca a 16 bits.
    const auto seed = static_cast<int16>(static_cast<int8>(m_nbLoadingTuneSeed));
    AEAudioHardware.LoadSound(SND_BANK_GENRL_LOADING, static_cast<eSoundID>(static_cast<int16>(2 * seed + 0)), SND_BANK_SLOT_COLLISIONS);
    AEAudioHardware.LoadSound(SND_BANK_GENRL_LOADING, static_cast<eSoundID>(static_cast<int16>(2 * seed + 1)), SND_BANK_SLOT_WEAPON_GEN);
}

// 0x4DD440
void CAEFrontendAudioEntity::Reset() {
    m_nLastFrameGeneral_or_nFrameCount = 0;
    m_nLastFrameMissionComplete        = 0;
    m_nLastFrameBulletPass             = 0;
    m_BulletPassCount                  = 0;
    m_f7E                              = -1;
    AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_NOISE_START, this);
}

// 0x4DD4A0
void CAEFrontendAudioEntity::AddAudioEvent(eAudioEvents event, float fVolumeBoost, float fSpeed) {
    // Como el original: una voz temporal se reutiliza para ambas peticiones.
    CAESound sound;
    const float volume = GetDefaultVolume(event) + fVolumeBoost;
    const auto loaded = [&](int bank, int slot) { return AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(bank), static_cast<eSoundBankSlot>(slot)); };
    const auto play = [&](int slot, int id, CVector pos, uint16 flags, float speed, int ev = AE_UNDEFINED, float variance = 0.f) {
        sound.Initialise(static_cast<eSoundBankSlot>(slot), static_cast<eSoundID>(id), this, pos, volume, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        sound.m_Speed = speed;
        sound.m_Flags = flags;
        sound.m_Event = ev;
        sound.m_SpeedVariance = variance;
        return AESoundManager.RequestNewSound(&sound);
    };
    const auto pair = [&](int slot, int first, uint16 flags, float speed, int ev = AE_UNDEFINED) {
        play(slot, first, CVector{-1.f, 0.f, 0.f}, flags, speed, ev);
        play(slot, first + 1, CVector{1.f, 0.f, 0.f}, flags, speed, ev);
    };
    const CVector centre{0.f, 1.f, 0.f};
    switch (event) {
    case 0: if (loaded(59, 0)) pair(0, 25, 0x151B, fSpeed); break;
    case 1: if (loaded(60, 1)) pair(1, 6, 0x151B, fSpeed); break;
    case 2: if (loaded(60, 1)) pair(1, 0, 0x151B, fSpeed); break;
    case 3: if (loaded(60, 1)) pair(1, 4, 0x111B, fSpeed); break;
    case 4: if (loaded(60, 1)) pair(1, 2, 0x151B, fSpeed); break;
    case 5: if (loaded(60, 1)) play(1, 8, centre, 0x11B, fSpeed); break;
    case 6: case 18: case 19: case 20: case 40:
    case 7: case 8: case 9: case 10:
    case 14: case 15: case 17:
        if (loaded(59, 0) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5u) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            const bool alternate = event == 14 || event == 15 || event == 17;
            pair(0, event >= 7 && event <= 10 ? 16 : 27, 0x111B, alternate ? std::bit_cast<float>(0x3F574BC7u) : fSpeed);
        }
        break;
    case 11: case 32: if (loaded(59, 0)) pair(0, 14, 0x151B, fSpeed); break;
    case 12: case 13: case 43:
        if (loaded(59, 0) && CTimer::GetFrameCounter() >= m_nLastFrameMissionComplete + 5u) {
            m_nLastFrameMissionComplete = CTimer::GetFrameCounter();
            pair(0, 18, 0x151B, fSpeed);
        }
        break;
    case 16:
        if (loaded(143, 5)) {
            const bool choice = CAEAudioUtility::ResolveProbability(0.5f);
            const float other = std::bit_cast<float>(0x3F983809u);
            play(5, 28, CVector{-1.f, 0.f, 0.f}, 0x110F, choice ? other : 1.f, event);
            play(5, 28, CVector{1.f, 0.f, 0.f}, 0x110F, choice ? 1.f : other, event);
            m_nLastTimeCarRespray = CTimer::GetTimeInMS();
        }
        break;
    case 27: case 28: case 29: case 30:
        if (loaded(m_BulletPassBank, 42) && CTimer::GetFrameCounter() >= m_nLastFrameBulletPass + 5u) {
            if (m_BulletPassCount > 10) {
                if (!AESoundManager.AreSoundsPlayingInBankSlot(42)) {
                    m_BulletPassBank = static_cast<eSoundBank>(static_cast<int16>(m_BulletPassBank) + 1);
                    if (static_cast<int16>(m_BulletPassBank) > 31) m_BulletPassBank = static_cast<eSoundBank>(28);
                    AEAudioHardware.LoadSoundBank(m_BulletPassBank, static_cast<eSoundBankSlot>(42));
                    m_BulletPassCount = 0;
                }
            } else {
                m_nLastFrameBulletPass = CTimer::GetFrameCounter();
                const auto id = CAEAudioUtility::GetRandomNumberInRange<int32>(0, 2);
                play(42, id, CVector{event <= 28 ? -0.1f : 0.1f, event == 27 || event == 29 ? -1.f : 1.f, 0.f}, 7, 1.f, event, 0.03125f);
                ++m_BulletPassCount;
            }
        }
        break;
    case 31:
        if (!m_bAmplifierWakeUp && loaded(59, 0)) {
            m_pAmplifierWakeUp = play(0, 8, centre, 0x11F, fSpeed);
            m_bAmplifierWakeUp = m_pAmplifierWakeUp != nullptr;
        }
        break;
    case 33:
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this)) {
            if (!loaded(59, 0)) break;
            pair(0, 4, 0x110F, fSpeed, event);
        }
        m_nLatestTimerCount = CTimer::GetTimeInMS();
        break;
    case 34:
        if (loaded(59, 0)) {
            auto& twin = CTimer::GetIsPaused() ? m_objRetunePaused : m_objRetune;
            if (!twin.IsActive()) {
                twin.Initialise(static_cast<eSoundBankSlot>(0), static_cast<eSoundID>(2), static_cast<eSoundID>(1), this, 200, 650);
                twin.PlayTwinLoopSound(centre, volume, 1.f, 1.f, 1.f, static_cast<eSoundEnvironment>(CTimer::GetIsPaused() ? 0x15B : 0x14B));
            }
        }
        break;
    case 35:
        if (!CTimer::GetIsPaused()) { if (m_objRetune.IsActive()) m_objRetune.StopSoundAndForget(); break; }
        [[fallthrough]];
    case 36: if (m_objRetunePaused.IsActive()) m_objRetunePaused.StopSoundAndForget(); break;
    case 37: if (loaded(59, 0)) play(0, 23, centre, 0x11B, 1.f); break;
    case 38: if (loaded(59, 0)) play(0, 23, centre, 0x11B, std::bit_cast<float>(0x3F641206u)); break;
    case 41: if (loaded(59, 0)) play(0, 10, centre, 3, fSpeed); break;
    case 42: if (loaded(59, 0)) play(0, 11, centre, 3, fSpeed); break;
    case 44: if (loaded(59, 0)) pair(0, 6, 0x151B, fSpeed); break;
    case 45: if (loaded(59, 0)) pair(0, 12, 0x151B, fSpeed); break;
    case 46: if (loaded(59, 0)) play(0, 9, centre, 0x11B, 1.f, AE_UNDEFINED, std::bit_cast<float>(0x3D70D845u)); break;
    case 47: case 49: case 101:
        if (loaded(59, 0) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this)) {
            play(0, event == 47 ? 3 : event == 49 ? 24 : 13, centre, event == 101 ? 0x101 : 0x103, fSpeed, event);
        }
        break;
    case 48: case 51: AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(static_cast<int16>(event == 48 ? 47 : 50), this); break;
    case 50: {
        const int16 seed = static_cast<int8>(m_nbLoadingTuneSeed);
        if (AEAudioHardware.IsSoundLoaded(static_cast<eSoundBank>(82), static_cast<eSoundID>(2 * seed), static_cast<eSoundBankSlot>(2))
            && AEAudioHardware.IsSoundLoaded(static_cast<eSoundBank>(82), static_cast<eSoundID>(2 * seed + 1), static_cast<eSoundBankSlot>(5))
            && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this)) {
            play(2, 2 * seed, CVector{-1.f, 0.f, 0.f}, 0x111B, fSpeed, event);
            play(5, 2 * seed + 1, CVector{1.f, 0.f, 0.f}, 0x111B, fSpeed, event);
        }
        break;
    }
    default: break;
    }
}


// 0x4DD480
bool CAEFrontendAudioEntity::IsRadioTuneSoundActive() {
    return CTimer::GetIsPaused()
        ? m_objRetunePaused.IsActive()
        : m_objRetune.IsActive();
}

// 0x4DD470
bool CAEFrontendAudioEntity::IsLoadingTuneActive() {
    return AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this);
}

// 0x4DEDA0
void CAEFrontendAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {


    // Como el original: multiplicación, doble y eje Z según 0x4DEDF9..0x4DEECC.
    if (!sound)
        return;

    const float fraction = static_cast<float>(curPlayPos) * std::bit_cast<float>(0x3B3B3EE7u);
    const float curPos = fraction + fraction;
    const float rear = curPos - 1.0f;
    const float front = 1.0f - curPos;

    const auto BulletSound = [=](const float x, const float y) {
        if (curPlayPos >= 0 && curPlayPos <= 350) {
            sound->SetPosition({ x, 0.0f, y });
        }
    };

    switch (sound->m_Event) {
    case AE_FRONTEND_CAR_RESPRAY:
        if (curPlayPos > 0 && CTimer::GetTimeInMS() > m_nLastTimeCarRespray + 1900) {
            sound->StopSoundAndForget();
        }
        break;
    case AE_FRONTEND_BULLET_PASS_LEFT_REAR:
        BulletSound(-0.1f, rear);
        break;
    case AE_FRONTEND_BULLET_PASS_LEFT_FRONT:
        BulletSound(-0.1f, front);
        break;
    case AE_FRONTEND_BULLET_PASS_RIGHT_REAR:
        BulletSound(+0.1f, rear);
        break;
    case AE_FRONTEND_BULLET_PASS_RIGHT_FRONT:
        BulletSound(+0.1f, front);
        break;
    case AE_FRONTEND_TIMER_COUNT:
        if (curPlayPos > 0 && CTimer::GetTimeInMS() > m_nLatestTimerCount + 100) {
            sound->StopSoundAndForget();
        }
        break;
    default:
        break;
    }

    if (sound == m_pAmplifierWakeUp && curPlayPos == -1) {
        m_pAmplifierWakeUp = nullptr;
        m_bAmplifierWakeUp = false;
    }
}

void CAEFrontendAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEFrontendAudioEntity, 0x862E54, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x5B9AB0);
    RH_ScopedInstall(Reset, 0x4DD440);
    RH_ScopedInstall(AddAudioEvent, 0x4DD4A0);
    RH_ScopedInstall(IsRadioTuneSoundActive, 0x4DD480);
    RH_ScopedInstall(IsLoadingTuneActive, 0x4DD470);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DEDA0);
}
