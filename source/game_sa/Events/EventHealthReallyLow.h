#pragma once

#include "EventEditableResponse.h"
#include "Ped.h"


class NOTSA_EXPORT_VTABLE CEventHealthReallyLow : public CEventEditableResponse {
public:
    CEventHealthReallyLow() = default;
    ~CEventHealthReallyLow() override = default;

    eEventType GetEventType() const override { return EVENT_HEALTH_REALLY_LOW; }
    int32 GetEventPriority() const override { return 69; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); }
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x5FF550 (lote 197A, sin validar)

public:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventHealthReallyLow* Constructor(); // 0x5FF500 (lote 197A, sin validar)
};
VALIDATE_SIZE(CEventHealthReallyLow, 0x14);
