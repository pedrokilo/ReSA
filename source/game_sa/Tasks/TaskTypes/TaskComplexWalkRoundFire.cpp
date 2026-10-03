/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundFire
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexWalkRoundFire.h"
#include "TaskSimpleGoToPoint.h"

void CTaskComplexWalkRoundFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundFire, 0x86f3e8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655720);
    RH_ScopedInstall(Destructor, 0x655780);

    RH_ScopedInstall(ComputeDetourTarget, 0x6581B0);

    RH_ScopedVMTInstall(Clone, 0x655D40);
    RH_ScopedVMTInstall(GetTaskType, 0x655770);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x655790);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6584E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6585C0);
}

// 0x655720
CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(eMoveState moveState, CVector const& firePos, float fireRadius, CVector const& targetPos) :
    m_fireBoundSphere{firePos, fireRadius},
    m_moveState{moveState},
    m_targetPos{targetPos}
{
}

CTaskComplexWalkRoundFire::CTaskComplexWalkRoundFire(const CTaskComplexWalkRoundFire& o) :
    CTaskComplexWalkRoundFire{o.m_moveState, o.m_fireBoundSphere.m_vecCenter, o.m_fireBoundSphere.m_fRadius, o.m_targetPos}
{
}

// 0x6581B0
bool CTaskComplexWalkRoundFire::ComputeDetourTarget(const CPed& ped, CVector& outTarget) const {
    // Como el original: CColSphere en la pila con Set(radio, centro, 0, 0, 0xFF) (0x40FD10)
    CColSphere sphere;
    sphere.Set(m_fireBoundSphere.m_fRadius, m_fireBoundSphere.m_vecCenter, SURFACE_DEFAULT, 0, tColLighting(0xFF));
    CVector unused;
    return CPedGeometryAnalyser::ComputeRouteRoundSphere(
        ped,
        sphere,
        m_initialPedPos,
        m_targetPos,
        unused,
        outTarget
    );
}

// 0x6584E0
CTask* CTaskComplexWalkRoundFire::CreateFirstSubTask(CPed* ped) {
    m_initialPedPos = ped->GetPosition();
    const auto target = GetDetourTarget(*ped); // Como el original: la ruta se calcula antes de reservar la tarea
    return new CTaskSimpleGoToPoint{ m_moveState, target };
}

// 0x6585C0
CTask* CTaskComplexWalkRoundFire::ControlSubTask(CPed* ped) {
    static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->UpdatePoint(GetDetourTarget(*ped));
    return m_pSubTask;
}

// NOTSA
CVector CTaskComplexWalkRoundFire::GetDetourTarget(const CPed& ped) const {
    CVector detourTargetPos{};
    (void)ComputeDetourTarget(ped, detourTargetPos); // TODO: Use a macro to check if return value is true (in debug mode)
    return detourTargetPos;
}
