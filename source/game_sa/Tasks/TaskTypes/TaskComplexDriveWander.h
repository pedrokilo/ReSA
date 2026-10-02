#pragma once

#include "TaskComplexCarDrive.h"

// todo: rename -> TaskComplexCarDriveWander.h

class NOTSA_EXPORT_VTABLE CTaskComplexCarDriveWander : public CTaskComplexCarDrive {
public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE_WANDER;

    CTaskComplexCarDriveWander(CVehicle* vehicle, eCarDrivingStyle carDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS, float fSpeed = 10.f);
    ~CTaskComplexCarDriveWander() override = default;

    // 0x63DD00 - el original construye la base con (veh, speed, MODEL_INVALID, style) y le asigna el vtable de CTaskComplexCarDriveWander (0x86E96C),
    // que es exactamente lo que hace el constructor 0x63CB10; antes se clonaba como CTaskComplexCarDrive (tipo de tarea incorrecto).
    CTask* Clone() const override { return new CTaskComplexCarDriveWander(m_Veh, static_cast<eCarDrivingStyle>(m_CarDrivingStyle), m_CruiseSpeed); };
    eTaskType GetTaskType() const override { return Type; } // 0x63CB40
    void SetUpCar() override;
    CTask* CreateSubTaskCannotGetInCar(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63CB10
    CTaskComplexCarDriveWander* Constructor(CVehicle* vehicle, eCarDrivingStyle carDrivingStyle, float fSpeed) {
        this->CTaskComplexCarDriveWander::CTaskComplexCarDriveWander(vehicle, carDrivingStyle, fSpeed);
        return this;
    }
};

VALIDATE_SIZE(CTaskComplexCarDrive, 0x24);
