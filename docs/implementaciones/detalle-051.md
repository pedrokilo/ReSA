<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1001"></a>

## Globales y helpers: source/app/platform/win/VideoMode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VideoModeInjectHooks() | [source/app/platform/win/VideoMode.cpp:125](../../source/app/platform/win/VideoMode.cpp#L125) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1002"></a>

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

<a id="grupo-1003"></a>

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

<a id="grupo-1004"></a>

## Globales y helpers: source/app/platform/win/WinPlatform.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetGammaWhenExiting() | [source/app/platform/win/WinPlatform.cpp:37](../../source/app/platform/win/WinPlatform.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gbGammaChanged: source/game_sa/Gamma.h:23 (0x00C920C8); Posible dato fijo ms_SavedGamma: source/game_sa/Gamma.h:5 (0x00C8D4C8) |
| Win32InjectHooks() | [source/app/platform/win/WinPlatform.cpp:161](../../source/app/platform/win/WinPlatform.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1005"></a>

## Globales y helpers: source/app/platform/win/WinPs.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetBestRefreshRate(RwUInt32 width, RwUInt32 height, RwUInt32 depth) | [source/app/platform/win/WinPs.cpp:389](../../source/app/platform/win/WinPs.cpp#L389) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GcurSelSS: source/app/platform/win/VideoMode.h:22 (0x00C920F4) |
| WinPsInjectHooks() | [source/app/platform/win/WinPs.cpp:539](../../source/app/platform/win/WinPs.cpp#L539) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1006"></a>

## Globales y helpers: source/app/platform/win/WndProc.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| __MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) | [source/app/platform/win/WndProc.cpp:41](../../source/app/platform/win/WndProc.cpp#L41) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AEAudioHardware: source/game_sa/Audio/Hardware/AEAudioHardware.cpp:30 (0x00B5F8B8); Posible dato fijo ForegroundApp: source/app/app.h:24 (0x008D621C); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo Pads: source/game_sa/Pad.h:111 (0x00B73458); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo RwInitialized: source/game_sa/RenderWare/RenderWare.h:22 (0x00C920E8); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0); Posible dato fijo gbGammaChanged: source/game_sa/Gamma.h:23 (0x00C920C8); Posible dato fijo ms_GammaTable: source/game_sa/Gamma.h:6 (0x00C8DAC8); Posible dato fijo ms_SavedGamma: source/game_sa/Gamma.h:5 (0x00C8D4C8) |
| InjectHooksWndProcStuff() | [source/app/platform/win/WndProc.cpp:289](../../source/app/platform/win/WndProc.cpp#L289) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1007"></a>

## Globales y helpers: source/game_sa/3dMarker.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarker.cpp:39](../../source/game_sa/3dMarker.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetAtomicForMarkerType(e3dMarkerType type) | [source/game_sa/3dMarker.cpp:53](../../source/game_sa/3dMarker.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pRpClumpArray: source/game_sa/3dMarkers.h:57 (0x00C7C6DC) |

<a id="grupo-1008"></a>

## Globales y helpers: source/game_sa/3dMarkers.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeStdSize(float sz, float dist) | [source/game_sa/3dMarkers.cpp:38](../../source/game_sa/3dMarkers.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarkers.cpp:55](../../source/game_sa/3dMarkers.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1009"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:641](../../source/game_sa/Animation/AnimManager.cpp#L641) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:646](../../source/game_sa/Animation/AnimManager.cpp#L646) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:660](../../source/game_sa/Animation/AnimManager.cpp#L660) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1010"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:126](../../source/game_sa/Animation/AnimManager.h#L126) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1011"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractorPedPlacer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| TransformPointOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:34](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| TransformVectorOG(const CMatrix& m, const CVector& v) | [source/game_sa/Attractors/PedAttractorPedPlacer.cpp:43](../../source/game_sa/Attractors/PedAttractorPedPlacer.cpp#L43) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1012"></a>

## Globales y helpers: source/game_sa/Audio/AESmoothFadeThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DsVolumeToDb(int32 dsVolume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:31](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| DbToDsVolume(float volume) | [source/game_sa/Audio/AESmoothFadeThread.cpp:36](../../source/game_sa/Audio/AESmoothFadeThread.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1013"></a>

## Globales y helpers: source/game_sa/Audio/AEStreamThread.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChannelPlayTime(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:30](../../source/game_sa/Audio/AEStreamThread.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ChannelLength(CAEStreamingChannel* channel) | [source/game_sa/Audio/AEStreamThread.cpp:34](../../source/game_sa/Audio/AEStreamThread.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1014"></a>

## Globales y helpers: source/game_sa/Audio/AudioEngine.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SExt(E v) | [source/game_sa/Audio/AudioEngine.cpp:37](../../source/game_sa/Audio/AudioEngine.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CallMgr(M method, A... args) | [source/game_sa/Audio/AudioEngine.cpp:39](../../source/game_sa/Audio/AudioEngine.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AERadioTrackManager: source/game_sa/Audio/Managers/AERadioTrackManager.cpp:30 (0x008CB6F8) |

<a id="grupo-1015"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ClearCollisionSoundEntry(tCollisionSound& entry) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:38](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Status: source/game_sa/Darkel.h:34 (0x0096A704) |
| GetLoopingCollisionMagnitude(const CVector& v, const CVector& t, bool zFirst) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:49](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsEntityModel(const CEntity* entity, ModelIndex mi) | [source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp:60](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1016"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AddCosMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:31](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddSinMulX87(float& v, float a, float r) | [source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1017"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEPedAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PedShirtLogarithmicVolume(float value, float baseVolume) | [source/game_sa/Audio/Entities/AEPedAudioEntity.cpp:723](../../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp#L723) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1018"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * ExeSpeechContextEntry(int16 idx) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:104](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindExeSpeechContext(int16 gCtx) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:109](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeVoiceBank(const tExeSpeechTypeTables& t, int16 voice) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:130](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L130) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ExeRange(const tExeSpeechTypeTables& t, int16 sCtx, int16 voice) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:135](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L135) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * ExeGenRange(int16 sCtx, int16 voice) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:140](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L140) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * SpeechOf(CPed* ped) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:145](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L145) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| & SpeechSlot(int32 idx) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:149](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L149) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo s_PedSpeechSlots: source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.h:170 (0x00B61C38) |
| * GetFacialTask(CEntity* entity) | [source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp:154](../../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp#L154) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1019"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| & GetVehicleAudioSettings(eModelID model) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h:283](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.VehicleAudioSettings.h#L283) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1020"></a>

## Globales y helpers: source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AudioLog10ScaledExe(float input, float factor) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:36](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsSurfaceAudioGrass(eSurfaceType surface) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:57](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L57) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| IsSurfaceWet(eSurfaceType surface) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:61](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_surfaceInfos: source/game_sa/SurfaceInfos_c.h:79 (0x00B79538) |
| GracefullyStopSound(CAESound*& sound) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:65](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StopAndForgetSound(CAESound*& sound) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:72](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L72) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetNumOfContactWheels(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:78](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L78) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeLerp(float lo, float hi, float t) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:91](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L91) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeInlinePiecewise5(float x, const float (&p)\[5\]\[2\]) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:98](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L98) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2Clamp01Exe(float r) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:117](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L117) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PedalToRaw(float v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:122](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L122) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * PlayVehicleSound(CAESoundManager::tSoundPlayParams p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:129](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L129) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AESoundManager: source/game_sa/Audio/Managers/AESoundManager.cpp:29 (0x00B62CB0) |
| PlayGenericSound( CAEVehicleAudioEntity* self, CAESound*& s, eSoundBankSlot slot, eSoundID& currSfx, eSoundID newSfx, float speed, float volume, float rollOff ) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:2050](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L2050) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftDistantSoundVolume(const CPhysical* v, float baseVol) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3473](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3473) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftCameraPoVFactor(const CPhysical* p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3486](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3486) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetAircraftAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3491](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3491) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCRotorSpeedFactor(float p) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3502](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3502) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPropSpeedFactor(float propSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3618](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3618) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePropSpeed(const CPlane* plane, float gas, float brake) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3627](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3627) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CalculatePlanePropFreq(CAEVehicleAudioEntity::tVehicleParams& vp, float ac) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3634](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3634) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeClamp01(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3653](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3653) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropSpeedFactor(float f) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3658](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3658) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropOrientation(const CEntity* e, float& R, float& Q) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3664](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3664) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropCamFactor(const CVector& entityPos, const CEntity* vehicle) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3675](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3675) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExePropDistantVolume(const CVector& entityPos, float pf) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3684](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3684) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliVolFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3915](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3915) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAEAudioUtility::GetPiecewiseLinear 0x004D9D90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliFreqFactor(float rs) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3919](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3919) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliRotorSpeed(const CVehicle* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3924](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3924) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliTilt(const CEntity* v) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3928](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3928) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ExeHeliDamageFreq(const CVehicle* v, float f0) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3933](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3933) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorSpeed(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3943](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3943) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetVolumeFactorForHeliRotor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3948](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3948) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliRotorFreqFactor(float rotorSpeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3959](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3959) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHeliForwardTilt(const CHeli* h) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:3970](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L3970) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetDummyRCAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4371](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4371) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeed(const CTrain* train) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4496](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4496) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainAbsClamp01(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4508](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4508) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetTrainSpeedFactor(const CTrain* train, float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4515](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4515) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G5TrainDummyAcFactor(float trspeed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:4525](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L4525) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessBicycleAudio(CAEVehicleAudioEntity& self, CAEVehicleAudioEntity::tVehicleParams& params, bool player) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5008](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5008) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G2GetFreqForIdle_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, float ratio) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5458](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5458) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3DriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, float revs, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5470](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5470) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| G3NonDriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, cTransmission* tr, float speed) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5474](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5474) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VehAudioPlayerVolume_KeepEcx(const CAEVehicleAudioEntity* self, uint32 /*entryEdx*/, CAEVehicleAudioEntity::tVehicleParams* vp, CAEVehicleAudioEntity::eVehicleEngineSoundType st) | [source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp:5481](../../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp#L5481) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
