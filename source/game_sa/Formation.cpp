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
 * Funciones bloqueadas: 8.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   DistributeDestinations | 0x0069B240 | S
 *   DistributeDestinations_CoverPoints | 0x0069B5B0 | S
 *   DistributeDestinations_PedsToAttack | 0x0069B700 | S
 *   FindCoverPoints | 0x0069B860 | S
 *   GenerateGatherDestinations | 0x0069A620 | S
 *   GenerateGatherDestinations_AroundCar | 0x0069A770 | S
 *   ReturnDestinationForPed | 0x00699FA0 | S
 *   ReturnTargetPedForPed | 0x00699F50 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Formation.h"

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

namespace {
// Match the executable's x87 operations and float stores at 0x699FF0.
float CoverSubtract(float a, float b) {
    float result;
    __asm {
        fld a
        fsub b
        fstp result
    }
    return result;
}
float CoverAdd(float a, float b) {
    float result;
    __asm {
        fld a
        fadd b
        fstp result
    }
    return result;
}
CVector CoverCorner(const CMatrix* matrix, float x, float y) {
    constexpr float z=0.f;
    CVector result;
    auto* output=&result;
    __asm {
        mov eax,matrix
        mov ecx,output
        fld dword ptr [eax+20h]
        fmul z
        fld dword ptr [eax+10h]
        fmul y
        faddp st(1),st
        fld dword ptr [eax]
        fmul x
        faddp st(1),st
        fadd dword ptr [eax+30h]
        fstp dword ptr [ecx]
        fld dword ptr [eax+24h]
        fmul z
        fld dword ptr [eax+4]
        fmul x
        faddp st(1),st
        fld dword ptr [eax+14h]
        fmul y
        faddp st(1),st
        fadd dword ptr [eax+34h]
        fstp dword ptr [ecx+4]
        fld dword ptr [eax+28h]
        fmul z
        fld dword ptr [eax+8]
        fmul x
        faddp st(1),st
        fld dword ptr [eax+18h]
        fmul y
        faddp st(1),st
        fadd dword ptr [eax+38h]
        fstp dword ptr [ecx+8]
    }
    return result;
}
float CoverDistance2D(const CVector& a, const CVector& b) {
    const float firstX=a.x, firstY=a.y, secondX=b.x, secondY=b.y;
    float result;
    __asm {
        fld firstX
        fsub secondX
        fld firstY
        fsub secondY
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fsqrt
        fstp st(2)
        fstp st(0)
        fstp result
    }
    return result;
}
void CoverNormalise(CVector& value) {
    auto* p=&value;
    __asm {
        mov ecx, p
        fld dword ptr [ecx+8]
        fld dword ptr [ecx+4]
        fld dword ptr [ecx]
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fld st(3)
        fmul st, st(4)
        faddp st(1), st
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fcom dword ptr ds:[0x858B50]
        fnstsw ax
        test ah, 41h
        jp positive
        fstp st(0)
        mov dword ptr [ecx], 3F800000h
        jmp finished
    positive:
        fsqrt
        fdivr dword ptr ds:[0x858624]
        fld st(0)
        fmul dword ptr [ecx]
        fstp dword ptr [ecx]
        fld st(0)
        fmul dword ptr [ecx+4]
        fstp dword ptr [ecx+4]
        fmul dword ptr [ecx+8]
        fstp dword ptr [ecx+8]
    finished:
    }
}
float CoverOffset(float position, float direction) {
    constexpr float scale=.75f;
    float result;
    __asm {
        fld direction
        fmul scale
        fadd position
        fstp result
    }
    return result;
}
}

