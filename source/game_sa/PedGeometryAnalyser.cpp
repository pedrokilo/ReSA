/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGeometryAnalyser
 * Reconstrucción: COMPLETA — 38/38 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 29.
 * Validación registrada: 38/38 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   CanPedJumpObstacle- | 0x005F1B00 | S
 *   CanPedJumpObstacle-contacted | 0x005F32D0 | S
 *   CanPedTargetPoint | 0x005F1B70 | S
 *   ComputeBuildingHitPoints | 0x005F1E30 | S
 *   ComputeClearTarget | 0x005F5D80 | S
 *   ComputeClosestSurfacePoint-posn | 0x005F36F0 | S
 *   ComputeClosestSurfacePoint-rect | 0x005F2C10 | S
 *   ComputeEntityBoundingBoxCornersUncached | 0x005F1FA0 | S
 *   ComputeEntityBoundingBoxPlanes | 0x005F3660 | S
 *   ComputeEntityBoundingBoxSegmentPlanesUncached | 0x005F1750 | S
 *   ComputeEntityBoundingSphere | 0x005F3C20 | S
 *   ComputeEntityDirs | 0x005F1500 | S
 *   ComputeEntityHitSide-1 | 0x005F3BC0 | S
 *   ComputeEntityHitSide-2 | 0x005F1450 | S
 *   ComputeEntityHitSide-3 | 0x005F3AC0 | S
 *   ComputeMoveDirToAvoidEntity | 0x005F3730 | S
 *   ComputePedHitSide-physical | 0x005F3640 | S
 *   ComputePedHitSide-posn | 0x005F1E70 | S
 *   ComputePedShotSide | 0x005F13F0 | S
 *   ComputeRouteRoundEntityBoundingBox-1 | 0x005F6110 | S
 *   ComputeRouteRoundEntityBoundingBox-2 | 0x005F3DD0 | S
 *   ComputeRouteRoundSphere | 0x005F1890 | S
 *   GetIsLineOfSightClear-ped | 0x005F5A30 | S
 *   GetIsLineOfSightClear-v3d | 0x005F2F00 | S
 *   GetNearestPed | 0x005F3590 | S
 *   IsEntityBlockingTarget | 0x005F3970 | S
 *   IsInAir | 0x005F1CB0 | S
 *   IsWanderPathClear | 0x005F2F70 | S
 *   LiesInsideBoundingBox | 0x005F3880 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedGeometryAnalyser.h"
#include "Interior/InteriorManager_c.h"

void CPedGeometryAnalyser::InjectHooks() {
    RH_ScopedClass(CPedGeometryAnalyser);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "", 0x5F1B00, bool(*)(const CPed&,const CEntity&));
    RH_ScopedOverloadedInstall(CanPedJumpObstacle, "contacted", 0x5F32D0, bool(*)(const CPed&,const CEntity&,const CVector&,const CVector&));
    RH_ScopedInstall(CanPedTargetPed, 0x5F1C40);
    RH_ScopedInstall(CanPedTargetPoint, 0x5F1B70);
    RH_ScopedInstall(ComputeBuildingHitPoints, 0x5F1E30);
    RH_ScopedInstall(ComputeClearTarget, 0x5F5D80);
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "ped", 0x5F3B70, bool (*)(const CPed& ped, CEntity& entity, CVector& point));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "posn", 0x5F36F0, bool(*)(const CVector&,CEntity&,CVector&));
    RH_ScopedOverloadedInstall(ComputeClosestSurfacePoint, "rect", 0x5F2C10, bool(*)(const CVector&,const CVector*,CVector&));
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncached, 0x5F1600);
    RH_ScopedInstall(ComputeEntityBoundingBoxCentreUncachedAll, 0x5F3B40);
    RH_ScopedInstall(ComputeEntityBoundingBoxCorners, 0x5F3650);
    RH_ScopedInstall(ComputeEntityBoundingBoxCornersUncached, 0x5F1FA0);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanes, 0x5F3660);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncached, 0x5F1670);
    RH_ScopedInstall(ComputeEntityBoundingBoxPlanesUncachedAll, 0x5F2B80);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanes, 0x5F36A0);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncached, 0x5F1750);
    RH_ScopedInstall(ComputeEntityBoundingBoxSegmentPlanesUncachedAll, 0x5F2BC0);
    RH_ScopedInstall(ComputeEntityBoundingSphere, 0x5F3C20);
    RH_ScopedInstall(ComputeMoveDirToAvoidEntity, 0x5F3730);
    RH_ScopedInstall(ComputeEntityDirs, 0x5F1500);
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "1", 0x5F3BC0, int32 (*)(const CPed& ped, CEntity& entity));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "2", 0x5F1450, int32 (*)(const CVector& point1, const CVector* point2, const float* x));
    RH_ScopedOverloadedInstall(ComputeEntityHitSide, "3", 0x5F3AC0, int32 (*)(const CVector& point, CEntity& entity));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "physical", 0x5F3640, int32(*)(const CPed&,const CPhysical&));
    RH_ScopedOverloadedInstall(ComputePedHitSide, "posn", 0x5F1E70, int32(*)(const CPed&,const CVector&));
    RH_ScopedInstall(ComputePedShotSide, 0x5F13F0);
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "1", 0x5F6110, int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32));
    RH_ScopedOverloadedInstall(ComputeRouteRoundEntityBoundingBox, "2", 0x5F3DD0, int32(*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32));
    RH_ScopedInstall(ComputeRouteRoundSphere, 0x5F1890);
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "ped", 0x5F5A30, bool(*)(const CPed&,const CVector&,CEntity&,float&));
    RH_ScopedOverloadedInstall(GetIsLineOfSightClear, "v3d", 0x5F2F00, bool(*)(const CVector&,const CVector&,CEntity&));
    RH_ScopedInstall(GetNearestPed, 0x5F3590);
    RH_ScopedInstall(IsEntityBlockingTarget, 0x5F3970);
    RH_ScopedInstall(IsInAir, 0x5F1CB0);
    RH_ScopedInstall(IsWanderPathClear, 0x5F2F70);
    RH_ScopedInstall(LiesInsideBoundingBox, 0x5F3880);
}

