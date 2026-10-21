/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CHud
 * Reconstrucción: COMPLETA — 39/39 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 39/39 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
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
#include "X87Math.h"

namespace {
// Constantes float leidas del EXE (algunas no son el float mas cercano al literal decimal).
inline float ExeFloat(uintptr_t address) {
    return std::bit_cast<float>(*reinterpret_cast<const uint32*>(address));
}
// fild maximumWidth/Height * 0x859520/0x859524 (x87 a 24 bits); el EXE multiplica despues por la constante de cada elemento.
inline float StretchBaseX() { return (float)RsGlobal.maximumWidth * ExeFloat(0x859520); }
inline float StretchBaseY() { return (float)RsGlobal.maximumHeight * ExeFloat(0x859524); }

// 0x406CE0: floorf del CRT del EXE (cdecl, un float en pila). `fld dword [esp+4]; sub esp,8; fstp qword [esp]` lo amplia a double (exacto, NaN silenciados) y llama al floor
// del CRT (0x8219F0, sin reconstruir, cdecl con un double) cuyo resultado queda en ST0 y se devuelve sin tocar (double: ni se redondea a float ni se pasa por SSE). Los
// llamadores del EXE (0x4D8793, 0x570689.., 0x589BF5) solo leen ST0: la funcion no deja nada mas en EAX/ECX/EDX que dependa de ella.
double FloorFloat(float value) {
    return reinterpret_cast<double(__cdecl*)(double)>(0x8219F0)(static_cast<double>(value));
}
}

void CHud::InjectHooks() {
    RH_ScopedClass(CHud);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BA850);
    RH_ScopedInstall(ReInitialise, 0x588880);
    RH_ScopedInstall(Shutdown, 0x588850);
    RH_ScopedInstall(Draw, 0x58FAE0);
    RH_ScopedGlobalInstall(FloorFloat, 0x406CE0);
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
    // Como el original (0x588A3B): 'mov dword ptr [0BAA400h],1' escribe un dword, no un byte.
    *reinterpret_cast<uint32*>(&bDrawClock) = 1;
    m_LastBreathTime                = 0;
}

// 0x588850
void CHud::Shutdown() {
    std::ranges::for_each(Sprites, [](auto& sprite) { sprite.Delete(); });
    CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("hud"));
}

// 0x588B60
float CHud::GetYPosBasedOnHealth(uint8 playerId, float pos, int8 offset) {
    // Como el original (0x588B7A..0x588BA2): fcomp 101.0 (0x866BB4) y despues
    // fild maximumHeight (0xC17048) * 0x859524 * offset (movsx) y fsubr pos, todo a 24 bits.
    if (!((float)FindPlayerInfo(playerId).m_nMaxHealth < 101.0f)) {
        return pos;
    }
    const float scale = (float)RsGlobal.maximumHeight * std::bit_cast<float>(*reinterpret_cast<const uint32*>(0x859524));
    return pos - scale * (float)(int32)offset;
}

// 0x588B50
bool CHud::HelpMessageDisplayed() {
    return m_nHelpMessageState != 0;
}

// 0x588F60
void CHud::SetMessage(const GxtChar* message) {
    auto* const dst = reinterpret_cast<GxtChar*>(m_Message);
    if (!message) {
        dst[0] = '\0';
        return;
    }
    // Como el original: copia byte a byte hasta el NUL o hasta 0x190 caracteres; en este ultimo caso el
    // terminador se escribe en m_Message[0x190] (0x588F8B), es decir, en el byte bajo de m_ZoneToPrint (0xBAB1D0).
    uint16 i = 0;
    for (;;) {
        const GxtChar c = message[i];
        if (!c) {
            dst[i] = '\0';
            return;
        }
        dst[i] = c;
        if (++i >= 0x190) {
            break;
        }
    }
    dst[i] = '\0';
}

