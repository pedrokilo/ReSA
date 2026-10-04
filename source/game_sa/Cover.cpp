/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCover
 * Reconstrucción: COMPLETA — 16/16 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 16/16 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Cover.h"

void CCover::InjectHooks() {
    RH_ScopedClass(CCover);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x698710);
    RH_ScopedInstall(RemoveCoverPointIfEntityLost, 0x698DB0);
    RH_ScopedInstall(RemoveCoverPointsForThisEntity, 0x698740);
    RH_ScopedInstall(ShouldThisBuildingHaveItsCoverPointsCreated, 0x699230);
    RH_ScopedInstall(Update, 0x6997E0);
    RH_ScopedInstall(AddCoverPoint, 0x698F30);
    RH_ScopedInstall(CalculateHorizontalSize, 0x6987F0);
    RH_ScopedInstall(DoLineCheckWithinObject, 0x698990);
    RH_ScopedInstall(DoesCoverPointStillProvideCover, 0x698DD0);
    RH_ScopedInstall(Find2HighestPoints, 0x6988E0);
    RH_ScopedInstall(FindAndReserveCoverPoint, 0x6992B0);
    RH_ScopedInstall(FindCoordinatesCoverPoint, 0x699570);
    RH_ScopedInstall(FindCoverPointsForThisBuilding, 0x699120);
    RH_ScopedNamedInstall(FindDirFromVector_Reversed, "FindDirFromVector", 0x698D40);
    RH_ScopedNamedInstall(FindVectorFromDir_Reversed, "FindVectorFromDir", 0x698D60);
    RH_ScopedInstall(FindVectorFromFirstToMissingVertex, 0x698790);
}

namespace {
// Como el original: el EXE accede directamente a los campos de cada CCoverPoint (0x1C bytes), sin pasar por
// EntityRef (no limpia ni registra referencias salvo donde se indica).
struct CoverPointRaw {
    int8     Type;      // 0x00
    int8     Usage;     // 0x01
    uint8    Dir;       // 0x02
    uint8    Pad;       // 0x03 (nunca se escribe)
    CVector  Point;     // 0x04
    CEntity* Entity;    // 0x10
    CPed*    Peds[2];   // 0x14
};
VALIDATE_SIZE(CoverPointRaw, 0x1C);

constexpr int32 NUM_COVER_POINTS = 100;

CoverPointRaw* RawPoints() { return reinterpret_cast<CoverPointRaw*>(CCover::m_Points.data()); }
CoverPointRaw& Raw(CCoverPoint& cpt) { return reinterpret_cast<CoverPointRaw&>(cpt); }
const CoverPointRaw& Raw(const CCoverPoint& cpt) { return reinterpret_cast<const CoverPointRaw&>(cpt); }
CCoverPoint* AsCoverPoint(CoverPointRaw& raw) { return reinterpret_cast<CCoverPoint*>(&raw); }
int32& NumPoints() { return reinterpret_cast<int32&>(CCover::m_NumPoints); } // Como el original: contador con signo

template<typename T>
T& At(const void* base, size_t offset) { return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset); }

CColModel* ColModelOf(const CEntity* entity) {
    return CModelInfo::ms_modelInfoPtrs[static_cast<int16>(entity->GetModelIndex())]->m_pColModel; // Como el original: movsx
}

// 0x59C730 (CrossProduct): a × b con los productos del EXE
CVector CoverCross(const CVector& a, const CVector& b) {
    return { b.z * a.y - a.z * b.y, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y };
}

// 0x59C790 (Multiply3x3): orden de sumas del EXE
CVector CoverMultiply3x3(const CMatrix& m, const CVector& v) {
    return {
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y,
    };
}

