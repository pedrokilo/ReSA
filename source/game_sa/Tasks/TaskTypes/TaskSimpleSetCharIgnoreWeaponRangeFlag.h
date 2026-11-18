#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleSetCharIgnoreWeaponRangeFlag final : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_SET_CHAR_IGNORE_WEAPON_RANGE_FLAG;

    static void InjectHooks();

    CTaskSimpleSetCharIgnoreWeaponRangeFlag(bool bIgnoreWeaponRange);
    CTaskSimpleSetCharIgnoreWeaponRangeFlag(const CTaskSimpleSetCharIgnoreWeaponRangeFlag&);
    ~CTaskSimpleSetCharIgnoreWeaponRangeFlag() override = default;

    // 0x474640
    CTask*    Clone() const override { return new CTaskSimpleSetCharIgnoreWeaponRangeFlag{ *this }; }
    // 0x4746A0
    eTaskType GetTaskType() const override { return Type; }
    // 0x4746B0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return false; }
    // 0x622B50
    bool      ProcessPed(CPed* ped) override { ped->bIgnoreWeaponRange = m_bIgnoreWeaponRange; return true; }

private: // Wrappers for hooks
    // 0x474620
    CTaskSimpleSetCharIgnoreWeaponRangeFlag* Constructor(bool a) {
        this->CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(a);
        return this;
    }

    // 0x4748F0
    CTaskSimpleSetCharIgnoreWeaponRangeFlag* Destructor() {
        this->CTaskSimpleSetCharIgnoreWeaponRangeFlag::~CTaskSimpleSetCharIgnoreWeaponRangeFlag();
        return this;
    }

protected:
    bool m_bIgnoreWeaponRange{};
};
