#pragma once

#include "Event.h"

#include "Event.h"
#include "EventInAir.h"


class NOTSA_EXPORT_VTABLE CEventStuckInAir : public CEvent {
public:
    CPed* m_ped;

public:
    CEventStuckInAir(CPed* ped);
    ~CEventStuckInAir() override;

    eEventType GetEventType() const override; // 0x4B14F0 (lote 153A, banco Fn153A)
    int32 GetEventPriority() const override;
    int32 GetLifeTime() override; // 0x4B1500 (lote 153A, banco Fn153A)
    CEvent* Clone() const noexcept override { return new CEventStuckInAir(m_ped); }
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;
    bool CanBeInterruptedBySameEvent() override; // 0x4B1510 (lote 153A, banco Fn153A)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventStuckInAir* Constructor(CPed* ped);

};
VALIDATE_SIZE(CEventStuckInAir, 0x10);
