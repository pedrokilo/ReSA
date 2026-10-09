/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEStreamingChannel
 * Reconstrucción: COMPLETA — 24/24 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 24/24 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <extensions/utility.hpp>

#include "AEStreamingChannel.h"
#include "AEAudioUtility.h"

//! Stereo channel FX Param EQ presets [0x8CBA70]
static inline DSFXParamEq s_FXParamEqPresets[(+eBassSetting::NUM)][2]{
    {{ 0.0f, 0.0f,  0.0f }, { 0.f,   0.0f,  0.0f }}, // Preset NORMAL [Unused]
    {{ 80.f, 30.f,  4.0f }, { 180.f, 30.f,  1.5f }}, // Preset BOOST
    {{ 80.f, 36.f, -15.f }, { 80.f,  36.f, -15.f }}, // Preset CUT
};

// 0x4F2200
CAEStreamingChannel::~CAEStreamingChannel() {
    DirectSoundBufferFadeToSilence();

    m_nState              = StreamingChannelState::Stopping;
    m_lStoppingFrameCount = 0;

    if (m_pDirectSoundBuffer8) {
        m_pDirectSoundBuffer8->SetFX(0, nullptr, nullptr);
    }

    if (m_pSilenceBuffer) {
        std::exchange(m_pSilenceBuffer, nullptr)->Release();
    }

    if (m_pDirectSoundBuffer) {
        std::exchange(m_pDirectSoundBuffer, nullptr)->Release();
    }

    if (m_pStreamingDecoder) {
        delete std::exchange(m_pStreamingDecoder, nullptr);
    }

    if (m_pNextStreamingDecoder) {
        delete std::exchange(m_pNextStreamingDecoder, nullptr);
    }
}

// 0x4F22F0
bool CAEStreamingChannel::Initialise() {
    VERIFY(SUCCEEDED(CoInitialize(nullptr)));
    bool created = false; // Como el original: bl = 1 solo si CreateSoundBuffer tiene exito; es el valor de retorno (mov al,bl, 0x4F23BE)

    DSBUFFERDESC bufferDesc{};
    bufferDesc.guid3DAlgorithm = GUID_NULL;
    bufferDesc.lpwfxFormat = &m_WaveFormat;
    bufferDesc.dwSize = 36;
    bufferDesc.dwBufferBytes = 0xC0000;
    bufferDesc.dwReserved = 0;
    bufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLFX | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE;

    m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_WaveFormat.cbSize = 0;
    m_WaveFormat.nChannels = 2;
    m_WaveFormat.wBitsPerSample = 16;
    m_WaveFormat.nSamplesPerSec = 48000;
    m_WaveFormat.nAvgBytesPerSec = 192000;
    m_WaveFormat.nBlockAlign = 4;

    if (SUCCEEDED(m_pDirectSound->CreateSoundBuffer(
        &bufferDesc,
        &m_pDirectSoundBuffer,
        0
    ))) {
        m_bInitialized = true;
        created = true;
        SetOriginalFrequency(m_WaveFormat.nSamplesPerSec);
        m_pBuffer = m_aBuffer;
        m_bLooped = true;
    }
    InitialiseSilence();
    return created;
}

// 0x4F1C70
void CAEStreamingChannel::InitialiseSilence() {
    DSBUFFERDESC bufferDesc{};
    bufferDesc.dwSize = 36;
    bufferDesc.lpwfxFormat = &m_WaveFormat;
    bufferDesc.dwBufferBytes = 0x8000;
    bufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE;

    if (SUCCEEDED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &m_pSilenceBuffer, 0))) {
        void *audioPtr;
        DWORD audioPtrBytes;

        m_pSilenceBuffer->Lock(0, 0, &audioPtr, &audioPtrBytes, nullptr, 0, DSBLOCK_ENTIREBUFFER);
        memset(audioPtr, 0, audioPtrBytes);
        m_pSilenceBuffer->Unlock(audioPtr, audioPtrBytes, nullptr, 0);
        m_pSilenceBuffer->Play(0, 0, DSBPLAY_LOOPING);

    } else {
        NOTSA_LOG_WARN("Creating silence buffer failed.");
    }
}