// 0x588FC0
void CHud::SetBigMessage(GxtChar* message, eMessageStyle style) {
    // Como el original (0x588FC9..0x588FDB): fcomp 0.0 + test ah,44h + jp: solo continua si BigMessageX[style] == 0 (NaN no).
    if (BigMessageX[(uint16)style] != 0.0f) {
        return;
    }

    auto* const big  = reinterpret_cast<GxtChar*>(m_BigMessage) + size_t(style) * BIG_MESSAGE_SIZE;
    auto* const last = reinterpret_cast<GxtChar*>(LastBigMessage) + size_t(style) * BIG_MESSAGE_SIZE;
    uint16 i = 0;

    if ((uint16)style == STYLE_WHITE_MIDDLE_SMALLER) { // cmp cx,5 (0x588FE6)
        // Como el original: se compara y copia caracter a caracter hasta el NUL (o 0x80); si un caracter difiere
        // de LastBigMessage[5] se reinician OddJob2OffTimer y OddJob2On. El mensaje de entrada no se modifica.
        do {
            const GxtChar c = message[i];
            if (!c) {
                break;
            }
            if (c != last[i]) {
                OddJob2OffTimer = 0.0f;
                OddJob2On       = 0;
            }
            big[i]  = message[i];
            last[i] = message[i];
        } while (++i < 0x80);
    } else {
        // Como el original (0x589034..0x589052): copia a m_BigMessage[style] y pone message[0] = 0.
        do {
            const GxtChar c = message[i];
            if (!c) {
                break;
            }
            big[i] = c;
        } while (++i < 0x80);
        message[0] = '\0';
    }
    // 0x58905E/0x589064: ambos buffers reciben el NUL en la posicion final (puede ser 0x80, la fila siguiente).
    big[i]  = '\0';
    last[i] = '\0';
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
    m_nHelpMessageMaxStatValue = (uint16)(int64)max; // _ftol2 (0x588DF5) y se guarda la palabra baja
    // Como el original (0x588E15..0x588E19): escribe una palabra '+'/'-' + NUL en gString (sin el relleno 0xFE de sprintf_s).
    *reinterpret_cast<uint16*>(gString) = state ? '+' : '-';
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

    // Como el original (0x58FCA6): 'cmp byte ptr [0BAA45Ch],1' compara el byte con 1 (no es un test de "distinto de cero").
    if (*reinterpret_cast<const uint8*>(&m_bDraw3dMarkers) == 1 && !TheCamera.m_bWideScreenOn) {
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

    // Como el original: ms = _ftol2(ms_fTimeStep * 0x858B38 * 1000.0 | -1000.0) y alfa = fade * 0x858CDC (0.001) * 255.0.
    const auto stepMs    = [] { return (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x858C4C)); };
    const auto stepMsNeg = [] { return (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x859948)); };
    const auto fadeAlpha = [] { return (float)(int32)m_ZoneFadeTimer * ExeFloat(0x858CDC) * ExeFloat(0x859AAC); };

    float alpha = 255.0f;
    switch (m_ZoneState) {
    case NAME_SHOW:
        m_ZoneFadeTimer = 1000;
        if ((float)(int32)m_ZoneNameTimer > ExeFloat(0x859A94)) { // fild + fcomp 3000.0 (0x58AC7A..0x58AC9E)
            m_ZoneState = NAME_FADE_OUT;
            m_ZoneFadeTimer = 1000;
        }
        break;

    case NAME_FADE_IN:
        if (!TheCamera.GetFading() && TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            m_ZoneFadeTimer += stepMs();
        }

        if (!((float)m_ZoneFadeTimer <= ExeFloat(0x858C4C))) {
            m_ZoneFadeTimer = 1000;
            m_ZoneState = NAME_SHOW;
        }

        if (TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            alpha = fadeAlpha();
            break;
        }
        m_ZoneState = NAME_FADE_OUT;
        m_ZoneFadeTimer = 1000;
        break;

    case NAME_FADE_OUT:
        if (!TheCamera.GetFading() && TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            m_ZoneFadeTimer += stepMsNeg();
        }

        if ((float)m_ZoneFadeTimer < 0.0f) {
            m_ZoneFadeTimer = 0;
            m_ZoneState = NAME_DONT_SHOW;
        }

        if (TheCamera.GetScreenFadeStatus() != NAME_FADE_IN) {
            alpha = fadeAlpha();
            break;
        }
        m_ZoneFadeTimer = 1000;
        break;

    case NAME_SWITCH:
        m_ZoneFadeTimer += stepMsNeg();
        if ((float)m_ZoneFadeTimer < 0.0f) {
            m_ZoneFadeTimer = 0;
            m_ZoneState = NAME_FADE_IN;
            m_ZoneToPrint = m_pLastZoneName;
        }
        alpha = fadeAlpha();
        break;

    default:
        break;
    }

    if (m_Message[0] || BigMessageX[STYLE_BOTTOM_RIGHT] != 0.0f || BigMessageX[STYLE_WHITE_MIDDLE] != 0.0f) {
        m_ZoneState = NAME_FADE_OUT;
        return;
    }

    m_ZoneNameTimer += stepMs();
    CFont::SetProportional(true);
    CFont::SetBackground(false, false);
    CFont::SetScaleForCurrentLanguage(StretchBaseX() * ExeFloat(0x858F08),
                                      StretchBaseY() * ExeFloat(0x866C0C));
    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetRightJustifyWrap((float)RsGlobal.maximumWidth * ExeFloat(0x859520) * ExeFloat(0x85A994));
    const uint8 alphaByte = (uint8)(int64)alpha; // _ftol2 + movzx bl,al (0x58AD80..0x58AD87)
    CFont::SetDropColor({ 0, 0, 0, alphaByte });
    CFont::SetFontStyle(FONT_GOTHIC);

    const CZoneInfo* info = CPopCycle::m_pCurrZoneInfo;
    if (CGangWars::bGangWarsActive && info && info->ZoneColor.r && info->ZoneColor.g && info->ZoneColor.b) {
        CFont::SetColor({ info->ZoneColor.r, info->ZoneColor.g, info->ZoneColor.b, alphaByte });
    } else {
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_BLUE, alphaByte));
    }

    // Como el original (0x58AE07..0x58AE5A): y = (H - sy*104) + sy*76 con sy = c524*H; x = W - (W*c520)*32.
    const float sy = ExeFloat(0x859524) * (float)RsGlobal.maximumHeight;
    const float y = ((float)RsGlobal.maximumHeight - ExeFloat(0x866B70) * sy) + sy * ExeFloat(0x866B74);
    const float x = (float)RsGlobal.maximumWidth - (float)RsGlobal.maximumWidth * ExeFloat(0x859520) * ExeFloat(0x85950C);
    CFont::PrintStringFromBottom(x, y, m_ZoneToPrint);
    CFont::SetSlant(0.0f);
}

