/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Ps
 * Reconstrucción: COMPLETA — 22/22 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 22/22 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <platform/win/WinPlatform.h>
#include <platform/platform.h>
#include <ddraw.h>
#include <dsound.h>
#include "VideoMode.h"
#include "VideoModeSelectDialog.h"
#include "extensions/Configs/Miscellaneous.hpp"
#include "LoadingScreen.h"
#include "C_PcSave.h"
#include <windows.h>

#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include "WindowedMode.hpp"
#include "SDLWrapper.hpp"
#endif
#include "winincl.h"
#include "intrin.h"
#include <mmsystem.h>

//! Disable "This function was depracated"
//#pragma warning (disable : 28159 4996)

// Como el original: el fabricante de la CPU se guarda en el global del EXE ("UnknownVendr" hasta psInitialize).
static auto& gCpuVendor = StaticRef<std::array<char, 13>>(0x8D6238);

static auto& PsGlobal = StaticRef<psGlobalType>(0xC8CF88);

// Como el original: psSelectDevice guarda aquí el contador de ciclos (rdtsc); nadie lo lee.
static auto& s_SelectDeviceTscHigh = StaticRef<uint32>(0xC92104);
static auto& s_SelectDeviceTscLow  = StaticRef<uint32>(0xC92108);

// 0x7455E0 - Get available videomem
static HRESULT GetVideoMemInfo(LPDWORD total, LPDWORD available) {
	LPDIRECTDRAW7 dd;

    if (HRESULT hr = DirectDrawCreateEx(NULL, (LPVOID*)&dd, IID_IDirectDraw7, NULL); FAILED(hr)) {
        return hr;
    }

	DDSCAPS2 caps;
	ZeroMemory(&caps, sizeof(DDSCAPS2));
	caps.dwCaps = DDSCAPS_VIDEOMEMORY;
	HRESULT hr = dd->GetAvailableVidMem(&caps, total, available);
	dd->Release();

    return hr;
}

//! 0x745840 - Check if DirectSound can be loaded
static BOOL CheckDirectSound() {
    // Como el original: inicializa COM aqui (sin mirar el resultado) y crea el dispositivo con DirectSoundCreate8
    // (DSOUND ordinal 11 en la importacion diferida del EXE).
    CoInitialize(NULL);

    LPDIRECTSOUND8 ds;
    if (FAILED(DirectSoundCreate8(NULL, &ds, NULL))) {
        return FALSE;
    }

    DSCAPS caps; // Como el original: solo se escribe dwSize
    caps.dwSize = sizeof(DSCAPS);
    const BOOL result = ds->GetCaps(&caps) == DS_OK; // Como el original: cualquier HRESULT distinto de 0 es fallo

    ds->Release();

    return result;
}

// 0x7465B0
static void InitialiseLanguage() {
    const auto sysDefaultLCID = PRIMARYLANGID(GetSystemDefaultLCID());
	const auto usrDefaultLCID = PRIMARYLANGID(GetUserDefaultLCID());
    const auto kbLayoutLCID   = PRIMARYLANGID(LOWORD(GetKeyboardLayout(0)));
    // Como el original: las tres consultas se repiten (solo usa el subidioma del usuario, que no cambia el resultado)
    GetSystemDefaultLCID();
    GetUserDefaultLCID();
    GetKeyboardLayout(0);

	FrontEndMenuManager.m_nTitleLanguage = sysDefaultLCID;

    FrontEndMenuManager.m_nTextLanguage = (int32)[&] {
	    switch (kbLayoutLCID) {
		case LANG_GERMAN:  return eLanguage::GERMAN;
		case LANG_SPANISH: return eLanguage::SPANISH;
		case LANG_FRENCH:  return eLanguage::FRENCH;
		case LANG_ITALIAN: return eLanguage::ITALIAN;
		default:           return eLanguage::AMERICAN;
	    }
    }();

    FrontEndMenuManager.m_nPrefsLanguage = [&] {
        switch (usrDefaultLCID) {
        case LANG_SPANISH: return eLanguage::SPANISH;
        default:           return eLanguage::AMERICAN;
        }
    }();

    // Reload text
	TheText.Unload(false);
	TheText.Load(false);
}

