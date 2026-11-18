#pragma once

#include "Event.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventVehicleDied : public CEvent {
public:
    CVehicle* m_vehicle;
    bool      m_IsOccupantLeavingCar;

public:
    CEventVehicleDied (CVehicle* vehicle);
    ~CEventVehicleDied () override;

    /*!
     * @addr 0x4B0D70
     */
    eEventType GetEventType() const override { return EVENT_VEHICLE_DIED; }
    /*!
     * @addr 0x4B0D90
     */
    int32 GetEventPriority() const override { return 72; }
    /*!
     * @addr 0x4B0D80
     */
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() const noexcept override { return new CEventVehicleDied(m_vehicle); } // 0x4B76D0
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleDied* Constructor(CVehicle* vehicle);

};

VALIDATE_SIZE(CEventVehicleDied, 0x14);
