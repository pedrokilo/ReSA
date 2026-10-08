/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEScriptAudioEntity
 * Reconstrucción: INCOMPLETA — 8/19 funciones reconstruidas.
 * Pendientes de reconstruir: 11.
 * Funciones bloqueadas: 11.
 * Validación registrada: 0/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   AttachMissionAudioToPhysical | 0x004EC100 | S
 *   ClearMissionAudio | 0x004EC040 | S
 *   GetMissionAudioLoadingStatus | 0x004EBF60 | S
 *   GetMissionAudioPosition | 0x004EC4D0 | S
 *   IsMissionAudioSampleFinished | 0x004EBFE0 | S
 *   PlayLoadedMissionAudio | 0x004EC270 | S
 *   PlayMissionBankSound | 0x004EC6D0 | S
 *   PreloadMissionAudio | 0x004EC190 | S
 *   ProcessMissionAudioEvent | 0x004ECCF0 | S
 *   SetMissionAudioPosition | 0x004EC0C0 | S
 *   UpdateParameters | 0x00000000 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   AttachMissionAudioToPhysical | 0x004EC100 | S
 *   ClearMissionAudio | 0x004EC040 | S
 *   GetMissionAudioLoadingStatus | 0x004EBF60 | S
 *   GetMissionAudioPosition | 0x004EC4D0 | S
 *   IsMissionAudioSampleFinished | 0x004EBFE0 | S
 *   PlayLoadedMissionAudio | 0x004EC270 | S
 *   PlayMissionBankSound | 0x004EC6D0 | S
 *   PreloadMissionAudio | 0x004EC190 | S
 *   ProcessMissionAudioEvent | 0x004ECCF0 | S
 *   SetMissionAudioPosition | 0x004EC0C0 | S
 *   UpdateParameters | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEScriptAudioEntity.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "AEAudioHardware.h"
#include "AEAmbienceTrackManager.h"

// 0x5074D0
CAEScriptAudioEntity::CAEScriptAudioEntity() : CAEAudioEntity() {
    m_nLastTimeHornPlayed = 0;
    field_7E = 0;
    field_7C = 0;
    m_Volume = 0.0f;
    m_Speed = 1.0f;
    field_7D = 0;
    field_8C = 0.0f;
}

// 0x5B9B60
void CAEScriptAudioEntity::Initialise() {
    for (auto& link : wavLinks) {
        link.Init();
    }
}

// 0x4EC150
void CAEScriptAudioEntity::Reset() {
    for (auto i = 0; i < MISSION_AUDIO_COUNT; i++) {
        ClearMissionAudio(i);
    }
    field_7C = 0;
    m_Entity = nullptr;
    field_7D = 0;
    field_8C = 2.0f;
}

// 0x0
void CAEScriptAudioEntity::AddAudioEvent(int32) {
    /* Android NOP */
}

// 0x4EC100
CVector* CAEScriptAudioEntity::AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical) {
    auto& link = wavLinks.data()[sampleId];
    link.m_pEntity = physical;
    link.m_vPosition = CVector{-1000.f, -1000.f, -1000.f};
    return &link.m_vPosition;
}

// 0x4EC040
void CAEScriptAudioEntity::ClearMissionAudio(uint8 sampleId) {
    if (sampleId >= MISSION_AUDIO_COUNT) {
        return;
    }
    AESoundManager.CancelSoundsInBankSlot(int16(sampleId + 26), true);
    auto& link = wavLinks.data()[sampleId];
    link.m_pEntity = nullptr;
    link.m_vPosition = CVector{-1000.f, -1000.f, -1000.f};
    link.m_Sound = nullptr;
}

// 0x4EBFE0
bool CAEScriptAudioEntity::IsMissionAudioSampleFinished(uint8 sampleId) {
    if (sampleId >= MISSION_AUDIO_COUNT) {
        return true;
    }
    if (sampleId >= 2) {
        return AESoundManager.AreSoundsPlayingInBankSlot(int16(sampleId + 26)) == 0;
    }
    return wavLinks.data()[sampleId].m_Sound == nullptr;
}

// 0x4EBF60
int8 CAEScriptAudioEntity::GetMissionAudioLoadingStatus(uint8 sampleId) {
    if (sampleId >= MISSION_AUDIO_COUNT) {
        return 1;
    }
    const auto& link = wavLinks.data()[sampleId];
    if (link.m_nBankId < 0) {
        return 1;
    }
    const auto bank = static_cast<eSoundBank>(uint16(link.m_nBankId));
    const auto slot = static_cast<eSoundBankSlot>(sampleId + 26);
    if (link.m_nBankSlotId >= 0) {
        return AEAudioHardware.GetSoundLoadingStatus(bank, static_cast<eSoundID>(uint16(link.m_nBankSlotId)), slot);
    }
    return AEAudioHardware.GetSoundBankLoadingStatus(bank, slot);
}

// 0x4EC020
int32 CAEScriptAudioEntity::GetMissionAudioEvent(uint8 sampleId) {
    return wavLinks[sampleId].m_nAudioEvent;
}

// 0x4EC0C0
void CAEScriptAudioEntity::SetMissionAudioPosition(uint8 sampleId, CVector& posn) {
    auto& link = wavLinks.data()[sampleId];
    link.m_vPosition = posn;
    link.m_pEntity = nullptr;
}

