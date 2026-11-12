#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventLeanOnVehicle : public CEvent {
public:
    CVehicle* m_vehicle;
    int32 m_leanAnimDurationInMs;

public:
    CEventLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs);
    ~CEventLeanOnVehicle() override;

    eEventType GetEventType() const override;                  // 0x65DB50 (lote 168A, banco Fn168A)
    int32 GetEventPriority() const override;                   // 0x65DE10 (lote 168A, banco Fn168A)
    int32 GetLifeTime() override;                              // 0x65DB60 (lote 168A, banco Fn168A)
    CEventLeanOnVehicle* Clone() const noexcept override;      // 0x65DB70 (lote 168A, banco Fn168A)
    bool AffectsPed(CPed* ped) override;                       // 0x65DBE0 (lote 168A, banco Fn168A)
    bool IsValid(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventLeanOnVehicle* Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs);

};
VALIDATE_SIZE(CEventLeanOnVehicle, 0x14);
