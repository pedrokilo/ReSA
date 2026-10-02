#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexEvasiveCower : public CTaskComplex {
public:
    CVector  m_Pos;
    CEntity* m_Entity;

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_EVASIVE_COWER;

    CTaskComplexEvasiveCower(CEntity* entity, const CVector& pos);
    ~CTaskComplexEvasiveCower() override;

    eTaskType GetTaskType() const override { return Type; } // 0x6554D0
    CTask* Clone() const override { return new CTaskComplexEvasiveCower(m_Entity, m_Pos); } // 0x655CD0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; } // 0x655700

    CTask* CreateSubTask(eTaskType taskType);

private: // Wrappers for hooks
    // 0x655460
    CTaskComplexEvasiveCower* Constructor(CEntity* entity, const CVector& pos) {
        this->CTaskComplexEvasiveCower::CTaskComplexEvasiveCower(entity, pos);
        return this;
    }
    // 0x6554E0
    CTaskComplexEvasiveCower* Destructor() {
        this->CTaskComplexEvasiveCower::~CTaskComplexEvasiveCower();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexEvasiveCower, 0x1C);
