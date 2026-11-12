#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleDrownInCar : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_DROWN_IN_CAR;

    CTaskSimpleDrownInCar() = default;  // 0x62FF20
    ~CTaskSimpleDrownInCar() override = default; // 0x62FF60

    eTaskType GetTaskType() const override; // 0x62FF40 (lote 177A, sin validar)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override; // 0x62FF50 (lote 177A, sin validar)
    CTask* Clone() const override; // 0x636010 (lote 181A, sin validar)
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
