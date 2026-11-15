#pragma once

#include "EventEditableResponse.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventVehicleOnFire : public CEventEditableResponse {
public:
    CVehicle* m_vehicle;

public:
    static void InjectHooks();

    CEventVehicleOnFire(CVehicle* vehicle);
    ~CEventVehicleOnFire() override;

    eEventType GetEventType() const override; // 0x4B1130 (lote 201A, sin validar)
    int32 GetEventPriority() const override; // 0x4B1160 (lote 201A, sin validar)
    int32 GetLifeTime() override; // 0x4B1140 (lote 201A, sin validar)
    bool AffectsPed(CPed* ped) override;
    float GetLocalSoundLevel() override; // 0x4B1150 (lote 201A, sin validar)
    CEventEditableResponse* CloneEditable() const noexcept override;

private:
    CEventVehicleOnFire* Constructor(CVehicle* vehicle);
};

VALIDATE_SIZE(CEventVehicleOnFire, 0x18);
