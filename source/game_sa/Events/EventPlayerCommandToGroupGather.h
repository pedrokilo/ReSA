#pragma once

#include "EventPlayerCommandToGroup.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventPlayerCommandToGroupGather : public CEventPlayerCommandToGroup {
public:
    static void InjectHooks();

    CEventPlayerCommandToGroupGather(CPed* target) : CEventPlayerCommandToGroup(PLAYER_GROUP_COMMAND_GATHER, target) {}
    ~CEventPlayerCommandToGroupGather() override = default;

    eEventType GetEventType() const override;                                    // 0x609270 (lote 179A, sin validar)
    int32 GetEventPriority() const override;                                     // 0x609330 (lote 179A, sin validar)
    CEventPlayerCommandToGroupGather* CloneEditable() const noexcept override;   // 0x609280 (lote 179A, sin validar)

    CEventPlayerCommandToGroupGather* Constructor(CPed* target);
};
VALIDATE_SIZE(CEventPlayerCommandToGroupGather, 0x1C);
