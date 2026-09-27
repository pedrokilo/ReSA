#pragma once

#include "Vector.h"

/**
 * Triángulo de colisión preparado para generación procedural de vegetación/objetos.
 *
 * Conserva geometría world, esfera de proximidad, semillas y cupos calculados por
 * tipo de planta. Los enlaces permiten a CPlantMgr moverlo entre listas de slots
 * activos y la lista libre sin cambiar su dirección.
 */
class CPlantLocTri {
public:
    CVector m_V1;
    CVector m_V2;
    CVector m_V3;
    CVector m_Center;
    float m_SphereRadius;
    std::array<float, 3> m_Seed;           //!< Semillas estables para distribuir hasta tres tipos de planta.
    std::array<uint16, 3> m_nMaxNumPlants; //!< Máximos derivados de área y densidad de la superficie.
    uint8 m_SurfaceId;
    tColLighting m_nLighting;

    union {
        struct {
            uint8 m_createsPlants : 1;  // 0x1
            uint8 m_createsObjects : 1; // 0x2
            uint8 m_createdObjects : 1; // 0x4
            uint8 m_flagPad : 5;
        };
        uint8_t m_Flags;
    };

    CPlantLocTri* m_NextTri;
    CPlantLocTri* m_PrevTri;

public:
    static void InjectHooks();

    CPlantLocTri* Add(const CVector& p1, const CVector& p2, const CVector& p3, uint8 surface, tColLighting lightning, bool createsPlants, bool createsObjects);
    void Release();
};
VALIDATE_SIZE(CPlantLocTri, 0x54);
