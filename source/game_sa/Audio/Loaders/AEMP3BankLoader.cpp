/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEMP3BankLoader
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AEMP3BankLoader.h"
#include "AEAudioUtility.h"
#include <cstdlib>

/**
 * Registra la implementación concreta de carga de bancos/sample SFX.
 */
void CAEMP3BankLoader::InjectHooks() {
    RH_ScopedClass(CAEMP3BankLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4DFB10);
    RH_ScopedInstall(Initialise, 0x4E08F0);
    RH_ScopedInstall(GetBankSlotBuffer, 0x4DFDE0);
    RH_ScopedInstall(GetSoundHeadroom, 0x4E01E0);
    RH_ScopedInstall(IsSoundBankLoaded, 0x4E0220);
    RH_ScopedInstall(GetSoundBankLoadingStatus, 0x4E0250);
    RH_ScopedInstall(GetSoundBuffer, 0x4E0280);
    RH_ScopedInstall(IsSoundLoaded, 0x4E03B0);
    RH_ScopedInstall(GetSoundLoadingStatus, 0x4E0400);
    RH_ScopedInstall(UpdateVirtualChannels, 0x4E0450);
    RH_ScopedInstall(LoadSoundBank, 0x4E0670);
    RH_ScopedInstall(LoadSound, 0x4E07A0);
    RH_ScopedInstall(Service, 0x4DFE30);
}

// 0x4E08F0
// Reinicia la cola y carga todas las tablas requeridas antes de aceptar solicitudes.
bool CAEMP3BankLoader::Initialise() {
    // Como el original (0x4DFB60, inline): de cada solicitud solo se escriben banco, slot y sonido (-1),
    // SlotInfo y Status; offsets, tamaños, punteros de stream y PakFileNo quedan como estaban.
    for (auto& req : m_Requests) {
        req.Bank     = SND_BANK_UNK;
        req.Slot     = SND_BANK_SLOT_NONE;
        req.SoundID  = -1;
        req.SlotInfo = nullptr;
        req.Status   = eSoundRequestStatus::INACTIVE;
    }
    m_NextOneSoundReqIdx = 0;
    m_RequestCnt         = 0;
    m_NextRequestIdx     = 0;

    if (!LoadBankSlotFile() || !LoadBankLookupFile() || !LoadSFXPakLookupFile()) {
        return false;
    }

    m_IsInitialised = true;
    return true;
}

// 0x4DFDE0, inlined
uint8* CAEMP3BankLoader::GetBankSlotBuffer(eSoundBankSlot bankSlot, uint32& outLength) {
    if (!m_IsInitialised) {
        return nullptr;
    }
    // Como el original: el slot se compara sin signo (movzx) con el contador con signo y fuera de rango se escribe 0.
    if ((int32)(uint16)(bankSlot) >= (int32)(int16)(m_BankSlotCnt)) {
        outLength = 0;
        return nullptr;
    }
    const auto& slot = m_BankSlots[(uint16)(bankSlot)];
    outLength = slot.NumBytes;
    return &m_Buffer[slot.OffsetBytes];
}

// 0x4E01E0
float CAEMP3BankLoader::GetSoundHeadroom(eSoundID soundId, eSoundBankSlot bankSlot) {
    // Como el original: sound ID sin signo y headroom * 0.01f (0x858C58), no / 100.
    return m_IsInitialised
        ? (float)(GetBankSlot(bankSlot).Sounds.data()[(uint16)(soundId)].Headroom) * 0.01f
        : 0.f;
}

// 0x4E0220
bool CAEMP3BankLoader::IsSoundBankLoaded(eSoundBank bankId, eSoundBankSlot bankSlot) {
    // Como el original: el banco del slot (movsx) se compara con el ID sin signo (movzx); -1 nunca coincide.
    return m_IsInitialised && (int32)(GetBankSlot(bankSlot).Bank) == (int32)(uint16)(bankId);
}

// 0x4E0250
bool CAEMP3BankLoader::GetSoundBankLoadingStatus(eSoundBank bankId, eSoundBankSlot bankSlot) {
    return IsSoundBankLoaded(bankId, bankSlot);
}

// 0x4E0280
/**
 * Devuelve una vista directa al sample cargado dentro del buffer del bank slot,
 * junto con tamaño y sample rate. Valida banco, slot y modo single-sound.
 */
