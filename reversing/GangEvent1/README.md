<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GangEvent1 - banco de CGroupEventHandler (30 fns) y CGangWars (32 fns)

Banco "estilo viejo" (como PadMouse1): se engancha en `CGame::Process` y, por funcion, ejecuta el original del EXE (hook con `State(false)`, llamada a la direccion del EXE) frente al candidato de la DLL (`State(true)`). Se compara traza de colaboradores, valor devuelto y estado tocado. x87 a 24 bits.

## Corpus (17792 pares)
Grupo: IsKillTask 256, Walk 256, Stare 384, VehicleDamage 384, ShotFired 256, SexyPed 384, SeenCop 256, PlayerCommand 256, PedThreat 384, PedFriend 384, NewGangMember 256, LeaderExitedCar 512, LeaderEnterExit 256, GunAimedAt 384, Gather 192, DraggedOutCar 384, Danger 192, Damage 384, MemberResponses 384, LeanOnVehicle 192, KillThreatsBasic 256, KillPlayerBasic 256, HassleThreat 384, HassleSexyPed 256, HandSignal 192, Greet 512, Flee 384, Driveby 128, EventResponseTasks 768, DoDeal 512.
GangWars: 32 funciones (Init 64 ... UpdateTerritory 384, PickZone 512, CreateDefending 512, StartOffensive 512); ver la tabla `fns` en `LiveComparison.hpp`.

## Mundo simulado
- Peds, inteligencias, matrices, vehiculos y grupos falsos en buffers propios; pool de peds/vehiculos falso (se intercambian 0xB74490/0xB74494 solo durante la llamada).
- Colaboradores con stub y traza: FindPlayerPed, SetTask, FindTaskByType/FindActive, rand (EXE y DLL), LookAt, EventGroup::Add, ComputeEventResponseTask, ComputeResponseTaskType, radar, stats, mensajes, poblacion, E/S de guardado.
- Tareas/eventos reservados en arenas rellenas con 0xCD (operator new/delete de CTask y asignadores); el relleno no escrito se ignora. SetTask describe un clon de la tarea (el relleno de pila del temporal difiere).
- Estaticos de CGangWars (0x96AB28..0x96ABD8), zonas (0xBA3794, 0xBA1DE8), poblacion/streaming y reloj 0xB7CB84 se guardan y restauran (`state_restored`).

## Exclusiones
- LeaderEnterExit: solo camino ProcessPed==true (el otro llega a CAnimManager::BlendAnimation con peaton falso).
- CGangWarsSaveStructure::Construct corregido en la fuente (0x5D2671 pone GangWarNavigationZoneIndex=-1 siempre y su bucle depende de pZoneToFightOver); ya no se enmascara.
- Los asignadores devueltos NO se borran con delete: el destructor deleting del EXE (0x69DB00) libera inline en el pool real 0xB744BC; se llama solo al destructor (flag 0).

## Uso
`bash reversing/_handoff/bench.sh GangEvent1 3`  (filtro: `gangevent1-only.txt`; salida `gangevent1-runtime.jsonl`).
No compilado todavia: revisar firmas/ABI de stubs, overloads de simbolos DLL, zancadas de pools (0x7C4/0xA18) y sizeof de tareas.

## Aprobación (09/10/2026)

Corrida `20261009-162300-336352`: passed=true, 17792 pares, 62 funciones con 0 fallos (30 de CGroupEventHandler y 32 de CGangWars), evento complete (state_restored), saves y configuraciones intactos. Registrada con `register_gangevent1.py` (evidencia en `runs/`, `validation-summary.json`). Con esto CGroupEventHandler y CGangWars quedan COMPLETAS. Límites: `ComputeResponseLeaderEnterExit` solo con ProcessPed==true; peds, grupos y vehículos falsos con pools falsos temporales; objetos devueltos sin `delete` (solo destructor); colaboradores doblados sin crédito.

## Revalidación 19/10/2026

Corrida `20261019-004051-297047` (informe en `resultados-gangevent1`): passed=true, 17792 pares, 62 funciones, 0 diferencias, estado restaurado, saves y configuraciones intactos. Sonda compilada desde el worktree `D:\Original\wq-f` (HEAD 98286d43c + fusión de CQuaternion; ver `artifacts/build-manifest.json`). Registrada con `register_revalidation_20261019.py` en `validation-summary-reval-20261019.json` (el `validation-summary.json` histórico no se toca; la evidencia anterior queda en `previous_evidence` del registro). Los hashes de `tested_sources` son los del HEAD compilado; el árbol principal tiene además trabajo sin commit del lote 27 (sin validar) en `GangWars.h` y `GroupEventHandler.cpp`, por lo que esos archivos no coinciden hoy con el árbol de trabajo.
