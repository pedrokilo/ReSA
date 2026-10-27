/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveCar
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

#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarSetPedOut.h"
#include "CarEnterExit.h"
#include "EventDamage.h"
#include "AnimManager.h"
#include "World.h"

#include "CarEnterExit.h"
#include "DamageManager.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "tHandlingData.h"

#include "TaskSimplePause.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "TaskSimpleDie.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarWaitToSlowDown.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleCarGetOut.h"
#include "TaskSimpleCarJumpOut.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskSimpleCarForcePedOut.h"

#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimplePause.h"
#include "EventGroupEvent.h"
#include "EventLeaderExitedCarAsDriver.h"
#include "PedGroups.h"

void CTaskComplexLeaveCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCar, 0x86E828, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B8C0);
    RH_ScopedInstall(Destructor, 0x63B970);
    RH_ScopedVMTInstall(MakeAbortable, 0x641100);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6419F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x641FC0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6421B0);
    RH_ScopedInstall(CreateSubTask, 0x641530);
}

// NOTSA: el EXE no tiene un constructor de tres argumentos de esta clase; 0x62F1A0 es el de CTaskComplexLeaveCarAndDie (vtable 0x86DD00)
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime) : CTaskComplexLeaveCar(targetVehicle, nTargetDoor, nDelayTime, false, true) {
    m_bDie = true;
}

// 0x63B8C0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) : CTaskComplex() {
    m_nTargetDoor                  = nTargetDoor;
    m_nDelayTime                   = nDelayTime;
    m_bSensibleLeaveCar            = bSensibleLeaveCar;
    m_pTargetVehicle               = targetVehicle;
    m_bForceGetOut                 = bForceGetOut;
    m_bDie                         = false;
    m_pTaskUtilityLineUpPedWithCar = nullptr;
    m_nDoorFlagsSet                = 0;
    m_nNumGettingInSet             = 0;
    m_nDieAnimID                   = ANIM_ID_KO_SHOT_FRONT_0;
    m_fDieAnimBlendDelta           = 4.0f;
    m_fDieAnimSpeed                = 1.0f;
    m_bIsInAir                     = false;

    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

// 0x63B970
CTaskComplexLeaveCar::~CTaskComplexLeaveCar() {
    if (m_pTargetVehicle) {
        m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsSet);
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInSet;
        m_pTargetVehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pTargetVehicle));
    }

    // Como el original: test edi,edi antes de operator delete (el destructor de la utilidad es trivial y delete nullptr llamaría a operator delete)
    if (m_pTaskUtilityLineUpPedWithCar) {
        delete m_pTaskUtilityLineUpPedWithCar;
    }
}

CTaskComplexLeaveCar::CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o) :
    CTaskComplexLeaveCar{ o.m_pTargetVehicle, o.m_nTargetDoor, o.m_nDelayTime, o.m_bSensibleLeaveCar, o.m_bForceGetOut }
{
}

