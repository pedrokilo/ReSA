/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMenuManager
 * Reconstrucción: COMPLETA — 61/61 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 61/61 entradas.
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

#include "MenuManager.h"
#include "MenuManager_Internal.h"
#include "MenuSystem.h"
#include "AEAudioUtility.h"
#include "Radar.h"
#include "Gamma.h"
#include "VideoMode.h"
#include "C_PcSave.h"
#include "platform.h"
#include "Hud.h"
#include "ControllerConfigManager.h"
#include "X87Math.h"

auto& FrontEndMenuManager = StaticRef<CMenuManager>(0xBA6748);

CMenuManager& GetMenu() {
    return FrontEndMenuManager;
}

void CMenuManager::InjectHooks() {
    RH_ScopedClass(CMenuManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x574350);
    RH_ScopedInstall(Destructor, 0x579440);
    RH_ScopedInstall(Initialise, 0x5744D0);
    RH_ScopedInstall(LoadAllTextures, 0x572EC0);
    RH_ScopedInstall(SwapTexturesRound, 0x5730A0);
    RH_ScopedInstall(UnloadTextures, 0x574630);
    RH_ScopedInstall(InitialiseChangedLanguageSettings, 0x573260);
    RH_ScopedInstall(HasLanguageChanged, 0x573CD0);
    RH_ScopedInstall(DoSettingsBeforeStartingAGame, 0x573330);
    RH_ScopedInstall(StretchX, 0x5733E0);
    RH_ScopedInstall(StretchY, 0x573410);
    RH_ScopedInstall(SwitchToNewScreen, 0x573680);
    RH_ScopedInstall(ScrollRadioStations, 0x573A00);
    RH_ScopedInstall(SetFrontEndRenderStates, 0x573A60);
    RH_ScopedInstall(SetDefaultPreferences, 0x573AE0);
    RH_ScopedInstall(GetNumberOfMenuOptions, 0x573E70);
    RH_ScopedInstall(JumpToGenericMessageScreen, 0x576AE0);

    RH_ScopedInstall(DrawFrontEnd, 0x57C290);
    RH_ScopedInstall(DrawBackground, 0x57B750);
    RH_ScopedInstall(DrawStandardMenus, 0x5794A0);
    RH_ScopedInstall(DrawWindow, 0x573EE0);
    RH_ScopedInstall(DrawWindowedText, 0x578F50);
    RH_ScopedInstall(DrawQuitGameScreen, 0x57D860);
    RH_ScopedInstall(DrawControllerScreenExtraText, 0x57D8D0);
    RH_ScopedInstall(DrawControllerBound, 0x57E6E0);
    RH_ScopedInstall(DrawControllerSetupScreen, 0x57F300);

    RH_ScopedInstall(CentreMousePointer, 0x57C520);
    RH_ScopedInstall(LoadSettings, 0x57C8F0);
    RH_ScopedInstall(SaveSettings, 0x57C660);
    RH_ScopedInstall(SaveStatsToFile, 0x57DDE0);
    RH_ScopedInstall(SaveLoadFileError_SetUpErrorScreen, 0x57C490);

    RH_ScopedInstall(CheckSliderMovement, 0x573440);
    RH_ScopedInstall(CheckFrontEndUpInput, 0x573840);
    RH_ScopedInstall(CheckFrontEndDownInput, 0x5738B0);
    RH_ScopedInstall(CheckFrontEndLeftInput, 0x573920);
    RH_ScopedInstall(CheckFrontEndRightInput, 0x573990);
    RH_ScopedInstall(CheckForMenuClosing, 0x576B70);
    RH_ScopedInstall(CheckHover, 0x57C4F0);
    RH_ScopedInstall(CheckMissionPackValidMenu, 0x57D720);
    RH_ScopedInstall(CheckCodesForControls, 0x57DB20);

    RH_ScopedInstall(DisplaySlider, 0x576860);
    RH_ScopedInstall(DisplayHelperText, 0x57E240);
    RH_ScopedInstall(SetHelperText, 0x57CD10);
    RH_ScopedInstall(ResetHelperText, 0x57CD30);
    RH_ScopedInstall(MessageScreen, 0x579330);
    RH_ScopedInstall(SmallMessageScreen, 0x574010);

    RH_ScopedInstall(PrintMap, 0x575130);
    RH_ScopedInstall(PrintStats, 0x574900);
    RH_ScopedInstall(PrintBriefs, 0x576320);
    RH_ScopedInstall(PrintRadioStationList, 0x5746F0);

    RH_ScopedInstall(UserInput, 0x57FD70);
    RH_ScopedInstall(AdditionalOptionInput, 0x5773D0);
    RH_ScopedInstall(CheckRedefineControlInput, 0x57E4D0);
    RH_ScopedInstall(RedefineScreenUserInput, 0x57EF50);

    RH_ScopedInstall(Process, 0x57B440);
    RH_ScopedInstall(ProcessStreaming, 0x573CF0);
    RH_ScopedInstall(ProcessFileActions, 0x578D60);
    RH_ScopedInstall(ProcessUserInput, 0x57B480);
    RH_ScopedInstall(ProcessMenuOptions, 0x576FE0);
    RH_ScopedInstall(ProcessPCMenuOptions, 0x57CD50);
    RH_ScopedInstall(ProcessMissionPackNewGame, 0x57D520);
}

// 0x574350
CMenuManager::CMenuManager() {
    // Como el original: `eh vector constructor iterator` sobre los 25 sprites (0x821D30), no solo el primero.
    for (auto& sprite : m_aFrontEndSprites) {
        sprite.Constructor();
    }
    m_nPlayerNumber          = 0;
    m_bDoVideoModeUpdate     = false;
    m_DeleteAllNextDefine    = false;
    m_DeleteAllBoundControls = false;
    m_nCurrentRwSubsystem    = 0;
    SetDefaultPreferences(SCREEN_DISPLAY_ADVANCED);
    SetDefaultPreferences(SCREEN_CONTROLLER_SETUP);
    field_EC                      = 0;
    m_pPressedKey                 = 0;
    m_MenuIsAbleToQuit            = false;
    m_nTitleLanguage              = 9;
    m_UserTrackIndex              = 0;
    m_ControlMethod               = eController::MOUSE_PLUS_KEYS;
    CCamera::m_bUseMouse3rdPerson = 1;
    m_nMousePosX                  = m_nMousePosWinX;
    m_ListSelection               = 0;
    m_bMainMenuSwitch             = true;
    m_nMousePosY                  = m_nMousePosWinY;
    m_nOldMousePosX               = 0;
    m_nOldMousePosY               = 0;
    m_DisplayTheMouse             = false;
    m_MouseInBounds               = eMouseInBounds::NONE;
    m_nTargetBlipIndex            = 0;
    m_bMenuAccessWidescreen       = false;
    SetDefaultPreferences(SCREEN_AUDIO_SETTINGS);
    SetDefaultPreferences(SCREEN_DISPLAY_SETTINGS);
    m_nRadioStation            = CAEAudioUtility::GetRandomRadioStation();
    m_bStreamingDisabled       = false;
    m_bAllStreamingStuffLoaded = false;

    m_bLanguageChanged  = false;
    m_nPrefsLanguage    = eLanguage::AMERICAN;
    m_nTextLanguage     = 0;
    m_nPreviousLanguage = eLanguage::AMERICAN;
    m_SystemLanguage    = 0;

    m_DisplayControllerOnFoot = false;
    m_bDontDrawFrontEnd       = false;
    m_bActivateMenuNextFrame  = false;
    m_bMenuActive             = false;
    m_bIsSaveDone             = false;
    m_bLoadingData            = false;
    m_isPreInitialised        = false;
    m_fStatsScrollSpeed       = 150.0f;
    m_nStatsScrollDirection   = 1;
    m_KeyPressedCode          = (RsKeyCodes)-1;
    m_PrefsUseVibration       = true;
    m_fMapZoom                = 300.0f;
    m_vMapOrigin.x            = APP_MINIMAL_WIDTH / 2;
    m_vMapOrigin.y            = 224.0f; // Como el original: 0x43600000 (448 / 2), no APP_MINIMAL_HEIGHT / 2 (240).
}

