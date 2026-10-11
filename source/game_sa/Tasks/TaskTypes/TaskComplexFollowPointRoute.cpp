/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowPointRoute
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TrigSimpleCpp.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskSimpleGoToPointFine.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexWalkRoundCar.h"

void CTaskComplexFollowPointRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPointRoute, 0x870078, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x671510);

    RH_ScopedInstall(GetSubTaskType, 0x668FE0);
    RH_ScopedInstall(CreateTask, 0x669090);
    RH_ScopedInstall(GetLastWaypoint, 0x669340);
    RH_ScopedInstall(GetNextWaypoint, 0x669420);
    RH_ScopedInstall(CalcGoToTaskType, 0x66E150);
    RH_ScopedInstall(CalcBlendRatio, 0x66E710);

    RH_ScopedVMTInstall(Clone, 0x673BE0);
    RH_ScopedVMTInstall(GetTaskType, 0x66E0B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x668EC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x671630);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66E320);
    RH_ScopedVMTInstall(ControlSubTask, 0x66E860);
}

// 0x671510
CTaskComplexFollowPointRoute::CTaskComplexFollowPointRoute(
    eMoveState moveState,
    CPointRoute const& route,
    Mode mode,
    float targetRadius,
    float slowDownDist,
    bool overshootTarget,
    bool useBlending,
    bool standStillAfterMove
) {
    // Como el original: solo se escriben los bits 0, 2 y 3 de las banderas (el resto conserva lo que hubiera en memoria)
    // y los cuatro valores de mezcla solo se ponen a cero si se usa la mezcla
    m_mode      = mode;
    m_moveState = moveState;
    m_bMustOvershootTarget = overshootTarget;
    m_bStandStillAfterMove = standStillAfterMove;
    m_bUseBlending         = useBlending;
    if (m_bUseBlending) {
        m_bSlowingDown = false;
        m_bSpeedingUp  = false;
        m_speedDecreaseDist = 0.f;
        m_speedIncreaseDist = 0.f;
        m_speedDecreaseAmt  = 0.f;
        m_speedIncreaseAmt  = 0.f;
    }

    // Como el original: CPointRoute::operator new (0x41B7C0), contador a cero y copia en linea 0x63A040,
    // que solo copia las entradas usadas (bucle con signo)
    auto* const newRoute = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (newRoute) {
        newRoute->m_NumEntries = 0;
        newRoute->m_NumEntries = route.m_NumEntries;
        for (int32 i = 0; i < (int32)newRoute->m_NumEntries; i++) {
            newRoute->m_Entries.data()[i] = route.m_Entries.data()[i];
        }
    }
    m_route = newRoute;

    m_slowDownDistance = slowDownDist;
    m_targetRadius     = targetRadius;
    m_bNewRoute        = true;
    m_currPointIdx     = 0;
    m_nRouteTraversals = 0;
    if (m_bUseBlending) {
        m_bSlowingDown = false;
        m_bSpeedingUp  = true;
        m_speedDecreaseDist = 0.f;
        m_speedIncreaseDist = 0.f;
        m_speedDecreaseAmt  = 0.f;
        m_speedIncreaseAmt  = 0.f;
    }
}

CTaskComplexFollowPointRoute::~CTaskComplexFollowPointRoute() {
    if (m_route) { // CPool::Delete ya no comprueba nullptr (como el EXE)
        delete m_route;
    }
}

CTaskComplexFollowPointRoute::CTaskComplexFollowPointRoute(const CTaskComplexFollowPointRoute& o) :
    CTaskComplexFollowPointRoute{
        o.m_moveState,
       *o.m_route,
        o.m_mode,
        o.m_targetRadius,
        o.m_slowDownDistance,
        o.m_bMustOvershootTarget,
        o.m_bUseBlending,
        o.m_bStandStillAfterMove
    }
{
}