// 0x4EC4D0
CVector* CAEScriptAudioEntity::GetMissionAudioPosition(uint8 sampleId) {
    auto& link = wavLinks.data()[sampleId];
    if (link.m_pEntity) {
        return &link.m_pEntity->GetPosition();
    }
    const auto& pos = link.m_vPosition;
    if (std::bit_cast<uint32>(pos.x) == 0xC47A0000u && std::bit_cast<uint32>(pos.y) == 0xC47A0000u && std::bit_cast<uint32>(pos.z) == 0xC47A0000u) {
        return nullptr;
    }
    return pos.x != 0.f || pos.y != 0.f || pos.z != 0.f ? &link.m_vPosition : nullptr;
}

// 0x4EC6D0
void CAEScriptAudioEntity::PlayMissionBankSound(eAudioEvents eventId, CVector& posn, CPhysical* physical, int16 sfxId, uint8 linkId, uint8 onlyIfNotPlaying, float volume, float maxDistance, float speed) {
    if (linkId < 2 || linkId >= MISSION_AUDIO_COUNT) {
        return;
    }
    if (onlyIfNotPlaying && AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(int16(eventId), this) != 0) {
        return;
    }
    const auto slot = static_cast<eSoundBankSlot>(linkId + 26);
    const auto bank = static_cast<eSoundBank>(uint16(wavLinks[linkId].m_nBankId));
    if (!AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
        return;
    }
    const auto event = static_cast<eAudioEvents>(uint16(eventId));
    const auto adjustedVolume = GetDefaultVolume(event) + volume;
    bool frontend{};
    CVector position;
    if (physical) {
        position = physical->GetPosition();
    } else if (posn == CVector{-1000.f, -1000.f, -1000.f} || posn == CVector{0.f, 0.f, 0.f}) {
        position = CVector{0.f, 1.f, 0.f};
        frontend = true;
    } else {
        position = posn;
        frontend = posn == CVector{-1.f, 0.f, 0.f} || posn == CVector{1.f, 0.f, 0.f};
    }
    m_tempSound.Initialise(slot, static_cast<eSoundID>(uint16(sfxId)), this, position, adjustedVolume, maxDistance, speed, 1.f, 0, 0, 0.f, 0);
    m_tempSound.m_Flags = 6;
    m_tempSound.SetFlags(SOUND_FRONT_END, frontend);
    if (physical) {
        m_tempSound.SetFlags(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY, 1);
        m_tempSound.RegisterWithPhysicalEntity(physical);
    }
    m_tempSound.m_Event = int32(uint16(eventId));
    AESoundManager.RequestNewSound(&m_tempSound);
}

// event eAudioEvents
// 0x4EC550
void CAEScriptAudioEntity::PlayResidentSoundEvent(eSoundBankSlot slot, eSoundBank bank, eSoundID sfx, eAudioEvents event, CVector& posn, CPhysical* physical, float vol, float speed, int16 playPosn, float maxDistance) {
    if (!AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
        return;
    }

    bool bFrontend = false;
    const auto volume = GetDefaultVolume(static_cast<eAudioEvents>(event)) + vol;
    CVector pos = [&] {
        if (physical) {
            return physical->GetPosition();
        } else if (posn == -1000.0f || posn.IsZero()) {
            bFrontend = true;
            return CVector{0.0f, 1.0f, 0.0f};
        } else {
            return posn;
        }
    }();

    AESoundManager.PlaySound({
        .BankSlotID         = slot,
        .SoundID            = sfx,
        .AudioEntity        = this,
        .Pos                = pos,
        .Volume             = volume,
        .RollOffFactor      = maxDistance,
        .Speed              = speed,
        .Doppler            = 1.0f,
        .FrameDelay         = 0,
        .Flags              = SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | (bFrontend ? SOUND_FRONT_END : 0u),
        .FrequencyVariance  = 0.0f,
        .PlayTime           = playPosn,
        .RegisterWithEntity = physical,
        .EventID            = (eAudioEvents)(event),
    });
}

// 0x4EC270
void CAEScriptAudioEntity::PlayLoadedMissionAudio(uint8 slotId) {
    CAESound sound;
    if (slotId >= MISSION_AUDIO_COUNT) {
        return;
    }
    auto& link = wavLinks[slotId];
    if (link.m_nBankId < 0 || link.m_nBankSlotId < 0 || GetMissionAudioLoadingStatus(slotId) != 1) {
        return;
    }
    auto volume = -100.f;
    bool protectedSound{};
    if (link.m_nAudioEvent != 65535) {
        volume = GetDefaultVolume(static_cast<eAudioEvents>(link.m_nAudioEvent));
        if (slotId < 2) {
            protectedSound = true;
            if (volume == -128.f) {
                volume = 6.f;
            }
        }
    }
    bool frontend{};
    CVector position;
    if (link.m_pEntity) {
        position = link.m_pEntity->GetPosition();
    } else {
        position = link.m_vPosition;
        if (position == CVector{-1000.f, -1000.f, -1000.f} || position == CVector{0.f, 0.f, 0.f}) {
            position = CVector{0.f, 1.f, 0.f};
            frontend = true;
        }
    }
    sound.Initialise(static_cast<eSoundBankSlot>(slotId + 26), static_cast<eSoundID>(uint16(link.m_nBankSlotId)), this, position, volume, 2.f, 1.f, 1.f, 0, 0, 0.f, 0);
    sound.m_Flags = 14;
    sound.SetFlags(SOUND_FRONT_END, frontend);
    sound.SetFlags(SOUND_IS_DUCKABLE, protectedSound);
    sound.SetFlags(SOUND_IS_COMPRESSABLE, protectedSound);
    sound.SetFlags(SOUND_SMOOTH_DUCKING, protectedSound);
    link.m_Sound = AESoundManager.RequestNewSound(&sound);
}

