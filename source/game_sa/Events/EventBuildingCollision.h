#pragma once

#include "Event.h"

#include "Event.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventKnockOffBike.h"


class NOTSA_EXPORT_VTABLE CEventBuildingCollision : public CEvent {
public:
    int16      m_pieceType;
    int16      m_moveState;
    float      m_damageIntensity;
    CBuilding* m_building;
    CVector    m_impactNormal;
    CVector    m_impactPos;

public:
    CEventBuildingCollision(int16 pieceType, float damageIntensity, CBuilding* building, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState);
    ~CEventBuildingCollision() override;

    eEventType GetEventType() const override;                          // 0x4ACFB0 (lote 149A, banco Fn149A: definido en el .cpp)
    bool TakesPriorityOver(const CEvent& refEvent) override;           // 0x4AD050
    bool CanBeInterruptedBySameEvent() override;                       // 0x4ACFD0
    int32 GetEventPriority() const override;                           // 0x4ACFE0
    int32 GetLifeTime() override;                                      // 0x4ACFC0
    CEventBuildingCollision* Clone() const noexcept override { return new CEventBuildingCollision(m_pieceType, m_damageIntensity, m_building, m_impactNormal, m_impactPos, m_moveState); }
    bool AffectsPed(CPed* ped) override;

    static bool CanTreatBuildingAsObject(CBuilding* building);
    bool IsHeadOnCollision(CPed* ped);
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventBuildingCollision* Constructor(int16 pieceType, float damageIntensity, CBuilding* building, const CVector& collisionImpactVelocity, const CVector& collisionPos, int16 moveState);

};
VALIDATE_SIZE(CEventBuildingCollision, 0x30);
