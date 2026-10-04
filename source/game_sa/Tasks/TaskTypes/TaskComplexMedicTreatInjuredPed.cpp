/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexMedicTreatInjuredPed
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

#include "TaskComplexMedicTreatInjuredPed.h"

#include "TaskComplexTreatAccident.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskComplexDriveWander.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexWanderMedic.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "InterestingEvents.h"

void CTaskComplexMedicTreatInjuredPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexMedicTreatInjuredPed, 0x86F518, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x658BA0);
    RH_ScopedInstall(CreateSubTask, 0x658DB0);
    RH_ScopedInstall(CreateDealWithNextAccidentTask, 0x65A020);
    RH_ScopedInstall(FindNearestAccident, 0x658CC0);
    RH_ScopedInstall(CalcTargetPosWithOffset, 0x658D20);
    RH_ScopedVMTInstall(Clone, 0x659AF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x659FE0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A990);
    RH_ScopedVMTInstall(ControlSubTask, 0x65ABF0);
}

CTaskComplexMedicTreatInjuredPed* CTaskComplexMedicTreatInjuredPed::Constructor(CVehicle* vehicle, CPed* ped, bool isDriver) {
    this->CTaskComplexMedicTreatInjuredPed::CTaskComplexMedicTreatInjuredPed(vehicle, ped, isDriver);
    return this;
}

// 0x658BA0
CTaskComplexMedicTreatInjuredPed::CTaskComplexMedicTreatInjuredPed(CVehicle* vehicle, CPed* partner, bool isDriver) : CTaskComplex() {
    m_pVehicle      = vehicle;
    m_pPartnerMedic = partner;
    m_bIsDriver     = isDriver;
    m_pAccident     = nullptr;
    m_bLeftCarOnce  = false;
    CEntity::SafeRegisterRef(m_pVehicle);
    CEntity::SafeRegisterRef(m_pPartnerMedic);
}

// 0x658C30
CTaskComplexMedicTreatInjuredPed::~CTaskComplexMedicTreatInjuredPed() {
    if (m_pAccident && m_pAccident->m_pPed && !m_pAccident->m_bIsRevived)
        m_pAccident->m_bIsTreated = false;

    CEntity::SafeCleanUpRef(m_pVehicle);
    CEntity::SafeCleanUpRef(m_pPartnerMedic);
}

// 0x658DB0
CTask* CTaskComplexMedicTreatInjuredPed::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_TREAT_ACCIDENT:
        return new CTaskComplexTreatAccident(m_pAccident);
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(10'000, false, false, 8.0F);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return new CTaskComplexEnterCarAsPassenger(m_pVehicle, 11, false);
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver(m_pVehicle);
    case TASK_COMPLEX_LEAVE_CAR:
        m_bLeftCarOnce = true;
        return new CTaskComplexLeaveCar(m_pVehicle, 0, 0, true, false);
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(m_pVehicle, nullptr, false);
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT:
        return new CTaskComplexDriveToPoint(m_pVehicle, m_vecAccidentPosition, 30.0F, 0, MODEL_INVALID, -1.0F, DRIVING_STYLE_AVOID_CARS);
    case TASK_COMPLEX_CAR_DRIVE_WANDER:
        return new CTaskComplexCarDriveWander(m_pVehicle, DRIVING_STYLE_AVOID_CARS, 30.0F);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill(PEDMOVE_RUN, m_vecAccidentPosition, 0.125F, 2.0F, false, false);
    case TASK_COMPLEX_WANDER:
        // Como el original: CGeneral::GetRandomNumberInRange(int, int) (0x407180), [0, 8)
        return new CTaskComplexWanderMedic(PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8), true);
    default:
        return nullptr;
    }
}

