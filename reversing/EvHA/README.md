<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventHandler: respuestas a daño, muerte y vehículos, y gestión de eventos (grupo A)

Validadas el 07/10/2026 con **17.984 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputeDamageResponse`, `ComputePersonalityResponseToDamage`, `ComputeDeathResponse`, `ComputeDeadPedResponse`, `ComputeVehicleDamageResponse`, `ComputeGotKnockedOverByCarResponse`, `ComputeVehicleCollisionResponse`, `ComputeVehiclePotentialCollisionResponse`, `ComputeEventResponseTask` (la de evento y la estática de peatón), `HandleEvents`, `Flush`, `FlushImmediately`, `SetEventResponseTask`, `RegisterKill`, `Constructor`, `Destructor`, `IsKillTaskAppropriate`, `IsTemporaryEvent`, `GetCurrentEventType` |

**Diferencias reales corregidas:**
- Regla general: el EXE solo escribe `m_EventResponseTask` en las ramas que crean o borran la respuesta; si falla una condición o la tarea del evento no se contempla, deja el valor anterior. La DLL escribía `nullptr` o abortaba con `NOTSA_UNREACHABLE`; ahora cada rama escribe exactamente lo que escribe el EXE. La distancia segura de las huidas es 60 (`0xC18CEC`, igual que `0xC18CE4`), no 100000.
- `ComputeDamageResponse` (rehecha desde el EXE; sin `FIX_BUGS`):
  - un ped que no está vivo (`IsAlive`, 0x4C018D) no responde;
  - sin `m_bAddToEventGroup` solo se llama a la respuesta de personalidad si el origen es un peatón (y no hay `REACT_TO_ATTACK`, ni es el jugador, ni es el mismo daño que el evento actual);
  - en moto o quad: con caída o muerte `ComputeKnockOffBikeResponse`; si no, la respuesta de personalidad solo con origen peatón (antes se afirmaba que lo fuera);
  - muerte:
    - la animación se calcula con la tarea simple activa: `TASK_SIMPLE_FALL` (sangre si el daño es `> 20`, dos tiradas por gota solo en X e Y, animación de la asociación si `mezcla > 0,5`, `delta >= 0` y `tiempo < total`; si la caída terminó, `FLOOR_HIT_F` o `FLOOR_HIT`) y `TASK_SIMPLE_GET_UP` (progreso `< 0,5` sin mirar `m_bHasPedGotUp`);
    - la tarea de muerte se crea ANTES de quitar la respuesta física y de registrar la muerte, y la caída mortal exige que el origen sea el jugador o un ped de misión;
  - disparo: se calcula `ComputeDamageAnim` (antes `ComputeAnim`), el tiempo de `FallAndGetUp` es `1000 / (cadencia * 0,025f)` truncado con `_ftol2`, `animAdded` se marca en las dos ramas de la animación parcial (que usa los ids 28 a 31) y la asfixia no comprueba el origen (el EXE se cae con un origen que no es peatón);
  - daño que no es de arma: el tiempo de `FallAndGetUp` depende de `m_pPlayerData` y no de `IsPlayer()`.
- `ComputePersonalityResponseToDamage` (rehecha): la tabla de tareas del EXE es otra: `CAR_DRIVE_MISSION_FLEE_SCENE` (conductor: `FleeScene`; si no, `FleeAnyMeans` si lo buscan y el origen es policía, o `SmartFleeEntity`), `CAR_DRIVE_MISSION_KILL_PED` (en coche: `CarDriveMissionKillPed`; si no, `KillPedOnFoot`), `FLEE_ENTITY` crea `FleeEntity` (no `SmartFleeEntity`), y las tres huidas (`FLEE_ENTITY`, `SMART_FLEE_ENTITY`, `FLEE_ANY_MEANS`) terminan con `Say(RUN_FROM_FIGHT)`; con `TASK_SIMPLE_DUCK` y una tarea de agacharse ya activa no se escribe nada y la duración es `GetRandomIntInRange(2000, 5000)`.
- `ComputeDeadPedResponse`: sin peatón muerto no se escribe nada; la distancia 2D es `y² + x²` y se compara con `0,5625001f` (0x85B9F4, con NaN no se hace nada); `SMART_FLEE_ENTITY` usa distancia 60.
- `ComputeGotKnockedOverByCarResponse`: el EXE compara `SMART_FLEE_ENTITY` (911), no `FLEE_ENTITY`; sin conductor huye del propio vehículo; `SmartFleeEntity(conductor, false, 60)`; `KILL_CRIMINAL` con conductor jugador borra la respuesta.
- `ComputeVehicleDamageResponse`:
  - `LEAVE_CAR_AND_FLEE` huye desde la posición del VEHÍCULO (antes la del ped);
  - `SEQUENCE` sin secuencia activa no escribe nada; el atacante que no es peatón ni vehículo no escribe;
  - `KILL_PED_ON_FOOT` sin matar crea `FleeScene` (conductor) o `SmartFleeEntity(vehículo, false, 60)`.
- `ComputeVehicleCollisionResponse` (rehecha):
  - la velocidad es `sqrt((x² + y²) + z²)` y los umbrales son 0,05 y 0,001 (antes se comparaba el cuadrado con 0,5² y 0,001²);
  - jugador de pie: producto escalar `(z + y) + x`, daño `max(0, d * masa + daño)` y límite 20; el resto de peatones usa el daño del evento;
  - el salto compara la rotación actual con la de apuntado (antes la restaba de sí misma);
  - `moveState == STILL` sale sin más; jugador parado con conductor: `AnnoyPlayerPed`, mirada y `ShakeFist` en la tarea PARCIAL (+0x30) para armas de los slots 1, 2 y 4 (antes se devolvía como respuesta);
  - al final, si la respuesta es `HitPedWithCar`, se incrementa la ira de la inteligencia del peatón con el que choca (antes la del conductor).
- `ComputeVehiclePotentialCollisionResponse` (rehecha):
  - delante/detrás se decide con el vector adelante del PEATÓN y el producto `(z + y) + x`;
  - `IsLooking` estaba invertido (el EXE no hace nada si ya mira);
  - usa `ComputeEntityBoundingBoxPlanes` (0x5F3660, con caché) y la anchura del `CColModel` del modelo;
  - comparaciones estrictas y orden de las tiradas del EXE (la tirada float se hace antes de descartar el vehículo lento), `GetRandomIntInRange` para las enteras, ángulo `LimitRadianAngle(GetRadianAngleBetweenPoints(vehículo, ped))`, nombres de mirada `CompVhclPotCollResp` (jugador) y `CompVehiclePotCollResp` (resto), y la secuencia se reserva antes de añadirle las tareas.
- `HandleEvents`:
  - la tarea abortada se actualiza si NO es nula (estaba invertido);
  - el evento de mayor prioridad se lee antes de `TickEvents` y no se vuelve a pedir;
  - un evento del mismo tipo solo se descarta si el actual no admite interrupción por el mismo evento (`CanBeInterruptedBySameEvent`);
  - al no poder abortar se guarda la tarea primaria, no la activa;
  - la tarea abortada se borra después de calcular la respuesta.
- `SetEventResponseTask`: la expresión facial se pide con `SetRequest(tipo, 10000, NONE, 0)` (la segunda duración es 0, no -1).
- `IsKillTaskAppropriate`: pasa a ser estática (cdecl sin `this`), como la función del EXE; antes el gancho desequilibraba la pila.
- `ComputeEventResponseTask` (evento): `EVENT_INTERIOR_USE_INFO` pasa la tarea activa tal cual (sin `dyn_cast`).
- `Constructor`: el historial y el temporizador ya no se inicializan por valor (el EXE no escribe el relleno de +0x1E y +0x1F); cambio mínimo en `EventHandler.h` y `EventHandlerHistory.h`.

**Método:**
- Original con los hooks del grupo desactivados frente a candidato, con fixtures idénticos y la misma semilla; se llama a la dirección del EXE (`thiscall` o `cdecl`) con un manejador falso de 0x34 bytes cuyas respuestas empiezan con un valor centinela (distingue "no escribe" de "escribe nullptr").
- Se comparan el valor devuelto, las cinco respuestas del manejador, TODAS las tareas nuevas del pool de tareas (tipo y 0x80 bytes con los punteros traducidos, en el orden de los slots, con huecos libres a 0xCD y `m_LastFreeSlot` restaurado), las listas de referencias, la memoria de los fixtures y la traza de las dependencias.
- Falsos: cuatro peatones (con matriz o sin ella, armas, tipo, banderas, datos de jugador e inteligencia con tareas primarias y secundarias), dos vehículos (matriz, velocidad, conductor y modelo falso en `ms_modelInfoPtrs`), un objeto, asociaciones de animación con su jerarquía, eventos con la disposición del EXE y vtable trazada (tipo, origen, prioridad, clon, destructor) y tareas con vtable propia (activa, simple, de asfixia, de pistola, de muerte por sigilo...).
- Simulados y trazados: `FindPlayerPed`/`FindPlayerVehicle`, `CPed::Say`/`SetPedState`, el RNG (`rand` 0x821B1E y `CGeneral::GetRandomNumber`), `IKChainManager_c::IsLooking`/`LookAt`, `CPedIntelligence` (`GetTaskFighting`, `GetTaskUseGun`, `GetTaskDuck`, `IsPedGoingForCarDoor`, `AddTaskPhysResponse`, `AddTaskEventResponseTemp`/`NonTemp`, `IncrementAngerAtPlayer`), `CEventDamage::ProcessDamage`/`ComputeDeathAnim`/`ComputeDamageAnim`, `CInterestingEvents::Add`, `CTaskComplexFallToDeath::CalcFall`, `CTaskSimpleChoking::UpdateChoke`, `CTaskComplexFacial::SetRequest`, las funciones de animación (`GetAnimAssociation`, `AddAnimation`, `BlendAnimation`, `RpAnimBlendClumpGetAssociation`/`GetFirstAssociation`, `SetCurrentTime`), `Fx_c::AddBlood`, `CCam::GetWeaponFirstPersonOn`, `CPlayerPed::AnnoyPlayerPed`, `CPedGeometryAnalyser` (esquinas, planos, dirección a evitar, saltar obstáculo), `ComputeEvasiveStepMoveDir`, `CPedGroups`, `CDarkel`, `CStats::IncrementStat`, `CTheScripts::IsPlayerOnAMission` y `CEventGroup`/`CTaskManager::StopTimers` (`HandleEvents`).
- `ComputeEventResponseTask`: las 59 `Compute*Response` del despacho se sustituyen por simuladas que anotan sus argumentos (evento, tarea activa, tarea simple y tarea abortada) y escriben lo configurado; en `HandleEvents` se simulan `ComputeEventResponseTask` y `SetEventResponseTask`. Esos hooks no se conmutan en esas funciones.
- Los constructores sin hook en el EXE (`CTaskSimpleDead` 0x630590, `CTaskSimpleAchieveHeading` 0x667E20 y `CTaskComplexStuckInAir` 0x67BA40) se redirigen a los de la DLL en las dos ejecuciones para comparar solo la lógica.
- Se restauran en cada ejecución y al final: pools de tareas y de rutas (estados y `m_LastFreeSlot`), secuencias de tareas, modos de cámara y cámara activa, contadores de caos y persecución del jugador, referencias de los fixtures, el reloj y la entrada del modelo falso.
- Fuera de los fixtures: el EXE desreferencia nulos en algunos caminos (asfixia con origen que no es peatón en `ComputeDamageResponse`); el banco los evita. `ComputeKnockOffBikeResponse` y las demás `Compute*Response` fuera del grupo se simulan o se ejecutan en el EXE, no se validan aquí.

Repetir: `python reversing/EvHA/build_probe.py` y `python reversing/run.py --case evha`. Evidencia: [validation-summary.json](validation-summary.json).
