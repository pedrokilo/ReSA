#include "StdInc.h"

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include "SDLWrapper.hpp"
#include "VideoPlayer.h"

namespace notsa::SDLWrapper {
namespace {
SDL_Window* window{};
HWND nativeWindow{};
WNDPROC originalProcedure{};
bool pumping{};
constexpr UINT_PTR EventTimer = 0x475441;

LRESULT CALLBACK PlatformWindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    // This procedure sits below SDL's foreign-window procedure. The ImGui debug
    // menu no longer exists, so every message reaches the native game procedure.
    if (message == WM_GRAPHNOTIFY && (gGameState == GAME_STATE_PLAYING_LOGO || gGameState == GAME_STATE_PLAYING_INTRO)) {
        VideoPlayer::OnGraphNotify();
        return 0;
    }
    const auto previous = originalProcedure;
    if (message == WM_NCDESTROY) {
        KillTimer(hwnd, EventTimer);
        nativeWindow = nullptr;
    }
    return CallWindowProcA(previous, hwnd, message, wparam, lparam);
}

void CALLBACK PumpEvents(HWND, UINT, UINT_PTR, DWORD) {
    ProcessEvents();
}
}

SDL_Window* GetWindow() {
    return window;
}

bool Initialize() {
    if (window) {
        return true;
    }
    if (!RsGlobal.ps || !PSGLOBAL(window) || !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC)) {
        return false;
    }
    nativeWindow = PSGLOBAL(window);
    originalProcedure = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(nativeWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(PlatformWindowProcedure)));
    const auto properties = SDL_CreateProperties();
    SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, nativeWindow);
    window = SDL_CreateWindowWithProperties(properties);
    SDL_DestroyProperties(properties);
    if (!window) {
        SetWindowLongPtrA(nativeWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalProcedure));
        nativeWindow = nullptr;
        return false;
    }
    // WM_TIMER is dispatched by either message loop, including native WinMain.
    // No game hook needs to remain locked just to keep the SDL queue alive.
    if (!SetTimer(nativeWindow, EventTimer, 10, PumpEvents)) {
        Terminate();
        return false;
    }
    NOTSA_LOG_DEBUG("SDL attached to native HWND; platform block size={}", sizeof(psGlobalType));
    return true;
}

void Terminate() {
    const auto hwnd = nativeWindow;
    if (hwnd) {
        KillTimer(hwnd, EventTimer);
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (hwnd && IsWindow(hwnd)) {
        SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalProcedure));
    }
    nativeWindow = nullptr;
    originalProcedure = nullptr;
    SDL_Quit();
}

void ProcessEvents() {
    if (!window || pumping) {
        return;
    }
    pumping = true;
    for (SDL_Event event; SDL_PollEvent(&event);) {
        if (event.type == SDL_EVENT_QUIT) {
            RsGlobal.quit = true;
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            FrontEndMenuManager.m_nMousePosWinX = static_cast<int32>(event.motion.x);
            FrontEndMenuManager.m_nMousePosWinY = static_cast<int32>(event.motion.y);
        }
    }
    // The game receives keyboard input from its Windows procedure and mouse /
    // controller state from DirectInput.
    pumping = false;
}
}
#endif
