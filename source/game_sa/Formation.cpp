/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CFormation
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Formation.h"
#include "X87Math.h"

// 0x699F50
void CFormation::ReturnTargetPedForPed(CPed* ped, CPed** pOutTargetPed) {
    for (int32 i = 0; i < int32(m_Peds.m_count); ++i) {
        if (m_Peds.m_peds[i] == ped && m_aPedLinkToDestinations[i] >= 0) {
            *pOutTargetPed = m_DestinationPeds.m_peds[m_aPedLinkToDestinations[i]];
            return;
        }
    }
}

// 0x699FA0
bool CFormation::ReturnDestinationForPed(CPed* ped, CVector* out) {
    // The executable scans all 24 slots, including slots beyond m_count.
    for (int32 i = 0; i < 24; ++i) {
        const auto destination = reinterpret_cast<const int32*>(m_aPedLinkToDestinations.data())[i];
        if (m_Peds.m_peds[i] == ped && destination >= 0) {
            *out = m_Destinations.m_Points[destination];
            return true;
        }
    }
    return false;
}

// 0x699FF0
void CFormation::FindCoverPointsBehindBox(
    CPointList* outPoints, CVector target, CMatrix* mat,
    const CVector& center, const CVector& bbMin, const CVector& bbMax, float cutoffDist
) {
    // The executable compares |target.z - matrix.z| < 7.0f (0x858F48); the boolean is the same with SSE floats:
    // both round the difference to 24 bits and NaN/overflow make the ordered comparison false.
    if (!(std::abs(target.z - mat->GetPosition().z) < 7.0f)) return;
    // 0x59C890 inlined: transform of {x, y, 0}, x87 chains without float stores (FormationTransformPointCpp).
    const CVector locals[4]{
        {bbMin.x, bbMin.y, 0.f},
        {bbMin.x, bbMax.y, 0.f},
        {bbMax.x, bbMax.y, 0.f},
        {bbMax.x, bbMin.y, 0.f}
    };
    CVector corners[4];
    for (int32 i = 0; i < 4; ++i) {
        FormationTransformPointCpp(reinterpret_cast<const float*>(mat), &locals[i].x, &corners[i].x);
    }
    float distances[4];
    for (int32 i = 0; i < 4; ++i) distances[i] = FormationDistance2DCpp(corners[i].x, corners[i].y, target.x, target.y);
    int32 closest = 0;
    for (int32 i = 1; i < 4; ++i) if (distances[i] < distances[closest]) closest = i;
    const auto& opposite = corners[(closest - 2) & 3];
    for (int32 i = 0; i < 4; ++i) {
        if (i == closest || ((i - 2) & 3) == closest || static_cast<int32>(outPoints->m_Count) >= 24 || !(distances[i] < cutoffDist)) continue;
        const auto& corner = corners[i];
        // 0x69A218..0x69A23B: direction = opposite - corner (fstp dword each), z = 0, then CVector::Normalise (0x59C910).
        CVector direction{RpNanSubCpp(opposite.x, corner.x), RpNanSubCpp(opposite.y, corner.y), 0.f};
        (void)PathFindNormaliseStatusCpp(&direction.x);
        // 0x69A240..0x69A260: operand order of the executable (direction + corner for x/y, corner.z + direction.z).
        const CVector point{RpNanAddCpp(direction.x, corner.x), RpNanAddCpp(direction.y, corner.y), RpNanAddCpp(corner.z, direction.z)};
        // 0x69A264..0x69A2B5: away = point - target (z = 0), normalised.
        CVector away{RpNanSubCpp(point.x, target.x), RpNanSubCpp(point.y, target.y), 0.f};
        (void)PathFindNormaliseStatusCpp(&away.x);
        // 0x69A2BA..0x69A2FF: x/y keep away*0.75 in ST0 (RpNanMulAddCpp); away.z*0.75 is stored as float first.
        const float scaledZ = X87TimesFloatCpp(away.z, 0.75f);
        outPoints->m_Points[outPoints->m_Count++] = CVector{
            RpNanMulAddCpp(away.x, 0.75f, point.x),
            RpNanMulAddCpp(away.y, 0.75f, point.y),
            RpNanAddCpp(scaledZ, point.z)
        };
    }
}

