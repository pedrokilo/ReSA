#include "StdInc.h"
#include "DatosExe/DataR5A.h"

#include <bit>
#include <cstring>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "PedGroups.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Radar.h"
#include "Streaming.h"
#include "PedType.h"
#include "TheZones.h"
#include "ZoneInfo.h"
#include "Clothes.h"
#include "Replay.h"
#include "Remote.h"
#include "Glass.h"
#include "BreakManager_c.h"
#include "ObjectData.h"
#include "StuckCarCheck.h"
#include "DamageManager.h"
#include "Shopping.h"
#include "Population.h"
#include "Scripted2dEffects.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "CarEnterExit.h"
#include "PatrolRoute.h"
#include "Conversations/Conversations.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "DatosExe/RdataConstantes.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskSimpleGangDriveBy.h"
#include "Tasks/TaskTypes/TaskComplexUseMobilePhone.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h"
#include "Tasks/TaskTypes/TaskComplexUseEffect.h"
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskComplexFleeAnyMeans.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"

// Codigo vivo (sin validar): 0x46D050 (7523 bytes de codigo + 1 de relleno hasta la tabla), CRunningScript::ProcessCommands1800To1899, el despachador original de los comandos de script 1800..1899 (ranura 18 de la tabla
// 0x8A6168; la usa ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con manejador 0x837FD0, 8 estados de desenrollado):
// solo protege los `new` de tareas (el destructor de desenrollado es operator delete), en C++ no hace falta.
// Salto con una tabla directa de 100 entradas (0x46EDB4): 23 comandos van a 0x46ED92 (devuelven 0 sin leer parametros: 1809 1810 1816 1819 1820 1821 1824 1825 1826 1829 1832 1848 1849 1850 1853 1856 1860 1864 1880 1881
// 1892 1893 1894) y 5 (1875 1878 1879 1896 1897) y los fuera de rango a 0x46ED96 (OR_INTERRUPT, -1). 1865 salta al cuerpo de 1800 (0x46D09F), 1837/1838 comparten la entrada 0x46DC1D y 1866 es una copia de 1801 en
// otra direccion (los tres pares comparten `case`).
// Los parametros se leen de ScriptParams (0xA43C78). Los indices, punteros y desplazamientos que vienen del script se usan sin comprobar rangos, como el EXE. Las comparaciones x87 con -100.0 son `z <= -100.0` (jp tras
// test ah,41h: con NaN no se toma) y las de distancia `d < radio` (test ah,5 + jp: con NaN tampoco).
namespace {
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x859014: float -100.0 (0xC2C80000)
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x858B8C: float 0.5; 0x859520: float 0.0015625 (1/640); 0x859524: float 0.0022321429 (1/448)
constexpr float EXE_HALF    = std::bit_cast<float>(0x3F000000u);
constexpr float EXE_INV_640 = std::bit_cast<float>(0x3ACCCCCDu);
constexpr float EXE_INV_448 = std::bit_cast<float>(0x3B124925u);
// distancia inicial "ninguna" de GET_RANDOM_CAR_IN_SPHERE_NO_SAVE / GET_RANDOM_CHAR_IN_SPHERE (dword 0x461C3F9A en la pila del EXE)
constexpr float EXE_FAR_DISTANCE = std::bit_cast<float>(0x461C3F9Au);
// dword 0x47C34FF3 que BREAK_OBJECT pasa como intensidad a CGlass::WindowRespondsToCollision
constexpr float EXE_GLASS_BREAK_INTENSITY = std::bit_cast<float>(0x47C34FF3u);
// 0x871640: float de .data (150.0 de serie) que usan DAMAGE_CAR_PANEL / DAMAGE_CAR_DOOR como intensidad
inline float& ExePanelDamageIntensity() {
    return DatosExe::R5::v871640;
}
// 0xC18DB8: ruta de patrulla global del EXE (CTheScripts)
inline CPatrolRoute* ExePatrolRoute() {
    return reinterpret_cast<CPatrolRoute*>(0xC18DB8);
}
// 0xC18CF0: tolerancia de cambio de posicion de la huida (float de .bss que inicializa el EXE al arrancar)
inline float ExeFleePosChangeTolerance() {
    return StaticRef<float>(0xC18CF0);
}

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
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
// Los comandos de avion/helicoptero solo cambian la mision si no es 57 ni 58 (MISSION_PLANE_CRASH_AND_BURN / MISSION_HELI_CRASH_AND_BURN)
inline void SetMissionUnlessCrashing(CVehicle* veh, eCarMission mission) {
    const auto current = veh->m_autoPilot.m_nCarMission;
    if (current != MISSION_PLANE_CRASH_AND_BURN && current != MISSION_HELI_CRASH_AND_BURN) {
        veh->m_autoPilot.m_nCarMission = mission;
    }
}
// m_autoPilot.m_TargetEntity (+0x41C): limpia la referencia anterior (si hay), guarda la nueva y la registra. El EXE no comprueba que el objetivo no sea nulo.
inline void SetAutoPilotTarget(CVehicle* veh, CEntity* target) {
    CVehicle*& ref = veh->m_autoPilot.m_TargetEntity;
    if (ref) {
        ref->CleanUpOldReference(reinterpret_cast<CEntity**>(&ref));
    }
    ref = reinterpret_cast<CVehicle*>(target);
    target->RegisterReference(reinterpret_cast<CEntity**>(&ref));
}
// Entidad que sostiene el ped segun el EXE: GetTaskHold(false) dos veces, SetPedPosition (ranura 8 de la tarea) y el campo +8 (m_pEntityToHold); nulo si no hay tarea o SetPedPosition da falso.
inline CEntity* HeldEntityOf(CPed* ped) {
    CEntity* held = nullptr;
    auto* const intelligence = ped->GetIntelligence();
    if (intelligence->GetTaskHold(false)) {
        if (intelligence->GetTaskHold(false)->SetPedPosition(ped)) {
            held = intelligence->GetTaskHold(false)->m_pEntityToHold;
        }
    }
    return held;
}
// 0x46A340 (ScriptTextPairSet): copia la primera cadena (hasta el cero INCLUIDO, sin limite) a +0 y la segunda a +0x18. Accesos volatiles: fijan el orden.
inline void ExeTextPairSet(void* self, const char* first, const char* second) {
    uint8                 character;
    const volatile uint8* source = reinterpret_cast<const volatile uint8*>(first);
    volatile uint8*       target = static_cast<uint8*>(self);
    do {
        character = *source++;
        *target++ = character;
    } while (character != 0);
    source = reinterpret_cast<const volatile uint8*>(second);
    target = static_cast<uint8*>(self) + 0x18;
    do {
        character = *source++;
        *target++ = character;
    } while (character != 0);
}
// 0x46AE80 (ScriptEntryListAdd): alta en la ruta de patrulla global (8 nodos de 0x28 bytes desde +4, 8 puntos desde +0x144, contador con signo en +0); con 8 o mas no hace nada.
inline bool ExeEntryListAdd(void* self, const uint32* position, const char* text) {
    auto* const     raw   = static_cast<uint8*>(self);
    volatile int32* count = reinterpret_cast<volatile int32*>(raw);
    if (*count >= 8) {
        return false;
    }
    volatile uint32* const destination = reinterpret_cast<volatile uint32*>(raw + 0x144 + static_cast<uint32>(*count) * 12u);
    destination[0]                     = position[0];
    destination[1]                     = position[1];
    destination[2]                     = position[2];
    volatile uint8* const entry        = raw + 4 + static_cast<uint32>(*count) * 0x28u;
    uint8                 character;
    const volatile uint8* source = reinterpret_cast<const volatile uint8*>(text);
    volatile uint8*       target = entry;
    do {
        character = *source++;
        *target++ = character;
    } while (character != 0);
    source = reinterpret_cast<const volatile uint8*>(text + 0x18);
    target = entry + 0x18;
    do {
        character = *source++;
        *target++ = character;
    } while (character != 0);
    *count = *count + 1;
    return true;
}
} // namespace

