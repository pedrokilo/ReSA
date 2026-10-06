/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCar
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Ragdoll/IKChainManager.h"

#include "TaskSimpleBikeJacked.h"
#include "TaskComplexEnterCar.h"
#include "TaskSimpleDuck.h"
#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskSimpleCarSlowDragPedOut.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarGetIn.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarCloseDoorFromInside.h"
#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskSimpleCarAlign.h"
#include "TaskSimpleCarOpenDoorFromOutside.h"
#include "TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "TaskSimplePickUpBike.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexLeaveCarAndDie.h"
#include "TaskComplexEnterBoatAsDriver.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimpleStandStill.h"

#include "EventCopCarBeingStolen.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"
#include "EventLeaderExitedCarAsDriver.h"
#include "EventLeaderEnteredCarAsDriver.h"

void CTaskComplexEnterCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCar, 0x86e6f0, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63A220);
    RH_ScopedInstall(Destructor, 0x63DFA0);
    
    RH_ScopedInstall(PreparePedForVehicleEnter, 0x63AC80);
    RH_ScopedInstall(PrepareVehicleForPedEnter, 0x63AC10);
    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x63ACC0);
    RH_ScopedInstall(GetTargetPos, 0x63A300);
    RH_ScopedInstall(GetCameraAvoidVehicle, 0x63A690);
    RH_ScopedInstall(SetVehicleFlags, 0x63AB90);
    RH_ScopedInstall(CreateSubTask, 0x63E040);

    RH_ScopedVMTInstall(MakeAbortable, 0x63A730);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63E990);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643A60);
    RH_ScopedVMTInstall(ControlSubTask, 0x63A890);
    RH_ScopedVMTInstall(CreateNextSubTask_AfterSimpleCarAlign, 0x63F970);
}

// 0x63A220
CTaskComplexEnterCar::CTaskComplexEnterCar(CVehicle* targetVehicle, bool bAsDriver, bool bQuitAfterOpeningDoor, bool bQuitAfterDraggingPedOut, bool bCarryOnAfterFallingOff) :
    m_bAsDriver{ bAsDriver },
    m_bQuitAfterOpeningDoor{ bQuitAfterOpeningDoor },
    m_bQuitAfterDraggingPedOut{ bQuitAfterDraggingPedOut },
    m_bCarryOnAfterFallingOff{ bCarryOnAfterFallingOff },
    m_Car{ targetVehicle }
{
    CEntity::SafeRegisterRef(m_Car);
}

// Como el original: helpers comunes de 0x63A890, 0x63E040, 0x63E990, 0x63F970 y 0x643A60
namespace {
// Grupo del peatón (siempre se pide), comprobación de líder y evento de grupo con el evento nuevo del pool
template<typename TEvent>
void AddLeaderGroupEvent(CPed* ped) {
    if (const auto grp = CPedGroups::GetPedsGroup(ped)) {
        if (grp->GetMembership().IsLeader(ped)) {
            grp->GetIntelligence().AddEvent(CEventGroupEvent{ ped, new TEvent{} });
        }
    }
}

// Como el original: velocidad 2D con fsqrt (no comparada al cuadrado); IsPlayer se vuelve a pedir si el primero no es jugador
bool IsCarTooFastToEnter(const CVehicle* car, CPed* ped) {
    const auto Speed = [car] {
        const auto& v = car->GetMoveSpeed();
        return std::sqrt(v.x * v.x + v.y * v.y);
    };
    return (!ped->IsPlayer() && Speed() > CCarEnterExit::ms_fMaxSpeed_CanDragPedOut)
        || (ped->IsPlayer() && Speed() > CCarEnterExit::ms_fMaxSpeed_PlayerCanDragPedOut);
}

// Como el original: solo automóviles (m_nVehicleType == 0); estado de la puerta como componente
void OpenTargetCarDoor(CVehicle* car, int32 door) {
    if (car->IsAutomobile()) {
        car->AsAutomobile()->GetDamageManager().SetDoorOpen_Component((tComponent)door);
    }
}

// Como el original: _ftol2 (truncado) y se guarda el byte bajo
void ApplyCruiseSpeed(CVehicle* car, float cruiseSpeed) {
    if (!(cruiseSpeed < 0.f)) {
        car->m_autoPilot.m_nCruiseSpeed = (uint8)(int32)cruiseSpeed;
    }
}
} // namespace

// 0x63DFA0
CTaskComplexEnterCar::~CTaskComplexEnterCar() {
    // Como el original: primero suelta la referencia del coche y después borra la utilidad de alineación
    CEntity::SafeCleanUpRef(m_Car);
    if (m_LineUpUtility) { // Como el original: test ebx,ebx antes de operator delete (0x63DFE0)
        delete m_LineUpUtility;
    }

    if (m_Car) {
        m_Car->m_nNumGettingIn -= m_NumGettingInSet;
        m_Car->ClearGettingInFlags(m_DoorFlagsSet);
    }
}

