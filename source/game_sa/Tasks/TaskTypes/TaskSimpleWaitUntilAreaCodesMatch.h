#pragma once

#include "TaskSimpleStandStill.h"

class CEntity;
class CTaskSimpleWaitUntilAreaCodesMatch;
class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleWaitUntilAreaCodesMatch : public CTaskSimpleStandStill {

public:
    CPed* m_pedToWaitFor{};
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_WAIT_FOR_MATCHING_AREA_CODES;

    CTaskSimpleWaitUntilAreaCodesMatch(CPed* pedToWaitFor);
    CTaskSimpleWaitUntilAreaCodesMatch(const CTaskSimpleWaitUntilAreaCodesMatch& o) : CTaskSimpleWaitUntilAreaCodesMatch{o.m_pedToWaitFor} {} // NOTSA
    ~CTaskSimpleWaitUntilAreaCodesMatch();

    // 0x637190
    CTask*    Clone() const override { return new CTaskSimpleWaitUntilAreaCodesMatch{ *this }; }
    // 0x6355C0
    eTaskType GetTaskType() const override { return Type; }
    // 0x6355D0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return priority == ABORT_PRIORITY_IMMEDIATE; }
    bool      ProcessPed(CPed* ped) override;
private: // Wrappers for hooks
    // 0x635540
    CTaskSimpleWaitUntilAreaCodesMatch* Constructor(CPed* pedToWaitFor) {
        this->CTaskSimpleWaitUntilAreaCodesMatch::CTaskSimpleWaitUntilAreaCodesMatch(pedToWaitFor);
        return this;
    }
    // 0x6355E0
    CTaskSimpleWaitUntilAreaCodesMatch* Destructor() {
        this->CTaskSimpleWaitUntilAreaCodesMatch::~CTaskSimpleWaitUntilAreaCodesMatch();
        return this;
    }
};
