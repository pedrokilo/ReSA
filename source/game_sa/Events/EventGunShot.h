#pragma once

#include "EventEditableResponse.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventGunShot : public CEventEditableResponse {
public:
    CEntity* m_firedBy;
    CVector  m_startPoint;
    CVector  m_endPoint;
    bool     m_bHasNoSound;

    static inline auto& ms_fGunShotSenseRangeForRiot2 = StaticRef<float>(0x8A625C);
    static void InjectHooks();

    CEventGunShot(CEntity* entity, CVector startPoint, CVector endPoint, bool bHasNoSound);
    ~CEventGunShot() override;

    /*!
     * @addr 0x4AC6B0
     */
    eEventType GetEventType() const override { return EVENT_SHOT_FIRED; }
    /*!
     * @addr 0x4AC710
     */
    int32 GetEventPriority() const override { return 35; }
    /*!
     * @addr 0x4AC6C0
     */
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    bool IsCriminalEvent() override;
    /*!
     * @addr 0x4AC830
     */
    void ReportCriminalEvent(CPed* ped) override { } // empty
    /*!
     * @addr 0x4AC6D0
     */
    CEntity* GetSourceEntity() const override { return m_firedBy; }
    bool TakesPriorityOver(const CEvent& refEvent) override;
    /*!
     * @addr 0x4AC6E0
     */
    float GetLocalSoundLevel() override { return m_bHasNoSound ? 0.0f : 160.0f; }
    /*!
     * @addr 0x4AC700
     */
    bool CanBeInterruptedBySameEvent() override { return true; }
    CEventEditableResponse* CloneEditable() const noexcept override;

    CEventGunShot* Constructor(CEntity* entity, CVector startPoint, CVector endPoint, bool bHasNoSound);
};

VALIDATE_SIZE(CEventGunShot, 0x34);
