/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedOnFoot
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePause.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskComplexKillPedOnFootMelee.h"
#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexSignalAtPed.h"
#include "CarEnterExit.h"
#include "CullZones.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "CopPed.h"
#include "PlayerPedData.h"
#include "Wanted.h"
#include "General.h"
#include "EventAreaCodes.h"

void CTaskComplexKillPedOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFoot, 0x86D894, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x620E30);
    RH_ScopedInstall(CreateSubTask, 0x625E70);

    RH_ScopedVMTInstall(MakeAbortable, 0x625E40);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62B150);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62B490);
    RH_ScopedVMTInstall(ControlSubTask, 0x626260);
}

CTaskComplexKillPedOnFoot::CTaskComplexKillPedOnFoot(
    CPed* target,
    int32 time,
    int32 pedFlags,
    int32 delay,
    int32 chance,
    uint8 nCompetence,
    bool bWaitForPlayerToBeSafe,
    bool bWaitingForPlayerToBeSafe
) :
    m_bWaitForPlayerToBeSafe{ bWaitForPlayerToBeSafe },
    m_bWaitingForPlayerToBeSafe{ bWaitingForPlayerToBeSafe },
    m_target{ target },
    m_pedFlags{ pedFlags },
    m_actionDelay{ delay },
    m_actionChance{ chance },
    m_nCompetence{ nCompetence },
    m_time{ time },
    m_startTime{ CTimer::GetTimeInMS() }
{
    CEntity::SafeRegisterRef(m_target);
}

CTaskComplexKillPedOnFoot::~CTaskComplexKillPedOnFoot() {
    CEntity::SafeCleanUpRef(m_target);
}

CTaskComplexKillPedOnFoot* CTaskComplexKillPedOnFoot::Constructor(CPed* target, int32 time, int32 pedFlags, int32 delay, int32 chance, int8 a7) {
    this->CTaskComplexKillPedOnFoot::CTaskComplexKillPedOnFoot(target, time, pedFlags, delay, chance, a7);
    return this;
}

namespace {
// Como el original: reserva con CTask::operator new (0x61A5A0) y, si el pool esta agotado (nulo), no construye y devuelve nulo
template <class T, class... Args>
T* KillNewTask(Args... args) {
    void* const mem = CTask::operator new(sizeof(T));
    if (!mem) {
        return nullptr;
    }
    return ::new (mem) T(args...);
}

// Como el original (0x62B380..0x62B3CE y 0x62B591..0x62B5DB, mismo bloque en CreateNextSubTask y CreateFirstSubTask):
// si el objetivo es el jugador y su CPlayerPedData tiene el bit 0 del byte +0x34 (m_bStoppedMoving), y el ped es
// pandillero o policia y no fue creado por mision, recibe una pistola de 1000 balas y queda parado
void GiveGangsterPistolIfPlayerStopped(CPed* ped, CPed* targetPlayer) {
    if (!targetPlayer) {
        return;
    }
    if (!targetPlayer->GetPlayerData()->m_bStoppedMoving) {
        return;
    }
    const auto pedType = ped->m_nPedType;
    if (!IsPedTypeGang(pedType) && pedType != PED_TYPE_COP) {
        return;
    }
    if (ped->IsCreatedByMission()) { // cmp byte [ped+0x484], 2
        return;
    }
    ped->GiveWeapon(WEAPON_PISTOL, 1000, true);
    ped->SetCurrentWeapon(WEAPON_PISTOL);
    ped->SetMoveState(PEDMOVE_STILL);
}
}

