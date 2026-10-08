/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CHud
 * Reconstrucción: COMPLETA — 38/38 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 8.
 * Validación registrada: 8/38 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   DrawCrossHairs | 0x0058E020 | S
 *   DrawHelpText | 0x0058B6E0 | S
 *   DrawMissionTimers | 0x0058B180 | S
 *   DrawPlayerInfo | 0x0058EAF0 | S
 *   DrawSubtitles | 0x0058C250 | S
 *   DrawSuccessFailedMessage | 0x0058C6A0 | S
 *   DrawVitalStats | 0x00589650 | S
 *   DrawWanted | 0x0058D9A0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Hud.h"
#include "Garages.h"
#include "IdleCam.h"
#include "MenuSystem.h"
#include "Radar.h"
#include "Vehicle.h"
#include "EntryExitManager.h"
#include "TaskSimpleUseGun.h"
#include "eHud.h"

void CHud::InjectHooks() {
    RH_ScopedClass(CHud);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA850);
    RH_ScopedInstall(ReInitialise, 0x588880);
    RH_ScopedInstall(Shutdown, 0x588850);
    RH_ScopedInstall(Draw, 0x58FAE0);
    RH_ScopedInstall(GetRidOfAllHudMessages, 0x588A50);
    RH_ScopedInstall(GetYPosBasedOnHealth, 0x588B60);
    RH_ScopedInstall(HelpMessageDisplayed, 0x588B50);
    RH_ScopedInstall(ResetWastedText, 0x589070);
    RH_ScopedInstall(SetMessage, 0x588F60);
    RH_ScopedInstall(SetBigMessage, 0x588FC0);
    RH_ScopedInstall(SetHelpMessage, 0x588BE0);
    RH_ScopedInstall(SetHelpMessageStatUpdate, 0x588D40);
    RH_ScopedInstall(SetHelpMessageWithNumber, 0x588E30);
    RH_ScopedInstall(SetVehicleName, 0x588F50);
    RH_ScopedInstall(SetZoneName, 0x588BB0);
    RH_ScopedInstall(DrawAfterFade, 0x58D490);
    RH_ScopedInstall(DrawAreaName, 0x58AA50);
    RH_ScopedInstall(DrawBustedWastedMessage, 0x58CA50);
    RH_ScopedInstall(DrawCrossHairs, 0x58E020); // -
    RH_ScopedInstall(DrawFadeState, 0x58D580);
    RH_ScopedInstall(DrawHelpText, 0x58B6E0);
    RH_ScopedInstall(DrawMissionTimers, 0x58B180);
    RH_ScopedInstall(DrawMissionTitle, 0x58D240);
    RH_ScopedInstall(DrawOddJobMessage, 0x58CC80);
    RH_ScopedInstall(DrawRadar, 0x58A330);
    RH_ScopedInstall(DrawScriptText, 0x58C080);
    RH_ScopedInstall(DrawSubtitles, 0x58C250);
    RH_ScopedInstall(DrawSuccessFailedMessage, 0x58C6A0);
    RH_ScopedInstall(DrawVehicleName, 0x58AEA0);
    RH_ScopedInstall(DrawVitalStats, 0x589650);
    RH_ScopedInstall(DrawAmmo, 0x5893B0);
    RH_ScopedInstall(DrawPlayerInfo, 0x58EAF0);
    RH_ScopedInstall(DrawTripSkip, 0x58A160);
    RH_ScopedInstall(DrawWanted, 0x58D9A0);
    RH_ScopedInstall(DrawWeaponIcon, 0x58D7D0);
    RH_ScopedInstall(RenderArmorBar, 0x5890A0);
    RH_ScopedInstall(RenderBreathBar, 0x589190);
    RH_ScopedInstall(RenderHealthBar, 0x589270);
}

// 0x5BA850
void CHud::Initialise() {
    static constexpr SpriteFileName textures[]= { // 0x8D128C
        { "fist",           "fistm"           },
        { "siteM16",        "siteM16m"        },
        { "siterocket",     "siterocketm"     },
        { "radardisc",      "radardiscA"      },
        { "radarRingPlane", "radarRingPlaneA" },
        { "SkipIcon",       "SkipIconA"       },
    };

    auto txd = CTxdStore::AddTxdSlot("hud");
    CTxdStore::LoadTxd(txd, "MODELS\\HUD.TXD");
    CTxdStore::AddRef(txd);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(txd);

    for (auto i = 0u; i < std::size(Sprites); i++) {
        const auto& [texture, mask] = textures[i];
        Sprites[i].SetTexture(texture, mask);
    }
    CTxdStore::PopCurrentTxd();
    ReInitialise();
}

// 0x588880
void CHud::ReInitialise() {
    memset(m_pHelpMessageToPrint, 0, sizeof(m_pHelpMessageToPrint));
    memset(m_pLastHelpMessage,    0, sizeof(m_pLastHelpMessage));
    memset(m_pHelpMessage,        0, sizeof(m_pHelpMessage));
    memset(m_Message,             0, sizeof(m_Message));
    memset(m_BigMessage,          0, sizeof(m_BigMessage));
    memset(BigMessageX,           0, sizeof(BigMessageX));

    OddJob2On       = 0;
    OddJob2Timer    = 0;
    OddJob2XOffset  = 0.0f;
    OddJob2OffTimer = 0.0f;
    PagerXOffset    = 150.0f;

    std::ranges::fill(TimerCounterHideState, 0);
    std::ranges::fill(TimerCounterWasDisplayed, 0);
    TimerMainCounterWasDisplayed = false;
    TimerMainCounterHideState = 0;

    const CPlayerInfo& playerInfo   = FindPlayerInfo();
    m_LastTimeEnergyLost            = playerInfo.m_nLastTimeEnergyLost;
    m_LastDisplayScore              = playerInfo.m_nDisplayMoney;
    m_fHelpMessageStatUpdateValue   = 0.0f;
    m_Wants_To_Draw_Hud             = true;
    m_bDraw3dMarkers                = true;
    m_ZoneNameTimer                 = 0;
    m_pZoneName                     = nullptr;
    m_pLastZoneName                 = nullptr;
    m_ZoneState                     = NAME_DONT_SHOW;
    m_nHelpMessageTimer             = 0;
    m_nHelpMessageFadeTimer         = 0;
    m_nHelpMessageState             = 0;
    m_bHelpMessageQuick             = false;
    m_nHelpMessageStatId            = 0;
    m_nHelpMessageMaxStatValue      = 1000;
    m_bHelpMessagePermanent         = false;
    m_fHelpMessageTime              = 1.0f;
    m_fHelpMessageBoxWidth          = 200.0f;
    m_pVehicleName                  = nullptr;
    m_pLastVehicleName              = nullptr;
    m_pVehicleNameToPrint           = nullptr;
    m_VehicleNameTimer              = 0;
    m_VehicleFadeTimer              = 0;
    m_VehicleState                  = NAME_DONT_SHOW;
    bScriptDontDisplayRadar         = false;
    bScriptForceDisplayWithCounters = false;
    bScriptDontDisplayVehicleName   = false;
    bScriptDontDisplayAreaName      = false;
    m_ItemToFlash                   = ITEM_NONE;
    m_EnergyLostTimer               = 0;
    m_EnergyLostFadeTimer           = 0;
    m_EnergyLostState               = 5;
    m_DisplayScoreTimer             = 0;
    m_DisplayScoreFadeTimer         = 0;
    m_DisplayScoreState             = 5;
    m_LastWanted                    = 0;
    m_WantedTimer                   = 0;
    m_WantedFadeTimer               = 0;
    m_WantedState                   = 5;
    m_LastWeapon                    = 0;
    m_WeaponTimer                   = 0;
    m_WeaponFadeTimer               = 0;
    m_WeaponState                   = 5;
    bDrawClock                      = true;
    m_LastBreathTime                = 0;
}

// 0x588850
void CHud::Shutdown() {
    std::ranges::for_each(Sprites, [](auto& sprite) { sprite.Delete(); });
    CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("hud"));
}

// 0x588B60
float CHud::GetYPosBasedOnHealth(uint8 playerId, float pos, int8 offset) {
    return (float)FindPlayerInfo(playerId).m_nMaxHealth < 101.0f
               ? pos - SCREEN_SCALE_Y((float)offset)
               : pos;
}

// 0x588B50
bool CHud::HelpMessageDisplayed() {
    return m_nHelpMessageState != 0;
}

// 0x588F60
void CHud::SetMessage(const GxtChar* message) {
    if (message) {
        strncpy_s((char*)m_Message, sizeof(m_Message), AsciiFromGxtChar(message), sizeof(m_Message));
    } else {
        m_Message[0] = '\0';
    }
}

// little bit different from OG
// 0x588FC0
void CHud::SetBigMessage(GxtChar* message, eMessageStyle style) {
    if (BigMessageX[style] != 0.0f) {
        return;
    }

    strncpy_s((char*)m_BigMessage[style], sizeof(m_BigMessage[style]), AsciiFromGxtChar(message), sizeof(m_BigMessage[style]));

    switch (style) {
    case STYLE_WHITE_MIDDLE_SMALLER: {
        if (strcmp(AsciiFromGxtChar(message), AsciiFromGxtChar(LastBigMessage[STYLE_WHITE_MIDDLE_SMALLER])) != 0) {
            OddJob2OffTimer = 0.0f;
            OddJob2On = 0;
        }
        strncpy_s((char*)LastBigMessage[style], sizeof(LastBigMessage[style]), AsciiFromGxtChar(message), sizeof(LastBigMessage[style]));
        break;
    }
    default: {
        message[0] = '\0';
    }
    }
}

// 0x588BE0
void CHud::SetHelpMessage(const GxtChar* text, bool quickMessage, bool permanent, bool addToBrief) {
    if (m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER][0] || CGarages::MessageIDString[0] || CReplay::Mode == MODE_PLAYBACK || CCutsceneMgr::IsRunning()) {
        return;
    }

    std::ranges::fill(m_pHelpMessageToPrint, '\0');
    std::ranges::fill(m_pLastHelpMessage, '\0');
    std::ranges::fill(m_pHelpMessage, '\0');

    CMessages::StringCopy(m_pHelpMessage, text, sizeof(m_pHelpMessage));
    CMessages::InsertPlayerControlKeysInString(m_pHelpMessage);
    if (m_nHelpMessageState && CMessages::StringCompare(m_pHelpMessage, m_pHelpMessageToPrint, sizeof(m_pHelpMessage)))
        return;

    std::ranges::fill(m_pLastHelpMessage, '\0');
    if (!text) {
        m_pHelpMessage[0] = '\0';
        m_pHelpMessageToPrint[0] = '\0';
    }

    if (permanent) {
        m_nHelpMessageState = 1;
        CMessages::StringCopy(m_pHelpMessageToPrint, m_pHelpMessage, sizeof(m_pHelpMessage));
        CMessages::StringCopy(m_pLastHelpMessage, m_pHelpMessage, sizeof(m_pHelpMessage));
    } else {
        m_nHelpMessageState = 0;
    }

    if (addToBrief)
        CMessages::AddToPreviousBriefArray(text);

    m_bHelpMessagePermanent = permanent;
    m_bHelpMessageQuick = quickMessage;
    m_nHelpMessageStatId = 0;
    m_nHelpMessageMaxStatValue = 1000;
    m_fHelpMessageStatUpdateValue = 0.0f;
}

