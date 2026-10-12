/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEPedSpeechAudioEntity
 * Reconstrucción: COMPLETA — 58/58 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 58/58 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include <bitset>
#include "AEPedSpeechAudioEntity.h"
#include "AEPedlessSpeechAudioEntity.h"
#include "AEAudioUtility.h"
#include "PedClothesDesc.h"
#include <AEAudioHardware.h>
#include <Audio/Enums/eSoundBankSlot.h>
#include "PedGroups.h"
#include "Stats.h"

#include "PedSpeechAudioEntityLUTs.inc.h"

void CAEPedSpeechAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPedSpeechAudioEntity, 0x85F310, 8);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x4E4F10);
    RH_ScopedInstall(IsGlobalContextImportantForInterupting, 0x4E4600);
    RH_ScopedInstall(IsGlobalContextUberImportant, 0x4E46F0);
    RH_ScopedInstall(GetNextMoodToUse, 0x4E4700);
    RH_ScopedInstall(GetVoiceForMood, 0x4E4760);
    RH_ScopedInstall(CanWePlayScriptedSpeech, 0x4E4950);
    RH_ScopedInstall(GetSpeechContextVolumeOffset, 0x4E4AE0);
    RH_ScopedInstall(RequestPedConversation, 0x4E50E0);
    RH_ScopedInstall(ReleasePedConversation, 0x4E52A0);
    RH_ScopedInstall(GetCurrentCJMood, 0x4E53B0);
    RH_ScopedInstall(StaticInitialise, 0x5B98C0);
    RH_ScopedInstall(GetSpecificSpeechContext, 0x4E4470);
    RH_ScopedInstall(Service, 0x4E3710);
    RH_ScopedInstall(Reset, 0x4E37B0);
    RH_ScopedInstall(ReservePedConversationSpeechSlots, 0x4E37F0);
    RH_ScopedInstall(ReservePlayerConversationSpeechSlot, 0x4E3870);
    RH_ScopedInstall(RequestPlayerConversation, 0x4E38C0);
    RH_ScopedInstall(ReleasePlayerConversation, 0x4E3960);
    RH_ScopedInstall(SetUpConversation, 0x4E3A00);
    RH_ScopedInstall(GetAudioPedType, 0x4E3C60);
    RH_ScopedInstall(GetVoice, 0x4E3CD0);
    RH_ScopedInstall(DisableAllPedSpeech, 0x4E3EB0);
    RH_ScopedInstall(IsGlobalContextPain, 0x4E44F0);
    RH_ScopedInstall(SetCJMood, 0x4E3ED0);
    RH_ScopedInstall(EnableAllPedSpeech, 0x4E3EC0);
    RH_ScopedInstall(IsCJDressedInForGangSpeech, 0x4E4270);
    RH_ScopedInstall(GetSexForSpecialPed, 0x4E4260);
    RH_ScopedInstall(IsGlobalContextImportantForWidescreen, 0x4E46B0);
    RH_ScopedInstall(GetRepeatTime, 0x4E47E0);
    RH_ScopedInstall(LoadAndPlaySpeech, 0x4E4840);
    RH_ScopedInstall(GetNumSlotsPlayingContext, 0x4E49B0);
    RH_ScopedInstall(GetNextPlayTime, 0x4E49E0);
    RH_ScopedInstall(SetNextPlayTime, 0x4E4A20);
    RH_ScopedInstall(DisablePedSpeech, 0x4E56D0);
    RH_ScopedInstall(DisablePedSpeechForScriptSpeech, 0x4E5700);
    RH_ScopedInstall(CanPedSayGlobalContext, 0x4E5730);
    RH_ScopedInstall(GetVoiceAndTypeFromModel, 0x4E58C0);
    RH_ScopedInstall(GetSoundAndBankIDs, 0x4E5920);
    RH_ScopedInstall(CanWePlayGlobalSpeechContext, 0x4E5F10);
    RH_ScopedInstall(AddSayEvent, 0x4E6550);
    RH_ScopedInstall(Initialise, 0x4E68D0);
    RH_ScopedInstall(CanPedHoldConversation, 0x4E69E0);
    RH_ScopedInstall(IsGlobalContextImportantForStreaming, 0x4E4510);
    RH_ScopedInstall(EnablePedSpeech, 0x4E3F70);
    RH_ScopedInstall(EnablePedSpeechForScriptSpeech, 0x4E3F90);
    RH_ScopedInstall(StopCurrentSpeech, 0x4E3FB0);
    RH_ScopedInstall(GetSoundAndBankIDsForScriptedSpeech, 0x4E4400);
    RH_ScopedInstall(GetSexFromModel, 0x4E4200);
    RH_ScopedInstall(GetPedTalking, 0x4E3F50);
    RH_ScopedInstall(GetVoiceAndTypeForSpecialPed, 0x4E4170);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E3520);
    RH_ScopedVMTInstall(AddScriptSayEvent, 0x4E4F70);
    RH_ScopedVMTInstall(Terminate, 0x4E5670);
    RH_ScopedVMTInstall(PlayLoadedSound, 0x4E5CD0);
    RH_ScopedVMTInstall(GetAllocatedVoice, 0x4E4120);
    RH_ScopedVMTInstall(WillPedChatAboutTopic, 0x4E5800);
    RH_ScopedVMTInstall(GetPedType, 0x4E4130);
    RH_ScopedVMTInstall(IsPedFemaleForAudio, 0x4E4150);
}

