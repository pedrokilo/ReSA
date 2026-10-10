/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Win
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
* This file contains WinMain and related functions
*/

#include "StdInc.h"

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include "SDLWrapper.hpp"
#endif

#include "LoadingScreen.h"
#include "ControllerConfigManager.h"
#include "Gamma.h"

#include "VideoPlayer.h"
#include "VideoMode.h"
#include "WinInput.h"
#include "WinPlatform.h"
#include "WndProc.h"
#include "WindowedMode.hpp"

#include <InjectHooksMain.h>
#include "extensions/Configs/FastLoader.hpp"
#include <extensions/CommandLine.h>
#include <extensions/debug.hpp>

constexpr auto NO_FOREGROUND_PAUSE = true;

// 0x747300
char* getDvdGamePath() {
    SetErrorMode(SEM_FAILCRITICALERRORS);

    size_t bufferSize = GetLogicalDriveStringsA(0, nullptr);
    const auto drivesBuffer = new char[bufferSize];

    GetLogicalDriveStringsA(bufferSize, drivesBuffer);
    assert(bufferSize > 0);

    char drivePath[8] = {};
    char volumeName[MAX_PATH + 1] = {};

    for (const auto* drive = drivesBuffer; *drive; drive += strlen(drive) + 1) {
        strcpy_s(drivePath, sizeof(drivePath), drive);

        if (GetDriveTypeA(drivePath) != DRIVE_CDROM) {
            continue;
        }

        if (!GetVolumeInformationA(drivePath, volumeName, sizeof(volumeName), nullptr, nullptr, nullptr, nullptr, 0)) {
            continue;
        }

        if (strcmp(volumeName, "GTA_SAN_ANDREAS") == 0) {
            const auto result = new char[strlen(drivePath) + 1];
            strcpy_s(result, strlen(drivePath) + 1, drivePath);
            delete[] drivesBuffer;
            return result;
        }
    }

    delete[] drivesBuffer;
    return nullptr;
}

// 0x746870
void MessageLoop() {
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE | PM_NOYIELD)) {
        if (msg.message == WM_QUIT) {
            RsGlobal.quit = true;
        } else {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }
}

// 0x7486A0
bool Win32_InitApplication(HINSTANCE hInstance) {
    WNDCLASS windowClass      = { 0 };
    windowClass.style         = CS_BYTEALIGNWINDOW;
#ifdef NOTSA_USE_SDL3
    windowClass.lpfnWndProc   = reinterpret_cast<WNDPROC>(0x747EB0);
#else
    windowClass.lpfnWndProc   = __MainWndProc;
#endif
    windowClass.hInstance     = hInstance;
    windowClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
    windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = APP_CLASS;
    return RegisterClass(&windowClass);
}

