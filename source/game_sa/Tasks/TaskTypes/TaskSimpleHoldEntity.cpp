/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHoldEntity
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleHoldEntity.h"

#include "TaskSimplePickUpEntity.h"
#include "TaskSimplePutDownEntity.h"

namespace {
// Como el original: constantes exactas del EXE
constexpr float HOLD_RAND_RECIP      = std::bit_cast<float>(0x38000100u); // 0x858C7C (1/32767)
constexpr float HOLD_PUTDOWN_CARRY105 = std::bit_cast<float>(0x3EDDDDDEu); // 0x870A70 (0.43333334)
constexpr float HOLD_PUTDOWN_CARRY05  = std::bit_cast<float>(0x3EBBBBBCu); // 0x870A6C (0.36666667)
constexpr float HOLD_PUTDOWN_CARRY    = std::bit_cast<float>(0x3F19999Au); // 0x870A68 (0.6)

// Como el original: atan2(y, x) con fpatan del x87 (precisión completa, se redondea al guardar)
float __declspec(naked) __cdecl HoldAtan2X87(float y, float x) {
    __asm {
        fld   dword ptr [esp+4]
        fld   dword ptr [esp+8]
        fpatan
        ret
    }
}

// Como el original: atan2(-fx, fy) - rot sin redondear el ángulo intermedio (fpatan y fsub en la pila)
float __declspec(naked) __cdecl HoldHeadingMinusX87(float fx, float fy, float rot) {
    __asm {
        fld   dword ptr [esp+8]
        fld   dword ptr [esp+4]
        fchs
        fxch  st(1)
        fpatan
        fsub  dword ptr [esp+0Ch]
        ret
    }
}

// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
CVector HoldMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// Como el original: Multiply3x3 (0x59C790) con el orden de sumas del EXE
CVector HoldMultiply3x3_59C790(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y
    };
}

// Como el original: transcripción de CMatrix::RotateZ (0x59B390): fcos queda en la pila sin redondear y fsin se
// guarda como float; mismo orden de productos y de escrituras (thiscall: ecx = matriz, ángulo en la pila, ret 4)
void __declspec(naked) __fastcall HoldRotateZ_59B390(CMatrix* m, int, float angle) {
    __asm {
        sub   esp, 20h
        fld   dword ptr [esp+24h]
        fcos
        fld   dword ptr [esp+24h]
        fsin
        fstp  dword ptr [esp+24h]
        fld   st(0)
        fmul  dword ptr [ecx]
        fld   dword ptr [esp+24h]
        fmul  dword ptr [ecx+4]
        fsubp st(1), st
        fld   st(1)
        fmul  dword ptr [ecx+4]
        fld   dword ptr [esp+24h]
        fmul  dword ptr [ecx]
        faddp st(1), st
        fld   st(2)
        fmul  dword ptr [ecx+10h]
        fld   dword ptr [esp+24h]
        fmul  dword ptr [ecx+14h]
        fsubp st(1), st
        fstp  dword ptr [esp+8]
        fld   dword ptr [esp+24h]
        mov   eax, dword ptr [esp+8]
        fmul  dword ptr [ecx+10h]
        mov   dword ptr [ecx+10h], eax
        fld   st(3)
        fmul  dword ptr [ecx+14h]
        faddp st(1), st
        fstp  dword ptr [esp+0Ch]
        fld   st(2)
        mov   edx, dword ptr [esp+0Ch]
        fmul  dword ptr [ecx+20h]
        mov   dword ptr [ecx+14h], edx
        fld   dword ptr [esp+24h]
        fmul  dword ptr [ecx+24h]
        fsubp st(1), st
        fstp  dword ptr [esp+10h]
        fld   dword ptr [esp+24h]
        mov   eax, dword ptr [esp+10h]
        fmul  dword ptr [ecx+20h]
        mov   dword ptr [ecx+20h], eax
        fld   st(3)
        fmul  dword ptr [ecx+24h]
        faddp st(1), st
        fstp  dword ptr [esp+14h]
        mov   edx, dword ptr [esp+14h]
        fld   st(2)
        fmul  dword ptr [ecx+30h]
        mov   dword ptr [ecx+24h], edx
        fld   dword ptr [esp+24h]
        fmul  dword ptr [ecx+34h]
        fsubp st(1), st
        fstp  dword ptr [esp+18h]
        fld   dword ptr [esp+24h]
        mov   eax, dword ptr [esp+18h]
        fmul  dword ptr [ecx+30h]
        mov   dword ptr [ecx+30h], eax
        fxch  st(3)
        fmul  dword ptr [ecx+34h]
        faddp st(3), st
        fxch  st(2)
        fstp  dword ptr [esp+1Ch]
        mov   edx, dword ptr [esp+1Ch]
        fstp  dword ptr [ecx]
        mov   dword ptr [ecx+34h], edx
        fstp  dword ptr [ecx+4]
        add   esp, 20h
        ret   4
    }
}
} // namespace

void CTaskSimpleHoldEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHoldEntity, 0x870B2C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedOverloadedInstall(Constructor, "1", 0x6913A0, CTaskSimpleHoldEntity*(CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, AnimationId, AssocGroupId, bool));
    RH_ScopedOverloadedInstall(Constructor, "2", 0x691470, CTaskSimpleHoldEntity * (CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, const char*, const char*, eAnimationFlags));
    RH_ScopedOverloadedInstall(Constructor, "3", 0x691550, CTaskSimpleHoldEntity * (CTaskSimpleHoldEntity::*)(CEntity*, CVector*, uint8, uint8, CAnimBlock*, CAnimBlendHierarchy*, eAnimationFlags));
    RH_ScopedVMTInstall(Clone, 0x6929B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x693BD0);
    RH_ScopedVMTInstall(ProcessPed, 0x693C40);
    RH_ScopedVMTInstall(SetPedPosition, 0x6940A0);
    RH_ScopedInstall(ReleaseEntity, 0x6916E0);
    RH_ScopedInstall(CanThrowEntity, 0x691700);
    RH_ScopedInstall(PlayAnim, 0x691720);
    RH_ScopedInstall(FinishAnimHoldEntityCB, 0x691740);
    RH_ScopedInstall(StartAnim, 0x692FF0);
    RH_ScopedInstall(DropEntity, 0x6930F0);
    RH_ScopedInstall(ChoosePutDownHeight, 0x693440);
}
CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd) { this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, bDisAllowDroppingOnAnimEnd); return this; }
CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) { this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animName, animBlockName, animFlags); return this; }
CTaskSimpleHoldEntity* CTaskSimpleHoldEntity::Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) { this->CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animBlock, animHierarchy, animFlags); return this; }

// 0x6913A0
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(
    CEntity* entityToHold,
    const CVector* posn,
    uint8 boneFrameId,
    uint8 boneFlags,
    AnimationId animId,
    AssocGroupId groupId,
    bool bDisAllowDroppingOnAnimEnd
)
    : CTaskSimple()
{
    m_pEntityToHold = entityToHold;
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
    m_nBoneFrameId = boneFrameId;
    m_bBoneFlags = boneFlags;
    m_nAnimId = animId;
    m_nAnimGroupId = groupId;
    m_bDisallowDroppingOnAnimEnd = bDisAllowDroppingOnAnimEnd;
    m_pAnimBlock = nullptr;
    m_pAnimBlendHierarchy = nullptr;
    m_bEntityDropped = false;
    m_bEntityRequiresProcessing = true;
    m_pAnimBlendAssociation = nullptr;
    if (posn)
        m_vecPosition = *posn;
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        m_pEntityToHold->RegisterReference(&m_pEntityToHold);
    }
}

CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold,
    const CVector& posn,
    uint8 boneFrameId,
    uint8 boneFlags,
    AnimationId animId,
    AssocGroupId groupId,
    bool bDisAllowDroppingOnAnimEnd
) :
    m_pEntityToHold{ entityToHold },
    m_vecPosition{ posn },
    m_nBoneFrameId{ boneFrameId },
    m_bBoneFlags{ boneFlags },
    m_nAnimId{ animId },
    m_nAnimGroupId{ groupId },
    m_bDisallowDroppingOnAnimEnd{ bDisAllowDroppingOnAnimEnd },
    m_pAnimBlock{},
    m_pAnimBlendHierarchy{},
    m_bEntityDropped{},
    m_bEntityRequiresProcessing{ true },
    m_pAnimBlendAssociation{}
{
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        CEntity::RegisterReference(m_pEntityToHold);
    }
}

// 0x691470
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags) : CTaskSimple()
{
    m_pEntityToHold = entityToHold;
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
    m_nBoneFrameId = boneFrameId;
    m_bBoneFlags = boneFlags;
    m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
    m_nAnimGroupId = ANIM_GROUP_DEFAULT;
    m_bEntityDropped = false;
    m_bEntityRequiresProcessing = true;
    m_bDisallowDroppingOnAnimEnd = false;
    m_pAnimBlendAssociation = nullptr;
    if (posn)
        m_vecPosition = *posn;
    if (m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        m_pEntityToHold->RegisterReference(&m_pEntityToHold);
    }
    m_animFlags = animFlags;
    m_pAnimBlock = CAnimManager::GetAnimationBlock(animBlockName);
    m_pAnimBlendHierarchy = CAnimManager::GetAnimation(animName, m_pAnimBlock);
    CAnimManager::AddAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_pAnimBlock));
}

// 0x691550
CTaskSimpleHoldEntity::CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags) : CTaskSimple()
{
    m_pEntityToHold = entityToHold;
    m_vecPosition = CVector(0.0f, 0.0f, 0.0f);
    m_nBoneFrameId = boneFrameId;
    m_bBoneFlags = boneFlags;
    m_fRotation = 0.0f;
    m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
    m_nAnimGroupId = ANIM_GROUP_DEFAULT;
    m_bEntityDropped = false;
    m_bEntityRequiresProcessing = true;
    m_bDisallowDroppingOnAnimEnd = false;
    m_pAnimBlendAssociation = nullptr;
    if (posn)
        m_vecPosition = *posn;
    if (m_pEntityToHold)
    {
        m_pEntityToHold->m_bStreamingDontDelete = true;
        m_pEntityToHold->RegisterReference(&m_pEntityToHold);
    }
    m_animFlags = animFlags;
    m_pAnimBlock = animBlock;
    m_pAnimBlendHierarchy = animHierarchy;
    CAnimManager::AddAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_pAnimBlock));
}

CTaskSimpleHoldEntity::~CTaskSimpleHoldEntity() {
    if (!m_bEntityRequiresProcessing && m_pEntityToHold) {
        m_pEntityToHold->m_bStreamingDontDelete = false;
        m_pEntityToHold->CleanUpOldReference(&m_pEntityToHold);
        m_pEntityToHold->m_bRemoveFromWorld = true;
        m_pEntityToHold = nullptr;
    }
    if (m_pAnimBlendAssociation) {
        m_pAnimBlendAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    if (m_pAnimBlock) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_pAnimBlock));
    }
}

// 0x6929B0
CTask* CTaskSimpleHoldEntity::Clone() const {
    if (m_pAnimBlendHierarchy)
        return new CTaskSimpleHoldEntity(m_pEntityToHold, &m_vecPosition, m_nBoneFrameId, m_bBoneFlags, m_pAnimBlock, m_pAnimBlendHierarchy, static_cast<eAnimationFlags>(m_animFlags));
    else
        return new CTaskSimpleHoldEntity(m_pEntityToHold, &m_vecPosition, m_nBoneFrameId, m_bBoneFlags, m_nAnimId, m_nAnimGroupId, false);
}

// 0x693BD0
bool CTaskSimpleHoldEntity::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_pAnimBlendAssociation) {
            m_pAnimBlendAssociation->m_BlendDelta = -4.0f;
            m_pAnimBlendAssociation->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnimBlendAssociation = nullptr;
        }
        DropEntity(ped, true);
        m_bEntityDropped = true;
        return true;
    }
    else {
        if (m_pAnimBlendAssociation)
            m_pAnimBlendAssociation->m_BlendDelta = -4.0f;
        return false;
    }
}

