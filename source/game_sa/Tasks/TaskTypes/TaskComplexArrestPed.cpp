/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexArrestPed
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "extensions/utility.hpp"

#include "TaskComplexArrestPed.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"
#include "TaskSimpleArrestPed.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexSeekEntityStandard.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskComplexOpenDriverDoor.h"
#include "TaskComplexOpenPassengerDoor.h"

#include "eTargetDoor.h"
#include "CarEnterExit.h"

void CTaskComplexArrestPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexArrestPed, 0x8709A8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68B990);
    RH_ScopedInstall(Destructor, 0x68BA00);
    RH_ScopedVMTInstall(MakeAbortable, 0x68BA60);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x690220);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6907A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x68D350);
    RH_ScopedInstall(CreateSubTask, 0x68CF80);
}

// 0x68B990
CTaskComplexArrestPed::CTaskComplexArrestPed(CPed* ped) : CTaskComplex() {
    m_PedToArrest = ped;
    m_Vehicle = nullptr;
    CEntity::SafeRegisterRef(m_PedToArrest);
}

// 0x68BA00
CTaskComplexArrestPed::~CTaskComplexArrestPed() {
    CEntity::SafeCleanUpRef(m_PedToArrest);
}

// 0x68BA60


bool CTaskComplexArrestPed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x690220 See #gists in discord


CTask* CTaskComplexArrestPed::CreateNextSubTask(CPed* ped) {
    if (!m_PedToArrest) return CreateSubTask(TASK_FINISHED, ped);
    const auto flag = [&](size_t offset, uint8 mask) {
        return (*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(m_pSubTask) + offset) & mask) != 0;
    };
    if (m_PedToArrest->bIsBeingArrested && m_pSubTask->GetTaskType() != TASK_SIMPLE_ARREST_PED) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_SEEK_ENTITY) {
            return CreateSubTask(flag(0x48, 4) ? TASK_SIMPLE_ARREST_PED : TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    }
    const auto closeEnough = [&] {
        auto delta = m_PedToArrest->GetPosition() - ped->GetPosition();
        const auto z = std::abs(delta.z);
        delta.z = 0.0f;
        return z <= 2.0f && delta.SquaredMagnitude() <= 9.0f;
    };
    const auto fallingTask = [&] {
        return static_cast<CTaskComplexFallAndGetUp*>(m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_FALL_AND_GET_UP));
    };
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR:
    case TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR:
        if (flag(0x10, 2) && m_PedToArrest->m_pVehicle && !m_PedToArrest->m_pVehicle->CanPedOpenLocks(ped)) {
            m_Vehicle = m_PedToArrest->m_pVehicle;
        }
        if (!m_PedToArrest->IsAlive()) return CreateSubTask(TASK_SIMPLE_ARREST_PED, ped);
        if (!m_PedToArrest->bInVehicle || flag(0x10, 2)) return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
        return CreateSubTask(m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR) ?
            TASK_COMPLEX_KILL_PED_ON_FOOT : TASK_SIMPLE_ARREST_PED, ped);
    case TASK_COMPLEX_DRAG_PED_FROM_CAR:
        if (auto* falling = fallingTask(); falling && falling->IsFalling() && !flag(0x10, 4) && closeEnough()) {
            falling->SetDownTime(100000);
            return CreateSubTask(TASK_SIMPLE_ARREST_PED, ped);
        }
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    case TASK_COMPLEX_SEEK_ENTITY:
        if (auto* falling = fallingTask(); falling && falling->IsFalling() && flag(0x48, 4) && closeEnough()) {
            falling->SetDownTime(100000);
            return CreateSubTask(TASK_SIMPLE_ARREST_PED, ped);
        }
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR:
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    case TASK_SIMPLE_ARREST_PED:
    case TASK_COMPLEX_DESTROY_CAR:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        if (m_PedToArrest->m_fHealth <= 0.0f) return CreateSubTask(TASK_SIMPLE_ARREST_PED, ped);
        if (auto* falling = fallingTask(); falling && falling->IsFalling()) {
            if (closeEnough()) {
                falling->SetDownTime(100000);
                return CreateSubTask(TASK_SIMPLE_ARREST_PED, ped);
            }
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        if (ped->m_nPedType != PED_TYPE_COP && m_PedToArrest->IsPlayer() && m_PedToArrest->GetPlayerData()->m_pWanted->m_NumCopsInPursuit > 0) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    default:
        return nullptr;
    }
}