// 0x699FF0
void CFormation::FindCoverPointsBehindBox(
    CPointList* outPoints, CVector target, CMatrix* mat,
    const CVector& center, const CVector& bbMin, const CVector& bbMax, float cutoffDist
) {
    const float targetZ=target.z, matrixZ=mat->GetPosition().z;
    bool withinHeight{};
    __asm {
        fld targetZ
        fsub matrixZ
        fabs
        fcomp dword ptr ds:[0x858F48]
        fnstsw ax
        test ah,5
        jp outsideHeight
        mov withinHeight,1
    outsideHeight:
    }
    if (!withinHeight) return;
    const CVector corners[4]{
        CoverCorner(mat,bbMin.x,bbMin.y),
        CoverCorner(mat,bbMin.x,bbMax.y),
        CoverCorner(mat,bbMax.x,bbMax.y),
        CoverCorner(mat,bbMax.x,bbMin.y)
    };
    float distances[4];
    for (int32 i=0;i<4;++i) distances[i]=CoverDistance2D(corners[i],target);
    int32 closest=0;
    for (int32 i=1;i<4;++i) if (distances[i]<distances[closest]) closest=i;
    const auto& opposite=corners[(closest-2)&3];
    for (int32 i=0;i<4;++i) {
        if (i==closest || ((i-2)&3)==closest || static_cast<int32>(outPoints->m_Count)>=24 || !(distances[i]<cutoffDist)) continue;
        const auto& corner=corners[i];
        CVector direction{CoverSubtract(opposite.x,corner.x),CoverSubtract(opposite.y,corner.y),0.f};
        CoverNormalise(direction);
        const CVector point{CoverAdd(corner.x,direction.x),CoverAdd(corner.y,direction.y),CoverAdd(corner.z,direction.z)};
        CVector away{CoverSubtract(point.x,target.x),CoverSubtract(point.y,target.y),0.f};
        CoverNormalise(away);
        outPoints->m_Points[outPoints->m_Count++]=CVector{CoverOffset(point.x,away.x),CoverOffset(point.y,away.y),CoverAdd(point.z,away.z*.75f)};
    }
}

namespace {
// Keep the x87 angle and cosine product extended until the executable's spills.
CVector GatherPoint(int32 index, int32 count, float heading, float radius, const CVector& pos) {
    constexpr float halfPi = std::bit_cast<float>(0x3FC90FDBu), pi = std::bit_cast<float>(0x40490FDBu), twoPi = std::bit_cast<float>(0x40C90FDBu);
    float sineProduct, resultX, resultY;
    const float px = pos.x, py = pos.y;
    __asm {
        fld radius
        cmp count, 1
        jg many
        fld heading
        fadd halfPi
        jmp angleReady
    many:
        fild count
        fild index
        fdiv st, st(1)
        fmul twoPi
        fsub heading
        fld pi
        fdiv st, st(2)
        faddp st(1), st
        fstp st(1)
    angleReady:
        fld st(0)
        fsin
        fmul st, st(2)
        fstp sineProduct
        fcos
        fmul st, st(1)
        fadd py
        fstp resultY
        fld sineProduct
        fadd px
        fstp resultX
        fstp st(0)
    }
    return {resultX, resultY, pos.z};
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
        const auto point = GatherPoint(i, int32(pedList.m_count), heading, radius, ped->GetPosition());
        if (m_Destinations.m_Count < m_Destinations.m_Points.size()) {
            m_Destinations.m_Points[m_Destinations.m_Count++] = point;
        }
    }
}

static float GatherCarFraction(int32 index, int32 count) {
    float result;
    __asm {
        fild index
        fidiv count
        fsubr dword ptr ds:[0x858B8C]
        fstp result
    }
    return result;
}

static float GatherCarComponent(float position, float right, float lateralOffset, float forward, float forwardLength, float factor, bool positive, int32 component) {
    float edge, product, result;
    __asm {
        fld right
        fmul lateralOffset
        cmp positive, 0
        je negativeSide
        fadd position
        jmp storeEdge
    negativeSide:
        fsubr position
    storeEdge:
        fstp edge
        fld forward
        fmul forwardLength
        cmp component, 2
        jne multiplyFraction
        fstp product
        fld product
    multiplyFraction:
        fmul factor
        cmp component, 0
        jne addEdge
        fstp product
        fld product
    addEdge:
        fadd edge
        fstp result
    }
    return result;
}

// 0x69A770
void CFormation::GenerateGatherDestinations_AroundCar(CPedList& pedList, CVehicle* veh) {
    const auto& box = veh->GetColModel()->m_boundBox;
    const float offset = float(double(box.m_vecMax.x) + 1.5);
    const float length = float(double(box.m_vecMax.y) - box.m_vecMin.y);
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
            const float factor = GatherCarFraction(i, count);
            const CVector point{
                GatherCarComponent(pos.x, right.x, offset, forward.x, length, factor, side != 0, 0),
                GatherCarComponent(pos.y, right.y, offset, forward.y, length, factor, side != 0, 1),
                GatherCarComponent(pos.z, right.z, offset, forward.z, length, factor, side != 0, 2)
            };
            if (m_Destinations.m_Count < m_Destinations.m_Points.size())
                m_Destinations.m_Points[m_Destinations.m_Count++] = point;
        }
    }
}

struct FormationExtended { uint8 bytes[10]; };

