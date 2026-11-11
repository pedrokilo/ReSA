#pragma once

#include "EventPedCollisionWithPed.h"

#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"


class NOTSA_EXPORT_VTABLE CEventPlayerCollisionWithPed : public CEventPedCollisionWithPed {
public:
    CEventPlayerCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState);
    ~CEventPlayerCollisionWithPed() override = default;

    eEventType GetEventType() const override; // 0x5FEE90 (lote 160A, banco Fn160A)
    CEventPlayerCollisionWithPed* Clone() const noexcept override; // 0x5FEEA0 (lote 160A, banco Fn160A)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPlayerCollisionWithPed* Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState);
};
VALIDATE_SIZE(CEventPlayerCollisionWithPed, 0x34);
