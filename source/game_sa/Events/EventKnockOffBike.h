#pragma once

#include "Event.h"
#include "Vector.h"

enum eKnockOffType : uint8 {
    KNOCK_OFF_TYPE_NONE = 0,
    KNOCK_OFF_TYPE_NONE_1 = 55, // same as KNOCK_OFF_TYPE_NONE = 0
    KNOCK_OFF_TYPE_FALL = 53,
    KNOCK_OFF_TYPE_SKIDBACK_FALLR = 54,
    KNOCK_OFF_TYPE_SKIDBACKFRONT = 49,
    KNOCK_OFF_TYPE_EXPLOSION = 51,
};

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventKnockOffBike : public CEvent {
public:
    CVector   m_moveSpeed;
    CVector   m_collisionImpactVelocity;
    float     m_damageIntensity;
    float     field_28;
    uint32    m_time;
    CPed*     m_ped;
    uint8     m_knockOffDirection;
    uint8     m_isVictimDriver : 1;
    uint8     m_forceKnockOff : 1; // If set, then it ignores CPed::CantBeKnockedOffBike flag
    uint8     m_knockOffType;
    uint8     m_exitDoor;
    CVehicle* m_vehicle;

public:
    CEventKnockOffBike(CVehicle* vehicle, const CVector& moveSpeed, const CVector& collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOutDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff);
    CEventKnockOffBike();
    ~CEventKnockOffBike() override;

    eEventType GetEventType() const override;                  // 0x4AFCB0 (lote 181A, sin validar)
    float GetLocalSoundLevel() override;                       // 0x4AFCD0 (lote 181A, sin validar)
    int32 GetEventPriority() const override;                   // 0x4AFCE0 (lote 181A, sin validar)
    int32 GetLifeTime() override;                              // 0x4AFCC0 (lote 181A, sin validar)
    CEvent* Clone() const noexcept override;                   // 0x4B7590 (lote 187A, sin validar)
    bool AffectsPed(CPed* ped) override;
    bool IsCriminalEvent() override;                           // 0x4B06F0 (lote 181A, sin validar)
    void ReportCriminalEvent(CPed* ped) override;

public:
    void operator=(const CEventKnockOffBike& right) { From(right); } // 0x4B4AA0
    void From(const CEventKnockOffBike& right);
    void SetPedOutCar(CPed* ped);
    int32 CalcForcesAndAnims(CPed* ped);
    bool SetPedSafePosition(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventKnockOffBike* Constructor(CVehicle* vehicle, CVector* moveSpeed, CVector* collisionImpactVelocity, float damageIntensity, float a6, int32 knockOffType, uint8 knockOutDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff); // lote 187A: el tipo es int32 (solo cuenta el BYTE bajo): Clone lo empuja extendido con signo, como el movsx del EXE
    CEventKnockOffBike* Constructor();

};

VALIDATE_SIZE(CEventKnockOffBike, 0x3C);
