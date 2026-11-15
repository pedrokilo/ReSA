#pragma once

#include "TaskComplex.h"

class CPed;

// Tarea del peaton que se identifica ante un policia (vtable 0x86F544, 0x10 bytes): se gira hacia el policia (CTaskSimpleAchieveHeading), levanta las manos 3 segundos
// (CTaskSimpleHandsUp) y termina; si el policia desaparece (la referencia se pone a nulo) termina en cuanto se le pide la subtarea.
class NOTSA_EXPORT_VTABLE CTaskComplexPresentIDToCop : public CTaskComplex {
public:
    CPed* m_pCop; // +0xC, con referencia registrada (CEntity::RegisterReference)

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_PRESENT_ID_TO_COP;

    explicit CTaskComplexPresentIDToCop(CPed* cop); // 0x6591D0
    ~CTaskComplexPresentIDToCop() override;         // cuerpo del EXE: 0x659240

    eTaskType GetTaskType() const override { return Type; } // 0x659230
    CTask* Clone() const override;                          // 0x659B60
    CTask* CreateNextSubTask(CPed* ped) override;           // 0x65AFE0
    CTask* CreateFirstSubTask(CPed* ped) override;          // 0x65B050
    CTask* ControlSubTask(CPed* ped) override;              // 0x65B070

    CTask* CreateSubTask(eTaskType taskType, CPed* ped); // 0x65A110
    float  ComputeHeading(CPed* ped);                    // 0x6592A0

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexPresentIDToCop* Constructor(CPed* cop);
};
VALIDATE_SIZE(CTaskComplexPresentIDToCop, 0x10);