// 0x588D40
void CHud::SetHelpMessageStatUpdate(eStatUpdateState state, uint16 statId, float diff, float max) {
    if (m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER][0] || CGarages::MessageIDString[0] || CReplay::Mode == MODE_PLAYBACK || CCutsceneMgr::IsCutsceneProcessing()) {
        return;
    }

    std::ranges::fill(m_pHelpMessageToPrint, '\0');
    std::ranges::fill(m_pLastHelpMessage, '\0');
    std::ranges::fill(m_pHelpMessage, '\0');

    if (m_nHelpMessageState && CMessages::StringCompare(m_pHelpMessage, m_pHelpMessageToPrint, sizeof(m_pHelpMessage)))
        return;

    std::ranges::fill(m_pLastHelpMessage, '\0');
    m_nHelpMessageState = 0;
    m_bHelpMessageQuick = false;
    m_bHelpMessagePermanent = false;
    m_nHelpMessageStatId = statId;
    m_fHelpMessageStatUpdateValue = diff;
    m_nHelpMessageMaxStatValue = (uint32)max;
    sprintf_s(gString, state == STAT_UPDATE_INCREASE ? "+" : "-");
    AsciiToGxtChar(gString, m_pHelpMessage);
}

// 0x588E30
void CHud::SetHelpMessageWithNumber(const GxtChar* text, int32 number, bool quickMessage, bool permanent) {
    if (m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER][0] || CGarages::MessageIDString[0] || CReplay::Mode == MODE_PLAYBACK || CCutsceneMgr::IsCutsceneProcessing()) {
        return;
    }

    GxtChar str[400];
    CMessages::InsertNumberInString(text, number, -1, -1, -1, -1, -1, str);
    CMessages::GetStringLength(str);
    CMessages::StringCopy(m_pHelpMessage, str, sizeof(m_pHelpMessage));
    CMessages::InsertPlayerControlKeysInString(m_pHelpMessage);

    std::ranges::fill(m_pLastHelpMessage, '\0');
    if (permanent) {
        m_nHelpMessageState = 1;
        CMessages::StringCopy(m_pHelpMessageToPrint, m_pHelpMessage, sizeof(m_pHelpMessage));
        CMessages::StringCopy(m_pLastHelpMessage, m_pHelpMessage, sizeof(m_pHelpMessage));
    } else {
        m_nHelpMessageState = 0;
    }

    m_bHelpMessagePermanent = permanent;
    m_bHelpMessageQuick = quickMessage;
    m_nHelpMessageStatId = 0;
    m_nHelpMessageMaxStatValue = 1000;
    m_fHelpMessageStatUpdateValue = 0.0f;
}

// 0x588F50
void CHud::SetVehicleName(const GxtChar* name) {
    m_pVehicleName = name;
}

// 0x588BB0
void CHud::SetZoneName(const GxtChar* name, bool displayImmediately) {
    if (displayImmediately) {
        m_pZoneName = name;
        return;
    }
    if (CGame::currArea || m_ZoneState != NAME_DONT_SHOW) {
        return;
    }
    m_pZoneName = name;
}

// called each frame from Render2dStuff()
// 0x58FAE0
void CHud::Draw() {
    if (CReplay::Mode == MODE_PLAYBACK || CWeapon::ms_bTakePhoto || FrontEndMenuManager.m_bActivateMenuNextFrame || gbCineyCamProcessedOnFrame == CTimer::GetFrameCounter())
        return;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERNEAREST));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,       RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));

    if (!TheCamera.m_bWideScreenOn) {
        DrawCrossHairs();
        if (FrontEndMenuManager.m_bHudOn && CTheScripts::bDisplayHud) {
            DrawPlayerInfo();
            DrawWanted();
        }
        if (!bScriptDontDisplayVehicleName) {
            DrawVehicleName();
        }
        DrawMissionTimers();
    }

    if (!bScriptDontDisplayRadar && !TheCamera.m_bWideScreenOn) {
        CPed* player = FindPlayerPed();
        CPad* pad = CPad::GetPad();
        if (!pad->GetDisplayVitalStats(player) || FindPlayerVehicle()) {
            bDrawingVitalStats = false;
            DrawRadar();
        } else {
            bDrawingVitalStats = true;
            DrawVitalStats();
        }
        if (!CGameLogic::SkipCanBeActivated() || bDrawingVitalStats) {
            HelpTripSkipShown = false;
        } else {
            DrawTripSkip();
            if (!HelpTripSkipShown) {
                SetHelpMessage(TheText.Get("SKIP_1"), true, false, false);
                HelpTripSkipShown = true;
            }
        }
    }

    if (m_bDraw3dMarkers && !TheCamera.m_bWideScreenOn) {
        CRadar::Draw3dMarkers();
    }

    if (!CTimer::GetIsUserPaused()) {
        if (!m_BigMessage[STYLE_MIDDLE][0]) {
            if (CMenuSystem::GetNumMenusInUse()) {
                CMenuSystem::Process(CMenuSystem::MENU_UNDEFINED);
            }
            DrawScriptText(true);
        }
        if (CTheScripts::bDrawSubtitlesBeforeFade) {
            DrawSubtitles();
        }
        DrawHelpText();
        DrawOddJobMessage(true);
        DrawSuccessFailedMessage();
        DrawBustedWastedMessage();
    }
}

// 0x58D490
void CHud::DrawAfterFade() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERNEAREST));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    if (CTimer::GetIsUserPaused() || CReplay::Mode == MODE_PLAYBACK || CWeapon::ms_bTakePhoto)
        return;

    auto vehicle = FindPlayerVehicle();
    if (!vehicle || (!vehicle->IsSubPlane() && !vehicle->IsSubHeli())) {
        if (!CCutsceneMgr::ms_cutsceneProcessing) {
            if (!FrontEndMenuManager.m_bMenuActive && !TheCamera.m_bWideScreenOn && !bScriptDontDisplayAreaName) {
                DrawAreaName();
            }
        }
    }

    if (!m_BigMessage[STYLE_MIDDLE][0]) {
        DrawScriptText(false);
    }

    if (!CTheScripts::bDrawSubtitlesBeforeFade) {
        DrawSubtitles();
    }

    DrawMissionTitle();
    DrawOddJobMessage(false);
}

// 0x58AA50
void CHud::DrawAreaName() {
    if (!m_pZoneName) {
        return;
    }

    if (m_pZoneName != m_pLastZoneName) {
        switch (m_ZoneState) {
        case NAME_DONT_SHOW:
            if (!CTheScripts::bPlayerIsOffTheMap && CTheScripts::bDisplayHud ||
                CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_1 ||
                CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_2
            ) {
                m_ZoneState = NAME_FADE_IN;
                m_ZoneNameTimer = 0;
                m_ZoneFadeTimer = 0;
                m_ZoneToPrint = m_pZoneName;
                if (m_VehicleState == NAME_SHOW || m_VehicleState == NAME_FADE_IN) {
                    m_VehicleState = NAME_FADE_OUT;
                }
            }
            break;
        case NAME_SHOW:
        case NAME_FADE_IN:
        case NAME_FADE_OUT:
            m_ZoneState = NAME_SWITCH;
            m_ZoneNameTimer = 0;
            break;
        case NAME_SWITCH:
            m_ZoneNameTimer = 0;
            break;
        default:
            break;
        }
        m_pLastZoneName = m_pZoneName;
    }

    if (!m_ZoneState)
        return;

    float alpha = 255.0f;
    switch (m_ZoneState) {
    case NAME_SHOW:
        m_ZoneFadeTimer = 1000;
        if (m_ZoneNameTimer > 3000) {
            m_ZoneState = NAME_FADE_OUT;
            m_ZoneFadeTimer = 1000;
        }
        break;

    case NAME_FADE_IN:
        if (!TheCamera.GetFading() && TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            m_ZoneFadeTimer += (int32)CTimer::GetTimeStepInMS();
        }

        if (m_ZoneFadeTimer > 1000) {
            m_ZoneFadeTimer = 1000;
            m_ZoneState = NAME_SHOW;
        }

        if (TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            alpha = (float)m_ZoneFadeTimer / 1000.0f * 255.0f;
            break;
        }
        m_ZoneState = NAME_FADE_OUT;
        m_ZoneFadeTimer = 1000;
        break;

    case NAME_FADE_OUT:
        if (!TheCamera.GetFading() && TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            m_ZoneFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        }

        if (m_ZoneFadeTimer < 0) {
            m_ZoneFadeTimer = 0;
            m_ZoneState = NAME_DONT_SHOW;
        }

        if (TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            alpha = (float)m_ZoneFadeTimer / 1000.0f * 255.0f;
            break;
        }
        m_ZoneFadeTimer = 1000;
        break;

    case NAME_SWITCH:
        m_ZoneFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        if (m_ZoneFadeTimer < 0) {
            m_ZoneFadeTimer = 0;
            m_ZoneState = NAME_FADE_IN;
            m_ZoneToPrint = m_pLastZoneName;
        }
        alpha = (float)m_ZoneFadeTimer / 1000.0f * 255.0f;
        break;

    default:
        break;
    }

    if (m_Message[0] || BigMessageX[STYLE_BOTTOM_RIGHT] != 0.0f || BigMessageX[STYLE_WHITE_MIDDLE] != 0.0f) {
        m_ZoneState = NAME_FADE_OUT;
        return;
    }

    m_ZoneNameTimer += (uint32)CTimer::GetTimeStepInMS();
    CFont::SetProportional(true);
    CFont::SetBackground(false, false);
    CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(1.2f), SCREEN_SCALE_Y(1.9f));
    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap(SCREEN_STRETCH_X(180.0f));
    CFont::SetDropColor({ 0, 0, 0, (uint8)alpha });
    CFont::SetFontStyle(FONT_GOTHIC);

    const CZoneInfo* info = CPopCycle::m_pCurrZoneInfo;
    const auto& color = info->ZoneColor; // Cppcheck: (warning) nullPointerRedundantCheck: Either the condition 'info' is redundant or there is possible null pointer dereference: info.
    if (CGangWars::bGangWarsActive && info && color.r && color.g && color.b) {
        CFont::SetColor({ color.r, color.g, color.b, (uint8)alpha});
    } else {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_BLUE, (uint8)alpha));
    }

    CFont::PrintStringFromBottom(SCREEN_STRETCH_FROM_RIGHT(32.0f), SCREEN_SCALE_FROM_BOTTOM(104.0f) + SCREEN_SCALE_Y(76.0f), m_ZoneToPrint);
    CFont::SetSlant(0.0f);
}

// 0x58CA50
void CHud::DrawBustedWastedMessage() {
    auto& message      = m_BigMessage[STYLE_WHITE_MIDDLE];
    auto& messageX     = BigMessageX[STYLE_WHITE_MIDDLE];
    auto& messageAlpha = BigMessageAlpha[STYLE_WHITE_MIDDLE];

    if (!message[0]) {
        messageX = '\0';
        return;
    }

    if (messageX == 0.0f) {
        messageX = 1.0f;
        messageAlpha = 0.0f;

        if (m_VehicleState) {
            m_VehicleState = NAME_DONT_SHOW;
        }
        if (m_ZoneState) {
            m_ZoneState = NAME_DONT_SHOW;
        }
        return;
    }

    messageAlpha += CTimer::GetTimeStepInMS() * 0.4f;
    messageAlpha = std::min(messageAlpha, 255.0f);

    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(2.1f), SCREEN_SCALE_Y(2.1f));
    CFont::SetProportional(true);
    CFont::SetJustify(false);
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetFontStyle(FONT_GOTHIC);
    CFont::SetEdge(3);
    CFont::SetDropColor({ 0, 0, 0, (uint8)messageAlpha });
    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_GRAY, (uint8)messageAlpha));
    CFont::PrintStringFromBottom(SCREEN_WIDTH / 2.0f, static_cast<float>(RsGlobal.maximumHeight / 2) - SCREEN_SCALE_Y(30.0f), message); // OG: posY static allocated var
}

// 0x589070
void CHud::ResetWastedText() {
    BigMessageX[STYLE_WHITE_MIDDLE] = 0.0f;
    m_BigMessage[STYLE_WHITE_MIDDLE][0] = '\0';

    BigMessageX[STYLE_MIDDLE] = 0.0f;
    m_BigMessage[STYLE_MIDDLE][0] = '\0';
}

