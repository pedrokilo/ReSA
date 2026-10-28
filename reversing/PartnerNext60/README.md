<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerNext60: CTaskComplexPartner::CreateNextSubTask (0x683AD0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerFirstSub31` (hook Virtual, `ret 4`, falsos de `operator new`, objetos de arena), `reversing/PartnerDealAnims31` (falsos con parche de entrada en el EXE y en la DLL), `reversing/PartnerDealSeq31` (constructores reales) y contrato `reversing/_handoff/BRIEF.md`, lecciones del lote 30.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerNext60
# o a mano, siempre en serie:
python -B reversing/PartnerNext60/build_probe.py
python -B reversing/PartnerNext60/run_local.py
python -B reversing/PartnerNext60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerNext60/fixture_oracle.py                                              # «ok»
python -B reversing/PartnerNext60/fixture_mirror.py                                              # compila con cl: arenas/constantes C++ == oráculo + cobertura
python -B reversing/PartnerNext60/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(CreateNextSubTask, 0x683AD0)` en `CTaskComplexPartner::InjectHooks`. `source.patch` contiene SÓLO los cambios de esta función respecto a HEAD (`git apply --check` contra los blobs de HEAD ok); `source-all.patch` es el `git diff HEAD` completo de las cuatro funciones del lote 60 (mismo `TaskComplexPartner.cpp`/`.h`, árbol `D:\Original\wc-6`). Los otros tres hooks pueden estar encendidos en el ASI sin afectar a este banco (sus llamadas se sustituyen por falsos). Además requiere el ASI con `GetPartnerSequence` como virtual pura de la base (cambio de `TaskComplexPartner.h` incluido en `source.patch`; ranura 13 = `_purecall` en la vtable 0x870664). Archivo de arranque en el clon: `partnernext60-runtime.start`; salida `partnernext60-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartner,CreateNextSubTask,0x00000000,1,0,1,V` (original_address 0x00683AD0). Función **compartida** por las vtables 0x870664, 0x8707C4 (Chat), 0x870754 (Deal) y las demás derivadas que no la sobrescriben.

Ya comprobado sin el juego: `fixture_oracle.py` -> `ok` (autoprueba sobre rutas, bytes de los constructores y bordes de la geometría); `fixture_mirror.py` -> `ok` (4096 casos, arenas iguales entre `Fixtures.hpp` compilado y el oráculo, 0 faltas de cobertura); `Core.hpp` compila con `cl` contra cabeceras simuladas; `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `LiveComparison.hpp` ni el proyecto.

## Qué hace la función (desensamblado 0x683AD0..0x6840BE, marco SEH propio, `ret 4`)

`this` en ECX (-> ESI), `ped` en la pila (-> EBP). Detalle completo en la cabecera de `fixture_oracle.py`. `partner = m_partner` (nulo -> 0); `t = FindActiveTaskByType(&partner->intel->taskMgr, m_taskId)` (0x681740); sin `t` o `t->m_partner != ped` -> 0. Se despacha por el BYTE `m_partnerState` (+0x59):
- **6**: `m_requiredAnimsStreamedIn` y `m_updateDirectionCount` != 0 (bytes) -> `--m_updateDirectionCount` y `GetPartnerSequence()` (vtable +0x34).
- **5** sin `m_makePedAlwaysFacePartner` (+0x5D): `0x6822B0` (estado con signo del partner, 0 si no hay tarea; llama a `Find`) >= 5 -> si la tarea por defecto del ped (`m_aPrimaryTasks[4]`) existe, es `TASK_COMPLEX_WANDER` (0x390) y su `GetWanderType()` (vtable +0x2C) es 0: forward del ped (`0x41CCB0`: matriz o `-sin h, cos h` con fsin/fcos), `GetNodeHeadingFromVector(-fx, -fy)` (0x53CDC0), `Wander = CTaskComplexWanderStandard(4, AL, true)` (0x48E4F0), `[wander+0x34] = CTimer::m_snTimeInMilliseconds + 100000` (0x66B150) y `SetTask(wander, 4, 0)` (0x681AF0); estado = 6. Siempre devuelve `CTaskSimpleStandStill(50, false, false, 8.0f)` (0x62F310).
- **5** con `+0x5D` != 0: si `m_firstToTargetFlag` (+0x5A) == 1: estado del partner == 6 -> estado = 6. Si no: geometría con `ped`/`partner` (distancia entre 0.99f y 1.01f -> estado = 6; si no, dirección corregida hacia el punto a 1 m del partner, normalizada con `0x44E480` y escalada a 0.02f cuando su módulo > 0.02f, proyectada sobre right/forward del ped en `ped+0x4D8/+0x4DC`). Devuelve StandStill.
- **4**: el partner en estado 4 o 5 -> banderas `+0x5A` (-1 -> 0 / 1) y estado = 5; si no, banderas (-1 -> 1 / 0) y copia `m_targetPoint` a `t->m_point`. StandStill.
- **3**: subtarea 0x387 -> `CTaskComplexTurnToFaceEntityOrCoord(partner, 0.5f, 0.2f)` (0x66B890); 0x398 -> estado = 4 y StandStill; otra -> 0.
- **2**: subtarea != 0xCB -> 0; `m_point.x == 0 && m_point.y == 0` (sólo igual) -> StandStill; si no estado = 3, word +0x70 = 0 y `CTaskComplexGoToPointAndStandStill(4, &m_point, 0.1f, 0, false, true)` (0x668120).
- **1**: subtarea != 0xCB -> 0; si `m_leadSpeaker` (byte): `0x681FE0(ped, &m_point, &m_targetPoint)` y `t->m_point = m_targetPoint`; estado = 2; StandStill.
- cualquier otro valor -> 0. Las reservas nulas de `operator new` devuelven 0 sin construir (salvo el Wander, ver riesgos).

## Fuente (`source.patch`)

`TaskComplexPartner.cpp`: `RH_ScopedVMTInstall(CreateNextSubTask, 0x683AD0)`, includes (`TaskComplexGoToPointAndStandStill`, `TaskComplexSequence`, `TaskComplexTurnToFaceEntityOrCoord`, `TaskComplexWanderStandard`, `General.h`, `IKChainManager.h`, `X87Math.h`), los ayudantes (`PartnerByte`, `PartnerStateOfPartner` = 0x6822B0, `PartnerTaskOfPartner`, `PartnerForwardOf` = 0x41CCB0 con `TrigSimpleSinCpp/CosCpp`, `PartnerNormalise2D` = 0x44E480, `PartnerNewTask` con `CTask::operator new` + comprobación de nulo, `PartnerCounter70`) y el cuerpo. `TaskComplexPartner.h`: `virtual CTaskComplexSequence* GetPartnerSequence() = 0;` (ranura 13, ya implementada por Chat/Deal/Greet/Shove). `0x681FE0` queda como llamada por dirección del EXE (función no reconstruida, colaborador).

## Fase 2: revisión contra el EXE y discrepancias del borrador

Re-desensamblado 0x683AD0..0x6840BE completo y contrastado rama a rama con el C++ y con el borrador del lote 27. **Discrepancias halladas y corregidas:**
1. El borrador usaba `bool` para `m_leadSpeaker`, `m_makePedAlwaysFacePartner` y `m_requiredAnimsStreamedIn`; el EXE prueba el byte entero (`test al,al`): `PartnerByte`.
2. El borrador llamaba a `CVector2D::Normalise` de la DLL, que **no** es 0x44E480 (la DLL pone `x = 1` con NaN y calcula `Magnitude()`; el EXE normaliza el NaN y sólo pone `x = 1` con la suma <= 0): `PartnerNormalise2D` reproduce el EXE.
3. El borrador usaba `new` simple para StandStill/Wander/GoTo/TurnToFace (la rama de pool agotado depende de `/Zc:throwingNew`); ahora `CTask::operator new` + comprobación de nulo + `::new`. Con el Wander nulo el EXE escribiría en `nulo+0x34`; el C++ simplemente no crea la tarea (divergencia sólo en un fallo del pool que cuelga el EXE; no se prueba).
4. El borrador calculaba la proyección con `ped->m_matrix->GetRight()` directamente; se leen los cuatro floats una vez, en el orden del EXE (`right0 * dx + right1 * dy`, `fwd0 * dx + fwd1 * dy`).
Sin más discrepancias: constantes 0.99f (0x862CD0), 1.01f (0x870750), 0.02f (0x858B38), 0.5f/0.2f (0x86FC7C/0x86FC80), 0.0f (0x858B50), 100000 (0x86FCDC), 0x390/0x398/0x387/0xCB, órdenes de llamada (Find, 6822B0, tipo, GetWanderType, forward, nodo, new, ctor, SetTask).

**Llamadores / residuo de registros.** La ranura 8 (+0x20) la llama el despachador genérico de tareas complejas; el escaneo `scan_calls.py 0x20` marca sólo sitios de otras clases (0x6BDE46, 0x485C7C...) que sobrescriben ECX/EDX antes de usarlos. EAX es el retorno (etiqueta en `observed.ret`); **ningún llamador lee ECX/EDX de salida**: sólo se registran.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido. **Sustituidos** por el mismo falso en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto con el hook encendido antes de parchear): `FindActiveTaskByType` 0x681740, `GetNodeHeadingFromVector` 0x53CDC0 (registra los bits de los dos floats y devuelve un valor de 32 bits aleatorio: el EXE usa AX, la DLL `(uint16)`), `CTask::operator new` 0x61A5A0 (registra el tamaño y devuelve la arena 0, la 1 o nulo), `CTaskManager::SetTask` 0x681AF0 (es inline en el encabezado: se parchea la copia fuera de línea que usa la DLL, la misma que engancha RH) y `0x681FE0` (sin reconstruir y sin hook: sólo se parchea la entrada del EXE, que la DLL llama por dirección; escribe tres floats del caso en el tercer argumento). **Reales** en ambos lados (deben ser saltos a la DLL): los constructores 0x62F310 (StandStill), 0x48E4F0 (Wander), 0x668120 (GoTo) y 0x66B890 (TurnToFace): el banco compara el objeto construido palabra a palabra y el oráculo modela sus bytes desde el desensamblado de los constructores (en la cabecera de `fixture_oracle.py`; relleno por reserva = byte del caso). **Ayudantes internos del EXE sin hook** (0x6822B0, 0x41CCB0, 0x44E480, 0x66B150): el banco exige que NO sean saltos y que sus bytes no cambien; el C++ los reproduce y se compara por sus efectos. Las llamadas virtuales (`GetPartnerSequence`, `GetTaskType` de la subtarea y de la tarea por defecto, `GetWanderType`) van por vtables falsas de arena (ranuras no previstas -> evento `trap`). `CTimer::m_snTimeInMilliseconds` (0xB7CB84) se fija por caso y se restaura.
Observado: retorno (etiqueta), eventos ordenados con argumentos, cambios de `this` (estado, banderas, contador, punto), cambios en la tarea del partner, `ped+0x4D8/0x4DC` (`shift`), resto de arenas intactas, los dos objetos reservados (palabras con etiquetas, vtable aparte, guardas) y sus vtables en `alloc_vtables` (EXE == DLL), ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]` incluido el marco SEH propio, x87 normalizado), ruta, `collaborators_dll`, estado y tiempo restaurados.

