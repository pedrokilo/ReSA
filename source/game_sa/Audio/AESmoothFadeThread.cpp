/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAESmoothFadeThread
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AESmoothFadeThread.h"

#include "AEAudioChannel.h"
#include "AEAudioUtility.h"

auto& AESmoothFadeThread = StaticRef<CAESmoothFadeThread>(0xB608D0);

namespace {
// Como el original: volumen DirectSound (centésimas de dB, cargado con fild) · 0.01f (0x858C58) en x87 a la
// precisión vigente, redondeado a float.
float DsVolumeToDb(int32 dsVolume) {
    static const float kToDb = 0.01f;
    float volume;
    __asm {
        fild  dword ptr dsVolume
        fmul  kToDb
        fstp  volume
    }
    return volume;
}

// Como el original: volumen · 100 (0x858628) truncado con _ftol2 (0x821B40, 64 bits; se usa la parte baja).
LONG DbToDsVolume(float volume) {
    return (LONG)(int64)(volume * 100.0f);
}
} // namespace

/**
 * Registra la cola y el hilo encargado de transiciones suaves de volumen DirectSound.
 */
void CAESmoothFadeThread::InjectHooks() {
    RH_ScopedClass(CAESmoothFadeThread);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(Start, 0x4EEA10);
    RH_ScopedInstall(Stop, 0x4EEA30);
    RH_ScopedInstall(WaitForExit, 0x4EEA40);
    RH_ScopedInstall(Initialise, 0x4EEEC0);
    RH_ScopedInstall(InitialiseRequestSlots, 0x4EE9F0);
    RH_ScopedInstall(CancelFade, 0x4EEA50);
    RH_ScopedInstall(SetBufferVolume, 0x4EEC80);
    RH_ScopedInstall(SmoothFadeProc, 0x4EEE90);
    RH_ScopedInstall(RequestFade, 0x4EEAA0);
    RH_ScopedInstall(Service, 0x4EED10);
}

CAESmoothFadeThread::CAESmoothFadeThread() {
    m_threadHandle         = INVALID_HANDLE_VALUE;
    m_dwThreadId           = 0;
    m_bInitialized       = false;
    m_bActive              = false;
    m_numRequests                = 0;
    m_lastRequest               = 0;
    m_currentTime     = 0;
    m_nNumAvailableBuffers = 0;
}

/**
 * Reinicia los slots y crea el thread suspendido; Start() controla cuándo comienza a servirlos.
 */
bool CAESmoothFadeThread::Initialise() {
    InitialiseRequestSlots();
    m_threadHandle = CreateThread(nullptr, 0, &CAESmoothFadeThread::SmoothFadeProc, this, CREATE_SUSPENDED, &m_dwThreadId);
    if (m_threadHandle == INVALID_HANDLE_VALUE || !m_threadHandle) { // NOTSA: nullptr check
        m_bInitialized = false;
        return false;
    }
    SetThreadPriority(m_threadHandle, 0);
    m_bSmoothFadesDisabled = false;
    m_bInitialized = true;
    return true; // Como el original: devuelve si se creó el hilo
}

void CAESmoothFadeThread::InitialiseRequestSlots() {
    for (auto& entry : m_aEntries) {
        entry.m_nStatus = eSmoothFadeEntryStatus::STATE_IDLE;
        entry.m_pSoundBuffer = nullptr;
    }
}

void CAESmoothFadeThread::Start() {
    m_bActive = true;
    ResumeThread(m_threadHandle);
}

void CAESmoothFadeThread::Stop() {
    m_bActive = false;
}

void CAESmoothFadeThread::WaitForExit() {
    WaitForSingleObject(m_threadHandle, -1);
}

/**
 * Avanza cada fade activo, escribe el volumen interpolado y libera la referencia
 * COM al buffer al completar o cancelar la solicitud.
 */
