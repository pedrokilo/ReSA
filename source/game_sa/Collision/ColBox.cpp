/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColBox
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

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
