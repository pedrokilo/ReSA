#pragma once

#include "TaskComplex.h"

class CPedAttractor;
class CTaskComplexWaitAtAttractor;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitAtAttractor : public CTaskComplex {

public:
    CPedAttractor* m_attractor{};
    int32 m_slot{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_AT_ATTRACTOR;

    CTaskComplexWaitAtAttractor(CPedAttractor* attractor, int32 slot);
    CTaskComplexWaitAtAttractor(const CTaskComplexWaitAtAttractor&);
    ~CTaskComplexWaitAtAttractor() = default;

    // 0x636940
    CTask*    Clone() const override { return new CTaskComplexWaitAtAttractor{ *this }; }
    // 0x632620
    eTaskType GetTaskType() const override { return Type; }
    // 0x632640
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    // 0x6326E0
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x6325F0
    CTaskComplexWaitAtAttractor* Constructor(CPedAttractor* attractor, int32 slot) {
        this->CTaskComplexWaitAtAttractor::CTaskComplexWaitAtAttractor(attractor, slot);
        return this;
    }
    // 0x632630
    __declspec(noinline) CTaskComplexWaitAtAttractor* Destructor() {
        this->CTaskComplexWaitAtAttractor::~CTaskComplexWaitAtAttractor();
        return this;
    }
    // 0x638590 (lote 198A, sin validar): destructor con borrado (ranura 0 de la vtable 0x86E108), `ret 4`
    CTaskComplexWaitAtAttractor* ScalarDeletingDestructor(uint8 flags);
};
