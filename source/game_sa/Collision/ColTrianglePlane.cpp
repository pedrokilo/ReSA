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
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ColTrianglePlane.h"
#include "ColTriangle.h"

void CColTrianglePlane::InjectHooks()
{
    RH_ScopedClass(CColTrianglePlane);
    RH_ScopedCategory("Collision");

    RH_ScopedOverloadedInstall(GetNormal, "Out", 0x411610, void(CColTrianglePlane::*)(CVector&));
    RH_ScopedInstall(Set, 0x411660);
}

CColTrianglePlane::CColTrianglePlane(const CColTriangle& tri, const CompressedVector* vertices) :
    CColTrianglePlane{
        vertices[tri.vA],
        vertices[tri.vB],
        vertices[tri.vC]
    }
{
}

CColTrianglePlane::CColTrianglePlane(const CStoredCollPoly& poly) :
    CColTrianglePlane{
        poly.verts[0],
        poly.verts[1],
        poly.verts[2]
    }
{
}

CColTrianglePlane::CColTrianglePlane(const CVector& vA, const CVector& vB, const CVector& vC) {
    const auto normal = (vC - vA).Cross(vB - vA).Normalized();

    m_normal       = normal;
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

void CColTrianglePlane::GetNormal(CVector& out) {
    out = GetNormal();
}

void CColTrianglePlane::Set(const CompressedVector* vertices, const CColTriangle& triangle) {
    *this = CColTrianglePlane{ triangle, vertices };
}
