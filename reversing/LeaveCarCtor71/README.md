<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCarCtor71: constructor y destructor de CTaskComplexLeaveCar y constructor de CTaskComplexLeaveCarAndDie, lote 71

Estado: **APROBADO en el juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 1b2235ec3 + `source.patch`): 3 x 16384 = 49152 pares, 0 diferencias, todas las compuertas en ambos lados.
Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leavecarctor71\20261026-181039-544291`.

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle*, int32, int32, bool, bool)` | 0x63B8C0 | `CTaskComplexLeaveCar` / `Constructor` / S | `Tasks/TaskTypes/TaskComplexLeaveCar.cpp/.h` |
| `CTaskComplexLeaveCar::~CTaskComplexLeaveCar()` | 0x63B970 | `CTaskComplexLeaveCar` / `Destructor` / S | `Tasks/TaskTypes/TaskComplexLeaveCar.cpp/.h` |
| `CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie(CVehicle*, int32, int32)` | 0x62F1A0 | `CTaskComplexLeaveCarAndDie` / `Constructor` / S | `Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp/.h` |

Filas de `docs/hooks.csv` (las anade el coordinador):
```
CTaskComplexLeaveCar,Constructor,0x0063B8C0,1,0,1,S
CTaskComplexLeaveCar,Destructor,0x0063B970,1,0,1,S
CTaskComplexLeaveCarAndDie,Constructor,0x0062F1A0,1,0,1,S
```
(`ControlSubTask` esta en el banco hermano `LeaveCarCore71`.)

## Hallazgo sobre el «constructor secundario» 0x62F1A0

0x62F1A0 **no es de `CTaskComplexLeaveCar`**: es el constructor de `CTaskComplexLeaveCarAndDie` (la vtable que escribe es 0x86DD00, la de `CTaskComplexLeaveCarAndDie`; el 0x86E828 de `CTaskComplexLeaveCar` solo lo escribe 0x63B8C0).
Desensamblado: llama a 0x63B8C0 (`call`, no en linea) con `(vehiculo, puerta, retardo, sensible = 0, forzar = 1)`, pone `[this] = 0x86DD00` y `m_bDie` (+0x1A) = 1 y devuelve `this` (`ret 0Ch`). El C++ ya existia en
`TaskComplexLeaveCarAndDie.cpp` (`: CTaskComplexLeaveCar{veh, door, delay, false, true}` + `m_bDie = true`) pero sin hook; ahora lleva `RH_ScopedInstall(Constructor, 0x62F1A0)` con la envoltura `Constructor` en el `.h`.
El constructor de tres argumentos de `CTaskComplexLeaveCar` (marcado `[[deprecated]]`, con el comentario «0x62F1A0») no tiene equivalente en el EXE; solo cambio el comentario a «NOTSA» y no se hookea.
Los tres llamadores de 0x62F1A0 (0x62FDC0, 0x63F5AB, 0x63F7B8) solo leen EAX.

## Destructor con borrado (ranura 0)

El «deleting destructor» de la ranura 0 de la vtable 0x86E828 es 0x6410E0: `push esi; mov esi,ecx; call 0x63B970; test byte ptr [esp+8],1; je ...; push esi; call 0x61A5B0 (CTask::operator delete); ...; ret 4`.
Solo llama a 0x63B970 (hookeado) y a `CTask::operator delete` (hookeado), asi que **no necesita hook propio**. El de `CTaskComplexLeaveCarAndDie` (0x62F250) hace lo mismo con 0x62F270, que escribe la vtable 0x86DD00 y salta (`jmp`) a 0x63B970.

## Desensamblado verificado

- **0x63B8C0** (marco SEH `push -1 / push 0x841618 / fs:[0]`, `ret 14h`): `call 0x61A3B0` (CTaskComplex: vtable 0x86D4CC, +4 = 0, +8 = 0); `[+0xC] = vehiculo`, `[+0x10] = puerta`, `[+0x14] = retardo`, `[+0x18] = byte sensible`, `[+0x19] = byte forzar`,
  `[+0x1A] = 0`, `[+0x1C] = 0`, `[+0x20] = 0`, `[+0x21] = 0`, `[+0x24] = 0xF` (`ANIM_ID_KO_SHOT_FRONT_0`), `[+0x28] = 4.0f`, `[+0x2C] = 1.0f`, `[+0x30] = 0`, `[this] = 0x86E828`; si el vehiculo no es nulo, `CEntity::RegisterReference(vehiculo, &this->+0xC)` (0x571B70). No escribe
  el relleno (+0x1B, +0x22, +0x23, +0x31..+0x33). Salida: EAX = this; ECX = cabeza SEH anterior; EDX = this+0xC (o lo que deje RegisterReference). Los 55 llamadores (`call`) solo leen EAX (el resto lo reescriben).
- **0x63B970** (marco SEH, `ret`): `[this] = 0x86E828`; si `m_pTargetVehicle` (+0xC): `ClearGettingOutFlags(vehiculo, (uint8)[+0x20])` (0x6D3060, argumento dword con el byte cero-extendido), `vehiculo->+0x485 -= (uint8)[+0x21]`, `CleanUpOldReference(vehiculo, &this->+0xC)` (0x571A00; el vehiculo se
  vuelve a leer de +0xC en cada paso); si la utilidad (+0x1C) no es nula: 0x64FC00 (es solo un `ret`) y `operator delete` (0x8214BD); `call 0x61A3D0` (~CTaskComplex: vtable 0x86D4CC, destructor con borrado de la subtarea por la vtable si no es nula, +8 = 0, vtable 0x86D48C). Los llamadores
  (el destructor con borrado, los `funclets` de desenrollado de SEH y el destructor de `CTaskComplexLeaveCarAndDie`) no leen EAX, ECX ni EDX.

