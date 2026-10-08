<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas B (lista de 8)

Validadas el 02/10/2026 con **12.736 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexGetOnBoatSeat` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexHitByGunResponse` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexInvestigateDisturbance` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexLeaveBoat` | Constructor, Destructor, `Clone`, `GetTaskType`, `CreateSubTask`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexLeaveCarAndFlee` | Constructor, Destructor, `Clone`, `GetTaskType`, `CreateSubTask`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexLeaveCarAndWander` | Constructor, Destructor, `Clone`, `GetTaskType`, `CreateSubTask`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |

**Diferencias reales corregidas:**
- `CTaskComplexHitByGunResponse`:
  - `MakeAbortable` (0x631E00) solo aborta la subtarea con prioridad urgente o inmediata; con otra devuelve `false` (heredaba la versión de `CTaskComplex`);
  - `CreateFirstSubTask` compara el dword completo sin signo y fuera de 0..3 devuelve `nullptr` (antes `NOTSA_UNREACHABLE` y truncado a `uint8`);
  - el constructor y `Clone` guardan/copian el dword completo del lado.
- `CTaskComplexInvestigateDisturbance`:
  - `CreateFirstSubTask` normaliza la dirección al punto (0x59C910) antes del producto con el vector adelante, suma `(z·z' + y·y') + x·x'`, usa el umbral 0.707 (0x863C1C, no `COS_45`) y un NaN también agacha;
  - la frase de alerta usa `rand() · (1/32767) < 0.2` (no `RandomBool(20)`) y el tipo del evento se consulta en cada comparación;
  - `CreateNextSubTask` crea `CTaskComplexGoToPointAndStandStill` con radio 0.5 (0x86FC84), no 1.5.
- `CTaskComplexLeaveBoat`: el hook `Destructor` apuntaba a 0x642250 (destructor con borrado del slot 0, `ret 4`); el destructor es 0x63BBD0.
- `CTaskComplexLeaveCarAndFlee`:
  - `CreateNextSubTask` usa el vehículo del peatón (`m_pVehicle`), no `m_veh`, para el punto de huida del barco (`pos + adelante · (radio · -2)`);
  - subtareas desconocidas devuelven `nullptr` en `CreateNextSubTask` y `CreateSubTask` (antes `NOTSA_UNREACHABLE`).

**Método:**
- 4 peatones con inteligencia propia, 2 vehículos con vtable falsa, handling falso y model info falso (con modelo de colisión) en un hueco libre de `CModelInfo::ms_modelInfoPtrs`, eventos falsos en la historia del gestor de eventos y tareas falsas con vtable trazada.
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); se comparan los fixtures, las tareas nuevas, las referencias, el retorno y la traza.
- `Say`, `SetTaskDuckSecondary`, `IKChainManager_c::LookAt`, `CTaskComplexWander::GetWanderTaskByPedType` y el RNG simulados con traza; `CTaskManager::SetTask` es real sobre la inteligencia del fixture.
- Fuera de los fixtures: `CTaskComplexLeaveCarAndFlee` con espera y sin vehículo (el EXE lee la posición de un puntero nulo; el DLL lo protege con un bugfix reversible).

Repetir: `python reversing/CTasks8b/build_probe.py` y `python reversing/run.py --case ctasks8b`. Evidencia: [validation-summary.json](validation-summary.json).
