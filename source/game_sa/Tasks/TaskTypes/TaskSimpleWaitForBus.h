#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CVehicle;
class CTaskSimpleWaitForBus;

class NOTSA_EXPORT_VTABLE CTaskSimpleWaitForBus : public CTaskSimple {

public:
    CVehicle* m_bus{}; // TODO/NOTE: This entity never has it's reference registered

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_WAIT_FOR_BUS;

    CTaskSimpleWaitForBus() = default;
    CTaskSimpleWaitForBus(const CTaskSimpleWaitForBus&);
    ~CTaskSimpleWaitForBus() = default;

    // 0x636AD0
    CTask*    Clone() const override { return new CTaskSimpleWaitForBus{*this}; }
    // 0x632A70
    eTaskType GetTaskType() const override { return Type; }
    // 0x632A80
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x632A50
    CTaskSimpleWaitForBus* Constructor() {
        this->CTaskSimpleWaitForBus::CTaskSimpleWaitForBus();
        return this;
    }
    // 0x632A90
    CTaskSimpleWaitForBus* Destructor() {
        this->CTaskSimpleWaitForBus::~CTaskSimpleWaitForBus();
        return this;
    }
};
