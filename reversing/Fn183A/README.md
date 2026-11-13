<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 900 grupos / 10514 funciones reconstruidas; 10514 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn183A: 19 funciones del EXE (lote 183A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-3` desde main 5ca991e137 + el commit 89eaf43558 del lote; plantilla `Fn181A`, con `Fn148A`/`Fn171A`/`Fn173A` para las instancias de `CLinkList` y los destructores con borrado). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn183a/20261112-232632-976843` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn183A-20261112-232632-976843`, `comparisons.jsonl.gz` de 43,7 MB; 639 MB sin comprimir). 19 x 16384 = **311296 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `f7307a997f5632f435e08f11af47a4b57ab5e2e97499bdbf68ff3ac743a2fab6`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El banco **no encontro diferencias**: ninguna fuente de produccion se corrigio ni se excluyo ninguna funcion. Unico cambio de fuentes respecto al commit del lote: dos funciones de acceso para el banco (`Lote183ABody_CTaskSimpleSetKindaStayInSamePlace` y `Lote183ABody_CTaskSimpleDuckToggle`, como las `Lote171ABody_*`). Las clases del listado de entrada eran en su mayoria incorrectas; los nombres reales salen de las vtables y del desensamblado (ver `reversing/PendienteValidar/183A.md`).

| # | Funcion | Entrada | Bytes |
|---|---|---|---|
| 0 | `CLinkList<CAnimBlendHierarchy*>::Shutdown` | 0x4D40A0 | 12 |
| 1 | `CLinkList<CAnimBlendHierarchy*>::Insert(T const&)` | 0x4D40C0 | 64 |
| 2 | `ConvertBoneTag2BoneName` (RpAnimBlend) | 0x4D56F0 | 249 |
| 3 | `CPad::IsEnterJustPressed` | 0x4D5980 | 46 |
| 4 | `CPad::IsStandardKeyJustPressed` | 0x4D59B0 | 40 |
| 5 | `CPad::IsCrossPressed` | 0x4D59E0 | 23 |
| 6-13 | destructores con borrado de `CTaskSimpleHitFromFront/Back/Left/Right` y `HitByGunFromFront/Rear/Left/Right` (cuerpo = salto a 0x61A6F0) | 0x637300 .. 0x637450 | 30 c/u |
| 14 | `CTaskSimpleStandStill` destructor con borrado | 0x637480 | 30 |
| 15 | `CTaskSimpleSetStayInSamePlace` destructor con borrado | 0x6374A0 | 30 |
| 16 | `CTaskSimpleSetKindaStayInSamePlace` destructor con borrado | 0x6374C0 | 30 |
| 17 | `CTaskSimpleDuckToggle` destructor con borrado | 0x6374E0 | 30 |
| 18 | `CTaskSimpleDie` destructor con borrado | 0x637500 | 30 |

Los ocho saltos puros 0x637320 .. 0x637470 (`jmp 0x61A6F0`) no se enganchan (documentado en el informe del lote); el EXE los recorre de verdad en el banco.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (cuerpos 0x61A6F0, 0x62F360, 0x62F5D0, 0x62FB40, CTask::Destructor 0x61A3A0, CTask::operator delete 0x61A5B0) y solo en la direccion del EXE para el operator delete del CRT 0x8214BD (Shutdown; la DLL lo alcanza por el salto de 0x8213AE, observacionalmente identico). Los cuerpos 0x62F650 y 0x62F6D0 (`mov [ecx],vtable; jmp 0x61A3A0`) corren REALES: el EXE con su hook apagado y la DLL con su funcion local; el falso va en CTask::Destructor. Memoria global tocada y restaurada: palabras de Enter / extenter de NewKeyState y OldKeyState (0xB733CC, 0xB7315C, 0xB733EA, 0xB7317A, con su vecina) y las 256 palabras de teclas estandar de ambos estados (0xB731A8, 0xB72F38), comparadas con `gchanges`. Particularidades: `Insert` se prueba tambien con listas torcidas (la cabeza usada apunta al enlace libre, auto-enlaces) y con la lista desalineada, y el oraculo reproduce el orden y las RELECTURAS del EXE; `ConvertBoneTag2BoneName` se compara por el TEXTO de la cadena (punteros distintos en EXE y DLL) con etiquetas de 32 bits (0x10009, -1, 0x1000012D...), y su tabla sale de `derive_bonenames.py` (lectura directa del EXE original); las funciones de teclas usan palabras con el byte bajo cero (0x100, 0xFF00...), basura en los bytes altos del argumento de pila y `this` nulo o crudo. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 311296 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn183A` (con timeout de `run_case` subido a 9000 s).

## Prueba negativa
`mutate.py` (19 mutaciones por ronda, condicionadas a algo que el juego real no produce). **Las 19 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-234641-456305` (14890 de 38912 pares con diferencias) y ronda 2 `20261113-000049-852345` (9677 de 38912); el lado EXE coincide con el oraculo en todos los pares. Incidencia: en la primera version de la ronda 2 las mutaciones de `IsEnterJustPressed` e `IsStandardKeyJustPressed` comparaban un `int16` con 0xFFFF / 0x8000 (siempre falso) y no fallaban; se corrigieron (conversion a `uint16`) y la ronda 2 se repitio entera (la corrida descartada no se archiva). Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
`this` nulo o crudo, listas con punteros fuera de las arenas, `links` reales liberados, estados reales de teclado, objetos de tarea reales construidos por el EXE y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada, registros ni inventarios): 0x4D5980 y 0x4D59B0 -> `RequestedPadDistanceTrainNop`, `...Hex`, `...HexV2`, `RequestedPadMutationRemaining3`, `RequestedPadSourceProposals`, `RequestedValidation20261008` (solo comentarios / copias de `Pad.h`); 0x4D59E0 -> los mismos mas `RequestedPadContext9`, `RequestedPadSelectors`, `RequestedPadShoulderInteraction` (**repetir** los que la ejercitan: Context9, Selectors, ShoulderInteraction). El resto de direcciones no aparece citado. `IsEnterJustPressed` pasa de inline a funcion en `Pad.cpp`.
