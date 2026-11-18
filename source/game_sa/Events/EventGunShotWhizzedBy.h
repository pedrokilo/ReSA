#pragma once

#include "EventGunShot.h"

#include "EventGunShot.h"

class NOTSA_EXPORT_VTABLE CEventGunShotWhizzedBy : public CEventGunShot {
public:
    constexpr static auto Type = eEventType::EVENT_SHOT_FIRED_WHIZZED_BY;

    static void InjectHooks();

    CEventGunShotWhizzedBy(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound);
    ~CEventGunShotWhizzedBy() override = default;

    /*!
     * @addr 0x4B2030
     */
    eEventType GetEventType() const override { return Type; }
    /*!
     * @addr 0x4B2060
     */
    int32 GetEventPriority() const override { return 36; }
    /*!
     * @addr 0x4B2040
     */
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    /*!
     * @addr 0x4B2050
     */
    bool CanBeInterruptedBySameEvent() override { return true; }
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x4B7AC0 (lote 201A, sin validar)

private:
    CEventGunShotWhizzedBy* Constructor(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound);
};
VALIDATE_SIZE(CEventGunShotWhizzedBy, 0x34);
