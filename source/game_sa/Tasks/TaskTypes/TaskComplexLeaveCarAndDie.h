#pragma once

#include "TaskComplexLeaveCar.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAndDie : public CTaskComplexLeaveCar {
 
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AND_DIE;

    CTaskComplexLeaveCarAndDie(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime); // 0x62F1A0
    CTaskComplexLeaveCarAndDie(const CTaskComplexLeaveCarAndDie&);

    CTask*    Clone() const override { return new CTaskComplexLeaveCarAndDie{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateFirstSubTask(CPed * ped) override;

private:
    CTaskComplexLeaveCarAndDie* Constructor(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime) { this->CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie(targetVehicle, nTargetDoor, nDelayTime); return this; }
};
