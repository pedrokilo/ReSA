#include "StdInc.h"

#include <bit>
#include <utility>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "PedGroups.h"
#include "FireManager.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Train.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Radar.h"
#include "Camera.h"
#include "Clock.h"
#include "GameLogic.h"
#include "Streaming.h"
#include "PathFind.h"
#include "Pad.h"
#include "Shadows.h"
#include "PedType.h"
#include "TheZones.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "Darkel.h"
#include "EntryExitManager.h"
#include "CarAI.h"
#include "StuntJumpManager.h"
#include "Ropes.h"
#include "WaterLevel.h"
#include "ModelIndices.h"
#include "Text/Text.h"
#include "X87Math.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Collision/BoundingBox.h"
#include "Collision/ColModel.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"
#include "Tasks/TaskTypes/TaskSimpleSlideToCoord.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexDrivePointRoute.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexFollowPatrolRoute.h"

// F4 de codigo vivo (sin validar): 0x472310 (8320 bytes), CRunningScript::ProcessCommands2000To2099, el despachador original de los comandos de script 2000..2099 (ranura 20 de la tabla 0x8A6168; la usa
// ProcessOneCommand cuando el comando no tiene manejador propio). Thiscall con un argumento de pila (`ret 4`; resultado en AL). Prologo SEH (marco con manejador 0x83821C, 10 estados de desenrollado): solo
// protege los `new` de tareas (el destructor de desenrollado es operator delete), en C++ no hace falta.
// Salto con una tabla directa de 100 entradas (0x474390): 24 comandos van a 0x47436E (devuelven 0 sin leer parametros: 2001 2002 2004 2012 2018 2019 2025-2029 2036 2053 2055-2057 2060 2061 2063 2067 2075 2081 2084
// 2098) y 8 (2008 2009 2050 2091-2095) y los fuera de rango a 0x474372 (OR_INTERRUPT, -1). Cada `case` replica el orden de colaboradores del desensamblado; los parametros se leen de ScriptParams (0xA43C78).
// Las comparaciones x87 con -100.0 son `z <= -100.0` (jp tras test ah,41h: con NaN no se toma). Los indices y desplazamientos que vienen del script se usan sin comprobar rangos, como el EXE.
namespace {
// 0x858B38: float 0.02 (0x3CA3D70A)
constexpr float EXE_K_0_02 = std::bit_cast<float>(0x3CA3D70Au);
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x859014: float -100.0 (0xC2C80000)
constexpr float EXE_MIN_GROUND_Z = std::bit_cast<float>(0xC2C80000u);
// 0x86FC84 y 0x86FC88: radio (0.5) y radio del estado de movimiento (2.0) de la tarea de ir a un punto
constexpr float EXE_GOTO_RADIUS           = std::bit_cast<float>(0x3F000000u);
constexpr float EXE_GOTO_MOVESTATE_RADIUS = std::bit_cast<float>(0x40000000u);
// 0x86FD08 y 0x86FD0C: floats por defecto (0.5 y 5.0) de CTaskComplexFollowPatrolRoute (se pasan como dwords en bruto)
constexpr uint32 EXE_PATROL_RADIUS        = 0x3F000000u;
constexpr uint32 EXE_PATROL_MOVESTATE_RAD = 0x40A00000u;
// 0x3E6147AE (0.22) y 0x3E3851EC (0.18): velocidad de aspas / helices que escribe SET_HELI_BLADES_FULL_SPEED
constexpr float EXE_HELI_BLADES_SPEED = std::bit_cast<float>(0x3E6147AEu);
constexpr float EXE_PLANE_PROP_SPEED  = std::bit_cast<float>(0x3E3851ECu);
// 0x859EA8: double 10.0 (base de la potencia de DISPLAY_TEXT_WITH_FLOAT)
constexpr double EXE_TEN = 10.0;
// 0x859E8C: texto que el EXE pasa como nombre de comando a CTaskComplexPartnerGreet
constexpr const char* EXE_GREET_PARTNER_NAME = "COMMAND_TASK_GREET_PARTNER";

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
// Campo por desplazamiento de un objeto del EXE (donde el miembro nombrado no esta en las cabeceras)
template<typename T>
inline T& RawField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(object) + offset);
}
} // namespace

static_assert(offsetof(CPhysical, m_fMass) == 0x8C && offsetof(CPhysical, m_fTurnMass) == 0x90);
static_assert(offsetof(CPhysical, m_vecCentreOfMass) == 0xA4);
static_assert(offsetof(CPhysical, m_vecTurnSpeed) == 0x50 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44);
static_assert(offsetof(CVehicle, m_vehicleSpecialColIndex) == 0x48B);
static_assert(offsetof(CVehicle, m_nVehicleSubType) == 0x594);
static_assert(offsetof(CVehicle, m_nHandlingFlagsIntValue) == 0x38C);
static_assert(offsetof(CVehicle, vehicleFlags) == 0x428);
static_assert(offsetof(CObject, m_nObjectFlags) == 0x140);
static_assert(offsetof(CObject, m_nColDamageEffect) == 0x144 && offsetof(CObject, m_nSpecialColResponseCase) == 0x145);
static_assert(offsetof(CPed, m_nWeaponShootingRate) == 0x719);
static_assert(offsetof(CPed, m_nWeaponSkill) == 0x72C);
static_assert(offsetof(CPed, m_nFightingStyle) == 0x72D && offsetof(CPed, m_nAllowedAttackMoves) == 0x72E);
static_assert(offsetof(CPlayerInfo, m_nBikeRearWheelCounter) == 0x104 && offsetof(CPlayerInfo, m_nBikeFrontWheelCounter) == 0x10C);
static_assert(offsetof(CAutomobile, m_fHeliRotorSpeed) == 0x84C);
static_assert(offsetof(CPlane, m_fPropSpeed) == 0x9C4);
static_assert(sizeof(tScriptText) == 0x44);
static_assert(offsetof(tScriptText, Pos) == 0x2C && offsetof(tScriptText, GXTKey) == 0x34 && offsetof(tScriptText, NumberToInsert1) == 0x3C);
static_assert(offsetof(tScriptText, DropShadowColor) == 0x1D && offsetof(tScriptText, TextEdge) == 0x22);
static_assert(sizeof(tHydraulicData) == 0x28 && offsetof(tHydraulicData, m_aWheelSuspension) == 0x18);

