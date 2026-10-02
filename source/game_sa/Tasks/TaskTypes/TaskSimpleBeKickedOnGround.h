#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE; no se registra en InjectHooksMain ni figura en hooks.csv.
class NOTSA_EXPORT_VTABLE CTaskSimpleBeKickedOnGround : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_BE_KICKED_ON_GROUND;

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    eTaskType GetTaskType() const override { return Type; } // 0x61FC40
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // bool ProcessPed(CPed* ped) override;
};
