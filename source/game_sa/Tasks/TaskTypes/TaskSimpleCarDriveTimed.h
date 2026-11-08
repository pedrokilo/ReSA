#pragma once

#include "TaskSimpleCarDrive.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarDriveTimed : public CTaskSimpleCarDrive {
public:
    int32      m_nTime;
    CTaskTimer m_nTimer;

public:
    static constexpr auto Type = TASK_SIMPLE_CAR_DRIVE_TIMED;

    CTaskSimpleCarDriveTimed() = delete;
    CTaskSimpleCarDriveTimed(CVehicle* vehicle, int32 nTime = -1);
    ~CTaskSimpleCarDriveTimed() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x46F600
    CTask* Clone() const override { return new CTaskSimpleCarDriveTimed(m_pVehicle, m_nTime); } // 0x46F570
    bool ProcessPed(class CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskSimpleCarDriveTimed* Constructor(CVehicle* vehicle, int32 nTime);
    CTaskSimpleCarDriveTimed* ScalarDeletingDestructor(uint8 flags); // 0x46F670 (lote 128A, banco Fn128A): destructor con borrado (ranura 0 de la vtable 0x859E50), `ret 4`
    // 0x46F690
    __declspec(noinline) CTaskSimpleCarDriveTimed* Destructor() {
        this->CTaskSimpleCarDriveTimed::~CTaskSimpleCarDriveTimed();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleCarDriveTimed, 0x70);
