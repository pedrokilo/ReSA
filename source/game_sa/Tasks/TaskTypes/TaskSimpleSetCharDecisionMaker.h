#pragma once

#include "TaskSimple.h"

class CEvent;
class CPed;
class CTaskSimpleSetCharDecisionMaker;

class NOTSA_EXPORT_VTABLE CTaskSimpleSetCharDecisionMaker : public CTaskSimple {

public:
    uint32 m_decisionMaker{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_SET_CHAR_DECISION_MAKER;

    CTaskSimpleSetCharDecisionMaker(uint32 decisionMaker);
    CTaskSimpleSetCharDecisionMaker(const CTaskSimpleSetCharDecisionMaker&); // NOTSA
    ~CTaskSimpleSetCharDecisionMaker() = default;

    CTask*    Clone() const override { return new CTaskSimpleSetCharDecisionMaker{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x46A470
    CTaskSimpleSetCharDecisionMaker* Constructor(uint32 dm) {
        this->CTaskSimpleSetCharDecisionMaker::CTaskSimpleSetCharDecisionMaker(dm);
        return this;
    }
    // 0x46AF10
    __declspec(noinline) CTaskSimpleSetCharDecisionMaker* Destructor() {
        this->CTaskSimpleSetCharDecisionMaker::~CTaskSimpleSetCharDecisionMaker();
        return this;
    }

    // 0x46AEF0 (lote 166A, banco Fn166A): destructor con borrado (ranura 0 de la vtable 0x859DA0), `ret 4`
    CTaskSimpleSetCharDecisionMaker* ScalarDeletingDestructor(uint8 flags);
};
