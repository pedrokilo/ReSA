/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToCarDoorAndStandStill
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskSimpleGoToPoint.h"

bool __fastcall GoToCarDoorIsVehicleInRange(CTaskComplexGoToCarDoorAndStandStill* task, void*, const CPed* ped) {
    return task->IsVehicleInRange(*ped);
}

// Como el original: 0x6458A0 solo toca eax y la pila x87; CreateFirstSubTask (0x64D45C) y ControlSubTask (0x64A94A)
// llaman a CreateSubTask justo después sin recargar ecx, así que el envoltorio conserva ecx y edx.
static __declspec(naked) void GoToCarDoorIsVehicleInRangeKeepRegs() {
    __asm {
        push   ecx
        push   edx
        push   dword ptr [esp + 0Ch]
        call   GoToCarDoorIsVehicleInRange
        movzx  eax, al
        pop    edx
        pop    ecx
        ret    4
    }
}

void CTaskComplexGoToCarDoorAndStandStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToCarDoorAndStandStill, 0x86ec4c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x645780);
    RH_ScopedInstall(Destructor, 0x64A580);

    RH_ScopedNamedGlobalInstall(GoToCarDoorIsVehicleInRangeKeepRegs, "IsVehicleInRange", 0x6458A0);
    RH_ScopedInstall(ComputeRouteToDoor, 0x645910);
    RH_ScopedInstall(CreateSubTask, 0x64A5F0);

    RH_ScopedVMTInstall(Clone, 0x6498B0);
    RH_ScopedVMTInstall(GetTaskType, 0x645830);
    RH_ScopedVMTInstall(MakeAbortable, 0x645840);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x64D2B0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x64D440);
    RH_ScopedVMTInstall(ControlSubTask, 0x64A820);
}

// 0x645780
CTaskComplexGoToCarDoorAndStandStill::CTaskComplexGoToCarDoorAndStandStill(
    CVehicle*  vehicle,
    eMoveState moveState,
    bool       bIsDriver,
    int32      targetSeat,
    float      targetRadius,
    float      slowDownDist,
    float      maxSeekDist,
    int32      maxSeekTime
) :
    m_Vehicle{vehicle},
    m_MoveState{moveState},
    m_bIsDriver{bIsDriver},
    m_TargetRadius{targetRadius},
    m_SlowDownDist{slowDownDist},
    m_MaxSeekDist{maxSeekDist},
    m_MaxSeekTime{maxSeekTime},
    m_TargetSeat{targetSeat}
{
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x6498B0
CTaskComplexGoToCarDoorAndStandStill::CTaskComplexGoToCarDoorAndStandStill(const CTaskComplexGoToCarDoorAndStandStill& o) :
    CTaskComplexGoToCarDoorAndStandStill{ o.m_Vehicle, o.m_MoveState, o.m_bIsDriver, o.m_TargetSeat, o.m_TargetRadius, o.m_SlowDownDist, o.m_MaxSeekDist, o.m_MaxSeekTime }
{
}

// 0x64A580
CTaskComplexGoToCarDoorAndStandStill::~CTaskComplexGoToCarDoorAndStandStill() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    if (m_RouteToDoor) { // Como el original: sin llamar a operator delete con la ruta nula
        delete m_RouteToDoor;
    }
}

// 0x645840
bool CTaskComplexGoToCarDoorAndStandStill::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_bTryingToEnterInWater && m_pSubTask && m_pSubTask->GetTaskType() == TASK_SIMPLE_PAUSE && event) {
        // Como el original: pide el tipo del evento dos veces (EVENT_IN_WATER y después EVENT_IN_AIR)
        if (event->GetEventType() == EVENT_IN_WATER || event->GetEventType() == EVENT_IN_AIR) {
            return false;
        }
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x64A5F0
CTask* CTaskComplexGoToCarDoorAndStandStill::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE: {
        ped->bHasJustLeftCar = true;
        return new CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse{ m_Vehicle, m_TargetDoor, m_TargetPt, m_MoveState };
    }
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ 1 };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{ 1 };
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        return new CTaskSimpleCarWaitForDoorNotToBeInUse{ m_Vehicle, (uint32)m_TargetDoor };
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return new CTaskComplexFollowPointRoute{ m_MoveState, *m_RouteToDoor, CTaskComplexFollowPointRoute::Mode::ONE_WAY, 0.5f, 5.f, false, false, false };
    case TASK_SIMPLE_GO_TO_POINT: {
        ped->bHasJustLeftCar = true;
        return new CTaskSimpleGoToPoint{ m_MoveState, m_TargetPt, m_TargetRadius, true };
    }
    default: // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nullptr
        return nullptr;
    }
}

