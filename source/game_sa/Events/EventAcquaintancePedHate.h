#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventAcquaintancePedHate : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override;                         // 0x420F40 (lote 190A, sin validar)
    int32 GetEventPriority() const override;                          // 0x420FD0 (lote 190A, sin validar)
    int32 GetLifeTime() override;                                     // 0x420F00 (lote 190A, sin validar)
    CEntity* GetSourceEntity() const override;                        // 0x420F10 (lote 190A, sin validar)
    bool CanBeInterruptedBySameEvent() override;                      // 0x420F20 (lote 190A, sin validar)
    CEventEditableResponse* CloneEditable() const noexcept override;  // 0x420F50 (lote 190A, sin validar)

public:
    friend class CEventAcquaintancePed;
    static void InjectHooks();

    CEventAcquaintancePedHate* Constructor2(CPed* ped);
};
VALIDATE_SIZE(CEventAcquaintancePedHate, 0x18);