static CVector FormationCentroid(const CVector* points, int32 count, bool destination) {
    float sumY, sumZ, inverseSpill, resultX, resultY, resultZ;
    __asm {
        mov esi, points
        mov ecx, count
        fldz
        fldz
        fldz
        test ecx, ecx
        jle centroidSumsReady
    centroidPoint:
        fxch st(2)
        fadd dword ptr [esi]
        fxch st(2)
        fxch st(1)
        fadd dword ptr [esi+4]
        fxch st(1)
        fadd dword ptr [esi+8]
        add esi, 12
        dec ecx
        jnz centroidPoint
    centroidSumsReady:
        fstp sumZ
        cmp destination, 0
        je centroidPeds
        fstp sumY
        fld dword ptr ds:[00858624h]
        fidiv count
        fxch st(1)
        fmul st, st(1)
        fstp resultX
        fld sumY
        fmul st, st(1)
        fstp resultY
        fld sumZ
        fmul st, st(1)
        fstp resultZ
        fstp st(0)
        jmp centroidDone
    centroidPeds:
        fld dword ptr ds:[00858624h]
        fidiv count
        fst inverseSpill
        fmul st, st(2)
        fstp resultX
        fmul inverseSpill
        fstp resultY
        fstp st(0)
        fld sumZ
        fmul inverseSpill
        fstp resultZ
    centroidDone:
    }
    return {resultX, resultY, resultZ};
}

static void FormationMeanRadius(const CVector* points, int32 count, const CVector& center, FormationExtended* result) {
    const float componentX = center.x, componentY = center.y, componentZ = center.z;
    __asm {
        mov esi, points
        mov ecx, count
        fldz
        test ecx, ecx
        jle divideRadius
    nextPoint:
        fld dword ptr [esi]
        fsub componentX
        fld dword ptr [esi+4]
        fsub componentY
        fld dword ptr [esi+8]
        fsub componentZ
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fld st(3)
        fmul st, st(4)
        faddp st(1), st
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        faddp st(1), st
        add esi, 12
        dec ecx
        jnz nextPoint
    divideRadius:
        fidiv count
        mov edi, result
        fstp tbyte ptr [edi]
    }
}

static void FormationRadiusScale(const FormationExtended& destination, const FormationExtended& ped, FormationExtended* result) {
    const auto* destinationData = destination.bytes;
    const auto* pedData = ped.bytes;
    float destinationRadius;
    __asm {
        mov esi, destinationData
        fld tbyte ptr [esi]
        fstp destinationRadius
    }
    if (destinationRadius < 1.0f) destinationRadius = 1.0f;
    __asm {
        mov esi, pedData
        fld tbyte ptr [esi]
        fld dword ptr ds:[0x858624]
        fcomp st(1)
        fnstsw ax
        test ah, 0x41
        jne radiusReady
        fstp st(0)
        fld dword ptr ds:[0x858624]
    radiusReady:
        fdivr destinationRadius
        mov edi, result
        fstp tbyte ptr [edi]
    }
}

static CVector FormationScalePosition(const CVector& point, const CVector& pedCenter, const CVector& destinationCenter, const FormationExtended& scale) {
    const float pointX = point.x, pointY = point.y, pointZ = point.z;
    const float pedX = pedCenter.x, pedY = pedCenter.y, pedZ = pedCenter.z;
    const float destinationX = destinationCenter.x, destinationY = destinationCenter.y, destinationZ = destinationCenter.z;
    const auto* scaleData = scale.bytes;
    float differenceZ, scaledX, resultX, resultY, resultZ;
    __asm {
        mov esi, scaleData
        fld tbyte ptr [esi]
        fld pointX
        fsub pedX
        fld pointY
        fsub pedY
        fld pointZ
        fsub pedZ
        fstp differenceZ
        fxch st(1)
        fmul st, st(2)
        fstp scaledX
        fmul st, st(1)
        fld differenceZ
        fmul st, st(2)
        fld scaledX
        fadd destinationX
        fstp resultX
        fxch st(1)
        fadd destinationY
        fstp resultY
        fadd destinationZ
        fstp resultZ
        fstp st(0)
    }
    return {resultX, resultY, resultZ};
}

