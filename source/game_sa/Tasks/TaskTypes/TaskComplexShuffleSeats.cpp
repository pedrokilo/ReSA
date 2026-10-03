/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexShuffleSeats
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexShuffleSeats.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskSimpleCarSetPedOut.h"

void CTaskComplexShuffleSeats::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexShuffleSeats, 0x86ea40, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63D130);
    RH_ScopedInstall(Destructor, 0x63D1A0);

    RH_ScopedInstall(CreateSubTask, 0x63D240);
    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x63D610);
    RH_ScopedVMTInstall(Clone, 0x63DEB0);
    RH_ScopedVMTInstall(GetTaskType, 0x63D190);
    RH_ScopedVMTInstall(MakeAbortable, 0x63D220);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63D420);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643700);
    RH_ScopedVMTInstall(ControlSubTask, 0x63D5C0);
}

// 0x63D130
CTaskComplexShuffleSeats::CTaskComplexShuffleSeats(CVehicle* veh) :
    m_Veh{veh}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexShuffleSeats::CTaskComplexShuffleSeats(const CTaskComplexShuffleSeats& o) :
    CTaskComplexShuffleSeats{o.m_Veh}
{
}

// 0x63D1A0
CTaskComplexShuffleSeats::~CTaskComplexShuffleSeats() {
    // Como el original: primero la referencia al vehiculo (sin anularla) y despues la utilidad
    CEntity::SafeCleanUpRef(m_Veh);
    if (m_TaskUtilityLineUpPedWithCar) { // Como el original: solo llama a delete si hay utilidad
        delete m_TaskUtilityLineUpPedWithCar;
    }
}

// 0x63D240
CTask* CTaskComplexShuffleSeats::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
        m_Veh->RemovePassenger(ped); // Como el original: sale del asiento de pasajero antes de pasar a conductor
        return new CTaskSimpleCarSetPedInAsDriver{m_Veh, m_TaskUtilityLineUpPedWithCar};
    case TASK_SIMPLE_CAR_SHUFFLE:
        return new CTaskSimpleCarShuffle{ m_Veh, m_OriginDoor, m_TaskUtilityLineUpPedWithCar };
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER: {
        if (m_Veh->m_pDriver == ped) {
            m_Veh->RemoveDriver(true);
        } else {
            m_Veh->RemovePassenger(ped);
        }
        return new CTaskSimpleCarSetPedInAsPassenger{ m_Veh, (eTargetDoor)m_TargetDoor, false, m_TaskUtilityLineUpPedWithCar };
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT: {
        // Como el original: usa el vehiculo del peaton (+0x58C), no el de la tarea
        const auto door = CCarEnterExit::ComputeTargetDoorToExit(ped->m_pVehicle, ped);
        return new CTaskSimpleCarSetPedOut{ ped->m_pVehicle, (eTargetDoor)door, true };
    }
    case TASK_FINISHED: {
        if (ped && !ped->bInVehicle) { // Como el original: comprueba el peaton
            ped->SetUsesCollision(true);
        }
        return nullptr;
    }
    default:
        return nullptr; // Como el original
    }
}

// 0x63D610
void CTaskComplexShuffleSeats::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    // Como el original: compara directamente conductor y los dos primeros pasajeros
    if (m_Veh->m_pDriver == ped) {
        m_OriginDoor = TARGET_DOOR_DRIVER;
    } else if (m_Veh->m_apPassengers[0] == ped) {
        m_OriginDoor = TARGET_DOOR_FRONT_RIGHT;
    } else if (m_Veh->m_apPassengers[1] == ped) {
        m_OriginDoor = TARGET_DOOR_REAR_LEFT;
    } else {
        m_OriginDoor = TARGET_DOOR_REAR_RIGHT;
    }

    const auto& pedPos  = ped->GetPosition();
    const auto  doorPos = CCarEnterExit::GetPositionToOpenCarDoor(m_Veh, m_OriginDoor);
    const CVector offset{ doorPos.x - pedPos.x, doorPos.y - pedPos.y, doorPos.z - pedPos.z };
    m_TaskUtilityLineUpPedWithCar = new CTaskUtilityLineUpPedWithCar{
        offset,
        600,
        0,
        m_OriginDoor
    };
}

// 0x63D220
bool CTaskComplexShuffleSeats::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x63D420
CTask* CTaskComplexShuffleSeats::CreateNextSubTask(CPed* ped) {
    if (m_bAbort) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
        case TASK_SIMPLE_CAR_SHUFFLE:
        case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
            return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
        case TASK_NONE:
        case TASK_SIMPLE_CAR_SET_PED_OUT:
            return CreateSubTask(TASK_FINISHED, ped);
        default:
            return nullptr; // Como el original
        }
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
    case TASK_SIMPLE_CAR_SET_PED_OUT: // Como el original
    case TASK_NONE:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_CAR_SHUFFLE: {
        switch (m_OriginDoor) {
        // Front seats
        case TARGET_DOOR_FRONT_RIGHT: m_TargetDoor = TARGET_DOOR_DRIVER;      break;
        case TARGET_DOOR_DRIVER:      m_TargetDoor = TARGET_DOOR_FRONT_RIGHT; break;

        // Rear seats
        case TARGET_DOOR_REAR_RIGHT:  m_TargetDoor = TARGET_DOOR_REAR_LEFT;   break;
        case TARGET_DOOR_REAR_LEFT:   m_TargetDoor = TARGET_DOOR_REAR_RIGHT;  break;
        default:                      break; // Como el original: conserva la puerta destino
        }
        if (m_TargetDoor == TARGET_DOOR_DRIVER) { // Inverted
            if (!m_Veh->m_pDriver) {
                return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER, ped);
            }
        } else {
            if (!m_Veh->m_apPassengers.data()[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Veh, m_TargetDoor)]) {
                return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER, ped);
            }
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    default:
        return nullptr; // Como el original
    }
}

// 0x643700
CTask* CTaskComplexShuffleSeats::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (m_Veh && CCarEnterExit::IsVehicleHealthy(m_Veh) && CCarEnterExit::IsPedHealthy(ped)) {
            CreateTaskUtilityLineUpPedWithCar(ped);
            return TASK_SIMPLE_CAR_SHUFFLE;
        } 
        return TASK_NONE;
    }(), ped);
}

// 0x63D5C0
CTask* CTaskComplexShuffleSeats::ControlSubTask(CPed* ped) {
    // Como el original: consulta el tipo de la subtarea (sin usarlo) y devuelve la subtarea leida al entrar
    const auto subTask = m_pSubTask;
    std::ignore = subTask->GetTaskType();
    if (!m_Veh || !CCarEnterExit::IsVehicleHealthy(m_Veh) || !CCarEnterExit::IsPedHealthy(ped)) {
        m_bAbort = true;
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }
    return subTask;
}
