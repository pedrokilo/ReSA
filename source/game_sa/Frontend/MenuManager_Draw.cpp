#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"

#include "AudioEngine.h"
#include "AEUserRadioTrackManager.h"
#include "Pad.h"
#include "MenuSystem.h"
#include "PostEffects.h"
#include "app/platform/platform.h"
#include "ControllerConfigManager.h"
#include "VideoMode.h"

using enum eControllerAction;

constexpr std::array<eControllerAction, 28> ControllerActionsAvailableOnFoot = {
    PED_FIRE_WEAPON,
    PED_CYCLE_WEAPON_RIGHT,
    PED_CYCLE_WEAPON_LEFT,
    PED_JUMPING,
    PED_SPRINT,
    CAMERA_CHANGE_VIEW_ALL_SITUATIONS,
    VEHICLE_ENTER_EXIT,
    GO_FORWARD,
    GO_BACK,
    GO_LEFT,
    GO_RIGHT,
    PED_LOOKBEHIND,
    PED_DUCK,
    PED_ANSWER_PHONE,
    VEHICLE_STEERUP,
    VEHICLE_STEERDOWN,
    VEHICLE_ACCELERATE,
    VEHICLE_RADIO_STATION_UP,
    VEHICLE_RADIO_STATION_DOWN,
    VEHICLE_RADIO_TRACK_SKIP,
    VEHICLE_HORN,
    VEHICLE_LOOKLEFT,
    VEHICLE_LOOKBEHIND,
    VEHICLE_MOUSELOOK,
    VEHICLE_TURRETLEFT,
    VEHICLE_TURRETRIGHT,
    PED_CYCLE_TARGET_LEFT,
    PED_FIRE_WEAPON_ALT
}; // 0x865598

constexpr std::array<eControllerAction, 25> ControllerActionsAvailableInCar = {
    PED_FIRE_WEAPON,
    PED_FIRE_WEAPON_ALT,
    GO_FORWARD,
    GO_BACK,
    GO_LEFT,
    GO_RIGHT,
    PED_SNIPER_ZOOM_IN,
    PED_SNIPER_ZOOM_OUT,
    PED_ANSWER_PHONE,
    VEHICLE_ENTER_EXIT,
    PED_WALK,
    VEHICLE_FIRE_WEAPON,
    VEHICLE_FIRE_WEAPON_ALT,
    VEHICLE_STEERLEFT,
    VEHICLE_STEERRIGHT,
    VEHICLE_STEERUP,
    VEHICLE_BRAKE,
    VEHICLE_LOOKLEFT,
    VEHICLE_LOOKRIGHT,
    VEHICLE_LOOKBEHIND,
    VEHICLE_MOUSELOOK,
    TOGGLE_SUBMISSIONS,
    VEHICLE_HANDBRAKE,
    PED_1RST_PERSON_LOOK_LEFT,
    PED_1RST_PERSON_LOOK_RIGHT
}; // 0x865608

// Como el original: el EXE inlinea el escalado como (W * 1/640) * a y (H * 1/448) * a,
// con las constantes float 0x859520 / 0x859524 y sin el atajo de StretchX/StretchY.
static inline float InlineStretchX(float a) { return SCREEN_WIDTH * 0.0015625f * a; }
static inline float InlineStretchY(float a) { return SCREEN_HEIGHT * 0.002232143f * a; }

// Como el original: escalado inline con atajo para 640x448 y factor precalculado (valor/640, valor/448)
// tal como está en las constantes del EXE (se pasa el patrón de bits exacto)
static inline float InlineScaleX(float at640, uint32 factorBits) {
    return RsGlobal.maximumWidth == DEFAULT_SCREEN_WIDTH ? at640 : SCREEN_WIDTH * std::bit_cast<float>(factorBits);
}
static inline float InlineScaleY(float at448, uint32 factorBits) {
    return RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? at448 : SCREEN_HEIGHT * std::bit_cast<float>(factorBits);
}

// 0x57C290
void CMenuManager::DrawFrontEnd() {
    if (m_bDontDrawFrontEnd) {
        return;
    }

    CFont::SetAlphaFade(255.0f);
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    DefinedState2d();
    SetFrontEndRenderStates();

    m_RadioAvailable = !AudioEngine.IsRadioRetuneInProgress();

    if (m_nCurrentScreen == SCREEN_INITIAL) {
        m_nCurrentScreen = m_bMainMenuSwitch ? SCREEN_MAIN_MENU : SCREEN_PAUSE_MENU;
    }

    if (m_nCurrentScreenItem == 0 && aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == MENU_ACTION_TEXT) {
        m_nCurrentScreenItem = 1;
    }

    DrawBackground();
}

// NOTSA
void CMenuManager::DrawBuildInfo() {
    std::string buildInfo = BUILD_NAME_FULL;
    buildInfo += std::format(" / RW {}.{}.{}.{}.{}",
        (0xF & RwEngineGetVersion() >> 16),
        (0xF & RwEngineGetVersion() >> 12),
        (0xF & RwEngineGetVersion() >> 8),
        (0xF & RwEngineGetVersion() >> 4),
        (0xF & RwEngineGetVersion() >> 0)
    );

    CFont::SetProportional(true);
    CFont::SetScale(StretchX(0.25f), StretchY(0.5f));
    CFont::SetColor({ 255, 255, 255, 100 });
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::PrintStringFromBottom(SCREEN_STRETCH_FROM_RIGHT(10.0f), SCREEN_STRETCH_FROM_BOTTOM(10.0f), GxtCharFromAscii(buildInfo.c_str()));
}

