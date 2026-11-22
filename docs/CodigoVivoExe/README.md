# Código vivo del EXE: auditoría, decisiones y plan de retirada

Rama `codigo-vivo` (desde `origin/main` a23d661f77, 21/11/2026). Objetivo del proyecto: un ejecutable de GTA San Andreas que no cargue el `gta_sa.exe` original. Este documento audita **qué código del EXE sigue «vivo»** (ni el C++ ni un enganche lo reemplaza), de dónde cuelga, qué hacer con cada grupo y en qué orden, y explica las entradas del binario que el catálogo no nombra. No se reconstruyó nada ni se ejecutó el juego: todo es estático (capstone sobre el EXE canónico `72ae59e4…ecc9b`, más el código fuente y los CSV del repositorio).

Archivos: [plan-lotes.md](plan-lotes.md) · [plan.csv](plan.csv) · [funciones-vivas.csv](funciones-vivas.csv) (una fila por función no reemplazada, con grupo, decisión, llamadores, alcance y lote) · [grupos.csv](grupos.csv) · [subgrupos.csv](subgrupos.csv) · [destinos-sin-catalogo.csv](destinos-sin-catalogo.csv) (los 819) · [entradas-sin-catalogar.csv](entradas-sin-catalogar.csv) (las 9.223) · [motivos.csv](motivos.csv) · [referencias-cpp-por-direccion.csv](referencias-cpp-por-direccion.csv) · [huecos.csv](huecos.csv) y [huecos-resumen.csv](huecos-resumen.csv) · [resumen.json](resumen.json).

**Estado de ejecución (rama `vivo-p2`):** P1 (V1–V3) verificada y P2 en curso; **P4 libpng/zlib y D3DX9 (E1, E3): ver [ejecucion-p4.md](ejecucion-p4.md)**; ver [ejecucion-p1-p2.md](ejecucion-p1-p2.md), [cierre.csv](cierre.csv), [cierre-p1.csv](cierre-p1.csv) y [cierre-p2.csv](cierre-p2.csv).

**Cierre V4–V8 (rama `vivo-v48`):** búsqueda por registro de las 114 llamadas indirectas de las funciones vivas conservadas y punto fijo de alcance; D3DX9 (V4), QuickTime (V6) y libpng (V8) cerrados, V5 y V7 parciales (25 abiertas con motivo exacto); ver [ejecucion-v48.md](ejecucion-v48.md) y [busqueda-registro-v48.csv](busqueda-registro-v48.csv).

**Raíces por valor (rama `vivo-raices`):** el experimento de la fase 8 ([fase8-experimento.md](fase8-experimento.md)) demostró que «muerto» no valía para direcciones que el C++ escribe como valor en tablas o callbacks; reclasificación de huecos y de las 379 direcciones citadas sin gancho, cola ordenada de «viva sin hook» ([cola-raices-valor-p6.csv](cola-raices-valor-p6.csv), lotes R1–R3 de la parte P7) y resolución de las 40 condicionadas a P6: ver [reclasificacion-raices-valor.md](reclasificacion-raices-valor.md).

**P3 (rama `vivo-p3`):** S1–S4 sustituidos por C++ propio (18 de las 19 del plan más `0x85742B`; `___updatetlocinfo` queda en la rama del locale), con bancos fuera del juego; ver [ejecucion-p3.md](ejecucion-p3.md) y [cierre-p3.csv](cierre-p3.csv).

**Partes P4 (lotes E2-*, CRT de MSVC sin locale) y P5 (F1–F4, arranque, inicializadores, importaciones) (rama `vivo-p5`):** diseño, cifras y pasos para el ejecutable propio en [fase8-arranque.md](fase8-arranque.md); resultado por función en [fase8-arranque-lotes.csv](fase8-arranque-lotes.csv), [fase8-inicializadores.csv](fase8-inicializadores.csv) y [fase8-funclets-f4.csv](fase8-funclets-f4.csv). **F4 (despachadores de comandos de script 1500–1599 y 2000–2099 reescritos en C++, 44 funclets descartados): ver [ejecucion-f4.md](ejecucion-f4.md).**

## 0. Resumen en diez líneas

