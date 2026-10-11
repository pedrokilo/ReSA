/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowNodeRoute
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 19/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TrigCompound6Cpp.h"

#include "Interior/InteriorManager_c.h"

#include "TaskComplexFollowNodeRoute.h"

#include "Tasks/TaskTypes/TaskSimpleGoToPointFine.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"

void CTaskComplexFollowNodeRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowNodeRoute, 0x8700a8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66EA30);
    RH_ScopedInstall(Destructor, 0x66EB70);

    RH_ScopedInstall(GetSubTaskType, 0x669650);

    RH_ScopedInstall(CalcGoToTaskType, 0x66EBE0);
    RH_ScopedInstall(SetTarget, 0x671750);
    RH_ScopedInstall(CreateSubTask, 0x669690);
    RH_ScopedInstall(GetLastWaypoint, 0x6698E0);
    RH_ScopedInstall(GetNextWaypoint, 0x669980);
    RH_ScopedInstall(ComputeRoute, 0x6699E0);
    RH_ScopedInstall(CalcBlendRatio, 0x66EDC0);
    RH_ScopedInstall(CanGoStraightThere, 0x66EF20);
    RH_ScopedInstall(ComputePathNodes, 0x66EFA0);

    RH_ScopedVMTInstall(Clone, 0x6713E0);
    RH_ScopedVMTInstall(GetTaskType, 0x66EB60);
    RH_ScopedVMTInstall(StopTimer, 0x6694E0);
    RH_ScopedVMTInstall(MakeAbortable, 0x669520);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6718D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x671800);
    RH_ScopedVMTInstall(ControlSubTask, 0x671AB0);
}

namespace {
// Como el original: dirección de nodo vacía global (0x8A5F44) que el EXE copia en m_StartNode
auto& s_EmptyNodeAddress = StaticRef<CNodeAddress>(0x8A5F44);

// Como el original: constantes leídas del EXE
constexpr float FNR_COS_45         = std::bit_cast<float>(0x3F34FDF4u); // 0x863C1C (0.707)
constexpr float FNR_INV_COS_45_P1  = std::bit_cast<float>(0x3F15F880u); // 0x8700A4 (1 / 1.707)
constexpr float FNR_PI             = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
constexpr float FNR_SPEED_SQ_LIMIT = 0.015625f;                          // 0x86FC74

// Como el original: fcos del x87 con precisión interna completa; producto y suma redondeados por la FPU


// Como el original: posición del nodo leída directamente del array del área (sin comprobaciones)
CVector FollowNodeRouteNodePos(CNodeAddress addr) {
    return ThePaths.m_pPathNodes[addr.m_wAreaId][addr.m_wNodeId].GetPosition();
}

// Como el original: el EXE solo comprueba m_wAreaId != -1 y que el área esté cargada
bool FollowNodeRouteNodeLoaded(CNodeAddress addr) {
    return addr.m_wAreaId != UINT16_MAX && ThePaths.IsAreaNodesAvailable(addr);
}
}; // namespace

