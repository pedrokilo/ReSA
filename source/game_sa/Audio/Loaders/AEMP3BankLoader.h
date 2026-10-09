#pragma once
#include "AEBankLoader.h"

/**
 * Configuración paralela de bank slot y sound ID para los virtual channels.
 *
 * Cada posición de BankSlotIDs corresponde a la misma posición de SoundIDs; los
 * centinelas SND_BANK_SLOT_NONE/-1 representan un canal sin sample asignado.
 */
struct tVirtualChannelSettings {
    std::array<eSoundBankSlot, 300> BankSlotIDs{};
    std::array<eSoundID, 300>       SoundIDs{};

    tVirtualChannelSettings() {
        rng::fill(BankSlotIDs, SND_BANK_SLOT_NONE);
        rng::fill(SoundIDs, -1);
    }
};
VALIDATE_SIZE(tVirtualChannelSettings, 0x4B0);

/**
 * Loader concreto que materializa bancos SFX en los slots gestionados por CAEBankLoader.
 *
 * Puede cargar un banco completo o un único sample. Las solicitudes se almacenan
 * en la cola fija heredada y Service() avanza su máquina de estados sobre CdStream.
 */
class CAEMP3BankLoader : public CAEBankLoader {
public:
    static void InjectHooks();

    CAEMP3BankLoader() = default;

    bool   Initialise();
    uint8* GetBankSlotBuffer(eSoundBankSlot bankSlot, uint32& outLength);
    float  GetSoundHeadroom(eSoundID soundId, eSoundBankSlot bankSlot);
    bool   IsSoundBankLoaded(eSoundBank bankId, eSoundBankSlot bankSlot);
    bool   GetSoundBankLoadingStatus(eSoundBank bankId, eSoundBankSlot bankSlot);
    uint8* GetSoundBuffer(eSoundID soundId, eSoundBankSlot bankSlot, uint32& outIndex, uint16& outSampleRate);
    int32  GetLoopOffset(eSoundID soundId, eSoundBankSlot bankSlot);
    bool   IsSoundLoaded(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) const;
    bool   GetSoundLoadingStatus(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot);
    void   UpdateVirtualChannels(tVirtualChannelSettings* settings, int16* lengths, int16* loopStartTimes);
    void   LoadSoundBank(eSoundBank bankId, eSoundBankSlot bankSlot);
    void   LoadSound(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot);
    // Procesa de forma incremental las solicitudes asíncronas pendientes.
    void   Service();

    // notsa
    CAEBankSlot& GetBankSlot(eSoundBankSlot slot) const;
private:
    // Inserta en la cola circular una solicitud de banco completo (sound == -1) o de un único sample.
    void AddRequest(eSoundBank bank, eSoundBankSlot slot, eSoundID sound);

private:
    friend class CAEAudioHardware; // Initialise reserva el bloque y llama a Constructor (el EXE no pone a cero el resto).
    // 0x4DFB10
    // Como el original: el constructor solo escribe m_IsInitialised y m_StreamingChannel (el resto queda sin tocar).
    CAEMP3BankLoader* Constructor() {
        m_IsInitialised    = false;
        m_StreamingChannel = 4;
        return this;
    }
};
VALIDATE_SIZE(CAEMP3BankLoader, 0x6E4);
