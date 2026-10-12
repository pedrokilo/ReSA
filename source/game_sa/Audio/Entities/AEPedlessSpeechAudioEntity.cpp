/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEPedlessSpeechAudioEntity
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEPedlessSpeechAudioEntity.h"
#include "X87Math.h"
#include "AEAudioUtility.h"
#include "AEVehicleAudioEntity.h"
#include "AESoundManager.h"


void CAEPedlessSpeechAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPedlessSpeechAudioEntity, 0x85F330, 8);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x4E6070);
    RH_ScopedInstall(Initialise, 0x4E4E10);
    RH_ScopedInstall(StopCurrentSpeech, 0x4E4E80);
    RH_ScopedInstall(AddSayEvent, 0x4E60D0);
    RH_ScopedVMTInstall(IsPedFemaleForAudio, 0x4E60C0);
    RH_ScopedVMTInstall(GetPedType, 0x4E60B0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E4D10);
    RH_ScopedVMTInstall(Terminate, 0x4E6300);
    RH_ScopedVMTInstall(PlayLoadedSound, 0x4E6380);
    RH_ScopedVMTInstall(GetAllocatedVoice, 0x4E6090);
    RH_ScopedVMTInstall(WillPedChatAboutTopic, 0x4E60A0);
}

// Como el original: reinicia un hueco de voz campo a campo (sin tocar el relleno de la estructura).
void CAEPedlessSpeechAudioEntity::ResetSpeechSlot(CAEPedSpeechSlot& speech) {
    speech.Status                          = CAEPedSpeechSlot::eStatus::FREE;
    speech.AudioEntity                     = nullptr;
    speech.Sound                           = nullptr;
    speech.SoundID                         = -1;
    speech.SoundBankID                     = SND_BANK_UNK;
    speech.StartPlaybackTime               = 0;
    speech.GCtx                            = CTX_GLOBAL_UNK;
    speech.PedAudioType                    = PED_TYPE_UNK;
    speech.IsReservedForPedConversation    = false;
    speech.IsReservedForPlayerConversation = false;
    speech.ForceAudible                    = false;
}

// 0x4E4E10
void CAEPedlessSpeechAudioEntity::Initialise() {
    // Como el original: solo escribe estos campos; no toca la vtable, m_tempSound, m_Sounds, la voz,
    // el sonido, el banco ni el hueco, y pone a nulo la entidad asociada sin CleanUpOldReference.
    m_Entity      = nullptr;
    AttachedRaw() = nullptr;
    m_Sound       = nullptr;
    m_EventVolume = -100.f;
    m_LastGCtx    = CTX_GLOBAL_UNK;
    m_NextTimeCanSayPain.fill(0);
    m_IsPlayingSpeech            = false;
    m_IsSpeechDisabled           = false;
    m_IsSpeechForScriptsDisabled = false;
    m_IsFrontend                 = false;
    m_IsForcedAudible            = false;
    m_IsFemale                   = 0;
    m_PedAudioType               = PED_TYPE_GFD;
    m_IsInitialized              = true;
}

// 0x4E4E80
void CAEPedlessSpeechAudioEntity::StopCurrentSpeech() {
    if (!m_IsInitialized || !m_IsPlayingSpeech) {
        return;
    }
    // Como el original: indexa el hueco directamente con el identificador con signo.
    auto& speech = s_PedSpeechSlots.data()[m_PedSpeechSlotID];
    if (speech.Status == CAEPedSpeechSlot::eStatus::PLAYING) {
        if (m_Sound) {
            m_Sound->StopSoundAndForget();
        }
        m_Sound = nullptr;
    }
    ResetSpeechSlot(s_PedSpeechSlots.data()[m_PedSpeechSlotID]);
    // Como el original: deja de hablar y libera el hueco.
    m_IsPlayingSpeech = false;
    m_PedSpeechSlotID = -1;
}

