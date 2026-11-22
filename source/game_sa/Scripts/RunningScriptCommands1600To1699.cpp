#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "World.h"
#include "Stats.h"
#include "General.h"
#include "Pickups.h"
#include "Pickup.h"
#include "PlayerInfo.h"
#include "Camera.h"
#include "DamageManager.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "PedAttractorManager.h"
#include "Scripted2dEffects.h"
#include "ScriptResourceManager.h"
#include "PedIntelligence.h"
#include "Fx/Fx.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Ragdoll/IKChainManager.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskSimpleClearLookAt.h"
#include "Tasks/TaskTypes/TaskSimpleTriggerLookAt.h"
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskSimpleThrowControl.h"
#include "Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.h"
#include "Tasks/TaskTypes/TaskComplexShuffleSeats.h"
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"

// F4 de codigo vivo (sin validar): 0x493FE0 (7968 bytes, hasta 0x495DFE), CRunningScript::ProcessCommands1600To1699, el despachador original de los comandos de script 1602..1699 (ranura 16 de la tabla 0x8A6168; lo usa
// ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). OJO: el EXE resta 0x642 (1602), no 1600: `lea eax,[ebp-642h]; cmp eax,61h; ja 0x495DE1`, de modo que los
// comandos 1600 y 1601 (y todo lo que no esta en 1602..1699) caen en el destino de -1. Prologo SEH (marco con manejador 0x838B93, 13 estados de desenrollado): solo protege los `new` de tareas y el CTaskSimpleTogglePedThreatScanner
// de pila, que en C++ se destruyen solos; el EXE comprueba el resultado de CTask::operator new (tarea nula): en C++ `new` no devuelve nulo.
// Salto con una tabla directa de 98 entradas (0x495E00; indice = comando - 1602): 18 comandos van a 0x495DDD (devuelven 0 sin leer parametros: 1610 1613 1617 1625 1626 1638 1647 1648 1649 1653 1656 1679 1692-1697) y 11
// (1604 1605 1609 1624 1680-1686) y los fuera de rango a 0x495DE1 (OR_INTERRUPT, -1). Los comandos 1629 y 1632 comparten cuerpo (0x4947C9); 1630, 1631, 1633 y 1634 caen unos en otros pero hacen lo mismo salvo GetIndexOfGlobalVariable.
// Los desplazamientos y los indices que vienen del script se usan sin comprobar rangos, como el EXE, salvo donde el EXE los comprueba (cosas del script 0..63, tomadores de decision 0..19).
// Las comparaciones x87 con -100.0 son `z <= -100.0` (jp tras test ah,41h: con NaN no se toma).
namespace {
// 0x859014: float -100.0 (0xC2C80000), umbral de "usar la altura del suelo"
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x858B40: float 50.0 (0x42480000), segundos a pasos de 0.02 de GET_CAR_SPEED_VECTOR
constexpr float EXE_FIFTY = std::bit_cast<float>(0x42480000u);
// 0x858B50: float 0.0 y 0x859E2C: float 360.0 (0x43B40000) de LIMIT_ANGLE
constexpr float EXE_ZERO    = 0.0f;
constexpr float EXE_360     = std::bit_cast<float>(0x43B40000u);
// 0x858C1C: float -1.0 (0xBF800000) de DETACH_OBJECT / DETACH_CAR
constexpr float EXE_MINUS_ONE = std::bit_cast<float>(0xBF800000u);
// 0x85A3B4: float -999.9 (0xC479F99A): centinela de ATTACH_CAR_TO_CAR
constexpr float EXE_ATTACH_SENTINEL = std::bit_cast<float>(0xC479F99Au);
// 0x85A3D4 y 0x85A3B8: textos que el EXE pasa como nombre de comando a IKChainManager_c::LookAt y a CTaskComplexPartnerChat
constexpr const char* EXE_LOOK_AT_OBJECT_NAME = "COMMAND_TASK_LOOK_AT_OBJECT";
constexpr const char* EXE_CHAT_PARTNER_NAME   = "COMMAND_TASK_CHAT_WITH_CHAR";

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
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, ptrdiff_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
} // namespace

static_assert(offsetof(CEntity, m_nModelIndex) == 0x22);
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == 0x44 && offsetof(CPhysical, m_fMass) == 0x8C);
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40 && offsetof(CPhysical, m_pAttachedTo) == 0xFC);
static_assert(offsetof(CPed, m_pTargetedObject) == 0x71C);
static_assert(offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_aWeapons) == 0x5A0);
static_assert(sizeof(CWeapon) == 0x1C);
static_assert(offsetof(CWeaponInfo, m_nFlags) == 0x18);
static_assert(offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_apPassengers) == 0x464 && offsetof(CVehicle, m_nMaxPassengers) == 0x488);
static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(offsetof(CPedIntelligence, m_FollowNodeThresholdDistance) == 0xCC);
static_assert(offsetof(CTaskComplexSequence, m_RepeatMode) == 0x30);
static_assert(sizeof(CPlayerInfo) == 0x190);
static_assert(sizeof(tUserList) == 0x24);

