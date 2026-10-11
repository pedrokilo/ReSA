<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas C y golpes (lista de 8)

Validadas el 02/10/2026 con **16.896 pares original/candidato dentro de GTA**, todos exactos: 48 funciones de 6 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexMoveBackAndJump` | Constructor, Destructor, `CreateSubTask`, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexObserveTrafficLights` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexReactToGunAimedAt` | Constructor, Destructor, `CreateSubTask`, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexWalkRoundFire` | Constructor, Destructor, `ComputeDetourTarget`, `Clone`, `GetTaskType`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskSimpleBeHit` | Constructor, Destructor, `FinishAnimBeHitCB`, `StartAnim`, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskSimpleBikeJacked` | Constructor, Destructor, `FinishAnimBikeHitCB`, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed`, `SetPedPosition` |

**Diferencias reales corregidas:**
- `CTaskComplexMoveBackAndJump`:
  - `CreateSubTask` (0x655390) es `__thiscall` con `ret 4`: estaba enganchada como función global `__cdecl` (pila descuadrada al mezclar EXE y DLL);
  - un tipo desconocido devuelve `nullptr` en `CreateSubTask` y en `CreateNextSubTask` (la DLL abortaba).
- `CTaskComplexObserveTrafficLights`:
  - la pausa de `CreateNextSubTask` sale de 0x407180 (`[3000, 5000)` con 1/32768) y se calcula antes de reservar la tarea;
  - `Clone` ya no pone a cero el relleno 0x16..0x17 (`new T` sin `()`).
- `CTaskComplexReactToGunAimedAt`:
  - el rumbo de `AchieveHeading` es `LimitRadianAngle(GetRadianAngleBetweenPoints(dx, dy, 0, 0))` y también se guarda en `m_fAimingRotation`;
  - `HandsUp` y `Pause` usan la fórmula de 0x407180, y `Pause` exige además `m_pVehicle`;
  - tras `Cower` dice `CTX_GLOBAL_GUN_RUN` (0x73) antes de huir, y el temporizador del paseo arranca después de crear la tarea;
  - `ControlSubTask` aborta el paseo con prioridad `URGENT`, no `LEISURE`;
  - tipos desconocidos devuelven `nullptr` y el temporizador ya no se inicializa con `{}` (relleno 0x1A..0x1B).
- `CTaskComplexWalkRoundFire`:
  - el constructor no escribe `m_initialPedPos` (+0x2C);
  - `ComputeDetourTarget` pasa una `CColSphere` hecha con `Set(radio, centro, 0, 0, 0xFF)` y la ruta se calcula antes de reservar la subtarea.
- `CTaskSimpleBeHit`:
  - la animación por defecto compara el daño (+0x1C) con 5, no la dirección;
  - `ProcessPed` comprueba `IsPlayer` y el tipo `GANG2` del peatón golpeado (no del atacante) y calcula el rumbo con `fpatan`;
  - `MakeAbortable` con prioridades fuera de 0..2 sigue el camino de `LEISURE`.
- `CTaskSimpleBikeJacked`:
  - el constructor guarda y registra el secuestrador (+0x20), que quedaba nulo;
  - `ProcessPed`: `SetRadioStation` y `PlayerAboutToExitVehicleAsDriver` son comprobaciones independientes (la segunda con `ped->m_pVehicle`), las animaciones se buscan en el clump del secuestrador (RHS, LHS, BIKE_FRONT) esperando también con NaN, `rand() & 0x3FF > 512` dice primero `SayJacked`, y la segunda animación se mezcla con delta 100.

**Método:**
- 3 peatones con inteligencia, clump y estadísticas propias, 2 vehículos con vtable falsa (hueco 47 `GetRideAnimData`), tareas falsas (la de `CTaskSimpleGoToPoint` con destino, radio y flags), evento falso, asociaciones de animación y tarea de pelea falsas.
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD) y reloj fijado por caso; las `CTaskUtilityLineUpPedWithCar` del heap se serializan y se liberan.
- `Say`, `Disable/EnablePedSpeech`, `SetRadioStation`, `SetPedPositionInCar`, `SayJacked`, `SayJacking`, `IsDriver`, los dos eventos de audio, `BlendAnimation`, `RpAnimBlendClumpGetAssociation`, `SetFinishCallback`, `SetCurrentTime`, `GetTaskFighting`, `CEventGroup::Add`, `LightForPeds`, `ComputeRouteRoundSphere`, las tres funciones de ángulos de `CGeneral` y el RNG simulados con traza.
- Fuera de los fixtures: combos de pelea mayores que 15 (la DLL indexa `m_aComboData` con `std::array`).

Repetir: `python reversing/CTasks8c/build_probe.py` y `python reversing/run.py --case ctasks8c`. Evidencia: [validation-summary.json](validation-summary.json).
