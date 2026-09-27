/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"

/**
 * Entidad global que sintetiza explosiones mediante varias CAESound superpuestas.
 */
class NOTSA_EXPORT_VTABLE CAEExplosionAudioEntity : public CAEAudioEntity {
public:
    uint32 m_Speed{}; //!< Índice cíclico dentro de gfExplosionFrequencyVariations.

public:
    static void InjectHooks();

    CAEExplosionAudioEntity();
    ~CAEExplosionAudioEntity() = default;

    static void StaticInitialise();

    void AddAudioEvent(eAudioEvents audioEvent, CVector& posn, float volume);
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
};

VALIDATE_SIZE(CAEExplosionAudioEntity, 0x80);
