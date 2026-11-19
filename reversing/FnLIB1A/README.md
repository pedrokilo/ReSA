# FnLIB1A: zlib 1.1.3 y libpng 1.0.12 compiladas desde la fuente (19/11/2026)

Primer lote del hito «EXE independiente» para las bibliotecas de PNG: compilar **zlib 1.1.3** y **libpng 1.0.12** (las versiones que lleva embebido `gta_sa.exe`), mapear sus funciones contra las del EXE y sustituir con la biblioteca compilada las 14 funciones de inflate que el proyecto ya reconstruía a mano (`ZlibInflate`), validándolas par a par contra el EXE.

Resumen:

| | Resultado |
|---|---|
| Bibliotecas compiladas desde fuente oficial | **sí**, las dos, Debug y Release, sin ningún parche a la fuente (los 42 `.c`/`.h` son idénticos byte a byte a los de `D:\Original\descargas`) |
| Opción CMake | `GTASA_WITH_ZLIB_LIBPNG` (por defecto **APAGADA**). El proyecto completo compila con ella encendida y apagada (Debug) |
| Mapa EXE ↔ fuente | `mapa_funciones.csv`: ver cifras en la sección 2 |
| Funciones sustituidas por la biblioteca (detrás de la opción) | las **14** de `ZlibInflate` (`0x7E9430`–`0x7EB260`) |
| Banco EXE vs biblioteca | **14 de 14 funciones sin diferencias** (296.269 casos; mínimo 18.680 pares por función; 0 fallos del oráculo de Python en los dos lados) |
| `validated=1` en `hooks.csv` | **ninguna** (evidencia fuera del juego; ver sección 4) |

## 1. Cómo se compilan

Fuentes mínimas y sus licencias en `libs/zlib-1.1.3/` (zlib: `adler32 compress crc32 deflate gzio infblock infcodes inffast inflate inftrees infutil trees uncompr zutil` + cabeceras; licencia en su `README`) y `libs/libpng-1.0.12/` (los 15 `png*.c`, `png.h`, `pngconf.h`, `LICENSE`, `README`). Sin binarios, sin pruebas (`example.c`, `pngtest.c`, `minigzip.c`), sin `contrib/`, `projects/`, `scripts/`, `msdos/`, ni los `.tar.gz` (siguen sin versionarse en `D:\Original\descargas`; SHA256 de los archivos comprobados por el coordinador: zlib `cae5847b…06f4`, libpng `e2ba46d5…3645f`).

- `libs/zlib-1.1.3/CMakeLists.txt` → biblioteca estática `zlib113`; `libs/libpng-1.0.12/CMakeLists.txt` → `libpng1012` (depende de `zlib113`).
- Mismo runtime que el proyecto (`CMAKE_MSVC_RUNTIME_LIBRARY`: `/MT` en Release, `/MTd` en Debug, heredado del `CMakeLists.txt` raíz).
- Opciones lo más cercanas posible al EXE (MSVC 7.1, optimizado, IA-32 puro): `/O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /Zp8 /W0`. `/arch:IA32` evita SSE/SSE2 (el EXE solo usa x87); `/Gd` = `__cdecl`; `/Zp8` = empaquetado por defecto. Las bibliotecas se compilan **optimizadas también en Debug** (el EXE original está optimizado y la comparación es de comportamiento); se anulan `/RTC1` y `/ZI` heredados con `MSVC_RUNTIME_CHECKS ""`, `MSVC_DEBUG_INFORMATION_FORMAT Embedded` y `set_directory_properties(COMPILE_OPTIONS "")` (`/ZI` no admite `/O2`).
- **Parches a la fuente: ninguno.** Ni K&R, ni `strerror`, ni `_vsnprintf` hicieron falta: MSVC 19.5 (VS 2026, `v145`) compila las dos bibliotecas con `/W0` sin errores (las funciones de `gzio.c`/`pngerror.c` que usan `vsprintf`/`fprintf` entran por las cabeceras del CRT).
- **Un único ajuste de compilación (no de fuente):** `huft_build` es `local` (`static`) en `inftrees.c` y el EXE la tiene como función propia (`0x7EAAB0`, una de las 14). Para poder sustituirla, **solo `inftrees.c`** se compila con `-Dlocal=` (`set_source_files_properties(inftrees.c PROPERTIES COMPILE_DEFINITIONS "local=")`). Efecto: `huft_build` y las tablas `cplens/cplext/cpdist/cpdext` pasan a ser símbolos externos; no hubo colisiones al enlazar.
- Tamaños (`.lib`, Release, con información de depuración embebida): `zlib113.lib` ≈ 318 kB y `libpng1012.lib` ≈ 1.018 kB (sin ella 82 kB y 269 kB). Símbolos de función compilados: 443 (zlib + libpng completas, lectura y escritura; incluye las funciones en línea del CRT que `stdio.h` introduce en cada unidad).