// 0x747420
RwBool psInitialize() {
    // NOTSA: el EXE no declara la aplicacion consciente de DPI; sin esto Windows escala la ventana (SDL3/ventana).
    SetProcessDPIAware();

    auto ps = &PsGlobal;

    ps->lastMousePos.y = 0.f;
    ps->lastMousePos.x = 0.f;
    ps->fullScreen    = FALSE;
    ps->diInterface   = NULL;
    ps->diMouse       = NULL;
    ps->diDevice1     = NULL;
    ps->diDevice2     = NULL;

    RsGlobal.ps = ps;

    CFileMgr::Initialise();
    C_PcSave::SetSaveDirectory(InitUserDirectories());

    InitialiseLanguage(); // Como el original

    gGameState = GAME_STATE_INITIAL;

    // Como el original: cpuid(1) y cpuid(0x80000001) solo rellenan una variable local sin uso; el fabricante
    // (cpuid 0: ebx, edx, ecx) se copia sin terminador al global del EXE.
    int regs[4];
    __cpuid(regs, 0);
    std::memcpy(gCpuVendor.data(), &regs[1], 4);
    std::memcpy(gCpuVendor.data() + 4, &regs[3], 4);
    std::memcpy(gCpuVendor.data() + 8, &regs[2], 4);

    OSVERSIONINFOA verInfo; // Como el original: solo se escribe el tamano
    verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    GetVersionExA(&verInfo);

    s_OSStatus.OSVer = WinVer::WIN_95;
    if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        // Como el original: NT anterior a la 4 se queda en WIN_95
        if (verInfo.dwMajorVersion == 4) {
            s_OSStatus.OSVer = WinVer::WIN_NT;
        } else if (verInfo.dwMajorVersion == 5) {
            s_OSStatus.OSVer = WinVer::WIN_2000_XP_2003;
        } else if (verInfo.dwMajorVersion > 5) {
            s_OSStatus.OSVer = WinVer::WIN_VISTA_OR_LATER;
        }
    } else if (verInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        s_OSStatus.OSVer = verInfo.dwMajorVersion > 4 || verInfo.dwMajorVersion == 4 && verInfo.dwMinorVersion > 0
            ? WinVer::WIN_98
            : WinVer::WIN_95;
    }

    // Como el original: los ajustes se cargan antes de comprobar nada
    FrontEndMenuManager.LoadSettings();

    // Como el original: GetDXVersion (0x745650, no reversada) detecta la version de DirectX
    s_OSStatus.DxVer = plugin::CallAndReturn<DWORD, 0x745650>();

    // Como el original: MessageBoxA, con el titulo pedido antes que el texto
    if (s_OSStatus.OSVer == WinVer::WIN_95) {
        const auto title = TheText.Get("WIN_TTL"); // Grand Theft Auto San Andreas
        const auto text  = TheText.Get("WIN_95");  // Grand Theft Auto San Andreas cannot run on Windows 95
        MessageBoxA(NULL, (LPCSTR)text, (LPCSTR)title, MB_OK);
        return FALSE;
    }

    if (s_OSStatus.DxVer < 0x900) {
        const auto title = TheText.Get("WIN_TTL");
        const auto text  = TheText.Get("WIN_DX"); // Grand Theft Auto San Andreas requires at least DirectX version 8.1
        MessageBoxA(NULL, (LPCSTR)text, (LPCSTR)title, MB_OK);
        return FALSE;
    }

    MEMORYSTATUS memstat; // Como el original: GlobalMemoryStatus rellena dwLength
    GlobalMemoryStatus(&memstat);
    s_OSStatus.RAM.TotalPhys    = memstat.dwTotalPhys;
    s_OSStatus.RAM.AvailPhys    = memstat.dwAvailPhys;
    s_OSStatus.RAM.TotalVirtual = memstat.dwTotalVirtual;
    s_OSStatus.RAM.AvailVirtual = memstat.dwAvailVirtual;

    GetVideoMemInfo(&s_OSStatus.VRAM.Total, &s_OSStatus.VRAM.Avail); // Como el original: sin comprobar

    if (!CheckDirectSound()) {
        const auto title = TheText.Get("WIN_TTL");
        const auto text  = TheText.Get("WIN_NSC"); // Grand Theft Auto San Andreas requires a sound card
        MessageBoxA(NULL, (LPCSTR)text, (LPCSTR)title, MB_ICONEXCLAMATION);
        return FALSE;
    }

    TheText.Unload(false); // Como el original

    return TRUE;
}

// 0x7458A0
void psTerminate() {
    // The executable owns this static block; SDL owns its separate wrapper.
}

// 0x7451B0
void psWindowSetText(const char* str) {
    SetWindowTextA(PSGLOBAL(window), str);
}

// 0x7451D0
void psErrorMessage(const char* str) {
    MessageBoxA(nullptr, str, RsGlobal.appName, MB_ICONERROR | MB_TASKMODAL | MB_TOPMOST);
}

