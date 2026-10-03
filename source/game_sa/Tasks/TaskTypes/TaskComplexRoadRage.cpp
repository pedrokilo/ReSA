/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexRoadRage
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "InterestingEvents.h"
#include "TaskComplexRoadRage.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleShakeFist.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexSeekEntityStandard.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexKillPedOnFoot.h"

void CTaskComplexRoadRage::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexRoadRage, 0x86d9f0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6220A0);
    RH_ScopedInstall(Destructor, 0x622110);

    RH_ScopedInstall(CreateSubTask, 0x629080);

    RH_ScopedVMTInstall(Clone, 0x623670);
    RH_ScopedVMTInstall(GetTaskType, 0x622100);
    RH_ScopedVMTInstall(MakeAbortable, 0x622170);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62E270);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62E170);
    RH_ScopedVMTInstall(ControlSubTask, 0x62E3F0);
}

// 0x6220A0
CTaskComplexRoadRage::CTaskComplexRoadRage(CPed* rageWith) :
    m_rageWith{rageWith}
{
    CEntity::SafeRegisterRef(m_rageWith);
}

CTaskComplexRoadRage::CTaskComplexRoadRage(const CTaskComplexRoadRage& o) :
    CTaskComplexRoadRage{o.m_rageWith}
{
}

// 0x622110
CTaskComplexRoadRage::~CTaskComplexRoadRage() {
    CEntity::SafeCleanUpRef(m_rageWith);
}

// 0x629080
CTask* CTaskComplexRoadRage::CreateSubTask(eTaskType taskType, CPed* ped) {
    if (!m_rageWith) { // Como el original: sin objetivo no se crea nada
        return nullptr;
    }
    switch (taskType) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        return new CTaskComplexTurnToFaceEntityOrCoord{ m_rageWith };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle,eTargetDoor::TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_SHAKE_FIST:
        return new CTaskSimpleShakeFist{};
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver{ ped->m_pVehicle };
    case TASK_COMPLEX_SEEK_ENTITY: {
        // Como el original: se persigue el vehiculo del objetivo; sin vehiculo se le ataca a pie
        const auto targetVeh = m_rageWith->m_pVehicle;
        if (!targetVeh) {
            return new CTaskComplexKillPedOnFoot{ m_rageWith, -1, 0, 0, 0, true };
        }
        const float radius = targetVeh->GetModelInfo()->GetColModel()->GetBoundRadius();
        return new CTaskComplexSeekEntityStandard{
            targetVeh,
            20'000,
            1'000,
            radius + 1.f,
            2.f,
            2.f,
            true,
            true
        };
    }
    case TASK_COMPLEX_DESTROY_CAR:
        if (!m_rageWith->m_pVehicle) { // Como el original: sin vehiculo se ataca a pie
            return new CTaskComplexKillPedOnFoot{ m_rageWith, -1, 0, 0, 0, true };
        }
        return new CTaskComplexDestroyCar{ m_rageWith->m_pVehicle };
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return new CTaskComplexKillPedOnFoot{ m_rageWith, -1, 0, 0, 0, true };
    default: // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nullptr
        return nullptr;
    }
}

// 0x622170
bool CTaskComplexRoadRage::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x62E270
CTask* CTaskComplexRoadRage::CreateNextSubTask(CPed* ped) {
    if (!m_rageWith) {
        return nullptr;
    }
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        return CreateSubTask(TASK_SIMPLE_SHAKE_FIST, ped);
    case TASK_COMPLEX_DESTROY_CAR:
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return CreateSubTask(TASK_FINISHED, ped); // End task after destroying car or killing ped, seems like the ped will just stop and admire his work after doing either of those things
    case TASK_SIMPLE_SHAKE_FIST:
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(
            ped->bInVehicle // Ternary inverted
                ? TASK_FINISHED                     // Still in car, seems like we failed to get out, just end task
                : TASK_COMPLEX_TURN_TO_FACE_ENTITY, // Got out, so go and seek the target ped
            ped
        );
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: {
        if (ped->m_pVehicle) {
            ped->m_pVehicle->m_autoPilot.SetCarMission(MISSION_CRUISE);
            ped->m_pVehicle->m_autoPilot.SetCruiseSpeed(10);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY:
        return CreateSubTask(
            m_rageWith->bInVehicle && m_rageWith->m_pVehicle && !ped->IsCurrentlyUnarmed()
                ? TASK_COMPLEX_DESTROY_CAR
                : TASK_COMPLEX_KILL_PED_ON_FOOT,
            ped
        );
    default: // Como el original
        return nullptr;
    }
}

// 0x62E170
CTask* CTaskComplexRoadRage::CreateFirstSubTask(CPed* ped) {
    if (!m_rageWith || !ped->m_pVehicle || !ped->bInVehicle || m_rageWith->IsCop()) {
        return nullptr;
    }

    // Maybe give ped a weapon
    if (ped->IsGangster()) { // 0x62E1C6 (Inverted)
        ped->GiveDelayedWeapon(WEAPON_PISTOL, 2000u);
        ped->SetCurrentWeapon(WEAPON_PISTOL);
    } else if (CGeneral::GetRandomIntInRange(0, 100) < 50) { // 0x62E1DA - Como el original: 0x407180
        // Como el original: segundo numero de 0x407180, <= 33 bate, <= 66 palo de golf, si no pala
        const auto r = CGeneral::GetRandomIntInRange(0, 100);
        const auto wtype = r <= 33 ? WEAPON_BASEBALLBAT : r > 66 ? WEAPON_SHOVEL : WEAPON_GOLFCLUB;
        ped->GiveDelayedWeapon(wtype, 1);
        ped->SetCurrentWeapon(wtype);
    }

    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_18, ped);

    return CreateSubTask(m_rageWith->bInVehicle ? TASK_COMPLEX_LEAVE_CAR : TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
}

// 0x62E3F0
CTask* CTaskComplexRoadRage::ControlSubTask(CPed* ped) {
    const auto subTask = m_pSubTask;
    if (!m_rageWith) {
        subTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return subTask;
    }

    switch (subTask->GetTaskType()) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT: {
        if (ped->m_pVehicle) {
            // Como el original: (objetivo - ped), suma (z*z + y*y) + x*x; vuelve al coche si esta a mas de 20 m
            const auto& a = m_rageWith->GetPosition();
            const auto& b = ped->GetPosition();
            const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
            if (dz * dz + dy * dy + dx * dx > sq(20.f)) {
                return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
            }
        }
        break;
    }
    case TASK_COMPLEX_DESTROY_CAR: {
        if (!m_rageWith->bInVehicle) {
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
        }
        if (ped->m_pVehicle) {
            // Como el original: suma (x*x + z*z) + y*y; vuelve al coche si esta a mas de 20 m
            const auto& a = m_rageWith->GetPosition();
            const auto& b = ped->GetPosition();
            const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
            if (dx * dx + dz * dz + dy * dy > sq(20.f)) {
                return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
            }
        }
        break;
    }
    }

    return subTask;
}