namespace {
// Como el original: las búsquedas usan las tablas de datos del EXE tal cual (mismo formato, mismos centinelas y
// mismos accesos fuera de rango que el código original).
constexpr uintptr_t EXE_SPEECH_CONTEXT_TABLE = 0x8C6A68; // entradas de 16 bytes: GCtx, 5 contextos específicos, RepeatTime, 0; termina en GCtx == -1
constexpr uintptr_t EXE_SPECIAL_PED_NAMES    = 0x8C65C0; // 45 nombres de 20 bytes
constexpr uintptr_t EXE_SPECIAL_PED_VOICES   = 0x8C6948; // 45 entradas de 6 bytes: voz, tipo, mujer
constexpr int16     NUM_EXE_SPECIAL_PEDS     = 45;

const int16* ExeSpeechContextEntry(int16 idx) {
    return reinterpret_cast<const int16*>(EXE_SPEECH_CONTEXT_TABLE + (uintptr_t)(int32)idx * 16);
}

// Como el original: búsqueda lineal hasta el centinela; devuelve el índice de la entrada (el del centinela si no está).
int16 FindExeSpeechContext(int16 gCtx) {
    int16 idx = 0;
    for (int16 ctx = *ExeSpeechContextEntry(0); ctx != -1 && ctx != gCtx; ctx = *ExeSpeechContextEntry(++idx)) {
    }
    return idx;
}

// Tablas por tipo de audio: banco por voz (int16), y rangos de sonidos por [contexto específico][voz].
struct tExeSpeechTypeTables {
    uintptr_t Banks;
    uintptr_t Ranges;
    int32     NumVoices;
    int16     MaxSpecificContext;
};
constexpr tExeSpeechTypeTables EXE_GEN_TABLES{ 0x8AF700, 0x8AF8A8, 0xD1, 0x67 }; // rangos de 2 bytes (uint8, uint8)
constexpr tExeSpeechTypeTables EXE_EMG_TABLES{ 0x8BA470, 0x8BA4D0, 0x2E, 0x22 };
constexpr tExeSpeechTypeTables EXE_PLY_TABLES{ 0x8BBED0, 0x8BBEF8, 0x14, 0x6F };
constexpr tExeSpeechTypeTables EXE_GNG_TABLES{ 0x8BE5B8, 0x8BE620, 0x34, 0x70 };
constexpr tExeSpeechTypeTables EXE_GFD_TABLES{ 0x8C4288, 0x8C42B0, 0x12, 0x78 };
constexpr tExeSpeechTypeTables EXE_PAIN_TABLES{ 0x8C64AC, 0x8C64B8, 0x03, 0x7FFF };

int16 ExeVoiceBank(const tExeSpeechTypeTables& t, int16 voice) {
    return *reinterpret_cast<const int16*>(t.Banks + (intptr_t)voice * 2);
}

// Rango de 4 bytes (int16 primero, int16 último)
const int16* ExeRange(const tExeSpeechTypeTables& t, int16 sCtx, int16 voice) {
    return reinterpret_cast<const int16*>(t.Ranges + ((intptr_t)sCtx * t.NumVoices + voice) * 4);
}

// Rango de 2 bytes de los peatones genéricos (uint8 primero, uint8 último)
const uint8* ExeGenRange(int16 sCtx, int16 voice) {
    return reinterpret_cast<const uint8*>(EXE_GEN_TABLES.Ranges + ((intptr_t)sCtx * EXE_GEN_TABLES.NumVoices + voice) * 2);
}

// Como el original: la entidad de voz de un peatón es una dirección (CPed + 0x294); no se desreferencia el peatón.
CAEPedSpeechAudioEntity* SpeechOf(CPed* ped) {
    return reinterpret_cast<CAEPedSpeechAudioEntity*>(reinterpret_cast<uint8*>(ped) + 0x294);
}

CAEPedSpeechSlot& SpeechSlot(int32 idx) {
    return CAEPedSpeechAudioEntity::s_PedSpeechSlots.data()[idx];
}

// Como el original: tarea facial secundaria sin comprobar su tipo (solo se comprueba la inteligencia).
CTaskComplexFacial* GetFacialTask(CEntity* entity) {
    auto* const intel = static_cast<CPed*>(entity)->GetIntelligence();
    if (!intel) {
        return nullptr;
    }
    return static_cast<CTaskComplexFacial*>(static_cast<CPed*>(entity)->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
}
}; // namespace

// Como el original (0x4E3350): reinicia el hueco campo a campo, sin tocar el relleno.
void CAEPedSpeechSlot::Reset() {
    Status                          = eStatus::FREE;
    AudioEntity                     = nullptr;
    Sound                           = nullptr;
    SoundID                         = -1;
    SoundBankID                     = SND_BANK_UNK;
    StartPlaybackTime               = 0;
    GCtx                            = CTX_GLOBAL_UNK;
    PedAudioType                    = PED_TYPE_UNK;
    IsReservedForPedConversation    = false;
    IsReservedForPlayerConversation = false;
    ForceAudible                    = false;
}

CAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity(eAudioPedType pt) noexcept :
    m_PedAudioType{pt}
{
    if (pt != PED_TYPE_UNK) {
        m_IsInitialized = true;
    }
    m_NextTimeCanSayPain.fill(0);
}

// NOTSA: constructor de conveniencia (Initialise ya no lo usa)
CAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity(CPed* ped) noexcept :
    CAEPedSpeechAudioEntity{}
{
    Initialise(ped);
}

// 0x4E4F10
CAEPedSpeechAudioEntity* CAEPedSpeechAudioEntity::Constructor() {
    // Como el original: solo escribe estos campos y la vtable del EXE; el constructor C++ inicializaría por valor
    // m_tempSound, m_Sounds, la voz, el sexo, el volumen forzado, el modo 2D y los tiempos de dolor, que el EXE no toca.
    auto* const base = reinterpret_cast<uint8*>(this);
    *reinterpret_cast<uint32*>(base + 0x10) = 0; // m_tempSound.m_PhysicalEntity
    m_IsInitialized              = false;
    m_PedAudioType               = PED_TYPE_GEN;
    m_IsPlayingSpeech            = false;
    m_IsSpeechDisabled           = false;
    m_IsSpeechForScriptsDisabled = false;
    m_Entity                     = nullptr;
    m_Sound                      = nullptr;
    *reinterpret_cast<uintptr_t*>(base) = 0x85F310;
    m_SoundID                    = -1;
    m_BankID                     = SND_BANK_UNK;
    m_PedSpeechSlotID            = -1;
    m_EventVolume                = -100.f;
    m_LastGCtx                   = CTX_GLOBAL_UNK;
    return this;
}

// 0x4E4950
tPedSpeechSlotID CAEPedSpeechAudioEntity::CanWePlayScriptedSpeech() {
    return GetFreeSpeechSlot();
}

// 0x4E4AE0
float __stdcall CAEPedSpeechAudioEntity::GetSpeechContextVolumeOffset(eGlobalSpeechContextS16 gctx) {
    // Como el original: (gordura - músculo) · 0.001 se recalcula (y se piden las estadísticas) en cada comparación.
    const auto Strain = [] {
        const float fat = CStats::GetStatValue(STAT_FAT);
        return (fat - CStats::GetStatValue(STAT_MUSCLE)) * 0.001f;
    };
    // Como el original: la base se interpola desde un float estático del EXE que nadie escribe (0xB61C14 / 0xB61C18).
    const auto StrainOffset = [&](float base, uintptr_t maxAddr) {
        float t;
        if (!(Strain() > 1.f) && Strain() < -1.f) {
            t = -1.f;
        } else if (Strain() > 1.f) {
            t = 1.f;
        } else {
            t = Strain();
        }
        const float k = (t + 1.f) * 0.5f;
        return (*reinterpret_cast<const float*>(maxAddr) - base) * k + base;
    };
    switch (gctx) {
    case CTX_GLOBAL_STOMACH_RUMBLE:        return -6.f;                             // 0x8C8128
    case CTX_GLOBAL_BREATHING:             return -12.f;                            // 0x8C8130
    case CTX_GLOBAL_PAIN_CJ_STRAIN:        return StrainOffset(-18.f, 0xB61C14);    // 0x8C8120
    case CTX_GLOBAL_PAIN_CJ_STRAIN_EXHALE: return StrainOffset(-21.f, 0xB61C18);    // 0x8C8124
    case CTX_GLOBAL_PAIN_CJ_DROWNING:      return 3.f;                              // 0x8C812C
    default:                               return 0.f;
    }
}

// 0x4E50E0
bool CAEPedSpeechAudioEntity::RequestPedConversation(CPed* pedA, CPed* pedB) {
    if (s_bAllSpeechDisabled) {
        return false;
    }
    auto& speechA = pedA->GetSpeechAE();
    auto& speechB = pedB->GetSpeechAE();
    if (speechA.m_IsSpeechForScriptsDisabled || speechA.m_IsSpeechDisabled || speechB.m_IsSpeechForScriptsDisabled || speechB.m_IsSpeechDisabled) {
        return false;
    }
    if (s_bPedConversationHappening || s_bPlayerConversationHappening) {
        return false;
    }
    // Como el original: llamadas virtuales a través de la vtable de cada entidad de voz
    auto* const vA = static_cast<CAEPedSpeechAudioEntity*>(&speechA);
    auto* const vB = static_cast<CAEPedSpeechAudioEntity*>(&speechB);
    const auto voiceA = vA->GetAllocatedVoice();
    if (voiceA == vB->GetAllocatedVoice()) {
        return false;
    }
    if (!pedA->CanPedHoldConversation() || !pedB->CanPedHoldConversation()) {
        return false;
    }
    if (pedA->GetPedTalking() || pedB->GetPedTalking()) {
        return false;
    }
    // Como el original: falla solo si la distancia al cuadrado es mayor que 1600 (un NaN pasa)
    if ((pedA->GetPosition() - TheCamera.GetPosition()).SquaredMagnitude() > 1600.f) {
        return false;
    }
    if ((pedB->GetPosition() - TheCamera.GetPosition()).SquaredMagnitude() > 1600.f) {
        return false;
    }
    if (!ReservePedConversationSpeechSlots()) {
        return false;
    }

    s_bPedConversationHappening = true;
    s_pConversationPed1         = pedA;
    s_pConversationPed2         = pedB;

    SetUpConversation();

    return true;
}

// 0x4E52A0
void CAEPedSpeechAudioEntity::ReleasePedConversation() {
    if (!s_bPedConversationHappening) {
        return;
    }
    if (s_pConversationPedSlot1 >= 0 && s_pConversationPedSlot2 >= 0) {
        // Como el original: solo se para la voz si el hueco está cargando, esperando, pedido o sonando (1, 2, 3 o 5)
        const auto StopIfActive = [](tPedSpeechSlotID slot) {
            const auto status = (int32)SpeechSlot(slot).Status;
            if ((status > 0 && status <= 3) || status == 5) {
                SpeechSlot(slot).AudioEntity->StopCurrentSpeech();
            }
        };
        StopIfActive(s_pConversationPedSlot1);
        SpeechSlot(s_pConversationPedSlot1).Reset();
        StopIfActive(s_pConversationPedSlot2);
        SpeechSlot(s_pConversationPedSlot2).Reset();
        s_pConversationPedSlot1 = -1;
        s_pConversationPedSlot2 = -1;
        s_pConversationPed1     = nullptr;
        s_pConversationPed2     = nullptr;
    }
    s_bPedConversationHappening = false; // Como el original: también si algún hueco no es válido
}

// 0x5B98C0
void CAEPedSpeechAudioEntity::StaticInitialise() {
    for (auto& slot : s_PedSpeechSlots) {
        slot.Reset();
    }
    for (auto& phrase : s_PhraseMemory) {
        phrase.SoundID = -1;
        phrase.BankID  = SND_BANK_UNK;
    }
    s_NextSpeechSlot = 0;

    Reset();

    // Como el original: vuelve a sortear los tiempos de todos los contextos tras Reset()
    for (auto& t : gGlobalSpeechContextNextPlayTime) {
        const auto now = CTimer::GetTimeInMS();
        t = now + CAEAudioUtility::GetRandomNumberInRange<int32>(3000, 7000);
    }

    s_pConversationPed1            = nullptr;
    s_pConversationPed2            = nullptr;
    s_pConversationPedSlot1        = -1;
    s_pConversationPedSlot2        = -1;
    s_pPlayerConversationPed       = nullptr;
    s_bPedConversationHappening    = false;
    s_bPlayerConversationHappening = false;
    rng::fill(s_Conversation, CTX_GLOBAL_UNK);

    s_nCJBasicMood   = MOOD_UNK;
    s_nCJGangBanging = -1;
    s_nCJFat         = -1;
    s_nCJWellDressed = -1;

    s_NextSpeechSlot      = 0;
    s_bAllSpeechDisabled  = false;
    s_bAPlayerSpeaking    = false;
    s_bForceAudible       = false;
    s_nCJMoodOverrideTime = 0;
}

// 0x4E3710
void CAEPedSpeechAudioEntity::Service() { // static
    bool forceAudible = false; // Como el original: s_bForceAudible solo se escribe al final
    for (int16 i = 0; i < (int16)s_PedSpeechSlots.size(); i++) {
        auto& speech = SpeechSlot(i);

        // Waiting for sound to load, and has loaded?
        bool waiting = speech.Status == CAEPedSpeechSlot::eStatus::WAITING;
        if (speech.Status == CAEPedSpeechSlot::eStatus::LOADING) {
            if (AEAudioHardware.IsSoundLoaded(speech.SoundBankID, speech.SoundID, (eSoundBankSlot)(SND_BANK_SLOT_SPEECH1 + i))) {
                speech.Status = CAEPedSpeechSlot::eStatus::WAITING;
                waiting       = true;
            }
        }

        // Sound is now loaded, waiting to be played
        if (waiting && speech.StartPlaybackTime <= CTimer::GetTimeInMS()) {
            if (auto* const ae = speech.AudioEntity) {
                ae->PlayLoadedSound();
            } else {
                speech.Status = CAEPedSpeechSlot::eStatus::FREE;
                continue;
            }
        }

        // `PlayLoadedSound` above might've modified the status, must check it again
        switch (speech.Status) {
        case CAEPedSpeechSlot::eStatus::REQUESTED:
        case CAEPedSpeechSlot::eStatus::PLAYING: {
            if (speech.ForceAudible) {
                forceAudible = true;
            }
        }
        }
    }
    s_bForceAudible = forceAudible;
}

// 0x4E37B0
void CAEPedSpeechAudioEntity::Reset() {
    for (auto& t : gGlobalSpeechContextNextPlayTime) {
        const auto now = CTimer::GetTimeInMS();
        t = now + CAEAudioUtility::GetRandomNumberInRange<int32>(3000, 7000);
    }
    s_nCJMoodOverrideTime = 0;
}

// 0x4E37F0
bool CAEPedSpeechAudioEntity::ReservePedConversationSpeechSlots() {
    // Como el original: los dos primeros huecos libres (0..4), sin usar s_NextSpeechSlot
    tPedSpeechSlotID slotA = -1, slotB = -1;
    for (tPedSpeechSlotID i = 0; i < PLAYER_SPEECH_SLOT; i++) {
        if (SpeechSlot(i).Status != CAEPedSpeechSlot::eStatus::FREE) {
            continue;
        }
        if (slotA == -1) {
            slotA = i;
        } else {
            slotB = i;
            break;
        }
    }
    if (slotB == -1) {
        return false;
    }

    SpeechSlot(slotA).IsReservedForPedConversation = true;
    SpeechSlot(slotB).IsReservedForPedConversation = true;
    SpeechSlot(slotA).Status = CAEPedSpeechSlot::eStatus::RESERVED;
    SpeechSlot(slotB).Status = CAEPedSpeechSlot::eStatus::RESERVED;
    s_pConversationPedSlot1  = slotA;
    s_pConversationPedSlot2  = slotB;

    return true;
}

// 0x4E3870
bool CAEPedSpeechAudioEntity::ReservePlayerConversationSpeechSlot() {
    // Como el original: el primer hueco libre (0..4), sin usar s_NextSpeechSlot
    tPedSpeechSlotID slot = 0;
    for (; slot < PLAYER_SPEECH_SLOT; slot++) {
        if (SpeechSlot(slot).Status == CAEPedSpeechSlot::eStatus::FREE) {
            break;
        }
    }
    if (slot == PLAYER_SPEECH_SLOT) {
        return false;
    }
    s_pConversationPedSlot1                    = slot;
    SpeechSlot(slot).IsReservedForPlayerConversation = true;
    SpeechSlot(slot).Status                    = CAEPedSpeechSlot::eStatus::RESERVED;
    return true;
}

// 0x4E38C0
bool CAEPedSpeechAudioEntity::RequestPlayerConversation(CPed* ped) {
    if (s_bAllSpeechDisabled) {
        return false;
    }

    if (ped->GetSpeechAE().m_IsSpeechForScriptsDisabled || ped->GetSpeechAE().m_IsSpeechDisabled) {
        return false;
    }

    // Como el original: FindPlayerPed se llama una vez por cada comprobación
    if (!FindPlayerPed()) {
        return false;
    }
    if (FindPlayerPed()->GetSpeechAE().m_IsSpeechDisabled || FindPlayerPed()->GetSpeechAE().m_IsSpeechForScriptsDisabled) {
        return false;
    }

    if (   s_bPedConversationHappening
        || s_bPlayerConversationHappening
        || ped->GetPedTalking()
        || !ReservePlayerConversationSpeechSlot()
    ) {
        return false;
    }

    s_pPlayerConversationPed       = ped;
    s_bPlayerConversationHappening = true;

    return true;
}

// 0x4E3960
void CAEPedSpeechAudioEntity::ReleasePlayerConversation() {
    if (!s_bPlayerConversationHappening) {
        return;
    }
    if (s_pConversationPedSlot1 >= 0) {
        auto& ss = SpeechSlot(s_pConversationPedSlot1);
        switch (ss.Status) {
        case CAEPedSpeechSlot::eStatus::FREE:
        case CAEPedSpeechSlot::eStatus::RESERVED:
            ss.Reset();
            break;
        }
        s_pConversationPedSlot1            = -1;
        ss.IsReservedForPlayerConversation = false; // Como el original: solo se quita la reserva
        s_pPlayerConversationPed           = nullptr;
    }
    s_bPlayerConversationHappening = false;
}

// 0x4E3A00
void CAEPedSpeechAudioEntity::SetUpConversation() {
    rng::fill(s_Conversation, CTX_GLOBAL_NO_SPEECH);

    const auto ForPed = [](const CPed* p, eGlobalSpeechContext ifFemale, eGlobalSpeechContext ifMale) {
        switch (p->m_nPedType) {
        case PED_TYPE_PROSTITUTE:
        case PED_TYPE_CIVFEMALE: return ifFemale;
        default:                 return ifMale;
        }
    };
    // Como el original: GetRandomNumberInRange(0, 1) (no DoCoinFlip)
    const auto CoinFlip = [] { return CAEAudioUtility::GetRandomNumberInRange<int32>(0, 1); };
    const auto Answer   = [] {
        switch ((int16)CAEAudioUtility::GetRandomNumberInRange<int32>(0, 2)) {
        case 0:  return CTX_GLOBAL_PCONV_AGREE_GOOD;
        case 1:  return CTX_GLOBAL_PCONV_AGREE_BAD;
        default: return CTX_GLOBAL_PCONV_ANS_NO;
        }
    };

    // Greeting: yes, ped 2, then ped 1
    s_Conversation[0] = ForPed(s_pConversationPed2, CTX_GLOBAL_PCONV_GREET_FEM, CTX_GLOBAL_PCONV_GREET_MALE);
    s_Conversation[1] = ForPed(s_pConversationPed1, CTX_GLOBAL_PCONV_GREET_FEM, CTX_GLOBAL_PCONV_GREET_MALE);

    switch ((int16)CAEAudioUtility::GetRandomNumberInRange<int32>(1, 10)) {
    case 1: case 2: case 3: case 4: {
        s_ConversationLength = 3;
        if ((int16)CoinFlip()) {
            s_Conversation[2] = CTX_GLOBAL_PCONV_STATE_GOOD;
            s_Conversation[3] = CTX_GLOBAL_PCONV_AGREE_GOOD;
        } else {
            s_Conversation[2] = CTX_GLOBAL_PCONV_STATE_BAD;
            s_Conversation[3] = CTX_GLOBAL_PCONV_AGREE_BAD;
        }
        s_Conversation[4] = CTX_GLOBAL_PCONV_QUESTION;
        s_Conversation[5] = Answer();
        s_Conversation[6] = ForPed(s_pConversationPed2, CTX_GLOBAL_PCONV_PART_FEM, CTX_GLOBAL_PCONV_PART_MALE);
        s_Conversation[7] = ForPed(s_pConversationPed1, CTX_GLOBAL_PCONV_PART_FEM, CTX_GLOBAL_PCONV_PART_MALE);
        break;
    }
    case 5: case 6: case 7: case 8: {
        s_ConversationLength = 2;
        (void)CoinFlip(); // Como el original: se sortea y no se usa
        s_Conversation[2] = CTX_GLOBAL_PCONV_QUESTION;
        s_Conversation[3] = Answer();
        s_Conversation[4] = ForPed(s_pConversationPed2, CTX_GLOBAL_PCONV_PART_FEM, CTX_GLOBAL_PCONV_PART_MALE);
        s_Conversation[5] = ForPed(s_pConversationPed1, CTX_GLOBAL_PCONV_PART_FEM, CTX_GLOBAL_PCONV_PART_MALE);
        break;
    }
    case 9: case 10: {
        s_ConversationLength = 1;
        const auto isGood = (int16)CoinFlip() != 0;
        s_Conversation[3] = CTX_GLOBAL_PCONV_DISMISS;
        s_Conversation[2] = isGood ? CTX_GLOBAL_PCONV_STATE_GOOD : CTX_GLOBAL_PCONV_STATE_BAD;
        break;
    }
    default:
        break; // Como el original: no hace nada más
    }
}

// 0x4E3C60
eAudioPedType CAEPedSpeechAudioEntity::GetAudioPedType(const char* name) {
    static const auto mapping = notsa::make_mapping<std::string_view, eAudioPedType>({
        {"PED_TYPE_GEN",    PED_TYPE_GEN   },
        {"PED_TYPE_EMG",    PED_TYPE_EMG   },
        {"PED_TYPE_PLAYER", PED_TYPE_PLAYER},
        {"PED_TYPE_GANG",   PED_TYPE_GANG  },
        {"PED_TYPE_GFD",    PED_TYPE_GFD   },
        {"PED_TYPE_SPC",    PED_TYPE_SPC   },
    });
    return notsa::find_value_or(mapping, name, PED_TYPE_UNK);
}

// 0x4E3CD0
ePedSpeechVoiceS16 CAEPedSpeechAudioEntity::GetVoice(const char* name, eAudioPedTypeS16 type) {
    // Como el original: strcmp contra las tablas de nombres del EXE (20 bytes por voz); SPC y tipos fuera de rango dan -1
    static constexpr struct { uintptr_t Begin, End; } NAMES[]{
        { 0x8AE6A8, 0x8AF6FC }, // PED_TYPE_GEN
        { 0x8BA0D8, 0x8BA470 }, // PED_TYPE_EMG
        { 0x8BBD40, 0x8BBED0 }, // PED_TYPE_PLAYER
        { 0x8BE1A8, 0x8BE5B8 }, // PED_TYPE_GANG
        { 0x8C4120, 0x8C4288 }, // PED_TYPE_GFD
    };
    const int32 t = (eAudioPedType)type;
    if ((uint32)t > 4) {
        return VOICE_UNK;
    }
    int16 idx = 0;
    for (auto p = NAMES[t].Begin; p < NAMES[t].End; p += 20, idx++) {
        if (!std::strcmp(name, reinterpret_cast<const char*>(p))) {
            return (ePedSpeechVoiceS16)idx;
        }
    }
    return VOICE_UNK;
}

// 0x4E3EB0
void CAEPedSpeechAudioEntity::DisableAllPedSpeech() {
    s_bAllSpeechDisabled = true;
}

// 0x4E3ED0
void CAEPedSpeechAudioEntity::SetCJMood(eCJMood basicMood, uint32 overrideTimeMS, int16 isGangBanging, int16 isFat, int16 isWellDressed) {
    s_nCJMoodOverrideTime = CTimer::GetTimeInMS() + overrideTimeMS;
    s_nCJBasicMood  = [=]{
        switch (basicMood) {
        case MOOD_AR:
        case MOOD_CR:
        case MOOD_PR:
        case MOOD_WR: return basicMood;
        default:      return MOOD_UNK;
        }
    }();
    s_nCJGangBanging = isGangBanging;
    s_nCJFat         = isFat;
    s_nCJWellDressed = isWellDressed;
}

// 0x4E53B0
eCJMood CAEPedSpeechAudioEntity::GetCurrentCJMood() {
    const uint32 now = CTimer::GetTimeInMS();
    auto* const plyr = FindPlayerPed();
    if (!plyr) {
        return MOOD_CR;
    }

    // Como el original: todas las condiciones se evalúan siempre (y en este orden), antes de mirar el modo forzado
    // Como el original: fcomp + test ah,1 (un NaN cuenta como menor)
    bool isWellDressed = CStats::GetStatValue(STAT_CLOTHES_RESPECT) >= 650.f && CStats::GetStatValue(STAT_FASHION_BUDGET) >= 10'000.f;

    bool isGangBanging = false;
    const auto numMembers = CPedGroups::ms_groups.data()[FindPlayerPed()->GetPlayerData()->m_nPlayerGroup].GetMembership().CountMembersExcludingLeader();
    if ((int16)numMembers > 1) {
        isGangBanging = true;
    } else if ((int16)numMembers == 1) {
        // Como el original: se recorren los 8 miembros (incluido el líder) sin cortar al encontrar uno
        auto& membership = CPedGroups::ms_groups.data()[FindPlayerPed()->GetPlayerData()->m_nPlayerGroup].GetMembership();
        for (int32 i = 0; i < 8; i++) {
            auto* const mem = membership.GetMember(i);
            if (!mem) {
                continue;
            }
            if (mem->m_nPedType == PED_TYPE_GANG2) {
                isGangBanging = true;
                continue;
            }
            const auto& speech = mem->GetSpeechAE();
            switch (speech.m_VoiceID) {
            case VOICE_GNG_RYDER:
            case VOICE_GNG_SWEET:
            case VOICE_GNG_SMOKE:
                if (speech.m_PedAudioType == PED_TYPE_GANG) {
                    isGangBanging = true;
                }
                break;
            }
        }
    }
    if (IsCJDressedInForGangSpeech()) {
        isGangBanging = true;
    }

    bool isFat = false;
    if (CStats::GetStatValue(STAT_FAT) >= 600.f) { // Como el original: un NaN cuenta como menor
        const float muscle = CStats::GetStatValue(STAT_MUSCLE);
        isFat = CStats::GetStatValue(STAT_FAT) - 200.f > muscle;
    }

    // Como el original: nivel de búsqueda del CWanted de los datos de jugador de este peatón (dword en +0x2C)
    auto* const playerData = plyr->GetPlayerData();
    const auto* const wanted = playerData ? reinterpret_cast<const uint8*>(playerData->m_pWanted) : nullptr;
    const auto wantedLevel = *reinterpret_cast<const int32*>(wanted + 0x2C);

    eCJMood mood;
    if (wantedLevel > 3) {
        mood = MOOD_PR;
    } else if (wantedLevel > 1) {
        mood = MOOD_AR;
    } else if (CTheScripts::LastMissionPassedTime != (uint32)-1) {
        if (now >= CTheScripts::LastMissionPassedTime) {
            mood = now < CTheScripts::LastMissionPassedTime + 180'000 ? MOOD_WR : MOOD_CR;
        } else {
            CTheScripts::LastMissionPassedTime = now;
            mood = MOOD_CR;
        }
    } else {
        mood = MOOD_CR;
    }

    // Como el original: el modo forzado solo vale si su tiempo es estrictamente mayor; los valores se toman como byte
    if (s_nCJMoodOverrideTime > now) {
        if (s_nCJBasicMood != MOOD_UNK) {
            mood = s_nCJBasicMood;
        }
        if (s_nCJGangBanging != -1) {
            isGangBanging = (uint8)s_nCJGangBanging != 0;
        }
        if (s_nCJFat != -1) {
            isFat = (uint8)s_nCJFat != 0;
        }
        if (s_nCJWellDressed != -1) {
            isWellDressed = (uint8)s_nCJWellDressed != 0;
        }
    }

    switch (mood) {
    case MOOD_AR: return isGangBanging ? MOOD_AG : MOOD_AR;
    case MOOD_CR: return isGangBanging ? MOOD_CG : isFat ? MOOD_CF : isWellDressed ? MOOD_CD : MOOD_CR;
    case MOOD_PR: return isGangBanging ? MOOD_PG : MOOD_PR;
    case MOOD_WR: return isGangBanging ? MOOD_WG : MOOD_WR;
    default:      return MOOD_CR;
    }
}

// 0x4E4700
eCJMood __stdcall CAEPedSpeechAudioEntity::GetNextMoodToUse(eCJMood currMood) {
    switch (currMood) {
    case MOOD_AG:
    case MOOD_PR: return MOOD_AR;
    case MOOD_PG: return MOOD_AG;
    case MOOD_WG: return MOOD_CG;
    default:      return MOOD_CR;
    }
}

// 0x4E4760
ePedSpeechVoiceS16 __stdcall CAEPedSpeechAudioEntity::GetVoiceForMood(eCJMood mood) {
    // Como el original: la voz es la primera del humor más el número aleatorio (0 o 1); humor desconocido => CR
    const auto r = CAEAudioUtility::GetRandomNumberInRange<int32>(0, 1);
    switch (mood) {
    case MOOD_AG: return (ePedSpeechVoiceS16)(VOICE_PLY_AG + r);
    case MOOD_AR: return (ePedSpeechVoiceS16)(VOICE_PLY_AR + r);
    case MOOD_CD: return (ePedSpeechVoiceS16)(VOICE_PLY_CD + r);
    case MOOD_CF: return (ePedSpeechVoiceS16)(VOICE_PLY_CF + r);
    case MOOD_CG: return (ePedSpeechVoiceS16)(VOICE_PLY_CG + r);
    case MOOD_PG: return (ePedSpeechVoiceS16)(VOICE_PLY_PG + r);
    case MOOD_PR: return (ePedSpeechVoiceS16)(VOICE_PLY_PR + r);
    case MOOD_WG: return (ePedSpeechVoiceS16)(VOICE_PLY_WG + r);
    case MOOD_WR: return (ePedSpeechVoiceS16)(VOICE_PLY_WR + r);
    case MOOD_CR:
    default:      return (ePedSpeechVoiceS16)(VOICE_PLY_CR + r);
    }
}

// 0x4E3EC0
void CAEPedSpeechAudioEntity::EnableAllPedSpeech() {
    s_bAllSpeechDisabled = false;
}

// 0x4E4270
bool CAEPedSpeechAudioEntity::IsCJDressedInForGangSpeech() {
    auto* const player = FindPlayerPed(PED_TYPE_PLAYER1);
    if (!player) {
        return false;
    }

    const auto& keys = player->GetPlayerData()->m_pPedClothesDesc->m_anTextureKeys;
    const auto torso = keys[CLOTHES_TEXTURE_TORSO], legs = keys[CLOTHES_TEXTURE_LEGS], hats = keys[CLOTHES_TEXTURE_HATS], glasses = keys[CLOTHES_TEXTURE_GLASSES];

    // Como el original: la ciudad más cercana a la posición del propio peatón (no a FindPlayerCoors)
    if (CGameLogic::FindCityClosestToPoint(player->GetPosition()) != LEVEL_NAME_LOS_SANTOS) {
        return false;
    }

    return torso == CKeyGen::GetUppercaseKey("hoodyagreen")
        || torso == CKeyGen::GetUppercaseKey("shirtbgang")
        || legs == CKeyGen::GetUppercaseKey("tracktrgang")
        || legs == CKeyGen::GetUppercaseKey("denimsgang")
        || hats == CKeyGen::GetUppercaseKey("bandgang")
        || hats == CKeyGen::GetUppercaseKey("hockey")
        || hats == CKeyGen::GetUppercaseKey("capgang")
        || hats == CKeyGen::GetUppercaseKey("capgangback")
        || hats == CKeyGen::GetUppercaseKey("capgangside")
        || hats == CKeyGen::GetUppercaseKey("capgangover")
        || hats == CKeyGen::GetUppercaseKey("capgangup")
        || glasses == CKeyGen::GetUppercaseKey("bandred3")
        || glasses == CKeyGen::GetUppercaseKey("bandblue3")
        || glasses == CKeyGen::GetUppercaseKey("bandgang3")
        || glasses == CKeyGen::GetUppercaseKey("bandblack3");
}

// 0x4E4600
bool __stdcall CAEPedSpeechAudioEntity::IsGlobalContextImportantForInterupting(eGlobalSpeechContext gCtx) {
    switch (gCtx) {
    case 0x0D: case 0x0F:            // CTX_GLOBAL_ARREST, CTX_GLOBAL_ARRESTED
    case 0x7D: case 0x7E: case 0x7F: // CTX_GLOBAL_JOIN_GANG_NO, CTX_GLOBAL_JOIN_GANG_YES, CTX_GLOBAL_JOIN_ME_ASK
        return true;
    }
    return false;
}

// 0x4E46F0 - unused
bool __stdcall CAEPedSpeechAudioEntity::IsGlobalContextUberImportant(int16 gCtx) {
    return false;
}

// 0x4E44F0
bool __stdcall CAEPedSpeechAudioEntity::IsGlobalContextPain(eGlobalSpeechContext gCtx) {
    return gCtx > CTX_GLOBAL_PAIN_START && gCtx < CTX_GLOBAL_PAIN_END;
}

// 0x4E46B0
bool CAEPedSpeechAudioEntity::IsGlobalContextImportantForWidescreen(eGlobalSpeechContext gCtx) {
    switch (m_PedAudioType) {
    case PED_TYPE_GFD:
    case PED_TYPE_PLAYER:
        return true;
    }
    switch (gCtx) {
    case 0x0D: case 0x0F: // CTX_GLOBAL_ARREST, CTX_GLOBAL_ARRESTED
    case 0x74:            // CTX_GLOBAL_HAVING_SEX
        return true;
    }
    return false;
}

// 0x4E4510
bool CAEPedSpeechAudioEntity::IsGlobalContextImportantForStreaming(eGlobalSpeechContext gCtx) {
    switch (m_PedAudioType) {
    case PED_TYPE_GFD:
    case PED_TYPE_PLAYER:
        return true;
    }
    // Como el original: arresto, conversaciones (0x30..0x40), unirse a la banda y conversación entre peatones
    if (gCtx == 0x0D || gCtx == 0x0F) {
        return true;
    }
    if ((gCtx >= 0x30 && gCtx <= 0x40) || (gCtx >= 0x7D && gCtx <= 0x7F) || (gCtx >= 0xA1 && gCtx <= 0xAB)) {
        return true;
    }
    return false;
}

// 0x4E47E0
int16 CAEPedSpeechAudioEntity::GetRepeatTime(eGlobalSpeechContext gCtx) const {
    if (gCtx < 0 || gCtx >= CTX_GLOBAL_NUM) {
        return 0;
    }
    const auto* const e = ExeSpeechContextEntry(FindExeSpeechContext(gCtx));
    return e[0] == -1 ? 0 : e[6];
}

// 0x4E4840
void CAEPedSpeechAudioEntity::LoadAndPlaySpeech(uint32 playbackTimeOffsetMS) {
    auto& speech = SpeechSlot(m_PedSpeechSlotID);
    switch (speech.Status) {
    case CAEPedSpeechSlot::eStatus::FREE:
    case CAEPedSpeechSlot::eStatus::RESERVED:
        break;
    default:
        return;
    }

    AEAudioHardware.LoadSound(m_BankID, m_SoundID, (eSoundBankSlot)(SND_BANK_SLOT_SPEECH1 + (uint16)m_PedSpeechSlotID));
    // Como el original: no se tocan Sound ni las reservas de conversación
    speech.Status            = CAEPedSpeechSlot::eStatus::LOADING;
    speech.SoundBankID       = m_BankID;
    speech.SoundID           = m_SoundID;
    speech.AudioEntity       = this;
    speech.StartPlaybackTime = CTimer::GetTimeInMS() + playbackTimeOffsetMS;
    speech.PedAudioType      = m_PedAudioType;
    speech.GCtx              = m_LastGCtx;
    speech.ForceAudible      = m_IsForcedAudible;
}

// 0x4E49B0
int32 CAEPedSpeechAudioEntity::GetNumSlotsPlayingContext(eGlobalSpeechContext gCtx) {
    // Como el original: solo los huecos 0..4 (el del jugador no cuenta)
    int32 n = 0;
    for (int32 i = 0; i < PLAYER_SPEECH_SLOT; i++) {
        const auto& speech = SpeechSlot(i);
        if (speech.Status != CAEPedSpeechSlot::eStatus::FREE && speech.GCtx == gCtx) {
            n++;
        }
    }
    return n;
}

// 0x4E49E0
uint32 CAEPedSpeechAudioEntity::GetNextPlayTime(eGlobalSpeechContext gCtx) const {
    if (gCtx < 0 || gCtx >= CTX_GLOBAL_NUM) {
        return 0;
    }
    return IsGlobalContextPain(gCtx)
        ? m_NextTimeCanSayPain.data()[gCtx - (CTX_GLOBAL_PAIN_START + 1)]
        : gGlobalSpeechContextNextPlayTime.data()[gCtx];
}

// 0x4E4A20
void CAEPedSpeechAudioEntity::SetNextPlayTime(eGlobalSpeechContext gCtx) {
    if (gCtx < 0 || gCtx >= CTX_GLOBAL_NUM) {
        return;
    }
    const auto* const e = ExeSpeechContextEntry(FindExeSpeechContext(gCtx));
    if (e[0] == -1) {
        return;
    }
    // Como el original: el aleatorio y el tiempo de repetición se suman en 16 bits con signo
    const auto delay = (int16)(CAEAudioUtility::GetRandomNumberInRange<int32>(1, 1000) + e[6]);
    const auto time  = CTimer::GetTimeInMS() + (int32)delay;
    if (IsGlobalContextPain(gCtx)) {
        m_NextTimeCanSayPain.data()[gCtx - (CTX_GLOBAL_PAIN_START + 1)] = time;
    } else {
        gGlobalSpeechContextNextPlayTime.data()[gCtx] = time;
    }
}

// 0x4E56D0
void CAEPedSpeechAudioEntity::DisablePedSpeech(int16 a1) {
    if (m_IsInitialized) {
        m_IsSpeechDisabled = true;
        if (a1)
            StopCurrentSpeech();
    }
}

// 0x4E5700
void CAEPedSpeechAudioEntity::DisablePedSpeechForScriptSpeech(int16 a1) {
    if (m_IsInitialized) {
        m_IsSpeechForScriptsDisabled = true;
        if (a1)
            StopCurrentSpeech();
    }
}

// 0x4E5730
bool CAEPedSpeechAudioEntity::CanPedSayGlobalContext(eGlobalSpeechContext gCtx) const {
    if (!m_IsInitialized) {
        return false;
    }
    const auto sCtx = GetSpecificSpeechContext(gCtx, m_PedAudioType);
    if (sCtx < 0) {
        return false;
    }
    // Como el original: sin comprobar el rango del contexto ni de la voz; los genéricos solo miran el byte bajo
    // y los GFD nunca pueden (la tabla de saltos del EXE los manda a `false`).
    switch (m_PedAudioType) {
    case PED_TYPE_GEN:    return *ExeGenRange(sCtx, m_VoiceID) != 0xFF;
    case PED_TYPE_EMG:    return ExeRange(EXE_EMG_TABLES, sCtx, m_VoiceID)[0] != -1;
    case PED_TYPE_PLAYER: return ExeRange(EXE_PLY_TABLES, sCtx, m_VoiceID)[0] != -1;
    case PED_TYPE_GANG:   return ExeRange(EXE_GNG_TABLES, sCtx, m_VoiceID)[0] != -1;
    default:              return false;
    }
}

// 0x4E58C0
bool CAEPedSpeechAudioEntity::GetVoiceAndTypeFromModel(eModelID modelId) {
    auto* const mi = CModelInfo::GetModelInfo(modelId)->AsPedModelInfoPtr();

    m_PedAudioType = mi->m_nPedAudioType; // Como el original: se escribe antes de comprobarlo
    if (m_PedAudioType < 0 || m_PedAudioType >= PED_TYPE_NUM) {
        return false;
    }
    if (m_PedAudioType == PED_TYPE_SPC) {
        return GetVoiceAndTypeForSpecialPed(mi->m_nKey);
    }

    m_VoiceID = mi->m_nVoiceId;
    if (m_VoiceID < 0) {
        return false;
    }

    mi->IncrementVoice();

    return true;
}

// 0x4E5920
int16 CAEPedSpeechAudioEntity::GetSoundAndBankIDs(eGlobalSpeechContext gCtx, eSpecificSpeechContext& outSpecificSpeechContext) {
    if (gCtx < 0 || gCtx >= CTX_GLOBAL_NUM || m_PedAudioType < 0 || m_PedAudioType >= PED_TYPE_NUM) {
        return -1;
    }

    const auto sCtx = GetSpecificSpeechContext(gCtx, m_PedAudioType);
    if (sCtx < 0) {
        return -1;
    }
    outSpecificSpeechContext = sCtx;

    // Como el original: rango [primero, último] de las tablas del EXE (sin comprobar la voz)
    int16 first, last;
    if (IsGlobalContextPain(gCtx)) {
        const int16 voice = m_PedAudioType == PED_TYPE_PLAYER ? VOICE_PAIN_CARL : m_IsFemale == 0 ? VOICE_PAIN_MALE : VOICE_PAIN_FEMALE;
        m_BankID = (eSoundBank)ExeVoiceBank(EXE_PAIN_TABLES, voice);
        const auto* const r = ExeRange(EXE_PAIN_TABLES, sCtx, voice);
        first = r[0];
        last  = r[1];
    } else {
        const tExeSpeechTypeTables* tables{};
        int16 voice = m_VoiceID;
        switch (m_PedAudioType) {
        case PED_TYPE_GEN: {
            if (sCtx > EXE_GEN_TABLES.MaxSpecificContext) {
                return -1;
            }
            m_BankID = (eSoundBank)ExeVoiceBank(EXE_GEN_TABLES, voice);
            const auto* const r = ExeGenRange(sCtx, voice);
            first = r[0]; // Como el original: bytes sin signo (0xFF es "sin sonidos")
            last  = r[1];
            if (first == 0xFF) {
                return -1;
            }
            break;
        }
        case PED_TYPE_EMG:    tables = &EXE_EMG_TABLES; break;
        case PED_TYPE_PLAYER: tables = &EXE_PLY_TABLES; break;
        case PED_TYPE_GANG:   tables = &EXE_GNG_TABLES; break;
        case PED_TYPE_GFD:    tables = &EXE_GFD_TABLES; break;
        default:              return -1;
        }
        if (tables) {
            if (sCtx > tables->MaxSpecificContext) {
                return -1;
            }
            if (m_PedAudioType == PED_TYPE_PLAYER) {
                // Como el original: se baja de humor hasta encontrar una voz con sonidos para el contexto (o llegar a CR)
                auto mood = GetCurrentCJMood();
                voice = GetVoiceForMood(mood);
                while (mood != MOOD_CR && ExeRange(EXE_PLY_TABLES, sCtx, voice)[0] == -1) {
                    mood  = GetNextMoodToUse(mood);
                    voice = GetVoiceForMood(mood);
                }
            }
            m_BankID = (eSoundBank)ExeVoiceBank(*tables, voice);
            const auto* const r = ExeRange(*tables, sCtx, voice);
            first = r[0];
            last  = r[1];
        }
    }
    if (first == -1) {
        return -1;
    }

    // Como el original: se prueban hasta 20 sonidos a partir de uno aleatorio; se elige el primero que no esté en la
    // memoria de frases o, si todos están, el usado hace más tiempo (mayor índice en la memoria).
    const int32 count  = (int32)last - (int32)first + 1;
    const int16 offset = (int16)CAEAudioUtility::GetRandomNumberInRange<int32>(0, (int32)last - (int32)first);
    const int16 limit  = (int16)std::min(count, 20);
    struct { int16 SoundID, PhraseIdx; } tried[20]{}; // (el EXE no lo inicializa; solo se leen entradas escritas)
    int16 i = 0, phraseIdx = 0;
    if (limit > 0) {
        do {
            if (phraseIdx == (int16)s_PhraseMemory.size()) {
                break;
            }
            const auto soundID = (int16)(((int32)offset + (int32)i) % count + first);
            m_SoundID = soundID;
            tried[i].SoundID = soundID;
            for (phraseIdx = 0; phraseIdx < (int16)s_PhraseMemory.size(); phraseIdx++) {
                const auto& p = s_PhraseMemory.data()[phraseIdx];
                if (p.SoundID == soundID && p.BankID == m_BankID) {
                    tried[i].PhraseIdx = phraseIdx;
                    break;
                }
            }
        } while (++i < limit);
    }
    if (i == limit && phraseIdx != (int16)s_PhraseMemory.size()) {
        int16 best = tried[0].SoundID, bestIdx = 0;
        for (int16 k = 0; k < limit; k++) {
            if (tried[k].PhraseIdx > bestIdx) {
                best    = tried[k].SoundID;
                bestIdx = tried[k].PhraseIdx;
            }
        }
        m_SoundID = best;
    }

    // Insert into front of `s_PhraseMemory`
    for (auto k = (int32)s_PhraseMemory.size() - 2; k >= 0; k--) {
        s_PhraseMemory.data()[k + 1].SoundID = s_PhraseMemory.data()[k].SoundID;
        s_PhraseMemory.data()[k + 1].BankID  = s_PhraseMemory.data()[k].BankID;
    }
    s_PhraseMemory.data()[0].SoundID = m_SoundID;
    s_PhraseMemory.data()[0].BankID  = m_BankID;

    return (int16)((uint16)m_SoundID - first);
}

// 0x4E5F10
int16 CAEPedSpeechAudioEntity::CanWePlayGlobalSpeechContext(eGlobalSpeechContext gCtx) {
    // Como el original: un participante de una conversación usa su hueco reservado (si está reservado)
    tPedSpeechSlotID slot = -1;
    bool fromConversation = false;
    if (s_bPedConversationHappening && !IsGlobalContextImportantForInterupting(gCtx) && !IsGlobalContextPain(gCtx)) {
        if (this == SpeechOf(s_pConversationPed1)) {
            slot = s_pConversationPedSlot1;
            fromConversation = slot != -1;
        } else if (this == SpeechOf(s_pConversationPed2)) {
            slot = s_pConversationPedSlot2;
            fromConversation = slot != -1;
        }
    }
    if (!fromConversation && s_bPlayerConversationHappening && !IsGlobalContextImportantForInterupting(gCtx) && !IsGlobalContextPain(gCtx)) {
        if (this == SpeechOf(s_pPlayerConversationPed)) {
            slot = s_pConversationPedSlot1;
        }
        fromConversation = slot != -1;
    }
    if (fromConversation) {
        return SpeechSlot(slot).Status == CAEPedSpeechSlot::eStatus::RESERVED ? slot : -1;
    }

    // Como el original: primero se busca (y se avanza) el hueco libre y después se mira el tiempo del contexto
    const auto freeSlot = GetFreeSpeechSlot();
    if (freeSlot == -1) {
        return -1;
    }
    if (gCtx >= 0 && gCtx < CTX_GLOBAL_NUM && CTimer::GetTimeInMS() < GetNextPlayTime(gCtx)) {
        return -1;
    }
    return freeSlot;
}

// 0x4E6550
int16 CAEPedSpeechAudioEntity::AddSayEvent(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    // Como el original: al retraso se le suma un desfase estático del EXE (0xB61C10)
    startTimeDelayMs += *reinterpret_cast<const uint32*>(0xB61C10);

    if (!m_IsInitialized || s_bAllSpeechDisabled || m_IsSpeechForScriptsDisabled) {
        return -1;
    }
    if (TheCamera.m_bWideScreenOn && !IsGlobalContextImportantForWidescreen(gCtx)) {
        return -1;
    }
    if (CGameLogic::GameState == GAMELOGIC_STATE_BUSTED) { // Como el original: byte 0x96A8B0 == 2
        return -1;
    }
    if (gCtx == CTX_GLOBAL_CHAT) {
        if (CTimer::GetFrameCounter() & 0x3F || s_bPedConversationHappening || s_bPlayerConversationHappening) {
            return -1;
        }
    } else if (gCtx == CTX_GLOBAL_PAIN_CJ_DROWNING && CWeather::UnderWaterness < 0.5f) {
        return -1;
    }
    if (!CAEAudioUtility::ResolveProbability(probability) || audioEvent != AE_SPEECH_PED) {
        return -1;
    }
    if (m_IsSpeechDisabled && !overrideSilence) {
        return -1;
    }

    const bool isPain = IsGlobalContextPain(gCtx);
    if (isPain || IsGlobalContextImportantForInterupting(gCtx)) {
        if (m_IsPlayingSpeech) {
            const eGlobalSpeechContext curr = SpeechSlot(m_PedSpeechSlotID).GCtx;
            if ((IsGlobalContextPain(curr) || IsGlobalContextImportantForInterupting(curr)) && curr != CTX_GLOBAL_PAIN_CJ_HIGH_FALL) {
                return -1;
            }
            StopCurrentSpeech();
        }
    } else if (m_IsPlayingSpeech) {
        return -1;
    }

    if (!m_Entity->AsPed()->IsAlive() && !isPain) {
        return -1;
    }

    // Como el original: todo el código de voz lee 0xB6B98C (s_pPlayerAttachedForRadio); 0xB6B99C (s_pVehicleAudioSettingsForRadio) sólo lo lee el audio de vehículos.
    if (IsGlobalContextImportantForStreaming(gCtx) || isForceAudible || isFrontEnd) {
        if (CStreaming::ms_numModelsRequested > 15 && CAEVehicleAudioEntity::s_pPlayerAttachedForRadio) {
            return -1;
        }
    } else if (CStreaming::IsVeryBusy()) {
        return -1;
    }

    if (m_PedAudioType == PED_TYPE_PLAYER) {
        // Como el original: FindPlayerPed(0), dos veces si ya habla otro jugador
        if (s_bAPlayerSpeaking && !m_IsPlayingSpeech && m_Entity == FindPlayerPed(0)) {
            return -1;
        }
        if (m_Entity != FindPlayerPed(0)) {
            return -1;
        }
    }

    // Como el original: falla solo si la distancia al cuadrado es mayor que 1600 (un NaN pasa)
    if ((m_Entity->GetPosition() - TheCamera.GetPosition()).SquaredMagnitude() > 1600.f) {
        return -1;
    }

    // Como el original: el hueco se pide antes de elegir el sonido
    tPedSpeechSlotID speechSlotID;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        if (CTimer::GetTimeInMS() < GetNextPlayTime(gCtx)) {
            return -1;
        }
        speechSlotID = PLAYER_SPEECH_SLOT;
    } else {
        speechSlotID = CanWePlayGlobalSpeechContext(gCtx);
        if (speechSlotID < 0 || speechSlotID >= PLAYER_SPEECH_SLOT) {
            return -1;
        }
    }

