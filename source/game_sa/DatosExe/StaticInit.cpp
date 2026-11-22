#include "StdInc.h"
#include "../Animation/AnimBlendAssocGroup.h"
#include "../Animation/AnimBlendHierarchy.h"
#include "../Audio/AESmoothFadeThread.h"
#include "../Audio/AudioEngine.h"
#include "../Audio/Entities/AEExplosionAudioEntity.h"
#include "../Audio/Entities/AETwinLoopSoundEntity.h"
#include "../Audio/Entities/AEWeatherAudioEntity.h"
#include "../Audio/Hardware/AEAudioHardware.h"
#include "../Audio/Loaders/AEStreamTransformer.h"
#include "../Audio/Managers/AESoundManager.h"
#include "../BreakManager_c.h"
#include "../Buoyancy.h"
#include "../Camera.h"
#include "../Collision/ColModel.h"
#include "../Collision/CollisionData.h"
#include "../ControllerConfigManager.h"
#include "../Core/Matrix.h"
#include "../Core/MatrixLinkList.h"
#include "../Core/OctTreeBase.h"
#include "../Core/PtrNodeDoubleLink.h"
#include "../Core/PtrNodeSingleLink.h"
#include "../Directory.h"
#include "../Escalator.h"
#include "../FireManager.h"
#include "../Fx/Fx.h"
#include "../Fx/FxManager.h"
#include "../GangInfo.h"
#include "../Garage.h"
#include "../HandShaker.h"
#include "../HudColours.h"
#include "../IdleCam.h"
#include "../InformFriendsEventQueue.h"
#include "../InformGroupEvent.h"
#include "../InterestingEvents.h"
#include "../Interior/FurnitureManager_c.h"
#include "../Interior/FurnitureSubGroup_c.h"
#include "../Interior/Furniture_c.h"
#include "../Interior/InteriorManager_c.h"
#include "../LoadMonitor.h"
#include "../Memory/MemoryHeap.h"
#include "../MenuManager.h"
#include "../Models/ModelInfo.h"
#include "../PatrolRoute.h"
#include "../PedClothesDesc.h"
#include "../PedGroup.h"
#include "../PhoneInfo.h"
#include "../Plant/ProcObjectMan.h"
#include "../PlayerInfo.h"
#include "../RGBA.h"
#include "../Ragdoll/BoneNodeManager.h"
#include "../Ragdoll/IKChainManager.h"
#include "../RealTimeShadowManager.h"
#include "../Scripts/Scripted2dEffects.h"
#include "../ScriptsForBrains.h"
#include "../Sector.h"
#include "../Sprite2d.h"
#include "../StencilShadowObject.h"
#include "../Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"
#include "../Tasks/PedScriptedTaskRecord.h"
#include "../Tasks/ScriptedBrainTaskStore.h"
#include "../Tasks/TaskComplexSequence.h"
#include "../Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h"
#include "../Text/Text.h"
#include "../VehicleAnimGroupData.h"
#include "../WaterCannon.h"
#include "../WaterCreatureManager_c.h"

#include "StaticInit.h"
#include "Crt/CrtOnexit.h"

#include <cstring>
#include <format>
#include <memory>
#include <vector>

// Generado por contrib/datos_exe_staticinit.py; no editar a mano (ver docs/DatosExe/inicializadores-estado.csv).

void DWHeliChaseParamsConstructExe(void* element);                                                           // Cam.cpp
void ModelInfoStoreConstructStatic(uint32 which);                                                            // Models/ModelInfo.cpp
void ModelInfoStoreDestructStatic(uint32 which);                                                             // Models/ModelInfo.cpp
void __fastcall SectorDestructorExe(CSector* self, void* edx);                                               // Sector.cpp (0x563130)
COctTreeBase* __fastcall OctTreeBaseConstructorExe(COctTreeBase* self, void* edx);                           // Core/OctTreeBase.cpp (0x5A7570)
CAEWeatherAudioEntity* __fastcall WeatherAudioEntityConstructorBody(CAEWeatherAudioEntity* self, void* edx); // Audio/Entities/AEWeatherAudioEntity.cpp (0x72A620)
namespace CInformFriendsEventBody { void __fastcall Destructor(CInformFriendsEvent* self, void* edx); }      // InformFriendsEventQueue.cpp (0x4B2950)

namespace DatosExe {
namespace {
template <class T, uintptr_t VA> void __cdecl DestroyAt() { std::destroy_at(&StaticRef<T>(VA)); }
template <class T, uintptr_t VA, size_t N> void __cdecl DestroyArrayAt() {
    for (size_t i = N; i-- > 0;) {
        std::destroy_at(&StaticRef<T>(VA) + i);
    }
}
template <class T, uintptr_t VA> void ConstructAtNoDtor() { std::construct_at(&StaticRef<T>(VA)); }
template <class T, uintptr_t VA> void RegisterDtor() {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        ExeCrt::Onexit(reinterpret_cast<void*>(&DestroyAt<T, VA>));
    }
}
template <class T, uintptr_t VA> void ConstructAt() {
    ConstructAtNoDtor<T, VA>();
    RegisterDtor<T, VA>();
}
template <class T, uintptr_t VA, size_t N, size_t SZ> void ConstructArrayAt(bool withDtor) {
    static_assert(sizeof(T) == SZ, "tamano de elemento distinto del EXE");
    for (size_t i = 0; i < N; ++i) {
        std::construct_at(&StaticRef<T>(VA) + i);
    }
    if (withDtor && !std::is_trivially_destructible_v<T>) {
        ExeCrt::Onexit(reinterpret_cast<void*>(&DestroyArrayAt<T, VA, N>));
    }
}
template <class V> void StoreRun(uintptr_t addr, size_t count, size_t stride, V value) {
    for (size_t i = 0; i < count; ++i) {
        StaticRef<V>(addr + i * stride) = value;
    }
}
template <class V> void CopyValue(uintptr_t dst, uintptr_t src) { StaticRef<V>(dst) = StaticRef<V>(src); }
// Filas escritas a mano (contrib/datos_exe_staticinit_nuevas.py). El destructor se llama SIN despacho virtual (`T::~T()`): los objetos pueden llevar la vtable del EXE.
template <class T, uintptr_t VA, size_t N> void ConstructArrayOnly() {
    for (size_t i = 0; i < N; ++i) {
        std::construct_at(&StaticRef<T>(VA) + i);
    }
}
template <class T, uintptr_t VA, size_t N> void __cdecl DestroyArrayNV() {
    for (size_t i = N; i-- > 0;) {
        (&StaticRef<T>(VA) + i)->T::~T();
    }
}
template <class T, uintptr_t VA, size_t N> void OnexitArrayNV() {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        ExeCrt::Onexit(reinterpret_cast<void*>(&DestroyArrayNV<T, VA, N>));
    }
}
template <class T, uintptr_t VA> void __cdecl DestroyNV() { (&StaticRef<T>(VA))->T::~T(); }
template <class T, uintptr_t VA> void OnexitNV() {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        ExeCrt::Onexit(reinterpret_cast<void*>(&DestroyNV<T, VA>));
    }
}
void OnexitFn(void(__cdecl* fn)()) { ExeCrt::Onexit(reinterpret_cast<void*>(fn)); }
using Thunk = void (*)();
struct Row {
    int       index;     // indice C++ de la tabla _initterm (docs/DatosExe/inicializadores.csv)
    uintptr_t va;        // direccion del objeto (primer elemento)
    size_t    size;      // bytes que ocupa lo que escribe la rutina
    Thunk     construct; // el constructor; nullptr si solo escribe ceros sobre .bss
    Thunk     onexit;    // registra el destructor con el atexit; nullptr si no hay efecto
};
}  // namespace

