/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColTrianglePlane
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

#include "ColTrianglePlane.h"
#include "ColTriangle.h"

/**
 * Registra la obtención y reconstrucción de planos asociados a triángulos de colisión.
 */
void CColTrianglePlane::InjectHooks()
{
    RH_ScopedClass(CColTrianglePlane);
    RH_ScopedCategory("Collision");

    RH_ScopedOverloadedInstall(GetNormal, "Out", 0x411610, void(CColTrianglePlane::*)(CVector&));
    RH_ScopedInstall(Set, 0x411660);
}

// Resuelve los índices del triángulo sobre el array comprimido y delega en el
// constructor geométrico común.
CColTrianglePlane::CColTrianglePlane(const CColTriangle& tri, const CompressedVector* vertices) :
    CColTrianglePlane{
        vertices[tri.vA],
        vertices[tri.vB],
        vertices[tri.vC]
    }
{
}

// Construye el plano directamente a partir de los tres vértices almacenados.
CColTrianglePlane::CColTrianglePlane(const CStoredCollPoly& poly) :
    CColTrianglePlane{
        poly.verts[0],
        poly.verts[1],
        poly.verts[2]
    }
{
}

/**
 * Calcula la normal unitaria, el desplazamiento del plano y su eje dominante.
 *
 * m_orientation codifica tanto el eje cuyo valor absoluto domina la normal como
 * su signo; esa clasificación permite elegir proyecciones más estables/rápidas
 * en otras rutinas de colisión.
 */
CColTrianglePlane::CColTrianglePlane(const CVector& vA, const CVector& vB, const CVector& vC) {
    const auto normalFull = (vC - vA).Cross(vB - vA).Normalized();

    m_normal       = normalFull;

    // Como el original: el desplazamiento y la orientación se calculan con la normal ya comprimida
    // (0x4117F9: se descomprime m_normal antes del producto escalar y de las comparaciones).
    const CVector normal = m_normal;
    m_normalOffset = vA.Dot(normal);

    m_orientation = [&, this] {
        const auto nx = std::abs(normal.x);
        const auto ny = std::abs(normal.y);
        const auto nz = std::abs(normal.z);

        using enum Orientation;
        if (nx > ny && nx > nz) {
            return normal.x <= 0.0f ? NEG_X : POS_X;
        }
        if (ny > nz) {
            return normal.y <= 0.0f ? NEG_Y : POS_Y;
        }
        return normal.z <= 0.0f ? NEG_Z : POS_Z;
    }();
} 

// Expande la normal comprimida en el vector de salida.
void CColTrianglePlane::GetNormal(CVector& out) {
    out = GetNormal();
}

// Recalcula por completo el plano reutilizando la misma lógica del constructor.
void CColTrianglePlane::Set(const CompressedVector* vertices, const CColTriangle& triangle) {
    // Como el original: solo se escriben los campos; el byte de relleno (+9) queda intacto.
    const CColTrianglePlane plane{ triangle, vertices };
    m_normal       = plane.m_normal;
    m_normalOffset = plane.m_normalOffset;
    m_orientation  = plane.m_orientation;
}