// 0x641100
bool CTaskComplexLeaveCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: sin vehiculo no hay nada que soltar
    if (!m_pTargetVehicle) {
        return true;
    }

    // Como el original: cada salida con exito suelta las banderas de puerta y el contador de ocupantes que esta tarea habia reservado
    const auto ReleaseVehicle = [this] {
        m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsSet);
        m_nDoorFlagsSet = 0;
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInSet;
        m_nNumGettingInSet = 0;
    };

    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event); // Como el original: sin comprobar la subtarea ni el resultado
        if (ped->bInVehicle) {
            // Como el original (0x63BAB0): la puerta se calcula solo si aun no hay
            if (!m_nTargetDoor) {
                m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, ped);
            }
            // Como el original: dos tareas temporales en la pila; se aborta la de cerrar la puerta y la de sacar al ped corre una vez. Llamadas directas (no virtuales), se destruyen en orden inverso
            CTaskSimpleCarCloseDoorFromOutside closeDoor{ m_pTargetVehicle, static_cast<uint32>(m_nTargetDoor), nullptr };
            closeDoor.CTaskSimpleCarCloseDoorFromOutside::MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
            CTaskSimpleCarSetPedOut setPedOut{ m_pTargetVehicle, static_cast<eTargetDoor>(m_nTargetDoor), m_bSensibleLeaveCar, false };
            setPedOut.CTaskSimpleCarSetPedOut::ProcessPed(ped);
        }
        ReleaseVehicle();
        return true;
    }

    if (priority != ABORT_PRIORITY_URGENT) {
        return false;
    }

    // Como el original: GetTaskType se vuelve a llamar en cada comprobacion (no se guarda el valor)
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_DIE) {
        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE_TIMED) {
            return false;
        }
        const bool aborted = m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
        if (aborted) {
            ReleaseVehicle();
        }
        return aborted;
    }

    // Como el original (0x4ABCA0): el evento de dano solo cuenta si mato al ped
    const auto EventKilledPed = [&] {
        return event && event->GetEventType() == EVENT_DAMAGE && static_cast<const CEventDamage*>(event)->HasKilledPed();
    };

    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN && EventKilledPed()) {
        const bool aborted = m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
        if (aborted) {
            ReleaseVehicle();
        }
        return aborted;
    }

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GET_UP_AND_STAND_STILL && EventKilledPed()) {
        const bool aborted = m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
        if (aborted) {
            ReleaseVehicle();
        }
        return aborted;
    }

    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_JUMP_OUT) {
        if (!event) {
            return false;
        }
        if (event->GetEventType() == EVENT_IN_AIR || event->GetEventType() == EVENT_IN_WATER) {
            if (event->GetEventType() == EVENT_IN_AIR) {
                m_bIsInAir = true;
            }
            if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event)) {
                return false;
            }

            // Como el original: el arma activa se lee con signo (movsx) y el indice no se comprueba
            float blendDelta;
            if (ped->m_aWeapons.data()[static_cast<int8>(ped->m_nActiveWeaponSlot)].m_Type == WEAPON_PARACHUTE
                && event->GetEventType() == EVENT_IN_AIR
                && m_pTargetVehicle->GetNumContactWheels() == 0
            ) {
                // Como el original: un avion rapido salta sin mas comprobaciones; si no, solo un jugador sobre el suelo a menos de 10 m
                bool fastPlane = false;
                if (m_pTargetVehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE) {
                    fastPlane = m_pTargetVehicle->m_vecMoveSpeed.Magnitude() > 0.2f; // flt_858CC4
                }
                if (!fastPlane) {
                    if (!ped->IsPlayer()) {
                        return false;
                    }
                    CColPoint colPoint;
                    CEntity*  hitEntity;
                    if (CWorld::ProcessVerticalLine(ped->GetPosition(), -10.0f, colPoint, hitEntity, true, false, false, false, true, false, nullptr)) {
                        return false;
                    }
                }
                blendDelta = 8.0f;
            } else {
                if (event->GetEventType() != EVENT_IN_WATER) {
                    return false;
                }
                if (!(ped->m_vecMoveSpeed.z < -0.3f)) { // flt_858EE4; con NaN no entra
                    return false;
                }
                blendDelta = 16.0f;
            }
            // Como el original: se pasa el objeto RW tal cual (GetRpClump comprobaria su tipo)
            CAnimManager::BlendAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), ANIM_GROUP_DEFAULT, ANIM_ID_FALL_FRONT, blendDelta);
            return false;
        }
    }

    // Como el original: resto de casos (y salto sin evento de aire/agua)
    if (!event) {
        return false;
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE_TIMED) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
    }
    return false;
}