// 0x58CA50
void CHud::DrawBustedWastedMessage() {
    auto& message      = m_BigMessage[STYLE_WHITE_MIDDLE];
    auto& messageX     = BigMessageX[STYLE_WHITE_MIDDLE];
    auto& messageAlpha = BigMessageAlpha[STYLE_WHITE_MIDDLE];

    // Como el original: 'posY' es una variable estatica local del EXE (0xBAB220) con su bandera de inicializacion
    // (bit 0 de 0xBAB224, "test byte ptr [0BAB224h],1" en 0x58CA5E).
    static auto& posY  = StaticRef<float>(0xBAB220);
    static auto& guard = StaticRef<uint32>(0xBAB224);

    if (!message[0]) {
        messageX = 0.0f;
        return;
    }

    // 0x58CA74..0x58CA97: (float)(maximumHeight / 2) - (fild maximumHeight * 0x859524) * 30.0
    const auto computePosY = [] {
        return (float)(RsGlobal.maximumHeight / 2) - StretchBaseY() * ExeFloat(0x858CA4);
    };

    if (!(guard & 1)) {
        guard |= 1;
        posY = computePosY();
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
        posY = computePosY(); // 0x58CB1A / 0x58CB42
        return;
    }

    // 0x58CB4A..0x58CB92: ms = _ftol2(ts * 0x858B38 * 1000.0), tratado como dword sin signo (fadd 2^32 si es negativo)
    const int32 ms = (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x858C4C));
    float msFloat = (float)ms;
    if (ms < 0) {
        msFloat += ExeFloat(0x858C54);
    }
    messageAlpha = msFloat * ExeFloat(0x858EE8) + messageAlpha;
    if (messageAlpha > ExeFloat(0x859AAC)) {
        messageAlpha = ExeFloat(0x859AAC);
    }

    CFont::SetBackground(false, false);
    CFont::SetScale(StretchBaseX() * ExeFloat(0x859974), StretchBaseY() * ExeFloat(0x859974));
    CFont::SetProportional(true);
    CFont::SetJustify(false);
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetFontStyle(FONT_GOTHIC);
    CFont::SetEdge(3);
    const uint8 alphaByte = (uint8)(int64)messageAlpha; // _ftol2 (0x58CC01)
    CFont::SetDropColor({ 0, 0, 0, alphaByte });
    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_GRAY, alphaByte));
    CFont::PrintStringFromBottom((float)(RsGlobal.maximumWidth / 2), posY, message);
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
    // Como el original: todos los temporizadores se tratan con signo (fild + fcomp) y se guardan como dword.
    int32 state, timer, fadeTimer;
    switch (fadingElement) {
    case WANTED_STATE:
        fadeTimer = (int32)m_WantedFadeTimer;
        state = (int32)m_WantedState;
        timer = (int32)m_WantedTimer;
        break;
    case ENERGY_LOST_STATE:
        fadeTimer = (int32)m_EnergyLostFadeTimer;
        state = (int32)m_EnergyLostState;
        timer = (int32)m_EnergyLostTimer;
        break;
    case DISPLAY_SCORE_STATE:
        fadeTimer = (int32)m_DisplayScoreFadeTimer;
        state = (int32)m_DisplayScoreState;
        timer = (int32)m_DisplayScoreTimer;
        break;
    case WEAPON_STATE:
        fadeTimer = (int32)m_WeaponFadeTimer;
        state = (int32)m_WeaponState;
        timer = (int32)m_WeaponTimer;
        break;
    default:
        state = timer = fadeTimer = (int32)fadingElement;
        break;
    }

    // Como el original (0x58D60A..0x58D61B): con forceFadingIn, estado 0 => fadeTimer = 0; estados 0, 1 y 3 => timer = 5, estado = 2.
    if (forceFadingIn) {
        switch (state) {
        case 0:
            fadeTimer = 0;
            [[fallthrough]];
        case 1:
        case 3:
            timer = 5;
            state = 2;
            break;
        default:
            break;
        }
    }

    float alpha = 255.0f;
    // Como el original (0x58D61D..0x58D628): los estados 0 y 5 saltan directamente a guardar (no suman al temporizador).
    if (state != 0 && state != 5) {
        // 0x58D701: _ftol2(ms_fTimeStep * 0x858B38 * 1000.0)
        const int32 ms = (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x858C4C));
        switch (state) {
        case 1: // 0x58D6D9
            fadeTimer = 1000;
            if ((float)timer > ExeFloat(0x859AA4)) { // 10000.0
                state = 3;
                fadeTimer = 3000;
            }
            break;
        case 2: // 0x58D69D
            fadeTimer += ms;
            if (!((float)fadeTimer <= ExeFloat(0x858C4C))) {
                fadeTimer = 1000;
                state = 1;
            }
            alpha = (float)fadeTimer * ExeFloat(0x858CDC) * ExeFloat(0x859AAC);
            break;
        case 3: // 0x58D641: se suma _ftol2(ms_fTimeStep * 0x858B38 * -1000.0)
            fadeTimer += (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x859948));
            if ((float)fadeTimer < 0.0f) {
                fadeTimer = 0;
                state = 0;
            }
            alpha = (float)fadeTimer * ExeFloat(0x858CDC) * ExeFloat(0x859AAC);
            break;
        default:
            break;
        }
        timer += ms;
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

    // Como el original (0x58D774..0x58D7A3): fcom 0.0 / fcom 255.0 sin std::clamp (un NaN pasa tal cual).
    if (alpha < 0.0f) {
        return 0.0f;
    }
    if (alpha > ExeFloat(0x859AAC)) {
        return ExeFloat(0x859AAC);
    }
    return alpha;
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
    CFont::SetScale(StretchBaseX(), StretchBaseY() * ExeFloat(0x859918));

    // Como el original (0x58D30F..0x58D348): ms = _ftol2(ms_fTimeStep * 0x858B38 * 1000.0) como dword sin signo (fadd 2^32 si es negativo).
    const auto stepMsFloat = [] {
        const int32 ms = (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x858C4C));
        float msFloat = (float)ms;
        if (ms < 0) {
            msFloat += ExeFloat(0x858C54);
        }
        return msFloat;
    };

    // 0x58D2E9..0x58D30D: fild (maximumWidth - 20) comparado con messageInUse, jne si (W - 20) <= messageInUse (o NaN)
    if (!((float)(RsGlobal.maximumWidth - 20) > messageInUse)) {
        messageX = CTimer::GetTimeStep() + messageX;
        if (messageX >= ExeFloat(0x858BB0)) { // fcomp 120.0 + test ah,1 + jne (un NaN no entra)
            messageX = 120.0f;
            messageAlpha = messageAlpha - stepMsFloat();
        }
        if (!(messageAlpha > 0.0f)) { // fcomp 0.0 + test ah,41h + jp (0x58D3B1..0x58D3BC)
            messageAlpha = 0.0f;
            message[0] = '\0';
            messageX = 0.0f;
        }
    } else {
        messageAlpha = 255.0f;
        messageInUse = stepMsFloat() * ExeFloat(0x858C24) + messageInUse;
    }

    CFont::SetEdge(2);
    const uint8 alphaByte = (uint8)(int64)messageAlpha; // _ftol2 (0x58D3E9, 0x58D40F)
    CFont::SetDropColor({ 0, 0, 0, alphaByte });
    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GOLD, alphaByte));
    // 0x58D42E..0x58D470: x = W - (W * c520) * 20.0, y = H - (H * c524) * 115.0
    CFont::PrintStringFromBottom(
        (float)RsGlobal.maximumWidth - StretchBaseX() * ExeFloat(0x858BA4),
        (float)RsGlobal.maximumHeight - StretchBaseY() * ExeFloat(0x863194),
        message
    );
    CFont::SetEdge(0);
}