// 0x69A620
void CFormation::GenerateGatherDestinations(CPedList& pedList, CPed* ped) {
    m_Destinations.m_Count = 0;
    std::memset(m_Destinations.m_PointHasBeenClaimed, 0, sizeof(m_Destinations.m_PointHasBeenClaimed));
    float radius;
    switch (pedList.m_count) {
    case 1: radius = 1.25f; break;
    case 2: radius = 1.5f; break;
    case 3: radius = 1.75f; break;
    case 4: radius = 2.125f; break;
    default: radius = 2.5f; break;
    }
    const float heading = ped->m_fCurrentRotation;
    for (int32 i = 0; i < int32(pedList.m_count); ++i) {
        CVector point;
        const auto& position = ped->GetPosition();
        FormationGatherPointCpp(i, int32(pedList.m_count), heading, radius, &position.x, &point.x);
        if (m_Destinations.m_Count < m_Destinations.m_Points.size()) {
            m_Destinations.m_Points[m_Destinations.m_Count++] = point;
        }
    }
}

// 0x69A770
void CFormation::GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* veh) {
    const auto& box = veh->GetColModel()->m_boundBox;
    // fld max.x; fadd 1.5; fstp / fld max.y; fsub min.y; fstp (0x69A789..0x69A7A8)
    const float offset = RpNanAddCpp(box.m_vecMax.x, 1.5f);
    const float length = RpNanSubCpp(box.m_vecMax.y, box.m_vecMin.y);
    CVector right = veh->GetMatrix().GetRight();
    CVector forward = veh->GetMatrix().GetForward();
    right.Normalise();
    forward.Normalise();
    m_Destinations.m_Count = 0;
    std::memset(m_Destinations.m_PointHasBeenClaimed, 0, sizeof(m_Destinations.m_PointHasBeenClaimed));
    const int32 firstCount = int32(pedList.m_count) / 2;
    const int32 secondCount = int32(pedList.m_count) - firstCount;
    for (int side = 0; side < 2; ++side) {
        const int32 count = side ? secondCount : firstCount;
        for (int32 i = 0; i < count; ++i) {
            const auto& pos = veh->GetPosition();
            CVector point;
            FormationGatherCarPointCpp(&pos.x, &right.x, offset, &forward.x, length, i, count, side != 0, &point.x);
            if (m_Destinations.m_Count < m_Destinations.m_Points.size())
                m_Destinations.m_Points[m_Destinations.m_Count++] = point;
        }
    }
}

// 0x69B240
void CFormation::DistributeDestinations(CPedList& pedList) {
    m_Peds = pedList;
    const int32 count = int32(m_Peds.m_count);
    if (!count) return;
    const int32 destinations = int32(m_Destinations.m_Count);
    CVector destinationCenter;
    FormationCentroidCpp(&m_Destinations.m_Points[0].x, destinations, true, &destinationCenter.x);
    std::array<CVector, 24> positions;
    for (int32 i = 0; i < count; ++i) {
        positions[i] = m_Peds.m_peds[i]->GetPosition();
    }
    CVector pedCenter;
    FormationCentroidCpp(&positions[0].x, count, false, &pedCenter.x);
    const double destinationRadius = FormationMeanRadiusCpp(&m_Destinations.m_Points[0].x, destinations, &destinationCenter.x);
    const double pedRadius = FormationMeanRadiusCpp(&positions[0].x, count, &pedCenter.x);
    const double scale = FormationRadiusScaleCpp(destinationRadius, pedRadius);
    for (int32 i = 0; i < count; ++i) {
        CVector scaled;
        FormationScalePositionCpp(&positions[i].x, &pedCenter.x, &destinationCenter.x, scale, &scaled.x);
        positions[i] = scaled;
    }
    float best = std::bit_cast<float>(0x497423FEu);
    for (int32 attempt = 0; attempt < count; ++attempt) {
        m_aFinalPedLinkToDestinations.fill(-1);
        std::memset(m_Destinations.m_PointHasBeenClaimed, 0, sizeof(m_Destinations.m_PointHasBeenClaimed));
        float total{};
        for (int32 i = 0; i < count; ++i) {
            // Separate executable helper chooses an unclaimed point and adds its cost.
            const int32 destination = FindNearestFreeDestination(positions[i], &total);
            m_aFinalPedLinkToDestinations[i] = destination;
            m_Destinations.m_PointHasBeenClaimed[destination] = true;
        }
        if (total < best) {
            std::copy_n(m_aFinalPedLinkToDestinations.begin(), count, m_aPedLinkToDestinations.begin());
            best = total;
        }
    }
}