// 0x4EC190
void CAEScriptAudioEntity::PreloadMissionAudio(uint8 slotId, int32 sampleId) {
    if (slotId >= MISSION_AUDIO_COUNT || !IsMissionAudioSampleFinished(slotId)) return;
    auto& link = wavLinks[slotId];
    auto event = static_cast<eAudioEvents>(sampleId);
    if (!CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(event, reinterpret_cast<eSoundBankS32&>(link.m_nBankId), link.m_nBankSlotId, slotId)) return;
    const auto bank = static_cast<eSoundBank>(uint16(link.m_nBankId));
    const auto slot = static_cast<eSoundBankSlot>(slotId + 26);
    if (link.m_nBankSlotId < 0) AEAudioHardware.LoadSoundBank(bank, slot);
    else AEAudioHardware.LoadSound(bank, static_cast<eSoundID>(uint16(link.m_nBankSlotId)), slot);
    link.m_nAudioEvent = int32(event);
    link.m_pEntity = nullptr;
    link.m_vPosition = CVector{-1000.f, -1000.f, -1000.f};
}

// 0x4ECCF0
void CAEScriptAudioEntity::ProcessMissionAudioEvent(eAudioEvents eventId, CVector& posn, CPhysical* physical, float volume, float speed) {
    return plugin::CallMethod<0x4ECCF0, CAEScriptAudioEntity*, eAudioEvents, CVector&, CPhysical*, float, float>(this, eventId, posn, physical, volume, speed);

    /*
    * Untested
    * Need to fix case's
    switch (eventId) {
    case AE_CAS4_NE:
    case AE_CAS4_NG:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PURCHASE_WEAPON);
        break;
    case AE_CAS4_NF:
    case AE_CAS4_NH:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_CAR_NO_CASH);
        break;
    case AE_CAS4_NJ:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RACE_321);
        break;
    case AE_CAS4_OA:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RACE_GO);
        break;
    case AE_CAS4_OB:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
        break;
    case AE_CAS4_OC:
        PlayMissionBankSound(eventId, posn, physical, 5, 3u);
        break;
    case AE_CAS4_OD:
        PlayMissionBankSound(eventId, posn, physical, 2, 3);
        break;
    case AE_CAS4_OE:
    case AE_CAS4_TB:
        PlayMissionBankSound(eventId, posn, physical, 3, 3u);
        break;
    case AE_CAS4_OF:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_WEAPON_CHAINSAW_IDLE);
        break;
    case AE_CAS4_OG:
    case AE_CAS4_PA:
    case AE_CAS4_PK:
        AEAmbienceTrackManager.StopSpecialMissionAmbienceTrack();
        break;
    case AE_CAS4_OH:
        PlayMissionBankSound(eventId, posn, physical, 6, 3u);
        break;
    case AE_CAS4_OJ:
        PlayMissionBankSound(eventId, posn, physical, 0, 3);
        break;
    case AE_CAS4_OK:
    case AE_CAS4_PG:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u);
        break;
    case AE_CAS4_OL:
        PlayMissionBankSound(eventId, posn, physical, 4, 3u, 1u);
        break;
    case AE_CAS4_OM:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_WEAPON_FIRE_MINIGUN_AMMO);
        break;
    case AE_CAS4_PB:
        PlayMissionBankSound(eventId, posn, physical, 6, 3);
        break;
    case AE_CAS4_PD:
        PlayMissionBankSound(eventId, posn, physical, 4, 3u);
        break;
    case AE_CAS4_PE:
        PlayMissionBankSound(eventId, posn, physical, 2, 3u);
        break;
    case AE_CAS4_PF:
        PlayMissionBankSound(eventId, posn, physical, 5, 3);
        break;
    case AE_CAS4_PH:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u, 0, 0.0f, 2.0f, 0.79f);
        break;
    case AE_CAS4_PJ:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_THUNDER);
        break;
    case AE_CAS4_QA:
        PlayMissionBankSound(eventId, posn, physical, 3, 3, 1);
        break;
    case AE_CAS4_QB:
        PlayMissionBankSound(eventId, posn, physical, 2, 2u, 1u, 0.0f, 3.0f);
        break;
    case AE_CAS4_RA:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u, 1u, 0.0f, 3.0f);
        break;
    case AE_CAS4_RB:
        PlayMissionBankSound(eventId, posn, physical, 3, 2u, 1u, 0.0f, 3.0f);
        PlayMissionBankSound(eventId, posn, physical, 0, 2u, 0, 0.0f, 3.0f, 1.0f);
        break;
    case AE_CAS4_RD:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS4_RB, this);
        PlayMissionBankSound(eventId, posn, physical, 4, 2u, 1u, 0.0f, 3.0f);
        break;
    case AE_CAS4_RE:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SELECT);
        break;
    case AE_CAS4_SA:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BACK);
        break;
    case AE_CAS4_SB:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_ERROR);
        break;
    case AE_CAS4_TA:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u);
        m_nLastTimeHornPlayed = CTimer::GetTimeInMS();
        field_7C = 0;
        break;
    case AE_CAS4_TC:
        if (physical)
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(AE_CAS5_AA, this, physical);
        else
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS5_AA, this);
        PlayMissionBankSound(eventId, posn, physical, 2, 3u);
        break;
    case AE_CAS4_ND:
        PlayMissionBankSound(eventId, posn, physical, 10, 3u, 1u);
        break;
    case AE_CAS4_HC:
        PlayResidentSoundEvent(2, 39, 19, eventId, posn, physical, 0.0f, 1.0f, 70, 1.0f);
        break;
    case AE_CAS4_HD:
        if (AEAudioHardware.IsSoundBankLoaded(242u, 28)) {
            PlayMissionBankSound(eventId, posn, physical, 0, 2);
            break;
        }
        if (AEAudioHardware.IsSoundBankLoaded(345u, 28)) {
            PlayMissionBankSound(eventId, posn, physical, 3, 2u);
            break;
        }
        break;
    case AE_CAS4_JA:
        PlayMissionBankSound(eventId, posn, physical, 2, 2u);
        break;
    case AE_CAS4_JB:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u);
        break;
    case AE_CAS4_JC: {
        auto sfxId = (int16)CAEAudioUtility::GetRandomNumberInRange(2, 6);
        PlayMissionBankSound(eventId, posn, physical, sfxId, 3u);
        break;
    }
    case AE_CAS4_JD:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u);
        break;
    case AE_CAS4_KA:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u);
        break;
    case AE_CAS4_KB:
        if (physical)
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(AE_CAS4_KA, this, physical);
        else
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS4_KA, this);
        break;
    case AE_CAS4_KC:
        if (physical) {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENING, physical->GetPosition(), 0.0f, 1.0f);
        } else {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENING, posn, 0.0f, 1.0f);
        }
        break;
    case AE_CAS4_KD:
        if (physical) {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENED, physical->GetPosition(), 0.0f, 1.0f);
        } else {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENED, posn, 0.0f, 1.0f);
        }
        break;
    case AE_CAS4_KG:
        PlayResidentSoundEvent(5, 143, 65, eventId, posn, physical, 0.0f);
        break;
    case AE_CAS4_KJ:
        PlayMissionBankSound(eventId, posn, physical, 8, 3);
        break;
    case AE_CAS4_LA:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u, 1u);
        m_nLastTimeHornPlayed = CTimer::GetTimeInMS();
        break;
    case AE_CAS4_LB:
        PlayMissionBankSound(eventId, posn, physical, 2, 3u, 1u);
        break;
    case AE_CAS4_LC:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u, 1u);
        break;
    case AE_CAS4_LD:
        PlayMissionBankSound(eventId, posn, physical, 5, 3);
        break;
    case AE_CAS4_LE:
        PlayMissionBankSound(eventId, posn, physical, 4, 3u);
        break;
    case AE_CAS4_MA:
        PlayMissionBankSound(eventId, posn, physical, 3, 3u, 1u);
        break;
    case AE_CAS4_NA:
        PlayMissionBankSound(eventId, posn, physical, 7, 3, 1);
        break;
    case AE_CAS4_NB:
        PlayMissionBankSound(eventId, posn, physical, 6, 3u, 1u);
        break;
    case AE_CAS4_NC:
        PlayMissionBankSound(eventId, posn, physical, 9, 3u, 1u);
        break;
    case AE_CAS4_HB:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u);
        break;
    case AE_CAS4_FA:
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(eventId, this))
            break;

        PlayMissionBankSound(eventId, posn, physical, 4, 3);
        break;
    case AE_CAS4_FC:
        PlayMissionBankSound(eventId, posn, physical, 10, 3u);
        break;
    case AE_CAS4_FE:
        PlayMissionBankSound(eventId, posn, physical, 8, 3);
        break;
    case AE_CAS4_FF: {
        auto sfxId = (int16)CAEAudioUtility::GetRandomNumberInRange(3, 5);
        PlayMissionBankSound(eventId, posn, physical, sfxId, 3u);
        break;
    }
    case AE_CAS4_FG:
        PlayMissionBankSound(eventId, posn, physical, 7, 3u);
        break;
    case AE_CAS4_FH:
        if (!AEAudioHardware.IsSoundBankLoaded(44u, 40)) {
            if (AESoundManager.AreSoundsPlayingInBankSlot(40))
                AESoundManager.CancelSoundsInBankSlot(40, 0);
            AEAudioHardware.LoadSoundBank(44, 40);
        }
        m_Entity = physical;
        field_7D = 1;
        break;
    case AE_CAS4_FJ:
        if (field_7D) {
            if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(eventId, this, physical))
                PlayResidentSoundEvent(40, 44, 1, eventId, posn, physical, 0.0f, 1.0f, 0, 2.5f);
        }
        break;
    case AE_CAS4_FK:
        if (field_7D) {
            if (physical)
                AESoundManager.CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(AE_CAS4_FJ, this, physical);
            else
                AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS4_FJ, this);
            PlayResidentSoundEvent(40, 44, 2, eventId, posn, physical, 0.0f, 1.0f, 0, 2.5f);
        }
        break;
    case AE_CAS4_FL:
        if (field_7D) {
            AESoundManager.CancelSoundsInBankSlot(40, 1);
            PlayResidentSoundEvent(40, 44, 3, eventId, posn, physical, 0.0f, 1.0f, 0, 2.5f);
            m_Entity = nullptr;
            field_7D = 0;
        }
        break;
    case AE_CAS4_HA:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u);
        break;
    case AE_CAS4_EH:
        PlayMissionBankSound(eventId, posn, physical, 3, 3u);
        break;
    case AE_CAS4_EC:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS4_EB, this);
        break;
    case AE_CAS4_DC:
        CAEVehicleAudioEntity::EnableHelicoptors();
        break;
    case AE_CAS4_DB:
        CAEVehicleAudioEntity::DisableHelicoptors();
        break;
    case AE_CAS4_DD:
        PlayResidentSoundEvent(2, 39, 64, eventId, posn, physical, 0.0f, 0.79f, 35, 1.0f);
        break;
    case AE_CAS4_ED:
        PlayMissionBankSound(eventId, posn, physical, 0, 2);
        break;
    case AE_CAS4_EE:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS4_ED, this);
        break;
    case AE_CAS4_EF: {
        auto sfxId = (int16)CAEAudioUtility::GetRandomNumberInRange(20, 28);
        PlayResidentSoundEvent(2, 39, sfxId, eventId, posn, physical, 0.0f);
        break;
    }
    case AE_CAS4_EG: {
        auto sfxId = (int16)CAEAudioUtility::GetRandomNumberInRange(0, 2);
        PlayMissionBankSound(eventId, posn, physical, sfxId, 3u);
        break;
    }
    case AE_CAS5_AA:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u);
        break;
    case AE_CAS5_AB:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u);
        break;
    case AE_CAS5_BE:
        PlayResidentSoundEvent(2, 39, 65, eventId, posn, physical, 0.0f, 1.0f, 16, 1.0f);
        break;
    case AE_CAS5_CB:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_WEAPON_FIRE);
        break;
    case AE_CAS5_CF:
        if (field_7E)
            PlayResidentSoundEvent(2, 39, 69, eventId, posn, physical, 0.0f, 0.94f, 33, 1.0f);
        else
            PlayResidentSoundEvent(2, 39, 70, eventId, posn, physical, 0.0f, 1.0f, 25, 1.0f);
        field_7E = (field_7E + 1) % 2;
        break;
    case AE_CAS5_DB:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u);
        break;
    case AE_CAS5_CG:
        PlayMissionBankSound(eventId, posn, physical, 0, 2u);
        break;
    case AE_CAS6_CB:
        PlayMissionBankSound(eventId, posn, physical, 0, 2u);
        break;
    case AE_CAS5_DA:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u);
        break;
    case AE_CAS5_EA:
        PlayMissionBankSound(eventId, posn, physical, 2, 2);
        break;
    case AE_CAS5_CE:
        if (field_7E)
            PlayResidentSoundEvent(2, 39, 69, eventId, posn, physical, 0.0f, 1.0f, 63, 1.0f);
        else
            PlayResidentSoundEvent(2, 39, 70, eventId, posn, physical, 0.0f, 1.0f, 62, 1.0f);
        field_7E = (field_7E + 1) % 2;
        break;
    case AE_CAS5_CC:
        AEAmbienceTrackManager.StopSpecialMissionAmbienceTrack();
        break;
    case AE_CAS6_AE:
    case AE_CAS6_FK:
    case AE_CAS6_GB:
    case AE_CAS6_HB:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u);
        break;
    case AE_CAS6_AA:
        if (AEAudioHardware.IsSoundBankLoaded(167u, 28)) {
            PlayMissionBankSound(eventId, posn, physical, 3, 2u);
        } else if (AEAudioHardware.IsSoundBankLoaded(345u, 28)) {
            PlayMissionBankSound(eventId, posn, physical, 4, 2u);
        }
        break;
    case AE_CAS6_AB:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u);
        PlayMissionBankSound(eventId, posn, physical, 0, 2u);
        break;
    case AE_CAS6_AC:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS6_AB, this);
        PlayMissionBankSound(eventId, posn, physical, 2, 2u);
        break;
    case AE_CAS6_AD:
    case AE_CAS6_FM:
    case AE_CAS6_FP:
        PlayMissionBankSound(eventId, posn, physical, 0, 2u);
        break;
    case AE_CAS6_BA:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u, 1u, -18.0f, 2.0f, 1.0f);
        field_8C = 1.0f;
        break;
    case AE_CAS6_BB:
    case AE_CAS6_BH:
        field_8C = 2.0f;
        break;
    case AE_CAS6_BC:
    case AE_CAS6_JK:
        PlayMissionBankSound(eventId, posn, physical, 2, 3u);
        break;
    case AE_CAS6_BD:
        PlayMissionBankSound(eventId, posn, physical, 3, 3u);
        break;
    case AE_CAS6_BE:
        PlayMissionBankSound(eventId, posn, physical, 7, 3);
        break;
    case AE_CAS6_BF:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u, 1u, -18.0, 2.0f, 1.0f);
        field_8C = 1.0;
        break;
    case AE_CAS6_BK:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u);
        break;
    case AE_CAS6_BL:
    case AE_CAS6_JJ:
        PlayMissionBankSound(eventId, posn, physical, 1, 3);
        break;
    case AE_CAS6_BM:
        PlayMissionBankSound(eventId, posn, physical, 2, 3u, 1u);
        break;
    case AE_CAS6_BN:
    case AE_CAS6_KC:
        PlayMissionBankSound(eventId, posn, physical, 4, 3u);
        break;
    case AE_CAS6_CA:
        PlayMissionBankSound(eventId, posn, physical, 5, 3, 1);
        break;
    case AE_CAS6_DA:
        PlayMissionBankSound(eventId, posn, physical, 1, 2u);
        PlayMissionBankSound(eventId, posn, physical, 0, 2u);
        break;
    case AE_CAS6_DB:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(1126, this);
        PlayMissionBankSound(eventId, posn, physical, 2, 2u);
        break;
    case AE_CAS6_EA:
        PlayMissionBankSound(eventId, posn, physical, 2, 2u);
        PlayMissionBankSound(eventId, posn, physical, 0, 2u);
        break;
    case AE_CAS6_EB:
        if (physical)
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(AE_CAS6_EA, this, physical);
        else
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS6_EA, this);
        PlayMissionBankSound(eventId, posn, physical, 3, 2u);
        break;
    case AE_CAS6_EC:
        PlayResidentSoundEvent(5, 143, 58, eventId, posn, physical, 0.0f);
        PlayResidentSoundEvent(5, 143, 40, eventId, posn, physical, 0.0f);
        break;
    case AE_CAS6_ED:
        PlayResidentSoundEvent(2, 39, 33, eventId, posn, physical, 0.0f);
        PlayResidentSoundEvent(2, 39, 50, eventId, posn, physical, 0.0f, 0.79f, 22, 1.0f);
        break;
    case AE_CAS6_EE:
        if (physical)
            AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, WEAPON_CAMERA, physical);
        else
            PlayResidentSoundEvent(5, 143, 45, eventId, posn, 0, 0.0f);
        break;
    case AE_CAS6_EF:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_BUY_CAR_MOD);
        break;
    case AE_CAS6_EG:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_CAR_RESPRAY);
        break;
    case AE_CAS6_EH:
        PlayResidentSoundEvent(5, 143, 34, eventId, posn, physical, 5.0f);
        PlayResidentSoundEvent(5, 143, 40, eventId, posn, physical, 0.0f);
        break;
    case AE_CAS6_EJ:
        PlayResidentSoundEvent(5, 143, 82, eventId, posn, physical, -3.0f);
        break;
    case AE_CAS6_EK:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 0.0f, 1.12f);
        break;
    case AE_CAS6_EL:
    case AE_CAS6_FC:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);
        break;
    case AE_CAS6_EM:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 0.0f, 1.26f);
        break;
    case AE_CAS6_EN:
    case AE_CAS6_FA: {
        auto random = (int16)CAEAudioUtility::GetRandomNumberInRange(20, 28);
        PlayResidentSoundEvent(2, 39, random, eventId, posn, physical, 0.0f);
        break;
    }
    case AE_CAS6_EO: {
        auto random = (int16)CAEAudioUtility::GetRandomNumberInRange(20, 28);
        PlayResidentSoundEvent(2, 39, random, eventId, posn, physical, 0.0f);
        PlayResidentSoundEvent(2, 39, 65, eventId, posn, physical, 0.0f);
        break;
    }
    case AE_CAS6_EP:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_GARAGE_DOOR_OPENED);
        break;
    case AE_CAS6_EQ:
    case AE_CAS6_LA:
    case AE_CAS6_LC:
        AEAmbienceTrackManager.StopSpecialMissionAmbienceTrack();
        break;
    case AE_CAS6_ER:
        if (physical)
            AudioEngine.ReportWaterSplash(physical, -6.0f, 0);
        else
            AudioEngine.ReportWaterSplash(posn, -6.0f);
        break;
    case AE_CAS6_ES:
        PlayResidentSoundEvent(2, 39, 19, eventId, posn, physical, 0.0f, 1.0f, 65, 1.0f);
        break;
    case AE_CAS6_EU:
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(eventId, this)) {
            PlayResidentSoundEvent(17, 74, 7, eventId, posn, physical, 0.0f);
            m_nLastTimeHornPlayed = CTimer::GetTimeInMS();
        }
        break;
    case AE_CAS6_FF:
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_MONEY);
        break;
    case AE_CAS6_FN:
        if (physical) {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENING, physical->GetPosition(), 0.0f, 1.0f);
        } else {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENING, posn, 0.0f, 1.0f);
        }
        break;
    case AE_CAS6_FO:
        if (physical) {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENED, physical->GetPosition(), 0.0f, 1.0f);
        } else {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENED, posn, 0.0f, 1.0f);
        }
        break;
    case AE_CAS6_GH:
        if (physical)
            AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, WEAPON_M4, physical);
        break;
    case AE_CAS6_GJ:
        PlayMissionBankSound(eventId, posn, physical, 3, 3u);
        break;
    case AE_CAS6_GK:
        if (physical) {
            m_ExplosionAudio.AddAudioEvent(AE_EXPLOSION, physical->GetPosition(), 0.0f);
        } else {
            m_ExplosionAudio.AddAudioEvent(AE_EXPLOSION, posn, 0.0f);
        }
        break;
    case AE_CAS6_HC:
        if (physical)
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntityAndPhysical(AE_CAS6_HB, this, physical);
        else
            AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_CAS6_HB, this);
        break;
    case AE_CAS6_HD:
        PlayResidentSoundEvent(AE_FRONTEND_FIRE_FAIL_SNIPERRIFFLE, 0, 0, eventId, posn, physical, 0.0f);
        break;
    case AE_CAS6_JA: {
        CVector vec0 = {-1.0f, 0.0f, 0.0f};
        PlayMissionBankSound(eventId, vec0, nullptr, 0, 3u);
        CVector vec1 = {+1.0f, 0.0f, 0.0f};
        PlayMissionBankSound(eventId, vec1, nullptr, 1, 3u);
        break;
    }
    case AE_CAS6_JB:
        if (physical) {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENING, physical->GetPosition(), 0.0f, 0.79f);
        } else {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENING, posn, 0.0f, 0.79f);
        }
        break;
    case AE_CAS6_JC:
        if (physical) {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENED, physical->GetPosition(), 0.0f, 1.0f);
        } else {
            m_GarageAudio.AddAudioEvent(AE_GARAGE_DOOR_OPENED, posn, 0.0f, 1.0f);
        }
        break;
    case AE_CAS6_JD:
    case AE_CAS6_KA:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u, 1u);
        m_nLastTimeHornPlayed = CTimer::GetTimeInMS();
        break;
    case AE_CAS6_JE:
        PlayMissionBankSound(eventId, posn, physical, 0, 3u);
        break;
    case AE_CAS6_JF:
        PlayMissionBankSound(eventId, posn, physical, 1, 3u);
        break;
    case AE_CAS6_JG:
        PlayMissionBankSound(eventId, posn, physical, 6, 3u);
        break;
    case AE_CAS6_JL:
        PlayMissionBankSound(eventId, posn, physical, 5, 3u, 0, 0.0f, 2.0f, 0.67f);
        break;
    case AE_CAS6_JM:
        PlayMissionBankSound(eventId, posn, physical, 5, 3, 0, 0.0f, 2.0f, 0.79f);
        break;
    case AE_CAS6_JN:
        PlayMissionBankSound(eventId, posn, physical, 5, 3u);
        break;
    case AE_CAS6_JO:
        PlayMissionBankSound(eventId, posn, physical, 8, 3u, 1u, 0.0f, 2.0f, 0.38f);
        break;
    case AE_CAS6_KB:
        PlayMissionBankSound(eventId, posn, physical, 3, 3, 1);
        break;
    case AE_CAS6_KD:
        PlayMissionBankSound(eventId, posn, physical, 7, 3u);
        break;
    case AE_CAS6_KE:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_BLAST_DOOR_SLIDE_START);
        break;
    case AE_CAS6_LB:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_WEAPON_FIRE_PLANE);
        break;
    case AE_CAS6_LD:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_RAIN_COLLISION);
        break;
    case AE_CAS9_AA:
    case AE_CAS9_AC:
        AEAmbienceTrackManager.StopSpecialMissionAmbienceTrack();
        break;
    case AE_CAS9_AB:
        AEAmbienceTrackManager.PlaySpecialMissionAmbienceTrack(AE_GARAGE_DOOR_CLOSING);
        break;
    case AE_CAS9_AD:
        if (CLocalisation::Blood() && physical && physical->GetIsTypePed()) {
            physical->AsPed()->GetAE().AddAudioEvent(AE_PED_CRUNCH, 0.0f, 1.0f, physical, 0, 0, 0);
        }
        break;
    case AE_CAS9_BA: {
        auto sfxId = (int16)CAEAudioUtility::GetRandomNumberInRange(78, 80);
        PlayResidentSoundEvent(5, 143, sfxId, eventId, posn, physical, 0.0f);
        break;
    }
    case AE_CRANE_WINCH_MOVE:
        if (!field_7D) {
            break;
        }
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(eventId, this, physical)) {
            PlayResidentSoundEvent(40, 44, 1, eventId, posn, physical, volume, speed, 0, 2.5f);
        }
        m_Speed = speed;
        m_Volume = GetDefaultVolume(AE_CRANE_WINCH_MOVE) + volume;
        m_nLastTimeHornPlayed = CTimer::GetTimeInMS();
        break;
    default:
        return;
    }
    */
}

