/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWorld
 * Reconstrucción: COMPLETA — 95/95 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 95/95 entradas.
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

#include "World.h"
#include "Ragdoll/IKChainManager.h"
#include "FireManager.h"
#include "CarCtrl.h"
#include "TagManager.h"
#include "Glass.h"
#include "Shadows.h"
#include "CustomBuildingDNPipeline.h"
#include "VehicleRecording.h"
#include "Garages.h"

#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"

// CWorld::SnookerTableMax = CVector{ 2495.8525f, -1671.41f, 12.9f };
// CWorld::SnookerTableMin = CVector{ 497.7925f, -1670.4f, 13.19f };

auto& FilledColPointIndex = StaticRef<uint32>(0xB7CD7C);
auto& gaTempSphereColPoints = StaticRef<std::array<CColPoint, 32>>(0xB9B250);

void CWorld::InjectHooks() {
    RH_ScopedClass(CWorld);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5631E0);
    RH_ScopedInstall(ShutDown, 0x564050);
    RH_ScopedInstall(Add, 0x563220);
    RH_ScopedInstall(Remove, 0x563280);
    RH_ScopedInstall(StopAllLawEnforcersInTheirTracks, 0x566C10);
    RH_ScopedInstall(CallOffChaseForArea, 0x566A60);
    RH_ScopedInstall(ExtinguishAllCarFiresInArea, 0x566950);
    RH_ScopedInstall(ClearPedsFromArea, 0x5667F0);
    RH_ScopedOverloadedInstall(TestForUnusedModels, "", 0x566510, void(*)());
    RH_ScopedOverloadedInstall(TestForBuildingsOnTopOfEachOther, "Void", 0x5664A0, void(*)());
    RH_ScopedInstall(PrintCarChanges, 0x566420);
    RH_ScopedInstall(TestSphereAgainstSectorList<CPtrListDoubleLink<CPhysical*>>, 0x566140);
    RH_ScopedInstall(UseDetonator, 0x5660B0);
    RH_ScopedInstall(RemoveFallenCars, 0x565E80);
    RH_ScopedInstall(RemoveFallenPeds, 0x565CB0);
    RH_ScopedInstall(ClearCarsFromArea, 0x566610);
    RH_ScopedInstall(TriggerExplosionSectorList<CPtrListDoubleLink<CPhysical*>>, 0x567750);
    RH_ScopedInstall(Process, 0x5684A0);
    RH_ScopedInstall(ProcessLineOfSight, 0x56BA00);
    RH_ScopedInstall(ProcessLineOfSightSector, 0x56B5E0);
    RH_ScopedInstall(ProcessLineOfSightSectorList<CPtrListDoubleLink<CPhysical*>>, 0x566EE0);
    RH_ScopedInstall(ProcessVerticalLine, 0x5674E0);
    RH_ScopedInstall(ProcessVerticalLine_FillGlobeColPoints, 0x567620);
    RH_ScopedInstall(ProcessVerticalLineSector, 0x564500);
    RH_ScopedInstall(ProcessVerticalLineSector_FillGlobeColPoints, 0x564420);
    RH_ScopedInstall(ProcessVerticalLineSectorList<CPtrListDoubleLink<CPhysical*>>, 0x5632B0);
    RH_ScopedInstall(ProcessVerticalLineSectorList_FillGlobeColPoints<CPtrListDoubleLink<CPhysical*>>, 0x5636A0);
    RH_ScopedInstall(ProcessForAnimViewer, 0x5633D0);
    RH_ScopedInstall(ProcessPedsAfterPreRender, 0x563430);
    RH_ScopedInstall(ProcessAttachedEntities, 0x5647F0);
    RH_ScopedInstall(TriggerExplosion, 0x56B790);
    RH_ScopedInstall(ClearExcitingStuffFromArea, 0x56A0D0);
    RH_ScopedInstall(TestSphereAgainstWorld, 0x569E20);
    RH_ScopedInstall(RepositionOneObject, 0x569850);
    RH_ScopedInstall(FindUnsuspectingTargetPed, 0x566DA0);
    RH_ScopedInstall(FindUnsuspectingTargetCar, 0x566C90);
    RH_ScopedInstall(FindLowestZForCoord, 0x5697F0);
    RH_ScopedInstall(FindRoofZFor3DCoord, 0x569750);
    RH_ScopedInstall(FindGroundZFor3DCoord, 0x5696C0);
    RH_ScopedInstall(FindGroundZForCoord, 0x569660);
    RH_ScopedInstall(FindNearestObjectOfType, 0x5693F0);
    RH_ScopedInstall(FindMissionEntitiesIntersectingCube, 0x569240);
    RH_ScopedInstall(FindObjectsIntersectingAngledCollisionBox, 0x568FF0);
    RH_ScopedInstall(FindObjectsIntersectingCube, 0x568DD0);
    RH_ScopedInstall(FindObjectsKindaColliding, 0x568B80);
    RH_ScopedInstall(FindObjectsOfTypeInRangeSectorList<CPtrListDoubleLink<CPhysical*>>, 0x5635C0);
    RH_ScopedInstall(FindObjectsInRangeSectorList<CPtrListDoubleLink<CPhysical*>>, 0x563500);
    RH_ScopedInstall(FindPlayerSlotWithRemoteVehiclePointer, 0x563FD0);
    RH_ScopedInstall(FindPlayerSlotWithVehiclePointer, 0x564000);
    RH_ScopedInstall(FindNearestObjectOfTypeSectorList<CPtrListDoubleLink<CPhysical*>>, 0x565450);
    RH_ScopedInstall(FindMissionEntitiesIntersectingCubeSectorList<CPtrListDoubleLink<CPhysical*>>, 0x565300);
    RH_ScopedInstall(FindObjectsIntersectingAngledCollisionBoxSectorList<CPtrListDoubleLink<CPhysical*>>, 0x565200);
    RH_ScopedInstall(FindObjectsIntersectingCubeSectorList<CPtrListDoubleLink<CPhysical*>>, 0x5650E0);
    RH_ScopedInstall(FindObjectsKindaCollidingSectorList<CPtrListDoubleLink<CPhysical*>>, 0x565000);
    RH_ScopedInstall(FindObjectsOfTypeInRange, 0x564C70);
    RH_ScopedInstall(FindObjectsInRange, 0x564A20);
    RH_ScopedInstall(FindPlayerSlotWithPedPointer, 0x563FA0);
    RH_ScopedInstall(FindLodOfTypeInRange, 0x564ED0);
    RH_ScopedInstall(SprayPaintWorld, 0x565B70);
    RH_ScopedInstall(GetIsLineOfSightClear, 0x56A490);
    RH_ScopedInstall(GetIsLineOfSightSectorClear, 0x568AD0);
    RH_ScopedInstall(GetIsLineOfSightSectorListClear<CPtrListDoubleLink<CPhysical*>>, 0x564970);
    RH_ScopedInstall(SetCarsOnFire, 0x5659F0);
    RH_ScopedInstall(SetPedsChoking, 0x565800);
    RH_ScopedInstall(SetPedsOnFire, 0x565610);
    RH_ScopedInstall(SetWorldOnFire, 0x56B910);
    RH_ScopedInstall(SetAllCarsCanBeDamaged, 0x5668F0);
    RH_ScopedInstall(CallOffChaseForAreaSectorListVehicles, 0x563A80);
    RH_ScopedInstall(CallOffChaseForAreaSectorListPeds, 0x563D00);
    RH_ScopedInstall(RemoveEntityInsteadOfProcessingIt, 0x563A10);
    RH_ScopedOverloadedInstall(TestForUnusedModels<CPtrListDoubleLink<CPhysical*>>, "InputArray", 0x5639D0, void(*)(CPtrListDoubleLink<CPhysical*>&, int32*));
    RH_ScopedOverloadedInstall(TestForBuildingsOnTopOfEachOther<CPtrListDoubleLink<CPhysical*>>, "", 0x563950, void(*)(CPtrListDoubleLink<CPhysical*>&));
    RH_ScopedInstall(RemoveStaticObjects, 0x563840);
    RH_ScopedInstall(ClearScanCodes, 0x563470);
    RH_ScopedInstall(CastShadowSectorList<CPtrListDoubleLink<CPhysical*>>, 0x563390);
    RH_ScopedInstall(ResetLineTestOptions, 0x5631C0);
    RH_ScopedInstall(RepositionCertainDynamicObjects, 0x56B9C0);
    RH_ScopedInstall(CameraToIgnoreThisObject, 0x563F40);
    RH_ScopedInstall(RemoveReferencesToDeletedObject, 0x565510);
    RH_ScopedInstall(ClearForRestart, 0x564360);

    RH_ScopedInstall(GetCurrentScanCode, 0x407250);
    RH_ScopedInstall(GetSector, 0x407260);
    RH_ScopedInstall(GetRepeatSector, 0x4072A0);
    RH_ScopedInstall(GetLodPtrList, 0x4072C0);

    RH_ScopedGlobalInstall(FindPlayerCoors, 0x56E010);
    RH_ScopedGlobalInstall(FindPlayerSpeed, 0x56E090);
    RH_ScopedGlobalInstall(FindPlayerEntity, 0x56E120);
    RH_ScopedGlobalInstall(FindPlayerTrain, 0x56E160);
    RH_ScopedGlobalInstall(FindPlayerCentreOfWorld, 0x56E250);
    RH_ScopedGlobalInstall(FindPlayerCentreOfWorld_NoSniperShift, 0x56E320);
    RH_ScopedGlobalInstall(FindPlayerCentreOfWorldForMap, 0x56E400);
    RH_ScopedGlobalInstall(FindPlayerHeading, 0x56E450);
    RH_ScopedGlobalInstall(FindPlayerPed, 0x56E210);
    RH_ScopedGlobalInstall(FindPlayerVehicle, 0x56E0D0);
    RH_ScopedGlobalInstall(FindPlayerWanted, 0x56E230);
}

// 0x5631C0
void CWorld::ResetLineTestOptions() {
    bIncludeDeadPeds  = false;
    bIncludeCarTyres  = false;
    bIncludeBikers    = false;
    pIgnoreEntity     = nullptr;
    fWeaponSpreadRate = 0.0f;
}

// 0x5631E0
void CWorld::Initialise() {
    ZoneScoped;

    bDoingCarCollisions    = false;
    bNoMoreCollisionTorque = false;
    bIncludeDeadPeds       = false;
    bIncludeCarTyres       = false;
    bIncludeBikers         = false;
    bForceProcessControl   = false;
    bProcessCutsceneOnly   = false;
    bSecondShift           = false;
    pIgnoreEntity          = nullptr;
    fWeaponSpreadRate      = 0.0f;

    CIplStore::Initialise();
}

// 0x563220
void CWorld::Add(CEntity* entity) {
    entity->UpdateRwMatrix();
    entity->UpdateRwFrame();
    entity->Add();
    if (!entity->GetIsTypeBuilding() && !entity->GetIsTypeDummy()) {
        if (!entity->GetIsStatic()) {
            entity->AsPhysical()->AddToMovingList();
        }
    }
}

// Remove entity from the world
// 0x563280
void CWorld::Remove(CEntity* entity) {
    entity->Remove();
    if (entity->GetIsTypePhysical()) {
        entity->AsPhysical()->RemoveFromMovingList();
    }
}

// 0x5632B0
template<typename PtrListType>
bool CWorld::ProcessVerticalLineSectorList(PtrListType& ptrList, const CColLine& colLine, CColPoint& colPoint, float& maxTouchDistance, CEntity*& outEntity, bool doSeeThroughCheck, CStoredCollPoly* collPoly) {
    auto maxTouchDistanceLocal{ maxTouchDistance };

    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision() || entity == pIgnoreEntity) {
            continue;
        }

        entity->SetCurrentScanCode();

        if (CCollision::ProcessVerticalLine(
                colLine,
                entity->GetMatrix(),
                *entity->GetColModel(),
                colPoint,
                maxTouchDistanceLocal,
                doSeeThroughCheck,
                false,
                collPoly
            )) {
            outEntity = entity;
            ms_iProcessLineNumCrossings += CCollision::ms_iProcessLineNumCrossings;
        }
    }

    if (maxTouchDistanceLocal < maxTouchDistance) {
        maxTouchDistance = maxTouchDistanceLocal;
        return true;
    }

    return false;
}

// inline
// debug function
// 0x563390
template<typename PtrListType>
inline void CWorld::CastShadowSectorList(PtrListType& ptrList, float xmin, float ymin, float xmax, float ymax) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision()) {
            continue;
        }
        entity->SetCurrentScanCode();

        // NOP?
    }
}

// unused
// debug function
// 0x5633D0
void CWorld::ProcessForAnimViewer() {
    for (auto* const entity : ms_listMovingEntityPtrs) {
        if (!entity->m_bRemoveFromWorld) {
            entity->UpdateAnim();
            entity->UpdateRwMatrix();
            entity->UpdateRwFrame(); // Como el original (0x5633EF..0x56341A): tras la matriz llama a CEntity::UpdateRwFrame
        }
    }
}

// 0x563430
void CWorld::ProcessPedsAfterPreRender() {
    ZoneScoped;

    if (CTimer::bSkipProcessThisFrame) {
        return;
    }

    // Como el original (0x563444..0x56345F): solo se mira el tipo; bRemoveFromWorld no se comprueba
    for (auto* const entity : ms_listMovingEntityPtrs) {
        if (entity->GetIsTypePed()) {
            entity->AsPed()->GetIntelligence()->ProcessAfterPreRender();
        }
    }
}

// 0x563470
void CWorld::ClearScanCodes() {
    const auto ProcessList = []<typename PtrListType>(PtrListType& list) {
        for (auto* const entity : list) {
            entity->SetScanCode(0);
        }
    };

    for (auto y = 0; y < MAX_SECTORS_Y; y++) {
        for (auto x = 0; x < MAX_SECTORS_X; x++) {
            auto& sector = GetSector(x, y);
            ProcessList(sector.Buildings);
            ProcessList(sector.Dummies);
        }
    }

    for (int32 y = 0; y < (int32)(MAX_REPEAT_SECTORS_Y); y++) {
        for (int32 x = 0; x < (int32)(MAX_REPEAT_SECTORS_X); x++) {
            auto& rs = GetRepeatSector(x, y);
            ProcessList(rs.Vehicles);
            ProcessList(rs.Peds);
            ProcessList(rs.Objects);
        }
    }
}

// Como el original (grupo A de CWorld, 0x563500..0x5693F0): el sector es ftol(floor(coord * 0.02f + 60.0f)) con x87 a 24 bits (multiplica, no divide),
// la lista LOD usa ftol(coord * 0.005f + 15.0f) (truncamiento, sin floor ni limites) y el sector repetido usa "& 0xF" sobre el entero con signo.
namespace {
int32 WldASectorCoord(float coord) {
    const float scaled = coord * 0.02f;
    return (int32)(int64)std::floor((double)(scaled + 60.0f));
}
int32 WldALodSectorCoord(float coord) {
    const float scaled = coord * 0.005f;
    return (int32)(int64)(scaled + 15.0f);
}
CSector& WldASector(int32 x, int32 y) {
    return CWorld::ms_aSectors[std::min(std::max(y, 0), MAX_SECTORS_Y - 1)][std::min(std::max(x, 0), MAX_SECTORS_X - 1)];
}
CRepeatSector& WldARepeatSector(int32 x, int32 y) {
    return CWorld::ms_aRepeatSectors[y & 0xF][x & 0xF];
}
// Como el original: distancia al cuadrado con las sumas del EXE; 2D = dx*dx + dy*dy, 3D = (dx*dx + dz*dz) + dy*dy
float WldADistSq(const CVector& a, const CVector& b, bool b2D) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    if (b2D) {
        const float xx = dx * dx;
        const float yy = dy * dy;
        return xx + yy;
    }
    const float xx = dx * dx;
    const float zz = dz * dz;
    const float xz = xx + zz;
    const float yy = dy * dy;
    return xz + yy;
}
// Como el original (CWorld::FindObjectsKindaColliding): 2D = sqrt(dx*dx + dy*dy), 3D = sqrt((dz*dz + dy*dy) + dx*dx)
float WldADistKinda(const CVector& a, const CVector& b, bool b2D) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    if (b2D) {
        const float xx = dx * dx;
        const float yy = dy * dy;
        return std::sqrt(xx + yy);
    }
    const float zz = dz * dz;
    const float yy = dy * dy;
    const float zy = zz + yy;
    const float xx = dx * dx;
    return std::sqrt(zy + xx);
}
// Como el original: distancia del vecino mas cercano, 2D = sqrt(dx*dx + dy*dy), 3D = sqrt((dx*dx + dz*dz) + dy*dy)
float WldADistNearest(const CVector& a, const CVector& b, bool b2D) {
    return std::sqrt(WldADistSq(a, b, b2D));
}
// Como el original: Multiply3x3 (0x59C810), vector por matriz, con el orden de sumas del EXE
CVector WldAMultiply3x3VM(const CVector& v, const CMatrix& m) {
    return {
        m.GetRight().y * v.y + m.GetRight().z * v.z + v.x * m.GetRight().x,
        m.GetForward().y * v.y + m.GetForward().x * v.x + m.GetForward().z * v.z,
        m.GetUp().y * v.y + m.GetUp().x * v.x + m.GetUp().z * v.z,
    };
}
// Como el original: el EXE usa el CColModel del modelo (no el especial del vehiculo) y el indice de modelo con signo
float WldABoundRadius(CEntity* entity) {
    return CModelInfo::GetModelInfo((int16)entity->m_nModelIndex)->GetColModel()->GetBoundRadius();
}
// Como el original: las cinco listas de un sector, en el orden edificios, vehiculos, peatones, objetos, dummies
template<typename Fn>
void WldAForEachList(int32 x, int32 y, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, Fn&& fn) {
    auto& sector = WldASector(x, y);
    auto& repeatSector = WldARepeatSector(x, y);
    if (buildings) {
        fn(sector.Buildings);
    }
    if (vehicles) {
        fn(repeatSector.Vehicles);
    }
    if (peds) {
        fn(repeatSector.Peds);
    }
    if (objects) {
        fn(repeatSector.Objects);
    }
    if (dummies) {
        fn(sector.Dummies);
    }
}
}

