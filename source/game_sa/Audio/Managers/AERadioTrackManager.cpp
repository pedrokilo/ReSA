/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAERadioTrackManager
 * Reconstrucción: COMPLETA — 43/43 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 14.
 * Validación registrada: 43/43 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   CheckForStationRetune | 0x004EB660 | S
 *   CheckForTrackConcatenation | 0x004EA930 | S
 *   ChooseAdvertIndex | 0x004E9570 | S
 *   ChooseDJBanterIndex | 0x004EA2D0 | S
 *   ChooseDJBanterIndexFromList | 0x004E95E0 | S
 *   ChooseIdentIndex | 0x004E94C0 | S
 *   ChooseMusicTrackIndex | 0x004EA270 | S
 *   ChooseTalkRadioShow | 0x004E8E40 | S
 *   IsVehicleRadioActive | 0x004E9800 | S
 *   QueueUpTracksForStation | 0x004EA670 | S
 *   Service | 0x004EB9A0 | S
 *   StopRadio | 0x004E9820 | S
 *   TrackRadioStation | 0x004EAC30 | S
 *   UpdateRadioVolumes | 0x004EA010 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AERadioTrackManager.h"
#include "RadioStreamsPC.h"

#include "AEAudioHardware.h"
#include "AEUserRadioTrackManager.h"
#include "AEAudioUtility.h"
#include "AEAudioEnvironment.h"

auto& AERadioTrackManager = StaticRef<CAERadioTrackManager>(0x8CB6F8);

void CAERadioTrackManager::InjectHooks() {
    RH_ScopedClass(CAERadioTrackManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Load, 0x5D40E0);
    RH_ScopedInstall(Save, 0x5D3EE0);
    RH_ScopedInstall(Initialise, 0x5B9390);
    RH_ScopedInstall(Service, 0x4EB9A0);
    RH_ScopedInstall(DisplayRadioStationName, 0x4E9E50);
    RH_ScopedInstall(CheckForStationRetune, 0x4EB660);
    RH_ScopedInstall(CheckForPause, 0x4EA590);
    RH_ScopedInstall(IsVehicleRadioActive, 0x4E9800);
    RH_ScopedInstall(AddDJBanterIndexToHistory, 0x4E97B0);
    RH_ScopedInstall(AddAdvertIndexToHistory, 0x4E9760);
    RH_ScopedInstall(AddIdentIndexToHistory, 0x4E9720);
    RH_ScopedInstall(AddMusicTrackIndexToHistory, 0x4E96C0);
    RH_ScopedOverloadedInstall(StartRadio, "manual", 0x4EB3C0, void (CAERadioTrackManager::*)(eRadioID, eBassSetting, float, bool));
    RH_ScopedOverloadedInstall(StartRadio, "with-settings", 0x4EB550, void (CAERadioTrackManager::*)(const tVehicleAudioSettings&));
    RH_ScopedInstall(CheckForStationRetuneDuringPause, 0x4EB890);
    RH_ScopedInstall(TrackRadioStation, 0x4EAC30);
    RH_ScopedInstall(ChooseTracksForStation, 0x4EB180);
    RH_ScopedInstall(CheckForTrackConcatenation, 0x4EA930);
    RH_ScopedInstall(QueueUpTracksForStation, 0x4EA670);
    RH_ScopedInstall(ChooseDJBanterIndex, 0x4EA2D0);
    RH_ScopedInstall(ChooseDJBanterIndexFromList, 0x4E95E0);
    RH_ScopedInstall(ChooseAdvertIndex, 0x4E9570);
    RH_ScopedInstall(ChooseIdentIndex, 0x4E94C0);
    RH_ScopedInstall(ChooseMusicTrackIndex, 0x4EA270);
    RH_ScopedInstall(ChooseTalkRadioShow, 0x4E8E40);
    RH_ScopedInstall(CheckForMissionStatsChanges, 0x4E8410);
    RH_ScopedInstall(StartTrackPlayback, 0x4EA640);
    RH_ScopedInstall(UpdateRadioVolumes, 0x4EA010);
    RH_ScopedInstall(PlayRadioAnnouncement, 0x4E8400);
    RH_ScopedInstall(GetCurrentRadioStationID, 0x4E83F0);
    RH_ScopedInstall(GetRadioStationListenTimes, 0x4E83E0);
    RH_ScopedInstall(GetRadioStationName, 0x4E9E10);
    RH_ScopedInstall(GetRadioStationNameKey, 0x4E8380);
    RH_ScopedInstall(HasRadioRetuneJustStarted, 0x4E8370);
    RH_ScopedInstall(StopRadio, 0x4E9820);
    RH_ScopedInstall(IsRadioOn, 0x4E8350, { .reversed = true });
    RH_ScopedInstall(InitialiseRadioStationID, 0x4E8330);
    RH_ScopedInstall(SetBassEnhanceOnOff, 0x4E9DB0);
    RH_ScopedInstall(SetBassSetting, 0x4E82F0);
    RH_ScopedInstall(SetRadioAutoRetuneOnOff, 0x4E82E0);
    RH_ScopedInstall(RetuneRadio, 0x4E8290, { .reversed = true });
    RH_ScopedInstall(ResetStatistics, 0x4E8200);
    RH_ScopedInstall(Reset, 0x4E7F80, { .reversed = true });
}

// Code from 0x5B9390
CAERadioTrackManager::CAERadioTrackManager(int32 hwClientHandle) :
    m_HwClientHandle{ hwClientHandle },
    m_nUserTrackPlayMode{ AEUserRadioTrackManager.GetUserTrackPlayMode() }
{
    // All constant value inits are done using member init lists

    rng::copy(CStats::GetFullFavoriteRadioStationList(), m_aListenTimes.begin());

    for (auto i = 0u; i < RADIO_COUNT; i++) {
        m_nMusicTrackIndexHistory[i].Reset();
        m_nDJBanterIndexHistory[i].Reset();
        m_nAdvertIndexHistory[i].Reset();
        m_nIdentIndexHistory[i].Reset();
    }

    // [1st radio, off]
    m_RequestedSettings = m_ActiveSettings = tRadioSettings{CAEAudioUtility::GetRandomRadioStation()};
}

// Cuerpo comun de Reset (0x4E7F80) e Initialise (0x5B9390): ambas escriben exactamente los mismos campos
// (sin tocar los que el EXE no toca: BassGain pedido, relleno de tRadioSettings, m_HwClientHandle en Reset...).
static void ResetCommon(CAERadioTrackManager& self) {
    // Como el original: 0x4E7F8B [ebp]=0, [ebp+1]=0
    self.m_bInitialised = false;
    self.m_bDisplayStationName = false;
    // Como el original: 0x4E7F91 call 0x558F90 + rep movsd (14 dwords) a [ebp+18h]
    rng::copy(CStats::GetFullFavoriteRadioStationList(), self.m_aListenTimes.begin());

    // Como el original: 0x4E7FD0..0x4E806D (bucle de 14 emisoras): historiales a -1, estado de radio y m_nTracksInARow=0
    for (auto i = 0u; i < RADIO_COUNT; i++) {
        CAERadioTrackManager::m_nMusicTrackIndexHistory[i].Reset();
        CAERadioTrackManager::m_nIdentIndexHistory[i].Reset();
        CAERadioTrackManager::m_nAdvertIndexHistory[i].Reset();
        CAERadioTrackManager::m_nDJBanterIndexHistory[i].Reset();
        self.m_aRadioState[i].Reset();
        self.m_nTracksInARow[i] = 0;
    }

    // Como el original: 0x4E8073 call 0x4D9C10 (GetRandomNumberInRange(1, 13)) -> [ebp+0ADh]; 0x4E808B/0x4E8091/0x4E80A5...
    // Solo se escriben los campos del EXE sobre m_RequestedSettings (el BassGain pedido y el relleno quedan como estaban) ...
    auto& rs = self.m_RequestedSettings;
    rs.StationID = CAEAudioUtility::GetRandomRadioStation();
    rs.BassSetting = eBassSetting::NORMAL;
    rs.TrackFlags = 2;
    for (size_t k = 0; k < tRadioSettings::NUM_TRACKS; k++) {
        rs.TrackQueue.data()[k] = -1;
        rs.TrackTypes.data()[k] = TYPE_NONE;
        rs.TrackIndices.data()[k] = -1;
    }
    rs.CurrTrackID = -1;     // 0x4E80B5
    rs.PrevTrackID = -1;     // 0x4E80BB
    rs.PlayTime = 0;         // 0x4E80C1
    rs.TrackLengthMs = 0;    // 0x4E80C7
    rs.CurrTrackType = TYPE_NONE; // 0x4E80CD
    rs.CurrTrackIdx = -1;    // 0x4E80D4
    rs.PrevTrackType = TYPE_NONE; // 0x4E80DB
    rs.PrevTrackIdx = -1;    // 0x4E80E2

    self.m_nStationsListed = 0;          // 0x4E80E9
    self.m_nStationsListDown = 0;        // 0x4E80EC
    self.m_nTimeRadioStationRetuned = 0; // 0x4E80EF
    self.m_nTimeToDisplayRadioName = 0;  // 0x4E80F2
    self.m_nSavedTimeMs = 0;             // 0x4E80F5 (mov [ebp+58h],ebx): el fuente no lo ponia a 0
    self.m_prev = 0;                     // 0x4E80F8
    self.field_60 = 0;                   // 0x4E80FB
    self.m_nRetuneStartedTime = 0;       // 0x4E80FE
    self.m_bEnabledInPauseMode = false;  // 0x4E8101
    self.m_nSavedGameClockDays = 0xFF;   // 0x4E8104
    self.m_nSavedGameClockHours = 0xFF;  // 0x4E8108
    self.m_bRadioAutoSelect = true;      // 0x4E810C
    self.m_bBassEnhance = true;          // 0x4E8110
    // Como el original: 0x4E8114..0x4E811F rep movsd de 15 dwords: ActiveSettings = copia COMPLETA (con relleno) de RequestedSettings
    std::memcpy(&self.m_ActiveSettings, &self.m_RequestedSettings, sizeof(tRadioSettings));
    self.m_nSavedRadioStationId = -1;       // 0x4E8124
    self.m_iRadioStationMenuRequest = -1;   // 0x4E8127
    self.m_iRadioStationScriptRequest = -1; // 0x4E812A
    CAERadioTrackManager::m_nSpecialDJBanterPending = 3;     // 0x4E812E
    CAERadioTrackManager::m_nSpecialDJBanterIndex = 0xFF;    // 0x4E8135
    self.m_bPauseMode = false;       // 0x4E813C
    self.m_bRetuneJustStarted = false; // 0x4E813F
    self.m_f80 = 0.0f;               // 0x4E8142
    self.m_f84 = 0.0f;               // 0x4E8148
    CAERadioTrackManager::ResetStatistics(); // 0x4E8150..0x4E81E8 (inline en el EXE)
}

