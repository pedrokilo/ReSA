#include "StdInc.h"

#include "TaskComplexDriveWander.h"
#include "CarCtrl.h"

void CTaskComplexCarDriveWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveWander, 0x86E96C, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63CB10);

    RH_ScopedVMTInstall(Clone, 0x63DD00);
    RH_ScopedVMTInstall(GetTaskType, 0x63CB40);
    RH_ScopedVMTInstall(SetUpCar, 0x63CB60);
    RH_ScopedVMTInstall(CreateSubTaskCannotGetInCar, 0x643240);
}

// 0x63CB10
CTaskComplexCarDriveWander::CTaskComplexCarDriveWander(CVehicle* vehicle, eCarDrivingStyle carDrivingStyle, float fSpeed)
    : CTaskComplexCarDrive(vehicle, fSpeed, MODEL_INVALID, carDrivingStyle)
{
    // NOP
}

// 0x63CB60
void CTaskComplexCarDriveWander::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission      = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed        = m_Veh->m_autoPilot.m_nCruiseSpeed;
    m_bIsCarSetUp          = true;
    if (!CCarCtrl::bCarIsBeingCreated) {
        CCarCtrl::JoinCarWithRoadSystem(m_Veh);
        m_Veh->SetStatus(STATUS_PHYSICS);
        m_Veh->m_autoPilot.SetCarMission(MISSION_CRUISE);
        m_Veh->m_autoPilot.SetCruiseSpeed((uint8)m_CruiseSpeed);
        m_Veh->m_autoPilot.m_speed = m_Veh->m_autoPilot.m_nCruiseSpeed;
        m_Veh->m_autoPilot.m_nCarDrivingStyle = static_cast<eCarDrivingStyle>(m_CarDrivingStyle);
    }
    if (m_Veh->vehicleFlags.bEngineBroken) {
        m_Veh->vehicleFlags.bEngineOn = false;
        m_Veh->vehicleFlags.bIsLawEnforcer = false;
    } else {
        m_Veh->vehicleFlags.bEngineOn = true;
    }
    m_Veh->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

// 0x643240
CTask* CTaskComplexCarDriveWander::CreateSubTaskCannotGetInCar(CPed* ped) {
    return CTaskComplexCarDrive::CreateSubTask(TASK_COMPLEX_WANDER, ped);
}
