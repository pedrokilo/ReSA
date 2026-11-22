#include "StdInc.h"

#include <bit>
#include <cstring>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "PedGroups.h"
#include "World.h"
#include "Streaming.h"
#include "StreamedScripts.h"
#include "TheZones.h"
#include "Zone.h"
#include "GangWars.h"
#include "Stats.h"
#include "PopCycle.h"
#include "Population.h"
#include "Shopping.h"
#include "CutsceneMgr.h"
#include "TagManager.h"
#include "PostEffects.h"
#include "GameLogic.h"
#include "Restart.h"
#include "MenuSystem.h"
#include "Radar.h"
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "3dMarkers.h"
#include "TimeCycle.h"
#include "Clothes.h"
#include "PedClothesDesc.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "DamageManager.h"
#include "Fx/Fx.h"
#include "Conversations/Conversations.h"
#include "PedIntelligence.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Models/PedModelInfo.h"
#include "Events/EventGroup.h"
#include "Events/EventAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseEffect.h"

// F4 de codigo vivo (sin validar): 0x474900 (6208 bytes hasta la tabla de saltos), CRunningScript::ProcessCommands2200To2299, el despachador original de los comandos de script 2200..2299 (ranura 22 de la tabla
// 0x8A6168; la usa ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con manejador 0x838269): solo protege
// objetos locales (CEventAttractor del comando 2208) y la reserva de la tarea; en C++ no hace falta.
// Salto con una tabla directa de 100 entradas (0x476140): 13 comandos van a 0x47611C (devuelven 0 sin leer parametros: 2201 2202 2205 2209 2218 2222 2224 2252-2255 2261) y los fuera de rango a
// 0x476120 (OR_INTERRUPT, -1; ninguna entrada de la tabla va ahi). Varias entradas comparten cuerpo: 2228-2233 (bit de variable activo), 2234-2236 y 2237-2239 (activar bit), 2240-2242 y
// 2243-2245 (borrar bit; global o local segun el grupo), 2275/2276 (ObjectInAngledAreaCheckCommand con el propio numero de comando), 2286/2287 (menu con numero(s)), 2297/2298 (comparar etiqueta de 16).
// Cada `case` replica el orden de colaboradores del desensamblado; los parametros se leen de ScriptParams (0xA43C78). Los indices y desplazamientos que vienen del script se usan sin comprobar rangos,
// como el EXE. Puentes por direccion (plugin::CallAndReturn): solo CAttractorScanner FindAttractorInRange (0x6002F0, funcion estatica sin declaracion publica) en el comando 2208.
namespace {
// 0x859520 y 0x859524: floats 1/640 (0x3ACCCCCD) y 1/448 (0x3B124925), escala de pantalla de CREATE_MENU
constexpr float EXE_INV_640 = std::bit_cast<float>(0x3ACCCCCDu);
constexpr float EXE_INV_448 = std::bit_cast<float>(0x3B124925u);
// 0x858628: float 100.0 (0x42C80000)
constexpr float EXE_K_100 = std::bit_cast<float>(0x42C80000u);
// 0x859EE0: texto "DUMMY" (etiqueta vacia de los comandos de menu)
constexpr const char* EXE_DUMMY_LABEL = "DUMMY";

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
// Bucle de copia del EXE (`mov cl,[src+i]; mov [dst+i],cl; inc i; test cl,cl; jne`): copia hasta el NUL inclusive, sin limite de destino.
inline void CopyUntilNul(char* dest, const char* src) {
    size_t i = 0;
    char   c;
    do {
        c       = src[i];
        dest[i] = c;
        i++;
    } while (c != '\0');
}
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
// _ftol del CRT del EXE (0x821B40): trunca a entero de 64 bits y el EXE usa la palabra baja.
inline int32 FtolExe(float value) {
    return static_cast<int32>(static_cast<int64>(value));
}
// 0x6C26F0: cdecl, indice como dword sin signo, -1 fuera de rango.
inline eCarNodes DoorNodeExe(uint32 door) {
    return door <= 5u ? CDamageManager::GetCarNodeIndexFromDoor(static_cast<eDoors>(door)) : static_cast<eCarNodes>(-1);
}

// 0x474A04 (GET_RANDOM_CHAR_IN_SPHERE_ONLY_DRUGS_BUYERS) y 0x4759CB (GET_RANDOM_CHAR_IN_SPHERE_NO_BRAIN): mismo recorrido de la reserva de peatones (del ultimo hueco al primero; el EXE usa el
// tamano de la reserva y `dec` antes de cada GetAt) con filtros distintos. La mejor distancia se guarda como entero (_ftol, empieza en 9999) y se relee con fild; solo entra un peaton cuya
// distancia sea estrictamente menor que el radio (comparacion ordenada: con NaN no entra) y que la mejor. El elegido pasa a peaton de mision (PED_MISSION), suma ms_nTotalMissionPeds y, si el
// script usa limpieza de mision, entra en CMissionCleanup (tipo 2, ped). Devuelve el manejador del elegido o -1.
int32 FindRandomCharInSphereExe(CRunningScript& script, const CVector& center, float radius, bool noBrainVariant) {
    int32 bestHandle = -1;
    int32 bestDist   = 0x270F;
    auto* const pool = GetPedPool();
    for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
        --i;
        CPed* const ped = pool->GetAt(i);
        if (!ped) {
            continue;
        }
        if (ped->GetCreatedBy() != PED_GAME) { // cmp byte [ped+0x484], 1
            continue;
        }
        if (ped->m_bRemoveFromWorld) { // test ah, 8 sobre las banderas +0x1C (bit 11)
            continue;
        }
        if (ped->bFadeOut) { // test byte [ped+0x470], 8
            continue;
        }
        if (script.IsPedDead(ped)) {
            continue;
        }
        if (!noBrainVariant) {
            // el modelo (+0x22, con signo de 16 bits) debe tener el bit 0 de su byte +0x32 (CPedModelInfo::m_nPedFlags)
            CBaseModelInfo* const modelInfo = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(ped->m_nModelIndex)];
            if (!(RawField<uint8>(modelInfo, 0x32) & 1)) {
                continue;
            }
            if (CPedGroups::GetPedsGroup(ped)) {
                continue;
            }
        } else {
            if (CPedGroups::GetPedsGroup(ped)) {
                continue;
            }
            if (RawField<uint32>(ped, 0x474) & 0x800000) { // bHasAScriptBrain
                continue;
            }
        }
        const CVector& pos = ped->GetPosition();
        CVector        diff;
        diff.x = pos.x - center.x;
        diff.y = pos.y - center.y;
        diff.z = pos.z - center.z;
        const float dist = diff.Magnitude();
        if (!(dist < radius)) {
            continue;
        }
        if (!(static_cast<float>(bestDist) > dist)) {
            continue;
        }
        bestHandle = static_cast<int32>(pool->GetRef(ped));
        bestDist   = FtolExe(dist);
    }
    if (bestHandle >= 0) {
        PedOf(bestHandle)->SetCharCreatedBy(PED_MISSION);
        CPopulation::ms_nTotalMissionPeds++;
        if (script.m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(bestHandle, MISSION_CLEANUP_ENTITY_TYPE_PED);
        }
    }
    return bestHandle;
}
} // namespace

