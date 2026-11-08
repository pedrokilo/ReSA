#pragma once

#include "TaskSimpleCarDrive.h"
#include "AutoPilot.h" // eAutoPilotTempAction
#include <extensions/WEnum.hpp>

class CPed;
class CEvent;
class CVehicle;
class CTaskSimpleCarSetTempAction;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarSetTempAction : public CTaskSimpleCarDrive {
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_SET_TEMP_ACTION;

    CTaskSimpleCarSetTempAction(CVehicle* veh, eAutoPilotTempAction action, uint32 timeMs);
    CTaskSimpleCarSetTempAction(const CTaskSimpleCarSetTempAction&);
    ~CTaskSimpleCarSetTempAction() = default;

    CTask*    Clone() const override { return new CTaskSimpleCarSetTempAction(*this); }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63D6F0
    CTaskSimpleCarSetTempAction* Constructor(CVehicle* veh, eAutoPilotTempAction action, uint32 timeMs) {
        this->CTaskSimpleCarSetTempAction::CTaskSimpleCarSetTempAction(veh, action, timeMs);
        return this;
    }
    // 0x63D730
    __declspec(noinline) CTaskSimpleCarSetTempAction* Destructor() {
        this->CTaskSimpleCarSetTempAction::~CTaskSimpleCarSetTempAction();
        return this;
    }
    CTaskSimpleCarSetTempAction* ScalarDeletingDestructor(uint8 flags); // 0x643760 (lote 127A, banco Fn127A): destructor con borrado (ranura 0 de la vtable 0x86EA6C), `ret 4`

private:
    notsa::WEnumS32<eAutoPilotTempAction> m_TempAct{};
    uint32                                m_DurMs{};
};