// Como el original: desplazamientos de CVehicle que CreateNextSubTask escribe o lee en crudo.
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0);
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(CVehicle, m_nVehicleType) == 0x590 && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CVehicle, m_nDoorLock) == 0x4F8);
static_assert(offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_nNumGettingIn) == 0x485 && offsetof(CVehicle, m_nGettingInFlags) == 0x486 && offsetof(CVehicle, m_nGettingOutFlags) == 0x487);
static_assert(offsetof(CAutomobile, m_damageManager) == 0x5A0 && offsetof(CAutomobile, m_aCarNodes) == 0x648);
static_assert(offsetof(tHandlingData, m_nModelFlags) == 0xCC);

namespace {
// 0x63BAB0: puerta por defecto del que se baja.
void ComputeTargetDoor(CTaskComplexLeaveCar* task, CPed* ped) {
    if (task->m_nTargetDoor == 0) {
        task->m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(task->m_pTargetVehicle, ped);
    }
}

// 0x63BA00: reserva la puerta y el hueco de salida en el vehículo.
void RegisterPedLeaving(CTaskComplexLeaveCar* task, CPed* ped) {
    const auto doorFlags = static_cast<uint8>(CCarEnterExit::ComputeDoorFlag(task->m_pTargetVehicle, task->m_nTargetDoor, true));
    task->m_nDoorFlagsSet = doorFlags;
    task->m_pTargetVehicle->SetGettingOutFlags(doorFlags);
    task->m_nNumGettingInSet = 1;
    task->m_pTargetVehicle->m_nNumGettingIn++;

    if (task->m_pTargetVehicle->m_pDriver && !task->m_pTargetVehicle->m_pDriver->IsPlayer() && ped == task->m_pTargetVehicle->m_pDriver && task->m_bSensibleLeaveCar) {
        task->m_pTargetVehicle->m_autoPilot.m_nCruiseSpeed = 0;
        task->m_pTargetVehicle->m_autoPilot.m_nCarMission = MISSION_NONE;
    }
    if (ped->IsPlayer() && ped == task->m_pTargetVehicle->m_pDriver) {
        task->m_pTargetVehicle->SetStatus(STATUS_FORCED_STOP);
    }
}

// 0x63BAE0: utilidad de alineación con la puerta (se crea con un desplazamiento nulo).
void CreateLineUpUtility(CTaskComplexLeaveCar* task) {
    // Como el original: comprobación de nulo tras operator new (MSVC antiguo); si no hay memoria se guarda nulo sin construir
    void* const memory = operator new(sizeof(CTaskUtilityLineUpPedWithCar));
    task->m_pTaskUtilityLineUpPedWithCar = memory ? new (memory) CTaskUtilityLineUpPedWithCar(CVector{ 0.0f, 0.0f, 0.0f }, 0, 0, task->m_nTargetDoor) : nullptr;
}

// 0x646CF0: ¿la puerta está siendo usada para subir (gettingIn) o para bajar (gettingOut)?
void IsDoorInUse(const CVehicle* vehicle, int32 door, bool& gettingIn, bool& gettingOut) {
    uint8 mask;
    switch (door) {
    case 8:  mask = 4; break;
    case 9:  mask = 8; break;
    case 10: mask = 1; break;
    case 11: mask = 2; break;
    default: return;
    }
    if (vehicle->m_nGettingInFlags & mask) {
        gettingIn = true;
    }
    if (vehicle->m_nGettingOutFlags & mask) {
        gettingOut = true;
    }
}
} // namespace

