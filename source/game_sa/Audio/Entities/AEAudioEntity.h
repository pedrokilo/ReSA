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
    // 0x5DE9E0 y, copia byte a byte, 0x4A9F60 (lote 148A, banco Fn148A; solo la alcanzan funclets de desenrollado de excepciones: 0x4A9F60 la llama el de 0x839928)
    CAEAudioEntity* Destructor() {
        // Como el original: restaura la vtable base del EXE (0x859988) y destruye m_tempSound con CAESound::Destructor (0x4EF660, con hook; envoltura sin expansion en linea: los bancos sustituyen su cuerpo).
        *reinterpret_cast<uintptr_t*>(this) = 0x859988;
        m_tempSound.Destructor();
        return this;
    }
};

VALIDATE_SIZE(CAEAudioEntity, 0x7C);
