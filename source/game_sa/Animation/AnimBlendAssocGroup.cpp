/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendAssocGroup
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/15 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "AnimBlendAssociation.h"
#include "AnimBlendStaticAssociation.h"
#include "RenderWare.h"
#include "AnimBlock.h"
#include "AnimBlendAssocGroup.h"

/**
 * Registra las operaciones de los grupos de asociaciones de animación.
 */
void CAnimBlendAssocGroup::InjectHooks() {
    RH_ScopedClass(CAnimBlendAssocGroup);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4CDE70);
    RH_ScopedInstall(Destructor, 0x4CE1D0);

    RH_ScopedInstall(GetNumAnimations, 0x45B050);
    RH_ScopedInstall(GetAnimBlock, 0x45B060);
    RH_ScopedInstall(InitEmptyAssociations, 0x4CDFB0);
    RH_ScopedInstall(DestroyAssociations, 0x4CDFF0);
    RH_ScopedOverloadedInstall(GetAnimation, "Name", 0x4CE040, CAnimBlendStaticAssociation *(CAnimBlendAssocGroup::*)(const char*));
    RH_ScopedOverloadedInstall(GetAnimation, "AnimationId", 0x4CE090, CAnimBlendStaticAssociation *(CAnimBlendAssocGroup::*)(AnimationId));
    RH_ScopedOverloadedInstall(CopyAnimation, "Name", 0x4CE0B0, CAnimBlendAssociation *(CAnimBlendAssocGroup::*)(const char*));
    RH_ScopedOverloadedInstall(CopyAnimation, "AnimationId", 0x4CE130, CAnimBlendAssociation *(CAnimBlendAssocGroup::*)(AnimationId));
    RH_ScopedInstall(GetAnimationId, 0x4CE1B0);
    RH_ScopedOverloadedInstall(CreateAssociations, "1", 0x4CE220, void(CAnimBlendAssocGroup::*)(const char*));
    RH_ScopedOverloadedInstall(CreateAssociations, "2", 0x4CE6E0, void(CAnimBlendAssocGroup::*)(const char*, RpClump*, const char**, uint32));
    RH_ScopedOverloadedInstall(CreateAssociations, "3", 0x4CE3B0, void(CAnimBlendAssocGroup::*)(const char*, const char*, const char*, uint32));
    //RH_ScopedOverloadedInstall(CreateAssociations, "", 0x4CE5C0, void(CAnimBlendAssocGroup::*)(const char*, RpClump*), { .reversed = false });
    RH_ScopedInstall(IsCreated, 0x4D37A0);
}

// 0x4CE0B0
// Resuelve la definición estática por nombre y crea una asociación reproducible independiente.
CAnimBlendAssociation* CAnimBlendAssocGroup::CopyAnimation(const char* name) {
    return CopyAnimation(GetAnimation(name));
}

// 0x4CE130
CAnimBlendAssociation* CAnimBlendAssocGroup::CopyAnimation(AnimationId id) {
    return CopyAnimation(GetAnimation(id));
}

// Implementación común: asegura que la jerarquía esté descomprimida antes de
// instanciar la asociación dinámica que mantendrá su propio estado de reproducción.
CAnimBlendAssociation* CAnimBlendAssocGroup::CopyAnimation(CAnimBlendStaticAssociation* anim) {
    if (!anim) {
        return nullptr;
    }
    CAnimManager::UncompressAnimation(anim->m_BlendHier);
    return new CAnimBlendAssociation(*anim);
}

// NOTSA
/**
 * Inicializa una asociación estática contra un clump temporal creado desde @p mi.
 *
 * El clump sólo se usa para resolver la correspondencia entre la jerarquía del
 * modelo y la animación; después se destruye. La asociación conserva IDs de grupo
 * y animación, no ownership sobre ese clump temporal.
 */