const Row kRows[] = {
    { 112, 0x96A9B8, 13 * 28, nullptr, nullptr },
    { 125, 0x96C048, 50 * 216, [] { ConstructArrayOnly<CGarage, 0x96C048, 50>(); }, [] { OnexitArrayNV<CGarage, 0x96C048, 50>(); } },
    { 195, 0xA913E8, 96 * 0x44, nullptr, nullptr },
    { 196, 0xA92D68, 128 * 0x3C, nullptr, nullptr },
    { 345, 0xA9AFB8, 8 * 12, [] { ConstructArrayOnly<CInformFriendsEvent, 0xA9AFB8, 8>(); }, [] { OnexitFn(+[] { for (size_t i = 8; i-- > 0;) { CInformFriendsEventBody::Destructor(&StaticRef<CInformFriendsEvent>(0xA9AFB8) + i, nullptr); } }); } },
    { 358, 0xAAE950, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_atomicModelInfoStore)>), [] { ModelInfoStoreConstructStatic(0); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(0); }); } },
    { 359, 0xB1BF58, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_damageAtomicModelInfoStore)>), [] { ModelInfoStoreConstructStatic(1); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(1); }); } },
    { 360, 0xB1C934, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_lodAtomicModelInfoStore)>), [] { ModelInfoStoreConstructStatic(2); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(2); }); } },
    { 361, 0xB1C960, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_timeModelInfoStore)>), [] { ModelInfoStoreConstructStatic(3); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(3); }); } },
    { 362, 0xB1E128, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_lodTimeModelInfoStore)>), [] { ModelInfoStoreConstructStatic(4); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(4); }); } },
    { 363, 0xB1E158, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_weaponModelInfoStore)>), [] { ModelInfoStoreConstructStatic(5); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(5); }); } },
    { 364, 0xB1E958, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_clumpModelInfoStore)>), [] { ModelInfoStoreConstructStatic(6); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(6); }); } },
    { 365, 0xB1F650, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_vehicleModelInfoStore)>), [] { ModelInfoStoreConstructStatic(7); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(7); }); } },
    { 366, 0xB478F8, sizeof(std::remove_reference_t<decltype(CModelInfo::ms_pedModelInfoStore)>), [] { ModelInfoStoreConstructStatic(8); }, [] { OnexitFn(+[] { ModelInfoStoreDestructStatic(8); }); } },
    { 431, 0xB612D8, sizeof(CAEStreamTransformer), [] { StaticRef<CAEStreamTransformer>(0xB612D8).Initialise(); }, nullptr },
    { 468, 0xB61C38, 6 * 28, nullptr, nullptr },
    { 518, 0xB6BA54, 4, [] { StaticRef<float>(0xB6BA54) = static_cast<float>(static_cast<double>(StaticRef<float>(0xB6BA4C)) / static_cast<double>(StaticRef<int16>(0x8CBC8C))); }, nullptr },
    { 559, 0xB6BB18, sizeof(CAETwinLoopSoundEntity), [] { StaticRef<CAETwinLoopSoundEntity>(0xB6BB18).DefaultConstructorExe(); }, [] { OnexitNV<CAETwinLoopSoundEntity, 0xB6BB18>(); } },
    { 560, 0xB6BBC0, sizeof(CAETwinLoopSoundEntity), [] { StaticRef<CAETwinLoopSoundEntity>(0xB6BBC0).DefaultConstructorExe(); }, [] { OnexitNV<CAETwinLoopSoundEntity, 0xB6BBC0>(); } },
    { 577, 0xB6FDA0, sizeof(CIdleCam), [] { StaticRef<CIdleCam>(0xB6FDA0).Init(); }, nullptr },
    { 580, 0xB6FEC0, 0x9C, [] { DWHeliChaseParamsConstructExe(&StaticRef<uint8>(0xB6FEC0)); }, nullptr },
    { 665, 0xB73458, 2 * 0x134, nullptr, nullptr },
    { 672, 0xB74240, sizeof(CMatrix), [] { StaticRef<CMatrix>(0xB74240).SetScale(1.0f); }, [] { OnexitNV<CMatrix, 0xB74240>(); } },
    { 727, 0xB7D0B8, 14400 * 8, nullptr, [] { OnexitFn(+[] { for (size_t i = 14400; i-- > 0;) { SectorDestructorExe(&StaticRef<CSector>(0xB7D0B8) + i, nullptr); } }); } },
    { 728, 0xB992B8, 256 * 12, nullptr, [] { OnexitFn(+[] { for (size_t i = 256; i-- > 0;) { for (size_t j = 3; j-- > 0;) { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xB992B8 + i * 12 + j * 4), nullptr); } } }); } },
    { 729, 0xB99EB8, 900 * 4, nullptr, [] { OnexitFn(+[] { for (size_t i = 900; i-- > 0;) { details::CPtrListSingleLink__FlushExe(reinterpret_cast<void*>(0xB99EB8 + i * 4), nullptr); } }); } },
    { 730, 0xB9ACC8, 4, nullptr, [] { OnexitFn(+[] { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xB9ACC8), nullptr); }); } },
    { 731, 0xB9ACCC, 4, nullptr, [] { OnexitFn(+[] { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xB9ACCC), nullptr); }); } },
    { 782, 0xBA86D4, 6 * 4, nullptr, nullptr },
    { 887, 0xBC1290, sizeof(COctTreeBase), [] { OctTreeBaseConstructorExe(&StaticRef<COctTreeBase>(0xBC1290), nullptr); }, [] { OnexitNV<COctTreeBase, 0xBC1290>(); } },
    { 888, 0xBC12C0, sizeof(CDirectory), [] { std::construct_at(&StaticRef<CDirectory>(0xBC12C0)); }, [] { OnexitNV<CDirectory, 0xBC12C0>(); } },
    { 975, 0xC03A48, 256 * 0x54, nullptr, nullptr },
    { 976, 0xC08E48, 40 * 0x14, nullptr, nullptr },
    { 1003, 0xC091F0, 10 * 16, [] { ConstructArrayOnly<CGangInfo, 0xC091F0, 10>(); }, nullptr },
    { 1048, 0xC09908, 4, [] { PedGroupDefaultTaskAllocatorFollowAnyMeansConstructorExe(&StaticRef<CPedGroupDefaultTaskAllocatorFollowAnyMeans>(0xC09908), nullptr); }, nullptr },
    { 1049, 0xC0990C, 4, [] { StaticRef<CPedGroupDefaultTaskAllocatorFollowLimited>(0xC0990C).Constructor(); }, nullptr },
    { 1050, 0xC09910, 4, [] { StaticRef<CPedGroupDefaultTaskAllocatorStandStill>(0xC09910).Constructor(); }, nullptr },
    { 1051, 0xC09914, 4, [] { StaticRef<CPedGroupDefaultTaskAllocatorChat>(0xC09914).Constructor(); }, nullptr },
    { 1052, 0xC09918, 4, [] { StaticRef<CPedGroupDefaultTaskAllocatorRandom>(0xC09918).Constructor(); }, nullptr },
    { 1053, 0xC0991C, 4, [] { StaticRef<CPedGroupDefaultTaskAllocatorSitInLeaderCar>(0xC0991C).Constructor(); }, nullptr },
    { 1163, 0xC178F0, 64 * 0x40, [] { ConstructArrayOnly<CTaskComplexSequence, 0xC178F0, 64>(); }, [] { OnexitArrayNV<CTaskComplexSequence, 0xC178F0, 64>(); } },
    { 1279, 0xC196E8, sizeof(CTaskComplexFollowLeaderInFormation::Offsets), [] { StaticRef<CTaskComplexFollowLeaderInFormation::Offsets>(0xC196E8).Constructor(); }, nullptr },
    { 1298, 0xC1A2B8, 4, nullptr, [] { OnexitFn(+[] { details::CPtrListDoubleLink__FlushExe(reinterpret_cast<void*>(0xC1A2B8), nullptr); }); } },
    { 1458, 0xC3A200, 64 * 0x24, [] { for (size_t i = 0; i < 64; ++i) { (&StaticRef<tUserList>(0xC3A200) + i)->Constructor(); } }, nullptr },
    { 1459, 0xC3BB00, 64 * 0x94, [] { for (size_t i = 0; i < 64; ++i) { (&StaticRef<CScriptedEffectPairs>(0xC3BB00) + i)->Constructor(); } }, nullptr },
    { 1532, 0xC71BF8, 45 * 0x70, nullptr, nullptr },
    { 1565, 0xC7DD58, 32 * 0xA0, nullptr, nullptr },
    { 1573, 0xC80740, 3 * 0x3CC, [] { ConstructArrayOnly<CWaterCannon, 0xC80740, 3>(); }, [] { OnexitArrayNV<CWaterCannon, 0xC80740, 3>(); } },
    { 1580, 0xC81360, sizeof(CAEWeatherAudioEntity), [] { WeatherAudioEntityConstructorBody(&StaticRef<CAEWeatherAudioEntity>(0xC81360), nullptr); }, [] { OnexitNV<CAEWeatherAudioEntity, 0xC81360>(); } },
    { 1591, 0xC87B40, sizeof(CMemoryHeap), nullptr, nullptr },
    { 1592, 0xC87B88, sizeof(CMemoryHeap), nullptr, nullptr },
    { 1593, 0xC87BD0, sizeof(CMemoryHeap), nullptr, nullptr },
    { 1620, 0xC888D0, sizeof(CAEExplosionAudioEntity), [] { StaticRef<CAEExplosionAudioEntity>(0xC888D0).Constructor(); }, [] { OnexitNV<CAEExplosionAudioEntity, 0xC888D0>(); } },
    { 1646, 0xC8A838, 2 * 0x2C, nullptr, nullptr },
    { 1653, 0xC8AAB8, 80 * 0x70, nullptr, nullptr },
};

