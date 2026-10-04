/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAESoundManager
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

#include "AESoundManager.h"

#include "AEAudioEnvironment.h"
#include "AEAudioHardware.h"

#include <reversiblebugfixes/Bugs.hpp>

auto& AESoundManager = StaticRef<CAESoundManager>(0xB62CB0);

/**
 * Registra la planificación de voces virtuales sobre los canales físicos disponibles.
 */
void CAESoundManager::InjectHooks() {
    RH_ScopedClass(CAESoundManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Service, 0x4F0000);
    RH_ScopedInstall(Initialise, 0x5B9690);
    RH_ScopedInstall(Terminate, 0x4EFAA0);
    RH_ScopedInstall(Reset, 0x4EF4D0);
    RH_ScopedInstall(PauseManually, 0x4EF510);
    RH_ScopedInstall(RequestNewSound, 0x4EFB10);
    RH_ScopedInstall(AreSoundsPlayingInBankSlot, 0x4EF520);
    RH_ScopedInstall(AreSoundsOfThisEventPlayingForThisEntity, 0x4EF570);
    RH_ScopedInstall(AreSoundsOfThisEventPlayingForThisEntityAndPhysical, 0x4EF5D0);
    RH_ScopedInstall(GetVirtualChannelForPhysicalChannel, 0x4EF630);
    RH_ScopedInstall(CancelSoundsInBankSlot, 0x4EFC60);
    RH_ScopedInstall(CancelSoundsOwnedByAudioEntity, 0x4EFCD0);
    RH_ScopedInstall(CancelSoundsOfThisEventPlayingForThisEntity, 0x4EFB90);
    RH_ScopedInstall(CancelSoundsOfThisEventPlayingForThisEntityAndPhysical, 0x4EFBF0);
}

// 0x5B9690
/**
 * Reserva canales hardware y arrays auxiliares de planificación.
 *
 * Las 300 CAESound permanecen preasignadas como voces virtuales; sólo un subconjunto
 * cabe simultáneamente en los canales físicos devueltos por AEAudioHardware.
 */
bool CAESoundManager::Initialise() {
    const auto availChannels = AEAudioHardware.GetNumAvailableChannels();
    if (availChannels <= 10) {
        return false;
    }

    m_NumAllocatedPhysicalChannels = availChannels >= MAX_NUM_SOUNDS ? MAX_NUM_SOUNDS : availChannels;
    m_AudioHardwareHandle       = AEAudioHardware.AllocateChannels(m_NumAllocatedPhysicalChannels);
    if (m_AudioHardwareHandle == -1) {
        return false;
    }

    // Como el original: la lista física es un new[] (0x821195) de un tipo con constructor/destructor declarados,
    // con el número de elementos en un dword delante (cookie); las otras dos son new[] sin cookie.
    if (auto* const cookie = static_cast<uint32*>(::operator new[](m_NumAllocatedPhysicalChannels * sizeof(tSoundReference) + sizeof(uint32)))) {
        *cookie                      = m_NumAllocatedPhysicalChannels;
        m_PhysicallyPlayingSoundList = reinterpret_cast<tSoundReference*>(cookie + 1);
    } else {
        m_PhysicallyPlayingSoundList = nullptr;
    }
    m_ChannelPosition      = static_cast<int16*>(::operator new[](m_NumAllocatedPhysicalChannels * sizeof(int16)));
    m_PrioritisedSoundList = static_cast<tSoundReference*>(::operator new[](m_NumAllocatedPhysicalChannels * sizeof(tSoundReference)));

    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        sound.m_IsInUse             = false;
        sound.m_IsPhysicallyPlaying = false;
    }

    std::fill_n(m_PhysicallyPlayingSoundList, m_NumAllocatedPhysicalChannels, -1);

    m_TimeLastCalled         = CTimer::GetTimeInMS();
    m_WasGamePausedLastFrame = false;
    m_TimeLastCalledUnpaused = 0;
    m_IsManuallyPaused       = false;

    // Como el original: printf del CRT del juego (0x821982), sin reservas (spdlog guarda el mensaje en su backtrace
    // aunque el nivel esté apagado y reservaría/liberaría memoria)
    reinterpret_cast<int(__cdecl*)(const char*, ...)>(0x821982)("Initialised SoundManager\n");
    return true;
}