// 0x493FE0
OpcodeResult CRunningScript::ProcessCommands1600To1699(int32 command) {
    // 0x49401E..: cierre comun de los comandos que crean un sistema de particulas (0x494D85 y 0x4942D1): fallo -> -1; si no, indice de script y lista de limpieza de mision.
    const auto FinishCreateFx = [&](FxSystem_c* fx) -> OpcodeResult {
        if (!fx) {
            P(0).iParam = -1;
            StoreParameters(1);
            return OR_CONTINUE;
        }
        P(0).uParam = CTheScripts::AddScriptEffectSystem(fx);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PARTICLE);
        }
        StoreParameters(1);
        return OR_CONTINUE;
    };
    // CREATE_FX_SYSTEM_ON_CHAR / _ON_CAR / _ON_OBJECT (0x494C26) y sus versiones con direccion (0x494A6E, 0x494CF3 y 0x494B1C): el desplazamiento (P1, P2, P3) usa la altura del suelo si
    // P3 <= -100.0; con direccion, P4..P6 es la direccion y P7 la bandera; sin direccion la bandera es P4.
    const auto CreateFxOnEntity = [&](const char* name, CEntity* entity, bool withDirection) -> OpcodeResult {
        const float x = P(1).fParam;
        const float y = P(2).fParam;
        float       z = P(3).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y);
        }
        const CVector origin{ x, y, z };
        FxSystem_c*   fx;
        if (!withDirection) {
            if (!entity->GetRwObject()) {
                entity->CreateRwObject();
            }
            RwMatrix* const matrix = entity->GetModellingMatrix();
            if (!matrix) {
                return FinishCreateFx(nullptr);
            }
            fx = g_fxMan.CreateFxSystem(name, origin, matrix, P(4).iParam != 0);
        } else {
            const CVector direction{ P(4).fParam, P(5).fParam, P(6).fParam };
            RwMatrix      transform;
            g_fx.CreateMatFromVec(&transform, &origin, &direction);
            if (!entity->GetRwObject()) {
                entity->CreateRwObject();
            }
            RwMatrix* const matrix = entity->GetModellingMatrix();
            if (!matrix) {
                return FinishCreateFx(nullptr);
            }
            fx = g_fxMan.CreateFxSystem(name, transform, matrix, P(7).iParam != 0);
        }
        return FinishCreateFx(fx);
    };
    // Vector de rotacion en grados (P(first)..P(first+2)) a radianes, como los fmul del EXE
    const auto RotationParams = [&](size_t first) {
        return CVector{ P(first).fParam * EXE_DEG_TO_RAD, P(first + 1).fParam * EXE_DEG_TO_RAD, P(first + 2).fParam * EXE_DEG_TO_RAD };
    };
    // Cola comun de DETACH_OBJECT y DETACH_CAR (0x495554..0x49556F): x = P1 * rad, y = (P2 * -1.0) * rad, z = P3 sin escalar, bandera = P4 != 0.
    const auto DetachEntity = [&](CPhysical* entity) {
        const float y1 = P(2).fParam * EXE_MINUS_ONE;
        if (!entity || !entity->m_pAttachedTo) {
            return;
        }
        const float y = y1 * EXE_DEG_TO_RAD;
        const float x = P(1).fParam * EXE_DEG_TO_RAD;
        entity->DettachEntityFromEntity(x, y, P(3).fParam, P(4).iParam != 0);
    };

    switch (command) {
    // 0x49401E
    case COMMAND_IS_CHAR_AT_SCRIPTED_ATTRACTOR: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 index     = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_2D_EFFECT);
        bool        flag      = false;
        if (index >= 0 && index < 0x40) {
            auto* const effect = reinterpret_cast<C2dEffectPedAttractor*>(&CScripted2dEffects::ms_effects.data()[index]);
            CPed* const user   = GetPedAttractorManager()->GetPedUsingEffect(effect, nullptr);
            if (pedHandle == -1) {
                flag = user != nullptr;
            } else {
                flag = PedOf(pedHandle) == user;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x4940AD
    case COMMAND_SET_SEQUENCE_TO_REPEAT: {
        CollectParameters(2);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEQUENCE_TASK);
        if (index < 0 || index >= 0x40) {
            return OR_CONTINUE;
        }
        // el EXE escribe el dword del script tal cual en el modo de repeticion de la secuencia ACTIVA (no en la del indice)
        auto& sequence        = CTaskSequences::GetActiveSequence();
        sequence.m_RepeatMode = static_cast<decltype(sequence.m_RepeatMode)>(P(1).uParam);
        return OR_CONTINUE;
    }
    // 0x4940F3
    case COMMAND_GET_SEQUENCE_PROGRESS: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        if (CPedScriptedTaskRecord::GetStatus(ped, 0x618) == static_cast<eScriptedTaskStatus>(0)) {
            P(0).iParam = -1;
            StoreParameters(1);
            return OR_CONTINUE;
        }
        // [[ped+0x47C]+0x10] = tarea primaria (TASK_PRIMARY_PRIMARY); su campo +0x10 es el progreso de la secuencia
        auto* const task = ped->GetIntelligence()->m_TaskMgr.GetTaskPrimary(TASK_PRIMARY_PRIMARY);
        P(0).iParam      = RawField<int32>(task, 0x10);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x49415B
    case COMMAND_CLEAR_LOOK_AT: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        if (pedHandle != -1) {
            CPed* const ped = PedOf(pedHandle);
            if (g_ikChainMan.IsLooking(ped)) {
                g_ikChainMan.AbortLookAt(ped, 500);
            }
            return OR_CONTINUE;
        }
        auto* const task = new CTaskSimpleClearLookAt();
        CTaskSequences::GetActiveSequence().AddTask(task);
        return OR_CONTINUE;
    }
    // 0x4941F5
    case COMMAND_SET_FOLLOW_NODE_THRESHOLD_DISTANCE: {
        CollectParameters(2);
        CPed* const ped                                       = PedOf(P(0).iParam);
        ped->GetIntelligence()->m_FollowNodeThresholdDistance = P(1).fParam;
        return OR_CONTINUE;
    }
    // 0x494228
    case COMMAND_CREATE_FX_SYSTEM: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(4);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM); // el EXE descarta el resultado
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y);
        }
        const CVector pos{ x, y, z };
        FxSystem_c* const fx = g_fxMan.CreateFxSystem(name, pos, nullptr, P(3).iParam != 0);
        return FinishCreateFx(fx);
    }
    // 0x494323
    case COMMAND_PLAY_FX_SYSTEM: {
        CollectParameters(1);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_EFFECT_SYSTEM);
        if (index < 0) {
            return OR_CONTINUE;
        }
        FxSystem_c* const fx = CTheScripts::ScriptEffectSystemArray.data()[index].m_pFxSystem; // sin limite superior, como el EXE
        if (!fx) {
            return OR_CONTINUE;
        }
        fx->Play();
        return OR_CONTINUE;
    }
    // 0x494361
    case COMMAND_STOP_FX_SYSTEM: {
        CollectParameters(1);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_EFFECT_SYSTEM);
        if (index < 0) {
            return OR_CONTINUE;
        }
        FxSystem_c* const fx = CTheScripts::ScriptEffectSystemArray.data()[index].m_pFxSystem;
        if (!fx) {
            return OR_CONTINUE;
        }
        fx->Stop();
        return OR_CONTINUE;
    }
    // 0x49439F y 0x494402 (cierre comun 0x4943D7): PLAY_AND_KILL_FX_SYSTEM y KILL_FX_SYSTEM; despues quitan el sistema de la lista del script y de la limpieza de mision
    case COMMAND_PLAY_AND_KILL_FX_SYSTEM:
    case COMMAND_KILL_FX_SYSTEM: {
        CollectParameters(1);
        const int32 handle = P(0).iParam;
        const int32 index  = CTheScripts::GetActualScriptThingIndex(handle, SCRIPT_THING_EFFECT_SYSTEM);
        if (index < 0) {
            return OR_CONTINUE;
        }
        FxSystem_c* const fx = CTheScripts::ScriptEffectSystemArray.data()[index].m_pFxSystem;
        if (!fx) {
            return OR_CONTINUE;
        }
        if (command == COMMAND_PLAY_AND_KILL_FX_SYSTEM) {
            fx->PlayAndKill();
        } else {
            fx->Kill();
        }
        CTheScripts::RemoveScriptEffectSystem(handle);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_PARTICLE);
        }
        return OR_CONTINUE;
    }
    // 0x49443C
    case COMMAND_GET_INT_STAT: {
        CollectParameters(1);
        const float value = CStats::GetStatValue(static_cast<eStats>(P(0).u16Param));
        P(0).iParam       = static_cast<int32>(value); // _ftol
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x494470
    case COMMAND_GET_FLOAT_STAT: {
        CollectParameters(1);
        P(0).fParam = CStats::GetStatValue(static_cast<eStats>(P(0).u16Param));
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4944A1
    case COMMAND_SET_OBJECT_RENDER_SCORCHED: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            obj->m_nPhysicalFlags |= 0x20000000u; // bRenderScorched
            return OR_CONTINUE;
        }
        obj->m_nPhysicalFlags &= 0xDFFFFFFFu;
        return OR_CONTINUE;
    }
    // 0x4944E9
    case COMMAND_TASK_LOOK_AT_OBJECT: {
        CollectParameters(3);
        int32 duration = P(2).iParam; // esi
        if (duration == -1) {
            duration = 20000;
        } else if (duration == -2) {
            duration = 0x7FFFFFFF;
        }
        const int32 pedHandle = P(0).iParam; // ebx
        CObject* const obj    = ObjectOf(P(1).iParam); // edi
        if (pedHandle != -1) {
            CPed* const ped = PedOf(pedHandle);
            g_ikChainMan.LookAt(EXE_LOOK_AT_OBJECT_NAME, ped, obj, duration, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 6, true);
            return OR_CONTINUE;
        }
        auto* const task = new CTaskSimpleTriggerLookAt(obj, duration, -1, CVector{ 0.0f, 0.0f, 0.0f }, true, 0.25f, 1000, 3);
        CTaskSequences::GetActiveSequence().AddTask(task);
        return OR_CONTINUE;
    }
    // 0x4945D5
    case COMMAND_LIMIT_ANGLE: {
        CollectParameters(1);
        float angle = P(0).fParam;
        if (angle < EXE_ZERO) {
            do {
                angle += EXE_360;
            } while (angle < EXE_ZERO);
        }
        while (angle > EXE_360) {
            angle -= EXE_360;
        }
        P(0).fParam = angle;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x49467B
    case COMMAND_OPEN_CAR_DOOR: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const int32     door = P(1).iParam;
        const int32     node = static_cast<int32>(CDamageManager::GetCarNodeIndexFromDoor(static_cast<eDoors>(door)));
        if (veh->IsDoorMissing(static_cast<eDoors>(door))) { // ranura 0x98 de la vtable
            return OR_CONTINUE;
        }
        if (!RawField<void*>(veh, 0x648 + node * 4)) { // m_aCarNodes[node] (desplazamiento 0x648), sin comprobar el indice
            return OR_CONTINUE;
        }
        veh->OpenDoor(nullptr, node, static_cast<eDoors>(door), 1.0f, true); // ranura 0x6C de la vtable
        return OR_CONTINUE;
    }
    // 0x4946E4
    case COMMAND_GET_PICKUP_COORDINATES: {
        CollectParameters(1);
        const int32 index = CPickups::GetActualPickupIndex(tPickupReference(P(0).iParam));
        CVector     pos{ 0.0f, 0.0f, -100.0f };
        if (index != -1) {
            pos = CPickups::aPickUps.data()[index].GetPosn();
        }
        P(0).fParam = pos.x;
        P(1).fParam = pos.y;
        P(2).fParam = pos.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x494767
    case COMMAND_REMOVE_DECISION_MAKER: {
        CollectParameters(1);
        const int32 handle = P(0).iParam;
        const int32 index  = CTheScripts::GetActualScriptThingIndex(handle, SCRIPT_THING_DECISION_MAKER);
        if (index >= 0 && index < 0x14) {
            if (CTheScripts::ScriptResourceManager.RemoveFromResourceManager(handle, RESOURCE_TYPE_DECISION_MAKER, this)) {
                CDecisionMakerTypesFileLoader::UnloadDecisionMaker(static_cast<eDecisionTypes>(index));
            }
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
        }
        return OR_CONTINUE;
    }
    // 0x4947C9 (VIEW_INTEGER_VARIABLE y WATCH_FLOAT_VARIABLE), 0x4947E5 (VIEW_FLOAT_VARIABLE) y 0x494801 (WATCH_INTEGER_VARIABLE): consumen la variable global y un texto de 40 caracteres
    case COMMAND_VIEW_INTEGER_VARIABLE:
    case COMMAND_VIEW_FLOAT_VARIABLE:
    case COMMAND_WATCH_INTEGER_VARIABLE:
    case COMMAND_WATCH_FLOAT_VARIABLE: {
        GetIndexOfGlobalVariable();
        char text[40];
        ReadTextLabelFromScript(text, 40);
        return OR_CONTINUE;
    }
    // 0x4947EC (BREAKPOINT) y 0x494808 (WRITE_DEBUG): solo el texto de 40 caracteres
    case COMMAND_BREAKPOINT:
    case COMMAND_WRITE_DEBUG: {
        char text[40];
        ReadTextLabelFromScript(text, 40);
        return OR_CONTINUE;
    }
    // 0x49481D (WRITE_DEBUG_WITH_INT) y 0x49483B (WRITE_DEBUG_WITH_FLOAT): el texto y un parametro
    case COMMAND_WRITE_DEBUG_WITH_INT:
    case COMMAND_WRITE_DEBUG_WITH_FLOAT: {
        char text[40];
        ReadTextLabelFromScript(text, 40);
        CollectParameters(1);
        return OR_CONTINUE;
    }
    // 0x494844
    case COMMAND_GET_CHAR_MODEL: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        P(0).iParam     = static_cast<int32>(static_cast<int16>(ped->m_nModelIndex)); // movsx word
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x494868
    case COMMAND_TASK_AIM_GUN_AT_COORD: {
        CollectParameters(5);
        const CVector target{ P(1).fParam, P(2).fParam, P(3).fParam };
        const int32   pedHandle = P(0).iParam;
        auto* const   task      = new CTaskSimpleGunControl(nullptr, target, CVector{}, eGunCommand::NONE, 500, P(4).iParam);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x4948EF
    case COMMAND_TASK_SHOOT_AT_COORD: {
        CollectParameters(5);
        const int32   pedHandle = P(0).iParam;
        const CVector target{ P(1).fParam, P(2).fParam, P(3).fParam };
        if (pedHandle != -1) {
            CPed* const ped    = PedOf(pedHandle);
            const auto  type   = ped->m_aWeapons.data()[static_cast<int8>(ped->m_nActiveWeaponSlot)].m_Type; // movsx byte
            auto* const winfo  = CWeaponInfo::GetWeaponInfo(type, static_cast<eWeaponSkill>(1));
            if ((winfo->m_nFlags >> 8) & 1) { // bThrow
                auto* const task = new CTaskSimpleThrowControl(nullptr, &target);
                GivePedScriptedTask(pedHandle, task, command);
                return OR_CONTINUE;
            }
        }
        auto* const task = new CTaskSimpleGunControl(nullptr, target, CVector{}, eGunCommand::FIREBURST, 5, P(4).iParam);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x4949F9
    case COMMAND_CREATE_FX_SYSTEM_ON_CHAR: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(5);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM); // el EXE descarta el resultado
        CPed* const ped = PedOf(P(0).iParam);
        return CreateFxOnEntity(name, ped, false);
    }
    // 0x494A6E
    case COMMAND_CREATE_FX_SYSTEM_ON_CHAR_WITH_DIRECTION: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(8);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM);
        CPed* const ped = PedOf(P(0).iParam);
        return CreateFxOnEntity(name, ped, true);
    }
    // 0x494B70
    case COMMAND_CREATE_FX_SYSTEM_ON_CAR: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(5);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        return CreateFxOnEntity(name, veh, false);
    }
    // 0x494BAC
    case COMMAND_CREATE_FX_SYSTEM_ON_CAR_WITH_DIRECTION: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(8);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        return CreateFxOnEntity(name, veh, true);
    }
    // 0x494BEC
    case COMMAND_CREATE_FX_SYSTEM_ON_OBJECT: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(5);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM);
        CObject* const obj = ObjectOf(P(0).iParam);
        return CreateFxOnEntity(name, obj, false);
    }
    // 0x494CB8
    case COMMAND_CREATE_FX_SYSTEM_ON_OBJECT_WITH_DIRECTION: {
        char name[32];
        ReadTextLabelFromScript(name, 32);
        CollectParameters(8);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_EFFECT_SYSTEM);
        CObject* const obj = ObjectOf(P(0).iParam);
        return CreateFxOnEntity(name, obj, true);
    }
    // 0x494DC6
    case COMMAND_TASK_DESTROY_CAR: {
        CollectParameters(2);
        const int32     pedHandle = P(0).iParam;
        CVehicle* const veh       = VehicleOf(P(1).iParam);
        auto* const     task      = new CTaskComplexDestroyCar(veh, 0, 0, 0);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x494E34
    case COMMAND_TASK_DIVE_AND_GET_UP: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        CVector     direction{ P(1).fParam, P(2).fParam, 0.0f };
        direction.Normalise();
        auto* const task = new CTaskComplexEvasiveDiveAndGetUp(nullptr, P(3).iParam, direction, true);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x494EA4
    case COMMAND_CUSTOM_PLATE_FOR_NEXT_CAR: {
        CollectParameters(1);
        char plate[9];
        ReadTextLabelFromScript(plate, 9);
        for (int32 i = 0; i < 8; i++) {
            const char c = plate[i];
            if (c == '_' || c == '\0') {
                plate[i] = ' ';
            }
        }
        CBaseModelInfo* const mi = CModelInfo::ms_modelInfoPtrs[P(0).iParam];
        plate[8]                 = '\0';
        if (!mi) {
            return OR_CONTINUE;
        }
        if (mi->GetModelType() != MODEL_INFO_VEHICLE) {
            return OR_CONTINUE;
        }
        auto* const vmi = static_cast<CVehicleModelInfo*>(mi);
        if (!vmi->GetVehicleStruct()) { // [mi+0x24]
            return OR_CONTINUE;
        }
        vmi->SetCustomCarPlateText(plate);
        return OR_CONTINUE;
    }
    // 0x494F1D
    case COMMAND_TASK_SHUFFLE_TO_NEXT_CAR_SEAT: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(1).iParam);
        auto* const     task = new CTaskComplexShuffleSeats(veh);
        GivePedScriptedTask(P(0).iParam, task, command);
        return OR_CONTINUE;
    }
    // 0x494F85
    case COMMAND_TASK_CHAT_WITH_CHAR: {
        CollectParameters(4);
        const bool  leadSpeaker = P(2).iParam != 0;
        const bool  noUpdates   = P(3).iParam != 0;
        const int32 pedHandle   = P(0).iParam;
        CPed* const partner     = PedOf(P(1).iParam);
        const int8  updateDirectionCount = noUpdates ? static_cast<int8>(-1) : static_cast<int8>(4);
        auto* const task = new CTaskComplexPartnerChat(EXE_CHAT_PARTNER_NAME, partner, leadSpeaker, 0.5f, updateDirectionCount, true, true, CVector{ 0.0f, 0.0f, 0.0f });
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x495066
    case COMMAND_ATTACH_CAMERA_TO_VEHICLE: {
        CollectParameters(9);
        CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector rotation{ P(4).fParam, P(5).fParam, P(6).fParam };
        const float tilt = P(7).fParam;
        const auto  sw   = static_cast<eSwitchType>(P(8).u16Param);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        TheCamera.TakeControlAttachToEntity(nullptr, veh, &offset, &rotation, tilt, sw, 1);
        return OR_CONTINUE;
    }
    // 0x495104
    case COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_VEHICLE: {
        CollectParameters(7);
        CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector lookAt{}; // el EXE pasa un buffer de pila sin inicializar (no se lee con objetivo)
        const float tilt = P(5).fParam;
        const auto  sw   = static_cast<eSwitchType>(P(6).u16Param);
        CVehicle* const attached = VehicleOf(P(0).iParam);
        CVehicle* const target   = VehicleOf(P(4).iParam);
        TheCamera.TakeControlAttachToEntity(target, attached, &offset, &lookAt, tilt, sw, 1);
        return OR_CONTINUE;
    }
    // 0x495187
    case COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_CHAR: {
        CollectParameters(7);
        CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector lookAt{};
        const float tilt = P(5).fParam;
        const auto  sw   = static_cast<eSwitchType>(P(6).u16Param);
        CVehicle* const attached = VehicleOf(P(0).iParam);
        CPed* const     target   = PedOf(P(4).iParam);
        TheCamera.TakeControlAttachToEntity(target, attached, &offset, &lookAt, tilt, sw, 1);
        return OR_CONTINUE;
    }
    // 0x49520C
    case COMMAND_ATTACH_CAMERA_TO_CHAR: {
        CollectParameters(9);
        CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector rotation{ P(4).fParam, P(5).fParam, P(6).fParam };
        const float tilt = P(7).fParam;
        const auto  sw   = static_cast<eSwitchType>(P(8).u16Param);
        CPed* const ped  = PedOf(P(0).iParam);
        TheCamera.TakeControlAttachToEntity(nullptr, ped, &offset, &rotation, tilt, sw, 1);
        return OR_CONTINUE;
    }
    // 0x4952A9
    case COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_VEHICLE: {
        CollectParameters(7);
        CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector lookAt{};
        const float tilt = P(5).fParam;
        const auto  sw   = static_cast<eSwitchType>(P(6).u16Param);
        CPed* const     attached = PedOf(P(0).iParam);
        CVehicle* const target   = VehicleOf(P(4).iParam);
        TheCamera.TakeControlAttachToEntity(target, attached, &offset, &lookAt, tilt, sw, 1);
        return OR_CONTINUE;
    }
    // 0x49531B
    case COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_CHAR: {
        CollectParameters(7);
        CVector offset{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector lookAt{};
        const float tilt = P(5).fParam;
        const auto  sw   = static_cast<eSwitchType>(P(6).u16Param);
        CPed* const attached = PedOf(P(0).iParam);
        CPed* const target   = PedOf(P(4).iParam);
        TheCamera.TakeControlAttachToEntity(target, attached, &offset, &lookAt, tilt, sw, 1);
        return OR_CONTINUE;
    }
    // 0x49539D
    case COMMAND_FORCE_CAR_LIGHTS: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        // byte +0x4A8: sustituye los bits 3 y 4 (m_nOverrideLights) por (P1 << 3) & 0x18
        uint8&      flags = RawField<uint8>(veh, 0x4A8);
        const uint8 old   = flags;
        const uint8 mask  = static_cast<uint8>((static_cast<uint8>(P(1).uParam) << 3) ^ old) & 0x18;
        flags             = old ^ mask;
        return OR_CONTINUE;
    }
    // 0x4953DD
    case COMMAND_ADD_PEDTYPE_AS_ATTRACTOR_USER: {
        CollectParameters(2);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_2D_EFFECT);
        if (index < 0 || index >= 0x40) {
            return OR_CONTINUE;
        }
        CScripted2dEffects::ms_userLists.data()[index].AddPedType(P(1).iParam);
        return OR_CONTINUE;
    }
    // 0x495425
    case COMMAND_ATTACH_OBJECT_TO_CAR: {
        CollectParameters(8);
        CObject* const  obj = ObjectOf(P(0).iParam);
        CVehicle* const veh = VehicleOf(P(1).iParam);
        const CVector   rot = RotationParams(5);
        const CVector   offset{ P(2).fParam, P(3).fParam, P(4).fParam };
        obj->AttachEntityToEntity(veh, offset, rot);
        return OR_CONTINUE;
    }
    // 0x4954E2
    case COMMAND_DETACH_OBJECT: {
        CollectParameters(5);
        CObject* const obj = ObjectOf(P(0).iParam);
        DetachEntity(obj);
        return OR_CONTINUE;
    }
    // 0x49557B
    case COMMAND_ATTACH_CAR_TO_CAR: {
        CollectParameters(8);
        CVehicle* const veh1   = VehicleOf(P(0).iParam);
        CVehicle* const veh2   = VehicleOf(P(1).iParam);
        const float     offX   = P(2).fParam;
        const CVector   offset{ offX, P(3).fParam, P(4).fParam };
        const CVector   rot = RotationParams(5);
        if (offX <= EXE_ATTACH_SENTINEL) { // fcomp + test ah,41h + jp: con NaN no se toma
            veh1->AttachEntityToEntity(veh2, static_cast<CVector*>(nullptr), static_cast<CQuaternion*>(nullptr));
            return OR_CONTINUE;
        }
        veh1->AttachEntityToEntity(veh2, offset, rot);
        return OR_CONTINUE;
    }
    // 0x495657
    case COMMAND_DETACH_CAR: {
        CollectParameters(5);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        DetachEntity(veh);
        return OR_CONTINUE;
    }
    // 0x4956D3
    case COMMAND_IS_OBJECT_ATTACHED: {
        CollectParameters(1);
        CObject* const obj  = ObjectOf(P(0).iParam);
        bool           flag = false;
        if (obj && obj->m_pAttachedTo) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x495719
    case COMMAND_IS_VEHICLE_ATTACHED: {
        CollectParameters(1);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        bool            flag = false;
        if (veh && veh->m_pAttachedTo) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x49575F
    case COMMAND_CLEAR_CHAR_TASKS: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        ped->GetIntelligence()->ClearTasks(true, true);
        return OR_CONTINUE;
    }
    // 0x49578F
    case COMMAND_TASK_TOGGLE_PED_THREAT_SCANNER: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        if (pedHandle != -1) {
            CPed* const ped = PedOf(pedHandle);
            // tarea de pila (marco SEH): se procesa con CTaskSimpleTogglePedThreatScanner::ProcessPed (0x6337A0) y se destruye
            CTaskSimpleTogglePedThreatScanner task{ P(1).iParam != 0, P(2).iParam != 0, P(3).iParam != 0 };
            task.CTaskSimpleTogglePedThreatScanner::ProcessPed(ped);
            return OR_CONTINUE;
        }
        auto* const task = new CTaskSimpleTogglePedThreatScanner(P(1).iParam != 0, P(2).iParam != 0, P(3).iParam != 0);
        CTaskSequences::GetActiveSequence().AddTask(task);
        return OR_CONTINUE;
    }
    // 0x495882
    case COMMAND_POP_CAR_DOOR: {
        CollectParameters(3);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const int32     door = P(1).iParam;
        const bool      show = P(2).iParam != 0;
        const auto      node = CDamageManager::GetCarNodeIndexFromDoor(static_cast<eDoors>(door));
        static_cast<CAutomobile*>(veh)->PopDoor(node, static_cast<eDoors>(door), show);
        return OR_CONTINUE;
    }
    // 0x4958D2
    case COMMAND_FIX_CAR_DOOR: {
        CollectParameters(2);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const int32     door = P(1).iParam;
        const auto      node = CDamageManager::GetCarNodeIndexFromDoor(static_cast<eDoors>(door));
        static_cast<CAutomobile*>(veh)->FixDoor(static_cast<int32>(node), static_cast<eDoors>(door));
        return OR_CONTINUE;
    }
    // 0x49590E
    case COMMAND_TASK_EVERYONE_LEAVE_CAR: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (veh->m_pDriver) {
            auto* const task = new CTaskComplexLeaveAnyCar(0, true, false);
            GivePedScriptedTask(GetPedPool()->GetRef(veh->m_pDriver), task, command);
        }
        if (veh->m_nMaxPassengers == 0) {
            return OR_CONTINUE;
        }
        int32 delayBase = 0; // ebx: 0x1F4 * indice
        for (int32 i = 0; i < static_cast<int32>(veh->m_nMaxPassengers); i++, delayBase += 0x1F4) {
            CPed* const passenger = veh->m_apPassengers.data()[i];
            if (!passenger) {
                continue;
            }
            const int32 delay = CGeneral::GetRandomNumberInRange<int32>(-250, 250) + delayBase + 0x1F4;
            auto* const task  = new CTaskComplexLeaveAnyCar(delay, true, false);
            GivePedScriptedTask(GetPedPool()->GetRef(passenger), task, command);
        }
        return OR_CONTINUE;
    }
    // 0x495A51
    case COMMAND_IS_PLAYER_TARGETTING_ANYTHING: {
        CollectParameters(1);
        CPlayerInfo& info = CWorld::Players[P(0).iParam];
        UpdateCompareFlag(info.m_pPed->m_pTargetedObject != nullptr);
        return OR_CONTINUE;
    }
    // 0x495A7D
    case COMMAND_GET_ACTIVE_CAMERA_COORDINATES: {
        const CVector source = TheCamera.GetActiveCam().m_vecSource;
        P(0).fParam          = source.x;
        P(1).fParam          = source.y;
        P(2).fParam          = source.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x495AA1
    case COMMAND_GET_ACTIVE_CAMERA_POINT_AT: {
        auto&       cam = TheCamera.GetActiveCam();
        const float x   = cam.m_vecFront.x + cam.m_vecSource.x;
        const float y   = cam.m_vecFront.y + cam.m_vecSource.y;
        const float z   = cam.m_vecFront.z + cam.m_vecSource.z;
        P(0).fParam     = x;
        P(1).fParam     = y;
        P(2).fParam     = z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x495AF4
    case COMMAND_POP_CAR_PANEL: {
        CollectParameters(3);
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        const int32     panel = P(1).iParam;
        const bool      show  = P(2).iParam != 0;
        const auto      node  = CDamageManager::GetCarNodeIndexFromPanel(static_cast<ePanels>(panel));
        static_cast<CAutomobile*>(veh)->PopPanel(node, static_cast<ePanels>(panel), show);
        return OR_CONTINUE;
    }
    // 0x495B44
    case COMMAND_FIX_CAR_PANEL: {
        CollectParameters(2);
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        const int32     panel = P(1).iParam;
        const auto      node  = CDamageManager::GetCarNodeIndexFromPanel(static_cast<ePanels>(panel));
        static_cast<CAutomobile*>(veh)->FixPanel(node, static_cast<ePanels>(panel));
        return OR_CONTINUE;
    }
    // 0x495B80
    case COMMAND_FIX_CAR_TYRE: {
        CollectParameters(2);
        const int32     wheel = P(1).iParam;
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        static_cast<CAutomobile*>(veh)->FixTyre(static_cast<eWheels>(wheel));
        return OR_CONTINUE;
    }
    // 0x495BAF
    case COMMAND_ATTACH_OBJECT_TO_OBJECT: {
        CollectParameters(8);
        CObject* const obj1 = ObjectOf(P(0).iParam);
        CObject* const obj2 = ObjectOf(P(1).iParam);
        const CVector  rot  = RotationParams(5);
        const CVector  offset{ P(2).fParam, P(3).fParam, P(4).fParam };
        obj1->AttachEntityToEntity(obj2, offset, rot);
        return OR_CONTINUE;
    }
    // 0x495C6A
    case COMMAND_ATTACH_OBJECT_TO_CHAR: {
        CollectParameters(8);
        CObject* const obj = ObjectOf(P(0).iParam);
        CPed* const    ped = PedOf(P(1).iParam);
        const CVector  rot = RotationParams(5);
        const CVector  offset{ P(2).fParam, P(3).fParam, P(4).fParam };
        obj->AttachEntityToEntity(ped, offset, rot);
        return OR_CONTINUE;
    }
    // 0x495D27
    case COMMAND_GET_CAR_SPEED_VECTOR: {
        CollectParameters(1);
        CVehicle* const veh   = VehicleOf(P(0).iParam);
        const CVector   speed = veh->m_vecMoveSpeed;
        P(0).fParam           = speed.x * EXE_FIFTY;
        P(1).fParam           = speed.y * EXE_FIFTY;
        P(2).fParam           = speed.z * EXE_FIFTY;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x495DAD
    case COMMAND_GET_CAR_MASS: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).fParam         = veh->m_fMass;
        StoreParameters(1);
        return OR_CONTINUE;
    }

    // 0x495DDD: comandos que el EXE deja vacios, sin leer parametros
    case COMMAND_ADD_PEDMODEL_AS_ATTRACTOR_USER:
    case COMMAND_PAUSE_FX_SYSTEM:
    case COMMAND_CREATE_FX_SYSTEM_WITH_DIRECTION:
    case COMMAND_SET_ATTRACTOR_PAIR:
    case COMMAND_PLACE_CHAR_AT_ATTRACTOR:
    case COMMAND_IS_CHAR_TOUCHING_ANY_OBJECT:
    case COMMAND_ADD_QUEUED_DIALOGUE:
    case COMMAND_IS_DIALOGUE_FINISHED:
    case COMMAND_IS_DIALOGUE_PLAYING:
    case COMMAND_CREATE_PED_GENERATOR_AT_ATTRACTOR:
    case COMMAND_GET_CHAR_AT_SCRIPTED_ATTRACTOR:
    case COMMAND_GET_CLOSEST_BUYABLE_OBJECT_TO_PLAYER:
    case COMMAND_ATTACH_CAMERA_TO_OBJECT:
    case COMMAND_ATTACH_CAMERA_TO_OBJECT_LOOK_AT_VEHICLE:
    case COMMAND_ATTACH_CAMERA_TO_OBJECT_LOOK_AT_CHAR:
    case COMMAND_ATTACH_CAMERA_TO_OBJECT_LOOK_AT_OBJECT:
    case COMMAND_ATTACH_CAMERA_TO_CHAR_LOOK_AT_OBJECT:
    case COMMAND_ATTACH_CAMERA_TO_VEHICLE_LOOK_AT_OBJECT:
        return OR_CONTINUE;

    // 0x495DE1: sin caso en el EXE (1604 1605 1609 1624 1680-1686), 1600, 1601 y fuera de rango
    default:
        return OR_INTERRUPT;
    }
}