## Fixtures: 4096 = 16 familias x 256

Partner nulo / `Find` nulo / tarea de otro peatón; estado 6 (streamed y contador en 0/1/no canónico, retorno de la secuencia nulo / arena / opaco); estado 5 sin mirar (estado del partner < 5 incluidos negativos y >= 5, tarea por defecto nula / otro tipo / Wander no estándar / estándar, forward por matriz o por rumbo — 0, ±π, π/2, 1, 10, 100, 1e7, ±inf, NaN, 2^63+ —, reservas nulas de la 2.ª); estado 5 mirando (`+0x5A` == 1 con estado del partner 6 o no; geometría con la distancia alrededor de 0.99f/1.01f ±1 ulp en x e y, coincidentes, ceros con signo, NaN (un solo tipo), inf, 1e9, y correcciones a ambos lados de 0.02f); estados 4, 3 (0x387, 0x398, otros), 2 (punto cero con signo / no cero), 1 (con y sin `m_leadSpeaker`), y estados sin rama (0, 7, 8, 0x80, 0xFF, 0x86, 0x105...); reservas nulas; tiempo con acarreo (0xFFFE795F/0xFFFE7960); relleno por objeto y bytes no canónicos. Mínimos calibrados (~50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- **Arenas** (suma a mano; las comprueba `fixture_mirror.py`): `this` 0x40 + 0x74 + 0x4C = 0x100; ped/partner 0x40 + 0x4E8 + 0x40 = 0x568; matrices 0x20 + 0x48 + 0x20 = 0x88; inteligencias 0x20 + 0x40 + 0x20 = 0x80; tareas falsas 0x20 + 0x30 + 0x30 = 0x80; tarea del partner 0x20 + 0x60 + 0x20 = 0xA0; objetos reservados 0x20 + 0x40 + 0x20 = 0x80; secuencia 0x10 + 0x20 + 0x10 = 0x40. Tamaños de las tareas creadas (argumento de `operator new`) comprobados con `static_assert` en `LiveComparison.hpp` (0x20, 0x38, 0x28, 0x28).
- **Coma flotante.** El EXE usa x87 con la palabra de control a 24 bits; el C++ del archivo (Debug, SSE) redondea cada operación a `float`. Coinciden mientras no haya desbordamiento/denormales intermedios: las fixtures usan coordenadas de hasta 1e9 (los cuadrados no desbordan) y no hay diferencias minúsculas no nulas (cuyo cuadrado sería denormal en float y no en x87); los NaN de entrada son de un solo tipo (0x7FC00000) porque x87 y SSE eligen distinto operando cuando se mezclan NaN con cargas distintas; `inf` produce el NaN indefinido en ambos. El oráculo emula `float32` con dobles (+, −, ×, ÷ y sqrt de float32 son exactos en doble). El `shift` se compara con el oráculo ignorando el signo/carga de los NaN; EXE == DLL se exige estrictamente en bits.
- **fsin/fcos**: el forward por rumbo usa `TrigSimpleSinCpp/CosCpp` (X87Math) frente al `fsin/fcos` del EXE (0x41CCB0); el oráculo usa `math.sin/cos` (error < 1 ulp) con tolerancia de 2 ulp (eventos `node~`), |h| <= 1e7 (la reducción de rango de 66 bits del x87 se aparta de `math` por encima) y los casos |h| >= 2^63 (el operando queda) y ±inf (NaN indefinido) exactos; EXE == DLL estricto.
- El partner es una «entidad» edificio sin IPL (ni temporal ni procedural): `RegisterReference` de `TurnToFace` no hace nada, de modo que el banco no toca `CReferences`. El registro real de referencias lo valida el banco del constructor de TurnToFace.
- GoTo: el oráculo modela la copia incondicional del punto (el relleno por reserva es distinto de cero, así que `[+0x20] != 0` y el vector difiere) y el radio mínimo 0.5f para `moveState` <= 4.
- Si la DLL inlinease `CTaskManager::SetTask`, `FindActiveTaskByType` o `CTask::operator new` (LTCG), el falso del cuerpo de la DLL no se invocaría y faltaría el evento: el banco falla con claridad (no da falso positivo).
- El Wander con reserva nula no se prueba (el EXE se detiene); ver la discrepancia 3.
- La vtable falsa supone las ranuras del EXE (13 `GetPartnerSequence`, 4 `GetTaskType`, 11 `GetWanderType`); si la DLL difiere, aparece un evento `trap`.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.


## Depuración en el worktree de integración (19/10/2026): APROBADO
Defecto del banco: la familia 14 usa el estado 0x105 (byte bajo 5) con `ped_mat` aleatorio; la rama geométrica del EXE (0x683DC7) lee `ped->m_matrix` sin comprobar nulo y fallaba con AV. `Fixtures.hpp` fuerza `pedMat = 1` cuando `(state & 0xFF) == 5`, cara != 0 y primero != 1. Sin cambios en producción.
