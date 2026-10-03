#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleUseAtm : public CTaskSimpleRunAnim {

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_USE_ATM; // Como el original: 0x1A4 (push 1A4h en 0x48DFE8)

    CTaskSimpleUseAtm();
    CTaskSimpleUseAtm(const CTaskSimpleUseAtm&); // NOTSA
    ~CTaskSimpleUseAtm() = default;

    CTask* Clone() const override { return new CTaskSimpleUseAtm{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x48DFE0
    CTaskSimpleUseAtm* Constructor() {
        this->CTaskSimpleUseAtm::CTaskSimpleUseAtm();
        return this;
    }
    // 0x48E960
    CTaskSimpleUseAtm* Destructor() {
        this->CTaskSimpleUseAtm::~CTaskSimpleUseAtm();
        return this;
    }
};
