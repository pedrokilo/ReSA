/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/**
 * Controla dos CAESound looped como una sola fuente continua.
 *
 * Ambas voces se crean en el mismo bank slot y permanecen activas, pero sólo una es
 * audible. Al vencer m_TimeToSwapSoundsMs se transfiere el volumen a la otra voz y
 * se programa un nuevo intervalo aleatorio, evitando reiniciar los samples.
 *
 * m_AudioEntity referencia al controlador que originó este twin-loop; no es propiedad
 * de esta clase.
 */

#include "AEAudioEntity.h"

#include <Enums/SoundIDs.h>
#include <Enums/eSoundBankSlot.h>

class NOTSA_EXPORT_VTABLE CAETwinLoopSoundEntity : public CAEAudioEntity {
    static constexpr uint32 NUM_SOUNDS = 2;
public:
    CAETwinLoopSoundEntity() = default;
    CAETwinLoopSoundEntity(
        eSoundBankSlot  bank,
        eSoundID        soundA,
        eSoundID        soundB,
        CAEAudioEntity* parent,
        uint16          swapTimeMin,
        uint16          swapTimeMax,
        int16           playPercentageA,
        int16           playPercentageB
    );

    ~CAETwinLoopSoundEntity();

    void Initialise(
        eSoundBankSlot  bank,
        eSoundID        soundA,
        eSoundID        soundB,
        CAEAudioEntity* parent,
        uint16          swapTimeMin,
        uint16          swapTimeMax,
        int16           playPercentageA = -1,
        int16           playPercentageB = -1
    );

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void UpdateTwinLoopSound(CVector pos, float volume, float speed);
    void SwapSounds();
    void StopSound();
    void StopSoundAndForget();

    float GetEmittedVolume() const;
    void  SetEmittedVolume(float volume) const;

    bool IsTwinLoopPlaying() const;
    bool DoSoundsSwitchThisFrame() const;

    void PlayTwinLoopSound(CVector pos, float volume, float speed, float rollOffFactor, float doppler = 1.f, eSoundEnvironment flags = SOUND_DEFAULT);

    bool IsActive() const noexcept { return m_IsInUse; }
private:
    void ReCalculateSwapTime();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

private:
    eSoundBankSlotS16 m_BankSlot{};
    std::array<eSoundID, NUM_SOUNDS>  m_SoundIDs{};
    CAEAudioEntity*                   m_AudioEntity{}; //!< Controlador lógico padre; no owned.
    int16                             m_IsInUse{};
    std::array<int16, NUM_SOUNDS>     m_SoundsLengths{};
    uint16                            m_SwapTimeMin{};
    uint16                            m_SwapTimeMax{};
    uint32                            m_TimeToSwapSoundsMs{}; //!< Timestamp de pausa para el próximo intercambio.
    bool                              m_IsPlayingFirstSound{};
    std::array<int16, NUM_SOUNDS>     m_SoundPlayPercentages{}; // 0 - 99%
    std::array<CAESound*, NUM_SOUNDS> m_Sounds{}; //!< Voces administradas por AESoundManager; no owned.
};
VALIDATE_SIZE(CAETwinLoopSoundEntity, 0xA8);