void CAESmoothFadeThread::Service() {
    m_currentTime = (uint32)CAEAudioUtility::GetCurrentTimeInMS();
    for (auto& entry : m_aEntries) {
        if (entry.m_nStatus == eSmoothFadeEntryStatus::STATE_ACTIVE) { // Como el original: sólo el estado 1 avanza
            if (!entry.m_pSoundBuffer)
                continue;

            if (m_currentTime < entry.m_nStartTime)
                continue;

            const uint32 elapsed = m_currentTime - entry.m_nStartTime;
            if (elapsed < entry.m_wFadeTime) {
                // Como el original: todo en la pila x87. Paso lineal pow(10, diff · 0.05) - 1 con el _CIpow del EXE
                // (0x822130), progreso = transcurrido (sin signo) / duración, y
                // volumen = inicio + 20 · log10(1 + paso · progreso) con fyl2x.
                static const double kTen = 10.0;
                static const float  kToLinear = std::bit_cast<float>(0x3D4CCCCDu), kOne = 1.0f, k2pow32 = 4294967296.0f, kDecibels = 20.0f;
                const float  diff = entry.m_fVolumeDiff, start = entry.m_fStartVolume;
                const int32  fadeTime = entry.m_wFadeTime;
                float        volume;
                __asm {
                    fld   kTen
                    fld   diff
                    fmul  kToLinear
                    mov   eax, 0x822130
                    call  eax
                    fsub  kOne
                    fild  dword ptr elapsed
                    cmp   elapsed, 0
                    jge   L_elapsedPositive
                    fadd  k2pow32
                L_elapsedPositive:
                    fidiv dword ptr fadeTime
                    fmulp st(1), st
                    fadd  kOne
                    fldlg2
                    fxch  st(1)
                    fyl2x
                    fmul  kDecibels
                    fadd  start
                    fstp  volume
                }
                entry.m_fCurVolume = volume;
                entry.m_pSoundBuffer->SetVolume(DbToDsVolume(volume));
                continue;
            }

            int32 curVolume;
            entry.m_pSoundBuffer->GetVolume((LPLONG)&curVolume);
            const auto fCurVolume = DsVolumeToDb(curVolume);
            if (entry.m_bStopBufferAfterFade || approxEqual2(fCurVolume, entry.m_fCurVolume, 0.01F)) {
                entry.m_pSoundBuffer->SetVolume(DbToDsVolume(entry.m_fTargetVolume));
                if (entry.m_bStopBufferAfterFade)
                    entry.m_pSoundBuffer->Stop();
            }
        } else if (entry.m_nStatus != eSmoothFadeEntryStatus::STATE_CANCELLED) {
            continue;
        }

        --g_numSoundChannelsUsed;
        if (entry.m_pSoundBuffer)
            entry.m_pSoundBuffer->Release();

        entry.m_pSoundBuffer = nullptr;
        entry.m_nStatus = eSmoothFadeEntryStatus::STATE_IDLE;
    }
}

void CAESmoothFadeThread::CancelFade(IDirectSoundBuffer* buffer) {
    if (m_bSmoothFadesDisabled)
        return;

    for (auto& entry : m_aEntries)
        if (entry.m_nStatus == eSmoothFadeEntryStatus::STATE_ACTIVE && entry.m_pSoundBuffer == buffer)
            entry.m_nStatus = eSmoothFadeEntryStatus::STATE_CANCELLED;
}

/**
 * Inserta o actualiza un fade para un buffer que esté reproduciéndose.
 *
 * AddRef() mantiene vivo el IDirectSoundBuffer mientras el thread lo usa; Service()
 * equilibra esa referencia al completar/cancelar la entrada.
 */