// 0x693C40
bool CTaskSimpleHoldEntity::ProcessPed(CPed* ped) {
    CTaskManager* taskManager = &ped->GetTaskManager();
    if (m_bEntityRequiresProcessing) {
        m_bEntityRequiresProcessing = false;
        if (GetTaskType() == TASK_SIMPLE_PUTDOWN_ENTITY && !m_pEntityToHold && !m_pAnimBlendHierarchy && !m_nAnimGroupId) {
            auto holdEntityTask = (CTaskSimpleHoldEntity*)taskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
            if (holdEntityTask && holdEntityTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY &&
                holdEntityTask->m_pEntityToHold && holdEntityTask->m_nAnimGroupId)
            {
                CEntity* entityToHold = holdEntityTask->m_pEntityToHold;
                m_pEntityToHold = entityToHold;
                entityToHold->RegisterReference(&m_pEntityToHold);

                m_vecPosition = holdEntityTask->m_vecPosition;
                m_nAnimId = ANIM_ID_PUTDWN;
                m_nAnimGroupId = holdEntityTask->m_nAnimGroupId;
                ChoosePutDownHeight(ped);
                holdEntityTask->ReleaseEntity();
            }
            else {
                m_bEntityDropped = true;
            }
        }
        CEntity* entityToHold = m_pEntityToHold;
        if (!entityToHold || m_bEntityDropped) {
            m_bEntityDropped = true;
        }
        else {
            entityToHold->SetUsesCollision(false);
            if (entityToHold->GetIsTypeObject()) {
                auto* objectToHold = entityToHold->AsObject();
                if (objectToHold->GetIsStatic()) {
                    objectToHold->SetIsStatic(false);
                    objectToHold->AddToMovingList();
                }
                objectToHold->physicalFlags.bAttachedToEntity = true;
                objectToHold->m_nFakePhysics = 0;
            }

            // Como el original: GetHeading en línea con fpatan; la resta se hace sobre el ángulo sin redondear
            if (const CMatrix* m = entityToHold->m_matrix) {
                m_fRotation = HoldHeadingMinusX87(m->GetForward().x, m->GetForward().y, ped->m_fCurrentRotation);
            } else {
                m_fRotation = entityToHold->m_placement.m_fHeading - ped->m_fCurrentRotation;
            }
        }
    }

    if (m_bEntityDropped)
        return true;

    if (m_nAnimId != ANIM_ID_NO_ANIMATION_SET || m_pAnimBlendHierarchy) {
        if (m_pAnimBlendAssociation) {
            if (m_pAnimBlendAssociation->m_BlendDelta < 0.0f && !m_bDisallowDroppingOnAnimEnd)
                DropEntity(ped, true);
        }
        else {
            StartAnim(ped);
        }
    }

    if (m_pEntityToHold) {
        CEntity* entityToHold = m_pEntityToHold;
        eTaskType taskId = GetTaskType();
        if (taskId != TASK_SIMPLE_HOLD_ENTITY) {
            if (taskId == TASK_SIMPLE_PUTDOWN_ENTITY) { // Como el original: solo PUTDOWN (antes cualquier otro tipo)
                auto* taskHoldEntity = static_cast<CTaskSimpleHoldEntity*>(taskManager->GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM));
                if (taskHoldEntity && taskHoldEntity->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY)
                    taskHoldEntity->MakeAbortable(ped);
            }
            else if (taskId == TASK_SIMPLE_PICKUP_ENTITY)
            {
                auto* taskPickUpEntity = static_cast<CTaskSimplePickUpEntity*>(this);
                CAnimBlendAssociation* animAssoc = taskPickUpEntity->m_pAnimBlendAssociation;
                if ((!animAssoc || taskPickUpEntity->m_fMovePedUntilAnimProgress > animAssoc->m_CurrentTime)
                    && (taskPickUpEntity->m_vecPickuposn.x != 0.0f || taskPickUpEntity->m_vecPickuposn.y != 0.0f))
                {
                    // Como el original: operator*(CMatrix, CVector) (0x59C890) y productos escalares (z + y) + x
                    CVector outPoint = HoldMatrixTimesPoint59C890(entityToHold->GetMatrix(), taskPickUpEntity->m_vecPickuposn);
                    const CVector& pedPos = ped->GetPosition();
                    outPoint.x -= pedPos.x;
                    outPoint.y -= pedPos.y;
                    outPoint.z -= pedPos.z;
                    const CMatrix& pedMat = *ped->m_matrix;
                    const float dotRight = outPoint.z * pedMat.GetRight().z + outPoint.y * pedMat.GetRight().y + outPoint.x * pedMat.GetRight().x;
                    ped->m_vecAnimMovingShiftLocal.x = dotRight / CTimer::GetTimeStep() * 0.1f + ped->m_vecAnimMovingShiftLocal.x;
                    const float dotFwd = outPoint.z * pedMat.GetForward().z + outPoint.y * pedMat.GetForward().y + outPoint.x * pedMat.GetForward().x;
                    ped->m_vecAnimMovingShiftLocal.y = dotFwd / CTimer::GetTimeStep() * 0.1f + ped->m_vecAnimMovingShiftLocal.y;
                    const CVector& entPos = entityToHold->GetPosition();
                    const CVector& pedPos2 = ped->GetPosition();
                    const CVector direction{ entPos.x - pedPos2.x, entPos.y - pedPos2.y, entPos.z - pedPos2.z };
                    ped->m_fAimingRotation = HoldAtan2X87(-direction.x, direction.y); // Como el original: fpatan
                }
            }
        }
        else {
            if ((m_nAnimGroupId == ANIM_GROUP_CARRY || m_nAnimGroupId == ANIM_GROUP_CARRY05 || m_nAnimGroupId == ANIM_GROUP_CARRY105) && ped->IsPlayer()) {
                CPad* pad = ped->AsPlayer()->GetPadFromPlayer();
                if (pad->ExitVehicleJustDown()) {
                    auto pTaskSimplePutDownEntity = new CTaskSimplePutDownEntity();
                    CEventScriptCommand eventScriptCommand(TASK_PRIMARY_PRIMARY, pTaskSimplePutDownEntity, false);
                    ped->GetEventGroup().Add(&eventScriptCommand, false);
                }
            }
        }
    }
    return false;
}

