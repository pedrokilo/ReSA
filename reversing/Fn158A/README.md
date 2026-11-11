<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 885 grupos / 10129 funciones reconstruidas; 10129 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn158A: 23 funciones diminutas del EXE (lote 158A)

Estado: **APROBADO dentro del juego** (10/11/2026, worktree `wv-2` desde main 9277fb4c56 + los cambios del lote; plantilla `Fn155A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn158a/20261110-182655-461677` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn158A-20261110-182655-461677`; `comparisons.jsonl.gz` de 41,1 MB, bajo el limite de GitHub; el crudo de 682 MB se borro de E:). 23 x 16384 = **376832 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, 376832 casos, 0 lados distintos del modelo). SHA256 de la sonda ejecutada: `c7ae2b6980a8215b723fbd834ecdf775e66af00036c8c0eb8e720b187b011342`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Las 23 direcciones del encargo **no tenian hook** (comprobado en codigo, `docs/hooks.csv` y `git worktree list`). Las 23 son viables en C++ puro (sin `__asm`/`naked`); no se salto ninguna. El listado atribuia las clases por cercania de direcciones y **varias eran incorrectas**: los nombres reales salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CEventLeaderEntryExit::HasEditableResponse` (vtable 0x859540, ranura 9; xor al,al) | 0x43E1E0 | 3 | `Events/EventLeaderEntryExit.cpp` (+ `.h`) | V |
| 1 | `CEventLeaderEntryExit::GetSourceEntity` (ranura 10; nulo) | 0x43E1F0 | 3 | idem | V |
| 2 | `CEventLeaderEntryExit::GetLifeTime` (ranura 3; 0) | 0x43E210 | 3 | idem | V |
| 3 | `CEventLeaderEntryExit::CanBeInterruptedBySameEvent` (ranura 15; mov al,1) | 0x43E280 | 3 | idem | V |
| 4 | `CEventLeaderEntryExit::GetEventType` (ranura 1; 0x4D) | 0x43E200 | 6 | idem | V |
| 5 | `CEventLeaderEntryExit::GetEventPriority` (ranura 2; 0x37) | 0x43E2C0 | 6 | idem | V |
| 6 | `CEventLeaderEntryExit` cuerpo del destructor (`mov [ecx],859540h; jmp 4B2210`) | 0x43E2B0 | 11 | idem | S |
| 7 | `CEventLeaderEntryExit::ScalarDeletingDestructor` (ranura 0) | 0x43E290 | 30 | idem | S |
| 8 | `CEventLeaderEntryExit::Clone` (ranura 4, marco SEH) | 0x43E220 | 88 | idem | V |
| 9 | `CTaskSimple::GetSubTask` (vtable 0x86D4A8, ranura 2; el listado decia CTaskSimpleUninterruptable; la comparten las vtables de todas las tareas simples) | 0x43E300 | 3 | `Tasks/TaskSimple.cpp` (+ `.h`; fichero nuevo) | V |
| 10 | `CTaskSimple::IsSimple` (ranura 3; mov al,1) | 0x43E310 | 3 | idem | V |
| 11 | `CTaskSimple::SetPedPosition` (ranura 8; xor al,al; `ret 4`) | 0x43E320 | 5 | idem | V |
| 12 | `CTaskSimpleUninterruptable::ProcessPed` (vtable 0x859580, ranura 7; `ret 4`) | 0x43E3A0 | 5 | `Tasks/TaskTypes/TaskSimpleUninterruptable.cpp` (+ `.h`) | V |
| 13 | `CTaskSimpleUninterruptable::GetTaskType` (ranura 4; 0xC9) | 0x43E380 | 6 | idem | V |
| 14 | `CTaskSimpleUninterruptable::MakeAbortable` (ranura 6; `cmp dword [esp+8],2; sete al; ret 0Ch`) | 0x43E390 | 11 | idem | V |
| 15 | `CTaskSimpleUninterruptable` cuerpo del destructor (`mov [ecx],859580h; jmp 61A3A0`) | 0x43E3D0 | 11 | idem | S |
| 16 | `CTaskSimpleUninterruptable::ScalarDeletingDestructor` (ranura 0) | 0x43E3B0 | 30 | idem | S |
| 17 | `CTaskSimpleUninterruptable::Clone` (ranura 1, marco SEH) | 0x43E330 | 80 | idem | V |
| 18 | `CTaskSimpleGoToPointFine::GetTaskType` (vtable 0x86F964, ranura 4; 0x39E) | 0x65EEE0 | 6 | `Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp` (+ `.h`) | V |
| 19 | `CTaskSimpleGoToPointFine` cuerpo del destructor (`mov [ecx],86F964h; jmp 667A00`) | 0x65EEF0 | 11 | idem | S |
| 20 | `CTaskComplexTrackEntity::ScalarDeletingDestructor` (vtable 0x86F998, ranura 0; el listado decia CTaskSimpleGoToPointFine) | 0x663620 | 30 | `Tasks/TaskTypes/TaskComplexTrackEntity.cpp` | S |
| 21 | `CTaskGangHasslePed::Clone` (vtable 0x86FA00, ranura 1; el listado decia CTaskSimpleGoToPointFine) | 0x6620D0 | 108 | `Tasks/TaskTypes/TaskGangHasslePed.cpp` (+ `.h`) | V |
| 22 | `CTaskSimpleCarForcePedOut::GetTaskType` (vtable 0x86EE94, ranura 4; 0x32F; el listado decia CTaskSimpleCarJumpOut) | 0x647770 | 6 | `Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp` (+ `.h`) | V |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`); tipo S = `RH_ScopedNamedGlobalInstall` (destructores con borrado y cuerpos de destructor: la macro V no puede nombrar la ranura 0). Las 23 filas nuevas de `docs/hooks.csv` siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S|V`.

## Funciones saltadas
Ninguna. `X87Math.cpp` no se toca (ninguna funcion usa la x87) y no hay opciones de compilacion nuevas en `source/CMakeLists.txt`.

## Cambios de produccion
- Constantes virtuales de `CEventLeaderEntryExit`, `CTaskSimpleUninterruptable`, `CTaskSimpleGoToPointFine` y `CTaskSimpleCarForcePedOut` (y `CTaskGangHasslePed::Clone`) pasan de inline en la cabecera a declaradas en `.h` y definidas en `.cpp`. `CEventLeaderEntryExit` gana las redefiniciones `GetEventPriority`, `HasEditableResponse` y `GetSourceEntity` (antes heredaba las de `CEventAreaCodes`/`CEvent`; mismo valor, ranura propia en el EXE).
- `GetSubTask`, `IsSimple` (ambas `final`) y `SetPedPosition` eran inline en `TaskSimple.h`: pasan al fichero nuevo `Tasks/TaskSimple.cpp` con `CTaskSimple::InjectHooks` (`RH_ScopedVirtualClass(CTaskSimple, 0x86D4A8, 9)`, llamado desde `CTask::InjectHooks`). Cambio de cabecera: recompila todo lo que incluye `TaskSimple.h`; el fichero nuevo exige reconfigurar CMake (GLOB_RECURSE).
- `AreaCodesDestructorBody` (0x4B2210) y `GoToDestructorBody` (0x667A00/0x667D50) pasan del espacio anonimo a `CEventAreaCodesBody` y `CTaskSimpleGoToBody` para que los cuerpos de las clases derivadas los llamen. `CEventAreaCodes::Constructor` (0x4B2190) pasa a `__declspec(noinline)` con `friend class CEventLeaderEntryExit` (su `Clone` lo llama como el EXE). `CTaskGangHasslePed::Constructor` pasa a `noinline`.
- `Clone` de los tres: `CEvent::operator new` / `CTask::operator new` (con hook) y el constructor enganchado; escriben la vtable del EXE (0x859540 / 0x859580) porque la de la DLL no vale para los llamadores del EXE; los de evento y de `CTaskSimpleUninterruptable` devuelven la memoria, el de `CTaskGangHasslePed` devuelve lo que deja el constructor en EAX (como el EXE).

## Revision contra el EXE (desensamblado propio con capstone sobre el PE canonico)
- Vtables leidas del PE: 0x859540 = [0x43E290, 0x43E200, 0x43E2C0, 0x43E210, 0x43E220, 0x4B2270, 0x4B23A0, 0x420E90, 0x420EA0, 0x43E1E0, 0x43E1F0, 0x4B2390, 0x420EB0, 0x420EC0, 0x4AC020, 0x43E280]; 0x859580 = [0x43E3B0, 0x43E330, 0x43E300, 0x43E310, 0x43E380, 0x421180, 0x43E390, 0x43E3A0, 0x43E320]; 0x86F964 con [4] = 0x65EEE0; 0x86F998 con [0] = 0x663620; 0x86FA00 con [1] = 0x6620D0; 0x86EE94 con [4] = 0x647770 y [2], [3], [8] = 0x43E300/310/320; la vtable de CTaskSimple 0x86D4A8 trae 0x43E300, 0x43E310, 0x43E320 en las ranuras 2, 3 y 8.
- 0x43E220 (`Clone`): `eax = 4B5620(10h)`; si no es nulo: `ecx = eax`, `push [edi+0Ch]`, `call 4B2190` (constructor de `CEventAreaCodes`), `[esi] = 859540h`, devuelve esi; nulo: devuelve 0. 0x43E330: `61A5A0(8)`, `call 61A390`, `[esi] = 859580h`, devuelve esi. 0x6620D0: `61A5A0(2Ch)`, `call 65FED0` con `[+0Ch], [+10h], [+14h], [+18h]` y devuelve el EAX del constructor.
- Destructores con borrado 0x43E290 / 0x43E3B0 / 0x663620 (30 bytes): `push esi; mov esi,ecx; call <cuerpo>; test byte [esp+8],1; je ..; push esi; call <liberacion>; add esp,4; mov eax,esi; pop esi; ret 4`; cuerpos 0x43E2B0, 0x43E3D0 y 0x65F460; liberaciones `CEvent::operator delete` 0x4B5630 y `CTask::operator delete` 0x61A5B0. Solo cuenta el bit 0 del BYTE de banderas.
- Cuerpos de destructor (11 bytes): vtable de la clase y `jmp` a la base (0x4B2210, 0x61A3A0, 0x667A00, las tres con hook).
- 0x43E390 compara el dword completo de la prioridad con 2 (`sete al`): 0x102 o 0x80000002 dan falso.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH (S o V: enlace de la entrada) apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40). Sin memoria global del juego. Plantilla `Fn155A`.
Falsos identicos en la direccion del EXE y en la funcion de la DLL (`F` / `H` / `Rt`): cuerpos de base 0x4B2210, 0x61A3A0 y 0x667A00 (con hook, `F`), cuerpo 0x65F460 (con hook propio, `H`: el hook se apaga y el falso va en la funcion libre de la DLL), cuerpos 0x43E2B0 y 0x43E3D0 (otras funciones probadas del lote, `Rt`), `CEvent::operator delete` 0x4B5630, `CTask::operator delete` 0x61A5B0, `CEvent::operator new` 0x4B5620, `CTask::operator new` 0x61A5A0 y los constructores 0x4B2190, 0x61A390 y 0x65FED0 (devuelven un EAX arbitrario del caso y dejan un dword arbitrario en [ECX]).
Se observa: bytes cambiados de las tres arenas, eventos de los falsos con todos sus argumentos y **el dword que tenia [ECX] al entrar (la vtable que dejo la funcion probada)**, valor devuelto (this, objeto nuevo o EAX del constructor como etiqueta; EAX completo en constantes; AL en bool) y estado x87 final (vacio). **EAX, ECX, EDX y EFLAGS tras la llamada** quedan en `original_residue` / `candidate_residue` (informativos).
16 familias de 1024 casos por funcion (ver `Fixtures.hpp`): posicion de `this` x relleno de las arenas; vtable previa; banderas 0, 1, 2, 3, 0xFF, 0x100, bits altos y aleatorias; prioridades de `MakeAbortable` (2, 0, 1, 3..5, 0xFFFFFFFF, 0x80000002, 0x102, 0x10002, 0x20000002...); memoria disponible o no en los `Clone`, ped en la arena o crudo, EAX del constructor nulo u otro.
`python -I fixture_selfcheck.py` (sin juego) pasa los 376832 casos por el oraculo y mide la cobertura de ramas (minimo 50 por funcion).