// 0x59C890 (CMatrix * CVector): orden de sumas del EXE
CVector CoverTransformPoint(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// 0x59C910 (CVector::Normalise): si la longitud al cuadrado es <= 0 solo pone x = 1 (NaN normaliza)
void CoverNormalise(CVector& v) {
    const float lenSq = (v.x * v.x + v.y * v.y) + v.z * v.z;
    if (lenSq <= 0.0f) {
        v.x = 1.0f;
        return;
    }
    const float inv = 1.0f / std::sqrt(lenSq);
    v.x *= inv;
    v.y *= inv;
    v.z *= inv;
}

// Como el original (0x69936D): producto escalar con el vector de la dirección calculado en línea. El seno
// (negado) entra al producto con la precisión interna del x87, solo el coseno se guarda a float.
float CoverDirDot(float deltaX, float deltaY, float deltaZ, int32 dirIndex) {
    static const float step = std::bit_cast<float>(0x3CC90FDBu); // 0x859BBC (2π/256)
    static const float zero = 0.0f;
    float cosValue, result;
    __asm {
        fld deltaX
        fld deltaY
        fld deltaZ
        fild dirIndex
        fmul step
        fld st(0)
        fsin
        fchs
        fxch st(1)
        fcos
        fstp cosValue
        fxch st(2)
        fmul cosValue
        fxch st(2)
        fmul st, st(3)
        faddp st(2), st
        fmul zero
        faddp st(1), st
        fstp result
        fstp st(0)
    }
    return result;
}
} // namespace

// 0x698710
void CCover::Init() {
    ZoneScoped;

    m_NumPoints = 0;
    m_ListOfProcessedBuildings.Flush();
    for (auto i = 0; i < NUM_COVER_POINTS; ++i) {
        RawPoints()[i].Type = 0; // Como el original: solo se borra el tipo
    }
}

// NOTSA: cleanup while the referenced buildings are still alive.
void CCover::ClearProcessedBuildings() {
    // Nodes can own registered entity references when FIX_BUGS is enabled.
    // Unregister them before freeing the storage containing their pointer.
    for (auto* node = m_ListOfProcessedBuildings.GetNode(); node; node = node->Next) {
        CEntity::SafeCleanUpRef(node->Item);
    }
    m_ListOfProcessedBuildings.Flush();
}

// 0x698DB0
void CCover::RemoveCoverPointIfEntityLost(CCoverPoint& cpt) {
    // Como el original: tipos 1..3 (con signo) sin entidad; solo se borra el tipo y se descuenta.
    auto& raw = Raw(cpt);
    if (raw.Type > 0 && raw.Type <= 3 && !raw.Entity) {
        raw.Type = 0;
        --NumPoints();
    }
}

// 0x698740
void CCover::RemoveCoverPointsForThisEntity(CEntity* entity) {
    for (auto i = 0; i < NUM_COVER_POINTS; ++i) {
        auto& raw = RawPoints()[i];
        if (!raw.Type || raw.Entity != entity) {
            continue;
        }
        if (raw.Entity) {
            raw.Entity->CleanUpOldReference(&raw.Entity);
        }
        --NumPoints();
        raw.Type   = 0;
        raw.Entity = nullptr;
    }
}

// 0x699230
bool CCover::ShouldThisBuildingHaveItsCoverPointsCreated(CBuilding* building) {
    // Como el original: distancia real (raíz de z² + y² + x²) frente a radio + 30, NaN no.
    const CVector playerPos = FindPlayerCoors();
    CVector centre;
    building->GetBoundCentre(centre);
    const float deltaX = centre.x - playerPos.x;
    const float deltaY = centre.y - playerPos.y;
    const float deltaZ = centre.z - playerPos.z;
    const float dist   = std::sqrt((deltaZ * deltaZ + deltaY * deltaY) + deltaX * deltaX);
    const float radius = ColModelOf(building)->m_boundSphere.m_fRadius + 30.0f;
    return radius > dist;
}

