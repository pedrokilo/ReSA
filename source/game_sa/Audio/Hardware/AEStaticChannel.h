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
    CAEStaticChannel* Constructor(IDirectSound* pDirectSound, uint16 channelId, bool hardwareMixAvailable, uint32 samplesPerSec, uint16 bitsPerSample); // 0x4F0B10 (lote 172A, banco Fn172A)

    void   Service() override;
    bool   IsSoundPlaying() override;
    int32  GetPlayTime() override;
    int32  GetLength() override;
    void   Play(int16 timeInMs, int8 unused, float scalingFactor) override;
    void   SynchPlayback() override;
    void   Stop() override;

    bool SetAudioBuffer(const void* buffer, uint32 size, int16 f88, int16 f8c, int16 loopOffset, uint16 frequency);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEStaticChannel* ScalarDeletingDestructor(uint8 flags); // 0x4F1020 (lote 142A, banco Fn142A): ranura 0 de la vtable 0x85F3CC del EXE, `ret 4`
};
VALIDATE_SIZE(CAEStaticChannel, 0x90);
