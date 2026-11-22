#include "StdInc.h"
#include "DatosExe/RdataConstantes.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "PedGroups.h"
#include "World.h"
#include "Stats.h"
#include "Population.h"
#include "PathFind.h"
#include "VehicleRecording.h"
#include "General.h"
#include "X87Math.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/CivilianPed.h"
#include "Entity/Ped/EmergencyPed.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "Attractors/PedAttractorPedPlacer.h"
#include "Scripted2dEffects.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventLeaderEnteredCarAsDriver.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskComplexUseSequence.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleePoint.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Tasks/TaskTypes/TaskComplexWanderStandard.h"
#include "Tasks/TaskTypes/TaskComplexWanderCriminal.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexFollowNodeRoute.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointShooting.h"
#include "Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskComplexBeInGroup.h"
#include "Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h"
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "Tasks/TaskTypes/TaskComplexUseEffect.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "Tasks/TaskTypes/TaskSimpleCarDrive.h"

// F4 de codigo vivo (sin validar): 0x490DB0 (7448 bytes), CRunningScript::ProcessCommands1500To1599 (thiscall, un argumento de pila, `ret 4`; resultado en AL). Es el despachador original de los comandos de script
// 1500..1599 (ranura 15 de la tabla 0x8A6168; lo usa ProcessOneCommand cuando el comando no tiene manejador propio), que Ghidra no separaba como funcion (sus 34 funclets de excepciones colgaban de el). Salta con
// una tabla directa de 94 entradas (0x492AC8; indice = comando - 1500): 23 comandos van a 0x492AA5 y devuelven 0 SIN leer parametros (1503 1504 1505 1507-1512 1514 1519 1520 1544 1545 1548 1552 1559 1564 1567
// 1568 1579-1581) y los comandos 1594..1599 (fuera de la tabla) van a 0x492AA9 (OR_INTERRUPT, -1). Cada `case` sigue el desensamblado: orden de colaboradores (CollectParameters, lecturas de ScriptParams,
// reservas de tarea) y resultados. Prologo SEH (marco con manejador 0x838A54): solo protege los `new` de tareas y los objetos locales con destructor (eventos y tareas de pila), que en C++ se destruyen solos; el
// EXE comprueba el resultado de CTask::operator new (tarea nula): en C++ `new` no devuelve nulo. Los indices que vienen del script (pools, grupos, secuencias) se usan sin comprobar rangos salvo donde el EXE
// los comprueba (grupos 0..7, secuencias y atractores 0..63).
namespace {
// 0x8595EC: float 0.017453292 (0x3C8EFA35), grados a radianes
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
// 0x859878 (57.29578), 0x85991C (90.0) y 0x859E2C (360.0): constantes de GET_HEADING_FROM_VECTOR_2D
constexpr float EXE_RAD_TO_DEG = std::bit_cast<float>(0x42652EE0u);
constexpr float EXE_NINETY     = std::bit_cast<float>(0x42B40000u);
constexpr float EXE_360        = std::bit_cast<float>(0x43B40000u);
// 0x85A318: double 1000.0 (milisegundos por segundo, GET_CHAR_ANIM_TOTAL_TIME)
constexpr double EXE_THOUSAND = 1000.0;
// 0x86FCA0, 0x86FCA4, 0x86FCA8: 0.5, 3.0 y 2.0 (radio, distancia de frenado y cambio de altura de CTaskComplexFollowNodeRoute); 0x86FCAC: 50000 (tiempo por defecto, ms)
constexpr float  EXE_FOLLOW_TOLERANCE    = std::bit_cast<float>(0x3F000000u);
constexpr float  EXE_FOLLOW_SLOWDOWN     = std::bit_cast<float>(0x40400000u);
constexpr float  EXE_FOLLOW_HEIGHT       = std::bit_cast<float>(0x40000000u);
constexpr int32  EXE_FOLLOW_DEFAULT_TIME = 0xC350;
// 0x86FC7C y 0x86FC80: 0.5 y 0.2 (velocidad de giro y diferencia maxima de rumbo de CTaskComplexTurnToFaceEntityOrCoord)
constexpr float EXE_TURN_RATE    = std::bit_cast<float>(0x3F000000u);
constexpr float EXE_TURN_MAXHEAD = std::bit_cast<float>(0x3E4CCCCDu);
// 0x86F678: 1000 (periodo de comprobacion de posicion de CTaskComplexSmartFleeEntity, ms)
constexpr int32 EXE_FLEE_POS_PERIOD = 1000;
// 0xC18CF0 (tolerancia de cambio de posicion de la huida) y 0xC18DB4 (radio de CTaskComplexGoToPointAnyMeans): flotantes de .bss que el EXE inicializa al arrancar
inline float FleePosChangeTolerance() { return StaticRef<float>(0xC18CF0); }
inline float GoToAnyMeansRadius() { return StaticRef<float>(0xC18DB4); }

inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef) y 0x4048E0 (CPool<CVehicle>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
// ComputeResponseTaskType: valores de eTaskType que compara SET_GROUP_MEMBER (0x5E8 y 0x5E9)
constexpr int32 EXE_TASK_GROUP_ENTER_CAR                 = 0x5E8;
constexpr int32 EXE_TASK_GROUP_ENTER_CAR_AND_PERFORM_SEQ = 0x5E9;
} // namespace