Enlace en el proyecto (`source/CMakeLists.txt`): si `GTASA_WITH_ZLIB_LIBPNG=ON`, `add_subdirectory(libs/...)`, `target_link_libraries(gta_reversed PRIVATE zlib113 libpng1012)` y `GTASA_WITH_ZLIB_LIBPNG=1` como definición. Con la opción encendida, los 14 cuerpos de `source/game_sa/RenderWare/rw/zlibinflate.cpp` (bloques `#ifdef GTASA_WITH_ZLIB_LIBPNG … #else … #endif` al principio de cada función; las líneas `RH_ScopedGlobalInstall` y por tanto `docs/hooks.csv` no cambian) llaman a la biblioteca: `inflate_blocks_reset/new/…/inflate_fast` y `huft_build`. Los prototipos se declaran en el propio `.cpp` con punteros opacos (no se incluye `zlib.h` para no chocar con `Byte/uInt/uLong` del proyecto); `z_stream` (0x38), `inflate_blocks_state` (0x40), `inflate_codes_state` (0x1C) e `inflate_huft` (8) tienen la misma disposición que las `ZInflate*` del proyecto y que las del EXE.

Compilaciones del proyecto (bajo `compile_lock`, `CMAKE_BUILD_PARALLEL_LEVEL=1`, `_CL_=-MP1`; `build_lib1a.sh`, salida `build_lib1a.out`; se hicieron dos veces, la segunda con el código final): **Debug con la opción ON: correcto** (00:55–01:05, `gta_reversed.asi` 50.429.952 B, SHA256 `7854d8b2…de60`) y **Debug con la opción OFF (por defecto): correcto** (01:05–01:12, 50.433.024 B, SHA256 `db703eaf…3957`; `build_lib1a_primera.out` conserva la primera pasada: ON 23:11–23:18 y OFF 23:18–23:25, también correctas). Sin errores de compilación ni de enlace (no hubo símbolos duplicados con las bibliotecas del proyecto). La configuración Release del proyecto no se compiló entera (no hay un `build/` Release en el worktree); sí se compilaron las dos bibliotecas en Release, ver arriba. El `.asi` Debug resultante que queda en `bin/Debug` es el de la opción apagada.

**Corrección de un dato del proyecto.** `zlibinflate.h/.cpp` decían «zlib 1.1.4». El EXE lleva la cadena `inflate 1.1.3 Copyright 1995-1998 Mark Adler` en `0x8841B1` y `deflate 1.1.3 Copyright 1995-1998 Jean-loup Gailly` en `0x883271`; no existe ninguna cadena 1.1.4. Los comentarios se corrigen a 1.1.3 (el código reconstruido era correcto: ver el banco).

## 2. Mapa EXE ↔ fuente (`mapa_funciones.csv`)

Región analizada: `0x7CF600–0x7EB5C0` (libpng + zlib del EXE; `rtpng` de RenderWare está al principio). Se descubrieron **343 funciones** por descenso recursivo desde las 196 de Ghidra (`docs/RenderWareExe/funciones.csv`) más huecos y objetivos de `call` (`exe_funcs.py`, con `capstone`); la fuente compilada aporta 443 funciones con tamaño, llamadas y cadenas (`compilar_objs.cmd` → `/Gy /FAs`; `fuente_lib.py` lee `.obj` y `.asm`).