// 0x6940A0
bool CTaskSimpleHoldEntity::SetPedPosition(CPed* ped) {
    bool bUpdateEntityToHoldPosition = false;
    if (GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY)
    {
        auto pickupEntityTask = static_cast<CTaskSimplePickUpEntity*>(this);
        if (!m_pAnimBlendAssociation || pickupEntityTask->m_fMovePedUntilAnimProgress > m_pAnimBlendAssociation->m_CurrentTime)
            return false;
        bUpdateEntityToHoldPosition = true;
    }
    else {
        auto pPutDownEntityTask = static_cast<CTaskSimplePutDownEntity*>(this);
        // Como el original: con NaN también se actualiza (test ah,5 / jp)
        if (GetTaskType() != TASK_SIMPLE_PUTDOWN_ENTITY || m_bEntityRequiresProcessing
            || m_pAnimBlendAssociation
            && !(pPutDownEntityTask->m_fPutDownHeightZ < m_pAnimBlendAssociation->m_CurrentTime)
        ) {
            bUpdateEntityToHoldPosition = true;
        }
    }

    if (bUpdateEntityToHoldPosition) {
        if (m_pEntityToHold) {
            m_pEntityToHold->SetIsVisible(ped->GetIsVisible());
            if (ped->bCalledPreRender) {
                if (m_bBoneFlags & HOLD_ENTITY_UPDATE_TRANSLATION_ONLY) {
                    CVector entityToHoldPos = m_vecPosition;
                    RpHAnimHierarchy* pHAnimHierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
                    int32 animIndex = RpHAnimIDGetIndex(pHAnimHierarchy, ped->m_apBones.data()[m_nBoneFrameId]->BoneTag);
                    RwMatrix* pBoneMatrix = &RpHAnimHierarchyGetMatrixArray(pHAnimHierarchy)[animIndex];
                    RwV3dTransformPoints((RwV3d*)&entityToHoldPos, (RwV3d*)&entityToHoldPos, 1, pBoneMatrix);
                    m_pEntityToHold->GetMatrix().UpdateMatrix(pBoneMatrix);
                    m_pEntityToHold->SetPosn(entityToHoldPos);
                }
                else {
                    // Como el original: Multiply3x3 (0x59C790) con la matriz del peatón leída sin reservarla
                    const CMatrix& pedMat = *ped->m_matrix;
                    CVector entityToHoldPos = HoldMultiply3x3_59C790(pedMat, m_vecPosition);
                    RpHAnimHierarchy* pHAnimHierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
                    int32 animIndex = RpHAnimIDGetIndex(pHAnimHierarchy, ped->m_apBones.data()[m_nBoneFrameId]->BoneTag);
                    RwMatrix* pBoneMatrix = RpHAnimHierarchyGetMatrixArray(pHAnimHierarchy);
                    const RwV3d& bonePos = *RwMatrixGetPos(&pBoneMatrix[animIndex]);
                    entityToHoldPos.x = bonePos.x + entityToHoldPos.x;
                    entityToHoldPos.y = bonePos.y + entityToHoldPos.y;
                    entityToHoldPos.z = bonePos.z + entityToHoldPos.z;
                    CMatrix rotationMatrix(pedMat);
                    HoldRotateZ_59B390(&rotationMatrix, 0, m_fRotation); // Como el original: RotateZ del EXE (fcos sin redondear)
                    *static_cast<CMatrix*>(&m_pEntityToHold->GetMatrix()) = rotationMatrix;
                    m_pEntityToHold->SetPosn(entityToHoldPos);
                }
            }
            else {
                if (ped->m_matrix)
                    m_pEntityToHold->SetPosn(ped->GetPosition());
                else
                    m_pEntityToHold->SetPosn(ped->m_placement.m_vPosn);
            }
            m_pEntityToHold->UpdateRwMatrix();
            m_pEntityToHold->UpdateRwFrame();
            return true;
        }
        else {
            MakeAbortable(ped);
            return false;
        }
        return false;
    }
    if (m_pEntityToHold)
        DropEntity(ped, false);
    return false;
}

