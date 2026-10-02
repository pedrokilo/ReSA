/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AESound.h"

#include "eAudioEvents.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CAEAudioEntity {
public:
    CEntity* m_Entity{};
    CAESound m_tempSound{};

protected:
    CAEAudioEntity() = default;
    ~CAEAudioEntity() = default;

public:
    static void InjectHooks();

    // 0x4D7E30 (vtable 0x859988, entrada 0)
    virtual void UpdateParameters(CAESound* sound, int16 curPlayPos) { /* Empty on purpose */ };

    static bool StaticInitialise();
    static void Shutdown();
    static float GetDefaultVolume(eAudioEvents event);

private: // Envoltorios para los hooks
    // 0x5DE9E0
    CAEAudioEntity* Destructor() {
        // Como el original: restaura la vtable base del EXE (0x859988) y destruye m_tempSound.
        *reinterpret_cast<uintptr_t*>(this) = 0x859988;
        m_tempSound.~CAESound();
        return this;
    }
};

VALIDATE_SIZE(CAEAudioEntity, 0x7C);
