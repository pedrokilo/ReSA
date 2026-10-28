<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FollowFootsteps62: CTaskComplexFollowPedFootsteps::{ControlSubTask 0x695090, CreateSubTask 0x695E40, CreateNextSubTask 0x694EE0, CreateFirstSubTask 0x695000, MakeAbortable 0x694ED0}

**Estado: APROBADO** (20480 pares = 5 funciones x 4096, cero diferencias; worktree `D:\Original\we-2`, HEAD `0ac292a76` + el fuente de esta carpeta). Carpeta de evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-followfootsteps62\20261020-005510-521803`. Plantillas: `reversing/PartnerControl60` (hook Virtual, vtables falsas), `reversing/PartnerNext60` (constructores reales, `operator new` falso) y contrato `reversing/_handoff/BRIEF.md`.

## Comandos

```
bash reversing/_handoff/bench_here.sh FollowFootsteps62                         # compila la sonda y corre el banco en el juego (candado global de GTA)
python -B reversing/FollowFootsteps62/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aun no existe o no esta completo
python -I reversing/FollowFootsteps62/fixture_oracle.py                         # autoprueba del oraculo: «ok»
python -B reversing/FollowFootsteps62/fixture_mirror.py                         # compila Fixtures.hpp con cl: arenas/constantes C++ == oraculo + cobertura
python -B reversing/FollowFootsteps62/register_hook.py <carpetaEvidencia>       # SOLO lo ejecuta el coordinador (SCOPE_NOTE ya rellenado)
```
Requiere el ASI con las cinco `RH_ScopedVMTInstall` en `CTaskComplexFollowPedFootsteps::InjectHooks` y la entrada por archivo de `source/CMakeLists.txt` (ver abajo). Archivo de arranque en el clon: `followfootsteps62-runtime.start`; salida `followfootsteps62-runtime.jsonl`.
Filas de `docs/hooks.csv` (las anade el coordinador): `CTaskComplexFollowPedFootsteps,<funcion>,0x00000000,1,0,1,V` para ControlSubTask (original_address 0x00695090), CreateSubTask (0x00695E40), CreateNextSubTask (0x00694EE0), CreateFirstSubTask (0x00695000) y MakeAbortable (0x00694ED0).

## Que hace cada funcion (desensamblado completo en la cabecera de `fixture_oracle.py`)

- **MakeAbortable 0x694ED0**: `mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]`: delega en la ranura 6 de la subtarea con los mismos argumentos (sin comprobar nulo). Se compara el AL devuelto (los llamadores hacen `test al,al`).
- **CreateFirstSubTask 0x695000**: sin objetivo -> `CreateSubTask(FINISHED 0x516)`; si no, `s = (dx² + dz²) + dy²` (objetivo - peaton, sin guardar a memoria) y `s > 1.4f` -> SEEK (0x38B), si no STAND_STILL (0xCB), por la ranura 11 de `this`.
- **CreateNextSubTask 0x694EE0**: sin objetivo -> FINISHED; `GetTaskType()` de la subtarea (con signo): NONE(0xC8) -> FINISHED; STAND_STILL (0xCB) o GO_TO_POINT (0x384) -> GO_TO_POINT si hay ruta con puntos, si no STAND_STILL; HIT_HEAD (0x1F4) -> STAND_STILL; SEEK (0x38B) -> como First pero `(dz² + dx²) + dy²`; cualquier otro -> FINISHED.
- **CreateSubTask 0x695E40**: sin objetivo o ruta -> 0; GO_TO_POINT: copia el primer punto, lo quita (`0x5F13A0`), `new CTaskSimpleGoToPoint((int16)m_moveState, pt, 0.01f, 0, 0)`; STAND_STILL `(10000, 0, 0, 8.0f)`; HIT_HEAD; SEEK `CTaskComplexSeekEntityStandard(objetivo, 50000, 1000, bInVehicle ? 4.0 : 1.0, 2.0, 2.0, 1, 1)`; FINISHED y cualquier otro tipo -> 0. Si `operator new` devuelve nulo, 0 (la ruta ya se modifico).
- **ControlSubTask 0x695090**: ver el flujo comentado en `fixture_oracle.py`: FINISHED si falta objetivo/ruta o esta muerto; SEEK si la subtarea ya es SEEK (devuelve la subtarea), si el objetivo va en vehiculo, si `|D|² > 64` o si `dist2D < 1 && |dz| > 2`; HIT_HEAD si el contador de la inteligencia del peaton (`+0x274`) > 30; temporizador de linea de vision (> 500 ms; bit 0 de `m_updateGoToPoint` = `AL & 1`); muy cerca (`|D|² < 1.4f²`) vacia la ruta y pasa a STAND_STILL; indicador alzado: ruta = [objetivo], `UpdatePoint` de la tarea GoTo activa; temporizador de 166 ms / ruta llena; paso de ruta con eliminacion del ultimo punto si `cos >= 0.95`; estado de movimiento RUN/WALK por la longitud acumulada (`> 3.5²`), escrito tambien en la subtarea GoToPoint. Para cambiar de subtarea: `MakeAbortable(ped, URGENT, 0)` y `CreateSubTask(nuevo, ped)`.

## Fuente (`source.patch`: SOLO estos cambios respecto a HEAD, `git apply --check` contra los blobs de HEAD ok)

- `source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp`: las cinco `RH_ScopedVMTInstall`, los cuerpos en C++ (sin `plugin::Call*`), constantes `kNearSq/kFarSq/kStepSq/kRunSq`, ayudantes de suma de cuadrados con orden del EXE (`FootstepsSquaredMagnitude`, `FootstepsDistSqZXY`, `FootstepsDistSqXZY`, `FootstepsDot`, `FootstepsDist2D`, con `#pragma float_control(precise, on)`), `FootstepsRouteRemove` (= 0x5F13A0) y el acceso a `GetSimplestActiveTask` por puntero a miembro volatil.
- `source/CMakeLists.txt`: entrada por archivo `game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp` con `/O2 /Oi /arch:IA32 /fp:fast /Zi`, sin PCH ni unity y `BasicRuntimeChecks=Default` (x87 con redondeo a 24 bits como el EXE; mismo patron que `ConversationForPed.cpp`). **Efecto colateral:** el constructor 0x694E20 y el destructor de la misma clase pasan a compilarse con esas opciones (ver «Revalidacion del constructor»).