// NOTSA
void MakeSurePedHasWeaponInHand(CPed* ped) {
    // Make sure ped has an actual weapon in their hand
    if (!ped->GetActiveWeapon().IsTypeMelee())
        return;

    if (ped->DoWeHaveWeaponAvailable(WEAPON_SHOTGUN)) { // Use shotgun (if available)
        ped->SetCurrentWeapon(WEAPON_SHOTGUN);
        return;
    }

    // Otherwise a pistol
    if (!ped->DoWeHaveWeaponAvailable(WEAPON_PISTOL)) { // Make sure they have one
        ped->GiveWeapon(WEAPON_PISTOL, 10, false);
    }
    ped->SetCurrentWeapon(WEAPON_PISTOL);
}

// 0x6907A0


CTask* CTaskComplexArrestPed::CreateFirstSubTask(CPed* ped) {
    if (!m_PedToArrest) {
        return nullptr;
    }

    m_bSubTaskNeedsToBeCreated = false;

    if (!m_PedToArrest->bInVehicle) {
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    }

    if (m_PedToArrest->m_pVehicle->IsBike() || m_PedToArrest->m_pVehicle->IsSubQuad()) { // Just drag ped from a bike/quad
        return CreateSubTask(TASK_COMPLEX_DRAG_PED_FROM_CAR, ped);
    }

    if (m_PedToArrest->m_pVehicle->IsSubBoat()) { // If they're in a boat, just destroy it
        MakeSurePedHasWeaponInHand(ped);
        return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
    } else {
        if (m_PedToArrest->m_pVehicle->IsUpsideDown() || m_PedToArrest->m_pVehicle->IsOnItsSide()) {
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        return CreateSubTask(TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR, ped);
    }
}

// 0x68D350

// 0x0
CTask* CTaskComplexArrestPed::ControlSubTask(CPed* ped) {
    auto requested = TASK_NONE;
    CTask* result = m_pSubTask;
    const auto destroyCar = [&] {
        if (ped->GetActiveWeapon().IsTypeMelee()) {
            ped->SetCurrentWeapon(ped->DoWeHaveWeaponAvailable(WEAPON_SHOTGUN) ? WEAPON_SHOTGUN : WEAPON_PISTOL);
        }
        requested = TASK_COMPLEX_DESTROY_CAR;
    };
    if (!m_PedToArrest || m_PedToArrest->m_fHealth <= 0.0f) {
        requested = TASK_FINISHED;
    } else if (m_bSubTaskNeedsToBeCreated) {
        if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            result = CreateFirstSubTask(ped);
        }
    } else {
        if (m_PedToArrest->bIsBeingArrested && m_pSubTask->GetTaskType() != TASK_SIMPLE_ARREST_PED &&
            m_pSubTask->GetTaskType() != TASK_COMPLEX_SEEK_ENTITY) {
            requested = TASK_COMPLEX_SEEK_ENTITY;
        } else {
            const auto type = m_pSubTask->GetTaskType();
            switch (type) {
            case TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR:
            case TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR: {
                const bool driver = type == TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR;
                const auto leaving = m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR);
                const auto distance = (m_PedToArrest->GetPosition() - ped->GetPosition()).SquaredMagnitude();
                if (leaving && m_PedToArrest->bInVehicle && distance < 25.0f) {
                    requested = TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR;
                } else if (!m_PedToArrest->bInVehicle) {
                    requested = TASK_COMPLEX_KILL_PED_ON_FOOT;
                } else if (!CCarEnterExit::IsRoomForPedToLeaveCar(m_PedToArrest->m_pVehicle, driver ? 10 : 8, nullptr)) {
                    if (CCarEnterExit::IsRoomForPedToLeaveCar(m_PedToArrest->m_pVehicle, driver ? 8 : 10, nullptr)) {
                        requested = driver ? TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR : TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR;
                    } else if (!driver) {
                        destroyCar();
                    }
                }
                break;
            }
            case TASK_COMPLEX_DRAG_PED_FROM_CAR:
            case TASK_COMPLEX_DESTROY_CAR:
                if (!m_PedToArrest->bInVehicle) requested = TASK_COMPLEX_KILL_PED_ON_FOOT;
                break;
            case TASK_COMPLEX_KILL_PED_ON_FOOT: {
                auto* falling = static_cast<CTaskComplexFallAndGetUp*>(m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_FALL_AND_GET_UP));
                if (falling && falling->IsFalling()) {
                    auto delta = ped->GetPosition() - m_PedToArrest->GetPosition();
                    const auto z = std::abs(delta.z);
                    delta.z = 0.0f;
                    if (z <= 2.0f && delta.SquaredMagnitude() <= 9.0f) {
                        falling->SetDownTime(100000);
                        requested = TASK_SIMPLE_ARREST_PED;
                    } else {
                        requested = TASK_COMPLEX_SEEK_ENTITY;
                    }
                    break;
                }
                auto* veh = m_PedToArrest->m_pVehicle;
                if (!m_PedToArrest->bInVehicle || !veh) break;
                if (int(veh->m_nVehicleType) == 5 || int(veh->m_nVehicleSubType) == 4 || int(veh->m_nVehicleSubType) == 3) {
                    destroyCar();
                } else if (!ped->GetActiveWeapon().IsTypeMelee() && !FindPlayerWanted()->IsClosestCop(ped->AsCop(), 2)) {
                    requested = TASK_COMPLEX_DESTROY_CAR;
                } else if (veh != m_Vehicle && !m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
                    if (int(veh->m_nVehicleType) == 9 || int(veh->m_nVehicleSubType) == 2) {
                        requested = TASK_COMPLEX_DRAG_PED_FROM_CAR;
                    } else if (veh->IsUpsideDown() || veh->IsOnItsSide()) {
                        requested = TASK_COMPLEX_DESTROY_CAR;
                    } else {
                        requested = TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR;
                    }
                }
                break;
            }
            default: break;
            }
        }
    }
    if (m_PedToArrest && m_PedToArrest->IsPlayer()) {
        if (FindPlayerWanted()->m_NumCopsInPursuit == 1) {
            ped->Say(CTX_GLOBAL_SOLO);
        } else {
            FindPlayerWanted();
        }
    }
    if (requested != TASK_NONE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return CreateSubTask(requested, ped);
    }
    return result;
}