// 0x563500
template<typename PtrListType>
void CWorld::FindObjectsInRangeSectorList(PtrListType& ptrList, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    const float radiusSq = radius * radius;
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        // Como el original: solo cuenta si dist < radio^2 (estricto; NaN no cuenta)
        if (!(WldADistSq(point, entity->GetPosition(), b2D) < radiusSq)) {
            continue;
        }

        if (*outCount < maxCount) {
            if (outEntities) {
                outEntities[*outCount] = entity;
            }
            ++*outCount;
        }
    }
}

// 0x5635C0
template<typename PtrListType>
void CWorld::FindObjectsOfTypeInRangeSectorList(uint32 modelId, PtrListType& ptrList, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    const float radiusSq = radius * radius;
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        // Como el original: el indice de modelo se lee con signo (movsx)
        if ((int32)(int16)entity->m_nModelIndex != (int32)modelId) {
            continue;
        }

        if (!(WldADistSq(point, entity->GetPosition(), b2D) < radiusSq)) {
            continue;
        }

        if (*outCount < maxCount) {
            if (outEntities) {
                outEntities[*outCount] = entity;
            }
            ++*outCount;
        }
    }
}

namespace {
// Como el original (0x40FC80): copia campo a campo sin los rellenos de la CColPoint (0xC, 0x1C y 0x26..0x27)
void WorldCopyColPointExe(CColPoint& dst, const CColPoint& src) {
    auto*       d = reinterpret_cast<uint8*>(&dst);
    const auto* sp = reinterpret_cast<const uint8*>(&src);
    std::memcpy(d + 0x00, sp + 0x00, 0xC);
    std::memcpy(d + 0x10, sp + 0x10, 0xC);
    std::memcpy(d + 0x20, sp + 0x20, 6);
    std::memcpy(d + 0x28, sp + 0x28, 4);
}
}

// 0x5636A0
template<typename PtrListType>
bool CWorld::ProcessVerticalLineSectorList_FillGlobeColPoints(PtrListType& ptrList, const CColLine& colLine, CEntity*& outEntity, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    // Como el original: outEntity y outCollPoly no se usan; la direccion es -1 salvo que (fin.z - inicio.z) >= 0 (NaN -> -1)
    const int32 direction = ((colLine.m_vecEnd.z - colLine.m_vecStart.z) >= 0.0f) ? 1 : -1;

    auto localColLine = colLine;

    // Como el original: si el nodo anterior dio un punto se vuelve a procesar LA MISMA entidad con la linea desplazada (incluso si era el ultimo nodo)
    typename PtrListType::NodeType* node = ptrList.GetNode();
    CEntity* entity{};
    bool dontGoToNextNode{};
    while (node || dontGoToNextNode) {
        if (!dontGoToNextNode) {
            entity = static_cast<CEntity*>(node->Item);
            node = node->Next;
            localColLine = colLine;
        }

        dontGoToNextNode = false;

        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision()) {
            continue;
        }

        entity->SetCurrentScanCode();

        float touchDist{ 1.f };
        CColPoint cp; // Como el original: sin inicializar
        if (!CCollision::ProcessVerticalLine(localColLine, entity->GetMatrix(), *entity->GetColModel(), cp, touchDist, doSeeThroughCheck, false, nullptr)) {
            continue;
        }

        if ((int32)FilledColPointIndex < (int32)std::size(gaTempSphereColPoints)) {
            // Como el original: el inicio de la linea se mueve siempre al punto de contacto
            const float distToEnd = localColLine.m_vecEnd.z - cp.m_vecPoint.z;
            localColLine.m_vecStart = cp.m_vecPoint;

            if (direction == 1 ? (distToEnd > 0.0f) : (distToEnd < 0.0f)) {
                entity->SetScanCode(ms_nCurrentScanCode - 1);
                dontGoToNextNode = true;
                WorldCopyColPointExe(gaTempSphereColPoints[FilledColPointIndex++], cp);
            }

            localColLine.m_vecStart.z += (float)direction * 0.1f;
        }
    }

    return (int32)FilledColPointIndex > 0;
}

// 0x563840
void CWorld::RemoveStaticObjects() {
    const auto ProcessList = []<typename PtrListType>(PtrListType& list) {
        for (auto* const entity : list) {
            Remove(entity);
            delete entity;
        }
    };

    for (auto y = 0; y < MAX_SECTORS_Y; y++) {
        for (auto x = 0; x < MAX_SECTORS_X; x++) {
            auto& sector = GetSector(x, y);
            ProcessList(sector.Buildings);
            ProcessList(sector.Dummies);
            sector.Buildings.Flush();
            sector.Dummies.Flush();
        }
    }

    for (auto y = 0; y < (int32)(MAX_REPEAT_SECTORS_Y); y++) {
        for (auto x = 0; x < (int32)(MAX_REPEAT_SECTORS_X); x++) {
            ProcessList(GetRepeatSector(x, y).Objects);
        }
    }
}

// debug function
// 0x563950
template<typename PtrListType>
void CWorld::TestForBuildingsOnTopOfEachOther(PtrListType& ptrList) {
    // Como el original (0x563950): solo recorre la lista leyendo las posiciones a locales; no compara modelos
    // ni registra nada (la comprobación de III Mobile no está en el EXE).
    for (typename PtrListType::NodeType *node1 = ptrList.GetNode(); node1; node1 = node1->Next) {
        [[maybe_unused]] const CVector pos1 = ((CEntity*)node1->Item)->GetPosition();
        for (typename PtrListType::NodeType *node2 = node1->Next; node2; node2 = node2->Next) {
            [[maybe_unused]] const CVector pos2 = ((CEntity*)node2->Item)->GetPosition();
        }
    }
}

// inline
// debug function
// 0x5639D0
template<typename PtrListType>
inline void CWorld::TestForUnusedModels(PtrListType& ptrList, int32* models) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        models[(int16)entity->m_nModelIndex]++; // Como el original: movsx del índice de modelo
    }
}

// 0x563A10
void CWorld::RemoveEntityInsteadOfProcessingIt(CEntity* entity) {
    if (entity->GetIsTypePed()) {
        if (FindPlayerPed() == entity) {
            Remove(entity);
        } else {
            CPopulation::RemovePed(entity->AsPed());
        }
    } else {
        Remove(entity);
        delete entity;
    }
}

// Como el original: CMatrix * CVector (0x59C890) con el orden de sumas del EXE (x: z, y, x; y: z, x, y; z: z, x, y)
static CVector WldD_MatTimesPoint(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// 0x563A80
void CWorld::CallOffChaseForAreaSectorListVehicles(CPtrListDoubleLink<CVehicle*>& ptrList, float x1, float y1, float x2, float y2, float minX, float minY, float maxX, float maxY) {
    for (auto* const veh : ptrList) {
        if (veh->IsScanCodeCurrent()) {
            continue;
        }

        veh->SetCurrentScanCode();

        if (veh == FindPlayerVehicle()) {
            continue;
        }

        const auto& pos = veh->GetPosition();

        // Como el original: limites estrictos (fcomp + test ah,41h / test ah,5 + jp); con NaN no pasa
        if (!(pos.x > minX && pos.x < maxX && pos.y > minY && pos.y < maxY)) {
            continue;
        }

        if (!veh->vehicleFlags.bIsLawEnforcer) {
            continue;
        }

        // Checking the mission type
        switch (veh->m_autoPilot.m_nCarMission) {
        case eCarMission::MISSION_RAMPLAYER_FARAWAY:
        case eCarMission::MISSION_RAMPLAYER_CLOSE:
        case eCarMission::MISSION_BLOCKPLAYER_FARAWAY:
        case eCarMission::MISSION_BLOCKPLAYER_CLOSE:
        case eCarMission::MISSION_DO_DRIVEBY_FARAWAY:
        case eCarMission::MISSION_DO_DRIVEBY_CLOSE:
            break;
        default:
            continue;
        }

        veh->m_autoPilot.SetTempAction(TEMPACT_WAIT, 2'000);

        const auto colData = veh->GetColModel()->m_pColData;
        // Como el original: el contador de esferas se lee con signo (movsx / jle)
        if (!colData || (int16)colData->m_nNumSpheres <= 0) {
            continue;
        }

        // Como el original: se recorren todas las esferas (sin salir al encontrar la primera) y la posicion
        // de cada esfera es matriz * centro (0x59C890), traslacion incluida
        const auto& mat = veh->GetMatrix();
        bool spheresIntersect = false;
        for (int32 i = 0; i < (int16)colData->m_nNumSpheres; i++) {
            const auto& sphere    = colData->m_pSpheres[i];
            const auto  spherePos = WldD_MatTimesPoint(mat, sphere.m_vecCenter);
            const auto  radius    = sphere.m_fRadius;

            if (spherePos.x + radius > x1 && spherePos.x - radius < x2 &&
                spherePos.y + radius > y1 && spherePos.y - radius < y2) {
                spheresIntersect = true;
            }
        }

        if (!spheresIntersect) {
            continue;
        }

        auto& speed = veh->m_vecMoveSpeed;

        // Como el original: con NaN en la comparacion con el centro se toma la rama del minimo
        if (pos.x > (x1 + x2) * 0.5f) {
            speed.x = std::max(speed.x, 0.0f);
        } else {
            speed.x = std::min(speed.x, 0.0f);
        }

        if (pos.y > (y1 + y2) * 0.5f) {
            speed.y = std::max(speed.y, 0.0f);
        } else {
            speed.y = std::min(speed.y, 0.0f);
        }
    }
}

// 0x563D00
void CWorld::CallOffChaseForAreaSectorListPeds(CPtrListDoubleLink<CPed*>& list, float minX, float minY, float maxX, float maxY, float biggerMinX, float biggerMinY, float biggerMaxX, float biggerMaxY) {
    for (auto* const ped : list) {
        if (ped->IsScanCodeCurrent()) {
            continue;
        }

        ped->SetCurrentScanCode();
        CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);

        CPed* leader = pedGroup ? pedGroup->GetMembership().GetLeader() : nullptr;

        if (ped == FindPlayerPed() || leader == FindPlayerPed()) {
            continue;
        }

        // Como el original: se usa el area exacta (los cuatro primeros limites, estrictos) y los "bigger" no se leen;
        // con NaN no pasa
        const auto& pos = ped->GetPosition();
        if (!(pos.x > minX && pos.x < maxX && pos.y > minY && pos.y < maxY)) {
            continue;
        }

        CTaskComplexKillPedOnFoot* taskKill = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>(true);
        CPed* objectivePed = taskKill ? taskKill->m_target : nullptr;

        CTaskComplexDestroyCar* taskDestroy = ped->GetTaskManager().Find<CTaskComplexDestroyCar>(true);
        CVehicle* objectiveVehicle = taskDestroy ? taskDestroy->m_VehicleToDestroy : nullptr;

        // Como el original (0x563E43..0x563E5D): solo se llama a la huida de los que persiguen al jugador,
        // bien a pie (objetivo = jugador) o bien destruyendo su coche
        if (objectivePed != FindPlayerPed() && !(objectiveVehicle && objectiveVehicle == FindPlayerVehicle())) {
            continue;
        }

        if (!ped->IsAlive()) {
            continue;
        }

        if (ped->m_nPedType == PED_TYPE_COP) {
            FindPlayerWanted()->RemovePursuitCop(ped->AsCop());
        }

        // Como el original: CTaskSimpleStandStill(0, true, false, 8.0f)
        ped->GetEventGroup().Add(CEventScriptCommand(3, new CTaskSimpleStandStill(0, true, false, 8.0f)));
    }
}

// unused
// 0x563F20
void CWorld::HandleCollisionZoneChange(eLevelName oldZone, eLevelName newZone) {
    // NOP
}

// unused
// debug function
// 0x563F30
void CWorld::DoZoneTestForChaser(CPhysical* physical) {
    // NOP
}

// 0x563F40
bool CWorld::CameraToIgnoreThisObject(CEntity* entity) {
    // Como el original (movsx word [esi+22h]): el indice de modelo se extiende con signo
    if (CGarages::IsModelIndexADoor(static_cast<int16>(entity->m_nModelIndex))) {
        return false;
    }

    switch (entity->AsObject()->m_pObjectInfo->m_nCameraAvoidObject) {
    case 1:
        return gCurCamColVars >= 10u;
    case 2:
        return false;
    case 3:
        return gCurCamColVars < 10u;
    default:
        return true;
    }
}

// Returns p ID (0 or 1), -1 - not found
// 0x563FA0
int32 CWorld::FindPlayerSlotWithPedPointer(void* ped) {
    for (int32 i = 0; i < MAX_PLAYERS; i++) {
        if (Players[i].m_pPed == ped) {
            return i;
        }
    }
    return -1;
}

// Returns p ID (0 or 1), -1 - not found
// 0x563FD0
int32 CWorld::FindPlayerSlotWithRemoteVehiclePointer(void* vehicle) {
    for (int32 i = 0; i < MAX_PLAYERS; i++) {
        if (Players[i].m_pRemoteVehicle == vehicle) {
            return i;
        }
    }
    return -1;
}

// Returns p ID (0 or 1)
// 0x564000
int32 CWorld::FindPlayerSlotWithVehiclePointer(CEntity* vehiclePtr) {
    // Como el original: compara directamente CPed::m_pVehicle (sin mirar bInVehicle) y, si no lo encuentra, devuelve 0 (no -1)
    for (int32 i = 0; i < MAX_PLAYERS; i++) {
        if (const auto* const ped = Players[i].m_pPed) {
            if (ped->m_pVehicle == vehiclePtr) {
                return i;
            }
        }
    }
    return 0;
}

// 0x564050
// This code is mostly similar to the original
void CWorld::ShutDown() {
    CIplStore::Shutdown(); // Como el original (0x405FA0): es lo primero que hace

    const auto IterateLodLists = [](auto&& fn) {
        for (auto y = 0; y < MAX_LOD_PTR_LISTS_Y; y++) {
            for (auto x = 0; x < MAX_LOD_PTR_LISTS_X; x++) {
                fn(ms_aLodPtrLists[y][x], x, y, "Lod");
            }
        }
    };

    const auto IterateRepeatSectorsLists = [](auto&& fn) {
        for (auto y = 0; y < (int32)(MAX_REPEAT_SECTORS_Y); y++) {
            for (auto x = 0; x < (int32)(MAX_REPEAT_SECTORS_X); x++) {
                auto& sector = GetRepeatSector(x, y);
                fn(sector.Vehicles, x, y, "Vehicles");
                fn(sector.Peds, x, y, "Peds");
                fn(sector.Objects, x, y, "Objects");
            }
        }
    };

    // Delete entities
    {
        const auto DeleteEntitiesInList = []<typename PtrListType>(PtrListType& list, int32 x, int32 y, const char* listName) {
            for (auto* const entity : list) {
                Remove(entity);
                delete entity;
            }
        };
        IterateLodLists(DeleteEntitiesInList);
        IterateRepeatSectorsLists(DeleteEntitiesInList);

        // Como el original: tras borrar los edificios y los dummies de cada sector se vacían las dos listas
        for (auto y = 0; y < MAX_SECTORS_Y; y++) {
            for (auto x = 0; x < MAX_SECTORS_X; x++) {
                auto& sector = GetSector(x, y);
                DeleteEntitiesInList(sector.Buildings, x, y, "Buildings");
                DeleteEntitiesInList(sector.Dummies, x, y, "Dummies");
                sector.Buildings.Flush();
                sector.Dummies.Flush();
            }
        }
    }

    // Ensure sectors are empty
    // Como el original: primero los sectores y después los sectores repetidos, con los mensajes del EXE
    // (0x864F30...) y x = i % 120, y = i / 120 también en los repetidos, formateados con sprintf.
    {
        for (auto i = 0; i < MAX_SECTORS_X * MAX_SECTORS_Y; i++) {
            auto& sector = ms_aSectors[i / MAX_SECTORS_X][i % MAX_SECTORS_X];
            if (!sector.Buildings.IsEmpty()) {
                sprintf(gString, "Building overlap list %d,%d not empty\n", i % 0x78, i / 0x78);
                sector.Buildings.Flush();
            }
            if (!sector.Dummies.IsEmpty()) {
                sprintf(gString, "Dummy overlap list %d,%d not empty\n", i % 0x78, i / 0x78);
                sector.Dummies.Flush();
            }
        }
        for (auto i = 0; i < (int32)(MAX_REPEAT_SECTORS_X * MAX_REPEAT_SECTORS_Y); i++) {
            auto& sector = ms_aRepeatSectors[i / (int32)MAX_REPEAT_SECTORS_X][i % (int32)MAX_REPEAT_SECTORS_X];
            if (!sector.Vehicles.IsEmpty()) {
                sprintf(gString, "Vehicle overlap list %d,%d not empty\n", i % 0x78, i / 0x78);
                sector.Vehicles.Flush();
            }
            if (!sector.Peds.IsEmpty()) {
                sprintf(gString, "Ped overlap list %d,%d not empty\n", i % 0x78, i / 0x78);
                sector.Peds.Flush();
            }
            if (!sector.Objects.IsEmpty()) {
                sprintf(gString, "Object overlap list %d,%d not empty\n", i % 0x78, i / 0x78);
                sector.Objects.Flush();
            }
        }
    }

    ms_listMovingEntityPtrs.Flush();
    ms_listObjectsWithControlCode.Flush();

    for (auto& player : Players) {
        player.m_PlayerData.DeAllocateData();
    }
}

// 0x564360
void CWorld::ClearForRestart() {
    if (CCutsceneMgr::HasLoaded()) {
        CCutsceneMgr::DeleteCutsceneData();
    }

    CProjectileInfo::RemoveAllProjectiles();
    CObject::DeleteAllTempObjects();
    CObject::DeleteAllMissionObjects();
    CPopulation::ConvertAllObjectsToDummyObjects();

    const auto DeleteEntitiesInList = []<typename PtrListType>(PtrListType& list) {
        for (auto* const entity : list) {
            Remove(entity);
            delete entity;
        }
    };

    // Como el original: solo las 16x16 celdas de repeticion, en orden de memoria
    for (auto y = 0; y < (int32)MAX_REPEAT_SECTORS_Y; y++) {
        for (auto x = 0; x < (int32)MAX_REPEAT_SECTORS_X; x++) {
            auto& sector = GetRepeatSector(x, y);
            DeleteEntitiesInList(sector.Peds);
            DeleteEntitiesInList(sector.Vehicles);
        }
    }

    CPickups::ReInit();
    CPools::CheckPoolsEmpty();
}

// 0x564420
bool CWorld::ProcessVerticalLineSector_FillGlobeColPoints(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    bool bSuccess{};

    bSuccess |= buildings && ProcessVerticalLineSectorList_FillGlobeColPoints(sector.Buildings, colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= vehicles && ProcessVerticalLineSectorList_FillGlobeColPoints(repeatSector.Vehicles, colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= peds && ProcessVerticalLineSectorList_FillGlobeColPoints(repeatSector.Peds, colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= objects && ProcessVerticalLineSectorList_FillGlobeColPoints(repeatSector.Objects, colLine, outEntity, doSeeThroughCheck, outCollPoly);
    bSuccess |= dummies && ProcessVerticalLineSectorList_FillGlobeColPoints(sector.Dummies, colLine, outEntity, doSeeThroughCheck, outCollPoly);

    return bSuccess;
}

// 0x564500
bool CWorld::ProcessVerticalLineSector(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    float max{ 1.f };

    const auto ProcessSector = [&](auto&& list) { // Wrapper to make code more readable
        ProcessVerticalLineSectorList(list, colLine, outColPoint, max, outEntity, doSeeThroughCheck, outCollPoly);
    };

    if (buildings) {
        ProcessSector(sector.Buildings);
    }
    if (vehicles) {
        ProcessSector(repeatSector.Vehicles);
    }
    if (peds) {
        ProcessSector(repeatSector.Peds);
    }
    if (objects) {
        ProcessSector(repeatSector.Objects);
    }
    if (dummies) {
        ProcessSector(sector.Dummies);
    }

    return max < 1.f;
}

// unused
// debug function
// 0x564600
void CWorld::CastShadow(float xmin, float ymin, float xmax, float ymax) {
    const int32 left   = std::max(GetSectorX(xmin), 0);
    const int32 top    = std::max(GetSectorY(ymin), 0);
    const int32 right  = std::min(GetSectorX(xmax), MAX_SECTORS_X - 1);
    const int32 bottom = std::min(GetSectorY(ymax), MAX_SECTORS_Y - 1);

    AdvanceCurrentScanCode();

    for (int32 y = top; y <= bottom; y++) {
        for (int32 x = left; x <= right; x++) {
            CastShadowSectorList(GetSector(x, y).Buildings, 0.0f, 0.0f, 0.0f, 0.0f); // 4 useless floats
        }
    }
}

// 0x5647F0
void CWorld::ProcessAttachedEntities() {
    for (int32 i = GetVehiclePool()->GetSize(); i; i--) {
        if (CVehicle* veh = GetVehiclePool()->GetAt(i - 1)) {
            if (const auto attachedTo = veh->m_pAttachedTo) {
                veh->m_pEntityIgnoredCollision = attachedTo;
                veh->PositionAttachedEntity();
                veh->UpdateRwMatrix();
                veh->UpdateRwFrame();
            }
        }
    }

    for (int32 i = GetObjectPool()->GetSize(); i; i--) {
        if (CObject* obj = GetObjectPool()->GetAt(i - 1)) {
            if (const auto attachedTo = obj->m_pAttachedTo) {
                Remove(obj);

                obj->PositionAttachedEntity();
                obj->UpdateRwMatrix();
                obj->UpdateRwFrame();
                Add(obj);
            }
        }
    }
}

// 0x564970
template<typename PtrListType>
bool CWorld::GetIsLineOfSightSectorListClear(PtrListType& ptrList, const CColLine& colLine, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent() || !entity->GetUsesCollision()) {
            continue;
        }

        entity->SetCurrentScanCode();

        if (entity == pIgnoreEntity) {
            continue;
        }

        if (doCameraIgnoreCheck && CameraToIgnoreThisObject(entity)) {
            continue;
        }

        if (CCollision::TestLineOfSight(colLine, entity->GetMatrix(), *entity->GetColModel(), doSeeThroughCheck, false)) {
            return false;
        }
    }
    return true;
}

// 0x564A20
void CWorld::FindObjectsInRange(const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    const int32 startSectorX = WldASectorCoord(point.x - radius);
    const int32 startSectorY = WldASectorCoord(point.y - radius);
    const int32 endSectorX = WldASectorCoord(point.x + radius);
    const int32 endSectorY = WldASectorCoord(point.y + radius);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            WldAForEachList(sectorX, sectorY, buildings, vehicles, peds, objects, dummies, [&](auto& list) {
                FindObjectsInRangeSectorList(list, point, radius, b2D, outCount, maxCount, outEntities);
            });
        }
    }
}