static_assert(offsetof(CPed, m_fMaxHealth) == 0x544);
static_assert(offsetof(CPed, m_pEnex) == 0x78C);

static_assert(offsetof(CObject, m_fScale) == 0x15C);
static_assert(offsetof(CPlane, m_minAltitude) == 0x9B0);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA);
static_assert(offsetof(CZone, m_ZoneInfoIndex) == 0x1C);
static_assert(offsetof(CEntryExit, m_nArea) == 0x32 && offsetof(CEntryExit, m_nSkyColor) == 0x33 && offsetof(CEntryExit, m_pLink) == 0x38);
static_assert(offsetof(CPlayerPedData, m_nPlayerGroup) == 0x38 && offsetof(CPlayerPedData, m_nScriptLimitToGangSize) == 0x43);
static_assert(offsetof(CPlayerInfo, m_PlayerData) == 0x4 && offsetof(CPlayerPedData, m_pPedClothesDesc) == 0x4);
static_assert(offsetof(CPedClothesDesc, m_anTextureKeys) == 0x28);

// 0x474900
OpcodeResult CRunningScript::ProcessCommands2200To2299(int32 command) {
    switch (command) {
    // 0x47493E
    case COMMAND_ENABLE_CRANE_CONTROLS: {
        CollectParameters(3);
        const bool raise   = P(0).uParam != 0; // 0xA44496
        const bool lower   = P(1).uParam != 0; // 0xA44495
        const bool release = P(2).uParam != 0; // 0xA44494
        CTheScripts::bEnableCraneRaise   = raise;
        CTheScripts::bEnableCraneLower   = lower;
        CTheScripts::bEnableCraneRelease = release;
        return OR_CONTINUE;
    }
    // 0x47497F
    case COMMAND_IS_PLAYER_IN_POSITION_FOR_CONVERSATION: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        const bool  res = CConversations::IsPlayerInPositionForConversation(ped, false);
        UpdateCompareFlag(res);
        return OR_CONTINUE;
    }
    // 0x4749BB
    case COMMAND_ENABLE_CONVERSATION: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        if (P(1).uParam != 0) {
            CConversations::EnableConversation(ped, true);
            return OR_CONTINUE;
        }
        CConversations::EnableConversation(ped, false);
        return OR_CONTINUE;
    }
    // 0x474A04
    case COMMAND_GET_RANDOM_CHAR_IN_SPHERE_ONLY_DRUGS_BUYERS: {
        CollectParameters(4);
        const CVector center{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   radius = P(3).fParam;
        P(0).iParam          = FindRandomCharInSphereExe(*this, center, radius, false);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x474BD4
    case COMMAND_GET_PED_TYPE: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        P(0).iParam     = static_cast<int32>(ped->m_nPedType);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x474C0A
    case COMMAND_TASK_USE_CLOSEST_MAP_ATTRACTOR: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        const float radius    = P(1).fParam;
        int32       model     = P(2).iParam;
        const float px        = P(3).fParam;
        const float py        = P(4).fParam;
        const float pz        = P(5).fParam;
        char        label[16]{};
        ReadTextLabelFromScript(label, 8);
        CVector pos;
        CPed*   ped = nullptr;
        if (pedHandle == -1) {
            pos = CVector{ px, py, pz };
        } else {
            ped = PedOf(pedHandle);
            pos = ped->GetPosition();
        }
        C2dEffect* effectInUse = nullptr;
        if (ped) {
            effectInUse = ped->GetIntelligence()->GetEffectInUse(); // 0x6018D0
        }
        if (model == 0) {
            model = -1;
        } else if (model < 0) {
            model = CTheScripts::UsedObjectArray[-model].nModelIndex;
        }
        C2dEffectPedAttractor* foundEffect = nullptr;
        CEntity*               foundEntity = nullptr;
        // 0x6002F0 (cdecl, nueve argumentos de pila): punto, radio, modelo, tipo de atractor 5, nombre, exigir hueco libre, efecto a excluir, salida efecto, salida entidad
        const bool found = plugin::CallAndReturn<bool, 0x6002F0, const CVector*, float, int32, int32, const char*, bool, C2dEffectPedAttractor*, C2dEffectPedAttractor**, CEntity**>(
            &pos, radius, model, 5, label, true, reinterpret_cast<C2dEffectPedAttractor*>(effectInUse), &foundEffect, &foundEntity
        );
        (void)found; // el EXE solo mira la salida del efecto
        if (!foundEffect) {
            UpdateCompareFlag(false);
            return OR_CONTINUE;
        }
        if (m_ExternalType != -1) {
            // script con cerebro: pasa a la lista de inactivos y entrega el atractor como evento (CEventAttractor con tarea 0xE9)
            RemoveScriptFromList(&CTheScripts::pActiveScripts);
            AddScriptToList(&CTheScripts::pIdleScripts);
            ShutdownThisScript();
            {
                CEventAttractor event(foundEffect, foundEntity, true);
                event.m_TaskId = static_cast<eTaskType>(0xE9);
                ped->GetEventGroup().Add(&event, true); // 0x4AB420
            }
            ped->GetIntelligence()->SetEffectInUse(reinterpret_cast<C2dEffect*>(foundEffect)); // 0x6018E0
            return OR_WAIT;
        }
        auto* const task = new CTaskComplexUseEffect(foundEffect, foundEntity);
        GivePedScriptedTask(pedHandle, task, command);
        UpdateCompareFlag(true);
        return OR_CONTINUE;
    }
    // 0x474E43
    case COMMAND_PLANE_ATTACK_PLAYER_USING_DOG_FIGHT: {
        CollectParameters(3);
        CVehicle* const veh      = VehicleOf(P(0).iParam);
        const float     altitude = P(2).fParam;
        const auto      mission  = static_cast<uint8>(veh->m_autoPilot.m_nCarMission);
        if (mission != 0x39 && mission != 0x3A) { // MISSION_PLANE_CRASH_AND_BURN / MISSION_HELI_CRASH_AND_BURN
            veh->m_autoPilot.m_nCarMission = static_cast<eCarMission>(0x3F); // MISSION_PLANE_DOG_FIGHT_PLAYER
        }
        static_cast<CPlane*>(veh)->m_minAltitude = altitude;
        return OR_CONTINUE;
    }
    // 0x474E88
    case COMMAND_CAN_TRIGGER_GANG_WAR_WHEN_ON_A_MISSION: {
        CollectParameters(1);
        CGangWars::bCanTriggerGangWarWhenOnAMission = P(0).uParam != 0;
        return OR_CONTINUE;
    }
    // 0x474EA8
    case COMMAND_CONTROL_MOVABLE_VEHICLE_PART: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        static_cast<CAutomobile*>(veh)->UpdateMovingCollision(P(1).fParam); // 0x6A1460
        return OR_CONTINUE;
    }
    // 0x474ED7
    case COMMAND_WINCH_CAN_PICK_VEHICLE_UP: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).uParam != 0) {
            RawField<uint8>(veh, 0x42D) |= 0x10; // bWinchCanPickMeUp
            return OR_CONTINUE;
        }
        RawField<uint8>(veh, 0x42D) &= 0xEF;
        return OR_CONTINUE;
    }
    // 0x474F22
    case COMMAND_OPEN_CAR_DOOR_A_BIT: {
        CollectParameters(3);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const uint32    door = P(1).uParam;
        const int32     node = static_cast<int32>(DoorNodeExe(door)); // 0x6C26F0
        if (veh->IsDoorMissing(static_cast<eDoors>(door))) { // vtable +0x98
            return OR_CONTINUE;
        }
        if (!RawField<RwFrame*>(veh, 0x648 + static_cast<size_t>(static_cast<ptrdiff_t>(node)) * 4)) { // CAutomobile::m_aCarNodes[node], sin comprobar rango
            return OR_CONTINUE;
        }
        veh->OpenDoor(nullptr, node, static_cast<eDoors>(door), P(2).fParam, true);
        return OR_CONTINUE;
    }
    // 0x474F8C
    case COMMAND_IS_CAR_DOOR_FULLY_OPEN: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const uint32    door = P(1).uParam;
        const int32     node = static_cast<int32>(DoorNodeExe(door)); // 0x6C26F0
        const bool      res  = veh->IsDoorFullyOpenU32(static_cast<uint32>(node)); // vtable +0x84 (version con el indice de nodo)
        UpdateCompareFlag(res);
        return OR_CONTINUE;
    }
    // 0x474FD9
    case COMMAND_SET_ALWAYS_DRAW_3D_MARKERS: {
        CollectParameters(1);
        if (P(0).uParam != 0) {
            C3dMarkers::ForceRender(true);
            return OR_CONTINUE;
        }
        C3dMarkers::ForceRender(false);
        return OR_CONTINUE;
    }
    // 0x47500D
    case COMMAND_STREAM_SCRIPT: {
        CollectParameters(1);
        P(0).iParam = static_cast<int32>(CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param))); // movsx
        CStreaming::RequestModel(P(0).iParam + 0x6676, 4);
        return OR_CONTINUE;
    }
    // 0x475049
    case COMMAND_HAS_STREAMED_SCRIPT_LOADED: {
        CollectParameters(1);
        P(0).iParam = static_cast<int32>(CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param))); // movsx
        // 0x964E08 = ms_aInfoForModel[indice + 0x6676] + 0x10 (estado de carga)
        UpdateCompareFlag(CStreaming::ms_aInfoForModel.data()[P(0).iParam + 0x6676].m_LoadState == eStreamingLoadState::LOADSTATE_LOADED);
        return OR_CONTINUE;
    }
    // 0x475085
    case COMMAND_SET_GANG_WARS_TRAINING_MISSION: {
        CollectParameters(1);
        CGangWars::bTrainingMission = P(0).uParam != 0;
        return OR_CONTINUE;
    }
    // 0x4750A5
    case COMMAND_SET_CHAR_HAS_USED_ENTRY_EXIT: {
        CollectParameters(4);
        CPed* const     ped = PedOf(P(0).iParam);
        const CVector2D pos{ P(1).fParam, P(2).fParam };
        const float     range = P(3).fParam;
        CEntryExit* const enex   = CEntryExitManager::GetInSlot(CEntryExitManager::FindNearestEntryExit(pos, range, -1)); // 0x43F4B0 + 0x43EF00
        CEntryExit*       target = enex;
        if (enex->m_pLink) {
            target         = enex->m_pLink;
            target->m_pLink = enex;
        }
        ped->SetAreaCode(static_cast<eAreaCodes>(target->m_nArea));
        ped->m_pEnex    = (target->m_nArea != 0) ? enex : nullptr;
        if (!ped->IsPlayer()) {
            return OR_CONTINUE;
        }
        CEntryExitManager::ms_entryExitStackPosn = 0;
        CEntryExitManager::AddEntryExitToStack(enex);
        const uint8 colour = target->m_nSkyColor;
        if (colour > 0) {
            CTimeCycle::StartExtraColour(static_cast<int32>(colour) - 1, false);
            return OR_CONTINUE;
        }
        CTimeCycle::StopExtraColour(false);
        return OR_CONTINUE;
    }
    // 0x475170
    case COMMAND_SET_CHAR_MAX_HEALTH: {
        CollectParameters(2);
        const float value = static_cast<float>(P(1).iParam); // fild
        CPed* const ped   = PedOf(P(0).iParam);
        ped->m_fMaxHealth = value;
        return OR_CONTINUE;
    }
    // 0x4751A5
    case COMMAND_SET_NIGHT_VISION: {
        CollectParameters(1);
        if (P(0).uParam != 0) {
            CPostEffects::ScriptNightVisionSwitch(true);
            return OR_CONTINUE;
        }
        CPostEffects::ScriptNightVisionSwitch(false);
        return OR_CONTINUE;
    }
    // 0x4751D9
    case COMMAND_SET_INFRARED_VISION: {
        CollectParameters(1);
        if (P(0).uParam != 0) {
            CPostEffects::ScriptInfraredVisionSwitch(true);
            return OR_CONTINUE;
        }
        CPostEffects::ScriptInfraredVisionSwitch(false);
        return OR_CONTINUE;
    }
    // 0x47520D
    case COMMAND_SET_ZONE_FOR_GANG_WARS_TRAINING: {
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        const int32 zoneIndex = CTheZones::FindZoneByLabel(label, static_cast<eZoneType>(2)); // movsx; -1 si no existe (el EXE no lo comprueba)
        CGangWars::ZoneInfoForTraining = static_cast<int32>(static_cast<uint16>(CTheZones::NavigationZoneArray.data()[zoneIndex].m_ZoneInfoIndex)); // movzx word
        return OR_CONTINUE;
    }
    // 0x475244: 2228-2233 IS_{GLOBAL,LOCAL}_VAR_BIT_SET_{CONST,VAR,LVAR}
    case COMMAND_IS_GLOBAL_VAR_BIT_SET_CONST:
    case COMMAND_IS_GLOBAL_VAR_BIT_SET_VAR:
    case COMMAND_IS_GLOBAL_VAR_BIT_SET_LVAR:
    case COMMAND_IS_LOCAL_VAR_BIT_SET_CONST:
    case COMMAND_IS_LOCAL_VAR_BIT_SET_VAR:
    case COMMAND_IS_LOCAL_VAR_BIT_SET_LVAR: {
        CollectParameters(2);
        const uint32 value = P(0).uParam;
        const uint32 bit   = 1u << (P(1).uParam & 31); // shl edx, cl
        UpdateCompareFlag((bit & value) != 0);
        return OR_CONTINUE;
    }
    // 0x475274: 2234-2236 SET_GLOBAL_VAR_BIT_*
    case COMMAND_SET_GLOBAL_VAR_BIT_CONST:
    case COMMAND_SET_GLOBAL_VAR_BIT_VAR:
    case COMMAND_SET_GLOBAL_VAR_BIT_LVAR: {
        tScriptParam* const var = GetPointerToScriptVariable(VAR_GLOBAL);
        CollectParameters(1);
        var->uParam |= 1u << (P(0).uParam & 31);
        return OR_CONTINUE;
    }
    // 0x4752A2: 2237-2239 SET_LOCAL_VAR_BIT_*
    case COMMAND_SET_LOCAL_VAR_BIT_CONST:
    case COMMAND_SET_LOCAL_VAR_BIT_VAR:
    case COMMAND_SET_LOCAL_VAR_BIT_LVAR: {
        tScriptParam* const var = GetPointerToScriptVariable(VAR_LOCAL);
        CollectParameters(1);
        var->uParam |= 1u << (P(0).uParam & 31);
        return OR_CONTINUE;
    }
    // 0x4752CC: 2240-2242 CLEAR_GLOBAL_VAR_BIT_*
    case COMMAND_CLEAR_GLOBAL_VAR_BIT_CONST:
    case COMMAND_CLEAR_GLOBAL_VAR_BIT_VAR:
    case COMMAND_CLEAR_GLOBAL_VAR_BIT_LVAR: {
        tScriptParam* const var = GetPointerToScriptVariable(VAR_GLOBAL);
        CollectParameters(1);
        var->uParam &= ~(1u << (P(0).uParam & 31));
        return OR_CONTINUE;
    }
    // 0x4752FC: 2243-2245 CLEAR_LOCAL_VAR_BIT_*
    case COMMAND_CLEAR_LOCAL_VAR_BIT_CONST:
    case COMMAND_CLEAR_LOCAL_VAR_BIT_VAR:
    case COMMAND_CLEAR_LOCAL_VAR_BIT_LVAR: {
        tScriptParam* const var = GetPointerToScriptVariable(VAR_LOCAL);
        CollectParameters(1);
        var->uParam &= ~(1u << (P(0).uParam & 31));
        return OR_CONTINUE;
    }
    // 0x47532C
    case COMMAND_SET_CHAR_CAN_BE_KNOCKED_OFF_BIKE: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        // dword +0x474: bits 27-28 = CantBeKnockedOffBike (2 bits) = bits 0-1 del parametro
        uint32& flags = RawField<uint32>(ped, 0x474);
        flags         = flags ^ (((P(1).uParam << 27) ^ flags) & 0x18000000u);
        return OR_CONTINUE;
    }
    // 0x47536F
    case COMMAND_SET_CHAR_COORDINATES_DONT_WARP_GANG: {
        CollectParameters(4);
        CPed* const ped = PedOf(P(0).iParam);
        SetCharCoordinates(*ped, CVector{ P(1).fParam, P(2).fParam, P(3).fParam }, false, true);
        return OR_CONTINUE;
    }
    // 0x4753B0
    case COMMAND_ADD_PRICE_MODIFIER: {
        CollectParameters(2);
        CShopping::AddPriceModifier(P(0).uParam, P(1).iParam);
        return OR_CONTINUE;
    }
    // 0x4753D6
    case COMMAND_REMOVE_PRICE_MODIFIER: {
        CollectParameters(1);
        CShopping::RemovePriceModifier(P(0).uParam);
        return OR_CONTINUE;
    }
    // 0x4753F4
    case COMMAND_INIT_ZONE_POPULATION_SETTINGS: {
        CTheZones::InitZonesPopulationSettings();
        return OR_CONTINUE;
    }
    // 0x475400
    case COMMAND_EXPLODE_CAR_IN_CUTSCENE_SHAKE_AND_BITS: {
        CollectParameters(4);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        RawField<uint8>(veh, 0x42A) |= 0x20; // bit 21 de las banderas del vehiculo
        const bool noCamShake        = P(1).uParam == 0;
        const bool noSpawnFlyingComp = P(2).uParam == 0;
        const bool explosionSound    = P(3).uParam != 0;
        veh->BlowUpCarCutSceneNoExtras(noCamShake, noSpawnFlyingComp, false, explosionSound); // vtable +0xA8
        return OR_CONTINUE;
    }
    // 0x475459
    case COMMAND_IS_SKIP_CUTSCENE_BUTTON_PRESSED: {
        const bool pressed = CCutsceneMgr::IsCutsceneSkipButtonBeingPressed(); // 0x4D5D10
        UpdateCompareFlag(pressed);
        return OR_CONTINUE;
    }
    // 0x47547A
    case COMMAND_GET_CUTSCENE_OFFSET: {
        const CVector offset = CCutsceneMgr::ms_cutsceneOffset;
        P(1).fParam          = offset.y;
        P(0).fParam          = offset.x;
        P(2).fParam          = offset.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x4754B8
    case COMMAND_SET_OBJECT_SCALE: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        obj->m_fScale      = P(1).fParam;
        return OR_CONTINUE;
    }
    // 0x4754E6
    case COMMAND_GET_CURRENT_POPULATION_ZONE_TYPE: {
        P(0).iParam = CPopCycle::m_nCurrentZoneType;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x475500
    case COMMAND_CREATE_MENU: {
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        char title[12]{};
        if (_stricmp(label, EXE_DUMMY_LABEL) != 0) {
            CopyUntilNul(title, label);
        }
        CollectParameters(7);
        // fild [0xC17044] (ancho de pantalla) * 1/640, fild [0xC17048] (alto) * 1/448; cada producto se redondea a float
        const float scaleX = static_cast<float>(StaticRef<int32>(0xC17044)) * EXE_INV_640;
        const float x      = P(0).fParam * scaleX;
        const float width  = scaleX * P(2).fParam;
        const float scaleY = static_cast<float>(StaticRef<int32>(0xC17048)) * EXE_INV_448;
        const float y      = scaleY * P(1).fParam;
        const auto  id     = CMenuSystem::CreateNewMenu(
            CMenuSystem::MENU_TYPE_DEFAULT,
            title,
            x,
            y,
            width,
            P(3).u8Param,
            P(4).uParam != 0,
            P(5).uParam != 0,
            static_cast<eFontAlignment>(P(6).iParam)
        );
        P(0).uParam = static_cast<uint32>(id); // movzx
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4755D0
    case COMMAND_SET_MENU_COLUMN_ORIENTATION: {
        CollectParameters(3);
        const uint8 id          = P(0).u8Param;
        const uint8 column      = P(1).u8Param;
        const uint8 orientation = P(2).u8Param;
        CMenuSystem::SetColumnOrientation(id, column, orientation);
        return OR_CONTINUE;
    }
    // 0x475602
    case COMMAND_GET_MENU_ITEM_SELECTED: {
        CollectParameters(1);
        const int8 res = CMenuSystem::CheckForSelected(static_cast<MenuId>(P(0).uParam));
        P(0).iParam    = static_cast<int32>(res); // movsx
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x475632
    case COMMAND_GET_MENU_ITEM_ACCEPTED: {
        CollectParameters(1);
        const int8 res = CMenuSystem::CheckForAccept(static_cast<MenuId>(P(0).uParam));
        P(0).iParam    = static_cast<int32>(res); // movsx
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x475661
    case COMMAND_ACTIVATE_MENU_ITEM: {
        CollectParameters(3);
        const uint8 id     = P(0).u8Param;
        const uint8 row    = P(1).u8Param;
        const uint8 enable = P(2).u8Param;
        CMenuSystem::ActivateOneItem(id, row, enable != 0);
        return OR_CONTINUE;
    }
    // 0x475693
    case COMMAND_DELETE_MENU: {
        CollectParameters(1);
        CMenuSystem::SwitchOffMenu(P(0).u8Param);
        return OR_CONTINUE;
    }
    // 0x4756B4
    case COMMAND_SET_MENU_COLUMN: {
        CollectParameters(2);
        const uint32 panel  = P(0).uParam;
        const uint32 column = P(1).uParam;
        char         header[12]{};
        {
            char label[16]{};
            ReadTextLabelFromScript(label, 8);
            if (_stricmp(label, EXE_DUMMY_LABEL) != 0) {
                CopyUntilNul(header, label);
            }
        }
        char row0[12]{};
        {
            char label[16]{};
            ReadTextLabelFromScript(label, 8);
            if (_stricmp(label, EXE_DUMMY_LABEL) != 0) {
                CopyUntilNul(row0, label);
            }
        }
        char rows[11][10]{}; // filas 1..11 (10 bytes cada una)
        for (int32 i = 0; i < 11; i++) {
            char label[16]{};
            ReadTextLabelFromScript(label, 8);
            if (_stricmp(label, EXE_DUMMY_LABEL) != 0) {
                CopyUntilNul(rows[i], label);
            }
        }
        CMenuSystem::InsertMenu(
            static_cast<MenuId>(panel),
            static_cast<uint8>(column),
            header,
            row0,
            rows[0],
            rows[1],
            rows[2],
            rows[3],
            rows[4],
            rows[5],
            rows[6],
            rows[7],
            rows[8],
            rows[9],
            rows[10]
        );
        return OR_CONTINUE;
    }
    // 0x475838
    case COMMAND_SET_BLIP_ENTRY_EXIT: {
        CollectParameters(4);
        const uint32    blip = P(0).uParam;
        const CVector2D pos{ P(1).fParam, P(2).fParam };
        const float     range = P(3).fParam;
        CEntryExit* const enex = CEntryExitManager::GetInSlot(CEntryExitManager::FindNearestEntryExit(pos, range, 0));
        CRadar::SetBlipEntryExit(blip, enex);
        return OR_CONTINUE;
    }
    // 0x475891
    case COMMAND_SWITCH_DEATH_PENALTIES: {
        CollectParameters(1);
        CGameLogic::bPenaltyForDeathApplies = P(0).uParam != 0;
        return OR_CONTINUE;
    }
    // 0x4758B1
    case COMMAND_SWITCH_ARREST_PENALTIES: {
        CollectParameters(1);
        CGameLogic::bPenaltyForArrestApplies = P(0).uParam != 0;
        return OR_CONTINUE;
    }
    // 0x4758D0
    case COMMAND_SET_EXTRA_HOSPITAL_RESTART_POINT: {
        CollectParameters(5);
        CRestart::ExtraHospitalRestartCoors.x = P(0).fParam;
        CRestart::ExtraHospitalRestartCoors.y = P(1).fParam;
        CRestart::ExtraHospitalRestartCoors.z = P(2).fParam;
        CRestart::ExtraHospitalRestartRadius  = P(3).fParam;
        CRestart::ExtraHospitalRestartHeading = P(4).fParam;
        return OR_CONTINUE;
    }
    // 0x475926
    case COMMAND_SET_EXTRA_POLICE_STATION_RESTART_POINT: {
        CollectParameters(5);
        CRestart::ExtraPoliceStationRestartCoors.x = P(0).fParam;
        CRestart::ExtraPoliceStationRestartCoors.y = P(1).fParam;
        CRestart::ExtraPoliceStationRestartCoors.z = P(2).fParam;
        CRestart::ExtraPoliceStationRestartRadius  = P(3).fParam;
        CRestart::ExtraPoliceStationRestartHeading = P(4).fParam;
        return OR_CONTINUE;
    }
    // 0x47597A
    case COMMAND_FIND_NUMBER_TAGS_TAGGED: {
        P(0).iParam = CTagManager::ms_numTagged;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x475996
    case COMMAND_GET_TERRITORY_UNDER_CONTROL_PERCENTAGE: {
        const float percentage = CGangWars::TerritoryUnderControlPercentage * EXE_K_100;
        P(0).iParam            = FtolExe(percentage);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4759BC: 2275 y 2276 (el EXE le pasa el propio numero de comando)
    case COMMAND_IS_OBJECT_IN_ANGLED_AREA_2D:
    case COMMAND_IS_OBJECT_IN_ANGLED_AREA_3D: {
        ObjectInAngledAreaCheckCommand(command);
        return OR_CONTINUE;
    }
    // 0x4759CB
    case COMMAND_GET_RANDOM_CHAR_IN_SPHERE_NO_BRAIN: {
        CollectParameters(4);
        const CVector center{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   radius = P(3).fParam;
        P(0).iParam          = FindRandomCharInSphereExe(*this, center, radius, true);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x475B96
    case COMMAND_SET_PLANE_UNDERCARRIAGE_UP: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).uParam != 0) {
            static_cast<CPlane*>(veh)->SetGearUp(); // 0x6CAC20
            return OR_CONTINUE;
        }
        static_cast<CPlane*>(veh)->SetGearDown(); // 0x6CAC70
        return OR_CONTINUE;
    }
    // 0x475BD4
    case COMMAND_DISABLE_ALL_ENTRY_EXITS: {
        CollectParameters(1);
        CEntryExitManager::ms_bDisabled = static_cast<int8>(P(0).uParam != 0);
        return OR_CONTINUE;
    }
    // 0x475BF3
    case COMMAND_ATTACH_ANIMS_TO_MODEL: {
        CollectParameters(1);
        const int32 model = ScriptModelIndex(P(0).iParam);
        char        name[16]{};
        ReadTextLabelFromScript(name, 8);
        CTheScripts::ScriptAttachAnimGroupToCharModel(model, name);                    // 0x474800
        CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels(model, name);    // 0x474750
        return OR_CONTINUE;
    }
    // 0x475C40
    case COMMAND_SET_OBJECT_AS_STEALABLE: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).uParam != 0) {
            obj->m_nObjectFlags |= 0x2000u;
            return OR_CONTINUE;
        }
        obj->m_nObjectFlags &= 0xFFFFDFFFu;
        return OR_CONTINUE;
    }
    // 0x475C91
    case COMMAND_SET_CREATE_RANDOM_GANG_MEMBERS: {
        CollectParameters(1);
        CPopulation::m_bDontCreateRandomGangMembers = P(0).uParam == 0;
        return OR_CONTINUE;
    }
    // 0x475CB0
    case COMMAND_ADD_SPARKS: {
        CollectParameters(7);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const int32   count = P(6).iParam;
        CVector       dir{ P(3).fParam, P(4).fParam, P(5).fParam };
        const float   force = dir.NormaliseAndMag(); // 0x59C970
        g_fx.AddSparks(pos, dir, force, count, CVector{ 0.0f, 0.0f, 0.0f }, static_cast<eSparkType>(1), std::bit_cast<float>(0x3ECCCCCDu), std::bit_cast<float>(0x3F800000u)); // 0x49F040
        return OR_CONTINUE;
    }
    // 0x475D73
    case COMMAND_GET_VEHICLE_CLASS: {
        CollectParameters(1);
        CVehicle* const       veh       = VehicleOf(P(0).iParam);
        CBaseModelInfo* const modelInfo = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(veh->m_nModelIndex)];
        P(0).iParam                     = static_cast<int32>(RawField<int8>(modelInfo, 0x4D)); // movsx byte (CVehicleModelInfo::m_nVehicleClass)
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x475DB1
    case COMMAND_CLEAR_CONVERSATION_FOR_CHAR: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        if (!ped) {
            return OR_CONTINUE;
        }
        CConversations::RemoveConversationForPed(ped);
        return OR_CONTINUE;
    }
    // 0x475DE4: 2286 SET_MENU_ITEM_WITH_NUMBER y 2287 SET_MENU_ITEM_WITH_2_NUMBERS
    case COMMAND_SET_MENU_ITEM_WITH_NUMBER:
    case COMMAND_SET_MENU_ITEM_WITH_2_NUMBERS: {
        CollectParameters(3);
        const uint32 panel  = P(0).uParam;
        const uint32 column = P(1).uParam;
        const uint32 row    = P(2).uParam;
        char         label[16]{};
        ReadTextLabelFromScript(label, 8);
        char text[16]{};
        CopyUntilNul(text, label);
        int32 secondNumber;
        if (command == COMMAND_SET_MENU_ITEM_WITH_NUMBER) {
            CollectParameters(1);
            secondNumber = -1;
        } else if (command == COMMAND_SET_MENU_ITEM_WITH_2_NUMBERS) {
            CollectParameters(2);
            secondNumber = P(1).iParam;
        } else {
            return OR_CONTINUE;
        }
        CMenuSystem::InsertOneMenuItemWithNumber(static_cast<MenuId>(panel), static_cast<uint8>(column), static_cast<uint8>(row), text, P(0).iParam, secondNumber);
        return OR_CONTINUE;
    }
    // 0x475E76
    case COMMAND_APPEND_TO_NEXT_CUTSCENE: {
        char name1[16]{};
        ReadTextLabelFromScript(name1, 8);
        char name2[16]{};
        ReadTextLabelFromScript(name2, 8);
        CCutsceneMgr::AppendToNextCutscene(name1, name2); // 0x4D5DB0
        return OR_CONTINUE;
    }
    // 0x475EB7
    case COMMAND_GET_NAME_OF_INFO_ZONE: {
        CollectParameters(3);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        CZone* const  zone = CTheZones::FindSmallestZoneForPosition(pos, true); // 0x572360
        char* const   dest = reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        CopyTextLabel(dest, zone->m_InfoLabel, 8); // strncpy(destino, zona (+0: etiqueta de informacion), 8)
        return OR_CONTINUE;
    }
    // 0x475F0A
    case COMMAND_VEHICLE_CAN_BE_TARGETTED_BY_HS_MISSILE: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).uParam != 0) {
            RawField<uint8>(veh, 0x42D) |= 0x40; // bVehicleCanBeTargettedByHS
            return OR_CONTINUE;
        }
        RawField<uint8>(veh, 0x42D) &= 0xBF;
        return OR_CONTINUE;
    }
    // 0x475F55
    case COMMAND_SET_FREEBIES_IN_VEHICLE: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).uParam != 0) {
            RawField<uint8>(veh, 0x428) |= 0x80;
            return OR_CONTINUE;
        }
        RawField<uint8>(veh, 0x428) &= 0x7F;
        return OR_CONTINUE;
    }
    // 0x475FA0
    case COMMAND_SET_SCRIPT_LIMIT_TO_GANG_SIZE: {
        CollectParameters(1);
        FindPlayerPed(-1)->GetPlayerData()->m_nScriptLimitToGangSize = P(0).u8Param;
        const uint32 groupIndex = FindPlayerPed(-1)->GetPlayerData()->m_nPlayerGroup;
        CPlayerPedData* const data = FindPlayerPed(-1)->GetPlayerData();
        auto&       membership = CPedGroups::ms_groups.data()[groupIndex].GetMembership(); // 0xC09928 + grupo * 0x2D4
        const int32 excess     = membership.CountMembersExcludingLeader() - static_cast<int32>(data->m_nScriptLimitToGangSize); // 0x5F6AA0
        if (excess <= 0) {
            return OR_CONTINUE;
        }
        membership.RemoveNFollowers(excess); // 0x5FB1D0
        return OR_CONTINUE;
    }
    // 0x47600F
    case COMMAND_MAKE_PLAYER_GANG_DISAPPEAR: {
        FindPlayerPed(-1)->MakePlayerGroupDisappear(); // 0x60A440
        return OR_CONTINUE;
    }
    // 0x476027
    case COMMAND_MAKE_PLAYER_GANG_REAPPEAR: {
        FindPlayerPed(-1)->MakePlayerGroupReappear(); // 0x60A4B0
        return OR_CONTINUE;
    }
    // 0x47603F
    case COMMAND_GET_CLOTHES_ITEM: {
        CollectParameters(2);
        CPlayerInfo&     info    = CWorld::Players[P(0).iParam]; // 0xB7CD98 + id * 0x190
        const int32      texture = P(1).iParam;
        const auto       dependency = CClothes::GetTextureDependency(static_cast<eClothesTexturePart>(texture)); // 0x5A7EA0
        CPedClothesDesc* const desc = info.m_PlayerData.m_pPedClothesDesc;
        P(0).uParam = desc->m_anTextureKeys.data()[texture];
        P(1).uParam = desc->m_anModelKeys.data()[static_cast<int32>(dependency)];
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x47608E
    case COMMAND_SHOW_UPDATE_STATS: {
        CollectParameters(1);
        CStats::bShowUpdateStats = P(0).uParam != 0;
        return OR_CONTINUE;
    }
    // 0x4760AB y 0x4760AF: 2297 (variable global) y 2298 (variable local)
    case COMMAND_IS_VAR_TEXT_LABEL16_EQUAL_TO_TEXT_LABEL:
    case COMMAND_IS_LVAR_TEXT_LABEL16_EQUAL_TO_TEXT_LABEL: {
        tScriptParam* const var = GetPointerToScriptVariable(command == COMMAND_IS_VAR_TEXT_LABEL16_EQUAL_TO_TEXT_LABEL ? VAR_GLOBAL : VAR_LOCAL);
        char                label[16]{};
        ReadTextLabelFromScript(label, 16);
        const bool equal = strncmp(reinterpret_cast<const char*>(var), label, 16) == 0; // 0x8214D0
        UpdateCompareFlag(equal);
        return OR_CONTINUE;
    }
    // 0x4760FC
    case COMMAND_SET_COORD_BLIP_APPEARANCE: {
        CollectParameters(2);
        const uint32 blip       = P(0).uParam;
        const uint8  appearance = P(1).u8Param;
        CRadar::SetCoordBlipAppearance(blip, static_cast<eBlipAppearance>(appearance));
        return OR_CONTINUE;
    }

    // 0x47611C: comandos que el EXE deja vacios, sin leer parametros
    case COMMAND_ALLOCATE_SCRIPT_TO_ATTRACTOR:
    case COMMAND_GET_CLOSEST_ATTRACTOR_WITH_THIS_SCRIPT:
    case COMMAND_GET_CONVERSATION_STATUS:
    case COMMAND_GET_CHAR_MAP_ATTRACTOR_STATUS:
    case COMMAND_STREAM_SCRIPT_INTERNAL:
    case COMMAND_DRAW_WINDOW_TEXT:
    case COMMAND_SET_CAR_PITCH:
    case COMMAND_PICK_UP_OBJECT_WITH_WINCH:
    case COMMAND_PICK_UP_VEHICLE_WITH_WINCH:
    case COMMAND_PICK_UP_CHAR_WITH_WINCH:
    case COMMAND_STORE_CAR_IN_NEAREST_IMPOUNDING_GARAGE:
    case COMMAND_CONSTANT_INT:
        return OR_CONTINUE;

    // 0x476120: fuera de rango (ninguna entrada de la tabla)
    default:
        return OR_INTERRUPT;
    }
}
