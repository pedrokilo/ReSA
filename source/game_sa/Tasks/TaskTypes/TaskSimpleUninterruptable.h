#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleUninterruptable : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_UNINTERRUPTABLE;

    CTaskSimpleUninterruptable() = default;
    ~CTaskSimpleUninterruptable() override = default;

    eTaskType GetTaskType() const override;   // 0x43E380 (lote 158A, banco Fn158A)
    CTask* Clone() const override;            // 0x43E330 (lote 158A)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override; // 0x43E390 (lote 158A)
    bool ProcessPed(CPed* ped) override;      // 0x43E3A0 (lote 158A)

    static void InjectHooks();
    CTaskSimpleUninterruptable* Constructor();
};
VALIDATE_SIZE(CTaskSimpleUninterruptable, 0x8);
