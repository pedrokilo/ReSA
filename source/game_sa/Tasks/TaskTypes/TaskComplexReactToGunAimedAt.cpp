/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexReactToGunAimedAt
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
#include "TaskComplexReactToGunAimedAt.h"
#include "PedStats.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleHandsUp.h"
#include "TaskSimplePause.h"
#include "TaskSimpleCower.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskComplexWanderStandard.h"

void CTaskComplexReactToGunAimedAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexReactToGunAimedAt, 0x870d78, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6971B0);
    RH_ScopedInstall(Destructor, 0x697230);

    RH_ScopedInstall(CreateSubTask, 0x697B70);

    RH_ScopedVMTInstall(Clone, 0x6976E0);
    RH_ScopedVMTInstall(GetTaskType, 0x697220);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6983C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x698530);
    RH_ScopedVMTInstall(ControlSubTask, 0x698550);
}

// 0x6971B0
CTaskComplexReactToGunAimedAt::CTaskComplexReactToGunAimedAt(CPed* aimedBy) :
    m_aimedBy{aimedBy}
{
    CEntity::SafeRegisterRef(m_aimedBy);
}


CTaskComplexReactToGunAimedAt::CTaskComplexReactToGunAimedAt(const CTaskComplexReactToGunAimedAt& o) :
    CTaskComplexReactToGunAimedAt{ o.m_aimedBy }
{
}

// 0x697230
CTaskComplexReactToGunAimedAt::~CTaskComplexReactToGunAimedAt() {
    CEntity::SafeCleanUpRef(m_aimedBy);
}

// 0x697B70
CTask* CTaskComplexReactToGunAimedAt::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        // Como el original: GetRadianAngleBetweenPoints + LimitRadianAngle, guardado también en m_fAimingRotation
        const auto& pedPos = ped->GetPosition();
        const auto& aimPos = m_aimedBy->GetPosition();
        const float dx = aimPos.x - pedPos.x;
        const float dy = aimPos.y - pedPos.y;
        const float heading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dx, dy, 0.0f, 0.0f));
        ped->m_fAimingRotation = heading;
        return new CTaskSimpleAchieveHeading{ heading };
    }
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_HANDS_UP: // Como el original: 0x407180 tras reservar la tarea
        return new CTaskSimpleHandsUp{ (uint32)CGeneral::GetRandomIntInRange(3000, 5000) };
    case TASK_SIMPLE_PAUSE: {
        // Como el original: 0x407180 ([1000, 2000)) antes de reservar la tarea
        const int32 time = ped->IsInVehicle() ? CGeneral::GetRandomIntInRange(1000, 2000) : 0;
        return new CTaskSimplePause{ time };
    }
    case TASK_SIMPLE_COWER:
        return new CTaskSimpleCower{};
    case TASK_COMPLEX_SMART_FLEE_ENTITY:
        return new CTaskComplexSmartFleeEntity{ m_aimedBy, false, 60.f, 1000000, 1000, 1.f };
    case TASK_COMPLEX_WANDER: {
        const auto heading = (uint8)CVector2D{ped->GetPosition() - m_aimedBy->GetPosition()}.NodeHeading();
        auto* const task = new CTaskComplexWanderStandard{ PEDMOVE_WALK, heading };
        m_wanderBeginTimer.Start(10'000); // Como el original: después de crear la tarea
        return task;
    }
    default: // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nullptr
        return nullptr;
    }
}

// 0x6983C0
CTask* CTaskComplexReactToGunAimedAt::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([ped, this] {
        // Cases ordered in the same way the tasks are created (eg.: Pause is created first)
        const auto type = m_pSubTask->GetTaskType();
        switch (type) {
        case TASK_SIMPLE_PAUSE:
            return ped->IsInVehicle()
                ? TASK_COMPLEX_LEAVE_CAR
                : TASK_SIMPLE_ACHIEVE_HEADING;
        case TASK_COMPLEX_LEAVE_CAR:
            return TASK_SIMPLE_ACHIEVE_HEADING;
        case TASK_SIMPLE_ACHIEVE_HEADING: {
            if (ped->m_pStats->m_flags & 0x80) {
                return TASK_SIMPLE_COWER; // => TASK_COMPLEX_SMART_FLEE_ENTITY
            } else {
                ped->Say(CTX_GLOBAL_GUN_COOL);
                return TASK_SIMPLE_HANDS_UP; // => TASK_COMPLEX_WANDER
            }
        }
        case TASK_SIMPLE_COWER:
            ped->Say(CTX_GLOBAL_GUN_RUN); // Como el original: 0x73 antes de huir
            return TASK_COMPLEX_SMART_FLEE_ENTITY;
        case TASK_SIMPLE_HANDS_UP:
            return TASK_COMPLEX_WANDER;
        case TASK_COMPLEX_SMART_FLEE_ENTITY:
        case TASK_COMPLEX_WANDER:
            return TASK_FINISHED;
        default: // Como el original: subtarea desconocida -> nullptr
            return TASK_NONE;
        }
    }(), ped);
}

// 0x698530
CTask* CTaskComplexReactToGunAimedAt::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
}

// 0x698550
CTask* CTaskComplexReactToGunAimedAt::ControlSubTask(CPed* ped) {
    if (!m_aimedBy) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_WANDER) {
        if (m_wanderBeginTimer.IsOutOfTime()) {
            if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) { // Como el original: prioridad 1
                return nullptr;
            }
        }
    }
    return m_pSubTask;
}