// 0x668FE0
eTaskType CTaskComplexFollowPointRoute::GetSubTaskType() {
    const auto numEntries = (int32)m_route->m_NumEntries;
    if (numEntries == 0) {
        return TASK_FINISHED;
    }

    // Como el original: comparaciones con signo
    const auto curr = (int32)m_currPointIdx;
    if (curr + 1 < numEntries) { // Current is not the last
        return TASK_SIMPLE_GO_TO_POINT;
    }

    if (curr + 1 == numEntries) { // Current will the last point
        return m_bStandStillAfterMove ? TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL : TASK_SIMPLE_GO_TO_POINT;
    }

    if (curr == numEntries) { // Current is the last point
        m_nRouteTraversals++;

        switch (m_mode) {
        case Mode::ONE_WAY: {
            if (m_bUseBlending) {
                m_bWillSlowDown = m_bSlowingDown = m_bSpeedingUp = false;
            }
            return TASK_FINISHED;
        }
        case Mode::RETURN: {
            if (m_nRouteTraversals != 1) {
                return TASK_FINISHED;
            }
            [[fallthrough]];
        }
        case Mode::BACK_AND_FORTH: {
            m_route->Reverse();
            [[fallthrough]];
        }
        case Mode::LOOP: {
            m_currPointIdx = 0;
            return GetSubTaskType(); // Recurse
        }
        }
    }

    return TASK_NONE;
}

// 0x669090
CTask* CTaskComplexFollowPointRoute::CreateTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT_FINE: {
        const auto task = new CTaskSimpleGoToPointFine{ CTaskSimpleGoToPointFine::BaseRatio(m_moveState), m_route->m_Entries.data()[m_currPointIdx] };
        // Como el original: si se esta frenando y la subtarea actual es GO_TO_POINT, se copian sus 4 bits de objetivo rodeado
        if (m_bSlowingDown && m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            task->gotoFlags.m_targetCircledFlags = static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->gotoFlags.m_targetCircledFlags;
        }
        return task;
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        // Como el original: la distancia y la consulta al padre se calculan antes de reservar la tarea
        const auto moveStateRadius = [this] {
            switch (m_mode) {
            case Mode::RETURN:
                return m_nRouteTraversals ? m_slowDownDistance : 0.f;
            case Mode::LOOP:
            case Mode::BACK_AND_FORTH:
                return 0.f;
            default:
                return m_slowDownDistance;
            }
        }();
        const bool goingForDoor = m_Parent && m_Parent->GetTaskType() == TASK_COMPLEX_WALK_ROUND_CAR && static_cast<CTaskComplexWalkRoundCar*>(m_Parent)->GoingForDoor();
        return new CTaskComplexGoToPointAndStandStill{
            m_moveState,
            m_route->m_Entries.data()[m_currPointIdx],
            m_targetRadius,
            moveStateRadius,
            false,
            goingForDoor
        };
    }
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{
            m_moveState,
            m_route->m_Entries.data()[m_currPointIdx],
            0.5f,
            false,
            m_bMustOvershootTarget
        };
    case TASK_NONE:
        return new CTaskSimpleLeaveGroup{};
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{ STAND_STILL_TIME };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    default: // Como el original: cualquier otro tipo (incluido TASK_FINISHED) no crea nada
        return nullptr;
    }
}

// 0x669340
CVector CTaskComplexFollowPointRoute::GetLastWaypoint(CPed* ped) {
    const auto* entries = m_route->m_Entries.data();
    if (m_route->m_NumEntries == 1) {
        return entries[0];
    }

    // Como el original: un modo fuera de [0, 3] devuelve el vector nulo
    if ((int32)m_mode < 0 || (int32)m_mode > 3) {
        return CVector{ 0.f, 0.f, 0.f };
    }

    if (m_currPointIdx) {
        return entries[m_currPointIdx - 1];
    }

    // Como el original: comparacion con signo de las vueltas recorridas
    if (m_nRouteTraversals > 0) {
        return entries[m_route->m_NumEntries - 1]; // Last point was the previous
    }
    return ped->GetPosition();
}

// 0x669420
CVector CTaskComplexFollowPointRoute::GetNextWaypoint(CPed* ped) {
    const auto* entries    = m_route->m_Entries.data();
    const auto  numEntries = (int32)m_route->m_NumEntries;
    if (numEntries == 1) {
        return entries[0];
    }

    const auto curr = (int32)m_currPointIdx;
    switch ((int32)m_mode) {
    case 0:
    case 1:
    case 2: // Como el original: el siguiente si existe; si no, el anterior
        return entries[curr + 1 < numEntries ? curr + 1 : curr - 1];
    case 3: // Como el original: en bucle, despues del ultimo vuelve al primero
        return entries[curr + 1 == numEntries ? 0 : curr + 1];
    default:
        return CVector{ 0.f, 0.f, 0.f };
    }
}

