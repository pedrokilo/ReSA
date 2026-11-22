#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Timer.h"
#include "CarCtrl.h"
#include "Population.h"
#include "AutoPilot.h"
#include "tHandlingData.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/CivilianPed.h"
#include "Entity/Ped/EmergencyPed.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "game_sa/Crt/CrtState.h"

// Rama vivo-scra (sin validar): despachador original de los comandos de script 100..196 (ranura 1 de s_OriginalCommandHandlerTable, 0x466DE0). thiscall con el numero de comando en la pila (`ret 4`)
// y AL = 0 (OR_CONTINUE) o 0xFF (OR_INTERRUPT: comando sin caso, destino 0x46834F). Indice = comando - 100 en la tabla de saltos 0x468364 (sin tabla de bytes). El marco SEH del prologo (manejador
// 0x837D7C) solo libera el `new` de peaton de CREATE_CHAR si su constructor lanza; no hace falta en C++. Como en RunningScriptCommands300To499.cpp, solo llevan caso los comandos que el EXE resuelve
// aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o no_implementado con caso en la tabla de saltos); el resto va a ProcessHandledCommand.
// Cada caso sigue el desensamblado (misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas); las comparaciones x87 conservan su sentido con NaN.
namespace {
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
// 0x859014: -100.0f. Con z <= -100.0 (ordenado; NaN no) el EXE pide la altura del suelo (CWorld::FindGroundZForCoord 0x569660): `fcom(p) -100.0; test ah,41h; jp` salta (no pide el suelo) con z > -100 o NaN.
constexpr float EXE_MIN_GROUND_Z = -100.0f;
inline float ScriptGroundZ(float x, float y, float z) {
    if (z <= EXE_MIN_GROUND_Z) {
        return CWorld::FindGroundZForCoord(x, y);
    }
    return z;
}
// 0x859D5C: 1/65536 (1.52587890625e-05f).
constexpr float EXE_ONE_OVER_65536 = std::bit_cast<float>(0x37800000u);
// 0x858624: 1.0f (altura sobre el suelo de CREATE_CHAR).
constexpr float EXE_ONE = 1.0f;
// 0x858B34: 60.0f (factor de la velocidad maxima del manejo en SET_CAR_CRUISE_SPEED).
constexpr float EXE_SIXTY = 60.0f;
// 0x858B8C: 0.5f (altura media de IS_CAR_IN_AREA_3D).
constexpr float EXE_HALF = 0.5f;

// 0x463490 (CAutoPilot, inlinado en el EXE en CAR_GOTO_COORDINATES / CAR_WANDER_RANDOMLY / CAR_SET_IDLE y llamado en SET_CAR_MISSION): cambia la mision (+0x2A) salvo que sea
// MISSION_PLANE_CRASH_AND_BURN (0x39) o MISSION_HELI_CRASH_AND_BURN (0x3A). Del argumento solo cuenta el byte bajo.
inline void SetCarMissionUnlessCrashAndBurn(CAutoPilot& autoPilot, uint8 mission) {
    if (autoPilot.m_nCarMission == MISSION_PLANE_CRASH_AND_BURN || autoPilot.m_nCarMission == MISSION_HELI_CRASH_AND_BURN) {
        return;
    }
    autoPilot.m_nCarMission = static_cast<eCarMission>(mission);
}
// 0x41BDD0 (CVehicle::SetEngineOn(true)): con el motor roto (bEngineBroken) lo apaga; si no, lo enciende.
inline void SetEngineOnTrue(CVehicle* vehicle) {
    vehicle->vehicleFlags.bEngineOn = !vehicle->vehicleFlags.bEngineBroken;
}
// `cmp al,1; movzx eax,al; ja ..; mov eax,1`: velocidad de crucero al menos 1.
inline void EnsureCruiseSpeedAtLeastOne(CVehicle* vehicle) {
    const uint8 speed = vehicle->m_autoPilot.m_nCruiseSpeed;
    vehicle->m_autoPilot.m_nCruiseSpeed = speed > 1 ? speed : 1;
}
} // namespace

static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390 && offsetof(CAutoPilot, m_nCarMission) == 0x2A && offsetof(CAutoPilot, m_nCruiseSpeed) == 0x40
    && offsetof(CAutoPilot, m_nTimeToStartMission) == 0x1C);
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(tHandlingData, m_transmissionData) + offsetof(cTransmission, m_MaxFlatVelocity) == 0x88);
static_assert(MISSION_CRUISE == 1 && MISSION_GOTOCOORDINATES == 8 && MISSION_GOTOCOORDINATES_STRAIGHTLINE == 9 && STATUS_PHYSICS == 3);

