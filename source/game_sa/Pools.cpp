/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPools
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 19/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include <Pools/Pools.h>
#include "CarCtrl.h"
#include "ObjectSaveStructure.h"
#include "ProjectileInfo.h"

#include <Pools/IplDefPool.h>
#include <Pools/PedPool.h>
#include <Pools/VehiclePool.h>
#include <Pools/BuildingPool.h>
#include <Pools/ObjectPool.h>
#include <Pools/DummyPool.h>
#include <Pools/ColModelPool.h>
#include <Pools/TaskPool.h>
#include <Pools/PedIntelligencePool.h>
#include <Pools/PtrNodeSingleLinkPool.h>
#include <Pools/PtrNodeDoubleLinkPool.h>
#include <Pools/EntryInfoNodePool.h>
#include <Pools/PointRoutePool.h>
#include <Pools/PatrolRoutePool.h>
#include <Pools/EventPool.h>
#include <Pools/NodeRoutePool.h>
#include <Pools/TaskAllocatorPool.h>
#include <Pools/PedAttractorPool.h>


// 0x4442D0
// Como el original: indice = (puntero - almacenamiento) / 0x7C4 con division con signo (multiplicacion por 0x41EE7CA7, sar 9 y
// correccion del signo); resultado = (indice << 8) + banderas[indice]. Deja ECX = tabla de banderas y EDX = indice << 8.
#pragma runtime_checks("", off)
uint32 CPedPool::GetRef(const CPed* ped) {
    const uintptr_t storage = *reinterpret_cast<const uintptr_t*>(this);          // primer miembro del pool
    const uintptr_t flags   = *(reinterpret_cast<const uintptr_t*>(this) + 1);    // segundo: tabla de banderas
    const int32     diff    = static_cast<int32>(reinterpret_cast<uintptr_t>(ped) - storage);
    const int32     high    = static_cast<int32>((static_cast<int64>(0x41EE7CA7) * diff) >> 32);
    const int32     shifted = high >> 9;
    const int32     index   = shifted + static_cast<int32>(static_cast<uint32>(shifted) >> 31);
    const uint32    byteFlag = *reinterpret_cast<const uint8*>(flags + static_cast<uintptr_t>(static_cast<uint32>(index)));
    const uint32    shiftedIndex = static_cast<uint32>(index) << 8;
    return ResidueRegsCpp(static_cast<uint32>(flags), shiftedIndex, byteFlag + shiftedIndex);
}
#pragma runtime_checks("", restore)

auto& ms_pPedPool               = StaticRef<CPedPool*>(0xB74490);
auto& ms_pVehiclePool           = StaticRef<CVehiclePool*>(0xB74494);
auto& ms_pBuildingPool          = StaticRef<CBuildingPool*>(0xB74498);
auto& ms_pObjectPool            = StaticRef<CObjectPool*>(0xB7449C);
auto& ms_pDummyPool             = StaticRef<CDummyPool*>(0xB744A0);
auto& ms_pColModelPool          = StaticRef<CColModelPool*>(0xB744A4);
auto& ms_pTaskPool              = StaticRef<CTaskPool*>(0xB744A8);
auto& ms_pPedIntelligencePool   = StaticRef<CPedIntelligencePool*>(0xB744C0);
auto& ms_pPtrNodeSingleLinkPool = StaticRef<CPtrNodeSingleLinkPool*>(0xB74484);
auto& ms_pPtrNodeDoubleLinkPool = StaticRef<CPtrNodeDoubleLinkPool*>(0xB74488);
auto& ms_pEntryInfoNodePool     = StaticRef<CEntryInfoNodePool*>(0xB7448C);
auto& ms_pPointRoutePool        = StaticRef<CPointRoutePool*>(0xB744B0);
auto& ms_pPatrolRoutePool       = StaticRef<CPatrolRoutePool*>(0xB744B4);
auto& ms_pEventPool             = StaticRef<CEventPool*>(0xB744AC);
auto& ms_pNodeRoutePool         = StaticRef<CNodeRoutePool*>(0xB744B8);
auto& ms_pTaskAllocatorPool     = StaticRef<CTaskAllocatorPool*>(0xB744BC);
auto& ms_pPedAttractorPool      = StaticRef<CPedAttractorPool*>(0xB744C4);

