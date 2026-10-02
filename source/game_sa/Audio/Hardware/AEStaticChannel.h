#pragma once
#include "AEAudioChannel.h"

class NOTSA_EXPORT_VTABLE CAEStaticChannel : public CAEAudioChannel {
public:
    bool                  m_bNeedData;
    bool                  m_bUnkn2;
    bool                  m_bNeedsSynch;
    bool                  m_bUnkn4;
    int32                 m_nCurrentBufferOffset;
    int32                 field_68;
    int32                 field_6C;
    uint32                m_nSyncTime;
    int32                 field_74;
    int32                 m_dwLockOffset;
    int32                 m_nNumLockBytes;
    uint16                m_nNumLoops;
    uint16                field_82;
    const void*           m_pBuffer;            //!< Source PCM bytes; this is not a COM interface.
    uint16                field_88;
    bool                  m_IsHardwareMixAvailable;
    char                  field_8B;
    int16                 field_8C;
    uint16                _pad8E;

public:
    CAEStaticChannel(IDirectSound* pDirectSound, uint16 channelId, bool arg3, uint32 samplesPerSec, uint16 bitsPerSample);

    void   Service() override;
    bool   IsSoundPlaying() override;
    int16  GetPlayTime() override;
    uint16 GetLength() override;
    void   Play(int16 timeInMs, int8 unused, float scalingFactor) override;
    void   SynchPlayback() override;
    void   Stop() override;

    bool SetAudioBuffer(const void* buffer, uint32 size, int16 f88, int16 f8c, int16 loopOffset, uint16 frequency);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CAEStaticChannel, 0x90);
