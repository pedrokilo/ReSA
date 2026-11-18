#pragma once

#include <TaskSimple.h>

class CVehicle;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarSetPedSlowDraggedOut final : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT;

    static void InjectHooks();

    CTaskSimpleCarSetPedSlowDraggedOut(CVehicle*, eTargetDoor);
    ~CTaskSimpleCarSetPedSlowDraggedOut() = default;

    // 0x64A0B0
    CTask*    Clone() const override { return new CTaskSimpleCarSetPedSlowDraggedOut{ *this }; }
    // 0x648410
    eTaskType GetTaskType() const override { return Type; }
    // 0x648420
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private:
    CVehicle::Ref m_Vehicle;
    eTargetDoor   m_Door;

private: // Wrappers for hooks
    // 0x6483B0
    CTaskSimpleCarSetPedSlowDraggedOut* Constructor(CVehicle* a, eTargetDoor b) {
        this->CTaskSimpleCarSetPedSlowDraggedOut::CTaskSimpleCarSetPedSlowDraggedOut(a, b);
        return this;
    }

    // 0x648430
    CTaskSimpleCarSetPedSlowDraggedOut* Destructor() {
        this->CTaskSimpleCarSetPedSlowDraggedOut::~CTaskSimpleCarSetPedSlowDraggedOut();
        return this;
    }
};
