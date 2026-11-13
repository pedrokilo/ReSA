<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 899 grupos / 10469 funciones reconstruidas; 10469 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn173A: 21 funciones diminutas del EXE (lote 173A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-3` desde main 7f2cac1f99 + los cambios del lote; plantilla `Fn171A`, con `Fn148A` para las instancias de `CLinkList`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn173a/20261112-005552-998575` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn173A-20261112-005552-998575`, `comparisons.jsonl.gz`). 21 x 16384 = **344064 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `a323b1f0e630679f97a8f0d4ff630b87e65551504253db854ab4619693024c55`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 28 direcciones. Ninguna tenia hook ni envoltura `plugin::Call*` (codigo, `docs/hooks.csv`, `git worktree list`). Se reconstruyen 21 funciones (las direcciones 0x4C4910 y 0x4C4923 son fragmentos de una sola funcion); 6 direcciones se saltan (ver abajo). Las clases del listado eran en su mayoria incorrectas: los nombres reales salen del desensamblado.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CPedList::FillUpHoles` (CPedList) | 0x699E20 | 179 | S |
| 1 | `CPointList::Clear` (CPedList) | 0x699F00 | 24 | S |
| 2 | `CLinkList<AlphaObjectInfo>::Shutdown` (CVisibilityPlugins) | 0x732D60 | 12 | S |
| 3 | `CLinkList<CPed*>::Shutdown` (CVisibilityPlugins) | 0x732D80 | 12 | S |
| 4 | `CLinkList<AlphaObjectInfo>::InsertSorted` | 0x733910 | 115 | S |
| 5 | `CLinkList<AlphaObjectInfo>::Clear` | 0x733F40 | 63 | S |
| 6 | `CreateDebugFont` (vacia, FINAL 0) | 0x734610 | 1 | S |
| 7 | `DestroyDebugFont` | 0x734620 | 1 | S |
| 8 | `FlushObrsPrintfs` | 0x734640 | 1 | S |
| 9 | `CDamageAtomicModelInfo::CreateInstance(RwMatrix*)` (fragmentos 0x4C4910 de 24 B y 0x4C4923 de 48 B, mas el `jmp 0x4C44D0` de 0x4C4953) | 0x4C4910 | 24+48 | V |
| 10 | `CDamageAtomicModelInfo::CreateInstance()` | 0x4C4960 | 103 | V |
| 11 | `CDamageAtomicModelInfo` cuerpo del destructor | 0x4C55D0 | 7 | S |
| 12 | `CLodAtomicModelInfo` cuerpo del constructor (el listado decia CDamageAtomicModelInfo) | 0x4C55E0 | 28 | S |
| 13 | `CWaterLevel::SyncWater` | 0x6E76E0 | 11 | S |
| 14 | `CPtrNodeSingleLink` operator new (CPtrNodeDoubleLinkPool) | 0x552380 | 11 | S |
| 15 | `CPtrNodeSingleLink` operator delete | 0x552390 | 41 | S |
| 16 | `CPtrNodeDoubleLink` operator new | 0x5523C0 | 11 | S |
| 17 | `CPtrNodeDoubleLink` operator delete | 0x5523D0 | 48 | S |
| 18 | `CAnimBlendSequence::CompressQuaternion` (CAnimBlendHierarchy) | 0x4CFA30 | 83 | S |
| 19 | `CAnimBlendSequence::DecompressQuaternion` | 0x4CFA90 | 91 | S |
| 20 | `CAnimBlendSequence::DecompressTranslation` | 0x4CFB10 | 70 | S |

Tipo V = `RH_ScopedVMTOverloadedInstall` (fila `0x00000000` en `docs/hooks.csv`). 21 filas nuevas en `docs/hooks.csv` (sin `sync_docs`/`hooks2md`).

## Funciones saltadas
- **0x617FC0 (102 B) y 0x618030 (103 B)** (`IKChain_c`) y **0x6E3EB0 (105 B) y 0x6E3F20 (106 B)** (`WaterCreature_c`): constructor y destructor de una clase con dos miembros construidos y un array de objetos (0x20 de 0x58 bytes; 0x80 de 0x34 bytes) con marco SEH (`fs:[0]`, tablas 0x83F061 / 0x83F091 / 0x848854 / 0x848884) que llaman a los iteradores de vectores del CRT del EXE (0x821D30, 0x821E02) con los callbacks 0x617850/0x617860 y 0x6E3E30/0x6E3E50 y a los constructores de miembros 0x4A8DD0 / 0x4A8DE0. En C++ el compilador generaria sus propios iteradores y marcos en la DLL: la secuencia de colaboradores (un falso identico en el EXE y en la DLL) no se puede reproducir, y las clases no estan definidas en el codigo. No se ha fabricado un envoltorio.
- **0x6E67A0 (208 B)** (`CWaterLevel`, D): marco SEH con una `CMatrix` temporal (constructor 0x59C050, operacion 0x59C890, destructor 0x59ACD0), puntero global 0xC1703C y comparaciones x87 con NaN; no se identifico a tiempo la semantica ni los llamadores, por lo que no se reconstruye (no se ha comprobado que sea imposible).
- **0x4C4923 (48 B)** no se salta: es un fragmento de la funcion de 0x4C4910 (hook unico).

## Revision contra el EXE (`dumpbin /disasm` sobre el PE)
- **0, 1**: 0x699E20 copia los no nulos de los 30 punteros desde +4 al principio y pone a cero el resto con `rep stosd` sin escribir `m_count`; 0x699F00 pone [this] y 6 dwords desde +0x124 a cero (el constructor 0x6986E0 la llama; su cuerpo C++ la escribe en linea y no se toca). `CPointList::Clear` es nueva.
- **2, 3, 4, 5** (instancias de `CLinkList`, enlaces de 0x14 y 0xC bytes): `Shutdown` llama a `delete[]` del CRT (0x8213AE) y anula el campo; `Clear` pasa los usados a la cabeza de la lista libre; `InsertSorted` avanza con `fld [enlace+8]; fcomp [dato+8]; test ah,1` (avanza con menor o no ordenado). Para esa comparacion exacta (un denormal no se aplana a cero como en el SSE del juego) se anade `SortedDistanceStepsPastCpp` (`X87Math`), y `InsertSorted` de `AlphaObjectInfo` es una especializacion explicita en `VisibilityPlugins.cpp` (declarada en `VisibilityPlugins.h`): no se toca `LinkList.h` (esta en el PCH).
- **6, 7, 8**: `ret`; las funciones C++ ya estaban vacias (`#define FINAL 0`); solo se enganchan en `AppInjectHooks`.
- **9, 10**: con el selector 0xA9B0B0 puesto clonan la plantilla danada (+0x20) con RpAtomicClone 0x749E60, RwFrameCreate 0x7F0410 y RpAtomicSetFrame 0x74BF20; la version con matriz copia 64 bytes con `rep movsd` (se usa `__movsd`) al frame+0x10 antes de asignarlo. Sin selector saltan a 0x4C44D0 (con matriz) o repiten en linea el cuerpo de 0x4C4530 (AddRef 0x4C4BA0, clon, frame, SetFrame, RemoveRef 0x4C4BB0); la DLL llama a `CAtomicModelInfo::CreateInstance`, que corre real en el banco (hook apagado).
- **11, 12**: `mov [ecx],0x85BB9C` y el cuerpo del constructor de `CLodAtomicModelInfo` (0x4C4A60 con hook; dos words a cero en +0x20/+0x22; vtable 0x85BC70; devuelve this).
- **13**: `[0xC228A4] = [0xB7CB84]` (`SyncWater`, ya existia sin hook).
- **14-17**: operator new = `mov ecx,[pool]; jmp New` (0x552240 / 0x5522E0, con hook, falsos); operator delete = liberacion en linea (indice por `>>3` o `/12` con signo, `|= 0x80`, baja el cursor con signo). `operator delete` de los nodos pasa a recibir un solo argumento (cdecl, como el EXE; el tamano se comprueba en el `operator delete` de la clase) y `CPtrNodeDoubleLinkPool::Delete` es nueva, sin comprobar el estado del slot (como la del pool simple).
- **18-20**: funciones cdecl sin `this`: x4096 y `_ftol` (64 bits) a word; int16 por 1/4096 (x4); int16 por 1/1024 (x3). Funciones estaticas de `CAnimBlendSequence` (el listado decia CAnimBlendHierarchy).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook (0x74BF20, 0x4C4BA0, 0x4C4BB0, 0x4C44D0 (V), 0x4C4A60, 0x552240, 0x5522E0) y solo en la direccion del EXE de los que la DLL llama por ella (0x8213AE, 0x749E60, 0x7F0410). Memoria global tocada y restaurada: selector 0xA9B0B0, punteros 0xB74484/0xB74488, 0xC228A4 y 0xB7CB84 (comparada con `gchanges`). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 344064 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn173A`.

## Prueba negativa
`mutate.py` (21 mutaciones por ronda, condicionadas a algo que el juego real no produce). **Las 21 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-022854-940685` (22123 de 43008 pares con diferencias) y ronda 2 `20261112-023344-718081` (20978 de 43008); el lado EXE coincide con el oraculo en todos los pares. Incidencia: una primera ronda 1 (mutacion de `InsertSorted` que corrompia la lista) tumbo el juego tras 8205 pares; se sustituyo por una mutacion no destructiva y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas). El manifiesto de la sonda se regenero con los hashes de las fuentes buenas.

## Fuera de los fixtures
`this` nulo o crudo, listas con enlaces corruptos, pools y nodos reales construidos por el EXE (nodos desalineados o fuera del pool, tamanos de `new` distintos de 8 y 12, nodos ya libres), objetos RenderWare reales, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x699F00 -> `Fn113A`, `Fn118A` (README), `Fn142A` (banco del constructor de `CPointList`: **repetir**); 0x734610/0x734620/0x734640 -> `Game22`, `AppBatch`, `GameIdle`, `HookUnlockGame` (bancos de partida que pasan por esas funciones vacias: **repetir**); 0x5523C0 -> `CEntB`; 0x617FC0 (saltada) -> `Fn124A`. Ademas, `Fn131A`, `Fn150A` y `Fn148A` usan pools de nodos, `CQuadTreeNode` y `CLinkList`: la firma de `operator delete` de los nodos y `CPtrNodeDoubleLinkPool::Delete` cambiaron. El resto de direcciones no aparece citado.
