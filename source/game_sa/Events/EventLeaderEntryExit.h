#pragma once

#include "EventAreaCodes.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"


class NOTSA_EXPORT_VTABLE CEventLeaderEntryExit : public CEventAreaCodes {
public:
    static void InjectHooks();
    CEventLeaderEntryExit* Constructor(CPed* ped);

    CEventLeaderEntryExit(CPed* ped) : CEventAreaCodes(ped) {}
    ~CEventLeaderEntryExit() override = default;

    eEventType GetEventType() const override;                      // 0x43E200 (lote 158A, banco Fn158A)
    int32 GetEventPriority() const override;                        // 0x43E2C0 (lote 158A)
    int32 GetLifeTime() override;                                   // 0x43E210 (lote 158A)
    CEventLeaderEntryExit* Clone() const noexcept override;         // 0x43E220 (lote 158A)
    bool HasEditableResponse() const override;                      // 0x43E1E0 (lote 158A)
    CEntity* GetSourceEntity() const override;                      // 0x43E1F0 (lote 158A)
    bool AffectsPedGroup(CPedGroup* pedGroup) override { return m_ped && pedGroup->GetMembership().GetLeader() == m_ped; }
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    bool CanBeInterruptedBySameEvent() override;                    // 0x43E280 (lote 158A)
};
VALIDATE_SIZE(CEventLeaderEntryExit, 0x10);