// 0x4EFAA0
void CAESoundManager::Terminate() {
    // Como el original: operator delete escalar (0x8214BD); la lista física se libera desde su cookie y solo si
    // existe, las otras dos siempre (aunque sean nulas).
    if (m_PhysicallyPlayingSoundList) {
        ::operator delete(reinterpret_cast<uint32*>(m_PhysicallyPlayingSoundList) - 1);
    }
    ::operator delete(m_ChannelPosition);
    ::operator delete(m_PrioritisedSoundList);

    m_PhysicallyPlayingSoundList = nullptr;
    m_ChannelPosition            = nullptr;
    m_PrioritisedSoundList       = nullptr;
}

// 0x4EF4D0
void CAESoundManager::Reset() {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive()) {
            continue;
        }

        sound.StopSound();
    }
}

// 0x4EF510
void CAESoundManager::PauseManually(uint8 bPause) {
    reinterpret_cast<uint8&>(m_IsManuallyPaused) = bPause; // Como el original: se guarda el byte tal cual (sin normalizar a bool)
}

// 0x4F0000
/**
 * Sincroniza cada frame las voces virtuales con los canales físicos.
 *
 * Actualiza tiempo/posición/volumen, mantiene sonidos uncancellable, prioriza el resto
 * por sus parámetros audibles, detiene voces desplazadas y asigna los canales libres.
 * Finalmente actualiza volumen, Doppler y pausa sobre los sonidos que siguen físicos.
 */
