#include "StdInc.h"

#include <bit>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "World.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedAttractorManager.h"
#include "CarEnterExit.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Train.h"
#include "Entity/Object/Object.h"
#include "Entity/Ped/PlayerPed.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "cHandlingDataMgr.h"
#include "tHandlingData.h"
#include "Camera.h"
#include "Cam.h"
#include "Timer.h"
#include "Pad.h"
#include "Pickup.h"
#include "Weapon.h"
#include "PostEffects.h"
#include "CullZones.h"
#include "Pickups.h"
#include "Wanted.h"
#include "GameLogic.h"
#include "Streaming.h"
#include "Renderer.h"
#include "GangWars.h"
#include "Garages.h"
#include "Population.h"
#include "Restart.h"
#include "Stats.h"
#include "Messages.h"
#include "Hud.h"
#include "Font.h"
#include "MenuManager.h"
#include "MenuSystem.h"
#include "PointLights.h"
#include "TheCarGenerators.h"
#include "CarGenerator.h"
#include "SpecialPlateHandler.h"
#include "EntryExitManager.h"
#include "Text/Text.h"
#include "Conversations/Conversations.h"
#include "Audio/AudioEngine.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedOut.h"
#include "Tasks/TaskTypes/TaskComplexUseGoggles.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"

// F4 de codigo vivo (sin validar): 0x47A760 (6124 bytes, hasta 0x47BF4C), CRunningScript::ProcessCommands2500To2599, el despachador original de los comandos de script 2500..2599 (ranura 25 de la tabla 0x8A6168; lo usa
// ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con manejador 0x8382E7, 3 estados de desenrollado): solo
// protege los `new` de tareas (el destructor de desenrollado es operator delete) y el objeto CTaskSimpleCarSetPedOut de la pila; en C++ no hace falta.
// Salto con una tabla directa de 100 entradas (0x47BF4C): 10 comandos van a 0x47BF2B (devuelven 0 sin leer parametros: 2502 2515 2524 2538 2547 2553 2560 2564 2565 2573) y 3 (2509 2510 2527) y los fuera de rango
// a 0x47BF2F (OR_INTERRUPT, -1). Cada `case` replica el orden de colaboradores del desensamblado; los parametros se leen de ScriptParams (0xA43C78).
// Colas compartidas del EXE: 0x47BB9A (`test al,al` -> bandera = 1) / 0x47BBA3 (UpdateCompareFlag con la bandera local, inicialmente 0) / 0x47BBAF (devuelve 0 sin tocar la bandera de comparacion).
// Los indices y desplazamientos que vienen del script se usan sin comprobar rangos, como el EXE.
namespace {
// 0x859014: float -100.0 (0xC2C80000)
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x859F00: float 0.015 (0x3C75C28F), correccion de Z del generador de coches con matricula
constexpr float EXE_K_0_015 = std::bit_cast<float>(0x3C75C28Fu);
// 0x858B8C: float 0.5 (0x3F000000), radio de IS_MONEY_PICKUP_AT_COORDS
constexpr float EXE_K_0_5 = std::bit_cast<float>(0x3F000000u);
// 0x859520: float 1/640 = 0.0015625 (0x3ACCCCCD), factor del ancho de columna de los menus
constexpr float EXE_K_1_640 = std::bit_cast<float>(0x3ACCCCCDu);
// 0x859A3C: float 1/255 (0x3B808081), de componente 0..255 a 0..1
constexpr float EXE_K_1_255 = std::bit_cast<float>(0x3B808081u);
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);

inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef), 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x465040 (CObjectPool::GetAtRef): sin comprobar el resultado, como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CObject* ObjectOf(int32 handle) {
    return CPools::GetObject(handle);
}
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
} // namespace

static_assert(offsetof(CPed, m_nAnimGroup) == 0x4D4);
static_assert(offsetof(CPed, m_pGogglesObject) == 0x4FC);
static_assert(offsetof(CPed, m_pVehicle) == 0x58C);
static_assert(offsetof(CVehicle, m_vehicleAudio) == 0x138);
static_assert(offsetof(CVehicle, m_nHandlingFlagsIntValue) == 0x38C);
static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590);
static_assert(offsetof(CVehicle, m_nPrimaryColor) == 0x434 && offsetof(CVehicle, m_nQuaternaryColor) == 0x437);
static_assert(offsetof(CTrain, m_nTrainFlags) == 0x5B8);
static_assert(offsetof(CBike, m_nNoOfContactWheels) == 0x804);
static_assert(offsetof(CAutomobile, m_wMiscComponentAngle) == 0x86C);
static_assert(offsetof(CPlayerPedData, m_nPlayerGroup) == 0x38 && offsetof(CPlayerPedData, m_nScriptLimitToGangSize) == 0x43);
static_assert(offsetof(CPlayerPedData, m_bForceInteriorLighting) == 0x86);
static_assert(sizeof(CPlayerInfo) == 0x190);
static_assert(sizeof(CCarGenerator) == 0x20);
static_assert(sizeof(CPickup) == 0x20 && offsetof(CPickup, m_nPickupType) == 0x1C);
static_assert(sizeof(CCam) == 0x238 && offsetof(CCamera, m_aCams) == 0x174);
static_assert(offsetof(CCam, m_nMode) == 0xC && offsetof(CCam, m_fVerticalAngle) == 0xAC && offsetof(CCam, m_fHorizontalAngle) == 0xBC);
static_assert(offsetof(CCamera, m_aCamTweak) == 0xCF4);
static_assert(sizeof(tHandlingData) == 0xE0 && offsetof(tHandlingData, m_nMonetaryValue) == 0xD8 && offsetof(cHandlingDataMgr, m_aVehicleHandling) == 0x14);
static_assert(offsetof(CVehicleModelInfo, m_nHandlingId) == 0x4A);
static_assert(offsetof(CMenuManager, m_bShowSubtitles) == 0x44);
static_assert(sizeof(tScriptSearchlight) == 0x7C);