// 0x6419F0
CTask* CTaskComplexLeaveCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        return CreateSubTask(ped->bInVehicle ? TASK_SIMPLE_CAR_SET_PED_OUT : TASK_FINISHED, ped);
    case TASK_SIMPLE_DIE:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_BOAT:
        return CreateSubTask(m_bDie ? TASK_SIMPLE_DIE : TASK_FINISHED, ped);
    case TASK_SIMPLE_CAR_DRIVE_TIMED: { // 0x2CF
        if (!ped->bInVehicle) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (!ped->m_pVehicle->IsPassenger(ped) && !ped->m_pVehicle->IsDriver(ped)) {
            ped->bInVehicle = false;
            return CreateSubTask(TASK_FINISHED, ped);
        }
        ComputeTargetDoor(this, ped);
        bool gettingIn  = false;
        bool gettingOut = false;
        IsDoorInUse(m_pTargetVehicle, m_nTargetDoor, gettingIn, gettingOut);
        if (!m_bForceGetOut) {
            if (gettingOut) {
                return CreateSubTask(TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE, ped);
            }
            if (gettingIn) {
                return CreateSubTask(TASK_FINISHED, ped);
            }
        }
        return CreateSubTask(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
    }
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        m_nDieAnimID = ANIM_ID_KO_SHOT_FRONT_0;
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN: {
        if (!m_pTargetVehicle->CanPedStepOutCar(false)) {
            RegisterPedLeaving(this, ped);
            ped->SetPedState(PEDSTATE_NONE);
            return CreateSubTask(TASK_SIMPLE_CAR_JUMP_OUT, ped);
        }

        // Puerta alternativa si no hay sitio para bajar por la elegida; `giveUp`: no hay salida normal (se fuerza al peatón)
        bool giveUp = false;
        if (!CCarEnterExit::IsRoomForPedToLeaveCar(m_pTargetVehicle, m_nTargetDoor, nullptr)) {
            const CVehicle* const vehicle = m_pTargetVehicle;
            int32 altDoor = 0;
            const auto isBikeOrTandem = [&] {
                return vehicle->m_nVehicleType == VEHICLE_TYPE_BIKE || vehicle->m_pHandlingData->m_bTandemSeats;
            };
            switch (m_nTargetDoor) {
            case 8:
                if (vehicle->m_pDriver || (vehicle->m_nGettingInFlags & 1)) {
                    giveUp = true;
                } else {
                    altDoor = 10;
                }
                break;
            case 9:
                if (vehicle->m_apPassengers.data()[1] || (vehicle->m_nGettingInFlags & 2)) {
                    giveUp = true;
                } else {
                    altDoor = 11;
                }
                break;
            case 10:
                altDoor = 8;
                if (!isBikeOrTandem() && (vehicle->m_apPassengers.data()[0] || (vehicle->m_nGettingInFlags & 4))) {
                    giveUp = true;
                }
                break;
            case 11:
                altDoor = 9;
                if (!isBikeOrTandem() && (vehicle->m_apPassengers.data()[2] || (vehicle->m_nGettingInFlags & 8))) {
                    giveUp = true;
                }
                break;
            default:
                break;
            }
            if (!giveUp && !CCarEnterExit::IsRoomForPedToLeaveCar(m_pTargetVehicle, altDoor, nullptr)) {
                giveUp = true;
            }
            if (!giveUp && m_pTargetVehicle->m_pHandlingData->m_bForceDoorCheck && m_pTargetVehicle->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE
                && !static_cast<CAutomobile*>(m_pTargetVehicle)->m_aCarNodes.data()[altDoor]) {
                giveUp = true;
            }
            if (!giveUp) {
                m_nTargetDoor = altDoor;
            }
        }

        RegisterPedLeaving(this, ped);
        ped->SetPedState(PEDSTATE_NONE);
        if (giveUp) {
            CreateLineUpUtility(this);
            return CreateSubTask(TASK_SIMPLE_CAR_FORCE_PED_OUT, ped);
        }
        if (m_bDie) {
            return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
        }
        CreateLineUpUtility(this);
        return CreateSubTask(TASK_SIMPLE_CAR_GET_OUT, ped);
    }
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        return CreateSubTask(TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN, ped);
    case TASK_SIMPLE_CAR_GET_OUT: {
        m_pTaskUtilityLineUpPedWithCar->m_nDoorOpenPosType = 2;
        if (!m_bForceGetOut || m_pTargetVehicle->m_nDoorLock != CARLOCK_UNLOCKED) {
            if (CCarEnterExit::CarHasDoorToClose(m_pTargetVehicle, m_nTargetDoor)) {
                return CreateSubTask(TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE, ped);
            }
        }
        if (m_pTargetVehicle->m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE && CCarEnterExit::CarHasDoorToClose(m_pTargetVehicle, m_nTargetDoor)) {
            auto& damageManager = static_cast<CAutomobile*>(m_pTargetVehicle)->m_damageManager;
            const auto door = static_cast<tComponent>(m_nTargetDoor);
            // Como el original: tres llamadas independientes a GetDoorStatus_Component (comprobación, segunda comprobación y valor a guardar)
            if (damageManager.GetDoorStatus_Component(door) == DAMSTATE_OK || damageManager.GetDoorStatus_Component(door) == DAMSTATE_DAMAGED) {
                damageManager.SetDoorStatus_Component(door, static_cast<eDoorStatus>(damageManager.GetDoorStatus_Component(door) + 1));
            }
        }
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    }
    case TASK_SIMPLE_CAR_JUMP_OUT: {
        if (m_bDie) {
            m_nDieAnimID         = ANIM_ID_KO_SHOT_FRONT_0;
            m_fDieAnimBlendDelta = 1000.0f;
            m_fDieAnimSpeed      = 0.5f;
            return CreateSubTask(TASK_SIMPLE_DIE, ped);
        }
        if (!m_pTargetVehicle || m_pTargetVehicle->m_nVehicleType == VEHICLE_TYPE_BIKE || m_pTargetVehicle->m_nVehicleSubType == VEHICLE_TYPE_QUAD) {
            return CreateSubTask(TASK_FINISHED, ped);
        }
        // Como el original: en el aire (el salto 0x641A9E cae despues de la escritura de m_nDieAnimID) o ahogandose se baja sin tocar la animacion de muerte
        if (m_bIsInAir || ped->bIsDrowning) {
            return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
        }
        m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsSet);
        m_nDoorFlagsSet = 0;
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInSet;
        m_nNumGettingInSet = 0;
        return CreateSubTask(TASK_COMPLEX_GET_UP_AND_STAND_STILL, ped);
    }
    case TASK_SIMPLE_CAR_FORCE_PED_OUT:
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return CreateSubTask(m_bDie ? TASK_SIMPLE_DIE : TASK_FINISHED, ped);
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        m_nDieAnimID         = ANIM_ID_FLOOR_HIT;
        m_fDieAnimBlendDelta = 1000.0f;
        m_fDieAnimSpeed      = 0.5f;
        if (m_pTargetVehicle && m_pTargetVehicle->m_nDoorLock == CARLOCK_COP_CAR) {
            m_pTargetVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
        }
        return CreateSubTask(TASK_SIMPLE_DIE, ped);
    default:
        return nullptr;
    }
}

