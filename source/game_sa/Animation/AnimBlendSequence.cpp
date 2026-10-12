/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendSequence
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

#include "AnimBlendSequence.h"
#include "X87Math.h"
#include <reversiblebugfixes/Bugs.hpp>

/**
 * Registra las operaciones de almacenamiento y conversión de keyframes.
 */
void CAnimBlendSequence::InjectHooks() {
    RH_ScopedClass(CAnimBlendSequence);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4D0C10);
    RH_ScopedInstall(Destructor, 0x4D0C30);
    RH_ScopedInstall(SetName, 0x4D0C50);
    RH_ScopedInstall(SetBoneTag, 0x4D0C70);
    RH_ScopedInstall(GetDataSize, 0x4D0C90);
    RH_ScopedInstall(SetNumFrames, 0x4D0CD0);
    RH_ScopedInstall(Uncompress, 0x4D0D40);
    RH_ScopedInstall(CompressKeyframes, 0x4D0F40);
    RH_ScopedInstall(MoveMemory, 0x4D1150);
    RH_ScopedInstall(Print, 0x4D1180);
    RH_ScopedInstall(RemoveQuaternionFlips, 0x4D1190);
    RH_ScopedInstall(RemoveUncompressedData, 0x4D12A0);
    RH_ScopedInstall(GetUKeyFrame, 0x4CF1F0);
    RH_ScopedInstall(GetCKeyFrame, 0x4CF220);
}

// 0x4D0C10
CAnimBlendSequence::CAnimBlendSequence() {
    // Como el original: pone a cero la palabra de flags completa (bits sin uso incluidos), el contador y el puntero,
    // y escribe solo la palabra baja del identificador (los bytes 2-3 quedan sin escribir).
    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(this) + 4) = 0;
    m_FramesNum = 0;
    m_Frames    = nullptr;
    m_BoneTag   = BONE_UNKNOWN;
}

// 0x4D0C30
// Sólo libera m_Frames cuando la secuencia es propietaria de ese almacenamiento.
CAnimBlendSequence::~CAnimBlendSequence() {
    // If we allocated memory, clean it
    if (!m_bUsingExternalMemory && m_Frames) {
        CMemoryMgr::Free(m_Frames);
    }
}

// 0x4D0F40
// Convierte el array a la variante comprimida apropiada según tenga traslación.
void CAnimBlendSequence::CompressKeyframes(uint8* pDataBlock) {
    // Como el original: sin comprobar si ya estaba comprimida; contador con signo (movsx).
    const auto num = static_cast<int16>(m_FramesNum);
    if (!num) {
        return;
    }

    // Como el original: rotación ·4096, traslación ·1024 y tiempo ·60 + 0,5, truncados con _ftol2 (64 bits) y
    // guardados como int16. Cada campo se lee y se escribe en el orden del EXE.
    const auto ToFixed = [](float v) { return static_cast<int16>(static_cast<int64>(v)); };
    uint8* const out = pDataBlock ? pDataBlock : static_cast<uint8*>(CMemoryMgr::Malloc((m_bHasTranslation ? 16u : 10u) * num, 0));
    const auto*  src = static_cast<const float*>(m_Frames);
    auto*        dst = reinterpret_cast<int16*>(out);
    if (m_bHasTranslation) {
        for (int32 i = 0; i < num; i++, src += 8, dst += 8) {
            dst[0] = ToFixed(src[0] * 4096.0f);
            dst[1] = ToFixed(src[1] * 4096.0f);
            dst[2] = ToFixed(src[2] * 4096.0f);
            dst[3] = ToFixed(src[3] * 4096.0f);
            dst[4] = ToFixed(src[4] * 60.0f + 0.5f);
            dst[5] = ToFixed(src[5] * 1024.0f);
            dst[6] = ToFixed(src[6] * 1024.0f);
            dst[7] = ToFixed(src[7] * 1024.0f);
        }
    } else {
        for (int32 i = 0; i < num; i++, src += 5, dst += 5) {
            dst[0] = ToFixed(src[0] * 4096.0f);
            dst[1] = ToFixed(src[1] * 4096.0f);
            dst[2] = ToFixed(src[2] * 4096.0f);
            dst[3] = ToFixed(src[3] * 4096.0f);
            dst[4] = ToFixed(src[4] * 60.0f + 0.5f);
        }
    }

    if (!m_bUsingExternalMemory) {
        CMemoryMgr::Free(m_Frames);
    }
    m_Frames               = out;
    m_bUsingExternalMemory = pDataBlock != nullptr;
    m_bIsCompressed        = true;
}

