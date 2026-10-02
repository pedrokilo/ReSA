/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDragPedFromCar
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexDragPedFromCar.h"

void CTaskComplexDragPedFromCar__InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDragPedFromCar, 0x86EB6C, 12); // 12 huecos, como CTaskComplexEnterCar
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(ControlSubTask, 0x640530);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643D00);
}

// 0x640430
CTaskComplexDragPedFromCar::CTaskComplexDragPedFromCar(CPed* ped, int32 draggedPedDownTime) : CTaskComplexEnterCar(nullptr, false, false, true, false) {
    m_Ped = ped;
    CEntity::SafeRegisterRef(m_Ped);
    m_DraggedPedDownTime = draggedPedDownTime;
}

// 0x6404D0
CTaskComplexDragPedFromCar::~CTaskComplexDragPedFromCar() {
    CEntity::SafeCleanUpRef(m_Ped);
}

// 0x640530


CTask* CTaskComplexDragPedFromCar::ControlSubTask(CPed* ped) {
    if (m_NumGettingInSet)
        return CTaskComplexEnterCar::ControlSubTask(ped);

    if (!m_Ped || m_Ped->bInVehicle || !m_pSubTask->MakeAbortable(ped))
        return CTaskComplexEnterCar::ControlSubTask(ped);

    return CTaskComplexEnterCar::CreateSubTask(TASK_NONE, nullptr);
}

// 0x643D00

CTask* CTaskComplexDragPedFromCar::CreateFirstSubTask(CPed* ped) {
    if (!m_Ped || !m_Ped->m_pVehicle || !m_Ped->bInVehicle ||
        (!m_Ped->m_pVehicle->IsPassenger(m_Ped) && !m_Ped->m_pVehicle->IsDriver(m_Ped))) {
        return CTaskComplexEnterCar::CreateSubTask(TASK_NONE, ped);
    }
    CEntity::SafeCleanUpRef(m_Car);
    m_Car = m_Ped->m_pVehicle;
    CEntity::SafeRegisterRef(m_Car);
    m_bAsDriver = m_Car->m_pDriver == m_Ped;
    m_bQuitAfterOpeningDoor = false;
    m_bQuitAfterDraggingPedOut = true;
    if (m_Car->m_pHandlingData->m_bTandemSeats || m_Car->IsBike() || m_Car->IsSubQuad()) {
        m_TargetSeat = 0;
    } else {
        m_TargetSeat = CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Ped);
    }
    return CTaskComplexEnterCar::CreateFirstSubTask(ped);
}