// 0x47A760
OpcodeResult CRunningScript::ProcessCommands2500To2599(int32 command) {
    switch (command) {
    // 0x47A7A2
    case COMMAND_SET_PETROL_TANK_WEAKPOINT: {
        CollectParameters(2);
        CVehicle* const veh                     = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bPetrolTankIsWeakPoint = P(1).u8Param & 1; // shl cl,6 + mascara 0x40 sobre el byte +0x42E
        return OR_CONTINUE;
    }
    // 0x47A7E1
    case COMMAND_IS_CHAR_USING_MAP_ATTRACTOR: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        UpdateCompareFlag(GetPedAttractorManager()->IsPedRegisteredWithEffect(ped));
        return OR_CONTINUE;
    }
    // 0x47A820
    case COMMAND_SET_PLAYER_MODEL: {
        CollectParameters(2);
        CPed* const ped       = CWorld::Players[P(0).iParam].m_pPed;
        const auto  animGroup = ped->m_nAnimGroup; // ebx = [esi+0x4D4]
        ped->DeleteRwObject();                     // vtable +0x20
        ped->m_nModelIndex = 0xFFFF;               // mov word [esi+0x22], 0xFFFF
        ped->SetModelIndex(P(1).uParam);           // vtable +0x14
        ped->m_nAnimGroup = animGroup;
        return OR_CONTINUE;
    }
    // 0x47A869
    case COMMAND_ARE_SUBTITLES_SWITCHED_ON: {
        UpdateCompareFlag(FrontEndMenuManager.m_bShowSubtitles); // byte en 0xBA678C
        return OR_CONTINUE;
    }
    // 0x47A880
    case COMMAND_REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION: {
        CollectParameters(2);
        CPed* const     ped = PedOf(P(0).iParam);
        CVehicle* const veh = VehicleOf(P(1).iParam);
        const CVector   pos = ped->GetPosition(); // matriz + 0x30 si hay matriz; si no, la posicion del placement
        {
            CTaskSimpleCarSetPedOut task(veh, static_cast<eTargetDoor>(CCarEnterExit::ComputeTargetDoorToExit(veh, ped)), false);
            task.ProcessPed(ped);
            ped->SetPosn(pos);
        }
        return OR_CONTINUE;
    }
    // 0x47A921
    case COMMAND_SET_OBJECT_PROOFS: {
        CollectParameters(6);
        CObject* const obj = ObjectOf(P(0).iParam);
        // el EXE reescribe el dword de banderas fisicas (+0x40) campo a campo, con el bit bajo de cada parametro
        obj->physicalFlags.bBulletProof    = P(1).iParam & 1;
        obj->physicalFlags.bFireProof      = P(2).iParam & 1;
        obj->physicalFlags.bExplosionProof = P(3).iParam & 1;
        obj->physicalFlags.bCollisionProof = P(4).iParam & 1;
        obj->physicalFlags.bMeleeProof     = P(5).iParam & 1;
        return OR_CONTINUE;
    }
    // 0x47A9B6
    case COMMAND_IS_CAR_TOUCHING_CAR: {
        CollectParameters(2);
        CVehicle* const first  = VehicleOf(P(0).iParam);
        CVehicle* const second = VehicleOf(P(1).iParam);
        UpdateCompareFlag(first->GetHasCollidedWith(second));
        return OR_CONTINUE;
    }
    // 0x47AA01
    case COMMAND_DOES_OBJECT_HAVE_THIS_MODEL: {
        CollectParameters(2);
        CObject* const obj   = ObjectOf(P(0).iParam);
        int32          model = P(1).iParam;
        if (model < 0) {
            model = CTheScripts::UsedObjectArray[-model].nModelIndex; // 0xA44B88 - id * 0x1C
        }
        UpdateCompareFlag(static_cast<int16>(obj->m_nModelIndex) == model); // movsx word [obj+0x22]
        return OR_CONTINUE;
    }
    // 0x47AA49
    case COMMAND_SET_TRAIN_FORCED_TO_SLOW_DOWN: {
        CollectParameters(2);
        CVehicle* const veh                                  = VehicleOf(P(0).iParam);
        static_cast<CTrain*>(veh)->trainFlags.bForceSlowDown = P(1).iParam != 0; // bit 1 del byte +0x5B9
        return OR_CONTINUE;
    }
    // 0x47AA8D
    case COMMAND_IS_VEHICLE_ON_ALL_WHEELS: {
        CollectParameters(1);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const auto      type = veh->m_nVehicleType;
        bool            flag = false;
        if (type == VEHICLE_TYPE_BIKE && static_cast<CBike*>(veh)->m_nNoOfContactWheels == 4) {
            flag = true;
        }
        if (type == VEHICLE_TYPE_AUTOMOBILE && static_cast<CAutomobile*>(veh)->m_nNumContactWheels == 4) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x47AAEB
    case COMMAND_DOES_PICKUP_EXIST: {
        CollectParameters(1);
        UpdateCompareFlag(CPickups::GetActualPickupIndex(tPickupReference{ P(0).iParam }) != -1);
        return OR_CONTINUE;
    }
    // 0x47AB20
    case COMMAND_ENABLE_AMBIENT_CRIME: {
        CollectParameters(1);
        StaticRef<bool>(0xB72994) = P(0).iParam != 0; // bandera de crimen ambiental (la leen CTaskComplexKillCriminal y MissionCleanup)
        return OR_CONTINUE;
    }
    // 0x47AB3F
    case COMMAND_CLEAR_WANTED_LEVEL_IN_GARAGE: {
        FindPlayerWanted(-1)->ClearWantedLevelAndGoOnParole();
        return OR_CONTINUE;
    }
    // 0x47AB57
    case COMMAND_SET_CHAR_SAY_CONTEXT_IMPORTANT: {
        CollectParameters(5);
        CPed* const ped = PedOf(P(0).iParam);
        const int16 said = ped->Say(static_cast<eGlobalSpeechContext>(P(1).u16Param), 0u, 1.0f, P(2).u8Param != 0, P(3).u8Param != 0, P(4).u8Param != 0);
        P(0).iParam      = said; // movsx eax, ax
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47ABBC
    case COMMAND_SET_CHAR_SAY_SCRIPT: {
        CollectParameters(5);
        CPed* const ped = PedOf(P(0).iParam);
        ped->SayScript(static_cast<eAudioEvents>(P(1).iParam), P(2).u8Param != 0, P(3).u8Param != 0, P(4).u8Param != 0);
        return OR_CONTINUE;
    }
    // 0x47AC06
    case COMMAND_FORCE_INTERIOR_LIGHTING_FOR_PLAYER: {
        CollectParameters(2);
        CPed* const ped                                = CWorld::Players[P(0).iParam].m_pPed;
        ped->GetPlayerData()->m_bForceInteriorLighting = P(1).iParam != 0; // byte +0x86 de los datos del jugador
        return OR_CONTINUE;
    }
    // 0x47AC3E
    case COMMAND_DISABLE_2ND_PAD_FOR_DEBUG: {
        CollectParameters(1);
        return OR_CONTINUE;
    }
    // 0x47AC4E
    case COMMAND_USE_DETONATOR: {
        CWorld::UseDetonator(FindPlayerPed(-1));
        return OR_CONTINUE;
    }
    // 0x47AC65
    case COMMAND_IS_MONEY_PICKUP_AT_COORDS: {
        CollectParameters(3);
        const CVector target{ P(0).fParam, P(1).fParam, P(2).fParam };
        bool          found = false;
        for (auto& pickup : CPickups::aPickUps) { // 620 entradas de 0x20 bytes desde 0x9788C0; el bucle acaba en cuanto hay una
            if (found) {
                break;
            }
            if (pickup.m_nPickupType == PICKUP_MONEY) {
                const CVector pos = pickup.GetPosn();
                CVector       diff;
                VectorSubtract(&diff, &pos, &target);
                if (diff.Magnitude() < EXE_K_0_5) { // fcomp + test ah,5 + jp: verdadero solo si es menor (con NaN, falso)
                    found = true;
                }
            }
        }
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }
    // 0x47AD10
    case COMMAND_SET_MENU_COLUMN_WIDTH: {
        CollectParameters(3);
        const float width = float(RsGlobal.maximumWidth) * EXE_K_1_640 * float(int32(P(2).u16Param)); // RsGlobal.maximumWidth (0xC17044)
        CMenuSystem::SetColumnWidth(P(0).u8Param, P(1).u8Param, static_cast<uint16>(static_cast<int32>(width))); // _ftol (0x821B40)
        return OR_CONTINUE;
    }
    // 0x47AD5A
    case COMMAND_MAKE_ROOM_IN_PLAYER_GANG_FOR_MISSION_PEDS: {
        CollectParameters(1);
        CPlayerPedData* const firstData = FindPlayerPed(-1)->GetPlayerData();
        const uint32          groupId   = firstData->m_nPlayerGroup;
        CPlayerPedData* const data      = FindPlayerPed(-1)->GetPlayerData();
        CPedGroupMembership&  members   = CPedGroups::ms_groups.data()[groupId].GetMembership(); // 0xC09928 + id * 0x2D4
        const int32           excess    = members.CountMembersExcludingLeader() - int32(data->m_nScriptLimitToGangSize) - P(0).iParam;
        if (excess > 0) {
            members.RemoveNFollowers(excess);
        }
        return OR_CONTINUE;
    }
    // 0x47ADB9
    case COMMAND_IS_CHAR_GETTING_IN_TO_A_CAR: {
        CollectParameters(1);
        CPed* const  ped      = PedOf(P(0).iParam);
        auto&        taskMgr  = ped->GetIntelligence()->m_TaskMgr;
        bool         flag     = false;
        if (taskMgr.FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER) || taskMgr.FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)) {
            if (!taskMgr.FindActiveTaskByType(TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL)) {
                flag = true;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x47AE2C
    case COMMAND_SET_UP_SKIP_FOR_SPECIFIC_VEHICLE: {
        CollectParameters(5);
        const CVector   pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float     angle = P(3).fParam;
        CVehicle* const veh   = VehicleOf(P(4).iParam);
        CGameLogic::SetUpSkip(pos, angle, false, veh, false);
        return OR_CONTINUE;
    }
    // 0x47AE95
    case COMMAND_GET_CAR_MODEL_VALUE: {
        CollectParameters(1);
        auto* const mi         = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[P(0).iParam]);
        const uint8 handlingId = static_cast<uint8>(mi->m_nHandlingId); // movzx byte [mi+0x4A]
        P(0).uParam            = gHandlingDataMgr.m_aVehicleHandling.data()[handlingId].m_nMonetaryValue;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47AECF
    case COMMAND_CREATE_CAR_GENERATOR_WITH_PLATE: {
        CollectParameters(12);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z > EXE_MIN_GROUND_Z) { // fcomp + test ah,41h + jne: con z <= -100 o NaN no se suma
            z += EXE_K_0_015;
        }
        const int32 id = CTheCarGenerators::CreateCarGenerator(
            CVector{ x, y, z },
            P(3).fParam,
            P(4).iParam,
            static_cast<int16>(P(5).u16Param),
            static_cast<int16>(P(6).u16Param),
            P(7).u8Param,
            P(8).u8Param,
            P(9).u8Param,
            P(10).u16Param,
            P(11).u16Param,
            0,
            true
        );
        char label[9];
        ReadTextLabelFromScript(label, 9);
        for (int32 i = 0; i < 8; i++) {
            if (label[i] == '_' || label[i] == '\0') {
                label[i] = ' ';
            }
        }
        label[8] = '\0';
        CTheCarGenerators::m_SpecialPlateHandler.Add(id, label);
        P(0).iParam = id;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47AFD2
    case COMMAND_FIND_TRAIN_DIRECTION: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(static_cast<CTrain*>(veh)->trainFlags.bClockwiseDirection); // bit 6 del byte +0x5B8
        return OR_CONTINUE;
    }
    // 0x47B00C
    case COMMAND_SET_AIRCRAFT_CARRIER_SAM_SITE: {
        CollectParameters(1);
        CObject::bAircraftCarrierSamSiteDisabled = P(0).iParam == 0;
        return OR_CONTINUE;
    }
    // 0x47B02B
    case COMMAND_DRAW_LIGHT_WITH_RANGE: {
        CollectParameters(7);
        const CVector point{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector direction{ 0.0f, 0.0f, 0.0f };
        const float   red   = float(P(3).iParam) * EXE_K_1_255;
        const float   green = float(P(4).iParam) * EXE_K_1_255;
        const float   blue  = float(P(5).iParam) * EXE_K_1_255;
        CPointLights::AddLight(PLTYPE_POINTLIGHT, point, direction, P(6).fParam, red, green, blue, 0, true, nullptr);
        return OR_CONTINUE;
    }
    // 0x47B0E7
    case COMMAND_ENABLE_BURGLARY_HOUSES: {
        CollectParameters(1);
        CEntryExitManager::EnableBurglaryHouses(P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x47B109
    case COMMAND_IS_PLAYER_CONTROL_ON: {
        CollectParameters(1);
        CPad* const pad = CPad::GetPad(P(0).iParam);
        UpdateCompareFlag(!pad->bPlayerSafe); // bit 5 del byte +0x10E (DisablePlayerControls), negado
        return OR_CONTINUE;
    }
    // 0x47B145
    case COMMAND_GET_CHAR_AREA_VISIBLE: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        P(0).uParam     = static_cast<uint32>(static_cast<uint8>(ped->GetAreaCode())); // movzx byte [ped+0x2F]: el id de area, no un booleano
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47B179
    case COMMAND_GIVE_NON_PLAYER_CAR_NITRO: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        static_cast<CAutomobile*>(veh)->NitrousControl(1);
        return OR_CONTINUE;
    }
    // 0x47B1A4
    case COMMAND_PLAYER_TAKE_OFF_GOGGLES: {
        CollectParameters(2);
        CPed* const ped = CWorld::Players[P(0).iParam].m_pPed;
        if (P(1).iParam == 0) {
            ped->TakeOffGoggles();
            return OR_CONTINUE;
        }
        if (!ped->m_pGogglesObject) {
            return OR_CONTINUE;
        }
        auto& taskMgr = ped->GetIntelligence()->m_TaskMgr;
        if (taskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY] != nullptr) { // cmp dword [intel+0x10], 0
            return OR_CONTINUE;
        }
        auto* const task = new CTaskComplexUseGoggles();
        taskMgr.SetTask(task, TASK_PRIMARY_PRIMARY, false);
        if (CPlayerPedData* const data = ped->GetPlayerData()) {
            RawField<uint8>(data, 0x85) = 1; // m_bDontAllowWeaponChange
        }
        return OR_CONTINUE;
    }
    // 0x47B250
    case COMMAND_ALLOW_FIXED_CAMERA_COLLISION: {
        CollectParameters(1);
        gAllowScriptedFixedCameraCollision = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47B26F
    case COMMAND_HAS_CHAR_SPOTTED_CHAR_IN_FRONT: {
        CollectParameters(2);
        CPed* const first  = PedOf(P(0).iParam);
        CPed* const second = PedOf(P(1).iParam);
        UpdateCompareFlag(first->OurPedCanSeeThisEntity(second, false));
        return OR_CONTINUE;
    }
    // 0x47B2BC
    case COMMAND_FORCE_BIG_MESSAGE_AND_COUNTER: {
        CollectParameters(1);
        CHud::bScriptForceDisplayWithCounters = P(0).u8Param != 0;
        return OR_CONTINUE;
    }
    // 0x47B2D8
    case COMMAND_SET_VEHICLE_CAMERA_TWEAK: {
        CollectParameters(4);
        // CCamera::m_aCamTweak[4] (0xB6FD5C): modelo, distancia, altura, angulo
        auto& tweak   = TheCamera.m_aCamTweak[4];
        tweak.ModelID = P(0).iParam;
        tweak.Angle   = P(3).fParam; // el EXE escribe 0xB6FD68 (Angle) antes que 0xB6FD60 (Dist) y 0xB6FD64 (Alt)
        tweak.Dist    = P(1).fParam;
        tweak.Alt     = P(2).fParam;
        return OR_CONTINUE;
    }
    // 0x47B314
    case COMMAND_RESET_VEHICLE_CAMERA_TWEAK: {
        auto& tweak   = TheCamera.m_aCamTweak[4];
        tweak.ModelID = -1;
        tweak.Dist    = 1.0f;
        tweak.Alt     = 1.0f;
        tweak.Angle   = 0.0f;
        return OR_CONTINUE;
    }
    // 0x47B343
    case COMMAND_REPORT_MISSION_AUDIO_EVENT_AT_CHAR: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        AudioEngine.ReportMissionAudioEvent(P(1).u16Param, ped);
        return OR_CONTINUE;
    }
    // 0x47B37A
    case COMMAND_DOES_DECISION_MAKER_EXIST: {
        CollectParameters(1);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_DECISION_MAKER);
        bool        flag  = false;
        if (index >= 0 && index < 20) {
            static_cast<void>(CDecisionMakerTypes::GetInstance());
            flag = CDecisionMakerTypes::m_IsActive.data()[index]; // 0xC0B01C + indice
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x47B3BB
    case COMMAND_IGNORE_HEIGHT_DIFFERENCE_FOLLOWING_NODES: {
        CollectParameters(2);
        CPed* const ped                                = PedOf(P(0).iParam);
        ped->bIgnoreHeightDifferenceFollowingNodes     = P(1).iParam != 0; // bit 15 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x47B406
    case COMMAND_SHUT_ALL_CHARS_UP: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            CAEPedSpeechAudioEntity::DisableAllPedSpeech();
        } else {
            CAEPedSpeechAudioEntity::EnableAllPedSpeech();
        }
        return OR_CONTINUE;
    }
    // 0x47B42F
    case COMMAND_SET_CHAR_GET_OUT_UPSIDE_DOWN_CAR: {
        CollectParameters(2);
        CPed* const ped              = PedOf(P(0).iParam);
        ped->bGetOutUpsideDownCar    = P(1).iParam != 0; // bit 17 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x47B479
    case COMMAND_REPORT_MISSION_AUDIO_EVENT_AT_CAR: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        AudioEngine.ReportMissionAudioEvent(P(1).u16Param, veh);
        return OR_CONTINUE;
    }
    // 0x47B4AF
    case COMMAND_DO_WEAPON_STUFF_AT_START_OF_2P_GAME: {
        CGameLogic::DoWeaponStuffAtStartOf2PlayerGame(true);
        return OR_CONTINUE;
    }
    // 0x47B4C0
    case COMMAND_HAS_GAME_JUST_RETURNED_FROM_FRONTEND: {
        UpdateCompareFlag(CPad::GetPad(0)->JustOutOfFrontEnd == 1);
        return OR_CONTINUE;
    }
    // 0x47B4EC
    case COMMAND_GET_CURRENT_LANGUAGE: {
        P(0).iParam = static_cast<int32>(static_cast<int8>(FrontEndMenuManager.m_nPrefsLanguage)); // movsx byte [0xBA67CC]
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47B509
    case COMMAND_IS_OBJECT_INTERSECTING_WORLD: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        UpdateCompareFlag(obj->TestCollision(false)); // llamada virtual +0x34
        return OR_CONTINUE;
    }
    // 0x47B543
    case COMMAND_GET_STRING_WIDTH: {
        char label[9]{};
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const text = TheText.Get(label);
        const float          width = CFont::GetStringWidth(text, true, true);
        P(0).iParam                = static_cast<int32>(width); // _ftol
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47B587
    case COMMAND_RESET_VEHICLE_HYDRAULICS: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if ((veh->m_nHandlingFlagsIntValue & VEHICLE_HANDLING_HYDRAULIC_INST) && veh->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE) {
            static_cast<CAutomobile*>(veh)->m_wMiscComponentAngle = 0; // mov word [veh+0x86C], 0
        }
        return OR_CONTINUE;
    }
    // 0x47B5CB
    case COMMAND_SET_RESPAWN_POINT_FOR_DURATION_OF_MISSION: {
        CollectParameters(3);
        CRestart::SetRespawnPointForDurationOfMission(CVector{ P(0).fParam, P(1).fParam, P(2).fParam });
        return OR_CONTINUE;
    }
    // 0x47B619
    case COMMAND_IS_THIS_MODEL_A_CAR: {
        CollectParameters(1);
        UpdateCompareFlag(CModelInfo::IsCarModel(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x47B63A
    case COMMAND_SWITCH_ON_GROUND_SEARCHLIGHT: {
        CollectParameters(2);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEARCH_LIGHT);
        if (index >= 0) {
            RawField<uint8>(&CTheScripts::ScriptSearchLightArray.data()[index], 2) = P(1).u8Param; // byte +2 del foco (0xA94D6A + indice * 0x7C)
        }
        return OR_CONTINUE;
    }
    // 0x47B672
    case COMMAND_IS_GANG_WAR_FIGHTING_GOING_ON: {
        UpdateCompareFlag(CGangWars::GangWarFightingGoingOn());
        return OR_CONTINUE;
    }
    // 0x47B698
    case COMMAND_IS_NEXT_STATION_ALLOWED: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(CTrain::IsNextStationAllowed(static_cast<CTrain*>(veh)));
        return OR_CONTINUE;
    }
    // 0x47B6C0
    case COMMAND_SKIP_TO_NEXT_ALLOWED_STATION: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CTrain::SkipToNextAllowedStation(static_cast<CTrain*>(veh));
        return OR_CONTINUE;
    }
    // 0x47B6EB
    case COMMAND_GET_STRING_WIDTH_WITH_NUMBER: {
        char label[9]{};
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const text = TheText.Get(label);
        CollectParameters(1);
        GxtChar message[0x100]{};
        CMessages::InsertNumberInString(text, P(0).iParam, -1, -1, -1, -1, -1, message);
        const float width = CFont::GetStringWidth(message, true, true);
        P(0).iParam       = static_cast<int32>(width); // _ftol
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47B75C
    case COMMAND_SHUT_CHAR_UP_FOR_SCRIPTED_SPEECH: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        if (P(1).iParam != 0) {
            if (ped) {
                ped->DisablePedSpeechForScriptSpeech(false);
            }
        } else if (ped) {
            ped->EnablePedSpeechForScriptSpeech();
        }
        return OR_CONTINUE;
    }
    // 0x47B7A2
    case COMMAND_ENABLE_DISABLED_ATTRACTORS_ON_OBJECT: {
        CollectParameters(2);
        CObject* const obj                         = ObjectOf(P(0).iParam);
        obj->objectFlags.bEnableDisabledAttractors = P(1).iParam & 1; // bit 24 del dword +0x140
        return OR_CONTINUE;
    }
    // 0x47B7E4
    case COMMAND_LOAD_SCENE_IN_DIRECTION: {
        CollectParameters(4);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   heading = P(3).fParam;
        CTimer::Stop();
        CRenderer::RequestObjectsInDirection(pos, heading * EXE_DEG_TO_RAD, 0x20);
        CStreaming::LoadScene(pos);
        CTimer::Update();
        return OR_CONTINUE;
    }
    // 0x47B84C
    case COMMAND_IS_PLAYER_USING_JETPACK: {
        CollectParameters(1);
        CPed* const ped = CWorld::Players[P(0).iParam].m_pPed;
        UpdateCompareFlag(ped->GetIntelligence()->GetTaskJetPack() != nullptr);
        return OR_CONTINUE;
    }
    // 0x47B88E
    case COMMAND_CLEAR_THIS_PRINT_BIG_NOW: {
        CollectParameters(1);
        CMessages::ClearThisPrintBigNow(static_cast<eMessageStyle>(P(0).iParam - 1));
        return OR_CONTINUE;
    }
    // 0x47B8AD
    case COMMAND_HAS_LANGUAGE_CHANGED: {
        UpdateCompareFlag(FrontEndMenuManager.HasLanguageChanged());
        return OR_CONTINUE;
    }
    // 0x47B8CE
    case COMMAND_INCREMENT_INT_STAT_NO_MESSAGE: {
        CollectParameters(2);
        const float value = static_cast<float>(P(1).iParam); // fild
        CStats::IncrementStat(static_cast<eStats>(P(0).u16Param), value);
        return OR_CONTINUE;
    }
    // 0x47B8FA
    case COMMAND_SET_EXTRA_CAR_COLOURS: {
        CollectParameters(3);
        CVehicle* const veh      = VehicleOf(P(0).iParam);
        veh->m_nTertiaryColor    = P(1).u8Param;
        veh->m_nQuaternaryColor  = P(2).u8Param;
        return OR_CONTINUE;
    }
    // 0x47B933
    case COMMAND_GET_EXTRA_CAR_COLOURS: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).uParam         = veh->m_nTertiaryColor;
        P(1).uParam         = veh->m_nQuaternaryColor;
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x47B977
    case COMMAND_MANAGE_ALL_POPULATION: {
        CPopulation::ManageAllPopulation();
        return OR_CONTINUE;
    }
    // 0x47B983
    case COMMAND_SET_NO_RESPRAYS: {
        CollectParameters(1);
        const bool noResprays = P(0).iParam != 0;
        CGarages::NoResprays  = noResprays;
        CGarages::AllRespraysCloseOrOpen(!noResprays);
        return OR_CONTINUE;
    }
    // 0x47B9AF
    case COMMAND_HAS_CAR_BEEN_RESPRAYED: {
        CollectParameters(1);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        bool            flag = false;
        if (veh->vehicleFlags.bHasBeenResprayed) { // bit 0 del byte +0x42F
            flag                                = true;
            veh->vehicleFlags.bHasBeenResprayed = false;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x47B9FB
    case COMMAND_ATTACH_MISSION_AUDIO_TO_CAR: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(1).iParam);
        AudioEngine.AttachMissionAudioToPhysical(static_cast<uint8>(P(0).u8Param - 1), veh); // dec cl
        return OR_CONTINUE;
    }
    // 0x47BA32
    case COMMAND_SET_HAS_BEEN_OWNED_FOR_CAR_GENERATOR: {
        CollectParameters(2);
        CCarGenerator* const gen            = CTheCarGenerators::Get(P(0).u16Param); // 0xC27AD0 + indice * 0x20
        gen->bPlayerHasAlreadyOwnedCar      = P(1).iParam != 0;                      // bit 3 del byte +0xD
        return OR_CONTINUE;
    }
    // 0x47BA6E
    case COMMAND_SET_UP_CONVERSATION_NODE_WITH_SCRIPTED_SPEECH: {
        char question[9]{};
        char answerYes[9]{};
        char answerNo[9]{};
        ReadTextLabelFromScript(question, 8);
        ReadTextLabelFromScript(answerYes, 8);
        ReadTextLabelFromScript(answerNo, 8);
        CollectParameters(3);
        CConversations::SetUpConversationNode(question, answerYes, answerNo, -P(0).iParam, -P(1).iParam, -P(2).iParam);
        return OR_CONTINUE;
    }
    // 0x47BAD9
    case COMMAND_SET_AREA_NAME: {
        char label[9]{};
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const text = TheText.Get(label);
        CHud::SetZoneName(text, true);
        return OR_CONTINUE;
    }
    // 0x47BB08
    case COMMAND_TASK_PLAY_ANIM_SECONDARY: {
        PlayAnimScriptCommand(command);
        return OR_CONTINUE;
    }
    // 0x47BB17
    case COMMAND_IS_CHAR_TOUCHING_CHAR: {
        CollectParameters(2);
        CPed* const     first    = PedOf(P(0).iParam);
        CPed* const     second   = PedOf(P(1).iParam);
        CVehicle*       firstVeh = nullptr;
        CVehicle*       secondVeh = nullptr;
        if (first->bInVehicle) { // test dword [ped+0x46C], 0x100
            firstVeh = first->m_pVehicle;
        }
        if (second->bInVehicle) {
            secondVeh = second->m_pVehicle;
        }
        bool touching;
        if (firstVeh) {
            if (secondVeh) {
                touching = firstVeh->GetHasCollidedWith(secondVeh) || secondVeh->GetHasCollidedWith(firstVeh);
            } else {
                touching = second->GetHasCollidedWith(firstVeh);
            }
        } else if (secondVeh) {
            touching = first->GetHasCollidedWith(secondVeh);
        } else {
            touching = first->GetHasCollidedWith(second);
        }
        UpdateCompareFlag(touching);
        return OR_CONTINUE;
    }
    // 0x47BBB6
    case COMMAND_DISABLE_HELI_AUDIO: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->m_vehicleAudio.EnableHelicoptor();
        } else {
            veh->m_vehicleAudio.DisableHelicoptor();
        }
        return OR_CONTINUE;
    }
    // 0x47BBF7
    case COMMAND_TASK_HAND_GESTURE: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        CPed* const target    = PedOf(P(1).iParam);
        auto* const task      = new CTaskComplexSignalAtPed(target, -1, false);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x47BC69
    case COMMAND_TAKE_PHOTO: {
        CollectParameters(1);
        CWeapon::ms_bTakePhoto                 = true;
        CPostEffects::m_bSavePhotoFromScript   = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47BC8F
    case COMMAND_INCREMENT_FLOAT_STAT_NO_MESSAGE: {
        CollectParameters(2);
        CStats::IncrementStat(static_cast<eStats>(P(0).u16Param), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x47BCB7
    case COMMAND_SET_PLAYER_GROUP_TO_FOLLOW_ALWAYS: {
        CollectParameters(2);
        const int32 playerId = P(0).iParam;
        const bool  enable   = P(1).iParam != 0;
        FindPlayerPed(playerId)->ForceGroupToAlwaysFollow(enable);
        return OR_CONTINUE;
    }
    // 0x47BCE8
    case COMMAND_IMPROVE_CAR_BY_CHEATING: {
        CollectParameters(2);
        CVehicle* const veh                = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bUseCarCheats    = P(1).u8Param & 1; // shl dl,1 + mascara 2 sobre el byte +0x42F
        return OR_CONTINUE;
    }
    // 0x47BD27
    case COMMAND_CHANGE_CAR_COLOUR_FROM_MENU: {
        CollectParameters(4);
        CVehicle* const veh    = VehicleOf(P(1).iParam);
        const uint8     colour = CMenuSystem::GetCarColourFromGrid(P(0).u8Param, P(3).u8Param);
        if (P(2).iParam == 1) {
            veh->m_nPrimaryColor = colour;
        } else {
            veh->m_nSecondaryColor = colour;
        }
        return OR_CONTINUE;
    }
    // 0x47BD82
    case COMMAND_HIGHLIGHT_MENU_ITEM: {
        CollectParameters(3);
        CMenuSystem::HighlightOneItem(P(0).u8Param, P(1).u8Param, P(2).u8Param != 0);
        return OR_CONTINUE;
    }
    // 0x47BDB4
    case COMMAND_SET_DISABLE_MILITARY_ZONES: {
        CollectParameters(1);
        CCullZones::bMilitaryZonesDisabled = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47BDD2
    case COMMAND_SET_CAMERA_POSITION_UNFIXED: {
        CollectParameters(2);
        CCam& cam              = TheCamera.m_aCams.data()[TheCamera.m_nActiveCam]; // 0xB6F19C + activa * 0x238
        cam.m_fVerticalAngle   = P(0).fParam;                                       // +0xAC
        cam.m_fHorizontalAngle = P(1).fParam;                                       // +0xBC
        if (cam.m_nMode == MODE_FOLLOWPED) {
            cam.Process_FollowPed_SA(FindPlayerPed(-1)->GetPosition(), 0.0f, 0.0f, 0.0f, true);
            return OR_CONTINUE;
        }
        if (FindPlayerVehicle(-1, false)) {
            const auto mode = cam.m_nMode;
            if (mode == MODE_CAM_ON_A_STRING || mode == MODE_BEHINDBOAT || mode == MODE_BEHINDCAR) { // 0x12, 0x16, 3
                cam.Process_FollowCar_SA(FindPlayerVehicle(-1, false)->GetPosition(), 0.0f, 0.0f, 0.0f, true);
            }
        }
        return OR_CONTINUE;
    }
    // 0x47BED3
    case COMMAND_SET_RADIO_TO_PLAYERS_FAVOURITE_STATION: {
        AudioEngine.RetuneRadio(CStats::FindMostFavoriteRadioStation());
        return OR_CONTINUE;
    }
    // 0x47BEE7
    case COMMAND_SET_DEATH_WEAPONS_PERSIST: {
        CollectParameters(2);
        CPed* const ped            = PedOf(P(0).iParam);
        ped->bDeathPickupsPersist  = P(1).iParam != 0; // bit 19 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x47BF2B: comandos que devuelven 0 sin leer parametros
    case COMMAND_SET_ALL_CARS_IN_AREA_VISIBLE:
    case COMMAND_IS_AMBIENT_CRIME_ENABLED:
    case COMMAND_SET_CHAR_CAN_CLIMB_OUT_WATER:
    case COMMAND_PLAYER_PUT_ON_GOGGLES:
    case COMMAND_GET_RANDOM_TRAIN_IN_SPHERE_NO_SAVE:
    case COMMAND_SET_MENU_HEADER_ORIENTATION:
    case COMMAND_IS_THIS_MODEL_A_BIKE:
    case COMMAND_SET_VEHICLE_FIRING_RATE_MULTIPLIER:
    case COMMAND_GET_VEHICLE_FIRING_RATE_MULTIPLIER:
    case COMMAND_BLOCK_VEHICLE_MODEL:
        return OR_CONTINUE;
    // 0x47BF2F: comandos sin caso (el EXE los manda al destino de -1)
    case COMMAND_IS_ITALIAN_GAME:
    case COMMAND_IS_SPANISH_GAME:
    case COMMAND_RESTORE_PLAYER_AFTER_2P_GAME:
    default:
        return OR_INTERRUPT;
    }
}
