/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Win
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
* This file contains WndProc and related functions
*/

#include "StdInc.h"

#include "winincl.h"
#include <rwplcore.h>
#include <Dbt.h>
#include <dshow.h>

#include <platform/platform.h>
#include "PostEffects.h"
#include "AEAudioHardware.h"
#include "VideoMode.h"
#include "VideoPlayer.h"
#include "WinInput.h"
#include "WinPlatform.h"
#include "Gamma.h"
#include "X87Math.h"

// El EXE lee y escribe estas globales con ancho de dword (mov eax, [mem]; mov dword ptr [mem], 1); los StaticRef<bool> de arriba solo cubren su primer byte.
static auto& RwInitializedDword        = StaticRef<uint32>(0xC920E8);
static auto& ForegroundAppDword        = StaticRef<uint32>(0x8D621C);
static auto& WindowedDword             = StaticRef<uint32>(0xC920CC);
static auto& CameraResizeRejectedDword = StaticRef<uint32>(0xC9246C); // 1 si RsEventHandler(rsCAMERASIZE) no pudo fijar el tamaño pedido (solo la escribe este procedimiento)

// 0x747EB0 (WNDPROC __stdcall, ret 0x10)
// Reconstrucción exacta del EXE: mismos mensajes, mismo orden de llamadas y mismos retornos. Los llamadores son user32 (la única referencia de código es Win32_InitApplication),
// que solo lee EAX; ECX/EDX no se leen y EBX/ESI/EDI/EBP los conserva el compilador. Las API de Win32 se llaman por sus versiones ANSI, como la tabla de importación del EXE.
LRESULT CALLBACK __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
    case WM_CLOSE: { // 0x748033
        ClipCursor(nullptr);
        WinInput::Shutdown();
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: { // 0x747EFA
        // Tamaño nuevo (LOWORD/HIWORD sin signo)
        const int width = LOWORD(lParam), height = HIWORD(lParam);
        RwRect wndsz{
            .x = 0,     .y = 0,
            .w = width, .h = height,
        };

        // ¿Nada que hacer? (RwInitialized se lee como dword)
        if (!RwInitializedDword || height <= 0 || width <= 0) {
            return 0;
        }

        // Intenta redimensionar la cámara
        RsEventHandler(rsCAMERASIZE, &wndsz);

        // ¿Ha fallado? El EXE exige que difieran ancho Y alto (en otro caso limpia el indicador y sale)
        if (wndsz.w == width || wndsz.h == height) {
            CameraResizeRejectedDword = 0; // 0x747F86
            return 0;
        }
        CameraResizeRejectedDword = 1; // 0x747F48
        ReleaseCapture();

        // Como el original: el WINDOWPLACEMENT no se inicializa (length incluido) y se consulta la ventana recibida, no PSGLOBAL(window)
        WINDOWPLACEMENT wndpl;
        GetWindowPlacement(hWnd, &wndpl);
        if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
            SendMessageA(hWnd, WM_WINDOWPOSCHANGED, 0, 0);
        }
        return 0;
    }
    case WM_ACTIVATE: { // 0x747F98
        // Como el original: solo cuenta la palabra baja de wParam (la alta es el indicador de minimizada)
        if (LOWORD(wParam) == 0) {
            CAudioEngine::PauseAllSounds();
            ForegroundAppDword = 0;
            // 0x747FAA: _ftol2(1000.0f / RsGlobal.frameLimit), con la división x87 a 24 bits
            SetTimer(hWnd, 1, WndProcTimerPeriodCpp(RsGlobal.frameLimit), nullptr);
            RsEventHandler(rsACTIVATE, nullptr);
        } else {
            CAudioEngine::ResumeAllSounds();
            ShowCursor(FALSE);
            RsEventHandler(rsACTIVATE, (void*)1);
            ForegroundAppDword = 1; // después del evento, como el EXE
        }

        //> 0x74800C Clear pads
        for (auto& pad : CPad::Pads) {
            pad.Clear(false, true);
        }
        break; // El EXE sigue hasta DefWindowProc
    }
    case WM_SETFOCUS: { // 0x748063
        // 0x53BC60 (OnSetFocus del menú), en línea
        if (!FrontEndMenuManager.m_bMainMenuSwitch && !FrontEndMenuManager.m_bMenuActive) {
            FrontEndMenuManager.m_bActivateMenuNextFrame = true;
        }
        ForegroundAppDword = 1;
        break;
    }
    case WM_KILLFOCUS: { // 0x748054
        ForegroundAppDword = 0;
        break;
    }
    case WM_ACTIVATEAPP: { // 0x748077
        //> Gamma (si ha cambiado): wParam completo elige la tabla; el dispositivo puede ser nulo
        if (gbGammaChanged) {
            if (const auto dev = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice()) {
                dev->SetGammaRamp(0, D3DSGR_CALIBRATE, wParam ? &CGamma::ms_GammaTable : &CGamma::ms_SavedGamma);
            }
        }

        // Como el original: el resto usa solo la palabra baja de wParam (si)
        const bool wndBeingActivated = LOWORD(wParam) != 0;
        switch (gGameState) {
        case GAME_STATE_PLAYING_LOGO:
        case GAME_STATE_PLAYING_INTRO: {
            // Como el original: el estado del filtro vive en la ranura de lParam, de modo que si GetState no lo escribe se compara con lParam
            OAFilterState state = (OAFilterState)lParam;
            VideoPlayer::GetMediaControl()->GetState(10, &state);
            if (!wndBeingActivated) { // Se desactiva la ventana: pausa el vídeo
                if (state == State_Running) {
                    if (const auto mc = VideoPlayer::GetMediaControl()) {
                        if (mc->Pause() != 0) { // 0x748106: cualquier valor distinto de S_OK (S_FALSE incluido)
                            OutputDebugStringA("Failed to pause the MPEG");
                        } else {
                            VideoPlayer::WaitState(State_Paused);
                        }
                    }
                }
            } else {
                VideoPlayer::UpdateWindow();
                if (state != State_Running) {
                    if (const auto mc = VideoPlayer::GetMediaControl()) {
                        if (mc->Run() != 0) { // 0x74813C
                            OutputDebugStringA("Failed to run the MPEG");
                        } else {
                            VideoPlayer::WaitState(State_Running);
                            SetFocus(PSGLOBAL(window));
                        }
                    }
                }
            }
            break;
        }
        case GAME_STATE_INITIAL: { // 0x748166
            if (!wndBeingActivated && PSGLOBAL(fullScreen)) {
                WindowedDword = 1;
            }
            break;
        }
        }

        //> 0x74817E Clear pads
        for (auto& pad : CPad::Pads) {
            pad.Clear(false, true);
        }
        return 0;
    }
    case WM_SETCURSOR: { // 0x7481CD
        ShowCursor(FALSE);
        SetCursor(nullptr);
        break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP: { //< 0x7481E2 / 0x74822C - wParam is a `VK_` (virtual key), lParam are the flags (See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keyup)
        // El EXE usa el mismo RsKeyCodes para GTATranslateKey y GTATranslateShiftKey
        RsKeyCodes ck;
        // El EXE comprueba solo AL (test al, al): GTATranslateKey (0x747820) devuelve `mov al, 1` con basura en los 24 bits altos de EAX
        if (static_cast<uint8>(GTATranslateKey(&ck, lParam, wParam))) {
            RsKeyboardEventHandler(
                (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
                    ? rsKEYDOWN
                    : rsKEYUP,
                &ck
            );
        }
        if (wParam == VK_SHIFT) {
            GTATranslateShiftKey(&ck); // El EXE empuja un segundo argumento (1 al pulsar, 0 al soltar) que la función no lee
        }
        return 0;
    }
    case WM_TIMER: // 0x748048: devuelve 0 sin pasar por DefWindowProc (que ejecutaría el TIMERPROC)
        return 0;
    case WM_MOUSEMOVE: { //< 0x748313
        FrontEndMenuManager.m_nMousePosWinX = GET_X_LPARAM(lParam);
        FrontEndMenuManager.m_nMousePosWinY = GET_Y_LPARAM(lParam);
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: { // 0x7482FC
        SetCapture(hWnd);
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: { // 0x748338
        ReleaseCapture();
        return 0;
    }
    case WM_MOUSEWHEEL:
        return 0;
    case WM_SIZING: { // 0x7482A5
        if (RwInitializedDword && gGameState == GAME_STATE_IDLE) {
            RsEventHandler(rsIDLE, (void*)TRUE);
        }

        // Como el original: solo SWP_NOMOVE (el tamaño sale del RECT recibido)
        const auto wndrect = reinterpret_cast<const RECT*>(lParam);
        SetWindowPos(
            hWnd,
            HWND_TOP,
            0,                              0,
            wndrect->right - wndrect->left, wndrect->bottom - wndrect->top,
            SWP_NOMOVE
        );

        CPostEffects::SetupBackBufferVertex();

        return 0;
    }
    case WM_DEVICECHANGE: { //> 0x74834A - Handle AudioHardware DVD removal
        if (wParam != DBT_DEVICEREMOVECOMPLETE) {
            break;
        }
        // Mismo orden de comprobaciones que el EXE (cada una llama a un colaborador distinto)
        const auto eparams = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);
        if (eparams->dbch_devicetype != DBT_DEVTYP_VOLUME) {
            break;
        }
        if (!AEAudioHardware.m_bInitialised || !AEAudioHardware.IsStreamingFromDVD()) {
            break;
        }
        const auto idev = reinterpret_cast<PDEV_BROADCAST_VOLUME>(lParam);
        if ((idev->dbcv_flags & DBTF_MEDIA) == 0) { // Not a media drive?
            break;
        }
        const auto dvletter = (uint8)AEAudioHardware.GetDVDDriveLetter(); // sin signo (cmp al, 41h ; jb)
        if (dvletter < 'A' || (idev->dbcv_unitmask & (1u << ((dvletter - 'A') & 31))) == 0) { // shl enmascara el contador a 5 bits
            break;
        }
        OutputDebugStringA("About to check CD drive...");
        CTimer::SetCodePause(true);
        if (CCutsceneMgr::ms_running == 1) { // cmp byte ptr [0xB5F851], 1
            CCutsceneMgr::SkipCutscene();
        }
        // Como el original: el mensaje «sin disco» se muestra una vez ANTES de consultar la unidad
        FrontEndMenuManager.NoDiskInDriveMessage();
        bool inserted = false;
        if (!FrontEndMenuManager.m_bQuitGameNoDVD) {
            while (true) {
                if (AEAudioHardware.CheckDVD()) {
                    inserted = true;
                    break;
                }
                FrontEndMenuManager.NoDiskInDriveMessage();
                if (FrontEndMenuManager.m_bQuitGameNoDVD) {
                    break;
                }
            }
        }
        if (inserted) {
            OutputDebugStringA("GTA Audio DVD has been inserted");
            CTimer::SetCodePause(false);
        } else { // Como el original: al salir sin disco CodePause se queda activo
            OutputDebugStringA("Exiting game as Audio CD was not inserted");
        }
        break;
    }
    case WM_GRAPHNOTIFY: { //< 0x74842A - Dispatched from VideoPlayer::Play
        switch (gGameState) {
        case GAME_STATE_PLAYING_INTRO:
        case GAME_STATE_PLAYING_LOGO: {
            VideoPlayer::OnGraphNotify();
            break;
        }
        }
        break;
    }
    }
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

void InjectHooksWndProcStuff() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Win");

    RH_ScopedGlobalInstall(__MainWndProc, 0x747EB0);
}
