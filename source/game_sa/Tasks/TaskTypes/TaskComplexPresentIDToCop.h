#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE; no se registra en InjectHooksMain ni figura en hooks.csv.
class NOTSA_EXPORT_VTABLE CTaskComplexPresentIDToCop : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_PRESENT_ID_TO_COP;

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    eTaskType GetTaskType() const override { return Type; } // 0x659230
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // CTask* CreateNextSubTask(CPed* ped) override;
    // CTask* CreateFirstSubTask(CPed* ped) override;
    // CTask* ControlSubTask(CPed* ped) override;
};
