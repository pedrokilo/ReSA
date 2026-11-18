#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitWall : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    constexpr static auto Type = TASK_SIMPLE_HIT_WALL;

    CTaskSimpleHitWall();
    CTaskSimpleHitWall(const CTaskSimpleHitWall& o);

    // 0x652FC0
    CTask* Clone() const override { return new CTaskSimpleHitWall{ *this }; }
    // 0x653030
    virtual bool IsInterruptable(CPed const* ped) { return false; }
private: // Wrappers for hooks
    // 0x652F90
    CTaskSimpleHitWall* Constructor() {
        this->CTaskSimpleHitWall::CTaskSimpleHitWall();
        return this;
    }
};
