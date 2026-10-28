<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies25C (grupo g25c de BRIEF_21 / lote 25, biblioteca 2): 20 funciones de RenderWare

PENDIENTE: banco preparado sin ejecutar dentro del juego. No constituye validacion hasta que `run_local.py` apruebe una corrida real. Las comprobaciones sin juego descritas abajo (arnes aislado con los bytes
reales del EXE, `offline/`) son evidencia previa, no sustituyen a la corrida.

## Funciones (todas de biblioteca: RenderWare SDK / plugins / motor; clasificacion y evidencia en `g25c-classification.md`)

| # | Funcion | Entrada | Pares | Colaboradores (falsos trazados en ambos lados) |
| --- | --- | --- | --- | --- |
| 1 | RpWorld::_rpMeshHeaderCreate | 0x758920 | 1024 | `rwmalloc` del motor (RwGlobals falso) |
| 2 | RpWorld::RpBuildMeshGenerateDefaultTriStrip | 0x7591B0 | 1024 | 0x7591D0 |
| 3 | RpWorld::RxNodeDefinitionGetD3D9WorldSectorAllInOne | 0x75E9F0 | 1024 | — |
| 4 | RpWorld::RpWorldSectorRegisterPlugin | 0x761C90 | 1024 | 0x8084A0 |
| 5 | RpWorld::RpWorldSectorRegisterPluginStream | 0x761CC0 | 1024 | 0x8088E0 |
| 6 | RpHAnim::RpHAnimFrameGetHierarchy | 0x7C5160 | 1024 | variable 0xC9B8C8 |
| 7 | RpSkin::_rpSkinDeinitialize | 0x7C8820 | 1024 | — |
| 8 | RpUVAnim::_rpUVAnimCustomDataStreamGetSize | 0x7CC010 | 1024 | — |
| 9 | RpUVAnim::RpUVAnimKeyFrameStreamGetSize | 0x7CC9D0 | 1536 | 0x7CC010 (1280 casos falso, 256 real) |
| 10 | RtAnim::RtAnimAnimationGetNumNodes | 0x7CD4F0 | 2048 | — |
| 11 | RtAnim::RtAnimInterpolatorDestroy | 0x7CD590 | 1024 | `rwfree` del motor (RwGlobals falso) |
| 12 | RtDict::RtDictSchemaDestruct | 0x7CED70 | 1024 | 0x809440 |
| 13 | RtDict::RtDictSchemaGetCurrentDict | 0x7CEEE0 | 1024 | — |
| 14 | RwPlCore::RwStreamWriteInt32 | 0x7ED460 | 1024 | 0x7ECB30 |
| 15 | RwCore::RwCameraSetViewOffset | 0x7EE1A0 | 2304 | 0x7F0910 |
| 16 | RwCore::RwCameraRegisterPlugin | 0x7EE450 | 1024 | 0x8084A0 |
| 17 | RwCore::RwFrameDirty | 0x7F0340 | 1024 | — |
| 18 | RwPlCore::RwMatrixOrthoNormalize | 0x7F1920 | 1024 | 0x7F1930 |
| 19 | RwPlCore::RwEngineRegisterPlugin | 0x7F2BB0 | 1024 | 0x8084A0 |
| 20 | RwPlCore::RwEngineGetPluginOffset | 0x7F2BE0 | 1024 | 0x808470 |

Total 23.296 pares. Todas son cdecl con 0..5 argumentos de pila. Hook de cada una: `RH_ScopedGlobalInstall` (clases `RpWorld`, `RpHAnim`, `RpSkin`, `RpUVAnim`, `RtAnim`, `RtDict`, `RwPlCore`, `RwCore`); el hook se apaga para el lado
original (EXE) y se enciende para el candidato (DLL) con `OriginalDependencies499/HookRoute.hpp` (una ruta por funcion, construidas todas antes de empezar).

## Archivos