// 0x5F1B00
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity) {
    if (entity.m_bIsTempBuilding) { return false; }
    const auto& origin = ped.GetPosition();
    const auto target = ped.GetForward() + origin;
    return CWorld::GetIsLineOfSightClear(origin, target, true, false, false, true, false, false, false);
}

// 0x5F32D0
bool CPedGeometryAnalyser::CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector& contactNormal, const CVector& contactPos) {
    auto origin = ped.GetPosition();
    auto direction = ped.GetForward();
    if (entity.m_bIsTempBuilding) { return false; }
    if (g_surfaceInfos.IsShallowWater(ped.m_nContactSurface)) { return true; }
    if (contactNormal.z > std::bit_cast<float>(0x3E2E147Bu)) {
        if (contactNormal.z > .9f) { return false; }
        const auto& sphere = ped.GetColModel()->m_pColData->m_pSpheres[0];
        origin.z += sphere.m_vecCenter.z - sphere.m_fRadius * contactNormal.z;
        const auto horizontal = std::sqrt(contactNormal.y * contactNormal.y + contactNormal.x * contactNormal.x);
        if (contactNormal.z > .5f) {
            direction = CVector{-contactNormal.x, -contactNormal.y, 0.f} / horizontal;
            direction += direction * horizontal * sphere.m_fRadius;
            direction *= std::min(2.f / horizontal, 4.f);
        } else {
            direction += direction * horizontal * sphere.m_fRadius;
        }
    } else if (!CPedGroups::IsInPlayersGroup(const_cast<CPed*>(&ped))) {
        origin.z -= .15f;
    }
    const auto target = direction + origin;
    if (!CWorld::GetIsLineOfSightClear(origin, target, true, false, false, true, false, false, false)) { return false; }
    const auto landing = direction * 3.f + origin;
    bool found = false;
    const auto ground = CWorld::FindGroundZFor3DCoord(landing, &found, nullptr);
    return found && landing.z - ground < 3.f;
}

// 0x5F1C40
bool CPedGeometryAnalyser::CanPedTargetPed(CPed& ped, CPed& targetPed, bool checkDirection) {
    // Como el original (0x5F1C4E-0x5F1C77): x e y se copian tal cual y z se calcula como (z + 0.75f) redondeado a float;
    // si el objetivo esta agachado se resta 1.0f DESPUES (no se suma -0.25f de golpe: el redondeo puede diferir).
    const auto& posn = targetPed.GetPosition();
    CVector target{ posn.x, posn.y, posn.z + 0.75f };
    if (targetPed.GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK)) {
        target.z -= 1.0f;
    }
    return CanPedTargetPoint(ped, target, checkDirection);
}

// 0x5F1B70
bool CPedGeometryAnalyser::CanPedTargetPoint(const CPed& ped, const CVector& a2, bool a3) {
    const auto delta = a2 - ped.GetPosition();
    if (a3) {
        const auto& forward = ped.GetForward();
        if ((delta.z * forward.z + delta.y * forward.y) + delta.x * forward.x < 0.f) { return false; }
    }
    if ((delta.z * delta.z + delta.x * delta.x) + delta.y * delta.y > 1600.f) { return false; }
    const auto origin = ped.GetPosition() + CVector{0.f, 0.f, .75f};
    return CWorld::GetIsLineOfSightClear(origin, a2, true, false, false, true, false, true, false);
}

// 0x5F1E30
// unused
int32 CPedGeometryAnalyser::ComputeBuildingHitPoints(const CVector& a1, const CVector& a2) {
    CEntity *outEntity;
    CColPoint v4;

    CWorld::ProcessLineOfSight(a1, a2, v4, outEntity, true, false, false, false, true, false, false, false);
    return CWorld::ms_iProcessLineNumCrossings;
}

// 0x5F5D80
void CPedGeometryAnalyser::ComputeClearTarget(const CPed& ped, const CVector& a2, CVector& a3) {
    a3 = a2;
    const auto& origin = ped.GetPosition();
    const auto epsilon = *reinterpret_cast<const float*>(0x8D22B0);
    const auto avoid = [&](CEntity* entity) {
        if (!entity) { return; }
        const auto delta = entity->GetPosition() - a3;
        if ((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x < 25.f && LiesInsideBoundingBox(ped, a3, *entity)) {
            float distance;
            if (!GetIsLineOfSightClear(ped, a3, *entity, distance)) {
                auto direction = a3 - origin;
                direction.Normalise();
                a3 -= direction * (epsilon + distance);
            }
        }
    };
    for (auto entity : ped.GetIntelligence()->m_vehicleScanner.m_apEntities) { avoid(entity); }
    for (auto entity : ped.GetIntelligence()->m_pedScanner.m_apEntities) { avoid(entity); }
    auto direction = origin - a3;
    direction.Normalise();
    direction *= epsilon;
    const auto samples = static_cast<int32>(5.f / epsilon) + 1;
    for (auto i = 0; i < samples; ++i) {
        const auto delta = origin - a3;
        if ((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x >= 25.f || (delta.x * direction.x + delta.y * direction.y) + direction.z * delta.z < 0.f) { return; }
        CColPoint point; CEntity* entity;
        CWorld::ProcessLineOfSight(origin, a3, point, entity, true, false, false, false, true, false, false, false);
        if (CWorld::ms_iProcessLineNumCrossings % 2 != 1) { return; }
        a3 += direction;
    }
}

// 0x5F3B70
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point) {
    CVector corners[4];
    const auto& posn = ped.GetPosition();
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F36F0
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, CEntity& entity, CVector& point) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);
}

