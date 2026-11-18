#pragma once

#include "EventEditableResponse.h"
#include "Vector.h"
#include "Enums/eMoveState.h"

class CPed;

class NOTSA_EXPORT_VTABLE CEventPotentialWalkIntoPed : public CEventEditableResponse {
public:
    CVector    m_targetPoint;
    CPed*      m_ped;
    eMoveState m_moveState;

public:
    CEventPotentialWalkIntoPed(CPed* ped, const CVector& targetPoint, eMoveState moveState);
    ~CEventPotentialWalkIntoPed() override;

    /*!
     * @addr 0x4AE760
     */
    eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }
    /*!
     * @addr 0x4AE790
     */
    int32 GetEventPriority() const override { return 28; }
    /*!
     * @addr 0x4AE770
     */
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    /*!
     * @addr 0x4AE780
     */
    CEntity* GetSourceEntity() const override { return m_ped; }
    bool TakesPriorityOver(const CEvent& refEvent) override;
    CEventPotentialWalkIntoPed* CloneEditable() const noexcept override; // 0x4B7090 (lote 202A, sin validar)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    /*!
     * @addr 0x4AE6E0
     */
    CEventPotentialWalkIntoPed* Constructor(CPed* ped, const CVector& targetPoint, eMoveState moveState) {
        this->CEventPotentialWalkIntoPed::CEventPotentialWalkIntoPed(ped, targetPoint, moveState);
        return this;
    }

};
VALIDATE_SIZE(CEventPotentialWalkIntoPed, 0x28);
