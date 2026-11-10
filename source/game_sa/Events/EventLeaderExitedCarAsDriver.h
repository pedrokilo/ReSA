#pragma once

#include "EventEditableResponse.h"
#include "PedGroup.h"

#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"

class NOTSA_EXPORT_VTABLE CEventLeaderExitedCarAsDriver : public CEventEditableResponse {
public:
    static void InjectHooks();

    CEventLeaderExitedCarAsDriver() = default;
    ~CEventLeaderExitedCarAsDriver() override = default;

    eEventType GetEventType() const override; // 0x4B8320 (lote 155A, banco Fn155A)
    int32 GetEventPriority() const override; // 0x4B84D0 (lote 155A, banco Fn155A)
    int32 GetLifeTime() override; // 0x4B8330 (lote 155A, banco Fn155A)
    bool AffectsPed(CPed* ped) override; // 0x4B8390 (lote 155A, banco Fn155A)
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    CEventLeaderExitedCarAsDriver* CloneEditable() const noexcept override; // 0x4B8340 (lote 156A, banco Fn156A): ranura 16 de la vtable 0x85B898

private:
    CEventLeaderExitedCarAsDriver* Constructor();
};
VALIDATE_SIZE(CEventLeaderExitedCarAsDriver, 0x14);
