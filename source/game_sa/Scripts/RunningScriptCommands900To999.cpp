#include "StdInc.h"

#include <bit>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "World.h"
#include "General.h"
#include "Messages.h"
#include "Text/Text.h"
#include "Garages.h"
#include "Streaming.h"
#include "PathFind.h"
#include "Pickups.h"
#include "Radar.h"
#include "3dMarkers.h"
#include "CarCtrl.h"
#include "Population.h"
#include "LoadedCarGroup.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "MenuManager.h"
#include "Wanted.h"
#include "AutoPilot.h"
#include "Audio/AudioEngine.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Building.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/MonsterTruck.h"
#include "Entity/Vehicle/QuadBike.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Bmx.h"
#include "Entity/Vehicle/Trailer.h"
#include "Entity/Ped/PlayerPed.h"

// Rama vivo-scra (sin validar): despachador original de los comandos de script 900..999 (ranura 9 de s_OriginalCommandHandlerTable, 0x483BD0). Thiscall con el numero de comando en la pila (`ret 4`);
// indice = comando - 900, tabla de bytes 0x4857C4 y de saltos 0x4856B8; fuera de rango o sin caso salta a 0x485699 (`or al, 0FFh`). Todos los casos con cuerpo terminan en `xor al, al` (OR_CONTINUE).
// Solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o no_implementado con caso en la tabla de saltos);
// el resto va a ProcessHandledCommand (manejador propio si existe, si no OR_INTERRUPT como el destino por defecto del EXE). El EXE envuelve el despachador en un marco SEH (manejador 0x838430) que solo
// libera el `new` de vehiculo de CREATE_RANDOM_CAR_FOR_CAR_PARK si su constructor lanza; no hace falta en C++.
// Cada caso sigue el desensamblado (misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas). Las comparaciones x87 `fcomp; test ah,41h; jne` (intercambio si a > b) y
// `fcom -100.0; test ah,41h; jp` (suelo si z <= -100.0) conservan su sentido con NaN.
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef), 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x465040 (CPool<CObject>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CObject* ObjectOf(int32 handle) {
    return CPools::GetObject(handle);
}
// 0x859014: -100.0f. Con z <= -100.0 (ordenado; NaN no) el EXE pide la altura del suelo (CWorld::FindGroundZForCoord 0x569660).
constexpr float EXE_MIN_GROUND_Z = -100.0f;
inline float ScriptGroundZ(float x, float y, float z) {
    if (z <= EXE_MIN_GROUND_Z) {
        return CWorld::FindGroundZForCoord(x, y);
    }
    return z;
}
// Constantes de .rdata
constexpr float EXE_ONE_FIFTIETH  = std::bit_cast<float>(0x3CA3D70Au); // 0x858B38: 0.02
constexpr float EXE_HALF_PI       = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4: pi / 2
constexpr float EXE_TWO_PI        = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC: 2 pi
constexpr float EXE_DEG_TO_RAD    = std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC
constexpr float EXE_NODE_MAX_DIST = std::bit_cast<float>(0x497423FEu); // inmediato de 0x4852B8: 999999.875
constexpr float EXE_MARKER_PULSE  = std::bit_cast<float>(0x3DCCCCCDu); // inmediato de 0x4845B1: 0.1
constexpr float EXE_PARK_SPEED    = std::bit_cast<float>(0x41100000u); // inmediato de 0x484FCF: 9.0

// Identificador de marcador/esfera del EXE: `mov edx, [esi+14h]; add edx, esi` (m_IP + this).
inline uint32 ScriptThingId(const CRunningScript* script, const uint8* ip) {
    return static_cast<uint32>(reinterpret_cast<uintptr_t>(script) + reinterpret_cast<uintptr_t>(ip));
}
// Indice de modelo de un parametro de script: negativo = entrada de CTheScripts::UsedObjectArray (0xA44B70, campo nModelIndex en +0x18).
inline int32 ScriptModelIndex(int32 model) {
    if (model < 0) {
        return CTheScripts::UsedObjectArray.data()[-model].nModelIndex;
    }
    return model;
}
} // namespace

