/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEStreamThread
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEStreamThread.h"
#include "AEAudioUtility.h"
#include "AEMP3TrackLoader.h"
#include "AEUserRadioTrackManager.h"
#include "AEVorbisDecoder.h"

namespace {
// Como el original: el EXE guarda y compara el eax entero de CAEAudioChannel::GetPlayTime (+0xC) y GetLength (+0x10)
// (en el canal de streaming son valores de 32 bits); se llama por la vtable del objeto como el EXE.
int32 ChannelPlayTime(CAEStreamingChannel* channel) {
    return reinterpret_cast<int32(__thiscall*)(CAEStreamingChannel*)>((*reinterpret_cast<void***>(channel))[3])(channel);
}

int32 ChannelLength(CAEStreamingChannel* channel) {
    return reinterpret_cast<int32(__thiscall*)(CAEStreamingChannel*)>((*reinterpret_cast<void***>(channel))[4])(channel);
}
} // namespace

/**
 * Registra el worker que prepara decoders y alimenta el canal de streaming.
 */
void CAEStreamThread::InjectHooks() {
    RH_ScopedClass(CAEStreamThread);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(Initialise, 0x4F1680);
    RH_ScopedInstall(Start, 0x4F11F0);
    RH_ScopedInstall(Stop, 0x4F1590);
    RH_ScopedInstall(Pause, 0x4F1200);
    RH_ScopedInstall(Resume, 0x4F1210);
    RH_ScopedInstall(WaitForExit, 0x4F1220);
    RH_ScopedInstall(PlayTrack, 0x4F1230);
    RH_ScopedInstall(GetTrackPlayTime, 0x4F1530);
    RH_ScopedInstall(GetTrackLengthMs, 0x4F1550);
    RH_ScopedInstall(GetActiveTrackID, 0x4F1560);
    RH_ScopedInstall(GetPlayingTrackID, 0x4F1570);
    RH_ScopedInstall(StopTrack, 0x4F1580);
    RH_ScopedInstall(MainLoop, 0x4F15C0);
    RH_ScopedInstall(Service, 0x4F1290);
}

// 0x4F11E0
CAEStreamThread::~CAEStreamThread() {
    DeleteCriticalSection(&m_criticalSection); // Como el original: DeleteCriticalSection (IAT 0x8580E0) sobre la sección crítica.
}

// 0x4F1680
/**
 * Captura el estado del CAEStreamingChannel, crea el thread suspendido y adquiere
 * el CAEMP3TrackLoader usado para resolver tracks internos.
 */
bool CAEStreamThread::Initialise(CAEStreamingChannel* streamingChannel) {
    m_bActive          = true;
    m_bThreadActive    = false;
    m_bNeedsService = 0;
    m_bPreparingStream = 0;
    m_bStopRequest     = false;
    m_nPlayingTrackId  = streamingChannel->GetPlayingTrackID();
    m_nPlayTime        = ChannelPlayTime(streamingChannel);
    m_nActiveTrackId   = streamingChannel->GetActiveTrackID();
    m_nTrackLengthMs   = ChannelLength(streamingChannel);

    m_nHandle = CreateThread(nullptr, 0, &CAEStreamThread::MainLoop, this, CREATE_SUSPENDED, (LPDWORD)(&m_lpThreadId));
    assert(m_nHandle);
    SetThreadPriority(m_nHandle, THREAD_PRIORITY_NORMAL);
    InitializeCriticalSection(&m_criticalSection);
    //TODO: NOTSA: Replace CriticalSection usages in this class with platform independant mutex from oswrapper

    m_pStreamingChannel = streamingChannel;

    m_pMp3TrackLoader = new CAEMP3TrackLoader();
    m_pMp3TrackLoader->Initialise();

    return m_bActive;
}

// 0x4F11F0
void CAEStreamThread::Start() {
    m_bThreadActive = true;
    ResumeThread(m_nHandle);
}

// 0x4F1590
void CAEStreamThread::Stop() {
    m_bThreadActive = false;
    if (std::exchange(m_bActive, false)) {
        delete m_pMp3TrackLoader;
    }
}

// 0x4F1200
void CAEStreamThread::Pause() const {
    SuspendThread(m_nHandle);
}

// 0x4F1210
void CAEStreamThread::Resume() const {
    ResumeThread(m_nHandle);
}

// 0x4F1220
void CAEStreamThread::WaitForExit() const {
    WaitForSingleObject(m_nHandle, INFINITE);
}

// 0x4F1530
int32 CAEStreamThread::GetTrackPlayTime() const {
    if (m_bPreparingStream)
        return -8;
    else
        return ChannelPlayTime(m_pStreamingChannel);
}

// 0x4F1550
int32 CAEStreamThread::GetTrackLengthMs() const {
    return m_nTrackLengthMs;
}

// 0x4F1560
int32 CAEStreamThread::GetActiveTrackID() const {
    return m_nActiveTrackId;
}

// 0x4F1570
int32 CAEStreamThread::GetPlayingTrackID() const {
    return m_nPlayingTrackId;
}

// 0x4F1230
// Publica bajo critical section una nueva petición de track actual/siguiente para Service().
void CAEStreamThread::PlayTrack(uint32 iTrackId, int32 iNextTrackId, uint32 a3, int32 a4, bool bIsUserTrack, bool bNextIsUserTrack) {
    EnterCriticalSection(&m_criticalSection);

    if (ChannelPlayTime(m_pStreamingChannel) == -2) {
        m_pStreamingChannel->Stop();
    }

    m_iTrackId         = iTrackId;
    m_iNextTrackId     = iNextTrackId;
    field_1C           = a3;
    m_TrackFlags = a4;
    m_bIsUserTrack     = bIsUserTrack;
    m_bNextIsUserTrack = bNextIsUserTrack;
    m_bNeedsService = 1;

    LeaveCriticalSection(&m_criticalSection);
}

