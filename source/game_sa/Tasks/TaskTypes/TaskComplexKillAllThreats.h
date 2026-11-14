#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;

// Tarea compleja que busca amenazas con el escaner de conocidos del peaton (CPedAcquaintanceScanner) y ataca a la mas cercana (TASK_KILL_ALL_THREATS, 1014). Campos (+0xC, +0x10, +0x14) tomados de su
// constructor (0x622180) y de como se pasan al constructor de CTaskComplexKillPedOnFoot en CreateNextSubTask (0x6293C0) y CreateFirstSubTask (0x629480). Lote 190A (sin validar).
class NOTSA_EXPORT_VTABLE CTaskComplexKillAllThreats : public CTaskComplex {
public:
    int32 m_pedFlags;     //!< +0xC: se pasa como pedFlags a CTaskComplexKillPedOnFoot
    int32 m_actionDelay;  //!< +0x10: se pasa como delay
    int32 m_actionChance; //!< +0x14: se pasa como chance

public:
    static constexpr auto Type = eTaskType::TASK_KILL_ALL_THREATS;

    static void InjectHooks();

    CTaskComplexKillAllThreats(int32 pedFlags, int32 actionDelay, int32 actionChance); // 0x622180
    ~CTaskComplexKillAllThreats() override;                                            // 0x6221C0

    CPed* ScanForThreat(CPed* ped); // 0x6221D0

    CTask*    Clone() const override;                                          // 0x623750 (lote 170A, banco Fn170A: enganchado aparte como S)
    eTaskType GetTaskType() const override { return Type; }                    // 0x6221B0 (lote 190A, sin validar)
    CTask*    CreateNextSubTask(CPed* ped) override;                           // 0x6293C0 (lote 190A, sin validar)
    CTask*    CreateFirstSubTask(CPed* ped) override;                          // 0x629480 (lote 190A, sin validar)
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }        // 0x622230 (lote 190A, sin validar)

private: // Wrappers for hooks
    // 0x622180
    CTaskComplexKillAllThreats* Constructor(int32 pedFlags, int32 actionDelay, int32 actionChance) {
        this->CTaskComplexKillAllThreats::CTaskComplexKillAllThreats(pedFlags, actionDelay, actionChance);
        return this;
    }
    // 0x6221C0
    __declspec(noinline) CTaskComplexKillAllThreats* Destructor() {
        this->CTaskComplexKillAllThreats::~CTaskComplexKillAllThreats();
        return this;
    }
    CTaskComplexKillAllThreats* ScalarDeletingDestructor(uint8 flags); // 0x6293A0 (lote 190A, sin validar): destructor con borrado (ranura 0 de la vtable 0x86DA1C), `ret 4`
};
VALIDATE_SIZE(CTaskComplexKillAllThreats, 0x18);
