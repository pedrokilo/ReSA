/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEAudioHardware
 * Reconstrucción: COMPLETA — 54/54 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/54 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEAudioHardware.h"
#include "AEMP3BankLoader.h"
#include "AEMP3TrackLoader.h"
#include "AEAudioEnvironment.h"
#include "AEAudioUtility.h"
#include "AEStaticChannel.h"
#include "AEUserRadioTrackManager.h"

auto& AEAudioHardware = StaticRef<CAEAudioHardware>(0xB5F8B8);

void CAEAudioHardware::InjectHooks() {
    RH_ScopedClass(CAEAudioHardware);
    RH_ScopedCategory("Audio/Hardware");

    RH_ScopedInstall(Constructor, 0x4D83E0);
    RH_ScopedInstall(Destructor, 0x4D83A0);

    RH_ScopedInstall(AllocateChannels, 0x5B9340);
    RH_ScopedInstall(RequestVirtualChannelSoundInfo, 0x4D8E60);
    RH_ScopedInstall(Query3DSoundEffects, 0x4D8490);
    RH_ScopedInstall(GetNumAvailableChannels, 0x4D8810);
    RH_ScopedInstall(GetChannelPlayTimes, 0x4D8820);
    RH_ScopedInstall(SetChannelVolume, 0x4D8870);
    RH_ScopedInstall(LoadSoundBank, 0x4D88A0);
    RH_ScopedInstall(IsSoundBankLoaded, 0x4D88C0);
    RH_ScopedInstall(GetSoundBankLoadingStatus, 0x4D88D0);
    RH_ScopedInstall(LoadSound, 0x4D8ED0);
    RH_ScopedInstall(IsSoundLoaded, 0x4D8EF0);
    RH_ScopedInstall(GetSoundLoadingStatus, 0x4D8F00);
    RH_ScopedInstall(StopSound, 0x4D88E0);
    RH_ScopedInstall(SetChannelPosition, 0x4D8920);
    RH_ScopedInstall(SetChannelFrequencyScalingFactor, 0x4D8960);
    RH_ScopedInstall(RescaleChannelVolumes, 0x4D8990);
    RH_ScopedInstall(UpdateReverbEnvironment, 0x4D8DA0);
    RH_ScopedInstall(GetSoundHeadroom, 0x4D8E30);
    RH_ScopedInstall(EnableEffectsLoading, 0x4D8E40);
    RH_ScopedInstall(DisableEffectsLoading, 0x4D8E50);
    RH_ScopedInstall(GetVirtualChannelSoundLengths, 0x4D8E90);
    RH_ScopedInstall(GetVirtualChannelSoundLoopStartTimes, 0x4D8EB0);
    RH_ScopedInstall(PlayTrack, 0x4D8F10);
    RH_ScopedInstall(StartTrackPlayback, 0x4D8F30);
    RH_ScopedInstall(StopTrack, 0x4D8F50);
    RH_ScopedInstall(GetTrackPlayTime, 0x4D8F60);
    RH_ScopedInstall(GetTrackLengthMs, 0x4D8F70);
    RH_ScopedInstall(GetActiveTrackID, 0x4D8F80);
    RH_ScopedInstall(GetPlayingTrackID, 0x4D8F90);
    RH_ScopedInstall(GetBeatInfo, 0x4D8FA0);
    RH_ScopedInstall(SetBassSetting, 0x4D94A0);
    RH_ScopedInstall(DisableBassEq, 0x4D94D0);
    RH_ScopedInstall(EnableBassEq, 0x4D94E0);
    RH_ScopedInstall(SetChannelFlags, 0x4D9500);
    RH_ScopedInstall(SetMusicMasterScalingFactor, 0x4D9530);
    RH_ScopedInstall(SetEffectsMasterScalingFactor, 0x4D9540);
    RH_ScopedInstall(SetMusicFaderScalingFactor, 0x4D9550);
    RH_ScopedInstall(SetEffectsFaderScalingFactor, 0x4D9560);
    RH_ScopedInstall(GetEffectsMasterScalingFactor, 0x4D9570);
    RH_ScopedInstall(GetMusicMasterScalingFactor, 0x4D9580);
    RH_ScopedInstall(GetEffectsFaderScalingFactor, 0x4D9590);
    RH_ScopedInstall(SetNonStreamFaderScalingFactor, 0x4D95A0);
    RH_ScopedInstall(SetStreamFaderScalingFactor, 0x4D95B0);
    RH_ScopedInstall(IsStreamingFromDVD, 0x4D95C0);
    RH_ScopedInstall(GetDVDDriveLetter, 0x4D95D0);
    RH_ScopedInstall(CheckDVD, 0x4D95E0);
    RH_ScopedInstall(PauseAllSounds, 0x4D95F0);
    RH_ScopedInstall(ResumeAllSounds, 0x4D9630);
    RH_ScopedInstall(InitDirectSoundListener, 0x4D9640);
    RH_ScopedInstall(Terminate, 0x4D97A0);
    RH_ScopedInstall(Service, 0x4D9870);
    RH_ScopedInstall(Initialise, 0x4D9930);
}