## Fase 2: revision contra el EXE y discrepancias (borrador del lote 27 -> fuente actual)

Re-desensamblado 0x694ED0..0x696025 y contrastado instruccion a instruccion con el C++ y con el codigo generado (`dumpbin /disasm` del `.obj` real). Lo encontrado y corregido:
1. **Constantes**: el borrador escribia `(double)1.4f * 1.4f`; el EXE hace `fld/fmul` de floats con la palabra de control de 24 bits, es decir el producto redondeado a float. Ahora `constexpr float kNearSq = 1.4f * 1.4f` etc. (comprobado en el `.obj`: `3ffae147`, `3dfae147`, `42800000`, `41440000`).
2. **Asociacion de las sumas**: con `/fp:fast` el compilador puede reasociar `x*x + y*y + z*z`; las cinco sumas viven en funciones `noinline` con `float_control(precise, on)` y el orden del EXE ((x²+y²)+z², (z²+x²)+y², (x²+z²)+y², producto escalar (wy·vy + wx·vx) + wz·vz).
3. **Llamadas a `GetSimplestActiveTask` (0x6819D0)**: esta definida en linea en `TaskManager.h`; con `/O2` se expandia (`GetActiveTask`+`GetLastTaskOf` directos) y el banco dio un acceso invalido en el primer intento. Se invoca por puntero a miembro volatil, con las dos llamadas del EXE (antes de `IsGoToTask` y antes de `UpdatePoint`).
4. **Bit del indicador de linea de vision**: el EXE hace `and al,1` sobre el AL de `GetIsLineOfSightClear`; con `bool` el compilador daba por hecho 0/1 y hacia `or al,cl` (un AL no canonico como 2 o 0xFF corrompia los otros 7 bits del byte `+0x1E`). El banco lo detecto en 453 casos de ControlSubTask; corregido pasando el retorno por memoria volatil y enmascarando.
5. `m_pSubTask` se lee sin comprobar nulo en MakeAbortable/CreateNextSubTask y el orden de las llamadas virtuales a `GetTaskType` (hasta cuatro en ControlSubTask) coincide con el EXE (el banco registra cada una).
Revisado sin discrepancias: los saltos con signo/sin signo (`cmp [..],1Eh; jle`, `cmp edx,1F4h; ja`, `cmp edx,0A6h; jb`, `cmp [route],8; jge`), la prueba `test ah,5` / `test ah,41h` de cada comparacion x87 (NaN: `64 < |D|²` falso, `dot >= 0.95` falso -> no se elimina, `12.25 < total` falso -> WALK), `movsx` de `m_moveState`, `sub eax,0FFFFFEB4h`, argumentos de `UpdatePoint` (0.5f de 0x86FC78, flag 0), argumentos de los constructores (pushes de CreateSubTask).