## Oraculo independiente
`fixture_oracle.py` reimplementa solo lo observable desde el desensamblado; no usa ninguna constante ni rama del C++ de produccion.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda; se aplican a las fuentes, se recompila la sonda y se corre con `FN158A_CASES=2048` por funcion; la corrida aprobada usa 16384). Las mutaciones se condicionan a `this` u objeto desalineado o a banderas imposibles para que la partida no se caiga antes del banco. **Las 23 funciones fallan en ambas rondas** (`negative-runs/`: `ronda1-20261110-190202-826514` con 17751 de 47104 pares con diferencias y `ronda2-20261110-190719-724491` con 13669; el lado EXE coincide con el oraculo en todos los pares: "lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, "restauradas 7 fuentes").

## Bancos repetidos
El grep de las direcciones (con y sin ceros, y en la forma `...h`) en `reversing/` solo encuentra 0x43E300/0x43E310 como ranuras de vtables sinteticas en `Tasks18` y `CTaskDead` (mas los README de `HooksActivados`, `CTaskDead` y `TasksPendingEvidence`, que son texto). Se repiten esos dos y una muestra de 6 bancos de las clases tocadas (8 en total, uno a uno bajo los candados compartidos), todos **APROBADOS sin adaptar**; su evidencia se refresco con `reversing/_handoff/refresh_bank.py`:
- `Tasks18` (20261110-191308-928349), `CTaskDead` (20261110-193015-713139), `Fn144A` (CEventLeaderEntryExit; 20261110-193239-480658), `Fn153A` (CTaskGangHasslePed y eventos; 20261110-201154-705387), `Fn140A` (CTaskGangHasslePed; 20261110-205419-349858), `ForcePedOutVt71` (CTaskSimpleCarForcePedOut; 20261110-211219-602286; un primer intento, 20261110-210823-234067, termino con "Error dentro de GTA" por coincidir con otro GTA y no se archivo), `EventsTasks56` (20261110-212004-761287) y `Fn154A` (CTaskGangHasslePed; 20261110-212740-669393).
- Intentado y no ejecutable: `UnlockTrackEntity` (su script falla con un error de E/S de Python antes de lanzar el juego; banco anterior a `bench_here.sh`, sin cambios en este lote).
- No repetidos (mismas clases): `Boat355` y los bancos 355 de tareas de grupo (`GangJoin355`, `GangFollower355`, `FormationFollow355`, `HandSignal355`, `HassleVehicle355`, `HitCar355`, `SmartFlee355`, `Sunbathe355`, `VehicleLean355`, `Wander355`), `Fn108A`, `Fn115A`, `Fn132A`, `Fn90A`, `STasks8a`, `TasksBatch`, `GangEvent1`, `GangHasslePending`, `TaskFinal1`, `LeaveCarSub70`, `ForcePedOut71`.
