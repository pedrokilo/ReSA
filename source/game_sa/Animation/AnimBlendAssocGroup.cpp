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
 * Validación registrada: 15/15 entradas.
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

// Como el original: CAnimManager::ms_aAnimations (0xB4EA40) indexado sin comprobar el rango.
static auto& s_AnimationsRaw = StaticRef<CAnimBlendHierarchy[2500]>(0xB4EA40);

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

    // Como el original: DestroyAssociations en línea, m_AnimBlock se guarda antes de reservar, m_NumAnims arranca
    // en 0 y cuenta cada vuelta (también las que no tienen modelo) y al final se fija a NumAnims. El bucle es con
    // signo y relee el bloque en cada vuelta. Sin registro (spdlog reserva memoria).
    DestroyAssociations();
    m_AnimBlock = CAnimManager::GetAnimationBlock(blockName);
    m_Anims     = new CAnimBlendStaticAssociation[m_AnimBlock->NumAnims];
    m_NumAnims  = 0;

    for (int32 i = 0; i < (int32)m_AnimBlock->NumAnims; i++) {
        const auto anim = &s_AnimationsRaw[m_AnimBlock->FirstAnimIdx + i];
        if (const auto mi = CModelInfo::GetModelInfoFromHashKey(anim->m_hashKey)) {
            CreateAssociation(&m_Anims[i], anim, mi, i);
        }
        m_NumAnims++;
    }
    m_NumAnims = m_AnimBlock->NumAnims;
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

    if (m_Anims) {
        DestroyAssociations();
    }
    m_AnimBlock = CAnimManager::GetAnimationBlock(blockName);
    m_Anims     = new CAnimBlendStaticAssociation[m_AnimBlock->NumAnims];

    // Se reinicia el contador porque algunas animaciones pueden no encontrarse;
    // incrementarlo sólo al crear una asociación mantiene el array compacto.
    m_NumAnims = 0;

    for (int32 i = 0; i < (int32)m_AnimBlock->NumAnims; i++) {
        const auto anim = &s_AnimationsRaw[m_AnimBlock->FirstAnimIdx + i];

        // Como el original: recorre todos los nombres sin cortar, así que gana la última coincidencia.
        int32 found = -1;
        const char* animName = animNames;
        for (int32 k = 0; k < (int32)m_AnimBlock->NumAnims; k++, animName += buffStride) {
            if (CKeyGen::GetUppercaseKey(animName) == anim->m_hashKey) {
                found = k;
            }
        }
        if (found == -1) {
            continue;
        }

        const char* modelName = modelNames + found * (int32)buffStride;
        CModelInfo::ms_lastPositionSearched = 0; // Why?
        const auto mi = CModelInfo::GetModelInfo(modelName, nullptr);
        if (!mi) {
            continue;
        }

        CreateAssociation(&m_Anims[m_NumAnims], anim, mi, i);
        ++m_NumAnims;
    }
}

// 0x4CE6E0
/**
 * Inicializa un número explícito de asociaciones usando un mismo RpClump.
 * Los nombres vacíos reservan su posición pero dejan ANIM_ID_UNDEFINED.
 */
void CAnimBlendAssocGroup::CreateAssociations(const char* blockName, RpClump* clump, const char** names, uint32 cnt) {
    ZoneScoped;

    // Como el original: DestroyAssociations en línea, m_NumAnims cuenta cada vuelta y al final vale cnt.
    DestroyAssociations();
    m_AnimBlock = CAnimManager::GetAnimationBlock(blockName);
    m_Anims     = new CAnimBlendStaticAssociation[cnt];
    m_NumAnims  = 0;

    for (int32 i = 0; i < (int32)cnt; i++) {
        const auto name  = names[i];
        const auto assoc = &m_Anims[i];

        if (name[0]) { // Check is empty string
            assoc->Init(clump, CAnimManager::GetAnimation(name, m_AnimBlock));
            assoc->m_AnimId = (AnimationId)(m_IdOffset + i);
        } else {
            assoc->m_AnimId = ANIM_ID_UNDEFINED;
        }

        assoc->m_AnimGroupId = m_GroupID;
        m_NumAnims++;
    }
    m_NumAnims = cnt;
}

// 0x4CDFF0
// Destruye el array propiedad del grupo y restablece su tamaño a cero.
void CAnimBlendAssocGroup::DestroyAssociations() {
    // Como el original: solo si hay array (si no, m_NumAnims no se toca). Con contador 0 libera el bloque
    // (cookie incluida) con operator delete; si no, llama al destructor vectorial por el hueco 0 de la vtable
    // del primer elemento con flags 3.
    if (!m_Anims) {
        return;
    }
    auto* const block = reinterpret_cast<uint32*>(m_Anims) - 1;
    if (*block) {
        using VectorDeletingDtor = void*(__thiscall*)(void*, uint32);
        (*reinterpret_cast<VectorDeletingDtor**>(m_Anims))[0](m_Anims, 3);
    } else {
        ::operator delete(block);
    }
    m_Anims    = nullptr;
    m_NumAnims = 0;
}

// 0x4CE040
// Busca por la misma clave uppercase utilizada al cargar las animaciones.
CAnimBlendStaticAssociation* CAnimBlendAssocGroup::GetAnimation(const char* name) {
    const auto key = CKeyGen::GetUppercaseKey(name);
    for (int32 i = 0; i < (int32)m_NumAnims; i++) { // Como el original: contador con signo
        if (m_Anims[i].m_BlendHier->m_hashKey == key) {
            return &m_Anims[i];
        }
    }
    return nullptr;
}

// 0x4CE090
CAnimBlendStaticAssociation* CAnimBlendAssocGroup::GetAnimation(AnimationId id) {
    return &m_Anims[id - m_IdOffset]; // Como el original: sin comprobar el rango
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
    // Como el original: de delante hacia atrás, con signo y releyendo m_NumAnims en cada vuelta.
    for (int32 i = 0; i < (int32)m_NumAnims; i++) {
        const auto anim = &m_Anims[i];
        if (!anim->IsValid()) {
            anim->Init(clump, anim->GetAnimHierarchy());
        }
    }
}

// 0x4CE1D0
CAnimBlendAssocGroup::~CAnimBlendAssocGroup() {
    DestroyAssociations();
}