    eSpecificSpeechContext sCtx;
    const auto soundID = GetSoundAndBankIDs(gCtx, sCtx);
    if (soundID < 0) {
        return -1;
    }

    SetNextPlayTime(gCtx);

    m_EventVolume     = GetDefaultVolume(AE_SPEECH_PED);
    m_IsFrontend      = isFrontEnd;
    m_IsForcedAudible = isForceAudible;
    if (const auto veh = m_Entity->AsPed()->GetVehicleIfInOne()) {
        if (veh == CAEAudioUtility::FindVehicleOfPlayer() && gCtx != CTX_GLOBAL_STOMACH_RUMBLE) {
            m_IsFrontend = true;
        }
    }
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        m_EventVolume = 3.f + m_EventVolume;
    }
    m_PedSpeechSlotID = speechSlotID;
    m_IsPlayingSpeech = true;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        s_bAPlayerSpeaking = true;
    }
    m_LastGCtx    = gCtx;
    m_EventVolume = GetSpeechContextVolumeOffset(gCtx) + m_EventVolume;

    LoadAndPlaySpeech(startTimeDelayMs);

    return soundID;
}

// 0x4E68D0
void CAEPedSpeechAudioEntity::Initialise(CEntity* ped) {
    // Como el original: solo estos campos (no toca el sonido, banco, hueco ni vtable)
    m_Entity      = ped;
    m_Sound       = nullptr;
    m_EventVolume = -100.f;
    m_LastGCtx    = CTX_GLOBAL_UNK;
    m_IsFemale    = 0;
    m_NextTimeCanSayPain.fill(0);
    m_IsPlayingSpeech            = false;
    m_IsSpeechDisabled           = false;
    m_IsSpeechForScriptsDisabled = false;
    m_IsFrontend                 = false;
    m_IsForcedAudible            = false;

    const int32 modelId = (int16)ped->m_nModelIndex; // Como el original: movsx
    if (modelId < 0) {
        return;
    }
    auto* mi = CModelInfo::GetModelInfo(modelId)->AsPedModelInfoPtr();
    m_PedAudioType = mi->m_nPedAudioType;
    if (m_PedAudioType < 0 || m_PedAudioType >= PED_TYPE_NUM) {
        return;
    }
    if (m_PedAudioType == PED_TYPE_SPC) {
        if (!GetVoiceAndTypeForSpecialPed(mi->m_nKey)) {
            return;
        }
    } else {
        m_VoiceID = mi->m_nVoiceId;
        if (m_VoiceID < 0) {
            return;
        }
        mi->IncrementVoice();
    }

    // Como el original: GetSexFromModel en línea
    mi = CModelInfo::GetModelInfo(modelId)->AsPedModelInfoPtr();
    if (mi->m_nPedAudioType < 0 || mi->m_nPedAudioType >= PED_TYPE_NUM) {
        return;
    }
    if (mi->m_nPedAudioType != PED_TYPE_SPC) {
        m_IsFemale = mi->m_nPedType == PED_TYPE_CIVFEMALE || mi->m_nPedType == PED_TYPE_PROSTITUTE ? 1 : 0;
    }
    m_IsInitialized = true;
}