## Fuente (`source.patch`, acumulado del lote 71: tambien contiene el cambio de `ControlSubTask` del banco LeaveCarCore71)

La fuente de las tres funciones ya era fiel al EXE; no hubo que corregir nada en ellas. Cambios: `TaskComplexLeaveCar.h` (envolturas `Constructor(...)` y `Destructor()` con el patron `this->T::T(...)` / `this->T::~T()` de otras clases; la declaracion `Constructor` que ya existia no tenia definicion),
`TaskComplexLeaveCar.cpp` (`RH_ScopedInstall(Constructor, 0x63B8C0)`, `RH_ScopedInstall(Destructor, 0x63B970)`, comentario del constructor de tres argumentos), `TaskComplexLeaveCarAndDie.h/.cpp` (envoltura y `RH_ScopedInstall(Constructor, 0x62F1A0)`).
Sin `__asm`/`naked`; ningun archivo nuevo en `source/game_sa`. Los constructores no tienen `{}` ni `new T()`: el relleno queda como estaba (verificado byte a byte).

## Banco

`Original` = bytes del EXE con el hook S de la funcion apagado; `candidato` = DLL con SOLO ese hook encendido (la entrada salta a la envoltura del ASI; comprobado en cada lado, `route_ok`). En 0x62F1A0 el hook de 0x63B8C0 esta apagado en ambos lados, asi que el original es EXE puro y el candidato DLL pura.
Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos EN ORDEN: `CEntity::RegisterReference` 0x571B70 (constructores), `CVehicle::ClearGettingOutFlags` 0x6D3060, `CEntity::CleanUpOldReference` 0x571A00 y `operator delete` 0x8214BD (destructor; el ultimo parche).
Corren reales: el constructor de `CTaskComplex` (0x61A3B0), `~CTaskComplex` (0x61A3D0; en el EXE con su hook de la DLL, en la DLL en linea) y `~CTask`. En el destructor la subtarea es un objeto con vtable falsa cuya ranura 0 (destructor con borrado) registra `(subtarea, indicador = 1)`.
- Constructores: el objeto (0x34 bytes) se construye sobre una arena con guardas de 0x40 bytes a cada lado, a 8 desplazamientos (0..7) y con 8 rellenos (4 ruidos, 0x00, 0xFF, 0xCD, 0xA5), y se compara **byte a byte todo el cambio de la arena** (relleno y guardas incluidos), la palabra de la vtable aparte (se compara `GetTaskType` por la vtable que quedo escrita: 0x2C0 y 0x2C1),
  EAX = this, los eventos (`RegisterReference(vehiculo, &this->+0xC)` solo si el vehiculo no es nulo), la arena del vehiculo (no debe cambiar) y las compuertas de ABI. Vehiculo: nulo, de arena, o crudo (1, 0xFFFFFFFF, 0x80000000, 0xCDCDCDCD...; nunca se desreferencia). Puertas y retardos extremos; los booleanos de la pila con 0/1 y con bytes altos basura (el EXE solo lee el byte bajo).
- Destructor: objeto ya construido al azar (vtable del EXE o ruido, subtarea nula o con vtable falsa, vehiculo nulo o de arena con relleno de ruido o 0xFF, utilidad nula / de arena / puntero crudo, banderas y contador 0, 0xFF o al azar). Se compara todo el cambio en las arenas (objeto sin la vtable, vehiculo, utilidad y subtarea: el byte +0x485 del vehiculo baja `m_nNumGettingInSet`), la traza de eventos
  (orden `ClearGettingOutFlags` -> `CleanUpOldReference` -> `operator delete` de la utilidad -> destructor de la subtarea) y las compuertas de ABI. Que la vtable cambie se registra (`vptr_changed`); su valor final no se compara (EXE: 0x86D48C; DLL: la de `CTask` de la DLL).
- ECX y EDX (y EAX del destructor) no se comparan: el EXE deja la cabeza SEH anterior en ECX y EDX segun RegisterReference, y ningun llamador los lee (los 55 llamadores de 0x63B8C0 y los 3 de 0x62F1A0 unen el camino del constructor con el de `operator new` nulo, `xor eax,eax`, que no deja nada util en ECX/EDX; se desensamblaron los puntos de union visibles y los llamadores del destructor).
16384 pares por funcion = 16 familias de 1024 (ver `Fixtures.hpp`). `coverage.json` = minimos de ramas (50 % de la primera corrida aprobada).

No cubierto: booleanos de la pila con el byte bajo distinto de 0/1 (el EXE los guarda tal cual; ningun llamador lo hace); la vtable final del objeto en el destructor; el comportamiento de `RegisterReference`/`CleanUpOldReference`/`ClearGettingOutFlags` (sustituidos; los prueban sus bancos); `~CTaskComplex` fuera del marco de esta clase;
vehiculo crudo en el destructor (el EXE escribe en `+0x485`).

## Comandos

```
bash reversing/_handoff/bench_here.sh LeaveCarCtor71                             # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1; compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCarCtor71/run_local.py --analyze <carpeta>/comparisons.jsonl
python -B reversing/LeaveCarCtor71/register_hook.py <carpeta-de-evidencia>       # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecarctor71-runtime.start`; salida `leavecarctor71-runtime.jsonl`.