// 0x641FC0 (0x641FC0..0x6421AC, `ret 4`, marco SEH propio): CTask::operator new (0x61A5A0) devuelve nulo con el pool agotado y el EXE lo comprueba antes de construir cada tarea.
CTask* CTaskComplexLeaveCar::CreateFirstSubTask(CPed* ped) {
    // Como el original: peaton arrestado (estado 0x3F o bandera +0x474 & 0x40) y jugador, o fuera del coche (con un mensaje por printf del CRT del juego, 0x821982): pausa indefinida
    const bool arrested = ped->m_nPedState == PEDSTATE_ARRESTED || ped->bIsBeingArrested;
    if (arrested && ped->IsPlayer()) {
        void* const memory = CTask::operator new(sizeof(CTaskSimplePause));
        return memory ? ::new (memory) CTaskSimplePause(-1) : nullptr;
    }
    if (!ped->bInVehicle) {
        reinterpret_cast<int(__cdecl*)(const char*, ...)>(0x821982)("CTaskComplexLeaveCar - ped not in car");
        void* const memory = CTask::operator new(sizeof(CTaskSimplePause));
        return memory ? ::new (memory) CTaskSimplePause(-1) : nullptr;
    }

    CVehicle* const vehicle = m_pTargetVehicle;
    if (vehicle->m_pDriver == ped) {
        // Como el original: si el conductor es el lider de su grupo, el grupo recibe un CEventLeaderExitedCarAsDriver (reservado con CEvent::operator new 0x4B5620, tambien con comprobacion de nulo)
        if (CPedGroup* const group = CPedGroups::GetPedsGroup(ped); group && group->GetMembership().IsLeader(ped)) {
            void* const memory = CEvent::operator new(sizeof(CEventLeaderExitedCarAsDriver));
            CEvent* const event = memory ? ::new (memory) CEventLeaderExitedCarAsDriver : nullptr; // Como el original (0x4B8300): sin inicializacion por valor (no se pone a cero el relleno del evento)
            CEventGroupEvent groupEvent{ ped, event };
            group->GetIntelligence().AddEvent(&groupEvent);
        }
        if (!ped->IsPlayer()) {
            ped->SetRadioStation();
        } else if (CVehicle* const pedVehicle = ped->m_pVehicle) {
            pedVehicle->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        }
    }

    if (vehicle->m_nVehicleType == VEHICLE_TYPE_BOAT) {
        void* const memory = CTask::operator new(sizeof(CTaskComplexLeaveBoat));
        return memory ? ::new (memory) CTaskComplexLeaveBoat(vehicle, 0) : nullptr;
    }
    void* const memory = CTask::operator new(sizeof(CTaskSimpleCarDriveTimed));
    return memory ? ::new (memory) CTaskSimpleCarDriveTimed(vehicle, m_nDelayTime) : nullptr;
}