CMenuManager* CMenuManager::Constructor() {
    this->CMenuManager::CMenuManager();
    return this;
}

CMenuManager* CMenuManager::Destructor() {
    this->CMenuManager::~CMenuManager();
    return this;
}

// 0x579440
CMenuManager::~CMenuManager() {
    UnloadTextures();
    // Como el original: `eh vector destructor iterator` (0x821E02) destruye los 25 sprites, del último al primero.
    for (auto i = std::size(m_aFrontEndSprites); i-- > 0;) {
        m_aFrontEndSprites[i].Destructor();
    }
}

// 0x5744D0
void CMenuManager::Initialise() {
    m_nPlayerNumber = 0;
    m_TimeToStopPadShaking = 0;
    m_nCurrentScreenItem = 0;
    m_SelectedSlot = 0;
    if (m_bDoVideoModeUpdate) {
        RwD3D9ChangeMultiSamplingLevels(m_nPrefsAntialiasing);
        SetVideoMode(m_nPrefsVideoMode);
        m_bDoVideoModeUpdate = false;
    }
    CentreMousePointer();
    m_DisplayTheMouse = false;

    m_nSelectedRow = 3;
    m_nSysMenu     = CMenuSystem::MENU_UNDEFINED;

    m_fMapZoom = FRONTEND_MAP_RANGE_MAX;

    // Como el original: los cuatro bordes se calculan antes de corregir nada,
    // y cada corrección es origen - (origen - 1100) o (límite - (origen + 1100)) + origen.
    const float top    = m_vMapOrigin.y - FRONTEND_MAP_RANGE_MAX;
    const float bottom = m_vMapOrigin.y + FRONTEND_MAP_RANGE_MAX;
    const float left   = m_vMapOrigin.x - FRONTEND_MAP_RANGE_MAX;
    const float right  = m_vMapOrigin.x + FRONTEND_MAP_RANGE_MAX;

    if (left > 0.0f) {
        m_vMapOrigin.x = m_vMapOrigin.x - left;
    }

    if (right < 640.0f) {
        m_vMapOrigin.x = (640.0f - right) + m_vMapOrigin.x;
    }

    if (top > 0.0f) {
        m_vMapOrigin.y = m_vMapOrigin.y - top;
    }

    if (bottom < 448.0f) {
        m_vMapOrigin.y = (448.0f - bottom) + m_vMapOrigin.y;
    }

    CRadar::SetMapCentreToPlayerCoords();
    CPad::StopPadsShaking();
    if (!m_isPreInitialised) {
        m_nCurrentScreen = SCREEN_INITIAL;
        m_bMapLoaded = true;
        m_nOldMousePosX = 0;
        m_nOldMousePosY = 0;
        m_DisplayTheMouse = false;
    }
    m_nRadioStation = AudioEngine.GetCurrentRadioStationID();
    CFileMgr::SetDir("");
    s_PcSaveHelper.PopulateSlotInfo();
    CTimer::StartUserPause();
}

// 0x572EC0
void CMenuManager::LoadAllTextures() {
    if (m_bTexturesLoaded)
        return;

    CTimer::Stop();

    // Como el original: frontend1, frontend_pc y frontend2 (en ese orden y con esos tamaños);
    // frontend3 no se carga aquí (solo SwapTexturesRound) y frontend2 no llama a TidyUpMemory.
    const auto LoadTexture = [=](unsigned memorySize, bool tidyUp, const char* slotName, const char* txdName, unsigned start, unsigned end) {
        CStreaming::MakeSpaceFor(memorySize);
        CStreaming::ImGonnaUseStreamingMemory();
        if (tidyUp) {
            CGame::TidyUpMemory(false, true);
        }
        CTxdStore::PushCurrentTxd();
        auto slot = CTxdStore::FindOrAddTxdSlot(slotName);
        CTxdStore::LoadTxd(slot, txdName);
        CTxdStore::AddRef(slot);
        CTxdStore::SetCurrentTxd(slot);

        for (auto i = start; i < end; i++) {
            const auto t = FrontEndFilenames[i];
            m_aFrontEndSprites[i].SetTexture(t.name, t.alpha);
            m_aFrontEndSprites[i].SetAddressing(rwTEXTUREADDRESSCLAMP);
        }

        CTxdStore::PopCurrentTxd();
        CStreaming::IHaveUsedStreamingMemory();
    };

    LoadTexture(0x21000, true,  "frontend1",   "MODELS\\FRONTEN1.TXD",  FRONTEND1_START, FRONTEND2_START);
    LoadTexture(0x3000,  true,  "frontend_pc", "MODELS/FRONTEN_pc.TXD", FRONTEND4_START, FRONTEND_SPRITE_COUNT);
    LoadTexture(0x4C000, false, "frontend2",   "MODELS\\FRONTEN2.TXD",  FRONTEND2_START, FRONTEND3_START);

    m_bTexturesLoaded = true;
    CTimer::Update();
}

