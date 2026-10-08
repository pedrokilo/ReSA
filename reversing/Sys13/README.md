<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tipos de peatón, temporizador y remolque (lista de 13)

Validadas el 02/10/2026 con **15.872 pares original/candidato dentro de GTA**, todos exactos: 39 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CPedType` | `Initialise`, `Shutdown`, `LoadPedData`, `Load`, `Save`, `FindPedType`, `GetPedFlag`, `GetPedTypeAcquaintances` (entrada y valor), `SetPedTypeAsAcquaintance`, `ClearPedTypeAcquaintances`, `ClearPedTypeAsAcquaintance`, `PoliceDontCareAboutCrimesAgainstPedType` |
| `CTimer` | `Initialise`, `Shutdown`, `Suspend`, `Resume`, `Stop`, `StartUserPause`, `EndUserPause`, `GetCyclesPerMillisecond`, `GetCyclesPerFrame`, `GetCurrentTimeInCycles`, `GetIsSlowMotionActive`, `UpdateVariables`, `Update` |
| `CTrailer` | Constructor, `ScanForTowLink`, `SetupSuspensionLines`, `SetTowLink`, `BreakTowLink`, `ResetSuspension`, `ProcessSuspension`, `ProcessEntityCollision`, `ProcessControl`, `ProcessAI`, `PreRender`, `GetTowHitchPos`, `GetTowBarPos` |

**Diferencias reales corregidas:**
- `CPedType`:
  - `Initialise` reserva con `new[]` y cookie (0x284 bytes) y vuelve a poner la tabla a cero; la copia NOTSA de `MayhemCheat` ya no se reserva aparte (almacenamiento propio);
  - `Shutdown` libera el bloque con cookie (`delete[]` de puntero − 4) y no toca la copia NOTSA;
  - `LoadPedData` empieza en el tipo 32 (fuera de la tabla), compara las palabras clave exactas (no por prefijo) y usa `sscanf`/`strtok` como el EXE;
  - `FindPedType` devuelve `PED_TYPE_COUNT` si no encuentra el nombre (la DLL abortaba);
  - `GetPedFlag` compara con signo y desplaza con los 5 bits bajos; `Load` y `Save` devuelven `true` en `al`.
- `CTimer`:
  - `Initialise` comprueba el BOOL de `QueryPerformanceFrequency` y divide solo la parte baja entre 1000;
  - `UpdateVariables`: divisor y contadores cargados con `fild` (sin signo), suma en float y `_ftol2`; paso no recortado `q · 0,05` (no `q / 20`); los ms se truncan antes de limitar a 300 (sin signo); un NaN deja el paso en 3,0;
  - `Update`: FPS, diferencia de 64 bits a float con un solo redondeo y contador del modo pausa como el EXE;
  - `GetCyclesPerFrame`: `fild` + `fmul 20` + `_ftol2`.
- `CTrailer`:
  - `ProcessAI` comprueba el estado remolcado (10, la DLL miraba el 8) y calcula el giro del remolque de equipaje con `fpatan` y los redondeos del x87 del EXE;
  - `SetTowLink`: `UpdateTractorLink` sobre el tractor, rumbo del tractor (no el propio) y recogida de soportes solo si es mayor que −1000 (la DLL hacía un `clamp` con límites invertidos);
  - `ProcessSuspension`: tres pasadas (muelles, velocidades, amortiguación), rueda 2 (`MISSING`, no `BURST`) sin contacto, condición con NaN como el EXE y tracción con la global 0xC1CB28 (no `0.004f`);
  - `ProcessEntityCollision`: soportes después del bucle de ruedas, referencia de rueda sin limpiar la anterior, sin copiar la iluminación del vehículo, triángulos repuestos solo si el valor guardado (con signo) es ≥ 0, `CColPoint` copiado como 0x40FC80 y `SetHasHitWall` siempre con puntos de colisión;
  - `SetupSuspensionLines`: `|y1 + y0|`, altura `P + (rueda/2 − z)`, las cuatro posiciones de rueda y el radio con `|min| > |max|`;
  - `ProcessControl`: el tramo lento de extensión no se limita a 1; `PreRender`: resta 2π una vez (no `fmod`), producto `(z·fz + y·fy) + x·fx` y número de líneas con signo;
  - `GetTowHitchPos`/`GetTowBarPos` transforman con el orden de sumas de 0x59C890; el constructor ya no pone a cero los `CColPoint` de los soportes.

**Método:**
- `CPedType` trabaja sobre una tabla falsa de 36 entradas (el puntero 0xC0BBE8 se cambia solo durante la llamada); `PED.DAT` se genera línea a línea simulando `OpenFile`/`ReadLine`/`CloseFile` (con `LoadLine` real). Se simulan el búfer de partida y `operator new[]`/`delete[]` (solo durante la llamada).
- `CTimer`: todas las globales del reloj y `bSnapShotActive` se guardan y se restauran en cada ejecución; `QueryPerformanceCounter`/`Frequency` y `RsTimer` devuelven valores fijos solo durante la llamada. El puntero de la función de reloj se compara por nombre (EXE 0x5617A0 = `GetOSWPerformanceTime`, 0x5617C0 = `GetMillisecondTime`). Ninguna función tiene `HookInstall` permanente y los hooks se reponen al terminar.
- `CTrailer` usa remolque, tractores y entidades falsos con vtables de trampas (huecos 59, 60 y 61 simulados), model infos, colmodel, líneas, marcos RW y handling falsos; los punteros de `ms_modelInfoPtrs` se cambian solo durante la llamada. Las funciones base de `CAutomobile` se simulan en el EXE y en el cuerpo de la DLL (tomado del hook virtual); el constructor de `CAutomobile` de la DLL es la primera llamada de `CTrailer::CTrailer`, cruzada con la de `CAutomobile::Constructor`.
- Se simulan y trazan muelles, velocidades, amortiguación, pares de ruedas, registros de colisión, referencias, listas de movimiento, audio, colocación, búsqueda de objetos, reproducción, ruedas del model info y `CMemoryMgr::Malloc`; `CMatrix`, `RwMatrixTransform`, `GetColModel` y `CDamageManager` corren de verdad.
- Fuera de los fixtures:
  - líneas de `PED.DAT` con palabras de 32 caracteres o más (el EXE desborda su búfer);
  - matrices nulas donde el EXE las lee sin comprobar y cajas de colisión con NaN (`-min.x` frente a `min.x · −1` solo cambia el signo del NaN);
  - la copia NOTSA de `MayhemCheat` en `Initialise`/`Shutdown`.

Repetir: `python reversing/Sys13/build_probe.py` y `python reversing/run.py --case sys13`. Evidencia: [validation-summary.json](validation-summary.json).
