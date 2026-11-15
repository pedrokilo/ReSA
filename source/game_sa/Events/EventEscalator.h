#pragma once

#include "Event.h"

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventEscalator : public CEvent {
public:
    CEventEscalator() = default;
    ~CEventEscalator() override = default;

    eEventType GetEventType() const override; // 0x5FF840 (lote 197A, sin validar)
    int32 GetEventPriority() const override; // 0x5FF870 (lote 197A, sin validar)
    int32 GetLifeTime() override; // 0x5FF850 (lote 197A, sin validar)
    CEventEscalator* Clone() const noexcept override; // 0x602B90 (lote 151A, banco Fn151A): ranura 4 de la vtable 0x86CD00
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventEscalator* Constructor(); // 0x5FF820 (lote 197A, sin validar)
};
VALIDATE_SIZE(CEventEscalator, 0xC);
