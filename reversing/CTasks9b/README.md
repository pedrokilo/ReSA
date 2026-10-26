<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas B (lista de 9)

Validadas el 02/10/2026 con **13.696 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexFallAndGetUp` | Constructor, `Constructor2`, `CreateSubTask`, `IsFalling`, `SetDownTime`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexInvestigateDeadPed` | Constructor, Destructor, `Clone`, `GetTaskType`, `ComputeTargetPos`, `CreateSubTask`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexKillPedGroupOnFoot` | Constructor, Destructor, `Clone`, `GetTaskType`, `ComputeNearestLivingGroupPed`, `CreateSubTask`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexMedicTreatInjuredPed` | Constructor, `Clone`, `CreateSubTask`, `CreateDealWithNextAccidentTask`, `FindNearestAccident`, `CalcTargetPosWithOffset`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexOnFire` | Constructor, Destructor, `Clone`, `GetTaskType`, `ComputeFireDamage`, `CreateSubTask`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |

**Diferencias reales corregidas:**
- `CTaskComplexFallAndGetUp`:
  - `IsFalling` devuelve falso sin subtarea; `SetDownTime` consulta el tipo de la subtarea sin esa comprobación;
  - `MakeAbortable`: con prioridad distinta de inmediata y urgente aborta la subtarea (si no está cayendo, tras poner el tiempo a 0) y devuelve **siempre** falso.
- `CTaskComplexInvestigateDeadPed`:
  - el constructor no guardaba el peatón muerto y el temporizador ya no pone a cero su relleno;
  - `ComputeTargetPos`: dirección solo en XY (z = 0) con `CVector::Normalise`;
  - `CreateSubTask`: `GO_TO_POINT_AND_STAND_STILL` calcula antes el destino, `STAND_STILL` hace `LookAt` de 1000 ms y los tipos desconocidos devuelven `nullptr`;
  - `CreateFirstSubTask` usa `GetRandomIntInRange(5000, 7000)`; `ControlSubTask` suma `(z·z + y·y) + x·x` (NaN no actualiza) y consulta el tipo dos veces.
- `CTaskComplexKillPedGroupOnFoot`:
  - `ComputeNearestLivingGroupPed`: ignora al líder solo si no es el propio peatón, la búsqueda recursiva parte del **líder**, salta al propio peatón, la vida NaN no cuenta, distancia `(z·z + y·y) + x·x` estricta y sin candidato devuelve el peatón ignorado; no comprueba si el grupo está activo;
  - `CreateNextSubTask` suelta el objetivo antes de mirar la subtarea; `CreateFirstSubTask` sin objetivo crea `TASK_NONE`;
  - `CreateSubTask`: `GetRandomIntInRange(3000, 4000)` y `nullptr` con tipos desconocidos.
- `CTaskComplexMedicTreatInjuredPed`:
  - `FindNearestAccident` y `CreateDealWithNextAccidentTask` anulan el accidente antes de buscar;
  - `CalcTargetPosWithOffset`: muslo izquierdo y luego derecho, `(der + izq) · 0,5`; `CreateSubTask` usa `GetRandomIntInRange(0, 8)`;
  - `CreateNextSubTask`: `!(sqrt(y·y + x·x) > 1) && !(|z| > 2)` (NaN trata el accidente);
  - `ControlSubTask`: todo `CAR_DRIVE` exige ir en vehículo, el conductor no mira la tarea del compañero y `DRIVE_TO_POINT` sin accidente válido anula `m_pAccident`.
- `CTaskComplexOnFire`:
  - `ComputeFireDamage` multiplica por 0,001 (no divide entre 1000); `CreateSubTask` devuelve `nullptr` con tipos desconocidos;
  - `ControlSubTask`: sin fuego solo termina si la subtarea acepta abortar, crea la tarea de morir antes de `RegisterKill` y devuelve la subtarea leída al entrar.

**Método:**
- 8 peatones con inteligencia propia, 2 vehículos con vtable falsa, 3 accidentes, fuego, datos de jugador y nivel de búsqueda falsos, grupo falso para `GetPedsGroup` y tareas falsas con vtable trazada.
- Los miembros del grupo a matar se escriben en `CPedGroups::ms_groups`, que se guarda y se restaura en cada ejecución igual que `CTimer::m_snTimeInMilliseconds` y `CTimer::ms_fTimeStep`; pool de tareas restaurado (slots libres con 0xCD).
- `Say`, `LookAt`, `CInterestingEvents::Add`, `GetPedsGroup`, `AreInSameGroup`, `GetNearestFreeAccident(ExceptThisOne)`, `GetBonePosition`, `IsEntityBlockingTarget`, `ComputeDamageResponse`, `PedsOnFire`, `CFire::Extinguish`, `RegisterKill`, `FindPlayerPed` y el RNG simulados con traza.
- Fuera de los fixtures: `CreateDealWithNextAccidentTask` con accidente válido y una subtarea que no es `GO_TO_POINT_AND_STAND_STILL` (el EXE devuelve el peatón como tarea y pierde la creada; FIX_BUGS devuelve la tarea), conductor sin accidente al salir del coche en `CTaskComplexMedicTreatInjuredPed::CreateNextSubTask` (el EXE lee un puntero nulo) y jugadores sin datos de jugador en `ComputeNearestLivingGroupPed` (el DLL lo comprueba, el EXE no).

Repetir: `python reversing/CTasks9b/build_probe.py` y `python reversing/run.py --case ctasks9b`. Evidencia: [validation-summary.json](validation-summary.json).
