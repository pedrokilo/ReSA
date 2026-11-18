<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11707 con evidencia histórica y 734 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn197A: 32 funciones de eventos y tareas del EXE (lote 197A)

Estado: **APROBADO dentro del juego** (15/11/2026, worktree `wv-6` desde main 9acc2209e0 + el lote 197A, cherry-pick de 77643863bc de la rama `reconstruccion-sin-validar`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn197a/20261115-092513-632540` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn197A-20261115-092513-632540`, `comparisons.jsonl.gz` de 61 MB). 32 x 16384 = **524288 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `a6e159b95ec737b3b8b30cb079061afab4be062a06a3eff9355e7f6b425557e0`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 32 direcciones (29 atribuidas a CEventSeenCop y 3 a CTaskComplexSunbathe). Ninguna tenia hook ni envoltura `plugin::Call*` antes del lote. Las clases reales salen de las vtables y del desensamblado (ver `reversing/PendienteValidar/197A.md`, fuera de este arbol de validacion). Se reconstruyen y validan las 32.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CEventHealthLow::CloneEditable` (CEventSeenCop) | 0x5FF490 | 80 | V |
| 1 | `CEventHealthLow` destructor con borrado | 0x5FF4E0 | 30 | S |
| 2 | `CEventHealthReallyLow::Constructor` | 0x5FF500 | 18 | S |
| 3 | `CEventHealthReallyLow` cuerpo del destructor | 0x5FF520 | 11 | S |
| 4 | `CEventHealthReallyLow::CloneEditable` | 0x5FF550 | 80 | V |
| 5 | `CEventHealthReallyLow` destructor con borrado | 0x5FF5A0 | 30 | S |
| 6 | `CEventLowAngerAtPlayer::Constructor` | 0x5FF5C0 | 18 | S |
| 7 | `CEventLowAngerAtPlayer` cuerpo del destructor | 0x5FF5E0 | 11 | S |
| 8 | `CEventLowAngerAtPlayer::CloneEditable` | 0x5FF610 | 80 | V |
| 9 | `CEventLowAngerAtPlayer` destructor con borrado | 0x5FF660 | 30 | S |
| 10 | `CEventHighAngerAtPlayer::Constructor` | 0x5FF680 | 18 | S |
| 11 | `CEventHighAngerAtPlayer` cuerpo del destructor | 0x5FF6A0 | 11 | S |
| 12 | `CEventHighAngerAtPlayer::CloneEditable` | 0x5FF6D0 | 80 | V |
| 13 | `CEventHighAngerAtPlayer` destructor con borrado | 0x5FF720 | 30 | S |
| 14 | `CEventOnFire::Constructor` | 0x5FF740 | 18 | S |
| 15 | `CEventOnFire` cuerpo del destructor | 0x5FF760 | 11 | S |
| 16 | `CEventOnFire::GetEventType` (0x3A) | 0x5FF770 | 6 | V |
| 17 | `CEventOnFire::GetLifeTime` | 0x5FF780 | 3 | V |
| 18 | `CEventOnFire::Clone` | 0x5FF790 | 80 | V |
| 19 | `CEventOnFire::GetLocalSoundLevel` (60.0f en ST0) | 0x5FF7E0 | 7 | V |
| 20 | `CEventOnFire` destructor con borrado | 0x5FF7F0 | 30 | S |
| 21 | `CEventEscalator::Constructor` | 0x5FF820 | 18 | S |
| 22 | `CEventEscalator::GetEventType` (0x4A) | 0x5FF840 | 6 | V |
| 23 | `CEventEscalator::GetLifeTime` | 0x5FF850 | 3 | V |
| 24 | `CEventEscalator` cuerpo del destructor (`jmp 0x4AC010`) | 0x5FF860 | 5 | S |
| 25 | `CEventEscalator::GetEventPriority` (0x38) | 0x5FF870 | 6 | V |
| 26 | `CEventSeenCop::GetEventPriority` (0x15) | 0x5FF880 | 6 | V |
| 27 | `CEventOnFire::GetEventPriority` (0x42) | 0x5FF890 | 6 | V |
| 28 | `CEventAcquaintancePedHateBadlyLit::GetEventPriority` (0x19) | 0x5FF8A0 | 6 | V |
| 29 | `ShouldLoadSunbatheAnims` (funcion libre; el listado decia CTaskComplexSunbathe) | 0x632190 | 90 | S |
| 30 | `CTaskComplexSunbathe::CreateSubTask` | 0x638290 | 725 | S |
| 31 | `CTaskComplexUseEffect` destructor con borrado (el listado decia CTaskComplexSunbathe) | 0x638570 | 30 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`).

## Aplicacion del lote sobre main
El cherry-pick de 77643863bc dio conflicto en `docs/hooks.csv` (se conservan las filas de main y se anaden las 32 del lote), `source/InjectHooksMain.cpp`, `EventHealthLow.cpp/.h` y `EventSeenCop.cpp/.h`: esos archivos tocan trabajo del lote 196A (que sigue sin validar en la rama y no esta en main). Se resuelve conservando solo lo del lote 197A: `CEventHealthLow` no declara `Constructor` (0x5FF440) ni engancha el cuerpo 0x5FF460 (queda como funcion propia sin hook, que llama al cuerpo enganchado 0x4AC480; el EXE corre su codigo real) y `CEventSeenCop` solo gana `GetEventPriority` (0x5FF880) en el `.cpp`.

