#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexObserveTrafficLights : public CTaskComplex {
public:
    CTaskTimer m_Timer;

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS;

    CTaskComplexObserveTrafficLights() = default; // 0x631790
    ~CTaskComplexObserveTrafficLights() override = default; // 0x6317C0

    eTaskType GetTaskType() const override { return Type; }    // 0x6317B0
    CTask* Clone() const override { return new CTaskComplexObserveTrafficLights; }            // 0x636410 - Como el original: sin () (no se pone a cero el relleno 0x16..0x17)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }  // 0x6317D0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override { return CreateNextSubTask(ped); }    // 0x6318D0
    CTask* ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x631790
    CTaskComplexObserveTrafficLights* Constructor() {
        this->CTaskComplexObserveTrafficLights::CTaskComplexObserveTrafficLights();
        return this;
    }
    // 0x6317C0
    CTaskComplexObserveTrafficLights* Destructor() {
        this->CTaskComplexObserveTrafficLights::~CTaskComplexObserveTrafficLights();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexObserveTrafficLights, 0x18);
