/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedInAsDriver
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarDrive.h"
#include "CarEnterExit.h"
#include "Crime.h"
#include "PedGroups.h"
#include "RwHelper.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventLeaderEnteredCarAsDriver.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventGroup.h"
#include "Events/EventGlobalGroup.h"

void CTaskSimpleCarSetPedInAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsDriver, 0x86EE28, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6470E0);

    RH_ScopedVMTInstall(Clone, 0x649E00);
    RH_ScopedVMTInstall(GetTaskType, 0x647150);
    RH_ScopedVMTInstall(MakeAbortable, 0x647160);
    RH_ScopedVMTInstall(ProcessPed, 0x64B950);
}

// 0x6470E0
CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* targetVehicle, CTaskUtilityLineUpPedWithCar* utility) : CTaskSimple() {
    m_bIsFinished = 0;
    m_pAnim = 0;
    m_pTargetVehicle = targetVehicle;
    m_pUtility = utility;
    m_bWarpingInToCar = 0;
    m_nDoorFlagsToClear = 0;
    m_nNumGettingInToClear = 0;
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* targetVehicle, bool warpingInToCar, CTaskUtilityLineUpPedWithCar* utility) : // NOTSA
    CTaskSimpleCarSetPedInAsDriver{ targetVehicle, utility }
{
    m_bWarpingInToCar = warpingInToCar;
}

CTaskSimpleCarSetPedInAsDriver::~CTaskSimpleCarSetPedInAsDriver() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x649E00
CTask* CTaskSimpleCarSetPedInAsDriver::Clone() const {
    auto task = new CTaskSimpleCarSetPedInAsDriver(m_pTargetVehicle, m_pUtility);
    task->m_bWarpingInToCar = m_bWarpingInToCar;
    task->m_nDoorFlagsToClear = m_nDoorFlagsToClear;
    task->m_nNumGettingInToClear = m_nNumGettingInToClear;
    return task;
}

// 0x64B950
bool CTaskSimpleCarSetPedInAsDriver::ProcessPed(CPed* ped) {
    CEntity::SafeCleanUpRef(ped->m_pVehicle);
    ped->m_pVehicle = m_pTargetVehicle;
    CEntity::RegisterReference(ped->m_pVehicle);

    // Como el original: el conductor actual (si lo hay y no es este peatón) sale del coche antes, salvo que ya esté saliendo.
    if (m_pTargetVehicle->m_pDriver != ped && m_pTargetVehicle->m_pDriver) {
        if (!m_pTargetVehicle->m_pDriver->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)
            && !m_pTargetVehicle->m_pDriver->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP)
            && !m_pTargetVehicle->m_pDriver->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR_AND_DIE)
            && !m_pTargetVehicle->m_pDriver->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_BIKE_JACKED)
        ) {
            CTaskSimpleCarSetPedOut task{
                m_pTargetVehicle,
                static_cast<eTargetDoor>(CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, m_pTargetVehicle->m_pDriver)),
                true,
                true
            };
            task.ProcessPed(m_pTargetVehicle->m_pDriver);
        }
    }

    ped->m_fAimingRotation = ped->m_fCurrentRotation;

    if (m_bWarpingInToCar) {
        const CVector pos = m_pTargetVehicle->GetPosition();
        ped->GetPosition() = pos;
    }

    // Como el original: si el peatón lidera un grupo sin evento actual, avisa al grupo de que el líder se sube de conductor.
    if (const auto grp = CPedGroups::GetPedsGroup(ped)) {
        if (grp->GetMembership().IsLeader(ped) && !grp->GetIntelligence().GetCurrentEvent()) {
            CEventGroupEvent groupEvent{ ped, new CEventLeaderEnteredCarAsDriver{ m_pTargetVehicle } };
            grp->GetIntelligence().AddEvent(&groupEvent);
        }
    }

    m_pTargetVehicle->SetDriver(ped);
    ped->bInVehicle = true;

    if (ped->IsPlayer()) {
        ped->GetPlayerData()->m_bPlayersGangActive = true;
        ped->AsPlayer()->ClearAdrenaline();
    }

    if (ped->IsPlayer() && !m_pTargetVehicle->vehicleFlags.bHasBeenOwnedByPlayer) {
        m_pTargetVehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;
        CCrime::ReportCrime(CRIME_CAR_STEAL, m_pTargetVehicle, FindPlayerPed(-1));
    }

    ped->UpdateStatEnteringVehicle();
    ped->SetMoveState(PEDMOVE_NONE);
    ped->SetMoveAnim();
    ped->m_bUsesCollision = false;

    if (m_pTargetVehicle->m_nAlarmState == 0xFFFF) {
        m_pTargetVehicle->m_nAlarmState = 0x3A98;
    }

    m_pTargetVehicle->vehicleFlags.bEngineOn = !m_pTargetVehicle->vehicleFlags.bEngineBroken;
    ped->SetPedState(PEDSTATE_DRIVING);

    // Como el original: el estado del vehículo según el jugador / peatón de misión / tren.
    if (ped->IsPlayer()) {
        m_pTargetVehicle->SetStatus(STATUS_PLAYER);
    } else if (m_pTargetVehicle->GetStatus() != STATUS_SIMPLE || ped->GetCreatedBy() == PED_MISSION) {
        if (m_pTargetVehicle->m_nVehicleSubType == VEHICLE_TYPE_TRAIN) {
            m_pTargetVehicle->SetStatus(STATUS_TRAIN_NOT_MOVING);
        } else {
            m_pTargetVehicle->SetStatus(STATUS_PHYSICS);
        }
    }

    if (m_nDoorFlagsToClear) {
        m_pTargetVehicle->ClearGettingInFlags(m_nDoorFlagsToClear);
    }
    if (m_nNumGettingInToClear) {
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInToClear;
    }

    CCarEnterExit::RemoveGetInAnims(ped);
    CCarEnterExit::AddInCarAnim(m_pTargetVehicle, ped, true);
    ped->RemoveWeaponWhenEnteringVehicle(0);

    // Como el original: solo el autobús que no es el modelo 0x1AF oculta al peatón en el coche.
    if (m_pTargetVehicle->vehicleFlags.bIsBus && m_pTargetVehicle->m_nModelIndex != 0x1AF) {
        ped->bRenderPedInCar = false;
    } else {
        ped->bRenderPedInCar = true;
    }

    CTask* task;
    if (!m_bWarpingInToCar) {
        task = new CTaskSimpleCarDrive(m_pTargetVehicle, m_pUtility, false);
    } else {
        task = new CTaskSimpleCarDrive(m_pTargetVehicle, nullptr, false);
    }
    ped->GetIntelligence()->m_TaskMgr.SetTask(task, TASK_PRIMARY_DEFAULT, false);

    if (ped->IsPlayer() && m_pTargetVehicle && m_pTargetVehicle->IsLawEnforcementVehicle()) {
        CEventCopCarBeingStolen copEvent{ ped, m_pTargetVehicle };
        GetEventGlobalGroup()->Add(&copEvent, false);
    }
    return true;
}
