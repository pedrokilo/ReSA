/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CModelInfo
 * Reconstrucción: COMPLETA — 38/38 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 38/38 entradas.
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

#include "ModelInfo.h"
#include "TempColModels.h"

auto& CModelInfo::ms_atomicModelInfoStore = StaticRef<CStore<CAtomicModelInfo, NUM_ATOMIC_MODEL_INFOS>>(0xAAE950);
auto& CModelInfo::ms_damageAtomicModelInfoStore = StaticRef<CStore<CDamageAtomicModelInfo, NUM_DAMAGE_ATOMIC_MODEL_INFOS>>(0xB1BF58);
auto& CModelInfo::ms_lodAtomicModelInfoStore = StaticRef<CStore<CLodAtomicModelInfo, NUM_LOD_ATOMIC_MODEL_INFOS>>(0xB1C934);
auto& CModelInfo::ms_timeModelInfoStore = StaticRef<CStore<CTimeModelInfo, NUM_TIME_MODEL_INFOS>>(0xB1C960);
auto& CModelInfo::ms_lodTimeModelInfoStore = StaticRef<CStore<CLodTimeModelInfo, NUM_LOD_TIME_MODEL_INFOS>>(0xB1E128);
auto& CModelInfo::ms_weaponModelInfoStore = StaticRef<CStore<CWeaponModelInfo, NUM_WEAPON_MODEL_INFOS>>(0xB1E158);
auto& CModelInfo::ms_clumpModelInfoStore = StaticRef<CStore<CClumpModelInfo, NUM_CLUMP_MODEL_INFOS>>(0xB1E958);
auto& CModelInfo::ms_vehicleModelInfoStore = StaticRef<CStore<CVehicleModelInfo, NUM_VEHICLE_MODEL_INFOS>>(0xB1F650);
auto& CModelInfo::ms_pedModelInfoStore = StaticRef<CStore<CPedModelInfo, NUM_PED_MODEL_INFOS>>(0xB478F8);
auto& CModelInfo::ms_2dFXInfoStore = StaticRef<CStore<C2dEffect, NUM_2DFX_INFOS>>(0xB4C2D8);

// Constructores de los almacenes (CStore) de model infos (lote 194A, sin validar). Cada uno es el marco del `eh vector constructor iterator` del CRT del EXE (0x821D30; thiscall, sin argumentos de pila, `ret`,
// 36 a 42 bytes; los llama el inicializador estatico de cada almacen): llama al constructor de ELEMENTO del EXE (se pasa por direccion: unos estan enganchados (0x4C5540...) y otros no (0x4C57A0, 0x4C75E0)) sobre
// cada elemento en orden ascendente (puntero + 4 + i * tamano; el iterador solo escribe lo que escribe el constructor del elemento, no pone a cero el resto), y DESPUES pone el contador (+0x0) a cero. Devuelve
// this. El destructor de desenrollado que pasa el EXE (0x4C5590...) solo se usa si un constructor lanzara y no tiene efecto aqui.
template<typename T, size_t Capacity>
static CStore<T, Capacity>* ConstructModelInfoStore(CStore<T, Capacity>* self, uintptr_t elementConstructor) {
    auto* const elements = reinterpret_cast<uint8*>(&self->m_aObjects);
    for (size_t i = 0; i < Capacity; ++i) {
        reinterpret_cast<void(__thiscall*)(void*)>(elementConstructor)(elements + i * sizeof(T));
    }
    self->m_nCount = 0;
    return self;
}

#define NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(name, Type, count, elementCtor)     static CStore<Type, count>* __fastcall name(CStore<Type, count>* self, void* /*edx*/) {         static_assert(sizeof(CStore<Type, count>) == sizeof(uint32) + sizeof(Type) * (count));         return ConstructModelInfoStore(self, elementCtor);     }