// 0x564C70
void CWorld::FindObjectsOfTypeInRange(uint32 modelId, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    const int32 startSectorX = WldASectorCoord(point.x - radius);
    const int32 startSectorY = WldASectorCoord(point.y - radius);
    const int32 endSectorX = WldASectorCoord(point.x + radius);
    const int32 endSectorY = WldASectorCoord(point.y + radius);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            WldAForEachList(sectorX, sectorY, buildings, vehicles, peds, objects, dummies, [&](auto& list) {
                FindObjectsOfTypeInRangeSectorList(modelId, list, point, radius, b2D, outCount, maxCount, outEntities);
            });
        }
    }
}

// 0x564ED0
void CWorld::FindLodOfTypeInRange(uint32 modelId, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    // Como el original: indices LOD con ftol (truncamiento) y sin limitar al rango
    const int32 startSectorX = WldALodSectorCoord(point.x - radius);
    const int32 startSectorY = WldALodSectorCoord(point.y - radius);
    const int32 endSectorX = WldALodSectorCoord(point.x + radius);
    const int32 endSectorY = WldALodSectorCoord(point.y + radius);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            FindObjectsOfTypeInRangeSectorList(modelId, GetLodPtrList(sectorX, sectorY), point, radius, b2D, outCount, maxCount, outEntities);
        }
    }
}

// 0x565000
template<typename PtrListType>
void CWorld::FindObjectsKindaCollidingSectorList(PtrListType& ptrList, const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        // Como el original: sin parar al llenarse (se marcan todos los scan codes) y solo cuenta si dist < radio del modelo + radio (estricto; NaN no cuenta)
        const CVector centre = entity->GetBoundCentre();
        const float radiusToCheck = WldABoundRadius(entity) + radius;
        if (!(WldADistKinda(point, centre, b2D) < radiusToCheck)) {
            continue;
        }

        if (*outCount < maxCount) {
            if (outEntities) {
                outEntities[*outCount] = entity;
            }
            ++*outCount;
        }
    }
}

// 0x5650E0
template<typename PtrListType>
void CWorld::FindObjectsIntersectingCubeSectorList(PtrListType& ptrList, const CVector& min, const CVector& max, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        // Como el original: radio del CColModel del modelo y comparaciones ordenadas (NaN no cuenta)
        const float boundRadius = WldABoundRadius(entity);
        const CVector& pos = entity->GetPosition();

        if (pos.x + boundRadius >= min.x && pos.x - boundRadius <= max.x
            && pos.y + boundRadius >= min.y && pos.y - boundRadius <= max.y
            && pos.z + boundRadius >= min.z && pos.z - boundRadius <= max.z) {
            if (*outCount < maxCount) {
                if (outEntities) {
                    outEntities[*outCount] = entity;
                }
                ++*outCount;
            }
        }
    }
}

// 0x565200
template<typename PtrListType>
void CWorld::FindObjectsIntersectingAngledCollisionBoxSectorList(PtrListType& ptrList, const CBox& box, const CMatrix& transform, const CVector& point, int16* outCount, int16 maxCount, CEntity** outEntities) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        // Como el original: (pos - punto) * matriz con Multiply3x3 (0x59C810) y radio del CColModel del modelo
        const CVector& pos = entity->GetPosition();
        const CVector diff{ pos.x - point.x, pos.y - point.y, pos.z - point.z };
        CColSphere sphere{
            WldAMultiply3x3VM(diff, transform),
            WldABoundRadius(entity)
        };
        if (CCollision::TestSphereBox(sphere, box)) {
            if (*outCount < maxCount) {
                if (outEntities) {
                    outEntities[*outCount] = entity;
                }
                ++*outCount;
            }
        }
    }
}

// 0x565300
template<typename PtrListType>
void CWorld::FindMissionEntitiesIntersectingCubeSectorList(PtrListType& ptrList, const CVector& cornerA, const CVector& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool vehiclesList, bool pedsList, bool objectsList) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        if (vehiclesList) {
            if (entity->AsVehicle()->GetCreatedBy() != eVehicleCreatedBy::MISSION_VEHICLE) {
                continue;
            }
        } else if (pedsList) {
            if (!entity->AsPed()->IsCreatedByMission()) {
                continue;
            }
        } else { // Como el original: objetos de mision (tipo 2 o 6)
            switch (entity->AsObject()->m_nObjectType) {
            case eObjectType::OBJECT_MISSION:
            case eObjectType::OBJECT_MISSION2:
                break;
            default:
                continue;
            }
        }

        // Como el original: misma prueba que FindObjectsIntersectingCubeSectorList (con el radio del modelo), no un punto dentro de la caja
        const float boundRadius = WldABoundRadius(entity);
        const CVector& pos = entity->GetPosition();

        if (pos.x + boundRadius >= cornerA.x && pos.x - boundRadius <= cornerB.x
            && pos.y + boundRadius >= cornerA.y && pos.y - boundRadius <= cornerB.y
            && pos.z + boundRadius >= cornerA.z && pos.z - boundRadius <= cornerB.z) {
            if (*outCount < maxCount) {
                if (outEntities) {
                    outEntities[*outCount] = entity;
                }
                ++*outCount;
            }
        }
    }
}

// 0x565450
template<typename PtrListType>
void CWorld::FindNearestObjectOfTypeSectorList(int32 modelId, PtrListType& ptrList, const CVector& point, float radius, bool b2D, CEntity*& outNearestEntity, float& outNearestDist) {
    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode();

        // Como el original: el indice de modelo se lee con signo (movsx); modelId < 0 acepta cualquiera
        if ((int32)(int16)entity->m_nModelIndex != modelId && modelId >= 0) {
            continue;
        }

        // Como el original: solo gana si dist < la mejor distancia hasta ahora (estricto; NaN no cuenta)
        const float dist = WldADistNearest(point, entity->GetPosition(), b2D);
        if (dist < outNearestDist) {
            outNearestDist = dist;
            outNearestEntity = entity;
        }
    }
}

// 0x565510
void CWorld::RemoveReferencesToDeletedObject(CEntity* entity) {
    for (int32 i = GetPedPool()->GetSize(); i; i--) {
        if (CPed* ped = GetPedPool()->GetAt(i - 1)) {
            if (ped != entity) {
                ped->RemoveRefsToEntity(entity);
                if (ped->m_standingOnEntity == entity) {
                    ped->m_standingOnEntity = nullptr;
                }
            }
        }
    }

    for (int32 i = GetVehiclePool()->GetSize(); i; i--) {
        if (CVehicle* veh = GetVehiclePool()->GetAt(i - 1)) {
            if (veh != entity) {
                veh->RemoveRefsToEntity(entity);
                veh->RemoveRefsToVehicle(entity);
            }
        }
    }

    for (int32 i = GetObjectPool()->GetSize(); i; i--) {
        if (CObject* obj = GetObjectPool()->GetAt(i - 1)) {
            if (obj != entity) {
                obj->RemoveRefsToEntity(entity);
            }
        }
    }
}