// round always false;
// 0x5730A0
void CMenuManager::SwapTexturesRound(bool round) {
    // Como el original (0x5730A7/0x5730B8): el indicador es el de FrontEndMenuManager (0xBA8268), no el de `this`.
    auto& texturesSwapped = FrontEndMenuManager.m_TexturesSwapped;
    if (texturesSwapped == round)
        return; // Swap doesn't needed

    const auto LoadTexture = [](const char* txdName, const char* slotName, unsigned size) {
        CStreaming::MakeSpaceFor(size);
        CStreaming::ImGonnaUseStreamingMemory();
        CTxdStore::PushCurrentTxd();
        auto slot = CTxdStore::FindOrAddTxdSlot(slotName);
        CTxdStore::LoadTxd(slot, txdName);
        CTxdStore::AddRef(slot);
        CTxdStore::SetCurrentTxd(slot);
    };

    texturesSwapped = round;

    if (round || !CTxdStore::FindTxdSlot("frontend3")) {
        auto slot = CTxdStore::FindTxdSlot("frontend2");
        std::ranges::for_each(m_apBackgroundTextures, [](auto& sprite) { sprite.Delete(); });

        CTxdStore::RemoveTxd(slot);
        LoadTexture("MODELS/FRONTEN3.TXD", "frontend3", 0x6800);

        for (unsigned i = FRONTEND3_START; i < FRONTEND4_START; i++) {
            m_aFrontEndSprites[i].SetTexture(FrontEndFilenames[i].name, FrontEndFilenames[i].alpha);
            m_aFrontEndSprites[i].SetAddressing(rwTEXTUREADDRESSCLAMP);
        }
    } else {
        auto slot = CTxdStore::FindTxdSlot("frontend3");
        std::ranges::for_each(m_apAdditionalBackgroundTextures, [](auto& sprite) { sprite.Delete(); });

        CTxdStore::RemoveTxd(slot);
        LoadTexture("MODELS/FRONTEN2.TXD", "frontend2", 0x4C000);

        for (unsigned i = FRONTEND2_START; i < FRONTEND3_START; i++) {
            m_aFrontEndSprites[i].SetTexture(FrontEndFilenames[i].name, FrontEndFilenames[i].alpha);
            m_aFrontEndSprites[i].SetAddressing(rwTEXTUREADDRESSCLAMP);
        }
    }
    CTxdStore::PopCurrentTxd();
    CStreaming::IHaveUsedStreamingMemory();
}

// 0x574630
void CMenuManager::UnloadTextures() {
    if (m_bTexturesLoaded) {
        SwapTexturesRound(false);

        const auto Unload = [](const char* slot, auto& sprites) {
            const auto txd = CTxdStore::FindTxdSlot(slot); // Como el original: se busca antes de borrar los sprites.
            std::ranges::for_each(sprites, [](auto& sprite) { sprite.Delete(); });
            CTxdStore::RemoveTxd(txd);
        };

        // Como el original: frontend1, frontend_pc y frontend2; frontend3 ya lo retiró SwapTexturesRound(false).
        Unload("frontend1",   m_apRadioSprites);
        Unload("frontend_pc", m_apMouseTextures);
        Unload("frontend2",   m_apBackgroundTextures);

        m_bTexturesLoaded = false;
    }
}

// 0x573260
void CMenuManager::InitialiseChangedLanguageSettings(bool reinitControls) {
    if (!m_bLanguageChanged)
        return;

    m_bLanguageChanged = false;

    CTimer::Stop();

    char mission[8] = {0};
    TheText.GetNameOfLoadedMissionText(mission);
    TheText.Load(CGame::bMissionPackGame != 0);
    if (mission[0]) {
        TheText.LoadMissionText(mission);
    }

    CTimer::Update();

    // Como el original (0x5732CA): m_SystemLanguage se compara como dword (1 = francés, 2 = alemán), sin truncar a 8 bits
    const auto lang88 = m_SystemLanguage;
    if (lang88 != (int32)eLanguage::FRENCH && lang88 != (int32)eLanguage::GERMAN) {
        switch (m_nPrefsLanguage) {
        case eLanguage::AMERICAN:
        case eLanguage::GERMAN:
        case eLanguage::ITALIAN:
        case eLanguage::SPANISH:
            CLocalisation::SetNormalGame();
            break;
        case eLanguage::FRENCH:
            CLocalisation::SetFrenchGame();
            break;
        default:
            break;
        }
    }

    if (lang88 == (int32)eLanguage::FRENCH) {
        CLocalisation::SetFrenchGame();
    }

    if (reinitControls) {
        ControlsManager.ReinitControls();
    }
}

// 0x573CD0
bool CMenuManager::HasLanguageChanged() {
    if (m_nPreviousLanguage == m_nPrefsLanguage)
        return false;

    m_nPreviousLanguage = m_nPrefsLanguage;
    return true;
}

// 0x573330
void CMenuManager::DoSettingsBeforeStartingAGame() {
    std::ranges::fill(CStats::FavoriteRadioStationList, 0);
    if (m_bMenuActive)
        AudioEngine.Reset();

    m_nRadioStation = CAEAudioUtility::GetRandomRadioStation();
    m_bDontDrawFrontEnd = true;
    m_bStartGameLoading = true;

    if (m_nTargetBlipIndex) {
        CRadar::ClearBlip(m_nTargetBlipIndex);
        m_nTargetBlipIndex = 0;
    }

    CStats::UpdateRespectStat(true);
    CCheat::m_bHasPlayerCheated = false;
    CHud::SetZoneName(nullptr, 1);
    TheCamera.SetFadeColour(0, 0, 0);
    TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
    TheCamera.ProcessFade();
    TheCamera.Fade(2.0f, eFadeFlag::FADE_OUT);
}

// 0x5733E0
float CMenuManager::StretchX(float x) {
    // Como el original: (ancho * x) * 0.0015625f (0x859520), no x * ancho / 640.
    return (RsGlobal.maximumWidth == DEFAULT_SCREEN_WIDTH) ? x : SCREEN_WIDTH * x * 0.0015625f;
}

// 0x573410
float CMenuManager::StretchY(float y) {
    // Como el original: (alto * y) * 0.002232143f (0x859524), no y * alto / 448.
    return (RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT) ? y : SCREEN_HEIGHT * y * 0.002232143f;
}