// 0x4E60D0
int16 CAEPedlessSpeechAudioEntity::AddSayEvent(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, CEntity* attachTo, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    if (!m_IsInitialized || !attachTo || s_bAllSpeechDisabled || m_IsSpeechForScriptsDisabled) {
        return -1;
    }
    if (TheCamera.m_bWideScreenOn && !IsGlobalContextImportantForWidescreen(gCtx)) {
        return -1;
    }
    if (CGameLogic::GameState == eGameState::GAME_STATE_PLAYING_LOGO) {
        return -1;
    }
    if (!CAEAudioUtility::ResolveProbability(probability) || audioEvent != AE_SPEECH_PED) {
        return -1;
    }
    if (m_IsSpeechDisabled && !overrideSilence) {
        return -1;
    }
    if (m_IsPlayingSpeech) {
        return -1;
    }
    // Como el original: compara los 16 bits bajos con signo (movsx).
    const auto ctx = (int16)(gCtx);
    if (ctx < CTX_GLOBAL_POLICE_BOAT || ctx > CTX_GLOBAL_POLICE_OVERBOARD) {
        return -1;
    }
    // Como el original: fija tipo GFD y voz 10 antes de seguir comprobando.
    m_PedAudioType = PED_TYPE_GFD;
    m_VoiceID      = (ePedSpeechVoiceS16)(10);

    // Como el original: todo el código de voz lee 0xB6B98C (s_pPlayerAttachedForRadio); 0xB6B99C (s_pVehicleAudioSettingsForRadio) sólo lo lee el audio de vehículos.
    if (IsGlobalContextImportantForStreaming(gCtx) || isForceAudible || isFrontEnd) {
        if (CStreaming::ms_numModelsRequested > 15 && CAEVehicleAudioEntity::s_pPlayerAttachedForRadio) {
            return -1;
        }
    } else if (CStreaming::IsVeryBusy()) {
        return -1;
    }

    // Como el original: (entidad - cámara) al cuadrado frente a 3600 (0x8599B8); a 60 m exactos o con NaN sigue.
    // El EXE suma el cuadrado como (z² + y²) + x² (CVector::SquaredMagnitude, 0x406DA0) sin redondear, y no como
    // x² + y² + z²: en casos límite cambia el resultado (X87Math.cpp).
    const CVector toCamera = attachTo->GetPosition() - TheCamera.GetPosition();
    if (VectorSquaredMagnitudeAboveCpp(&toCamera.x, 3600.f)) {
        return -1;
    }

    // Como el original: primero el hueco (0..4) y después el sonido.
    const auto speechSlotID = CanWePlayGlobalSpeechContext(gCtx);
    if (speechSlotID < 0 || speechSlotID >= 5) {
        return -1;
    }
    eSpecificSpeechContext sCtx;
    const auto soundID = GetSoundAndBankIDs(gCtx, sCtx);
    if (soundID < 0) {
        return -1;
    }

    // Como el original: la referencia a la entidad solo se cambia cuando la frase sale adelante.
    m_AttachedTo = attachTo;
    SetNextPlayTime(gCtx);

    m_EventVolume     = GetDefaultVolume(AE_SPEECH_PED);
    m_IsFrontend      = isFrontEnd;
    m_IsForcedAudible = isForceAudible;
    m_PedSpeechSlotID = speechSlotID;
    m_IsPlayingSpeech = true;
    m_LastGCtx        = gCtx;
    LoadAndPlaySpeech(startTimeDelayMs);

    return soundID;
}

// 0x4E4D10
void CAEPedlessSpeechAudioEntity::UpdateParameters(CAESound* sound, int16 playTime) {
    auto& speech = s_PedSpeechSlots.data()[m_PedSpeechSlotID];
    if (playTime == -1) {
        m_IsPlayingSpeech = false;
        ResetSpeechSlot(speech);
        m_Sound           = nullptr;
        m_SoundID         = -1;
        m_BankID          = SND_BANK_UNK;
        m_PedSpeechSlotID = -1;
        m_IsForcedAudible = false;
        return;
    }
    speech.Status = CAEPedSpeechSlot::eStatus::PLAYING;
    // Como el original: solo sonidos no frontales del evento de voz, y con la posición de la entidad asociada.
    if (!m_IsFrontend && sound->m_Event == AE_SPEECH_PED) {
        if (CEntity* const attached = m_AttachedTo) {
            sound->SetPosition(attached->GetPosition());
        }
    }
}