// 0x4C5CB0: 14000 CAtomicModelInfo de 0x20 bytes, constructor 0x4C5540 (destructor 0x4C5590)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(AtomicModelInfoStoreConstructor, CAtomicModelInfo, CModelInfo::NUM_ATOMIC_MODEL_INFOS, 0x4C5540)
// 0x4C5D10: 70 CDamageAtomicModelInfo de 0x24 bytes, constructor 0x4C55A0 (destructor 0x4C55D0)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(DamageAtomicModelInfoStoreConstructor, CDamageAtomicModelInfo, CModelInfo::NUM_DAMAGE_ATOMIC_MODEL_INFOS, 0x4C55A0)
// 0x4C5D70: 1 CLodAtomicModelInfo de 0x24 bytes, constructor 0x4C55E0 (destructor 0x4C5620)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(LodAtomicModelInfoStoreConstructor, CLodAtomicModelInfo, CModelInfo::NUM_LOD_ATOMIC_MODEL_INFOS, 0x4C55E0)
// 0x4C5DD0: 169 CTimeModelInfo de 0x24 bytes, constructor 0x4C5640 (destructor 0x4C5680)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(TimeModelInfoStoreConstructor, CTimeModelInfo, CModelInfo::NUM_TIME_MODEL_INFOS, 0x4C5640)
// 0x4C5E30: 1 CLodTimeModelInfo de 0x28 bytes, constructor 0x4C5690 (destructor 0x4C56E0)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(LodTimeModelInfoStoreConstructor, CLodTimeModelInfo, CModelInfo::NUM_LOD_TIME_MODEL_INFOS, 0x4C5690)
// 0x4C5E90: 51 CWeaponModelInfo de 0x28 bytes, constructor 0x4C5760 (destructor 0x4C5790)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(WeaponModelInfoStoreConstructor, CWeaponModelInfo, CModelInfo::NUM_WEAPON_MODEL_INFOS, 0x4C5760)
// 0x4C5EF0: 92 CClumpModelInfo de 0x24 bytes, constructor 0x4C56F0 (destructor 0x4C5750)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(ClumpModelInfoStoreConstructor, CClumpModelInfo, CModelInfo::NUM_CLUMP_MODEL_INFOS, 0x4C56F0)
// 0x4C5F50: 212 CVehicleModelInfo de 0x308 bytes, constructor 0x4C75E0 (sin reconstruir; destructor 0x4C5920)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(VehicleModelInfoStoreConstructor, CVehicleModelInfo, CModelInfo::NUM_VEHICLE_MODEL_INFOS, 0x4C75E0)
// 0x4C67D0: 278 CPedModelInfo de 0x44 bytes, constructor 0x4C57A0 (sin reconstruir; destructor 0x4C62F0)
NOTSA_MODEL_INFO_STORE_CONSTRUCTOR(PedModelInfoStoreConstructor, CPedModelInfo, CModelInfo::NUM_PED_MODEL_INFOS, 0x4C57A0)

#undef NOTSA_MODEL_INFO_STORE_CONSTRUCTOR

// Tamanos de elemento que usa el iterador del EXE
static_assert(sizeof(CAtomicModelInfo) == 0x20 && sizeof(CDamageAtomicModelInfo) == 0x24 && sizeof(CLodAtomicModelInfo) == 0x24 && sizeof(CTimeModelInfo) == 0x24);
static_assert(sizeof(CLodTimeModelInfo) == 0x28 && sizeof(CWeaponModelInfo) == 0x28 && sizeof(CClumpModelInfo) == 0x24 && sizeof(CVehicleModelInfo) == 0x308 && sizeof(CPedModelInfo) == 0x44);

/**
 * Registra las operaciones del catálogo global de model infos.
 */
