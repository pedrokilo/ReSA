/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEPoliceScannerAudioEntity
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEPoliceScannerAudioEntity.h"

#include "AEAudioHardware.h"

// 0x4E6E00
CAEPoliceScannerAudioEntity::~CAEPoliceScannerAudioEntity() {
    if (s_pPSControlling == this && s_nScannerPlaybackState != STATE_INITIAL) {
        s_bStoppingScanner = true;
        if (s_pSound) {
            s_pSound->StopSoundAndForget();
            s_pSound = nullptr;
        }
        FinishedPlayingScannerDialogue();
    }
}

// 0x5B9C30
void CAEPoliceScannerAudioEntity::StaticInitialise() {
    s_NextNewScannerDialogueTime = 0;
    s_bScannerDisabled           = false;
    s_nScannerPlaybackState      = STATE_INITIAL;
    s_pPSControlling             = nullptr;
    s_pCurrentSlots              = nullptr;
    s_fVolumeOffset              = 0.0f;
}

// 0x4E6E90
void CAEPoliceScannerAudioEntity::Reset() {
    StopScanner(true);
    FinishedPlayingScannerDialogue();
}

// 0x4E71E0
void CAEPoliceScannerAudioEntity::AddAudioEvent(eAudioEvents event, eCrimeType crimeType, const CVector& point) {
    if (event != AE_CRIME_COMMITTED || static_cast<int32>(crimeType) <= 1 || static_cast<int32>(crimeType) >= 23) return;
    std::array<tScannerSlot, 5> first{}, second{};
    first[0] = {static_cast<eSoundBank>(150), static_cast<eSoundID>(reinterpret_cast<const int16*>(0x8C8160)[CAEAudioUtility::GetRandomNumberInRange<int32>(0, 3)])};
    first[1] = {static_cast<eSoundBank>(151), static_cast<eSoundID>(reinterpret_cast<const int16*>(0x8C8168)[static_cast<int32>(crimeType)])};
    const auto* zone = CTheZones::FindSmallestZoneForPosition(point, true);
    if (!zone) return;
    int index = 0;
    for (; index < 194; ++index) {
        if (!memcmp(zone->m_TextLabel, reinterpret_cast<const char*>(0x8C8198) + index * 8, 8) && reinterpret_cast<const int16*>(0x8C87A8)[index] >= 0) break;
    }
    if (index == 194) return;
    if (reinterpret_cast<const uint8*>(0x8C8930)[index]) {
        const float width = float(int32(zone->m_fX2) - int32(zone->m_fX1));
        const float height = float(int32(zone->m_fY2) - int32(zone->m_fY1));
        const float centreX = width * 0.5f + float(zone->m_fX1);
        const float centreY = height * 0.5f + float(zone->m_fY1);
        const float quarterX = width * 0.25f, quarterY = height * 0.25f;
        bool hasY = false;
        if (point.y > centreY + quarterY) { first[2] = {static_cast<eSoundBank>(149), static_cast<eSoundID>(2)}; hasY = true; }
        else if (point.y < centreY - quarterY) { first[2] = {static_cast<eSoundBank>(149), static_cast<eSoundID>(3)}; hasY = true; }
        if (point.x > centreX + quarterX) first[3] = {static_cast<eSoundBank>(149), static_cast<eSoundID>(1)};
        else if (point.x < centreX - quarterX) first[3] = {static_cast<eSoundBank>(149), static_cast<eSoundID>(4)};
        else if (!hasY) first[3] = {static_cast<eSoundBank>(149), static_cast<eSoundID>(0)};
    }
    first[4] = {static_cast<eSoundBank>(147), static_cast<eSoundID>(reinterpret_cast<const int16*>(0x8C87A8)[index])};
    CPed* ped = nullptr;
    for (int32 player = 0; player < 2; ++player) {
        if (reinterpret_cast<uint8*>(FindPlayerWanted(player)) + 0x21C == reinterpret_cast<uint8*>(this)) ped = FindPlayerPed(player);
    }
    if (ped) {
        if (ped->bInVehicle) {
            if (auto* vehicle = ped->m_pVehicle) {
                const int32 type = *reinterpret_cast<const int8*>(reinterpret_cast<const uint8*>(vehicle) + 0x1D4);
                if (type >= 0 && type < 46 && reinterpret_cast<const int16*>(0x8C89F8)[type] >= 0) {
                    second[0] = {static_cast<eSoundBank>(150), static_cast<eSoundID>(7)};
                    second[1] = {static_cast<eSoundBank>(150), static_cast<eSoundID>(type == 25 || type == 29 || type == 30 || type == 37 || type == 41 || type == 34 ? 3 : 1)};
                    if (reinterpret_cast<const uint8*>(0x8C8A54)[type]) {
                        int16 colour = -1;
                        if (vehicle->GetRemapIndex() != -1) colour = 4;
                        else {
                            const uint8 value = vehicle->m_nPrimaryColor;
                            if (value > 0 && value < 127) colour = reinterpret_cast<const int16*>(0x8C8A88)[value];
                        }
                        if (colour >= 0) second[3] = {static_cast<eSoundBank>(148), static_cast<eSoundID>(colour)};
                    }
                    second[4] = {static_cast<eSoundBank>(152), static_cast<eSoundID>(reinterpret_cast<const int16*>(0x8C89F8)[type])};
                }
            }
        } else {
            int id = -1;
            if (ped->GetIntelligence()->GetTaskSwim()) id = 2;
            else if (!ped->GetIntelligence()->GetTaskJetPack()) id = 4;
            if (id != -1) {
                second[0] = {static_cast<eSoundBank>(150), static_cast<eSoundID>(7)};
                second[4] = {static_cast<eSoundBank>(150), static_cast<eSoundID>(id)};
            }
        }
    }
    PlayPoliceScannerDialogue(first.data(), second.data());
}