// 0x625E40
bool CTaskComplexKillPedOnFoot::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    ped->bDontAcceptIKLookAts = false; // and [ped+0x474], 0xFFBFFFFF
    if (!m_pSubTask) {
        return true;
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x62B150
CTask* CTaskComplexKillPedOnFoot::CreateNextSubTask(CPed* ped) {
    if (!m_target) {
        return CreateSubTask(TASK_FINISHED, ped);
    }

    // Como el original: no se comprueba m_pSubTask (GetTaskType es la ranura +0x10 de la VMT)
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE: { // 0xCA
        if (ped->m_nPedType == PED_TYPE_COP && static_cast<CCopPed*>(ped)->m_bDontPursuit) { // byte [ped+0x79C]
            // Como el original: word [CWanted+0x1C] (m_ChanceOnRoadBlock), comparacion sin signo "ja"
            if (FindPlayerWanted(-1)->m_ChanceOnRoadBlock > 0) {
                return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED, ped);
            }
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        if (m_bWaitForPlayerToBeSafe && m_bWaitingForPlayerToBeSafe && !FindPlayerWanted(-1)->m_bEverybodyBackOff) {
            m_bWaitingForPlayerToBeSafe = false;
            return CreateFirstSubTask(ped); // llamada virtual (ranura +0x24)
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_DRAG_PED_FROM_CAR: { // 0x2BF
        // Como el original: byte [subtask+0x10] bit 2 (CTaskComplexEnterCar::m_bQuitAfterDraggingPedOut, protegido)
        const auto dragFlags = *(reinterpret_cast<const uint8*>(m_pSubTask) + 0x10);
        if (!(dragFlags & 4)) {
            if (ped->GetActiveWeapon().IsTypeMelee()) {
                return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE, ped);
            }
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED, ped);
        }
        return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x2C0
        CPed* targetPlayer = nullptr;
        if (m_target->IsPlayer()) {
            targetPlayer = m_target;
        }

        if (ped->bInVehicle) {
            return CreateSubTask(TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
        }

        // Como el original: delta = objetivo - ped, en floats; distancia = sqrt((dz*dz + dy*dy) + dx*dx)
        const CVector& pedPos    = ped->GetPosition();
        const CVector& targetPos = m_target->GetPosition();
        const float dx = targetPos.x - pedPos.x;
        const float dy = targetPos.y - pedPos.y;
        const float dz = targetPos.z - pedPos.z;

        // Como el original: NoPolice() primero; si no, hay que estar sobre otra entidad distinta y a distancia < 5.0
        // (comparacion ordenada: con NaN es false)
        if (CCullZones::NoPolice()
            || (m_target->m_standingOnEntity
                && m_target->m_standingOnEntity != ped->m_standingOnEntity
                && sqrtf(dz * dz + dy * dy + dx * dx) < 5.0f)
        ) {
            GiveGangsterPistolIfPlayerStopped(ped, targetPlayer);
        }

        // Como el original: la ranura activa se vuelve a leer tras GiveWeapon/SetCurrentWeapon
        if (ped->GetActiveWeapon().IsTypeMelee()) {
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE, ped);
        }
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED, ped);
    }
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x2CF
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    case TASK_NONE:                           // 0xC8 (CTaskSimpleLeaveGroup)
    case TASK_SIMPLE_STAND_STILL:             // 0xCB
    case TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE: // 0x3E9
    case TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED: // 0x3EA
    case TASK_COMPLEX_DESTROY_CAR:            // 0x3EB
        return CreateSubTask(TASK_FINISHED, ped);
    default:
        return nullptr;
    }
}