// 0x58E020
void CHud::DrawCrossHairs() {
    auto* player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
    const int mode = int(CCamera::GetActiveCamera().m_nMode);
    bool custom = false, circle = mode == 42 || mode == 40 || mode == 52 || mode == 39;
    if (mode == 16) {
        if (FindPlayerVehicle(-1, false) && (FindPlayerVehicle(-1, false)->m_nModelIndex == 520 || FindPlayerVehicle(-1, false)->m_nModelIndex == 425)) custom = true;
    } else if (mode == 7 || mode == 8 || mode == 51 || mode == 34 || mode == 45 || mode == 46) {
        if (player && !player->GetActiveWeapon().IsTypeMelee()) custom = true;
    }
    auto weapon = player->GetActiveWeapon().m_Type;
    const auto* data = reinterpret_cast<const uint8*>(player->GetPlayerData());
    if (!player->m_pTargetedObject && (data[0x34] & 8) && (!player->GetIntelligence()->GetTaskUseGun() || !player->GetIntelligence()->GetTaskUseGun()->m_SkipAim) && (mode == 53 || mode == 55 || mode == 65) && int(player->m_nPedState) != 59 && int(player->m_nPedState) != 57 && ((int(weapon) > 21 && int(weapon) < 32) || int(weapon) == 32 || int(weapon) == 33 || int(weapon) == 38 || int(weapon) == 37)) circle = !(mode == 53 && TheCamera.m_bTransitionState);
    const auto script = int(CTheScripts::bDrawCrossHair);
    if (!custom && !circle && !script) return;
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(2)); RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(0));
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    const float sx = float(RsGlobal.maximumWidth) * f(0x859520), sy = float(RsGlobal.maximumHeight) * f(0x859524);
    const auto rect = [](float left,float bottom,float right,float top) { CRect result;result.left=left;result.bottom=bottom;result.right=right;result.top=top;return result; };
    const auto quadrants = [&](float originX, float originY, float extentX, float extentY) {
        const float right = extentX * .5f + originX, top = extentY * .5f + originY;
        const CRGBA white(255, 255, 255, 255);
        Sprites[SPRITE_SITE_M16].Draw(rect(originX, originY, right, top), white);
        Sprites[SPRITE_SITE_M16].Draw(rect(originX + extentX, originY, right, top), white);
        Sprites[SPRITE_SITE_M16].Draw(rect(originX, originY + extentY, right, top), white);
        Sprites[SPRITE_SITE_M16].Draw(rect(originX + extentX, originY + extentY, right, top), white);
    };
    if (circle && (mode == 53 || mode == 55 || mode == 65)) {
        const float x = float(RsGlobal.maximumWidth) * CCamera::m_f3rdPersonCHairMultX, y = float(RsGlobal.maximumHeight) * CCamera::m_f3rdPersonCHairMultY;
        const float radius = player->GetWeaponRadiusOnScreen();
        if (radius == f(0x858CC4)) CSprite2d::DrawRect(rect(x - 1.f, y - 1.f, x + 1.f, y + 1.f), CRGBA(255, 255, 255, 255));
        const float ex = sx * 64.f * radius, ey = sy * 64.f * radius;
        quadrants((ex * .5f + x) - ex, (ey * .5f + y) - ey, ex, ey);
    } else if (script != 2 && (mode == 34 || mode == 42 || mode == 41 || mode == 45)) {
        const float ex = sx * 64.f, ey = sy * 64.f;
        quadrants(float(RsGlobal.maximumWidth / 2) - ex * .5f, float(RsGlobal.maximumHeight / 2) - ey * .5f, ex, ey);
    } else {
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(2));
        RwTexture* texture = nullptr;
        float ex{}, ey{}, ox{}, oy{};
        if (FindPlayerPed(-1)->GetActiveWeapon().m_Type == WEAPON_CAMERA || FindPlayerPed(-1)->GetActiveWeapon().m_Type == WEAPON_SNIPERRIFLE || script == 2) {
            if (FindPlayerPed(-1)->GetActiveWeapon().m_Type == WEAPON_CAMERA || script == 2) { ex = sx * 256.f; ey = sy * 192.f; }
            else { ex = sx * 210.f; ey = sy * 210.f; }
            auto* info = CWeaponInfo::GetWeaponInfo(weapon, static_cast<eWeaponSkill>(1));
            if (info->m_nModelId1 > 0) {
                auto* model = CModelInfo::GetModelInfo(info->m_nModelId1);
                auto* txd = CTxdStore::ms_pTxdPool->GetAt(model->m_nTxdIndex);
                if (txd->m_pRwDictionary) texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(model->m_nKey, "CROSSHAIR"));
            }
        } else if (mode == 8 || mode == 16 || mode == 40 || mode == 51 || mode == 52) {
            texture = Sprites[SPRITE_SITE_ROCKET].m_pTexture; ex = sx * 24.f; ey = sy * 24.f; ox = sx * 20.f; oy = sy * 20.f;
        }
        if (texture) {
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(0)); RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(3)); RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);
            ex *= .5f; ey *= .5f;
            const float cx = float(RsGlobal.maximumWidth / 2), cy = float(RsGlobal.maximumHeight / 2);
            const auto sprite = [&](float x,float y,uint8 u,uint8 v) { CSprite::RenderOneXLUSprite(CVector{x,y,1.f}, CVector2D{ex,ey}, 255,255,255,255,.01f,255,u,v); };
            sprite((cx-ex)-ox,(cy-ey)-oy,0,0); sprite(cx+ex+ox,(cy-ey)-oy,1,0); sprite((cx-ex)-ox,cy+ey+oy,0,1); sprite(cx+ex+ox,cy+ey+oy,1,1);
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(0));
        }
    }
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(5)); RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(6)); RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(1));
}

// 0x58D580
float CHud::DrawFadeState(DRAW_FADE_STATE fadingElement, int32 forceFadingIn) {
    uint32 state, timer, fadeTimer;
    switch (fadingElement) {
    case WANTED_STATE:
        fadeTimer = m_WantedFadeTimer;
        state = m_WantedState;
        timer = m_WantedTimer;
        break;
    case ENERGY_LOST_STATE:
        fadeTimer = m_EnergyLostFadeTimer;
        state = m_EnergyLostState;
        timer = m_EnergyLostTimer;
        break;
    case DISPLAY_SCORE_STATE:
        fadeTimer = m_DisplayScoreFadeTimer;
        state = m_DisplayScoreState;
        timer = m_DisplayScoreTimer;
        break;
    case WEAPON_STATE:
        fadeTimer = m_WeaponFadeTimer;
        state = m_WeaponState;
        timer = m_WeaponTimer;
        break;
    default:
        state = fadingElement;
        timer = fadingElement;
        fadeTimer = fadingElement;
        break;
    }

    if (forceFadingIn) {
        switch (state) {
        case NAME_DONT_SHOW:
            fadeTimer = 0;
            break;
        case NAME_SWITCH:
        case NAME_FADE_OUT:
            timer = 5;
            state = NAME_FADE_IN;
            break;
        default:
            break;
        }
    }

    float alpha = 255.0f;
    if (state != NAME_DONT_SHOW) {
        switch (state) {
        case NAME_SHOW:
            fadeTimer = 1000;
            if (timer > 10'000) {
                fadeTimer = 3000;
                state = NAME_FADE_OUT;
            }
            break;
        case NAME_FADE_IN:
            fadeTimer += (uint32)CTimer::GetTimeStepInMS();
            if (fadeTimer > 1000) {
                state = NAME_SHOW;  
                fadeTimer = 1000;
            }
            alpha = float(fadeTimer) / 1000.0f * 255.0f;
            break;
        case NAME_FADE_OUT:
            fadeTimer -= (uint32)CTimer::GetTimeStepInMS();
            if (fadeTimer < 0) {
                fadeTimer = 0;
                state = NAME_DONT_SHOW;
            }
            alpha = float(fadeTimer) / 1000.0f * 255.0f;
            break;
        default:
            break;
        }
        timer += (uint32)CTimer::GetTimeStepInMS();
    }

    switch (fadingElement) {
    case WANTED_STATE:
        m_WantedFadeTimer = fadeTimer;
        m_WantedState = state;
        m_WantedTimer = timer;
        break;
    case ENERGY_LOST_STATE:
        m_EnergyLostFadeTimer = fadeTimer;
        m_EnergyLostState = state;
        m_EnergyLostTimer = timer;
        break;
    case DISPLAY_SCORE_STATE:
        m_DisplayScoreFadeTimer = fadeTimer;
        m_DisplayScoreState = state;
        m_DisplayScoreTimer = timer;
        break;
    case WEAPON_STATE:
        m_WeaponFadeTimer = fadeTimer;
        m_WeaponState = state;
        m_WeaponTimer = timer;
        break;
    default:
        break;
    }

    return std::clamp(alpha, 0.0f, 255.0f);
}