// 0x69B240
void CFormation::DistributeDestinations(CPedList& pedList) {
    m_Peds = pedList;
    const int32 count = int32(m_Peds.m_count);
    if (!count) return;
    const int32 destinations = int32(m_Destinations.m_Count);
    const CVector destinationCenter = FormationCentroid(m_Destinations.m_Points.data(), destinations, true);
    std::array<CVector, 24> positions;
    for (int32 i = 0; i < count; ++i) {
        positions[i] = m_Peds.m_peds[i]->GetPosition();
    }
    const CVector pedCenter = FormationCentroid(positions.data(), count, false);
    FormationExtended destinationRadius, pedRadius, scale;
    FormationMeanRadius(m_Destinations.m_Points.data(), destinations, destinationCenter, &destinationRadius);
    FormationMeanRadius(positions.data(), count, pedCenter, &pedRadius);
    FormationRadiusScale(destinationRadius, pedRadius, &scale);
    for (int32 i = 0; i < count; ++i) {
        positions[i] = FormationScalePosition(positions[i], pedCenter, destinationCenter, scale);
    }
    float best = std::bit_cast<float>(0x497423FEu);
    for (int32 attempt = 0; attempt < count; ++attempt) {
        m_aFinalPedLinkToDestinations.fill(-1);
        std::memset(m_Destinations.m_PointHasBeenClaimed, 0, sizeof(m_Destinations.m_PointHasBeenClaimed));
        float total{};
        for (int32 i = 0; i < count; ++i) {
            // Separate executable helper chooses an unclaimed point and adds its cost.
            const int32 destination = plugin::CallAndReturn<int32, 0x69B1B0, CVector, float*>(positions[i], &total);
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
    const auto distance = [](const CVector& a, const CVector& b) {
        const double x = double(a.x) - b.x, y = double(a.y) - b.y;
        return std::sqrt(y * y + x * x);
    };
    for (int32 point = 0; point < int32(m_Destinations.m_Count); ++point) {
        const auto& destination = m_Destinations.m_Points[point];
        const float targetDistance = float(distance(destination, pos));
        float bestScore = 0.4f;
        int32 bestPed = -1;
        for (int32 ped = 0; ped < int32(m_Peds.m_count); ++ped) {
            if (m_aPedLinkToDestinations[ped] >= 0) continue;
            const auto& position = m_Peds.m_peds[ped]->GetPosition();
            const double pedDistance = distance(position, pos);
            if (!(pedDistance + 1.0 >= targetDistance)) continue;
            const double score = 1.0 - ((distance(position, destination) + targetDistance) - pedDistance) / pedDistance;
            if (score > bestScore) {
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
                const double x = double(a.x) - b.x, y = double(a.y) - b.y;
                const double distance = std::sqrt(x * x + y * y);
                if (distance < bestDistance) {
                    bestDistance = float(distance);
                    selectedPed = ped;
                    selectedTarget = target;
                }
            }
        }
        m_aPedLinkToDestinations[selectedPed] = selectedTarget;
        --remaining[selectedTarget];
    }
}

static bool FormationVehicleIsStationary(const CVector& speed) {
    const float componentX = speed.x, componentY = speed.y, componentZ = speed.z;
    uint8 accepted;
    __asm {
        fld componentX
        fmul st, st
        fld componentY
        fmul st, st
        faddp st(1), st
        fld componentZ
        fmul st, st
        faddp st(1), st
        fsqrt
        fcomp dword ptr ds:[0x858B4C]
        fnstsw ax
        test ah, 5
        setnp accepted
    }
    return accepted != 0;
}

// 0x69B860
void CFormation::FindCoverPoints(CVector pos, float radius) {
    m_Destinations.m_Count = 0;
    std::memset(m_Destinations.m_PointHasBeenClaimed, 0, sizeof(m_Destinations.m_PointHasBeenClaimed));
    auto* vehicles = GetVehiclePool();
    for (int32 i = int32(vehicles->GetSize()); i-- > 0;) {
        auto* vehicle = vehicles->GetAt(i);
        if (!vehicle || !vehicle->IsAutomobile()) continue;
        if (!FormationVehicleIsStationary(vehicle->m_vecMoveSpeed)) continue;
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
        const double x = double(origin.x) - pos.x, y = double(origin.y) - pos.y;
        if (!(std::sqrt(y * y + x * x) < radius)) continue;
        CVector direction{float(x), float(y), float(double(origin.z) - pos.z)};
        direction.Normalise();
        const CVector point{float(double(direction.x) + origin.x),float(double(direction.y) + origin.y),float(double(direction.z) + origin.z)};
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
    RH_ScopedGlobalInstall(DistributeDestinations, 0x69B240);
    RH_ScopedGlobalInstall(DistributeDestinations_CoverPoints, 0x69B5B0);
    RH_ScopedGlobalInstall(DistributeDestinations_PedsToAttack, 0x69B700);
    RH_ScopedGlobalInstall(FindCoverPoints, 0x69B860);
}