1. **Las «5.569 funciones vivas» son 5.569 trozos de 5.279 funciones.** `binary_function_ranges.csv` tiene 19.169 filas, pero son trozos de 17.765 funciones de Ghidra (1.404 funciones tienen cuerpo en varios trozos). Las cifras de aquí son por función única; 5.279 funciones = 535.410 bytes = 5.569 trozos.
2. De esas 5.279, **192 (39.518 bytes) ya tienen enganche** en `main` aunque los CSV de estado no las marcan (142 del CRT, 48 de libpng, 2 del controlador D3D9) y **5.087 (495.892 bytes) están vivas de verdad**: CRT 3.922, RenderWare/D3D 1.037, juego 120, `_rwcseg` 8.
3. **El grueso no es código que haya que reconstruir.** 3.548 son *funclets* de excepciones (`Unwind@`/`Catch@`, 37 KB), 944 son el compilador de shaders de D3DX9 (331 KB) y 18 son el protector: 4.804 funciones (439 KB, el 82 % de los bytes) se **descartan** sin escribir código (sus dueños ya son C++, nadie las llama).
4. Solo **209 funciones (54.834 bytes)** son alcanzables hoy desde una referencia del C++ por dirección o desde la entrada del CRT (0x824570); otras 220 (36 KB) solo cuelgan de punteros en tablas de datos (179 son internas de D3DX9) y 4.658 no las alcanza nada.
5. El trabajo real es pequeño: **183 a ENLAZAR** (135 del CRT, 48 de libpng/zlib), **19 a SUSTITUIR** (6 usercall del controlador D3D9, 6 callbacks de vectores x87 de `_rwcseg`, 3 puentes de QuickTime, 4 numéricas del CRT) y **37 a la fase 8** (arranque del CRT, `_initterm`, thunks de importación).
6. **D3DX9 no necesita ni una función viva:** ningún C++ ni código vivo llama a su interior; los 12 envoltorios de entrada (0x7652AE–0x7654B8) ya están reconstruidos (reenvían a `d3dx9_43.dll`) con 4 enganches **desactivados** (`{.enabled = false}`). Enlazar la biblioteca real equivale a descartar el cuerpo embebido.
7. Fuera de las funciones hay **822.371 bytes de `.text` sin función de Ghidra**: 1.673 inicializadores estáticos de `_initterm` (4 son funciones, 1.669 viven en 4 huecos de 54,8 KB), 1.717 envolturas de excepciones, tablas de saltos, relleno y **270 huecos con 128 KB de código con referencia pero sin función** (parte 6 del plan).
8. **Las 9.223 entradas sin nombre** no son un hueco de reconstrucción: 3.842 son funciones reales ya reemplazadas (falta el nombre en el catálogo), 3.548 funclets, 912 D3DX9, 508 CRT, 151 thunks, 141 de libpng y QuickTime, 91 excluidas del juego, 30 varias, y solo **239 importan para quitar el EXE** (55.968 bytes).
9. **Los 819 destinos originales** ya casi no se citan: en el código actual 760 solo aparecen en el enganche (comentario o `RH_Scoped`), 39 siguen llamándose por dirección a una función ya reemplazada (retirar el puente), 17 son puentes de QuickTime sin referencia (se descartan; EXT1A los retiró) y 3 son `operator new`/`free` del CRT (enlazar).
10. Plan: **67 lotes en 6 partes** (descartes verificables, puentes del C++, sustituir, enlazar, fase 8, huecos). Orden recomendado en la sección 7.

## 1. Método

Herramientas nuevas (reproducibles, estáticas): `contrib/codigo_vivo.py` (reúne los hechos del EXE y del C++; `--cache` guarda un pickle), `contrib/codigo_vivo_informe.py` (clasifica, decide y escribe los CSV), `contrib/codigo_vivo_auditoria.py` (9.223, 819, huecos y puentes) y `contrib/codigo_vivo_plan.py` (lotes, `plan.csv`, `plan-lotes.md` y la columna `lote`).

