/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleIKPointArm
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleIKPointArm.h"
#include "Ragdoll/IKChain.h"
#include "Ragdoll/IKChainManager.h"

void CTaskSimpleIKPointArm::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKPointArm, 0x86E3A4, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x634150);
    RH_ScopedInstall(Destructor, 0x634240);
    RH_ScopedInstall(UpdatePointArmInfo, 0x634370);
    RH_ScopedVMTInstall(Clone, 0x634250);
    RH_ScopedVMTInstall(GetTaskType, 0x634230);
    RH_ScopedVMTInstall(CreateIKChain, 0x6342F0);
}

// Como el original: el EXE guarda y pasa el hueso de desplazamiento como dword (la palabra alta tambien cuenta)
static eBoneTag32 RawBoneTag32(const eBoneTag16& bone) {
    eBoneTag32 r;
    r.m_Value = *reinterpret_cast<const int32*>(&bone);
    return r;
}

// 0x634150
CTaskSimpleIKPointArm::CTaskSimpleIKPointArm(const char* purpose, eIKArm arm, CEntity* targetEntity, eBoneTag32 offsetBoneTag, CVector offsetPos, float speed, int32 blendTime) :
    // Como el original: el constructor de CTaskSimpleIKChain va en linea y no escribe 0x14, 0x16, 0x48, 0x4C ni 0x50
    CTaskSimpleIKChain{ UninitTag{ *reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(this) + 0x16) } },
    m_Arm{arm}
{
    m_EffectorBone    = arm ? BONE_L_HAND : BONE_R_HAND; // Como el original: cualquier brazo distinto de 0 es el izquierdo
    *reinterpret_cast<int32*>(&m_OffsetBone) = offsetBoneTag.get_underlying(); // Como el original: se escribe como dword
    m_BlendDuration   = blendTime;
    m_EffectorOffset  = CVector{ 0.05f, 0.f, 0.f }; // Como el original: el desplazamiento va en X
    m_OffsetPos       = offsetPos;
    m_IKChain         = nullptr;
    m_Speed           = speed;
    m_Blend           = 0.f;
    m_Duration        = 999'999;
    m_IsBlendingOut   = 0;
    m_TargetEntity    = targetEntity;
    m_TargetEntityExisted = targetEntity != nullptr;
}

// 0x634250
CTask* CTaskSimpleIKPointArm::Clone() const {
    // Como el original: el hueso de desplazamiento se pasa como dword
    const auto clone = new CTaskSimpleIKPointArm("", m_Arm, m_TargetEntity, RawBoneTag32(m_OffsetBone), m_OffsetPos, m_Speed, m_BlendDuration);
    if (m_IKChain) {
        clone->m_Blend       = m_Blend;
        clone->m_EndTime     = m_EndTime;
        clone->m_TargetBlend = m_TargetBlend;
        clone->m_TargetTime  = m_TargetTime;
    }
    return clone;
}

// 0x634370
void CTaskSimpleIKPointArm::UpdatePointArmInfo(const char* purpose, CEntity* pointAtEntity, eBoneTag32 offsetBoneTag, CVector offset, float speed, int32 blendDuration) {
    m_TargetEntity        = pointAtEntity;
    m_TargetEntityExisted = !!pointAtEntity;
    *reinterpret_cast<int32*>(&m_OffsetBone) = offsetBoneTag.get_underlying(); // Como el original: se escribe como dword
    m_OffsetPos           = offset;
    m_Speed               = speed;
    m_BlendDuration       = blendDuration;
    m_EndTime             = CTimer::GetTimeInMS() + 999'999;
    m_TargetBlend         = 1.f;
    m_TargetTime          = CTimer::GetTimeInMS() + blendDuration;
    m_IsBlendingOut       = false;

    if (m_IKChain) {
        m_IKChain->UpdateEntity(m_TargetEntity);
        m_IKChain->UpdateOffset(RawBoneTag32(m_OffsetBone), m_OffsetPos);
        m_IKChain->UpdateTarget(true);
    }
}

// 0x6342F0
bool CTaskSimpleIKPointArm::CreateIKChain(CPed* ped) {
    m_PivotBone = BONE_NECK;
    // Como el original: hueco 1 + (brazo != 0) y el hueso de desplazamiento como dword
    m_IKChain   = g_ikChainMan.AddIKChain("", (eIKChainSlot)(1 + (m_Arm != 0)), ped, m_EffectorBone, m_EffectorOffset, BONE_NECK, m_TargetEntity,
                                          RawBoneTag32(m_OffsetBone), m_OffsetPos, m_Speed, 3);
    return !!m_IKChain;
}
