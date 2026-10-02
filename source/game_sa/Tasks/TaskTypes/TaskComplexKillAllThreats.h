#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE; no se registra en InjectHooksMain ni figura en hooks.csv.
class NOTSA_EXPORT_VTABLE CTaskComplexKillAllThreats : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_KILL_ALL_THREATS;

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    eTaskType GetTaskType() const override { return Type; } // 0x6221B0
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // CTask* CreateNextSubTask(CPed* ped) override;
    // CTask* CreateFirstSubTask(CPed* ped) override;
    // CTask* ControlSubTask(CPed* ped) override;
};