// 0x5B9390
// Como el original: NO es "*this = CAERadioTrackManager{}". Es el cuerpo del constructor (guarda el handle, modo 7 = STOPPED,
// mismos campos que Reset, field_368 = 0 y el modo de reproduccion de pistas de usuario).
bool CAERadioTrackManager::Initialise(int32 channelId) {
    m_HwClientHandle = channelId;                      // 0x5B939F
    m_nMode = eRadioTrackMode::RADIO_STOPPED;          // 0x5B93A2
    ResetCommon(*this);
    field_368 = 0;                                     // 0x5B9611
    m_nUserTrackPlayMode = AEUserRadioTrackManager.GetUserTrackPlayMode(); // 0x5B960C call 0x4F3330 -> [ebp+36Ch]
    return true;
}

// 0x4E8330
void CAERadioTrackManager::InitialiseRadioStationID(eRadioID id) {
    m_RequestedSettings.StationID = m_ActiveSettings.StationID = id;
}

// 0x4E7F80
void CAERadioTrackManager::Reset() {
    ResetCommon(*this);
}

// 0x4E8200
void CAERadioTrackManager::ResetStatistics() {
    m_nStatsCitiesPassed = 0;
    m_nStatsLastHitGameClockDays = -1;
    m_nStatsLastHitGameClockHours = -1;
    m_nStatsLastHitTimeOutHours = -1;
    m_nStatsPassedCasino3 = false;
    m_nStatsPassedCasino6 = false;
    m_nStatsPassedCasino10 = false;
    m_nStatsPassedCat1 = false;
    m_nStatsPassedDesert1 = false;
    m_nStatsPassedDesert3 = false;
    m_nStatsPassedDesert5 = false;
    m_nStatsPassedDesert8 = false;
    m_nStatsPassedDesert10 = false;
    m_nStatsPassedFarlie3 = false;
    m_nStatsPassedLAFin2 = false;
    m_nStatsPassedMansion2 = false;
    m_nStatsPassedRyder2 = false;
    m_nStatsPassedRiot1 = false;
    m_nStatsPassedSCrash1 = false;
    m_nStatsPassedStrap4 = false;
    m_nStatsPassedSweet2 = false;
    m_nStatsPassedTruth2 = false;
    m_nStatsPassedVCrash2 = false;
    m_nStatsStartedBadlands = false;
    m_nStatsStartedCat2 = false;
    m_nStatsStartedCrash1 = false;
}

// 0x4E8350
bool CAERadioTrackManager::IsRadioOn() const {
    return m_nMode != eRadioTrackMode::RADIO_STOPPED || m_bInitialised || m_nStationsListed || m_nStationsListDown;
}

// 0x4E8370
bool CAERadioTrackManager::HasRadioRetuneJustStarted() const {
    return m_bRetuneJustStarted;
}

// 0x4E83E0
int32* CAERadioTrackManager::GetRadioStationListenTimes() {
    return m_aListenTimes.data();
}

// 0x4E83F0
eRadioID CAERadioTrackManager::GetCurrentRadioStationID() const {
    // Como el original: 0x4E83F0 mov al,[ecx+0E9h] -> es m_ActiveSettings.StationID (0xC4+0x25), NO el pedido (0xAD).
    return m_ActiveSettings.StationID == RADIO_INVALID ? RADIO_OFF : m_ActiveSettings.StationID;
}

// 0x4E82E0
void CAERadioTrackManager::SetRadioAutoRetuneOnOff(bool enable) {
    m_bRadioAutoSelect = enable;
}

// 0x4E82F0
void CAERadioTrackManager::SetBassSetting(eBassSetting bassSetting, float bassGrain) {
    m_RequestedSettings.BassGain = m_ActiveSettings.BassGain = bassGrain;
    m_RequestedSettings.BassSetting = m_ActiveSettings.BassSetting = bassSetting;
    AEAudioHardware.SetBassSetting(m_bBassEnhance ? bassSetting : eBassSetting::NORMAL, bassGrain);
}

// 0x4E9DB0
void CAERadioTrackManager::SetBassEnhanceOnOff(bool enable) {
    m_bBassEnhance = enable;
    if (m_nMode == eRadioTrackMode::RADIO_PLAYING) {
        m_RequestedSettings.BassSetting = m_ActiveSettings.BassSetting;
        m_RequestedSettings.BassGain = m_ActiveSettings.BassGain;
        if (enable) {
            AEAudioHardware.SetBassSetting(m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain);
        } else {
            AEAudioHardware.SetBassSetting(eBassSetting::NORMAL, m_ActiveSettings.BassGain);
        }
    }
}

// 0x4E8290
void CAERadioTrackManager::RetuneRadio(eRadioID id) {
    const auto retunedStation = [id] {
        if (id == RADIO_USER_TRACKS && !AEUserRadioTrackManager.m_nUserTracksCount) {
            return RADIO_OFF;
        } else {
            return id;
        }
    }();

    if (CTimer::GetIsPaused()) {
        m_iRadioStationMenuRequest = retunedStation;
        m_nRetuneStartedTime = CTimer::GetTimeInMSPauseMode();
    } else {
        m_iRadioStationScriptRequest = retunedStation;
    }
}

// 0x4E9E50
void CAERadioTrackManager::DisplayRadioStationName() {
    if (CTimer::GetIsPaused())
        return;

    if (TheCamera.m_bWideScreenOn)
        return;

    if (!FindPlayerVehicle())
        return;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (m_bDisplayStationName && IsVehicleRadioActive()) {
        m_nTimeToDisplayRadioName = CTimer::GetTimeInMS() + 2500;
        m_bDisplayStationName = false;
    }

    if (CTimer::GetTimeInMS() < m_nTimeToDisplayRadioName) {
        int station = m_nStationsListed + m_RequestedSettings.StationID;
        if (station) {
            if (station >= RADIO_COUNT) {
                station -= RADIO_COUNT - 1;
            } else if (station <= 0) {
                station += RADIO_COUNT - 1;
            }

            // Como el original: 0x4E9EF3 GetRadioStationName se llama ANTES de los CFont::Set*.
            const auto* const name = GetRadioStationName((eRadioID)station);

            // Como el original: 0x4E9F11..0x4E9F3C: escala = (alto * K448) * 0.9 y (ancho * K640) * 0.6 con K448 = 0x3B124925 y
            // K640 = 0x3ACCCCCD (constantes [859524]/[859520]); no usa SCREEN_SCALE_X/Y (division ni factor de aspecto).
            const float kHeight = std::bit_cast<float>(0x3B124925u);
            const float kWidth = std::bit_cast<float>(0x3ACCCCCDu);
            const float scaleY = (float(RsGlobal.maximumHeight) * kHeight) * 0.9f;
            const float scaleX = (float(RsGlobal.maximumWidth) * kWidth) * 0.6f;

            CFont::SetFontStyle(eFontStyle::FONT_MENU);
            CFont::SetJustify(false);
            CFont::SetBackground(false, false);
            CFont::SetScale(scaleX, scaleY);
            CFont::SetProportional(true);
            CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
            CFont::SetRightJustifyWrap(0.0f);
            CFont::SetEdge(1);
            CFont::SetDropColor(CRGBA(0, 0, 0, 255));
            CFont::SetColor(HudColour.GetRGB(m_nStationsListed || m_nStationsListDown ? HUD_COLOUR_DARK_GRAY : HUD_COLOUR_GOLD));
            // Como el original: 0x4E9FBF..0x4E9FF6: x = float(ancho / 2) (division ENTERA: cdq/sub/sar), y = (alto * K448) * 22.0.
            CFont::PrintString(float(RsGlobal.maximumWidth / 2), (float(RsGlobal.maximumHeight) * kHeight) * 22.0f, name);
            CFont::DrawFonts();
        }
    }
}

// 0x4E9E10
const GxtChar* CAERadioTrackManager::GetRadioStationName(eRadioID id) {
    // Como el original: 0x4E9E17..0x4E9E19 test al,al / jle -> xor eax,eax (devuelve NULL, no aborta).
    if (id <= 0) {
        return nullptr;
    }

    char key[8];
    GetRadioStationNameKey(id, key);
    return TheText.Get(key);
}

// 0x4E8380
void CAERadioTrackManager::GetRadioStationNameKey(eRadioID id, char* outStr) {
    switch (id) {
    case RADIO_OFF:
        *std::format_to_n(outStr, 7u, "FEA_NON").out = '\0';
        break;
    case RADIO_USER_TRACKS:
        *std::format_to_n(outStr, 7u, "FEA_MP3").out = '\0';
        break;
    default:
        assert(0 <= id && id < RADIO_USER_TRACKS);
        *std::format_to_n(outStr, 7u, "FEA_R{:d}", (int32)id - 1).out = '\0';
        break;
    }
}

// 0x4E9800
bool CAERadioTrackManager::IsVehicleRadioActive() {
    if (const auto opts = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio()) {
        switch (opts->RadioType) {
        case AE_RT_CIVILIAN:
        case AE_RT_EMERGENCY:
        case AE_RT_UNKNOWN:
            return true;
        default:
            break;
        }
    }
    return false;
}

