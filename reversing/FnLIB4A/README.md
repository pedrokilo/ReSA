# FnLIB4A: escritura de PNG (libpng 1.0.12), compresion deflate (zlib 1.1.3) y capa RtPNG sustituidas (21/11/2026)

Cuarto lote del hito «EXE independiente» para PNG. Parte de LIB1A (zlib/libpng compiladas desde la fuente oficial, opcion CMake `GTASA_WITH_ZLIB_LIBPNG`), LIB2A (lectura de libpng) y LIB3A (libjpeg). Aqui se sustituyen lo que aun ejecutaba codigo original del EXE:

- la **compresion deflate de zlib 1.1.3** (`deflate.c`, `trees.c`, `adler32`, `crc32`),
- la **escritura de libpng 1.0.12** (`pngwrite.c`, `pngwutil.c`, `pngwtran.c`, `pngwio.c`, los ajustes de `pngtrans.c`, `png_set_error_fn/mem_fn` y `png_error/png_warning`),
- la capa **RtPNG** de RenderWare (`RtPNGImageWrite`, `RtPNGImageRead` y sus 5 callbacks), reconstruida en C++.

## Resumen

| | Resultado |
|---|---|
| Funciones sustituidas por la biblioteca oficial (hooks tras `GTASA_WITH_ZLIB_LIBPNG`) | **103** (37.400 bytes del EXE): 25 de zlib (14.354 bytes: `deflate.c` 13, `trees.c` 10, `adler32`, `crc32`) y 78 de libpng (23.046 bytes: escritura, ajustes de transformaciones, errores) |
| Funciones reconstruidas en C++ | **7** (RtPNG: `RtPNGImageWrite` 0x7CF600, `RtPNGImageRead` 0x7CF9B0 y 5 callbacks); sin validar (necesitan RenderWare y archivos) |
| Equivalencia comprobada | banco par a par con el EXE original mapeado en memoria (sin ejecutar el juego): **todas las sustituciones a la vez, 0 diferencias** (cifras abajo); cada funcion por separado con su control negativo |
| `validated=1` en `hooks.csv` | **ninguna** (evidencia fuera del juego); 7 + 103 filas nuevas con `reversed=1, validated=0` |
| Compilacion del proyecto (Debug, `GTASA_WITH_ZLIB_LIBPNG=ON`) | `EXIT=0` (`_build_LIB4A.log`) |
| `contrib/audit_hook_catalog.py --strict` | OK |

**Verificacion de que la compresion es identica byte a byte: si.** La zlib 1.1.3 oficial produce exactamente los mismos bytes que el `deflate` del EXE para los mismos parametros (niveles -1..9, estrategias 0..2, `windowBits` 9..15 y raw, `memLevel` 1..9, diccionario, `deflateParams`, `deflateReset`, `deflateCopy`, vaciados `Z_NO_FLUSH/PARTIAL/SYNC/FULL/FINISH`, buffers de salida de 1 byte a 70 kB): no hizo falta reconstruir ninguna funcion de deflate en C++.

## Metodo

Toda la prueba corre fuera del juego, en un proceso de 32 bits (`build_bench.cmd`: base 0x400000 con la reserva de IND0A) que copia en memoria el `gta_sa.exe` original (solo lectura; `E:\JUEGOS\GTA San Andreas - CVehicle Test`) y llama a sus funciones por direccion. A diferencia de LIB1A/LIB2A (llamada funcion a funcion con estado fabricado) aqui se hace **sustitucion en vivo** (`common.h`, `Patch`): se escribe un `jmp` en la entrada de la funcion del EXE hacia la de la biblioteca, exactamente lo que hace `RH_ScopedGlobalInstall`, y se ejecuta un **flujo completo** (compresion de un flujo; escritura de un PNG con todos sus chunks) con el EXE puro y con el EXE parcheado, sobre la misma arena de memoria determinista (misma direccion, relleno `0xCD`, huecos de 64 bytes). Se compara todo: valores devueltos y contadores de `z_stream`, **bytes de salida**, registro de callbacks (errores, avisos con hash del mensaje, reservas con tamano, liberaciones, escrituras con tamano, vaciados) y la **memoria entera de la arena** (estado de deflate, `png_struct`, `png_info`, buffers). Los punteros a codigo (EXE contra biblioteca) se consideran iguales. FPU a 24 bits (como lo deja Direct3D), biblioteca con `/arch:IA32`.

