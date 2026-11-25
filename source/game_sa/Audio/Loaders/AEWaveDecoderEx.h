#pragma once

#include <vector>

#include "AEStreamingDecoder.h"
#include "WaveExtCore.h"

/**
 * NOTSA: decoder de WAV PCM ampliado para user tracks (SilentPatch, MIT: "PCM WAVE profiles" y "WAVE con ID3"). No existe en el EXE.
 * Acepta PCM de 8/16/24 bits, mono o estereo, hasta 48 kHz, con cualquier chunk (ID3, LIST...) antes o despues de 'data'. Salida: PCM estereo de 16 bits a la frecuencia del archivo.
 * Se usa solo con la opcion SilentPatch WaveProfilesFix y solo si el archivo no es el WAV canonico que ya lee CAEWaveDecoder.
 */
class CAEWaveDecoderEx : public CAEStreamingDecoder {
public:
    explicit CAEWaveDecoderEx(CAEDataStream* dataStream);
    ~CAEWaveDecoderEx() override = default;

    bool   Initialise() override;
    size_t FillBuffer(void* dest, size_t size) override;
    long   GetStreamLengthMs() override;
    long   GetStreamPlayTimeMs() override;
    void   SetCursor(unsigned long pos) override;
    int32  GetSampleRate() override;
    int32  GetStreamID() override;

    /** true si el archivo es el WAV de 44 bytes (PCM estereo de 16 bits, chunk data justo despues de fmt de 16 bytes y sin chunks detras de los datos) que ya acepta CAEWaveDecoder tal cual. */
    static bool IsCanonicalHeader(CAEDataStream* dataStream);

private:
    bool                 m_bInitialized = false;
    WaveExtInfo          m_info{};
    std::vector<uint8_t> m_scratch;
};
