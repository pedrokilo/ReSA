#pragma once

#include "Vector.h"
#include "TaskComplex.h"

class CPedGroup;
class CTaskComplexFollowLeaderAnyMeans;
class CTask;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowLeaderAnyMeans : public CTaskComplex {

public:
    CPedGroup* m_group {};
    CPed*      m_ped{};
    CVector    m_pos{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_FOLLOW_LEADER_ANY_MEANS;

    CTaskComplexFollowLeaderAnyMeans(CPedGroup* pedGroup, CPed* ped, CVector const& pos);
    CTaskComplexFollowLeaderAnyMeans(const CTaskComplexFollowLeaderAnyMeans&);
    ~CTaskComplexFollowLeaderAnyMeans();

    CTask* CreateSubTask(eTaskType taskType, CPed const* ped);

    CTask*    Clone() const override { return new CTaskComplexFollowLeaderAnyMeans{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
    __declspec(noinline) CTaskComplexFollowLeaderAnyMeans* Destructor(); // 0x694B30 (con hook, lote 185A): cuerpo del destructor del EXE (vtable 0x870C68, limpia +0x10, CTaskComplex::Destructor); sin expansion en linea

private: // Wrappers for hooks
    // 0x694AA0
    CTaskComplexFollowLeaderAnyMeans* Constructor(CPedGroup* pedGroup, CPed* ped, CVector const& pos) {
        this->CTaskComplexFollowLeaderAnyMeans::CTaskComplexFollowLeaderAnyMeans(pedGroup, ped, pos);
        return this;
    }
};