// 0x6997E0
void CCover::Update() {
    ZoneScoped;

    if (*reinterpret_cast<const uint8*>(&CReplay::Mode) == MODE_PLAYBACK) { // Como el original: compara el byte
        return;
    }

    // 0x699800: puntos cuya entidad ya no sirve (solo se borra el tipo; la referencia no se toca)
    for (auto i = 0; i < NUM_COVER_POINTS; ++i) {
        auto& raw = RawPoints()[i];
        bool remove = false;
        switch (raw.Type) {
        case 1: // OBJECT: up.z < 0.9 (con GetMatrix en línea)
            remove = !raw.Entity || raw.Entity->GetMatrix().GetUp().z < 0.9f;
            break;
        case 2: { // VEHICLE: |velocidad| > 0.01 (raíz de (x² + y²) + z²)
            if (!raw.Entity) {
                remove = true;
                break;
            }
            const auto& speed = At<CVector>(raw.Entity, 0x44); // CPhysical::m_vecMoveSpeed
            remove = std::sqrt((speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z) > 0.01f;
            break;
        }
        case 3:
            remove = !raw.Entity;
            break;
        }
        if (remove) {
            --NumPoints();
            raw.Type = 0;
        }
    }

    const auto frame = CTimer::GetFrameCounter();
    const auto DistToPlayer2D = [](const CVector& pos) {
        const CVector playerPos = FindPlayerCoors();
        const float deltaX = playerPos.x - pos.x;
        const float deltaY = playerPos.y - pos.y;
        return std::sqrt(deltaY * deltaY + deltaX * deltaX);
    };

    switch (frame % 32) {
    case 26: { // 0x6998B5: vehículos (de la última ranura a la primera)
        auto* const pool = GetVehiclePool();
        for (auto i = (int32)pool->GetSize(); i;) {
            --i;
            auto* const veh = pool->GetAt(i);
            if (!veh) {
                continue;
            }
            if (At<int32>(veh, 0x590) != 0) { // m_nVehicleType != VEHICLE_TYPE_AUTOMOBILE
                continue;
            }
            const auto& speed = veh->GetMoveSpeed();
            if (!(std::sqrt((speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z) < 0.005f)) {
                continue;
            }
            if (!(At<uint8>(veh, 0x42E) & 4)) { // vehicleFlags.bDoesProvideCover
                continue;
            }
            if (!(DistToPlayer2D(veh->GetPosition()) < 25.0f)) {
                continue;
            }
            AddCoverPoint(CCoverPoint::eType::VEHICLE, veh, nullptr, CCoverPoint::eUsage::LOWCOVER, CCoverPoint::Dir{ uint8(0xFF) });
        }
        return;
    }
    case 28: { // 0x6999B5: objetos (de la última ranura a la primera)
        auto* const pool = GetObjectPool();
        for (auto i = (int32)pool->GetSize(); i;) {
            --i;
            auto* const obj = pool->GetAt(i);
            if (!obj) {
                continue;
            }
            if (!(obj->m_matrix->GetUp().z > 0.95f)) { // Como el original: matriz sin comprobar, NaN se descarta
                continue;
            }
            if (!obj->CanBeUsedToTakeCoverBehind()) {
                continue;
            }
            if (!(DistToPlayer2D(obj->GetPosition()) < 25.0f)) {
                continue;
            }
            AddCoverPoint(CCoverPoint::eType::OBJECT, obj, nullptr, CCoverPoint::eUsage::LOWCOVER, CCoverPoint::Dir{ uint8(0xFF) });
        }
        return;
    }
    }

    /* Process buildings too */
    if (frame % 8 != 5) { // 0x699A71
        return;
    }

    // BUG:
    // This implementation may've accessed objects after they've been destroyed (use-after-free
    // I've modified it so that it uses references that get null'd automatically
    for (CPtrNodeDoubleLink<CBuilding*> *it = m_ListOfProcessedBuildings.GetNode(), *next{}; it; it = next) {
        next            = it->Next;
        auto* const obj = it->Item;

        if (!notsa::IsFixBugs() || obj) { // If fixbugs the reference may've got cleared
            if (ShouldThisBuildingHaveItsCoverPointsCreated(obj)) {
                continue;
            }
            RemoveCoverPointsForThisEntity(obj);
            if (notsa::IsFixBugs()) { // FIXBUGS: Use-after-free
                CEntity::SafeCleanUpRef(reinterpret_cast<CEntity*&>(it->Item));
            }
        }
        m_ListOfProcessedBuildings.DeleteNode(it);
    }

    if (FindPlayerVehicle()) { // 0x699AE1
        return;
    }

    // Como el original: rectángulo de ±30 alrededor del jugador (cuatro llamadas), sectores con floor del CRT
    // sobre x * 0.02 + 60, límites [0, 119], y después se avanza el código de escaneo.
    const float minX = FindPlayerCoors().x - 30.0f;
    const float minY = FindPlayerCoors().y - 30.0f;
    const float maxX = FindPlayerCoors().x + 30.0f;
    const float maxY = FindPlayerCoors().y + 30.0f;
    const auto SectorOf = [](float coord) {
        const float scaled = coord * 0.02f + 60.0f;
        return (int32)(int64)std::floor((double)scaled); // Como el original: _ftol2 (64 bits, se queda la parte baja)
    };
    const auto startX = std::max(SectorOf(minX), 0);
    const auto startY = std::max(SectorOf(minY), 0);
    const auto endX   = std::min(SectorOf(maxX), 119);
    const auto endY   = std::min(SectorOf(maxY), 119);
    CWorld::AdvanceCurrentScanCode();

    for (auto y = startY; y <= endY; ++y) {
        for (auto x = startX; x <= endX; ++x) {
            for (auto* node = CWorld::GetSector(x, y).Buildings.GetNode(); node; node = node->Next) {
                auto* const obj = node->Item;
                if (obj->IsScanCodeCurrent()) {
                    continue;
                }
                obj->SetCurrentScanCode();
                if (!ShouldThisBuildingHaveItsCoverPointsCreated(obj)) {
                    continue;
                }
                if (m_ListOfProcessedBuildings.IsMemberOfList(obj)) {
                    continue;
                }
                FindCoverPointsForThisBuilding(obj);
                auto* const link = m_ListOfProcessedBuildings.AddItem(obj);
                if (notsa::IsFixBugs()) { // FIXBUGS: Use-after-free
                    CEntity::SafeRegisterRef(reinterpret_cast<CEntity*&>(link->Item));
                }
                return; // Como el original: como mucho un edificio nuevo por llamada
            }
        }
    }
}

// notsa - allocate new point, increments `NumPoints`
CCoverPoint* CCover::GetFree() {
    for (auto& cpt : m_Points) {
        if (cpt.GetType() == CCoverPoint::eType::NONE) {
            m_NumPoints++;
            return &cpt;
        }
    }
    return nullptr;
}

// 0x698F30
// NOTE: Original function didn't return the (possibly) created point, we do
CCoverPoint* CCover::AddCoverPoint(CCoverPoint::eType type, CEntity* coverEntity, const CVector* pos, CCoverPoint::eUsage usage, CCoverPoint::Dir dir) {
    if (NumPoints() >= NUM_COVER_POINTS) { // Como el original: comparación con signo
        return nullptr;
    }

    const auto dirByte = std::bit_cast<uint8>(dir);
    auto* const points = RawPoints();
    CVector atPos{}; // El EXE lo deja sin inicializar si no hay ni posición ni entidad
    for (auto i = 0; i < NUM_COVER_POINTS; ++i) {
        auto& raw = points[i];

        // 0x698F5D
        if (raw.Type == (int8)type) {
            switch (raw.Type) {
            case 1:
            case 2:
                if (coverEntity == raw.Entity) {
                    return AsCoverPoint(raw);
                }
                break;
            case 3:
                if (pos->x == raw.Point.x && pos->y == raw.Point.y && pos->z == raw.Point.z) {
                    return AsCoverPoint(raw);
                }
                break;
            }
        }

        // 0x698F9D - BUG: The original reads `m_PointOnMap` for every non-free point (it's stale for objects/vehicles)
        if (notsa::IsFixBugs() ? raw.Type != 3 : raw.Type == 0) {
            continue;
        }

        // Como el original: primero la posición dada y, si no hay, la de la entidad
        if (pos) {
            atPos = *pos;
        } else if (coverEntity) {
            atPos = coverEntity->GetPosition();
        }

        // 0x698FDD: distancia real (raíz de (z² + y²) + x²) contra 0.8 y 2.0
        const float deltaX = raw.Point.x - atPos.x;
        const float deltaY = raw.Point.y - atPos.y;
        const float deltaZ = raw.Point.z - atPos.z;
        const float dist   = std::sqrt((deltaZ * deltaZ + deltaY * deltaY) + deltaX * deltaX);
        if (dist < 0.8f) {
            return AsCoverPoint(raw);
        }
        if (!(dist < 2.0f)) {
            continue;
        }

        // 0x699051: diferencia de dirección en bytes (45° = 32, 315° = 223)
        const auto diff = uint8(raw.Dir - dirByte);
        if (diff < 0x20 || diff > 0xDF) {
            return AsCoverPoint(raw);
        }
    }

    // 0x699077: primer hueco libre (el EXE no comprueba el límite)
    auto idx = 0;
    while (points[idx].Type) {
        ++idx;
    }
    auto& raw = points[idx];
    raw.Type    = (int8)type;
    raw.Dir     = dirByte;
    raw.Usage   = (int8)usage;
    raw.Peds[0] = nullptr;
    raw.Peds[1] = nullptr;
    ++NumPoints();
    raw.Entity = coverEntity;
    if (coverEntity && (!coverEntity->GetIsTypeBuilding() || coverEntity->GetIplIndex() != 0)) {
        coverEntity->RegisterReference(&raw.Entity);
    }
    if (pos) {
        raw.Point = *pos;
    }
    return AsCoverPoint(raw);
}

// 0x6987F0
float CCover::CalculateHorizontalSize(CColTriangle* triangle, CVector* vertPositions) {
    // Como el original: mínimos/máximos con comparaciones que eligen el nuevo vértice ante NaN, y raíz de dx² + dy².
    const auto& v0 = vertPositions[triangle->vA];
    const auto& v1 = vertPositions[triangle->vB];
    const auto& v2 = vertPositions[triangle->vC];
    float minX = v0.x, maxX = v0.x, minY = v0.y, maxY = v0.y;
    if (!(maxX < v1.x)) {
        minX = v1.x;
    }
    if (!(minX < v2.x)) {
        minX = v2.x;
    }
    if (!(maxX > v1.x)) {
        maxX = v1.x;
    }
    if (!(maxX > v2.x)) {
        maxX = v2.x;
    }
    if (!(maxY < v1.y)) {
        minY = v1.y;
    }
    if (!(minY < v2.y)) {
        minY = v2.y;
    }
    if (!(maxY > v1.y)) {
        maxY = v1.y;
    }
    if (!(maxY > v2.y)) {
        maxY = v2.y;
    }
    const float sizeX = maxX - minX;
    const float sizeY = maxY - minY;
    return std::sqrt(sizeX * sizeX + sizeY * sizeY);
}

// 0x698990
bool CCover::DoLineCheckWithinObject(CColTriangle* triangles, int32 numTriangles, CVector* vertPositions, CVector* normals, CVector start, CVector end) {
    // Como el original: el segmento cruza el plano del triángulo y el punto de corte queda del mismo lado
    // de las tres aristas (productos vectoriales con CrossProduct del EXE y sumas en su orden).
    const auto Dot = [](const CVector& a, const CVector& n) { return (a.x * n.x + a.y * n.y) + a.z * n.z; };
    for (auto i = 0; i < numTriangles; ++i) {
        const auto& tri = triangles[i];
        const auto& n   = normals[i];
        const auto& vA  = vertPositions[tri.vA];

        const float d1 = Dot(start - vA, n);
        const float d2 = Dot(end - vA, n);
        if (!(d2 * d1 < 0.0f)) {
            continue;
        }
        const float abs1 = d1 < 0.0f ? -d1 : d1;
        const float abs2 = d2 < 0.0f ? -d2 : d2;
        const float t    = abs1 / (abs2 + abs1);
        const float s    = 1.0f - t;
        const CVector hit{ end.x * t + start.x * s, end.y * t + start.y * s, end.z * t + start.z * s };

        const auto& vB = vertPositions[tri.vB];
        const auto& vC = vertPositions[tri.vC];
        const float dotA = Dot(CoverCross(hit - vA, vB - vA), n);
        const float dotB = Dot(CoverCross(hit - vB, vC - vB), n);
        if (!(dotB * dotA > 0.0f)) {
            continue;
        }
        const float dotC = Dot(CoverCross(hit - vC, vA - vC), n);
        if (dotC * dotA > 0.0f) {
            return true;
        }
    }
    return false;
}

// 0x698DD0
bool CCover::DoesCoverPointStillProvideCover(CCoverPoint* cpt, CVector pos) {
    if (!cpt) {
        return false;
    }
    RemoveCoverPointIfEntityLost(*cpt);
    const auto& raw = Raw(*cpt);
    if (!raw.Type) {
        return false;
    }
    if (raw.Type != 3) {
        return true;
    }
    // Como el original: (pos - punto) · FindVectorFromDir(dir) sumando z, y, x
    const float deltaX = pos.x - raw.Point.x;
    const float deltaY = pos.y - raw.Point.y;
    const float deltaZ = pos.z - raw.Point.z;
    const CVector dirVec = FindVectorFromDir_Reversed(raw.Dir);
    return (deltaZ * dirVec.z + deltaY * dirVec.y) + deltaX * dirVec.x > 0.0f;
}

// 0x6988E0
void CCover::Find2HighestPoints(CColTriangle* triangle, CVector* vertPositions, int32& outPoint1, int32& outPoint2) {
    // Como el original: se descarta el vértice estrictamente más bajo que los otros dos (NaN no lo es).
    const float zA = vertPositions[triangle->vA].z;
    const float zB = vertPositions[triangle->vB].z;
    const float zC = vertPositions[triangle->vC].z;
    if (zA < zB && zA < zC) {
        outPoint1 = triangle->vB;
        outPoint2 = triangle->vC;
    } else if (zB < zA && zB < zC) {
        outPoint1 = triangle->vA;
        outPoint2 = triangle->vC;
    } else {
        outPoint1 = triangle->vA;
        outPoint2 = triangle->vB;
    }
}

// 0x6992B0
CCoverPoint* CCover::FindAndReserveCoverPoint(CPed* ped, const CVector& targetPos, bool isForAttack) {
    // Como el original: se elige el punto más cercano al peatón en 2D (< 15, |Δz| < 4) y, si es para atacar,
    // solo los que quedan más cerca del objetivo que el peatón + 4.
    auto* const points = RawPoints();
    float bestDist = 99999.9f; // 0x85999C
    auto  bestIdx  = -1;
    CVector pointPos{}; // El EXE conserva el valor anterior para tipos sin posición
    for (auto i = 0; i < NUM_COVER_POINTS; ++i) {
        auto& raw = points[i];
        if (raw.Type > 0 && raw.Type <= 3 && !raw.Entity) { // RemoveCoverPointIfEntityLost en línea
            --NumPoints();
            raw.Type = 0;
        }
        if (!AsCoverPoint(raw)->CanAccommodateAnotherPed()) {
            continue;
        }

        const auto type = raw.Type;
        if (type > 0 && type <= 2) {
            pointPos = raw.Entity->GetPosition();
        } else if (type == 3) {
            pointPos = raw.Point;
        }
        if (type == 3) {
            const float dot = CoverDirDot(targetPos.x - raw.Point.x, targetPos.y - raw.Point.y, targetPos.z - raw.Point.z, raw.Dir);
            if (!(dot > 0.0f)) {
                continue;
            }
        }

        const auto& pedPos = ped->GetPosition();
        float heightDiff = pedPos.z - pointPos.z;
        if (heightDiff < 0.0f) {
            heightDiff = -(pedPos.z - pointPos.z);
        }
        if (!(heightDiff < 4.0f)) {
            continue;
        }

        const auto Dist2D = [](const CVector& a, const CVector& b) {
            const float deltaX = a.x - b.x;
            const float deltaY = a.y - b.y;
            return std::sqrt(deltaY * deltaY + deltaX * deltaX);
        };
        const float pedToPoint = Dist2D(pedPos, pointPos);
        if (!(pedToPoint < 15.0f)) {
            continue;
        }
        const float pedToTarget   = Dist2D(pedPos, targetPos);
        const float pointToTarget = Dist2D(pointPos, targetPos);
        if (!(pedToPoint < bestDist)) {
            continue;
        }
        if (isForAttack && !(pointToTarget < pedToTarget + 4.0f)) {
            continue;
        }
        bestIdx  = i;
        bestDist = pedToPoint;
    }
    if (bestIdx < 0) {
        return nullptr;
    }
    auto* const cpt = AsCoverPoint(points[bestIdx]);
    cpt->ReserveCoverPointForPed(ped);
    return cpt;
}

// 0x699570
bool CCover::FindCoordinatesCoverPoint(const CCoverPoint& cpt, CPed* ped, const CVector& position, CVector& outCoordinates) {
    const auto& raw = Raw(cpt);
    if (raw.Peds[0] != ped && raw.Peds[1] != ped) {
        return false;
    }
    switch (raw.Type) {
    case 3: // 0x6995AF
        outCoordinates = raw.Point;
        return true;
    case 2: { // 0x6995D4
        auto* const veh = raw.Entity;
        if (!veh) {
            return false;
        }
        CPointList points;
        auto* const col = ColModelOf(veh);
        CFormation::FindCoverPointsBehindBox(&points, position, veh->m_matrix, col->m_boundSphere.m_vecCenter, col->m_boundBox.m_vecMin, col->m_boundBox.m_vecMax, 1000000.0f);
        if (points.m_Count != 2) {
            return false;
        }
        // Como el original: el más cercano al peatón en 2D (raíces); empate o NaN -> el segundo
        const auto& pedPos = ped->GetPosition();
        const float d0x = points.m_Points[0].x - pedPos.x, d0y = points.m_Points[0].y - pedPos.y;
        const float d0Sq = d0y * d0y + d0x * d0x;
        const float d1x = points.m_Points[1].x - pedPos.x, d1y = points.m_Points[1].y - pedPos.y;
        const float d1 = std::sqrt(d1y * d1y + d1x * d1x);
        outCoordinates = d1 > std::sqrt(d0Sq) ? points.m_Points[0] : points.m_Points[1];
        return true;
    }
    case 1: { // 0x6996F2
        auto* const obj = raw.Entity;
        if (!obj) {
            return false;
        }
        const auto& bbMax = ColModelOf(obj)->m_boundBox.m_vecMax;
        const float radius = (bbMax.x > bbMax.y ? bbMax.x : bbMax.y) + 0.4f;
        const auto& objPos = obj->GetPosition();
        CVector dir{ position.x - objPos.x, position.y - objPos.y, 0.0f };
        CoverNormalise(dir);
        const float offX = dir.x * radius, offY = dir.y * radius, offZ = dir.z * radius;
        outCoordinates = CVector{ objPos.x - offX, objPos.y - offY, objPos.z - offZ };
        return true;
    }
    }
    return false;
}

// 0x699120
void CCover::FindCoverPointsForThisBuilding(CBuilding* building) {
    auto* const mi = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(building->GetModelIndex())];
    const int32 count = mi->m_n2dfxCount;
    for (auto i = 0; i < count; ++i) {
        auto* const fx = mi->Get2dEffect(i);
        if (At<uint8>(fx, 0xC) != EFFECT_COVER_POINT) {
            continue;
        }
        // Como el original: dirección (x, y, 0) girada con Multiply3x3 y convertida con fpatan + _ftol2,
        // y posición transformada con CMatrix * CVector, ambas con el orden de sumas del EXE.
        const CVector dirOfCover{ At<float>(fx, 0x10), At<float>(fx, 0x14), 0.0f };
        const CVector dir     = CoverMultiply3x3(building->GetMatrix(), dirOfCover);
        const auto    dirByte = uint8(FindDirFromVector_Reversed(dir));
        const CVector pos     = CoverTransformPoint(building->GetMatrix(), At<CVector>(fx, 0x0));
        AddCoverPoint(CCoverPoint::eType::POINTONMAP, building, &pos, (CCoverPoint::eUsage)At<uint8>(fx, 0x18), CCoverPoint::Dir{ dirByte });
    }
}

// 0x698D40 - unused
CCoverPoint::Dir CCover::FindDirFromVector(CVector dir) {
    return CCoverPoint::Dir{ uint8(FindDirFromVector_Reversed(dir)) };
}

// Como el original: devuelve el entero completo de _ftol2 (truncado a 64 bits) en EAX.
int32 CCover::FindDirFromVector_Reversed(CVector dir) {
    const float scale = 0x1.45f306p+5f;
    const float x = dir.x, y = dir.y;
    uint16 oldControl, truncControl;
    int64 result;
    __asm {
        fld x
        fchs
        fld y
        fpatan
        fmul scale
        fnstcw oldControl
        mov ax, oldControl
        or ax, 0C00h
        mov truncControl, ax
        fldcw truncControl
        fistp result
        fldcw oldControl
    }
    return int32(result);
}

// 0x698D60
CVector CCover::FindVectorFromDir(CCoverPoint::Dir direction) {
    return FindVectorFromDir_Reversed(std::bit_cast<uint8>(direction));
}

CVector CCover::FindVectorFromDir_Reversed(uint8 direction) {
    const int32 value = direction;
    const float scale = 0x1.921fb6p-6f;
    float x, y;
    __asm {
        fild value
        fmul scale
        fld st(0)
        fsin
        fchs
        fstp x
        fcos
        fstp y
    }
    return {x, y, 0.f};
}

// unused
// 0x698790
CVector CCover::FindVectorFromFirstToMissingVertex(CColTriangle* triangle, int32* a3, CVector* vertPositions) {
    // Como el original: los índices de referencia son int32 completos y el vértice que falta es el primero
    // de A, B, C que no coincide con ninguno de los dos.
    const int32 first = a3[0];
    int32 missing;
    if (triangle->vA != first && triangle->vA != a3[1]) {
        missing = triangle->vA;
    } else if (triangle->vB != first && triangle->vB != a3[1]) {
        missing = triangle->vB;
    } else {
        missing = triangle->vC;
    }
    const auto& to   = vertPositions[missing];
    const auto& from = vertPositions[first];
    return { to.x - from.x, to.y - from.y, to.z - from.z };
}
