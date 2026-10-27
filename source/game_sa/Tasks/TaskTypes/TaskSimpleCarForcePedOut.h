#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;

class CVehicle;

// Vtable 0x86EE94 (9 entradas): [0] 0x64BF50 destructor con borrado (llama al cuerpo 0x647790 y, con el bit 0 del argumento, a CTask::operator delete 0x61A5B0), [1] 0x649EE0 Clone,
// [4] 0x647770 GetTaskType, [6] 0x647780 MakeAbortable, [7] 0x6477F0 ProcessPed. Reconstruidos y validados: constructor (0x647710), cuerpo del destructor (0x647790, hook simple),
// Clone, MakeAbortable y ProcessPed (hooks de vtable). El destructor con borrado de la DLL lo genera el compilador (sin hook; se valida contra 0x64BF50 en el banco ForcePedOutVt71).
class NOTSA_EXPORT_VTABLE CTaskSimpleCarForcePedOut : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_CAR_FORCE_PED_OUT;

    static void InjectHooks();

    CVehicle* m_pVehicle; // +0x8
    int32     m_nDoor;    // +0xC

    CTaskSimpleCarForcePedOut(CVehicle* vehicle, int32 door); // 0x647710
    ~CTaskSimpleCarForcePedOut() override;                    // 0x647790

    eTaskType GetTaskType() const override { return Type; } // 0x647770
    CTask* Clone() const override;                          // 0x649EE0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;                 // 0x647780
    bool ProcessPed(CPed* ped) override;                    // 0x6477F0

private:
    // Envoltura del hook del constructor (0x647710)
    CTaskSimpleCarForcePedOut* Destructor() { this->CTaskSimpleCarForcePedOut::~CTaskSimpleCarForcePedOut(); return this; } // 0x647790
    CTaskSimpleCarForcePedOut* Constructor(CVehicle* vehicle, int32 door) { this->CTaskSimpleCarForcePedOut::CTaskSimpleCarForcePedOut(vehicle, door); return this; }
};

VALIDATE_SIZE(CTaskSimpleCarForcePedOut, 0x10);
