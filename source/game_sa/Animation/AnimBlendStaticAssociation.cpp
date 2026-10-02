/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendStaticAssociation
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimBlendStaticAssociation.h"

/**
 * Registra el ciclo de vida de las asociaciones estáticas de animación.
 */
void CAnimBlendStaticAssociation::InjectHooks() {
    RH_ScopedVirtualClass(CAnimBlendStaticAssociation, 0x85C6CC, 1);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor1, 0x4CE940);
    RH_ScopedInstall(Constructor2, 0x4CEF60);
    RH_ScopedInstall(Destructor, 0x4CDF50);

    RH_ScopedInstall(Init, 0x4CEC20);
    RH_ScopedInstall(AllocateSequenceArray, 0x4CE960);
    RH_ScopedInstall(FreeSequenceArray, 0x4CE9A0);
}

// 0x4CEF60
CAnimBlendStaticAssociation::CAnimBlendStaticAssociation(RpClump* clump, CAnimBlendHierarchy* h) {
    Init(clump, h);
}

// 0x4CEC00 (We hook with the vtbl with a deleting destructor wrapper, not this fn directly)
CAnimBlendStaticAssociation::~CAnimBlendStaticAssociation() {
    FreeSequenceArray();
}

// 0x4CEC20
/**
 * Construye el mapa frame-del-clump -> CAnimBlendSequence.
 *
 * Para cada secuencia busca el bone/frame correspondiente por tag o hash y guarda
 * un puntero en el índice equivalente del clump. No copia las secuencias ni toma
 * ownership de la jerarquía.
 */
void CAnimBlendStaticAssociation::Init(RpClump* clump, CAnimBlendHierarchy* h) {
    m_BlendHier = h;

    if (!clump) {
        return;
    }

    const auto clumpAnimData = RpAnimBlendClumpGetData(clump);
    assert(clumpAnimData);

    m_NumBlendNodes = clumpAnimData->m_NumFrameData;
    AllocateSequenceArray(m_NumBlendNodes);

    // Initialize sequences from the data in the clump
    for (auto& seq : m_BlendHier->GetSequences()) {
        if (!seq.m_FramesNum) {
            continue;
        }
        const auto frameData = seq.IsUsingBoneTag()
            ? RpAnimBlendClumpFindBone(clump, seq.GetBoneTag())
            : RpAnimBlendClumpFindFrameFromHashKey(clump, seq.GetNameHashKey());
        if (!frameData) {
            continue;
        }
        const auto nodeIdx = frameData - clumpAnimData->m_FrameDatas;
        assert(nodeIdx >= 0 && nodeIdx < m_NumBlendNodes);
        m_BlendSeqs[nodeIdx] = &seq;
    }
}

// 0x4CE960
// Reserva el array de punteros por nodo y lo inicializa a nullptr.
void CAnimBlendStaticAssociation::AllocateSequenceArray(int32 count) {
    m_BlendSeqs = (CAnimBlendSequence**)CMemoryMgr::Malloc(sizeof(CAnimBlendSequence*) * count);
    for (auto& array : std::span{ m_BlendSeqs, (size_t)count }) {
        array = nullptr;
    }
}

// 0x4CE9A0
// Libera únicamente el mapa de punteros; las secuencias pertenecen a la jerarquía.
void CAnimBlendStaticAssociation::FreeSequenceArray() {
    if (m_BlendSeqs) {
        CMemoryMgr::Free(m_BlendSeqs);
    }
}
