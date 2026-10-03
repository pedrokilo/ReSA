/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveCarAsPassengerWait
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
#include "extensions/utility.hpp"
#include "TaskComplexLeaveCarAsPassengerWait.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
 
void CTaskComplexLeaveCarAsPassengerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAsPassengerWait, 0x86e7a4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B430);
    RH_ScopedInstall(Destructor, 0x63B4A0);

    RH_ScopedInstall(GetNextPassengerToLeave, 0x63B500);
    RH_ScopedInstall(SetPedInZerothPassengerSeat, 0x63B540);
    RH_ScopedInstall(CreateSubTask, 0x640BE0);

    RH_ScopedVMTInstall(Clone, 0x63D8C0);
    RH_ScopedVMTInstall(GetTaskType, 0x63B490);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x643F90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x644010);
    RH_ScopedVMTInstall(ControlSubTask, 0x644050);
}

// 0x63B430
CTaskComplexLeaveCarAsPassengerWait::CTaskComplexLeaveCarAsPassengerWait(CVehicle* veh) :
    m_veh{veh}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveCarAsPassengerWait::CTaskComplexLeaveCarAsPassengerWait(const CTaskComplexLeaveCarAsPassengerWait& o) :
    CTaskComplexLeaveCarAsPassengerWait{ o.m_veh }
{
}

// 0x63B4A0
CTaskComplexLeaveCarAsPassengerWait::~CTaskComplexLeaveCarAsPassengerWait(){
    CEntity::SafeCleanUpRef(m_veh);
}

/*!
* @addr 0x63B500
* @brief Get the very next passanger (any of the occupants excluding the driver) to leave the vehicle
*/
CPed* CTaskComplexLeaveCarAsPassengerWait::GetNextPassengerToLeave() {
    // Como el original: recorre m_nMaxPassengers asientos sin limitarse a los 8 del array
    CPed* const* const passengers = m_veh->m_apPassengers.data();
    const int32 numSeats = m_veh->m_nMaxPassengers;
    for (int32 i = 0; i < numSeats; i++) {
        if (passengers[i]) {
            return passengers[i];
        }
    }
    return nullptr;
}

// 0x63B540
void CTaskComplexLeaveCarAsPassengerWait::SetPedInZerothPassengerSeat(CPed* ped) {
    CTaskSimpleCarSetPedOut{ m_veh, TARGET_DOOR_FRONT_RIGHT, true, true }.ProcessPed(ped);
    CTaskSimpleCarSetPedInAsPassenger{ m_veh, TARGET_DOOR_FRONT_RIGHT, true }.ProcessPed(ped);
}

// 0x640BE0
CTask* CTaskComplexLeaveCarAsPassengerWait::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GO_TO_POINT: {
        // Como el original: tres rand() en este orden: distancia hacia delante (1..2), signo (> 0.5 -> +1) y distancia lateral (1..2);
        // desplazamiento = derecha * lateral + delante * frontal, despues se suma la posicion del peaton
        const CVector right   = m_veh->GetRight();
        const CVector forward = m_veh->GetForward();
        float fwdMult = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL + 1.f;
        fwdMult = ((float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL > 0.5f ? 1.f : -1.f) * fwdMult;
        const float rightMult = (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL + 1.f;
        const CVector offset{
            right.x * rightMult + forward.x * fwdMult,
            right.y * rightMult + forward.y * fwdMult,
            right.z * rightMult + forward.z * fwdMult
        };
        const CVector& pedPos = ped->GetPosition();
        return new CTaskSimpleGoToPoint{
            PEDMOVE_WALK,
            CVector{ offset.x + pedPos.x, offset.y + pedPos.y, offset.z + pedPos.z },
            1.5f
        };
    }
    case TASK_COMPLEX_LEAVE_CAR: {
        SetPedInZerothPassengerSeat(ped);
        return new CTaskComplexLeaveCar{
            m_veh,
            TARGET_DOOR_FRONT_LEFT,
            0,
            true,
            false
        };
    }
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive{ m_veh };
    case TASK_FINISHED:
        return nullptr;
    default: // Como el original: tipos desconocidos devuelven nullptr
        return nullptr;
    }
}

// 0x643F90
CTask* CTaskComplexLeaveCarAsPassengerWait::CreateNextSubTask(CPed* ped) {
    const auto nextType = [this] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_LEAVE_CAR:
            return m_veh
                ? TASK_SIMPLE_GO_TO_POINT
                : TASK_FINISHED;
        case TASK_SIMPLE_CAR_DRIVE:
        case TASK_SIMPLE_GO_TO_POINT:
            return TASK_FINISHED;
        default: // Como el original: subtarea desconocida -> nullptr sin llamar a CreateSubTask
            return TASK_NONE;
        }
    }();
    return nextType == TASK_NONE ? nullptr : CreateSubTask(nextType, ped);
}

// 0x644010
CTask* CTaskComplexLeaveCarAsPassengerWait::CreateFirstSubTask(CPed* ped) {
    if (!m_veh) {
        return nullptr; // Como el original: sin llamar a CreateSubTask
    }
    return CreateSubTask(
        GetNextPassengerToLeave() == ped
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_SIMPLE_CAR_DRIVE,
        ped
    );
}

// 0x644050
CTask* CTaskComplexLeaveCarAsPassengerWait::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE) {
        if (GetNextPassengerToLeave() == ped) {
            if (m_pSubTask->MakeAbortable(ped)) {
                return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
            }
        }
    }
    return m_pSubTask;
}
