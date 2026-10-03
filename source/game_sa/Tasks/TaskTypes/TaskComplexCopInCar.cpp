/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCopInCar
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexCopInCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexPolicePursuit.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexCarDriveMission.h"
#include "eCarDrivingStyle.h"

void CTaskComplexCopInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCopInCar, 0x870A2C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68C7F0);
    RH_ScopedInstall(Destructor, 0x68C8C0);
    RH_ScopedInstall(CreateSubTask, 0x68C9E0);
    RH_ScopedVMTInstall(Clone, 0x68CEC0);
    RH_ScopedVMTInstall(GetTaskType, 0x68C8B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x68C940);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68FA50);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x68FA10);
    RH_ScopedVMTInstall(ControlSubTask, 0x68FD50);
}

// 0x68C7F0
CTaskComplexCopInCar::CTaskComplexCopInCar(CVehicle* vehicle, CPed* partnerCop, CPed* suspect, bool isDriver) : CTaskComplex(),
    m_Vehicle{ vehicle },
    m_Cop{ partnerCop },
    m_Suspect{ suspect },
    m_IsDriver{ isDriver }
{
    m_flag0x4 = true;
    CEntity::SafeRegisterRef(m_Vehicle);
    CEntity::SafeRegisterRef(m_Cop);
    CEntity::SafeRegisterRef(m_Suspect);
}

// 0x68C8C0
CTaskComplexCopInCar::~CTaskComplexCopInCar() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    CEntity::SafeCleanUpRef(m_Cop);
    CEntity::SafeCleanUpRef(m_Suspect);
}
 
// 0x68C9E0
CTask* CTaskComplexCopInCar::CreateSubTask(eTaskType taskType, CPed* copPed) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(m_Vehicle);
    case TASK_COMPLEX_LEAVE_CAR: {
        copPed->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_KNOCK_OFF_BIKE);
        return new CTaskComplexLeaveCar(m_Vehicle, 0, 0, true, false);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: {
        copPed->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_SHOT_FIRED);
        return new CTaskComplexEnterCarAsDriver(m_Vehicle);
    }
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(1000, true, false, 8.f);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: {
        copPed->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_SHOT_FIRED);
        return new CTaskComplexEnterCarAsPassenger(m_Vehicle, 0, false);
    }
    case TASK_COMPLEX_POLICE_PURSUIT:
        return new CTaskComplexPolicePursuit; // Como el original: sin inicialización por valor (0x68BA70 no escribe 0xD-0xF)
    case TASK_COMPLEX_WANDER:
        return new CTaskComplexWanderCop(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8)); // Como el original: 0x407180
    case TASK_COMPLEX_CAR_DRIVE_MISSION: {
        // Como el original: misión y velocidad se calculan antes de reservar la tarea
        int32 mission;
        float speed;
        if (m_Suspect->IsPlayer()) {
            if (m_Suspect->bInVehicle) {
                mission = (int32)CCarAI::FindPoliceCarMissionForWantedLevel(); // Como el original: se pide antes que la velocidad
                speed = (float)CCarAI::FindPoliceCarSpeedForWantedLevel(m_Vehicle);
            } else {
                speed = (float)CCarAI::FindPoliceCarSpeedForWantedLevel(m_Vehicle);
                mission = MISSION_APPROACHPLAYER_FARAWAY;
            }
        } else {
            // Como el original: un solo producto por 54.0 (0x858C94)
            speed = m_Vehicle->m_pHandlingData->m_transmissionData.m_MaxFlatVelocity * 54.f;
            if (m_Suspect->bInVehicle) {
                mission = (CGeneral::GetRandomNumber() & 3) < 2 ? MISSION_BLOCKPLAYER_FARAWAY : MISSION_RAMPLAYER_FARAWAY;
            } else {
                mission = MISSION_KILLPED_CLOSE;
            }
        }
        const auto targetEntity = m_Suspect->bInVehicle ? (CEntity*)m_Suspect->m_pVehicle : (CEntity*)m_Suspect;
        const auto style = (int32)speed; // Como el original: _ftol2
        return new CTaskComplexCarDriveMission(m_Vehicle, targetEntity, (eCarMission)mission, (eCarDrivingStyle)style, 10.f);
    }
    default: // Como el original: TASK_FINISHED y tipos desconocidos devuelven nullptr
        return nullptr;
    }
}

