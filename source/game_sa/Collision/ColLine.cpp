/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColLine
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ColLine.h"
#include "Lines.h"

void CColLine::InjectHooks() {
    RH_ScopedClass(CColLine);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40EF10);
}

// 0x40EF50
CColLine::CColLine(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// 0x40EF10
void CColLine::Set(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// NOTSA
float CColLine::DistTo(CVector pt) const {
    return CCollision::DistToLine(m_vecStart, m_vecEnd, pt);
}

// NOTSA
void CColLine::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    const auto colorARGB = color.ToIntARGB();
    CLines::RenderLineNoClipping(transform.TransformPoint(m_vecStart), transform.TransformPoint(m_vecEnd), colorARGB, colorARGB);
}
