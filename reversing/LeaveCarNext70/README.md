<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCarNext70: CTaskComplexLeaveCar::CreateNextSubTask (0x6419F0), lote 70

Estado: **APROBADO en el juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 1e45e275d + `source.patch`): 16384 pares, 0 diferencias, todas las compuertas en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leavecarnext70\20261026-163436-192141`. (Corridas previas, no aprobadas: `...-162236-384135` encontro 3 defectos reales de produccion, ver «Hallazgos»; `...-162801-797816` murio por «GTA no responde» antes de empezar los casos y se repitio sin cambios.)

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CTask* CTaskComplexLeaveCar::CreateNextSubTask(CPed*)` | 0x6419F0 (vtable 0x86E828 + 8*4) | `CTaskComplexLeaveCar` / `CreateNextSubTask` / V (`RH_ScopedVMTInstall`) | `Tasks/TaskTypes/TaskComplexLeaveCar.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador; virtual): `CTaskComplexLeaveCar,CreateNextSubTask,0x00000000,1,0,1,V`.
Constructor, destructor, `ControlSubTask`, `MakeAbortable`, `CreateFirstSubTask` y `CreateSubTask` no se tocan (siguen igual que en HEAD; `CreateSubTask` 0x641530 es una envoltura que reconstruye otro agente).

## Que hace (desensamblado 0x6419F0..0x641F83, 1428 bytes, thiscall `ret 4`, EAX = tarea nueva o nulo)

El tipo de la subtarea actual (`GetTaskType`, vtable +0x10 de `m_pSubTask` +8) decide; casi siempre devuelve `CreateSubTask(tipo, ped)` (0x641530):
- 0xCA pausa, 0xD4 muerte -> `TASK_FINISHED` (0x516). 0xCE (levantarse) -> `SET_PED_OUT` (0x330) si `bInVehicle`, si no 0x516. 0x2CC (salir del bote) -> muerte si `m_bDie`, si no 0x516.
- 0x2CF: sin `bInVehicle` -> 0x516; si el peaton no es pasajero ni conductor de `ped->m_pVehicle` (IsPassenger 0x6D1BD0, IsDriver 0x6D1C40) limpia `bInVehicle` -> 0x516; si no calcula la puerta (0x63BAB0, `ComputeTargetDoorToExit` si `m_nTargetDoor == 0`), consulta 0x646CF0 (banderas de entrada/salida de la puerta) y, sin `m_bForceGetOut`, devuelve 0x32A (puerta en uso para bajar) o 0x516 (en uso para subir); si no 0x329.
- 0x326 -> `m_nDieAnimID = 0xF`, 0x330. 0x32A -> 0x329. 0x32F -> 0x330. 0x330 -> muerte si `m_bDie`, si no 0x516.
- 0x329 (esperar a frenar): si `CanPedStepOutCar(false)` (vtable +0xE4 del vehiculo) es falso -> reserva puerta (0x63BA00), `SetPedState(0)` y 0x32E. Si no, mira `IsRoomForPedToLeaveCar(veh, puerta, 0)`; sin sitio busca la puerta alternativa (8->10, 9->11, 10->8, 11->9 segun conductor/pasajeros/banderas de entrada, moto o asientos en tandem, 2o `IsRoomForPedToLeaveCar`, nodo de carroceria si `m_bForceDoorCheck`); si no la hay (`giveUp`) reserva puerta, `SetPedState(0)`, crea la utilidad de alineacion (0x63BAE0) y devuelve 0x32F; si hay sitio: 0x337 si `m_bDie`, si no crea la utilidad y 0x32D. La reserva (0x63BA00) llama a `ComputeDoorFlag`, `SetGettingOutFlags`, `m_nNumGettingIn++` y, segun `IsPlayer` del conductor/peaton, pone el piloto automatico a parado (0x3BA/0x3D0) y el estado de la entidad a `STATUS_FORCED_STOP` (0x36).
- 0x32D (bajando): `m_pTaskUtilityLineUpPedWithCar->m_nDoorOpenPosType = 2`; 0x326 si `CarHasDoorToClose` (salvo forzado con el vehiculo desbloqueado); si es automovil y tiene puerta que cerrar, sube el estado de la puerta (0 o 2 -> +1, tres llamadas independientes a `GetDoorStatus_Component`); 0x330.
- 0x32E (saltar): `m_bDie` -> anim 0xF, mezcla 1000.0f, velocidad 0.5f y muerte; sin vehiculo, moto o quad -> 0x516; `m_bIsInAir` o `bIsDrowning` (bit 19 de +0x470) -> 0x330 (sin escribir la animacion: el salto 0x641A9E cae despues de esa escritura); si no `ClearGettingOutFlags`, pone a 0 las banderas/contador reservados y devuelve 0xCE.
- 0x337: anim 0x24, mezcla 1000.0f, velocidad 0.5f; si el vehiculo tiene `m_nDoorLock == 5` (CARLOCK_COP_CAR) lo pone a 1; muerte (0xD4).
- Cualquier otro tipo: nulo.

## Cambios de produccion (`source.patch`)

`TaskComplexLeaveCar.cpp` solo: `RH_ScopedVMTInstall(CreateNextSubTask, 0x6419F0)`, el cuerpo en C++ puro (sin `__asm`/`naked`), includes, `static_assert` de los desplazamientos de CVehicle que se leen/escriben y cuatro ayudantes locales en un espacio de nombres anonimo (las ayudantes en linea del EXE 0x63BAB0, 0x63BA00, 0x63BAE0 y 0x646CF0). La utilidad de alineacion se reserva con `operator new` + comprobacion de nulo + `new (memoria)`, como el EXE. El `.h` no cambia. Sin floats calculados (solo las constantes 1000.0f y 0.5f).

## Hallazgos

Primera corrida (892 de 16384 pares con diferencias; los tres eran defectos reales de produccion, corregidos y revalidados):
1. 0x32E en el aire: la primera version escribia `m_nDieAnimID = 0xF`; el salto `jne 0x641A9E` del EXE cae DESPUES de esa escritura (solo la hace la ruta 0x326).
2. 0x337: `CARLOCK_LOCKED_PLAYER_INSIDE` vale 4; el EXE compara con 5 (`CARLOCK_COP_CAR`).
3. 0x32D: el EXE pasa la puerta como dword a `GetDoorStatus_Component`/`SetDoorStatus_Component`; en la DLL el parametro es `tComponent` (uint8). No es un defecto del banco ni se corrige en produccion (cambiar CDamageManager queda fuera de alcance): las fixtures de 0x32D limitan la puerta a 0..255 (el juego solo usa 0..11). Ids de puerta > 255 en esa rama quedan sin validar.
Una corrida abortada: GTA «no responde» antes de los casos (infraestructura), repetida sin cambios. La primera ejecucion del banco murio con exit 3 porque `operator new` (0x82119A) estaba sustituido mientras el propio banco reservaba parches: el parche de `operator new` es el ultimo de la lista y `Patches` reserva su vector antes (defecto del banco, corregido).

## Banco

`Original` = bytes del EXE con el hook V apagado (entrada 0x6419F0 y ranura de la vtable del EXE intactas); `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI y la ranura 0x86E848 apunta a el; comprobado en cada lado, `route_ok`). Se llama a la entrada del EXE con ECX = tarea.
Colaboradores (15) sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos EN ORDEN y devuelven valores del caso: `CreateSubTask` 0x641530 (colaborador: en este arbol sin hook se parchea la envoltura `CTaskComplexLeaveCar::CreateSubTask` de la DLL; cuando se integre su hook, el banco parchea el cuerpo del hook, `run_local.py` acepta ambos), constructor de `CTaskUtilityLineUpPedWithCar` 0x64FBB0, `CPed::IsPlayer` 0x5DF8F0 y `SetPedState` 0x5E4500, `CCarEnterExit::ComputeDoorFlag` 0x64E550, `ComputeTargetDoorToExit` 0x64F110, `IsRoomForPedToLeaveCar` 0x6504C0 y `CarHasDoorToClose` 0x64EE10, `CVehicle::SetGettingOutFlags` 0x6D3020, `ClearGettingOutFlags` 0x6D3060, `IsPassenger(CPed*)` 0x6D1BD0 e `IsDriver(const CPed*)` 0x6D1C40, `CDamageManager::GetDoorStatus_Component` 0x6C2250 y `SetDoorStatus_Component` 0x6C21E0 (en la DLL se parchea el miembro que llama el codigo reconstruido, no el envoltorio del hook) y `operator new` 0x82119A (el ultimo parche: la DLL reenvia a esa direccion). `CanPedStepOutCar` se llama por la vtable falsa del vehiculo (ranura 57), igual en ambos lados. `GetTaskType` de la subtarea es una vtable falsa. `SetGettingOutFlags` recibe en el EXE basura en los bytes altos del argumento (pila sin inicializar): el falso lo enmascara a 8 bits.
Se compara EAX (etiquetado), los cambios en 8 arenas (tarea, subtarea, peaton, vehiculo, manejo, utilidad existente, utilidad nueva y objeto auxiliar), la traza de eventos y el hash de globales (no hay regiones globales); ECX/EDX solo se registran (el unico llamador virtual lee EAX).
16384 pares = 16 familias de 1024 (tipos ajenos, tipos sencillos, 0x2CF, 0x329 sin poder bajar con calma / con sitio / puertas 8-9 / 10-11 / raras / todo aleatorio, 0x32D normal y con automovil, 0x32E con y sin muerte, 0x337 y 0x330, mezclas con pool de la utilidad agotado y utilidad nula/cruda).
`coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).

No cubierto: ids de puerta > 255 en la rama 0x32D (ver Hallazgos); la propia validez de `CreateSubTask` y de los demas colaboradores sustituidos (este banco no la prueba); no hay oraculo independiente en Python (la referencia es el EXE par a par); el residuo de EAX/ECX/EDX y la basura de pila no se comparan. El constructor, destructor y `ControlSubTask` de la clase siguen sin hook ni banco (fuera de alcance).

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh LeaveCarNext70                             # compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCarNext70/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/LeaveCarNext70/register_hook.py <carpeta-de-evidencia>       # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecarnext70-runtime.start`; salida `leavecarnext70-runtime.jsonl`.