// 0x62B490
CTask* CTaskComplexKillPedOnFoot::CreateFirstSubTask(CPed* ped) {
    // Como el original: se escribe el byte de banderas (and FD / or 04) antes de comprobar el objetivo
    m_bNewTarget = false;
    m_bRoomToDragPedOutOfCar = true;

    if (!m_target) {
        ped->bDontAcceptIKLookAts = false;
        return nullptr;
    }

    // Como el original: comparacion x87 ordenada (con NaN es false)
    if (m_target->m_fHealth <= 0.0f) {
        m_bTargetKilled = true;
    }

    // Como el original: delta = objetivo - ped; distancia = sqrt((dz*dz + dy*dy) + dx*dx)
    const CVector& pedPos    = ped->GetPosition();
    const CVector& targetPos = m_target->GetPosition();
    const float dx = targetPos.x - pedPos.x;
    const float dy = targetPos.y - pedPos.y;
    const float dz = targetPos.z - pedPos.z;
    const float dist = sqrtf(dz * dz + dy * dy + dx * dx);

    CPed* targetPlayer = nullptr;
    if (m_target->IsPlayer()) {
        targetPlayer = m_target;
    }

    // Tramo final comun (0x62B76A): crea la subtarea y suelta lo que el ped lleve en la mano
    const auto Finish = [&](int32 taskId) -> CTask* {
        CTask* const subTask = CreateSubTask(taskId, ped);
        ped->DropEntityThatThisPedIsHolding(true);
        return subTask;
    };

    if (ped->m_pVehicle && ped->bInVehicle) {
        return Finish(TASK_COMPLEX_LEAVE_CAR);
    }

    if (targetPlayer) {
        if (CCullZones::NoPolice()
            || (m_target->m_standingOnEntity
                && m_target->m_standingOnEntity != ped->m_standingOnEntity
                && dist < 5.0f) // comparacion ordenada: con NaN es false
        ) {
            GiveGangsterPistolIfPlayerStopped(ped, targetPlayer);
        }
    }

    if (m_target->bInVehicle && m_target->m_pVehicle) {
        CVehicle* veh = m_target->m_pVehicle;
        if (veh->IsDriver(m_target) || veh->IsPassenger(m_target)) {
            m_bRoomToDragPedOutOfCar = true;

            veh = m_target->m_pVehicle;
            const float vx = veh->m_vecMoveSpeed.x;
            const float vy = veh->m_vecMoveSpeed.y;

            const auto subType = veh->m_nVehicleSubType; // +0x594
            if (subType != VEHICLE_TYPE_HELI && subType != VEHICLE_TYPE_PLANE && !ped->bStayInSamePlace) {
                // Como el original: sqrt(y*y + x*x) > 0.1 (ordenada) manda a destruir el coche; con NaN sigue adelante
                if (!(sqrtf(vy * vy + vx * vx) > 0.1f) && veh->CanPedOpenLocks(ped)) {
                    // Como el original: en moto (+0x590 == 9) solo si el ped lleva arma cuerpo a cuerpo
                    if (veh->m_nVehicleType != VEHICLE_TYPE_BIKE || ped->GetActiveWeapon().IsTypeMelee()) {
                        if (m_target->m_pVehicle->m_nVehicleType != VEHICLE_TYPE_BOAT) {
                            const auto door = CCarEnterExit::ComputeTargetDoorToExit(m_target->m_pVehicle, m_target);
                            m_bRoomToDragPedOutOfCar = CCarEnterExit::IsRoomForPedToLeaveCar(m_target->m_pVehicle, door, nullptr);
                            if (m_bRoomToDragPedOutOfCar) {
                                if (m_target->bDontDragMeOutCar) {
                                    return Finish(TASK_COMPLEX_DESTROY_CAR);
                                }
                                return Finish(TASK_COMPLEX_DRAG_PED_FROM_CAR);
                            }
                        }
                    }
                }
            }
            return Finish(TASK_COMPLEX_DESTROY_CAR);
        }
    }

    if (ped->GetActiveWeapon().IsTypeMelee()) {
        return Finish(TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE);
    }
    return Finish(TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED);
}