// 0x5F2C10
bool CPedGeometryAnalyser::ComputeClosestSurfacePoint(const CVector& posn, const CVector* corners, CVector& point) {
    auto minimum = std::numeric_limits<float>::max();
    bool found = false;
    for (auto i = 0; i < 4; ++i) {
        const auto edge = corners[(i + 1) % 4] - corners[i];
        const auto length = std::sqrt((edge.z * edge.z + edge.y * edge.y) + edge.x * edge.x);
        const auto normal = edge * (1.f / length);
        const auto relative = posn - corners[i];
        const auto projection = (relative.z * normal.z + relative.y * normal.y) + relative.x * normal.x;
        if (projection >= 0.f && projection <= length) {
            const auto candidate = corners[i] + normal * projection;
            const auto delta = posn - candidate;
            const auto squared = (delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x;
            if (squared < minimum) { minimum = squared; point = candidate; found = true; }
        }
    }
    // Endpoint fallback is only performed when no edge projection was accepted.
    if (!found) {
        for (auto i = 0; i < 4; ++i) {
            const auto delta = corners[i] - posn;
            const auto squared = (delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x;
            if (squared < minimum) { minimum = squared; point = corners[i]; found = true; }
        }
    }
    return found;
}

// inlined into CPedGeometryAnalyser::ComputeEntityBoundingSphere
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentre(float zPos, CEntity& entity, CVector& center) {
    ComputeEntityBoundingBoxCentreUncachedAll(zPos, entity, center);
}

// 0x5F1600
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached(float zPos, const CVector* corners, CVector& center) {
    center.Set(0.0f, 0.0f, zPos);

    center.x = corners[0].x;
    center.y = corners[0].y;

    center.x += corners[1].x;
    center.y += corners[1].y;

    center.x += corners[2].x;
    center.y += corners[2].y;

    center.x += corners[3].x;
    center.y += corners[3].y;

    center.x *= 0.25f;
    center.y *= 0.25f;
}

// 0x5F3B40
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncachedAll(float zPos, CEntity& entity, CVector& center) {
    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
}

// 0x5F3650
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners(float zPos, CEntity& entity, CVector* corners) {
    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
}

// 0x5F1FA0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, CVector* corners) {
    if (entity.GetIsTypeBuilding() && entity.m_bIsTempBuilding && g_interiorMan.GetBoundingBox(&entity, corners)) {
        for (auto i = 0; i < 4; ++i) { corners[i].z = zPos; }
        return;
    }
    auto& matrix = entity.GetMatrix();
    const auto* model = entity.GetColModel();
    const auto* data = model->m_pColData;
    auto lower = model->m_boundBox.m_vecMin;
    auto upper = model->m_boundBox.m_vecMax;
    if (entity.GetIsTypeObject() && data && upper.z - lower.z > 6.f && (data->m_nNumSpheres || data->m_nNumBoxes)) {
        lower = CVector{FLT_MAX, FLT_MAX, FLT_MAX};
        upper = CVector{-FLT_MAX, -FLT_MAX, -FLT_MAX};
        const auto consider = [&](const CVector& lo, const CVector& hi) {
            const auto worldLo = matrix.TransformPoint(lo), worldHi = matrix.TransformPoint(hi);
            if ((zPos - 1.f <= worldLo.z || zPos - 1.f <= worldHi.z) && (worldLo.z <= zPos + 1.f || worldHi.z <= zPos + 1.f)) {
                lower.x = lower.x < lo.x ? lower.x : lo.x;
                lower.y = lower.y < lo.y ? lower.y : lo.y;
                lower.z = lower.z < lo.z ? lower.z : lo.z;
                upper.x = hi.x < upper.x ? upper.x : hi.x;
                upper.y = hi.y < upper.y ? upper.y : hi.y;
                upper.z = hi.z < upper.z ? upper.z : hi.z;
            }
        };
        for (auto i = 0; i < data->m_nNumBoxes; ++i) { consider(data->m_pBoxes[i].m_vecMin, data->m_pBoxes[i].m_vecMax); }
        for (auto i = 0; i < data->m_nNumSpheres; ++i) {
            const auto& sphere = data->m_pSpheres[i];
            const auto radius = CVector{sphere.m_fRadius, sphere.m_fRadius, sphere.m_fRadius};
            consider(sphere.m_vecCenter - radius, sphere.m_vecCenter + radius);
        }
    }
    const auto padding = *reinterpret_cast<const float*>(0x8D22B0);
    const CVector expandedLower{lower.x - padding, lower.y - padding, lower.z - padding};
    const CVector expandedUpper{upper.x + padding, upper.y + padding, upper.z + padding};
    const CVector extent{float((double(expandedUpper.x)-expandedLower.x)*.5),float((double(expandedUpper.y)-expandedLower.y)*.5),float((double(expandedUpper.z)-expandedLower.z)*.5)};
    const auto center = matrix.TransformPoint((expandedUpper + expandedLower) * .5f);
    const CVector axes[]{matrix.GetRight(), matrix.GetForward(), matrix.GetUp()};
    const float half[]{extent.x, extent.y, extent.z};
    float widths[3];
    for (auto i = 0; i < 3; ++i) {
        const auto norm = axes[i].y * axes[i].y + axes[i].x * axes[i].x;
        widths[i] = half[i] * norm + half[i] * norm;
    }
    const auto selected = widths[0] > widths[1] && widths[0] > widths[2] ? 0 : widths[1] > widths[2] ? 1 : 2;
    const CVector axis{axes[selected].x, axes[selected].y, 0.f};
    const auto positive = axis * half[selected] + center;
    const auto negative = center - axis * half[selected];
    auto normal = axis;
    normal.Normalise();
    float along = 0.f, across = 0.f;
    // Other axes are visited in the same order as the executable's three cases.
    const int others[3][2]{{1, 2}, {0, 2}, {0, 1}};
    for (auto j : others[selected]) {
        auto parallel = (normal.y * axes[j].y + normal.x * axes[j].x) * half[j];
        auto perpendicular = (normal.x * axes[j].y - normal.y * axes[j].x) * half[j];
        if (parallel < 0.f) { parallel = -parallel; }
        if (perpendicular < 0.f) { perpendicular = -perpendicular; }
        along += parallel;
        across += perpendicular;
    }
    const auto extra = normal * along;
    const CVector side{across * normal.y, -normal.x * across, across * 0.f};
    corners[0] = positive + extra - side;
    corners[1] = negative - extra - side;
    corners[2] = negative - extra + side;
    corners[3] = positive + extra + side;
    for (auto i = 0; i < 4; ++i) { corners[i].z = zPos; }

}

// 0x5F3660
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector(*outPlanes)[4], float* outPlanesDot) {
    ComputeEntityBoundingBoxPlanesUncachedAll(zPos, entity, outPlanes, outPlanesDot);
}

