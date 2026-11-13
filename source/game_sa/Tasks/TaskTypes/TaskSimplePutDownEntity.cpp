/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePutDownEntity
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimplePutDownEntity.h"

void CTaskSimplePutDownEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePutDownEntity, 0x870b74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x691990);

    RH_ScopedVMTInstall(Clone, 0x692B70);
    RH_ScopedVMTInstall(GetTaskType, 0x691900);

    RH_ScopedInstall(ScalarDeletingDestructor, 0x6935C0); // lote 176A (sin validar)
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity() : CTaskSimpleHoldEntity(nullptr, nullptr, PED_NODE_RIGHT_HAND, HOLD_ENTITY_FLAG_1, ANIM_ID_NO_ANIMATION_SET, ANIM_GROUP_DEFAULT, false) {
    m_fPutDownHeightZ = 0.6f;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd, float fPutDownHeightZ)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, bDisAllowDroppingOnAnimEnd)
{
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags, float fPutDownHeightZ)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animName, animBlockName, animFlags)
{
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* pAnimBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float fPutDownHeightZ)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, pAnimBlock, animHierarchy, animFlags)
{
    m_fPutDownHeightZ = fPutDownHeightZ;
}

// 0x692B70
CTask* CTaskSimplePutDownEntity::Clone() const {
    return m_pAnimBlendHierarchy
        ? new CTaskSimplePutDownEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_nBoneFrameId,
            m_bBoneFlags,
            m_pAnimBlock,
            m_pAnimBlendHierarchy,
            static_cast<eAnimationFlags>(m_animFlags),
            m_fPutDownHeightZ
        )
        : new CTaskSimplePutDownEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_nBoneFrameId,
            m_bBoneFlags,
            m_nAnimId,
            m_nAnimGroupId,
            false,
            m_fPutDownHeightZ
        );
}

// 0x6935C0 (lote 176A, sin validar; el listado la atribuia a CTaskSimpleHoldEntity; destructor con borrado de CTaskSimplePutDownEntity del EXE (ranura 0 de su vtable 0x870B74; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6935E0; test byte [esp+8],1;
// je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6935E0; `jmp 0x691630`: es el de CTaskSimpleHoldEntity), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el
// bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimplePutDownEntity* CTaskSimplePutDownEntity::ScalarDeletingDestructor(uint8 flags) {
    HoldEntityDestructorBody(this, nullptr); // 0x6935E0 es `jmp 0x691630`
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
