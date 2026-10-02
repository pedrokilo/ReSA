#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleSetStayInSamePlace : public CTaskSimple {
public:
    bool m_bStayInSamePlace;

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_SIMPLE_SET_STAY_IN_SAME_PLACE;

    explicit CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace);
    ~CTaskSimpleSetStayInSamePlace() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x62F5B0
    CTask* Clone() const override { return new CTaskSimpleSetStayInSamePlace(m_bStayInSamePlace); } // 0x635BD0
    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; } // 0x62F5C0
    bool ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x62F590
    CTaskSimpleSetStayInSamePlace* Constructor(bool bStayInSamePlace) {
        this->CTaskSimpleSetStayInSamePlace::CTaskSimpleSetStayInSamePlace(bStayInSamePlace);
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleSetStayInSamePlace, 0xC);