uint8* CAEMP3BankLoader::GetSoundBuffer(eSoundID soundId, eSoundBankSlot bankSlot, uint32& outSize, uint16& outSampleRate) {
    if (!m_IsInitialised) {
        return nullptr;
    }

    const auto& slot = GetBankSlot(bankSlot);
    if (slot.Bank == SND_BANK_UNK) {
        return nullptr;
    }

    // Como el original: el sound ID se compara sin signo (movzx) y los contadores con signo; no se mira el contador
    // de slots ni se pasa por GetBankSlotBuffer/GetBankLookup.
    const auto sfx    = (int32)(uint16)(soundId);
    const auto sounds = slot.Sounds.data();
    if (slot.NumSounds != -1 ? (int32)(slot.NumSounds) <= sfx : (int32)(m_BankSlotSound.data()[bankSlot]) != sfx) {
        return nullptr;
    }

    outSize = (sfx < AE_BANK_MAX_NUM_SOUNDS && slot.NumSounds == -1) || sfx < (int32)(slot.NumSounds) - 1
        ? sounds[(sfx + 1) % AE_BANK_MAX_NUM_SOUNDS].BankOffsetBytes - sounds[sfx].BankOffsetBytes
        : m_BankLkups[slot.Bank].NumBytes - sounds[sfx].BankOffsetBytes;
    outSampleRate = sounds[sfx].SampleFrequency;

    return &m_Buffer[sounds[sfx].BankOffsetBytes + slot.OffsetBytes];
}

// 0x4E0380
int32 CAEMP3BankLoader::GetLoopOffset(eSoundID soundId, eSoundBankSlot bankSlot) {
    if (!m_IsInitialised) {
        return -1;
    }

    auto& slot = GetBankSlot(bankSlot);

    assert(slot.IsSingleSound() ? m_BankSlotSound[bankSlot] == soundId : soundId < slot.NumSounds);
    return slot.Sounds[soundId].LoopStartOffset;
}

// 0x4E03B0
bool CAEMP3BankLoader::IsSoundLoaded(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) const {
    // Como el original: banco y sonido guardados (movsx) frente a los IDs sin signo (movzx).
    return m_IsInitialised
        && (int32)(GetBankSlot(bankSlot).Bank) == (int32)(uint16)(bankId)
        && (int32)(m_BankSlotSound.data()[bankSlot]) == (int32)(uint16)(soundId);
}

// 0x4E0400
bool CAEMP3BankLoader::GetSoundLoadingStatus(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) {
    return IsSoundLoaded(bankId, soundId, bankSlot);
}

// 0x4E0450
// Deriva duración y loop-start en milisegundos para cada virtual channel configurado.
void CAEMP3BankLoader::UpdateVirtualChannels(tVirtualChannelSettings* settings, int16* lengths, int16* loopStartTimes) {
    for (auto i = 0u; i < std::size(settings->BankSlotIDs); i++) {
        const auto slotID = settings->BankSlotIDs.data()[i];
        const auto sfx    = settings->SoundIDs.data()[i];

        // Como el original: cualquier slot o sonido negativo invalida el canal; un banco con NumSounds negativo
        // acepta cualquier sonido.
        if (slotID < 0 || sfx < 0) {
            lengths[i]        = -1;
            loopStartTimes[i] = -1;
            continue;
        }
        const auto& slot = m_BankSlots[slotID];
        if (slot.Bank == SND_BANK_UNK || (sfx >= slot.NumSounds && slot.NumSounds >= 0)) {
            lengths[i]        = -1;
            loopStartTimes[i] = -1;
            continue;
        }

        // Como el original: el último sonido del banco termina en el tamaño del banco (lookup), el resto en el siguiente.
        const auto& item = slot.Sounds.data()[sfx];
        const auto  next = (int32)(sfx) == (int32)(slot.NumSounds) - 1
            ? m_BankLkups[slot.Bank].NumBytes
            : slot.Sounds.data()[sfx + 1].BankOffsetBytes;
        const auto length = CAEAudioUtility::ConvertFromBytesToMS(next - item.BankOffsetBytes, item.SampleFrequency, 1);

        const auto loopOffset = item.LoopStartOffset;
        const auto loopStart  = loopOffset != (uint32)(-1)
            ? CAEAudioUtility::ConvertFromBytesToMS(2 * loopOffset, item.SampleFrequency, 1) // Why `*2`?
            : (uint32)(-1);

        lengths[i]        = (int16)(length);
        loopStartTimes[i] = (int16)(loopStart);
    }
}

/**
 * Añade una solicitud a la cola circular y copia en ella los datos del lookup.
 * SoundID == -1 representa carga del banco completo.
 */
