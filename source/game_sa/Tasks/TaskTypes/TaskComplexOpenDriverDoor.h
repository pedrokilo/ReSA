#pragma once

#include "TaskComplexEnterCar.h"
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexOpenDriverDoor : public CTaskComplexEnterCar {
public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR;

    explicit CTaskComplexOpenDriverDoor(CVehicle* vehicle) : CTaskComplexEnterCar(vehicle, true, true, false, false) {
        m_TargetSeat = 10;
    } // 0x640390
    ~CTaskComplexOpenDriverDoor() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x6403C0
    CTask* Clone() const override {
        auto task = new CTaskComplexOpenDriverDoor(m_Car);
        task->m_MoveState = m_MoveState;
        return task;
    } // 0x643870

private: // Wrappers for hooks
    // 0x640390
    CTaskComplexOpenDriverDoor* Constructor(CVehicle* vehicle) {
        this->CTaskComplexOpenDriverDoor::CTaskComplexOpenDriverDoor(vehicle);
        return this;
    }
    // 0x6403D0 (destructor no-deleting, llamado desde el deleting 0x643CA0 del vtable)
    CTaskComplexOpenDriverDoor* Destructor() {
        this->CTaskComplexOpenDriverDoor::~CTaskComplexOpenDriverDoor();
        return this;
    }
};