1. **Funciones.** 17.765 entradas de Ghidra (`binary_functions.csv`) con sus 19.169 trozos (`binary_function_ranges.csv`); el tamaño de una función es la suma de sus trozos.
2. **Estado.** *reemplazada* = `estado=reemplazada` en `docs/JuegoExe/funciones.csv` o `docs/RenderWareExe/funciones.csv` (12.486 funciones); *enganchada* = tiene `RH_Scoped…Install` activo en `source/` o fila en `hooks.csv`/`hooks_notsa.csv` sin estar marcada (192); *viva* = ninguna de las dos (5.087). Los enganches con `{.enabled = false}` no cuentan (4, todos de D3DX9).
3. **Hechos del EXE** (capstone, todas las funciones y los 15.636 huecos del `.text`): llamadas y saltos directos, saltos a mitad de otra función (fragmentos), inmediatos que son direcciones de función (`push offset`, callbacks), accesos por la IAT (214 importaciones), accesos absolutos a `.text` (tablas de índices o saltos), punteros a función en datos (vtables, tablas), inicializadores de `_initterm` (tablas 0x8A5A14–0x8A5A30 y 0x8A4000–0x8A5A10) y estructuras de excepciones `0x19930520` (1.727 `ehfuncinfo`).
4. **Hechos del C++** (`source/` sin comentarios): toda literal hexadecimal que caiga en 0x401000–0x857FFF, separada en *enganche* (dentro de `RH_Scoped…`) y *referencia* (`plugin::Call`, punteros a función, literales). Una referencia que no es enganche es una raíz: el C++ llama a esa dirección del EXE.
5. **Dueño de cada funclet.** El funclet `Unwind@`/`Catch@` aparece en el mapa de limpieza de un `ehfuncinfo`; ese `ehfuncinfo` lo carga una envoltura (`mov eax,tabla; jmp __CxxFrameHandler`) que cae en un hueco sin función y a la que apunta un `push offset` del dueño. Se resolvieron 3.504 de 3.548; 44 quedan sin dueño (lote F4).
6. **Alcance.** Cierre transitivo sobre funciones vivas desde (a) las referencias del C++ y (b) la entrada del CRT más los inicializadores; las aristas que salen de funciones reemplazadas no cuentan (su cuerpo ya no se ejecuta). Un puntero en una tabla de datos se anota aparte (`tabla`), porque la tabla puede ser o no alcanzable.
7. **Grupo y decisión.** Grupo por origen (protector, excepciones, importaciones, D3DX9, libpng/zlib, QuickTime, RenderWare no reconstruible, juego excluido, CRT); dentro del CRT, rol por nombre de Ghidra, por API del sistema que usa (heap, locale, E/S, arranque, hilos, excepciones), por cuerpo (x87 sin llamadas) y, si falta, por los vecinos del grafo (las filas con `(por vecinos)` son heurística). Reglas de decisión: ver 3.

Estado de las referencias: «reemplazada ⇒ el C++ lo hace» viene de los CSV del proyecto, no de una prueba nueva; este informe no certifica equivalencia.

## 2. Cifras

### 2.1 Del mapa al código vivo

| Concepto | Funciones | Bytes |
| --- | ---: | ---: |
| Entradas de Ghidra (19.169 trozos) | 17.765 | 3.724.559 |
| Reemplazadas (CSV del juego y de RenderWare) | 12.486 | 3.189.149 |
| Enganchadas aunque el CSV no lo diga | 192 | 39.518 |
| **Vivas** | **5.087** | **495.892** |
| — código del juego (0x401000–0x74FFFF) | 120 | 29.070 |
| — RenderWare y D3D (0x750000–0x81FFFF) | 1.037 | 376.401 |
| — CRT y bibliotecas (0x820000–0x856FFF) | 3.922 | 89.539 |
| — `_rwcseg` (0x857000) | 8 | 882 |

Alcance de las 5.087 vivas: **209 funciones / 54.834 bytes** alcanzables desde C++ (133, 45.389 B) o desde la entrada del CRT (76 más, 9.445 B); **220 / 36.118 B** solo por puntero en tabla (179 de D3DX9, 35 del CRT); **4.658 / 404.940 B** que no alcanza nada. Solo 39 funciones vivas las cita el C++ directamente por dirección (6 del protector en `ExeProtectionStub.cpp`, 7 de RenderWare, 3 de QuickTime, 14 del CRT, 7 de libpng, 2 excluidas).

### 2.2 Por grupo y decisión (las 5.279 funciones no reemplazadas)

