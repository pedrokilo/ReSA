#include "StdInc.h"

#include <bit>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "PedGroups.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Camera.h"
#include "Streaming.h"
#include "PathFind.h"
#include "Pad.h"
#include "TheZones.h"
#include "Darkel.h"
#include "ModelIndices.h"
#include "Door.h"
#include "DamageManager.h"
#include "Hud.h"
#include "HudColours.h"
#include "MenuSystem.h"
#include "Weather.h"
#include "PostEffects.h"
#include "GangWars.h"
#include "Garage.h"
#include "Garages.h"
#include "Shopping.h"
#include "Pickups.h"
#include "Stats.h"
#include "Explosion.h"
#include "GameLogic.h"
#include "EntryExit.h"
#include "PedAttractorManager.h"
#include "ScriptsForBrains.h"
#include "StreamedScripts.h"
#include "Audio/AudioEngine.h"
#include "Audio/AudioZones.h"
#include "Fx/FxSystem.h"
#include "Fx/FxPrtMult.h"
#include "X87Math.h"

// F4 de codigo vivo (sin validar): 0x4762D0 (5948 bytes hasta la tabla de saltos 0x477C0C), CRunningScript::ProcessCommands2300To2399, el despachador original de los comandos de script 2300..2399 (el que usa
// ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo sin SEH (solo `sub esp,0x84` y push de ebx/esi/edi): ningun case
// reserva memoria con `new`.
// Salto con una tabla directa de 100 entradas (0x477C0C; `lea eax,[edi-0x8FC]; cmp eax,0x63; ja 0x477BFC; jmp [eax*4+0x477C0C]`): 18 comandos van a 0x477BEE (devuelven 0 sin leer parametros: 2300 2306 2307
// 2314 2315 2321 2331 2337 2343 2346 2348 2349 2354 2357 2360 2364 2367 2371) y los fuera de rango a 0x477BFC (OR_INTERRUPT, -1). 2392, 2393 y 2394 comparten el cuerpo de 0x4778AA. Los parametros se leen
// de ScriptParams (0xA43C78). Los indices y desplazamientos que vienen del script se usan sin comprobar rangos, como el EXE. Las comparaciones x87 se escriben con la semantica del EXE (con NaN: `test ah,41h;
// jne` no se toma el salto del intercambio -> `a > b` estricto; `test ah,1; jne` -> salta con a < b o NaN).
namespace {
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x859014: float -100.0 (0xC2C80000)
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x858B8C: float 0.5
constexpr float EXE_HALF = std::bit_cast<float>(0x3F000000u);
// 0x859520 (1/640) y 0x859524 (1/448): factores de pantalla de las ventanas de script
constexpr float EXE_K_1_640 = std::bit_cast<float>(0x3ACCCCCDu);
constexpr float EXE_K_1_448 = std::bit_cast<float>(0x3B124925u);
// 0xC47A0000: float -1000.0, nivel de agua cuando no hay agua
constexpr float EXE_NO_WATER = std::bit_cast<float>(0xC47A0000u);
// 0x859EE0: texto "DUMMY" (nombre vacio de una ventana)
constexpr const char* EXE_DUMMY = "DUMMY";

inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef), 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x465040 (CObjectPool::GetAtRef): sin comprobar el resultado (salvo donde el EXE lo comprueba), como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CObject* ObjectOf(int32 handle) {
    return CPools::GetObject(handle);
}
// 0xA44B88 - id * 0x1C: modelo negativo del script = entrada -id de CTheScripts::UsedObjectArray (su nModelIndex); si no, el propio valor.
inline int32 ScriptModelIndex(int32 id) {
    return id >= 0 ? id : CTheScripts::UsedObjectArray[-id].nModelIndex;
}
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
// Puerta `idx` de un vehiculo: 0x5B8 + idx * 0x18 (CAutomobile::m_doors), sin comprobar el indice
inline CDoor* DoorAt(CVehicle* veh, int32 idx) {
    return reinterpret_cast<CDoor*>(static_cast<uint8*>(static_cast<void*>(veh)) + 0x5B8) + idx;
}
} // namespace