// 0x4E6BC0
void CAEPoliceScannerAudioEntity::PrepSlots() {
    for (auto i = 0; i < NUM_POLICE_SCANNER_SLOTS; i++) {
        s_SlotState[i] = s_pCurrentSlots[i].IsActive();
    }
}

// 0x4E6CD0
void CAEPoliceScannerAudioEntity::LoadSlots() {
    if (!s_pCurrentSlots) {
        return;
    }

    bool canPlay = true;
    for (auto i = 0; i < NUM_POLICE_SCANNER_SLOTS; i++) {
        const auto slot = (eSoundBankSlot)(SND_BANK_SLOT_SCANNER_FIRST + i);
        auto& currentSlot = s_pCurrentSlots[i];

        if (s_SlotState[i]) {
            if (s_SlotState[i] == 2) {
                bool loaded = AEAudioHardware.IsSoundLoaded(currentSlot.Bank, currentSlot.SoundID, slot);
                if (loaded) {
                    s_SlotState[i] = 3;
                } else {
                    canPlay = false;
                }
            }
        } else if (currentSlot.IsActive()) {
            s_SlotState[i] = 1;
        } else {
            if (!CStreaming::IsVeryBusy()) {
                AEAudioHardware.LoadSound(currentSlot.Bank, currentSlot.SoundID, slot);
                s_SlotState[i] = 2;
            }
            canPlay = false;
        }
    }
    if (canPlay) {
        s_nScannerPlaybackState = FOUR;
    }
}

// 0x4E6DB0
void CAEPoliceScannerAudioEntity::EnableScanner() {
    s_bScannerDisabled = false;
}

// 0x4E71B0
void CAEPoliceScannerAudioEntity::DisableScanner(bool a1, bool bStopSound) {
    s_bScannerDisabled = true;
    if (a1 && s_nScannerPlaybackState != STATE_INITIAL) {
        if (s_pPSControlling) {
            StopScanner(bStopSound);
        }
    }
}

// 0x4E6DC0
void CAEPoliceScannerAudioEntity::StopScanner(bool bStopSound) {
    if (s_nScannerPlaybackState == STATE_INITIAL)
        return;

    s_bStoppingScanner = true;
    if (bStopSound) {
        if (s_pSound) {
            s_pSound->StopSoundAndForget();
            s_pSound = nullptr;
        }
        FinishedPlayingScannerDialogue();
    }
}

// 0x4E6C30
void CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_STOP);
    s_nScannerPlaybackState      = STATE_INITIAL;
    s_pPSControlling             = nullptr;
    s_pCurrentSlots              = nullptr;
    s_bStoppingScanner           = false;
    s_NextNewScannerDialogueTime = s_NextNewScannerDialogueTime + CTimer::GetTimeInMS();
    s_fVolumeOffset              = 0.0f;

    std::ranges::fill(s_SlotState, -1);
    s_SlotState[4] = 1;

    rng::fill(s_ScannerSlotFirst, tScannerSlot{});
    rng::fill(s_ScannerSlotSecond, tScannerSlot{});
}