// 0x4E8410
void CAERadioTrackManager::CheckForMissionStatsChanges() {
    // Como el original: los indicadores (B62C58..B62C73) se leen con movsx/movzx exactos y las estadisticas se comparan
    // como float (fild + fcompp + test ah,5 + jp) y se convierten con _ftol2 (truncado de 64 bits, byte bajo).
    if (m_nSpecialDJBanterPending != 3) {
        // 0x4E841A..0x4E8443: movzx dias_actuales - movsx m_nStatsLastHitGameClockDays
        int32 days = int32(CClock::GetGameClockDays()) - int32(int8(m_nStatsLastHitGameClockDays));
        if (days < 0) {
            // 0x4E842C..0x4E8441: dec al (byte) / jns / add al,0Ch; movsx; movzx [daysInMonth + mes]
            int8 month = int8(uint8(CClock::GetGameClockMonth()) - 1);
            if (month < 0) {
                month = int8(month + 12);
            }
            days += StaticRef<uint8>(0x8CCF24 + size_t(int32(month)));
        }
        // 0x4E8443..0x4E8464: horas = dias*24 - movsx(horas guardadas) + movzx(horas actuales); jl con signo contra movzx(limite)
        if (days * 24 - int32(int8(m_nStatsLastHitGameClockHours)) + int32(CClock::GetGameClockHours()) >= int32(m_nStatsLastHitTimeOutHours)) {
            m_nSpecialDJBanterPending = 3;
        }
    }

    // 0x4E846D..0x4E84DE: ciudades. El EXE llama dos veces a CStats::GetStatValue (0x558E40) cuando entra.
    {
        const float value = CStats::GetStatValue(STAT_CITY_UNLOCKED);
        // 0x4E8489..0x4E8490: fcompp + test ah,5 + jp salta con NaN (desordenado) o guardado >= valor: solo entra si guardado < valor
        if (float(int8(m_nStatsCitiesPassed)) < value) {
            const uint8 passed = uint8(int64(CStats::GetStatValue(STAT_CITY_UNLOCKED)));
            m_nStatsCitiesPassed = passed;
            if (passed == 1 || passed == 2) {
                m_nStatsLastHitGameClockDays = CClock::GetGameClockDays();
                m_nStatsLastHitGameClockHours = CClock::GetGameClockHours();
                m_nStatsLastHitTimeOutHours = 24;
                m_nSpecialDJBanterPending = 1;
                m_nSpecialDJBanterIndex = uint8(passed - 1);
            }
        }
    }

    const auto Update = [](uint8& inputStat, const eStats stat, const uint8 specialDJBanterIndex) {
        const float value = CStats::GetStatValue(stat);
        // fcompp + test ah,5 + jp: con NaN el EXE salta (no entra)
        if (float(int8(inputStat)) < value) {
            inputStat = uint8(int64(CStats::GetStatValue(stat)));
            if (inputStat == 1) {
                m_nStatsLastHitGameClockDays = CClock::GetGameClockDays();
                m_nStatsLastHitGameClockHours = CClock::GetGameClockHours();
                m_nStatsLastHitTimeOutHours = 24 * 7;
                m_nSpecialDJBanterPending = 2;
                m_nSpecialDJBanterIndex = specialDJBanterIndex;
            }
        }
    };

    Update(m_nStatsPassedCasino3, STAT_LEAST_FAVORITE_RADIO_STATION, 0);
    Update(m_nStatsPassedCasino6, STAT_CURRENT_WEAPON_SKILL, 1);
    Update(m_nStatsPassedCasino10, STAT_WEAPON_SKILL_LEVELS, 2);
    Update(m_nStatsPassedCat1, STAT_LOCAL_LIQUOR_STORE_MISSION_ACCOMPLISHED, 3);
    Update(m_nStatsPassedDesert1, STAT_PLAYING_TIME, 4);
    Update(m_nStatsPassedDesert3, STAT_PILOT_RANKING, 5);
    Update(m_nStatsPassedDesert5, STAT_STRONGEST_GANG, 6);
    Update(m_nStatsPassedDesert8, STAT_2ND_STRONGEST_GANG, 7);
    Update(m_nStatsPassedDesert10, STAT_3RD_STRONGEST_GANG, 8);
    Update(m_nStatsPassedFarlie3, STAT_MIKE_TORENO_MISSION_ACCOMPLISHED, 9);
    Update(m_nStatsPassedLAFin2, STAT_LEAST_FAVORITE_GANG, 10);
    Update(m_nStatsPassedMansion2, STAT_A_HOME_IN_THE_HILLS_MISSION_ACCOMPLISHED, 11);
    Update(m_nStatsPassedRyder2, STAT_RYDERS_MISSION_ROBBING_UNCLE_SAM_ACCOMPLISHED, 12);
    Update(m_nStatsPassedRiot1, STAT_RIOT_MISSION_ACCOMPLISHED, 13);
    Update(m_nStatsPassedSCrash1, STAT_GANG_STRENGTH, 14);
    Update(m_nStatsPassedStrap4, STAT_TERRITORY_UNDER_CONTROL, 15);
    Update(m_nStatsPassedSweet2, STAT_DRIVE_THRU_MISSION_ACCOMPLISHED, 16);
    Update(m_nStatsPassedTruth2, STAT_ARE_YOU_GOING_TO_SAN_FIERRO_MISSION_ACCOMPLISHED, 17);
    Update(m_nStatsPassedVCrash2, STAT_HIGH_NOON_MISSION_ACCOMPLISHED, 18);
    Update(m_nStatsStartedBadlands, STAT_THE_GREEN_SABRE_MISSION_ACCOMPLISHED, 19);
    Update(m_nStatsStartedCat2, STAT_MAYBE_CATALINA_MEETING, 20);
    Update(m_nStatsStartedCrash1, STAT_MAYBE_WU_ZI_MEETING, 21);
}

// 0x4EA930
void CAERadioTrackManager::CheckForTrackConcatenation() {
    int8 count = 1;
    int8 userIndex = 1;
    auto& settings = m_ActiveSettings;
    if (settings.StationID == static_cast<eRadioID>(12)) {
        const auto savedMode = m_nUserTrackPlayMode;
        if (savedMode != AEUserRadioTrackManager.GetUserTrackPlayMode()) {
            if ((savedMode == 2 || AEUserRadioTrackManager.GetUserTrackPlayMode() == 2) && settings.PlayTime != -4) {
                AEUserRadioTrackManager.SetUserTrackIndex(settings.TrackQueue[0]);
                settings.TrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
                settings.TrackTypes[1] = TYPE_USER_TRACK;
                settings.TrackIndices[1] = int8(settings.TrackQueue[1]);
                userIndex = 2;
                AEAudioHardware.PlayTrack(uint32(settings.TrackQueue[0]), settings.TrackQueue[1], 0, uint8(settings.TrackFlags), settings.TrackTypes[0] == TYPE_USER_TRACK, settings.TrackTypes[1] == TYPE_USER_TRACK);
            }
            m_nUserTrackPlayMode = AEUserRadioTrackManager.GetUserTrackPlayMode();
        }
    }
    const auto next = settings.TrackQueue[1];
    if (AEAudioHardware.GetActiveTrackID() != next || next < 0) return;
    settings.SwitchToNextTrack();
    if (settings.TrackQueue[1] == -1) {
        const auto station = settings.StationID;
        if (station == static_cast<eRadioID>(12)) {
            if (!StaticRef<uint8>(0xBA67F8) && CAEAudioUtility::ResolveProbability(std::bit_cast<float>(0x3E2E147Bu))) {
                settings.TrackQueue[size_t(userIndex)] = ChooseAdvertIndex(station);
                settings.TrackTypes[size_t(userIndex)] = TYPE_ADVERT;
                ++userIndex;
            }
            settings.TrackQueue[size_t(userIndex)] = AEUserRadioTrackManager.SelectUserTrackIndex();
            settings.TrackTypes[size_t(userIndex)] = TYPE_USER_TRACK;
            settings.TrackIndices[size_t(userIndex)] = int8(settings.TrackQueue[size_t(userIndex)]);
            ++userIndex;
            settings.TrackQueue[size_t(userIndex)] = AEUserRadioTrackManager.SelectUserTrackIndex();
            settings.TrackTypes[size_t(userIndex)] = TYPE_USER_TRACK;
            settings.TrackIndices[size_t(userIndex)] = int8(settings.TrackQueue[size_t(userIndex)]);
        } else if (settings.TrackTypes[0] == TYPE_INTRO || settings.TrackTypes[0] == TYPE_TRACK || settings.TrackTypes[0] == TYPE_OUTRO) {
            if (station == static_cast<eRadioID>(0)) {
                QueueUpTracksForStation(station, &count, 2, settings);
            } else if (int8(m_nTracksInARow[size_t(int8(station))]) < 2 && CAEAudioUtility::ResolveProbability(.5f)) {
                if (CAEAudioUtility::ResolveProbability(.5f)) QueueUpTracksForStation(station, &count, 0, settings);
                QueueUpTracksForStation(station, &count, 3, settings);
            } else {
                if (CAEAudioUtility::ResolveProbability(.5f)) QueueUpTracksForStation(station, &count, 0, settings);
                if (!QueueUpTracksForStation(station, &count, 2, settings)) QueueUpTracksForStation(station, &count, 1, settings);
            }
        } else QueueUpTracksForStation(station, &count, 3, settings);
    }
    AEAudioHardware.PlayTrack(uint32(settings.TrackQueue[0]), settings.TrackQueue[1], 0, uint8(settings.TrackFlags), settings.TrackTypes[0] == TYPE_USER_TRACK, settings.TrackTypes[1] == TYPE_USER_TRACK);
}


