#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CTaskSimpleClearLookAt;

class NOTSA_EXPORT_VTABLE CTaskSimpleClearLookAt : public CTaskSimple {
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CLEAR_LOOK_AT;

    CTaskSimpleClearLookAt() = default;
    ~CTaskSimpleClearLookAt() = default;

    CTask*    Clone() const override { return new CTaskSimpleClearLookAt{}; } // Como el original (0x636F20): tarea nueva, sin copiar el padre
    // 0x634690
    eTaskType GetTaskType() const override { return Type; }
    // 0x6346B0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x634670
    CTaskSimpleClearLookAt* Constructor() {
        this->CTaskSimpleClearLookAt::CTaskSimpleClearLookAt();
        return this;
    }
    // 0x6346A0
    CTaskSimpleClearLookAt* Destructor() {
        this->CTaskSimpleClearLookAt::~CTaskSimpleClearLookAt();
        return this;
    }
};
