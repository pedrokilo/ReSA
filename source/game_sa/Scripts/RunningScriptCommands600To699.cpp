#include "StdInc.h"

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Radar.h"
#include "Camera.h"
#include "Garages.h"
#include "Streaming.h"

// Despachador original de los comandos de script 600..699 (ranura 6 de s_OriginalCommandHandlerTable, 0x47F370), thiscall con el numero de comando en la pila (`ret 4`) y AL = 0 (OR_CONTINUE) o 0xFF
// (OR_INTERRUPT: comando sin caso, destino 0x47F9AD). Solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original o
// no_implementado con caso en la tabla de saltos 0x47F9BC / tabla de indices 0x47FA04); el resto va a ProcessHandledCommand. Cada caso sigue el desensamblado (misma secuencia de CollectParameters /
// lecturas de ScriptParams / llamadas) y las comparaciones x87 con -100.0 (0x859014) conservan su sentido con NaN.
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x4048E0 (CPool<CVehicle>::GetAtRef): sin comprobar el resultado, como el EXE.
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
// Primer indice de modelo de los personajes especiales (SPECIAL01 = 290 = 0x122).
constexpr int32 EXE_SPECIAL_CHAR_MODEL_BASE = 0x122;
} // namespace

static_assert(offsetof(CRunningScript, m_szName) == 0x8);
static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6 && offsetof(CRunningScript, m_IsExternal) == 0xC7 && offsetof(CRunningScript, m_ExternalType) == 0xC9);
static_assert(offsetof(CAutomobile, autoFlags) == 0x868);
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40);
static_assert(offsetof(CObject, m_nObjectType) == 0x13C);
static_assert(offsetof(CBaseModelInfo, m_nAlpha) == 0xC);