// 0x565610
// NOTE: Radius is treated as a cuboid with the height of 10, width and length of 2 * radius
//       Radius for `fireCreator` ped is halved
void CWorld::SetPedsOnFire(float x, float y, float z, float radius, CEntity* fireCreator) {
    // Como el original (0x565610): las distancias son |pos - centro| < límite (resta y valor absoluto en float, no
    // centro ± radio), con NaN la comparación falla. La caja solo admite a la víctima si NO es invulnerable
    // (o no hay creador, o el creador es el jugador).
    const auto Abs = [](float v) { return v < 0.0f ? -v : v; };

    for (int32 i = GetPedPool()->GetSize(); i; i--) {
        CPed* ped = GetPedPool()->GetAt(i - 1);
        if (!ped) {
            continue;
        }

        if (ped->m_nPedState == PEDSTATE_DEAD || ped->bInVehicle || ped->m_pFire || ped->physicalFlags.bFireProof) {
            continue;
        }

        const CVector& pos = ped->GetPosition();
        if (!(Abs(pos.z - z) < 5.0f)) {
            continue;
        }

        const float limit = (ped == fireCreator ? 0.5f : 1.0f) * radius; // Como el original: radio mitad para el propio creador
        if (!(Abs(pos.x - x) < limit)) {
            continue;
        }
        if (!(Abs(pos.y - y) < limit)) {
            continue;
        }

        if (!ped->physicalFlags.bInvulnerable
            || !fireCreator
            || (fireCreator->GetIsTypePed() && fireCreator->AsPed()->IsPlayer())) {
            gFireManager.StartFire(ped, fireCreator, 0.8f, 1, 7'000, 2);
        }
    }
}

// 0x565800
// NOTE: Radius is treated as a cuboid with the height of 10, width and length of 2 * radius
void CWorld::SetPedsChoking(float x, float y, float z, float radius, CEntity* gasCreator) {
    // Como el original (0x565800): |pos - centro| < límite en cada eje (z con 5,0), con NaN la comparación falla.
    const auto Abs = [](float v) { return v < 0.0f ? -v : v; };

    for (int32 i = GetPedPool()->GetSize(); i; i--) {
        CPed* ped = GetPedPool()->GetAt(i - 1);
        if (!ped) {
            continue;
        }

        if (ped->m_nPedState == PEDSTATE_DEAD || ped->bInVehicle || ped->m_pFire || ped->physicalFlags.bFireProof || ped->IsCreatedByMission()) {
            continue;
        }

        const CVector& pos = ped->GetPosition();
        if (!(Abs(pos.z - z) < 5.0f)) {
            continue;
        }
        if (!(Abs(pos.x - x) < radius)) {
            continue;
        }
        if (!(Abs(pos.y - y) < radius)) {
            continue;
        }

        CWeapon::GenerateDamageEvent(
            ped,
            gasCreator,
            eWeaponType::WEAPON_TEARGAS,
            1,
            PED_PIECE_TORSO,
            ped->GetLocalDirection(ped->GetPosition2D() - CVector2D{ x, y })
        );
    }
}

// 0x5659F0
// NOTE: Radius is treated as a cuboid with the height of 10, width and length of 2 * radius
void CWorld::SetCarsOnFire(CVector pos, float radius, CEntity* fireCreator) {
    // Como el original (0x5659F0): |pos - centro| < límite en cada eje (z con 5,0), con NaN la comparación falla.
    const auto Abs = [](float v) { return v < 0.0f ? -v : v; };

    for (int32 i = GetVehiclePool()->GetSize(); i; i--) {
        CVehicle* vehicle = GetVehiclePool()->GetAt(i - 1);
        if (!vehicle) {
            continue;
        }

        if (vehicle->GetStatus() == STATUS_WRECKED) {
            continue;
        }

        if (vehicle->m_pFire) {
            continue; // Already on fire
        }

        if (vehicle->physicalFlags.bFireProof) {
            continue;
        }

        const CVector& vpos = vehicle->GetPosition();
        if (!(Abs(vpos.z - pos.z) < 5.0f)) {
            continue;
        }
        if (!(Abs(vpos.x - pos.x) < radius)) {
            continue;
        }
        if (!(Abs(vpos.y - pos.y) < radius)) {
            continue;
        }

        gFireManager.StartFire(vehicle, fireCreator, 0.8f, 1, 7'000, 100);
    }
}

// 0x565B70
eSprayPaintState CWorld::SprayPaintWorld(CVector& posn, CVector& outDir, float radius, bool processTagAlphaState) {
    const int16 SPRAY_ALPHA_CHANGE = StaticRef<int16>(0x8CDEF0); // Como el original: se lee la variable del EXE (vale 8)

    CEntity* objects[15]; // Como el original: sin inicializar (solo se leen los 'count' primeros)
    int16 count;
    FindObjectsInRange(posn, radius, false, &count, (uint16)std::size(objects), objects, true, false, false, false, false);
    if (count <= 0) {
        return eSprayPaintState::NOT_FOUND;
    }

    bool hasChangedAlphaTo255{}, hasFoundTag{};
    // Como el original: con processTagAlphaState a false el valor nuevo no se asigna y queda el que había en el registro
    // (al principio el número de objetos, después el del tag anterior).
    int32 newAlpha = count;
    for (auto i = 0; i < count; i++) {
        auto entity = objects[i];
        if (!CTagManager::IsTag(*entity)) {
            continue;
        }

        hasFoundTag = true;

        outDir = entity->GetMatrix().GetForward(); // Must use GetMatrix to make sure the matrix is allocated

        const uint8 currAlpha = CTagManager::GetAlpha(*entity);
        if (processTagAlphaState) {
            newAlpha = std::min(int32(SPRAY_ALPHA_CHANGE) + int32(currAlpha), 255);
        }

        if (currAlpha < 255 && (int16)newAlpha == 255) {
            hasChangedAlphaTo255 = true;
        }

        CTagManager::SetAlpha(*entity, (uint8)newAlpha);
    }

    if (hasChangedAlphaTo255) {
        return eSprayPaintState::PAINTED;
    }

    return hasFoundTag ? eSprayPaintState::DISCOVERED : eSprayPaintState::NOT_FOUND;
}

// 0x565CB0
void CWorld::RemoveFallenPeds() {
    for (int32 i = GetPedPool()->GetSize(); i; i--) {
        CPed* ped = GetPedPool()->GetAt(i - 1);
        if (!ped) {
            continue;
        }

        const CVector& vecPedPos = ped->GetPosition();
        // Como el original (0x565CFF): solo por debajo del limite, estricto (con NaN no pasa)
        if (!(vecPedPos.z < MAP_Z_LOW_LIMIT)) {
            continue;
        }
        NOTSA_LOG_WARN("&&&&&&Another ped has fallen through the map&&&&&&&&&& {:4f} {:4f} {:4f}", vecPedPos.x, vecPedPos.y, vecPedPos.z); // R* log
        if (!ped->IsCreatedBy(ePedCreatedBy::PED_GAME) || ped->IsPlayer()) {
            // Como el original: distancia maxima 999999.88f (0x497423FE)
            CNodeAddress pathNodeAddress = ThePaths.FindNodeClosestToCoors(vecPedPos, PATH_TYPE_PED, 999999.88f, 0, 0, 0, 0, 0);
            // Como el original: solo se comprueba el area del nodo (cmp ax, 0FFFFh)
            if (pathNodeAddress.IsAreaValid()) {
                CVector pathNodePos = ThePaths.m_pPathNodes[pathNodeAddress.m_wAreaId][pathNodeAddress.m_wNodeId].GetPosition(); // Como el original: sin comprobar el nodo
                pathNodePos.z += 2.0f;
                ped->Teleport(pathNodePos, false);
            } else {
                // Como el original: solo se escribe la z de la matriz (sin comprobar que exista)
                ped->m_matrix->GetPosition().z = -95.0f;
            }
            ped->ResetMoveSpeed();
        } else {
            CPopulation::RemovePed(ped);
        }
    }
}

// 0x565E80
void CWorld::RemoveFallenCars() {
    for (int32 i = GetVehiclePool()->GetSize(); i; i--) {
        CVehicle* vehicle = GetVehiclePool()->GetAt(i - 1);
        if (!vehicle) {
            continue;
        }

        const CVector& vecPos = vehicle->GetPosition();
        // Como el original (0x565EDD): solo por debajo del limite, estricto (con NaN no pasa)
        if (!(vecPos.z < MAP_Z_LOW_LIMIT)) {
            continue;
        }

        NOTSA_LOG_WARN("&&&&&&Another vehicle has fallen through the map&&&&&&&&&& {:4f} {:4f} {:4f}", vecPos.x, vecPos.y, vecPos.z); // R* log

        const auto ShouldWeKeepIt = [vehicle]() {
            if (vehicle->IsCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE) && !vehicle->physicalFlags.bRenderScorched) {
                return true;
            }

            if (vehicle == FindPlayerVehicle()) {
                return true;
            }

            return vehicle->m_pDriver && vehicle->m_pDriver->IsPlayer();
        };

        if (ShouldWeKeepIt()) {
            // Como el original: nodos de vehiculos (tipo 0) y distancia maxima 999999.88f (0x497423FE)
            CNodeAddress pathNodeAddress = ThePaths.FindNodeClosestToCoors(vecPos, PATH_TYPE_VEH, 999999.88f, 0, 0, 0, 0, 0);
            // Como el original: solo se comprueba el area del nodo (cmp ax, 0FFFFh)
            if (pathNodeAddress.IsAreaValid()) {
                auto pathNodePos = ThePaths.m_pPathNodes[pathNodeAddress.m_wAreaId][pathNodeAddress.m_wNodeId].GetPosition(); // Como el original: sin comprobar el nodo
                pathNodePos.z += 3.0f;
                vehicle->Teleport(pathNodePos, true);
            } else {
                vehicle->Teleport(CVector(vecPos.x, vecPos.y, 0), true);
            }
            vehicle->ResetMoveSpeed();
        } else {
            if (!vehicle->IsCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE)) {
                if (!vehicle->IsCreatedBy(eVehicleCreatedBy::PARKED_VEHICLE)) {
                    continue;
                }
            }

            vehicle->Remove();
            if (vehicle->GetIsTypePhysical()) {
                vehicle->RemoveFromMovingList();
            }

            delete vehicle;
        }
    }
}

// 0x5660B0
void CWorld::UseDetonator(CPed* creator) {
    // Como el original (0x5660B0): recorre el pool de atrás adelante (el orden fija el de los RegisterReference) y solo
    // registra la referencia si el creador no es nulo.
    for (int32 i = GetVehiclePool()->GetSize(); i; i--) {
        CVehicle* veh = GetVehiclePool()->GetAt(i - 1);
        if (!veh) {
            continue;
        }

        if (veh->m_nBombOnBoard != 3) {
            continue;
        }

        if (veh->m_pWhoInstalledBombOnMe != creator) {
            continue;
        }

        veh->m_nBombOnBoard = 0;
        veh->m_wBombTimer = 500;
        veh->m_pWhoDetonatedMe = creator;
        if (creator) {
            creator->RegisterReference(reinterpret_cast<CEntity**>(&veh->m_pWhoDetonatedMe));
        }
    }
    CProjectileInfo::RemoveDetonatorProjectiles();
}

// 0x566140
// Find first entity colliding with the sphere
template<typename PtrListType>
CEntity* CWorld::TestSphereAgainstSectorList(PtrListType& ptrList, CVector sphereCenter, float sphereRadius, CEntity* ignoreEntity, bool doCameraIgnoreCheck) {
    if (ptrList.IsEmpty()) {
        return nullptr;
    }

    CColModel      sphereColModel{};
    CCollisionData sphereColData{};

    sphereColModel.m_pColData = &sphereColData;
    sphereColModel.m_boundBox = { {-sphereRadius, -sphereRadius, -sphereRadius}, {sphereRadius, sphereRadius, sphereRadius} };
    sphereColModel.m_boundSphere = { {}, sphereRadius };

    CColSphere csphere{ {}, sphereRadius };
    sphereColData.m_nNumSpheres = 1;
    sphereColData.m_pSpheres = &csphere;

    CMatrix sphereMatrix{};
    sphereMatrix.SetTranslate(sphereCenter);

    for (auto* const entity : ptrList) {
        if (entity->IsScanCodeCurrent()) {
            continue;
        }

        entity->SetCurrentScanCode(); // Como el original: se marca antes de las demás comprobaciones

        if (!entity->GetUsesCollision() || ignoreEntity == entity) {
            continue;
        }

        if (doCameraIgnoreCheck && CameraToIgnoreThisObject(entity)) {
            continue;
        }

        // Como el original (0x566140): el radio sale del modelo de colisión del model info (no del GetColModel de la entidad),
        // la distancia es sqrt((dx² + dy²) + dz²) con dx = centro - centro de la entidad, y solo se sigue con dist < suma
        // (con NaN se descarta).
        const CVector boundCentre = entity->GetBoundCentre();
        const float dx = sphereCenter.x - boundCentre.x;
        const float dy = sphereCenter.y - boundCentre.y;
        const float dz = sphereCenter.z - boundCentre.z;
        const float dist = sqrtf(dx * dx + dy * dy + dz * dz);
        if (!(dist < sphereRadius + entity->GetModelInfo()->GetColModel()->m_boundSphere.m_fRadius)) {
            continue; // Bound spheres not colliding
        }

        CColModel& entityColModel = *entity->GetColModel();

        if (CCollision::ProcessColModels(sphereMatrix, sphereColModel, entity->GetMatrix(), entityColModel, gaTempSphereColPoints, nullptr, nullptr, false)) {
            sphereColModel.m_pColData = nullptr; // Make sure CColModel destructor doesn't try to delete our local variable
            return entity;
        }
    }

    sphereColModel.m_pColData = nullptr; // Make sure CColModel destructor doesn't try to delete our local variable
    return nullptr;
}

// unused
// debug function
// 0x566420
void CWorld::PrintCarChanges() {
    // Como el original: la tabla de modelos es la del EXE (0xB7CBA8) y el aviso sale por el printf del CRT del juego (0x821982)
    auto* const modelIndexes = reinterpret_cast<uint32*>(0xB7CBA8);
    CVehiclePool* vehiclePool = GetVehiclePool();

    // Como el original: de la ultima ranura a la primera; solo cuentan los automoviles (el resto y las ranuras
    // libres se registran como modelo 0) y el modelo se extiende con signo
    for (int32 i = vehiclePool->GetSize(); i != 0;) {
        i--;
        CVehicle* veh = vehiclePool->GetAt(i);

        int32 modelNow = 0;
        if (veh && veh->IsAutomobile()) {
            modelNow = (int32)(int16)veh->m_nModelIndex;
        }

        if ((int32)modelIndexes[i] == modelNow) {
            continue;
        }

        reinterpret_cast<int(__cdecl*)(const char*, ...)>(0x821982)("Car ModelIndex (slot:%d) has changed from %d into %d\n", i, (int32)modelIndexes[i], modelNow);
        modelIndexes[i] = (uint32)modelNow;
    }
}

// unused
// debug function
// In III Mobile
void CWorld::CheckBuildingOrientations() {
    const auto ProcessList = []<typename PtrListType>(PtrListType& list) {
        for (typename PtrListType::NodeType* node = list.GetNode(); node; node = node->Next) {
            // NOP
        }
    };

    for (auto y = 0; y < MAX_SECTORS_Y; y++) {
        for (auto x = 0; x < MAX_SECTORS_X; x++) {
            auto& sector = GetSector(x, y);
            ProcessList(sector.Buildings);
        }
    }
}

// unused
// debug function
// In mobile NOP
// 0x5664A0
void CWorld::TestForBuildingsOnTopOfEachOther() {
    for (auto y = 0; y < MAX_SECTORS_Y; y++) {
        for (auto x = 0; x < MAX_SECTORS_X; x++) {
            auto& sector = GetSector(x, y);
            TestForBuildingsOnTopOfEachOther(sector.Buildings);
            TestForBuildingsOnTopOfEachOther(sector.Dummies);
        }
    }
}

// unused
// debug function
// 0x566510
void CWorld::TestForUnusedModels() {
    static int32 usageModelCounts[TOTAL_DFF_MODEL_IDS]{}; // SA uses a stack-allocated variable, but 80 kB on the stack isn't nice, so we are going to do it this way
    std::ranges::fill(usageModelCounts, 0);

    AdvanceCurrentScanCode();

    for (auto y = 0; y < MAX_SECTORS_Y; y++) {
        for (auto x = 0; x < MAX_SECTORS_X; x++) {
            auto& sector = GetSector(x, y);
            TestForUnusedModels(sector.Buildings, usageModelCounts);
            TestForUnusedModels(sector.Dummies, usageModelCounts);
        }
    }

    // Como el original (0x566510): termina aquí; el informe de modelos sin usar de III Mobile no está en el EXE.
}

// 0x566610
void CWorld::ClearCarsFromArea(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    CBoundingBox box{ { minX, minY, minZ }, { maxX, maxY, maxZ } }; // NOTSA, but makes code cleaner
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        // Como el original (0x566659..0x566674): FindPlayerPed() se desreferencia en cada vehiculo, antes de mirar el tipo
        if (FindPlayerPed()->m_pContactEntity == &veh && veh.IsBoat()) {
            continue;
        }

        if (!box.IsPointWithin(veh.GetPosition())) {
            continue;
        }

        if (veh.vehicleFlags.bIsLocked || !veh.CanBeDeleted()) {
            continue;
        }

        RemoveVehicleAndItsOccupants(&veh);
    }
}

// 0x5667F0
void CWorld::ClearPedsFromArea(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    CBoundingBox box{ {minX, minY, minZ}, {maxX, maxY, maxZ} }; // NOTSA, but makes code cleaner
    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (!ped.IsPlayer() && ped.CanBeDeleted()) {
            if (box.IsPointWithin(ped.GetPosition())) {
                CPopulation::RemovePed(&ped);
            }
        }
    }
}

// 0x5668F0
void CWorld::SetAllCarsCanBeDamaged(bool enable) {
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        veh.vehicleFlags.bCanBeDamaged = enable;
    }
}

// 0x566950
void CWorld::ExtinguishAllCarFiresInArea(CVector point, float radius) {
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        // Como el original (0x5669CF..0x566A11): distancia al cuadrado sumando (dz^2 + dy^2) + dx^2 y comparacion
        // estricta con radio^2 (con igualdad o NaN no se apaga)
        const auto& vehPos = veh.GetPosition();
        const float dz     = point.z - vehPos.z;
        const float dy     = point.y - vehPos.y;
        const float dx     = point.x - vehPos.x;
        const float distSq = (dz * dz + dy * dy) + dx * dx;
        if (radius * radius > distSq) {
            veh.ExtinguishCarFire();
        }
    }
}

// 0x566A60
void CWorld::CallOffChaseForArea(float minX, float minY, float maxX, float maxY) {
    // Como el original (0x566A60): area ampliada 10 unidades; sector = ftol(floor(v * 0.02f + 60.0f)) con x87 a 24 bits
    const float biggerMinX = minX - 10.0f;
    const float biggerMinY = minY - 10.0f;
    const float biggerMaxX = maxX + 10.0f;
    const float biggerMaxY = maxY + 10.0f;
    const auto SectorCoord = [](float coord) {
        const float scaled = coord * 0.02f;
        return (int32)(int64)std::floor((double)(scaled + 60.0f));
    };
    const int32 sectorMinX = SectorCoord(biggerMinX);
    const int32 sectorMinY = SectorCoord(biggerMinY);
    const int32 sectorMaxX = SectorCoord(biggerMaxX);
    const int32 sectorMaxY = SectorCoord(biggerMaxY);

    AdvanceCurrentScanCode();

    // Como el original: filas (y) de fuera, columnas (x) de dentro, sin comprobar que el rango sea valido
    for (int32 y = sectorMinY; y <= sectorMaxY; y++) {
        for (int32 x = sectorMinX; x <= sectorMaxX; x++) {
            auto& sector = ms_aRepeatSectors[y & 0xF][x & 0xF];
            CallOffChaseForAreaSectorListVehicles(sector.Vehicles, minX, minY, maxX, maxY, biggerMinX, biggerMinY, biggerMaxX, biggerMaxY);
            CallOffChaseForAreaSectorListPeds(sector.Peds, minX, minY, maxX, maxY, biggerMinX, biggerMinY, biggerMaxX, biggerMaxY);
        }
    }
}

