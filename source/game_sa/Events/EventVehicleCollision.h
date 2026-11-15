#pragma once

#include "Event.h"

#include "Event.h"
#include "TaskSimpleGoTo.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexHitPedWithCar.h"

enum eVehicleEvadeType {
    VEHICLE_EVADE_NONE = 0,
    VEHICLE_EVADE_BY_HITSIDE = 1
};

class NOTSA_EXPORT_VTABLE CEventVehicleCollision : public CEvent {
public:
    int16     m_pieceType;
    int16     m_evadeType;
    float     m_fDamageIntensity;
    CVehicle* m_vehicle;
    CVector   m_impactNormal;
    CVector   m_impactPos;
    int8      m_moveState;
    uint8     m_DirectionToWalkRoundCar;

public:
    static void InjectHooks();

    CEventVehicleCollision(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType);
    ~CEventVehicleCollision() override;

    eEventType GetEventType() const override; // 0x4AC8F0 (lote 201A, sin validar)
    int32 GetEventPriority() const override; // 0x4AC910 (lote 202A, sin validar)
    int32 GetLifeTime() override;            // 0x4AC900 (lote 202A, sin validar)
    CEvent* Clone() const noexcept override { return new CEventVehicleCollision(m_pieceType, m_fDamageIntensity, m_vehicle, m_impactNormal, m_impactPos, m_moveState, VEHICLE_EVADE_NONE); } // 0x4B6BC0
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override; // 0x4AC980 (lote 202A, sin validar)

private:
    CEventVehicleCollision* Constructor(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType);
};
VALIDATE_SIZE(CEventVehicleCollision, 0x34);
