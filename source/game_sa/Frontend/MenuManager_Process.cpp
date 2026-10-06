#include "StdInc.h"

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "AudioEngine.h"
#include "Pad.h"
#include "Cheat.h"
#include "GenericGameStorage.h"
#include "Camera.h"
#include "Replay.h"
#include "MenuSystem.h"
#include "PostEffects.h"
#include "VideoMode.h"
#include "C_PcSave.h"
#include "Radar.h"
#include "ControllerConfigManager.h"

// 0x57B440
void CMenuManager::Process() {
    ZoneScoped;

    if (m_bMenuActive) {
        ProcessStreaming(m_bAllStreamingStuffLoaded);
        UserInput();
        ProcessFileActions();
        D3DResourceSystem::TidyUpD3DIndexBuffers(1);
        D3DResourceSystem::TidyUpD3DTextures(1);
    }
    CheckForMenuClosing();
}

// 0x573CF0
void CMenuManager::ProcessStreaming(bool streamAll) {
    // Exit early if menu is inactive or not on the map screen
    if (!m_bMenuActive || m_nCurrentScreen != SCREEN_MAP) {
        return;
    }

    // Exit if map isn't loaded or streaming is disabled
    if (!m_bMapLoaded || m_bStreamingDisabled) {
        return;
    }

    // Como el original: un único tamaño de tesela = zoom * (1/6) (fmul [0x85F0A0]),
    // no (zoom * 2) / 12, que redondea distinto.
    const float tileSize = m_fMapZoom * std::bit_cast<float>(0x3E2AAAABu); // 0.16666667f

    // Define map boundaries
    const float mapAreaX = m_vMapOrigin.x - m_fMapZoom;
    const float mapAreaY = m_vMapOrigin.y - m_fMapZoom;

    // Margen de la pantalla (mismo para X e Y en el original)
    const float padding = tileSize * 3.0f;
    const float minEdge = 60.0f - padding;

    // Iterate over map tiles to determine which sections to load or remove
    for (auto x = 0; x < (int32)MAX_RADAR_WIDTH_TILES; x++) {
        const float tileLeft = (float)x * tileSize + mapAreaX;
        for (auto y = 0; y < (int32)MAX_RADAR_HEIGHT_TILES; y++) {
            // Como el original: la tesela debe quedar estrictamente dentro del área visible
            // (no basta con solaparse) y el borde derecho/inferior se calcula como (i + 1) * tamaño + origen.
            if (tileLeft > minEdge
                && (float)y * tileSize + mapAreaY > minEdge
                && padding + 580.0f > (float)(x + 1) * tileSize + mapAreaX
                && padding + 388.0f > (float)(y + 1) * tileSize + mapAreaY
            ) {
                CRadar::RequestMapSection(x, y);
            } else {
                CRadar::RemoveMapSection(x, y);
            }
        }
    }

    // todo: rename streamAll and m_bAllStreamingStuffLoaded
    if (streamAll) {
        CStreaming::LoadAllRequestedModels(false);
        m_bAllStreamingStuffLoaded = false;
    } else {
        CStreaming::LoadRequestedModels();
    }
}