// 0x566C10
void CWorld::StopAllLawEnforcersInTheirTracks() {
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        if (veh.vehicleFlags.bIsLawEnforcer) {
            veh.m_vecMoveSpeed = CVector{};
        }
    }
}

// 0x566C90
CVehicle* CWorld::FindUnsuspectingTargetCar(CVector point, CVector playerPosn) {
    // Como el original: distancia inicial 9999.9 (0x864F90), recorre el pool de MAYOR a MENOR indice, y los
    // compara con NaN descartan el candidato (fcomp + test ah,5 / test ah,41h)
    float nearestDist2D = 9999.9f;
    CVehicle* nearestVeh{};
    auto* const pool = GetVehiclePool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const veh = pool->GetAt(i);
        if (!veh) {
            continue;
        }
        if (!veh->IsSubAutomobile() || !veh->IsCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE)) {
            continue;
        }

        switch (veh->GetStatus()) {
        case eEntityStatus::STATUS_PHYSICS:
        case eEntityStatus::STATUS_SIMPLE:
            break;
        default:
            continue;
        }

        const CVector& pos = veh->GetPosition();
        const float dx = point.x - pos.x;
        const float dy = point.y - pos.y;
        const float dist2D = std::sqrt(dx * dx + dy * dy);
        if (!(dist2D < nearestDist2D)) {
            continue;
        }

        // Como el original: (dz*fz + dy*fy) + dx*fx
        const CVector& fwd = veh->GetForward();
        const float pdx = playerPosn.x - pos.x;
        const float pdy = playerPosn.y - pos.y;
        const float pdz = playerPosn.z - pos.z;
        const float dot = (pdz * fwd.z + pdy * fwd.y) + pdx * fwd.x;
        if (!(dot > 0.0f)) {
            continue; // `point` is behind the ped
        }

        nearestVeh = veh;
        nearestDist2D = dist2D;
    }
    return nearestVeh;
}

// 0x566DA0
CPed* CWorld::FindUnsuspectingTargetPed(CVector point, CVector playerPosn) {
    // Como el original: distancia inicial 9999.9 (0x461C3F9A), recorre el pool de MAYOR a MENOR indice, acepta los
    // tipos CIVMALE y GANG1..GANG10 (ambos inclusive), descarta los que YA ESTAN en TASK_COMPLEX_WANDER y los
    // compara con NaN descartan el candidato
    float nearestDist2D = 9999.9f;
    CPed* nearestPed{};
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (!ped) {
            continue;
        }
        if (!ped->IsCreatedBy(ePedCreatedBy::PED_GAME) || !ped->IsAlive()) {
            continue;
        }

        const auto type = (int32)ped->m_nPedType;
        if (!(type >= (int32)PED_TYPE_GANG1 && type <= (int32)PED_TYPE_GANG10) && type != (int32)PED_TYPE_CIVMALE) {
            continue;
        }

        if (CTask* const active = ped->GetTaskManager().GetActiveTask()) {
            if (active->GetTaskType() == TASK_COMPLEX_WANDER) {
                continue;
            }
        }

        const CVector& pedPos = ped->GetPosition();
        const float dx = point.x - pedPos.x;
        const float dy = point.y - pedPos.y;
        const float dist2D = std::sqrt(dx * dx + dy * dy);
        if (!(dist2D < nearestDist2D)) {
            continue;
        }

        // Como el original: (dz*fz + dy*fy) + dx*fx
        const CVector& fwd = ped->GetForward();
        const float pdx = playerPosn.x - pedPos.x;
        const float pdy = playerPosn.y - pedPos.y;
        const float pdz = playerPosn.z - pedPos.z;
        const float dot = (pdz * fwd.z + pdy * fwd.y) + pdx * fwd.x;
        if (!(dot > 0.0f)) {
            continue; // `point` is behind the ped
        }

        nearestPed = ped;
        nearestDist2D = dist2D;
    }
    return nearestPed;
}