// 0x745560
auto Win32_InitInstance(HINSTANCE hInstance) {
    RECT rect = { 0, 0, RsGlobal.maximumWidth, RsGlobal.maximumHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    return CreateWindowEx(
        0,
        APP_CLASS,
        RsGlobal.appName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
}

// 0x7468E0
bool IsAlreadyRunning() {
    CreateEvent(nullptr, false, true, APP_CLASS);
    if (GetLastError() != ERROR_ALREADY_EXISTS) {
        return false;
    }

    const auto fgwnd = FindWindow(APP_CLASS, RsGlobal.appName);
    SetForegroundWindow(fgwnd ? fgwnd : PSGLOBAL(window)); // Como el original: compara con NULL, no con INVALID_HANDLE_VALUE
    
    return true;
};

// 0x746060
bool IsForegroundApp() {
    return StaticRef<uint32>(0x8D621C) != 0; // Como el original: ForegroundApp es un dword en el EXE
}

// Code from winmain, 0x748DCF
bool ProcessGameLogic(INT nCmdShow) {
    if (RsGlobal.quit || FrontEndMenuManager.m_bStartGameLoading) {
        return false;
    }

#ifdef NOTSA_USE_SDL3
    notsa::SDLWrapper::ProcessEvents();
#else
    // Process Window messages
    if (MSG msg; PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE | PM_NOYIELD)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return true;
    }
#endif

    // Procesar mensajes antes de pausar permite recuperar el foco. La ruta original de Idle necesita
    // esta pausa también cuando AppEventHandler está desactivado y se ejecuta el dispatcher del EXE.
    if ((!NO_FOREGROUND_PAUSE && !ForegroundApp) || ShouldPauseOriginalIdleInBackground()) {
        if (isForeground) {
            isForeground = false;
        }
        Sleep(100);
        return true;
    }

    FrameMark;

    // TODO: Move this out from here (It's not platform specific at all)
    switch (gGameState) {
    case GAME_STATE_INITIAL: {
        const auto ProcessSplash = [](eLoadingLogo id) {
            CLoadingScreen::LoadSplashes(true, id);
            CLoadingScreen::Init(true, true);
            CLoadingScreen::DoPCTitleFadeIn();
            CLoadingScreen::DoPCTitleFadeOut();
            CLoadingScreen::Shutdown();
        };
        if (!g_FastLoaderConfig.NoEAX) {
            ProcessSplash(eLoadingLogo::EAX);
        }
        if (!g_FastLoaderConfig.NoNVidia) {
            ProcessSplash(eLoadingLogo::NVIDIA);
        }
        ChangeGameStateTo(GAME_STATE_LOGO);
        break;
    }
    case GAME_STATE_LOGO: {
        if (!g_FastLoaderConfig.NoLogo) {
            if (!Windowed) {
                VideoPlayer::Play(nCmdShow, "movies\\Logo.mpg");
            }
        }
        ChangeGameStateTo(g_FastLoaderConfig.NoLogo ? GAME_STATE_TITLE : GAME_STATE_PLAYING_LOGO);
        break;
    }
    case GAME_STATE_PLAYING_LOGO:
    case GAME_STATE_PLAYING_INTRO: { // 0x748B17
        CPad::UpdatePads();
        auto* pad = CPad::GetPad();
        if (   Windowed
            || ControlsManager.GetJoyButtonJustDown()
            || pad->NewState.CheckForInput()
            || CPad::IsMouseLButtonPressed()
            || CPad::IsEnterJustPressed()
            || pad->IsStandardKeyJustPressed(VK_SPACE)
            || CPad::IsMenuKeyJustPressed()
            || CPad::IsTabJustPressed()
        ) {
            ChangeGameStateTo([] {
                switch (gGameState) {
                case GAME_STATE_PLAYING_LOGO:  return GAME_STATE_TITLE;
                case GAME_STATE_PLAYING_INTRO: return GAME_STATE_FRONTEND_LOADING;
                default:                       NOTSA_UNREACHABLE();
                }
            }());
        }
        break;
    }
    case GAME_STATE_TITLE: {
        if (!g_FastLoaderConfig.NoTitleOrIntro) {
            VideoPlayer::Shutdown();
            VideoPlayer::Play(nCmdShow, FrontEndMenuManager.GetMovieFileName());
        }
        ChangeGameStateTo(g_FastLoaderConfig.NoTitleOrIntro ? GAME_STATE_FRONTEND_LOADING : GAME_STATE_PLAYING_INTRO);
        break;
    }
    case GAME_STATE_FRONTEND_LOADING: {
        VideoPlayer::Shutdown();
        CLoadingScreen::Init(true, false);
        if (!g_FastLoaderConfig.NoCopyright) {
            CLoadingScreen::DoPCTitleFadeIn();
        }
        if (!CGame::InitialiseEssentialsAfterRW()) {
            RsGlobal.quit = true;
        }
        CGame::InitialiseCoreDataAfterRW();
        ChangeGameStateTo(GAME_STATE_FRONTEND_LOADED);
        anisotropySupportedByGFX = (((const D3DCAPS9*)RwD3D9GetCaps())->RasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0; // todo: func

#ifdef NOTSA_WINDOWED_MODE
        RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ);
#endif

        break;
    }
    case GAME_STATE_FRONTEND_LOADED: {
        FrontEndMenuManager.m_bActivateMenuNextFrame = true;
        FrontEndMenuManager.m_bMainMenuSwitch = true;
        if (IsVMNotSelected) {
            FrontEndMenuManager.m_nPrefsVideoMode = FrontEndMenuManager.m_nDisplayVideoMode = gCurrentVideoMode;
            IsVMNotSelected = false;
        }
        ChangeGameStateTo(GAME_STATE_FRONTEND_IDLE);
        if (g_FastLoaderConfig.NoCopyright) {
            CLoadingScreen::SkipCopyrightSplash();
        } else {
            CLoadingScreen::DoPCTitleFadeOut();
        }
        break;
    }
    case GAME_STATE_FRONTEND_IDLE: { // 0x748CB2
        WINDOWPLACEMENT wndpl{ .length = sizeof(WINDOWPLACEMENT) };
        VERIFY(GetWindowPlacement(PSGLOBAL(window), &wndpl));
        if (g_FastLoaderConfig.ShouldLoadSaveGame()) {
            RsEventHandler(rsFRONTENDIDLE, nullptr); // We need to still run the frontend processing once because it has some important stuff
            if ((GetAsyncKeyState(g_FastLoaderConfig.SkipSaveGameLoadKey) & 0xF000) == 0) {
                g_FastLoaderConfig.StartGame(g_FastLoaderConfig.SaveGameToLoad); // Load game
            }
            g_FastLoaderConfig.TriedLoadingSaveGame = true;
        } else if (g_FastLoaderConfig.RenderAtAllTimes || wndpl.showCmd != SW_SHOWMINIMIZED) {
            RsEventHandler(rsFRONTENDIDLE, nullptr);
        }
        if (FrontEndMenuManager.m_bMenuActive && !FrontEndMenuManager.m_bLoadingData) {
            break;
        }
        ChangeGameStateTo(GAME_STATE_LOADING_STARTED);
        if (!FrontEndMenuManager.m_bLoadingData) {
            break;
        }
        NOTSA_SWCFALLTHRU; // Fall down and start loading
    }
    case GAME_STATE_LOADING_STARTED: {
        if (!g_FastLoaderConfig.NoLoadingTune) {
            AudioEngine.StartLoadingTune();
        }

        InitialiseGame();
        ChangeGameStateTo(GAME_STATE_IDLE);
        FrontEndMenuManager.m_bMainMenuSwitch = false;

        AudioEngine.InitialisePostLoading();

        break;
    }
    case GAME_STATE_IDLE: {
        if (!RwInitialized)
            break;

        auto v9_1 = 1000.0f / (float)RsGlobal.frameLimit;
        auto v9_2 = (float)CTimer::GetCurrentTimeInCycles() / (float)CTimer::GetCyclesPerMillisecond();
        if (!FrontEndMenuManager.m_bPrefsFrameLimiter && CReplay::Mode != eReplayMode::MODE_PLAYBACK && !AudioEngine.IsBeatInfoPresent() || v9_1 < v9_2) {
            RsEventHandler(rsIDLE, (void*)true);
        }
        break;
    }
    }

    if (!isForeground) {
        isForeground = true;
    }

    return true;
}

// Code from winmain, 0x7489FB
void MainLoop(INT nCmdShow) {
    bool isNewGameFirstTime = true;
    while (true) {
        RwInitialized = true;

        // Useless, and annoying
        //RwV2d pos{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        //RsMouseSetPos(&pos);

        gamma.Init();

        // Game logic main loop
        while (ProcessGameLogic(nCmdShow));

        // 0x748DDA
        RwInitialized = false;
        FrontEndMenuManager.UnloadTextures();
        if (!FrontEndMenuManager.m_bStartGameLoading) {
            break;
        }

        // load game
        CCheat::ResetCheats();
        CTimer::Stop();

        if (FrontEndMenuManager.m_bLoadingData) {
            CGame::ShutDownForRestart();
            CGame::InitialiseWhenRestarting();
            FrontEndMenuManager.m_bLoadingData = false;
        } else if (isNewGameFirstTime) {
            CTimer::Stop();
            ChangeGameStateTo(
                FrontEndMenuManager.m_nGameState != 1
                    ? GAME_STATE_LOADING_STARTED
                    : GAME_STATE_FRONTEND_LOADED
            );
        } else {
            CCheat::ResetCheats();
            CGame::ShutDownForRestart();
            CTimer::Stop();
            CGame::InitialiseWhenRestarting();
        }

        isNewGameFirstTime = false;
        FrontEndMenuManager.m_nGameState = 0;
        FrontEndMenuManager.m_bStartGameLoading = false;
    }

}

// 0x748710
INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {
    const notsa::ScopeGuard shutdownLogging{[] {
        // Windows terminates other threads before DLL detach; join logging workers now.
        if (notsa::Logging::HasInstance()) {
            notsa::Logging::GetSingleton().Shutdown();
        }
    }};
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);
    if (IsAlreadyRunning()) {
        return false;
    }

    if (rsEVENTERROR == RsEventHandler(rsINITIALIZE, nullptr)) {
        return false;
    }

    if (!Win32_InitApplication(instance)) {
        NOTSA_LOG_ERR("Window class registration failed: {}", GetLastError());
        return false;
    }

    char** argv = __argv;
    int    argc = __argc;
    for (int i = 0; i < argc; i++) {
        RsEventHandler(rsPREINITCOMMANDLINE, argv[i]);
    }

    // Dirty fix for crash in crt when trying to free cmd line args
    // Yeah we leak memory, but who cares
    // Crash was in `uninitialize_allocated_io_buffers` (`C:\Program Files (x86)\Windows Kits\10\Source\<Win 10 SDK Version>\ucrt\internal\initialization.cpp`)
    __argc  = 0;
    __argv  = nullptr;
    __wargv = nullptr;

    PSGLOBAL(window) = Win32_InitInstance(instance);

    if (!PSGLOBAL(window)) {
        return false;
    }
    PSGLOBAL(instance) = instance; // Not used anywhere, just set here

    // 0x7487CF
    VERIFY(WinInput::Initialise());
    ControlsManager.ReinitControls();

    // 0x748847
    if (RsEventHandler(rsRWINITIALIZE, PSGLOBAL(window)) == rsEVENTERROR) {
        DestroyWindow(PSGLOBAL(window));
        RsEventHandler(rsTERMINATE, nullptr);
        return false;
    }

    // 0x7488EE
    for (auto i = 0; i < argc; i++) {
        RsEventHandler(rsCOMMANDLINE, argv[i]);
    }

#ifndef NOTSA_USE_SDL3
    if (MultipleSubSystems || PSGLOBAL(fullScreen)) {
        SetWindowLongPtr(PSGLOBAL(window), GWL_STYLE, (LONG_PTR)WS_POPUP);
        SetWindowPos(PSGLOBAL(window), nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
#endif

    RwRect rect{ 0, 0, RsGlobal.maximumWidth, RsGlobal.maximumHeight };
    RsEventHandler(rsCAMERASIZE, &rect);

    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0u, nullptr, 2u);
    SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 0u, nullptr, 2u);
    SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 0u, nullptr, 2u);
    STICKYKEYS pvParam { .cbSize = sizeof(STICKYKEYS) };
    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &pvParam, 2u);
    STICKYKEYS pvParam1 = { .cbSize = sizeof(STICKYKEYS), .dwFlags = SKF_TWOKEYSOFF };
    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &pvParam1, 2u);

    UpdateWindow(PSGLOBAL(window));
    ShowWindow(PSGLOBAL(window), nCmdShow);

    // 0x748995
    CFileMgr::SetDirMyDocuments();
    if (auto* file = CFileMgr::OpenFile("gta_sa.set", "rb")) {
        if (!ControlsManager.LoadSettings(file)) {
            ControlsManager.ReinitControls();
        }
        CFileMgr::CloseFile(file);
    }
    CFileMgr::SetDir("");

    SetErrorMode(SEM_FAILCRITICALERRORS);

    // 0x7489FB
    MainLoop(nCmdShow);

    // if game is loaded, shut it down
    if (gGameState == GAME_STATE_IDLE) {
        CGame::Shutdown();
    }

    // now quit 0x748E75
    AudioEngine.Shutdown();