| Grupo | Decisión | Funciones | Bytes | Con ref. C++ | Alcanzables C++ | Alcanzables entrada |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Excepciones (Unwind/Catch) | DESCARTAR (dueño ya C++) | 3.504 | 36.838 | 0 | 0 | 0 |
| Excepciones (Unwind/Catch) | REVISAR (44 sin dueño) | 44 | 592 | 0 | 0 | 0 |
| D3DX9 | DESCARTAR (se enlaza la DLL real) | 932 | 330.595 | 0 | 0 | 0 |
| Protección del EXE | DESCARTAR | 18 | 2.551 | 6 | 7 | 0 |
| Juego excluido (cerrado por inspección) | DESCARTAR | 85 | 19.808 | 2 | 2 | 1 |
| CRT y arranque | DESCARTAR (sin alcance) | 203 | 31.723 | 0 | 0 | 0 |
| CRT y arranque | ENLAZAR (CRT de MSVC) | 135 | 15.244 | 10 | 48 | 75 |
| CRT y arranque | SUSTITUIR (numérico, ExeCrt) | 4 | 830 | 2 | 2 | 2 |
| CRT y arranque | FASE8 (arranque, `_initterm`) | 31 | 3.365 | 2 | 10 | 24 |
| CRT y arranque | CUBIERTA (ya enganchada) | 142 | 25.290 | 32 | 0 | 0 |
| libpng y zlib | ENLAZAR (libs/) | 48 | 32.924 | 7 | 48 | 0 |
| libpng y zlib | DESCARTAR (sin alcance) | 14 | 10.924 | 0 | 0 | 0 |
| libpng y zlib | CUBIERTA | 48 | 9.670 | 10 | 0 | 0 |
| QuickTime/QTML | SUSTITUIR (3 puentes de QTMLRuntime.cpp) | 3 | 78 | 3 | 3 | 0 |
| QuickTime/QTML | DESCARTAR (sin referencia, EXT1A) | 31 | 1.237 | 0 | 0 | 0 |
| RenderWare no reconstruible | SUSTITUIR (6 usercall D3D9, 6 vectores x87) | 12 | 3.407 | 7 | 12 | 0 |
| RenderWare no reconstruible | DESCARTAR (RpSkin, RtAnim, usercall sin llamador) | 17 | 5.656 | 0 | 0 | 0 |
| RenderWare no reconstruible | CUBIERTA (`_rwD3D9CameraClear/BeginUpdate`) | 2 | 4.558 | 2 | 0 | 0 |
| Importaciones y carga retardada | FASE8 | 6 | 120 | 0 | 1 | 1 |

### 2.3 Por decisión

| Decisión | Qué significa | Funciones | Bytes |
| --- | --- | ---: | ---: |
| DESCARTAR | muerto, huérfano, protector o cuerpo de biblioteca que sobra | 4.804 | 439.332 |
| ENLAZAR | lo aporta una biblioteca real (CRT de MSVC 135, libpng/zlib 48) | 183 | 48.168 |
| SUSTITUIR | C++ propio: no hay biblioteca equivalente | 19 | 4.315 |
| FASE8 | lo genera el enlace del ejecutable propio (arranque, `_initterm`, thunks) | 37 | 3.485 |
| REVISAR | funclets sin dueño resuelto | 44 | 592 |
| CUBIERTA | ya tiene enganche; basta retirar el original | 192 | 39.518 |
| **Total** | | **5.279** | **535.410** |

A esto se suman fuera de las funciones: **1.669 inicializadores estáticos** en huecos (decisión FASE8, lote F2) y los 270 huecos de código con referencia (REVISAR, parte 6).

### 2.4 CRT por rol (3.922 funciones vivas del 0x820000+, 3.548 son funclets)

Tras quitar los funclets quedan 373 funciones: arranque 36 (27 FASE8, 9 sin alcance), excepciones 65 (51 ENLAZAR), numérico 87 (83 sin alcance, 4 SUSTITUIR), cadenas y memoria 41 (12), montón 40 (21), locale 42 (20), E/S y stdio 22 (15), estado por hilo y bloqueos 32 (15), inicializadores 4, sin rol 3. Detalle en [subgrupos.csv](subgrupos.csv). El rol «por vecinos» es una heurística: sirve para repartir el trabajo, no para nombrar.

## 3. Decisiones por grupo