// 0x566EE0
template<typename PtrListType>
bool CWorld::ProcessLineOfSightSectorList(PtrListType& ptrList, const CColLine& colLine, CColPoint& outColPoint, float& minTouchDistance, CEntity*& outEntity, bool doSeeThroughCheck, bool doIgnoreCameraCheck, bool doShootThroughCheck) {
    // Como el original: el modelo de colision de las ruedas se construye siempre (en la pila)
    CCollisionData wheelColData;
    CColSphere     wheelSpheres[6];
    CColModel      wheelCol;

    auto* const head = ptrList.GetNode();

    float localMinTouchDist = minTouchDistance;

    // Como el original: estas tres marcas dependen solo de la ENTIDAD CABEZA de la lista
    bool headDeadPeds = false, headBikers = false, headTyres = false;

    if (bIncludeCarTyres) {
        if (head && static_cast<CEntity*>(head->Item)->GetIsTypeVehicle()) {
            headTyres = true;
            wheelCol.m_pColData = &wheelColData;
            wheelColData.m_nNumSpheres = 6;
            wheelColData.m_nNumBoxes = 0;
            wheelColData.m_nNumTriangles = 0;
            wheelColData.m_nNumLines = 0;
            wheelColData.m_pSpheres = wheelSpheres;
        }
    }

    if (head) {
        if (bIncludeDeadPeds && static_cast<CEntity*>(head->Item)->GetIsTypePed()) {
            headDeadPeds = true;
        }
        if (bIncludeBikers && static_cast<CEntity*>(head->Item)->GetIsTypePed()) {
            headBikers = true;
        }

        for (auto* node = head; node;) {
            CEntity* const entity = static_cast<CEntity*>(node->Item);
            const bool     scanCodeCurrent = entity->IsScanCodeCurrent();
            node = node->Next; // Como el original: el siguiente nodo se lee antes de procesar la entidad

            if (scanCodeCurrent || entity == pIgnoreEntity) {
                continue;
            }
            if (!entity->GetUsesCollision() && !headDeadPeds && !headBikers) {
                continue;
            }
            if (doIgnoreCameraCheck && CameraToIgnoreThisObject(entity)) {
                continue;
            }

            entity->SetCurrentScanCode();

            // Como el original: la distancia de las ruedas parte del valor que habia ANTES de procesar el modelo principal
            float wheelTouchDist = localMinTouchDist;

            CColModel* colModel = nullptr;
            if (entity->GetIsTypePed()) {
                const auto ped = entity->AsPed();
                if (ped->GetUsesCollision()
                    || ped->m_pAttachedTo
                    || (headDeadPeds && !ped->IsAlive())
                    || (headBikers && ped->bTestForShotInVehicle)) {
                    colModel = CModelInfo::GetModelInfo(entity->m_nModelIndex)->AsPedModelInfoPtr()->AnimatePedColModelSkinned(reinterpret_cast<RpClump*>(entity->GetRwObject()));
                }
            } else if (entity->GetUsesCollision()) {
                colModel = entity->GetColModel();
            }

            if (colModel && colModel->m_pColData) {
                const auto colData = colModel->m_pColData;

                float makeBiggerBy = 0.0f;

                if (fWeaponSpreadRate > 0.0f) {
                    // Como el original: ((dx*dx + dy*dy) + dz*dz) con x87 a 24 bits
                    const CVector& pos = entity->GetPosition();
                    const float    dx = pos.x - colLine.m_vecStart.x;
                    const float    dy = pos.y - colLine.m_vecStart.y;
                    const float    dz = pos.z - colLine.m_vecStart.z;
                    makeBiggerBy = std::sqrt((dx * dx + dy * dy) + dz * dz) * fWeaponSpreadRate;

                    colModel->m_boundBox.m_vecMin.x -= makeBiggerBy;
                    colModel->m_boundBox.m_vecMin.y -= makeBiggerBy;
                    colModel->m_boundBox.m_vecMin.z -= makeBiggerBy;
                    colModel->m_boundBox.m_vecMax.x += makeBiggerBy;
                    colModel->m_boundBox.m_vecMax.y += makeBiggerBy;
                    colModel->m_boundBox.m_vecMax.z += makeBiggerBy;
                    colModel->m_boundSphere.m_fRadius += makeBiggerBy;

                    for (int32 i = 0; i < (int16)colData->m_nNumSpheres; i++) {
                        colData->m_pSpheres[i].m_fRadius += makeBiggerBy;
                    }
                }

                if (CCollision::ProcessLineOfSight(
                        colLine,
                        entity->GetMatrix(),
                        *colModel,
                        outColPoint,
                        localMinTouchDist,
                        doSeeThroughCheck,
                        doShootThroughCheck
                    )) {
                    outEntity = entity;
                    ms_iProcessLineNumCrossings += CCollision::ms_iProcessLineNumCrossings;
                }

                if (makeBiggerBy > 0.0f) {
                    colModel->m_boundBox.m_vecMin.x += makeBiggerBy;
                    colModel->m_boundBox.m_vecMin.y += makeBiggerBy;
                    colModel->m_boundBox.m_vecMin.z += makeBiggerBy;
                    colModel->m_boundBox.m_vecMax.x -= makeBiggerBy;
                    colModel->m_boundBox.m_vecMax.y -= makeBiggerBy;
                    colModel->m_boundBox.m_vecMax.z -= makeBiggerBy;
                    colModel->m_boundSphere.m_fRadius -= makeBiggerBy;

                    for (int32 i = 0; i < (int16)colData->m_nNumSpheres; i++) {
                        colData->m_pSpheres[i].m_fRadius -= makeBiggerBy;
                    }
                }
            }

            // Como el original: las ruedas se comprueban para toda entidad de una lista cuya cabeza es un vehiculo
            if (headTyres) {
                const auto veh = entity->AsVehicle();
                if (veh->SetUpWheelColModel(&wheelCol)) {
                    CColPoint wheelCP; // Como el original: sin inicializar
                    const auto& mat = veh->GetMatrix();

                    if (CCollision::ProcessLineOfSight(colLine, mat, wheelCol, wheelCP, wheelTouchDist, false, doShootThroughCheck)) {
                        ms_iProcessLineNumCrossings += CCollision::ms_iProcessLineNumCrossings;

                        if (wheelTouchDist < localMinTouchDist) {
                            localMinTouchDist = wheelTouchDist;
                            WorldCopyColPointExe(outColPoint, wheelCP);
                            outEntity = entity;
                        } else {
                            // Como el original: producto escalar con el orden (y*y + z*z) + x*x en x87 a 24 bits
                            const CVector& right = mat.GetRight();
                            const float    ldx = colLine.m_vecEnd.x - colLine.m_vecStart.x;
                            const float    ldy = colLine.m_vecEnd.y - colLine.m_vecStart.y;
                            const float    ldz = colLine.m_vecEnd.z - colLine.m_vecStart.z;
                            const float    lineDirDotRight = (ldy * right.y + ldz * right.z) + ldx * right.x;

                            const CVector& matPos = mat.GetPosition();
                            const float    wdx = wheelCP.m_vecPoint.x - matPos.x;
                            const float    wdy = wheelCP.m_vecPoint.y - matPos.y;
                            const float    wdz = wheelCP.m_vecPoint.z - matPos.z;
                            const float    wheelDotRight = (wdy * right.y + wdz * right.z) + wdx * right.x;

                            if ((lineDirDotRight < 0.0f && wheelDotRight > 0.0f) || (lineDirDotRight > 0.0f && wheelDotRight < 0.0f)) {
                                const float lineMagnitude = std::sqrt((ldx * ldx + ldy * ldy) + ldz * ldz);
                                if (std::abs(lineDirDotRight) / lineMagnitude > 0.5f) {
                                    localMinTouchDist = wheelTouchDist;
                                    WorldCopyColPointExe(outColPoint, wheelCP);
                                    outEntity = entity;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Como el original: el modelo de ruedas no es propietario de sus datos
    wheelCol.m_pColData = nullptr;

    if (localMinTouchDist < minTouchDistance) {
        minTouchDistance = localMinTouchDist;
        return true;
    }
    return false;
}

// Como el original (0x5674E0, 0x567620, 0x56BA00, 0x56A490): el sector es ftol(floor(coord * 0.02f + 60.0f)) con x87 a 24 bits (multiplica, no divide entre 50)
// y el sector repetido usa "& 0xF" sobre el entero con signo (no abs() % 16).
namespace {
int32 WorldSectorCoordExe(float coord) {
    const float scaled = coord * 0.02f + 60.0f;
    return (int32)(int64)std::floor((double)scaled);
}
CSector& WorldSectorExe(int32 x, int32 y) {
    return CWorld::ms_aSectors[std::min(std::max(y, 0), MAX_SECTORS_Y - 1)][std::min(std::max(x, 0), MAX_SECTORS_X - 1)];
}
CRepeatSector& WorldRepeatSectorExe(int32 x, int32 y) {
    return CWorld::ms_aRepeatSectors[y & 0xF][x & 0xF];
}
}

// 0x5674E0
// ProcessVerticalLine => ProcessVerticalLineSector => ProcessVerticalLineSectorList => CCollision::ProcessVerticalLine
bool CWorld::ProcessVerticalLine(const CVector& origin, float distance, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    AdvanceCurrentScanCode();
    ms_iProcessLineNumCrossings = 0;

    const int32 secX = WorldSectorCoordExe(origin.x), secY = WorldSectorCoordExe(origin.y);
    return ProcessVerticalLineSector(
        WorldSectorExe(secX, secY),
        WorldRepeatSectorExe(secX, secY),
        CColLine{ origin, CVector{origin.x, origin.y, distance} },
        outColPoint, outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly
    );
}

// 0x567620
bool CWorld::ProcessVerticalLine_FillGlobeColPoints(const CVector& origin, float distance, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
    AdvanceCurrentScanCode();

    const int32 secX = WorldSectorCoordExe(origin.x), secY = WorldSectorCoordExe(origin.y);
    FilledColPointIndex = 0;
    return ProcessVerticalLineSector_FillGlobeColPoints(
        WorldSectorExe(secX, secY),
        WorldRepeatSectorExe(secX, secY),
        CColLine{ origin, CVector{origin.x, origin.y, distance} },
        outEntity, buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, outCollPoly
    );
}

namespace {
// Como el original: Multiply3x3 (0x59C790), matriz por vector sin traslación, con el orden de sumas del EXE
CVector WldEMultiply3x3MV(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// Como el original: Multiply3x3 (0x59C810), vector por matriz (productos punto con los ejes), con el orden de sumas del EXE
CVector WldEMultiply3x3VM(const CVector& v, const CMatrix& m) {
    return {
        m.GetRight().y * v.y + m.GetRight().z * v.z + v.x * m.GetRight().x,
        m.GetForward().y * v.y + m.GetForward().x * v.x + m.GetForward().z * v.z,
        m.GetUp().y * v.y + m.GetUp().x * v.x + m.GetUp().z * v.z,
    };
}

// Como el original: MultiplyMatrixWithVector (0x59C890), con traslación y el orden de sumas del EXE
CVector WldEMatrixTimesPoint(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}
}

// 0x567750
// Como el original: sin comprobación de scan code, cálculos en float con el orden y las constantes del EXE (x87 a 24 bits):
// 1/radio y 1/1400 se multiplican (no se dividen), las sumas de distancias siguen el orden del EXE, etc.
template<typename PtrListType>
void CWorld::TriggerExplosionSectorList(PtrListType& ptrList, const CVector& point, float radius, float visibleDistance, CEntity* victim, CEntity* creator, bool processVehicleBombTimer, float damage) {
    constexpr float VEHICLE_EXPLOSION_THIS_EXPLODING_MULT = 0.2f; // 0x8CDF10
    constexpr float VEHICLE_EXPLOSION_STD_MASS = 3000.0f; // 0x8CDF0C

    // Como el original: variable de pila del EXE ([esp+0Ch]) que persiste entre entidades; el código de los ATM la lee antes de asignarla
    float entityRelDistToRadiusEnd_Doubled = 0.0f;

    for (auto* const entity : ptrList) {
        const CVector& entityPos = entity->GetPosition();
        const float dz = entityPos.z - point.z;
        const float dy = entityPos.y - point.y;
        const float dx = entityPos.x - point.x;
        const float entityToPointDist = sqrtf(dx * dx + dy * dy + dz * dz);

        if (!(entityToPointDist < radius)) { // con NaN se descarta
            continue;
        }

        if (entity->GetIsTypeObject()) {
            entity->AsObject()->TryToExplode();
        }

        if (entity->physicalFlags.bExplosionProof) {
            continue;
        }

        if (entity->GetIsTypePed() && entity->AsPed()->bInVehicle) {
            continue;
        }

        if (entity->GetIsStatic()) {
            if (entity->GetIsTypeObject()) {
                if (!entity->physicalFlags.bDisableCollisionForce) {
                    const auto object = entity->AsObject();

                    if (visibleDistance > object->m_pObjectInfo->m_fUprootLimit
                        || (int32)(int16)object->m_nModelIndex == (int32)ModelIndices::MI_FENCE.get_underlying()
                        || (int32)(int16)object->m_nModelIndex == (int32)ModelIndices::MI_FENCE2.get_underlying()) {
                        if (CGlass::IsObjectGlass(object)) {
                            CGlass::WindowRespondsToExplosion(object, point);
                        } else {
                            object->SetIsStatic(false);
                            object->AddToMovingList();

                            if ((int32)(int16)object->m_nModelIndex == (int32)ModelIndices::MI_FIRE_HYDRANT.get_underlying() && !object->objectFlags.bIsExploded) {
                                g_fx.TriggerWaterHydrant(object->GetPosition());
                                object->objectFlags.bIsExploded = true;
                            } else if (!object->m_pObjectInfo->m_bCausesExplosion) {
                                object->objectFlags.bIsExploded = true;
                            }
                        }
                    }
                }

                // Como el original: con el creador siendo el jugador, el ATM pierde vida con el valor residual de la variable de pila
                if ((int32)(int16)entity->m_nModelIndex == (int32)ModelIndices::MI_ATM.get_underlying() && creator == FindPlayerPed(-1)) {
                    const float health = entity->AsObject()->m_fHealth;
                    entity->AsObject()->m_fHealth = health - (entityRelDistToRadiusEnd_Doubled + entityRelDistToRadiusEnd_Doubled);
                }

                if (entity->GetIsStatic()) {
                    const float progress = (radius - entityToPointDist) / radius;
                    const float progress2x = progress + progress;
                    entityRelDistToRadiusEnd_Doubled = progress2x < 1.0f ? progress2x : 1.0f;
                    entity->AsObject()->ObjectDamage(entityRelDistToRadiusEnd_Doubled * 300.0f, nullptr, nullptr, creator, WEAPON_EXPLOSION);
                }
            } else if (entity->GetUsesCollision()) {
                entity->SetIsStatic(false);
                entity->AddToMovingList();
            }
        }

        if (entity->GetIsStatic() || !entity->GetUsesCollision()) {
            continue;
        }

        const float invRadius = 1.0f / radius;
        {
            const float progress2x = (radius - entityToPointDist) * invRadius;
            const float doubled = progress2x + progress2x;
            entityRelDistToRadiusEnd_Doubled = doubled < 1.0f ? doubled : 1.0f;
        }

        float impactVelocityFactor = entity->m_fMass * StaticRef<float>(0x864F98) * entityRelDistToRadiusEnd_Doubled * visibleDistance; // 0x864F98 = 1/1400

        const float invDist = 1.0f / (entityToPointDist < 0.01f ? 0.01f : entityToPointDist);
        CVector impactVelocity{ dx * invDist, dy * invDist, dz * invDist };
        if (impactVelocity.z < 0.0f) {
            impactVelocity.z = 0.0f;
        }

        switch (entity->GetType()) {
        case ENTITY_TYPE_VEHICLE: {
            const auto veh = entity->AsVehicle();

            if (auto driver = veh->m_pDriver; veh->IsSubBMX() && driver) {
                CEventKnockOffBike event{ veh, veh->m_vecMoveSpeed, impactVelocity, 0.f, 0.f, KNOCK_OFF_TYPE_EXPLOSION, 0, 0, nullptr, true, false };
                driver->GetIntelligence()->m_eventGroup.Add(&event, false);

                CWeapon::GenerateDamageEvent(
                    driver,
                    veh,
                    WEAPON_EXPLOSION,
                    (int32)(int64)(entityRelDistToRadiusEnd_Doubled * damage * 1100.f),
                    PED_PIECE_TORSO,
                    driver->GetLocalDirection(impactVelocity)
                );
            }

            if (entity->GetStatus() == STATUS_SIMPLE) {
                CCarCtrl::SwitchVehicleToRealPhysics(veh);
            }

            veh->InflictDamage(creator, WEAPON_EXPLOSION, entityRelDistToRadiusEnd_Doubled * damage * 1100.f, {});

            if (processVehicleBombTimer) {
                if (veh->m_wBombTimer) {
                    veh->m_wBombTimer = (int16)((uint16)veh->m_wBombTimer / 10 + 1); // Como el original: el temporizador se lee sin signo
                }
            }

            CVector colPointPos{};
            CColPoint cp; // Como el original: sin inicializar (solo se lee tras un choque o, para los aviones, sin él)
            float maxTouchDist{ 100'000.f };

            // Como el original: el centro de la esfera es -(entidad - punto) llevado al espacio del vehículo con Multiply3x3 (0x59C810)
            CColSphere sphere;
            sphere.Set(entityToPointDist, WldEMultiply3x3VM(CVector{ -dx, -dy, -dz }, veh->GetMatrix()), (eSurfaceType)0, 0, tColLighting{ 0xFF });
            CColBox box;
            box.Set(veh->GetColModel()->m_boundBox.m_vecMin, veh->GetColModel()->m_boundBox.m_vecMax, (eSurfaceType)0, 0, tColLighting{ 0xFF });

            if (CCollision::ProcessSphereBox(sphere, box, cp, maxTouchDist)) {
                colPointPos = WldEMultiply3x3MV(veh->GetMatrix(), cp.m_vecPoint);
                CVector colNormal = WldEMultiply3x3MV(veh->GetMatrix(), CVector{ -cp.m_vecNormal.x, -cp.m_vecNormal.y, -cp.m_vecNormal.z });
                if (colNormal.z < -0.2f) {
                    colNormal.z = -0.2f;
                } else if (colNormal.z < 0.2f && colNormal.z > 0.0f) {
                    colNormal.z += 0.2f;
                }

                const CVector& vehPos = veh->GetPosition();
                const float cx = (colPointPos.x + vehPos.x) - point.x;
                const float cy = (colPointPos.y + vehPos.y) - point.y;
                const float cz = (colPointPos.z + vehPos.z) - point.z;
                const float colPointToExploPointDist = sqrtf((cz * cz + cy * cy) + cx * cx); // Como el original: sumas en orden z, y, x

                float forceFactor = (radius - colPointToExploPointDist) * invRadius;
                forceFactor = forceFactor + forceFactor;
                if (!(forceFactor < 1.0f)) {
                    forceFactor = 1.0f;
                }
                if (entity == victim) {
                    forceFactor *= VEHICLE_EXPLOSION_THIS_EXPLODING_MULT;
                }

                float massFactor = veh->m_fMass / VEHICLE_EXPLOSION_STD_MASS;
                if (1.0f < massFactor) {
                    massFactor = 1.0f;
                }
                forceFactor = massFactor * forceFactor * visibleDistance;

                const CVector speed = veh->GetSpeed(colPointPos);
                // Como el original: el producto punto (y, z, x) se multiplica por la masa
                const float dot = ((colNormal.y * speed.y + colNormal.z * speed.z) + colNormal.x * speed.x) * veh->m_fMass;
                if (dot > forceFactor * 3.0f) {
                    const float reduced = forceFactor - dot;
                    forceFactor = reduced < 0.0f ? 0.0f : reduced;
                }

                colNormal.x *= forceFactor;
                colNormal.y *= forceFactor;
                colNormal.z *= forceFactor;

                if (!veh->physicalFlags.bDisableCollisionForce) {
                    veh->ApplyForce(colNormal, colPointPos, true); // Como el original: ApplyForce (0x542B50), no ApplyTurnForce
                }
            }

            if (veh->IsSubPlane()) {
                CVector normalBackwards{ -cp.m_vecNormal.x, -cp.m_vecNormal.y, -cp.m_vecNormal.z };
                CVector colPos{ colPointPos.x + veh->GetPosition().x, colPointPos.y + veh->GetPosition().y, colPointPos.z + veh->GetPosition().z };
                veh->VehicleDamage(1000.f, eVehicleCollisionComponent::DEFAULT, creator, &colPos, &normalBackwards, WEAPON_EXPLOSION);
            }

            break;
        }
        case ENTITY_TYPE_PED: {
            const auto ped = entity->AsPed();

            // Como el original: la dirección local se calcula con el impacto negado (x, y)
            const auto pedLocalDir = ped->GetLocalDirection(CVector2D{ -impactVelocity.x, -impactVelocity.y });
            const auto attachedTo = ped->m_pAttachedTo;
            if (attachedTo && attachedTo->GetIsTypeVehicle() && attachedTo->GetStatus() == STATUS_WRECKED) {
                CPedDamageResponseCalculator pedDamageResponseCalculator{ creator, 1000.f, WEAPON_EXPLOSION, PED_PIECE_TORSO, false };

                CEventDamage eventDamage{ creator, CTimer::GetTimeInMS(), WEAPON_EXPLOSION, PED_PIECE_TORSO, static_cast<uint8>(pedLocalDir), false, !!ped->bInVehicle };
                if (eventDamage.AffectsPed(ped)) {
                    pedDamageResponseCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);
                } else {
                    eventDamage.m_damageResponse.m_bDamageCalculated = true;
                }

                ped->GetIntelligence()->m_eventGroup.Add(&eventDamage, false);
            } else {
                if (!attachedTo) {
                    float ivf = impactVelocityFactor;
                    const float quarterMass = ped->m_fMass * 0.25f;
                    if (ivf > quarterMass) {
                        ivf = quarterMass;
                    }

                    // Como el original: producto punto (y, z, x) por la masa; el máximo con 0 es (0 > v) ? 0 : v
                    const float dot = ((impactVelocity.y * ped->m_vecMoveSpeed.y + impactVelocity.z * ped->m_vecMoveSpeed.z) + impactVelocity.x * ped->m_vecMoveSpeed.x) * ped->m_fMass;
                    if (dot > ivf + ivf) {
                        const float reduced = ivf - dot;
                        ivf = reduced < 0.0f ? 0.0f : reduced;
                    }

                    impactVelocity.x *= ivf;
                    impactVelocity.y *= ivf;
                    float z = ivf * impactVelocity.z;

                    if (ped->bIsStanding && ped->GetUsesCollision()) {
                        ped->bIsStanding = false;
                        z = z + 4.f;
                    } else {
                        z = CTimer::GetTimeStep() * ped->m_fMass * StaticRef<float>(0x863984) + z; // 0x863984 = 1/125; paso * masa, no GetTimeStepInMS
                    }
                    impactVelocity.z = z;

                    ped->ApplyMoveForce(impactVelocity);
                }

                CPedDamageResponseCalculator pedDamageResponseCalculator{ creator, entityRelDistToRadiusEnd_Doubled * 250.f, WEAPON_EXPLOSION, PED_PIECE_TORSO, false };

                CEventDamage eventDamage{ creator, CTimer::GetTimeInMS(), WEAPON_EXPLOSION, PED_PIECE_TORSO, static_cast<uint8>(pedLocalDir), false, !!ped->bInVehicle };
                if (eventDamage.AffectsPed(ped)) {
                    pedDamageResponseCalculator.ComputeDamageResponse(ped, eventDamage.m_damageResponse, true);
                } else {
                    eventDamage.m_damageResponse.m_bDamageCalculated = true;
                }

                ped->GetIntelligence()->m_eventGroup.Add(&eventDamage, false);
            }

            // Como el original: se informa del delito en los dos casos y el tipo de víctima (no el del creador) decide si es de policía
            if (creator && creator->GetIsTypePed()) {
                CCrime::ReportCrime(ped->m_nPedType == PED_TYPE_COP ? CRIME_DAMAGE_COP_CAR : CRIME_DAMAGE_CAR, ped, creator->AsPed());
            }
            break;
        }
        case ENTITY_TYPE_OBJECT: {
            if (!entity->physicalFlags.bDisableCollisionForce && !entity->physicalFlags.bDisableZ) {
                if (impactVelocity.z < 0.1f) {
                    impactVelocity.z = 0.2f;
                }

                // Como el original: producto punto (z, y, x) por la masa
                const float dot = ((impactVelocity.z * entity->m_vecMoveSpeed.z + impactVelocity.y * entity->m_vecMoveSpeed.y) + impactVelocity.x * entity->m_vecMoveSpeed.x) * entity->m_fMass;
                if (dot > impactVelocityFactor * 4.0f) {
                    const float reduced = impactVelocityFactor - dot;
                    impactVelocityFactor = reduced < 0.0f ? 0.0f : reduced;
                }
                impactVelocity.x *= impactVelocityFactor;
                impactVelocity.y *= impactVelocityFactor;
                impactVelocity.z *= impactVelocityFactor;
                entity->ApplyMoveForce(impactVelocity);

                const float turnMassRatio = entity->m_fTurnMass / entity->m_fMass;
                const float turnFactor = 1.0f < turnMassRatio ? 1.0f : turnMassRatio;
                impactVelocity.x *= turnFactor;
                impactVelocity.y *= turnFactor;
                impactVelocity.z *= turnFactor;
                entity->ApplyTurnForce(impactVelocity, { 0.f, 0.f, entity->GetModelInfo()->GetColModel()->m_boundSphere.m_fRadius * 0.5f });
            }

            entity->AsObject()->ObjectDamage(entityRelDistToRadiusEnd_Doubled * 300.f, nullptr, nullptr, creator, WEAPON_EXPLOSION);
            break;
        }
        }
    }
}

// 0x5684A0
namespace {
// Como el original (0x568952..0x568978): CRT pow del EXE (0x822130, precision completa) con base 0.707 (0x8CDEEC)
// y exponente ms_fTimeStep; cada componente se multiplica por el resultado y se redondea a float.
// Fuera de la lambda de Process porque MSVC no admite ensamblador en linea dentro de lambdas.
void __cdecl WorldDampMoveSpeedExe(float* moveSpeed) {
    __asm {
        mov eax, 0x8CDEEC
        fld dword ptr [eax]
        mov eax, 0xB7CB5C
        fld dword ptr [eax]
        mov eax, 0x822130
        call eax
        mov edx, moveSpeed
        fld st(0)
        fmul dword ptr [edx]
        fstp dword ptr [edx]
        fld st(0)
        fmul dword ptr [edx + 4]
        fstp dword ptr [edx + 4]
        fmul dword ptr [edx + 8]
        fstp dword ptr [edx + 8]
    }
}
} // namespace

void CWorld::Process() {
    ZoneScoped;

    const auto IterateMovingList = [&](auto&& fn) {
        for (auto* const entity : ms_listMovingEntityPtrs) {
            fn(entity);
        }
    };

    IterateMovingList([&](CPhysical* entity) {
        if (entity->GetIsTypePed()) {
            GetEventGlobalGroup()->AddEventsToPed(entity->AsPed());
        }
    });

    // Como el original (0x5684D1..0x5684FE): solo los grupos activos (CPedGroups::ms_activeGroups)
    for (auto i = 0u; i < CPedGroups::ms_groups.size(); i++) {
        if (CPedGroups::ms_activeGroups[i]) {
            GetEventGlobalGroup()->AddEventsToGroup(&CPedGroups::ms_groups[i]);
        }
    }

    CInformFriendsEventQueue::Process();
    CInformGroupEventQueue::Process();

    GetEventGlobalGroup()->Flush(false);

    if ((CTimer::m_FrameCounter % 64) == 0) {
        CReferences::PruneAllReferencesInWorld();
    }

    if (bProcessCutsceneOnly && CCutsceneMgr::ms_running) {
        for (auto* obj : CCutsceneMgr::ms_pCutsceneObjects) {
            if (obj && !obj->m_pCollisionList.IsEmpty()) {
                obj->UpdateAnim();
                obj->ProcessControl();
                obj->ProcessCollision();
                obj->UpdateRwMatrix();
                obj->UpdateRwFrame();
            }
        }
        return;
    }

    IterateMovingList([&](CEntity* entity) {
        if (!entity->m_bRemoveFromWorld) {
            entity->UpdateAnim();
        }
    });

    // Process moving entities (And possibly remove them from the world)
    {
        ZoneScopedN("Process moving entities");

        const auto DoProcessMovingEntity = [&](CEntity* entity) {
            if (entity->m_bRemoveFromWorld) {
                if (entity->GetIsTypePed()) {
                    if (FindPlayerPed() == entity) {
                        Remove(entity);
                    } else {
                        CPopulation::RemovePed(entity->AsPed());
                    }
                } else {
                    Remove(entity);
                    delete entity;
                }
            } else {
                entity->ProcessControl();
                if (entity->GetIsStatic()) {
                    entity->AsPhysical()->RemoveFromMovingList();
                }
            }
        };

        IterateMovingList(DoProcessMovingEntity);
        bForceProcessControl = true;
        IterateMovingList([&](CEntity* entity) {
            if (entity->GetWasPostponed()) {
                DoProcessMovingEntity(entity);
            }
        });
        bForceProcessControl = false;
    }

    for (auto* const obj : ms_listObjectsWithControlCode) {
        obj->ProcessControlLogic();
    }

    g_LoadMonitor.StartTimer(eLoadType::COLLISION);
    if (CReplay::Mode == MODE_PLAYBACK) {
        ZoneScopedN("Update entity RW");

        IterateMovingList([&](CEntity* entity) {
            entity->SetIsInSafePosition(true);
            entity->UpdateRwMatrix();
            entity->UpdateRwFrame();
        });
    } else {
        // Process collision
        {
            ZoneScopedN("Process collision");

            const auto ProcessMovingEntityCollision = [](CEntity* entity) {
                if (!entity->GetIsInSafePosition()) {
                    entity->ProcessCollision();
                    entity->UpdateRwMatrix();
                    entity->UpdateRwFrame();
                }
            };

            const auto ProcessMovingEntityCollisions = [&] {
                IterateMovingList(ProcessMovingEntityCollision);
            };

            bNoMoreCollisionTorque = false;
            ProcessMovingEntityCollisions();
            bNoMoreCollisionTorque = true;

            // Process entities a few more times..
            for (auto i = 0; i < 4; i++) {
                ProcessMovingEntityCollisions();
            }

            // Mark entities as `stuck` if they're still in unsafe positions
            IterateMovingList([&](CEntity* entity) {
                if (!entity->GetIsInSafePosition()) {
                    entity->SetIsStuck(true);

                    entity->ProcessCollision();
                    entity->UpdateRwMatrix();
                    entity->UpdateRwFrame();

                    if (!entity->GetIsInSafePosition()) {
                        entity->SetIsStuck(true);
                    }
                }
            });
        }

        // Process "Shift" (Not sure what that is)
        {
            ZoneScopedN("Process shift");

            bSecondShift = false;

            IterateMovingList([&](CEntity* entity) {
                if (!entity->GetIsInSafePosition()) {
                    entity->ProcessShift();
                    entity->UpdateRwMatrix();
                    entity->UpdateRwFrame();

                    if (!entity->GetIsInSafePosition()) {
                        entity->SetIsStuck(true);
                    }
                }
            });

            bSecondShift = true;

            IterateMovingList([&](CEntity* entity) {
                if (!entity->GetIsInSafePosition()) {
                    entity->ProcessShift();
                    entity->UpdateRwMatrix();
                    entity->UpdateRwFrame();

                    if (!entity->GetIsInSafePosition()) {
                        entity->SetIsStuck(true);

                        if (entity->TreatAsPlayerForCollisions()) {
                            NOTSA_LOG_DEBUG("STUCK: Final Step: Player Entity {} Is Stuck", entity->GetModelIndex()); // R* log
                            const auto physical = entity->AsPhysical();
                            // Como el original (0x568952..0x568978): CRT pow del EXE (0x822130, precision completa) con base
                            // 0.707 (0x8CDEEC) y como exponente ms_fTimeStep (no en milisegundos); cada componente se
                            // multiplica por el resultado y se redondea a float
                            WorldDampMoveSpeedExe(&physical->m_vecMoveSpeed.x);
                            physical->ApplyMoveSpeed();
                            physical->ApplyTurnSpeed();
                        }
                    }
                }
            });
        }
    }
    g_LoadMonitor.EndTimer(eLoadType::COLLISION);

    CVehicleRecording::SaveOrRetrieveDataForThisFrame();
    g_ikChainMan.Update(CTimer::ms_fTimeStep * 0.02f); // Como el original (0x5689E3): multiplica por 0.02 (0x858B38), no divide entre 50
    ProcessAttachedEntities();

    IterateMovingList([&](CEntity* entity) {
        if (entity->GetIsTypePed()) {
            const auto ped = entity->AsPed();
            ped->GetIntelligence()->ProcessAfterProcCol();
            if (const auto attachedTo = ped->m_pAttachedTo) {
                ped->m_pEntityIgnoredCollision = attachedTo;
                ped->PositionAttachedPed();
                ped->UpdateRwMatrix();
                ped->UpdateRwFrame();
            }
        }
    });

    CMessages::Process();

    for (auto&& [i, p] : rngv::enumerate(Players)) {
        if (p.m_pPed) {
            p.Process((uint32)i);
        }
    }

    CPedScriptedTaskRecord::Process();
    CPedGroups::Process();

    switch (CTimer::GetFrameCounter() % 8) {
    case 1: RemoveFallenPeds(); break;
    case 5: RemoveFallenCars(); break;
    }
}

// 0x568AD0
bool CWorld::GetIsLineOfSightSectorClear(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doIgnoreCameraCheck) {
    const auto ProcessSectorList = [&]<typename PtrListType>(PtrListType& list, bool doIgnoreCamCheckForThisSector) {
        return GetIsLineOfSightSectorListClear(list, colLine, doSeeThroughCheck, doIgnoreCamCheckForThisSector);
    };
    return (!buildings || ProcessSectorList(sector.Buildings, false))
        && (!vehicles  || ProcessSectorList(repeatSector.Vehicles, false))
        && (!peds      || ProcessSectorList(repeatSector.Peds, false))
        && (!objects   || ProcessSectorList(repeatSector.Objects, doIgnoreCameraCheck))
        && (!dummies   || ProcessSectorList(sector.Dummies, false));
}

// 0x568B80
void CWorld::FindObjectsKindaColliding(const CVector& point, float radius, bool b2D, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    const int32 startSectorX = WldASectorCoord(point.x - radius);
    const int32 startSectorY = WldASectorCoord(point.y - radius);
    const int32 endSectorX = WldASectorCoord(point.x + radius);
    const int32 endSectorY = WldASectorCoord(point.y + radius);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            WldAForEachList(sectorX, sectorY, buildings, vehicles, peds, objects, dummies, [&](auto& list) {
                FindObjectsKindaCollidingSectorList(list, point, radius, b2D, outCount, maxCount, outEntities);
            });
        }
    }
}

// 0x568DD0
void CWorld::FindObjectsIntersectingCube(const CVector& cornerA, const CVector& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    const int32 startSectorX = WldASectorCoord(cornerA.x);
    const int32 startSectorY = WldASectorCoord(cornerA.y);
    const int32 endSectorX = WldASectorCoord(cornerB.x);
    const int32 endSectorY = WldASectorCoord(cornerB.y);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            WldAForEachList(sectorX, sectorY, buildings, vehicles, peds, objects, dummies, [&](auto& list) {
                FindObjectsIntersectingCubeSectorList(list, cornerA, cornerB, outCount, maxCount, outEntities);
            });
        }
    }
}

// 0x568FF0
void CWorld::FindObjectsIntersectingAngledCollisionBox(const CBox& box, const CMatrix& transform, const CVector& point, float x1, float y1, float x2, float y2, int16* outCount, int16 maxCount, CEntity** outEntities, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    const int32 startSectorX = WldASectorCoord(x1);
    const int32 startSectorY = WldASectorCoord(y1);
    const int32 endSectorX = WldASectorCoord(x2);
    const int32 endSectorY = WldASectorCoord(y2);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            WldAForEachList(sectorX, sectorY, buildings, vehicles, peds, objects, dummies, [&](auto& list) {
                FindObjectsIntersectingAngledCollisionBoxSectorList(list, box, transform, point, outCount, maxCount, outEntities);
            });
        }
    }
}

// 0x569240
void CWorld::FindMissionEntitiesIntersectingCube(const CVector& cornerA, const CVector& cornerB, int16* outCount, int16 maxCount, CEntity** outEntities, bool vehicles, bool peds, bool objects) {
    const int32 startSectorX = WldASectorCoord(cornerA.x);
    const int32 startSectorY = WldASectorCoord(cornerA.y);
    const int32 endSectorX = WldASectorCoord(cornerB.x);
    const int32 endSectorY = WldASectorCoord(cornerB.y);

    AdvanceCurrentScanCode();

    *outCount = 0;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            auto& repeatSector = WldARepeatSector(sectorX, sectorY);
            if (vehicles) {
                FindMissionEntitiesIntersectingCubeSectorList(repeatSector.Vehicles, cornerA, cornerB, outCount, maxCount, outEntities, true, false, false);
            }
            if (peds) {
                FindMissionEntitiesIntersectingCubeSectorList(repeatSector.Peds, cornerA, cornerB, outCount, maxCount, outEntities, false, true, false);
            }
            if (objects) {
                FindMissionEntitiesIntersectingCubeSectorList(repeatSector.Objects, cornerA, cornerB, outCount, maxCount, outEntities, false, false, true);
            }
        }
    }
}

// 0x5693F0
CEntity* CWorld::FindNearestObjectOfType(int32 modelId, const CVector& point, float radius, bool b2D, bool buildings, bool vehicles, bool peds, bool objects, bool dummies) {
    const int32 startSectorX = WldASectorCoord(point.x - radius);
    const int32 startSectorY = WldASectorCoord(point.y - radius);
    const int32 endSectorX = WldASectorCoord(point.x + radius);
    const int32 endSectorY = WldASectorCoord(point.y + radius);

    AdvanceCurrentScanCode();

    CEntity* hitEntity = nullptr;
    // Como el original: la mejor distancia arranca en el radio y cada lista solo la mejora con dist < mejor
    float nearestDist = radius;
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            WldAForEachList(sectorX, sectorY, buildings, vehicles, peds, objects, dummies, [&](auto& list) {
                FindNearestObjectOfTypeSectorList(modelId, list, point, nearestDist, b2D, hitEntity, nearestDist);
            });
        }
    }
    return hitEntity;
}

