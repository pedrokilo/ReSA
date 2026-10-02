#pragma once

#include "AEWeaponAudioEntity.h"

class CPed;

class CAEPedWeaponAudioEntity : public CAEWeaponAudioEntity {
public:
    static void InjectHooks();

    CAEPedWeaponAudioEntity();
    ~CAEPedWeaponAudioEntity() = default;

    void Initialise(CPed* ped);
    void Terminate();
    void AddAudioEvent(eAudioEvents event);
    void Service();

private:
    bool  m_bInitialised;
    CPed* m_Ped;
};
VALIDATE_SIZE(CAEPedWeaponAudioEntity, 0xA8);
