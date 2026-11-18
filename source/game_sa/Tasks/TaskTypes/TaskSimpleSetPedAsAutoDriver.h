#pragma once

#include "TaskSimple.h"

class CEvent;
class CVehicle;
class CTaskSimpleSetPedAsAutoDriver;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleSetPedAsAutoDriver : public CTaskSimple {

public:
    CVehicle* m_veh = {}; // 0x8

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER;

    CTaskSimpleSetPedAsAutoDriver(CVehicle* veh);
    CTaskSimpleSetPedAsAutoDriver(const CTaskSimpleSetPedAsAutoDriver&);
    ~CTaskSimpleSetPedAsAutoDriver();

    // 0x64A2F0
    CTask*    Clone() const override { return new CTaskSimpleSetPedAsAutoDriver{*this}; }
    // 0x648FA0
    eTaskType GetTaskType() const override { return Type; }
    // 0x648FB0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x648F40
    CTaskSimpleSetPedAsAutoDriver* Constructor(CVehicle* veh) {
        this->CTaskSimpleSetPedAsAutoDriver::CTaskSimpleSetPedAsAutoDriver(veh);
        return this;
    }
    // 0x648FC0
    CTaskSimpleSetPedAsAutoDriver* Destructor() {
        this->CTaskSimpleSetPedAsAutoDriver::~CTaskSimpleSetPedAsAutoDriver();
        return this;
    }
    CTaskSimpleSetPedAsAutoDriver* ScalarDeletingDestructor(uint8 flags); // 0x64CD60 (lote 180A, sin validar): destructor con borrado (ranura 0 de la vtable 0x86EFF4), `ret 4`
};
