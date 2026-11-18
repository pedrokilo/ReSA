#pragma once

#include "Event.h"

#include "Event.h"
#include "TaskComplexEnterCar.h"
#include "TheScripts.h"


class NOTSA_EXPORT_VTABLE CEventVehicleToSteal : public CEvent {
public:
    CVehicle* m_vehicle;

public:
    CEventVehicleToSteal(CVehicle* vehicle);
    ~CEventVehicleToSteal() override;

    /*!
     * @addr 0x4AF6D0
     */
    eEventType GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }
    /*!
     * @addr 0x4AF6F0
     */
    int32 GetEventPriority() const override { return 7; }
    /*!
     * @addr 0x4AF6E0
     */
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() const noexcept override; // 0x4B74B0 (lote 187A, sin validar)
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleToSteal* Constructor(CVehicle* vehicle);

};
VALIDATE_SIZE(CEventVehicleToSteal, 0x10);
