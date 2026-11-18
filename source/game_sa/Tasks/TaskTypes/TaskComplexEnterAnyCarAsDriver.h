#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexEnterAnyCarAsDriver : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER;

    CTaskComplexEnterAnyCarAsDriver() = default;
    ~CTaskComplexEnterAnyCarAsDriver() = default;

    /*!
     * @addr 0x63DE60
     */
    CTask* Clone() const override { return new CTaskComplexEnterAnyCarAsDriver{}; }
    /*!
     * @addr 0x63D0F0
     */
    eTaskType GetTaskType() const override { return Type; }
    /*!
     * @addr 0x63D110
     */
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    /*!
     * @addr 0x63D120
     */
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexEnterAnyCarAsDriver* Destructor() { this->CTaskComplexEnterAnyCarAsDriver::~CTaskComplexEnterAnyCarAsDriver(); return this; }
};
