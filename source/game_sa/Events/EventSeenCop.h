#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventSeenCop : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override; // 0x5FF3B0 (lote 196A, sin validar)
    int32 GetEventPriority() const override; // 0x5FF880 (lote 197A, sin validar)
    bool AffectsPed(CPed* ped) override { return CEventAcquaintancePed::AffectsPed(ped); }
    CEventSeenCop* CloneEditable() const noexcept override; // 0x5FF3C0 (lote 196A, sin validar)

public:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventSeenCop* Constructor1(CPed* cop);
};
VALIDATE_SIZE(CEventSeenCop, 0x18);
