#include "StdInc.h"
#include "DatosExe/DataInicializadaB.h"
#include "DatosExe/RdataCadenas.h"
#include "DatosExe/RdataConstantes.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "PedGroups.h"
#include "SearchLight.h"
#include "FireManager.h"
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
#include "VehicleRecording.h"
#include "Camera.h"
#include "GameLogic.h"
#include "Game.h"
#include "CarCtrl.h"
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

// Lote 238A (sin validar): 0x496E00 (8462 bytes), CRunningScript::ProcessCommands1700To1799 (thiscall, un argumento de pila, `ret 4`; resultado en AL).
// El listado decia CTaskComplexSeekEntityRadiusAngleOffset: esta funcion es el despachador original de los comandos de script 1700..1799, ranura 17 de la tabla 0x8A6168
// (la usa ProcessOneCommand cuando el comando no tiene manejador propio). Cada `case` sigue el desensamblado; los parametros se leen de ScriptParams (0xA43C78) como el EXE
// (siempre despues de CollectParameters y con el mismo orden de lecturas/llamadas). No usa SEH propio: las entradas de la tabla de excepciones del EXE solo liberan la
// memoria de la tarea si su constructor lanza (no ocurre).
namespace {
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);

inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef) y 0x4048E0 (CPool<CVehicle>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
// 0xA44B88 - id * 0x1C: modelo negativo del script = entrada -id de CTheScripts::UsedObjectArray (su nModelIndex); si no, el propio valor.
inline int32 ScriptModelIndex(int32 id) {
    return id >= 0 ? id : CTheScripts::UsedObjectArray[-id].nModelIndex;
}
// strncpy(destino, origen, 16) del CRT del EXE (0x821F40): copia hasta 16 caracteres, para en el NUL y rellena con ceros hasta 16.
inline void CopyTextLabel16(char* dest, const char* src) {
    size_t i = 0;
    for (; i < 16 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < 16; i++) {
        dest[i] = '\0';
    }
}
// 0x8D237C: movimiento por defecto de CTaskComplexSeekEntity (vale 6); el EXE lo copia al task y lo vuelve a escribir con 6.
inline auto& SeekMoveStateGlobal = StaticRef<int32>(0x8D237C);
} // namespace

