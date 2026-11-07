# Fn115A: constructores y destructores de tareas y eventos (lote 115A)

Estado: **APROBADO dentro del juego** (07/11/2026, worktree `wv-3` sobre main f78028561 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn115a/20261107-125208-210014` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn115A-20261107-125208-210014`, `comparisons.jsonl.gz` de 77 MB). 20 x 16384 = 327680 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 vacia, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).

Veinte funciones de codigo de juego (nueve cuerpos no virtuales de destructor y once constructores del EXE) en C++ puro (sin `__asm`/`naked`, cuerpos en el espacio de nombres anonimo del `.cpp` de cada clase, como en los lotes 106A/117A) y enganchadas (tipo S). Los nombres de `pendientes-A.md` atribuyen por cercania; la clase real sale de la vtable que escribe cada funcion (coincide con `RH_ScopedVirtualClass` de la clase).

| Funcion | Direccion | Bytes | Fuente |
|---|---|---|---|
| `CEventDontJoinPlayerGroup` destructor | 0x609140 | 90 | `Events/EventDontJoinPlayerGroup.cpp` |
| `CEventPassObject` destructor | 0x65DCF0 | 90 | `Events/EventPassObject.cpp` |
| `CTaskComplexKillPedOnFoot` destructor | 0x620EF0 | 90 | `Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp` |
| `CTaskComplexSequence` destructor (Flush + base) | 0x6389F0 | 79 | `Tasks/TaskComplexSequence.cpp` |
| `CTaskSimpleCarSetPedInAsDriver` destructor | 0x647170 | 90 | `TaskSimpleCarSetPedInAsDriver.cpp` |
| `CTaskSimpleCarSetPedOut` destructor | 0x647950 | 90 | `TaskSimpleCarSetPedOut.cpp` |
| `CTaskSimpleGoTo` destructor | 0x667D50 | 11 | `TaskSimpleGoTo.cpp` |
| `CTaskComplexWander` destructor | 0x669D20 | 11 | `TaskComplexWander.cpp` |
| `CTaskComplexJump` destructor | 0x67A060 | 11 | `TaskComplexJump.cpp` |
| `CTaskSimpleAnim` constructor(bool) | 0x61A6C0 | 47 | `TaskSimpleAnim.cpp` |
| `CTaskSimplePlayHandSignalAnim` constructor | 0x61AE50 | 76 | `TaskSimplePlayHandSignalAnim.cpp` |
| `CTaskSimpleDrown` constructor | 0x62FEC0 | 62 | `TaskSimpleDrown.cpp` |
| `CTaskComplexAttractorPartnerWait` constructor | 0x633250 | 34 | `TaskComplexAttractorPartnerWait.cpp` |
| `CTaskComplexUseGoggles` constructor | 0x634EF0 | 18 | `TaskComplexUseGoggles.cpp` |
| `CTaskComplexUseEffectRunning` constructor | 0x636780 | 39 | `TaskComplexUseEffectRunning.cpp` |
| `CTaskComplexUseEffectSprinting` constructor | 0x636860 | 39 | `TaskComplexUseEffectSprinting.cpp` |
| `CTaskComplexStuckInAir` constructor | 0x67BA40 | 18 | `TaskComplexStuckInAir.cpp` |
| `CTaskSimpleArrestPed` constructor | 0x68B620 | 91 | `TaskSimpleArrestPed.cpp` |
| `CTaskAllocatorPlayerCommandAttack` constructor | 0x69C480 | 50 | `TaskAllocatorPlayerCommandAttack.cpp` |
| `CTaskSimpleFacial` constructor | 0x690C70 | 48 | `TaskSimpleFacial.cpp` |

Total 1085 bytes; ninguna supera 1200 B ni depende de mas de dos funciones sin hook (todos los colaboradores tienen hook). Dejadas: `CTaskComplexPartner` destructor 0x683A40 y `CTaskComplexPartnerDeal` 0x684200 (usan `repe cmps` y colaboradores), constructor de `CTaskSimpleSlideToCoord` 0x66C3E0 (depende de 0x61A6C0, del mismo lote), `CTaskComplexObserveTrafficLightsAndAchieveHeading` 0x631910 (clase sin `InjectHooks`) y las clases de vtable sin registrar (0x86CD8C, 0x86F45C, 0x8701A8).

Cambios de produccion: `CEventBody::EventDestructorBody` (Event.cpp) y `CTaskComplexBody::Destructor/Constructor` (Task.cpp) salen del espacio anonimo / de `static` para que los cuerpos nuevos los llamen como el original; `CTaskComplexUseEffect::Constructor` y `CTaskAllocatorAttack::Constructor` pasan a publicos (los llaman los constructores de las subclases). Cada cuerpo se instala con `RH_ScopedNamedGlobalInstall(..., "Constructor"|"Destructor", direccion)`. Filas anadidas a `docs/hooks.csv`.

## Revision contra el EXE (desensamblado)
Destructores (thiscall, `ret`, marco SEH que solo desenrolla): vtable propia, `CleanUpOldReference` (0x571A00) de la referencia no nula (+0xC, +0xC, +0x10, +0x10, +8), y la cola: `~CEvent` 0x4AC010 (eventos), `~CTask` 0x61A3A0 (CarSetPedInAsDriver, CarSetPedOut, GoTo), `~CTaskComplex` 0x61A3D0 (KillPedOnFoot, Wander, Jump) o `Flush` 0x632C10 + 0x61A3D0 (Sequence). Constructores (thiscall, `ret N`, EAX = this): base 0x61A390 / 0x61A3B0 / 0x6321F0 / 0x69C240 (+ `FindPlayerPed(-1)` 0x56E210 antes en PlayerCommandAttack), campos y vtable propia del EXE; detalles en `fixture_oracle.py` (tabla `CTORS`). Llamadores (`callers116.py`): ningun `call` lee EAX/ECX/EDX antes de escribirlos; los cinco avisos (0x620FF6, 0x48E606, 0x48E726, 0x658856, 0x675466) son `jmp` de cola seguidos de relleno.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado durante el banco): 0x571A00, 0x571B70, 0x4AC010, 0x61A390, 0x61A3A0, 0x61A3B0, 0x61A3D0, 0x632C10, 0x6321F0, 0x56E210, 0x69C240; registran sus argumentos, los punteros como etiquetas de arena y la vtable del objeto en ese momento (comprueba el orden). Se observan todos los bytes cambiados de las tres arenas (objeto de 0x80 bytes con guardas y dos entidades) salvo el puntero de ArrestPed (etiqueta), eventos de los falsos, EAX y el estado x87. 16 familias de 1024 casos (`Fixtures.hpp`): punteros en arena/nulos/crudos, dwords 0/1/2/0x100/0xFFFFFF01/0x80/aleatorios, vtable inicial propia/base/cero/-1/aleatoria, modo del jugador. `fixture_selfcheck.py` ejecuta el oraculo sin juego sobre los 327680 casos.

## Fuera de los fixtures
Objetos reales con vtables del ASI, metodos virtuales, registros volatiles, `this` nulo, la exactitud de los colaboradores falseados.