static_assert(offsetof(CVehicle, m_autoPilot) == 0x390 && offsetof(CVehicle, m_nVehicleType) == 0x590);
static_assert(offsetof(CAutoPilot, m_nCurrentLane) == 0x27 && offsetof(CAutoPilot, m_nNextLane) == 0x28 && offsetof(CAutoPilot, m_nCarDrivingStyle) == 0x29);
static_assert(offsetof(CAutoPilot, m_nCarMission) == 0x2A && offsetof(CAutoPilot, m_nTempAction) == 0x2B && offsetof(CAutoPilot, m_speed) == 0x38 && offsetof(CAutoPilot, m_nCruiseSpeed) == 0x40);
static_assert(offsetof(CAutomobile, autoFlags) == 0x868 && offsetof(CBike, nBikeFlags) == 0x614);
static_assert(offsetof(COnscreenTimer, m_bPaused) == 0x151); // 0xBA18D9 = CUserDisplay::OnscnTimer (0xBA1788) + 0x151
static_assert(offsetof(CMenuManager, m_bMenuActive) == 0x5C && offsetof(CMenuManager, m_bIsSaveDone) == 0x5F); // 0xBA67A4 y 0xBA67A7 (FrontEndMenuManager = 0xBA6748)
static_assert(offsetof(CWanted, m_Multiplier) == 0x14);

