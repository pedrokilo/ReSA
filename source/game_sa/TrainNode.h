/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CTrainNode {
public:
    int16  x;
    int16  y;
    int16  z;
    uint16 m_nDistanceFromStart;
    tColLighting m_nSurfaceLighting;
    bool m_bSurfLightingFound;

public:
    static void InjectHooks();

    CTrainNode();

    void SetX(float X);
    void SetY(float Y);
    void SetZ(float Z);
    [[nodiscard]] float GetX() const;
    [[nodiscard]] float GetY() const;
    [[nodiscard]] float GetZ() const;
    CVector GetPosn() const;
    void SetDistanceFromStart(float dist);
    [[nodiscard]] float GetDistanceFromStart() const;
    tColLighting GetLightingFromCollision();

private:
    // Envoltorio con la ABI del EXE (0x6F5F80 devuelve el byte en AL; tColLighting tiene constructor y MSVC lo devuelve por puntero oculto): es el destino del hook.
    uint8 GetLightingFromCollisionExeAbi();
};
VALIDATE_SIZE(CTrainNode, 0xA);