namespace {
float GeometryPlaneOffset(const CVector* plane, const CVector* previous) {
    float result;
    // 0x5F16EC..0x5F171B: retain x87 products/sums until the final float store.
    __asm {
        mov ecx, plane
        mov edx, previous
        fld dword ptr [edx+8]
        fmul dword ptr [ecx+8]
        fld dword ptr [edx+4]
        fmul dword ptr [ecx+4]
        faddp st(1), st
        fld dword ptr [edx]
        fmul dword ptr [ecx]
        faddp st(1), st
        fchs
        fstp result
    }
    return result;
}
}

// 0x5F1670
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(float zPos, const CVector* corners, CVector(*outPlanes)[4], float* outPlanesDot) {
    const CVector* corner2 = &corners[3];
    for (auto i = 0; i < 4; i++) {
        const CVector& corner = corners[i];
        CVector& plane = (*outPlanes)[i];
        CVector direction = corner - *corner2;
        direction.Normalise();
        plane.x = direction.y;
        plane.y = -direction.x;
        plane.z = 0.0f;
        // point-normal plane equation:
        // ax + by + cz + d = 0
        // d = - n . P
        outPlanesDot[i] = GeometryPlaneOffset(&plane, corner2);

        corner2 = &corner;
    }
}

// 0x5F2B80
void CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, CVector (*outPlanes)[4], float* outPlanesDot) {
    CVector corners[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached(zPos, corners, outPlanes, outPlanesDot);
}

// 0x5F36A0
void CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, CVector* normals, float* dots) {
    ComputeEntityBoundingBoxSegmentPlanesUncachedAll(zPos, entity, normals, dots);
}

// 0x5F1750
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached(const CVector* corners, CVector& center, CVector* a3, float* a4) {
    for (auto i = 0; i < 4; ++i) {
        a3[i] = CVector{ -(corners[i].y - center.y), corners[i].x - center.x, 0.f };
        a4[i] = -((a3[i].z * corners[i].z + a3[i].y * corners[i].y) + a3[i].x * corners[i].x);
    }
    // The original leaves its input corner pointer in EAX.
    return const_cast<CVector*>(corners);
}

// 0x5F2BC0
CVector* CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, CVector* a3, float* a4) {
    CVector corners[4];
    CVector center;

    ComputeEntityBoundingBoxCornersUncached(zPos, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(zPos, corners, center);
    return ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, a3, a4);
}

// 0x5F3C20
void CPedGeometryAnalyser::ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere& a3) {
    CVector corners[4], center;
    ComputeEntityBoundingBoxCornersUncached(ped.GetPosition().z, entity, corners);
    ComputeEntityBoundingBoxCentre(ped.GetPosition().z, entity, center);
    auto maximum = 0.f;
    for (const auto& corner : corners) {
        const auto delta = corner - center;
        const auto squared = (delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x;
        if (squared > maximum) { maximum = squared; }
    }
    a3.Set(std::sqrt(maximum) * 1.1f, center, static_cast<eSurfaceType>(0), 0, tColLighting{0xFF});
}

// 0x5F3730
int32 CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& outDirToAvoidEntity) {
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    const auto& position = ped.GetPosition();
    const auto first = (planes[1].y * position.y + planes[1].z * position.z) + planes[1].x * position.x + dots[1];
    // Preserve the original third-plane Z product and branch-specific EAX values.
    const auto second = (planes[3].y * position.y + planes[3].z * planes[3].z) + planes[3].x * position.x + dots[3];
    if (first > 0.f) { outDirToAvoidEntity = planes[1]; return static_cast<int32>(reinterpret_cast<uintptr_t>(&outDirToAvoidEntity)); }
    if (second > 0.f) { outDirToAvoidEntity = planes[3]; return std::bit_cast<int32>(planes[3].z); }
    if (first > second) { outDirToAvoidEntity = planes[1]; return std::bit_cast<int32>(planes[1].y); }
    outDirToAvoidEntity = planes[3];
    return static_cast<int32>(reinterpret_cast<uintptr_t>(&outDirToAvoidEntity));
}

//! @notsa
CVector CPedGeometryAnalyser::ComputeEntityDir(const CEntity& entity, eDirection dir) {
    switch (dir) {
    case eDirection::FORWARD:  return entity.GetForward();
    case eDirection::LEFT:     return -entity.GetRight();
    case eDirection::BACKWARD: return -entity.GetForward();
    case eDirection::RIGHT:    return entity.GetRight();
    default:                   NOTSA_UNREACHABLE();
    }
}

