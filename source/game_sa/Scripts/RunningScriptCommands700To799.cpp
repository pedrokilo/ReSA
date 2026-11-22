#include "StdInc.h"

#include <bit>
#include <cmath>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Collision/ColModel.h"
#include "Collision/ColPoint.h"
#include "ModelIndices.h"
#include "Camera.h"
#include "Timer.h"
#include "CarCtrl.h"
#include "AutoPilot.h"
#include "PathFind.h"
#include "WaterLevel.h"
#include "SurfaceInfos_c.h"
#include "FireManager.h"
#include "Pickups.h"
#include "Pickup.h"
#include "CutsceneMgr.h"
#include "ProjectileInfo.h"
#include "Garages.h"
#include "Garage.h"
#include "Stats.h"
#include "Darkel.h"
#include "Text/Text.h"

// Rama vivo-scra (sin validar): 0x47FA30 (7000 bytes), CRunningScript::ProcessCommands700To799, el despachador original de los comandos de script 700..799 (ranura 7 de la tabla 0x8A6168).
// Thiscall con el numero de comando en la pila (`ret 4`; resultado en AL). Indice = comando - 703 (`add eax, 0xFFFFFD41; cmp eax, 0x5F`), tabla de bytes 0x480FD8 y de saltos 0x480F10 (cubre 703..798);
// los comandos sin caso y los fuera de rango van a 0x480F00 (`or al, 0xFF` = OR_INTERRUPT). Los comandos 741, 742, 755 y 793 comparten 0x480EF1 (`xor al, al; ret 4` sin leer parametros).
// Solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o no_implementado con caso en la tabla de saltos);
// los demas (reescrito, nop) caen en ProcessHandledCommand (manejador propio si lo hay; si no, OR_INTERRUPT como el destino por defecto del EXE). Cada caso sigue el desensamblado (capstone sobre el
// EXE canonico): misma secuencia de CollectParameters / lecturas de ScriptParams (0xA43C78) / llamadas, mismas constantes de .rdata y las comparaciones x87 conservan su sentido con NaN.
// Este despachador no monta marco SEH propio (no crea tareas).
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef), 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x465040 (CPool<CObject>::GetAtRef): sin comprobar el resultado salvo donde el EXE lo hace.
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CObject* ObjectOf(int32 handle) {
    return CPools::GetObject(handle);
}
// 0x859014: float -100.0 (0xC2C80000). Con z <= -100.0 (`fcom; test ah,41h; jp`: ordenado, con NaN no) el EXE pide la altura del suelo (CWorld::FindGroundZForCoord 0x569660).
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
inline float ScriptGroundZ(float x, float y, float z) {
    if (z <= EXE_MIN_GROUND_Z) {
        return CWorld::FindGroundZForCoord(x, y);
    }
    return z;
}
// 0x858624: float 1.0 (0x3F800000), compresion de la suspension en IS_CAR_IN_WATER
constexpr float EXE_ONE = std::bit_cast<float>(0x3F800000u);
// 0x858B8C: float 0.5 (0x3F000000), altura sobre el suelo del pickup de dinero
constexpr float EXE_MONEY_PICKUP_Z_OFFSET = std::bit_cast<float>(0x3F000000u);
// 0x497423FE (inmediato): 999999.88f, distancia maxima de CPathFind::FindNodeClosestToCoors
constexpr float EXE_MAX_NODE_DIST = std::bit_cast<float>(0x497423FEu);
// Misiones que el EXE conserva al dar una orden de conduccion (0x39 y 0x3A)
inline bool KeepsCarMission(eCarMission mission) {
    return mission == MISSION_PLANE_CRASH_AND_BURN || mission == MISSION_HELI_CRASH_AND_BURN;
}
// Cola comun de las ordenes de conduccion (706, 723): estado fisico, CVehicle::SetEngineOn(true) (0x41BDD0: con el motor roto lo apaga), velocidad de crucero al menos 1
// y hora de inicio de la mision (CTimer::m_snTimeInMilliseconds, 0xB7CB84).
inline void StartScriptedCarMission(CVehicle* vehicle) {
    vehicle->SetStatus(STATUS_PHYSICS); // byte +0x36: (& 7) | 0x18
    vehicle->vehicleFlags.bEngineOn = !vehicle->vehicleFlags.bEngineBroken;
    if (vehicle->m_autoPilot.m_nCruiseSpeed <= 1) {
        vehicle->m_autoPilot.m_nCruiseSpeed = 1;
    }
    vehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
}
// Comprobacion "en pantalla" que el EXE escribe en linea (0x47FDBC..0x47FE87): radio de la esfera del modelo (ms_modelInfoPtrs[(int16)modelo]->m_pColModel->m_boundSphere.m_fRadius),
// centro con CEntity::GetBoundCentre (0x534250) y CCamera::IsSphereVisible(centro, radio) (0x420D40), en ese orden.
inline bool IsEntityOnScreenExe(CEntity* entity) {
    const float radius = CModelInfo::GetModelInfo(static_cast<int16>(entity->m_nModelIndex))->GetColModel()->GetBoundRadius();
    const CVector centre = entity->GetBoundCentre();
    return TheCamera.IsSphereVisible(centre, radius);
}
// strncpy(destino, origen, n) del CRT del EXE (0x821F40): copia hasta n caracteres, para en el NUL y rellena con ceros hasta n.
inline void CopyTextLabel(char* dest, const char* src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
}
} // namespace