## Correcciones de fuente
- `CTaskComplexSunbathe::CreateSubTask` (0x638290): el codigo del lote construia las subtareas con `::new (memoria) CTaskSimple...(...)` (constructor de C++ directo) y devolvia el puntero a la memoria. El EXE llama a los constructores enganchados 0x48E750, 0x61ABE0 y 0x61A900 y devuelve SU EAX (nulo sin memoria). Ahora usa los cuerpos enganchados (`Constructor` / `Constructor2`, con `friend class CTaskComplexSunbathe` en `TaskSimplePause.h`, `TaskSimpleRunTimedAnim.h` y `TaskSimpleRunAnim.h`) y devuelve su resultado. Corregido por revision estatica ANTES de la primera corrida del banco (el banco falsea esos constructores en la direccion del EXE y en el cuerpo del hook; con el constructor directo habria registrado diferencias; no se comprobo sin la correccion).
- La primera corrida (083305-677144, 524288 pares) pasaron las otras 31 funciones y CreateSubTask dio 1579/16384 diferencias, TODAS por el banco y no por la fuente: el azar de la DLL (`CGeneral::GetRandomIntInRange`) es codigo en linea que llama a `CGeneral::GetRandomNumber`, no al hook 0x407180, asi que el falso de 0x407180 nunca se ejecutaba en la DLL. Se rediseno: falso de rand() en 0x821B1E (EXE) y en `CGeneral::GetRandomNumber` (DLL), el 0x407180 del EXE corre real (hook de la DLL apagado) y el oraculo modela la conversion (`rand & 0xFFFF`, por 2^-15, por el rango redondeado a 24 bits, truncada), con el segundo `GetTimeLeft` limitado a magnitudes cuya conversion cabe en 32 bits. Sin cambios de fuente por ello.
- Sin diferencias en las demas funciones: `ShouldLoadSunbatheAnims` (SSE de la DLL contra x87 del EXE) coincide en los 16384 casos (umbral 0.04 y sus ulps, NaN, infinitos, denormales).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook: CEvent::operator new 0x4B5620 y delete 0x4B5630, constructores 0x4ABFC0 (CEvent) y 0x4AC450 (CEventEditableResponse), cuerpos 0x4AC010 (~CEvent) y 0x4AC480 (~CEventEditableResponse), CTask::operator new 0x61A5A0 y delete 0x61A5B0, cuerpo 0x632270 (CTaskComplexUseEffect), constructores 0x48E750, 0x61ABE0 y 0x61A900, GetTimeLeft 0x62EB80 y ShouldLoadSunbatheAnims 0x632190 (esta, otra funcion probada del lote: `Rt`), y los cuerpos de destructor 0x5FF520, 0x5FF5E0, 0x5FF6A0 y 0x5FF760 (otras funciones probadas, `Rt`). Sin hook en este lote y reales: el cuerpo 0x5FF460 de CEventHealthLow (lo valida el lote 196A; el EXE corre su codigo, la DLL su funcion propia) y el 0x407180 del EXE (con rand() 0x821B1E falso; en la DLL, CGeneral::GetRandomNumber con el mismo falso). Memoria global tocada y restaurada: el jugador en foco 0xB7CD74 y la tabla de jugadores 0xB7CD98 (ShouldLoadSunbatheAnims: dos peds y dos vehiculos sinteticos). La subtarea de CreateSubTask usa una vtable propia (ranura 4 = GetTaskType falso; las demas anotan una llamada inesperada). 16 familias de 1024 casos por funcion (`Fixtures.hpp`); `python -I fixture_selfcheck.py` pasa los 524288 casos por el oraculo con cobertura minima 50 por marca y funcion. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn197A` (la corrida dura unos 28 minutos de juego; con un envoltorio propio y un solo candado para la principal y las dos rondas negativas).

## Prueba negativa
`mutate.py` (32 mutaciones por ronda, condicionadas a `this`/`self` desalineado salvo las dos de `ShouldLoadSunbatheAnims`, que cambian `>` por `>=` y la constante 0x3D23D70B por 0x3D23D70A). **Las 32 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261115-095332-980545` (23612 de 65536 pares con diferencias, 2048 casos por funcion) y ronda 2 `20261115-095724-586490` (23119 de 65536); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y sonda buena recompilada para regenerar el manifiesto. Incidencia: una segunda copia del envoltorio de sesion arranco una segunda corrida principal al terminar la primera; se cerro el GTA propio y se descarto (sin efecto en los resultados).

## Fuera de los fixtures
`this` nulo o crudo, jugadores en foco distintos de 0 y 1, objetos reales construidos por el EXE (eventos, tareas, peds, vehiculos, bloques de animaciones y pools reales), rangos de azar cuya conversion no cabe en 32 bits (el `_ftol` del EXE y el `cvttss2si` de la DLL difieren) y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x5FF860 -> `Fn151A` (banco del cuerpo del destructor de CEventEscalator con `EscalatorDestructorBody` por su cuenta; ahora esa direccion tiene hook: **repetir**) y `CppOnlyMigration`; 0x632190 -> `DependenciesTask11` y `Tasks355Evidence` (texto/parche); 0x638290 -> `Sunbathe355` (usa 0x638290 del EXE como colaborador al validar CreateNextSubTask/CreateFirstSubTask; ahora tiene hook oficial: **repetir**) y `Tasks355Evidence`. El resto de direcciones no aparece citado. Ademas, los bancos que usan como colaboradores los constructores 0x48E750, 0x61ABE0 y 0x61A900 o los cuerpos 0x4AC450, 0x4AC480, 0x4ABFC0 y 0x4AC010 no cambian (no se tocaron esas funciones).
