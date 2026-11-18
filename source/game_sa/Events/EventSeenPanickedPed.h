#pragma once

#include "EventEditableResponse.h"


class NOTSA_EXPORT_VTABLE CEventSeenPanickedPed : public CEventEditableResponse {
public:
    CPed* m_ped;

public:
    explicit CEventSeenPanickedPed(CPed* ped);
    ~CEventSeenPanickedPed() override;

    /*!
     * @addr 0x4B20F0
     */
    eEventType GetEventType() const override { return EVENT_SEEN_PANICKED_PED; }
    /*!
     * @addr 0x4B2120
     */
    int32 GetEventPriority() const override { return 13; }
    /*!
     * @addr 0x4B2100
     */
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    /*!
     * @addr 0x4B2110
     */
    CEntity* GetSourceEntity() const override { return m_ped;}
    CEventSeenPanickedPed* CloneEditable() const noexcept override; // 0x4B7B30 (lote 201A, sin validar)

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CEventSeenPanickedPed* Constructor(CPed* ped);
};
VALIDATE_SIZE(CEventSeenPanickedPed, 0x18);