// 0x4EB660
void CAERadioTrackManager::CheckForStationRetune() {
    if (int8(m_ActiveSettings.StationID) == 0) return;
    m_bRetuneJustStarted = false;
    const auto active = m_nMode == eRadioTrackMode::RADIO_STARTING || m_nMode == eRadioTrackMode::RADIO_WAITING_TO_PLAY || m_nMode == eRadioTrackMode::RADIO_PLAYING || m_bInitialised || m_nStationsListed != 0 || m_nStationsListDown != 0 || int8(m_ActiveSettings.StationID) == 13;
    if (active && AudioEngine.GetCutsceneTrackStatus() == 0) {
        const auto* options = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
        if (options && (int8(options->RadioType) == 0 || int8(options->RadioType) == 3 || int8(options->RadioType) == 2) && StaticRef<uint8>(0xA43088) != 1) {
            if (int8(CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio()->RadioType) != 0) return;
            bool requested = true;
            if (m_iRadioStationScriptRequest < 0) {
                if (CPad::GetPad(0)->NextStationJustUp()) {
                    m_nStationsListDown = m_nStationsListed;
                    m_nStationsListed = int32(uint32(m_nStationsListed) + 1u);
                    m_nTimeRadioStationRetuned = StaticRef<uint32>(0xB7CB84);
                } else if (CPad::GetPad(0)->LastStationJustUp()) {
                    m_nStationsListDown = m_nStationsListed;
                    m_nStationsListed = int32(uint32(m_nStationsListed) - 1u);
                    m_nTimeRadioStationRetuned = StaticRef<uint32>(0xB7CB84);
                } else requested = false;
            } else {
                m_nStationsListDown = m_nStationsListed;
                m_nStationsListed = int32(uint32(m_iRadioStationScriptRequest) - uint32(int32(int8(m_RequestedSettings.StationID))));
                m_iRadioStationScriptRequest = -1;
                m_nTimeRadioStationRetuned = StaticRef<uint32>(0xB7CB84);
            }
            if (requested) {m_bDisplayStationName = true;m_bRetuneJustStarted = true;}
        }
    }
    if (m_nStationsListed == 0 && m_nStationsListDown == 0) return;
    auto station = int8(uint8(int8(m_RequestedSettings.StationID)) + uint8(m_nStationsListed));
    if (station < 1) station = int8(station + 13);
    else if (station > 13) station = int8(station - 13);
    if (station == 13 || (station == 12 && StaticRef<uint16>(0xB6B976) == 0)) {
        StopRadio(nullptr, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF, 0.f, 1.f);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.f, 1.f);
    } else {
        if (int8(m_ActiveSettings.StationID) == 13) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON, 0.f, 1.f);
            m_ActiveSettings.StationID = static_cast<eRadioID>(-1);
        } else StopRadio(nullptr, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START, 0.f, 1.f);
        const auto delay = !(StaticRef<float>(0xB6F14C) > StaticRef<float>(0x858C20)) ? 2000u : 4000u;
        if (StaticRef<uint32>(0xB7CB84) <= m_nTimeRadioStationRetuned + 1500u) return;
        if (StaticRef<uint32>(0xB7CB84) <= field_60 + delay) return;
    }
    StartRadio(static_cast<eRadioID>(station), m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, false);
    m_nStationsListed = 0;
    m_nStationsListDown = 0;
}


// 0x4EB890
void CAERadioTrackManager::CheckForStationRetuneDuringPause() {
    if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA && IsRadioOn() || m_iRadioStationMenuRequest <= RADIO_INVALID)
        return;

    if (m_iRadioStationMenuRequest != RADIO_OFF) {
        if (m_ActiveSettings.StationID == RADIO_OFF) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON);
            m_ActiveSettings.StationID = RADIO_INVALID;
        } else {
            AudioEngine.StopRadio(nullptr, true);
        }

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        if (CTimer::GetTimeInMSPauseMode() > m_nRetuneStartedTime + 700u) {
            StartRadio((eRadioID)m_iRadioStationMenuRequest, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, 0);
            m_iRadioStationMenuRequest = RADIO_INVALID;
        }
    } else {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        StartRadio(RADIO_OFF, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, 0);
        m_iRadioStationMenuRequest = RADIO_INVALID;
    }
}

// 0x4EA640
void CAERadioTrackManager::StartTrackPlayback() {
    AEAudioHardware.SetChannelFlags(m_HwClientHandle, 0, 55);
    AEAudioHardware.StartTrackPlayback();
    UpdateRadioVolumes();
}

// 0x4EA010
static float CalculateRadioVolume355() {
    const auto effects = AEAudioHardware.GetEffectsMasterScalingFactor();
    const auto music = AEAudioHardware.GetMusicMasterScalingFactor();
    const auto twenty = StaticRef<float>(0x858BA4);
    const auto nine = StaticRef<float>(0x85EED4);
    float result;
    __asm {
        fld effects
        fdiv music
        fldlg2
        fxch st(1)
        fyl2x
        fmul twenty
        fsub nine
        fstp result
    }
    return result;

}

void CAERadioTrackManager::UpdateRadioVolumes() {
    auto volume = -4.f;
    if (!((StaticRef<uint8>(0xB7CB49) || StaticRef<uint8>(0xB7CB48)) && m_bEnabledInPauseMode)) {
        if (CCutsceneMgr::IsRunning()) volume = -100.f;
        else if (StaticRef<uint8>(0xB6F065)) volume = -16.f;
        else if (AEAudioHardware.GetEffectsMasterScalingFactor() > 0.f && AEAudioHardware.GetMusicMasterScalingFactor() > 0.f) {
            bool duck = StaticRef<uint8>(0xB613E4) != 0;
            if (!duck) {
                for (uint8 slot = 0; slot < 2; ++slot) {
                    if (!AudioEngine.IsMissionAudioSampleFinished(slot) && AudioEngine.GetMissionAudioEvent(slot) != 65535) {
                        auto* position = AudioEngine.GetMissionAudioPosition(slot);
                        if (!position) { duck = true; break; }
                        CVector relative;
                        CAEAudioEnvironment::GetPositionRelativeToCamera_Vector(&relative, position);
                        if (relative.Magnitude() <= 15.f) { duck = true; break; }
                    }
                }
            }
            if (duck) {
                const auto first = CalculateRadioVolume355();
                const auto attenuation = first < 0.f ? CalculateRadioVolume355() : 0.f;
                m_f80 = attenuation;
                m_f84 = StaticRef<float>(0x85904C) * attenuation;
                volume = attenuation - 4.f;
            } else if (m_f80 < 0.f) {
                const auto increased = m_f84 + m_f80;
                m_f80 = increased < 0.f ? increased : 0.f;
                volume = m_f80 - 4.f;
            }
        }
        if (AudioEngine.IsAmbienceRadioActive()) volume -= 20.f;
    }
    if (m_bBassEnhance) {
        const auto bass = int8(m_ActiveSettings.BassSetting);
        if (bass == 1) volume -= 2.f;
        else if (bass == 2) volume += 1.5f;
    }
    AEAudioHardware.SetChannelVolume(int16(m_HwClientHandle), 0, volume, 0);
}

// 0x4E8400
void CAERadioTrackManager::PlayRadioAnnouncement(uint32) {
    // NOP
}

// 0x4EB550
void CAERadioTrackManager::StartRadio(const tVehicleAudioSettings& settings) {
    // plugin::CallMethod<0x4EB550, CAERadioTrackManager*, tVehicleAudioSettings*>(this, settings);

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (settings.RadioType == AE_RT_EMERGENCY) {
        StartRadio(RADIO_EMERGENCY_AA, settings.BassSetting, settings.BassFactor, 0);
        return;
    }

    if (settings.RadioType != AE_RT_CIVILIAN)
        return;

    const bool needsRetune = [&] {
       if (!m_bRadioAutoSelect)
           return false;

       const auto savedId = m_nSavedRadioStationId;
       if (savedId < 0 || savedId == settings.RadioStation || savedId == RADIO_OFF || savedId == RADIO_EMERGENCY_AA)
           return false;

       if (CTimer::GetTimeInMS() > m_nSavedTimeMs + 60'000)
           return false;

       // Como el original: 0x4EB5CF..0x4EB618: horas y dias guardados se leen como int8 (test + jl, movsx) y la diferencia es
       // (dia_actual - dia_guardado) [+ dias del mes anterior si es negativa] * 24 - horas_guardadas + hora_actual > 5.
       const auto savedHours = int8(m_nSavedGameClockHours);
       const auto savedDays = int8(m_nSavedGameClockDays);
       if (savedHours < 0 || savedDays < 0)
           return false;

       int32 days = int32(CClock::GetGameClockDays()) - int32(savedDays);
       if (days < 0) {
           int8 month = int8(uint8(CClock::GetGameClockMonth()) - 1);
           if (month < 0)
               month = int8(month + 12);
           days += StaticRef<uint8>(0x8CCF24 + size_t(int32(month)));
       }

       if (days * 24 - int32(savedHours) + int32(CClock::GetGameClockHours()) > 5)
           return false;

       return true;
    }();

    if (needsRetune) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        StartRadio((eRadioID)m_nSavedRadioStationId, settings.BassSetting, settings.BassFactor, 0);
    } else {
        StartRadio(settings.RadioStation, settings.BassSetting, settings.BassFactor, 0);
    }
}

// 0x4EB3C0
void CAERadioTrackManager::StartRadio(eRadioID id, eBassSetting bassSetting, float bassGain, bool skipTrack) {
    id = std::min(id, RADIO_OFF);

    if (CTimer::GetIsPaused()) {
        m_bEnabledInPauseMode = true;

        if (IsRadioOn() && id == m_ActiveSettings.StationID) {
            m_aRadioState[id].m_iTimeInPauseModeInMs = CTimer::GetTimeInMSPauseMode();
            return;
        }
    }

    if (id != RADIO_OFF && CAudioEngine::IsAmbienceTrackActive()) {
        if (!CTimer::GetIsPaused() && CAudioEngine::DoesAmbienceTrackOverrideRadio()) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
            return;
        }
        AudioEngine.StopAmbienceTrack(false);
    }

    auto* const rs = &m_RequestedSettings;
    rs->StationID = id;
    rs->BassSetting             = bassSetting;
    rs->BassGain            = bassGain;
    if (id == RADIO_OFF) {
        // Como el original: 0x4EB498..0x4EB4C3 solo vacia la cola, los tipos y los indices; NO restablece el resto
        // (BassSetting/BassGain recien escritos, PlayTime...) como hacia rs->Reset() (*this = {}).
        for (size_t k = 0; k < tRadioSettings::NUM_TRACKS; k++) {
            rs->TrackQueue.data()[k] = -1;
            rs->TrackTypes.data()[k] = TYPE_NONE;
            rs->TrackIndices.data()[k] = -1;
        }
    } else if (m_aRadioState[id].m_iTimeInMs < 0 || !TrackRadioStation(id, skipTrack)) {
        ChooseTracksForStation(rs->StationID);
        rs->PlayTime = CAEAudioUtility::GetRandomNumberInRange(0, 300'000);
    }
    switch (m_nMode) {
    case eRadioTrackMode::RADIO_STARTING:
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
    case eRadioTrackMode::RADIO_PLAYING:
        m_nMode = eRadioTrackMode::RADIO_STOPPING;
    }
    m_aRadioState[rs->StationID].m_iTimeInPauseModeInMs = -1;

    m_bInitialised = true;
}

