#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexHitPedWithCar final : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_HIT_PED_WITH_CAR;

    static void InjectHooks();

    CTaskComplexHitPedWithCar(CVehicle* veh, float impulseMagnitude);
    CTaskComplexHitPedWithCar(const CTaskComplexHitPedWithCar&);
    ~CTaskComplexHitPedWithCar();

    static CVector ComputeEvasiveStepMoveDir(const CPed* ped, CVehicle* veh);

    bool HitKillsPed(CPed* ped) const; // 0x653AA0 (lote 176A, sin validar)
    bool HitHurtsPed(CPed* ped);
    CTask* CreateSubTask(eTaskType);

    CTask*    Clone() const override { return new CTaskComplexHitPedWithCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6539A0
    CTaskComplexHitPedWithCar* Constructor(CVehicle* vehicle, float damageIntensity) {
        this->CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(vehicle, damageIntensity);
        return this;
    }

    // 0x653A30
    __declspec(noinline) CTaskComplexHitPedWithCar* Destructor() {
        this->CTaskComplexHitPedWithCar::~CTaskComplexHitPedWithCar();
        return this;
    }
    CTaskComplexHitPedWithCar* ScalarDeletingDestructor(uint8 flags); // 0x6560C0 (lote 126A, banco Fn126A): destructor con borrado (ranura 0 de la vtable 0x86F294), `ret 4`

public:
    CVehicle* m_Veh{};
    float     m_ImpulseMag{};
    int32     m_PedHitSide; // Como el original: se escribe en CreateFirstSubTask.
    int32     m_DownTime{1000};
    CVector   m_MoveDir; // Como el original: se escribe solamente para el paso evasivo.
};            