// 0x58B6E0
void CHud::DrawHelpText() {
    if (!m_pHelpMessage[0]) { m_nHelpMessageState = 0; return; }
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    const float sx = float(RsGlobal.maximumWidth) * f(0x859520), sy = float(RsGlobal.maximumHeight) * f(0x859524);
    if (!CMessages::StringCompare(m_pHelpMessage, m_pLastHelpMessage, 400)) {
        if (!m_nHelpMessageState) {
            m_nHelpMessageState = 2; m_nHelpMessageTimer = 0; m_nHelpMessageFadeTimer = 0;
            CMessages::StringCopy(m_pHelpMessageToPrint, m_pHelpMessage, 400);
            CFont::SetOrientation(static_cast<eFontAlignment>(1)); CFont::SetJustify(false); CFont::SetWrapx((sx * 200.f + sx * 34.f) - sx * 4.f); CFont::SetFontStyle(static_cast<eFontStyle>(1)); CFont::SetBackground(true, true); CFont::SetDropShadowPosition(0);
            m_fHelpMessageTime = float(CFont::GetNumberLines(sx * 34.f, sy * 28.f, m_pHelpMessageToPrint) + 3);
            CFont::SetWrapx(float(RsGlobal.maximumWidth));
            AudioEngine.ReportFrontendAudioEvent(static_cast<eAudioEvents>(32), 0.f, 1.f);
        } else if (m_nHelpMessageState >= 1 && m_nHelpMessageState <= 4) { m_nHelpMessageState = 4; m_nHelpMessageTimer = 5; }
        CMessages::StringCopy(m_pLastHelpMessage, m_pHelpMessage, 400);
    }
    if (!m_nHelpMessageState) return;
    float alpha = 200.f;
    const auto ticks = [&] { return int32((CTimer::GetTimeStep() * f(0x858B38)) * f(0x858C4C)); };
    const auto fade = [&]() -> int32& { return reinterpret_cast<int32&>(m_nHelpMessageFadeTimer); };
    if (m_nHelpMessageState == 1) {
        fade() = 600;
        if (!m_bHelpMessagePermanent && (m_fHelpMessageTime * 1000.f < float(int32(m_nHelpMessageTimer)) || (m_bHelpMessageQuick && 3000.f < float(int32(m_nHelpMessageTimer))))) { m_nHelpMessageState = 3; fade() = 600; }
    } else if (m_nHelpMessageState == 2) {
        if (!TheCamera.m_bWideScreenOn) { fade() += ticks() * 2; if (float(fade()) > 0.f) { fade() = 0; m_nHelpMessageState = 1; } alpha = (float(fade()) * f(0x858CDC)) * 200.f; }
    } else if (m_nHelpMessageState == 3) {
        fade() -= ticks() * 2; if (float(fade()) < 0.f || TheCamera.m_bWideScreenOn) { fade() = 0; m_nHelpMessageState = 0; } alpha = (float(fade()) * f(0x858CDC)) * 200.f;
    } else if (m_nHelpMessageState == 4) {
        fade() -= ticks() * 2; if (float(fade()) < 0.f) { fade() = 0; m_nHelpMessageState = 2; CMessages::StringCopy(m_pHelpMessageToPrint, m_pLastHelpMessage, 400); } alpha = (float(fade()) * f(0x858CDC)) * 200.f;
    }
    if (CCutsceneMgr::ms_running) return;
    m_nHelpMessageTimer += uint32(ticks());
    CFont::SetAlphaFade(alpha); CFont::SetProportional(true); CFont::SetScaleForCurrentLanguage(sx * f(0x8665BC), sy * f(0x858F14));
    const auto finish = [] { CFont::SetAlphaFade(255.f); };
    if (!m_nHelpMessageStatId && (m_BigMessage[0][0] || m_BigMessage[4][0] || StaticRef<bool>(0x96C014))) { finish(); return; }
    if (m_nHelpMessageStatId && TheCamera.m_bWideScreenOn) { finish(); return; }
    CFont::SetOrientation(static_cast<eFontAlignment>(1)); CFont::SetJustify(false);
    if (!m_nHelpMessageStatId) {
        const float nominal = sx * 200.f;
        CFont::SetWrapx(sx * m_fHelpMessageBoxWidth == nominal ? (sx * 34.f + nominal) - sx * 4.f : sx * 34.f + (m_fHelpMessageBoxWidth - 4.f) * sx);
    } else CFont::SetWrapx(float(RsGlobal.maximumWidth));
    CFont::SetFontStyle(static_cast<eFontStyle>(1)); CFont::SetBackground(true, true); CFont::SetDropShadowPosition(0); CFont::SetBackgroundColor(CRGBA(0, 0, 0, uint8(int32(alpha)))); CFont::SetColor(HudColour.GetRGB(static_cast<eHudColours>(4)));
    const float offset = StaticRef<float>(0x8D0938);
    float x = sx * 34.f, y = sy * 28.f + ((150.f - offset) * .6f) * sy;
    if (!m_nHelpMessageStatId) {
        const int shift = TheCamera.m_bWideScreenOn && !StaticRef<bool>(0xBA6793) ? 56 : 0;
        y = sy * 28.f + ((float(150 + shift) - offset) * .6f) * sy;
    } else {
        const char* format = m_nHelpMessageStatId < 10 ? "STAT00%d" : m_nHelpMessageStatId < 100 ? "STAT0%d" : "STAT%d";
        std::sprintf(gString, format, int(m_nHelpMessageStatId));
        x = sx * 10.f + sx * 34.f + CFont::GetStringWidth(TheText.Get(gString), true, false);
        CFont::SetWrapx(sx * 75.f + x); CFont::PrintString(sx * 34.f, y, TheText.Get(gString));
        AsciiToGxtChar(reinterpret_cast<const char*>(0x866C18), reinterpret_cast<GxtChar*>(0xC1B100));
        const float value = m_nHelpMessageStatId == 336 ? float(FindPlayerPed(-1)->GetPlayerGroup().GetMembership().CountMembersExcludingLeader()) : CStats::GetStatValue(static_cast<eStats>(m_nHelpMessageStatId));
        const bool positive = m_pHelpMessageToPrint[0] == StaticRef<GxtChar>(0xC1B100);
        const auto progressColor = HudColour.GetRGBA(static_cast<eHudColours>(positive ? 1 : 0), uint8(int32(alpha)));
        const auto barColor = HudColour.GetRGBA(static_cast<eHudColours>(4), uint8(int32(alpha)));
        const float inverse = 1.f / float(m_nHelpMessageMaxStatValue);
        const float progressAdd = (inverse * m_fHelpMessageStatUpdateValue) * 100.f;
        const float progress = (inverse * value) * 100.f;
        CSprite2d::DrawBarChart(x, sy * 28.f + (155.f - offset) * .6f, uint16(int32(sx * f(0x86503C))), uint8(int32(sy * f(0x858CCC))), progress > 2.f ? progress : 2.f, int8(int32(progressAdd > 3.f ? progressAdd : 3.f)), 0, 0, barColor, progressColor);
        x += sx * 65.f;
    }
    CFont::PrintString(x, y, m_pHelpMessageToPrint); CFont::SetWrapx(float(RsGlobal.maximumWidth)); finish();
}


// 0x58B180
void CHud::DrawMissionTimers() {
    if ((m_BigMessage[4][0] && !StaticRef<bool>(0xBAA3FA)) || StaticRef<bool>(0x96C014)) return;
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    const float sx = float(RsGlobal.maximumWidth) * f(0x859520), sy = float(RsGlobal.maximumHeight) * f(0x859524);
    const auto focus = CWorld::PlayerInFocus;
    const bool coop = StaticRef<uint32>(0xB7CF28) != 0;
    float clockY = GetYPosBasedOnHealth(1, GetYPosBasedOnHealth(focus, sy * f(0x859518), 12), 12);
    float counterY = GetYPosBasedOnHealth(1, GetYPosBasedOnHealth(focus, sy * f(0x858BA4) + sy * f(0x859518), 12), 12);
    if (coop) { clockY += sy * f(0x858C90); counterY += sy * f(0x858C90); }
    CFont::SetProportional(true); CFont::SetBackground(false, false); CFont::SetScale(sx * f(0x858B8C), sy);
    CFont::SetOrientation(static_cast<eFontAlignment>(2)); CFont::SetRightJustifyWrap(0.f); CFont::SetFontStyle(static_cast<eFontStyle>(2));
    CFont::SetWrapx(sx * f(0x85951C)); CFont::SetEdge(2); CFont::SetDropColor(CRGBA(0, 0, 0, 255));
    auto* clock = reinterpret_cast<const uint8*>(0xBA1788);
    auto* flags = reinterpret_cast<uint8*>(0xBAA394);
    if (coop && !clock[0x38]) StaticRef<uint8>(0xBAA38A) = 0;
    for (int i = 0; i < 4; ++i) if (!*reinterpret_cast<const uint8*>(0xBA1806 + 0x44 * i)) flags[i] = 0;
    if (StaticRef<uint8>(0xBA18D8) != 1) return;
    GxtChar text[200];
    const float textX = float(RsGlobal.maximumWidth) - sx * f(0x85950C);
    if (clock[0x38] == 1) {
        auto& blink = StaticRef<int16>(0xBAA388);
        if (!StaticRef<uint8>(0xBAA38A)) blink = 1;
        StaticRef<uint8>(0xBAA38A) = 1;
        if (blink && ++blink > 50) blink = 0;
        CFont::SetColor(HudColour.GetRGB(static_cast<eHudColours>(4)));
        if ((CTimer::GetFrameCounter() & 4) || !blink) {
            AsciiToGxtChar(reinterpret_cast<const char*>(clock + 0xE), text); CFont::PrintString(textX, clockY, text);
            if (clock[4]) CFont::PrintString(textX - sx * f(0x85991C), clockY, TheText.Get(reinterpret_cast<const char*>(clock + 4)));
        }
    } else {
        counterY = GetYPosBasedOnHealth(focus, sy * f(0x859518), 12);
        if (StaticRef<uint8>(0xB7D077) < 101) counterY -= sy * f(0x858CCC);
        if (coop) counterY += sy * f(0x858C90);
    }
    for (int i = 0; i < 4; ++i) {
        const auto* counter = reinterpret_cast<const uint8*>(0xBA17C8 + i * 0x44);
        if (counter[0x3E] != 1) continue;
        auto& blink = *reinterpret_cast<int16*>(0xBAA38C + i * 2);
        if (!flags[i] && counter[0x3F] == 1) blink = 1;
        flags[i] = 1;
        if (blink && ++blink > 50) blink = 0;
        if (!(CTimer::GetFrameCounter() & 4) && blink) continue;
        const auto color = static_cast<eHudColours>(counter[0x40]);
        CFont::SetColor(HudColour.GetRGB(color));
        const float y = ((sy * f(0x858BA4)) * float(i)) * sy + counterY;
        if (*reinterpret_cast<const int16*>(counter + 0x12) == 1) {
            const int16 value = int16(std::atol(reinterpret_cast<const char*>(counter + 0x14)));
            const float width = sx * f(0x866C10);
            CSprite2d::DrawBarChart(textX - width, sy * f(0x858B44) + y, uint16(int32(width)), uint8(int32(sy * f(0x85EED4))), (float(value) * f(0x858C58)) * f(0x858628), 0, 0, 1, HudColour.GetRGB(color), CRGBA(0, 0, 0, 0));
        } else {
            AsciiToGxtChar(reinterpret_cast<const char*>(counter + 0x14), text); CFont::PrintString(textX, y, text);
        }
        if (counter[8]) CFont::PrintString(textX - sx * f(0x85991C), y, TheText.Get(reinterpret_cast<const char*>(counter + 8)));
    }
}

// 0x58D240
void CHud::DrawMissionTitle() {
    auto& message      = m_BigMessage[STYLE_BOTTOM_RIGHT];
    auto& messageX     = BigMessageX[STYLE_BOTTOM_RIGHT];
    auto& messageAlpha = BigMessageAlpha[STYLE_BOTTOM_RIGHT];
    auto& messageInUse = BigMessageInUse[STYLE_BOTTOM_RIGHT];

    if (!message[0]) {
        messageX = 0.0f;
        return;
    }

    if (messageX == 0.0f) {
        messageInUse = -60.0f;
        messageX = 1.0f;
        m_ZoneState = NAME_DONT_SHOW;
        m_ZoneFadeTimer = 0;
        SetHelpMessage(nullptr, true, false, false);
        return;
    }

    CFont::SetBackground(false, false);
    CFont::SetProportional(true);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetFontStyle(FONT_PRICEDOWN);
    CFont::SetScale(SCREEN_STRETCH_X(1.0f), SCREEN_SCALE_Y(1.3f));

    if (messageInUse >= SCREEN_WIDTH - 20.0f) { // magic shit
        messageX += CTimer::GetTimeStep();
        if (messageX >= 120.0f) {
            messageX = 120.0f;
            messageAlpha -= CTimer::GetTimeStepInMS();
        }
        if (messageAlpha <= 0.0f) {
            messageAlpha = 0.0f;
            message[0] = '\0';
            messageX = 0.0f;
        }
    } else {
        messageAlpha = 255.0f;
        messageInUse += CTimer::GetTimeStepInMS() * 0.3f;
    }

    CFont::SetEdge(2);
    CFont::SetDropColor({ 0, 0, 0, uint8(messageAlpha) });
    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GOLD, (uint8)messageAlpha));
    CFont::PrintStringFromBottom(SCREEN_SCALE_FROM_RIGHT(20.0f), SCREEN_SCALE_FROM_BOTTOM(115.0f), message);
    CFont::SetEdge(0);
}

// It looks like the original, but needs to be recheck
// 0x58CC80
void CHud::DrawOddJobMessage(bool displayImmediately) {
    const auto& m1 = m_BigMessage[STYLE_BOTTOM_RIGHT];
    const auto& m4 = m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER];
    if (displayImmediately == CTheScripts::bDrawOddJobTitleBeforeFade && !m1[0] && m4[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(0.6f), SCREEN_SCALE_Y(1.35f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(350.0f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_GOLD));
        CFont::PrintStringFromBottom(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(140.0f), m4);
    }

    if (!displayImmediately)
        return;

    const auto& m6 = m_BigMessage[STYLE_LIGHT_BLUE_TOP];
    if (m6[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(1.0f), SCREEN_SCALE_Y(1.8f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(500.0f));
        CFont::SetFontStyle(FONT_PRICEDOWN);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(60.0f), m6);
    }

    const auto& m3 = m_BigMessage[STYLE_MIDDLE_SMALLER];
    if (m3[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(0.6f), SCREEN_SCALE_Y(1.35f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(500.0f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_GOLD));
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(155.0f), m3);
    }

    if (OddJob2OffTimer > 0.0f) {
        OddJob2OffTimer -= CTimer::GetTimeStepInMS();
    }

    const auto& m5 = m_BigMessage[STYLE_WHITE_MIDDLE_SMALLER];
    if (!m5[0])
        return;

    if (OddJob2OffTimer > 0.0f)
        return;

    switch (OddJob2On) {
    case 0:
        OddJob2XOffset = 380.0f;
        OddJob2On = 1;
        break;
    case 1:
        if (OddJob2XOffset <= 2.0f) {
            OddJob2On = 2;
            OddJob2Timer = 0;
        } else {
            OddJob2XOffset -= std::min(OddJob2XOffset / 6.0f, 40.0f);
        }
        break;
    case 2:
        OddJob2Timer += (uint16)CTimer::GetTimeStepInMS();
        if (OddJob2Timer > 1500) {
            OddJob2On = 3;
        }
        break;
    case 3:
        OddJob2XOffset -= std::max(OddJob2XOffset / 5.0f, 30.0f);
        if (OddJob2XOffset < -380.0f) {
            OddJob2On = 0;
            OddJob2OffTimer = 5000.0f;
        }
        break;
    default:
        break;
    }

    if (!m1[0]) {
        CFont::SetBackground(false, false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(0.6f), SCREEN_SCALE_Y(1.35f));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(SCREEN_STRETCH_X(500.0f));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
        CFont::PrintString(static_cast<float>(RsGlobal.maximumWidth / 2), SCREEN_STRETCH_Y(217.0f), m5);
    }
}

