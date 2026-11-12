#pragma once

#include "Event.h"

#include "Event.h"
#include "Interior_c.h"
#include "InteriorInfo_t.h"


class NOTSA_EXPORT_VTABLE CEventInteriorUseInfo : public CEvent {
public:
    InteriorInfo_t* m_InteriorInfo;
    Interior_c*     m_Interior;
    uint32          m_ActionAnimTime;
    int8            m_nLoopAction;

public:
    CEventInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction);
    ~CEventInteriorUseInfo() override = default;

    eEventType GetEventType() const override { return EVENT_INTERIOR_USE_INFO; }
    int32 GetEventPriority() const override { return 9; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() const noexcept override; // 0x674F00 (lote 184A, sin validar)
    bool AffectsPed(CPed* ped) override;
    bool IsValid(CPed* ped) override; // 0x4B1640 (lote 153A, banco Fn153A)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventInteriorUseInfo* Constructor(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction); // 0x674EA0 (lote 184A, sin validar)
};
VALIDATE_SIZE(CEventInteriorUseInfo, 0x1C);
