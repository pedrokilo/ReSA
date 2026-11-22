#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Stats.h"
#include "Pad.h"
#include "PlayerInfo.h"
#include "Camera.h"
#include "Credits.h"
#include "Localisation.h"
#include "Audio/AudioEngine.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"

// Lote SCRB de codigo vivo (rama vivo-scrb, sin validar): 0x489500 (3616 bytes), CRunningScript::ProcessCommands1000To1099, el despachador original de los comandos de script 1000..1099 (ranura 10 de la
// tabla 0x8A6168; ProcessOneCommand lo llama cuando el comando no tiene manejador propio). Thiscall con el numero de comando en la pila (`ret 4`); AL = 0 (OR_CONTINUE) o 0xFF (OR_INTERRUPT). Salta por una tabla
// indirecta (indices en 0x48A2B4, saltos en 0x48A204; indice = comando - 1000): los comandos nop (1000 1033 1035 1046 1071 1086-1088) terminan en 0x48A1E2 con 0 y los no implementados (50 comandos) en 0x48A1E6 con -1.
// Como en los despachadores P6B, solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original); los 15 reescritos y los nop se
// resuelven antes en ProcessOneCommand, de modo que el resto cae en `default` y devuelve OR_INTERRUPT. Prologo SEH (marco con manejador 0x838478): solo protege las tareas locales de los casos con tarea
// (los de comandos reescritos), que aqui no estan. Cada caso sigue el desensamblado: misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas.
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef) y 0x4048E0 (CPool<CVehicle>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
// 0x859F60: float 3.3333333 (0x40555555), pies por metro... en realidad 10/3: el EXE multiplica los metros por esta constante
constexpr float EXE_METRES_TO_FEET = std::bit_cast<float>(0x40555555u);
// 0x457A0000: 4000.0f (radio de ExtinguishAllCarFiresInArea alrededor del jugador)
constexpr float EXE_EXTINGUISH_RADIUS = 4000.0f;

// Comparacion ordenada de x87: intercambia solo cuando a > b (con NaN no intercambia).
inline void SwapIfGreater(float& a, float& b) {
    if (a > b) {
        std::swap(a, b);
    }
}
} // namespace

static_assert(offsetof(CVehicle, m_nPrimaryColor) == 0x434 && offsetof(CVehicle, m_nSecondaryColor) == 0x435);
static_assert(offsetof(CVehicle, m_apPassengers) == 0x464 && offsetof(CVehicle, m_nMaxPassengers) == 0x488);
static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590);
static_assert(offsetof(CAutomobile, autoFlags) == 0x868 && offsetof(CAutomobile, m_fCarTraction) == 0x8A0);
static_assert(offsetof(CPlayerInfo, m_bFreeHealthCare) == 0x152 && sizeof(CPlayerInfo) == 0x190);
static_assert(offsetof(CEntity, m_nModelIndex) == 0x22);
static_assert(offsetof(CEntity, m_nFlags) == 0x1C);
static_assert(offsetof(CCamera, m_bIgnoreFadingStuffForMusic) == 0x25);