// 0x58A330
void CHud::DrawRadar() {
    if (CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_1 ||
        CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_2 ||
        FrontEndMenuManager.m_nRadarMode == eRadarMode::RADAR_MODE_OFF ||
        (m_ItemToFlash == ITEM_RADAR && EachFrames(8))
    ) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,     RWRSTATE(rwFILTERNEAREST));

    CRadar::DrawMap();

    if (FrontEndMenuManager.m_nRadarMode == eRadarMode::RADAR_MODE_BLIPS_ONLY) {
        CRadar::DrawBlips();
        return;
    }

    CVehicle* vehicle = FindPlayerVehicle();
    CRect rect;
    if (vehicle && vehicle->IsSubPlane() && vehicle->m_nModelIndex != MODEL_VORTEX) {
        float angle = PI - std::atan2(-vehicle->m_matrix->GetRight().z, vehicle->m_matrix->GetUp().z);
        CRadar::DrawRotatingRadarSprite(
            Sprites[SPRITE_RADAR_RING_PLANE],
            SCREEN_STRETCH_X(87.0f),
            SCREEN_STRETCH_FROM_BOTTOM(66.0f),
            angle,
            (uint32)SCREEN_STRETCH_X(78.0f),
            (uint32)SCREEN_STRETCH_Y(59.0f),
            CRGBA(255, 255, 255, 255)
        );
    }

    CPlayerPed* player = FindPlayerPed();
    // Draws Altimeter on Planes And Helis or when parachuting down
    if (vehicle && (vehicle->IsSubPlane() || vehicle->IsSubHeli() && vehicle->m_nModelIndex != MODEL_VORTEX)
        || player->GetActiveWeapon().m_Type == WEAPON_PARACHUTE
    ) {
        rect.left   = SCREEN_STRETCH_X(40.0f) - SCREEN_STRETCH_X(20.0f);
        rect.bottom    = SCREEN_STRETCH_FROM_BOTTOM(104.0f);
        rect.right  = SCREEN_STRETCH_X(40.0f) - SCREEN_STRETCH_X(10.0f);
        rect.top = SCREEN_STRETCH_Y(76.0f) + SCREEN_STRETCH_FROM_BOTTOM(104.0f);
        CSprite2d::DrawRect(rect, { 10, 10, 10, 100 }); // rectangle

        const CVector& pos = vehicle ? vehicle->GetPosition() : player->GetPosition();
        auto lineY = 950.0f;
        if (pos.z <= 200.0f) {
            lineY = 200.0f;
        };
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

        rect.left   = SCREEN_STRETCH_X(40.0f) - SCREEN_STRETCH_X(25.0f);
        rect.bottom    = SCREEN_STRETCH_FROM_BOTTOM(104.0f) + SCREEN_STRETCH_Y(76.0f) - std::min(SCREEN_STRETCH_Y(76.0f), SCREEN_STRETCH_Y(76.0f) * pos.z / lineY);
        rect.right  = SCREEN_STRETCH_X(40.0f) - 5.0f;
        rect.top = rect.bottom + 2.0f;
        CSprite2d::DrawRect(rect, { 200, 200, 200, 200 }); // horizontal line (current height)
    }

    // NOTSA: rects are optimized
    const auto black = CRGBA(0, 0, 0, 255);

    rect.left   = SCREEN_STRETCH_X(36.0f);
    rect.bottom    = SCREEN_STRETCH_FROM_BOTTOM(108.0f);
    rect.right  = SCREEN_STRETCH_X(87.0f);
    rect.top = SCREEN_STRETCH_FROM_BOTTOM(66.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // top left

    rect.bottom = SCREEN_STRETCH_FROM_BOTTOM(24.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // bottom left

    rect.left = SCREEN_STRETCH_X(138.0f);
    rect.bottom  = SCREEN_STRETCH_FROM_BOTTOM(108.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // top right

    rect.bottom = SCREEN_STRETCH_FROM_BOTTOM(24.0f);
    Sprites[SPRITE_RADAR_DISC].Draw(rect, black); // bottom right

    CRadar::DrawBlips();
}

// 0x58C080
void CHud::DrawScriptText(bool isBeforeFade) {
    CTheScripts::DrawScriptSpritesAndRectangles(isBeforeFade);

    for (auto& t : CTheScripts::IntroTextLines) {
        if (!t.GXTKey[0]) { /* empty key? */
            continue;
        }
        if (t.IsDrawBeforeFade != isBeforeFade) {
            continue;
        }

        CFont::SetScale(SCREEN_SCALE_X(t.Scale.x), SCREEN_SCALE_Y(t.Scale.y / 2.0f));
        CFont::SetColor(t.Color);
        CFont::SetJustify(t.Justify);
        if (t.HasRightJustify) {
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        } else {
            CFont::SetOrientation(t.IsCentered ? eFontAlignment::ALIGN_CENTER : eFontAlignment::ALIGN_LEFT);
        }
        CFont::SetWrapx(SCREEN_SCALE_X(t.WrapX));
        CFont::SetCentreSize(SCREEN_SCALE_X(t.CentreSize));
        CFont::SetBackground(t.HasBg, false);
        CFont::SetBackgroundColor(t.BgColor);
        CFont::SetProportional(t.IsProportional);
        CFont::SetDropColor(t.DropShadowColor);
        if (t.TextEdge) {
            CFont::SetEdge(t.TextEdge);
        } else {
            CFont::SetDropShadowPosition(t.DropShadow);
        }
        CFont::SetFontStyle((eFontStyle)t.FontStyle);

        GxtChar text[400];
        CMessages::InsertNumberInString(
            TheText.Get(t.GXTKey),
            t.NumberToInsert1,
            t.NumberToInsert2,
            -1,
            -1,
            -1,
            -1,
            text
        );
        CMessages::InsertPlayerControlKeysInString(text);
        // todo: Replace DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT
        // The first letter doesn't look good in window mode, but it looks fine in full-screen mode
        CFont::PrintString(
            SCREEN_SCALE_FROM_RIGHT(DEFAULT_SCREEN_WIDTH - t.Pos.x),
            SCREEN_SCALE_FROM_BOTTOM(DEFAULT_SCREEN_HEIGHT - t.Pos.y),
            text
        );
        CFont::SetEdge(0);
    }
}

namespace {
float SubtitleScaleY(int height, bool widescreen) {
    float result;
    __asm {
        fild height
        fmul dword ptr ds:[00859524h]
        cmp widescreen, 0
        je subtitle_scale_y_normal
        fmul dword ptr ds:[00858F08h]
        jmp subtitle_scale_y_store
    subtitle_scale_y_normal:
        fmul dword ptr ds:[00866C20h]
    subtitle_scale_y_store:
        fstp result
    }
    return result;
}
float SubtitleCentreVitals(int width) {
    float result;
    __asm {
        fild width
        fld dword ptr ds:[00859520h]
        fmul st, st(1)
        fld dword ptr ds:[00859000h]
        fmul st, st(1)
        fld st(1)
        fmul dword ptr ds:[00858BA4h]
        fsubr st, st(3)
        fsub st, st(1)
        fxch st(2)
        fmul dword ptr ds:[00865374h]
        fadd st, st(1)
        fsubp st(2), st
        fxch st(1)
        fmul dword ptr ds:[00858C98h]
        fstp result
        fstp st(0)
        fstp st(0)
    }
    return result;
}
float SubtitleScaleX(int width, bool vitals) {
    float result;
    __asm {
        fild width
        fmul dword ptr ds:[00859520h]
        fmul dword ptr ds:[00866C24h]
        cmp vitals, 0
        je subtitle_scale_store
        fmul dword ptr ds:[00858C98h]
    subtitle_scale_store:
        fstp result
    }
    return result;
}
}
// 0x58C250
void CHud::DrawSubtitles() {
    if (!m_Message[0]) return;
    if (m_BigMessage[STYLE_WHITE_MIDDLE][0] && !CGameLogic::IsCoopGameGoingOn()) return;
    if (int32(m_VehicleState)) m_VehicleState = static_cast<eNameState>(3);
    if (int32(m_ZoneState)) m_ZoneState = static_cast<eNameState>(3);
    CFont::SetBackground(false, false);
    CFont::SetBackgroundColor(CRGBA{0,0,0,128});
    CFont::SetOrientation(static_cast<eFontAlignment>(0));
    CFont::SetProportional(true);
    CFont::SetDropShadowPosition(0);
    CFont::SetFontStyle(static_cast<eFontStyle>(1));
    CFont::SetColor(CRGBA{225,225,225,255});
    CFont::SetDropShadowPosition(2);
    CFont::SetDropColor(CRGBA{0,0,0,255});
    const double sx = double(RsGlobal.maximumWidth) * double(0.0015625f);
    const double sy = double(RsGlobal.maximumHeight) * double(0.002232143f);
    auto& wasWidescreen = StaticRef<bool>(0xBAB214);
    float x, y;
    if (!TheCamera.m_bWideScreenOn) {
        if (wasWidescreen) m_Message[0] = 0;
        wasWidescreen = false;
        CFont::SetScaleForCurrentLanguage(SubtitleScaleX(RsGlobal.maximumWidth, false), SubtitleScaleY(RsGlobal.maximumHeight, false));
        // Como el original: mantiene ambos operandos en x87 hasta guardar la coordenada.
        const int subtitleHeight = RsGlobal.maximumHeight;
        __asm {
            fild subtitleHeight
            fld dword ptr ds:[00859524h]
            fmul st, st(1)
            fld dword ptr ds:[00866C1Ch]
            fmul st, st(1)
            fsubr st, st(2)
            fxch st(1)
            fadd st, st
            fsubp st(1), st
            fstp y
            fstp st(0)
        }
        if (CTheScripts::bUseMessageFormatting) {
            CFont::SetCentreSize(float(sx * CTheScripts::MessageWidth));
            x = float(sx * CTheScripts::MessageCentre);
        } else {
            const double left = sx * 140.0 + sx * 8.0;
            const double width = ((double(RsGlobal.maximumWidth) - sx * 20.0) - sx * 8.0) - left;
            if (bDrawingVitalStats) {
                CFont::SetScaleForCurrentLanguage(SubtitleScaleX(RsGlobal.maximumWidth, true), SubtitleScaleY(RsGlobal.maximumHeight, false));
                CFont::SetCentreSize(SubtitleCentreVitals(RsGlobal.maximumWidth));
                x = float(sx * 40.0 + width * 0.5 + left);
            } else {
                CFont::SetCentreSize(float(width));
                x = float(width * 0.5 + left);
            }
        }
    } else {
        wasWidescreen = true;
        if (!StaticRef<bool>(0xBA678C) && CCutsceneMgr::ms_running) {
            CFont::SetDropShadowPosition(0);
            return;
        }
        CFont::SetCentreSize(float(double(RsGlobal.maximumWidth) - sx * 60.0));
        CFont::SetScale(SubtitleScaleX(RsGlobal.maximumWidth, false), SubtitleScaleY(RsGlobal.maximumHeight, true));
        y = float(double(RsGlobal.maximumHeight) - sy * 80.0);
        x = float(RsGlobal.maximumWidth / 2);
    }
    CFont::PrintString(x, y, m_Message);
    CFont::SetDropShadowPosition(0);
}

// 0x58C6A0
void CHud::DrawSuccessFailedMessage() {
    auto& initialized = StaticRef<uint32>(0xBAB21C);
    auto& baseline = StaticRef<float>(0xBAB218);
    const auto constant = [](uintptr_t address) { return *reinterpret_cast<const float*>(address); };
    const float sy = float(RsGlobal.maximumHeight) * constant(0x859524);
    const float middle = float(RsGlobal.maximumHeight / 2) - sy * constant(0x85862C);
    if (!(initialized & 1)) { initialized |= 1; baseline = middle; }
    auto& message = m_BigMessage[0];
    auto& age = BigMessageX[0];
    auto& position = BigMessageInUse[0];
    auto& alpha = BigMessageAlpha[0];
    if (!message[0]) { age = 0.f; return; }
    if (age == 0.f) {
        position = -60.f; age = 1.f; alpha = 0.f;
        if (m_BigMessage[3][0] || m_BigMessage[5][0]) baseline = middle + sy * constant(0x858FE8);
        else if (!m_BigMessage[2][0] && CFont::GetNumberLines(float(RsGlobal.maximumWidth / 2), middle, message) > 1) baseline = middle - sy * constant(0x858B48);
        else baseline = middle;
        return;
    }
    CFont::SetBackground(false, false);
    CFont::SetScale(float(double(float(RsGlobal.maximumWidth) * constant(0x859520)) * *reinterpret_cast<const double*>(0x866C30)), float(double(sy) * *reinterpret_cast<const double*>(0x866C38)));
    CFont::SetProportional(true);
    CFont::SetJustify(false);
    CFont::SetOrientation(static_cast<eFontAlignment>(0));
    CFont::SetCentreSize(float(RsGlobal.maximumWidth) * constant(0x859520) * constant(0x866C28));
    CFont::SetFontStyle(static_cast<eFontStyle>(3));
    CFont::SetEdge(2);
    const auto opacity = uint8(int32(alpha));
    CFont::SetDropColor(CRGBA(0, 0, 0, opacity));
    CFont::SetColor(HudColour.GetRGBA(static_cast<eHudColours>(6), opacity));
    if (float(RsGlobal.maximumWidth - 20) > position) {
        const uint32 ticks = uint32(int32((CTimer::GetTimeStep() * constant(0x858B38)) * constant(0x858C4C)));
        const float delta = float(ticks) * constant(0x858C24);
        position += delta; alpha += delta;
        if (alpha > 255.f) alpha = 255.f;
    } else {
        age += CTimer::GetTimeStep();
        if (age >= constant(0x858BB0)) {
            age = 120.f;
            const uint32 ticks = uint32(int32((CTimer::GetTimeStep() * constant(0x858B38)) * constant(0x858C4C)));
            alpha -= float(ticks) * constant(0x858C24);
        }
        if (alpha < 0.f) { alpha = 0.f; message[0] = 0; }
    }
    CFont::PrintString(float(RsGlobal.maximumWidth / 2), baseline, message);
}

// 0x58AEA0
void CHud::DrawVehicleName() {
    if (!m_pVehicleName) {
        m_VehicleState = NAME_DONT_SHOW;
        m_VehicleNameTimer = 0;
        m_VehicleFadeTimer = 0;
        m_pLastVehicleName = nullptr;
        return;
    }

    if (m_pVehicleName != m_pLastVehicleName) {
        switch (m_VehicleState) {
        case NAME_DONT_SHOW:
            m_VehicleState = NAME_FADE_IN;
            m_VehicleNameTimer = 0;
            m_VehicleFadeTimer = 0;
            m_pVehicleNameToPrint = m_pVehicleName;
            if (m_ZoneState == NAME_SHOW || m_ZoneState == NAME_FADE_IN) {
                m_ZoneState = NAME_FADE_OUT;
            }
            break;
        case NAME_SHOW:
        case NAME_FADE_IN:
        case NAME_FADE_OUT:
        case NAME_SWITCH:
            m_VehicleState = NAME_SWITCH;
            m_VehicleNameTimer = 0;
            break;
        default:
            break;
        }
        m_pLastVehicleName = m_pVehicleName;
    }

    if (!m_VehicleState)
        return;

    float alpha = 0.0f;
    switch (m_VehicleState) {
    case NAME_SHOW:
        if (m_VehicleNameTimer > 3000) {
            m_VehicleState = NAME_FADE_OUT;
            m_VehicleFadeTimer = 1000;
        }
        alpha = 255.0f;
        break;
    case NAME_FADE_IN:
        m_VehicleFadeTimer += (int32)CTimer::GetTimeStepInMS();
        if (m_VehicleFadeTimer > 1000) {
            m_VehicleFadeTimer = 1000;
            m_VehicleState = NAME_SHOW;
        }
        alpha = float(m_VehicleFadeTimer) / 1000.0f * 255.0f;
        break;
    case NAME_FADE_OUT:
        m_VehicleFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        if (m_VehicleFadeTimer < 0) {
            m_VehicleState = NAME_DONT_SHOW;
            m_VehicleFadeTimer = 0;
        }
        alpha = float(m_VehicleFadeTimer) / 1000.0f * 255.0f;
        break;
    case NAME_SWITCH:
        m_VehicleFadeTimer -= (int32)CTimer::GetTimeStepInMS();
        if (m_VehicleFadeTimer < 0) {
            m_VehicleNameTimer = 0;
            m_VehicleState = NAME_FADE_IN;
            m_VehicleFadeTimer = 0;
            m_pVehicleNameToPrint = m_pLastVehicleName;
        }
        alpha = float(m_VehicleFadeTimer) / 1000.0f * 255.0f;
        break;
    default:
        break;
    }

    if (!m_Message[0]) {
        m_VehicleNameTimer += (int32)CTimer::GetTimeStepInMS();
        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        CFont::SetScaleForCurrentLanguage(SCREEN_STRETCH_X(1.0f), SCREEN_SCALE_Y(1.5f));
        CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        CFont::SetRightJustifyWrap(0.0f);
        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GREEN, (uint8)alpha));
        CFont::SetDropColor({ 0, 0, 0, (uint8)alpha });
        if (CTheScripts::bDisplayHud) {
            CFont::PrintString(
                SCREEN_STRETCH_FROM_RIGHT(32.0f),
                SCREEN_STRETCH_FROM_BOTTOM(104.0f),
                m_pVehicleNameToPrint
            );
        }
        CFont::SetSlant(0.0f);
    }
}