// 0x6916E0
void CTaskSimpleHoldEntity::ReleaseEntity() {
    CEntity::ClearReference(m_pEntityToHold);
}

// 0x691700
bool CTaskSimpleHoldEntity::CanThrowEntity() const {
    return    m_pEntityToHold
           && m_bBoneFlags == HOLD_ENTITY_UPDATE_TRANSLATION_ONLY
           && m_nBoneFrameId == PED_NODE_RIGHT_HAND;
}

// 0x691720
void CTaskSimpleHoldEntity::PlayAnim(AnimationId animId, AssocGroupId grpId) {
    m_nAnimId = animId;
    m_nAnimGroupId = grpId;
}

// 0x691740
void CTaskSimpleHoldEntity::FinishAnimHoldEntityCB(CAnimBlendAssociation* animAssoc, void* data) {
    auto taskHoldEntity = reinterpret_cast<CTaskSimpleHoldEntity*>(data);
    if (taskHoldEntity->m_bDisallowDroppingOnAnimEnd) {
        taskHoldEntity->m_nAnimId = ANIM_ID_NO_ANIMATION_SET;
        taskHoldEntity->m_nAnimGroupId = ANIM_GROUP_DEFAULT;
        taskHoldEntity->m_pAnimBlendAssociation = nullptr;
    }
    else
    {
        if (taskHoldEntity->GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY && animAssoc->m_BlendAmount > 0.0f) {
            CEntity::ClearReference(taskHoldEntity->m_pEntityToHold);
        }
        taskHoldEntity->m_bEntityDropped = true;
        taskHoldEntity->m_pAnimBlendAssociation = nullptr;
    }
}

