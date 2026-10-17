/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "BaseModelInfo.h"
#include "AtomicModelInfo.h"
#include "DamageAtomicModelInfo.h"
#include "LodAtomicModelInfo.h"
#include "VehicleModelInfo.h"
#include "WeaponModelInfo.h"
#include "TimeModelInfo.h"
#include "LodTimeModelInfo.h"
#include "PedModelInfo.h"


/**
 * Registro global que mapea IDs de modelo a CBaseModelInfo y administra stores tipados.
 *
 * Los objetos viven en CStore de capacidad fija; ms_modelInfoPtrs proporciona una
 * vista unificada por model ID independientemente de la subclase concreta.
 */
class CModelInfo {
public:
    // Tabla maestra model ID -> model info; las instancias reales pertenecen a stores tipados.
    static constexpr int32 NUM_MODEL_INFOS = 20000;
    static inline auto& ms_modelInfoPtrs = StaticRef<CBaseModelInfo*[NUM_MODEL_INFOS]>(0xA9B0C8);
    static inline auto& ms_lastPositionSearched = StaticRef<int32>(0xAAE948);

    static constexpr int32 NUM_ATOMIC_MODEL_INFOS = 14000;
    static CStore<CAtomicModelInfo, NUM_ATOMIC_MODEL_INFOS>& ms_atomicModelInfoStore;

    static constexpr int32 NUM_DAMAGE_ATOMIC_MODEL_INFOS = 70;
    static CStore<CDamageAtomicModelInfo, NUM_DAMAGE_ATOMIC_MODEL_INFOS>& ms_damageAtomicModelInfoStore;

    static constexpr int32 NUM_LOD_ATOMIC_MODEL_INFOS = 1;
    static CStore<CLodAtomicModelInfo, NUM_LOD_ATOMIC_MODEL_INFOS>& ms_lodAtomicModelInfoStore;

    static constexpr int32 NUM_TIME_MODEL_INFOS = 169;
    static CStore<CTimeModelInfo, NUM_TIME_MODEL_INFOS>& ms_timeModelInfoStore;

    static constexpr int32 NUM_LOD_TIME_MODEL_INFOS = 1;
    static CStore<CLodTimeModelInfo, NUM_LOD_TIME_MODEL_INFOS>& ms_lodTimeModelInfoStore;

    static constexpr int32 NUM_WEAPON_MODEL_INFOS = 51;
    static CStore<CWeaponModelInfo, NUM_WEAPON_MODEL_INFOS>& ms_weaponModelInfoStore;

    static constexpr int32 NUM_CLUMP_MODEL_INFOS = 92;
    static CStore<CClumpModelInfo, NUM_CLUMP_MODEL_INFOS>& ms_clumpModelInfoStore;

    static constexpr int32 NUM_VEHICLE_MODEL_INFOS = 212;
    static CStore<CVehicleModelInfo, NUM_VEHICLE_MODEL_INFOS>& ms_vehicleModelInfoStore;

    static constexpr int32 NUM_PED_MODEL_INFOS = 278;
    static CStore<CPedModelInfo, NUM_PED_MODEL_INFOS>& ms_pedModelInfoStore;

    static constexpr int32 NUM_2DFX_INFOS = 100;
    static CStore<C2dEffect, NUM_2DFX_INFOS>& ms_2dFXInfoStore;

public:
    static void InjectHooks();

    // Reinicia el store 2DFX y los rangos registrados en cada modelo.
    static void ReInit2dEffects();
    // Libera recursos internos de todos los model infos y vacía los stores.
    static void ShutDown();
    static CAtomicModelInfo* AddAtomicModel(int32 index);
    static CDamageAtomicModelInfo* AddDamageAtomicModel(int32 index);
    static CLodAtomicModelInfo* AddLodAtomicModel(int32 index);
    static CTimeModelInfo* AddTimeModel(int32 index);
    static CLodTimeModelInfo* AddLodTimeModel(int32 index);
    static CWeaponModelInfo* AddWeaponModel(int32 index);
    static CClumpModelInfo* AddClumpModel(int32 index);
    static CVehicleModelInfo* AddVehicleModel(int32 index);
    static class CPedModelInfo* AddPedModel(int32 index);
    static void Initialise();
    static CBaseModelInfo* GetModelInfo(const char* name, int32* index = nullptr);
    static CBaseModelInfo* GetModelInfoFromHashKey(uint32 uiHash, int32* index = nullptr);
    static CBaseModelInfo* GetModelInfoUInt16(const char* name, uint16* pOutIndex = nullptr);
    // get model in range (search for model only in range (min;max))
    static CBaseModelInfo* GetModelInfo(const char* name, int32 minIndex, int32 maxIndex);
    static CStore<C2dEffect, NUM_2DFX_INFOS>* Get2dEffectStore();
    static bool IsBoatModel(int32 index);
    static bool IsCarModel(int32 index);
    static bool IsTrainModel(int32 index);
    static bool IsHeliModel(int32 index);
    static bool IsPlaneModel(int32 index);
    static bool IsBikeModel(int32 index);
    static bool IsFakePlaneModel(int32 index);
    static bool IsMonsterTruckModel(int32 index);
    static bool IsQuadBikeModel(int32 index);
    static bool IsBmxModel(int32 index);
    static bool IsTrailerModel(int32 index);
    // Devuelve -1 si el ID no corresponde a vehículo; en caso contrario devuelve eVehicleType.
    static int32 IsVehicleModelType(int32 index);

    static CBaseModelInfo* GetModelInfo(int32 index) { return ms_modelInfoPtrs[index]; }
    static auto GetPedModelInfo(int32 index) { return GetModelInfo(index)->AsPedModelInfoPtr(); }
    static auto GetVehicleModelInfo(int32 index) { return GetModelInfo(index)->AsVehicleModelInfoPtr(); }
    static void SetModelInfo(int32 index, CBaseModelInfo* pInfo) { ms_modelInfoPtrs[index] = pInfo; }
};
