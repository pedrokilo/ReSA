/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUsePairedAttractor
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUsePairedAttractor.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskComplexAttractorPartnerWait.h"
#include "TaskComplexUseAttractorPartner.h"
#include "Scripted2dEffects.h"

void CTaskComplexUsePairedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUsePairedAttractor, 0x86e258, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6331A0);
    RH_ScopedInstall(Destructor, 0x6331E0);

    RH_ScopedInstall(CreateSubTask, 0x638D30);
    RH_ScopedInstall(SetPartnership, 0x6331F0);
    RH_ScopedInstall(ClearPartnership, 0x633220);
    RH_ScopedInstall(SeekPartnership, 0x638AE0);
    RH_ScopedVMTInstall(Clone, 0x636C70);
    RH_ScopedVMTInstall(GetTaskType, 0x6331D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x639E40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639F70);
    RH_ScopedVMTInstall(ControlSubTask, 0x638BD0);
}

// 0x6331A0
CTaskComplexUsePairedAttractor::CTaskComplexUsePairedAttractor(CPedAttractor* attractor) :
    m_Attractor{ attractor }
{
    assert(attractor);
}

// 0x636C70
CTaskComplexUsePairedAttractor::CTaskComplexUsePairedAttractor(const CTaskComplexUsePairedAttractor& o) :
    CTaskComplexUsePairedAttractor{o.m_Attractor}
{
}

// 0x638D30
CTask* CTaskComplexUsePairedAttractor::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_FINISHED: { // 0x638ED0
        GetPedAttractorManager()->BroadcastDeparture(ped, m_Attractor);
        return nullptr;
    }
    case TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR: { // 0x638E65
        ClearPartnership();
        return new CTaskComplexUseScriptedAttractor{ *m_Attractor };
    }
    case TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT: { // 0x638D70
        if (!m_IsLeader) {
            m_Partner->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_USE_PAIRED_ATTRACTOR); // Resultado sin usar en el original
        }
        return new CTaskComplexAttractorPartnerWait{ m_IsLeader, *m_CurrentFxPair };
    }
    case TASK_COMPLEX_USE_ATTRACTOR_PARTNER: { // 0x638DE7
        if (!m_IsLeader) {
            m_Partner->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_USE_PAIRED_ATTRACTOR); // Resultado sin usar en el original
        }
        return new CTaskComplexUseAttractorPartner{ m_IsLeader, *m_CurrentFxPair };
    }
    default:
        return nullptr;
    }
}

// 0x639E40
CTask* CTaskComplexUsePairedAttractor::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_USE_ATTRACTOR_PARTNER: { // 0x639EE7
        if (m_IsLeader) {
            m_PartnerProgress = (m_PartnerProgress + 5) % CScriptedEffectPairs::MAX_NUM_EFFECT_PAIRS;
        }
        CTask* ret = nullptr;
        switch (m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].PartnerUseMode) {
        case CScriptedEffectPair::eMode::USE_PARTNER_ONCE:         ret = CreateSubTask(TASK_FINISHED, ped); break;
        case CScriptedEffectPair::eMode::LOOK_FOR_ANOTHER_PARTNER: ret = CreateSubTask(TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR, ped); break;
        }
        ClearPartnership();
        return ret;
    }
    case TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR: { // 0x639E63
        m_SoloProgress++;
        if (m_Partner) {
            return CreateSubTask(TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT, ped);
        }
        return CreateSubTask(
            // Acceso sin comprobar como el original (un índice -1 lee el byte anterior a la tabla).
            CScripted2dEffects::ms_useAgainFlags.data()[CScripted2dEffects::GetIndex(m_Attractor->m_Fx)]
                ? TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR
                : TASK_FINISHED,
            ped
        );
    }
    case TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT: { // 0x639EB6
        return CreateSubTask(
            m_Partner
                ? TASK_COMPLEX_USE_ATTRACTOR_PARTNER
                : TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR,
            ped
        );
    }
    default:
        return nullptr;
    }
}

// 0x638BD0
CTask* CTaskComplexUsePairedAttractor::ControlSubTask(CPed* ped) {
    const auto PartnerHas = [this](eTaskType tt) { return m_Partner->GetTaskManager().FindActiveTaskByType(tt) != nullptr; };

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_USE_ATTRACTOR_PARTNER: { // 0x638CB8
        // Se aborta salvo que el compañero siga con el atractor emparejado y además use o espere al compañero.
        if (!m_Partner || !PartnerHas(TASK_COMPLEX_USE_PAIRED_ATTRACTOR)
            || (!PartnerHas(TASK_COMPLEX_USE_ATTRACTOR_PARTNER) && !PartnerHas(TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT))) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }
        return m_pSubTask;
    }
    case TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT: { // 0x638C35
        if (!m_Partner) {
            m_IsLeader      = false;
            m_CurrentFxPair = nullptr;
        } else if (!PartnerHas(TASK_COMPLEX_USE_PAIRED_ATTRACTOR)) {
            ClearPartnership();
        } else {
            if (PartnerHas(m_IsLeader ? TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT : TASK_COMPLEX_USE_ATTRACTOR_PARTNER)) {
                m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            }
            return m_pSubTask;
        }
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return m_pSubTask;
    }
    case TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR: { // 0x638BF4
        if (!m_Partner) {
            SeekPartnership(ped);
        }
        if (m_Partner && m_CurrentFxPair->UsePartnerImmediately) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }
        return m_pSubTask;
    }
    default:
        return m_pSubTask;
    }
}

// 0x6331F0
// El original no suelta la referencia anterior (solo se llama sin compañero).
void CTaskComplexUsePairedAttractor::SetPartnership(CPed* partner, bool isLeader, const CScriptedEffectPair* pair) {
    m_Partner = partner;
    partner->RegisterReference(reinterpret_cast<CEntity**>(&m_Partner));
    m_IsLeader      = isLeader;
    m_CurrentFxPair = pair;
}

// 0x633220
void CTaskComplexUsePairedAttractor::ClearPartnership() {
    if (m_Partner) {
        m_Partner->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_Partner));
        m_Partner = nullptr;
    }
    m_IsLeader      = false;
    m_CurrentFxPair = nullptr;
}

// 0x638AE0
void CTaskComplexUsePairedAttractor::SeekPartnership(CPed* us) {
    auto* const pairs = CScripted2dEffects::GetEffectPairs(m_Attractor->m_Fx);
    if (pairs->NumPairs <= 0) {
        return;
    }
    // El original recorre los 4 pares aunque ya haya encontrado compañero.
    for (auto k = 0; k < CScriptedEffectPairs::MAX_NUM_EFFECT_PAIRS; k++) {
        const auto        i    = (m_PartnerProgress + k + 4) % CScriptedEffectPairs::MAX_NUM_EFFECT_PAIRS;
        const auto* const pair = &pairs->Pairs[i];
        if (pair->Effects[0].Effect == -1 || pair->Effects[1].Effect == -1) {
            continue;
        }
        auto* const partner = GetPedAttractorManager()->GetPedUsingEffect(CScripted2dEffects::GetEffect(pair->Effects[1].Effect));
        if (!partner) {
            continue;
        }
        auto* const tPartner = static_cast<CTaskComplexUsePairedAttractor*>(partner->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_USE_PAIRED_ATTRACTOR));
        if (!tPartner || tPartner->m_Partner || m_Partner) {
            continue;
        }
        m_PartnerProgress = i;
        SetPartnership(partner, true, pair);
        tPartner->SetPartnership(us, false, pair);
    }
}
