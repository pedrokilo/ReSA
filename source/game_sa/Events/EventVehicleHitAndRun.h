#pragma once

#include "Event.h"
#include "Ped.h"
#include "Vehicle.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CEventVehicleHitAndRun : public CEvent {
public:
    CPed*     m_victim;
    CVehicle* m_vehicle;

public:
    CEventVehicleHitAndRun(CPed* victim, CVehicle* vehicle);
    ~CEventVehicleHitAndRun() override;

    eEventType GetEventType() const override;                         // 0x4AEA00 (lote 199A, sin validar)
    int32 GetEventPriority() const override;                          // 0x4AEA40 (lote 199A, sin validar)
    int32 GetLifeTime() override;                                     // 0x4AEA10 (lote 199A, sin validar)
    /*!
     * @addr 0x4B7100
     */
    CEvent* Clone() const noexcept override { return new CEventVehicleHitAndRun(m_victim, m_vehicle); }
    bool AffectsPed(CPed* ped) override;                              // 0x4AEA20 (lote 199A, sin validar)
    /*!
     * @addr 0x4B27B0
     */
    bool IsCriminalEvent() override { return m_vehicle->m_pDriver && m_vehicle->m_pDriver->IsPlayer(); }
    void ReportCriminalEvent(CPed* ped) override;
    float GetLocalSoundLevel() override;                              // 0x4AEA30 (lote 199A, sin validar)

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleHitAndRun* Constructor(CPed* victim, CVehicle* vehicle);

};

VALIDATE_SIZE(CEventVehicleHitAndRun, 0x14);
