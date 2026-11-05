/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColLine
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

#include "ColLine.h"
#include "Lines.h"

/**
 * Registra la operación de inicialización de segmentos de colisión.
 */
void CColLine::InjectHooks() {
    RH_ScopedClass(CColLine);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40EF10);
    RH_ScopedInstall(Constructor, 0x40EF50);
}

// 0x40EF50
// Inicializa los extremos geométricos del segmento.
// Como el original: copia los tres dwords de cada extremo como enteros (un NaN senalizante conserva su carga) y no toca los campos de tamano (+0xC y +0x1C).
CColLine::CColLine(const CVector& start, const CVector& end) {
    std::memcpy(&m_vecStart, &start, sizeof(CVector));
    std::memcpy(&m_vecEnd, &end, sizeof(CVector));
}

// 0x40EF10
// Reutiliza la instancia sustituyendo ambos extremos del segmento.
void CColLine::Set(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// NOTSA
// Delega el cálculo geométrico común a CCollision::DistToLine().
float CColLine::DistTo(CVector pt) const {
    return CCollision::DistToLine(m_vecStart, m_vecEnd, pt);
}

// NOTSA
// Dibuja el segmento transformado sin clipping como ayuda de depuración.
void CColLine::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    const auto colorARGB = color.ToIntARGB();
    CLines::RenderLineNoClipping(transform.TransformPoint(m_vecStart), transform.TransformPoint(m_vecEnd), colorARGB, colorARGB);
}
