/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "ColSurface.h"

class CColDisk : public CColSphere {
public:
    CVector     m_vThickness{};
    float       m_fThickness{};

public:
    static void InjectHooks();

    // 0x40FD50 (lote 153A, banco Fn153A): extremo inicial (centro y radio de CColSphere), extremo final (m_vThickness y m_fThickness), material, pieza e iluminacion en una sola operacion.
    void Set(float startRadius, const CVector& start, const CVector& end, float endRadius, eSurfaceType material, uint8 pieceType, tColLighting lighting);
};
VALIDATE_SIZE(CColDisk, 0x24);