void CModelInfo::InjectHooks()
{
    RH_ScopedClass(CModelInfo);
    RH_ScopedCategory("Models");

    RH_ScopedInstall(Initialise, 0x4C6810);
    RH_ScopedNamedGlobalInstall(AtomicModelInfoStoreConstructor, "AtomicModelInfoStoreConstructor", 0x4C5CB0);                // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(DamageAtomicModelInfoStoreConstructor, "DamageAtomicModelInfoStoreConstructor", 0x4C5D10);  // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(LodAtomicModelInfoStoreConstructor, "LodAtomicModelInfoStoreConstructor", 0x4C5D70);        // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(TimeModelInfoStoreConstructor, "TimeModelInfoStoreConstructor", 0x4C5DD0);                  // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(LodTimeModelInfoStoreConstructor, "LodTimeModelInfoStoreConstructor", 0x4C5E30);            // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(WeaponModelInfoStoreConstructor, "WeaponModelInfoStoreConstructor", 0x4C5E90);              // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(ClumpModelInfoStoreConstructor, "ClumpModelInfoStoreConstructor", 0x4C5EF0);                // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(VehicleModelInfoStoreConstructor, "VehicleModelInfoStoreConstructor", 0x4C5F50);            // lote 194A (sin validar)
    RH_ScopedNamedGlobalInstall(PedModelInfoStoreConstructor, "PedModelInfoStoreConstructor", 0x4C67D0);                    // lote 194A (sin validar)
    RH_ScopedInstall(Get2dEffectStore, 0x4C5A60);
    RH_ScopedInstall(ShutDown, 0x4C63E0);
    RH_ScopedInstall(ReInit2dEffects, 0x4C63B0);

    RH_ScopedInstall(GetModelInfoUInt16, 0x4C59F0);
    RH_ScopedInstall(GetModelInfoFromHashKey, 0x4C59B0);
    RH_ScopedOverloadedInstall(GetModelInfo, "full", 0x4C5940, CBaseModelInfo * (*)(const char*, int32*));
    RH_ScopedOverloadedInstall(GetModelInfo, "minmax", 0x4C5A20, CBaseModelInfo*(*)(const char*, int32, int32));

    RH_ScopedInstall(AddAtomicModel, 0x4C6620);
    RH_ScopedInstall(AddDamageAtomicModel, 0x4C6650);
    RH_ScopedInstall(AddLodAtomicModel, 0x4C6680);
    RH_ScopedInstall(AddTimeModel, 0x4C66B0);
    RH_ScopedInstall(AddLodTimeModel, 0x4C66E0);
    RH_ScopedInstall(AddWeaponModel, 0x4C6710);
    RH_ScopedInstall(AddClumpModel, 0x4C6740);
    RH_ScopedInstall(AddVehicleModel, 0x4C6770);
    RH_ScopedInstall(AddPedModel, 0x4C67A0);

    RH_ScopedInstall(IsBoatModel, 0x4C5A70);
    RH_ScopedInstall(IsCarModel, 0x4C5AA0);
    RH_ScopedInstall(IsTrainModel, 0x4C5AD0);
    RH_ScopedInstall(IsHeliModel, 0x4C5B00);
    RH_ScopedInstall(IsPlaneModel, 0x4C5B30);
    RH_ScopedInstall(IsBikeModel, 0x4C5B60);
    RH_ScopedInstall(IsFakePlaneModel, 0x4C5B90);
    RH_ScopedInstall(IsMonsterTruckModel, 0x4C5BC0);
    RH_ScopedInstall(IsQuadBikeModel, 0x4C5BF0);
    RH_ScopedInstall(IsBmxModel, 0x4C5C20);
    RH_ScopedInstall(IsTrailerModel, 0x4C5C50);
    RH_ScopedInstall(IsVehicleModelType, 0x4C5C80);
}

// 0x4C63B0
// Vacía el store global de 2DFX y reinicia el rango registrado por cada model info.
void CModelInfo::ReInit2dEffects()
{
    ms_2dFXInfoStore.m_nCount = 0;
    for (int32 i = 0; i < NUM_MODEL_INFOS; ++i) {
        if (auto* mi = GetModelInfo(i)) // Como el original: salta las entradas vacías
            mi->Init2dEffects();
    }
}

// 0x4C63E0
/**
 * Ejecuta Shutdown() sobre cada store tipado y luego reinicia todos sus contadores.
 * Los stores son globales/fijos; no se destruyen aquí sus bloques de almacenamiento.
 *
 * @addr 0x4C63E0
 */
void CModelInfo::ShutDown()
{
    for (uint32 i = 0; i < ms_atomicModelInfoStore.m_nCount; ++i)
        ms_atomicModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_damageAtomicModelInfoStore.m_nCount; ++i)
        ms_damageAtomicModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_lodAtomicModelInfoStore.m_nCount; ++i)
        ms_lodAtomicModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_timeModelInfoStore.m_nCount; ++i)
        ms_timeModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_lodTimeModelInfoStore.m_nCount; ++i)
        ms_lodTimeModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_weaponModelInfoStore.m_nCount; ++i)
        ms_weaponModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_clumpModelInfoStore.m_nCount; ++i)
        ms_clumpModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_vehicleModelInfoStore.m_nCount; ++i)
        ms_vehicleModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_pedModelInfoStore.m_nCount; ++i)
        ms_pedModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_2dFXInfoStore.m_nCount; ++i)
        ms_2dFXInfoStore.GetItemAtIndex(i).Shutdown();

    ms_atomicModelInfoStore.m_nCount = 0;
    ms_damageAtomicModelInfoStore.m_nCount = 0;
    ms_lodAtomicModelInfoStore.m_nCount = 0;
    ms_timeModelInfoStore.m_nCount = 0;
    ms_lodTimeModelInfoStore.m_nCount = 0;
    ms_weaponModelInfoStore.m_nCount = 0;
    ms_clumpModelInfoStore.m_nCount = 0;
    ms_vehicleModelInfoStore.m_nCount = 0;
    ms_pedModelInfoStore.m_nCount = 0;
    ms_2dFXInfoStore.m_nCount = 0;
}

