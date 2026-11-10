/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColDisk
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ColDisk.h"


namespace {
// Copia tres dwords en orden, leyendo y escribiendo cada uno una sola vez como enteros (como los `mov` del EXE: no quita la senal de un NaN y, si origen y destino se solapan, cada lectura ve las escrituras anteriores).
inline void CopyThreeDwords(volatile uint32* dst, const volatile uint32* src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}
} // namespace

void CColDisk::InjectHooks() {
    RH_ScopedClass(CColDisk);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40FD50); // lote 153A (banco Fn153A)
}

// 0x40FD50 (lote 153A, banco Fn153A; thiscall, siete dwords en la pila, `ret 1Ch`, 85 bytes): en este orden, como el EXE: radio inicial en +0xC; copia dword a dword del vector inicial (+0..8) y del final (+0x14..0x1C);
// radio final en +0x20 y los tres bytes de superficie (+0x10 material, +0x11 pieza, +0x12 iluminacion; el byte +0x13 no se toca). Los vectores y los radios se copian como enteros (conservan los bits de un NaN).
void CColDisk::Set(float startRadius, const CVector& start, const CVector& end, float endRadius, eSurfaceType material, uint8 pieceType, tColLighting lighting) {
    *reinterpret_cast<volatile uint32*>(&m_fRadius) = std::bit_cast<uint32>(startRadius);
    CopyThreeDwords(reinterpret_cast<volatile uint32*>(&m_vecCenter), reinterpret_cast<const volatile uint32*>(&start));
    CopyThreeDwords(reinterpret_cast<volatile uint32*>(&m_vThickness), reinterpret_cast<const volatile uint32*>(&end));
    *reinterpret_cast<volatile uint32*>(&m_fThickness) = std::bit_cast<uint32>(endRadius);
    m_Surface.m_nMaterial = material;
    m_Surface.m_nPiece = pieceType;
    m_Surface.m_nLighting = lighting;
}
