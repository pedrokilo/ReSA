#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Ped/CivilianPed.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/EmergencyPed.h"
#include "Radar.h"
#include "Camera.h"
#include "Timer.h"
#include "Restart.h"
#include "Weather.h"
#include "CarCtrl.h"
#include "CarEnterExit.h"
#include "Population.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "TheCarGenerators.h"
#include "Audio/AudioEngine.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h"

// Parte 6 del codigo vivo (rama vivo-p6b, sin validar): despachadores originales de los comandos de script 300..399 (ranura 3 de s_OriginalCommandHandlerTable, 0x47C100) y 400..499 (ranura 4, 0x47D210),
// que cerraban el hueco 0x47C0F5 de la parte 6. Ambos son thiscall con el numero de comando en la pila (`ret 4`) y AL = 0 (OR_CONTINUE) o 0xFF (OR_INTERRUPT: comando sin caso).
// Solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o no_implementado con caso en la tabla de saltos): ProcessOneCommand
// solo llega a estos despachadores con esos comandos. Los comandos con manejador propio (reescrito, nop) ya no tienen caso aqui, de modo que GTASA_CMD_BYPASS no los devuelve al EXE en estas dos centenas.
// Cada caso sigue el desensamblado (misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas) y las comparaciones x87 con -100.0 (0x859014) conservan su sentido con NaN.
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
// 0x859014: -100.0f. Con z <= -100.0 (ordenado; NaN no) el EXE pide la altura del suelo (CWorld::FindGroundZForCoord 0x569660).
constexpr float EXE_MIN_GROUND_Z = -100.0f;
inline float ScriptGroundZ(float x, float y, float z) {
    if (z <= EXE_MIN_GROUND_Z) {
        return CWorld::FindGroundZForCoord(x, y);
    }
    return z;
}
// 0x859E2C: 360.0f y 0x859878 / 0x8595EC: conversiones entre radianes y grados.
constexpr float EXE_RAD_TO_DEG = std::bit_cast<float>(0x42652EE1u); // 57.29578
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
} // namespace

static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(offsetof(CVehicle, m_nNumPassengers) == 0x484 && offsetof(CVehicle, m_nMaxPassengers) == 0x488);
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(tHandlingData, m_fTurnMass) == 0xC && offsetof(tHandlingData, m_fBuoyancyConstant) == 0x24);
static_assert(offsetof(CPhysical, m_fMass) == 0x8C && offsetof(CPhysical, m_fTurnMass) == 0x90 && offsetof(CPhysical, m_fBuoyancyConstant) == 0xA0);
static_assert(offsetof(CPed, m_pVehicle) == 0x58C && offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_nAnimGroup) == 0x4D4);

