#pragma once

#include "Event.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"


class NOTSA_EXPORT_VTABLE CEventLeaderQuitEnteringCarAsDriver : public CEvent {
public:
    static void InjectHooks();

    CEventLeaderQuitEnteringCarAsDriver() = default;
    ~CEventLeaderQuitEnteringCarAsDriver() override = default;

    eEventType GetEventType() const override;     // 0x63A130 (lote 156A, banco Fn156A)
    int32 GetEventPriority() const override;      // 0x63A1E0 (lote 156A, banco Fn156A)
    int32 GetLifeTime() override;                 // 0x63A140 (lote 156A, banco Fn156A)
    CEventLeaderQuitEnteringCarAsDriver* Clone() const noexcept override; // 0x63A150 (lote 156A, banco Fn156A)
    bool AffectsPed(CPed* ped) override;          // 0x63A1A0 (lote 156A, banco Fn156A)
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool TakesPriorityOver(const CEvent& refEvent) override; // 0x4B1040 (lote 156A, banco Fn156A)

private:
    CEventLeaderQuitEnteringCarAsDriver* Constructor();
};
VALIDATE_SIZE(CEventLeaderQuitEnteringCarAsDriver, 0xC);
