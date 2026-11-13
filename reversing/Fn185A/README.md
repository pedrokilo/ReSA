<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 902 grupos / 10558 funciones reconstruidas; 10558 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn185A: 26 funciones del lote 185A (validacion del lote reconstruido sin validar)

Estado: **APROBADO dentro del juego** (13/11/2026, worktree `wv-2` desde main 3bf52cbb0b + cherry-pick de 632c88c31f + correcciones impuestas por el banco; plantillas `Fn182A`, `Fn173A` y `Fn176A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn185a/20261113-005204-484075` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn185A-20261113-005204-484075`, `comparisons.jsonl.gz`, 63 MB). 26 x 16384 = **425984 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, que ademas reimplementa la aritmetica x87 de 24 bits con fracciones exactas). SHA256 de la sonda ejecutada: `dfadf6aecc4e900fc83567e16c6c7aa23fb9b2df592ad84eadb100f14d6da678`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

| # | Funcion real | Entrada | Tipo |
|---|---|---|---|
| 0 | CVehiclePool | 0x4048E0 | S |
| 1 | COnscreenTimerEntry::Process | 0x44CB10 | S |
| 2 | COnscreenTimer::ProcessForDisplay | 0x44CC20 | S |
| 3 | COnscreenTimer::Process | 0x44CD30 | S |
| 4 | DoFade | 0x53E600 | S |
| 5 | CLinkList<CPed*>::Clear | 0x53E8D0 | S |
| 6 | CObject::ConstructorDefault | 0x5A1D10 | S |
| 7 | CObject::ConstructorFromDummy | 0x5A1DF0 | S |
| 8 | CObject::operator new(size_t, int32 poolRef) | 0x5A1EF0 | S |
| 9 | CObject::operator delete(void*) | 0x5A1F20 | S |
| 10 | cHandlingDataMgr::Initialise | 0x5BF3D0 | S |
| 11 | CTaskComplexPartnerChat::GetTaskType | 0x681E10 | V |
| 12 | CTaskComplexPartnerShove::GetTaskType | 0x681E20 | V |
| 13 | CTaskComplexPartnerChat::Clone | 0x684E30 | V |
| 14 | CTaskComplexPartnerShove::Clone | 0x684ED0 | V |
| 15 | CTaskComplexPartner::ScalarDeletingDestructor | 0x684F60 | S |
| 16 | CTaskComplexPartnerDeal::ScalarDeletingDestructor | 0x684F80 | S |
| 17 | CTaskComplexPartnerGreet::ScalarDeletingDestructor | 0x684FA0 | S |
| 18 | CTaskComplexPartnerChat::ScalarDeletingDestructor | 0x684FC0 | S |
| 19 | CTaskComplexPartnerShove::ScalarDeletingDestructor | 0x684FE0 | S |
| 20 | CTaskComplexFollowPedFootsteps::Clone | 0x695890 | V |
| 21 | CTaskComplexFollowLeaderInFormation::ConstructorFormation | 0x695B20 | S |
| 22 | CTaskComplexFollowLeaderInFormation::ScalarDeletingDestructor | 0x695BC0 | S |
| 23 | CTaskComplexFollowLeaderAnyMeans::ScalarDeletingDestructor | 0x695BE0 | S |
| 24 | CTaskComplexSeekEntityAiming::ScalarDeletingDestructor | 0x695C00 | S |
| 25 | CTaskComplexFollowPedFootsteps::DestructorBody | 0x695DC0 | S |

Las clases del listado de entrada eran en su mayoria incorrectas; los nombres reales y el detalle por funcion estan en `reversing/PendienteValidar/185A.md`. Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 26 filas pasan a `validated=1`.

## Funciones corregidas por el banco
Las correcciones de fuente salieron de las corridas parciales (160 casos por funcion) y de la lectura del desensamblado antes de la primera corrida; la corrida completa posterior no dio diferencias.
- **DoFade (0x53E600)**: la DLL construia el rectangulo con `CRect(left, bottom, right, top)` = (-5, alto + 5, ancho + 5, -5): el constructor de `CRect` lo da por invertido (`assert(!IsFlipped())`, se dispara en cada fotograma de una compilacion de depuracion y detenia el banco). Ahora se rellenan los campos (memoria (-5, -5, ancho + 5, alto + 5), la del EXE). Ademas el EXE suma ancho y alto como ENTEROS (`add ecx,5`, con desbordamiento) y luego `fild` + `fstp`; la DLL sumaba `5.0f` al float (distinto a partir de 2^24): ahora `float(int32(uint32(ancho) + 5))`.
- **Destructores con borrado de FollowLeaderInFormation (0x695BC0), FollowLeaderAnyMeans (0x695BE0) y SeekEntityAiming (0x695C00)**: llamaban al destructor generado por el compilador, que deja la vtable de la DLL y no pasa por `CTaskComplex::Destructor` (0x61A3D0). Los cuerpos 0x694A40, 0x694B30 y 0x694C10 (hook `Destructor`, antes envoltura del destructor generado) son ahora la secuencia exacta del EXE: vtable de la clase (0x870C3C / 0x870C68 / 0x870C94), `CleanUpOldReference` (0x571A00) si la referencia no es nula y `CTaskComplexBody::Destructor`; los destructores con borrado los llaman como el `call` del EXE (`Destructor()` publica y `noinline`).
- **Constructores de CObject (0x5A1D10 y 0x5A1DF0)**: el cuerpo era `this->CObject::CObject(...)` (CPhysical e Init en linea de la DLL, imposibles de sustituir) seguido de la vtable. Ahora la secuencia del EXE con las llamadas a los cuerpos con hook: `CPhysical::Constructor` (0x542260, publica y `noinline`), vtable 0x866F60, `Init` (0x59F840, `noinline`), y desde el dummy los pasos de `ObjectAdoptDummy` (compartidos con `CObject::CObject(CDummyObject*)`: `SetModelIndexNoCreate` con el WORD con signo de +0x22, antes zero-extendido; si no hay RwObject la matriz del dummy se crea si falta y se pasa a `SetMatrix`; `DetachFromRwObject`; `Init`; bytes +0x2E y +0x2F; bit 0x200; atomo propio si el byte de tipo es 1, si no `GetFirstAtomic`; `IsCBPCPipelineAttached`).
- **Clone de FollowPedFootsteps (0x695890)**: el archivo se compila con `/O2` e inlining y el constructor se expandia en `Clone` (el falso del constructor no se veia): `Constructor` marcado `noinline`.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) de 0x1000 con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (ver `LiveComparison.hpp`); el constructor base de `CTaskComplex` (0x61A3B0) y el cuerpo 0x684280 de Greet corren reales con el hook apagado. Memoria global tocada y restaurada: ventana de 0x100 bytes de `ScriptSpace` (+0x1000; los ids de variable apuntan dentro), paso del temporizador, pantalla ancha, bandera de HUD, modo de replay, variables del fundido, ancho y alto de pantalla y el puntero al pool de objetos (pools sinteticos). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 425984 casos por el oraculo (cobertura de ramas por funcion >= 50 en cada clave). Variables de entorno de iteracion: `FN185A_CASES` (multiplo de 16) y `FN185A_ONLY` (indice). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn185A` (la corrida completa tarda ~50 min de juego; `run_local.py` lleva timeout de 9000 s y hay que subir el de `bench_here.sh`, que es de 3400 s).

## Prueba negativa
`mutate.py` (26 mutaciones por ronda, condicionadas a `this` desalineado, al bit 2 de un pool de la plantilla, a pools de menos de 100 ranuras, a tamanos de pedido de mas de 0x10000 o a anchuras de pantalla de mas de 28 bits; algunas viven en `Core/Pool.h` y `Core/LinkList.h`). **Las 26 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261113-013738-649865` (20509 de 53248 pares con diferencias; minimo 3 en `COnscreenTimer::ProcessForDisplay`) y ronda 2 `20261113-014753-043106` (17185 de 53248; minimo 111); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
`this` nulo o crudo; ids de variable de script fuera de la ventana sembrada o la segunda variable de un contador con id 0 (la DLL tiene un assert de `GetPointerToScriptVariable` que el EXE no tiene); anchos de pantalla con rectangulo invertido en `CRect`; `CPool<CVehicle,CHeli>::GetAtRef` con referencias fuera de rango (el EXE no comprueba limites; la plantilla si); `CObject::operator new` sobre una ranura ocupada y `delete` sobre una libre (asserts de la plantilla); bytes +0x75 de `CTaskComplexPartnerChat` distintos de 0/1 (la DLL los pasa como `bool`); la cuenta de direcciones (+0x5B) se compara como byte con signo (el EXE la empuja con signo, la DLL puede empujar los bits altos que quiera); `IsCBPCPipelineAttached` con EAX distinto de 0/1 (el EXE prueba EAX, la DLL solo AL); clump sin atomos en `ConstructorFromDummy` (el EXE llamaria a 0x5D7F40 con nulo); bytes de `bool` distintos de 0/1 en JustLoaded y StillToFadeOut (el EXE compara con 1); NaN senalizantes en el paso del temporizador mas alla de los patrones sembrados; y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)

Busqueda de cada direccion (con y sin `0x`, ceros a la izquierda opcionales, sin distinguir mayusculas) en `reversing/*/` (fuentes, `.py`, `.md`, `.json`, `.txt`, `.csv`; sin `_handoff`, `runs/`, `CppTwelveLevel6Final`, este documento ni los inventarios BinaryCoverageAudit, que son listados y no bancos).

| Direccion | Bancos / carpetas |
|---|---|
| 0x4048E0 | RadarBlip28 (README.md, fixture_oracle.py) |
| 0x44CC20 | HookUnlockGame (RenderPairs.hpp), Sys12 (LiveComparison.hpp) |
| 0x44CD30 | Misc70 (LiveComparison.hpp) |
| 0x53E600 | GameIdle (LiveComparison.hpp), HookUnlockGame (IdlePairs.hpp) |
| 0x53E8D0 | GameIdle (LiveComparison.hpp, README.md), HookUnlockGame (IdlePairs.hpp) |
| 0x5A1D10 | ObjectCtor28, Fn172A, Helpers65C, Boat23 (LiveComparison.hpp), AutomobileFinal4 (CasesG2.hpp), CppOnlyMigration/approvals.json, validation-registry/parte-004.json y parte-005.json |
| 0x5A1EF0 | PoolsWave4 (LiveComparison.hpp, README.md), PoolsPending/combo/5D4800.txt |
| 0x5A1F20 | Fn125A, Fn142A, Fn144A (validation-summary.json), Helpers65C (README.md), CppOnlyMigration/approvals.json, validation-registry/parte-005.json |
| 0x695890, 0x695B20 | Tasks355Evidence/CTaskComplexFollowLeaderInFormation_695740.txt (volcado de evidencia), Fn132A/README.md (solo 0x695B20) |
| 0x44CB10, 0x5A1DF0, 0x5BF3D0, 0x681E10, 0x681E20, 0x684E30, 0x684ED0, 0x684F60, 0x684F80, 0x684FA0, 0x684FC0, 0x684FE0, 0x695BC0, 0x695BE0, 0x695C00, 0x695DC0 | ninguno (solo BinaryCoverageAudit/unmatched-functions.csv y virtual-slots-outside-catalogue.csv, que son inventarios) |

Los cuerpos que estas funciones invocan y que si tienen banco: 0x683A40 (Fn123A), 0x684200 (Fn122A), 0x684320 (Fn153A), 0x684750 (Fn151A), 0x694E20, 0x611F20 (Fn132A), 0x6949A0, 0x61A5B0, 0x61A3D0 y 0x61A3B0 (tareas base).

**Repetir ademas** (funciones de otros lotes cuyo cuerpo cambio al corregir este): 0x694A40 -> `RequestedParallelTasks`, `RequestedPreflightTasks`, `RequestedValidation20261008`; 0x694B30 -> `CTasks8a`, `RequestedValidation20261008`; 0x694C10 -> `CTasks7b`, `RequestedValidation20261008` (los tres cuerpos de destructor ahora son la secuencia del EXE y no el destructor generado); 0x694E20 -> `EventsTasks56`, `FollowFootsteps62` (constructor ahora `noinline`); 0x59F840 y 0x542260 -> `InteriorPlacement`, `Object48`, `ObjectCtor28`, `Physical56`, `CPedD`, `CVehB` (`Init` y `CPhysical::Constructor` ahora `noinline`); 0x5A1D10 -> `Fn172A` (el falso por direccion del EXE de ese banco ahora tiene hook); y los bancos de `CObject(CDummyObject*)` (`ObjectCtor28`, `Object48`: el constructor de C++ comparte `ObjectAdoptDummy`, con el WORD con signo del modelo).