// 0x58CC80
void CHud::DrawOddJobMessage(bool displayImmediately) {
    // Como el original: cada coordenada es (fild maximumWidth/Height * 0x859520/0x859524) * valor; las posiciones X de
    // texto son float(maximumWidth / 2) con division entera con signo (cdq/sub/sar 0x58CD6E..0x58CD88).
    const auto halfWidth = [] { return (float)(RsGlobal.maximumWidth / 2); };

    const auto& m1 = m_BigMessage[STYLE_BOTTOM_RIGHT];
    const auto& m4 = m_BigMessage[STYLE_MIDDLE_SMALLER_HIGHER];
    if (displayImmediately == CTheScripts::bDrawOddJobTitleBeforeFade && !m1[0] && m4[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(StretchBaseX() * ExeFloat(0x858CC8), StretchBaseY() * ExeFloat(0x866C48));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(StretchBaseX() * ExeFloat(0x858A4C));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_GOLD));
        CFont::PrintStringFromBottom(halfWidth(), StretchBaseY() * ExeFloat(0x865374), m4);
    }

    if (!displayImmediately)
        return;

    const auto& m6 = m_BigMessage[STYLE_LIGHT_BLUE_TOP];
    if (m6[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(StretchBaseX(), StretchBaseY() * ExeFloat(0x859BB4));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(StretchBaseX() * ExeFloat(0x858B58));
        CFont::SetFontStyle(FONT_PRICEDOWN);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
        CFont::PrintString(halfWidth(), StretchBaseY() * ExeFloat(0x858B34), m6);
    }

    const auto& m3 = m_BigMessage[STYLE_MIDDLE_SMALLER];
    if (m3[0]) {
        CFont::SetBackground(false, false);
        CFont::SetJustify(false);
        CFont::SetScaleForCurrentLanguage(StretchBaseX() * ExeFloat(0x858CC8), StretchBaseY() * ExeFloat(0x866C48));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(StretchBaseX() * ExeFloat(0x858B58));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_GOLD));
        CFont::PrintString(halfWidth(), StretchBaseY() * ExeFloat(0x859990), m3);
    }

    // Como el original: ms = _ftol2(ms_fTimeStep * 0x858B38 * 1000.0); como dword sin signo para el temporizador flotante (fadd 2^32 si es negativo).
    const auto stepMs = [] { return (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x858C4C)); };

    // 0x58CFA8..0x58CFEA: fcomp 0.0 + test ah,41h + jne
    if (OddJob2OffTimer > ExeFloat(0x858B50)) {
        const int32 ms = stepMs();
        float msFloat = (float)ms;
        if (ms < 0) {
            msFloat += ExeFloat(0x858C54);
        }
        OddJob2OffTimer = OddJob2OffTimer - msFloat;
    }

    const auto& m5 = m_BigMessage[STYLE_WHITE_MIDDLE_SMALLER];
    if (!m5[0])
        return;

    // 0x58CFFD..0x58D00E: jp => sale con el temporizador > 0 o NaN
    if (!(OddJob2OffTimer <= ExeFloat(0x858B50)))
        return;

    switch ((int32)(int16)OddJob2On) {
    case 0:
        OddJob2XOffset = 380.0f; // mov dword ptr [0BAA39Ch],43BE0000h
        OddJob2On = 1;
        break;
    case 1:
        if (!(OddJob2XOffset > ExeFloat(0x858CA0))) { // fcomp 2.0 + test ah,41h + jne
            OddJob2On = 2;
            OddJob2Timer = 0;
        } else {
            float step = OddJob2XOffset * ExeFloat(0x85F0A0); // 0x58D05C: * 0.16666667
            if (step > ExeFloat(0x858A10)) {                  // fcom 40.0
                step = ExeFloat(0x858A10);
            }
            OddJob2XOffset = OddJob2XOffset - step;
        }
        break;
    case 2:
        OddJob2Timer = (uint16)(OddJob2Timer + (uint16)stepMs());
        if ((int16)OddJob2Timer > 1500) { // cmp cx,5DCh + jle (con signo)
            OddJob2On = 3;
        }
        break;
    case 3: {
        float step = OddJob2XOffset * ExeFloat(0x858CC4); // 0x58D0E3: * 0.2
        if (step < ExeFloat(0x858CA4)) {                  // fcom 30.0 + test ah,5
            step = ExeFloat(0x858CA4);
        }
        OddJob2XOffset = OddJob2XOffset - step;
        if (OddJob2XOffset < ExeFloat(0x866C44)) {        // -380.0
            OddJob2On = 0;
            OddJob2OffTimer = 5000.0f; // mov dword ptr [0BAA398h],459C4000h
        }
        break;
    }
    default:
        break;
    }

    if (!m1[0]) {
        CFont::SetBackground(false, false);
        CFont::SetScaleForCurrentLanguage(StretchBaseX() * ExeFloat(0x858CC8), StretchBaseY() * ExeFloat(0x866C48));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetProportional(true);
        CFont::SetCentreSize(StretchBaseX() * ExeFloat(0x858B58));
        CFont::SetFontStyle(FONT_MENU);
        CFont::SetEdge(2);
        CFont::SetDropColor({ 0, 0, 0, 255 });
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
        CFont::PrintString(halfWidth(), StretchBaseY() * ExeFloat(0x866C40), m5);
    }
}

// 0x58A330
// Como el original (0x58A782..0x58AA25): el EXE rellena el CRect con stores directos (+0 left, +4 bottom = Y mayor, +8 right,
// +0xC top = Y menor) sin comprobar nada; el constructor de la DLL hace assert(!IsFlipped()) y los discos 2 y 4 (left > right)
// y 3 y 4 (bottom > top) estan invertidos a proposito (espejo), asi que se rellenan los campos directamente.
static CRect HudRawRect(float left, float bottom, float right, float top) {
    CRect result;
    result.left = left; result.bottom = bottom; result.right = right; result.top = top;
    return result;
}

