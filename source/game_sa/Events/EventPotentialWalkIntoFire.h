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


class NOTSA_EXPORT_VTABLE CEventPotentialWalkIntoFire : public CEventEditableResponse {
public:
    CVector m_firePos;
    float m_fireSize;
    float m_radius;
    eMoveState m_moveState;

public:
    CEventPotentialWalkIntoFire(const CVector& firePos, float fireSize, eMoveState moveState);
    ~CEventPotentialWalkIntoFire() override = default;

    eEventType GetEventType() const override;   // 0x4B1ED0 (lote 184A, sin validar)
    int32 GetEventPriority() const override;     // 0x4B1EF0 (lote 184A, sin validar)
    int32 GetLifeTime() override;                // 0x4B1EE0 (lote 184A, sin validar)
    bool AffectsPed(CPed* ped) override;
    CEventPotentialWalkIntoFire* CloneEditable() const noexcept override { return new CEventPotentialWalkIntoFire(m_firePos, m_fireSize, m_moveState); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPotentialWalkIntoFire* Constructor(const CVector& firePos, float fireSize, eMoveState moveState);

};
VALIDATE_SIZE(CEventPotentialWalkIntoFire, 0x2C);
