# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1061"></a>

## Globales y helpers: source/app/platform/preset_view.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| NegateThroughX87(float value) | [source/app/platform/preset_view.cpp:31](../../source/app/platform/preset_view.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RsSetNextPresetView(RwCamera* camera) | [source/app/platform/preset_view.cpp:71](../../source/app/platform/preset_view.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:28 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:27 (0x00C17080) |
| RsSetPreviousPresetView(RwCamera* camera) | [source/app/platform/preset_view.cpp:88](../../source/app/platform/preset_view.cpp#L88) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:28 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:27 (0x00C17080) |
| * RsGetPresetViewDescription() | [source/app/platform/preset_view.cpp:118](../../source/app/platform/preset_view.cpp#L118) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CurrentPresetView: source/app/platform/preset_view.cpp:28 (0x008D2E30); Posible dato fijo NumPresetViews: source/app/platform/preset_view.cpp:27 (0x00C17080); Posible dato fijo PresetViews: source/app/platform/preset_view.cpp:26 (0x00C1707C) |

<a id="grupo-1062"></a>

## Globales y helpers: source/app/platform/win/DelayLoadThunks.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DelayLoadThunksInjectHooks() | [source/app/platform/win/DelayLoadThunks.cpp:51](../../source/app/platform/win/DelayLoadThunks.cpp#L51) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1063"></a>

## Globales y helpers: source/app/platform/win/ImportThunks.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Slot(uintptr_t slot) | [source/app/platform/win/ImportThunks.cpp:76](../../source/app/platform/win/ImportThunks.cpp#L76) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ImportThunksInjectHooks() | [source/app/platform/win/ImportThunks.cpp:164](../../source/app/platform/win/ImportThunks.cpp#L164) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1064"></a>

## Globales y helpers: source/app/platform/win/QTMLRuntime.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AcquireInitTermMutex() | [source/app/platform/win/QTMLRuntime.cpp:45](../../source/app/platform/win/QTMLRuntime.cpp#L45) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReleaseInitTermMutex(HANDLE mutex) | [source/app/platform/win/QTMLRuntime.cpp:54](../../source/app/platform/win/QTMLRuntime.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| QTMLRuntimeInjectHooks() | [source/app/platform/win/QTMLRuntime.cpp:250](../../source/app/platform/win/QTMLRuntime.cpp#L250) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1065"></a>

## Globales y helpers: source/app/platform/win/VideoMode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VideoModeInjectHooks() | [source/app/platform/win/VideoMode.cpp:131](../../source/app/platform/win/VideoMode.cpp#L131) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1066"></a>

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
| InitDialog(HWND hDlg) | [source/app/platform/win/VideoModeSelectDialog.cpp:113](../../source/app/platform/win/VideoModeSelectDialog.cpp#L113) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:41 (0x00BA6748); Posible dato fijo GcurSelVM: source/app/platform/win/VideoMode.h:32 (0x008D6220) |
| DialogFunc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) | [source/app/platform/win/VideoModeSelectDialog.cpp:141](../../source/app/platform/win/VideoModeSelectDialog.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo GcurSelSS: source/app/platform/win/VideoMode.h:22 (0x00C920F4); Posible dato fijo GcurSelVM: source/app/platform/win/VideoMode.h:32 (0x008D6220) |
| CreateVidModeSelectDialog(HINSTANCE hInst, HWND hWnd) | [source/app/platform/win/VideoModeSelectDialog.cpp:194](../../source/app/platform/win/VideoModeSelectDialog.cpp#L194) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1067"></a>

## Globales y helpers: source/app/platform/win/WinMain.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Win32_InitApplication(HINSTANCE hInstance) | [source/app/platform/win/WinMain.cpp:105](../../source/app/platform/win/WinMain.cpp#L105) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00747EB0 |
| Win32_InitInstance(HINSTANCE hInstance) | [source/app/platform/win/WinMain.cpp:121](../../source/app/platform/win/WinMain.cpp#L121) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| ProcessGameLogic(INT nCmdShow) | [source/app/platform/win/WinMain.cpp:178](../../source/app/platform/win/WinMain.cpp#L178) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo AudioEngine: source/game_sa/Audio/AudioEngine.cpp:31 (0x00B6BC90); Posible dato fijo ControlsManager: source/game_sa/ControllerConfigManager.cpp:28 (0x00B70198); Posible dato fijo ForegroundApp: source/app/app.h:24 (0x008D621C); Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:41 (0x00BA6748); Posible dato fijo IsVMNotSelected: source/app/platform/win/VideoMode.h:41 (0x008D6218); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo RwInitialized: source/game_sa/RenderWare/RenderWare.h:22 (0x00C920E8); Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo anisotropySupportedByGFX: source/app/platform/win/WinPlatform.h:42 (0x00C87FFC); Posible dato fijo gColourTop: source/app/app.h:26 (0x00B72CA0); Posible dato fijo gCurrentVideoMode: source/app/platform/win/VideoMode.h:48 (0x008D6220); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0); Posible dato fijo isForeground: source/app/platform/win/WinPlatform.h:43 (0x00C920EC) |
| MainLoop(INT nCmdShow) | [source/app/platform/win/WinMain.cpp:357](../../source/app/platform/win/WinMain.cpp#L357) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:41 (0x00BA6748); Posible dato fijo RwInitialized: source/game_sa/RenderWare/RenderWare.h:22 (0x00C920E8); Posible dato fijo gamma: source/game_sa/Gamma.h:25 (0x00C92134) |
| WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) | [source/app/platform/win/WinMain.cpp:531](../../source/app/platform/win/WinMain.cpp#L531) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| InjectWinMainStuff() | [source/app/platform/win/WinMain.cpp:552](../../source/app/platform/win/WinMain.cpp#L552) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1068"></a>

## Globales y helpers: source/app/platform/win/WinPlatform.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetGammaWhenExiting() | [source/app/platform/win/WinPlatform.cpp:37](../../source/app/platform/win/WinPlatform.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gbGammaChanged: source/game_sa/Gamma.h:24 (0x00C920C8); Posible dato fijo ms_SavedGamma: source/game_sa/Gamma.h:5 (0x00C8D4C8) |
| Win32InjectHooks() | [source/app/platform/win/WinPlatform.cpp:161](../../source/app/platform/win/WinPlatform.cpp#L161) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1069"></a>

## Globales y helpers: source/app/platform/win/WinPs.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| WinPsInjectHooks() | [source/app/platform/win/WinPs.cpp:626](../../source/app/platform/win/WinPs.cpp#L626) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1070"></a>

## Globales y helpers: source/app/platform/win/WindowedMode.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| DXCHECK(HRESULT hr) | [source/app/platform/win/WindowedMode.cpp:19](../../source/app/platform/win/WindowedMode.cpp#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1071"></a>

## Globales y helpers: source/app/platform/win/WndProc.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| InjectHooksWndProcStuff() | [source/app/platform/win/WndProc.cpp:302](../../source/app/platform/win/WndProc.cpp#L302) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1072"></a>

## Globales y helpers: source/game_sa/3dMarker.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarker.cpp:39](../../source/game_sa/3dMarker.cpp#L39) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetAtomicForMarkerType(e3dMarkerType type) | [source/game_sa/3dMarker.cpp:53](../../source/game_sa/3dMarker.cpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_pRpClumpArray: source/game_sa/3dMarkers.h:57 (0x00C7C6DC) |

<a id="grupo-1073"></a>

## Globales y helpers: source/game_sa/3dMarkers.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ComputeStdSize(float sz, float dist) | [source/game_sa/3dMarkers.cpp:38](../../source/game_sa/3dMarkers.cpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MarkerWithUp(const CVector& up) | [source/game_sa/3dMarkers.cpp:55](../../source/game_sa/3dMarkers.cpp#L55) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1074"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendAssociationDestructorBodyExe(CAnimBlendAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendAssociation.cpp:37](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L37) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall CAnimBlendAssociationScalarDeletingDestructor(CAnimBlendAssociation* self, void* /*edx*/, uint8 flags) | [source/game_sa/Animation/AnimBlendAssociation.cpp:52](../../source/game_sa/Animation/AnimBlendAssociation.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAnimBlendAssociation::DestructorBody 0x004CECF0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1075"></a>

## Globales y helpers: source/game_sa/Animation/AnimBlendStaticAssociation.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAnimBlendStaticAssociationDestructorBodyExe(CAnimBlendStaticAssociation* self, void* /*edx*/) | [source/game_sa/Animation/AnimBlendStaticAssociation.cpp:27](../../source/game_sa/Animation/AnimBlendStaticAssociation.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1076"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RoundSectionSize(uint32 size) | [source/game_sa/Animation/AnimManager.cpp:672](../../source/game_sa/Animation/AnimManager.cpp#L672) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * NewAnimSequences(int32 n) | [source/game_sa/Animation/AnimManager.cpp:677](../../source/game_sa/Animation/AnimManager.cpp#L677) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FloatToKeyFrameWord(float v) | [source/game_sa/Animation/AnimManager.cpp:691](../../source/game_sa/Animation/AnimManager.cpp#L691) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1077"></a>

## Globales y helpers: source/game_sa/Animation/AnimManager.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| IsClumpSkinned(RpClump *clump) | [source/game_sa/Animation/AnimManager.h:127](../../source/game_sa/Animation/AnimManager.h#L127) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1078"></a>

## Globales y helpers: source/game_sa/AttractorScanner.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AttractorSectorCell(float coordinate, bool useEnd) | [source/game_sa/AttractorScanner.cpp:77](../../source/game_sa/AttractorScanner.cpp#L77) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1079"></a>

## Globales y helpers: source/game_sa/Attractors/PedAtmAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall PedATMAttractorConstructor(CPedATMAttractor* self, void* /*edx*/, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState) | [source/game_sa/Attractors/PedAtmAttractor.cpp:27](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L27) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedATMAttractorDestructorBody(CPedATMAttractor* self, void* /*edx*/) | [source/game_sa/Attractors/PedAtmAttractor.cpp:32](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedATMAttractor::DestructorBody 0x005EE530 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall PedATMAttractorScalarDeletingDestructor(CPedATMAttractor* self, void* /*edx*/, uint8 flags) | [source/game_sa/Attractors/PedAtmAttractor.cpp:42](../../source/game_sa/Attractors/PedAtmAttractor.cpp#L42) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CPedATMAttractor::ScalarDeletingDestructor 0x005EE4E0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1080"></a>

## Globales y helpers: source/game_sa/Attractors/PedAttractor.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SArrayPushBack4(SArrayRaw&lt;uint32&gt;* self, const uint32* x) | [source/game_sa/Attractors/PedAttractor.cpp:621](../../source/game_sa/Attractors/PedAttractor.cpp#L621) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