// 0x63A730
bool CTaskComplexEnterCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_Car) {
        return true;
    }
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        m_pSubTask->MakeAbortable(ped, priority, event);
        // Como el original: la tarea de quedarse quieto vive hasta el final de la rama
        CTaskSimpleStandStill standStill{ -1, false, false, 8.0f };
        standStill.ProcessPed(ped);
        if (m_TargetDoor != 0) { // TODO: Enum
            CTaskSimpleCarSetPedOut{ m_Car, (eTargetDoor)m_TargetDoor, false }.ProcessPed(ped);
        }
        if (m_TargetDoor != 0 && CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor)) {
            CTaskSimpleCarCloseDoorFromOutside{ m_Car, (uint32)m_TargetDoor, nullptr }.MakeAbortable(ped, priority, event);
        }
        return true;
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL) {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }
    return false;
}

// 0x63E990
CTask* CTaskComplexEnterCar::CreateNextSubTask(CPed* ped) {
    const auto C = [this, ped](eTaskType tt){ return CreateSubTask(tt, ped); };

    if (m_bAborting) { // 0x63E9BE
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
            return C(TASK_SIMPLE_CAR_SET_PED_OUT);
        case TASK_NONE:
        case TASK_SIMPLE_STAND_STILL:
        case TASK_COMPLEX_FALL_AND_GET_UP:
        case TASK_COMPLEX_LEAVE_CAR:
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
        case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL:
        case TASK_SIMPLE_CAR_ALIGN:
        case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_BIKE_PICK_UP:
        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE:
        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_CAR_GET_IN:
        case TASK_SIMPLE_CAR_SHUFFLE:
        case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_SIMPLE_CAR_QUICK_DRAG_PED_OUT: // Como el original: también termina
        case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT:
        case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR:
            return C(TASK_FINISHED);
        default:
            return nullptr; // Como el original: el resto de tipos no crea nada
        }
    }

    if (!m_Car || !CCarEnterExit::IsVehicleHealthy(m_Car)) { // 0x63F8D9
        return C(TASK_FINISHED);
    }

    // Como el original: CCarEnterExit::IsCarDoorReady o CarHasPartiallyOpenDoor
    const auto CanUseTargetDoor = [this]() {
        return CCarEnterExit::IsCarDoorReady(m_Car, m_TargetDoor) || CCarEnterExit::CarHasPartiallyOpenDoor(m_Car, m_TargetDoor);
    };
    const auto StartEnteringCar = [&, this]() {
        SetVehicleFlags(ped);
        PrepareVehicleForPedEnter(ped);
        PreparePedForVehicleEnter(ped);
        CreateTaskUtilityLineUpPedWithCar(ped);

        return m_Car->IsBike() || m_Car->IsSubQuad() // For bikes/quads we don't have to align, and can immidiately continue, for everything else first align, then do the rest.
            ? CreateNextSubTask_AfterSimpleCarAlign(ped)
            : C(TASK_SIMPLE_CAR_ALIGN);
    };

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL: {
        const auto tGoToDoor = static_cast<CTaskComplexGoToCarDoorAndStandStill*>(m_pSubTask);

        if (!tGoToDoor->HasAchievedDoor()) { // 0x63EBEA
            return C(TASK_FINISHED);
        }

        // Como el original: copia la puerta y el punto objetivo (no el asiento)
        m_TargetDoor               = tGoToDoor->GetTargetDoor();
        m_TargetDoorPos            = tGoToDoor->GetTargetPt();
        m_TargetDoorOppositeToFlag = ComputeTargetDoorOppositeToFlag();

        if (CCarEnterExit::IsCarDoorInUse(m_Car, m_TargetDoor, m_TargetDoorOppositeToFlag)) { // 0x63EC24
            if (ped->IsPlayer()) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 0.f;
                ped->SetMoveState(PEDMOVE_STILL);
                ped->AsPlayer()->SetRealMoveAnim();
            } else {
                ped->SetMoveState(PEDMOVE_STILL);
                ped->SetMoveAnim();
            }
            return C(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE);
        }

        if (!CanUseTargetDoor()) { // 0x63ECBC
            return C(TASK_FINISHED);
        }
        return StartEnteringCar();
    }
    case TASK_SIMPLE_CAR_ALIGN: // 0x63ED39
        return CreateNextSubTask_AfterSimpleCarAlign(ped);
    case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE: { // 0x63ED58
        if (m_Car) {
            ApplyCruiseSpeed(m_Car, m_CruiseSpeed);
        }
        return C(TASK_SIMPLE_STAND_STILL);
    }
    case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE:
    case TASK_SIMPLE_BIKE_PICK_UP: { // 0x63EDA5
        if (IsCarTooFastToEnter(m_Car, ped)) {
            ped->SetUsesCollision(true);
            return C(ped->IsPlayer() ? TASK_FINISHED : TASK_COMPLEX_FALL_AND_GET_UP);
        }

        if (m_bQuitAfterOpeningDoor) { // 0x63EE4E
            m_bQuitAfterOpeningDoor = false;
            return C(TASK_FINISHED);
        }

        if (!m_bQuitAfterDraggingPedOut && !CCarEnterExit::IsCarSlowJackRequired(m_Car, m_TargetDoor)) { // 0x63EE6A
            return C(TASK_SIMPLE_CAR_GET_IN);
        }

        // Como el original: guarda el peatón y registra la referencia sin soltar la anterior
        m_DraggedPed = CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor);
        CEntity::SafeRegisterRef(m_DraggedPed);

        if (!m_bQuitAfterDraggingPedOut && !(m_DraggedPed && !CPedGroups::AreInSameGroup(m_DraggedPed, ped) && !m_DraggedPed->bDontDragMeOutCar)) { // 0x63EECE
            // Como el original: el peatón (amigo) ya está en el asiento objetivo -> cerrar la puerta o terminar
            if (   (m_DraggedPed && m_bAsDriver && m_Car->IsDriver(m_DraggedPed))
                || (m_DraggedPed && !m_bAsDriver && m_DraggedPed == m_Car->m_apPassengers.data()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Car, m_TargetDoor)])
            ) {
                return C(CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor) ? TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE : TASK_FINISHED);
            }
            return C(TASK_SIMPLE_CAR_GET_IN);
        }

        if (m_DraggedPed && m_DraggedPed->IsPlayer() && CGameLogic::SkipState == SKIP_IN_PROGRESS) { // 0x63EF63
            CTask* const task = C(TASK_FINISHED); // Como el original: antes del evento de grupo
            AddLeaderGroupEvent<CEventLeaderQuitEnteringCarAsDriver>(m_DraggedPed);
            return task;
        }

        if (m_DraggedPed) { // 0x63F034
            return CreateDragPedOutSubTask(ped);
        }

        if (m_bQuitAfterDraggingPedOut) { // 0x63F17A
            OpenTargetCarDoor(m_Car, m_TargetDoor);
            return C(TASK_FINISHED);
        }
        return C(TASK_SIMPLE_CAR_GET_IN);
    }
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE: { // 0x63F68A
        if (!m_Car || !CCarEnterExit::IsVehicleHealthy(m_Car) || !CCarEnterExit::IsPedHealthy(ped)) {
            return C(TASK_FINISHED);
        }
        if (!m_bAsDriver || m_TargetDoor != 8) {
            return C(m_bAsDriver ? TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER : TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER);
        }
        if (m_Car->m_pDriver) {
            CEventDraggedOutCar draggedOut{ m_Car, ped, true };
            m_Car->m_pDriver->GetIntelligence()->GetEventGroup().Add(&draggedOut, true); // Como el original: válido
            // Como el original: vivo y con salud > 0 (o NaN) sale del coche; si no, LeaveCarAndDie (0x62F1A0)
            CTask* task;
            if (m_Car->m_pDriver->IsAlive() && !(m_Car->m_pDriver->m_fHealth <= 0.f)) {
                task = new CTaskComplexLeaveCar{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0, false, true };
            } else {
                task = new CTaskComplexLeaveCarAndDie{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0 };
            }
            m_Car->m_pDriver->GetIntelligence()->AddTaskPrimaryMaybeInGroup(task, true);
        }
        return C(m_Car->m_pHandlingData->m_bTandemSeats ? TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER : TASK_SIMPLE_CAR_SHUFFLE);
    }
    case TASK_SIMPLE_CAR_GET_IN: { // 0x63F3A2
        CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar(m_Car, m_DraggedPed, ped);
        bool bDontCloseDoors{};
        if (m_bAsDriver && ped->IsPlayer() && m_Car->IsAutomobile() && !m_Car->IsSubPlane() && !m_Car->IsSubHeli()) {
            const auto pad = CPad::GetPad(0);
            // Como el original: !(acelerar < 255) o !(frenar < 255)
            if (!((float)pad->GetAccelerate() < 255.f) || !((float)pad->GetBrake() < 255.f)) {
                // Como el original: estado de la puerta (componente) antes de CarHasDoorToClose
                auto& dm = m_Car->AsAutomobile()->GetDamageManager();
                const auto comp = (tComponent)m_TargetDoor;
                if ((dm.GetDoorStatus_Component(comp) == DAMSTATE_OK || dm.GetDoorStatus_Component(comp) == DAMSTATE_DAMAGED)
                    && CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor)) {
                    dm.SetDoorStatus_Component(comp, (eDoorStatus)(dm.GetDoorStatus_Component(comp) + 1));
                }
                bDontCloseDoors = true;
            }
        }

        if (CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor) && !bDontCloseDoors) {
            return C(TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE);
        }

        if (!m_bAsDriver) {
            return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER);
        }

        if (m_TargetDoor != 8 || !m_Car->IsAutomobile() || m_Car->m_pHandlingData->m_bTandemSeats) {
            return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER);
        }

        if (m_Car->m_pDriver) {
            CEventDraggedOutCar draggedOut{ m_Car, ped, true };
            m_Car->m_pDriver->GetIntelligence()->GetEventGroup().Add(&draggedOut, true); // Como el original: válido
            // Como el original: con el conductor vivo crea LeaveCarAndDie (0x62F1A0); si no, LeaveCar
            CTask* task;
            if (m_Car->m_pDriver->IsAlive()) {
                task = new CTaskComplexLeaveCarAndDie{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0 };
            } else {
                task = new CTaskComplexLeaveCar{ m_Car, CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Car->m_pDriver), 0, false, true };
            }
            m_Car->m_pDriver->GetIntelligence()->AddTaskPrimaryMaybeInGroup(task, true);
        }
        return C(TASK_SIMPLE_CAR_SHUFFLE);
    }
    case TASK_SIMPLE_CAR_SHUFFLE: // 0x63F874
        return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER);
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: { // 0x63F223
        if (!CanUseTargetDoor()) {
            return nullptr;
        }
        return StartEnteringCar();
    }
    case TASK_SIMPLE_CAR_QUICK_DRAG_PED_OUT: { // 0x63F2E0
        CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar(m_Car, m_DraggedPed, ped);
        return C(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER);
    }
    case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT: { // 0x63F319
        if (!m_bQuitAfterDraggingPedOut) {
            return C(TASK_SIMPLE_CAR_GET_IN);
        }
        m_bQuitAfterDraggingPedOut = false;
        return C(m_DraggedPed ? TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR : TASK_FINISHED);
    }
    case TASK_COMPLEX_FALL_AND_GET_UP: { // 0x63EAE9
        if (FindPlayerPed() == ped) {
            FindPlayerInfo().SetLastTargetVehicle(nullptr);
        }

        if (m_bCarryOnAfterFallingOff) {
            C(TASK_FINISHED); // Como el original: se llama (efectos secundarios) y se descarta el resultado
            return CreateFirstSubTask(ped);
        }
        return C(TASK_FINISHED);
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x63EB65
        if (ped->bInVehicle) {
            return C(TASK_FINISHED);
        }
        return C(m_bAsDriver && m_Car->IsBoat() ? TASK_COMPLEX_ENTER_BOAT_AS_DRIVER : TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL);
    }
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR:
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
    case TASK_SIMPLE_STAND_STILL:
    case TASK_NONE:
        return C(TASK_FINISHED);

    default:
        return nullptr; // Como el original: el resto de tipos (SET_PED_OUT, PAUSE, ...) no crea nada
    }
}

