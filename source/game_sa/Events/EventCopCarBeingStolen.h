#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventCopCarBeingStolen : public CEvent {
public:
    CPed* m_hijacker;
    CVehicle* m_vehicle;

public:
    CEventCopCarBeingStolen(CPed* hijacker, CVehicle* vehicle);
    ~CEventCopCarBeingStolen() override;

    /*!
     * @addr 0x4B17C0
     */
    eEventType GetEventType() const override { return EVENT_COP_CAR_BEING_STOLEN; }
    /*!
     * @addr 0x4B17E0
     */
    int32 GetEventPriority() const override { return 38; }
    /*!
     * @addr 0x4B17D0
     */
    int32 GetLifeTime() override { return 0; }
    CEventCopCarBeingStolen* Clone() const noexcept override; // 0x4B78B0 (lote 201A, sin validar)
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventCopCarBeingStolen* Constructor(CPed* hijacker, CVehicle* vehicle);

};
VALIDATE_SIZE(CEventCopCarBeingStolen, 0x14);
