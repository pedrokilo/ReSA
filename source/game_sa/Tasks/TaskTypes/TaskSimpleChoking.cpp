/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleChoking
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

#include "TaskSimpleChoking.h"

// 0x620480: al terminar la animacion la tarea olvida su animacion y queda terminada
static void ChokeFinishCallback(CAnimBlendAssociation*, void* data) {
    const auto self = static_cast<CTaskSimpleChoking*>(data);
    self->m_pAnim     = nullptr;
    self->m_bIsFinished = true;
}

// Como el original: 1000 - _ftol2(rand() & 0xFFFF * 2^-15 * -1000) (x87 a 24 bits)
static uint32 RandomChokeTime() {
    return 1000u - static_cast<uint32>(static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * 3.0517578125e-05f * -1000.0f));
}

// Como el original: rand() * (1/32767) * k + base con cada operacion redondeada a float
static float RandomChokeSpeed(float k, float base) {
    return static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL * k + base;
}

// 0x6202C0
CTaskSimpleChoking::CTaskSimpleChoking(CPed* attacker, bool bIsTeargas) :
    m_pAttacker{attacker},
    m_bIsTeargas{bIsTeargas}
{
    m_pAnim       = nullptr;
    m_bIsFinished = false;
    CEntity::SafeRegisterRef(m_pAttacker);
    m_nTimeRemaining = RandomChokeTime();
    m_nTimeStarted   = CTimer::GetTimeInMS();
}

// 0x623220
CTaskSimpleChoking::CTaskSimpleChoking(const CTaskSimpleChoking& o) :
    CTaskSimpleChoking{o.m_pAttacker, o.m_bIsTeargas}
{
}

// 0x620370
CTaskSimpleChoking::~CTaskSimpleChoking() {
    if (m_pAnim) {
        m_pAnim->m_BlendDelta = -4.0f;
        m_pAnim->SetDefaultFinishCallback();
    }
    CEntity::SafeCleanUpRef(m_pAttacker);
}

// 0x6203F0
bool CTaskSimpleChoking::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        // Como el original: un evento de prioridad menor que 57 no la interrumpe
        if (event && event->GetEventPriority() < 57) {
            return false;
        }
        if (m_pAnim) {
            m_pAnim->m_BlendDelta = -4.0f;
            m_pAnim->SetDefaultFinishCallback();
            m_pAnim = nullptr;
        }
        m_bIsFinished = true;
        return true;
    }
    if (m_pAnim) {
        m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        m_pAnim->m_BlendDelta = -4.0f;
        m_pAnim->SetDefaultFinishCallback();
        m_pAnim = nullptr;
    }
    return true;
}

// 0x620490
bool CTaskSimpleChoking::ProcessPed(CPed* ped) {
    if (m_bIsFinished) {
        return true;
    }

    if (m_pAttacker && !ped->IsPlayer() && !m_bIsTeargas) {
        // Como el original: la matriz del peaton se usa sin guarda de nulo; el producto escalar se suma como (z + y) + x
        const auto& pedPos      = ped->GetPosition();
        const auto& attackerPos = m_pAttacker->GetPosition();
        const auto& forward     = ped->m_matrix->GetForward();
        const float dx = attackerPos.x - pedPos.x;
        const float dy = attackerPos.y - pedPos.y;
        const float dz = attackerPos.z - pedPos.z;
        const float dot = dz * forward.z + dy * forward.y + dx * forward.x;
        if (dot > 0.0f) { // fcomp + test ah,41h: solo con dot estrictamente positivo (NaN no)
            // fpatan trabaja a precision completa: atan2(-dx, dy) como en el EXE
            const float negDx = -dx;
            float       result;
            __asm {
                fld   dword ptr [negDx]
                fld   dword ptr [dy]
                fpatan
                fstp  dword ptr [result]
            }
            ped->m_fAimingRotation = result;
        }
    }

    // Como el original: el tiempo restante baja (_ftol2(timeStep * 0.02 * 1000), conversion de 64 bits) sin pasar de cero (comparacion sin signo)
    const auto elapsed = static_cast<uint32>(static_cast<int64>(CTimer::ms_fTimeStep * 0.02f * 1000.0f));
    m_nTimeRemaining = m_nTimeRemaining > elapsed ? m_nTimeRemaining - elapsed : 0;

    if (!m_pAnim) {
        m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.0f);
        m_pAnim->SetFinishCallback(ChokeFinishCallback, this);
        m_pAnim->m_Speed = RandomChokeSpeed(0.25f, 0.75f);
    } else if (m_nTimeRemaining == 0) {
        if (m_pAnim->m_AnimId == ANIM_ID_GAS_CWR) {
            m_pAnim->SetDefaultFinishCallback();
            m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_IDLE_TIRED, 4.0f);
            m_pAnim->SetFinishCallback(ChokeFinishCallback, this);
            m_nTimeRemaining = CTimer::GetTimeInMS() - m_nTimeStarted;
            if (m_nTimeRemaining >= static_cast<uint32>(CGeneral::GetRandomIntInRange(8000, 12000))) {
                m_nTimeRemaining = CGeneral::GetRandomIntInRange(8000, 12000);
            }
        } else {
            m_pAnim->m_BlendDelta = -4.0f;
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        }
    }

    ped->Say(CTX_GLOBAL_PAIN_COUGH, 0, 1.0f, false, false, false);
    return false;
}

// 0x620660
void CTaskSimpleChoking::UpdateChoke(CPed* victim, CPed* attacker, bool bIsTeargas) {
    m_bIsTeargas = bIsTeargas;
    if (m_pAttacker != attacker) {
        CEntity::ChangeEntityReference(m_pAttacker, attacker);
    }
    m_bIsFinished = false;
    if (m_pAnim && m_pAnim->m_AnimId != ANIM_ID_GAS_CWR) {
        m_pAnim->SetDefaultFinishCallback();
        m_pAnim = CAnimManager::BlendAnimation(victim->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_GAS_CWR, 4.0f);
        m_pAnim->SetFinishCallback(ChokeFinishCallback, this);
        m_pAnim->m_Speed = RandomChokeSpeed(std::bit_cast<float>(0x3E99999Au), std::bit_cast<float>(0x3F4CCCCDu));
        m_nTimeStarted = CTimer::GetTimeInMS() - m_nTimeRemaining;
    }
    m_nTimeRemaining = RandomChokeTime();
}

void CTaskSimpleChoking::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleChoking, 0x86d820, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6202C0);
    RH_ScopedInstall(Destructor, 0x620370);

    RH_ScopedInstall(UpdateChoke, 0x620660);

    RH_ScopedVMTInstall(Clone, 0x623220);
    RH_ScopedVMTInstall(GetTaskType, 0x620360);
    RH_ScopedVMTInstall(MakeAbortable, 0x6203F0);
    RH_ScopedVMTInstall(ProcessPed, 0x620490);
}