// 0x63F970
CTask* CTaskComplexEnterCar::CreateNextSubTask_AfterSimpleCarAlign(CPed* ped) {
    const auto C = [this, ped](eTaskType tt) { return CreateSubTask(tt, ped); };

    // Como el original: se decide la subtarea y después se mira a los ocupantes (salvo en autobuses)
    CTask* task{};
    if (IsCarTooFastToEnter(m_Car, ped)) {
        ped->SetUsesCollision(true);
        task = C(ped->IsPlayer() ? TASK_FINISHED : TASK_COMPLEX_FALL_AND_GET_UP);
    } else if (m_bQuitAfterOpeningDoor) { // 0x63FA2E
        if (CCarEnterExit::CarHasDoorToOpen(m_Car, m_TargetDoor)) {
            task = C(CCarEnterExit::CarHasOpenableDoor(m_Car, m_TargetDoor, ped) ? TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE : TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE);
        } else {
            m_bQuitAfterOpeningDoor = false;
            task = C(TASK_FINISHED);
        }
    } else if (!m_bQuitAfterDraggingPedOut && CCarEnterExit::IsCarQuickJackPossible(m_Car, m_TargetDoor, ped)) { // 0x63FA8D
        task = nullptr;
    } else if (!m_Car->IsBike() && !m_Car->IsSubQuad()) { // 0x63FAA6
        if (CCarEnterExit::CarHasDoorToOpen(m_Car, m_TargetDoor)) {
            if (!CCarEnterExit::CarHasOpenableDoor(m_Car, m_TargetDoor, ped)) { // 0x63FADA
                task = C(TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE);
            } else if (m_bQuitAfterDraggingPedOut || m_bQuitAfterOpeningDoor || m_Car->GetAnimGroupId() != ANIM_GROUP_CONVCARANIMS || m_TargetDoor != 10 || m_Car->m_pDriver) { // 0x63FAE6
                task = C(TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE);
            } else {
                task = C(TASK_SIMPLE_CAR_GET_IN);
            }
        } else if (!CCarEnterExit::IsCarSlowJackRequired(m_Car, m_TargetDoor)) {
            task = C(m_bQuitAfterDraggingPedOut ? TASK_FINISHED : TASK_SIMPLE_CAR_GET_IN);
        } else {
            // Como el original: guarda el peatón y registra la referencia sin soltar la anterior
            m_DraggedPed = CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor);
            CEntity::SafeRegisterRef(m_DraggedPed);

            if (m_DraggedPed && m_DraggedPed->IsPlayer() && CGameLogic::SkipState == SKIP_IN_PROGRESS) { // 0x63FB68
                task = C(TASK_FINISHED); // Como el original: antes del evento de grupo
                AddLeaderGroupEvent<CEventLeaderQuitEnteringCarAsDriver>(m_DraggedPed);
            } else if (!m_bQuitAfterDraggingPedOut && !(m_DraggedPed && !CPedGroups::AreInSameGroup(m_DraggedPed, ped) && !m_DraggedPed->bDontDragMeOutCar)) { // 0x63FC2E
                if (   (m_DraggedPed && m_bAsDriver && m_Car->IsDriver(m_DraggedPed))
                    || (m_DraggedPed && !m_bAsDriver && m_DraggedPed == m_Car->m_apPassengers.data()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Car, m_TargetDoor)])
                ) {
                    task = C(CCarEnterExit::CarHasDoorToClose(m_Car, m_TargetDoor) ? TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE : TASK_FINISHED);
                } else {
                    task = C(TASK_SIMPLE_CAR_GET_IN);
                }
            } else if (m_bQuitAfterDraggingPedOut && !m_DraggedPed) { // 0x63FCE0
                task = C(TASK_FINISHED);
            } else {
                task = CreateDragPedOutSubTask(ped);
            }
        }
    } else if (CCarEnterExit::IsCarSlowJackRequired(m_Car, m_TargetDoor)) { // 0x63FE36 - bicis y quads
        // Como el original: guarda el peatón y registra la referencia sin soltar la anterior
        m_DraggedPed = CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor);
        CEntity::SafeRegisterRef(m_DraggedPed);

        if (m_DraggedPed && m_DraggedPed->IsPlayer() && CGameLogic::SkipState == SKIP_IN_PROGRESS) { // 0x63FE93
            CTask* const finished = C(TASK_FINISHED);
            AddLeaderGroupEvent<CEventLeaderQuitEnteringCarAsDriver>(m_DraggedPed);
            return finished; // Como el original: sin mirar a los ocupantes
        }

        // Como el original: la subtarea se crea antes que las tareas de los peatones derribados
        task = C(!m_bQuitAfterDraggingPedOut && m_TargetDoor == 18 ? TASK_SIMPLE_CAR_GET_IN : TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT);

        m_DraggedPed->GetIntelligence()->AddTaskPrimaryMaybeInGroup(
            new CTaskSimpleBikeJacked{ m_Car, m_TargetDoor != 18 || m_Car->m_apPassengers[0] ? 10u : 11u, (uint32)m_DraggedPedDownTime, ped, true },
            true
        );
        if (m_DraggedPed == m_Car->m_pDriver && m_Car->m_apPassengers[0]) { // 0x63FFB6
            // Como el original: el pasajero cae 200 ms después y sin el último argumento
            m_Car->m_apPassengers[0]->GetIntelligence()->AddTaskPrimaryMaybeInGroup(
                new CTaskSimpleBikeJacked{ m_Car, 11u, (uint32)(m_DraggedPedDownTime + 200), ped, false },
                true
            );
            if (m_DraggedPed == m_Car->m_pDriver) {
                AddLeaderGroupEvent<CEventLeaderExitedCarAsDriver>(m_DraggedPed);
            }
        }
    } else if (m_bQuitAfterDraggingPedOut) { // 0x6400AE
        task = C(TASK_FINISHED);
    } else if (m_Car->GetStatus() == STATUS_ABANDONED && m_Car->IsBike()) { // 0x6400BC
        // Como el original: comparaciones con NaN -> recoger la bici
        const auto& mat = *m_Car->m_matrix;
        if (!(mat.GetRight().z < 0.5f) || !(mat.GetRight().z > -0.5f) || !(mat.GetUp().z > 0.f)) { // 0x6400F3 - Check if the bike is on it's side
            task = C(TASK_SIMPLE_BIKE_PICK_UP);
        } else {
            task = C(TASK_SIMPLE_CAR_GET_IN);
            m_Car->AsBike()->bikeFlags.bGettingPickedUp = true; // Como el original: después de crear la subtarea
        }
    } else {
        task = C(TASK_SIMPLE_CAR_GET_IN);
    }

    // 0x64013B - Como el original: los 8 asientos (no solo m_nMaxPassengers) y después el conductor
    if (!m_Car->vehicleFlags.bIsBus) {
        const auto LookAtOccupant = [ped](CPed* occupant) {
            // Como el original: GetRandomIntInRange del EXE (2000 - ftol(r * -1000 / 32768), ...)
            const int32 time = ped->GetIntelligence()->IsFriendlyWith(*occupant) || CPedGroups::AreInSameGroup(ped, occupant)
                ? CGeneral::GetRandomIntInRange(2'000, 3'000)
                : CGeneral::GetRandomIntInRange(6'000, 8'000);
            g_ikChainMan.LookAt("TaskEnterCar", occupant, ped, time, BONE_HEAD, nullptr, false, 0.5f, 350, 3, false);
        };
        for (size_t i = 0; i < m_Car->m_apPassengers.size(); i++) {
            if (m_Car->m_apPassengers[i]) {
                LookAtOccupant(m_Car->m_apPassengers[i]);
            }
        }
        if (m_Car->m_pDriver) {
            LookAtOccupant(m_Car->m_pDriver);
        }
    }
    return task;
}