// 0x7451F0
void psWarningMessage(const char* str) {
    MessageBoxA(nullptr, str, RsGlobal.appName, MB_ICONWARNING | MB_TASKMODAL | MB_TOPMOST);
}

// 0x745210
RwBool psCameraBeginUpdate(RwCamera* camera) {
    if (RwCameraBeginUpdate(Scene.m_pRwCamera)) {
        return TRUE;
    }
    RsEventHandler(rsACTIVATE, nullptr);
    return FALSE;
}

// 0x745240
RwCamera* psCameraShowRaster(RwCamera* camera) {
    // Mobile anyway flags = 0x0
    const auto flags = FrontEndMenuManager.m_bPrefsFrameLimiter || CLoadingScreen::IsActive() ? rwRASTERFLIPWAITVSYNC : rwRASTERFLIPDONTWAIT;
    return RwCameraShowRaster(camera, PSGLOBAL(window), flags);
}

// 0x745270
uint32 psTimer() {
    // Como el original: timeGetTime con la resolucion minima del temporizador multimedia
    TIMECAPS timeCaps;
    timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));
    timeBeginPeriod(timeCaps.wPeriodMin);
    const auto time = (uint32)timeGetTime();
    timeEndPeriod(timeCaps.wPeriodMin);
    return time;
}

// 0x7452B0
RwImage* psGrabScreen(RwCamera* camera) {
    auto* device = static_cast<IDirect3DDevice9*>(RwD3D9GetCurrentD3DDevice());

    // Como el original: no se comprueba ningun HRESULT
    D3DDISPLAYMODE displayMode;
    device->GetDisplayMode(0, &displayMode);

    IDirect3DSurface9* surface;
    device->CreateOffscreenPlainSurface(displayMode.Width, displayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);
    device->GetFrontBufferData(0, surface);

    D3DLOCKED_RECT lockedRect;
    if (PSGLOBAL(fullScreen)) {
        surface->LockRect(&lockedRect, nullptr, D3DLOCK_READONLY);
    } else {
        // Como el original: rectangulo de toda la ventana (con marco y barra de titulo)
        RECT rect;
        GetWindowRect(PSGLOBAL(window), &rect);
        displayMode.Height = rect.bottom - rect.top;
        displayMode.Width = rect.right - rect.left;
        surface->LockRect(&lockedRect, &rect, D3DLOCK_READONLY);
    }

    RwImage* image = RwImageCreate(int32(displayMode.Width), int32(displayMode.Height), 32);
    if (image) {
        RwImageAllocatePixels(image);

        auto* pixels = (RwRGBA*)RwImageGetPixels(image);
        auto* imagePixels = (uint8*)lockedRect.pBits;

        for (auto h = 0u; h < displayMode.Height; h++) {
            for (auto w = 0u; w < displayMode.Width; w++) {
                pixels->red   = imagePixels[sizeof(RwRGBA) * w + 2];
                pixels->green = imagePixels[sizeof(RwRGBA) * w + 1];
                pixels->blue  = imagePixels[sizeof(RwRGBA) * w + 0];
                pixels->alpha = 255;
                pixels++;
            }
            imagePixels += lockedRect.Pitch;
        }
    }

    // Como el original: la superficie no se desbloquea ni se libera
    return image;
}

// 0x7453E0
void psMouseSetVisibility(RwBool visible) {
    ::ShowCursor(visible);
}

// 0x7453F0
void psMouseSetPos(RwV2d* pos) {
    // Como el original: _ftol2 (truncamiento de 64 bits)
    POINT point = { .x = LONG((int64)pos->x), .y = LONG((int64)pos->y) };
    ::ClientToScreen(PSGLOBAL(window), &point);
    ::SetCursorPos(point.x, point.y);
    // Como el original: guarda la posicion truncada a entero (vuelve a leer pos)
    PSGLOBAL(lastMousePos).x = (float)(int32)(int64)pos->x;
    PSGLOBAL(lastMousePos).y = (float)(int32)(int64)pos->y;
}

// 0x745470
char* psPathnameCreate(const char* buffer) {
    // Como el original: funciones de cadena de RenderWare (RwEngineInstance)
    auto path = (char*)CMemoryMgr::Malloc(uint32(rwstrlen(buffer) + 1), 0);
    if (path) {
        rwstrcpy(path, buffer);

        while (auto ch = rwstrchr(path, '/')) {
            *ch = '\\';
        }
    }

    return path;
}