// 0x4E69E0
bool CAEPedSpeechAudioEntity::CanPedHoldConversation() const {
    return CanPedSayGlobalContext(CTX_GLOBAL_PCONV_QUESTION);
}

// 0x4E3F70
void CAEPedSpeechAudioEntity::EnablePedSpeech() {
    if (m_IsInitialized) {
        m_IsSpeechDisabled = false;
    }
}

// 0x4E3F90
void CAEPedSpeechAudioEntity::EnablePedSpeechForScriptSpeech() {
    if (m_IsInitialized) {
        m_IsSpeechForScriptsDisabled = false;
    }
}

// 0x4E3FB0
void CAEPedSpeechAudioEntity::StopCurrentSpeech() {
    if (!m_IsInitialized || !m_IsPlayingSpeech) {
        return;
    }

    if (auto* const tFacial = GetFacialTask(m_Entity)) {
        tFacial->StopAll();
    }

    if (SpeechSlot(m_PedSpeechSlotID).Status == CAEPedSpeechSlot::eStatus::PLAYING) {
        if (m_Sound) {
            m_Sound->StopSoundAndForget();
        }
        m_Sound = nullptr;
    }

    // Como el original: un hueco reservado para una conversación se reinicia y vuelve a quedar reservado (si lo está
    // para ambas, solo queda la reserva de la conversación entre peatones)
    auto& speech = SpeechSlot(m_PedSpeechSlotID);
    if (speech.IsReservedForPedConversation) {
        speech.Reset();
        SpeechSlot(m_PedSpeechSlotID).IsReservedForPedConversation = true;
        SpeechSlot(m_PedSpeechSlotID).Status = CAEPedSpeechSlot::eStatus::RESERVED;
    } else if (speech.IsReservedForPlayerConversation) {
        speech.Reset();
        SpeechSlot(m_PedSpeechSlotID).IsReservedForPlayerConversation = true;
        SpeechSlot(m_PedSpeechSlotID).Status = CAEPedSpeechSlot::eStatus::RESERVED;
    } else {
        speech.Reset();
    }

    m_IsPlayingSpeech = false;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        s_bAPlayerSpeaking = false;
    }
    m_PedSpeechSlotID = -1;
}