// 0x589650
void CHud::DrawVitalStats() {
    if (StaticRef<uint8>(0xA43088) == 1 || TheCamera.m_bWideScreenOn) return;
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    auto* player = FindPlayerPed(-1);
    auto weapon = player->GetActiveWeapon().m_Type;
    if (int(weapon) == 32) weapon = static_cast<eWeaponType>(28);
    const float sx = float(RsGlobal.maximumWidth) * f(0x859520), sy = float(RsGlobal.maximumHeight) * f(0x859524);
    CFont::SetBackground(false, false); CFont::SetColor(CRGBA(225, 225, 225, 255)); CFont::SetWrapx(sx * f(0x85951C)); CFont::SetRightJustifyWrap(0.f); CFont::SetProportional(true);
    const int16 labelX = int16(int32(sx * f(0x85862C) + f(0x858A10)));
    const int16 barX = int16(int32(sx * f(0x85991C) + f(0x858A10)));
    const int16 barOffset = int16(int32(sy * f(0x858C80)));
    const bool hasSkill = player->GetIntelligence()->GetTaskSwim() || (int(weapon) >= 22 && int(weapon) <= 32);
    const float top = float(RsGlobal.maximumHeight) - sy * f(0x865374);
    const float panelTop = hasSkill ? top : top + sy * f(0x858B48);
    FrontEndMenuManager.DrawWindow(CRect(40.f, panelTop, sx * f(0x858F98) + 40.f, top + sy * f(0x86532C)), "FEH_STA", 0, CRGBA(0, 0, 0, 190), false, true);
    int16 labelY = int16(int32(top + (hasSkill ? sy * f(0x858B48) : (sy * f(0x858B48)) + (sy * f(0x858B48)))));
    CFont::SetFontStyle(static_cast<eFontStyle>(1)); CFont::SetOrientation(static_cast<eFontAlignment>(1)); CFont::SetScale(sx * f(0x858F9C), sy * f(0x858C20)); CFont::SetColor(CRGBA(225, 225, 225, 255)); CFont::SetEdge(0);
    const auto bar = [&](float value) {
        CSprite2d::DrawBarChart(float(barX), float(int(labelY) + barOffset), uint16(int32(sx * f(0x858CE0))), uint8(int32(sy * f(0x85862C))), value, 0, 0, 1, CRGBA(200, 200, 200, 255), CRGBA(0, 0, 0, 0));
    };
    CFont::PrintString(float(labelX), float(labelY), TheText.Get("STAT068"));
    bar((CStats::GetStatValue(static_cast<eStats>(64)) * f(0x858CDC)) * f(0x858628));
    labelY = int16(int32(float(labelY) + sy * f(0x858B48)));
    bool skillShown=false;
    if (player->GetIntelligence()->GetTaskSwim()) {
        skillShown=true;
        CFont::PrintString(float(labelX), float(labelY), TheText.Get("STAT225"));
        bar((CStats::GetStatValue(static_cast<eStats>(225)) * f(0x858CDC)) * f(0x858628));
    } else if (int(weapon) >= 22 && int(weapon) <= 32) {
        skillShown=true;
        CFont::PrintString(float(labelX), float(labelY), TheText.Get("CURWSKL"));
        const auto stat = CWeaponInfo::GetSkillStatIndex(weapon);
        const float reaction = *reinterpret_cast<const float*>(0xB78F40 + (int(stat) - 69) * 4);
        const float product = *reinterpret_cast<const float*>(0xB78EB4 + int(stat) * 4) * reaction;
        const float value = CStats::GetStatValue(stat);
        float progress = 1.f;
        if (value <= f(0x866BEC)) {
            const float rounded = reinterpret_cast<float(__cdecl*)(float)>(0x406CE0)((reaction * f(0x858B1C) + value) / product);
            progress = (rounded * product) * f(0x858CDC);
        }
        bar(progress * f(0x858628));
    }
    if (skillShown) labelY = int16(int32(float(labelY) + sy * f(0x858B48)));
    constexpr int stats[]{22, 23, 21, 25}; constexpr const char* keys[]{"STAT022", "STAT023", "STAT021", "STAT025"};
    for (int i = 0; i < 4; ++i) {
        CFont::PrintString(float(labelX), float(labelY), TheText.Get(keys[i]));
        bar((CStats::GetStatValue(static_cast<eStats>(stats[i])) * f(0x858CDC)) * f(0x858628));
        labelY = int16(int32(float(labelY) + sy * f(0x858B48) + (i==3?sy*f(0x858B3C):0.f)));
    }
    CFont::SetFontStyle(static_cast<eFontStyle>(3)); CFont::SetOrientation(static_cast<eFontAlignment>(2)); CFont::SetScale(sx * f(0x858CB0), sy * f(0x858CB0)); CFont::SetEdge(1); CFont::SetColor(CRGBA(200, 200, 200, 255)); CFont::SetDropColor(CRGBA(0, 0, 0, 255));
    std::sprintf(gString, "DAY_%d", int(StaticRef<uint8>(0xB7014E)));
    CFont::PrintString(sx * f(0x858970) + 40.f, float(labelY), TheText.Get(gString));
}

