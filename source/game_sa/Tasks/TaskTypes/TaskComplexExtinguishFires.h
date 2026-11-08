#pragma once

#include "TaskComplex.h"

class CTaskComplexExtinguishFires;
class CFire;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexExtinguishFires : public CTaskComplex {

public:
    CFire* m_fireToExt{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_EXTINGUISH_FIRES;

    CTaskComplexExtinguishFires() = default;
    CTaskComplexExtinguishFires(const CTaskComplexExtinguishFires&);
    ~CTaskComplexExtinguishFires() = default;

    CFire* GetNearestFire(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexExtinguishFires{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
private: // Wrappers for hooks
    // 0x6975C0
    CTaskComplexExtinguishFires* Constructor() {
        this->CTaskComplexExtinguishFires::CTaskComplexExtinguishFires();
        return this;
    }
    // 0x6975F0
    __declspec(noinline) CTaskComplexExtinguishFires* Destructor() {
        this->CTaskComplexExtinguishFires::~CTaskComplexExtinguishFires();
        return this;
    }
    CTaskComplexExtinguishFires* ScalarDeletingDestructor(uint8 flags); // 0x6980F0 (lote 127A, banco Fn127A): destructor con borrado (ranura 0 de la vtable 0x870DD0), `ret 4`
};
