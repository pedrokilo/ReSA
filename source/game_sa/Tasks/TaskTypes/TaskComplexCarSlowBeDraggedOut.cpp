/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCarSlowBeDraggedOut
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h>
#include <Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h>
#include <Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.h>
#include <Tasks/TaskTypes/TaskSimpleCarSetPedOut.h>
#include <Tasks/TaskTypes/TaskSimpleCarFallOut.h>
#include <Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h>

void CTaskComplexCarSlowBeDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOut, 0x86EF54, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648490);
    RH_ScopedInstall(Destructor, 0x64C580);

    RH_ScopedInstall(CreateTaskUtilityLineUpPedWithCar, 0x648520);
    RH_ScopedInstall(CreateSubTask, 0x64C6A0);
    RH_ScopedInstall(PrepareVehicleForPedExit, 0x6485A0);

    RH_ScopedVMTInstall(Clone, 0x64A120);
    RH_ScopedVMTInstall(GetTaskType, 0x648500);
    RH_ScopedVMTInstall(MakeAbortable, 0x64C600);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x64C810);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x64C8B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x648510);
}

// 0x648490
CTaskComplexCarSlowBeDraggedOut::CTaskComplexCarSlowBeDraggedOut(CVehicle* vehicle, eTargetDoor door, bool isDraggedBySelf) :
    m_Door{ door },
    m_Vehicle{ vehicle },
    m_IsDraggedBySelf{ isDraggedBySelf }
{ }

// 0x64C580
CTaskComplexCarSlowBeDraggedOut::~CTaskComplexCarSlowBeDraggedOut() {
    // Como el original: primero se limpia la referencia del vehiculo y despues se borra la utilidad de alineacion
    // Como el original: CleanUpOldReference sin anular el campo (la limpieza posterior de EntityRef no encuentra nada)
    if (CVehicle* veh = m_Vehicle) {
        veh->CleanUpOldReference(reinterpret_cast<CEntity**>(std::addressof(m_Vehicle)));
    }
    if (m_LineUpUtility) { // Como el original: test edi,edi antes del destructor y operator delete (0x64C5BF)
        delete m_LineUpUtility; // sin anular el campo
    }
}

// 0x648520
void CTaskComplexCarSlowBeDraggedOut::CreateTaskUtilityLineUpPedWithCar(CPed* ped) {
    UNUSED(ped);

    // Como el original: la utilidad anterior (si la hay) no se libera, se sobrescribe el puntero
    m_LineUpUtility = new CTaskUtilityLineUpPedWithCar(CVector{}, 0, 0, m_Door);
}

// 0x64C6A0
CTask* CTaskComplexCarSlowBeDraggedOut::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_FALL_OUT: { // 0x64C7A9
        return new CTaskSimpleCarFallOut{m_Vehicle, m_Door, m_LineUpUtility};
    }
    case TASK_FINISHED: { // 0x64C718
        return nullptr;
    }
    case TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT: { // 0x64C770
        return new CTaskSimpleCarSetPedSlowDraggedOut{ m_Vehicle, m_Door };
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT: { // 0x64C72C
        return new CTaskSimpleCarSetPedOut{ m_Vehicle, m_Door, !m_IsDraggedBySelf }; // Como el original: sete (invertido)
    }
    case TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT: { // 0x64C6E3
        return new CTaskSimpleCarSlowBeDraggedOut{ m_Vehicle, m_Door, m_LineUpUtility, m_IsDraggedBySelf };
    }
    default:
        return nullptr; // Como el original: tipos desconocidos devuelven nullptr
    }
}

// 0x64C600
bool CTaskComplexCarSlowBeDraggedOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: solo con prioridad inmediata se aborta la subtarea, se saca al peaton y se devuelve true
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
    CTaskSimpleCarSetPedSlowDraggedOut{ m_Vehicle, m_Door }.ProcessPed(ped);
    return true;
}

// 0x64C810
CTask* CTaskComplexCarSlowBeDraggedOut::CreateNextSubTask(CPed* ped) {
    if (!ped->IsInVehicle()) {
        return nullptr;
    }
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_FALL_OUT:                 return TASK_SIMPLE_CAR_SET_PED_OUT;
        case TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT:      return TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT;
        case TASK_SIMPLE_CAR_SET_PED_OUT:              // Como el original: SET_PED_OUT termina
        case TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT: return TASK_FINISHED;
        default:                                       return TASK_NONE; // Como el original: nullptr (CreateSubTask devuelve nullptr)
        }
    }());
}

// 0x6485A0
void CTaskComplexCarSlowBeDraggedOut::PrepareVehicleForPedExit(CPed* ped) {
    if (m_Door == TARGET_DOOR_DRIVER) {
        m_Vehicle->SetStatus(m_Vehicle->IsDriverAPlayer() ? STATUS_FORCED_STOP : STATUS_ABANDONED);
    }
}

// 0x??????
void CTaskComplexCarSlowBeDraggedOut::PreparePedForVehicleExit(CPed* ped) {
    ped->SetOrientation(0.f, 0.f, ped->m_fCurrentRotation);
    ped->SetPedState(PEDSTATE_NONE);
}

// 0x64C8B0
CTask* CTaskComplexCarSlowBeDraggedOut::CreateFirstSubTask(CPed* ped) {
    // Como el original: fuera del vehiculo no hace nada y devuelve nullptr
    if (!ped->IsInVehicle()) {
        return nullptr;
    }
    PrepareVehicleForPedExit(ped);
    PreparePedForVehicleExit(ped);

    CreateTaskUtilityLineUpPedWithCar(ped);

    return CreateSubTask(m_IsDraggedBySelf
        ? TASK_SIMPLE_CAR_FALL_OUT
        : TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT
    );
}