Método (`mapa2.py`, `mapa3.py`, `mapa_final.py`), de más a menos fiable:
1. **Cadenas únicas** (mensajes de `png_error/png_warning`, nombres de chunk, «1.0.12»): votos ponderados por rareza, mejor emparejamiento mutuo → *alto*.
2. **Anclas por constantes** (`adler32`: BASE 65521 y NMAX 5552) y las **14 de `ZlibInflate`**, ya reconstruidas desde el desensamblado y comprobadas por el banco → *alto*.
3. **Grafo de llamadas**: callees ya emparejados que coinciden con los de la fuente, orden de definición dentro de cada `.c` (el orden de funciones del EXE sigue el del fichero) → *medio* (*alto* si todos los callees coinciden).
4. **Alineamiento por orden entre anclas** (programación dinámica con tamaño, nº de llamadas, uso de x87 y cadenas) → *medio* si la similitud ≥ 0,85 y *bajo* si no.

Cifras (`mapa_funciones.csv`, columna `en_cierre176` = las 176 funciones libpng+zlib que alcanza el código propio según IND0A):

| | Todas (343) | Cierre de 176 |
|---|---:|---:|
| Equivalente **alto** | 41 | 15 |
| **medio** | 104 | 46 |
| **bajo** | 67 | 52 |
| **Sin equivalente claro** | 131 | 63 |

Las dos raíces del cierre (`0x7CF600`, `0x7CF9B0`) **no son libpng**: son `RtPNGImageWrite/Read` de `rtpng` (Criterion/RenderWare), llamadas desde `rtpng.cpp`; se marcan «sin equivalente» y hay que reconstruirlas en C++ aparte. Las «sin equivalente» restantes son sobre todo funciones pequeñas sin cadenas ni llamadas distintivas, fragmentos de la descubierta (la de `0x7D2E00` con tamaño 48 kB es un error del descenso sobre una tabla) y funciones de `pngrutil.c`/`pngrtran.c`/`pngwtran.c`/`pngtrans.c` cuya forma cambia mucho con el compilador. **Diferencias conocidas EXE ↔ fuente:** el EXE se compiló con MSVC 7.1 (la fuente compilada aquí con 19.5): las funciones de `pngrutil.c`/`pngwutil.c` salen 1,5–2,5 veces mayores en el EXE (`png_write_chunk_*` y los `png_crc_*` en línea, conversiones x87 sin optimizar). Las 14 de zlib salen de tamaño casi igual (0,9–1,2x) y de grafo idéntico.

Funciones de zlib **fuera de las 14** que ya se identifican: `0x7E9300` `adler32` (alto), `0x7E1280` `inflateInit2_` y `0x7E1410` `inflate` y `0x7E1840` `inflateSetDictionary` (alto), `0x7E1230` `inflateEnd`, `0x7E18D0` `inflateSync`, `0x7E19F0` `inflateSyncPoint` (medio). `crc32` y `inflateReset` están en la región pero sin equivalente fiable todavía.

Regenerar: `compilar_objs.cmd <dir>` (`/O2`), `python exe_funcs.py`, `python mapa_final.py <dir>` desde esta carpeta (datos de entrada: `datos/cierre_libpng_zlib.csv`, copia de las 176 filas de `FnIND0A/datos/cierre_codigo.csv`).

## 3. Primer lote de sustitución y banco par a par (`bench.cpp`)

**Funciones (14):** `inflate_blocks_reset` `0x7E9430`, `inflate_blocks_new` `0x7E94B0`, `inflate_blocks` `0x7E9570`, `inflate_blocks_free` `0x7EA1C0`, `inflate_set_dictionary` `0x7EA250`, `inflate_codes_new` `0x7EA2A0`, `inflate_codes` `0x7EA2E0`, `inflate_codes_free` `0x7EA9E0`, `inflate_trees_bits` `0x7EAA00`, `huft_build` `0x7EAAB0`, `inflate_trees_dynamic` `0x7EAF60`, `inflate_trees_fixed` `0x7EB0F0`, `inflate_flush` `0x7EB120`, `inflate_fast` `0x7EB260`. Son las 14 de `ZlibInflate` de `hooks.csv` (`validated=0` en las 14).

