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
    int8 m_Speed{}; //!< Índice cíclico dentro de gfExplosionFrequencyVariations (un byte con signo en el original).

public:
    static void InjectHooks();

    CAEExplosionAudioEntity();
    ~CAEExplosionAudioEntity() = default;

    static void StaticInitialise();

    // 0x5074B0 (lote 144A, banco Fn144A): constructor del EXE (vtable del EXE, sin llamar al de la base ni tocar m_Entity); devuelve this
    CAEExplosionAudioEntity* Constructor();

    void AddAudioEvent(eAudioEvents audioEvent, CVector& posn, float volume);
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
};

VALIDATE_SIZE(CAEExplosionAudioEntity, 0x80);
