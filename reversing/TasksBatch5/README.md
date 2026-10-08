<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas `CTaskComplexWait*` y `WalkAlongsidePed` (lote 4)

Validadas el 02/10/2026 con **12.064 pares original/candidato dentro de GTA**, todos exactos: 38 funciones con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexWaitAtAttractor` | 7 |
| `CTaskComplexWaitForBackup` | 8 |
| `CTaskComplexWaitForBus` | 7 |
| `CTaskComplexWaitForDryWeather` | 7 |
| `CTaskComplexWalkAlongsidePed` | 9 |

**Diferencias reales corregidas:**
- `WaitForBackup`:
  - orden de registro y limpieza de referencias (agresor y después refuerzo);
  - rumbo con `GetRadianAngleBetweenPoints` + `LimitRadianAngle`;
  - prueba de distancia invertida en `CreateNextSubTask`;
  - pausa aleatoria en [1000, 2000) con `GetRandomIntInRange`;
  - los tipos desconocidos devuelven nulo;
  - temporizador sin inicialización por valor.
- `WaitForBus`: `MakeAbortable` no se sobrescribe (el original usa el de la clase base).
- `WalkAlongsidePed`:
  - `ControlSubTask` reescrita: punto relativo al peatón, órdenes de suma del EXE, rama de distancia exacta 0,0625 con `SetMoveAnimSpeed`, límites con NaN, comparaciones estrictas;
  - los tipos desconocidos devuelven nulo.
- `CTaskComplexSeekEntity` (envoltorio del hook 0x661DC0, fuera del lote): intercambiaba los dos intervalos. Afectaba a todas las llamadas del EXE.

**Método:**
- Peatones con matriz, inteligencia, datos de jugador y clump falsos.
- Se simulan y se trazan:
  - `FindActiveTaskByType`, `GetMoveStateFromGoToTask`, `RpAnimBlendClumpGetAssociation`;
  - `SetMoveAnimSpeed`, `UpdatePoint`, `FindPlayerPed` y el RNG.
- El reloj y la lluvia se fijan por caso y se restauran.

Repetir: `python reversing/TasksBatch5/build_probe.py` y `python reversing/run.py --case tasks5`. Evidencia: [validation-summary.json](validation-summary.json).