// 0x692FF0
void CTaskSimpleHoldEntity::StartAnim(CPed* ped) {
    if (m_pAnimBlendHierarchy) {
        m_animFlags |= ANIMATION_DONT_ADD_TO_PARTIAL_BLEND | ANIMATION_IS_BLEND_AUTO_REMOVE | ANIMATION_IS_PARTIAL;
        m_pAnimBlendAssociation = CAnimManager::BlendAnimation(ped->GetRpClump(), m_pAnimBlendHierarchy, m_animFlags, 4.0f);
    } else {
        if (m_nAnimGroupId && !m_pAnimBlock) {
            CAnimBlock* animBlock = CAnimManager::GetAnimationBlock(m_nAnimGroupId);
            if (!animBlock) {
                animBlock = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(m_nAnimGroupId));
            }
            const auto blockIndex = CAnimManager::GetAnimationBlockIndex(animBlock);
            if (!animBlock->IsLoaded) {
                CStreaming::RequestModel(IFPToModelId(blockIndex), STREAMING_KEEP_IN_MEMORY);
                return;
            }
            CAnimManager::AddAnimBlockRef(blockIndex);
            m_pAnimBlock = animBlock;
        }
        m_pAnimBlendAssociation = CAnimManager::BlendAnimation(ped->GetRpClump(), m_nAnimGroupId, m_nAnimId, 4.0f);
        m_pAnimBlendAssociation->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        if (GetTaskType() == TASK_SIMPLE_HOLD_ENTITY) {
            m_pAnimBlendAssociation->m_Flags |= ANIMATION_DONT_ADD_TO_PARTIAL_BLEND;
        }
    }

    if (GetTaskType() == TASK_SIMPLE_PICKUP_ENTITY)
        m_pAnimBlendAssociation->SetFinishCallback(CTaskSimpleHoldEntity::FinishAnimHoldEntityCB, this);
    else
        m_pAnimBlendAssociation->SetDeleteCallback(CTaskSimpleHoldEntity::FinishAnimHoldEntityCB, this);
}

