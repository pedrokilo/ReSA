#pragma once

#include "AEAudioEntity.h"

class CVector;
class CWaterCannon;

/**
 * Sonoriza un CWaterCannon mediante cuatro capas persistentes.
 *
 * Dos siguen la punta activa del cañón y otras dos representan el punto de splash.
 * Los punteros CAESound son referencias a voces de AESoundManager, no ownership.
 */
class NOTSA_EXPORT_VTABLE CAEWaterCannonAudioEntity : public CAEAudioEntity {
public:
    bool      m_bInitialized;
    bool      m_bSplashInfoEnabled;
    CVector   m_vecPosn;
    float     m_fSplashMagnitude;
    CAESound* m_Sounds[4]; //!< Voces virtuales gestionadas externamente por AESoundManager.

public:
    CAEWaterCannonAudioEntity();
    ~CAEWaterCannonAudioEntity();

    void Initialise(CWaterCannon* waterCannon);
    void Terminate();
    static void StaticInitialise();

    void AddAudioEvent(eAudioEvents event, CVector& posn);
    void SetSplashInfo(CVector posn, float magnitude);
    void ClearSplashInfo();
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
    void UpdateGenericWaterCannonSound(bool splashInfoEnabled, int16 id, eSoundBankSlot bankSlotId, int16 sfxId, float speed, float volume, CVector posn, float soundDistance);
    void Service();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEWaterCannonAudioEntity* Constructor();

};

VALIDATE_SIZE(CAEWaterCannonAudioEntity, 0xA0);
