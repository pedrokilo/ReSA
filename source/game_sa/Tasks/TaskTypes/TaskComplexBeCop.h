#pragma once

#include "TaskComplexWanderCop.h"

class NOTSA_EXPORT_VTABLE CTaskComplexBeCop : public CTaskComplexWanderCop {
public:
    static constexpr auto Type = TASK_COMPLEX_BE_COP;

    static void InjectHooks();

    // 0x4993D0
    // Como el original: `dir` es un byte (uint8) que se copia tal cual a m_nDir.
    CTaskComplexBeCop(eMoveState moveState, uint8 dir, CTask* task) : CTaskComplexWanderCop(moveState, dir) {
        m_pGoToPointAndStandStillTask = static_cast<CTaskComplexGoToPointAndStandStill*>(task);
    };

    eTaskType GetTaskType() const override { return Type; } // 0x499430

    // 0x499440
    CTask* Clone() const override;

private: // Wrappers para los hooks
    // 0x4994E0
    __declspec(noinline) CTaskComplexBeCop* Destructor() {
        this->CTaskComplexBeCop::~CTaskComplexBeCop();
        return this;
    }
    CTaskComplexBeCop* ScalarDeletingDestructor(uint8 flags); // 0x4994C0 (lote 125A, banco Fn125A): destructor con borrado (ranura 0 de la vtable), `ret 4`
};
VALIDATE_SIZE(CTaskComplexBeCop, 0x50);