// 0x588A50
void CHud::GetRidOfAllHudMessages(bool arg0) {
    std::ranges::fill(m_pHelpMessageToPrint, '\0');
    std::ranges::fill(m_pLastHelpMessage, '\0');
    std::ranges::fill(m_pHelpMessage, '\0');
    std::ranges::fill(m_Message, '\0');

    m_ZoneNameTimer               = 0;
    m_pZoneName                   = nullptr;
    m_ZoneState                   = NAME_DONT_SHOW;
    m_nHelpMessageTimer           = 0;
    m_nHelpMessageFadeTimer       = 0;
    m_nHelpMessageState           = 0;
    m_bHelpMessageQuick           = false;
    m_nHelpMessageMaxStatValue    = 1000;
    m_nHelpMessageStatId          = 0;
    m_fHelpMessageStatUpdateValue = 0.0f;
    m_bHelpMessagePermanent       = false;
    m_fHelpMessageTime            = 1.0f;
    m_pVehicleName                = nullptr;
    m_pVehicleNameToPrint         = nullptr;
    m_VehicleNameTimer            = 0;
    m_VehicleFadeTimer            = 0;
    m_VehicleState                = NAME_DONT_SHOW;

    for (auto i = 0; i < NUM_MESSAGE_STYLES; ++i) {
        if (BigMessageX[i] != 0.0f)
            continue;

        if (arg0) {
            if (BigMessageX[i] == BigMessageX[STYLE_BOTTOM_RIGHT] ||
                BigMessageX[i] == BigMessageX[STYLE_MIDDLE_SMALLER_HIGHER]
            ) {
                continue;
            }
        }
        std::ranges::fill(m_BigMessage[i], '\0');
    }
}

// 0x5893B0
void CHud::DrawAmmo(CPed* ped, int32 x, int32 y, float alpha) {
    const auto MAX_CLIP = 9999;

    const auto& weapon = ped->GetActiveWeapon();
    const auto& totalAmmo = weapon.m_TotalAmmo;
    const auto& ammoInClip = weapon.m_AmmoInClip;
    const auto& ammoClip = CWeaponInfo::GetWeaponInfo(weapon.m_Type, ped->GetWeaponSkill())->m_nAmmoClip;

    if (ammoClip <= 1 || ammoClip >= 1000) {
        sprintf_s(gString, "%d", totalAmmo);
    } else {
        uint32 total, current;

        if (weapon.m_Type == WEAPON_FLAMETHROWER ) {
            uint32 out = MAX_CLIP;
            if ((totalAmmo - ammoInClip) / 10 <= MAX_CLIP) {
                out = (totalAmmo - ammoInClip) / 10u;
            }
            total = out;

            current = ammoInClip / 10;
        } else {
            auto out = totalAmmo - ammoInClip;
            if (totalAmmo - ammoInClip > MAX_CLIP) {
                out = MAX_CLIP;
            }
            total = out;

            current = ammoInClip;
        }
        sprintf_s(gString, "%d-%d", total, current);
    }
    AsciiToGxtChar(gString, gGxtString);

    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.3f), SCREEN_STRETCH_Y(0.7f));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetCentreSize(SCREEN_STRETCH_Y(640.0f));
    CFont::SetProportional(true);
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);

    if (   totalAmmo - weapon.m_AmmoInClip >= MAX_CLIP
        || CDarkel::FrenzyOnGoing()
        || weapon.m_Type == WEAPON_UNARMED
        || weapon.m_Type == WEAPON_DETONATOR
        || weapon.m_Type == WEAPON_DILDO1
        || weapon.m_Type == WEAPON_DILDO2
        || weapon.m_Type == WEAPON_VIBE1
        || weapon.m_Type == WEAPON_VIBE2
        || weapon.m_Type == WEAPON_FLOWERS
        || weapon.m_Type == WEAPON_CANE
        || weapon.m_Type == WEAPON_PARACHUTE
        || CWeaponInfo::GetWeaponInfo(weapon.m_Type)->m_nWeaponFire == WEAPON_FIRE_USE
        || CWeaponInfo::GetWeaponInfo(weapon.m_Type)->m_nSlot <= 1
    ) {
        CFont::SetEdge(0);
        return;
    }

    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_BLUE, (uint8)alpha));
    CFont::PrintString((float)x, (float)y, gGxtString);
    CFont::SetEdge(0);
}

// 0x58EAF0
void CHud::DrawPlayerInfo() {
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    const float sx = float(RsGlobal.maximumWidth) * f(0x859520), sy = float(RsGlobal.maximumHeight) * f(0x859524);
    const auto focus = CWorld::PlayerInFocus;
    auto* player = CWorld::Players[focus].m_pPed;
    auto* other = StaticRef<CPlayerPed*>(0xB7CF28);
    GxtChar text[200]; char ascii[200];
    const auto typography = [&] {
        CFont::SetBackground(false, false); CFont::SetScale(sx * f(0x866CAC), sy * f(0x858F14)); CFont::SetProportional(false); CFont::SetFontStyle(static_cast<eFontStyle>(3)); CFont::SetOrientation(static_cast<eFontAlignment>(2)); CFont::SetRightJustifyWrap(0.f); CFont::SetEdge(2);
    };
    if (StaticRef<uint32>(0xBAA400) == 1) {
        typography(); CFont::SetDropColor(CRGBA(0, 0, 0, 255));
        std::sprintf(ascii, "%02d:%02d", int(StaticRef<uint8>(0xB70153)), int(StaticRef<uint8>(0xB70152))); AsciiToGxtChar(ascii, text);
        CFont::SetColor(HudColour.GetRGB(static_cast<eHudColours>(4))); CFont::PrintString(float(RsGlobal.maximumWidth) - sx * f(0x85950C), sy * f(0x858F8C), text); CFont::SetEdge(0);
    }
    const auto fadeElement = [&](uintptr_t address, int current) {
        auto& state = StaticRef<int32>(address); auto& fade = StaticRef<int32>(address + 4); auto& timer = StaticRef<int32>(address + 8); auto& previous = StaticRef<int32>(address + 12);
        float alpha = 255.f;
        if (previous != current) {
            if (!state) fade = 0;
            if (!state || state == 1 || state == 3) { state = 2; timer = 5; }
        }
        if (state && state != 5) {
            if (state == 1) { fade = 1000; if (float(timer) > f(0x859AA4)) { state = 3; fade = 3000; } }
            else if (state == 2 || state == 3) {
                fade += int32((CTimer::GetTimeStep() * f(0x858B38)) * (state == 2 ? f(0x858C4C) : f(0x859948)));
                if (state == 2 && float(fade) > 1000.f) { fade = 1000; state = 1; }
                if (state == 3 && float(fade) < 0.f) { fade = 0; state = 0; }
                alpha = (float(fade) * f(0x858CDC)) * 255.f;
            }
            timer += int32((CTimer::GetTimeStep() * f(0x858B38)) * f(0x858C4C));
        }
        previous = current;
        if (alpha < 0.f) alpha = 0.f; else if (alpha > 255.f) alpha = 255.f;
        return alpha;
    };
    fadeElement(0xBAA434, StaticRef<int32>(0xB7CE7C + 400 * focus));
    if (m_EnergyLostState) {
        const int healthX = int32(float(RsGlobal.maximumWidth) - sx * f(0x86535C)), armorX = int32(float(RsGlobal.maximumWidth) - sx * f(0x866B78));
        RenderHealthBar(focus, healthX, int32(GetYPosBasedOnHealth(focus, sy * f(0x866CA8), 10)));
        if (other) RenderHealthBar(1, healthX, int32(GetYPosBasedOnHealth(1, GetYPosBasedOnHealth(focus, sy * f(0x866CA4), 12), 12)));
        RenderArmorBar(focus, armorX, int32(GetYPosBasedOnHealth(focus, sy * f(0x862D38), 3)));
        if (other) RenderArmorBar(1, armorX, int32(GetYPosBasedOnHealth(1, GetYPosBasedOnHealth(focus, sy * f(0x866CA0), 12), 12)));
        const auto submerged = [](CPlayerPed* p) {
            if (p->GetIntelligence()->GetTaskSwim()) return true;
            const auto* raw = reinterpret_cast<const uint8*>(p);
            if ((*reinterpret_cast<const uint32*>(raw + 0x46C) & 0x100) && p->m_pVehicle) {
                const auto* vehicle = reinterpret_cast<const uint8*>(p->m_pVehicle);
                if ((*reinterpret_cast<const uint32*>(vehicle + 0x40) & 0x100) && (vehicle[0x42B] & 0x40)) return true;
            }
            return false;
        };
        bool breath = submerged(player);
        if (!breath) breath = player->GetPlayerData()->m_fBreath < CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) && CTimer::GetTimeInMS() < uint32(m_LastBreathTime + 500);
        if (breath) m_LastBreathTime = int32(CTimer::GetTimeInMS());
        bool otherBreath = false;
        if (other) { otherBreath = submerged(other); if (!otherBreath) otherBreath = other->GetPlayerData()->m_fBreath < CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG); if (otherBreath) m_LastBreathTime = int32(CTimer::GetTimeInMS()); }
        if (breath) RenderBreathBar(focus, armorX, int32(GetYPosBasedOnHealth(focus, sy * f(0x86503C), 6)));
        if (otherBreath && other) RenderBreathBar(1, armorX, int32(GetYPosBasedOnHealth(1, GetYPosBasedOnHealth(focus, sy * f(0x866C9C), 12), 12)));
    }
    const int money = StaticRef<int32>(0xB7CE54 + 400 * focus);
    const auto moneyAlpha = uint8(int32(fadeElement(0xBAA424, money)));
    if (m_DisplayScoreState) {
        CFont::SetColor(HudColour.GetRGBA(static_cast<eHudColours>(money < 0 ? 0 : 1), moneyAlpha));
        std::sprintf(ascii, money < 0 ? "-$%07d" : "$%08d", money < 0 ? -money : money); AsciiToGxtChar(ascii, text);
        CFont::SetProportional(false); CFont::SetBackground(false, false); CFont::SetScale(sx * f(0x866CAC), sy * f(0x858F14)); CFont::SetOrientation(static_cast<eFontAlignment>(2)); CFont::SetRightJustifyWrap(0.f); CFont::SetFontStyle(static_cast<eFontStyle>(3)); CFont::SetDropShadowPosition(0); CFont::SetEdge(2); CFont::SetDropColor(CRGBA(0,0,0,moneyAlpha));
        CFont::PrintString(float(RsGlobal.maximumWidth) - sx * f(0x85950C), GetYPosBasedOnHealth(focus, sy * f(0x866C88), 12), text); CFont::SetEdge(0);
    }
    const float weaponAlpha = fadeElement(0xBAA404, int(player->GetActiveWeapon().m_Type));
    if (m_WeaponState) {
        const float iconX = float(RsGlobal.maximumWidth) - (sx * f(0x85950C) + float(RsGlobal.maximumWidth) * f(0x866C84));
        DrawWeaponIcon(player, int32(iconX), int32(sy * f(0x858BA4)), weaponAlpha);
        if (other) DrawWeaponIcon(other, int32(float(RsGlobal.maximumWidth) - (sx * f(0x85950C) + f(0x866C7C))), int32(GetYPosBasedOnHealth(focus, sy * f(0x866C80), 12)), weaponAlpha);
        const int ammoX = int32(iconX + (sx * f(0x866C4C)) * .5f);
        DrawAmmo(player, ammoX, int32(sy * f(0x858BA4) + sy * f(0x866C78)), weaponAlpha);
        if (other) DrawAmmo(other, ammoX, int32(GetYPosBasedOnHealth(focus, sy * f(0x866C80) + sy * f(0x866C78), 12)), weaponAlpha);
    }
}


inline void CHud::DrawClock() {
    char ascii[16];
    GxtChar gxtText[16];
    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.55f), SCREEN_STRETCH_Y(1.1f));
    CFont::SetProportional(false);
    CFont::SetFontStyle(FONT_PRICEDOWN);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetEdge(2);
    CFont::SetDropColor({0, 0, 0, 255});
    sprintf_s(ascii, "%02d:%02d", CClock::ms_nGameClockHours, CClock::ms_nGameClockMinutes);
    AsciiToGxtChar(ascii, gxtText);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
    CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(32.0f), SCREEN_STRETCH_Y(22.0f), gxtText);
    CFont::SetEdge(0);
}