// 0x626260
CTask* CTaskComplexKillPedOnFoot::ControlSubTask(CPed* ped) {
    ped->bDontAcceptIKLookAts = true; // or [ped+0x474], 0x400000

    // Tramo final comun (0x6269D3): aviso de cambio de codigo de area y cambio de subtarea
    const auto Finish = [&](int32 newTaskId) -> CTask* {
        if (m_target) {
            // Como el original: +0x584 = m_pContactEntity, byte +0x2F = codigo de area
            CEntity* const targetContact = m_target->m_pContactEntity;
            if (targetContact) {
                CEntity* const pedContact = ped->m_pContactEntity;
                if (pedContact && targetContact->GetAreaCode() != pedContact->GetAreaCode()) {
                    CEventAreaCodes event{ m_target };
                    ped->GetIntelligence()->GetEventGroup().Add(static_cast<CEvent*>(&event), false);
                }
            }
        }

        if (newTaskId != TASK_NONE) {
            if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                return CreateSubTask(newTaskId, ped);
            }
        }

        if (m_target) {
            plugin::Call<0x65E9A0, CPed*, CPed*>(ped, m_target); // sin reconstruir
        }
        return m_pSubTask;
    };

    if (!m_target) {
        return Finish(TASK_FINISHED);
    }

    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_PAUSE) {
        if (m_bWaitForPlayerToBeSafe && m_bWaitingForPlayerToBeSafe && !FindPlayerWanted(-1)->m_bEverybodyBackOff) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        }
        return m_pSubTask;
    }

    // Tiempo agotado. Como el original: suma sin signo m_startTime + m_time y comparacion sin signo
    if (m_time > 0 && CTimer::GetTimeInMS() > m_startTime + (uint32)m_time) {
        if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return Finish(TASK_NONE);
        }
        return KillNewTask<CTaskSimpleStandStill>(0, false, false, 8.0f);
    }

    if (m_bNewTarget) {
        if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return Finish(TASK_NONE);
        }
        return CreateFirstSubTask(ped); // llamada virtual (ranura +0x24)
    }

    // CWanted del jugador objetivo. Como el original: el resultado se desreferencia sin comprobar nulos
    const auto GetTargetWanted = [&]() -> CWanted* {
        CPlayerPedData* const playerData = m_target->GetPlayerData();
        return playerData ? playerData->m_pWanted : nullptr;
    };

    // Comprueba si hay sitio para sacar al objetivo y actualiza el bit 2 de las banderas
    const auto UpdateRoomToDragOut = [&]() -> bool {
        const auto door = CCarEnterExit::ComputeTargetDoorToExit(m_target->m_pVehicle, m_target);
        m_bRoomToDragPedOutOfCar = CCarEnterExit::IsRoomForPedToLeaveCar(m_target->m_pVehicle, door, nullptr);
        return m_bRoomToDragPedOutOfCar;
    };

    int32 newTaskId = TASK_NONE; // ebx = 0xC8: sin cambio
    bool  runSwitch = true;

    if (!m_bTargetKilled) {
        // Como el original: comparacion x87 ordenada; con NaN no se toma la rama de objetivo muerto
        if (m_target->m_fHealth <= 0.0f) {
            if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                m_bTargetKilled = true;
                if (m_pSubTask->GetTaskType() == TASK_COMPLEX_SIGNAL_AT_PED) {
                    return m_pSubTask;
                }
                if (ped->m_nPedType == PED_TYPE_COP && !m_target->IsPlayer()) {
                    ped->Say((eGlobalSpeechContext)0xE, 0, 1.0f, false, false, false);
                }
                if (ped->bSignalAfterKill) { // bit 12 del dword +0x478
                    return CreateSubTask(TASK_COMPLEX_SIGNAL_AT_PED, ped);
                }
                return KillNewTask<CTaskSimpleLeaveGroup>();
            }
            // Como el original: si no se puede abortar sigue con el tratamiento normal
        } else if (m_target->IsPlayer()) {
            bool backOff = GetTargetWanted()->m_bEverybodyBackOff; // byte [CWanted+0x1E] bit 2
            if (!backOff) {
                // Como el original: policia con m_bDontPursuit y word [CWanted+0x1C] == 0
                backOff = ped->m_nPedType == PED_TYPE_COP
                    && static_cast<CCopPed*>(ped)->m_bDontPursuit
                    && FindPlayerWanted(-1)->m_ChanceOnRoadBlock == 0;
            }
            if (backOff && m_pSubTask->GetTaskType() != TASK_SIMPLE_PAUSE) {
                newTaskId = TASK_SIMPLE_PAUSE;
                if (m_bWaitForPlayerToBeSafe && GetTargetWanted()->m_bEverybodyBackOff) {
                    m_bWaitingForPlayerToBeSafe = true; // or cl, 0x40
                }
                runSwitch = false;
            }
        }
    }

    if (runSwitch) {
        switch (m_pSubTask->GetTaskType()) { // 0x6264CD
        case TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE: { // 0x3E9
            static_cast<CTaskComplexKillPedOnFootMelee*>(m_pSubTask)->m_bShotFiredByPlayerFlag = m_bShotFiredByPlayerFlag;
            if (!ped->GetActiveWeapon().IsTypeMelee()) {
                newTaskId = TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED;
                break;
            }

            const bool targetInVehicle = m_target->bInVehicle;
            if (targetInVehicle) {
                // Como el original: aqui no se comprueba m_pVehicle
                const auto subType = m_target->m_pVehicle->m_nVehicleSubType;
                if (subType == VEHICLE_TYPE_PLANE || subType == VEHICLE_TYPE_HELI) {
                    newTaskId = TASK_COMPLEX_DESTROY_CAR;
                    break;
                }
            }
            if (!targetInVehicle) {
                break;
            }
            CVehicle* const veh = m_target->m_pVehicle;
            if (!veh) {
                break;
            }
            if (veh->m_nVehicleType == VEHICLE_TYPE_BOAT) {
                break;
            }
            if (ped->bStayInSamePlace) {
                break;
            }
            if (!veh->CanPedOpenLocks(ped)) {
                break;
            }
            newTaskId = UpdateRoomToDragOut() ? TASK_COMPLEX_DRAG_PED_FROM_CAR : TASK_COMPLEX_DESTROY_CAR;
            break;
        }
        case TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED: { // 0x3EA
            static_cast<CTaskComplexKillPedOnFootArmed*>(m_pSubTask)->m_bShotFiredByPlayer = m_bShotFiredByPlayerFlag;

            if (ped->GetActiveWeapon().IsTypeMelee() && !ped->IsPlayer()) {
                newTaskId = TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE;
            } else if (ped->GetActiveWeapon().m_TotalAmmo == 0 && !ped->IsPlayer()) {
                // Como el original: busca la primera ranura con municion (con signo, > 0)
                size_t slot = 0;
                for (; slot < NUM_WEAPON_SLOTS; slot++) {
                    if ((int32)ped->GetWeaponInSlot(slot).m_TotalAmmo > 0) {
                        break;
                    }
                }
                if (slot < NUM_WEAPON_SLOTS) {
                    ped->SetCurrentWeapon((int32)slot);
                } else {
                    ped->SetCurrentWeapon(WEAPON_UNARMED);
                    newTaskId = TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE;
                }
            }

            if (!m_target->bInVehicle) {
                break;
            }
            CVehicle* veh = m_target->m_pVehicle;
            if (!veh) {
                break;
            }
            if (!veh->IsDriver(m_target) && !veh->IsPassenger(m_target)) {
                break;
            }
            veh = m_target->m_pVehicle;
            const float vx = veh->m_vecMoveSpeed.x;
            const float vy = veh->m_vecMoveSpeed.y;
            const auto  subType = veh->m_nVehicleSubType;
            if (subType == VEHICLE_TYPE_PLANE || subType == VEHICLE_TYPE_HELI) {
                newTaskId = TASK_COMPLEX_DESTROY_CAR;
                break;
            }
            if (veh->m_nVehicleType == VEHICLE_TYPE_BIKE || veh->m_nVehicleType == VEHICLE_TYPE_BOAT) {
                break;
            }
            if (ped->bStayInSamePlace) {
                break;
            }
            // Como el original: x*x + y*y <= 0.1 ordenada (con NaN no continua)
            if (!(sqrtf(vx * vx + vy * vy) <= 0.1f)) {
                break;
            }
            if (!veh->CanPedOpenLocks(ped)) {
                break;
            }
            newTaskId = UpdateRoomToDragOut() ? TASK_COMPLEX_DRAG_PED_FROM_CAR : TASK_COMPLEX_DESTROY_CAR;
            break;
        }
        case TASK_COMPLEX_DRAG_PED_FROM_CAR: { // 0x2BF
            if (!m_target->bInVehicle) {
                break;
            }
            CVehicle* veh = m_target->m_pVehicle;
            if (!veh) {
                break;
            }
            if (!veh->IsDriver(m_target) && !veh->IsPassenger(m_target)) {
                break;
            }
            veh = m_target->m_pVehicle;
            const float vx = veh->m_vecMoveSpeed.x;
            const float vy = veh->m_vecMoveSpeed.y;
            if (sqrtf(vx * vx + vy * vy) > 0.1f) { // ordenada: con NaN no se toma
                newTaskId = TASK_COMPLEX_DESTROY_CAR;
                break;
            }
            if (!m_timer.IsOutOfTime()) {
                break;
            }
            veh = m_target->m_pVehicle;
            if (!veh) {
                break;
            }
            // Como el original: aqui se prueba primero IsPassenger y luego IsDriver
            if (!veh->IsPassenger(m_target) && !veh->IsDriver(m_target)) {
                break;
            }
            if (!UpdateRoomToDragOut()) {
                newTaskId = TASK_COMPLEX_DESTROY_CAR;
            }
            m_timer.Start(2000); // 0x6266C1
            break;
        }
        case TASK_COMPLEX_DESTROY_CAR: { // 0x3EB
            if (!m_target->bInVehicle) {
                newTaskId = ped->GetActiveWeapon().IsTypeMelee() ? TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE : TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED;
                break;
            }
            if (m_target->m_pVehicle && !CCarEnterExit::IsVehicleHealthy(m_target->m_pVehicle)) {
                newTaskId = TASK_COMPLEX_SIGNAL_AT_PED;
                break;
            }
            if (m_target->bDontDragMeOutCar) {
                break;
            }
            // Como el original: no se comprueba m_pVehicle a partir de aqui
            CVehicle* const veh = m_target->m_pVehicle;
            const float vx = veh->m_vecMoveSpeed.x;
            const float vy = veh->m_vecMoveSpeed.y;
            const auto  subType = veh->m_nVehicleSubType;
            if (subType == VEHICLE_TYPE_PLANE || subType == VEHICLE_TYPE_HELI) {
                break;
            }
            if (veh->m_nVehicleType == VEHICLE_TYPE_BIKE || veh->m_nVehicleType == VEHICLE_TYPE_BOAT) {
                break;
            }
            if (ped->bStayInSamePlace) {
                break;
            }
            if (!(sqrtf(vx * vx + vy * vy) <= 0.1f)) {
                break;
            }
            if (!veh->CanPedOpenLocks(ped)) {
                break;
            }
            if (m_bRoomToDragPedOutOfCar) { // test byte [this+0xC], 4
                newTaskId = TASK_COMPLEX_DRAG_PED_FROM_CAR;
                break;
            }
            if (!m_timer.IsOutOfTime()) {
                break;
            }
            if (UpdateRoomToDragOut()) {
                newTaskId = TASK_COMPLEX_DRAG_PED_FROM_CAR;
            }
            m_timer.Start(2000); // 0x6266C1
            break;
        }
        default:
            break;
        }
    }

    m_bShotFiredByPlayerFlag = false; // 0x6269CF: and byte [this+0xC], 0xFE
    return Finish(newTaskId);
}

