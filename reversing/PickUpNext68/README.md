<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PickUpNext68: CTaskComplexGoPickUpEntity::CreateNextSubTask 0x691AE0 (virtual, vtable 0x870B98 slot 8, lote 68)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 8ff5230a6 + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pickupnext68\20261026-135718-367375`. (Una corrida previa, `20261026-134847-938375`, encontro 3 diferencias reales de produccion, ver «Hallazgos».)

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CTask* CTaskComplexGoPickUpEntity::CreateNextSubTask(CPed*)` | 0x691AE0 (vtable 0x870B98 + 8*4) | `CTaskComplexGoPickUpEntity` / `CreateNextSubTask` / V (`RH_ScopedVMTInstall`) | `Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador; virtual): `CTaskComplexGoPickUpEntity,CreateNextSubTask,0x00000000,1,0,1,V`.

## Que hace (desensamblado 0x691AE0..0x691D4B, 619 bytes, thiscall `ret 4`, EAX = tarea nueva o nulo)

Sin `m_pEntity` (+0xC) devuelve nulo. Si no, el tipo de la subtarea actual (`GetTaskType`, vtable +0x10 de `m_pSubTask` +8) decide:

1. `0x134` (TASK_SIMPLE_PICKUP_ENTITY): `m_nTimePassed...` (+0x28) = `CTimer::m_snTimeInMilliseconds` (0xB7CB84); si `GetTaskSecondary(4)` (0x681810) no es nulo devuelve nulo; si no `CTask::operator new(0x3C)` (0x61A5A0), `CTaskSimpleHoldEntity(entidad, &m_vecPosition, 6, 1, 0x13E, 0x51, false)` (0x6913A0) y `SetTaskSecondary(tarea, 4)` (0x681B60, tambien con la tarea nula si el pool esta agotado); devuelve nulo.
2. `0x386` (TASK_SIMPLE_ACHIEVE_HEADING): velocidad de mezcla 0.6f (0x870A80). Con `m_nAnimGroupId == 0x51`: `top = GetColModel()->min.z + pos.z` de la entidad (0x535300, suma x87 de 24 bits **no guardada como float**); `top > ped.z` -> grupo 0x53 y mezcla 0x870A88 (0.2f); si no, `ped.z < 0.55f + top` (0x866CAC) -> grupo 0x52 y mezcla 0x870A84 (0x3E888889). `CTask::operator new(0x4C)` y `CTaskSimplePickUpEntity(entidad, &m_vecPosition, 6, 1, 0x13D, grupo, mezcla)` (0x6917B0); copia `m_vecPickupPosition` a +0x40 de la tarea nueva; devuelve la tarea.
3. `0x387` (TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL): `dy = ent.y - ped.y`, `dx = ent.x - ped.x` (fsub + fstp float), `CTask::operator new(0x18)` y `CTaskSimpleAchieveHeading(fpatan(-dx, dy), 1.0f, 0.001f)` (0x667E20); devuelve la tarea (o nulo).
4. Cualquier otro tipo: nulo.

## Cambios de produccion (`source.patch`)

`TaskComplexGoPickUpEntity.cpp` solo: `RH_ScopedVMTInstall(CreateNextSubTask, 0x691AE0)`, el cuerpo en C++ puro (sin `__asm`/`naked`), includes y `X87Round24` (redondeo a 24 bits por enteros). El `.h` no cambia.
`dx`/`dy` usan `AutoPreSubCpp` (X87Math, ya existente) y el angulo `TrigAtan2Cpp(-dx, dy)`.

## Hallazgos

- **Defecto real de produccion (corregido)**: la primera version comparaba `colMin.z + ent.z` con `float` de SSE; con MXCSR FTZ (el estado del juego) una suma desnormal (0 + 1.4e-45) se vaciaba a 0 y la rama `top > ped.z` cambiaba (grupo 0x52 en vez de 0x53): 3 de 16384 pares. La x87 conserva el desnormal y no guarda la suma como float (exponente extendido, desbordes finitos). Corregido con suma en `double` + `X87Round24` (enteros) y `AutoPreSubCpp` para `dx`/`dy`. Las fixtures incluyen ahora diferencias desnormales dominantes, sumas que desbordan el float, infinitos y NaN canonicos (un NaN por resta).
- **Diferencia deliberada**: si `CTask::operator new` devuelve nulo en la rama `0x386`, el EXE escribe `m_vecPickupPosition` en la direccion 0x40 (nulo + 0x40) y se cae; el C++ devuelve nulo. Esa combinacion queda fuera del banco (el EXE no la sobrevive). En las ramas `0x387` y `0x134` la comprobacion de nulo reproduce al EXE (devuelve nulo / pasa nulo a `SetTaskSecondary`) y esta cubierta (`goto_alloc_null` 1078 casos, `hold_alloc_null` 1093).

## Banco

`Original` = bytes del EXE con el hook V apagado (entrada 0x691AE0 y ranura de la vtable del EXE intactas); `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI y la ranura 0x870BB8 apunta a el; comprobado en cada lado, `route_ok`). Se llama a la entrada del EXE con ECX = tarea.
Colaboradores (6) sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos EN ORDEN: `CTask::operator new` 0x61A5A0, `CEntity::GetColModel` 0x535300, `CTaskManager::GetTaskSecondary` 0x681810 y `SetTaskSecondary` 0x681B60 y los constructores de `CTaskSimpleHoldEntity` 0x6913A0 y `CTaskSimplePickUpEntity` 0x6917B0 (su cuerpo en la DLL se obtiene de la primera llamada de la envoltura `Constructor-1/-2` del hook, como en Helpers66D). `GetTaskType` de la subtarea es una vtable falsa. El constructor de `CTaskSimpleAchieveHeading` (0x667E20) no tiene hook: corre real en ambos lados y se compara el objeto de 0x18 bytes (tipo de tarea en lugar del puntero a la vtable).
Se compara EAX (etiquetado), los cambios en 12 arenas (tarea, subtarea, entidad, peaton, inteligencia, matrices, modelo de colision, tarea secundaria y los tres bloques de `operator new`), la traza de eventos y el hash de globales (`CTimer` se restaura); ECX/EDX solo se registran (el unico llamador virtual lee EAX).
16384 pares = 16 familias de 1024 (sin entidad, tipos ajenos, 0x134 con/sin secundaria y con pool agotado, 0x387 normal/desnormal/especial/pool agotado, 0x386 con grupo distinto de 0x51 y los tres umbrales de z, valores especiales y mezcla).
`coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).

No cubierto: la rama `0x386` con pool agotado (el EXE se cae); no hay oraculo independiente en Python (la referencia es el EXE par a par); la propia validez de los constructores de Hold/PickUp y de los colaboradores sustituidos no la prueba este banco.

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh PickUpNext68                               # compila la sonda y corre en el juego (candado global)
python -B reversing/PickUpNext68/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/PickUpNext68/register_hook.py <carpeta-de-evidencia>         # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `pickupnext68-runtime.start`; salida `pickupnext68-runtime.jsonl`.
