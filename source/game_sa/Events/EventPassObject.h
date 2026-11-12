#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventPassObject : public CEvent {
public:
    CPed* m_giver;
    bool  m_dontPassObject;

public:
    explicit CEventPassObject(CPed* giver, bool dontPassObject = false);
    ~CEventPassObject() override;

    eEventType GetEventType() const override;                  // 0x65DD50 (lote 168A, banco Fn168A)
    int32 GetEventPriority() const override;                   // 0x65DE20 (lote 168A, banco Fn168A)
    int32 GetLifeTime() override;                              // 0x65DD60 (lote 168A, banco Fn168A)
    CEventPassObject* Clone() const noexcept override;         // 0x65DD70 (lote 168A, banco Fn168A)
    bool AffectsPed(CPed* ped) override;                       // 0x65DDE0 (lote 168A, banco Fn168A)
    bool IsValid(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPassObject* Constructor(CPed* giver, bool dontPassObject);
    CEventPassObject* Destructor();

};
VALIDATE_SIZE(CEventPassObject, 0x14);