// 0x489500
OpcodeResult CRunningScript::ProcessCommands1000To1099(int32 command) {
    switch (command) {
    // 0x48954A: SET_UPSIDEDOWN_CAR_NOT_DAMAGED
    case 1005: {
        CollectParameters(2);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        automobile->autoFlags.bDoesNotGetDamagedUpsideDown = (P(1).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x48963E: GET_CAR_COLOURS
    case 1011: {
        CollectParameters(1);
        const auto* const vehicle = VehicleOf(P(0).iParam);
        P(0).iParam = vehicle->m_nPrimaryColor;
        P(1).iParam = vehicle->m_nSecondaryColor;
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x489683: SET_ALL_CARS_CAN_BE_DAMAGED
    case 1012: {
        CollectParameters(1);
        if (P(0).iParam) {
            CWorld::SetAllCarsCanBeDamaged(true);
            return OR_CONTINUE;
        }
        CWorld::SetAllCarsCanBeDamaged(false);
        CWorld::ExtinguishAllCarFiresInArea(FindPlayerCoors(-1), EXE_EXTINGUISH_RADIUS);
        return OR_CONTINUE;
    }
    // 0x4896E8: SET_CAR_CAN_BE_DAMAGED (al activarlo no apaga el fuego; al desactivarlo lo apaga antes de limpiar el bit)
    case 1013: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        if (P(1).iParam) {
            vehicle->vehicleFlags.bCanBeDamaged = true;
            return OR_CONTINUE;
        }
        vehicle->ExtinguishCarFire();
        vehicle->vehicleFlags.bCanBeDamaged = false;
        return OR_CONTINUE;
    }
    // 0x489731: SET_DRUNK_INPUT_DELAY
    case 1021: {
        CollectParameters(2);
        CPad::GetPad(P(0).iParam)->SetDrunkInputDelay(P(1).iParam);
        return OR_CONTINUE;
    }
    // 0x48979F: GET_OFFSET_FROM_OBJECT_IN_WORLD_COORDS (la matriz del objeto se usa sin comprobar si existe)
    case 1024: {
        CollectParameters(4);
        auto* const object = CPools::GetObject(P(0).iParam);
        const CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector result;
        Multiply3x3Exe(&result, object->m_matrix, &offset);
        result += object->GetPosition();
        P(0).fParam = result.x;
        P(1).fParam = result.y;
        P(2).fParam = result.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x489831: GET_OFFSET_FROM_CAR_IN_WORLD_COORDS (igual que el del objeto, con el pool de vehiculos)
    case 1031: {
        CollectParameters(4);
        auto* const vehicle = VehicleOf(P(0).iParam);
        const CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector result;
        Multiply3x3Exe(&result, vehicle->m_matrix, &offset);
        result += vehicle->GetPosition();
        P(0).fParam = result.x;
        P(1).fParam = result.y;
        P(2).fParam = result.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x4898B4: IS_GERMAN_GAME
    case 1036: {
        UpdateCompareFlag(CLocalisation::GermanGame());
        return OR_CONTINUE;
    }
    // 0x4898CB: CLEAR_MISSION_AUDIO (la ranura es un byte: dec dl)
    case 1037: {
        CollectParameters(1);
        AudioEngine.ClearMissionAudio(static_cast<uint8>(static_cast<uint8>(P(0).iParam) - 1));
        return OR_CONTINUE;
    }
    // 0x4898F0: SET_FREE_HEALTH_CARE
    case 1044: {
        CollectParameters(2);
        CWorld::Players[P(0).iParam].m_bFreeHealthCare = (P(1).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x489AAE: SET_OBJECT_DRAW_LAST
    case 1048: {
        CollectParameters(2);
        auto* const object = CPools::GetObject(P(0).iParam);
        object->m_bDrawLast = (P(1).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x489B53: SET_NEAR_CLIP
    case 1053: {
        CollectParameters(1);
        TheCamera.SetNearClipScript(P(0).fParam);
        return OR_CONTINUE;
    }
    // 0x489B74: SET_RADIO_CHANNEL (-1 no hace nada; 11 pasa a 12; se sintoniza canal + 1)
    case 1054: {
        CollectParameters(1);
        int32 channel = P(0).iParam;
        if (channel == -1) {
            return OR_CONTINUE;
        }
        if (channel == 0xB) {
            channel = 0xC;
            P(0).iParam = channel;
        }
        AudioEngine.RetuneRadio(static_cast<eRadioID>(static_cast<int8>(channel + 1)));
        return OR_CONTINUE;
    }
    // 0x489BAE: SET_CAR_TRACTION (los coches normales (tipo 0) usan m_fCarTraction; el resto el campo 0x794)
    case 1059: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        const float traction = P(1).fParam;
        if (vehicle->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE) {
            static_cast<CAutomobile*>(vehicle)->m_fCarTraction = traction;
        } else {
            *reinterpret_cast<float*>(reinterpret_cast<uint8*>(vehicle) + 0x794) = traction;
        }
        return OR_CONTINUE;
    }
    // 0x489BF3: ARE_MEASUREMENTS_IN_METRES
    case 1060: {
        UpdateCompareFlag(CLocalisation::Metric());
        return OR_CONTINUE;
    }
    // 0x489C1A: CONVERT_METRES_TO_FEET
    case 1061: {
        CollectParameters(1);
        P(0).fParam = P(0).fParam * EXE_METRES_TO_FEET;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x489C48: SET_CAR_AVOID_LEVEL_TRANSITIONS (bit 2 del byte 0x3DB)
    case 1064: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        auto& flags = *(reinterpret_cast<uint8*>(vehicle) + 0x3DB);
        if (P(1).iParam) {
            flags |= 0x04;
        } else {
            flags &= 0xFB;
        }
        return OR_CONTINUE;
    }
    // 0x489C92: CLEAR_AREA_OF_CHARS (ordena cada par de coordenadas con la comparacion ordenada de x87)
    case 1067: {
        CollectParameters(6);
        float x1 = P(0).fParam, y1 = P(1).fParam, z1 = P(2).fParam;
        float x2 = P(3).fParam, y2 = P(4).fParam, z2 = P(5).fParam;
        SwapIfGreater(x1, x2);
        SwapIfGreater(y1, y2);
        SwapIfGreater(z1, z2);
        CWorld::ClearPedsFromArea(x1, y1, z1, x2, y2, z2);
        return OR_CONTINUE;
    }
    // 0x489D5B: SET_TOTAL_NUMBER_OF_MISSIONS (estadistica 0x94 = entero convertido con fild)
    case 1068: {
        CollectParameters(1);
        CStats::SetStatValue(static_cast<eStats>(0x94), static_cast<float>(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x489D82: CONVERT_METRES_TO_FEET_INT (fild + fmul + _ftol: el producto se redondea una sola vez)
    case 1069: {
        CollectParameters(1);
        const float product = static_cast<float>(static_cast<double>(P(0).iParam) * static_cast<double>(EXE_METRES_TO_FEET));
        P(0).iParam = static_cast<int32>(product);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x489DB4: REGISTER_FASTEST_TIME
    case 1070: {
        CollectParameters(2);
        CStats::RegisterFastestTime(static_cast<eStats>(P(0).iParam), P(1).iParam);
        return OR_CONTINUE;
    }
    // 0x489E73: IS_CAR_PASSENGER_SEAT_FREE (el indice con signo se compara con el maximo de pasajeros; sin limite inferior, como el EXE)
    case 1073: {
        CollectParameters(2);
        auto* const vehicle = VehicleOf(P(0).iParam);
        const int32 seat = P(1).iParam;
        bool free = false;
        if (seat < static_cast<int32>(vehicle->m_nMaxPassengers)) {
            free = (*(reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(vehicle) + 0x464) + seat) == nullptr);
        }
        UpdateCompareFlag(free);
        return OR_CONTINUE;
    }
    // 0x489F56: START_CREDITS
    case 1076: {
        CCredits::Start();
        return OR_CONTINUE;
    }
    // 0x489F62: STOP_CREDITS
    case 1077: {
        CCredits::Stop();
        return OR_CONTINUE;
    }
    // 0x489F6E: ARE_CREDITS_FINISHED
    case 1078: {
        UpdateCompareFlag(!CCredits::bCreditsGoing);
        return OR_CONTINUE;
    }
    // 0x489F94: SET_MUSIC_DOES_FADE (byte 0xB6F04D de la camara)
    case 1084: {
        CollectParameters(1);
        TheCamera.m_bIgnoreFadingStuffForMusic = (P(0).iParam == 0);
        return OR_CONTINUE;
    }
    // 0x489FB3: GET_CAR_MODEL (movsx de 16 bits)
    case 1089: {
        CollectParameters(1);
        const auto* const vehicle = VehicleOf(P(0).iParam);
        P(0).iParam = static_cast<int16>(vehicle->m_nModelIndex);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    default:
        return OR_INTERRUPT;
    }
}