void CHud::DrawRadar() {
    if (CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_1 ||
        CEntryExitManager::ms_exitEnterState == EXIT_ENTER_STATE_2 ||
        FrontEndMenuManager.m_nRadarMode == eRadarMode::RADAR_MODE_OFF ||
        (m_ItemToFlash == ITEM_RADAR && EachFrames(8))
    ) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,     RWRSTATE(rwSHADEMODEFLAT));   // 0x58A384: estado 7 con valor 1

    CRadar::DrawMap();

    if (FrontEndMenuManager.m_nRadarMode == eRadarMode::RADAR_MODE_BLIPS_ONLY) {
        CRadar::DrawBlips();
        return;
    }

    // Como el original: todas las coordenadas salen de sx = fild maximumWidth * 0x859520 y sy = fild maximumHeight * 0x859524
    // con el orden exacto de multiplicaciones/sumas del EXE (x87 a 24 bits); las constantes se leen del EXE.
    const float sx = StretchBaseX();
    const float sy = StretchBaseY();
    const float sx40 = ExeFloat(0x858A10) * sx; // 40.0 * sx
    const float baseY = (float)RsGlobal.maximumHeight - sy * ExeFloat(0x866B70); // H - sy * 104.0

    CVehicle* vehicle = FindPlayerVehicle(-1, false);
    if (vehicle && vehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE && vehicle->m_nModelIndex != MODEL_VORTEX) {
        // Anillo del avion (0x58A3E3..0x58A551): el EXE lo expande en linea (cuatro esquinas girando alrededor del centro)
        // y lo dibuja con CSprite2d::Draw(x1..y4, color) (0x728520), no con CRadar::DrawRotatingRadarSprite.
        const auto& matrix = *vehicle->m_matrix;
        const float negRightZ = -matrix.GetRight().z;
        const float upZ       = matrix.GetUp().z;
        const float quarterPi = ExeFloat(0x859AB0);
        // fld negRightZ; fld upZ; fpatan; fchs; fsub quarterPi; fstp (0x58A422..0x58A435): -atan2(-right.z, up.z) - pi/4 con un solo redondeo
        const float baseAngle = HudRadarBaseAngleCpp(negRightZ, upZ, quarterPi);
        const float p1      = ExeFloat(0x866B78) * sx;              // 94.0 * sx
        const float radiusX = p1 - sx * ExeFloat(0x859008);         // p1 - 18.0 * sx
        const float centerX = p1 * ExeFloat(0x858B8C);              // p1 * 0.5
        const float q       = ExeFloat(0x866B74) * sy;              // 76.0 * sy
        const float radiusY = q - ExeFloat(0x859008) * sy;          // q - 18.0 * sy
        const float centerY = q * ExeFloat(0x858B8C);               // q * 0.5
        const float zero    = ExeFloat(0x858B50);                   // 0.0 (el EXE multiplica por el, no lo elimina)
        const float halfPi  = ExeFloat(0x858FE4);
        float xs[4], ys[4];
        for (int32 k = 0; k < 4; ++k) {
            const float angle = (float)k * halfPi + baseAngle;
            // fsin/fcos sin redondear (0x58A4C4..0x58A4F6): el seno y el coseno entran en la x87 sin pasar por float
            HudRadarRingPointCpp(angle, zero, radiusX, radiusY, sx40, centerX, centerY, baseY, &xs[k], &ys[k]);
        }
        // Orden de argumentos del EXE (0x58A52B..0x58A551): (e1, e0, e2, e3)
        Sprites[SPRITE_RADAR_RING_PLANE].Draw(xs[1], ys[1], xs[0], ys[0], xs[2], ys[2], xs[3], ys[3], CRGBA(255, 255, 255, 255));
    }

    // Altimetro en aviones y helicopteros (no el Vortex) o con paracaidas (0x58A556..0x58A5CC, 0x58A5A8..0x58A5C6)
    CVehicle* vehicle2 = FindPlayerVehicle(-1, false);
    const bool flyingVehicle = vehicle2 && (vehicle2->m_nVehicleSubType == VEHICLE_TYPE_PLANE || vehicle2->m_nVehicleSubType == VEHICLE_TYPE_HELI)
                               && vehicle2->m_nModelIndex != MODEL_VORTEX;
    if (flyingVehicle || FindPlayerPed(-1)->GetActiveWeapon().m_Type == WEAPON_PARACHUTE) {
        // CRect(left, bottom, right, top): +0xC = Y menor, +4 = Y mayor
        const CRect altimeter = HudRawRect( sx40 - sx * ExeFloat(0x858BA4), baseY, sx40 - sx * ExeFloat(0x85862C), ExeFloat(0x866B74) * sy + baseY );
        CSprite2d::DrawRect(altimeter, CRGBA(10, 10, 10, 100)); // rectangulo

        const CVehicle* v = FindPlayerVehicle(-1, false);
        const CVector* pos;
        if (v) {
            pos = v->m_matrix ? &v->m_matrix->GetPosition() : &v->m_placement.m_vPosn;
        } else {
            const CPed* ped = FindPlayerPed(-1);
            pos = ped->m_matrix ? &ped->m_matrix->GetPosition() : &ped->m_placement.m_vPosn;
        }
        // 0x58A68F..0x58A6C6: fcom 200.0 + test ah,41h + jne => z <= 200 (o NaN) usa 0x858B4C; si no, 0x866C08.
        const float z = pos->z;
        const float seventySixSy = ExeFloat(0x866B74) * sy;
        const float factor = z > ExeFloat(0x858A48) ? ExeFloat(0x866C08) : ExeFloat(0x858B4C);
        const float product = seventySixSy * (z * factor);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

        // min() del EXE (0x404330): (a < b) ? a : b
        const float minimum = seventySixSy < product ? seventySixSy : product;
        const float lineY = (baseY + seventySixSy) - minimum; // 0x58A722..0x58A734
        const CRect line = HudRawRect( sx40 - sx * ExeFloat(0x858FE8), lineY, sx40 - ExeFloat(0x858C80), lineY + ExeFloat(0x858CA0) );
        CSprite2d::DrawRect(line, CRGBA(200, 200, 200, 200)); // linea horizontal (altura actual)
    }

    // Discos del radar (0x58A782..0x58AA25). El EXE los calcula uno a uno (no reutiliza un CRect) y los pasa a
    // CSprite2d::Draw(CRect, CRGBA) de la ranura RADAR_DISC; el 2.o y el 4.o tienen left > right (espejo horizontal).
    const auto black = CRGBA(0, 0, 0, 255);
    const float discRight = ExeFloat(0x866B78) * sx * ExeFloat(0x858B8C) + sx40;          // (94*sx)*0.5 + 40*sx
    const float discBottomTop = (ExeFloat(0x866B74) * sy) * ExeFloat(0x858B8C) + baseY;    // (76*sy)*0.5 + baseY
    const float fourSy = sy * ExeFloat(0x858B90);                                          // 4.0 * sy
    const float discLeft1 = sx40 - sx * ExeFloat(0x858B90);                                // 40*sx - 4*sx
    const float discLeft2 = (sx * ExeFloat(0x858B90) + sx40) + ExeFloat(0x866B78) * sx;    // (4*sx + 40*sx) + 94*sx
    const float lower = (fourSy + baseY) + ExeFloat(0x866B74) * sy;                        // (4*sy + baseY) + 76*sy

    Sprites[SPRITE_RADAR_DISC].Draw(HudRawRect( discLeft1, baseY - fourSy, discRight, discBottomTop ), black); // arriba izquierda
    Sprites[SPRITE_RADAR_DISC].Draw(HudRawRect( discLeft2, baseY - fourSy, discRight, discBottomTop ), black); // arriba derecha
    Sprites[SPRITE_RADAR_DISC].Draw(HudRawRect( discLeft1, lower, discRight, discBottomTop ), black);          // abajo izquierda
    Sprites[SPRITE_RADAR_DISC].Draw(HudRawRect( discLeft2, lower, discRight, discBottomTop ), black);          // abajo derecha

    CRadar::DrawBlips();
}

