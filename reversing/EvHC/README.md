<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEventHandler: respuestas a eventos (grupo C)

Validadas el 07/10/2026 con **6.528 pares original/candidato dentro de GTA**, todos exactos: 17 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEventHandler` | `ComputeOnEscalatorResponse`, `ComputeOnFireResponse`, `ComputePassObjectResponse`, `ComputePedEnteredVehicleResponse`, `ComputePedFriendResponse`, `ComputePedSoundQuietResponse`, `ComputePedThreatBadlyLitResponse`, `ComputePedThreatResponse`, `ComputePedToChaseResponse`, `ComputePedToFleeResponse`, `ComputePlayerCollisionWithPedResponse`, `ComputePlayerWantedLevelResponse`, `ComputePotentialPedCollideResponse`, `ComputePotentialWalkIntoFireResponse`, `ComputeReallyLowHealthResponse`, `ComputeReviveResponse`, `ComputeScriptCommandResponse` |

**Diferencias reales corregidas:**
- Regla general: el EXE solo escribe `m_EventResponseTask` (y la tarea de decir o la parcial) en las rutas que crean una tarea o que ponen `TASK_NONE`; sin peatón, sin entidad o con un tipo de tarea no soportado deja los campos intactos. La DLL escribía siempre un nulo y usaba `NOTSA_UNREACHABLE` en los tipos no soportados. Corregido en todas las funciones (sin `FIX_BUGS` en ninguna).
- `ComputePlayerCollisionWithPedResponse`:
  - la mirada (`IKChainManager::LookAt`) siempre es del dueño del manejador hacia la víctima; la DLL la invertía según el caso;
  - "delante" es `dot >= 0` y "detrás" es `dot <= 0` (con NaN falsos), no el bit de signo;
  - la víctima nula no escribe nada;
  - `AnnoyPlayerPed` se llama en las rutas exactas del EXE (no con "detrás" a secas), no se llama a `AsPlayer`, y el caso "ambos andando" sin delante ni detrás no dice nada;
  - el caso parado/ninguno frente a andando/corriendo/esprintando acepta `PEDMOVE_NONE` además de `PEDMOVE_STILL`, y la elección entre caerse (`FallAndGetUp`, sin tarea de decir) y `HitResponse` usa las debilidades de defensa como el EXE;
  - se añade el orden exacto de creación de tareas (golpe, decir; o decir, mirada, golpe).
- `ComputeReviveResponse`: las tareas de levantarse y de decir se crean antes que la de deambular, y esta última se guarda en la ranura `TASK_PRIMARY_DEFAULT` (la DLL usaba `TASK_PRIMARY_PRIMARY`).
- `ComputeOnEscalatorResponse`: el EXE cambia la vtable de la tarea a la de `CTaskSimpleOnEscalator` (0x85B8DC); la DLL dejaba una `CTaskSimpleStandStill` normal.
- `ComputePlayerWantedLevelResponse`: el EXE crea un `CTaskComplexPolicePursuit`; la DLL abortaba.
- `ComputePedEnteredVehicleResponse`:
  - una sola tirada `GetRandomIntInRange(300, 600)` (0x407180) antes del `switch`, se use o no; la DLL tiraba la plantilla entera, que puede salir desfasada en uno;
  - sin vehículo, peatón o `bInVehicle` no se escribe nada.
- `ComputePedThreatResponse`:
  - `TASK_COMPLEX_FLEE_ENTITY` crea `CTaskComplexFleeEntity` (no la variante smart);
  - `TASK_COMPLEX_SMART_FLEE_ENTITY` estaba invertida: policía buscado y cop -> `FleeAnyMeans`, el resto -> `SmartFleeEntity` (andando solo con el evento "no gusta");
  - `TASK_SMART_FLEE_ENTITY_WALKING` pone el estado de movimiento en caminar;
  - se añade `TASK_COMPLEX_KILL_PED_ON_FOOT_STAND_STILL` (marca `bStayInSamePlace`);
  - `TASK_SIMPLE_DUCK` con tarea de agacharse ya presente no escribe nada (la DLL la reutilizaba);
  - `TASK_COMPLEX_LEAVE_CAR` exige vehículo y `bInVehicle`;
  - `TASK_SIMPLE_GANG_DRIVEBY` es solo para pasajeros (el conductor no), busca `TASK_SIMPLE_CAR_DRIVE` (0x2C5) y no la conducción compleja, pide el arma con habilidad STD, toma el máximo con `>` (NaN deja el rango de visión + 5) y compara la distancia con raíz, no al cuadrado;
  - `TASK_COMPLEX_KILL_PED_AND_REENTER_CAR` crea primero la secuencia, luego la tarea de matar y luego la de conducir.
- `ComputePotentialPedCollideResponse`: la distancia al líder es 2D (z a cero), el estado de movimiento se compara con signo, el líder jugador se decide por `m_pPlayerData` y se quita la rama `FIX_BUGS`.

**Método:**
- Original (hooks desactivados) frente a candidato con fixtures idénticos; las funciones se llaman en la dirección del EXE con `this` = manejador propio de 0x34 bytes con centinelas en las tareas de respuesta, de decir y parcial.
- Tres peatones falsos (vtable de trampas, matriz propia o posición en la colocación, inteligencia, estadísticas y arma activa), un vehículo (datos de manejo, conductor), diez tareas falsas (vtable propia: destructor, `GetSubTask`, `IsSimple`, `GetTaskType`, `MakeAbortable`) y eventos escritos a mano sobre una vtable falsa (`GetEventType`, `GetSourceEntity`, `CloneScriptTask`).
- Las tareas nuevas del pool se siguen por hueco (`m_LastFreeSlot` restaurado y huecos libres con 0xCD): se compara el tipo, los 0x80 bytes y las referencias registradas, y se borran después con la secuencia desenganchada de sus subtareas.
- Se simulan y trazan `CPed::Say`, `AnnoyPlayerPed`, `ComputePedHitSide`, `RestartNonPartialAnims`, `GetWanderTaskByPedType`, `IsKillTaskAppropriate` (cdecl en el EXE, thiscall en la DLL), `CInterestingEvents::Add`, `IsPlayerOnAMission`, `IKChainManager_c::LookAt`, `GetPedFOVRange`, `GetMoveStateFromGoToTask`, `CEventGroup::Add`, el registro de tareas de script y el generador aleatorio (rand 0x821B1E y `CGeneral::GetRandomNumber`).
- Se guardan y restauran el reloj, las 64 secuencias de tareas y las referencias de las entidades.

Repetir: `python reversing/EvHC/build_probe.py` y `python reversing/run.py --case evhc`. Evidencia: [validation-summary.json](validation-summary.json).