// 0x6421B0
CTask* CTaskComplexLeaveCar::ControlSubTask(CPed* ped) {
    if (!m_pTargetVehicle) {
        return nullptr;
    }

    // Como el original: GetTaskType se vuelve a llamar en cada comprobacion (no se guarda el valor) y solo si el ped no esta en el vehiculo
    if (!ped->bInVehicle
        && m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_SET_PED_OUT
        && m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_JUMP_OUT
        && m_pSubTask->GetTaskType() != TASK_COMPLEX_GET_UP_AND_STAND_STILL
        && m_pSubTask->GetTaskType() != TASK_SIMPLE_DIE
    ) {
        return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
    }

    if (!m_bSensibleLeaveCar && m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }

    return m_pSubTask;
}

namespace {
// Como el original (0x641530): cada rama reserva con CTask::operator new (0x61A5A0) y, si el pool esta agotado (nulo), no construye y devuelve nulo.
template<class T, class... Args>
CTask* NewLeaveCarSubTask(Args... args) {
    void* const memory = CTask::operator new(sizeof(T));
    if (!memory) {
        return nullptr;
    }
    if constexpr (sizeof...(Args) == 0) {
        return ::new (memory) T; // sin inicializacion por valor: el EXE no pone a cero el bloque antes del constructor
    } else {
        return ::new (memory) T(args...);
    }
}

// Tamanos que reserva el EXE en cada rama
static_assert(sizeof(CTaskSimplePause) == 0x18 && sizeof(CTaskComplexGetUpAndStandStill) == 0xC && sizeof(CTaskSimpleDie) == 0x28 && sizeof(CTaskComplexLeaveBoat) == 0x14
    && sizeof(CTaskSimpleCarDriveTimed) == 0x70 && sizeof(CTaskSimpleCarCloseDoorFromOutside) == 0x1C && sizeof(CTaskSimpleCarWaitToSlowDown) == 0x14
    && sizeof(CTaskSimpleCarWaitForDoorNotToBeInUse) == 0x14 && sizeof(CTaskSimpleCarGetOut) == 0x20 && sizeof(CTaskSimpleCarJumpOut) == 0x24
    && sizeof(CTaskSimpleCarSetPedOut) == 0x18 && sizeof(CTaskComplexCarSlowBeDraggedOut) == 0x1C);
} // namespace