bool CAESmoothFadeThread::RequestFade(IDirectSoundBuffer* buffer, float fTargetVolume, int16 fadeTime, bool bStopBufferAfterFade) {
    if (!m_bInitialized || m_bSmoothFadesDisabled)
        return false;

    uint32 status;
    buffer->GetStatus((LPDWORD)&status);
    if (!(status & DSBSTATUS_PLAYING))
        return false;

    int32 curVolume;
    buffer->GetVolume((LPLONG)&curVolume);
    const auto fCurVolume = DsVolumeToDb(curVolume);

    if (approxEqual2(fCurVolume, fTargetVolume, 0.01F)) {
        if (bStopBufferAfterFade)
            buffer->Stop();

        return true;
    }

    bool bFound = false;
    int32  iFreeInd = -1;
    for (int32 i = 0; i < (int32)m_nNumAvailableBuffers; ++i) { // Como el original: comparación con signo
        auto& sound = m_aEntries.data()[i];
        if (sound.m_nStatus == eSmoothFadeEntryStatus::STATE_IDLE && !bFound) {
            bFound = true;
            iFreeInd = i;
            continue;
        }

        if (sound.m_nStatus != eSmoothFadeEntryStatus::STATE_ACTIVE)
            continue;

        if (sound.m_pSoundBuffer != buffer)
            continue;

        if (fadeTime + sound.m_nStartTime - m_currentTime <= 1)
            continue;

        bFound = true;
        iFreeInd = i;
        break;
    }

    if (!bFound)
        return false;

    auto& sound = m_aEntries.data()[iFreeInd];
    if (sound.m_nStatus == eSmoothFadeEntryStatus::STATE_ACTIVE) {
        if (sound.m_bStopBufferAfterFade)
            return true;
    } else {
        ++g_numSoundChannelsUsed;
        buffer->AddRef();
        sound.m_pSoundBuffer = buffer;
    }

    sound.m_fTargetVolume        = fTargetVolume;
    sound.m_fCurVolume           = fCurVolume;
    sound.m_fStartVolume         = fCurVolume;
    sound.m_fVolumeDiff          = fTargetVolume - fCurVolume;
    sound.m_bStopBufferAfterFade = bStopBufferAfterFade;

    if (fadeTime == -2)
        sound.m_wFadeTime = (status & DSBSTATUS_LOCHARDWARE) ? 20 : 28;
    else if (fadeTime == -1)
        sound.m_wFadeTime = (status & DSBSTATUS_LOCHARDWARE) ? 20 : 30;
    else
        sound.m_wFadeTime = fadeTime;

    sound.m_nStartTime = m_currentTime;
    sound.m_nStatus = eSmoothFadeEntryStatus::STATE_ACTIVE;

    return true;
}

// Escribe directamente el volumen salvo que exista un fade activo incompatible, que se cancela.
void CAESmoothFadeThread::SetBufferVolume(IDirectSoundBuffer* buffer, float volume) {
    const auto newVolume = DbToDsVolume(volume);
    if (m_bSmoothFadesDisabled) {
        buffer->SetVolume(newVolume);
        return;
    }

    int32 entryInd;
    for (entryInd = 0; entryInd < NUM_SMOOTHFADE_ENTRIES; ++entryInd) {
        auto& entry = m_aEntries[entryInd];
        if (entry.m_nStatus == eSmoothFadeEntryStatus::STATE_ACTIVE && entry.m_pSoundBuffer == buffer)
            break;
    }

    if (entryInd >= NUM_SMOOTHFADE_ENTRIES)
        buffer->SetVolume(newVolume);
    else {
        auto& entry = m_aEntries[entryInd];
        if (entry.m_bStopBufferAfterFade || approxEqual2(volume, entry.m_fTargetVolume, 0.01F))
            return;

        entry.m_nStatus = eSmoothFadeEntryStatus::STATE_CANCELLED;
        buffer->SetVolume(newVolume);
    }
}

// Loop del thread: procesa fades aproximadamente cada milisegundo mientras esté activo.
DWORD WINAPI CAESmoothFadeThread::SmoothFadeProc(void* smoothFade) {
#ifdef TRACY_ENABLE
    tracy::SetThreadName("AESmoothFadeThread");
#endif

    auto* fade = static_cast<CAESmoothFadeThread*>(smoothFade);
    while (fade->m_bActive) {
        ZoneScoped;

        fade->Service();
        Sleep(1);
    }

    return 0;
}