// 0x4EE960
void CAEScriptAudioEntity::ReportMissionAudioEvent(eAudioEvents eventId, CPhysical* physical, float volume, float speed) {
    CVector posn{ -1000.0f, -1000.0f, -1000.0f };
    ProcessMissionAudioEvent(eventId, posn, physical, volume, speed);
}

// 0x4EE940
void CAEScriptAudioEntity::ReportMissionAudioEvent(eAudioEvents eventId, CVector& posn) {
    ProcessMissionAudioEvent(eventId, posn, nullptr);
}

// 0x4EC970
void CAEScriptAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (!sound) return;
    CVector sentinel{-1000.f, -1000.f, -1000.f};
    for (auto& link : wavLinks) {
        if (link.m_Sound == sound) {
            if (curPlayPos == -1) {
                link.m_Sound = nullptr;
                return;
            }
            if (link.m_pEntity) sound->SetPosition(link.m_pEntity->GetPosition());
            continue;
        }
        const auto now = CTimer::GetTimeInMS();
        switch (sound->m_Event) {
        case 104:
            if (now > m_nLastTimeHornPlayed + 300u) {
                sound->StopSoundAndForget();
                m_nLastTimeHornPlayed = 0;
                PlayResidentSoundEvent(static_cast<eSoundBankSlot>(40), static_cast<eSoundBank>(44), static_cast<eSoundID>(2), static_cast<eAudioEvents>(1021), sound->m_CurrPos, reinterpret_cast<CPhysical*>(sound->m_AudioEntity), -12.f, 1.f, 0, 2.5f);
            } else {
                sound->m_Volume = m_Volume;
                sound->m_Speed = m_Speed;
            }
            break;
        case 1086:
            if (now > m_nLastTimeHornPlayed + 4500u) {
                if (sound->m_Volume > StaticRef<float>(0x85A708)) sound->m_Volume -= StaticRef<float>(0x858B1C);
                else {
                    sound->StopSoundAndForget();
                    m_nLastTimeHornPlayed = 0;
                }
                if (sound->m_Speed > 0.f) {
                    auto speed = sound->m_Speed - StaticRef<float>(0x858CDC);
                    if (0.f > speed) speed = 0.f;
                    sound->m_Speed = speed;
                }
            }
            if (now > m_nLastTimeHornPlayed + 4800u && !field_7C) {
                const auto sfx = int16(CAEAudioUtility::GetRandomNumberInRange<int32>(1, 3));
                PlayMissionBankSound(static_cast<eAudioEvents>(1160), sentinel, nullptr, sfx, 3, 0, 0.f, 2.f, 1.f);
                field_7C = 1;
            }
            break;
        case 1113:
        case 1118: {
            auto volume = GetDefaultVolume(static_cast<eAudioEvents>(sound->m_Event));
            if (std::bit_cast<uint32>(field_8C) == 0x3F800000u) {
                if (sound->m_Volume < volume) {
                    const auto increased = sound->m_Volume + StaticRef<float>(0x858B1C);
                    sound->m_Volume = increased < volume ? increased : volume;
                }
            } else if (std::bit_cast<uint32>(field_8C) == 0x40000000u) {
                volume -= StaticRef<float>(0x859008);
                if (sound->m_Volume > volume) {
                    const auto reduced = sound->m_Volume - StaticRef<float>(0x858B1C);
                    sound->m_Volume = reduced > volume ? reduced : volume;
                } else sound->StopSoundAndForget();
            }
            break;
        }
        case 1042:
        case 1167:
        case 1177:
            if (now > m_nLastTimeHornPlayed + 300u) {
                sound->StopSoundAndForget();
                m_nLastTimeHornPlayed = 0;
            }
            break;
        case 1147:
            if (now > m_nLastTimeHornPlayed + 500u) {
                sound->StopSoundAndForget();
                m_nLastTimeHornPlayed = 0;
            }
            break;
        }
    }
}

