#pragma once

#include "Base.h"
#include "Vector.h"

#include "TaskSimple.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleWaitUntilPedIsOutCar : public CTaskSimple {
public:
    CPed*   m_PedToWaitFor;
    bool    m_bIsNotGoingTowardsPos;
    CVector m_Pos;

public:
    static constexpr auto Type = TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR;

    CTaskSimpleWaitUntilPedIsOutCar(CPed * ped, CVector const& pos);
    ~CTaskSimpleWaitUntilPedIsOutCar() override;



/*!
 * @addr 0x64A4B0
 */
CTask* Clone() const override { return new CTaskSimpleWaitUntilPedIsOutCar(m_PedToWaitFor, m_Pos); }



/*!
 * @addr 0x6494D0
 */
eTaskType GetTaskType() const override { return Type; }



/*!
 * @addr 0x6494E0
 */
bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }

    bool ProcessPed(CPed * ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    /*!
     * @addr 0x649450
     */
    CTaskSimpleWaitUntilPedIsOutCar* Constructor(CPed * ped, CVector const& pos) { this->CTaskSimpleWaitUntilPedIsOutCar::CTaskSimpleWaitUntilPedIsOutCar(ped, pos); return this; }
    /*!
     * @addr 0x6494F0
     */
    CTaskSimpleWaitUntilPedIsOutCar* Destructor() { this->CTaskSimpleWaitUntilPedIsOutCar::~CTaskSimpleWaitUntilPedIsOutCar(); return this; }

                };
VALIDATE_SIZE(CTaskSimpleWaitUntilPedIsOutCar, 0x1C);
