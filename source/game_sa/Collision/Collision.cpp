/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCollision
 * Reconstrucción: COMPLETA — 59/59 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 59/59 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/


#include "StdInc.h"

#include <numbers>

#include "Collision.h"
#include "ColHelpers.h"
#include "PedModelInfo.h"
#include "TaskSimpleHoldEntity.h"

#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"

#define NOTSA_VANILLA_COLLISIONS // TODO: move to config.h?

using Shape = CCollision::DebugSettings::ShapeShapeCollision::Shape;

/*!
* @addr 0x416260
*/
void CCollision::Init() {
    ZoneScoped;

    ms_colModelCache.Init(50);
    ms_collisionInMemory = 0;
    CColStore::Initialise();
}

// 0x4162E0
void CCollision::Shutdown() {
    // Como el original: recorre la lista USADA desde la cola (usedListTail.prev, 0x96593C) hacia la cabeza y en cada
    // enlace con datos llama a CCollisionData::RemoveTrianglePlanes (0x40F6A0, miembro), sin recolocar el enlace.
    for (auto i = ms_colModelCache.usedListTail.prev; i != &ms_colModelCache.usedListHead; i = i->prev) {
        if (i->data) {
            i->data->RemoveTrianglePlanes();
        }
    }
    ms_colModelCache.Shutdown();
    CColStore::Shutdown();
}

// 0x411E20
void CCollision::Update() {
    // empty
}

// 0x411E30
void CCollision::SortOutCollisionAfterLoad() {
    CColStore::LoadCollision(TheCamera.m_mCameraMatrix.GetPosition(), false);
    CStreaming::LoadAllRequestedModels(false);
}


// 0x416330
void CCollision::CalculateTrianglePlanes(CCollisionData* colData) {
    ZoneScoped;

    if (!colData->m_nNumTriangles) { // No triangles => no planes to calculate
        return;
    }

    if (colData->m_pTrianglePlanes) { // Planes already calculated?
        ms_colModelCache.Insert(*colData->GetLinkPtr()); // Re-insert link at front
    } else {
        auto l = ms_colModelCache.Insert(colData);
        if (!l) { // No more free space?
            // Remove least-recently used item
            const auto llr = ms_colModelCache.GetTail();
            llr->data->RemoveTrianglePlanes();
            ms_colModelCache.Remove(llr);

            // This should succeed now
            VERIFY(l = ms_colModelCache.Insert(colData));
        }
        colData->CalculateTrianglePlanes();
        colData->SetLinkPtr(l);
    }
}

// 0x416400
void CCollision::RemoveTrianglePlanes(CCollisionData* colData) {
    ZoneScoped;

    if (!colData->m_pTrianglePlanes) {
        return;
    }

    const auto l = colData->GetLinkPtr();
    ms_colModelCache.Remove(l);
    colData->RemoveTrianglePlanes();
}

// 0x411E70
bool CCollision::TestSphereSphere(CColSphere const& sphere1, CColSphere const& sphere2) { // Yes, it's __stdcall
    ZoneScoped;

    // Como el original (0x411E70, cdecl): d2 = (dz^2 + dx^2) + dy^2 y cierto solo si (r1+r2)^2 > d2 (estricto; NaN da falso)
    const float dx = sphere1.m_vecCenter.x - sphere2.m_vecCenter.x;
    const float dy = sphere1.m_vecCenter.y - sphere2.m_vecCenter.y;
    const float dz = sphere1.m_vecCenter.z - sphere2.m_vecCenter.z;
    const float rr = sphere1.m_fRadius + sphere2.m_fRadius;
    const float d2 = (dz * dz + dx * dx) + dy * dy;
    return rr * rr > d2;
}

// 0x411EC0
void CalculateColPointInsideBox(CBox const& box, CVector const& point, CColPoint& colPoint) {
    // Como el original (0x411EC0): el centro es (max + min) * 0.5 y el EXE solo escribe punto, normal (3 floats) y profundidad
    // (el resto del CColPoint queda intacto). NaN en las comparaciones de signo: el EXE usa `test ah,41h`, o sea `!(a > 0)`.
    const float cx = (box.m_vecMax.x + box.m_vecMin.x) * 0.5f;
    const float cy = (box.m_vecMax.y + box.m_vecMin.y) * 0.5f;
    const float cz = (box.m_vecMax.z + box.m_vecMin.z) * 0.5f;

    const float px = point.x - cx;
    const float py = point.y - cy;
    const float pz = point.z - cz;

    // Distancia del punto a la cara mas cercana de cada eje
    const float dx = !(px > 0.f) ? point.x - box.m_vecMin.x : box.m_vecMax.x - point.x;
    const float dy = !(py > 0.f) ? point.y - box.m_vecMin.y : box.m_vecMax.y - point.y;
    const float dz = !(pz > 0.f) ? point.z - box.m_vecMin.z : box.m_vecMax.z - point.z;

    colPoint.m_vecPoint = point;

    if (dx < dy && dx < dz) {
        colPoint.m_vecNormal = CVector{ px > 0.f ? 1.f : -1.f, 0.f, 0.f };
        colPoint.m_fDepth    = dx;
    } else if (dy < dx && dy < dz) {
        colPoint.m_vecNormal = CVector{ 0.f, py > 0.f ? 1.f : -1.f, 0.f };
        colPoint.m_fDepth    = dy;
    } else {
        colPoint.m_vecNormal = CVector{ 0.f, 0.f, pz > 0.f ? 1.f : -1.f };
        colPoint.m_fDepth    = dz;
    }
}

/*!
* @addr 0x4120C0
* @brief Tests if the \a bb is fully inside \a sphere
*/
bool CCollision::TestSphereBox(CSphere const& sphere, CBox const& box) {
    ZoneScoped;

    for (auto i = 0u; i < 3u; i++) {
        if (sphere.m_vecCenter[i] + sphere.m_fRadius < box.m_vecMin[i] ||
            sphere.m_vecCenter[i] - sphere.m_fRadius > box.m_vecMax[i]
        ) {
            return false;
        }
    }
    return true;
}

/*!
* @addr 0x412130
*/
bool CCollision::ProcessSphereBox(CColSphere const& sph, CColBox const& box, CColPoint & colp, float& minDistSq) {
    ZoneScoped;

	// GTA's code is too complicated, uses a huge 3x3x3 if statement
	// we can simplify the structure a lot
    // Some of the original code, to give you an idea:
    /*
    if (sphere.m_vecCenter.x + sphere.m_fRadius < bb.m_vecMin.x)
        return false;

    if (sphere.m_vecCenter.x + sphere.m_fRadius > bb.m_vecMax.x)
        return false;

    CVector colPos{};

    if (sphere.m_vecCenter.x >= bb.m_vecMin.x) {
        if (sphere.m_vecCenter.x <= bb.m_vecMax.x) {
            if (sphere.m_vecCenter.y + sphere.m_fRadius > bb.m_vecMin.y)
                return false;

            if (sphere.m_vecCenter.y - sphere.m_fRadius > bb.m_vecMax.y)
                return false;

            if (sphere.m_vecCenter.y >= bb.m_vecMin.y) {
                if (sphere.m_vecCenter.y <= bb.m_vecMax.y) {
                    if (sphere.m_vecCenter.z + sphere.m_fRadius > bb.m_vecMin.z)
                        return false;

                    if (sphere.m_vecCenter.z - sphere.m_fRadius > bb.m_vecMax.z)
                        return false;

                    if (sphere.m_vecCenter.z >= bb.m_vecMin.z) {
                        if (sphere.m_vecCenter.z <= bb.m_vecMax.z) {
                            CColPoint boxCP{};
                            CalculateColPointInsideBox(bb, sphere.m_vecCenter, boxCP);

                            diskColPoint.m_vecPoint = boxCP.m_vecPoint - boxCP.m_vecNormal * sphere.m_fRadius;
                            diskColPoint.m_fDepth = boxCP.m_fDepth;

                            diskColPoint.m_nLightingA = sphere.ligthing;
                            diskColPoint.m_nSurfaceTypeA = sphere.m_nMaterial;

                            diskColPoint.m_nLightingB = bb.ligthing;
                            diskColPoint.m_nSurfaceTypeB = bb.m_nMaterial;

                            maxTouchDistance = 0.f;

                            return true;
                        } else {
                            colPos.x = sphere.m_vecCenter.x;
                            colPos.y = sphere.m_vecCenter.y;
                            colPos.z = bb.m_vecMax.z;
                        }
                    } else {
                        colPos.x = sphere.m_vecCenter.x;
                        colPos.y = sphere.m_vecCenter.y;
                        colPos.z = bb.m_vecMin.z;
                    }
                } else {
                    if (sphere.m_vecCenter.z + sphere.m_fRadius > bb.m_vecMin.z)
                        return false;

                    if (sphere.m_vecCenter.z - sphere.m_fRadius > bb.m_vecMax.z)
                        return false;

                    if (sphere.m_vecCenter.z >= bb.m_vecMin.z) {
                        if (sphere.m_vecCenter.z <= bb.m_vecMax.z) {
                            colPos.x = sphere.m_vecCenter.x;
                            colPos.y = bb.m_vecMax.y;

                            if (sphere.m_vecCenter.z > bb.m_vecMax.z)
                                colPos.y = bb.m_vecMax.y;

                            if (sphere.m_vecCenter.z <= bb.m_vecMax.z) {
                                colPos.z = sphere.m_vecCenter.z;
                                colPos.y = bb.m_vecMax.y;
                            } else {
                                colPos.z = bb.m_vecMax.z;
                            }
                        } else {

                        }
                    } else {
                        colPos.y = bb.m_vecMax.y;
                        colPos.x = sphere.m_vecCenter.x;
                        colPos.z = bb.m_vecMin.z;
                    }
                }
            } else {
                if (sphere.m_vecCenter.z + sphere.m_fRadius > bb.m_vecMin.z)
                    return false;

                if (sphere.m_vecCenter.z - sphere.m_fRadius > bb.m_vecMax.z)
                    return false;

                if (sphere.m_vecCenter.z)
            }
        }
    }
    */

	// First make sure we have a collision at all
    if (!TestSphereBox(sph, box))
        return false;

	// Now find out where the sphere center lies in relation to all the sides
    enum class ClosestCorner {
        INSIDE,
        MIN,    
        MAX,
    };
    using enum ClosestCorner;

    ClosestCorner axies[3];
    for (auto i = 0; i < 3; i++) {
        axies[i] = sph.m_vecCenter[i] < box.m_vecMin[i] ? MIN :
                   sph.m_vecCenter[i] > box.m_vecMax[i] ? MAX :
                   INSIDE;
    }

	if(axies[0] == INSIDE && axies[1] == INSIDE && axies[2] == INSIDE) { // Sphere center is inside the bb
        CColPoint boxCP{};
        CalculateColPointInsideBox(box, sph.m_vecCenter, boxCP);

        // Original (0x412130): face-normal from CalculateColPointInsideBox, depth includes radius
        colp.m_vecNormal     = boxCP.m_vecNormal;
        colp.m_vecPoint      = sph.m_vecCenter - boxCP.m_vecNormal * sph.m_fRadius;
        colp.m_fDepth        = boxCP.m_fDepth + sph.m_fRadius;

        // Como el original: copia material, pieza e iluminacion (3 bytes) de cada superficie
        colp.m_nSurfaceTypeA = sph.m_Surface.m_nMaterial;
        colp.m_nPieceTypeA   = sph.m_Surface.m_nPiece;
        colp.m_nLightingA    = sph.m_Surface.m_nLighting;
        colp.m_nSurfaceTypeB = box.m_Surface.m_nMaterial;
        colp.m_nPieceTypeB   = box.m_Surface.m_nPiece;
        colp.m_nLightingB    = box.m_Surface.m_nLighting;

        minDistSq            = 0.f; // Original sets it to 0 for inside hits
        return true;
    } else { // Sphere centre is outside on at least one axis

        // Position of closest corner:
        const CVector p{
            axies[0] == MIN ? box.m_vecMin.x : axies[0] == MAX ? box.m_vecMax.x
                                                               : sph.m_vecCenter.x,

            axies[1] == MIN ? box.m_vecMin.y : axies[1] == MAX ? box.m_vecMax.y
                                                               : sph.m_vecCenter.y,

            axies[2] == MIN ? box.m_vecMin.z : axies[2] == MAX ? box.m_vecMax.z
                                                               : sph.m_vecCenter.z,
        };

        const auto dir    = sph.m_vecCenter - p;
        const auto distSq = dir.SquaredMagnitude();
        if (distSq < minDistSq) {
            const auto dist = std::sqrt(distSq);
            // Como el original (0x4125E4): solo descarta si dist > radio o NaN (con dist == radio sigue)
            if (!(dist <= sph.m_fRadius)) {
                return false;
            }

            // Como el original (0x4119D0): operator/ multiplica por el inverso (1.0f / dist)
            const auto invDist   = 1.0f / dist;
            colp.m_vecNormal     = CVector{ dir.x * invDist, dir.y * invDist, dir.z * invDist };
            colp.m_vecPoint      = p;
            colp.m_fDepth        = sph.m_fRadius - dist;

            colp.m_nSurfaceTypeA = sph.m_Surface.m_nMaterial;
            colp.m_nPieceTypeA   = sph.m_Surface.m_nPiece;
            colp.m_nLightingA    = sph.m_Surface.m_nLighting;

            colp.m_nSurfaceTypeB = box.m_Surface.m_nMaterial;
            colp.m_nPieceTypeB   = box.m_Surface.m_nPiece;
            colp.m_nLightingB    = box.m_Surface.m_nLighting;

            minDistSq            = distSq;

            return true;
        }
    }
    return false;
}

/*!
* Check if point is within the triangle
* Unused function - Most likely inlined
* @addr 0x412700
*/
bool __stdcall CCollision::PointInTriangle(CVector const& point, CVector const* triPoints) {
    ZoneScoped;

    // Como el original (0x412700, stdcall): mismo orden de sumas/restas que el EXE (x87 a 24 bits = float)
    const float v1x = triPoints[1].x - triPoints[0].x;
    const float v1y = triPoints[1].y - triPoints[0].y;
    const float v1z = triPoints[1].z - triPoints[0].z;
    const float v2x = triPoints[2].x - triPoints[0].x;
    const float v2y = triPoints[2].y - triPoints[0].y;
    const float v2z = triPoints[2].z - triPoints[0].z;

    const float v1_magSq  = (v1z * v1z + v1x * v1x) + v1y * v1y;
    const float v2_magSq  = (v2z * v2z + v2y * v2y) + v2x * v2x;
    const float v1_dot_v2 = (v2z * v1z + v2y * v1y) + v2x * v1x;

    const float px = point.x - triPoints[0].x;
    const float py = point.y - triPoints[0].y;
    const float pz = point.z - triPoints[0].z;

    const float v1_dot_p = (pz * v1z + px * v1x) + py * v1y;
    const float v2_dot_p = (pz * v2z + py * v2y) + px * v2x;

    const float a = v1_dot_p * v2_magSq - v2_dot_p * v1_dot_v2;
    if (a < 0.f) {
        return false;
    }
    const float b = v2_dot_p * v1_magSq - v1_dot_p * v1_dot_v2;
    if (b < 0.f) {
        return false;
    }
    const float c = v2_magSq * v1_magSq - v1_dot_v2 * v1_dot_v2;
    return !(c < a + b); // NaN pasa, como `test ah,5` del EXE
}

/*!
* @addr 0x412850
*
* @param ln0 Origin of line seg.
* @param ln1 End of line seg.
* @param pt  The point
* 
* @returns Sq. dist. from `pt` to point closest to `pt` on the line segment (ln0, ln1) 
*/
float CCollision::DistToLineSqr(CVector const& ln0, CVector const& ln1, CVector const& pt) {
    ZoneScoped;

    // Como el original (0x412850): l = ln1 - ln0, p = pt - ln0; mismo orden de sumas que el EXE
    const float lx = ln1.x - ln0.x;
    const float ly = ln1.y - ln0.y;
    const float lz = ln1.z - ln0.z;
    const float px = pt.x - ln0.x;
    const float py = pt.y - ln0.y;
    const float pz = pt.z - ln0.z;

    const float pl = (px * lx + pz * lz) + py * ly;
    if (pl <= 0.f) { // Antes del origen
        return (pz * pz + py * py) + px * px;
    }

    const float ll = (lx * lx + lz * lz) + ly * ly;
    if (pl >= ll) { // Despues del final: se resta pt - ln1 directamente
        const float ex = pt.x - ln1.x;
        const float ey = pt.y - ln1.y;
        const float ez = pt.z - ln1.z;
        return (ez * ez + ey * ey) + ex * ex;
    }

    const float cSq = (pz * pz + py * py) + px * px;
    const float bSq = (pl * pl) / ll;
    const float r   = cSq - bSq;
    return r <= 0.f ? 0.f : r; // el EXE devuelve +0 si el resultado no es positivo
}

// 0x417610
float CCollision::DistToLine(const CVector& lineStart, const CVector& lineEnd, const CVector& point) {
    ZoneScoped;

    return std::sqrt(DistToLineSqr(lineStart, lineEnd, point));
}

/*!
* @addr 0x412970
* @brief Similar to DistToLineSqr it always returns the distance to the projected intersection point.
*/
float CCollision::DistToMathematicalLine(CVector const* lineStart, CVector const* lineEnd, CVector const* point) {
    ZoneScoped;

    // Como el original (0x412970): aSq = |p|^2 - (p.l)^2 / |l|^2 (divide por la longitud de la linea, no por la de p)
    const float lx = lineEnd->x - lineStart->x;
    const float ly = lineEnd->y - lineStart->y;
    const float lz = lineEnd->z - lineStart->z;
    const float px = point->x - lineStart->x;
    const float py = point->y - lineStart->y;
    const float pz = point->z - lineStart->z;

    const float dot  = (px * lx + pz * lz) + py * ly;
    const float pMag = (pz * pz + py * py) + px * px;
    const float lMag = (lx * lx + lz * lz) + ly * ly;

    const float aSq = pMag - (dot * dot) / lMag;
    return aSq <= 0.f ? 0.f : std::sqrt(aSq); // NaN llega a la raiz, como `test ah,41h`
}

/*!
* @addr 0x412A30
* @brief Same as DistToMathematicalLine but in 2D
*/
float CCollision::DistToMathematicalLine2D(float lineStartX, float lineStartY, float lineEndX, float lineEndY, float pointX, float pointY) {
    ZoneScoped;

    // Como el original (0x412A30)
    const float px  = pointX - lineStartX;
    const float py  = pointY - lineStartY;
    const float dot = px * lineEndX + py * lineEndY;
    const float distSq = (py * py + px * px) - dot * dot;
    return distSq <= 0.f ? 0.f : std::sqrt(distSq);
}

/*!
* @addr 0x412A80
* @brief TODO
*/
float CCollision::DistAlongLine2D(float lineX, float lineY, float lineDirX, float lineDirY, float pointX, float pointY) {
    ZoneScoped;

    return (pointX - lineX) * lineDirX + (pointY - lineY) * lineDirY;
}


/*!
* Calculate point closest to `point` on line (l0 - l1)
*
* @notsa
*/
CVector CCollision::GetClosestPtOnLine(const CVector& l0, const CVector& l1, const CVector& point) {
    ZoneScoped;

    const auto lnMagSq = (l1 - l0).SquaredMagnitude();
	const auto dot = (point - l0).Dot(l1 - l0);
    if (dot <= 0.0f) {
		return l0;
    }
    if (dot >= lnMagSq) {
		return l1;
    }
    return lerp(l0, l1, dot / lnMagSq);
}


// 0x417FD0
void CCollision::ClosestPointOnLine(const CVector& point, const CVector& l0, const CVector& l1, CVector& closest) {
    ZoneScoped;

    // Como el original (0x417FD0): el primer argumento es el PUNTO, el segundo el origen de la recta y el tercero su final
    const float ax = point.x - l0.x;
    const float ay = point.y - l0.y;
    const float az = point.z - l0.z;
    const float bx = l1.x - l0.x;
    const float by = l1.y - l0.y;
    const float bz = l1.z - l0.z;

    const float len = std::sqrt((bz * bz + by * by) + bx * bx);
    const float inv = 1.0f / len;
    const float nx  = bx * inv;
    const float ny  = by * inv;
    const float nz  = bz * inv;

    const float proj = (nz * az + ny * ay) + nx * ax;
    if (proj < 0.f) {
        closest = l0;
        return;
    }
    if (proj > len) {
        closest = l1;
        return;
    }
    const float x = nx * proj;
    const float y = ny * proj;
    const float z = nz * proj;
    closest.x = x + l0.x;
    closest.y = y + l0.y;
    closest.z = z + l0.z;
}

// 0x415950
void CCollision::Closest3(CVector* verts, CVector* point) {
    ZoneScoped;

    // Como el original (0x415950): sustituye `point` por el mas cercano de los 3 puntos `verts[0..2]`
    const float d0x = point->x - verts[0].x, d0y = point->y - verts[0].y, d0z = point->z - verts[0].z;
    const float d1x = point->x - verts[1].x, d1y = point->y - verts[1].y, d1z = point->z - verts[1].z;
    const float d2x = point->x - verts[2].x, d2y = point->y - verts[2].y, d2z = point->z - verts[2].z;

    const float dist0 = (d0z * d0z + d0x * d0x) + d0y * d0y;
    const float dist1 = (d1z * d1z + d1y * d1y) + d1x * d1x;
    const float dist2 = (d2z * d2z + d2y * d2y) + d2x * d2x;

    if (dist0 < dist1) {
        *point = (dist0 < dist2) ? verts[0] : verts[2];
    } else {
        *point = (dist1 < dist2) ? verts[1] : verts[2];
    }
}