// 0x65A020
CTask* CTaskComplexMedicTreatInjuredPed::CreateDealWithNextAccidentTask(CPed* ped, CAccident* accident) {
    m_pAccident = nullptr; // Como el original: se anula antes de buscar
    m_pAccident = CAccidentManager::GetInstance()->GetNearestFreeAccidentExceptThisOne(ped->GetPosition(), accident, true);
    if (m_pAccident && m_pAccident->m_pPed) {
        m_pAccident->m_bIsTreated = true;
        CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
        if (m_pSubTask && m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
            auto subTask = reinterpret_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask);
            subTask->GoToPoint(m_vecAccidentPosition, 0.5F, 2.0F, false);
            subTask->m_moveState = PEDMOVE_RUN;
            return subTask;
        } else {
            auto* const task = CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
            // Como el original: el EXE devuelve el peatón en vez de la tarea creada (que se pierde).
            // Rama inalcanzable desde CreateNextSubTask/ControlSubTask (la subtarea siempre es GO_TO_POINT_AND_STAND_STILL)
            UNUSED(task);
            return reinterpret_cast<CTask*>(ped);
        }
    } else
        return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
}

// 0x658CC0
void CTaskComplexMedicTreatInjuredPed::FindNearestAccident(CVector& posn) {
    m_pAccident = nullptr; // Como el original: se anula antes de buscar
    m_pAccident = CAccidentManager::GetInstance()->GetNearestFreeAccident(posn, true);
}

// 0x658D20
void CTaskComplexMedicTreatInjuredPed::CalcTargetPosWithOffset(CPed* ped, CPed* injuredPed) {
    // Como el original: GetBonePosition (0x5E4280) del muslo izquierdo y luego del derecho, (der + izq) * 0.5 por componente
    CVector left, right;
    injuredPed->GetBonePosition(&left, BONE_L_THIGH, false);
    injuredPed->GetBonePosition(&right, BONE_R_THIGH, false);
    m_vecAccidentPosition = CVector{ (right.x + left.x) * 0.5F, (right.y + left.y) * 0.5F, (right.z + left.z) * 0.5F };
}

// 0x659AF0
CTask* CTaskComplexMedicTreatInjuredPed::Clone() const {
    return new CTaskComplexMedicTreatInjuredPed(m_pVehicle, m_pPartnerMedic, m_bIsDriver);
}

// 0x659FE0
CTask* CTaskComplexMedicTreatInjuredPed::CreateFirstSubTask(CPed* ped) {
    if (!m_pVehicle)
        return CreateSubTask(TASK_COMPLEX_WANDER);

    if (m_bIsDriver)
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER);

    return CreateSubTask(TASK_SIMPLE_CAR_DRIVE);
}

// 0x65A990
CTask* CTaskComplexMedicTreatInjuredPed::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (subTaskId == TASK_COMPLEX_TREAT_ACCIDENT) {
        FindNearestAccident(ped->GetPosition());
        if (!m_pAccident)
            return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);
        else {
            m_pAccident->m_bIsTreated = true;
            CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
            return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
        }
    }

    if (subTaskId == TASK_SIMPLE_STAND_STILL)
        return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER);

    if (subTaskId == TASK_COMPLEX_ENTER_CAR_AS_DRIVER || subTaskId == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)
        return CreateSubTask(ped->bInVehicle ? TASK_SIMPLE_CAR_DRIVE : TASK_COMPLEX_WANDER);

    if (subTaskId == TASK_COMPLEX_LEAVE_CAR) {
        g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_12, ped);
        if (m_bIsDriver && m_pAccident->m_pPed && m_pAccident->m_bIsTreated) {
            CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
            return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
        } else {
            FindNearestAccident(ped->GetPosition());
            if (m_pAccident) {
                m_pAccident->m_bIsTreated = true;
                CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
                return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL);
            } else {
                return CreateSubTask(m_bIsDriver ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_SIMPLE_STAND_STILL);
            }
        }
    }

    if (subTaskId == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        // Como el original: sqrt(y·y + x·x) y |z| comparados con > (NaN trata el accidente)
        const auto& pedPos = ped->GetPosition();
        const float dx = pedPos.x - m_vecAccidentPosition.x, dy = pedPos.y - m_vecAccidentPosition.y, dz = pedPos.z - m_vecAccidentPosition.z;
        if (!(std::sqrt(dy * dy + dx * dx) > 1.0F) && !(std::fabs(dz) > 2.0F))
            return CreateSubTask(TASK_COMPLEX_TREAT_ACCIDENT);
        else
            return CreateDealWithNextAccidentTask(ped, m_pAccident);
    }

    return nullptr;
}