// 0x4E6300
void CAEPedlessSpeechAudioEntity::Terminate() {
    StopCurrentSpeech();
    // Como el original: solo reinicia estos campos (no toca frontal/audible, contexto, dolor ni m_tempSound).
    m_Entity                     = nullptr;
    m_IsInitialized              = false;
    m_PedAudioType               = PED_TYPE_UNK;
    m_VoiceID                    = VOICE_UNK;
    m_IsPlayingSpeech            = false;
    m_IsSpeechDisabled           = false;
    m_IsSpeechForScriptsDisabled = false;
    m_Sound                      = nullptr;
    m_SoundID                    = -1;
    m_BankID                     = SND_BANK_UNK;
    m_PedSpeechSlotID            = -1;
    m_EventVolume                = -100.f;
    m_AttachedTo                 = nullptr;
}

// 0x4E6380
void CAEPedlessSpeechAudioEntity::PlayLoadedSound() {
    CEntity* const attached = m_AttachedTo;
    if (!attached) {
        StopCurrentSpeech();
        return;
    }
    // Como el original: prepara m_tempSound (no una CAESound local) y la registra con la entidad asociada.
    const auto bankSlot = (eSoundBankSlot)((uint16)(m_PedSpeechSlotID + SND_BANK_SLOT_SPEECH1));
    if (m_IsFrontend) {
        m_tempSound.Initialise(bankSlot, m_SoundID, this, CVector{ 0.f, 1.f, 0.f }, 3.f, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        m_tempSound.m_Flags = 0x387; // comprimible, atenuable, actualizaciones, cancelable y frontal
    } else {
        m_tempSound.Initialise(bankSlot, m_SoundID, this, attached->GetPosition(), 12.f, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        m_tempSound.m_Flags         = 0x86; // actualizaciones y cancelable
        m_tempSound.m_RollOffFactor = 3.5f;
    }
    if (m_IsForcedAudible) {
        m_tempSound.m_Volume = 3.f;
    }
    m_tempSound.RegisterWithPhysicalEntity(attached);
    m_AttachedTo = nullptr;

    m_Sound = AESoundManager.RequestNewSound(&m_tempSound);
    if (m_Sound) {
        s_PedSpeechSlots.data()[m_PedSpeechSlotID].Sound  = m_Sound;
        s_PedSpeechSlots.data()[m_PedSpeechSlotID].Status = CAEPedSpeechSlot::eStatus::PLAYING;
    } else {
        ResetSpeechSlot(s_PedSpeechSlots.data()[m_PedSpeechSlotID]);
        m_IsPlayingSpeech = false;
        // Como el original: comprueba el tipo jugador también aquí.
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            s_bAPlayerSpeaking = false;
        }
        m_PedSpeechSlotID = -1;
    }
}

CAEPedlessSpeechAudioEntity* CAEPedlessSpeechAudioEntity::Constructor() {
    // Como el original (0x4E6070 + 0x4E4F10): solo escribe estos campos y la vtable del EXE; el constructor C++
    // inicializaría por valor m_tempSound, m_Sounds, el dolor y la entidad asociada, que el EXE deja sin tocar.
    auto* const base = reinterpret_cast<uint8*>(this);
    *reinterpret_cast<uintptr_t*>(base) = 0x85F330;
    m_Entity = nullptr;
    *reinterpret_cast<uint32*>(base + 0x10) = 0; // m_tempSound.m_PhysicalEntity
    m_IsInitialized              = false;
    m_PedAudioType               = PED_TYPE_GEN;
    m_IsPlayingSpeech            = false;
    m_IsSpeechDisabled           = false;
    m_IsSpeechForScriptsDisabled = false;
    m_Sound                      = nullptr;
    m_SoundID                    = -1;
    m_BankID                     = SND_BANK_UNK;
    m_PedSpeechSlotID            = -1;
    m_EventVolume                = -100.f;
    m_LastGCtx                   = CTX_GLOBAL_UNK;
    return this;
}

CAEPedlessSpeechAudioEntity* CAEPedlessSpeechAudioEntity::Destructor() {
    this->CAEPedlessSpeechAudioEntity::~CAEPedlessSpeechAudioEntity();
    return this;
}
