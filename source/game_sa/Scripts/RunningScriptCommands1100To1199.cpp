#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Camera.h"
#include "Pad.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "Pickups.h"
#include "Pickup.h"
#include "DamageManager.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Entity/Ped/Ped.h"

// Lote SCRB de codigo vivo (sin validar): 0x48A320 (4720 bytes), CRunningScript::ProcessCommands1100To1199, despachador original de los comandos de script 1100..1199
// (ranura 11 de la tabla 0x8A6168; thiscall con el numero de comando en la pila, `ret 4`; resultado en AL: 0 = OR_CONTINUE, 0xFF = OR_INTERRUPT para los comandos sin caso). La tabla de saltos
// (indice de bytes 0x48B4D8, tabla 0x48B418) empieza en el comando 1101 (comando - 0x44D); el 1100 y todo lo que cae fuera del rango va al caso por defecto (0x48B406: devuelve -1).
// Como en las ranuras 300 y 400 (P6B), solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado `original`): ProcessOneCommand solo
// llega a este despachador con esos comandos. Los comandos con manejador propio (reescrito, nop) no llevan caso; los no implementados (0x48B406) devuelven -1 por el caso por defecto.
// Cada caso sigue el desensamblado (misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas) y las operaciones x87 se hacen en float, en el mismo orden.
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef), 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x465040 (CPool<CObject>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CObject* ObjectOf(int32 handle) {
    return CPools::GetObject(handle);
}
// Acceso por desplazamiento a campos de CVehicle/CAutomobile/CBike que el EXE lee o escribe y que no tienen miembro con nombre utilizable aqui.
template<typename T>
inline T& RawAt(void* base, size_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(base) + offset);
}
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x858628: 100.0 (SET_INTERPOLATION_PARAMETERS)
constexpr float EXE_ONE_HUNDRED = std::bit_cast<float>(0x42C80000u);
// 0x858B8C: 0.5 (radio de IS_ANY_PICKUP_AT_COORDS y sumando de la altura del pickup de proteccion)
constexpr float EXE_HALF = std::bit_cast<float>(0x3F000000u);
// 0x859F80: 250.0 (la salud del coche por debajo de la cual IS_CAR_ON_FIRE responde que si)
constexpr float EXE_BURNING_HEALTH = std::bit_cast<float>(0x437A0000u);
// 0x859014: -100.0f. Con z <= -100.0 (ordenado; NaN no) el EXE pide la altura del suelo.
constexpr float EXE_MIN_GROUND_Z = -100.0f;
} // namespace

static_assert(offsetof(CVehicle, m_pDriver) == 0x460);
static_assert(offsetof(CVehicle, m_pFire) == 0x490);
static_assert(offsetof(CVehicle, m_fHealth) == 0x4C0);
static_assert(offsetof(CVehicle, m_nLastWeaponDamageType) == 0x508);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nTempAction) == 0x3BB);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nTempActionTime) == 0x3BC);
static_assert(offsetof(CAutomobile, m_damageManager) == 0x5A0);

