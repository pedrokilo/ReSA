/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleIKChain
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"
#include "TaskSimpleIKChain.h"
#include "Ragdoll/IKChainManager.h"

void CTaskSimpleIKChain::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKChain, 0x86DB48, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6339C0);
    RH_ScopedInstall(Destructor, 0x633A90);

    RH_ScopedInstall(BlendOut, 0x633C40);
    RH_ScopedInstall(GetIKChain, 0x633C70);
    RH_ScopedVMTInstall(Clone, 0x633B00);
    RH_ScopedVMTInstall(GetTaskType, 0x62EC30);
    RH_ScopedVMTInstall(MakeAbortable, 0x639450);
    RH_ScopedVMTInstall(ProcessPed, 0x633C80);
    RH_ScopedVMTInstall(CreateIKChain, 0x633BD0);
}

// Como el original: el EXE guarda y pasa el hueso de desplazamiento como dword (la palabra alta tambien cuenta)
static eBoneTag32 RawBoneTag32(const eBoneTag16& bone) {
    eBoneTag32 r;
    r.m_Value = *reinterpret_cast<const int32*>(&bone);
    return r;
}

// 0x6339C0
CTaskSimpleIKChain::CTaskSimpleIKChain(
    const char* name,
    eBoneTag32 effectorBone,
    CVector effectorPos,
    eBoneTag32 pivotBone,
    CEntity* entity,
    eBoneTag32 offsetBone,
    CVector offsetPos,
    float speed,
    int32 time,
    int32 blendTime
) :
    m_EffectorBone{ effectorBone },
    m_Duration{ time },
    m_BlendDuration{ blendTime },
    m_EffectorOffset{ effectorPos },
    m_OffsetPos{ offsetPos },
    m_PivotBone{ pivotBone },
    m_TargetEntity{ entity },
    m_TargetEntityExisted{ !!entity },
    m_Speed{ speed }
{
    *reinterpret_cast<int32*>(&m_OffsetBone) = offsetBone.get_underlying(); // Como el original: se escribe como dword
}

// 0x633A90
CTaskSimpleIKChain::~CTaskSimpleIKChain() {
    if (m_IKChain) {
        g_ikChainMan.RemoveIKChain(m_IKChain);
    }
}

// 0x633B00
CTask* CTaskSimpleIKChain::Clone() const {
    auto* task = new CTaskSimpleIKChain("", m_EffectorBone, m_EffectorOffset, m_PivotBone, m_TargetEntity, RawBoneTag32(m_OffsetBone), m_OffsetPos, m_Speed, m_Duration, m_BlendDuration);
    if (m_IKChain) {
        task->m_Blend       = m_Blend;
        task->m_EndTime     = m_EndTime;
        task->m_TargetBlend = m_TargetBlend;
        task->m_TargetTime  = m_TargetTime;
    }
    return task;
}

// 0x633C40
void CTaskSimpleIKChain::BlendOut(int32 blendOutTime) {
    if (m_IsBlendingOut) {
        return;
    }
    if (m_Duration == -1) {
        m_Duration = 0;
    }
    m_EndTime       = CTimer::GetTimeInMS() + blendOutTime;
    m_IsBlendingOut = true;
}

// 0x633C70
IKChain_c* CTaskSimpleIKChain::GetIKChain() const {
    return m_IKChain;
}

// 0x639450
bool CTaskSimpleIKChain::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        return true;
    }
    BlendOut(250);
    return false;
}

// 0x633C80
bool CTaskSimpleIKChain::ProcessPed(CPed* ped) {
    // Como el original: el paso en ms se trunca a entero (_ftol2) y la hora se lee al entrar
    const auto timeStepMs = static_cast<int32>(static_cast<int64>(CTimer::GetTimeStep() * 0.02f * 1000.0f));
    const auto now        = (int32)CTimer::GetTimeInMS();

    if (!m_IKChain) {
        if (m_TargetEntityExisted && !m_TargetEntity) {
            return true;
        }
        if (!CreateIKChain(ped)) {
            return true;
        }
        if (m_Duration == -1) {
            m_EndTime = -1;
        } else {
            m_EndTime = CTimer::GetTimeInMS() + m_Duration;
        }
        m_TargetTime = CTimer::GetTimeInMS() + m_BlendDuration;
        m_TargetBlend = 1.f;
        m_IKChain->SetBlend(m_Blend);
        return false;
    }

    // 0x633D2B
    // Como el original: solo caduca si tiene duración (m_Duration != -1)
    if (m_Duration != -1 && now > m_EndTime) {
        g_ikChainMan.RemoveIKChain(m_IKChain);
        m_IKChain = nullptr;
        return true;
    }

    //
    // Deal with blending
    //

    // 0x633D50
    if (m_TargetEntityExisted && !m_TargetEntity) {
        m_TargetEntityExisted = false;
        m_IKChain->UpdateTarget(false);
        BlendOut();
    }

    // 0x633D75
    if (m_Duration != -1) {
        if (now >= m_EndTime - m_BlendDuration) {
            m_TargetBlend = 0.f;
            m_TargetTime = m_EndTime;
        }
    }

    // 0x633D98
    if (now > m_TargetTime) {
        m_Blend = m_TargetBlend;
    } else {
        // Como el original: fild/fidiv con enteros exactos y tope solo si supera 1 (un NaN se conserva)
        const int32 denom = m_TargetTime - timeStepMs - now;
        float t = IntRatioCpp(timeStepMs, denom); // X87Math.cpp
        if (t > 1.0f) {
            t = 1.0f;
        }
        m_Blend = (m_TargetBlend - m_Blend) * t + m_Blend;
    }

    m_IKChain->SetBlend(m_Blend);
    return false;
}

// 0x633BD0
bool CTaskSimpleIKChain::CreateIKChain(CPed* ped) {
    m_IKChain = g_ikChainMan.AddIKChain("", eIKChainSlot::CUSTOM, ped, m_EffectorBone, m_EffectorOffset, m_PivotBone, m_TargetEntity, RawBoneTag32(m_OffsetBone), m_OffsetPos, m_Speed, 3);
    return !!m_IKChain;
}