- `Fixtures.hpp`: tablas (`kW32`, `kRet`, `kFloat`, `kStep`), numero de casos y `Make(fn, c)`, que genera la fixture de cada caso. Solo C++ estandar: `fixture_oracle.py` lee las tablas de aqui y repite las formulas.
- `Core.hpp`: nucleo compartido por la sonda y el arnes (arena, colaboradores falsos, compuertas ABI, FNSAVE, ejecucion de un lado, bucle de pares). Escribe un unico formato de registro.
- `LiveComparison.hpp`: la sonda dentro del juego (`Dependencies25CLive::Update()`, se engancha en `CGame::Process` con `build_probe.py`).
- `fixture_oracle.py`, `run_local.py`: oraculo independiente y compuertas (ver abajo). `build_probe.py`: compila la sonda.
- `original-disassembly.txt` (las 20 funciones), `callers.dis.txt` (llamadores directos y 8 instrucciones tras cada llamada).
- `offline/`: arnes aislado (`build.py`, `harness.cpp`, `exemem.cpp`, `gen_exebytes.py` + `exebytes.inc`, `mkrsp.py`, `mutations.py`).

## Que se compara en cada par (todo en bits; original == candidato, y ambos == oraculo)

- Memoria: el arena propio completo (4096 bytes con 16 bytes de guarda a cada lado, hash FNV-64 antes y despues). Las 19 funciones que no escriben no pueden cambiarlo; `RwCameraSetViewOffset` solo puede cambiar las dos palabras de
  `viewOffset` (+0x78/+0x7C): se registran las cinco palabras +0x74..+0x87 antes y despues y el hash del resto. Ningun objeto del motor RenderWare ni del juego se toca.
- Valor de retorno (EAX) en todas salvo `RtAnimInterpolatorDestroy` (void); ESP y EBX/ESI/EDI/EBP; semillas distintas en EAX/ECX/EDX antes de la llamada. ECX/EDX al volver solo se registran
  (`original_residue`/`candidate_residue`): ningun llamador del EXE los lee (`g25c-classification.md`, `callers.dis.txt`).
- Traza de colaboradores en orden (tipo y argumentos; un valor dentro del arena se registra como desplazamiento con el bit 32 activo): detecta permutaciones, constantes equivocadas (`0x30502`, `(buildMesh, 0, 1)`, registros 0x8D736C/0x8E222C/0x8E2298...).
- x87 completo: FNSAVE de 108 bytes tras la llamada (TOP, FCW, FSW, FTW y los diez bytes de cada registro ST, incluidos los vacios) y MXCSR. Antes de cada llamada `PrimeX87(seed)` deja en los ocho registros un residuo conocido y DISTINTO por
  caso (ocho doubles normales derivados de `Rand`). `kCompareDeadX87Registers = true` (no se relaja). **Ninguna de las 20 ejecuta instrucciones x87** (ni el EXE ni la DLL; `RwMatrixOrthoNormalize` reenvia a 0x7F1930, que aqui se sustituye
  por un falso sin x87): FIP debe ser el del ultimo `fstp` de `PrimeX87` (en el ASI), igual en los dos lados y sin normalizar.
- Variables globales del EXE sustituidas durante la llamada y restauradas justo despues: `RwEngineInstance` (0xC97B24) apunta a un `RwGlobals` falso del arena solo en `_rpMeshHeaderCreate` y `RtAnimInterpolatorDestroy`; 0xC9B8C8 (desplazamiento HAnim)
  solo en `RpHAnimFrameGetHierarchy`. `globals_restored` y el evento `global_state_final` comprueban que vuelven al valor inicial.
- Vigilados 16 bytes de las 20 entradas, de las 8 direcciones de colaboradores del EXE que no son una de las 20 y de las 7 entradas de envoltorios de la DLL: deben quedar intactos tras cada llamada y los hooks se restauran (`HookRoute::Restore`).

## Colaboradores y falsos