void CPools::InjectHooks() {
    RH_ScopedClass(CPools);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x550F10);
    RH_ScopedInstall(ShutDown, 0x5519F0);
    RH_ScopedInstall(CheckBuildingAtomics, 0x550170);
    RH_ScopedInstall(CheckPoolsEmpty, 0x551950);
    RH_ScopedInstall(GetObject, 0x550050);
    RH_ScopedInstall(GetObjectRef, 0x550020);
    RH_ScopedInstall(GetPed, 0x54FF90);
    RH_ScopedInstall(GetPedRef, 0x54FF60);
    RH_ScopedInstall(GetVehicle, 0x54FFF0);
    RH_ScopedInstall(GetVehicleRef, 0x54FFC0);
    RH_ScopedInstall(Load, 0x5D0890);
    RH_ScopedInstall(LoadObjectPool, 0x5D4A40);
    RH_ScopedInstall(LoadPedPool, 0x5D2D70);
    RH_ScopedInstall(LoadVehiclePool, 0x5D2A20);
    RH_ScopedInstall(MakeSureSlotInObjectPoolIsEmpty, 0x550080);
    RH_ScopedInstall(Save, 0x5D0880);
    RH_ScopedInstall(SaveObjectPool, 0x5D4940);
    RH_ScopedInstall(SavePedPool, 0x5D4B40);
    RH_ScopedInstall(SaveVehiclePool, 0x5D4800);
}

namespace {
// Tamanos de elemento de cada pool tomados del EXE (imul/shl del constructor de CPool de cada tipo: 0x550180..0x550E40).
// Las comprobaciones solo se hacen sobre los tipos cuyo tamano ya valida su propia cabecera con VALIDATE_SIZE.
static_assert(sizeof(CCopPed) == 0x7C4);                 // 0x5503F8: imul eax,eax,7C4h
static_assert(sizeof(CHeli) == 0xA18);                   // 0x5504C8: imul eax,eax,0A18h
static_assert(sizeof(CBuilding) == 0x38);                // 0x550578: imul eax,eax,38h
static_assert(sizeof(CCutsceneObject) == 0x19C);         // 0x550648: imul eax,eax,19Ch
static_assert(sizeof(CDummy) == 0x38);                   // 0x5506F8: imul eax,eax,38h
static_assert(sizeof(CColModel) == 0x30);                // 0x5507C9: lea eax,[edi+edi*2] / shl eax,4
static_assert(sizeof(CTaskSimpleSlideToCoord) == 0x80);  // 0x550898: shl eax,7
static_assert(sizeof(CEventDamage) == 0x44);             // 0x550968: imul eax,eax,44h
static_assert(sizeof(CPointRoute) == 0x64);              // 0x550A38: imul eax,eax,64h
static_assert(sizeof(CNodeRoute) == 0x24);               // 0x550BD6: lea eax,[edi+edi*8] / shl eax,2
static_assert(sizeof(CPedIntelligence) == 0x294);        // 0x550D78: imul eax,eax,294h
static_assert(sizeof(CEntryInfoNode) == 0x14);           // 0x550326: lea eax,[edi+edi*4] / shl eax,2
static_assert(sizeof(CPedAttractorPoolSlot) == 0xC4);    // 0x550E48: imul eax,eax,0C4h

// Como el original: constructor de CPool del EXE (0x550180 y sus gemelos 0x550250..0x550E40, uno por tipo de pool).
// Reserva el objeto del pool (operator new de 0x14 bytes, 0x82119A), el almacenamiento (n * tamano) y el vector de
// estados (n bytes) con operator new[] (0x821195), en ese orden, y escribe SOLO +0 almacenamiento, +4 estados, +8 tamano,
// +0xC ultima ranura libre (-1), +0x10 propietario (1) y 0x80 en cada estado (ranura libre, ID 0). No rellena el
// almacenamiento (el CPool de la DLL lo rellena con 0xFD) ni toca +0x11..+0x13 (basura de operator new). El nombre del pool
// que reciben los constructores no se guarda en ningun sitio. Si operator new devuelve null el EXE no construye el pool
// (0x550F3E: je) y deja el puntero global a 0.
template<class PoolT>
PoolT* ConstructPool(int32 capacity, uint32 elementSize) {
    auto* const pool = static_cast<uint8*>(::operator new(0x14));
    if (!pool) {
        return nullptr;
    }
    auto* const storage = static_cast<uint8*>(::operator new[](size_t(capacity) * elementSize));
    auto* const states  = static_cast<uint8*>(::operator new[](size_t(capacity)));
    *reinterpret_cast<uint8**>(pool + 0x0) = storage;
    *reinterpret_cast<uint8**>(pool + 0x4) = states;
    *reinterpret_cast<int32*>(pool + 0x8)  = capacity;
    *reinterpret_cast<int32*>(pool + 0xC)  = -1;
    *(pool + 0x10)                         = 1;
    for (int32 i = 0; i < capacity; ++i) {
        states[i] = 0x80;
    }
    return reinterpret_cast<PoolT*>(pool);
}

// Como el original: destructor de CPool + operator delete del EXE (p. ej. 0x551A07..0x551A40, repetido para cada pool).
// Si el puntero es null no hace nada; si el tamano (con signo) es > 0: libera con delete[] (0x8213AE) el almacenamiento
// y luego los estados SOLO si el byte +0x10 (propietario) no es cero, y pone a 0 +0, +4, +8 y +0xC; en todos los casos
// termina con operator delete escalar (0x8214BD) sobre el objeto. El puntero global NO se pone a cero.
void DestroyPool(void* poolPtr) {
    auto* const pool = static_cast<uint8*>(poolPtr);
    if (!pool) {
        return;
    }
    if (*reinterpret_cast<int32*>(pool + 0x8) > 0) {
        if (*(pool + 0x10) != 0) {
            ::operator delete[](*reinterpret_cast<void**>(pool + 0x0));
            ::operator delete[](*reinterpret_cast<void**>(pool + 0x4));
        }
        *reinterpret_cast<void**>(pool + 0x0) = nullptr;
        *reinterpret_cast<void**>(pool + 0x4) = nullptr;
        *reinterpret_cast<int32*>(pool + 0x8) = 0;
        *reinterpret_cast<int32*>(pool + 0xC) = 0;
    }
    ::operator delete(pool);
}
} // namespace

