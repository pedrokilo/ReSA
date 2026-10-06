/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedOut
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

#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimplePlayerOnFoot.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexWander.h"
#include "CarEnterExit.h"
#include "Garages.h"

void CTaskSimpleCarSetPedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedOut, 0x86EEB8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(PositionPedOutOfCollision, 0x6479B0);

    RH_ScopedVMTInstall(Clone, 0x649F50);
    RH_ScopedVMTInstall(GetTaskType, 0x647930);
    RH_ScopedVMTInstall(MakeAbortable, 0x647940);
    RH_ScopedVMTInstall(ProcessPed, 0x647D10);
}

CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine, bool warpingOutOfCar) :
    m_nTargetDoor{ nTargetDoor },
    m_pTargetVehicle{ targetVehicle },
    m_bSwitchOffEngine{ bSwitchOffEngine },
    m_bWarpingOutOfCar{ warpingOutOfCar }
{
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

CTaskSimpleCarSetPedOut::~CTaskSimpleCarSetPedOut() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x6479B0
void CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(CPed* ped, CVehicle* veh, int32 door) {
    // Como el original: sin vehículo dado usa el del peatón; si tampoco hay, no hace nada.
    if (!veh) {
        veh = ped->m_pVehicle;
        if (!veh) {
            return;
        }
    }

    // Como el original: fpatan(-top.x, top.y) (o el heading guardado sin matriz), redondeado a float al guardarlo.
    const float heading = veh->GetHeading();
    ped->m_fCurrentRotation = heading;
    ped->m_fAimingRotation  = heading;
    if (ped->m_matrix) {
        ped->m_matrix->SetRotateZOnly(heading);
    } else {
        ped->m_placement.m_fHeading = heading;
    }

    if (veh->m_nVehicleType == VEHICLE_TYPE_BOAT) {
        if (veh->m_pHandlingData->m_bSitInBoat) {
            // Como el original: la posición del peatón se desplaza media unidad a lo largo del vector "up" del barco.
            const auto& up = veh->m_matrix->GetUp();
            const float dx = up.x * 0.5f;
            const float dy = up.y * 0.5f;
            const float dz = up.z * 0.5f;
            auto& pos = ped->m_matrix->GetPosition();
            pos.x = dx + pos.x;
            pos.y = dy + pos.y;
            pos.z = dz + pos.z;
        }

        if (ped->TestCollision(false)) {
            if (veh->vehicleFlags.bIsDrowning) {
                ped->m_matrix->GetPosition().z -= 0.3f;
                if (!ped->TestCollision(false)) {
                    return;
                }
                ped->m_matrix->GetPosition().z += 0.3f;
                ped->PositionPedOutOfCollision(door, veh, true);
                return;
            }

            // Como el original: se retrocede 0.3 por el vector "forward" del peatón y, si sigue chocando, se vuelve a avanzar.
            auto* m = ped->m_matrix;
            const float fx = m->GetForward().x * 0.3f;
            const float fy = m->GetForward().y * 0.3f;
            const float fz = m->GetForward().z * 0.3f;
            m->GetPosition().x = m->GetPosition().x - fx;
            m->GetPosition().y = m->GetPosition().y - fy;
            m->GetPosition().z = m->GetPosition().z - fz;
            if (ped->TestCollision(false)) {
                const CVector fwd = 0.3f * ped->m_matrix->GetForward();
                ped->m_matrix->GetPosition() += fwd;
                ped->PositionPedOutOfCollision(door, veh, true);
                return;
            }
        }

        ped->m_vecMoveSpeed.x = veh->m_vecMoveSpeed.x * 0.9f;
        ped->m_vecMoveSpeed.y = veh->m_vecMoveSpeed.y * 0.9f;
        ped->m_vecMoveSpeed.z = veh->m_vecMoveSpeed.z * 0.9f;
        ped->m_vecMoveSpeed.z -= 0.1f;
        ped->bIsStanding = true;
        if (!ped->m_standingOnEntity) {
            ped->m_standingOnEntity = veh;
            veh->RegisterReference(&ped->m_standingOnEntity);
        }
        return;
    }

    CWorld::pIgnoreEntity = veh;
    bool blocked = false;
    {
        const auto& p = ped->GetPosition();
        const CVector c{ p.x, p.y, p.z - 0.2f };
        const auto* e = CWorld::TestSphereAgainstWorld(c, 0.4f, veh, true, true, false, false, false, false);
        if (e && e != veh->m_pAttachedTo) {
            blocked = true;
        }
    }
    {
        const auto& p = ped->GetPosition();
        const CVector c{ p.x, p.y, p.z + 0.2f };
        const auto* e = CWorld::TestSphereAgainstWorld(c, 0.4f, veh, true, true, false, false, false, false);
        if (e && e != veh->m_pAttachedTo) {
            blocked = true;
        }
    }
    if (!CWorld::GetIsLineOfSightClear(veh->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false) || blocked) {
        ped->PositionPedOutOfCollision(door, veh, true);
    }
    CWorld::pIgnoreEntity = nullptr;
}

// 0x649F50
CTask* CTaskSimpleCarSetPedOut::Clone() const {
    // Como el original: el constructor con (vehículo, puerta, apagar motor) y luego se copian los cinco bytes de estado.
    auto* task = new CTaskSimpleCarSetPedOut(m_pTargetVehicle, m_nTargetDoor, m_bSwitchOffEngine);
    task->m_bWarpingOutOfCar     = m_bWarpingOutOfCar;
    task->m_bFallingOutOfCar     = m_bFallingOutOfCar;
    task->m_bKnockedOffBike      = m_bKnockedOffBike;
    task->m_nDoorFlagsToClear    = m_nDoorFlagsToClear;
    task->m_nNumGettingInToClear = m_nNumGettingInToClear;
    return task;
}

// 0x647D10
bool CTaskSimpleCarSetPedOut::ProcessPed(CPed* ped) {
    ped->bInVehicle = false;
    ped->m_bUsesCollision = true;
    ped->UpdateStatLeavingVehicle();

    if (!m_bKnockedOffBike) {
        PositionPedOutOfCollision(ped, nullptr, m_nTargetDoor);
    }
    CCarEnterExit::RemoveCarSitAnim(ped);
    ped->RestartNonPartialAnims();

    if (!m_bKnockedOffBike && !m_bFallingOutOfCar && m_pTargetVehicle->m_nVehicleSubType != VEHICLE_TYPE_BOAT) {
        ped->m_vecMoveSpeed = CVector{};
    }

    if (ped->m_pVehicle) {
        if (m_nDoorFlagsToClear) {
            m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsToClear);
        }
        if (m_nNumGettingInToClear) {
            m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInToClear;
        }

        if (ped->m_pVehicle->m_pDriver == ped) {
            ped->m_pVehicle->RemoveDriver(!m_bSwitchOffEngine);
            ped->m_pVehicle->SetStatus(STATUS_ABANDONED);
            if (ped->m_pVehicle->m_nDoorLock == CARLOCK_COP_CAR) {
                ped->m_pVehicle->m_nDoorLock = CARLOCK_UNLOCKED;
            }
            if (ped->m_nPedType == PED_TYPE_COP && ped->m_pVehicle->IsLawEnforcementVehicle()) {
                ped->m_pVehicle->ChangeLawEnforcerState(false);
            }
        } else {
            ped->m_pVehicle->RemovePassenger(ped);
        }

        CVector pos = ped->GetPosition();
        if (CGarages::IsPointWithinAnyGarage(pos)) {
            ped->m_pVehicle->m_nOverrideLights = NO_CAR_LIGHT_OVERRIDE;
            ped->m_pVehicle->vehicleFlags.bLightsOn = false;
        }
    }

    if (!m_bFallingOutOfCar && !m_bKnockedOffBike && ped->m_pVehicle && ped->m_pVehicle->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        auto* veh = ped->m_pVehicle;
        // Como el original: la comparación contra la constante double 0.1 (0x86EEE0); NaN no cuenta.
        if (std::fabs((double)veh->m_vecMoveSpeed.x) < 0.1 && std::fabs((double)veh->m_vecMoveSpeed.y) < 0.1) {
            veh->AsBike()->bikeFlags.bOnSideStand = true;
        }
    }

    // Como el original: tarea por defecto según el tipo de peatón (jugador / de misión / el resto).
    CTask* task;
    if (ped->IsPlayer()) {
        task = new CTaskSimplePlayerOnFoot();
    } else if (ped->GetCreatedBy() == PED_MISSION) {
        task = new CTaskSimpleStandStill(999999, true, false, 8.0f);
    } else {
        task = CTaskComplexWander::GetWanderTaskByPedType(ped);
    }
    ped->GetIntelligence()->m_TaskMgr.SetTask(task, TASK_PRIMARY_DEFAULT, false);

    ped->ReplaceWeaponWhenExitingVehicle();
    ped->bHasJustLeftCar = true;
    ped->m_nPedState = PEDSTATE_IDLE;
    if (m_pTargetVehicle && m_pTargetVehicle->physicalFlags.bTouchingWater) {
        ped->physicalFlags.bTouchingWater = true;
    }
    return true;
}