// 0x4E4400
bool CAEPedSpeechAudioEntity::GetSoundAndBankIDsForScriptedSpeech(eAudioEvents scriptID) {
    // Como el original: los campos se ponen a -1 antes de llamar y solo se escriben si ambos son válidos
    m_BankID  = SND_BANK_UNK;
    m_SoundID = -1;
    int32         soundID = -1;
    eSoundBankS32 bankID{ (eSoundBank)-1 };
    CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(scriptID, bankID, soundID, -1);
    if (bankID < 0 || soundID < 0) {
        return false;
    }
    m_BankID  = (eSoundBank)bankID;
    m_SoundID = (int16)soundID;
    return true;
}

// 0x4E4200
bool CAEPedSpeechAudioEntity::GetSexFromModel(eModelID model) {
    const auto* const mi = CModelInfo::GetModelInfo(model)->AsPedModelInfoPtr();
    if (mi->m_nPedAudioType < 0 || mi->m_nPedAudioType >= PED_TYPE_NUM) {
        return false; // Como el original
    }
    if (mi->m_nPedAudioType == PED_TYPE_SPC) {
        return true;
    }
    m_IsFemale = mi->m_nPedType == PED_TYPE_CIVFEMALE || mi->m_nPedType == PED_TYPE_PROSTITUTE ? 1 : 0;
    return true;
}