// 0x7454E0
void psPathnameDestroy(char* buffer) {
    if (buffer) {
        RwFree(buffer);
    }
}

// 0x745500
char psPathGetSeparator() {
    return '\\';
}

// 0x745520
RwBool psInstallFileSystem() {
    return TRUE;
}

// 0x745530
RwBool psNativeTextureSupport() {
    return RwD3D9DeviceSupportsDXTTexture();
}

// 0x745540
void psDebugMessageHandler(RwDebugType type, const RwChar* str) {
    // Como el original: manejador de depuracion de RW, solo lo envia al depurador
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
}

// 0x7458B0
RwBool psAlwaysOnTop(RwBool alwaysOnTop) {
    const auto hwnd = PSGLOBAL(window);

    RECT winRect;
    GetWindowRect(hwnd, &winRect); // Como el original: sin comprobar

    return SetWindowPos(
        hwnd,
        alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
        winRect.left,                 winRect.top,
        winRect.right - winRect.left, winRect.bottom - winRect.top,
        0
    );
}

// 0x7460A0
static RwUInt32 GetBestRefreshRate(RwUInt32 width, RwUInt32 height, RwUInt32 depth) {
    // Como el original: version de SDK 31 (0x1F)
    const auto d3d = Direct3DCreate9(31);

    const auto format = depth == 32
        ? D3DFMT_X8R8G8B8
        : depth == 24
            ? D3DFMT_R8G8B8
            : D3DFMT_R5G6B5;

    // Como el original: recorre los modos en orden ascendente; centinela 0x7FFFFFFF
    RwUInt32 refreshRate = INT32_MAX;
    const auto numModes = (int32)d3d->GetAdapterModeCount(GcurSelSS, format);
    for (auto i = 0; i < numModes; i++) {
        D3DDISPLAYMODE mode;
        d3d->EnumAdapterModes(GcurSelSS, format, i, &mode);

        if (mode.Width != width || mode.Height != height || mode.Format != format) {
            continue;
        }

        if (mode.RefreshRate == 0) {
            d3d->Release();
            return 0;
        }

        if (mode.RefreshRate < refreshRate && mode.RefreshRate >= 60) {
            refreshRate = mode.RefreshRate;
        }
    }
    d3d->Release();

    return refreshRate == INT32_MAX ? (RwUInt32)-1 : refreshRate;
}