// 0x472310
OpcodeResult CRunningScript::ProcessCommands2000To2099(int32 command) {
    switch (command) {
    // 0x47234E
    case COMMAND_GET_CURRENT_DAY_OF_WEEK: {
        P(0).uParam = CClock::CurrentDay; // movzx byte -> dword
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x47236A
    case COMMAND_REGISTER_SCRIPT_BRAIN_FOR_CODE_USE: {
        CollectParameters(1);
        P(0).iParam = CTheScripts::StreamedScripts.GetProperIndexFromIndexUsedByScript(static_cast<int16>(P(0).u16Param)); // movsx
        char label[8];
        ReadTextLabelFromScript(label, 8);
        CTheScripts::ScriptsForBrains.AddNewStreamedScriptBrainForCodeUse(static_cast<int16>(P(0).u16Param), label, 3);
        return OR_CONTINUE;
    }
    // 0x4723C6
    case COMMAND_APPLY_FORCE_TO_CAR: {
        CollectParameters(7);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CVector         force{ P(1).fParam, P(2).fParam, P(3).fParam };
        CVector         point{ P(4).fParam, P(5).fParam, P(6).fParam };
        CVector         rotatedCom, dir, rotatedCom2, offset, cross;
        // punto += matriz * centro de masas
        point += *Multiply3x3Exe(&rotatedCom, veh->m_matrix, &veh->m_vecCentreOfMass);
        dir = force;
        dir.Normalise();
        // offset = punto - matriz * centro de masas; cruz = offset x direccion normalizada
        CVector* const sub = VectorSubtract(&offset, &point, Multiply3x3Exe(&rotatedCom2, veh->m_matrix, &veh->m_vecCentreOfMass));
        CVector* const crs = CrossProduct(&cross, sub, &dir);
        // fuerza *= 1 / (1/masa + |cruz|^2 / masaDeGiro)
        const float magSq = crs->SquaredMagnitudeExe();
        const float k     = 1.0f / (1.0f / veh->m_fMass + magSq / veh->m_fTurnMass);
        force *= k;
        veh->ApplyForce(force, point, true);
        return OR_CONTINUE;
    }
    // 0x472510
    case COMMAND_IS_INT_LVAR_EQUAL_TO_INT_VAR: {
        auto* const lvar = GetPointerToScriptVariable(VAR_LOCAL);
        auto* const var  = GetPointerToScriptVariable(VAR_GLOBAL);
        UpdateCompareFlag(var->iParam == lvar->iParam);
        return OR_CONTINUE;
    }
    // 0x47253A
    case COMMAND_IS_FLOAT_LVAR_EQUAL_TO_FLOAT_VAR: {
        auto* const lvar = GetPointerToScriptVariable(VAR_LOCAL);
        auto* const var  = GetPointerToScriptVariable(VAR_GLOBAL);
        UpdateCompareFlag(var->fParam == lvar->fParam); // fcomp + test ah,44h: verdadero solo si son iguales (con NaN, falso)
        return OR_CONTINUE;
    }
    // 0x472576
    case COMMAND_ADD_TO_CAR_ROTATION_VELOCITY: {
        CollectParameters(4);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        const CVector   local{ P(1).fParam * EXE_K_0_02, P(2).fParam * EXE_K_0_02, P(3).fParam * EXE_K_0_02 };
        CVector         cur = veh->m_vecTurnSpeed;
        CVector         rotated;
        const CVector&  rot = *Multiply3x3Exe(&rotated, veh->m_matrix, &local);
        cur.x               = rot.x + cur.x;
        cur.y               = rot.y + cur.y;
        cur.z               = rot.z + cur.z;
        if (veh->m_bIsStatic) { // test byte [esi+1Ch], 4
            veh->SetIsStatic(false);
            veh->AddToMovingList();
        }
        veh->m_vecTurnSpeed = cur;
        return OR_CONTINUE;
    }
    // 0x472677
    case COMMAND_SET_CAR_ROTATION_VELOCITY: {
        CollectParameters(4);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        const float     step = CTimer::ms_fTimeStep;
        const CVector   local{ step * P(1).fParam, step * P(2).fParam, step * P(3).fParam };
        CVector         rotated;
        const CVector   rot = *Multiply3x3Exe(&rotated, veh->m_matrix, &local);
        if (veh->m_bIsStatic) {
            veh->SetIsStatic(false);
            veh->AddToMovingList();
        }
        // CPhysical::SetTurnSpeed(x, y, z) (0x45AFB0): copia los tres dwords a la velocidad de giro
        veh->m_vecTurnSpeed = CVector{ rot.x * EXE_K_0_02, rot.y * EXE_K_0_02, rot.z * EXE_K_0_02 };
        return OR_CONTINUE;
    }
    // 0x472759
    case COMMAND_SET_CHAR_SHOOT_RATE: {
        CollectParameters(2);
        CPed* const ped            = PedOf(P(0).iParam);
        ped->m_nWeaponShootingRate = P(1).u8Param;
        return OR_CONTINUE;
    }
    // 0x472786
    case COMMAND_IS_MODEL_IN_CDIMAGE: {
        CollectParameters(1);
        const int32 model = ScriptModelIndex(P(0).iParam);
        UpdateCompareFlag(CStreaming::IsObjectInCdImage(model));
        return OR_CONTINUE;
    }
    // 0x4727B4
    case COMMAND_REMOVE_OIL_PUDDLES_IN_AREA: {
        CollectParameters(4);
        float x1 = P(0).fParam; // [esp+14h]
        float x2 = P(2).fParam; // [esp+18h]
        float y1 = P(1).fParam; // [esp+20h]
        float y2 = P(3).fParam; // [esp+10h]
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        CShadows::RemoveOilInArea(x1, x2, y1, y2);
        return OR_CONTINUE;
    }
    // 0x472845
    case COMMAND_SET_BLIP_AS_FRIENDLY: {
        CollectParameters(2);
        CRadar::SetBlipFriendly(P(0).uParam, P(1).u8Param != 0);
        return OR_CONTINUE;
    }
    // 0x47286C
    case COMMAND_TASK_SWIM_TO_COORD: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        CVector     pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        if (pedHandle != -1) {
            CPed* const ped = PedOf(pedHandle);
            if (ped && ped->GetIntelligence()->GetTaskSwim()) {
                ped->GetIntelligence()->GetTaskSwim()->m_vecPos = pos;
                return OR_CONTINUE;
            }
        }
        auto* const task = new CTaskSimpleSwim(&pos, nullptr);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x472932
    case COMMAND_GET_MODEL_DIMENSIONS: {
        CollectParameters(1);
        const int32   model = ScriptModelIndex(P(0).iParam);
        const auto&   box   = CModelInfo::ms_modelInfoPtrs[model]->GetColModel()->m_boundBox;
        const CVector min   = box.m_vecMin;
        const CVector max   = box.m_vecMax;
        P(0).fParam         = min.x;
        P(1).fParam         = min.y;
        P(2).fParam         = min.z;
        P(3).fParam         = max.x;
        P(4).fParam         = max.y;
        P(5).fParam         = max.z;
        StoreParameters(6);
        return OR_CONTINUE;
    }
    // 0x4729A4 y 0x472A32: COPY_CHAR_DECISION_MAKER (tipo 0) y COPY_GROUP_DECISION_MAKER (tipo 1)
    case COMMAND_COPY_CHAR_DECISION_MAKER:
    case COMMAND_COPY_GROUP_DECISION_MAKER: {
        CollectParameters(1);
        int32 source = -1;
        if (P(0).iParam != -1) {
            source = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_DECISION_MAKER);
        }
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_DECISION_MAKER); // el EXE descarta el resultado
        const int32 copy = CDecisionMakerTypes::GetInstance()->CopyDecisionMaker(source, command == COMMAND_COPY_CHAR_DECISION_MAKER ? 0 : 1, m_UsesMissionCleanup);
        P(0).iParam      = CTheScripts::GetNewUniqueScriptThingIndex(copy, SCRIPT_THING_DECISION_MAKER);
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
        }
        return OR_CONTINUE;
    }
    // 0x472ABF
    case COMMAND_TASK_DRIVE_POINT_ROUTE_ADVANCED: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        CVehicle*   veh       = nullptr;
        if (P(1).iParam >= 0) {
            veh = VehicleOf(P(1).iParam);
        }
        const float cruiseSpeed = P(2).fParam;
        const int32 mode        = P(3).iParam;
        int32       model       = P(4).iParam;
        if (model == 0) {
            model = -1;
        } else if (model == 1) {
            model = 0x19F;
        }
        const int32 drivingStyle = P(5).iParam;
        auto* const task         = new CTaskComplexDrivePointRoute(
            veh,
            StaticRef<CPointRoute>(0xC18D50), // ruta global del EXE (CTheScripts)
            cruiseSpeed,
            static_cast<uint32>(mode),
            static_cast<eModelID>(model),
            -1.0f,
            static_cast<eCarDrivingStyle>(drivingStyle)
        );
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x472B88
    case COMMAND_IS_RELATIONSHIP_SET: {
        CollectParameters(3);
        const auto   id       = static_cast<AcquaintanceId>(P(0).iParam);
        const auto   pedType1 = static_cast<ePedType>(P(1).iParam);
        const auto   pedType2 = static_cast<ePedType>(P(2).iParam);
        const uint32 mask     = CPedType::GetPedTypeAcquaintances(id, pedType1);
        const uint32 flag     = CPedType::GetPedFlag(pedType2);
        UpdateCompareFlag((mask & flag) != 0);
        return OR_CONTINUE;
    }
    // 0x472BD0
    case COMMAND_SET_CAR_ALWAYS_CREATE_SKIDS: {
        CollectParameters(2);
        CVehicle* const veh                = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bAlwaysSkidMarks = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x472C1B
    case COMMAND_GET_CITY_FROM_COORDS: {
        CollectParameters(3);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        P(0).uParam = CTheZones::GetLevelFromPosition(pos);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x472C63
    case COMMAND_HAS_OBJECT_OF_TYPE_BEEN_SMASHED: {
        CollectParameters(5);
        const float px = P(0).fParam;
        const float py = P(1).fParam;
        float       pz = P(2).fParam;
        if (pz <= EXE_MIN_GROUND_Z) { // fcom + test ah,41h + jp
            pz = CWorld::FindGroundZForCoord(px, py);
        }
        const CVector pos{ px, py, pz };
        const int32   model  = ScriptModelIndex(P(3).iParam);
        const float   radius = P(4).fParam;
        auto* const   pool   = GetObjectPool();
        bool          found  = false;
        // el EXE recorre la reserva desde el ultimo hueco al primero y comprueba `found` antes de cada objeto
        for (int32 i = static_cast<int32>(pool->GetSize()); i != 0;) {
            --i;
            if (found) {
                break;
            }
            CObject* const obj = pool->GetAt(i);
            if (obj && (obj->m_nObjectFlags & 0x400) && static_cast<int16>(obj->m_nModelIndex) == model) { // bit 10 (bIsBroken)
                CVector diff;
                VectorSubtract(&diff, &pos, &obj->GetPosition());
                if (diff.Magnitude() <= radius) {
                    found = true;
                }
            }
        }
        UpdateCompareFlag(found);
        return OR_CONTINUE;
    }
    // 0x472D93
    case COMMAND_IS_PLAYER_PERFORMING_WHEELIE: {
        CollectParameters(1);
        CPlayerInfo& info = CWorld::Players[P(0).iParam];
        bool         flag = false;
        if (info.m_pPed->bInVehicle && info.m_pPed->m_pVehicle->GetVehicleAppearance() == eVehicleAppearance::VEHICLE_APPEARANCE_BIKE && info.m_nBikeRearWheelCounter > 0) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x472DF2
    case COMMAND_IS_PLAYER_PERFORMING_STOPPIE: {
        CollectParameters(1);
        CPlayerInfo& info = CWorld::Players[P(0).iParam];
        bool         flag = false;
        if (info.m_pPed->bInVehicle && info.m_pPed->m_pVehicle->GetVehicleAppearance() == eVehicleAppearance::VEHICLE_APPEARANCE_BIKE && info.m_nBikeFrontWheelCounter > 0) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x472E4A
    case COMMAND_SET_CHECKPOINT_COORDS: {
        CollectParameters(4);
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        const int32   index = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_CHECKPOINT);
        if (index < 0) {
            return OR_CONTINUE;
        }
        CCheckpoint* const checkpoint = CTheScripts::ScriptCheckpointArray.data()[index].m_Checkpoint;
        if (!checkpoint) {
            return OR_CONTINUE;
        }
        CCheckpoints::UpdatePos(checkpoint->m_ID, pos);
        return OR_CONTINUE;
    }
    // 0x472EBC
    case COMMAND_CONTROL_CAR_HYDRAULICS: {
        CollectParameters(5);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (veh->m_vehicleSpecialColIndex < 0) {
            veh->GetSpecialColModel();
        }
        if (veh->m_vehicleSpecialColIndex <= -1) {
            return OR_CONTINUE;
        }
        float* const wheels = CVehicle::m_aSpecialHydraulicData.data()[veh->m_vehicleSpecialColIndex].m_aWheelSuspension.data();
        wheels[0]           = P(1).fParam;
        wheels[1]           = P(2).fParam;
        wheels[2]           = P(3).fParam;
        wheels[3]           = P(4).fParam;
        return OR_CONTINUE;
    }
    // 0x472F57
    case COMMAND_GET_GROUP_SIZE: {
        CollectParameters(1);
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        if (groupIndex >= 0 && groupIndex < 8) {
            auto&       membership = CPedGroups::ms_groups.data()[groupIndex].GetMembership();
            const int32 hasLeader  = membership.GetLeader() != nullptr;
            P(0).iParam            = hasLeader;
            P(1).iParam            = membership.CountMembersExcludingLeader();
            StoreParameters(2);
            return OR_CONTINUE;
        }
        P(0).iParam = 0;
        P(1).iParam = 0;
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x472FD4
    case COMMAND_SET_OBJECT_COLLISION_DAMAGE_EFFECT: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            const uint8 value = obj->m_nSpecialColResponseCase;
            if (value == 0) {
                return OR_CONTINUE;
            }
            obj->m_nColDamageEffect        = value;
            obj->m_nSpecialColResponseCase = 0;
            return OR_CONTINUE;
        }
        const uint8 value = obj->m_nColDamageEffect;
        if (value == 0) {
            return OR_CONTINUE;
        }
        obj->m_nSpecialColResponseCase = value;
        obj->m_nColDamageEffect        = 0;
        return OR_CONTINUE;
    }
    // 0x47303D
    case COMMAND_SET_CAR_FOLLOW_CAR: {
        CollectParameters(3);
        CVehicle* const target   = VehicleOf(P(1).iParam);
        CVehicle* const follower = VehicleOf(P(0).iParam);
        CCarAI::TellCarToFollowOtherCar(follower, target, P(2).fParam);
        return OR_CONTINUE;
    }
    // 0x47307F
    case COMMAND_PLAYER_ENTERED_QUARRY_CRANE: {
        CRopes::PlayerControlsCrane = eControlledCrane::QUARRY_CRANE;
        return OR_CONTINUE;
    }
    // 0x473090
    case COMMAND_PLAYER_ENTERED_LAS_VEGAS_CRANE: {
        CRopes::PlayerControlsCrane    = eControlledCrane::LAS_VEGAS_CRANE;
        CWaterLevel::m_bWaterFogScript = false;
        return OR_CONTINUE;
    }
    // 0x4730A8
    case COMMAND_SWITCH_ENTRY_EXIT: {
        char name[8];
        ReadTextLabelFromScript(name, 8);
        CollectParameters(1);
        CEntryExitManager::SetEnabledByName(name, P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x4730F4
    case COMMAND_DISPLAY_TEXT_WITH_FLOAT: {
        CollectParameters(2);
        char label[8];
        ReadTextLabelFromScript(label, 8);
        (void)TheText.Get(label); // el EXE llama a CText::Get y descarta el resultado
        {
            auto& line = CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame];
            line.Pos.x = P(0).fParam;
            line.Pos.y = P(1).fParam;
        }
        CollectParameters(2);
        const float value     = P(0).fParam;
        const int32 whole     = static_cast<int32>(value);          // _ftol
        const float precision = static_cast<float>(P(1).iParam);    // fild
        {
            auto& line = CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame];
            CopyTextLabel(line.GXTKey, label, 8);
        }
        const uint16 lineIndex = CTheScripts::NumberOfIntroTextLinesThisFrame;
        auto&        line      = CTheScripts::IntroTextLines.data()[lineIndex];
        line.NumberToInsert1   = whole;
        const double scale     = ExeCompatiblePowCpp(EXE_TEN, static_cast<double>(precision));
        line.NumberToInsert2   = static_cast<int32>(scale * (static_cast<double>(value) - static_cast<double>(whole)));
        CTheScripts::NumberOfIntroTextLinesThisFrame = static_cast<uint16>(lineIndex + 1);
        return OR_CONTINUE;
    }
    // 0x4731CE
    case COMMAND_DOES_GROUP_EXIST: {
        CollectParameters(1);
        bool        exists     = false;
        const int32 groupIndex = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        if (groupIndex >= 0 && groupIndex < 8) {
            exists = true;
        }
        UpdateCompareFlag(exists);
        return OR_CONTINUE;
    }
    // 0x47320D
    case COMMAND_GIVE_MELEE_ATTACK_TO_CHAR: {
        CollectParameters(3);
        CPed* const ped = PedOf(P(0).iParam);
        if (static_cast<uint32>(static_cast<uint8>(ped->m_nFightingStyle)) != P(1).uParam) {
            ped->m_nFightingStyle      = static_cast<eFightingStyle>(P(1).u8Param);
            ped->m_nAllowedAttackMoves = 0;
        }
        switch (static_cast<uint32>(P(2).iParam - 1)) {
        case 0: ped->m_nAllowedAttackMoves = 0; break;
        case 1: ped->m_nAllowedAttackMoves |= 1; break;
        case 2: ped->m_nAllowedAttackMoves |= 2; break;
        case 3: ped->m_nAllowedAttackMoves |= 4; break;
        case 4: ped->m_nAllowedAttackMoves |= 8; break;
        case 5: ped->m_nAllowedAttackMoves |= 0xF; break;
        default: break;
        }
        return OR_CONTINUE;
    }
    // 0x4732B1
    case COMMAND_SET_CAR_HYDRAULICS: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->AddVehicleUpgrade(static_cast<uint16>(ModelIndices::MI_HYDRAULICS));
            return OR_CONTINUE;
        }
        veh->RemoveVehicleUpgrade(static_cast<uint16>(ModelIndices::MI_HYDRAULICS));
        return OR_CONTINUE;
    }
    // 0x4732FE
    case COMMAND_IS_2PLAYER_GAME_GOING_ON: {
        UpdateCompareFlag(CGameLogic::IsCoopGameGoingOn());
        return OR_CONTINUE;
    }
    // 0x473328
    case COMMAND_GET_CAMERA_FOV: {
        P(0).fParam = TheCamera.GetActiveCam().m_fFOV;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x473350
    case COMMAND_DOES_CAR_HAVE_HYDRAULICS: {
        CollectParameters(1);
        CVehicle* const veh  = VehicleOf(P(0).iParam);
        bool            flag = false;
        if (static_cast<int32>(veh->m_nVehicleSubType) == 0 && (static_cast<uint32>(veh->m_nHandlingFlagsIntValue) & 0x20000)) {
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x4733A6
    case COMMAND_TASK_CHAR_SLIDE_TO_COORD_AND_PLAY_ANIM: {
        CollectParameters(6);
        const float   aimRotation = P(4).fParam * EXE_DEG_TO_RAD;
        const int32   pedHandle   = P(0).iParam;
        float         speed       = P(5).fParam;
        const CVector pos{ P(1).fParam, P(2).fParam, P(3).fParam };
        if (speed < 0.0f) { // fcomp 0.0 + test ah,5 + jp
            speed = std::bit_cast<float>(0x3DCCCCCDu);
        }
        char animName[24];
        ReadTextLabelFromScript(animName, 24);
        char animBlock[16];
        ReadTextLabelFromScript(animBlock, 16);
        CollectParameters(6);
        const float blendDelta = P(0).fParam;
        const int32 time       = P(5).iParam;
        uint32      flags      = 0x10;
        if (P(1).iParam != 0 || time > 0) {
            flags = 0x12;
        }
        if (P(2).iParam != 0) {
            flags |= 0x40;
        }
        if (P(3).iParam != 0) {
            flags |= 0x80;
        }
        if (P(4).iParam == 0) {
            flags |= 8;
        }
        const bool  inSequence = CTaskSequences::ms_iActiveSequence >= 0;
        auto* const sequence   = new CTaskComplexSequence();
        auto* const goTo       = new CTaskComplexGoToPointAndStandStill(static_cast<eMoveState>(4), pos, EXE_GOTO_RADIUS, EXE_GOTO_MOVESTATE_RADIUS, false, false);
        sequence->AddTask(goTo);
        CTaskSimpleSlideToCoord* slide;
        if (time > 0) {
            slide = new CTaskSimpleSlideToCoord(pos, aimRotation, speed, animName, animBlock, flags, blendDelta, inSequence, static_cast<uint32>(time));
        } else {
            slide = new CTaskSimpleSlideToCoord(pos, aimRotation, speed, animName, animBlock, flags, blendDelta, inSequence, static_cast<uint32>(-1));
        }
        sequence->AddTask(slide);
        GivePedScriptedTask(pedHandle, sequence, command);
        return OR_CONTINUE;
    }
    // 0x4735FD
    case COMMAND_GET_TOTAL_NUMBER_OF_PEDS_KILLED_BY_PLAYER: {
        CollectParameters(1);
        P(0).iParam = CDarkel::FindTotalPedsKilledByPlayer(P(0).iParam);
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x473617
    case COMMAND_GET_LEVEL_DESIGN_COORDS_FOR_OBJECT: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        CVector        out;
        const CVector  result = *obj->FindTriggerPointCoors(&out, P(1).iParam);
        P(0).fParam           = result.x;
        P(1).fParam           = result.y;
        P(2).fParam           = result.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x473661
    case COMMAND_SAVE_TEXT_LABEL_TO_DEBUG_FILE: {
        char label[16];
        ReadTextLabelFromScript(label, 16);
        return OR_CONTINUE;
    }
    // 0x473679
    case COMMAND_GET_CHAR_HIGHEST_PRIORITY_EVENT: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        P(0).uParam     = RawField<uint8>(ped->GetIntelligence(), 0xD1); // movzx byte [intel+0D1h]
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4736A6
    case COMMAND_GET_PARKING_NODE_IN_AREA: {
        CollectParameters(6);
        float x1 = P(0).fParam; // [esp+14h]
        float x2 = P(3).fParam; // [esp+18h]
        float z1 = P(2).fParam; // [esp+1Ch]
        float y1 = P(1).fParam; // [esp+20h]
        float z2 = P(5).fParam; // [esp+30h]
        float y2 = P(4).fParam; // [esp+10h]
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        if (z1 > z2) {
            std::swap(z1, z2);
        }
        const CVector result = ThePaths.FindParkingNodeInArea(x1, x2, y1, y2, z1, z2);
        P(0).fParam          = result.x;
        P(1).fParam          = result.y;
        P(2).fParam          = result.z;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x47379B
    case COMMAND_GET_CAR_CHAR_IS_USING: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CVehicle*   veh = nullptr;
        if (ped->bInVehicle) {
            veh = ped->m_pVehicle;
        } else {
            auto&  tasks = ped->GetTaskManager();
            CTask* task  = tasks.FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER); // 0x2BD
            if (!task) {
                task = tasks.FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER); // 0x2BC
            }
            if (task) {
                veh = static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar();
            }
        }
        if (veh) {
            P(0).iParam = GetVehiclePool()->GetRef(veh);
        } else {
            P(0).iParam = -1;
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x473831
    case COMMAND_TASK_PLAY_ANIM_NON_INTERRUPTABLE: {
        PlayAnimScriptCommand(command);
        return OR_CONTINUE;
    }
    // 0x473840
    case COMMAND_ADD_STUNT_JUMP: {
        CollectParameters(16);
        const CVector c1{ P(0).fParam, P(1).fParam, P(2).fParam };
        const CVector h1{ P(3).fParam, P(4).fParam, P(5).fParam };
        CBoundingBox  startBox;
        startBox.SetMinMax(CVector{ c1.x - h1.x, c1.y - h1.y, c1.z - h1.z }, CVector{ h1.x + c1.x, h1.y + c1.y, h1.z + c1.z });
        const CVector c2{ P(6).fParam, P(7).fParam, P(8).fParam };
        const CVector h2{ P(9).fParam, P(10).fParam, P(11).fParam };
        CBoundingBox  endBox;
        endBox.SetMinMax(CVector{ c2.x - h2.x, c2.y - h2.y, c2.z - h2.z }, CVector{ h2.x + c2.x, h2.y + c2.y, h2.z + c2.z });
        const CVector cameraPos{ P(12).fParam, P(13).fParam, P(14).fParam };
        CStuntJumpManager::AddOne(startBox, endBox, cameraPos, P(15).iParam);
        return OR_CONTINUE;
    }
    // 0x473A01
    case COMMAND_SET_OBJECT_COORDINATES_AND_VELOCITY: {
        CollectParameters(4);
        CObject* const obj = ObjectOf(P(0).iParam);
        const float    px  = P(1).fParam;
        const float    py  = P(2).fParam;
        float          pz  = P(3).fParam;
        if (pz <= EXE_MIN_GROUND_Z) {
            pz = CWorld::FindGroundZForCoord(px, py);
        }
        const CVector pos{ px, py, pz };
        CVector       diff;
        VectorSubtract(&diff, &pos, &obj->GetPosition());
        const float timeStep = CTimer::ms_fTimeStep;
        obj->m_vecMoveSpeed  = CVector{ diff.x / timeStep, diff.y / timeStep, diff.z / timeStep };
        // recorte de cada componente a [-1, 1] (la comparacion ordenada deja intacto un NaN)
        float* const speed = &obj->m_vecMoveSpeed.x;
        for (int i = 0; i < 3; i++) {
            if (speed[i] < -1.0f) {
                speed[i] = -1.0f;
            }
            if (speed[i] > 1.0f) {
                speed[i] = 1.0f;
            }
        }
        MarkSurroundingEntitiesForCollisionWithTrain(pos, 25.0f, obj, false);
        obj->Teleport(pos, false);
        CTheScripts::ClearSpaceForMissionEntity(pos, obj);
        return OR_CONTINUE;
    }
    // 0x473BF5
    case COMMAND_SET_CHAR_KINDA_STAY_IN_SAME_PLACE: {
        CollectParameters(2);
        CPed* const ped            = PedOf(P(0).iParam);
        ped->bKindaStayInSamePlace = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x473C38
    case COMMAND_TASK_FOLLOW_PATROL_ROUTE: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam;
        const int32 moveState = P(1).iParam;
        const int32 mode      = P(2).iParam;
        auto* const memory    = static_cast<CTaskComplexFollowPatrolRoute*>(CTask::operator new(0x30));
        CTaskComplexFollowPatrolRoute* task = nullptr;
        if (memory) {
            task = CTaskComplexFollowPatrolRouteConstructorExe(
                memory,
                nullptr,
                moveState,
                reinterpret_cast<const CPatrolRoute*>(0xC18DB8), // ruta de patrulla global del EXE (CTheScripts)
                mode,
                EXE_PATROL_RADIUS,
                EXE_PATROL_MOVESTATE_RAD
            );
        }
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x473CB3
    case COMMAND_IS_CHAR_IN_AIR: {
        CollectParameters(1);
        CPed* const  ped    = PedOf(P(0).iParam);
        bool         flag   = false;
        const uint32 flags1 = RawField<uint32>(ped, 0x46C);
        if (flags1 & 0x200) { // bIsInTheAir
            flag = true;
        } else if (!(flags1 & 1)) { // bIsStanding
            const uint32 flags2 = RawField<uint32>(ped, 0x470);
            if ((flags2 & 0x10) || static_cast<int32>(flags2) < 0) {
                CTask* task = ped->GetTaskManager().GetSimplestActiveTask();
                if (task) {
                    task = ped->GetTaskManager().GetSimplestActiveTask();
                    if (task->GetTaskType() == static_cast<eTaskType>(0xCF)) {
                        flag = true;
                    }
                }
            }
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }
    // 0x473D38
    case COMMAND_GET_CHAR_HEIGHT_ABOVE_GROUND: {
        CollectParameters(1);
        CPed* const   ped     = PedOf(P(0).iParam);
        const CVector pos     = ped->GetPosition();
        const float   groundZ = CWorld::FindGroundZFor3DCoord(pos, nullptr, nullptr);
        P(0).fParam           = pos.z - groundZ;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x473DA3
    case COMMAND_SET_CHAR_WEAPON_SKILL: {
        CollectParameters(2);
        CPed* const ped     = PedOf(P(0).iParam);
        ped->m_nWeaponSkill = static_cast<eWeaponSkill>(P(1).u8Param);
        return OR_CONTINUE;
    }
    // 0x473DD1
    case COMMAND_SET_TEXT_EDGE: {
        CollectParameters(5);
        const CRGBA color{ P(1).u8Param, P(2).u8Param, P(3).u8Param, P(4).u8Param };
        auto&       line = CTheScripts::IntroTextLines.data()[CTheScripts::NumberOfIntroTextLinesThisFrame];
        line.DropShadowColor = color;
        line.TextEdge        = static_cast<int8>(P(0).u8Param);
        return OR_CONTINUE;
    }
    // 0x473E51
    case COMMAND_SET_CAR_ENGINE_BROKEN: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->vehicleFlags.bEngineBroken = true;
            veh->vehicleFlags.bEngineOn     = false;
            return OR_CONTINUE;
        }
        veh->vehicleFlags.bEngineBroken = false;
        return OR_CONTINUE;
    }
    // 0x473EA8
    case COMMAND_IS_THIS_MODEL_A_BOAT: {
        CollectParameters(1);
        UpdateCompareFlag(CModelInfo::IsBoatModel(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x473EE1
    case COMMAND_IS_THIS_MODEL_A_PLANE: {
        CollectParameters(1);
        UpdateCompareFlag(CModelInfo::IsPlaneModel(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x473F19
    case COMMAND_IS_THIS_MODEL_A_HELI: {
        CollectParameters(1);
        UpdateCompareFlag(CModelInfo::IsHeliModel(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x473F52
    case COMMAND_SET_FIRST_PERSON_IN_CAR_CAMERA_MODE: {
        CollectParameters(1);
        if (P(0).iParam != 0) {
            TheCamera.m_bDisableFirstPersonInCar = false;
            return OR_CONTINUE;
        }
        TheCamera.m_bDisableFirstPersonInCar = true;
        TheCamera.m_nCarZoom                 = 2;
        return OR_CONTINUE;
    }
    // 0x473F8A
    case COMMAND_TASK_GREET_PARTNER: {
        CollectParameters(4);
        CPed* const ped1               = PedOf(P(0).iParam);
        CPed* const ped2               = PedOf(P(1).iParam);
        const float distanceMultiplier = P(2).fParam;
        const int32 handShakeType      = P(3).iParam;
        {
            auto* const task = new CTaskComplexPartnerGreet(EXE_GREET_PARTNER_NAME, ped2, true, distanceMultiplier, handShakeType, CVector{ 0.0f, 0.0f, 0.0f });
            GivePedScriptedTask(P(0).iParam, task, command);
        }
        {
            auto* const task = new CTaskComplexPartnerGreet(EXE_GREET_PARTNER_NAME, ped1, false, distanceMultiplier, handShakeType, CVector{ 0.0f, 0.0f, 0.0f });
            GivePedScriptedTask(P(1).iParam, task, command);
        }
        return OR_CONTINUE;
    }
    // 0x4740DC
    case COMMAND_SET_HELI_BLADES_FULL_SPEED: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (veh->IsSubHeli()) {
            static_cast<CAutomobile*>(veh)->m_fHeliRotorSpeed = EXE_HELI_BLADES_SPEED;
            return OR_CONTINUE;
        }
        if (veh->IsSubPlane()) {
            static_cast<CPlane*>(veh)->m_fPropSpeed = EXE_PLANE_PROP_SPEED;
        }
        return OR_CONTINUE;
    }
    // 0x474128
    case COMMAND_DISPLAY_HUD: {
        CollectParameters(1);
        CTheScripts::bDisplayHud = P(0).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x474148
    case COMMAND_CONNECT_LODS: {
        CollectParameters(2);
        const int32    handle1 = P(0).iParam;
        CObject* const obj1    = ObjectOf(handle1);
        const int32    handle2 = P(1).iParam;
        CObject* const obj2    = ObjectOf(handle2);
        CTheScripts::ScriptConnectLodsFunction(handle1, handle2);
        CTheScripts::AddToListOfConnectedLodObjects(obj1, obj2);
        return OR_CONTINUE;
    }
    // 0x47418F
    case COMMAND_SET_MAX_FIRE_GENERATIONS: {
        CollectParameters(1);
        gFireManager.m_nMaxFireGenerationsAllowed = P(0).uParam;
        return OR_CONTINUE;
    }
    // 0x4741AB
    case COMMAND_TASK_DIE_NAMED_ANIM: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        char        animName[24];
        ReadTextLabelFromScript(animName, 24);
        char animBlock[16];
        ReadTextLabelFromScript(animBlock, 16);
        CollectParameters(2);
        const float blendDelta = P(0).fParam;
        uint32      flags      = 0x10;
        if (P(1).iParam != 0) {
            flags = 0xD0;
        }
        auto* const task = new CTaskSimpleDie(animName, animBlock, static_cast<eAnimationFlags>(flags), blendDelta, 1.0f);
        GivePedScriptedTask(pedHandle, task, command);
        return OR_CONTINUE;
    }
    // 0x474261
    case COMMAND_SET_PLAYER_DUCK_BUTTON: {
        CollectParameters(2);
        if (P(1).iParam != 0) {
            CPad::GetPad(P(0).iParam)->bDisablePlayerDuck = false;
            return OR_CONTINUE;
        }
        CPad::GetPad(P(0).iParam)->bDisablePlayerDuck = true;
        return OR_CONTINUE;
    }
    // 0x4742AC
    case COMMAND_SET_POOL_TABLE_COORDS: {
        CollectParameters(6);
        CWorld::SnookerTableMax = CVector{ P(3).fParam, P(4).fParam, P(5).fParam };
        CWorld::SnookerTableMin = CVector{ P(0).fParam, P(1).fParam, P(2).fParam };
        return OR_CONTINUE;
    }
    // 0x474315
    case COMMAND_IS_AUDIO_BUILD: {
        UpdateCompareFlag(false);
        return OR_CONTINUE;
    }
    // 0x474322
    case COMMAND_HAS_OBJECT_BEEN_PHOTOGRAPHED: {
        CollectParameters(1);
        CObject* const obj  = ObjectOf(P(0).iParam);
        bool           flag = false;
        if (obj && (obj->m_nObjectFlags & 0x1000)) { // bIsPhotographed
            obj->m_nObjectFlags &= ~0x1000u;
            flag = true;
        }
        UpdateCompareFlag(flag);
        return OR_CONTINUE;
    }

    // 0x47436E: comandos que el EXE deja vacios, sin leer parametros
    case COMMAND_SET_CURRENT_DAY_OF_WEEK:
    case COMMAND_ACTIVATE_INTERIORS:
    case COMMAND_REGISTER_OBJECT_SCRIPT_BRAIN_FOR_CODE_USE:
    case COMMAND_GET_CAR_ROTATION_VELOCITY:
    case COMMAND_TASK_GO_STRAIGHT_TO_COORD_WITHOUT_STOPPING:
    case COMMAND_GET_BEAT_INFO_FOR_CURRENT_TRACK:
    case COMMAND_HAS_CHAR_SPOTTED_CAR:
    case COMMAND_SET_ROPE_HEIGHT_FOR_HELI:
    case COMMAND_GET_ROPE_HEIGHT_FOR_HELI:
    case COMMAND_IS_CAR_LOWRIDER:
    case COMMAND_IS_PERFORMANCE_CAR:
    case COMMAND_SET_ONSCREEN_TIMER_DISPLAY:
    case COMMAND_ALLOCATE_SCRIPT_TO_OBJECT:
    case COMMAND_SET_TWO_PLAYER_CAM_MODE_SAME_CAR_SHOOTING:
    case COMMAND_SET_TWO_PLAYER_CAM_MODE_SAME_CAR_NO_SHOOTING:
    case COMMAND_SET_TWO_PLAYER_CAM_MODE_NOT_BOTH_IN_CAR:
    case COMMAND_GET_CHAR_BREATH:
    case COMMAND_SET_CHAR_BREATH:
    case COMMAND_ARE_PATHS_LOADED_FOR_CAR:
    case COMMAND_FORCE_NEXT_DIE_ANIM:
    case COMMAND_ARE_PATHS_LOADED_IN_AREA:
    case COMMAND_IS_3D_COORD_IN_ZONE:
    case COMMAND_GET_CLOSEST_PICKUP_COORDS_TO_COORD:
    case COMMAND_CLEAR_QUEUED_DIALOGUE:
        return OR_CONTINUE;

    // 0x474372: sin caso en el EXE (2008 2009 2050 2091-2095) y fuera de rango
    default:
        return OR_INTERRUPT;
    }
}
