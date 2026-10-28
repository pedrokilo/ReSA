<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples (lista de 19)

Validadas el 02/10/2026 con **18.432 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 10 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleCower`, `CTaskSimpleHandsUp`, `CTaskSimpleUseAtm` | Constructor, Destructor, `Clone`, `IsInterruptable` |
| `CTaskSimpleGoTo` | Constructor, `HasCircledTarget`, `QuitIK`, `SetUpIK` |
| `CTaskSimpleHitHead` | Constructor, `FinishAnimCB`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleInAir` | Constructor, `DeleteAnimCB`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleNone` | `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimplePause` | Constructor, `Clone`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimplePickUpEntity` | Constructor-1, Constructor-2, `Clone`, `GetTaskType` |
| `CTaskSimpleWaitUntilLeaderAreaCodesMatch` | `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleUseAtm`: el tipo de tarea era `TASK_SIMPLE_CAR_SET_TEMP_ACTION` (0x2D3); el EXE usa `TASK_SIMPLE_USE_ATM` (0x1A4). Afecta al constructor y a `Clone`.
- `CTaskSimplePause`:
  - el temporizador ya no se inicializa con `{}`, que ponía a cero el relleno;
  - `ProcessPed` arranca el temporizador sin comprobar `interval >= 0`.
- `CTaskSimpleInAir`:
  - el constructor ya no pone a cero el temporizador;
  - `MakeAbortable` faltaba poner `bIsInTheAir = false`.
- `CTaskSimpleInAir::ProcessPed` tenía seis diferencias:
  - la condición de trepar trata NaN como el EXE;
  - el orden de sumas del producto escalar;
  - los contadores se comparan con signo;
  - el paso de tiempo es `ms_fTimeStep·0,02·1000` truncado;
  - el mínimo de Z se actualiza con un `if` exacto;
  - se acepta una tarea padre nula.
- `CTaskSimpleGoTo`:
  - el constructor conserva los bits 6 y 7 de las banderas;
  - `HasCircledTarget` devuelve falso con una diferencia de altura NaN;
  - `SetUpIK` usa el orden de sumas del EXE y el punto de mirada `(2·dir.z + ped.z) + 0,61`.
- `CTaskSimplePickUpEntity`: los dos constructores ponen a cero la posición de recogida.
- `CTaskSimpleWaitUntilLeaderAreaCodesMatch::Clone`: solo copia el peatón; +0x24 y +0x34 vuelven a cero y +0x28 no se escribe.

**Método:**
- Peatones, clump, inteligencia, asociaciones de animación, tareas padre y eventos falsos.
- Los constructores corren sobre búferes 0xCD.
- En cada ejecución se restaura el pool de tareas.
- En la ejecución original se desactivan también los hooks de los constructores base.
- Simulados y trazados:
  - animación;
  - movimiento y agacharse;
  - `Say` y `ApplyMoveForce`;
  - consultas del mundo y `TestForClimb`;
  - el gestor de IK;
  - las búsquedas de tareas.

Repetir: `python reversing/STasks19/build_probe.py` y `python reversing/run.py --case stasks19`. Evidencia: [validation-summary.json](validation-summary.json).