// 0x4EC900
void CAEScriptAudioEntity::Service() {
    CVector posn = {-1000.0f, -1000.0f, -1000.0f};
    if (!m_Physical)
        return;
    if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SCRIPT_CRANE_ENTER, this) != 0)
        return;

    PlayResidentSoundEvent(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_BANK_GENRL_CRANE_P, 0, AE_SCRIPT_CRANE_ENTER, posn, m_Physical, 0.0f, 1.0f, 0, 2.5f);
}

void CAEScriptAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEScriptAudioEntity, 0x862E58, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x5074D0);
    RH_ScopedInstall(Initialise, 0x5B9B60);
    RH_ScopedInstall(Service, 0x4EC900);
    RH_ScopedInstall(Reset, 0x4EC150);
    RH_ScopedInstall(GetMissionAudioLoadingStatus, 0x4EBF60);
    RH_ScopedInstall(IsMissionAudioSampleFinished, 0x4EBFE0);
    RH_ScopedInstall(GetMissionAudioEvent, 0x4EC020);
    RH_ScopedInstall(ClearMissionAudio, 0x4EC040);
    RH_ScopedInstall(SetMissionAudioPosition, 0x4EC0C0);
    RH_ScopedInstall(AttachMissionAudioToPhysical, 0x4EC100);
    RH_ScopedInstall(PreloadMissionAudio, 0x4EC190, { .reversed = false });
    RH_ScopedInstall(PlayLoadedMissionAudio, 0x4EC270, { .reversed = false });
    RH_ScopedInstall(GetMissionAudioPosition, 0x4EC4D0);
    RH_ScopedInstall(PlayResidentSoundEvent, 0x4EC550);
    RH_ScopedInstall(PlayMissionBankSound, 0x4EC6D0, { .reversed = false });
    RH_ScopedInstall(ProcessMissionAudioEvent, 0x4ECCF0, { .reversed = false });
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "1", 0x4EE960, void (CAEScriptAudioEntity::*)(eAudioEvents, CPhysical*, float, float));
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "2", 0x4EE940, void (CAEScriptAudioEntity::*)(eAudioEvents, CVector&));
    RH_ScopedVMTInstall(UpdateParameters, 0x4EC970, { .reversed = false });
}

CAEScriptAudioEntity* CAEScriptAudioEntity::Constructor() {
    this->CAEScriptAudioEntity::CAEScriptAudioEntity();
    return this;
}