/*!
* Computes closest points C1 and C2 of S1(s)=P1+s*d1 and
* S2(t)=P2+t*d2, returning s and t.
*
* @param       s1p0 Origin of ln. seg 1
* @param       d1 Direction of ln. seg. 1 (Not normalized!)
* @param       a  Sq. mag. of ln. seg 1
* @param       s2p1 As s1p0 ln. seg. 2
* @param       d1 As d1 ln. seg. 2
* @param       e  As a for ln. seg. 2
* @param [out] s  Intersection parameter on ln. seg. 1
* @param [out] t  Intersection parameter on ln. seg. 2
* @param [out] c1 Point on ln. seg. 1
* @param [out] c2 Point on ln. seg. 2
* 
* @returns squared distance between between S1(s) and S2(t)
*
* Credit: Code from "Real-Time Collision Detection" by Christer Ericson, published by Morgan Kaufmann Publishers, © 2005 Elsevier Inc
*/
float CCollision::ClosestPtSegmentSegment(
    CVector p1, CVector d1, float a,
    CVector p2, CVector d2, float e,
    float& s, float& t,
    CVector& c1, CVector& c2
) {
    ZoneScoped;

    // Must be non-negative!
    assert(a >= 0.f);
    assert(e >= 0.f);

    constexpr auto EPSILON = std::numeric_limits<float>::epsilon();

    const auto r = p1 - p2;
    const auto f = d2.Dot(r);

    // Check if either or both segments degenerate into points
    if (a <= EPSILON && e <= EPSILON) {
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return (c1 - c2).SquaredMagnitude();
    }

    if (a <= EPSILON) {
        // First segment degenerates into a point
        s = 0.0f;
        t = f / e;
        // s = 0 => t = (b*s + f) / e = f / e
        t = std::clamp(t, 0.0f, 1.0f);
    } else {
        const auto c = d1.Dot(r);
        if (e <= EPSILON) {
            // Second segment degenerates into a point
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
            // t = 0 => s = (b*t - c) / a = -c / a
        } else {
            // The general nondegenerate case starts here
            const auto b = d1.Dot(d2);
            const auto denom = a * e - b * b;
            // Always nonnegative
            // If segments not parallel, compute closest point on L1 to L2 and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if (denom != 0.0f) {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            } else {
                s = 0.0f;
            }
            // Compute point on L2 closest to S1(s) using
            // t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
            t = (b * s + f) / e;
            // If t in [0,1] done. Else clamp t, recompute s for the new value
            // of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
            // and clamp s to [0, 1]
            if (t < 0.0f) {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return (c1 - c2).SquaredMagnitude();
}

// 0x415A40
/*!
* This one took me quite a bit to figure out.
* Of course they weren't logical at all and did something weird :)
* Turns out, they used an algorithm out of a book by Dan Sunday,
* see here (https://web.archive.org/web/20210330143700/http://geomalgorithms.com/a07-_distance.html)
* I opted to use something different, but it remains to be seen how much more efficient it is (or isnt)
*
* @param s1p0 Seg. 1 origin
* @param s2p0 Seg. 2 origin
* @param s2p1 Seg. 2 end
* @param u    Dir. of seg. 1 (Unnormalized!)
* @param a    Sq. mag. of seg 1
*
* @returns Sq. dist. of the closest points on the 2 line segments
*/
float ClosestSquaredDistanceBetweenFiniteLines(
    const CVector& s1p0,
    const CVector& s2p0, const CVector& s2p1,
    const CVector& u, float a
) {
    // Como el original (0x415A40): el algoritmo de Dan Sunday con el orden de operaciones del EXE (x87 a 24 bits = float).
    // `a` es el cuadrado de |u| que pasa el llamador (no se recalcula).
    constexpr float SMALL_NUM = 0.00001f; // 0x858C14

    const float vx = s2p1.x - s2p0.x, vy = s2p1.y - s2p0.y, vz = s2p1.z - s2p0.z;
    const float wx = s1p0.x - s2p0.x, wy = s1p0.y - s2p0.y, wz = s1p0.z - s2p0.z;

    const float b = (vz * u.z + vy * u.y) + vx * u.x;
    const float c = (vx * vx + vz * vz) + vy * vy;
    const float d = (wz * u.z + wy * u.y) + wx * u.x;
    const float e = (wx * vx + wz * vz) + wy * vy;
    const float D = c * a - b * b;

    float sN, sD = D, tN, tD = D;
    if (D < SMALL_NUM) { // Casi paralelas
        sN = 0.0f;
        sD = 1.0f;
        tN = e;
        tD = c;
    } else {
        sN = e * b - d * c;
        tN = e * a - d * b;
        if (sN < 0.0f) {
            sN = 0.0f;
            sD = 1.0f;
            tN = e;
            tD = c;
        } else if (sN > D) {
            sN = D;
            tN = e + b;
            tD = c;
        }
    }

    // Recorte de s al rango [0, sD] para un t dado (-d o b - d)
    const auto ClampS = [&](float x) {
        if (x < 0.0f) {
            sN = 0.0f;
        } else if (x > a) {
            sN = sD;
        } else {
            sN = x;
            sD = a;
        }
    };

    if (tN < 0.0f) {
        tN = 0.0f;
        ClampS(-d);
    } else if (tN > tD) {
        tN = tD;
        ClampS(b - d);
    }

    const float sc = std::fabs(sN) < SMALL_NUM ? 0.0f : sN / sD;
    const float tc = std::fabs(tN) < SMALL_NUM ? 0.0f : tN / tD;

    const float tvx = vx * tc, tvy = vy * tc, tvz = tc * vz;
    const float px = sc * u.x + wx;
    const float py = sc * u.y + wy;
    const float pz = sc * u.z + wz;
    const float dPx = px - tvx;
    const float dPy = py - tvy;
    const float dPz = pz - tvz;
    return (dPz * dPz + dPy * dPy) + dPx * dPx;
}

/*!
* Calculate clamped barycentric coordinates of a point (p) on a triangle (a, b, c)
* See: https://gamedev.stackexchange.com/a/23745
*
* @notsa
*/
CVector CCollision::GetBaryCoordsOnTriangle(CVector a, CVector b, CVector c, CVector p) {
    ZoneScoped;

    const auto vab = b - a, vac = c - a, vap = p - a;
    const auto bb  = vab.Dot(vab);
    const auto bc  = vab.Dot(vac);
    const auto cc  = vac.Dot(vac);
    const auto pb  = vap.Dot(vab);
    const auto pc  = vap.Dot(vac);
    const auto d   = bb * cc - bc * bc;
    const auto v   = (cc * pb - bc * pc) / d;
    const auto w   = (bb * pc - bc * pb) / d;
    const auto u   = 1.0f - v - w;
    return { u, v, w };
}

/*!
* Get barycentric coords of point (p) clamped onto triangle (a, b, c)
* See: https://stackoverflow.com/a/37923949
*
* @notsa
*/
CVector CCollision::GetClampedBaryCoordsIntoTriangle(CVector a, CVector b, CVector c, CVector p) {
    ZoneScoped;

    // Calculate barycentric coords
    const auto [u, v, w] = GetBaryCoordsOnTriangle(a, b, c, p);

    // Calculate new `t` 
    const auto Get = [&](CVector v1, CVector v2) {
        const auto d = v1 - v2;
        return std::clamp((p - v2).Dot(d) / d.Dot(d), 0.f, 1.f);
    };

    if (u < 0.f) {
        const auto t = Get(c, b);
        return { 0.0f, 1.0f - t, t };
    }
    
    if (v < 0.f) {
        const auto t = Get(a, c);
        return { t, 0.0f, 1.0f - t };
    }

    
    if (w < 0.f) {
        const auto t = Get(b, a);
        return { 1.0f - t, t, 0.0f };
    }

    return { u, v, w }; // Point was in the triangle
}

/*!
* Calculate the point closest to `p` on the triangle (a, b, c)
* Basically same as `ClosestPtPointTriangle`.
* I'm leaving it here (even though it's unused), as it might actually be faster than the abovementioned function .
*/
CVector CCollision::GetCoordsClampedIntoTriangle(CVector a, CVector b, CVector c, CVector p) {
    ZoneScoped;

    const auto [u, v, w] = GetClampedBaryCoordsIntoTriangle(a, b, c, p);
    return a * u + b * v + c * w;
}

/*!
* Calculate the point closest to `p` on the triangle (a, b, c)
* 
* Credit: Code from "Real-Time Collision Detection" by Christer Ericson, published by Morgan Kaufmann Publishers, © 2005 Elsevier Inc
*/
CVector ClosestPtPointTriangle(
    CVector a, CVector b, CVector c,
    CVector p
) {
    // Check if P in vertex region outside A
    const auto ab = b - a;
    const auto ac = c - a;
    const auto ap = p - a;

    const auto d1 = ab.Dot(ap);
    const auto d2 = ac.Dot(ap);

    if (d1 <= 0.0f && d2 <= 0.0f) {
        return a; // barycentric coordinates (1,0,0)
    }

    // Check if P in vertex region outside B
    const auto bp = p - b;
    const auto d3 = ab.Dot(bp);
    const auto d4 = ac.Dot(bp);
    if (d3 >= 0.0f && d4 <= d3) {
        return b; // barycentric coordinates (0,1,0)
    }

    // Check if P in edge region of AB, if so return projection of P onto AB
    const auto vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        const auto v = d1 / (d1 - d3);
        return a + v * ab; // barycentric coordinates (1-v,v,0)
    }

    // Check if P in vertex region outside C
    const auto cp = p - c;
    const auto d5 = ab.Dot(cp);
    const auto d6 = ac.Dot(cp);
    if (d6 >= 0.0f && d5 <= d6) {
        return c; // barycentric coordinates (0,0,1)
    }

    // Check if P in edge region of AC, if so return projection of P onto AC
    const auto vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        const auto w = d2 / (d2 - d6);
        return a + w * ac; // barycentric coordinates (1-d1,0,d1)
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    const auto va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        const auto w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b); // barycentric coordinates (0,1-d1,d1)
    }

    // P inside face region. Compute Q through its barycentric coordinates (r,v,d1)
    const auto denom = va + vb + vc;
    const auto v     = vb / denom;
    const auto w     = vc / denom;
    return a + ab * v + ac * w; // = r*a + v*b + d1*c, r = va * denom = 1.0f-v-d1
}

NOTSA_FORCEINLINE bool ProcessLineSphere_Internal(
    const CColLine& line,
    const CColSphere& sphere,
    CVector* ip,
    float*   depth
) {
    if (!CCollision::s_DebugSettings.ShapeShapeCollision.IsEnabled(Shape::SSPHERE, Shape::SLINE)) {
        return false;
    }

    // C - sphere center
    // P - line segment origin
    // d - line segment dir (unnormalized in our case)
    // m - P - C
    // Solve:             (d.d)t^2 + 2(m.d)t + (m.m) - r^2 = 0 (for t)
    // Quadratic formula: (-b ± sqrt(b²-ac)) / a
    // Where:              a = d.d, b = m.d, c = m.m - r^2

    const auto d = line.m_vecEnd - line.m_vecStart;
    const auto m = line.m_vecStart - sphere.m_vecCenter;
    const auto c = m.Dot(m) - sq(sphere.m_fRadius);
    const auto a = d.Dot(d);

    // Line origin inside sphere
    if (c <= 0.f) {
        if (CCollision::s_DebugSettings.AllowLineOriginInsideSphere) {
            if (depth) {
                // No need to check for depth value, as 0 is the lowest it ever gets
                *ip    = line.m_vecStart;
                *depth = 0.f;
            }
            return true;
        }
        return false;
    }

    const auto b = m.Dot(d);

    // Ray origin outside sphere and ray pointing away from sphere
    if (b > 0.f) {
        return false;
    }

    const auto discr = sq(b) - a * c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f) {
        return false;
    }

    // NOTSA: Zero-length line → a = d.Dot(d) == 0 → avoid 0/0
    if (a <= 0.0f) {
        return false;
    }

    // Solve quadratic for smallest `t` (As it's closer to the line origin)
    const auto t = (-b - std::sqrt(discr)) / a;

    // Finally, check if the intersection point is actually on the line segment
    if (t > 1.f) {
        return false;
    }

    // Calculate intersection point (if needed)
    if (depth) {
        if (t >= *depth) {
            return false; // Act like there was no intersection
        }
        *depth = t;
        *ip    = line.m_vecStart + d * t;
    }

    return true;
}

/*!
* @addr 0x412AA0
* @brief Process line sphere intersection - Doesn'maxTouchDist deal well with cases where line starts/ends inside the sphere.
*
* @param[in,out] depth `t` parameter - relative distance on line from it's origin (`line.start`)
*/
bool CCollision::ProcessLineSphere(CColLine const& line, CColSphere const& sphere, CColPoint& colPoint, float& depth) {
    ZoneScoped;

    // Como el original (0x412AA0): sin atajo para origen dentro de la esfera ni ajustes de depuracion.
    // El orden de las sumas es el del x87 (z, x, y en `a`; z, y, x en `b` y en `m.m`).
    const float dx = line.m_vecEnd.x - line.m_vecStart.x;
    const float dy = line.m_vecEnd.y - line.m_vecStart.y;
    const float dz = line.m_vecEnd.z - line.m_vecStart.z;
    const float a  = (dz * dz + dx * dx) + dy * dy;

    // Como el original: m = centro - origen
    const float mx = sphere.m_vecCenter.x - line.m_vecStart.x;
    const float my = sphere.m_vecCenter.y - line.m_vecStart.y;
    const float mz = sphere.m_vecCenter.z - line.m_vecStart.z;
    const float b  = -((mz * dz + my * dy) + mx * dx);
    const float mm = (mz * mz + my * my) + mx * mx;
    const float c  = mm - sphere.m_fRadius * sphere.m_fRadius;

    const float discr = b * b - c * a;
    if (discr < 0.0f) {
        return false;
    }

    const float t = (-b - std::sqrt(discr)) / a;
    if (t < 0.0f || t > 1.0f || !(t < depth)) {
        return false;
    }

    const CVector point{ dx * t + line.m_vecStart.x, dy * t + line.m_vecStart.y, dz * t + line.m_vecStart.z };
    CVector       normal{ point.x - sphere.m_vecCenter.x, point.y - sphere.m_vecCenter.y, point.z - sphere.m_vecCenter.z };
    normal.Normalise();

    colPoint.m_vecPoint      = point;
    colPoint.m_vecNormal     = normal;

    // Como el original: copia 3 bytes (material, pieza, iluminacion) a B; A solo recibe material y pieza a 0
    colPoint.m_nSurfaceTypeB = sphere.m_Surface.m_nMaterial;
    colPoint.m_nPieceTypeB   = sphere.m_Surface.m_nPiece;
    colPoint.m_nLightingB    = sphere.m_Surface.m_nLighting;
    colPoint.m_nSurfaceTypeA = {};
    colPoint.m_nPieceTypeA   = 0;

    depth = t;

    ms_iProcessLineNumCrossings += 2;

    return true;
}

// 0x417470
bool CCollision::TestLineSphere(
    const CColLine& line,
    const CColSphere& sphere
) {
    ZoneScoped;

    // Como el original (0x417470): solo prueba la raiz cercana de la ecuacion de segundo grado (t en [0, 1]);
    // la suma de cuadrados y el orden de operaciones son los del EXE (x87 a 24 bits = float)
    const CVector& st = line.m_vecStart;
    const CVector& c  = sphere.m_vecCenter;

    const float dx = line.m_vecEnd.x - st.x;
    const float dy = line.m_vecEnd.y - st.y;
    const float dz = line.m_vecEnd.z - st.z;
    const float len = std::sqrt((dz * dz + dx * dx) + dy * dy);

    const float sx = c.x - st.x;
    const float sy = c.y - st.y;
    const float sz = c.z - st.z;

    if (len < 0.000001f) { // Linea degenerada (0x858C18): solo se mira si el punto esta dentro de la esfera
        const float r  = sphere.m_fRadius;
        const float d2 = (sz * sz + sy * sy) + sx * sx;
        return r * r >= d2; // `test ah,1h`: NaN da falso
    }

    const float dot  = (sz * dz + sx * dx) + sy * dy;
    const float h    = dot * -2.0f;
    const float len2 = len * len;
    const float h2  = h * h;

    const float cMag   = (c.z * c.z + c.y * c.y) + c.x * c.x;
    const float stMag  = (st.z * st.z + st.y * st.y) + st.x * st.x;
    const float sum    = cMag + stMag;
    const float cs     = (st.z * c.z + st.x * c.x) + st.y * c.y;
    const float s2     = sum - (cs + cs);
    const float rr     = sphere.m_fRadius * sphere.m_fRadius;
    const float disc   = h2 - ((s2 - rr) * len2) * 4.0f;
    if (disc < 0.f) {
        return false;
    }

    const float t = ((-h) - std::sqrt(disc)) / (len2 + len2);
    if (t < 0.f) {
        return false;
    }
    return !(t > 1.0f);
}

// 0x412C70
// Maybe just adapt the code from: http://www.3dkingdoms.com/weekly/weekly.php?a=3 ? Looks nicer than this one
bool CCollision::TestLineBox_DW(CColLine const& line, CBox const& box) {
    ZoneScoped;

    // Como el original (0x412C70): codigo de salida tipo Cohen-Sutherland a partir de los bits de signo de las
    // diferencias (el signo de -0.0 cuenta), y despues se cruzan los planos de la caja
    const CVector& s  = line.m_vecStart;
    const CVector& e  = line.m_vecEnd;
    const CVector& mn = box.m_vecMin;
    const CVector& mx = box.m_vecMax;

    const auto Neg = [](float v) -> uint32 { return std::bit_cast<uint32>(v) >> 31; };

    const float sMaxX = mx.x - s.x, sMaxY = mx.y - s.y, sMaxZ = mx.z - s.z;
    const float sMinX = s.x - mn.x, sMinY = s.y - mn.y, sMinZ = s.z - mn.z;
    const float eMaxX = mx.x - e.x, eMaxY = mx.y - e.y, eMaxZ = mx.z - e.z;
    const float eMinX = e.x - mn.x, eMinY = e.y - mn.y, eMinZ = e.z - mn.z;

    const uint32 codeS = Neg(sMinX) | (Neg(sMaxX) << 1) | (Neg(sMinY) << 2) | (Neg(sMaxY) << 3) | (Neg(sMinZ) << 4) | (Neg(sMaxZ) << 5);
    const uint32 codeE = Neg(eMinX) | (Neg(eMaxX) << 1) | (Neg(eMinY) << 2) | (Neg(eMaxY) << 3) | (Neg(eMinZ) << 4) | (Neg(eMaxZ) << 5);

    if (codeS & codeE) { // Ambos extremos fuera por el mismo lado
        return false;
    }
    if (codeS == 0 || codeE == 0) { // Un extremo dentro de la caja
        return true;
    }

    const uint32 code = codeS | codeE;
    const float  dx   = e.x - s.x;
    const float  dy   = e.y - s.y;
    const float  dz   = e.z - s.z;

    const auto InYZ = [&](float t) {
        const float y = dy * t + s.y;
        const float z = dz * t + s.z;
        return y > mn.y && y < mx.y && z > mn.z && z < mx.z;
    };
    const auto InXZ = [&](float t) {
        const float x = dx * t + s.x;
        const float z = dz * t + s.z;
        return x > mn.x && x < mx.x && z > mn.z && z < mx.z;
    };
    const auto InXY = [&](float t) {
        const float x = dx * t + s.x;
        const float y = dy * t + s.y;
        return x > mn.x && x < mx.x && y > mn.y && y < mx.y;
    };

    if (code & 0x3) { // Planos X
        const float inv = 1.0f / dx;
        if ((code & 0x1) && InYZ(-(sMinX * inv))) {
            return true;
        }
        if ((code & 0x2) && InYZ(sMaxX * inv)) {
            return true;
        }
    }
    if (code & 0xC) { // Planos Y
        const float inv = 1.0f / dy;
        if ((code & 0x4) && InXZ(-(sMinY * inv))) {
            return true;
        }
        if ((code & 0x8) && InXZ(sMaxY * inv)) {
            return true;
        }
    }
    if (code & 0x30) { // Planos Z
        const float inv = 1.0f / dz;
        if ((code & 0x10) && InXY(-(sMinZ * inv))) {
            return true;
        }
        if ((code & 0x20) && InXY(sMaxZ * inv)) {
            return true;
        }
    }
    return false;
}

// 0x413070
bool CCollision::TestLineBox(CColLine const& line, CBox const& box) {
    ZoneScoped;

    return TestLineBox_DW(line, box);
}

/*!
* @addr 0x413080
* @brief Test vertical \a line against \a bb
*/
bool CCollision::TestVerticalLineBox(CColLine const& line, CBox const& box) {
    ZoneScoped;

    // Como el original (0x413080): comparaciones estrictas en X/Y (NaN falla) y en Z el menor/mayor se elige con `start.z < end.z`
    const CVector& s = line.m_vecStart;
    if (!(s.x > box.m_vecMin.x) || !(s.y > box.m_vecMin.y) || !(s.x < box.m_vecMax.x) || !(s.y < box.m_vecMax.y)) {
        return false;
    }

    float lo, hi;
    if (s.z < line.m_vecEnd.z) {
        lo = s.z;
        hi = line.m_vecEnd.z;
    } else {
        lo = line.m_vecEnd.z;
        hi = s.z;
    }
    if (lo > box.m_vecMax.z) {
        return false;
    }
    return !(hi < box.m_vecMin.z);
}

// 0x413100
/*!
* @addr 0x413100
* @brief Process \a line and \a bb collision.
*
* @param[out]    diskColPoint         Collision point
* @param[in,out] maxTouchDistance Collision point depth inside bb - If calculated value is higher than this value the function will return false, and no colpoint will be set.
*
* @returns If there was a collision or not. If there was a collision, but calculated depth is bigger than `maxTouchDistance` it returns false regardless.
*/
bool CCollision::ProcessLineBox(CColLine const& line, CColBox const& box, CColPoint& colPoint, float& maxTouchDistance) {
    ZoneScoped;

    // Como el original (0x413100): origen estrictamente dentro de la caja => punto interior y distancia 0
    const auto& s  = line.m_vecStart;
    const auto& e  = line.m_vecEnd;
    const auto& mn = box.m_vecMin;
    const auto& mx = box.m_vecMax;

    const auto IsEndOutside = [&] {
        return e.x < mn.x || e.y < mn.y || e.z < mn.z || e.x > mx.x || e.y > mx.y || e.z > mx.z;
    };

    if (s.x > mn.x && s.y > mn.y && s.z > mn.z && s.x < mx.x && s.y < mx.y && s.z < mx.z) {
        if (IsEndOutside()) {
            ms_iProcessLineNumCrossings++;
        }

        // El original escribe solo punto, normal y profundidad (el resto de campos queda intacto)
        CColPoint inside;
        CalculateColPointInsideBox(box, s, inside);
        colPoint.m_vecPoint  = inside.m_vecPoint;
        colPoint.m_vecNormal = inside.m_vecNormal;
        colPoint.m_fDepth    = inside.m_fDepth;

        colPoint.m_nSurfaceTypeA = {};
        colPoint.m_nPieceTypeA   = 0;
        colPoint.m_nSurfaceTypeB = box.m_Surface.m_nMaterial;
        colPoint.m_nPieceTypeB   = box.m_Surface.m_nPiece;
        colPoint.m_nLightingB    = box.m_Surface.m_nLighting;

        maxTouchDistance = 0.0f;
        return true;
    }

    // Como el original: t = A / (A - B) con A, B las distancias firmadas de los extremos al plano (no (ex - sx))
    float   mint = 1.0f;
    CVector p;
    CVector normal;

    { // Plano x minimo
        const float A = mn.x - s.x, B = mn.x - e.x;
        if (B * A < 0.0f) {
            const float t = A / (A - B);
            const float y = (e.y - s.y) * t + s.y;
            const float z = (e.z - s.z) * t + s.z;
            if (y > mn.y && y < mx.y && z > mn.z && z < mx.z && t < mint) {
                mint   = t;
                p      = CVector{ mn.x, y, z };
                normal = CVector{ -1.0f, 0.0f, 0.0f };
            }
        }
    }
    { // Plano x maximo
        const float A = s.x - mx.x, B = e.x - mx.x;
        if (B * A < 0.0f) {
            const float t = A / (A - B);
            const float y = (e.y - s.y) * t + s.y;
            const float z = (e.z - s.z) * t + s.z;
            if (y > mn.y && y < mx.y && z > mn.z && z < mx.z && t < mint) {
                mint   = t;
                p      = CVector{ mx.x, y, z };
                normal = CVector{ 1.0f, 0.0f, 0.0f };
            }
        }
    }
    { // Plano y minimo
        const float A = mn.y - s.y, B = mn.y - e.y;
        if (B * A < 0.0f) {
            const float t = A / (A - B);
            const float x = (e.x - s.x) * t + s.x;
            const float z = (e.z - s.z) * t + s.z;
            if (x > mn.x && x < mx.x && z > mn.z && z < mx.z && t < mint) {
                mint   = t;
                p      = CVector{ x, mn.y, z };
                normal = CVector{ 0.0f, -1.0f, 0.0f };
            }
        }
    }
    { // Plano y maximo
        const float A = s.y - mx.y, B = e.y - mx.y;
        if (B * A < 0.0f) {
            const float t = A / (A - B);
            const float x = (e.x - s.x) * t + s.x;
            const float z = (e.z - s.z) * t + s.z;
            if (x > mn.x && x < mx.x && z > mn.z && z < mx.z && t < mint) {
                mint   = t;
                p      = CVector{ x, mx.y, z };
                normal = CVector{ 0.0f, 1.0f, 0.0f };
            }
        }
    }
    { // Plano z minimo
        const float A = mn.z - s.z, B = mn.z - e.z;
        if (B * A < 0.0f) {
            const float t = A / (A - B);
            const float x = (e.x - s.x) * t + s.x;
            const float y = (e.y - s.y) * t + s.y;
            if (x > mn.x && x < mx.x && y > mn.y && y < mx.y && t < mint) {
                mint   = t;
                p      = CVector{ x, y, mn.z };
                normal = CVector{ 0.0f, 0.0f, -1.0f };
            }
        }
    }
    { // Plano z maximo
        const float A = s.z - mx.z, B = e.z - mx.z;
        if (B * A < 0.0f) {
            const float t = A / (A - B);
            const float x = (e.x - s.x) * t + s.x;
            const float y = (e.y - s.y) * t + s.y;
            if (x > mn.x && x < mx.x && y > mn.y && y < mx.y && t < mint) {
                mint   = t;
                p      = CVector{ x, y, mx.z };
                normal = CVector{ 0.0f, 0.0f, 1.0f };
            }
        }
    }

    if (!(mint < maxTouchDistance)) {
        return false;
    }

    colPoint.m_vecPoint  = p;
    colPoint.m_vecNormal = normal;

    colPoint.m_nSurfaceTypeB = box.m_Surface.m_nMaterial;
    colPoint.m_nPieceTypeB   = box.m_Surface.m_nPiece;
    colPoint.m_nLightingB    = box.m_Surface.m_nLighting;
    colPoint.m_nSurfaceTypeA = {};
    colPoint.m_nPieceTypeA   = 0;

    maxTouchDistance = mint;

    // Como el original: +1 si el final queda dentro de la caja, +2 si queda fuera
    ms_iProcessLineNumCrossings += IsEndOutside() ? 2 : 1;

    return true;
}

