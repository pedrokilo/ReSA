#pragma once

#include "TaskSimpleStandStill.h"

class CPed;
class CEvent;

// Tarea simple de peaton sobre una escalera mecanica (vtable 0x85B8DC del EXE). Es un CTaskSimpleStandStill (misma talla 0x20, sin campos propios) con otra vtable: el EXE la construye con
// CTaskSimpleStandStill{0, true, false, 8.0f} y le cambia la vtable (CEventHandler::ComputeOnEscalatorResponse, 0x4BC150).
class NOTSA_EXPORT_VTABLE CTaskSimpleOnEscalator : public CTaskSimpleStandStill {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_ON_ESCALATOR;

    static void InjectHooks();

    CTaskSimpleOnEscalator() : CTaskSimpleStandStill{0, true, false, 8.0f} {}

    eTaskType GetTaskType() const override; // 0x4B8720 (lote 171A, banco Fn171A)
    CTask* Clone() const override;          // 0x4B86C0 (lote 171A)
    bool ProcessPed(CPed* ped) override;    // 0x62F520 (lote 171A)
};
VALIDATE_SIZE(CTaskSimpleOnEscalator, 0x20);
