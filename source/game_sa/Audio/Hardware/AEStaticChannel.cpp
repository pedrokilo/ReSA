/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEStaticChannel
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEStaticChannel.h"

#include "AESmoothFadeThread.h"

void CAEStaticChannel::InjectHooks() {
    RH_ScopedVirtualClass(CAEStaticChannel, 0x85F3CC, 9);
    RH_ScopedCategory("Audio/Hardware");

    RH_ScopedVMTInstall(Service, 0x4F10D0);
    RH_ScopedVMTInstall(IsSoundPlaying, 0x4F0F40);
    RH_ScopedVMTInstall(GetPlayTime, 0x4F0F70);
    RH_ScopedVMTInstall(GetLength, 0x4F0FA0);
    RH_ScopedVMTInstall(Play, 0x4F0BD0);
    RH_ScopedVMTInstall(SynchPlayback, 0x4F1040);
    RH_ScopedVMTInstall(Stop, 0x4F0FB0);

    RH_ScopedInstall(SetAudioBuffer, 0x4F0C40);
}

CAEStaticChannel::CAEStaticChannel(IDirectSound* pDirectSound, uint16 channelId, bool hardwareMixAvailable, uint32 samplesPerSec, uint16 bitsPerSample) :
    CAEAudioChannel(pDirectSound, channelId, samplesPerSec, bitsPerSample),
    m_bNeedData(false),
    m_bNeedsSynch(false),
    m_IsHardwareMixAvailable(hardwareMixAvailable)
{
}

// 0x4F10D0
void CAEStaticChannel::Service() {
    if (!m_pDirectSoundBuffer) {
        m_nBufferStatus = 0;
        return;
    }

    // Como el original: comparación sin signo (jbe) del tiempo transcurrido.
    if (m_bNeedData && CTimer::GetTimeInMS() - m_nSyncTime > uint32(field_74)) {
        uint8* ppvAudioPtr1{};
        DWORD pdwAudioBytes{};

        VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Lock(
            m_dwLockOffset,
            m_nNumLockBytes,
            reinterpret_cast<LPVOID*>(&ppvAudioPtr1),
            &pdwAudioBytes,
            nullptr,
            0,
            0
        )));

        for (auto i = 0u; i < m_nNumLoops; i++) {
            memcpy(
                &ppvAudioPtr1[i * m_nNumLockBytes],
                (uint8*)m_pBuffer + m_nCurrentBufferOffset,
                m_nNumLockBytes
            );
        }
        VERIFY(SUCCEEDED(m_pDirectSoundBuffer->Unlock(ppvAudioPtr1, pdwAudioBytes, nullptr, 0)));
        m_bNeedData = false;
    }

    UpdateStatus();

    if (!m_bPaused && !bufferStatus.Bit0x1) {
        // Como el original: se libera antes de anular el puntero.
        if (m_pDirectSoundBuffer) {
            --g_numSoundChannelsUsed;
            m_pDirectSoundBuffer->Release();
        }
        m_pDirectSoundBuffer = nullptr;
    }
}

// 0x4F0F40
bool CAEStaticChannel::IsSoundPlaying() {
    if (!m_pDirectSoundBuffer)
        return false;

    if (m_bPaused || m_bNeedsSynch)
        return true;

    return CAEAudioChannel::IsBufferPlaying();
}

// 0x4F0F70
int32 CAEStaticChannel::GetPlayTime() {
    if (!IsSoundPlaying())
        return -1;

    const auto curPos = CAEAudioChannel::GetCurrentPlaybackPosition();
    return static_cast<int16>(CAEAudioChannel::ConvertFromBytesToMS(curPos)); // Como el original: movsx eax,ax
}

// 0x4F0FA0
int32 CAEStaticChannel::GetLength() {
    return static_cast<int16>(CAEAudioChannel::ConvertFromBytesToMS(m_nLengthInBytes)); // Como el original: movsx eax,ax
}

// 0x4F0BD0
void CAEStaticChannel::Play(int16 timeInMs, int8 unused, float scalingFactor) {
    if (m_bLooped && m_nCurrentBufferOffset != 0 || !timeInMs) {
        m_bUnkn2 = false;
    } else {
        m_pDirectSoundBuffer->SetCurrentPosition(ConvertFromMsToBytes(timeInMs));
        m_bUnkn2 = true;
    }
    m_bNeedsSynch = true;
    m_bPaused = scalingFactor == 0.0f;
}

    

// 0x4F1040
void CAEStaticChannel::SynchPlayback() {
    if (!m_pDirectSoundBuffer || !m_bNeedsSynch || m_bPaused)
        return;

    if (m_bUnkn2) {
        m_pDirectSoundBuffer->SetVolume(-10000);
        if (!AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_Volume, -2, false)) {
            const auto dwVolume = static_cast<LONG>(m_Volume * 100.0F);
            m_pDirectSoundBuffer->SetVolume(dwVolume);
        }
    }

    m_pDirectSoundBuffer->Play(0, 0, m_bLooped);
    m_nSyncTime = CTimer::GetTimeInMS();
    m_bNeedsSynch = false;
}