static_assert(offsetof(CAutomobile, m_wheelColPoint) == 0x724 && offsetof(CColPoint, m_nSurfaceTypeB) == 0x23);
static_assert(offsetof(CAutomobile, m_fWheelsSuspensionCompression) == 0x7D4);
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390);
static_assert(offsetof(CAutoPilot, m_nTimeToStartMission) == 0x1C && offsetof(CAutoPilot, m_nCarMission) == 0x2A);
static_assert(offsetof(CAutoPilot, m_nCruiseSpeed) == 0x40 && offsetof(CAutoPilot, m_vecDestinationCoors) == 0x5C);
static_assert(offsetof(CVehicle, m_nLastWeaponDamageType) == 0x508);
static_assert(offsetof(CPlayerInfo, m_nTotalNumCollectables) == 0xC4 && sizeof(CPlayerInfo) == 0x190);

// 0x47FA30
OpcodeResult CRunningScript::ProcessCommands700To799(int32 command) {
    switch (command) {
    case COMMAND_IS_CAR_IN_WATER: { // 703, 0x47FA67: vehiculo NULL -> falso; sumergido (bit 8 de las banderas fisicas) o Vortex con la rueda 0 comprimida (< 1.0, NaN no) sobre agua poco profunda
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        bool inWater = false;
        if (vehicle) {
            if (vehicle->physicalFlags.bSubmergedInWater) {
                inWater = true;
            } else if (vehicle->m_nModelIndex == MODEL_VORTEX) {
                CAutomobile* const automobile = static_cast<CAutomobile*>(vehicle);
                if (automobile->m_fWheelsSuspensionCompression[0] < EXE_ONE // `fcomp; test ah,5; jp`: solo con "menor" ordenado
                    && g_surfaceInfos.IsShallowWater(static_cast<SurfaceId>(automobile->m_wheelColPoint[0].m_nSurfaceTypeB))) { // 0x55E730, byte +0x747
                    inWater = true;
                }
            }
        }
        UpdateCompareFlag(inWater);
        return OR_CONTINUE;
    }
    case COMMAND_GET_CLOSEST_CAR_NODE: { // 705, 0x47FBDE: sin nodo devuelve (0, 0, 0) y bandera falsa
        CollectParameters(3);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = ScriptGroundZ(x, y, P(2).fParam);
        const CNodeAddress node = ThePaths.FindNodeClosestToCoors(CVector{ x, y, z }, PATH_TYPE_VEH, EXE_MAX_NODE_DIST, 0, 1, 0, 0, 0); // 0x44F460
        bool found = false;
        const CVector nodePos = ThePaths.FindNodeCoorsForScript(node, &found); // 0x4505E0
        if (found) { // 0x47FB98
            P(0).fParam = nodePos.x;
            P(1).fParam = nodePos.y;
            P(2).fParam = nodePos.z;
        } else { // 0x47FC87
            P(0).iParam = 0;
            P(1).iParam = 0;
            P(2).iParam = 0;
        }
        StoreParameters(3);
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }
    case COMMAND_CAR_GOTO_COORDINATES_ACCURATE: { // 706, 0x47FCA7
        CollectParameters(4);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const float x = P(1).fParam;
        const float y = P(2).fParam;
        const float z = ScriptGroundZ(x, y, P(3).fParam);
        const CVector dest{ x, y, vehicle->GetDistanceFromCentreOfMassToBaseOfModel() + z }; // 0x536BE0, `fadd`
        const bool joined = CCarCtrl::JoinCarWithRoadSystemGotoCoors(vehicle, dest, false, false); // 0x42F870
        auto& mission = vehicle->m_autoPilot.m_nCarMission; // byte +0x3BA
        if (!KeepsCarMission(mission)) {
            mission = joined ? MISSION_GOTOCOORDINATES_STRAIGHTLINE_ACCURATE : MISSION_GOTOCOORDINATES_ACCURATE; // 0xD con exito, 0xC sin el
        }
        StartScriptedCarMission(vehicle);
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_ON_SCREEN: { // 714, 0x47FDA1
        CollectParameters(1);
        UpdateCompareFlag(IsEntityOnScreenExe(VehicleOf(P(0).iParam)));
        return OR_CONTINUE;
    }
    case COMMAND_IS_OBJECT_ON_SCREEN: { // 716, 0x47FE34
        CollectParameters(1);
        UpdateCompareFlag(IsEntityOnScreenExe(ObjectOf(P(0).iParam)));
        return OR_CONTINUE;
    }
    case COMMAND_IS_SCRIPT_FIRE_EXTINGUISHED: { // 720, 0x47FF83: indice fuera de [0, 60) -> falso
        CollectParameters(1);
        const int32 fire = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_FIRE); // 0x4839A0
        bool extinguished = false;
        if (fire >= 0 && fire < 60) {
            extinguished = gFireManager.IsScriptFireExtinguished(static_cast<int16>(fire)); // 0x5396E0
        }
        UpdateCompareFlag(extinguished);
        return OR_CONTINUE;
    }
    case COMMAND_REMOVE_SCRIPT_FIRE: { // 721, 0x47FFC3: indice fuera de [0, 60) -> 0x480EF1 (nada)
        CollectParameters(1);
        const int32 fire = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_FIRE);
        if (fire >= 0 && fire < 60) {
            gFireManager.RemoveScriptFire(static_cast<int16>(fire)); // 0x539700
        }
        return OR_CONTINUE;
    }
    case COMMAND_BOAT_GOTO_COORDS: { // 723, 0x480008
        CollectParameters(4);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const float x = P(1).fParam;
        const float y = P(2).fParam;
        const float z = P(3).fParam;
        // [esp+0x54]: el EXE lo pone a 0 en el prologo y solo CWaterLevel::GetWaterLevel (0x6EB690) lo escribe, y solo con z <= -100.0. Con otra z el destino queda a z = 0 (z del script ignorada).
        float waterLevel = 0.0f;
        if (z <= EXE_MIN_GROUND_Z) {
            (void)CWaterLevel::GetWaterLevel(x, y, z, waterLevel, true, nullptr);
        }
        auto& mission = vehicle->m_autoPilot.m_nCarMission;
        if (!KeepsCarMission(mission)) {
            mission = MISSION_GOTOCOORDINATES_ASTHECROWSWIMS; // 0xE
        }
        vehicle->m_autoPilot.m_vecDestinationCoors = CVector{ x, y, waterLevel }; // +0x3EC
        StartScriptedCarMission(vehicle);
        return OR_CONTINUE;
    }
    case COMMAND_BOAT_STOP: { // 724, 0x4800E5: sin comprobar el vehiculo
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        auto& mission = vehicle->m_autoPilot.m_nCarMission;
        if (!KeepsCarMission(mission)) {
            mission = MISSION_NONE;
        }
        vehicle->SetStatus(STATUS_PHYSICS);
        vehicle->vehicleFlags.bEngineOn = false; // byte +0x428 & 0xEF
        vehicle->m_autoPilot.m_nCruiseSpeed = 0;
        return OR_CONTINUE;
    }
    case COMMAND_SET_BOAT_CRUISE_SPEED: { // 731, 0x480288: _ftol (0x821B40, truncado a 64 bits) y se guarda el byte bajo
        CollectParameters(2);
        const uint8 speed = static_cast<uint8>(static_cast<int64>(P(1).fParam));
        VehicleOf(P(0).iParam)->m_autoPilot.m_nCruiseSpeed = speed;
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_MONEY_PICKUP: { // 737, 0x48056A
        CollectParameters(5);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y) + EXE_MONEY_PICKUP_Z_OFFSET; // `fadd [0x858B8C]`
        }
        (void)CPickups::GetActualPickupIndex(tPickupReference{ CollectNextParameterWithoutIncreasingPC() }); // 0x464250 + 0x4552A0 (el resultado no se usa)
        const ePickupType type = P(4).iParam != 0 ? PICKUP_MONEY_DOESNTDISAPPEAR : PICKUP_MONEY; // 0x13 u 8
        P(0).iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, static_cast<uint32>(ModelIndices::MI_MONEY.get_underlying()), type, P(3).uParam, 0, 0, nullptr); // 0x456F20, MI_MONEY (0x8CD59C)
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_LOAD_CUTSCENE: { // 740, 0x4806FB
        char name[8 + 1]{}; // el EXE usa 8 bytes de pila; el byte extra solo garantiza el NUL final
        ReadTextLabelFromScript(name, 8);
        CCutsceneMgr::LoadCutsceneData(name); // 0x4D5E80
        return OR_CONTINUE;
    }
    case COMMAND_START_CUTSCENE: // 743, 0x48072B
        CCutsceneMgr::StartCutscene(); // 0x5B1460
        return OR_CONTINUE;
    case COMMAND_GET_CUTSCENE_TIME: // 744, 0x48073F: solo EAX
        P(0).iParam = static_cast<int32>(CCutsceneMgr::GetCutsceneTimeInMilleseconds()); // 0x5B0550
        StoreParameters(1);
        return OR_CONTINUE;
    case COMMAND_HAS_CUTSCENE_FINISHED: // 745, 0x480761
        UpdateCompareFlag(CCutsceneMgr::HasCutsceneFinished()); // 0x5B0570
        return OR_CONTINUE;
    case COMMAND_CLEAR_CUTSCENE: // 746, 0x48078A
        CCutsceneMgr::DeleteCutsceneData(); // 0x4D5ED0
        return OR_CONTINUE;
    case COMMAND_SET_COLLECTABLE1_TOTAL: { // 749, 0x4807CF: Players[PlayerInFocus] (0xB7CD98 + 0x190 * byte 0xB7CD74) + 0xC4
        CollectParameters(1);
        CWorld::Players[CWorld::PlayerInFocus].m_nTotalNumCollectables = P(0).uParam;
        return OR_CONTINUE;
    }
    case COMMAND_IS_PROJECTILE_IN_AREA: { // 750, 0x4807FF
        CollectParameters(6);
        float x1 = P(0).fParam;
        float y1 = P(1).fParam;
        float z1 = P(2).fParam;
        float x2 = P(3).fParam;
        float y2 = P(4).fParam;
        float z2 = P(5).fParam;
        // `fcomp; test ah,41h; jne`: solo se intercambian con "mayor" ordenado (con NaN no)
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        UpdateCompareFlag(CProjectileInfo::IsProjectileInRange(x1, x2, y1, y2, z1, z2, false)); // 0x739860
        if (CTheScripts::DbgFlag) { // 0x859CF8
            // 0x4808FC: llamada a 0x486AE0 (x1, y1, z1, x2, y2, z2), rutina vacia en la version de venta (ScriptDebugBoxStubExe en TheScripts.cpp): sin efecto.
        }
        return OR_CONTINUE;
    }
    case COMMAND_GET_CAR_FORWARD_X: { // 760, 0x4809B2: x / sqrt(y*y + x*x) en la x87 con un solo redondeo al guardar
        CollectParameters(1);
        const CVector& forward = VehicleOf(P(0).iParam)->GetForward(); // m_matrix (+0x14) sin comprobar, +0x10/+0x14
        P(0).fParam = forward.x / std::sqrt(forward.y * forward.y + forward.x * forward.x);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_GET_CAR_FORWARD_Y: { // 761, 0x480A04: y / sqrt(y*y + x*x)
        CollectParameters(1);
        const CVector& forward = VehicleOf(P(0).iParam)->GetForward();
        P(0).fParam = forward.y / std::sqrt(forward.y * forward.y + forward.x * forward.x);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_CHANGE_GARAGE_TYPE: { // 762, 0x480A57: el nombre se lee antes que el parametro; con garaje < 0 (int16) no hace nada
        char name[8 + 1]{};
        ReadTextLabelFromScript(name, 8);
        const int16 garage = CGarages::GetGarageNumberByName(name); // 0x447680
        CollectParameters(1);
        if (garage >= 0) {
            CGarages::ChangeGarageType(garage, static_cast<eGarageType>(P(0).u8Param), 0); // 0x4476D0
        }
        return OR_CONTINUE;
    }
    case COMMAND_SET_PROGRESS_TOTAL: { // 781, 0x480D28: `fild` del entero
        CollectParameters(1);
        CStats::SetStatValue(static_cast<eStats>(1), static_cast<float>(P(0).iParam)); // 0x55A070
        return OR_CONTINUE;
    }
    case COMMAND_REGISTER_MISSION_PASSED: { // 792, 0x480D75
        char name[8 + 1]{};
        ReadTextLabelFromScript(name, 8);
        (void)TheText.Get(name); // 0x6A0050 (el resultado no se usa)
        CopyTextLabel(CStats::LastMissionPassedName, name, 8); // strncpy 0x821F40 a 0xB78A00
        CStats::IncrementStat(static_cast<eStats>(0x93), 1.0f); // 0x55C180
        CStats::IncrementStat(static_cast<eStats>(0xB1), CStats::GetStatValue(static_cast<eStats>(0xB0))); // 0x558E40: el EXE reutiliza la ranura de pila del argumento
        CStats::SetStatValue(static_cast<eStats>(0xB0), 0.0f);
        CTheScripts::LastMissionPassedTime = CTimer::m_snTimeInMilliseconds; // 0xA476A8
        return OR_CONTINUE;
    }
    case COMMAND_HAS_CAR_BEEN_DAMAGED_BY_WEAPON: { // 798, 0x480E8A: vehiculo NULL -> falso; ultimo arma (+0x508, con signo) contra el parametro
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        bool damaged = false;
        if (vehicle) {
            const int32 weapon = P(1).iParam;
            const int32 lastDamage = static_cast<int8>(vehicle->m_nLastWeaponDamageType); // movsx
            if (weapon == WEAPON_ANYMELEE || weapon == WEAPON_ANYWEAPON) { // 0x38, 0x39
                damaged = CDarkel::CheckDamagedWeaponType(static_cast<eWeaponType>(lastDamage), static_cast<eWeaponType>(weapon)); // 0x43D9E0
            } else {
                damaged = lastDamage == weapon;
            }
        }
        UpdateCompareFlag(damaged);
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_CUTSCENE_OBJECT: // 741
    case COMMAND_SET_CUTSCENE_ANIM:      // 742
    case COMMAND_LOAD_SPECIAL_MODEL:     // 755
    case COMMAND_SET_CHAR_RUNNING:       // 793
        return OR_CONTINUE; // 0x480EF1: `xor al, al; ret 4` sin leer parametros
    default: // 0x480F00 (`or al, 0xFF`) o comando con manejador propio
        return ProcessHandledCommand(command);
    }
}