- `bench_deflate.cpp`: `deflateInit_/Init2_/SetDictionary/Reset/Params/deflate/End/Copy` con datos de 0 a 400 kB (aleatorios, texto, rachas, copias con mutaciones, ruido suave), fallos de reserva inyectados, parametros invalidos y flujos de dos vueltas (`deflateReset`). Modos `unit` (API del EXE contra el de la biblioteca), `insitu` y `probar`.
- `bench_png.cpp`: escritura de PNG completa con `png_create_write_struct_2` del EXE: todas las combinaciones de tipo de color y profundidad (1..16 bits, paleta, gris, RGB, alfa), Adam7, anchos de 1 a 500 y alturas de 1 a 160, `png_set_filter` y heuristicas ponderadas (x87), niveles/estrategias/ventana/memLevel de compresion, tamanos de buffer de compresion, transformaciones (`bgr`, `swap`, `packing`, `packswap`, `shift`, `filler`, `invert_*`, `swap_alpha`, transformacion de usuario), `png_set_error_fn/mem_fn`, escritura por `png_write_row`/`rows`/`image`, `png_write_flush`/`set_flush`, cabecera `before_PLTE`, y todos los chunks que tiene el EXE (PLTE, tRNS, bKGD, hIST, gAMA, sRGB, cHRM, sBIT, pHYs, oFFs, pCAL, sCAL, iCCP, tEXt, zTXt, sPLT, trozos desconocidos); errores de libpng (parametros invalidos, `longjmp` del banco) y fallos de reserva.
- `oraculo_png.py` (independiente, `zlib` de Python): firma, CRC de cada chunk y que los IDAT se descompriman a exactamente los bytes que pide el IHDR (con o sin Adam7): de 300 PNG de salida, **89 completos y validos**, 1 truncado a proposito (el banco corta filas en el 5 % de los casos) y 17 atipicos (profundidad/tipo invalidos que el banco prueba para llegar a `png_error`).

### Busqueda de las direcciones por comportamiento

Las 2 raices de LIB1A (`mapa_funciones.csv`) y el mapa por forma dejaban sin pareja casi todo lo de escritura y deflate (`bajo`/`sin equivalente`). Se resolvio por comportamiento, como en LIB2A pero con parches en vivo (`buscar_deflate.py`, `buscar_png.py`, `buscar_png2.py`):

1. **Control negativo (trampa)**: cada funcion candidata del EXE se sustituye por `xor eax,eax; ret`; si el resultado del flujo no cambia, la funcion no se ejecuta o no importa en los escenarios y **no se engancha** (`no_ejercida`).
2. **Pareja**: cada candidata que se ejecuta se sustituye, una por una, por cada funcion de la biblioteca (deflate: 17 candidatas x 33 funciones; escritura PNG: 60 x ~90 y 19 x 36 en la segunda pasada, unos 6.700 pares en total, cada uno en su proceso con tiempo limite): **solo la pareja correcta da cero diferencias**; las ~6.600 parejas equivocadas dan diferencias o fallo. Resultado en `buscar_*.json`; unica ambiguedad real: `png_save_uint_32`/`png_save_int_32` (mismo cuerpo), resuelta por orden en el fichero.
3. **Mapa final** (`mapa_final.py` -> `mapa_final.csv`): 105 funciones con tamano en el EXE, nombre y archivo de la fuente, como se identifico y referencias en el EXE (llamadas, saltos, punteros).
4. **Confirmacion por funcion** (`confirmar.py`, `confirmar_extra.py` -> `confirmacion.csv`): 500 escenarios de deflate / 120-700 de PNG por funcion con la sola funcion sustituida, y trampa. Una funcion solo se engancha si es `identica` y `se_ejecuta`.
5. **Prueba final con todo a la vez** (`run_todo.py`, `resultados/todo_parcheado.txt`).