// 0x4E3F50
bool CAEPedSpeechAudioEntity::GetPedTalking() const {
    return m_IsInitialized && m_IsPlayingSpeech;
}

// 0x4E4170
bool CAEPedSpeechAudioEntity::GetVoiceAndTypeForSpecialPed(uint32 modelNameHash) {
    // Como el original: se calcula la clave de cada nombre de la tabla del EXE en cada búsqueda
    int16 idx = 0;
    for (; idx < NUM_EXE_SPECIAL_PEDS; idx++) {
        if (CKeyGen::GetUppercaseKey(reinterpret_cast<const char*>(EXE_SPECIAL_PED_NAMES + idx * 20)) == modelNameHash) {
            break;
        }
    }
    if (idx == NUM_EXE_SPECIAL_PEDS) {
        return false;
    }
    const auto* const e   = reinterpret_cast<const uint8*>(EXE_SPECIAL_PED_VOICES + idx * 6);
    const auto voice      = *reinterpret_cast<const int16*>(e);
    const auto type       = *reinterpret_cast<const int16*>(e + 2);
    const auto isFemale   = e[4];
    if (voice == -1 || type == -1) {
        return false;
    }
    m_PedAudioType = (eAudioPedType)type;
    m_VoiceID      = (ePedSpeechVoiceS16)voice;
    m_IsFemale     = isFemale;
    return true;
}

