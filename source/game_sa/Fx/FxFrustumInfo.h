/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "FxSphere.h"
#include "FxPlane.h"

class FxFrustumInfo_c {
public:
    FxSphere_c m_Sphere;
    std::array<FxPlane_c, 4> m_Planes;

    static void InjectHooks();

    // 0x4AA030: el EXE devuelve EAX = 0/1, deja EDX = la esfera y conserva ECX = this (sus llamadores lo usan).
    // IsCollisionRegs deja esos registros con ResidueRegsCpp; IsCollision es la forma normal para el resto del codigo.
    uint32 IsCollisionRegs(FxSphere_c& sphere);
    bool   IsCollision(FxSphere_c& sphere) { return IsCollisionRegs(sphere) != 0; }
};
VALIDATE_SIZE(FxFrustumInfo_c, 0x54);