// 0x578D60
void CMenuManager::ProcessFileActions() {
    switch (m_nCurrentScreen) {
    case SCREEN_LOAD_FIRST_SAVE:
        if (m_CurrentlyLoading) {
            if (CGenericGameStorage::CheckSlotDataValid((int8)m_SelectedSlot)) { // Como el original: movsx de la ranura (0x578EEA)
                // Prepare for game start if not switching from main menu
                if (!m_bMainMenuSwitch) {
                    DoSettingsBeforeStartingAGame();
                }

                m_bDontDrawFrontEnd = true;
                CGame::bMissionPackGame = false;
                m_bLoadingData = true;
            } else {
                // Load Game
                //
                // Load Failed! There was an error while loading the current game. Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_SAVED, "FET_LG", "FES_LCE");
            }
            m_CurrentlyLoading = false;
        } else {
            m_CurrentlyLoading = true;
        }
        break;

    case SCREEN_DELETE_FINISHED:
        if (m_CurrentlyDeleting) {
            if (s_PcSaveHelper.DeleteSlot(m_SelectedSlot)) {
                // Update slot info and switch to success screen
                s_PcSaveHelper.PopulateSlotInfo();
                SwitchToNewScreen(SCREEN_DELETE_SUCCESSFUL);
                m_nCurrentScreenItem = true;
            } else {
                // Delete Game
                //
                // Deleting Failed! There was an error while deleting the current game. Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_SAVED, "FES_DEL", "FES_DEE");
            }
            m_CurrentlyDeleting = false;
        } else {
            m_CurrentlyDeleting = true;
        }
        break;

    case SCREEN_SAVE_DONE_1:
        if (m_CurrentlySaving) {
            if (CGame::bMissionPackGame) {
                // Check mission pack file availability
                CFileMgr::SetDirMyDocuments();
                sprintf(gString, "MPACK//MPACK%d//SCR.SCM", CGame::bMissionPackGame);
                auto file = CFileMgr::OpenFile(gString, "rb");
                CFileMgr::SetDir(""); // FIX_BUGS

                if (!file) {
                    // Save Game
                    //
                    // Save failed! The current Mission Pack is not available. Please recheck that the current Mission Pack is installed correctly.
                    JumpToGenericMessageScreen(SCREEN_GAME_LOADED, "FET_SG", "FES_NIM");
                    m_CurrentlySaving = false; // Como el original: también se reinicia en este camino (0x578E5E)
                    return;
                } else {
                    CFileMgr::CloseFile(file);
                }
            }

            // Como el original: SaveSlot y PopulateSlotInfo van antes de cambiar de pantalla (0x578DFA, 0x578E08)
            const auto saveFailed = (uint8)s_PcSaveHelper.SaveSlot(m_SelectedSlot); // Como el original: solo se comprueba AL (0x578E0D)
            s_PcSaveHelper.PopulateSlotInfo();

            if (saveFailed) {
                // Save Game
                //
                // Save failed! There was an error while saving the current game. Please check your savegame directory and try again.
                JumpToGenericMessageScreen(SCREEN_GAME_LOADED, "FET_SG", "FES_CMP");
            } else {
                // Save Game
                //
                // Save Successful. Select OK to continue.
                SwitchToNewScreen(SCREEN_SAVE_DONE_2);
            }

            m_CurrentlySaving = false;
        } else {
            m_CurrentlySaving = true;
        }
        break;

    default:
        return;
    }
}

