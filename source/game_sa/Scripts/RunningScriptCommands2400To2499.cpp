#include "StdInc.h"

#include <bit>
#include <cstring>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Pad.h"
#include "Camera.h"
#include "Stats.h"
#include "Hud.h"
#include "MenuSystem.h"
#include "Shopping.h"
#include "WaterLevel.h"
#include "GameLogic.h"
#include "EntryExitManager.h"
#include "Audio/AudioEngine.h"
#include "FireManager.h"
#include "PathFind.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "Fx/Fx.h"
#include "Fx/FxManager.h"
#include "Conversations/Conversations.h"
#include "VehicleRecording.h"
#include "Population.h"
#include "PostEffects.h"
#include "Darkel.h"
#include "TheZones.h"
#include "Events/EventGunShot.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Train.h"
#include "Entity/Object/Object.h"
#include "Entity/Ped/Ped.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Models/ClumpModelInfo.h"
#include "VisibilityPlugins.h"
#include "Core/KeyGen.h"
#include "Crt/CrtCtype.h"
#include "X87Math.h"
#include "ScriptsForBrains.h"
#include "ScriptResourceManager.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h"

// F4 de codigo vivo (sin validar): 0x478000 (6905 bytes = 0x1AF9, `ret 4` en 0x479AF6 y 3 de relleno hasta la tabla), CRunningScript::ProcessCommands2400To2499, el despachador original de los comandos de script
// 2400..2499 (ranura 24 de la tabla 0x8A6168; la usa ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con
// manejador 0x83828E, 1 estado de desenrollado): solo protege el `new` de la tarea de COMMAND_TASK_SET_IGNORE_WEAPON_RANGE_FLAG, en C++ no hace falta.
// Salto con una tabla directa de 100 entradas (0x479AFC): 11 comandos van a 0x479AD8 (devuelven 0 sin leer parametros: 2402 2403 2425 2430 2431 2447 2448 2451 2457 2469 2481) y los fuera de rango a
// 0x479ADC (OR_INTERRUPT, -1; ningun comando del rango 2400..2499 cae ahi). COMMAND_GET_CURRENT_CAR_MOD usa ademas una tabla secundaria de 17 entradas (0x479C8C). Cada `case` replica el orden de
// colaboradores del desensamblado; los parametros se leen de ScriptParams (0xA43C78).
// Las comparaciones x87 (`fcom` + `test ah,..` + `jp`/`jne`) se escriben con la semantica del EXE: con NaN el salto de rechazo se toma. Los indices y desplazamientos que vienen del script se usan sin
// comprobar rangos, como el EXE.
namespace {
// 0x859520 y 0x859524: floats 1/640 (0x3ACCCCCD) y 1/448 (0x3B124925), factores de pantalla de CREATE_MENU_GRID
constexpr float EXE_SCREEN_K_X = std::bit_cast<float>(0x3ACCCCCDu);
constexpr float EXE_SCREEN_K_Y = std::bit_cast<float>(0x3B124925u);
// 0x859014: float -100.0 (0xC2C80000)
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x859EE0: texto "DUMMY" con el que CREATE_MENU_GRID decide si copia el titulo
constexpr const char* EXE_MENU_DUMMY_TITLE = "DUMMY";

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
// 0xA44B88 - id * 0x1C: modelo negativo del script = entrada -id de CTheScripts::UsedObjectArray (su nModelIndex); si no, el propio valor.
inline int32 ScriptModelIndex(int32 id) {
    return id >= 0 ? id : CTheScripts::UsedObjectArray[-id].nModelIndex;
}
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
// CModelInfo::ms_modelInfoPtrs[veh->m_nModelIndex] (movsx word [veh+0x22]) como informacion de modelo de vehiculo
inline CVehicleModelInfo* VehicleModelOf(CVehicle* veh) {
    return CModelInfo::ms_modelInfoPtrs[static_cast<int16>(veh->m_nModelIndex)]->AsVehicleModelInfoPtr();
}
// Banderas de manejo (dword +0xD0 de la entrada del vehiculo en cHandlingDataMgr; matriz 0xC2B9DC con paso 0xE0) segun el id de manejo del modelo (byte +0x4A)
inline uint32 HandlingFlagsOfModel(CVehicleModelInfo* model) {
    const uint32 handlingId = static_cast<uint8>(model->m_nHandlingId);
    return StaticRef<uint32>(0xC2BAAC + handlingId * 0xE0u);
}
} // namespace

static_assert(offsetof(CPad, bDisablePlayerCycleWeapon) == 0x11D && offsetof(CPad, bDisablePlayerDisplayVitalStats) == 0x11F);
static_assert(offsetof(CPed, m_nBodypartToRemove) == 0x754);
static_assert(offsetof(CPhysical, m_pEntityIgnoredCollision) == 0x128 && offsetof(CPhysical, m_fContactSurfaceBrightness) == 0x12C);
static_assert(offsetof(CVehicle, vehicleFlags) == 0x428);
static_assert(offsetof(CVehicle, m_nDoorLock) == 0x4F8);
static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_ObstructingEntity) == 0x420);
static_assert(offsetof(CAutomobile, m_damageManager) == 0x5A0);
static_assert(offsetof(CVehicleModelInfo, m_nHandlingId) == 0x4A && offsetof(CVehicleModelInfo, m_nVehicleClass) == 0x4D && offsetof(CVehicleModelInfo, m_nNumColorVariations) == 0x2D0);
static_assert(offsetof(CTaskSimpleSwim, m_nSwimState) == 0xA);
static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6 && offsetof(CRunningScript, m_IsTextBlockOverride) == 0xC8);
static_assert(TASK_SIMPLE_HOLD_ENTITY == 0x133);
static_assert(TASK_SECONDARY_ATTACK == 0 && TASK_SECONDARY_FACIAL_COMPLEX == 3);
static_assert(VAR_GLOBAL == 2);