// 0x573680
void CMenuManager::SwitchToNewScreen(eMenuScreen screen) {
    // Works well, but needs more attention because of the trash gotos
    m_nPrevScreen = m_nCurrentScreen;
    m_ControllerError = eControllerError::NONE;

    ResetHelperText();

    if (m_nCurrentScreen == SCREEN_DISPLAY_SETTINGS) {
        m_nDisplayAntialiasing = m_nPrefsAntialiasing;
        m_nDisplayVideoMode    = m_nPrefsVideoMode;
    }

    if (screen == SCREEN_GO_BACK) {
        if (m_bMainMenuSwitch) {
            switch (m_nCurrentScreen) {
            case SCREEN_START_GAME:
                m_nCurrentScreenItem = 0;
                break;
            case SCREEN_OPTIONS:
                m_nCurrentScreenItem = 1;
                break;
            case SCREEN_QUIT_GAME_ASK:
                m_nCurrentScreenItem = 2;
                break;
            default:
                m_nCurrentScreenItem = aScreens[m_nCurrentScreen].m_nStartEntry;
                break;
            }
        } else {
            m_nCurrentScreenItem = aScreens[m_nCurrentScreen].m_nStartEntry;
        }
        m_nCurrentScreen = aScreens[m_nCurrentScreen].m_nParentMenu;
    } else {
        if (screen) {
            m_nCurrentScreenItem = 0;
            m_nCurrentScreen     = screen;
        }
        else {
            // Como el original: el destino se lee con la opción actual antes de ponerla a 0.
            const auto target    = aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nTargetMenu;
            m_nCurrentScreenItem = 0;
            m_nCurrentScreen     = target;
        }
    }

    if (m_nCurrentScreen == SCREEN_GAME_SAVE) {
        m_nCurrentScreenItem = 9;
    }

    if (m_nPrevScreen == SCREEN_AUDIO_SETTINGS) {
        AudioEngine.StopRadio(nullptr, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
    }

    if (m_nCurrentScreen == SCREEN_AUDIO_SETTINGS) {
        AudioEngine.StartRadio(m_nRadioStation, eBassSetting::NORMAL);
    }

    if (m_bMainMenuSwitch) {
        switch (m_nCurrentScreen) {
        case SCREEN_NEW_GAME_ASK:
            CGame::bMissionPackGame = 0;
            DoSettingsBeforeStartingAGame();
            m_bDontDrawFrontEnd = true;
            break;
        case SCREEN_MISSION_PACK_LOADING_ASK:
            CGame::bMissionPackGame = m_nMissionPackGameId;
            DoSettingsBeforeStartingAGame();
            m_bDontDrawFrontEnd = true;
            break;
        case SCREEN_LOAD_GAME_ASK:
            m_nCurrentScreen = SCREEN_LOAD_FIRST_SAVE;
            break;
        }
    }
}

// 0x573A00
void CMenuManager::ScrollRadioStations(int8 numStations) {
    if (!m_nRadioStation || CAudioEngine::IsCutsceneTrackActive()) {
        return;
    }

    m_nRadioStation = static_cast<eRadioID>(m_nRadioStation + numStations);
    if (m_nRadioStation <= RADIO_EMERGENCY_AA) {
        m_nRadioStation = RADIO_OFF;
    }
    if (m_nRadioStation >= RADIO_COUNT) {
        m_nRadioStation = RADIO_CLASSIC_HIP_HOP;
    }
    AudioEngine.RetuneRadio(m_nRadioStation);
    SaveSettings();
}

// 0x573A60
void CMenuManager::SetFrontEndRenderStates() {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
}

// 0x573AE0
void CMenuManager::SetDefaultPreferences(eMenuScreen screen) {
    switch (screen) {
    case SCREEN_AUDIO_SETTINGS:
        m_nSfxVolume           = 64;
        m_nRadioVolume         = 64;
        m_PrefsAudioOutputMode = true;
        m_bRadioEq             = true;
        m_bRadioAutoSelect     = true;
        m_bTracksAutoScan      = false;
        m_RadioMode            = eRadioMode::RADIO_MODE_RADIO;
        // Como el original (0x573B10...0x573B72): los cuatro valores se leen de FrontEndMenuManager (0xBA6795...0xBA6799),
        // no de `this`; con el singleton es lo mismo, con otro objeto no.
        AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);
        AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
        AudioEngine.SetBassEnhanceOnOff(FrontEndMenuManager.m_bRadioEq);
        AudioEngine.SetRadioAutoRetuneOnOff(FrontEndMenuManager.m_bRadioAutoSelect);
        break;
    case SCREEN_DISPLAY_SETTINGS:
    case SCREEN_DISPLAY_ADVANCED:
        g_fx.SetFxQuality(FX_QUALITY_HIGH);
        m_PrefsBrightness                = 256;
        SetBrightness(256.0f, true);
        m_fDrawDistance                  = 1.2f;
        CRenderer::ms_lodDistScale       = 1.2f;
        m_bPrefsFrameLimiter             = true;
        m_bHudOn                         = true;
        m_bSavePhotos                    = true;
        m_bPrefsMipMapping               = true;
        m_nPrefsAntialiasing             = 1;
        m_nDisplayAntialiasing           = 1;
        m_bWidescreenOn                  = false;
        m_bMapLegend                     = false;
        m_nRadarMode                     = eRadarMode::RADAR_MODE_MAPS_AND_BLIPS;
        m_nDisplayVideoMode              = m_nPrefsVideoMode;
        m_ShowLocationsBlips             = true;
        m_ShowContactsBlips              = true;
        m_ShowMissionBlips               = true;
        m_ShowOtherBlips                 = true;
        m_ShowGangAreaBlips              = true;
        m_bShowSubtitles                 = false; // Como el original: mov byte ptr [esi+44h],0 (0x573BF1)
        break;
    case SCREEN_CONTROLLER_SETUP:
        m_ControlMethod                  = eController::MOUSE_PLUS_KEYS;
        bInvertMouseY                    = false;
        CVehicle::m_bEnableMouseSteering = false;
        CCamera::m_fMouseAccelHorzntl    = 0.0025f;
        CCamera::m_bUseMouse3rdPerson    = true;
        CVehicle::m_bEnableMouseFlying   = true;
        m_bInvertPadX1                   = false;
        m_bInvertPadY1                   = false;
        m_bInvertPadX2                   = false;
        m_bInvertPadY2                   = false;
        m_bSwapPadAxis1                  = false;
        m_bSwapPadAxis2                  = false;
        break;
    default:
        return;
    }
}

// 0x573E70
uint32 CMenuManager::GetNumberOfMenuOptions() {
    static auto& s_PrevScreen = StaticRef<int32>(0x8CDFF0);

    if (m_nCurrentScreen == SCREEN_MAP || m_nCurrentScreen == SCREEN_BRIEF) {
        return 2;
    }

    // return previously calculated value (caching)
    if (s_PrevScreen == m_nCurrentScreen) {
        return m_nNumberOfMenuOptions;
    }

    const auto Calc = [=]() -> uint8 {
        auto result = 0;
        for (auto& item : aScreens[m_nCurrentScreen].m_aItems) {
            const auto actionType = item.m_nActionType;
            if (!actionType) {
                break;
            }
            if (actionType != MENU_ACTION_TEXT && actionType != MENU_ACTION_SKIP) {
                result++;
            }
        }
        return result;
    };

    s_PrevScreen = m_nCurrentScreen;
    m_nNumberOfMenuOptions = 0;
    m_nNumberOfMenuOptions = Calc();

    return m_nNumberOfMenuOptions;
}

// 0x576AE0
void CMenuManager::JumpToGenericMessageScreen(eMenuScreen screen, const char* titleKey, const char* textKey) {
    SwitchToNewScreen(screen);

    auto& mscreen = aScreens[m_nCurrentScreen];

    // Como el original: el destino se escribe en el segundo elemento (+0x26 = m_aItems[1].m_nTargetMenu).
    switch (screen) {
    case SCREEN_GAME_SAVED:
        mscreen.m_aItems[1].m_nTargetMenu = SCREEN_START_GAME;
        break;
    case SCREEN_GAME_LOADED:
        mscreen.m_aItems[1].m_nTargetMenu = SCREEN_GAME_SAVE;
        break;
    default:
        break;
    }
    // Como el original: strncpy de 8 bytes (rellena con ceros y no fuerza el terminador).
    strncpy(mscreen.m_szTitleName, titleKey, 8);
    strncpy(mscreen.m_aItems[0].m_szName, textKey, 8);
}