// 0x576FE0
// @param pressedLR Arrow button pressed. <0 for left, >0 for right
// @param cancelPressed Returns true to go back.
// @param acceptPressed Is enter pressed. Used for AA mode and resolution
void CMenuManager::ProcessMenuOptions(int8 pressedLR, bool& cancelPressed, bool acceptPressed) {
    // Handle PC-specific options first
    if (ProcessPCMenuOptions(pressedLR, acceptPressed)) {
        return;
    }

    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_BACK:
        cancelPressed = true;
        return;
    case MENU_ACTION_YES:
    case MENU_ACTION_NO:
    case MENU_ACTION_MENU:
        SwitchToNewScreen(item->m_nTargetMenu);
        return;
    case MENU_ACTION_NEW_GAME:
        ProcessMissionPackNewGame();
        return;
    case MENU_ACTION_MPACK: {
        // Como el original (0x57724F): sin comprobar el rango; lee el Id de m_MissionPacks[opción - 2]
        // (la copia del array y la aserción de rango no existen en el EXE)
        m_nMissionPackGameId = (&m_MissionPacks[0] + (m_nCurrentScreenItem - 2))->m_Id;
        SwitchToNewScreen(SCREEN_MISSION_PACK_LOADING_ASK);
        return;
    }
    case MENU_ACTION_MPACKGAME:
        CGame::bMissionPackGame = m_nMissionPackGameId;
        DoSettingsBeforeStartingAGame();
        return;
    case MENU_ACTION_SAVE_SLOT: {
        if (item->m_nType >= eMenuEntryType::TI_SLOT1 && item->m_nType <= eMenuEntryType::TI_SLOT8) {
            m_SelectedSlot = (uint8)(m_nCurrentScreenItem - 1);
            const auto slotState = CGenericGameStorage::ms_Slots.data()[(int8)m_SelectedSlot]; // Como el original: sin comprobar el rango

            // Como el original: solo actúa en las pantallas de cargar/borrar (0x57729B)
            switch (m_nCurrentScreen) {
            case SCREEN_LOAD_GAME:
                if (slotState != eSlotState::SLOT_FREE && slotState != eSlotState::SLOT_CORRUPTED) {
                    SwitchToNewScreen(SCREEN_LOAD_GAME_ASK);
                }
                break;
            case SCREEN_DELETE_GAME:
                if (slotState != eSlotState::SLOT_FREE) {
                    SwitchToNewScreen(SCREEN_DELETE_GAME_ASK);
                }
                break;
            default:
                break;
            }
        }
        return;
    }
    case MENU_ACTION_STANDARD_GAME:
        CGame::bMissionPackGame = 0;
        DoSettingsBeforeStartingAGame();
        m_bDontDrawFrontEnd = true;
        return;
    case MENU_ACTION_15:
        m_bDontDrawFrontEnd = true;
        return;
    case MENU_ACTION_SAVE_GAME: {
        if (item->m_nType >= eMenuEntryType::TI_SLOT1 && item->m_nType <= eMenuEntryType::TI_SLOT8) {
            m_SelectedSlot = m_nCurrentScreenItem - 1;
            SwitchToNewScreen(SCREEN_SAVE_WRITE_ASK);
        }
        return;
    }
    case MENU_ACTION_STAT: {
        // Como el original (0x577108): dirección 1 si pressedLR == 1, si no 0
        const auto Cycle = [this] { // 0 -> 150 -> 20 (y se queda en 20)
            if (m_fStatsScrollSpeed == 0.0f) {
                m_fStatsScrollSpeed = 150.0f;
            } else if (m_fStatsScrollSpeed == 150.0f) {
                m_fStatsScrollSpeed = 20.0f;
            }
        };
        const auto Toggle = [this] { // 0 -> 150, cualquier otro valor (o NaN) -> 0
            m_fStatsScrollSpeed = (m_fStatsScrollSpeed == 0.0f) ? 150.0f : 0.0f;
        };
        if (pressedLR == 1) {
            if (m_nStatsScrollDirection) {
                Cycle();
            } else {
                Toggle();
            }
            m_nStatsScrollDirection = 1;
        } else {
            if (m_nStatsScrollDirection) {
                Toggle();
            } else {
                Cycle();
            }
            m_nStatsScrollDirection = 0;
        }
        return;
    }
    case MENU_ACTION_INVERT_PAD:
        CPad::bInvertLook4Pad ^= true;
        return;
    case MENU_ACTION_23:
        m_DisplayControllerOnFoot ^= true;
        return;
    case MENU_ACTION_SUBTITLES:
        m_bShowSubtitles ^= true;
        SaveSettings();
        return;
    case MENU_ACTION_WIDESCREEN:
        m_bWidescreenOn ^= true;
        SaveSettings();
        return;
    case MENU_ACTION_RADIO_EQ:
        m_bRadioEq ^= true;
        AudioEngine.SetBassEnhanceOnOff(m_bRadioEq);
        SaveSettings();
        return;
    case MENU_ACTION_RADIO_RETUNE:
        m_bRadioAutoSelect ^= true;
        AudioEngine.SetRadioAutoRetuneOnOff(m_bRadioAutoSelect);
        SaveSettings();
        return;
    case MENU_ACTION_RADIO_STATION:
        ScrollRadioStations(pressedLR);
        return;
    case MENU_ACTION_SHOW_LEGEND:
        m_bMapLegend ^= true;
        return;
    case MENU_ACTION_RADAR_MODE:
        // Como el original (0x57720C): suma con signo y salta al otro extremo, sin módulo
        m_nRadarMode = (eRadarMode)((int32)m_nRadarMode + pressedLR);
        if ((int32)m_nRadarMode < 0) {
            m_nRadarMode = (eRadarMode)2;
        }
        if ((int32)m_nRadarMode > 2) {
            m_nRadarMode = (eRadarMode)0;
        }
        SaveSettings();
        return;
    case MENU_ACTION_HUD_MODE:
        m_bHudOn = !m_bHudOn;
        SaveSettings();
        return;
    case MENU_ACTION_LANGUAGE: {
        // Como el original (0x5771B8): recorre los 5 idiomas (0..4) en ambos sentidos
        const auto prevLanguage = (uint8)m_nPrefsLanguage;
        if (pressedLR > 0) {
            m_nPrefsLanguage = prevLanguage == 4 ? (eLanguage)0 : (eLanguage)(uint8)(prevLanguage + 1);
        } else {
            m_nPrefsLanguage = prevLanguage == 0 ? (eLanguage)4 : (eLanguage)(uint8)(prevLanguage - 1);
        }

        m_nPreviousLanguage = (eLanguage)-99; // what the fuck
        m_bLanguageChanged = true;
        InitialiseChangedLanguageSettings(0);
        SaveSettings();
        return;
    }
    default:
        return;
    }
}

