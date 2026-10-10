<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Tareas complejas A (lista de 9)

Validadas el 02/10/2026 con **14.592 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTaskComplexCopInCar` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexEnterCarAsDriverTimed` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `StopTimer`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexEnterCarAsPassengerTimed` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `StopTimer`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask` |
| `CTaskComplexEvasiveDiveAndGetUp` | Constructor, Destructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `CreateSubTask` |
| `CTaskComplexFacial` | Constructor, `Clone`, `GetTaskType`, `MakeAbortable`, `ControlSubTask`, `CreateFirstSubTask`, `CreateNextSubTask`, `SetRequest`, `StopAll` |

**Diferencias reales corregidas:**
- `CTaskComplexCopInCar`:
  - `Clone` usa el constructor (registra las tres referencias), no la copia implícita;
  - `CreateSubTask`: rumbo de patrulla con `GetRandomIntInRange(0, 8)` (0x407180), misión pedida antes que la velocidad, velocidad `MaxFlatVelocity * 54` en un solo producto, `CTaskComplexPolicePursuit` sin inicialización por valor y tipos desconocidos devuelven `nullptr`;
  - `CreateNextSubTask` sin la aserción de tipo de policía;
  - `ControlSubTask`: en persecución solo vuelve al coche el **conductor**, salud NaN, coche volcado (`IsUpsideDown`) y distancias NaN no siguen; en `CAR_DRIVE` el sospechoso alejándose pide la nueva misión sin esperar al temporizador y con el temporizador cumplido llama a `CreateNextSubTask`; temporizador con 0x407180 sin tocar `m_bStopped`; la subtarea se lee al entrar.
- `CTaskComplexEnterCarAsDriverTimed`: `MakeAbortable` mira la prioridad antes de `IsTemporaryEvent`; el temporizador ya no pone a cero el relleno 0x22-0x23.
- `CTaskComplexEnterCarAsPassengerTimed`: el temporizador ya no pone a cero el relleno 0x2A-0x2B.
- `CTaskComplexEvasiveDiveAndGetUp`:
  - el constructor registra la referencia del vehículo y el destructor la limpia;
  - `CreateNextSubTask` calcula `LimitRadianAngle(actual) - HALF_PI` (double exacto de 0x86F418) y escribe 0x558 y 0x55C;
  - rumbo con `GetRadianAngleBetweenPoints(x, y, 0, 0)` y tipos o subtareas desconocidos devuelven `nullptr`.
- `CTaskComplexFacial`:
  - constructor propio: no escribe las duraciones ni el byte 0xF y decide si masca con `(int)(rand() * 1/32768 * 100) > 70`; `Clone` ya no pone a cero la tarea;
  - `ControlSubTask` reinicia el temporizador de la tarea facial sin comprobar el signo ni tocar `m_bStopped` (`CTaskSimpleFacial::StartTimer`).

**Método:**
- 4 peatones con inteligencia y clump propios (tipo 2), 2 vehículos con vtable falsa y handling propio, evento falso con `GetEventType` trazado, `CWanted` falso, asociación de animación falsa y tareas falsas con vtable trazada (`MakeAbortable` responde según una máscara por llamada).
- Pool de tareas restaurado en cada ejecución (slots libres con 0xCD); `CTimer::m_snTimeInMilliseconds` y `CTimer::ms_fTimeStep` se guardan y se restauran.
- RNG, `RpAnimBlendClumpGetAssociation`, `IsTemporaryEvent`, `CVehicle::IsDriver`/`IsUpsideDown`/`IsOnItsSide`/`ChangeLawEnforcerState`, `CCarAI::FindPoliceCarMissionForWantedLevel`/`FindPoliceCarSpeedForWantedLevel`, `FindPlayerWanted`, `CWanted::CanCopJoinPursuit`, `SetPedDecisionMakerType` y `ComputePassengerIndexFromCarDoor` simulados con traza.
- Distancias exactas (4 y 10), NaN en posiciones, salud y velocidad, y temporizadores justo cumplidos.
- Fuera de los fixtures: vehículo o sospechoso nulos donde el EXE los desreferencia en `CTaskComplexCopInCar` (crear `CAR_DRIVE_MISSION`, salir del coche).

Repetir: `python reversing/CTasks9a/build_probe.py` y `python reversing/run.py --case ctasks9a`. Evidencia: [validation-summary.json](validation-summary.json).
