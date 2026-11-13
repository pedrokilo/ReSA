/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEAudioChannel
 * Reconstrucción: COMPLETA — 16/16 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 16/16 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEAudioChannel.h"

#include "AEAudioUtility.h"
#include "AESmoothFadeThread.h"
#include <AEAudioHardware.h>

auto& g_numSoundChannelsUsed = StaticRef<uint32>(0xB5F898);

#if 0 // No EAX
static std::array<uint32, 48> s_EAXEnvironments = {
    EAX_ENVIRONMENT_GENERIC, EAX_ENVIRONMENT_PADDEDCELL, EAX_ENVIRONMENT_ROOM,
    EAX_ENVIRONMENT_BATHROOM, EAX_ENVIRONMENT_LIVINGROOM, EAX_ENVIRONMENT_STONEROOM,
    EAX_ENVIRONMENT_AUDITORIUM, EAX_ENVIRONMENT_CONCERTHALL, EAX_ENVIRONMENT_CAVE,
    EAX_ENVIRONMENT_ARENA, EAX_ENVIRONMENT_HANGAR, EAX_ENVIRONMENT_CARPETEDHALLWAY,
    EAX_ENVIRONMENT_HALLWAY, EAX_ENVIRONMENT_STONECORRIDOR, EAX_ENVIRONMENT_ALLEY,
    EAX_ENVIRONMENT_FOREST, EAX_ENVIRONMENT_CITY, EAX_ENVIRONMENT_MOUNTAINS,
    EAX_ENVIRONMENT_QUARRY, EAX_ENVIRONMENT_PLAIN, EAX_ENVIRONMENT_PARKINGLOT,
    EAX_ENVIRONMENT_SEWERPIPE, EAX_ENVIRONMENT_UNDERWATER, EAX_ENVIRONMENT_ROOM,
    EAX_ENVIRONMENT_PADDEDCELL, EAX_ENVIRONMENT_ROOM, EAX_ENVIRONMENT_BATHROOM,
    EAX_ENVIRONMENT_LIVINGROOM, EAX_ENVIRONMENT_STONEROOM, EAX_ENVIRONMENT_AUDITORIUM,
    EAX_ENVIRONMENT_CONCERTHALL, EAX_ENVIRONMENT_CAVE, EAX_ENVIRONMENT_ARENA,
    EAX_ENVIRONMENT_HANGAR, EAX_ENVIRONMENT_CARPETEDHALLWAY, EAX_ENVIRONMENT_HALLWAY,
    EAX_ENVIRONMENT_STONECORRIDOR, EAX_ENVIRONMENT_ALLEY, EAX_ENVIRONMENT_FOREST,
    EAX_ENVIRONMENT_CITY, EAX_ENVIRONMENT_MOUNTAINS, EAX_ENVIRONMENT_QUARRY,
    EAX_ENVIRONMENT_PLAIN, EAX_ENVIRONMENT_PARKINGLOT, EAX_ENVIRONMENT_SEWERPIPE,
    EAX_ENVIRONMENT_UNDERWATER, EAX_ENVIRONMENT_DRUGGED, EAX_ENVIRONMENT_BATHROOM
};
#endif

namespace {
// 0x4D7C00 (lote 149A, banco Fn149A): destructor con borrado de CAEAudioChannel del EXE (ranura 0 de su vtable 0x85F03C; thiscall, un argumento de pila, `ret 4`, 81 bytes). El cuerpo del destructor va EN LINEA y
// en este orden (que un Release de COM puede observar a traves de `this`): vtable de la clase (0x85F03C) al principio; si hay buffer DirectSound (+8) decrementa el contador de canales usados (0xB5F898); relee +8 y,
// si no es nulo, lo libera con Release (ranura 2 de su vtable, stdcall); pone +8 a 0; LEE +0xC DESPUES de ese primer Release y, si no es nulo, lo libera tambien; pone +0xC a 0; con el bit 0 del BYTE de banderas
// llama a operator delete del CRT (0x8214BD) con `this`; devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CAEAudioChannel* __fastcall AEAudioChannelScalarDeletingDestructor(CAEAudioChannel* self, void* /*edx*/, uint8 flags) {
    *reinterpret_cast<uint32*>(self) = 0x85F03C;
    if (self->m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
    }
    if (self->m_pDirectSoundBuffer) {
        self->m_pDirectSoundBuffer->Release();
    }
    IDirectSound3DBuffer* const buffer3D = self->m_pDirectSound3DBuffer; // Como el original: se lee DESPUES del primer Release
    self->m_pDirectSoundBuffer = nullptr;
    if (buffer3D) {
        buffer3D->Release();
    }
    self->m_pDirectSound3DBuffer = nullptr;
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}
} // namespace