// 0x57C520
void CMenuManager::CentreMousePointer() {
    CVector2D pos{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    if (pos.x != 0.0f && pos.y != 0.0f) {
        // Como el original: psMouseSetPos va en línea con mitades enteras (ancho / 2),
        // y lastMousePos recibe esas mitades convertidas a float (no ancho * 0.5f).
        POINT point = { .x = RsGlobal.maximumWidth / 2, .y = RsGlobal.maximumHeight / 2 };
        ::ClientToScreen(PSGLOBAL(window), &point);
        ::SetCursorPos(point.x, point.y);
        PSGLOBAL(lastMousePos).x = (float)(RsGlobal.maximumWidth / 2);
        PSGLOBAL(lastMousePos).y = (float)(RsGlobal.maximumHeight / 2);
    }
}

// 0x57C8F0
void CMenuManager::LoadSettings() {
    CFileMgr::SetDirMyDocuments();

    auto file = CFileMgr::OpenFile("gta_sa.set", "rb");
    if (!file) {
        CFileMgr::SetDir("");
        return;
    }

    // Como el original: el idioma previo se guarda antes de leer y se restaura si el fichero no vale.
    const auto previousLang = m_nPrefsLanguage;

    // Como el original: se pone a true antes de leer (0x57C933); la lectura o los valores por defecto lo sobrescriben.
    bInvertMouseY = true;

    const auto SetToDefaultSettings = [&]() {
        SetDefaultPreferences(SCREEN_AUDIO_SETTINGS);
        SetDefaultPreferences(SCREEN_DISPLAY_SETTINGS);
        SetDefaultPreferences(SCREEN_DISPLAY_ADVANCED);
        SetDefaultPreferences(SCREEN_CONTROLLER_SETUP);
        // Como el original: pone a 0 el subsistema RW (+0xDC), no el modo de vídeo, y restaura el idioma previo.
        m_nCurrentRwSubsystem = 0;
        m_nPrefsLanguage = previousLang;
        m_nRadioStation = RADIO_CLASSIC_HIP_HOP;

        CFileMgr::CloseFile(file);
        CFileMgr::SetDir("");
    };

    const auto ReadFromFile = [&](auto& ref, size_t size = 0u) {
        CFileMgr::Read(file, &ref, size != 0 ? size : sizeof(ref));
    };

    {
        char buf[29]; // Como el original: sin poner a cero (el EXE solo lo rellena con la lectura).
        ReadFromFile(buf, 29u);
        if (!strncmp(buf, TopLineEmptyFile, 26u)) {
            return SetToDefaultSettings();
        }

        CFileMgr::Seek(file, 0, 0);
    }

    uint32 version = 0u;
    struct { uint8 m_unk; uint8 m_separator; uint8 m_underscore; uint8 _align; } constants;
    FxQuality_e fxQuality = FX_QUALITY_HIGH;

    ReadFromFile(version);
    // Como el original: comparación con signo (jl).
    if ((int32)version < (int32)SETTINGS_FILE_VERSION || !ControlsManager.LoadSettings(file)) {
        return SetToDefaultSettings();
    }

    ReadFromFile(CCamera::m_fMouseAccelHorzntl);
    ReadFromFile(bInvertMouseY);
    ReadFromFile(CVehicle::m_bEnableMouseSteering);
    ReadFromFile(CVehicle::m_bEnableMouseFlying);
    ReadFromFile(m_nSfxVolume);
    ReadFromFile(m_nRadioVolume);
    ReadFromFile(m_nRadioStation);
    ReadFromFile(m_bRadioAutoSelect);
    ReadFromFile(m_bRadioEq);
    ReadFromFile(m_PrefsBrightness);
    ReadFromFile(m_bPrefsMipMapping);
    ReadFromFile(m_bTracksAutoScan);
    ReadFromFile(m_nDisplayAntialiasing);
    ReadFromFile(fxQuality);
    ReadFromFile(constants.m_unk);
    ReadFromFile(m_fDrawDistance);
    ReadFromFile(m_bShowSubtitles);
    ReadFromFile(m_bWidescreenOn);
    ReadFromFile(m_bPrefsFrameLimiter);
    ReadFromFile(m_nDisplayVideoMode);
    ReadFromFile(m_ControlMethod);
    ReadFromFile(m_nPrefsLanguage);
    ReadFromFile(m_bHudOn);
    ReadFromFile(m_nRadarMode);
    ReadFromFile(m_RadioMode);
    ReadFromFile(m_bSavePhotos);
    ReadFromFile(constants.m_separator);
    ReadFromFile(m_bInvertPadX1);
    ReadFromFile(m_bInvertPadY1);
    ReadFromFile(m_bInvertPadX2);
    ReadFromFile(m_bInvertPadY2);
    ReadFromFile(m_bSwapPadAxis1);
    ReadFromFile(m_bSwapPadAxis2);
    ReadFromFile(m_bMapLegend);
    ReadFromFile(m_UserTrackIndex);
    ReadFromFile(m_nCurrentRwSubsystem);
    ReadFromFile(constants.m_underscore);

    if (constants.m_unk != 'T' ||
        constants.m_separator != 0x1D || // ASCII GS - Group Separator
        constants.m_underscore != '_'
    ) {
        return SetToDefaultSettings();
    }

    CCamera::m_bUseMouse3rdPerson = (m_ControlMethod == eController::MOUSE_PLUS_KEYS);
    CRenderer::ms_lodDistScale = m_fDrawDistance;
    g_fx.SetFxQuality(fxQuality);
    SetBrightness(static_cast<float>(m_PrefsBrightness), true);
    m_nPrefsAntialiasing = m_nDisplayAntialiasing;
    m_bDoVideoModeUpdate = true;
    // Como el original (0x57CC19...0x57CC5D): volúmenes, ecualizador y autosintonía salen de FrontEndMenuManager
    // (0xBA6795...0xBA6799); solo la emisora (+0x52) sale de `this`.
    AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);
    AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
    AudioEngine.SetBassEnhanceOnOff(FrontEndMenuManager.m_bRadioEq);
    AudioEngine.SetRadioAutoRetuneOnOff(FrontEndMenuManager.m_bRadioAutoSelect);
    AudioEngine.RetuneRadio(m_nRadioStation);

    if (previousLang == m_nPrefsLanguage) {
        m_LoadedLanguage = false;
    } else {
        m_LoadedLanguage = true;
        TheText.Load(false);
        m_bLanguageChanged = true;
        InitialiseChangedLanguageSettings(false);
        OutputDebugStringA("The previously saved language is now in use"); // SA
    }

    CFileMgr::CloseFile(file);
    CFileMgr::SetDir("");
}

