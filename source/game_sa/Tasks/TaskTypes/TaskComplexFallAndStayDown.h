#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;

// Tarea compleja que hace caer al peaton con una animacion de KO y lo deja en el suelo (TASK_COMPLEX_FALL_AND_STAY_DOWN, 209). Vtable 0x870480. Campos (+0xC, +0x10) tomados de sus constructores
// (0x6789D0 y 0x678A10) y de como se pasan a CTaskSimpleFall en CreateFirstSubTask (0x678B20). Clase reconstruida en el lote 191A (sin validar).
class NOTSA_EXPORT_VTABLE CTaskComplexFallAndStayDown : public CTaskComplex {
public:
    AnimationId  m_nFallAnimId;    //!< +0xC: animacion de caida (KO_SKID_FRONT, KO_SPIN_R, KO_SKID_BACK o KO_SPIN_L)
    AssocGroupId m_nFallAnimGroup; //!< +0x10: grupo de animacion

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_FALL_AND_STAY_DOWN;

    static void InjectHooks();

    CTaskComplexFallAndStayDown(AnimationId fallAnimId, AssocGroupId fallAnimGroup); // 0x6789D0
    explicit CTaskComplexFallAndStayDown(int32 direction);                          // 0x678A10
    ~CTaskComplexFallAndStayDown() override = default;                              // 0x678A90 (cuerpo)

    CTask* CreateSubTask(eTaskType taskType); // 0x678BD0

    eTaskType GetTaskType() const override;                                                                    // 0x678A00
    CTask*    Clone() const override;                                                                          // 0x67C270
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override; // 0x678AA0
    CTask*    CreateNextSubTask(CPed* ped) override;                                                           // 0x67CBE0
    CTask*    CreateFirstSubTask(CPed* ped) override;                                                          // 0x678B20
    CTask*    ControlSubTask(CPed* ped) override;                                                              // 0x678BC0

private: // Wrappers for hooks
    // 0x6789D0
    CTaskComplexFallAndStayDown* Constructor(AnimationId fallAnimId, AssocGroupId fallAnimGroup) {
        this->CTaskComplexFallAndStayDown::CTaskComplexFallAndStayDown(fallAnimId, fallAnimGroup);
        return this;
    }
    // 0x678A10
    CTaskComplexFallAndStayDown* Constructor2(int32 direction) {
        this->CTaskComplexFallAndStayDown::CTaskComplexFallAndStayDown(direction);
        return this;
    }
    // 0x678A90
    __declspec(noinline) CTaskComplexFallAndStayDown* Destructor() {
        this->CTaskComplexFallAndStayDown::~CTaskComplexFallAndStayDown();
        return this;
    }
    CTaskComplexFallAndStayDown* ScalarDeletingDestructor(uint8 flags); // 0x67CBC0: destructor con borrado (ranura 0 de la vtable 0x870480), `ret 4`
};
VALIDATE_SIZE(CTaskComplexFallAndStayDown, 0x14);