// 0x57B750
void CMenuManager::DrawBackground() {
    if (!m_bTexturesLoaded) {
        return;
    }

    const auto GetSpriteId = [=]() -> auto {
        switch (m_nCurrentScreen) {
        case SCREEN_STATS:
        case SCREEN_LANGUAGE:
        case SCREEN_QUIT_GAME_ASK:
            return FRONTEND_SPRITE_BACK4;
        case SCREEN_START_GAME:
        case SCREEN_USER_TRACKS_OPTIONS:
        case SCREEN_OPTIONS:
            return FRONTEND_SPRITE_BACK3;
        case SCREEN_BRIEF:
        case SCREEN_LOAD_GAME:
        case SCREEN_DELETE_GAME:
        case SCREEN_GAME_SAVE:
            return FRONTEND_SPRITE_BACK2;
        case SCREEN_AUDIO_SETTINGS:
            return FRONTEND_SPRITE_BACK5;
        case SCREEN_DISPLAY_SETTINGS:
        case SCREEN_DISPLAY_ADVANCED:
            return FRONTEND_SPRITE_BACK7;
        case SCREEN_MAP:
        case SCREEN_CONTROLS_DEFINITION:
            return FRONTEND_SPRITE_ARROW;
        case SCREEN_CONTROLLER_SETUP:
        case SCREEN_MOUSE_SETTINGS:
            return FRONTEND_SPRITE_BACK6;
        default:
            return FRONTEND_SPRITE_BACK8;
        }
    };

    const auto GetBackgroundRect = [=]() -> CRect {
        switch (m_BackgroundSprite) {
        case SCREEN_LOAD_FIRST_SAVE:
        case SCREEN_SAVE_DONE_1:
        case SCREEN_DELETE_FINISHED:
        case SCREEN_DELETE_SUCCESSFUL:
        case SCREEN_SAVE_WRITE_ASK:
            // Como el original: W - StretchX(256) (0x5733E0)
            return {
                SCREEN_WIDTH - StretchX(256.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(256.0f)
            };
        case SCREEN_GAME_SAVE: {
            // Como el original: la mitad del ancho es división entera (cdq/sar) antes de pasar a float
            const float halfWidth = (float)(RsGlobal.maximumWidth / 2);
            return {
                halfWidth - StretchX(128.0f),
                0.0f,
                StretchX(128.0f) + halfWidth,
                StretchY(256.0f)
            };
        }
        case SCREEN_SAVE_DONE_2:
            return {
                SCREEN_WIDTH - StretchX(300.0f),
                0.0f,
                SCREEN_WIDTH,
                StretchY(200.0f)
            };
        default:
            assert(true); // Bad R*
            return {};    // suppress warn
        }
    };

    m_BackgroundSprite = GetSpriteId();
    auto backgroundRect = GetBackgroundRect();

    // 0x57B7E1
    CRect screenRect(-5.0f, -5.0f, SCREEN_WIDTH + 5.0f, SCREEN_HEIGHT + 5.0f);
    CSprite2d::DrawRect(screenRect, MENU_BG);

    if (m_BackgroundSprite) {
        m_aFrontEndSprites[m_BackgroundSprite].Draw(backgroundRect, CRGBA(255, 255, 255, 255));
    }

    // 0x57BA02
    if (m_nCurrentScreen == SCREEN_MAP) {
        auto originalOrigin = m_vMapOrigin;
        auto originalZoom   = m_fMapZoom;

        PrintMap();

        m_fMapZoom   = originalZoom;
        m_vMapOrigin = originalOrigin;
    }

#ifdef USE_BUILD_INFORMATION
    DrawBuildInfo();
#endif

    // 0x57BA42
    if (m_nCurrentScreen == SCREEN_CONTROLS_DEFINITION) {
        DrawControllerSetupScreen();
    } else if (m_nCurrentScreen == SCREEN_EMPTY) {
        DrawQuitGameScreen();
    } else {
        DrawStandardMenus(true);
    }

    // 0x57BA6B
    if (m_ControllerError != eControllerError::NONE) {
        if (!bWereError) {
            bWereError = true;
            m_ErrorStartTime = CTimer::GetTimeInMSPauseMode();
        }

        if (m_ErrorPendingReset) {
            m_ErrorStartTime = CTimer::GetTimeInMSPauseMode();
            m_ErrorPendingReset = false;
        }

        // Como el original: MessageScreen inlineado (sin fondo, sin cámara iniciada) y luego CFont::DrawFonts
        switch (m_ControllerError) {
        case eControllerError::VEHICLE:
            // Error! Changing controls on the 'In Vehicle' screen has caused one or more control actions to be unbound on the 'On Foot' screen. Please check all control actions are set
            // Press ESC to continue
            MessageScreen("FEC_ER3", false, false);
            break;
        case eControllerError::FOOT:
            // Error! Changing controls on the 'On Foot' screen has caused one or more control actions to be unbound on the 'In Vehicle' screen. Please check all control actions are set
            // Press ESC to continue
            MessageScreen("FEC_ER2", false, false);
            break;
        default:
            // Error! One or more control actions are not bound to a key or button. Please check all control actions are set
            // Press ESC to continue
            MessageScreen("FEC_ERI", false, false);
            break;
        }
        CFont::DrawFonts();

        // Como el original: con error de mandos la función termina aquí (no hay escaneo ni ratón)
        if (CTimer::GetTimeInMSPauseMode() - m_ErrorStartTime <= 7'000) {
            CPad::GetPad(m_nPlayerNumber);
            if (!CPad::IsEscJustPressed() || CTimer::GetTimeInMSPauseMode() - m_ErrorStartTime <= 1'000) {
                return;
            }
        }
        m_ControllerError = eControllerError::NONE;
        m_ErrorPendingReset = true;
        return;
    }

    // 0x57BC19
    if (m_bScanningUserTracks) {
        static auto& updateScanningTime = StaticRef<bool>(0x8CDFFA); // true
        static auto& progressDir = StaticRef<int32>(0x8CDFFC); // -1
        static auto& progressPos = StaticRef<int32>(0x8CE000); // DEFAULT_SCREEN_WIDTH / 2

        if (!bScanningUserTracks) {
            bScanningUserTracks = true;
            m_UserTrackScanningTime = CTimer::GetTimeInMSPauseMode();
        }

        // SCANNING USER TRACKS - PLEASE WAIT
        //
        // Press ESC to cancel
        MessageScreen("FEA_SMP", false, false);

        const auto left = DEFAULT_SCREEN_WIDTH / 2 - 50;
        const auto right = DEFAULT_SCREEN_WIDTH / 2 + 50;

        if ((CTimer::m_FrameCounter & 4) != 0) {
            progressPos -= progressDir;
            if (progressPos >= right) {
                progressDir = 1;
            }
            if (progressPos <= left) {
                progressDir = -1;
            }
        }

        // Como el original: escalado inline con atajo 640x448 y constantes precalculadas
        // (250/448 = 0x865560, 245/448 = 0x865324, 375/640 = 0x865558, 270/640 = 0x865550)
        const float barTop    = RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? 250.0f : SCREEN_HEIGHT * std::bit_cast<float>(0x3F0EDB6Eu);
        const float barBottom = RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? 245.0f : SCREEN_HEIGHT * 0.546875f;
        const float barLeft   = RsGlobal.maximumWidth  == DEFAULT_SCREEN_WIDTH  ? 270.0f : SCREEN_WIDTH  * 0.421875f;
        const float barRight  = RsGlobal.maximumWidth  == DEFAULT_SCREEN_WIDTH  ? 375.0f : SCREEN_WIDTH  * 0.5859375f;
        CSprite2d::DrawRect({ barLeft, barBottom, barRight, barTop }, MENU_PROGRESS_BG); // (left, bottom, right, top)

        // Como el original: (float)pos * W * 1/640 inline, con atajo para 640
        const auto ScaleProgressX = [](int32 x) {
            return RsGlobal.maximumWidth == DEFAULT_SCREEN_WIDTH ? (float)x : (float)x * SCREEN_WIDTH * 0.0015625f;
        };
        CSprite2d::DrawRect({ ScaleProgressX(progressPos), barBottom, ScaleProgressX(progressPos + 5), barTop }, MENU_TEXT_LIGHT_GRAY);

        CFont::DrawFonts();
        ResetHelperText();

        if (updateScanningTime) {
            m_UserTrackScanningTime = CTimer::GetTimeInMSPauseMode();
            updateScanningTime = false;
        }

        if (!AEUserRadioTrackManager.ScanUserTracks()) {
            CPad::GetPad(m_nPlayerNumber);
            if (!CPad::IsEscJustPressed()) {
                return;
            }
        }
        if (CTimer::GetTimeInMSPauseMode() - m_UserTrackScanningTime <= 3'000) {
            if (!CPad::GetPad(m_nPlayerNumber)->IsEscJustPressed()) {
                return;
            }
        }

        // Como el original: el estado 2/3 se pone a OFF antes de mirarlo, así que un ERROR
        // sin ESC no deja texto de ayuda (FEA_SCF solo sale con ESC)
        auto helperText = HELPER_NONE;
        auto scanState  = AEUserRadioTrackManager.m_nUserTracksScanState;
        if (scanState >= USER_TRACK_SCAN_COMPLETE && scanState <= USER_TRACK_SCAN_ERROR) {
            AEUserRadioTrackManager.m_nUserTracksScanState = USER_TRACK_SCAN_OFF;
        }
        if (scanState == USER_TRACK_SCAN_COMPLETE && !CPad::GetPad(m_nPlayerNumber)->IsEscJustPressed()) {
            AEUserRadioTrackManager.Initialise();
            helperText = FEA_SCS;
        } else {
            scanState = AEUserRadioTrackManager.m_nUserTracksScanState;
            if (scanState >= USER_TRACK_SCAN_COMPLETE && scanState <= USER_TRACK_SCAN_ERROR) {
                AEUserRadioTrackManager.m_nUserTracksScanState = USER_TRACK_SCAN_OFF;
            }
            if (scanState == USER_TRACK_SCAN_ERROR || CPad::GetPad(m_nPlayerNumber)->IsEscJustPressed()) {
                helperText = FEA_SCF;
            }
        }

        if (helperText) {
            m_bScanningUserTracks = false;
            updateScanningTime = true;
            SetHelperText(helperText);
        }
        DisplayHelperText(nullptr);
        return;
    } else if (m_DisplayTheMouse) { // 0x57BF62
        CFont::RenderFontBuffer();

        // Como el original: los límites del mapa se truncan a enteros (_ftol2) y se comparan con la posición entera
        const int32 mapTop    = (int32)StretchY(60.0f);
        const int32 mapBottom = (int32)(SCREEN_HEIGHT - StretchY(60.0f));
        const int32 mapLeft   = (int32)StretchX(60.0f);
        const int32 mapRight  = (int32)(SCREEN_WIDTH - StretchX(60.0f));

        const auto DrawCursor = [this](eFrontend spriteId) {
            // Como el original: todas las medidas pasan por StretchX/StretchY (0x5733E0/0x573410)
            CRect rect;
            const float mouseX = float(m_nMousePosX);
            rect.left   = StretchX(6.0f) + mouseX;
            const float mouseY = float(m_nMousePosY);
            rect.bottom = StretchY(3.0f) + mouseY;
            rect.right  = StretchX(24.0f) + mouseX;
            rect.top    = StretchY(21.0f) + mouseY;
            m_aFrontEndSprites[spriteId].Draw(rect, { 100, 100, 100, 50 }); // shadow

            rect.left   = float(m_nMousePosX);
            rect.bottom = float(m_nMousePosY);
            rect.right  = StretchX(18.0f) + rect.left;
            rect.top    = StretchY(18.0f) + rect.bottom;
            m_aFrontEndSprites[spriteId].Draw(rect, CRGBA(255, 255, 255, 255));
        };

        bool crossHair = false;
        if (m_nCurrentScreen == SCREEN_MAP) {
            const bool lmb = CPad::IsMouseLButton();
            CPad::GetPad(m_nPlayerNumber);
            crossHair = lmb
                && m_nMousePosY > mapTop && m_nMousePosY < mapBottom
                && m_nMousePosX > mapLeft && m_nMousePosX < mapRight;
        }
        DrawCursor(crossHair ? FRONTEND_SPRITE_CROSS_HAIR : FRONTEND_SPRITE_MOUSE);
    }
}

// 0x5794A0
void CMenuManager::DrawStandardMenus(bool drawTitle) {
    constexpr uint16 DEFAULT_CONTENT_X = APP_MINIMAL_WIDTH / 2;
    constexpr uint16 DEFAULT_CONTENT_Y = APP_MINIMAL_HEIGHT / 4 + 10;

    const GxtChar* rightColumnText; // text for right column
    GxtChar        rightColumnBuffer[64]; // Como el original: búfer local compartido por resolución y antialiasing
    const GxtChar* displayText;     // text to display

    float itemYPosition = 0.0;
    bool shouldDrawStandardItems = true;
    CFont::SetBackground(false, false);
    CFont::SetProportional(true);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    // Como el original: W - 10 en entero y RightJustifyWrap fijo a 10.0
    CFont::SetWrapx((float)(RsGlobal.maximumWidth - 10));
    CFont::SetRightJustifyWrap(10.0f);
    CFont::SetCentreSize(SCREEN_WIDTH);

    if (m_nCurrentScreen == eMenuScreen::SCREEN_STATS) {
        PrintStats();
    }
    if (m_nCurrentScreen == eMenuScreen::SCREEN_BRIEF) {
        PrintBriefs();
    }
    if (m_nCurrentScreen == eMenuScreen::SCREEN_AUDIO_SETTINGS && drawTitle) {
        PrintRadioStationList();
    }

    if (drawTitle && aScreens[m_nCurrentScreen].m_szTitleName[0]) {
        if (m_nCurrentScreen != eMenuScreen::SCREEN_MAP || !m_bMapLoaded) {
            CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
            CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
            // Como el original: escalas y posición inline (1.3/640, 2.1/448, 40/640, 28/448)
            CFont::SetScale(InlineScaleX(1.3f, 0x3B051EB8u), InlineScaleY(2.1f, 0x3B999999u));
            CFont::SetEdge(1);
            CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
            CFont::SetDropColor(HudColour.GetRGB(HUD_COLOUR_BLACK));
            CFont::PrintString(InlineScaleX(40.0f, 0x3D800000u), InlineScaleY(28.0f, 0x3D800000u), TheText.Get(aScreens[m_nCurrentScreen].m_szTitleName));
        }
    }

    // 0x5796B4
    if (aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT) {
        // Como el original: W - 40 en entero y escala inline (0.49/640, 1.2/448)
        CFont::SetWrapx((float)(RsGlobal.maximumWidth - 40));
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetScaleForCurrentLanguage(InlineScaleX(std::bit_cast<float>(0x3EFAE147u), 0x3A48B439u), InlineScaleY(1.2f, 0x3B2F8AF9u));
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetEdge(2);
        CFont::SetDropColor(MENU_BG);
        CFont::SetColor(MENU_TEXT_NORMAL);

        const auto GetText = [&](bool condition, const char* key = "") {
            return TheText.Get(!condition ? aScreens[m_nCurrentScreen].m_aItems[0].m_szName : key);
        };
        const GxtChar* textOne;

        // 0x57979A
        switch (m_nCurrentScreen) {
        case eMenuScreen::SCREEN_NEW_GAME_ASK:  textOne = GetText(m_bMainMenuSwitch, "FESZ_QQ"); break; // Are you sure you want to start a new game?
        case eMenuScreen::SCREEN_LOAD_GAME_ASK: textOne = GetText(m_bMainMenuSwitch, "FES_LCG"); break; // Are you sure you want to load this save game?
        case eMenuScreen::SCREEN_SAVE_WRITE_ASK: {
            switch (GetSavedGameState(m_SelectedSlot)) {
            case eSlotState::SLOT_FILLED:    textOne = TheText.Get("FESZ_QO"); break; // Are you sure you wish to overwrite this save file?
            case eSlotState::SLOT_CORRUPTED: textOne = TheText.Get("FESZ_QC"); break; // Proceed with overwriting this corrupted save game?
            default:                         textOne = GetText(false); break;
            }
            break;
        }
        case eMenuScreen::SCREEN_QUIT_GAME_ASK: textOne = GetText(m_bMainMenuSwitch, "FEQ_SRW"); break; // Are you sure you want to quit the game?
        default:                                textOne = GetText(false); break;
        }

        CFont::PrintString(InlineScaleX(60.0f, 0x3DC00000u), InlineScaleY(97.0f, 0x3E5DB6DBu), textOne);
        CFont::SetWrapx((float)(RsGlobal.maximumWidth - 10));
        CFont::SetRightJustifyWrap(10.0f);
    }

    if (m_nCurrentScreen == eMenuScreen::SCREEN_CONTROLS_DEFINITION) {
        if (m_EditingControlOptions) {
            shouldDrawStandardItems = false;
        }
        DrawControllerScreenExtraText(-8);
    }

    const bool hasLabel = aScreens[m_nCurrentScreen].m_aItems[0].m_nActionType == eMenuAction::MENU_ACTION_TEXT;

    // 0x5798CE
    for (auto i = 0; i < std::size(aScreens[m_nCurrentScreen].m_aItems); i++) {
        auto& item = aScreens[m_nCurrentScreen].m_aItems[i];
        rightColumnText = nullptr;
        const uint16 lineHeight = (item.m_nType == eMenuEntryType::TI_MPACK) ? 20 : 30;

        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        // Como el original: escalas inline (0.7/640, 1/448, 0.42/640, 0.95/448) y sombra negra constante
        if (item.m_nType < eMenuEntryType::TI_SLOT1 || item.m_nType > eMenuEntryType::TI_SLOT8) {
            CFont::SetScale(InlineScaleX(0.7f, 0x3A8F5C29u), InlineScaleY(1.0f, 0x3B124925u));
            CFont::SetEdge(2);
        } else {
            CFont::SetEdge(1);
            CFont::SetScale(InlineScaleX(std::bit_cast<float>(0x3ED70A3Eu), 0x3A2C0832u), InlineScaleY(0.95f, 0x3B0AF8AFu));
        }
        CFont::SetDropColor(MENU_BG);
        if (i == m_nCurrentScreenItem && m_bMapLoaded) {
            CFont::SetColor(MENU_TEXT_SELECTED);
        } else {
            CFont::SetColor(MENU_TEXT_NORMAL);
        }

        switch (item.m_nAlign) {
        case eMenuAlign::MENU_ALIGN_LEFT:  CFont::SetOrientation(eFontAlignment::ALIGN_LEFT); break;
        case eMenuAlign::MENU_ALIGN_RIGHT: CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT); break;
        default:                           CFont::SetOrientation(eFontAlignment::ALIGN_CENTER); break;
        }

        // Como el original: los packs de misiones siempre se recolocan
        if ((!item.m_X && !item.m_Y) || item.m_nType == eMenuEntryType::TI_MPACK) {
            if (i == 0 || (i == 1 && hasLabel)) {
                item.m_X = DEFAULT_CONTENT_X;
                item.m_Y = DEFAULT_CONTENT_Y;
            } else {
                item.m_X = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_X;
                item.m_Y = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_Y + lineHeight;
            }
        }

        if (item.m_nActionType == eMenuAction::MENU_ACTION_TEXT || std::string(item.m_szName).empty()) {
            continue;
        }

        const bool isSlot = IsSaveSlot(item.m_nType);

        // Como el original: el desplazamiento X de la flecha es entero (_ftol2) y se compara en 16 bits
        int32 xOffset = 0;

        switch (item.m_nType) {
        case eMenuEntryType::TI_MPACK: {
            // Como el original (0x579CDD): sin comprobar el rango del índice (con i < 2 lee antes del array)
            const auto& MPacks = m_MissionPacks[i - 2];
            // Como el original: se mira el id del pack (no el nombre) y el nombre va a gGxtString (0xC1B100)
            if (!MPacks.m_Id) {
                item.m_nActionType = eMenuAction::MENU_ACTION_SKIP;
                displayText = nullptr;
                item.m_Y = aScreens[m_nCurrentScreen].m_aItems[i - 1].m_Y;
            } else {
                AsciiToGxtChar(MPacks.m_Name, gGxtString);
                item.m_nActionType = eMenuAction::MENU_ACTION_MPACK;
                displayText = gGxtString;
            }
            break;
        }
        case eMenuEntryType::TI_MOUSEJOYPAD:
            // Como el original (0x579D5A): cualquier método distinto de 0 es el del mando
            displayText = TheText.Get(m_ControlMethod != eController::MOUSE_PLUS_KEYS
                ? "FEJ_TIT" // Joypad Settings
                : "FEC_MOU" // Mouse Settings
            );
            break;
        default: {
            if (isSlot) {
                item.m_X = 80;
                CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);

                // Como el original: el nombre se usa en su propio búfer (0x5D0F40) y, si mide 254 o más,
                // se le concatena "..." en sitio; un nombre o texto vacío cae al caso de ranura libre
                displayText = nullptr;
                switch (GetSavedGameState(i - 1)) {
                case eSlotState::SLOT_FILLED: { // Valid save
                    auto* saveName = const_cast<GxtChar*>(GetNameOfSavedGame(i - 1));
                    if (GxtCharStrlen(saveName) >= 254) {
                        AsciiToGxtChar("...", gGxtString2);
                        GxtCharStrcat(saveName, gGxtString2);
                    }
                    rightColumnText = GetSavedGameDateAndTime(i - 1);
                    displayText = saveName;
                    break;
                }
                case eSlotState::SLOT_CORRUPTED: // Corrupted save
                    displayText = TheText.Get("FESZ_CS");
                    break;
                default:
                    break;
                }
                if (!displayText || !*displayText) { // Empty slot
                    std::sprintf(gString, "FEM_SL%d", i); // Como el original: sprintf (0x821BB5); sprintf_s rellena el resto con 0xFE en depuración
                    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
                    item.m_X = DEFAULT_CONTENT_X;
                    displayText = TheText.Get(gString);
                    xOffset = (int32)InlineScaleX(40.0f, 0x3D800000u);
                }
                break;
            }
            displayText = TheText.Get(item.m_szName);
            break;
        }
        }

        const auto GetOnOffText = [](bool condition) {
            return TheText.Get(condition ? "FEM_ON" : "FEM_OFF"); // ON : OFF
        };

        // 0x579DA3
        switch (item.m_nActionType) {
        case eMenuAction::MENU_ACTION_RADIO_STATION:           rightColumnText = AudioEngine.GetRadioStationName(m_nRadioStation); break;
        case eMenuAction::MENU_ACTION_FRAME_LIMITER:           rightColumnText = GetOnOffText(m_bPrefsFrameLimiter); break;
        case eMenuAction::MENU_ACTION_SUBTITLES:               rightColumnText = GetOnOffText(m_bShowSubtitles); break;
        case eMenuAction::MENU_ACTION_WIDESCREEN:              rightColumnText = GetOnOffText(m_bWidescreenOn); break;
        case eMenuAction::MENU_ACTION_RADIO_EQ:                rightColumnText = GetOnOffText(m_bRadioEq); break;
        case eMenuAction::MENU_ACTION_RADIO_RETUNE:            rightColumnText = GetOnOffText(m_bRadioAutoSelect); break;
        case eMenuAction::MENU_ACTION_SHOW_LEGEND:             rightColumnText = GetOnOffText(m_bMapLegend); break;
        case eMenuAction::MENU_ACTION_HUD_MODE:                rightColumnText = GetOnOffText(m_bHudOn); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_X:   rightColumnText = GetOnOffText(m_bInvertPadX1); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_Y:   rightColumnText = GetOnOffText(m_bInvertPadY1); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_X2:  rightColumnText = GetOnOffText(m_bInvertPadX2); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_INVERT_Y2:  rightColumnText = GetOnOffText(m_bInvertPadY2); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1: rightColumnText = GetOnOffText(m_bSwapPadAxis1); break;
        case eMenuAction::MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2: rightColumnText = GetOnOffText(m_bSwapPadAxis2); break;
        case eMenuAction::MENU_ACTION_USER_TRACKS_AUTO_SCAN:   rightColumnText = GetOnOffText(m_bTracksAutoScan); break;
        case eMenuAction::MENU_ACTION_STORE_PHOTOS:            rightColumnText = GetOnOffText(m_bSavePhotos); break;
        case eMenuAction::MENU_ACTION_RADAR_MODE:
            switch (m_nRadarMode) {
            case eRadarMode::RADAR_MODE_MAPS_AND_BLIPS: rightColumnText = TheText.Get("FED_RDM"); break; // MAP & BLIPS
            case eRadarMode::RADAR_MODE_BLIPS_ONLY:     rightColumnText = TheText.Get("FED_RDB"); break; // BLIPS ONLY
            case eRadarMode::RADAR_MODE_OFF:            rightColumnText = TheText.Get("FEM_OFF"); break; // OFF
            default:                                    break; // Como el original: otro valor no deja texto en la columna derecha
            }
            break;
        case eMenuAction::MENU_ACTION_LANGUAGE:
            switch (m_nPrefsLanguage) {
            case eLanguage::AMERICAN: rightColumnText = TheText.Get("FEL_ENG"); break; // English
            case eLanguage::FRENCH:   rightColumnText = TheText.Get("FEL_FRE"); break; // French
            case eLanguage::GERMAN:   rightColumnText = TheText.Get("FEL_GER"); break; // German
            case eLanguage::ITALIAN:  rightColumnText = TheText.Get("FEL_ITA"); break; // Italian
            case eLanguage::SPANISH:  rightColumnText = TheText.Get("FEL_SPA"); break; // Spanish
            default:                  break; // Como el original: otro valor no deja texto en la columna derecha
            }
            break;
        case eMenuAction::MENU_ACTION_FX_QUALITY:
            switch (g_fx.GetFxQuality()) {
            case FxQuality_e::FX_QUALITY_LOW:       rightColumnText = TheText.Get("FED_FXL"); break; // LOW
            case FxQuality_e::FX_QUALITY_MEDIUM:    rightColumnText = TheText.Get("FED_FXM"); break; // MEDIUM
            case FxQuality_e::FX_QUALITY_HIGH:      rightColumnText = TheText.Get("FED_FXH"); break; // HIGH
            case FxQuality_e::FX_QUALITY_VERY_HIGH: rightColumnText = TheText.Get("FED_FXV"); break; // VERY HIGH
            default:                                break; // Como el original: otro valor no deja texto en la columna derecha
            }
            break;
        case eMenuAction::MENU_ACTION_MIP_MAPPING:
            rightColumnText = GetOnOffText(m_bPrefsMipMapping);
            if (!m_bMainMenuSwitch) {
                CFont::SetColor({ 14, 30, 47, 255 });
            }
            break;
        case eMenuAction::MENU_ACTION_ANTIALIASING: {
            // Como el original: búfer de 100 bytes con CMemoryMgr::Calloc + rwsprintf, liberado al final
            auto* tmp = (RwChar*)CMemoryMgr::Calloc(100, 1, 0);
            if (m_nDisplayAntialiasing > 1) {
                rwsprintf(tmp, "%d", m_nDisplayAntialiasing - 1);
                AsciiToGxtChar(tmp, rightColumnBuffer);
                CMemoryMgr::Free(tmp);
                rightColumnText = rightColumnBuffer;
            } else {
                rightColumnText = TheText.Get("FEM_OFF");
                CMemoryMgr::Free(tmp);
            }
            break;
        }
        case eMenuAction::MENU_ACTION_CONTROLS_MOUSE_INVERT_Y:
            // Standard for all new games
            rightColumnText = GetOnOffText(!bInvertMouseY);
            break;
        case eMenuAction::MENU_ACTION_RESOLUTION: {
            // Como el original: se corta en el primer '(' quitando los espacios previos
            AsciiToGxtChar(GetVideoModeList()[m_nDisplayVideoMode], rightColumnBuffer);
            for (auto c = 0; rightColumnBuffer[c]; c++) {
                if (rightColumnBuffer[c] == '(') {
                    while (c > 0 && rightColumnBuffer[c - 1] == ' ') {
                        c--;
                    }
                    rightColumnBuffer[c] = 0;
                    break;
                }
            }
            rightColumnText = rightColumnBuffer;
            break;
        }
        case eMenuAction::MENU_ACTION_CONTROL_TYPE:
            // Como el original (0x57A141): solo 0 y 1 dan texto; otro valor no deja texto
            switch (m_ControlMethod) {
            case eController::MOUSE_PLUS_KEYS: rightColumnText = TheText.Get("FET_SCN"); break; // Mouse + Keys
            case eController::JOYPAD:          rightColumnText = TheText.Get("FET_CCN"); break; // Joypad
            default:                           break;
            }
            break;
        case eMenuAction::MENU_ACTION_MOUSE_STEERING:
            rightColumnText = GetOnOffText(CVehicle::m_bEnableMouseSteering);
            if (m_ControlMethod == eController::JOYPAD) {
                CFont::SetColor({ 14, 30, 47, 255 });
            }
            break;
        case eMenuAction::MENU_ACTION_MOUSE_FLY:
            rightColumnText = GetOnOffText(CVehicle::m_bEnableMouseFlying);
            if (m_ControlMethod == eController::JOYPAD) {
                CFont::SetColor({ 14, 30, 47, 255 });
            }
            break;
        case eMenuAction::MENU_ACTION_USER_TRACKS_PLAY_MODE:
            // Como el original (0x579E7A): solo 0, 1 y 2 dan texto (FEA_PR1/2/3)
            if (m_RadioMode >= 0 && m_RadioMode < RADIO_MODE_COUNT) {
                rightColumnText = TheText.Get(std::format("FEA_PR{}", m_RadioMode + 1).c_str());
            }
            break;
        default:
            break;
        }
        
        // Como el original: m_X/m_Y se leen como int16 (movsx)
        const auto scaledX = StretchX((float)(int16)item.m_X);
        const auto scaledY = StretchY((float)(int16)item.m_Y);

        // Como el original (1.0 US): se imprime siempre en (m_X, m_Y); el número de ranura es de la v1.01
        if (displayText) {
            CFont::PrintString(scaledX, scaledY, displayText);
        }

        // 0x57A204
        if (rightColumnText) {
            CFont::SetFontStyle(eFontStyle::FONT_MENU);
            CFont::SetEdge(1);
            CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
            // Como el original: escalas inline (0.35/640, 0.95/448, 0.56/640, 0.7/640, 1/448)
            if (isSlot) {
                CFont::SetScale(InlineScaleX(0.35f, 0x3A0F5C29u), InlineScaleY(0.95f, 0x3B0AF8AFu));
            } else if (m_nCurrentScreen == SCREEN_AUDIO_SETTINGS && i == 5) {
                CFont::SetScale(InlineScaleX(0.56f, 0x3A656042u), InlineScaleY(1.0f, 0x3B124925u));
            } else {
                CFont::SetScale(InlineScaleX(0.7f, 0x3A8F5C29u), InlineScaleY(1.0f, 0x3B124925u));
            }

            // Como el original: W - 40/640 inline
            CFont::PrintString(SCREEN_WIDTH - InlineScaleX(40.0f, 0x3D800000u), scaledY, rightColumnText);
        }

        // Check if text was properly initialized and we're on the current selection
        if ((displayText && i == m_nCurrentScreenItem) && (m_nCurrentScreen != SCREEN_MAP && m_nCurrentScreen != SCREEN_BRIEF)) {
            // Calculate X position for highlighted item if not already done
            // Como el original: X = (W * 1/640) * m_X sin atajo de 640, resultado truncado a entero
            if ((int16)xOffset == 0) {
                const float itemX = InlineStretchX((float)(int16)item.m_X);
                switch (item.m_nAlign) {
                case eMenuAlign::MENU_ALIGN_LEFT:
                    xOffset = (int32)(itemX - StretchX(40.0f));
                    break;
                case eMenuAlign::MENU_ALIGN_RIGHT: {
                    const float offset = StretchX(40.0f);
                    xOffset = (int32)(offset + itemX);
                    break;
                }
                default: {
                    const float left = itemX - StretchX(40.0f);
                    xOffset = (int32)(left - CFont::GetStringWidth(displayText, true, false) * 0.5f);
                    break;
                }
                }
            }

            // 0x57A50C - Draw highlight rectangle for selected item if map is loaded
            if (m_bMapLoaded) {
                switch (m_nCurrentScreen) {
                case eMenuScreen::SCREEN_LOAD_FIRST_SAVE:
                case eMenuScreen::SCREEN_DELETE_FINISHED:
                case eMenuScreen::SCREEN_SAVE_DONE_1:
                    break;
                default: {
                    // Como el original: Y = (H * 1/448) * m_Y sin atajo, y los márgenes verticales usan StretchX
                    const float left   = (float)(int16)xOffset;
                    const float itemY  = InlineStretchY((float)(int16)item.m_Y);
                    const float bottom = itemY - StretchX(5.0f);
                    const float right  = StretchX(32.0f) + left;
                    const float top    = StretchX(47.0f) + itemY;
                    m_aFrontEndSprites[0].Draw({ left, bottom, right, top }, CRGBA(255, 255, 255, 255)); // (left, bottom, right, top)
                    break;
                }
                }
            }
        }

        // 0x57A5F1 - Check for video mode changes
        // Como el original: seis comprobaciones independientes en este orden, comparando 8 bytes del nombre
        const auto currentItemName = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_szName;
        const auto IsCurrentItem   = [&](const char* key) { return !memcmp(currentItemName, key, 8); };
        const auto IsDisplayScreen = [&] { return m_nCurrentScreen == SCREEN_DISPLAY_SETTINGS || m_nCurrentScreen == SCREEN_DISPLAY_ADVANCED; };

        if (m_nDisplayVideoMode == m_nPrefsVideoMode && IsCurrentItem("FED_RES") && m_HelperText == eHelperText::FET_APP) { // RESOLUTION
            ResetHelperText();
        }
        if (m_nDisplayAntialiasing == m_nPrefsAntialiasing && IsCurrentItem("FED_AAS") && m_HelperText == eHelperText::FET_APP) { // ANTI ALIASING
            ResetHelperText();
        }
        if (m_nDisplayVideoMode != m_nPrefsVideoMode && IsCurrentItem("FED_RES")) {
            SetHelperText(eHelperText::FET_APP);
        }
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing && IsCurrentItem("FED_AAS")) {
            SetHelperText(eHelperText::FET_APP);
        }
        if (m_nDisplayAntialiasing != m_nPrefsAntialiasing && !IsCurrentItem("FED_AAS") && IsDisplayScreen()) {
            m_nDisplayAntialiasing = m_nPrefsAntialiasing;
            SetHelperText(eHelperText::FET_RSO);
        }
        if (m_nDisplayVideoMode != m_nPrefsVideoMode && !IsCurrentItem("FED_RES") && IsDisplayScreen()) {
            m_nDisplayVideoMode = m_nPrefsVideoMode;
            SetHelperText(eHelperText::FET_RSO);
        }

        // Como el original (0x57A7D4): las medidas de la barra van en línea con el atajo 640x448 y la constante
        // precalculada (valor/640 o valor/448); el resto de comprobaciones usan StretchX/StretchY (0x5733E0/0x573410).
        const auto processSlider = [&](float value, eMouseInBounds leftBound, eMouseInBounds rightBound, bool isRadioVolumeSlider = false, bool onlyCheckX = false) {
            const float top    = isRadioVolumeSlider ? 95.0f  : 125.0f;
            const float bottom = isRadioVolumeSlider ? 120.0f : 150.0f;
            const auto sliderPos = DisplaySlider(
                InlineScaleX(500.0f, 0x3F480000u),
                InlineScaleY(top, isRadioVolumeSlider ? 0x3E592492u : 0x3E8EDB6Eu),
                InlineScaleY(4.0f, 0x3C124925u),
                InlineScaleY(20.0f, 0x3D36DB6Eu),
                InlineScaleX(100.0f, 0x3E200000u),
                value,
                (int32)InlineScaleX(3.0f, 0x3B99999Au)
            );
            if (i == m_nCurrentScreenItem && shouldDrawStandardItems) {
                if (CheckHover(0, (int32)((float)sliderPos - StretchX(3.0f)), (int32)StretchY(top), (int32)StretchY(bottom))) {
                    m_MouseInBounds = leftBound;
                } else if (CheckHover((int32)(StretchX(3.0f) + (float)sliderPos), (int32)StretchX(SCREEN_WIDTH), (int32)StretchY(top), (int32)StretchY(bottom))) {
                    m_MouseInBounds = rightBound;
                    // Como el original (0x57A9B3 / 0x57AFF2): si falla alguna comprobación, vuelve a NONE
                    // (fcomp + test ah,41h/5: con NaN se conserva; el slider del ratón solo mira la X)
                    const float mouseX = (float)m_nMousePosX;
                    const float mouseY = (float)m_nMousePosY;
                    if (!(StretchX(500.0f) > mouseX)
                        && (onlyCheckX || (!(StretchY(top) > mouseY) && !(StretchY(bottom) < mouseY)))
                    ) {
                        return;
                    }
                    m_MouseInBounds = eMouseInBounds::NONE;
                } else {
                    m_MouseInBounds = eMouseInBounds::NONE;
                }
            }
        };

        // 0x57A7D4 - Handle sliders
        // Como el original: los valores son (int * const) o (resta * const), no divisiones (1/384 y 1/0.875 de .rdata)
        switch (item.m_nActionType) {
        case MENU_ACTION_BRIGHTNESS: processSlider((float)m_PrefsBrightness * std::bit_cast<float>(0x3B2AAAABu), eMouseInBounds::SLIDER_LEFT, eMouseInBounds::SLIDER_RIGHT); break;
        case MENU_ACTION_RADIO_VOL:  processSlider((float)m_nRadioVolume * 0.015625f, eMouseInBounds::RADIO_VOL_LEFT, eMouseInBounds::RADIO_VOL_RIGHT, true); break;
        case MENU_ACTION_SFX_VOL:    processSlider((float)m_nSfxVolume * 0.015625f, eMouseInBounds::SFX_VOL_LEFT, eMouseInBounds::SFX_VOL_RIGHT); break;
        case MENU_ACTION_DRAW_DIST:  processSlider((m_fDrawDistance - 0.925f) * std::bit_cast<float>(0x3F924925u), eMouseInBounds::DRAW_DIST_LEFT, eMouseInBounds::DRAW_DIST_RIGHT); break;
        case MENU_ACTION_MOUSE_SENS: processSlider(CCamera::m_fMouseAccelHorzntl / 0.005f, eMouseInBounds::MOUSE_SENS_LEFT, eMouseInBounds::MOUSE_SENS_RIGHT, false, true); break;
        default:                     break;
        }

        // 0x57B239
        if (displayText) {
            // Como el original (0x57B24B): el producto se convierte como sin signo (fild + 2^32 si es negativo)
            itemYPosition += (float)(uint32)(29 * (int32)CFont::GetNumberLines(60.0f, itemYPosition, displayText));
        }
        if (item.m_nActionType == MENU_ACTION_RADIO_STATION) {
            itemYPosition += 70;
        }
    }

    // 0x57B29F
    switch (m_nCurrentScreen) {
    case eMenuScreen::SCREEN_STATS:
    case eMenuScreen::SCREEN_AUDIO_SETTINGS:
    case eMenuScreen::SCREEN_DISPLAY_SETTINGS:
    case eMenuScreen::SCREEN_USER_TRACKS_OPTIONS:
    case eMenuScreen::SCREEN_DISPLAY_ADVANCED:
    case eMenuScreen::SCREEN_CONTROLLER_SETUP:
    case eMenuScreen::SCREEN_MOUSE_SETTINGS:
    case eMenuScreen::SCREEN_JOYPAD_SETTINGS:
        DisplayHelperText(nullptr);
        break;
    default:
        return;
    }
}

