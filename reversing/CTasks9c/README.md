<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas C (lista de 9)

Validadas el 02/10/2026 con **19.840 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexPassObject` | Constructor, Destructor, `AbortIK`, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexRoadRage` | Constructor, Destructor, `CreateSubTask`, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexScreamInCarThenLeave` | Constructor, Destructor, `CreateSubTask`, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexSitDownThenIdleThenStandUp` | Constructor, Destructor, `CreateSubTask`, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexWanderCop` | Constructor, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `ScanForStuff`, `LookForCarAlarms`, `LookForCriminals`, `LookForStolenCopCars`, `ShouldPursuePlayer` |

**Diferencias reales corregidas:**
- `CTaskComplexPassObject`:
  - el constructor no escribe el destino (+0x14), la entidad a pasar (+0x2C) ni el relleno del temporizador (sin `{}`);
  - `CreateFirstSubTask` arranca el temporizador de 1 s, registra la entidad solo si no es nula (sin assert), suma `(x² + y²) + z²` y crea `TurnToFaceEntityOrCoord` hacia el otro peatón (no hacia el punto medio);
  - `ControlSubTask` crea la `CTaskSimpleHoldEntity` con el constructor de puntero de 0x6913A0.
- `CTaskComplexRoadRage`:
  - `CreateSubTask` devuelve `nullptr` sin objetivo y con tipos desconocidos; `SeekEntity` persigue el vehículo **del objetivo** y, sin vehículo, `SeekEntity` y `DestroyCar` crean `KillPedOnFoot`;
  - `ControlSubTask` vuelve al coche cuando el objetivo está a **más** de 20 m (estaba invertido), con el orden de sumas de cada caso;
  - `CreateFirstSubTask` usa dos tiradas de 0x407180 (`< 50` y luego `<= 33` bate, `> 66` pala, si no palo de golf);
  - `CreateNextSubTask` devuelve `nullptr` con tipos desconocidos y comprueba `bInVehicle` y `m_pVehicle` del objetivo.
- `CTaskComplexScreamInCarThenLeave`:
  - el temporizador ya no se inicializa con `{}` (relleno 0x1E..0x1F);
  - huir esperando a que el coche frene es `(rand() & 0x3FF) <= 0x200` fuera de misión, decidido antes de reservar la tarea;
  - `ControlSubTask` mira primero el temporizador (lo reanuda aunque la subtarea no sea `CarDrive`);
  - tipos desconocidos devuelven `nullptr`.
- `CTaskComplexSitDownThenIdleThenStandUp`: `MakeAbortable` aborta la subtarea con prioridad `LEISURE`, `IsLoaded` se compara con 1 y los tipos desconocidos devuelven `nullptr`.
- `CTaskComplexWanderCop`:
  - `ScanForStuff` mira el tipo de la subtarea, no el de la propia tarea;
  - `ShouldPursuePlayer` compara el nivel de búsqueda con signo y vuelve a llamar a `FindPlayerWanted`;
  - `LookForCarAlarms` reconoce la alarma 0xFFFF (comparaba con -1) y repite `FindPlayerVehicle` como el EXE; `LookForStolenCopCars` repite `FindPlayerPed`;
  - `LookForCriminals`: NaN no pasa las pruebas de distancia y de producto, orden de sumas del EXE y `GetActiveTask` dos veces;
  - `CreateNextSubTask` llama a `GetTaskType` de la subtarea antes que al de la tarea guardada;
  - la persecución se crea con `new CTaskComplexPolicePursuit` (sin `()`, no pone a cero el relleno).

**Método:**
- 5 peatones con inteligencia propia (escáner de peatones y gestor de tareas), 2 vehículos con vtable falsa (huecos 57/58), tareas falsas trazadas, datos de jugador y `CWanted` falsos, y un `CBaseModelInfo`/`CColModel` falso en `ms_modelInfoPtrs[400]`.
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); las tareas nuevas se serializan y se liberan; se guardan y restauran el reloj, el puntero del modelo y los bytes `IsLoaded` de los bloques de animación usados.
- RNG, `Say`, IK de brazos, entidad sostenida, armas, `FindActiveTaskByType`, `SetTask`/`SetTaskSecondary`, `DropEntity`, eventos interesantes, misión, tareas del padre `CTaskComplexWander`, bloques de animación, `RequestModel`, `FindPlayer*`, nivel de búsqueda, `CanCopJoinPursuit`, línea de visión y `CEventGroup::Add` simulados con traza.
- Fuera de los fixtures: el destructor de `CTaskComplexPassObject` con entidad a pasar registrada (la DLL la limpia con `FIX_BUGS`), `LookForStolenCopCars` sin datos de jugador (el EXE falla) y bytes de `bool` distintos de 0/1 en los constructores.

Repetir: `python reversing/CTasks9c/build_probe.py` y `python reversing/run.py --case ctasks9c`. Evidencia: [validation-summary.json](validation-summary.json).
