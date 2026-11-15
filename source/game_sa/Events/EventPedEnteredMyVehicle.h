#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventPedEnteredMyVehicle : public CEventEditableResponse {
public:
    CEventPedEnteredMyVehicle(/*const*/ CPed* ped, /*const*/ CVehicle* vehicle, eTargetDoor targetDoor); // todo: make const as OG
    ~CEventPedEnteredMyVehicle() override;

    eEventType GetEventType() const override { return EVENT_PED_ENTERED_MY_VEHICLE; }
    int32 GetEventPriority() const override { return 39; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override; // 0x4AEC00 (lote 199A, sin validar)
    CEntity* GetSourceEntity() const override { return m_PedThatEntered; }
    float GetLocalSoundLevel() override { return 60.0f; }
    CEventEditableResponse* CloneEditable() const noexcept override; // 0x4B7170 (lote 199A, sin validar)

public:
    CPed*       m_PedThatEntered;
    CVehicle*   m_Vehicle;
    eTargetDoor m_TargetDoor;
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPedEnteredMyVehicle* Constructor(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor);
};
VALIDATE_SIZE(CEventPedEnteredMyVehicle, 0x20);
