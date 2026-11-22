#include "StdInc.h"
#include "../Animation/AnimBlendAssocGroup.h"
#include "../Animation/AnimBlendHierarchy.h"
#include "../Audio/AESmoothFadeThread.h"
#include "../Audio/AudioEngine.h"
#include "../Audio/Hardware/AEAudioHardware.h"
#include "../Audio/Managers/AESoundManager.h"
#include "../BreakManager_c.h"
#include "../Buoyancy.h"
#include "../Camera.h"
#include "../Collision/ColModel.h"
#include "../Collision/CollisionData.h"
#include "../ControllerConfigManager.h"
#include "../Core/Matrix.h"
#include "../Core/MatrixLinkList.h"
#include "../Escalator.h"
#include "../FireManager.h"
#include "../Fx/Fx.h"
#include "../Fx/FxManager.h"
#include "../HandShaker.h"
#include "../HudColours.h"
#include "../InformGroupEvent.h"
#include "../InterestingEvents.h"
#include "../Interior/FurnitureManager_c.h"
#include "../Interior/FurnitureSubGroup_c.h"
#include "../Interior/Furniture_c.h"
#include "../Interior/InteriorManager_c.h"
#include "../LoadMonitor.h"
#include "../MenuManager.h"
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
#include "../ScriptsForBrains.h"
#include "../Sprite2d.h"
#include "../StencilShadowObject.h"
#include "../Tasks/PedScriptedTaskRecord.h"
#include "../Tasks/ScriptedBrainTaskStore.h"
#include "../Text/Text.h"
#include "../VehicleAnimGroupData.h"
#include "../WaterCreatureManager_c.h"

#include "StaticInit.h"
#include "Crt/CrtOnexit.h"

// Generado por contrib/datos_exe_staticinit.py; no editar a mano (ver docs/DatosExe/inicializadores-estado.csv).

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
}  // namespace

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
    // C++ 197 (0x0084A7B0): 128 x CSprite2d (4 bytes) en 0x00A94B68
    ConstructArrayAt<CSprite2d, 0x00A94B68, 128, 4>(true);
    // C++ 284 (0x0084B2A0): CPedClothesDesc en 0x00A9A810 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CPedClothesDesc, 0x00A9A810>();
    // C++ 310 (0x0084B5D0): Fx_c en 0x00A9AE00 (destructor 0x0049E630  no es el de la clase)
    ConstructAtNoDtor<Fx_c, 0x00A9AE00>();
    // C++ 314 (0x0084B650): FxManager_c en 0x00A9AE80
    ConstructAt<FxManager_c, 0x00A9AE80>();
    // C++ 346 (0x0084BA60): 8 x CInformGroupEvent (16 bytes) en 0x00A9B018
    ConstructArrayAt<CInformGroupEvent, 0x00A9B018, 8, 16>(true);
    // C++ 389 (0x0084BFD0): 2500 x CAnimBlendHierarchy (24 bytes) en 0x00B4EA40
    ConstructArrayAt<CAnimBlendHierarchy, 0x00B4EA40, 2500, 24>(true);
    // C++ 410 (0x0084C270): CAEAudioHardware en 0x00B5F8B8
    ConstructAt<CAEAudioHardware, 0x00B5F8B8>();
    // C++ 411 (0x0084C290): CAESmoothFadeThread en 0x00B608D0 (destructor 0x004EE9E0  no es el de la clase)
    ConstructAtNoDtor<CAESmoothFadeThread, 0x00B608D0>();
    // C++ 496 (0x0084CD20): CAESoundManager en 0x00B62CB0 (destructor 0x004EFD60  no es el de la clase)
    ConstructAtNoDtor<CAESoundManager, 0x00B62CB0>();
    // C++ 512 (0x0084CF10): valores iniciales
    StoreRun<uint32>(0xB6BA3C, 1, 4, 0xC0000000);
    // C++ 519 (0x0084CFE0): valores iniciales
    StoreRun<uint32>(0xB6BA58, 1, 4, 0x3F800000);
    // C++ 533 (0x0084D190): valores iniciales
    StoreRun<uint32>(0x8CC0B8, 1, 4, 0xC1000000);
    StoreRun<uint32>(0x8CC0C0, 1, 4, 0xC0A00000);
    // C++ 567 (0x0084D5D0): CAudioEngine en 0x00B6BC90
    ConstructAt<CAudioEngine, 0x00B6BC90>();
    // C++ 575 (0x0084D6D0): 6 x CHandShaker (148 bytes) en 0x00B6ECA0
    ConstructArrayAt<CHandShaker, 0x00B6ECA0, 6, 148>(false);
    // C++ 576 (0x0084D700): CCamera en 0x00B6F028 (destructor 0x0050A870 CCamera::DestructorBody no es el de la clase)
    ConstructAtNoDtor<CCamera, 0x00B6F028>();
    // C++ 578 (0x0084D740): destructor de CMatrix en 0x00B6FE40
    RegisterDtor<CMatrix, 0x00B6FE40>();
    // C++ 579 (0x0084D750): valores iniciales
    StoreRun<uint32>(0xB6FE88, 1, 4, 0x8CC958);
    // C++ 593 (0x0084D920): CControllerConfigManager en 0x00B70198 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CControllerConfigManager, 0x00B70198>();
    // C++ 636 (0x0084DE70): CFireManager en 0x00B71F80
    ConstructAt<CFireManager, 0x00B71F80>();
    // C++ 649 (0x0084E010): CLoadMonitor en 0x00B72978 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CLoadMonitor, 0x00B72978>();
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
    // C++ 763 (0x0084EE40): CMenuManager en 0x00BA6748
    ConstructAt<CMenuManager, 0x00BA6748>();
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
    // C++ 904 (0x00850040): CAnimBlendAssocGroup en 0x00BC4020
    ConstructAt<CAnimBlendAssocGroup, 0x00BC4020>();
    // C++ 974 (0x00850910): CRGBA en 0x00C03A44 (sin destructor (el funclet no hace nada))
    ConstructAtNoDtor<CRGBA, 0x00C03A44>();
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
    // C++ 1639 (0x00855C40): CColModel en 0x00C8A7DC
    ConstructAt<CColModel, 0x00C8A7DC>();
    // C++ 1223 (0x852840): cos(pi/8) como float en 0xC18D48 (TaskSimpleGoTo::ms_fLookAtThresholdDotProduct); fld qword [0x870190]; fcos; fstp dword
    StaticRef<float>(0xC18D48) = static_cast<float>(std::cos(0.39269909262657166));
    // C++ 1329 (0x853540): tres restas float en 0xC1C208..0xC1C210 (posicion del arma del Tiger: punto2 - punto1 con las constantes 0x8D318C..0x8D31A0)
    StaticRef<float>(0xC1C208) = StaticRef<float>(0x8D3198) - StaticRef<float>(0x8D318C);
    StaticRef<float>(0xC1C20C) = StaticRef<float>(0x8D319C) - StaticRef<float>(0x8D3190);
    StaticRef<float>(0xC1C210) = StaticRef<float>(0x8D31A0) - StaticRef<float>(0x8D3194);
}

} // namespace DatosExe
