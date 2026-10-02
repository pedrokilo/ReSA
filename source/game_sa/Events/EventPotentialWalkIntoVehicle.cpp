/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialWalkIntoVehicle
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPotentialWalkIntoVehicle.h"


void CEventPotentialWalkIntoVehicle::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoVehicle, 0x85AED0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AE320);
    RH_ScopedVMTInstall(AffectsPed, 0x4AE420);
}

// 0x4AE320
CEventPotentialWalkIntoVehicle::CEventPotentialWalkIntoVehicle(CVehicle* vehicle, int32 moveState) {
    m_vehicle = vehicle;
    m_moveState = moveState;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventPotentialWalkIntoVehicle::~CEventPotentialWalkIntoVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4AE320
CEventPotentialWalkIntoVehicle* CEventPotentialWalkIntoVehicle::Constructor(CVehicle* vehicle, int32 moveState) {
    this->CEventPotentialWalkIntoVehicle::CEventPotentialWalkIntoVehicle(vehicle, moveState);
    return this;
}

// 0x4AE420
bool CEventPotentialWalkIntoVehicle::AffectsPed(CPed* ped) {
    auto taskEnterCarAsDriver = ped->GetTaskManager().Find<CTaskComplexEnterCarAsDriver>();
    auto goToTask = reinterpret_cast<CTaskSimpleGoTo*>(ped->GetTaskManager().GetSimplestActiveTask());
    if (ped->IsPlayer() && !taskEnterCarAsDriver && !CTask::IsGoToTask(goToTask))
        return false;

    if (!ped->IsAlive() || ped->bInVehicle || m_moveState == PEDMOVE_STILL)
        return false;

    // Como el original: con CTaskComplexWalkRoundCar activa siempre devuelve false; si el vehiculo es su remolque o
    // tractor se cambia, pasando el campo de 4 bits (con signo) de la direccion de rodeo del byte 0xD
    if (const auto task = ped->GetTaskManager().Find<CTaskComplexWalkRoundCar>()) {
        if (task->m_Veh != m_vehicle) {
            if ((m_vehicle->m_pVehicleBeingTowed && m_vehicle->m_pVehicleBeingTowed == task->m_Veh) ||
                (m_vehicle->m_pTowingVehicle && m_vehicle->m_pTowingVehicle == task->m_Veh)
            ) {
                // bits 2-5 del byte 0xD con signo: (int8)(b << 2) >> 4
                const auto dir = static_cast<int8>(reinterpret_cast<const uint8*>(task)[0xD] << 2) >> 4;
                task->SetNewVehicle(m_vehicle, static_cast<uint8>(dir));
            }
        }
        return false;
    }

    if (!ped->m_pAttachedTo && m_vehicle && m_vehicle->IsSubVehicleTypeValid() && !m_vehicle->IsSubFakeAircraft()) {
        bool isGoToPointTask = false;
        CVector targetPos;
        if (goToTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            targetPos = goToTask->m_vecTargetPoint;
            isGoToPointTask = true;
        }
        if (taskEnterCarAsDriver) {
            if (m_vehicle == taskEnterCarAsDriver->GetTargetCar() && m_vehicle->IsSubPlane())
                return false;

            targetPos = taskEnterCarAsDriver->GetTargetPos();
        }
        if (isGoToPointTask || taskEnterCarAsDriver) {
            CVector surfacePoint;
            CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_vehicle, surfacePoint);
            int32 hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(surfacePoint, *m_vehicle);
            if (hitSide != CPedGeometryAnalyser::ComputeEntityHitSide(targetPos, *m_vehicle))
                return true;

            return false;
        }
        return true;
    }
    return false;
}