void CAEMP3BankLoader::AddRequest(eSoundBank bank, eSoundBankSlot slot, eSoundID sound) {
    const auto& lookup = GetBankLookup((uint16)(bank));

    // Como el original: se escriben campo a campo; StreamDataPtr/StreamBufPtr (y BankNumBytes en las de un sonido)
    // conservan su valor anterior.
    auto& req           = m_Requests.data()[(int16)(m_NextRequestIdx)];
    req.Bank            = bank;
    req.Slot            = slot;
    req.SoundID         = sound;
    req.SlotInfo        = &m_BankSlots[slot];
    req.PakFileNo       = lookup.PakFileNo;
    req.BankOffsetBytes = lookup.FileOffset;
    if (sound == -1) {
        req.BankNumBytes = lookup.NumBytes;
    }
    req.Status = eSoundRequestStatus::REQUESTED;

    m_RequestCnt++;
    m_NextRequestIdx = (uint16)(((int32)(int16)(m_NextRequestIdx) + 1) % (int32)(std::size(m_Requests)));
}

// 0x4E0670
void CAEMP3BankLoader::LoadSoundBank(eSoundBank bankId, eSoundBankSlot bankSlot) {
    if (!m_IsInitialised) {
        return;
    }

    // Como el original: banco sin signo frente al contador (con signo, `>`), slot con signo y `>` (no `>=`);
    // los rangos se comprueban antes de IsSoundBankLoaded.
    const auto bank = (int32)(uint16)(bankId);
    if (bank > (int32)(int16)(m_BankLkupCnt) || bankSlot < 0 || bankSlot > (int16)(m_BankSlotCnt)) {
        return;
    }

    // Already loaded?
    if (IsSoundBankLoaded(bankId, bankSlot)) {
        return;
    }

    // Como el original: una solicitud pendiente del mismo banco y slot basta, sea del banco o de un sonido.
    for (const auto& req : m_Requests) {
        if ((int32)(req.Bank) == bank && req.Slot == bankSlot) {
            return;
        }
    }

    AddRequest(bankId, bankSlot, -1);
}

// 0x4E07A0
void CAEMP3BankLoader::LoadSound(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) {
    if (!m_IsInitialised) {
        return;
    }

    // Como el original: primero el sound ID (sin signo), luego banco y slot y por último IsSoundLoaded.
    if ((uint16)(soundId) >= AE_BANK_MAX_NUM_SOUNDS) {
        return;
    }
    const auto bank = (int32)(uint16)(bankId);
    if (bank > (int32)(int16)(m_BankLkupCnt) || bankSlot < 0 || bankSlot > (int16)(m_BankSlotCnt)) {
        return;
    }

    if (IsSoundLoaded(bankId, soundId, bankSlot)) {
        return;
    }

    for (const auto& req : m_Requests) {
        if (req.SoundID == soundId && (int32)(req.Bank) == bank && req.Slot == bankSlot) {
            return;
        }
    }

    AddRequest(bankId, bankSlot, soundId);
}

// 0x4DFE30
/**
 * Máquina de estados de las cargas asíncronas.
 *
 * Inicia lecturas por sectores, espera a CdStream, copia metadata/sample al slot
 * definitivo y libera el buffer temporal antes de marcar la solicitud inactiva.
 */
