/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: VideoPlayer
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "VideoPlayer.h"

#include "dshow.h"
#pragma comment(lib, "strmiids.lib") // Class identifiers (CLSIDs) and interface identifiers (IIDs).

namespace VideoPlayer {

auto& pvGraphBuilder = StaticRef<IGraphBuilder*>(0xC920D4);
auto& pvMediaControl = StaticRef<IMediaControl*>(0xC920D8);
auto& pvMediaEvent = StaticRef<IMediaEventEx*>(0xC920DC);
auto& pvVideoWindow = StaticRef<IVideoWindow*>(0xC920E0);
auto& pvMediaSeeking = StaticRef<IMediaSeeking*>(0xC920E4);

void InjectHooks() {
    RH_ScopedNamespace(VideoPlayer);
    RH_ScopedCategory("App");

    RH_ScopedInstall(Shutdown, 0x7467B0);
    RH_ScopedInstall(WaitState, 0x745F90);
    RH_ScopedInstall(OnGraphNotify, 0x745FB0);
    RH_ScopedInstall(UpdateWindow, 0x7466D0);
    RH_ScopedInstall(Release, 0x746740);
    RH_ScopedInstall(Play, 0x747660);
}

// 0x746740
void Release() {
#define RELEASE(p) if (p) { (p)->Release(); }  (p) = nullptr;

    RELEASE(pvMediaEvent);
    RELEASE(pvMediaSeeking);
    RELEASE(pvMediaControl);
    RELEASE(pvVideoWindow);
    RELEASE(pvGraphBuilder);

#undef RELEASE
}

// 0x7467B0
void Shutdown() {
    if (pvMediaControl) {
        pvMediaControl->Stop();
    }
    Release();
}

// 0x745F90
void WaitState(int32 state) {
    OAFilterState curState;
    do {
        pvMediaControl->GetState(10, &curState);
    }
    while (state != curState);
}

// 0x745FB0
// Como el original: devuelve el HRESULT del último FreeEventParams (0 si no hubo eventos)
long OnGraphNotify() {
    LONG code = 0;
    LONG_PTR param1 = 0, param2 = 0;
    HRESULT hr = 0;

    while (!FAILED(pvMediaEvent->GetEvent(&code, &param1, &param2, 0))) {
        hr = pvMediaEvent->FreeEventParams(code, param1, param2);
        if (code != EC_COMPLETE) {
            continue;
        }
        // Possibly advance game state
        ChangeGameStateTo([] {
            switch (gGameState) {
            case GAME_STATE_PLAYING_LOGO:  return GAME_STATE_TITLE;
            case GAME_STATE_PLAYING_INTRO: return GAME_STATE_FRONTEND_LOADING;
            }
            return (eGameState)gGameState; // No change
        }());
        pvMediaEvent->SetNotifyWindow(NULL, 0, 0);
    }
    return hr;
}

// 0x7466D0
// Como el original: devuelve el HRESULT de la última llamada a IVideoWindow
long UpdateWindow() {
    RECT rect;
    GetClientRect(PSGLOBAL(window), &rect);
    HRESULT hr;

    if (FAILED(hr = pvVideoWindow->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvVideoWindow->SetWindowPosition() failed/n", hr);
        return hr;
    }

    // Como el original: SetFocus se llama aunque put_MessageDrain falle
    if (FAILED(hr = pvVideoWindow->put_MessageDrain((OAHWND) PSGLOBAL(window)))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in (pvVideoWindow->put_MessageDrain((OAHWND) PSGLOBAL(window)))\n", hr);
    }

    SetFocus(PSGLOBAL(window));
    return hr;
}

// 0x747660
void Play(int32 nCmdShow, const char* path) {
    UpdateWindow(PSGLOBAL(window));

#ifdef FIX_BUGS
    const auto size = MultiByteToWideChar(0, 0, path, -1, nullptr, 0);
    WCHAR* fileName = new WCHAR[size];
    MultiByteToWideChar(0, 0, path, -1, fileName, size);
#else
    // Buffer overflow may happen in MultiByteToWideChar.
    // Como el original: búfer sin inicializar y 255 caracteres como máximo
    WCHAR fileName[256];
    MultiByteToWideChar(0, 0, path, -1, fileName, 255);
#endif
    HRESULT hr;

    if (FAILED(hr = CoInitialize(nullptr))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in CoInitialize(NULL)\n", hr);
        return;
    }

    if (FAILED(hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&pvGraphBuilder))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&pvGraphBuilder)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->RenderFile(&fileName[0], nullptr))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->RenderFile(&wFileName[0], NULL)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&pvMediaControl))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&pvMediaControl)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&pvMediaEvent))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&pvMediaEvent)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&pvMediaSeeking))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&pMS)\n", hr);
        return;
    }

    if (FAILED(hr = pvGraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&pvVideoWindow))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&pvVideoWindow)\n", hr);
        return;
    }

    if (FAILED(hr = pvVideoWindow->put_Owner((OAHWND)PSGLOBAL(window)))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvVideoWindow->put_Owner((OAHWND) PSGLOBAL(window))\n", hr);
        return;
    }

    if (FAILED(hr = pvVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)\n", hr);
        return;
    }

    if (FAILED(hr = pvMediaEvent->SetNotifyWindow((OAHWND)PSGLOBAL(window), WM_GRAPHNOTIFY, 0))) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvMediaEvent->SetNotifyWindow((OAHWND)PSGLOBAL(window), WM_GRAPHNOTIFY, 0)\n", hr);
        return;
    }

    UpdateWindow();

    if (FAILED(hr = pvMediaControl->Run())) {
        NOTSA_LOG_DEBUG("FAILED(hr=0x{:x}) in pvMediaControl->Run()\n", hr);
        return;
    }

    SetFocus(PSGLOBAL(window));

#ifdef FIX_BUGS
    delete[] fileName;
#endif
};

auto GetMediaControl() -> IMediaControl* {
    return pvMediaControl;
}

} // namespace VideoPlayer