// 0x66EA30
CTaskComplexFollowNodeRoute::CTaskComplexFollowNodeRoute(
    eMoveState     moveState,
    const CVector& target,
    float          targetTolerance,
    float          slowDownDist,
    float          followNodeThresholdHeightChange,
    bool           bKeepNodesHeadingAwayFromTarget,
    int32          timeToFinishMs,
    bool           bUseBlending
) :
    m_TargetPt{target},
    m_MoveState{moveState},
    m_TargetTolerance{targetTolerance},
    m_SlowDownDist{slowDownDist},
    m_FollowNodeThresholdHeightChange{followNodeThresholdHeightChange},
    m_CurrPtIdx{0},
    m_TimeToFinishMs{timeToFinishMs}
{
    // Como el original: de m_CurrNode solo se escribe el área
    m_CurrNode.m_wAreaId = UINT16_MAX;

    // Como el original: solo se escriben los bits 0 y 3; con mezcla se limpian los bits 5 y 6 y los flotantes
    m_bKeepNodesHeadingAwayFromTarget = bKeepNodesHeadingAwayFromTarget;
    m_bUseBlending                    = bUseBlending;
    if (m_bUseBlending) {
        m_bSlowingDown      = false;
        m_bSpeedingUp       = false;
        m_SpeedDecreaseDist = 0.f;
        m_SpeedIncreaseDist = 0.f;
        m_SpeedDecreaseAmt  = 0.f;
        m_SpeedIncreaseAmt  = 0.f;
    }

    // Como el original: primero CPointRoute (0x41B7C0, solo el contador) y después CNodeRoute (0x41B860, contador y
    // m_wAreaId de cada entrada); `new T{}` inicializaba por valor el resto
    auto* const ptRoute = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (ptRoute) {
        ptRoute->m_NumEntries = 0;
    }
    m_PtRoute = ptRoute;

    auto* const nodeRoute = static_cast<CNodeRoute*>(CNodeRoute::operator new(sizeof(CNodeRoute)));
    if (nodeRoute) {
        nodeRoute->m_NumEntries = 0;
        for (auto& e : nodeRoute->m_Entries) {
            e.m_wAreaId = UINT16_MAX;
        }
    }
    m_NodeRoute = nodeRoute;

    m_StartNode = s_EmptyNodeAddress;
}

// 0x6713E0
CTaskComplexFollowNodeRoute::CTaskComplexFollowNodeRoute(const CTaskComplexFollowNodeRoute& o) :
    CTaskComplexFollowNodeRoute{
        o.m_MoveState,
        o.m_TargetPt,
        o.m_TargetTolerance,
        o.m_SlowDownDist,
        o.m_FollowNodeThresholdHeightChange,
        o.m_bKeepNodesHeadingAwayFromTarget,
        o.m_TimeToFinishMs,
        o.m_bUseBlending
    }
{
    m_StartNode = o.m_StartNode;
}

// 0x66EB70
CTaskComplexFollowNodeRoute::~CTaskComplexFollowNodeRoute() {
    // Como el original: cada ruta se libera solo si no es nula (`delete nullptr` llegaba a CNodeRoutePool::Delete,
    // que no comprueba nulos y falla en la aserción IsPtrFromPool)
    if (m_PtRoute) {
        delete m_PtRoute;
    }
    if (m_NodeRoute) {
        delete m_NodeRoute;
    }
}

// 0x669650
eTaskType CTaskComplexFollowNodeRoute::GetSubTaskType(uint32 progress, bool bLastRoutePointIsTarget, const CPointRoute& route) {
    if (!bLastRoutePointIsTarget) {
        return TASK_SIMPLE_GO_TO_POINT;
    }
    if (progress == route.m_NumEntries) {
        return TASK_FINISHED;
    }
    return route.m_NumEntries != progress + 1
        ? TASK_SIMPLE_GO_TO_POINT
        : TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL;
}

// 0x6694E0
void CTaskComplexFollowNodeRoute::StopTimer(const CEvent* event) {
    // Como el original: el temporizador se pausa (m_bStopped y se descuenta el tiempo transcurrido), no se detiene
    if (!CEventHandler::IsTemporaryEvent(*event)) {
        m_FinishedTimer.Pause();
    }
}