// 0x6930F0
void CTaskSimpleHoldEntity::DropEntity(CPed* ped, bool bAddEventSoundQuiet) {
    if (!m_pEntityToHold) {
        return;
    }
    m_pEntityToHold->SetUsesCollision(true);
    if (!m_pEntityToHold->GetIsTypeObject()) {
        CEntity::ClearReference(m_pEntityToHold);
        return;
    }
    // Como el original: solo se coloca en el suelo si se pide (por defecto no)
    bool bPlaceOnGround = false;
    auto* objectToHold = m_pEntityToHold->AsObject();
    objectToHold->m_pEntityIgnoredCollision = ped;
    if (objectToHold->physicalFlags.bDisableCollisionForce && bAddEventSoundQuiet) {
        if (objectToHold->objectFlags.bIsLiftable || objectToHold->IsMissionObject()) {
            bPlaceOnGround = true;
        } else {
            if (objectToHold->m_nObjectType != OBJECT_TEMPORARY) {
                ++CObject::nNoTempObjects;
            }
            objectToHold->SetUsesCollision(false);
            objectToHold->SetIsVisible(false);
            objectToHold->m_nObjectType = OBJECT_TEMPORARY;
            objectToHold->m_nRemovalTime = 0;
        }
    } else {
        objectToHold->physicalFlags.bAttachedToEntity = false;
        if (!bAddEventSoundQuiet) {
            objectToHold->m_vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f); // Como el original: se detiene
        } else {
            if (objectToHold->GetIsStatic()) {
                objectToHold->SetIsStatic(false);
                objectToHold->AddToMovingList();
            }
            // Como el original: parte de la velocidad del peatón; rand() · (1/32767) · rango + mínimo con la matriz leída
            // sin reservarla (delante, derecha y arriba en ese orden)
            CVector& speed = objectToHold->m_vecMoveSpeed;
            speed = ped->m_vecMoveSpeed;
            const CMatrix* pedMat = ped->m_matrix;
            const float fwdMult = (float)CGeneral::GetRandomNumber() * HOLD_RAND_RECIP * 0.03f + 0.03f;
            speed.x = fwdMult * pedMat->GetForward().x + speed.x;
            speed.y = fwdMult * pedMat->GetForward().y + speed.y;
            speed.z = fwdMult * pedMat->GetForward().z + speed.z;
            const float rightMult = (float)CGeneral::GetRandomNumber() * HOLD_RAND_RECIP * 0.06f - 0.03f;
            speed.x = rightMult * pedMat->GetRight().x + speed.x;
            speed.y = rightMult * pedMat->GetRight().y + speed.y;
            speed.z = rightMult * pedMat->GetRight().z + speed.z;
            speed.x = pedMat->GetUp().x * 0.01f + speed.x;
            speed.y = pedMat->GetUp().y * 0.01f + speed.y;
            speed.z = pedMat->GetUp().z * 0.01f + speed.z;
            if (objectToHold->TestCollision(false)) {
                bPlaceOnGround = true;
            }
        }
    }

    if (objectToHold->objectFlags.bIsLiftable && ped->GetPlayerData() && bAddEventSoundQuiet) {
        CEventSoundQuiet eventSoundQuiet(ped, 60.0f, -1, CVector(0.0f, 0.0f, 0.0f));
        GetEventGlobalGroup()->Add(&eventSoundQuiet, false);
    }

    if (bPlaceOnGround) {
        objectToHold->ResetMoveSpeed();
        objectToHold->SetIsStatic(true);
        objectToHold->RemoveFromMovingList();
        CVector objectToHoldPosition = ped->GetPosition();
        objectToHoldPosition.z = objectToHoldPosition.z - 1.0f - objectToHold->GetColModel()->m_boundBox.m_vecMin.z;
        objectToHold->SetPosn(objectToHoldPosition);
        objectToHold->UpdateRwMatrix();
        objectToHold->UpdateRwFrame();
    }
    CEntity::ClearReference(m_pEntityToHold);
}

// 0x693440
void CTaskSimpleHoldEntity::ChoosePutDownHeight(CPed* ped) {
    CVector origin = (ped->GetForward() * 0.65f) + ped->GetPosition();
    origin.z += 0.2f;
    float distance = origin.z - 1.5f;
    CEntity* outEntity = nullptr;
    CColPoint colPoint{};
    if (!CWorld::ProcessVerticalLine(origin, distance, colPoint, outEntity, true, false, false, true, false, false, nullptr)) {
        m_nAnimGroupId = ANIM_GROUP_CARRY;
    }
    else {
        if (ped->GetPosition().z - 0.2f <= colPoint.m_vecPoint.z) {
            m_nAnimGroupId = ANIM_GROUP_CARRY105;
        }
        else {
            if (ped->GetPosition().z - 0.7f <= colPoint.m_vecPoint.z)
                m_nAnimGroupId = ANIM_GROUP_CARRY05;
            else
                m_nAnimGroupId = ANIM_GROUP_CARRY;
        }
    }
    if (GetTaskType() == TASK_SIMPLE_PUTDOWN_ENTITY) {
        auto taskPutDownEntity = static_cast<CTaskSimplePutDownEntity*>(this);
        // Como el original: valores exactos del EXE (0x870A70, 0x870A6C y 0x870A68)
        if (m_nAnimGroupId == ANIM_GROUP_CARRY105)
            taskPutDownEntity->m_fPutDownHeightZ = HOLD_PUTDOWN_CARRY105;
        else if (m_nAnimGroupId == ANIM_GROUP_CARRY05)
            taskPutDownEntity->m_fPutDownHeightZ = HOLD_PUTDOWN_CARRY05;
        else
            taskPutDownEntity->m_fPutDownHeightZ = HOLD_PUTDOWN_CARRY;
    }
}