// 0x641530
CTask* CTaskComplexLeaveCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_PAUSE:
        return NewLeaveCarSubTask<CTaskSimplePause>(-1);
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        return NewLeaveCarSubTask<CTaskComplexGetUpAndStandStill>();
    case TASK_SIMPLE_DIE:
        return NewLeaveCarSubTask<CTaskSimpleDie>(static_cast<AssocGroupId>(0), static_cast<AnimationId>(m_nDieAnimID), m_fDieAnimBlendDelta, m_fDieAnimSpeed);
    case TASK_COMPLEX_LEAVE_BOAT:
        return NewLeaveCarSubTask<CTaskComplexLeaveBoat>(m_pTargetVehicle, 0u);
    case TASK_SIMPLE_CAR_DRIVE_TIMED:
        return NewLeaveCarSubTask<CTaskSimpleCarDriveTimed>(m_pTargetVehicle, m_nDelayTime);
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        return NewLeaveCarSubTask<CTaskSimpleCarCloseDoorFromOutside>(m_pTargetVehicle, static_cast<uint32>(m_nTargetDoor), m_pTaskUtilityLineUpPedWithCar);
    case TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN: {
        using SlowDownType = CTaskSimpleCarWaitToSlowDown::SlowDownType;
        if (m_bDie) {
            return NewLeaveCarSubTask<CTaskSimpleCarWaitToSlowDown>(m_pTargetVehicle, SlowDownType::DONT_WAIT);
        }
        if (m_bSensibleLeaveCar) {
            // IsPlayer (0x5DF8F0) se llama antes de reservar, como el original
            const bool isPlayer = ped->IsPlayer();
            return NewLeaveCarSubTask<CTaskSimpleCarWaitToSlowDown>(m_pTargetVehicle, isPlayer ? SlowDownType::SLOW_ENOUGH_TO_STEP_OR_JUMP : SlowDownType::SLOW_ENOUGH_TO_STEP);
        }
        return NewLeaveCarSubTask<CTaskSimpleCarWaitToSlowDown>(m_pTargetVehicle, SlowDownType::DONT_WAIT);
    }
    case TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE:
        return NewLeaveCarSubTask<CTaskSimpleCarWaitForDoorNotToBeInUse>(m_pTargetVehicle, static_cast<uint32>(m_nTargetDoor), 0u);
    case TASK_SIMPLE_CAR_GET_OUT:
        return NewLeaveCarSubTask<CTaskSimpleCarGetOut>(m_pTargetVehicle, static_cast<uint32>(m_nTargetDoor), m_pTaskUtilityLineUpPedWithCar);
    case TASK_SIMPLE_CAR_JUMP_OUT:
        return NewLeaveCarSubTask<CTaskSimpleCarJumpOut>(m_pTargetVehicle, static_cast<uint32>(m_nTargetDoor), m_pTaskUtilityLineUpPedWithCar);
    case TASK_SIMPLE_CAR_FORCE_PED_OUT:
        return NewLeaveCarSubTask<CTaskSimpleCarForcePedOut>(m_pTargetVehicle, static_cast<int32>(m_nTargetDoor));
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return NewLeaveCarSubTask<CTaskSimpleCarSetPedOut>(m_pTargetVehicle, static_cast<eTargetDoor>(m_nTargetDoor), m_bSensibleLeaveCar);
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        return NewLeaveCarSubTask<CTaskComplexCarSlowBeDraggedOut>(m_pTargetVehicle, static_cast<eTargetDoor>(m_nTargetDoor), true);
    default:
        return nullptr;
    }
}
