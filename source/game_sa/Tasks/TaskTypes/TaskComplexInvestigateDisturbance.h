#pragma once

#include "Vector.h"
#include "TaskComplex.h"

class CTaskComplexInvestigateDisturbance;
class CEvent;
class CPed;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexInvestigateDisturbance : public CTaskComplex {
public:
    CVector  m_pos{};
    CEntity* m_entity{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_INVESTIGATE_DISTURBANCE;

    CTaskComplexInvestigateDisturbance(CVector const& targetPos, CEntity* srcEntity);
    CTaskComplexInvestigateDisturbance(const CTaskComplexInvestigateDisturbance&);
    ~CTaskComplexInvestigateDisturbance();

    // 0x66D3D0
    CTask*    Clone() const override { return new CTaskComplexInvestigateDisturbance{ *this }; }
    // 0x66C790
    eTaskType GetTaskType() const override { return Type; }
    // 0x66C800
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return !m_pSubTask || m_pSubTask->MakeAbortable(ped, priority, event); }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    // 0x66CA30
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x66C720
    CTaskComplexInvestigateDisturbance* Constructor(CVector const& targetPos, CEntity* srcEntity) {
        this->CTaskComplexInvestigateDisturbance::CTaskComplexInvestigateDisturbance(targetPos, srcEntity);
        return this;
    }

    // 0x66C7A0
public:
    // 0x66C7A0
    __declspec(noinline) CTaskComplexInvestigateDisturbance* Destructor() {
        this->CTaskComplexInvestigateDisturbance::~CTaskComplexInvestigateDisturbance();
        return this;
    }
};
