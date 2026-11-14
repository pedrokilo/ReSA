<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 14/11/2026 (Ciudad de México):** 906 grupos / 10590 funciones reconstruidas; 10590 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn188A: 13 funciones del EXE (lote 188A)

Estado: **APROBADO dentro del juego** (14/11/2026, worktree `wv-3` desde main dea54b9cc9 + el commit del lote 188A (cherry-pick de 1a2b8f1b2f de `reconstruccion-sin-validar`) + las correcciones de fuente de abajo; plantillas `Fn187A` (nucleo y herramientas) y `Fn178A` (cuerpo, GetEventType, CloneEditable y destructor con borrado de CEventAcquaintancePedHateBadlyLit)). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn188a/20261114-153625-307072` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn188A-20261114-153625-307072`, `comparisons.jsonl.gz` de 26,6 MB). 13 x 16384 = **212992 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `74958887667ca429cb0e3993b9bdfb24a5fe7360c11ce839ba67f2188a8ee41c`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle de las funciones y riesgos: `reversing/PendienteValidar/188A.md` (commit del lote). El listado de entrada atribuia todo a CEventScriptedAttractor; las clases reales salen de las vtables (0x86C9F0, 0x86CA38, 0x86CA80 y 0x86CAC8, leidas de .rdata del EXE) y del desensamblado.

| # | Funcion real | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CEventScriptedAttractor` cuerpo del destructor | 0x5FEF70 | 11 | S |
| 1 | `CEventScriptedAttractor::GetEventType` | 0x5FEF80 | 5 | V |
| 2 | `CEventScriptedAttractor::CloneEditable` | 0x5FEF90 | 93 | V |
| 3 | `CEventScriptedAttractor` destructor con borrado | 0x5FEFF0 | 30 | S |
| 4 | `CEventAcquaintancePedRespect` cuerpo del destructor | 0x5FF030 | 11 | S |
| 5 | `CEventAcquaintancePedRespect::CloneEditable` | 0x5FF050 | 88 | V |
| 6 | `CEventAcquaintancePedRespect` destructor con borrado | 0x5FF0B0 | 30 | S |
| 7 | `CEventAcquaintancePedLike` cuerpo del destructor | 0x5FF0F0 | 11 | S |
| 8 | `CEventAcquaintancePedLike::CloneEditable` | 0x5FF110 | 88 | V |
| 9 | `CEventAcquaintancePedLike` destructor con borrado | 0x5FF170 | 30 | S |
| 10 | `CEventAcquaintancePedDislike` cuerpo del destructor | 0x5FF1B0 | 11 | S |
| 11 | `CEventAcquaintancePedDislike::CloneEditable` | 0x5FF1D0 | 88 | V |
| 12 | `CEventAcquaintancePedDislike` destructor con borrado | 0x5FF230 | 30 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Las 13 filas quedan con validated=1 (sin `sync_docs`/`hooks2md`).

## Correcciones de fuente hechas al validar
Hechas tras leer el desensamblado del EXE y **antes** de la primera ejecucion del banco (la ejecucion posterior aprueba con la fuente corregida; no se ejecuto el banco con la fuente original del lote):
- **Los cuatro `CloneEditable`** (0x5FEF90, 0x5FF050, 0x5FF110, 0x5FF1D0): el EXE llama al constructor de la base (0x4AF350 `CEventAttractor(efecto, entidad, 0)` o 0x4AF820 `CEventAcquaintancePed(conocido)`) y despues escribe la vtable de la clase (0x86C9F0 / 0x86CA38 / 0x86CA80 / 0x86CAC8). El lote construia con `::new (memory) CEvent...(...)`, que ejecuta el constructor de C++ (vtable de la DLL, sin pasar por el envoltorio enganchado del constructor). Ahora llaman al envoltorio enganchado (`static_cast<CEventAttractor*>(memory)->Constructor(effect, entity, false)` y `static_cast<CEventAcquaintancePed*>(memory)->Constructor(ped)`) y escriben la vtable del EXE. `CEventAttractor` declara `friend class CEventScriptedAttractor` porque su `Constructor` es privado.
- Sin cambios en los cuerpos del destructor, GetEventType ni los destructores con borrado: el banco los aprueba tal como llegaron.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A, B, C y D de 0x1000 bytes con guardas de 0x40). Falsos identicos en la direccion del EXE y en el cuerpo de la DLL para los colaboradores con hook: `~CEventAttractor` cuerpo 0x4AF400, `~CEventAcquaintancePed` cuerpo 0x4AF890, `CEvent::operator new` 0x4B5620 y `operator delete` 0x4B5630, constructores 0x4AF350 (tres argumentos, `ret 0Ch`, con el tercero anotado como dword completo) y 0x4AF820, y los cuerpos del propio lote (0x5FEF70, 0x5FF030, 0x5FF0F0, 0x5FF1B0) para los destructores con borrado. Cada falso anota ECX, argumentos y el dword de `[ECX]` al entrar y deja un dword propio en `[ECX]`, de modo que el orden vtable / llamada queda observado; el valor devuelto del constructor falso se aleatoriza para comprobar que `CloneEditable` devuelve la memoria y no el EAX del constructor. Ninguna funcion toca memoria global. 16 familias de 1024 casos por funcion (objeto alineado, desalineado, al borde y cerca del borde; relleno aleatorio, ceros, unos y 0xCD; memoria de `new` nula o en la arena B; banderas del destructor con bit 0 puesto o no y con bits altos); `python -I fixture_selfcheck.py` pasa los 212992 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn188A` (con un envoltorio de una sola sesion del candado para la corrida completa y las rondas negativas).

## Prueba negativa
`mutate.py` (13 mutaciones por ronda, condicionadas a `this` / `self` desalineado: vtable del cuerpo, constante de GetEventType, argumento del constructor o comprobacion de memoria en CloneEditable, bandera o llamada al cuerpo en el destructor con borrado). **Las 13 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261114-155012-296191` con 8796 de 26624 pares con diferencias y ronda 2 `20261114-155221-292020` con 3577 de 26624; el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y manifiesto principal regenerado.

## Fuera de los fixtures
`this` nulo o crudo, pool de eventos lleno o corrupto con `CEvent::operator new` real, objetos reales construidos por el EXE (eventos con referencias registradas de verdad), el marco SEH de `CloneEditable` (solo libera la memoria si el constructor lanza; se omite), el tercer argumento del constructor de `CEventAttractor` con bits altos (el EXE siempre empuja 0) y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep de las 13 direcciones (como `0x5FEF70` ... `0x5FF230`) en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): ningun banco las cita. Por nombre de clase (CEventScriptedAttractor, CEventAcquaintancePedRespect/Like/Dislike) aparecen en `CppIntegerWorld4`, `CppIntegerWorld10`, `EventsTasks56`, `Fn92C`, `HookUnlockGame`, `RequestedPreflightTaskCtorDtor`, `RequestedPoolsAtomicCheck`, `RequestedPoolsSixGetters(V2)`, `RequestedRemainingLeafPair(V2)` y otros `Requested*`; conviene repetir los que crean o clonan estos eventos (CloneEditable cambia de ruta: ahora pasa por el envoltorio de los constructores 0x4AF350 y 0x4AF820) y los de `TaskComplexWanderProstitute` / `TaskComplexWanderStandard`, que usan las clases en pila.