Los nombres de las funciones `local` de zlib (`fill_window`, `longest_match`, `deflate_fast/slow/stored`, `build_tree`, `pqdownheap`, `scan_tree`, `send_tree`, `compress_block`, `bi_windup`, `_tr_*`) y las dos funciones `static` de `pngwutil.c` (`png_text_compress`, `png_write_compressed_data_out`) son funciones propias del EXE; para engancharlas se exportan **sin tocar la fuente**: `deflate.c` y `trees.c` con `-Dlocal=` (`libs/zlib-1.1.3/CMakeLists.txt`, como `inftrees.c` en LIB1A) y `pngwutil.c` con la inclusion forzada `exe_funciones_estaticas.h` (incluye antes las cabeceras del CRT y deja `static` vacio; `pngwutil.c` solo usa `static` en esas dos funciones).

### Configuracion de libpng que el EXE no lleva (hallazgos que corrigen LIB2A)

Descubierto porque el banco de escritura daba chunks distintos o `png_text` de otro tamano:

- **Sin iTXt**: el EXE no tiene `png_write_iTXt` ni `png_handle_iTXt` y su `png_text` mide **16 bytes** (`shl eax, 4` en `png_set_text`, 0x7D2410), no 28 como con iTXt. La biblioteca de LIB1A/LIB2A se compilaba **con** iTXt. Se anaden `PNG_NO_READ_iTXt PNG_NO_WRITE_iTXt` a las definiciones publicas de `libpng1012` (`libs/libpng-1.0.12/CMakeLists.txt`). Se repitio el banco de LIB2A (`png_set_text`, `png_get_text`, `png_free_data`, `png_info_destroy`, 16.384 pares cada una) con la biblioteca corregida: **0 diferencias** (tampoco las daba antes porque el banco fabricaba `png_text` con la disposicion de la propia biblioteca).
- **Sin escritura de tIME**: ni `png_write_info` ni `png_write_end` del EXE llaman a `png_write_tIME` (la funcion no existe): `PNG_NO_WRITE_tIME`. Con ella, la biblioteca escribia un chunk tIME de 19 bytes mas.
- Con `PNG_NO_STDIO` el EXE escribe `sCAL` con `png_write_sCAL_s` (cadenas `scal_s_width/height` de `png_info`) y no con la version de coma flotante; `png_set_sCAL(double)` no rellena esas cadenas (identico en el EXE y en la biblioteca).

## Resultados del banco

**Prueba final con las 103 sustituciones a la vez** (`resultados/todo_parcheado.txt`; trozos de 250 escenarios en procesos separados):

| Banco | Escenarios | Pasos comparados | Bytes de PNG | Diferencias | Fallos |
|---|---:|---:|---:|---:|---:|
| `bench_deflate` (las 25 funciones de zlib sustituidas) | 2.500 | 119.199 | (salida de deflate comparada byte a byte en cada paso) | **0** | 0 |
| `bench_png` (las 103 funciones sustituidas) | 3.000 | 268.192 | 3.404.993 | **0** | 0 |

**Por funcion** (`confirmacion.csv`): cada una de las 103 funciones enganchadas, sustituida **sola**, da 0 diferencias en 300 (deflate) / 100-700 (PNG) escenarios y su **control negativo** (trampa `xor eax,eax; ret`) cambia el resultado, o sea que el banco la ejercita (`se_ejecuta`). Las funciones raras (`png_set_swap`, `png_set_swap_alpha`, `png_set_invert_mono`, `png_do_invert`, `png_do_swap`, `png_do_write_swap_alpha`) dieron `se_ejecuta` en la pasada de 600-700 escenarios (`confirmar_extra.py`) y solo `identica` en la de 100 (la trampa no cambio nada con tan pocos).