**Banco.** No necesita el juego: las 14 son funciones puras de enteros (sin x87, sin CRT, sin estado global; solo leen tablas de `.rdata/.data` y llaman a `zalloc/zfree/checkfn` por punteros), así que `bench.exe` (32 bits, enlazado en `0x400000` con la reserva `reserva_imagen.asm` de IND0A que cubre `0x401000–0xCB1000`) **copia en memoria el `gta_sa.exe` original (el archivo no se toca; SHA256 `72ae59e4…ecc9b` impreso al inicio) y llama a sus funciones por su dirección**. Cada caso se ejecuta **dos veces con las mismas entradas y la misma arena de memoria (misma dirección)**: con las funciones del EXE y con las de la biblioteca (mismos prototipos `__cdecl`). Tras cada llamada se compara: valor devuelto, campos de `z_stream` (punteros normalizados), memoria de la arena (estado, ventana, tablas de Huffman, bloques liberados), salida, mensaje de error por contenido y registro de `zalloc/zfree`. El asignador es determinista (relleno `0xCD`, reutilización de bloques liberados del mismo tamaño, huecos de 36 kB antes de cada reserva para que las lecturas fuera de ventana de flujos corruptos sean deterministas) e inyecta fallos de reserva (1–5ª llamada) en ~8 % de los casos. Entradas fuera de dominio (`n = 0` en `huft_build`, que da vueltas 2^32 veces en las dos versiones; flujos raw sin el «byte ficticio» que exige zlib 1.1.3) no se generan.

**Escenarios** (`bench.cpp`; casos de `gen_cases.py`): flujos completos con entradas y salidas en trozos (`inflate_blocks` + `reset/new/free`), longitudes de código de Huffman (`huft_build`, `inflate_trees_bits`, `inflate_trees_dynamic`: completas, incompletas, sobresuscritas, dispersas, de un solo símbolo), tablas fijas, `inflate_flush` con ventana/lectura/escritura/salida aleatorias, `inflate_codes` e `inflate_fast` con estados fabricados (tablas fijas y dinámicas, bits pendientes, ventanas de 300 a 32768) y ciclo de vida de bloques con `set_dictionary` en todos los modos.

**Oráculos independientes** (módulo `zlib` de Python, otra implementación y versión): (a) flujos válidos: SHA-256 de la salida y adler32 (con la ventana de decodificación ≥ la del flujo); clasificación de flujos corruptos/truncados; (b) clasificación de las longitudes de código por la desigualdad de Kraft (completo/incompleto/sobresuscrito) calculada en Python → valor devuelto esperado de `huft_build`, `inflate_trees_bits` e `inflate_trees_dynamic`; (c) modelo de `inflate_flush` escrito en Python desde la especificación de `infutil.c` (bytes copiados, punteros, `avail_out`, adler32 encadenado).

**Relleno indefinido de las tablas.** Cada entrada `inflate_huft` (8 B) tiene 2 bytes de relleno de la `union` (offset +2..+3) que ni el EXE ni la fuente inicializan: contienen basura de pila distinta en cada compilación. Esos 2 bytes por entrada (solo dentro de las regiones de tablas registradas) **se enmascaran**; el resto de la comparación es exacta. El banco cuenta además los pasos en los que el *único* motivo de diferencia es ese relleno (informativo; ver resultados). Se comprobó con dos corridas (enmascarada y cruda) que sin enmascarar solo difieren esos bytes.

Resultados de la corrida completa (`bench.exe`, `full_run.txt`):

| Función | Dirección | Pares comparados | Diferencias | Comprobaciones del oráculo de Python (fallos EXE / biblioteca) |
|---|---|---:|---:|---|
| `inflate_blocks_reset` | `0x7E9430` | 74.718 | 0 | — (0 / 0) |
| `inflate_blocks_new` | `0x7E94B0` | 80.000 | 0 | — (0 / 0) |
| `inflate_blocks` | `0x7E9570` | 2.742.645 | 0 | 8.230 (0 / 0) |
| `inflate_blocks_free` | `0x7EA1C0` | 74.718 | 0 | — (0 / 0) |
| `inflate_set_dictionary` | `0x7EA250` | 18.680 | 0 | — (0 / 0) |
| `inflate_codes_new` | `0x7EA2A0` | 42.741 | 0 | — (0 / 0) |
| `inflate_codes` | `0x7EA2E0` | 64.937 | 0 | — (0 / 0) |
| `inflate_codes_free` | `0x7EA9E0` | 42.045 | 0 | — (0 / 0) |
| `inflate_trees_bits` | `0x7EAA00` | 20.055 | 0 | 18.263 (0 / 0) |
| `huft_build` | `0x7EAAB0` | 120.000 | 0 | 120.000 (0 / 0) |
| `inflate_trees_dynamic` | `0x7EAF60` | 65.151 | 0 | 36.856 (0 / 0) |
| `inflate_trees_fixed` | `0x7EB0F0` | 29.491 | 0 | — (0 / 0) |
| `inflate_flush` | `0x7EB120` | 20.000 | 0 | 20.000 (0 / 0) |
| `inflate_fast` | `0x7EB260` | 27.958 | 0 | — (0 / 0) |