static_assert(offsetof(CPhysical, m_fMass) == 0x8C && offsetof(CPhysical, m_fTurnMass) == 0x90);
static_assert(offsetof(CPhysical, m_vecTurnSpeed) == 0x50);
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40);
static_assert(offsetof(CObject, m_nObjectFlags) == 0x140);
static_assert(offsetof(CObject, m_nLastWeaponDamage) == 0x148);
static_assert(offsetof(CPad, bDisablePlayerJump) == 0x11E);
static_assert(offsetof(CPlane, m_fLandingGearStatus) == 0x9CC);
static_assert(offsetof(CAutomobile, m_damageManager) == 0x5A0 && offsetof(CAutomobile, m_swingingChassis) == 0x70C);
static_assert(sizeof(CDoor) == 0x18);
static_assert(offsetof(CPed, m_pEnex) == 0x78C);
static_assert(offsetof(CPlayerInfo, m_nMaxArmour) == 0x150);
static_assert(offsetof(CPedGroup, m_bMembersEnterLeadersVehicle) == 4);
static_assert(offsetof(CEntryExit, m_pLink) == 0x38 && offsetof(CEntryExit, m_vecExitPos) == 0x20 && offsetof(CEntryExit, m_fExitAngle) == 0x2C);
static_assert(sizeof(CStreamedScripts::CStreamedScriptInfo) == 0x20);
static_assert(sizeof(tScriptRectangle) == 0x3C);
static_assert(offsetof(tScriptRectangle, gxt1) == 0x20 && offsetof(tScriptRectangle, gxt2) == 0x2A && offsetof(tScriptRectangle, m_Alignment) == 0x34 && offsetof(tScriptRectangle, m_nTextboxStyle) == 0x38);
static_assert(offsetof(tScriptSearchlight, m_bClipIfColliding) == 1 && sizeof(tScriptSearchlight) == 0x7C);

