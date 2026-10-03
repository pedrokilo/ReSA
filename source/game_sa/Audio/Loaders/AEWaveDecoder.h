#pragma once

#define WIN32_LEAN_AND_MEAN

#include "winincl.h"
#include <mmreg.h> // WAVEFORMAT

#include "AEStreamingDecoder.h"

/**
 * Decoder ligero de WAV PCM de 16 bits.
 *
 * Como el original: lee la cabecera RIFF canónica de 44 bytes y sólo acepta PCM
 * estéreo de 16 bits; FillBuffer() copia directamente desde el stream.
 */
class NOTSA_EXPORT_VTABLE CAEWaveDecoder : public CAEStreamingDecoder {
public:
    CAEWaveDecoder(CAEDataStream* dataStream);
    virtual ~CAEWaveDecoder() = default; // 0x503290

    bool   Initialise() override;
    size_t FillBuffer(void* dest, size_t size) override;
    long   GetStreamLengthMs() override;
    long   GetStreamPlayTimeMs() override;
    void   SetCursor(unsigned long pos) override;
    int32  GetSampleRate() override;
    int32  GetStreamID() override;

private:
    bool           m_bInitialized;
    CAEDataStream* _dataStreamCopy; //!< Alias histórico del stream de la clase base; no ownership adicional.

    // RIFF header
    union {
        struct {
            char   riffFourCC[4];
            uint32 fileSize;
            char   waveFourCC[4];
            char   fmtFourCC[4];
            uint32 fmtChunkSize;
            uint16 formatTag;
            uint16 channelCount;
            uint32 sampleRate;
            uint32 bytesPerSecond;
            uint16 blockAlign;
            uint16 bitsPerSample;
            char   dataFourCC[4];  //!< Como el original: la cabecera del chunk data se lee junto con la cabecera RIFF.
            uint32 dataChunkSize;  //!< Longitud del chunk PCM.
        };
        char fileHeaderArray[44];
    };

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEWaveDecoder* Constructor(CAEDataStream* dataStream);
    void            Destructor();
};

VALIDATE_SIZE(CAEWaveDecoder, 0x3c);