// 0x68CF80
CTask* CTaskComplexArrestPed::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ARREST_PED:
        if (m_PedToArrest->m_pVehicle) {
            if (m_PedToArrest->m_pVehicle->IsDriver(m_PedToArrest)) {
                m_PedToArrest->m_pVehicle->vehicleFlags.bIsHandbrakeOn = true;
                m_PedToArrest->m_pVehicle->SetStatus(STATUS_FORCED_STOP);
            }
        }
        return new CTaskSimpleArrestPed(m_PedToArrest);

    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return new CTaskComplexKillPedOnFoot(m_PedToArrest, -1, 0, 0, 0, 1);

    case TASK_COMPLEX_DESTROY_CAR:
        return new CTaskComplexDestroyCar(m_PedToArrest->m_pVehicle, 0, 0, 0);

    case TASK_COMPLEX_SEEK_ENTITY: {
        float radius = m_PedToArrest->bIsBeingArrested ? 4.0f : 3.0f;
        return new CTaskComplexSeekEntityStandard(m_PedToArrest, 50'000, 1000, radius, 2.0f, 2.0f, true, true);
    }
    case TASK_COMPLEX_DRAG_PED_FROM_CAR:
        return new CTaskComplexDragPedFromCar(m_PedToArrest, 100'000);

    case TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR:
        return new CTaskComplexOpenDriverDoor(m_PedToArrest->m_pVehicle);

    case TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR:
        return new CTaskComplexOpenPassengerDoor(m_PedToArrest->m_pVehicle, 8); // todo: magic number

    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR: {
        return new CTaskSimpleWaitUntilPedIsOutCar{m_PedToArrest, m_PedToArrest->GetPosition() - ped->GetPosition()};
    }
    default:
        return nullptr;
    }
}