// 0x483BD0 (rama vivo-scra, sin validar): despachador de los comandos 900..999.
OpcodeResult CRunningScript::ProcessCommands900To999(int32 command) {
    switch (command) {
    case COMMAND_PRINT_STRING_IN_STRING_NOW: { // 900, 0x483C11: dos etiquetas en el mismo buffer, despues dos parametros
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const text = TheText.Get(label);
        ReadTextLabelFromScript(label, 8);
        const GxtChar* const string = TheText.Get(label);
        CollectParameters(2);
        CMessages::AddMessageJumpQWithString(text, static_cast<uint32>(P(0).iParam), P(1).u16Param, const_cast<GxtChar*>(string), CTheScripts::bAddNextMessageToPreviousBriefs);
        CTheScripts::bAddNextMessageToPreviousBriefs = true;
        return OR_CONTINUE;
    }
    case COMMAND_IS_POINT_OBSCURED_BY_A_MISSION_ENTITY: { // 906, 0x483C8C
        CollectParameters(6);
        const float x = P(0).fParam, y = P(1).fParam, z = P(2).fParam;
        const float rx = P(3).fParam, ry = P(4).fParam, rz = P(5).fParam;
        float minX = x - rx;
        float maxX = rx + x;
        float minY = y - ry;
        float maxY = ry + y;
        float minZ = z - rz;
        float maxZ = rz + z;
        if (minX > maxX) { // `fcomp; test ah,41h; jne`: solo con min > max (ordenado)
            std::swap(minX, maxX);
        }
        if (minY > maxY) {
            std::swap(minY, maxY);
        }
        if (minZ > maxZ) {
            std::swap(minZ, maxZ);
        }
        const CVector cornerA{ minX, minY, minZ };
        const CVector cornerB{ maxX, maxY, maxZ };
        int16 count = 0; // la funcion lo pone a 0 al empezar
        CWorld::FindMissionEntitiesIntersectingCube(cornerA, cornerB, &count, 2, nullptr, true, true, true); // 0x569240
        UpdateCompareFlag(count > 0);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_TO_OBJECT_VELOCITY: { // 908, 0x483DF6: velocidad += parametro * 0.02 (componente a componente)
        CollectParameters(4);
        CObject* const object = ObjectOf(P(0).iParam);
        CVector& speed = object->m_vecMoveSpeed;
        speed.x = P(1).fParam * EXE_ONE_FIFTIETH + speed.x;
        speed.y = P(2).fParam * EXE_ONE_FIFTIETH + speed.y;
        speed.z = P(3).fParam * EXE_ONE_FIFTIETH + speed.z;
        return OR_CONTINUE;
    }
    case COMMAND_SET_OBJECT_DYNAMIC: { // 914, 0x4841E3: bIsStatic es el bit 2 del byte +0x1C; SetIsStatic es la entrada +0x10 de la vtable
        CollectParameters(2);
        CObject* const object = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            if (object->m_bIsStatic) {
                object->SetIsStatic(false);
                object->AddToMovingList(); // 0x542800
            }
        } else {
            if (!object->m_bIsStatic) {
                object->SetIsStatic(true);
                object->RemoveFromMovingList(); // 0x542860
            }
        }
        return OR_CONTINUE;
    }
    case COMMAND_PLAY_MISSION_PASSED_TUNE: { // 916, 0x4842AF: solo con 1 o 2 (pistas 11 y 12)
        CollectParameters(1);
        const int32 tune = P(0).iParam;
        if (tune == 1 || tune == 2) {
            AudioEngine.PreloadBeatTrack(static_cast<int16>(tune + 10)); // 0x507F40
            AudioEngine.PlayPreloadedBeatTrack(true);                    // 0x507180
        }
        return OR_CONTINUE;
    }
    case COMMAND_CLEAR_AREA: { // 917, 0x4842EC
        CollectParameters(5);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        const CVector point{ P(0).fParam, P(1).fParam, z };
        CWorld::ClearExcitingStuffFromArea(point, P(3).fParam, static_cast<uint8>(P(4).iParam)); // 0x56A0D0
        return OR_CONTINUE;
    }
    case COMMAND_FREEZE_ONSCREEN_TIMER: { // 918, 0x484370: byte 0xBA18D9
        CollectParameters(1);
        CUserDisplay::OnscnTimer.m_bPaused = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SWITCH_CAR_SIREN: { // 919, 0x48438F: byte +0x42D, bit 7
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        vehicle->vehicleFlags.bSirenOrAlarm = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_WATERTIGHT: { // 924, 0x4843DA: solo automoviles (+0x868 bit 2) y motos (+0x614 bit 2); otros tipos no hacen nada
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE) {
            static_cast<CAutomobile*>(vehicle)->autoFlags.bWaterTight = P(1).iParam != 0;
        } else if (vehicle->m_nVehicleType == VEHICLE_TYPE_BIKE) {
            static_cast<CBike*>(vehicle)->bikeFlags.bWaterTight = P(1).iParam != 0;
        }
        return OR_CONTINUE;
    }
    case COMMAND_TURN_CAR_TO_FACE_COORD: { // 927, 0x4844B8: rumbo = ATanOfXY(pos - punto) + pi/2, menos 2 pi si pasa de 2 pi; sin UpdateRwMatrix
        CollectParameters(3);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const CVector& pos = vehicle->GetPosition();
        const float dx = pos.x - P(1).fParam;
        const float dy = pos.y - P(2).fParam;
        float heading = CGeneral::GetATanOfXY(dx, dy) + EXE_HALF_PI; // 0x53CC70
        if (heading > EXE_TWO_PI) {
            heading -= EXE_TWO_PI;
        }
        vehicle->SetHeading(heading); // 0x43E0C0
        return OR_CONTINUE;
    }
    case COMMAND_DRAW_SPHERE: { // 929, 0x484562: cilindro rojo (255, 0, 0, 228), pulso 2048 / 0.1, sin giro
        CollectParameters(4);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        CVector pos{ P(0).fParam, P(1).fParam, z };
        C3dMarkers::PlaceMarkerSet(ScriptThingId(this, m_IP), MARKER3D_CYLINDER, pos, P(3).fParam, 255, 0, 0, 228, 2048, EXE_MARKER_PULSE, 0); // 0x725BA0
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_STATUS: { // 930, 0x484607: de STATUS_SIMPLE a otro estado distinto de 2 pasa a fisica real; el estado son los 5 bits altos del byte +0x36
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle->GetStatus() == STATUS_SIMPLE && P(1).iParam != STATUS_SIMPLE) {
            CCarCtrl::SwitchVehicleToRealPhysics(vehicle); // 0x423FC0
        }
        vehicle->SetStatus(static_cast<eEntityStatus>(P(1).u8Param));
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_STRONG: { // 939, 0x48470F: byte +0x429, bit 7
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        vehicle->vehicleFlags.bTakeLessDamage = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_SWITCH_STREAMING: { // 943, 0x48478E: byte 0x9654B0 = (parametro == 0)
        CollectParameters(1);
        CStreaming::ms_disableStreaming = P(0).iParam == 0;
        return OR_CONTINUE;
    }
    case COMMAND_IS_GARAGE_OPEN: { // 944, 0x4847AD
        char name[8];
        ReadTextLabelFromScript(name, 8);
        const int16 garage = CGarages::GetGarageNumberByName(name); // 0x447680
        bool result = false;
        if (garage >= 0) {
            result = CGarages::IsGarageOpen(garage); // 0x447D00
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_IS_GARAGE_CLOSED: { // 945, 0x4847E4
        char name[8];
        ReadTextLabelFromScript(name, 8);
        const int16 garage = CGarages::GetGarageNumberByName(name); // 0x447680
        bool result = false;
        if (garage >= 0) {
            result = CGarages::IsGarageClosed(garage); // 0x447D30
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_SWAP_NEAREST_BUILDING_MODEL: { // 950, 0x48481B: el edificio mas cercano (distancia 3D < 2 * radio) del modelo origen pasa al modelo destino
        CollectParameters(6);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = ScriptGroundZ(x, y, P(2).fParam);
        const float radius = P(3).fParam;
        const int32 modelFrom = ScriptModelIndex(P(4).iParam);
        float bestDist = radius + radius; // `fadd st(0), st(0)`
        const int32 modelTo = ScriptModelIndex(P(5).iParam);
        CEntity* best = nullptr;
        CEntity* found[16];
        int16 count = 0; // la funcion lo pone a 0 al empezar
        const CVector pos{ x, y, z };
        CWorld::FindObjectsOfTypeInRange(static_cast<uint32>(modelFrom), pos, radius, true, &count, 16, found, true, false, false, false, false); // 0x564C70
        const int32 foundCount = count; // movsx
        if (foundCount <= 0) {
            return OR_CONTINUE;
        }
        for (uint16 i = 0; i < foundCount; ++i) {
            CEntity* const entity = found[i];
            const CVector& ePos = entity->GetPosition();
            const CVector delta{ ePos.x - x, ePos.y - y, ePos.z - z };
            const float dist = delta.Magnitude(); // 0x4082C0
            if (dist < bestDist) { // `fcom; test ah,5; jp`: solo con dist < mejor (ordenado)
                bestDist = dist;
                best = entity;
            }
        }
        if (best) {
            CBuilding* const building = static_cast<CBuilding*>(best);
            building->ReplaceWithNewModel(modelTo);                                // 0x403EC0
            CTheScripts::AddToBuildingSwapArray(building, modelFrom, modelTo);     // 0x481140
        }
        return OR_CONTINUE;
    }
    case COMMAND_SWITCH_WORLD_PROCESSING: { // 951, 0x4849CC: byte 0xB7CD6D = (parametro == 0)
        CollectParameters(1);
        CWorld::bProcessCutsceneOnly = P(0).iParam == 0;
        return OR_CONTINUE;
    }
    case COMMAND_CLEAR_AREA_OF_CARS: { // 954, 0x4849EB: cada eje se ordena (intercambio solo con min > max, ordenado)
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
        CWorld::ClearCarsFromArea(x1, y1, z1, x2, y2, z2); // 0x566610
        return OR_CONTINUE;
    }
    case COMMAND_ADD_SPHERE: { // 956, 0x484AB4
        CollectParameters(4);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = ScriptGroundZ(x, y, P(2).fParam);
        const float radius = P(3).fParam;
        (void)CTheScripts::GetActualScriptThingIndex(CollectNextParameterWithoutIncreasingPC(), SCRIPT_THING_SPHERE); // 0x464250 + 0x4839A0 (el resultado no se usa)
        P(0).iParam = static_cast<int32>(CTheScripts::AddScriptSphere(ScriptThingId(this, m_IP), CVector{ x, y, z }, radius)); // 0x483B30
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_REMOVE_SPHERE: { // 957, 0x484B77
        CollectParameters(1);
        CTheScripts::RemoveScriptSphere(P(0).iParam); // 0x483BA0
        return OR_CONTINUE;
    }
    case COMMAND_DISPLAY_ONSCREEN_TIMER_WITH_STRING: { // 963, 0x484C57: variable, parametro y etiqueta; el texto de TheText no se usa (se pasa la etiqueta)
        const uint16 variable = GetIndexOfGlobalVariable();
        CollectParameters(1);
        const bool decrease = P(0).iParam != 0;
        char label[8];
        ReadTextLabelFromScript(label, 8);
        (void)TheText.Get(label); // 0x6A0050 (el resultado no se usa)
        CUserDisplay::OnscnTimer.AddClock(variable, label, decrease ? eTimerDirection::DECREASE : eTimerDirection::INCREASE); // 0x44CD50
        return OR_CONTINUE;
    }
    case COMMAND_DISPLAY_ONSCREEN_COUNTER_WITH_STRING: { // 964, 0x484CB3: igual, con el tipo de contador (16 bits) y linea 0
        const uint16 variable = GetIndexOfGlobalVariable();
        CollectParameters(1);
        char label[8];
        ReadTextLabelFromScript(label, 8);
        (void)TheText.Get(label); // 0x6A0050 (el resultado no se usa)
        CUserDisplay::OnscnTimer.AddCounter(variable, static_cast<eOnscreenCounter>(P(0).u16Param), label, 0); // 0x44CDA0
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_RANDOM_CAR_FOR_CAR_PARK: { // 965, 0x484D08
        CollectParameters(4);
        if (CCarCtrl::NumRandomCars >= 45) { // 0x969094
            return OR_CONTINUE;
        }
        const auto modelId = static_cast<int32>(CPopulation::m_AppropriateLoadedCars.PickRandomCar(false, true)); // 0xC0E9F8, 0x611C50
        if (modelId == -1) {
            return OR_CONTINUE;
        }
        // Tabla de saltos 0x485828 sobre (tipo - 1): automovil (0), barco, tren y avioneta (8) usan el caso por defecto 0x484EC0 (CAutomobile)
        CVehicle* vehicle;
        switch (CModelInfo::GetModelInfo(modelId)->AsVehicleModelInfoPtr()->m_nVehicleType) {
        case VEHICLE_TYPE_MTRUCK:
            vehicle = new CMonsterTruck(modelId, RANDOM_VEHICLE); // 0x6C8D60
            break;
        case VEHICLE_TYPE_QUAD:
            vehicle = new CQuadBike(modelId, RANDOM_VEHICLE); // 0x6CE370
            break;
        case VEHICLE_TYPE_HELI:
            vehicle = new CHeli(modelId, RANDOM_VEHICLE); // 0x6C4190
            break;
        case VEHICLE_TYPE_PLANE:
            vehicle = new CPlane(modelId, RANDOM_VEHICLE); // 0x6C8E20
            break;
        case VEHICLE_TYPE_BIKE: {
            CBike* const bike = new CBike(modelId, RANDOM_VEHICLE); // 0x6BF430
            bike->bikeFlags.bOnSideStand = true; // +0x614 |= 0x10
            vehicle = bike;
            break;
        }
        case VEHICLE_TYPE_BMX: {
            CBmx* const bmx = new CBmx(modelId, RANDOM_VEHICLE); // 0x6BF820
            bmx->bikeFlags.bOnSideStand = true;
            vehicle = bmx;
            break;
        }
        case VEHICLE_TYPE_TRAILER:
            vehicle = new CTrailer(modelId, RANDOM_VEHICLE); // 0x6D03A0
            break;
        default:
            vehicle = new CAutomobile(modelId, RANDOM_VEHICLE, true); // 0x6B0A90
            break;
        }
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = P(2).fParam;
        const CVector pos{ x, y, vehicle->GetHeightAboveRoad() + z }; // entrada +0xD4 de la vtable
        vehicle->SetPosn(pos);                                  // 0x4241C0
        vehicle->SetHeading(P(3).fParam * EXE_DEG_TO_RAD);      // 0x43E0C0
        CTheScripts::ClearSpaceForMissionEntity(pos, vehicle);  // 0x486B00
        vehicle->SetStatus(STATUS_ABANDONED);
        vehicle->vehicleFlags.bIsLocked = false;
        vehicle->vehicleFlags.bIsCarParkVehicle = true;
        CCarCtrl::JoinCarWithRoadSystem(vehicle); // 0x42F5A0
        CAutoPilot& autoPilot = vehicle->m_autoPilot;
        if (autoPilot.m_nCarMission != MISSION_PLANE_CRASH_AND_BURN && autoPilot.m_nCarMission != MISSION_HELI_CRASH_AND_BURN) {
            autoPilot.m_nCarMission = MISSION_NONE;
        }
        autoPilot.m_nTempAction = TEMPACT_NONE;
        autoPilot.m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
        autoPilot.m_speed = EXE_PARK_SPEED;
        autoPilot.m_nCruiseSpeed = 9;
        autoPilot.m_nCurrentLane = 0;
        autoPilot.m_nNextLane = 0;
        vehicle->vehicleFlags.bEngineOn = false;
        CWorld::Add(vehicle); // 0x563220
        return OR_CONTINUE;
    }
    case COMMAND_SET_WANTED_MULTIPLIER: { // 967, 0x485003: copia de 32 bits
        CollectParameters(1);
        FindPlayerWanted(-1)->m_Multiplier = P(0).fParam; // 0x56E230
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_VISIBLY_DAMAGED: { // 969, 0x485037: byte +0x42A, bit 0
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        UpdateCompareFlag(vehicle->vehicleFlags.bIsDamaged);
        return OR_CONTINUE;
    }
    case COMMAND_HAS_MISSION_AUDIO_LOADED: { // 976, 0x4851C7: estado de carga (byte) == 1
        CollectParameters(1);
        const auto status = static_cast<uint8>(AudioEngine.GetMissionAudioLoadingStatus(static_cast<uint8>(P(0).iParam - 1))); // 0x5072A0
        UpdateCompareFlag(status == 1);
        return OR_CONTINUE;
    }
    case COMMAND_HAS_MISSION_AUDIO_FINISHED: { // 978, 0x485228
        CollectParameters(1);
        UpdateCompareFlag(AudioEngine.IsMissionAudioSampleFinished(static_cast<uint8>(P(0).iParam - 1))); // 0x5072C0
        return OR_CONTINUE;
    }
    case COMMAND_GET_CLOSEST_CAR_NODE_WITH_HEADING: { // 979, 0x485261: sin nodo valido devuelve cuatro ceros y la bandera a falso
        CollectParameters(3);
        const float z = ScriptGroundZ(P(0).fParam, P(1).fParam, P(2).fParam);
        const CVector pos{ P(0).fParam, P(1).fParam, z };
        const CNodeAddress node = ThePaths.FindNodeClosestToCoors(pos, PATH_TYPE_VEH, EXE_NODE_MAX_DIST, 0, 1, 0, 0, 0); // 0x44F460
        bool found = false;
        const CVector coors = ThePaths.FindNodeCoorsForScript(node, &found); // 0x4505E0
        bool result;
        if (found) {
            P(2).fParam = coors.z;
            P(0).fParam = coors.x;
            P(1).fParam = coors.y;
            P(3).fParam = ThePaths.FindNodeOrientationForCarPlacement(node); // 0x450320
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
    case COMMAND_SET_MISSION_AUDIO_POSITION: { // 983, 0x4853D9
        CollectParameters(4);
        CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        AudioEngine.SetMissionAudioPosition(static_cast<uint8>(P(0).iParam - 1), pos); // 0x507300
        return OR_CONTINUE;
    }
    case COMMAND_ACTIVATE_SAVE_MENU: { // 984, 0x48542C: solo con jugador a pie; activa el menu de guardado (0xBA67A7) y para al jugador
        if (FindPlayerPed(-1) && !FindPlayerPed(-1)->bInVehicle) { // 0x56E210; bInVehicle = bit 8 de +0x46C
            FrontEndMenuManager.m_bIsSaveDone = true;
            FindPlayerPed(-1)->SetVelocity(CVector{ 0.0f, 0.0f, 0.0f }); // 0x441130
            FindPlayerPed(-1)->ResetTurnSpeed();                         // 0x45AFB0 con (0, 0, 0)
        }
        return OR_CONTINUE;
    }
    case COMMAND_HAS_SAVE_GAME_FINISHED: { // 985, 0x48548A: ni menu activo (0xBA67A4) ni menu de guardado pendiente (0xBA67A7)
        bool result;
        if (FrontEndMenuManager.m_bMenuActive) {
            result = false;
        } else {
            result = !FrontEndMenuManager.m_bIsSaveDone;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_BLIP_FOR_PICKUP: { // 988, 0x4854B9: el EXE pasa ademas el nombre del script (this + 8) como quinto argumento, que SetEntityBlip no usa
        CollectParameters(1);
        (void)CPickups::GetActualPickupIndex(tPickupReference{ P(0).iParam }); // 0x4552A0 (el resultado no se usa)
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC()); // 0x464250 + 0x582870 (el resultado no se usa)
        const auto blip = CRadar::SetEntityBlip(BLIP_PICKUP, P(0).iParam, 6, BLIP_DISPLAY_BOTH); // 0x5839A0
        CRadar::ChangeBlipScale(blip, 3); // 0x583CC0
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_FORCE_RANDOM_PED_TYPE: // 991, 0x485695: `xor al, al` sin leer parametros (destino compartido con 937, 980, 986 y 993, que tienen manejador propio)
        return OR_CONTINUE;
    default: // 0x485699 y comandos con manejador propio
        return ProcessHandledCommand(command);
    }
}