| Grupo | Decisión | Razón y qué hacer |
| --- | --- | --- |
| Protección del EXE (18) | **DESCARTAR** | Código del protector (pushad/longjmp, registros); lo decidió el proyecto. Seis tienen una literal en `ExeProtectionStub.cpp`: se retira con el archivo. |
| Excepciones (3.548) | **DESCARTAR** (3.504) | Funclets de limpieza de funciones ya reemplazadas; el compilador genera los de su C++. 44 sin dueño: revisar (F4); casi seguro huérfanos. |
| D3DX9 (944) | **ENLAZAR `d3dx9_43.dll`** (cuerpo embebido: DESCARTAR) | Decisión del usuario. Las 932 vivas no tienen raíz: solo 4 llamadas externas, todas desde 12 envoltorios ya reconstruidos. Activar sus 4 enganches desactivados en `d3dx9math.cpp` (E3) y descartar el resto. |
| libpng y zlib (110 no reemplazadas) | **ENLAZAR** `libs/libpng-1.0.12` + `zlib-1.1.3` | `GTASA_WITH_ZLIB_LIBPNG` ya está ON y 48 funciones tienen hook (LIB2A); 48 vivas son alcanzables desde C++ y hay que cubrirlas; 14 sin alcance se descartan. |
| QuickTime (34) | **SUSTITUIR** 3, **DESCARTAR** 31 | EXT1A retiró QuickTime; 3 puentes aún los cita `QTMLRuntime.cpp`. Sustituir por Media Foundation solo si se quiere la radio personal. |
| RenderWare no reconstruible (31) | **SUSTITUIR** 12, DESCARTAR 17, CUBIERTA 2 | 6 usercall del controlador D3D9 (0x4CB730–0x4CCE60, llamadas desde `D3D9RasterFormat.cpp`), 6 callbacks de vectores x87 de `_rwcseg` (`rwcore.cpp:7490`; ya existen los SSE2). |
| Juego excluido (85) | **DESCARTAR** | 51 `usercall-cubierta` (sus llamadoras son C++ validado), 20 saltos puros, 11 fragmentos, 2 QuickTime, 1 CRT… Dos las cita aún el C++ (0x4A2BB0 en `FxEmitter.cpp`, 0x85742B en `rwcore.cpp`): retirar esas citas. |
| CRT (373 vivas + 142 cubiertas) | **ENLAZAR el CRT de MSVC**; SUSTITUIR lo numérico con redondeo | Montón, locale, E/S, cadenas, hilos y runtime de excepciones los aporta el CRT del compilador (ya hay hooks `ExeCrt::*` que reproducen los del EXE). Las conversiones numéricas (`FUN_0082872C` y vecinas) se quedan en C++ propio con redondeo idéntico hasta probar el CRT nuevo. 203 sin alcance se descartan. |
| Arranque e importaciones | **FASE 8** | `entry` 0x824570, `_cinit`, `_initterm`, `atexit`, thunks `DelayLoad_*` y `RtlUnwind`: los genera el enlazador. WinMain pasa a ser el punto de entrada propio. |
| Inicializadores (1.673) | **FASE 8** | El C++ propio inicializa sus globales por constructor (plan `DatosExe`); no se ejecuta `_initterm` del EXE. |

## 4. Las 9.223 entradas sin nombre catalogado

`catalogue_names` vacío solo significa que `hooks.csv` no tiene esa dirección con nombre. Clasificación ([entradas-sin-catalogar.csv](entradas-sin-catalogar.csv), [motivos.csv](motivos.csv)):

| Motivo | Funciones | Bytes | Importa para quitar el EXE |
| --- | ---: | ---: | --- |
| Función real ya reemplazada (falta nombre) | 3.842 | 786.727 | no: solo falta catalogar (la mayoría del juego) |
| Funclet de excepciones | 3.548 | 37.430 | no (descarte) |
| Biblioteca D3DX9 | 912 | 330.441 | no (descarte; se enlaza la DLL) |
| Biblioteca CRT (0x820000+) | 508 | 76.088 | sí: 135 enlazar, 4 sustituir, 31 fase 8; el resto se descarta o está cubierto |
| Thunk o salto puro | 151 | 592 | no |
| Biblioteca libpng y zlib | 109 | 53.513 | sí: 48 |
| Excluidas del juego (cubierta 51, protector 18, usercall RW 17, QuickTime 2, otras 3) | 91 | 28.915 | no (descarte), salvo 6 usercall RW por sustituir |
| Biblioteca QuickTime | 32 | 832 | 3 |
| Función real viva sin nombre ni reemplazo | 16 | 1.983 | 10: 6 callbacks de vectores x87 (sustituir) y 4 thunks `DelayLoad_` (fase 8); los otros 6 (RpSkin, RtAnim y un callback sin llamador) se descartan |
| Fragmento interno | 12 | 535 | no |
| Thunk de importación | 2 | 12 | fase 8 |
| **Total** | **9.223** | | **239 funciones, 55.968 bytes** |

