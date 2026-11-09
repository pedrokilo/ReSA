<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 872 grupos / 9633 funciones reconstruidas; 9633 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1501"></a>

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

<a id="grupo-1502"></a>

## notsa::SDLWrapper

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PlatformWindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) | [source/app/platform/SDLWrapper.cpp:16](../../source/app/platform/SDLWrapper.cpp#L16) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| PumpEvents(HWND, UINT, UINT_PTR, DWORD) | [source/app/platform/SDLWrapper.cpp:31](../../source/app/platform/SDLWrapper.cpp#L31) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * GetWindow() | [source/app/platform/SDLWrapper.cpp:36](../../source/app/platform/SDLWrapper.cpp#L36) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize() | [source/app/platform/SDLWrapper.cpp:40](../../source/app/platform/SDLWrapper.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |
| Terminate() | [source/app/platform/SDLWrapper.cpp:68](../../source/app/platform/SDLWrapper.cpp#L68) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ProcessEvents() | [source/app/platform/SDLWrapper.cpp:85](../../source/app/platform/SDLWrapper.cpp#L85) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo FrontEndMenuManager: source/game_sa/MenuManager.cpp:40 (0x00BA6748); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040) |

<a id="grupo-1503"></a>

## notsa::WindowedMode

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetDesktopRes() | [source/app/platform/win/WindowedMode.cpp:165](../../source/app/platform/win/WindowedMode.cpp#L165) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MainCameraRebuildRaster(void* camera) | [source/app/platform/win/WindowedMode.cpp:173](../../source/app/platform/win/WindowedMode.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Scene: source/game_sa/Scene.cpp:3 (0x00C17038) |
| AdjustPresentParams(D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:254](../../source/app/platform/win/WindowedMode.cpp#L254) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); Posible dato fijo RsGlobal: source/game_sa/RenderWare/RenderWare.h:24 (0x00C17040); Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C); Posible dato fijo Windowed: source/app/platform/win/WinPlatform.h:44 (0x00C920CC); Posible dato fijo gGameState: source/app/app.h:19 (0x00C8D4C0) |
| HookDirect3DDeviceReplacerSA() noexcept | [source/app/platform/win/WindowedMode.cpp:410](../../source/app/platform/win/WindowedMode.cpp#L410) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Pendiente en catálogo: WindowedModeHooks::notsa::WindowedMode::HookDirect3DDeviceReplacerSA 0x007F6781 | 14 / 1 | Posible dato fijo Present: source/app/platform/win/WindowedMode.cpp:109 (0x00C9C040); ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| AdjustVideoModeOnResize(int32 w, int32 h) | [source/app/platform/win/WindowedMode.cpp:480](../../source/app/platform/win/WindowedMode.cpp#L480) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |

<a id="grupo-1504"></a>

## notsa::WindowedMode::D3D9ProxyDevice

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Proxy_Reset(LPDIRECT3DDEVICE9 self, D3DPRESENT_PARAMETERS* pp) | [source/app/platform/win/WindowedMode.cpp:347](../../source/app/platform/win/WindowedMode.cpp#L347) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Posible dato fijo DisplayModes: source/app/platform/win/WindowedMode.cpp:120 (0x00C97C48) |
| Proxy_SetViewport(LPDIRECT3DDEVICE9 self, D3DVIEWPORT9* vp) | [source/app/platform/win/WindowedMode.cpp:374](../../source/app/platform/win/WindowedMode.cpp#L374) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Initialize(LPDIRECT3DDEVICE9 dev) | [source/app/platform/win/WindowedMode.cpp:380](../../source/app/platform/win/WindowedMode.cpp#L380) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1505"></a>

## notsa::WindowedMode::PosChangeNoRecursion

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:157](../../source/app/platform/win/WindowedMode.cpp#L157) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ~PosChangeNoRecursion() | [source/app/platform/win/WindowedMode.cpp:160](../../source/app/platform/win/WindowedMode.cpp#L160) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1506"></a>

## notsa::WindowedMode::ScopedRestoreWindowInfo

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ScopedRestoreWindowInfo() noexcept | [source/app/platform/win/WindowedMode.cpp:308](../../source/app/platform/win/WindowedMode.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C) |
| ~ScopedRestoreWindowInfo() noexcept | [source/app/platform/win/WindowedMode.cpp:324](../../source/app/platform/win/WindowedMode.cpp#L324) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo WindowHandle: source/app/platform/win/WindowedMode.cpp:106 (0x00C97C1C) |

<a id="grupo-1507"></a>

## notsa::detail

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| VerifyMacroImpl(bool result) | [source/app/app_debug.h:13](../../source/app/app_debug.h#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| AddressOfFunction(T fn) | [source/game_sa/Scripts/CommandParser/Parser.hpp:103](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| NOPCommandHandler(Args... args) | [source/game_sa/Scripts/CommandParser/Parser.hpp:109](../../source/game_sa/Scripts/CommandParser/Parser.hpp#L109) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1508"></a>

## notsa::details::PtrListDoubleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListDoubleLink.h:19](../../source/game_sa/Core/PtrListDoubleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListDoubleLink.h:33](../../source/game_sa/Core/PtrListDoubleLink.h#L33) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListDoubleLink.h:60](../../source/game_sa/Core/PtrListDoubleLink.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1509"></a>

## notsa::details::PtrListSingleLinkTraits

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * AddNode(NodeType*& head, NodeType* node) | [source/game_sa/Core/PtrListSingleLink.h:19](../../source/game_sa/Core/PtrListSingleLink.h#L19) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * UnlinkNode(NodeType*& head, NodeType* node, NodeType* prev) | [source/game_sa/Core/PtrListSingleLink.h:28](../../source/game_sa/Core/PtrListSingleLink.h#L28) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsNodeValid(const NodeType& node) requires std::is_pointer_v&lt;ItemType&gt; | [source/game_sa/Core/PtrListSingleLink.h:47](../../source/game_sa/Core/PtrListSingleLink.h#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1510"></a>

## notsa::envmap

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * PoolNew(Pool* pool) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:104](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L104) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| PoolDelete(Pool* pool, const void* obj) | [source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h:123](../../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h#L123) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1511"></a>

## notsa::script

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetScriptCommandName(eScriptCommands cmd) | [source/game_sa/Enums/eScriptCommands.h:2904](../../source/game_sa/Enums/eScriptCommands.h#L2904) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| CommandHasCustomHandler() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:21](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L21) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GTAProcessCommand(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:40](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetHandlerOfCommand() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:46](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IterateCommandIDs(auto&& functor) | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:56](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GenerateLUT() | [source/game_sa/Scripts/CommandParser/LUTGenerator.hpp:61](../../source/game_sa/Scripts/CommandParser/LUTGenerator.hpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Read(CRunningScript* S) | [source/game_sa/Scripts/CommandParser/ReadArg.hpp:111](../../source/game_sa/Scripts/CommandParser/ReadArg.hpp#L111) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo UsedObjectArray: source/game_sa/Scripts/TheScripts.h:384 (0x00A44B70) |
| StoreArg(CRunningScript* S, T arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:18](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L18) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const T& arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:30](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const CVector& v3) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:53](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L53) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const CVector2D& v2) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:59](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, CompareFlagUpdate flag) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:65](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, T&& value) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:74](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, T value) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:94](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L94) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StoreArg(CRunningScript* S, const MultiRet&lt;Ts...&gt;& arg) | [source/game_sa/Scripts/CommandParser/StoreArg.hpp:102](../../source/game_sa/Scripts/CommandParser/StoreArg.hpp#L102) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| return_multiple(Ts&&... args) | [source/game_sa/Scripts/CommandParser/Utility.hpp:20](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L20) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FixAngleDegrees(float deg) | [source/game_sa/Scripts/CommandParser/Utility.hpp:38](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L38) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GetPedOrItsVehicle(CPed& ped) -&gt; CPhysical& | [source/game_sa/Scripts/CommandParser/Utility.hpp:49](../../source/game_sa/Scripts/CommandParser/Utility.hpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1512"></a>

## notsa::script::commands::basic

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::basic::RegisterHandlers() | [source/game_sa/Scripts/Commands/BasicCommands.cpp:230](../../source/game_sa/Scripts/Commands/BasicCommands.cpp#L230) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1513"></a>

## notsa::script::commands::camera

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::camera::RegisterHandlers() | [source/game_sa/Scripts/Commands/CameraCommands.cpp:308](../../source/game_sa/Scripts/Commands/CameraCommands.cpp#L308) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1514"></a>

## notsa::script::commands::character

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::character::RegisterHandlers() | [source/game_sa/Scripts/Commands/CharacterCommands.cpp:4041](../../source/game_sa/Scripts/Commands/CharacterCommands.cpp#L4041) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1515"></a>

## notsa::script::commands::cleo::audiostream

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::audiostream::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp:89](../../source/game_sa/Scripts/Commands/CLEO/CLEOAudioStreamCommands.cpp#L89) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1516"></a>

## notsa::script::commands::cleo::character

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::character::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOCharacterCommands.cpp:6](../../source/game_sa/Scripts/Commands/CLEO/CLEOCharacterCommands.cpp#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1517"></a>

## notsa::script::commands::cleo::dynamiclibrary

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::dynamiclibrary::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEODynamicLibraryCommands.cpp:30](../../source/game_sa/Scripts/Commands/CLEO/CLEODynamicLibraryCommands.cpp#L30) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1518"></a>

## notsa::script::commands::cleo::fs

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::fs::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOFsCommands.cpp:54](../../source/game_sa/Scripts/Commands/CLEO/CLEOFsCommands.cpp#L54) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1519"></a>

## notsa::script::commands::cleo::game

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::game::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOGameCommands.cpp:14](../../source/game_sa/Scripts/Commands/CLEO/CLEOGameCommands.cpp#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1520"></a>

## notsa::script::commands::cleo::generic

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| notsa::script::commands::cleo::generic::RegisterHandlers() | [source/game_sa/Scripts/Commands/CLEO/CLEOGenericCommands.cpp:47](../../source/game_sa/Scripts/Commands/CLEO/CLEOGenericCommands.cpp#L47) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
