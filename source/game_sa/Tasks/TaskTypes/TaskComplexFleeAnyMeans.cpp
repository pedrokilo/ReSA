/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFleeAnyMeans
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
#include "TaskComplexFleeAnyMeans.h"
#include "TaskComplexFleeShooting.h"
#include "TaskComplexWanderFlee.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexCarDriveMission.h"
#include "TaskComplexGoToPointShooting.h"

void CTaskComplexFleeAnyMeans::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleeAnyMeans, 0x86f7f0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65CC60);
    RH_ScopedInstall(Destructor, 0x65CD10);

    RH_ScopedInstall(CreateSubTask, 0x65D2E0);

    RH_ScopedVMTInstall(Clone, 0x65D030);
    RH_ScopedVMTInstall(GetTaskType, 0x65CD00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65D540);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65D730);
    RH_ScopedVMTInstall(ControlSubTask, 0x65D780);
}

// 0x65CC60
CTaskComplexFleeAnyMeans::CTaskComplexFleeAnyMeans(
    CEntity* entity,
    bool attackWhileFeeling,
    float safeDist,
    int32 fleeTime,
    int32 shootTime,
    int32 shootRecoverTime,
    float stealCarDist,
    int32 posChangePeriod,
    float posChangeTolerance
) :
    CTaskComplexSmartFleeEntity{ entity, false, safeDist, fleeTime, posChangePeriod, posChangeTolerance },
    m_minDistFromFleeEntityToStealCar{ stealCarDist },
    m_shootWhileFleeing{ attackWhileFeeling },
    m_shootRecoverTime{shootRecoverTime},
    m_shootTime{shootTime}
{
    // Como el original: la entidad se registra otra vez (ya la registra SmartFlee; el segundo registro no hace nada)
    CEntity::SafeRegisterRef(m_fleeFrom);
}

CTaskComplexFleeAnyMeans::CTaskComplexFleeAnyMeans(const CTaskComplexFleeAnyMeans& o) :
    CTaskComplexFleeAnyMeans{
        o.m_fleeFrom,
        o.m_shootWhileFleeing,
        o.m_safeDistance,
        o.m_time,
        o.m_shootTime,
        o.m_shootRecoverTime,
        o.m_minDistFromFleeEntityToStealCar,
        o.m_posCheckPeriod,
        o.m_posChangeTolerance
    }
{
}

// 0x65CD10
CTaskComplexFleeAnyMeans::~CTaskComplexFleeAnyMeans() {
    // Como el original: la entidad se limpia aquí y otra vez en el destructor de SmartFlee
    CEntity::SafeCleanUpRef(m_fleeFrom);
#ifdef FIX_BUGS
    CEntity::SafeCleanUpRef(m_stealVeh);
#endif
}

// 0x65D2E0
CTask* CTaskComplexFleeAnyMeans::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_FLEE_SHOOTING:
        return new CTaskComplexFleeShooting{
            m_fleeFrom,
            m_pedScream,
            m_safeDistance,
            m_time,
            m_shootTime,
            m_shootRecoverTime,
            m_posCheckPeriod,
            m_posChangeTolerance
        };
    case TASK_COMPLEX_WANDER:
        return new CTaskComplexWanderFlee{
            PEDMOVE_SPRINT,
            (uint8)CGeneral::GetRandomIntInRange(0, 8) // Como el original: 0x407180 con 1/32768
        };
    case TASK_COMPLEX_STEAL_CAR:
        return new CTaskComplexStealCar{ m_stealVeh };
    case TASK_COMPLEX_CAR_DRIVE_MISSION:
        return new CTaskComplexCarDriveMission{
            ped->m_pVehicle,
            nullptr,
            MISSION_CRUISE,
            DRIVING_STYLE_PLOUGH_THROUGH,
            40.f
        };
    case TASK_COMPLEX_GO_TO_POINT_SHOOTING:
        return new CTaskComplexGoToPointShooting{
            PEDMOVE_RUN,
            m_stealVeh->GetPosition(),
            m_stealVeh,
            {},
            m_stealVeh->GetModelInfo()->GetColModel()->GetBoundRadius() + 0.35f,
            4.f
        };
    default: // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nullptr
        return nullptr;
    }
}