/*!
* @addr 0x4138D0
* @returns If there was an intersection - TODO: What if lines are colinear?
*/
bool CCollision::Test2DLineAgainst2DLine(float line1StartX, float line1StartY, float line1EndX, float line1EndY, float line2StartX, float line2StartY, float line2EndX, float line2EndY) {
    ZoneScoped;

    // Como el original (0x4138D0): NaN pasa cada prueba (`test ah,41h`)
    const float a = line2StartX - line1StartX;
    const float b = line2StartY - line1StartY;
    const float x1 = (a + line2EndX) * line1EndY - (b + line2EndY) * line1EndX;
    const float x2 = a * line1EndY - b * line1EndX;
    if (x1 * x2 > 0.f) {
        return false;
    }

    const float c = line1StartX - line2StartX;
    const float d = line1StartY - line2StartY;
    const float y1 = (c + line1EndX) * line2EndY - (d + line1EndY) * line2EndX;
    const float y2 = c * line2EndY - d * line2EndX;
    return !(y1 * y2 > 0.f);
}

namespace ColProc59b {
// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
inline CVector MultiplyMatrixWithVector(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: Multiply3x3 (0x59C790), igual que el anterior sin la posicion
inline CVector Multiply3x3(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y
    };
}

// Como el original: CColPoint::operator= del EXE (0x40FC80) no copia field_C, field_1C ni el relleno 0x26
inline void CopyColPoint(CColPoint& dst, const CColPoint& src) {
    dst.m_vecPoint      = src.m_vecPoint;
    dst.m_vecNormal     = src.m_vecNormal;
    dst.m_nSurfaceTypeA = src.m_nSurfaceTypeA;
    dst.m_nPieceTypeA   = src.m_nPieceTypeA;
    dst.m_nLightingA    = src.m_nLightingA;
    dst.m_nSurfaceTypeB = src.m_nSurfaceTypeB;
    dst.m_nPieceTypeB   = src.m_nPieceTypeB;
    dst.m_nLightingB    = src.m_nLightingB;
    dst.m_fDepth        = src.m_fDepth;
}
} // namespace ColProc59b

/*
* Process disk-colpoint or otherwise line-colpoint collision.
* @addr 0x413960
* @param tempTriCol    Colpoint with a triangle (Space B)
* @param matBA         Transformation matrix from B's space into A's (The space we're in)
* @param disk          The disk (Space A)
* @param diskColPoint  Disk collision point (Space A)
* @param lineCollision If there was a line collision (Only checked if no discr collision)
* @param lineRatio     Not sure
* @param lineColPoint  Line colpoint (Only valid if `lineCollision` was set)
* @returns If the diskColPoint collides with the discr
*/
// 0x413960
bool CCollision::ProcessDiscCollision(
    CColPoint& tempTriCol,
    const CMatrix& matBA,
    const CColDisk& disk,
    CColPoint& diskColPoint,
    bool& lineCollision,
    float& lineRatio,
    CColPoint& lineColPoint
) {
    ZoneScoped;

    // Como el original (0x413960): transformaciones con el orden de sumas del EXE (0x59C890 / 0x59C790)
    const auto cp       = ColProc59b::MultiplyMatrixWithVector(matBA, tempTriCol.m_vecPoint);
    const auto cpNormal = ColProc59b::Multiply3x3(matBA, tempTriCol.m_vecNormal);

    // Como el original: las sumas del producto escalar van en orden y, z, x
    const float normalDotThick = (cpNormal.y * disk.m_vThickness.y + cpNormal.z * disk.m_vThickness.z) + cpNormal.x * disk.m_vThickness.x;
    if (!(std::abs(normalDotThick) < 0.77f)) {
        // Como el original: se salta el calculo del producto del centro
    } else {
        const float dx = cp.x - disk.m_vecCenter.x;
        const float dy = cp.y - disk.m_vecCenter.y;
        const float dz = cp.z - disk.m_vecCenter.z;
        const float centerDotThick = (dy * disk.m_vThickness.y + dz * disk.m_vThickness.z) + dx * disk.m_vThickness.x;
        if (std::abs(centerDotThick) < disk.m_fThickness) {
            // Como el original: sqrt((r^2 - dy^2) - dx^2) + cp.z, sin limitar a 0 (si es negativo da NaN)
            const float lineRatioNow = std::sqrt((disk.m_fRadius * disk.m_fRadius - dy * dy) - dx * dx) + cp.z;
            // Como el original: fcomp + test ah,1 -> salta si es menor O NaN (solo escribe si lineRatioNow >= lineRatio)
            if (lineRatioNow >= lineRatio) {
                lineCollision = true;
                lineRatio     = lineRatioNow;
                ColProc59b::CopyColPoint(lineColPoint, tempTriCol);
            }
            return false; // False is returned here, but `lineCollision` was set to true.
        }
    }

    if (disk.m_Surface.m_nPiece < 17 && tempTriCol.m_fDepth > diskColPoint.m_fDepth) {
        ColProc59b::CopyColPoint(diskColPoint, tempTriCol);
        diskColPoint.m_nSurfaceTypeB = SURFACE_WHEELBASE;
        return true;
    }
    return false;
}


/*!
* Process line-triangle intersection, internal function (used to implement `ProcessLineTriangle` and `TestLineTriangle`)
* @tparam TestOnly If we're only doing an intersection check, and are not interested in the intersection point, normal, etc. In this case the last 3 arguments can be nullptr.
* @notsa
*/
template<bool TestOnly>
bool NOTSA_FORCEINLINE ProcessLineTriangle_Internal(
    const CColLine& line,
    const CStoredCollPoly& poly,
    const CColTrianglePlane& plane,
    float* inOutMaxTouchDist,
    CVector* outIP,
    CVector* outPlNorm
) {
    if (!CCollision::s_DebugSettings.ShapeShapeCollision.IsEnabled(Shape::SLINE, Shape::STRI)) {
        return false;
    }

    const auto &va = poly.verts[0],
               &vb = poly.verts[1],
               &vc = poly.verts[2];

#ifdef NOTSA_VANILLA_COLLISIONS
    // If the line is vertical, we can do some quick bound checks
    if (line.IsVertical() && !CColTriangle::GetBoundingRect(va, vb, vc).IsPointInside(line.m_vecStart)) {
        return false;
    }

    const auto plNorm = plane.GetNormal();
    if constexpr (!TestOnly) {
        *outPlNorm = plNorm;
    }

    // Origin of line on the plane
    const auto plNormDotLnOrigin = plane.GetPtDotNormal(line.m_vecStart);

    // Check if both points are above or below the plane, if so, no chance of intersection
    if (std::signbit(plNormDotLnOrigin) == std::signbit(plane.GetPtDotNormal(line.m_vecEnd))) {
return false;
    }

    // Magnitude of line on plane
    const auto plLnMag = -(line.m_vecEnd - line.m_vecStart).Dot(plNorm);

#ifdef FIX_BUGS
    // Line is (near-)parallel to the plane, no intersection.
    // `plLnMag ~ 0` makes `t = plNormDotLnOrigin / plLnMag` explode to NaN/Inf, which then
    // slips past the `t >= maxTouchDist` compare (NaN compares false) and corrupts the
    // wheel contact / touch distance downstream. Reject near-parallel lines outright.
    if (std::abs(plLnMag) < 1e-6f) {
        return false;
    }
#endif

    const auto t = plNormDotLnOrigin / plLnMag;
    if constexpr (!TestOnly) {
        if (t >= *inOutMaxTouchDist) {
            return false;
        }
    }

	// Find point of intersection
    const auto ip = lerp(line.m_vecStart, line.m_vecEnd, t);
    if constexpr (!TestOnly) {
        *outIP = ip;
    }

    // Get the points relative to the plane's orientation
    // This way the bound checks can be done in 2D
    const auto [pl_va, pl_vb, pl_vc, pl_ip] = [&]() -> std::tuple<CVector2D, CVector2D, CVector2D, CVector2D> {
	    // We do the test in 2D.
        // With the plane direction we can figure out how to project the vectors.
	    // normal = (c - a) x (b - a)
        using enum CColTrianglePlane::Orientation;
	    switch (plane.m_orientation){
	    case POS_X: return {
            {va.y, va.z},
            {vc.y, vc.z},
            {vb.y, vb.z},
            {ip.y, ip.z}
        };
        case NEG_X: return {
            {va.y, va.z},
            {vb.y, vb.z},
            {vc.y, vc.z},
            {ip.y, ip.z}
        };
        case POS_Y: return {
            {va.z, va.x},
            {vc.z, vc.x},
            {vb.z, vb.x},
            {ip.z, ip.x}
        };
        case NEG_Y: return {
            {va.z, va.x},
            {vb.z, vb.x},
            {vc.z, vc.x},
            {ip.z, ip.x}
        };
        case POS_Z: return {
            {va.x, va.y},
            {vc.x, vc.y},
            {vb.x, vb.y},
            {ip.x, ip.y}
        };
        case NEG_Z: return {
			{va.x, va.y},
			{vb.x, vb.y},
			{vc.x, vc.y},
			{ip.x, ip.y}
		};
	    default: NOTSA_UNREACHABLE();
	    }
    }();

	// This is our triangle:
	// pl_vc---pl_vb
	//    \     /
	//     \   /
	//      \ /
	//     pl_va
	// We can use the "2v2 cross product" to check on which side
	// a vector is of another. Test is true if point is inside of all edges.
    const auto pl_ip_a = pl_ip - pl_va;
    if ((pl_vb - pl_va).Cross(pl_ip_a) >= 0.0f && (pl_vc - pl_va).Cross(pl_ip_a) <= 0.0f && (pl_vc - pl_vb).Cross(pl_ip - pl_vb) >= 0.0f) {
        if (inOutMaxTouchDist) {
            *inOutMaxTouchDist = t;
        }
        return true;
    }
    return false;
#else // Not really tested (might not work properly)
    // https://stackoverflow.com/a/42752998
    const auto eB = vb - va,
               eC = vc - va,
               n  = eC.Cross(eB);

    const auto lnseg = line.m_vecEnd - line.m_vecStart;
    
    const auto det = -lnseg.Dot(n);
    if (det < 1e-6f) {
        return false;
    }

    const auto AO = line.m_vecStart - va;
    const auto t  = AO.Dot(n) / det;
    if (t < 0.f || t > 1.f) {
        return false;
    }

    if constexpr (!TestOnly) {
        if (t >= *inOutMaxTouchDist) {
            return false;
        }
    }

    const auto DAO = lnseg.Cross(AO);

    const auto u = eC.Dot(DAO) / det;
    if (u > 1.f) {
        return false;
    }

    const auto v = -eB.Dot(DAO) / det;
    if (v > 1.f) {
        return false;
    }

    if (u + v > 1.f) {
        return false;
    }

    if constexpr (!TestOnly) {
        *inOutMaxTouchDist = t;
        *outIP             = line.m_vecStart + lnseg * t;
        *outPlNorm         = plane.GetNormal();
    }
    return true;
#endif
}

/*!
* @addr 0x413AC0
*/
bool CCollision::TestLineTriangle(const CColLine& line, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& plane) {
    ZoneScoped;

    // Como el original (0x413AC0): corta la recta con el plano y prueba el punto en 2D, eliminando el eje dominante
    // segun la orientacion del plano (tabla de saltos de 6 casos; las orientaciones > 5 dejan basura en el EXE y no se prueban)
    const auto* const pl = reinterpret_cast<const int16*>(&plane);
    const float nx = (float)pl[0] * 0.000244140625f;
    const float ny = (float)pl[1] * 0.000244140625f;
    const float nz = (float)pl[2] * 0.000244140625f;
    const float d  = (float)pl[3] * 0.0078125f;
    const uint8 orientation = *(reinterpret_cast<const uint8*>(&plane) + 8);

    const CVector& s = line.m_vecStart;
    const CVector& e = line.m_vecEnd;

    const float nzsz = nz * s.z;
    const float nysy = ny * s.y;
    const float nxsx = nx * s.x;

    const float dEnd   = ((nz * e.z + ny * e.y) + nx * e.x) - d;
    const float dStart = ((nxsx + nysy) + nzsz) - d;
    if (!(dEnd * dStart <= 0.f)) { // Ambos extremos del mismo lado (o NaN)
        return false;
    }

    const float dx = e.x - s.x;
    const float dy = e.y - s.y;
    const float dz = e.z - s.z;

    const float num = ((d - nxsx) - nysy) - nzsz;
    const float den = (dz * nz + dy * ny) + dx * nx;
    const float t   = num / den;

    const float ix = dx * t + s.x;
    const float iy = dy * t + s.y;
    const float iz = dz * t + s.z;

    if (orientation > 5) {
        return false;
    }

    const auto Vtx = [&](uint16 idx, int axis) {
        return (float)reinterpret_cast<const int16*>(&verts[idx])[axis] * 0.0078125f;
    };

    // Ejes (p, q) del plano 2D por orientacion y si se intercambian B y C (los impares)
    constexpr int axes[6][2] = { {1, 2}, {1, 2}, {2, 0}, {2, 0}, {0, 1}, {0, 1} };
    const int  p    = axes[orientation][0];
    const int  q    = axes[orientation][1];
    const bool swapBC = (orientation & 1) != 0;

    const float Ap = Vtx(tri.vA, p), Aq = Vtx(tri.vA, q);
    const float Bp = Vtx(tri.vB, p), Bq = Vtx(tri.vB, q);
    const float Cp = Vtx(tri.vC, p), Cq = Vtx(tri.vC, q);
    const float Ip = (p == 0) ? ix : (p == 1) ? iy : iz;
    const float Iq = (q == 0) ? ix : (q == 1) ? iy : iz;

    const float Sbp = swapBC ? Cp : Bp, Sbq = swapBC ? Cq : Bq; // [esp+8], [esp+10h] del EXE
    const float Scp = swapBC ? Bp : Cp, Scq = swapBC ? Bq : Cq; // [esp+3Ch], [esp+30h] del EXE

    const float u0 = Iq - Aq;
    const float v0 = Ip - Ap;

    const float s1 = (Scp - Ap) * u0 - (Scq - Aq) * v0;
    if (s1 < 0.f) {
        return false;
    }
    const float s2 = (Sbp - Ap) * u0 - (Sbq - Aq) * v0;
    if (s2 > 0.f) {
        return false;
    }
    const float s3 = (Iq - Scq) * (Sbp - Scp) - (Ip - Scp) * (Sbq - Scq);
    return !(s3 < 0.f);
}

namespace ColProc59b {
// Como el original: prueba 2D comun a ProcessLineTriangle (0x414640), ProcessVerticalLineTriangle (0x414BBA)
// e IsStoredPolyStillValidVerticalLine (0x415158). Las orientaciones pares usan B = v1, C = v2; las impares, al reves.
inline bool PointInTriangle2D(uint8 orientation, const CVector& v0, const CVector& v1, const CVector& v2, const CVector& ip) {
    if (orientation > 5) {
        return false; // El EXE usa basura de la pila; no ocurre con planos validos
    }
    const CVector& vb = (orientation & 1) ? v2 : v1;
    const CVector& vc = (orientation & 1) ? v1 : v2;

    const auto Pick = [&](const CVector& v) {
        switch (orientation >> 1) {
        case 0:  return CVector2D{ v.y, v.z };
        case 1:  return CVector2D{ v.z, v.x };
        default: return CVector2D{ v.x, v.y };
        }
    };
    const auto A = Pick(v0);
    const auto B = Pick(vb);
    const auto C = Pick(vc);
    const auto P = Pick(ip);

    const float X1 = P.y - A.y;
    const float Y1 = P.x - A.x;
    if ((C.x - A.x) * X1 - (C.y - A.y) * Y1 < 0.0f) {
        return false;
    }
    if ((B.x - A.x) * X1 - (B.y - A.y) * Y1 > 0.0f) {
        return false;
    }
    return !((P.y - C.y) * (B.x - C.x) - (P.x - C.x) * (B.y - C.y) < 0.0f);
}

// Como el original (0x414901..): el rectangulo envolvente de ProcessVerticalLineTriangle es asimetrico respecto a v0
inline bool IsOutsideRange(float s, float v0, float v1, float v2) {
    return (s < v0 && s < v1 && s < v2) || (!(s < v0) && s > v1 && s > v2);
}

// Como el original: escritura del resultado de ProcessLineTriangle / ProcessVerticalLineTriangle
inline void StoreTriangleHit(CColPoint& cp, const CVector& point, const CVector& normal, const CColTriangle& tri, CStoredCollPoly* collPoly,
                             const CVector& v0, const CVector& v1, const CVector& v2) {
    cp.m_vecPoint      = point;
    cp.m_vecNormal     = normal;
    cp.m_nSurfaceTypeB = tri.m_nMaterial;
    cp.m_nPieceTypeB   = 0;
    cp.m_nLightingB    = tri.m_nLight;
    cp.m_nSurfaceTypeA = {};
    cp.m_nPieceTypeA   = 0;
    if (collPoly) {
        collPoly->verts[0] = v0;
        collPoly->verts[1] = v1;
        collPoly->verts[2] = v2;
        collPoly->valid    = true;
        const uint32 light = tri.m_nLight.value; // El EXE escribe el byte de iluminacion extendido a dword en +0x28
        std::memcpy(reinterpret_cast<uint8*>(collPoly) + 0x28, &light, sizeof(light));
    }
}

// Como el original: copia de 3 bytes (material, pieza, iluminacion) a la superficie A / B de un CColPoint
inline void CopySurfaceA(CColPoint& cp, const CColSurface& surf) {
    cp.m_nSurfaceTypeA = surf.m_nMaterial;
    cp.m_nPieceTypeA   = surf.m_nPiece;
    cp.m_nLightingA    = surf.m_nLighting;
}
inline void CopySurfaceB(CColPoint& cp, const CColSurface& surf) {
    cp.m_nSurfaceTypeB = surf.m_nMaterial;
    cp.m_nPieceTypeB   = surf.m_nPiece;
    cp.m_nLightingB    = surf.m_nLighting;
}
} // namespace ColProc59b

/*!
* Processes `line \a tri collision.
* 
* @addr 0x4140F0
*
* @param[out]    diskColPoint     Collision point
* @param[in,out] maxTouchDistance Distance from line origin to intersection point
* @param[out]    collPoly         If given (can be null) stored the uncompressed vertices of the triangle and set's it's `actual` field to `true`
*
* @returns If there was a collision that was closer to the beginning of the line than `maxTouchDistance`
*/
bool CCollision::ProcessLineTriangle(const CColLine& line, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& plane, CColPoint& colPoint, float& maxTouchDistance, CStoredCollPoly* collPoly) {
    ZoneScoped;

    // Como el original (0x4140F0): el orden de operaciones es el del x87
    const CVector n = plane.m_normal;
    const float   D = static_cast<float>(plane.m_normalOffset);
    const auto&   s = line.m_vecStart;
    const auto&   e = line.m_vecEnd;

    const float nxsx = n.x * s.x;
    const float nysy = n.y * s.y;
    const float nzsz = n.z * s.z;
    const float E    = (n.z * e.z + n.y * e.y) + n.x * e.x;
    const float S    = (nxsx + nysy) + nzsz;
    if (!((S - D) * (E - D) <= 0.0f)) { // Ambos extremos del mismo lado (o NaN)
        return false;
    }

    const float dx  = e.x - s.x;
    const float dy  = e.y - s.y;
    const float dz  = e.z - s.z;
    const float num = ((D - nxsx) - nysy) - nzsz;
    const float den = (dz * n.z + dy * n.y) + dx * n.x;
    const float t   = num / den;

    const CVector ip{ dx * t + s.x, dy * t + s.y, dz * t + s.z };

    const CVector v0 = verts[tri.vA];
    const CVector v1 = verts[tri.vB];
    const CVector v2 = verts[tri.vC];
    if (!ColProc59b::PointInTriangle2D(static_cast<uint8>(plane.m_orientation), v0, v1, v2, ip)) {
        return false;
    }
    if (!(t < maxTouchDistance)) {
        return false;
    }

    ColProc59b::StoreTriangleHit(colPoint, ip, n, tri, collPoly, v0, v1, v2);
    maxTouchDistance = t;
    return true;
}

// 0x4147E0
bool CCollision::ProcessVerticalLineTriangle(
    const CColLine& line,
    const CompressedVector* verts,
    const CColTriangle& tri,
    const CColTrianglePlane& plane,
    CColPoint& colPoint,
    float& maxTouchDistance,
    CStoredCollPoly* collPoly
) {
    ZoneScoped;

    // Como el original (0x4147E0): rectangulo envolvente en x e y y t = num / (n.z * dz)
    const CVector v0 = verts[tri.vA];
    const CVector v1 = verts[tri.vB];
    const CVector v2 = verts[tri.vC];
    const auto&   s  = line.m_vecStart;
    const auto&   e  = line.m_vecEnd;

    if (ColProc59b::IsOutsideRange(s.x, v0.x, v1.x, v2.x) || ColProc59b::IsOutsideRange(s.y, v0.y, v1.y, v2.y)) {
        return false;
    }

    const CVector n = plane.m_normal;
    const float   D = static_cast<float>(plane.m_normalOffset);

    const float nzsz = n.z * s.z;
    const float E    = (n.z * e.z + n.y * e.y) + n.x * e.x;
    const float S    = (n.x * s.x + n.y * s.y) + nzsz;
    if (!((S - D) * (E - D) <= 0.0f)) {
        return false;
    }

    const float dz  = e.z - s.z;
    const float num = ((D - n.x * s.x) - n.y * s.y) - nzsz;
    const float t   = num / (n.z * dz);

    const CVector ip2D{ s.x, s.y, t * dz + s.z };
    if (!ColProc59b::PointInTriangle2D(static_cast<uint8>(plane.m_orientation), v0, v1, v2, ip2D)) {
        return false;
    }
    if (!(t < maxTouchDistance)) {
        return false;
    }

    // El punto final se calcula con la formula general: start + (end - start) * t
    const CVector ip{ (e.x - s.x) * t + s.x, (e.y - s.y) * t + s.y, (e.z - s.z) * t + s.z };
    ColProc59b::StoreTriangleHit(colPoint, ip, n, tri, collPoly, v0, v1, v2);
    maxTouchDistance = t;
    return true;
}

