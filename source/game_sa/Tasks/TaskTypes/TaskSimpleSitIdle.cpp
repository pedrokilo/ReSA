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
 * Validación registrada: 7/7 entradas.
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
    // Como el original: el temporizador se arranca sin comprobar que la duracion sea >= 0 (no CTaskTimer::Start)
    m_animStartedTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_animStartedTimer.m_nInterval  = m_durationMs;
    m_animStartedTimer.m_bStarted   = true;

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
            m_anim = nullptr; // Como el original: mov dword ptr [esi+10h],0
        }
        if (g_ikChainMan.IsLooking(ped)) { // Como el original: se comprueba antes de abortar
            g_ikChainMan.AbortLookAt(ped, 750);
        }
        return true;
    }
    default: {
        // Como el original: tambien la duracion pasa a -1 (or eax,0FFFFFFFFh / mov [esi+0Ch],eax)
        m_durationMs = -1;
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
    // Como el original: (int)(rand() * (1/32768) * 100) < 5, no RandomBool
    if (!g_ikChainMan.IsLooking(ped) && static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * (1.f / 32768.f) * 100.f) < 5) {
        const auto closestPed = ped->GetIntelligence()->GetPedScanner().GetClosestPedInRange();
        // Como el original: 3000 - (int)(rand() * (1/32768) * -5000), en [3000, 8000)
        const auto lookTime = 3000 - static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * (1.f / 32768.f) * -5000.f);

        // First try looking at the closest ped (if any)
        const auto hasLookedAtClosestPed = [&] {
            if (!closestPed) {
                return false;
            }
            if (DotProduct(closestPed->GetPosition() - ped->GetPosition(), ped->GetForwardVector()) <= 0.2f) { // Must be in cone of [-78, 78] deg in front of `ped`
                return false;
            }
            if (CGeneral::GetRandomIntInRange(0, 100) >= 25) { // Como el original: 0x407180 despues del producto escalar
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
            // Como el original: (pos + forward * [1, 5]) + right * [-4, 4], en ese orden
            CVector pos = ped->GetPosition();
            const auto fwdScale = CGeneral::GetRandomNumberInRange(1.f, 5.f);
            pos += ped->GetForwardVector() * fwdScale;
            const auto rightScale = CGeneral::GetRandomNumberInRange(-4.f, 4.f);
            pos += ped->GetRightVector() * rightScale;
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
        if (g_ikChainMan.IsLooking(ped)) { // Como el original: se comprueba antes de abortar
            g_ikChainMan.AbortLookAt(ped, 750u);
        }
        return true;
    }

    return false;
}
