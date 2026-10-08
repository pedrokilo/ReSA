<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CGame (lista de 22)

Validadas el 03/10/2026 con **5.824 pares original/candidato dentro de GTA**, todos exactos: 22 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CGame` | `CanSeeOutSideFromCurrArea`, `CanSeeWaterFromCurrArea`, `ValidateVersion`, `CameraDestroy`, `TidyUpMemory`, `DrasticTidyUpMemory`, `ReloadIPLs`, `InitialiseOnceBeforeRW`, `InitialiseEssentialsAfterRW`, `InitialiseCoreDataAfterRW`, `InitialiseRenderWare`, `ShutdownRenderWare`, `FinalShutdown`, `Init1`, `Init2`, `Init3`, `Initialise`, `ReInitGameObjectVariables`, `InitialiseWhenRestarting`, `ShutDownForRestart`, `Shutdown`, `Process` |

**Diferencias reales corregidas:**
- `Init1`:
  - `strcpy` como el EXE (`strcpy_s` rellenaba `aDatFile` con 0xFE en Debug);
  - sin `CMemoryMgr::PushMemId/PopMemId` (el EXE no los llama; escribían el id de memoria y la pila de ids del EXE).
- `Init2`: solo se carga la piel del jugador 1 (`LoadPlayerSkin` de `Players[0]`, no de los dos); `ms_fLODDistance` = 500 (0x43FA0000), no 0; sin log NOTSA.
- `InitialiseEssentialsAfterRW`: sin `PushMemId/PopMemId` (además la salida temprana dejaba la pila desequilibrada).
- `InitialiseRenderWare`:
  - `CameraCreate` en línea con las comprobaciones del EXE (marco, rasters y sus padres); si falla destruye la cámara y deja `Scene.m_pRwCamera` a nulo (antes había `assert` y no se ponía a nulo);
  - guarda el callback de restauración D3D anterior en 0xBD0114 e instala el del EXE (0x5BA120); la DLL reinstalaba el mismo callback.
- `CameraDestroy`: se usa la de `app_camera.cpp`, que lee el padre de cada raster antes de destruirlo; la copia de `Game.cpp` lo leía después (uso tras liberar).
- `ValidateVersion`: `strncpy` (0x821F40) en vez de `strncpy_s`; el fallo de apertura se compara con -1 como el EXE y muestra la pantalla de carga «Invalid version» antes de abortar.
- `ShutDownForRestart`: faltaba el borrado en línea de `CTheZones::ZonesVisited` (100 bytes, sin tocar `ZonesRevealed`) tras limpiar los jugadores.
- `Shutdown`: la lista de edificios procesados de `CCover` se vacía donde lo hace el EXE (tras `CTimeCycle::Shutdown`), no al principio.
- `Process`: paso × 0.02f (0x858B38) para `g_fx` y las criaturas de agua (antes paso / 50); los ciclos se leen antes que ciclos/ms.

**Método:**
- Todas las funciones llamadas (≈330, dirección del EXE y símbolo de la DLL) se sustituyen por thunks trazados genéricos; nunca corre un init/shutdown real ni se reentra en el fotograma real (`Process` incluido).
- Los thunks guardan nombre, `ECX`, cadenas por contenido, vectores por valor y bool/int8 a 8 bits en un registro fijo sin reservas, y devuelven fixtures: bools (salidas tempranas), ciclos del temporizador, slots de TXD, cámara/marco/rasters/mundo RW falsos, fichero y bytes de `peds.col` falsos.
- `Shutdown` usa peatones falsos con vtable trazada para el `delete` del jugador y un `ms_tasks` falso con `operator delete` trazado.
- Los globales que escriben las funciones se rellenan con basura determinista, se comparan por hash y se restauran en cada ejecución.
- Fuera de los fixtures:
  - las rutas de fallo de `ValidateVersion` (el EXE se queda en un bucle infinito; la DLL aborta);
  - las funciones vacías del EXE que la DLL tiene en línea (`PreparePathData`, `AllocatePathFindInfoMem`) y el constructor de `CRGBA` sin uso de `InitialiseWhenRestarting`.

Repetir: `python reversing/Game22/build_probe.py` y `python reversing/run.py --case game22`. Evidencia: [validation-summary.json](validation-summary.json).