// 0x573EE0
void CMenuManager::DrawWindow(const CRect& coords, const char* key, uint8 color, CRGBA backColor, bool /*unused*/, bool background) {
    if (background) {
        CSprite2d::DrawRect(coords, backColor);
    }

    if (key && *key) {
        CFont::SetWrapx(coords.right);
        CFont::SetColor(CRGBA(MENU_TEXT_LIGHT_GRAY - color, MENU_TEXT_LIGHT_GRAY.a));
        CFont::SetDropColor(MENU_BG);
        CFont::SetEdge(2);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
        // Como el original: SetScaleForCurrentLanguage (0x7193A0) con escalado inline sin corrección de aspecto
        CFont::SetScaleForCurrentLanguage(SCREEN_WIDTH * 0.0015625f, InlineStretchY(1.4f));

        // Como el original: fcomp bottom/top; si no es "bottom < top" (incl. NaN) usa top
        const float minY = coords.bottom < coords.top ? coords.bottom : coords.top;
        const auto  text = TheText.Get(key);
        float y = minY - InlineStretchY(16.0f);
        float x = InlineStretchX(10.0f) + coords.left;
        CFont::PrintString(x, y, text);
    }
}

// 0x578F50
void CMenuManager::DrawWindowedText(float x, float y, float wrap, const char* title, const char* message, eFontAlignment alignment) {
    // Como el original: todo el escalado está inlineado con las constantes del EXE
    const float wrapX     = (x + wrap) - InlineStretchX(10.0f);
    const float rightWrap = InlineStretchX(10.0f) + (x - wrap);
    const float centreSize = wrap - (InlineStretchX(10.0f) + InlineStretchX(10.0f));
    // Como el original: escala de subtítulos con atajo 640x448 y constante precalculada 0.7/640 (0x865434)
    const auto  GetSubtitleScaleX = [] { return RsGlobal.maximumWidth == DEFAULT_SCREEN_WIDTH ? 0.7f : SCREEN_WIDTH * 0.00109375f; };
    const auto  GetSubtitleScaleY = [] { return RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? 1.0f : SCREEN_HEIGHT * 0.002232143f; };

    CRect textRect;
    CFont::SetWrapx(wrapX);
    CFont::SetRightJustifyWrap(rightWrap);
    CFont::SetCentreSize(centreSize);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetOrientation(alignment);
    {
        const float scaleY = GetSubtitleScaleY();
        const float scaleX = GetSubtitleScaleX();
        CFont::SetScale(scaleX, scaleY);
    }

    CFont::GetTextRect(&textRect, x, y, TheText.Get(message));
    textRect.left -= 4.0f;
    // Como el original: 22/448 precalculado (0x865430) con atajo para 448
    textRect.bottom += RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? 22.0f : SCREEN_HEIGHT * std::bit_cast<float>(0x3D492492u);
    // Como el original: el fondo es negro con alfa 190 (push 0BEh), no MENU_BG
    CSprite2d::DrawRect(textRect, CRGBA(0, 0, 0, 190));
    CFont::SetColor(MENU_TEXT_LIGHT_GRAY);
    CFont::SetDropColor(MENU_BG);
    CFont::SetEdge(2);
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
    CFont::SetScaleForCurrentLanguage(InlineStretchX(1.1f), InlineStretchY(1.4f));
    CFont::SetWrapx(textRect.right);

    if (title && *title) {
        const auto text = TheText.Get(title);
        CFont::PrintString(InlineStretchX(20.0f) + textRect.left, textRect.top - InlineStretchY(16.0f), text);
    }

    if (message && *message) {
        CFont::SetWrapx(wrapX);
        CFont::SetRightJustifyWrap(rightWrap);
        CFont::SetCentreSize(centreSize);
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetOrientation(alignment);
        {
            const float scaleY = GetSubtitleScaleY();
            const float scaleX = GetSubtitleScaleX();
            CFont::SetScale(scaleX, scaleY);
        }
        // Como el original: color HUD azul claro (0x58FEA0 con índice 3) antes de la sombra
        CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
        CFont::SetDropShadowPosition(2);
        CFont::SetDropColor(MENU_BG);
        const auto text = TheText.Get(message);
        CFont::PrintString(x, InlineStretchY(15.0f) + y, text);
    }
}