// 0x57C660
void CMenuManager::SaveSettings() {
    CFileMgr::SetDirMyDocuments();

    auto file = CFileMgr::OpenFile("gta_sa.set", "w+b");
    if (!file) {
        CFileMgr::SetDir("");
        return;
    }

    const auto WriteToFile = [&](auto&& v, size_t size = 0u) {
        CFileMgr::Write(file, &v, size != 0 ? size : sizeof(v));
    };

    WriteToFile(SETTINGS_FILE_VERSION);
    ControlsManager.SaveSettings(file);
    WriteToFile(CCamera::m_fMouseAccelHorzntl);
    WriteToFile(bInvertMouseY);
    WriteToFile(CVehicle::m_bEnableMouseSteering);
    WriteToFile(CVehicle::m_bEnableMouseFlying);
    WriteToFile(m_nSfxVolume);
    WriteToFile(m_nRadioVolume);
    WriteToFile(m_nRadioStation);
    WriteToFile(m_bRadioAutoSelect);
    WriteToFile(m_bRadioEq);
    WriteToFile(m_PrefsBrightness);
    WriteToFile(m_bPrefsMipMapping);
    WriteToFile(m_bTracksAutoScan);
    WriteToFile(m_nPrefsAntialiasing);
    WriteToFile(g_fx.GetFxQuality());
    WriteToFile(int8(84)); // T
    WriteToFile(m_fDrawDistance);
    WriteToFile(m_bShowSubtitles);
    WriteToFile(m_bWidescreenOn);
    WriteToFile(m_bPrefsFrameLimiter);
    WriteToFile(m_nPrefsVideoMode);
    WriteToFile(m_ControlMethod);
    WriteToFile(m_nPrefsLanguage);
    WriteToFile(m_bHudOn);
    WriteToFile(m_nRadarMode);
    WriteToFile(m_RadioMode);
    WriteToFile(m_bSavePhotos);
    WriteToFile(int8(29)); // GS - Group Separator
    WriteToFile(m_bInvertPadX1);
    WriteToFile(m_bInvertPadY1);
    WriteToFile(m_bInvertPadX2);
    WriteToFile(m_bInvertPadY2);
    WriteToFile(m_bSwapPadAxis1);
    WriteToFile(m_bSwapPadAxis2);
    WriteToFile(m_bMapLegend);
    WriteToFile(m_UserTrackIndex);
    WriteToFile(RwEngineGetCurrentSubSystem());
    WriteToFile(int8(95)); // _ underscore

    CFileMgr::CloseFile(file);
    CFileMgr::SetDir("");
}