// 0x643A60
CTask* CTaskComplexEnterCar::CreateFirstSubTask(CPed* ped) {
    const auto C = [this, ped](eTaskType tt){ return CreateSubTask(tt, ped); };

    m_EnterCarStartTime = CTimer::GetTimeInMS();

    if (   !m_Car || m_Car->m_pFire
        || !CCarEnterExit::IsVehicleHealthy(m_Car) || !CCarEnterExit::IsPedHealthy(ped)
        || !m_bAsDriver && !m_Car->m_nMaxPassengers                                         // Wants to enter as passenger, but there are no passenger seats
        || m_Car->IsTrain() && m_Car->AsTrain()->trainFlags.bNotOnARailRoad
    ) {
        return C(TASK_FINISHED);
    }

    // Como el original: el grupo se pide siempre, antes de mirar las banderas
    const auto pedGrp = CPedGroups::GetPedsGroup(ped);
    if (m_bAsDriver && !m_bQuitAfterOpeningDoor && !m_bQuitAfterDraggingPedOut && pedGrp && pedGrp->GetMembership().IsLeader(ped)) { // 0x643B15
        pedGrp->GetIntelligence().AddEvent(CEventGroupEvent{ ped, new CEventLeaderEnteredCarAsDriver{ m_Car } });
    }

    if (ped->bInVehicle) {
        return C(ped->m_pVehicle == m_Car
            ? TASK_SIMPLE_CAR_DRIVE_TIMED
            : TASK_COMPLEX_LEAVE_CAR
        );
    }

    if (m_Car->IsBoat()) {
        return C(TASK_COMPLEX_ENTER_BOAT_AS_DRIVER);
    }

    if (const auto tGoToCarDoor = static_cast<CTaskComplexGoToCarDoorAndStandStill*>(C(TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL))) {
        if (ped->physicalFlags.bSubmergedInWater && !ped->bIsStanding) {
            if (notsa::contains({ MODEL_SKIMMER, MODEL_VORTEX, MODEL_SEASPAR, MODEL_LEVIATHN }, m_Car->GetModelId())) {
                if (CCarEnterExit::GetNearestCarDoor(ped, m_Car, m_TargetDoorPos, m_TargetDoor)) {
                    tGoToCarDoor->SetTargetDoor(m_TargetDoor);
                    tGoToCarDoor->SetTargetPt(m_TargetDoorPos);
                    tGoToCarDoor->SetTryingToEnterInWater(true);
                }
            }
        }
        return tGoToCarDoor;
    }

    return nullptr;
}