// 0x57D860
void CMenuManager::DrawQuitGameScreen() {
    m_DisplayTheMouse = false;
    CSprite2d::DrawRect({ 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT }, MENU_BG);
    SaveSettings();
    RsEventHandler(rsQUITAPP, nullptr);
}

// 0x57D8D0
void CMenuManager::DrawControllerScreenExtraText(int32 startingYPos) {
    const auto maxActions      = GetMaxAction();
    const auto verticalSpacing = GetVerticalSpacing();
    // Como el original: la X se guarda como entero (_ftol2 tras cada suma) y sobrevive al bucle
    // (el original la deja sin inicializar si no hay acciones)
    int32 textX = 0;
    if (maxActions > 0) {
        for (auto actionIndex = 0u; actionIndex < maxActions; actionIndex++) {
            textX = (int32)StretchX(240.0f);

            for (const auto& order : CONTROLLER_ORDERS_SET) {
                if (const auto buttonText = ControlsManager.GetControllerSettingTextWithOrderNumber(eControllerAction(actionIndex), order)) {
                    // Como el original: StretchY se llama en cada impresión
                    CFont::PrintString((float)textX, StretchY((float)startingYPos), buttonText);
                    textX = (int32)(StretchX(75.0f) + (float)textX);
                }
            }

            // Como el original: compara con m_nCurrentScreenItem (+0x54) y parpadea con m_InputWaitBlink/m_LastBlinkTime (+0x1B51/+0x1B54)
            if ((int32)actionIndex == m_nCurrentScreenItem && m_EditingControlOptions) {
                const auto now = CTimer::GetTimeInMSPauseMode();
                if (now - m_LastBlinkTime > 150) {
                    m_InputWaitBlink = !m_InputWaitBlink;
                    m_LastBlinkTime  = now;
                }
                if (m_InputWaitBlink) {
                    CFont::SetColor(MENU_BG);
                    const auto text = TheText.Get("FEC_QUE"); // ???
                    // Como el original: la Y no se escala aquí
                    CFont::PrintString((float)textX, (float)startingYPos, text);
                    CFont::SetColor(MENU_TEXT_NORMAL);
                }
            }

            startingYPos += verticalSpacing;
        }
    }

    // 0x57DA95
    // Como el original: fuera del bucle, con la última X y la Y acumulada + 10
    if (m_MenuIsAbleToQuit) {
        if (const auto comboText = ControlsManager.GetButtonComboText(eControllerAction(m_nCurrentScreenItem))) {
            CFont::SetColor(MENU_ERROR);
            CFont::PrintString((float)textX, StretchY((float)(startingYPos + 10)), comboText);
        }
    }
}

