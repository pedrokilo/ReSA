#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventEscalator : public CEvent {
public:
    CEventEscalator() = default;
    ~CEventEscalator() override = default;

    eEventType GetEventType() const override { return EVENT_ON_ESCALATOR; }
    int32 GetEventPriority() const override { return 56; }
    int32 GetLifeTime() override { return 0; }
    CEventEscalator* Clone() const noexcept override; // 0x602B90 (lote 151A, banco Fn151A): ranura 4 de la vtable 0x86CD00
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CEventEscalator, 0xC);