inline void CHud::DrawMoney(const CPlayerInfo& playerInfo, uint8 alpha) {
    char ascii[16];
    GxtChar gxtText[16];

    if (playerInfo.m_nDisplayMoney < 0) {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_RED, alpha));
        auto m_nDisplayMoney = playerInfo.m_nDisplayMoney;
        if (m_nDisplayMoney < 0) {
            m_nDisplayMoney = -m_nDisplayMoney;
        }
        sprintf_s(ascii, "-$%07d", m_nDisplayMoney);
    } else {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GREEN, alpha));
        sprintf_s(ascii, "$%08d", std::abs(playerInfo.m_nDisplayMoney));
    }
    AsciiToGxtChar(ascii, gxtText);
    CFont::SetProportional(false);
    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.55f), SCREEN_STRETCH_Y(1.1f));
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap(0.0f);
    CFont::SetFontStyle(FONT_PRICEDOWN);
    CFont::SetDropShadowPosition(0);
    CFont::SetEdge(2);
    CFont::SetDropColor({ 0, 0, 0, uint8(alpha) });
    CFont::PrintString(SCREEN_STRETCH_FROM_RIGHT(32.0f), GetYPosBasedOnHealth(CWorld::PlayerInFocus, SCREEN_STRETCH_Y(89.0f), 12), gxtText);
    CFont::SetEdge(0);
}

inline void CHud::DrawWeapon(CPlayerPed* ped0, CPlayerPed* ped1) {
    const auto magic = SCREEN_WIDTH * 0.17343046f; // todo: magic
    if (m_WeaponState) {
        DrawWeaponIcon(ped0, (int32)(SCREEN_WIDTH - (SCREEN_STRETCH_X(32.0f) + magic)), (int32)SCREEN_STRETCH_Y(20.0f), (float)m_WeaponFadeTimer);
        if (ped1) {
            const auto posX = (int32)(SCREEN_WIDTH - (SCREEN_STRETCH_X(32.0f) + 111.0f));
            const auto posY = (int32)GetYPosBasedOnHealth(CWorld::PlayerInFocus, SCREEN_STRETCH_Y(138.0f), 12);
            DrawWeaponIcon(ped1, posX, posY, (float)m_WeaponFadeTimer);
        }

        const auto ammoPosX = (int32)(SCREEN_WIDTH - (magic + SCREEN_STRETCH_X(32.0f)) + SCREEN_STRETCH_X(47.0f / 2.0f));
        const auto ammoPosY = SCREEN_STRETCH_Y(43.0f);
        DrawAmmo(ped0, ammoPosX, (int32)ammoPosY + (int32)SCREEN_STRETCH_Y(20.0f), (float)m_WeaponFadeTimer);
        if (ped1) {
            const auto posY = (int32)GetYPosBasedOnHealth(CWorld::PlayerInFocus, ammoPosY + SCREEN_STRETCH_Y(138.0f), 12);
            DrawAmmo(ped1, ammoPosX, posY, (float)m_WeaponFadeTimer);
        }
    }
}

// 0x58A160
void CHud::DrawTripSkip() {
    CRect rect{
        SCREEN_STRETCH_X(54.0f),
        SCREEN_STRETCH_FROM_BOTTOM(189.0f),
        SCREEN_STRETCH_X(118.0f),
        SCREEN_STRETCH_FROM_BOTTOM(125.0f)
    };
    Sprites[SPRITE_SKIP_ICON].Draw(rect, CRGBA(255, 255, 255, 255));

    CFont::SetBackground(false, false);
    CFont::SetScale(SCREEN_STRETCH_X(0.3f), SCREEN_SCALE_Y(0.7f));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetCentreSize(SCREEN_WIDTH);
    CFont::SetProportional(true);
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
    CFont::PrintString(
        SCREEN_STRETCH_X(64.0f) / 2.0f + SCREEN_STRETCH_X(54.0f),
        SCREEN_STRETCH_FROM_BOTTOM(127.0f),
        TheText.Get("FEC_TSK") // TRIP SKIP
    );
}

// 0x58D9A0
void CHud::DrawWanted() {
    const auto f = [](uintptr_t a) { return *reinterpret_cast<const float*>(a); };
    const int wanted = int(FindPlayerWanted(-1)->GetWantedLevel());
    const int parole = int(FindPlayerWanted(-1)->m_WantedLevelBeforeParole);
    int state = int(m_WantedState), fade = int(m_WantedFadeTimer), timer = int(m_WantedTimer);
    float alpha = 255.f;
    const bool changed = m_LastWanted != uint32(wanted);
    if (changed) {
        if (!state) fade = 0;
        if (!state || state == 1 || state == 3) { state = 2; timer = 5; }
        m_LastWanted = uint32(wanted);
    }
    if (state && state != 5) {
        if (state == 1) {
            fade = 1000;
            if (float(timer) > f(0x859AA4)) { state = 3; fade = 3000; }
        } else if (state == 2 || state == 3) {
            const float rate = state == 2 ? f(0x858C4C) : f(0x859948);
            fade += int32((CTimer::GetTimeStep() * f(0x858B38)) * rate);
            if (state == 2 && float(fade) > f(0x858C4C)) { fade = 1000; state = 1; }
            if (state == 3 && float(fade) < 0.f) { fade = 0; state = 0; }
            alpha = (float(fade) * f(0x858CDC)) * 255.f;
        }
        timer += int32((CTimer::GetTimeStep() * f(0x858B38)) * f(0x858C4C));
    }
    m_WantedState = uint32(state); m_WantedFadeTimer = uint32(fade); m_WantedTimer = uint32(timer);
    if (alpha < 0.f) alpha = 0.f; else if (alpha > 255.f) alpha = 255.f;
    StaticRef<bool>(0xBAB228) = !changed;
    if (!state) return;
    const float sx = float(RsGlobal.maximumWidth) * f(0x859520), sy = float(RsGlobal.maximumHeight) * f(0x859524);
    const float width = sx * f(0x866C60), height = sy * f(0x866C64);
    CFont::SetBackground(false, false); CFont::SetScale(width, height);
    CFont::SetOrientation(static_cast<eFontAlignment>(2)); CFont::SetProportional(true); CFont::SetFontStyle(static_cast<eFontStyle>(0));
    GxtChar star[20]; AsciiToGxtChar("]", star);
    float x = float(RsGlobal.maximumWidth) - sx * f(0x863210);
    if (!(wanted > 0 && StaticRef<bool>(0xBAB228)) && parole <= 0) return;
    const uint8 opacity = uint8(int32(alpha));
    for (int i = 0; i < 6; ++i) {
        CFont::SetEdge(1); CFont::SetDropColor(CRGBA(0, 0, 0, opacity)); CFont::SetScale(width, height);
        if (i < wanted && (CTimer::GetTimeInMS() > FindPlayerWanted(-1)->m_LastTimeWantedLevelChanged + 2000u || (CTimer::GetFrameCounter() & 4))) {
            CFont::SetColor(HudColour.GetRGBA(static_cast<eHudColours>(6), opacity));
            float y = sy * f(0x866C5C);
            if (*reinterpret_cast<const uint8*>(0xB7CEE7 + 400 * CWorld::PlayerInFocus) < 101) y -= sy * f(0x858CCC);
            CFont::PrintString(x, y, star);
        } else if (i < parole && (CTimer::GetFrameCounter() & 4)) {
            CFont::SetColor(CRGBA(uint8(int32(float(*reinterpret_cast<const uint8*>(0xBAB244)) * f(0x858C98))), uint8(int32(float(*reinterpret_cast<const uint8*>(0xBAB245)) * f(0x858C98))), uint8(int32(float(*reinterpret_cast<const uint8*>(0xBAB246)) * f(0x858C98))), opacity));
            CFont::PrintString(x, GetYPosBasedOnHealth(CWorld::PlayerInFocus, sy * f(0x866C5C), 12), star);
        } else if (i >= wanted) {
            CFont::SetEdge(0); CFont::SetColor(CRGBA(0, 0, 0, uint8(int32(alpha * f(0x858CB0)))));
            CFont::SetScale(width * f(0x858F08), height * f(0x858F08));
            CFont::PrintString(x, GetYPosBasedOnHealth(CWorld::PlayerInFocus, sy * f(0x866C5C), 12) - (sy + sy), star);
        }
        x -= sx * f(0x859008);
    }
    CFont::SetEdge(0);
}

// 0x58D7D0
void CHud::DrawWeaponIcon(CPed* ped, int32 x, int32 y, float alpha) {
    const auto x0 = (float)x;
    const auto y0 = (float)y;
    const float width  = SCREEN_STRETCH_X(47.0f);
    const float height = SCREEN_STRETCH_Y(58.0f);
    const float halfWidth  = width / 2.0f;
    const float halfHeight = height / 2.0f;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));

    auto modelId = ped->GetActiveWeapon().GetWeaponInfo().m_nModelId1;
    if (modelId <= 0) {
        Sprites[SPRITE_FIST].Draw({ x0, y0, width + x0, height + y0 }, CRGBA(255, 255, 255, (uint8)alpha));
        return;
    }

    auto mi = CModelInfo::GetModelInfo(modelId);
    auto txd = CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex);
    if (!txd)
        return;

    auto texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(mi->m_nKey, "ICON"));
    if (!texture)
        return;

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(texture)));
    CSprite::RenderOneXLUSprite(
        { x0 + halfWidth, y0 + halfHeight, 1.0f },
        { halfWidth, halfHeight },
        255u, 255u, 255u, 255,
        1.0f,
        255,
        0, 0
    );
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(FALSE));
}

// 0x5890A0
void CHud::RenderArmorBar(int32 playerId, int32 x, int32 y) {
    auto* player = FindPlayerPed(playerId);
    if ((m_ItemToFlash == ITEM_ARMOUR && EachFrames(8)) || player->m_fArmour <= 1.0f)
        return;

    const auto info = player->GetPlayerInfoForThisPlayerPed();
    CSprite2d::DrawBarChart(
        (float)x,
        (float)y,
        (uint16)SCREEN_STRETCH_X(62.0f),
        (uint8)SCREEN_STRETCH_Y(9.0f),
        player->m_fArmour / (float)info->m_nMaxArmour * 100.0f,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY),
        CRGBA(0, 0, 0, 0)
    );
}

// 0x589190
void CHud::RenderBreathBar(int32 playerId, int32 x, int32 y) {
    if (m_ItemToFlash == ITEM_BREATH && EachFrames(8))
        return;

    auto* player = FindPlayerPed(playerId);
    CSprite2d::DrawBarChart(
        (float)x,
        (float)y,
        (uint16)SCREEN_STRETCH_X(62.0f),
        (uint8)SCREEN_STRETCH_Y(9.0f),
        player->GetPlayerData()->m_fBreath / CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) * 100.0f,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE),
        CRGBA(0, 0, 0, 0)
    );
}

// 0x589270
void CHud::RenderHealthBar(int32 playerId, int32 x, int32 y) {
    if (m_ItemToFlash == ITEM_HEALTH && EachFrames(8))
        return;

    auto* player = FindPlayerPed(playerId);
    if ((int)player->m_fHealth < 10 && EachFrames(8))
        return;

    const float x109 = SCREEN_STRETCH_X(109.0f);
    const auto info = player->GetPlayerInfoForThisPlayerPed();
    const auto totalWidth = uint16(x109 * (float)info->m_nMaxHealth / CStats::GetFatAndMuscleModifier(STAT_MOD_10));

    CSprite2d::DrawBarChart(
        x109 - (float)totalWidth + (float)x,
        (float)y,
        totalWidth,
        (uint8)SCREEN_STRETCH_Y(9.0f),
        player->m_fHealth * 100.0f / (float)info->m_nMaxHealth,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_RED),
        CRGBA(0, 0, 0, 0)
    );
}