// 0x4E6F60
void CAEPoliceScannerAudioEntity::PlayLoadedDialogue() {
    int16 i;
    for (i = 0; i < NUM_POLICE_SCANNER_SLOTS; ++i) {
        if (s_SlotState[i] == FIVE) {
            break;
        }
    }

    if (i == NUM_POLICE_SCANNER_SLOTS) {
        i = 0;
        goto LABEL_7;
    }

    if (i >= NUM_POLICE_SCANNER_SLOTS) {
    LABEL_9:
        auto volumeChange = StaticRef<float>(0xB61D54) + s_fVolumeOffset;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_CLICK, volumeChange);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_STOP);
        if (s_nSectionPlaying) {
            FinishedPlayingScannerDialogue();
        } else {
            auto v2 = (int16*)s_ScannerSlotSecond;
            s_pCurrentSlots = s_ScannerSlotSecond;
            s_nSectionPlaying = 1;
            auto v3 = s_SlotState;
            auto v4 = 5;
            do {
                *v3 = *v2 < 0 || v2[1] < 0;
                v2 += 2;
                ++v3;
                --v4;
            } while (v4);
            s_nPlaybackStartTime = 0;
            s_nAbortPlaybackTime = StaticRef<uint32>(0x8C8148) + CTimer::GetTimeInMS();
            s_nScannerPlaybackState = TWO;
        }
    } else {
    LABEL_7:
        while (s_SlotState[i] != 3) {
            if (++i >= 5)
                goto LABEL_9;
        }

        auto volume = GetDefaultVolume(AE_CRIME_COMMITTED) + s_fVolumeOffset;
        CAESound sound;
        sound.Initialise((eSoundBankSlot)(SND_BANK_SLOT_SCANNER_FIRST + i), s_pCurrentSlots[i].SoundID, this, { 0.0, 1.0f, 0.0f }, volume, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
        sound.m_ClientVariable = (float)i;
        sound.m_Flags = SOUND_FRONT_END | SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES | SOUND_IS_DUCKABLE;
        sound.m_Event = AE_CRIME_COMMITTED;

        s_pSound = AESoundManager.RequestNewSound(&sound);
        if (s_pSound) {
            s_SlotState[i] = 5;
            s_nScannerPlaybackState = SEVEN;
        }
    }
}

// 0x4E6B60
void CAEPoliceScannerAudioEntity::PopulateScannerDialogueLists(const tScannerSlot* first, const tScannerSlot* second) {
    assert(first && second);
    if (s_nScannerPlaybackState == STATE_INITIAL) {
        for (auto slotIndex = 0; slotIndex < NUM_POLICE_SCANNER_SLOTS; slotIndex++) {
            s_ScannerSlotFirst[slotIndex] = first[slotIndex];
            s_ScannerSlotSecond[slotIndex] = second[slotIndex];
        }
    }
}

// inlined
// 0x4E6C00
bool CAEPoliceScannerAudioEntity::CanWePlayNewScannerDialogue() {
    if (s_nScannerPlaybackState != STATE_INITIAL)
        return false;

    if (CTimer::GetTimeInMS() < s_NextNewScannerDialogueTime)
        return false;

    if (TheCamera.m_bWideScreenOn)
        return false;

    if (s_bScannerDisabled)
        return false;

    return true;
}

// 0x4E6ED0
void CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue(tScannerSlot* first, tScannerSlot* second) {
    assert(first && second);
    if (CanWePlayNewScannerDialogue()) { // todo: maybe little bit wrong
        PopulateScannerDialogueLists(first, second);
        s_pPSControlling = this;
        s_nSectionPlaying = 0;
        s_pCurrentSlots = s_ScannerSlotFirst;
        PrepSlots();
        s_nScannerPlaybackState = TWO;
        s_nPlaybackStartTime = CTimer::GetTimeInMS() + 2000; // todo: gSpeechContextLookup[365][6]
        s_nAbortPlaybackTime = CTimer::GetTimeInMS() + 5000; // todo: gSpeechContextLookup[366][0]
    }
}

