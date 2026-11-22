#include "StdInc.h"
#include "AEBankLoader.h"
#include "../Streaming.h"
#include "../Radar.h"
#include "../CarCtrl.h"
#include "../PostEffects.h"
#include "TheScripts.h"
#include "../LoadingScreen.h"
#include "../VehicleRecording.h"
#include "../Collision/ColStore.h"
#include "../EntryExitManager.h"
#include "../X87Math.h"
#include "game_sa/Crt/CrtState.h"
#include "game_sa/Crt/CrtMath.h"
#include "game_sa/Crt/CrtFile.h"
#include "../PathFind.h"
#include "../Collision/Collision.h"
#include "../Scripts/TheScripts.h"
#include "UpsideDownCarCheck.h"
#include "Lines.h"
#include "FireManager.h"
#include "PedGroups.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "LoadingScreen.h"
#include "../Scripts/Scripted2dEffects.h"
#include "Shadows.h"
#include "VehicleRecording.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexWander.h"
#include "extensions/File.hpp"
#include "X87Math.h"
#include "../Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../Audio/AEAudioUtility.h"
#include "AEAudioUtility.h"
#include "../Audio/Entities/AEGlobalWeaponAudioEntity.h"
#include "AEAudioHardware.h"
#include "../Entity/Physical.h"
#include "CarCtrl.h"
#include "Glass.h"
#include "TaskSimpleClimb.h"
#include "RealTimeShadowManager.h"
#include "../MenuSystem.h"
#include "../Interior/Interior_c.h"
#include "../Interior/InteriorGroup_c.h"
#include "../Interior/Furniture_c.h"
#include "../Interior/FurnitureManager_c.h"
#include "../Interior/InteriorManager_c.h"
#include "Building.h"
#include "../ClothesBuilder.h"
#include "../PedClothesDesc.h"
#include "../Plugins/JPegCompressPlugin/LibJpegExports.h"
#include "jpeglib.h"
#include "../Plugins/JPegCompressPlugin/LibJpegCallback.h"
#include "../Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "CustomCarEnvMapPipeline.h"
#include "../PedPlacement.h"
#include "../Scripts/RunningScript.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "SearchLight.h"
#include "CutsceneMgr.h"
#include "World.h"
#include "Weapon.h"
#include "Ragdoll/IKChainManager.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Train.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Radar.h"
#include "Camera.h"
#include "GameLogic.h"
#include "Game.h"
#include "Streaming.h"
#include "TagManager.h"
#include "PathFind.h"
#include "StuckCarCheck.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetTempAction.h"
#include "Tasks/TaskTypes/TaskComplexCarDriveMission.h"
#include "Tasks/TaskTypes/TaskComplexArrestPed.h"
#include "Tasks/TaskTypes/TaskSimpleRunAnim.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h"
#include "Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "Tasks/TaskTypes/TaskSimpleTriggerLookAt.h"
#include "Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.h"
#include "Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h"
#include "Tasks/TaskTypes/TaskComplexDrivePointRoute.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAiming.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h"
#include "Entity/Vehicle/Plane.h"
#include "Clock.h"
#include "Pad.h"
#include "PedType.h"
#include "TheZones.h"
#include "Darkel.h"
#include "EntryExitManager.h"
#include "CarAI.h"
#include "StuntJumpManager.h"
#include "Ropes.h"
#include "WaterLevel.h"
#include "ModelIndices.h"
#include "Text/Text.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Collision/BoundingBox.h"
#include "Collision/ColModel.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"
#include "Tasks/TaskTypes/TaskSimpleSlideToCoord.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexFollowPatrolRoute.h"
#include "../Entity/Vehicle/Bike.h"
#include "Buoyancy.h"
#include "Events/EventKnockOffBike.h"
#include "Events/EventGroup.h"
#include "CullZones.h"
#include "Stats.h"
#include "SurfaceInfos_c.h"
#include "RwHelper.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Events/EventVehicleOnFire.h"
#include "Events/EventGlobalGroup.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendFrameData.h"
#include "../cTransmission.h"
#include "../Entity/Vehicle/Vehicle.h"
#include "Garages.h"
#include "CustomCarPlateMgr.h"
#include "VehicleSaveStructure.h"
#include "RideAnims.h"
#include "Rope.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "Coronas.h"
#include "Collision/Collision.h"
#include "Weather.h"
#include "CustomBuildingDNPipeline.h"
#include "PedClothesDesc.h"
#include "Skidmarks.h"
#include "PlayerPed.h"
#include "WeaponInfo.h"
#include "../StencilShadowObject.h"
#include "../World.h"
#include "../Pools/Pools.h"
#include "../Pools/VehiclePool.h"
#include "../Font.h"
#include "eLanguage.h"
#include "../3dMarkers.h"

