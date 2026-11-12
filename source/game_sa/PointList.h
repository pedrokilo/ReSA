/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CPointList {
public:
    static void InjectHooks();

    bool Empty() const noexcept;
    void AddPoint(CVector pt) noexcept;
    void MergeListsRemovingDoubles(CPointList* main, CPointList* toBeMerged) noexcept;

public:
    uint32                  m_Count{};
    std::array<CVector, 24> m_Points{};
    bool                    m_PointHasBeenClaimed[24]{};

private:
    CPointList* Constructor(); // 0x6986E0 (lote 142A, banco Fn142A)
    void Clear();              // 0x699F00 (lote 173A, banco Fn173A)
};
VALIDATE_SIZE(CPointList, 0x13C);