// 0x4F1FF0
void CAEStreamingChannel::SetReady() {
    // Como el original (0x4F1FF3 / 0x4F201D): dos comprobaciones consecutivas; tras pasar de Stopping a Stopped
    // la segunda ya ve Stopped.
    if (m_nState == StreamingChannelState::Stopping) {
        AESmoothFadeThread.CancelFade(m_pDirectSoundBuffer);
        m_pDirectSoundBuffer->Stop();
        m_nState = StreamingChannelState::Stopped;
    }
    if (m_nState == StreamingChannelState::Stopped) {
        if (m_pStreamingDecoder)
            m_nState = StreamingChannelState::UNK_MINUS_2;
    }
}

// 0x4F1F30
void CAEStreamingChannel::SetBassEQ(eBassSetting mode, float gain) {
    if (mode == eBassSetting::NORMAL) {
        if (m_bEQEnabled) {
            RemoveFX();
        }
        return;
    }

    if (!m_bEQEnabled && !AddFX()) {
        return;
    }

    for (auto ch = 0; ch < 2; ch++) {
        IDirectSoundFXParamEq* fxEqParam;
        if (FAILED(m_pDirectSoundBuffer8->GetObjectInPath(
            GUID_All_Objects,
            ch,
            IID_IDirectSoundFXParamEq,
            reinterpret_cast<LPVOID*>(&fxEqParam)
        ))) {
            continue;
        }

        DSFXParamEq savedFxExParam{ s_FXParamEqPresets[(+mode)][ch] };
        savedFxExParam.fGain *= gain;
        VERIFY(SUCCEEDED(fxEqParam->SetAllParameters(&savedFxExParam)));
        fxEqParam->Release();
    }
}

// 0x4F2060
void CAEStreamingChannel::SetFrequencyScalingFactor(float factor) {
    if (factor == 0.0f) {
        if (!m_pDirectSoundBuffer || m_nState == StreamingChannelState::Paused || !IsBufferPlaying())
            return;

        DirectSoundBufferFadeToSilence();
        m_nState = StreamingChannelState::Paused;
    } else {
        SetFrequency(static_cast<uint32>((float)m_nOriginalFrequency * factor));

        if (m_nState != StreamingChannelState::Paused)
            return;

        // Como el original (0x4F20FB..0x4F2159): sin buffer se salta directo a "m_nState = Started"
        if (m_pDirectSoundBuffer) {
            m_pDirectSoundBuffer->SetVolume(-10'000);
            m_pDirectSoundBuffer->Play(0, 0, m_bLooped ? DSBPLAY_LOOPING : 0);

            if (!AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_Volume, 35, false))
                m_pDirectSoundBuffer->SetVolume(static_cast<int32>(m_Volume * 100.0f));
        }

        m_nState = StreamingChannelState::Started;
    }
}

// 0x4F1870
void CAEStreamingChannel::SynchPlayback() {
    // NOP
}

// 0x4F1E20
uint32 CAEStreamingChannel::FillBuffer(void* buffer, uint32 size) {
    auto filled = m_pStreamingDecoder->FillBuffer(buffer, size);
    if (filled < size) {
        if (m_bLoopTrack) {
            while (filled < size) {
                m_pStreamingDecoder->SetCursor(1);
                filled += m_pStreamingDecoder->FillBuffer((uint8*)buffer + filled, size - filled);
            }
        } else {
            if (m_pNextStreamingDecoder) {
                if (m_pNextStreamingDecoder->GetSampleRate() == m_pStreamingDecoder->GetSampleRate()) {
                    if (m_pStreamingDecoder)
                        delete m_pStreamingDecoder;

                    m_pStreamingDecoder = m_pNextStreamingDecoder;
                    m_pNextStreamingDecoder = nullptr;

                    filled += m_pStreamingDecoder->FillBuffer((uint8*)buffer + filled, size - filled);
                } else {
                    memset((uint8*)buffer + filled, 0, size - filled);
                    filled = size;
                    m_bSilenced = true;
                }
            } else {
                m_nState = StreamingChannelState::Finished;
            }
        }
    }

    // Como el original (0x4F1F0B..0x4F1F1A): call [vtable+0Ch] = GetStreamPlayTimeMs (no GetStreamLengthMs, +8), solo con decoder
    if (m_pStreamingDecoder) {
        m_nStreamPlayTimeMs = m_pStreamingDecoder->GetStreamPlayTimeMs();
    }
    return filled;
}