**Sin sustituir nada (`unit`)** (`resultados/unit_def_*.txt`, `unit_png_*.txt`): API publico del EXE contra el de la biblioteca: deflate 4.000 escenarios (189.147 pasos) y escritura PNG 2.000 escenarios (178.456 pasos, 2.475.373 bytes de PNG), **0 diferencias**.

### Hallazgos del banco (entradas fuera de dominio o comportamiento indefinido; ninguno es una diferencia de logica)

1. **`windowBits` = 8** lo admite zlib 1.1.3 pero falla (en el EXE y en la biblioteca por igual: flujo corrupto/cuelgue); no se genera. Los demas valores, validos o no, si.
2. **Memoria no determinista en el propio EXE**: en 1 de cada ~4.000 flujos de deflate, 1 palabra de la tabla `prev` (relleno sin inicializar del estado) toma valores distintos entre dos ejecuciones del **mismo EXE**; el banco enmascara solo esas palabras (se detectan repitiendo la ejecucion del EXE) y la salida y el resto de la memoria siguen siendo identicos.
3. **`png_check_keyword` con mas de 79 caracteres** hace `new_key[79] = '\0'` sobre un `char**` (error de libpng 1.0.12, corregido en 1.0.13): escribe 316 bytes mas alla de la variable del llamador. El EXE sobrevive por la disposicion de su pila (MSVC 7.1) y la biblioteca (MSVC 19.5) se cuelga/aborta. Una palabra clave de mas de 79 caracteres es invalida segun la especificacion y el juego no escribe texto en PNG (RtPNG no llama a `png_set_text`); el banco limita las claves a 79.
4. `png_set_compression_buffer_size` de 0 o muy pequeno (< 100) hace bucles infinitos de escritura de chunks de pocos bytes en el EXE y en la biblioteca; se limita a >= 100. `png_set_shift` con bits significativos 0 o mayores que la profundidad no termina en `png_do_shift`; `png_set_filler` en imagenes con alfa y `png_set_user_transform_info` con profundidad/canales distintos de los de la imagen desbordan los buffers de fila: fuera de dominio, tampoco se generan.
5. `png_set_unknown_chunks` guarda como `location` el `mode` del `png_ptr` en ese momento (0 antes de `png_write_info`), por eso el banco llama a `png_set_unknown_chunk_location`.

## Funciones que NO se enganchan (y por que)

| Direccion | Bytes | Funcion | Motivo |
|---|---:|---|---|
| 0x7E8DC0 | 171 | `_tr_tally` | sin llamadores: el compilador la inlino en `deflate_fast/slow`; el codigo suelto no se ejecuta |
| 0x7DA9F0 / 0x7DAA80 | 88 / 58 | `png_write_chunk_start` / `png_write_chunk_end` | sin llamadores (inlinadas en todos los chunks) |
| 0x7DAA50 | 46 | `png_write_chunk_data` | un unico llamador (0x7DCF91, dentro de `png_write_zTXt`, para el resto de la entrada sin comprimir `comp.input`) que el banco no alcanza (la trampa no cambia nada): sin evidencia |
| 0x7DA8F0 / 0x7DA920 | 35 / 19 | `png_save_int_32` / `png_save_uint_16` | sin llamadores (inlinadas) |
| 0x7DB290 / 0x7DC240 | 190 / 679 | `png_write_gAMA_fixed` / `png_write_cHRM_fixed` | sin llamadores: el EXE usa las versiones de coma flotante |
| 0x7D1200 | 265 | `png_write_png` | sin llamadores ni referencias (API de alto nivel que el juego no usa); el banco no la ejercita |
| 0x7D0380 | 35 | `png_create_write_struct` | sin llamadores; usa el `malloc` del CRT del EXE (sin montón inicializado en el banco), no se puede probar |
| 0x7D0510 / 0x7D0560 / 0x7D05D0 | 77 / 102 / 190 | `png_write_init_2` / `png_write_init` / `png_write_init_3` | API obsoleta: solo se llaman entre si, sin llamadores externos; usan `png_create_struct` con el `malloc` del CRT |
| 0x7D3550 / 0x7D3590 / 0x7D3620 | 207 / 143 / 80 | resto de `pngerror.c` (mensajes por defecto, `png_chunk_*`) | no identificadas una a una (el banco no las ejercita: `png_error`/`png_warning` van a la biblioteca, que usa las suyas); quedan sin ejecutarse mientras nadie las llame |

