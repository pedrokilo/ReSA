#pragma once

#include "Event.h"

#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"


class NOTSA_EXPORT_VTABLE CEventObjectCollision : public CEvent {
public:
    int16    m_pieceType;
    int16    m_moveState;
    float    m_damageIntensity;
    CObject* m_object;
    CVector  m_impactNormal;
    CVector  m_impactPos;

public:
    static void InjectHooks();

    CEventObjectCollision(int16 pieceType, float damageIntensity, CObject* object, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState);
    ~CEventObjectCollision() override;

    eEventType GetEventType() const override;                           // 0x4ACDA0
    bool TakesPriorityOver(const CEvent& refEvent) override;            // 0x4ACEF0
    int32 GetEventPriority() const override;                            // 0x4ACDC0
    int32 GetLifeTime() override;                                       // 0x4ACDB0
    CEventObjectCollision* Clone() const noexcept override { return new CEventObjectCollision(m_pieceType, m_damageIntensity, m_object, m_impactNormal, m_impactPos, m_moveState); }
    bool AffectsPed(CPed* ped) override;
    bool IsHeadOnCollision(CPed* ped);                                  // 0x4ACE80

private:
    CEventObjectCollision* Constructor(int16 pieceType, float damageIntensity, CObject* object, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState);
};
VALIDATE_SIZE(CEventObjectCollision, 0x30);