// 0x416450
bool CCollision::ProcessSphereSphere(const CColSphere& spA, const CColSphere& spB, CColPoint& colPoint, float& maxTouchDistance) {
    ZoneScoped;

    // Como el original (0x416450): sin la salida temprana por distancia al cuadrado; la suma va en orden z, x, y
    const float dx = spA.m_vecCenter.x - spB.m_vecCenter.x;
    const float dy = spA.m_vecCenter.y - spB.m_vecCenter.y;
    const float dz = spA.m_vecCenter.z - spB.m_vecCenter.z;

    const float distSq = (dz * dz + dx * dx) + dy * dy;
    float       touchDist = std::sqrt(distSq) - spB.m_fRadius;
    const float depth     = spA.m_fRadius - touchDist; // Se calcula antes de limitar a 0
    if (touchDist < 0.0f) {
        touchDist = 0.0f;
    }

    const float touchDistSq = touchDist * touchDist;
    if (!(touchDistSq < maxTouchDistance)) {
        return false;
    }
    if (!(touchDist < spA.m_fRadius)) {
        return false;
    }

    CVector normal{ dx, dy, dz };
    normal.Normalise();

    colPoint.m_vecPoint  = CVector{
        spA.m_vecCenter.x - normal.x * touchDist,
        spA.m_vecCenter.y - normal.y * touchDist,
        spA.m_vecCenter.z - normal.z * touchDist
    };
    colPoint.m_vecNormal = normal;

    colPoint.m_nSurfaceTypeA = spA.m_Surface.m_nMaterial;
    colPoint.m_nPieceTypeA   = spA.m_Surface.m_nPiece;
    colPoint.m_nLightingA    = spA.m_Surface.m_nLighting;

    colPoint.m_nSurfaceTypeB = spB.m_Surface.m_nMaterial;
    colPoint.m_nPieceTypeB   = spB.m_Surface.m_nPiece;
    colPoint.m_nLightingB    = spB.m_Surface.m_nLighting;

    colPoint.m_fDepth = depth;
    maxTouchDistance  = touchDistSq;

    return true;
}

/*!
* See: https://realtimecollisiondetection.net/blog/?p=103
* 
* @addr 0x4165B0
*/
bool CCollision::TestSphereTriangle(
    const CColSphere& sphere,
    const CompressedVector* verts,
    const CColTriangle& tri,
    const CColTrianglePlane& plane
) {
    ZoneScoped;

    // Como el original (0x4165B0): distancia al plano, proyeccion en la base (arista AB normalizada, perpendicular en el plano)
    // y, segun cuantas aristas contienen la proyeccion, distancia al vertice, a la arista o al plano; cierto si dist < radio.
    // (El Ericson de la version anterior no es lo que hace el EXE; tampoco existe la comprobacion de depuracion s_DebugSettings.)
    const auto* const pl = reinterpret_cast<const int16*>(&plane);
    const float nx = (float)pl[0] * 0.000244140625f;
    const float ny = (float)pl[1] * 0.000244140625f;
    const float nz = (float)pl[2] * 0.000244140625f;

    const CVector& P = sphere.m_vecCenter;
    const float    R = sphere.m_fRadius;

    const float dot    = (nx * P.x + nz * P.z) + ny * P.y;
    const float distPl = dot - (float)pl[3] * 0.0078125f;
    if (std::fabs(distPl) > R) {
        return false;
    }

    const auto Vtx = [&](uint16 idx, int axis) {
        return (float)reinterpret_cast<const int16*>(&verts[idx])[axis] * 0.0078125f;
    };
    const float Ax = Vtx(tri.vA, 0), Ay = Vtx(tri.vA, 1), Az = Vtx(tri.vA, 2);
    const float Bx = Vtx(tri.vB, 0), By = Vtx(tri.vB, 1), Bz = Vtx(tri.vB, 2);
    const float Cx = Vtx(tri.vC, 0), Cy = Vtx(tri.vC, 1), Cz = Vtx(tri.vC, 2);

    // Arista AB normalizada
    const float e1x = Bx - Ax, e1y = By - Ay, e1z = Bz - Az;
    const float len1 = std::sqrt((e1z * e1z + e1y * e1y) + e1x * e1x);
    const float inv1 = 1.0f / len1;
    const float enx  = e1x * inv1, eny = e1y * inv1, enz = e1z * inv1;

    // Perpendicular en el plano: e1n x normal (0x59C730)
    const float cx = nz * eny - enz * ny;
    const float cy = enz * nx - nz * enx;
    const float cz = enx * ny - nx * eny;

    // C - A y P - A en la base (e1n, c)
    const float wx = Cx - Ax, wy = Cy - Ay, wz = Cz - Az;
    const float wE = (wy * eny + wx * enx) + wz * enz;
    const float wC = (wx * cx + wz * cz) + wy * cy;

    const float px = P.x - Ax, py = P.y - Ay, pz = P.z - Az;
    const float pE = (px * enx + pz * enz) + py * eny;
    const float pC = (px * cx + pz * cz) + py * cy;

    volatile float zero = 0.f; // El EXE multiplica por 0.0 (fmul [858B50h]): da NaN con infinitos
    const float Q1 = pC * len1 - pE * zero;
    const bool  f1 = Q1 >= 0.f;

    const float pEwC = pE * wC;
    const float pCwE = pC * wE;
    const bool  f2   = (pEwC - pCwE) >= 0.f;

    const float Dx  = wE - len1;
    const float px3 = pE - len1;
    const bool  f3  = (Dx * pC - px3 * wC) >= 0.f;

    const auto DistTo = [&](float vx, float vy, float vz) { // |P - V|
        const float x = P.x - vx, y = P.y - vy, z = P.z - vz;
        return std::sqrt((x * x + y * y) + z * z);
    };
    const auto DistEdge = [&](float x) {
        return std::sqrt(x * x + distPl * distPl);
    };

    float dist;
    switch ((int)f1 + (int)f2 + (int)f3) {
    case 0:
        return false;
    case 1:
        if (f1) {
            dist = DistTo(Cx, Cy, Cz);
        } else if (f2) {
            dist = DistTo(Bx, By, Bz);
        } else {
            dist = DistTo(Ax, Ay, Az);
        }
        break;
    case 2:
        if (!f1) { // Arista AB
            const float l2 = len1 * len1;
            const float t  = (pE * len1 + pC * zero) / l2;
            if (t <= 0.f) {
                dist = DistTo(Ax, Ay, Az);
            } else if (t >= 1.f) {
                dist = DistTo(Bx, By, Bz);
            } else {
                dist = DistEdge(Q1 / std::sqrt(l2));
            }
        } else if (!f2) { // Arista AC
            const float l2 = wC * wC + wE * wE;
            const float t  = (pC * wC + pE * wE) / l2;
            if (t <= 0.f) {
                dist = DistTo(Ax, Ay, Az);
            } else if (t >= 1.f) {
                dist = DistTo(Cx, Cy, Cz);
            } else {
                dist = DistEdge((pCwE - pEwC) / std::sqrt(l2));
            }
        } else { // Arista BC
            const float Dy = wC;
            const float l2 = Dy * Dy + Dx * Dx;
            const float t  = (px3 * Dx + pC * Dy) / l2;
            if (t <= 0.f) {
                dist = DistTo(Bx, By, Bz);
            } else if (t >= 1.f) {
                dist = DistTo(Cx, Cy, Cz);
            } else {
                dist = DistEdge((pC * Dx - px3 * Dy) / std::sqrt(l2));
            }
        }
        break;
    default: // 3: la proyeccion esta dentro del triangulo
        dist = std::fabs(distPl);
        break;
    }
    return dist < R;
}

// 0x416BA0
bool CCollision::ProcessSphereTriangle(
    const CColSphere& sphere,
    const CompressedVector* verts,
    const CColTriangle& tri,
    const CColTrianglePlane& plane,
    CColPoint& colPoint,
    float& maxTouchDistance
) {
    ZoneScoped;

    // Como el original (0x416BA0): el orden de operaciones es el del x87 (ver comentarios en cada paso)
    const auto&   c  = sphere.m_vecCenter;
    const CVector n  = plane.m_normal;
    const float   pd = ((n.z * c.z + n.y * c.y) + n.x * c.x) - static_cast<float>(plane.m_normalOffset);

    const float absPd = std::fabs(pd);
    if (absPd > sphere.m_fRadius) {
        return false;
    }
    const float pd2 = pd * pd;
    if (pd2 > maxTouchDistance) {
        return false;
    }

    const CVector va = verts[tri.vA];
    const CVector vb = verts[tri.vB];
    const CVector vc = verts[tri.vC];

    // Base ortogonal del triangulo: vec2 = normalizado(B - A), vec1 = vec2 x n
    const float ux  = vb.x - va.x;
    const float uy  = vb.y - va.y;
    const float uz  = vb.z - va.z;
    const float len = std::sqrt((ux * ux + uz * uz) + uy * uy);
    const float inv = 1.0f / len;
    const CVector vec2{ ux * inv, uy * inv, uz * inv };
    const CVector vec1{
        n.z * vec2.y - vec2.z * n.y,
        vec2.z * n.x - n.z * vec2.x,
        vec2.x * n.y - n.x * vec2.y
    };

    // Coordenadas 2D de C y del centro de la esfera (A = (0, 0), B = (0, len))
    const CVector vac{ vc.x - va.x, vc.y - va.y, vc.z - va.z };
    const float   cy = (vac.x * vec2.x + vac.z * vec2.z) + vac.y * vec2.y;
    const float   cx = (vac.x * vec1.x + vac.z * vec1.z) + vac.y * vec1.y;

    const CVector vas{ c.x - va.x, c.y - va.y, c.z - va.z };
    const float   sy = (vas.x * vec2.x + vas.z * vec2.z) + vas.y * vec2.y;
    const float   sx = (vas.x * vec1.x + vas.z * vec1.z) + vas.y * vec1.y;

    const float d1  = sx * len - sy * 0.0f;
    const bool  c1  = d1 >= 0.0f;
    const bool  c2  = (sy * cx - sx * cy) >= 0.0f;
    const float cyl = cy - len;
    const float syl = sy - len;
    const bool  c3  = (cyl * sx - syl * cx) >= 0.0f;
    const int   count = int(c1) + int(c2) + int(c3);

    CVector ip{};   // Con 0 condiciones (solo con NaN) el EXE usa basura de la pila
    float   dist = len;

    const auto Vertex = [&](const CVector& v) {
        const CVector d{ c.x - v.x, c.y - v.y, c.z - v.z };
        dist = std::sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
        ip   = v;
    };
    // Borde (origen + e * t) a distancia perpendicular d / sqrt(l2)
    const auto EdgeInterior = [&](const CVector& origin, const CVector& e, float t, float d, float l2) {
        const float q = d / std::sqrt(l2);
        dist = std::sqrt(q * q + pd2);
        ip   = CVector{ e.x * t + origin.x, e.y * t + origin.y, e.z * t + origin.z };
    };

    if (count == 3) {
        // Proyeccion dentro del triangulo
        dist = absPd;
        ip   = CVector{ c.x - n.x * pd, c.y - n.y * pd, c.z - n.z * pd };
    } else if (count == 1) {
        // Region de un vertice
        if (c1) {
            Vertex(vc);
        } else if (c2) {
            Vertex(vb);
        } else {
            Vertex(va);
        }
    } else if (count == 2) {
        if (!c1) { // Borde AB
            const float l2 = len * len;
            const float t  = (sy * len + sx * 0.0f) / l2;
            if (t <= 0.0f) {
                Vertex(va);
            } else if (t >= 1.0f) {
                Vertex(vb);
            } else {
                EdgeInterior(va, CVector{ ux, uy, uz }, t, d1, l2);
            }
        } else if (c2) { // Borde BC (c1 y c2)
            const float bc2 = cx * cx + cyl * cyl;
            const float t   = (syl * cyl + sx * cx) / bc2;
            if (t <= 0.0f) {
                Vertex(vb);
            } else if (t >= 1.0f) {
                Vertex(vc);
            } else {
                const float d3 = sx * cyl - syl * cx;
                EdgeInterior(vb, CVector{ vc.x - vb.x, vc.y - vb.y, vc.z - vb.z }, t, d3, bc2);
            }
        } else { // Borde AC (c1 y c3)
            const float ac2 = cx * cx + cy * cy;
            const float t   = (sx * cx + sy * cy) / ac2;
            if (t <= 0.0f) {
                Vertex(va);
            } else if (t >= 1.0f) {
                Vertex(vc);
            } else {
                const float d2 = sx * cy - sy * cx;
                EdgeInterior(va, vac, t, d2, ac2);
            }
        }
    }

    const float distSq = dist * dist;
    if (!(dist < sphere.m_fRadius)) {
        return false;
    }
    if (!(distSq < maxTouchDistance)) {
        return false;
    }

    CVector normal{ c.x - ip.x, c.y - ip.y, c.z - ip.z };
    normal.Normalise();

    colPoint.m_vecPoint  = ip;
    colPoint.m_vecNormal = normal;

    // Como el original: superficie A completa (3 bytes); B solo material y pieza 0 (la iluminacion B no se escribe)
    colPoint.m_nSurfaceTypeA = sphere.m_Surface.m_nMaterial;
    colPoint.m_nPieceTypeA   = sphere.m_Surface.m_nPiece;
    colPoint.m_nLightingA    = sphere.m_Surface.m_nLighting;
    colPoint.m_nSurfaceTypeB = tri.m_nMaterial;
    colPoint.m_nPieceTypeB   = 0;

    colPoint.m_fDepth = sphere.m_fRadius - dist;
    maxTouchDistance  = distSq;

    return true;
}

namespace ColWorld59c {
// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE (x: z, y, x; y: z, x, y; z: z, x, y)
inline CVector MulPoint(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: Multiply3x3 (0x59C790), igual que el anterior sin la posicion
inline CVector MulVector(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y
    };
}
} // namespace ColWorld59c

// 0x417730
bool CCollision::TestLineOfSight(
    const CColLine& lnws,
    const CMatrix& transform,
    CColModel& cm,
    bool doSeeThroughCheck,
    bool doShootThroughCheck
) {
    ZoneScoped;

    const auto cd = cm.GetData();
    if (!cd) {
        return false;
    }

    // Como el original: la linea se pasa a espacio objeto con Invert + MultiplyMatrixWithVector (0x59B920 / 0x59C890)
    const CMatrix invTransform = Invert(transform);
    const CColLine lnos{
        ColWorld59c::MulPoint(invTransform, lnws.m_vecStart),
        ColWorld59c::MulPoint(invTransform, lnws.m_vecEnd)
    };

    // Como el original: la caja envolvente y las cajas se prueban con TestLineBox_DW (0x412C70)
    if (!TestLineBox_DW(lnos, cm.GetBoundingBox())) {
        return false;
    }

    // Como el original: se OMITE la superficie cuando la comprobacion esta activa y la superficie es traspasable
    // (IsSeeThrough 0x55E6B0 / IsShootThrough 0x55E6D0); antes estaba invertido.
    const auto ShouldTest = [=](uint32 surf) {
        return (!doSeeThroughCheck || !g_surfaceInfos.IsSeeThrough(surf))
            && (!doShootThroughCheck || !g_surfaceInfos.IsShootThrough(surf));
    };

    // Como el original: contadores con signo (movsx / jle) y recorrido ascendente
    const auto numSpheres = static_cast<int16>(cd->m_nNumSpheres);
    for (int32 i = 0; i < numSpheres; i++) {
        const auto& sphere = cd->m_pSpheres[i];
        if (ShouldTest(sphere.m_Surface.m_nMaterial) && TestLineSphere(lnos, sphere)) {
            return true;
        }
    }

    const auto numBoxes = static_cast<int16>(cd->m_nNumBoxes);
    for (int32 i = 0; i < numBoxes; i++) {
        const auto& box = cd->m_pBoxes[i];
        if (ShouldTest(box.m_Surface.m_nMaterial) && TestLineBox_DW(lnos, box)) {
            return true;
        }
    }

    // Lastly, check triangles
    CalculateTrianglePlanes(cd);
    const auto numTris = static_cast<int16>(cd->m_nNumTriangles);
    for (int32 i = 0; i < numTris; i++) {
        const auto& tri = cd->m_pTriangles[i];
        if (ShouldTest(tri.m_nMaterial) && TestLineTriangle(lnos, cd->m_pVertices, tri, cd->m_pTrianglePlanes[i])) {
            return true;
        }
    }

    // No intersection whatsovever
    return false;
}

// 0x417950
bool CCollision::ProcessLineOfSight(const CColLine& lnws, const CMatrix& transform, CColModel& colModel, CColPoint& colPoint, float& maxTouchDistance, bool doSeeThroughCheck,
                                    bool doShootThroughCheck) {
    ZoneScoped;

    // Como el original: sin assert; con datos nulos devuelve falso
    const auto colData = colModel.m_pColData;
    if (!colData)
        return false;

    // Como el original: Invert (0x59B920) y MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
    const CMatrix invertedTransform = Invert(const_cast<CMatrix&>(transform)); // Hack: cast away constness. TODO: Fix-up constness of CMatrix methods

    // Transform lime into object space
    const CColLine line_OS = {
        ColWorld59c::MulPoint(invertedTransform, lnws.m_vecStart),
        ColWorld59c::MulPoint(invertedTransform, lnws.m_vecEnd),
    };

    if (!TestLineBox_DW(line_OS, colModel.GetBoundingBox()))
        return false;

    const auto CheckSeeAndShootThrough = [=](auto material) {
        return (!doSeeThroughCheck || !g_surfaceInfos.IsSeeThrough(material)) && (!doShootThroughCheck || !g_surfaceInfos.IsShootThrough(material));
    };

    float localMinTouchDist = maxTouchDistance;

    // Como el original: contadores con signo (movsx / jle)
    const auto numSpheres = static_cast<int16>(colData->m_nNumSpheres);
    for (int32 i = 0; i < numSpheres; i++) {
        if (const auto& sphere = colData->m_pSpheres[i]; CheckSeeAndShootThrough(sphere.m_Surface.m_nMaterial)) {
            ProcessLineSphere(line_OS, sphere, colPoint, localMinTouchDist);
        }
    }

    const auto numBoxes = static_cast<int16>(colData->m_nNumBoxes);
    for (int32 i = 0; i < numBoxes; i++) {
        if (const auto& box = colData->m_pBoxes[i]; CheckSeeAndShootThrough(box.m_Surface.m_nMaterial)) {
            ProcessLineBox(line_OS, box, colPoint, localMinTouchDist);
        }
    }

    CalculateTrianglePlanes(colData);

    const auto numTris = static_cast<int16>(colData->m_nNumTriangles);
    for (int32 i = 0; i < numTris; i++) {
        if (const auto& tri = colData->m_pTriangles[i]; CheckSeeAndShootThrough(tri.m_nMaterial)) {
            // Como el original: el contador de cruces solo sube cuando el triangulo colisiona
            if (ProcessLineTriangle(line_OS, colData->m_pVertices, tri, colData->m_pTrianglePlanes[i], colPoint, localMinTouchDist, nullptr)) {
                ms_iProcessLineNumCrossings++;
            }
        }
    }

    // Como el original: fcomp + test ah,5 + jp: cierto solo si local < max (NaN falso)
    if (localMinTouchDist < maxTouchDistance) {
        colPoint.m_vecPoint  = ColWorld59c::MulPoint(transform, colPoint.m_vecPoint);
        colPoint.m_vecNormal = ColWorld59c::MulVector(transform, colPoint.m_vecNormal);
        maxTouchDistance = localMinTouchDist;
        return true;
    }
    return false;
}

// 0x417BF0
bool CCollision::ProcessVerticalLine(
    const CColLine& lnws,
    const CMatrix& transform,
    CColModel& cm,
    CColPoint& cp,
    float& maxTouchDistance,
    bool doSeeThroughCheck,
    bool doShootThroughCheck,
    CStoredCollPoly* outColPoly
) {
    ZoneScoped;

    const auto cd = cm.GetData();
    if (!cd) {
        return false;
    }

    // Como el original (0x417BF0): la linea pasa al espacio del objeto como (v - pos) . ejes, con el orden de sumas
    // del EXE (x, z, y), y NO se fuerza a que sea vertical; el filtro es "saltar las superficies que se ven a traves"
    const auto ToObject = [&](const CVector& v) {
        const float dx = v.x - transform.GetPosition().x;
        const float dy = v.y - transform.GetPosition().y;
        const float dz = v.z - transform.GetPosition().z;
        return CVector{
            (dx * transform.GetRight().x   + dz * transform.GetRight().z)   + dy * transform.GetRight().y,
            (dx * transform.GetForward().x + dz * transform.GetForward().z) + dy * transform.GetForward().y,
            (dx * transform.GetUp().x      + dz * transform.GetUp().z)      + dy * transform.GetUp().y
        };
    };
    CColLine lnos;
    lnos.m_vecStart = ToObject(lnws.m_vecStart);
    lnos.m_vecEnd   = ToObject(lnws.m_vecEnd);

    if (!TestLineBox(lnos, cm.GetBoundingBox())) {
        return false;
    }

    float localMaxTouchDist = maxTouchDistance;

    const auto IsSkipped = [&](eSurfaceType surf) {
        return doSeeThroughCheck && g_surfaceInfos.IsSeeThrough(surf);
    };

    for (int32 i = 0, num = (int16)cd->m_nNumSpheres; i < num; i++) {
        if (!IsSkipped(cd->m_pSpheres[i].GetSurfaceType())) {
            ProcessLineSphere(lnos, cd->m_pSpheres[i], cp, localMaxTouchDist);
        }
    }
    for (int32 i = 0, num = (int16)cd->m_nNumBoxes; i < num; i++) {
        if (!IsSkipped(cd->m_pBoxes[i].GetSurfaceType())) {
            ProcessLineBox(lnos, cd->m_pBoxes[i], cp, localMaxTouchDist);
        }
    }

    // Lastly, triangles (el EXE usa un CStoredCollPoly estatico cuya marca `valid` se pone a 0 antes del bucle)
    CalculateTrianglePlanes(cd);
    CStoredCollPoly storedColPoly;
    std::memset(&storedColPoly, 0, sizeof(storedColPoly));
    for (int32 i = 0, num = (int16)cd->m_nNumTriangles; i < num; i++) {
        if (!IsSkipped(cd->m_pTriangles[i].GetSurfaceType())) {
            ProcessLineTriangle(lnos, cd->m_pVertices, cd->m_pTriangles[i], cd->m_pTrianglePlanes[i], cp, localMaxTouchDist, &storedColPoly);
        }
    }

    if (!(localMaxTouchDist < maxTouchDistance)) {
        return false; // No collisions closer to line origin than originally
    }

    // Transform back from object space (MultiplyMatrixWithVector / Multiply3x3 del EXE)
    cp.m_vecPoint  = ColProc59b::MultiplyMatrixWithVector(transform, cp.m_vecPoint);
    cp.m_vecNormal = ColProc59b::Multiply3x3(transform, cp.m_vecNormal);

    if (storedColPoly.valid && outColPoly) {
        std::memcpy(outColPoly, &storedColPoly, sizeof(CStoredCollPoly)); // rep movs de 0x2C bytes
        for (auto& vtx : outColPoly->verts) {
            vtx = ColProc59b::MultiplyMatrixWithVector(transform, vtx);
        }
    }

    maxTouchDistance = localMaxTouchDist;
    return true;
}

// 0x417F20
bool CCollision::SphereCastVsSphere(const CColSphere& spA, const CColSphere& spB, const CColSphere& spS) {
    ZoneScoped;

    if (TestSphereSphere(spA, spS) || TestSphereSphere(spB, spS)) {
        return true;
    }
    return CCollision::TestLineSphere(
        { spA.m_vecCenter, spB.m_vecCenter },
        { spS.m_vecCenter, spS.m_fRadius + spA.m_fRadius }
    );
}

// 0x418100 // unused
void CCollision::ClosestPointsOnPoly(CColTriangle* /*tri*/, CVector* verts, CVector* point, CVector* out) {
    ZoneScoped;

    // Como el original (0x418100): punto mas cercano de cada arista (v1-v0, v2-v1, v0-v2) a `point`
    ClosestPointOnLine(*point, verts[1], verts[0], out[0]);
    ClosestPointOnLine(*point, verts[2], verts[1], out[1]);
    ClosestPointOnLine(*point, verts[0], verts[2], out[2]);
}