// 0x68C940
bool CTaskComplexCopInCar::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!m_pSubTask) {
        return true;
    }

    // Como el original: devuelve lo que diga la subtarea
    const bool aborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (!aborted || !event || event->GetEventType() != EVENT_DAMAGE) {
        return aborted;
    }

    const auto dmgEvent = (CEventDamage*)event;
    if (!dmgEvent->m_damageResponse.m_bHealthZero || !dmgEvent->m_bAddToEventGroup || !ped->bInVehicle) {
        return aborted;
    }

    if (ped->m_pVehicle == m_Vehicle && m_Vehicle && m_Vehicle->IsDriver(ped)) {
        m_Vehicle->SetStatus(STATUS_ABANDONED);
        m_Vehicle->m_autoPilot.SetCarMission(MISSION_NONE);
        m_Vehicle->m_autoPilot.SetCruiseSpeed(0);
    }

    return aborted;
}

// 0x68FA50
CTask* CTaskComplexCopInCar::CreateNextSubTask(CPed* ped) {
    if (!m_Suspect || m_Suspect->IsStateDead()) {
        return CreateSubTask(TASK_FINISHED, ped);
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_CAR_DRIVE_MISSION: {
        m_flag0x2 = true;
        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
    }
    case TASK_COMPLEX_POLICE_PURSUIT: {
        const auto subTask = static_cast<CTaskComplexPolicePursuit*>(m_pSubTask);
        const auto wanted = FindPlayerWanted();
        if (wanted->GetWantedLevel() == eWantedLevel::WANTED_CLEAN) {
            return CreateSubTask(TASK_FINISHED, ped);
        }

        // Como el original: sin comprobar el tipo de peatón
        if (wanted->CanCopJoinPursuit(ped->AsCop()) && subTask->m_CouldJoinPursuit) {
            if (m_Suspect->bIsBeingArrested) {
                return CreateSubTask(TASK_FINISHED, ped);
            }

            if (m_Suspect->bInVehicle && m_Suspect->m_pVehicle != m_Vehicle && m_Vehicle) {
                // Como el original: la distancia se calcula antes de mirar si es el conductor
                const float distSq = (ped->GetPosition() - m_Vehicle->GetPosition()).SquaredMagnitude();
                if (m_IsDriver) {
                    if (ped->bInVehicle) {
                        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
                    }
                    if (distSq < 16.f) {
                        return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
                    }
                }
            }
        }

        m_Timer2.m_nStartTime = CTimer::GetTimeInMS();
        m_Timer2.m_nInterval = 3000;
        m_Timer2.m_bStarted = true;
        return CreateSubTask(TASK_COMPLEX_WANDER, ped);
    }
    case TASK_SIMPLE_CAR_DRIVE: {
        m_Vehicle->m_autoPilot.SetCarMission(MISSION_NONE);
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x68FA89
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:    // 0x68FA94
    {
        if (ped->IsInVehicle()) {
            ped->m_pVehicle->ChangeLawEnforcerState(true);
            m_flag0x4 = false;
            m_bIsSuspectInCar = m_Suspect->bInVehicle;
            return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped);
        } else {
            return CreateSubTask(TASK_COMPLEX_POLICE_PURSUIT, ped);
        }
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x68FAA0
        return CreateSubTask(TASK_COMPLEX_POLICE_PURSUIT, ped);
    }
    default:
        return CreateSubTask(TASK_FINISHED, ped);
    }
}

// 0x68FA10
CTask* CTaskComplexCopInCar::CreateFirstSubTask(CPed* ped) {
    ped->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_SHOT_FIRED);
    if (!m_Cop) {
        m_IsDriver = true;
    }
    return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
}