// 0x466DE0 (rama vivo-scra, sin validar): ver el comentario de cabecera.
OpcodeResult CRunningScript::ProcessCommands100To199(int32 command) {
    switch (command) {
    case COMMAND_GENERATE_RANDOM_FLOAT: { // 152, 0x4674E4: tres tiradas descartadas y la cuarta * (1/65536) (rand del CRT del EXE, 0x821B1E); la variable se resuelve antes de las tiradas
        tScriptParam* const result = GetPointerToScriptVariable(VAR_GLOBAL); // 0x464790 (push 2)
        (void)ExeCrt::Rand();
        (void)ExeCrt::Rand();
        (void)ExeCrt::Rand();
        result->fParam = static_cast<float>(ExeCrt::Rand()) * EXE_ONE_OVER_65536; // fild + fmul + fstp
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_CHAR: { // 154, 0x46754F
        CollectParameters(5);
        GetCorrectPedModelIndexForEmergencyServiceType(static_cast<ePedType>(P(0).iParam), &P(1).uParam); // 0x464F50: con la direccion de ScriptParams[1]
        const ePedType pedType = static_cast<ePedType>(P(0).iParam);
        CPed* ped;
        if (pedType == PED_TYPE_COP) {
            void* const memory = CPed::operator new(0x7C4); // 0x5E4720
            ped = memory ? ::new (memory) CCopPed(P(1).uParam) : nullptr; // 0x5DDC60
        } else if (pedType == PED_TYPE_MEDIC || pedType == PED_TYPE_FIREMAN) {
            void* const memory = CPed::operator new(0x79C);
            ped = memory ? ::new (memory) CEmergencyPed(pedType, P(1).uParam) : nullptr; // 0x5DE340
        } else {
            void* const memory = CPed::operator new(0x79C);
            ped = memory ? ::new (memory) CCivilianPed(pedType, P(1).uParam) : nullptr; // 0x5DDB70
        }
        void* const  taskMemory = CTask::operator new(0x20); // 0x61A5A0
        CTask* const task       = taskMemory ? ::new (taskMemory) CTaskSimpleStandStill(999999, true, false, 8.0f) : nullptr; // 0x62F310
        ped->GetTaskManager().SetTask(task, TASK_PRIMARY_DEFAULT, false); // 0x681AF0
        ped->SetCharCreatedBy(PED_MISSION); // 0x5E47E0
        ped->bAllowMedicsToReviveMe = false; // bit 31 de las banderas (+0x46C)

        const float px = P(2).fParam;
        const float py = P(3).fParam;
        const float pz = ScriptGroundZ(px, py, P(4).fParam) + EXE_ONE; // fadd 1.0 (0x858624) tambien sobre la altura del suelo
        const CVector pos{ px, py, pz };
        ped->SetPosn(pos); // 0x4241C0
        ped->SetOrientation(0.0f, 0.0f, 0.0f); // 0x439A80
        CTheScripts::ClearSpaceForMissionEntity(pos, ped); // 0x486B00
        if (m_UsesMissionCleanup) {
            ped->m_nFlags |= 0x40000u; // bIsStaticWaitingForCollision
        }
        CWorld::Add(ped); // 0x563220
        CPopulation::ms_nTotalMissionPeds = CPopulation::ms_nTotalMissionPeds + 1;
        P(0).iParam = CPools::GetPedRef(ped); // 0x4442D0
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PED); // 0x4637E0
        }
        return OR_CONTINUE;
    }
    case COMMAND_DELETE_CHAR: { // 155, 0x467789: RemoveThisPed sin comprobar el puntero
        CollectParameters(1);
        CTheScripts::RemoveThisPed(PedOf(P(0).iParam)); // 0x486240
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PED); // 0x4654B0
        }
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_CAR: { // 165, 0x467AB3: sin corregir la altura con el suelo
        CollectParameters(4);
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVehicle* const vehicle = CCarCtrl::CreateCarForScript(P(0).iParam, pos, m_UsesMissionCleanup); // 0x431F80
        P(0).iParam = CPools::GetVehicleRef(vehicle); // 0x424160
        StoreParameters(1); // 0x468294
        return OR_CONTINUE;
    }
    case COMMAND_DELETE_CAR: { // 166, 0x467B1A
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle) {
            CWorld::Remove(vehicle); // 0x563280
            CWorld::RemoveReferencesToDeletedObject(vehicle); // 0x565510
            delete vehicle; // destructor virtual (ranura 0) con flag 1
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE); // 0x4654B0
        }
        return OR_CONTINUE;
    }
    case COMMAND_CAR_GOTO_COORDINATES: { // 167, 0x467B83
        CollectParameters(4);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const float px = P(1).fParam;
        const float py = P(2).fParam;
        const float z  = ScriptGroundZ(px, py, P(3).fParam);
        const CVector pos{ px, py, vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + z }; // 0x536BE0; `fadd [z]`
        CAutoPilot& autoPilot = vehicle->m_autoPilot;
        if (CCarCtrl::JoinCarWithRoadSystemGotoCoors(vehicle, pos, false, false)) { // 0x42F870
            SetCarMissionUnlessCrashAndBurn(autoPilot, MISSION_GOTOCOORDINATES_STRAIGHTLINE);
        } else {
            SetCarMissionUnlessCrashAndBurn(autoPilot, MISSION_GOTOCOORDINATES);
        }
        vehicle->SetStatus(STATUS_PHYSICS); // byte +0x36: (valor & 7) | 0x18
        SetEngineOnTrue(vehicle);
        EnsureCruiseSpeedAtLeastOne(vehicle);
        autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
        return OR_CONTINUE;
    }
    case COMMAND_CAR_WANDER_RANDOMLY: { // 168, 0x467C83
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        CCarCtrl::JoinCarWithRoadSystem(vehicle); // 0x42F5A0
        SetCarMissionUnlessCrashAndBurn(vehicle->m_autoPilot, MISSION_CRUISE);
        SetEngineOnTrue(vehicle);
        EnsureCruiseSpeedAtLeastOne(vehicle);
        vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
        return OR_CONTINUE;
    }
    case COMMAND_CAR_SET_IDLE: { // 169, 0x467CFF
        CollectParameters(1);
        SetCarMissionUnlessCrashAndBurn(VehicleOf(P(0).iParam)->m_autoPilot, MISSION_NONE);
        return OR_CONTINUE;
    }
    case COMMAND_GET_CAR_COORDINATES: { // 170, 0x467D48: posicion de la matriz (+0x14) o de la transformacion simple (+4)
        CollectParameters(1);
        const CVector pos = VehicleOf(P(0).iParam)->GetPosition();
        P(0).fParam = pos.x;
        P(1).fParam = pos.y;
        P(2).fParam = pos.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_COORDINATES: { // 171, 0x467DB5
        CollectParameters(4);
        CCarCtrl::SetCoordsOfScriptCar(VehicleOf(P(0).iParam), P(1).fParam, P(2).fParam, P(3).fParam, 0, 1); // 0x4342A0
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_CRUISE_SPEED: { // 173, 0x467E05: el parametro truncado (_ftol 0x821B40, byte bajo) y limitado a la velocidad maxima del manejo (+0x88) * 60
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const uint8 speed = static_cast<uint8>(static_cast<int64>(P(1).fParam));
        vehicle->m_autoPilot.m_nCruiseSpeed = speed;
        const float requested = static_cast<float>(speed);
        const float limit     = vehicle->m_pHandlingData->m_transmissionData.m_MaxFlatVelocity * EXE_SIXTY; // redondeado a float (fstp)
        // `fcom limit; test ah,5; jnp`: se queda con la pedida solo si es menor que el limite; con el limite igual, mayor o NaN usa el limite.
        const float value = requested < limit ? requested : limit;
        vehicle->m_autoPilot.m_nCruiseSpeed = static_cast<uint8>(static_cast<int64>(value)); // _ftol, byte bajo
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_MISSION: { // 175, 0x467EC1
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        SetCarMissionUnlessCrashAndBurn(vehicle->m_autoPilot, P(1).u8Param); // 0x463490
        vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
        SetEngineOnTrue(vehicle); // 0x41BDD0
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_IN_AREA_2D: { // 176, 0x467F1B
        CollectParameters(6);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        UpdateCompareFlag(vehicle->IsWithinArea(P(1).fParam, P(2).fParam, P(3).fParam, P(4).fParam)); // 0x54F200, 0x4859D0
        if (P(5).iParam) {
            CTheScripts::HighlightImportantArea(reinterpret_cast<int32>(this) + reinterpret_cast<int32>(m_IP), P(1).fParam, P(2).fParam, P(3).fParam, P(4).fParam, EXE_MIN_GROUND_Z); // 0x485E00
        }
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_IN_AREA_3D: { // 177, 0x467FBD
        CollectParameters(8);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        UpdateCompareFlag(vehicle->IsWithinArea(P(1).fParam, P(2).fParam, P(3).fParam, P(4).fParam, P(5).fParam, P(6).fParam)); // 0x54F2B0
        if (P(7).iParam) {
            const float z = (P(6).fParam + P(3).fParam) * EXE_HALF; // fld z2; fadd z1; fmul 0.5
            CTheScripts::HighlightImportantArea(reinterpret_cast<int32>(this) + reinterpret_cast<int32>(m_IP), P(1).fParam, P(2).fParam, P(4).fParam, P(5).fParam, z); // 0x485E00
        }
        return OR_CONTINUE;
    }
    default:
        return ProcessHandledCommand(command);
    }
}