static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nTimeToStartMission) == 0x3AC);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0);
static_assert(offsetof(CVehicleModelInfo, m_anUpgrades) == 0x2D6);
static_assert(offsetof(CRunningScript, m_SceneSkipIP) == 0xD8 && offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(offsetof(CBaseModelInfo, m_nFlags) == 0x12);

// 0x496E00
OpcodeResult CRunningScript::ProcessCommands1700To1799(int32 commandId) {
    switch (commandId) {
    // 0x496E61
    case COMMAND_TASK_DIVE_FROM_ATTACHMENT_AND_GET_UP: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 timeOnGround = P(1).iParam;
        auto* const task = new CTaskComplexDiveFromAttachedEntityAndGetUp(timeOnGround);
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x496EBB
    case COMMAND_ATTACH_CHAR_TO_BIKE: {
        CollectParameters(9);
        const float verticalLimit = P(6).fParam * EXE_DEG_TO_RAD;   // [esp] (arg 4 de AttachPedToBike)
        const float horizontalLimit = P(7).fParam * EXE_DEG_TO_RAD; // [esp+4]
        // Orden del EXE: primero se leen los parametros, luego la moto (param 1) y despues el peaton (param 0).
        const CVector offset{ P(2).fParam, P(3).fParam, P(4).fParam };
        const uint16 arg2 = P(5).u16Param;
        const int32 weapon = P(8).iParam;
        CVehicle* const bike = VehicleOf(P(1).iParam);
        CPed* const ped = PedOf(P(0).iParam);
        ped->AttachPedToBike(bike, offset, arg2, verticalLimit, horizontalLimit, static_cast<eWeaponType>(weapon));
        return OR_CONTINUE;
    }
    // 0x496F5F
    case COMMAND_TASK_GOTO_CHAR_OFFSET: {
        CollectParameters(5);
        const float angle = P(4).fParam * EXE_DEG_TO_RAD; // [esp+24h]
        const int32 pedHandle = P(0).iParam;          // ebp
        const int32 time = P(2).iParam;               // edi
        const float radius = P(3).fParam;             // [esp+1Ch]
        CPed* const target = PedOf(P(1).iParam);      // ebx
        // Con tiempo negativo el EXE usa 50000 ms (0xC350); si no, el tiempo del script. Resto: 1000, 1.0f (0x85A40C), 2.0f (0x85A410), 2.0f (0x85A414), true, true.
        auto* const task = new CTaskComplexSeekEntityRadiusAngleOffset(
            target,
            time < 0 ? 50000 : time,
            1000,
            1.0f,
            2.0f,
            2.0f,
            true,
            true,
            CEntitySeekPosCalculatorRadiusAngleOffset{ radius, angle }
        );
        task->m_moveState = static_cast<eMoveState>(SeekMoveStateGlobal);
        SeekMoveStateGlobal = 6;
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x49707D
    case COMMAND_TASK_LOOK_AT_COORD: {
        CollectParameters(5);
        const int32 pedHandle = P(0).iParam; // eax
        CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        int32 duration = P(4).iParam; // esi
        if (duration == -1) {
            duration = 20000;
        } else if (duration == -2) {
            duration = 999999;
        }
        if (pedHandle != -1) {
            CPed* const ped = PedOf(pedHandle);
            g_ikChainMan.LookAt(DatosExe::Rdata::kS85A3F0, ped, nullptr, duration, BONE_UNKNOWN, &pos, false, 0.25f, 500, 6, true);
        } else {
            auto* const task = new CTaskSimpleTriggerLookAt(nullptr, duration, static_cast<int32>(-1), pos, true, 0.25f, 1000, 3);
            CTaskSequences::GetActiveSequence().AddTask(task);
        }
        return OR_CONTINUE;
    }
    // 0x4971B8
    case COMMAND_IS_RECORDING_GOING_ON_FOR_CAR: {
        CollectParameters(1);
        UpdateCompareFlag(false);
        return OR_CONTINUE;
    }
    // 0x4971D1
    case COMMAND_HIDE_CHAR_WEAPON_FOR_SCRIPTED_CUTSCENE: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        if (P(1).iParam != 0) {
            ped->ReplaceWeaponForScriptedCutscene();
            return OR_CONTINUE;
        }
        ped->RemoveWeaponForScriptedCutscene();
        if (ped->bInVehicle) {
            ped->RemoveWeaponWhenEnteringVehicle(0);
        }
        return OR_CONTINUE;
    }
    // 0x497228
    case COMMAND_GET_CHAR_SPEED: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        // En un vehiculo (con vehiculo no nulo) la velocidad es la del vehiculo; si no, la del peaton. Magnitud * 50.0f (0x858B40).
        const CVector* speed = &ped->m_vecMoveSpeed;
        if (ped->bInVehicle && ped->m_pVehicle) {
            speed = &ped->m_pVehicle->m_vecMoveSpeed;
        }
        P(0).fParam = speed->Magnitude() * 50.0f;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x497281
    case COMMAND_SET_GROUP_DECISION_MAKER: {
        CollectParameters(2);
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        if (groupIndex < 0 || groupIndex >= 8) {
            return OR_CONTINUE;
        }
        int32 decisionMaker = -1;
        if (P(1).iParam != -1) {
            decisionMaker = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_DECISION_MAKER);
        }
        CPedGroups::ms_groups[groupIndex].GetIntelligence().SetGroupDecisionMakerType(static_cast<eDecisionMakerType>(decisionMaker));
        return OR_CONTINUE;
    }
    // 0x4972E2
    case COMMAND_LOAD_GROUP_DECISION_MAKER: {
        CollectParameters(1);
        char name[128];
        CDecisionMakerTypesFileLoader::GetGrpDMName(P(0).iParam, name);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_DECISION_MAKER); // el EXE descarta el resultado
        const int32 decisionMaker = CDecisionMakerTypesFileLoader::LoadDecisionMaker(name, static_cast<eDecisionTypes>(1), m_UsesMissionCleanup);
        P(0).iParam = CTheScripts::GetNewUniqueScriptThingIndex(decisionMaker, SCRIPT_THING_DECISION_MAKER);
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
        }
        return OR_CONTINUE;
    }
    // 0x49736A
    case COMMAND_DISABLE_PLAYER_SPRINT: {
        CollectParameters(2);
        CPlayerInfo& info = CWorld::Players[P(0).iParam];
        info.m_pPed->GetPlayerData()->m_bPlayerSprintDisabled = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x4973B9
    case COMMAND_TASK_SIT_DOWN: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam; // ebx
        const int32 time = P(1).iParam;      // edi
        // Con tiempo -1 el EXE usa el entero de 0x86DB28 (sin llamar con el del script).
        const int32 duration = time == -1 ? DatosExe::Rdata::k86DB28 : time;
        auto* const task = new CTaskComplexSitDownThenIdleThenStandUp(duration, false, false);
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x497444
    case COMMAND_CREATE_SEARCHLIGHT: {
        CollectParameters(8);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_SEARCH_LIGHT); // resultado descartado
        const CVector start{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector target{ P(3).fParam, P(4).fParam, P(5).fParam };
        P(0).iParam = CTheScripts::AddScriptSearchLight(start, nullptr, target, P(6).fParam, P(7).fParam);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_SEARCHLIGHT);
            StoreParameters(1);
        } else {
            StoreParameters(1);
        }
        return OR_CONTINUE;
    }
    // 0x497505
    case COMMAND_DELETE_SEARCHLIGHT: {
        CollectParameters(1);
        if (CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEARCH_LIGHT) < 0) {
            return OR_CONTINUE;
        }
        CTheScripts::RemoveScriptSearchLight(P(0).iParam);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_SEARCHLIGHT);
        }
        return OR_CONTINUE;
    }
    // 0x49755D
    case COMMAND_DOES_SEARCHLIGHT_EXIST: {
        CollectParameters(1);
        bool exists = false;
        if (CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEARCH_LIGHT) >= 0) {
            exists = true;
        }
        UpdateCompareFlag(exists);
        return OR_CONTINUE;
    }
    // 0x497598
    case COMMAND_MOVE_SEARCHLIGHT_BETWEEN_COORDS: {
        CollectParameters(8);
        reinterpret_cast<void(__cdecl*)(int32, uint32, uint32, uint32, uint32, uint32, uint32, uint32)>(0x493360)(P(0).iParam, P(1).uParam, P(2).uParam, P(3).uParam, P(4).uParam, P(5).uParam, P(6).uParam, P(7).uParam); // CTheScripts::MoveSearchLightBetweenTwoPoints (lote 223A)
        return OR_CONTINUE;
    }
    // 0x4975E5
    case COMMAND_POINT_SEARCHLIGHT_AT_COORD: {
        CollectParameters(5);
        reinterpret_cast<void(__cdecl*)(int32, uint32, uint32, uint32, uint32)>(0x493480)(P(0).iParam, P(1).uParam, P(2).uParam, P(3).uParam, P(4).uParam); // CTheScripts::MoveSearchLightToPointAndStop (lote 223A)
        return OR_CONTINUE;
    }
    // 0x49761E
    case COMMAND_POINT_SEARCHLIGHT_AT_CHAR: {
        CollectParameters(3);
        const float pathSpeed = P(2).fParam;
        CPed* const ped = PedOf(P(1).iParam);
        reinterpret_cast<void(__cdecl*)(int32, CEntity*, uint32)>(0x493420)(P(0).iParam, ped, std::bit_cast<uint32>(pathSpeed)); // CTheScripts::MoveSearchLightToEntity (lote 223A)
        return OR_CONTINUE;
    }
    // 0x497656
    case COMMAND_IS_CHAR_IN_SEARCHLIGHT: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(1).iParam);
        const bool spotted = CSearchLight::IsSpottedEntity(P(0).iParam, *ped);
        UpdateCompareFlag(spotted);
        return OR_CONTINUE;
    }
    // 0x4976A2
    case COMMAND_HAS_CUTSCENE_LOADED: {
        UpdateCompareFlag(CCutsceneMgr::ms_cutsceneLoadStatus == CCutsceneMgr::LoadStatus::LOADED);
        return OR_CONTINUE;
    }
    // 0x4976C8
    case COMMAND_TASK_TURN_CHAR_TO_FACE_COORD: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        // 0x86FC7C y 0x86FC80: los dos floats por defecto del EXE (velocidad de giro y diferencia maxima).
        auto* const task = new CTaskComplexTurnToFaceEntityOrCoord(pos, DatosExe::Rdata::k86FC7C, DatosExe::Rdata::k86FC80);
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x49776F
    case COMMAND_TASK_DRIVE_POINT_ROUTE: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam; // ebx
        CVehicle* vehicle = nullptr;          // edi
        if (P(1).iParam >= 0) {
            vehicle = VehicleOf(P(1).iParam);
        }
        const float cruiseSpeed = P(2).fParam;
        auto* const task = new CTaskComplexDrivePointRoute(vehicle, StaticRef<CPointRoute>(0xC18D50), cruiseSpeed, 2, static_cast<eModelID>(-1), -1.0f, static_cast<eCarDrivingStyle>(0));
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x4977E3
    case COMMAND_FIRE_SINGLE_BULLET: {
        CollectParameters(7);
        const CVector from{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector to{ P(3).fParam, P(4).fParam, P(5).fParam };
        FireOneInstantHitRound(from, to, P(6).iParam);
        return OR_CONTINUE;
    }
    // 0x497852
    case COMMAND_IS_LINE_OF_SIGHT_CLEAR: {
        CollectParameters(11);
        const CVector from{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector to{ P(3).fParam, P(4).fParam, P(5).fParam };
        const bool clear = CWorld::GetIsLineOfSightClear(
            from,
            to,
            P(6).iParam != 0,
            P(7).iParam != 0,
            P(8).iParam != 0,
            P(9).iParam != 0,
            P(10).iParam != 0,
            false,
            false
        );
        UpdateCompareFlag(clear);
        return OR_CONTINUE;
    }
    // 0x497908
    case COMMAND_GET_CAR_ROLL: {
        CollectParameters(1);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        P(0).fParam = automobile->GetCarRoll();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x497940
    case COMMAND_POINT_SEARCHLIGHT_AT_VEHICLE: {
        CollectParameters(3);
        const float pathSpeed = P(2).fParam;
        CVehicle* const vehicle = VehicleOf(P(1).iParam);
        reinterpret_cast<void(__cdecl*)(int32, CEntity*, uint32)>(0x493420)(P(0).iParam, vehicle, std::bit_cast<uint32>(pathSpeed)); // CTheScripts::MoveSearchLightToEntity (lote 223A)
        return OR_CONTINUE;
    }
    // 0x497978
    case COMMAND_IS_VEHICLE_IN_SEARCHLIGHT: {
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(1).iParam);
        const bool spotted = CSearchLight::IsSpottedEntity(P(0).iParam, *vehicle);
        UpdateCompareFlag(spotted);
        return OR_CONTINUE;
    }
    // 0x4979B4
    case COMMAND_CREATE_SEARCHLIGHT_ON_VEHICLE: {
        CollectParameters(9);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_SEARCH_LIGHT); // resultado descartado
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const CVector start{ P(1).fParam, P(2).fParam, P(3).fParam };
        const CVector target{ P(4).fParam, P(5).fParam, P(6).fParam };
        P(0).iParam = CTheScripts::AddScriptSearchLight(start, vehicle, target, P(7).fParam, P(8).fParam);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_SEARCHLIGHT);
            StoreParameters(1);
        } else {
            StoreParameters(1);
        }
        return OR_CONTINUE;
    }
    // 0x497A65
    case COMMAND_TASK_GO_TO_COORD_WHILE_AIMING: {
        CollectParameters(11);
        const int32 pedHandle = P(0).iParam; // ebx
        const float arg6 = P(6).fParam;
        const float arg5 = P(5).fParam;
        CPed* aimAt = nullptr; // edi
        if (P(7).iParam >= 0) {
            aimAt = PedOf(P(7).iParam);
        }
        const CVector movePos{ P(1).fParam, P(2).fParam, P(3).fParam };
        const CVector aimPos{ P(8).fParam, P(9).fParam, P(10).fParam };
        auto* const task = new CTaskComplexGoToPointAiming(static_cast<eMoveState>(P(4).iParam), movePos, aimAt, aimPos, arg5, arg6);
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x497B75
    case COMMAND_GET_NUMBER_OF_FIRES_IN_RANGE: {
        CollectParameters(4);
        const CVector point{ P(0).fParam, P(1).fParam, P(2).fParam };
        P(0).iParam = gFireManager.GetNumFiresInRange(point, P(3).fParam);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x497BC7
    case COMMAND_ADD_BLIP_FOR_SEARCHLIGHT: {
        CollectParameters(1);
        CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEARCH_LIGHT); // resultado descartado
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CRadar::GetActualBlipArrayIndex(next); // resultado descartado
        // El EXE llama con cinco argumentos (el ultimo, el nombre del script en this+8); SetEntityBlip solo usa los cuatro primeros.
        const auto blip = CRadar::SetEntityBlip(BLIP_SPOTLIGHT, P(0).iParam, 0, static_cast<eBlipDisplay>(3));
        CRadar::ChangeBlipScale(blip, 3);
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x497C26
    case COMMAND_SKIP_TO_END_AND_STOP_PLAYBACK_RECORDED_CAR: {
        CollectParameters(1);
        CVehicleRecording::SkipToEndAndStopPlaybackRecordedCar(VehicleOf(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x497C50
    case COMMAND_TASK_CAR_TEMP_ACTION: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        CVehicle* vehicle = nullptr; // edi
        if (P(1).iParam != -1) {
            vehicle = VehicleOf(P(1).iParam);
        }
        const int32 action = P(2).iParam; // ebx
        const int32 time = P(3).iParam;   // ebp
        auto* const task = new CTaskSimpleCarSetTempAction(vehicle, static_cast<eAutoPilotTempAction>(action), static_cast<uint32>(time));
        // El EXE guarda el dword completo de la accion en +0x60; eAutoPilotTempAction es de 8 bits y el constructor de la DLL lo truncaria (lo halló el banco Fn238A)
        *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(task) + 0x60) = action; // m_TempAct (privado)
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x497CDB
    case COMMAND_SET_LA_RIOTS: {
        CollectParameters(1);
        gbLARiots = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x497CFB
    case COMMAND_REMOVE_CHAR_FROM_GROUP: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CPedGroup* const group = CPedGroups::GetPedsGroup(ped);
        if (!group) {
            return OR_CONTINUE;
        }
        CPedGroupMembership& membership = group->GetMembership(); // edi + 8
        if (membership.IsLeader(ped)) {
            return OR_CONTINUE;
        }
        membership.RemoveMember(ped);
        group->Process();
        return OR_CONTINUE;
    }
    // 0x497D54
    case COMMAND_ATTACH_SEARCHLIGHT_TO_SEARCHLIGHT_OBJECT: {
        CollectParameters(7);
        // Orden del EXE: los objetos se resuelven del parametro 3 al 1 (CPool<CObject>::GetAtRef, 0x465040).
        CObject* const bulb = CPools::GetObject(P(3).iParam);
        CObject* const housing = CPools::GetObject(P(2).iParam);
        CObject* const tower = CPools::GetObject(P(1).iParam);
        CTheScripts::AttachSearchlightToSearchlightObject(P(0).iParam, tower, housing, bulb, CVector{ P(4).fParam, P(5).fParam, P(6).fParam });
        return OR_CONTINUE;
    }
    // 0x497DB8
    case COMMAND_DISPLAY_TIMER_BARS: {
        CollectParameters(1);
        return OR_CONTINUE;
    }
    // 0x497DC8
    case COMMAND_SWITCH_EMERGENCY_SERVICES: {
        CollectParameters(1);
        CCarCtrl::bAllowEmergencyServicesToBeCreated = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x497DE8
    case COMMAND_SET_VAR_TEXT_LABEL16: {
        tScriptParam* const dest = GetPointerToScriptVariable(VAR_GLOBAL);
        char label[16];
        ReadTextLabelFromScript(label, 16);
        CopyTextLabel16(reinterpret_cast<char*>(dest), label);
        return OR_CONTINUE;
    }
    // 0x497E1E
    case COMMAND_SET_LVAR_TEXT_LABEL16: {
        tScriptParam* const dest = GetPointerToScriptVariable(VAR_LOCAL);
        char label[16];
        ReadTextLabelFromScript(label, 16);
        CopyTextLabel16(reinterpret_cast<char*>(dest), label);
        return OR_CONTINUE;
    }
    // 0x497E54
    case COMMAND_CREATE_CHECKPOINT: {
        CollectParameters(8);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_CHECKPOINT); // resultado descartado
        const CVector at{ P(1).fParam, P(2).fParam, P(3).fParam };
        const CVector pointTo{ P(4).fParam, P(5).fParam, P(6).fParam };
        P(0).iParam = CTheScripts::AddScriptCheckpoint(at, pointTo, P(7).fParam, static_cast<eCheckpointType>(P(0).iParam));
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_CHECKPOINT);
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x497EF9
    case COMMAND_DELETE_CHECKPOINT: {
        CollectParameters(1);
        if (CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_CHECKPOINT) < 0) {
            return OR_CONTINUE;
        }
        CTheScripts::RemoveScriptCheckpoint(P(0).iParam);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_CHECKPOINT);
        }
        return OR_CONTINUE;
    }
    // 0x497F51
    case COMMAND_SWITCH_RANDOM_TRAINS: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            CTrain::DisableRandomTrains(false);
        } else {
            CTrain::DisableRandomTrains(true);
        }
        return OR_CONTINUE;
    }
    // 0x497F85
    case COMMAND_CREATE_MISSION_TRAIN: {
        CollectParameters(5);
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        CTrain* train = nullptr; // el EXE no lo inicializa: CreateMissionTrain lo escribe siempre
        CTrain::CreateMissionTrain(pos, P(4).iParam != 0, P(0).iParam, &train, nullptr, -1, -1, true);
        P(0).iParam = CPools::GetVehicleRef(train);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x498001
    case COMMAND_DELETE_MISSION_TRAINS: {
        CTrain::RemoveMissionTrains();
        return OR_CONTINUE;
    }
    // 0x49800D
    case COMMAND_MARK_MISSION_TRAINS_AS_NO_LONGER_NEEDED: {
        CTrain::ReleaseMissionTrains();
        return OR_CONTINUE;
    }
    // 0x498019
    case COMMAND_DELETE_ALL_TRAINS: {
        CTrain::RemoveAllTrains();
        return OR_CONTINUE;
    }
    // 0x498025
    case COMMAND_SET_TRAIN_SPEED: {
        CollectParameters(2);
        CTrain::SetTrainSpeed(static_cast<CTrain*>(VehicleOf(P(0).iParam)), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x49805A
    case COMMAND_SET_TRAIN_CRUISE_SPEED: {
        CollectParameters(2);
        CTrain::SetTrainCruiseSpeed(static_cast<CTrain*>(VehicleOf(P(0).iParam)), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x498090
    case COMMAND_GET_TRAIN_CABOOSE: {
        CollectParameters(1);
        CTrain* const caboose = CTrain::FindCaboose(static_cast<CTrain*>(VehicleOf(P(0).iParam)));
        P(0).iParam = CPools::GetVehicleRef(caboose);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4980C9
    case COMMAND_DELETE_PLAYER: {
        CollectParameters(1);
        CPlayerPed::RemovePlayerPed(P(0).iParam);
        return OR_CONTINUE;
    }
    // 0x4980E7
    case COMMAND_SET_TWO_PLAYER_CAMERA_MODE: {
        CollectParameters(1);
        TheCamera.StartCooperativeCamMode();
        return OR_CONTINUE;
    }
    // 0x498101
    case COMMAND_TASK_CAR_MISSION: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        CVehicle* vehicle = nullptr;
        if (P(1).iParam >= 0) {
            vehicle = VehicleOf(P(1).iParam);
        }
        CVehicle* target = nullptr; // ebp
        if (P(2).iParam >= 0) {
            target = VehicleOf(P(2).iParam);
        }
        const float speed = P(4).fParam;
        auto* const task = new CTaskComplexCarDriveMission(vehicle, target, static_cast<eCarMission>(P(3).iParam), static_cast<eCarDrivingStyle>(P(5).iParam), speed);
        // El EXE entrega el dword completo del parametro al constructor, que lo guarda en +0x28; eCarMission es de 8 bits y el constructor de la DLL lo truncaria (lo halló el banco Fn238A)
        *reinterpret_cast<int32*>(&task->m_nCarMission) = P(3).iParam;
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x4981BE
    case COMMAND_TASK_GO_TO_OBJECT: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam; // ebp
        CObject* const object = CPools::GetObject(P(1).iParam);
        const int32 time = P(2).iParam; // ebx
        const float radius = P(3).fParam;
        // 0x859E30 y 0x859E34: 2.0f y 2.0f; el resto del EXE: 1000 ms entre escaneos, cansancio y mirar al terminar.
        auto* const task = new CTaskComplexSeekEntityStandard(object, time, 1000, radius, 2.0f, 2.0f, true, true);
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x498259
    case COMMAND_TASK_WEAPON_ROLL: {
        CollectParameters(2);
        const bool toTheRight = P(1).iParam != 0;
        const int32 pedHandle = P(0).iParam; // edi
        CTask* task;
        if (toTheRight) {
            task = new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0), static_cast<AnimationId>(0x3B), 4.0f, false);
        } else {
            task = new CTaskSimpleRunAnim(static_cast<AssocGroupId>(0), static_cast<AnimationId>(0x39), 4.0f, false);
        }
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x4982EF
    case COMMAND_TASK_CHAR_ARREST_CHAR: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam; // ebx
        CPed* const target = PedOf(P(1).iParam); // edi
        auto* const task = new CTaskComplexArrestPed(target);
        GivePedScriptedTask(pedHandle, task, commandId);
        return OR_CONTINUE;
    }
    // 0x498340
    case COMMAND_GET_AVAILABLE_VEHICLE_MOD: {
        CollectParameters(2);
        const CVehicle* const vehicle = VehicleOf(P(0).iParam);
        auto* const modelInfo = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(vehicle->m_nModelIndex)]);
        P(0).iParam = modelInfo->m_anUpgrades[P(1).iParam]; // int16 con signo
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x498388
    case COMMAND_GET_VEHICLE_MOD_TYPE: {
        CollectParameters(1);
        const int32 modelId = ScriptModelIndex(P(0).iParam);
        const uint16 flags = CModelInfo::ms_modelInfoPtrs[modelId]->m_nFlags; // +0x12
        const uint32 kind = (flags >> 10) & 0x1F;
        if (flags & 0x100) {
            // Tabla de bytes 0x4990BC indexada por kind-1 (sin signo; > 0x15 no cambia nada) y tabla de saltos 0x4990A0
            switch (kind) {
            case 1:  P(0).iParam = 0x0B; break;
            case 2:  P(0).iParam = 0x0C; break;
            case 12: P(0).iParam = 0x0E; break;
            case 13: P(0).iParam = 0x0F; break;
            case 19: P(0).iParam = 0x0D; break;
            case 20:
            case 21:
            case 22: P(0).iParam = 0x10; break;
            default: break;
            }
        } else {
            // Tabla de saltos 0x4990D4 (kind 0..17; el resto no cambia nada)
            switch (kind) {
            case 0:  P(0).iParam = 0; break;
            case 1:
            case 2:  P(0).iParam = 1; break;
            case 6:  P(0).iParam = 2; break;
            case 8:
            case 9:  P(0).iParam = 3; break;
            case 10: P(0).iParam = 4; break;
            case 11: P(0).iParam = 5; break;
            case 12: P(0).iParam = 6; break;
            case 14: P(0).iParam = 7; break;
            case 15: P(0).iParam = 8; break;
            case 16: P(0).iParam = 9; break;
            case 17: P(0).iParam = 10; break;
            default: break;
            }
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4985A6
    case COMMAND_ADD_VEHICLE_MOD: {
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const int32 modelId = ScriptModelIndex(P(1).iParam);
        P(0).iParam = vehicle->AddVehicleUpgrade(modelId);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4985E4
    case COMMAND_REMOVE_VEHICLE_MOD: {
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const int32 modelId = ScriptModelIndex(P(1).iParam);
        vehicle->RemoveVehicleUpgrade(modelId);
        return OR_CONTINUE;
    }
    // 0x498624
    case COMMAND_REQUEST_VEHICLE_MOD: {
        CollectParameters(1);
        CStreaming::RequestVehicleUpgrade(ScriptModelIndex(P(0).iParam), 0xC);
        return OR_CONTINUE;
    }
    // 0x498654
    case COMMAND_HAS_VEHICLE_MOD_LOADED: {
        CollectParameters(1);
        UpdateCompareFlag(CStreaming::HasVehicleUpgradeLoaded(ScriptModelIndex(P(0).iParam)));
        return OR_CONTINUE;
    }
    // 0x49868D
    case COMMAND_MARK_VEHICLE_MOD_AS_NO_LONGER_NEEDED: {
        CollectParameters(1);
        const int32 modelId = ScriptModelIndex(P(0).iParam);
        CStreaming::SetMissionDoesntRequireModel(modelId);
        const int32 other = CVehicleModelInfo::ms_linkedUpgrades.FindOtherUpgrade(static_cast<int16>(modelId)); // movsx eax, ax
        if (other == -1) {
            return OR_CONTINUE;
        }
        CStreaming::SetMissionDoesntRequireModel(other);
        return OR_CONTINUE;
    }
    // 0x4986DC
    case COMMAND_GET_NUM_AVAILABLE_PAINTJOBS: {
        CollectParameters(1);
        const CVehicle* const vehicle = VehicleOf(P(0).iParam);
        auto* const modelInfo = static_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(vehicle->m_nModelIndex)]);
        P(0).iParam = modelInfo->GetNumRemaps();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x49870C
    case COMMAND_GIVE_VEHICLE_PAINTJOB: {
        CollectParameters(2);
        const int32 remap = P(1).iParam;
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        vehicle->SetRemap(remap);
        return OR_CONTINUE;
    }
    // 0x49873B
    case COMMAND_IS_GROUP_MEMBER: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam); // edi
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_PED_GROUP);
        bool isMember = false;
        if (groupIndex >= 0 && groupIndex < 8 && CPedGroups::ms_activeGroups[groupIndex]) {
            isMember = CPedGroups::ms_groups[groupIndex].GetMembership().IsMember(ped);
        }
        UpdateCompareFlag(isMember);
        return OR_CONTINUE;
    }
    // 0x498796
    case COMMAND_IS_GROUP_LEADER: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam); // edi
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_PED_GROUP);
        bool isLeader = false;
        if (groupIndex >= 0 && groupIndex < 8) {
            isLeader = CPedGroups::ms_groups[groupIndex].GetMembership().IsLeader(ped);
        }
        UpdateCompareFlag(isLeader);
        return OR_CONTINUE;
    }
    // 0x498801
    case COMMAND_SET_GROUP_SEPARATION_RANGE: {
        CollectParameters(2);
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        if (groupIndex >= 0 && groupIndex < 8) {
            CPedGroups::ms_groups[groupIndex].GetMembership().SetSeparationRange(P(1).fParam);
        }
        return OR_CONTINUE;
    }
    // 0x49883D
    case COMMAND_LIMIT_TWO_PLAYER_DISTANCE: {
        CollectParameters(1);
        CGameLogic::bLimitPlayerDistance = true;
        CGameLogic::MaxPlayerDistance = P(0).fParam;
        return OR_CONTINUE;
    }
    // 0x498860
    case COMMAND_RELEASE_TWO_PLAYER_DISTANCE: {
        CGameLogic::bLimitPlayerDistance = false;
        return OR_CONTINUE;
    }
    // 0x49886E
    case COMMAND_SET_PLAYER_PLAYER_TARGETTING: {
        CollectParameters(1);
        CGameLogic::bPlayersCannotTargetEachOther = P(0).iParam == 0;
        return OR_CONTINUE;
    }
    // 0x49888D
    case COMMAND_GET_SCRIPT_FIRE_COORDS: {
        CollectParameters(1);
        const int32 fireIndex = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_FIRE);
        if (fireIndex >= 0 && fireIndex < 0x3C) {
            const CVector coords = gFireManager.GetScriptFireCoords(static_cast<int16>(fireIndex)); // [esp+40h..48h]
            P(1).fParam = coords.y;
            P(2).fParam = coords.z;
            P(0).fParam = coords.x;
        } else {
            P(1).iParam = 0;
            P(2).iParam = 0;
            P(0).iParam = 0;
        }
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x498900
    case COMMAND_GET_NTH_CLOSEST_CAR_NODE_WITH_HEADING: {
        CollectParameters(4);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float z = P(2).fParam;
        if (z <= DatosExe::Rdata::k859014) { // -100.0f
            z = CWorld::FindGroundZForCoord(x, y);
        }
        const int32 nth = P(3).iParam - 1;
        // 0x497423FE = 999999.875f
        const CNodeAddress node = ThePaths.FindNthNodeClosestToCoors(CVector{ x, y, z }, 0, 999999.875f, false, true, nth, false, false, nullptr);
        bool found;
        const CVector nodePos = ThePaths.FindNodeCoorsForScript(node, &found);
        bool result;
        if (found) {
            P(2).fParam = nodePos.z;
            P(0).fParam = nodePos.x;
            P(1).fParam = nodePos.y;
            P(3).fParam = ThePaths.FindNodeOrientationForCarPlacement(node);
            result = true;
        } else {
            P(0).iParam = 0;
            P(1).iParam = 0;
            P(2).iParam = 0;
            P(3).iParam = 0;
            result = false;
        }
        StoreParameters(4);
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x498A2C
    case COMMAND_SET_PLAYERS_CAN_BE_IN_SEPARATE_CARS: {
        CollectParameters(1);
        CGameLogic::bPlayersCanBeInSeparateCars = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x498A4B
    case COMMAND_DOES_CAR_HAVE_STUCK_CAR_CHECK: {
        CollectParameters(1);
        UpdateCompareFlag(CTheScripts::StuckCars.IsCarInStuckCarArray(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x498A86
    case COMMAND_SET_PLAYBACK_SPEED: {
        CollectParameters(2);
        const float speed = P(1).fParam;
        CVehicleRecording::SetPlaybackSpeed(VehicleOf(P(0).iParam), speed);
        return OR_CONTINUE;
    }
    // 0x498AB7
    case COMMAND_ARE_ANY_CHARS_NEAR_CHAR: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        const CVector pos = ped->GetPosition(); // matriz + 0x30 si hay matriz; si no, la posicion de +4
        const float radius = P(1).fParam;
        const float radiusSq = radius * radius; // fmul y fstp a float
        auto& pool = *GetPedPool();
        bool found = false;
        for (auto i = static_cast<int32>(pool.GetSize()); i != 0;) {
            i--;
            if (found) {
                break;
            }
            CPed* const other = pool.GetAt(i);
            if (!other || other == ped) {
                continue;
            }
            const CVector otherPos = other->GetPosition();
            const CVector delta{ otherPos.x - pos.x, otherPos.y - pos.y, otherPos.z - pos.z };
            if (delta.SquaredMagnitudeExe() < radiusSq) {
                found = true;
            }
        }
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }
    // 0x498BCF
    case COMMAND_SKIP_CUTSCENE_END: {
        m_SceneSkipIP = 0;
        return OR_CONTINUE;
    }
    // 0x498BE0
    case COMMAND_GET_PERCENTAGE_TAGGED_IN_AREA: {
        CollectParameters(4);
        const float x1 = P(0).fParam;
        const float y1 = P(1).fParam;
        const float x2 = P(2).fParam;
        const float y2 = P(3).fParam;
        // Orden de x87 del EXE: fcom y salto con `test ah, 41h`: se intercambian solo si el primero es MAYOR (los NaN no intercambian).
        float minX = x1, maxX = x2;
        if (x1 > x2) {
            minX = x2;
            maxX = x1;
        }
        float minY = y1, maxY = y2;
        if (y1 > y2) {
            minY = y2;
            maxY = y1;
        }
        CRect area;
        area.left = minX;
        area.top = maxY;
        area.right = maxX;
        area.bottom = minY;
        P(0).iParam = CTagManager::GetPercentageTaggedInArea(area);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x498CA4
    case COMMAND_SET_TAG_STATUS_IN_AREA: {
        CollectParameters(5);
        const float x1 = P(0).fParam;
        const float y1 = P(1).fParam;
        const float x2 = P(2).fParam;
        const float y2 = P(3).fParam;
        float minX = x1, maxX = x2;
        if (x1 > x2) {
            minX = x2;
            maxX = x1;
        }
        float minY = y1, maxY = y2;
        if (y1 > y2) {
            minY = y2;
            maxY = y1;
        }
        CRect area;
        area.left = minX;
        area.top = maxY;
        area.right = maxX;
        area.bottom = minY;
        if (P(4).iParam != 0) {
            CTagManager::SetAlphaInArea(area, 0xFF);
        } else {
            CTagManager::SetAlphaInArea(area, 0);
        }
        return OR_CONTINUE;
    }
    // 0x498D79
    case COMMAND_CAR_GOTO_COORDINATES_RACING: {
        CollectParameters(4);
        CVehicle* const vehicle = VehicleOf(P(0).iParam); // esi
        float z = P(3).fParam;
        const float x = P(1).fParam;
        const float y = P(2).fParam;
        if (z <= DatosExe::Rdata::k859014) { // -100.0f
            z = CWorld::FindGroundZForCoord(x, y);
        }
        const CVector dest{ x, y, vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + z };
        const bool joined = CCarCtrl::JoinCarWithRoadSystemGotoCoors(vehicle, dest, false, false);
        auto& mission = vehicle->m_autoPilot.m_nCarMission;
        // 0x39 y 0x3A: los dos valores que el EXE conserva (carreras); si no, 9 con exito y 0x21 (33) sin el.
        if (mission != static_cast<eCarMission>(0x39) && mission != static_cast<eCarMission>(0x3A)) {
            mission = joined ? static_cast<eCarMission>(9) : static_cast<eCarMission>(0x21);
        }
        vehicle->SetStatus(STATUS_PHYSICS);
        reinterpret_cast<void(__thiscall*)(CVehicle*, uint8)>(0x41BDD0)(vehicle, 1); // CVehicle::SetEngineOn(true)
        vehicle->m_autoPilot.m_nCruiseSpeed = std::max<uint8>(vehicle->m_autoPilot.m_nCruiseSpeed, 1);
        vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
        return OR_CONTINUE;
    }
    // 0x498E78
    case COMMAND_START_PLAYBACK_RECORDED_CAR_USING_AI: {
        CollectParameters(2);
        CVehicleRecording::StartPlaybackRecordedCar(VehicleOf(P(0).iParam), P(1).iParam, true, false);
        return OR_CONTINUE;
    }
    // 0x498EAA
    case COMMAND_SKIP_IN_PLAYBACK_RECORDED_CAR: {
        CollectParameters(2);
        const float distance = P(1).fParam;
        CVehicleRecording::SkipForwardInRecording(VehicleOf(P(0).iParam), distance);
        return OR_CONTINUE;
    }
    // 0x498ED8
    case COMMAND_SKIP_CUTSCENE_START_INTERNAL: {
        CollectParameters(1);
        m_SceneSkipIP = P(0).iParam;
        return OR_CONTINUE;
    }
    // Sin cuerpo (0x498EED): no consumen parametros y devuelven OR_CONTINUE
    case COMMAND_TASK_KILL_THREATS_ON_FOOT_WHILE_DUCKING:     // 1700
    case COMMAND_TASK_PLAY_ANIM_WITH_VELOCITY_EXTRACTION:     // 1702
    case COMMAND_TASK_OPEN_DRIVER_DOOR:                       // 1734
    case COMMAND_SET_VEHICLE_RECORDS_COLLISIONS:              // 1739
    case COMMAND_GET_CAR_LAST_ROUTE_COORDS:                   // 1742
    case COMMAND_CREATE_SCRIPT_GANG_ROADBLOCK:                // 1780
    case COMMAND_CLEAR_TWO_PLAYER_CAMERA_MODE:                // 1782
    case COMMAND_SET_PLAYER_PASSENGER_CAN_SHOOT:              // 1783
    case COMMAND_GET_HEIGHT_OF_CAR_WHEELS_FROM_GROUND:        // 1785
    case COMMAND_SWITCH_PLAYER_CROSSHAIR:                     // 1787
    case COMMAND_GET_CAR_VALUE:                               // 1790
    case COMMAND_SKIP_CUTSCENE_START:                         // 1792
        return OR_CONTINUE;
    // Sin cuerpo (0x498EF1): devuelven OR_INTERRUPT (-1)
    case COMMAND_SET_GROUP_DEFAULT_TASK:                      // 1720
    case COMMAND_DRAW_CROSS_IN_FRONT_OF_DEBUG_CAMERA:         // 1740
    case COMMAND_DRAW_DEBUG_CUBE:                             // 1741
    case COMMAND_VAR_TEXT_LABEL16:                            // 1747
    case COMMAND_LVAR_TEXT_LABEL16:                           // 1748
        return OR_INTERRUPT;
    default:
        break; // fuera de 1700..1799 (`ja` sin signo en 0x496E54): OR_INTERRUPT
    }
    return OR_INTERRUPT;
}
