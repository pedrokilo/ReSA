#pragma once

#include "EventPedCollisionWithPed.h"
#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>
#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"

class NOTSA_EXPORT_VTABLE CEventPedCollisionWithPlayer : public CEventPedCollisionWithPed {
public:
    CEventPedCollisionWithPlayer(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState);
    ~CEventPedCollisionWithPlayer() override = default;

    eEventType GetEventType() const override; // 0x5FED90 (lote 160A, banco Fn160A)
    CEventPedCollisionWithPlayer* Clone() const noexcept override; // 0x5FEDA0 (lote 160A, banco Fn160A)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPedCollisionWithPlayer* Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState);
};
VALIDATE_SIZE(CEventPedCollisionWithPlayer, 0x34);