// 0x4F1580
void CAEStreamThread::StopTrack() {
    m_bStopRequest = true;
}

// 0x4F15C0
/**
 * Loop del worker. Pausa el canal al perder foreground, sirve cambios de stream y
 * delega el bombeo continuo a CAEStreamingChannel.
 */
DWORD WINAPI CAEStreamThread::MainLoop(LPVOID data) {
#ifdef TRACY_ENABLE
    tracy::SetThreadName("AEStreamThread");
#endif

    const auto self = reinterpret_cast<CAEStreamThread*>(data);

    bool wasForeground = true;
    bool play = false;

    while (self->m_bThreadActive) {
        ZoneScoped;

        bool isForeground = IsForegroundApp();
        if (isForeground) {
            if (!wasForeground && play){
                self->m_pStreamingChannel->Play(0, 0, 1.0f);
            }

            // Como el original: solo los 32 bits bajos del tiempo (eax de _ftol2), resta sin signo
            const auto start = static_cast<uint32>(CAEAudioUtility::GetCurrentTimeInMS());
            self->Service();
            self->m_pStreamingChannel->Service();
            const auto end = static_cast<uint32>(CAEAudioUtility::GetCurrentTimeInMS());
            if (end - start < 5) {
                OS_ThreadSleep(start - end + 5);
            }
        } else if (wasForeground) {
            if (self->m_pStreamingChannel->IsBufferPlaying()) {
                self->m_pStreamingChannel->Pause();
                play = true;
            } else {
                play = false;
            }
        }
        wasForeground = isForeground;
        OS_ThreadSleep(100);
    }

    return 0;
}

// 0x4F1290
/**
 * Procesa atómicamente las peticiones pendientes de reproducción.
 *
 * Construye el decoder adecuado para tracks internos o user tracks, prepara el
 * siguiente stream y conserva una copia estable del estado visible para otros hilos.
 */
void CAEStreamThread::Service() {
    m_pStreamingChannel->UpdatePlayTime();
    if (m_bStopRequest) {
        m_bStopRequest = false; // Como el original: se baja antes de llamar a Stop
        m_pStreamingChannel->Stop();
    }

    const auto SaveStreamingState = [&] {
        if (m_bPreparingStream)
            return;

        m_nPlayingTrackId = m_pStreamingChannel->GetPlayingTrackID();
        m_nPlayTime = ChannelPlayTime(m_pStreamingChannel);
        m_nActiveTrackId = m_pStreamingChannel->GetActiveTrackID();
        m_nTrackLengthMs = ChannelLength(m_pStreamingChannel);
    };

    if (!m_bNeedsService) {
        SaveStreamingState();
        return;
    }

    EnterCriticalSection(&m_criticalSection);
    m_bNeedsService = false;

    const auto LoadDecoder = [&](bool userTrackCheck, uint32 trackId) {
        CAEStreamingDecoder* ptr{};
        if (userTrackCheck) {
            ptr = AEUserRadioTrackManager.LoadUserTrack(trackId);
        } else {
            ptr = new CAEVorbisDecoder(m_pMp3TrackLoader->GetDataStream(trackId), 0);
        }

        if (ptr && !ptr->Initialise()) {
            delete std::exchange(ptr, nullptr);
        }
        return ptr;
    };

    CAEStreamingDecoder* nextDecoder{};
    if (m_iNextTrackId != -1) { // RADIO_INVALID
         nextDecoder = LoadDecoder(m_bNextIsUserTrack, m_iNextTrackId);
    } else {
        m_pStreamingChannel->SetNextStream(nullptr);
    }

    if (m_iNextTrackId == -1 || m_pStreamingChannel->GetPlayingTrackID() != m_iTrackId) {
        auto* currDecoder = LoadDecoder(m_bIsUserTrack, m_iTrackId);
        if (currDecoder) {
            // Como el original: posición pedida (field_1C) módulo la duración, división sin signo
            currDecoder->SetCursor(field_1C % static_cast<uint32>(currDecoder->GetStreamLengthMs()));

            m_pStreamingChannel->SetNextStream(nextDecoder);
            m_pStreamingChannel->PrepareStream(currDecoder, m_TrackFlags, 1u);
            m_bPreparingStream = false;
        } else {
            m_pStreamingChannel->SetNextStream(nullptr);
            if (nextDecoder) {
                m_pStreamingChannel->PrepareStream(nextDecoder, m_TrackFlags, 1u);
                m_bPreparingStream = false;
            } else {
                m_bPreparingStream = true;
            }
        }
    } else {
        if (nextDecoder)
            m_pStreamingChannel->SetNextStream(nextDecoder);
        m_pStreamingChannel->SetReady();
    }

    if (m_bPreparingStream) {
        m_nPlayingTrackId = m_iNextTrackId == -1 ? m_iTrackId : m_iNextTrackId;
        m_nActiveTrackId = m_iNextTrackId == -1 ? m_iTrackId : m_iNextTrackId;
    } else {
        SaveStreamingState();
    }

    LeaveCriticalSection(&m_criticalSection);
}
