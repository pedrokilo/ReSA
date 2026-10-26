<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCarSub70: CTaskComplexLeaveCar::CreateSubTask(eTaskType, CPed*) 0x641530 (no virtual, lote 70)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-3`, HEAD 1e45e275d + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leavecarsub70\20261026-164245-569754`. (Una corrida previa, `20261026-163200-608810`, dio 914 diferencias: defecto de forma en la rama 206, ver «Hallazgos»; la segunda, `20261026-163740-303138`, ya aprobo, antes de calibrar `coverage.json` con sus conteos.)

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CTask* CTaskComplexLeaveCar::CreateSubTask(eTaskType, CPed*)` | 0x641530 (1214 B) | `CTaskComplexLeaveCar` / `CreateSubTask` / S (`RH_ScopedInstall`) | `Tasks/TaskTypes/TaskComplexLeaveCar.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador): `CTaskComplexLeaveCar,CreateSubTask,0x00641530,1,0,1,S`.

## Que hace (desensamblado 0x641530..0x6419EB, thiscall, dos argumentos en la pila, `ret 8`, EAX = tarea nueva o nulo)

Arbol de comparaciones con signo sobre el tipo pedido; en cada rama `CTask::operator new(tamano)` (0x61A5A0), si es nulo devuelve nulo, si no llama al constructor y devuelve el bloque:

| Tipo | Tarea (tamano) | Constructor (EXE) y argumentos |
|---|---|---|
| 202 PAUSE | CTaskSimplePause (0x18) | 0x48E750 (-1) |
| 206 GET_UP_AND_STAND_STILL | CTaskComplexGetUpAndStandStill (0xC) | 0x678130 () |
| 212 DIE | CTaskSimpleDie (0x28) | 0x62FA00 (0, m_nDieAnimID, m_fDieAnimBlendDelta, m_fDieAnimSpeed) |
| 716 LEAVE_BOAT | CTaskComplexLeaveBoat (0x14) | 0x63BB60 (vehiculo, 0) |
| 719 CAR_DRIVE_TIMED | CTaskSimpleCarDriveTimed (0x70) | 0x5FF940 (vehiculo, m_nDelayTime) |
| 806 CLOSE_DOOR_FROM_OUTSIDE | CTaskSimpleCarCloseDoorFromOutside (0x1C) | 0x6464F0 (vehiculo, puerta, utilidad de alineacion) |
| 809 WAIT_TO_SLOW_DOWN | CTaskSimpleCarWaitToSlowDown (0x14) | 0x646990 (vehiculo, tipo): `m_bDie` -> 2; si no, `m_bSensibleLeaveCar` -> `IsPlayer` (0x5DF8F0, **antes** de reservar) ? 0 : 1; si no 2 |
| 810 WAIT_FOR_DOOR_NOT_TO_BE_IN_USE | CTaskSimpleCarWaitForDoorNotToBeInUse (0x14) | 0x646B70 (vehiculo, puerta, 0) |
| 813 CAR_GET_OUT | CTaskSimpleCarGetOut (0x20) | 0x6471D0 (vehiculo, puerta, utilidad) |
| 814 CAR_JUMP_OUT | CTaskSimpleCarJumpOut (0x24) | 0x64BE40 (vehiculo, puerta, utilidad) |
| 815 CAR_FORCE_PED_OUT | CTaskSimpleCarForcePedOut (0x10, sin reconstruir) | 0x647710 (vehiculo, puerta) |
| 816 CAR_SET_PED_OUT | CTaskSimpleCarSetPedOut (0x18) | 0x6478B0 (vehiculo, puerta, **byte** `m_bSensibleLeaveCar`) |
| 823 COMPLEX_CAR_SLOW_BE_DRAGGED_OUT | CTaskComplexCarSlowBeDraggedOut (0x1C) | 0x648490 (vehiculo, puerta, 1) |
| otro | - | nulo |

## Cambios de produccion (`source.patch`)

