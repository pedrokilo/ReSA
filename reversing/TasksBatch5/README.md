<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
