<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# MiscWave2: banco por pares de CCarEnterExit y CRunningScript

Banco nuevo (estilo `Small2`/`PadMouse1`, un solo banco) para las funciones `validated=0, reversed=1` de `CCarEnterExit` y `CRunningScript` del EXE GTA SA 1.0 US. Compilado y ejecutado: aprobado el 09/10/2026 (ver "Aprobacion" al final).

Uso: `bash reversing/_handoff/bench.sh MiscWave2 3`. Filtro opcional: `miscwave2-only.txt` en la carpeta del juego con `Clase::Funcion`.

Original = hook desactivado; candidato = hook activo; siempre se llama a la direccion del EXE. Todo sobre objetos falsos en memoria propia (vehiculo, peds, matriz, model info, ColModel, scripts y bytecode falsos); nunca se toca el script real. Cada ejecucion restaura los globales tocados (ScriptParams, ScriptSpace[0..0xFFF], locales de mision, CommandsExecuted, datos de switch, OnAMissionFlag, FailCurrentMission, reloj, CHud::m_BigMessage, CWorld::Players, ranura 400 de model infos) y al final se comprueba `state_restored` (globales, entradas parcheadas, control x87).

## Pares (19264)

| Funcion | Casos | Funcion | Casos |
|---|---|---|---|
| ComputeDoorFlag | 512 | Init | 64 |
| ComputeOppositeDoorFlag | 512 | AddScriptToList | 64 |
| ComputePassengerIndexFromCarDoor | 384 | RemoveScriptFromList | 128 |
| ComputeSlowJackedPed | 384 | UpdatePC | 128 |
| ComputeTargetDoorToEnterAsPassenger | 256 | UpdateCompareFlag | 256 |
| ComputeTargetDoorToExit | 512 | GetPointerToLocalVariable | 256 |
| IsCarDoorInUse | 1024 | GetPointerToLocalArrayElement | 512 |
| IsCarSlowJackRequired | 512 | ReadArrayInformation | 512 |
| IsVehicleHealthy / IsPedHealthy | 128 / 128 | GetPointerToScriptVariable | 1024 |
| CarHasDoorToOpen / ToClose / PartiallyOpenDoor / IsCarDoorReady / CarHasOpenableDoor / IsCarQuickJackPossible | 256 c/u | GetIndexOfGlobalVariable | 512 |
| IsClearToDriveAway | 512 | CollectParameters | 1024 |
| IsPathToDoorBlockedByVehicleCollisionModel | 1024 | CollectNextParameterWithoutIncreasingPC | 512 |
| RemoveGetInAnims / RemoveCarSitAnim | 128 / 128 | StoreParameters | 1024 |
| AddInCarAnim | 512 | ReadParametersForNewlyStartedScript | 1024 |
| QuitEnteringCar | 1024 | ReadTextLabelFromScript | 1024 |
| | | GetCorrectPedModelIndexForEmergencyServiceType | 256 |
| | | ThisIsAValidRandomPed / IsPedDead / GetPadState | 256 / 128 / 64 |
| | | DoDeathArrestCheck | 512 |
| | | ProcessOneCommand / Process | 256 / 512 |

## Colaboradores sustituidos (EntryPatch en la direccion del EXE y en el simbolo de la DLL, mismo stub; todas las `Body()` se calculan antes de crear ningun parche)

`CWorld::ProcessLineOfSight` (0x56BA00), `CAnimManager::BlendAnimation` (0x4D4610), `RpAnimBlendClumpGetFirstAssociation` (0x4D6A70), `RpAnimBlendGetNextAssociation` (0x4D6AD0), `RpAnimBlendClumpGetAssociation(uint32)` (0x4D68B0), `CPed::StopNonPartialAnims` (0x5DED10), `CPed::RestartNonPartialAnims` (0x5DED50), `CMessages::ClearSmallMessagesOnly` (0x69DD30), `CCutsceneMgr::IsCutsceneSkipButtonBeingPressed` (0x4D5D10). Los stubs solo actuan en el hilo del banco. El vehiculo falso usa una vtable falsa con trazas por ranura (31/33/35/37 = IsDoorReady/FullyOpen/Closed/Missing U32, 47 = GetRideAnimData): si la DLL llamase otra ranura, la traza lo delata.

## Decisiones del fixture