static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C);
static_assert(offsetof(CVehicle, m_apPassengers) == 0x464 && offsetof(CVehicle, m_nMaxPassengers) == 0x488);
static_assert(offsetof(CPed, m_pVehicle) == 0x58C);
static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);
static_assert(sizeof(tScriptText) == 0x44);
static_assert(offsetof(tScriptText, DropShadowColor) == 0x1D && offsetof(tScriptText, DropShadow) == 0x21);
static_assert(sizeof(CPedGroup) == 0x2D4 && offsetof(CPedGroup, m_bIsMissionGroup) == 0x2D0);
static_assert(offsetof(CTaskComplexSequence, m_bFlushTasks) == 0x38 && offsetof(CTaskComplexSequence, m_RefCnt) == 0x3C);
static_assert(sizeof(CTaskComplexSequence) == 0x40);
static_assert(offsetof(CAnimBlendAssociation, m_BlendHier) == 0x14 && offsetof(CAnimBlendAssociation, m_CurrentTime) == 0x20 && offsetof(CAnimBlendAssociation, m_Flags) == 0x2E);
static_assert(offsetof(CAnimBlendHierarchy, m_fTotalTime) == 0x10);

// 0x490DB0
OpcodeResult CRunningScript::ProcessCommands1500To1599(int32 command) {
    // Los casos con tarea acaban igual: CRunningScript::GivePedScriptedTask(ped, tarea, comando) (0x492A90).
    const auto GiveTask = [&](int32 pedHandle, CTask* task) {
        GivePedScriptedTask(pedHandle, task, command);
    };
    // Busca la asociacion de animacion del ped por nombre (el texto se lee del script; 24 bytes)
    const auto FindCharAnim = [&](CPed* ped) -> CAnimBlendAssociation* {
        char animName[24];
        ReadTextLabelFromScript(animName, 24);
        return RpAnimBlendClumpGetAssociation(ped->GetRpClump(), animName);
    };

    switch (command) {
    // 0x490DEE: TASK_SMART_FLEE_POINT
    case 1500: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        const CVector point{ P(1).fParam, P(2).fParam, P(3).fParam };
        const float safeDist = P(4).fParam;
        const int32 time     = P(5).iParam;
        GiveTask(pedHandle, new CTaskComplexSmartFleePoint(point, true, safeDist, time));
        return OR_CONTINUE;
    }
    // 0x490E63: TASK_SMART_FLEE_CHAR (la distancia segura es un ENTERO del script: fild)
    case 1501: {
        CollectParameters(4);
        const float safeDist  = static_cast<float>(P(2).iParam);
        const int32 pedHandle = P(0).iParam;
        const int32 time      = P(3).iParam;
        CPed* const fleeFrom  = PedOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskComplexSmartFleeEntity(fleeFrom, true, safeDist, time, EXE_FLEE_POS_PERIOD, FleePosChangeTolerance()));
        return OR_CONTINUE;
    }
    // 0x490EF7: TASK_WANDER_STANDARD
    case 1502: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        auto* const task = new CTaskComplexWanderStandard(PEDMOVE_WALK, static_cast<uint8>(CGeneral::GetRandomNumberInRange<int32>(0, 8)), true);
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x490F46: TASK_KILL_CHAR_ON_FOOT
    case 1506: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        CPed* const target    = PedOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskComplexKillPedOnFoot(target, -1, 0, 0, 0, 1));
        return OR_CONTINUE;
    }
    // 0x490FA0: START_RECORDING_CAR (lee los dos parametros y no hace nada mas)
    case 1513: {
        CollectParameters(2);
        return OR_CONTINUE;
    }
    // 0x490FB0: START_PLAYBACK_RECORDED_CAR
    case 1515: {
        CollectParameters(2);
        const int32 fileNumber = P(1).iParam;
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        CVehicleRecording::StartPlaybackRecordedCar(vehicle, fileNumber, false, false);
        return OR_CONTINUE;
    }
    // 0x490FE6: STOP_PLAYBACK_RECORDED_CAR (con vehiculo nulo no hace nada)
    case 1516: {
        CollectParameters(1);
        if (CVehicle* const vehicle = VehicleOf(P(0).iParam)) {
            CVehicleRecording::StopPlaybackRecordedCar(vehicle);
        }
        return OR_CONTINUE;
    }
    // 0x491018: PAUSE_PLAYBACK_RECORDED_CAR
    case 1517: {
        CollectParameters(1);
        CVehicleRecording::PausePlaybackRecordedCar(VehicleOf(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x491043: UNPAUSE_PLAYBACK_RECORDED_CAR
    case 1518: {
        CollectParameters(1);
        CVehicleRecording::UnpausePlaybackRecordedCar(VehicleOf(P(0).iParam));
        return OR_CONTINUE;
    }
    // 0x49106E, 0x4910CE, 0x49112E y 0x49118F: SET_CAR_ESCORT_CAR_LEFT / RIGHT / REAR / FRONT (misiones 0x1D..0x20); no cambia la mision si ya es 0x39 o 0x3A
    case 1521:
    case 1522:
    case 1523:
    case 1524: {
        CollectParameters(2);
        CVehicle* const vehicle = VehicleOf(P(0).iParam);
        CVehicle* const target  = VehicleOf(P(1).iParam);
        vehicle->m_autoPilot.m_TargetEntity = target;
        target->RegisterReference(reinterpret_cast<CEntity**>(&vehicle->m_autoPilot.m_TargetEntity));
        const auto mission = static_cast<uint8>(vehicle->m_autoPilot.m_nCarMission);
        if (mission != 0x39 && mission != 0x3A) {
            vehicle->m_autoPilot.m_nCarMission = static_cast<eCarMission>(0x1D + (command - 1521));
        }
        return OR_CONTINUE;
    }
    // 0x4911EF: TASK_FOLLOW_PATH_NODES_TO_COORD (tiempo -1 = 50000, tiempo -2 = -1)
    case 1525: {
        CollectParameters(6);
        const CVector target{ P(1).fParam, P(2).fParam, P(3).fParam };
        const int32 pedHandle  = P(0).iParam;
        const int32 moveState  = P(4).iParam;
        int32       time       = P(5).iParam;
        if (time == -1) {
            time = EXE_FOLLOW_DEFAULT_TIME;
        } else if (time == -2) {
            time = -1;
        }
        auto* const task = new CTaskComplexFollowNodeRoute(static_cast<eMoveState>(moveState), target, EXE_FOLLOW_TOLERANCE, EXE_FOLLOW_SLOWDOWN, EXE_FOLLOW_HEIGHT, true, time, true);
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x491289: IS_CHAR_IN_ANGLED_AREA_2D y variantes (1526..1537)
    case 1526:
    case 1527:
    case 1528:
    case 1529:
    case 1530:
    case 1531:
    case 1532:
    case 1533:
    case 1534:
    case 1535:
    case 1536:
    case 1537: {
        CharInAngledAreaCheckCommand(command);
        return OR_CONTINUE;
    }
    // 0x491298: IS_CHAR_IN_TAXI (bInVehicle, vehiculo no nulo y modelo 0x1A4 o 0x1B6)
    case 1538: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        bool inTaxi = false;
        if (ped->bInVehicle) {
            if (CVehicle* const vehicle = ped->m_pVehicle) {
                const uint16 model = vehicle->m_nModelIndex;
                if (model == 0x1A4 || model == 0x1B6) {
                    inTaxi = true;
                }
            }
        }
        UpdateCompareFlag(inTaxi);
        return OR_CONTINUE;
    }
    // 0x4912F4: TASK_GO_TO_COORD_ANY_MEANS (vehiculo solo si el parametro es >= 0)
    case 1539: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        const CVector target{ P(1).fParam, P(2).fParam, P(3).fParam };
        const int32 moveState = P(4).iParam;
        const int32 vehHandle = P(5).iParam;
        CVehicle* vehicle = nullptr;
        if (vehHandle >= 0) {
            vehicle = VehicleOf(vehHandle);
        }
        GiveTask(pedHandle, new CTaskComplexGoToPointAnyMeans(moveState, target, vehicle, GoToAnyMeansRadius(), -1));
        return OR_CONTINUE;
    }
    // 0x49137C: GET_HEADING_FROM_VECTOR_2D: atan(x, y) * 57.29578 - 90, sumando 360 mientras sea negativo
    case 1540: {
        CollectParameters(2);
        float heading = CGeneral::GetATanOfXY(P(0).fParam, P(1).fParam) * EXE_RAD_TO_DEG - EXE_NINETY;
        while (heading < 0.0f) {
            heading += EXE_360;
        }
        P(0).fParam = heading;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x4913F5: TASK_PLAY_ANIM
    case 1541: {
        PlayAnimScriptCommand(command);
        return OR_CONTINUE;
    }
    // 0x491404: LOAD_PATH_NODES_IN_AREA (ordena cada eje; el EXE solo intercambia si el primero es MAYOR)
    case 1542: {
        CollectParameters(4);
        float x1 = P(0).fParam;
        float y1 = P(1).fParam;
        float x2 = P(2).fParam;
        float y2 = P(3).fParam;
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        ThePaths.MakeRequestForNodesToBeLoaded(x1, x2, y1, y2);
        return OR_CONTINUE;
    }
    // 0x491494: RELEASE_PATH_NODES
    case 1543: {
        ThePaths.ReleaseRequestedNodes();
        return OR_CONTINUE;
    }
    // 0x4914A5: LOAD_CHAR_DECISION_MAKER
    case 1546: {
        CollectParameters(1);
        char name[128];
        CDecisionMakerTypesFileLoader::GetPedDMName(P(0).iParam, name);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_DECISION_MAKER); // el EXE descarta el resultado
        const int32 decisionMaker = CDecisionMakerTypesFileLoader::LoadDecisionMaker(name, static_cast<eDecisionTypes>(0), m_UsesMissionCleanup);
        P(0).iParam = CTheScripts::GetNewUniqueScriptThingIndex(decisionMaker, SCRIPT_THING_DECISION_MAKER);
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_DECISION_MAKER);
        }
        return OR_CONTINUE;
    }
    // 0x49152D: SET_CHAR_DECISION_MAKER
    case 1547: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        int32 decisionMaker = -1;
        if (P(1).iParam != -1) {
            decisionMaker = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_DECISION_MAKER);
        }
        ped->GetIntelligence()->SetPedDecisionMakerType(decisionMaker);
        return OR_CONTINUE;
    }
    // 0x491575: SET_TEXT_DROPSHADOW: color (cuatro bytes) y tamano (un byte) en la linea de texto actual
    case 1549: {
        CollectParameters(5);
        const CRGBA color{ static_cast<uint8>(P(1).iParam), static_cast<uint8>(P(2).iParam), static_cast<uint8>(P(3).iParam), static_cast<uint8>(P(4).iParam) };
        auto& line = CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
        line.DropShadowColor = color;
        line.DropShadow      = static_cast<int8>(P(0).iParam);
        return OR_CONTINUE;
    }
    // 0x4915F5: IS_PLAYBACK_GOING_ON_FOR_CAR
    case 1550: {
        CollectParameters(1);
        UpdateCompareFlag(CVehicleRecording::IsPlaybackGoingOnForCar(VehicleOf(P(0).iParam)));
        return OR_CONTINUE;
    }
    // 0x49162A: SET_SENSE_RANGE: un ped, o (con -1) todos los peatones de mision que se mueven
    case 1551: {
        CollectParameters(2);
        const float range = P(1).fParam;
        if (P(0).iParam != -1) {
            CPed* const ped = PedOf(P(0).iParam);
            ped->GetIntelligence()->SetSeeingRange(range);
            ped->GetIntelligence()->SetHearingRange(range);
            return OR_CONTINUE;
        }
        for (auto* const entity : CWorld::ms_listMovingEntityPtrs) {
            if (entity->GetType() == ENTITY_TYPE_PED) {
                auto* const ped = static_cast<CPed*>(entity);
                if (ped->GetCreatedBy() == PED_MISSION) {
                    ped->GetIntelligence()->SetSeeingRange(range);
                    ped->GetIntelligence()->SetHearingRange(range);
                }
            }
        }
        return OR_CONTINUE;
    }
    // 0x4916CA: IS_CHAR_PLAYING_ANIM
    case 1553: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        const bool playing = FindCharAnim(ped) != nullptr;
        UpdateCompareFlag(playing);
        return OR_CONTINUE;
    }
    // 0x491727: SET_CHAR_ANIM_PLAYING_FLAG (bit 0 de las banderas de la asociacion)
    case 1554: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CAnimBlendAssociation* const assoc = FindCharAnim(ped);
        CollectParameters(1);
        if (assoc) {
            if (P(0).iParam != 0) {
                assoc->m_Flags |= 1;
            } else {
                assoc->m_Flags &= static_cast<uint16>(~1);
            }
        }
        return OR_CONTINUE;
    }
    // 0x491794: GET_CHAR_ANIM_CURRENT_TIME (tiempo actual / duracion total; 0.0 sin asociacion)
    case 1555: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CAnimBlendAssociation* const assoc = FindCharAnim(ped);
        float progress = 0.0f;
        if (assoc) {
            progress = assoc->m_CurrentTime / assoc->m_BlendHier->m_fTotalTime;
        }
        P(0).fParam = progress;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x491802: SET_CHAR_ANIM_CURRENT_TIME (fraccion de la duracion total)
    case 1556: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CAnimBlendAssociation* const assoc = FindCharAnim(ped);
        CollectParameters(1);
        if (assoc) {
            assoc->SetCurrentTime(assoc->m_BlendHier->m_fTotalTime * P(0).fParam);
        }
        return OR_CONTINUE;
    }
    // 0x49186F: OPEN_SEQUENCE_TASK
    case 1557: {
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_SEQUENCE_TASK); // el EXE descarta el resultado
        const int32 slot = CTaskSequences::GetAvailableSlot(m_UsesMissionCleanup);
        if (slot >= 0 && slot < 0x40) {
            CTaskSequences::ms_bIsOpened[slot] = true;
            CTaskSequences::ms_taskSequence[slot].Flush();
            CTaskSequences::ms_iActiveSequence = slot;
            P(0).iParam = CTheScripts::GetNewUniqueScriptThingIndex(slot, SCRIPT_THING_SEQUENCE_TASK);
            if (m_UsesMissionCleanup) {
                CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_TASK_SEQUENCE);
            }
        } else {
            P(0).iParam = -1;
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x491908: CLOSE_SEQUENCE_TASK
    case 1558: {
        CollectParameters(1);
        const int32 slot = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_SEQUENCE_TASK);
        if (slot >= 0 && slot < 0x40) {
            CTaskSequences::ms_bIsOpened[slot] = false;
            CTaskSequences::ms_iActiveSequence = -1;
        }
        return OR_CONTINUE;
    }
    // 0x49194B: PERFORM_SEQUENCE_TASK
    case 1560: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 slot = CTheScripts::GetActualScriptThingIndex(P(1).iParam, SCRIPT_THING_SEQUENCE_TASK);
        if (slot >= 0 && slot < 0x40) {
            GiveTask(pedHandle, new CTaskComplexUseSequence(slot));
        }
        return OR_CONTINUE;
    }
    // 0x4919AC: SET_CHAR_COLLISION (bit 0 de las banderas de la entidad)
    case 1561: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        ped->m_bUsesCollision = P(1).iParam != 0;
        return OR_CONTINUE;
    }
    // 0x4919ED: GET_CHAR_ANIM_TOTAL_TIME (duracion total * 1000.0, en flotante; 0.0 sin asociacion)
    case 1562: {
        CollectParameters(1);
        CPed* const ped = PedOf(P(0).iParam);
        CAnimBlendAssociation* const assoc = FindCharAnim(ped);
        float total = 0.0f;
        if (assoc) {
            total = static_cast<float>(static_cast<double>(assoc->m_BlendHier->m_fTotalTime) * EXE_THOUSAND);
        }
        P(0).fParam = total;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x491A4A: CLEAR_SEQUENCE_TASK
    case 1563: {
        CollectParameters(1);
        const int32 handle = P(0).iParam;
        const int32 slot = CTheScripts::GetActualScriptThingIndex(handle, SCRIPT_THING_SEQUENCE_TASK);
        if (slot >= 0 && slot < 0x40) {
            auto& sequence = CTaskSequences::ms_taskSequence[slot];
            CTaskSequences::ms_bIsOpened[slot] = false;
            if (sequence.m_RefCnt == 0) {
                sequence.m_bFlushTasks = false;
                sequence.Flush();
            } else {
                sequence.m_bFlushTasks = true;
            }
            CTheScripts::ScriptSequenceTaskArray[slot].m_bUsed = false;
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_TASK_SEQUENCE);
        }
        return OR_CONTINUE;
    }
    // 0x491ABC: ADD_ATTRACTOR
    case 1565: {
        CollectParameters(6);
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_2D_EFFECT); // el EXE descarta el resultado
        const int32 effectIdx = CScripted2dEffects::AddScripted2DEffect(-1.0f);
        const int32 sequenceIdx = CTheScripts::GetActualScriptThingIndex(P(5).iParam, SCRIPT_THING_SEQUENCE_TASK);
        if (effectIdx < 0 || effectIdx >= 0x40 || sequenceIdx < 0 || sequenceIdx >= 0x40) {
            P(0).iParam = -1;
            StoreParameters(1);
            return OR_CONTINUE;
        }
        // Posicion y dos direcciones: (-sin(a), cos(a), 0) normalizadas, con a = grados * 0.017453292 (fsin/fcos del EXE)
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        const float angle1 = P(3).fParam * EXE_DEG_TO_RAD;
        CVector dir1{ -TrigSimpleSinCpp(angle1), TrigSimpleCosCpp(angle1), 0.0f };
        dir1.Normalise();
        const float angle2 = P(4).fParam * EXE_DEG_TO_RAD;
        CVector dir2{ -TrigSimpleSinCpp(angle2), TrigSimpleCosCpp(angle2), 0.0f };
        dir2.Normalise();
        auto& effect = *reinterpret_cast<C2dEffectPedAttractor*>(&CScripted2dEffects::ms_effects[effectIdx]);
        effect.m_Pos = pos;
        CScripted2dEffects::ms_effectSequenceTaskIDs[effectIdx] = sequenceIdx;
        effect.m_vecQueueDir    = dir1;
        effect.m_vecUseDir      = dir1;
        effect.m_vecForwardDir  = dir2;
        effect.m_Type           = EFFECT_ATTRACTOR;
        effect.m_nAttractorType = static_cast<ePedAttractorType>(7);
        P(0).iParam = CTheScripts::GetNewUniqueScriptThingIndex(effectIdx, SCRIPT_THING_2D_EFFECT);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PED_QUEUE);
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x491C33: CLEAR_ATTRACTOR
    case 1566: {
        CollectParameters(1);
        const int32 handle = P(0).iParam;
        const int32 effectIdx = CTheScripts::GetActualScriptThingIndex(handle, SCRIPT_THING_2D_EFFECT);
        if (effectIdx >= 0 && effectIdx < 0x40) {
            CScripted2dEffects::ms_activated[effectIdx] = false;
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_PED_QUEUE);
        }
        return OR_CONTINUE;
    }
    // 0x491C7F: CREATE_CHAR_AT_ATTRACTOR
    case 1569: {
        CollectParameters(4);
        const int32 effectIdx = CTheScripts::GetActualScriptThingIndex(P(2).iParam, SCRIPT_THING_2D_EFFECT);
        if (effectIdx < 0 || effectIdx >= 0x40) {
            P(0).iParam = -1;
            StoreParameters(1);
            return OR_CONTINUE;
        }
        GetCorrectPedModelIndexForEmergencyServiceType(static_cast<ePedType>(P(0).iParam), reinterpret_cast<uint32*>(&P(1).iParam));
        const int32 pedType = P(0).iParam;
        CPed* ped;
        if (pedType == PED_TYPE_COP) {
            ped = new CCopPed(static_cast<uint32>(P(1).iParam));
        } else if (pedType == PED_TYPE_MEDIC || pedType == PED_TYPE_FIREMAN) {
            ped = new CEmergencyPed(static_cast<ePedType>(pedType), static_cast<uint32>(P(1).iParam));
        } else {
            ped = new CCivilianPed(static_cast<ePedType>(pedType), static_cast<uint32>(P(1).iParam));
        }
        ped->SetCharCreatedBy(PED_MISSION);
        ped->bAllowMedicsToReviveMe = false;
        auto* const effect = reinterpret_cast<C2dEffectPedAttractor*>(&CScripted2dEffects::ms_effects[effectIdx]);
        CPedAttractorPedPlacer::PlacePedAtEffect(*effect, nullptr, ped, 0.01f);
        const CVector pos = ped->GetPosition();
        CTheScripts::ClearSpaceForMissionEntity(pos, ped);
        if (m_UsesMissionCleanup) {
            ped->m_bIsStaticWaitingForCollision = true;
        }
        CWorld::Add(ped);
        // La conducta inicial depende del comando pasado en el cuarto parametro (1466 = quieto, 1503 = ninguna, 1504 = criminal, el resto = estandar)
        const int32 behaviour = P(3).iParam - 1466;
        if (behaviour != 37) { // 1503: sin tarea por defecto
            CTask* task;
            if (behaviour == 0) {
                task = new CTaskSimpleStandStill(0, true, false, 8.0f);
            } else if (behaviour == 38) {
                task = new CTaskComplexWanderCriminal(PEDMOVE_WALK, static_cast<uint8>(CGeneral::GetRandomNumberInRange<int32>(0, 8)), true);
            } else {
                task = new CTaskComplexWanderStandard(PEDMOVE_WALK, static_cast<uint8>(CGeneral::GetRandomNumberInRange<int32>(0, 8)), true);
            }
            ped->GetIntelligence()->m_TaskMgr.SetTask(task, TASK_PRIMARY_DEFAULT, false);
        }
        auto* const useEffect = new CTaskComplexUseEffect(effect, nullptr);
        CEventScriptCommand scriptEvent{ TASK_PRIMARY_PRIMARY, useEffect, false };
        auto* const added = static_cast<CEventScriptCommand*>(ped->GetIntelligence()->m_eventGroup.Add(&scriptEvent, false));
        const int32 recordSlot = CPedScriptedTaskRecord::GetVacantSlot();
        CPedScriptedTaskRecord::ms_scriptedTasks[recordSlot].Set(ped, command, added);
        CPopulation::ms_nTotalMissionPeds++;
        P(0).iParam = GetPedPool()->GetRef(ped);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_PED);
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x491FFF: TASK_LEAVE_CAR_IMMEDIATELY
    case 1570: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        CVehicle* const vehicle = VehicleOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskComplexLeaveCar(vehicle, 0, 0, false, false));
        return OR_CONTINUE;
    }
    // 0x492058: INCREMENT_INT_STAT (cantidad entera del script: fild)
    case 1571: {
        CollectParameters(2);
        CStats::IncrementStat(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), static_cast<float>(P(1).iParam));
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, static_cast<eStats>(P(0).iParam), static_cast<float>(P(1).iParam));
        return OR_CONTINUE;
    }
    // 0x49209D: INCREMENT_FLOAT_STAT
    case 1572: {
        CollectParameters(2);
        CStats::IncrementStat(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), P(1).fParam);
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, static_cast<eStats>(P(0).iParam), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x4920C0: DECREMENT_INT_STAT
    case 1573: {
        CollectParameters(2);
        CStats::DecrementStat(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), static_cast<float>(P(1).iParam));
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, static_cast<eStats>(P(0).iParam), static_cast<float>(P(1).iParam));
        return OR_CONTINUE;
    }
    // 0x492105: DECREMENT_FLOAT_STAT
    case 1574: {
        CollectParameters(2);
        CStats::DecrementStat(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), P(1).fParam);
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, static_cast<eStats>(P(0).iParam), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x492142: REGISTER_INT_STAT (como el INCREMENT: el aviso usa STAT_UPDATE_INCREASE)
    case 1575: {
        CollectParameters(2);
        CStats::SetNewRecordStat(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), static_cast<float>(P(1).iParam));
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, static_cast<eStats>(P(0).iParam), static_cast<float>(P(1).iParam));
        return OR_CONTINUE;
    }
    // 0x492168: REGISTER_FLOAT_STAT
    case 1576: {
        CollectParameters(2);
        CStats::SetNewRecordStat(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), P(1).fParam);
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, static_cast<eStats>(P(0).iParam), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x492188: SET_INT_STAT
    case 1577: {
        CollectParameters(2);
        CStats::SetStatValue(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), static_cast<float>(P(1).iParam));
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, static_cast<eStats>(P(0).iParam), static_cast<float>(P(1).iParam));
        return OR_CONTINUE;
    }
    // 0x4921CD: SET_FLOAT_STAT
    case 1578: {
        CollectParameters(2);
        CStats::SetStatValue(static_cast<eStats>(static_cast<uint16>(P(0).iParam)), P(1).fParam);
        CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_INCREASE, static_cast<eStats>(P(0).iParam), P(1).fParam);
        return OR_CONTINUE;
    }
    // 0x49220A: GET_SCRIPT_TASK_STATUS (-1 se devuelve como 7)
    case 1582: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        const int32 status = static_cast<int32>(CPedScriptedTaskRecord::GetStatus(ped, P(1).iParam));
        P(0).iParam = status;
        if (status == -1) {
            P(0).iParam = 7;
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x492258: CREATE_GROUP
    case 1583: {
        CollectParameters(1);
        const int32 allocatorType = P(0).iParam;
        const int32 next = CollectNextParameterWithoutIncreasingPC();
        CTheScripts::GetActualScriptThingIndex(next, SCRIPT_THING_PED_GROUP); // el EXE descarta el resultado
        const int32 groupId = CPedGroups::AddGroup();
        auto& group = CPedGroups::ms_groups[groupId];
        group.GetIntelligence().SetDefaultTaskAllocatorType(static_cast<ePedGroupDefaultTaskAllocatorType>(allocatorType));
        group.m_bIsMissionGroup = true;
        P(0).iParam = CTheScripts::GetNewUniqueScriptThingIndex(groupId, SCRIPT_THING_PED_GROUP);
        StoreParameters(1);
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.AddEntityToList(P(0).iParam, MISSION_CLEANUP_ENTITY_TYPE_GROUP);
        }
        return OR_CONTINUE;
    }
    // 0x4922D8: SET_GROUP_LEADER (si el ped no es el jugador, tarea CTaskComplexBeInGroup(grupo, lider) como comando de script)
    case 1584: {
        CollectParameters(2);
        const int32 groupId = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        CPed* const ped = PedOf(P(1).iParam);
        if (groupId >= 0 && groupId < 8) {
            if (!ped->IsPlayer()) {
                auto* const task = new CTaskComplexBeInGroup(groupId, true);
                CEventScriptCommand scriptEvent{ TASK_PRIMARY_PRIMARY, task, false };
                ped->GetIntelligence()->m_eventGroup.Add(&scriptEvent, false);
            }
            auto& group = CPedGroups::ms_groups[groupId];
            group.GetMembership().SetLeader(ped);
            group.Process();
        }
        return OR_CONTINUE;
    }
    // 0x4923BA: SET_GROUP_MEMBER
    case 1585: {
        CollectParameters(2);
        const int32 groupId = CTheScripts::GetActualScriptThingIndex(P(0).iParam, SCRIPT_THING_PED_GROUP);
        CPed* const ped = PedOf(P(1).iParam);
        if (groupId < 0 || groupId >= 8) {
            return OR_CONTINUE;
        }
        auto* const beInGroup = new CTaskComplexBeInGroup(groupId, false);
        CEventScriptCommand scriptEvent{ TASK_PRIMARY_PRIMARY, beInGroup, false };
        ped->GetIntelligence()->m_eventGroup.Add(&scriptEvent, false);
        auto& group      = CPedGroups::ms_groups[groupId];
        auto& membership = group.GetMembership();
        if (membership.CountMembersExcludingLeader() >= 7 && membership.GetLeader() && membership.GetLeader()->IsPlayer()) {
            membership.RemoveNFollowers(1);
        }
        membership.AddFollower(ped);
        group.Process();
        CPed* const leader = membership.GetLeader();
        if (!leader) {
            return OR_CONTINUE;
        }
        // Vehiculo del lider: el suyo si va dentro; si no, el de su tarea de subir como conductor
        CVehicle* vehicle = nullptr;
        if (leader->bInVehicle) {
            vehicle = leader->m_pVehicle;
        }
        if (!vehicle) {
            auto* const enterTask = static_cast<CTaskComplexEnterCar*>(leader->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER));
            if (!enterTask) {
                return OR_CONTINUE;
            }
            vehicle = enterTask->GetTargetCar();
            if (!vehicle) {
                return OR_CONTINUE;
            }
        }
        // Primera plaza de pasajero libre (el EXE recorre m_apPassengers hasta m_nMaxPassengers)
        const int32 maxPassengers = vehicle->m_nMaxPassengers;
        int32 seat = 0;
        if (maxPassengers <= 0) {
            return OR_CONTINUE;
        }
        for (; seat < maxPassengers; seat++) {
            if (vehicle->m_apPassengers.data()[seat] == nullptr) {
                break;
            }
        }
        if (seat >= maxPassengers) {
            return OR_CONTINUE;
        }
        CEventLeaderEnteredCarAsDriver leaderEvent{ vehicle };
        leaderEvent.ComputeResponseTaskType(&group);
        const int32 responseTask = static_cast<int32>(leaderEvent.m_TaskId);
        if (responseTask == EXE_TASK_GROUP_ENTER_CAR) {
            CTaskComplexEnterCarAsPassenger enterCar{ vehicle, 0, true };
            group.GetIntelligence().SetScriptCommandTask(ped, enterCar);
            if (!ped->bHasGroupDriveTask) {
                ped->bHasGroupDriveTask = true;
                CTaskComplexSequence sequence;
                sequence.AddTask(new CTaskSimpleCarDrive(vehicle, nullptr, true));
                if (CTask* const defaultTask = group.GetIntelligence().GetTaskDefault(ped)) {
                    sequence.AddTask(defaultTask->Clone());
                }
                group.GetIntelligence().SetDefaultTask(ped, sequence);
            }
        } else if (responseTask == EXE_TASK_GROUP_ENTER_CAR_AND_PERFORM_SEQ) {
            auto* const innerEvent = new CEventLeaderEnteredCarAsDriver(vehicle);
            CEventGroupEvent groupEvent{ leader, innerEvent };
            group.GetIntelligence().AddEvent(&groupEvent);
        }
        return OR_CONTINUE;
    }
    // 0x4926E9: REMOVE_GROUP
    case 1586: {
        CollectParameters(1);
        const int32 handle = P(0).iParam;
        const int32 groupId = CTheScripts::GetActualScriptThingIndex(handle, SCRIPT_THING_PED_GROUP);
        if (groupId >= 0 && groupId < 8) {
            bool removeOnlyFollowers = false;
            if (CPedGroups::ms_activeGroups[groupId]) {
                auto& membership = CPedGroups::ms_groups[groupId].GetMembership();
                if (membership.GetLeader() && membership.GetLeader()->IsPlayer()) {
                    removeOnlyFollowers = true;
                }
            }
            if (removeOnlyFollowers) {
                CPedGroups::RemoveAllFollowersFromGroup(groupId);
                return OR_CONTINUE;
            }
            CPedGroups::RemoveGroup(groupId);
        }
        if (m_UsesMissionCleanup) {
            CTheScripts::MissionCleanUp.RemoveEntityFromList(handle, MISSION_CLEANUP_ENTITY_TYPE_GROUP);
        }
        return OR_CONTINUE;
    }
    // 0x49277A: TASK_LEAVE_ANY_CAR
    case 1587: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        GiveTask(pedHandle, new CTaskComplexLeaveAnyCar(0, true, false));
        return OR_CONTINUE;
    }
    // 0x4927D3: TASK_KILL_CHAR_ON_FOOT_WHILE_DUCKING
    case 1588: {
        CollectParameters(5);
        CPed* const target = PedOf(P(1).iParam);
        const int32 pedHandle = P(0).iParam;
        GiveTask(pedHandle, new CTaskComplexKillPedOnFoot(target, -1, P(2).iParam, P(3).iParam, P(4).iParam, 1));
        return OR_CONTINUE;
    }
    // 0x49283D: TASK_AIM_GUN_AT_CHAR (posicion y destino nulos = vectores a cero, orden NONE, rafaga 1)
    case 1589: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam;
        CPed* const target = PedOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskSimpleGunControl(target, CVector{}, CVector{}, eGunCommand::NONE, 1, P(2).iParam));
        return OR_CONTINUE;
    }
    // 0x49289D: TASK_SIDE_STEP_AND_SHOOT (lee nueve parametros y no hace nada mas)
    case 1590: {
        CollectParameters(9);
        return OR_CONTINUE;
    }
    // 0x4928AD: TASK_GO_TO_COORD_WHILE_SHOOTING
    case 1591: {
        CollectParameters(8);
        const int32 pedHandle = P(0).iParam;
        const int32 moveState = P(4).iParam;
        const float targetRadius = P(5).fParam;
        const float slowDownDist = P(6).fParam;
        CPed* const aimAt = PedOf(P(7).iParam);
        const CVector movePos{ P(1).fParam, P(2).fParam, P(3).fParam };
        GiveTask(pedHandle, new CTaskComplexGoToPointShooting(static_cast<eMoveState>(moveState), movePos, aimAt, CVector{}, targetRadius, slowDownDist));
        return OR_CONTINUE;
    }
    // 0x492975: TASK_STAY_IN_SAME_PLACE (con ped: se ejecuta al momento; con -1: se anade a la secuencia activa)
    case 1592: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        if (pedHandle != -1) {
            CPed* const ped = PedOf(pedHandle);
            CTaskSimpleSetStayInSamePlace task{ P(1).iParam != 0 };
            task.ProcessPed(ped);
            return OR_CONTINUE;
        }
        auto* const task = new CTaskSimpleSetStayInSamePlace(P(1).iParam != 0);
        CTaskSequences::ms_taskSequence[CTaskSequences::ms_iActiveSequence].AddTask(task);
        return OR_CONTINUE;
    }
    // 0x492A36: TASK_TURN_CHAR_TO_FACE_CHAR
    case 1593: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        CPed* const target = PedOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskComplexTurnToFaceEntityOrCoord(target, EXE_TURN_RATE, EXE_TURN_MAXHEAD));
        return OR_CONTINUE;
    }

    // 0x492AA5: comandos que el EXE deja vacios (no leen parametros)
    case 1503:
    case 1504:
    case 1505:
    case 1507:
    case 1508:
    case 1509:
    case 1510:
    case 1511:
    case 1512:
    case 1514:
    case 1519:
    case 1520:
    case 1544:
    case 1545:
    case 1548:
    case 1552:
    case 1559:
    case 1564:
    case 1567:
    case 1568:
    case 1579:
    case 1580:
    case 1581:
        return OR_CONTINUE;

    // 0x492AA9: 1594..1599 y los fuera de rango
    default:
        return OR_INTERRUPT;
    }
}
