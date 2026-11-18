#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventInAir : public CEvent {
public:
    CEventInAir() = default;
    ~CEventInAir() override = default;

    /*!
     * @addr 0x4B0CD0
     */
    eEventType GetEventType() const override { return EVENT_IN_AIR; }
    /*!
     * @addr 0x4B0CF0
     */
    int32 GetEventPriority() const override { return 61; }
    /*!
     * @addr 0x4B0CE0
     */
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() const noexcept override; // 0x4B76A0 (lote 187A, sin validar)
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventInAir* Constructor();

};
VALIDATE_SIZE(CEventInAir, 0xC);