// 0x57E6E0
void CMenuManager::DrawControllerBound(uint16 verticalOffset, bool isOppositeScreen) {
    const auto verticalSpacing = GetVerticalSpacing();
    const auto maxActions      = GetMaxAction();

    using ControlActionMapping = std::pair<eControllerAction, int32>;

    static constexpr std::array<ControlActionMapping, 41> CarActionMappings = {{
        { PED_CYCLE_WEAPON_RIGHT,            -1 },
        { PED_CYCLE_WEAPON_LEFT,             -1 },
        { CAMERA_CHANGE_VIEW_ALL_SITUATIONS, -1 },
        { PED_JUMPING,                       -1 },
        { PED_SPRINT,                        -1 },
        { PED_LOOKBEHIND,                    -1 },
        { PED_DUCK,                          -1 },
        { VEHICLE_STEERDOWN,                 -1 },
        { VEHICLE_ACCELERATE,                -1 },
        { VEHICLE_RADIO_STATION_UP,          -1 },
        { VEHICLE_RADIO_STATION_DOWN,        -1 },
        { VEHICLE_RADIO_TRACK_SKIP,          -1 },
        { VEHICLE_HORN,                      -1 },
        { VEHICLE_TURRETLEFT,                -1 },
        { VEHICLE_TURRETRIGHT,               -1 },
        { PED_CYCLE_TARGET_LEFT,             -1 },
        { PED_FIRE_WEAPON,                   18 },
        { PED_FIRE_WEAPON_ALT,               19 },
        { GO_FORWARD,                        24 },
        { GO_BACK,                           25 },
        { GO_LEFT,                           20 },
        { GO_RIGHT,                          21 },
        { PED_SNIPER_ZOOM_IN,                22 },
        { PED_SNIPER_ZOOM_OUT,               23 },
        { VEHICLE_ENTER_EXIT,                47 },
        { PED_ANSWER_PHONE,                  10 },
        { PED_WALK,                          26 },
        { VEHICLE_FIRE_WEAPON,               27 },
        { VEHICLE_FIRE_WEAPON_ALT,           28 },
        { VEHICLE_STEERLEFT,                 29 },
        { VEHICLE_STEERRIGHT,                30 },
        { VEHICLE_STEERUP,                   11 },
        { VEHICLE_BRAKE,                     31 },
        { TOGGLE_SUBMISSIONS,                38 },
        { VEHICLE_HANDBRAKE,                 39 },
        { PED_1RST_PERSON_LOOK_LEFT,         41 },
        { PED_1RST_PERSON_LOOK_RIGHT,        40 },
        { VEHICLE_LOOKLEFT,                  36 },
        { VEHICLE_LOOKRIGHT,                 37 },
        { VEHICLE_LOOKBEHIND,                34 },
        { VEHICLE_MOUSELOOK,                 35 },
    }};

    static constexpr std::array<ControlActionMapping, 51> PedActionMappings = {{
        { PED_FIRE_WEAPON,                   0  },
        { VEHICLE_RADIO_TRACK_SKIP,          0  },
        { PED_FIRE_WEAPON_ALT,               2  },
        { PED_CYCLE_WEAPON_RIGHT,            3  },
        { PED_CYCLE_WEAPON_LEFT,             49 },
        { GO_FORWARD,                        50 },
        { GO_BACK,                           48 },
        { GO_LEFT,                           47 },
        { VEHICLE_MOUSELOOK,                 47 },
        { GO_RIGHT,                          4  },
        { TOGGLE_SUBMISSIONS,                4  },
        { PED_SNIPER_ZOOM_IN,                5  },
        { VEHICLE_HANDBRAKE,                 5  },
        { PED_SNIPER_ZOOM_OUT,               6  },
        { PED_1RST_PERSON_LOOK_LEFT,         6  },
        { VEHICLE_ENTER_EXIT,                7  },
        { PED_1RST_PERSON_LOOK_RIGHT,        7  },
        { CAMERA_CHANGE_VIEW_ALL_SITUATIONS, 8  },
        { PED_JUMPING,                       9  },
        { PED_SPRINT,                        10 },
        { VEHICLE_LOOKBEHIND,                10 },
        { PED_LOOKBEHIND,                    11 },
        { PED_CYCLE_TARGET_RIGHT,            11 },
        { PED_DUCK,                          12 },
        { PED_ANSWER_PHONE,                  13 },
        { PED_WALK,                          45 },
        { VEHICLE_FIRE_WEAPON,               15 },
        { VEHICLE_FIRE_WEAPON_ALT,           16 },
        { VEHICLE_STEERUP,                   32 },
        { CONVERSATION_YES,                  32 },
        { VEHICLE_STEERDOWN,                 33 },
        { CONVERSATION_NO,                   33 },
        { VEHICLE_TURRETLEFT,                -1 },
        { VEHICLE_TURRETRIGHT,               -1 },
        { VEHICLE_TURRETUP,                  -1 },
        { VEHICLE_TURRETDOWN,                -1 },
        { PED_CYCLE_TARGET_LEFT,             -1 },
        { PED_CENTER_CAMERA_BEHIND_PLAYER,   -1 },
        { NETWORK_TALK,                      -1 },
        { GROUP_CONTROL_FWD,                 -1 },
        { GROUP_CONTROL_BWD,                 -1 },
        { PED_1RST_PERSON_LOOK_UP,           -1 },
        { PED_1RST_PERSON_LOOK_DOWN,         -1 },
        { VEHICLE_RADIO_STATION_DOWN,        1  },
        { VEHICLE_HORN,                      1  },
        { VEHICLE_RADIO_STATION_UP,          44 },
        { VEHICLE_BRAKE,                     52 },
        { VEHICLE_ACCELERATE,                51 },
        { VEHICLE_STEERLEFT,                 17 },
        { VEHICLE_STEERRIGHT,                14 },
        { PED_LOCK_TARGET,                   14 },
    }};

    // Main loop - process each action
    // Como el original: X e Y se guardan como enteros (_ftol2); la Y se recalcula al final de cada vuelta
    int32 currentY = (int32)StretchY((float)verticalOffset);
    for (auto actionIndex = 0u; actionIndex < maxActions; actionIndex++) {
        int32 currentX = (int32)StretchX(270.0f);
        eControllerAction action = eControllerAction::CA_NONE;
        CFont::SetColor(MENU_TEXT_WHITE);

        // Map action index to controller action
        if (m_RedefiningControls == eControlMode::VEHICLE) {
            for (const auto& mapping : CarActionMappings) {
                if (mapping.first == ControllerActionsAvailableInCar[actionIndex]) {
                    action = (eControllerAction)mapping.second;
                    break;
                }
            }
        } else if (m_RedefiningControls == eControlMode::FOOT) {
            for (const auto& mapping : PedActionMappings) {
                if (mapping.first == (eControllerAction)actionIndex) { // Cast actionIndex to eControllerAction for comparison
                    if (m_ControlMethod == eController::MOUSE_PLUS_KEYS && notsa::contains({ VEHICLE_STEERUP, CONVERSATION_YES, VEHICLE_STEERDOWN, CONVERSATION_NO }, mapping.first)) {
                        action = eControllerAction::CA_NONE;
                    } else {
                        action = (eControllerAction)mapping.second;
                    }
                    break;
                }
            }
        }

        // 0x57EA1E - Highlight selected action
        if (m_ListSelection == (int32)actionIndex && !isOppositeScreen) {
            // Como el original: Y en 16 bits con signo a partir de m_ListSelection, y orden de llamadas Y2, X(20), Y1, X(260)
            const int32 rowTop    = (int16)(verticalSpacing * (int16)m_ListSelection + verticalOffset + 1);
            const float rowY2     = StretchY((float)(rowTop + 10));
            const float rightX    = SCREEN_WIDTH - StretchX(20.0f);
            const float rowY1     = StretchY((float)rowTop);
            const float leftX     = StretchX(260.0f);
            CSprite2d::DrawRect({ leftX, rowY1, rightX, rowY2 }, MENU_TEXT_SELECTED); // (left, bottom, right, top)
            CFont::SetColor(MENU_TEXT_WHITE);
        }

        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        {
            const float scaleY = StretchY(0.6f);
            const float scaleX = StretchX(0.3f);
            CFont::SetScale(scaleX, scaleY);
        }
        CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);

        // Draw control bindings
        bool hasControl = false;
        if (action != eControllerAction::CA_NONE) {
            for (const auto& order : CONTROLLER_ORDERS_SET) {
                if (m_DeleteAllNextDefine && m_ListSelection == (int32)actionIndex) {
                    break;
                }
                if (auto buttonText = ControlsManager.GetControllerSettingTextWithOrderNumber(action, order)) {
                    hasControl = true;
                    if (!isOppositeScreen) {
                        CFont::PrintString((float)currentX, (float)currentY, buttonText);
                    }
                    currentX = (int32)(StretchX(75.0f) + (float)currentX);
                }
            }
        }

        // 0x57EBD9 + 0x57EBEA - Handle special cases and selection
        if (action == eControllerAction::COMBOLOCK) {
            // 0x57EBEA
            CFont::SetColor(MENU_BG);
            if (!isOppositeScreen) {
                CFont::PrintString((float)currentX, (float)currentY, TheText.Get("FEC_CMP")); // COMBO: Uses LOOK LEFT + LOOK RIGHT together
            }
        }

        // 0x57EB63 + 0x57EBD9 + 0x57EC13 - Handle unbound case
        if (action != eControllerAction::CA_NONE && action != eControllerAction::COMBOLOCK && !hasControl) {
            // 0x57EC1F
            m_RadioAvailable = false;
            if (m_ListSelection != (int32)actionIndex || !m_EditingControlOptions) {
                CFont::SetColor(MENU_ERROR);
                if (!isOppositeScreen) {
                    CFont::PrintString((float)currentX, (float)currentY, TheText.Get("FEC_UNB")); // UNBOUND
                }
            }
        }

        // 0x57ECA2 - Handle selection and state
        if ((int32)actionIndex == m_ListSelection) {
            if (action == eControllerAction::CA_NONE || action == eControllerAction::COMBOLOCK) {
                // CANNOT SET A CONTROL FOR THIS ACTION
                DisplayHelperText("FET_EIG");
            } else {
                m_OptionToChange = (int32)action;
                if (m_EditingControlOptions) {
                    // Como el original: aquí parpadea con m_OptionFlashColorState/m_LastHighlightToggleTime (+0x1B5C/+0x1B60)
                    const auto now = CTimer::GetTimeInMSPauseMode();
                    if (now - m_LastHighlightToggleTime > 150) {
                        m_OptionFlashColorState = !m_OptionFlashColorState;
                        m_LastHighlightToggleTime = now;
                    }
                    if (m_OptionFlashColorState) {
                        CFont::SetColor(MENU_BG);
                        // Como el original: el texto se pide aunque sea la pantalla opuesta
                        const auto text = TheText.Get("FEC_QUE"); // ???
                        if (!isOppositeScreen) {
                            CFont::PrintString((float)currentX, (float)currentY, text);
                        }
                    }
                    if (m_DeleteAllBoundControls) {
                        // BACKSPACE - CLEAR
                        // CLICK LMB / RETURN - CHANGE
                        DisplayHelperText("FET_CIG");
                    } else {
                        // SELECT A NEW CONTROL FOR THIS ACTION
                        // ESC - CANCEL
                        DisplayHelperText("FET_RIG");
                    }
                    m_CanBeDefined = true;
                } else {
                    // BACKSPACE - CLEAR
                    // CLICK LMB / RETURN - CHANGE
                    DisplayHelperText("FET_CIG");
                    m_CanBeDefined = false;
                    m_DeleteAllBoundControls = false;
                }
            }
        }

        currentY = (int32)StretchY((float)((int32)verticalSpacing * (int32)(actionIndex + 1) + (int32)verticalOffset));
    }
}