Es decir: **no hay 9.223 funciones por reconstruir**. 7.390 son nombres que faltan (ya reemplazadas) o funclets huérfanos; 1.561 son bibliotecas (D3DX9, CRT, libpng/zlib, QuickTime); 256 son thunks, fragmentos o excluidas del juego; 16 son funciones reales vivas; y las que importan para quitar el EXE son 239.

### 4.1 Los 819 destinos originales explícitos fuera del catálogo

Lista completa con llamador catalogado, estado actual y decisión en [destinos-sin-catalogo.csv](destinos-sin-catalogo.csv) (los 819 de `reversing/BinaryCoverageAudit/review-source-calls.csv`: 418 de RenderWare, 393 del juego, 8 del CRT; 395 con aristas desde entradas catalogadas, 515 por puntero a función y 301 por `plugin::Call`).

| Estado actual | Decisión | Destinos |
| --- | --- | ---: |
| Reemplazada o enganchada, y el C++ ya no la cita por dirección | RETIRADA (solo queda el enganche) | 760 |
| Reemplazada o enganchada, y el C++ aún la llama por dirección | REEMPLAZAR_LLAMADA (llamar al C++) | 39 |
| Viva, sin referencia vigente (17 puentes de QuickTime, retirados en EXT1A) | DESCARTAR | 17 |
| Viva, citada por `MemoryMgr.cpp` (`operator new` 0x821195, `free` 0x8214BD y 0x8213AE) | ENLAZAR (CRT de MSVC) | 3 |

Más allá de los 819: hoy el C++ cita por dirección (sin contar enganches) **473 entradas de función**: 390 reemplazadas y 44 enganchadas (puentes a retirar, [referencias-cpp-por-direccion.csv](referencias-cpp-por-direccion.csv)), 39 vivas, además de 183 direcciones del rango de código que no son entrada de función (170 en huecos o datos del `.text`, 13 a mitad de función): constantes numéricas o saltos interiores que hay que revisar (lotes B-R).

## 5. Huecos del `.text` (822.371 bytes sin función de Ghidra)

| Categoría | Huecos | Bytes |
| --- | ---: | ---: |
| Relleno de alineación | 9.987 | 79.735 |
| Envolturas de excepciones (`mov eax,tabla; jmp __CxxFrameHandler`) | 1.717 | 35.468 |
| Inicializadores estáticos de `_initterm` (1.669 entradas en 4 huecos) | 4 | 54.804 |
| Tablas de saltos o punteros | 341 | 19.863 |
| Tablas de datos del `.text` referenciadas por el código | 269 | 73.695 |
| **Código con referencia pero sin función de Ghidra** | **270** | **128.357** |
| Sin referencia directa (datos o código no alcanzado) | 3.048 | 430.449 |

Los 270 huecos de código con referencia (juego 161, RenderWare 97, CRT 12) son el único código ejecutable que no figura en ninguna cifra de funciones: parte 6 del plan. Los 3.048 «sin referencia directa» mezclan datos (tablas de comandos, nombres) con código al que solo se llega por registro; no se cuentan como vivos.

## 6. Plan por partes y lotes

Detalle en [plan-lotes.md](plan-lotes.md) y [plan.csv](plan.csv): 67 lotes.

| Parte | Contenido | Lotes | Funciones o direcciones | Riesgo |
| --- | --- | ---: | ---: | --- |
| P1 | Descartes verificables: protector, excluidas, funclets huérfanos, D3DX9, sin alcance de CRT/libpng/QuickTime/RW | 19 | 4.804 | bajo (CRT sin alcance: medio) |
| P2 | Retirar los puentes del C++ por dirección (434 entradas reemplazadas o enganchadas + 183 direcciones por revisar) | 23 | 617 | bajo |
| P3 | Sustituir por C++ propio: QuickTime (3), usercall D3D9 (6), vectores x87 (6), numérico CRT (4) | 4 | 19 | medio-alto |
| P4 | Enlazar: libpng/zlib (48), CRT de MSVC por rol (135), D3DX9 (4 enganches) | 11 | 184 | medio, alto en locale |
| P5 | Fase 8: arranque y `_initterm`, thunks de importación, funclets sin dueño | 4 | 81 + 1.669 inicializadores | alto |
| P6 | 270 huecos de código sin función de Ghidra | 6 | 270 | medio |