// 0x68FD50
CTask* CTaskComplexCopInCar::ControlSubTask(CPed* ped) {
    const auto subTask = m_pSubTask; // Como el original: se lee al entrar

    if (!m_Suspect || m_Suspect->IsStateDead()) {
        return subTask->MakeAbortable(ped) ? CreateSubTask(TASK_FINISHED, ped) : subTask;
    }

    // 0x68FD7E
    if (m_Vehicle && !m_Vehicle->m_pDriver && m_Vehicle->m_autoPilot.m_nCarMission != MISSION_NONE) {
        m_Vehicle->m_autoPilot.SetCarMission(MISSION_NONE);
    }

    if (!m_IsDriver && (!m_Cop || m_Cop->IsStateDead())) { // 0x68FDA7
        m_IsDriver = true;
    }

    // Como el original: el vehículo del sospechoso va lento (velocidad² · paso · 50 <= 1 o NaN) y cerca (distancia² <= 100 o NaN)
    const auto IsSuspectVehicleSlowAndNear = [ped](CVehicle* suspectVeh) {
        const float distSq = (suspectVeh->GetPosition() - ped->GetPosition()).SquaredMagnitude();
        const CVector moveSpeed = suspectVeh->m_vecMoveSpeed;
        if (moveSpeed.SquaredMagnitude() * CTimer::GetTimeStep() * 50.f > 1.f) {
            return false;
        }
        return !(distSq > 100.f);
    };

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_POLICE_PURSUIT: { // 0x690061
        if (!m_Suspect->bInVehicle) {
            return subTask;
        }
        const auto suspectVeh = m_Suspect->m_pVehicle;
        if (!suspectVeh || suspectVeh == m_Vehicle || !m_Vehicle) {
            return subTask;
        }
        // Como el original: solo el conductor vuelve al coche
        if (!m_IsDriver) {
            return subTask;
        }
        if (!(m_Vehicle->m_fHealth > 0.f)) { // Como el original: NaN tampoco sigue
            return subTask;
        }
        if (m_Vehicle->m_pFire) {
            return subTask;
        }
        if (m_Vehicle->IsUpsideDown()) { // Como el original: 0x6D1D90
            return subTask;
        }
        if (m_Vehicle->IsOnItsSide()) {
            return subTask;
        }
        const float distSq = (m_Vehicle->GetPosition() - ped->GetPosition()).SquaredMagnitude();
        if (IsSuspectVehicleSlowAndNear(suspectVeh)) {
            return subTask;
        }
        if (!(distSq < 16.f)) {
            return subTask;
        }
        return subTask->MakeAbortable(ped) ? CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped) : subTask;
    }
    case TASK_COMPLEX_WANDER: {
        return m_Timer2.IsOutOfTime() && subTask->MakeAbortable(ped)
                   ? CreateSubTask(TASK_COMPLEX_POLICE_PURSUIT, ped)
                   : subTask;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION: { // 0x68FDDE
        if ((bool)m_bIsSuspectInCar != m_Suspect->bInVehicle) {
            if (subTask->MakeAbortable(ped)) {
                m_bIsSuspectInCar = m_Suspect->bInVehicle;
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped);
            }
        }
        return m_flag0x2 && subTask->MakeAbortable(ped)
                   ? CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped)
                   : subTask;
    }
    case TASK_SIMPLE_CAR_DRIVE: { // 0x68FE65
        if (!m_flag0x2) {
            return subTask;
        }

        if (!m_IsDriver) {
            if (subTask->MakeAbortable(ped)) {
                m_flag0x2 = false;
                return CreateNextSubTask(ped);
            }
            return subTask;
        }

        if (!m_Timer1.m_bStarted) {
            // Como el original: 0x407180 y sin tocar m_bStopped
            m_Timer1.m_nInterval = m_flag0x4 ? CGeneral::GetRandomIntInRange(0, 2000) + 3000 : CGeneral::GetRandomIntInRange(0, 1250) + 250;
            m_Timer1.m_nStartTime = CTimer::GetTimeInMS();
            m_Timer1.m_bStarted = true;
            return subTask;
        }

        // Como el original: el sospechoso se aleja en otro vehículo -> nueva misión si la subtarea se deja abortar
        if (m_Suspect->bInVehicle) {
            const auto suspectVeh = m_Suspect->m_pVehicle;
            if (suspectVeh && suspectVeh != m_Vehicle && !IsSuspectVehicleSlowAndNear(suspectVeh) && subTask->MakeAbortable(ped)) {
                m_flag0x2 = false;
                m_flag0x4 = false;
                m_Timer1.m_bStarted = false;
                m_bIsSuspectInCar = m_Suspect->bInVehicle;
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped);
            }
        }

        if (m_Timer1.IsOutOfTime() && subTask->MakeAbortable(ped)) {
            m_flag0x2 = false;
            m_Timer1.m_bStarted = false;
            return CreateNextSubTask(ped);
        }
        return subTask;
    }
    default:
        break;
    }
    return subTask;
}
