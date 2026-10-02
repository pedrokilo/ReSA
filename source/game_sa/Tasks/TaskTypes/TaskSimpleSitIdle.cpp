/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSitIdle
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleSitIdle.h"
#include "Ragdoll/IKChainManager.h"

void CTaskSimpleSitIdle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSitIdle, 0x86df18, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631160);
    RH_ScopedInstall(Destructor, 0x6311B0);

    RH_ScopedInstall(StartAnim, 0x631260);

    RH_ScopedVMTInstall(Clone, 0x636280);
    RH_ScopedVMTInstall(GetTaskType, 0x6311A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6311E0);
    RH_ScopedVMTInstall(ProcessPed, 0x637A60);
}

// 0x631160
CTaskSimpleSitIdle::CTaskSimpleSitIdle(int32 durationMs, bool sitAfterStep) :
    m_durationMs{durationMs},
    m_bSitAfterStep{sitAfterStep}
{
}

CTaskSimpleSitIdle::CTaskSimpleSitIdle(const CTaskSimpleSitIdle& o) :
    CTaskSimpleSitIdle{o.m_durationMs, o.m_bSitAfterStep}
{
}

// 0x631260
void CTaskSimpleSitIdle::StartAnim(CPed* ped) {
    m_animStartedTimer.Start(m_durationMs);

    const auto StartAnim = [this, ped](auto grpId, auto animId) { m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), grpId, animId, 256.f); };
    if (m_bSitAfterStep) {
        StartAnim(ANIM_GROUP_ATTRACTORS, ANIM_ID_STEPSIT_LOOP);
    } else {
        StartAnim(ANIM_GROUP_DEFAULT, ANIM_ID_SEAT_IDLE);
    }

    if (m_anim) {
        m_anim->m_Speed = CGeneral::GetRandomNumberInRange(0.9f, 1.5f);
    }
}

// 0x6311E0
bool CTaskSimpleSitIdle::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
            m_anim->SetDefaultFinishCallback();
        }
        g_ikChainMan.AbortLookAt(ped, 750); // Omitted redudant `IsLooking` check.
        return true;
    }
    default: {
        m_animStartedTimer.SetOutOfTime(); // Eventually leads to `ProcessPed` returning `true` that finishes the task
        return false;
    }
    }
}

// 0x637A60
bool CTaskSimpleSitIdle::ProcessPed(CPed* ped) {
    if (!m_anim) {
        StartAnim(ped);
    }

    // Do look at somewhere/another ped with random chance
    if (!g_ikChainMan.IsLooking(ped) && CGeneral::RandomBool(5.f)) {
        const auto lookTime = CGeneral::GetRandomNumberInRange(3000, 5000);

        // First try looking at the closest ped (if any)
        const auto hasLookedAtClosestPed = [this, ped, lookTime] {
            const auto closestPed = ped->GetIntelligence()->GetPedScanner().GetClosestPedInRange();
            if (!closestPed) {
                return false;
            }
            if (!CGeneral::RandomBool(25.f)) { // Moved this early out here to optimize
                return false;
            }
            if (DotProduct(closestPed->GetPosition() - ped->GetPosition(), ped->GetForwardVector()) <= 0.2f) { // Must be in cone of [-78, 78] deg in front of `ped`
                return false;
            }

            g_ikChainMan.LookAt(
                "TaskSitIdle",
                ped,
                closestPed,
                lookTime,
                BONE_HEAD,
                nullptr,
                false,
                0.25f,
                750u,
                3,
                false
            );

            return true;
        }();

        // If that failed, look at a random point somewhere in front of the ped
        if (!hasLookedAtClosestPed) {
            CVector pos = ped->GetPosition()
                        + ped->GetForwardVector() * CGeneral::GetRandomNumberInRange(1.f, 5.f)
                        + ped->GetRightVector() * CGeneral::GetRandomNumberInRange(-4.f, 4.f);
            g_ikChainMan.LookAt(
                "TaskSitIdle",
                ped,
                nullptr,
                lookTime,
                BONE_UNKNOWN,
                &pos,
                false,
                0.25f,
                750u,
                3,
                false
            );
        }
    }

    if (m_animStartedTimer.IsOutOfTime()) {
        g_ikChainMan.AbortLookAt(ped, 750u); // Omitted redudant `IsLooking` check
        return true;
    }

    return false;
}
