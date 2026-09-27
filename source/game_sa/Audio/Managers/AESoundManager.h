#pragma once

#include "AESound.h"
#include "Physical.h"
#include "Timer.h"
#include <Enums/eAudioEvents.h>

#undef PlaySound

static constexpr int32 MAX_NUM_SOUNDS = 300;
static constexpr int32 MAX_NUM_AUDIO_CHANNELS = 64;

enum eSoundPlayingStatus : int16 {
    SOUND_NOT_PLAYING = 0,
    SOUND_PLAYING = 1,
    SOUND_HAS_STARTED = 2,
};

using tSoundReference = int16;

/**
 * Planificador de hasta 300 voces virtuales sobre un número menor de canales físicos.
 *
 * CAESound conserva el estado lógico aunque no tenga canal hardware. Service()
 * prioriza qué voces merecen los canales disponibles, mantiene las uncancellable,
 * aplica pausa/Doppler/volumen y sincroniza play time con AEAudioHardware.
 */
class CAESoundManager {
public:
    uint16           m_NumAllocatedPhysicalChannels;
    int16            m_AudioHardwareHandle;
    CAESound         m_VirtuallyPlayingSoundList[MAX_NUM_SOUNDS]; //!< Pool fijo de voces lógicas.
    tSoundReference* m_PhysicallyPlayingSoundList;                        //!< Por canal físico, índice de la voz virtual asignada o -1.
    int16*           m_ChannelPosition;                                   //!< Play time reportado por cada canal físico.
    tSoundReference* m_PrioritisedSoundList;                              //!< Ranking temporal de voces candidatas a canal físico.
    int16            m_VirtualChannelSoundLengths[MAX_NUM_SOUNDS];        //!< Sound lengths in ms (Size: `m_AllocatedPhysicalChannels`)
    int16            m_VirtualChannelSoundLoopStartTimes[MAX_NUM_SOUNDS]; //!< Sound loop start times in ms (Size: `m_AllocatedPhysicalChannels`)
    uint32           m_TimeLastCalled;
    bool             m_WasGamePausedLastFrame;
    bool             m_IsManuallyPaused;
    uint32           m_TimeLastCalledUnpaused;

public:
    static void InjectHooks();

    CAESoundManager() = default;  // 0x4EFD30
    ~CAESoundManager() = default; // 0x4EFD60

    bool      Initialise();
    void      Terminate();
    void      Reset();
    void      PauseManually(uint8 bPause);
    void      Service();
    CAESound* RequestNewSound(CAESound* pSound);

    struct tSoundPlayParams {
        eSoundBankSlot  BankSlotID;               //!< Slot que contiene el banco solicitado.
        eSoundID        SoundID;                  //!< The sound ID to play (From the specified bank slot)
        CAEAudioEntity* AudioEntity;              //!< Controlador lógico que recibirá callbacks; no owned.
        CVector         Pos;                      //!< The position
        float           Volume;                   //!< The volume (in dB)
        float           RollOffFactor{ 1.f };     //!< ??
        float           Speed{ 1.f };             //!< Speed
        float           Doppler{ 1.f };           //!< Scales the factor of doppler shift effect, see `CAEAudioEnvironment::GetDopplerRelativeFrequency`
        uint8           FrameDelay{ 0 };          //!< After how many frames the sound will be played
        uint32          Flags{ 0 };               //!< See `eSoundEnvironment`
        float           FrequencyVariance{ 0.f }; //!< Random speed variance (?)
        int16           PlayTime{ 0 };            //!< Where to start the sound (AKA `CurrentPlayPosition`)

        // NOTSA Args //
        CEntity*        RegisterWithEntity{};     //!< Entidad world cuya vida/posición seguirá la voz; activa el flag correspondiente.
        int32           EventID{ AE_UNDEFINED };  //!< Event ID, this can be anything, but it's usually `eAudioEvents` value
        float           ClientVariable{ 0.f };    //!< Custom value that is just stored
    };
    /*!
     * @brief Construye y solicita una nueva voz virtual a partir de parámetros de alto nivel.
     * @return Slot CAESound asignado, o nullptr si el pool virtual está lleno.
     */
    CAESound* PlaySound(tSoundPlayParams p);
    int16     AreSoundsPlayingInBankSlot(int16 bankSlot);
    int16     AreSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity);
    int16     AreSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical);
    void      CancelSoundsOfThisEventPlayingForThisEntity(int16 eventId, CAEAudioEntity* audioEntity);
    void      CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(int16 eventId, CAEAudioEntity* audioEntity, CPhysical* physical);
    void      CancelSoundsInBankSlot(int16 bankSlot, bool bFullStop);
    void      CancelSoundsOwnedByAudioEntity(CAEAudioEntity* audioEntity, bool bFullStop);
    int16     GetVirtualChannelForPhysicalChannel(int16 physicalChannel) const;

    auto GetPhysicallyPlayingSoundList() const { return std::span{ m_PhysicallyPlayingSoundList, m_NumAllocatedPhysicalChannels }; }
    auto GetChannelPositions() const { return std::span{ m_ChannelPosition, m_NumAllocatedPhysicalChannels }; }
    auto GetPrioritisedSoundList() const { return std::span{ m_PrioritisedSoundList, m_NumAllocatedPhysicalChannels }; }

private:
    CAESound* GetFreeSound(size_t* outIdx);

public:
    bool IsPaused() const { return CTimer::GetIsPaused() || m_IsManuallyPaused; }
    bool IsSoundPaused(const CAESound& sound) const { return CAESoundManager::IsPaused() && !sound.IsUnpausable(); }
};
VALIDATE_SIZE(CAESoundManager, 0x8CBC);

extern CAESoundManager& AESoundManager;