// 0x63A890
CTask* CTaskComplexEnterCar::ControlSubTask(CPed* ped) {
    // Como el original: devuelve la subtarea leída al entrar (o nullptr si el jugador abandona)
    CTask* ret = m_pSubTask;

    // Como el original: IsGroupLeader antes de mirar la subtarea; GetPedsGroup + IsLeader después
    const auto AddQuitEventIfLeader = [&, this](bool bCheckSubTask) {
        if (m_bAsDriver && !m_bQuitAfterOpeningDoor && !m_bQuitAfterDraggingPedOut && CPedGroups::IsGroupLeader(ped)) {
            if (!bCheckSubTask || m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_GET_IN) {
                AddLeaderGroupEvent<CEventLeaderQuitEnteringCarAsDriver>(ped);
            }
        }
    };

    if (ped->IsPlayer() && !m_bQuitAfterDraggingPedOut && m_Car) { // 0x63A8BA
        if (CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Car, m_EnterCarStartTime, m_pSubTask)) {
            AddQuitEventIfLeader(true);
            if (CPed* jacked; m_TargetDoor > 0 && (jacked = CCarEnterExit::ComputeSlowJackedPed(m_Car, m_TargetDoor)) && jacked->bDontDragMeOutCar) {
                m_bQuitAfterOpeningDoor = true;
            } else {
                m_bQuitAfterDraggingPedOut = true;
            }
        }
    }

    if (!m_Car || !CCarEnterExit::IsVehicleHealthy(m_Car) || !CCarEnterExit::IsPedHealthy(ped)) { // 0x63A9D6
        m_bAborting = true;
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    } else if (m_bAsDriver || ped->IsPlayer()) { // 0x63AA03
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL:
        case TASK_NONE:
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
            if (ped->IsPlayer() && CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Car, m_EnterCarStartTime, m_pSubTask)) {
                AddQuitEventIfLeader(false);
                ret = nullptr;
            }
            break;
        }
    }

    // 0x63AB28 - Como el original: al final, con la subtarea actual
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL) {
        const auto tGoToCarDoor = static_cast<CTaskComplexGoToCarDoorAndStandStill*>(m_pSubTask);
        if (tGoToCarDoor->GetTargetDoor() != 0) {
            m_TargetDoorPos = tGoToCarDoor->GetTargetPt();
        }
    }

    return ret;
}

