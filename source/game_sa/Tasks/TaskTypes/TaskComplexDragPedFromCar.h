#pragma once

#include "TaskComplexEnterCar.h"
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexDragPedFromCar : public CTaskComplexEnterCar {
public:
    CPed* m_Ped;

public:
    static constexpr auto Type = TASK_COMPLEX_DRAG_PED_FROM_CAR;

    CTaskComplexDragPedFromCar(CPed* ped, int32 draggedPedDownTime = 0);
    ~CTaskComplexDragPedFromCar() override;

    eTaskType GetTaskType() const override; // 0x6404C0 (lote 177A, sin validar)
    CTask* Clone() const override;          // 0x643950 (lote 177A, sin validar)

    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;

};
VALIDATE_SIZE(CTaskComplexDragPedFromCar, 0x54);
extern void CTaskComplexDragPedFromCar__InjectHooks();
