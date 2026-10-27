<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Capa de plataforma Ps (lista de 22)

Validadas el 03/10/2026 con **6.720 pares original/candidato dentro de GTA**, todos exactos: 22 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `Ps` | `psInitialize`, `psTerminate`, `psWindowSetText`, `psErrorMessage`, `psWarningMessage`, `psCameraBeginUpdate`, `psCameraShowRaster`, `psTimer`, `psGrabScreen`, `psMouseSetVisibility`, `psMouseSetPos`, `psPathnameCreate`, `psPathnameDestroy`, `psPathGetSeparator`, `psInstallFileSystem`, `psNativeTextureSupport`, `psDebugMessageHandler`, `psAlwaysOnTop`, `psSelectDevice`, `InitialiseLanguage`, `CheckDirectSound`, `GetVideoMemInfo` |

**Diferencias reales corregidas:**
- `psInitialize`:
  - llama a `InitialiseLanguage` y carga los ajustes antes de las comprobaciones;
  - la versión de DirectX sale de `GetDXVersion` (0x745650) en vez de un `LoadLibrary("D3D9.DLL")` propio;
  - NT anterior a la 4 y plataformas desconocidas quedan como Windows 95 (antes `WIN_NT` o `NOTSA_UNREACHABLE`);
  - avisos con `MessageBoxA` (no `W`), título pedido antes que el texto;
  - el fabricante de CPU se escribe en el global del EXE (0x8D6238), sin `CoInitialize` propio ni `VERIFY`, y termina con `TheText.Unload(false)`.
- `CheckDirectSound`: `CoInitialize(NULL)` al principio, `DirectSoundCreate8` (ordinal 11) y `GetCaps` solo vale si devuelve exactamente 0.
- `InitialiseLanguage`: repite las tres consultas de idioma como el EXE.
- `psTimer`: `timeGetDevCaps`/`timeBeginPeriod`/`timeGetTime`/`timeEndPeriod` (antes `OS_TimeMS`, basado en QPC).
- `psGrabScreen`: quitados los `FIX_BUGS` (siempre `GetWindowRect` en ventana) y el `UnlockRect`/`Release`/`delete` de la superficie, que el EXE no hace.
- `psMouseSetPos`: truncamiento `_ftol2` de 64 bits y `lastMousePos` guarda la posición truncada a entero.
- `psDebugMessageHandler`: es un `RwDebugHandler` (`OutputDebugStringA(str)` y `"\n"`), no un manejador de eventos Rs.
- `psPathnameCreate`: `strlen`/`strcpy`/`strchr` de RenderWare y `Malloc(len+1, 0)`.
- `psSelectDevice` (y `GetBestRefreshRate` 0x7460A0):
  - guarda el `rdtsc` en 0xC92104/08 y pide la lista de modos antes de mirar el modo guardado;
  - la búsqueda de 800x600x32 avanza `GcurSelVM` desde el modo actual, respeta `IsVMNotSelected` y usa `MB_OK` (quitada la resolución nativa `FIX_BUGS`);
  - `GetVideoModeInfo` antes de `SetVideoMode`; refresco con SDK 31, modos en orden ascendente, centinela 0x7FFFFFFF y `Release` antes de devolver.
- Tipos de retorno/argumento `RwBool` (eax completo) en `psCameraBeginUpdate`, `psInstallFileSystem`, `psNativeTextureSupport`, `psAlwaysOnTop`, `psSelectDevice` y `psMouseSetVisibility`; `psAlwaysOnTop` devuelve el `BOOL` de `SetWindowPos`.

**Método:**
- API de Windows y DirectX simuladas con traza parcheando, solo durante la llamada, la IAT del EXE (ddraw, dsound y d3d9 son importaciones diferidas) y la de la DLL; los demás hilos pasan a la función real.
- Objetos COM falsos para DirectDraw, DirectSound, Direct3D9, el dispositivo y la superficie; RW, `CText`, `CFileMgr`, `C_PcSave`, `CMenuManager::LoadSettings`, `GetVideoModeList` y `GetDXVersion` con `EntryPatch` en el EXE y en la DLL; `rwfree` se sustituye en `RwEngineInstance`.
- `RsGlobal.ps` apunta a un bloque Ps falso; ni ventana, ni modo de vídeo, ni cursor, ni dispositivo, ni cámara reales se tocan.
- Se guardan y restauran en cada ejecución `RsGlobal`, `PsGlobal`, `s_OSStatus`, el fabricante de CPU, `gGameState`, `CMenuManager`, `CLoadingScreen::m_bActive`, `Scene.m_pRwCamera`, `GsubSysInfo` y los globals de selección de vídeo.
- NOTSA conservados y fuera de la comparación: `SetProcessDPIAware` en `psInitialize`, `SDLWrapper::Initialize` + `SDL_SetWindowSize` en `psSelectDevice` (compilación con SDL3) y la opción `SkipDeviceSelection` (forzada a falso). El diálogo de selección usa el procedimiento de la DLL; solo se compara si hay procedimiento.

Repetir: `python reversing/Ps22/build_probe.py` y `python reversing/run.py --case ps22`. Evidencia: [validation-summary.json](validation-summary.json).
