#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Entity/Vehicle/Automobile.h"
#include "Restart.h"
#include "Darkel.h"
#include "Explosion.h"
#include "Pickups.h"
#include "Garages.h"
#include "Gangs.h"
#include "Streaming.h"
#include "CutsceneMgr.h"
#include "Coronas.h"
#include "Text/Text.h"
#include "Animation/AnimManager.h"
#include "Crt/CrtCtype.h"

// Despachador original de los comandos de script 500..599 (ranura 5 de s_OriginalCommandHandlerTable, 0x47E090), thiscall con el numero de comando en la pila (`ret 4`) y AL = 0 (OR_CONTINUE) o 0xFF
// (OR_INTERRUPT: comando sin caso, destino 0x47F235). Solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o
// no_implementado con caso en la tabla de saltos 0x47F24C / tabla de indices 0x47F304); el resto va a ProcessHandledCommand. El EXE envuelve el despachador en un marco SEH (manejador 0x838353) que solo
// protege los `new` de tareas de los casos ya reescritos; no hace falta en C++. Cada caso sigue el desensamblado (misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas) y las
// comparaciones x87 conservan su sentido con NaN.
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x404910 (CPool<CPed>::GetAtRef): sin comprobar el resultado salvo donde lo hace el EXE.
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
// Indice de modelo de script: los negativos son entradas de CTheScripts::UsedObjectArray (0xA44B70, +0x18 = nModelIndex).
inline int32 ScriptModel(int32 model) {
    if (model < 0) {
        model = CTheScripts::UsedObjectArray.data()[-model].nModelIndex;
    }
    return model;
}
// 0x859014: -100.0f. Con z <= -100.0 (ordenado; NaN no) el EXE pide la altura del suelo (CWorld::FindGroundZForCoord 0x569660).
constexpr float EXE_MIN_GROUND_Z = -100.0f;
// 0x858C24: 0.3f (IS_CAR_UPSIDEDOWN) y 0x858B50: 0.0f (IS_CAR_UPRIGHT), comparados con la z del vector up de la matriz.
constexpr float EXE_UPSIDEDOWN_UP_Z = std::bit_cast<float>(0x3E99999Au);
constexpr float EXE_UPRIGHT_UP_Z    = 0.0f;
// 0x858B8C: 0.5f, altura sobre el suelo de los pickups creados con z <= -100.
constexpr float EXE_PICKUP_GROUND_OFFSET = 0.5f;
// 0x859A90: -3000.0f y 0x859F1C: 12001.0f (identificador de corona de DRAW_CORONA); 0x858C54: 4294967296.0f (correccion de la conversion sin signo).
constexpr float  EXE_CORONA_ID_ORIGIN = std::bit_cast<float>(0xC53B8000u);
constexpr float  EXE_CORONA_ID_SCALE  = std::bit_cast<float>(0x463B8400u);
constexpr double EXE_TWO_POW_32       = 4294967296.0;
// Primer indice de modelo de los personajes especiales (SPECIAL01 = 290 = 0x122).
constexpr int32 EXE_SPECIAL_CHAR_MODEL_BASE = 0x122;
// Banderas de CStreaming::RequestSpecialChar (0xC = STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED).
constexpr int32 EXE_SPECIAL_CHAR_STREAMING_FLAGS = 0xC;

// _ftol (0x821B40): truncamiento a 64 bits; el EXE usa solo EAX (los 32 bits bajos).
inline int32 ExeFtolLow(float value) {
    return static_cast<int32>(static_cast<uint32>(static_cast<uint64>(static_cast<int64>(value))));
}
} // namespace

static_assert(offsetof(CRunningScript, m_szName) == 0x8 && offsetof(CRunningScript, m_IP) == 0x14);
static_assert(offsetof(CVehicle, m_nDoorLock) == 0x4F8 && offsetof(CVehicle, m_fHealth) == 0x4C0);
static_assert(offsetof(CVehicle, m_nPrimaryColor) == 0x434 && offsetof(CVehicle, m_nSecondaryColor) == 0x435);
static_assert(offsetof(CPed, m_nAnimGroup) == 0x4D4);

