#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"


class NOTSA_EXPORT_VTABLE CEventLeaderEnteredCarAsDriver : public CEventEditableResponse {
public:
    CVehicle* m_vehicle;

public:
    static void InjectHooks();

    CEventLeaderEnteredCarAsDriver(CVehicle* vehicle);
    ~CEventLeaderEnteredCarAsDriver() override;

    eEventType GetEventType() const override;                  // 0x48E280 (lote 169A, banco Fn169A)
    int32 GetEventPriority() const override;                   // 0x48E340 (lote 169A)
    int32 GetLifeTime() override;                              // 0x48E290 (lote 169A)
    bool AffectsPed(CPed* ped) override;                       // 0x48E300 (lote 169A)
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    bool CanBeInterruptedBySameEvent() override;               // 0x48E310 (lote 169A)
    CEventLeaderEnteredCarAsDriver* CloneEditable() const noexcept override; // 0x48E2A0 (lote 169A): ranura 16 de la vtable 0x85A198

private:
    CEventLeaderEnteredCarAsDriver* Constructor(CVehicle* vehicle);

};
VALIDATE_SIZE(CEventLeaderEnteredCarAsDriver, 0x18);
