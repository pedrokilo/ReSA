<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