Fuera de esta entrega (siguen siendo del EXE; los hooks de lectura de LIB2A solo cubren parte): las funciones de lectura de `pngread.c`/`pngrutil.c`/`pngrtran.c` (`png_read_info`, `png_read_image`, `png_read_end`, `png_handle_*`, `png_do_*`, `png_create_read_struct_2`, `png_destroy_read_struct`, ...), que `RtPNGImageRead` llama por direccion.

## RtPNG (`source/game_sa/RenderWare/rw/rtpng.cpp`)

`RtPNGImageWrite` (821 bytes) y `RtPNGImageRead` (1.039 bytes) y los callbacks de libpng (0x7CF940 malloc de RenderWare con la pista 0x3010D, 0x7CF960 free, 0x7CF980 escritura al `RwStream` guardado en `png_struct+0x54` o `png_error`, 0x7CF9A0 vaciado vacio, 0x7CFDE0 lectura del `RwStream`) se reconstruyeron desde el desensamblado: crean la estructura de libpng, abren el `RwStream`, fijan IHDR (profundidad 4/8: paleta con PLTE y tRNS desde el alfa de cada entrada; 32: RGBA de 8 bits), escriben/leen las filas con punteros a los pixeles de la `RwImage` y liberan en el mismo orden (incluidas las fugas del EXE en las rutas de error de memoria). **Sin validar**: usan el sistema de archivos y la memoria de RenderWare y no se pueden ejecutar fuera del juego (se contrastaron con el desensamblado instruccion a instruccion: orden de llamadas, argumentos, tabla de saltos de los tipos de color, bucles de paleta).

**`setjmp`**: el EXE llamaba a `_setjmp3` de su CRT (0x825F7C) sobre `png_ptr->jmpbuf` y libpng hacia `longjmp` con el CRT del EXE. Con `png_error` enganchado a la biblioteca el `longjmp` lo hace el CRT de la DLL, asi que RtPNG usa el `setjmp` de la DLL (las variables leidas tras el `longjmp` son `volatile`), y por eso el codigo C++ de RtPNG solo se activa con `GTASA_WITH_ZLIB_LIBPNG`; sin la opcion las dos funciones siguen llamando al EXE y nada se engancha. Riesgo conocido: un error durante `png_create_read_struct_2` (codigo del EXE, no enganchado; solo con una version de libpng distinta de la 1.0.12, que RtPNG nunca pasa) haria `longjmp` de la DLL sobre un `jmp_buf` hecho por el CRT del EXE.

## Que falta

- **Corrida en juego** (sonda en `CGame::Process`, `register_hook.py`) de todo lo anterior con la opcion encendida: captura de pantalla (`common.cpp:195` llama a `RtPNGImageWrite`) y carga de PNG; solo entonces `validated=1`. El juego no se lanzo (prohibido a los agentes).
- Reconstruir o enganchar la lectura que queda (ver arriba).
- Configuracion **Release** del proyecto con la opcion encendida (no se compilo entera).

## Archivos

`common.h` (mapa del EXE, arena, parches), `bench_deflate.cpp`, `bench_png.cpp`, `build_bench.cmd`, `reserva_imagen.asm`; `buscar_*.py/.json/.out` (busqueda por comportamiento), `mapa_final.py/.csv`, `confirmar*.py`, `confirmacion.csv`, `run_todo.py`, `gen_hooks.py` (genera `zlibdeflate.cpp`, `libpngwrite.cpp` y las filas de `docs/hooks.csv`), `oraculo_png.py`, `exe_lib.py` y `lista.py` (capstone sobre el PE leido a mano), `resultados/`. Comandos: `build_bench.cmd <dir> deflate|png`, `bench_*.exe <exe> unit|insitu|probar N semilla [nombre=0xDIR,...]`.
