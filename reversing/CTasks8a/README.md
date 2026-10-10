<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas A (lista de 8)

Validadas el 02/10/2026 con **14.144 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexEnterBoatAsDriver` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexExtinguishFires` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `GetNearestFire` |
| `CTaskComplexFleeAnyMeans` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexFleeEntity` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexFleeShooting` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexFollowLeaderAnyMeans` | Constructor, Destructor, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |

**Diferencias reales corregidas:**
- `CTaskComplexEnterBoatAsDriver`: `CreateSubTask` y `CreateNextSubTask` devuelven `nullptr` con tipos desconocidos (no `NOTSA_UNREACHABLE`).
- `CTaskComplexExtinguishFires`:
  - `GetNearestFire`: suma `(z·z + x·x) + y·y` y una distancia NaN acepta el fuego (`test ah,5`);
  - `ControlSubTask` guarda el nuevo fuego en `m_fireToExt` antes de `GoToPoint`, consulta el tipo de la subtarea dos veces y solo busca el fuego en las ramas que lo usan;
  - `CreateNextSubTask` solo busca el fuego en las ramas conocidas, índice de arma con signo y `nullptr` con subtareas desconocidas.
- `CTaskComplexFleeAnyMeans`:
  - constructor y destructor registran y limpian otra vez la entidad (como el EXE);
  - `ControlSubTask`: el vehículo más cercano se pide antes de mirar la subtarea, distancia `(z·z + y·y) + x·x` estricta, `CVector::Normalise` y producto `(z + y) + x` (NaN no sigue), comprobación de `CCarEnterExit::IsVehicleStealable` que faltaba, respuesta `TASK_COMPLEX_LEAVE_CAR_AND_FLEE` en el evento a los ocupantes y bucle hasta `m_nMaxPassengers`;
  - `CreateSubTask` usa `GetRandomIntInRange(0, 8)` (0x407180) para el rumbo de huida; `m_stealVeh` se anula después de crear la tarea; tipos desconocidos devuelven `nullptr`.
- `CTaskComplexFleeEntity`:
  - el constructor no escribe `m_pos` ni el relleno del temporizador;
  - `CreateFirstSubTask` mira el **tipo** de entidad (peatón) para el hueso, no el estado, y arranca el temporizador sin comprobar el periodo; `CreateSubTask` ya no toca temporizador ni posición;
  - `ControlSubTask`: distancia `(z·z + y·y) + x·x` estricta (NaN no actualiza) y el temporizador solo se reinicia al actualizar; evento `CEventSeenPanickedPed` local con `Add(&event)`.
- `CTaskComplexFleeShooting`:
  - el constructor no escribe el relleno del temporizador de arma y registra otra vez la entidad (el destructor la limpia otra vez);
  - `ControlSubTask` pone `PEDMOVE_RUN` cuando hay tarea de ataque, arranca los temporizadores sin comprobar el periodo y usa `GetWeaponSkill()` con índice de arma con signo.
- `CTaskComplexFollowLeaderAnyMeans`:
  - `CreateNextSubTask` y `ControlSubTask` miran si el **líder** (`m_ped`) va en vehículo, no el peatón;
  - `CreateSubTask` escribe 7 en +0x50 de la tarea de búsqueda (como el EXE, dentro del slot del pool); tipos desconocidos devuelven `nullptr`.

**Método:**
- 4 peatones con inteligencia propia, 2 vehículos con vtable falsa (`CanPedStepOutCar` trazado), 2 fuegos, grupo, info de arma, model info y modelo de colisión falsos (hueco libre de `ms_modelInfoPtrs`) y tareas falsas con vtable trazada.
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); `CTimer::m_snTimeInMilliseconds` y el hueco de model info se guardan y se restauran.
- `FindNearestFire`, `CFire::Extinguish`, `IsVehicleStealable`, `GetWeaponSkill`, `GetWeaponInfo`, `ControlGun`, `Say`, `CInterestingEvents::Add`, `IsLooking`/`LookAt`, los dos `SetFleePosition`, `CEventGroup::Add` y el RNG simulados con traza.
- Fuera de los fixtures: `m_stealVeh` en el destructor de `CTaskComplexFleeAnyMeans` (FIX_BUGS limpia su referencia), `CTaskComplexFleeEntity::CreateFirstSubTask` sin entidad (el DLL devuelve `nullptr`, el EXE la desreferencia), fuego cercano y `m_fireToExt` nulos a la vez en `CTaskComplexExtinguishFires::CreateNextSubTask` y líder nulo en `CTaskComplexFollowLeaderAnyMeans::CreateNextSubTask` (el EXE lee un puntero nulo).

Repetir: `python reversing/CTasks8a/build_probe.py` y `python reversing/run.py --case ctasks8a`. Evidencia: [validation-summary.json](validation-summary.json).