// 0x66E150
eTaskType CTaskComplexFollowPointRoute::CalcGoToTaskType(CPed* ped, eTaskType subTaskType) {
    // Como el original: se copia la cantidad de frenado en la de aceleracion y se borra "va a frenar"
    m_speedIncreaseAmt = m_speedDecreaseAmt;
    m_bWillSlowDown    = false;

    if ((int32)m_moveState > PEDMOVE_WALK) {
        const auto numEntries = (int32)m_route->m_NumEntries;
        auto currPt = (int32)m_currPointIdx;
        if (currPt >= numEntries) {
            currPt = numEntries - 1;
        }
        if (currPt < 0) {
            return subTaskType;
        }

        const auto&   currPtPos = m_route->m_Entries.data()[currPt];
        const CVector last      = GetLastWaypoint(ped);
        CVector toCurr{ currPtPos.x - last.x, currPtPos.y - last.y, currPtPos.z - last.z };
        toCurr.z = 0.f;
        toCurr.Normalise();
        const CVector next = GetNextWaypoint(ped);
        CVector toNext{ next.x - currPtPos.x, next.y - currPtPos.y, 0.f };
        toNext.Normalise();

        // Como el original: orden y, x, z del producto escalar
        const float dot = toCurr.y * toNext.y + toCurr.x * toNext.x + toNext.z * toCurr.z;
        if (dot < std::bit_cast<float>(0x3F34FDF4u)) { // 0.707f (0x863C1C)
            float t = 1.f - (dot + 1.f) * std::bit_cast<float>(0x3F15F880u); // 0x8700A4
            if (t < 0.f) {
                t = 0.f;
            } else if (t > 1.f) {
                t = 1.f;
            }
            if (m_moveState == PEDMOVE_SPRINT) {
                m_speedDecreaseDist = 5.f;
                m_speedIncreaseDist = 5.f;
                m_speedDecreaseAmt  = t * 3.f;
            } else {
                m_speedDecreaseDist = 4.f;
                m_speedIncreaseDist = 4.f;
                m_speedDecreaseAmt  = t * 1.5f;
            }
            m_bWillSlowDown = true;
        } else {
            m_bWillSlowDown = false;
        }
    }

    if (m_bSlowingDown && (subTaskType == TASK_SIMPLE_GO_TO_POINT || subTaskType == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL)) {
        m_bSlowingDown = false;
        m_bSpeedingUp  = true;
        return TASK_SIMPLE_GO_TO_POINT_FINE;
    }
    m_bSlowingDown = false;
    m_bSpeedingUp  = false;
    return subTaskType;
}

// Como el original: fcos del x87 con precision interna completa (el llamador redondea a float)

// 0x66E710
float CTaskComplexFollowPointRoute::CalcBlendRatio(CPed* ped, bool slowing) {
    float distSq, requiredDist;
    if (slowing) {
        const auto& pedPos = ped->GetPosition();
        const auto& point  = m_route->m_Entries.data()[m_currPointIdx];
        const float dx = point.x - pedPos.x, dy = point.y - pedPos.y, dz = point.z - pedPos.z;
        distSq       = dz * dz + dy * dy + dx * dx; // Como el original: orden z, y, x
        requiredDist = m_speedDecreaseDist;
    } else {
        const auto&   pedPos = ped->GetPosition();
        const CVector last   = GetLastWaypoint(ped);
        const float dx = pedPos.x - last.x, dy = pedPos.y - last.y, dz = pedPos.z - last.z;
        distSq       = dz * dz + dy * dy + dx * dx;
        requiredDist = m_speedIncreaseDist;
    }

    const float requiredDistSq = requiredDist * requiredDist;
    if (!(distSq < requiredDistSq)) { // Como el original: tambien con NaN
        return -1.f;
    }

    float progress = distSq / requiredDistSq;
    if (progress < 0.f) {
        progress = 0.f;
    } else if (progress > 1.f) {
        progress = 1.f;
    }
    const float angle  = progress * std::bit_cast<float>(0x40490FDBu); // PI (0x858CB8)
    const float cosine = TrigSimpleCosCpp(angle);
    float mult = cosine * 0.5f + 0.5f;
    mult *= slowing ? m_speedDecreaseAmt : m_speedIncreaseAmt;

    const float ratio = CTaskSimpleGoToPointFine::BaseRatio(m_moveState) - mult;
    const float walk  = CTaskSimpleGoToPointFine::BaseRatio(PEDMOVE_WALK);
    return walk > ratio ? walk : ratio;
}