// 0x57CD50
// @param pressedLR Arrow button pressed. <0 for left, >0 for right
// @param acceptPressed Is enter pressed. Used for AA mode and resolution
bool CMenuManager::ProcessPCMenuOptions(int8 pressedLR, bool acceptPressed) {
    tMenuScreen* screen   = &aScreens[m_nCurrentScreen];
    tMenuScreenItem* item = &screen->m_aItems[m_nCurrentScreenItem];

    switch (item->m_nActionType) {
    case MENU_ACTION_USER_TRACKS_SCAN:
        m_bScanningUserTracks = true;
        return true;
    case MENU_ACTION_CTRLS_JOYPAD:
        // Como el original (0x57D2FA): cualquier valor distinto de JOYPAD va a la pantalla del ratón
        if (m_ControlMethod == eController::JOYPAD) {
            SwitchToNewScreen(SCREEN_JOYPAD_SETTINGS);
        } else {
            SwitchToNewScreen(SCREEN_MOUSE_SETTINGS);
        }
        return true;
    case MENU_ACTION_CTRLS_FOOT: // Redefine Controls -> Foot Controls
        m_RedefiningControls = eControlMode::FOOT;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        m_ListSelection = 0;
        return true;
    case MENU_ACTION_CTRLS_CAR: // Redefine Controls -> Vehicle Controls
        m_RedefiningControls = eControlMode::VEHICLE;
        SwitchToNewScreen(SCREEN_CONTROLS_DEFINITION);
        m_ListSelection = 0;
        return true;
    case MENU_ACTION_FRAME_LIMITER:
        m_bPrefsFrameLimiter ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_RADAR_MODE:
        m_nRadarMode = eRadarMode(m_nRadarMode + (notsa::IsFixBugs() ? (pressedLR == 0 ? 1 : pressedLR) : 1));
        if (m_nRadarMode < eRadarMode::RADAR_MODE_MAPS_AND_BLIPS) {
            m_nRadarMode = eRadarMode(RADAR_MODE_COUNT - 1);
        } else if (m_nRadarMode >= eRadarMode::RADAR_MODE_COUNT) {
            m_nRadarMode = eRadarMode::RADAR_MODE_MAPS_AND_BLIPS;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_LANG_ENGLISH:
    case MENU_ACTION_LANG_FRENCH:
    case MENU_ACTION_LANG_GERMAN:
    case MENU_ACTION_LANG_ITALIAN:
    case MENU_ACTION_LANG_SPANISH:
#ifdef USE_LANG_RUSSIAN
    case MENU_ACTION_LANG_RUSSIAN:
#endif
#ifdef USE_LANG_JAPANESE
    case MENU_ACTION_LANG_JAPANESE:
#endif
    {
        auto newLang = static_cast<eLanguage>(item->m_nActionType - MENU_ACTION_LANG_ENGLISH);
        if (m_nPrefsLanguage != newLang) {
            m_nPrefsLanguage = newLang;
            m_bLanguageChanged = true;
            InitialiseChangedLanguageSettings(false);
            SaveSettings();
        }
        return true;
    }
    case MENU_ACTION_FX_QUALITY: {
        const auto quality = g_fx.GetFxQuality();
        if (notsa::IsFixBugs() && pressedLR < 0) { // NOTSA: la flecha izquierda retrocede
            g_fx.SetFxQuality(quality <= FX_QUALITY_LOW ? FxQuality_e(FX_QUALITY_COUNT - 1) : FxQuality_e(quality - 1));
        } else if (quality >= FX_QUALITY_LOW && quality < FX_QUALITY_COUNT) {
            // Como el original (0x57D185): 0 -> 1 -> 2 -> 3 -> 0 con una única llamada a SetFxQuality;
            // un valor fuera de rango no se toca
            g_fx.SetFxQuality(quality == FX_QUALITY_COUNT - 1 ? FX_QUALITY_LOW : FxQuality_e(quality + 1));
        }
        SaveSettings();
        return true;
    }
    case MENU_ACTION_MIP_MAPPING:
        if (!m_bMainMenuSwitch) {
            return true;
        }
        m_bPrefsMipMapping ^= true;
        RwTextureSetMipmapping(m_bPrefsMipMapping);
        SaveSettings();
        return true;
    case MENU_ACTION_ANTIALIASING: {
        if (acceptPressed) {
            if (m_nDisplayAntialiasing != m_nPrefsAntialiasing) {
                m_nPrefsAntialiasing = m_nDisplayAntialiasing;
                RwD3D9ChangeMultiSamplingLevels(m_nDisplayAntialiasing);
                SetVideoMode(m_nPrefsVideoMode);
                SaveSettings();
            }
            return true;
        }

        // scroll loop
        // Como el original (0x57D0C6): el máximo se consulta solo cuando hace falta
        // (comparación sin signo) y el tope fijo es 4
        if (pressedLR > 0) {
            ++m_nDisplayAntialiasing;
            if (m_nDisplayAntialiasing > 4 || (uint32)m_nDisplayAntialiasing > RwD3D9EngineGetMaxMultiSamplingLevels()) {
                m_nDisplayAntialiasing = 1;
            }
        } else {
            --m_nDisplayAntialiasing;
            if (m_nDisplayAntialiasing < 1) {
                m_nDisplayAntialiasing = RwD3D9EngineGetMaxMultiSamplingLevels() > 4u
                    ? 4
                    : (int32)RwD3D9EngineGetMaxMultiSamplingLevels(); // El original vuelve a llamarla
            }
        }
        return true;
    }
    case MENU_ACTION_45:
        m_CanBeDefined = true;
        m_EditingControlOptions = true;
        m_bJustOpenedControlRedefWindow = true;
        m_OptionToChange = m_nCurrentScreenItem;
        m_pPressedKey = &m_KeyPressedCode;
        return true;
    case MENU_ACTION_CONTROLS_MOUSE_INVERT_Y:
        bInvertMouseY ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_X:
        m_bInvertPadX1 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_Y:
        m_bInvertPadY1 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_X2:
        m_bInvertPadX2 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_INVERT_Y2:
        m_bInvertPadY2 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_SWAP_AXIS1:
        m_bSwapPadAxis1 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_CONTROLS_JOY_SWAP_AXIS2:
        m_bSwapPadAxis2 ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_QUIT:
        SwitchToNewScreen(SCREEN_EMPTY);
        return true;
    case MENU_ACTION_MENU_CLOSE:
        m_bDontDrawFrontEnd = 1;
        return true;
    case MENU_ACTION_PAUSE:
        SwitchToNewScreen(SCREEN_GO_BACK);
        return true;
    case MENU_ACTION_RESOLUTION: {
        // Como el original (0x57CF9B): con Intro no se desplaza la lista, se aplique o no el modo
        if (acceptPressed) {
            if (m_nDisplayVideoMode != m_nPrefsVideoMode) {
                m_nPrefsVideoMode = m_nDisplayVideoMode;
                SetVideoMode(m_nDisplayVideoMode);
                CentreMousePointer();
                m_DisplayTheMouse = true;
                m_nCurrentScreenItem = 5;
                SaveSettings();
                CPostEffects::DoScreenModeDependentInitializations();
            }
            return true;
        }

        // Como el original: primero la lista; el número de modos solo se pide al salirse del rango
        auto videoModes = GetVideoModeList();
#ifdef FIX_BUGS
        const auto numVideoModes = RwEngineGetNumVideoModes();
#endif

        if (pressedLR > 0) {
            do {
                ++m_nDisplayVideoMode;
            }
#ifdef FIX_BUGS // Out of array bounds access
            while (m_nDisplayVideoMode < numVideoModes && !videoModes[m_nDisplayVideoMode]);
#else
            while (!videoModes[m_nDisplayVideoMode]);
#endif

            if (m_nDisplayVideoMode >= RwEngineGetNumVideoModes()) {
                m_nDisplayVideoMode = 0;
                while (!videoModes[m_nDisplayVideoMode]) {
                    ++m_nDisplayVideoMode;
                }
            }
        } else {
            do {
                --m_nDisplayVideoMode;
            }
#ifdef FIX_BUGS // Out of array bounds access
            while (m_nDisplayVideoMode >= 0 && !videoModes[m_nDisplayVideoMode]);
#else
            while (!videoModes[m_nDisplayVideoMode]);
#endif

            if (m_nDisplayVideoMode < 0) {
                m_nDisplayVideoMode = RwEngineGetNumVideoModes() - 1;
                while (!videoModes[m_nDisplayVideoMode]) {
                    --m_nDisplayVideoMode;
                }
            }
        }
        return true;
    }
    case MENU_ACTION_RESET_CFG: {
        const auto targetMenu = aScreens[m_nCurrentScreen].m_aItems[2].m_nTargetMenu;
        SetDefaultPreferences(targetMenu);
        if (targetMenu == SCREEN_DISPLAY_ADVANCED) {
            RwD3D9ChangeMultiSamplingLevels(m_nPrefsAntialiasing);
            SetVideoMode(m_nPrefsVideoMode);
        } else if (targetMenu == SCREEN_CONTROLLER_SETUP) {
            ControlsManager.ReinitControls();
        }
        SaveSettings();
        SwitchToNewScreen(targetMenu);
        SetHelperText(FET_HRD);
        return true;
    }
    case MENU_ACTION_CONTROL_TYPE:
        // Como el original (0x57CF5B): cualquier valor distinto de JOYPAD pasa a JOYPAD
        if (m_ControlMethod == eController::JOYPAD) {
            m_ControlMethod = eController::MOUSE_PLUS_KEYS;
            CCamera::m_bUseMouse3rdPerson = true;
        } else {
            m_ControlMethod = eController::JOYPAD;
            CCamera::m_bUseMouse3rdPerson = false;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_STEERING:
        if (m_ControlMethod != eController::MOUSE_PLUS_KEYS) {
            return true;
        }
        CVehicle::m_bEnableMouseSteering ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_MOUSE_FLY:
        if (m_ControlMethod != eController::MOUSE_PLUS_KEYS) {
            return true;
        }
        CVehicle::m_bEnableMouseFlying ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_USER_TRACKS_PLAY_MODE:
        m_RadioMode = eRadioMode((notsa::IsFixBugs() ? (pressedLR == 0 ? 1 : pressedLR) : pressedLR) + m_RadioMode);
        if (m_RadioMode < eRadioMode::RADIO_MODE_RADIO) {
            m_RadioMode = eRadioMode(RADIO_MODE_COUNT - 1);
        } else if (m_RadioMode >= eRadioMode::RADIO_MODE_COUNT) {
            m_RadioMode = eRadioMode::RADIO_MODE_RADIO;
        }
        SaveSettings();
        return true;
    case MENU_ACTION_USER_TRACKS_AUTO_SCAN:
        m_bTracksAutoScan ^= true;
        SaveSettings();
        return true;
    case MENU_ACTION_STORE_PHOTOS:
        m_bSavePhotos ^= true;
        SaveSettings();
        return true;
    default:
        return false;
    }
}

// 0x57D520
void CMenuManager::ProcessMissionPackNewGame() {
    // Reset mission pack IDs
    for (auto& mpack : m_MissionPacks) {
        mpack.m_Id = 0u;
    }

    bool isAnyMissionPackAvailable = false;
    uint8 numMissionPacks = 0; // Como el original: los packs se guardan seguidos, no en la plaza de la carpeta

    // Scan for available mission packs
    for (auto i = 0; i < 25; i++) {
        CFileMgr::SetDirMyDocuments(); // Como el original: en cada iteración (0x57D563)
        sprintf(gString, "MPACK//MPACK%d//MPACK.DAT", i);
        if (auto file = CFileMgr::OpenFile(gString, "rb")) {
            // MPACK.DAT file format:
            //
            // <ID>#<NAME OF THE MPACK>#
            // Ex.: '5#Design Your Own Mission#'

            isAnyMissionPackAvailable = true;

            // Como el original: lectura byte a byte con CFileMgr::Read; cada campo termina en '#',
            // se ignoran '\n' y '\r' y los campos vacíos; un fichero puede tener varios packs.
            const auto fileSize = CFileMgr::GetTotalSize(file);
            char   field[256];
            uint8  fieldLen  = 0;
            bool   readingId = true;
#ifdef FIX_BUGS // El contador del original es de 8 bits: con ficheros de más de 255 bytes no termina nunca
            for (int32 pos = 0; pos < fileSize; pos++) {
#else
            for (uint8 pos = 0; (int32)pos < fileSize; pos++) {
#endif
                char ch = 0;
                CFileMgr::Read(file, &ch, 1);
                if (ch == '#') {
                    if (fieldLen > 0) {
                        field[fieldLen] = '\0';
                        if (readingId) {
                            m_MissionPacks[numMissionPacks].m_Id = (uint8)atoi(field);
                        } else {
                            strcpy(m_MissionPacks[numMissionPacks].m_Name, field); // Como el original: strcpy (strcpy_s rellena con 0xFE en depuración)
                            numMissionPacks++;
                        }
                        readingId = !readingId;
                        fieldLen  = 0;
                    }
                } else if (ch != '\n' && ch != '\r') {
                    field[fieldLen++] = ch;
                }
            }
            CFileMgr::CloseFile(file);
        }
    }

    if (isAnyMissionPackAvailable) {
        SwitchToNewScreen(SCREEN_SELECT_GAME);
    } else {
        SwitchToNewScreen(SCREEN_NEW_GAME_ASK);

        auto screen = &aScreens[m_nCurrentScreen];
        if (CGame::bMissionPackGame) {
            // Are you sure you want to start a new standard game?
            // All current game progress in this Mission Pack will be lost. Proceed?
            strncpy(screen->m_aItems[0].m_szName, "FESZ_MR", 8u); // Como el original: strncpy (strncpy_s rellena con 0xFE en depuración)
        } else {
            // Are you sure you want to start a new game?
            // All current game progress will be lost. Proceed?
            strncpy(screen->m_aItems[0].m_szName, "FESZ_QR", 8u);
        }
    }
}