// 0x550F10
void CPools::Initialise() {
    ZoneScoped;

    // Como el original (0x550F10..0x5512D7): 17 pools en este orden. Los tamanos de elemento son los del EXE; el pool de
    // rutas de patrulla usa 0x1A4 bytes por elemento (0x550B08: imul eax,eax,1A4h), no el void* de CPatrolRoutePool.
    ms_pPtrNodeSingleLinkPool = ConstructPool<CPtrNodeSingleLinkPool>(70000, 8);    // 0x550186: lea eax,[edi*8+0]
    ms_pPtrNodeDoubleLinkPool = ConstructPool<CPtrNodeDoubleLinkPool>(3200, 12);    // 0x550256: lea eax,[edi+edi*2] / shl eax,2
    ms_pEntryInfoNodePool     = ConstructPool<CEntryInfoNodePool>(500, 0x14);
    ms_pPedPool               = ConstructPool<CPedPool>(140, 0x7C4);
    ms_pVehiclePool           = ConstructPool<CVehiclePool>(110, 0xA18);
    ms_pBuildingPool          = ConstructPool<CBuildingPool>(13000, 0x38);
    ms_pObjectPool            = ConstructPool<CObjectPool>(350, 0x19C);
    ms_pDummyPool             = ConstructPool<CDummyPool>(2500, 0x38);
    ms_pColModelPool          = ConstructPool<CColModelPool>(10150, 0x30);
    ms_pTaskPool              = ConstructPool<CTaskPool>(500, 0x80);
    ms_pEventPool             = ConstructPool<CEventPool>(200, 0x44);
    ms_pPointRoutePool        = ConstructPool<CPointRoutePool>(64, 0x64);
    ms_pPatrolRoutePool       = ConstructPool<CPatrolRoutePool>(32, 0x1A4);
    ms_pNodeRoutePool         = ConstructPool<CNodeRoutePool>(64, 0x24);
    ms_pTaskAllocatorPool     = ConstructPool<CTaskAllocatorPool>(16, 0x20);
    ms_pPedIntelligencePool   = ConstructPool<CPedIntelligencePool>(140, 0x294);
    ms_pPedAttractorPool      = ConstructPool<CPedAttractorPool>(64, 0xC4);
}

