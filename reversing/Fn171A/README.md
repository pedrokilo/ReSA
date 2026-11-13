<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 903 grupos / 10577 funciones reconstruidas; 10577 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn171A: 23 funciones diminutas de tareas del EXE (lote 171A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-5` desde main 19c9abfe5e + los cambios del lote; plantilla `Fn167A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn171a/20261111-222145-140725` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn171A-20261111-222145-140725`, `comparisons.jsonl.gz` de 48,8 MB). 23 x 16384 = **376832 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b80a6caf7d792744b036a0abe5671bed3697be7c5a77765201727d44aec376d8`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 23 tenia hook ni envoltura `plugin::Call*` (se comprobo codigo, `docs/hooks.csv` y `git worktree list`). Se reconstruyen las 23 (ninguna se salta). Las clases del listado eran en su mayoria incorrectas: los nombres reales salen del desensamblado y de las vtables leidas del PE.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CTaskSimpleWaitForPizza` cuerpo del constructor (WaitForBus) | 0x632B30 | 32 | S |
| 1 | `CTaskSimpleWaitForPizza::GetTaskType` (0xEF) | 0x632B50 | 6 | V |
| 2 | `CTaskSimpleWaitForPizza::MakeAbortable` | 0x632B60 | 5 | V |
| 3 | `CTaskSimpleWaitForPizza` cuerpo del destructor | 0x632B70 | 11 | S |
| 4 | `CTaskSimpleWaitForPizza::ProcessPed` (temporizador de 2000 ms) | 0x632B80 | 72 | V |
| 5 | `CTaskSimpleWaitForPizza::Clone` | 0x636B40 | 113 | V |
| 6 | `CTaskSimpleOnEscalator::Clone` (StandStill) | 0x4B86C0 | 89 | V |
| 7 | `CTaskSimpleOnEscalator::GetTaskType` (0x51C) | 0x4B8720 | 6 | V |
| 8 | `CTaskSimpleOnEscalator` destructor con borrado | 0x4B8730 | 30 | S |
| 9 | `CTaskSimpleOnEscalator` cuerpo del destructor | 0x4B8750 | 11 | S |
| 10 | `CTaskSimpleOnEscalator::ProcessPed` (StandStill) | 0x62F520 | 99 | V |
| 11 | `CTaskSimpleStandStill::GetTaskType` (0xCB) | 0x62F350 | 6 | V |
| 12 | `CTaskComplexGoToBoatSteeringWheel::GetTaskType` (0x33E) | 0x6490F0 | 6 | V |
| 13 | `CTaskComplexGoToBoatSteeringWheel::ComputeTargetPos` | 0x649160 | 172 | S |
| 14 | `CTaskSimpleCreateCarAndGetIn` destructor con borrado (GoToBoat) | 0x64E480 | 30 | S |
| 15 | `CVehicleAnimGroupData::ComputeCriticalBlendTime` (GoToBoat) | 0x64E4B0 | 41 | S |
| 16 | `CTaskSimpleArrestPed::Clone` (CopInCar) | 0x68CD10 | 96 | V |
| 17 | `CTaskComplexArrestPed::Clone` (CopInCar) | 0x68CD70 | 96 | V |
| 18 | `CTaskSimpleArrestPed` destructor con borrado (CopInCar) | 0x68CF40 | 30 | S |
| 19 | `CTaskComplexArrestPed` destructor con borrado (CopInCar) | 0x68CF60 | 30 | S |
| 20 | `CTaskComplexWalkRoundObject::GetTaskType` (0x1FE) | 0x6550C0 | 6 | V |
| 21 | `CTaskComplexWalkRoundObject` cuerpo del destructor | 0x6550D0 | 106 | S |
| 22 | `CTaskComplexWalkRoundObject::CreateSubTask` | 0x655290 | 161 | S |

Fuentes: `TaskSimpleWaitForPizza`, `TaskSimpleOnEscalator` (ahora clase completa con vtable 0x85B8DC, derivada de StandStill; `InjectHooks` activado en `InjectHooksMain.cpp`), `TaskSimpleStandStill`, `TaskComplexGoToBoatSteeringWheel`, `TaskSimpleCreateCarAndGetIn`, `VehicleAnimGroupData`, `TaskSimpleArrestPed`, `TaskComplexArrestPed`, `TaskComplexWalkRoundObject` (todas en `source/game_sa/`). Notas: `ComputeCriticalBlendTime` llama ahora a `GetGroup` (0x6E3B00) antes, como el EXE; `ComputeTargetPos` llama a `Multiply3x3Exe` (0x59C790) y el archivo de la clase se compila con `/O2 /Ob0 /Oi /arch:IA32 /fp:fast` (primera corrida: 973 de 16384 pares con diferencias en esa funcion por denormales aplanados por el SSE del juego; con la x87 cero); `CreateSubTask` devuelve el EAX del constructor como el EXE. Los destructores con borrado con cuerpo con hook usan funciones propias sin expansion en linea y `Lote171ABody_...` para el banco.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (ver `LiveComparison.hpp`). Memoria global tocada y restaurada: reloj 0xB7CB84, escalones 0x8CD674/8 y tabla de model infos 0xA9B0C8. 16 familias de 1024 casos por funcion (`Fixtures.hpp`), incluyendo NaN/inf/denormales (salvo dos NaN en una suma), memoria nula, banderas de pila, tipos de subtarea. `python -I fixture_selfcheck.py` pasa los 376832 casos por el oraculo.

## Prueba negativa
`mutate.py` (23 mutaciones por ronda, condicionadas a `this` desalineado). **Las 23 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261111-231011-068216` con 15002 de 47104 pares con diferencias y ronda 2 `20261111-231551-745343` con 6522; el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep de `0x...` / `...h` en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x632B70 -> `Fn149A` (banco del destructor con borrado de Pizza; su colaborador 0x632B70 ahora tiene hook: **repetir**); 0x655290 -> `WalkRoundControl31/README.md` (texto); 0x68CD70 y 0x68CF60 -> `RequestedArrestRemainingFour/evidence` y `TasksPendingEvidence/driveby-arrest-vtables.txt` (evidencia/texto); 0x632B50 y 0x4B8720 -> `HooksActivados/README.md` (texto). Ademas, los bancos de clases tocadas (GoToBoat: Boat355, CTasks6a, FormationFollow355, GangFollower355, GangJoin355, HandSignal355, HassleVehicle355, HitCar355; WalkRoundObject: WalkRound*31) no citan direcciones pero la clase GoToBoat se recompila con x87.