// 0x5F1500
CVector* CPedGeometryAnalyser::ComputeEntityDirs(const CEntity& entity, CVector* posn) {
    CVector forward, right;
    if (entity.m_matrix) {
        forward = entity.m_matrix->GetForward();
        right = entity.m_matrix->GetRight();
    } else {
        const auto heading = entity.GetHeading();
        float sine, cosine;
        __asm {
            fld heading
            fsin
            fstp sine
            fld heading
            fcos
            fstp cosine
        }
        forward = CVector{ -sine, cosine, 0.f };
        right = CVector{ cosine, sine, 0.f };
    }
    posn[0] = forward;
    posn[1] = -right;
    posn[2] = -forward;
    posn[3] = right;
    return posn + 3;
}

// 0x5F3BC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CPed& ped, CEntity& entity) {
    return ComputeEntityHitSide(ped.GetPosition(), entity);
}

// 0x5F1450
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point1, const CVector* point2, const float* x) {
    for (auto side = 0; side < 4; ++side) {
        const auto previous = (side + 3) % 4;
        const auto& before = point2[previous];
        const auto& after = point2[side];
        const auto p = (before.y * point1.y + before.z * point1.z) + point1.x * before.x + x[previous];
        const auto q = (after.y * point1.y + after.z * point1.z) + point1.x * after.x + x[side];
        if (p >= 0.f && q < 0.f) {
            return side;
        }
    }
    return 0;
}

// 0x5F3AC0
int32 CPedGeometryAnalyser::ComputeEntityHitSide(const CVector& point, CEntity& entity) {
    CVector corners[4], normals[4], center;
    float dots[4];
    ComputeEntityBoundingBoxCornersUncached(point.z, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(point.z, corners, center);
    ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, normals, dots);
    return ComputeEntityHitSide(point, normals, dots);
}

// 0x5F3640
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CPhysical& physical) {
    return ComputePedHitSide(ped, physical.m_vecMoveSpeed);
}

// 0x5F1E70
int32 CPedGeometryAnalyser::ComputePedHitSide(const CPed& ped, const CVector& posn) {
    auto direction = -posn;
    direction.Normalise();
    CVector dirs[4];
    ComputeEntityDirs(ped, dirs);
    const auto dot = [&direction](const CVector& v) { return (v.z * direction.z + v.y * direction.y) + v.x * direction.x; };
    float maximum = -1.f;
    const auto first = dot(dirs[0]);
    int32 side = std::bit_cast<int32>(first);
    if (first >= maximum) { maximum = first; side = 0; }
    for (auto i = 1; i < 4; ++i) {
        const auto value = dot(dirs[i]);
        if (value >= maximum) { maximum = value; side = i; }
    }
    return side;
}

// 0x5F13F0
int32 CPedGeometryAnalyser::ComputePedShotSide(const CPed& ped, const CVector& posn) {
    const auto* origin = &ped.GetPosition();
    const auto* target = &posn;
    const auto rotation = ped.m_fCurrentRotation;
    constexpr float quarter = std::bit_cast<float>(0x3F490FDBu);
    constexpr float full = std::bit_cast<float>(0x40C90FDBu);
    constexpr float scale = std::bit_cast<float>(0x3F22F983u);
    int32 result;
    // Keep the original extended-precision atan and truncation sequence.
    __asm {
        mov ecx, origin
        mov edx, target
        fld dword ptr [edx]
        fsub dword ptr [ecx]
        fchs
        fld dword ptr [edx+4]
        fsub dword ptr [ecx+4]
        fpatan
        fsub rotation
        fadd quarter
        fldz
        fxch st(1)
        fcom st(1)
        fnstsw ax
        fstp st(1)
        test ah, 5
        jp nonnegative
        fadd full
    nonnegative:
        fmul scale
        mov eax, 0821B40h
        call eax
        mov result, eax
    }
    return result;
}

// 0x5F6110
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& posn, CPointRoute& pointRoute, int32 a5) {
    return ComputeRouteRoundEntityBoundingBox(ped, ped.GetPosition(), entity, posn, pointRoute, a5);
}