// 0x4762D0
OpcodeResult CRunningScript::ProcessCommands2300To2399(int32 command) {
    switch (command) {
    // 0x477BEE (destino comun: devuelve 0 sin leer parametros)
    case COMMAND_GET_MENU_POSITION:
    case COMMAND_SET_OBJECT_BEEN_PHOTOGRAPHED_FLAG:
    case COMMAND_SET_CHAR_BEEN_PHOTOGRAPHED_FLAG:
    case COMMAND_IS_PLAYBACK_FOR_CAR_PAUSED:
    case COMMAND_TRIGGER_PED_BOUNCE:
    case COMMAND_REGISTER_STREAMED_SCRIPT:
    case COMMAND_RESET_LATEST_CONSOLE_COMMAND:
    case COMMAND_CAMERA_SET_SHAKE_SIMULATION:
    case COMMAND_ALLOCATE_STREAMED_SCRIPT_TO_PED_GENERATOR:
    case COMMAND_SET_PLAYER_CAN_BE_DAMAGED:
    case COMMAND_GET_PLAYERS_GANG_IN_CAR_ACTIVE:
    case COMMAND_SET_PLAYERS_GANG_IN_CAR_ACTIVE:
    case COMMAND_CAMERA_IS_FOV_RUNNING:
    case COMMAND_CAMERA_IS_SHAKE_RUNNING:
    case COMMAND_CLEAR_ALL_QUEUED_DIALOGUE:
    case COMMAND_SET_ONSCREEN_COUNTER_COLOUR:
    case COMMAND_IS_FIRE_BUTTON_PRESSED:
    case COMMAND_SET_CAMERA_BEHIND_CHAR:
        return OR_CONTINUE;

    // 0x4762F8
    case COMMAND_SET_HEATHAZE_EFFECT: {
        CollectParameters(1);
        CPostEffects::ScriptHeatHazeFXSwitch(P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x476323
    case COMMAND_IS_HELP_MESSAGE_BEING_DISPLAYED: {
        UpdateCompareFlag(CHud::HelpMessageDisplayed());
        return OR_CONTINUE;
    }
    // 0x476346
    case COMMAND_HAS_OBJECT_BEEN_DAMAGED_BY_WEAPON: {
        CollectParameters(2);
        CObject* const obj  = ObjectOf(P(0).iParam);
        bool           flag = false;
        if (obj) {
            const int32 weapon  = P(1).iParam;
            const int32 damaged = static_cast<int8>(obj->m_nLastWeaponDamage); // movsx byte
            if (weapon == 0x38 || weapon == 0x39) {
                flag = CDarkel::CheckDamagedWeaponType(static_cast<eWeaponType>(damaged), static_cast<eWeaponType>(weapon));
            } else {
                flag = damaged == weapon;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x4763BB
    case COMMAND_CLEAR_OBJECT_LAST_WEAPON_DAMAGE: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (obj) {
            obj->m_nLastWeaponDamage = 0xFF;
        }
        return OR_CONTINUE;
    }
    // 0x4763F2
    case COMMAND_SET_PLAYER_JUMP_BUTTON: {
        CollectParameters(2);
        CPad::GetPad(P(0).iParam)->bDisablePlayerJump = P(1).iParam == 0;
        return OR_CONTINUE;
    }
    // 0x47644C
    case COMMAND_GET_HUD_COLOUR: {
        CollectParameters(1);
        const CRGBA colour = HudColour.GetRGB(static_cast<eHudColours>(P(0).u8Param));
        P(0).uParam        = colour.r;
        P(1).uParam        = colour.g;
        P(2).uParam        = colour.b;
        P(3).uParam        = colour.a;
        StoreParameters(4);
        return OR_CONTINUE;
    }
    // 0x4764AF
    case COMMAND_LOCK_DOOR: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            obj->LockDoor(); // 0x59F5C0
            return OR_CONTINUE;
        }
        obj->m_nPhysicalFlags &= 0xFFFFFFF3u; // bDisableCollisionForce y bCollidable (bits 2 y 3)
        obj->m_vecTurnSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        obj->SetIsStatic(true); // virtual +0x10
        return OR_CONTINUE;
    }
    // 0x47652F
    case COMMAND_SET_OBJECT_MASS: {
        CollectParameters(2);
        ObjectOf(P(0).iParam)->m_fMass = P(1).fParam;
        return OR_CONTINUE;
    }
    // 0x476563
    case COMMAND_GET_OBJECT_MASS: {
        CollectParameters(1);
        P(0).fParam = ObjectOf(P(0).iParam)->m_fMass;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4765A0
    case COMMAND_SET_OBJECT_TURN_MASS: {
        CollectParameters(2);
        ObjectOf(P(0).iParam)->m_fTurnMass = P(1).fParam;
        return OR_CONTINUE;
    }
    // 0x4765D5
    case COMMAND_GET_OBJECT_TURN_MASS: {
        CollectParameters(1);
        P(0).fParam = ObjectOf(P(0).iParam)->m_fTurnMass;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x476612
    case COMMAND_SET_SPECIFIC_ZONE_TO_TRIGGER_GANG_WAR: {
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const int16 zone = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO); // tipo 2
        CGangWars::SetSpecificZoneToTriggerGangWar(zone);
        return OR_CONTINUE;
    }
    // 0x476646
    case COMMAND_CLEAR_SPECIFIC_ZONES_TO_TRIGGER_GANG_WAR: {
        CGangWars::ClearSpecificZonesToTriggerGangWar();
        return OR_CONTINUE;
    }
    // 0x476659
    case COMMAND_SET_ACTIVE_MENU_ITEM: {
        CollectParameters(2);
        CMenuSystem::SetActiveMenuItem(static_cast<MenuId>(P(0).u8Param), static_cast<int8>(P(1).u8Param));
        return OR_CONTINUE;
    }
    // 0x47668A
    case COMMAND_MARK_STREAMED_SCRIPT_AS_NO_LONGER_NEEDED: {
        CollectParameters(1);
        const int16 index = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param));
        P(0).iParam       = index; // movsx
        CStreaming::SetMissionDoesntRequireStreamedScript(index);
        return OR_CONTINUE;
    }
    // 0x4766C5
    case COMMAND_REMOVE_STREAMED_SCRIPT: {
        CollectParameters(1);
        const int16 index = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param));
        P(0).iParam       = index; // movsx
        CStreaming::RemoveStreamedScriptModel(index);
        return OR_CONTINUE;
    }
    // 0x476701
    case COMMAND_SET_MESSAGE_FORMATTING: {
        CollectParameters(3);
        CTheScripts::MessageCentre         = P(1).u16Param;
        CTheScripts::bUseMessageFormatting = P(0).iParam != 0;
        CTheScripts::MessageWidth          = P(2).u16Param;
        return OR_CONTINUE;
    }
    // 0x476742
    case COMMAND_START_NEW_STREAMED_SCRIPT: {
        CollectParameters(1);
        const int16 index = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param));
        P(0).iParam       = index; // movsx
        CRunningScript* const newScript = CTheScripts::StreamedScripts.StartNewStreamedScript(index);
        ReadParametersForNewlyStartedScript(newScript);
        return OR_CONTINUE;
    }
    // 0x476788
    case COMMAND_REGISTER_STREAMED_SCRIPT_INTERNAL: {
        CollectParameters(1);
        return OR_CONTINUE;
    }
    // 0x47679F
    case COMMAND_SET_WEATHER_TO_APPROPRIATE_TYPE_NOW: {
        CWeather::SetWeatherToAppropriateTypeNow();
        return OR_CONTINUE;
    }
    // 0x4767B2
    case COMMAND_WINCH_CAN_PICK_OBJECT_UP: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            obj->m_nObjectFlags |= 0x40000u; // bCanBeAttachedToMagnet
        } else {
            obj->m_nObjectFlags &= 0xFFFBFFFFu;
        }
        return OR_CONTINUE;
    }
    // 0x476810
    case COMMAND_SWITCH_AUDIO_ZONE: {
        char label[8];
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        CAudioZones::SwitchAudioZone(label, P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x47684E
    case COMMAND_SET_CAR_ENGINE_ON: {
        CollectParameters(2);
        const bool    on  = P(1).iParam != 0;
        CVehicle* const veh = VehicleOf(P(0).iParam);
        // CVehicle::SetEngineOn(on) (0x41BDD0): con el motor roto lo apaga; si no, lo pone al bit 0 del argumento
        if (veh->vehicleFlags.bEngineBroken) {
            veh->vehicleFlags.bEngineOn = false;
        } else {
            veh->vehicleFlags.bEngineOn = on;
        }
        return OR_CONTINUE;
    }
    // 0x47688A
    case COMMAND_SET_CAR_LIGHTS_ON: {
        CollectParameters(2);
        CVehicle* const veh         = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bLightsOn = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x4768D5
    case COMMAND_GET_LATEST_CONSOLE_COMMAND: {
        GetPointerToScriptVariable(VAR_GLOBAL); // el EXE descarta el resultado
        return OR_CONTINUE;
    }
    // 0x4768EC
    case COMMAND_GET_USER_OF_CLOSEST_MAP_ATTRACTOR: {
        CollectParameters(5);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   radius = P(3).fParam;
        const int32   model  = P(4).iParam;
        char          label[8];
        ReadTextLabelFromScript(label, 8);
        const int32 modelIndex = model == 0 ? -1 : ScriptModelIndex(model);
        C2dEffectPedAttractor* effect = nullptr;
        CEntity*               entity = nullptr;
        // 0x6002F0 (CAttractorScanner... FindAttractorInRange, estatica de AttractorScanner.cpp y sin declaracion): cdecl con nueve argumentos de pila
        (void)plugin::CallAndReturn<bool, 0x6002F0>(&pos, radius, modelIndex, int32(5), static_cast<const char*>(label), int32(0), int32(0), &effect, &entity);
        P(0).iParam = -1;
        if (effect) {
            CPed* const ped = GetPedAttractorManager()->GetPedUsingEffect(effect, entity);
            if (ped) {
                P(0).iParam = CPools::GetPedRef(ped);
            }
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4769F3
    case COMMAND_SWITCH_ROADS_BACK_TO_ORIGINAL: {
        CollectParameters(6);
        float x1 = P(0).fParam, y1 = P(1).fParam, z1 = P(2).fParam;
        float x2 = P(3).fParam, y2 = P(4).fParam, z2 = P(5).fParam;
        if (x1 > x2) { // fcomp + test ah,41h + jne
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        ThePaths.SwitchRoadsOffInArea(x1, x2, y1, y2, z1, z2, false, true, true);
        return OR_CONTINUE;
    }
    // 0x476ACB
    case COMMAND_SWITCH_PED_ROADS_BACK_TO_ORIGINAL: {
        CollectParameters(6);
        float x1 = P(0).fParam, y1 = P(1).fParam, z1 = P(2).fParam;
        float x2 = P(3).fParam, y2 = P(4).fParam, z2 = P(5).fParam;
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        ThePaths.SwitchPedRoadsOffInArea(x1, x2, y1, y2, z1, z2, false, true);
        return OR_CONTINUE;
    }
    // 0x476BA1
    case COMMAND_GET_PLANE_UNDERCARRIAGE_POSITION: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).fParam         = static_cast<CPlane*>(veh)->m_fLandingGearStatus; // +0x9CC
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x476BDE
    case COMMAND_CAMERA_SET_VECTOR_TRACK: {
        CollectParameters(8);
        CVector     from{ P(0).fParam, P(1).fParam, P(2).fParam };
        CVector     to{ P(3).fParam, P(4).fParam, P(5).fParam };
        const float duration = static_cast<float>(P(6).iParam); // fild
        const bool  ease     = P(7).iParam != 0;
        TheCamera.VectorTrackLinear(&to, &from, duration, ease);
        return OR_CONTINUE;
    }
    // 0x476C58
    case COMMAND_CAMERA_SET_LERP_FOV: {
        CollectParameters(4);
        const float duration = static_cast<float>(P(2).iParam); // fild
        TheCamera.LerpFOV(P(0).fParam, P(1).fParam, duration, P(3).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x476C9B
    case COMMAND_SWITCH_AMBIENT_PLANES: {
        CollectParameters(1);
        CPlane::SwitchAmbientPlanes(P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x476CC6
    case COMMAND_SET_DARKNESS_EFFECT: {
        CollectParameters(2);
        CPostEffects::ScriptDarknessFilterSwitch(P(0).iParam != 0, P(1).iParam);
        return OR_CONTINUE;
    }
    // 0x476CF8
    case COMMAND_CAMERA_RESET_NEW_SCRIPTABLES: {
        TheCamera.InitialiseScriptableComponents();
        return OR_CONTINUE;
    }
    // 0x476D10
    case COMMAND_GET_NUMBER_OF_INSTANCES_OF_STREAMED_SCRIPT: {
        CollectParameters(1);
        const int16 index = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param));
        // m_aScripts[index].m_NumberOfUsers (0xA47B64 + index * 0x20), sin comprobar el indice
        P(0).uParam = *(reinterpret_cast<uint8*>(&CTheScripts::StreamedScripts) + static_cast<int32>(index) * 0x20 + 4);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x476D3F
    case COMMAND_ALLOCATE_STREAMED_SCRIPT_TO_RANDOM_PED: {
        CollectParameters(3);
        const int16 index = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param));
        const uint16 p1   = P(1).u16Param;
        const uint16 p2   = P(2).u16Param;
        P(0).iParam       = index; // movsx
        CTheScripts::ScriptsForBrains.AddNewScriptBrain(index, static_cast<int16>(p1), p2, 0, -1, -1.0f);
        return OR_CONTINUE;
    }
    // 0x476D99
    case COMMAND_ALLOCATE_STREAMED_SCRIPT_TO_OBJECT: {
        CollectParameters(5);
        const int16 index = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param));
        P(0).iParam       = index; // movsx
        const int32 model = ScriptModelIndex(P(1).iParam);
        CTheScripts::ScriptsForBrains.AddNewScriptBrain(index, static_cast<int16>(model), P(2).u16Param, 1, static_cast<int8>(P(4).u8Param), P(3).fParam);
        return OR_CONTINUE;
    }
    // 0x476E09
    case COMMAND_GET_GROUP_MEMBER: {
        CollectParameters(2);
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        CPed* const member     = CPedGroups::ms_groups.data()[groupIndex].m_groupMembership.GetMember(P(1).iParam); // 0xC09928 + indice * 0x2D4
        int32       handle     = -1;
        if (member) {
            handle = CPools::GetPedRef(member);
        }
        P(0).iParam = handle;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x476E6E
    case COMMAND_GET_WATER_HEIGHT_AT_COORDS: {
        CollectParameters(3);
        const float x          = P(0).fParam;
        const float y          = P(1).fParam;
        const bool  withWaves  = P(2).iParam != 0;
        float       waterLevel = 0.0f;
        bool        found;
        if (withWaves) {
            found = CWaterLevel::GetWaterLevel(x, y, 0.0f, waterLevel, 1, nullptr); // 0x6EB690
        } else {
            found = CWaterLevel::GetWaterLevelNoWaves(CVector{ x, y, 0.0f }, &waterLevel, nullptr, nullptr); // 0x6E8580
        }
        if (!found) {
            waterLevel = EXE_NO_WATER;
        }
        P(0).fParam = waterLevel;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x476EE9
    case COMMAND_CAMERA_PERSIST_TRACK: {
        CollectParameters(1);
        TheCamera.m_bCameraPersistTrack = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x476F10
    case COMMAND_CAMERA_PERSIST_POS: {
        CollectParameters(1);
        TheCamera.m_bCameraPersistPosition = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x476F37
    case COMMAND_CAMERA_PERSIST_FOV: {
        CollectParameters(1);
        TheCamera.m_bBlockZoom = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x476F5D
    case COMMAND_CAMERA_IS_VECTOR_MOVE_RUNNING: {
        UpdateCompareFlag(TheCamera.VectorMoveRunning());
        return OR_CONTINUE;
    }
    // 0x476F7D
    case COMMAND_CAMERA_IS_VECTOR_TRACK_RUNNING: {
        UpdateCompareFlag(TheCamera.VectorTrackRunning());
        return OR_CONTINUE;
    }
    // 0x476F9D
    case COMMAND_CAMERA_SET_VECTOR_MOVE: {
        CollectParameters(8);
        CVector     from{ P(0).fParam, P(1).fParam, P(2).fParam };
        CVector     to{ P(3).fParam, P(4).fParam, P(5).fParam };
        const float duration = static_cast<float>(P(6).iParam); // fild
        const bool  ease     = P(7).iParam != 0;
        TheCamera.VectorMoveLinear(&to, &from, duration, ease);
        return OR_CONTINUE;
    }
    // 0x477017
    case COMMAND_DRAW_WINDOW: {
        CollectParameters(4);
        const float p0 = P(0).fParam;
        const float p1 = P(1).fParam;
        const float p2 = P(2).fParam;
        const float p3 = P(3).fParam;
        char        label[8];
        char        header[8];
        ReadTextLabelFromScript(label, 8);
        header[0] = '\0';
        if (_stricmp(label, EXE_DUMMY) != 0) { // 0x8229B6
            size_t i = 0;
            do {
                header[i] = label[i];
            } while (label[i++] != '\0');
        }
        CollectParameters(1);
        const float widthScale  = static_cast<float>(RsGlobal.maximumWidth) * EXE_K_1_640;  // fild [0xC17044] * 1/640
        const float heightScale = static_cast<float>(RsGlobal.maximumHeight) * EXE_K_1_448; // fild [0xC17048] * 1/448
        const uint16 index      = CTheScripts::NumberOfIntroRectanglesThisFrame;
        tScriptRectangle& rect  = CTheScripts::IntroRectangles.data()[index];
        rect.m_nType            = eScriptRectangleType::TEXT;
        rect.m_nTextureId       = -1;
        rect.cornerA.x          = p0 * widthScale;
        rect.cornerA.y          = p1 * heightScale;
        rect.cornerB.x          = widthScale * p2;
        rect.cornerB.y          = heightScale * p3;
        rect.m_nAngle           = 0.0f;
        const uint32 style      = P(0).uParam;
        rect.m_nTransparentColor = CRGBA(0, 0, 0, 0xBE);
        {
            size_t i = 0;
            do {
                rect.gxt1[i] = header[i];
            } while (header[i++] != '\0');
        }
        CTheScripts::NumberOfIntroRectanglesThisFrame++;
        rect.gxt2[0]         = '\0';
        rect.m_Alignment     = static_cast<eFontAlignment>(1);
        rect.m_nTextboxStyle = style;
        return OR_CONTINUE;
    }
    // 0x477175
    case COMMAND_ATTACH_CAR_TO_OBJECT: {
        CollectParameters(8);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CObject* const  obj = ObjectOf(P(1).iParam);
        const CVector   offset{ P(2).fParam, P(3).fParam, P(4).fParam };
        const float     rx = P(5).fParam * EXE_DEG_TO_RAD;
        const float     ry = P(6).fParam * EXE_DEG_TO_RAD;
        const float     rz = P(7).fParam * EXE_DEG_TO_RAD;
        veh->AttachEntityToEntity(obj, offset, CVector{ rx, ry, rz }); // 0x54D570
        return OR_CONTINUE;
    }
    // 0x477239
    case COMMAND_SET_GARAGE_RESPRAY_FREE: {
        char label[8];
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        const int16 garage = CGarages::GetGarageNumberByName(label);
        if (garage < 0) {
            return OR_CONTINUE;
        }
        CGarages::aGarages[garage].m_bRespraysAlwaysFree = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47729D
    case COMMAND_SET_CHAR_BULLETPROOF_VEST: {
        CollectParameters(2);
        CPed* const ped            = PedOf(P(0).iParam);
        ped->bHasBulletProofVest = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x4772E5
    case COMMAND_SET_CINEMA_CAMERA: {
        CollectParameters(1);
        TheCamera.m_bCinemaCamera = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47730C
    case COMMAND_SET_CHAR_FIRE_DAMAGE_MULTIPLIER: {
        CollectParameters(2); // el EXE solo lee los parametros
        return OR_CONTINUE;
    }
    // 0x477323
    case COMMAND_SET_GROUP_FOLLOW_STATUS: {
        CollectParameters(2);
        const int32 group = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        if (group < 0 || group >= 8) {
            return OR_CONTINUE;
        }
        CPedGroups::ms_groups[group].m_bMembersEnterLeadersVehicle = P(1).iParam != 0; // 0xC09924 + indice * 0x2D4
        return OR_CONTINUE;
    }
    // 0x477372
    case COMMAND_SET_SEARCHLIGHT_CLIP_IF_COLLIDING: {
        CollectParameters(2);
        const int32 light = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEARCH_LIGHT);
        if (light < 0) {
            return OR_CONTINUE;
        }
        CTheScripts::ScriptSearchLightArray.data()[light].m_bClipIfColliding = P(1).iParam != 0; // 0xA94D69 + indice * 0x7C
        return OR_CONTINUE;
    }
    // 0x4773CB
    case COMMAND_HAS_PLAYER_BOUGHT_ITEM: {
        CollectParameters(1);
        UpdateCompareFlag(CShopping::HasPlayerBought(P(0).uParam));
        return OR_CONTINUE;
    }
    // 0x4773F8
    case COMMAND_SET_CAMERA_IN_FRONT_OF_CHAR: {
        CollectParameters(1);
        TheCamera.SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString(PedOf(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x47742B
    case COMMAND_GET_PLAYER_MAX_ARMOUR: {
        CollectParameters(1);
        P(0).uParam = CWorld::Players[P(0).iParam].m_nMaxArmour; // 0xB7CEE8 + id * 0x190, movzx byte
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47744C
    case COMMAND_SET_CHAR_USES_UPPERBODY_DAMAGE_ANIMS_ONLY: {
        CollectParameters(2);
        CPed* const ped                   = PedOf(P(0).iParam);
        ped->bUpperBodyDamageAnimsOnly = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47749A
    case COMMAND_SET_CHAR_SAY_CONTEXT: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        const int16 res = ped->Say(static_cast<eGlobalSpeechContext>(P(1).u16Param), 0, 1.0f, false, false, false);
        P(0).iParam     = res; // movsx
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4774DA
    case COMMAND_ADD_EXPLOSION_VARIABLE_SHAKE: {
        CollectParameters(5);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        CExplosion::AddExplosion(nullptr, nullptr, static_cast<eExplosionType>(P(3).iParam), pos, 0, 1, P(4).fParam, 0);
        return OR_CONTINUE;
    }
    // 0x477545
    case COMMAND_ATTACH_MISSION_AUDIO_TO_CHAR: {
        CollectParameters(2);
        CPed* const ped  = PedOf(P(1).iParam);
        const uint8 slot = static_cast<uint8>(P(0).u8Param - 1); // dec dl
        AudioEngine.AttachMissionAudioToPed(slot, ped);
        return OR_CONTINUE;
    }
    // 0x477584
    case COMMAND_UPDATE_PICKUP_MONEY_PER_DAY: {
        CollectParameters(2);
        CPickups::UpdateMoneyPerDay(tPickupReference(P(0).iParam), P(1).u16Param);
        return OR_CONTINUE;
    }
    // 0x4775B3
    case COMMAND_GET_NAME_OF_ENTRY_EXIT_CHAR_USED: {
        CollectParameters(1);
        CPed* const ped  = PedOf(P(0).iParam);
        char* const dest = reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        const CEntryExit* const enex = ped->m_pEnex;
        if (enex) {
            const char* src = enex->m_szName; // strcpy hasta el NUL
            char*       d   = dest;
            char        c;
            do {
                c    = *src++;
                *d++ = c;
            } while (c != '\0');
        } else {
            *dest = '\0';
        }
        return OR_CONTINUE;
    }
    // 0x47760C
    case COMMAND_GET_POSITION_OF_ENTRY_EXIT_CHAR_USED: {
        CollectParameters(1);
        CPed* const     ped   = PedOf(P(0).iParam);
        CEntryExit* const enex = ped->m_pEnex;
        CVector         pos{ 0.0f, 0.0f, 0.0f };
        float           angle = 0.0f;
        if (enex) {
            if (enex->m_pLink) {
                angle = enex->m_fExitAngle * EXE_DEG_TO_RAD;
                pos   = enex->m_vecExitPos;
            } else {
                pos = enex->GetPosition(); // 0x43E090
            }
        }
        P(0).fParam = pos.x;
        P(1).fParam = pos.y;
        P(2).fParam = pos.z;
        P(3).fParam = angle;
        StoreParameters(4);
        return OR_CONTINUE;
    }
    // 0x4776C3
    case COMMAND_IS_CHAR_TALKING: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        UpdateCompareFlag(ped->GetPedTalking());
        return OR_CONTINUE;
    }
    // 0x477703
    case COMMAND_DISABLE_CHAR_SPEECH: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        ped->DisablePedSpeech(P(1).u8Param != 0); // el EXE pasa la palabra de 16 bits; el destino solo lee el byte bajo
        return OR_CONTINUE;
    }
    // 0x47773D
    case COMMAND_ENABLE_CHAR_SPEECH: {
        CollectParameters(1);
        PedOf(P(0).iParam)->EnablePedSpeech();
        return OR_CONTINUE;
    }
    // 0x47776C
    case COMMAND_SET_UP_SKIP: {
        CollectParameters(4);
        CGameLogic::SetUpSkip(CVector{ P(0).fParam, P(1).fParam, P(2).fParam }, P(3).fParam, false, nullptr, false);
        return OR_CONTINUE;
    }
    // 0x4777CE
    case COMMAND_CLEAR_SKIP: {
        CGameLogic::ClearSkip(false);
        return OR_CONTINUE;
    }
    // 0x4777E6
    case COMMAND_PRELOAD_BEAT_TRACK: {
        CollectParameters(1);
        AudioEngine.PreloadBeatTrack(static_cast<int16>(P(0).u16Param));
        return OR_CONTINUE;
    }
    // 0x477811
    case COMMAND_GET_BEAT_TRACK_STATUS: {
        P(0).iParam = static_cast<int32>(AudioEngine.GetBeatTrackStatus()); // movsx al
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x477823
    case COMMAND_PLAY_BEAT_TRACK: {
        AudioEngine.PlayPreloadedBeatTrack(false);
        return OR_CONTINUE;
    }
    // 0x47783D
    case COMMAND_STOP_BEAT_TRACK: {
        AudioEngine.StopBeatTrack();
        return OR_CONTINUE;
    }
    // 0x477855
    case COMMAND_FIND_MAX_NUMBER_OF_GROUP_MEMBERS: {
        P(0).iParam = CStats::FindMaxNumberOfGroupMembers();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47785F
    case COMMAND_VEHICLE_DOES_PROVIDE_COVER: {
        CollectParameters(2);
        CVehicle* const veh                 = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bDoesProvideCover = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x4778AA: CREATE_SNAPSHOT_PICKUP (2392), CREATE_HORSESHOE_PICKUP (2393) y CREATE_OYSTER_PICKUP (2394)
    case COMMAND_CREATE_SNAPSHOT_PICKUP:
    case COMMAND_CREATE_HORSESHOE_PICKUP:
    case COMMAND_CREATE_OYSTER_PICKUP: {
        CollectParameters(3);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        uint32      model;
        ePickupType type;
        eStats      stat;
        if (command == COMMAND_CREATE_SNAPSHOT_PICKUP) {
            model = static_cast<uint16>(ModelIndices::MI_PICKUP_CAMERA); // word [0x8CD5D8]
            type  = PICKUP_SNAPSHOT;                                     // 0x14
            stat  = STAT_TOTAL_SNAPSHOTS;                                // 0xE8
        } else if (command == COMMAND_CREATE_HORSESHOE_PICKUP) {
            model = static_cast<uint16>(ModelIndices::MI_HORSESHOE); // word [0x8CD754]
            type  = PICKUP_ONCE;                                      // 3
            stat  = STAT_TOTAL_HORSESHOES;                            // 0xF2
        } else {
            model = static_cast<uint16>(ModelIndices::MI_OYSTER); // word [0x8CD750]
            type  = PICKUP_ONCE;                                   // 3
            stat  = STAT_TOTAL_OYSTERS;                            // 0xF4
        }
        CStats::IncrementStat(stat, 1.0f);
        if (z <= EXE_MIN_GROUND_Z) { // fcomp + test ah,41h + jp: con NaN no se toma
            z = CWorld::FindGroundZForCoord(x, y) + EXE_HALF;
        }
        P(0).iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, model, type, 0, 0, 0, nullptr);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4779A7
    case COMMAND_HAS_OBJECT_BEEN_UPROOTED: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        UpdateCompareFlag(!obj->GetIsStatic()); // 0x4633E0
        return OR_CONTINUE;
    }
    // 0x4779EC
    case COMMAND_ADD_SMOKE_PARTICLE: {
        CollectParameters(12);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector vel{ P(3).fParam, P(4).fParam, P(5).fParam };
        // FxPrtMult_c::Constructor7f (0x4AB290): rojo, verde, azul, alfa, tamano, 1.0 (rotacion) y vida
        const FxPrtMult_c mult(P(6).fParam, P(7).fParam, P(8).fParam, P(9).fParam, P(10).fParam, 1.0f, P(11).fParam);
        StaticRef<FxSystem_c*>(0xA9AE20)->AddParticle(pos, vel, 0.0f, mult, -1.0f, 1.2f, 0.6f, false); // 0x4AA440
        return OR_CONTINUE;
    }
    // 0x477AA6
    case COMMAND_IS_CHAR_STUCK_UNDER_CAR: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        UpdateCompareFlag(ped->bStuckUnderCar);
        return OR_CONTINUE;
    }
    // 0x477AE5
    case COMMAND_CONTROL_CAR_DOOR: {
        CollectParameters(4);
        CVehicle* const veh      = VehicleOf(P(0).iParam);
        const int32     door     = P(1).iParam;
        const int32     status   = P(2).iParam;
        const float     angle    = static_cast<float>(P(3).iParam); // fild
        if (door < 6) {
            if (angle >= 0.0f) { // test ah,1; jne: salta con angle < 0 o NaN
                DoorAt(veh, door)->Open(angle); // 0x6F4790
            }
            static_cast<CAutomobile*>(veh)->m_damageManager.SetDoorStatus(static_cast<eDoors>(door), static_cast<eDoorStatus>(status)); // 0x6C21C0
            static_cast<CAutomobile*>(veh)->SetDoorDamage(static_cast<eDoors>(door), false); // 0x6B1600
            return OR_CONTINUE;
        }
        if (angle >= 0.0f) {
            static_cast<CAutomobile*>(veh)->m_swingingChassis.Open(angle); // 0x6F4790 sobre +0x70C
        }
        RawField<uint8>(veh, 0x717) = static_cast<uint8>(status);
        return OR_CONTINUE;
    }
    // 0x477B8C
    case COMMAND_GET_DOOR_ANGLE_RATIO: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const int32     door = P(1).iParam;
        if (door < 6) {
            P(0).fParam = DoorAt(veh, door)->m_angle; // +0x5C4 + indice * 0x18
        } else {
            P(0).uParam = RawField<uint32>(veh, 0x718);
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }

    default:
        return OR_INTERRUPT;
    }
}