void CAESoundManager::Service() {
    // Clear sounds uncancellable status for this frame
    std::fill_n(m_PrioritisedSoundList, m_NumAllocatedPhysicalChannels, -1);

    // Calculate time diff from last update
    uint32 timeSinceLastUpdate;
    if (CAESoundManager::IsPaused()) {
        if (m_WasGamePausedLastFrame) {
            timeSinceLastUpdate = CTimer::GetTimeInMSPauseMode() - m_TimeLastCalled;
        } else {
            m_TimeLastCalledUnpaused = m_TimeLastCalled;
            timeSinceLastUpdate      = 0;
        }

        m_TimeLastCalled         = CTimer::GetTimeInMSPauseMode();
        m_WasGamePausedLastFrame = true;
    } else {
        timeSinceLastUpdate = CTimer::GetTimeInMS() - (m_WasGamePausedLastFrame ? m_TimeLastCalledUnpaused : m_TimeLastCalled);

        m_TimeLastCalled         = CTimer::GetTimeInMS();
        m_WasGamePausedLastFrame = false;
    }

    // Get current frame sounds infos
    AEAudioHardware.GetChannelPlayTimes(m_AudioHardwareHandle, m_ChannelPosition);
    AEAudioHardware.GetVirtualChannelSoundLengths(m_VirtualChannelSoundLengths);
    AEAudioHardware.GetVirtualChannelSoundLoopStartTimes(m_VirtualChannelSoundLoopStartTimes);

    // 0x4F011C - Initialize sounds that are using percentage specified start positions
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsActive() || !sound.IsAudioHardwareAware() || !sound.GetPlayTimeIsPercentage()) {
            continue;
        }

        sound.SetFlags(eSoundEnvironment::SOUND_START_PERCENTAGE, false);
        if (sound.m_IsPhysicallyPlaying) {
            continue;
        }

        // Como el original: fild del producto entero, fmul por 0,01f (0x858C58) en x87 y _ftol2
        int32       product = sound.m_PlayTime * m_VirtualChannelSoundLengths[i];
        const float percent = 0.01f;
        float       scaled;
        __asm {
            fild  product
            fmul  percent
            fstp  scaled
        }
        sound.m_PlayTime = static_cast<int16>(static_cast<int64>(scaled));
    }

    // 0x4F016D - Stop sounds that turned inactive
    for (auto&& [i, ref] : rngv::enumerate(GetPhysicallyPlayingSoundList())) {
        if (ref == -1) {
            continue;
        }
        auto& sound                  = m_VirtuallyPlayingSoundList[ref];
        sound.m_PlayTime = m_ChannelPosition[i];
        if (sound.m_HasRequestedStopped) {
            AEAudioHardware.StopSound(m_AudioHardwareHandle, i);
        }
    }

    // 0x4F01E4 - Update sounds playtime
    // Como el original: CAESound::UpdatePlayTime va en línea; el avance se suma en float al play time antes de
    // truncar con _ftol2 (16 bits bajos) y el módulo es con signo.
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsActive() || !sound.IsAudioHardwareAware() || sound.m_FrameDelay) {
            continue;
        }
        const int16 progress  = CAESoundManager::IsSoundPaused(sound) ? int16(0) : static_cast<int16>(timeSinceLastUpdate);
        const int16 length    = m_VirtualChannelSoundLengths[i];
        const int16 loopStart = m_VirtualChannelSoundLoopStartTimes[i];
        sound.m_Length = length;
        if (sound.m_IsPhysicallyPlaying) {
            continue;
        }
        if (sound.m_HasRequestedStopped) {
            sound.m_PlayTime = -1;
            continue;
        }
        const auto playTime = static_cast<int16>(static_cast<int64>(static_cast<float>(progress) * sound.m_ListenerSpeed + static_cast<float>(sound.m_PlayTime)));
        sound.m_PlayTime = playTime;
        if (playTime < length) {
            continue;
        }
        if (loopStart == -1) {
            sound.m_PlayTime = -1;
            continue;
        }
        if (length == 0 && notsa::bugfixes::AESound_UpdatePlayTime_DivisionByZero) { // NOTSA: el EXE divide por cero
            sound.m_PlayTime = loopStart;
            continue;
        }
        sound.m_PlayTime = static_cast<int16>(int32(playTime) % int32(length) + loopStart);
    }

    // 0x4F02FB - Remove sounds that ended from sounds table
    for (auto&& [chN, ref] : rngv::enumerate(GetChannelPositions())) {
        if (ref == -1) {
            GetPhysicallyPlayingSoundList()[chN] = -1;
        }
    }

    // 0x4F0329 - Mark sounds that ended as finished
    // Como el original: mira el retardo de frames (0x54), no m_IsAudioHardwareAware
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_FrameDelay || sound.m_PlayTime != -1) {
            continue;
        }
        sound.SoundHasFinished();
    }

    // 0x4F03E5, 0x4F040D - Update sounds positions and volumes
    // Como el original: dos pasadas, primero todos los UpdateParameters y después todos los CalculateVolume
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive()) {
            continue;
        }
        sound.UpdateParameters(sound.m_PlayTime);
    }
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive()) {
            continue;
        }
        sound.CalculateVolume();
    }

    // 0x4F042C - Find prioritized sounds
    auto numPrioritisedSounds = 0;
    for (auto&& [i, ref] : rngv::enumerate(GetPhysicallyPlayingSoundList())) {
        if (ref == -1) {
            continue;
        }
        if (!m_VirtuallyPlayingSoundList[ref].IsUnancellable()) {
            continue;
        }
        m_PrioritisedSoundList[numPrioritisedSounds++] = ref;
    }

    // Check if we need to insert any of the already playing sounds into the list
    for (auto&& [i, sound] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!sound.IsActive()) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying && sound.IsUnancellable()) {
            continue;
        }
        if (sound.m_FrameDelay != 0) {
            continue;
        }

        int16 chN = m_NumAllocatedPhysicalChannels - 1;

        // 0x4F04CE - Find last slot in use
        // Como el original: también mira el hueco numPrioritisedSounds (puede acabar en numPrioritisedSounds - 1)
        while (chN >= numPrioritisedSounds && m_PrioritisedSoundList[chN] == -1) {
            chN--;
        }

        // 0x4F04EB - Find where to insert
        // Como el original: sigue buscando si no es más bajo (NaN incluido) o si lo pide más físicamente;
        // se inserta detrás del primero más alto con PlayPhysically menor o igual.
        for (; chN >= numPrioritisedSounds; chN--) {
            const auto& soundB = m_VirtuallyPlayingSoundList[m_PrioritisedSoundList[chN]];
            if (!(sound.m_ListenerVolume < soundB.m_ListenerVolume)) {
                continue;
            }
            if (sound.GetPlayPhysically() > soundB.GetPlayPhysically()) {
                continue;
            }
            break;
        }

        // 0x4F0529 - Insert at given index
        if (chN != m_NumAllocatedPhysicalChannels - 1) {
            // Shift to right
            for (auto i = m_NumAllocatedPhysicalChannels - 1; i > chN + 1; --i) {
                m_PrioritisedSoundList[i] = m_PrioritisedSoundList[i - 1];
            }

            // Insert
            m_PrioritisedSoundList[chN + 1] = i;
        }
    }

    // 0x4F0585 - Stop songs that aren't marked as uncancellable in this frame
    for (auto i = 0; i < m_NumAllocatedPhysicalChannels; ++i) {
        const auto ref = m_PhysicallyPlayingSoundList[i];
        if (ref == -1) {
            continue;
        }

        int32 chN;
        for (chN = 0; chN < m_NumAllocatedPhysicalChannels; ++chN) {
            if (ref == m_PrioritisedSoundList[chN]) {
                break;
            }
        }

        if (chN == m_NumAllocatedPhysicalChannels) {
            m_VirtuallyPlayingSoundList[ref].m_IsPhysicallyPlaying = false;
            m_PhysicallyPlayingSoundList[i]                        = -1;
            AEAudioHardware.StopSound(m_AudioHardwareHandle, i);
        } else {
            m_PrioritisedSoundList[chN] = -1;
        }
    }

    // 0x4F0628 - Play sounds that require that
    for (auto i = 0, chN = 0; i < m_NumAllocatedPhysicalChannels; ++i) {
        const auto ref = m_PrioritisedSoundList[i];
        if (ref == -1) {
            continue;
        }

        // Find a free channel to use
        while (chN < m_NumAllocatedPhysicalChannels && m_PhysicallyPlayingSoundList[chN] != -1) {
            chN++;
        }
        if (chN >= m_NumAllocatedPhysicalChannels) {
            break;
        }

        m_PhysicallyPlayingSoundList[chN] = ref;
        auto& sound                       = m_VirtuallyPlayingSoundList[ref];
        sound.m_IsPhysicallyPlaying               = true;

        const auto freqFactor = sound.GetRelativePlaybackFrequencyWithDoppler() * sound.GetSlowMoFrequencyScalingFactor();

        CAEAudioHardwarePlayFlags flags{};
        flags.CopyFromAESound(sound);

        AEAudioHardware.PlaySound(m_AudioHardwareHandle, chN, sound.m_SoundID, sound.m_BankSlot, sound.m_PlayTime, flags.m_nFlags, sound.GetSpeed());
        AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, chN, sound.m_ListenerVolume, 0);
        AEAudioHardware.SetChannelPosition(m_AudioHardwareHandle, chN, sound.GetRelativePosition(), 0);
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, chN, freqFactor);
    }

    // 0x4F0894
    for (auto i = 0; i < m_NumAllocatedPhysicalChannels; ++i) {
        const auto ref = m_PhysicallyPlayingSoundList[i];
        if (ref == -1) {
            continue;
        }

        auto& sound = m_VirtuallyPlayingSoundList[ref];
        if (!sound.IsActive()) {
            continue;
        }

        if (!CAESoundManager::IsSoundPaused(sound)) {
            AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, i, sound.m_ListenerVolume, 0);
            auto freq        = sound.GetRelativePlaybackFrequencyWithDoppler();
            auto slomoFactor = sound.GetSlowMoFrequencyScalingFactor();
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, i, freq * slomoFactor);
        } else {
            AEAudioHardware.SetChannelVolume(m_AudioHardwareHandle, i, -100.0F, 0);
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_AudioHardwareHandle, i, 0.0F);
        }
        AEAudioHardware.SetChannelPosition(m_AudioHardwareHandle, i, sound.GetRelativePosition(), 0);
    }

    // 0x4F0AB8
    AEAudioHardware.Service();

    // 0x4F0B05
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive()) {
            continue;
        }

        sound.m_IsAudioHardwareAware = true;
        if (static_cast<int8>(sound.m_FrameDelay) > 0 && !CAESoundManager::IsSoundPaused(sound)) { // Como el original: byte con signo (jle)
            --sound.m_FrameDelay;
        }
    }
}