namespace {
struct GeometryRouteLength355 { uint8 storage[10]{}; };
void GeometryRouteLength355Compute(const CVector* points,int count,GeometryRouteLength355* result) {
    __asm {
        mov edx,points
        mov ecx,count
        dec ecx
        fldz
        test ecx,ecx
        jle geometry_route_length_done
    geometry_route_length_loop:
        fld dword ptr [edx+0Ch]
        fsub dword ptr [edx]
        fmul st,st(0)
        fld dword ptr [edx+10h]
        fsub dword ptr [edx+4]
        fmul st,st(0)
        faddp st(1),st
        fld dword ptr [edx+14h]
        fsub dword ptr [edx+8]
        fmul st,st(0)
        faddp st(1),st
        fsqrt
        faddp st(1),st
        add edx,0Ch
        dec ecx
        jne geometry_route_length_loop
    geometry_route_length_done:
        mov edx,result
        fstp tbyte ptr [edx]
    }
}
bool GeometryRouteLength355Less(const GeometryRouteLength355* first,const GeometryRouteLength355* second) {
    uint16 status;
    __asm {
        mov edx,second
        fld tbyte ptr [edx]
        mov edx,first
        fld tbyte ptr [edx]
        fcompp
        fnstsw status
    }
    return (status&0x4500)==0x0100;
}
}
// 0x5F3DD0
int32 CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& a2, CEntity& entity, const CVector& a4, CPointRoute& pointRoute, int32 a6){
    pointRoute.Clear();
    auto& padding = *reinterpret_cast<float*>(0x8D22B0);
    const auto savedPadding = padding;
    padding = .175f;
    CVector corners[4], planes[4], temporaryCorners[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxCornersUncached(z, entity, temporaryCorners);
    ComputeEntityBoundingBoxPlanesUncached(z, temporaryCorners, &planes, dots);
    padding = savedPadding;
    const auto planeDistance = [](const CVector& p, const CVector& n, float d) { return (p.z * n.z + p.x * n.x) + p.y * n.y + d; };
    for (auto i = 0; i < 4; ++i) {
        if (planeDistance(a2, planes[i], dots[i]) > 0.f && planeDistance(a4, planes[i], dots[i]) > 0.f) { return 0; }
    }
    CVector sideVertices[4], sideCenter, sideNormals[4]; float sideDots[4];
    ComputeEntityBoundingBoxCornersUncached(z, entity, sideVertices);
    ComputeEntityBoundingBoxCentreUncached(z, sideVertices, sideCenter);
    ComputeEntityBoundingBoxSegmentPlanesUncached(sideVertices, sideCenter, sideNormals, sideDots);
    if (ComputeEntityHitSide(a2, sideNormals, sideDots) == ComputeEntityHitSide(a4, sideNormals, sideDots)) { return 0; }
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxCornersUncached(z, entity, temporaryCorners);
    ComputeEntityBoundingBoxPlanesUncached(z, temporaryCorners, &planes, dots);
    auto before = a2, after = a4;
    const auto originalBefore = before;
    auto direction = after - before;
    direction.Normalise();
    bool adjusted = false;
    const auto classify = [](float value) { return value > .2f ? 1 : value < -.2f ? -1 : 0; };
    for (auto i = 0; i < 4; ++i) {
        const auto p = planeDistance(before, planes[i], dots[i]);
        const auto q = planeDistance(after, planes[i], dots[i]);
        const auto first = classify(p), second = classify(q);
        const auto crossing = [&] {
            const auto denominator = (direction.z * planes[i].z + direction.x * planes[i].x) + direction.y * planes[i].y;
            return direction * (-planeDistance(originalBefore, planes[i], dots[i]) / denominator) + originalBefore;
        };
        if (first == -1) {
            if (second == 1) { after = crossing(); }
        } else if (second == -1) {
            if (first == 1) { before = crossing(); }
        } else {
            adjusted = true;
            if (first == 0) { before += planes[i] * (.2f - p); }
            if (second == 0) { after += planes[i] * (.2f - q); }
        }
    }
    if (!adjusted) {
        CColSphere sphere;
        ComputeEntityBoundingSphere(ped, entity, sphere);
        CVector planar{a4.x - a2.x, a4.y - a2.y, 0.f};
        if (std::sqrt(planar.y * planar.y + planar.x * planar.x) == 0.f) { return 0; }
        planar.Normalise();
        if (!sphere.IntersectRay(a2, planar, before, after)) { return 0; }
    }
    const auto segmentOrigin = before;
    direction = after - before;
    direction.Normalise();
    int enter = -1, leave = -1;
    for (auto i = 0; i < 4; ++i) {
        const auto p = planeDistance(before, planes[i], dots[i]);
        const auto q = planeDistance(after, planes[i], dots[i]);
        if (p == 0.f && q == 0.f) { continue; }
        const auto denominator = (direction.x * planes[i].x + direction.y * planes[i].y) + direction.z * planes[i].z;
        const auto crossing = direction * (-planeDistance(segmentOrigin, planes[i], dots[i]) / denominator) + segmentOrigin;
        if (p >= 0.f && q < 0.f) { enter = i; before = crossing; }
        else if (p < 0.f && q >= 0.f) { leave = i; after = crossing; }
    }
    if (enter < 0 || leave < 0) { return 0; }
    CVector paths[2][6];
    int sizes[2]{2, 2};
    paths[0][0] = paths[1][0] = a2;
    auto index = (enter + 3) % 4;
    const auto lastReverse = leave % 4;
    paths[0][1] = corners[index];
    while (index != lastReverse) {
        index = (index + 3) % 4;
        paths[0][sizes[0]++] = corners[index];
    }
    paths[0][sizes[0]++] = a4;
    index = enter % 4;
    const auto lastForward = (leave + 3) % 4;
    paths[1][1] = corners[index];
    while (index != lastForward) {
        index = (index + 1) % 4;
        paths[1][sizes[1]++] = corners[index];
    }
    paths[1][sizes[1]++] = a4;
    int selected = a6 == 1 ? 0 : 1;
    if (a6 == 0) {
        CVector center;
        center.x = ((corners[0].x + corners[1].x) + corners[2].x + corners[3].x) * .25f;
        center.y = ((corners[0].y + corners[1].y) + corners[2].y + corners[3].y) * .25f;
        center.z = ((corners[0].z + corners[1].z) + corners[2].z + corners[3].z) * .25f;
        struct RouteTest { bool clear; int stop; CEntity* obstacle; GeometryRouteLength355 length; } tests[2];
        for (auto path = 0; path < 2; ++path) {
            tests[path] = {true, sizes[path], nullptr, {}};
            for (auto i = 1; i < sizes[path]; ++i) {
                const auto& from = paths[path][i-1]; const auto& to = paths[path][i];
                auto planar = CVector{to.x - from.x, to.y - from.y, 0.f};
                planar.Normalise();
                CVector offset{planar.y * .5f, -planar.x * .5f, 0.f};
                const auto relative = to - center;
                if ((relative.z * 0.f + relative.y * offset.y) + relative.x * offset.x < 0.f) { offset *= -1.f; }
                CColPoint hit;
                auto& obstacle = tests[path].obstacle;
                if (CWorld::ProcessLineOfSight(from, to, hit, obstacle, true, true, true, true, false, false, false, false) || CWorld::ProcessLineOfSight(from + offset, to + offset, hit, obstacle, true, true, true, true, false, false, false, false)) {
                    if (obstacle != &entity && obstacle != &ped) {
                        const auto passPed = obstacle->GetIsTypePed() && reinterpret_cast<CPed*>(obstacle)->m_nPedState != static_cast<ePedState>(1) && [&] {
                            const auto& forward = obstacle->GetMatrix().GetForward();
                            const auto movement = to - from;
                            return (movement.x * forward.x + movement.y * forward.y) + movement.z * forward.z >= 0.f;
                        }();
                        if (!passPed) { tests[path].clear = false; tests[path].stop = i; break; }
                    } else { obstacle = nullptr; }
                }
            }
            GeometryRouteLength355Compute(paths[path],sizes[path],&tests[path].length);
        }
        if (!tests[0].clear) {
            if (tests[1].clear) { selected = 1; }
            else if (tests[0].stop == 1 && tests[1].stop >= 2) { selected = 1; }
            else if (tests[0].stop > 1 && tests[1].stop == 1) { selected = 0; }
            else if (tests[0].obstacle == tests[1].obstacle) { selected = GeometryRouteLength355Less(&tests[0].length,&tests[1].length) ? 0 : 1; }
            else { selected = tests[0].obstacle->GetColModel()->m_boundSphere.m_fRadius < tests[1].obstacle->GetColModel()->m_boundSphere.m_fRadius ? 0 : 1; }
        } else if (!tests[1].clear) { selected = 0; }
        else { selected = GeometryRouteLength355Less(&tests[0].length,&tests[1].length) ? 0 : 1; }
    }
    for (auto i = 1; i < sizes[selected]-1; ++i) { pointRoute.AddUnlessFull(paths[selected][i]); }
    pointRoute.AddUnlessFull(a4);
    const auto finalSideAt = [&](const CVector& point) {
        ComputeEntityBoundingBoxCornersUncached(point.z, entity, sideVertices);
        ComputeEntityBoundingBoxCentreUncached(point.z, sideVertices, sideCenter);
        ComputeEntityBoundingBoxSegmentPlanesUncached(sideVertices, sideCenter, sideNormals, sideDots);
        return ComputeEntityHitSide(point, sideNormals, sideDots);
    };
    if (pointRoute.GetSize() > 1 && finalSideAt(a2) == finalSideAt(pointRoute.m_Entries[1])) {
        for (auto i = 0u; i < pointRoute.GetSize()-1; ++i) { pointRoute.m_Entries[i] = pointRoute.m_Entries[i+1]; }
        --pointRoute.m_NumEntries;
    }
    --pointRoute.m_NumEntries;
    return selected + 1;

}

// 0x5F1890
bool CPedGeometryAnalyser::ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& a3, const CVector& a4, CVector& a5, CVector& a6) {
    const auto& origin = ped.GetPosition();
    a5 = a4;
    CVector first, second;
    if (const_cast<CColSphere&>(sphere).IntersectPoint(a4)) {
        auto direction = a4 - a3;
        direction.Normalise();
        if (const_cast<CColSphere&>(sphere).IntersectRay(origin, direction, first, second)) { a5 = second; }
    }
    auto direction = a5 - origin;
    direction.Normalise();
    if (!const_cast<CColSphere&>(sphere).IntersectRay(a5, direction, first, second)) { a6 = a5; return false; }
    const auto distanceSquared = [](const CVector& v) { return (v.z * v.z + v.y * v.y) + v.x * v.x; };
    if (distanceSquared(a5 - origin) < distanceSquared(first - origin)) { a6 = a5; return false; }
    if (const_cast<CColSphere&>(sphere).IntersectRay(origin, direction, first, second)) {
        const auto delta = sphere.m_vecCenter - origin;
        const auto projection = (delta.z * direction.z + delta.y * direction.y) + delta.x * direction.x;
        auto normal = direction * projection + origin - sphere.m_vecCenter;
        normal.Normalise();
        a6 = normal * sphere.m_fRadius + sphere.m_vecCenter;
    }
    return true;
}