296.269 casos (cada uno con su comparación EXE vs biblioteca), **cero diferencias**; mínimo de pares por función 18.680 (≥ 16.384 exigidos); 20.000 flujos deflate (Python juzga 9.102; 8.230 terminan bien en ambos lados con SHA-256 y adler32 idénticos a los de Python; 2.673 flujos corruptos dan `Z_DATA_ERROR` donde Python da error; 2.024 truncados); 120.000 conjuntos de longitudes de código; 20.000 casos de `inflate_flush` contra el modelo de Python. Archivos: `resultados/corrida_completa.txt` (salida íntegra), `resultados/sha256_casos_y_banco.txt` (SHA256 de los casos, generados con la semilla fija de `gen_cases.py`, y de `bench.exe`). Comandos: `python -I gen_cases.py <dir> 20000 120000 20000` y `bench.exe <dir> <gta_sa.exe>`.

Hallazgos del banco (comportamiento indefinido de zlib 1.1.3 o entradas fuera de dominio; ninguno es una diferencia de lógica):

1. **Lectura fuera de `cpdext[]`/`cpdist[]` con códigos de distancia 30 y 31.** `huft_build` lee `e[*p - s]` y `d[*p - s]` para símbolos de distancia 30/31, pero las tablas de distancia tienen 30 entradas (las de longitudes tienen 31, con centinelas 112). Lo que se lee es lo que haya detrás de la tabla en cada binario: en el EXE `cpdext[30] = 0x23284040` y `cpdext[31] = 0x64492429` (restos de cadenas), de donde `exop = 0x40+0x50 = 0x90` y `0x29+0x50 = 0x79`, exactamente los bytes que difieren de la biblioteca (`0xBF`, `0xC3`). El RFC 1951 prohíbe esos símbolos y ningún compresor los emite. Efecto en el banco: (a) en `huft_build`/`inflate_trees_dynamic` con longitudes no nulas en los símbolos 30/31 (3.068 casos) se compara todo salvo el contenido de la tabla; (b) los escenarios de `inflate_codes`/`inflate_fast` solo generan tablas dinámicas válidas (sin esos símbolos): con ellas hay 0 diferencias; con tablas que sí los tenían, al decodificar un código de distancia 30/31 el resultado difería (82 de 3.000 casos de `codes`, 22 de 3.000 de `fast`, mismo origen). Para flujos válidos, y por tanto para el juego, no hay diferencia.
2. **Relleno de `inflate_huft`**: 2 de cada 8 bytes de cada entrada están sin definir (ver arriba); 312.440 pasos difieren solo ahí.
3. **Byte ficticio**: zlib 1.1.3 necesita un byte más tras un flujo deflate sin cabecera (el banco añade 4 ceros; el EXE y la biblioteca coinciden en ello).
4. **`huft_build` con `n = 0`** da vueltas 2^32 veces en el EXE y en la biblioteca (nunca se llama así).
5. **Artefactos del propio banco corregidos durante la puesta a punto** (no de la biblioteca): entrada sin inicializar leída con `avail_in = 0` (ahora `g_in` se rellena con un patrón), asignador agotado por los huecos, inyección de fallos aplicada a reservas del banco, oráculo de Python que juzgaba flujos corrompidos con ventana menor, un generador de códigos «completos» que producía códigos incompletos (la prueba negativa lo delató: las mutaciones de `inflate_trees_bits` e `inflate_trees_dynamic` no saltaban) y un `codes_free` sin prueba directa en modo START.