#ifdef NOTSA_USE_SDL3
    // NOTSA: libera SDL mientras existen el dispositivo D3D, la ventana nativa y los hilos (antes lo hacía el
    // destructor del menú ImGui, que ya no se crea).
    notsa::SDLWrapper::Terminate();
#endif
    FreeVideoModeList();
    RsEventHandler(rsRWTERMINATE, nullptr);
    DestroyWindow(PSGLOBAL(window));
    RsEventHandler(rsTERMINATE, nullptr);
    free(argv);
    ShowCursor(true);

    SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &pvParam, 2u);
    SystemParametersInfo(SPI_SETPOWEROFFACTIVE, 1u, nullptr, 2u); // TODO: GUID_VIDEO_POWERDOWN_TIMEOUT
    SystemParametersInfo(SPI_SETLOWPOWERACTIVE, 1u, nullptr, 2u);
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1u, nullptr, 2u);
    // nullsub_0x72F3C0()
    SetErrorMode(0);

    return 0; // Msg.wParam
}

#ifdef NOTSA_STANDALONE
INT WINAPI WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {
    notsa::debug::DisplayConsole();
    CommandLine::Load(__argc, __argv);
    if (CommandLine::s_WaitForDebugger) {
        notsa::debug::WaitForDebugger();
    }
#ifdef NOTSA_STANDALONE_DUMP_HOOKS_ONLY
    NOTSA_LOG_INFO("Dumping hooks only, no memory writing will be performed");
    if (CommandLine::s_DumpHooksPath.empty()) {
        NOTSA_LOG_ERR("No path provided for dumping hooks, use `--dump-hooks-to` CLI argument");
        return 1;
    }
    InjectHooksMain(GetModuleHandle(nullptr)); // this will call injecthooks which then ends up dumping the data
    return 0;
#else
    NOTSA_LOG_ERR("This executable is meant to be used for dumping hooks only, see `NOTSA_STANDALONE_DUMP_HOOKS_ONLY` option");
    return 1;
#endif
}
#endif

void InjectWinMainStuff() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Win");

    RH_ScopedGlobalInstall(IsForegroundApp, 0x746060);
    RH_ScopedGlobalInstall(IsAlreadyRunning, 0x7468E0);

    // WinMain is selected at startup; MessageLoop can also be switched at runtime.
    RH_ScopedGlobalInstall(NOTSA_WinMain, 0x748710);
    RH_ScopedGlobalInstall(MessageLoop, 0x746870);

#ifndef NOTSA_USE_SDL3
    RH_ScopedGlobalInstall(Win32_InitApplication, 0x7486A0);
    RH_ScopedGlobalInstall(Win32_InitInstance, 0x745560, {.locked = true});
#endif
}