// 0x66EBE0
eTaskType CTaskComplexFollowNodeRoute::CalcGoToTaskType(CPed* ped, eTaskType subTaskType) {
    if (subTaskType != TASK_SIMPLE_GO_TO_POINT) {
        return subTaskType;
    }

    m_SpeedIncreaseAmt = m_SpeedDecreaseAmt;
    m_bWillSlowDown    = false;

    if (m_MoveState > PEDMOVE_WALK) {
        // Como el original: el índice se limita (con signo) al último punto; si queda negativo se sale sin tocar las banderas
        const auto numPts = (int32)m_PtRoute->m_NumEntries;
        auto       ptIdx  = (int32)m_CurrPtIdx;
        if (ptIdx >= numPts) {
            ptIdx = numPts - 1;
        }
        if (ptIdx < 0) {
            return TASK_SIMPLE_GO_TO_POINT;
        }

        // Como el original: direcciones en 2D (z = 0) normalizadas; producto escalar y*y + x*x + z*z
        const CVector& currPt = m_PtRoute->m_Entries.data()[ptIdx];
        const CVector  lastPt = GetLastWaypoint(ped);
        CVector        dirIn{ currPt.x - lastPt.x, currPt.y - lastPt.y, 0.f };
        dirIn.Normalise();
        const CVector nextPt = GetNextWaypoint(ped);
        CVector       dirOut{ nextPt.x - currPt.x, nextPt.y - currPt.y, 0.f };
        dirOut.Normalise();
        const float cosAngle = dirIn.y * dirOut.y + dirIn.x * dirOut.x + dirIn.z * dirOut.z;

        if (cosAngle < FNR_COS_45) { // Como el original: estrictamente menor (test ah,5; jp)
            // Como el original: multiplicación por 1/(cos45+1) y límites [0, 1] (NaN se queda igual)
            float amt = 1.f - (cosAngle + 1.f) * FNR_INV_COS_45_P1;
            if (amt < 0.f) {
                amt = 0.f;
            } else if (amt > 1.f) {
                amt = 1.f;
            }
            if (m_MoveState == PEDMOVE_SPRINT) {
                m_SpeedDecreaseDist = m_SpeedIncreaseDist = 5.f;
                m_SpeedDecreaseAmt  = amt * 3.f;
            } else {
                m_SpeedDecreaseDist = m_SpeedIncreaseDist = 4.f;
                m_SpeedDecreaseAmt  = amt * 1.5f;
            }
            m_bWillSlowDown = true;
        } else {
            m_bWillSlowDown = false;
        }
    }

    if (m_bSlowingDown) {
        m_bSlowingDown = false;
        m_bSpeedingUp  = true;
        return TASK_SIMPLE_GO_TO_POINT_FINE;
    }
    m_bSlowingDown = false;
    m_bSpeedingUp  = false;
    return subTaskType;
}

// 0x66EDC0
float CTaskComplexFollowNodeRoute::CalcBlendRatio(CPed* ped, bool slowing) const {
    // Como el original: distancia al cuadrado sumada en orden z, y, x
    float distSq, changeDist;
    if (slowing) {
        const CVector& pos = ped->GetPosition();
        const CVector& pt  = m_PtRoute->m_Entries.data()[m_CurrPtIdx];
        const float    dx = pt.x - pos.x, dy = pt.y - pos.y, dz = pt.z - pos.z;
        distSq     = dz * dz + dy * dy + dx * dx;
        changeDist = m_SpeedDecreaseDist;
    } else {
        const CVector& pos  = ped->GetPosition();
        const CVector  last = GetLastWaypoint(ped);
        const float    dx = pos.x - last.x, dy = pos.y - last.y, dz = pos.z - last.z;
        distSq     = dz * dz + dy * dy + dx * dx;
        changeDist = m_SpeedIncreaseDist;
    }
    const float changeDistSq = changeDist * changeDist;
    if (!(distSq < changeDistSq)) { // Como el original: también con NaN
        return -1.f;
    }
    float ratio = distSq / changeDistSq;
    if (ratio < 0.f) { // Como el original: NaN no se limita (test ah,41h)
        ratio = 0.f;
    } else if (ratio > 1.f) {
        ratio = 1.f;
    }
    const float change         = TrigCompoundCosHalf(ratio * FNR_PI, 0.5f) * (slowing ? m_SpeedDecreaseAmt : m_SpeedIncreaseAmt);
    const float ratioMoveState = CTaskSimpleGoToPointFine::BaseRatio(m_MoveState) - change;
    const float ratioMin       = CTaskSimpleGoToPointFine::BaseRatio(PEDMOVE_WALK) + 0.75f;
    return ratioMin > ratioMoveState ? ratioMin : ratioMoveState;
}

