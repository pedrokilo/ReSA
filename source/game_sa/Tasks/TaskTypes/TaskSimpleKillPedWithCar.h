#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleKillPedWithCar final : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_KILL_PED_WITH_CAR;

    static void InjectHooks();

    CTaskSimpleKillPedWithCar(CVehicle* car, float impulseMag);
    CTaskSimpleKillPedWithCar(const CTaskSimpleKillPedWithCar&);
    ~CTaskSimpleKillPedWithCar() override = default;

    void DamageCarBonnet(CPed const*);

    CTask*    Clone() const override { return new CTaskSimpleKillPedWithCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    CTaskSimpleKillPedWithCar* ScalarDeletingDestructor(uint8 flags); // 0x6563B0 (lote 176A, sin validar): destructor con borrado (ranura 0 de la vtable 0x86F2C0), `ret 4`
    void ShakePadIfPlayerVehicle();                                   // 0x6563D0 (lote 176A, sin validar): sacude el mando si el coche es el del jugador

    // 0x653C10
    CTaskSimpleKillPedWithCar* Constructor(CVehicle* a, float b) {
        this->CTaskSimpleKillPedWithCar::CTaskSimpleKillPedWithCar(a, b);
        return this;
    }

    // 0x653C90
    CTaskSimpleKillPedWithCar* Destructor() {
        this->CTaskSimpleKillPedWithCar::~CTaskSimpleKillPedWithCar();
        return this;
    }

private:
    CVehicle::Ref m_Car{};
    float         m_ImpulseMag{};
};
