#pragma once

#include "vorbis/vorbisfile.h"

#include "AEStreamingDecoder.h"

/**
 * Decoder Ogg Vorbis respaldado por un CAEDataStream.
 *
 * libvorbis accede al stream exclusivamente mediante callbacks estáticos que
 * redirigen lectura/seek/tell/close al objeto fuente adoptado por la clase base.
 */
class CAEVorbisDecoder : public CAEStreamingDecoder {
public:
    CAEVorbisDecoder(CAEDataStream* dataStream, int32 userTrack);
    ~CAEVorbisDecoder() override;

    bool   Initialise() override;
    size_t FillBuffer(void* dest, size_t size) override;
    long   GetStreamLengthMs() override;
    long   GetStreamPlayTimeMs() override;
    void   SetCursor(unsigned long pos) override;
    int32  GetSampleRate() override;
    int32  GetStreamID() override;

private:
    ov_callbacks    m_sCallbacks;
    OggVorbis_File* m_pVorbisFileHandle; //!< Estado libvorbis propiedad del decoder.
    bool            m_bInitialized;
    bool            m_bIsUserTrack;
    CAEDataStream*  m_pDataStreamCopy;
    vorbis_info*    m_pVorbisInfo; //!< Metadata prestada por libvorbis; no se libera por separado.

    static ov_callbacks staticCallbacks;

    static size_t ReadCallback(void* ptr, size_t size, size_t nmemb, void* opaque);
    static int32    CloseCallback(void* opaque);
    static int32    SeekCallback(void* opaque, ogg_int64_t offset, int32 whence);
    static long   TellCallback(void* opaque);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEVorbisDecoder* Constructor(CAEDataStream* dataStream, int32 unknown);
    CAEVorbisDecoder* Destructor();
};

VALIDATE_SIZE(CAEVorbisDecoder, 0x28);