// 0x418150 // unused
void CCollision::ClosestPointOnPoly(CColTriangle* /*tri*/, CVector* verts, CVector* point) {
    ZoneScoped;

    // Como el original (0x418150): puntos mas cercanos de las 3 aristas y despues el mas cercano de los 3 (modifica `point`)
    CVector out[3];
    ClosestPointOnLine(*point, verts[1], verts[0], out[0]);
    ClosestPointOnLine(*point, verts[2], verts[1], out[1]);
    ClosestPointOnLine(*point, verts[0], verts[2], out[2]);
    Closest3(out, point);
}

// 0x418580
void CCollision::CalculateTrianglePlanes(CColModel* colModel) {
    ZoneScoped;

    plugin::Call<0x418580, CColModel*>(colModel);
    if (colModel->m_pColData && colModel->m_pColData->m_pTriangles) {
        assert(colModel->m_pColData->m_pTrianglePlanes); // If model has triangles it should also have triPls by now (otherwise random crashes will occour)
    }
}

// 0x4185A0
void CCollision::RemoveTrianglePlanes(CColModel* colModel) {
    plugin::Call<0x4185A0, CColModel*>(colModel);
}

// TODO: This function could be refactored to use ranges instead of these ugly static variables :D
/*!
 * @brief 0x4185C0 Calculate collisions between \a cmA and \a cmB.
 *
 * Collisions calculated:
 *  A\B  Tri Sp  Box Lines
 * Tri       +
 * Sp    +   +   +
 * Box       +
 * Lines +   +   +
 *
 * Note: Originally the game calculated some disk stuff as well, but SA doesn'maxTouchDist use disks (they're disabled when loadedin CFileLoader)
 * thus I omitted the code for it (it would've made the already messy code even worse)
 *
 * @param         transformA           Transformation matrix of model A - Usually owning entity's matrix.
 * @param         cmA                  Col model A
 * @param         transformB           Transformation matrix of model B - Usually owning entity's matrix.
 * @param         cmA                  Col model B
 * @param[out]    lineCPs              Line collision points (At most 16 - It can be null if you're sure the model has no lines)
 * @param[out]    sphereCPs            Sphere collision points (At most 32)
 * @param[in,out] maxTouchDistances    Only used if model has lines - If you're sure it has none it can be null. It has to be an array of the same size as the number of lines .
 * @param         bReturnAllCollisions              
 *
 * @returns Number of sphere collision points found (At most 31 = `std::size(sphereCPs) - 1`; each
 *          accepted collision also pre-arms the *next* array slot's `m_fDepth` to `-1.f` as a
 *          "not-yet-written" sentinel, so one trailing index must always stay writeable)
 */