// 0x4EAC30
bool CAERadioTrackManager::TrackRadioStation(eRadioID id, bool skipTrack) {
    auto& state = m_aRadioState[size_t(int8(id))];
    if (state.m_nGameClockHours >= 0 && state.m_nGameClockDays >= 0) {
        auto days = int32(StaticRef<uint8>(0xB70154)) - state.m_nGameClockDays;
        if (days < 0) {
            const auto month = int8(StaticRef<uint8>(0xB70155));
            const auto previousMonth = int8(month - 1) < 0 ? int8(month + 11) : int8(month - 1);
            days += StaticRef<uint8>(0x8CCF24 + previousMonth);
        }
        if (days * 24 - state.m_nGameClockHours + int32(StaticRef<uint8>(0xB70153)) > 5) return false;
    }
    auto elapsed = int32(StaticRef<uint32>(0xB7CB84) - uint32(state.m_iTimeInMs));
    if (elapsed < 7001) elapsed = 7000;
    const auto skipMinimum = int32(uint32(state.m_aElapsed[0]) + 1u);
    if (skipTrack && elapsed <= skipMinimum) elapsed = skipMinimum;
    auto& settings = m_RequestedSettings;
    settings.TrackQueue.fill(-1);
    settings.TrackTypes.fill(TYPE_NONE);
    settings.TrackIndices.fill(-1);
    int8 count = 0;
    int32 total = 0;
    for (int32 i = 0; i < 3; ++i) {
        total = int32(uint32(total) + uint32(state.m_aElapsed[size_t(i)]));
        if (elapsed > total) continue;
        settings.PlayTime = i == 0 ? int32(uint32(state.m_iTrackPlayTime) + uint32(elapsed)) : int32(uint32(state.m_aElapsed[size_t(i)]) - uint32(total) + uint32(elapsed));
        const auto type = state.m_aTrackTypes[size_t(i)];
        switch (type) {
        case TYPE_INDENT: case TYPE_ADVERT: case TYPE_DJ_BANTER:
            settings.TrackQueue[0] = state.m_aTrackQueue[size_t(i)];
            settings.TrackTypes[0] = type;
            count = 1;
            QueueUpTracksForStation(id, &count, id == static_cast<eRadioID>(12) ? 4 : 3, settings);
            return true;
        case TYPE_INTRO: case TYPE_TRACK: case TYPE_USER_TRACK: {
            const auto copies = type == TYPE_INTRO ? 3 : 2;
            for (int32 j = 0; j < copies; ++j) {
                // The EXE copies adjacent fields even when i+j exceeds the three saved tracks.
                settings.TrackQueue[size_t(j)] = reinterpret_cast<const int32*>(state.m_aTrackQueue.data())[i + j];
                settings.TrackTypes[size_t(j)] = reinterpret_cast<const int8*>(state.m_aTrackTypes.data())[i + j];
            }
            if (type == TYPE_USER_TRACK && settings.TrackQueue[1] == -1) {
                settings.TrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
                settings.TrackTypes[1] = TYPE_USER_TRACK;
                settings.TrackIndices[1] = int8(settings.TrackQueue[1]);
            }
            return true;
        }
        case TYPE_OUTRO:
            settings.TrackQueue[0] = state.m_aTrackQueue[size_t(i)];
            settings.TrackTypes[0] = type;
            count = 1;
            if (id == static_cast<eRadioID>(0)) QueueUpTracksForStation(id, &count, 2, settings);
            else {
                if (CAEAudioUtility::ResolveProbability(.5f)) QueueUpTracksForStation(id, &count, 0, settings);
                if (!QueueUpTracksForStation(id, &count, 2, settings)) QueueUpTracksForStation(id, &count, 1, settings);
            }
            return true;
        default: return false;
        }
    }
    if (elapsed <= int32(uint32(total) + 7000u)) {
        if (id == static_cast<eRadioID>(12)) QueueUpTracksForStation(id, &count, 4, settings);
        QueueUpTracksForStation(id, &count, id == static_cast<eRadioID>(12) ? 4 : 3, settings);
        elapsed = int32(uint32(elapsed) - uint32(total));
        settings.PlayTime = elapsed > 4999 ? 5000 : elapsed;
        return true;
    }
    if (elapsed <= int32(uint32(total) + 155000u)) {
        QueueUpTracksForStation(id, &count, 4, settings);
        settings.PlayTime = int32(uint32(elapsed) - uint32(total) - 5000u);
        return true;
    }
    if (int32(uint32(total) + 160000u) < elapsed) return false;
    if (id == static_cast<eRadioID>(12)) {
        QueueUpTracksForStation(id, &count, 4, settings);
        if (!StaticRef<uint8>(0xBA67F8) && CAEAudioUtility::ResolveProbability(std::bit_cast<float>(0x3E2E147Bu))) QueueUpTracksForStation(id, &count, 1, settings);
    } else {
        QueueUpTracksForStation(id, &count, 5, settings);
        AddMusicTrackIndexToHistory(id, settings.TrackIndices[size_t(count - 1)]);
        if (id == static_cast<eRadioID>(0)) QueueUpTracksForStation(id, &count, 2, settings);
        else if (CAEAudioUtility::ResolveProbability(.5f)) {
            if (CAEAudioUtility::ResolveProbability(.5f)) QueueUpTracksForStation(id, &count, 0, settings);
            QueueUpTracksForStation(id, &count, 3, settings);
        } else {
            if (CAEAudioUtility::ResolveProbability(.5f)) QueueUpTracksForStation(id, &count, 0, settings);
            if (!QueueUpTracksForStation(id, &count, 2, settings)) QueueUpTracksForStation(id, &count, 1, settings);
        }
    }
    settings.PlayTime = int32(uint32(elapsed) - uint32(total) - 155000u);
    return true;
}


// 0x4EA670
bool CAERadioTrackManager::QueueUpTracksForStation(eRadioID id, int8* count, int8 state, tRadioSettings& settings) {
    const auto station = int32(int8(id));
    const auto advance = [&] { *count = int8(*count + 1); };
    const auto queueSimple = [&](int32 track, int8 type, bool rejectInvalid) {
        settings.TrackQueue.data()[*count] = track;
        if (rejectInvalid && track == -1) {
            return false;
        }
        settings.TrackTypes.data()[*count] = type;
        advance();
        return true;
    };
    const auto queueOutro = [&] {
        const auto index = station * 31 + int32(settings.TrackIndices.data()[*count]);
        const auto* range = &StaticRef<int32>(0x8CA1B0) + index * 2;
        return queueSimple(CAEAudioUtility::GetRandomNumberInRange(range[0], range[1]), TYPE_OUTRO, false);
    };
    switch (state) {
    case TYPE_INDENT:
        return station != 12 && queueSimple(ChooseIdentIndex(id), TYPE_INDENT, true);
    case TYPE_ADVERT:
        return queueSimple(ChooseAdvertIndex(id), TYPE_ADVERT, false);
    case TYPE_DJ_BANTER:
        return station != 12 && queueSimple(ChooseDJBanterIndex(id), TYPE_DJ_BANTER, true);
    case TYPE_INTRO: {
        if (station == 12) {
            return false;
        }
        settings.TrackIndices.data()[*count] = ChooseMusicTrackIndex(id);
        const auto index = station * 31 + int32(settings.TrackIndices.data()[*count]);
        const auto* introRange = &StaticRef<int32>(0x8C9610) + index * 2;
        queueSimple(CAEAudioUtility::GetRandomNumberInRange(introRange[0], introRange[1]), TYPE_INTRO, false);
        settings.TrackIndices.data()[*count] = settings.TrackIndices.data()[*count - 1];
        queueSimple((&StaticRef<int32>(0x8C9040))[station * 31 + int32(settings.TrackIndices.data()[*count])], TYPE_TRACK, false);
        settings.TrackIndices.data()[*count] = settings.TrackIndices.data()[*count - 1];
        return queueOutro();
    }
    case TYPE_TRACK:
        if (station == 12) {
            const auto track = AEUserRadioTrackManager.SelectUserTrackIndex();
            settings.TrackQueue.data()[*count] = track;
            settings.TrackTypes.data()[*count] = TYPE_USER_TRACK;
            settings.TrackIndices.data()[*count] = int8(track);
            advance();
            return true;
        }
        settings.TrackIndices.data()[*count] = ChooseMusicTrackIndex(id);
        queueSimple((&StaticRef<int32>(0x8C9040))[station * 31 + int32(settings.TrackIndices.data()[*count])], TYPE_TRACK, false);
        settings.TrackIndices.data()[*count] = settings.TrackIndices.data()[*count - 1];
        return queueOutro();
    case TYPE_OUTRO:
        if (station == 12) {
            return false;
        }
        settings.TrackIndices.data()[*count] = ChooseMusicTrackIndex(id);
        return queueOutro();
    default:
        return true;
    }
}

