/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPlayHandSignalAnim
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
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePlayHandSignalAnim.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexPlayHandSignalAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPlayHandSignalAnim, 0x86d5dc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61B2B0);
    RH_ScopedInstall(Destructor, 0x61BDF0);

    RH_ScopedInstall(GetAnimIdForPed, 0x61B460);
    RH_ScopedInstall(CreateSubTask, 0x61B2F0);

    RH_ScopedVMTInstall(Clone, 0x61BA00);
    RH_ScopedVMTInstall(GetTaskType, 0x61B2E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x61B570);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x61B4F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x61B580);
}

// 0x61B2B0
CTaskComplexPlayHandSignalAnim::CTaskComplexPlayHandSignalAnim(AnimationId animationId, float blendFactor) :
    m_animationId{animationId},
    m_AnimBlenDelta{blendFactor}
{
    // Como el original (0x61B2C3: and byte ptr [esi+14h],0F8h): solo se ponen a 0 los bits 0-2; el resto del byte queda sin escribir.
    *(reinterpret_cast<uint8*>(this) + 0x14) &= 0xF8;
}

// 0x61BDF0
CTaskComplexPlayHandSignalAnim::~CTaskComplexPlayHandSignalAnim() {
    // Como el original: indice 0 = izquierda (A9B6F0/F8), 1 = derecha (A9B6F4/FC).
    enum {
        LEFT,
        RIGHT,
    };
    enum {
        NONFAT,
        FAT
    };
    const eModelID handModels[2][2]{
        // nonfat           fat
        { MODEL_SHANDL, MODEL_FHANDL }, // left
        { MODEL_SHANDR, MODEL_FHANDR }  // right
    };

    // Como el original (0x61BE13-0x61BE4E): solo quita la referencia de cada mano si el bit 0 (izquierda) / bit 1 (derecha) esta activo.
    if (m_bLeftHandLoaded) {
        CModelInfo::GetModelInfo(handModels[LEFT][m_DoUseFatHands ? FAT : NONFAT])->RemoveRef();
    }
    if (m_bRightHandLoaded) {
        CModelInfo::GetModelInfo(handModels[RIGHT][m_DoUseFatHands ? FAT : NONFAT])->RemoveRef();
    }

    // Deal with anim
    if (m_bAnimationLoaded) { // Remove anim ref
        CAnimManager::RemoveAnimBlockRef(ms_animBlock);
    } else if (ms_animBlock != -1 && !CAnimManager::GetAnimBlocks()[ms_animBlock].RefCnt) { 
        if (!rng::all_of(std::array{ LEFT, RIGHT }, [&, this](auto i) { // Unload anim block if not all of the models has refs
            return CModelInfo::GetModelInfo(handModels[i][m_DoUseFatHands ? FAT : NONFAT])->m_nRefCount != 0;
        })) {
            CStreaming::RemoveModel(IFPToModelId(ms_animBlock));        
        }
    }
}

// 0x61B460
#ifdef _DEBUG
#pragma optimize("", on)
#endif
AnimationId __stdcall CTaskComplexPlayHandSignalAnim::GetAnimIdForPed(CPed* ped) {
    const auto type = ped->m_nPedType;
    if (type == 7) return static_cast<AnimationId>(320);
    if (type == 8) return static_cast<AnimationId>(321);
    if (type == 9) return static_cast<AnimationId>(322);
    if (type == 11) return static_cast<AnimationId>(324);
    return static_cast<AnimationId>(type == 14 ? 323 : -1);
}
#ifdef _DEBUG
#pragma optimize("", off)
#endif

// 0x61B2F0
CTask* CTaskComplexPlayHandSignalAnim::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case static_cast<eTaskType>(0xCB):
        return new CTaskSimpleStandStill(0, true, false, 8.0f);
    case static_cast<eTaskType>(0x1A9): {
        auto* sequence = new CTaskComplexSequence;
        // Como el original: reservar antes de pedir el aleatorio del tiempo.
        auto* idleStorage = CTask::operator new(sizeof(CTaskSimpleStandStill));
        auto* idle = idleStorage ? ::new (idleStorage) CTaskSimpleStandStill(CGeneral::GetRandomIntInRange(0, 1500), false, false, 8.0f) : nullptr;
        sequence->AddTask(idle);
        sequence->AddTask(new CTaskSimplePlayHandSignalAnim(m_animationId, m_AnimBlenDelta, m_DoUseFatHands, false));
        return sequence;
    }
    default:
        return nullptr;
    }
}

// 0x61BA00
CTask* CTaskComplexPlayHandSignalAnim::Clone() const {
    return new CTaskComplexPlayHandSignalAnim(m_animationId, m_AnimBlenDelta);
}

// 0x61B570
CTask* CTaskComplexPlayHandSignalAnim::CreateNextSubTask(CPed* ped) {
    return nullptr;
}

// 0x61B4F0
CTask* CTaskComplexPlayHandSignalAnim::CreateFirstSubTask(CPed* ped) {
    if (g_ikChainMan.IsArmPointing(static_cast<eIKArm>(1), ped)) {
        return nullptr;
    }
    if (m_animationId == ANIM_ID_UNDEFINED) {
        m_animationId = GetAnimIdForPed(ped);
        if (m_animationId == ANIM_ID_UNDEFINED) {
            return nullptr;
        }
    }
    const auto model = ped->GetModelIndex();
    m_DoUseFatHands = model == 103 || model == 105 || model == 107;
    return CreateSubTask(static_cast<eTaskType>(0xCB));
}

// 0x61B580
CTask* CTaskComplexPlayHandSignalAnim::ControlSubTask(CPed* ped) {
    auto* result = m_pSubTask;
    const auto left = m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL;
    const auto right = m_DoUseFatHands ? MODEL_FHANDR : MODEL_SHANDR;
    if (CStreaming::IsModelLoaded(left)) {
        if (!m_bLeftHandLoaded) {
            CModelInfo::GetModelInfo(left)->AddRef();
            m_bLeftHandLoaded = true;
        }
    } else {
        CStreaming::RequestModel(left, STREAMING_KEEP_IN_MEMORY);
    }
    if (CStreaming::IsModelLoaded(right)) {
        if (!m_bRightHandLoaded) {
            CModelInfo::GetModelInfo(right)->AddRef();
            m_bRightHandLoaded = true;
        }
    } else {
        CStreaming::RequestModel(right, STREAMING_KEEP_IN_MEMORY);
    }
    if (m_bLeftHandLoaded && m_bRightHandLoaded) {
        if (ms_animBlock == uint32(-1)) {
            ms_animBlock = CAnimManager::GetAnimationBlockIndex("ghands");
        }
        // The original task reads the executable's 20-byte animation-block table.
        if (*reinterpret_cast<const uint8*>(0x961ADC + 20 * ms_animBlock) == 1) {
            if (!m_bAnimationLoaded) {
                CAnimManager::AddAnimBlockRef(ms_animBlock);
                m_bAnimationLoaded = true;
            }
        } else {
            CStreaming::RequestModel(IFPToModelId(ms_animBlock), STREAMING_KEEP_IN_MEMORY);
        }
    }
    if (m_bAnimationLoaded && m_pSubTask->GetTaskType() == static_cast<eTaskType>(0xCB)) {
        return CreateSubTask(static_cast<eTaskType>(0x1A9));
    }
    return result;
}