// 0x64D2B0
CTask* CTaskComplexGoToCarDoorAndStandStill::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE: { // 0x64D3B8
        m_TargetPt = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
        return CreateSubTask(TASK_SIMPLE_GO_TO_POINT, ped);
    }
    case TASK_SIMPLE_GO_TO_POINT: { // 0x64D3E9
        // Como el original: solo escribe la bandera cuando se alcanza la puerta
        if (static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->WasTaskSuccessful(ped)) {
            m_bAchievedTargetDoor = true;
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE: // 0x64D387
        return CreateSubTask(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE, ped);
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: // 0x64D2EB
        return CreateFirstSubTask(ped);
    case TASK_SIMPLE_STAND_STILL: // 0x64D420
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_PAUSE: { // 0x64D314
        if (m_bTryingToEnterInWater) {
            // Como el original: raíz de x·x + y·y estrictamente menor que 0,5
            const CVector d = m_TargetPt - ped->GetPosition();
            if (std::sqrt(d.x * d.x + d.y * d.y) < 0.5f) {
                m_bAchievedTargetDoor = true;
            }
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    default: // Como el original: devuelve nullptr
        return nullptr;
    }
}

// 0x64D440
CTask* CTaskComplexGoToCarDoorAndStandStill::CreateFirstSubTask(CPed* ped) {
    if (!IsVehicleInRange(*ped)) {
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }

    if (m_bTryingToEnterInWater) {
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }

    // Como el original: el temporizador se arranca sin comprobar el intervalo
    const auto StartSeekTimer = [this] {
        m_SeekTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_SeekTimer.m_nInterval  = m_MaxSeekTime;
        m_SeekTimer.m_bStarted   = true;
    };
    // Si el camino está bloqueado hay que rodear el vehículo primero
    const auto GoToDoor = [&](eTaskType straight) {
        if (CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel(ped, m_Vehicle, m_TargetPt)) {
            ComputeRouteToDoor(*ped);
            return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, ped); // 0x64D4E7
        }
        return CreateSubTask(straight, ped);
    };

    if (m_bIsDriver) {
        if (m_TargetSeat != 0) {
            // Como el original: con asiento fijo de conductor no arranca el temporizador
            m_TargetDoor = m_TargetSeat;
            m_TargetPt   = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetSeat);
            return GoToDoor(TASK_SIMPLE_GO_TO_POINT);
        }
        if (!CCarEnterExit::GetNearestCarDoor(ped, m_Vehicle, m_TargetPt, m_TargetDoor)) {
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
        StartSeekTimer();
        return GoToDoor(TASK_SIMPLE_GO_TO_POINT);
    }

    if (m_TargetSeat == 0) {
        if (CCarEnterExit::GetNearestCarPassengerDoor(ped, m_Vehicle, &m_TargetPt, &m_TargetDoor, true, true, true)) {
            StartSeekTimer();
            return GoToDoor(TASK_SIMPLE_GO_TO_POINT);
        }
        if (!CCarEnterExit::GetNearestCarPassengerDoor(ped, m_Vehicle, &m_TargetPt, &m_TargetDoor, true, false, true)) {
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
        return GoToDoor(TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE);
    }

    // Como el original: sin comprobar el índice y con la bandera bDontDragMeOutCar (bit 29 de 0x46C)
    if (const auto psgrAtDoor = m_Vehicle->m_apPassengers.data()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Vehicle, m_TargetSeat)]) {
        if (psgrAtDoor->bDontDragMeOutCar) {
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
        }
    }
    m_TargetDoor = m_TargetSeat;
    m_TargetPt   = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetSeat);
    StartSeekTimer();
    return GoToDoor(TASK_SIMPLE_GO_TO_POINT);
}