// 0x625E70
CTask* CTaskComplexKillPedOnFoot::CreateSubTask(int32 taskId, CPed* ped) {
    switch (taskId) {
    case TASK_NONE: // 0xC8
        return KillNewTask<CTaskSimpleLeaveGroup>();
    case TASK_SIMPLE_PAUSE: { // 0xCA
        // Como el original: se construye un CTaskSimpleStandStill temporal (0, false, false, 8.0) y se ejecuta su
        // ProcessPed solo por sus efectos sobre el ped; el resultado se descarta
        CTaskSimpleStandStill standStill{ 0, false, false, 8.0f };
        standStill.ProcessPed(ped);
        if (m_bWaitForPlayerToBeSafe && m_bWaitingForPlayerToBeSafe) {
            return KillNewTask<CTaskSimplePause>(10000);
        }
        return KillNewTask<CTaskSimplePause>(2000);
    }
    case TASK_SIMPLE_STAND_STILL: // 0xCB
        return KillNewTask<CTaskSimpleStandStill>(0, false, false, 8.0f);
    case TASK_COMPLEX_DRAG_PED_FROM_CAR: { // 0x2BF
        CTask* const task = KillNewTask<CTaskComplexDragPedFromCar>(m_target, 0);
        m_timer.Start(2000); // Como el original: el temporizador se reinicia aunque falle el pool
        return task;
    }
    case TASK_COMPLEX_LEAVE_CAR: // 0x2C0
        return KillNewTask<CTaskComplexLeaveCar>(ped->m_pVehicle, 0, 0, true, true);
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x2CF
        return KillNewTask<CTaskSimpleCarDriveTimed>(ped->m_pVehicle, 2000);
    case TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE: { // 0x3E9
        CTask* const task = KillNewTask<CTaskComplexKillPedOnFootMelee>(m_target);
        m_timer.Start(2000);
        return task;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED: { // 0x3EA
        auto* const task = KillNewTask<CTaskComplexKillPedOnFootArmed>(m_target, m_pedFlags, m_actionDelay, m_actionChance, (int8)m_nCompetence);
        // El EXE escribe +0x35 sin comprobar nulo (se caeria con el pool lleno); aqui se protege
        if (task) {
            task->m_aimImmediate = m_bAimImmediate;
        }
        m_bAimImmediate = false;
        m_timer.Start(2000);
        return task;
    }
    case TASK_COMPLEX_DESTROY_CAR: // 0x3EB
        return KillNewTask<CTaskComplexDestroyCar>(m_target->m_pVehicle, 0, 0, 0);
    case TASK_COMPLEX_SIGNAL_AT_PED: { // 0x4BA
        if (m_target == FindPlayerPed(0)) {
            ped->Say((eGlobalSpeechContext)0x103, 0, 1.0f, false, false, false);
        } else {
            // Como el original: GetPedsGroup se llama dos veces (la segunda para obtener el lider)
            bool skipSay = false;
            if (CPedGroups::GetPedsGroup(m_target)) {
                CPed* const leader = CPedGroups::GetPedsGroup(m_target)->GetMembership().GetLeader();
                skipSay = leader == FindPlayerPed(0);
            }
            if (!skipSay) {
                ped->Say((eGlobalSpeechContext)0x102, 0, 1.0f, false, false, false);
            }
        }
        // Como el original: el numero aleatorio se obtiene antes de reservar la tarea
        const int32 delay = CGeneral::GetRandomIntInRange(0, 1500);
        return KillNewTask<CTaskComplexSignalAtPed>(m_target, delay, true);
    }
    case TASK_FINISHED: // 0x516
        ped->bDontAcceptIKLookAts = false;
        return nullptr;
    default:
        return nullptr;
    }
}
