#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventAcquaintancePedLike : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override { return EVENT_ACQUAINTANCE_PED_LIKE; }
    int32 GetEventPriority() const override { return 22; }
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x5FF110 (lote 188A, sin validar)

public:
    friend void InjectHooksMain();
    static void InjectHooks();
};
VALIDATE_SIZE(CEventAcquaintancePedLike, 0x18);