static_assert(offsetof(CObject, m_fHealth) == 0x154 && offsetof(CObject, m_nObjectFlags) == 0x140 && offsetof(CObject, m_pObjectInfo) == 0x160);
static_assert(offsetof(CObjectData, m_vecBreakVelocity) == 0x38 && offsetof(CObjectData, m_fBreakVelocityRand) == 0x44);
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390 && offsetof(CAutoPilot, m_nCarMission) == 0x2A && offsetof(CAutoPilot, m_nCruiseSpeed) == 0x40 && offsetof(CAutoPilot, m_TargetEntity) == 0x8C);
static_assert(offsetof(CAutoPilot, m_nCarCtrlFlags) == 0x4B);
static_assert(offsetof(CVehicle, m_nCreatedBy) == 0x4A4);
static_assert(offsetof(CPed, m_nPedState) == 0x530 && offsetof(CPed, m_nPedType) == 0x598);
static_assert(offsetof(CPedIntelligence, m_nDmNumPedsToScan) == 0xC4 && offsetof(CPedIntelligence, m_fDmRadius) == 0xC8);
static_assert(offsetof(CTaskSimpleGangDriveBy, m_bFromScriptCommand) == 0xE && offsetof(CTaskSimpleGangDriveBy, m_pTargetEntity) == 0x34);
static_assert(offsetof(CTaskSimpleCarSetPedInAsDriver, m_bWarpingInToCar) == 0x18);
static_assert(offsetof(CTaskSimpleCarSetPedInAsPassenger, m_bWarpingInToCar) == 0x1C);
static_assert(offsetof(CAutomobile, m_damageManager) == 0x5A0);
static_assert(sizeof(CZoneInfo) == 0x11 && sizeof(CZone) == 0x20 && offsetof(CZone, m_ZoneInfoIndex) == 0x1C);
static_assert(sizeof(tScriptRectangle) == 0x3C && offsetof(tScriptRectangle, m_nTextureId) == 6 && offsetof(tScriptRectangle, cornerA) == 8 && offsetof(tScriptRectangle, cornerB) == 0x10);
static_assert(offsetof(tScriptRectangle, m_nAngle) == 0x18 && offsetof(tScriptRectangle, m_nTransparentColor) == 0x1C && offsetof(tScriptRectangle, gxt1) == 0x20);
static_assert(TASK_COMPLEX_USE_MOBILE_PHONE == 0x640 && TASK_SIMPLE_GANG_DRIVEBY == 0x3FE);

