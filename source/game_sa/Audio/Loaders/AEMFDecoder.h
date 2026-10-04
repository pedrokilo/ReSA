#pragma once

#include "AEStreamingDecoder.h"

/**
 * Decoder de streaming de user tracks (MP3/AAC...) basado en QuickTime, como gta-sa.exe 1.0 US.
 *
 * Como el original: toda la decodificación pasa por las funciones de pegamento QuickTime del EXE
 * (0x818xxx-0x81Axxx, llamadas a su dirección) y por sus ayudantes 0x4E7990/0x4E7A80/0x4E7B80.
 * Sin QuickTime instalado InitLibrary devuelve false y el gestor de user tracks pasa el MP3 al decoder WMA.
 */
class NOTSA_EXPORT_VTABLE CAEMFDecoder : public CAEStreamingDecoder {
public:
    // Como el original: SoundComponentData de QuickTime (28 bytes).
    struct tQTSoundComponentData {
        int32  flags;
        uint32 format;      // OSType
        int16  numChannels;
        int16  sampleSize;
        uint32 sampleRate;  // UnsignedFixed 16.16
        int32  sampleCount;
        uint8* buffer;
        int32  reserved;
    };
    VALIDATE_SIZE(tQTSoundComponentData, 0x1C);

    // Datos del formato de la pista que rellena 0x4E7A80 (local de 0x44 bytes de Initialise en el EXE).
    struct tQTSoundFormatInfo {
        uint32 unk00;
        int32  numChannels;   // +0x04 (escrito con movsx; Initialise lo lee como word)
        uint32 sampleRate;    // +0x08
        uint8  unk0C[0x1C];
        uint32 format;        // +0x28
        uint8  unk2C[0xC];
        int16  compressionID; // +0x38 (-2 = compresión variable)
        uint8  unk3A[4];
        int16  sampleSize;    // +0x3E
        uint8  unk40[4];
    };
    VALIDATE_SIZE(tQTSoundFormatInfo, 0x44);

private:
    static inline auto& quickTimeInitialized = StaticRef<bool>(0xB61D70);

    char*                 m_pFilename;            // 0x08
    bool                  m_bInitialized;         // 0x0C
    void*                 m_movie;                // 0x10
    void*                 m_media;                // 0x14
    void*                 m_soundConverter;       // 0x18
    tQTSoundComponentData m_inputFormat;          // 0x1C
    tQTSoundComponentData m_outputFormat;         // 0x38
    // 0x54..0xAD: bloque que recibe 0x4E7B80 (callback de SoundConverterFillBuffer) como refCon.
    tQTSoundComponentData m_fillData;             // 0x54
    int32                 m_fillRecordSize;       // 0x70
    int32                 m_fillExtendedFlags;    // 0x74
    int32                 m_fillBufferSize;       // 0x78
    uint8                 m_fillUnk7C[0x1C];      // 0x7C
    uint8**               m_sourceBuffer;         // 0x98 (Handle)
    void*                 m_sourceMedia;          // 0x9C
    long                  m_nPlayTime;            // 0xA0
    long                  m_nLength;              // 0xA4 (unidades de la media)
    int32                 m_sourceBufferSize;     // 0xA8
    bool                  m_bSourceDataLeft;      // 0xAC
    bool                  m_bVariableCompression; // 0xAD
    void*                 m_fillBufferProc;       // 0xB0 (0x4E7B80)
    int32                 m_nTrackID;             // 0xB4
    long                  m_nLengthMs;            // 0xB8
    int32                 m_nSampleRate;          // 0xBC
    float                 m_fTimeDivider;         // 0xC0

public:
    CAEMFDecoder(char* filename, int32 trackID);
    ~CAEMFDecoder() override;

    bool   Initialise() override;
    size_t FillBuffer(void* dest, size_t size) override;
    long   GetStreamLengthMs() override;
    long   GetStreamPlayTimeMs() override;
    void   SetCursor(unsigned long pos) override;
    int32  GetSampleRate() override;
    int32  GetStreamID() override;

    static bool InitLibrary();
    static void Shutdown();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEMFDecoder* Constructor(char* filename, int32 trackID);
    CAEMFDecoder* Destructor();
};

VALIDATE_SIZE(CAEMFDecoder, 0xc4);
