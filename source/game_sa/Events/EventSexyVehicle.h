#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventSexyVehicle : public CEvent {
public:
    CVehicle* m_vehicle;

public:
    CEventSexyVehicle(CVehicle* vehicle);
    ~CEventSexyVehicle() override;

    eEventType GetEventType() const override { return EVENT_SEXY_VEHICLE; }
    int32 GetEventPriority() const override { return 3; }
    int32 GetLifeTime() override { return 0; }
    CEventSexyVehicle* Clone() const noexcept override; // 0x4B72F0 (lote 199A, sin validar)
    bool AffectsPed(CPed* ped) override;                // 0x4AF100 (lote 199A, sin validar)
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventSexyVehicle* Constructor(CVehicle* vehicle);
};
VALIDATE_SIZE(CEventSexyVehicle, 0x10);
