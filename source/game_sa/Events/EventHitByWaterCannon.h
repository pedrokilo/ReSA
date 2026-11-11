#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventHitByWaterCannon : public CEvent {
public:
    CVector m_point;
    CVector m_moveSpeed;

public:
    CEventHitByWaterCannon(const CVector& point, const CVector& moveSpeed);
    ~CEventHitByWaterCannon() override = default;

    eEventType GetEventType() const override;   // 0x4B12E0 (lote 166A, banco Fn166A)
    int32 GetEventPriority() const override;    // 0x4B1310 (lote 166A)
    int32 GetLifeTime() override;               // 0x4B12F0 (lote 166A)
    CEvent* Clone() const noexcept override { return new CEventHitByWaterCannon(m_point, m_moveSpeed); }
    bool AffectsPed(CPed* ped) override;
    float GetLocalSoundLevel() override;        // 0x4B1300 (lote 166A): `fld dword [0x85ADBC]` (55.0f); `ret`

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventHitByWaterCannon* Constructor(const CVector& point, const CVector& moveSpeed);

};
VALIDATE_SIZE(CEventHitByWaterCannon, 0x24);
