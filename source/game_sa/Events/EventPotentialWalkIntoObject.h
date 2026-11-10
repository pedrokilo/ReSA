#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "Enums/eMoveState.h"
#include "Events/EventPotentialWalkIntoPed.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "Events/EventPotentialWalkIntoVehicle.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleGoTo.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskComplexFollowPedFootsteps.h"
#include "TaskComplexKillPedOnFoot.h"


class NOTSA_EXPORT_VTABLE CEventPotentialWalkIntoObject : public CEventEditableResponse {
public:
    CObject* m_object;
    int32 m_moveState;

public:
    CEventPotentialWalkIntoObject(CObject* object, int32 moveState);
    ~CEventPotentialWalkIntoObject() override;

    eEventType GetEventType() const override;                       // 0x4AE640 (lote 152A, banco Fn152A): `mov eax,2Ch; ret` (ranura 1)
    bool TakesPriorityOver(const CEvent& refEvent) override;        // 0x4AE6D0 (lote 152A, banco Fn152A): `mov al,1; ret 4` (ranura 11)
    CEventPotentialWalkIntoObject* CloneEditable() const noexcept override { return new CEventPotentialWalkIntoObject(m_object, m_moveState); }
    int32 GetEventPriority() const override;                        // 0x4AE660 (lote 152A, banco Fn152A): `mov eax,1Bh; ret` (ranura 2)
    int32 GetLifeTime() override;                                   // 0x4AE650 (lote 152A, banco Fn152A): `xor eax,eax; ret` (ranura 3)
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPotentialWalkIntoObject* Constructor(CObject* object, int32 moveState);

};
VALIDATE_SIZE(CEventPotentialWalkIntoObject, 0x1C);
