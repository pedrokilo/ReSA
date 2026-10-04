/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendHierarchy
 * Reconstrucción: COMPLETA — 16/16 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 16/16 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimBlendHierarchy.h"

/**
 * Registra las operaciones de ciclo de vida, compresión y consulta de jerarquías.
 */
void CAnimBlendHierarchy::InjectHooks() {
    RH_ScopedClass(CAnimBlendHierarchy);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4CF270);
    RH_ScopedInstall(Destructor, 0x4CF970);
    RH_ScopedInstall(Shutdown, 0x4CF980);
    RH_ScopedInstall(RemoveAnimSequences, 0x4CF8E0);
    RH_ScopedInstall(AllocSequenceBlock, 0x4CF510);
    RH_ScopedInstall(FindSequence, 0x4CF290);
    RH_ScopedInstall(SetName, 0x4CF2D0);
    RH_ScopedInstall(CalcTotalTime, 0x4CF2F0);
    RH_ScopedInstall(CalcTotalTimeCompressed, 0x4CF3E0);
    RH_ScopedInstall(RemoveQuaternionFlips, 0x4CF4E0);
    RH_ScopedInstall(GetSequenceBlock, 0x4CF560);
    RH_ScopedInstall(Uncompress, 0x4CF5F0);
    RH_ScopedInstall(CompressKeyframes, 0x4CF6C0);
    RH_ScopedInstall(RemoveUncompressedData, 0x4CF760);
    RH_ScopedInstall(MoveMemory, 0x4CF800);
    RH_ScopedInstall(Print, 0x4CF8A0);
}

// 0x4CF270
// Inicializa una jerarquía sin secuencias ni bloque de animación asociado.
CAnimBlendHierarchy::CAnimBlendHierarchy() {
    // Como el original: m_hashKey queda sin escribir.
    m_pSequences = nullptr;
    m_nSeqCount = 0;
    m_bIsCompressed = false;
    m_bKeepCompressed = false;
    m_nAnimBlockId = -1;
    m_fTotalTime = 0.0f;
    m_Link = nullptr;
}

// 0x4CF970
CAnimBlendHierarchy::~CAnimBlendHierarchy() {
    RemoveAnimSequences();
}

// 0x4CF980
void CAnimBlendHierarchy::Shutdown() {
    RemoveAnimSequences();
    m_bIsCompressed = false;
}

// 0x4CF8E0
/**
 * Elimina todas las secuencias y retira la jerarquía de la caché descomprimida.
 *
 * Cuando las secuencias comparten un bloque externo de frames, ese bloque se libera
 * una sola vez después de destruir el array de CAnimBlendSequence.
 */
void CAnimBlendHierarchy::RemoveAnimSequences() {
    void* oldFrameData = nullptr;
    if (m_pSequences && m_pSequences->m_bUsingExternalMemory) {
        oldFrameData = m_pSequences->m_Frames;
    }
    CAnimManager::RemoveFromUncompressedCache(this);
    delete[] m_pSequences;
    m_pSequences = nullptr;
    m_nSeqCount = 0;
    m_fTotalTime = 0.0f;
    if (oldFrameData) {
        CMemoryMgr::Free(oldFrameData);
    }
}

// 0x4CF510
// Reserva un bloque contiguo capaz de almacenar todas las secuencias en el formato elegido.
uint8* CAnimBlendHierarchy::AllocSequenceBlock(bool compressed) const {
    // Como el original: contador con signo (movsx).
    size_t size = 0;
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        size += m_pSequences[i].GetDataSize(compressed);
    }
    return static_cast<uint8*>(CMemoryMgr::Malloc(size, 0));
}

// 0x4CF290
// Busca una secuencia por hash uppercase de nombre; devuelve nullptr si no existe.
CAnimBlendSequence* CAnimBlendHierarchy::FindSequence(const char* name) const {
    // Como el original: calcula el hash antes de mirar el contador (con signo) y compara la palabra
    // identificadora completa sin mirar si la secuencia usa bone tag.
    const auto hash = CKeyGen::GetUppercaseKey(name);
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        if (*reinterpret_cast<const uint32*>(&m_pSequences[i]) == hash) {
            return &m_pSequences[i];
        }
    }
    return nullptr;
}

// 0x4CF2D0
void CAnimBlendHierarchy::SetName(const char* name) {
    m_hashKey = CKeyGen::GetUppercaseKey(name);
}

// 0x4CF2F0
void CAnimBlendHierarchy::CalcTotalTime() {
    // Como el original: contadores con signo, paso según el bit de traslación (sin mirar el de compresión),
    // máximo con "dt > total" y deltas restados del último al segundo keyframe.
    m_fTotalTime = 0.0f;
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        auto& seq = m_pSequences[i];
        // Como el original: sin keyframes se lee el keyframe -1 (antes del bloque de frames) y no hay deltas
        const int32 numFrames = static_cast<int16>(seq.m_FramesNum);
        const int32 stride    = seq.m_bHasTranslation ? 32 : 20;
        auto* const frames    = static_cast<uint8*>(seq.m_Frames);
        const float last      = *reinterpret_cast<const float*>(frames + (numFrames - 1) * stride + 16);
        if (last > m_fTotalTime) {
            m_fTotalTime = last;
        }
        for (int32 j = numFrames - 1; j >= 1; j--) {
            auto& dt = *reinterpret_cast<float*>(frames + j * stride + 16);
            dt = dt - *reinterpret_cast<const float*>(frames + (j - 1) * stride + 16);
        }
    }
}