// 0x478000
OpcodeResult CRunningScript::ProcessCommands2400To2499(int32 command) {
    switch (command) {
    // 0x47803E
    case COMMAND_SET_PLAYER_DISPLAY_VITAL_STATS_BUTTON: {
        CollectParameters(2);
        if (P(1).iParam != 0) {
            CPad::GetPad(P(0).iParam)->bDisablePlayerDisplayVitalStats = false;
            return OR_CONTINUE;
        }
        CPad::GetPad(P(0).iParam)->bDisablePlayerDisplayVitalStats = true;
        return OR_CONTINUE;
    }
    // 0x478089
    case COMMAND_SET_CHAR_KEEP_TASK: {
        CollectParameters(2);
        CPed* const ped            = PedOf(P(0).iParam);
        ped->bKeepTasksAfterCleanUp = P(1).iParam != 0; // bit 4 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x4780D1
    case COMMAND_CREATE_MENU_GRID: {
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        char title[16]{};
        if (ExeCrt::Stricmp(label, EXE_MENU_DUMMY_TITLE) != 0) {
            // copia byte a byte hasta el NUL inclusive
            size_t i = 0;
            char   c;
            do {
                c        = label[i];
                title[i] = c;
                i++;
            } while (c != '\0');
        }
        CollectParameters(7);
        const float scaleX = static_cast<float>(StaticRef<int32>(0xC17044)) * EXE_SCREEN_K_X; // RsGlobal.maximumWidth
        const float x      = P(0).fParam * scaleX;
        const float width  = scaleX * P(2).fParam;
        const float scaleY = static_cast<float>(StaticRef<int32>(0xC17048)) * EXE_SCREEN_K_Y; // RsGlobal.maximumHeight
        const float y      = scaleY * P(1).fParam;
        const MenuId id    = CMenuSystem::CreateNewMenu(
            CMenuSystem::MENU_TYPE_GRID,
            title,
            x,
            y,
            width,
            P(3).u8Param,
            P(4).u8Param != 0,
            P(5).u8Param != 0,
            static_cast<eFontAlignment>(P(6).u8Param)
        );
        P(0).iParam = id; // movzx eax, al
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47819F
    case COMMAND_IS_CHAR_SWIMMING: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        UpdateCompareFlag(ped->GetIntelligence()->GetTaskSwim() != nullptr);
        return OR_CONTINUE;
    }
    // 0x4781E1
    case COMMAND_GET_CHAR_SWIM_STATE: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        P(0).iParam     = static_cast<int16>(ped->GetIntelligence()->GetTaskSwim()->m_nSwimState); // movsx word [task+0xA], sin comprobar la tarea
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x478221
    case COMMAND_START_CHAR_FACIAL_TALK: {
        CollectParameters(2);
        CPed* const ped    = PedOf(P(0).iParam);
        auto* const facial = static_cast<CTaskComplexFacial*>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
        facial->SetRequest(eFacialExpression::TALKING, P(1).iParam, eFacialExpression::NONE, 0);
        return OR_CONTINUE;
    }
    // 0x478266
    case COMMAND_STOP_CHAR_FACIAL_TALK: {
        CollectParameters(1);
        CPed* const ped    = PedOf(P(0).iParam);
        auto* const facial = static_cast<CTaskComplexFacial*>(ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
        facial->StopAll();
        return OR_CONTINUE;
    }
    // 0x47829F
    case COMMAND_IS_BIG_VEHICLE: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(veh->vehicleFlags.bIsBig != 0); // bit 2 del byte +0x429
        return OR_CONTINUE;
    }
    // 0x4782D9
    case COMMAND_SWITCH_POLICE_HELIS: {
        CollectParameters(1);
        CHeli::SwitchPoliceHelis(P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x4782FD
    case COMMAND_STORE_CAR_MOD_STATE: {
        CShopping::StoreVehicleMods();
        return OR_CONTINUE;
    }
    // 0x478309
    case COMMAND_RESTORE_CAR_MOD_STATE: {
        CShopping::RestoreVehicleMods();
        return OR_CONTINUE;
    }
    // 0x478315, tabla secundaria 0x479C8C (17 entradas)
    case COMMAND_GET_CURRENT_CAR_MOD: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        int32           result = -1;
        switch (P(1).uParam) {
        case 0:
            result = veh->GetUpgrade(0);
            break;
        case 1:
            result = veh->GetUpgrade(1);
            if (result == -1) {
                result = veh->GetUpgrade(2);
            }
            break;
        case 2:
            result = veh->GetUpgrade(6);
            break;
        case 3:
            result = veh->GetUpgrade(8);
            if (result == -1) {
                result = veh->GetUpgrade(9);
            }
            break;
        case 4:
            result = veh->GetUpgrade(0xA);
            break;
        case 5:
            result = veh->GetUpgrade(0xB);
            break;
        case 6:
            result = veh->GetUpgrade(0xC);
            break;
        case 7:
            result = veh->GetUpgrade(0xE);
            break;
        case 8:
            result = veh->GetUpgrade(0xF);
            break;
        case 9:
            result = veh->GetUpgrade(0x10);
            break;
        case 10:
            result = veh->GetUpgrade(0x11);
            break;
        case 11:
            break; // 0x4784D1: se queda en -1
        case 12:
            result = veh->GetReplacementUpgrade(2);
            break;
        case 13:
            result = veh->GetReplacementUpgrade(0x13);
            break;
        case 14:
            result = veh->GetReplacementUpgrade(0xC);
            break;
        case 15:
            result = veh->GetReplacementUpgrade(0xD);
            break;
        case 16:
            result = veh->GetReplacementUpgrade(0x14);
            break;
        default:
            break; // fuera de la tabla (comparacion sin signo > 0x10): -1
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4784E6
    case COMMAND_IS_CAR_LOW_RIDER: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(((HandlingFlagsOfModel(VehicleModelOf(veh)) >> 25) & 1) != 0); // bit 25: bLowRider
        return OR_CONTINUE;
    }
    // 0x478534
    case COMMAND_IS_CAR_STREET_RACER: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(((HandlingFlagsOfModel(VehicleModelOf(veh)) >> 26) & 1) != 0); // bit 26: bStreetRacer
        return OR_CONTINUE;
    }
    // 0x478581
    case COMMAND_FORCE_DEATH_RESTART: {
        CGameLogic::ForceDeathRestart();
        return OR_CONTINUE;
    }
    // 0x47858D
    case COMMAND_SYNC_WATER: {
        CWaterLevel::SyncWater();
        return OR_CONTINUE;
    }
    // 0x478599 y 0x479774 (comparten la cola 0x47979C): SET_CHAR_COORDINATES_NO_OFFSET (warpGang=1, offset=0) y
    // SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET (warpGang=0, offset=0)
    case COMMAND_SET_CHAR_COORDINATES_NO_OFFSET: {
        CollectParameters(4);
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        SetCharCoordinates(*PedOf(P(0).iParam), pos, true, false);
        return OR_CONTINUE;
    }
    // 0x4785C6
    case COMMAND_DOES_SCRIPT_FIRE_EXIST: {
        CollectParameters(1);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_FIRE);
        bool        flag  = false;
        if (index >= 0 && index < 0x3C) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x478605
    case COMMAND_RESET_STUFF_UPON_RESURRECTION: {
        CGameLogic::ResetStuffUponResurrection();
        return OR_CONTINUE;
    }
    // 0x478611
    case COMMAND_IS_EMERGENCY_SERVICES_VEHICLE: {
        CollectParameters(1);
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        bool            flag  = false;
        const int32     model = static_cast<int16>(veh->m_nModelIndex);
        // 0x1A0 (ambulance), 0x197 (firetruk), 0x220 (firela)
        if (veh->IsLawEnforcementVehicle() || model == 0x1A0 || model == 0x197 || model == 0x220) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x47866F
    case COMMAND_KILL_FX_SYSTEM_NOW: {
        CollectParameters(1);
        const int32 handle = P(0).iParam;
        const int32 index  = CTheScripts::GetActualScriptThingIndex(handle, SCRIPT_THING_EFFECT_SYSTEM);
        if (index < 0) {
            return OR_CONTINUE;
        }
        FxSystem_c* const system = CTheScripts::ScriptEffectSystemArray.data()[index].m_pFxSystem;
        if (!system) {
            return OR_CONTINUE;
        }
        g_fxMan.DestroyFxSystem(system);
        CTheScripts::RemoveScriptEffectSystem(handle);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_PARTICLE);
        }
        return OR_CONTINUE;
    }
    // 0x4786D6
    case COMMAND_IS_OBJECT_WITHIN_BRAIN_ACTIVATION_RANGE: {
        CollectParameters(1);
        CObject* const obj      = ObjectOf(P(0).iParam);
        const uint8    playerId = CWorld::PlayerInFocus;
        bool           flag     = false;
        if (CWorld::Players[playerId].m_pPed && obj) {
            const CVector pos = FindPlayerCentreOfWorld(playerId);
            if (CTheScripts::ScriptsForBrains.IsObjectWithinBrainActivationRange(obj, pos)) {
                flag = true;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x47875E
    case COMMAND_COPY_SHARED_CHAR_DECISION_MAKER: {
        CollectParameters(1);
        int32 source = -1;
        if (P(0).iParam != -1) {
            source = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_DECISION_MAKER);
        }
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        if (CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_DECISION_MAKER) != -1) {
            P(0).iParam = next;
        } else {
            const int32 copy = CDecisionMakerTypes::GetInstance()->CopyDecisionMaker(source, 0, m_UsesMissionCleanup);
            P(0).iParam      = CTheScripts::GetNewUniqueScriptThingIndex(copy, SCRIPT_THING_DECISION_MAKER);
            if (m_UsesMissionCleanup) {
                CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
            }
        }
        CTheScripts::ScriptResourceManager.AddToResourceManager(P(0).iParam, RESOURCE_TYPE_DECISION_MAKER, this);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x478805
    case COMMAND_REPORT_MISSION_AUDIO_EVENT_AT_POSITION: {
        CollectParameters(4);
        CVector      pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const uint16 eventId = P(3).u16Param;
        AudioEngine.ReportMissionAudioEvent(eventId, pos);
        return OR_CONTINUE;
    }
    // 0x47884B
    case COMMAND_REPORT_MISSION_AUDIO_EVENT_AT_OBJECT: {
        CollectParameters(2);
        CObject* const obj     = ObjectOf(P(0).iParam);
        const uint16   eventId = P(1).u16Param;
        AudioEngine.ReportMissionAudioEvent(eventId, obj);
        return OR_CONTINUE;
    }
    // 0x478881
    case COMMAND_ATTACH_MISSION_AUDIO_TO_OBJECT: {
        CollectParameters(2);
        CObject* const obj  = ObjectOf(P(1).iParam);
        const uint8    slot = static_cast<uint8>(P(0).u8Param - 1); // dec al
        AudioEngine.AttachMissionAudioToObject(slot, obj);
        return OR_CONTINUE;
    }
    // 0x4788B8
    case COMMAND_GET_NUM_CAR_COLOURS: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).uParam         = VehicleModelOf(veh)->m_nNumColorVariations; // movzx byte [model+0x2D0]
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4788FB
    case COMMAND_EXTINGUISH_FIRE_AT_POINT: {
        CollectParameters(4);
        gFireManager.ExtinguishPoint(CVector{ P(0).fParam, P(1).fParam, P(2).fParam }, P(3).fParam);
        return OR_CONTINUE;
    }
    // 0x478952
    case COMMAND_HAS_TRAIN_DERAILED: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(static_cast<CTrain*>(veh)->trainFlags.bNotOnARailRoad != 0); // bit 0 del byte +0x5B9
        return OR_CONTINUE;
    }
    // 0x478988
    case COMMAND_SET_CHAR_FORCE_DIE_IN_CAR: {
        CollectParameters(2);
        CPed* const ped      = PedOf(P(0).iParam);
        ped->bForceDieInCar = (P(1).iParam & 1) != 0; // (P1 << 7) a bit 7 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x4789BF
    case COMMAND_SET_ONLY_CREATE_GANG_MEMBERS: {
        CollectParameters(1);
        CPopulation::m_bOnlyCreateRandomGangMembers = P(0).iParam == 0; // sete (0xC0FCB3)
        return OR_CONTINUE;
    }
    // 0x4789DE
    case COMMAND_GET_OBJECT_MODEL: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        P(0).iParam        = static_cast<int16>(obj->m_nModelIndex); // movsx word
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x478A13
    case COMMAND_SET_CHAR_USES_COLLISION_CLOSEST_OBJECT_OF_TYPE: {
        CollectParameters(7);
        const float px = P(0).fParam;
        const float py = P(1).fParam;
        float       pz = P(2).fParam;
        if (pz <= EXE_MIN_GROUND_Z) { // fcomp + test ah,41h + jp
            pz = CWorld::FindGroundZForCoord(px, py);
        }
        const float   radius = P(3).fParam;
        float         best   = radius + radius; // fadd st0, st0
        const int32   model  = ScriptModelIndex(P(4).iParam);
        const CVector pos{ px, py, pz };
        CEntity*      found[16];
        int16         count = 0;
        CWorld::FindObjectsOfTypeInRange(static_cast<uint32>(model), pos, radius, true, &count, 16, found, true, false, false, true, true);
        CEntity* closest = nullptr;
        if (count > 0) {
            for (uint16 i = 0; static_cast<int32>(i) < static_cast<int32>(count); i++) {
                CEntity* const  entity = found[i];
                const CVector&  epos   = entity->GetPosition();
                const CVector   diff{ epos.x - px, epos.y - py, epos.z - pz };
                const float     dist = diff.Magnitude();
                if (dist < best) { // fcom + test ah,5 + jp: solo si es estrictamente menor (con NaN no)
                    best    = dist;
                    closest = entity;
                }
            }
        }
        CPed* const ped = PedOf(P(6).iParam);
        if (!closest) {
            return OR_CONTINUE;
        }
        ped->m_pEntityIgnoredCollision = (P(5).iParam != 0) ? nullptr : closest;
        return OR_CONTINUE;
    }
    // 0x478BB7
    case COMMAND_CLEAR_ALL_SCRIPT_FIRE_FLAGS: {
        gFireManager.ClearAllScriptFireFlags();
        return OR_CONTINUE;
    }
    // 0x478BC8
    case COMMAND_GET_CAR_BLOCKING_CAR: {
        CollectParameters(1);
        CVehicle* const veh      = VehicleOf(P(0).iParam);
        int32           result   = -1;
        CEntity* const  blocking = veh->m_autoPilot.m_ObstructingEntity;
        if (blocking && blocking->GetIsTypeVehicle()) { // byte [+0x36] & 7 == 2
            result = GetVehiclePool()->GetRef(static_cast<CVehicle*>(blocking));
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x478C1C
    case COMMAND_GET_CURRENT_VEHICLE_PAINTJOB: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).iParam         = veh->GetRemapIndex();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x478C53
    case COMMAND_SET_HELP_MESSAGE_BOX_SIZE: {
        CollectParameters(1);
        CHud::m_fHelpMessageBoxWidth = static_cast<float>(P(0).iParam); // fild + fstp (0x8D0934)
        return OR_CONTINUE;
    }
    // 0x478C6F
    case COMMAND_SET_GUNSHOT_SENSE_RANGE_FOR_RIOT2: {
        CollectParameters(1);
        CEventGunShot::ms_fGunShotSenseRangeForRiot2 = P(0).fParam; // copia del dword (0x8A625C)
        return OR_CONTINUE;
    }
    // 0x478C89: STRING_CAT16 (limite 16) y STRING_CAT8 (limite 8), tres operandos de variable (tipo 2)
    case COMMAND_STRING_CAT16:
    case COMMAND_STRING_CAT8: {
        char* const first  = reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        char* const second = reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        size_t      len1   = 0;
        while (first[len1] != '\0') {
            len1++;
        }
        size_t len2 = 0;
        while (second[len2] != '\0') {
            len2++;
        }
        const size_t limit = (command == COMMAND_STRING_CAT16) ? 16 : 8;
        if (len1 + len2 >= limit) {
            return OR_CONTINUE; // el EXE no lee el tercer operando
        }
        char* const result = reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        // strcpy(result, first) byte a byte hasta el NUL inclusive
        {
            size_t i = 0;
            char   c;
            do {
                c         = first[i];
                result[i] = c;
                i++;
            } while (c != '\0');
        }
        // la longitud de `second` se mide DESPUES de la copia (puede solaparse); copia len+1 bytes al final del resultado: `rep movsd` y luego `rep movsb`
        size_t n = 0;
        while (second[n] != '\0') {
            n++;
        }
        n++;
        char* dst = result;
        while (*dst != '\0') {
            dst++;
        }
        const char* src = second;
        for (size_t words = n >> 2; words != 0; words--) {
            uint32 w;
            std::memcpy(&w, src, sizeof(w));
            std::memcpy(dst, &w, sizeof(w));
            src += 4;
            dst += 4;
        }
        for (size_t rest = n & 3; rest != 0; rest--) {
            *dst++ = *src++;
        }
        return OR_CONTINUE;
    }
    // 0x478D2F
    case COMMAND_GET_CAR_MOVING_COMPONENT_OFFSET: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).fParam         = static_cast<CAutomobile*>(veh)->GetMovingCollisionOffset();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x478D67
    case COMMAND_SET_NAMED_ENTRY_EXIT_FLAG: {
        char name[16]{};
        ReadTextLabelFromScript(name, 8);
        CollectParameters(2);
        CEntryExitManager::SetEntryExitFlagWithName(name, P(0).uParam, P(1).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x478DA5
    case COMMAND_PAUSE_CURRENT_BEAT_TRACK: {
        CollectParameters(1);
        AudioEngine.PauseBeatTrack(P(0).u8Param != 0); // movzx byte
        return OR_CONTINUE;
    }
    // 0x478DC7
    case COMMAND_SET_PLAYER_CYCLE_WEAPON_BUTTON: {
        CollectParameters(2);
        if (P(1).iParam != 0) {
            CPad::GetPad(P(0).iParam)->bDisablePlayerCycleWeapon = false;
            return OR_CONTINUE;
        }
        CPad::GetPad(P(0).iParam)->bDisablePlayerCycleWeapon = true;
        return OR_CONTINUE;
    }
    // 0x478E13
    case COMMAND_MARK_ROAD_NODE_AS_DONT_WANDER: {
        CollectParameters(3);
        ThePaths.MarkRoadNodeAsDontWander(P(0).fParam, P(1).fParam, P(2).fParam);
        return OR_CONTINUE;
    }
    // 0x478E41
    case COMMAND_UNMARK_ALL_ROAD_NODES_AS_DONT_WANDER: {
        ThePaths.UnMarkAllRoadNodesAsDontWander();
        return OR_CONTINUE;
    }
    // 0x478E52
    case COMMAND_SET_CHECKPOINT_HEADING: {
        CollectParameters(2);
        const float heading = P(1).fParam;
        const int32 index   = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_CHECKPOINT);
        if (index < 0) {
            return OR_CONTINUE;
        }
        CCheckpoint* const checkpoint = CTheScripts::ScriptCheckpointArray.data()[index].m_Checkpoint;
        if (!checkpoint) {
            return OR_CONTINUE;
        }
        CCheckpoints::SetHeading(checkpoint->m_ID, heading);
        return OR_CONTINUE;
    }
    // 0x478EA4
    case COMMAND_SET_MISSION_RESPECT_TOTAL: {
        CollectParameters(1);
        CStats::SetStatValue(STAT_RESPECT_MISSION_TOTAL, static_cast<float>(P(0).iParam)); // 0xE4
        return OR_CONTINUE;
    }
    // 0x478ECB
    case COMMAND_AWARD_PLAYER_MISSION_RESPECT: {
        CollectParameters(1);
        CStats::IncrementStat(STAT_RESPECT_MISSION, static_cast<float>(P(0).iParam)); // 0xE0
        return OR_CONTINUE;
    }
    // 0x478EF2
    case COMMAND_SET_CAR_COLLISION: {
        CollectParameters(2);
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        uint32          flags = RawField<uint32>(veh, 0x1C); // CEntity: bit 0 bUsesCollision
        uint32          phys  = RawField<uint32>(veh, 0x40); // CPhysical: bit 1 bApplyGravity
        if (P(1).iParam != 0) {
            flags |= 1;
            phys |= 2;
        } else {
            flags &= 0xFFFFFFFEu;
            phys &= 0xFFFFFFFDu;
        }
        RawField<uint32>(veh, 0x1C) = flags;
        RawField<uint32>(veh, 0x40) = phys;
        return OR_CONTINUE;
    }
    // 0x478F43
    case COMMAND_CHANGE_PLAYBACK_TO_USE_AI: {
        CollectParameters(1);
        CVehicleRecording::ChangeCarPlaybackToUseAI(VehicleOf(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x478F6E
    case COMMAND_CAMERA_SET_SHAKE_SIMULATION_SIMPLE: {
        CollectParameters(3);
        TheCamera.AddShakeSimple(P(0).fParam, P(1).iParam, P(2).fParam);
        return OR_CONTINUE;
    }
    // 0x478F9C
    case COMMAND_IS_NIGHT_VISION_ACTIVE: {
        UpdateCompareFlag(CPostEffects::m_bNightVision);
        return OR_CONTINUE;
    }
    // 0x478FB2
    case COMMAND_SET_CREATE_RANDOM_COPS: {
        CollectParameters(1);
        CPopulation::m_bDontCreateRandomCops = P(0).iParam == 0; // sete (0xC0FCB4)
        return OR_CONTINUE;
    }
    // 0x478FD2
    case COMMAND_TASK_SET_IGNORE_WEAPON_RANGE_FLAG: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        if (pedHandle != -1) {
            CPed* const ped        = PedOf(pedHandle);
            ped->bIgnoreWeaponRange = (P(1).u8Param & 1) != 0; // (byte P1 << 9) a bit 9 del dword +0x478
            return OR_CONTINUE;
        }
        auto* const task = new CTaskSimpleSetCharIgnoreWeaponRangeFlag(P(1).u8Param != 0);
        GivePedScriptedTask(-1, task, command);
        return OR_CONTINUE;
    }
    // 0x47905C
    case COMMAND_TASK_PICK_UP_SECOND_OBJECT: {
        ScriptTaskPickUpObject(command);
        return OR_CONTINUE;
    }
    // 0x47906B
    case COMMAND_DROP_SECOND_OBJECT: {
        CollectParameters(2);
        CPed* const ped  = PedOf(P(0).iParam);
        const bool  flag = P(1).iParam != 0;
        auto&       mgr  = ped->GetTaskManager();
        if (!mgr.GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            return OR_CONTINUE;
        }
        if (mgr.GetTaskSecondary(TASK_SECONDARY_ATTACK)->GetTaskType() != TASK_SIMPLE_HOLD_ENTITY) { // vtable +0x10, 0x133
            return OR_CONTINUE;
        }
        auto* const hold = static_cast<CTaskSimpleHoldEntity*>(mgr.GetTaskSecondary(TASK_SECONDARY_ATTACK));
        hold->DropEntity(ped, flag);
        hold->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr); // vtable +0x18 con (ped, 1, 0)
        return OR_CONTINUE;
    }
    // 0x479107
    case COMMAND_REMOVE_OBJECT_ELEGANTLY: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        CTheScripts::CleanUpThisObject(obj);
        if (obj) {
            if (obj->GetRwObject()) {
                RpClumpForAllAtomics(
                    reinterpret_cast<RpClump*>(obj->GetRwObject()),
                    CClumpModelInfo::SetAtomicRendererCB,                                // 0x4C5280
                    reinterpret_cast<void*>(CVisibilityPlugins::RenderFadingClumpCB)     // 0x733630
                );
            }
            obj->m_nObjectFlags |= 0x400000;
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_OBJECT);
        }
        return OR_CONTINUE;
    }
    // 0x47917C
    case COMMAND_DRAW_CROSSHAIR: {
        CollectParameters(1);
        CTheScripts::bDrawCrossHair = static_cast<eCrossHairType>(P(0).iParam != 0 ? 1 : 0); // dword 0xA44490
        return OR_CONTINUE;
    }
    // 0x47919E
    case COMMAND_SET_UP_CONVERSATION_NODE_WITH_SPEECH: {
        char label1[16]{};
        ReadTextLabelFromScript(label1, 8);
        char label2[16]{};
        ReadTextLabelFromScript(label2, 8);
        char label3[16]{};
        ReadTextLabelFromScript(label3, 8);
        CollectParameters(3);
        CConversations::SetUpConversationNode(label1, label2, label3, P(0).iParam, P(1).iParam, P(2).iParam);
        return OR_CONTINUE;
    }
    // 0x479203
    case COMMAND_SHOW_BLIPS_ON_ALL_LEVELS: {
        CollectParameters(1);
        CTheScripts::RadarShowBlipOnAllLevels = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x479222
    case COMMAND_SET_CHAR_DRUGGED_UP: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        ped->bDruggedUp = P(1).iParam != 0; // bit 10 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x47926D
    case COMMAND_IS_CHAR_HEAD_MISSING: {
        CollectParameters(1);
        CPed* const ped  = PedOf(P(0).iParam);
        bool        flag = false;
        if (ped && ped->bRemoveHead && ped->m_nBodypartToRemove == 2) { // bit 15 del dword +0x46C y byte +0x754
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x4792AF
    case COMMAND_GET_HASH_KEY: {
        char label[16]{};
        ReadTextLabelFromScript(label, 15);
        P(0).uParam = CKeyGen::GetUppercaseKey(label);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4792E5
    case COMMAND_SET_UP_CONVERSATION_END_NODE_WITH_SPEECH: {
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        CConversations::SetUpConversationNode(label, nullptr, nullptr, P(0).iParam, 0, 0);
        return OR_CONTINUE;
    }
    // 0x47931F
    case COMMAND_RANDOM_PASSENGER_SAY: {
        CollectParameters(2);
        CVehicle* const veh       = VehicleOf(P(0).iParam);
        CPed* const     passenger = veh->PickRandomPassenger();
        if (!passenger) {
            return OR_CONTINUE;
        }
        passenger->Say(static_cast<eGlobalSpeechContext>(P(1).u16Param), 0, 1.0f, false, false, false);
        return OR_CONTINUE;
    }
    // 0x47936D
    case COMMAND_HIDE_ALL_FRONTEND_BLIPS: {
        CollectParameters(1);
        CTheScripts::HideAllFrontEndMapBlips = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x47938D
    case COMMAND_SET_PLAYER_IN_CAR_CAMERA_MODE: {
        CollectParameters(1);
        if (P(0).iParam == 4) {
            return OR_CONTINUE;
        }
        StaticRef<int32>(0xB6F0DC) = P(0).iParam; // TheCamera + 0xB4 (distancia de zoom del coche, dword)
        return OR_CONTINUE;
    }
    // 0x4793B0
    case COMMAND_IS_CHAR_IN_ANY_TRAIN: {
        CollectParameters(1);
        CPed* const ped  = PedOf(P(0).iParam);
        bool        flag = false;
        if (ped->bInVehicle && static_cast<int32>(ped->m_pVehicle->m_nVehicleType) == 6) { // VEHICLE_TYPE_TRAIN
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x479401
    case COMMAND_SET_UP_SKIP_AFTER_MISSION: {
        CollectParameters(4);
        CGameLogic::SetUpSkip(CVector{ P(0).fParam, P(1).fParam, P(2).fParam }, P(3).fParam, true, nullptr, false);
        return OR_CONTINUE;
    }
    // 0x47945B
    case COMMAND_SET_VEHICLE_IS_CONSIDERED_BY_PLAYER: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        uint8&          byte = RawField<uint8>(veh, 0x42E); // bit 5
        byte                 = static_cast<uint8>((byte & ~0x20u) | ((P(1).iParam != 0) ? 0x20u : 0u));
        return OR_CONTINUE;
    }
    // 0x47949F
    case COMMAND_GET_RANDOM_CAR_MODEL_IN_MEMORY: {
        CollectParameters(1);
        const bool  flag  = P(0).iParam != 0;
        const int32 model = CPopulation::m_AppropriateLoadedCars.PickRandomCar(false, flag);
        P(0).iParam       = model;
        if (model == -1) {
            P(1).iParam = -1;
            StoreParameters(2);
            return OR_CONTINUE;
        }
        P(1).iParam = static_cast<int32>(static_cast<int8>(CModelInfo::ms_modelInfoPtrs[model]->AsVehicleModelInfoPtr()->m_nVehicleClass)); // movsx byte [+0x4D]
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x479500
    case COMMAND_GET_CAR_DOOR_LOCK_STATUS: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).iParam         = static_cast<int32>(veh->m_nDoorLock);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x479536
    case COMMAND_SET_CLOSEST_ENTRY_EXIT_FLAG: {
        CollectParameters(5);
        const CVector2D pos{ P(0).fParam, P(1).fParam };
        const int32     index = CEntryExitManager::FindNearestEntryExit(pos, P(2).fParam, -1);
        CEntryExitManager::SetEntryExitFlagWithIndex(index, P(3).uParam, P(4).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x47958C
    case COMMAND_SET_CHAR_SIGNAL_AFTER_KILL: {
        CollectParameters(2);
        CPed* const ped       = PedOf(P(0).iParam);
        ped->bSignalAfterKill = P(1).iParam != 0; // bit 12 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x4795C7
    case COMMAND_SET_CHAR_WANTED_BY_POLICE: {
        CollectParameters(2);
        CPed* const ped       = PedOf(P(0).iParam);
        ped->bWantedByPolice = P(1).iParam != 0; // bit 11 del dword +0x478
        return OR_CONTINUE;
    }
    // 0x47960D
    case COMMAND_SET_ZONE_NO_COPS: {
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        const int16 zone = CTheZones::FindZoneByLabel(label, static_cast<eZoneType>(2));
        if (zone < 0) {
            return OR_CONTINUE;
        }
        const uint16 infoIndex = static_cast<uint16>(CTheZones::NavigationZoneArray[zone].m_ZoneInfoIndex); // movzx word [zona+0x1C]
        CTheZones::ZoneInfoArray[infoIndex].IsNoCops = (P(0).u8Param & 1) != 0;                              // bit 7 del byte +0xF
        return OR_CONTINUE;
    }
    // 0x47966F
    case COMMAND_ADD_BLOOD: {
        CollectParameters(8);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector dir{ P(3).fParam, P(4).fParam, P(5).fParam };
        CPed* const   ped = PedOf(P(7).iParam);
        g_fx.AddBlood(pos, dir, P(6).iParam, ped->m_fContactSurfaceBrightness);
        return OR_CONTINUE;
    }
    // 0x4796EC
    case COMMAND_DISPLAY_CAR_NAMES: {
        CollectParameters(1);
        CHud::bScriptDontDisplayVehicleName = P(0).iParam == 0; // sete (0xBAA3F9)
        return OR_CONTINUE;
    }
    // 0x47970C
    case COMMAND_DISPLAY_ZONE_NAMES: {
        CollectParameters(1);
        CHud::bScriptDontDisplayAreaName = P(0).iParam == 0; // sete (0xBAA3F8)
        return OR_CONTINUE;
    }
    // 0x47972B
    case COMMAND_IS_CAR_DOOR_DAMAGED: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        UpdateCompareFlag(static_cast<uint32>(static_cast<CAutomobile*>(veh)->m_damageManager.GetDoorStatus(static_cast<eDoors>(P(1).iParam))) != 0);
        return OR_CONTINUE;
    }
    // 0x479774 (comparte la cola 0x47979C con SET_CHAR_COORDINATES_NO_OFFSET)
    case COMMAND_SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET: {
        CollectParameters(4);
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        SetCharCoordinates(*PedOf(P(0).iParam), pos, false, false);
        return OR_CONTINUE;
    }
    // 0x4797B6
    case COMMAND_SET_MINIGAME_IN_PROGRESS: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            m_IsTextBlockOverride                           = true; // byte [this+0xC8]
            CTheScripts::bMiniGameInProgress                 = true;
            CTheScripts::bDisplayNonMiniGameHelpMessages     = false;
            return OR_CONTINUE;
        }
        m_IsTextBlockOverride                           = false;
        CTheScripts::bMiniGameInProgress                 = false;
        CTheScripts::bDisplayNonMiniGameHelpMessages     = true;
        return OR_CONTINUE;
    }
    // 0x479800
    case COMMAND_IS_MINIGAME_IN_PROGRESS: {
        UpdateCompareFlag(CTheScripts::bMiniGameInProgress);
        return OR_CONTINUE;
    }
    // 0x479817
    case COMMAND_SET_FORCE_RANDOM_CAR_MODEL: {
        CollectParameters(1);
        StaticRef<int32>(0xA4448C) = P(0).iParam;
        return OR_CONTINUE;
    }
    // 0x479831
    case COMMAND_GET_RANDOM_CAR_OF_TYPE_IN_ANGLED_AREA_NO_SAVE: {
        CollectParameters(6);
        const float x1       = P(0).fParam;
        const float y1       = P(1).fParam;
        const float x2       = P(2).fParam;
        const float y2       = P(3).fParam;
        const float width    = P(4).fParam;
        const int32 modelArg = P(5).iParam;
        auto* const pool     = GetVehiclePool();
        int32       result   = -1;
        // el EXE recorre la reserva desde el ultimo hueco al primero y deja de buscar en cuanto encuentra uno
        for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
            --i;
            if (result != -1) {
                break;
            }
            CVehicle* const veh = pool->GetAt(i);
            if (!veh) {
                continue;
            }
            const auto appearance = veh->GetVehicleAppearance();
            if (appearance != VEHICLE_APPEARANCE_AUTOMOBILE && appearance != VEHICLE_APPEARANCE_BIKE) {
                continue;
            }
            const int32 model = static_cast<int16>(veh->m_nModelIndex);
            if (model != modelArg && modelArg >= 0) {
                continue;
            }
            if (!veh->CanBeDeleted()) {
                continue;
            }
            const CVector& pos = veh->GetPosition();
            if (!CTheScripts::IsPointInAngledArea(reinterpret_cast<const CVector2D*>(&pos), x1, y1, x2, y2, width)) {
                continue;
            }
            result = pool->GetRef(veh);
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x479931
    case COMMAND_ADD_NEXT_MESSAGE_TO_PREVIOUS_BRIEFS: {
        CollectParameters(1);
        StaticRef<uint8>(0xA4448B) = P(0).u8Param; // mov al, byte [0xA43C78]; mov byte [0xA4448B], al (CTheScripts::bAddNextMessageToPreviousBriefs, copia del byte)
        return OR_CONTINUE;
    }
    // 0x47994B
    case COMMAND_FAIL_KILL_FRENZY: {
        CDarkel::FailKillFrenzy();
        return OR_CONTINUE;
    }
    // 0x479957
    case COMMAND_IS_COP_VEHICLE_IN_AREA_3D_NO_SAVE: {
        CollectParameters(6);
        float x1 = P(0).fParam;
        float x2 = P(3).fParam;
        float y1 = P(1).fParam;
        float y2 = P(4).fParam;
        float z1 = P(2).fParam;
        float z2 = P(5).fParam;
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        auto* const pool  = GetVehiclePool();
        const int32 count = static_cast<int32>(pool->GetSize());
        bool        found = false;
        for (int32 i = 0; i < count; i++) {
            CVehicle* const veh = pool->GetAt(i);
            if (!veh || !veh->IsLawEnforcementVehicle() || veh->m_nModelIndex == 0x1AE) {
                continue;
            }
            const CVector& pos = veh->GetPosition();
            // fcomp + test ah,1 + jne (rechaza menor o NaN) y fcomp + test ah,41h + jp (rechaza mayor o NaN)
            if (pos.x >= x1 && pos.x <= x2 && pos.y >= y1 && pos.y <= y2 && pos.z >= z1 && pos.z <= z2) {
                found = true;
            }
        }
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }

    // 0x479AD8: comandos que el EXE deja vacios, sin leer parametros
    case COMMAND_DOES_CAR_HAVE_ROOF:
    case COMMAND_SET_BLIP_FADE:
    case COMMAND_LOAD_SHARED_CHAR_DECISION_MAKER:
    case COMMAND_IS_POLICE_VEHICLE_IN_PURSUIT:
    case COMMAND_GET_CAR_COLOUR_FROM_MENU_INDEX:
    case COMMAND_RADIANS_TO_DEGREES:
    case COMMAND_DEGREES_TO_RADIANS:
    case COMMAND_SET_CHAR_AIR_RESISTANCE_MULTIPLIER:
    case COMMAND_SET_PLAYER_FIRE_WITH_SHOULDER_BUTTON:
    case COMMAND_SET_CCTV_EFFECT:
    case COMMAND_GET_CPU_LEVEL:
        return OR_CONTINUE;

    // 0x479ADC: fuera de rango (ningun comando del rango cae aqui)
    default:
        return OR_INTERRUPT;
    }
}