// 0x746190
RwBool psSelectDevice() {
    const auto wnd  = PSGLOBAL(window);
    const auto inst = PSGLOBAL(instance);

    if (!UseDefaultVM) {
        GnumSubSystems = RwEngineGetNumSubSystems();
        if (!GnumSubSystems) {
            return FALSE;
        }

        /* Just to be sure ... */
        GnumSubSystems = std::min(MAX_SUBSYSTEMS, GnumSubSystems);

        /* Get the names of all the sub systems */
        for (auto i = 0; i < GnumSubSystems; i++) {
            RwEngineGetSubSystemInfo(&GsubSysInfo[i], i);
        }

        /* Get the default selection */
        GcurSelSS = RwEngineGetCurrentSubSystem();

        const auto tsc = __rdtsc(); // Como el original
        s_SelectDeviceTscHigh = (uint32)(tsc >> 32);
        s_SelectDeviceTscLow  = (uint32)tsc;
    }

    if (GnumSubSystems > 1) {
        MultipleSubSystems = TRUE;

        if (!UseDefaultVM) {
            if (g_MiscConfig.SkipDeviceSelection) {
                // NOTSA (opcion de configuracion, desactivada por defecto): same choice the dialog would offer,
                // without waiting for the user: the GPU and video mode saved in gta_sa.set when still valid,
                // otherwise RW's current ones.
                if (const auto ss = FrontEndMenuManager.m_nCurrentRwSubsystem; ss >= 0 && ss < GnumSubSystems) {
                    GcurSelSS = ss;
                }
                VERIFY(RwEngineSetSubSystem(GcurSelSS));
                const auto vm = FrontEndMenuManager.m_nDisplayVideoMode;
                GcurSelVM = vm > 0 && vm < RwEngineGetNumVideoModes() && GetVideoModeList()[vm] ? vm : RwEngineGetCurrentVideoMode();
            } else if (!CreateVidModeSelectDialog(inst, wnd)) {
                return FALSE; // User failed to select video mode
            }
        }

        if (!RwEngineSetSubSystem(GcurSelSS)) {
            return FALSE;
        }
    } else {
        MultipleSubSystems = FALSE;

        if (!RwEngineSetSubSystem(GcurSelSS)) {
            return FALSE;
        }

        if (!UseDefaultVM) {
            // Como el original: la lista se pide siempre, antes de mirar el modo guardado
            const auto vmList    = GetVideoModeList();
            const auto vmDisplay = FrontEndMenuManager.m_nDisplayVideoMode;
            if (vmList[vmDisplay] && vmDisplay) {
                GcurSelVM = FrontEndMenuManager.m_nPrefsVideoMode = vmDisplay;
            } else {
                // Como el original: busca 800x600x32 exclusivo desde el modo actual avanzando GcurSelVM;
                // si IsVMNotSelected es falso se queda con el primer modo examinado.
                for (;; GcurSelVM++) {
                    if (GcurSelVM >= RwEngineGetNumVideoModes()) {
                        MessageBoxA(NULL, "Cannot find 800x600x32 video mode", "GTA: San Andreas", MB_OK);
                        return FALSE;
                    }

                    RwVideoMode vmi;
                    RwEngineGetVideoModeInfo(&vmi, GcurSelVM);
                    if (!IsVMNotSelected) {
                        break;
                    }
                    if (vmi.width == 800 && vmi.height == 600 && vmi.depth == 32 && (vmi.flags & rwVIDEOMODEEXCLUSIVE)) {
                        break;
                    }
                }
            }
        }
    }

    RwVideoMode vmi;
    RwEngineGetVideoModeInfo(&vmi, GcurSelVM);

    FrontEndMenuManager.m_nCurrentScreenItem = 0;

    // Set selected videomode
    if (!RwEngineSetVideoMode(GcurSelVM)) {
        return FALSE;
    }

#ifdef NOTSA_USE_SDL3
    // NOTSA: con SDL3 la ventana la gestiona SDL; hay que ajustarla al modo elegido.
    if (!notsa::SDLWrapper::Initialize()) { return FALSE; }
    SDL_SetWindowSize(notsa::SDLWrapper::GetWindow(), (int)(vmi.width), (int)(vmi.height));
#endif

    if (vmi.flags & rwVIDEOMODEEXCLUSIVE) {
        if (const auto rr = GetBestRefreshRate(vmi.width, vmi.height, vmi.depth); rr != (RwUInt32)-1) {
            RwD3D9EngineSetRefreshRate(rr);
        }

        RsGlobal.maximumWidth  = vmi.width;
        RsGlobal.maximumHeight = vmi.height;
        PSGLOBAL(fullScreen)   = TRUE;
    }

    RwD3D9EngineSetMultiSamplingLevels(FrontEndMenuManager.m_nPrefsAntialiasing);

    return TRUE;
}

void WinPsInjectHooks() {
    RH_ScopedCategory("Win");
    RH_ScopedNamespaceName("Ps");

    RH_ScopedGlobalInstall(psInitialize, 0x747420);
    RH_ScopedGlobalInstall(psTerminate, 0x7458A0);
    RH_ScopedGlobalInstall(psWindowSetText, 0x7451B0);
    RH_ScopedGlobalInstall(psErrorMessage, 0x7451D0);
    RH_ScopedGlobalInstall(psWarningMessage, 0x7451F0);
    RH_ScopedGlobalInstall(psCameraBeginUpdate, 0x745210);
    RH_ScopedGlobalInstall(psCameraShowRaster, 0x745240);
    RH_ScopedGlobalInstall(psTimer, 0x745270);
    RH_ScopedGlobalInstall(psGrabScreen, 0x7452B0);
    RH_ScopedGlobalInstall(psMouseSetVisibility, 0x7453E0);
    RH_ScopedGlobalInstall(psMouseSetPos, 0x7453F0);
    RH_ScopedGlobalInstall(psPathnameCreate, 0x745470);
    RH_ScopedGlobalInstall(psPathnameDestroy, 0x7454E0);
    RH_ScopedGlobalInstall(psPathGetSeparator, 0x745500);
    RH_ScopedGlobalInstall(psInstallFileSystem, 0x745520);
    RH_ScopedGlobalInstall(psNativeTextureSupport, 0x745530);
    RH_ScopedGlobalInstall(psDebugMessageHandler, 0x745540);
    RH_ScopedGlobalInstall(psAlwaysOnTop, 0x7458B0);
    RH_ScopedGlobalInstall(psSelectDevice, 0x746190);

    RH_ScopedGlobalInstall(InitialiseLanguage, 0x7465B0);
    RH_ScopedGlobalInstall(CheckDirectSound, 0x745840);
    RH_ScopedGlobalInstall(GetVideoMemInfo, 0x7455E0);
}
