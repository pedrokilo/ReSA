<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 900 grupos / 10514 funciones reconstruidas; 10514 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn184A: 26 funciones diminutas del EXE (lote 184A)

Estado: **APROBADO dentro del juego** (13/11/2026, worktree `wv-5` desde main 9faecfcc8b + el commit del lote 184A (cherry-pick de a2879855ad de `reconstruccion-sin-validar`) + la correccion de fuente de abajo; plantillas `Fn180A` y `Fn176A` (destructores de eventos con el pool)). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn184a/20261112-225613-794446` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn184A-20261112-225613-794446`, `comparisons.jsonl.gz` de 60,6 MB). 26 x 16384 = **425984 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `b96f0ff922ffefe4d09463b0ebdddf452cef9c038d4f30e3e8107a7326207004`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle de las funciones, los riesgos y la saltada (0x49EA70, `jmp` puro a un `ret`): `reversing/PendienteValidar/184A.md` (commit del lote). Las clases del listado de entrada eran en su mayoria incorrectas: los nombres reales salen del desensamblado y las vtables.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CTaskComplexWander::GetTaskType` (CTaskComplexWanderCop) | 0x460CD0 | 6 | V |
| 1 | `CTaskComplexWanderCop::Clone` | 0x460CE0 | 102 | V |
| 2 | `CTaskComplexWanderCop::GetWanderType` | 0x460D50 | 6 | V |
| 3 | `CTaskComplexWanderCop` destructor con borrado | 0x460D60 | 30 | S |
| 4 | `CTaskComplexWanderCop` cuerpo del destructor | 0x460D80 | 87 | S |
| 5 | `FxEntitySystem` constructor (Fx_c) | 0x49EA80 | 12 | S |
| 6 | `Fx_c::Init` | 0x49EA90 | 66 | S |
| 7 | `Fx_c::Reset` | 0x49EAE0 | 21 | S |
| 8 | `FxEntitySystem` destructor con borrado (Fx_c) | 0x4A11C0 | 27 | S |
| 9 | `Fx_c::CreateEntityFx` | 0x4A11E0 | 153 | S |
| 10 | `Fx_c::DestroyEntityFx` | 0x4A1280 | 67 | S |
| 11 | `Fx_c::ExitEntitySystems` | 0x4A12D0 | 57 | S |
| 12 | `Fx_c::Exit` | 0x4A1320 | 26 | S |
| 13 | `CEventPotentialWalkIntoFire::GetEventType` | 0x4B1ED0 | 6 | V |
| 14 | `CEventPotentialWalkIntoFire::GetLifeTime` | 0x4B1EE0 | 3 | V |
| 15 | `CEventPotentialWalkIntoFire::GetEventPriority` | 0x4B1EF0 | 6 | V |
| 16 | `CEventFireNearby::AffectsPed` (CEventPotentialWalkIntoFire) | 0x4B1F90 | 45 | V |
| 17-21 | destructor con borrado de `CEventGunShotWhizzedBy`, `CEventSeenPanickedPed`, `CEventAreaCodes`, `CEventPlayerCommandToGroup` y `CEventDanger` | 0x4B6990, 0x4B69E0, 0x4B6A30, 0x4B6A80, 0x4B6AD0 | 76 | S |
| 22 | `CEventInteriorUseInfo` constructor (CTaskComplexWanderCop) | 0x674EA0 | 48 | S |
| 23 | `CEventInteriorUseInfo` cuerpo del destructor | 0x674ED0 | 11 | S |
| 24 | `CEventInteriorUseInfo::Clone` | 0x674F00 | 122 | V |
| 25 | `CEventInteriorUseInfo` destructor con borrado | 0x674F90 | 30 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 26 filas quedan con validated=1 (sin `sync_docs`/`hooks2md`).

## Correccion de fuente hecha al validar
- **`CEventInteriorUseInfo::Constructor` (0x674EA0) y, por el, `Clone` (0x674F00)**: el cuerpo del lote usaba el constructor de C++ de la clase (constructor base en linea, vtable de la DLL), pero el EXE llama primero al constructor de `CEvent` (0x4ABFC0) y deja la vtable 0x8701A8. Ahora llama al cuerpo enganchado `CEvent::Constructor` (`friend class CEventInteriorUseInfo` en `Event.h`, como `CEventEscalator` en el lote 151A), escribe los campos y la vtable 0x8701A8. Hecha tras leer el desensamblado y **antes** de la primera ejecucion (la primera ejecucion, de 256 casos por funcion, ya paso con cero diferencias; solo se quejo de la cobertura de ramas, que exige 50 casos por marca y con 256 no llega).
- El resto de las funciones pasaron sin cambios de fuente (incluidas `ExitEntitySystems` y `DestroyEntityFx`, corregidas ya en el lote: el siguiente nodo se lee antes de liberar).

## Revision contra el EXE (`dumpbin /disasm` sobre el PE)
- **0, 2, 13-15**: constantes `mov eax, imm` (0x390, 1, 0x2E, 0x1F) y `xor eax,eax`; no leen `this`. **1** `Clone`: `new(0x50)` 0x61A5A0; sin memoria nulo; constructor 0x460C80 con `[this+0xC]` y el BYTE `[this+0x10]` ampliado a cero; devuelve el EAX del constructor. **3**, **25**, **8**, **17-21**: `push esi; mov esi,ecx; call cuerpo; test byte [esp+8],1; je ..; delete; mov eax,esi; ret 4` (cuerpos 0x460D80, 0x674ED0, 0x49EA70 que es `ret`; delete 0x61A5B0, 0x4B5630 o 0x8214BD; los cinco eventos liberan la ranura del pool 0xB744AC en linea, `/0x44` con signo, `|= 0x80`, cursor con signo). **4**: vtable 0x859C80, delete virtual (ranura 0, bandera 1) de `+0x28` si no es nulo, cuerpo 0x669D20.
- **5**: ListItem_c 0x4A8DB0 (`+0` y `+4` a cero). **6** `Init`: 0x4A98E0, 0x5C2420 ("models\effects.fxp"), 0x4A93E0 (0xC813E0, 0xC812F0), 0x49E660 (version C++ de la DLL), `+0x50 = 0`; el EXE devuelve AL=1 (void aqui; los llamadores lo ignoran). **7** `Reset`: 0x4A98B0 y salto a 0x49E660. **9** `CreateEntityFx`: CreateFxSystem 0x4A9BE0 (nombre, pos, matriz, 1); si no es nulo: new(0x10) 0x82119A, nodo, `+8` sistema, `+0xC` entidad, AddItem 0x4A8DF0 y Play 0x4AA2F0. **10** y **11**: recorrido con el siguiente leido antes de liberar; RemoveItem 0x4A8E30; Kill 0x4AA3F0 / DestroyFxSystem 0x4A9810; delete 0x8214BD. **12** `Exit`: 0x4A12D0, 0x49E850 y salto a 0x4A9A10.
- **16** `AffectsPed`: inteligencia `[ped+0x47C]`, FindActiveTaskByType 0x681740 con ECX = inteligencia+4 y tipo 0x25C; si hay tarea AL = 0; si no `CPed::IsAlive` 0x5E0170. **22**: `CEvent` 0x4ABFC0 primero, `+0xC` info, BYTE `+0x18`, vtable, `+0x10`, `+0x14`; **23**: vtable y salto a 0x4AC010; **24**: new(0x1C) 0x4B5620, constructor base 0x4ABFC0, campos y vtable.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A, B, C y D de 0x1000 bytes con guardas de 0x40). Falsos identicos en la direccion del EXE y en el cuerpo de la DLL para los colaboradores con hook (0x460D80, 0x669D20, 0x4B2070, 0x4B2130, 0x4B2210, 0x4B2470, 0x4B26A0, 0x674ED0, 0x4AC010, 0x61A5A0, 0x61A5B0, 0x4B5620, 0x4B5630, 0x460C80, 0x4ABFC0, 0x4A98E0, 0x5C2420, 0x4A93E0, 0x4A98B0, 0x4A9BE0, 0x4A9810, 0x4A9A10, 0x4AA2F0, 0x4AA3F0, 0x681740, 0x5E0170 y, en `Fx_c::Exit`, 0x4A12D0 con su propia ruta); solo en la direccion del EXE los que la DLL llama por ella (operator new 0x82119A y operator delete 0x8214BD); en la direccion del EXE y en su funcion propia de la DLL los que no tienen hook (0x49E660 = `Fx_c::InitStaticSystems`, 0x49E850 = `Fx_c::ExitStaticSystems`). Corren reales, comparando la ejecucion del EXE con el codigo en linea de la plantilla `TList_c` de la DLL: ListItem_c 0x4A8DB0, el destructor del nodo 0x49EA70, AddItem 0x4A8DF0 (hook apagado por su ruta), RemoveItem 0x4A8E30, y la liberacion en linea del pool de eventos sobre un pool sintetico (0xB744AC, restaurado). El falso del constructor de CEvent deja 28 bytes de relleno distinto en `+0..+0x1B` para ver el orden de las escrituras; el delete virtual de la tarea hija anota el dword de `[this]` (comprueba que la vtable se escribe antes). Listas de 0 a 5 nodos con recuento coherente (la plantilla de la DLL tiene asserts). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 425984 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn184A` (el limite de 3400 s no alcanza para la corrida completa: se uso un envoltorio con timeouts largos y los mismos candados).

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`; una por funcion y ronda, 26 por ronda; se aplican a las fuentes de produccion, se recompila la sonda y se corre con `FN184A_CASES=2048` casos por funcion), condicionadas a `this` desalineado (ronda 1 cualquier desalineo; ronda 2 el de 2 bytes), que el juego real no produce. **Las 26 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-235244-805685` con 17154 de 53248 pares con diferencias y ronda 2 `20261112-235630-920041` con 7342 de 53248; el lado EXE coincide con el oraculo en todos los pares ("lado EXE fuera del oraculo 0"). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, comprobado tras cada ronda) y manifiesto principal restaurado. Se compilaron las dos sondas mutadas bajo el candado de compilacion y se corrieron las dos rondas en una sola sesion del candado de juego.

## Fuera de los fixtures
`this` nulo o crudo; listas corruptas o con recuento incoherente; operator new nulo en `CreateEntityFx` (el EXE escribiria en la direccion 8); objetos reales construidos por el EXE; y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x460D80 -> `BeCop60` (**repetir**: cambia el cuerpo del destructor); 0x49EA90, 0x49EAE0 y 0x4A1320 -> `Game22` (**repetir**: partida completa que pasa por Init, Reset y Exit); 0x4A11E0 y 0x4A1280 -> `CEntC` (**repetir**). El resto de direcciones no aparece citado. Conviene repetir ademas los que usan `CEventInteriorUseInfo` (su constructor y Clone cambiaron de forma) y los que pasan por `ExitEntitySystems`/`DestroyEntityFx` del juego.
