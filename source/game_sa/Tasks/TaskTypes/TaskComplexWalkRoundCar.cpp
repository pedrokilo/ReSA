/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundCar
 * Reconstrucción: INCOMPLETA — 9/13 funciones reconstruidas.
 * Pendientes de reconstruir: 4.
 * Funciones bloqueadas: 4.
 * Validación registrada: 0/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   ComputeRouteRoundBigCar | 0x00656BB0 | S
 *   ComputeRouteRoundSmallCar | 0x006544F0 | S
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ComputeRouteRoundBigCar | 0x00656BB0 | S
 *   ComputeRouteRoundSmallCar | 0x006544F0 | S
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexWalkRoundCar.h"

void CTaskComplexWalkRoundCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundCar, 0x86f308, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6541B0);
    RH_ScopedInstall(Destructor, 0x656B00);

    RH_ScopedInstall(SetNewVehicle, 0x654290);
    RH_ScopedInstall(CreateRouteTask, 0x6542E0);
    RH_ScopedInstall(ComputeRouteRoundSmallCar, 0x6544F0, { .reversed = false });
    RH_ScopedInstall(GoingForDoor, 0x654720);
    RH_ScopedInstall(ComputeRouteRoundBigCar, 0x656BB0, { .reversed = false });
    RH_ScopedInstall(ComputeRoute, 0x657B80);

    RH_ScopedVMTInstall(Clone, 0x655B00);
    RH_ScopedVMTInstall(GetTaskType, 0x654280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656B70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x658200, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x654370, { .reversed = false });
}

// 0x6541B0
CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(eMoveState moveState, CVector const& targetPt, CVehicle* vehicle, bool isPedGoingForCarDoor, uint8 forceThisDirectionRoundCar) :
    m_MoveState{(uint8)moveState},
    m_bIsPedGoingForCarDoor{isPedGoingForCarDoor},
    m_ForceThisDirectionRoundCar{forceThisDirectionRoundCar},
    m_TargetPt{targetPt},
    m_Veh{vehicle},
    m_Route{new CPointRoute{}}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(const CTaskComplexWalkRoundCar& o) :
    CTaskComplexWalkRoundCar{
        (eMoveState)o.m_MoveState,
        o.m_TargetPt,
        o.m_Veh,
        o.m_bIsPedGoingForCarDoor,
        o.m_ForceThisDirectionRoundCar
    }
{
}

// 0x656B00
CTaskComplexWalkRoundCar::~CTaskComplexWalkRoundCar() {
    CEntity::SafeCleanUpRef(m_Veh);
    delete m_Route;
}


// 0x654290
void CTaskComplexWalkRoundCar::SetNewVehicle(CVehicle * vehicle, uint8 forceThisDirectionRoundCar) {
    if (notsa::IsFixBugs()) {
        if (vehicle == m_Veh) {
            return;
        }
    }
    CEntity::ChangeEntityReference(m_Veh, vehicle);
    m_ForceThisDirectionRoundCar    = forceThisDirectionRoundCar;
    m_bFirstSubTaskNeedsToBeCreated = true;
    m_Route->Clear();
}

// 0x6542E0
CTask* CTaskComplexWalkRoundCar::CreateRouteTask(CPed*) const {
    if (m_Route->IsEmpty()) {
        return nullptr;
    }
    return new CTaskComplexFollowPointRoute{ (eMoveState)m_MoveState, *m_Route };
}

// 0x6544F0
float CTaskComplexWalkRoundCar::ComputeRouteRoundSmallCar(CPed* ped) {
    return plugin::CallMethodAndReturn<float, 0x6544F0, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}

// 0x656BB0
float CTaskComplexWalkRoundCar::ComputeRouteRoundBigCar(CPed* ped) {
    return plugin::CallMethodAndReturn<float, 0x656BB0, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}

// 0x657B80
float CTaskComplexWalkRoundCar::ComputeRoute(CPed* ped) {
    return m_Veh->vehicleFlags.bIsBig
        ? ComputeRouteRoundBigCar(ped)
        : ComputeRouteRoundSmallCar(ped);
}

// 0x656B70
CTask* CTaskComplexWalkRoundCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateRouteTask(ped);
    }
    return nullptr;
}

// 0x658200
CTask* CTaskComplexWalkRoundCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x658200, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}

// 0x654370
CTask* CTaskComplexWalkRoundCar::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x654370, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}