// 0x69B5B0
void CFormation::DistributeDestinations_CoverPoints(const CPedList& pedlist, CVector pos) {
    m_Peds = pedlist;
    if (!m_Peds.m_count) return;
    m_aPedLinkToDestinations.fill(-1);
    for (int32 point = 0; point < int32(m_Destinations.m_Count); ++point) {
        const auto& destination = m_Destinations.m_Points[point];
        // 0x69B5F5..0x69B626: fld dest.x; fsub pos.x; ...; fsqrt; fstp dword (x87 a 24 bits, no doubles de SSE).
        const float targetDistance = FormationDistance2DCpp(destination.x, destination.y, pos.x, pos.y);
        float bestScore = 0.4f;
        int32 bestPed = -1;
        for (int32 ped = 0; ped < int32(m_Peds.m_count); ++ped) {
            if (m_aPedLinkToDestinations[ped] >= 0) continue;
            const auto& position = m_Peds.m_peds[ped]->GetPosition();
            double score;
            // 0x69B653..0x69B6C6: la distancia del peatón y el cociente quedan sin redondear en ST0; sólo el mejor puntaje se guarda como float.
            if (!FormationCoverScoreCpp(&position.x, &destination.x, &pos.x, targetDistance, &score)) continue;
            if (score > double(bestScore)) { // fcom; test ah,41h; jne -> sólo "mayor que" ordenado
                bestScore = float(score);
                bestPed = ped;
            }
        }
        if (bestPed >= 0) m_aPedLinkToDestinations[bestPed] = point;
    }
}

// 0x69B700
void CFormation::DistributeDestinations_PedsToAttack(const CPedList& pedList) {
    m_Peds = pedList;
    if (!m_Peds.m_count) return;
    m_aPedLinkToDestinations.fill(-1);
    const double rounded = std::ceil(double(int32(pedList.m_count)) / int32(m_DestinationPeds.m_count));
    const int32 capacity = std::max(2, int32(int64(rounded)));
    std::array<int32, 30> remaining;
    for (int32 i = 0; i < int32(m_DestinationPeds.m_count); ++i) remaining[i] = capacity;
    // Valid executable inputs provide at least one finite eligible destination.
    int32 selectedPed{}, selectedTarget{};
    for (int32 assigned = 0; assigned < int32(pedList.m_count); ++assigned) {
        float bestDistance = std::bit_cast<float>(0x497423FEu);
        for (int32 ped = 0; ped < int32(pedList.m_count); ++ped) {
            if (m_aPedLinkToDestinations[ped] >= 0) continue;
            for (int32 target = 0; target < int32(m_DestinationPeds.m_count); ++target) {
                if (remaining[target] <= 0) continue;
                const auto& a = pedList.m_peds[ped]->GetPosition();
                const auto& b = m_DestinationPeds.m_peds[target]->GetPosition();
                // 0x69B7E0..0x69B805: la raíz queda en ST0 (24 bits) y se compara contra el mejor float; si gana se guarda como float.
                const float distance = FormationDistance2DCpp(a.x, a.y, b.x, b.y);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    selectedPed = ped;
                    selectedTarget = target;
                }
            }
        }
        m_aPedLinkToDestinations[selectedPed] = selectedTarget;
        --remaining[selectedTarget];
    }
}

