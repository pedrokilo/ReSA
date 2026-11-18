#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleHitFromRight;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitFromRight : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_RIGHT;

    CTaskSimpleHitFromRight();
    CTaskSimpleHitFromRight(const CTaskSimpleHitFromRight&);

    // 0x62EDF0
    virtual CTask* Clone() const { return new CTaskSimpleHitFromRight{ *this }; }
    // 0x62EE60
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x62EDC0
    CTaskSimpleHitFromRight* Constructor() {
        this->CTaskSimpleHitFromRight::CTaskSimpleHitFromRight();
        return this;
    }
};