// notsa
ePainSpeechVoices CAEPedSpeechAudioEntity::GetPainVoice() const {
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        return VOICE_PAIN_CARL;
    }
    return m_IsFemale
        ? VOICE_PAIN_FEMALE
        : VOICE_PAIN_MALE;
}

// 0x4E3520
void CAEPedSpeechAudioEntity::UpdateParameters(CAESound* sound, int16 playTime) {
    if (playTime == -1) { // Sound has finished?
        m_IsPlayingSpeech = false;
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            s_bAPlayerSpeaking = false;
        }
        SpeechSlot(m_PedSpeechSlotID).Reset();
        m_Sound   = nullptr;
        m_SoundID = -1;
        m_BankID  = SND_BANK_UNK;

        // Como el original: se compara con el hueco que tenía antes de soltarlo
        if (s_bPedConversationHappening) {
            if (   (this == SpeechOf(s_pConversationPed1) && m_PedSpeechSlotID == s_pConversationPedSlot1)
                || (this == SpeechOf(s_pConversationPed2) && m_PedSpeechSlotID == s_pConversationPedSlot2)
            ) {
                SpeechSlot(m_PedSpeechSlotID).Status = CAEPedSpeechSlot::eStatus::RESERVED;
            }
        } else if (s_bPlayerConversationHappening) {
            if (this == SpeechOf(s_pPlayerConversationPed) && m_PedSpeechSlotID == s_pConversationPedSlot1) {
                SpeechSlot(m_PedSpeechSlotID).Status = CAEPedSpeechSlot::eStatus::RESERVED;
            }
        }

        m_PedSpeechSlotID = -1;
        m_IsForcedAudible = false;

        if (auto* const tFacial = GetFacialTask(m_Entity)) {
            tFacial->StopAll();
        }
    } else { // Update sound
        SpeechSlot(m_PedSpeechSlotID).Status = CAEPedSpeechSlot::eStatus::PLAYING;
        auto* const ped = m_Entity->AsPed();
        // Como el original: bit 15 de las banderas (bRemoveHead) y cabeza pendiente de quitar
        if (ped->bRemoveHead && ped->m_nBodypartToRemove == PED_NODE_HEAD) {
            sound->StopSound();
        } else if (!m_IsFrontend && ped) {
            sound->SetPosition(ped->GetPosition());
        }
    }
}

// 0x4E4F70
void CAEPedSpeechAudioEntity::AddScriptSayEvent(eAudioEvents audioEvent, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    if (!m_IsInitialized || s_bAllSpeechDisabled) {
        return;
    }
    if (TheCamera.m_bWideScreenOn) {
        return;
    }
    if (audioEvent != AE_SCRIPT_SPEECH_PED) {
        return;
    }
    if (m_IsSpeechDisabled && !overrideSilence) {
        return;
    }
    if (m_IsPlayingSpeech) {
        return;
    }
    if (CStreaming::IsVeryBusy()) {
        return;
    }
    tPedSpeechSlotID speechSlotID;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        if (s_bAPlayerSpeaking && !m_IsPlayingSpeech) {
            return;
        }
        speechSlotID = PLAYER_SPEECH_SLOT;
    } else {
        speechSlotID = CanWePlayScriptedSpeech();
        if (speechSlotID < 0 || speechSlotID >= PLAYER_SPEECH_SLOT) {
            return;
        }
    }
    if (!GetSoundAndBankIDsForScriptedSpeech(scriptID)) {
        return;
    }

    m_EventVolume     = GetDefaultVolume(AE_SCRIPT_SPEECH_PED); // Como el original: volumen del evento 0x35
    m_IsFrontend      = isFrontEnd;
    m_IsForcedAudible = isForceAudible;
    // Como el original: en el vehículo del jugador la voz pasa a 2D (no se fuerza el volumen)
    if (const auto veh = m_Entity->AsPed()->GetVehicleIfInOne()) {
        if (veh == CAEAudioUtility::FindVehicleOfPlayer()) {
            m_IsFrontend = true;
        }
    }
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        m_EventVolume = 3.f + m_EventVolume;
    }

    m_PedSpeechSlotID = speechSlotID;
    m_IsPlayingSpeech = true;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        s_bAPlayerSpeaking = true;
    }
    m_LastGCtx = CTX_GLOBAL_UNK;

    LoadAndPlaySpeech(0);
}

// 0x4E5670
void CAEPedSpeechAudioEntity::Terminate() {
    StopCurrentSpeech();
    // Como el original: solo estos campos (no toca la vtable, el sexo, el modo 2D, el volumen forzado ni el dolor)
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
}

// 0x4E5CD0
void CAEPedSpeechAudioEntity::PlayLoadedSound() {
    if (!m_Entity) {
        return;
    }

    // Como el original: CAESound local inicializado sin banderas; las banderas, la atenuación y el volumen forzado se
    // escriben después directamente en el sonido
    CAESound sound;
    const auto slot = (eSoundBankSlot)(uint16)(m_PedSpeechSlotID + SND_BANK_SLOT_SPEECH1);
    if (m_IsFrontend) {
        sound.Initialise(slot, m_SoundID, this, CVector{0.f, 1.f, 0.f}, 3.f, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        sound.m_Flags = SOUND_IS_COMPRESSABLE | SOUND_IS_DUCKABLE | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END; // 0x307
    } else {
        sound.Initialise(slot, m_SoundID, this, m_Entity->GetPosition(), m_EventVolume, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        sound.m_Flags         = SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE; // 6
        sound.m_RollOffFactor = CAEVehicleAudioEntity::s_pPlayerAttachedForRadio ? 3.f : 2.f;
    }
    if (m_IsForcedAudible) {
        sound.m_Volume = 3.f;
    }

    m_Sound = AESoundManager.RequestNewSound(&sound);
    if (m_Sound) {
        SpeechSlot(m_PedSpeechSlotID).Sound  = m_Sound;
        SpeechSlot(m_PedSpeechSlotID).Status = CAEPedSpeechSlot::eStatus::PLAYING;
        switch (m_LastGCtx) {
        case CTX_GLOBAL_BREATHING:
        case CTX_GLOBAL_STOMACH_RUMBLE:
            break;
        default: {
            // Como el original: sin comprobar la inteligencia ni la tarea; segunda expresión NONE con duración 0
            auto* const facial = static_cast<CTaskComplexFacial*>(static_cast<CPed*>(m_Entity)->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX));
            facial->SetRequest(eFacialExpression::TALKING, 2800, eFacialExpression::NONE, 0);
        }
        }
    } else {
        SpeechSlot(m_PedSpeechSlotID).Reset();
        m_IsPlayingSpeech = false;
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            s_bAPlayerSpeaking = false;
        }
        m_PedSpeechSlotID = -1;
    }
}

// 0x4E4120
int16 CAEPedSpeechAudioEntity::GetAllocatedVoice() {
    return m_VoiceID;
}

// 0x4E5800
bool CAEPedSpeechAudioEntity::WillPedChatAboutTopic(int16 topic) {
    switch (topic) {
    case 0:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_CAR);
    case 1:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_CLOTHES);
    case 2:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_HAIR);
    case 3:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_PHYS);
    case 4:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_SHOES);
    case 5:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_SMELL);
    case 6:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_TATTOO);
    case 7:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_WEATHER);
    case 8:
    case 9:  return true;
    default: return false;
    }
}

// 0x4E4130
eAudioPedType CAEPedSpeechAudioEntity::GetPedType() {
    return m_IsInitialized
        ? m_PedAudioType
        : PED_TYPE_UNK;
}

// 0x4E4150
bool CAEPedSpeechAudioEntity::IsPedFemaleForAudio() {
    // Como el original: solo el byte bajo de m_IsFemale
    return m_IsInitialized
        ? (uint8)m_IsFemale != 0
        : false;
}

// 0x4E4950 (CanWePlayScriptedSpeech) y búsqueda en línea de 0x4E5F10
tPedSpeechSlotID CAEPedSpeechAudioEntity::GetFreeSpeechSlot() {
    // Como el original: módulo con signo a partir de s_NextSpeechSlot; el último hueco es siempre del jugador
    const int32 next = s_NextSpeechSlot;
    int16 n = 0, i = 0;
    for (; n < PLAYER_SPEECH_SLOT; n++) {
        i = (int16)(((int32)n + next) % PLAYER_SPEECH_SLOT);
        if (SpeechSlot(i).Status == CAEPedSpeechSlot::eStatus::FREE) {
            break;
        }
    }
    if (n == PLAYER_SPEECH_SLOT) {
        return -1;
    }
    s_NextSpeechSlot = (tPedSpeechSlotID)(((int32)i + 1) % PLAYER_SPEECH_SLOT);
    return i;
}

// 0x4E4470
eSpecificSpeechContext __stdcall CAEPedSpeechAudioEntity::GetSpecificSpeechContext(eGlobalSpeechContext gCtx, eAudioPedType pedAudioType) {
    if (gCtx < 0 || gCtx >= CTX_GLOBAL_NUM || pedAudioType < 0 || pedAudioType >= PED_TYPE_NUM) {
        return -1;
    }
    // Como el original: PED_TYPE_SPC lee la palabra siguiente de la entrada (RepeatTime)
    const auto* const e = ExeSpeechContextEntry(FindExeSpeechContext(gCtx));
    if (e[0] == -1) {
        return -1;
    }
    return e[1 + pedAudioType];
}

// notsa
const tGlobalSpeechContextInfo* CAEPedSpeechAudioEntity::GetGlobalSpeechContextInfo(eGlobalSpeechContext gCtx) {
    assert(gCtx > 0 && gCtx < CTX_GLOBAL_NUM);

    // Must use a loop because there are a few skipped values (TODO: Though I guess we could fix this?)
    for (const auto& e : gSpeechContextLookup) {
        if (e.GCtx == gCtx) {
            return &e;
        }
    }
    return nullptr;
}

// notsa
const tSpecificSpeechContextInfo* CAEPedSpeechAudioEntity::GetSpecificSpeechContextInfo(eSpecificSpeechContext sCtx, eGlobalSpeechContext gCtx, eAudioPedType pt, ePedSpeechVoiceS16 voice) {
    if (IsGlobalContextPain(gCtx)) {
        return &gPainSpeechLookup[sCtx][voice];
    }
    switch (pt) {
    case PED_TYPE_GEN:    return &gGenSpeechLookup[sCtx][voice];
    case PED_TYPE_EMG:    return &gEmgSpeechLookup[sCtx][voice];
    case PED_TYPE_PLAYER: return &gPlySpeechLookup[sCtx][voice];
    case PED_TYPE_GANG:   return &gGngSpeechLookup[sCtx][voice];
    case PED_TYPE_GFD:    return &gGfdSpeechLookup[sCtx][voice];
    default:              return nullptr;
    }
}

