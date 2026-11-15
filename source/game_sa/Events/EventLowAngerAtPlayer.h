#pragma once

#include "EventEditableResponse.h"
#include "Ped.h"


class NOTSA_EXPORT_VTABLE CEventLowAngerAtPlayer : public CEventEditableResponse {
public:
    CEventLowAngerAtPlayer() = default;
    ~CEventLowAngerAtPlayer() override = default;

    eEventType GetEventType() const override { return EVENT_LOW_ANGER_AT_PLAYER; }
    int32 GetEventPriority() const override { return 33; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return ped->IsAlive(); }
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x5FF610 (lote 197A, sin validar)

public:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventLowAngerAtPlayer* Constructor(); // 0x5FF5C0 (lote 197A, sin validar)
};
VALIDATE_SIZE(CEventLowAngerAtPlayer, 0x14);
