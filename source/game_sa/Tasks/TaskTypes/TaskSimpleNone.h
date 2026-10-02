#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleNone : public CTaskSimple {
public:
    static constexpr auto Type = TASK_NONE;

    static void InjectHooks();

    CTaskSimpleNone() = default;
    ~CTaskSimpleNone() override = default; // 0x463560

    eTaskType GetTaskType() const override { return Type; } // 0x4635C0
    CTask* Clone() const override { return new CTaskSimpleNone(); } // 0x463570
    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; } // 0x4635D0
    bool ProcessPed(CPed* ped) override { return true; } // 0x4635E0

private: // Wrappers para los hooks
    // 0x463560
    CTaskSimpleNone* Destructor() {
        this->CTaskSimpleNone::~CTaskSimpleNone();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleNone, 0x8);