Todo colaborador que la DLL llame directamente tiene `EntryPatch` en la direccion del EXE Y en la entrada de la DLL (`Resolve()` salta los `E9` de enlazado incremental), con el mismo falso (`StubAdd`, `StubAddStream`, `StubGetOffset`, `StubStreamWrite`, `StubSList`,
`StubUpdate`, `StubUV`); 0x7591D0 y 0x7F1930, que la DLL llama por direccion, se parchean una sola vez. `RwMalloc`/`RwFree` del motor no tienen direccion propia: el falso esta en los punteros +0x134/+0x138 de un `RwGlobals` del arena.
`_rpUVAnimCustomDataStreamGetSize` (0x7CC010) es a la vez una de las 20 y colaborador de `RpUVAnimKeyFrameStreamGetSize`: en 1280 de sus 1536 casos se sustituye y en 256 se deja el real (EXE para el original, la reconstruccion para el candidato).
Los falsos devuelven `ret` (EAX), valor propio del caso (nulo, exito, error, signo, desbordes, aleatorio). Los colaboradores pueden estar o no enlazados por otras reconstrucciones (por ejemplo g25e hookea 0x808470): al parchear las dos entradas el banco no depende de ello.

## Fixtures (Fixtures.hpp; fixture_oracle.py las repite)

- Argumentos: en `c % 4 == 0` todos aleatorios y distintos (detectan permutaciones de argumentos); en el resto mitad de `kW32` (extremos enteros, patrones float NaN/inf/subnormal, constantes de RenderWare como 0x30502) y mitad aleatorios.
- `RpHAnimFrameGetHierarchy`: marco en el arena / puntero crudo / nulo, destino aleatorio en el arena, desplazamiento = destino - 4 - marco (unos 170 casos con desplazamiento >= 2^31, es decir con la suma de 32 bits envolviendo).
- `RpUVAnimKeyFrameStreamGetSize`: numFrames con desbordes de `<< 5` (1158 casos) y de la suma (433), 8 valores de retorno del colaborador, 256 casos con el colaborador real.
- `RtAnimAnimationGetNumNodes`: 0..12 nodos x 12 pasos (0,4,...,64) x 14 disposiciones (posicion del bloque de fotogramas, de la info de interpolacion y de la animacion); **cortafuegos contra el bucle dependiente de datos**: pasado el ultimo fotograma
  toda palabra del arena apunta a `pFrames` (el recorrido termina aunque el paso fuera otro), los campos vecinos del paso (+4, +0xC) son pequenos y validos, y el paso 0 solo se usa con 0 nodos.
- `RwCameraSetViewOffset`: 24 x 24 patrones float (+-0, subnormales, +-1, +-FLT_MAX, +-inf, NaN con distinta carga y senalizantes, +-1e30, 1e-30) x 4 solapes (estructura aparte, `offset == &viewOffset`, `&viewOffset.y`, `&viewOffset - 4`: este ultimo
  distingue el orden lee-x/escribe-x/lee-y/escribe-y) x 3 tipos de padre (nulo, en el arena, puntero crudo).
- `RwFrameDirty`: los 256 valores de `privateFlags` x 4 raices (el propio marco en dos posiciones, otro marco en dos posiciones; el +3 del marco no raiz lleva el complemento).

## Compuertas de run_local.py