// 0x5F5A30
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CPed& ped, const CVector& a2, CEntity& entity, float& a4) {
    auto origin = ped.GetPosition();
    auto target = a2;
    CColSphere sphere;
    ComputeEntityBoundingSphere(ped, entity, sphere);
    auto direction = target - origin;
    direction.Normalise();
    CVector first, second;
    if (!sphere.IntersectRay(origin, direction, first, second)) { return true; }
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    a4 = 0.f;
    const auto epsilon = *reinterpret_cast<const float*>(0x8D22B0);
    const auto classify = [epsilon](float distance) { return distance > epsilon ? 1 : distance < -epsilon ? -1 : 0; };
    for (auto i = 0; i < 4; ++i) {
        const auto& n = planes[i];
        const auto p = (origin.z * n.z + origin.y * n.y) + origin.x * n.x + dots[i];
        const auto q = (target.z * n.z + target.y * n.y) + target.x * n.x + dots[i];
        const auto before = classify(p), after = classify(q);
        if (before < 0) {
            if (after > 0) {
                const auto denominator = (direction.z * n.z + direction.y * n.y) + direction.x * n.x;
                if (denominator > .001f) { target = direction * ((-1.f / denominator) * p) + origin; }
            }
        } else {
            if (after >= 0) { return true; }
            if (before >= 1) {
                const auto denominator = (direction.z * n.z + direction.y * n.y) + direction.x * n.x;
                if (denominator > .001f) { origin = direction * ((-1.f / denominator) * p) + origin; }
            }
        }
    }
    const auto delta = target - origin;
    a4 = std::sqrt((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x);
    return false;
}

// 0x5F2F00
bool CPedGeometryAnalyser::GetIsLineOfSightClear(const CVector& a1, const CVector& a2, CEntity& a3) {
    const CColLine line{a1, a2};
    auto colModel = a3.GetColModel();
    return !CCollision::TestLineOfSight(line, a3.GetMatrix(), *colModel, false, false);
}

// 0x5F3590
CPed* CPedGeometryAnalyser::GetNearestPed(const CVector& point) {
    auto pool = GetPedPool();
    auto minimum = std::numeric_limits<float>::max();
    CPed* nearest = nullptr;
    for (auto i = pool->GetSize(); i > 0; --i) {
        if (auto candidate = pool->GetAt(i - 1)) {
            const auto delta = point - candidate->GetPosition();
            const auto squared = (delta.z * delta.z + delta.x * delta.x) + delta.y * delta.y;
            if (squared < minimum) { minimum = squared; nearest = candidate; }
        }
    }
    return nearest;
}

// 0x5F3970
bool CPedGeometryAnalyser::IsEntityBlockingTarget(CEntity* entity, const CVector& point, float distance) {
    const auto delta = entity->GetPosition() - point;
    if (!(std::abs(delta.z) <= 3.f)) { return false; }
    const auto radius = entity->GetColModel()->m_boundSphere.m_fRadius;
    // The executable compares squared radii with the unsquared XY distance.
    if (!(distance * distance + radius * radius >= std::sqrt(delta.x * delta.x + delta.y * delta.y))) { return false; }
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = entity->GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, *entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    for (auto i = 0; i < 4; ++i) {
        if ((point.y * planes[i].y + planes[i].z * point.z) + planes[i].x * point.x + dots[i] + distance * .5f > 0.f) { return false; }
    }
    return true;
}

// 0x5F1CB0
bool CPedGeometryAnalyser::IsInAir(const CPed& ped) {
    if (ped.bInVehicle) { return false; }
    auto& manager = ped.GetTaskManager();
    if (manager.GetActiveTask()) {
        if (ped.GetIntelligence()->GetTaskSwim() || ped.GetIntelligence()->GetTaskJetPack() || manager.GetSimplestActiveTask()->GetTaskType() == static_cast<eTaskType>(0xFE)) { return false; }
    }
    bool falling = false;
    if (manager.GetActiveTask()) { falling = manager.GetActiveTask()->GetTaskType() == static_cast<eTaskType>(0xD3); }
    auto origin = ped.GetPosition();
    CColPoint point;
    CEntity* entity;
    auto ground = CWorld::ProcessVerticalLine(origin, ped.GetPosition().z - 1.5f, point, entity, true, true, false, true, false, false, nullptr);
    if (!ground && !falling) {
        origin.z -= 1.f;
        ground = CWorld::TestSphereAgainstWorld(origin, .15f, const_cast<CPed*>(&ped), true, false, false, false, false, false) != nullptr;
    }
    return !ground;
}

// 0x5F2F70
CPedGeometryAnalyser::WanderPathClearness CPedGeometryAnalyser::IsWanderPathClear(const CVector& from, const CVector& to, float maxHeightChange, int32 maxSamples) {
    if (std::abs(from.z - to.z) > maxHeightChange) { return WanderPathClearness::BLOCKED_HEIGHT; }
    const auto z = from.z < to.z ? from.z : to.z;
    if (!CWorld::GetIsLineOfSightClear({from.x, from.y, z}, {to.x, to.y, z}, true, false, false, false, false, false, false)) { return WanderPathClearness::BLOCKED_LOS; }
    auto direction = to - from;
    const auto length = std::sqrt((direction.z * direction.z + direction.x * direction.x) + direction.y * direction.y);
    const auto samples = std::min(maxSamples, static_cast<int32>(reinterpret_cast<double(__cdecl*)(double)>(0x8219F0)(length)));
    if (samples != 0) {
        direction.Normalise();
        for (auto i = 1; i < samples; ++i) {
            const auto sample = direction * float(i) + from;
            float water;
            if (CWaterLevel::GetWaterLevel(sample.x, sample.y, sample.z, water, 0, nullptr)) {
                CColPoint point; CEntity* entity = nullptr;
                if (!CWorld::ProcessVerticalLine({sample.x, sample.y, water}, from.z > to.z ? from.z : to.z, point, entity, true, false, false, false, false, false, nullptr)) { return WanderPathClearness::BLOCKED_WATER; }
            }
        }
        CColPoint point; CEntity* entity = nullptr;
        if (!CWorld::ProcessVerticalLine(from, from.z - 5.f, point, entity, true, false, false, false, false, false, nullptr)) { return WanderPathClearness::BLOCKED_SHARP_DROP; }
        auto previous = point.m_vecPoint.z + .5f;
        for (auto i = 1; i < samples; ++i) {
            const auto sample = direction * float(i) + from;
            if (!CWorld::ProcessVerticalLine({sample.x, sample.y, previous}, previous - 2.f, point, entity, true, false, false, false, false, false, nullptr)) { return WanderPathClearness::BLOCKED_SHARP_DROP; }
            if (std::abs(point.m_vecPoint.z - previous) > 1.f) { return WanderPathClearness::BLOCKED_SHARP_DROP; }
            previous = point.m_vecPoint.z + .5f;
        }
    }
    return WanderPathClearness::CLEAR;
}

// 0x5F3880
bool CPedGeometryAnalyser::LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity) {
    const auto delta = posn - entity.GetPosition();
    const auto radius = entity.GetColModel()->m_boundSphere.m_fRadius;
    if (!(radius * radius > (delta.y * delta.y + delta.x * delta.x) + delta.z * delta.z)) { return false; }
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    for (auto i = 0; i < 4; ++i) {
        if ((posn.y * planes[i].y + planes[i].z * posn.z) + planes[i].x * posn.x + dots[i] < 0.f) { return true; }
    }
    return false;
}

// 0x41B7C0
void* CPointRoute::operator new(uint32 size) {
    return GetPointRoutePool()->New();
}

// 0x41B7D0
void CPointRoute::operator delete(void* ptr, size_t sz) {
    GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(ptr));
}
