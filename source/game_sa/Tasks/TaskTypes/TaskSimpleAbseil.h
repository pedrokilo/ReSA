#pragma once
#include "TaskSimpleRunAnim.h"
#include "AnimationEnums.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleAbseil : public CTaskSimpleRunAnim
{
public:
    static void InjectHooks();

    CTaskSimpleAbseil() : CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_ABSEIL, 4.0F, TASK_SIMPLE_ABSEIL, "Abseil", false) {}
    ~CTaskSimpleAbseil() override {}
    CTask* Clone() const override; // 0x6586D0 (lote 180A, banco Fn180A)

    virtual bool IsInterruptable(CPed* ped) { return false; }

private:
    CTaskSimpleAbseil* Constructor(); // 0x6586A0 (lote 180A, banco Fn180A)
};

VALIDATE_SIZE(CTaskSimpleAbseil, 0x20);