/**
 * Registra las operaciones comunes de un canal DirectSound.
 */
void CAEAudioChannel::InjectHooks() {
    RH_ScopedVirtualClass(CAEAudioChannel, 0x85F03C, 9);
    RH_ScopedCategory("Audio/Hardware");
    RH_ScopedNamedGlobalInstall(AEAudioChannelScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4D7C00); // lote 149A (banco Fn149A)
    RH_ScopedInstall(IsBufferPlaying, 0x4D7990);                                                               // lote 149A (banco Fn149A)
    RH_ScopedInstall(Constructor, 0x4D7890);                                                                   // lote 180A (sin validar)
    RH_ScopedInstall(DestructorBody, 0x4D7910);                                                                // lote 180A (sin validar)

    RH_ScopedInstall(SetPosition, 0x4D7950);
    RH_ScopedInstall(UpdateStatus, 0x4D7BD0);
    RH_ScopedInstall(Lost, 0x4D7A10);
    RH_ScopedInstall(ConvertFromBytesToMS, 0x4D79D0);
    RH_ScopedInstall(ConvertFromMsToBytes, 0x4D79F0);
    RH_ScopedInstall(SetFrequency, 0x4D7A50);
    RH_ScopedInstall(SetVolume, 0x4D7C60);
    RH_ScopedInstall(SetOriginalFrequency, 0x4D7A70);
    RH_ScopedVMTInstall(SetFrequencyScalingFactor, 0x4D7D00);
    RH_ScopedInstall(GetCurrentPlaybackPosition, 0x4D79A0);
    RH_ScopedInstall(SetReverbAndDepth, 0x4D7AA0);
    RH_ScopedInstall(SetNotInRoom, 0x4D7B50);
}

// 0x4D7890
// Inicializa formato PCM y estado lógico del canal sin crear todavía buffers DirectSound.
CAEAudioChannel::CAEAudioChannel(IDirectSound* directSound, uint16 channelId, uint32 samplesPerSec, uint16 bitsPerSample) {
    m_pDirectSound               = directSound;
    m_nChannelId                 = channelId;
    m_nFlags                     = 0;
    m_nBufferStatus              = 0;
    m_nFrequency                 = samplesPerSec;
    m_nOriginalFrequency         = samplesPerSec;
    m_Volume                    = -100.0f;
    m_pDirectSoundBuffer         = nullptr;
    m_pDirectSound3DBuffer       = nullptr;
    m_bPaused                    = false;
    m_bLooped                    = false;
    m_bShouldStop                = false;
    m_WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;
    m_WaveFormat.nChannels       = 1;
    m_WaveFormat.nSamplesPerSec  = samplesPerSec;
    m_WaveFormat.nAvgBytesPerSec = samplesPerSec * (bitsPerSample / 8);
    m_WaveFormat.nBlockAlign     = 2;
    m_WaveFormat.wBitsPerSample  = bitsPerSample;
    m_WaveFormat.cbSize          = 0;
}

// 0x4D7890 (lote 180A, sin validar; thiscall, cuatro dwords de pila, `ret 0x10`, 115 bytes): constructor de la base del EXE. Como el original: el constructor de C++ (mismos campos de formato PCM, estado logico, +0x28 y +0x5C a cero,
// volumen -100.0 = 0xC2C80000, buffers DirectSound nulos) y despues la vtable de CAEAudioChannel del EXE (0x85F03C) en lugar de la de la DLL; no escribe +0x46 (m_bShouldPlay) ni la zona de relleno. nAvgBytesPerSec = frecuencia *
// (bits >> 3) en 32 bits. Devuelve `this` (lo llaman los constructores de los canales del EXE, por ejemplo 0x4F0B10).
CAEAudioChannel* CAEAudioChannel::Constructor(IDirectSound* directSound, uint16 channelId, uint32 samplesPerSec, uint16 bitsPerSample) {
    this->CAEAudioChannel::CAEAudioChannel(directSound, channelId, samplesPerSec, bitsPerSample);
    *reinterpret_cast<uint32*>(this) = 0x85F03C;
    return this;
}