// 0x668EC0
bool CTaskComplexFollowPointRoute::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    // Adjust GoTo task's radius if event is `CEventVehicleCollision` or `CEventPotentialWalkIntoVehicle`
    if (event && (int32)m_currPointIdx + 1 < (int32)m_route->m_NumEntries) {
        CVehicle* vehicle;
        switch (event->GetEventType()) {
        case EVENT_VEHICLE_COLLISION:
            vehicle = static_cast<const CEventVehicleCollision*>(event)->m_vehicle;
            break;
        case EVENT_POTENTIAL_WALK_INTO_VEHICLE:
            vehicle = static_cast<const CEventPotentialWalkIntoVehicle*>(event)->m_vehicle;
            break;
        default:
            vehicle = nullptr;
            break;
        }
        // Como el original: el tipo de la subtarea se consulta aunque no haya vehiculo
        const auto subTaskType = m_pSubTask->GetTaskType();
        if ((subTaskType == TASK_SIMPLE_GO_TO_POINT || subTaskType == TASK_SIMPLE_GO_TO_POINT_FINE) && m_pSubTask && vehicle) {
            const auto  gotoPointTask = static_cast<CTaskSimpleGoToPoint*>(m_pSubTask);
            const auto& speed         = vehicle->m_vecMoveSpeed;
            if (speed.x * speed.x + speed.y * speed.y + speed.z * speed.z < 0.015625f) { // 0.125^2 (0x86FC74)
                if (CPedGeometryAnalyser::IsEntityBlockingTarget(vehicle, gotoPointTask->m_vecTargetPoint, gotoPointTask->m_fRadius)) {
                    const CVector d = ped->GetPosition() - vehicle->GetPosition();
                    gotoPointTask->m_fRadius = std::sqrt(d.x * d.x + d.y * d.y);
                }
            }
        }
    }

    if (priority == ABORT_PRIORITY_LEISURE) {
        m_route->m_NumEntries = 0;
    }

    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x671630
CTask* CTaskComplexFollowPointRoute::CreateNextSubTask(CPed* ped) {
    // Como el original: el tipo de la subtarea se vuelve a consultar en cada comprobacion
    if (m_pSubTask->GetTaskType() == TASK_FINISHED) {
        return nullptr;
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_LEAVE_CAR) {
        return ped->bInVehicle ? nullptr : CreateFirstSubTask(ped);
    }
    // Como el original: tras TASK_NONE o STAND_STILL la ruta termina siempre
    if (m_pSubTask->GetTaskType() == TASK_NONE) {
        return CreateTask(TASK_FINISHED, ped);
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return CreateTask(TASK_FINISHED, ped);
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
        if ((int32)m_currPointIdx + 1 == (int32)m_route->m_NumEntries && !m_bStandStillAfterMove) {
            return CreateTask(TASK_FINISHED, ped);
        }
    }

    // Otherwise advance to the next point..
    m_currPointIdx++;

    auto nextSubTask = GetSubTaskType();
    if (m_bUseBlending) {
        nextSubTask = CalcGoToTaskType(ped, nextSubTask);
    }
    return CreateTask(nextSubTask, ped);
}