// 0x66EF20
bool CTaskComplexFollowNodeRoute::CanGoStraightThere(const CPed* ped, const CVector& from, const CVector& to, float maxDist) const {
    // Como el original: distancia (no al cuadrado) sumada en orden z, x, y; solo se sale si es mayor (NaN sigue, test ah,41h)
    const float dx = from.x - to.x, dy = from.y - to.y, dz = from.z - to.z;
    if (std::sqrt(dz * dz + dx * dx + dy * dy) > maxDist) {
        return false;
    }
    if (ped->bIgnoreHeightDifferenceFollowingNodes) {
        return true;
    }
    return CPedGeometryAnalyser::IsWanderPathClear(from, to, m_FollowNodeThresholdHeightChange, 4) == CPedGeometryAnalyser::WanderPathClearness::CLEAR;
}

// 0x671750
void CTaskComplexFollowNodeRoute::SetTarget(CPed* ped, const CVector& targetPt, float targetTolerance, float slowDownDistance, float followNodeThresholdHeightChange, bool bForce) {
    if (!bForce) {
        if (m_TargetPt == targetPt && m_TargetTolerance == targetTolerance && m_SlowDownDist == slowDownDistance && m_FollowNodeThresholdHeightChange == followNodeThresholdHeightChange) {
            return; // Nothing changed
        }
    }

    m_TargetPt                        = targetPt;
    m_TargetTolerance                 = targetTolerance;
    m_SlowDownDist                    = slowDownDistance;
    m_FollowNodeThresholdHeightChange = followNodeThresholdHeightChange;

    if (m_bUseBlending) {
        m_SpeedDecreaseDist = m_SpeedIncreaseDist = 0.f;
        m_SpeedDecreaseAmt  = m_SpeedIncreaseAmt  = 0.f;
        // Como el original: se limpia m_bSlowingDown pero m_bSpeedingUp se ACTIVA (and ~0x20; or 0x40)
        m_bSlowingDown = false;
        m_bSpeedingUp  = true;
    }

    ComputePathNodes(ped);
    ComputeRoute();

    m_bNewTarget = true;
}

// 0x669690
CTask* CTaskComplexFollowNodeRoute::CreateSubTask(eTaskType taskType, CPed* ped) const {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT_FINE:
        return new CTaskSimpleGoToPointFine{ CTaskSimpleGoToPointFine::BaseRatio(m_MoveState), GetCurrentPt(), 0.5f, nullptr };
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill{ m_MoveState, GetCurrentPt(), m_TargetTolerance, m_SlowDownDist, false, false };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{ m_MoveState, GetCurrentPt(), 0.5f, false, false };
    case TASK_SIMPLE_STAND_STILL: {
        // Como el original: la tarea se crea antes del Teleport
        const auto task = new CTaskSimpleStandStill{ 0, false, false, 8.f };
        ped->Teleport(m_TargetPt, false);
        return task;
    }
    default: // Como el original: cualquier otro tipo (TASK_FINISHED incluido) devuelve nullptr
        return nullptr;
    }
}

// 0x6698E0
CVector CTaskComplexFollowNodeRoute::GetLastWaypoint(CPed* ped) const {
    const auto pts = m_PtRoute->m_Entries.data();
    if (m_PtRoute->m_NumEntries == 1) {
        return pts[0];
    }
    return m_CurrPtIdx != 0
        ? pts[m_CurrPtIdx - 1]
        : ped->GetPosition();
}

// 0x669980
CVector CTaskComplexFollowNodeRoute::GetNextWaypoint(CPed* ped) const {
    const auto pts = m_PtRoute->m_Entries.data();
    if (m_PtRoute->m_NumEntries == 1) {
        return pts[0];
    }
    // Como el original: comparación con signo
    return (int32)(m_CurrPtIdx + 1) < (int32)m_PtRoute->m_NumEntries
        ? pts[m_CurrPtIdx + 1]
        : pts[m_CurrPtIdx - 1];
}

