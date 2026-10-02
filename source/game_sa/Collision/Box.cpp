/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBox
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Lines.h"

/**
 * Registra las operaciones básicas de la caja de colisión alineada con los ejes.
 */
void CBox::InjectHooks()
{
    RH_ScopedClass(CBox);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40EDE0);
    RH_ScopedInstall(Recalc, 0x40EE20);
}

// Sustituye ambos extremos sin corregir su orden; Recalc() puede normalizarlos después.
void CBox::Set(const CVector& vecMin, const CVector& vecMax)
{
    m_vecMin = vecMin;
    m_vecMax = vecMax;
}

/**
 * Normaliza la caja para garantizar que m_vecMin <= m_vecMax en cada eje.
 *
 * Esto permite que las pruebas de inclusión y las dimensiones funcionen incluso
 * cuando los puntos extremos se proporcionaron en orden inverso.
 */
void CBox::Recalc()
{
    if (m_vecMin.x > m_vecMax.x)
        std::swap(m_vecMin.x, m_vecMax.x);

    if (m_vecMin.y > m_vecMax.y)
        std::swap(m_vecMin.y, m_vecMax.y);

    if (m_vecMin.z > m_vecMax.z)
        std::swap(m_vecMin.z, m_vecMax.z);
}

// NOTSA
// Construye las ocho esquinas, aplica la transformación y dibuja las doce aristas
// sin clipping. Es una ayuda de depuración/render, no modifica la geometría de colisión.
// TODO(OPT): Refactor code (meaningful names, etc) and possibly use std::optional for `CMatrix` (In cases where a unity matrix would be used otherwise)
void CBox::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    auto workVec = m_vecMin;
    CVector v1 = transform.TransformPoint(workVec);

    workVec.z = m_vecMax.z;
    CVector v2 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.x = m_vecMax.x;
    CVector v3 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.y = m_vecMax.y;
    CVector v4 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.y = m_vecMax.y;
    workVec.z = m_vecMax.z;
    CVector v5 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.x = m_vecMax.x;
    workVec.z = m_vecMax.z;
    CVector v6 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.x = m_vecMax.x;
    workVec.y = m_vecMax.y;
    CVector v7 = transform.TransformPoint(workVec);

    workVec = m_vecMax;
    CVector v8 = transform.TransformPoint(workVec);

    const auto colorRGBA = color.ToInt();
    CLines::RenderLineNoClipping(v1, v2, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v1, v3, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v1, v4, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v5, v2, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v5, v8, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v5, v4, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v6, v2, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v6, v8, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v6, v3, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v7, v8, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v7, v3, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v7, v4, colorRGBA, colorRGBA);
}

// Comprueba inclusión usando límites cerrados en X, Y y Z.
bool CBox::IsPointInside(const CVector& point) const {
    return point.x >= m_vecMin.x && point.x <= m_vecMax.x
        && point.y >= m_vecMin.y && point.y <= m_vecMax.y
        && point.z >= m_vecMin.z && point.z <= m_vecMax.z;
}

// Expande sólo los ejes necesarios para que pt quede dentro de la caja.
void CBox::StretchToPoint(const CVector& pt) {
    for (size_t i = 0; i < 3; i++) {
        m_vecMin[i] = std::min(m_vecMin[i], pt[i]);
        m_vecMax[i] = std::max(m_vecMax[i], pt[i]);
    }
}

/**
 * Calcula por eje la separación mínima entre un punto y la caja.
 *
 * Un componente vale 0 cuando la coordenada ya cae dentro del intervalo de ese eje;
 * por tanto, el vector completo es cero para cualquier punto interior.
 */
CVector CBox::GetShortestVectorDistToPt(const CVector& pt) const {
    const auto CalculateAxis = [&](int32 i) {
        return std::max({
            m_vecMin[i] - pt[i],
            pt[i] - m_vecMax[i],
            0.0f
        });
    };
    return {
        CalculateAxis(0),
        CalculateAxis(1),
        CalculateAxis(2)
    };
}
