#pragma once

#include "TaskComplexEnterCar.h"

class NOTSA_EXPORT_VTABLE CTaskComplexOpenPassengerDoor : public CTaskComplexEnterCar {
public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR;

    CTaskComplexOpenPassengerDoor(CVehicle* vehicle, int32 targetSeat) : CTaskComplexEnterCar(vehicle, false, true, false, false) {
        m_TargetSeat = targetSeat;
    } // 0x6403E0
    ~CTaskComplexOpenPassengerDoor() override = default; // 0x640420

    eTaskType GetTaskType() const override{ return Type; } // 0x640410
    CTask* Clone() const override { return new CTaskComplexOpenPassengerDoor(m_Car, m_TargetSeat); } // 0x6438E0

private: // Wrappers for hooks
    // 0x6403E0
    CTaskComplexOpenPassengerDoor* Constructor(CVehicle* vehicle, int32 targetSeat) {
        this->CTaskComplexOpenPassengerDoor::CTaskComplexOpenPassengerDoor(vehicle, targetSeat);
        return this;
    }
    // 0x640420
    CTaskComplexOpenPassengerDoor* Destructor() {
        this->CTaskComplexOpenPassengerDoor::~CTaskComplexOpenPassengerDoor();
        return this;
    }
};
