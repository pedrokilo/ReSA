/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplexEnterCar.h"

class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsPassenger : public CTaskComplexEnterCar {
public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;

    CTaskComplexEnterCarAsPassenger(CVehicle* targetVehicle, int32 nTargetSeat = 0, bool bCarryOnAfterFallingOff = false);
    CTaskComplexEnterCarAsPassenger(const CTaskComplexEnterCarAsPassenger&);
    ~CTaskComplexEnterCarAsPassenger() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x640370
    CTask* Clone() const override { return new CTaskComplexEnterCarAsPassenger{*this}; } // 0x6437F0

private: // Wrappers for hooks
    // 0x640340
    CTaskComplexEnterCarAsPassenger* Constructor(CVehicle* targetVehicle, int32 nTargetSeat, bool bCarryOnAfterFallingOff) {
        this->CTaskComplexEnterCarAsPassenger::CTaskComplexEnterCarAsPassenger(targetVehicle, nTargetSeat, bCarryOnAfterFallingOff);
        return this;
    }
};

VALIDATE_SIZE(CTaskComplexEnterCarAsPassenger, 0x50);
