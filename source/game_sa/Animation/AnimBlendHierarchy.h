/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AnimBlendSequence.h"
#undef MoveMemory

/*!
 * @brief Jerarquía reutilizable que representa una animación completa.
 *
 * Contiene una CAnimBlendSequence por nodo/bone animado. Las asociaciones de clump
 * referencian esta jerarquía y crean su propio estado de reproducción; la jerarquía
 * permanece gestionada por CAnimManager mientras el bloque IFP siga cargado.
 *
 * Las secuencias pueden mantenerse comprimidas o expandirse temporalmente y entrar
 * en la caché LRU de animaciones descomprimidas.
 */
class CAnimBlendHierarchy {
public:
    uint32              m_hashKey;
    CAnimBlendSequence* m_pSequences; //!< Array propiedad de la jerarquía; una secuencia por nodo animado.
    uint16              m_nSeqCount;
    bool                m_bIsCompressed;
    bool                m_bKeepCompressed;
    int32               m_nAnimBlockId;
    float               m_fTotalTime;
    CLink<CAnimBlendHierarchy*>* m_Link; //!< Enlace utilizado por la caché/lista de jerarquías descomprimidas.

public:
    CAnimBlendHierarchy();
    ~CAnimBlendHierarchy();

    void Shutdown();

    // Reserva almacenamiento contiguo para todos los frames en el formato indicado.
    uint8* AllocSequenceBlock(bool compressed) const;

    //! @addr 0x4CF2F0
    void CalcTotalTime() { ICalcTotalTime<false>(); }

    //! @addr 0x4CF3E0
    void CalcTotalTimeCompressed() { ICalcTotalTime<true>(); }

    // Destruye secuencias, libera su bloque compartido y retira esta jerarquía de la caché.
    void RemoveAnimSequences();
    void RemoveQuaternionFlips() const;
    void RemoveUncompressedData();

    void SetName(const char* string);
    // Expande todas las secuencias y calcula datos derivados cuando sea necesario.
    void Uncompress();

    CAnimBlendSequence* FindSequence(const char* name) const;
    void* GetSequenceBlock() const;
    void CompressKeyframes() const;

    void MoveMemory();
    void Print();

    auto GetSequences() const { return std::span{ m_pSequences, (size_t)m_nSeqCount }; }
    auto GetHashKey() const { return m_hashKey; }
    auto GetTotalTime() const { return m_fTotalTime; }
    bool IsRunningCompressed() const { return m_bKeepCompressed; }
    bool IsUncompressed() const { return !m_bIsCompressed; }
    void SetNumSequences(size_t n);

    uint32 GetIndex() const;

private: // Function implementations
    template<bool Compressed>
    void ICalcTotalTime() {
        m_fTotalTime = 0.0f;
        for (auto& seq : GetSequences()) {
            if (seq.m_FramesNum == 0) { // FIX_BUGS by Mitchell Tobass
                continue;
            }
            m_fTotalTime = std::max<float>(m_fTotalTime, seq.GetKeyFrame<Compressed>(seq.m_FramesNum - 1)->DeltaTime);
            for (auto j = seq.m_FramesNum; j --> 1;) {
                const auto kfA = seq.GetKeyFrame<Compressed>(j - 1);
                const auto kfB = seq.GetKeyFrame<Compressed>(j);
                kfB->DeltaTime = kfB->DeltaTime - kfA->DeltaTime; // TODO/NOTE: With `FixedFloat` this has unnecessary conversions float <=> int
            }
        }
    }

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CAnimBlendHierarchy* Constructor() { this->CAnimBlendHierarchy::CAnimBlendHierarchy(); return this; }
    CAnimBlendHierarchy* Destructor() { this->CAnimBlendHierarchy::~CAnimBlendHierarchy(); return this; }
};

VALIDATE_SIZE(CAnimBlendHierarchy, 0x18);