eSoundBank CAEPedSpeechAudioEntity::GetVoiceSoundBank(eGlobalSpeechContext gCtx, eAudioPedType pt, ePedSpeechVoiceS16 voice) {
    if (IsGlobalContextPain(gCtx)) {
        return gPainVoiceToBankLookup[voice];
    }
    switch (pt) {
    case PED_TYPE_GEN:    return gGenSpeechVoiceToBankLookup[voice];
    case PED_TYPE_EMG:    return gEmgSpeechVoiceToBankLookup[voice];
    case PED_TYPE_PLAYER: return gPlySpeechVoiceToBankLookup[voice];
    case PED_TYPE_GANG:   return gGngSpeechVoiceToBankLookup[voice];
    case PED_TYPE_GFD:    return gGfdSpeechVoiceToBankLookup[voice];
    default:              NOTSA_UNREACHABLE();
    }
}


// The templated functions below must be here because
// due to a cyclic dependency with `CPed` they can't be defined in the header

// 0x4E6550 / 0x4E60D0
template<bool IsPedless>
int16 CAEPedSpeechAudioEntity::I_AddSayEvent(CVector pos, eAudioEvents audioEvent, eGlobalSpeechContext gCtx, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    if (!m_IsInitialized || s_bAllSpeechDisabled || m_IsSpeechForScriptsDisabled) {
        return -1;
    }

    if (TheCamera.m_bWideScreenOn && !IsGlobalContextImportantForWidescreen(gCtx)) {
        return -1;
    }

    if constexpr (!IsPedless) {
        if (gCtx == CTX_GLOBAL_CHAT) { // 0x4E65BA
            if (CTimer::GetFrameCounter() % 64 || s_bPedConversationHappening || s_bPlayerConversationHappening) {
                return -1;
            }
        } else if (gCtx == CTX_GLOBAL_PAIN_CJ_DROWNING && CWeather::UnderWaterness < 0.5f)  {
            return -1;
        }
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

    if constexpr (IsPedless) {
        if (m_IsPlayingSpeech) { // 0x4E616C
            return -1;
        }
        if (gCtx < CTX_GLOBAL_POLICE_BOAT || gCtx > CTX_GLOBAL_POLICE_OVERBOARD) { // 0x4E6176
            return -1;
        }
    } else { // 0x4E6638
        const auto IsContextImportant = [](eGlobalSpeechContext c) {
            return IsGlobalContextPain(c) || IsGlobalContextImportantForInterupting(c);
            };
        if (IsContextImportant(gCtx)) {
            if (const auto* const speech = GetCurrentSpeech()) {
                if (speech->GCtx != CTX_GLOBAL_PAIN_CJ_HIGH_FALL && IsContextImportant(speech->GCtx)) {
                    return -1;
                }
            }
            StopCurrentSpeech();
        } else if (m_IsPlayingSpeech) { // 0x4E664D
            return -1;
        }

        if (!m_Entity->AsPed()->IsAlive() && !IsGlobalContextPain(gCtx)) { // 0x4E66A7
            return -1;
        }
    }

    if (IsGlobalContextImportantForStreaming(gCtx) || isForceAudible || isFrontEnd) {
        if (CStreaming::ms_numModelsRequested > 15 && CAEVehicleAudioEntity::s_pPlayerAttachedForRadio) {
            return -1;
        }
    } else if (CStreaming::IsVeryBusy()) {
        return -1;
    }

    if constexpr (!IsPedless) {
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            if (FindPlayerPed() != m_Entity || s_bAPlayerSpeaking && !m_IsPlayingSpeech) {
                return -1;
            }
        }
    }

    if (CVector::DistSqr(TheCamera.GetPosition(), pos) >= sq(IsPedless ? 60.f : 40.f)) {
        return -1;
    }

    eSpecificSpeechContext sCtx;
    const auto soundID = CAEPedSpeechAudioEntity::GetSoundAndBankIDs(gCtx, sCtx);
    if (soundID == -1) {
        return -1;
    }

    const auto speechSlotID = !IsPedless && m_PedAudioType == PED_TYPE_PLAYER
        ? CTimer::GetTimeInMS() >= GetNextPlayTime(gCtx)
        ? PLAYER_SPEECH_SLOT
        : -1
        : CanWePlayGlobalSpeechContext(gCtx);
    if (speechSlotID == -1) {
        return -1;
    }

    SetNextPlayTime(gCtx);
    m_IsForcedAudible = isForceAudible;

    m_IsFrontend = isFrontEnd;
    if constexpr (!IsPedless) {
        if (const auto veh = m_Entity->AsPed()->GetVehicleIfInOne()) {
            if (veh == FindPlayerVehicle() && gCtx != CTX_GLOBAL_STOMACH_RUMBLE) {
                m_IsFrontend = true;
            }
        }
    }

    m_EventVolume = GetDefaultVolume(AE_SPEECH_PED);
    if constexpr (!IsPedless) {
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            m_EventVolume += 3.f;
        }
        m_EventVolume += GetSpeechContextVolumeOffset(gCtx);
    }

    m_IsPlayingSpeech = true;
    if constexpr (!IsPedless) {
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            s_bAPlayerSpeaking = true;
        }
    }

    m_LastGCtx        = gCtx;
    m_PedSpeechSlotID = speechSlotID;
    LoadAndPlaySpeech(startTimeDelayMs);

    return soundID;
}

template
int16 CAEPedSpeechAudioEntity::I_AddSayEvent<true>(CVector pos, eAudioEvents audioEvent, eGlobalSpeechContext gCtx, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd);

// 0x4E3520 / 0x4E4D10
template<bool IsPedless>
void CAEPedSpeechAudioEntity::I_UpdateParameters(CAESound* sound, int16 playTime) {
    auto* const ped = IsPedless ? nullptr : m_Entity->AsPed();
    auto* const speech = GetCurrentSpeech();
    if (playTime == -1) { // Sound has finished?
        *speech = {
            .Status = CAEPedSpeechSlot::eStatus::FREE
        };

        m_Sound           = nullptr;
        m_SoundID         = -1;
        m_BankID          = SND_BANK_UNK;
        m_IsForcedAudible = false;
        m_IsPlayingSpeech = false;
        m_PedSpeechSlotID = -1;

        if constexpr (!IsPedless) {
            if (m_PedAudioType == PED_TYPE_PLAYER) {
                s_bAPlayerSpeaking = false;
            }

            if (s_bPedConversationHappening) {
                if (   this == &s_pConversationPed1->GetSpeechAE() && m_PedSpeechSlotID == s_pConversationPedSlot1
                    || this == &s_pConversationPed2->GetSpeechAE() && m_PedSpeechSlotID == s_pConversationPedSlot2
                ) {
                    speech->Status = CAEPedSpeechSlot::eStatus::RESERVED;
                }
            } else if (s_bPlayerConversationHappening) {
                if (this == &s_pPlayerConversationPed->GetSpeechAE() && m_PedSpeechSlotID == s_pConversationPedSlot1) {
                    speech->Status = CAEPedSpeechSlot::eStatus::RESERVED;
                }
            }

            if (auto* const tFacial = notsa::cast<CTaskComplexFacial>(m_Entity->AsPed()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX))) {
                tFacial->StopAll();
            }
        }
    } else { // Update sound
        speech->Status = CAEPedSpeechSlot::eStatus::PLAYING;
        if constexpr (IsPedless) {
            if (!m_IsFrontend || sound->m_Event == AE_SPEECH_PED) {
                sound->SetPosition(m_Entity->GetPosition());
            }
        } else {
            if (ped->bIsTalking && ped->m_nBodypartToRemove == PED_NODE_HEAD) {
                sound->StopSound();
            } else if (!m_IsFrontend) {
                sound->SetPosition(m_Entity->GetPosition());
            }
        }
    }
}

template
void CAEPedSpeechAudioEntity::I_UpdateParameters<true>(CAESound* sound, int16 playTime);

// 0x4E5CD0 / 0x4E6380
template<bool IsPedless>
void CAEPedSpeechAudioEntity::I_PlayLoadedSound(CEntity* attachTo) {
    if (!attachTo) {
        // Done by the caller (No other solution)
        // if constexpr (IsPedless) {
        //     StopCurrentSpeech()
        // }
        return;
    }

    const auto DoPlaySound = [&](CVector pos, uint32 flags, float volume, float maxDist) {
        m_Sound = AESoundManager.PlaySound({
            .BankSlotID         = (eSoundBankSlot)(SND_BANK_SLOT_SPEECH_FIRST + m_PedSpeechSlotID),
            .SoundID            = m_SoundID,
            .AudioEntity        = this,
            .Pos                = pos,
            .Volume             = m_IsForcedAudible ? 3.f : volume,
            .RollOffFactor      = maxDist,
            .Speed              = 1.f,
            .Doppler            = 1.f,
            .FrameDelay         = 0,
            .Flags              = flags,
            .RegisterWithEntity = IsPedless ? attachTo : nullptr
        });
    };

    if (m_IsFrontend) {
        DoPlaySound(
            {0.f, 1.f, 0.f},
            SOUND_IS_COMPRESSABLE | SOUND_IS_DUCKABLE | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
            3.f,
            1.f
        );
    } else {
        DoPlaySound(
            attachTo->GetPosition(),
            SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE,
            IsPedless ? 12.f : m_EventVolume,
            IsPedless ? 3.5f : CAEVehicleAudioEntity::s_pPlayerAttachedForRadio ? 3.f : 2.f
        );
    }

    auto* const speech = GetCurrentSpeech();
    if (m_Sound) {
        speech->Sound = m_Sound;
        speech->Status = CAEPedSpeechSlot::eStatus::PLAYING;
        if constexpr (!IsPedless) {
            switch (m_LastGCtx) {
            case CTX_GLOBAL_BREATHING:
            case CTX_GLOBAL_STOMACH_RUMBLE:
                break;
            default: {
                notsa::cast<CTaskComplexFacial>(m_Entity->AsPed()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX))->SetRequest(
                    eFacialExpression::TALKING,
                    2800
                );
            }
            }
        }
    } else {
        *speech = CAEPedSpeechSlot{};
        if constexpr (!IsPedless) {
            if (m_PedAudioType == PED_TYPE_PLAYER) {
                s_bAPlayerSpeaking = false;
            }
        }
        m_IsPlayingSpeech = false;
        m_PedSpeechSlotID = -1;
    }
}

template
void CAEPedSpeechAudioEntity::I_PlayLoadedSound<true>(CEntity* attachTo);
