<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11768 con evidencia histórica y 673 pendientes de validación en 25 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn219A: 30 funciones del EXE (lote 219A)

Estado: **APROBADO dentro del juego** (17/11/2026, worktree `wv-5` desde f22c514ffe, sin cherry-pick; plantilla `Fn209A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn219a/20261117-102644-571034` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn219A-20261117-102644-571034`, `comparisons.jsonl.gz`). 30 x 16384 = **491520 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia (24 bits) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `2821ce8f80c01a7f341e6246cfc3f78cb80eb262388335be0f55d701393f12b9`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El lote ya estaba integrado con hooks y `validated=0` (`reversing/PendienteValidar/219A.md`). Faltaba el banco; el banco/la revision encontraron **defectos de la fuente**, corregidos (abajo).

| # | Funcion | Entrada | Tipo |
|---|---|---|---|
| 0 | `ListItem_c::~ListItem_c` (cuerpo vacio) | 0x4A8DC0 | S |
| 1-5 | `TList_c`: constructor, destructor vacio, `RemoveItem`, `RemoveHead`, `GetItemOffset` | 0x4A8DD0, 0x4A8DE0, 0x4A8E30, 0x4A8E70, 0x4A9010 | S |
| 6 | `FxFrustumInfo_c::FxFrustumInfo_c` | 0x4A9090 | S |
| 7-15 | vaciados de vector (`_Tidy`) D..L | 0x5EC040, 0x5EC0B0, 0x5EC180, 0x5EC1F0, 0x5EC260, 0x5EC2D0, 0x5EC340, 0x5EC3B0, 0x5EC3E0 | S |
| 16-20 | `fill_n` que devuelve el final (4 bytes x3, 20 y 12 bytes) | 0x5EC070, 0x5EC0E0, 0x5EC110, 0x5EC140, 0x5EC1B0 | S |
| 21 | `vector<T*>::erase(pos)` | 0x5EC220 | S |
| 22-26 | `vector::_Xlen` B..F | 0x5ECAB0, 0x5ECB30, 0x5ECBB0, 0x5ECC30, 0x5ECCB0 | S |
| 27-29 | `vector::insert(pos, n, valor)` (4 bytes A, 20 bytes, 4 bytes B) | 0x5ECF10, 0x5ED1B0, 0x5ED4B0 | S |

Excluidas: ninguna. Saltadas por no ser funciones llamables (ya documentado): los manejadores `catch` 0x5ED0A3 y 0x5ED388.

## Defectos corregidos
- **Hooks repetidos hacia la misma funcion C++** (RH no admite dos hooks a la misma funcion de la DLL): los nueve vaciados (`SArrayTidyStride4`), los tres `fill_n` que devuelven el final de 4 bytes (`SArrayFillNReturnEndStride4`, compartida ademas con 0x4F3D80 del 209A) y los cinco `vector::_Xlen` (`VectorXlen`) compartian funcion. Ahora cada direccion tiene su instancia de plantilla: `SArrayTidyStride4Inst<3..11>`, `SArrayFillNReturnEndStride4Inst<1..3>`, `VectorXlenInst<1..5>` (PedAttractor.cpp y AEUserRadioTrackManager.cpp). El banco comprueba que cada direccion salta a SU cuerpo (`bodies` en el ASI, ruta por funcion).
- **`VectorXlen`**: la primera version de las instancias delegaba en `VectorXlen`, que tiene hook; con ese hook apagado la funcion de la DLL salta al codigo del EXE y la instancia no probaba el C++ (la prueba negativa lo delato: las cinco `_Xlen` no fallaban con la mutacion). El cuerpo vive ahora en `VectorXlenImpl` (sin hook), llamada por `VectorXlen` (0x4F41B0, validada en 209A, comportamiento identico) y por las cinco instancias.
El resto de las fuentes del lote (List_c.cpp, ListItem_c.cpp, FxFrustumInfo.cpp, los tres insert, erase y fill_n 20/12) no necesito cambios.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40. Listas con nodos solapados con la propia lista, cadenas con ciclos/cortes y argumento BYTE con ruido en los bits altos; vectores con cada rama de `insert` (realloc, cola menor/mayor, n = 0, `_Xlen`, 0xCCCCCCC para 20 bytes con `size()` real); `erase` con la salida solapada con el propio vector. Colaboradores: falso identico en la direccion del EXE y en el cuerpo de la DLL (operator delete/new, `assign`, `runtime_error(const string&)`, `_CxxThrowException`, fill_n, envoltorios de copia, fill_n que devuelve el final, fill, copy_backward y `_Xlen` de cada instancia; `FE` = solo en la direccion del EXE cuando la DLL los llama por ella). Corren reales con su hook apagado: `uninitialized_copy` 0x5EAE10/0x5EAE40/0x5EAE70, `size()` 0x5E9800, los `fill_n` 0x5EB2A0/0x5EB2E0/0x5EB310/0x5EB340/0x5EB390 y `FxSphere_c` 0x4A9F70. Las funciones que no vuelven terminan con una salida no local del falso (se comprueba todo lo anterior al lanzamiento). `python -I fixture_selfcheck.py` pasa los 491520 casos por el oraculo con cobertura de ramas >= 50 por marca. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn219A` (con los timeouts de `run_local.py` subidos a 9000 s; yo use `run_main.sh` / `neg_all.sh`).

## Prueba negativa
`mutate.py` (16 mutaciones por ronda que cubren las 30 funciones: los cuerpos compartidos se mutan una vez; condicionadas a `this` o un puntero de argumento desalineado a 4 bytes, que el juego real no produce). **Las 30 funciones fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261117-091802-163845` (22089 de 61440 pares con diferencias) y ronda 2 `20261117-092216-203500` (18773 de 61440); el lado EXE coincide con el oraculo en todos los pares. Incidencia: la primera pareja de rondas (con el defecto de `VectorXlen` de arriba) dejaba las cinco `_Xlen` sin fallar; se corrigio la fuente, se cambio el ancla de la mutacion y se repitieron las dos rondas y la corrida principal. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
`this` nulo o crudo, listas con enlaces a memoria no mapeada, vectores con punteros incoherentes, `erase` con posicion fuera de [inicio, fin) (el EXE no termina), `insert` con posicion fuera del vector, el desenrollado SEH de los `insert` y de `_Xlen`, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x4A8DC0 -> `Fn124A`, `Fn165A`, `Fn176A`; 0x4A8DD0 -> `Fn146A`, `Fn165A`, `Fn173A`, `Fn186A`, `Fn202A`; 0x4A8DE0 -> `Fn165A`, `Fn173A`, `Fn176A`; 0x4A8E30 -> `Fn184A`; 0x5EC040..0x5EC3E0 (vaciados) -> solo `Fn209A` (README, como defecto latente; su banco no los ejecuta). Los bancos de FxManager_c/Interior/FurnitureManager que ejecutan 0x4A8DC0/0x4A8DD0/0x4A8DE0 deberian repetirse en el pase final (**repetir**: Fn165A, Fn173A, Fn176A, Fn184A). El resto de direcciones no aparece citado.