void RunRow(int index) {
    for (const Row& r : kRows) {
        if (r.index != index) {
            continue;
        }
        if (r.construct) {
            r.construct();
        }
        if (r.onexit) {
            r.onexit();
        }
        return;
    }
}

void RunStaticInitializers() {
    // C++ 45 (0x00849470): CColModel en 0x00968A00
    ConstructAt<CColModel, 0x00968A00>();
    // C++ 46 (0x00849490): 20 x CColModel (48 bytes) en 0x00968A30
    ConstructArrayAt<CColModel, 0x00968A30, 20, 48>(true);
    // C++ 47 (0x008494C0): CColModel en 0x00968DF0
    ConstructAt<CColModel, 0x00968DF0>();
    // C++ 48 (0x008494E0): CColModel en 0x00968E20
    ConstructAt<CColModel, 0x00968E20>();
    // C++ 49 (0x00849500): CColModel en 0x00968E50
    ConstructAt<CColModel, 0x00968E50>();
    // C++ 50 (0x00849520): CColModel en 0x00968E80
    ConstructAt<CColModel, 0x00968E80>();
    // C++ 51 (0x00849540): CColModel en 0x00968EB0
    ConstructAt<CColModel, 0x00968EB0>();
    // C++ 52 (0x00849560): CColModel en 0x00968EE0
    ConstructAt<CColModel, 0x00968EE0>();
    // C++ 53 (0x00849580): CColModel en 0x00968F10
    ConstructAt<CColModel, 0x00968F10>();
    // C++ 54 (0x008495A0): CColModel en 0x00968F40
    ConstructAt<CColModel, 0x00968F40>();
    // C++ 55 (0x008495C0): CColModel en 0x00968F70
    ConstructAt<CColModel, 0x00968F70>();
    // C++ 56 (0x008495E0): CColModel en 0x00968FA0
    ConstructAt<CColModel, 0x00968FA0>();
    // C++ 57 (0x00849600): CColModel en 0x00968FD0
    ConstructAt<CColModel, 0x00968FD0>();
    // C++ 112 (0x00849CE0): CWeapon x 13 (GameLogic::SavedWeapons): ctor 0x441E00 solo escribe ceros (+0x14, +0x15, +0x18); dtor 0x4411A0 es un `ret`
    RunRow(112);
    // C++ 125 (0x00849E90): CGarage x 50 (CGarages::aGarages): ctor 0x4470E0 (vtable de CAEDoorAudioEntity en +0x50 y ceros) = el constructor por defecto de la clase; dtor 0x447110 = ~CGarage
    RunRow(125);
    // C++ 138 (0x0084A040): valores iniciales
    StoreRun<uint16>(0x972CD0, 5000, 4, 0xFFFF);
    // C++ 139 (0x0084A060): valores iniciales
    StoreRun<uint16>(0x977AF0, 32, 4, 0xFFFF);
    // C++ 140 (0x0084A080): valores iniciales
    StoreRun<uint16>(0x977B7C, 8, 4, 0xFFFF);
    // C++ 153 (0x0084A220): CPhoneInfo en 0x00977BE8 (destructor 0x00454200  no es el de la clase)
    ConstructAtNoDtor<CPhoneInfo, 0x00977BE8>();
    // C++ 178 (0x0084A540): CPlayerInfo en 0x00A430B0 (destructor 0x0045B110  no es el de la clase)
    ConstructAtNoDtor<CPlayerInfo, 0x00A430B0>();
    // C++ 186 (0x0084A640): valores iniciales
    StoreRun<uint16>(0xA435A0, 325, 4, 0xFFFF);
    // C++ 194 (0x0084A740): CScriptsForBrains en 0x00A90CF0 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CScriptsForBrains, 0x00A90CF0>();
    // C++ 195 (0x0084A750): tScriptText x 96 (IntroTextLines): ctor 0x463750 es `mov eax,ecx; ret` (el C++ propio SI inicializa miembros, el EXE no); dtor 0x463760 `ret`
    RunRow(195);
    // C++ 196 (0x0084A780): tScriptRectangle x 128 (IntroRectangles): ctor 0x463770 es `mov eax,ecx; ret`; dtor 0x463780 `ret`
    RunRow(196);
    // C++ 197 (0x0084A7B0): 128 x CSprite2d (4 bytes) en 0x00A94B68
    ConstructArrayAt<CSprite2d, 0x00A94B68, 128, 4>(true);
    // C++ 284 (0x0084B2A0): CPedClothesDesc en 0x00A9A810 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CPedClothesDesc, 0x00A9A810>();
    // C++ 310 (0x0084B5D0): Fx_c en 0x00A9AE00 (destructor 0x0049E630  no es el de la clase)
    ConstructAtNoDtor<Fx_c, 0x00A9AE00>();
    // C++ 314 (0x0084B650): FxManager_c en 0x00A9AE80
    ConstructAt<FxManager_c, 0x00A9AE80>();
    // C++ 345 (0x0084BA30): CInformFriendsEvent x 8: ctor 0x4AC0D0 (ped 0, evento 0, instante -1) = los inicializadores de miembro de la estructura; dtor 0x4B2950 (CInformFriendsEventBody::Destructor) en orden inverso
    RunRow(345);
    // C++ 346 (0x0084BA60): 8 x CInformGroupEvent (16 bytes) en 0x00A9B018
    ConstructArrayAt<CInformGroupEvent, 0x00A9B018, 8, 16>(true);
    // C++ 358 (0x0084BBF0): CModelInfo::ms_atomicModelInfoStore: constructor 0x4C5CB0 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C5840 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(358);
    // C++ 359 (0x0084BC10): CModelInfo::ms_damageAtomicModelInfoStore: constructor 0x4C5D10 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C5860 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(359);
    // C++ 360 (0x0084BC30): CModelInfo::ms_lodAtomicModelInfoStore: constructor 0x4C5D70 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C5880 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(360);
    // C++ 361 (0x0084BC50): CModelInfo::ms_timeModelInfoStore: constructor 0x4C5DD0 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C58A0 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(361);
    // C++ 362 (0x0084BC70): CModelInfo::ms_lodTimeModelInfoStore: constructor 0x4C5E30 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C58C0 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(362);
    // C++ 363 (0x0084BC90): CModelInfo::ms_weaponModelInfoStore: constructor 0x4C5E90 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C58E0 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(363);
    // C++ 364 (0x0084BCB0): CModelInfo::ms_clumpModelInfoStore: constructor 0x4C5EF0 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C5900 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(364);
    // C++ 365 (0x0084BCD0): CModelInfo::ms_vehicleModelInfoStore: constructor 0x4C5F50 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C6370 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(365);
    // C++ 366 (0x0084BCF0): CModelInfo::ms_pedModelInfoStore: constructor 0x4C67D0 (ModelInfoStoreConstructStatic: constructor de elemento del EXE sobre cada elemento y contador a 0); destructor 0x4C6390 (ModelInfoStoreDestructStatic: destructor de elemento en orden inverso)
    RunRow(366);
    // C++ 389 (0x0084BFD0): 2500 x CAnimBlendHierarchy (24 bytes) en 0x00B4EA40
    ConstructArrayAt<CAnimBlendHierarchy, 0x00B4EA40, 2500, 24>(true);
    // C++ 410 (0x0084C270): CAEAudioHardware en 0x00B5F8B8
    ConstructAt<CAEAudioHardware, 0x00B5F8B8>();
    // C++ 411 (0x0084C290): CAESmoothFadeThread en 0x00B608D0 (destructor 0x004EE9E0  no es el de la clase)
    ConstructAtNoDtor<CAESmoothFadeThread, 0x00B608D0>();
    // C++ 431 (0x0084C500): CAEStreamTransformer: 0x4F1750 (Initialise: clave fija de 16 bytes); sin destructor
    RunRow(431);
    // C++ 468 (0x0084C9A0): CAEPedSpeechSlot x 6: ctor 0x4E3330 es `mov eax,ecx; ret` (el C++ propio SI inicializa miembros, el EXE no); dtor 0x4E3340 `ret`
    RunRow(468);
    // C++ 496 (0x0084CD20): CAESoundManager en 0x00B62CB0 (destructor 0x004EFD60  no es el de la clase)
    ConstructAtNoDtor<CAESoundManager, 0x00B62CB0>();
    // C++ 512 (0x0084CF10): valores iniciales
    StoreRun<uint32>(0xB6BA3C, 1, 4, 0xC0000000);
    // C++ 518 (0x0084CFC0): float en 0xB6BA54 = [0xB6BA4C] / (float)(int16)[0x8CBC8C] (movsx; fild; fdivr; fstp dword); depende de la constante x87 0xB6BA4C (tabla C++ 0x84CF80, ya escrita por RunX87StaticInitializers)
    RunRow(518);
    // C++ 519 (0x0084CFE0): valores iniciales
    StoreRun<uint32>(0xB6BA58, 1, 4, 0x3F800000);
    // C++ 533 (0x0084D190): valores iniciales
    StoreRun<uint32>(0x8CC0B8, 1, 4, 0xC1000000);
    StoreRun<uint32>(0x8CC0C0, 1, 4, 0xC0A00000);
    // C++ 559 (0x0084D4D0): CAEWeatherAudioEntity::m_sRainSoundL: constructor por defecto 0x4F63B0 (DefaultConstructorExe); dtor 0x4F2B80 = ~CAETwinLoopSoundEntity
    RunRow(559);
    // C++ 560 (0x0084D4F0): CAEWeatherAudioEntity::m_sRainSoundR: constructor por defecto 0x4F63B0 (DefaultConstructorExe); dtor 0x4F2B80 = ~CAETwinLoopSoundEntity
    RunRow(560);
    // C++ 567 (0x0084D5D0): CAudioEngine en 0x00B6BC90
    ConstructAt<CAudioEngine, 0x00B6BC90>();
    // C++ 575 (0x0084D6D0): 6 x CHandShaker (148 bytes) en 0x00B6ECA0
    ConstructArrayAt<CHandShaker, 0x00B6ECA0, 6, 148>(false);
    // C++ 576 (0x0084D700): CCamera en 0x00B6F028 (destructor 0x0050A870 CCamera::DestructorBody no es el de la clase)
    ConstructAtNoDtor<CCamera, 0x00B6F028>();
    // C++ 577 (0x0084D720): gIdleCam: 0x50E6D0 (CIdleCam::Init); sin destructor
    RunRow(577);
    // C++ 578 (0x0084D740): destructor de CMatrix en 0x00B6FE40
    RegisterDtor<CMatrix, 0x00B6FE40>();
    // C++ 579 (0x0084D750): valores iniciales
    StoreRun<uint32>(0xB6FE88, 1, 4, 0x8CC958);
    // C++ 580 (0x0084D770): parametros de la camara de helicoptero DW (1 x 0x9C): ctor de elemento 0x5173F0 (salta a 0x50E180: valores por defecto); dtor 0x50E080 `ret`
    RunRow(580);
    // C++ 593 (0x0084D920): CControllerConfigManager en 0x00B70198 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CControllerConfigManager, 0x00B70198>();
    // C++ 636 (0x0084DE70): CFireManager en 0x00B71F80
    ConstructAt<CFireManager, 0x00B71F80>();
    // C++ 649 (0x0084E010): CLoadMonitor en 0x00B72978 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CLoadMonitor, 0x00B72978>();
    // C++ 665 (0x0084E1F0): CPad x 2 (CPad::Pads): ctor 0x541D80 = CPad::Clear(true, true), que solo deja ceros; dtor 0x53ED60 `ret`
    RunRow(665);
    // C++ 672 (0x0084E2E0): gDummyMatrix: ctor 0x59AED0 (CMatrix::SetScale(1.0f)); dtor 0x59ACD0 = ~CMatrix
    RunRow(672);
    // C++ 673 (0x0084E300): CMatrixLinkList en 0x00B74288
    ConstructAt<CMatrixLinkList, 0x00B74288>();
    // C++ 711 (0x0084E7B0): CRGBA en 0x00B7CB10 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CRGBA, 0x00B7CB10>();
    // C++ 712 (0x0084E7E0): valores iniciales
    CopyValue<uint16>(0xB7CB20, 0xC81320);
    // C++ 713 (0x0084E7F0): valores iniciales
    CopyValue<uint16>(0xB7CB24, 0xC8131C);
    // C++ 726 (0x0084E980): 2 x CPlayerInfo (400 bytes) en 0x00B7CD98
    ConstructArrayAt<CPlayerInfo, 0x00B7CD98, 2, 400>(false);
    // C++ 727 (0x0084E9B0): CSector x 14400 (CWorld::ms_aSectors): ctor de elemento 0x564040 solo escribe ceros; dtor 0x563130 (SectorDestructorExe: Flush de dummies y de edificios) en orden inverso
    RunRow(727);
    // C++ 728 (0x0084E9E0): CRepeatSector x 256 (CWorld::ms_aRepeatSectors): ctor de elemento 0x563190 (tres listas, ctor 0x563180 pone ceros); dtor 0x5631B0 (tres Flush 0x5630F0 -> 0x552470) en orden inverso
    RunRow(728);
    // C++ 729 (0x0084EA10): CPtrListSingleLink x 900 (CWorld::ms_aLodPtrLists): ctor de elemento 0x43E3F0 pone el puntero a cero; dtor 0x43E1A0 (salto al Flush 0x552400) en orden inverso
    RunRow(729);
    // C++ 730 (0x0084EA40): CWorld::ms_listMovingEntityPtrs: solo destructor (Flush 0x552470); no hay constructor
    RunRow(730);
    // C++ 731 (0x0084EA50): CWorld::ms_listObjectsWithControlCode: solo destructor (Flush 0x552470); no hay constructor
    RunRow(731);
    // C++ 763 (0x0084EE40): CMenuManager en 0x00BA6748
    ConstructAt<CMenuManager, 0x00BA6748>();
    // C++ 782 (0x0084F0A0): CRGBA x 6 (CRadar::ArrowBlipColour): ctor 0x463360 es `mov eax,ecx; ret`; dtor 0x43CE70 `ret`
    RunRow(782);
    // C++ 783 (0x0084F0D0): 64 x CSprite2d (4 bytes) en 0x00BAA250
    ConstructArrayAt<CSprite2d, 0x00BAA250, 64, 4>(true);
    // C++ 790 (0x0084F1C0): 6 x CSprite2d (4 bytes) en 0x00BAB1FC
    ConstructArrayAt<CSprite2d, 0x00BAB1FC, 6, 4>(true);
    // C++ 791 (0x0084F1F0): CHudColours en 0x00BAB22C (destructor 0x0058FD90  no es el de la clase)
    ConstructAtNoDtor<CHudColours, 0x00BAB22C>();
    // C++ 798 (0x0084F2D0): 7 x CSprite2d (4 bytes) en 0x00BAB35C
    ConstructArrayAt<CSprite2d, 0x00BAB35C, 7, 4>(true);
    // C++ 799 (0x0084F300): FurnitureManager_c en 0x00BAB380
    ConstructAt<FurnitureManager_c, 0x00BAB380>();
    // C++ 800 (0x0084F320): 128 x FurnitureSubGroup_c (28 bytes) en 0x00BAD3F8
    ConstructArrayAt<FurnitureSubGroup_c, 0x00BAD3F8, 128, 28>(true);
    // C++ 801 (0x0084F350): 256 x Furniture_c (20 bytes) en 0x00BAE1F8
    ConstructArrayAt<Furniture_c, 0x00BAE1F8, 256, 20>(false);
    // C++ 832 (0x0084F740): InteriorManager_c en 0x00BAF670
    ConstructAt<InteriorManager_c, 0x00BAF670>();
    // C++ 858 (0x0084FA80): BreakManager_c en 0x00BB4240 (destructor 0x0059E640  no es el de la clase)
    ConstructAtNoDtor<BreakManager_c, 0x00BB4240>();
    // C++ 884 (0x0084FDC0): ProcObjectMan_c en 0x00BB7CB0
    ConstructAt<ProcObjectMan_c, 0x00BB7CB0>();
    // C++ 887 (0x0084FE20): gOctTreeBase: ctor 0x5A7570 (OctTreeBaseConstructorExe); dtor 0x5A4140 = ~COctTreeBase
    RunRow(887);
    // C++ 888 (0x0084FE40): ClothesBuilder::playerImg: ctor vacio 0x532290 = CDirectory() (no escribe m_bOwnsEntries); dtor 0x5322D0 = ~CDirectory
    RunRow(888);
    // C++ 904 (0x00850040): CAnimBlendAssocGroup en 0x00BC4020
    ConstructAt<CAnimBlendAssocGroup, 0x00BC4020>();
    // C++ 974 (0x00850910): CRGBA en 0x00C03A44 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CRGBA, 0x00C03A44>();
    // C++ 975 (0x00850940): CPlantLocTri x 256: ctor 0x5DB2B0 es `mov eax,ecx; ret`; dtor 0x5DB2C0 `ret`
    RunRow(975);
    // C++ 976 (0x00850970): CPlantColEntEntry x 40: ctor 0x5DB290 es `mov eax,ecx; ret`; dtor 0x5DB2A0 `ret`
    RunRow(976);
    // C++ 1003 (0x00850CE0): CGangInfo x 10 (CGangs::Gang): ctor 0x5DE520 (byte +0 = 0xFF, resto a cero) = los inicializadores de miembro de la clase; dtor 0x5DE540 `ret`
    RunRow(1003);
    // C++ 1048 (0x00851270): CPedGroupDefaultTaskAllocatorFollowAnyMeans: ctor 0x5F6450 (solo la vtable de la clase del EXE); dtor 0x5F6460 solo devuelve la vtable a la base (sin efecto observable)
    RunRow(1048);
    // C++ 1049 (0x00851290): CPedGroupDefaultTaskAllocatorFollowLimited: ctor 0x5F6480 (solo la vtable de la clase del EXE); dtor 0x5F6490 solo devuelve la vtable a la base (sin efecto observable)
    RunRow(1049);
    // C++ 1050 (0x008512B0): CPedGroupDefaultTaskAllocatorStandStill: ctor 0x5F64B0 (solo la vtable de la clase del EXE); dtor 0x5F64C0 solo devuelve la vtable a la base (sin efecto observable)
    RunRow(1050);
    // C++ 1051 (0x008512D0): CPedGroupDefaultTaskAllocatorChat: ctor 0x5F64E0 (solo la vtable de la clase del EXE); dtor 0x5F64F0 solo devuelve la vtable a la base (sin efecto observable)
    RunRow(1051);
    // C++ 1052 (0x008512F0): CPedGroupDefaultTaskAllocatorRandom: ctor 0x5F6510 (solo la vtable de la clase del EXE); dtor 0x5F6520 solo devuelve la vtable a la base (sin efecto observable)
    RunRow(1052);
    // C++ 1053 (0x00851310): CPedGroupDefaultTaskAllocatorSitInLeaderCar: ctor 0x5F6540 (solo la vtable de la clase del EXE); dtor 0x5F6550 solo devuelve la vtable a la base (sin efecto observable)
    RunRow(1053);
    // C++ 1054 (0x00851330): 8 x CPedGroup (724 bytes) en 0x00C09920
    ConstructArrayAt<CPedGroup, 0x00C09920, 8, 724>(true);
    // C++ 1067 (0x008514E0): CInterestingEvents en 0x00C0B058 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CInterestingEvents, 0x00C0B058>();
    // C++ 1080 (0x00851680): 128 x CPedScriptedTaskRecordData (20 bytes) en 0x00C0B1E8
    ConstructArrayAt<CPedScriptedTaskRecordData, 0x00C0B1E8, 128, 20>(false);
    // C++ 1111 (0x00851A70): BoneNodeManager_c en 0x00C10820
    ConstructAt<BoneNodeManager_c, 0x00C10820>();
    // C++ 1118 (0x00851B50): IKChainManager_c en 0x00C15448
    ConstructAt<IKChainManager_c, 0x00C15448>();
    // C++ 1155 (0x00851FF0): CColModel en 0x00C17824
    ConstructAt<CColModel, 0x00C17824>();
    // C++ 1156 (0x00852010): CCollisionData en 0x00C17854 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CCollisionData, 0x00C17854>();
    // C++ 1163 (0x008520E0): CTaskComplexSequence x 64 (ms_taskSequence): ctor de elemento 0x632BD0 = el constructor por defecto de la clase; dtor 0x6389F0 = ~CTaskComplexSequence
    RunRow(1163);
    // C++ 1164 (0x00852110): 48 x CScriptedBrainTaskEntry (16 bytes) en 0x00C188F0
    ConstructArrayAt<CScriptedBrainTaskEntry, 0x00C188F0, 48, 16>(false);
    // C++ 1207 (0x00852680): valores iniciales
    StoreRun<uint32>(0xC18CE4, 1, 4, 0x42700000);
    // C++ 1208 (0x00852690): valores iniciales
    StoreRun<uint32>(0xC18CE8, 1, 4, 0x42700000);
    // C++ 1209 (0x008526A0): valores iniciales
    StoreRun<uint32>(0xC18CEC, 1, 4, 0x42700000);
    // C++ 1210 (0x008526B0): valores iniciales
    StoreRun<uint32>(0xC18CF0, 1, 4, 0x3F800000);
    // C++ 1225 (0x00852860): valores iniciales
    StoreRun<uint32>(0xC18DB4, 1, 4, 0x3F000000);
    // C++ 1226 (0x00852870): CPatrolRoute en 0x00C18DB8
    ConstructAt<CPatrolRoute, 0x00C18DB8>();
    // C++ 1239 (0x00852A10): CColModel en 0x00C19518
    ConstructAt<CColModel, 0x00C19518>();
    // C++ 1240 (0x00852A30): CColModel en 0x00C19548
    ConstructAt<CColModel, 0x00C19548>();
    // C++ 1241 (0x00852A50): CColModel en 0x00C19578
    ConstructAt<CColModel, 0x00C19578>();
    // C++ 1242 (0x00852A70): CColModel en 0x00C195A8
    ConstructAt<CColModel, 0x00C195A8>();
    // C++ 1279 (0x00852F10): CTaskComplexFollowLeaderInFormation::ms_offsets: 0x694850 (Offsets::Constructor: tablas de formacion y escala 1.0); sin destructor
    RunRow(1279);
    // C++ 1298 (0x00853170): CCover::m_ListOfProcessedBuildings: solo destructor (Flush 0x552470); no hay constructor
    RunRow(1298);
    // C++ 1322 (0x00853460): CText en 0x00C1B340
    ConstructAt<CText, 0x00C1B340>();
    // C++ 1354 (0x00853880): cBuoyancy en 0x00C1C890 (destructor 0x006C2B80  no es el de la clase)
    ConstructAtNoDtor<cBuoyancy, 0x00C1C890>();
    // C++ 1396 (0x00853DC0): 4 x CColModel (48 bytes) en 0x00C1CC78
    ConstructArrayAt<CColModel, 0x00C1CC78, 4, 48>(true);
    // C++ 1397 (0x00853DF0): CColModel en 0x00C1CD38
    ConstructAt<CColModel, 0x00C1CD38>();
    // C++ 1398 (0x00853E10): CCollisionData en 0x00C1CD68 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CCollisionData, 0x00C1CD68>();
    // C++ 1399 (0x00853E20): 30 x CVehicleAnimGroup (148 bytes) en 0x00C1CDC0
    ConstructArrayAt<CVehicleAnimGroup, 0x00C1CDC0, 30, 148>(false);
    // C++ 1406 (0x00853F10): WaterCreatureManager_c en 0x00C1DF30
    ConstructAt<WaterCreatureManager_c, 0x00C1DF30>();
    // C++ 1458 (0x008545B0): tUserList x 64 (CScripted2dEffects::ms_userLists): ctor de elemento 0x6FA6D0 (tUserList::Constructor: -1 en las 8 entradas y bUseList = 0); dtor 0x6F9E00 `ret`
    RunRow(1458);
    // C++ 1459 (0x008545E0): CScriptedEffectPairs x 64 (CScripted2dEffects::ms_effectPairs): ctor de elemento 0x6FA790 (CScriptedEffectPairs::Constructor: cuatro pares y Reset); dtor 0x6F9E60 solo recorre destructores vacios
    RunRow(1459);
    // C++ 1476 (0x00854810): valores iniciales
    StoreRun<uint32>(0xC40300, 1, 4, 0x42480000);
    // C++ 1481 (0x008548A0): valores iniciales
    CopyValue<uint32>(0xC40314, 0xC17044);
    // C++ 1482 (0x008548B0): valores iniciales
    CopyValue<uint32>(0xC40318, 0xC17048);
    // C++ 1489 (0x00854980): CRealTimeShadowManager en 0x00C40350
    ConstructAt<CRealTimeShadowManager, 0x00C40350>();
    // C++ 1504 (0x00854B60): 64 x CStencilShadowObject (28 bytes) en 0x00C6A198
    ConstructArrayAt<CStencilShadowObject, 0x00C6A198, 64, 28>(true);
    // C++ 1520 (0x00854D50): 32 x CEscalator (336 bytes) en 0x00C6E9A8
    ConstructArrayAt<CEscalator, 0x00C6E9A8, 32, 336>(true);
    // C++ 1524 (0x00854DC0): 2 x CSprite2d (4 bytes) en 0x00C71AD0
    ConstructArrayAt<CSprite2d, 0x00C71AD0, 2, 4>(true);
    // C++ 1525 (0x00854DF0): 15 x CSprite2d (4 bytes) en 0x00C71AD8
    ConstructArrayAt<CSprite2d, 0x00C71AD8, 15, 4>(true);
    // C++ 1532 (0x00854EE0): CFallingGlassPane x 45 (CGlass::aGlassPanes): ctor de elemento 0x71A8B0 solo escribe ceros (+0x40, +0x44 de la CMatrix); dtor 0x71A8C0 = ~CMatrix sobre una matriz sin RenderWare asociado (sin efecto)
    RunRow(1532);
    // C++ 1565 (0x00855310): C3dMarker x 32 (C3dMarkers::m_aMarkerArray): ctor de elemento 0x720F60 solo escribe ceros (el C++ propio SI inicializa miembros, el EXE no); dtor 0x720F70 = ~CMatrix sobre una matriz sin RenderWare asociado
    RunRow(1565);
    // C++ 1573 (0x00855420): CWaterCannon x 3 (CWaterCannons::aCannons): ctor de elemento 0x728B10 (vtable de CAEWaterCannonAudioEntity en +0x32c y ceros) = constructor por defecto; dtor 0x728B30 = ~CWaterCannon
    RunRow(1573);
    // C++ 1580 (0x00855510): CWeather::m_WeatherAudioEntity: ctor 0x72A620 (WeatherAudioEntityConstructorBody); dtor 0x72A400 = ~CAEWeatherAudioEntity
    RunRow(1580);
    // C++ 1591 (0x00855670): g_Heaps[0] (CMemoryHeap): ctor 0x72E860 solo pone m_FirstBlock a cero; sin destructor
    RunRow(1591);
    // C++ 1592 (0x00855680): g_Heaps[1] (CMemoryHeap): ctor 0x72E860 solo pone m_FirstBlock a cero; sin destructor
    RunRow(1592);
    // C++ 1593 (0x00855690): g_Heaps[2] (CMemoryHeap): ctor 0x72E860 solo pone m_FirstBlock a cero; sin destructor
    RunRow(1593);
    // C++ 1620 (0x008559E0): CExplosion::m_ExplosionAudioEntity: ctor 0x5074B0 (CAEExplosionAudioEntity::Constructor); dtor 0x506B90 = ~CAEExplosionAudioEntity
    RunRow(1620);
    // C++ 1639 (0x00855C40): CColModel en 0x00C8A7DC
    ConstructAt<CColModel, 0x00C8A7DC>();
    // C++ 1646 (0x00855D20): CWeaponEffects x 2 (gCrossHair): ctor 0x742A90 es `mov eax,ecx; ret`; dtor 0x742AA0 `ret`
    RunRow(1646);
    // C++ 1653 (0x00855E10): CWeaponInfo x 80 (CWeaponInfo::aWeaponInfo): ctor 0x743C30 es `mov eax,ecx; ret`; dtor 0x743C40 `ret`
    RunRow(1653);
    // C++ 1223 (0x852840): cos(pi/8) como float en 0xC18D48 (TaskSimpleGoTo::ms_fLookAtThresholdDotProduct); fld qword [0x870190]; fcos; fstp dword
    StaticRef<float>(0xC18D48) = static_cast<float>(std::cos(0.39269909262657166));
    // C++ 1329 (0x853540): tres restas float en 0xC1C208..0xC1C210 (posicion del arma del Tiger: punto2 - punto1 con las constantes 0x8D318C..0x8D31A0)
    StaticRef<float>(0xC1C208) = StaticRef<float>(0x8D3198) - StaticRef<float>(0x8D318C);
    StaticRef<float>(0xC1C20C) = StaticRef<float>(0x8D319C) - StaticRef<float>(0x8D3190);
    StaticRef<float>(0xC1C210) = StaticRef<float>(0x8D31A0) - StaticRef<float>(0x8D3194);
}

