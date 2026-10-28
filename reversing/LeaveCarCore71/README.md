<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCarCore71: CTaskComplexLeaveCar::ControlSubTask (0x6421B0), lote 71

Estado: **APROBADO en el juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 1b2235ec3 + `source.patch`): 16384 pares, 0 diferencias, todas las compuertas en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leavecarcore71\20261026-181708-869919`.
Control negativo (el banco detecta el defecto corregido): `...esultados-leavecarcore7161026-181543-890770`, con el cuerpo anterior de `ControlSubTask` restaurado temporalmente: FALLO, 11774/16384 pares con diferencias. (La primera corrida aprobada, `...-180618-199495`, uso la misma fuente final.)

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CTask* CTaskComplexLeaveCar::ControlSubTask(CPed*)` | 0x6421B0 (vtable 0x86E828 + 10*4 = 0x86E850) | `CTaskComplexLeaveCar` / `ControlSubTask` / V (`RH_ScopedVMTInstall`) | `Tasks/TaskTypes/TaskComplexLeaveCar.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador; virtual): `CTaskComplexLeaveCar,ControlSubTask,0x00000000,1,0,1,V`.
El constructor, el destructor y el constructor de `CTaskComplexLeaveCarAndDie` van en el banco hermano `LeaveCarCtor71`. `MakeAbortable`, `CreateNextSubTask`, `CreateFirstSubTask` y `CreateSubTask` no se tocan.
La vtable de `CTaskComplexLeaveCarAndDie` (0x86DD00) comparte esta funcion en la ranura 10 (0x86DD28 -> 0x6421B0): el hook V solo cambia la ranura de la clase base, pero tambien salta la entrada 0x6421B0, asi que las instancias de `CTaskComplexLeaveCarAndDie` creadas por el EXE tambien pasan por la DLL; el banco prueba la ranura de `CTaskComplexLeaveCar`.

## Que hace (desensamblado 0x6421B0..0x642246, thiscall `ret 4`, EAX = subtarea o nulo)

1. `m_pTargetVehicle` (+0xC) nulo -> devuelve nulo.
2. Si el ped NO esta en el vehiculo (`bInVehicle`, bit 8 de +0x46C) y el tipo de la subtarea (`GetTaskType`, vtable +0x10, **una llamada nueva por comprobacion**) no es 0x330 (SET_PED_OUT), 0x32E (JUMP_OUT), 0xCE (GET_UP_AND_STAND_STILL) ni 0xD4 (DIE): devuelve `CreateSubTask(0x330, ped)` (0x641530).
3. Si `m_bSensibleLeaveCar` (+0x18) es 0 y el tipo (otra llamada a `GetTaskType`) es 0x329 (WAIT_TO_SLOW_DOWN): `m_pSubTask->MakeAbortable(ped, 0, nullptr)` (vtable +0x18; prioridad 0 = LEISURE; resultado ignorado).
4. Devuelve `m_pSubTask`.

## Cambios de produccion (`source.patch`, acumulado del lote 71: tambien contiene los del banco LeaveCarCtor71)

`TaskComplexLeaveCar.cpp`: `RH_ScopedVMTInstall(ControlSubTask, 0x6421B0)` y el cuerpo. Defecto real encontrado al verificar contra el desensamblado: la version anterior pedia el tipo a la subtarea UNA vez (`const auto subTaskType`)
y lo reutilizaba; el EXE llama a `GetTaskType` hasta cinco veces (una por comparacion, y ninguna si el ped esta en el vehiculo). El comportamiento visible solo difiere si `GetTaskType` no es puro, pero el numero y el orden
de llamadas a la subtarea es observable (y lo comparan los bancos). Ahora cada comprobacion llama de nuevo, como el EXE.

## Banco

`Original` = bytes del EXE con el hook V apagado (entrada 0x6421B0 y ranura 0x86E850 intactas); `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI y la ranura apunta a el; comprobado en cada lado, `route_ok`).
Se llama a la entrada del EXE con ECX = tarea y el ped por la pila (`ret 4`). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos EN ORDEN:
`CreateSubTask` 0x641530 (con hook en este arbol: se parchea el cuerpo del hook; `run_local.py` acepta ambos casos) y la vtable falsa de la subtarea: `GetTaskType` (ranura 4) devuelve el tipo que el caso asigna a ESA llamada
(secuencia de hasta 6 tipos), `MakeAbortable` (ranura 6) registra (subtarea, ped, prioridad, evento) y devuelve un valor del caso; las demas ranuras son trampas (no deben llamarse).
Se compara EAX (etiquetado por arena), los cambios en 5 arenas (tarea, subtarea, ped, vehiculo, auxiliar: ninguna debe cambiar), la traza de eventos, el hash de globales (no hay regiones globales) y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP,
DF, SEH, x87 normalizado, ruta, hooks y entradas vigiladas restaurados). ECX/EDX solo se registran (el unico llamador es la llamada virtual de `CTaskComplex`, que lee EAX).
16384 pares = 16 familias de 1024: sin vehiculo, dentro y sensata, dentro y no sensata con/sin 0x329, fuera con el tipo clave en la 1a/2a/3a/4a comprobacion, ninguno de los cuatro con `CreateSubTask` normal y nulo, secuencias donde un tipo guardado
difiere, palabras +0x46C/+0x470 con todos los bits (solo cuenta el bit 8), vehiculo crudo (1, 0xFFFFFFFF, 0x80000000, 0xCDCDCDCD...) que no se desreferencia.
`coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).

## Hallazgos

- Defecto de produccion (ver arriba): cinco llamadas a `GetTaskType` frente a una. Corregido antes de la primera corrida.
- Control negativo: con la version anterior del cuerpo (tipo guardado una vez) el banco FALLA en 11774/16384 pares (corrida `20261026-181543-890770`); con la fuente final pasa 16384/16384 (corridas `20261026-180618-199495` y `20261026-181708-869919`, esta ultima es la registrable: su manifiesto coincide con la fuente actual).

No cubierto: `m_pSubTask` nulo (el EXE falla igual: no es una entrada valida); el residuo de EAX/ECX/EDX y la basura de pila no se comparan; la validez de `CreateSubTask` y de los metodos virtuales de la subtarea (este banco no los prueba).

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh LeaveCarCore71                             # compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCarCore71/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/LeaveCarCore71/register_hook.py <carpeta-de-evidencia>       # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecarcore71-runtime.start`; salida `leavecarcore71-runtime.jsonl`.