// 0x569660
float CWorld::FindGroundZForCoord(float x, float y) {
    CEntity* hitEntity;
    CColPoint cp; // Como el original: sin inicializar
    return ProcessVerticalLine({ x, y, 1000.f }, -1000.f, cp, hitEntity, true, false, false, false, true, false, nullptr)
        ? cp.m_vecPoint.z
        : 20.f;
}

// 0x5696C0
float CWorld::FindGroundZFor3DCoord(CVector coord, bool* outResult, CEntity** outEntity) {
    CEntity*  localOutEntity;
    CColPoint colPoint; // Como el original: sin inicializar
    if (ProcessVerticalLine(coord, -1000.0f, colPoint, localOutEntity, true, false, false, false, false, false, nullptr)) {
        if (outResult) {
            *outResult = true;
        }
        if (outEntity) {
            *outEntity = localOutEntity;
        }
        return colPoint.m_vecPoint.z;
    } else {
        if (outResult) {
            *outResult = false;
        }
        if (outEntity) {
            *outEntity = nullptr;
        }
        return 0.0f;
    }
}

// 0x569750
float CWorld::FindRoofZFor3DCoord(float x, float y, float z, bool* outResult) {
    CEntity* outEntity;
    CColPoint colPoint; // Como el original: sin inicializar
    if (ProcessVerticalLine({ x, y, z }, 1000.0f, colPoint, outEntity, true, false, false, false, true, false, nullptr)) {
        if (outResult) {
            *outResult = true;
        }
        return colPoint.m_vecPoint.z;
    } else {
        if (outResult) {
            *outResult = false;
        } else {
            NOTSA_LOG_DEBUG("THERE IS NO MAP BELOW THE FOLLOWING COORS:{} {} {}. (FindGroundZFor3DCoord)", x, y, z); // R* triggered
        }
        return 20.0f;
    }
}

// 0x5697F0
float CWorld::FindLowestZForCoord(float x, float y) {
    CEntity* outEntity;
    CColPoint colPoint; // Como el original: sin inicializar
    if (ProcessVerticalLine({ x, y, -1000.0f }, 1000.0f, colPoint, outEntity, true, false, false, false, true, false, nullptr)) {
        return colPoint.m_vecPoint.z;
    }

    return 20.0f;
}

// 0x563690
void CWorld::CheckBlockListIntegrity() {
    // NOP
}

// 0x569850
void CWorld::RepositionOneObject(CEntity* object) {
    using namespace ModelIndices;

    // I mean, you can imagine this isn't original code, but we want to make it look readable :)
    // I mean, they probably used a macro or something... so, who knows :D
    // Maybe.. maybe they were time travellers and used c++20 ranges, and had a good compiler to mask it away
    // you never know..

    // Como el original (0x569850): el índice del objeto se lee con signo (movsx) y los de la tabla sin signo (movzx).
    const auto IsObjectModelAnyOf = [object](std::initializer_list<ModelIndex> models) {
        for (const auto& model : models) {
            if ((int32)(int16)object->m_nModelIndex == (int32)model.get_underlying()) {
                return true;
            }
        }
        return false;
    };

    const auto modelInfo = CModelInfo::GetModelInfo(object->m_nModelIndex);
    const auto colModel = modelInfo->GetColModel();

    // Recalculate position to be on ground level where is `point`
    // Como el original: no llama a FindGroundZFor3DCoord (que devuelve 20,0 sin suelo): hace ProcessVerticalLine con
    // solo edificios y usa 0,0 si no hay suelo; la altura de la caja es como mínimo 2,0.
    const auto RecalcZPosAtPoint = [&](float pointX, float pointY) {
        float height = colModel->m_boundBox.m_vecMax.z - colModel->m_boundBox.m_vecMin.z;
        if (2.0f > height) {
            height = 2.0f;
        }

        CColPoint colPoint;
        CEntity*  hitEntity;
        const CVector origin{ pointX, pointY, object->GetMatrix().GetPosition().z + height };
        const float   groundZ = ProcessVerticalLine(origin, -1000.0f, colPoint, hitEntity, true, false, false, false, false, false, nullptr)
            ? colPoint.m_vecPoint.z
            : 0.0f;

        object->GetMatrix().GetPosition().z = groundZ - colModel->m_boundBox.m_vecMin.z;
        object->UpdateRwMatrix();
        object->UpdateRwFrame();
    };

    if (modelInfo->SwaysInWind() || IsObjectModelAnyOf({
        MI_PARKINGMETER,
        MI_PHONEBOOTH1,
        MI_WASTEBIN,
        MI_BIN,
        MI_POSTBOX1,
        MI_NEWSSTAND,
        MI_TRAFFICCONE,
        MI_DUMP1,
        MI_ROADWORKBARRIER1,
        MI_ROADBLOCKFUCKEDCAR1,
        MI_ROADBLOCKFUCKEDCAR2,
        MI_BUSSIGN1,
        MI_NOPARKINGSIGN1,
        MI_PHONESIGN,
        MI_FIRE_HYDRANT,
        MI_BOLLARDLIGHT,
        MI_PARKTABLE,
        MI_PARKINGMETER2,
        MI_TELPOLE02,
        MI_PARKBENCH,
        MI_BARRIER1
    })) {
        const CVector position = object->GetMatrix().GetPosition();
        RecalcZPosAtPoint(position.x, position.y);
    } else if (IsObjectModelAnyOf({
        MI_SINGLESTREETLIGHTS1,
        MI_SINGLESTREETLIGHTS2,
        MI_SINGLESTREETLIGHTS3,
        MI_TRAFFICLIGHTS_MIAMI,
        MI_TRAFFICLIGHTS_VEGAS,
        MI_TRAFFICLIGHTS_TWOVERTICAL,
        MI_TRAFFICLIGHTS_3,
        MI_TRAFFICLIGHTS_4,
        MI_TRAFFICLIGHTS_5,
        MI_MLAMPPOST,
        MI_STREETLAMP1,
        MI_STREETLAMP2
    })) {
        // Como el original: sin datos de colisión no se recoloca nada; con una caja se usa su centro (x, y) y su z mínima;
        // con esferas, la de menor z (por debajo de 1000); sin cajas ni esferas, la posición del objeto. El punto se lleva
        // al mundo con MultiplyMatrixWithVector (0x59C890, con traslación).
        if (const auto CD = colModel->m_pColData) {
            CVector point = object->GetPosition();

            if (CD->m_nNumBoxes == 1) {
                const auto& box = CD->m_pBoxes[0];
                point.x = (box.m_vecMax.x + box.m_vecMin.x) * 0.5f;
                point.y = (box.m_vecMax.y + box.m_vecMin.y) * 0.5f;
                point.z = box.m_vecMin.z;
                point = WldEMatrixTimesPoint(object->GetMatrix(), point);
            } else if ((int16)CD->m_nNumSpheres > 0) {
                point.z = 1000.f;

                for (auto i = 0; i < (int16)CD->m_nNumSpheres; i++) {
                    const auto& sphere = CD->m_pSpheres[i];
                    if (sphere.m_vecCenter.z < point.z) {
                        point = sphere.m_vecCenter;
                    }
                }

                if (point.z < 1000.f) {
                    point = WldEMatrixTimesPoint(object->GetMatrix(), point);
                }
            }

            RecalcZPosAtPoint(point.x, point.y);
        }
    }

    // Como el original: la boya sí hace el ProcessVerticalLine (con efectos sobre el scan code y los cruces), ignora el
    // resultado y coloca la z con las constantes del EXE: (6,0 - alto * 0,5) + alto * 0,2f (0x858B44, 0x858B8C, 0x858CC4).
    if ((int32)(int16)object->m_nModelIndex == (int32)MI_BUOY.get_underlying()) {
        auto& matrix = object->GetMatrix();

        CColPoint colPoint;
        CEntity*  hitEntity;
        const CVector origin{ matrix.GetPosition().x, matrix.GetPosition().y, matrix.GetPosition().z + 2.0f };
        ProcessVerticalLine(origin, -1000.0f, colPoint, hitEntity, true, false, false, false, false, false, nullptr);

        const float height = colModel->m_boundBox.m_vecMax.z - colModel->m_boundBox.m_vecMin.z;
        object->GetMatrix().GetPosition().z = (6.0f - 0.5f * height) + height * 0.2f;
    }
}