// 0x66EFA0
void CTaskComplexFollowNodeRoute::ComputePathNodes(const CPed* ped) {
    // Como el original: la ruta de nodos se vacía al principio
    m_NodeRoute->m_NumEntries = 0;

    // Make sure TargetPt.z is on the ground
    {
        bool hasTargetPtGroundZ = false;
        const float targetPtGroundZ = CWorld::FindGroundZFor3DCoord(CVector{ m_TargetPt.x, m_TargetPt.y, m_TargetPt.z + 1.f }, &hasTargetPtGroundZ, nullptr);
        if (hasTargetPtGroundZ) {
            m_TargetPt.z = targetPtGroundZ + 1.f;
        }
    }

    if (!CanGoStraightThere(ped, ped->GetPosition(), m_TargetPt, ped->GetIntelligence()->m_FollowNodeThresholdDistance)) {
        ThePaths.ComputeRoute(
            PATH_TYPE_PED,
            ped->GetPosition(),
            m_TargetPt,
            m_StartNode,
            m_NodeRoute
        );
        m_StartNode = s_EmptyNodeAddress;
    }

    // Como el original: se compara la distancia 2D del objetivo al primer nodo con la del objetivo al peatón (+3 o +50)
    const auto ClearIfFirstNodeFar = [this](const CVector& pedPos, float extra) {
        const auto first = m_NodeRoute->m_Entries[0];
        if (!FollowNodeRouteNodeLoaded(first)) {
            return;
        }
        const CVector nodePos  = FollowNodeRouteNodePos(first);
        const float   nx       = m_TargetPt.x - nodePos.x, ny = m_TargetPt.y - nodePos.y;
        const float   px       = m_TargetPt.x - pedPos.x, py = m_TargetPt.y - pedPos.y;
        const float   nodeDist = std::sqrt(nx * nx + ny * ny);
        const float   pedDist  = std::sqrt(px * px + py * py);
        if (pedDist + extra < nodeDist) {
            m_NodeRoute->m_NumEntries = 0;
        }
    };
    {
        const CVector& pedPos = ped->GetPosition();
        if (std::abs(pedPos.z - m_TargetPt.z) < m_FollowNodeThresholdHeightChange && (int32)m_NodeRoute->m_NumEntries > 0 && !m_bKeepNodesHeadingAwayFromTarget) {
            ClearIfFirstNodeFar(pedPos, 3.f); // 0x66F139
        } else if ((int32)m_NodeRoute->m_NumEntries > 0 && !m_bKeepNodesHeadingAwayFromTarget) {
            ClearIfFirstNodeFar(pedPos, 50.f); // 0x66F209
        }
    }

    if (!g_interiorMan.GetPedsInteriorGroup(ped)) { // 0x66F216
        // Make the route as short as possible by "cutting" down excess nodes
        // Como el original: una sola posición en la pila ([esp+18h]); si el nodo no está cargado se reutiliza la
        // anterior (con su z ya subida), o lo que hubiera en la pila si es el primero (bytes crudos: sin la
        // comprobación de variable sin inicializar de /RTC)
        alignas(CVector) uint8 nodePosStorage[sizeof(CVector)];
        auto& nodePos = *reinterpret_cast<CVector*>(nodePosStorage);
        for (int32 i = 0; i < (int32)m_NodeRoute->m_NumEntries; i++) {
            const auto node = m_NodeRoute->m_Entries.data()[i];

            if (FollowNodeRouteNodeLoaded(node)) {
                nodePos = FollowNodeRouteNodePos(node);
            }
            nodePos.z += 1.f;
            if (CanGoStraightThere(ped, nodePos, m_TargetPt, ped->GetIntelligence()->m_FollowNodeThresholdDistance)) {
                if (i + 1 <= 8) {
                    m_NodeRoute->m_NumEntries = i + 1;
                }
                break;
            }
        }
    } else if (m_NodeRoute->m_NumEntries != 0) {
        const auto routeFirstNode = m_NodeRoute->m_Entries[0];
        if (FollowNodeRouteNodeLoaded(routeFirstNode)) {
            const auto pedInt = g_interiorMan.GetPedsInterior(ped); // 0x66F2EE

            auto routeFirstNodeCoors = FollowNodeRouteNodePos(routeFirstNode);
            routeFirstNodeCoors.z += 1.f;
            const auto routeFirstNodeInt = g_interiorMan.GetVectorsInterior(routeFirstNodeCoors); // 0x66F330

            if (pedInt) {
                if (pedInt != routeFirstNodeInt && !ThePaths.These2NodesAreAdjacent(routeFirstNode, pedInt->GetNodeAddress())) {
                    m_NodeRoute->m_NumEntries = 0;
                    ThePaths.ComputeRoute( // 0x66F38D
                        PATH_TYPE_PED,
                        ped->GetPosition(),
                        m_TargetPt,
                        pedInt->GetNodeAddress(),
                        m_NodeRoute
                    );
                }
            } else if (routeFirstNodeInt && !ThePaths.These2NodesAreAdjacent(routeFirstNode, routeFirstNodeInt->GetNodeAddress())) {
                m_NodeRoute->m_NumEntries = 0;
                // Como el original: la ruta se calcula desde el objetivo hasta el peatón y luego se invierte
                ThePaths.ComputeRoute( // 0x66F3E0
                    PATH_TYPE_PED,
                    m_TargetPt,
                    ped->GetPosition(),
                    routeFirstNodeInt->GetNodeAddress(),
                    m_NodeRoute
                );
                // Como el original (0x667900): inversión en el sitio con contador con signo
                auto* const entries = m_NodeRoute->m_Entries.data();
                for (int32 i = 0, j = (int32)m_NodeRoute->m_NumEntries - 1; i < j; i++, j--) {
                    std::swap(entries[i], entries[j]);
                }
            }
        }
    }

    // 0x66F3F2 - Como el original: solo se comprueba m_wAreaId de m_CurrNode
    m_CurrPtIdx = 0;
    if (m_CurrNode.m_wAreaId != UINT16_MAX) {
        for (int32 i = 0; i < (int32)m_NodeRoute->m_NumEntries; i++) {
            if (m_NodeRoute->m_Entries.data()[i] == m_CurrNode) {
                m_CurrPtIdx = (uint32)i;
                break;
            }
        }
    }
    if ((int32)m_NodeRoute->m_NumEntries > 0) {
        m_CurrNode = m_NodeRoute->m_Entries.data()[m_CurrPtIdx];
    }
}