// 0x58C080
void CHud::DrawScriptText(bool isBeforeFade) {
    CTheScripts::DrawScriptSpritesAndRectangles(isBeforeFade);

    for (auto& t : CTheScripts::IntroTextLines) {
        if (!t.GXTKey[0]) {
            continue;
        }
        if (t.IsDrawBeforeFade != isBeforeFade) {
            continue;
        }

        // Como el original (0x58C0BB..0x58C14B): sin correccion de relacion de aspecto; todo es
        // (fild maximumWidth/Height * 0x859520/0x859524) * valor, y la altura de la escala se multiplica por 0.5 al final.
        CFont::SetScale(StretchBaseX() * t.Scale.x, StretchBaseY() * t.Scale.y * ExeFloat(0x858B8C));
        CFont::SetColor(t.Color);
        CFont::SetJustify(t.Justify);
        if (t.HasRightJustify) {
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        } else {
            CFont::SetOrientation(t.IsCentered ? eFontAlignment::ALIGN_CENTER : eFontAlignment::ALIGN_LEFT);
        }
        CFont::SetWrapx(StretchBaseX() * t.WrapX);
        CFont::SetCentreSize(StretchBaseX() * t.CentreSize);
        CFont::SetBackground(t.HasBg, false);
        CFont::SetBackgroundColor(t.BgColor);
        CFont::SetProportional(t.IsProportional);
        CFont::SetDropColor(t.DropShadowColor);
        if (t.TextEdge) {
            CFont::SetEdge(t.TextEdge); // movzx eax,byte [esi+1Eh] (el DLL recibe int8)
        } else {
            CFont::SetDropShadowPosition((int16)(uint8)t.DropShadow); // movzx eax,byte [esi+1Dh]
        }
        CFont::SetFontStyle((eFontStyle)(uint8)t.FontStyle);

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
        // Como el original (0x58C1DD..0x58C225): x = W - (640.0 - Pos.x) * (W * c520), y = H - (448.0 - Pos.y) * (H * c524)
        CFont::PrintString(
            (float)RsGlobal.maximumWidth - (ExeFloat(0x85951C) - t.Pos.x) * StretchBaseX(),
            (float)RsGlobal.maximumHeight - (ExeFloat(0x8652B0) - t.Pos.y) * StretchBaseY(),
            text
        );
        CFont::SetEdge(0);
    }
}

namespace {
float SubtitleScaleY(int height, bool widescreen) {
    // fild height; fmul 0x859524; fmul (0x858F08 con pantalla ancha, 0x866C20 si no); fstp
    return IntTimesTwoFloatsCpp(height, ExeFloat(0x859524), ExeFloat(widescreen ? 0x858F08 : 0x866C20));
}
float SubtitleCentreVitals(int width) {
    // 0x58C4F6..0x58C52F: la cadena x87 de fild/fmul/fsub/fadd con el ancho entero exacto; ayudante de X87Math
    return HudSubtitleCentreVitalsCpp(width, ExeFloat(0x859520), ExeFloat(0x859000), ExeFloat(0x858BA4), ExeFloat(0x865374), ExeFloat(0x858C98));
}
float SubtitleScaleX(int width, bool vitals) {
    // fild width; fmul 0x859520; fmul 0x866C24; [fmul 0x858C98 con vitals]; fstp (el producto parcial ya es un float de 24 bits)
    const float base = IntTimesTwoFloatsCpp(width, ExeFloat(0x859520), ExeFloat(0x866C24));
    return vitals ? base * ExeFloat(0x858C98) : base;
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
        y = HudSubtitleBottomYCpp(RsGlobal.maximumHeight, ExeFloat(0x859524), ExeFloat(0x866C1C));
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

    // Como el original: ms = _ftol2(ms_fTimeStep * 0x858B38 * 1000.0 | -1000.0) y alfa = fade * 0x858CDC (0.001) * 255.0.
    const auto stepMs    = [] { return (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x858C4C)); };
    const auto stepMsNeg = [] { return (int32)(int64)(CTimer::GetTimeStep() * ExeFloat(0x858B38) * ExeFloat(0x859948)); };
    const auto fadeAlpha = [] { return (float)m_VehicleFadeTimer * ExeFloat(0x858CDC) * ExeFloat(0x859AAC); };

    // En el EXE el alfa queda SIN inicializar si el estado no esta en 1..4 (0x58AF3D..0x58AF40 salta a 0x58B040): el 0 es solo un valor definido.
    float alpha = 0.0f;
    switch (m_VehicleState) {
    case NAME_SHOW:
        if ((float)(int32)m_VehicleNameTimer > ExeFloat(0x859A94)) { // fild + fcomp 3000.0
            m_VehicleState = NAME_FADE_OUT;
            m_VehicleFadeTimer = 1000;
        }
        alpha = ExeFloat(0x859AAC);
        break;
    case NAME_FADE_IN:
        m_VehicleFadeTimer += stepMs();
        if (!((float)m_VehicleFadeTimer <= ExeFloat(0x858C4C))) {
            m_VehicleFadeTimer = 1000;
            m_VehicleState = NAME_SHOW;
        }
        alpha = fadeAlpha();
        break;
    case NAME_FADE_OUT:
        m_VehicleFadeTimer += stepMsNeg();
        if (m_VehicleFadeTimer < 0) {
            m_VehicleState = NAME_DONT_SHOW;
            m_VehicleFadeTimer = 0;
        }
        alpha = fadeAlpha();
        break;
    case NAME_SWITCH:
        m_VehicleFadeTimer += stepMsNeg();
        if (m_VehicleFadeTimer < 0) {
            m_VehicleNameTimer = 0;
            m_VehicleState = NAME_FADE_IN;
            m_VehicleFadeTimer = 0;
            m_pVehicleNameToPrint = m_pLastVehicleName;
        }
        alpha = fadeAlpha();
        break;
    default:
        break;
    }

