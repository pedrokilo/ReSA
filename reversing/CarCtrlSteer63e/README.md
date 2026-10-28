<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlSteer63e: SteerAICarParkParallel y SteerAICarParkPerpendicular (lote 63, CCarCtrl parte B1, tanda 5)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-2`, HEAD df26d6fe7 + fuente de las tandas 1 a 5). 8192 pares (2 x 4096), cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauracion
en ambos lados (incluido `seh_restored`: el EXE monta un marco SEH y lo desmonta) y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, desde el desensamblado); cobertura minima cumplida. Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63e\20261020-210157-044966`.

**Regresion sobre el arbol FINAL** (las seis tandas 63/63b..63f juntas, compiladas y corridas de nuevo el 20/10/2026 21:10..22:30): APROBADO, carpeta de evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63e\20261020-222232-189336`; es la que corresponde al `artifacts/build-manifest.json` de este banco (usar esta para `register_hook.py`).

| Funcion | Direccion | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CCarCtrl::SteerAICarParkParallel(vehicle, steer, accel, brake, handbrake)` | 0x433BA0 | Simple | 4096 | HeadingForTarget 0x433280, CTask::operator new 0x61A5A0, CTaskManager::SetTask 0x681AF0 |
| `CCarCtrl::SteerAICarParkPerpendicular(vehicle, steer, accel, brake, handbrake)` | 0x433EA0 | Simple | 4096 | HeadingForTarget 0x433280, CTask::operator new 0x61A5A0, CTaskManager::SetTask 0x681AF0 |

`cdecl`, `void`, 5 argumentos (el llamador limpia la pila); ambas con marco SEH del compilador en el EXE (el banco comprueba que `fs:[0]` se restaure). NO se sustituyen (solo se apaga su hook, de modo que el
EXE ejecuta su codigo original y la DLL el suyo sobre la misma memoria): `CPathNode::GetPosition` 0x420A10, `CVector::Normalise` 0x59C910, `CCollision::DistToMathematicalLine` 0x412970,
`CTaskComplexLeaveAnyCar::Constructor` 0x421150 y `CTask::CTask` 0x61A3B0 (el EXE construye la tarea de cada pasajero en linea con esa ultima y la del conductor llamando a 0x421150).

## Que hacen (resumen del desensamblado)

Ambas leen los nodos de camino `m_currentAddress` (+0x390) y `m_startingRouteNode` (+0x394) del piloto automatico (`ThePaths.m_pPathNodes[area] + 0x1C * indice`). Si el area de cualquiera de los dos no esta
cargada: mision 0xB y salidas 0 (en el orden freno, acelerador, giro) sin llamar a HeadingForTarget.
- **Parallel (mision 0x2E)**: punto de aparcado = nodo inicial + 1 m en la direccion nodo actual -> nodo inicial (resta 3D, `Normalise`); si no, el nodo inicial. `HeadingForTarget(veh, 0, x, y, ...)`, crucero a `min(crucero, 8)`;
  con mision 0x2E y distancia al punto < 4: mision 0x32 y sale; con otra mision y distancia < 2: aparca.
- **Perpendicular (mision 0x2D)**: si el vehiculo esta a menos de 6 m de la recta nodo inicial - nodo actual (`DistToMathematicalLine`, z = 0): mision 0x31; apunta al nodo actual mientras siga en la 0x2D, si no al inicial;
  crucero a `min(crucero, 8)`; si la distancia al nodo inicial (no al punto de mira) < 2: aparca (ademas `movementFlags |= 2`).
- **Aparcar**: mision 0xB, banderas (+0x428) `&= 0xAF` (motor y luces), y al conductor y a cada pasajero una `CTaskComplexLeaveAnyCar(0, true, false)` en la ranura primaria 3 (`CTask::operator new(0x14)` + `SetTask`;
  si el pool esta agotado la tarea es nula).

## Fuente (CarCtrl.cpp)

`float` puro (con el x87 a 24 bits cada operacion del EXE equivale a la de `float`), nodos leidos con `ThePaths.m_pPathNodes[area] + indice` como el EXE (sin comprobar el indice) y `CPathNode::GetPosition()`.
Las tareas se crean con `CTask::operator new` + `::new(mem) CTaskComplexLeaveAnyCar(0, true, false)` comprobando el nulo (el EXE comprueba el resultado de la reserva) y `SetTask` se llama POR LA DIRECCION del EXE
(0x681AF0): esta definido en linea en la cabecera y con `/O2` se expandiria sin pasar por el hook. `#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"` anadido.

## Revision propia contra el EXE (fase 2)

Se desensamblaron `0x433BA0..0x433E99` y `0x433EA0..0x434187` completos y se contrastaron bloque a bloque: orden de las dos comprobaciones de area, que los nodos `ebp`/`edi` son (inicial, actual) y no al reves, que el
punto de aparcado de Parallel no es el del giro en Perpendicular (usa el nodo inicial), el orden de las operaciones de la normalizacion, la secuencia de tareas (conductor primero, despues los 8 pasajeros en orden, una
reserva por ocupante), y que los campos que escribe la construccion de la tarea son `+0` vptr, `+4` y `+8` a 0 (base), `+0xC` retardo 0, `+0x10` true y `+0x11` false (los bytes `+0x12/+0x13` quedan sin escribir).
Defectos reales hallados por el banco: ninguno (aprobado a la primera; el parche del compilador `operator =` ambiguo en `CVector` fue un error de compilacion, no del banco).

## Registros (EAX/ECX/EDX)

Llamador unico del EXE de cada una, dentro de `SteerAICarWithPhysics_OnlyMission` (0x436A90): ParkParallel 0x43798E y ParkPerpendicular 0x43795A. Tras la llamada solo hacen `add esp,14h; pop edi; pop ebx; pop esi;
pop ebp; add esp,0E4h; ret` en una funcion `void`: **ninguno lee EAX/ECX/EDX**. Solo se registran.

## Banco

Igual estructura que CarCtrlSteer63 (falsos identicos en ambos lados con parche en la entrada del EXE y en el cuerpo de la DLL, arena con guardas, compuertas de ABI, oraculo independiente desde el
desensamblado, espejo del generador). Los nodos de camino son `CPathNode` reales de 0x1C bytes en la arena (posicion int16 en +8/+0xA/+0xC) cuyos punteros por area se escriben en
`CPathFind::m_pPathNodes` (0x96F854, 48 entradas, restauradas tras cada lado); conductor, 8 pasajeros y sus inteligencias son objetos del banco; los 9 bloques de tareas se inicializan con un patron conocido y el banco
compara su contenido final (con la vtable fichada como `VT`, ya que el EXE pone la suya y la DLL otra).

## Suposiciones y limites

- Colaboradores falsos (HeadingForTarget, operator new, SetTask): no se validan sus cuerpos. NaN silenciosos (solo en las salidas del falso de HeadingForTarget).
- Los indices de nodo son 0..3 y las areas 0..47; vehiculo con matriz o sin ella (la posicion sale de la matriz o del placement).
- Pasajeros y conductor con inteligencia siempre no nula (el EXE no la comprueba).

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlSteer63e   # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlSteer63e/fixture_oracle.py       # autoprueba del oraculo
python -I reversing/CarCtrlSteer63e/fixture_mirror.py       # coherencia de las fixtures + cobertura minima (sin el juego)
python -B reversing/CarCtrlSteer63e/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlSteer63e/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