// 0x6699E0
void CTaskComplexFollowNodeRoute::ComputeRoute() {
    m_PtRoute->m_NumEntries = 0;
    // Como el original: contador leído una vez; cada punto se añade solo si hay sitio (contador con signo < 8)
    const auto numNodes = (int32)m_NodeRoute->m_NumEntries;
    for (int32 i = 0; i < numNodes; i++) {
        const auto node = m_NodeRoute->m_Entries.data()[i];
        if (!ThePaths.IsAreaNodesAvailable(node)) {
            continue;
        }
        const CVector nodePos = FollowNodeRouteNodePos(node);
        if ((int32)m_PtRoute->m_NumEntries < 8) {
            m_PtRoute->m_Entries.data()[m_PtRoute->m_NumEntries] = nodePos;
            m_PtRoute->m_NumEntries++;
        }
    }
    if ((int32)m_PtRoute->m_NumEntries < 8) {
        m_PtRoute->m_Entries.data()[m_PtRoute->m_NumEntries] = m_TargetPt;
        m_PtRoute->m_NumEntries++;
        m_LastRoutePointIsTarget = true;
    } else {
        m_LastRoutePointIsTarget = false;
    }
}

// 0x669520
bool CTaskComplexFollowNodeRoute::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (event && m_CurrPtIdx + 1 != m_PtRoute->m_NumEntries) {
        CVehicle* vehicle;
        switch (event->GetEventType()) {
        case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
            vehicle = static_cast<const CEventPotentialWalkIntoVehicle*>(event)->m_vehicle;
            break;
        case EVENT_VEHICLE_COLLISION:
            vehicle = static_cast<const CEventVehicleCollision*>(event)->m_vehicle;
            break;
        default:
            vehicle = nullptr;
            break;
        }
        // Como el original: el tipo de la subtarea se consulta antes de comprobar el vehículo
        const auto subTaskType = m_pSubTask->GetTaskType();
        if ((subTaskType == TASK_SIMPLE_GO_TO_POINT || subTaskType == TASK_SIMPLE_GO_TO_POINT_FINE) && m_pSubTask && vehicle) {
            // Como el original: velocidad al cuadrado (x, y, z) estrictamente menor que 0.015625
            const auto& spd = vehicle->m_vecMoveSpeed;
            if (spd.x * spd.x + spd.y * spd.y + spd.z * spd.z < FNR_SPEED_SQ_LIMIT) {
                const auto tGoToSubTask = static_cast<CTaskSimpleGoTo*>(m_pSubTask);
                if (CPedGeometryAnalyser::IsEntityBlockingTarget(vehicle, tGoToSubTask->m_vecTargetPoint, tGoToSubTask->m_fRadius)) {
                    const CVector d = ped->GetPosition() - vehicle->GetPosition();
                    tGoToSubTask->m_fRadius = std::sqrt(d.x * d.x + d.y * d.y);
                }
            }
        }
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x6718D0
CTask* CTaskComplexFollowNodeRoute::CreateNextSubTask(CPed* ped) {
    // Como el original: GetTaskType de la subtarea se llama en cada comprobación
    if (m_FinishedTimer.IsOutOfTime() && m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        ped->Teleport(m_TargetPt, false);
        return CreateSubTask(TASK_FINISHED, ped); // 0x671949
    }

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_LEAVE_CAR) { // 0x671969
        return ped->bInVehicle
            ? CreateSubTask(TASK_FINISHED, ped)
            : CreateFirstSubTask(ped);
    }

    eTaskType tt;
    if (m_LastRoutePointIsTarget) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: // 0x6719E5
            m_CurrPtIdx++;
            break;
        case TASK_SIMPLE_STAND_STILL: // 0x6719CC
            return CreateSubTask(TASK_FINISHED, ped);
        case TASK_SIMPLE_GO_TO_POINT:      // 0x6719F9 - Go to next node in the route
        case TASK_SIMPLE_GO_TO_POINT_FINE: // Como el original: mismo caso que TASK_SIMPLE_GO_TO_POINT
            m_CurrPtIdx++;
            if ((int32)m_CurrPtIdx < (int32)m_NodeRoute->m_NumEntries) {
                m_CurrNode = m_NodeRoute->m_Entries.data()[m_CurrPtIdx];
            }
            break;
        default:
            return nullptr; // 0x6719C4
        }
        tt = GetSubTaskType(m_CurrPtIdx, m_LastRoutePointIsTarget, *m_PtRoute);
    } else { // 0x671A32 - Proceed to the next point
        if (m_CurrPtIdx + 1 == m_PtRoute->m_NumEntries) { // Reached the last point in the route, calculate next few
            SetTarget(ped, m_TargetPt, m_TargetTolerance, m_SlowDownDist, m_FollowNodeThresholdHeightChange, true);
            return CreateFirstSubTask(ped);
        }
        m_CurrPtIdx++;
        m_CurrNode = m_NodeRoute->m_Entries.data()[m_CurrPtIdx];
        tt = GetSubTaskType(m_CurrPtIdx, m_LastRoutePointIsTarget, *m_PtRoute);
    }

    // Como el original: CalcGoToTaskType solo se usa CON mezcla
    return CreateSubTask(m_bUseBlending ? CalcGoToTaskType(ped, tt) : tt, ped);
}