// 0x4D7910 (lote 180A, sin validar; thiscall, sin argumentos de pila, `ret`, 64 bytes): cuerpo del destructor de la base del EXE (los cuerpos de los canales derivados terminan llamandolo). Como el original, con lecturas frescas de la
// memoria: vtable de CAEAudioChannel del EXE (0x85F03C) al principio; si hay buffer DirectSound (+8) decrementa el contador de canales usados (0xB5F898); relee +8 y, si no es nulo, lo libera (Release, ranura 2, stdcall); lee +0xC
// DESPUES de ese Release, pone +8 a cero y libera el buffer 3D si no es nulo; pone +0xC a cero. Es la misma secuencia que AEAudioChannelScalarDeletingDestructor (0x4D7C00) y ChannelReleaseSequence (AEStaticChannel.cpp).
void CAEAudioChannel::DestructorBody() {
    volatile uint32* const vt       = reinterpret_cast<volatile uint32*>(this);
    volatile uint32* const buffer   = reinterpret_cast<volatile uint32*>(reinterpret_cast<uint8*>(this) + 8);
    volatile uint32* const buffer3D = reinterpret_cast<volatile uint32*>(reinterpret_cast<uint8*>(this) + 0xC);
    *vt = 0x85F03C;
    if (*buffer != 0) {
        --g_numSoundChannelsUsed;
    }
    if (const uint32 p = *buffer; p != 0) {
        reinterpret_cast<IUnknown*>(uintptr_t(p))->Release();
    }
    const uint32 q = *buffer3D;
    *buffer = 0;
    if (q != 0) {
        reinterpret_cast<IUnknown*>(uintptr_t(q))->Release();
    }
    *buffer3D = 0;
}

// 0x4D7910
CAEAudioChannel::~CAEAudioChannel() {
    if (m_pDirectSoundBuffer) {
        --g_numSoundChannelsUsed;
        std::exchange(m_pDirectSoundBuffer, nullptr)->Release();
    }

    if (m_pDirectSound3DBuffer) {
        std::exchange(m_pDirectSound3DBuffer, nullptr)->Release();
    }
}

// 0x4D7D00
/**
 * Aplica pitch relativo a la frecuencia original.
 *
 * Un factor 0 pausa el canal mediante fade/Stop; un factor posterior distinto de
 * cero restaura reproducción y volumen conservando la posición actual.
 */
void CAEAudioChannel::SetFrequencyScalingFactor(float factor) {
    if (factor == 0.0F) {
        if (m_pDirectSoundBuffer &&
            !m_bPaused &&
            IsBufferPlaying() &&
            !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, -100.0F, -1, true)
        ) {
            m_pDirectSoundBuffer->Stop();
        }

        m_bPaused = true;
        return;
    }

    const auto newFreq = static_cast<uint16>(static_cast<int64>(float(m_nOriginalFrequency) * factor)); // 0x4D7D73: truncated to 16 bits
    SetFrequency(newFreq);

    if (m_bPaused) {
        if (m_pDirectSoundBuffer) {
            const auto curPos = GetCurrentPlaybackPosition();
            if (curPos != 0) {
                m_pDirectSoundBuffer->SetVolume(-10000);
            }

            m_pDirectSoundBuffer->Play(0, 0, m_bLooped);

            if (curPos != 0 && !AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, m_Volume, -2, false)) {
                const auto volume = static_cast<LONG>(m_Volume * 100.0F);
                m_pDirectSoundBuffer->SetVolume(volume);
            }
        }
        m_bPaused = false;
    }
}

// 0x4D7950
void CAEAudioChannel::SetPosition(const CVector& vecPos) const {
    if (!m_pDirectSoundBuffer)
        return;

    if (!m_pDirectSound3DBuffer)
        return;

#ifdef USE_DSOUND
    m_pDirectSound3DBuffer->SetPosition(vecPos.x, vecPos.y, vecPos.z, DS3D_DEFERRED);
#endif
}

// 0x4D7C60
// Usa smooth-fade para cambios grandes durante reproducción y escritura directa en el resto.
void CAEAudioChannel::SetVolume(float volume) {
    if (!m_pDirectSoundBuffer)
        return;

    if (IsBufferPlaying() && (float)std::abs((int32)(volume - m_Volume)) > 60.0F) { // 0x4D7C7E: difference truncated to int first
        if (volume <= m_Volume) {
            if (AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, volume, -1, false)) {
                m_Volume = volume;
                return;
            }
        } else if (AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, volume, -2, false)) {
            m_Volume = volume;
            return;
        }
    }

    AESmoothFadeThread.SetBufferVolume(m_pDirectSoundBuffer, volume);
    m_Volume = volume;
}

// 0x4D7990 (lote 149A, banco Fn149A; thiscall, `ret`, 7 bytes: `mov eax,[ecx+5Ch]; and eax,1`): bit 0 (DSBSTATUS_PLAYING) del estado del buffer (+0x5C).
bool CAEAudioChannel::IsBufferPlaying() const {
    return m_nBufferStatus & DSBSTATUS_PLAYING;
}