// 0x65D540
CTask* CTaskComplexFleeAnyMeans::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_STEAL_CAR: {
        // Como el original: m_stealVeh se pone a nulo después de crear la tarea (sin limpiar la referencia)
        const auto task = ped->IsInVehicle()
            ? CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped)
            : CreateFirstSubTask(ped);
        m_stealVeh = nullptr;
        return task;
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION:
        return ped->IsInVehicle()
            ? CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped)
            : CreateFirstSubTask(ped);
    case TASK_COMPLEX_GO_TO_POINT_SHOOTING:
        return m_stealVeh
            ? CreateSubTask(TASK_COMPLEX_STEAL_CAR, ped)
            : CreateFirstSubTask(ped);
    case TASK_COMPLEX_WANDER:
    case TASK_COMPLEX_FLEE_SHOOTING:
        return CreateSubTask(TASK_FINISHED, ped);
    default: // Como el original: una subtarea desconocida devuelve nullptr
        return nullptr;
    }
}

// 0x65D730
CTask* CTaskComplexFleeAnyMeans::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        return ped->IsInVehicle()
            ? TASK_COMPLEX_CAR_DRIVE_MISSION
            : m_shootWhileFleeing
                ? TASK_COMPLEX_FLEE_SHOOTING
                : TASK_COMPLEX_WANDER;
    }(), ped);
}

// 0x65D780
CTask* CTaskComplexFleeAnyMeans::ControlSubTask(CPed* ped) {
    if (!m_fleeFrom) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)
            ? nullptr
            : m_pSubTask;
    }

    // Como el original: primero se busca el vehículo más cercano y después se mira el tipo de la subtarea (dos llamadas)
    const auto closestVeh = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange();
    if (!closestVeh) {
        return m_pSubTask;
    }
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_FLEE_SHOOTING && m_pSubTask->GetTaskType() != TASK_COMPLEX_WANDER) {
        return m_pSubTask;
    }

    // Check if we're far enough from the entity we're fleeing from to enter a car
    // Como el original: (z*z + y*y) + x*x y solo se sigue si la distancia es estrictamente mayor (NaN no sigue)
    {
        const CVector d = ped->GetPosition() - m_fleeFrom->GetPosition();
        const float distSq = d.z * d.z + d.y * d.y + d.x * d.x;
        if (!(m_minDistFromFleeEntityToStealCar * m_minDistFromFleeEntityToStealCar < distSq)) {
            return m_pSubTask;
        }
    }

    // Check if vehicle is in front of the ped ([-90, 90] deg)
    // Como el original: CVector::Normalise (0x59C910), producto (z + y) + x con el forward de la matriz; NaN no sigue
    {
        CVector pedToVeh = closestVeh->GetPosition() - ped->GetPosition();
        pedToVeh.Normalise();
        const auto& fwd = ped->GetForward();
        if (!(pedToVeh.z * fwd.z + pedToVeh.y * fwd.y + pedToVeh.x * fwd.x > 0.f)) {
            return m_pSubTask;
        }
    }

    // Como el original: el vehículo tiene que poder robarse
    if (!CCarEnterExit::IsVehicleStealable(closestVeh, ped)) {
        return m_pSubTask;
    }

    // Register it
    CEntity::ChangeEntityReference(m_stealVeh, closestVeh);

    // Add `VehicleDamageWeapon` all occupants of the stolen vehicle (if we're attacking)
    eTaskType taskType;
    if (m_shootWhileFleeing) {
        taskType = TASK_COMPLEX_GO_TO_POINT_SHOOTING;
        const auto AddEventForVehicleOccupant = [&](CPed* occupant) {
            if (occupant && !occupant->IsPlayer()) {
                // Como el original: la respuesta del evento se fija a TASK_COMPLEX_LEAVE_CAR_AND_FLEE
                CEventVehicleDamageWeapon event{ m_stealVeh, ped, WEAPON_PISTOL };
                event.m_TaskId = TASK_COMPLEX_LEAVE_CAR_AND_FLEE;
                occupant->GetIntelligence()->m_eventGroup.Add(&event, false);
            }
        };
        AddEventForVehicleOccupant(m_stealVeh->m_pDriver);
        // Como el original: se recorre hasta m_nMaxPassengers (releído en cada vuelta)
        for (int32 i = 0; i < (int32)m_stealVeh->m_nMaxPassengers; i++) {
            AddEventForVehicleOccupant(m_stealVeh->m_apPassengers.data()[i]);
        }
    } else {
        if (!closestVeh->CanPedStepOutCar(false)) {
            return m_pSubTask;
        }
        taskType = TASK_COMPLEX_STEAL_CAR;
    }

    // Finally, try aborting the current task (`COMPLEX_FLEE_SHOOTING` or `COMPLEX_WANDER`)
    if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
        return CreateSubTask(taskType, ped);
    }

    return m_pSubTask;
}