// 0x671800
CTask* CTaskComplexFollowNodeRoute::CreateFirstSubTask(CPed* ped) {
    if (m_TimeToFinishMs >= 0) {
        m_FinishedTimer.Start(m_TimeToFinishMs);
    }

    // Como el original: solo se ponen a cero los flotantes (las banderas no cambian)
    if (m_bUseBlending) {
        m_SpeedDecreaseDist = m_SpeedIncreaseDist = 0.f;
        m_SpeedDecreaseAmt  = m_SpeedIncreaseAmt  = 0.f;
    }

    if (ped->bInVehicle) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    }

    SetTarget(ped, m_TargetPt, m_TargetTolerance, m_SlowDownDist, m_FollowNodeThresholdHeightChange, true);
    m_bNewTarget = false;

    const auto tt = GetSubTaskType(m_CurrPtIdx, m_LastRoutePointIsTarget, *m_PtRoute);
    // Como el original: CalcGoToTaskType solo se usa CON mezcla
    return CreateSubTask(m_bUseBlending ? CalcGoToTaskType(ped, tt) : tt, ped);
}

// 0x671AB0
CTask* CTaskComplexFollowNodeRoute::ControlSubTask(CPed* ped) {
    // Como el original: GetTaskType de la subtarea se llama en cada comprobación
    if (m_bNewTarget && m_pSubTask->GetTaskType() != TASK_COMPLEX_LEAVE_CAR) {
        return CreateFirstSubTask(ped);
    }

    if (m_FinishedTimer.IsOutOfTime() && m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)
            ? CreateSubTask(TASK_SIMPLE_STAND_STILL, ped)
            : m_pSubTask;
    }

    if (!m_bUseBlending) {
        return m_pSubTask;
    }

    // Como el original: si frena pero la subtarea no es GoToPointFine se pasa a la comprobación de acelerar
    // (0x671B6A -> 0x671C2E), que vuelve a llamar a GetTaskType
    if (m_bSlowingDown && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
        // Como el original: con ratio < 0 se usa el de la forma de moverse (0 y NaN se mantienen)
        auto br = CalcBlendRatio(ped, true);
        if (br < 0.f) {
            br = CTaskSimpleGoToPointFine::BaseRatio(m_MoveState);
        }
        static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask)->SetMoveRatio(br);
    } else if (m_bSpeedingUp) {
        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
            // Como el original: al acelerar se mide desde el punto anterior; con ratio < 0 (o NaN) se vuelve a ir normal
            const auto br = CalcBlendRatio(ped, false);
            if (!(br >= 0.f)) {
                m_bSpeedingUp = false;
                static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask)->SetMoveRatio(CTaskSimpleGoToPointFine::BaseRatio(m_MoveState));
                return CreateSubTask(TASK_SIMPLE_GO_TO_POINT, ped);
            }
            static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask)->SetMoveRatio(br);
        }
    }

    // If we're supposed to slow down (use `CTaskSimpleGoToPointFine`), but aren't yet
    if (!m_bWillSlowDown || m_bSlowingDown || m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) { // 0x671BA8
        return m_pSubTask;
    }
    const auto br = CalcBlendRatio(ped, true);
    if (!(br >= 0.f)) { // Blend ratios under 0 use the standard `CTaskSimpleGoToPoint` [Como el original: también NaN]
        return m_pSubTask;
    }
    m_bWillSlowDown = false;
    m_bSpeedingUp   = false;
    m_bSlowingDown  = true;

    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
        static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask)->SetMoveRatio(br);
        return m_pSubTask;
    }
    const auto tGoToPtFine = CreateSubTask(TASK_SIMPLE_GO_TO_POINT_FINE, ped);
    static_cast<CTaskSimpleGoToPointFine*>(tGoToPtFine)->SetMoveRatio(br);
    return tGoToPtFine;
}
