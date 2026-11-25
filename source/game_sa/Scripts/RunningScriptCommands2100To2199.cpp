#include "StdInc.h"
#include "SilentPatchScripts.h"
#include "extensions/SilentPatchConfig.h"

#include <bit>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "PedGroups.h"
#include "World.h"
#include "PlayerInfo.h"
#include "PedIntelligence.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Radar.h"
#include "Camera.h"
#include "Cam.h"
#include "Clock.h"
#include "Pad.h"
#include "TheZones.h"
#include "Zone.h"
#include "ZoneInfo.h"
#include "TimeCycle.h"
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "Interior/InteriorManager_c.h"
#include "GangWars.h"
#include "Shopping.h"
#include "Core/KeyGen.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "Birds.h"
#include "CarCtrl.h"
#include "VehicleRecording.h"
#include "PedClothesDesc.h"
#include "Fx/FxSystem.h"
#include "ScriptsForBrains.h"
#include "StreamedScripts.h"
#include "RwHelper.h"
#include "Events/EventGroup.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGlobalGroup.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"
#include "Tasks/TaskTypes/TaskComplexFollowPedFootsteps.h"
#include "Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h"
#include "Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h"

// F4 de codigo vivo (sin validar): 0x470A90 (5861 bytes, hasta 0x472175), CRunningScript::ProcessCommands2100To2199, el despachador original de los comandos de script 2100..2199 (ranura 21 de la tabla
// 0x8A6168; la usa ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con manejador 0x83817E): solo protege
// los `new` de tareas / eventos (el destructor de desenrollado es operator delete o el destructor del evento), en C++ no hace falta.
// Salto con una tabla directa de 100 entradas (0x472178, comando - 0x834): 30 comandos van a 0x472154 (devuelven 0 sin leer parametros: 2104 2107 2120-2124 2127 2135 2140 2141 2143 2146 2147 2154
// 2157-2160 2167 2176 2178 2181 2182 2191 2193 2194 2196-2198) y 2 (2132 2149) y los fuera de rango a 0x472158 (OR_INTERRUPT, -1). Cada `case` replica el orden de colaboradores del desensamblado; los
// parametros se leen de ScriptParams (0xA43C78). Los indices y desplazamientos que vienen del script se usan sin comprobar rangos, como el EXE.
// Las comparaciones x87 `test ah,41h; jp` tras fcom(p) saltan con "mayor" y con NaN (no se salta con menor o igual); `test ah,5; jp` salta salvo con "menor"; `test ah,44h; jp` salta salvo con "igual".
namespace {
// 0x858B38 y 0x858B40: floats 0.02 y 50.0 (0x3CA3D70A, 0x42480000)
constexpr float EXE_K_0_02 = std::bit_cast<float>(0x3CA3D70Au);
constexpr float EXE_K_50   = std::bit_cast<float>(0x42480000u);
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x859014: float -100.0 (0xC2C80000)
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x858C58: float 0.01 (0x3C23D70A); 0x859E80: float 1000.0 (0x447A0000); 0x858B8C: float 0.5 (0x3F000000)
constexpr float EXE_DRAG_THRESHOLD = std::bit_cast<float>(0x3C23D70Au);
constexpr float EXE_DRAG_DIVISOR   = std::bit_cast<float>(0x447A0000u);
constexpr float EXE_DRAG_HALF      = std::bit_cast<float>(0x3F000000u);
// 0x41200000: 10.0f que se pasa al constructor de CTaskComplexWalkAlongsidePed
constexpr float EXE_WALK_ALONGSIDE_DIST = std::bit_cast<float>(0x41200000u);
// 0x40800000: 4.0f (velocidad de mezcla) de la tarea de morir de DAMAGE_CHAR
constexpr float EXE_DIE_BLEND_DELTA = std::bit_cast<float>(0x40800000u);

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
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras o es privado)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
} // namespace

static_assert(offsetof(CPhysical, m_fAirResistance) == 0x98 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44);
static_assert(offsetof(CVehicle, m_fDirtLevel) == 0x4B0);
static_assert(offsetof(CVehicle, m_nVehicleWeaponInUse) == 0x513);
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390);
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384);
static_assert(offsetof(CObject, m_nObjectFlags) == 0x140);
static_assert(offsetof(CPed, m_fHealth) == 0x540 && offsetof(CPed, m_fArmour) == 0x548);
static_assert(offsetof(CPed, m_pVehicle) == 0x58C);
static_assert(offsetof(CPed, m_fTurretAngleA) == 0x778 && offsetof(CPed, m_fTurretAngleB) == 0x77C);
static_assert(offsetof(CPed, m_pEnex) == 0x78C);
static_assert(offsetof(CPad, bDisablePlayerFireWeapon) == 0x11B);
static_assert(offsetof(CAnimBlendAssociation, m_BlendHier) == 0x14 && offsetof(CAnimBlendAssociation, m_CurrentTime) == 0x20 && offsetof(CAnimBlendAssociation, m_Speed) == 0x24);
static_assert(offsetof(CAnimBlendHierarchy, m_fTotalTime) == 0x10);
static_assert(sizeof(CPedGroup) == 0x2D4);
static_assert(sizeof(CPlayerInfo) == 0x190);