// Prueba de que cada fila reproduce lo que hizo la rutina del EXE. Solo con la DLL sobre el EXE original (el _initterm del EXE ya dejo los objetos en su estado inicial y el juego aun no
// los ha tocado: se llama al instalar los ganchos). Por fila: guarda los bytes del EXE, pone la zona a cero (el estado del .bss del ejecutable independiente), ejecuta el constructor del C++
// propio, compara y restaura los bytes del EXE. Una fila sin constructor se da por buena si la zona que dejo el EXE es toda ceros (la rutina solo escribia ceros). Las palabras que en el EXE
// son una vtable del EXE (.rdata) no se comparan (el C++ propio puede dejar la suya) y se cuentan aparte. No registra destructores ni toca nada fuera de la zona de la fila.
StaticInitReport& LastReport() {
    static StaticInitReport report;
    return report;
}

StaticInitReport VerifyStaticInitializers() {
    StaticInitReport rep;
    for (const Row& r : kRows) {
        if (!r.size) {
            continue;
        }
        ++rep.rows;
        auto* const mem = reinterpret_cast<uint8_t*>(r.va);
        const std::vector<uint8_t> exe(mem, mem + r.size);
        std::memset(mem, 0, r.size);
        if (r.construct) {
            r.construct();
        }
        const std::vector<uint8_t> mine(mem, mem + r.size);
        std::memcpy(mem, exe.data(), r.size);
        int shown = 0;
        bool different = false;
        for (size_t i = 0; i < r.size; ++i) {
            if (mine[i] == exe[i]) {
                continue;
            }
            const size_t w = i & ~size_t{ 3 };
            if (w + 4 <= r.size) {
                uint32_t exeWord;
                std::memcpy(&exeWord, &exe[w], 4);
                if (exeWord >= 0x858000 && exeWord < 0x8A4000) {
                    ++rep.vtableWords;
                    i = w + 3;
                    continue;
                }
            }
            different = true;
            if (shown++ < 6) {
                rep.detail += std::format("fila {} (0x{:X}) +0x{:X}: EXE 0x{:02X}, C++ 0x{:02X}; ", r.index, r.va, i, exe[i], mine[i]);
            }
        }
        if (different) {
            ++rep.different;
            if (!rep.firstDifferent) {
                rep.firstDifferent = static_cast<uint32_t>(r.index);
            }
        } else {
            ++rep.equal;
        }
    }
    LastReport() = rep;
    return rep;
}

const StaticInitReport& LastStaticInitReport() { return LastReport(); }

} // namespace DatosExe
