#include "StdInc.h"

#include "ColBox.h"

void CColBox::InjectHooks() {
    RH_ScopedClass(CColBox);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40EE70);                                          // Lote 116A, banco Fn116A
    RH_ScopedInstall(operator=, 0x40EEC0);                                    // Lote 116A
}

// 0x40EEC0
// Like the original: copies min, max, material + piece (one word) and lighting; m_nLight (0x1B) is not copied
CColBox& CColBox::operator=(const CColBox& right) {
    m_vecMin = right.m_vecMin;
    m_vecMax = right.m_vecMax;
    // Como el original (0x40EEC0..0x40EF06), lectura y escritura en este orden y con estos anchos: la palabra +0x18 (material y pieza) y despues el byte +0x1A (iluminacion). Con el origen solapado
    // (por ejemplo this - 1) el byte se lee despues de haber escrito la palabra, no antes.
    const auto* const source = reinterpret_cast<const volatile uint8*>(&right);
    auto* const       target = reinterpret_cast<volatile uint8*>(this);
    *reinterpret_cast<volatile uint16*>(target + 0x18) = *reinterpret_cast<const volatile uint16*>(source + 0x18);
    *(target + 0x1A) = *(source + 0x1A);
    return *this;
}

// 0x40EE70
void CColBox::Set(const CVector& sup, const CVector& inf, eSurfaceType material, uint8 pieceType, tColLighting lighting) {
    m_vecMin = sup;
    m_vecMax = inf;
    m_Surface.m_nMaterial = material;
    m_Surface.m_nPiece = pieceType;
    m_Surface.m_nLighting = lighting;
}