// 0x4E7590
void CAEPoliceScannerAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos == -1) {
        s_pSound = nullptr;
        if (s_bStoppingScanner) {
            if (s_nScannerPlaybackState != STATE_INITIAL) {
                s_bStoppingScanner = true; // V1048 [CWE-1164] The 's_bStoppingScanner' variable was assigned the same value
                FinishedPlayingScannerDialogue();
            }
            return;
        }
        PlayLoadedDialogue();
        return;
    }

    if (s_bStoppingScanner) {
        sound->m_Volume = sound->m_Volume - 6.0f; // todo: *(float*)&gSpeechContextLookup[366][4]
        return;
    }

    if (sound->m_Length > 0 && curPlayPos > sound->m_Length - 40 && sound->m_BankSlot != 37) { // todo: -40 should be replaced with by gSpeechContextLookup[366][2]
        sound->SetFlags(eSoundEnvironment::SOUND_REQUEST_UPDATES, false);
        s_pSound = nullptr;
        PlayLoadedDialogue();
        return;
    }
}

// 0x4E7630
void CAEPoliceScannerAudioEntity::Service() {
    const auto startDelay = StaticRef<uint32>(0x8C8154);
    const auto noiseVolume = StaticRef<float>(0x8C8158);
    const auto clickVolume = StaticRef<float>(0xB61D54);

    bool finishPlaying;
    if (TheCamera.m_bWideScreenOn && s_nScannerPlaybackState != STATE_INITIAL) {
        finishPlaying = true;
        s_bStoppingScanner = true;
    } else {
        finishPlaying = s_bStoppingScanner;
    }

    switch (s_nScannerPlaybackState) {
    case TWO:
        if (CTimer::GetTimeInMS() > s_nAbortPlaybackTime || finishPlaying) {
            FinishedPlayingScannerDialogue();
            break;
        }

        LoadSlots();
        break;
    case FOUR:
        if (finishPlaying) {
            FinishedPlayingScannerDialogue();
            break;
        }

        if (CTimer::GetTimeInMS() >= s_nPlaybackStartTime) {
            s_fVolumeOffset = CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio ? 0.0f : StaticRef<float>(0x8C815C);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_CLICK,       s_fVolumeOffset + clickVolume, 1.0f);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_START, s_fVolumeOffset + noiseVolume, 1.0f); // todo: noiseVolume is gSpeechContextLookup[367][0]
            s_nPlaybackStartTime = startDelay + CTimer::GetTimeInMS(); // todo: startDelay is gSpeechContextLookup[366][6]
            s_nScannerPlaybackState = FIVE;
        }
        break;
    case FIVE:
        if (finishPlaying) {
            FinishedPlayingScannerDialogue();
            break;
        }

        if (CTimer::GetTimeInMS() >= s_nPlaybackStartTime && s_pPSControlling) {
            s_nScannerPlaybackState = SEVEN;
            s_pPSControlling->PlayLoadedDialogue();
        }
        break;
    default:
        return;
    }
}

void CAEPoliceScannerAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPoliceScannerAudioEntity, 0x85F368, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x56DA00);
    RH_ScopedInstall(Destructor, 0x4E6E00);
    RH_ScopedInstall(StaticInitialise, 0x5B9C30);
    RH_ScopedInstall(Reset, 0x4E6E90);
    RH_ScopedInstall(AddAudioEvent, 0x4E71E0);
    RH_ScopedInstall(PrepSlots, 0x4E6BC0);
    RH_ScopedInstall(LoadSlots, 0x4E6CD0);
    RH_ScopedInstall(EnableScanner, 0x4E6DB0);
    RH_ScopedInstall(DisableScanner, 0x4E71B0);
    RH_ScopedInstall(StopScanner, 0x4E6DC0);
    RH_ScopedInstall(FinishedPlayingScannerDialogue, 0x4E6C30);
    RH_ScopedInstall(PlayLoadedDialogue, 0x4E6F60);
    RH_ScopedInstall(PopulateScannerDialogueLists, 0x4E6B60);
    RH_ScopedInstall(CanWePlayNewScannerDialogue, 0x4E6C00);
    RH_ScopedInstall(PlayPoliceScannerDialogue, 0x4E6ED0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E7590);
    RH_ScopedInstall(Service, 0x4E7630);
}

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::Constructor() {
    this->CAEPoliceScannerAudioEntity::CAEPoliceScannerAudioEntity();
    return this;
}

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::Destructor() {
    this->CAEPoliceScannerAudioEntity::~CAEPoliceScannerAudioEntity();
    return this;
}
