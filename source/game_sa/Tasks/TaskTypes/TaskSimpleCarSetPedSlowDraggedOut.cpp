/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedSlowDraggedOut
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
#include "TaskSimpleCarSetPedSlowDraggedOut.h"
#include "TaskSimplePlayerOnFoot.h"
#include <TaskComplexWander.h>
#include <TaskSimpleStandStill.h>
#include <PedPlacement.h>

void CTaskSimpleCarSetPedSlowDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedSlowDraggedOut, 0x86EF30, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6483B0);
    RH_ScopedInstall(Destructor, 0x648430);

    RH_ScopedVMTInstall(Clone, 0x64A0B0);
    RH_ScopedVMTInstall(GetTaskType, 0x648410);
    RH_ScopedVMTInstall(MakeAbortable, 0x648420);
    RH_ScopedVMTInstall(ProcessPed, 0x64C220);
}

// 0x6483B0
CTaskSimpleCarSetPedSlowDraggedOut::CTaskSimpleCarSetPedSlowDraggedOut(CVehicle* target, eTargetDoor door) :
    m_Vehicle{target},
    m_Door{door}
{}

// 0x64C220
bool CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed(CPed* ped) {
    if (!ped->m_pVehicle || !m_Vehicle) {
        return true;
    }

    // Como el original: se calcula el flag de la puerta y el resultado se descarta (0x64C266, sin efectos)

    // Remove ped as passenger/driver from the vehicle
    if (m_Vehicle->m_pDriver == ped) { // 0x64C27C
        m_Vehicle->RemoveDriver(true);
        if (m_Vehicle->m_nDoorLock == CARLOCK_COP_CAR) {
            m_Vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
        }
        if (ped->IsCop() && m_Vehicle->IsLawEnforcementVehicle()) {
            m_Vehicle->ChangeLawEnforcerState(false);
        }
    } else { // 0x64C2B4
        // Como el original: recorre m_nMaxPassengers (+0x488, releido en cada vuelta) y sigue tras quitarlo
        for (auto i = 0; i < (int32)m_Vehicle->m_nMaxPassengers; i++) {
            if (m_Vehicle->m_apPassengers.data()[i] == ped) {
                m_Vehicle->RemovePassenger(ped);
            }
        }
    }

    // Ped's not in a vehicle anymore (But don't clear `m_pVehicle` yet!)
    ped->bInVehicle = false;

    // Set default task after setting the ped out
    {
        CTask* task;
        if (ped->IsPlayer()) {
            task = new CTaskSimplePlayerOnFoot{}; // 0x64C320
        } else if (ped->IsCreatedBy(PED_MISSION)) { // Como el original: m_nCreatedBy (+0x484), no m_nPedType
            task = new CTaskSimpleStandStill{999'999, true}; // 0x64C355
        } else {
            task = CTaskComplexWander::GetWanderTaskByPedType(ped); // 0x64C378
        }
        ped->GetTaskManager().SetTask(task, TASK_PRIMARY_DEFAULT);
    }

    // 0x64C38F
    ped->RestartNonPartialAnims();
    ped->SetUsesCollision(true);
    ped->UpdateStatLeavingVehicle();
    ped->ReplaceWeaponWhenExitingVehicle();

    // 0x64C3D0 - Position ped out of the vehicle
    const auto getUpOffset = CCarEnterExit::ms_vecPedGetUpAnimOffset;
    const auto doorOffset = m_Vehicle->GetAnimGroup().ComputeAnimDoorOffsets(
        (m_Door == TARGET_DOOR_FRONT_RIGHT || m_Door == TARGET_DOOR_REAR_RIGHT)
            ? JACK_PED_RIGHT
            : JACK_PED_LEFT
    );
    const auto offset = CVector{doorOffset.x - getUpOffset.x, doorOffset.y - getUpOffset.y, 0.f};
    auto pos = ped->GetMatrix().TransformPoint(offset);
    pos = CPedPlacement::FindZCoorForPed(pos).first; // Como el original: la Z se corrige en el mismo vector (0x616920)

    // Como el original: la velocidad se anula despues de buscar la Z y antes de colocar al peaton
    ped->ResetMoveSpeed();
    ped->SetPosn(pos);

    // 0x64C50B - Reset rotation
    // Como el original: se usa el rumbo del vehiculo (fpatan del x87), no el del peaton
    float heading;
    if (const auto mat = m_Vehicle->m_matrix) {
        const float fx = mat->GetForward().x, fy = mat->GetForward().y;
        __asm {
            fld fx
            fchs
            fld fy
            fpatan
            fstp heading
        }
    } else {
        heading = m_Vehicle->m_placement.m_fHeading;
    }
    ped->m_fCurrentRotation = ped->m_fAimingRotation = heading;
    ped->CalculateNewOrientation();

    // 0x64C52B - Make sure ped's not in a collision
    if (!CCarEnterExit::IsRoomForPedToLeaveCar(m_Vehicle, m_Door, &offset)) {
        ped->PositionPedOutOfCollision(m_Door, m_Vehicle, true);
    }

    // 0x64C554 - Set flags and reset move anim
    ped->bHasJustLeftCar      = true;
    ped->m_nPedState          = PEDSTATE_IDLE;
    ped->m_nSwimmingMoveState = PEDMOVE_STILL;
    ped->SetMoveAnim();

    // Task's over
    return true;
}
