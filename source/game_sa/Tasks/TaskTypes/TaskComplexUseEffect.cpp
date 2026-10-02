/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseEffect
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

#include "TaskComplexUseEffect.h"
#include "TaskComplexGoToAttractor.h"
#include "TaskSimpleNone.h"

#include "EventPedCollisionWithPed.h"
#include "Scripted2dEffects.h"

void CTaskComplexUseEffect::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffect, 0x86e0d8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6321F0);
    RH_ScopedInstall(Destructor, 0x632270);

    RH_ScopedVMTInstall(Clone, 0x636710);
    RH_ScopedVMTInstall(GetTaskType, 0x632260);
    RH_ScopedVMTInstall(MakeAbortable, 0x6322F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632420);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632460);
    RH_ScopedVMTInstall(ControlSubTask, 0x632570);
}

// 0x6321F0
CTaskComplexUseEffect::CTaskComplexUseEffect(C2dEffectPedAttractor* attractor, CEntity* entity) :
    m_2dFx{attractor},
    m_Entity{entity}
{
    CEntity::SafeRegisterRef(m_Entity);
}

// For 0x636710
CTaskComplexUseEffect::CTaskComplexUseEffect(const CTaskComplexUseEffect& o) :
    CTaskComplexUseEffect{o.m_2dFx, o.m_Entity}
{
}

// 0x632270
CTaskComplexUseEffect::~CTaskComplexUseEffect() {
    if (m_Ped && m_PedAttractor) {
        GetPedAttractorManager()->DeRegisterPed(m_Ped, m_PedAttractor);
    }
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x6322F0
bool CTaskComplexUseEffect::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }

    GetPedAttractorManager()->DeRegisterPed(ped, m_PedAttractor);
    m_PedAttractor = nullptr;
    m_Ped          = nullptr;

    if (!event || m_pSubTask->GetTaskType() != TASK_COMPLEX_GO_TO_ATTRACTOR) {
        return true;
    }
    if (!notsa::contains({ EVENT_PED_COLLISION_WITH_PED, EVENT_PED_COLLISION_WITH_PLAYER }, event->GetEventType())) {
        return true;
    }
    const auto& attrPos = static_cast<CTaskComplexGoToAttractor*>(m_pSubTask)->m_vecAttrPosn;
    if ((ped->GetPosition() - attrPos).SquaredMagnitude2D() < sq(1.5f)) {
        // El original mira el estado de movimiento de la víctima, no el de la tarea.
        if (const auto victim = static_cast<const CEventPedCollisionWithPed*>(event)->m_victim) {
            if ((victim->GetPosition() - attrPos).SquaredMagnitude2D() < sq(1.5f) && victim->m_nMoveState == PEDMOVE_STILL) {
                m_bAbort = true;
            }
        }
    }
    return true;
}

// 0x632420
CTask* CTaskComplexUseEffect::CreateNextSubTask(CPed* ped) {
    if (m_PedAttractor && GetPedAttractorManager()->GetRelevantAttractor(ped, m_2dFx, m_Entity) == m_PedAttractor) {
        return m_PedAttractor->GetTaskForPed(ped);
    }
    return nullptr;
}

// 0x632460
CTask* CTaskComplexUseEffect::CreateFirstSubTask(CPed* ped) {
    CEntity::ChangeEntityReference(m_Ped, ped);
    m_PedAttractor = !m_bAbort && (m_Entity || CScripted2dEffects::GetIndex(m_2dFx) >= 0)
        ? GetPedAttractorManager()->RegisterPedWithAttractor(ped, m_2dFx, m_Entity, m_MoveState)
        : nullptr;
    return m_PedAttractor
        ? m_PedAttractor->GetTaskForPed(ped)
        : new CTaskSimpleNone{};
}

// 0x632570
CTask* CTaskComplexUseEffect::ControlSubTask(CPed* ped) {
    if (ped->IsCop()) {
        // El original vuelve a llamar a FindPlayerWanted para CanCopJoinPursuit.
        if (FindPlayerWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN && FindPlayerWanted()->CanCopJoinPursuit(ped->AsCop())) {
            if (m_pSubTask->MakeAbortable(ped)) {
                GetPedAttractorManager()->DeRegisterPed(ped, m_PedAttractor);
                return nullptr;
            }
        }
    }
    return m_PedAttractor
        ? m_PedAttractor->GetTaskForPed(ped)
        : CreateFirstSubTask(ped);
}
