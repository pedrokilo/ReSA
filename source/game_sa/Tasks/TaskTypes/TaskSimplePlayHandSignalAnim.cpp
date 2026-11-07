/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePlayHandSignalAnim
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimplePlayHandSignalAnim.h"

#include "Ragdoll/IKChainManager.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x61AE50 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimplePlayHandSignalAnim del EXE (thiscall, cuatro dwords en la pila, `ret 10h`, devuelve this). Base CTaskSimple (0x61A390); id de animacion en +0x10, factor (patron de bits tal cual) en +0x14, byte +0x18 = (byte bajo del tercer argumento != 0), bit 2 del byte +0xC = bit 0 del cuarto, +8/+0x1C/+0x20 = 0 y vtable 0x86D5B8.
CTaskSimplePlayHandSignalAnim* __fastcall HandSignalConstructorBody(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint32 animId, uint32 blendBits, uint32 fatHands, uint32 holdLastFrame) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    bytes[0xC] = static_cast<uint8>(((holdLastFrame & 1) << 2) | (bytes[0xC] & 0xF8));
    *reinterpret_cast<uint32*>(bytes + 0x10) = animId;
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x20) = 0;
    *reinterpret_cast<uint32*>(bytes) = 0x86D5B8;
    *reinterpret_cast<uint32*>(bytes + 0x14) = blendBits;
    bytes[0x18] = static_cast<uint8>(fatHands) != 0 ? 1 : 0;
    return self;
}
} // namespace

void CTaskSimplePlayHandSignalAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayHandSignalAnim, 0x86D5B8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(HandSignalConstructorBody, "Constructor", 0x61AE50);

    RH_ScopedVMTInstall(ProcessPed, 0x61BDA0);
    RH_ScopedInstall(StartAnim, 0x61AF60);
}

// 0x61AE50
CTaskSimplePlayHandSignalAnim::CTaskSimplePlayHandSignalAnim(AnimationId animationId, float fBlendFactor, bool bFatHands, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame) {
    m_PlayerIdlesAnimBlockId = animationId;
    m_LeftHand = nullptr;
    m_RightHand = nullptr;
    m_AnimBlenDelta = fBlendFactor;
    m_DoUseFatHands = bFatHands;
}

CTaskSimplePlayHandSignalAnim::~CTaskSimplePlayHandSignalAnim() {
    if (m_LeftHand) {
        CWorld::Remove(m_LeftHand);
        delete m_LeftHand;
        --CObject::nNoTempObjects;
    }

    if (m_RightHand) {
        CWorld::Remove(m_RightHand);
        delete m_RightHand;
        --CObject::nNoTempObjects;
    }
}

// 0x61BDA0
bool CTaskSimplePlayHandSignalAnim::ProcessPed(CPed* ped) {
    if (g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_LEFT, ped) || m_bIsFinished) {
        return true;
    }

    if (!m_pAnim) {
        if (CObject::nNoTempObjects + 1 >= 150) {
            return true;
        }

        CTaskSimplePlayHandSignalAnim::StartAnim(ped);
    }

    return m_bIsFinished;
}

// 0x61AF60
void CTaskSimplePlayHandSignalAnim::StartAnim(CPed* ped) {
    // Como el original: GSIGN1 - trunc(rand * (1/32768) * -5), es decir [GSIGN1, GSIGN5]
    const auto animId = static_cast<AnimationId>(CGeneral::GetRandomIntInRange((int32)ANIM_ID_GSIGN1, (int32)ANIM_ID_GSIGN5 + 1));

    //
    // Pointing / weapon logic
    //
    if (ped->GetEntityThatThisPedIsHolding() || g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_RIGHT, ped) || ped->GetActiveWeapon().m_Type != eWeaponType::WEAPON_UNARMED) {
        m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_HANDSIGNALL, animId, m_AnimBlenDelta);
        m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);

        if (m_PlayerIdlesAnimBlockId == -1) {
            return;
        }

        const auto handIndex = m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL;
        m_LeftHand    = new CHandObject(handIndex, ped, true);
        if (!m_LeftHand) {
            return;
        }

        CWorld::Add(m_LeftHand);
        auto* animHierarchy = CAnimManager::GetAnimAssociation(ANIM_GROUP_LHAND, m_PlayerIdlesAnimBlockId)->m_BlendHier;
        CAnimManager::AddAnimation(m_LeftHand->GetRpClump(), animHierarchy, 0);
        ++CObject::nNoTempObjects;
        return;
    }

    //
    // Both arms logic
    //
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_HANDSIGNAL, animId, m_AnimBlenDelta);
    m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    if (m_PlayerIdlesAnimBlockId == -1) {
        return;
    }

    const auto CreateHandObject = [ped, this](eModelID handModel, AssocGroupId animGrpId, bool isLeftHand) {
        const auto handObj = new CHandObject(handModel, ped, isLeftHand);
        if (handObj) {
            CWorld::Add(handObj);
            CAnimManager::AddAnimation(
                handObj->GetRpClump(),
                CAnimManager::GetAnimAssociation(animGrpId, m_PlayerIdlesAnimBlockId)->GetAnimHierarchy(), // Como el original: LHAND (0x4F) o RHAND (0x50)
                0
            );
            CObject::nNoTempObjects++;
        }
        return handObj;
    };
    m_LeftHand = CreateHandObject(m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL, ANIM_GROUP_LHAND, true);
    m_RightHand = CreateHandObject(m_DoUseFatHands ? MODEL_FHANDR : MODEL_SHANDR, ANIM_GROUP_RHAND, false);
}
