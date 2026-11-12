#pragma once

#include "Event.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventNewGangMember : public CEvent {
public:
    CPed* m_member;

public:
    static void InjectHooks();

    CEventNewGangMember(CPed* member);
    ~CEventNewGangMember() override;

    eEventType GetEventType() const override;                  // 0x608FD0 (lote 168A, banco Fn168A)
    int32 GetEventPriority() const override { return 42; }
    int32 GetLifeTime() override;                              // 0x608FE0 (lote 168A, banco Fn168A)
    CEventNewGangMember* Clone() const noexcept override;      // 0x608FF0 (lote 168A, banco Fn168A)
    bool AffectsPed(CPed* ped) override;                       // 0x609050 (lote 168A, banco Fn168A)

    CEventNewGangMember* Constructor(CPed* member);
};
VALIDATE_SIZE(CEventNewGangMember, 0x10);
