/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialWalkIntoPed
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventPotentialWalkIntoPed.h"
#include "TaskComplexFollowPedFootsteps.h"

void CEventPotentialWalkIntoPed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoPed, 0x85AF60, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AE6E0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AE800);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AE950);
}

// 0x4AE6E0
CEventPotentialWalkIntoPed::CEventPotentialWalkIntoPed(CPed* ped, const CVector& targetPoint, eMoveState moveState) :
    m_targetPoint{targetPoint},
    m_ped{ped},
    m_moveState{moveState}
{
    CEntity::SafeRegisterRef(m_ped);
}

CEventPotentialWalkIntoPed::~CEventPotentialWalkIntoPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4AE800
bool CEventPotentialWalkIntoPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive() || !m_ped || m_moveState == PEDMOVE_STILL) {
        return false;
    }

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>(m_ped->GetTaskManager())) {
        return false;
    }

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexFollowPedFootsteps>()) {
        if (task->m_targetPed == m_ped) {
            return false;
        }
    }

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>()) {
        if (task->m_target == m_ped) {
            return false;
        }
    }

    if (const auto simplest = ped->GetTaskManager().GetSimplestActiveTask()) {
        if (CTask::IsGoToTask(simplest)) {
            return true;
        }
    }

    return false;
}

// 0x4AE950
bool CEventPotentialWalkIntoPed::TakesPriorityOver(const CEvent& refEvent) {
    return CEventHandler::IsTemporaryEvent(refEvent) ? true : CEvent::TakesPriorityOver(refEvent);
}