    if (!m_Message[0]) {
        m_VehicleNameTimer += stepMs();
        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        // Como el original (0x58B080..0x58B0A5): x = fild W * c520, y = (fild H * c524) * 1.5 (0x858CE8)
        CFont::SetScaleForCurrentLanguage(StretchBaseX(), StretchBaseY() * ExeFloat(0x858CE8));
        CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
        CFont::SetRightJustifyWrap(0.0f);
        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        CFont::SetEdge(2);
        const uint8 alphaByte = (uint8)(int64)alpha; // 0x58B0CF..0x58B0D6
        CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_GREEN, alphaByte));
        CFont::SetDropColor({ 0, 0, 0, alphaByte });
        if (CTheScripts::bDisplayHud) {
            // 0x58B116..0x58B156: x = W - (W * c520) * 32.0, y = H - (H * c524) * 104.0
            CFont::PrintString(
                (float)RsGlobal.maximumWidth - StretchBaseX() * ExeFloat(0x85950C),
                (float)RsGlobal.maximumHeight - StretchBaseY() * ExeFloat(0x866B70),
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
    FrontEndMenuManager.DrawWindow(HudRawRect(40.f, panelTop, sx * f(0x858F98) + 40.f, top + sy * f(0x86532C)), "FEH_STA", 0, CRGBA(0, 0, 0, 190), false, true);
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
            const float rounded = static_cast<float>(FloorFloat((reaction * f(0x858B1C) + value) / product));
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

        // Como el original (0x588B0F..0x588B23): con arg0 se saltan los estilos 1 y 4 comparando la DIRECCION
        // del elemento (cmp edx,0BAA3E0h / cmp edx,0BAA3ECh), no el valor del float.
        if (arg0 && (i == STYLE_BOTTOM_RIGHT || i == STYLE_MIDDLE_SMALLER_HIGHER)) {
            continue;
        }
        std::ranges::fill(m_BigMessage[i], '\0');
    }
}

// 0x5893B0
void CHud::DrawAmmo(CPed* ped, int32 x, int32 y, float alpha) {
    constexpr int32 MAX_CLIP = 9999;

    const auto& weapon = ped->GetActiveWeapon();
    // Como el original: toda la aritmetica de municion es con signo de 32 bits (imul 66666667h, cmp/jg, cmp/jle).
    const int32 totalAmmo  = (int32)weapon.m_TotalAmmo;
    const int32 ammoInClip = (int32)weapon.m_AmmoInClip;
    const int32 ammoClip   = (int16)CWeaponInfo::GetWeaponInfo(weapon.m_Type, ped->GetWeaponSkill())->m_nAmmoClip; // movsx eax,word [eax+20h]
    const int32 remaining  = (int32)((uint32)totalAmmo - (uint32)ammoInClip); // 0x588... sub

    // Como el original: sprintf (0x821BB5) sin el relleno 0xFE de sprintf_s.
    if (ammoClip <= 1 || ammoClip >= 1000) {
        std::sprintf(gString, "%d", totalAmmo);
    } else if (weapon.m_Type == WEAPON_FLAMETHROWER) {
        const int32 total = std::min(remaining / 10, MAX_CLIP);
        std::sprintf(gString, "%d-%d", total, ammoInClip / 10);
    } else {
        const int32 total = remaining > MAX_CLIP ? MAX_CLIP : remaining;
        std::sprintf(gString, "%d-%d", total, ammoInClip);
    }
    AsciiToGxtChar(gString, gGxtString);

    // Como el original (0x5894A6..0x58952A): anchura = fild maximumWidth * 0x859520 * k; altura = fild maximumHeight * 0x859524 * k.
    const float stretchX = (float)RsGlobal.maximumWidth  * ExeFloat(0x859520);
    const float stretchY = (float)RsGlobal.maximumHeight * ExeFloat(0x859524);
    CFont::SetBackground(false, false);
    CFont::SetScale(stretchX * ExeFloat(0x858C24), stretchY * ExeFloat(0x858CB0));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetCentreSize(stretchX * ExeFloat(0x85951C)); // OJO: el EXE usa maximumWidth (0xC17044), no la altura
    CFont::SetProportional(true);
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);

    // Como el original (0x58954D..0x58963E): fcomp 9999.0 con test ah,5 y test ah,44h => se omite con restante >= 9999.
    if (   remaining >= MAX_CLIP
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
        || CWeaponInfo::GetWeaponInfo(weapon.m_Type, eWeaponSkill::STD)->m_nWeaponFire == WEAPON_FIRE_USE
        || CWeaponInfo::GetWeaponInfo(weapon.m_Type, eWeaponSkill::STD)->m_nSlot <= 1
    ) {
        CFont::SetEdge(0);
        return;
    }

    CFont::SetColor(HudColour.GetRGBA(HUD_COLOUR_LIGHT_BLUE, (uint8)(int64)alpha));
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
    // Como el original (0x58A163..0x58A1DB): todo en x87 a 24 bits, con este orden exacto de operaciones.
    const float sx = StretchBaseX();                                    // fild maximumWidth * 0x859520
    const float left = ExeFloat(0x858C94) * sx;                         // 54.0 * sx
    const float sy = ExeFloat(0x859524) * (float)RsGlobal.maximumHeight;
    const float top = ((float)RsGlobal.maximumHeight - sy * ExeFloat(0x866B70)) - sy * ExeFloat(0x85989C); // (H - sy*104) - sy*85
    // CRect(left, bottom, right, top): la Y menor va en +0xC (0x58A1CB = top) y la mayor en +4 (0x58A1DF = sy*64 + top).
    const CRect rect = HudRawRect(left, top, sx * ExeFloat(0x859A44) + left, sy * ExeFloat(0x859A44) + top);
    Sprites[SPRITE_SKIP_ICON].Draw(rect, CRGBA(255, 255, 255, 255));

    CFont::SetBackground(false, false);
    CFont::SetScale(StretchBaseX() * ExeFloat(0x858C24), StretchBaseY() * ExeFloat(0x858CB0));
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetCentreSize((float)RsGlobal.maximumWidth);
    CFont::SetProportional(true);
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));

    // Como el original (0x58A299..0x58A324): la cadena se obtiene al final, justo antes de imprimir.
    const float sx2 = StretchBaseX();
    const float sy2 = StretchBaseY();
    const GxtChar* text = TheText.Get("FEC_TSK"); // TRIP SKIP
    const float y = (((float)RsGlobal.maximumHeight - sy2 * ExeFloat(0x866B70)) - sy2 * ExeFloat(0x85989C) + sy2 * ExeFloat(0x859A44)) - (sy2 + sy2);
    const float x = sx2 * ExeFloat(0x859A44) * ExeFloat(0x858B8C) + sx2 * ExeFloat(0x858C94);
    CFont::PrintString(x, y, text);
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
    // Como el original (0x58D7D5..0x58D7DD): primero el filtro de textura lineal.
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    const auto modelId = ped->GetActiveWeapon().GetWeaponInfo().m_nModelId1;
    if (modelId <= 0) {
        // Como el original (0x58D917..0x58D988): rectangulo con
        // ancho = (fild maximumWidth * 0x859520) * 47.0 y alto = (fild maximumHeight * 0x859524) * 58.0; alfa por _ftol2.
        const float width  = (float)RsGlobal.maximumWidth  * ExeFloat(0x859520) * ExeFloat(0x866C4C);
        const float height = (float)RsGlobal.maximumHeight * ExeFloat(0x859524) * ExeFloat(0x866C50);
        const float x0 = (float)x;
        const float y0 = (float)y;
        // CRect(left, bottom, right, top): el EXE escribe {x, alto + y, x + ancho, y} (offsets 0, 4, 8, 0xC).
        const CRect rect = HudRawRect(x0, y0, width + x0, height + y0);
        Sprites[SPRITE_FIST].Draw(rect, CRGBA(255, 255, 255, (uint8)(int64)alpha));
        return;
    }

    // Como el original (0x58D80B..0x58D857): la ranura del TXD se lee con el byte de "libre" (bit 7) y el diccionario
    // RW (primer dword del objeto) se comprueba antes de buscar la textura.
    auto mi = CModelInfo::GetModelInfo(modelId);
    auto txd = CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex);
    if (!txd || !txd->m_pRwDictionary)
        return;

    auto texture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, CKeyGen::AppendStringToKey(mi->m_nKey, "ICON"));
    if (!texture)
        return;

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(texture)));
    // Como el original (0x58D877..0x58D8FA): mitades = ((fild max * const) * 47.0|58.0) * 0.5, centro = coordenada + mitad.
    const float halfHeight = (float)RsGlobal.maximumHeight * ExeFloat(0x859524) * ExeFloat(0x866C50) * ExeFloat(0x858B8C);
    const float halfWidth  = (float)RsGlobal.maximumWidth  * ExeFloat(0x859520) * ExeFloat(0x866C4C) * ExeFloat(0x858B8C);
    CSprite::RenderOneXLUSprite(
        { (float)x + halfWidth, (float)y + halfHeight, 1.0f },
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
    if (m_ItemToFlash == ITEM_ARMOUR && EachFrames(8)) // 0x5890A3..0x5890B4
        return;

    auto* player = FindPlayerPed(playerId);
    // Como el original (0x5890D1..0x5890DC): fcomp 1.0 + test ah,41h + jne: sale con armadura <= 1.0 y con NaN.
    if (!(player->m_fArmour > 1.0f))
        return;

    // Como el original: el maximo sale de Players[playerId] (0x589110, [esi+0B7CEE8h]) y la anchura/altura son
    // fild maximumWidth/Height * 0x859520/0x859524 * 62.0/9.0 (x87 a 24 bits), truncadas con _ftol2.
    const auto& info = FindPlayerInfo(playerId);
    const float width  = (float)RsGlobal.maximumWidth  * ExeFloat(0x859520) * ExeFloat(0x86503C);
    const float height = (float)RsGlobal.maximumHeight * ExeFloat(0x859524) * ExeFloat(0x85EED4);
    CSprite2d::DrawBarChart(
        (float)x,
        (float)y,
        (uint16)(int64)width,
        (uint8)(int64)height,
        player->m_fArmour / (float)info.m_nMaxArmour * 100.0f,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY),
        CRGBA(0, 0, 0, 0)
    );
}