// 0x4CF3E0
void CAnimBlendHierarchy::CalcTotalTimeCompressed() {
    // Como el original: el tiempo del último keyframe es int16 ·0x3C888889 (1/60 del EXE) y los deltas se restan
    // en enteros de 16 bits (antes se convertían a float y se volvían a redondear con +0,5).
    m_fTotalTime = 0.0f;
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        auto& seq = m_pSequences[i];
        // Como el original: sin keyframes se lee el keyframe -1 (antes del bloque de frames) y no hay deltas
        const int32 numFrames = static_cast<int16>(seq.m_FramesNum);
        const int32 stride    = seq.m_bHasTranslation ? 16 : 10;
        auto* const frames    = static_cast<uint8*>(seq.m_Frames);
        const float last      = static_cast<float>(*reinterpret_cast<const int16*>(frames + (numFrames - 1) * stride + 8)) * std::bit_cast<float>(0x3C888889u);
        if (last > m_fTotalTime) {
            m_fTotalTime = last;
        }
        for (int32 j = numFrames - 1; j >= 1; j--) {
            auto& dt = *reinterpret_cast<int16*>(frames + j * stride + 8);
            dt = static_cast<int16>(dt - *reinterpret_cast<const int16*>(frames + (j - 1) * stride + 8));
        }
    }
}

// 0x4CF4E0
void CAnimBlendHierarchy::RemoveQuaternionFlips() const {
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        m_pSequences[i].RemoveQuaternionFlips();
    }
}

// 0x4CF560
void* CAnimBlendHierarchy::GetSequenceBlock() const {
    return m_pSequences->m_bUsingExternalMemory
        ? m_pSequences->m_Frames
        : nullptr;
}

// 0x4CF5F0
/**
 * Expande todas las secuencias en un bloque nuevo y libera el bloque comprimido previo.
 * La primera expansión también normaliza quaternions y calcula la duración total.
 */
void CAnimBlendHierarchy::Uncompress() {
    // Como el original: sin asserts; reserva el bloque nuevo antes de leer el antiguo y recorre con contador
    // con signo.
    auto uSeqData = AllocSequenceBlock(false);
    const auto cSeqData = GetSequenceBlock();

    // Now uncompress sequence data
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        m_pSequences[i].Uncompress(uSeqData);
        uSeqData += m_pSequences[i].GetDataSize(false);
    }

    // Now we no longer need the old compressed data
    if (cSeqData) {
        CMemoryMgr::Free(cSeqData);
    }

    m_bIsCompressed = false;
    if (m_fTotalTime == 0.0f) {
        RemoveQuaternionFlips();
        CalcTotalTime();
    }
}

// 0x4CF6C0
// Convierte todas las secuencias a un único bloque comprimido contiguo.
void CAnimBlendHierarchy::CompressKeyframes() const {
    auto cSeqData = AllocSequenceBlock(true);
    const auto uSeqData = GetSequenceBlock();

    // Compress current data
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        m_pSequences[i].CompressKeyframes(cSeqData);
        cSeqData += m_pSequences[i].GetDataSize(true);
    }

    // Now we no longer need the old uncompressed data
    if (uSeqData) {
        CMemoryMgr::Free(uSeqData);
    }
}

// 0x4CF760
void CAnimBlendHierarchy::RemoveUncompressedData() {
    // Como el original: CompressKeyframes en línea, pero llamando a CAnimBlendSequence::RemoveUncompressedData
    // (que salta las secuencias sin frames) en vez de a CompressKeyframes; sin asserts.
    auto cSeqData = AllocSequenceBlock(true);
    const auto uSeqData = GetSequenceBlock();

    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        m_pSequences[i].RemoveUncompressedData(cSeqData);
        cSeqData += m_pSequences[i].GetDataSize(true);
    }

    if (uSeqData) {
        CMemoryMgr::Free(uSeqData);
    }

    m_bIsCompressed = true;
}

// 0x4CF800
// Como el original (antes no estaba implementada): si el bloque es compartido lo mueve una vez y desplaza el
// puntero de todas las secuencias; si no, mueve cada secuencia. Devuelve si algo cambió de sitio.
bool CAnimBlendHierarchy::MoveMemory() {
    if (!m_pSequences) {
        return false;
    }

    bool moved = false;
    if (m_pSequences->m_bUsingExternalMemory) {
        auto* const oldData = static_cast<uint8*>(m_pSequences->m_Frames);
        if (!oldData) {
            return false;
        }
        auto* const newData = static_cast<uint8*>(CMemoryMgr::MoveMemory(oldData));
        if (newData == oldData) {
            return false;
        }
        for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
            m_pSequences[i].m_Frames = static_cast<uint8*>(m_pSequences[i].m_Frames) + (newData - oldData);
        }
        return true;
    }

    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        if (m_pSequences[i].MoveMemory()) {
            moved = true;
        }
    }
    return moved;
}

// 0x4CF8A0
void CAnimBlendHierarchy::Print() {
    CAnimManager::UncompressAnimation(this);
    for (int32 i = 0; i < static_cast<int16>(m_nSeqCount); i++) {
        m_pSequences[i].Print();
    }
}

// NOTSA
// Reserva el array propietario de secuencias que compondrá esta jerarquía.
void CAnimBlendHierarchy::SetNumSequences(size_t n) {
    m_nSeqCount  = (uint16)n;
    m_pSequences = new CAnimBlendSequence[n]; // Yes, they used `new`
}

uint32 CAnimBlendHierarchy::GetIndex() const {
    return CAnimManager::GetAnimIndex(this);
}
