/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Box.h"

class CBoundingBox : public CBox {
public:
    constexpr CBoundingBox() : CBox(CVector{1.0f}, CVector{-1.0f}) {}
    constexpr CBoundingBox(CVector min, CVector max) : CBox(min, max) {}
    constexpr explicit CBoundingBox(const CBox& box) : CBox(box) {}

    static void InjectHooks();

    CBoundingBox* Constructor(); // 0x40FAF0 (lote 130A, banco Fn130A): constructor por defecto del EXE (min = (1,1,1), max = (-1,-1,-1)), thiscall sin argumentos, devuelve this

    [[nodiscard]] bool IsPointWithin(const CVector& point) const;
    inline void SetMinMax(CVector min, CVector max);
};

VALIDATE_SIZE(CBoundingBox, 0x18);

// in entity.cpp
static auto& numBBFailed = StaticRef<int32>(0xB71808); // 0xB71808