// 0x57DDE0
void CMenuManager::SaveStatsToFile() {
    CFileMgr::SetDirMyDocuments();
    char date[12]{};
    _strdate_s(date, 12u);

    // Como el original: un solo CText::Get (con el nombre de la última misión o "ITBEG").
    const GxtChar* lastMissionPassedKey = CStats::LastMissionPassedName[0]
        ? TheText.Get(CStats::LastMissionPassedName)
        : TheText.Get("ITBEG"); // In the beginning

    auto file = CFileMgr::OpenFile("stats.html", "w");
    if (file) {
        const auto ToUpperCase = [](const char* s) {
            // HACK: This seems to be the cleanest way...
            auto wide = UTF8ToUnicode(s);
            rng::transform(wide, wide.begin(), towupper);
            return UnicodeToUTF8(wide);
        };

        // Como el original: mismas cadenas y mismos cortes de fprintf que el EXE (0x86636C...0x865708).
        fprintf_s(file, "<title>Grand Theft Auto San Andreas Stats</title>\n");
        fprintf_s(file, "<body bgcolor=\"#000000\" leftmargin=\"10\" topmargin=\"10\" marginwidth=\"10\" marginheight=\"10\">\n");
        fprintf_s(file, "<table width=\"560\" align=\"center\" border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\n");
        fprintf_s(file, "<tr align=\"center\" valign=\"top\"> \n");
        fprintf_s(file, "<td height=\"59\" colspan=\"2\" bgcolor=\"#000000\"><div align=\"center\"><font color=\"#FFFFFF\" size=\"5\" face=\"Arial, \n");
        fprintf_s(file, "Helvetica, sans-serif\">-------------------------------------------------------------------</font><font \n");
        fprintf_s(file, "size=\"5\" face=\"Arial, Helvetica, sans-serif\"><br>\n");
        fprintf_s(file, "<strong><font color=\"#FFFFFF\">GRAND THEFT AUTO SAN ANDREAS ");
        fprintf_s(file, "%s</font></strong><br><font\n", ToUpperCase(GxtCharToUTF8(TheText.Get("FEH_STA"))).c_str()); // Stats
        fprintf_s(file, "color=\"#FFFFFF\">-------------------------------------------------------------------</font></font></div></td> </tr>\n");
        fprintf_s(file, "<tr align=\"center\" valign=\"top\" bgcolor=\"#000000\">     <td height=\"22\" colspan=\"2\">&nbsp;</td>  </tr>\n");
        fprintf_s(file, "<tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> \n");
        fprintf_s(file, "<td height=\"40\" colspan=\"2\"> <p><font color=\"#F0000C\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong><font color=\"#F0000C\" size=\"1\">%s: \n", GxtCharToUTF8(TheText.Get("FES_DAT"), 0u)); // DATE
        fprintf_s(file, "%s</font><br>        %s: </strong>", date, GxtCharToUTF8(TheText.Get("FES_CMI"), 0u));     // LAST MISSION PASSED
        fprintf_s(file, "%s<strong><br></strong> </font></p></td></tr>\n", ToUpperCase(GxtCharToUTF8(lastMissionPassedKey)).c_str());
        fprintf_s(file, "<tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> <td height=\"5\" colspan=\"2\"></td> </tr> <tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> \n");
        fprintf_s(file, "<td height=\"10\" colspan=\"2\"></td> </tr> <tr align=\"center\" valign=\"top\" bgcolor=\"#000000\"> \n");
        fprintf_s(file, "<td height=\"20\" colspan=\"2\"><font color=\"#F0000C\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>%s</strong>\n", GxtCharToUTF8(TheText.Get("CRIMRA"), 0u)); // Criminal rating:
        TextCopy(gGxtString, CStats::FindCriminalRatingString());
        {
            const auto ratingStr = GxtCharToUTF8(gGxtString, 0u); // Como el original: se convierte antes de pedir el número
            fprintf_s(file, "%s (%d)</font></td>  </tr>  <tr align=\"left\" valign=\"top\" bgcolor=\"#000000\"><td height=\"10\" colspan=\"2\"></td>  </tr>\n", ratingStr, CStats::FindCriminalRatingNumber());
        }

        static constexpr const char* strToPrint[] = {
            "FES_PLA", "FES_MON", "FES_WEA", "FES_GAN",
            "FES_CRI", "FES_ACH", "FES_MIS", "FES_MSC"
        };
        for (auto menuItem = 0u; menuItem < 8u; menuItem++) {
            auto numStatLines = CStats::ConstructStatLine(99'999, menuItem);

            fprintf_s(file, "</font></strong></div></td> </tr> <tr align=\"left\" valign=\"top\" bgcolor=\"#000000\">  <td height=\"25\" colspan=\"2\"></td> </tr>\n");
            fprintf_s(file, "<tr align=\"left\" valign=\"top\"><td height=\"30\" bgcolor=\"#000000\"><font color=\"#009900\" size=\"4\" face=\"Arial, Helvetica, sans-serif\"><strong>\n");

            fprintf_s(file, "%s", GxtCharToUTF8(TheText.Get(strToPrint[menuItem]), 0u));
            fprintf_s(file, "</strong></font></td> <td width=\"500\" align=\"right\" valign=\"middle\" bgcolor=\"#000000\"> <div align=\"right\"><strong><font color=\"#FF0CCC\">\n");

            for (auto stat = 0; stat < numStatLines; stat++) {
                CStats::ConstructStatLine(stat, menuItem);

                auto str = GxtCharToUTF8(gGxtString, 0u);
                if (*str) {
                    fprintf_s(file, "</font></strong></div></td> </tr> <tr align=\"left\" valign=\"top\" bgcolor=\"#000000\">  <td height=\"10\" colspan=\"2\"></td> </tr>\n");
                }

                fprintf_s(file, "<tr align=\"left\" valign=\"top\"><td width=\"500\" height=\"22\" bgcolor=\"#555555\"><font color=\"#FFFFFF\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><strong>\n");
                if (*str) {
                    fprintf_s(file, "%s", str);
                } else {
                    fprintf_s(file, " ");
                }
                fprintf_s(file, "</strong></font></td> <td width=\"500\" align=\"right\" valign=\"middle\" bgcolor=\"#555555\"> <div align=\"right\"><strong><font color=\"#FFFFFF\">\n");
                auto val = GxtCharToUTF8(gGxtString2, 0u);
                auto valFormatted = (char*)val;

                // todo. xref: CStats::ConstructStatLine, PrintStats
                static auto& unk = StaticRef<uint16>(0xB794CC);
                if (unk) { // stat line formatted in percents?
                    // Como el original: atoi * 0.001f * 100.0f, y 100 si no es < 100 (NaN incluido); sprintf sin límite.
                    // fild/fmul/fmul del EXE (0x57E116..0x57E123, constantes 0x858CDC y 0x858628): el entero entra exacto en el x87
                    // (sin redondear antes a float) y cada producto se redondea a 24 bits (X87Math.cpp).
                    float percent = IntTimesTwoFloatsCpp(atoi(val), 0.001f, 100.0f);
                    if (!(percent < 100.0f)) {
                        percent = 100.0f;
                    }
                    std::sprintf(valFormatted, "%0.0f%%", (double)percent);
                }

                for (auto v = valFormatted; *v; v++) {
                    if (*v == '|') {
                        *v = -70; // double vertical bar
                    }
                }
                fprintf_s(file, "%s", valFormatted);
            }
        }

        // Como el original: cierre de la tabla y pie con enlaces (0x865AC8, 0x865900...0x865708).
        fprintf_s(file, "</font></strong></div></td> </tr> <tr align=\"left\" valign=\"top\" bgcolor=\"#000000\">  <td height=\"10\" colspan=\"2\"></td> </tr>\n");
        fprintf_s(file, "</table><br><table width=\"560\" border=\"0\"  align=\"center\" cellspacing=\"0\" cellpadding=\"5\"><tr align=\"center\" valign=\"middle\" bgcolor=\"#000000\">");
        fprintf_s(file, "<td><font color=\"#F0000C\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><a href=\"http://www.rockstargames.com/sanandreas\">rockstargames.com/sanandreas</a></font></td>\n");
        fprintf_s(file, "<td><font color=\"#F0000C\" size=\"2\" face=\"Arial, Helvetica, sans-serif\"><a href=\"http://www.rockstargames.com\">rockstargames.com</a></font></td>\n");
        fprintf_s(file, "<td><font color=\"#F0000C\" size=\"2\" face=\"Arial, Helvetica, sans-serif\">&nbsp;<a href=\"http://www.rockstarnorth.com\">rockstarnorth.com</a></font></td></tr>\n");
        fprintf_s(file, "</table>\n</body>\n");
        CFileMgr::CloseFile(file); // Como el original: fclose solo si se abrió
    }

    CFileMgr::SetDir("");
    m_HelperText             = FET_STS;
    m_nHelperTextFadingAlpha = 300;
}

// 0x57C490
// PS2 leftover?
void CMenuManager::SaveLoadFileError_SetUpErrorScreen() {
    switch (s_PcSaveHelper.error) {
    case C_PcSave::eErrorCode::E1:
    case C_PcSave::eErrorCode::E2:
    case C_PcSave::eErrorCode::E3:
        SwitchToNewScreen(SCREEN_SAVE_GAME_FAILED);
        break;
    case C_PcSave::eErrorCode::FAILED_TO_OPEN:
    case C_PcSave::eErrorCode::FAILED_TO_READ:
    case C_PcSave::eErrorCode::FAILED_TO_CLOSE:
        SwitchToNewScreen(SCREEN_SAVE_WRITE_FAILED);
        break;
    case C_PcSave::eErrorCode::SLOT_INVALID:
        SwitchToNewScreen(SCREEN_SAVE_FAILED_FILE_ERROR);
        break;
    case C_PcSave::eErrorCode::E8:
    case C_PcSave::eErrorCode::E9:
    case C_PcSave::eErrorCode::E10:
        SwitchToNewScreen(SCREEN_SAVE_GAME_DONE);
        break;
    default:
        return;
    }
}

// 0x57E240
void CMenuManager::DisplayHelperText(const char* key) {
    CFont::SetScale(StretchX(0.4f), m_nCurrentScreen == SCREEN_MAP ? StretchY(0.5f) : StretchY(0.6f));
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetOrientation(eFontAlignment::ALIGN_RIGHT);
    CFont::SetEdge(0);

    const auto x = StretchX(610.0f);
    auto y = SCREEN_HEIGHT - StretchY(10.0f); // Como el original: alto - StretchY (0x573410), no SCREEN_STRETCH_FROM_BOTTOM.
    const GxtChar* text;

    // 0x57E29D
    if (key) {
        text = TheText.Get(key);
        CFont::SetColor(MENU_TEXT_WHITE);
    } else {
        // 0x57E2E2
        uint8 alpha = 255;
        if (m_HelperText && m_HelperText != eHelperText::FET_APP) {
            if (CTimer::GetTimeInMSPauseMode() - m_HelperTextUpdatedTime > 10) {
                m_HelperTextUpdatedTime = CTimer::GetTimeInMSPauseMode();
                m_nHelperTextFadingAlpha -= 2;
            }
            if (m_nHelperTextFadingAlpha < 1) {
                // Como el original (0x57E312): escritura en línea, no llama a ResetHelperText
                m_HelperText             = HELPER_NONE;
                m_nHelperTextFadingAlpha = 300;
            }
            alpha = std::min(m_nHelperTextFadingAlpha, 255);
        }
        CFont::SetColor(CRGBA(MENU_TEXT_WHITE, alpha));

        switch (m_HelperText) {
        case FET_APP: text = TheText.Get("FET_APP"); break; // CLICK LMB / RETURN - APPLY NEW SETTING
        case FET_HRD: text = TheText.Get("FET_HRD"); break; // DEFAULT SETTINGS RESTORED
        case FET_RSO: text = TheText.Get("FET_RSO"); break; // ORIGINAL SETTING RESTORED
        case FEA_SCF: text = TheText.Get("FEA_SCF"); break; // FAILED TO SCAN USER TRACKS
        case FEA_SCS: text = TheText.Get("FEA_SCS"); break; // USER TRACKS SCANNED SUCCESSFULLY
        case FET_STS: text = TheText.Get("FET_STS"); break; // STATS SAVED TO 'STATS.HTML'
        default:
            switch (aScreens[m_nCurrentScreen].m_aItems[m_nCurrentScreenItem].m_nActionType) {
            case MENU_ACTION_BACK:             text = TheText.Get("FEH_BPO"); break; // CLICK LMB / RETURN - BACK
            case MENU_ACTION_MENU:
            case MENU_ACTION_CTRLS_JOYPAD:
            case MENU_ACTION_CTRLS_FOOT:
            case MENU_ACTION_CTRLS_CAR:        text = TheText.Get("FEH_JMP"); break; // CLICK LMB / RETURN - ENTER MENU
            case MENU_ACTION_USER_TRACKS_SCAN: text = TheText.Get("FEH_SNC"); break; // CLICK LMB / RETURN - SCAN USER TRACKS
            case MENU_ACTION_RESOLUTION:       text = TheText.Get("FET_MIG"); break; // LEFT / RIGHT / MOUSEWHEEL - ADJUST
            default:
                text = TheText.Get(m_nCurrentScreen ? "FET_MIG" : "FEH_SSA"); // CURSORS - MOVE~n~S - SAVE TO FILE
                if (m_nCurrentScreen == SCREEN_MAP) {
                    y = SCREEN_HEIGHT - StretchY(2.0f);
                }
                break;
            }
            break;
        }
    }

    CFont::PrintStringFromBottom(x, y, text);
}

// 0x57CD10
void CMenuManager::SetHelperText(eHelperText messageId) {
    m_HelperText             = messageId;
    m_nHelperTextFadingAlpha = 300;
}

// 0x57CD30
void CMenuManager::ResetHelperText() {
    m_HelperText             = HELPER_NONE;
    m_nHelperTextFadingAlpha = 300;
}

// 0x57C5E0
void CMenuManager::NoDiskInDriveMessage() {
    DoRWStuffStartOfFrame(0, 0, 0, 0, 0, 0, 0);
    if (RsGlobal.quit) {
        return;
    }
    MessageLoop();
    CPad::UpdatePads();
    MessageScreen("NO_PCCD", true, false);
    CFont::DrawFonts();
    DoRWStuffEndOfFrame();
    if (CPad::IsEscJustPressed()) {
        m_bQuitGameNoDVD = true;
        RsEventHandler(rsQUITAPP, NULL);
    }
}

// 0x579330
void CMenuManager::MessageScreen(const char* key, bool blackBackground, bool cameraUpdateStarted) {
    const CRect fullscreen = CRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!cameraUpdateStarted) {
        if (!RsCameraBeginUpdate(Scene.m_pRwCamera))
            return;

        if (blackBackground) {
            CSprite2d::DrawRect(fullscreen, MENU_BG);
        }
    }

    CSprite2d::SetRecipNearClip();
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    DefinedState2d();

    if (blackBackground) {
        CSprite2d::DrawRect(fullscreen, MENU_BG);
    }

    SmallMessageScreen(key);
    CFont::DrawFonts(); // Como el original: llama a 0x71A210 (DrawFonts)
}