// 0x4EFB10
/**
 * Copia una voz temporal al primer slot virtual libre y solicita al hardware la
 * metadata necesaria para duración/loop de ese índice.
 */
CAESound* CAESoundManager::RequestNewSound(CAESound* pSound) {
    size_t sidx;
    auto   s = GetFreeSound(&sidx);
    if (s) {
        *s = *pSound;
        pSound->UnregisterWithPhysicalEntity();
        s->NewVPSLEntry();
        AEAudioHardware.RequestVirtualChannelSoundInfo((uint16)sidx, s->m_SoundID, s->m_BankSlot);
    }

    return s;
}

/**
 * Helper NOTSA que construye una CAESound temporal desde parámetros de alto nivel
 * y la entrega a RequestNewSound().
 */
CAESound* CAESoundManager::PlaySound(tSoundPlayParams p) {
    if (p.RegisterWithEntity) {
        p.Flags |= SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY;
    }
    CAESound s;
    s.Initialise(
        p.BankSlotID,
        p.SoundID,
        p.AudioEntity,
        p.Pos,
        p.Volume,
        p.RollOffFactor,
        p.Speed,
        p.Doppler,
        p.FrameDelay,
        p.Flags,
        p.FrequencyVariance,
        p.PlayTime
    );
    s.m_Event          = p.EventID;
    s.m_ClientVariable = p.ClientVariable;
    if (p.Flags & SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY) {
        s.RegisterWithPhysicalEntity(p.RegisterWithEntity);
    }
    return RequestNewSound(&s);
}

