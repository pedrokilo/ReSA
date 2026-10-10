<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas simples A (lista de 6)

Validadas el 02/10/2026 con **21.312 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskSimpleClearLookAt`, `CTaskSimpleDoHandSignal`, `CTaskSimpleDuckToggle` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleCreateCarAndGetIn` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleFall` | Constructor, `FinishFallAnimCB`, `MakeAbortable`, `ProcessFall`, `ProcessPed`, `StartAnim` |
| `CTaskSimpleGiveCPR` | Constructor, `Clone`, `FinishGiveCPRAnimCB`, `MakeAbortable`, `ProcessPed`, `ReviveDeadPed` |
| `CTaskSimpleGoToPoint` | Constructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `UpdatePoint` |

**Diferencias reales corregidas:**
- `CTaskSimpleClearLookAt::Clone` y `CTaskSimpleDuckToggle::Clone`: copiaban la tarea entera, padre incluido; el EXE construye una tarea nueva (DuckToggle solo pasa el modo).
- `CTaskSimpleDuckToggle::ProcessPed`: la tarea más simple puede ser nula (`dyn_cast_if_present`); antes fallaba la aserción.
- `CTaskSimpleDoHandSignal::ProcessPed`: el constructor de `CTaskComplexPlayHandSignalAnim` ya no limpia el bit `m_DoUseFatHands`; el EXE solo limpia los bits 0-2.
- `CTaskSimpleCreateCarAndGetIn`:
  - `Clone` solo copia la posición y el modelo;
  - el destructor ya no pone `m_CreatedVeh` a nulo;
  - `ProcessPed` sigue esperando mientras el vehículo es estático y termina cuando deja de serlo (estaba invertido);
  - `CTaskSimpleCarSetPedInAsDriver` se marca con `m_bWarpingInToCar = true`.
- `CTaskSimpleFall::ProcessPed`: el paso de tiempo es `ms_fTimeStep·0,02·1000` truncado con `_ftol2` (`GetTimeStepInMS` divide entre 50).
- `CTaskSimpleGiveCPR::MakeAbortable`:
  - la prioridad se comprueba antes de leer el evento (con `ABORT_PRIORITY_IMMEDIATE` el evento puede ser nulo);
  - también aborta si la salud del peatón es ≤ 0, con cualquier evento.
- `CTaskSimpleGoToPoint`:
  - el constructor conserva los bits 5-7 de sus banderas;
  - `ProcessPed` calcula distancia, altura y dirección antes de mover al peatón y sus comparaciones tratan NaN como el EXE.

**Método:**
- Peatones, clump, inteligencia, datos de jugador, pad, asociaciones de animación, tareas, evento, accidente e información de arma falsos.
- Dos vehículos falsos dentro de un pool de vehículos falso; el puntero global se cambia solo durante la llamada.
- `CreateCarAndGetIn` nunca crea vehículos reales: creación del coche, streaming, limpieza de misión, caminos, cámara y espacio libre simulados y trazados.
- Los constructores corren sobre búferes 0xCD; en cada ejecución se restaura el pool de tareas.
- Las tareas creadas se liberan sin destructor (el de `CTaskComplexPlayHandSignalAnim` toca contadores de modelos reales).
- Simulados y trazados:
  - animación e IK;
  - tareas secundarias, agacharse, disparo y `StandStill`;
  - `HasCircledTarget`, `SetUpIK` y el escáner de eventos;
  - movimiento del jugador, sprint, arma y superficie;
  - ángulos de `CGeneral`, eventos interesantes y grupos de eventos.

Repetir: `python reversing/STasks6a/build_probe.py` y `python reversing/run.py --case stasks6a`. Evidencia: [validation-summary.json](validation-summary.json).
