#pragma once

#include "TaskComplex.h"

class CPed;
class CPedAttractor;
class CTaskComplexUseAttractor;

class NOTSA_EXPORT_VTABLE CTaskComplexUseAttractor : public CTaskComplex {

public:
    CPedAttractor* m_attractor; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_ATTRACTOR;

    CTaskComplexUseAttractor(CPedAttractor*);
    CTaskComplexUseAttractor(const CTaskComplexUseAttractor&);
    ~CTaskComplexUseAttractor() = default;

    // 0x6369B0
    CTask*    Clone() const override { return new CTaskComplexUseAttractor{ *this }; }
    // 0x632710
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6326F0
    auto Constructor(CPedAttractor* attractor) {
        this->CTaskComplexUseAttractor::CTaskComplexUseAttractor(attractor);
        return this;
    }

    // 0x632720
    __declspec(noinline) auto Destructor() {
        this->CTaskComplexUseAttractor::~CTaskComplexUseAttractor();
        return this;
    }
    // 0x6385B0 (lote 198A, sin validar): destructor con borrado (ranura 0 de la vtable 0x86E134), `ret 4`
    CTaskComplexUseAttractor* ScalarDeletingDestructor(uint8 flags);
};
