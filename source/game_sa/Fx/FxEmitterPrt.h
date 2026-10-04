#pragma once

#include "ListItem_c.h"
#include "Vector.h"
#include "Particle.h"
#include "extensions/FixedFloat.hpp"

struct RwRGBA;
class FxSystem_c;

/**
 * Partícula concreta emitida por un FxEmitter.
 *
 * Extiende Particle_c con multiplicadores visuales compactos, variación aleatoria y
 * estado de rotación; las colecciones se reservan desde FxMemoryPool_c.
 */
// Como el original: sólo hereda de Particle_c (enlaces en 0x0/0x4); m_MultColor en 0x2C y tamaño 0x3C
// (FxManager_c::Init 0x4A992D reserva 1000 * 0x3C + 4 bytes y FxEmitter_c::CreateParticle escribe en 0x2C..0x38).
class FxEmitterPrt_c : public Particle_c {
public:
    CRGBA m_MultColor;
    FixedFloat<uint8, 255.0f> m_MultSize;
    FixedFloat<uint8, 255.0f> m_MultRot;
    uint8 m_RandR;
    uint8 m_RandG;
    uint8 m_RandB;
    FixedFloat<uint8, 100.0f> m_Brightness;
    int8  m_RotZ;
    bool  m_bLocalToSystem;
    float m_CurrentRotation;

public:
    FxEmitterPrt_c() = default;  // 0x4A94E0
    ~FxEmitterPrt_c() = default; // 0x4A94F0

    static void InjectHooks();

    static void* operator new[](size_t size);
};
VALIDATE_SIZE(FxEmitterPrt_c, 0x3C);
