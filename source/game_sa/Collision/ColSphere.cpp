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
 * Validación registrada: 5/5 entradas.
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
    // Como el original: d = origen - centro; b = (dx*Dx + dz*Dz + dy*Dy) * 2 y c = (dz*dz + dx*dx + dy*dy) - r*r, en ese orden.
    const float dx = rayOrigin.x - m_vecCenter.x;
    const float dy = rayOrigin.y - m_vecCenter.y;
    const float dz = rayOrigin.z - m_vecCenter.z;
    const float b = (dx * direction.x + dz * direction.z + dy * direction.y) * 2.0f;
    const float c = dz * dz + dx * dx + dy * dy - m_fRadius * m_fRadius;
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
    // Como el original: todas las sumas siguen el orden z, x, y (o z, y, x para la longitud) del EXE.
    const CVector originCenterDistance = startPoint - m_vecCenter;
    CVector rayDirection = endPoint - startPoint;
    const float rayLength = std::sqrt(rayDirection.z * rayDirection.z + rayDirection.y * rayDirection.y + rayDirection.x * rayDirection.x);
    rayDirection.Normalise();
    const float b = (rayDirection.z * originCenterDistance.z + rayDirection.x * originCenterDistance.x + rayDirection.y * originCenterDistance.y) * 2.0f;
    const float c = originCenterDistance.z * originCenterDistance.z + originCenterDistance.x * originCenterDistance.x + originCenterDistance.y * originCenterDistance.y - m_fRadius * m_fRadius;
    // Como el original: el termino a (|dir|^2 tras normalizar) no se usa: b^2 - c*4.
    const float discriminant = b * b - c * 4.0f;
    // discriminante == 0: hay un punto tangente; > 0: hay dos cortes.
    if (discriminant < 0.0f) {
        return false;
    }

    const float discriminantSquareRoot = std::sqrt(discriminant);
    const float numerator2 = (discriminantSquareRoot - b) * 0.5f;
    const float numerator1 = (-b - discriminantSquareRoot) * 0.5f;
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
    // Como el original: d = centro - punto y r*r > (dz*dz + dx*dx + dy*dy), estricto.
    const CVector distance = m_vecCenter - point;
    return m_fRadius * m_fRadius > distance.z * distance.z + distance.x * distance.x + distance.y * distance.y;
}

// 0x410090
// Dos esferas se solapan cuando la distancia entre centros es menor que la suma de radios.
bool CColSphere::IntersectSphere(const CColSphere& right) const {
    // Como el original: (r2 + r1)^2 > (dz*dz + dx*dx + dy*dy), estricto.
    const CVector distance = m_vecCenter - right.m_vecCenter;
    const float radii = right.m_fRadius + m_fRadius;
    return radii * radii > distance.z * distance.z + distance.x * distance.x + distance.y * distance.y;
}

// Conserva los metadatos de superficie mientras transforma la esfera geométrica.
auto TransformObject(const CColSphere& sp, const CMatrix& transform) -> CColSphere {
    return CColSphere{
        TransformObject(static_cast<const CSphere&>(sp), transform),
        sp.m_Surface
    };
}