// 0x574010
void CMenuManager::SmallMessageScreen(const char* key) {
    // Como el original: StretchX/StretchY van en línea con la constante ya dividida (valor / 640 o / 448)
    // que el compilador dejó en .rdata, sin pasar por CMenuManager::StretchX/StretchY.
    const auto StretchXK = [](float x, uint32 kBits) {
        return RsGlobal.maximumWidth == DEFAULT_SCREEN_WIDTH ? x : SCREEN_WIDTH * std::bit_cast<float>(kBits);
    };
    const auto StretchYK = [](float y, uint32 kBits) {
        return RsGlobal.maximumHeight == DEFAULT_SCREEN_HEIGHT ? y : SCREEN_HEIGHT * std::bit_cast<float>(kBits);
    };

    CFont::SetBackground(false, false);
    CFont::SetProportional(true);
    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetCentreSize(SCREEN_WIDTH * 0.0015625f * 430.0f); // Como el original: (ancho * 0.0015625f) * 430 sin comprobar 640 ni corregir aspecto
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetColor(HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY));
    CFont::SetDropShadowPosition(0);
    CFont::SetScale(StretchXK(0.56f, 0x3A656042u), StretchYK(1.0f, 0x3B124925u));

    // Como el original: los bordes derecho/inferior se restan del ancho/alto en float.
    DrawWindow({
            StretchXK(95.0f, 0x3E180000u),
            StretchYK(125.0f, 0x3E8EDB6Eu),
            SCREEN_WIDTH - StretchXK(95.0f, 0x3E180000u),
            SCREEN_HEIGHT - StretchYK(165.0f, 0x3EBC9249u)
        },
        nullptr,
        0,
        MENU_BG,
        false,
        true
    );

    // Como el original: GetNumberLines recibe (StretchX(320), StretchY(135)) y la Y final es entera (_ftol2).
    const auto lines = CFont::GetNumberLines(StretchXK(320.0f, 0x3F000000u), StretchYK(135.0f, 0x3E9A4925u), TheText.Get(key));
    int32 y;
    if (lines > 1) {
        y = (int32)(StretchYK(200.0f, 0x3EE49249u) - (float)lines * StretchYK(9.0f, 0x3CA49249u));
    } else {
        y = (int32)StretchYK(190.0f, 0x3ED92492u);
    }

    auto text = TheText.Get(key);
    if (!TheText.m_bCdErrorLoaded) {
        for (auto& k : { "NOCD", "OPENCD", "WRONGCD", "CDERROR" }) {
            // Como el original (0x574297..0x5742CF): copia cuando la clave es DISTINTA de alguna de las cuatro (jne), es decir, siempre
            if (_stricmp(k, key) != 0) {
                // Como el original: copia el texto de error del CD sobre la cadena GXT devuelta (0x718600).
                AsciiToGxtChar(TheText.m_szCdErrorText, const_cast<GxtChar*>(text));
                break; // Como el original: la cadena de || se detiene en la primera coincidencia
            }
        }
    }
    CFont::PrintString(StretchXK(320.0f, 0x3F000000u), (float)y, text);
}

//! NOTSA
void CMenuManager::SimulateGameLoad(bool newGame, uint32 slot) {
    m_bDontDrawFrontEnd     = newGame;
    m_SelectedSlot          = slot;
    CGame::bMissionPackGame = false;
    if (newGame) {
        DoSettingsBeforeStartingAGame();
    } else {
        m_nCurrentScreen = SCREEN_LOAD_FIRST_SAVE;
        m_CurrentlyLoading = true;
    } 
}

// NOTSA
void CMenuManager::SetBrightness(float brightness, bool arg2) {
    gamma.SetGamma(brightness / 512.0f, arg2);
}