int32 CCollision::ProcessColModels(const CMatrix& transformA, CColModel& cmA,
    const CMatrix& transformB, CColModel& cmB,
    std::array<CColPoint, 32>& sphereCPs,
    CColPoint* lineCPs,
    float* maxTouchDistances,
    bool bReturnAllCollisions
) {
    ZoneScoped;

    using namespace ColProc59b;
    using ColProc59b::MultiplyMatrixWithVector;
    using ColProc59b::Multiply3x3;

    // Como el original (0x4185C0): transcripcion del flujo del EXE, incluidas sus rarezas (ver comentarios).
    // Las transformaciones usan MultiplyMatrixWithVector (0x59C890) y Multiply3x3 (0x59C790) con su orden de sumas.
    if (!cmA.m_pColData) {
        return 0;
    }
    if (!cmB.m_pColData) {
        return 0;
    }

    CCollisionData& cdA = *cmA.m_pColData;
    CCollisionData& cdB = *cmB.m_pColData;

    // 1e24f del EXE (0x6753C21C)
    constexpr float BIG = std::bit_cast<float>(0x6753C21Cu);

    // Maximo de puntos de esfera que devuelve el EXE: 31 (el siguiente hueco se arma con profundidad -1)
    constexpr int32 MAX_CPS = 31;

    const auto Transform = [](const CMatrix& m, CColPoint& cp) {
        cp.m_vecPoint  = ColProc59b::MultiplyMatrixWithVector(m, cp.m_vecPoint);
        cp.m_vecNormal = ColProc59b::Multiply3x3(m, cp.m_vecNormal);
    };

    // Matriz de A a B y esfera envolvente de A en el espacio de B
    const CMatrix transformAtoB = Invert(transformB) * transformA;

    const CColSphere colABoundSphereSpaceB{ ColProc59b::MultiplyMatrixWithVector(transformAtoB, cmA.m_boundSphere.m_vecCenter), cmA.m_boundSphere.m_fRadius };

    if (!TestSphereBox(colABoundSphereSpaceB, cmB.m_boundBox)) {
        return 0;
    }

    // Matriz de B a A
    const CMatrix transformBtoA = Invert(transformA) * transformB;

    // Tamanos de los arrays estaticos del EXE
    constexpr auto MAX_SPHERES{ 128u };
    constexpr auto MAX_BOXES{ 64u };
    constexpr auto MAX_TRIS{ 600u };
    constexpr auto MAX_LINES{ 16u };

    static CColSphere sphA[MAX_SPHERES];
    static uint32     collSphA[MAX_SPHERES];
    static CColSphere sphB[MAX_SPHERES];
    static uint32     collSphB[MAX_SPHERES];
    static uint32     collBoxB[MAX_BOXES];
    static uint32     collTrisB[MAX_TRIS];
    static uint32     collTriA[MAX_TRIS];
    static uint32     collBoxA[MAX_BOXES];
    static bool       lineHit[MAX_LINES];

    const auto CopySurface = [](CColSurface& dst, const CColSurface& src) { // El EXE copia 3 bytes (material, pieza, iluminacion)
        dst.m_nMaterial = src.m_nMaterial;
        dst.m_nPiece    = src.m_nPiece;
        dst.m_nLighting = src.m_nLighting;
    };

    // Esferas de A transformadas al espacio de B y las que chocan con la caja envolvente de B
    {
        const int32 num = (int16)cdA.m_nNumSpheres;
        for (int32 i = 0; i < num; i++) {
            sphA[i].m_fRadius   = cdA.m_pSpheres[i].m_fRadius;
            sphA[i].m_vecCenter = ColProc59b::MultiplyMatrixWithVector(transformAtoB, cdA.m_pSpheres[i].m_vecCenter);
            CopySurface(sphA[i].m_Surface, cdA.m_pSpheres[i].m_Surface);
        }
    }
    uint32 numCollSphA = 0;
    for (int32 i = 0, num = (int16)cdA.m_nNumSpheres; i < num; i++) {
        if (TestSphereBox(sphA[i], cmB.m_boundBox)) {
            collSphA[numCollSphA++] = i;
        }
    }

    // Esferas de B transformadas al espacio de A y las que chocan con la caja envolvente de A
    {
        const int32 num = (int16)cdB.m_nNumSpheres;
        for (int32 i = 0; i < num; i++) {
            sphB[i].m_fRadius   = cdB.m_pSpheres[i].m_fRadius;
            sphB[i].m_vecCenter = ColProc59b::MultiplyMatrixWithVector(transformBtoA, cdB.m_pSpheres[i].m_vecCenter);
            CopySurface(sphB[i].m_Surface, cdB.m_pSpheres[i].m_Surface);
        }
    }
    uint32 numCollSphB = 0;
    for (int32 i = 0, num = (int16)cdB.m_nNumSpheres; i < num; i++) {
        if (TestSphereBox(sphB[i], cmA.m_boundBox)) {
            collSphB[numCollSphB++] = i;
        }
    }

    if (!numCollSphA && !cdA.m_nNumLines && !numCollSphB) {
        return 0;
    }

    // Discos de B (0x418902): se transforman a sphB[numSpheres + i] y los que chocan con la caja de A se anotan en
    // collSphB EMPEZANDO POR EL PRINCIPIO (con el indice del disco, no numSpheres + i) y sin tocar numCollSphB.
    // Es una rareza del EXE (pisa las esferas normales que hubieran chocado); se conserva tal cual.
    uint32 numDiskB = 0;
    if (cdB.bUsesDisks && cdB.m_nNumLines) {
        const int32 numDisks   = (int8)cdB.m_nNumLines;
        const int32 numSpheres = (int16)cdB.m_nNumSpheres;
        for (int32 i = 0; i < numDisks; i++) {
            const CColDisk& disk = cdB.m_pDisks[i];
            sphB[numSpheres + i].m_fRadius   = disk.m_fRadius;
            sphB[numSpheres + i].m_vecCenter = ColProc59b::MultiplyMatrixWithVector(transformBtoA, disk.m_vecCenter);
            CopySurface(sphB[numSpheres + i].m_Surface, disk.m_Surface);
        }
        uint32 listPos = 0;
        for (int32 i = 0; i < numDisks; i++) {
            if (TestSphereBox(sphB[numSpheres + i], cmA.m_boundBox)) {
                collSphB[listPos++] = i;
                numDiskB++;
                if (listPos >= MAX_SPHERES) {
                    break;
                }
            }
        }
    }

    // Cajas de B que chocan con la esfera envolvente de A
    uint32 numCollBoxB = 0;
    for (int32 i = 0, num = (int16)cdB.m_nNumBoxes; i < num; i++) {
        if (TestSphereBox(colABoundSphereSpaceB, cdB.m_pBoxes[i])) {
            collBoxB[numCollBoxB++] = i;
            if (numCollBoxB >= MAX_BOXES) {
                break;
            }
        }
    }

    // Triangulos de B que chocan con la esfera envolvente de A (el EXE llama siempre a CalculateTrianglePlanes y corta a 599)
    CalculateTrianglePlanes(&cdB);
    uint32 numCollTrisB = 0;
    constexpr uint32 TRI_LIMIT = 0x257;
    if ((int16)cdB.m_nNumTriangles != 0) {
        if (cdB.bHasFaceGroups) {
            // Los grupos se recorren desde el que esta justo antes del contador hacia abajo (orden del EXE)
            const uint32 numGroups = cdB.GetNumFaceGroups();
            for (uint32 g = 0; (int32)g < (int32)numGroups; g++) {
                const auto& group = *reinterpret_cast<const ColHelpers::TFaceGroup*>(reinterpret_cast<const uint8*>(cdB.m_pTriangles) - 0x20 - g * 0x1C);
                if (TestSphereBox(colABoundSphereSpaceB, group.bb)) {
                    for (int32 t = (int16)group.first; t <= (int16)group.last; t++) {
                        if (TestSphereTriangle(colABoundSphereSpaceB, cdB.m_pVertices, cdB.m_pTriangles[t], cdB.m_pTrianglePlanes[t])) {
                            collTrisB[numCollTrisB++] = t;
                            if (numCollTrisB >= TRI_LIMIT) {
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            for (int32 t = 0, num = (int16)cdB.m_nNumTriangles; t < num; t++) {
                if (TestSphereTriangle(colABoundSphereSpaceB, cdB.m_pVertices, cdB.m_pTriangles[t], cdB.m_pTrianglePlanes[t])) {
                    collTrisB[numCollTrisB++] = t;
                    if (numCollTrisB >= TRI_LIMIT) {
                        break;
                    }
                }
            }
        }
    }

    if (!numCollSphB && !numCollBoxB && !numCollTrisB) {
        return 0;
    }

    // Esferas de A contra esferas, cajas y triangulos de B
    sphereCPs[0].m_fDepth = -1.f;
    int32 n = 0; // Numero de puntos de esfera
    for (uint32 a = 0; a < numCollSphA; a++) {
        const uint32      aIdx  = collSphA[a];
        const CColSphere& origA = cdA.m_pSpheres[aIdx];
        const CColSphere& sphereA = sphA[aIdx]; // En el espacio de B

        bool  advance = false;
        float minTouchDist = BIG;

        // 0x418CF9 - Esferas (los indices >= numSpheres son discos; sin discos se saltan)
        for (uint32 j = 0; j < numCollSphB; j++) {
            const int32 idx = (int32)collSphB[j];
            const CColSphere* sphereB;
            if (idx < (int16)cdB.m_nNumSpheres) {
                sphereB = &cdB.m_pSpheres[idx];
            } else if (cdB.bUsesDisks && cdB.m_nNumLines) {
                sphereB = &cdB.m_pDisks[idx - (int16)cdB.m_nNumSpheres];
            } else {
                continue;
            }
            if (ProcessSphereSphere(sphereA, *sphereB, sphereCPs[n], minTouchDist)) {
                advance = true;
            }
        }

        // 0x418D86 - Cajas
        for (uint32 j = 0; j < numCollBoxB; j++) {
            auto& cp = sphereCPs[n];
            if (ProcessSphereBox(sphereA, cdB.m_pBoxes[collBoxB[j]], cp, minTouchDist)) {
                CopySurfaceA(cp, origA.m_Surface);
                if (bReturnAllCollisions && origA.m_Surface.m_nPiece <= 2 && n < MAX_CPS) {
                    n++;
                    advance      = false;
                    minTouchDist = BIG;
                    sphereCPs[n].m_fDepth = -1.f;
                } else {
                    advance = true;
                }
            }
        }

        // 0x418E44 - Triangulos
        for (uint32 j = 0; j < numCollTrisB; j++) {
            auto& cp = sphereCPs[n];
            const uint32 t = collTrisB[j];
            if (ProcessSphereTriangle(sphereA, cdB.m_pVertices, cdB.m_pTriangles[t], cdB.m_pTrianglePlanes[t], cp, minTouchDist)) {
                if (bReturnAllCollisions && origA.m_Surface.m_nPiece <= 2 && n < MAX_CPS) {
                    n++;
                    advance      = false;
                    minTouchDist = BIG;
                    sphereCPs[n].m_fDepth = -1.f;
                } else {
                    advance = true;
                }
            }
        }

        // 0x418F02
        if (advance) {
            if (n >= MAX_CPS) {
                break;
            }
            n++;
            sphereCPs[n].m_fDepth = -1.f;
        }
    }

    // Lineas (o discos) de A contra esferas, cajas y triangulos de B
    if (cdA.bUsesDisks) { // 0x418F44
        const int32 numDisks = (int8)cdA.m_nNumLines;

        // El EXE escribe la distancia de entrada del llamante en disk.m_vecCenter.z del modelo VIVO y no la restaura
        for (int32 i = 0; i < numDisks; i++) {
            cdA.m_pDisks[i].m_vecCenter.z = maxTouchDistances[i];
        }

        // Los discos de A se transforman al espacio de B en sphA[i] (pisando las esferas ya procesadas)
        for (int32 i = 0; i < numDisks; i++) {
            const CColDisk& disk = cdA.m_pDisks[i];
            sphA[i].m_fRadius   = disk.m_fRadius;
            sphA[i].m_vecCenter = ColProc59b::MultiplyMatrixWithVector(transformAtoB, disk.m_vecCenter);
            CopySurface(sphA[i].m_Surface, disk.m_Surface);
        }

        for (int32 i = 0; i < numDisks; i++) {
            const CColDisk&   disk       = cdA.m_pDisks[i];
            const CColSphere& sphereAinB = sphA[i];
            CColPoint&        thisLineCP = lineCPs[i];
            float&            maxTouch   = maxTouchDistances[i];
            CColPoint&        cp         = sphereCPs[n];

            lineHit[i]  = false;
            cp.m_fDepth = -1.0f;

            if (TestSphereBox(sphereAinB, cmB.m_boundBox)) {
                bool  emitCP = false;
                float minTouchDist = BIG;
                float bestBoxDist  = BIG;

                const auto HitK = [&](const CVector& cpInA) {
                    const float dy = cpInA.y - disk.m_vecCenter.y;
                    const float dx = cpInA.x - disk.m_vecCenter.x;
                    return std::sqrt((disk.m_fRadius * disk.m_fRadius - dy * dy) - dx * dx) + cpInA.z;
                };
                const auto StoreLine = [&](float hitK) {
                    lineHit[i] = true;
                    maxTouch   = hitK;
                    CopyColPoint(thisLineCP, cp);
                    CopySurfaceA(thisLineCP, disk.m_Surface);
                };

                // Esferas: solo producen contacto de linea (siempre sobre las esferas normales de B)
                for (uint32 j = 0; j < numCollSphB; j++) {
                    const CColSphere& sphereB = cdB.m_pSpheres[collSphB[j]];
                    if (ProcessSphereSphere(sphereAinB, sphereB, cp, minTouchDist)) {
                        const CVector cpInA = ColProc59b::MultiplyMatrixWithVector(transformBtoA, cp.m_vecPoint);
                        const float   hitK  = HitK(cpInA);
                        if (!(hitK < maxTouch)) {
                            StoreLine(hitK);
                        }
                        minTouchDist = BIG;
                    }
                }

                // Cajas
                for (uint32 j = 0; j < numCollBoxB; j++) {
                    if (ProcessSphereBox(sphereAinB, cdB.m_pBoxes[collBoxB[j]], cp, minTouchDist)) {
                        const CVector cpInA     = ColProc59b::MultiplyMatrixWithVector(transformBtoA, cp.m_vecPoint);
                        const CVector normalInA = ColProc59b::Multiply3x3(transformBtoA, cp.m_vecNormal);

                        if (!(cpInA.z < disk.m_vecCenter.z) || !(normalInA.z > 0.5f)) {
                            if (minTouchDist < bestBoxDist) {
                                bestBoxDist = minTouchDist;
                                CopySurfaceA(cp, disk.m_Surface);
                                emitCP = true;
                            }
                        } else {
                            const float hitK = HitK(cpInA);
                            if (!(hitK < maxTouch)) {
                                if (0.35f * disk.m_fRadius < hitK - maxTouch) {
                                    bestBoxDist = minTouchDist;
                                    CopySurfaceA(cp, disk.m_Surface);
                                    emitCP = true;
                                }
                                StoreLine(hitK);
                            }
                        }
                    }
                    minTouchDist = BIG;
                }

                // Triangulos: triCP se copia UNA vez (rep movs) y despues solo se reinicia la profundidad
                CColPoint triCP;
                std::memcpy(&triCP, &cp, sizeof(CColPoint));
                for (uint32 j = 0; j < numCollTrisB; j++) {
                    const uint32 t = collTrisB[j];
                    triCP.m_fDepth = -1.0f;
                    if (ProcessSphereTriangle(sphereAinB, cdB.m_pVertices, cdB.m_pTriangles[t], cdB.m_pTrianglePlanes[t], triCP, minTouchDist)) {
                        const CVector planeNormal = cdB.m_pTrianglePlanes[t].GetNormal();
                        if (planeNormal.z > 0.3f && transformB.GetUp().z > 0.9f) {
                            triCP.m_vecNormal = planeNormal;
                        }
                        if (ProcessDiscCollision(triCP, transformBtoA, disk, cp, lineHit[i], maxTouch, thisLineCP)) {
                            emitCP = true;
                        }
                    }
                }

                if (emitCP && n < MAX_CPS) {
                    n++;
                    sphereCPs[n].m_fDepth = -1.f;
                }

                if (lineHit[i]) {
                    Transform(transformB, thisLineCP);
                } else {
                    maxTouch = -1e8f;
                }
            }
        }
        // Nota: el EXE no restaura disk.m_vecCenter.z
    } else { // 0x4196B9
        const int32 numLines = (int8)cdA.m_nNumLines;
        static CColLine linesB[MAX_LINES];
        for (int32 i = 0; i < numLines; i++) {
            linesB[i].m_vecStart = ColProc59b::MultiplyMatrixWithVector(transformAtoB, cdA.m_pLines[i].m_vecStart);
            linesB[i].m_vecEnd   = ColProc59b::MultiplyMatrixWithVector(transformAtoB, cdA.m_pLines[i].m_vecEnd);
        }
        for (int32 i = 0; i < numLines; i++) {
            lineHit[i] = false;
            auto& thisLineCP = lineCPs[i];
            auto& touchDist  = maxTouchDistances[i];

            for (uint32 j = 0; j < numCollSphB; j++) { // 0x419799
                if (ProcessLineSphere(linesB[i], cdB.m_pSpheres[collSphB[j]], thisLineCP, touchDist)) {
                    lineHit[i] = true;
                }
            }
            for (uint32 j = 0; j < numCollBoxB; j++) { // 0x419803
                if (ProcessLineBox(linesB[i], cdB.m_pBoxes[collBoxB[j]], thisLineCP, touchDist)) {
                    lineHit[i] = true;
                }
            }
            for (uint32 j = 0; j < numCollTrisB; j++) { // 0x419865
                const uint32 t = collTrisB[j];
                if (ProcessLineTriangle(linesB[i], cdB.m_pVertices, cdB.m_pTriangles[t], cdB.m_pTrianglePlanes[t], thisLineCP, touchDist, nullptr)) {
                    lineHit[i] = true;
                }
            }
            if (lineHit[i]) { // 0x4198DA
                Transform(transformB, thisLineCP);
            }
        }
    }

    // 0x419968 - Todos los puntos de esfera (tambien los emitidos por los discos) pasan al espacio del mundo
    for (int32 k = 0; k < n; k++) {
        Transform(transformB, sphereCPs[k]);
    }

    // 0x4199D5 - Triangulos y cajas de A contra las esferas de B
    int32 numAdded = 0;
    if (numCollSphB > 0 && (cdA.m_nNumTriangles || cdA.m_nNumBoxes)) {
        const CVector colBCenterInA = ColProc59b::MultiplyMatrixWithVector(transformBtoA, cmB.m_boundSphere.m_vecCenter);
        const CColSphere colBSphereInASpace{ colBCenterInA, cmB.m_boundSphere.m_fRadius };

        uint32 numCollTriA = 0;
        if (cdA.m_nNumTriangles) {
            if (cmA.m_pColData) {
                CalculateTrianglePlanes(cmA.m_pColData);
            }
            for (int32 t = 0, num = (int16)cdA.m_nNumTriangles; t < num; t++) {
                if (TestSphereTriangle(colBSphereInASpace, cdA.m_pVertices, cdA.m_pTriangles[t], cdA.m_pTrianglePlanes[t])) {
                    collTriA[numCollTriA++] = t;
                    if (numCollTriA >= TRI_LIMIT) {
                        break;
                    }
                }
            }
        }

        // Cajas de A (el EXE no limita aqui el numero de cajas: desbordaria sus arrays; se limita a 64 por seguridad)
        uint32 numCollBoxA = 0;
        for (int32 i = 0, num = (int16)cdA.m_nNumBoxes; i < num; i++) {
            if (TestSphereBox(colBSphereInASpace, cdA.m_pBoxes[i])) {
                if (numCollBoxA < MAX_BOXES) {
                    collBoxA[numCollBoxA++] = i;
                }
            }
        }

        const int32 numRealSphB = (int32)numCollSphB - (int32)numDiskB;

        // 0x419B76 - Esferas de B contra triangulos de A (solo si hay triangulos que chocan)
        if (numCollTriA) {
            sphereCPs[n].m_fDepth = -1.f;
            bool stop = false;
            for (int32 k = 0; k < numRealSphB && !stop; k++) {
                const CColSphere& sphereB = sphB[collSphB[k]];
                auto&             cp      = sphereCPs[n];
                float             minTouchDist = BIG;
                bool              any = false;
                for (uint32 j = 0; j < numCollTriA; j++) {
                    const uint32 t = collTriA[j];
                    if (ProcessSphereTriangle(sphereB, cdA.m_pVertices, cdA.m_pTriangles[t], cdA.m_pTrianglePlanes[t], cp, minTouchDist)) {
                        any = true;
                    }
                }
                if (any) {
                    cp.m_vecNormal.x = cp.m_vecNormal.x * -1.0f;
                    cp.m_vecNormal.y = cp.m_vecNormal.y * -1.0f;
                    cp.m_vecNormal.z = cp.m_vecNormal.z * -1.0f;
                    if (n >= MAX_CPS) {
                        stop = true;
                        break;
                    }
                    n++;
                    numAdded++;
                    sphereCPs[n].m_fDepth = -1.f;
                }
            }
        }

        // 0x419CBE - Esferas de B contra cajas de A
        if (numCollBoxA && numRealSphB > 0) {
            for (int32 k = 0; k < numRealSphB; k++) {
                const uint32      sIdx    = collSphB[k];
                const CColSphere& origB   = cdB.m_pSpheres[sIdx];
                float             minTouchDist = BIG;
                for (uint32 j = 0; j < numCollBoxA; j++) {
                    auto& cp = sphereCPs[n];
                    const auto& bb = cdA.m_pBoxes[collBoxA[j]];
                    if (ProcessSphereBox(sphB[sIdx], bb, cp, minTouchDist)) {
                        CopySurfaceA(cp, bb.m_Surface);
                        CopySurfaceB(cp, origB.m_Surface);
                        cp.m_vecNormal.x = cp.m_vecNormal.x * -1.0f;
                        cp.m_vecNormal.y = cp.m_vecNormal.y * -1.0f;
                        cp.m_vecNormal.z = cp.m_vecNormal.z * -1.0f;
                        if (n >= MAX_CPS) {
                            break;
                        }
                        n++;
                        numAdded++;
                        sphereCPs[n].m_fDepth = -1.f;
                    }
                }
            }
        }
    }

    // 0x419E2C - Los puntos anadidos pasan al espacio del mundo con la matriz de A y se intercambian las superficies A y B
    for (int32 k = n - numAdded; k < n; k++) {
        auto& cp = sphereCPs[k];
        Transform(transformA, cp);
        std::swap(cp.m_nSurfaceTypeA, cp.m_nSurfaceTypeB);
        std::swap(cp.m_nPieceTypeA, cp.m_nPieceTypeB);
        std::swap(cp.m_nLightingA, cp.m_nLightingB);
    }

    return n;
}


// 0x414D70
bool CCollision::IsStoredPolyStillValidVerticalLine(const CVector& lineOrigin, float lnMag, CColPoint& colPoint, CStoredCollPoly* collPoly) {
    ZoneScoped;

    if (!collPoly->valid) {
        return false;
    }

    // Como el original (0x414D70): normal exacta = normalizada((v2 - v0) x (v1 - v0)) y el extremo de la linea es
    // (origen.x, origen.y, lnMag) (el parametro es la z final, no un factor)
    const CVector& v0 = collPoly->verts[0];
    const CVector& v1 = collPoly->verts[1];
    const CVector& v2 = collPoly->verts[2];

    const CVector e1{ v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
    const CVector e2{ v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
    CVector n{
        e1.z * e2.y - e2.z * e1.y,
        e2.z * e1.x - e1.z * e2.x,
        e2.x * e1.y - e1.x * e2.y
    };
    n.Normalise();

    const float d = (n.z * v0.z + n.y * v0.y) + n.x * v0.x;

    uint8 orientation;
    const float ax = n.x < 0.0f ? -n.x : n.x;
    const float ay = n.y < 0.0f ? -n.y : n.y;
    const float az = n.z < 0.0f ? -n.z : n.z;
    if (ax > ay && ax > az) {
        orientation = n.x > 0.0f ? 0 : 1;
    } else if (!(ay <= az)) {
        orientation = n.y > 0.0f ? 2 : 3;
    } else {
        orientation = n.z > 0.0f ? 4 : 5;
    }

    const float ox = lineOrigin.x, oy = lineOrigin.y, oz = lineOrigin.z;
    const float oxnx = ox * n.x;
    const float oyny = oy * n.y;
    const float oznz = oz * n.z;

    const float E = (oy * n.y + ox * n.x) + n.z * lnMag;
    const float S = (oxnx + oyny) + oznz;
    if (!((S - d) * (E - d) <= 0.0f)) {
        collPoly->valid = false;
        return false;
    }

    const float dx  = ox - ox;
    const float dy  = oy - oy;
    const float dz  = lnMag - oz;
    const float num = ((d - oxnx) - oyny) - oznz;
    const float den = (dz * n.z + dy * n.y) + dx * n.x;
    const float t   = num / den;

    const CVector ip{ dx * t + ox, dy * t + oy, dz * t + oz };

    if (!ColProc59b::PointInTriangle2D(orientation, v0, v1, v2, ip)) {
        collPoly->valid = false;
        return false;
    }

    colPoint.m_vecPoint = ip;
    return true;
}

// 0x415230
CColBox CCollision::GetBoundingBoxFromTwoSpheres(const CColSphere& spA, const CColSphere& spB) {
    ZoneScoped;

    // Como el original (0x415230): el menor centro menos el radio de A y el mayor centro mas el radio de A
    // (si no es `A < B`, incluida la igualdad y NaN, el menor es B y el mayor A)
    const float r = spA.m_fRadius;
    CVector min, max;
    for (size_t i = 0; i < 3; i++) {
        const float a = spA.m_vecCenter[i];
        const float b = spB.m_vecCenter[i];
        if (a < b) {
            min[i] = a - r;
            max[i] = r + b;
        } else {
            min[i] = b - r;
            max[i] = r + a;
        }
    }
    return CColBox{ CBox{min, max} };
}

// 0x4152C0
bool CCollision::IsThisVehicleSittingOnMe(CVehicle* veh, CVehicle* vehOnMe) {
    ZoneScoped;

    if (!veh || !vehOnMe) {
        return false;
    }
    const auto Check = [veh](auto& wheelColEntities) {
        return notsa::contains(wheelColEntities, veh);
    };
    switch (vehOnMe->m_nVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE: return Check(vehOnMe->AsAutomobile()->m_apWheelCollisionEntity);
    case VEHICLE_TYPE_BIKE:       return Check(vehOnMe->AsBike()->m_aGroundPhysicalPtrs);
    default:                      return false;
    }
}

// Como el original: funcion 0x50D590 (sin hook propio). Anima la colision de golpe del modelo del peaton, calcula
// offset = front.source con el orden de sumas z, y, x y recorre las 12 esferas (2 bloques de 6) con
// val = ((dot - offset) - radio) - nearClip; el minimo empieza en 999999.0f (0x497423F0), no en FLT_MAX.
// El producto escalar de las esferas 0 y 1 de cada bloque suma z, x, y; el de las otras cuatro suma z, y, x.
float GetNearestDistanceOfPedSphereToCameraNearClip(CPed* ped) {
    const auto mi = ped->GetPedModelInfo();

    // Calculate hit colmodel
    mi->AnimatePedColModelSkinnedWorld(ped->GetRpClump());
    const auto hitCM = mi->m_pHitColModel;

    // Calculate some other shite
    auto&       cam     = TheCamera.GetActiveCamera();
    const auto& front   = cam.m_vecFront;
    const auto  offset  = (front.z * cam.m_vecSource.z + front.y * cam.m_vecSource.y) + front.x * cam.m_vecSource.x;
    const auto  nearClp = RwCameraGetNearClipPlane(Scene.m_pRwCamera);

    // Como el original: 12 esferas fijas, sin comprobar el numero real (no hay assert ni comprobacion de nulo)
    const auto spheres = hitCM->m_pColData->m_pSpheres;
    float ret = std::bit_cast<float>(0x497423F0u);
    for (int32 i = 0; i < 12; i++) {
        const auto& c = spheres[i].m_vecCenter;
        const float dot = (i % 6) < 2
            ? (c.z * front.z + c.x * front.x) + c.y * front.y
            : (c.z * front.z + c.y * front.y) + c.x * front.x;
        const float val = ((dot - offset) - spheres[i].m_fRadius) - nearClp;
        if (val < ret) { // fcomp + test ah,5 + jp: solo cambia si es estrictamente menor (NaN no)
            ret = val;
        }
    }
    return ret;
}

// 0x415320
bool CCollision::CheckCameraCollisionPeds(
    int32 sectorX,
    int32 sectorY,
    const CVector& pos,
    const CVector& /*unused*/,
    float& /*unused*/
) {
    ZoneScoped;

    // Como el original: el radio 1.0 (0x8CCB8C) se eleva al cuadrado una vez
    constexpr auto gPedCylinderWidth = 1.f;
    const float    pedCylinderSq     = gPedCylinderWidth * gPedCylinderWidth;

    bool addedAny = false;

    // Como el original: indice del sector repetido con `& 0xF` (las coordenadas negativas se envuelven, no se usa abs)
    auto& sector = CWorld::ms_aRepeatSectors[sectorY & 0xF][sectorX & 0xF];
    for (auto* const ped : sector.Peds) {
        if (ped->IsScanCodeCurrent()) {
            continue;
        }

        ped->SetCurrentScanCode();

        if (!ped->GetIsVisible() || CWorld::pIgnoreEntity == ped || ped->IsPlayer()) {
            continue;
        }

        // Como el original: dist2 = dy*dy + dx*dx; solo sigue si dist2 < radio^2 (NaN descarta)
        const auto bc  = ped->GetBoundCentre();
        const float dx = pos.x - bc.x;
        const float dy = pos.y - bc.y;
        if (!(dy * dy + dx * dx < pedCylinderSq)) {
            continue;
        }

        // Como el original: solo sigue si la distancia es <= 0 (fcomp con 0.0 y test ah,41h; NaN descarta)
        if (!(GetNearestDistanceOfPedSphereToCameraNearClip(ped) <= 0.f)) {
            continue;
        }

        // Como el original: se guarda y se registra la referencia sin comprobar el limite del array (std::array::data)
        auto* const slots = gpMadeInvisibleEntities.data();
        slots[gNumEntitiesSetInvisible] = ped;
        ped->RegisterReference(&slots[gNumEntitiesSetInvisible]);
        gNumEntitiesSetInvisible++;

        // Add entity the peds holds too (if any)
        // Como el original: GetTaskHold(false) (push 0), no el valor por defecto true
        if (const auto task = ped->GetIntelligence()->GetTaskHold(false)) {
            if (const auto ent = task->m_pEntityToHold) {
                if (ent->GetIsVisible()) {
                    ent->SetIsVisible(false);
                    slots[gNumEntitiesSetInvisible] = ent;
                    ent->RegisterReference(&slots[gNumEntitiesSetInvisible]);
                    gNumEntitiesSetInvisible++;
                }
            }
        }

        ped->SetIsVisible(false);
        addedAny = true;
    }
    return addedAny;
}

// 0x415540
void ResetMadeInvisibleObjects() {
    // Como el original: el contador se relee en cada vuelta (con signo), se trabaja sobre la propia casilla del array
    // (la referencia se registro con su direccion) y la casilla se pone a nulo despues de limpiarla.
    auto* const slots = gpMadeInvisibleEntities.data();
    for (int32 i = 0; i < static_cast<int32>(gNumEntitiesSetInvisible); i++) {
        if (!slots[i]) { // Must check, as the reference system might've cleared it
            continue;
        }
        slots[i]->SetIsVisible(true);
        if (slots[i]) {
            slots[i]->CleanUpOldReference(&slots[i]);
        }
        slots[i] = nullptr;
    }
    gNumEntitiesSetInvisible = 0;
}

// 0x415620
bool CCollision::RayPolyPOP(CVector* pos, CVector* dir, CColTriangle* /*unused*/, CVector* normal, CVector* planePt) {
    ZoneScoped;

    // Como el original (0x415620): mueve `pos` a lo largo de `dir` hasta el plano (normal, punto planePt); falso si
    // `pos` esta delante del plano o el rayo no lo alcanza
    const float dx = planePt->x - pos->x;
    const float dy = planePt->y - pos->y;
    const float dz = planePt->z - pos->z;

    const float dist = (dz * normal->z + dx * normal->x) + dy * normal->y;
    if (dist > 0.f) {
        return false;
    }

    const float denom = (normal->z * dir->z + normal->y * dir->y) + dir->x * normal->x;
    if (dist <= denom) {
        return false;
    }

    const float t  = dist / denom;
    const float tx = t * dir->x;
    const float ty = t * dir->y;
    const float tz = t * dir->z;
    pos->x = tx + pos->x;
    pos->y = ty + pos->y;
    pos->z = tz + pos->z;
    return true;
}

// 0x4156D0
int32 CCollision::GetPrincipleAxis(const CVector& normal) {
    ZoneScoped;

    // Como el original (0x4156D0): con NaN en y/z el EXE devuelve 4
    const float nx = std::abs(normal.x);
    const float ny = std::abs(normal.y);
    const float nz = std::abs(normal.z);
    if (nx > ny && nx > nz) {
        return 0; // X
    }
    return ny > nz
        ? 2  // Y
        : 4; // Z
}

// NOTSA
bool IsPointInPoly2D(CVector2D pt, CVector2D a, CVector2D b, CVector2D c) {
    // Code they used originally: https://stackoverflow.com/revisions/2049593/4
    // It's slower than than the one below.
    
    // Based on: https://stackoverflow.com/a/9755252
    const auto pt_a = pt - a;
    const bool s_ab = (b - a).Cross(pt_a) > 0.f;
    return (c - a).Cross(pt_a) > 0.f != s_ab && (c - b).Cross(pt - b) > 0.f == s_ab;
}

// 0x415730
bool CCollision::PointInPoly(
    const CVector& testPt,
    const CColTriangle& /*unused*/,
    const CVector& normal,
    const CVector* verts // Uncompressed vertices
) {
    ZoneScoped;

    // Como el original (0x415730): tres pruebas de lado en el plano 2D del eje principal, todas con el mismo signo.
    // Los ejes X y Z comparan con >= 0 y el eje Y con <= 0 (NaN cuenta como lado negativo en X/Z y positivo en Y).
    const CVector& v0 = verts[0];
    const CVector& v1 = verts[1];
    const CVector& v2 = verts[2];

    bool f1, f2, f3;
    switch (GetPrincipleAxis(normal)) {
    case 4: { // Z: (x, y)
        const float c1 = (testPt.x - v0.x) * (v1.y - v0.y) - (testPt.y - v0.y) * (v1.x - v0.x);
        const float c2 = (testPt.x - v1.x) * (v2.y - v1.y) - (testPt.y - v1.y) * (v2.x - v1.x);
        const float c3 = (testPt.x - v2.x) * (v0.y - v2.y) - (testPt.y - v2.y) * (v0.x - v2.x);
        f1 = c1 >= 0.f;
        f2 = c2 >= 0.f;
        f3 = c3 >= 0.f;
        break;
    }
    case 2: { // Y: (x, z)
        const float c1 = (testPt.x - v0.x) * (v1.z - v0.z) - (testPt.z - v0.z) * (v1.x - v0.x);
        const float c2 = (v2.z - v1.z) * (testPt.x - v1.x) - (testPt.z - v1.z) * (v2.x - v1.x);
        const float c3 = (testPt.x - v2.x) * (v0.z - v2.z) - (testPt.z - v2.z) * (v0.x - v2.x);
        f1 = !(c1 > 0.f) && !std::isnan(c1);
        f2 = !(c2 > 0.f) && !std::isnan(c2);
        f3 = !(c3 > 0.f) && !std::isnan(c3);
        break;
    }
    case 0: { // X: (y, z)
        const float c1 = (testPt.y - v0.y) * (v1.z - v0.z) - (v1.y - v0.y) * (testPt.z - v0.z);
        const float c2 = (testPt.y - v1.y) * (v2.z - v1.z) - (testPt.z - v1.z) * (v2.y - v1.y);
        const float c3 = (testPt.y - v2.y) * (v0.z - v2.z) - (testPt.z - v2.z) * (v0.y - v2.y);
        f1 = c1 >= 0.f;
        f2 = c2 >= 0.f;
        f3 = c3 >= 0.f;
        break;
    }
    default:
        return false; // Inalcanzable (GetPrincipleAxis solo devuelve 0, 2 o 4)
    }
    return f1 == f2 && f1 == f3;
}

// 0x415CF0
bool CCollision::SphereCastVersusVsPoly(
    const CColSphere& spA,
    const CColSphere& spB,
    const CColTriangle& tri,
    const CColTrianglePlane& triPlane,
    CompressedVector* verts
) {
    ZoneScoped;

    // Como el original: AB = B - A; |AB|^2 = (z^2 + y^2) + x^2
    const CVector& pos = spA.m_vecCenter;
    const CVector  spAToB{ spB.m_vecCenter.x - pos.x, spB.m_vecCenter.y - pos.y, spB.m_vecCenter.z - pos.z };
    const float    spAToBDistSq = (spAToB.z * spAToB.z + spAToB.y * spAToB.y) + spAToB.x * spAToB.x;

    // Como el original: la normal (int16 * 1/4096, 0x858BFC) y el termino constante (int16 * 1/128, 0x858B88) se leen en crudo
    const auto* const rawPlane = reinterpret_cast<const int16*>(&triPlane);
    const CVector     plNorm{ float(rawPlane[0]) * 0.000244140625f, float(rawPlane[1]) * 0.000244140625f, float(rawPlane[2]) * 0.000244140625f };
    const float       plOffset = float(rawPlane[3]) * 0.0078125f;

    const auto spARadius = spA.m_fRadius;

    // Como el original: producto escalar con suma z, y, x
    const float plSpCenterDist  = ((pos.z * plNorm.z + pos.y * plNorm.y) + pos.x * plNorm.x) - plOffset;
    const bool  isSpTouchingPl  = !(std::abs(plSpCenterDist) > spARadius); // fcomp + test ah,41h + jne: NaN cuenta como tocando

    // Sphere's center projected onto the plane's normal
    const float projDist = isSpTouchingPl ? plSpCenterDist : spARadius;
    CVector spAProjPl{ pos.x - plNorm.x * projDist, pos.y - plNorm.y * projDist, pos.z - plNorm.z * projDist };

    const CVector vA = verts[tri.vA];

    if (!isSpTouchingPl) {
        // Como el original: ((vz - pz) * nz + (vy - py) * ny) + (vx - px) * nx
        const float vtxAToSpDistSqOnPl = ((vA.z - spAProjPl.z) * plNorm.z + (vA.y - spAProjPl.y) * plNorm.y) + (vA.x - spAProjPl.x) * plNorm.x;
        if (vtxAToSpDistSqOnPl > 0.f) {
            return false;
        }
        const float spAToBDistSqOnPl = (plNorm.z * spAToB.z + plNorm.y * spAToB.y) + plNorm.x * spAToB.x;
        if (vtxAToSpDistSqOnPl <= spAToBDistSqOnPl) {
            return false; // If spA was closer than spB then there's no way spB would touch it, so we're finished
        }
        const float ratio = vtxAToSpDistSqOnPl / spAToBDistSqOnPl; // Interpolate between spA -> spB
        spAProjPl.x = spAToB.x * ratio + spAProjPl.x;
        spAProjPl.y = spAToB.y * ratio + spAProjPl.y;
        spAProjPl.z = spAToB.z * ratio + spAProjPl.z;
    }

    const CVector vB = verts[tri.vB], vC = verts[tri.vC];

    const CVector cverts[]{vA, vB, vC};
    if (PointInPoly(spAProjPl, tri, plNorm, cverts)) {
        return true;
    }

    const auto maxDistSq = spARadius * spARadius;
    return ClosestSquaredDistanceBetweenFiniteLines(pos, vA, vB, spAToB, spAToBDistSq) < maxDistSq
        || ClosestSquaredDistanceBetweenFiniteLines(pos, vC, vB, spAToB, spAToBDistSq) < maxDistSq
        || ClosestSquaredDistanceBetweenFiniteLines(pos, vA, vC, spAToB, spAToBDistSq) < maxDistSq;
}

/*!
* Below is used for the camera anti-clipping bullshittery.
* (When you move the camera and instead of clipping thru the
* object it slides along it's collision)
*
* Honestly, as much as I bash the devs from 2003, this one
* is quite well done (Other than using static variables xD)
*
* Definitions:
* spA - Sphere representing the camera
* spB - `spA` but it's center offset by the spBToA of the player (Basically, where the camera would be the next timestep)
* ws  - "World space"
* os  - "Object space"
* bs  - Bullshit (You probably knew this one already, extensively used in the code)
*
* TODO:
* Sometime in the distant future we should get rid of the global variables... :)
*/

//! A singular entry in the ColCache used for the anti-clipping bs
// Como el original: struct sin constructor ni valores por defecto (16 bytes: tipo, entidad y tres indices de 16 bits).
// El EXE escribe solo los campos que necesita (ver SphereCastVsEntity / SphereCastVsCaches): no se pone a cero el resto.
struct CColCacheEntry {
    enum class eType : uint8 {
        NONE,
        TRIANGLE,
        SPHERE,
        BOX,
        // BACKSIDE_TRIANGLE // TODO: Add this. Search for `BULLSHIT_DETECTOR`
    };

    eType     type;
    CEntity*  ent;
    uint16    triIdx; // Why the fuck would they use 3 ints instead of 1 for this is beyond any imgination  - TODO: Use 1 singular uint32 here, and refactor the code accordingly!
    uint16    sphIdx;
    uint16    boxIdx;
};
static_assert(sizeof(CColCacheEntry) == 0x10);
//! Value should be in range (0, 1] (Default: 0.001)
constexpr auto gLimitPrecisionOfBinarySearch = 0.001f;

//! Size of the collision cache used - Larger values should allow more complex collisions (Default: 100)
constexpr size_t COL_CACHE_SIZE = 100;

/*!
* The cache has regions.
* Each region starts with an entry that has `ent` set.
* All following entries (up until the next such entry) belong to it.
*/
using ColCache = std::array<CColCacheEntry, COL_CACHE_SIZE>;

//! Cache used currently
static inline auto& gpColCache = StaticRef<ColCache*>(0x9655CC);

//! Entries in the cache that is used currently
static inline auto& gColCacheNumEntries = StaticRef<uint32>(0x9655D8);

//! Unused
static inline auto& gpColCache2 = StaticRef<ColCache*>(0x9655C8);

//! Fuck knows
static inline auto& gbTryDoubleSidedCollision = StaticRef<bool>(0x9655E4);

//! Last "distance" that wasn'plSpCenterDist colliding - result from the binary search
static inline auto& gLastRadiusUsedInCollisionPreventionOfCamera = StaticRef<float>(0xB6EC6C);

// 0x415590
bool CCollision::SphereCastVsBBox(
    const CColSphere& spA,
    const CColSphere& spB,
    const CColBox& box
) {
    ZoneScoped;

    const auto     r = spA.m_fRadius;
    const CVector  vRadius{ r, r, r };
    return TestLineBox_DW(
        CColLine{ spA.m_vecCenter, spB.m_vecCenter },
        CColBox{ box.m_vecMin - vRadius, box.m_vecMax + vRadius } // Not sure what's going on
    );
}

/*!
* The input to this function is basically generated by `SphereCastVsEntity`
* The output format (written to `out`) is exactly the same as the input.
*
* @param       spAws    As discussed.
* @param       spBToA Player's spBToA
* @param       numIn    Number of entries in the `in` cache
* @param       in       Input cache (To process)
* @param [out] numOut   Number of entries written to the `out` cache
* @param [out] out      Output cache (Of entries [from `in`] that had collided)
* @addr 0x4181B0
*/
bool CCollision::SphereCastVsCaches(
    const CColSphere& spAws,
    const CVector& velocity,

    int32 numIn,
    CColCacheEntry* in,

    int32& numOut,
    CColCacheEntry* out
) {
    ZoneScoped;

    // Como el original: B = A + velocidad (centro) con el mismo radio
    const CColSphere spBws{
        CVector{ spAws.m_vecCenter.x + velocity.x, spAws.m_vecCenter.y + velocity.y, spAws.m_vecCenter.z + velocity.z },
        spAws.m_fRadius
    };

    // Como el original: se recorren TODAS las entradas de forma lineal; la entrada con `ent` abre una region nueva
    // (matriz inversa, esferas en espacio objeto y colision de la entidad) y las siguientes (ent nulo) la reutilizan.
    CColSphere               spAos = spAws, spBos = spBws;
    CCollisionData*          ecd{};
    const CompressedVector*  verts{};
    const CColTriangle*      tris{};
    const CColTrianglePlane* triPls{};
    for (int32 left = numIn; left > 0; left--, in++) {
        const auto entity = in->ent;
        if (entity) {
            spAos = spAws;
            spBos = spBws;

            const auto& entMat = entity->GetMatrix();
            ecd = entity->GetColModel()->m_pColData;
            if (!ecd) {
                continue; // El EXE salta a la siguiente entrada (las siguientes de esta region solo existen si ecd no es nulo)
            }
            verts  = ecd->m_pVertices;
            tris   = ecd->m_pTriangles;
            triPls = ecd->m_pTrianglePlanes;

            // Como el original: Invert (0x59B920) y MultiplyMatrixWithVector (0x59C890) sobre los centros
            const CMatrix invEntMat = Invert(entMat);
            spAos.m_vecCenter = ColWorld59c::MulPoint(invEntMat, spAws.m_vecCenter);
            spBos.m_vecCenter = ColWorld59c::MulPoint(invEntMat, spBws.m_vecCenter);

            // Have to push this nevertheless
            // If there are no collisions it will be overwritten
            // Como el original: solo se escriben el tipo (NONE) y la entidad
            out[numOut].type = CColCacheEntry::eType::NONE;
            out[numOut].ent  = entity;
        }
        if (!ecd) {
            continue;
        }

        // Como el original: tras cada colision se anota la entrada (solo tipo e indice propio) y la siguiente casilla
        // se deja con entidad nula y tipo NONE.
        const auto Finish = [&](CColCacheEntry::eType type) {
            out[numOut].type = type;
            numOut++;
            out[numOut].ent  = nullptr;
            out[numOut].type = CColCacheEntry::eType::NONE;
        };

        using enum CColCacheEntry::eType;
        switch (in->type) {
        case TRIANGLE: {
            const uint16 triIdx = in->triIdx;
            if (triIdx < SHRT_MAX) { // cmp di,7FFFh + jae (sin signo)
                if (SphereCastVersusVsPoly(spAos, spBos, tris[triIdx], triPls[triIdx], const_cast<CompressedVector*>(verts))) {
                    out[numOut].triIdx = triIdx;
                    Finish(TRIANGLE);
                }
            } else { // Search in file for: BULLSHIT_DETECTOR - cara trasera: indice (0xFFFF - idx) y esferas intercambiadas
                const uint16 backIdx = uint16(0xFFFFu - triIdx);
                if (SphereCastVersusVsPoly(spBos, spAos, tris[backIdx], triPls[backIdx], const_cast<CompressedVector*>(verts))) {
                    out[numOut].triIdx = triIdx;
                    Finish(TRIANGLE);
                }
            }
            break;
        }
        case SPHERE: {
            const uint16 sphIdx = in->sphIdx;
            if (SphereCastVsSphere(spAos, spBos, ecd->m_pSpheres[sphIdx])) {
                out[numOut].sphIdx = sphIdx;
                Finish(SPHERE);
            }
            break;
        }
        case BOX: {
            const uint16 boxIdx = in->boxIdx;
            if (SphereCastVsBBox(spAos, spBos, ecd->m_pBoxes[boxIdx])) {
                out[numOut].boxIdx = boxIdx;
                Finish(BOX);
            }
            break;
        }
        default: // Como el original: tipos desconocidos se ignoran
            break;
        }
    }

    return numOut > 0;
}

/*!
* Similar to `SphereCastVsCaches`, but only against a single entity.
* Result is written to `gpColCache`, and the number of entries to `gColCacheNumEntries`
* @addr 0x419F00
*/
bool CCollision::SphereCastVsEntity(
    const CColSphere& spAws,
    const CColSphere& spBws,
    CEntity* entity
) {
    ZoneScoped;

    if (!entity->GetUsesCollision() || TheCamera.IsExtraEntityToIgnore(entity)) {
        return false;
    }

    // Como el original: la matriz (se crea si falta) y la inversa se calculan ANTES de mirar si hay datos de colision
    const CMatrix invEntMat = Invert(entity->GetMatrix());

    // Como el original: el centro de cada esfera se pasa con MultiplyMatrixWithVector y AMBAS esferas usan el radio de spAws
    // (el EXE copia [spA + 0xC] en las dos; antes se "arreglaba" usando el radio de spBws).
    const CColSphere spAos{ ColWorld59c::MulPoint(invEntMat, spAws.m_vecCenter), spAws.m_fRadius };
    const CColSphere spBos{ ColWorld59c::MulPoint(invEntMat, spBws.m_vecCenter), spAws.m_fRadius };

    const auto ecm = entity->GetColModel();
    const auto ecd = ecm->GetData();
    if (!ecd) {
        return false;
    }

    if (!SphereCastVsBBox(spAos, spBos, ecm->GetBoundingBox())) {
        return false;
    }

    using enum CColCacheEntry::eType;

    auto* const cache = gpColCache->data();
    auto anyCollisionsDetected = false;

    // Como el original: si ya hay 99 entradas devuelve true; la entrada nueva escribe tipo, entidad (solo la primera), indice y tipo final.
    const auto AddEntryToColCache = [&, entity](CColCacheEntry::eType type, uint16 idx) {
        if (static_cast<int32>(gColCacheNumEntries) >= static_cast<int32>(COL_CACHE_SIZE - 1)) { // TODO: Magic number
            return true;
        }
        auto& e = cache[gColCacheNumEntries];
        e.type = NONE;
        if (!anyCollisionsDetected) { // Only the first entry has the entity set, subsequent ones have nullptr
            e.ent = entity;
            anyCollisionsDetected = true;
        } else {
            e.ent = nullptr;
        }
        gColCacheNumEntries = gColCacheNumEntries + 1;
        switch (type) {
        case TRIANGLE: e.triIdx = idx; break;
        case SPHERE:   e.sphIdx = idx; break;
        case BOX:      e.boxIdx = idx; break;
        default:       break;
        }
        e.type = type;
        return false;
    };

    // Process spheres
    // Como el original: de la ULTIMA esfera a la primera
    for (int32 idx = static_cast<int16>(ecd->m_nNumSpheres) - 1; idx >= 0; idx--) {
        if (!SphereCastVsSphere(spAos, spBos, ecd->m_pSpheres[idx])) {
            continue;
        }
        if (AddEntryToColCache(SPHERE, static_cast<uint16>(idx))) {
            return true;
        }
    }

    // Process triangles
    {
        CalculateTrianglePlanes(ecm->m_pColData);

        const auto verts  = ecd->m_pVertices;
        const auto tris   = ecd->m_pTriangles;
        const auto triPls = ecd->m_pTrianglePlanes;
        const bool tryDoubleSided = gbTryDoubleSidedCollision;

        const auto ProcessTri = [&](int32 triIdx) { // If `true` is returned the calle should `return true` too, otherwise nothing.
            const auto& tri   = tris[triIdx];
            const auto& triPl = triPls[triIdx];

            if (SphereCastVersusVsPoly(spAos, spBos, tri, triPl, verts)) {
                if (AddEntryToColCache(TRIANGLE, static_cast<uint16>(triIdx))) {
                    return true;
                }
            }

            // Como el original: |normal.z (int16 * 1/4096)| < 0.05 (el codigo antiguo hacia abs(bool))
            if (tryDoubleSided && std::abs(float(reinterpret_cast<const int16*>(&triPl)[2]) * 0.000244140625f) < 0.05f
                && SphereCastVersusVsPoly(spBos, spAos, tri, triPl, verts)) {
                if (AddEntryToColCache(TRIANGLE, static_cast<uint16>(0xFFFFu - static_cast<uint32>(triIdx)))) { // Search in file for: BULLSHIT_DETECTOR
                    return true;
                }
            }

            return false;
        };

        // Como el original: sin triangulos no se hace nada
        if (ecd->m_nNumTriangles != 0) {
            if (ecd->bHasFaceGroups) {
                // Como el original: los grupos se leen desde el final (el primero esta justo antes de los triangulos) y con signo
                const auto numFaceGroups = static_cast<int32>(ecd->GetNumFaceGroups());
                for (int32 fgIdx = 0; fgIdx < numFaceGroups; fgIdx++) {
                    const auto& fg = *reinterpret_cast<const ColHelpers::TFaceGroup*>(reinterpret_cast<const uint8*>(tris) - 0x20 - fgIdx * 0x1C);
                    if (!SphereCastVsBBox(spAos, spBos, CColBox{ fg.bb })) {
                        continue;
                    }

                    for (int32 triIdx = static_cast<int16>(fg.first); triIdx <= static_cast<int16>(fg.last); triIdx++) {
                        if (ProcessTri(triIdx)) {
                            return true;
                        }
                    }
                }
            } else {
                // Como el original: de el ULTIMO triangulo al primero
                for (int32 i = static_cast<int16>(ecd->m_nNumTriangles) - 1; i >= 0; i--) {
                    if (ProcessTri(i)) {
                        return true;
                    }
                }
            }
        }
    }

    // Process boxes
    // Como el original: de la ULTIMA caja a la primera
    for (int32 idx = static_cast<int16>(ecd->m_nNumBoxes) - 1; idx >= 0; idx--) {
        if (SphereCastVsBBox(spAos, spBos, ecd->m_pBoxes[idx])) {
            if (AddEntryToColCache(BOX, static_cast<uint16>(idx))) {
                return true;
            }
        }
    }

    return anyCollisionsDetected;
}

namespace ColWorld59c {
// Como el original (0x41A820 / 0x41A990 / 0x41AB20): esfera de la entidad = centro de GetBoundCentre (0x534250) y radio de la
// colision del MODELO base (modelinfo + 0x14, +0x24), no la esfera transformada de GetColModel().
inline CColSphere EntityBoundSphere(CEntity* entity) {
    const float radius = entity->GetModelInfo()->GetColModel()->GetBoundingSphere().m_fRadius;
    return CColSphere{ entity->GetBoundCentre(), radius };
}
} // namespace ColWorld59c

// 0x41A820
bool CCollision::CheckCameraCollisionBuildings(
    int32 X,
    int32 Y,
    const CColBox& pBox,
    const CColSphere& spS,
    const CColSphere& spA,
    const CColSphere& spB
) {
    ZoneScoped;

    // Como el original: FindPlayerVehicle(-1, false) y comprobacion del flag bDontCollideWithFlyers (bit 0 de +0x43)
    const auto plyrVeh = FindPlayerVehicle(-1, false);
    const auto checkFlyerCollision = plyrVeh && plyrVeh->physicalFlags.bDontCollideWithFlyers;

    bool anyCollided = false;
    for (auto* const entity : CWorld::GetSector(X, Y).Buildings) {
        if (!entity->ProcessScan()) {
            continue;
        }

        // Como el original: se omite si DoesNotCollideWithFlyers (antes estaba negado) y, aparte, si es la entidad ignorada
        if (checkFlyerCollision && entity->DoesNotCollideWithFlyers()) {
            continue;
        }
        if (CWorld::pIgnoreEntity == entity) {
            continue;
        }

        if (!TestSphereSphere(spS, ColWorld59c::EntityBoundSphere(entity))) {
            continue;
        }

        if (SphereCastVsEntity(spA, spB, entity)) {
            anyCollided = true;
        }
    }
    return anyCollided;
}

// 0x41A990
bool CCollision::CheckCameraCollisionVehicles(
    int32 X,
    int32 Y,
    const CColBox& bbSpAB,
    const CColSphere& spS,
    const CColSphere& spA,
    const CColSphere& spB,
    const CVector* plyrVehVel
) {
    ZoneScoped;

    static auto& gpLastSittingOnEntity   = StaticRef<CEntity*>(0x9689D8);
    static auto& gFramesSittingOnTimeOut = StaticRef<int32>(0x9689D4);
    static auto& gFramesToConsiderSittingOnStillTrue = StaticRef<int32>(0x8A5B1C); // 30

    bool anyCollided = false;
    // Como el original: indice del sector repetido con `& 0xF`
    for (auto* const entity : CWorld::ms_aRepeatSectors[Y & 0xF][X & 0xF].Vehicles) {
        if (!entity->ProcessScan()) {
            continue;
        }

        if (CWorld::pIgnoreEntity == entity) {
            continue;
        }

        if (IsThisVehicleSittingOnMe(reinterpret_cast<CVehicle*>(CWorld::pIgnoreEntity), entity)) {
            gpLastSittingOnEntity = entity;
            gFramesSittingOnTimeOut = gFramesToConsiderSittingOnStillTrue;
            continue;
        }

        if (gpLastSittingOnEntity == entity) {
            if (gFramesSittingOnTimeOut-- == 0) { // TODO: FPS bug
                gpLastSittingOnEntity = nullptr;
            }
            continue;
        }

        if (plyrVehVel) {
            // Como el original: (dz^2 + dy^2) + dx^2 y solo sigue si es MENOR que el limite (NaN descarta)
            const auto& ms = entity->GetMoveSpeed();
            const float dx = plyrVehVel->x - ms.x;
            const float dy = plyrVehVel->y - ms.y;
            const float dz = plyrVehVel->z - ms.z;
            if (!((dz * dz + dy * dy) + dx * dx < relVelCamCollisionVehiclesSqr)) {
                continue;
            }
        }

        if (!TestSphereSphere(spS, ColWorld59c::EntityBoundSphere(entity))) {
            continue;
        }

        if (SphereCastVsEntity(spA, spB, entity)) {
            anyCollided = true;
        }
    }
    return anyCollided;
}

// 0x41AB20
bool CCollision::CheckCameraCollisionObjects(
    int32 X,
    int32 Y,
    const CColBox& pBox,
    const CColSphere& spS,
    const CColSphere& spA,
    const CColSphere& spB
) {
    ZoneScoped;

    // Pirulax: At this point I'm certain R* devs were paid by lines written

    bool anyCollided = false;
    // Como el original: indice del sector repetido con `& 0xF`
    for (auto* const entity : CWorld::ms_aRepeatSectors[Y & 0xF][X & 0xF].Objects) {
        if (!entity->ProcessScan()) {
            continue;
        }

        // Como el original: se omite si CameraToIgnoreThisObject Y, por separado, si es la entidad ignorada (antes era un &&)
        if (CWorld::CameraToIgnoreThisObject(entity)) {
            continue;
        }
        if (CWorld::pIgnoreEntity == entity) {
            continue;
        }

        if (!TestSphereSphere(spS, ColWorld59c::EntityBoundSphere(entity))) {
            continue;
        }

        if (SphereCastVsEntity(spA, spB, entity)) {
            anyCollided = true;
        }
    }
    return anyCollided;
}

// Ah, yes, the ultimate solution, just use static variables!
static inline auto& gnBottom = StaticRef<int32>(0x965598);
static inline auto& gnTop = StaticRef<int32>(0x965590);
static inline auto& gnRight = StaticRef<int32>(0x965594);
static inline auto& gnLeft = StaticRef<int32>(0x96559C);

// 0x4154A0
bool CCollision::CheckPeds(
    const CVector& src,
    const CVector& normal, /*unused*/
    float& nearest /*unused*/
) {
    ZoneScoped;

    if (!bCamCollideWithPeds) {
        return false;
    }

    // Como el original: los limites se leen una sola vez y el bucle no tiene aserto (CWorld::IterateSectors si lo tiene)
    const int32 left = gnLeft, bottom = gnBottom, right = gnRight, top = gnTop;
    bool anyCollides = false;
    for (int32 sy = bottom; sy <= top; sy++) {
        for (int32 sx = left; sx <= right; sx++) {
            if (CheckCameraCollisionPeds(sx, sy, src, normal, nearest)) {
                anyCollides = true;
            }
        }
    }
    return anyCollides;
}

// 0x41AC40
bool CCollision::BuildCacheOfCameraCollision(
    const CColSphere& spA,
    const CColSphere& spB
) {
    ZoneScoped;

    const auto spABBox = GetBoundingBoxFromTwoSpheres(spA, spB);

    // Como el original: tamano = max - min; centro = tamano * 0.5 + min (no (max + min) / 2);
    // radio = sqrt((z^2 + y^2) + x^2) * 0.5 (suma en orden z, y, x).
    const float sizeX = spABBox.m_vecMax.x - spABBox.m_vecMin.x;
    const float sizeY = spABBox.m_vecMax.y - spABBox.m_vecMin.y;
    const float sizeZ = spABBox.m_vecMax.z - spABBox.m_vecMin.z;
    const CVector center{ sizeX * 0.5f + spABBox.m_vecMin.x, sizeY * 0.5f + spABBox.m_vecMin.y, sizeZ * 0.5f + spABBox.m_vecMin.z };
    const float   radius = std::sqrt((sizeZ * sizeZ + sizeY * sizeY) + sizeX * sizeX) * 0.5f;
    const auto    spABBSp = CColSphere{CSphere{ center, radius }};

    // Como el original: sector = floor(v * 0.02 + 60) (0x858B38 / 0x858B34, no v / 50 + 60) convertido con _ftol2;
    // izquierda y abajo solo se acotan por debajo (a 0), derecha y arriba solo por arriba (a 119).
    const auto SectorOf = [](float v) { return static_cast<int32>(std::floor(v * 0.02f + 60.0f)); };
    const int32 sectorLeft   = SectorOf(spABBox.m_vecMin.x);
    const int32 sectorBottom = SectorOf(spABBox.m_vecMin.y);
    const int32 sectorRight  = SectorOf(spABBox.m_vecMax.x);
    const int32 sectorTop    = SectorOf(spABBox.m_vecMax.y);
    gnLeft   = sectorLeft   > 0 ? sectorLeft : 0;
    gnBottom = sectorBottom > 0 ? sectorBottom : 0;
    gnRight  = sectorRight  >= 119 ? 119 : sectorRight;
    gnTop    = sectorTop    >= 119 ? 119 : sectorTop;

    CWorld::AdvanceCurrentScanCode();

    // Como el original: las tres opciones de la camara se leen una vez antes del bucle
    const bool doVehicles  = bCamCollideWithVehicles;
    const bool doBuildings = bCamCollideWithBuildings;
    const bool doObjects   = bCamCollideWithObjects;

    gColCacheNumEntries = 0;

    const auto ogpIgnoreEntity = CWorld::pIgnoreEntity;
    if (!CWorld::pIgnoreEntity) {
        auto& plyrtm = FindPlayerPed(0)->GetTaskManager();

        // Como el original: primero la tarea de entrar de CONDUCTOR (0x2BD = 701) y despues la de pasajero (0x2BC = 700)
        if (const auto task = static_cast<CTaskComplexEnterCar*>(plyrtm.Find<CTaskComplexEnterCarAsDriver, CTaskComplexEnterCarAsPassenger>())) {
            CWorld::pIgnoreEntity = task->GetCameraAvoidVehicle();
        }
    }

    // Como el original: FindPlayerVehicle(0, false) y una COPIA de FindPlayerSpeed(0)
    CVector        plyrVehVelCopy;
    const CVector* plyrVehVel = nullptr;
    if (FindPlayerVehicle(0, false)) {
        plyrVehVelCopy = FindPlayerSpeed(0);
        plyrVehVel     = &plyrVehVelCopy;
    }

    // Como el original: limites leidos una vez; si no hay rango el valor devuelto es basura de pila (aqui falso)
    const int32 left = gnLeft, bottom = gnBottom, right = gnRight, top = gnTop;
    bool anyCollision = false;
    for (int32 sy = bottom; sy <= top; sy++) {
        for (int32 sx = left; sx <= right; sx++) {
            if (doBuildings) {
                gbTryDoubleSidedCollision = true;
                if (CheckCameraCollisionBuildings(sx, sy, spABBox, spABBSp, spA, spB)) {
                    anyCollision = true;
                }
                gbTryDoubleSidedCollision = false;
            }
            if (doVehicles) {
                if (CheckCameraCollisionVehicles(sx, sy, spABBox, spABBSp, spA, spB, plyrVehVel)) {
                    anyCollision = true;
                }
            }
            if (doObjects) {
                if (CheckCameraCollisionObjects(sx, sy, spABBox, spABBSp, spA, spB)) {
                    anyCollision = true;
                }
            }
        }
    }

    CWorld::pIgnoreEntity = ogpIgnoreEntity;

    return anyCollision;
}

/*!
* Cast the cone of the camera against the world
* Honestly, as much as I bash the devs from 2003, this one
* is quite well done (Other than using static variables xD)
* This function is responsible for the anit-clipping
* (When you move the camera and instead of clipping thru the
* object it slides along it's collision)
* 
* @addr 0x41B000
*
* @param spA     Sphere representing the camera
* @param spB     `spA` but it's center offset by the spBToA of the player (Basically, where the camera would be the next timestep)
* @param dst     Minimum distance that doesn'plSpCenterDist collide ("Distance" is a bad word tbh, it's more like a scale from [minDist, 1])
* @param minDist See `dst`
*/
bool CCollision::CameraConeCastVsWorldCollision(
    const CColSphere& spA,
    const CColSphere& spB,
    float& dst,
    float minDist
) {
    ZoneScoped;

    // Como el original: las dos cachés viven en la pila SIN inicializar (antes `ColCache caches[2]{}` las ponia a cero)
    alignas(CColCacheEntry) uint8 cacheStorage[2][sizeof(ColCache)];
    auto* const caches = reinterpret_cast<ColCache*>(cacheStorage);
    gpColCache  = &caches[0];
    gpColCache2 = &caches[1];

    if (!BuildCacheOfCameraCollision(spA, spB)) {
        return false;
    }

    // Reminder: The 2 spheres are offset by the spBToA of the player...
    const CVector velocity{ spB.m_vecCenter.x - spA.m_vecCenter.x, spB.m_vecCenter.y - spA.m_vecCenter.y, spB.m_vecCenter.z - spA.m_vecCenter.z };

    // Radius of the badass spehere we're going to use
    const auto spRadius = spA.m_fRadius;

    // Badass sphere that represents the camera
    // (gets smaller and smaller as we progress with the binary search)
    CColSphere spCam{ spA.m_vecCenter, spRadius };

    // Como el original: el numero de entradas se lleva en un registro (gColCacheNumEntries y los punteros globales
    // de las cachés NO se actualizan durante la busqueda) y se alternan las dos cachés.
    int32     numIn   = static_cast<int32>(gColCacheNumEntries);
    ColCache* cur     = &caches[0];
    ColCache* next    = &caches[1];

    // Now, we do a badass binary search to find the closest 
    // possible distance to the collision such that the camera
    // (or well, the sphere representing it `spCam`)
    // isn'plSpCenterDist clipping into it
    // Como el original: rng = max - min se recalcula AL FINAL de cada vuelta y es lo que se compara con el limite
    // (la version anterior comparaba el valor previo a la actualizacion y hacia una vuelta de mas).
    float max = 1.f, min = minDist;
    float rng = 1.f - minDist;
    float dstValue;
    do {
        dstValue = rng * 0.5f + min;

        spCam.m_fRadius = dstValue * spRadius;

        const CVector scaledVelocity{ velocity.x * dstValue, velocity.y * dstValue, velocity.z * dstValue };
        if (int32 numOut = 0; SphereCastVsCaches(spCam, scaledVelocity, numIn, cur->data(), numOut, next->data())) {
            numIn = numOut;
            std::swap(cur, next);
            max = dstValue;
        } else {
            min = dstValue;
        }
        rng = max - min;
    } while (rng > gLimitPrecisionOfBinarySearch);

    dst = dstValue;

    // Como el original: se guarda el RADIO final de la esfera de la camara (dst * radio), no dst
    gLastRadiusUsedInCollisionPreventionOfCamera = spCam.m_fRadius;

    return true;
}

// Como el original: el EXE NO lee el parametro `sphere`. Prueba una CColSphere LOCAL sin inicializar: los 16 bytes
// (x, y, z, radio) que quedan en la pila en [ESP_entrada - 0x68 .. - 0x59] (0x41A665/6B/72/79, ninguna escritura previa
// los toca). El puente desnudo de abajo los copia al entrar, antes de cualquier push o llamada, y el cuerpo C++ los usa.
static uint32 gSphereVsEntityStackSphere[4];

static bool __cdecl SphereVsEntityBody(CColSphere* /*sphere: ignorada, como en el EXE*/, CEntity* entity) {
    // Como el original (0x41A5C0): bit 0 de +0x1C (m_bUsesCollision) y despues CCamera::IsExtraEntityToIgnore (0x50CE80)
    if (!entity->m_bUsesCollision || TheCamera.IsExtraEntityToIgnore(entity)) {
        return false;
    }

    // Como el original: la matriz se crea si falta (0x54F560 + 0x54F1B0) y se invierte (0x59B920) a una temporal
    // que se descarta (0x59ACD0); todo ANTES de mirar si hay datos de colision.
    CMatrix inv;
    Invert(entity->GetMatrix(), inv);

    const auto cm = entity->GetColModel();
    const auto cd = cm->m_pColData;
    if (!cd) {
        return false;
    }

    CColSphere sph;
    std::memcpy(&sph, gSphereVsEntityStackSphere, sizeof(gSphereVsEntityStackSphere));
    const float sx = sph.m_vecCenter.x, sy = sph.m_vecCenter.y, sz = sph.m_vecCenter.z, sr = sph.m_fRadius;

    // Como el original (0x41A656): contador int16, de la ultima a la primera; distancia^2 sumada en orden z, y, x
    // y contacto solo si (r1 + r2)^2 > dist^2 (fcompp + test ah,41h: con NaN no hay contacto).
    for (int32 i = (int16)cd->m_nNumSpheres - 1; i >= 0; i--) {
        const auto& cs = cd->m_pSpheres[i];
        const float dx = sx - cs.m_vecCenter.x;
        const float dy = sy - cs.m_vecCenter.y;
        const float dz = sz - cs.m_vecCenter.z;
        const float r  = sr + cs.m_fRadius;
        const float d2 = (dz * dz + dy * dy) + dx * dx;
        if (r * r > d2) {
            return true;
        }
    }

    // Como el original (0x41A6B1): cajas, de la ultima a la primera
    for (int32 i = (int16)cd->m_nNumBoxes - 1; i >= 0; i--) {
        if (CCollision::TestSphereBox(sph, cd->m_pBoxes[i])) {
            return true;
        }
    }

    // Como el original (0x41A6E0): siempre se calculan los planos (0x416330)
    CCollision::CalculateTrianglePlanes(cd);

    const auto verts = cd->m_pVertices;
    const auto tris  = cd->m_pTriangles;
    const auto plns  = cd->m_pTrianglePlanes;
    if ((int16)cd->m_nNumTriangles == 0) {
        return false;
    }

    if (cd->bHasFaceGroups) {
        // Como el original (0x41A720): el contador de grupos (int32 con signo) esta en tris - 4 y el grupo g en tris - 0x20 - g * 0x1C
        const int32 numGroups = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(tris) - 4);
        for (int32 g = 0; g < numGroups; g++) {
            const auto& group = *reinterpret_cast<const ColHelpers::TFaceGroup*>(reinterpret_cast<const uint8*>(tris) - 0x20 - g * 0x1C);
            if (!CCollision::TestSphereBox(sph, group.bb)) {
                continue;
            }
            for (int32 t = (int16)group.first; t <= (int16)group.last; t++) {
                if (CCollision::TestSphereTriangle(sph, verts, tris[t], plns[t])) {
                    return true;
                }
            }
        }
        return false;
    }

    // Como el original (0x41A7C4): sin grupos, de la ultima a la primera
    for (int32 t = (int16)cd->m_nNumTriangles - 1; t >= 0; t--) {
        if (CCollision::TestSphereTriangle(sph, verts, tris[t], plns[t])) {
            return true;
        }
    }
    return false;
}

// 0x41A5A0
bool __declspec(naked) CCollision::SphereVsEntity(CColSphere* sphere, CEntity* entity) {
    // Al entrar [esp] = retorno, [esp+4] = sphere, [esp+8] = entity (cdecl). Los 16 bytes de basura de pila de
    // [esp-0x68] se capturan sin tocar la pila y se salta al cuerpo con los argumentos intactos.
    __asm {
        mov eax, dword ptr [esp-0x68]
        mov dword ptr [gSphereVsEntityStackSphere+0], eax
        mov eax, dword ptr [esp-0x64]
        mov dword ptr [gSphereVsEntityStackSphere+4], eax
        mov eax, dword ptr [esp-0x60]
        mov dword ptr [gSphereVsEntityStackSphere+8], eax
        mov eax, dword ptr [esp-0x5C]
        mov dword ptr [gSphereVsEntityStackSphere+12], eax
        jmp SphereVsEntityBody
    }
}

void CCollision::InjectHooks() {
#ifdef TEST_COLLISION_FUNCS
    // Must be done before hooks are injected
    for (auto i = 0; i < 20; i++) {
       Tests(i);
    }
#endif

    RH_ScopedClass(CCollision);
    RH_ScopedCategoryGlobal();

    ////
    // Test & Process
    ////

    // Hooks disabled due to bad performance in debug mode
    const bool bEnableHooks = false;
    
    RH_ScopedInstall(Test2DLineAgainst2DLine, 0x4138D0);

    RH_ScopedInstall(ProcessDiscCollision, 0x413960);

    RH_ScopedInstall(TestLineBox_DW, 0x412C70);
    RH_ScopedInstall(TestLineBox, 0x413070);
    RH_ScopedInstall(ProcessLineBox, 0x413100);
    RH_ScopedInstall(TestVerticalLineBox, 0x413080);

    RH_ScopedInstall(TestLineTriangle, 0x413AC0);
    RH_ScopedInstall(ProcessLineTriangle, 0x4140F0);
    RH_ScopedInstall(ProcessVerticalLineTriangle, 0x4147E0);

    RH_ScopedInstall(TestLineSphere, 0x417470);
    RH_ScopedInstall(ProcessLineSphere, 0x412AA0);

    RH_ScopedInstall(TestSphereBox, 0x4120C0);
    RH_ScopedInstall(ProcessSphereBox, 0x412130);

    RH_ScopedInstall(TestSphereSphere, 0x411E70);
    RH_ScopedInstall(ProcessSphereSphere, 0x416450);

    RH_ScopedInstall(TestSphereTriangle, 0x4165B0);
    RH_ScopedInstall(ProcessSphereTriangle, 0x416BA0);

    RH_ScopedInstall(ProcessColModels, 0x4185C0);

    RH_ScopedInstall(TestLineOfSight, 0x417730);
    RH_ScopedInstall(ProcessLineOfSight, 0x417950);
    RH_ScopedInstall(ProcessVerticalLine, 0x417BF0);

    ////
    // Rest
    ////

    RH_ScopedInstall(Init, 0x416260);
    RH_ScopedInstall(Shutdown, 0x4162E0);
    RH_ScopedInstall(Update, 0x411E20);
    RH_ScopedInstall(SortOutCollisionAfterLoad, 0x411E30);
    RH_ScopedGlobalInstall(CalculateColPointInsideBox, 0x411EC0);
    RH_ScopedInstall(PointInTriangle, 0x412700);
    RH_ScopedInstall(DistToLineSqr, 0x412850);
    RH_ScopedInstall(DistToMathematicalLine, 0x412970);
    RH_ScopedInstall(DistToMathematicalLine2D, 0x412A30);
    RH_ScopedInstall(DistAlongLine2D, 0x412A80);
    RH_ScopedInstall(IsStoredPolyStillValidVerticalLine, 0x414D70);
    RH_ScopedInstall(GetBoundingBoxFromTwoSpheres, 0x415230);
    RH_ScopedInstall(IsThisVehicleSittingOnMe, 0x4152C0);
    RH_ScopedInstall(CheckCameraCollisionPeds, 0x415320);
    RH_ScopedInstall(CheckPeds, 0x4154A0);
    RH_ScopedGlobalInstall(ResetMadeInvisibleObjects, 0x415540);
    RH_ScopedInstall(SphereCastVsBBox, 0x415590);
    RH_ScopedInstall(RayPolyPOP, 0x415620);
    RH_ScopedInstall(GetPrincipleAxis, 0x4156D0);
    RH_ScopedInstall(PointInPoly, 0x415730);
    RH_ScopedInstall(Closest3, 0x415950);
    RH_ScopedGlobalInstall(ClosestSquaredDistanceBetweenFiniteLines, 0x415A40);
    RH_ScopedInstall(SphereCastVersusVsPoly, 0x415CF0);
    RH_ScopedInstall(DistToLine, 0x417610); 
    RH_ScopedInstall(SphereCastVsSphere, 0x417F20);
    RH_ScopedInstall(ClosestPointOnLine, 0x417FD0);
    RH_ScopedInstall(ClosestPointsOnPoly, 0x418100);
    RH_ScopedInstall(ClosestPointOnPoly, 0x418150);
    RH_ScopedInstall(SphereCastVsCaches, 0x4181B0);
    RH_ScopedInstall(SphereCastVsEntity, 0x419F00);
    RH_ScopedInstall(SphereVsEntity, 0x41A5A0);
    RH_ScopedInstall(CheckCameraCollisionBuildings, 0x41A820);
    RH_ScopedInstall(CheckCameraCollisionVehicles, 0x41A990);
    RH_ScopedInstall(CheckCameraCollisionObjects, 0x41AB20);
    RH_ScopedInstall(BuildCacheOfCameraCollision, 0x41AC40);
    RH_ScopedInstall(CameraConeCastVsWorldCollision, 0x41B000);

    RH_ScopedOverloadedInstall(CalculateTrianglePlanes, "colData", 0x416330, void (*)(CCollisionData*));
    RH_ScopedOverloadedInstall(RemoveTrianglePlanes, "colData", 0x416400, void (*)(CCollisionData*));
}

void CCollision::Tests(int32 i) {
#ifdef TEST_COLLISION_FUNCS
    const auto seed = (uint32)time(nullptr) + i;
    srand(seed);
    std::cout << "CCollision::Tests seed: " << seed << std::endl;

    const auto VectorEq = [](const CVector& lhs, const CVector& rhs, float epsilon = 0.01f) {
        for (auto i = 0u; i < 3u; i++) {
            if (!approxEqual(lhs[i], rhs[i], epsilon)) {
                return false;
            }
        }
        return true;
    };

    const auto ColPointEq = [&](const CColPoint& lhs, const CColPoint& rhs) {
        return VectorEq(lhs.m_vecPoint, rhs.m_vecPoint)
            && approxEqual(lhs.m_fDepth, rhs.m_fDepth, 0.01f)
            && VectorEq(lhs.m_vecNormal, rhs.m_vecNormal);
    };

    const auto RandomVector = [](float min = -100.f, float max = 100.f) {
        return CVector{
            CGeneral::GetRandomNumberInRange(min, max),
            CGeneral::GetRandomNumberInRange(min, max),
            CGeneral::GetRandomNumberInRange(min, max)
        };
    };

    const auto RandomNormal = [&]() {
        return RandomVector().Normalized();
    };

    const auto RandomSphere = [&](float min = -100.f, float max = 100.f) {
        return CColSphere{ RandomVector(min, max), CGeneral::GetRandomNumberInRange(min, max) };
    };

    const auto RandomBox = [&](float min = -100.f, float max = 100.f) {
        CColBox cb{ RandomVector(min, max), RandomVector(min, max) };
        cb.Recalc();
        return cb;
    };

    const auto RandomLine = [&](float min = -100.f, float max = 100.f) {
        return CColLine{ RandomVector(min, max) , RandomVector(min, max) };
    };

    const auto RandomVerticalLine = [&](float min = -100.f, float max = 100.f) {
        const auto pos = RandomVector(min, max);
        return CColLine{ {pos.x, pos.y, pos.z + fabs(pos.x)}, {pos.x, pos.y, pos.z - fabs(pos.x)} };
    };

    const auto RandomTriangleVertices = [&](float min = -100.f, float max = 100.f) {
        const auto vtxA = RandomVector(min, max);
        const auto norm = RandomNormal();
        return std::array<CompressedVector, 3>{
            vtxA,
            vtxA.Cross(norm),
            norm.Cross(vtxA)
        };
    };

    const auto Test = [](auto name, auto org, auto rev, auto cmp, auto&&... args) {
        const auto orgResult = org(args...);
        const auto revResult = rev(args...);
        if (!cmp(orgResult, revResult)) {
            std::cerr << "[CCollision::Tests]: " << name << " failed. " << std::endl;
            assert(0);
        }
    };
#if 1
    // TestSphereSphere
    {
        auto sp1 = RandomSphere(), sp2 = RandomSphere();
        auto Original = plugin::CallAndReturn<bool, 0x411E70, CColSphere const&, CColSphere const&>;
        Test("TestSphereSphere", Original, TestSphereSphere, std::equal_to<>{}, sp1, sp2);
    }

    // CalculateColPointInsideBox
    {
        const auto Org = [&](auto box, auto point) {
            CColPoint cp{};
            plugin::Call<0x411EC0, CBox const&, CVector const&, CColPoint&>(box, point, cp);
            return cp;
        };

        const auto Rev = [](auto box, auto point) {
            CColPoint cp{};
            CalculateColPointInsideBox(box, point, cp);
            return cp;
        };

        Test("CalculateColPointInsideBox", Org, Rev, ColPointEq, RandomBox(), RandomVector());
    }

    // TestSphereBox
    {
        const auto Org = plugin::CallAndReturn<bool, 0x4120C0, CSphere const&, CBox const&>;
        Test("TestSphereBox", Org, TestSphereBox, std::equal_to{}, RandomSphere(), RandomBox());
    }

    // CalculateColPointInsideBox
    {
        // both will be of type pair<CColPoint, float> 
        const auto CmpEq = [&](auto o, auto r) {
            return o.second == r.second && ColPointEq(o.first, r.first);
        };

        const auto Org = [&](auto sp, auto box) {
            CColPoint cp{};
            float dist{1.f};
            plugin::Call<0x412130, CColSphere const&, CColBox const&, CColPoint&, float&>(sp, box, cp, dist);
            return std::make_pair(cp, dist);
        };

        const auto Rev = [](auto sp, auto box) {
            CColPoint cp{};
            float dist{1.f};
            ProcessSphereBox(sp, box, cp, dist);
            return std::make_pair(cp, dist);
        };

        Test("CalculateColPointInsideBox", Org, Rev, CmpEq, RandomSphere(), RandomBox());
    }

    // PointInTriangle
    {
        const CVector tri[3]{ RandomVector(), RandomVector(), RandomVector() };
        const auto Org = (bool(__stdcall*)(CVector const&, CVector const*))0x412700;
        Test("PointInTriangle", Org, PointInTriangle, std::equal_to{}, RandomVector(), tri);
    }

    // DistToLineSqr
    {
        const auto Org = plugin::CallAndReturn<float, 0x412850, CVector const*, CVector const*, CVector const*>;
        const auto ls{ RandomVector() }, le{ RandomVector() }, p{ RandomVector() };
        const auto CmpEq = [](float org, float rev) {
            return approxEqual(org, rev, 0.02f);
        };
        Test("DistToLineSqr", Org, DistToLineSqr, CmpEq, &ls, &le, &p);
    }

    // DistToMathematicalLine2D
    {
        const auto Org = plugin::CallAndReturn<float, 0x412A30, float, float, float, float, float, float>;
        const auto ls{ RandomVector() }, le{ RandomVector() }, p{ RandomVector() };
        const auto CmpEq = [](float org, float rev) {
            return approxEqual(org, rev, 0.02f);
        };
        Test("DistToLineSqr", Org, DistToMathematicalLine2D, CmpEq, ls.x, ls.y, le.x, le.y, p.x, p.y);
    }

    // ClosestSquaredDistanceBetweenFiniteLines
    {
        const auto Org = plugin::CallAndReturn<float, 0x415A40, const CVector&, const CVector&, const CVector&, const CVector&, float>;

        const auto DoLnTest = [&](CColLine lnA, CColLine lnB) {
            Test(
                "ClosestSquaredDistanceBetweenFiniteLines",
                Org, ClosestSquaredDistanceBetweenFiniteLines, std::equal_to{},
                lnA.m_vecStart, lnA.m_vecEnd,
                lnB.m_vecStart, lnB.m_vecEnd - lnB.m_vecStart, (lnB.m_vecEnd - lnB.m_vecStart).SquaredMagnitude()
            );
        };

        // Vertical Parallel lines, expected: 100 [GTA: 0]
        DoLnTest(
            { { -5.f, 0.f, 0.f, }, CVector{ -5.f, 0.f, 100.f, } },
            { { +5.f, 0.f, 0.f, }, CVector{ +5.f, 0.f, 100.f, } }
        );

        // Vertical lines intersecting at the top (line end), expected: 0 [GTA: 100]
        DoLnTest(
            { { -5.f, 0.f, 0.f, }, CVector{ +5.f, 0.f, 100.f, } },
            { { +5.f, 0.f, 0.f, }, CVector{ +5.f, 0.f, 100.f, } }
        );

        // Vertical lines intersecting at the bottom (line origin), expected: 0 [GTA: 0]
        DoLnTest(
            { { +5.f, 0.f, 0.f, }, CVector{ -5.f, 0.f, 100.f, } },
            { { +5.f, 0.f, 0.f, }, CVector{ +5.f, 0.f, 100.f, } }
        );

        // Vertical lines intersecting at middle, expected: 0 [GTA: 100]
        DoLnTest(
            { { -5.f, 0.f, 0.f, }, CVector{ +10.f, 0.f, 100.f, } },
            { { +5.f, 0.f, 0.f, }, CVector{ +5.f, 0.f, 100.f, } }
        );

        /*
        const auto lnA = RandomLine();
        const auto lnB = RandomLine();

        Test(
            "ClosestSquaredDistanceBetweenFiniteLines",
            Org, ClosestSquaredDistanceBetweenFiniteLines, std::equal_to{},
            lnA.m_vecStart, lnA.m_vecEnd,
            lnB.m_vecStart, lnB.m_vecEnd - lnB.m_vecStart, (lnB.m_vecEnd - lnB.m_vecStart).SquaredMagnitude()
        );
        */
    }

    // ProcessLineSphere
    {
        const auto Org = [&](auto line, auto sp) {
            CColPoint cp{};
            float depth{ 100.f };
            const bool s = plugin::CallAndReturn<bool, 0x412AA0, CColLine const&, CColSphere const&, CColPoint&, float&>(line, sp, cp, depth);
            return std::make_tuple(cp, depth, s);
        };

        const auto Rev = [](auto line, auto sp) {
            CColPoint cp{};
            float depth{ 100.f };
            const bool s = ProcessLineSphere(line, sp, cp, depth);
            return std::make_tuple(cp, depth, s);
        };

        const auto CmpEq = [&](auto org, auto rev) {
            auto [org_cp, org_d, org_s] = org;
            auto [rev_cp, rev_d, rev_s] = rev;
            return org_s == rev_s && approxEqual(rev_d, org_d, 0.001f) && ColPointEq(org_cp, rev_cp);
        };

        Test("ProcessLineSphere", Org, Rev, CmpEq, RandomLine(), RandomSphere());
    }

    // TestLineSphere
    {
        const auto ln = RandomLine();
        const auto sp = RandomSphere();

        const auto Org = plugin::CallAndReturn<bool, 0x417470, const CColLine&, const CColSphere&>;
        Test("TestLineSphere", Org, TestLineSphere, std::equal_to{}, ln, sp);
    }

    // TestLineBox
    {
        const auto Org = plugin::CallAndReturn<bool, 0x413070, CColLine const&, CBox const&>;
        Test("TestLineBox", Org, TestLineBox, std::equal_to{}, RandomLine(), RandomBox());
    }

    // TestVerticalLineBox
    {
        const auto Org = plugin::CallAndReturn<bool, 0x413080, CColLine const&, CBox const&>;
        Test("TestVerticalLineBox", Org, TestVerticalLineBox, std::equal_to{}, RandomVerticalLine(), RandomBox());
    }

    // TestLineTriangle
    {
        const auto vtxs  = RandomTriangleVertices();
        const auto tri   = CColTriangle{ 0, 1, 2, SURFACE_CAR_PANEL, {} };
        const auto tripl = tri.GetPlane(vtxs.data());

        const auto line  = RandomLine();

        const auto Org = plugin::CallAndReturn<bool, 0x413AC0, const CColLine&, const CompressedVector*, const CColTriangle&, const CColTrianglePlane&>;
        /*
        const auto Benchmark = [&](auto fn, const char* title) {
            using namespace std::chrono;
            const auto begin = high_resolution_clock::now();
            for (auto triIdx = 0; triIdx < 100'000'000; triIdx++) {
                const auto volatile v1 = fn(line, vtxs.data(), tri, triPl);
            }
            printf("[%s]: Took %llu ms\n", title, duration_cast<milliseconds>(high_resolution_clock::now() - begin).count());
            //std::cout << "Took " << duration_cast<milliseconds>(high_resolution_clock::now() - begin) << " ms" << std::endl;
        };
        if (triIdx % 2) {
            Benchmark(TestLineTriangle, "TestLineTriangle");
            Benchmark(Org, "Org");
        } else {
            Benchmark(Org, "Org");
            Benchmark(TestLineTriangle, "TestLineTriangle");
        }
        printf("\n\n");
        */
        Test("TestLineTriangle", Org, TestLineTriangle, std::equal_to{}, line, vtxs.data(), tri, tripl);
    }

    // TestSphereTriangle
    {
        const auto sp = RandomSphere();

        const auto vtxs  = RandomTriangleVertices();
        const auto tri   = CColTriangle{ 0, 1, 2, SURFACE_CAR_PANEL, {} };
        const auto tripl = tri.GetPlane(vtxs.data());

        // Our version seems to fail sometimes, but I'v2 assume they're edge cases
        const auto Org = plugin::CallAndReturn<bool, 0x4165B0, const CColSphere&, const CompressedVector*, const CColTriangle&, const CColTrianglePlane&>;
        Test("TestSphereTriangle", Org, TestSphereTriangle, std::equal_to{}, sp, vtxs.data(), tri, tripl);
    }

    // PointInPoly
    {
        const auto vtxs = RandomTriangleVertices();
        const auto tri = CColTriangle{ 0, 1, 2, SURFACE_CAR_PANEL, {} };
        const auto tripl = tri.GetPlane(vtxs.data());
        const auto pt = RandomVector();
        const CVector ucverts[]{
            vtxs[tri.vA],
            vtxs[tri.vB],
            vtxs[tri.vC]
        };

        const auto Org = plugin::CallAndReturn<bool, 0x415730, const CVector&, const CColTriangle&, const CVector&, const CVector*>;
        Test("PointInPoly", Org, PointInPoly, std::equal_to{}, pt, tri, tripl.GetNormal(), ucverts);
    }

#endif

    // ProcessLineBox
    /*{
        const auto Org = [&](auto line, auto bb) {
            CColPoint cp{};
            float depth{ 100.f };
            const bool s = plugin::CallAndReturn<bool, 0x413100, CColLine const&, CColBox const&, CColPoint&, float&>(line, bb, cp, depth);
            return std::make_tuple(cp, depth, s);
        };

        const auto Rev = [](auto line, auto bb) {
            CColPoint cp{};
            float depth{ 100.f };
            const bool s = ProcessLineBox(line, bb, cp, depth);
            return std::make_tuple(cp, depth, s);
        };

        const auto CmpEq = [&](auto org, auto rev) {
            auto [org_cp, org_d, org_s] = org;
            auto [rev_cp, rev_d, rev_s] = rev;
            return org_s == rev_s && approxEqual(rev_d, org_d, 0.001f) && ColPointEq(org_cp, rev_cp);
        };

        Test("ProcessLineBox", Org, Rev, CmpEq, RandomLine(), RandomBox());
    }*/
#endif
}
