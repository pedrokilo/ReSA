<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 05/11/2026 (Ciudad de México):** 837 grupos / 9145 funciones reconstruidas; 9145 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1001"></a>

## Game

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GameInit() | [source/app/app_game.cpp:71](../../source/app/app_game.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::GameInit 0x005BF3B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InitialiseGame() | [source/app/app_game.cpp:88](../../source/app/app_game.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::InitialiseGame 0x0053E580 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| TheGame() | [source/app/app_game.cpp:103](../../source/app/app_game.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::TheGame 0x0053DF10 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RwInitialize(void* param) | [source/app/app_game.cpp:112](../../source/app/app_game.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::RwInitialize 0x005BF390 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RwTerminate() | [source/app/app_game.cpp:117](../../source/app/app_game.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::RwTerminate 0x0053D910 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RenderEffects() | [source/app/app_game.cpp:124](../../source/app/app_game.cpp#L124) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::RenderEffects 0x0053E170 | 0 / 0 | Posible dato fijo NumberOfScriptSearchLights: source/game_sa/Scripts/TheScripts.h:408 (0x00A90830); Posible dato fijo NumberOfSearchLights: source/game_sa/Entity/Vehicle/Heli.h:100 (0x00C1C96C); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:38 (0x00A9AE00) |
| RenderScene() | [source/app/app_game.cpp:157](../../source/app/app_game.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::RenderScene 0x0053DF40 | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo TypeOfMirror: source/game_sa/Mirrors.h:25 (0x00C7C724); Posible dato fijo UnderWaterness: source/game_sa/Weather.h:49 (0x00C8132C); Posible dato fijo bRenderingReflection: source/game_sa/Mirrors.h:23 (0x00C7C728); Posible dato fijo g_breakMan: source/game_sa/BreakManager_c.cpp:23 (0x00BB4240) |
| RenderMenus() | [source/app/app_game.cpp:245](../../source/app/app_game.cpp#L245) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::RenderMenus 0x0053E530 | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| Render2dStuff() | [source/app/app_game.cpp:254](../../source/app/app_game.cpp#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::Render2dStuff 0x0053E230 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo OnscnTimer: source/game_sa/UserDisplay.h:16 (0x00BA1788); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| RenderDebugShit() | [source/app/app_game.cpp:314](../../source/app/app_game.cpp#L314) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::RenderDebugShit 0x0053E160 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Idle(void* param) | [source/app/app_game.cpp:320](../../source/app/app_game.cpp#L320) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::Idle 0x0053E920 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo LightningFlash: source/game_sa/Weather.h:33 (0x00C812CC); Posible dato fijo NumLights: source/game_sa/PointLights.h:42 (0x00C3F0D0); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo bCreditsGoing: source/game_sa/Credits.h:12 (0x00C6E97C); Posible dato fijo gColourTop: source/app/app.h:26 (0x00B72CA0); Posible dato fijo g_realTimeShadowMan: source/game_sa/RealTimeShadowManager.cpp:24 (0x00C40350) |
| FrontendIdle() | [source/app/app_game.cpp:434](../../source/app/app_game.cpp#L434) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Game::FrontendIdle 0x0053E770 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo bCreditsGoing: source/game_sa/Credits.h:12 (0x00C6E97C); Posible dato fijo gColourTop: source/app/app.h:26 (0x00B72CA0) |

<a id="grupo-1002"></a>

## Globales y helpers: source/app/app.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AppInjectHooks() | [source/app/app.cpp:39](../../source/app/app.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChangeGameStateTo(eGameState newgs) | [source/app/app.cpp:58](../../source/app/app.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| ShouldPauseOriginalIdleInBackground() | [source/app/app.cpp:115](../../source/app/app.cpp#L115) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1003"></a>

## Globales y helpers: source/app/app_camera.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer) | [source/app/app_camera.cpp:6](../../source/app/app_camera.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CameraSize(RwCamera* camera, RwRect* rect, RwReal viewWindow, RwReal aspectRatio) | [source/app/app_camera.cpp:54](../../source/app/app_camera.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo gColourTop: source/app/app.h:26 (0x00B72CA0) |

<a id="grupo-1004"></a>

## Globales y helpers: source/app/app_debug.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OpenCharsetSafe() | [source/app/app_debug.cpp:21](../../source/app/app_debug.cpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CreateDebugFont() | [source/app/app_debug.cpp:29](../../source/app/app_debug.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DestroyDebugFont() | [source/app/app_debug.cpp:39](../../source/app/app_debug.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ObrsPrintfString(const char* str, int16 x, int16 y) | [source/app/app_debug.cpp:49](../../source/app/app_debug.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FlushObrsPrintfs() | [source/app/app_debug.cpp:56](../../source/app/app_debug.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WindowsExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) | [source/app/app_debug.cpp:63](../../source/app/app_debug.cpp#L63) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LogCurrentStack() | [source/app/app_debug.cpp:207](../../source/app/app_debug.cpp#L207) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ContinueOnAssert(const wchar_t* expression, const wchar_t* file, unsigned line) | [source/app/app_debug.cpp:227](../../source/app/app_debug.cpp#L227) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ContinueOnCrtReport(int reportType, wchar_t* message, int* returnValue) | [source/app/app_debug.cpp:240](../../source/app/app_debug.cpp#L240) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InstallContinueOnAssert() | [source/app/app_debug.cpp:263](../../source/app/app_debug.cpp#L263) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1005"></a>

## Globales y helpers: source/app/app_game.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AppGameInjectHooks() | [source/app/app_game.cpp:48](../../source/app/app_game.cpp#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeViewWindow() | [source/app/app_game.cpp:82](../../source/app/app_game.cpp#L82) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_fFOV: source/game_sa/Draw.h:14 (0x008D5038) |

<a id="grupo-1006"></a>

## Globales y helpers: source/app/app_input.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AppInputInjectHooks() | [source/app/app_input.cpp:25](../../source/app/app_input.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| HandleKeyEvent(bool isDown, RsKeyStatus* ks) | [source/app/app_input.cpp:50](../../source/app/app_input.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TempKeyState: source/game_sa/Pad.h:107 (0x00B72CB0); Posible dato fijo padNumber: source/game_sa/Pad.h:114 (0x00B73400) |
| HandlePadButtonDown(RsPadButtonStatus* padButtonStatus) | [source/app/app_input.cpp:193](../../source/app/app_input.cpp#L193) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo padNumber: source/game_sa/Pad.h:114 (0x00B73400) |
| HandlePadButtonUp(RsPadButtonStatus* padButtonStatus) | [source/app/app_input.cpp:216](../../source/app/app_input.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo padNumber: source/game_sa/Pad.h:114 (0x00B73400) |

<a id="grupo-1007"></a>

## Globales y helpers: source/app/app_light.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AppLightInjectHooks() | [source/app/app_light.cpp:24](../../source/app/app_light.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetAmbientColours() | [source/app/app_light.cpp:368](../../source/app/app_light.cpp#L368) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Light::SetAmbientColours-void 0x00735D30 | 0 / 0 | Posible dato fijo AmbientLightColourForFrame: source/app/app_light.h:9 (0x00C886D4); Posible dato fijo pAmbient: source/app/app_light.h:7 (0x00C886E8) |
| SetAmbientColours(RwRGBAReal* color) | [source/app/app_light.cpp:373](../../source/app/app_light.cpp#L373) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Light::SetAmbientColours-color 0x00735D50 | 0 / 0 | Posible dato fijo pAmbient: source/app/app_light.h:7 (0x00C886E8) |

<a id="grupo-1008"></a>

## Globales y helpers: source/app/platform/platform.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| rsCommandLine_Eax() | [source/app/platform/platform.cpp:29](../../source/app/platform/platform.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| rsPreInitCommandLine_Eax(uint32 /*entryEcx*/, uint32 entryEdx) | [source/app/platform/platform.cpp:38](../../source/app/platform/platform.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RsInjectHooks() | [source/app/platform/platform.cpp:54](../../source/app/platform/platform.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * RsCameraShowRaster(RwCamera* camera) | [source/app/platform/platform.cpp:119](../../source/app/platform/platform.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsWideScreenRatio(float ratio) | [source/app/platform/platform.cpp:387](../../source/app/platform/platform.cpp#L387) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFullScreenRatio(float ratio) | [source/app/platform/platform.cpp:394](../../source/app/platform/platform.cpp#L394) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1009"></a>

## Globales y helpers: source/app/platform/preset_view.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RsSetPresetView(RwCamera* camera, int32 viewNum) | [source/app/platform/preset_view.cpp:28](../../source/app/platform/preset_view.cpp#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:25 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:24 (0x00C17080); Posible dato fijo PresetViews: source/app/platform/preset_view.cpp:23 (0x00C1707C) |
| RsSetNextPresetView(RwCamera* camera) | [source/app/platform/preset_view.cpp:56](../../source/app/platform/preset_view.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:25 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:24 (0x00C17080) |
| RsSetPreviousPresetView(RwCamera* camera) | [source/app/platform/preset_view.cpp:73](../../source/app/platform/preset_view.cpp#L73) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:25 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:24 (0x00C17080) |
| RsDestroyPresetViews() | [source/app/platform/preset_view.cpp:84](../../source/app/platform/preset_view.cpp#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:24 (0x00C17080); Posible dato fijo PresetViews: source/app/platform/preset_view.cpp:23 (0x00C1707C) |
| * RsGetPresetViewDescription() | [source/app/platform/preset_view.cpp:102](../../source/app/platform/preset_view.cpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:25 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:24 (0x00C17080); Posible dato fijo PresetViews: source/app/platform/preset_view.cpp:23 (0x00C1707C) |

<a id="grupo-1010"></a>

## Globales y helpers: source/app/platform/win/VideoMode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VideoModeInjectHooks() | [source/app/platform/win/VideoMode.cpp:125](../../source/app/platform/win/VideoMode.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1011"></a>

## Globales y helpers: source/app/platform/win/VideoModeSelectDialog.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetCBCurSel(HWND hComboBox) | [source/app/platform/win/VideoModeSelectDialog.cpp:33](../../source/app/platform/win/VideoModeSelectDialog.cpp#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetCBCurSel(HWND hComboBox, RwInt32 entryIdx) | [source/app/platform/win/VideoModeSelectDialog.cpp:37](../../source/app/platform/win/VideoModeSelectDialog.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCBItemData(HWND hComboBox, RwInt32 entryIdx) | [source/app/platform/win/VideoModeSelectDialog.cpp:41](../../source/app/platform/win/VideoModeSelectDialog.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCBCurSelData(HWND hComboBox) | [source/app/platform/win/VideoModeSelectDialog.cpp:45](../../source/app/platform/win/VideoModeSelectDialog.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SetSelectedVM(HWND hDlg, RwInt32 vm) | [source/app/platform/win/VideoModeSelectDialog.cpp:49](../../source/app/platform/win/VideoModeSelectDialog.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GcurSelVM: source/app/platform/win/VideoMode.h:32 (0x008D6220) |
| FillAvailableVMs(HWND hVMSel) | [source/app/platform/win/VideoModeSelectDialog.cpp:55](../../source/app/platform/win/VideoModeSelectDialog.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FillAvailableDevices(HWND hDevSel) | [source/app/platform/win/VideoModeSelectDialog.cpp:97](../../source/app/platform/win/VideoModeSelectDialog.cpp#L97) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GcurSelSS: source/app/platform/win/VideoMode.h:22 (0x00C920F4); Posible dato fijo GnumSubSystems: source/app/platform/win/VideoMode.h:19 (0x00C920F0); Posible dato fijo GsubSysInfo: source/app/platform/win/VideoMode.h:16 (0x00C8CFC0) |
| ComboBoxClear(HWND hComboBox) | [source/app/platform/win/VideoModeSelectDialog.cpp:108](../../source/app/platform/win/VideoModeSelectDialog.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InitDialog(HWND hDlg) | [source/app/platform/win/VideoModeSelectDialog.cpp:113](../../source/app/platform/win/VideoModeSelectDialog.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo GcurSelVM: source/app/platform/win/VideoMode.h:32 (0x008D6220) |
| DialogFunc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) | [source/app/platform/win/VideoModeSelectDialog.cpp:141](../../source/app/platform/win/VideoModeSelectDialog.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GcurSelSS: source/app/platform/win/VideoMode.h:22 (0x00C920F4); Posible dato fijo GcurSelVM: source/app/platform/win/VideoMode.h:32 (0x008D6220) |
| CreateVidModeSelectDialog(HINSTANCE hInst, HWND hWnd) | [source/app/platform/win/VideoModeSelectDialog.cpp:194](../../source/app/platform/win/VideoModeSelectDialog.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1012"></a>

## Globales y helpers: source/app/platform/win/WinMain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * getDvdGamePath() | [source/app/platform/win/WinMain.cpp:49](../../source/app/platform/win/WinMain.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Win32_InitApplication(HINSTANCE hInstance) | [source/app/platform/win/WinMain.cpp:98](../../source/app/platform/win/WinMain.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00747EB0 |
| Win32_InitInstance(HINSTANCE hInstance) | [source/app/platform/win/WinMain.cpp:114](../../source/app/platform/win/WinMain.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| ProcessGameLogic(INT nCmdShow) | [source/app/platform/win/WinMain.cpp:153](../../source/app/platform/win/WinMain.cpp#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo ForegroundApp: source/app/app.h:24 (0x008D621C); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo IsVMNotSelected: source/app/platform/win/VideoMode.h:41 (0x008D6218); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo RwInitialized: source/game_sa/RenderWare/RenderWare.h:22 (0x00C920E8); Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo anisotropySupportedByGFX: source/app/platform/win/WinPlatform.h:42 (0x00C87FFC); Posible dato fijo gColourTop: source/app/app.h:26 (0x00B72CA0); Posible dato fijo gCurrentVideoMode: source/app/platform/win/VideoMode.h:48 (0x008D6220); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0); Posible dato fijo isForeground: source/app/platform/win/WinPlatform.h:43 (0x00C920EC) |
| MainLoop(INT nCmdShow) | [source/app/platform/win/WinMain.cpp:332](../../source/app/platform/win/WinMain.cpp#L332) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RwInitialized: source/game_sa/RenderWare/RenderWare.h:22 (0x00C920E8); Posible dato fijo gamma: source/game_sa/Gamma.h:24 (0x00C92134) |
| WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) | [source/app/platform/win/WinMain.cpp:506](../../source/app/platform/win/WinMain.cpp#L506) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectWinMainStuff() | [source/app/platform/win/WinMain.cpp:527](../../source/app/platform/win/WinMain.cpp#L527) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1013"></a>

## Globales y helpers: source/app/platform/win/WinPlatform.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetGammaWhenExiting() | [source/app/platform/win/WinPlatform.cpp:37](../../source/app/platform/win/WinPlatform.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gbGammaChanged: source/game_sa/Gamma.h:23 (0x00C920C8); Posible dato fijo ms_SavedGamma: source/game_sa/Gamma.h:5 (0x00C8D4C8) |
| Win32InjectHooks() | [source/app/platform/win/WinPlatform.cpp:161](../../source/app/platform/win/WinPlatform.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1014"></a>

## Globales y helpers: source/app/platform/win/WinPs.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetBestRefreshRate(RwUInt32 width, RwUInt32 height, RwUInt32 depth) | [source/app/platform/win/WinPs.cpp:445](../../source/app/platform/win/WinPs.cpp#L445) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GcurSelSS: source/app/platform/win/VideoMode.h:22 (0x00C920F4) |
| WinPsInjectHooks() | [source/app/platform/win/WinPs.cpp:595](../../source/app/platform/win/WinPs.cpp#L595) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1015"></a>

## Globales y helpers: source/app/platform/win/WindowedMode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DXCHECK(HRESULT hr) | [source/app/platform/win/WindowedMode.cpp:18](../../source/app/platform/win/WindowedMode.cpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1016"></a>

## Globales y helpers: source/app/platform/win/WndProc.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectHooksWndProcStuff() | [source/app/platform/win/WndProc.cpp:305](../../source/app/platform/win/WndProc.cpp#L305) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1017"></a>

## Globales y helpers: source/game_sa/3dMarker.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarker.cpp:39](../../source/game_sa/3dMarker.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetAtomicForMarkerType(e3dMarkerType type) | [source/game_sa/3dMarker.cpp:53](../../source/game_sa/3dMarker.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pRpClumpArray: source/game_sa/3dMarkers.h:57 (0x00C7C6DC) |

<a id="grupo-1018"></a>

## Globales y helpers: source/game_sa/3dMarkers.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeStdSize(float sz, float dist) | [source/game_sa/3dMarkers.cpp:38](../../source/game_sa/3dMarkers.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarkers.cpp:55](../../source/game_sa/3dMarkers.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1019"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:653](../../source/game_sa/Animation/AnimManager.cpp#L653) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:658](../../source/game_sa/Animation/AnimManager.cpp#L658) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:672](../../source/game_sa/Animation/AnimManager.cpp#L672) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1020"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:127](../../source/game_sa/Animation/AnimManager.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