### Riesgos

* **«Sin alcance» es una cota.** Se basa en llamadas directas, inmediatos y punteros en datos; no ve llamadas por registro con la dirección calculada en ejecución. Los lotes V5–V8 piden buscar esas llamadas antes de retirar.
* **Reemplazada no es validada.** El estado viene de los CSV y de los enganches del código; no se probó nada. D3DX9 es el ejemplo: 12 envoltorios figuran como reemplazados pero 4 enganches están desactivados.
* **Enganches condicionales.** Se cuentan los `RH_Scoped…Install` del código fuente aunque estén bajo `#ifdef` (por ejemplo `CrtHeap` solo con `GTASA_CRT_HEAP_HOOKS`); la cifra de enganchadas puede ser mayor que la real hasta fijar la configuración.
* **Dos CRT.** Mientras la DLL va sobre el EXE hay dos CRT (el del EXE y el de la DLL); al enlazar uno solo cambian el montón, `rand`, el redondeo x87/SSE y las conversiones de coma flotante. Por eso las numéricas van a C++ propio y la prueba de redondeo precede al enlace.
* **Funclets sin dueño (44) y dueños aproximados.** Cuando la envoltura de excepciones cae en un hueco, el dueño se toma de la función de Ghidra que lo precede; no afecta al descarte (dueños reemplazados) salvo error de atribución.
* **Inicializadores en huecos.** Los 1.669 no son funciones de Ghidra: las funciones a las que llaman figuran en `gap_calls` solo como arista de la entrada; al quitar `_initterm` hay que reproducir lo que construyen (constantes calculadas, constructores de CRT/bibliotecas).
* **Ramas paralelas.** No se tocó `Crt/*` ni las ramas `crt4a-locale`, `new-delete-d3dx9`, `datos-exe`, `enl-png`; los lotes E2 (CRT), E1 (libpng) y E3 (D3DX9) deben coordinarse con ellas, y la medición de datos (`datos_exe.py`) debería excluir las funciones DESCARTAR de «vivas» (muchos «datos compartidos con código vivo» pasarán a huérfanos).

## 7. Orden recomendado

1. **P1 primero y por scripts** (V1, V2, V3; después V4 con E3, y V5–V8 con la comprobación de llamadas por registro): quita 4.804 funciones (439 KB) de la lista viva sin escribir una línea de C++ de juego, y deja la lista real en ~280 funciones.
2. **P2 en paralelo** (B1–B18, B-R1–B-R5): es mecánico y de bajo riesgo; deja de depender de `plugin::Call` a funciones que ya son C++.
3. **E3 (D3DX9)** y **E1 (libpng/zlib)**: no tienen código nuevo; solo configuración y cerrar los hooks que faltan. Coordinar con `new-delete-d3dx9` y `enl-png`.
4. **P3**: S3 (vectores x87, ya hay SSE2), S1 (QuickTime, opcional), S2 (usercall del controlador D3D9, alto), S4 (numéricas del CRT).
5. **E2 (CRT de MSVC por rol)**: decidir primero el toolchain y probar el redondeo; locale con `crt4a-locale`.
6. **P5 (fase 8)**: arranque, `_initterm` y thunks, junto con la entrada propia; F4 al final de la revisión.
7. **P6**: antes de dar por cerrado el código, decidir si los 270 huecos con referencia son funciones, envolturas o datos.

## 8. Reproducir

```
python -X utf8 contrib/codigo_vivo_informe.py [--exe RUTA] [--cache hechos.pkl]   # CSV, resumen.json, 9.223, 819, huecos, puentes
python -X utf8 contrib/codigo_vivo_plan.py                                        # plan.csv, plan-lotes.md y columna lote
```

`codigo_vivo.py --cache` solo guarda los hechos (22 s; sin caché el informe tarda unos 30 s). Las dos salidas son deterministas: se comprobó que una segunda ejecución sin caché da el mismo `resumen.json` y el mismo `funciones-vivas.csv`.