void CAEMP3BankLoader::Service() {
    for (auto i = 0; i < (int32)(std::size(m_Requests)); i++) {
        auto& req = m_Requests[i];

        const auto AllocateMemoryAndRead = [&](uint32 readSizeSectors) {
            // Como el original: buffer de CMemoryMgr::Malloc, puntero de datos desplazado dentro del sector y
            // posición de CdStream = handle | (sector & 0xFFFFFF).
            const auto sector = req.BankOffsetBytes / STREAMING_SECTOR_SIZE;
            const auto buf    = (uint8*)(CMemoryMgr::Malloc(readSizeSectors * STREAMING_SECTOR_SIZE, 0));
            req.StreamBufPtr  = buf;
            req.StreamDataPtr = (AEAudioStream*)(buf + (req.BankOffsetBytes - sector * STREAMING_SECTOR_SIZE));

            CdStreamRead( // 0x4E016F
                (int16)(m_StreamingChannel),
                buf,
                std::bit_cast<CdStreamPos>((uint32)(m_StreamHandles[req.PakFileNo]) | (sector & 0xFFFFFF)),
                (int32)(readSizeSectors)
            );
        };

        // Como el original: al terminar se liberan el buffer y la solicitud (banco, slot y sonido a -1).
        const auto FinishRequest = [&] {
            CMemoryMgr::Free(req.StreamBufPtr);
            req.Bank         = SND_BANK_UNK;
            req.Slot         = SND_BANK_SLOT_NONE;
            req.SoundID      = -1;
            req.StreamBufPtr = nullptr;
            req.Status       = eSoundRequestStatus::INACTIVE;
            m_RequestCnt--;
        };

        switch (req.Status) {
        case eSoundRequestStatus::REQUESTED: { // 0x4E0117
            if (CdStreamGetStatus((int16)(m_StreamingChannel)) != eCdStreamStatus::READING_SUCCESS) {
                break;
            }

            // For single sound requests we load the header only and then later the sound, otherwise the whole bank
            AllocateMemoryAndRead(req.SoundID == -1 ? (req.BankNumBytes + sizeof(AEAudioStream)) / STREAMING_SECTOR_SIZE + 2 : 4);

            // Now wait for it to be read
            req.Status = eSoundRequestStatus::PENDING_READ;
            break;
        }
        case eSoundRequestStatus::PENDING_READ: { // 0x4DFF58
            if (CdStreamGetStatus((int16)(m_StreamingChannel)) != eCdStreamStatus::READING_SUCCESS) {
                break;
            }

            // Como el original: la metadata va al slot `m_BankSlots[req.Slot]`; SlotInfo solo da el offset del buffer.
            const auto stream = req.StreamDataPtr;
            auto&      slot   = m_BankSlots[req.Slot];

            if (req.SoundID == -1) { // 0x4DFF6E - Load whole bank?
                memcpy(&m_Buffer[req.SlotInfo->OffsetBytes], &stream->BankData, req.BankNumBytes);

                slot.Sounds    = stream->Sounds;
                slot.Bank      = req.Bank;
                slot.NumSounds = stream->NumSounds;

                m_BankSlotSound.data()[req.Slot] = -1; // Whole bank loaded, so use `-1`

                FinishRequest();
            } else { // 0x4E001B - Load specified sound only
                // At this point only the header (AEAudioStream) has been
                // loaded into memory, with that info we can calculate
                // where the sound's data is and load it in the next step
                slot.Sounds    = stream->Sounds;
                slot.Bank      = SND_BANK_UNK;
                slot.NumSounds = -1;

                m_BankSlotSound.data()[req.Slot] = -1; // This will be set when the sound has been loaded below

                // 0x4E006F - Calculate bank size
                const auto& item      = stream->Sounds.data()[req.SoundID];
                const auto  nextOrEnd = (int32)(req.SoundID) < (int32)(stream->NumSounds) - 1
                    ? stream->Sounds.data()[req.SoundID + 1].BankOffsetBytes // Otherwise use next sound's offset
                    : m_BankLkups[req.Bank].NumBytes;                        // If no more sounds we use the end of bank
                req.BankNumBytes     = nextOrEnd - item.BankOffsetBytes;
                req.BankOffsetBytes += item.BankOffsetBytes + sizeof(AEAudioStream);

                // 0x4E00BA - De-allocate old buffer
                CMemoryMgr::Free(req.StreamBufPtr);

                // 0x4E00C7 - Read the sound's data
                AllocateMemoryAndRead(req.BankNumBytes / STREAMING_SECTOR_SIZE + 2);

                req.Status = eSoundRequestStatus::PENDING_LOAD_ONE_SOUND;
            }
            break;
        }
        case eSoundRequestStatus::PENDING_LOAD_ONE_SOUND: { // 0x4DFE5A
            if (CdStreamGetStatus((int16)(m_StreamingChannel)) != eCdStreamStatus::READING_SUCCESS || req.SoundID == -1) {
                break;
            }

            // Copy over data into internal buffer as the request buffer will be deallocated now
            memcpy(&m_Buffer[req.SlotInfo->OffsetBytes], req.StreamDataPtr, req.BankNumBytes);

            auto& slot = m_BankSlots[req.Slot];
            slot.Bank                                                                  = req.Bank;
            slot.Sounds.data()[req.SoundID].BankOffsetBytes                            = 0;
            slot.Sounds.data()[(req.SoundID + 1) % AE_BANK_MAX_NUM_SOUNDS].BankOffsetBytes = req.BankNumBytes; // Como el original: módulo con signo

            m_BankSlotSound.data()[req.Slot] = req.SoundID;

            FinishRequest();

            if ((int32)(int16)(m_NextOneSoundReqIdx) == i) {
                m_NextOneSoundReqIdx = (uint16)((i + 1) % (int32)(std::size(m_Requests)));
            }
            break;
        }
        default: // Como el original: INACTIVE y estados desconocidos se ignoran.
            break;
        }
    }
}

CAEBankSlot& CAEMP3BankLoader::GetBankSlot(eSoundBankSlot slot) const {
    return m_BankSlots[slot];
}