// 0x57F300
void CMenuManager::DrawControllerSetupScreen() {
    const auto verticalSpacing = GetVerticalSpacing();
    const auto maxActions      = GetMaxAction();
    // Como el original: mismas ranuras, pero las llamadas a TheText.Get siguen el orden del EXE
    // (la ranura 10 se pide justo tras la 5 y FEC_LUD antes que FEC_LDU; 40, 41 y 43 quedan a nullptr)
    std::array<const GxtChar*, 44> keys{};
    keys[0]  = TheText.Get("FEC_FIR");                                                              // Fire
    keys[1]  = TheText.Get("FEC_FIA");                                                              // Secondary Fire
    keys[2]  = TheText.Get("FEC_NWE");                                                              // Next weapon/target
    keys[3]  = TheText.Get("FEC_PWE");                                                              // Previous weapon/target
    keys[4]  = TheText.Get(m_RedefiningControls == eControlMode::VEHICLE ? "FEC_ACC" : "FEC_FOR");  // Accelerate : Forward
    keys[5]  = TheText.Get(m_RedefiningControls == eControlMode::VEHICLE ? "FEC_BRA" : "FEC_BAC");  // Brake/Reverse : Backwards
    keys[10] = TheText.Get(m_RedefiningControls == eControlMode::VEHICLE ? "FEC_TSK" : "FEC_COY");  // Trip Skip : Conversation - Yes
    keys[6]  = TheText.Get("FEC_LEF");                                                              // Left
    keys[7]  = TheText.Get("FEC_RIG");                                                              // Right
    keys[8]  = TheText.Get("FEC_PLU");                                                              // Steer Forward/Down
    keys[9]  = TheText.Get("FEC_PLD");                                                              // Steer Back/Up
    keys[11] = TheText.Get("FEC_CON");                                                              // Conversation - No
    keys[12] = TheText.Get("FEC_GPF");                                                              // Group Ctrl Forward
    keys[13] = TheText.Get("FEC_GPB");                                                              // Group Ctrl Back
    keys[14] = TheText.Get("FEC_ZIN");                                                              // Zoom in
    keys[15] = TheText.Get("FEC_ZOT");                                                              // Zoom out
    keys[16] = TheText.Get("FEC_EEX");                                                              // Enter+exit
    keys[17] = TheText.Get("FEC_RSC");                                                              // Next radio station
    keys[18] = TheText.Get("FEC_RSP");                                                              // Previous radio station
    keys[19] = TheText.Get("FEC_RTS");                                                              // User track skip
    keys[20] = TheText.Get("FEC_HRN");                                                              // Horn
    keys[21] = TheText.Get("FEC_SUB");                                                              // Sub-mission
    keys[22] = TheText.Get("FEC_CMR");                                                              // Change camera
    keys[23] = TheText.Get("FEC_JMP");                                                              // Jump
    keys[24] = TheText.Get("FEC_SPN");                                                              // Sprint
    keys[25] = TheText.Get("FEC_HND");                                                              // Handbrake
    keys[26] = TheText.Get("FEC_TAR");                                                              // Aim Weapon
    keys[27] = TheText.Get("FEC_CRO");                                                              // Crouch
    keys[28] = TheText.Get("FEC_ANS");                                                              // Action
    keys[29] = TheText.Get("FEC_PDW");                                                              // Walk
    keys[30] = TheText.Get("FEC_TFL");                                                              // Special Ctrl Left
    keys[31] = TheText.Get("FEC_TFR");                                                              // Special Ctrl Right
    keys[32] = TheText.Get("FEC_TFU");                                                              // Special Ctrl Up
    keys[33] = TheText.Get("FEC_TFD");                                                              // Special Ctrl Down
    keys[34] = TheText.Get("FEC_LBA");                                                              // Look behind
    keys[35] = TheText.Get("FEC_VML");                                                              // Mouse Look
    keys[36] = TheText.Get("FEC_LOL");                                                              // Look left
    keys[37] = TheText.Get("FEC_LOR");                                                              // Look right
    keys[39] = TheText.Get("FEC_LUD");                                                              // Look Up
    keys[38] = TheText.Get("FEC_LDU");                                                              // Look Down
    keys[42] = TheText.Get("FEC_CEN");                                                              // Center camera

    // 0x57F68E
    CFont::SetFontStyle(eFontStyle::FONT_GOTHIC);
    CFont::SetScale(StretchX(0.9f), StretchY(1.7f));
    CFont::SetEdge(0);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_BLUE));
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    // Como el original: W - StretchX(48) (0x5733E0), no SCREEN_STRETCH_FROM_RIGHT
    CFont::PrintString(SCREEN_WIDTH - StretchX(48.0f), StretchY(11.0f),
        TheText.Get(m_ControlMethod == eController::MOUSE_PLUS_KEYS
            ? "FET_SCN" // Mouse + Keys
            : "FET_CCN" // Joypad
        )
    );
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    // Como el original: con otro modo de redefinición no imprime el título
    switch (m_RedefiningControls) {
    case eControlMode::VEHICLE:
        CFont::PrintString(StretchX(48.0f), StretchY(11.0f), TheText.Get("FET_CCR")); // Vehicle Controls
        break;
    case eControlMode::FOOT:
        CFont::PrintString(StretchX(48.0f), StretchY(11.0f), TheText.Get("FET_CFT")); // Foot Controls
        break;
    default:
        break;
    }
    {
        // Como el original: H - StretchY(50) y W - StretchX(20), en el orden de llamadas del EXE
        const float top    = SCREEN_HEIGHT - StretchY(50.0f);
        const float right  = SCREEN_WIDTH - StretchX(20.0f);
        const float bottom = StretchY(50.0f);
        const float left   = StretchX(20.0f);
        CSprite2d::DrawRect({ left, bottom, right, top }, { 49, 101, 148, 100 }); // (left, bottom, right, top)
    }

    // 0x57F8C0
    for (auto i = 0u; i < maxActions; i++) {
        if (!m_EditingControlOptions) {
            if (m_nMousePosX > StretchX(20.0f)
                && m_nMousePosX < StretchX(600.0f)
                && m_nMousePosY > StretchY(float(i * verticalSpacing + 69))
                && m_nMousePosY < StretchY(float(verticalSpacing * (i + 1) + 69))) {
                m_CurrentMouseOption = i;
                if (m_nOldMousePosX != m_nMousePosX || m_nOldMousePosY != m_nMousePosY) {
                    m_ListSelection = i;
                }
                // Como el original: con SELECT sobre otra fila no se toca m_MouseInBounds
                if (m_MouseInBounds == eMouseInBounds::SELECT) {
                    if ((int32)i == m_ListSelection) {
                        m_EditingControlOptions = true;
                        m_bJustOpenedControlRedefWindow = true;
                        m_pPressedKey = &m_KeyPressedCode;
                        m_MouseInBounds = eMouseInBounds::NONE;
                    }
                } else {
                    m_MouseInBounds = eMouseInBounds::NONE;
                }
            }
        }

        CFont::SetColor(MENU_TEXT_NORMAL);
        CFont::SetScale(StretchX(0.4f), StretchY(0.6f));
        CFont::SetFontStyle(eFontStyle::FONT_MENU);
        CFont::SetWrapx(StretchX(100.0f) + SCREEN_WIDTH);
        // Como el original: solo imprime en modo a pie o vehículo
        const GxtChar* actionText = nullptr;
        switch (m_RedefiningControls) {
        case eControlMode::VEHICLE: actionText = keys[+ControllerActionsAvailableInCar[i]]; break;
        case eControlMode::FOOT:    actionText = keys[+ControllerActionsAvailableOnFoot[i]]; break;
        default:                    break;
        }

        if (actionText) {
            CFont::PrintString(StretchX(40.0f), StretchY(float(i * verticalSpacing + 69)), actionText);
        }
    }

    // 0x57FAF9
    DrawControllerBound(69, false);
    if (!m_EditingControlOptions) {
        CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
        // Como el original: el ancho se calcula primero y las Y salen de H - StretchY(n)
        const float textWidth = StretchX(CFont::GetStringWidth(TheText.Get("FEDS_TB"), true, false));
        if (m_nMousePosX < StretchX(35.0f) + textWidth
            && m_nMousePosX > StretchX(15.0f)
            && m_nMousePosY > SCREEN_HEIGHT - StretchY(33.0f)
            && m_nMousePosY < SCREEN_HEIGHT - StretchY(10.0f)) {
            m_MouseInBounds = eMouseInBounds::BACK_BUTTON;
        } else if (m_nMousePosX > StretchX(20.0f)
            && m_nMousePosX < StretchX(600.0f)
            && m_nMousePosY > StretchY(48.0f)
            && m_nMousePosY < SCREEN_HEIGHT - StretchY(33.0f)) {
            m_MouseInBounds = eMouseInBounds::ENTER_MENU;
        } else {
            m_MouseInBounds = eMouseInBounds::NONE;
        }
    }

    // 0x57FCC4
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetScale(StretchX(0.7f), StretchY(1.0f));
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetEdge(0);
    CFont::SetColor(MENU_TEXT_NORMAL);
    // Como el original: el texto se vuelve a pedir aquí
    const auto backText = TheText.Get("FEDS_TB"); // Back
    CFont::PrintString(StretchX(33.0f), SCREEN_HEIGHT - StretchY(38.0f), backText);
}

