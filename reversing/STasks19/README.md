<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
