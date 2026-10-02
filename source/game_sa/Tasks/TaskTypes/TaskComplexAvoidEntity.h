#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE; no se registra en InjectHooksMain ni figura en hooks.csv.
// En el original el vtable no se referencia desde ningún sitio (tarea sin uso).
class NOTSA_EXPORT_VTABLE CTaskComplexAvoidEntity : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_AVOID_ENTITY;

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    eTaskType GetTaskType() const override { return Type; } // 0x66AAD0
    // CTask* Clone() const override;                                                      // 0x66D0C0
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override; // 0x66AD40
    // CTask* CreateNextSubTask(CPed* ped) override;                                       // 0x66AD70
    // CTask* CreateFirstSubTask(CPed* ped) override;                                      // 0x672530
    // CTask* ControlSubTask(CPed* ped) override;                                          // 0x66ADE0
};