// 0x63E040
CTask* CTaskComplexEnterCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT: { // 0x63E8C7
        if (ped && ped->IsPlayer() && !ped->AsPlayer()->bHasDisplayedPlayerQuitEnterCarHelpText) {
            ped->AsPlayer()->bHasDisplayedPlayerQuitEnterCarHelpText = true;

            // To stop Carl from entering a car once he has opened the door or dragged out a
            // driver or passenger, use the movement controls or press ~k~~PED_FIREWEAPON~.
            CHud::SetHelpMessage(TheText.Get("JCK_HLP"));
        }
        return new CTaskSimpleCarSlowDragPedOut{ m_Car, (eTargetDoor)m_TargetDoor, m_LineUpUtility, m_bQuitAfterDraggingPedOut };
    }
    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR: // 0x63E846
        return new CTaskSimpleWaitUntilPedIsOutCar{ m_DraggedPed, IsTargetDoorOnTheLeft() ? -m_Car->GetRight() : m_Car->GetRight() };
    case TASK_FINISHED: { // 0x63E66A
        if (!ped) {
            return nullptr;
        }

        if (!ped->bInVehicle && !ped->GetUsesCollision()) { // 0x63E67A
            ped->SetUsesCollision(true);
            if (m_Car && m_TargetDoor != 0) { // TODO: Enum
                CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(ped, m_Car, m_TargetDoor);
            }
        }

        // Como el original: jugador, coche de policía, banderas y puertas marcadas, en ese orden
        if (ped->IsPlayer() && m_Car && m_Car->IsLawEnforcementVehicle() && m_bAsDriver && !m_bQuitAfterOpeningDoor && !m_bQuitAfterDraggingPedOut && m_DoorFlagsSet) { // 0x63E6A8
            GetEventGlobalGroup()->Add(CEventCopCarBeingStolen{ ped, m_Car });
        }

        if (!ped->bInVehicle && m_bAsDriver && !m_bQuitAfterOpeningDoor && !m_bQuitAfterDraggingPedOut) { // 0x63E70E
            AddLeaderGroupEvent<CEventLeaderQuitEnteringCarAsDriver>(ped);
        }

        // 0x63E7A2 - Como el original: el jugador fuera del coche, con conductor y sin "salir tras abrir la puerta";
        // cualquier otro peatón dentro del coche
        if (ped->IsPlayer() && !ped->bInVehicle && m_Car && m_Car->m_pDriver && !m_bQuitAfterOpeningDoor) {
            ApplyCruiseSpeed(m_Car, m_CruiseSpeed);
        } else if (!ped->IsPlayer() && ped->bInVehicle && m_Car) {
            ApplyCruiseSpeed(m_Car, m_CruiseSpeed);
        }

        return nullptr;
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT: // 0x63E617
        return new CTaskSimpleCarSetPedOut{ m_Car, (eTargetDoor)m_TargetDoor, true };
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE: // 0x63E5E1
        return new CTaskSimpleCarCloseDoorFromOutside{ m_Car, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_GET_IN: // 0x63E50D
        return new CTaskSimpleCarGetIn{ m_Car, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_SHUFFLE: // 0x63E543
        return new CTaskSimpleCarShuffle{ m_Car, m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE: // 0x63E4D7
        return new CTaskSimpleCarWaitForDoorNotToBeInUse{m_Car, (uint32)m_TargetDoor, (uint32)m_TargetDoorOppositeToFlag };
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER: // 0x63E5AB
        return new CTaskSimpleCarSetPedInAsPassenger{ m_Car, (eTargetDoor)m_TargetDoor, false, m_LineUpUtility };
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER: // 0x63E579
        return new CTaskSimpleCarSetPedInAsDriver{ m_Car, m_LineUpUtility };
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE: // 0x63E47B
        return new CTaskSimpleCarCloseDoorFromInside{ m_Car, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL: // 0x63E333
        return new CTaskComplexGoToCarDoorAndStandStill{ m_Car, m_MoveState, m_bAsDriver, m_TargetSeat, 0.5f, 4.f, 50.f, 30'000 };
    case TASK_SIMPLE_CAR_ALIGN: // 0x63E391
        return new CTaskSimpleCarAlign{ m_Car, m_TargetDoorPos, (uint32)m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE: // 0x63E3CB
        return new CTaskSimpleCarOpenDoorFromOutside{ m_Car, m_TargetDoor, m_LineUpUtility, m_bQuitAfterOpeningDoor };
    case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE: // 0x63E40F
        return new CTaskSimpleCarOpenLockedDoorFromOutside{ m_Car, m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_BIKE_PICK_UP: // 0x63E445
        return new CTaskSimplePickUpBike{ m_Car, m_TargetDoor, m_LineUpUtility };
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x63E2F2
        return new CTaskSimpleCarDriveTimed{ m_Car };
    case TASK_COMPLEX_LEAVE_CAR: // 0x63E2B8
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false }; // Como el original: el vehículo del peatón (no m_Car)
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER: // 0x63E28A
        return new CTaskComplexEnterBoatAsDriver{ m_Car };
    case TASK_COMPLEX_FALL_AND_GET_UP: { // 0x63E102
        // Como el original: la tarea se reserva antes de todo lo demás
        auto* const task = new CTaskComplexFallAndGetUp{ IsTargetDoorOnTheLeft() ? ANIM_ID_KO_SPIN_L : ANIM_ID_KO_SPIN_R, ANIM_GROUP_DEFAULT, 2000 };
        if (FindPlayerPed() == ped) {
            FindPlayerInfo().SetLastTargetVehicle(m_Car);
        }
        if (ped && !ped->bInVehicle) {
            ped->SetUsesCollision(true);
        }
        if (m_Car) {
            CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(ped, m_Car, m_TargetDoor);
        }
        if (m_Car) {
            // Como el original: sin assert sobre m_nNumGettingIn
            m_Car->m_nNumGettingIn -= m_NumGettingInSet;
            m_NumGettingInSet       = 0;

            m_Car->ClearGettingInFlags(m_DoorFlagsSet);
            m_DoorFlagsSet = 0;

            // Como el original: IsDoorMissingU32 (virtual) antes de mirar el tipo; puerta como componente
            if (!m_Car->IsDoorMissingU32(m_TargetDoor) && m_Car->IsAutomobile()) {
                m_Car->AsAutomobile()->GetDamageManager().SetDoorOpen_Component((tComponent)m_TargetDoor);
            }
        }
        return task;
    }
    case TASK_NONE:
        return new CTaskSimpleNone{}; // 0x63E0D8 - NOTE/TODO: Weird
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{}; // 0x63E0A6
    default:
        return nullptr; // Como el original: tipos desconocidos devuelven nullptr
    }
}

// 0x63AB90
void CTaskComplexEnterCar::SetVehicleFlags(CPed* ped) {
    m_DoorFlagsSet = CCarEnterExit::ComputeDoorFlag(m_Car, m_TargetDoor, true);
    m_Car->SetGettingInFlags(m_DoorFlagsSet);

    m_NumGettingInSet = true;
    m_Car->m_nNumGettingIn++;

    if (m_TargetDoorOppositeToFlag) {
        const auto oppositeDoorFlag = CCarEnterExit::ComputeDoorFlag(m_Car, m_TargetDoorOppositeToFlag, true);
        if (oppositeDoorFlag != m_DoorFlagsSet) {
            m_Car->SetGettingInFlags(oppositeDoorFlag);
            m_DoorFlagsSet |= oppositeDoorFlag;
        }
    }
}

// 0x63AB70
int32 CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag() const {
    return m_bAsDriver && m_TargetDoor == 8 ? 10 : 0; // I guess `8` is the driver side door?
}

// 0x63AC80
void CTaskComplexEnterCar::PreparePedForVehicleEnter(CPed* ped) {
    ped->SetUsesCollision(false);
    // Como el original: GetTaskDuck se pide dos veces
    if (ped->GetIntelligence()->GetTaskDuck(true)) {
        ped->GetIntelligence()->GetTaskDuck(true)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
    }
}

// 0x63AC10
void CTaskComplexEnterCar::PrepareVehicleForPedEnter(CPed* ped) {
    if (const auto carCruiseSpeed = m_Car->m_autoPilot.m_nCruiseSpeed) {
        m_CruiseSpeed = (float)carCruiseSpeed;
    }
    if (!ped->IsPlayer() || !CCarEnterExit::CarHasDoorToOpen(m_Car, (eDoors)m_TargetDoor) || CCarEnterExit::CarHasOpenableDoor(m_Car, m_TargetDoor, ped)) {
        m_Car->m_autoPilot.SetCruiseSpeed(0);
    }
}

// 0x63ACC0
void CTaskComplexEnterCar::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    // Como el original: el desplazamiento se calcula antes de reservar la utilidad
    const CVector offset = CCarEnterExit::GetPositionToOpenCarDoor(m_Car, m_TargetDoor) - ped->GetPosition();
    m_LineUpUtility = new CTaskUtilityLineUpPedWithCar{ offset, 600, 0, m_TargetDoor }; // Como el original: la anterior se pierde
}

// NOTSA - Copy paste code elimination (0x63F034 / 0x63FCF1)
CTask* CTaskComplexEnterCar::CreateDragPedOutSubTask(CPed* ped) {
    // Como el original: la subtarea se crea primero; el evento de arrastrado vive hasta el final
    CTask* const task = CreateSubTask(TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT, ped);
    m_DraggedPed->GetIntelligence()->AddTaskPrimaryMaybeInGroup(new CTaskComplexCarSlowBeDraggedOutAndStandUp{ m_Car, m_TargetDoor }, true);
    CEventDraggedOutCar draggedOut{ m_Car, ped, m_Car->m_pDriver == m_DraggedPed };
    m_DraggedPed->GetIntelligence()->GetEventGroup().Add(&draggedOut, true); // Como el original: válido
    if (m_Car->m_pDriver == m_DraggedPed) {
        AddLeaderGroupEvent<CEventLeaderExitedCarAsDriver>(m_DraggedPed);
    }
    return task;
}

// 0x63A300
CVector CTaskComplexEnterCar::GetTargetPos() const {
    if (m_TargetDoor != 0) { // TODO: Enum
        return m_TargetDoorPos;
    }
    if (const auto tGoTo = notsa::dyn_cast_if_present<CTaskComplexGoToCarDoorAndStandStill>(m_pSubTask)) {
        return tGoTo->GetTargetPt();
    }
    return {};
}

// 0x63A690
CVehicle* CTaskComplexEnterCar::GetCameraAvoidVehicle() {
    if (m_Car && m_pSubTask) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_LEAVE_CAR:
        case TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE:
        case TASK_SIMPLE_BIKE_PICK_UP:
        case TASK_SIMPLE_STAND_STILL:
        case TASK_SIMPLE_CAR_QUICK_DRAG_PED_OUT:
        case TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT:
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        case TASK_SIMPLE_CAR_DRIVE_TIMED:
        case TASK_SIMPLE_CAR_GET_IN:
        case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE:
        case TASK_SIMPLE_CAR_SHUFFLE:
        case TASK_SIMPLE_CAR_ALIGN:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
            return m_Car;
        }
    }
    return nullptr;
}