// 0x569E20
CEntity* CWorld::TestSphereAgainstWorld(CVector sphereCenter, float sphereRadius, CEntity* ignoreEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doCameraIgnoreCheck) {
    // Como el original (0x569E20): sector = ftol(floor(coord * 0.02f + 60.0f)) con x87 a 24 bits y sector repetido con "& 0xF"
    const int32 startSectorX = WorldSectorCoordExe(sphereCenter.x - sphereRadius);
    const int32 startSectorY = WorldSectorCoordExe(sphereCenter.y - sphereRadius);
    const int32 endSectorX = WorldSectorCoordExe(sphereCenter.x + sphereRadius);
    const int32 endSectorY = WorldSectorCoordExe(sphereCenter.y + sphereRadius);

    AdvanceCurrentScanCode();

    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            const auto ProcessSector = [&]<typename PtrListType>(PtrListType& list, bool doIgnoreCameraCheckForThisSector) {
                return TestSphereAgainstSectorList(list, sphereCenter, sphereRadius, ignoreEntity, doIgnoreCameraCheckForThisSector);
            };

            auto& sector = WorldSectorExe(sectorX, sectorY);
            auto& repeatSector = WorldRepeatSectorExe(sectorX, sectorY);

            CEntity* hitEntity{};

            if (buildings && (hitEntity = ProcessSector(sector.Buildings, false))) {
                return hitEntity;
            }

            if (vehicles && (hitEntity = ProcessSector(repeatSector.Vehicles, false))) {
                return hitEntity;
            }

            if (peds && (hitEntity = ProcessSector(repeatSector.Peds, false))) {
                return hitEntity;
            }

            if (objects && (hitEntity = ProcessSector(repeatSector.Objects, doCameraIgnoreCheck))) {
                return hitEntity;
            }

            if (dummies && (hitEntity = ProcessSector(sector.Dummies, false))) {
                return hitEntity;
            }
        }
    }
    return nullptr;
}

// 0x56A0D0
// Remove all peds/vehicles not related to the p's group in the area + projectiles, explosions, pickups, etc..
void CWorld::ClearExcitingStuffFromArea(const CVector& point, float radius, uint8 bRemoveProjectilesAndShadows) {
    // Remove all peds in radius who aren't followers of the p's group
    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (ped.IsPlayer() || !ped.CanBeDeleted()) {
            continue;
        }

        // Como el original (0x56A142..0x56A17F): distancia 2D al cuadrado (dx^2 + dy^2), estricta contra radio^2 (con NaN no pasa)
        const auto& pedPos = ped.GetPosition();
        const float dy     = pedPos.y - point.y;
        const float dx     = pedPos.x - point.x;
        if (!(dx * dx + dy * dy < radius * radius)) {
            continue;
        }

        // Como el original: el grupo del jugador se busca por cada candidato
        const auto playerGroup = CPedGroups::GetPedsGroup(FindPlayerPed());
        if (!playerGroup || !ped.IsFollowerOfGroup(*playerGroup)) {
            CPopulation::RemovePed(&ped);
        }
    }

    // Remove all vehicles in radius in which there are no peds who're follower's of the p's group
    for (auto& veh : GetVehiclePool()->GetAllValid()) {
        // Como el original: FindPlayerPed y el grupo se consultan en cada vehiculo, antes que nada
        const auto playerGroup = CPedGroups::GetPedsGroup(FindPlayerPed());
        if (playerGroup && veh.AreAnyOfPassengersFollowerOfGroup(*playerGroup)) {
            continue;
        }

        if (FindPlayerPed()->m_pContactEntity == &veh && veh.IsBoat()) {
            continue;
        }

        const auto& vehPos = veh.GetPosition();
        const float dy     = vehPos.y - point.y;
        const float dx     = vehPos.x - point.x;
        if (!(dx * dx + dy * dy < radius * radius)) {
            continue;
        }

        if (veh.vehicleFlags.bIsLocked || !veh.CanBeDeleted()) {
            continue;
        }

        if (CGarages::IsPointWithinHideOutGarage(veh.GetPosition())) {
            continue;
        }

        RemoveVehicleAndItsOccupants(&veh);
    }

    CObject::DeleteAllTempObjectsInArea(point, radius);
    gFireManager.ExtinguishPoint(point, radius);
    ExtinguishAllCarFiresInArea(point, radius);
    CExplosion::RemoveAllExplosionsInArea(point, radius);
    if (bRemoveProjectilesAndShadows) {
        CProjectileInfo::RemoveAllProjectiles();
        CShadows::TidyUpShadows();
    }
    CPickups::RemoveUnnecessaryPickups(point, radius);
}

// 0x56A490
bool CWorld::GetIsLineOfSightClear(const CVector& origin, const CVector& target, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck) {
    AdvanceCurrentScanCode();

    const int32 originSectorX = WorldSectorCoordExe(origin.x);
    const int32 originSectorY = WorldSectorCoordExe(origin.y);
    const int32 targetSectorX = WorldSectorCoordExe(target.x);
    const int32 targetSectorY = WorldSectorCoordExe(target.y);

    const auto ProcessSector = [&](int32 x, int32 y) {
        return GetIsLineOfSightSectorClear(
            WorldSectorExe(x, y),
            WorldRepeatSectorExe(x, y),
            { origin, target },
            buildings,
            vehicles,
            peds,
            objects,
            dummies,
            doSeeThroughCheck,
            doCameraIgnoreCheck
        );
    };

    // Como el original: recorre la columna x de 'from' a 'to' (ascendente si from < to; si no descendente, siempre procesa al menos 'from')
    const auto ProcessColumn = [&](int32 x, int32 from, int32 to) {
        if (from < to) {
            for (int32 y = from; y <= to; y++) {
                if (!ProcessSector(x, y)) {
                    return false;
                }
            }
        } else {
            for (int32 y = from; y >= to; y--) {
                if (!ProcessSector(x, y)) {
                    return false;
                }
            }
        }
        return true;
    };

    if (originSectorX == targetSectorX) {
        if (originSectorY == targetSectorY) { // Mismo sector
            return ProcessSector(originSectorX, originSectorY);
        }
        // Misma X: de origen a destino en Y
        return ProcessColumn(originSectorX, originSectorY, targetSectorY);
    }

    if (originSectorY == targetSectorY) { // Misma Y: de origen a destino en X
        if (originSectorX >= targetSectorX) {
            for (int32 x = originSectorX; x >= targetSectorX; x--) {
                if (!ProcessSector(x, originSectorY)) {
                    return false;
                }
            }
        } else {
            for (int32 x = originSectorX; x <= targetSectorX; x++) {
                if (!ProcessSector(x, originSectorY)) {
                    return false;
                }
            }
        }
        return true;
    }

    // Como el original: pendiente en float (x87 a 24 bits); la rama es "destino.x > origen.x" (NaN -> derecha a izquierda)
    const float slope = (target.y - origin.y) / (target.x - origin.x);

    // Como el original: sector Y donde la recta cruza el borde X (en unidades de mundo) k * 50
    const auto CrossingSectorY = [&](int32 k) {
        const float edgeX  = (float)k * 50.0f;
        const float yAtEdge = (edgeX - origin.x) * slope + origin.y;
        return WorldSectorCoordExe(yAtEdge);
    };

    if (!(target.x <= origin.x)) { // De izquierda a derecha: borde derecho del sector (x - 59) * 50
        int32 endY = CrossingSectorY(originSectorX - 59);
        if (!ProcessColumn(originSectorX, originSectorY, endY)) {
            return false;
        }
        for (int32 x = originSectorX + 1; x < targetSectorX; x++) {
            const int32 startY = endY;
            endY = CrossingSectorY(x - 59);
            if (!ProcessColumn(x, startY, endY)) {
                return false;
            }
        }
        return ProcessColumn(targetSectorX, endY, targetSectorY);
    } else { // De derecha a izquierda: borde izquierdo del sector (x - 60) * 50
        int32 endY = CrossingSectorY(originSectorX - 60);
        if (!ProcessColumn(originSectorX, originSectorY, endY)) {
            return false;
        }
        for (int32 x = originSectorX - 1; x > targetSectorX; x--) {
            const int32 startY = endY;
            endY = CrossingSectorY(x - 60);
            if (!ProcessColumn(x, startY, endY)) {
                return false;
            }
        }
        return ProcessColumn(targetSectorX, endY, targetSectorY);
    }
}

// 0x56B5E0
bool CWorld::ProcessLineOfSightSector(CSector& sector, CRepeatSector& repeatSector, const CColLine& colLine, CColPoint& outColPoint, float& maxTouchDistance, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
    float localMaxTouchDist = maxTouchDistance;

    const auto bIncludeDeadPeds_Original = bIncludeDeadPeds;
    const auto bIncludeBikers_Original = bIncludeBikers;
    const auto fWeaponSpreadRate_Original = fWeaponSpreadRate;

    bIncludeDeadPeds = false;
    bIncludeBikers = false;
    fWeaponSpreadRate = 0.f;

    const auto ProcessSector = [&]<typename PtrListType>(PtrListType& list, bool doIgnoreCameraCheckForThisSector = false) {
        ProcessLineOfSightSectorList(list, colLine, outColPoint, localMaxTouchDist, outEntity, doSeeThroughCheck, doIgnoreCameraCheckForThisSector, doShootThroughCheck);
    };

    if (buildings) {
        ProcessSector(sector.Buildings);
    }

    if (fWeaponSpreadRate_Original > 0.f) {
        fWeaponSpreadRate = fWeaponSpreadRate_Original;
    }

    if (vehicles) {
        ProcessSector(repeatSector.Vehicles);
    }

    if (peds) {
        if (bIncludeDeadPeds_Original) {
            bIncludeDeadPeds = bIncludeDeadPeds_Original;
        }

        if (bIncludeBikers_Original) {
            bIncludeBikers = bIncludeBikers_Original;
        }

        ProcessSector(repeatSector.Peds);

        bIncludeDeadPeds = false;
        bIncludeBikers = false;
    }

    if (objects) {
        ProcessSector(repeatSector.Objects, doCameraIgnoreCheck); // Como el original: solo los objetos usan la comprobacion de camara
    }

    if (dummies) {
        ProcessSector(sector.Dummies);
    }

    bIncludeDeadPeds = bIncludeDeadPeds_Original;
    bIncludeBikers = bIncludeBikers_Original;

    if (localMaxTouchDist < maxTouchDistance) {
        maxTouchDistance = localMaxTouchDist;
        return true;
    }
    return false;
}

// 0x56B790
void CWorld::TriggerExplosion(const CVector& point, float radius, float visibleDistance, CEntity* victim, CEntity* creator, bool processVehicleBombTimer, float damage) {
    // Como el original (0x56B790): sector = ftol(floor(coord * 0.02f + 60.0f)), sector repetido con "& 0xF" y sin avanzar
    // el scan code (las listas se recorren sin comprobarlo)
    const int32 startSectorX = WorldSectorCoordExe(point.x - radius);
    const int32 startSectorY = WorldSectorCoordExe(point.y - radius);
    const int32 endSectorX = WorldSectorCoordExe(point.x + radius);
    const int32 endSectorY = WorldSectorCoordExe(point.y + radius);

    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            const auto ProcessSector = [&](auto& sector) {
                TriggerExplosionSectorList(sector, point, radius, visibleDistance, victim, creator, processVehicleBombTimer, damage);
            };

            auto& sector = WorldRepeatSectorExe(sectorX, sectorY);
            ProcessSector(sector.Vehicles);
            ProcessSector(sector.Peds);
            ProcessSector(sector.Objects);
        }
    }
}

// 0x56B910
void CWorld::SetWorldOnFire(CVector pos, float radius, CEntity* fireCreator) {
    if (TestSphereAgainstWorld(pos, radius, nullptr, true, false, false, true, false, false)) {
        if (!gFireManager.GetNumFiresInRange(pos, 2.f)) {
            gFireManager.StartFire(pos, 0.8f, 1u, nullptr, 7'000u, 100u, 1u);
        }
    }
}

// 0x56B9C0
void CWorld::RepositionCertainDynamicObjects() {
    for (int32 i = GetDummyPool()->GetSize(); i; i--) {
        if (CDummy* dummy = GetDummyPool()->GetAt(i - 1)) {
            RepositionOneObject(dummy);
        }
    }
}

// 0x56BA00
bool CWorld::ProcessLineOfSight(const CVector& origin, const CVector& target, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck) {
    AdvanceCurrentScanCode();
    ms_iProcessLineNumCrossings = 0;

    outEntity = nullptr;

    const int32 originSectorX = WorldSectorCoordExe(origin.x);
    const int32 originSectorY = WorldSectorCoordExe(origin.y);
    const int32 targetSectorX = WorldSectorCoordExe(target.x);
    const int32 targetSectorY = WorldSectorCoordExe(target.y);

    float touchDist = 1.f;
    const auto ProcessSector = [&](int32 x, int32 y) {
        return ProcessLineOfSightSector(
            WorldSectorExe(x, y),
            WorldRepeatSectorExe(x, y),
            { origin, target },
            outColPoint,
            touchDist,
            outEntity,
            buildings,
            vehicles,
            peds,
            objects,
            dummies,
            doSeeThroughCheck,
            doCameraIgnoreCheck,
            doShootThroughCheck
        );
    };

    // Como el original: recorre la columna x de 'from' a 'to' (ascendente si from < to; si no descendente, siempre procesa al menos 'from')
    const auto ProcessColumn = [&](int32 x, int32 from, int32 to) {
        if (from < to) {
            for (int32 y = from; y <= to; y++) {
                ProcessSector(x, y);
            }
        } else {
            for (int32 y = from; y >= to; y--) {
                ProcessSector(x, y);
            }
        }
    };

    if (originSectorX == targetSectorX) {
        if (originSectorY == targetSectorY) { // Mismo sector
            return ProcessSector(originSectorX, originSectorY); // Como el original: devuelve el resultado de la funcion de sector
        } else { // Misma X: de origen a destino en Y
            ProcessColumn(originSectorX, originSectorY, targetSectorY);
        }
    } else if (originSectorY == targetSectorY) { // Misma Y: de origen a destino en X
        if (originSectorX >= targetSectorX) {
            for (int32 x = originSectorX; x >= targetSectorX; x--) {
                ProcessSector(x, originSectorY);
            }
        } else {
            for (int32 x = originSectorX; x <= targetSectorX; x++) {
                ProcessSector(x, originSectorY);
            }
        }
    } else {
        // Como el original: pendiente en float (x87 a 24 bits); la rama es "destino.x > origen.x" (NaN -> derecha a izquierda)
        const float slope = (target.y - origin.y) / (target.x - origin.x);

        // Como el original: sector Y donde la recta cruza el borde X (en unidades de mundo) k * 50
        const auto CrossingSectorY = [&](int32 k) {
            const float edgeX   = (float)k * 50.0f;
            const float yAtEdge = (edgeX - origin.x) * slope + origin.y;
            return WorldSectorCoordExe(yAtEdge);
        };

        if (!(target.x <= origin.x)) { // De izquierda a derecha: borde derecho del sector (x - 59) * 50
            int32 endY = CrossingSectorY(originSectorX - 59);
            ProcessColumn(originSectorX, originSectorY, endY);
            for (int32 x = originSectorX + 1; x < targetSectorX; x++) {
                const int32 startY = endY;
                endY = CrossingSectorY(x - 59);
                ProcessColumn(x, startY, endY);
            }
            ProcessColumn(targetSectorX, endY, targetSectorY);
        } else { // De derecha a izquierda: borde izquierdo del sector (x - 60) * 50
            int32 endY = CrossingSectorY(originSectorX - 60);
            ProcessColumn(originSectorX, originSectorY, endY);
            for (int32 x = originSectorX - 1; x > targetSectorX; x--) {
                const int32 startY = endY;
                endY = CrossingSectorY(x - 60);
                ProcessColumn(x, startY, endY);
            }
            ProcessColumn(targetSectorX, endY, targetSectorY);
        }
    }

    return touchDist < 1.f;
}

// NOTSA
// Remove a vehicle from the world, along with all of it's occupants.
void CWorld::RemoveVehicleAndItsOccupants(CVehicle* veh) {
    if (const auto driver = veh->m_pDriver) {
        CPopulation::RemovePed(driver);
        // Como el original (0x566741..0x566750): si tras borrar al conductor la referencia sigue puesta se limpia con
        // CleanUpOldReference (this = lo que haya en m_pDriver) y despues se pone a nulo
        if (veh->m_pDriver) {
            veh->m_pDriver->CleanUpOldReference(reinterpret_cast<CEntity**>(&veh->m_pDriver));
        }
        veh->m_pDriver = nullptr;
    }

    for (const auto passenger : veh->GetPassengers()) {
        if (passenger) {
            veh->RemovePassenger(passenger);
            CPopulation::RemovePed(passenger);
        }
    }

    if (CCarCtrl::IsThisVehicleInteresting(veh)) {
        CGarages::StoreCarInNearestImpoundingGarage(veh);
    }

    CCarCtrl::RemoveFromInterestingVehicleList(veh);

    Remove(veh);
    delete veh;
}