// 0x47E090 (sin validar): despachador de los comandos 500..598 (indice = comando - 500, tabla de bytes 0x47F304 y de saltos 0x47F24C). Los comandos 540 y 561 comparten destino con 592 (0x47F21F,
// `xor al, al` sin leer parametros); los demas sin caso (y todo lo que cae fuera de 500..598) devuelven -1 (0x47F235).
OpcodeResult CRunningScript::ProcessCommands500To599(int32 command) {
    switch (command) {
    case COMMAND_IS_CAR_UPSIDEDOWN: { // 500, 0x47E0D1: `fcomp 0.3; test ah,41h; jnp` -> cierto con up.z <= 0.3 (NaN no)
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const CVector up = vehicle->m_matrix->GetUp(); // matriz (+0x14), vector up (+0x20), sin comprobar el puntero
        UpdateCompareFlag(up.z <= EXE_UPSIDEDOWN_UP_Z);
        return OR_CONTINUE;
    }
    case COMMAND_CANCEL_OVERRIDE_RESTART: // 502, 0x47E19B
        CRestart::CancelOverrideRestart(); // 0x460800
        return OR_CONTINUE;
    case COMMAND_START_KILL_FRENZY: { // 505, 0x47E263: la etiqueta se lee antes que los parametros; "DUMMY" (0x859EE0) = sin mensaje
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const GxtChar* text = nullptr;
        if (ExeCrt::Stricmp(label, "DUMMY") != 0) { // 0x8229B6
            text = TheText.Get(label); // 0x6A0050
        }
        CollectParameters(8);
        CDarkel::StartFrenzy( // 0x43D3B0
            static_cast<eWeaponType>(P(0).iParam), P(1).iParam, P(2).u16Param, P(3).iParam, text,
            P(4).iParam, P(5).iParam, P(6).iParam, P(7).iParam != 0, false);
        return OR_CONTINUE;
    }
    case COMMAND_READ_KILL_FRENZY_STATUS: { // 506, 0x47E304: sin parametros de entrada; el estado se amplia sin signo (movzx)
        P(0).iParam = static_cast<int32>(static_cast<uint16>(CDarkel::ReadStatus())); // 0x43D1E0
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SQRT: { // 507, 0x47E331: fld + fsqrt + fstp (redondeo a 24 bits)
        CollectParameters(1);
        P(0).fParam = std::sqrt(P(0).fParam);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_LOCK_CAR_DOORS: { // 522, 0x47E40A: con el vehiculo NULL no hace nada
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        if (vehicle) {
            vehicle->m_nDoorLock = static_cast<eCarLock>(P(1).iParam); // +0x4F8, 32 bits
        }
        return OR_CONTINUE;
    }
    case COMMAND_EXPLODE_CAR: { // 523, 0x47E44F: ranura +0xA4 de la vtable
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        vehicle->BlowUpCar(nullptr, false);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_EXPLOSION: { // 524, 0x47E48D
        CollectParameters(4);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        CExplosion::AddExplosion(nullptr, nullptr, static_cast<eExplosionType>(P(3).iParam), pos, 0, 1, -1.0f, 0); // 0x736A50
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_UPRIGHT: { // 525, 0x47E500: `fcomp 0.0; test ah,1; je` -> cierto salvo con up.z < 0.0 o NaN (C0 a uno)
        CollectParameters(1);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        const CVector up = vehicle->m_matrix->GetUp();
        UpdateCompareFlag(up.z >= EXE_UPRIGHT_UP_Z);
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_PICKUP: { // 531, 0x47E57A
        CollectParameters(5);
        const int32 model = ScriptModel(P(0).iParam);
        float       z     = P(4).fParam;
        const float x     = P(2).fParam;
        const float y     = P(3).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y) + EXE_PICKUP_GROUND_OFFSET; // 0x569660
        }
        static_cast<void>(CPickups::GetActualPickupIndex(tPickupReference{ CollectNextParameterWithoutIncreasingPC() })); // 0x464250 + 0x4552A0 (el resultado no se usa)
        P(0).iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, static_cast<uint32>(model), static_cast<ePickupType>(P(1).u8Param), 0, 0, 0, nullptr); // 0x456F20
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_HAS_PICKUP_BEEN_COLLECTED: { // 532, 0x47E662
        CollectParameters(1);
        UpdateCompareFlag(CPickups::IsPickUpPickedUp(tPickupReference{ P(0).iParam })); // 0x454B40
        return OR_CONTINUE;
    }
    case COMMAND_REMOVE_PICKUP: { // 533, 0x47E6AB
        CollectParameters(1);
        CPickups::RemovePickUp(tPickupReference{ P(0).iParam }); // 0x4573D0
        return OR_CONTINUE;
    }
    case COMMAND_SET_TAXI_LIGHTS: { // 534, 0x47E6D9: sin comprobar el vehiculo ni su tipo
        CollectParameters(2);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        automobile->SetTaxiLight(P(1).iParam != 0); // 0x6A3740
        return OR_CONTINUE;
    }
    case COMMAND_SET_TARGET_CAR_FOR_MISSION_GARAGE: { // 539, 0x47E78F: la etiqueta se lee antes que el parametro; garaje de 16 bits con signo
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const int16 garage = CGarages::GetGarageNumberByName(label); // 0x447680
        CollectParameters(1);
        if (garage < 0) {
            return OR_CONTINUE;
        }
        if (P(0).iParam < 0) {
            CGarages::SetTargetCarForMissionGarage(garage, nullptr); // 0x447C40
        } else {
            CGarages::SetTargetCarForMissionGarage(garage, VehicleOf(P(0).iParam));
        }
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_HEALTH: { // 548, 0x47E9E0: parametro entero (fild)
        CollectParameters(2);
        const float health = static_cast<float>(P(1).iParam);
        VehicleOf(P(0).iParam)->m_fHealth = health;
        return OR_CONTINUE;
    }
    case COMMAND_GET_CAR_HEALTH: { // 551, 0x47EA74 (sigue en 0x47EA4B): _ftol 0x821B40
        CollectParameters(1);
        P(0).iParam = ExeFtolLow(VehicleOf(P(0).iParam)->m_fHealth);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_CHANGE_CAR_COLOUR: { // 553, 0x47EA97
        CollectParameters(3);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        vehicle->m_nPrimaryColor   = P(1).u8Param;
        vehicle->m_nSecondaryColor = P(2).u8Param;
        return OR_CONTINUE;
    }
    case COMMAND_SET_GANG_WEAPONS: { // 567, 0x47ED00: banda de 16 bits (movzx)
        CollectParameters(4);
        CGangs::SetGangWeapons(static_cast<int16>(P(0).u16Param), static_cast<eWeaponType>(P(1).iParam), static_cast<eWeaponType>(P(2).iParam), static_cast<eWeaponType>(P(3).iParam)); // 0x5DE550
        return OR_CONTINUE;
    }
    case COMMAND_LOAD_SPECIAL_CHARACTER: { // 572, 0x47EDA9: el indice (base 1) se guarda en ScriptParams[0] ya decrementado; la etiqueta pasa a minusculas (solo 'A'..'Z', los 8 bytes)
        CollectParameters(1);
        P(0).iParam = P(0).iParam - 1;
        char name[8];
        ReadTextLabelFromScript(name, 8);
        for (char& c : name) {
            const uint8 u = static_cast<uint8>(c);
            if (u >= 'A' && u <= 'Z') {
                c = static_cast<char>(u + 0x20);
            }
        }
        CStreaming::RequestSpecialChar(P(0).iParam, name, EXE_SPECIAL_CHAR_STREAMING_FLAGS); // 0x40B450
        CTheScripts::ScriptResourceManager.AddToResourceManager(P(0).iParam + EXE_SPECIAL_CHAR_MODEL_BASE, RESOURCE_TYPE_MODEL_OR_SPECIAL_CHAR, this); // 0x4704B0
        return OR_CONTINUE;
    }
    case COMMAND_HAS_SPECIAL_CHARACTER_LOADED: { // 573, 0x47EE2D
        CollectParameters(1);
        P(0).iParam = P(0).iParam - 1;
        UpdateCompareFlag(CStreaming::HasSpecialCharLoaded(P(0).iParam)); // 0x407F00
        return OR_CONTINUE;
    }
    case COMMAND_SET_CUTSCENE_OFFSET: { // 580, 0x47EEB8
        CollectParameters(3);
        const CVector offset{ P(0).fParam, P(1).fParam, P(2).fParam };
        CCutsceneMgr::SetCutsceneOffset(offset); // 0x47E070
        return OR_CONTINUE;
    }
    case COMMAND_SET_ANIM_GROUP_FOR_CHAR: { // 581, 0x47EF01: busqueda sin mayusculas (_stricmp) del nombre de grupo; sin coincidencia queda el numero de grupos
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        char name[16];
        ReadTextLabelFromScript(name, 16);
        uint16 group = 0;
        bool   found = false;
        while (static_cast<uint32>(group) < StaticRef<uint32>(0xB4EA28) && !found) { // contador de 16 bits (movzx) contra el entero de 0xB4EA28
            if (ExeCrt::Stricmp(name, CAnimManager::GetAnimGroupName(static_cast<AssocGroupId>(group))) == 0) { // 0x4D3A20 + 0x8229B6
                found = true;
            } else {
                group++;
            }
        }
        ped->m_nAnimGroup = static_cast<AssocGroupId>(group); // +0x4D4
        return OR_CONTINUE;
    }
    case COMMAND_DRAW_CORONA: { // 591, 0x47F08E
        CollectParameters(9);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y); // 0x569660
        }
        const CVector pos{ x, y, z };
        // Identificador: _ftol((y + 3000) * 12001) pasado a float sin signo (fild + 2^32 si es negativo), mas (x + 3000), _ftol de la suma (redondeada una vez a 24 bits), + m_IP + this.
        // fild es exacto; el valor sin signo solo se redondea a 24 bits en la correccion fadd 2^32 (negativos), por eso se suma en double y se redondea una vez a float.
        const int32 rowKey = ExeFtolLow((y - EXE_CORONA_ID_ORIGIN) * EXE_CORONA_ID_SCALE);
        double      rowValue = static_cast<double>(rowKey);
        if (rowKey < 0) {
            rowValue = static_cast<double>(static_cast<float>(rowValue + EXE_TWO_POW_32)); // fadd [0x858C54]
        }
        const float  keySum = static_cast<float>(rowValue + static_cast<double>(x - EXE_CORONA_ID_ORIGIN)); // faddp
        const uint32 id     = static_cast<uint32>(ExeFtolLow(keySum)) + reinterpret_cast<uint32>(m_IP) + reinterpret_cast<uint32>(this);
        CCoronas::RegisterCorona( // 0x6FC580
            id, nullptr, P(6).u8Param, P(7).u8Param, P(8).u8Param, 255, pos, P(3).fParam, 450.0f,
            static_cast<eCoronaType>(P(4).u8Param), static_cast<eCoronaFlareType>(P(5).u8Param), CORREFL_SIMPLE, LOSCHECK_OFF, TRAIL_OFF,
            0.0f, false, 1.5f, false, 15.0f, false, false);
        return OR_CONTINUE;
    }
    case COMMAND_IS_CAR_IN_MISSION_GARAGE:               // 540
    case COMMAND_SET_SCRIPT_POLICE_HELI_TO_CHASE_CHAR:   // 561
        return OR_CONTINUE; // 0x47F21F: `xor al, al; ret 4` sin leer parametros (destino compartido con 592, que tiene manejador propio)
    default: // 0x47F235 (o manejador propio)
        return ProcessHandledCommand(command);
    }
}
