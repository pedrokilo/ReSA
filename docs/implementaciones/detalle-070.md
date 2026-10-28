<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1381"></a>

## UserTracksLegacyVector

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| UserTracksLegacyVector::PushBack(tUserTracksInfo* const& value) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:671](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L671) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: UserTracksLegacyVector::PushBack 0x004F4620 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Colaborador por dirección 0x004F3B30: fuera del catálogo consultado; ruta efectiva sin resolver; Colaborador por dirección 0x004F4320: fuera del catálogo consultado; ruta efectiva sin resolver |

<a id="grupo-1382"></a>

## VideoMode

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ** GetVideoModeList() | [source/app/platform/win/VideoMode.cpp:39](../../source/app/platform/win/VideoMode.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoMode::GetVideoModeList 0x00745AF0 | 0 / 0 | Posible dato fijo gCurrentGpu: source/app/platform/win/VideoMode.h:47 (0x008D6248); Posible dato fijo gVideoModes: source/app/platform/win/VideoMode.h:46 (0x00C920D0); Posible dato fijo s_OSStatus: source/app/platform/win/WinPlatform.h:40 (0x00C8CF68) |
| FreeVideoModeList() | [source/app/platform/win/VideoMode.cpp:94](../../source/app/platform/win/VideoMode.cpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoMode::FreeVideoModeList 0x00745A80 | 0 / 0 | Posible dato fijo gVideoModes: source/app/platform/win/VideoMode.h:46 (0x00C920D0) |
| SetVideoMode(int32 mode) | [source/app/platform/win/VideoMode.cpp:108](../../source/app/platform/win/VideoMode.cpp#L108) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoMode::SetVideoMode 0x00745C70 | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo gVideoModes: source/app/platform/win/VideoMode.h:46 (0x00C920D0) |
| IsVideoModeExclusive() | [source/app/platform/win/VideoMode.cpp:119](../../source/app/platform/win/VideoMode.cpp#L119) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoMode::IsVideoModeExclusive 0x00745CA0 | 0 / 0 | Posible dato fijo gCurrentVideoMode: source/app/platform/win/VideoMode.h:48 (0x008D6220) |

<a id="grupo-1383"></a>

## VideoPlayer

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectHooks() | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:34](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Release() | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:47](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoPlayer::Release 0x00746740 | 0 / 0 | Posible dato fijo pvGraphBuilder: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:28 (0x00C920D4); Posible dato fijo pvMediaControl: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:29 (0x00C920D8); Posible dato fijo pvMediaEvent: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:30 (0x00C920DC); Posible dato fijo pvMediaSeeking: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:32 (0x00C920E4); Posible dato fijo pvVideoWindow: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:31 (0x00C920E0) |
| Shutdown() | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:60](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoPlayer::Shutdown 0x007467B0 | 0 / 0 | Posible dato fijo pvMediaControl: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:29 (0x00C920D8) |
| WaitState(int32 state) | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:68](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoPlayer::WaitState 0x00745F90 | 0 / 0 | Posible dato fijo pvMediaControl: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:29 (0x00C920D8) |
| OnGraphNotify() | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:78](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoPlayer::OnGraphNotify 0x00745FB0 | 0 / 0 | Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0); Posible dato fijo pvMediaEvent: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:30 (0x00C920DC) |
| UpdateWindow() | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:103](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoPlayer::UpdateWindow 0x007466D0 | 0 / 0 | Posible dato fijo pvVideoWindow: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:31 (0x00C920E0) |
| Play(int32 nCmdShow, const char* path) | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:123](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: VideoPlayer::Play 0x00747660 | 0 / 0 | Posible dato fijo pvGraphBuilder: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:28 (0x00C920D4); Posible dato fijo pvMediaControl: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:29 (0x00C920D8); Posible dato fijo pvMediaEvent: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:30 (0x00C920DC); Posible dato fijo pvMediaSeeking: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:32 (0x00C920E4); Posible dato fijo pvVideoWindow: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:31 (0x00C920E0) |
| GetMediaControl() -&gt; IMediaControl* | [source/app/platform/win/VideoPlayer/VideoPlayer.cpp:202](../../source/app/platform/win/VideoPlayer/VideoPlayer.cpp#L202) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo pvMediaControl: source/app/platform/win/VideoPlayer/VideoPlayer.cpp:29 (0x00C920D8) |

<a id="grupo-1384"></a>

## WaterCreatureManager_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaterCreatureManager_c::InjectHooks() | [source/game_sa/WaterCreatureManager_c.cpp:25](../../source/game_sa/WaterCreatureManager_c.cpp#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::WaterCreatureManager_c() | [source/game_sa/WaterCreatureManager_c.cpp:38](../../source/game_sa/WaterCreatureManager_c.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::Init() | [source/game_sa/WaterCreatureManager_c.cpp:42](../../source/game_sa/WaterCreatureManager_c.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::Init 0x006E3F90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::Exit() | [source/game_sa/WaterCreatureManager_c.cpp:53](../../source/game_sa/WaterCreatureManager_c.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::Exit 0x006E3FD0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::GetRandomWaterCreatureId() | [source/game_sa/WaterCreatureManager_c.cpp:65](../../source/game_sa/WaterCreatureManager_c.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::GetRandomWaterCreatureId 0x006E4040 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::TryToFreeUpWaterCreatures(int32 numToFree) | [source/game_sa/WaterCreatureManager_c.cpp:83](../../source/game_sa/WaterCreatureManager_c.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::TryToFreeUpWaterCreatures 0x006E40E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::CanAddWaterCreatureAtPos(int32 id, CVector pos) | [source/game_sa/WaterCreatureManager_c.cpp:101](../../source/game_sa/WaterCreatureManager_c.cpp#L101) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::CanAddWaterCreatureAtPos 0x006E4510 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreatureManager_c::TryToExitGroup(WaterCreature_c* waterCreature) | [source/game_sa/WaterCreatureManager_c.cpp:118](../../source/game_sa/WaterCreatureManager_c.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::TryToExitGroup 0x006E45B0 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| WaterCreatureManager_c::Update(float deltaTime) | [source/game_sa/WaterCreatureManager_c.cpp:151](../../source/game_sa/WaterCreatureManager_c.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreatureManager_c::Update 0x006E4F10 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028) |
| & GetCreatureInfo(uint32/*eWaterCreatureType*/ type) | [source/game_sa/WaterCreatureManager_c.h:62](../../source/game_sa/WaterCreatureManager_c.h#L62) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_waterCreatureInfos: source/game_sa/WaterCreatureManager_c.h:37 (0x008D3698) |

<a id="grupo-1385"></a>

## WaterCreature_c

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WaterCreature_c::InjectHooks() | [source/game_sa/WaterCreature_c.cpp:24](../../source/game_sa/WaterCreature_c.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| WaterCreature_c::Init(int32 nType, CVector* vecPos, WaterCreature_c* parent, float fWaterLevel, float fWaterDepth) | [source/game_sa/WaterCreature_c.cpp:92](../../source/game_sa/WaterCreature_c.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreature_c::Init 0x006E4160 | 0 / 0 | Posible dato fijo currArea: source/game_sa/Game.h:21 (0x00B72914); Posible dato fijo nNoTempObjects: source/game_sa/Entity/Object/Object.h:94 (0x00BB4A70) |
| WaterCreature_c::Exit() | [source/game_sa/WaterCreature_c.cpp:167](../../source/game_sa/WaterCreature_c.cpp#L167) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreature_c::Exit 0x006E3E60 | 0 / 0 | Posible dato fijo g_waterCreatureMan: source/game_sa/WaterCreatureManager_c.cpp:23 (0x00C1DF30); Posible dato fijo nNoTempObjects: source/game_sa/Entity/Object/Object.h:94 (0x00BB4A70) |
| WaterCreature_c::Update(float fTimeStep) | [source/game_sa/WaterCreature_c.cpp:178](../../source/game_sa/WaterCreature_c.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: WaterCreature_c::Update 0x006E4670 | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:41 (0x00B6F028); Posible dato fijo g_waterCreatureMan: source/game_sa/WaterCreatureManager_c.cpp:23 (0x00C1DF30) |
| WaterCreature_c() | [source/game_sa/WaterCreature_c.h:23](../../source/game_sa/WaterCreature_c.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetObject() const | [source/game_sa/WaterCreature_c.h:48](../../source/game_sa/WaterCreature_c.h#L48) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsJellyfish() const | [source/game_sa/WaterCreature_c.h:49](../../source/game_sa/WaterCreature_c.h#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSmallFish() const | [source/game_sa/WaterCreature_c.h:50](../../source/game_sa/WaterCreature_c.h#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1386"></a>

## Win

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MessageLoop() | [source/app/platform/win/WinMain.cpp:85](../../source/app/platform/win/WinMain.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Win::MessageLoop 0x00746870 | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| IsAlreadyRunning() | [source/app/platform/win/WinMain.cpp:135](../../source/app/platform/win/WinMain.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Win::IsAlreadyRunning 0x007468E0 | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| IsForegroundApp() | [source/app/platform/win/WinMain.cpp:148](../../source/app/platform/win/WinMain.cpp#L148) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Win::IsForegroundApp 0x00746060 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) | [source/app/platform/win/WinMain.cpp:383](../../source/app/platform/win/WinMain.cpp#L383) | C++ · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: Win::NOTSA_WinMain 0x00748710 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo MultipleSubSystems: source/app/platform/win/VideoMode.h:25 (0x00C92118); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| GTATranslateShiftKey(RsKeyCodes* keyCode) | [source/app/platform/win/WinPlatform.cpp:51](../../source/app/platform/win/WinPlatform.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Win::GTATranslateShiftKey 0x00747CD0 | 0 / 0 | Posible dato fijo s_OSStatus: source/app/platform/win/WinPlatform.h:40 (0x00C8CF68) |
| GTATranslateKey(RsKeyCodes* ck, LPARAM lParam, UINT vk) | [source/app/platform/win/WinPlatform.cpp:83](../../source/app/platform/win/WinPlatform.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: Win::GTATranslateKey 0x00747820 | 0 / 0 | Posible dato fijo s_OSStatus: source/app/platform/win/WinPlatform.h:40 (0x00C8CF68) |
| __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) | [source/app/platform/win/WndProc.cpp:49](../../source/app/platform/win/WndProc.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Win::__MainWndProc 0x00747EB0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Posible dato fijo AEAudioHardware: source/game_sa/Audio/Hardware/AEAudioHardware.cpp:30 (0x00B5F8B8); Posible dato fijo CameraResizeRejectedDword: source/app/platform/win/WndProc.cpp:44 (0x00C9246C); Posible dato fijo ForegroundAppDword: source/app/platform/win/WndProc.cpp:42 (0x008D621C); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo Pads: source/game_sa/Pad.h:111 (0x00B73458); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo RwInitializedDword: source/app/platform/win/WndProc.cpp:41 (0x00C920E8); Posible dato fijo WindowedDword: source/app/platform/win/WndProc.cpp:43 (0x00C920CC); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0); Posible dato fijo gbGammaChanged: source/game_sa/Gamma.h:23 (0x00C920C8); Posible dato fijo ms_GammaTable: source/game_sa/Gamma.h:6 (0x00C8DAC8); Posible dato fijo ms_SavedGamma: source/game_sa/Gamma.h:5 (0x00C8D4C8); Posible dato fijo ms_running: source/game_sa/CutsceneMgr.h:77 (0x00B5F851) |

<a id="grupo-1387"></a>

## WinInput

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateInput() | [source/app/platform/win/WinInput.cpp:32](../../source/app/platform/win/WinInput.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialise() | [source/app/platform/win/WinInput.cpp:45](../../source/app/platform/win/WinInput.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198) |
| ReleaseInput() | [source/app/platform/win/WinInput.cpp:59](../../source/app/platform/win/WinInput.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Shutdown() | [source/app/platform/win/WinInput.cpp:66](../../source/app/platform/win/WinInput.cpp#L66) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadSetRanges(LPDIRECTINPUTDEVICE8 dev, DWORD padNum) | [source/app/platform/win/WinInput.cpp:71](../../source/app/platform/win/WinInput.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144) |
| diMouseInit(bool exclusive) | [source/app/platform/win/WinInput.cpp:134](../../source/app/platform/win/WinInput.cpp#L134) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| diPadInitBody() | [source/app/platform/win/WinInput.cpp:158](../../source/app/platform/win/WinInput.cpp#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x007477C0: fuera del catálogo consultado; ruta efectiva sin resolver; Posible dato fijo AllValidWinJoys: source/game_sa/ControllerConfigManager.h:170 (0x00C92144); Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| diPadInit() | [source/app/platform/win/WinInput.cpp:204](../../source/app/platform/win/WinInput.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: Input::diPadInit 0x007485C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| EnumDevicesCallback(LPCDIDEVICEINSTANCEA inst, LPVOID) | [source/app/platform/win/WinInput.cpp:217](../../source/app/platform/win/WinInput.cpp#L217) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetMouseSetUp() | [source/app/platform/win/WinInput.cpp:251](../../source/app/platform/win/WinInput.cpp#L251) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748) |
| InjectPadHooks() | [source/app/platform/win/WinInput.cpp:303](../../source/app/platform/win/WinInput.cpp#L303) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectHooks() | [source/app/platform/win/WinInput.cpp:310](../../source/app/platform/win/WinInput.cpp#L310) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsKeyPressed(unsigned int keyCode) | [source/app/platform/win/WinInput.cpp:321](../../source/app/platform/win/WinInput.cpp#L321) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsKeyDown(unsigned int keyCode) | [source/app/platform/win/WinInput.cpp:325](../../source/app/platform/win/WinInput.cpp#L325) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1388"></a>

## cBuoyancy

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cBuoyancy::InjectHooks() | [source/game_sa/Buoyancy.cpp:50](../../source/game_sa/Buoyancy.cpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::ProcessBuoyancy(CPhysical* entity, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) | [source/game_sa/Buoyancy.cpp:67](../../source/game_sa/Buoyancy.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::ProcessBuoyancy 0x006C3EF0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::ProcessBuoyancyBoat(CVehicle* vehicle, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce, bool bUnderwater) | [source/game_sa/Buoyancy.cpp:107](../../source/game_sa/Buoyancy.cpp#L107) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::ProcessBuoyancyBoat 0x006C3030 | 0 / 0 | Posible dato fijo afBoatVolumeDistribution: source/game_sa/Buoyancy.h:61 (0x008D32CC); Posible dato fijo afBoatVolumeDistributionDinghy: source/game_sa/Buoyancy.h:64 (0x008D3338); Posible dato fijo afBoatVolumeDistributionSail: source/game_sa/Buoyancy.h:63 (0x008D3314); Posible dato fijo afBoatVolumeDistributionSpeed: source/game_sa/Buoyancy.h:65 (0x008D335C); Posible dato fijo fBuoyancyBoatNormalWeight: source/game_sa/Buoyancy.cpp:27 (0x008D3384); Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::CalcBuoyancyForce(CPhysical* entity, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce) | [source/game_sa/Buoyancy.cpp:195](../../source/game_sa/Buoyancy.cpp#L195) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::CalcBuoyancyForce 0x006C2750 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::PreCalcSetup(CPhysical* entity, float fBuoyancy) | [source/game_sa/Buoyancy.cpp:216](../../source/game_sa/Buoyancy.cpp#L216) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::PreCalcSetup 0x006C2B90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::AddSplashParticles(CPhysical* entity, CVector vecFrom, CVector vecTo, CVector vecSplashDir, uint8 bReduceParticleSize) | [source/game_sa/Buoyancy.cpp:325](../../source/game_sa/Buoyancy.cpp#L325) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::AddSplashParticles 0x006C34E0 | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo fBuoyancySplashMaxSpeed: source/game_sa/Buoyancy.cpp:29 (0x008D3388); Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:33 (0x00A9AE00) |
| cBuoyancy::SimpleCalcBuoyancy(CPhysical* entity) | [source/game_sa/Buoyancy.cpp:422](../../source/game_sa/Buoyancy.cpp#L422) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::SimpleCalcBuoyancy 0x006C3B00 | 0 / 0 | Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::SimpleSumBuoyancyData(CVector* vecWaterOffset, tWaterLevel ePointState) | [source/game_sa/Buoyancy.cpp:505](../../source/game_sa/Buoyancy.cpp#L505) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::SimpleSumBuoyancyData 0x006C2970 | 0 / 0 | Posible dato fijo calcStruct: source/game_sa/Buoyancy.h:60 (0x00C1C858); Posible dato fijo fPointVolMultiplier: source/game_sa/Buoyancy.h:59 (0x008D32C8) |
| cBuoyancy::FindWaterLevel(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState) | [source/game_sa/Buoyancy.cpp:546](../../source/game_sa/Buoyancy.cpp#L546) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::FindWaterLevel 0x006C2810 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cBuoyancy::FindWaterLevelNorm(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState, CVector* outVecNormal) | [source/game_sa/Buoyancy.cpp:566](../../source/game_sa/Buoyancy.cpp#L566) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cBuoyancy::FindWaterLevelNorm 0x006C28C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1389"></a>

## cHandlingDataMgr

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cHandlingDataMgr::InjectHooks() | [source/game_sa/cHandlingDataMgr.cpp:32](../../source/game_sa/cHandlingDataMgr.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::LoadHandlingData() | [source/game_sa/cHandlingDataMgr.cpp:204](../../source/game_sa/cHandlingDataMgr.cpp#L204) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::LoadHandlingData 0x005BD830 | 0 / 0 | Posible dato fijo VehicleNames: source/game_sa/cHandlingDataMgr.cpp:24 (0x008D3978); Posible dato fijo m_vehicleAnimGroups: source/game_sa/VehicleAnimGroupData.h:146 (0x00C1CDC0) |
| cHandlingDataMgr::Initialise() | [source/game_sa/cHandlingDataMgr.cpp:359](../../source/game_sa/cHandlingDataMgr.cpp#L359) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetFlyingPointer(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:370](../../source/game_sa/cHandlingDataMgr.cpp#L370) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetFlyingPointer 0x006F52D0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetBoatPointer(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:378](../../source/game_sa/cHandlingDataMgr.cpp#L378) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetBoatPointer 0x006F5300 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetVehiclePointer(uint32 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:385](../../source/game_sa/cHandlingDataMgr.cpp#L385) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * cHandlingDataMgr::GetBikeHandlingPointer(uint32 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:389](../../source/game_sa/cHandlingDataMgr.cpp#L389) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::HasFrontWheelDrive(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:396](../../source/game_sa/cHandlingDataMgr.cpp#L396) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::HasFrontWheelDrive 0x006A0480 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::HasRearWheelDrive(uint8 handlingId) | [source/game_sa/cHandlingDataMgr.cpp:401](../../source/game_sa/cHandlingDataMgr.cpp#L401) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::HasRearWheelDrive 0x006A04B0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::GetHandlingId(const char* nameToFind) | [source/game_sa/cHandlingDataMgr.cpp:407](../../source/game_sa/cHandlingDataMgr.cpp#L407) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::GetHandlingId 0x006F4FD0 | 0 / 0 | Posible dato fijo VehicleNames: source/game_sa/cHandlingDataMgr.cpp:24 (0x008D3978) |
| cHandlingDataMgr::ConvertDataToWorldUnits(tHandlingData* h) | [source/game_sa/cHandlingDataMgr.cpp:434](../../source/game_sa/cHandlingDataMgr.cpp#L434) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertDataToWorldUnits 0x006F5010 | 0 / 0 | Posible dato fijo s_HandlingAccelConst: source/game_sa/cHandlingDataMgr.cpp:419 (0x00C2B9B4); Posible dato fijo s_HandlingVelocityConst: source/game_sa/cHandlingDataMgr.cpp:420 (0x00C2B9BC) |
| cHandlingDataMgr::ConvertDataToGameUnits(tHandlingData* h) | [source/game_sa/cHandlingDataMgr.cpp:449](../../source/game_sa/cHandlingDataMgr.cpp#L449) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertDataToGameUnits 0x006F5080 | 0 / 0 | Posible dato fijo s_HandlingAccelConst: source/game_sa/cHandlingDataMgr.cpp:419 (0x00C2B9B4); Posible dato fijo s_HandlingVelocityConst: source/game_sa/cHandlingDataMgr.cpp:420 (0x00C2B9BC) |
| cHandlingDataMgr::ConvertBikeDataToWorldUnits(tBikeHandlingData* bikeHandling) | [source/game_sa/cHandlingDataMgr.cpp:461](../../source/game_sa/cHandlingDataMgr.cpp#L461) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertBikeDataToWorldUnits 0x006F5240 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::ConvertBikeDataToGameUnits(tBikeHandlingData* bikeHandling) | [source/game_sa/cHandlingDataMgr.cpp:469](../../source/game_sa/cHandlingDataMgr.cpp#L469) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::ConvertBikeDataToGameUnits 0x006F5290 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cHandlingDataMgr::FindExactWord(const char* name, const char* nameTable, uint32 entrySize, uint32 entryCount) | [source/game_sa/cHandlingDataMgr.cpp:477](../../source/game_sa/cHandlingDataMgr.cpp#L477) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cHandlingDataMgr::FindExactWord 0x006F4F30 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1390"></a>

## cTransmission

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| cTransmission::InjectHooks() | [source/game_sa/cTransmission.cpp:24](../../source/game_sa/cTransmission.cpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::DisplayGearRatios() | [source/game_sa/cTransmission.cpp:42](../../source/game_sa/cTransmission.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::DisplayGearRatios 0x006D0590 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::InitGearRatios() | [source/game_sa/cTransmission.cpp:60](../../source/game_sa/cTransmission.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::InitGearRatios 0x006D0460 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateGearForSimpleCar(float speed, uint8& currentGear) | [source/game_sa/cTransmission.cpp:93](../../source/game_sa/cTransmission.cpp#L93) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateGearForSimpleCar 0x006D0530 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| cTransmission::CalculateDriveAcceleration(const float& gasPedal, uint8& currentGear, float& gearChangeCount, float& velocity, float* a6, float* a7, uint8 allWheelsOnGround, uint8 handlingCheat) | [source/game_sa/cTransmission.cpp:112](../../source/game_sa/cTransmission.cpp#L112) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: cTransmission::CalculateDriveAcceleration 0x006D05E0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1391"></a>

## detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RegionValueOf(float p) | [source/game_sa/PathFind.cpp:151](../../source/game_sa/PathFind.cpp#L151) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeRand() | [source/game_sa/PathFind.cpp:158](../../source/game_sa/PathFind.cpp#L158) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptWidthScale(uint8 width) | [source/game_sa/PathFind.cpp:825](../../source/game_sa/PathFind.cpp#L825) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ScriptOffsetPos(const CVector& nodePos, float px, float py, float scale) | [source/game_sa/PathFind.cpp:828](../../source/game_sa/PathFind.cpp#L828) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetCoorsOfRegion(size_t p, size_t nareas) | [source/game_sa/PathFind.cpp:2020](../../source/game_sa/PathFind.cpp#L2020) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1392"></a>

## details

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * CPtrNodeDoubleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:4](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeDoubleLink__operator_delete(void* data, size_t sz) | [source/game_sa/Core/PtrNodeDoubleLink.cpp:9](../../source/game_sa/Core/PtrNodeDoubleLink.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * CPtrNodeSingleLink__operator_new(size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:4](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L4) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CPtrNodeSingleLink__operator_delete(void* data, size_t sz) | [source/game_sa/Core/PtrNodeSingleLink.cpp:9](../../source/game_sa/Core/PtrNodeSingleLink.cpp#L9) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1393"></a>

## details::PtrListIterator

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PtrListIterator(NodeType* node) | [source/game_sa/Core/PtrList.h:23](../../source/game_sa/Core/PtrList.h#L23) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator*() | [source/game_sa/Core/PtrList.h:31](../../source/game_sa/Core/PtrList.h#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator-&gt;() | [source/game_sa/Core/PtrList.h:32](../../source/game_sa/Core/PtrList.h#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator++() | [source/game_sa/Core/PtrList.h:34](../../source/game_sa/Core/PtrList.h#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator++(int) | [source/game_sa/Core/PtrList.h:41](../../source/game_sa/Core/PtrList.h#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator==(const PtrListIterator& a, const PtrListIterator& b) | [source/game_sa/Core/PtrList.h:47](../../source/game_sa/Core/PtrList.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1394"></a>

## notsa

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::InjectWindowedModeHooks() | [source/app/platform/win/WindowedMode.cpp:1878](../../source/app/platform/win/WindowedMode.cpp#L1878) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CColModel&gt;() | [source/game_sa/Pools/Pools.h:75](../../source/game_sa/Pools/Pools.h#L75) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CPedIntelligence&gt;() | [source/game_sa/Pools/Pools.h:80](../../source/game_sa/Pools/Pools.h#L80) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CEntryInfoNode&gt;() | [source/game_sa/Pools/Pools.h:91](../../source/game_sa/Pools/Pools.h#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CPointRoute&gt;() | [source/game_sa/Pools/Pools.h:96](../../source/game_sa/Pools/Pools.h#L96) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CNodeRoute&gt;() | [source/game_sa/Pools/Pools.h:104](../../source/game_sa/Pools/Pools.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CTaskAllocator&gt;() | [source/game_sa/Pools/Pools.h:109](../../source/game_sa/Pools/Pools.h#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf&lt;CPedAttractor&gt;() | [source/game_sa/Pools/Pools.h:114](../../source/game_sa/Pools/Pools.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:123](../../source/game_sa/Pools/Pools.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:129](../../source/game_sa/Pools/Pools.h#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:135](../../source/game_sa/Pools/Pools.h#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:141](../../source/game_sa/Pools/Pools.h#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:147](../../source/game_sa/Pools/Pools.h#L147) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:153](../../source/game_sa/Pools/Pools.h#L153) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & PoolOf() | [source/game_sa/Pools/Pools.h:159](../../source/game_sa/Pools/Pools.h#L159) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1395"></a>

## notsa::Logging

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::Logging::Logging() | [source/app/app_debug.cpp:276](../../source/app/app_debug.cpp#L276) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::~Logging() | [source/app/app_debug.cpp:300](../../source/app/app_debug.cpp#L300) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::Shutdown() | [source/app/app_debug.cpp:304](../../source/app/app_debug.cpp#L304) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| notsa::Logging::Create(std::string name, std::optional&lt;spdlog::level::level_enum&gt; level) -&gt; notsa::log_ptr | [source/app/app_debug.cpp:316](../../source/app/app_debug.cpp#L316) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1396"></a>

## notsa::Route

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetAll() | [source/game_sa/Route.hpp:13](../../source/game_sa/Route.hpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAll() const | [source/game_sa/Route.hpp:14](../../source/game_sa/Route.hpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEmpty() const | [source/game_sa/Route.hpp:17](../../source/game_sa/Route.hpp#L17) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsFull() const | [source/game_sa/Route.hpp:20](../../source/game_sa/Route.hpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Add(T_Entries&&... entry) | [source/game_sa/Route.hpp:24](../../source/game_sa/Route.hpp#L24) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddUnlessFull(const CVector& entry) | [source/game_sa/Route.hpp:27](../../source/game_sa/Route.hpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddMultipleUnlessFull(T_Points&&... entry) | [source/game_sa/Route.hpp:35](../../source/game_sa/Route.hpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Reverse() | [source/game_sa/Route.hpp:38](../../source/game_sa/Route.hpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Clear() | [source/game_sa/Route.hpp:41](../../source/game_sa/Route.hpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetSize() const | [source/game_sa/Route.hpp:44](../../source/game_sa/Route.hpp#L44) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ResizeTo(uint32 s) | [source/game_sa/Route.hpp:47](../../source/game_sa/Route.hpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & operator\[\](size_t idx) | [source/game_sa/Route.hpp:49](../../source/game_sa/Route.hpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| operator\[\](size_t idx) const | [source/game_sa/Route.hpp:50](../../source/game_sa/Route.hpp#L50) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1397"></a>

## notsa::SDLWrapper

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatformWindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) | [source/app/platform/SDLWrapper.cpp:16](../../source/app/platform/SDLWrapper.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| PumpEvents(HWND, UINT, UINT_PTR, DWORD) | [source/app/platform/SDLWrapper.cpp:31](../../source/app/platform/SDLWrapper.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetWindow() | [source/app/platform/SDLWrapper.cpp:36](../../source/app/platform/SDLWrapper.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize() | [source/app/platform/SDLWrapper.cpp:40](../../source/app/platform/SDLWrapper.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| Terminate() | [source/app/platform/SDLWrapper.cpp:68](../../source/app/platform/SDLWrapper.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessEvents() | [source/app/platform/SDLWrapper.cpp:85](../../source/app/platform/SDLWrapper.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1398"></a>

## notsa::WindowedMode

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetDesktopRes() | [source/app/platform/win/WindowedMode.cpp:165](../../source/app/platform/win/WindowedMode.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MainCameraRebuildRaster(void* camera) | [source/app/platform/win/WindowedMode.cpp:173](../../source/app/platform/win/WindowedMode.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038) |
| AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:254](../../source/app/platform/win/WindowedMode.cpp#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| HookDirect3DDeviceReplacerSA() noexcept | [source/app/platform/win/WindowedMode.cpp:410](../../source/app/platform/win/WindowedMode.cpp#L410) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: WindowedModeHooks::notsa::WindowedMode::HookDirect3DDeviceReplacerSA 0x007F6781 | 14 / 1 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AdjustVideoModeOnResize(int32 w, int32 h) | [source/app/platform/win/WindowedMode.cpp:480](../../source/app/platform/win/WindowedMode.cpp#L480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |

<a id="grupo-1399"></a>

## notsa::WindowedMode::D3D9ProxyDevice

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Proxy_Reset(LPDIRECT3DDEVICE9 self, D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:347](../../source/app/platform/win/WindowedMode.cpp#L347) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |
| Proxy_SetViewport(LPDIRECT3DDEVICE9 self, D3DVIEWPORT9* vp) | [source/app/platform/win/WindowedMode.cpp:374](../../source/app/platform/win/WindowedMode.cpp#L374) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize(LPDIRECT3DDEVICE9 dev) | [source/app/platform/win/WindowedMode.cpp:380](../../source/app/platform/win/WindowedMode.cpp#L380) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1400"></a>

## notsa::WindowedMode::PosChangeNoRecursion

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:157](../../source/app/platform/win/WindowedMode.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:160](../../source/app/platform/win/WindowedMode.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
