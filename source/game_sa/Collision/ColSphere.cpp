/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColSphere
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ColSphere.h"

/**
 * Registra las pruebas geométricas de CColSphere.
 */
void CColSphere::InjectHooks() {
    RH_ScopedClass(CColSphere);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40FD10);
    RH_ScopedInstall(IntersectRay, 0x40FF20);
    RH_ScopedInstall(IntersectEdge, 0x4100E0);
    RH_ScopedInstall(IntersectPoint, 0x410040);
    RH_ScopedInstall(IntersectSphere, 0x410090);
}

// 0x40FD10
// Actualiza conjuntamente geometría y metadatos de superficie de la esfera.
void CColSphere::Set(float radius, const CVector& center, eSurfaceType material, uint8 pieceType, tColLighting lighting) {
    m_fRadius = radius;
    m_vecCenter = center;
    m_Surface.m_nMaterial = material;
    m_Surface.m_nPiece = pieceType;
    m_Surface.m_nLighting = lighting;
}

// 0x40FF20
/**
 * Interseca una recta parametrizada con la esfera resolviendo la ecuación cuadrática.
 *
 * @param direction Se asume con la escala apropiada para la parametrización usada.
 * @return true cuando existen soluciones reales; ambos puntos de corte se devuelven.
 */
bool CColSphere::IntersectRay(const CVector& rayOrigin, const CVector& direction, CVector& intersectPoint1, CVector& intersectPoint2) {
    CVector distance = rayOrigin - m_vecCenter;
    float b = 2.0f * DotProduct(direction, distance);
    float c = DotProduct(distance, distance) - m_fRadius * m_fRadius;
    float t0 = 0.0f, t1 = 0.0f;
    if (CGeneral::SolveQuadratic(1.0f, b, c, t0, t1)) {
        intersectPoint1 = (t0 * direction) + rayOrigin;
        intersectPoint2 = (t1 * direction) + rayOrigin;
        return true;
    }
    return false;
}

// 0x4100E0
/**
 * Restringe la intersección esfera/rayo al segmento [startPoint, endPoint].
 *
 * Los parámetros de entrada/salida se recortan a los extremos del segmento cuando
 * uno de los cortes matemáticos queda fuera de su longitud finita.
 */
bool CColSphere::IntersectEdge(const CVector& startPoint, const CVector& endPoint, CVector& intersectPoint1, CVector& intersectPoint2) {
    CVector originCenterDistance = startPoint - m_vecCenter;
    CVector rayDirection = endPoint - startPoint;
    float rayLength = rayDirection.Magnitude();
    rayDirection.Normalise();
    float a = DotProduct(rayDirection, rayDirection);
    float b = 2.0f * DotProduct(originCenterDistance, rayDirection);
    float c = DotProduct(originCenterDistance, originCenterDistance) - m_fRadius * m_fRadius;
    float discriminant = b * b - 4.0f * a * c; // discriminante = b^2 - 4ac
    // discriminante == 0: hay un punto tangente; > 0: hay dos cortes.
    if (discriminant < 0.0f) {
        return false;
    }

    float discriminantSquareRoot = sqrt(discriminant);
    float numerator1 = (-b - discriminantSquareRoot) * 0.5f;
    float numerator2 = (discriminantSquareRoot - b) * 0.5f;
    if (numerator1 > rayLength || numerator2 < 0.0f) {
        return false;
    }

    intersectPoint2 = endPoint;
    if (numerator2 < rayLength) {
        intersectPoint2 = (rayDirection * numerator2) + startPoint;
    }

    intersectPoint1 = startPoint;
    if (numerator1 > 0.0f) {
        intersectPoint1 = (rayDirection * numerator1) + startPoint;
    }
    return true;
}

// 0x410040
// Utiliza distancia al cuadrado y una comparación estricta: el borde exacto no cuenta.
bool CColSphere::IntersectPoint(const CVector& point) {
    CVector distance = m_vecCenter - point;
    return m_fRadius * m_fRadius > distance.SquaredMagnitude();
}

// 0x410090
// Dos esferas se solapan cuando la distancia entre centros es menor que la suma de radios.
bool CColSphere::IntersectSphere(const CColSphere& right) const {
    CVector distance = m_vecCenter - right.m_vecCenter;
    return std::powf(m_fRadius + right.m_fRadius, 2.0f) > distance.SquaredMagnitude();
}

// Conserva los metadatos de superficie mientras transforma la esfera geométrica.
auto TransformObject(const CColSphere& sp, const CMatrix& transform) -> CColSphere {
    return CColSphere{
        TransformObject(static_cast<const CSphere&>(sp), transform),
        sp.m_Surface
    };
}