Igualdad fila a fila (`probe_case.analyzer`), hook registrado y cuerpo en el ASI, colaboradores sustituidos, ninguna coincidencia de valor crudo con el arena, todas las compuertas de ABI/x87/ruta en ambos lados, `fp_difference` vacio, FIP igual,
fixture registrada == oraculo, EAX y traza de CADA lado == oraculo (modelo de `fixture_oracle.expected`, desde el desensamblado), memoria final == inicial (salvo las dos palabras de `viewOffset`, que se comparan con la simulacion exacta del orden de lecturas),
desplazamiento HAnim == oraculo, cobertura minima de cada clase de estres y final con restauracion (hooks, variables globales, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza un registro.

## Ejecucion

```
python -B reversing/Dependencies25C/fixture_oracle.py                 # cobertura declarada de las fixtures
python -B reversing/Dependencies25C/build_probe.py                    # compila la sonda (CMake del proyecto; restaura Game.cpp)
python -B reversing/Dependencies25C/run_local.py                      # corre dentro del juego y analiza
python -B reversing/Dependencies25C/run_local.py --analyze <comparisons.jsonl>
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de salida> Dependencies25C
```
Antes: aplicar `g25c.patch`; anexar las filas de `g25c-hooks.csv.txt` a `docs/hooks.csv` y regenerar el estado (`python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md --update-source-status`); compilar el proyecto real;
`python -B contrib/audit_register_residue.py` (sin cambios). La sonda lee/escribe `dependencies25c-runtime.start` / `dependencies25c-runtime.jsonl` en la carpeta del juego.

## Evidencia previa sin juego (arnes aislado, carpeta `offline/`)

`python -B offline/build.py` compila con las opciones Debug del proyecto (`mkrsp.py` lee `build/source/gta_reversed.vcxproj`, sin tocar `build\`) `harness.cpp` y los OCHO fuentes reales reconstruidos
(`source/game_sa/RenderWare/rw/{rpworld,rphanim,rpskin,rpuvanim,rtanim,rtdict,rwplcore,rwcore}.cpp`) y los enlaza (`exemem.obj` primero, base 0x400000, seccion `.exemem` RWX de 10 MB que cubre 0x600000..0xFFFFFF). El arnes copia los bytes REALES de las 20 funciones
(`exebytes.inc`, extraidos del `gta-sa.exe` original con `gen_exebytes.py`) a sus direcciones verdaderas y las ejecuta frente a las funciones compiladas de los fuentes, con el mismo `Core.hpp`, las mismas 23.296 fixtures, los mismos falsos y el mismo registro que la sonda
(el hook RH y la ruta EXE -> thunk -> DLL no se ejercitan: eso es lo que anade la corrida real). `run_local.py --analyze offline/out/comparisons.jsonl` valida esa corrida contra el oraculo independiente.

| Resultado | Valor |
| --- | --- |
| Pares ejecutados / con diferencias | 23.296 / 0 |
| Oraculo (EAX, traza, memoria, desplazamiento HAnim, viewOffset, coberturas) | acepta el registro |
| `offline/mutations.py` (un error plausible en cada una de las 20 reconstrucciones: constantes, orden de argumentos, desplazamientos, mascaras...) | las 20 mutaciones acumulan diferencias y el analizador rechaza el registro |

## Limites

- `RpBuildMeshGenerateDefaultTriStrip` (0x7591D0) y `RwMatrixOrthoNormalize` (0x7F1930) reenvian a rutinas internas de RenderWare todavia sin reconstruir (1134 y 968 bytes, esta ultima x87): lo que se valida es el reenvio exacto (argumentos, valor de retorno, pila,
  no volatiles, estado x87), con la rutina destino sustituida por un falso. Su reconstruccion es un lote posterior.
- `_rpMeshHeaderCreate`/`RtAnimInterpolatorDestroy` se prueban con asignadores falsos (no se llama a `malloc`/`free` reales); `RtDictSchemaDestruct` con `_rwSListDestroy` falso (el real libera memoria del motor).
- `RtAnimAnimationGetNumNodes` no se prueba con memoria no legible ni con paso 0 y primera cabecera distinta (el EXE y la DLL no terminarian); `RpHAnimFrameGetHierarchy` y `RwFrameDirty` desreferencian sin comprobar nulos, igual que el EXE, y las fixtures
  solo usan direcciones legibles.
- Un valor crudo de una fixture que cayera dentro del arena se confundiria en la traza con un puntero al arena: la sonda lo comprueba antes de empezar (`no_raw_collision`).
