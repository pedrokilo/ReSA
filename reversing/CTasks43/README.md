<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas (lista de 43)

Validadas el 02/10/2026 con **11.072 pares original/candidato dentro de GTA**, todos exactos: 40 funciones de 8 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexAttractorPartnerWait` | `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `GetTaskType` |
| `CTaskComplexCarDriveMission` | Constructor, Destructor, `Clone`, `GetTaskType`, `SetUpCar` |
| `CTaskComplexCarDriveWander` | Constructor, `Clone`, `CreateSubTaskCannotGetInCar`, `GetTaskType`, `SetUpCar` |
| `CTaskComplexDie` | Constructor, `CreateFirstSubTask`, `CreateNextSubTask`, `MakeAbortable`, `SayDeathSample` |
| `CTaskComplexEnterAnyCarAsDriver` | `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `GetTaskType` |
| `CTaskComplexGetUpAndStandStill` | Constructor, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexObserveTrafficLightsAndAchieveHeading` | `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `MakeAbortable` |
| `CTaskComplexSeekEntityAnyMeansXYOffset` | `Clone`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `GetTaskType` |

**Diferencias reales corregidas:**
- `CTaskComplexCarDrive` (base de las dos de conducción):
  - el constructor pone `m_bAsDriver` a 1 (`mov byte ptr [esi+1Ch],1`);
  - ninguno de los dos constructores (0x63C940, 0x63C9D0) escribe los tres bytes del estilo, la misión y la velocidad originales.
- `CTaskComplexCarDriveMission`: la misión es un dword en +0x28 (constructor 0x63CC30); `SetUpCar` convierte la velocidad con `_ftol2` y guarda el byte bajo.
- `CTaskComplexCarDriveWander::SetUpCar`: misma conversión de la velocidad; solo apaga el motor (`and cl,0EFh`) sin tocar `bIsLawEnforcer`.
- `CTaskComplexDie`:
  - guarda la dirección de caída como dword completo en +0x24 (constructor 0x630040), también en el hook;
  - `CreateNextSubTask` no devuelve nulo en aviones/helicópteros ahogándose, sigue por el camino general (0x630329/0x63032E saltan a 0x630371);
  - `CreateFirstSubTask` pasa el arma real a `CTaskComplexDieInCar`, no `WEAPON_UNARMED`.
- `CTaskComplexEnterAnyCarAsDriver::CreateFirstSubTask`: recorre las 16 entradas del escáner, solo acepta vehículos robables y suma la distancia como (z² + y²) + x².
- `CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateNextSubTask`: la subtarea se crea sin value-init (no escribe el relleno +0x16..+0x17).
- `CTaskComplexSeekEntityAnyMeans` / `XYOffset`:
  - el temporizador de escaneo no se value-inicializa (el ctor 0x695600 no escribe +0x22..+0x23);
  - `Clone` (0x696030) crea un `XYOffset` (vtable 0x870CF0) y registra la referencia a la entidad;
  - `CreateFirstSubTask` (0x6960B0) calcula la posición y arranca el temporizador antes de reservar la subtarea;
  - el temporizador se arranca en línea sin tocar `m_bStopped`, y `ControlSubTask` (0x696232) escribe el punto sin comprobar el tipo de la subtarea.

**Método:**
- Original con los hooks del lote desactivados frente a candidato con fixtures idénticos en búferes propios: peatones, vehículos, inteligencia (escáner de vehículos), par de efectos y tareas falsas (vtable trazada).
- Simulados y trazados: `CPed::Say`, `CPed::SetPedState`, `CPedIntelligence::ClearTasks`, `CLocalisation::Blood`, `CAEPedAudioEntity::AddAudioEvent`, `CCarCtrl::JoinCarWithRoadSystem`, `CCarEnterExit::IsVehicleStealable`, `CTaskComplexWander::GetWanderTaskByPedType`, `CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos` y el RNG.
- Pool de tareas: `m_LastFreeSlot` y estados restaurados, slots libres con 0xCD en cada ejecución.
- Se guardan y restauran el reloj, `CCarCtrl::bCarIsBeingCreated` y `CTaskSequences::ms_taskSequence`.
- Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias, el retorno y la traza.

Repetir: `python reversing/CTasks43/build_probe.py` y `python reversing/run.py --case ctasks43`. Evidencia: [validation-summary.json](validation-summary.json).