// 0x46D050
OpcodeResult CRunningScript::ProcessCommands1800To1899(int32 command) {
    switch (command) {
    // 0x46D08E y 0x46E4B4 (1865 salta aqui, 0x46D09F): CLEAR_CHAR_DECISION_MAKER_EVENT_RESPONSE y CLEAR_GROUP_DECISION_MAKER_EVENT_RESPONSE
    case COMMAND_CLEAR_CHAR_DECISION_MAKER_EVENT_RESPONSE:
    case COMMAND_CLEAR_GROUP_DECISION_MAKER_EVENT_RESPONSE: {
        CollectParameters(2);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_DECISION_MAKER);
        if (index < 0 || index >= 0x14) {
            return OR_CONTINUE;
        }
        CDecisionMakerTypes::GetInstance()->FlushDecisionMakerEventResponse(index, static_cast<eEventType>(P(1).iParam));
        return OR_CONTINUE;
    }
    // 0x46D0D3 y 0x46E4CB (copia en otra direccion): ADD_CHAR_DECISION_MAKER_EVENT_RESPONSE y ADD_GROUP_DECISION_MAKER_EVENT_RESPONSE
    case COMMAND_ADD_CHAR_DECISION_MAKER_EVENT_RESPONSE:
    case COMMAND_ADD_GROUP_DECISION_MAKER_EVENT_RESPONSE: {
        CollectParameters(9);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_DECISION_MAKER);
        // El EXE guarda los parametros en la pila en este orden de memoria (de menor a mayor direccion): probabilidades = P6 P5 P3 P4 y banderas = P8 P7
        // (los dos punteros se pasan a CDecisionMakerTypes::AddEventResponse tal cual)
        float chances[4] = { P(6).fParam, P(5).fParam, P(3).fParam, P(4).fParam };
        int32 flags[2]   = { P(8).iParam, P(7).iParam };
        if (index < 0 || index >= 0x14) {
            return OR_CONTINUE;
        }
        CDecisionMakerTypes::GetInstance()->AddEventResponse(index, static_cast<eEventType>(P(1).iParam), static_cast<eTaskType>(P(2).iParam), chances, flags);
        return OR_CONTINUE;
    }
    // 0x46D166
    case COMMAND_TASK_PICK_UP_OBJECT: {
        ScriptTaskPickUpObject(command);
        return OR_CONTINUE;
    }
    // 0x46D175
    case COMMAND_DROP_OBJECT: {
        CollectParameters(2);
        CPed* const ped        = PedOf(P(0).iParam);
        const bool  quietSound = P(1).iParam != 0;
        if (!ped->GetIntelligence()->GetTaskHold(false)) {
            return OR_CONTINUE;
        }
        ped->GetIntelligence()->GetTaskHold(false)->DropEntity(ped, quietSound);
        return OR_CONTINUE;
    }
    // 0x46D1DB
    case COMMAND_EXPLODE_CAR_IN_CUTSCENE: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        RawField<uint8>(veh, 0x42A) |= 0x20; // bit 21 de vehicleFlags (+0x428)
        veh->BlowUpCar(nullptr, true);       // ranura 41 de la vtable (0xA4)
        return OR_CONTINUE;
    }
    // 0x46D21A
    case COMMAND_BUILD_PLAYER_MODEL: {
        CollectParameters(1);
        CClothes::RebuildPlayer(CWorld::Players[P(0).iParam].m_pPed, false);
        CReplay::Init();
        return OR_CONTINUE;
    }
    // 0x46D24B
    case COMMAND_PLANE_ATTACK_PLAYER: {
        CollectParameters(3);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        const float     minAltitude = P(2).fParam;
        SetMissionUnlessCrashing(veh, MISSION_PLANE_ATTACK_PLAYER);
        RawField<float>(veh, 0x9B0) = minAltitude;
        return OR_CONTINUE;
    }
    // 0x46D290
    case COMMAND_PLANE_FLY_IN_DIRECTION: {
        CollectParameters(4);
        CVehicle* const veh     = VehicleOf(P(0).iParam);
        const float     heading = P(1).fParam * EXE_DEG_TO_RAD;
        const float     minAlt  = P(2).fParam;
        const uint32    maxAlt  = P(3).uParam;
        SetMissionUnlessCrashing(veh, MISSION_PLANE_FLYINDIRECTION);
        RawField<float>(veh, 0x9B4)  = heading;
        RawField<uint32>(veh, 0x9A8) = maxAlt;
        RawField<float>(veh, 0x9B8)  = heading;
        RawField<float>(veh, 0x9B0)  = minAlt;
        return OR_CONTINUE;
    }
    // 0x46D304
    case COMMAND_PLANE_FOLLOW_ENTITY: {
        CollectParameters(4);
        CVehicle* const veh    = VehicleOf(P(0).iParam);
        CEntity*        target = nullptr;
        if (P(1).iParam >= 0) {
            target = PedOf(P(1).iParam);
        }
        if (P(2).iParam >= 0) {
            target = VehicleOf(P(2).iParam);
        }
        const uint32 minAltitude = P(3).uParam;
        SetMissionUnlessCrashing(veh, MISSION_PLANE_FOLLOW_ENTITY);
        SetAutoPilotTarget(veh, target);
        veh->vehicleFlags.bEngineOn = !veh->vehicleFlags.bEngineBroken; // CVehicle::SetEngineOn(true) (0x41BDD0)
        RawField<uint32>(veh, 0x9B0) = minAltitude;
        return OR_CONTINUE;
    }
    // 0x46D3A7
    case COMMAND_TASK_DRIVE_BY: {
        CollectParameters(10);
        const int32 pedHandle = P(0).iParam;
        const CVector targetPos{ P(3).fParam, P(4).fParam, P(5).fParam };
        const float   abortRange = P(6).fParam;
        const uint8   style      = P(7).u8Param;
        const bool    seatRHS    = P(8).iParam != 0;
        const int32   frequency  = P(9).iParam;
        CPed*         targetPed  = nullptr;
        CEntity*      target     = nullptr;
        if (P(1).iParam >= 0) {
            targetPed = PedOf(P(1).iParam);
            target    = targetPed;
        }
        if (P(2).iParam >= 0) {
            target = VehicleOf(P(2).iParam);
        }
        if (pedHandle != -1) {
            CTask* const current = PedOf(pedHandle)->GetIntelligence()->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
            if (current && current->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY && static_cast<CTaskSimpleGangDriveBy*>(current)->m_pTargetEntity == targetPed) {
                return OR_CONTINUE;
            }
        }
        auto* const task = new CTaskSimpleGangDriveBy(target, &targetPos, abortRange, static_cast<int8>(frequency), static_cast<eDrivebyStyle>(style), seatRHS);
        task->m_bFromScriptCommand = true;
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x46D4FA
    case COMMAND_SET_CAR_STAY_IN_SLOW_LANE: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->m_autoPilot.m_nCarCtrlFlags |= 0x10;
        } else {
            veh->m_autoPilot.m_nCarCtrlFlags &= 0xEF;
        }
        return OR_CONTINUE;
    }
    // 0x46D545
    case COMMAND_TAKE_REMOTE_CONTROL_OF_CAR: {
        CollectParameters(2);
        CRemote::SetPlayerRemoteControlledCar(VehicleOf(P(1).iParam)); // el EXE lee el segundo parametro
        return OR_CONTINUE;
    }
    // 0x46D56F
    case COMMAND_IS_CLOSEST_OBJECT_OF_TYPE_SMASHED_OR_DAMAGED: {
        CollectParameters(7);
        const float px = P(0).fParam;
        const float py = P(1).fParam;
        float       pz = P(2).fParam;
        if (pz <= EXE_MIN_GROUND_Z) { // fcom + test ah,41h + jp
            pz = CWorld::FindGroundZForCoord(px, py);
        }
        const int32   model  = ScriptModelIndex(P(4).iParam);
        const float   radius = P(3).fParam;
        const CVector pos{ px, py, pz };
        CEntity* const found = CWorld::FindNearestObjectOfType(model, pos, radius, false, false, false, false, true, true);
        bool           flag  = false;
        if (found && found->GetType() == ENTITY_TYPE_OBJECT) {
            CObject* const obj = static_cast<CObject*>(found);
            if ((obj->m_nObjectFlags & 0x400) && P(5).iParam != 0) {
                flag = true;
            }
            if ((obj->m_bRenderDamaged || !obj->m_bIsVisible) && P(6).iParam != 0) { // CEntity::IsInvisibleOrRenderDamaged (0x46A2F0): bit 9 puesto o bit 7 apagado
                flag = true;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x46D65E
    case COMMAND_START_SETTING_UP_CONVERSATION: {
        CollectParameters(1);
        CConversations::StartSettingUpConversation(PedOf(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x46D689
    case COMMAND_FINISH_SETTING_UP_CONVERSATION: {
        CConversations::DoneSettingUpConversation(false);
        return OR_CONTINUE;
    }
    // 0x46D69A
    case COMMAND_IS_CONVERSATION_AT_NODE: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        char        label[8]{};
        ReadTextLabelFromScript(label, 8);
        bool flag = false;
        if (CConversations::IsConversationAtNode(label, ped)) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x46D6F4
    case COMMAND_GET_OBJECT_HEALTH: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        P(0).iParam        = static_cast<int32>(obj->m_fHealth); // fld + _ftol2
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x46D71E
    case COMMAND_SET_OBJECT_HEALTH: {
        CollectParameters(2);
        const float health = static_cast<float>(P(1).iParam); // fild
        CObject* const obj = ObjectOf(P(0).iParam);
        obj->m_fHealth     = health;
        return OR_CONTINUE;
    }
    // 0x46D754
    case COMMAND_BREAK_OBJECT: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (CGlass::IsObjectGlass(obj)) {
            const CVector pos = obj->GetPosition();
            CGlass::WindowRespondsToCollision(obj, EXE_GLASS_BREAK_INTENSITY, CVector{ 0.0f, 0.0f, 0.0f }, pos, false);
            obj->m_nObjectFlags |= 0x400;
            return OR_CONTINUE;
        }
        const auto* const info = obj->m_pObjectInfo;
        CVector           velocity = info->m_vecBreakVelocity;
        g_breakMan.Add(obj, &velocity, info->m_fBreakVelocityRand, P(1).iParam != 0);
        obj->m_nFlags &= 0xFFFFFF7Eu; // bUsesCollision (bit 0) y bIsVisible (bit 7)
        if (!obj->GetIsStatic()) {
            obj->RemoveFromMovingList();
        }
        obj->m_nPhysicalFlags |= 0x800000;
        obj->m_nObjectFlags |= 0x400;
        obj->m_nFlags |= 4; // bIsStatic
        obj->m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        obj->m_vecTurnSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        obj->DeleteRwObject(); // ranura 8 de la vtable (0x20)
        return OR_CONTINUE;
    }
    // 0x46D8A7
    case COMMAND_HELI_ATTACK_PLAYER: {
        CollectParameters(3);
        CVehicle* const veh         = VehicleOf(P(0).iParam);
        const float     minAltitude = P(2).fParam;
        SetMissionUnlessCrashing(veh, MISSION_HELI_ATTACK_PLAYER);
        RawField<float>(veh, 0x9B0)    = minAltitude;
        veh->m_autoPilot.m_nCruiseSpeed = 0x64;
        return OR_CONTINUE;
    }
    // 0x46D8F3 y 0x46D994 (copia casi identica con otra mision): HELI_FOLLOW_ENTITY (39) y POLICE_HELI_CHASE_ENTITY (40)
    case COMMAND_HELI_FOLLOW_ENTITY:
    case COMMAND_POLICE_HELI_CHASE_ENTITY: {
        CollectParameters(4);
        CVehicle* const veh    = VehicleOf(P(0).iParam);
        CEntity*        target = nullptr;
        if (P(1).iParam >= 0) {
            target = PedOf(P(1).iParam);
        }
        if (P(2).iParam >= 0) {
            target = VehicleOf(P(2).iParam);
        }
        const uint32 minAltitude = P(3).uParam;
        SetMissionUnlessCrashing(veh, command == COMMAND_HELI_FOLLOW_ENTITY ? MISSION_HELI_FOLLOW_ENTITY : MISSION_HELI_POLICE_BEHAVIOUR);
        SetAutoPilotTarget(veh, target);
        RawField<uint32>(veh, 0x9B0)    = minAltitude;
        veh->m_autoPilot.m_nCruiseSpeed = 0x64;
        return OR_CONTINUE;
    }
    // 0x46DA35
    case COMMAND_TASK_USE_MOBILE_PHONE: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        if (P(1).iParam > 0) {
            auto* const task = CTaskComplexUseMobilePhoneConstructorExe(static_cast<CTaskComplexUseMobilePhone*>(CTask::operator new(0x20)), -1);
            GivePedScriptedTask(pedHandle, task, command);
            return OR_CONTINUE;
        }
        CPed* const ped = PedOf(pedHandle);
        CTask* const task = ped->GetIntelligence()->m_TaskMgr.FindTaskByType(TASK_PRIMARY_PRIMARY, TASK_COMPLEX_USE_MOBILE_PHONE);
        if (!task) {
            return OR_CONTINUE;
        }
        if (task->GetTaskType() != TASK_COMPLEX_USE_MOBILE_PHONE) {
            return OR_CONTINUE;
        }
        static_cast<CTaskComplexUseMobilePhone*>(task)->Quit(ped);
        return OR_CONTINUE;
    }
    // 0x46DAE8
    case COMMAND_TASK_WARP_CHAR_INTO_CAR_AS_DRIVER: {
        CollectParameters(2);
        const int32     pedHandle = P(0).iParam;
        CVehicle* const veh       = VehicleOf(P(1).iParam);
        auto* const     task      = new CTaskSimpleCarSetPedInAsDriver(veh, nullptr);
        task->m_bWarpingInToCar   = true;
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x46DB5D
    case COMMAND_TASK_WARP_CHAR_INTO_CAR_AS_PASSENGER: {
        CollectParameters(3);
        const int32     pedHandle = P(0).iParam;
        CVehicle* const veh       = VehicleOf(P(1).iParam);
        int32           door      = 0;
        if (P(2).iParam >= 0) {
            door = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, P(2).iParam);
        }
        auto* const task        = new CTaskSimpleCarSetPedInAsPassenger(veh, static_cast<eTargetDoor>(door), false, nullptr);
        task->m_bWarpingInToCar = true;
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x46DBE9
    case COMMAND_SWITCH_COPS_ON_BIKES: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            CStreaming::DisableCopBikes(false);
            return OR_CONTINUE;
        }
        CStreaming::DisableCopBikes(true);
        return OR_CONTINUE;
    }
    // 0x46DC1D
    case COMMAND_IS_FLAME_IN_ANGLED_AREA_2D:
    case COMMAND_IS_FLAME_IN_ANGLED_AREA_3D: {
        FlameInAngledAreaCheckCommand(command);
        return OR_CONTINUE;
    }
    // 0x46DC2C
    case COMMAND_ADD_STUCK_CAR_CHECK_WITH_WARP: {
        CollectParameters(7);
        const bool stuck    = P(3).iParam != 0;
        const bool flipped  = P(4).iParam != 0;
        const bool inWater  = P(5).iParam != 0;
        CTheScripts::StuckCars.AddCarToCheck(P(0).iParam, P(1).fParam, P(2).uParam, true, stuck, flipped, inWater, static_cast<int8>(P(6).u8Param));
        return OR_CONTINUE;
    }
    // 0x46DCAD
    case COMMAND_DAMAGE_CAR_PANEL: {
        CollectParameters(2);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        automobile->m_damageManager.ApplyDamage(automobile, static_cast<tComponent>(P(1).iParam + 0xB), ExePanelDamageIntensity(), 1.0f);
        return OR_CONTINUE;
    }
    // 0x46DCF0
    case COMMAND_SET_CAR_ROLL: {
        CollectParameters(2);
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        const float     roll  = P(1).fParam;
        const CVector   pos   = veh->GetPosition();
        const float     heading = veh->GetHeading();
        veh->m_matrix->SetRotateZ(heading);
        veh->m_matrix->RotateY(roll * EXE_DEG_TO_RAD);
        // 0x46AE00: suma el vector a la posicion de la matriz (x, y, z en este orden; la suma de floats es conmutativa)
        CVector& matrixPos = veh->m_matrix->GetPosition();
        matrixPos.x        = matrixPos.x + pos.x;
        matrixPos.y        = pos.y + matrixPos.y;
        matrixPos.z        = pos.z + matrixPos.z;
        return OR_CONTINUE;
    }
    // 0x46DD76
    case COMMAND_SUPPRESS_CAR_MODEL: {
        CollectParameters(1);
        CTheScripts::AddToSuppressedCarModels(static_cast<eModelID>(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x46DD95
    case COMMAND_DONT_SUPPRESS_CAR_MODEL: {
        CollectParameters(1);
        CTheScripts::RemoveFromSuppressedCarModels(static_cast<eModelID>(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x46DDB4: rellena con -1 las 40 dwords (160 bytes) desde 0xA44940
    case COMMAND_DONT_SUPPRESS_ANY_CAR_MODELS: {
        std::memset(CTheScripts::SuppressedVehicleModels.data(), 0xFF, 0x28 * sizeof(uint32));
        return OR_CONTINUE;
    }
    // 0x46DDCA y 0x46DE0E: el EXE llama a dos funciones sin cuerpo (0x571980 y 0x571970: thiscall con ECX = 0xB9B9A4, `xor al,al`) que devuelven siempre false y no leen sus argumentos
    case COMMAND_IS_PS2_KEYBOARD_KEY_PRESSED:
    case COMMAND_IS_PS2_KEYBOARD_KEY_JUST_PRESSED: {
        CollectParameters(1);
        UpdateCompareFlag(false);
        return OR_CONTINUE;
    }
    // 0x46DE3E
    case COMMAND_IS_CHAR_HOLDING_OBJECT: {
        CollectParameters(2);
        CPed*    ped = nullptr;
        CObject* obj = nullptr;
        if (P(0).iParam >= 0) {
            ped = PedOf(P(0).iParam);
        }
        if (P(1).iParam >= 0) {
            obj = ObjectOf(P(1).iParam);
        }
        bool flag = false;
        if (ped) {
            CEntity* const held = HeldEntityOf(ped);
            if (obj) {
                if (obj == held) {
                    flag = true;
                }
            } else if (held) {
                flag = true;
            }
            UpdateCompareFlag(flag);
            return OR_CONTINUE;
        }
        // sin ped: recorre la reserva de peds desde el ultimo hueco al primero y para en cuanto alguno sostiene el objeto (o cualquier objeto si no se dio ninguno)
        auto* const pool = GetPedPool();
        for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
            --i;
            if (flag) {
                break;
            }
            CPed* const candidate = pool->GetAt(i);
            if (!candidate) {
                continue;
            }
            CEntity* const held = HeldEntityOf(candidate);
            if (obj) {
                if (obj == held) {
                    flag = true;
                }
            } else if (held) {
                flag = true;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x46DF7A
    case COMMAND_SET_CAR_CAN_GO_AGAINST_TRAFFIC: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->m_autoPilot.m_nCarCtrlFlags &= 0xBF;
        } else {
            veh->m_autoPilot.m_nCarCtrlFlags |= 0x40;
        }
        return OR_CONTINUE;
    }
    // 0x46DFC5
    case COMMAND_DAMAGE_CAR_DOOR: {
        CollectParameters(2);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        automobile->m_damageManager.ApplyDamage(automobile, static_cast<tComponent>(P(1).iParam + 5), ExePanelDamageIntensity(), 1.0f);
        return OR_CONTINUE;
    }
    // 0x46E009
    case COMMAND_GET_RANDOM_CAR_IN_SPHERE_NO_SAVE: {
        CollectParameters(5);
        const CVector target{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   radius = P(3).fParam;
        const int32   model  = P(4).iParam;
        int32         result = -1;
        float         best   = EXE_FAR_DISTANCE;
        auto* const   pool   = GetVehiclePool();
        for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
            --i;
            CVehicle* const veh = pool->GetAt(i);
            if (!veh) {
                continue;
            }
            if (veh->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE && veh->GetVehicleAppearance() != VEHICLE_APPEARANCE_BIKE) {
                continue;
            }
            if (RawField<uint8>(veh, 0x428) & 1) { // bIsLawEnforcer
                continue;
            }
            if (static_cast<int16>(veh->m_nModelIndex) != model && model >= 0) {
                continue;
            }
            if (!veh->CanBeDeleted()) {
                continue;
            }
            const CVector pos = veh->GetPosition();
            const CVector diff{ pos.x - target.x, pos.y - target.y, pos.z - target.z };
            const float   distance = diff.Magnitude();
            if (!(distance < radius)) {
                continue;
            }
            if (!(distance < best)) {
                continue;
            }
            result = pool->GetRef(veh);
            best   = distance;
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x46E172
    case COMMAND_GET_RANDOM_CHAR_IN_SPHERE: {
        CollectParameters(7);
        const CVector target{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float   radius = P(3).fParam;
        int32         result = -1;
        float         best   = EXE_FAR_DISTANCE;
        auto* const   pool   = GetPedPool();
        for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
            --i;
            CPed* const ped = pool->GetAt(i);
            if (!ped) {
                continue;
            }
            if (ped->GetCreatedBy() != PED_GAME) {
                continue;
            }
            if (ped->m_nFlags & 0x800) { // bit 11
                continue;
            }
            if (RawField<uint8>(ped, 0x470) & 8) {
                continue;
            }
            if (IsPedDead(ped)) {
                continue;
            }
            if (!ThisIsAValidRandomPed(ped->m_nPedType, P(4).iParam != 0, P(5).iParam != 0, P(6).iParam != 0)) {
                continue;
            }
            if (CPedGroups::GetPedsGroup(ped)) {
                continue;
            }
            const CVector pos = ped->GetPosition();
            const CVector diff{ pos.x - target.x, pos.y - target.y, pos.z - target.z };
            const float   distance = diff.Magnitude();
            if (!(distance < radius)) {
                continue;
            }
            if (!(distance < best)) {
                continue;
            }
            result = static_cast<int32>(pool->GetRef(ped));
            best   = distance;
        }
        if (result >= 0) {
            PedOf(result)->SetCharCreatedBy(PED_MISSION);
            CPopulation::ms_nTotalMissionPeds++;
            if (m_UsesMissionCleanup) {
                CTheScripts::MissionCleanUp.AddEntityToList(result, MISSION_CLEANUP_ENTITY_TYPE_PED);
            }
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x46E348
    case COMMAND_HAS_CHAR_BEEN_ARRESTED: {
        CollectParameters(1);
        bool flag = false;
        if (static_cast<int32>(PedOf(P(0).iParam)->m_nPedState) == 0x3F) { // PEDSTATE_ARRESTED
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x46E37A
    case COMMAND_SET_PLANE_THROTTLE: {
        CollectParameters(2);
        CVehicle* const veh         = VehicleOf(P(0).iParam);
        RawField<uint32>(veh, 0x998) = P(1).uParam;
        return OR_CONTINUE;
    }
    // 0x46E3A8
    case COMMAND_HELI_LAND_AT_COORDS: {
        CollectParameters(6);
        auto* const automobile = static_cast<CAutomobile*>(VehicleOf(P(0).iParam));
        automobile->TellHeliToGoToCoors(P(1).fParam, P(2).fParam, P(3).fParam, P(4).fParam, P(5).fParam);
        SetMissionUnlessCrashing(automobile, MISSION_HELI_LAND);
        return OR_CONTINUE;
    }
    // 0x46E411
    case COMMAND_PLANE_STARTS_IN_AIR: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        static_cast<CPlane*>(veh)->IsAlreadyFlying(); // el EXE descarta el resultado
        if (static_cast<int16>(veh->m_nModelIndex) == 0x208) {
            RawField<uint16>(veh, 0x86C) = 0;
        }
        return OR_CONTINUE;
    }
    // 0x46E450
    case COMMAND_SET_RELATIONSHIP: {
        CollectParameters(3);
        const uint32 flag = CPedType::GetPedFlag(static_cast<ePedType>(P(2).iParam));
        CPedType::SetPedTypeAsAcquaintance(static_cast<AcquaintanceId>(P(0).iParam), static_cast<ePedType>(P(1).iParam), static_cast<int32>(flag));
        return OR_CONTINUE;
    }
    // 0x46E482
    case COMMAND_CLEAR_RELATIONSHIP: {
        CollectParameters(3);
        const uint32 flag = CPedType::GetPedFlag(static_cast<ePedType>(P(2).iParam));
        CPedType::ClearPedTypeAsAcquaintance(static_cast<AcquaintanceId>(P(0).iParam), static_cast<ePedType>(P(1).iParam), static_cast<int32>(flag));
        return OR_CONTINUE;
    }
    // 0x46E55E
    case COMMAND_DRAW_SPRITE_WITH_ROTATION: {
        CollectParameters(10);
        const float centreX   = P(1).fParam;
        const float centreY   = P(2).fParam;
        const float halfWidth = P(3).fParam * EXE_HALF;
        const float halfHeight = P(4).fParam * EXE_HALF;
        const float scaleX    = static_cast<float>(RsGlobal.maximumWidth) * EXE_INV_640;  // fild [0xC17044]
        const float scaleY    = static_cast<float>(RsGlobal.maximumHeight) * EXE_INV_448; // fild [0xC17048]
        auto&       rect      = CTheScripts::IntroRectangles.data()[CTheScripts::NumberOfIntroRectanglesThisFrame];
        rect.m_nType          = eScriptRectangleType::MONOCOLOR_ANGLED;
        rect.m_nTextureId     = static_cast<int16>(P(0).iParam - 1);
        rect.cornerA.x        = (centreX - halfWidth) * scaleX;
        rect.cornerA.y        = (centreY - halfHeight) * scaleY;
        rect.cornerB.x        = (halfWidth + centreX) * scaleX;
        rect.cornerB.y        = (halfHeight + centreY) * scaleY;
        rect.m_nAngle         = P(5).fParam * EXE_DEG_TO_RAD;
        rect.m_nTransparentColor = CRGBA{ P(6).u8Param, P(7).u8Param, P(8).u8Param, P(9).u8Param };
        CTheScripts::NumberOfIntroRectanglesThisFrame++;
        rect.gxt1[0] = '\0';
        return OR_CONTINUE;
    }
    // 0x46E675
    case COMMAND_TASK_USE_ATTRACTOR: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 index     = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_2D_EFFECT);
        if (index < 0 || index >= 0x40) {
            return OR_CONTINUE;
        }
        // 0xC3AB00 + index * 0x40 (CScripted2dEffects::ms_effects): el atractor del script
        auto* const effect = reinterpret_cast<C2dEffectPedAttractor*>(&CScripted2dEffects::ms_effects.data()[index]);
        auto* const task   = new CTaskComplexUseEffect(effect, nullptr);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x46E6FF
    case COMMAND_TASK_SHOOT_AT_CHAR: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam;
        CPed* const target    = PedOf(P(1).iParam);
        const int32 duration  = P(2).iParam;
        auto* const task      = new CTaskSimpleGunControl(target, CVector{}, CVector{}, static_cast<eGunCommand>(3), 1, duration);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x46E77C
    case COMMAND_SET_INFORM_RESPECTED_FRIENDS: {
        CollectParameters(3);
        auto* const intelligence         = PedOf(P(0).iParam)->GetIntelligence();
        const uint32 numPeds             = P(2).uParam;
        intelligence->m_fDmRadius        = P(1).fParam;
        intelligence->m_nDmNumPedsToScan = numPeds;
        return OR_CONTINUE;
    }
    // 0x46E7BC
    case COMMAND_IS_CHAR_RESPONDING_TO_EVENT: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        UpdateCompareFlag(ped->GetIntelligence()->IsRespondingToEvent(static_cast<eEventType>(P(1).iParam)));
        return OR_CONTINUE;
    }
    // 0x46E802
    case COMMAND_SET_OBJECT_VISIBLE: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            obj->m_nFlags |= 0x80; // bIsVisible
        } else {
            obj->m_nFlags &= 0xFFFFFF7Fu;
        }
        return OR_CONTINUE;
    }
    // 0x46E84A
    case COMMAND_TASK_FLEE_CHAR_ANY_MEANS: {
        CollectParameters(8);
        const int32 pedHandle     = P(0).iParam;
        const float safeDistance  = P(2).fParam;
        const int32 fleeTime      = P(3).iParam;
        const bool  attackFleeing = P(4).iParam != 0;
        const int32 shootTime     = P(5).iParam;
        const int32 shootRecover  = P(6).iParam;
        const float stealCarDist  = P(7).fParam;
        CPed* const target        = PedOf(P(1).iParam);
        auto* const task          = new CTaskComplexFleeAnyMeans(target, attackFleeing, safeDistance, fleeTime, shootTime, shootRecover, stealCarDist, DatosExe::Rdata::k86F678, ExeFleePosChangeTolerance());
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x46E91A
    case COMMAND_STOP_RECORDING_CAR: {
        CollectParameters(1);
        return OR_CONTINUE;
    }
    // 0x46E92A: pone a cero el contador de la ruta de patrulla global
    case COMMAND_FLUSH_PATROL_ROUTE: {
        ExePatrolRoute()->m_nNumNodes = 0;
        return OR_CONTINUE;
    }
    // 0x46E93B
    case COMMAND_EXTEND_PATROL_ROUTE: {
        CollectParameters(3);
        const uint32 position[3] = { P(0).uParam, P(1).uParam, P(2).uParam };
        char         first[0x18]{};
        ReadTextLabelFromScript(first, 0x18);
        char second[0x10]{};
        ReadTextLabelFromScript(second, 0x10);
        // par de textos (0x28 bytes: primera cadena en +0 y segunda en +0x18); el EXE lo deja en la pila sin inicializar
        uint8 pair[0x28]{};
        if (std::memcmp(first, "NONE", 5) != 0) { // 0x859E38: "NONE\0"
            ExeTextPairSet(pair, first, second);
        } else {
            pair[0]    = 0;
            pair[0x18] = 0;
        }
        ExeEntryListAdd(ExePatrolRoute(), position, reinterpret_cast<const char*>(pair));
        return OR_CONTINUE;
    }
    // 0x46E9FC
    case COMMAND_PLAY_OBJECT_ANIM: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        char           animName[0x18]{};
        ReadTextLabelFromScript(animName, 0x18);
        char blockName[0x10]{};
        ReadTextLabelFromScript(blockName, 0x10);
        CollectParameters(3);
        const float            blendDelta = P(0).fParam;
        CAnimBlock* const      block      = CAnimManager::GetAnimationBlock(blockName);
        CAnimBlendHierarchy* const hierarchy = CAnimManager::GetAnimation(animName, block);
        int32 flags = 0;
        if (P(1).iParam != 0) {
            flags = 2;
        }
        if (P(2).iParam == 0) {
            flags |= 8;
        }
        RpClump* const clump = reinterpret_cast<RpClump*>(obj->GetRwObject()); // +0x18
        if (!clump) {
            UpdateCompareFlag(false);
            return OR_CONTINUE;
        }
        if (!RpAnimBlendClumpIsInitialized(clump)) {
            RpAnimBlendClumpInit(clump);
        }
        CAnimManager::BlendAnimation(clump, hierarchy, flags, blendDelta);
        UpdateCompareFlag(true);
        return OR_CONTINUE;
    }
    // 0x46EAE7
    case COMMAND_SET_RADAR_ZOOM: {
        CollectParameters(1);
        CTheScripts::RadarZoomValue = P(0).u8Param;
        return OR_CONTINUE;
    }
    // 0x46EB03
    case COMMAND_DOES_BLIP_EXIST: {
        CollectParameters(1);
        UpdateCompareFlag(CRadar::GetActualBlipArrayIndex(P(0).iParam) != -1);
        return OR_CONTINUE;
    }
    // 0x46EB37
    case COMMAND_LOAD_PRICES: {
        char label[0x10]{};
        ReadTextLabelFromScript(label, 0x10);
        CShopping::LoadPrices(label);
        return OR_CONTINUE;
    }
    // 0x46EB59
    case COMMAND_LOAD_SHOP: {
        char label[0x10]{};
        ReadTextLabelFromScript(label, 0x10);
        CShopping::LoadShop(label);
        return OR_CONTINUE;
    }
    // 0x46EB7B
    case COMMAND_GET_NUMBER_OF_ITEMS_IN_SHOP: {
        P(0).iParam = StaticRef<int32>(0xA9A7F0);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x46EB97
    case COMMAND_GET_ITEM_IN_SHOP: {
        CollectParameters(1);
        P(0).uParam = StaticRef<std::array<uint32, 300>>(0xA9A318).data()[P(0).iParam];
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x46EBAF
    case COMMAND_GET_PRICE_OF_ITEM: {
        CollectParameters(1);
        P(0).iParam = CShopping::GetPrice(P(0).uParam);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x46EBDB
    case COMMAND_TASK_DEAD: {
        CollectParameters(1);
        // CTaskComplexDie(arma 0, grupo de animaciones 0, animacion 0xF, mezcla 4.0, velocidad 0.0, 0, 0, 0, 0)
        auto* const task = new CTaskComplexDie(static_cast<eWeaponType>(0), static_cast<AssocGroupId>(0), static_cast<AnimationId>(0xF), 4.0f, 0.0f, false, false, static_cast<eDirection>(0), false);
        GivePedScriptedTask(P(0).iParam, task, command);
        return OR_CONTINUE;
    }
    // 0x46EC43
    case COMMAND_SET_CAR_AS_MISSION_CAR: {
        CollectParameters(1);
        CVehicle* const veh    = VehicleOf(P(0).iParam);
        const int32     handle = GetVehiclePool()->GetRef(veh);
        if (m_UsesMissionCleanup) {
            const uint32 createdBy = static_cast<uint8>(veh->m_nCreatedBy);
            if (createdBy == RANDOM_VEHICLE || createdBy == PARKED_VEHICLE) {
                veh->SetVehicleCreatedBy(MISSION_VEHICLE);
                CTheScripts::MissionCleanUp.AddEntityToList(handle, MISSION_CLEANUP_ENTITY_TYPE_VEHICLE);
            }
        }
        return OR_CONTINUE;
    }
    // 0x46ECA2
    case COMMAND_SET_ZONE_POPULATION_TYPE: {
        char label[8]{};
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        const int16 zoneIndex = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO);
        if (zoneIndex >= 0) {
            const uint16 infoIndex = static_cast<uint16>(CTheZones::NavigationZoneArray.data()[zoneIndex].m_ZoneInfoIndex);
            CTheZones::ZoneInfoArray.data()[infoIndex].PopType = P(0).u8Param & 0x1F; // solo los 5 bits bajos del byte +0xF
        }
        return OR_CONTINUE;
    }
    // 0x46ED05
    case COMMAND_SET_ZONE_DEALER_STRENGTH: {
        char label[8]{};
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        const int16 zoneIndex = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO);
        if (zoneIndex >= 0) {
            const uint16 infoIndex = static_cast<uint16>(CTheZones::NavigationZoneArray.data()[zoneIndex].m_ZoneInfoIndex);
            CTheZones::ZoneInfoArray.data()[infoIndex].DealerStrength = P(0).u8Param;
        }
        return OR_CONTINUE;
    }
    // 0x46ED4F: sin comprobar el indice de zona (el EXE no lo hace)
    case COMMAND_GET_ZONE_DEALER_STRENGTH: {
        char label[8]{};
        ReadTextLabelFromScript(label, 8);
        const int16  zoneIndex = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO);
        const uint16 infoIndex = static_cast<uint16>(CTheZones::NavigationZoneArray.data()[zoneIndex].m_ZoneInfoIndex);
        P(0).uParam            = CTheZones::ZoneInfoArray.data()[infoIndex].DealerStrength;
        StoreParameters(1);
        return OR_CONTINUE;
    }

    // 0x46ED92: comandos que el EXE deja vacios, sin leer parametros
    case COMMAND_ALLOCATE_SCRIPT_TO_PED_GENERATOR:
    case COMMAND_ALLOCATE_SCRIPT_TO_RANDOM_PED:
    case COMMAND_SET_UP_CONVERSATION_NODE:
    case COMMAND_CLEAR_ALL_CONVERSATIONS:
    case COMMAND_GET_CHAR_LIGHTING:
    case COMMAND_SET_CLOSEST_OBJECT_OF_TYPE_RENDER_SCORCHED:
    case COMMAND_GET_VEHICLE_WHEEL_UPGRADE_CLASS:
    case COMMAND_GET_NUM_WHEELS_IN_UPGRADE_CLASS:
    case COMMAND_GET_WHEEL_IN_UPGRADE_CLASS:
    case COMMAND_HELI_FLY_IN_DIRECTION:
    case COMMAND_SET_UP_CONVERSATION_END_NODE:
    case COMMAND_SET_ZONE_RADAR_COLOURS:
    case COMMAND_GIVE_LOWRIDER_SUSPENSION_TO_CAR:
    case COMMAND_DOES_CAR_HAVE_LOWRIDER_SUSPENSION:
    case COMMAND_GET_RANDOM_CAR_IN_SPHERE:
    case COMMAND_GET_COORDS_OF_CLOSEST_COLLECTABLE1:
    case COMMAND_GET_STAT_CHANGE_AMOUNT:
    case COMMAND_CLEAR_ALL_RELATIONSHIPS:
    case COMMAND_SET_CHAR_SPECIAL_EVENT:
    case COMMAND_SET_ATTRACTOR_AS_COVER_NODE:
    case COMMAND_IS_SEARCHLIGHT_IN_ANGLED_AREA_2D:
    case COMMAND_IS_SEARCHLIGHT_IN_ANGLED_AREA_3D:
    case COMMAND_SWITCH_SEARCHLIGHT_BULB:
        return OR_CONTINUE;

    // 0x46ED96: sin caso en el EXE (1875 1878 1879 1896 1897) y fuera de rango
    default:
        return OR_INTERRUPT;
    }
}
