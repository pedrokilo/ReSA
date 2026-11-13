<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 899 grupos / 10469 funciones reconstruidas; 10469 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn169A: 23 funciones diminutas del EXE (lote 169A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-3` desde main 19c9abfe5e + los cambios del lote; plantillas `Fn153A`, `Fn155A` y `Fn156A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn169a/20261111-224646-881590` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn169A-20261111-224646-881590`). 23 x 16384 = **376832 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `cf9da42adc762da012fdb5286e6461e7538aca2a9dc7d8458d0766c4a8c9651b`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 23 tenia hook; ninguna se salta (todas viables en C++ puro). Los nombres salen del desensamblado y de las vtables leidas del PE; el listado atribuia varias a clases equivocadas.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CEventLeaderEnteredCarAsDriver` cuerpo del destructor (marco SEH) | 0x48E220 | 90 | `Events/EventLeaderEnteredCarAsDriver.cpp` | S |
| 1 | `CEventLeaderEnteredCarAsDriver::GetEventType` (0x36) | 0x48E280 | 6 | idem | V |
| 2 | `::GetLifeTime` (0) | 0x48E290 | 3 | idem | V |
| 3 | `::CloneEditable` (ranura 16) | 0x48E2A0 | 96 | idem | V |
| 4 | `::AffectsPed` (false) | 0x48E300 | 5 | idem | V |
| 5 | `::CanBeInterruptedBySameEvent` (true) | 0x48E310 | 3 | idem | V |
| 6 | `::ScalarDeletingDestructor` | 0x48E320 | 30 | idem | S |
| 7 | `::GetEventPriority` (0x2E) | 0x48E340 | 6 | idem | V |
| 8 | `CEventLeaderExitedCarAsDriver::TakesPriorityOver` (el listado decia Entered; vtable 0x85B898 ranura 11) | 0x4B0F70 | 5 | `Events/EventLeaderExitedCarAsDriver.cpp` | V |
| 9 | `CEvent::AffectsPed` (ranura 5) | 0x4ABFE0 | 5 | `Events/Event.cpp` (+ `.h`) | V |
| 10 | `CEvent::AffectsPedGroup` (ranura 6) | 0x4ABFF0 | 5 | idem | V |
| 11 | `CEvent::CanBeInterruptedBySameEvent` (ranura 15) | 0x4AC000 | 3 | idem | V |
| 12 | `CEvent::IsValid` (ranura 14) | 0x4AC020 | 35 | idem | V |
| 13 | `CInformFriendsEvent` destructor de elemento (copia) | 0x4AC0F0 | 50 | `InformFriendsEventQueue.cpp` | S |
| 14 | `CInformFriendsEvent` constructor con argumentos | 0x4AC130 | 45 | idem | S |
| 15 | `CInformFriendsEvent::Process` (un elemento) | 0x4AC160 | 87 | idem | S |
| 16 | `CInformFriendsEvent` destructor de elemento (el que el EXE pasa al iterador de vectores) | 0x4B2950 | 50 | idem | S |
| 17 | `CTaskComplex::GetSubTask` (ranura 2) | 0x421190 | 4 | `Tasks/TaskComplex.cpp` (nuevo) + `.h` | V |
| 18 | `CTaskComplex::IsSimple` (ranura 3) | 0x4211A0 | 3 | idem | V |
| 19 | `CTaskComplex::MakeAbortable` (ranura 6, salto a la ranura 6 de la subtarea) | 0x4211B0 | 8 | idem | V |
| 20 | `CTaskComplex::ScalarDeletingDestructor` (el listado decia CTask) | 0x61A620 | 59 | idem | S |
| 21 | `CTask::ScalarDeletingDestructor` (ranura 0 de 0x86D48C) | 0x61A660 | 56 | `Tasks/Task.cpp` | S |
| 22 | `CTaskSimpleAnim` cuerpo del destructor (antes funcion propia sin hook) | 0x61A6F0 | 159 | `Tasks/TaskTypes/TaskSimpleAnim.cpp` | S |

Las 23 filas nuevas estan en `docs/hooks.csv`. Se anade `PoolFreeSlotRawShift` a `Pools/PoolRawDelete.h` (liberacion de pool con indice `sar 7`, como la liberacion EN LINEA de tareas del EXE: 0x61A620 y 0x61A660 no llaman a 0x61A5B0).

## Revision contra el EXE
- Las constantes y `mov al,1` / `xor al,al` son lo que dice la tabla. 0x4B0F70 es la ranura 11 de la vtable de `CEventLeaderExitedCarAsDriver`.
- 0x48E220: vtable 0x85A198, `CleanUpOldReference(+14h)` si no es nulo y `call 0x4AC480`. 0x48E2A0: `operator new(0x18)` y constructor 0x48E1C0 con +14h. 0x48E320: cuerpo + `CEvent::operator delete` si el bit 0.
- 0x4AC020: byte +8 o `GetLifeTime()` virtual comparado con +4 (con signo).
- Elemento de `CInformFriendsEvent` (ped +0, evento +4, instante +8): 0x4B2950/0x4AC0F0 limpian la referencia, borran el evento con su destructor virtual (`push 1; call [vtable]`) y dejan el instante en -1; 0x4AC130 registra la referencia y guarda `reloj (0xB7CB84) + retardo`; 0x4AC160 anade el evento al grupo de eventos del ped (+47Ch, +68h; `CEventGroup::Add` 0x4AB420 con `false`) si el instante (sin signo) ya vencio. Sin llamadores directos salvo 0x4B2950 (iterador de vectores del arreglo estatico).
- 0x4211B0 salta a la ranura 6 de la subtarea, no a una direccion fija: se escribe como llamada virtual (viable).
- 0x61A6F0: vtable 0x86D504, `SetFinishCallback` 0x4CEBE0 con `DefaultAnimCB` 0x4D1390, bit 4 en la palabra +0x2E, y si no `m_bHoldLastFrame` y peso > 0 y velocidad >= 0 (ordenadas) velocidad = -4.0; `~CTask` 0x61A3A0.
- Dos entradas del EXE no pueden compartir el mismo cuerpo de hook (la ruta del banco resolvia una por la otra): la copia 0x4AC0F0 tiene su propia funcion `DestructorCopy` que llama a la de 0x4B2950.

## Banco
Una funcion por ruta (original = bytes del EXE con el hook apagado; candidato = DLL con solo ese hook). Arenas A, B, C con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL: `CEvent::operator delete` 0x4B5630, `CleanUpOldReference` 0x571A00, `RegisterReference` 0x571B70, `CEvent::operator new` 0x4B5620, constructor 0x48E1C0, `CEventGroup::Add` 0x4AB420, `SetFinishCallback` 0x4CEBE0, cuerpos 0x4AC480, 0x61A3D0 y 0x61A3A0 (y 0x48E220 como funcion probada del lote). Vtables falsas en la arena C (GetLifeTime, destructor virtual con borrado de un evento, MakeAbortable de una subtarea). Pool de tareas falso en la arena B (puntero 0xB744A8) y reloj 0xB7CB84 fijados por caso y restaurados. 16 familias de 1024 casos por funcion (`Fixtures.hpp`); `python -I fixture_selfcheck.py` (sin juego) pasa los 376832 casos por el oraculo.

## Fuera de los fixtures
`this` nulo o crudo, objetos reales del EXE, subtarea nula en `MakeAbortable` (el EXE se cae), peds o animaciones crudos, eventos crudos en los destructores de elemento y la exactitud de los colaboradores falseados.

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, 23 por ronda, condicionadas a `this` o banderas que el juego no produce; `FN169A_CASES=2048`). **Las 23 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261111-231914-865091` con 18257 de 47104 pares con diferencias y ronda 2 `20261111-233237-827469` con 11155; el lado EXE coincide con el oraculo en todos los pares. Una primera ronda 1 dejo la copia 0x4AC0F0 sin diferencias porque su funcion propia no estaba mutada; se anadio una mutacion propia y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, comprobado tras cada ronda): Event.cpp `847b38dd...`, EventLeaderEnteredCarAsDriver.cpp `e8a87896...`, EventLeaderExitedCarAsDriver.cpp `d524af01...`, InformFriendsEventQueue.cpp `d7122cff...`, Task.cpp `726a44bd...`, TaskComplex.cpp `b0e76134...`, TaskSimpleAnim.cpp `38d6fea1...`.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
- 0x61A6F0: `Fn147A`, `Fn150A` y `Fn151A` la daban por sin hook (`U(0x61A6F0, ...)`): se cambio a `H(...)` y `(0x61A6F0, True)` en su `run_local.py`; **cambio sin ejecutar, hay que repetirlos**.
- 0x421190 y 0x4211A0: `Tasks18` (vtable sintetica que apunta a ellas; ahora saltan a la DLL), citadas tambien en `HooksActivados/README.md` y `TasksPendingEvidence/*.txt` (texto). 0x4211B0: `Helpers65B/README.md` (texto). 0x61A660: `RequestedPreflightTaskCtorDtor/.../ctask-deleting-destructor.combo.txt` (texto). 0x4AC020: `Fn158A/README.md` (texto).
- El resto no esta citado por ningun banco, pero los hooks V de `CEvent` y de `CTaskComplex` los recorren todos los bancos de eventos y tareas complejas.
