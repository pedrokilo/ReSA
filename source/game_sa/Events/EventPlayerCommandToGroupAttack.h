#pragma once

#include "EventPlayerCommandToGroup.h"
#include "EventEditableResponse.h"

class CPedGroup;
class CPed;

class NOTSA_EXPORT_VTABLE CEventPlayerCommandToGroupAttack : public CEventPlayerCommandToGroup {
public:
    static void InjectHooks();

    CEventPlayerCommandToGroupAttack(CPed* target) : CEventPlayerCommandToGroup(PLAYER_GROUP_COMMAND_ATTACK, target) {}
    ~CEventPlayerCommandToGroupAttack() override = default;

    eEventType GetEventType() const override;         // 0x5F6360 (lote 157A, banco Fn157A)
    int32 GetEventPriority() const override;          // 0x5F6400 (lote 157A)
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    CEventPlayerCommandToGroupAttack* CloneEditable() const noexcept override; // 0x5F6370 (lote 157A)

private:
    CEventPlayerCommandToGroupAttack* Constructor(CPed* target);
};
VALIDATE_SIZE(CEventPlayerCommandToGroupAttack, 0x1C);
