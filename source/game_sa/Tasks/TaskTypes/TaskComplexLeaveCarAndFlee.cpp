/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveCarAndFlee
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

#include <reversiblebugfixes/Bugs.hpp>

#include "TaskComplexLeaveCarAndFlee.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexWander.h"

void CTaskComplexLeaveCarAndFlee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndFlee, 0x86e8ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63BF90);
    RH_ScopedInstall(Destructor, 0x63C030);

    RH_ScopedInstall(CreateSubTask, 0x63C0A0);

    RH_ScopedVMTInstall(Clone, 0x63DB40);
    RH_ScopedVMTInstall(GetTaskType, 0x63C020);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x642390);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6424C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x63C090);
}

// 0x63BF90
CTaskComplexLeaveCarAndFlee::CTaskComplexLeaveCarAndFlee(CVehicle* targetVeh, CVector const& fleePoint, eTargetDoor targetDoor, int32 fleeDelayTime, bool waitForCarToSlowDown) :
    m_veh{targetVeh},
    m_door{targetDoor},
    m_fleeDelayMs{fleeDelayTime},
    m_fleePoint{fleePoint},
    m_waitForCarToSlowDown{waitForCarToSlowDown}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveCarAndFlee::CTaskComplexLeaveCarAndFlee(const CTaskComplexLeaveCarAndFlee& o) :
    CTaskComplexLeaveCarAndFlee{ o.m_veh, o.m_fleePoint, o.m_door, o.m_fleeDelayMs, false }
{
}

// 0x63C030
CTaskComplexLeaveCarAndFlee::~CTaskComplexLeaveCarAndFlee() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63C0A0
CTask* CTaskComplexLeaveCarAndFlee::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_SMART_FLEE_POINT: // 0x63C177
        return new CTaskComplexSmartFleePoint{
            m_waitForCarToSlowDown && (!notsa::bugfixes::CTaskComplexLeaveCarAndFlee_MissingNullCheckForVehicleOnFlee || m_veh ) // Inverted
                ? m_veh->GetPosition()
                : m_fleePoint,
            false,
            60.f,
            1'000'000
        };
    case TASK_COMPLEX_LEAVE_CAR: // 0x63C12E
        return new CTaskComplexLeaveCar{
            m_veh,
            (int32)(m_door),
            m_fleeDelayMs,
            m_waitForCarToSlowDown,
            true
        };
    case TASK_SIMPLE_GO_TO_POINT: // 0x63C0E6
        return new CTaskSimpleGoToPoint{ PEDMOVE_RUN, m_fleePoint };
    default: // Como el original: TASK_FINISHED y tipos desconocidos devuelven nullptr
        return nullptr;
    }
}

// 0x642390
CTask* CTaskComplexLeaveCarAndFlee::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) { // Tasks in same order as they're created
    case TASK_COMPLEX_LEAVE_CAR: {
        // Como el original: se usa el vehículo del peatón (m_pVehicle), no m_veh
        const auto veh = ped->m_pVehicle;
        if (!veh || !veh->IsBoat()) {
            return CreateSubTask(TASK_COMPLEX_SMART_FLEE_POINT);
        }
        // Como el original: punto = posición + adelante * (radio * -2.0)
        const float k = veh->GetModelInfo()->GetColModel()->GetBoundRadius() * -2.f;
        const auto& fwd = veh->GetForward();
        const auto& pos = veh->GetPosition();
        m_fleePoint.x = fwd.x * k + pos.x;
        m_fleePoint.y = fwd.y * k + pos.y;
        m_fleePoint.z = fwd.z * k + pos.z;
        return CreateSubTask(TASK_SIMPLE_GO_TO_POINT);
    }
    case TASK_SIMPLE_GO_TO_POINT:
        return CreateSubTask(TASK_COMPLEX_SMART_FLEE_POINT);
    case TASK_COMPLEX_SMART_FLEE_POINT: {
        const auto task = CreateSubTask(TASK_FINISHED);
        ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);
        return task;
    }
    default: // Como el original: subtareas desconocidas devuelven nullptr
        return nullptr;
    }
}

// 0x6424C0
CTask* CTaskComplexLeaveCarAndFlee::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(
        ped->bInVehicle
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_COMPLEX_SMART_FLEE_POINT
    );
}