void CAnimBlendAssocGroup::CreateAssociation(CAnimBlendStaticAssociation* assoc, CAnimBlendHierarchy* anim, CBaseModelInfo* mi, size_t groupAnimId) {
    const auto clump = reinterpret_cast<RpClump*>(mi->CreateInstance());
    RpAnimBlendClumpInit(clump);
    assoc->Init(clump, anim);
#ifdef SA_SKINNED_PEDS
    if (IsClumpSkinned(clump)) {
        RpClumpForAllAtomics(clump, AtomicRemoveAnimFromSkinCB, nullptr);
    }
#endif
    RpClumpDestroy(clump);

    assoc->m_AnimId      = (AnimationId)(m_IdOffset + groupAnimId);
    assoc->m_AnimGroupId = (AssocGroupId)(m_GroupID);
}

// 0x4CE220
/**
 * Crea una asociación por cada animación del bloque.
 *
 * Cada jerarquía se vincula al model info cuyo hash coincide con el almacenado
 * por la animación. Las entradas sin modelo correspondiente se dejan sin inicializar.
 */
void CAnimBlendAssocGroup::CreateAssociations(const char* blockName) {
    ZoneScoped;

    const auto ablock = AllocateForBlock(blockName, -1);

    for (size_t i = 0; i < ablock->NumAnims; i++) {
        const auto id    = (AnimationId)(ablock->FirstAnimIdx + i);
        const auto anim  = &CAnimManager::GetAnimation(id);
        const auto assoc = &GetAssociations()[i];
        const auto mi    = CModelInfo::GetModelInfoFromHashKey(anim->m_hashKey);
        if (!mi) {
            NOTSA_LOG_WARN("ModelInfo (Key={}) for animation(Block={}; ID={}) not found", anim->m_hashKey, blockName, (int)id);
            continue;
        }
        CreateAssociation(assoc, anim, mi, i);
    }

    NOTSA_LOG_DEBUG("Created #{} associations for block ({})", m_NumAnims, blockName);
}

// 0x4CE3B0
/**
 * Construye sólo las asociaciones que pueden resolverse en dos buffers paralelos
 * de nombres de animación/modelo. m_NumAnims se compacta para no dejar huecos.
 */
void CAnimBlendAssocGroup::CreateAssociations(const char* blockName, const char* animNames, const char* modelNames, uint32 buffStride) {
    ZoneScoped;

    if (!modelNames) {
        return CreateAssociations(blockName);
    }

    const auto ab = AllocateForBlock(blockName, -1);

    // Se reinicia el contador porque algunas animaciones pueden no encontrarse;
    // incrementarlo sólo al crear una asociación mantiene el array compacto.
    m_NumAnims = 0;

    for (size_t i = 0; i < ab->NumAnims; i++) {
        const auto id    = (AnimationId)(ab->FirstAnimIdx + i);
        const auto anim  = &CAnimManager::GetAnimation(id);
        const auto assoc = &m_Anims[m_NumAnims]; // Can't use GetAssociations() helper method, as m_NumAnims will always be (no. of elements + 1)

        const auto* animName = animNames;
        const auto* modelName = modelNames;

        // Busca la pareja nombre de animación/modelo. Ambos buffers son paralelos:
        // la entrada k de animNames corresponde a la entrada k de modelNames.
        bool found;
        for (size_t k = 0; found = k < ab->NumAnims;k++, animName += buffStride, modelName += buffStride) {
            if (CKeyGen::GetUppercaseKey(animName) == anim->GetHashKey()) {
                break;
            }
        }
        if (!found) {
            NOTSA_LOG_WARN("Name of Animation(Block={}; ID={}) not found", modelName, blockName, (int)id);
            continue;
        }

        CModelInfo::ms_lastPositionSearched = 0; // Why?
        const auto mi = CModelInfo::GetModelInfo(modelName);
        if (!mi) {
            NOTSA_LOG_WARN("ModelInfo ({}) for Animation (Name={}; Block={}; ID={}) not found", modelName, animName, blockName, (int)id);
            continue;
        }

        CreateAssociation(assoc, anim, mi, i);
        ++m_NumAnims;

        NOTSA_LOG_TRACE("Created Animation(Name={}; Block={}; ID={}) from model ({})", animName, blockName, (int)id, modelName);
    }

    NOTSA_LOG_TRACE("Created #{} associations for block ({})", m_NumAnims, blockName);
}