// 0x4D79A0
uint32 CAEAudioChannel::GetCurrentPlaybackPosition() const {
    if (!m_pDirectSoundBuffer)
        return 0;

    uint32 outPos = 0;
    m_pDirectSoundBuffer->GetCurrentPosition(reinterpret_cast<LPDWORD>(&outPos), nullptr);
    return outPos;
}

// 0x4D79D0
uint32 CAEAudioChannel::ConvertFromBytesToMS(uint32 bytes) const {
    return CAEAudioUtility::ConvertFromBytesToMS(bytes, m_WaveFormat.nSamplesPerSec, m_WaveFormat.nChannels);
}

// 0x4D79F0
uint32 CAEAudioChannel::ConvertFromMsToBytes(uint32 ms) const {
    return CAEAudioUtility::ConvertFromMSToBytes(ms, m_WaveFormat.nSamplesPerSec, m_WaveFormat.nChannels);
}

// 0x4D7A50
void CAEAudioChannel::SetFrequency(uint32 freq) {
    if (m_nFrequency == freq)
        return;

    m_nFrequency = freq;

    #ifdef USE_DSOUND
    if (m_pDirectSoundBuffer) {
        VERIFY_TODO_FIX(SUCCEEDED(m_pDirectSoundBuffer->SetFrequency(freq)));
    }
    #endif
}

// 0x4D7A70
void CAEAudioChannel::SetOriginalFrequency(uint32 freq) {
    SetFrequency(freq);
    m_nOriginalFrequency = freq;
}

// 0x4D7BD0
// Refresca flags del buffer y trata inmediatamente un buffer DirectSound perdido.
void CAEAudioChannel::UpdateStatus() {
#ifdef USE_DSOUND
    m_pDirectSoundBuffer->GetStatus(reinterpret_cast<LPDWORD>(&m_nBufferStatus));
    if (m_nBufferStatus & DSBSTATUS_BUFFERLOST) {
        Lost();
    }
#endif
}

// 0x4D7A10
bool CAEAudioChannel::Lost() const {
#ifdef USE_DSOUND
    // 0x4D7A20: two Restore() calls per iteration (BUG: infinite loop if it never restores)
    do {
        if (m_pDirectSoundBuffer->Restore() == DSERR_BUFFERLOST) {
            OS_ThreadSleep(10);
        }
    } while (m_pDirectSoundBuffer->Restore() == DSERR_BUFFERLOST);
#endif

    return true;
}

// GUID de propiedades EAX y tabla de entornos del EXE (la DLL no incluye las cabeceras EAX)
static inline auto& s_EAXListenerPropertySet = StaticRef<GUID>(0x85EFDC);
static inline auto& s_EAXBufferPropertySet   = StaticRef<GUID>(0x85EF9C);
static inline auto& s_EAXEnvironments        = StaticRef<std::array<int32, 48>>(0x8AC198);

// 0x4D7AA0
bool CAEAudioChannel::SetReverbAndDepth(uint32 reverbEnv, uint32 depth) {
    int32 room = 100 * depth;
    bool result = false;
    if (!m_pDirectSoundBuffer || !m_pDirectSound3DBuffer || AEAudioHardware.m_n3dEffectsQueryResult != 1) {
        return false;
    }

    IKsPropertySet* propertySet{};
    if (FAILED(m_pDirectSound3DBuffer->QueryInterface(IID_IKsPropertySet, (LPVOID*)&propertySet))) {
        return false;
    }
    // Entorno (propiedad 2) y sala (propiedad 5); el índice del entorno es int8 con signo como en el original.
    if (SUCCEEDED(propertySet->Set(s_EAXListenerPropertySet, 2, nullptr, 0, &s_EAXEnvironments.data()[(int8)reverbEnv], sizeof(int32)))
        && SUCCEEDED(propertySet->Set(s_EAXListenerPropertySet, 5, nullptr, 0, &room, sizeof(room)))) {
        result = true;
    }
    if (propertySet) {
        propertySet->Release();
    }
    return result;
}

// 0x4D7B50
void CAEAudioChannel::SetNotInRoom(bool onStreet) {
    if (!m_pDirectSoundBuffer || !m_pDirectSound3DBuffer || AEAudioHardware.m_n3dEffectsQueryResult != 1) {
        return;
    }

    IKsPropertySet* propertySet{};
    if (m_pDirectSound3DBuffer->QueryInterface(IID_IKsPropertySet, (LPVOID*)&propertySet) != S_OK) {
        return;
    }
    int32 room = onStreet ? 0 : -10000; // Sala del buffer (propiedad 7): máxima en la calle, mínima dentro
    propertySet->Set(s_EAXBufferPropertySet, 7, nullptr, 0, &room, sizeof(room));
    if (propertySet) {
        propertySet->Release();
    }
}