// 0x47F370 (sin validar): despachador de los comandos 659..697 (indice = comando - 659, tabla de bytes 0x47FA04 y de saltos 0x47F9BC). Los comandos 676 y 677 devuelven 0 sin leer parametros
// (0x47F9A1, destino compartido con la salida de varios casos); los demas sin caso (y todo lo que cae fuera de 659..697) devuelven -1 (0x47F9AD).
OpcodeResult CRunningScript::ProcessCommands600To699(int32 command) {
    switch (command) {
    case COMMAND_GET_CONTROLLER_MODE: { // 659, 0x47F399: sin parametros de entrada, siempre 0
        P(0).iParam = 0;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAN_RESPRAY_CAR: { // 660, 0x47F3B8: byte +0x868, bit 1 (a uno con el parametro 0); sin comprobar el vehiculo ni su tipo
        CollectParameters(2);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        automobile->autoFlags.bShouldNotChangeColour = P(1).iParam == 0;
        return OR_CONTINUE;
    }
    case COMMAND_UNLOAD_SPECIAL_CHARACTER: { // 662, 0x47F40D: el indice (base 1) se guarda en ScriptParams[0] ya decrementado
        CollectParameters(1);
        P(0).iParam = P(0).iParam - 1;
        if (CTheScripts::ScriptResourceManager.RemoveFromResourceManager(P(0).iParam + EXE_SPECIAL_CHAR_MODEL_BASE, RESOURCE_TYPE_MODEL_OR_SPECIAL_CHAR, this)) { // 0x470510
            CStreaming::SetMissionDoesntRequireSpecialChar(P(0).iParam); // 0x40B490
        }
        return OR_CONTINUE;
    }
    case COMMAND_ACTIVATE_GARAGE: { // 665, 0x47F4B6
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const int16 garage = CGarages::GetGarageNumberByName(label); // 0x447680 (movsx)
        if (garage >= 0) {
            CGarages::ActivateGarage(garage); // 0x447CD0
        }
        return OR_CONTINUE;
    }
    case COMMAND_CREATE_OBJECT_NO_OFFSET: { // 667, 0x47F4F1: como CREATE_OBJECT pero sin elevar la z con la distancia del centro de masa a la base
        CollectParameters(4);
        int32 model = P(0).iParam;
        if (model < 0) {
            model = CTheScripts::UsedObjectArray.data()[-model].nModelIndex;
        }
        CBaseModelInfo* const modelInfo = CModelInfo::ms_modelInfoPtrs[model];
        modelInfo->m_nAlpha = 255;
        CObject* const object = CObject::Create(model, false); // 0x5A1F60
        if (!m_IsExternal && m_ExternalType == -1) {
            object->m_nObjectType = OBJECT_MISSION;
        } else {
            object->m_nObjectType = OBJECT_MISSION2;
        }
        const float x = P(1).fParam;
        const float y = P(2).fParam;
        const float z = ScriptGroundZ(x, y, P(3).fParam);
        object->SetPosn(x, y, z);                // 0x420B80
        object->SetOrientation(0.0f, 0.0f, 0.0f); // 0x439A80
        object->UpdateRwMatrix();                // 0x446F90
        object->UpdateRwFrame();                 // 0x532B00
        if (modelInfo->AsLodAtomicModelInfoPtr()) { // ranura +0xC de la vtable del modelo
            object->SetupBigBuilding(); // 0x533150
        }
        CTheScripts::ClearSpaceForMissionEntity(CVector{ x, y, z }, object); // 0x486B00
        CWorld::Add(object); // 0x563220
        P(0).iParam = CPools::GetObjectRef(object); // 0x465070
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_OBJECT); // 0x4637E0
        }
        return OR_CONTINUE;
    }
    case COMMAND_SWITCH_WIDESCREEN: { // 675, 0x47F684
        CollectParameters(1);
        if (P(0).iParam != 0) {
            TheCamera.SetWideScreenOn(); // 0x50C140
        } else {
            TheCamera.SetWideScreenOff(); // 0x50C150
        }
        return OR_CONTINUE;
    }
    case COMMAND_ADD_SPRITE_BLIP_FOR_CONTACT_POINT: { // 679, 0x47F6BD
        CollectParameters(4);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = ScriptGroundZ(x, y, P(2).fParam);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC()); // 0x464250 + 0x582870 (el resultado no se usa)
        const auto blip = CRadar::SetCoordBlip(BLIP_CONTACT_POINT, CVector{ x, y, z }, static_cast<eBlipColour>(2), BLIP_DISPLAY_BOTH, m_szName); // 0x583820
        CRadar::SetBlipSprite(blip, static_cast<eRadarSprite>(P(3).iParam)); // 0x583D70
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_ADD_SPRITE_BLIP_FOR_COORD: { // 680, 0x47F75D
        CollectParameters(4);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        const float z = ScriptGroundZ(x, y, P(2).fParam);
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC()); // 0x464250 + 0x582870 (el resultado no se usa)
        const auto blip = CRadar::SetCoordBlip(BLIP_COORD, CVector{ x, y, z }, static_cast<eBlipColour>(5), BLIP_DISPLAY_BOTH, m_szName); // 0x583820
        CRadar::SetBlipSprite(blip, static_cast<eRadarSprite>(P(3).iParam)); // 0x583D70
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    case COMMAND_SET_CAR_ONLY_DAMAGED_BY_PLAYER: { // 682, 0x47F85E (sigue en 0x47F83C): banderas fisicas +0x40, bit 22 (0x400000)
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        vehicle->physicalFlags.bInvulnerable = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    case COMMAND_DEACTIVATE_GARAGE: { // 697, 0x47F976
        char label[8];
        ReadTextLabelFromScript(label, 8);
        const int16 garage = CGarages::GetGarageNumberByName(label); // 0x447680 (movsx)
        if (garage >= 0) {
            CGarages::DeActivateGarage(garage); // 0x447CB0
        }
        return OR_CONTINUE;
    }
    case COMMAND_ADD_SPRITE_BLIP_FOR_CAR:  // 676
    case COMMAND_ADD_SPRITE_BLIP_FOR_CHAR: // 677
        return OR_CONTINUE; // 0x47F9A1: `xor al, al; ret 4` sin leer parametros
    default: // 0x47F9AD (o manejador propio)
        return ProcessHandledCommand(command);
    }
}
