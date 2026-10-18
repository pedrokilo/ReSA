<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventHandler: respuestas a eventos, grupo B (19 funciones)

Validadas el 07/10/2026 con **5.696 pares original/candidato dentro de GTA**, todos exactos: 19 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputeAreaCodesResponse`, `ComputeAttractorResponse`, `ComputeBuildingCollisionResponse`, `ComputeCarUpsideDownResponse`, `ComputeChatPartnerResponse`, `ComputeCopCarBeingStolenResponse`, `ComputeCreatePartnerTaskResponse`, `ComputeDangerResponse`, `ComputeDontJoinGroupResponse`, `ComputeDraggedOutCarResponse`, `ComputeFireNearbyResponse`, `ComputeGunAimedAtResponse`, `ComputeHighAngerAtPlayerResponse`, `ComputeInWaterResponse`, `ComputeInteriorUseInfoResponse`, `ComputeLowAngerAtPlayerResponse`, `ComputeLowHealthResponse`, `ComputeObjectCollisionPassiveResponse`, `ComputeObjectCollisionResponse` |

**Diferencias reales corregidas:**
- Regla general: el EXE solo escribe `m_EventResponseTask` en las ramas que crean o borran la respuesta; cuando falla una condición o la tarea del evento no se contempla, deja el valor anterior. La DLL escribía `nullptr` en esos casos (`ComputeAttractor`, `BuildingCollision`, `CarUpsideDown`, `CopCarBeingStolen`, `CreatePartnerTask`, `Danger`, `DraggedOutCar`, `FireNearby`, `GunAimedAt`, `High/LowAngerAtPlayer`, `LowHealth`, `ObjectCollision(Passive)`); ahora cada rama escribe exactamente lo que escribe el EXE. Se quitan los `NOTSA_UNREACHABLE` de `High/LowAngerAtPlayer`, `LowHealth` y `GunAimedAt`.
- `ComputeAttractorResponse`: con la tarea `USE_EFFECT` y sin hueco libre se borra la respuesta (0x4B9C66); antes no se tocaba.
- `ComputeBuildingCollisionResponse` (rehecha desde el EXE):
  - sin tarea simple de ir a un punto ni objetivo de `KillPedOnFoot` la función termina siempre ahí: crea `HitHead` (colisión frontal, no se puede saltar y el movimiento es más rápido que andar) o borra la respuesta;
  - el punto de destino es la posición del objetivo de `KillPedOnFoot` si existe y, si no, el de la tarea de ir a un punto; antes se usaba siempre el segundo (y se desreferenciaba nulo);
  - la tirada para andar alrededor es `(GetRandomIntInRange(0, 0x400) & 0xF) == 0xF` y se hace siempre antes de mirar la tarea simple (antes `RandomBool(100/15)`);
  - la normal se compara con 0,707 (0x86C8F4), no con `COS_45`, y con NaN no se borra la respuesta (`test ah,41h` incluye C3, que NaN activa);
  - la colisión no frontal con `KillPedOnFoot` sin objetivo, con `SeekEntity` sin entidad o sin ninguna de las dos crea `WalkRoundBuildingAttempt` hacia el punto (antes devolvía nulo);
  - la colisión como objeto sin tarea de ir a un punto no escribe nada.
- `ComputeCarUpsideDownResponse`: el retardo usa `GetRandomIntInRange(100, 400)` (0x407180); no escribe si el ped no está en ese coche.
- `ComputeChatPartnerResponse`: el EXE consume una tirada `GetRandomIntInRange(3, 7)` (0x4B9934) antes de crear la tarea y la descarta.
- `ComputeCopCarBeingStolenResponse`: el nivel de búsqueda es el del ladrón (`m_pPlayerData->m_pWanted`, 0x4BB7C5), no el del jugador 0; la tarea se crea antes de subirlo.
- `ComputeCreatePartnerTaskResponse`: fuera de los tipos 0 a 6 no escribe nada.
- `ComputeDangerResponse`: sin peligro o con otra tarea no escribe nada; con `FLEE_SCENE` y sin ser el conductor borra la respuesta.
- `ComputeDraggedOutCarResponse` (rehecha desde el EXE):
  - los ids de tarea eran incorrectos (usaba `HIT_HEAD` y `EVASIVE_STEP`): el EXE compara 200, 700 (`ENTER_CAR_AS_PASSENGER`), 702 (`STEAL_CAR`), 911 (`SMART_FLEE_ENTITY`), 1000, 1003 y 1105;
  - la tarea de bajarse del coche se crea antes del switch (aunque luego no se use) y siempre se envuelve en una `CTaskComplexSequence` con la respuesta, incluso con respuesta nula o tarea desconocida;
  - 702: pausa o girarse hacia el ladrón (`rand & 0x3FF > 0x200`) y puño, `Say`, y `StealCar` (conductor) o `DragPedFromCar` y `EnterCarAsPassenger`;
  - 1000: si no conviene matar, `SmartFleeEntity(ladrón, false, 60)`; si sí, la secuencia añade `KillPedOnFoot` (sin `DragPedFromCar`) y entra como conductor o pasajero; sin ladrón no hay `Say` ni `KillPedOnFoot`;
  - 1105: ladrón jugador borra la respuesta; 911 sin ladrón y sin conductor crea `StealCar`;
  - el ángulo es `LimitRadianAngle(GetRadianAngleBetweenPoints(ladrón, ped))` con 0,5 y 0,2.
- `ComputeGunAimedAtResponse`:
  - la tarea activa decide huir (908 y 909 `FleeEntity`, 910 y 911 `SmartFleeEntity`); la tarea simple no se usa (antes se llamaba a `GetTaskType` sobre punteros posiblemente nulos);
  - `SMART_FLEE_ENTITY` y `CAR_DRIVE_MISSION_FLEE_SCENE` crean `SmartFleeEntity` (no `FleeEntity`) cuando no se huye con todos los medios;
  - `KILL_CRIMINAL`: el jugador borra la respuesta y si no conviene matar huye con `FleeEntity`;
  - con una tarea de agacharse ya activa no se escribe nada (antes se devolvía esa tarea como respuesta).
- `ComputeInteriorUseInfoResponse`: crea una `CTaskInteriorUseInfo` nueva con los datos del evento (0x675A50); antes clonaba la tarea activa.
- `ComputeObjectCollisionResponse` ya no es copia de la versión pasiva: primero la tarea simple, sin comprobar `bInVehicle`, y el movimiento (con signo, `int16`) antes del objeto.

**Método:**
- Original con el hook de cada función desactivado frente a candidato, con fixtures idénticos y la misma semilla; se llama a la dirección del EXE (`thiscall`) con un manejador falso de 0x34 bytes cuya respuesta empieza con un valor centinela.
- Se comparan el valor final de `m_EventResponseTask`, todas las tareas nuevas del pool de tareas (tipo y 0x80 bytes con los punteros traducidos, en el orden de los slots, con huecos libres a 0xCD y `m_LastFreeSlot` restaurado), las listas de referencias, la memoria de los fixtures y la traza.
- Falsos: cuatro peatones (con matriz o sin ella, armas, tipo, bandera de coche, datos de jugador), dos vehículos con sus datos de manejo (grupo de animación del autobús incluido), edificio, objeto, inteligencia con `KillPedOnFoot`/`SeekEntity`/`EnterCarAsPassengerWait`/`PartnerChat` en las ranuras primarias, evento con vtable trazada y tareas activa, simple (con punto de destino) y de agacharse con vtable propia.
- Simulados y trazados: `FindPlayerPed`, `CPed::Say`, `CPedAttractorManager::HasEmptySlot`, `IKChainManager_c::IsLooking`/`LookAt`, `CTaskSimpleClimb::TestForClimb`, `CPedGroups::IsInPlayersGroup`, `CPedGeometryAnalyser::CanPedJumpObstacle`/`CanPedTargetPed`, `CWanted::SetWantedLevelNoDrop`, `CEventBuildingCollision::CanTreatBuildingAsObject`/`IsHeadOnCollision`, `CPedIntelligence::IsFriendlyWith`/`GetTaskDuck` y el RNG (`rand` 0x821B1E y `CGeneral::GetRandomNumber`). `IsKillTaskAppropriate` y las armas se usan reales.
- Los constructores de `CTaskComplexLeaveCar` (0x63B8C0), `CTaskComplexDragPedFromCar` (0x640430) y `CTaskSimpleAchieveHeading` (0x667E20) no tienen hook: en las dos ejecuciones se redirigen a los de la DLL para comparar solo la lógica de las funciones.
- Se restauran en cada ejecución y al final: estados del pool de tareas y `m_LastFreeSlot`, referencias de los fixtures y el reloj.
- Fuera de los fixtures: el EXE desreferencia nulos en algunos caminos (ladrón nulo al girarse en `DraggedOutCar`, tarea simple nula en `BuildingCollision`); el banco los evita (con ladrón nulo y tarea 1000 fuerza la rama de pausa).
- No se llama a `CEventObjectCollision::IsHeadOnCollision` (0x4ACE80) en `ComputeObjectCollisionResponse`: el EXE descarta el resultado y no tiene efectos laterales.

Repetir: `python reversing/EvHB/build_probe.py` y `python reversing/run.py --case evhb`. Evidencia: [validation-summary.json](validation-summary.json).