// 0x5519F0
void CPools::ShutDown() {
    // Como el original (0x5519F3..0x551E58): printf del CRT del EXE (0x821982) al empezar y al terminar; libera cada pool
    // con DestroyPool y NO pone a cero el puntero global (queda colgante hasta Initialise).
    const auto exePrintf = reinterpret_cast<int(__cdecl*)(const char*, ...)>(0x821982);
    exePrintf("Shutdown pool started\n");
    DestroyPool(ms_pPtrNodeSingleLinkPool);
    DestroyPool(ms_pPtrNodeDoubleLinkPool);
    DestroyPool(ms_pEntryInfoNodePool);
    DestroyPool(ms_pPedPool);
    DestroyPool(ms_pVehiclePool);
    DestroyPool(ms_pBuildingPool);
    DestroyPool(ms_pObjectPool);
    DestroyPool(ms_pDummyPool);
    DestroyPool(ms_pColModelPool);
    DestroyPool(ms_pTaskPool);
    DestroyPool(ms_pEventPool);
    DestroyPool(ms_pPointRoutePool);
    DestroyPool(ms_pPatrolRoutePool);
    DestroyPool(ms_pNodeRoutePool);
    DestroyPool(ms_pTaskAllocatorPool);
    DestroyPool(ms_pPedIntelligencePool);
    DestroyPool(ms_pPedAttractorPool);
    exePrintf("Shutdown pool done\n");
}

// 0x550170
int32 CPools::CheckBuildingAtomics() {
    return -1;
}

// 0x551950
void CPools::CheckPoolsEmpty() {
    for (auto& obj : ms_pObjectPool->GetAllValid()) {
        if (obj.m_nObjectType != OBJECT_TYPE_DECORATION)
            continue;

        const auto& objPos = obj.GetPosition();
        NOTSA_LOG_DEBUG("Offending object: MI: {} Coors:{} {} {}", obj.m_nModelIndex, objPos.x, objPos.y, objPos.z);
    }
    NOTSA_LOG_DEBUG("Pools have been cleared!");
}

// 0x550050
CObject* CPools::GetObject(int32 handle) {
    return ms_pObjectPool->GetAtRef(handle);
}

// 0x550020
int32 CPools::GetObjectRef(CObject* object) {
    return ms_pObjectPool->GetRef(object);
}

// 0x54FF90
CPed* CPools::GetPed(int32 handle) {
    return ms_pPedPool->GetAtRef(handle);
}

// 0x54FF60
int32 CPools::GetPedRef(CPed* ped) {
    return ms_pPedPool->GetRef(ped);
}

// 0x54FFF0
CVehicle* CPools::GetVehicle(int32 handle) {
    return ms_pVehiclePool->GetAtRef(handle);
}

// 0x54FFC0
int32 CPools::GetVehicleRef(CVehicle* vehicle) {
    return ms_pVehiclePool->GetRef(vehicle);
}

// 0x5D0890
bool CPools::Load() {
    bool loaded = LoadPedPool();
    if (loaded)
        loaded = LoadObjectPool();

    return loaded;
}

// 0x5D4A40
bool CPools::LoadObjectPool() {
    auto iNumObjects = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
    for (int32 i = 0; i < iNumObjects; ++i)
    {
        auto iPoolRef = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        auto iModelId = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();

        auto* objInPool = GetObjectPool()->GetAtRefNoChecks(iPoolRef);
        if (objInPool)
            CPopulation::ConvertToDummyObject(objInPool);

        auto* newObj = new(iPoolRef) CObject(iModelId, false);
        // Como el original (0x5D4AFC..0x5D4B1D): lee el tamano (4 bytes, sin comprobarlo), luego 0x30 bytes y llama a Extract (0x5D2220);
        // no pasa por CObject::Load (0x5D2870), que en la DLL tiene un assert que el EXE no tiene.
        [[maybe_unused]] const auto structSize = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        auto data = CGenericGameStorage::LoadDataFromWorkBuffer<CObjectSaveStructure>();
        data.Extract(newObj);
        CWorld::Add(newObj);
    }

    return true;
}

// 0x5D2D70
bool CPools::LoadPedPool() {
    const auto pedCount = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
    // Como el original (0x5D2D93): el puntero al ped (esi) se pone a 0 una vez antes del bucle y se conserva entre vueltas
    CPlayerPed* playerPed = nullptr;
    for (auto i = 0; i < pedCount; i++) {
        const auto poolRef = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        const auto model   = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();
        const auto pedType = CGenericGameStorage::LoadDataFromWorkBuffer<int32>();

        if (pedType == PED_TYPE_PLAYER1) {
            playerPed = new(poolRef) CPlayerPed(0, false);
            playerPed->SetWeaponAccuracy(100);
            CWorld::Players[0].m_pPed = playerPed;
        }
        assert(playerPed != nullptr);

        playerPed->Load();
        CWorld::Add(playerPed);
    }
    return true;
}

