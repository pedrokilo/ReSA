#pragma once

#include "TaskComplexCarDrive.h"
class CVehicle;

enum field_30_enum : int32 {
    DEFAULT,
    ACCURATE,
    STRAIGHT_LINE,
    RACING,
};

class NOTSA_EXPORT_VTABLE CTaskComplexDriveToPoint : public CTaskComplexCarDrive {
public:
    CVector m_Point;
    field_30_enum field_30;
    float m_Radius;
    bool field_38;

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_CAR_DRIVE_TO_POINT;

    CTaskComplexDriveToPoint(CVehicle* vehicle, const CVector& point, float speed, int32 arg4, eModelID carModelIndexToCreate, float radius, eCarDrivingStyle drivingStyle);
    ~CTaskComplexDriveToPoint() override = default;

    eTaskType GetTaskType() const override { return Type;} // 0x63CE60
    CTask* Clone() const override { return new CTaskComplexDriveToPoint(m_Veh, m_Point, m_CruiseSpeed, field_30, m_DesiredCarModel, m_Radius, static_cast<eCarDrivingStyle>(m_CarDrivingStyle)); } // 0x63DDE0

    void SetUpCar() override;
    CTask* CreateSubTaskCannotGetInCar(CPed* ped) override;
    CTask* Drive(CPed* ped) override;


    bool IsTargetBlocked(CPed* ped) const;
    bool IsTargetBlocked(CPed* ped, CEntity** entities, int32 numEntities) const;

    /// NOTSA
    void GoToPoint(const CVector& point);

private: // Wrappers for hooks
    // 0x63CE00
    CTaskComplexDriveToPoint* Constructor(CVehicle* vehicle, const CVector& point, float speed, int32 arg4, eModelID carModelIndexToCreate, float radius, eCarDrivingStyle drivingStyle) {
        this->CTaskComplexDriveToPoint::CTaskComplexDriveToPoint(vehicle, point, speed, arg4, carModelIndexToCreate, radius, drivingStyle);
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexDriveToPoint, 0x3C);
