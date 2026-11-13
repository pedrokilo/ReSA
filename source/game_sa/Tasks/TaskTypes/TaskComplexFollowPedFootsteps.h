#pragma once

#include "TaskComplex.h"

class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowPedFootsteps : public CTaskComplex {
public:
    CPed*        m_targetPed;
    uint32       m_subTaskCreateCheckTimer;
    uint32       m_lineOfSightCheckTimer;
    CPointRoute* m_pointRoute;
    int16        m_moveState;
    uint8        m_updateGoToPoint : 1;

public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_PED_FOOTSTEPS;

    explicit CTaskComplexFollowPedFootsteps(CPed* ped);
    ~CTaskComplexFollowPedFootsteps() override;

    CTask* Clone() const override; // 0x695890 (lote 185A, sin validar)
    eTaskType GetTaskType() const override; // 0x694EC0 (lote 166A, banco Fn166A)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    virtual CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    __declspec(noinline) CTaskComplexFollowPedFootsteps* Constructor(CPed* ped); // 0x694E20 (con hook): sin expansion en linea (el archivo se compila con /O2 e inlining) para que Clone la llame por su cuerpo enganchado
};
VALIDATE_SIZE(CTaskComplexFollowPedFootsteps, 0x20);