/**
 * Draws slider
 * @param x       widget pos x
 * @param y       widget pos y
 * @param barHeight1      height of start strip
 * @param barHeight2      height of end strip
 * @param length  widget length
 * @param value   current value in range [ 0.0f, 1.0f ] (progress)
 * @param barSpacing size of strip
 * @return
 * @see Audio Setup -> Radio or SFX volume
 * @addr 0x576860
 */
int32 CMenuManager::DisplaySlider(float x, float y, float barHeight1, float barHeight2, float length, float value, int32 barSpacing) {
    constexpr auto NUM_BARS               = 16;
    constexpr auto BAR_WIDTH_FACTOR       = 1.0f / NUM_BARS;         // 0.0625f
    constexpr auto VALUE_THRESHOLD_OFFSET = BAR_WIDTH_FACTOR / 2.0f; // 0.03125f
    
    // Como el original: fcomp + test ah,41h -> h1 solo si h1 > h2 (NaN elige h2)
    float maxBarHeight = barHeight1 > barHeight2 ? barHeight1 : barHeight2;
    auto lastActiveBarX = 0;

    for (auto i = 0; i < NUM_BARS; i++) {
        float barIndex     = float(i);
        float currentBarX  = x + barIndex * length * BAR_WIDTH_FACTOR;
        float barFreeSpace = (barHeight1 * (NUM_BARS - barIndex) + barHeight2 * barIndex) * BAR_WIDTH_FACTOR;

        // Set bar color based on value threshold
        CRGBA barColor = barIndex * BAR_WIDTH_FACTOR + VALUE_THRESHOLD_OFFSET < value
            ? MENU_TEXT_SELECTED
            : MENU_TEXT_NORMAL;
        if (barColor == MENU_TEXT_SELECTED) {
            lastActiveBarX = (int32)currentBarX;
        }

        // Como el original: los rectángulos se escriben campo a campo (sin el constructor de CRect, que aborta si queda invertido por redondeo)
        CRect barRect;
        barRect.left   = currentBarX;
        barRect.bottom = y + maxBarHeight - barFreeSpace;
        barRect.right  = currentBarX + barSpacing;
        barRect.top    = y + maxBarHeight;

        // Draw shadow rectangle with resolution scaling
        // Como el original: desplazamiento inline con atajo 640x448 y constantes 2/640 (0x865394), 2/448 (0x865398);
        // y el orden de las sumas es (bs + sx) + x y (max + sy) + y
        const float shadowX = RsGlobal.maximumWidth  == DEFAULT_SCREEN_WIDTH  ? 2.0f : SCREEN_WIDTH  * std::bit_cast<float>(0x3B4CCCCDu);
        const float shadowY = RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? 2.0f : SCREEN_HEIGHT * std::bit_cast<float>(0x3B924925u);
        CRect shadowRect;
        shadowRect.left   = currentBarX + shadowX;
        shadowRect.bottom = barRect.bottom + shadowY;
        shadowRect.right  = ((float)barSpacing + shadowX) + currentBarX;
        shadowRect.top    = (maxBarHeight + shadowY) + y;
        CSprite2d::DrawRect(shadowRect, MENU_SHADOW);

        // Draw foreground rectangle
        CSprite2d::DrawRect(barRect, barColor);
    }
    return lastActiveBarX;
}

// NOTSA
// Based on 0x57D914 - 0x57D933
uint32 CMenuManager::GetMaxAction() {
    // TODO: Magis values
    switch (m_RedefiningControls) {
    case eControlMode::FOOT:
        switch (m_ControlMethod) {
        case eController::MOUSE_PLUS_KEYS:
            return 22;
        case eController::JOYPAD:
            return 28;
        default:
            NOTSA_UNREACHABLE();
        }
    case eControlMode::VEHICLE:
        return 25;
    default:
        NOTSA_UNREACHABLE();
    }
}

// NOTSA
// Based on 0x57D8F1 - 0x57D905
uint32 CMenuManager::GetVerticalSpacing() {
    // TODO: Magis values
    switch (m_RedefiningControls) {
    case eControlMode::FOOT:
        switch (m_ControlMethod) {
        case eController::MOUSE_PLUS_KEYS:
            return 15; // 4 * 1 + 11
        case eController::JOYPAD:
            return 11; // 4 * 0 + 11
        default:
            NOTSA_UNREACHABLE();
        }
    case eControlMode::VEHICLE:
        return 13;
    default:
        NOTSA_UNREACHABLE();
    }
}
