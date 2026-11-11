/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendStaticAssociation
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimBlendStaticAssociation.h"

namespace {
// 0x4CEC00 (lote 165A, banco Fn165A): cuerpo del destructor de CAnimBlendStaticAssociation del EXE (thiscall, sin argumentos de pila, `ret`, 20 bytes sin marco SEH: `mov [ecx],85C6CCh; mov ecx,[ecx+0Ch];
// test ecx,ecx; je ..; push ecx; call 0x72F430; pop ecx; ret`). Deja la vtable de la clase del EXE (0x85C6CC) y libera con CMemoryMgr::Free (0x72F430, con hook) el mapa de secuencias (+0xC) si no es nulo, SIN poner
// el puntero a cero (el destructor generado por el compilador dejaria la vtable de la DLL y FreeSequenceArray pone el puntero a cero). Lo llama la tabla de destructores del CRT (0x4CDF8A).
__declspec(noinline) void __fastcall CAnimBlendStaticAssociationDestructorBodyExe(CAnimBlendStaticAssociation* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85C6CC;
    void* const sequences = self->m_BlendSeqs;
    if (sequences) {
        CMemoryMgr::Free(sequences);
    }
}

// 0x4CEF30 (lote 165A, banco Fn165A): destructor con borrado de CAnimBlendStaticAssociation del EXE (vtable 0x85C6CC, 41 bytes; thiscall, un argumento de pila, `ret 4`) con el cuerpo expandido en linea: lee el
// mapa de secuencias (+0xC), deja la vtable (0x85C6CC) y libera el mapa con CMemoryMgr::Free (0x72F430, con hook) si no es nulo; despues libera `this` con operator delete (0x8214BD, CRT, sin hook) solo si el bit 0
// del BYTE de banderas esta puesto (a diferencia de 0x4CDF50, no mira el bit 1) y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CAnimBlendStaticAssociation* __fastcall CAnimBlendStaticAssociationScalarDeletingDestructor(CAnimBlendStaticAssociation* self, void* /*edx*/, uint8 flags) {
    void* const sequences = self->m_BlendSeqs;
    *reinterpret_cast<uint32*>(self) = 0x85C6CC;
    if (sequences) {
        CMemoryMgr::Free(sequences);
    }
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}
} // namespace

/**
 * Registra el ciclo de vida de las asociaciones estáticas de animación.
 */
void CAnimBlendStaticAssociation::InjectHooks() {
    RH_ScopedVirtualClass(CAnimBlendStaticAssociation, 0x85C6CC, 1);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor1, 0x4CE940);
    RH_ScopedInstall(Constructor2, 0x4CEF60);
    RH_ScopedInstall(Destructor, 0x4CDF50);
    RH_ScopedNamedGlobalInstall(CAnimBlendStaticAssociationDestructorBodyExe, "DestructorBody", 0x4CEC00);                          // lote 165A (banco Fn165A)
    RH_ScopedNamedGlobalInstall(CAnimBlendStaticAssociationScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4CEF30);        // lote 165A (banco Fn165A)

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

// 0x4CDF50
// Como el original: destructor virtual con borrado (`ret 4`). Con el bit 1 destruye el array (contador en
// this-4, de atrás hacia delante, como `eh vector destructor iterator`) y devuelve el bloque; con el bit 0
// libera la memoria con operator delete.
CAnimBlendStaticAssociation* CAnimBlendStaticAssociation::Destructor(uint8 flags) {
    if (flags & 2) {
        auto* const block = reinterpret_cast<uint32*>(this) - 1;
        for (uint32 i = *block; i-- > 0;) {
            (this + i)->CAnimBlendStaticAssociation::~CAnimBlendStaticAssociation();
        }
        if (flags & 1) {
            ::operator delete(block);
        }
        return reinterpret_cast<CAnimBlendStaticAssociation*>(block);
    }
    this->CAnimBlendStaticAssociation::~CAnimBlendStaticAssociation();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
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
    AllocateSequenceArray(static_cast<int16>(m_NumBlendNodes)); // Como el original: movsx del número de nodos

    // Initialize sequences from the data in the clump
    // Como el original: contador con signo (movsx de m_nSeqCount), la búsqueda del frame se hace antes de
    // mirar el número de frames y este se compara con signo
    for (int32 i = 0; i < static_cast<int16>(m_BlendHier->m_nSeqCount); ++i) {
        auto& seq = m_BlendHier->m_pSequences[i];
        const auto frameData = seq.IsUsingBoneTag()
            ? RpAnimBlendClumpFindBone(clump, seq.GetBoneTag())
            : RpAnimBlendClumpFindFrameFromHashKey(clump, seq.GetNameHashKey());
        if (!frameData || static_cast<int16>(seq.m_FramesNum) <= 0) {
            continue;
        }
        const auto nodeIdx = frameData - clumpAnimData->m_FrameDatas;
        m_BlendSeqs[nodeIdx] = &seq;
    }
}

// 0x4CE960
// Reserva el array de punteros por nodo y lo inicializa a nullptr.
void CAnimBlendStaticAssociation::AllocateSequenceArray(int32 count) {
    m_BlendSeqs = (CAnimBlendSequence**)CMemoryMgr::Malloc(sizeof(CAnimBlendSequence*) * count);
    for (int32 i = 0; i < count; ++i) { // Como el original: contador con signo, sin vueltas si count <= 0
        m_BlendSeqs[i] = nullptr;
    }
}

// 0x4CE9A0
// Libera únicamente el mapa de punteros; las secuencias pertenecen a la jerarquía.
void CAnimBlendStaticAssociation::FreeSequenceArray() {
    if (m_BlendSeqs) {
        CMemoryMgr::Free(m_BlendSeqs);
    }
}