// 0x4C6620
CAtomicModelInfo* CModelInfo::AddAtomicModel(int32 index)
{
    auto& mi = ms_atomicModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6650
CDamageAtomicModelInfo* CModelInfo::AddDamageAtomicModel(int32 index)
{
    auto& mi = ms_damageAtomicModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6680
CLodAtomicModelInfo* CModelInfo::AddLodAtomicModel(int32 index)
{
    auto& mi =  ms_lodAtomicModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C66B0
CTimeModelInfo* CModelInfo::AddTimeModel(int32 index)
{
    auto& mi = ms_timeModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C66E0
CLodTimeModelInfo* CModelInfo::AddLodTimeModel(int32 index)
{
    auto& mi = ms_lodTimeModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6710
CWeaponModelInfo* CModelInfo::AddWeaponModel(int32 index)
{
    auto& mi = ms_weaponModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6740
CClumpModelInfo* CModelInfo::AddClumpModel(int32 index)
{
    auto& mi = ms_clumpModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6770
CVehicleModelInfo* CModelInfo::AddVehicleModel(int32 index)
{
    auto& mi = ms_vehicleModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C67A0
CPedModelInfo* CModelInfo::AddPedModel(int32 index)
{
    auto& mi = ms_pedModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6810
/**
 * Reinicia el catálogo y crea los model infos temporales usados por piezas de vehículo.
 *
 * Estas entradas reutilizan los CTempColModels globales y el TXD generic, sin tomar
 * ownership de esos modelos de colisión compartidos.
 *
 * @addr 0x4C6810
 */
void CModelInfo::Initialise() {
    ZoneScoped;

    memset(ms_modelInfoPtrs, 0, sizeof(ms_modelInfoPtrs));
    ms_damageAtomicModelInfoStore.m_nCount = 0;
    ms_lodAtomicModelInfoStore.m_nCount = 0;
    ms_timeModelInfoStore.m_nCount = 0;
    ms_lodTimeModelInfoStore.m_nCount = 0;
    ms_weaponModelInfoStore.m_nCount = 0;
    ms_clumpModelInfoStore.m_nCount = 0;
    ms_vehicleModelInfoStore.m_nCount = 0;
    ms_pedModelInfoStore.m_nCount = 0;
    ms_2dFXInfoStore.m_nCount = 0;
    ms_atomicModelInfoStore.m_nCount = 0;

    auto door1 = AddAtomicModel(MODEL_TEMPCOL_DOOR1);
    door1->SetColModel(&CTempColModels::ms_colModelDoor1, false);
    door1->SetTexDictionary("generic");
    door1->m_fDrawDistance = 80.0F;

    auto bumper1 = AddAtomicModel(MODEL_TEMPCOL_BUMPER1);
    bumper1->SetColModel(&CTempColModels::ms_colModelBumper1, false);
    bumper1->SetTexDictionary("generic");
    bumper1->m_fDrawDistance = 80.0F;

    auto modelPanel1 = AddAtomicModel(MODEL_TEMPCOL_PANEL1);
    modelPanel1->SetColModel(&CTempColModels::ms_colModelPanel1, false);
    modelPanel1->SetTexDictionary("generic");
    modelPanel1->m_fDrawDistance = 80.0F;

    auto bonnet1 = AddAtomicModel(MODEL_TEMPCOL_BONNET1);
    bonnet1->SetColModel(&CTempColModels::ms_colModelBonnet1, false);
    bonnet1->SetTexDictionary("generic");
    bonnet1->m_fDrawDistance = 80.0F;

    auto boot1 = AddAtomicModel(MODEL_TEMPCOL_BOOT1);
    boot1->SetColModel(&CTempColModels::ms_colModelBoot1, false);
    boot1->SetTexDictionary("generic");
    boot1->m_fDrawDistance = 80.0F;

    auto wheel1 = AddAtomicModel(MODEL_TEMPCOL_WHEEL1);
    wheel1->SetColModel(&CTempColModels::ms_colModelWheel1, false);
    wheel1->SetTexDictionary("generic");
    wheel1->m_fDrawDistance = 80.0F;

    auto bodyPart1 = AddAtomicModel(MODEL_TEMPCOL_BODYPART1);
    bodyPart1->SetColModel(&CTempColModels::ms_colModelBodyPart1, false);
    bodyPart1->SetTexDictionary("generic");
    bodyPart1->m_fDrawDistance = 80.0F;

    auto bodyPart2 = AddAtomicModel(MODEL_TEMPCOL_BODYPART2);
    bodyPart2->SetColModel(&CTempColModels::ms_colModelBodyPart2, false);
    bodyPart2->SetTexDictionary("generic");
    bodyPart2->m_fDrawDistance = 80.0F;
}

// 0x4C5940
/**
 * Busca por hash uppercase utilizando ms_lastPositionSearched como punto de inicio.
 * La posición se conserva para acelerar búsquedas consecutivas durante la carga.
 *
 * @addr 0x4C5940
 */
CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32* index)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    auto iCurInd = ms_lastPositionSearched;

    while (iCurInd < NUM_MODEL_INFOS) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            if (index)
                *index = iCurInd;

            return mi;
        }

        ++iCurInd;
    }

    iCurInd = ms_lastPositionSearched;
    if (iCurInd < 0)
        return nullptr;

    while (iCurInd >= 0) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            if (index)
                *index = iCurInd;

            return mi;
        }

        --iCurInd;
    }

    return nullptr;
}

// 0x4C59B0
CBaseModelInfo* CModelInfo::GetModelInfoFromHashKey(uint32 uiHash, int32* index)
{
    for (int32 i = 0; i < NUM_MODEL_INFOS; ++i) {
        auto mi = GetModelInfo(i);
        if (mi && mi->m_nKey == uiHash) {
            if (index)
                *index = i;

            return mi;
        }
    }

    return nullptr;
}

// 0x4C59F0
CBaseModelInfo* CModelInfo::GetModelInfoUInt16(const char* name, uint16* pOutIndex)
{
    int32 modelId = 0;
    auto result = GetModelInfo(name, &modelId);
    if (pOutIndex)
        *pOutIndex = modelId;

    return result;
}

// 0x4C5A20
CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32 minIndex, int32 maxIndex)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    if (minIndex > maxIndex)
        return nullptr;

    for (int32 i = minIndex; i <= maxIndex; ++i) {
        auto mi = GetModelInfo(i);
        if (mi && mi->m_nKey == iKey)
            return mi;
    }

    return nullptr;
}

// 0x4C5A60
#ifdef _DEBUG
#pragma optimize("", on)
#endif
// 0x4C5A60
CStore<C2dEffect, CModelInfo::NUM_2DFX_INFOS>* CModelInfo::Get2dEffectStore()
{
    return reinterpret_cast<CStore<C2dEffect, NUM_2DFX_INFOS>*>(uintptr_t{0xB4C2D8});
}
#ifdef _DEBUG
#pragma optimize("", off)
#endif

// 0x4C5A70
bool CModelInfo::IsBoatModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsBoat();
}