// 0x66E320
CTask* CTaskComplexFollowPointRoute::CreateFirstSubTask(CPed* ped) {
    m_bNewRoute = false;

    if (m_bUseBlending) {
        m_speedDecreaseDist = 0.f;
        m_speedIncreaseDist = 0.f;
        m_speedDecreaseAmt  = 0.f;
        m_speedIncreaseAmt  = 0.f;
    }

    const auto npts = (int32)m_route->m_NumEntries;
    if (npts == 0) {
        return CreateTask(TASK_NONE, ped);
    }

    if (ped->bInVehicle) {
        return CreateTask(TASK_COMPLEX_LEAVE_CAR, ped);
    }

    // Find next point to follow
    if (m_mode != Mode::ONE_WAY) {
        const auto& pedPos        = ped->GetPosition();
        const auto* entries       = m_route->m_Entries.data();
        auto        closestIdx    = -1;
        auto        closestDistSq = std::numeric_limits<float>::max();

        for (auto i = 0; i < npts; i++) {
            const auto  nextPtIdx = (i + 1) % npts;
            const auto& thisPtPos = entries[i % npts];
            const auto& nextPtPos = entries[nextPtIdx];
            CVector dir{ nextPtPos.x - thisPtPos.x, nextPtPos.y - thisPtPos.y, nextPtPos.z - thisPtPos.z };
            const float segLenSq = dir.z * dir.z + dir.x * dir.x + dir.y * dir.y; // Como el original: orden z, x, y
            dir.Normalise();

            // Como el original: orden z, y, x del producto escalar
            const float dot = (pedPos.z - thisPtPos.z) * dir.z + (pedPos.y - thisPtPos.y) * dir.y + (pedPos.x - thisPtPos.x) * dir.x;
            if (!(dot > 0.f)) {
                continue;
            }
            if (!(std::sqrt(segLenSq) > dot)) { // Como el original: se compara la longitud (fsqrt), no los cuadrados
                continue;
            }

            const float projX  = dir.x * dot + thisPtPos.x;
            const float projY  = dir.y * dot + thisPtPos.y;
            const float projZ  = dir.z * dot + thisPtPos.z;
            const float dx     = pedPos.x - projX, dy = pedPos.y - projY, dz = pedPos.z - projZ;
            const float distSq = dz * dz + dy * dy + dx * dx; // Como el original: orden z, y, x
            if (distSq < closestDistSq * closestDistSq) { // Como el original: se compara con el cuadrado del mejor valor
                closestDistSq = distSq;
                closestIdx    = nextPtIdx;
            }
        }

        // If above loop didn't manage to find a point, try this
        if (closestIdx == -1) {
            auto closestDist = std::numeric_limits<float>::max();
            for (auto i = 0; i < npts; i++) {
                const float dx = pedPos.x - entries[i].x, dy = pedPos.y - entries[i].y, dz = pedPos.z - entries[i].z;
                const float distSq = dz * dz + dy * dy + dx * dx;
                if (distSq < closestDist * closestDist) {
                    closestDist = distSq;
                    closestIdx  = i;
                }
            }
        }

        // Como el original: sin comprobar que se encontrara un punto
        m_currPointIdx = (uint32)closestIdx;
    }

    // Same code found in `CreateNextSubTask`
    auto nextSubTask = GetSubTaskType();
    if (m_bUseBlending) {
        nextSubTask = CalcGoToTaskType(ped, nextSubTask);
    }
    return CreateTask(nextSubTask, ped);
}

// 0x66E860
CTask* CTaskComplexFollowPointRoute::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_LEAVE_CAR) {
        return m_pSubTask;
    }

    // Como el original: contador estatico contra el valor global 30 (0x86C938) y numero de puntos con signo
    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30 || (int32)m_route->m_NumEntries <= 0) {
        return nullptr;
    }

    // Como el original: una ruta nueva vuelve a empezar desde CreateFirstSubTask
    if (m_bNewRoute) {
        return CreateFirstSubTask(ped);
    }

    if (m_bUseBlending && m_pSubTask->GetTaskType() == TASK_NONE) {
        m_bWillSlowDown = m_bSlowingDown = m_bSpeedingUp = false;
        return m_pSubTask;
    }

    if (!m_bUseBlending) {
        return m_pSubTask;
    }

    const auto gotoPointFine = static_cast<CTaskSimpleGoToPointFine*>(m_pSubTask);
    if (m_bSlowingDown && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
        auto ratio = CalcBlendRatio(ped, true);
        if (ratio < 0.f) {
            ratio = CTaskSimpleGoToPointFine::BaseRatio(m_moveState);
        }
        gotoPointFine->SetMoveRatio(ratio);
    } else if (m_bSpeedingUp && m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
        // Como el original: al acelerar se mide desde el punto anterior (false) y se aplica la proporcion
        const auto ratio = CalcBlendRatio(ped, false);
        if (ratio >= 0.f) {
            gotoPointFine->SetMoveRatio(ratio);
        } else {
            m_bSpeedingUp = false;
            gotoPointFine->SetMoveRatio(CTaskSimpleGoToPointFine::BaseRatio(m_moveState));
            return CreateTask(TASK_SIMPLE_GO_TO_POINT, ped);
        }
    }

    if (m_bWillSlowDown && !m_bSlowingDown) {
        const auto ratio = CalcBlendRatio(ped, true);
        if (ratio >= 0.f) {
            // Como el original: pasa a frenar y deja de acelerar
            m_bWillSlowDown = false;
            m_bSpeedingUp   = false;
            m_bSlowingDown  = true;
            if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE) {
                gotoPointFine->SetMoveRatio(ratio);
            } else {
                const auto task = static_cast<CTaskSimpleGoToPointFine*>(CreateTask(TASK_SIMPLE_GO_TO_POINT_FINE, ped));
                task->SetMoveRatio(ratio);
                return task;
            }
        }
    }
    return m_pSubTask;
}
