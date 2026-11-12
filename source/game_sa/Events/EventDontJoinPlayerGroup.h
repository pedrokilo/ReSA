#pragma once

#include "Event.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventDontJoinPlayerGroup : public CEvent {
public:
    CPed* m_player;

public:
    static void InjectHooks();

    CEventDontJoinPlayerGroup(CPed* player);
    ~CEventDontJoinPlayerGroup() override;

    eEventType GetEventType() const override; // 0x6091A0 (lote 155A, banco Fn155A)
    int32 GetEventPriority() const override; // 0x609320 (lote 179A, sin validar)
    int32 GetLifeTime() override; // 0x6091B0 (lote 155A, banco Fn155A)
    CEventDontJoinPlayerGroup* Clone() const noexcept override; // 0x6091C0 (lote 155A, banco Fn155A)
    bool AffectsPed(CPed* ped) override; // 0x609220 (lote 155A, banco Fn155A)

    __declspec(noinline) CEventDontJoinPlayerGroup* Constructor(CPed* player);
};
VALIDATE_SIZE(CEventDontJoinPlayerGroup, 0x10);
