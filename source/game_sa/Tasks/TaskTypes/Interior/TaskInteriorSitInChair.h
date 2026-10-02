#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE; no se registra en InjectHooksMain ni figura en hooks.csv.
class NOTSA_EXPORT_VTABLE CTaskInteriorSitInChair : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_SIT_ON_CHAIR;

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    eTaskType GetTaskType() const override { return Type; } // 0x675C80
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // bool ProcessPed(CPed* ped) override;
};