// 0x64A820
CTask* CTaskComplexGoToCarDoorAndStandStill::ControlSubTask(CPed* ped) {
    if (!m_Vehicle) {
        return nullptr;
    }

    if (!ped->IsCop()) {
        if (const auto drvr = m_Vehicle->m_pDriver) {
            if (drvr->IsPlayer() && m_Vehicle->m_pDriver->m_nPedState == PEDSTATE_ARRESTED) {
                return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x64A875
            }
        }
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE:
    case TASK_SIMPLE_PAUSE:
    case TASK_SIMPLE_STAND_STILL:
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: { // 0x64A890
        if (m_TargetDoor != 0) { // TODO: Enum
            m_TargetPt = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
        }
        return m_pSubTask;
    }
    case TASK_SIMPLE_GO_TO_POINT: { // 0x64AA04
        // Como el original: la posición de la puerta se calcula antes de comprobar la distancia
        const auto doorOpenPos = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
        CTask* result;
        if (!IsVehicleInRange(*ped)) {
            m_TargetPt = doorOpenPos;
            result = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x64AA50
        } else if (m_TargetSeat != 0) {
            // Como el original: con asiento fijo usa la puerta actual (no la del asiento)
            m_TargetPt = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);
            static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(m_TargetPt, m_TargetRadius, false);
            result = m_pSubTask;
        } else {
            CVector pt; // Como el original: sin inicializar (lo escriben las funciones de puertas)
            int32   door = 0;
            const bool found = m_bIsDriver
                ? CCarEnterExit::GetNearestCarDoor(ped, m_Vehicle, pt, door)
                : CCarEnterExit::GetNearestCarPassengerDoor(ped, m_Vehicle, &pt, &door, true, true, true);
            if (found) {
                m_TargetPt   = pt;
                m_TargetDoor = door;
                static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(m_TargetPt, m_TargetRadius, false);
                result = m_pSubTask;
            } else if (m_bIsDriver) {
                m_TargetPt = pt;
                result = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
            } else if (CCarEnterExit::GetNearestCarPassengerDoor(ped, m_Vehicle, &pt, &door, true, false, true)) {
                m_TargetPt   = pt;
                m_TargetDoor = door;
                result = CreateSubTask(TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE, ped);
            } else {
                m_TargetPt = pt;
                result = CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
            }
        }
        ped->bPushOtherPeds = true; // Como el original: al final de todos los caminos
        return result;
    }
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE: { // 0x64A8FA
        const auto doorOpenPos = CCarEnterExit::GetPositionToOpenCarDoor(m_Vehicle, m_TargetDoor);

        if (!IsVehicleInRange(*ped)) {
            m_TargetPt = doorOpenPos;
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped); // 0x64A94A
        }

        if (!CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel(ped, m_Vehicle, doorOpenPos)) {
            m_TargetPt = doorOpenPos;
            return CreateSubTask(TASK_SIMPLE_GO_TO_POINT, ped); // 0x64A990
        }

        // Como el original: Magnitude() (0x406DA0) estrictamente mayor que 0,01 (0x86EC40)
        if ((m_TargetPt - doorOpenPos).Magnitude() > 0.01f) { // 0x64A9D1
            m_TargetPt = doorOpenPos;
            return CreateFirstSubTask(ped);
        }

        return m_pSubTask;
    }
    default: // Como el original: devuelve nullptr
        return nullptr;
    }
}

// 0x6458A0
bool CTaskComplexGoToCarDoorAndStandStill::IsVehicleInRange(const CPed& ped) {
    // Como el original: vehículo menos peatón, (z·z + x·x) + y·y estrictamente menor que el cuadrado del radio
    const CVector d = m_Vehicle->GetPosition() - ped.GetPosition();
    return d.z * d.z + d.x * d.x + d.y * d.y < m_MaxSeekDist * m_MaxSeekDist;
}

// 0x645910
void CTaskComplexGoToCarDoorAndStandStill::ComputeRouteToDoor(const CPed& ped) {
    if (!m_RouteToDoor) {
        // Como el original: el pool solo devuelve memoria y únicamente se escribe el contador
        m_RouteToDoor = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
        if (m_RouteToDoor) {
            m_RouteToDoor->m_NumEntries = 0;
        }
    }
    m_RouteToDoor->Clear();

    const CVector pedPos = ped.GetPosition();

    // Como el original: primero los lados y después los planos
    const auto pedSide  = CPedGeometryAnalyser::ComputeEntityHitSide(pedPos, *m_Vehicle);
    const auto doorSide = CPedGeometryAnalyser::ComputeEntityHitSide(m_TargetPt, *m_Vehicle);

    CVector vehBBPlanes[4];
    float   vehBBPlanesDot[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(pedPos.z, *m_Vehicle, &vehBBPlanes, vehBBPlanesDot); // Como el original: 0x5F3660
    // Como el original: pos + n * (-((n.z * p.z + n.y * p.y) + n.x * p.x + d))
    const auto CalculatePositionOnPlane = [&](const CVector& pos, int32 side) {
        const CVector& n = vehBBPlanes[side];
        const float t = -(n.z * pos.z + n.y * pos.y + n.x * pos.x + vehBBPlanesDot[side]);
        return CVector{ t * n.x + pos.x, t * n.y + pos.y, t * n.z + pos.z };
    };

    const auto pedPosOnBBPlane  = CalculatePositionOnPlane(pedPos, pedSide);
    const auto doorPosOnBBPlane = CalculatePositionOnPlane(m_TargetPt, doorSide);

    CPointRoute routeAroundVeh;
    CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(ped, pedPosOnBBPlane, *m_Vehicle, doorPosOnBBPlane, routeAroundVeh, 0);

    m_RouteToDoor->AddUnlessFull(pedPosOnBBPlane);
    for (const auto& pt : routeAroundVeh.GetAll()) {
        m_RouteToDoor->AddUnlessFull(pt);
    }
    m_RouteToDoor->AddUnlessFull(doorPosOnBBPlane);
}