// 0x47C100 (lote P6B, sin validar): despachador de los comandos 311..399 (indice = comando - 311, tabla de bytes 0x47D1B4 y de saltos 0x47D0F4). Los comandos 354, 358, 385 y 393 devuelven 0 sin leer
// parametros (0x47D0E0); los demas sin caso (y todo lo que cae fuera de 311..399) devuelven -1 (0x47D0EA).
OpcodeResult CRunningScript::ProcessCommands300To399(int32 command) {
    switch (command) {
    case COMMAND_IS_CAR_MODEL: { // 311, 0x47C127
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        UpdateCompareFlag(static_cast<int16>(vehicle->m_nModelIndex) == P(1).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_CAR_GENERATOR: { // 331, 0x47C169
        CollectParameters(12);
        CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        if (pos.z > EXE_MIN_GROUND_Z) { // `fcomp -100.0; test ah,41h; jne`: solo con z > -100.0
            pos.z += 0.015f; // 0x859F00
        }
        P(0).iParam = CTheCarGenerators::CreateCarGenerator(
            pos, P(3).fParam, P(4).iParam,
            static_cast<int16>(P(5).u16Param), static_cast<int16>(P(6).u16Param),
            static_cast<uint8>(P(7).iParam), static_cast<uint8>(P(8).iParam), static_cast<uint8>(P(9).iParam),
            P(10).u16Param, P(11).u16Param, 0, true);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_DISPLAY_ONSCREEN_TIMER: { // 334, 0x47C284: el indice de la variable se lee antes que los parametros
        const uint16 variable = GetIndexOfGlobalVariable();
        CollectParameters(1);
        CUserDisplay::OnscnTimer.AddClock(variable, nullptr, P(0).iParam != 0 ? eTimerDirection::DECREASE : eTimerDirection::INCREASE);
        return OR_CONTINUE;
    }
    case COMMAND_CLEAR_ONSCREEN_TIMER: { // 335, 0x47C2D2
        const uint16 variable = GetIndexOfGlobalVariable();
        CUserDisplay::OnscnTimer.ClearClock(variable);
        return OR_CONTINUE;
    }
    case COMMAND_CLEAR_ONSCREEN_COUNTER: { // 337, 0x47C2F1
        const uint16 variable = GetIndexOfGlobalVariable();
        CUserDisplay::OnscnTimer.ClearCounter(variable);
        return OR_CONTINUE;
    }
    case COMMAND_SET_TIME_SCALE: { // 349, 0x47C518: copia de 32 bits sin pasar por la x87
        CollectParameters(1);
        CTimer::ms_fTimeScale = P(0).fParam;
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_CAR_OLD: { // 353, 0x47C610
        CollectParameters(3);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC());
        P(0).iParam = CRadar::SetEntityBlip(BLIP_CAR, P(0).iParam, static_cast<uint32>(P(1).iParam), static_cast<eBlipDisplay>(P(2).iParam));
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_REMOVE_BLIP: { // 356, 0x47C660
        CollectParameters(1);
        CRadar::ClearBlip(P(0).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_CHANGE_BLIP_COLOUR: { // 357, 0x47C682
        CollectParameters(2);
        CRadar::ChangeBlipColour(P(0).iParam, static_cast<eBlipColour>(P(1).iParam));
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_COORD_OLD: { // 359, 0x47C6AA
        CollectParameters(5);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC());
        P(0).iParam = CRadar::SetCoordBlip(BLIP_COORD, CVector{ P(0).fParam, P(1).fParam, z }, static_cast<eBlipColour>(P(3).iParam), static_cast<eBlipDisplay>(P(4).iParam), m_szName);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_CHANGE_BLIP_SCALE: { // 360, 0x47C764
        CollectParameters(2);
        CRadar::ChangeBlipScale(P(0).iParam, P(1).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_SET_FADING_COLOUR: { // 361, 0x47C78C
        CollectParameters(3);
        TheCamera.SetFadeColour(static_cast<uint8>(P(0).iParam), static_cast<uint8>(P(1).iParam), static_cast<uint8>(P(2).iParam));
        return OR_CONTINUE;
    }
    case COMMAND_GET_FADING_STATUS: { // 363, 0x47C831
        UpdateCompareFlag(TheCamera.GetFading());
        return OR_CONTINUE;
    }
    case COMMAND_ADD_HOSPITAL_RESTART: { // 364, 0x47C85A
        CollectParameters(5);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        CRestart::AddHospitalRestartPoint(CVector{ P(0).fParam, P(1).fParam, z }, P(3).fParam, P(4).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_POLICE_RESTART: { // 365, 0x47C8DC
        CollectParameters(5);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        CRestart::AddPoliceRestartPoint(CVector{ P(0).fParam, P(1).fParam, z }, P(3).fParam, P(4).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_OVERRIDE_NEXT_RESTART: { // 366, 0x47C95E
        CollectParameters(4);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        CRestart::OverrideNextRestart(CVector{ P(0).fParam, P(1).fParam, z }, P(3).fParam);
        return OR_CONTINUE;
    }
    case COMMAND_GET_CAR_HEADING: { // 372, 0x47CBE1: grados normalizados a [0, 360]
        CollectParameters(1);
        float heading = VehicleOf(P(0).iParam)->GetHeading() * EXE_RAD_TO_DEG;
        if (heading < 0.0f) {
            heading += 360.0f; // 0x859E2C
        }
        if (heading > 360.0f) {
            heading -= 360.0f;
        }
        P(0).fParam = heading;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_HEADING: { // 373, 0x47CC64
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        float heading = P(1).fParam;
        if (heading < 0.0f) {
            heading += 360.0f;
        }
        if (heading > 360.0f) {
            heading -= 360.0f;
        }
        vehicle->SetHeading(heading * EXE_DEG_TO_RAD);
        vehicle->UpdateRwMatrix();
        return OR_CONTINUE;
    }
    case COMMAND_DECLARE_MISSION_FLAG: { // 384, 0x47CE00: lee el entero de 16 bits tras el byte de tipo directamente del script
        m_IP += 1;
        const uint16 offset = *reinterpret_cast<const uint16*>(m_IP);
        m_IP += 2;
        CTheScripts::OnAMissionFlag = offset;
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_HEALTH_GREATER: { // 389, 0x47CE71: parametro entero (fild), comparacion estricta (salud > parametro; NaN no)
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        UpdateCompareFlag(static_cast<float>(P(1).iParam) < vehicle->m_fHealth);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_CAR: { // 390, 0x47CEBF
        CollectParameters(1);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC());
        const auto blip = CRadar::SetEntityBlip(BLIP_CAR, P(0).iParam, 0, BLIP_DISPLAY_BOTH);
        CRadar::ChangeBlipScale(blip, 3);
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_CHAR: { // 391, 0x47CEFD
        CollectParameters(1);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC());
        const auto blip = CRadar::SetEntityBlip(BLIP_CHAR, P(0).iParam, 1, BLIP_DISPLAY_BOTH);
        CRadar::ChangeBlipScale(blip, 3);
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_OBJECT: { // 392, 0x47CF25
        CollectParameters(1);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC());
        const auto blip = CRadar::SetEntityBlip(BLIP_OBJECT, P(0).iParam, 6, BLIP_DISPLAY_BOTH);
        CRadar::ChangeBlipScale(blip, 3);
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_COORD: { // 394, 0x47CF4E
        CollectParameters(3);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC());
        const auto blip = CRadar::SetCoordBlip(BLIP_COORD, CVector{ P(0).fParam, P(1).fParam, z }, static_cast<eBlipColour>(5), BLIP_DISPLAY_BOTH, m_szName);
        CRadar::ChangeBlipScale(blip, 3);
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_CHANGE_BLIP_DISPLAY: { // 395, 0x47D00B
        CollectParameters(2);
        CRadar::ChangeBlipDisplay(P(0).iParam, static_cast<eBlipDisplay>(P(1).iParam));
        return OR_CONTINUE;
    }
    case COMMAND_ADD_ONE_OFF_SOUND: { // 396, 0x47D034
        CollectParameters(4);
        CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        AudioEngine.ReportMissionAudioEvent(P(3).u16Param, pos);
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_STUCK_ON_ROOF: { // 399, 0x47D0AC
        CollectParameters(1);
        UpdateCompareFlag(CTheScripts::UpsideDownCars.HasCarBeenUpsideDownForAWhile(P(0).iParam));
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_CHAR_OLD:               // 354
    case COMMAND_DIM_BLIP:                            // 358
    case COMMAND_DECLARE_MISSION_FLAG_FOR_CONTACT:    // 385
    case COMMAND_ADD_BLIP_FOR_CONTACT_POINT:          // 393
        return OR_CONTINUE; // 0x47D0E0: `xor al, al; ret 4` sin leer parametros
    default: // 0x47D0EA
        return OR_INTERRUPT;
    }
}

// 0x47D210 (lote P6B, sin validar): despachador de los comandos 400..499 (indice = comando - 400, tabla de bytes 0x47DFE0 y de saltos 0x47DF58). El EXE lo envuelve en un marco SEH (manejador 0x838329) que solo
// libera el `new` de peaton de CREATE_CHAR_AS_PASSENGER si su constructor lanza; no hace falta en C++. Los comandos 417..426 comparten caso (CharInAreaCheckCommand con el numero de comando), 427 y 428
// (CarInAreaCheckCommand) y 429..432 (LocateCarCommand). Los demas sin caso devuelven -1 (0x47DF40).
OpcodeResult CRunningScript::ProcessCommands400To499(int32 command) {
    switch (command) {
    case COMMAND_ADD_UPSIDEDOWN_CAR_CHECK: { // 400, 0x47D24E
        CollectParameters(1);
        CTheScripts::UpsideDownCars.AddCarToCheck(P(0).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_REMOVE_UPSIDEDOWN_CAR_CHECK: { // 401, 0x47D27E
        CollectParameters(1);
        CTheScripts::UpsideDownCars.RemoveCarFromCheck(P(0).iParam);
        return OR_CONTINUE;
    }
    case COMMAND_IS_CHAR_IN_AREA_ON_FOOT_2D:               // 417..426, 0x47D2AD
    case COMMAND_IS_CHAR_IN_AREA_IN_CAR_2D:
    case COMMAND_IS_CHAR_STOPPED_IN_AREA_2D:
    case COMMAND_IS_CHAR_STOPPED_IN_AREA_ON_FOOT_2D:
    case COMMAND_IS_CHAR_STOPPED_IN_AREA_IN_CAR_2D:
    case COMMAND_IS_CHAR_IN_AREA_ON_FOOT_3D:
    case COMMAND_IS_CHAR_IN_AREA_IN_CAR_3D:
    case COMMAND_IS_CHAR_STOPPED_IN_AREA_3D:
    case COMMAND_IS_CHAR_STOPPED_IN_AREA_ON_FOOT_3D:
    case COMMAND_IS_CHAR_STOPPED_IN_AREA_IN_CAR_3D:
        CharInAreaCheckCommand(command);
        return OR_CONTINUE;
    case COMMAND_IS_CAR_STOPPED_IN_AREA_2D: // 427, 428, 0x47D2CB
    case COMMAND_IS_CAR_STOPPED_IN_AREA_3D:
        CarInAreaCheckCommand(command);
        return OR_CONTINUE;
    case COMMAND_LOCATE_CAR_2D: // 429..432, 0x47D2E9
    case COMMAND_LOCATE_STOPPED_CAR_2D:
    case COMMAND_LOCATE_CAR_3D:
    case COMMAND_LOCATE_STOPPED_CAR_3D:
        LocateCarCommand(command);
        return OR_CONTINUE;
    case COMMAND_GIVE_WEAPON_TO_CHAR: { // 434, 0x47D307
        CollectParameters(3);
        CPed* const ped = PedOf(P(0).iParam);
        const auto slot = ped->GiveWeapon(static_cast<eWeaponType>(P(1).iParam), static_cast<uint32>(P(2).iParam), true);
        if (ped->IsPlayer()) {
            ped->GetPlayerData()->m_nChosenWeapon = static_cast<uint8>(slot);
            return OR_CONTINUE;
        }
        ped->SetCurrentWeapon(static_cast<int32>(slot));
        if (ped->bInVehicle && ped->m_pVehicle) {
            const auto weaponType = ped->m_aWeapons[static_cast<int8>(ped->m_nActiveWeaponSlot)].m_Type;
            ped->RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nModelId1);
        }
        return OR_CONTINUE;
    }
    case COMMAND_FORCE_WEATHER: { // 437, 0x47D42B
        CollectParameters(1);
        CWeather::ForceWeather(static_cast<eWeatherType>(P(0).u16Param));
        return OR_CONTINUE;
    }
    case COMMAND_FORCE_WEATHER_NOW: { // 438, 0x47D45C
        CollectParameters(1);
        CWeather::ForceWeatherNow(static_cast<eWeatherType>(P(0).u16Param));
        return OR_CONTINUE;
    }
    case COMMAND_RELEASE_WEATHER: { // 439, 0x47D48D
        CWeather::ReleaseWeather();
        return OR_CONTINUE;
    }
    case COMMAND_GET_GAME_TIMER: { // 445, 0x47D69E
        P(0).iParam = static_cast<int32>(CTimer::GetTimeInMS());
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_STORE_WANTED_LEVEL: { // 448, 0x47D6A8
        CollectParameters(1);
        P(0).iParam = static_cast<int32>(CWorld::Players[P(0).iParam].m_pPed->GetWantedLevel());
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_STOPPED: { // 449, 0x47D6CD
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        UpdateCompareFlag(CTheScripts::IsVehicleStopped(vehicle));
        return OR_CONTINUE;
    }
    case COMMAND_MARK_CAR_AS_NO_LONGER_NEEDED: { // 451, 0x47D75A
        CollectParameters(1);
        CTheScripts::CleanUpThisVehicle(VehicleOf(P(0).iParam));
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
        }
        return OR_CONTINUE;
    }
    case COMMAND_DONT_REMOVE_CHAR: { // 453, 0x47D7E1: sin comprobar la limpieza de mision
        CollectParameters(1);
        CTheScripts::MissionCleanUp.RemoveEntityFromList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PED);
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_CHAR_AS_PASSENGER: { // 456, 0x47D845
        CollectParameters(4);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const ePedType pedType = static_cast<ePedType>(P(1).iParam);
        uint32 typeSpecificModelId = static_cast<uint32>(P(2).iParam);
        GetCorrectPedModelIndexForEmergencyServiceType(pedType, &typeSpecificModelId);
        P(2).iParam = static_cast<int32>(typeSpecificModelId); // el EXE pasa la direccion de ScriptParams[2]
        CPed* ped;
        if (pedType == PED_TYPE_COP) {
            ped = new CCopPed{ typeSpecificModelId };
        } else if (pedType == PED_TYPE_MEDIC || pedType == PED_TYPE_FIREMAN) {
            ped = new CEmergencyPed{ pedType, typeSpecificModelId };
        } else {
            ped = new CCivilianPed{ pedType, typeSpecificModelId };
        }
        ped->SetCharCreatedBy(PED_MISSION);
        ped->bAllowMedicsToReviveMe = false; // bit 31 de las banderas del peaton
        if (vehicle->vehicleFlags.bIsBus) { // byte +0x429, bit 1
            ped->bRenderPedInCar = false; // bit 13
        }
        int32 door = 0;
        if (P(3).iParam >= 0) {
            door = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(vehicle, P(3).iParam);
        }
        {
            CTaskSimpleCarSetPedInAsPassenger task{ vehicle, static_cast<eTargetDoor>(door), true }; // el EXE (0x47D98A) pone m_bWarpingInToCar (+0x1C)=1 tras construir: sin utilidad (nula) ProcessPed no debe llamar a CTaskUtilityLineUpPedWithCar::ProcessPed
            task.ProcessPed(ped);
        }
        CWorld::Add(ped);
        CPopulation::ms_nTotalMissionPeds++;
        P(0).iParam = CPools::GetPedRef(ped);
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PED);
        }
        return OR_CONTINUE;
    }
    case COMMAND_GET_NUMBER_OF_PASSENGERS: { // 489, 0x47DD2F
        CollectParameters(1);
        P(0).iParam = VehicleOf(P(0).iParam)->m_nNumPassengers;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_GET_MAXIMUM_NUMBER_OF_PASSENGERS: { // 490, 0x47DD75
        CollectParameters(1);
        P(0).iParam = VehicleOf(P(0).iParam)->m_nMaxPassengers;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_DENSITY_MULTIPLIER: { // 491, 0x47DDBB
        CollectParameters(1);
        CCarCtrl::CarDensityMultiplier = P(0).fParam;
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_HEAVY: { // 492, 0x47DDE6: con el vehiculo NULL no hace nada; si no masa x3, masa de giro x5 y flotabilidad x2 (o los valores del manejo tal cual) y bMakeMassTwiceAsBig
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle) {
            tHandlingData* const handling = vehicle->m_pHandlingData;
            if (P(1).iParam != 0) {
                vehicle->physicalFlags.bMakeMassTwiceAsBig = true;
                vehicle->m_fMass = handling->m_fMass * 3.0f;
                vehicle->m_fTurnMass = handling->m_fTurnMass * 5.0f;
                vehicle->m_fBuoyancyConstant = handling->m_fBuoyancyConstant + handling->m_fBuoyancyConstant;
            } else {
                vehicle->physicalFlags.bMakeMassTwiceAsBig = false;
                vehicle->m_fMass = handling->m_fMass;
                vehicle->m_fTurnMass = handling->m_fTurnMass;
                vehicle->m_fBuoyancyConstant = handling->m_fBuoyancyConstant;
            }
        }
        return OR_CONTINUE;
    }
    default: // 0x47DF40
        return OR_INTERRUPT;
    }
}