// 0x4D0D40
// Expande el array comprimido para permitir interpolación con valores completos.
void CAnimBlendSequence::Uncompress(uint8* pDataBlock) {
    // Como el original: sin comprobar si estaba comprimida; contador con signo (movsx).
    const auto num = static_cast<int16>(m_FramesNum);
    if (!num) {
        return;
    }

    // Como el original: reserva el tamaño SIN comprimir (32/20 bytes por frame; antes se pedía el comprimido y se
    // escribía fuera del bloque) y multiplica por las constantes del EXE: 1/4096, 1/1024 y 0x3C888889 (1/60, no /60).
    const auto inv60 = std::bit_cast<float>(0x3C888889u);
    uint8* const out = pDataBlock ? pDataBlock : static_cast<uint8*>(CMemoryMgr::Malloc((m_bHasTranslation ? 32u : 20u) * num, 0));
    const auto*  src = static_cast<const int16*>(m_Frames);
    auto*        dst = reinterpret_cast<float*>(out);
    if (m_bHasTranslation) {
        for (int32 i = 0; i < num; i++, src += 8, dst += 8) {
            dst[0] = static_cast<float>(src[0]) * 0.000244140625f;
            dst[1] = static_cast<float>(src[1]) * 0.000244140625f;
            dst[2] = static_cast<float>(src[2]) * 0.000244140625f;
            dst[3] = static_cast<float>(src[3]) * 0.000244140625f;
            dst[4] = static_cast<float>(src[4]) * inv60;
            dst[5] = static_cast<float>(src[5]) * 0.0009765625f;
            dst[6] = static_cast<float>(src[6]) * 0.0009765625f;
            dst[7] = static_cast<float>(src[7]) * 0.0009765625f;
        }
    } else {
        for (int32 i = 0; i < num; i++, src += 5, dst += 5) {
            dst[0] = static_cast<float>(src[0]) * 0.000244140625f;
            dst[1] = static_cast<float>(src[1]) * 0.000244140625f;
            dst[2] = static_cast<float>(src[2]) * 0.000244140625f;
            dst[3] = static_cast<float>(src[3]) * 0.000244140625f;
            dst[4] = static_cast<float>(src[4]) * inv60;
        }
    }

    if (!m_bUsingExternalMemory) {
        CMemoryMgr::Free(m_Frames);
    }
    m_Frames               = out;
    m_bUsingExternalMemory = pDataBlock != nullptr;
    m_bIsCompressed        = false;
}

// 0x4D12A0
void CAnimBlendSequence::RemoveUncompressedData(uint8* frameData) {
    if (m_FramesNum) {
        CompressKeyframes(frameData);
    }
}

// 0x4D0C90
size_t CAnimBlendSequence::GetDataSize(bool compressed) const {
    const auto kfSize = compressed
        ? m_bHasTranslation ? sizeof(KeyFrameTransCompressed) : sizeof(KeyFrameCompressed)
        : m_bHasTranslation ? sizeof(KeyFrameTrans) : sizeof(KeyFrame);
    // Como el original: el contador se extiende con signo (movsx).
    return kfSize * static_cast<int16>(m_FramesNum);
}

// 0x4D1190
/**
 * Mantiene quaternions consecutivos en el mismo hemisferio.
 *
 * q y -q describen la misma orientación; invertir el segundo cuando el producto
 * escalar es negativo evita interpolaciones que recorran el arco largo.
 */
void CAnimBlendSequence::RemoveQuaternionFlips() const {
    // Como el original: contador con signo y sin mirar el flag de compresión; el paso depende solo de la traslación.
    if (static_cast<int16>(m_FramesNum) < 2) {
        return;
    }

    float last[4];
    std::memcpy(last, m_Frames, sizeof(last));
    for (int32 i = 1; i < static_cast<int16>(m_FramesNum); i++) {
        auto* const f = reinterpret_cast<float*>(static_cast<uint8*>(m_Frames) + i * (m_bHasTranslation ? 32 : 20));
        if (AnimQuaternionDotNegativeCpp(last, f)) {
            f[0] = -f[0];
            f[1] = -f[1];
            f[2] = -f[2];
            f[3] = -f[3];
        }
        std::memcpy(last, f, sizeof(last));
    }
}

// 0x4D0C50
void CAnimBlendSequence::SetName(const char* name) {
    if (notsa::bugfixes::AnimBlendSequence_SetName_SetBoneTagFlag) {
        m_IsUsingBoneTag = false;
    }
    m_NameHashKey = CKeyGen::GetUppercaseKey(name);
}

// 0x4D0C70
void CAnimBlendSequence::SetBoneTag(eBoneTag32 boneId) {
    // Como el original: compara el valor de 32 bits con -1 (cmp eax,-1); eBoneTag es int16 y la comparación por enum
    // trataba como -1 cualquier valor con la palabra baja 0xFFFF (p. ej. 0xFFFF). Guarda la palabra baja.
    if (boneId.get_underlying() != -1) {
        m_IsUsingBoneTag = true;
        m_BoneTag         = static_cast<eBoneTag>(static_cast<int16>(boneId.get_underlying()));
    }
}

// 0x4D0CD0
/**
 * Configura formato y cantidad de frames y selecciona su almacenamiento.
 *
 * frameData transfiere únicamente el uso del buffer, no su ownership: cuando se
 * proporciona, m_bUsingExternalMemory impide que el destructor lo libere.
 */
void CAnimBlendSequence::SetNumFrames(uint32 count, bool bHasTranslation, bool compressed, void* frameData) {
    // Como el original: los flags de rotación, traslación y compresión se añaden con OR (nunca se borran) y el
    // tamaño del bloque se calcula con los argumentos y el contador de 32 bits, no con los flags guardados.
    const bool isExternal = frameData != nullptr;
    m_bHasRotation = true;
    if (bHasTranslation) {
        m_bHasTranslation = true;
    }
    if (!frameData) {
        frameData = CMemoryMgr::Malloc(compressed ? (bHasTranslation ? count * 16 : count * 10) : (bHasTranslation ? count * 32 : count * 20), 0);
    }
    m_Frames               = frameData;
    m_FramesNum            = (uint16)count;
    m_bUsingExternalMemory = isExternal; // NOTSA: el EXE no toca este flag (lo pone el cargador ANP3)
    if (compressed) {
        m_bIsCompressed = true;
    }
}

// 0x4D1150
// Solicita al memory manager reubicar sólo buffers propios y comunica si cambió la dirección.
bool CAnimBlendSequence::MoveMemory() {
    if (m_bUsingExternalMemory || !m_Frames) {
        return false;
    }

    const auto prevPtr = std::exchange(m_Frames, (CAnimBlendSequence*)CMemoryMgr::MoveMemory(m_Frames));
    return prevPtr != m_Frames; // Return true if the memory was moved
}

// 0x4D1180
void CAnimBlendSequence::Print() {
#ifndef FINAL

#endif
}