// 0x65ABF0
CTask* CTaskComplexMedicTreatInjuredPed::ControlSubTask(CPed* ped) {
    const auto subTask = m_pSubTask;

    switch (subTask->GetTaskType()) {
    case TASK_SIMPLE_CAR_DRIVE: {
        // Como el original: todo exige ir en vehículo; el conductor solo mira si su compañero ya es pasajero
        if (!ped->bInVehicle) {
            break;
        }
        if (m_bIsDriver) {
            if (m_pVehicle->IsPassenger(m_pPartnerMedic)) {
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER);
            }
            break;
        }
        if (m_bLeftCarOnce || !m_pPartnerMedic) {
            break;
        }
        const auto partnerTask = m_pPartnerMedic->GetTaskManager().GetActiveTask();
        if (!partnerTask || partnerTask->GetTaskType() != TASK_COMPLEX_MEDIC_TREAT_INJURED_PED) {
            break;
        }
        const auto taskTreat = static_cast<CTaskComplexMedicTreatInjuredPed*>(partnerTask);
        if (!taskTreat->m_pAccident || !taskTreat->m_pAccident->m_pPed) {
            break;
        }
        m_vecAccidentPosition = taskTreat->m_vecAccidentPosition;
        if ((m_pVehicle->GetPosition() - m_vecAccidentPosition).SquaredMagnitude() < 10.0F * 10.0F) {
            ped->Say(CTX_GLOBAL_VAN, 0, 1.0F);
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR);
        }
        break;
    }
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT: {
        if (!ped->bInVehicle) {
            break;
        }
        if (!m_pAccident || !m_pAccident->m_pPed) {
            m_pAccident = nullptr; // Como el original
            return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER);
        }
        if ((m_pVehicle->GetPosition() - m_vecAccidentPosition).SquaredMagnitude() < 10.0F * 10.0F) {
            ped->Say(CTX_GLOBAL_VAN, 0, 1.0F);
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR);
        }
        break;
    }
    case TASK_COMPLEX_CAR_DRIVE_WANDER: {
        if (!ped->bInVehicle) {
            break;
        }
        FindNearestAccident(ped->GetPosition());
        if (m_pAccident) {
            m_pAccident->m_bIsTreated = true;
            CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
            return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_TO_POINT);
        }
        break;
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        if (!m_pAccident || !m_pAccident->m_pPed) {
            return CreateDealWithNextAccidentTask(ped, nullptr);
        }

        const auto goToTask = static_cast<CTaskComplexGoToPointAndStandStill*>(subTask);
        if (goToTask->m_moveState == PEDMOVE_WALK) {
            break;
        }
        // Como el original: (accidente - peatón), (x·x + y·y) + z·z < 25 (NaN no sigue)
        if (!((m_vecAccidentPosition - ped->GetPosition()).SquaredMagnitude() < 25.0F)) {
            break;
        }
        for (const auto entity : ped->GetIntelligence()->m_vehicleScanner.m_apEntities) {
            if (entity && CPedGeometryAnalyser::IsEntityBlockingTarget(entity, m_vecAccidentPosition, 0.125F)) {
                return CreateDealWithNextAccidentTask(ped, nullptr);
            }
        }

        CalcTargetPosWithOffset(ped, m_pAccident->m_pPed);
        goToTask->m_moveState = PEDMOVE_WALK;
        goToTask->GoToPoint(m_vecAccidentPosition, 0.5F, 2.0F, false);
        break;
    }
    }

    return subTask;
}
