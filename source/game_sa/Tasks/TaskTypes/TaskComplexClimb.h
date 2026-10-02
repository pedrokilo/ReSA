#pragma once

#include "TaskComplexJump.h"

class NOTSA_EXPORT_VTABLE CTaskComplexClimb : public CTaskComplexJump {
public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_CLIMB;

    CTaskComplexClimb() : CTaskComplexJump(CTaskComplexJump::eForceClimb::FORCE) {}; // 0x46A630
    ~CTaskComplexClimb() override = default; // 0x46A6E0

    // Direcciones verificadas en el vtable 0x859DCC (antes estaban desplazadas: 0x46A6B0 es GetTaskType y 0x46A6C0 el destructor deleting)
    CTask* Clone() const override { return new CTaskComplexClimb(); } // 0x46A650
    eTaskType GetTaskType() const override { return Type; } // 0x46A6B0

private: // Wrappers for hooks
    // 0x46A630
    CTaskComplexClimb* Constructor() {
        this->CTaskComplexClimb::CTaskComplexClimb();
        return this;
    }
    // 0x46A6E0
    CTaskComplexClimb* Destructor() {
        this->CTaskComplexClimb::~CTaskComplexClimb();
        return this;
    }
};