// 0x589190
void CHud::RenderBreathBar(int32 playerId, int32 x, int32 y) {
    if (m_ItemToFlash == ITEM_BREATH && EachFrames(8)) // 0x589193..0x5891A4
        return;

    auto* player = FindPlayerPed(playerId);
    const float width  = (float)RsGlobal.maximumWidth  * ExeFloat(0x859520) * ExeFloat(0x86503C);
    const float height = (float)RsGlobal.maximumHeight * ExeFloat(0x859524) * ExeFloat(0x85EED4);
    CSprite2d::DrawBarChart(
        (float)x,
        (float)y,
        (uint16)(int64)width,
        (uint8)(int64)height,
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
    if (m_ItemToFlash == ITEM_HEALTH && EachFrames(8)) // 0x589273..0x589287
        return;

    auto* player = FindPlayerPed(playerId);
    // Como el original (0x5892A4..0x5892AF): _ftol2 y cmp ax,0Ah / jge: se compara solo la palabra baja con signo.
    if ((int16)(int64)player->m_fHealth < 10 && EachFrames(8))
        return;

    // Como el original (0x5892B8..0x5892EC): (fild maximumWidth * 0x859520) * maxHealth * 109.0 (0x866BB8) / modificador(10), _ftol2.
    const auto& info = FindPlayerInfo(playerId);
    const float x109 = (float)RsGlobal.maximumWidth * ExeFloat(0x859520) * (float)(int32)info.m_nMaxHealth * ExeFloat(0x866BB8);
    const auto totalWidth = (uint16)(int64)(x109 / CStats::GetFatAndMuscleModifier(STAT_MOD_10));
    const float height = (float)RsGlobal.maximumHeight * ExeFloat(0x859524) * ExeFloat(0x85EED4);
    // 0x58933C..0x58939A: X = (fild maximumWidth * 0x859520 * 109.0) - totalWidth + x
    const float x109b = (float)RsGlobal.maximumWidth * ExeFloat(0x859520) * ExeFloat(0x866BB8);

    CSprite2d::DrawBarChart(
        x109b - (float)totalWidth + (float)x,
        (float)y,
        totalWidth,
        (uint8)(int64)height,
        player->m_fHealth * 100.0f / (float)info.m_nMaxHealth,
        false,
        false,
        true,
        HudColour.GetRGB(HUD_COLOUR_RED),
        CRGBA(0, 0, 0, 0)
    );
}
