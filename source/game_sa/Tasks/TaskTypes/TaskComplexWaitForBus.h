#pragma once

#include "TaskComplex.h"

class CPed;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForBus : public CTaskComplex {
public:
    CVehicle* m_bus = {}; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_FOR_BUS;

    CTaskComplexWaitForBus() = default;
    CTaskComplexWaitForBus(const CTaskComplexWaitForBus&);
    ~CTaskComplexWaitForBus() = default;

    CTask*    Clone() const override;
    eTaskType GetTaskType() const override;
    // Como el original: el VMT 0x86E18C usa en el slot 6 el MakeAbortable base de CTaskComplex (0x4211B0),
    // que delega en la subtarea; no hay sobrescritura que devuelva siempre true.
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x632900
    CTaskComplexWaitForBus* Constructor() {
        this->CTaskComplexWaitForBus::CTaskComplexWaitForBus();
        return this;
    }

    // 0x632930
    CTaskComplexWaitForBus* Destructor() {
        this->CTaskComplexWaitForBus::~CTaskComplexWaitForBus();
        return this;
    }
};