`TaskComplexLeaveCar.cpp` solo: `RH_ScopedInstall(CreateSubTask, 0x641530)`, includes de las 12 tareas, un ayudante `NewLeaveCarSubTask<T>(args...)` (operator new + nulo + `::new`) con `static_assert` de los tamanos y el `switch` en C++ puro (sin `__asm`/`naked`).
El `.h` no cambia. Constructor, destructor y `ControlSubTask` (que llama a `CreateSubTask` directamente) no se tocan ni se enganchan.
**Lo unico que sigue delegando en el EXE**: el constructor de `CTaskSimpleCarForcePedOut` (0x647710, `plugin::CallMethod`): la clase es una plantilla sin reconstruir (sin campos ni constructor); no se reconstruye aqui.

## Hallazgos

- **Banco/produccion (corregido en produccion, sin efecto en el resultado final)**: la primera version construia `CTaskComplexGetUpAndStandStill` con `new (memoria) T()`, que por tener constructor `= default` hace inicializacion por valor (pone a cero el bloque antes del constructor; 914 de 1024 casos de 206 con 8 bytes distintos en el arena). El EXE no lo hace. El estado final del objeto real es el mismo (el constructor fija +0, +4 y +8), pero el ayudante usa `::new (memoria) T` sin parentesis cuando no hay argumentos y la rama coincide byte a byte con el EXE.
- Sin defectos de ABI: `ret 8`, no volatiles, DF, SEH y x87 identicos; bytes de bandera no canonicos (`m_bSensibleLeaveCar` = 0x80, 0xFF...) llegan intactos al constructor de `SetPedOut`; los floats de `Die` (NaN con carga util, infinitos, subnormales) viajan como dword sin alterarse.

## Banco

`Original` = bytes del EXE con el hook S apagado; `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI; comprobado en cada lado, `route_ok`). Se llama a la entrada del EXE con ECX = tarea y `(tipo, ped)` en la pila.
Colaboradores (15) sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos EN ORDEN: `CTask::operator new` 0x61A5A0, `CPed::IsPlayer` 0x5DF8F0, `CEntity::RegisterReference` 0x571B70
y los constructores 0x48E750, 0x678130, 0x62FA00 (Constructor-1), 0x63BB60, 0x5FF940, 0x6464F0, 0x646990, 0x646B70, 0x6471D0, 0x64BE40 y 0x648490 (su cuerpo en la DLL se obtiene de la primera llamada de la envoltura `Constructor` del hook, como en PickUpNext68);
0x647710 (CarForcePedOut) solo en el EXE (la DLL lo llama por esa direccion). El constructor de `CTaskSimpleCarSetPedOut` (0x6478B0, sin hook) corre real en ambos lados y se compara el objeto (tipo de tarea en lugar del puntero a la vtable).
Se compara EAX (etiquetado), los cambios en 13 arenas (tarea, vehiculo, utilidad, peaton y 9 bloques de `operator new`), la traza de eventos y el hash de globales; ECX/EDX solo se registran (los llamadores del EXE leen EAX).
16384 pares = 16 familias de 1024: un tipo por familia (13), `WaitToSlowDown` con `m_bDie` / sensata / no sensata, y una mezcla con tipos ajenos (vecinos de los cortes 0x2CC/0x32A/0x330, negativos, extremos). ~1/7 de los casos con el pool agotado,
vehiculo nulo/arena/crudo, puertas y retardos extremos, bytes de bandera no canonicos, `IsPlayer` con basura en los bits altos.
`coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).

No cubierto: la propia validez de los constructores y colaboradores sustituidos (otros bancos); el constructor 0x647710 solo se ve por su secuencia de llamada (sustituido); no hay oraculo independiente en Python (la referencia es el EXE par a par).

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh LeaveCarSub70                              # compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCarSub70/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/LeaveCarSub70/register_hook.py <carpeta-de-evidencia>        # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecarsub70-runtime.start`; salida `leavecarsub70-runtime.jsonl`.
