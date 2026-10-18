<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas y grupos (lista de 70)

Validadas el 02/10/2026 con **11.296 pares original/candidato dentro de GTA**, todos exactos: 28 funciones con `validated=1`. `AllocateDefaultTasks` de `Random` y `SitInLeaderCar` ya estaban validadas.

| Clase | Funciones |
|---|---|
| `CPedGroupDefaultTaskAllocatorChat` | `AllocateDefaultTasks`, `GetType` |
| `CPedGroupDefaultTaskAllocatorFollowAnyMeans` | `AllocateDefaultTasks`, `GetType` |
| `CPedGroupDefaultTaskAllocatorFollowLimited` | `AllocateDefaultTasks`, `GetType` |
| `CPedGroupDefaultTaskAllocatorStandStill` | `AllocateDefaultTasks`, `GetType` |
| `CPedGroupDefaultTaskAllocatorRandom` | `GetType` |
| `CPedGroupDefaultTaskAllocatorSitInLeaderCar` | `GetType` |
| `CPedGroups` | `Process`, `RemoveGroup` |
| `CTaskAllocatorKillThreatsBasicRandomGroup` | `AllocateTasks`, `GetType` |
| `CTaskComplexDragPedFromCar` | `ControlSubTask`, `CreateFirstSubTask` |
| `CTaskSequences` | `Init`, `CleanUpForShutdown` |
| `CTaskSimpleFinishBrain` | `Clone`, `GetTaskType` |
| `CTaskSimpleLeaveGroup` | Constructor, Destructor |
| `CTaskSimplePlayHandSignalAnim` | `ProcessPed`, `StartAnim` |
| `CTaskSimpleScratchHead` | `Clone`, `IsInterruptable` |
| `CTimeModelInfo` | `GetModelType`, `GetTimeInfo` |

**Diferencias reales corregidas:**
- `Chat::AllocateDefaultTasks` reescrita: secuencia por miembro (ir al punto y orientarse) y rondas en las que un miembro al azar charla y los demás esperan, con el RNG del original.
- `FollowAnyMeans` y `FollowLimited`:
  - los seguidores (pares 0-6) siguen al líder;
  - el líder (par 7) deambula en grupo con `GetRandomIntInRange(0, 8)`.
- `CPedGroups::Process`:
  - el tipo de decisión solo se pone en grupos activos que no son de misión;
  - el contador de muertes se reinicia como el original.
- `CPedGroups::RemoveGroup` accede al grupo sin la aserción de grupo activo.
- `CTaskAllocatorKillThreatsBasicRandomGroup`:
  - `AllocateTasks` y `GetType` son virtuales y estaban instaladas como funciones normales (el hook recursaba por el thunk);
  - la pausa usa `GetRandomIntInRange(1000, 2000)`.
- `CTaskSimplePlayHandSignalAnim::StartAnim`: animación aleatoria como el EXE; la mano derecha usa `ANIM_GROUP_RHAND` (la DLL usaba siempre la izquierda).
- Fuera de la lista, necesarios para el banco:
  - el destructor de `CTaskSimpleChat` (un `assert` de "no revertido") es el de la clase base;
  - el constructor de `CTaskComplexFollowLeaderAnyMeans` tenía un `DebugBreak()` y registraba la referencia sobre el parámetro en vez del miembro.

**Diferencia no reproducible (enmascarada):** el EXE pasa a `FollowLeaderAnyMeans`/`InFormation` un `CVector` cuya z queda sin escribir en la pila; la DLL pone 0.

**Método:**
- Grupos de peatones falsos.
- `ms_groups`, secuencias y contadores globales rellenados por ejecución y restaurados.
- Slot libre del pool de tareas restaurado.
- Se simulan y se trazan las dependencias (RNG, inteligencia de grupo, eventos interesantes, animaciones, objetos de mano, `CWorld::Add`).

Repetir: `python reversing/Tasks70/build_probe.py` y `python reversing/run.py --case tasks70`. Evidencia: [validation-summary.json](validation-summary.json).