// 0x4F23D0
int32 CAEStreamingChannel::PrepareStream(CAEStreamingDecoder* newDecoder, int8 soundFlags, bool stopCurrent) {
    if (!newDecoder || !m_pDirectSoundBuffer)
        return -1;

    if (stopCurrent) {
        switch (m_nState) {
        case StreamingChannelState::UNK_MINUS_3:
        case StreamingChannelState::Finished:
        case StreamingChannelState::Started:
            DirectSoundBufferFadeToSilence();
            break;
        default:
            break;
        }

        m_nState = StreamingChannelState::Stopped;
    }

    if (m_pStreamingDecoder)
        delete m_pStreamingDecoder;

    m_bLoopTrack = (soundFlags & 1) != 0;
    m_pStreamingDecoder = newDecoder;
    void* audioBuffer;
    DWORD audioBytes;
    if (SUCCEEDED(m_pDirectSoundBuffer->Lock(
        0,
        0,
        &audioBuffer,
        &audioBytes,
        nullptr,
        0,
        DSBLOCK_ENTIREBUFFER
    ))) {
        const auto written = FillBuffer(audioBuffer, audioBytes);
        if (written == audioBytes) {
            m_bNeedToFinish = 0;
        } else {
            memset(reinterpret_cast<uint8*>(audioBuffer) + written, 0, audioBytes - written);
            m_bNeedToFinish = 1;
        }

        // ?
        if (audioBytes & 0xFFFFFFFC) {
            for (auto i = 0u; i < audioBytes >> 2; i++) {
                static_cast<uint32*>(audioBuffer)[i] |= 0x10001u;
            }
        }

        m_pDirectSoundBuffer->Unlock(audioBuffer, audioBytes, nullptr, 0);
    }

    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_3:
    case StreamingChannelState::Started:
    case StreamingChannelState::Paused:
        break;
    default:
        m_nState = StreamingChannelState::UNK_MINUS_2;
        break;
    }

    m_lastSlot = 0;
    SetOriginalFrequency(m_pStreamingDecoder->GetSampleRate());
    m_bSilenced = false;
    m_bNeedSwitch = false;
    m_bShouldStop = false;
    m_pDirectSoundBuffer->SetCurrentPosition(0);
    return 0;
}

// 0x4F1DE0
void CAEStreamingChannel::SetNextStream(CAEStreamingDecoder* decoder) {
    if (m_pNextStreamingDecoder) {
        delete m_pNextStreamingDecoder;
    }
    m_pNextStreamingDecoder = decoder;
}

// 0x4F1A60
int32 CAEStreamingChannel::GetPlayingTrackID() {
    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_3:
    case StreamingChannelState::Finished:
    case StreamingChannelState::Started:
    case StreamingChannelState::Stopping:
        if (m_pStreamingDecoder)
            return m_pStreamingDecoder->GetStreamID();

        [[fallthrough]];
    default:
        return -1;
    }
}

// 0x4F1A40
int32 CAEStreamingChannel::GetActiveTrackID() {
    return m_pStreamingDecoder ? m_pStreamingDecoder->GetStreamID() : -1;
}

