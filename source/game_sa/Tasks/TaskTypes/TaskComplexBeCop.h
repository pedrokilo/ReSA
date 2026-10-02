#pragma once

#include "TaskComplexWanderCop.h"

class NOTSA_EXPORT_VTABLE CTaskComplexBeCop : public CTaskComplexWanderCop {
public:
    static constexpr auto Type = TASK_COMPLEX_BE_COP;

    static void InjectHooks();

    // 0x4993D0
    // NOTA: en el original `dir` es un byte (uint8) que se copia a m_nDir; aquí es `bool`,
    // así que una dirección distinta de 0/1 se trunca a 1. No se instala.
    CTaskComplexBeCop(eMoveState moveState, bool dir, CTask* task) : CTaskComplexWanderCop(moveState, dir) {
        m_pGoToPointAndStandStillTask = static_cast<CTaskComplexGoToPointAndStandStill*>(task);
    };

    eTaskType GetTaskType() const override { return Type; } // 0x499430

    // 0x499440
    // NOTA: el original solo clona m_pGoToPointAndStandStillTask si no es nulo (si lo es, pasa nullptr);
    // esta versión lo desreferencia sin comprobar y además pasa m_nDir por el `bool` del constructor.
    CTask* Clone() const override { return new CTaskComplexBeCop(m_nMoveState, m_nDir, m_pGoToPointAndStandStillTask->Clone()); }

private: // Wrappers para los hooks
    // 0x4994E0
    CTaskComplexBeCop* Destructor() {
        this->CTaskComplexBeCop::~CTaskComplexBeCop();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexBeCop, 0x50);