## Llamadores y residuo de registros

Las cinco son ranuras de la vtable (CTaskComplex: 6 MakeAbortable, 8 CreateNext, 9 CreateFirst, 10 Control, 11 CreateSubTask). Escaneo de todo `.text` del EXE (capstone) de `call [reg+0x18/0x20/0x24/0x28/0x2C]`: 27 sitios leen ECX/EDX tras la llamada y todos son falsos positivos (`xor ecx,ecx`/`xor edx,edx` que los escriben, `lea ecx,[ecx]` de relleno, y `push ecx` en 0x68136D que reserva un hueco de float que `fstp [esp]` sobrescribe). **Ningun llamador lee ECX/EDX de salida; EAX es el retorno** (para MakeAbortable los llamadores usan AL). El banco compara EAX (AL en MakeAbortable) y solo registra ECX/EDX/EFLAGS (`residue_differs` = 4096 en ControlSubTask: el residuo es distinto y no importa).

## Que se compara

Original = EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO ese hook encendido. **Sustituidos** en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL, resuelto con el hook encendido antes de parchear): `CPed::IsAlive` 0x5E0170, `CWorld::GetIsLineOfSightClear` 0x56A490, `CTaskManager::GetSimplestActiveTask` 0x6819D0, `CTask::IsGoToTask` 0x61A360 (sin hook: la DLL tiene su cuerpo), `CTaskSimpleGoToPoint::UpdatePoint` 0x645700 y `CTask::operator new` 0x61A5A0. **Reales** en ambos lados (hook en HEAD, saltos a la DLL, vigilados): constructores 0x667CD0, 0x62F310, 0x653060 y 0x46AC10, que construyen en una arena que devuelve el falso de `operator new`; se compara el objeto palabra a palabra y el oraculo comprueba los campos que dependen de los argumentos. **No sustituidos** (EXE contra DLL): `CVector` (operator-, SquaredMagnitude), `CVector::Normalise` (0x59C910 contra `Vector.cpp`), `CPointRoute::AddUnlessFull` (0x48E180 contra la plantilla en linea) y la eliminacion de puntos (0x5F13A0 contra `FootstepsRouteRemove`), y `CTimer::m_snTimeInMilliseconds` (global 0xB7CB84, fijado por caso y restaurado).
Las tres llamadas virtuales (GetTaskType y MakeAbortable de la subtarea, CreateSubTask de `this`) van por vtables falsas de arena; cualquier otra ranura registra un evento `trap`. Observado por caso: retorno (etiqueta o AL), secuencia ordenada de eventos con argumentos (incluido el contenido de los vectores y los bytes de los booleanos), cambios de `this` (todo el objeto y sus guardas), de la ruta (todo `CPointRoute`) y de la subtarea (`+8`), objeto construido, el resto de arenas intactas, temporizador, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado: FCW+TOP+etiquetas+ST validos), ruta (`route_ok`), colaboradores (`collaborators_dll`), estado restaurado.

