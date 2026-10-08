<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples C (lista de 6)

Validadas el 02/10/2026 con **15.232 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 8 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleSetKindaStayInSamePlace` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleSetPedAsAutoDriver` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleThrowControl` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleTurn180` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleWaitForBus` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleWaitUntilAreaCodesMatch` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleWaitUntilPedIsInCar` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleWaitUntilPedIsOutCar` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |

**Diferencias reales corregidas:**
- `CTaskSimpleSetPedAsAutoDriver::ProcessPed`: `bEngineOn` estaba invertido; el EXE lo enciende salvo que `bEngineBroken` esté activo.
- `CTaskSimpleThrowControl`:
  - el constructor ya no hace `assert` con entidad nula (el EXE la acepta);
  - `ProcessPed` vuelve a llamar a `GetTaskSecondary` en cada uso, como el EXE.
- `CTaskSimpleWaitForBus::ProcessPed`:
  - devuelve `false` y no toca `m_bus` si no encuentra autobús;
  - la velocidad se compara de forma estricta (`0,005² > |v|²`) y antes que las plazas libres.
- `CTaskSimpleWaitUntilPedIsInCar::Clone`: construye desde el peatón (registra la referencia y `m_bIsWalkingToDoor` vuelve a `false`).
- `CTaskSimpleWaitUntilPedIsOutCar::ProcessPed`: el ángulo de mirada usa `GetRadianAngleBetweenPoints` (0x53CBE0), no la versión en grados.

**Método:**
- Peatones, clump, inteligencias con escáner de vehículos, vehículos, entidad, asociaciones de animación, tarea secundaria, tarea de lanzamiento y evento falsos.
- Los constructores corren sobre búferes 0xCD.
- En cada ejecución se restaura el pool de tareas.
- En la ejecución original se desactivan también los constructores de `CTaskSimpleThrowProjectile` y `CTaskSimpleStandStill`.
- Simulados y trazados:
  - animación;
  - movimiento, rumbo, `IsPlayer` y agacharse;
  - las búsquedas y cambios de tareas;
  - `JoinCarWithRoadSystem`;
  - los ángulos de `CGeneral`.

Repetir: `python reversing/STasks6c/build_probe.py` y `python reversing/run.py --case stasks6c`. Evidencia: [validation-summary.json](validation-summary.json).
