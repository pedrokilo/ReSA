/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialWalkIntoFire
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPotentialWalkIntoFire.h"


void CEventPotentialWalkIntoFire::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoFire, 0x85B6A0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1E20);
    RH_ScopedVMTInstall(AffectsPed, 0x4B6890);
}

// 0x4B1E20
CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(const CVector& firePos, float fireSize, eMoveState moveState) :
    m_firePos{firePos},
    m_fireSize{fireSize},
    m_moveState{moveState}
{
    if (fireSize < 1.0f) {
        m_radius = 0.7f;
    } else {
        m_radius = fireSize >= 2.0f
            ? 1.5f
            : 1.0f;
    }
    m_radius += 0.35f;
}

// 0x4B1E20
CEventPotentialWalkIntoFire* CEventPotentialWalkIntoFire::Constructor(const CVector& firePos, float fireSize, eMoveState moveState) {
    this->CEventPotentialWalkIntoFire::CEventPotentialWalkIntoFire(firePos, fireSize, moveState);
    return this;
}

// 0x4B6890
bool CEventPotentialWalkIntoFire::AffectsPed(CPed* ped) {
    if (ped->IsAlive() && m_moveState != PEDMOVE_STILL) {
        auto goToTask = static_cast<CTaskSimpleGoTo*>(ped->GetTaskManager().GetSimplestActiveTask());
        if (goToTask && CTask::IsGoToTask(goToTask)) {
            CColSphere colSphere;
            colSphere.Set(m_radius, m_firePos, SURFACE_DEFAULT, 0, tColLighting(0xFF));
            CVector intersectPoint2;
            CVector intersectPoint1;
            return colSphere.IntersectEdge(ped->GetPosition(), goToTask->m_vecTargetPoint, intersectPoint1, intersectPoint2);
        }
    }
    return false;
}