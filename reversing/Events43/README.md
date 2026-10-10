<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Eventos, asignadores y datos de peatón/vehículo (lista de 43)

Validadas el 02/10/2026 con **13.824 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 9 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventGunShot` | Constructor, `AffectsPed`, `CloneEditable`, `IsCriminalEvent`, `TakesPriorityOver` |
| `CEventVehicleDamage` | Constructor, `AffectsPed`, `GetSourceEntity`, `IsCriminalEvent`, `ReportCriminalEvent` |
| `CTaskAllocatorAttack` | Constructor, Destructor, `GetType`, `AllocateTasks`, `ProcessGroup` |
| `CTaskAllocatorKillThreatsDriveby` | Constructor, Destructor, `GetType`, `AllocateTasks`, `ProcessGroup` |
| `CPedScriptedTaskRecord` | `GetRecordAssociatedWithEvent`, `GetStatus` (las dos), `GetVacantSlot`, `Process` |
| `CPlayerPedData` | Constructor, Destructor, `AllocateData`, `DeAllocateData`, `SetInitialState` |
| `CUpsideDownCarCheck` | `AddCarToCheck`, `RemoveCarFromCheck`, `HasCarBeenUpsideDownForAWhile`, `IsCarUpsideDown`, `UpdateTimers` |
| `CVehicleAnimGroup` | `InitAnimGroup`, `CopyAnimGroup`, `GetGroup`, `ComputeCriticalBlendTime`, `ComputeAnimDoorOffsets` |
| `CVehicleAnimGroupData` | `GetAnimDoorOffset`, `GetGroupForAnim`, `GetInOutTimings`, `UsesKartDrivingAnims`, `UsesTruckDrivingAnims` |

**Diferencias reales corregidas:**
- `CEventGunShot::AffectsPed`:
  - `FindPlayerWanted` solo se llama para policías y en cada uso; el nivel se compara con signo;
  - distancia como (z² + y²) + x² y producto escalar como (y·fy + z·fz) + x·fx;
  - con NaN: el rango de disturbios toma la rama "<= 0", la distancia sigue adelante y el producto cuenta como positivo.
- `CTaskAllocatorAttack`: destructor (0x69C2D0) que limpia la referencia del objetivo antes que la del originador; `ProcessGroup` llama directamente a `AllocateTasks` (0x69C3F0), no por la vtable.
- `CTaskAllocatorKillThreatsDriveby`:
  - layout incorrecto: heredaba de `CTaskAllocatorKillThreatsBasic` y volvía a declarar la amenaza y el temporizador (medía 0x24 en vez de 0x14 y `AllocateTasks` leía la amenaza de +0x14);
  - constructor (0x69CAF0) y `ProcessGroup` (0x69CBC0) propios, este último llamando a `AllocateTasks` por la vtable;
  - `AllocateTasks`:
    - toma el grupo directamente del id (`ms_groups + id * 0x2D4`, sin comprobar si está activo);
    - compara `m_pDriver` y recorre los huecos de miembros saltando los nulos;
    - tirada con `CGeneral::GetRandomIntInRange` (0x407180), [1000, 2000).
- `CTaskSimpleGangDriveBy`: el constructor pone a cero `m_vecCoords` (0x621836) antes de copiar la posición.
- `CPedScriptedTaskRecord::Process`: `HasTaskPrimary` en línea (0x6086BF) solo compara la primera tarea no nula de los huecos 0..2 y los huecos 3 y 4.
- `CPlayerPedData`:
  - `AllocateData` tiene el constructor de `CWanted` en línea: solo escribe la vtable 0x85F368 en +0x21C y anula +0x22C (no pone a cero los 0x29C bytes);
  - `DeAllocateData` solo llama a `operator delete` con punteros no nulos;
  - `SetInitialState` aplica `(flags & 0xFFFFF7B0) | 0x1010` y después `& ~0x80`.
- `CUpsideDownCarCheck`:
  - `IsCarUpsideDown` mira primero la orientación y solo entonces devuelve `CanPedStepOutCar(false)` (antes se llamaba primero y se invertía);
  - `UpdateTimers`:
    - paso en ms truncado con `_ftol2` una vez antes del bucle;
    - `GetAtRef` en línea sin comprobar límites ni saltarse el manejador 0;
    - un coche desaparecido deja manejador -1 y tiempo 0;
  - `HasCarBeenUpsideDownForAWhile` usa `>` estricto (`ja`); `AddCarToCheck` solo ocupa el primer hueco libre.
- `CVehicleAnimGroup`:
  - `GetGroup`:
    - un id fuera de la lista usa el propio id como bit;
    - máscara `_ftol2(pow(2, bit))` (0 fuera de [0, 31]);
    - grupo extendido con signo;
  - `ComputeCriticalBlendTime` compara el desplazamiento con 0 como en el x87 (solo es 0 si las tres componentes son ±0);
  - `ComputeAnimDoorOffsets` usa el índice de puerta como animación por defecto y el número de fotogramas con signo.

**Método:**
- Original con los hooks del lote desactivados frente a candidato con fixtures idénticos.
- Falsos: peatones, vehículos (vtable con `CanPedStepOutCar` trazado), inteligencias, eventos y tareas (vtable propia), un `CPedGroup` y su inteligencia, un pool de vehículos (5 huecos de 0xA18 con byte de guarda), un `CWanted` y animaciones (asociación, jerarquía, secuencia y fotogramas).
- Simulados y trazados:
  - `FindPlayerPed`, `FindPlayerWanted`, `CWanted::IsInPursuit`/`CanCopJoinPursuit`/`RegisterCrime`, `CPedGroups::AreInSameGroup`/`GetPedsGroup`, `CWorld::GetIsLineOfSightClear`;
  - `CPedGroupIntelligence::FlushTasks`/`SetEventResponseTask` (la tarea se serializa en el momento)/`GetTaskScriptCommand`, `CTaskManager::FindTaskByType`, `ComputeClosestPeds`, `CTaskAllocatorKillOnFoot::AllocateTasks`;
  - `CInterestingEvents::Add`, `CEventGroup::HasEvent`, `CScriptedBrainTaskStore::GetTask`, `CStats::GetFatAndMuscleModifier`, `CAnimManager::GetAnimAssociation`/`UncompressAnimation`, `operator new`/`delete` (solo durante la llamada) y el RNG.
- Se guardan y restauran en cada ejecución y al final: los pools de eventos y tareas (huecos libres con 0xCD), `ms_scriptedTasks`, `CPedGroups::ms_groups`, `m_vehicleAnimGroups`, el puntero del pool de vehículos, `ms_fGunShotSenseRangeForRiot2`, el reloj y `ms_fTimeStep`.

Repetir: `python reversing/Events43/build_probe.py` y `python reversing/run.py --case events43`. Evidencia: [validation-summary.json](validation-summary.json).