// 0x4D83E0
// Como el original: sólo estos campos se inicializan; el resto queda tal cual.
CAEAudioHardware::CAEAudioHardware() {
    for (auto i = 0; i < MAX_NUM_AUDIO_CHANNELS; i++) {
        m_anNumChannelsInSlot[i] = 0;
        m_afChannelVolumes[i]    = -1000.0f;
        m_awChannelFlags[i]      = 0;
    }
    // m_VirtualChannelSettings: su constructor pone BankSlotIDs/SoundIDs a -1.
    m_fMusicMasterScalingFactor    = 1.0f;
    m_fEffectMasterScalingFactor   = 1.0f;
    m_fMusicFaderScalingFactor     = 1.0f;
    m_fEffectsFaderScalingFactor   = 1.0f;
    m_fNonStreamFaderScalingFactor = 1.0f;
    m_fStreamFaderScalingFactor    = 1.0f;
    m_nNumAvailableChannels        = 0;
    m_n3dEffectsQueryResult        = 0;
    m_bDisableEffectsLoading       = false;
    m_bInitialised                 = false;
    m_nReverbEnvironment           = (uint8)-1;
    m_nReverbDepth                 = -10000;
}

// 0x4D9930
bool CAEAudioHardware::Initialise() {
    m_pMP3BankLoader  = new CAEMP3BankLoader;
    m_pMP3TrackLoader = new CAEMP3TrackLoader;
    if (!m_pMP3BankLoader->Initialise() || !m_pMP3TrackLoader->Initialise()) {
        NOTSA_LOG_ERR("Failed initializing MP3 module!");
        return false;
    }

    CoInitialize(nullptr); // Como el original: se ignora el resultado.

#if 0 // No EAX
    if (FAILED(EAXDirectSoundCreate(
        const_cast<GUID*>(&DSDEVID_DefaultPlayback),
        reinterpret_cast<IDirectSound**>(&m_pDSDevice),
        0
    ))) {
        return false;
    }
#else
    // Como el original: DirectSoundCreate8(NULL, ...) (dispositivo por defecto).
    if (FAILED(DirectSoundCreate8(nullptr, &m_pDSDevice, 0))) {
        return false;
    }
#endif

    m_dsCaps.dwSize = sizeof(DSCAPS);
    m_pDSDevice->GetCaps(&m_dsCaps);

    if (FAILED(m_pDSDevice->SetCooperativeLevel(PSGLOBAL(window), DSSCL_PRIORITY))
        || !InitDirectSoundListener(2, 48'000, 16))
        return false;

    m_pDSDevice->GetSpeakerConfig((LPDWORD)&m_nSpeakerConfig);

    m_pStreamingChannel = new CAEStreamingChannel(m_pDSDevice, 0);
    m_pStreamingChannel->Initialise();

    const uint32 numFree3dBuffers = m_dsCaps.dwFreeHw3DAllBuffers;
    if (numFree3dBuffers < 24) {
        m_nNumChannels = 48;
        AESmoothFadeThread.m_nNumAvailableBuffers = 48;
        m_IsHardwareMixAvailable = false;
    } else {
        m_nNumChannels = (uint16)std::min(numFree3dBuffers, 64u) - 7;
        m_IsHardwareMixAvailable = true;
        AESmoothFadeThread.m_nNumAvailableBuffers = 7;
    }

    for (auto i = 1u; i < m_nNumChannels; i++) {
        m_aChannels[i] = new CAEStaticChannel(m_pDSDevice, i, m_IsHardwareMixAvailable, 44'100, 16);
    }
    m_aChannels[0] = m_pStreamingChannel; // Como el original: se escribe después del bucle.

    m_pStreamingChannel->SetVolume(-100.0f);
    m_awChannelFlags[0] = 55; // todo: flag

    if (FrontEndMenuManager.m_bTracksAutoScan) {
        AEUserRadioTrackManager.ScanUserTracks();

        // Como el original: lee el estado una vez por iteración; COMPLETE/ERROR -> OFF y
        // sale; IN_PROGRESS -> Sleep(100) y repite; cualquier otro valor sale.
        auto& state = AEUserRadioTrackManager.m_nUserTracksScanState;
        for (;;) {
            const auto current = *static_cast<volatile eUserTrackScanState*>(&state);
            if (current == USER_TRACK_SCAN_COMPLETE || current == USER_TRACK_SCAN_ERROR) {
                state = USER_TRACK_SCAN_OFF;
            }
            if (current != USER_TRACK_SCAN_IN_PROGRESS) {
                break;
            }
            Sleep(100);
        }
    }

    AEUserRadioTrackManager.Initialise();
    AESmoothFadeThread.Initialise();
    AESmoothFadeThread.Start();
    m_pStreamThread.Initialise(m_pStreamingChannel);
    m_pStreamThread.Start();

    m_nNumAvailableChannels = m_nNumChannels;
    m_bInitialised = true;

    return true;
}

// 0x4D9640
bool CAEAudioHardware::InitDirectSoundListener(uint32 numChannels, uint32 samplesPerSec, uint32 bitsPerSample) {
    if (!m_pDSDevice)
        return false;

    DSBUFFERDESC dsBuffDsc{
        sizeof(DSBUFFERDESC),
        DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER,
        0,
        0,
        0,
    };
    LPDIRECTSOUNDBUFFER soundbuf;
    if (FAILED(m_pDSDevice->CreateSoundBuffer(&dsBuffDsc, &soundbuf, NULL))) {
        return false;
    }

    // Como el original: (WORD)bits >> 3, multiplicado por los canales y truncado a WORD;
    // nAvgBytesPerSec usa ese nBlockAlign ya truncado.
    const auto nBlockAlign = (WORD)((uint32)((WORD)bitsPerSample >> 3) * numChannels);
    WAVEFORMATEX wavFmt{
        .wFormatTag      = 1,
        .nChannels       = (WORD)numChannels,
        .nSamplesPerSec  = samplesPerSec,
        .nAvgBytesPerSec = (uint32)nBlockAlign * samplesPerSec,
        .nBlockAlign     = nBlockAlign,
        .wBitsPerSample  = (WORD)bitsPerSample,
        .cbSize          = 0
    };
    if (FAILED(soundbuf->SetFormat(&wavFmt))) {
#ifdef FIX_BUGS
        soundbuf->Release();
#endif
        return false;
    }

    auto& listener = m_pDirectSound3dListener;
    if (FAILED(soundbuf->QueryInterface(
        IID_IDirectSound3DListener,
        (LPVOID*)&listener
    ))) {
        soundbuf->Release();
        return false;
    }

    listener->SetPosition(0.f, 0.f, 0.f, TRUE);
    listener->SetOrientation(
        0.f, 1.f, 0.f,  // In Front
        0.f, 0.f, -1.f, // Bottom
        TRUE
    );
    listener->SetRolloffFactor(0.f, TRUE);
    listener->SetDopplerFactor(0.f, TRUE);
    listener->CommitDeferredSettings();

    soundbuf->Release();

    Query3DSoundEffects();

    return true;
}

// 0x4D97A0
void CAEAudioHardware::Terminate() {
    m_bInitialised = false;
    m_pStreamThread.Stop();
    AESmoothFadeThread.Stop();
    m_pStreamThread.WaitForExit();
    AESmoothFadeThread.WaitForExit(); // Como el original (0x4EEA40).
    // Como el original: recorre los 64 slots (no m_nNumChannels) y los pone a nullptr.
    for (auto& ch : m_aChannels) {
        if (ch) {
            delete ch;
            ch = nullptr;
        }
    }
    if (m_pMP3BankLoader) {
        delete m_pMP3BankLoader;
        m_pMP3BankLoader = nullptr;
    }
    if (m_pMP3TrackLoader) {
        delete m_pMP3TrackLoader;
        m_pMP3TrackLoader = nullptr;
    }
    SAFE_RELEASE(m_pDirectSound3dListener);
    SAFE_RELEASE(m_pDSDevice);
}

void CAEAudioHardware::PlaySound(int16 channel, uint16 channelSlot, uint16 soundIdInSlot, uint16 bankSlot, int16 playPosition, int16 flags, float speed) {
    plugin::CallMethod<0x4D86B0, CAEAudioHardware*, int16, uint16, uint16, uint16, int16, int16, float>(this, channel, channelSlot, soundIdInSlot, bankSlot, playPosition, flags, speed);
}

// 0x5B9340
int16 CAEAudioHardware::AllocateChannels(uint16 numChannels) {
    if (!numChannels || numChannels > m_nNumAvailableChannels) {
        return -1;
    }

    size_t slot = 0;
    while (slot < MAX_NUM_AUDIO_CHANNELS && m_anNumChannelsInSlot[slot]) {
        slot += m_anNumChannelsInSlot[slot];
    }

    if (slot >= MAX_NUM_AUDIO_CHANNELS) {
        return -1;
    }

    m_anNumChannelsInSlot[slot] = numChannels;
    m_nNumAvailableChannels    -= numChannels;

    return (int16)slot;
}

// 0x4D94A0
void CAEAudioHardware::SetBassSetting(eBassSetting bassSetting, float bassGain) {
    m_BassSetting = static_cast<int32>(static_cast<int8>(bassSetting)); // Como el original: movsx a dword.
    m_BassGain    = bassGain;
    m_pStreamingChannel->SetBassEQ(bassSetting, bassGain);
}

// 0x4D8810
uint16 CAEAudioHardware::GetNumAvailableChannels() const {
    return m_nNumAvailableChannels;
}

// 0x4D8820
void CAEAudioHardware::GetChannelPlayTimes(int16 channel, int16* playTimes) {
    // Como el original: comparación sin signo de `channel` con `channel + n - 1`, de modo que
    // sólo se copian n - 1 tiempos y en orden ascendente (con n == 0 y channel == 0 desborda igual).
    const uint32 numInSlot = m_anNumChannelsInSlot[channel];
    if (!playTimes)
        return;

    const uint32 first = (uint32)(int32)channel;
    const uint32 last  = numInSlot + first - 1;
    if (first >= last)
        return;

    for (uint32 k = 0; k < last - first; k++) {
        playTimes[k] = m_aChannels[first + k]->GetPlayTime();
    }
}

// 0x4D8870
void CAEAudioHardware::SetChannelVolume(int16 channel, uint16 channelId, float volume, uint8 a4) {
    if (channel >= 0 && channelId < m_anNumChannelsInSlot[channel])
        m_afChannelVolumes[channel + channelId] = volume;
}

// 0x4D88A0
void CAEAudioHardware::LoadSoundBank(eSoundBank bank, eSoundBankSlot slot) {
    if (!m_bDisableEffectsLoading) {
        m_pMP3BankLoader->LoadSoundBank(bank, slot);
    }
}

// 0x4D88C0
bool CAEAudioHardware::IsSoundBankLoaded(eSoundBank bank, eSoundBankSlot slot) {
    return m_pMP3BankLoader->IsSoundBankLoaded(bank, slot);
}

// 0x4D88D0
int8 CAEAudioHardware::GetSoundBankLoadingStatus(eSoundBank bank, eSoundBankSlot slot) {
    return m_pMP3BankLoader->GetSoundBankLoadingStatus(bank, slot);
}

// notsa
bool CAEAudioHardware::EnsureSoundBankIsLoaded(eSoundBank bank, eSoundBankSlot slot, bool checkLoadingTune, bool cancelSoundsInSlot) {
    if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
        return true;
    }
    if (cancelSoundsInSlot) {
        if (AESoundManager.AreSoundsPlayingInBankSlot(slot)) {
            AESoundManager.CancelSoundsInBankSlot(slot, false);
        }
    }
    if (checkLoadingTune && AudioEngine.IsLoadingTuneActive()) {
        return false;
    }
    LoadSoundBank(bank, slot);
    return false;
}

// 0x4D8ED0
void CAEAudioHardware::LoadSound(eSoundBank bank, eSoundID sound, eSoundBankSlot slot) {
    if (!m_bDisableEffectsLoading) {
        m_pMP3BankLoader->LoadSound(bank, sound, slot);
    }
}

// 0x4D8EF0
bool CAEAudioHardware::IsSoundLoaded(eSoundBank bank, eSoundID sfx, eSoundBankSlot slot) {
    return m_pMP3BankLoader->IsSoundLoaded(bank, sfx, slot);
}

// 0x4D8F00
bool CAEAudioHardware::GetSoundLoadingStatus(eSoundBank bank, eSoundID sfx, eSoundBankSlot slot) {
    return m_pMP3BankLoader->GetSoundLoadingStatus(bank, sfx, slot);
}

// 0x4D88E0
void CAEAudioHardware::StopSound(int16 channel, uint16 channelId) const {
    if (channel >= 0 && channelId < m_anNumChannelsInSlot[channel]) {
        const auto ch = m_aChannels[channel + channelId];
        if (ch) {
            ch->Stop();
        }
    }
}

// 0x4D8920
void CAEAudioHardware::SetChannelPosition(int16 slotId, uint16 channelId, const CVector& posn, uint8 unused) const {
    if (slotId >= 0 && channelId < m_anNumChannelsInSlot[slotId]) {
        const auto ch = m_aChannels[slotId + channelId];
        if (ch) {
            ch->SetPosition(posn);
        }
    }
}

// 0x4D8960
void CAEAudioHardware::SetChannelFrequencyScalingFactor(int16 channel, uint16 channelId, float factor) {
    if (channel >= 0 && channelId < m_anNumChannelsInSlot[channel]) {
        m_afChannelsFrqScalingFactor[channel + channelId] = factor;
    }
}

// 0x4D8990
// Recompute all channel gains and push them to the hardware. Runs in 3 passes:
//  1. Mark idle channels as silent (-100.0f) and find the loudest active volume
//     per group (global + secondary).
//  2. Normalize each channel's volume by its group max (or clamp it to <= 0),
//     convert it to a linear gain (10 ^ (dB / 20)) and accumulate the sums used
//     for headroom control (channels near their end only count their remaining play time).
//  3. Scale the gains by the master scale and category faders, convert back to
//     dB (20 * log10) and push volume + frequency scaling to the hardware.
// Between passes 1 and 2 each group max is released toward the previous frame's
// value (0.5 step if that frame was slow-fadeout, else 1.2), so ducked volumes
// fade back in instead of popping.
void CAEAudioHardware::RescaleChannelVolumes() {
    float maxVolumeGlobal           = 0.0f;
    float maxVolumeSecondary        = 0.0f;
    float scaleSum                  = 0.0f;
    float fullSum                   = 0.0f;
    bool  isMaxGlobalSlowFadeout    = false;
    bool  isMaxSecondarySlowFadeout = false;

    for (auto&& [i, ch] : rngv::enumerate(GetChannels())) {
        if (!ch || ch->GetPlayTime() == -1) {
            m_afChannelVolumes[i] = -100.0f;
        } else {
            if (maxVolumeGlobal < m_afChannelVolumes[i]) {
                maxVolumeGlobal        = m_afChannelVolumes[i];
                isMaxGlobalSlowFadeout = (m_awChannelFlags[i] & FLAG_SLOW_FADEOUT) != 0;
            }
            if ((m_awChannelFlags[i] & FLAG_SECONDARY_GROUP) && maxVolumeSecondary < m_afChannelVolumes[i]) {
                maxVolumeSecondary        = m_afChannelVolumes[i];
                isMaxSecondarySlowFadeout = (m_awChannelFlags[i] & FLAG_SLOW_FADEOUT) != 0;
            }
        }
    }

    // Release the group maxima toward the previous frame's values (ducking fade-out)
    // TODO: FPS dependent logic
    if (maxVolumeGlobal < m_PrevMaxVolumeGlobal) {
        if (m_PrevMaxGlobalSlowFadeout) {
            const float vol = m_PrevMaxVolumeGlobal - 0.5f;
            if (vol > maxVolumeGlobal) {
                maxVolumeGlobal        = vol;
                isMaxGlobalSlowFadeout = true;
            }
        } else {
            const float vol = m_PrevMaxVolumeGlobal - 1.2f;
            if (vol > maxVolumeGlobal) {
                maxVolumeGlobal = vol;
            }
        }
    }

    // TODO: FPS dependent logic
    if (maxVolumeSecondary < m_PrevMaxVolumeSecondary) {
        if (m_PrevMaxSecondarySlowFadeout) {
            const float vol = m_PrevMaxVolumeSecondary - 0.5f;
            if (vol > maxVolumeSecondary) {
                maxVolumeSecondary        = vol;
                isMaxSecondarySlowFadeout = true;
            }
        } else {
            const float vol = m_PrevMaxVolumeSecondary - 1.2f;
            if (vol > maxVolumeSecondary) {
                maxVolumeSecondary = vol;
            }
        }
    }

    for (auto&& [i, ch] : rngv::enumerate(GetChannels())) {
        const auto flags = m_awChannelFlags[i];
        auto&      vol   = m_afChannelVolumes[i];

        if (flags & FLAG_CLAMP_VOL_TO_NEG) {
            // Como el original: sólo conserva vol si vol < 0 (NaN y -0.0 pasan a +0.0).
            vol = vol < 0.0f ? vol : 0.0f;
        } else if (flags & FLAG_SECONDARY_GROUP) {
            vol -= maxVolumeSecondary;
        } else {
            vol -= maxVolumeGlobal;
        }

        // NOTE: `pow` in double precision on purpose - the original computes it on the
        // x87 FPU in temp-real precision (`FLD qword [10.0]` @ 0x4D8B58).
        m_ChannelGains[i] = static_cast<float>(std::pow(10.0, vol * 0.05f));

        if (flags & FLAG_UNDUCKABLE) {
            fullSum += m_ChannelGains[i];
        } else if ((flags & FLAG_FADE_NEAR_END) && ch && ch->GetPlayTime() >= 0) {
            // NOTE: `GetPlayTime()` is called twice on purpose (0x4D8B9A / 0x4D8BBB) -
            // it reads live playback state, so caching it would deviate from the original.
            // NOTE: `static_cast<int16>` reproduces the original's MOVSX sign-extension (0x4D8BB2).
            const auto  length    = static_cast<int16>(ch->GetLength());
            const auto  playTime  = ch->GetPlayTime();
            // Como el original: fild exacto de la diferencia, fmul y fidiv redondeados a float.
            const float scaled    = static_cast<float>(static_cast<double>(length - playTime) * m_ChannelGains[i]);
            const float remaining = static_cast<float>(static_cast<double>(scaled) / static_cast<double>(length));
            scaleSum += remaining < 0.0f ? 0.0f : std::min(remaining, m_ChannelGains[i]);
        } else {
            scaleSum += m_ChannelGains[i];
        }
    }

    m_PrevMaxVolumeGlobal         = maxVolumeGlobal;
    m_PrevMaxVolumeSecondary      = maxVolumeSecondary;
    m_PrevMaxSecondarySlowFadeout = isMaxSecondarySlowFadeout;
    m_PrevMaxGlobalSlowFadeout    = isMaxGlobalSlowFadeout;

    // NOTE: Operation order matters - the original does FSUBR -> FMUL -> FDIV, so
    // reassociating `(6.4f - fullSum * 0.8f) * 16383.0f / scaleSum` changes the result.
    // NOTE: Exact `== 0.0f` compare on purpose - matches the original's FCOMP against 0.0.
    const float masterScale = scaleSum == 0.0f
        ? 0.0f
        : std::min((6.4f - fullSum * 0.8f) * 16383.0f / scaleSum, 16383.0f);

    for (auto&& [i, ch] : rngv::enumerate(GetChannels())) {
        const auto flags = m_awChannelFlags[i];
        auto&      gain  = m_ChannelGains[i];

        gain *= (flags & FLAG_UNDUCKABLE) ? 16383.0f : masterScale;

        if (flags & FLAG_IS_MUSIC) {
            gain *= m_fMusicMasterScalingFactor * m_fMusicFaderScalingFactor;
        } else {
            gain *= m_fEffectMasterScalingFactor * m_fEffectsFaderScalingFactor;
        }

        gain *= (flags & FLAG_IS_NOT_STREAM) ? m_fNonStreamFaderScalingFactor : m_fStreamFaderScalingFactor;

        if (ch) {
            // NOTE: Exact `== 0.0f` compare on purpose - the original does FCOMP against
            // 0.0 and sends NaN gains down the log path.
            if (gain == 0.0f) {
                ch->SetVolume(-100.0f);
            } else {
                ch->SetVolume(20.0f * CAEAudioUtility::AudioLog10(gain * (1.0f / 16383.0f)));
            }
            ch->SetFrequencyScalingFactor(m_afChannelsFrqScalingFactor[i]);
        }
    }
}

// 0x4D8DA0
void CAEAudioHardware::UpdateReverbEnvironment() {
    int8 reverb;
    int32 depth;
    CAEAudioEnvironment::GetReverbEnvironmentAndDepth(&reverb, &depth);
    // Como el original: comparación de bytes (cmp dl,al).
    if ((uint8)reverb == m_nReverbEnvironment && depth == m_nReverbDepth) { // Nothing is changing?
        return;
    }

    // Como el original: se guarda el nuevo entorno sólo cuando algún canal acepta el cambio
    // (se detiene en el primero); sin canales o si ninguno lo acepta no se guarda nada.
    for (const auto ch : GetChannels()) {
        if (ch && ch->SetReverbAndDepth(reverb, depth)) {
            m_nReverbEnvironment = reverb;
            m_nReverbDepth       = depth;
            return;
        }
    }
}

// 0x4D8E30
float CAEAudioHardware::GetSoundHeadroom(eSoundID sfx, eSoundBankSlot slot) {
    return m_pMP3BankLoader->GetSoundHeadroom(sfx, slot);
}

// 0x4D8E40
void CAEAudioHardware::EnableEffectsLoading() {
    m_bDisableEffectsLoading = false; // weird name
}

// 0x4D8E50
void CAEAudioHardware::DisableEffectsLoading() {
    m_bDisableEffectsLoading = true; // weird name
}

// 0x4D8E60
void CAEAudioHardware::RequestVirtualChannelSoundInfo(uint16 vch, eSoundID sfx, eSoundBankSlot slot) {
    if (vch >= MAX_NUM_SOUNDS) { // Como el original: índice fuera de rango -> no hace nada.
        return;
    }

    m_VirtualChannelSettings.BankSlotIDs[vch] = slot;
    m_VirtualChannelSettings.SoundIDs[vch]    = sfx;
}

// 0x4D8E90
void CAEAudioHardware::GetVirtualChannelSoundLengths(int16* soundLengths) const {
    memcpy(soundLengths, m_VirtualChannelSoundLengths, sizeof(m_VirtualChannelSoundLengths));
}

// 0x4D8EB0
void CAEAudioHardware::GetVirtualChannelSoundLoopStartTimes(int16* soundLoopStartTimes) const {
    memcpy(soundLoopStartTimes, m_VirtualChannelLoopTimes, sizeof(m_VirtualChannelLoopTimes)); // size is 600, replaced by loop?
}

// 0x4D8F10
void CAEAudioHardware::PlayTrack(uint32 trackID, int nextTrackID, uint32 startOffsetMs, uint8 trackFlags, bool bUserTrack, bool bUserNextTrack) {
    m_PlayingTrackFlags = trackFlags;
    m_pStreamThread.PlayTrack(trackID, nextTrackID, startOffsetMs, trackFlags, bUserTrack, bUserNextTrack);
}

// 0x4D8F30
void CAEAudioHardware::StartTrackPlayback() const {
    m_pStreamingChannel->Play(0, m_PlayingTrackFlags, 1.0f);
}

// 0x4D8F50
void CAEAudioHardware::StopTrack() {
    m_pStreamThread.StopTrack();
}

// 0x4D8F60
int32 CAEAudioHardware::GetTrackPlayTime() const {
    return m_pStreamThread.GetTrackPlayTime();
}

// 0x4D8F70
int32 CAEAudioHardware::GetTrackLengthMs() const {
    return m_pStreamThread.GetTrackLengthMs();
}

// 0x4D8F80
int32 CAEAudioHardware::GetActiveTrackID() const {
    return m_pStreamThread.GetActiveTrackID();
}

// 0x4D8F90
int32 CAEAudioHardware::GetPlayingTrackID() const {
    return m_pStreamThread.GetPlayingTrackID();
}

// 0x4D8FA0
// Como el original: actualiza gBeatInfo, lo copia entero a `beatInfo` y libera el tTrackInfo
// con operator delete. Los tiempos de la ventana son relativos al inicio de la ventana de
// búsqueda (playTime, o playTime - 50 acotado a 0 si ya había un beat anterior), con
// comparaciones con signo y sin comprobar límites al buscar.
void CAEAudioHardware::GetBeatInfo(tBeatInfo* beatInfo) {
    auto& bi = gBeatInfo;

    if (m_pStreamThread.GetActiveTrackID() == -1) {
        bi.IsBeatInfoPresent = 0;
        rng::fill(bi.BeatWindow, tTrackInfo::tBeat{});
        return; // Como el original: no copia nada a `beatInfo`.
    }

    tTrackInfo* const trkInfo = m_pMP3TrackLoader->GetTrackInfo(m_pStreamThread.GetActiveTrackID());
    int32             time    = m_pStreamThread.GetTrackPlayTime();

    const tTrackInfo::tBeat* const beats = trkInfo->m_aBeats.data();
    const auto beatTime = [&](int32 idx) { return static_cast<int32>(beats[idx].m_nTime); };
    const auto beatKey  = [&](int32 idx) { return static_cast<int32>(beats[idx].m_nKey); };
    const auto setWindow = [&](int32 slot, int32 idx) {
        bi.BeatWindow[slot].m_nTime = static_cast<uint32>(beatTime(idx) - time);
        bi.BeatWindow[slot].m_nKey  = beats[idx].m_nKey;
    };
    // 0x4D9088 / 0x4D933A - hasta 10 beats anteriores a `i`, en las posiciones 9..0
    const auto copyPrevious = [&](int32 i) {
        for (int32 k = 0; k < 10; k++) {
            if (i - 1 - k >= 0) {
                setWindow(9 - k, i - 1 - k);
            }
        }
    };

    if (time < 0 || beatTime(0) < 0) {
        if (time != -7) {
            bi.IsBeatInfoPresent = 0;
            rng::fill(bi.BeatWindow, tTrackInfo::tBeat{});
        }
    } else {
        const int32 prevBeatNumber = bi.BeatNumber;
        if (prevBeatNumber >= 0) {
            time = std::max(time - 50, 0);
        }

        bi.IsBeatInfoPresent = 1;
        bi.BeatTypeThisFrame = 0;
        bi.BeatNumber        = -1;
        rng::fill(bi.BeatWindow, tTrackInfo::tBeat{});

        //> 0x4D904C - Primer beat dentro de la ventana
        int32 i = 0;
        while (beatTime(i) < time && beatKey(i) > 0) {
            i++;
        }

        if (beatKey(i) > 0) {
            // 0x4D9074
            if (i != prevBeatNumber && i > 0) {
                bi.BeatTypeThisFrame = beatKey(i - 1);
            }
            copyPrevious(i);

            // 0x4D91A1 - hasta 10 beats desde `i`, cada uno comprobado por separado
            for (int32 k = 0; k < 10; k++) {
                if (i + k < (int32)std::size(trkInfo->m_aBeats) && beatKey(i + k) > 0) {
                    setWindow(10 + k, i + k);
                }
            }
            bi.BeatNumber = i;
        } else if (i > 0) {
            copyPrevious(i); // 0x4D9332
        }
    }

    *beatInfo = bi;
    operator delete(trkInfo); // Como el original: operator delete (0x8214BD) sin destructor.
}

// 0x4D94E0
void CAEAudioHardware::EnableBassEq() {
    // Como el original: pasa el byte bajo extendido sin signo (movzx).
    m_pStreamingChannel->SetBassEQ(static_cast<eBassSetting>(static_cast<uint8>(m_BassSetting)), m_BassGain);
}

// 0x4D94D0
void CAEAudioHardware::DisableBassEq() {
    m_pStreamingChannel->SetBassEQ(eBassSetting::NORMAL, 0.f);
}

// 0x4D9500
void CAEAudioHardware::SetChannelFlags(int16 channel, uint16 channelId, int16 flags) {
    if (channel >= 0 && channelId < m_anNumChannelsInSlot[channel])
        m_awChannelFlags[channel + channelId] = flags;
}

// 0x4D9530
void CAEAudioHardware::SetMusicMasterScalingFactor(float factor) {
    m_fMusicMasterScalingFactor = factor;
}

// 0x4D9540
void CAEAudioHardware::SetEffectsMasterScalingFactor(float factor) {
    m_fEffectMasterScalingFactor = factor;
}

// 0x4D9550
void CAEAudioHardware::SetMusicFaderScalingFactor(float factor) {
    m_fMusicFaderScalingFactor = factor;
}

// 0x4D9560
void CAEAudioHardware::SetEffectsFaderScalingFactor(float factor) {
    m_fEffectsFaderScalingFactor = factor;
}

// 0x4D9570
float CAEAudioHardware::GetEffectsMasterScalingFactor() const {
    return m_fEffectMasterScalingFactor;
}

// 0x4D9580
float CAEAudioHardware::GetMusicMasterScalingFactor() const {
    return m_fMusicMasterScalingFactor;
}

// 0x4D9590
float CAEAudioHardware::GetEffectsFaderScalingFactor() const {
    return m_fEffectsFaderScalingFactor;
}

// 0x4D95A0
void CAEAudioHardware::SetNonStreamFaderScalingFactor(float factor) {
    m_fNonStreamFaderScalingFactor = factor;
}

// 0x4D95B0
void CAEAudioHardware::SetStreamFaderScalingFactor(float factor) {
    m_fStreamFaderScalingFactor = factor;
}

// 0x4D95C0
bool CAEAudioHardware::IsStreamingFromDVD() {
    return m_pMP3TrackLoader->m_bStreamingFromDVD;
}

// 0x4D95D0
char CAEAudioHardware::GetDVDDriveLetter() {
    return m_pMP3TrackLoader->m_pszDvdDrivePath[0];
}

// 0x4D95E0
bool CAEAudioHardware::CheckDVD() {
    return m_pMP3TrackLoader->IsCurrentAudioStreamAvailable();
}

// 0x4D95F0
void CAEAudioHardware::PauseAllSounds() {
    if (!m_bInitialised) {
        return;
    }
    for (const auto ch : GetChannels()) {
        ch->SetFrequencyScalingFactor(0.f);
    }
}

// 0x4D9630
void CAEAudioHardware::ResumeAllSounds() {
    if (!m_bInitialised) {
        return;
    }
    RescaleChannelVolumes();
}

// GUID de propiedades EAX del EXE (la DLL no incluye las cabeceras EAX)
static inline auto& s_EAXContextPropertySet = StaticRef<GUID>(0x85EFEC);
static inline auto& s_EAXFXSlot0PropertySet = StaticRef<GUID>(0x85EFDC);

// 0x4D8490
// Como el original: crea un buffer 3D de prueba (PCM mono 44100 Hz 16 bits) y, si el
// buffer 3D expone IKsPropertySet, intenta fijar las propiedades EAX (slot 0, entorno
// ROOM y room -1000); si todo funciona m_n3dEffectsQueryResult = 1, si no 2.
void CAEAudioHardware::Query3DSoundEffects() {
    WAVEFORMATEX format;
    format.wFormatTag      = 1; // WAVE_FORMAT_PCM
    format.nChannels       = 1;
    format.nSamplesPerSec  = 44'100;
    format.nAvgBytesPerSec = 88'200;
    format.nBlockAlign     = 2;
    format.wBitsPerSample  = 16;
    format.cbSize          = 0;

    DSBUFFERDESC bufferDesc;
    bufferDesc.dwSize          = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags         = DSBCAPS_CTRL3D;
    bufferDesc.dwBufferBytes   = 1024;
    bufferDesc.dwReserved      = 0;
    bufferDesc.lpwfxFormat     = &format; // Como el original: no es nullptr.
    bufferDesc.guid3DAlgorithm = GUID_NULL; // 0x859150 (ceros)

    IDirectSoundBuffer* buffer{};
    if (FAILED(m_pDSDevice->CreateSoundBuffer(&bufferDesc, &buffer, nullptr)))
        return;

    IDirectSound3DBuffer* buffer3D{};
    if (FAILED(buffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&buffer3D)))
        return; // Como el original: sale sin liberar el buffer.

    if (buffer && buffer3D) {
        IKsPropertySet* propertySet{};
        if (buffer3D->QueryInterface(IID_IKsPropertySet, (LPVOID*)&propertySet) == S_OK) {
            GUID  fxSlot0     = s_EAXFXSlot0PropertySet;
            int32 environment = 2;     // EAX_ENVIRONMENT_ROOM
            int32 room        = -1000; // EAXREVERB_DEFAULTROOM
            if (SUCCEEDED(propertySet->Set(s_EAXContextPropertySet, 2, nullptr, 0, &fxSlot0, sizeof(fxSlot0)))       // EAXCONTEXT_PRIMARYFXSLOTID
                && SUCCEEDED(propertySet->Set(s_EAXFXSlot0PropertySet, 2, nullptr, 0, &environment, sizeof(environment))) // EAXREVERB_ENVIRONMENT
                && SUCCEEDED(propertySet->Set(s_EAXFXSlot0PropertySet, 5, nullptr, 0, &room, sizeof(room)))               // EAXREVERB_ROOM
            ) {
                m_n3dEffectsQueryResult = 1;
            }
            SAFE_RELEASE(propertySet);
        }
        if (!m_n3dEffectsQueryResult)
            m_n3dEffectsQueryResult = 2;
    }

    SAFE_RELEASE(buffer3D);
    SAFE_RELEASE(buffer);
}

// 0x4D9870
void CAEAudioHardware::Service() {
    IsForegroundApp(); // Como el original: se llama (0x746060) y se ignora el resultado.
    m_pDirectSound3dListener->CommitDeferredSettings(); // Como el original: sin comprobar el HRESULT.
    RescaleChannelVolumes();
    if (m_n3dEffectsQueryResult) {
        UpdateReverbEnvironment();
    }
    for (const auto ch : GetChannels() | rng::views::drop(1)) {
        ch->SynchPlayback();
    }
    for (const auto ch : GetChannels() | rng::views::drop(1)) {
        ch->Service();
    }
    m_pMP3BankLoader->UpdateVirtualChannels(
        &m_VirtualChannelSettings,
        m_VirtualChannelSoundLengths,
        m_VirtualChannelLoopTimes
    );
    m_pMP3BankLoader->Service();
}

const CAEBankSlot& CAEAudioHardware::GetBankSlot(eSoundBankSlot slot) const {
    return m_pMP3BankLoader->GetBankSlot(slot);
}