// 0x5D2A20
// Used in CPools::Load (Android 1.0)
bool CPools::LoadVehiclePool() {
    int32 count;
    CGenericGameStorage::LoadDataFromWorkBuffer(&count, 4);
    for (auto i = 0; i < count; i++) {
        // Como el original (0x5D2A3C..0x5D2A4E): el primer valor es la referencia de pool (se usa con operator new(size, ref) = NewAt)
        int32 modelId, poolRef;

        CGenericGameStorage::LoadDataFromWorkBuffer(&poolRef, 4);
        CGenericGameStorage::LoadDataFromWorkBuffer(&modelId, 4);

        CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
        CStreaming::LoadAllRequestedModels(false);

        CVehicle* vehicle = nullptr;
        switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
        case VEHICLE_TYPE_MTRUCK:
            vehicle = new(poolRef) CMonsterTruck(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_QUAD:
            vehicle = new(poolRef) CQuadBike(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_HELI:
            vehicle = new(poolRef) CHeli(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_PLANE:
            vehicle = new(poolRef) CPlane(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_BOAT:
            vehicle = new(poolRef) CBoat(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_TRAIN:
            vehicle = new(poolRef) CTrain(modelId, RANDOM_VEHICLE);
            break;
        case VEHICLE_TYPE_BIKE:
            vehicle = new(poolRef) CBike(modelId, RANDOM_VEHICLE);
            vehicle->AsBike()->bikeFlags.bOnSideStand = true;
            break;
        case VEHICLE_TYPE_BMX:
            vehicle = new(poolRef) CBmx(modelId, RANDOM_VEHICLE);
            vehicle->AsBike()->bikeFlags.bOnSideStand = true;
            break;
        case VEHICLE_TYPE_TRAILER:
            vehicle = new(poolRef) CTrailer(modelId, RANDOM_VEHICLE);
            break;
        default:
            vehicle = new(poolRef) CAutomobile(modelId, RANDOM_VEHICLE, true);
            break;
        }
        assert(vehicle);
        vehicle->Load();
        --CCarCtrl::NumRandomCars;
        CCarCtrl::UpdateCarCount(vehicle, false);
        CWorld::Add(vehicle);
    }
    return true;
}

// 0x550080
void CPools::MakeSureSlotInObjectPoolIsEmpty(int32 slot) {
    // Como el original (0x550080): si la ranura esta libre (bit 7 del byte de estado) no hace nada.
    auto* const pool = GetObjectPool();
    if (pool->IsFreeSlotAtIndex(slot)) {
        return;
    }
    auto* const obj = pool->GetAt(slot);

    // Objeto temporal (m_nObjectType == 3, +0x13C): CWorld::Remove (0x563280) y destructor virtual con borrado.
    if (obj->m_nObjectType == OBJECT_TEMPORARY) {
        CWorld::Remove(obj);
        delete obj;
        return;
    }

    // Como el original (0x5500E8): RemoveIfThisIsAProjectile (0x739A40) borra el proyectil y devuelve true -> fin.
    // Solo si NO era un proyectil se sustituye el objeto por una copia (la version anterior tenia la condicion invertida).
    if (CProjectileInfo::RemoveIfThisIsAProjectile(obj)) {
        return;
    }
    auto* const newObj = new CObject(static_cast<int16>(obj->m_nModelIndex), false); // operator new 0x5A1EE0 + ctor 0x5A1D70 (movsx de +0x22)
    CWorld::Remove(obj);
    std::memcpy(static_cast<void*>(newObj), static_cast<const void*>(obj), 0x19C);   // rep movs dword x 0x67
    CWorld::Add(newObj);
    obj->ClearRwObjectPointer();
    delete obj;
    newObj->m_pReferences = nullptr;
}

// 0x5D0880
bool CPools::Save() {
    bool saved = SavePedPool();
    if (saved)
        saved = SaveObjectPool();

    return saved;
}

// 0x5D4940
bool CPools::SaveObjectPool() {
    const auto size = ms_pObjectPool->GetSize();
    int32 count = 0;
    for (size_t i = 0; i < size; ++i) {
        if (auto* obj = ms_pObjectPool->GetAt(i); obj && obj->m_nObjectType == 2) {
            ++count;
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(count);
    for (size_t i = 0; i < size; ++i) {
        if (auto* obj = ms_pObjectPool->GetAt(i); obj && obj->m_nObjectType == 2) {
            const int32 reference = GetObjectRef(obj);
            const int32 model = static_cast<int16>(obj->m_nModelIndex);
            CGenericGameStorage::SaveDataToWorkBuffer(reference);
            CGenericGameStorage::SaveDataToWorkBuffer(model);
            CObjectSaveStructure data;
            data.Construct(obj);
            CGenericGameStorage::SaveDataToWorkBuffer(int32(sizeof(data)));
            CGenericGameStorage::SaveDataToWorkBuffer(data);
        }
    }
    return true;
}

// 0x5D4B40
bool CPools::SavePedPool() {
    const auto size = ms_pPedPool->GetSize();
    int32 count = 0;
    for (size_t i = 0; i < size; ++i) {
        if (auto* ped = ms_pPedPool->GetAt(i); ped && !ped->bInVehicle && ped->m_nPedType == PED_TYPE_PLAYER1) {
            ++count;
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(count);
    for (size_t i = 0; i < size; ++i) {
        if (auto* ped = ms_pPedPool->GetAt(i); ped && !ped->bInVehicle && ped->m_nPedType == PED_TYPE_PLAYER1) {
            const int32 reference = GetPedRef(ped);
            const int32 model = static_cast<int16>(ped->m_nModelIndex);
            const int32 type = ped->m_nPedType;
            CGenericGameStorage::SaveDataToWorkBuffer(reference);
            CGenericGameStorage::SaveDataToWorkBuffer(model);
            CGenericGameStorage::SaveDataToWorkBuffer(type);
            ped->Save();
        }
    }
    return true;
}

// 0x5D4800
// Used in CPools::Save (Android 1.0)
bool CPools::SaveVehiclePool() {
    const auto shouldSave = [](CVehicle* vehicle) {
        if (!vehicle || vehicle->m_nCreatedBy != MISSION_VEHICLE || vehicle->m_pDriver) {
            return false;
        }
        return std::ranges::none_of(vehicle->m_apPassengers, [](CPed* passenger) { return passenger != nullptr; });
    };
    const auto size = ms_pVehiclePool->GetSize();
    int32 count = 0;
    for (size_t i = 0; i < size; ++i) {
        if (shouldSave(ms_pVehiclePool->GetAt(i))) {
            ++count;
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(count);
    for (size_t i = 0; i < size; ++i) {
        if (auto* vehicle = ms_pVehiclePool->GetAt(i); shouldSave(vehicle)) {
            const int32 reference = GetVehicleRef(vehicle);
            const int32 model = static_cast<int16>(vehicle->m_nModelIndex);
            CGenericGameStorage::SaveDataToWorkBuffer(reference);
            CGenericGameStorage::SaveDataToWorkBuffer(model);
            vehicle->Save();
        }
    }
    return true;
}

// 0x404550
CPedPool* GetPedPool() {
    return ms_pPedPool;
}

// 0x404560
CVehiclePool* GetVehiclePool() {
    return ms_pVehiclePool;
}

// 0x403DF0
CBuildingPool* GetBuildingPool() {
    return ms_pBuildingPool;
}

// 0x404570
CObjectPool* GetObjectPool() {
    return ms_pObjectPool;
}

// 0x404580
CDummyPool* GetDummyPool() {
    return ms_pDummyPool;
}

// 0x40F000
CColModelPool* GetColModelPool() {
    return ms_pColModelPool;
}

// 0x61A330
CTaskPool* GetTaskPool() {
    return ms_pTaskPool;
}

// 0x5FF930
CPedIntelligencePool* GetPedIntelligencePool() {
    return ms_pPedIntelligencePool;
}

// 0x552190
CPtrNodeSingleLinkPool* GetPtrNodeSingleLinkPool() {
    return ms_pPtrNodeSingleLinkPool;
}

// 0x5521A0
CPtrNodeDoubleLinkPool* GetPtrNodeDoubleLinkPool() {
    return ms_pPtrNodeDoubleLinkPool;
}

// 0x536C80
CEntryInfoNodePool* GetEntryInfoNodePool() {
    return ms_pEntryInfoNodePool;
}

// 0x41B400
CPointRoutePool* GetPointRoutePool() {
    return ms_pPointRoutePool;
}

// 0x41B410
CPatrolRoutePool* GetPatrolRoutePool() {
    return ms_pPatrolRoutePool;
}

// 0x4ABF80
CEventPool* GetEventPool() {
    return ms_pEventPool;
}

// 0x41B420
CNodeRoutePool* GetNodeRoutePool() {
    return ms_pNodeRoutePool;
}

// 0x69BB70
CTaskAllocatorPool* GetTaskAllocatorPool() {
    return ms_pTaskAllocatorPool;
}

// 0x5E95A0
CPedAttractorPool* GetPedAttractorPool() {
    return ms_pPedAttractorPool;
}