// 0x4E9820
void CAERadioTrackManager::StopRadio(tVehicleAudioSettings* options, bool duringPause) {
    auto& settings = m_ActiveSettings;
    const auto paused = StaticRef<uint8>(0xB7CB49) || StaticRef<uint8>(0xB7CB48);
    if (m_nMode == eRadioTrackMode::RADIO_STARTING || m_nMode == eRadioTrackMode::RADIO_WAITING_TO_PLAY || m_nMode == eRadioTrackMode::RADIO_PLAYING) {
        if (!paused || duringPause) m_nMode = eRadioTrackMode::RADIO_STOPPING;
        auto& state = m_aRadioState[size_t(int8(settings.StationID))];
        for (size_t i = 0; i < 3; ++i) {
            state.m_aElapsed[i] = 0;
            state.m_iTrackPlayTime = -1;
            state.m_aTrackQueue[i] = -1;
            state.m_aTrackTypes[i] = TYPE_NONE;
        }
        state.m_iTimeInMs = int32(StaticRef<uint32>(0xB7CB84));
        state.m_nGameClockDays = int8(StaticRef<uint8>(0xB70154));
        state.m_nGameClockHours = int8(StaticRef<uint8>(0xB70153));
        if (state.m_iTimeInPauseModeInMs >= 0 && int8(settings.StationID) != 0 && int8(settings.StationID) != 13) {
            auto& listened = m_aListenTimes[size_t(int8(settings.StationID))];
            listened = int32(uint32(listened) + StaticRef<uint32>(0xB7CB7C) - uint32(state.m_iTimeInPauseModeInMs));
        }
        if (int8(settings.StationID) == 13) state.m_aElapsed[0] = 0;
        else {
            state.m_aElapsed[0] = int32(uint32(settings.TrackLengthMs) - uint32(settings.PlayTime) - 100u);
            const auto type = settings.CurrTrackType;
            switch (type) {
            case TYPE_INDENT: case TYPE_ADVERT: case TYPE_DJ_BANTER: case TYPE_OUTRO:
                state.m_iTrackPlayTime = settings.PlayTime;
                state.m_aTrackQueue[0] = settings.CurrTrackID;
                state.m_aTrackTypes[0] = type;
                break;
            case TYPE_INTRO:
                state.m_aElapsed[1] = int8(settings.StationID) == 11 ? StaticRef<int32>(0x8CAD50 + int32(settings.TrackIndices[0]) * 4) : 150000;
                state.m_aElapsed[2] = 5000;
                state.m_iTrackPlayTime = settings.PlayTime;
                if (settings.TrackQueue[0] == settings.CurrTrackID) {
                    for (size_t i = 0; i < 3; ++i) {state.m_aTrackQueue[i] = settings.TrackQueue[i];state.m_aTrackTypes[i] = settings.TrackTypes[i];}
                } else {
                    state.m_aTrackQueue[0] = settings.CurrTrackID;state.m_aTrackTypes[0] = type;
                    for (size_t i = 1; i < 3; ++i) {state.m_aTrackQueue[i] = settings.TrackQueue[i - 1];state.m_aTrackTypes[i] = settings.TrackTypes[i - 1];}
                }
                break;
            case TYPE_TRACK: case TYPE_USER_TRACK:
                state.m_aElapsed[1] = 5000;
                state.m_iTrackPlayTime = settings.PlayTime;
                if (settings.TrackQueue[0] == settings.CurrTrackID) {
                    for (size_t i = 0; i < 2; ++i) {state.m_aTrackQueue[i] = settings.TrackQueue[i];state.m_aTrackTypes[i] = settings.TrackTypes[i];}
                } else {
                    state.m_aTrackQueue[0] = settings.CurrTrackID;state.m_aTrackTypes[0] = type;
                    state.m_aTrackQueue[1] = settings.TrackQueue[0];state.m_aTrackTypes[1] = settings.TrackTypes[0];
                }
                break;
            }
        }
    }
    m_bInitialised = false;
    m_bEnabledInPauseMode = false;
    if ((StaticRef<uint8>(0xB7CB49) || StaticRef<uint8>(0xB7CB48)) && !duringPause) {
        m_iRadioStationMenuRequest = -1;
        m_nRetuneStartedTime = 0;
    }
    if (!options) {
        if (duringPause) {m_nStationsListed = 0;m_nStationsListDown = 0;m_bRetuneJustStarted = false;}
        return;
    }
    m_nStationsListed = 0;
    m_nStationsListDown = 0;
    m_iRadioStationScriptRequest = -1;
    m_bDisplayStationName = false;
    m_bRetuneJustStarted = false;
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.f, 1.f);
    if (int8(settings.StationID) == -1) settings.StationID = static_cast<eRadioID>(13);
    options->RadioStation = settings.StationID;
    options->BassSetting = settings.BassSetting;
    if (m_nMode != eRadioTrackMode::RADIO_STOPPED || m_bInitialised || m_nStationsListed != 0 || m_nStationsListDown != 0) {
        m_nSavedTimeMs = StaticRef<uint32>(0xB7CB84);
        m_nSavedGameClockDays = StaticRef<uint8>(0xB70154);
        m_nSavedGameClockHours = StaticRef<uint8>(0xB70153);
        m_nSavedRadioStationId = int8(settings.StationID);
    }
    if (int8(settings.StationID) == 0) settings.StationID = static_cast<eRadioID>(CAEAudioUtility::GetRandomNumberInRange<int32>(1, 13));
}


// 0x4E94C0
int32 CAERadioTrackManager::ChooseIdentIndex(eRadioID id) {
    const auto station = int32(int8(id));
    const auto* range = &StaticRef<int32>(0x8C8FB0) + station * 2;
    if (range[0] == 1922) {
        return -1;
    }
    for (;;) {
        const auto selected = CAEAudioUtility::GetRandomNumberInRange(range[0], range[1]);
        if (station == 6 && selected == 1100 && CStats::GetStatValue(static_cast<eStats>(338)) == 0.f) {
            continue;
        }
        const auto count = std::min(8, range[1] - range[0] - 1);
        const auto* history = &StaticRef<int32>(0xB62980) + station * 8;
        bool repeated = false;
        for (int32 i = 0; i < count; ++i) {
            if (history[i] == selected) {
                repeated = true;
                break;
            }
        }
        if (!repeated) {
            return selected;
        }
    }
}

// 0x4E9570
int32 CAERadioTrackManager::ChooseAdvertIndex(eRadioID id) {
    const auto station = int32(int8(id));
    const auto* banned = &StaticRef<int32>(0x8CADD0) + station * 23;
    const auto* history = &StaticRef<int32>(0xB620C0) + station * 40;
    for (;;) {
        const auto selected = CAEAudioUtility::GetRandomNumberInRange(StaticRef<int32>(0x8C8B88), StaticRef<int32>(0x8C8B8C));
        bool repeated = false;
        for (int32 i = 0; i < 23; ++i) {
            if (banned[i] == selected) {
                repeated = true;
                break;
            }
        }
        if (repeated) {
            continue;
        }
        for (int32 i = 0; i < 40; ++i) {
            if (history[i] == selected) {
                repeated = true;
                break;
            }
        }
        if (!repeated) {
            return selected;
        }
    }
}

// 0x4EA270
int8 CAERadioTrackManager::ChooseMusicTrackIndex(eRadioID id) {
    if (id == static_cast<eRadioID>(11)) {
        return ChooseTalkRadioShow();
    }
    const auto station = int32(int8(id));
    const auto available = (&StaticRef<int32>(0x8C9010))[station];
    const auto* history = &StaticRef<int8>(0xB62B40) + station * 20;
    for (;;) {
        const auto selected = int8(CAEAudioUtility::GetRandomNumberInRange(0, available - 1));
        bool repeated = false;
        for (int32 i = 0; i < std::min(20, available - 2); ++i) {
            if (history[i] == selected) {
                repeated = true;
                break;
            }
        }
        if (!repeated) {
            return selected;
        }
    }
}

// 0x4EA2D0
int32 CAERadioTrackManager::ChooseDJBanterIndex(eRadioID id) {
    const auto station = int32(int8(id));
    const auto special = StaticRef<int8>(0xB62C73);
    const auto index = int32(StaticRef<int8>(0xB62C72));
    int32 selected = -1;
    if (special == 0) {
        selected = (&StaticRef<int32>(0x8C8BF0))[station * 2];
    } else if (special == 1) {
        const auto* range = &StaticRef<int32>(0x8C8C50) + station * 2;
        if (index == 0 || (index == 1 && range[0] != range[1])) {
            selected = range[index];
        }
    } else if (special == 2) {
        selected = (&StaticRef<int32>(0x8CB280))[station * 22 + index];
    }
    if (selected == 1922) {
        selected = -1;
    } else if (selected >= 0) {
        const auto* history = &StaticRef<int32>(0xB61D78) + station * 15;
        for (int32 i = 0; i < 15; ++i) {
            if (history[i] == selected) {
                selected = -1;
                break;
            }
        }
    }
    if (selected != -1) {
        return selected;
    }
    const auto choose = [&](uintptr_t list) { return ChooseDJBanterIndexFromList(id, reinterpret_cast<int32**>(list)); };
    if (station == 0) {
        return choose(CGameLogic::LaRiotsActiveHere() ? 0x8C8E30 : 0x8C8CB0);
    }
    if (!CAEAudioUtility::ResolveProbability(.6f) || StaticRef<int32>(0xB72914) != 0) {
        return selected;
    }
    const auto hour = StaticRef<uint8>(0xB70153);
    const auto forecast = [](int32 weather) { return CWeather::ForecastWeather(static_cast<eWeatherType>(weather), 3); };
    if (forecast(16) || forecast(8)) {
        if (CAEAudioUtility::ResolveProbability(.5f)) {
            selected = choose(0x8C8E90);
            if (selected != -1) {
                return selected;
            }
        }
    } else {
        // Preserve these forecast calls: the following EXE hour range [8,6) is impossible.
        const auto sunny = forecast(0) || forecast(2) || forecast(13) || forecast(6) || forecast(11) || forecast(17);
        (void)sunny;
        if (forecast(9) && CAEAudioUtility::ResolveProbability(.5f)) {
            selected = choose(0x8C8F50);
            if (selected != -1) {
                return selected;
            }
        }
    }
    uintptr_t list{};
    if (hour >= 6 && hour <= 8) {
        list = 0x8C8DD0;
    } else if (hour >= 18 && hour <= 20) {
        list = 0x8C8D70;
    } else if (hour >= 22 || hour <= 2) {
        list = 0x8C8E30;
    }
    if (list && CAEAudioUtility::ResolveProbability(.3f)) {
        selected = choose(list);
        if (selected != -1) {
            return selected;
        }
    }
    return choose(0x8C8CB0);
}

// 0x4E95E0
int32 CAERadioTrackManager::ChooseDJBanterIndexFromList(eRadioID id, int32** list) {
    const auto station = int32(int8(id));
    const auto* ranges = reinterpret_cast<int32*>(list);
    const auto start = ranges[station * 2];
    if (start == 1922) {
        return -1;
    }
    const auto available = ranges[station * 2 + 1] - start + 1;
    const auto offset = CAEAudioUtility::GetRandomNumberInRange(0, available - 1);
    if (available <= 0) {
        return -1;
    }
    const auto* range = &StaticRef<int32>(0x8C8CB0) + station * 2;
    const auto count = std::min(15, range[1] - range[0] - 1);
    const auto* history = &StaticRef<int32>(0xB61D78) + station * 15;
    for (int32 i = 0; i < available; ++i) {
        const auto selected = start + (offset + i) % available;
        bool repeated = false;
        for (int32 h = 0; h < count; ++h) {
            if (history[h] == selected) {
                repeated = true;
                break;
            }
        }
        if (!repeated) {
            return selected;
        }
    }
    return -1;
}