## Fixtures: 5 x 4096 (16 familias x 256 por funcion)

Ver la cabecera de `Fixtures.hpp` y `fixture_oracle.py` (estadisticas de cobertura por rama). Extremos: bordes exactos de 1.4², 8², 2.0, 1.0, 500, 166, 30, 0.35², coseno 0.95 (angulos 17.9..18.2 grados), contadores con signo 0x7FFFFFFF/0x80000000, temporizadores que envuelven (`now` 5, 0x80000000, 0xFFFFFF00), AL no canonico (2, 0x80, 0xFF, basura en los 24 bits altos), NaN/inf en posiciones y en puntos de la ruta, posicion por matriz o por placement (la otra con valores distintos), ruta con 0..8 puntos, `m_moveState` negativo, tipo de subtarea fuera de rango (0x80000384, 0x1384, 0xFFFFFFFF...), `operator new` nulo, resultado de `CreateSubTask` nulo/arena/arbitrario. Los minimos de `run_local.py` estan calibrados con `fixture_mirror.py` (~50 % de lo medido).

## Suposiciones y riesgos

- **Arenas** (suma a mano; las comprueba `fixture_mirror.py` contra el oraculo): this 0x20 + 0x20 + 0x20 = 0x60; subtarea 0x20 + 0x30 + 0x20 = 0x70; peaton/objetivo 0x40 + 0x480 + 0x40 = 0x500; matriz 0x10 + 0x48 + 0x18 = 0x70; inteligencia 0x20 + 0x278 + 0x20 = 0x2B8; ruta 0x20 + 0x64 + 0x24 = 0xA8; tarea de GetSimplestActiveTask 0x10 + 0x30 + 0x10 = 0x50; tarea reservada 0x20 + 0x50 + 0x20 = 0x90.
- El objetivo es una «entidad edificio sin IPL» (`SetType(BUILDING)`, ni temporal ni procedural): `CEntity::RegisterReference` del constructor de SEEK no hace nada (como en PartnerNext60); el banco de ese constructor valida el registro de referencias.
- **Magnitudes**: las posiciones se mantienen en rangos donde el redondeo a float en memoria y el rango de exponente extendido del x87 dan la misma decision (|coordenadas| <= 1e9, sin denormales); inf/NaN solo en posiciones y en un punto de la ruta. `CVector::Normalise` (0x59C910) se valida solo en ese rango (la DLL lo compila con SSE).
- La ruta usa contadores 0..8 (GO_TO_POINT con contador 0 deja el contador en 0xFFFFFFFF como el EXE; incluido).
- El oraculo parte del desensamblado y coincide con el EXE en los 20480 casos (comprobado en cada corrida por `run_local.py`).
- `static_assert` en `LiveComparison.hpp`: tamanos y offsets de `CTaskComplexFollowPedFootsteps`, `CPointRoute`, `CTaskSimpleGoTo::m_moveState`, `CPed` (inteligencia +0x47C, banderas +0x46C), `CPedIntelligence::m_AnotherStaticCounter`.

## Revalidacion del constructor

El constructor 0x694E20 (y el destructor) de la clase pasan a compilarse con las opciones por archivo de arriba. Se repitio el banco historico `reversing/EventsTasks56` (128 pares, incluye ese constructor) en este worktree con el fuente final: **APROBADO** (`E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-evtask5661020-010003-865151`; hubo que fijar `runtime_runner.GAME` al clon porque su `build_probe.py`/`run_local.py` no lo hacen). El destructor no tiene hook ni banco propio.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