// 0x4F18A0
int32 CAEStreamingChannel::UpdatePlayTime() {
    if (m_nState != StreamingChannelState::Started && m_nState != StreamingChannelState::UNK_MINUS_3) {
        m_nPlayTime = static_cast<int32>(m_nState);
        return m_nPlayTime;
    }

    if (CAEAudioUtility::GetCurrentTimeInMS() - m_nLastUpdateTime < 1'000) {
        return GetPlayTime(); // call [vtable+0Ch]
    }

    DWORD currentPlayCursor;
    m_pDirectSoundBuffer->GetCurrentPosition(&currentPlayCursor, nullptr);

    // Como el original (0x4F1901..0x4F19B3), en float de 24 bits y en este orden:
    //  - fild con correccion +2^32 si el dword es negativo
    //  - freqFactor = frecuencia * [859CE0] (0.004), no frecuencia / 250
    //  - la mitad del buffer sale de cursor / 0x60000 (mul 0AAAAAAABh / shr 12h), no de freqFactor
    const auto ToFloat = [](uint32 v) {
        auto f = static_cast<float>(static_cast<int32>(v));
        if (static_cast<int32>(v) < 0) {
            f = f + StaticRef<float>(0x858C54);
        }
        return f;
    };
    const float streamMs   = static_cast<float>(static_cast<int32>(m_nStreamPlayTimeMs)); // fild [esi+60078h]
    const float freqFactor = ToFloat(m_nFrequency) * StaticRef<float>(0x859CE0);
    const float upperLimit = StaticRef<float>(0x85F414) / freqFactor;
    const float curStep    = ToFloat(currentPlayCursor) / freqFactor;
    const uint8 half       = static_cast<uint8>((static_cast<uint64>(currentPlayCursor) * 0xAAAAAAABull >> 32) >> 18);

    float t;
    if (half != 0) {
        if (m_lastWrittenSlot == 1) {
            t = (curStep - upperLimit) + (streamMs - upperLimit);
        } else {
            t = (curStep - upperLimit) + streamMs;
        }
    } else if (m_lastWrittenSlot != 0) {
        t = curStep + streamMs;
    } else {
        t = (streamMs - upperLimit) + curStep;
    }
    m_nPlayTime = static_cast<int32>(t); // _ftol2: truncacion

    const float asFloat = static_cast<float>(m_nPlayTime); // fild [esi+6007Ch] / fcom [esp+8] / test ah,1
    if (asFloat >= upperLimit) {
        m_nPlayTime = static_cast<int32>(asFloat - upperLimit); // fsub en float, luego truncacion
    }

    m_nPlayTime       = m_nPlayTime <= 0 ? 0 : m_nPlayTime; // setle / dec / and
    m_nLastUpdateTime = CAEAudioUtility::GetCurrentTimeInMS();
    return m_nPlayTime;
}

// 0x4F1AE0
bool CAEStreamingChannel::AddFX() {
    if (m_bEQEnabled)
        return true;

    DSEFFECTDESC effectDesc[2]{};
    for (auto& desc : effectDesc) {
        desc.guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
        desc.dwFlags = 0;
        desc.dwReserved1 = desc.dwReserved2 = 0;
        desc.dwSize = 32;
    }

    bool wasStopped = false;
    if (bufferStatus.Bit0x1) {
        m_pDirectSoundBuffer->Stop();
        wasStopped = true;
    }

    DWORD resultCodes[2]; // Como el original: SetFX recibe un array local sin inicializar (0x4F1B6D)
    if (FAILED(m_pDirectSoundBuffer8->SetFX(
        std::size(effectDesc),
        effectDesc,
        resultCodes
    )))
        return false;

    IDirectSoundFXParamEq* fxParamEq; // Como el original: un unico local sin inicializar, compartido por ambas iteraciones (si GetObjectInPath falla conserva el valor anterior)
    for (auto i = 0; i < 2; i++) { // Set both channels
        m_pDirectSoundBuffer8->GetObjectInPath(
            GUID_All_Objects,
            i,
            IID_IDirectSoundFXParamEq,
            reinterpret_cast<LPVOID*>(&fxParamEq)
        );
        fxParamEq->SetAllParameters(&s_FXParamEqPresets[0][i]);
        fxParamEq->Release();
    }

    if (wasStopped)
        m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

    m_bEQEnabled = true;
    return true;
}

// 0x4F1C20
void CAEStreamingChannel::RemoveFX() {
    bool stopped{false};
    if (bufferStatus.Bit0x1) {
        m_pDirectSoundBuffer->Stop();
        stopped = true;
    }

    m_pDirectSoundBuffer8->SetFX(0, nullptr, nullptr);

    if (stopped)
        m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

    m_bEQEnabled = false;
}

// 0x4F2040
bool CAEStreamingChannel::IsSoundPlaying() {
    switch (m_nState) {
    case StreamingChannelState::UNK_MINUS_3:
    case StreamingChannelState::Finished:
    case StreamingChannelState::Stopping:
    case StreamingChannelState::Started:
        return true;
    default:
        return false;
    }
}