// 0x4EB180
void CAERadioTrackManager::ChooseTracksForStation(eRadioID id) {
    int8 trackCount = 0;

    for (auto i = 0u; i < tRadioSettings::NUM_TRACKS; i++) {
        m_RequestedSettings.TrackTypes[i] = TYPE_NONE;
        m_RequestedSettings.TrackQueue[i] = -1;
        m_RequestedSettings.TrackIndices[i] = -1;
    }

    if (!CAEAudioUtility::ResolveProbability(0.95f)) {
        if (id) {
            if (CAEAudioUtility::ResolveProbability(0.5f))
                QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);

            if (!QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings))
                QueueUpTracksForStation(id, &trackCount, TYPE_ADVERT, m_RequestedSettings);

            if (id == RADIO_USER_TRACKS) {
                QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
                return;
            }
        } else {
            QueueUpTracksForStation(RADIO_EMERGENCY_AA, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        return;
    }

    if (id == RADIO_USER_TRACKS) {
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
        if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
            QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_ADVERT, m_RequestedSettings);
        }
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.9f)) {
        QueueUpTracksForStation(id, &trackCount, TYPE_TRACK, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        return;
    }

    QueueUpTracksForStation(id, &trackCount, TYPE_OUTRO, m_RequestedSettings);
    // Como el original: 0x4EB2AE..0x4EB2C0 lee el byte en Requested+0x32+trackCount (con trackCount = 0 lee PrevTrackType);
    // TrackIndices[-1] abortaria en depuracion, de ahi .data().
    AddMusicTrackIndexToHistory(id, m_RequestedSettings.TrackIndices.data()[trackCount - 1]);

    if (id == RADIO_EMERGENCY_AA) {
        QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f))
        QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);

    if (!QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings))
        QueueUpTracksForStation(id, &trackCount, TYPE_ADVERT, m_RequestedSettings);
}

// 0x4E8E40
int8 CAERadioTrackManager::ChooseTalkRadioShow() {
  int iVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  char *pcVar5;
  float fVar6;
  char local_20 [32];

  pcVar5 = local_20;
  for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
    pcVar5[0] = -1;
    pcVar5[1] = -1;
    pcVar5[2] = -1;
    pcVar5[3] = -1;
    pcVar5 = pcVar5 + 4;
  }
  pcVar5[0] = -1;
  pcVar5[1] = -1;
  cVar4 = '\0';
  pcVar5[2] = -1;
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x136));
  if ((fVar6 == (float)0.0) || (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x137)), fVar6 != (float)0.0)) {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x138));
    if (fVar6 != (float)0.0) {
      local_20[0] = '\x0f';
      goto LAB_004e8eb3;
    }
  }
  else {
    local_20[0] = '\x0e';
LAB_004e8eb3:
    cVar4 = '\x01';
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x139));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\f';
LAB_004e8f15:
    cVar4 = cVar4 + '\x01';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x139));
    if ((fVar6 != (float)0.0) && (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x138)), fVar6 == (float)0.0)) {
      local_20[cVar4] = '\r';
      goto LAB_004e8f15;
    }
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13b));
  if ((fVar6 != (float)0.0) && (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13c)), fVar6 == (float)0.0)) {
    local_20[cVar4] = '\x06';
    cVar4 = cVar4 + '\x01';
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x12e));
  if ((fVar6 == (float)0.0) || (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13a)), fVar6 != (float)0.0)) {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13c));
    if ((fVar6 != (float)0.0) && (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13d)), fVar6 == (float)0.0)) {
      local_20[cVar4] = '\x04';
      goto LAB_004e8ff3;
    }
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13d));
    if (fVar6 != (float)0.0) {
      local_20[cVar4] = '\x05';
      goto LAB_004e8ff3;
    }
  }
  else {
    local_20[cVar4] = '\x03';
LAB_004e8ff3:
    cVar4 = cVar4 + '\x01';
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13e));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\a';
LAB_004e903b:
    cVar4 = cVar4 + '\x01';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13e));
    if (fVar6 != (float)0.0) {
      local_20[cVar4] = '\b';
      goto LAB_004e903b;
    }
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13f));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\t';
LAB_004e90db:
    cVar4 = cVar4 + '\x01';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x13f));
    if (((fVar6 != (float)0.0) && (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x130)), fVar6 == (float)0.0))
       && (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x140)), fVar6 == (float)0.0)) {
      local_20[cVar4] = '\n';
      goto LAB_004e90db;
    }
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x140));
    if (fVar6 != (float)0.0) {
      local_20[cVar4] = '\v';
      goto LAB_004e90db;
    }
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x141));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\x1b';
LAB_004e9123:
    cVar4 = cVar4 + '\x01';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x141));
    if (fVar6 != (float)0.0) {
      local_20[cVar4] = '\x1c';
      goto LAB_004e9123;
    }
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x142));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\x1d';
LAB_004e916b:
    cVar4 = cVar4 + '\x01';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x142));
    if (fVar6 != (float)0.0) {
      local_20[cVar4] = '\x1e';
      goto LAB_004e916b;
    }
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x143));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\0';
LAB_004e91f1:
    cVar4 = cVar4 + '\x01';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x144));
    if ((fVar6 != (float)0.0) && (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x145)), fVar6 == (float)0.0)) {
      local_20[cVar4] = '\x01';
      goto LAB_004e91f1;
    }
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x146));
    if (fVar6 != (float)0.0) {
      local_20[cVar4] = '\x02';
      goto LAB_004e91f1;
    }
  }
  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x12e));
  if (fVar6 == (float)0.0) {
    local_20[cVar4] = '\x10';
  }
  else {
    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x12e));
    if ((fVar6 == (float)0.0) || (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x12f)), fVar6 != (float)0.0)) {
      fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x12f));
      if ((fVar6 == (float)0.0) || (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x143)), fVar6 != (float)0.0))
      {
        fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x143));
        if ((fVar6 == (float)0.0) || (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x130)), fVar6 != (float)0.0)
           ) {
          fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x130));
          if ((fVar6 == (float)0.0) ||
             (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x131)), fVar6 != (float)0.0)) {
            fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x131));
            if ((fVar6 == (float)0.0) ||
               (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x132)), fVar6 != (float)0.0)) {
              fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x132));
              if ((fVar6 == (float)0.0) ||
                 (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x133)), fVar6 != (float)0.0)) {
                fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x133));
                if ((fVar6 == (float)0.0) ||
                   (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x134)), fVar6 != (float)0.0)) {
                  fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x134));
                  if ((fVar6 == (float)0.0) ||
                     (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x135)), fVar6 != (float)0.0)) {
                    fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0x135));
                    if ((fVar6 == (float)0.0) ||
                       (fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0xb5)), fVar6 == (float)4.0)) {
                      fVar6 = (float)CStats::GetStatValue(static_cast<eStats>(0xb5));
                      if (fVar6 != (float)4.0) goto LAB_004e9481;
                      local_20[cVar4] = '\x1a';
                    }
                    else {
                      local_20[cVar4] = '\x19';
                    }
                  }
                  else {
                    local_20[cVar4] = '\x18';
                  }
                }
                else {
                  local_20[cVar4] = '\x17';
                }
              }
              else {
                local_20[cVar4] = '\x16';
              }
            }
            else {
              local_20[cVar4] = '\x15';
            }
          }
          else {
            local_20[cVar4] = '\x14';
          }
        }
        else {
          local_20[cVar4] = '\x13';
        }
      }
      else {
        local_20[cVar4] = '\x12';
      }
    }
    else {
      local_20[cVar4] = '\x11';
    }
  }
  cVar4 = cVar4 + '\x01';
LAB_004e9481:
  iVar2 = cVar4 + -1;
  do {
    iVar1 = CAEAudioUtility::GetRandomNumberInRange(0,iVar2);
    cVar4 = '\0';
    if (iVar2 < 1) {
      return local_20[iVar1];
    }
    iVar3 = 0;
    while (local_20[iVar1] != (&StaticRef<int8>(0xB62C1C))[iVar3]) {
      cVar4 = cVar4 + '\x01';
      iVar3 = (int)cVar4;
      if (iVar2 <= iVar3) {
        return local_20[iVar1];
      }
    }
  } while( true );
}

// 0x4E96C0
void CAERadioTrackManager::AddMusicTrackIndexToHistory(eRadioID id, int8 trackIndex) {
    if (trackIndex >= 0 && m_nMusicTrackIndexHistory[id].indices[0] != trackIndex) {
        m_nMusicTrackIndexHistory[id].PutAtFirst(trackIndex);
        m_nTracksInARow[id]++;
    }
}

// 0x4E9720
void CAERadioTrackManager::AddIdentIndexToHistory(eRadioID id, int32 trackIndex) {
    // Como el original: 0x4E972F mov esi,[esp+0Ch] / 0x4E9733 cmp eax,esi: el indice es un dword (no un int8).
    if (m_nIdentIndexHistory[id].indices[0] != trackIndex)
        m_nIdentIndexHistory[id].PutAtFirst(trackIndex);
}

// 0x4E9760
void CAERadioTrackManager::AddAdvertIndexToHistory(eRadioID id, int32 trackIndex) {
    // Como el original: 0x4E9761 mov ebx,[esp+0Ch] / 0x4E9771 cmp [..],ebx: el indice es un dword (no un int8).
    if (m_nAdvertIndexHistory[id].indices[0] != trackIndex) {
        m_nAdvertIndexHistory[id].PutAtFirst(trackIndex);
        m_nTracksInARow[id] = 0;
    }
}

// 0x4E97B0
void CAERadioTrackManager::AddDJBanterIndexToHistory(eRadioID id, int32 trackIndex) {
    // Como el original: 0x4E97B1 mov ebx,[esp+0Ch] / 0x4E97C0 cmp [..],ebx: el indice es un dword (no un int8).
    if (m_nDJBanterIndexHistory[id].indices[0] != trackIndex) {
        m_nDJBanterIndexHistory[id].PutAtFirst(trackIndex);
        m_nTracksInARow[id] = 0;
    }
}

