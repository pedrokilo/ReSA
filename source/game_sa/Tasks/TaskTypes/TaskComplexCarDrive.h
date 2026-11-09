#pragma once

#include "TaskComplex.h"
#include "eCarMission.h"
#include "eCarDrivingStyle.h"
#include <extensions/WEnum.hpp>

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexCarDrive : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE;

    CTaskComplexCarDrive(CVehicle* vehicle, bool asDriver = true);
    CTaskComplexCarDrive(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate = MODEL_INVALID, eCarDrivingStyle carDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS);
    CTaskComplexCarDrive(const CTaskComplexCarDrive&);
    ~CTaskComplexCarDrive() override;

    CTask*    Clone() const override { return new CTaskComplexCarDrive{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    virtual void   SetUpCar();
    virtual CTask* CreateSubTaskCannotGetInCar(CPed* ped) { return CreateSubTask(TASK_FINISHED, ped); }
    virtual CTask* Drive(CPed* ped) { return m_pSubTask; }

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexCarDrive* Constructor_0(CVehicle* vehicle) { this->CTaskComplexCarDrive::CTaskComplexCarDrive(vehicle); return this; }
    CTaskComplexCarDrive* Constructor_1(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate, eCarDrivingStyle carDrivingStyle) { this->CTaskComplexCarDrive::CTaskComplexCarDrive(vehicle, speed, carModelIndexToCreate, carDrivingStyle); return this; }
    CTaskComplexCarDrive* ScalarDeletingDestructor(uint8 flags); // 0x642F80 (lote 128A, banco Fn128A): destructor con borrado (ranura 0 de la vtable 0x86E934), `ret 4`

public:
    // 0x63CA40: envoltura con hook; la llama el cuerpo reconstruido de CTaskComplexDriveToPoint (0x63CE70; lote 141A, banco Fn141A), sin expansion en linea
    __declspec(noinline) CTaskComplexCarDrive* Destructor() { this->CTaskComplexCarDrive::~CTaskComplexCarDrive(); return this; }
    CVehicle*                         m_Veh{};
    float                             m_CruiseSpeed{};
    eModelID                          m_DesiredCarModel{ MODEL_INVALID };
    notsa::WEnumS32<eCarDrivingStyle> m_CarDrivingStyle{ DRIVING_STYLE_STOP_FOR_CARS };
    bool                              m_bAsDriver{};
    // Como el original: ninguno de los dos constructores (0x63C940, 0x63C9D0) escribe estos tres bytes
    eCarDrivingStyle                  m_OriginalDrivingStyle;
    int8                              m_OriginalMission;
    uint8                             m_OriginalSpeed;
    bool                              m_bIsCarSetUp{};
};
VALIDATE_SIZE(CTaskComplexCarDrive, 0x24);
