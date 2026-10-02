#pragma once

#include "TaskSimple.h"
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleEvasiveStep : public CTaskSimple {
public:
    CEntity* m_Entity;
    bool     m_bFinished;
    CAnimBlendAssociation *m_Assoc;

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_SIMPLE_EVASIVE_STEP;

    explicit CTaskSimpleEvasiveStep(CEntity* entity);
    ~CTaskSimpleEvasiveStep() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleEvasiveStep(m_Entity); } // 0x655810
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);
    static void FinishAnimEvasiveStepCB(CAnimBlendAssociation *assoc, void* data);

private: // Wrappers for hooks
    // 0x653160
    CTaskSimpleEvasiveStep* Constructor(CEntity* entity) {
        this->CTaskSimpleEvasiveStep::CTaskSimpleEvasiveStep(entity);
        return this;
    }
    // 0x6531D0
    CTaskSimpleEvasiveStep* Destructor() {
        this->CTaskSimpleEvasiveStep::~CTaskSimpleEvasiveStep();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleEvasiveStep, 0x14);