#include "DataBssF.h"
#include "Crt/CrtOnexit.h"

namespace DatosExe::Bss {
namespace {
template <class T> struct Slot {
    alignas(16) unsigned char raw[sizeof(T)]{};
    T& ref() { return *std::launder(reinterpret_cast<T*>(raw)); }
};
template <class T> void Build(Slot<T>& s, void (__cdecl* dtor)()) {
    std::construct_at(reinterpret_cast<T*>(s.raw));
    if constexpr (!std::is_trivially_destructible_v<T>) {
        ExeCrt::Onexit(reinterpret_cast<void*>(dtor));
    }
}
}  // namespace

static Slot<HANDLE> sl_8E4004; // 0x008E4004, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(HANDLE) <= 4, "TAM 0x008E4004");
static void __cdecl dt_8E4004() { std::destroy_at(&sl_8E4004.ref()); }
using Ty_8E4004 = HANDLE;
template <> Ty_8E4004& b8E4004<Ty_8E4004>() { return sl_8E4004.ref(); }
static Slot<HANDLE> sl_8E4008; // 0x008E4008, hueco hasta el siguiente objeto conocido: 8 bytes
static_assert(sizeof(HANDLE) <= 8, "TAM 0x008E4008");
static void __cdecl dt_8E4008() { std::destroy_at(&sl_8E4008.ref()); }
using Ty_8E4008 = HANDLE;
template <> Ty_8E4008& b8E4008<Ty_8E4008>() { return sl_8E4008.ref(); }
static Slot<RwStream> sl_8E48AC; // 0x008E48AC, hueco hasta el siguiente objeto conocido: 36 bytes
static_assert(sizeof(RwStream) <= 36, "TAM 0x008E48AC");
static void __cdecl dt_8E48AC() { std::destroy_at(&sl_8E48AC.ref()); }
using Ty_8E48AC = RwStream;
template <> Ty_8E48AC& b8E48AC<Ty_8E48AC>() { return sl_8E48AC.ref(); }
static Slot<CColPool*> sl_965560; // 0x00965560, hueco hasta el siguiente objeto conocido: 8 bytes
static_assert(sizeof(CColPool*) <= 8, "TAM 0x00965560");
static void __cdecl dt_965560() { std::destroy_at(&sl_965560.ref()); }
using Ty_965560 = CColPool*;
template <> Ty_965560& b965560<Ty_965560>() { return sl_965560.ref(); }
static Slot<CEntryExitsPool*> sl_96A7D8; // 0x0096A7D8, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(CEntryExitsPool*) <= 4, "TAM 0x0096A7D8");
static void __cdecl dt_96A7D8() { std::destroy_at(&sl_96A7D8.ref()); }
using Ty_96A7D8 = CEntryExitsPool*;
template <> Ty_96A7D8& b96A7D8<Ty_96A7D8>() { return sl_96A7D8.ref(); }
static Slot<std::array<int32, NUM_PATH_INTERIOR_AREAS>> sl_96EA98; // 0x0096EA98, hueco hasta el siguiente objeto conocido: 32 bytes
static_assert(sizeof(std::array<int32, NUM_PATH_INTERIOR_AREAS>) <= 32, "TAM 0x0096EA98");
static void __cdecl dt_96EA98() { std::destroy_at(&sl_96EA98.ref()); }
using Ty_96EA98 = std::array<int32, NUM_PATH_INTERIOR_AREAS>;
template <> Ty_96EA98& b96EA98<Ty_96EA98>() { return sl_96EA98.ref(); }
static Slot<std::array<bool, NUM_PATH_MAP_AREAS>> sl_96EF90; // 0x0096EF90, hueco hasta el siguiente objeto conocido: 64 bytes
static_assert(sizeof(std::array<bool, NUM_PATH_MAP_AREAS>) <= 64, "TAM 0x0096EF90");
static void __cdecl dt_96EF90() { std::destroy_at(&sl_96EF90.ref()); }
using Ty_96EF90 = std::array<bool, NUM_PATH_MAP_AREAS>;
template <> Ty_96EF90& b96EF90<Ty_96EF90>() { return sl_96EF90.ref(); }
static Slot<std::array<bool, NUM_PATH_MAP_AREAS>> sl_96EFD0; // 0x0096EFD0, hueco hasta el siguiente objeto conocido: 64 bytes
static_assert(sizeof(std::array<bool, NUM_PATH_MAP_AREAS>) <= 64, "TAM 0x0096EFD0");
static void __cdecl dt_96EFD0() { std::destroy_at(&sl_96EFD0.ref()); }
using Ty_96EFD0 = std::array<bool, NUM_PATH_MAP_AREAS>;
template <> Ty_96EFD0& b96EFD0<Ty_96EFD0>() { return sl_96EFD0.ref(); }
static Slot<std::array<CRunningScript, MAX_NUM_SCRIPTS>> sl_A8B430; // 0x00A8B430, hueco hasta el siguiente objeto conocido: 21504 bytes
static_assert(sizeof(std::array<CRunningScript, MAX_NUM_SCRIPTS>) <= 21504, "TAM 0x00A8B430");
static void __cdecl dt_A8B430() { std::destroy_at(&sl_A8B430.ref()); }
using Ty_A8B430 = std::array<CRunningScript, MAX_NUM_SCRIPTS>;
template <> Ty_A8B430& bA8B430<Ty_A8B430>() { return sl_A8B430.ref(); }
static Slot<std::array<CColPoint, 32>> sl_A951F8; // 0x00A951F8, hueco hasta el siguiente objeto conocido: 1408 bytes
static_assert(sizeof(std::array<CColPoint, 32>) <= 1408, "TAM 0x00A951F8");
static void __cdecl dt_A951F8() { std::destroy_at(&sl_A951F8.ref()); }
using Ty_A951F8 = std::array<CColPoint, 32>;
template <> Ty_A951F8& bA951F8<Ty_A951F8>() { return sl_A951F8.ref(); }
static Slot<AnimBlendFrameData*> sl_B5F87C; // 0x00B5F87C, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(AnimBlendFrameData*) <= 4, "TAM 0x00B5F87C");
static void __cdecl dt_B5F87C() { std::destroy_at(&sl_B5F87C.ref()); }
using Ty_B5F87C = AnimBlendFrameData*;
template <> Ty_B5F87C& bB5F87C<Ty_B5F87C>() { return sl_B5F87C.ref(); }
static Slot<LARGE_INTEGER> sl_B610F0; // 0x00B610F0, hueco hasta el siguiente objeto conocido: 8 bytes
static_assert(sizeof(LARGE_INTEGER) <= 8, "TAM 0x00B610F0");
static void __cdecl dt_B610F0() { std::destroy_at(&sl_B610F0.ref()); }
using Ty_B610F0 = LARGE_INTEGER;
template <> Ty_B610F0& bB610F0<Ty_B610F0>() { return sl_B610F0.ref(); }
static Slot<eWeaponType> sl_B6131C; // 0x00B6131C, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(eWeaponType) <= 4, "TAM 0x00B6131C");
static void __cdecl dt_B6131C() { std::destroy_at(&sl_B6131C.ref()); }
using Ty_B6131C = eWeaponType;
template <> Ty_B6131C& bB6131C<Ty_B6131C>() { return sl_B6131C.ref(); }
static Slot<std::array<CColPoint, 32>> sl_B73C98; // 0x00B73C98, hueco hasta el siguiente objeto conocido: 1408 bytes
static_assert(sizeof(std::array<CColPoint, 32>) <= 1408, "TAM 0x00B73C98");
static void __cdecl dt_B73C98() { std::destroy_at(&sl_B73C98.ref()); }
using Ty_B73C98 = std::array<CColPoint, 32>;
template <> Ty_B73C98& bB73C98<Ty_B73C98>() { return sl_B73C98.ref(); }
static Slot<std::array<CMenuSystem::Menu*, 2>> sl_BA82D8; // 0x00BA82D8, hueco hasta el siguiente objeto conocido: 8 bytes
static_assert(sizeof(std::array<CMenuSystem::Menu*, 2>) <= 8, "TAM 0x00BA82D8");
static void __cdecl dt_BA82D8() { std::destroy_at(&sl_BA82D8.ref()); }
using Ty_BA82D8 = std::array<CMenuSystem::Menu*, 2>;
template <> Ty_BA82D8& bBA82D8<Ty_BA82D8>() { return sl_BA82D8.ref(); }
static Slot<eRadarTraceHeight> sl_BAA350; // 0x00BAA350, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(eRadarTraceHeight) <= 4, "TAM 0x00BAA350");
static void __cdecl dt_BAA350() { std::destroy_at(&sl_BAA350.ref()); }
using Ty_BAA350 = eRadarTraceHeight;
template <> Ty_BAA350& bBAA350<Ty_BAA350>() { return sl_BAA350.ref(); }
static Slot<TList_c<FurnitureEntity_c>> sl_BAD3EC; // 0x00BAD3EC, hueco hasta el siguiente objeto conocido: 12 bytes
static_assert(sizeof(TList_c<FurnitureEntity_c>) <= 12, "TAM 0x00BAD3EC");
static void __cdecl dt_BAD3EC() { std::destroy_at(&sl_BAD3EC.ref()); }
using Ty_BAD3EC = TList_c<FurnitureEntity_c>;
template <> Ty_BAD3EC& bBAD3EC<Ty_BAD3EC>() { return sl_BAD3EC.ref(); }
static Slot<CDirectory::DirectoryInfo> sl_BBCDC8; // 0x00BBCDC8, hueco hasta el siguiente objeto conocido: 17600 bytes
static_assert(sizeof(CDirectory::DirectoryInfo) <= 17600, "TAM 0x00BBCDC8");
static void __cdecl dt_BBCDC8() { std::destroy_at(&sl_BBCDC8.ref()); }
using Ty_BBCDC8 = CDirectory::DirectoryInfo;
template <> Ty_BBCDC8& bBBCDC8<Ty_BBCDC8>() { return sl_BBCDC8.ref(); }
static Slot<FILESTREAM> sl_BD0B74; // 0x00BD0B74, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(FILESTREAM) <= 4, "TAM 0x00BD0B74");
static void __cdecl dt_BD0B74() { std::destroy_at(&sl_BD0B74.ref()); }
using Ty_BD0B74 = FILESTREAM;
template <> Ty_BD0B74& bBD0B74<Ty_BD0B74>() { return sl_BD0B74.ref(); }
static Slot<std::array<uint8, 204'800>> sl_BD0B78; // 0x00BD0B78, hueco hasta el siguiente objeto conocido: 204800 bytes
static_assert(sizeof(std::array<uint8, 204'800>) <= 204800, "TAM 0x00BD0B78");
static void __cdecl dt_BD0B78() { std::destroy_at(&sl_BD0B78.ref()); }
using Ty_BD0B78 = std::array<uint8, 204'800>;
template <> Ty_BD0B78& bBD0B78<Ty_BD0B78>() { return sl_BD0B78.ref(); }
static Slot<D3DMATRIX> sl_C02C28; // 0x00C02C28, hueco hasta el siguiente objeto conocido: 64 bytes
static_assert(sizeof(D3DMATRIX) <= 64, "TAM 0x00C02C28");
static void __cdecl dt_C02C28() { std::destroy_at(&sl_C02C28.ref()); }
using Ty_C02C28 = D3DMATRIX;
template <> Ty_C02C28& bC02C28<Ty_C02C28>() { return sl_C02C28.ref(); }
static Slot<std::array<CColPoint, 32>> sl_C102A0; // 0x00C102A0, hueco hasta el siguiente objeto conocido: 1408 bytes
static_assert(sizeof(std::array<CColPoint, 32>) <= 1408, "TAM 0x00C102A0");
static void __cdecl dt_C102A0() { std::destroy_at(&sl_C102A0.ref()); }
using Ty_C102A0 = std::array<CColPoint, 32>;
template <> Ty_C102A0& bC102A0<Ty_C102A0>() { return sl_C102A0.ref(); }
static Slot<CPointRoute> sl_C18D50; // 0x00C18D50, hueco hasta el siguiente objeto conocido: 100 bytes
static_assert(sizeof(CPointRoute) <= 100, "TAM 0x00C18D50");
static void __cdecl dt_C18D50() { std::destroy_at(&sl_C18D50.ref()); }
using Ty_C18D50 = CPointRoute;
template <> Ty_C18D50& bC18D50<Ty_C18D50>() { return sl_C18D50.ref(); }
static Slot<tWheelState> sl_C1C26C; // 0x00C1C26C, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(tWheelState) <= 4, "TAM 0x00C1C26C");
static void __cdecl dt_C1C26C() { std::destroy_at(&sl_C1C26C.ref()); }
using Ty_C1C26C = tWheelState;
template <> Ty_C1C26C& bC1C26C<Ty_C1C26C>() { return sl_C1C26C.ref(); }
static Slot<tWheelState> sl_C1C270; // 0x00C1C270, hueco hasta el siguiente objeto conocido: 12 bytes
static_assert(sizeof(tWheelState) <= 12, "TAM 0x00C1C270");
static void __cdecl dt_C1C270() { std::destroy_at(&sl_C1C270.ref()); }
using Ty_C1C270 = tWheelState;
template <> Ty_C1C270& bC1C270<Ty_C1C270>() { return sl_C1C270.ref(); }
static Slot<tTransmissionGear*> sl_C1CB30; // 0x00C1CB30, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(tTransmissionGear*) <= 4, "TAM 0x00C1CB30");
static void __cdecl dt_C1CB30() { std::destroy_at(&sl_C1CB30.ref()); }
using Ty_C1CB30 = tTransmissionGear*;
template <> Ty_C1CB30& bC1CB30<Ty_C1CB30>() { return sl_C1CB30.ref(); }
static Slot<tTransmissionGear*> sl_C1CB34; // 0x00C1CB34, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(tTransmissionGear*) <= 4, "TAM 0x00C1CB34");
static void __cdecl dt_C1CB34() { std::destroy_at(&sl_C1CB34.ref()); }
using Ty_C1CB34 = tTransmissionGear*;
template <> Ty_C1CB34& bC1CB34<Ty_C1CB34>() { return sl_C1CB34.ref(); }
static Slot<CColSphere> sl_C1CD98; // 0x00C1CD98, hueco hasta el siguiente objeto conocido: 20 bytes
static_assert(sizeof(CColSphere) <= 20, "TAM 0x00C1CD98");
static void __cdecl dt_C1CD98() { std::destroy_at(&sl_C1CD98.ref()); }
using Ty_C1CD98 = CColSphere;
template <> Ty_C1CD98& bC1CD98<Ty_C1CD98>() { return sl_C1CD98.ref(); }
static Slot<RxVertexIndex*> sl_C6A170; // 0x00C6A170, hueco hasta el siguiente objeto conocido: 4 bytes
static_assert(sizeof(RxVertexIndex*) <= 4, "TAM 0x00C6A170");
static void __cdecl dt_C6A170() { std::destroy_at(&sl_C6A170.ref()); }
using Ty_C6A170 = RxVertexIndex*;
template <> Ty_C6A170& bC6A170<Ty_C6A170>() { return sl_C6A170.ref(); }
static Slot<std::array<tFontData, 2>> sl_C718B0; // 0x00C718B0, hueco hasta el siguiente objeto conocido: 420 bytes
static_assert(sizeof(std::array<tFontData, 2>) <= 420, "TAM 0x00C718B0");
static void __cdecl dt_C718B0() { std::destroy_at(&sl_C718B0.ref()); }
using Ty_C718B0 = std::array<tFontData, 2>;
template <> Ty_C718B0& bC718B0<Ty_C718B0>() { return sl_C718B0.ref(); }
static Slot<RwRGBAReal> sl_C80434; // 0x00C80434, hueco hasta el siguiente objeto conocido: 16 bytes
static_assert(sizeof(RwRGBAReal) <= 16, "TAM 0x00C80434");
static void __cdecl dt_C80434() { std::destroy_at(&sl_C80434.ref()); }
using Ty_C80434 = RwRGBAReal;
template <> Ty_C80434& bC80434<Ty_C80434>() { return sl_C80434.ref(); }
static Slot<RwRGBAReal> sl_C80444; // 0x00C80444, hueco hasta el siguiente objeto conocido: 20 bytes
static_assert(sizeof(RwRGBAReal) <= 20, "TAM 0x00C80444");
static void __cdecl dt_C80444() { std::destroy_at(&sl_C80444.ref()); }
using Ty_C80444 = RwRGBAReal;
template <> Ty_C80444& bC80444<Ty_C80444>() { return sl_C80444.ref(); }

void ConstructF() {
    Build(sl_8E4004, &dt_8E4004);
    Build(sl_8E4008, &dt_8E4008);
    Build(sl_8E48AC, &dt_8E48AC);
    Build(sl_965560, &dt_965560);
    Build(sl_96A7D8, &dt_96A7D8);
    Build(sl_96EA98, &dt_96EA98);
    Build(sl_96EF90, &dt_96EF90);
    Build(sl_96EFD0, &dt_96EFD0);
    Build(sl_A8B430, &dt_A8B430);
    Build(sl_A951F8, &dt_A951F8);
    Build(sl_B5F87C, &dt_B5F87C);
    Build(sl_B610F0, &dt_B610F0);
    Build(sl_B6131C, &dt_B6131C);
    Build(sl_B73C98, &dt_B73C98);
    Build(sl_BA82D8, &dt_BA82D8);
    Build(sl_BAA350, &dt_BAA350);
    Build(sl_BAD3EC, &dt_BAD3EC);
    Build(sl_BBCDC8, &dt_BBCDC8);
    Build(sl_BD0B74, &dt_BD0B74);
    Build(sl_BD0B78, &dt_BD0B78);
    Build(sl_C02C28, &dt_C02C28);
    Build(sl_C102A0, &dt_C102A0);
    Build(sl_C18D50, &dt_C18D50);
    Build(sl_C1C26C, &dt_C1C26C);
    Build(sl_C1C270, &dt_C1C270);
    Build(sl_C1CB30, &dt_C1CB30);
    Build(sl_C1CB34, &dt_C1CB34);
    Build(sl_C1CD98, &dt_C1CD98);
    Build(sl_C6A170, &dt_C6A170);
    Build(sl_C718B0, &dt_C718B0);
    Build(sl_C80434, &dt_C80434);
    Build(sl_C80444, &dt_C80444);
}

namespace {
const bool kConstructF = (ConstructF(), true);
}

} // namespace DatosExe::Bss