// 0x470A90
OpcodeResult CRunningScript::ProcessCommands2100To2199(int32 command) {
    switch (command) {
    // 0x470ACE
    case COMMAND_DO_CAMERA_BUMP: {
        CollectParameters(2);
        TheCamera.GetActiveCam().DoCamBump(P(0).fParam, P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x470B03
    case COMMAND_GET_CURRENT_DATE: {
        P(0).uParam = CClock::ms_nGameClockDays;  // movzx byte [0xB70154]
        P(1).uParam = CClock::ms_nGameClockMonth; // movzx byte [0xB70155]
        { // SilentPatch (MIT, Silent): gym glitch
            int32 day = P(0).iParam, month = P(1).iParam;
            if (SilentPatchScripts::GymGlitchFixDate(m_szName, day, month)) {
                P(0).iParam = day;
                P(1).iParam = month;
            }
        }
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x470B2C
    case COMMAND_SET_OBJECT_ANIM_SPEED: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        char           animName[24];
        ReadTextLabelFromScript(animName, 24);
        CAnimBlendAssociation* const assoc = RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(obj->GetRwObject()), animName);
        CollectParameters(1);
        if (assoc) {
            assoc->m_Speed = P(0).fParam;
        }
        return OR_CONTINUE;
    }
    // 0x470B91
    case COMMAND_IS_OBJECT_PLAYING_ANIM: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        char           animName[24];
        ReadTextLabelFromScript(animName, 24);
        bool       flag     = false;
        RwObject*  rwObject = obj->GetRwObject();
        if (rwObject && RwObjectGetType(rwObject) == rpCLUMP) { // cmp byte [obj], 2
            auto* const clump = reinterpret_cast<RpClump*>(rwObject);
            if (RpAnimBlendClumpIsInitialized(clump) && RpAnimBlendClumpGetAssociation(clump, animName)) {
                flag = true;
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x470C0C
    case COMMAND_GET_OBJECT_ANIM_CURRENT_TIME: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        char           animName[24];
        ReadTextLabelFromScript(animName, 24);
        CAnimBlendAssociation* const assoc  = RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(obj->GetRwObject()), animName);
        float                        result = 0.0f;
        if (assoc) {
            result = assoc->m_CurrentTime / assoc->m_BlendHier->m_fTotalTime;
        }
        P(0).fParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x470C81
    case COMMAND_SET_OBJECT_ANIM_CURRENT_TIME: {
        CollectParameters(1);
        CObject* const obj = ObjectOf(P(0).iParam);
        char           animName[24];
        ReadTextLabelFromScript(animName, 24);
        CAnimBlendAssociation* const assoc = RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(obj->GetRwObject()), animName);
        CollectParameters(1);
        if (assoc) {
            assoc->SetCurrentTime(assoc->m_BlendHier->m_fTotalTime * P(0).fParam);
        }
        return OR_CONTINUE;
    }
    // 0x470CF4
    case COMMAND_SET_CHAR_VELOCITY: {
        CollectParameters(4);
        const CVector velocity{ P(1).fParam * EXE_K_0_02, P(2).fParam * EXE_K_0_02, P(3).fParam * EXE_K_0_02 };
        CPed* const   ped = PedOf(P(0).iParam);
        ped->SetVelocity(velocity); // 0x441130
        return OR_CONTINUE;
    }
    // 0x470D4F
    case COMMAND_GET_CHAR_VELOCITY: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        P(0).fParam     = ped->m_vecMoveSpeed.x * EXE_K_50;
        P(1).fParam     = ped->m_vecMoveSpeed.y * EXE_K_50;
        P(2).fParam     = ped->m_vecMoveSpeed.z * EXE_K_50;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x470DAA
    case COMMAND_SET_CHAR_ROTATION: {
        CollectParameters(4);
        CPed* const ped = PedOf(P(0).iParam);
        CWorld::Remove(ped);
        ped->SetOrientation(P(1).fParam * EXE_DEG_TO_RAD, P(2).fParam * EXE_DEG_TO_RAD, P(3).fParam * EXE_DEG_TO_RAD);
        CWorld::Add(ped);
        return OR_CONTINUE;
    }
    // 0x470E15
    case COMMAND_GET_CAR_UPRIGHT_VALUE: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        P(0).fParam         = veh->m_matrix->GetUp().z; // el EXE no comprueba la matriz (+0x14), z de +0x20
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x470E5B
    case COMMAND_SET_VEHICLE_AREA_VISIBLE: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        veh->SetAreaCode(static_cast<eAreaCodes>(P(1).u8Param)); // byte [veh+2Fh]
        return OR_CONTINUE;
    }
    // 0x470E86
    case COMMAND_SELECT_WEAPONS_FOR_VEHICLE: {
        CollectParameters(2);
        CVehicle* const veh        = VehicleOf(P(0).iParam);
        veh->m_nVehicleWeaponInUse = static_cast<eCarWeapon>(P(1).u8Param); // byte [veh+513h]
        return OR_CONTINUE;
    }
    // 0x470EB4
    case COMMAND_GET_CITY_PLAYER_IS_IN: {
        CollectParameters(1);
        P(0).uParam = static_cast<uint32>(CTheZones::m_CurrLevel); // dword [0xBA6718]
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x470ED9
    case COMMAND_GET_NAME_OF_ZONE: {
        CollectParameters(3);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        CZone* const  zone = CTheZones::FindSmallestZoneForPosition(pos, true);
        char* const   dest = reinterpret_cast<char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        CopyTextLabel(dest, zone->m_TextLabel, 8); // strncpy(destino, zona+8, 8)
        return OR_CONTINUE;
    }
    // 0x470F3B (variable global, 8 caracteres) y 0x470F3B de nuevo para 2118: EXE_VAR_TEXT_LABEL_EMPTY compara el primer byte con 0 (byte de 0x858B54)
    case COMMAND_IS_VAR_TEXT_LABEL_EMPTY:
    case COMMAND_IS_VAR_TEXT_LABEL16_EMPTY: {
        const char* const label = reinterpret_cast<const char*>(GetPointerToScriptVariable(VAR_GLOBAL));
        UpdateCompareFlag(label[0] == '\0');
        return OR_CONTINUE;
    }
    // 0x470F73 (variable local) -> 0x470F3D
    case COMMAND_IS_LVAR_TEXT_LABEL_EMPTY:
    case COMMAND_IS_LVAR_TEXT_LABEL16_EMPTY: {
        const char* const label = reinterpret_cast<const char*>(GetPointerToScriptVariable(VAR_LOCAL));
        UpdateCompareFlag(label[0] == '\0');
        return OR_CONTINUE;
    }
    // 0x470F77
    case COMMAND_ACTIVATE_INTERIOR_PEDS: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            g_interiorMan.ActivatePeds(true);
        } else {
            g_interiorMan.ActivatePeds(false);
        }
        return OR_CONTINUE;
    }
    // 0x470FAA
    case COMMAND_SET_VEHICLE_CAN_BE_TARGETTED: {
        CollectParameters(2);
        CVehicle* const veh                     = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bVehicleCanBeTargetted = P(1).iParam != 0; // byte [veh+42Dh], bit 2
        return OR_CONTINUE;
    }
    // 0x470FF5
    case COMMAND_TASK_FOLLOW_FOOTSTEPS: {
        CollectParameters(2);
        CPed* const target = PedOf(P(1).iParam);
        auto* const task   = new CTaskComplexFollowPedFootsteps(target);
        GivePedScriptedTask(P(0).iParam, task, command);
        return OR_CONTINUE;
    }
    // 0x47105C
    case COMMAND_DAMAGE_CHAR: {
        CollectParameters(3);
        CPed* const ped    = PedOf(P(0).iParam);
        float       damage = static_cast<float>(P(1).iParam); // fild
        if (P(2).iParam != 0) {
            // el armadura absorbe primero (con NaN: se agota y el resto es damage - armadura)
            if (damage <= ped->m_fArmour) {
                ped->m_fArmour = ped->m_fArmour - damage;
                damage         = 0.0f;
            } else {
                damage         = damage - ped->m_fArmour;
                ped->m_fArmour = 0.0f;
            }
        }
        const float health = ped->m_fHealth - damage;
        ped->m_fHealth     = health;
        if (!(health <= 0.0f)) { // fcomp 0.0 + test ah,41h + jp
            return OR_CONTINUE;
        }
        ped->m_fHealth = 0.0f;
        auto* const task = new CTaskComplexDie(
            static_cast<eWeaponType>(0),
            static_cast<AssocGroupId>(0),
            static_cast<AnimationId>(0xF),
            EXE_DIE_BLEND_DELTA,
            0.0f,
            false,
            false,
            static_cast<eDirection>(0),
            false
        );
        CEventScriptCommand event(3, task, false);
        ped->GetIntelligence()->GetEventGroup().Add(static_cast<CEvent*>(&event), false);
        return OR_CONTINUE;
    }
    // 0x471161
    case COMMAND_SET_CAR_CAN_BE_VISIBLY_DAMAGED: {
        CollectParameters(2);
        CVehicle* const veh                                                = VehicleOf(P(0).iParam);
        static_cast<CAutomobile*>(veh)->autoFlags.bCanBeVisiblyDamaged = P(1).iParam != 0; // byte [veh+868h], bit 4
        return OR_CONTINUE;
    }
    // 0x4711AB
    case COMMAND_SET_HELI_REACHED_TARGET_DISTANCE: {
        CollectParameters(2);
        CVehicle* const veh                     = VehicleOf(P(0).iParam);
        veh->m_autoPilot.m_ucHeliTargetDist2    = P(1).u8Param; // byte [veh+3DFh]
        return OR_CONTINUE;
    }
    // 0x4711D9
    case COMMAND_GET_SOUND_LEVEL_AT_COORDS: {
        CollectParameters(4);
        CPed* ped = nullptr;
        if (P(0).iParam != -1) {
            ped = PedOf(P(0).iParam);
        }
        CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        P(0).fParam = GetEventGlobalGroup()->GetSoundLevel(ped, pos); // 0x4ABA50 + 0x4AB900
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47125A
    case COMMAND_SET_CHAR_ALLOWED_TO_DUCK: {
        CollectParameters(2);
        CPed* const ped          = PedOf(P(0).iParam);
        ped->bNotAllowedToDuck   = P(1).iParam == 0; // bit 24 de +46Ch: se borra con un valor distinto de 0
        return OR_CONTINUE;
    }
    // 0x4712AB
    case COMMAND_SET_HEADING_FOR_ATTACHED_PLAYER: {
        CollectParameters(3);
        CTheScripts::fCameraHeadingWhenPlayerIsAttached     = P(1).fParam * EXE_DEG_TO_RAD; // 0xA4449C
        CTheScripts::fCameraHeadingStepWhenPlayerIsAttached = P(2).fParam * EXE_DEG_TO_RAD; // 0xA44498
        return OR_CONTINUE;
    }
    // 0x4712DF
    case COMMAND_TASK_WALK_ALONGSIDE_CHAR: {
        CollectParameters(2);
        CPed* const target = PedOf(P(1).iParam);
        auto* const task   = new CTaskComplexWalkAlongsidePed(target, EXE_WALK_ALONGSIDE_DIST);
        GivePedScriptedTask(P(0).iParam, task, command);
        return OR_CONTINUE;
    }
    // 0x47134B
    case COMMAND_CREATE_EMERGENCY_SERVICES_CAR: {
        CollectParameters(4);
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        UpdateCompareFlag(CCarCtrl::ScriptGenerateOneEmergencyServicesCar(P(0).uParam, pos));
        return OR_CONTINUE;
    }
    // 0x4713B5
    case COMMAND_TASK_KINDA_STAY_IN_SAME_PLACE: {
        CollectParameters(2);
        if (P(0).iParam != -1) {
            CPed* const ped = PedOf(P(0).iParam);
            // el EXE construye la tarea en la pila, llama a ProcessPed (0x62F660) y la destruye
            CTaskSimpleSetKindaStayInSamePlace task(P(1).iParam != 0);
            task.ProcessPed(ped);
            return OR_CONTINUE;
        }
        auto* const task = new CTaskSimpleSetKindaStayInSamePlace(P(1).iParam != 0);
        CTaskSequences::AddTaskToActiveSequence(task); // 0x632D10 sobre ms_taskSequence[ms_iActiveSequence]
        return OR_CONTINUE;
    }
    // 0x471470
    case COMMAND_START_PLAYBACK_RECORDED_CAR_LOOPED: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CVehicleRecording::StartPlaybackRecordedCar(veh, P(1).iParam, false, true);
        return OR_CONTINUE;
    }
    // 0x4714A5
    case COMMAND_SET_CHAR_AREA_VISIBLE: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        ped->SetAreaCode(static_cast<eAreaCodes>(P(1).u8Param)); // byte [ped+2Fh]
        if (P(1).iParam != 0) {
            return OR_CONTINUE;
        }
        ped->m_pEnex = nullptr; // dword [ped+78Ch]
        if (!ped->IsPlayer()) {
            return OR_CONTINUE;
        }
        CEntryExitManager::ms_entryExitStackPosn = 0; // 0x96A7C4
        CTimeCycle::StopExtraColour(false);
        return OR_CONTINUE;
    }
    // 0x471504
    case COMMAND_IS_ATTACHED_PLAYER_HEADING_ACHIEVED: {
        CollectParameters(1);
        UpdateCompareFlag(CTheScripts::fCameraHeadingStepWhenPlayerIsAttached == 0.0f); // fcomp 0.0 + test ah,44h: verdadero solo si es igual
        return OR_CONTINUE;
    }
    // 0x47152E
    case COMMAND_ENABLE_ENTRY_EXIT_PLAYER_GROUP_WARPING: {
        CollectParameters(4);
        const CVector2D pos{ P(0).fParam, P(1).fParam };
        const int32     slot = CEntryExitManager::FindNearestEntryExit(pos, P(2).fParam, -1);
        CEntryExit*     enex = CEntryExitManager::GetInSlot(slot);
        if (P(3).iParam != 0) {
            enex->m_nFlags |= 0x100; // or word [enex+30h], 100h
            CEntryExit* const link = enex->m_pLink;
            if (!link) {
                return OR_CONTINUE;
            }
            link->m_nFlags |= 0x100;
            return OR_CONTINUE;
        }
        enex->m_nFlags &= 0xFEFF;
        CEntryExit* const link = enex->m_pLink;
        if (!link) {
            return OR_CONTINUE;
        }
        link->m_nFlags &= 0xFEFF;
        return OR_CONTINUE;
    }
    // 0x4715B8
    case COMMAND_GET_CLOSEST_STEALABLE_OBJECT: {
        CollectParameters(4);
        const float   x      = P(0).fParam;
        const float   y      = P(1).fParam;
        const float   z      = P(2).fParam;
        const float   radius = P(3).fParam;
        float         best   = radius + radius; // fadd st0,st0
        const CVector point{ x, y, z };
        int16         count = 0;
        CEntity*      list[16];
        CWorld::FindObjectsInRange(point, radius, true, &count, 16, list, false, false, false, true, false);
        CEntity* found = nullptr;
        // el EXE compara el contador como `jbe` sin signo y recorre con indice sin signo
        if (static_cast<uint32>(static_cast<int32>(count)) > 0u) {
            for (uint32 i = 0; i < static_cast<uint32>(static_cast<int32>(count)); i++) {
                CEntity* const entity = list[i];
                if (entity->GetType() == ENTITY_TYPE_OBJECT && (static_cast<CObject*>(entity)->m_nObjectFlags & 0x2000)) {
                    const CVector& position = entity->GetPosition();
                    const CVector  diff{ position.x - x, position.y - y, position.z - z };
                    const float    distance = diff.Magnitude();
                    if (distance < best) { // fcom + test ah,5 + jp
                        best  = distance;
                        found = entity;
                    }
                }
            }
            if (found) {
                P(0).iParam = GetObjectPool()->GetRef(static_cast<CObject*>(found));
                StoreParameters(1);
                return OR_CONTINUE;
            }
        }
        P(0).iParam = -1;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47173A
    case COMMAND_IS_PROCEDURAL_INTERIOR_ACTIVE: {
        CollectParameters(1);
        UpdateCompareFlag(g_interiorMan.IsGroupActive(P(0).iParam) != 0);
        return OR_CONTINUE;
    }
    // 0x47175E: CLEAR_THIS_VIEW_INTEGER_VARIABLE, CLEAR_THIS_VIEW_FLOAT_VARIABLE, CLEAR_THIS_INTEGER_WATCHPOINT y CLEAR_THIS_FLOAT_WATCHPOINT: leen el indice de la variable y no hacen nada mas
    case COMMAND_CLEAR_THIS_VIEW_INTEGER_VARIABLE:
    case COMMAND_CLEAR_THIS_VIEW_FLOAT_VARIABLE:
    case COMMAND_CLEAR_THIS_INTEGER_WATCHPOINT:
    case COMMAND_CLEAR_THIS_FLOAT_WATCHPOINT: {
        (void)GetIndexOfGlobalVariable();
        return OR_CONTINUE;
    }
    // 0x47176C: SWITCH_START (0x871) y SWITCH_CONTINUED (0x872)
    case COMMAND_SWITCH_START:
    case COMMAND_SWITCH_CONTINUED: {
        uint8 chunk;
        if (command == COMMAND_SWITCH_START) {
            CollectParameters(4);
            CTheScripts::ValueToCheckInSwitchStatement        = P(0).iParam;                          // 0xA43F5C
            CTheScripts::SwitchDefaultAddress                 = P(3).iParam;                          // 0xA43F54
            CTheScripts::NumberOfEntriesStillToReadForSwitch  = static_cast<uint16>(P(1).iParam * 2); // 0xA43F60
            CTheScripts::SwitchDefaultExists                  = P(2).iParam != 0;                     // 0xA43F58
            chunk                                             = 14;
        } else {
            chunk = 18;
        }
        const uint16 chunkSize = chunk;
        CollectParameters(static_cast<int16>(chunkSize));
        if (CTheScripts::NumberOfEntriesStillToReadForSwitch > chunkSize) {
            // quedan mas entradas de las que caben en este bloque: se guardan `chunk` pares y se vuelve (jbe contra el contador sin signo)
            for (uint32 i = 0; i < chunk; i += 2) {
                CTheScripts::AddToSwitchJumpTable(P(i).iParam, P(i + 1).iParam);
            }
            const uint16 remaining = static_cast<uint16>(CTheScripts::NumberOfEntriesStillToReadForSwitch - chunkSize);
            CTheScripts::NumberOfEntriesStillToReadForSwitch = remaining;
            if (remaining != 0) {
                return OR_CONTINUE;
            }
        } else {
            // ultimo bloque: se guardan las entradas que faltan y se salta
            const uint32 remaining = CTheScripts::NumberOfEntriesStillToReadForSwitch;
            for (uint32 i = 0; i < remaining; i += 2) {
                CTheScripts::AddToSwitchJumpTable(P(i).iParam, P(i + 1).iParam);
            }
            CTheScripts::NumberOfEntriesStillToReadForSwitch = 0;
        }
        int32 address = 0;
        CTheScripts::UseSwitchJumpTable(address); // 0x4703C0
        UpdatePC(address);
        return OR_CONTINUE;
    }
    // 0x47188A
    case COMMAND_REMOVE_CAR_RECORDING: {
        CollectParameters(1);
        CVehicleRecording::RemoveRecordingFile(P(0).iParam);
        return OR_CONTINUE;
    }
    // 0x4718A9
    case COMMAND_SET_ZONE_POPULATION_RACE: {
        char label[8];
        ReadTextLabelFromScript(label, 8);
        CollectParameters(1);
        const int16 zoneIndex = CTheZones::FindZoneByLabel(label, ZONE_TYPE_INFO); // 0x572C40(etiqueta, 2)
        if (zoneIndex < 0) {
            return OR_CONTINUE;
        }
        CZoneInfo& zoneInfo = CTheZones::ZoneInfoArray[CTheZones::NavigationZoneArray[zoneIndex].m_ZoneInfoIndex];
        zoneInfo.PopRaces   = P(0).u8Param & 0xF; // solo el nibble bajo del byte +10h
        return OR_CONTINUE;
    }
    // 0x471916
    case COMMAND_SET_OBJECT_ONLY_DAMAGED_BY_PLAYER: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            obj->m_nPhysicalFlags |= 0x400000u;
        } else {
            obj->m_nPhysicalFlags &= 0xFFBFFFFFu;
        }
        return OR_CONTINUE;
    }
    // 0x47195E
    case COMMAND_CREATE_BIRDS: {
        CollectParameters(8);
        // el EXE convierte con fild los seis primeros parametros enteros en las coordenadas
        // SilentPatch (MIT, Silent): CREATE_BIRDS interpretaba las coordenadas como enteros (arregla los pajaros de "Ice Cold Killa")
        static const bool asFloat = SilentPatchOption("FixCreateBirds", true);
        const auto    C       = [&](int i) { return asFloat ? P(i).fParam : static_cast<float>(P(i).iParam); };
        const CVector start{ C(0), C(1), C(2) };
        const CVector target{ C(3), C(4), C(5) };
        CBirds::CreateNumberOfBirds(start, target, P(6).iParam, static_cast<eBirdsBiome>(P(7).iParam), false);
        return OR_CONTINUE;
    }
    // 0x471A00
    case COMMAND_SET_VEHICLE_DIRT_LEVEL: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        veh->m_fDirtLevel   = P(1).fParam;
        return OR_CONTINUE;
    }
    // 0x471A2D
    case COMMAND_SET_GANG_WARS_ACTIVE: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            CGangWars::SetGangWarsActive(true);
        } else {
            CGangWars::SetGangWarsActive(false);
        }
        return OR_CONTINUE;
    }
    // 0x471A61
    case COMMAND_IS_GANG_WAR_GOING_ON: {
        UpdateCompareFlag(CGangWars::GangWarGoingOn());
        return OR_CONTINUE;
    }
    // 0x471A70
    case COMMAND_GIVE_PLAYER_CLOTHES_OUTSIDE_SHOP: {
        CollectParameters(1);
        CPlayerInfo& info = CWorld::Players[P(0).iParam]; // imul 190h + 0xB7CD98
        char         textureName[16];
        ReadTextLabelFromScript(textureName, 16);
        char modelName[16];
        ReadTextLabelFromScript(modelName, 16);
        CollectParameters(1);
        info.m_PlayerData.m_pPedClothesDesc->SetTextureAndModel(textureName, modelName, static_cast<eClothesTexturePart>(P(0).iParam));
        CShopping::SetPlayerHasBought(CKeyGen::GetUppercaseKey(textureName));
        return OR_CONTINUE;
    }
    // 0x471AF2
    case COMMAND_CLEAR_LOADED_SHOP: {
        CShopping::RemoveLoadedShop();
        return OR_CONTINUE;
    }
    // 0x471AFE
    case COMMAND_SET_GROUP_SEQUENCE: {
        CollectParameters(2);
        const int32 group = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        if (group < 0 || group >= 8) {
            return OR_CONTINUE;
        }
        int32 sequence = -1;
        if (P(1).iParam != -1) {
            sequence = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_SEQUENCE_TASK);
        }
        // dword [0xC09BEC + grupo*2D4h] = CPedGroupIntelligence::m_TaskSeqId (privado), +0x2CC dentro de CPedGroup
        RawField<int32>(&CPedGroups::ms_groups.data()[group], 0x2CC) = sequence;
        return OR_CONTINUE;
    }
    // 0x471B4F
    case COMMAND_SET_CHAR_DROPS_WEAPONS_WHEN_DEAD: {
        CollectParameters(2);
        CPed* const ped                   = PedOf(P(0).iParam);
        ped->bDoesntDropWeaponsWhenDead   = P(1).iParam == 0; // bit 9 de +474h: se borra con un valor distinto de 0
        return OR_CONTINUE;
    }
    // 0x471BA0
    case COMMAND_SET_CHAR_NEVER_LEAVES_GROUP: {
        CollectParameters(2);
        CPed* const ped          = PedOf(P(0).iParam);
        ped->bNeverLeavesGroup   = P(1).iParam != 0; // bit 14 de +474h
        return OR_CONTINUE;
    }
    // 0x471BF1
    case COMMAND_SET_PLAYER_FIRE_BUTTON: {
        CollectParameters(2);
        if (P(1).iParam != 0) {
            CPad::GetPad(P(0).iParam)->bDisablePlayerFireWeapon = false;
            return OR_CONTINUE;
        }
        CPad::GetPad(P(0).iParam)->bDisablePlayerFireWeapon = true;
        return OR_CONTINUE;
    }
    // 0x471C3C
    case COMMAND_ATTACH_FX_SYSTEM_TO_CHAR_BONE: {
        CollectParameters(3);
        CPed* const ped   = PedOf(P(1).iParam);
        const int32 index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_EFFECT_SYSTEM);
        if (index >= 0) {
            FxSystem_c* const fx = CTheScripts::ScriptEffectSystemArray.data()[index].m_pFxSystem;
            if (fx) {
                fx->AttachToBone(ped, static_cast<eBoneTag>(P(2).iParam));
            }
        }
        return OR_CONTINUE;
    }
    // 0x471C8F
    case COMMAND_REGISTER_ATTRACTOR_SCRIPT_BRAIN_FOR_CODE_USE: {
        CollectParameters(1);
        P(0).iParam = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param)); // movsx
        char label[8];
        ReadTextLabelFromScript(label, 8);
        CTheScripts::ScriptsForBrains.AddNewStreamedScriptBrainForCodeUse(static_cast<int16>(P(0).u16Param), label, 5);
        return OR_CONTINUE;
    }
    // 0x471CE9
    case COMMAND_SET_HEADING_LIMIT_FOR_ATTACHED_CHAR: {
        CollectParameters(3);
        CPed* const ped        = PedOf(P(0).iParam);
        const float angleB     = P(2).fParam * EXE_DEG_TO_RAD;
        ped->m_fTurretAngleA   = P(1).u16Param; // word [ped+778h]
        ped->m_fTurretAngleB   = angleB;        // dword [ped+77Ch]
        return OR_CONTINUE;
    }
    // 0x471D2B
    case COMMAND_ADD_BLIP_FOR_DEAD_CHAR: {
        CollectParameters(1);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CRadar::GetActualBlipArrayIndex(next); // resultado descartado
        // El EXE llama con cinco argumentos (el ultimo, el nombre del script en this+8); SetEntityBlip solo usa los cuatro primeros.
        const auto blip = CRadar::SetEntityBlip(BLIP_CHAR, P(0).iParam, 1, static_cast<eBlipDisplay>(3));
        CRadar::ChangeBlipScale(blip, 3);
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x471D7A
    case COMMAND_GET_DEAD_CHAR_COORDINATES: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CVector     pos;
        if (ped->bInVehicle && ped->m_pVehicle) {
            pos = ped->m_pVehicle->GetPosition();
        } else {
            ped->GetBonePosition(&pos, static_cast<eBoneTag>(1), false); // 0x5E4280(&pos, 1, 0)
        }
        P(0).fParam = pos.x;
        P(1).fParam = pos.y;
        P(2).fParam = pos.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x471E0C
    case COMMAND_TASK_PLAY_ANIM_WITH_FLAGS: {
        PlayAnimScriptCommand(command);
        return OR_CONTINUE;
    }
    // 0x471E1B
    case COMMAND_SET_VEHICLE_AIR_RESISTANCE_MULTIPLIER: {
        CollectParameters(2);
        CVehicle* const veh        = VehicleOf(P(0).iParam);
        const float     multiplier = P(1).fParam;
        tHandlingData* const handling = veh->m_pHandlingData;
        if (!handling) {
            return OR_CONTINUE;
        }
        const float drag = handling->m_fDragMult;
        if (drag > EXE_DRAG_THRESHOLD) { // fcomp 0.01 + test ah,41h + jne: con NaN va a la otra rama
            veh->m_fAirResistance = (drag / EXE_DRAG_DIVISOR) * EXE_DRAG_HALF;
            veh->m_fAirResistance = multiplier * veh->m_fAirResistance;
            return OR_CONTINUE;
        }
        veh->m_fAirResistance = drag;
        veh->m_fAirResistance = multiplier * veh->m_fAirResistance;
        return OR_CONTINUE;
    }
    // 0x471EA5
    case COMMAND_SET_CAR_COORDINATES_NO_OFFSET: {
        CollectParameters(4);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CCarCtrl::SetCoordsOfScriptCar(veh, P(1).fParam, P(2).fParam, P(3).fParam, 0, 0);
        return OR_CONTINUE;
    }
    // 0x471EE8
    case COMMAND_SET_USES_COLLISION_OF_CLOSEST_OBJECT_OF_TYPE: {
        CollectParameters(6);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) { // fcomp -100.0 + test ah,41h + jp
            z = CWorld::FindGroundZForCoord(x, y);
        }
        const float radius = P(3).fParam;
        float       best   = radius + radius; // fadd st0,st0
        int32       model  = P(4).iParam;
        if (model < 0) {
            model = CTheScripts::UsedObjectArray.data()[-model].nModelIndex; // 0xA44B88 - id*1Ch
        }
        const CVector point{ x, y, z };
        int16         count = 0;
        CEntity*      list[16];
        CWorld::FindObjectsOfTypeInRange(static_cast<uint32>(model), point, radius, true, &count, 16, list, true, false, false, true, true);
        if (static_cast<int32>(count) <= 0) {
            return OR_CONTINUE;
        }
        CEntity* found = nullptr;
        for (int32 i = 0; i < static_cast<int32>(count); i++) {
            CEntity* const entity   = list[i];
            const CVector& position = entity->GetPosition();
            const CVector  diff{ position.x - x, position.y - y, position.z - z };
            const float    distance = diff.Magnitude();
            if (distance < best) { // fcom + test ah,5 + jp
                best  = distance;
                found = entity;
            }
        }
        if (!found) {
            return OR_CONTINUE;
        }
        if (P(5).iParam != 0) {
            found->m_nFlags |= 1u;
        } else {
            found->m_nFlags &= 0xFFFFFFFEu;
        }
        return OR_CONTINUE;
    }
    // 0x47207D
    case COMMAND_SET_TIME_ONE_DAY_FORWARD: {
        CClock::OffsetClockByADay(1);
        return OR_CONTINUE;
    }
    // 0x47208E
    case COMMAND_SET_TIMER_BEEP_COUNTDOWN_TIME: {
        const uint16 variable = GetIndexOfGlobalVariable();
        CollectParameters(1);
        CUserDisplay::OnscnTimer.SetClockBeepCountdownSecs(variable, P(0).uParam);
        return OR_CONTINUE;
    }
    // 0x4720BB
    case COMMAND_ATTACH_TRAILER_TO_CAB: {
        CollectParameters(2);
        CVehicle* const trailer = VehicleOf(P(0).iParam);
        CVehicle* const cab     = VehicleOf(P(1).iParam);
        if (trailer && cab) {
            trailer->SetTowLink(cab, true); // call [vtable+0F4h](cab, 1)
        }
        return OR_CONTINUE;
    }
    // 0x472100
    case COMMAND_IS_VEHICLE_TOUCHING_OBJECT: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CObject* const  obj = ObjectOf(P(1).iParam);
        bool            flag = false;
        if (veh && veh->GetHasCollidedWith(obj)) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }

    // 0x472154: comandos que el EXE deja vacios, sin leer parametros
    case COMMAND_SET_OBJECT_ANIM_PLAYING_FLAG:
    case COMMAND_GET_OBJECT_ANIM_TOTAL_TIME:
    case COMMAND_SWITCH:
    case COMMAND_ENDSWITCH:
    case COMMAND_CASE:
    case COMMAND_DEFAULT:
    case COMMAND_BREAK:
    case COMMAND_GET_GROUP_LEADER:
    case COMMAND_SET_WATER_CONFIGURATION:
    case COMMAND_TASK_USE_ATTRACTOR_ADVANCED:
    case COMMAND_TASK_FOLLOW_PATH_NODES_TO_COORD_SHOOTING:
    case COMMAND_START_PLAYBACK_RECORDED_CAR_USING_AI_LOOPED:
    case COMMAND_GET_MODEL_NAME_FOR_DEBUG_ONLY:
    case COMMAND_TASK_USE_NEARBY_ENTRY_EXIT:
    case COMMAND_CLEAR_ALL_VIEW_VARIABLES:
    case COMMAND_CLEAR_ALL_BREAKPOINTS:
    case COMMAND_CLEAR_ALL_WATCHPOINTS:
    case COMMAND_IS_THIS_MODEL_A_TRAIN:
    case COMMAND_GET_VEHICLE_CHAR_IS_STANDING_ON:
    case COMMAND_GET_VEHICLE_DIRT_LEVEL:
    case COMMAND_DRAW_RECT_WITH_TITLE:
    case COMMAND_SET_ATTRACTOR_RADIUS:
    case COMMAND_CONST_INT:
    case COMMAND_CONST_FLOAT:
    case COMMAND_SET_TIME_ONE_DAY_BACK:
    case COMMAND_TASK_SIT_IN_RESTAURANT:
    case COMMAND_GET_RANDOM_ATTRACTOR_ON_CLOSEST_OBJECT_OF_TYPE:
    case COMMAND_ADD_INTERESTING_ENTITY_FOR_CHAR:
    case COMMAND_CLEAR_INTERESTING_ENTITIES_FOR_CHAR:
    case COMMAND_GET_CLOSEST_ATTRACTOR:
        return OR_CONTINUE;

    // 0x472158: BLOCK_NODES_IN_AREA (2132) y FREEZE_STATE_OF_INTERIORS (2149) no tienen caso en el EXE, y los comandos fuera de rango
    default:
        return OR_INTERRUPT;
    }
}
