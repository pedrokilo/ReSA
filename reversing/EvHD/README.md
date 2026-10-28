<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Manejador de eventos, respuestas (grupo D)

Validadas el 07/10/2026 con **9.728 pares original/candidato dentro de GTA**, todos exactos: 13 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputeSeenCopResponse`, `ComputeSeenPanickedPedResponse`, `ComputeSexyPedResponse`, `ComputeSexyVehicleResponse`, `ComputeShotFiredResponse`, `ComputeShotFiredWhizzedByResponse`, `ComputeSignalAtPedResponse`, `ComputeSpecialResponse`, `ComputeVehicleDiedResponse`, `ComputeVehicleOnFireResponse`, `ComputeVehiclePotentialPassiveCollisionResponse`, `ComputeVehicleToStealResponse`, `ComputeWaterCannonResponse` |

**Diferencias reales corregidas:**
- Todas las funciones (salvo `ComputeSignalAtPedResponse`, que ya coincidía): solo escriben `m_EventResponseTask` en las rutas en que lo hace el EXE. Antes se asignaba siempre el resultado de una lambda, así que "sin ped", "sin vehículo" o "tarea no contemplada" dejaban `nullptr` (o abortaban con `NOTSA_UNREACHABLE`) en vez de dejar la respuesta anterior intacta. Se quitan las lambdas y los `NOTSA_UNREACHABLE`.
- `ComputeSeenCopResponse`: el EXE compara con `TASK_NONE` (200); la DLL usaba `TASK_FINISHED` (1302), así que un evento con `TASK_NONE` abortaba.
- `ComputeSeenPanickedPedResponse`:
  - el historial de eventos que se consulta es el del peatón origen del evento (inteligencia +0x34), no el del propio manejador;
  - `TASK_COMPLEX_SMART_FLEE_ENTITY` crea `CTaskComplexSmartFleeEntity` con `scream = true`.
- `ComputeShotFiredResponse`:
  - `TASK_SIMPLE_DUCK`: si ya hay una tarea de agacharse **no** se escribe nada y si no la hay se crea (estaba invertido);
  - `TASK_COMPLEX_KILL_PED_ON_FOOT` y `TASK_COMPLEX_KILL_CRIMINAL`: mismo tipo de peatón o "no es peatón" no escriben; el jugador como autor escribe `nullptr` solo en la ruta de criminal; `TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH` pasa la entidad en bruto sin comprobar que sea peatón;
  - `TASK_COMPLEX_KILL_CRIMINAL`: la huida (`CTaskComplexSmartFleeEntity`) fija `m_moveState = PEDMOVE_WALK` (0x4BCACB);
  - `TASK_COMPLEX_TURN_TO_FACE_ENTITY` pasa también el 0,2 de `maxHeading`.
- `ComputeShotFiredWhizzedByResponse`:
  - un duck con contador < 0 sigue por la ruta del duck de la inteligencia y acaba creando la tarea nueva, no sale con `nullptr`;
  - la duración sale de `GetRandomIntInRange(3000, 5000)` (0x407180), no de la plantilla `uint16` (que usa 1/32767 sobre [min, max-1]).
- `ComputeSpecialResponse`: sin vehículo, `LEAVE_CAR`, `LEAVE_CAR_AND_WANDER` y `LEAVE_CAR_AND_FLEE` no escriben; `TASK_COMPLEX_KILL_CRIMINAL` escribe `nullptr`; las tres tareas de atractor se reservan sin `{}` (sin poner a cero).
- `ComputeVehicleDiedResponse`: sin vehículo no escribe.
- `ComputeVehicleOnFireResponse`:
  - `CTaskComplexDie` se crea con arma `WEAPON_UNARMED` (0), no `WEAPON_EXPLOSION`;
  - `CTaskComplexLeaveAnyCar` se crea con `(0, false, true)` en `LEAVE_CAR`, y con `forceGetOut = true` también en las secuencias de huida y deambular;
  - `SMART_FLEE_ENTITY` huye del vehículo del evento (no del del peatón);
  - la rama de vehículo destruido o sin salud exige que el vehículo del evento sea el mismo en el que va el peatón;
  - la secuencia se reserva antes que sus tareas.
- `ComputeVehiclePotentialPassiveCollisionResponse`:
  - la condición de "ya va en un vehículo" estaba invertida;
  - el estado de movimiento que se compara con `PEDMOVE_STILL` es el del evento, no el de la tarea `GoTo`;
  - `IsPedGoingForCarDoor` se llama antes de reservar la tarea.
- `ComputeVehicleToStealResponse`: la condición de "ya va en un vehículo" estaba invertida.
- `ComputeWaterCannonResponse`: orden del EXE (`Say`, lado del golpe, tarea, fuerza y velocidad) y umbral `> 0,2` en vez de `>= 0,2`.
- `ComputeSexyVehicleResponse`: la tarea se reserva antes del `LookAt`.

**Método:**
- `CEventHandler` falso (`m_Ped` en +0, historial en +8/+0xC y un centinela en `m_EventResponseTask`, para ver las rutas que no escriben), tres peatones, dos vehículos, un objeto, tres inteligencias con historial, tareas y eventos falsos con vtable propia (trampas en los huecos no usados) y eventos de prueba sobre la vtable del EXE (`SeenPanickedPed`, `GunShotWhizzedBy`) o falsa.
- Se compara el manejador entero, las tareas nuevas del pool de tareas (tipo + 0x70 bytes, slots libres a 0xCD y `m_LastFreeSlot` restaurado), las rutas nuevas del pool de `CPointRoute`, las entidades falsas (sin el nodo de referencias), las listas de referencias, el reloj, el paso de tiempo, el RNG y la traza; después se borra la tarea de respuesta (con las de su secuencia) y los pools deben quedar como estaban.
- Se simulan y se trazan `FindPlayerPed`, `IsKillTaskAppropriate` (cdecl estática en ambos), `ComputeKnockOffBikeResponse`, `CInterestingEvents::Add`, `IKChainManager_c::LookAt`, `CPed::Say`, `CTaskSimpleDuck::RestartTask`, `CPedGroups::GetPedsGroup`, `IsPedGoingForCarDoor`, `ComputePedHitSide`, `ApplyMoveForce` y el RNG (`rand` 0x821B1E + `GetRandomNumber`); `GetTaskDuck` se simula sin traza (lector puro).

Repetir: `python reversing/EvHD/build_probe.py` y `python reversing/run.py --case evhd`. Evidencia: [validation-summary.json](validation-summary.json).
