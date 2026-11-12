<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10344 funciones reconstruidas; 10344 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn175A: 4 funciones del EXE (lote 175A, cierre de saltadas)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-2` desde main 40696b81d3 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn175a/20261112-100639-486310` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn175A-20261112-100639-486310`, `comparisons.jsonl.gz` de 20,8 MB). 4 x 16384 = **65536 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `91168faff4d8c4144e12a6804c6ae4077884f9cbadb8b94dcaa324847cd8c0ee`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las cuatro tenia hook (codigo y `docs/hooks.csv`) y ninguna estaba envuelta con `plugin::Call*`. Las cuatro son viables en C++ puro; no se salto ninguna. Las clases del listado eran erroneas en las cuatro; los nombres salen del desensamblado (`dumpbin /disasm`), de la vtable 0x870114 leida del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CQuadTreeNode<void*>` cuerpo del destructor, marco SEH (CPtrNodeDoubleLinkPool) | 0x552520 | 114 | `Core/QuadTreeNode.h` | S |
| 1 | `CPatrolRoute::Constructor` (CTaskComplexInvestigateDisturbance) | 0x66D440 | 112 | `PatrolRoute.cpp` (+ `.h`) | S |
| 2 | `CPatrolRoute::Assign`, copia de la ruta (CTaskComplexInvestigateDisturbance) | 0x66D4B0 | 155 | `PatrolRoute.cpp` (+ `.h`) | S |
| 3 | `CTaskComplexFollowPatrolRoute` constructor, vtable 0x870114 (CTaskComplexWanderProstitute) | 0x674930 | 182 | `Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp` (+ `.h`) | S |

4 filas nuevas en `docs/hooks.csv` (`RH_ScopedInstall` / `RH_ScopedNamedInstall` / `RH_ScopedNamedGlobalInstall`).

## Por que 0x552520 estaba sin hacer
Ya tenia funcion propia desde el lote 150A (`DestructorBodyExe`) pero llamaba al destructor generado por el compilador y no tenia hook: su banco la falseaba como colaborador del destructor con borrado 0x404590. El lote 173A solo hizo los `operator new/delete` de los pools de nodos. Ahora `DestructorBodyExe` reproduce el EXE (cuatro hijos leidos de uno en uno; por cada uno no nulo se destruye con el mismo cuerpo y se libera su nodo con `PoolFreeNodeExe`, equivalente a 0x552BC0, leyendo `[0xB745BC]` despues; al final el Flush del EXE 0x552400) y se engancha.

## Revision contra el EXE
- **0x66D440**: `[this] = 0`; `eh vector constructor` 0x821D30 sobre `this+4` (8 entradas de 0x28, constructor de entrada 0x46AE30: bytes +0 y +0x18 a cero) y despues los mismos 16 bytes a cero en linea; devuelve `this`. La DLL escribe directamente el mismo efecto.
- **0x66D4B0** (`ret 4`): copia el contador; si es <= 0 (con signo) no hace mas; si no, por cada i copia 3 dwords de +0x144+12*i y los dos nombres como cadenas C completas (sin limite de 0x18) y repite mientras i < contador releido de la ruta destino.
- **0x674930** (`ret 14h`): base 0x61A3B0; word +0x10 = a; vtable 0x870114; word +0xC = b; `+0x20 &= ~2`; `CPatrolRoute::operator new` (0x41B810, tamano 0x1A4); si hay memoria, 0x66D440 y el EAX que devuelve es el puntero usado; +0x1C = ese EAX; 0x66D4B0(ruta) SIN comprobar nulo (con el pool lleno el EXE se cae); +0x14 = c, +0x20 |= 1, +0x18 = d, words +0x12 y +0xE a cero; devuelve `this`. Tamano de la clase 0x30 (`Clone` 0x674BB0 reserva 0x30). Campos +0x24..+0x2F desconocidos.
- **0x552520**: ver arriba; no escribe memoria y no lee el nivel.

## Banco
Plantilla `Fn155A`. Una funcion por ruta: original = bytes del EXE con el hook apagado; candidato = DLL con solo ese hook. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: 0x552400 (con hook), 0x552BC0 (sin hook; funcion propia `PoolFreeNodeExe`), 0x61A3B0, 0x41B810 y las funciones probadas 0x66D440 y 0x66D4B0 (en el banco de 0x674930). 0x66D440 llama en el EXE a 0x821D30 y 0x46AE30, que se comprueban como codigo del EXE sin hook. 16 familias de 1024 casos por funcion (arbol de nodos A/B/C con hijos compartidos y hasta 3 niveles; contadores 0, negativos, 1..8 y 9..12 con nombres cortos y largos y rutas origen en B, C, A o la misma; reserva nula, EAX del constructor distinto del nodo, palabras altas en a y b). `python -I fixture_selfcheck.py` pasa los casos por el oraculo.

## Fuera de los fixtures
Ver `scope_note.txt`.

## Bancos antiguos que citan las direcciones (no repetidos)
0x552520: `Fn150A` (colaborador sin hook, ahora con hook: hay que repetirlo), `Fn153A`. 0x66D440 y 0x66D4B0: ninguno. 0x674930: `Fn170A`.

## Prueba negativa
Dos rondas (`mutate.py`, una mutacion por funcion y ronda, condicionadas a `this` desalineado; `FN175A_CASES=2048`): **las 4 funciones fallan en ambas** (`negative-runs/`): ronda 1 `20261112-103812-465857` con 3421 de 8192 pares con diferencias (DestructorBody 1098, Constructor 663, Assign 786, ConstructorExe 874), ronda 2 `20261112-105156-435627` con 2632 (1070, 570, 389, 603); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas (SHA256 igual a las copias buenas): `QuadTreeNode.h` bb268e40..., `PatrolRoute.cpp` 389e4829..., `TaskComplexFollowPatrolRoute.cpp` f34b0b52...