// 0x4F19E0
// Como el original: devuelve los 32 bits completos (tiempo de reproducción o el estado), sin truncar a 16 bits.
int32 CAEStreamingChannel::GetPlayTime() {
    switch (m_nState) {
    case StreamingChannelState::Started:
    case StreamingChannelState::UNK_MINUS_3:
        if (CAEAudioUtility::GetCurrentTimeInMS() - m_nLastUpdateTime >= 1000u)
            return UpdatePlayTime();
        else // Como el original: parte baja del tiempo actual + (m_nPlayTime - parte baja de m_nLastUpdateTime), módulo 2^32
            return static_cast<int32>(static_cast<uint32>(m_nPlayTime) - static_cast<uint32>(m_nLastUpdateTime) + static_cast<uint32>(CAEAudioUtility::GetCurrentTimeInMS()));
    default:
        break;
    }

    return static_cast<int32>(m_nState); // possibly not intended.
}

// 0x4F1880
// Como el original: devuelve el long de GetStreamLengthMs sin truncar (o -1 sin decoder).
int32 CAEStreamingChannel::GetLength() {
    return m_pStreamingDecoder ? static_cast<int32>(m_pStreamingDecoder->GetStreamLengthMs()) : -1;
}

// 0x4F1D40
void CAEStreamingChannel::Play(int16 startOffsetMs, int8 soundFlags, float freqFac) {
    if (!m_pStreamingDecoder || !m_pDirectSoundBuffer)
        return;

    SetOriginalFrequency(m_pStreamingDecoder->GetSampleRate());

    if (m_nState != StreamingChannelState::Paused) {
        m_bLoopTrack = (soundFlags & 1) != 0;
    }

    if (m_nState == StreamingChannelState::Stopping) {
        AESmoothFadeThread.CancelFade(m_pDirectSoundBuffer);
        m_pDirectSoundBuffer->Stop();

        m_nState = StreamingChannelState::Stopped;
    }

    m_nState = StreamingChannelState::Started;
    m_pDirectSoundBuffer->SetVolume(static_cast<int32>(m_Volume * 100.0f));
    m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

// 0x4F2170
void CAEStreamingChannel::Pause() {
    if (!m_pDirectSoundBuffer || !IsBufferPlaying()) // Como el original: call 4D7990 / test al,al / je (0x4F217A)
        return;

    DirectSoundBufferFadeToSilence();

    m_nState = StreamingChannelState::Paused;
}

// 0x4F1A90
void CAEStreamingChannel::Stop(bool bUpdateState) {
    DirectSoundBufferFadeToSilence();

    if (bUpdateState) {
        m_nState = StreamingChannelState::Stopping;
        m_lStoppingFrameCount = 0;
    }
}

// unused
// 0x4F21C0
void CAEStreamingChannel::Stop() {
    DirectSoundBufferFadeToSilence();
    m_nState = StreamingChannelState::Stopping;
    m_lStoppingFrameCount = 0;
}

// 0x4F2550
void CAEStreamingChannel::Service() {
    UpdateStatus();
    if (m_nBufferStatus & 1) {
        if (m_nState == StreamingChannelState::Started) m_nState = StreamingChannelState::UNK_MINUS_3;
        else if (m_nState == StreamingChannelState::Stopping) {
            ++m_lStoppingFrameCount;
            if (m_lStoppingFrameCount > 6) m_pDirectSoundBuffer->Stop();
        } else if (m_nState != StreamingChannelState::UNK_MINUS_3 && m_nState != StreamingChannelState::Finished) {
            m_lStoppingFrameCount = 0;
            if (m_nState == StreamingChannelState::Stopped) m_pDirectSoundBuffer->Stop();
        }
    } else if (m_nState == StreamingChannelState::Stopping) m_nState = StreamingChannelState::Stopped;
    if (m_nState == StreamingChannelState::UNK_MINUS_3 && m_nBufferStatus == 0) m_nState = StreamingChannelState::Paused;
    if (m_nState != StreamingChannelState::UNK_MINUS_3 && m_nState != StreamingChannelState::Finished && m_nState != StreamingChannelState::Paused) return;
    if (m_pStreamingDecoder->GetSampleRate() != m_nOriginalFrequency) SetOriginalFrequency(m_pStreamingDecoder->GetSampleRate());
    DWORD cursor;
    m_pDirectSoundBuffer->GetCurrentPosition(&cursor, nullptr);
    const uint8 slot = static_cast<uint8>(cursor / 0x60000u);
    if (slot == m_lastSlot) return;
    uint32 written = 0;
    if (m_nState == StreamingChannelState::Finished) {
        if (m_bShouldStop) {
            m_pDirectSoundBuffer->Stop();
            m_nState = StreamingChannelState::Stopped;
            m_bShouldStop = false;
            return;
        }
        m_bShouldStop = true;
    } else if (m_bSilenced) {
        m_bSilenced = false;
        m_bNeedSwitch = true;
    } else if (m_bNeedSwitch) {
        auto* next = m_pNextStreamingDecoder;
        m_pNextStreamingDecoder = nullptr;
        m_nState = StreamingChannelState::Paused;
        Stop(false);
        PrepareStream(next, 0, false);
        static_cast<CAEAudioChannel*>(this)->Play(0, 0, 1.f);
        return;
    } else if (!m_bNeedToFinish) {
        written = FillBuffer(m_pBuffer, 0x60000);
        m_lastWrittenSlot = slot;
    } else {
        m_nState = StreamingChannelState::Finished;
        return;
    }
    void* dest;
    DWORD bytes;
    if (SUCCEEDED(m_pDirectSoundBuffer->Lock(uint32(m_lastSlot) * 0x60000u, 0x60000, &dest, &bytes, nullptr, nullptr, 0))) {
        if (written) memcpy(dest, m_pBuffer, std::min(written, uint32(bytes)));
        memset(static_cast<uint8*>(dest) + written, 0, 0x60000u - written);
        for (uint32 i = 0; i < bytes / 4u; ++i) static_cast<uint32*>(dest)[i] |= 0x10001u;
        m_pDirectSoundBuffer->Unlock(dest, bytes, nullptr, 0);
    }
    m_lastSlot = slot;
}

void CAEStreamingChannel::InjectHooks() {
    RH_ScopedVirtualClass(CAEStreamingChannel, 0x85F3F0, 9);
    RH_ScopedCategory("Audio/Hardware");

    RH_ScopedInstall(Constructor, 0x4F1800);
    RH_ScopedInstall(Destructor, 0x4F2200);
    RH_ScopedVMTInstall(SynchPlayback, 0x4F1870);
    RH_ScopedInstall(PrepareStream, 0x4F23D0);
    RH_ScopedInstall(Initialise, 0x4F22F0);
    RH_ScopedInstall(Pause, 0x4F2170);
    RH_ScopedInstall(SetReady, 0x4F1FF0);
    RH_ScopedInstall(SetBassEQ, 0x4F1F30);
    RH_ScopedInstall(FillBuffer, 0x4F1E20);
    RH_ScopedInstall(InitialiseSilence, 0x4F1C70);
    RH_ScopedInstall(SetNextStream, 0x4F1DE0);
    RH_ScopedInstall(AddFX, 0x4F1AE0);
    RH_ScopedOverloadedInstall(Stop, "class", 0x4F1A90, void(CAEStreamingChannel::*)(bool));
    RH_ScopedInstall(GetPlayingTrackID, 0x4F1A60);
    RH_ScopedInstall(GetActiveTrackID, 0x4F1A40);
    RH_ScopedInstall(UpdatePlayTime, 0x4F18A0);
    RH_ScopedInstall(RemoveFX, 0x4F1C20);
    RH_ScopedVMTInstall(Service, 0x4F2550);
    RH_ScopedVMTInstall(IsSoundPlaying, 0x4F2040);
    RH_ScopedVMTInstall(GetPlayTime, 0x4F19E0);
    RH_ScopedVMTInstall(GetLength, 0x4F1880);
    RH_ScopedVMTInstall(Play, 0x4F1D40);
    RH_ScopedVMTOverloadedInstall(Stop, "virtual", 0x4F21C0, void(CAEStreamingChannel::*)());
    RH_ScopedVMTInstall(SetFrequencyScalingFactor, 0x4F2060);
}