// 0x4CE6E0
/**
 * Inicializa un número explícito de asociaciones usando un mismo RpClump.
 * Los nombres vacíos reservan su posición pero dejan ANIM_ID_UNDEFINED.
 */
void CAnimBlendAssocGroup::CreateAssociations(const char* blockName, RpClump* clump, const char** names, uint32 cnt) {
    ZoneScoped;

    AllocateForBlock(blockName, cnt);

    for (size_t i = 0; i < cnt; i++) {
        const auto name  = names[i];
        const auto assoc = &GetAssociations()[i];

        if (name[0]) { // Check is empty string
            assoc->Init(clump, CAnimManager::GetAnimation(name, m_AnimBlock));
            assoc->m_AnimId = (AnimationId)(m_IdOffset + i);
            assert(assoc->m_AnimId < ANIM_ID_MAX);
        } else {
            assoc->m_AnimId = ANIM_ID_UNDEFINED;
        }

        assoc->m_AnimGroupId = m_GroupID;
    }

    NOTSA_LOG_DEBUG("Created #{} associations for block ({})", m_NumAnims, blockName);
}

// 0x4CDFF0
// Destruye el array propiedad del grupo y restablece su tamaño a cero.
void CAnimBlendAssocGroup::DestroyAssociations() {
    delete[] std::exchange(m_Anims, nullptr);
    m_NumAnims = 0;
}

// 0x4CE040
// Busca por la misma clave uppercase utilizada al cargar las animaciones.
CAnimBlendStaticAssociation* CAnimBlendAssocGroup::GetAnimation(const char* name) {
    const auto key = CKeyGen::GetUppercaseKey(name);
    for (auto& a : GetAssociations()) {
        if (a.GetHashKey() == key) {
            return &a;
        }
    }
    return nullptr;
}

// 0x4CE090
CAnimBlendStaticAssociation* CAnimBlendAssocGroup::GetAnimation(AnimationId id) {
    return &GetAssociations()[id - m_IdOffset];
}

// 0x4CE1B0
AnimationId CAnimBlendAssocGroup::GetAnimationId(const char* AnimName) {
    if (const auto a = GetAnimation(AnimName)) {
        return a->m_AnimId;
    }
    return ANIM_ID_UNDEFINED;
}

// 0x4CDFB0
void CAnimBlendAssocGroup::InitEmptyAssociations(RpClump* clump) {
    for (size_t i = m_NumAnims; i-- > 0;) {
        const auto anim = &GetAssociations()[i];
        if (!anim->IsValid()) {
            anim->Init(clump, anim->GetAnimHierarchy());
        }
    }
}

// NOTSA
/**
 * Reemplaza cualquier array anterior y reserva asociaciones para el bloque solicitado.
 * @return El CAnimBlock global; el grupo no es propietario del bloque.
 */
CAnimBlock* CAnimBlendAssocGroup::AllocateForBlock(const char* blockName, int32 numAnims) {
    if (m_Anims) {
        DestroyAssociations();
    }

    const auto ablock = CAnimManager::GetAnimationBlock(blockName);

    if (numAnims == -1) {
        numAnims = (int32)ablock->NumAnims;
    }

    m_AnimBlock = ablock;
    m_Anims     = new CAnimBlendStaticAssociation[numAnims];
    m_NumAnims  = numAnims;

    return ablock;
}

// 0x4CE1D0
CAnimBlendAssocGroup::~CAnimBlendAssocGroup() {
    DestroyAssociations();
}
