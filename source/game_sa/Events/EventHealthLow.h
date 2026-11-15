#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "Ped.h"


class NOTSA_EXPORT_VTABLE CEventHealthLow : public CEventEditableResponse {
public:
    CEventHealthLow() = default;
    ~CEventHealthLow() override = default;

    eEventType GetEventType() const override { return EVENT_HEALTH_LOW; }
    int32 GetEventPriority() const override { return 68; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); };
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x5FF490 (lote 197A, sin validar)

public:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CEventHealthLow, 0x14);