// 0x48A320
OpcodeResult CRunningScript::ProcessCommands1100To1199(int32 command) {
    switch (command) {
    // 0x48A350: SET_OBJECT_ROTATION
    case 1107: {
        CollectParameters(4);
        auto* const object = ObjectOf(P(0).iParam);
        CWorld::Remove(object);
        const float rz = P(3).fParam * EXE_DEG_TO_RAD;
        const float ry = P(2).fParam * EXE_DEG_TO_RAD;
        const float rx = P(1).fParam * EXE_DEG_TO_RAD;
        object->SetOrientation(rx, ry, rz);
        object->UpdateRwMatrix();
        object->UpdateRwFrame();
        CWorld::Add(object);
        return OR_CONTINUE;
    }
    // 0x48A723: SET_INTERPOLATION_PARAMETERS (100.0 - P0 se calcula con el entero de P0 reinterpretado como float: fsub dword ptr)
    case 1120: {
        CollectParameters(2);
        const float first = P(0).fParam;
        const float second = EXE_ONE_HUNDRED - first;
        TheCamera.SetParametersForScriptInterpolation(first, second, P(1).uParam);
        return OR_CONTINUE;
    }
    // 0x48A8A8: SET_CAR_STAY_IN_FAST_LANE (bit 3 del byte 0x3DB del coche)
    case 1126: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        uint8& flags = RawAt<uint8>(vehicle, 0x3DB);
        if (P(1).iParam) {
            flags |= 0x08;
        } else {
            flags &= 0xF7;
        }
        return OR_CONTINUE;
    }
    // 0x48A93B: CLEAR_CAR_LAST_WEAPON_DAMAGE
    case 1128: {
        CollectParameters(1);
        auto* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle) {
            vehicle->m_nLastWeaponDamageType = 0xFF;
        }
        return OR_CONTINUE;
    }
    // 0x48A974: GET_DRIVER_OF_CAR (-1 sin conductor)
    case 1132: {
        CollectParameters(1);
        auto* const vehicle = VehicleOf(P(0).iParam);
        auto* const driver = vehicle->m_pDriver;
        if (driver) {
            P(0).iParam = CPools::GetPedRef(driver);
        } else {
            P(0).iParam = -1;
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48A9E3: GET_NUMBER_OF_FOLLOWERS (solo si el ped es lider de su grupo)
    case 1133: {
        CollectParameters(1);
        auto* const ped = PedOf(P(0).iParam);
        uint8 count = 0;
        if (auto* const group = CPedGroups::GetPedsGroup(ped)) {
            auto& membership = group->GetMembership();
            if (membership.IsLeader(ped)) {
                count = (uint8)membership.CountMembersExcludingLeader();
            }
        }
        P(0).iParam = count;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48AB19 (compartido con 1137..1142): LOCATE_CHAR_IN_CAR_OBJECT_2D (1139) y LOCATE_CHAR_IN_CAR_OBJECT_3D (1142)
    case 1139:
    case 1142: {
        LocateCharObjectCommand(command);
        return OR_CONTINUE;
    }
    // 0x48AB30: SET_CAR_TEMP_ACTION (accion, y hora actual del juego (0xB7CB84) + duracion)
    case 1143: {
        CollectParameters(3);
        auto* const vehicle = VehicleOf(P(0).iParam);
        RawAt<uint8>(vehicle, 0x3BB) = P(1).u8Param;
        RawAt<uint32>(vehicle, 0x3BC) = CTimer::GetTimeInMS() + P(2).uParam;
        return OR_CONTINUE;
    }
    // 0x48AC3E: GET_REMOTE_CONTROLLED_CAR (-1 si no hay)
    case 1156: {
        CollectParameters(1);
        auto* const remote = CWorld::Players[P(0).iParam].m_pRemoteVehicle;
        if (remote) {
            P(0).iParam = CPools::GetVehicleRef(remote);
        } else {
            P(0).iParam = -1;
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48AC7B: IS_PC_VERSION
    case 1157: {
        UpdateCompareFlag(true);
        return OR_CONTINUE;
    }
    // 0x48AD30: SET_ENABLE_RC_DETONATE (0xC1CC00 = detonacion deshabilitada)
    case 1162: {
        CollectParameters(1);
        CVehicle::bDisableRemoteDetonation = (P(0).iParam == 0);
        return OR_CONTINUE;
    }
    // 0x48AD58: SET_CAR_RANDOM_ROUTE_SEED (word en el desplazamiento 0x45E del coche)
    case 1163: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        RawAt<uint16>(vehicle, 0x45E) = (uint16)P(1).iParam;
        return OR_CONTINUE;
    }
    // 0x48AD8F: IS_ANY_PICKUP_AT_COORDS (pickups activos a menos de 0.5 del punto)
    case 1164: {
        CollectParameters(3);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = P(2).fParam;
        bool found = false;
        for (auto& pickup : CPickups::aPickUps) {
            if (found) {
                break;
            }
            if (pickup.m_nPickupType == PICKUP_NONE) {
                continue;
            }
            const CVector pos = pickup.GetPosn();
            const float dx = pos.x - x;
            const float dy = pos.y - y;
            const float dz = pos.z - z;
            if (CVector{ dx, dy, dz }.Magnitude() < EXE_HALF) {
                found = true;
            }
        }
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }
    // 0x48AEDC: GET_POSITION_OF_ANALOGUE_STICKS (primero lee el pad, luego escribe los cuatro parametros)
    case 1172: {
        CollectParameters(1);
        auto* const pad = CPad::GetPad(P(0).iParam);
        P(0).iParam = pad->NewState.LeftStickX;
        P(1).iParam = pad->NewState.LeftStickY;
        P(2).iParam = pad->NewState.RightStickX;
        P(3).iParam = pad->NewState.RightStickY;
        StoreParameters(4);
        return OR_CONTINUE;
    }
    // 0x48AF33: IS_CAR_ON_FIRE
    case 1173: {
        CollectParameters(1);
        auto* const vehicle = VehicleOf(P(0).iParam);
        bool result = false;
        if (vehicle->m_pFire) {
            result = true;
        }
        if (vehicle->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE) {
            if (static_cast<CAutomobile*>(vehicle)->m_damageManager.GetEngineStatus() >= 0xE1) {
                result = true;
            }
        }
        if (vehicle->m_fHealth < EXE_BURNING_HEALTH) {
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x48AFBB: IS_CAR_TYRE_BURST (moto: tipo 9, estados de rueda en 0x65C/0x65D; resto: CDamageManager)
    case 1174: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        bool result = false;
        if (vehicle->m_nVehicleType == VEHICLE_TYPE_BIKE) {
            const int32 tyre = P(1).iParam;
            if (tyre == 4) {
                if (RawAt<uint8>(vehicle, 0x65C) == 1) {
                    result = true;
                }
                if (RawAt<uint8>(vehicle, 0x65D) == 1) {
                    result = true;
                }
            } else {
                int32 idx = tyre;
                if (tyre == 2) {
                    idx = 0;
                    P(1).iParam = idx;
                } else if (tyre == 3) {
                    idx = 1;
                    P(1).iParam = idx;
                }
                if (RawAt<uint8>(vehicle, 0x65C + (size_t)idx) == 1) {
                    result = true;
                }
            }
        } else {
            auto& damageManager = static_cast<CAutomobile*>(vehicle)->m_damageManager;
            const int32 tyre = P(1).iParam;
            if (tyre == 4) {
                for (uint32 i = 0; i < 4; i++) {
                    if ((uint32)damageManager.GetWheelStatus((eCarWheel)i) == 1) {
                        result = true;
                    }
                }
            } else if ((uint32)damageManager.GetWheelStatus((eCarWheel)tyre) == 1) {
                result = true;
            }
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x48B070: HELI_GOTO_COORDS (los dos ultimos parametros llegan como flotantes: altitud minima y maxima)
    case 1186: {
        CollectParameters(6);
        auto* const vehicle = VehicleOf(P(0).iParam);
        static_cast<CAutomobile*>(vehicle)->TellHeliToGoToCoors(P(1).fParam, P(2).fParam, P(3).fParam, P(4).fParam, P(5).fParam);
        return OR_CONTINUE;
    }
    // 0x48B0DC: IS_INT_VAR_EQUAL_TO_CONSTANT (variable global; el EXE lee la variable antes de recoger la constante)
    case 1187: {
        auto* const var = GetPointerToScriptVariable(VAR_GLOBAL);
        CollectParameters(1);
        UpdateCompareFlag(var->iParam == P(0).iParam);
        return OR_CONTINUE;
    }
    // 0x48B100: IS_INT_LVAR_EQUAL_TO_CONSTANT (variable local)
    case 1188: {
        auto* const var = GetPointerToScriptVariable(VAR_LOCAL);
        CollectParameters(1);
        UpdateCompareFlag(var->iParam == P(0).iParam);
        return OR_CONTINUE;
    }
    // 0x48B19D: CREATE_PROTECTION_PICKUP (el pickup se crea medio metro por encima del suelo si z <= -100)
    case 1190: {
        CollectParameters(5);
        const float x = P(0).fParam;
        float z = P(2).fParam;
        const float y = P(1).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y) + EXE_HALF;
        }
        (void)CPickups::GetActualPickupIndex(CollectNextParameterWithoutIncreasingPC());
        const auto model = static_cast<uint16>(ModelIndices::MI_PICKUP_REVENUE);
        P(0).iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, model, static_cast<ePickupType>(0x10), P(3).uParam, P(4).uParam, 0, nullptr);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    default:
        return OR_INTERRUPT;
    }
}
