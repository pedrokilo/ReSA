#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventCarUpsideDown : public CEvent {
public:
    CVehicle* m_vehicle;

public:
    explicit CEventCarUpsideDown(CVehicle* vehicle);
    ~CEventCarUpsideDown() override;

    /*!
     * @addr 0x4B1D20
     */
    eEventType GetEventType() const override { return EVENT_CAR_UPSIDE_DOWN; }
    /*!
     * @addr 0x4B1D40
     */
    int32 GetEventPriority() const override { return 18; }
    /*!
     * @addr 0x4B1D30
     */
    int32 GetLifeTime() override { return 0; }
    CEventCarUpsideDown* Clone() const noexcept override; // 0x4B79D0 (lote 201A, sin validar)
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventCarUpsideDown* Constructor(CVehicle* vehicle);
    CEventCarUpsideDown* Destructor();

};
VALIDATE_SIZE(CEventCarUpsideDown, 0x10);
