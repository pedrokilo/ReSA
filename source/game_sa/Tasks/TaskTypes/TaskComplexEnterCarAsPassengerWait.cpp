/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCarAsPassengerWait
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexEnterCarAsPassengerWait.h"
#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "CarEnterExit.h"
#include "Models/ModelInfo.h"

void CTaskComplexEnterCarAsPassengerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsPassengerWait, 0x86E778, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B320);
    RH_ScopedInstall(Destructor, 0x63B3C0);

    RH_ScopedInstall(CreateSubTask, 0x6408D0);
    RH_ScopedVMTInstall(Clone, 0x63D850);
    RH_ScopedVMTInstall(GetTaskType, 0x63B3B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x643E10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643F00);
    RH_ScopedVMTInstall(ControlSubTask, 0x640730);
}

// 0x63B320
CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(CVehicle* target, CPed* waitFor, bool forceFrontSeat, eMoveState ms) :
    m_Car{target},
    m_WaitForPed{waitFor},
    m_bForceFrontSeat{forceFrontSeat},
    m_MoveState{ms}
{
    CEntity::SafeRegisterRef(m_Car);
    CEntity::SafeRegisterRef(m_WaitForPed);
}
    
// NOTSA (for 0x63D850)
CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(const CTaskComplexEnterCarAsPassengerWait& o) :
    CTaskComplexEnterCarAsPassengerWait{
        o.m_Car,
        o.m_WaitForPed,
        o.m_bForceFrontSeat,
        o.m_MoveState
    }
{
}

// 0x63B3C0
CTaskComplexEnterCarAsPassengerWait::~CTaskComplexEnterCarAsPassengerWait() {
    CEntity::SafeCleanUpRef(m_Car);
    CEntity::SafeCleanUpRef(m_WaitForPed);
}

// 0x6408D0
CTask* CTaskComplexEnterCarAsPassengerWait::CreateSubTask(int32 taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return new CTaskSimpleCarDriveTimed(m_Car, 0);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED: {
        CTaskComplexEnterCarAsPassengerTimed* task;
        if (m_bForceFrontSeat) {
            const auto door = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(m_Car, 0);
            task = new CTaskComplexEnterCarAsPassengerTimed(m_Car, door, static_cast<uint32>(-1), true);
        } else {
            task = new CTaskComplexEnterCarAsPassengerTimed(m_Car, 0, static_cast<uint32>(-1), true);
        }
        // Como el original: se copia el estado de movimiento (offset 0x1C de la tarea nueva, privado en esa clase).
        *reinterpret_cast<eMoveState*>(reinterpret_cast<uint8*>(task) + 0x1C) = m_MoveState;
        return task;
    }
    case TASK_COMPLEX_SEQUENCE: {
        const auto sequence = new CTaskComplexSequence;
        sequence->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_Car));
        sequence->AddTask(new CTaskSimpleStandStill(1000, false, false, 8.0f));
        return sequence;
    }
    case TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR:
        return new CTaskSimpleWaitUntilPedIsInCar(m_WaitForPed);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        CVector point;
        if (CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Car, point)) {
            return new CTaskComplexGoToPointAndStandStill(m_MoveState, point, 1.5f, 2.0f, false, false);
        }
        // Como el original: sin punto sobre la superficie se va al centro del vehículo con el radio de su esfera + 1.5.
        const float radius = CModelInfo::ms_modelInfoPtrs[(int16)m_Car->m_nModelIndex]->GetColModel()->GetBoundRadius() + 1.5f;
        return new CTaskComplexGoToPointAndStandStill(m_MoveState, m_Car->GetPosition(), radius, 2.0f, false, false);
    }
    default: // incluido TASK_FINISHED
        return nullptr;
    }
}

// 0x643E10
CTask* CTaskComplexEnterCarAsPassengerWait::CreateNextSubTask(CPed* ped) {
    if (!m_Car) {
        return CreateSubTask(TASK_FINISHED, ped);
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED:
        if (ped->bInVehicle) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        m_EnterCarFails++;
        if ((int32)m_EnterCarFails > 15) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (!m_Car) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        return CreateSubTask(TASK_COMPLEX_SEQUENCE, ped);
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_SEQUENCE:
    case TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR:
        if (m_Car) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED, ped);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        if (m_WaitForPed) {
            return CreateSubTask(TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR, ped);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    default:
        return nullptr;
    }
}

// 0x643F00
CTask* CTaskComplexEnterCarAsPassengerWait::CreateFirstSubTask(CPed* ped) {
    if (ped->bInVehicle && ped->m_pVehicle && ped->m_pVehicle == m_Car && m_Car->IsPassenger(ped)) {
        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
    }
    if (!m_WaitForPed) {
        if (m_Car) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED, ped);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    if (m_Car) {
        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x640730
CTask* CTaskComplexEnterCarAsPassengerWait::ControlSubTask(CPed* ped) {
    if (!m_bPlayedSample) {
        if (m_WaitForPed && m_WaitForPed->IsPlayer()) {
            // Como el original: el jugador espera y el peatón ya está subiendo al coche
            if (ped->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_CAR_GET_IN)) {
                m_bPlayedSample = true;
                ped->Say(static_cast<eGlobalSpeechContext>(0x23), 0, 1.0f, false, false, false);
            }
        } else if (!m_bPlayedSample && !m_WaitForPed) {
            // Como el original: sin peatón de espera, si el líder del grupo del peatón es el jugador
            if (const auto grp = CPedGroups::GetPedsGroup(ped)) {
                if (const auto leader = grp->GetMembership().GetLeader()) {
                    if (leader->IsPlayer()) {
                        if (ped->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_CAR_GET_IN)) {
                            m_bPlayedSample = true;
                            ped->Say(static_cast<eGlobalSpeechContext>(0x28), 0, 1.0f, false, false, false);
                        }
                    }
                }
            }
        }
    }

    if (m_Car) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
            // Como el original: si no hay punto sobre la superficie, `closest` queda sin escribir (basura de pila).
            CVector closest;
            const bool found = CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Car, closest);
            const auto goTo = static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
            const float dx = closest.x - goTo->m_vecTargetPoint.x;
            const float dy = closest.y - goTo->m_vecTargetPoint.y;
            const float dz = closest.z - goTo->m_vecTargetPoint.z;
            const float distSq = dx * dx + dy * dy + dz * dz;
            if (found) {
                if (distSq > 0.025f) {
                    goTo->GoToPoint(closest, 1.5f, 2.0f, false);
                }
            } else if (distSq > 0.025f) {
                const float radius = CModelInfo::ms_modelInfoPtrs[(int16)m_Car->m_nModelIndex]->GetColModel()->GetBoundRadius() + 1.5f;
                goTo->GoToPoint(m_Car->GetPosition(), radius, 2.0f, false);
            }
        }
    }
    return m_pSubTask;
}