// 0x4EF520
int16 CAESoundManager::AreSoundsPlayingInBankSlot(int16 bankSlot) {
    auto nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_BankSlot != bankSlot) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }
    return nPlaying;
}

// 0x4EF570
int16 CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity) {
    auto nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_Event != eventId || sound.m_AudioEntity != audioEntity) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }

    return nPlaying;
}

// 0x4EF5D0
int16 CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical) {
    bool nPlaying = eSoundPlayingStatus::SOUND_NOT_PLAYING;
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_Event != eventId || sound.m_AudioEntity != audioEntity || sound.m_PhysicalEntity != physical) {
            continue;
        }
        if (sound.m_IsPhysicallyPlaying) {
            return eSoundPlayingStatus::SOUND_HAS_STARTED;
        }
        nPlaying = eSoundPlayingStatus::SOUND_PLAYING;
    }

    return nPlaying;
}

// 0x4EFB90
void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_Event != eventId || sound.m_AudioEntity != audioEntity) {
            continue;
        }
        sound.StopSoundAndForget();
    }
}

// 0x4EFBF0
void CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_Event != eventId || sound.m_AudioEntity != audioEntity || sound.m_PhysicalEntity != physical) {
            continue;
        }
        sound.StopSoundAndForget();
    }
}

// 0x4EFC60
void CAESoundManager::CancelSoundsInBankSlot(int16 bankSlot, bool bFullStop) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_BankSlot != bankSlot) {
            continue;
        }
        if (bFullStop) {
            sound.StopSoundAndForget();
        } else {
            sound.StopSound();
        }
    }
}

// 0x4EFCD0
void CAESoundManager::CancelSoundsOwnedByAudioEntity(CAEAudioEntity* audioEntity, bool bFullStop) {
    for (CAESound& sound : m_VirtuallyPlayingSoundList) {
        if (!sound.IsActive() || sound.m_AudioEntity != audioEntity) {
            continue;
        }
        if (bFullStop) {
            sound.StopSoundAndForget();
        } else {
            sound.StopSound();
        }
    }
}

// 0x4EF630, unused
int16 CAESoundManager::GetVirtualChannelForPhysicalChannel(int16 physicalChannel) const {
    const auto chIdx = static_cast<int16>(physicalChannel - m_AudioHardwareHandle); // Como el original: resta de 16 bits
    if (chIdx < 0 || chIdx >= m_NumAllocatedPhysicalChannels) {
        return -1;
    }
    return m_PhysicallyPlayingSoundList[chIdx];
}

// NOTSA
// Busca linealmente el primer slot virtual marcado como inactivo.
CAESound* CAESoundManager::GetFreeSound(size_t* outIdx) {
    for (auto&& [i, s] : rngv::enumerate(m_VirtuallyPlayingSoundList)) {
        if (!s.IsActive()) {
            if (outIdx) {
                *outIdx = (size_t)i;
            }
            return &s;
        }
    }
    return nullptr;
}