**Prueba negativa de 2 rondas** (`mutate.py`, `neg_run.py`, `negative-runs/ronda1.json`, `ronda2.json`): una mutación por función (14 por ronda), activada por la variable de entorno `LIB1A_MUT=k` en una copia de la zlib mutada (la fuente buena nunca se toca) y solo en condiciones que el juego real no necesita (un tamaño de ventana concreto, un nº de bits pendientes, `avail_in == 17`, …). **Resultado: las 14 mutaciones se detectan en la ronda 1 y las 14 en la ronda 2** (`resultados/negativas.txt`, `negative-runs/ronda1.json` y `ronda2.json`; cada mutación produce diferencias EXE-vs-biblioteca en la función mutada, con 2.000–6.000 casos del escenario que la ejerce). Diferencias en la función mutada, ronda 1 / ronda 2: `inflate_blocks_reset` 1.844 / 236, `inflate_blocks_new` 353 / 225, `inflate_blocks` 4 / 1.484, `inflate_blocks_free` 470 / 484, `inflate_set_dictionary` 261 / 15, `inflate_trees_bits` 246 / 122, `huft_build` 10 / 34, `inflate_trees_dynamic` 335 / 321, `inflate_trees_fixed` 1.000 / 1.000, `inflate_flush` 184 / 154 y, para `inflate_codes_new`, `inflate_codes`, `inflate_codes_free` e `inflate_fast`, las cifras de los JSON. Las primeras versiones del banco **no** detectaban 3 mutaciones de la ronda 1 y 1 de la ronda 2 (todas en `inflate_trees_bits`/`inflate_trees_dynamic`): eso reveló que el generador producía casi solo códigos incompletos; se corrigió y se repitió toda la corrida.

## 4. Qué se registra y qué no

Todas las pruebas son **fuera del juego** (el EXE original mapeado en un proceso de prueba). Los lotes de validación del proyecto (`Fn230A` y siguientes) registran evidencia con una corrida **dentro del juego** (`register_hook.py`), con sonda en `CGame::Process` y el candado del juego; eso no se ha hecho aquí (el juego estaba ocupado por otros agentes y la validación equivalente del lado EXE ya se hace sin él). Por eso **`validated` sigue en 0 en las 14 filas de `hooks.csv`**: la evidencia de este lote (EXE original vs biblioteca, par a par, sin diferencias) es la base para la corrida en juego de la siguiente ola (`ZlibInflate` con `GTASA_WITH_ZLIB_LIBPNG=ON`: la prueba ya está montada, falta la sonda en juego y su registro).

## 5. Qué falta para sustituir las 176

- **libpng, lectura** (`pngread/pngrutil/pngrtran/pngset/pngget`, ≈ 90 funciones): el banco se parece, pero las funciones no son puras de enteros: usan `png_ptr` con callbacks (`png_read_data`, `png_error` con `longjmp`), `png_malloc`/`png_free` y x87 (gamma, `png_do_gamma`, `png_build_gamma_table`). Hace falta la misma técnica (EXE mapeado + estructuras fabricadas + `zalloc` determinista) con un `png_struct` real creado por cada lado y un oráculo con la salida de `pypng`/`libpng` moderna, y x87 a 24 bits (método de `validation-bench-method`).
- **libpng, escritura** (`pngwrite/pngwutil/pngwtran`, ≈ 50 funciones): requiere `deflate` (el EXE lleva `deflate 1.1.3`: 17 funciones de `deflate.c` + 21 de `trees.c`, aún sin mapa fiable); validar con la compresión de Python como oráculo de descompresión y comparar salida byte a byte EXE vs biblioteca.
- **Funciones sin equivalente claro** (63 del cierre): subir primero la confianza del mapa (comparación semántica de cuerpos, `-Dlocal=` también en `png*.c` si hubiese `static` sustituibles) y registrar las `bajo`.
- **`rtpng`** (`0x7CF600`, `0x7CF9B0`, lectura/escritura de `RwImage` por Criterion): reconstruir en C++ (`rtpng.cpp`) usando la biblioteca enlazada.
- **Integración en juego** de `ZlibInflate` con la opción encendida (sonda + `register_hook.py`) y decidir si la opción pasa a ser el valor por defecto.
- Compilar y probar la configuración **Release** del proyecto con la opción encendida.

## Archivos

`exe_lib.py`, `exe_funcs.py`, `fuente_lib.py`, `mapa2.py`, `mapa3.py`, `mapa_final.py`, `compilar_objs.cmd` (mapa); `bench.cpp`, `build_bench.cmd`, `reserva_imagen.asm`, `gen_cases.py`, `mutate.py`, `neg_run.py`, `neg_chain.sh`, `run_all.sh` (banco); `build_lib1a.sh`, `build_lib1a.out` (compilación del proyecto); `mapa_funciones.csv`, `exe_funcs.csv`, `datos/cierre_libpng_zlib.csv`, `negative-runs/` (datos).
