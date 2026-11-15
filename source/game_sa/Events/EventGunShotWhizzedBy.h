#pragma once

#include "EventGunShot.h"

#include "EventGunShot.h"

class NOTSA_EXPORT_VTABLE CEventGunShotWhizzedBy : public CEventGunShot {
public:
    constexpr static auto Type = eEventType::EVENT_SHOT_FIRED_WHIZZED_BY;

    static void InjectHooks();

    CEventGunShotWhizzedBy(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound);
    ~CEventGunShotWhizzedBy() override = default;

    eEventType GetEventType() const override { return Type; }
    int32 GetEventPriority() const override { return 36; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    bool CanBeInterruptedBySameEvent() override { return true; }
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x4B7AC0 (lote 201A, sin validar)

private:
    CEventGunShotWhizzedBy* Constructor(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound);
};
VALIDATE_SIZE(CEventGunShotWhizzedBy, 0x34);
