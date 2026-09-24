/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePickUpEntity
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimplePickUpEntity.h"

void CTaskSimplePickUpEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePickUpEntity, 0x870b50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "1", 0x691870, CTaskSimplePickUpEntity*(CTaskSimplePickUpEntity::*)(CEntity*, CVector*, uint8, uint8, CAnimBlock*, CAnimBlendHierarchy*, eAnimationFlags, float));
    RH_ScopedOverloadedInstall(Constructor, "2", 0x6917B0, CTaskSimplePickUpEntity * (CTaskSimplePickUpEntity::*)(CEntity*, CVector*, uint8, uint8, AnimationId, AssocGroupId, float));

    RH_ScopedVMTInstall(Clone, 0x692A90);
    RH_ScopedVMTInstall(GetTaskType, 0x691810);
}

CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(
    CEntity* entityToHold,
    const CVector* posn,
    uint8 boneFrameId,
    uint8 boneFlags,
    CAnimBlock* pAnimBlock,
    CAnimBlendHierarchy* animHierarchy,
    eAnimationFlags animFlags,
    float fMovePedUntilAnimProgress
)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, pAnimBlock, animHierarchy, animFlags)
{
    m_fMovePedUntilAnimProgress = fMovePedUntilAnimProgress;
}

CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(
    CEntity* entityToHold,
    const CVector* posn,
    uint8 boneFrameId,
    uint8 boneFlags,
    AnimationId animId,
    AssocGroupId groupId,
    float fMovePedUntilAnimProgress
)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, false)
{
    m_fMovePedUntilAnimProgress = fMovePedUntilAnimProgress;
}

// 0x692A90
CTask* CTaskSimplePickUpEntity::Clone() const {
    return m_pAnimBlendHierarchy
        ? new CTaskSimplePickUpEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_nBoneFrameId,
            m_bBoneFlags,
            m_pAnimBlock,
            m_pAnimBlendHierarchy,
            static_cast<eAnimationFlags>(m_animFlags),
            m_fMovePedUntilAnimProgress
        )
        : new CTaskSimplePickUpEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_nBoneFrameId,
            m_bBoneFlags,
            m_nAnimId,
            m_nAnimGroupId,
            m_fMovePedUntilAnimProgress
        );
}
