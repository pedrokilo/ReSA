<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PickUpFirst68: CTaskComplexGoPickUpEntity::CreateFirstSubTask (0x693610), virtual de subtareas de la tarea de recoger un objeto (lote 68)

Estado: **VALIDADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-2`, HEAD 8ff5230a6 de main + `source.patch`): 16384 pares, cero diferencias (carpeta de evidencia al final).
Plantillas: `reversing/PartnerNext60` (hook Virtual, falsos de `operator new`, constructor real en arena), `reversing/LeaveCar67` y `reversing/Helpers66A` (ABI, falsos con parche de entrada en el EXE y en la DLL).

| Funcion | Direccion | Hook RH (clase / nombre) | Tipo | Fuente |
|---|---|---|---|---|
| `CTask* CTaskComplexGoPickUpEntity::CreateFirstSubTask(CPed*)` (vtable 0x870B98, ranura 9) | 0x693610..0x6939CB | `CTaskComplexGoPickUpEntity` / `CreateFirstSubTask` | V | `TaskComplexGoPickUpEntity.cpp` (+ ayudantes en `X87Math.cpp/.h`) |

Fila de `docs/hooks.csv` que debe anadir el coordinador (hook virtual: direccion 0x00000000 y tipo V; la direccion real va en `evidence.original_address` del registro):
`CTaskComplexGoPickUpEntity,CreateFirstSubTask,0x00000000,1,0,1,V`

## Que hace (desensamblado, `ret 4`, marco SEH propio, EAX = tarea creada o 0)

Unico llamador relevante: `CTaskManager::AddSubTasks` (0x681A53: `call [edx+24h]`, luego `mov edi,eax`); sobrescribe ECX y EDX antes de usarlos, asi que solo EAX se compara (ECX/EDX se registran).

1. Sin entidad (`[this+0xC]`) devuelve 0 sin escribir nada. Si no: `[this+0x28]` = `CTimer::m_snTimeInMilliseconds` (0xB7CB84).
2. `d = ped - entidad` (posicion de la matriz +0x30 si la hay, si no la del placement +4), cada resta a 24 bits y guardada como float. Caja de colision = `ms_modelInfoPtrs[movsx(word [ent+0x22])]->[+0x14]` (min +0, max +0xC);
   `[this+0x10..0x18]` = (-0.2f [0x870A74], -min.y - 0.2f [0x870A78], -min.z).
3. Rumbo: `2*max.y < max.x` -> `SetHeading(atan2(-dx, dy))`; si no `2*max.x < max.y` -> `SetHeading(atan2(-dx, dy) - 1.5707964f [0x858FE4])` (fpatan menos pi/2 con un solo redondeo); si no `GetMatrix` y, si `up.z < 0.9f` [0x858C20],
   `SetHeading(GetHeading())`. SetHeading en linea: con matriz `CMatrix::SetRotateZOnly` (0x59B020), sin matriz `[ent+0x10]` = rumbo.
4. Tipo `[ent+0x36] & 7` == 4 (objeto): `[ent+0x44..0x4C]` y `[ent+0x50..0x58]` = 0 y `SetIsStatic(true)` (vtable +0x10). Con objeto RW (`[ent+0x18]`): `CMatrix::UpdateRwMatrix` (0x59AD70) si hay matriz o
   `CSimpleTransform::UpdateRwMatrix` (0x54EF40), sobre `[[ent+0x18]+4]+0x10`.
5. Punto de recogida: derecha = `(dz*r.z + dy*r.y) + dx*r.x` (guardada como float), frente = `(dz*f.z + dy*f.y) + dx*f.x` (en registro, 24 bits y exponente extendido), `a = |frente|`; la matriz se pide antes de cada producto
   (`AllocateMatrix` 0x54F560 + `UpdateMatrix` 0x54F1B0 si falta). derecha > a -> (0.4f + max.x, 0); -a > derecha -> (min.x - 0.4f, 0); si no (0, frente > 0.0f ? 0.4f + max.y : min.y - 0.4f); z = min.z + 1.0f.
6. `matriz * pickup` (0x59C890), `CTask::operator new(0x28)` (0x61A5A0) y, si no es nulo, `CTaskComplexGoToPointAndStandStill(4, punto, 0.2f, 0.0f, false, true)` (0x668120); si es nulo devuelve 0.

## Fuente (`source.patch`)

- `TaskComplexGoPickUpEntity.cpp`: `RH_ScopedVMTInstall(CreateFirstSubTask, 0x693610)` y el cuerpo en C++ (sin `__asm`/`naked`, sin llamadas al EXE por direccion). `CTask::operator new` con comprobacion de nulo (como `Clone`).
  `SetHeading`/`GetHeading`/`UpdateRwMatrix`/`GetMatrix` son funciones de la DLL (el EXE las lleva en linea; hacen las mismas llamadas a colaboradores, en el mismo orden). El `.h` no cambia.
- `X87Math.cpp/.h` (anadido al FINAL): `GoPickUpDeltaCpp`, `GoPickUpDoubleLessCpp`, `GoPickUpNegMinusCpp` y `GoPickUpRegionCpp`. **Defecto real hallado por el banco** (primera corrida: 19/16384 diferencias, todas en la familia de
  valores diminutos): con float de SSE (y MXCSR con FTZ) un producto o una suma subnormales dan 0, mientras que el EXE, con la x87 a 24 bits de mantisa y exponente extendido, conserva un valor distinto de cero
  y la comparacion `frente > 0.0f` / `derecha > |frente|` sale al reves. Los ayudantes se compilan con `/arch:IA32`; en `GoPickUpRegionCpp` el frente y su valor absoluto van en `double` (sin perdida frente al registro de la x87,
  y evita que un `float` los redondee); se comprobo el codigo generado (misma secuencia `fld/fmul/faddp`, `fld st0; fabs; fcomp`, sin `fstp` intermedio del frente).
- Asuncion no cubierta por el banco: el indice del modelo se extiende con signo (`(int16)`, como `movsx` del EXE) pero las fixtures usan solo indices 0..19999 (un indice negativo lee memoria anterior a `ms_modelInfoPtrs`).

## Banco

`Original` = bytes del EXE con el hook RH de la funcion apagado; `candidato` = DLL con SOLO ese hook encendido (`HookRoute` con el cuerpo del hook Virtual). 16384 pares = 16 familias x 1024
(sin entidad; rumbo A/B/C con y sin matriz; bordes de la caja alrededor de `2*max.y = max.x`, ceros, NaN, inf; ped a la derecha / izquierda / delante / detras de la entidad; NaN, inf, enormes, minimos normales, subnormales y diminutos
por debajo del menor float; `operator new` nulo; mezcla al azar).

- **Sustituidos por el mismo falso en la direccion del EXE y en el cuerpo de la DLL** (cuerpos resueltos antes de parchear; registran sus argumentos EN ORDEN): sin hook en HEAD (la entrada del EXE debe seguir siendo codigo original):
  `CMatrix::SetRotateZOnly` 0x59B020, `CMatrix::UpdateRwMatrix` 0x59AD70, `CSimpleTransform::UpdateRwMatrix` 0x54EF40, `CSimpleTransform::UpdateMatrix` 0x54F1B0; con hook en HEAD (deben ser saltos a la DLL en ambos lados):
  `CPlaceable::AllocateMatrix` 0x54F560 (deja una matriz de arena en `[ent+0x14]`), `CMatrix * CVector` 0x59C890 (devuelve el punto del caso) y `CTask::operator new` 0x61A5A0 (devuelve una arena o nulo).
- **Real en ambos lados**: el constructor `CTaskComplexGoToPointAndStandStill` 0x668120 (salto a la DLL en ambos lados; construye en la arena del falso de `operator new` y se comparan sus palabras).
- `SetIsStatic` va por la vtable falsa de la entidad (ranura 4; las demas ranuras registran un evento `trap`). `CTimer::m_snTimeInMilliseconds` y la entrada de `ms_modelInfoPtrs` del caso se fijan por caso y se restauran.
- Observado por lado: retorno (etiqueta), eventos ordenados con argumentos, bytes cambiados de la tarea y de la entidad (campos escritos con valores centinela), puntero de matriz de la entidad (etiqueta), arenas restantes intactas,
  palabras del objeto construido, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, `collaborators_dll`, estado y tiempo restaurados. ECX/EDX de salida solo se registran.
- `run_local.py` exige ademas que la salida de CADA lado coincida con `fixture_oracle.py` (modelo escrito solo desde el desensamblado: x87 a 24 bits con exponente extendido, comparaciones ordenadas, rumbo con tolerancia de 2 ulp por fpatan de 53 contra 64 bits),
  la cobertura de ramas (`coverage.json` = minimos, ~50 % de lo medido) y la restauracion final.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh PickUpFirst68   # compila la sonda y corre en el juego (candado global)
python -B reversing/PickUpFirst68/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/PickUpFirst68/fixture_oracle.py                                          # autoprueba del oraculo, sin juego
python -B reversing/PickUpFirst68/register_hook.py <carpeta-de-evidencia>                    # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `pickupfirst68-runtime.start`; salida `pickupfirst68-runtime.jsonl`.