// 0x4F0FB0
void CAEStaticChannel::Stop() {
    if (m_pDirectSoundBuffer &&
        CAEAudioChannel::IsBufferPlaying() &&
        !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, -100.0F, -1, true)
    ) {
        m_pDirectSoundBuffer->Stop();
    }

    { // todo: Same as CAEAudioChannel::~CAEAudioChannel
    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        m_pDirectSoundBuffer->Release();
        m_pDirectSoundBuffer = nullptr;
    }

    if (m_pDirectSound3DBuffer) {
        m_pDirectSound3DBuffer->Release();
        m_pDirectSound3DBuffer = nullptr;
    }
    }
    m_bPaused = false; // Como el original: 0x4F100F
}

// 0x4F0C40
bool CAEStaticChannel::SetAudioBuffer(const void* buffer, uint32 size, int16 f88, int16 f8c, int16 loopOffset, uint16 frequency) {
    if (!size || !frequency) {
        return false;
    }
    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        m_pDirectSoundBuffer->Release();
    }
    m_pDirectSoundBuffer = nullptr;
    if (m_pDirectSound3DBuffer) {
        m_pDirectSound3DBuffer->Release();
    }
    m_pDirectSound3DBuffer = nullptr;
    m_pBuffer = buffer;
    m_nCurrentBufferOffset = 0;
    field_68 = 0;
    m_bLooped = false;
    m_bNeedData = false;
    m_bPaused = false;
    field_6C = 0;
    m_nLengthInBytes = size;
    field_88 = f88;
    field_8C = f8c;
    if (loopOffset != -1) {
        m_bLooped = true;
        m_nCurrentBufferOffset = int32(loopOffset) * 2;
        field_68 = size;
    }
    if (m_bLooped && m_nCurrentBufferOffset && uint32(field_68) != uint32(m_nCurrentBufferOffset)) {
        m_nNumLockBytes = uint32(field_68) - uint32(m_nCurrentBufferOffset);
        field_6C = std::max(uint32(field_68), 24000u) / uint32(m_nNumLockBytes) + 1;
        m_nBufferSize = uint32(field_6C) * uint32(m_nNumLockBytes);
    } else {
        m_nBufferSize = size;
    }
    m_WaveFormat = {WAVE_FORMAT_PCM, 1, frequency, uint32(frequency) * 2, 2, 16, 0};
    m_nFrequency = frequency;
    m_nOriginalFrequency = frequency;
    DSBUFFERDESC desc{};
    desc.dwSize = sizeof(desc);
    // 0x4F0D56: lea ecx,[ecx*4+80B4h] -> GLOBALFOCUS (0x8000), no GETCURRENTPOSITION2.
    desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS
        | (m_IsHardwareMixAvailable ? DSBCAPS_LOCHARDWARE : DSBCAPS_LOCSOFTWARE);
    desc.dwBufferBytes = m_nBufferSize;
    desc.lpwfxFormat = &m_WaveFormat;
    if (FAILED(m_pDirectSound->CreateSoundBuffer(&desc, &m_pDirectSoundBuffer, nullptr))) {
        return false;
    }
    ++g_numSoundChannelsUsed;
    void* audio{};
    DWORD audioBytes{};
    if (FAILED(m_pDirectSoundBuffer->Lock(0, m_nLengthInBytes, &audio, &audioBytes, nullptr, nullptr, DSBLOCK_ENTIREBUFFER))) {
        if (m_pDirectSoundBuffer) {
            m_pDirectSoundBuffer->Release();
        }
        m_pDirectSoundBuffer = nullptr;
        // The original does not decrement the global channel count on this failure.
        return false;
    }
    uint32 initialPosition{};
    const auto* source = static_cast<const uint8*>(m_pBuffer);
    auto* destination = static_cast<uint8*>(audio);
    if (!m_nCurrentBufferOffset) {
        std::memcpy(destination, source, size);
        if (size < m_nLengthInBytes) {
            std::memset(destination + size, 0, m_nLengthInBytes - size);
        }
        m_bNeedData = false;
    } else {
        initialPosition = m_nBufferSize - uint32(m_nCurrentBufferOffset);
        std::memcpy(destination + initialPosition, source, m_nCurrentBufferOffset);
        m_nNumLoops = uint32(m_nCurrentBufferOffset) / uint32(m_nNumLockBytes) + 1;
        m_dwLockOffset = m_nBufferSize - uint32(m_nNumLoops) * uint32(m_nNumLockBytes);
        for (uint16 i = 0; i < uint32(field_6C) - m_nNumLoops; ++i) {
            std::memcpy(destination + uint32(i) * uint32(m_nNumLockBytes), source + m_nCurrentBufferOffset, m_nNumLockBytes);
        }
        field_74 = int16(ConvertFromBytesToMS(m_nCurrentBufferOffset));
        m_bNeedData = true;
    }
    m_pDirectSoundBuffer->Unlock(audio, audioBytes, nullptr, 0);
    m_pDirectSoundBuffer->SetCurrentPosition(initialPosition);
    m_pDirectSoundBuffer->QueryInterface(IID_IDirectSound3DBuffer, reinterpret_cast<void**>(&m_pDirectSound3DBuffer));
    m_Volume = -100.f;
    m_pDirectSoundBuffer->SetVolume(-10000);
    return true;
}
