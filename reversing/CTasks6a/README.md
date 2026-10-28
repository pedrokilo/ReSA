<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas A (lista de 6)

Validadas el 02/10/2026 con **14.784 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskAllocatorKillThreatsBasic` | Constructor, Destructor, `GetType`, `ProcessGroup`, `AllocateTasks`, `ComputeClosestPeds` |
| `CTaskComplexBeStill` | Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexDieInCar` | `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask`, `MakeAbortable`, `PreparePedVehicleForPedDeath` |
| `CTaskComplexDriveFireTruck` | Constructor, `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexFallToDeath` | Constructor, `CalcFall`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `MakeAbortable` |
| `CTaskComplexGoToBoatSteeringWheel` | Constructor, Destructor, `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexGoToPointAndStandStillTimed` | Constructor, `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `MakeAbortable`, `StopTimer` |

**Diferencias reales corregidas:**
- `CTaskAllocatorKillThreatsBasic`:
  - `AllocateTasks`: la duración de `CTaskSimpleLookAbout` sale de `CGeneral::GetRandomIntInRange(1000, 2000)` (0x407180); un solo `GetGroupId` y comparación con `&ms_groups[id]`;
  - el constructor no inicializa por valor el temporizador (bytes de relleno 0x12..0x13 sin escribir).
- `CTaskComplexDieInCar::CreateSubTask`: para `TASK_COMPLEX_LEAVE_CAR_AND_DIE` crea una `CTaskComplexLeaveCarAndDie` (0x62F1A0, vtable 0x86DD00); antes creaba una `CTaskComplexLeaveCar` con otro tipo. Se añade ese constructor a `CTaskComplexLeaveCarAndDie`.
- `CTaskComplexDriveFireTruck::CreateFirstSubTask` y `ControlSubTask` comprueban `bInVehicle` y el vehículo **del peatón**, no `m_pVehicle`, y en `ControlSubTask` antes de leer el tipo de la subtarea.
- `CTaskComplexFallToDeath`:
  - `CreateNextSubTask`: las dos pruebas de velocidad vertical estaban invertidas (aterrizaje fuerte con `|z| >= 0,1` o NaN; vuelve al aire si `|z| >= 0,01` o NaN);
  - `CalcFall`: las tres líneas de visión estaban invertidas, los intercambios usan `rand() & 3`, las sumas siguen el orden del EXE (`(0,375 + z) + dz`), la prueba de barandilla parte de `z - 0,5` y las constantes se leen de 0x8D2F00..0x8D2F0C.
- `CTaskComplexGoToBoatSteeringWheel`:
  - el constructor no escribe el punto objetivo;
  - `ComputeTargetPos` usa el tipo del model info, `Multiply3x3` con el orden del EXE y luego la posición;
  - `ControlSubTask` usa `(dz² + dy²) + dx²` y llama a `GoToPoint` sin la condición extra;
  - tipos desconocidos devuelven `nullptr` en lugar de `NOTSA_UNREACHABLE`.
- `CPedPlacement::FindZCoorForPed` (usada por `CTaskComplexGoToPointAndStandStillTimed::ControlSubTask`): primero la línea vertical exacta y después la desplazada 0,1, y el suelo vale solo si supera -99 estrictamente.

**Método:**
- 16 peatones con inteligencia propia, 3 vehículos con vtable falsa (`CanPedStepOutCar` trazada), evento falso con `Clone`, incendios, clump, asociaciones, edificio de contacto y tareas falsas.
- Los dos grupos falsos se alinean con `CPedGroups::ms_groups` (el EXE compara `&ms_groups[GetGroupId(g)]` con el grupo de la inteligencia).
- El model info del vehículo es falso y ocupa un hueco libre de `CModelInfo::ms_modelInfoPtrs`, que se restaura.
- Las tareas de la pila que recibe `SetEventResponseTask` se serializan en el momento (secuencia con sus subtareas del pool).
- Líneas de visión, línea vertical, incendio más cercano, fuerza, animaciones, eventos, grupos y RNG simulados con traza.
- Fuera de los fixtures: direcciones de caída fuera de 0..3 en `CreateFirstSubTask` (`NOTSA_UNREACHABLE`).

Repetir: `python reversing/CTasks6a/build_probe.py` y `python reversing/run.py --case ctasks6a`. Evidencia: [validation-summary.json](validation-summary.json).