// 0x4C5AA0
bool CModelInfo::IsCarModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsAutomobile();
}

// 0x4C5AD0
bool CModelInfo::IsTrainModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsTrain();
}

// 0x4C5B00
bool CModelInfo::IsHeliModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsHeli();
}

// 0x4C5B30
bool CModelInfo::IsPlaneModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsPlane();
}

// 0x4C5B60
bool CModelInfo::IsBikeModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsBike();
}

// 0x4C5B90
bool CModelInfo::IsFakePlaneModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->m_nVehicleType == VEHICLE_TYPE_FPLANE;
}

// 0x4C5BC0
bool CModelInfo::IsMonsterTruckModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsMonsterTruck();
}

// 0x4C5BF0
bool CModelInfo::IsQuadBikeModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsQuad();
}

// 0x4C5C20
bool CModelInfo::IsBmxModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsBMX();
}

// 0x4C5C50
bool CModelInfo::IsTrailerModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsTrailer();
}

// 0x4C5C80
int32 CModelInfo::IsVehicleModelType(int32 index)
{
    if (index >= NUM_MODEL_INFOS)
        return -1;

    auto mi = GetModelInfo(index);
    if (!mi)
        return -1;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return -1;

    return mi->AsVehicleModelInfoPtr()->m_nVehicleType;
}