// 0x4EA590
void CAERadioTrackManager::CheckForPause() {
    if (CTimer::GetIsPaused()) {
        m_bPauseMode = true;
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, m_bEnabledInPauseMode ? 1.0f : 0.0f);
    } else {
        const auto* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();

        if (settings && notsa::contains({
                AE_RT_CIVILIAN,
                AE_RT_EMERGENCY,
                AE_RT_UNKNOWN
            }, settings->RadioType)
            || CAudioEngine::IsAmbienceRadioActive()
        ) {
            m_bPauseMode = false;
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, 1.0f);
        } else {
            StopRadio(nullptr, false);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
            m_bPauseMode = false;
        }
    }
}

// 0x4EB9A0
void CAERadioTrackManager::Service(int32 playTime) {
    auto& settings = m_ActiveSettings;
    StaticRef<int32>(0x8CBA68) = settings.PlayTime;
    settings.PlayTime = playTime;
    settings.TrackLengthMs = AEAudioHardware.GetTrackLengthMs();
    settings.CurrTrackID = AEAudioHardware.GetPlayingTrackID();
    if (!StaticRef<uint8>(0xB7CB49) && !StaticRef<uint8>(0xB7CB48)) {
        CheckForMissionStatsChanges();
        CheckForStationRetune();
    } else CheckForStationRetuneDuringPause();
    if (m_bInitialised && m_nMode == eRadioTrackMode::RADIO_STOPPED) {
        if (int8(m_RequestedSettings.StationID) == 13) {
            settings = m_RequestedSettings;
            const auto* options = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
            if (options && (int8(options->RadioType) == 0 || int8(options->RadioType) == 3 || int8(options->RadioType) == 2)) m_bDisplayStationName = true;
            m_bInitialised = false;
        } else if (!AudioEngine.IsAmbienceTrackActive()) {
            settings = m_RequestedSettings;
            m_nMode = eRadioTrackMode::RADIO_STARTING;
            if (IsVehicleRadioActive()) m_bDisplayStationName = true;
            m_bInitialised = false;
        }
    }
    switch (m_nMode) {
    case eRadioTrackMode::RADIO_STARTING:
        if (settings.PlayTime < 0) settings.PlayTime = 0;
        AEAudioHardware.SetBassSetting(m_bBassEnhance ? settings.BassSetting : static_cast<eBassSetting>(0), settings.BassGain);
        AEAudioHardware.PlayTrack(uint32(settings.TrackQueue[0]), settings.TrackQueue[1], uint32(settings.PlayTime), uint8(settings.TrackFlags), settings.TrackTypes[0] == TYPE_USER_TRACK, settings.TrackTypes[1] == TYPE_USER_TRACK);
        m_nMode = eRadioTrackMode::RADIO_WAITING_TO_PLAY;
        return;
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
        if (settings.PlayTime == -2) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.f, 1.f);
            StartTrackPlayback();
            field_60 = StaticRef<uint32>(0xB7CB84);
            m_aRadioState[size_t(int8(settings.StationID))].m_iTimeInPauseModeInMs = int32(StaticRef<uint32>(0xB7CB7C));
            m_nMode = eRadioTrackMode::RADIO_PLAYING;
            return;
        }
        if (settings.PlayTime == -8 && (settings.CurrTrackID == settings.TrackQueue[1] || (settings.CurrTrackID == settings.TrackQueue[0] && settings.TrackQueue[1] == -1))) {
            settings.TrackQueue[0] = ChooseAdvertIndex(static_cast<eRadioID>(12));
            settings.TrackTypes[0] = TYPE_ADVERT;
            settings.TrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
            settings.TrackTypes[1] = TYPE_USER_TRACK;
            settings.TrackIndices[1] = int8(settings.TrackQueue[1]);
            m_nMode = eRadioTrackMode::RADIO_STARTING;
            return;
        }
        break;
    case eRadioTrackMode::RADIO_PLAYING: {
        if (int8(settings.StationID) == 12 && settings.PlayTime == -6) {
            if (AEAudioHardware.GetActiveTrackID() == settings.TrackQueue[0] && settings.TrackQueue[1] != -1) {
                settings.TrackQueue[0] = settings.TrackQueue[1];
                settings.TrackIndices[0] = settings.TrackIndices[1];
                settings.TrackTypes[0] = settings.TrackTypes[1];
                settings.TrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
                settings.TrackTypes[1] = TYPE_USER_TRACK;
                settings.TrackIndices[1] = int8(settings.TrackQueue[1]);
            } else {
                settings.TrackQueue[0] = AEUserRadioTrackManager.SelectUserTrackIndex();
                settings.TrackTypes[0] = TYPE_USER_TRACK;
                settings.TrackIndices[0] = int8(settings.TrackQueue[0]);
                settings.TrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
                settings.TrackTypes[1] = TYPE_USER_TRACK;
                settings.TrackIndices[1] = int8(settings.TrackQueue[1]);
            }
            m_nMode = eRadioTrackMode::RADIO_STARTING;
        }
        const auto record = [&](int8 type, int32 track, int8 index) {
            if (type == TYPE_INTRO || type == TYPE_TRACK || type == TYPE_OUTRO || type == TYPE_USER_TRACK) AddMusicTrackIndexToHistory(settings.StationID, index);
            else if (type == TYPE_INDENT) reinterpret_cast<void(__thiscall*)(CAERadioTrackManager*,eRadioID,int32)>(0x4E9720)(this, settings.StationID, track);
            else if (type == TYPE_ADVERT) reinterpret_cast<void(__thiscall*)(CAERadioTrackManager*,eRadioID,int32)>(0x4E9760)(this, settings.StationID, track);
            else if (type == TYPE_DJ_BANTER) reinterpret_cast<void(__thiscall*)(CAERadioTrackManager*,eRadioID,int32)>(0x4E97B0)(this, settings.StationID, track);
        };
        if (settings.TrackQueue[0] == settings.CurrTrackID) {
            record(settings.TrackTypes[0], settings.TrackQueue[0], settings.TrackIndices[0]);
            settings.CurrTrackType = settings.TrackTypes[0];
            settings.CurrTrackIdx = settings.TrackIndices[0];
        } else if (settings.PrevTrackID == settings.CurrTrackID) {
            record(settings.PrevTrackType, settings.PrevTrackID, settings.PrevTrackIdx);
            settings.CurrTrackType = settings.PrevTrackType;
            settings.CurrTrackIdx = settings.PrevTrackIdx;
        }
        if (int8(settings.StationID) == 12 && (settings.TrackTypes[0] == TYPE_USER_TRACK || AEUserRadioTrackManager.GetUserTrackPlayMode() == 0)) {
            if (CPad::GetPad(0)->IsRadioTrackSkipPressed()) {
                StopRadio(nullptr, true);
                while (m_nMode != eRadioTrackMode::RADIO_STOPPED || m_bInitialised || m_nStationsListed != 0 || m_nStationsListDown != 0) {
                    Service(AEAudioHardware.GetTrackPlayTime());
                    AEAudioHardware.Service();
                }
                StartRadio(settings.StationID, settings.BassSetting, settings.BassGain, true);
            }
        }
        CheckForPause();
        UpdateRadioVolumes();
        CheckForTrackConcatenation();
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.f, 1.f);
        return;
    }
    case eRadioTrackMode::RADIO_STOPPING:
    case eRadioTrackMode::RADIO_STOPPING_CHANNELS_STOPPED:
        AEAudioHardware.StopTrack();
        m_nMode = eRadioTrackMode::RADIO_WAITING_TO_STOP;
        return;
    case eRadioTrackMode::RADIO_STOPPING_SILENCED:
        m_nMode = eRadioTrackMode::RADIO_STOPPING_CHANNELS_STOPPED;
        return;
    case eRadioTrackMode::RADIO_WAITING_TO_STOP:
        if (settings.PlayTime == -6 || settings.PlayTime == -8) m_nMode = eRadioTrackMode::RADIO_STOPPED;
        else if (settings.PlayTime == -7 || settings.PlayTime == -2) {AEAudioHardware.StopTrack();return;}
        break;
    default: break;
    }
}


// 0x5D40E0
void CAERadioTrackManager::Load() {
    for (auto r = 0; r < RADIO_COUNT; r++) {
        for (auto& historyIndex : m_nMusicTrackIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(historyIndex);
        }

        for (auto& identIndex : m_nIdentIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(identIndex);
        }

        for (auto& advertIndex : m_nAdvertIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(advertIndex);
        }

        for (auto& banterIndex : m_nDJBanterIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(banterIndex);
        }
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsCitiesPassed);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCasino3);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCasino6);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCasino10);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCat1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert3);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert5);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert8);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert10);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedFarlie3);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedLAFin2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedMansion2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedRyder2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedRiot1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedSCrash1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedStrap4);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedSweet2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedTruth2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedVCrash2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsStartedBadlands);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsStartedCat2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsStartedCrash1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsLastHitGameClockDays);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsLastHitGameClockHours);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsLastHitTimeOutHours);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nSpecialDJBanterPending);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nSpecialDJBanterIndex);
}

// 0x5D3EE0
void CAERadioTrackManager::Save() {
    for (auto r = 0; r < RADIO_COUNT; r++) {
        for (auto& historyIndex : m_nMusicTrackIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(historyIndex);
        }

        for (auto& identIndex : m_nIdentIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(identIndex);
        }

        for (auto& advertIndex : m_nAdvertIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(advertIndex);
        }

        for (auto& banterIndex : m_nDJBanterIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(banterIndex);
        }
    }

    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsCitiesPassed);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCasino3);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCasino6);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCasino10);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCat1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert3);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert5);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert8);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert10);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedFarlie3);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedLAFin2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedMansion2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedRyder2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedRiot1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedSCrash1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedStrap4);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedSweet2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedTruth2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedVCrash2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsStartedBadlands);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsStartedCat2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsStartedCrash1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsLastHitGameClockDays);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsLastHitGameClockHours);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsLastHitTimeOutHours);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nSpecialDJBanterPending);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nSpecialDJBanterIndex);
}
