/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointAnyMeans
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

#include "TaskComplexGoToPointAnyMeans.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskSimpleCreateCarAndGetIn.h"
#include "CarEnterExit.h"

void CTaskComplexGoToPointAnyMeans::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAnyMeans, 0x86FF68, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "1", 0x66B720, CTaskComplexGoToPointAnyMeans*(CTaskComplexGoToPointAnyMeans::*)(int32, CVector const&, float, int32));
    RH_ScopedOverloadedInstall(Constructor, "2", 0x66B790, CTaskComplexGoToPointAnyMeans*(CTaskComplexGoToPointAnyMeans::*)(int32, CVector const&, CVehicle*, float, int32));
    RH_ScopedInstall(Destructor, 0x66B830);
    RH_ScopedInstall(CreateSubTask, 0x6705D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6728A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6729C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x672A50);
}

// 0x66B720
CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, float radius, int32 modelId) : CTaskComplex() {
    m_Pos = posn;
    m_fRadius = radius;
    m_MoveState = static_cast<eMoveState>(moveState);
    m_nModelId = modelId;
    m_nStartTimeInMs = 0;
    m_nTimeOffsetInMs = 0;
    m_bRefreshTime = false;
    m_bResetStartTime = false;
    m_Vehicle = nullptr;
}

// optimized (DRY)
// 0x66B790
CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, CVehicle* vehicle, float radius, int32 modelId)
    : CTaskComplexGoToPointAnyMeans(moveState, posn, radius, modelId)
{
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x66B830
CTaskComplexGoToPointAnyMeans::~CTaskComplexGoToPointAnyMeans() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x6705D0
CTask* CTaskComplexGoToPointAnyMeans::CreateSubTask(int32 taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver(m_Vehicle);
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT:
        // Como el original: velocidad, radio y modelo en -1 y estilo de conducción 0
        return new CTaskComplexDriveToPoint(m_Vehicle, m_Pos, -1.0f, 0, MODEL_INVALID, -1.0f, DRIVING_STYLE_STOP_FOR_CARS);
    case TASK_SIMPLE_CREATE_CAR_AND_GET_IN:
        return new CTaskSimpleCreateCarAndGetIn(ped->GetPosition(), m_nModelId);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill(m_MoveState, m_Pos, m_fRadius, 2.0f, false, false);
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
        return new CTaskComplexFollowNodeRoute(m_MoveState, m_Pos, m_fRadius, 3.0f, 2.0f, false, -1, true);
    default: // incluida TASK_FINISHED
        return nullptr;
    }
}

// 0x6728A0
CTask* CTaskComplexGoToPointAnyMeans::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_CREATE_CAR_AND_GET_IN:
        return CreateSubTask(ped->bInVehicle ? TASK_COMPLEX_CAR_DRIVE_TO_POINT : TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        // Como el original: se lee el indicador del peatón y después el vehículo
        if (ped->bInVehicle && ped->m_pVehicle) {
            return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_TO_POINT, ped);
        }
        return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT:
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
        return CreateSubTask(TASK_FINISHED, ped);
    default: // incluida TASK_FINISHED
        return nullptr;
    }
}

// 0x6729C0
CTask* CTaskComplexGoToPointAnyMeans::CreateFirstSubTask(CPed* ped) {
    if (m_Vehicle) {
        return CreateSubTask(ped->IsInVehicle() ? TASK_COMPLEX_CAR_DRIVE_TO_POINT : TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
    }

    if (ped->IsInVehicle() && ped->m_pVehicle->IsDriver(ped))
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_TO_POINT, ped);
    else
        return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
}

// 0x672A50
CTask* CTaskComplexGoToPointAnyMeans::ControlSubTask(CPed* ped) {
    CTask* const subTask = m_pSubTask;
    if (subTask->GetTaskType() != TASK_COMPLEX_FOLLOW_NODE_ROUTE) {
        return subTask;
    }

    // Como el original: con un modelo para crear el coche se arma el temporizador de 3000 ms la primera vez
    if (m_nModelId != -1) {
        if (!m_bRefreshTime) {
            m_nStartTimeInMs  = CTimer::m_snTimeInMilliseconds;
            m_nTimeOffsetInMs = 3000;
            m_bRefreshTime    = true;
        }
    }
    if (m_bRefreshTime) {
        if (m_bResetStartTime) {
            m_nStartTimeInMs  = CTimer::m_snTimeInMilliseconds;
            m_bResetStartTime = false;
        }
        // Como el original: comparación sin signo
        if (!(CTimer::m_snTimeInMilliseconds < m_nTimeOffsetInMs + m_nStartTimeInMs)) {
            CTask* const created = CreateSubTask(TASK_SIMPLE_CREATE_CAR_AND_GET_IN, ped);
            m_nTimeOffsetInMs = 3000;
            m_bRefreshTime    = true;
            m_nStartTimeInMs  = CTimer::m_snTimeInMilliseconds;
            return created;
        }
    }

    // Como el original: distancia al cuadrado (dz² + dy²) + dx² frente a 50² (0x86FD04)
    const CVector& pedPos = ped->GetPosition();
    const float dx = pedPos.x - m_Pos.x;
    const float dy = pedPos.y - m_Pos.y;
    const float dz = pedPos.z - m_Pos.z;
    const float distSq = (dz * dz + dy * dy) + dx * dx;
    if (distSq > 50.0f * 50.0f) {
        // Como el original: el escáner de coches de la inteligencia (+0xD4) da el más cercano
        CVehicle* const closest = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange();
        if (closest) {
            if (closest != m_Vehicle && CCarEnterExit::IsVehicleStealable(closest, ped)) {
                // Como el original: no se libera la referencia anterior; se escribe el puntero y se registra
                m_Vehicle = closest;
                CTask* const created = CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
                m_Vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_Vehicle));
                return created;
            }
        }
    }
    return subTask;
}
