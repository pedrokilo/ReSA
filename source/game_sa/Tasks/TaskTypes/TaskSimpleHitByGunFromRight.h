#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;
class CTaskSimpleHitByGunFromRight;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitByGunFromRight : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_BY_GUN_RIGHT;

    CTaskSimpleHitByGunFromRight();
    CTaskSimpleHitByGunFromRight(const CTaskSimpleHitByGunFromRight&);

    // 0x62F0B0
    virtual CTask* Clone() const { return new CTaskSimpleHitByGunFromRight{ *this }; }
    // 0x62F120
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x62F080
    CTaskSimpleHitByGunFromRight* Constructor() {
        this->CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight();
        return this;
    }
};
