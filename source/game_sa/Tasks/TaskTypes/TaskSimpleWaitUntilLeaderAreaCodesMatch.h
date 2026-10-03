#pragma once

#include "TaskSimpleWaitUntilAreaCodesMatch.h"
#include "Vector.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleWaitUntilLeaderAreaCodesMatch final : public CTaskSimpleWaitUntilAreaCodesMatch {
public:
    bool    m_bGrabbedStartPos{};
    CVector m_vStartPos; // Como el original: ni el constructor ni Clone escriben +0x28
    bool    m_b{};

public:
    static constexpr auto Type = eTaskType::TASK_WAIT_FOR_MATCHING_LEADER_AREA_CODES;

    static void InjectHooks();

    using CTaskSimpleWaitUntilAreaCodesMatch::CTaskSimpleWaitUntilAreaCodesMatch;
    // Como el original (0x5F67F0): solo copia el peatón; +0x24 y +0x34 vuelven a cero y +0x28 no se escribe
    CTaskSimpleWaitUntilLeaderAreaCodesMatch(const CTaskSimpleWaitUntilLeaderAreaCodesMatch& o) : CTaskSimpleWaitUntilAreaCodesMatch{o.m_pedToWaitFor} {}

    CTask*    Clone() const override { return new CTaskSimpleWaitUntilLeaderAreaCodesMatch{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    bool      ProcessPed(CPed* ped) override;
};

VALIDATE_SIZE(CTaskSimpleWaitUntilLeaderAreaCodesMatch, 0x38);
