/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveCarAndWander
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskSimpleNone.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexWander.h"

void CTaskComplexLeaveCarAndWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndWander, 0x86e880, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63BDD0);

    RH_ScopedInstall(Destructor, 0x63BE50);

    RH_ScopedInstall(CreateSubTask, 0x63BEC0);

    RH_ScopedVMTInstall(Clone, 0x63DAD0);
    RH_ScopedVMTInstall(GetTaskType, 0x63BE40);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6422C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642340);
    RH_ScopedVMTInstall(ControlSubTask, 0x63BEB0);
}

// 0x63BDD0
CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(CVehicle* veh, eTargetDoor door, int32 leaveDelayMs, bool sensibleLeaveVeh) :
    m_veh{veh},
    m_door{door},
    m_bSensibleLeaveCar{sensibleLeaveVeh},
    m_leaveDelayMs{ leaveDelayMs }
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveCarAndWander::CTaskComplexLeaveCarAndWander(const CTaskComplexLeaveCarAndWander& o) :
    CTaskComplexLeaveCarAndWander{ o.m_veh, o.m_door, o.m_leaveDelayMs, o.m_bSensibleLeaveCar }
{
}

// 0x63BE50
CTaskComplexLeaveCarAndWander::~CTaskComplexLeaveCarAndWander() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63BEC0
CTask* CTaskComplexLeaveCarAndWander::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{
            m_veh,
            (int32)m_door,
            m_leaveDelayMs,
            m_bSensibleLeaveCar,
            false
        };
    case TASK_NONE:
        return new CTaskSimpleNone{};
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x6422C0
CTask* CTaskComplexLeaveCarAndWander::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_NONE:
    case TASK_COMPLEX_LEAVE_CAR:
        break;
    default:
        return nullptr;
    }

    auto* next = CreateSubTask(TASK_FINISHED);
    auto& manager = ped->GetTaskManager();
    auto* current = manager.GetTaskPrimary(TASK_PRIMARY_DEFAULT);
    if (!current || current->GetTaskType() != TASK_COMPLEX_WANDER) {
        manager.SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);
    }
    return next;
}

// 0x642340
CTask* CTaskComplexLeaveCarAndWander::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        ped->bInVehicle
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_NONE
    );
}

CTask* CTaskComplexLeaveCarAndWander::Clone() const { return new CTaskComplexLeaveCarAndWander{ *this }; }
eTaskType CTaskComplexLeaveCarAndWander::GetTaskType() const { return Type; }
CTask* CTaskComplexLeaveCarAndWander::ControlSubTask(CPed* ped) { return m_pSubTask; }