// 0x69B860
void CFormation::FindCoverPoints(CVector pos, float radius) {
    m_Destinations.m_Count = 0;
    std::memset(m_Destinations.m_PointHasBeenClaimed, 0, sizeof(m_Destinations.m_PointHasBeenClaimed));
    auto* vehicles = GetVehiclePool();
    for (int32 i = int32(vehicles->GetSize()); i-- > 0;) {
        auto* vehicle = vehicles->GetAt(i);
        if (!vehicle || !vehicle->IsAutomobile()) continue;
        if (!FormationVehicleStationaryCpp(&vehicle->m_vecMoveSpeed.x)) continue;
        const auto& box = vehicle->GetColModel()->m_boundBox;
        if (!(box.m_vecMax.z < 1.5f)) continue;
        CPointList points;
        points.m_Count = 0;
        std::memset(points.m_PointHasBeenClaimed, 0, sizeof(points.m_PointHasBeenClaimed));
        FindCoverPointsBehindBox(&points, pos, vehicle->m_matrix, vehicle->GetColModel()->m_boundSphere.m_vecCenter, box.m_vecMin, box.m_vecMax, radius);
        for (int32 point = 0; point < int32(points.m_Count); ++point) {
            if (m_Destinations.m_Count < m_Destinations.m_Points.size())
                m_Destinations.m_Points[m_Destinations.m_Count++] = points.m_Points[point];
        }
    }
    auto* objects = GetObjectPool();
    for (int32 i = int32(objects->GetSize()); i-- > 0;) {
        auto* object = objects->GetAt(i);
        if (!object || !(object->m_matrix->GetUp().z > 0.95f) || !object->CanBeUsedToTakeCoverBehind()) continue;
        const auto& origin = object->GetPosition();
        // 0x69BA4F..0x69BAA4: distancia 2D en x87 (24 bits) comparada con radius; dx, dy, dz se guardan como float, se normalizan con 0x59C910
        // y se suman a origin (dirección + origen) en x87.
        if (!(FormationDistance2DCpp(origin.x, origin.y, pos.x, pos.y) < radius)) continue;
        CVector direction{RpNanSubCpp(origin.x, pos.x), RpNanSubCpp(origin.y, pos.y), RpNanSubCpp(origin.z, pos.z)};
        (void)PathFindNormaliseStatusCpp(&direction.x);
        const CVector point{RpNanAddCpp(direction.x, origin.x), RpNanAddCpp(direction.y, origin.y), RpNanAddCpp(direction.z, origin.z)};
        if (m_Destinations.m_Count < m_Destinations.m_Points.size())
            m_Destinations.m_Points[m_Destinations.m_Count++] = point;
    }
}

void CFormation::InjectHooks() {
    RH_ScopedClass(CFormation);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(ReturnTargetPedForPed, 0x699F50);
    RH_ScopedGlobalInstall(ReturnDestinationForPed, 0x699FA0);
    RH_ScopedGlobalInstall(FindCoverPointsBehindBox, 0x699FF0);
    RH_ScopedGlobalInstall(GenerateGatherDestinations, 0x69A620);
    RH_ScopedGlobalInstall(GenerateGatherDestinations_AroundCar, 0x69A770);
    RH_ScopedGlobalInstall(FindNearestFreeDestination, 0x69B1B0);
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240);
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0);
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700);
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860);
}

// 0x69B1B0: first free destination with the smallest distance (x87 rounding and store points in FormationNearestFreeCpp).
int32 CFormation::FindNearestFreeDestination(CVector origin, float* total) {
    return FormationNearestFreeCpp(
        &m_Destinations.m_Points[0].x,
        reinterpret_cast<const unsigned char*>(m_Destinations.m_PointHasBeenClaimed),
        int32(m_Destinations.m_Count),
        &origin.x,
        total
    );
}