- Los ids de puerta incluyen valores fuera de dominio (-1, 0, 1, 7, 12, 17, 19, 100): el EXE devuelve 0/-1/false (tabla de saltos con `ja` -> default); la DLL ya no usa `NOTSA_UNREACHABLE` en ComputeDoorFlag/ComputeOppositeDoorFlag.
- `IsCarQuickJackPossible`: el EXE hace consultas virtuales y `CanPedOpenLocks` y siempre devuelve 0 (0x64EF5F `xor al,al`); la DLL las omite. Solo se comparan resultado y memoria (sin traza virtual).
- Los vehiculos falsos tienen el byte 0x48B = 0xFF (sin ColModel propio) para que `CEntity::GetColModel` (0x535300) lea el model info de la ranura 400, que se sustituye temporalmente por uno falso con ColModel, CCollisionData y esferas falsas.
- `GetNearestCarDoor`, `IsVehicleStealable`, `MakeUndraggedDriverPedLeaveCar`, `SetPedInCarDirect` y `SetAnimOffsetForEnterOrExitVehicle` se revisaron y corrigieron pero NO estan en el banco (ver abajo).
- CRunningScript: indices de arrays solo en variables 0..3 (locales 30..33, globales 0x20..0x3C) para que ningun puntero salga del fixture; `CollectParameters`/`StoreParameters` con cuenta >= 0 (el EXE trata la cuenta como `movzx` de 16 bits); `DoDeathArrestCheck` solo con profundidad de pila >= 1 (con 0 el EXE lee `m_IPStack[0xFFFF]`).
- ProcessOneCommand/Process ejecutan solo NOP y WAIT (cada secuencia termina en WAIT, tambien los destinos de scene-skip, IPStack[0] y el codigo de salto). `ScriptParams` se excluye de la comparacion en estos dos (los manejadores propios de la DLL no tienen por que rellenar el area de parametros del EXE).
- Cada secuencia de PutSeq termina con 2 bytes a 0 (NOP): `ProcessOneCommand` de la DLL tiene un `VERIFY(next.Command <= COMMAND_HIGHEST_VANILLA_ID)` (NOTSA, ausente en el EXE) que abortaba con bytes aleatorios tras el WAIT y cortaba la ejecucion en el primer caso.
- AddInCarAnim: el EXE llama DOS veces a la ranura virtual 47 (GetRideAnimData) cuando devuelve no nulo (0x64F733/0x64F743 y 0x64F7D5/0x64F7E3); la fuente se corrigio para reproducirlo (la traza `ride` debe salir duplicada).
- `ReadTextLabelFromScript` compara el buffer de destino completo (0x300 bytes con guardas) incluidos los bytes que strncpy rellena con ceros.
- x87 a 24 bits durante cada llamada (`_controlfp(_PC_24)`), como el juego.

## No cubierto (regla 17)

- `GetNearestCarDoor` (0x6528F0), `IsVehicleStealable` (0x6510D0): dependen de CPedIntelligence, CPedGroups, CUpsideDownCarCheck, CTaskManager y varias consultas del mundo; hacen falta ~12 colaboradores con convenciones distintas. No se doblan con seguridad en este banco.
- `MakeUndraggedDriverPedLeaveCar` (0x64F600): construye un `CEventDraggedOutCar` real (0x4AD250) y llama a `CEventGroup::Add` (0x4AB420) sobre `CPedIntelligence+0x68`; requiere un CPedIntelligence y el pool de eventos reales.
- `SetPedInCarDirect` (0x650280): reserva tareas del pool de tareas reales (`call 0061A5A0`) y llama a `CTaskManager::SetTask` (0x681AF0) y a `CPedAcquaintance::SetAsAcquaintance` (0x608DA0).
- `SetAnimOffsetForEnterOrExitVehicle` (0x64F860): pide y descarga bloques IFP reales (`CStreaming::RequestModel` 0x4087E0, `LoadAllRequestedModels` 0x40EA10, `AddAnimBlockRef` 0x4D3FB0) y escribe estaticos del EXE (0xC18C3C..0xC18C80): E/S de streaming real.
- `CRunningScript::SetCharCoordinates` (0x464DC0) y `GivePedScriptedTask-OG` (0x465C20): tocan el mundo real (`CWorld::PutToGroundIfTooLow`, `ClearSpaceForMissionEntity`, teletransporte de peds/grupos) y los pools de tareas/eventos/registros de tareas de script.

## Supuestos a revisar al compilar

- `FindHook` por nombre de clase `CCarEnterExit`/`CRunningScript` (la segunda se registra con `RH_ScopedCategory("Scripts")`).
- Las sobrecargas `static_cast` de los colaboradores (`BlendAnimation(RpClump*,AssocGroupId,AnimationId,float)`, `RpAnimBlend*Association(..., uint32)`) y que `CPed::StopNonPartialAnims/RestartNonPartialAnims` sean publicas.
- Las ranuras virtuales: los desplazamientos 0x7C/0x84/0x8C/0x94 del EXE = IsDoorReadyU32/FullyOpenU32/ClosedU32/MissingU32 (separacion de 8 bytes = dos ranuras por pareja U32/enum).

## Aprobacion (09/10/2026)

Corrida aprobada `20261009-034057-598047`: 19264 pares, 0 diferencias, 44 funciones, state_restored, saves y configuraciones intactos; sonda ff89df5b... Evidencia en `runs/20261009-034057-598047/` (jsonl en .gz con hash verificado), resumen en `validation-summary.json`, registro con `register_miscwave2.py` (44 funciones) y documentacion con `refresh_documentation_cut311.py`. Segun hooks.csv NINGUNA de las dos clases queda completa: CCarEnterExit conserva 5 pendientes (GetNearestCarDoor, IsVehicleStealable, MakeUndraggedDriverPedLeaveCar, SetAnimOffsetForEnterOrExitVehicle, SetPedInCarDirect) y CRunningScript 2 (GivePedScriptedTask-OG, SetCharCoordinates), todas FUERA del banco y SIN credito. Limites: ProcessOneCommand y Process solo con NOP y WAIT (ScriptParams excluido), indices de arrays solo en variables 0..3, DoDeathArrestCheck solo con pila >= 1, colaboradores trazados sin credito. La corrida fallida anterior (`previous.jsonl`) no se conserva.
