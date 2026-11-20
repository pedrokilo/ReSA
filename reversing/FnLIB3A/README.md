# FnLIB3A: libjpeg 6b (IJG) compilada desde la fuente oficial (19/11/2026)

> **Estado a 20/11/2026 (LIB3B, sección 8): las 60 filas del mapa están validadas (22 de la tanda 1 + 38 de la tanda 3; 62 pruebas con las 24 funciones de la tanda 1, 1.240.000 pares, 0 diferencias, 126 mutaciones detectadas).** Las secciones 4, 5 y 7 describen el estado de la tanda 1/2 y quedan superadas por la 8.

Lote del hito «EXE independiente» para libjpeg: compilar **libjpeg 6b** (la que lleva `gta_sa.exe`, `jversion` «6b 27-Mar-1998»), mapear sus funciones contra las del EXE y sustituir con la biblioteca los callbacks que el código propio todavía toma del EXE, validándolos par a par contra el EXE. Método de `reversing/FnLIB1A/README.md`. **Primera tanda: 24 funciones (22 de las 58 raíces S3A/S4A de `docs/ExeIndependiente/raices.csv`).**

| | Resultado |
|---|---|
| Fuente oficial | `jpegsrc.v6b.tar.gz` de `D:\Original\descargas\ijg-jpeg-6b` extraído en un directorio nuevo; SHA256 `75c3ec241e9996504fe02a9ed4d12f16b74ade713972f3db9e65ce95cd27e35d` (es el valor que circula para ese archivo; no se pudo contrastar contra ijg.org desde aquí). Los 58 archivos de `libs/libjpeg-6b/` son copia byte a byte de los del árbol (`diff -rq` limpio); nada del paquete se ejecutó |
| Opción CMake | `GTASA_WITH_LIBJPEG` (por defecto **APAGADA**) en `source/CMakeLists.txt` |
| Funciones sustituidas (detrás de la opción) | **24** (lista abajo): 22 raíces + `grayscale_convert` (0x5C35F0, ya enganchada) + `null_convert` de jdcolor (0x5C90D0) |
| Banco EXE vs biblioteca | **24 de 24 sin diferencias**: 20.000 pares por función (≥ 16.384), 480.000 pares, 0 diferencias, x87 a 24 bits |
| Oráculo independiente (Python) | 0 fallos en todas (`resultados/oraculo.txt`) |
| Prueba negativa | **48 de 48 mutaciones detectadas** (2 rondas × 24; `resultados/negativas.txt`, `negative-runs/ronda1.json`, `ronda2.json`) |
| `validated=1` en `hooks.csv` | **ninguna**: la evidencia es fuera del juego; falta la corrida dentro del juego con `register_hook.py` (el juego no se lanza desde este lote) |

## 1. Cómo se compila

- `libs/libjpeg-6b/`: las 46 `.c` del núcleo (compresión, descompresión, `jmemmgr`, `jmemnobs`, `jutils`, `jerror`; sin `cjpeg/djpeg/jpegtran/example`, sin `jmem*` alternativos, sin makefiles), 10 cabeceras, `README` (licencia) y `jconfig.vc` como `jconfig.h`. **Fuente oficial sin ningún parche.**
- `CMakeLists.txt` → estática `libjpeg6b`, mismas opciones que zlib/libpng de LIB1A: `/O2 /Ob2 /Oy- /GS- /Gd /arch:IA32 /Zp8` (solo x87; optimizada también en Debug, `/RTC1` y `/ZI` anulados). La precisión del FPU (24 bits de mantisa) no la fija la biblioteca sino el proceso (el juego; el banco la fija con `_controlfp(_PC_24)`).
- **Envoltorios** `wrap/w_<archivo>.c` (generados por `gen_wrappers.py`): cada `.c` oficial entra por un envoltorio que lo `#include`a entero y exporta (`jpeg6b_export.h`, `jpeg6b_find("jccolor","rgb_ycc_convert")`) las direcciones de **todas** sus funciones, también las `static` (`METHODDEF`/`LOCAL`): los callbacks del EXE son `METHODDEF` y se enlazan por puntero, así que no se pueden exportar sin tocar la fuente. 330 funciones en 46 archivos. `jpeg6b_exports.c` es el índice.
- **`exe_config.h` (diferencias de configuración EXE ↔ oficial, descubiertas por el banco)**, aplicadas desde el envoltorio tras `jpeglib.h` y antes del `.c`:
  1. `MAX_COMPONENTS = 4` (oficial 10): `start_pass_upsample` (0x5CCE60) del EXE escribe `next_row_out` en +0x2C (12 + 4N + 4N, N = 4). Cambia la disposición de las estructuras privadas.
  2. `RGB_PIXELSIZE = 4` (oficial 3): `rgb_ycc_convert` (0x5C32C0) avanza la entrada de 4 en 4 (RGBA); `RGB_RED/GREEN/BLUE` = 0/1/2.
  3. Solo DCT en coma flotante: `DCT_ISLOW_SUPPORTED` y `DCT_IFAST_SUPPORTED` sin definir. `start_pass_fdctmgr` (0x5C3940) del EXE devuelve `JERR_NOT_COMPILED` (48) con `JDCT_ISLOW/IFAST`. (Pendiente de confirmar para la parte de descompresión: `jddctmgr`/`jidct*`.)
  `exe_prelude.h` = el mismo prólogo para `compilar_objs.cmd` (`/FI`).
- Enlace en el proyecto: con `GTASA_WITH_LIBJPEG=ON` se hace `add_subdirectory(libs/libjpeg-6b)`, `target_link_libraries(... libjpeg6b)` y `GTASA_WITH_LIBJPEG=1`. El directorio de includes de la biblioteca es PRIVADO (las cabeceras oficiales chocarían con las de libjpeg-turbo de la DLL). `LibJpegCallback.h` define `LIBJPEG_CB(direccion, "archivo", "funcion")`: con la opción apagada es la dirección del EXE (comportamiento actual, sin cambios); encendida, el puntero de la función de la biblioteca. Se aplica en 30 asignaciones de punteros de `LibJpegCompressEncode.cpp` (jinit_color_converter, jinit_forward_dct), `LibJpegDecompressUpsample.cpp` (jinit_upsampler) y `JPegCompress.cpp` (jinit_color_deconverter). Las estructuras son las del EXE (el banco lo comprueba: mismos desplazamientos).
- Compilaciones del proyecto (bajo `compile_lock`, `build_lib3a.sh`, salidas `build_lib3a.out`, `build_OFF.log`, `build_ON.log`): ver sección 5.

## 2. Funciones (24) y mapa EXE ↔ fuente

`mapa_funciones.csv` lista las 58 raíces con su estado. Mapa por **comportamiento + estructura**: el orden de los archivos en el EXE no es el alfabético (`jcapistd` 0x5C26B0, `jccoefct`, `jccolor` 0x5C31E0, `jcdctmgr`, `jchuff`, `jcinit` 0x5C5260, `jcmainct`, `jcmarker`, … `jdcolor`/`jdmerge` 0x5C8Bxx–0x5C93xx, `jdhuff` 0x5C9790, `jdinput`, `jdmainct`, `jdmarker`, `jdmaster`, `jdpostct`, `jdsample` 0x5CCDF0–0x5CD5F0, `jerror` 0x5CD6B0, `jfdctflt` 0x5CD860, `jmemmgr` 0x5CEBC0, `jmemnobs` 0x5CF800, `jutils` 0x5CF890, `jcapimin` 0x5CF960, `jdapimin` 0x5CFD20); se descubrieron 384 funciones en 0x5C2600–0x5D0250 (`exe_funcs.py`, `exe_funcs.csv`), se leyeron los nombres ya documentados en los comentarios del código propio (80 direcciones con nombre) y cada candidato se **probó con el banco**: un emparejamiento erróneo da diferencias de inmediato, así que un 0 con 20.000 casos es a la vez mapa y validación. Aciertos y correcciones de la primera hipótesis: `0x5C8FB0` es `ycc_rgb_convert` de jdcolor (no una de jdmerge); el bloque `0x5C8B40–0x5C8E10` parece de jdmerge (sin validar); `0x5C90D0` (`null_convert` de jdcolor, no figuraba entre las raíces) y `0x5C9340` (`start_pass_dcolor`) confirmadas.

| Archivo | Funciones (dirección) |
|---|---|
| jccolor | `rgb_ycc_start` 0x5C31E0, `rgb_ycc_convert` 0x5C32C0, `rgb_gray_convert` 0x5C33E0, `cmyk_ycck_convert` 0x5C3490, `grayscale_convert` 0x5C35F0, `null_convert` 0x5C3660, `null_method` 0x5C36F0 |
| jcdctmgr / jfdctflt | `start_pass_fdctmgr` 0x5C3940, `forward_DCT_float` 0x5C3B70, `jpeg_fdct_float` 0x5CD860 |
| jdsample | `start_pass_upsample` 0x5CCE60, `fullsize_upsample` 0x5CCF60, `noop_upsample` 0x5CCF70, `int_upsample` 0x5CCF80, `h2v1_upsample` 0x5CD080, `h2v2_upsample` 0x5CD0E0, `h2v1_fancy_upsample` 0x5CD160, `h2v2_fancy_upsample` 0x5CD220 |
| jdcolor | `ycc_rgb_convert` 0x5C8FB0, `null_convert` 0x5C90D0, `grayscale_convert` 0x5C9150, `gray_rgb_convert` 0x5C9180, `ycck_cmyk_convert` 0x5C91E0, `start_pass_dcolor` 0x5C9340 |

## 3. Banco (`bench.cpp`) y oráculo (`oraculo.py`)

`bench.exe` (32 bits, base 0x400000, reserva `reserva_imagen.asm`) mapea el `gta_sa.exe` original (solo lectura, copia de pruebas; SHA256 `72ae59e4…ecc9b`) y llama a sus funciones por dirección; la biblioteca se llama por `jpeg6b_find`. Cada caso, **dos veces con las mismas entradas y la misma arena de memoria (misma dirección)**: se compara valor devuelto, si hubo `error_exit` y su código, y **toda la arena usada** (estructuras, tablas, filas de salida, bytes de relleno). La arena se rellena con basura aleatoria antes de cada caso (una función que no escribe lo que debe lo delata). Fixtures: `jpeg_compress_struct`/`jpeg_decompress_struct` reales (las del EXE tienen los mismos desplazamientos), gestor de memoria y de errores simulados (`longjmp`), filas con valores extremos sesgados (0, 255, 127/128/129), anchos 1–96, 1–4 filas, offsets de fila, factores de expansión 1–4, 1–4 componentes, tablas de cuantización con y sin errores (5.695 de 20.000 casos de `start_pass_fdctmgr` terminan en error, igual en los dos lados). x87 a 24 bits (`_controlfp(_PC_24)`), igual que el juego.

Oráculo (Python puro, sin biblioteca ni EXE) sobre la traza de cada caso: tabla `rgb_ycc` y conversiones YCC (constantes FIX de JFIF), CMYK→YCCK, gris, `null_convert`; interpolación triangular de `h2v1/h2v2_fancy_upsample`, réplica de `int/h2v1/h2v2_upsample`; divisores de cuantización flotantes de `start_pass_fdctmgr` **emulando el x87 a 24 bits** (cada operación se redondea a float: sin ello el oráculo discrepaba por 1 ulp, que es el defecto «x87 a 24 bits» de la memoria de validación); DCT 8×8 exacta en doble precisión para `jpeg_fdct_float` (máx. |error| 0,00101) y `forward_DCT_float` (191.936 coeficientes: 191.761 idénticos al redondeo exacto y 175 empates en .5 con ±1); `ycc_rgb_convert` con las tablas de JFIF y recorte. Resultado: 0 fallos.

| Función | Dirección | Pares | Diferencias | Fallos del oráculo |
|---|---|---:|---:|---:|
| `rgb_ycc_start` | 0x5C31E0 | 20.000 | 0 | 0 |
| `rgb_ycc_convert` | 0x5C32C0 | 20.000 | 0 | 0 |
| `rgb_gray_convert` | 0x5C33E0 | 20.000 | 0 | 0 |
| `cmyk_ycck_convert` | 0x5C3490 | 20.000 | 0 | 0 |
| `grayscale_convert` (jccolor) | 0x5C35F0 | 20.000 | 0 | 0 |
| `null_convert` (jccolor) | 0x5C3660 | 20.000 | 0 | 0 |
| `null_method` | 0x5C36F0 | 20.000 | 0 | 0 |
| `start_pass_fdctmgr` | 0x5C3940 | 20.000 | 0 | 0 |
| `jpeg_fdct_float` | 0x5CD860 | 20.000 | 0 | 0 |
| `forward_DCT_float` | 0x5C3B70 | 20.000 | 0 | 0 |
| `start_pass_upsample` | 0x5CCE60 | 20.000 | 0 | 0 |
| `fullsize/noop/int/h2v1/h2v2/h2v1_fancy/h2v2_fancy_upsample` (7) | 0x5CCF60–0x5CD220 | 20.000 c/u | 0 | 0 |
| `start_pass_dcolor` | 0x5C9340 | 20.000 | 0 | 0 |
| `ycc_rgb_convert` | 0x5C8FB0 | 20.000 | 0 | 0 |
| `null_convert` (jdcolor) | 0x5C90D0 | 20.000 | 0 | 0 |
| `grayscale_convert` (jdcolor) | 0x5C9150 | 20.000 | 0 | 0 |
| `gray_rgb_convert` | 0x5C9180 | 20.000 | 0 | 0 |
| `ycck_cmyk_convert` | 0x5C91E0 | 20.000 | 0 | 0 |

Archivos: `resultados/corrida_completa.txt`, `resultados/oraculo.txt`. Comandos: `build_bench.cmd <dir>` y `bench.exe <dir> <gta_sa.exe> --n 20000`, luego `python -I oraculo.py <dir>`. Cuidado: `jpeg_fdct_float` y `forward_DCT_float` escriben traza solo de 1.500 casos (el oráculo es O(4096) por bloque), pero los 20.000 se comparan EXE vs biblioteca.

**Prueba negativa de 2 rondas** (`mutate.py`, `neg_run.py`): copia de la biblioteca con una mutación por función (24 por ronda, rondas distintas) activada por la variable `lib3a_mut` (`LIB3A_MUT=k`) solo en condiciones concretas de datos que el juego rara vez necesita (un valor de píxel, una columna, un tamaño de fila…); la fuente buena nunca se toca. **Las 24 se detectan en la ronda 1 y las 24 en la ronda 2.** En la primera pasada fallaron 2 mutaciones, y ninguna era un fallo del banco sino mutantes equivalentes (`h2v2_fancy` +1 que se pierde en `>> 4`; `grayscale_convert` con una condición sobre `num_rows` que nunca es cierta porque el bucle lo decrementa): se rediseñaron y se repitió todo. Las diferencias por mutación van de 20 (`rgb_gray_convert`) a 20.000.

## 4. Hallazgos

1. **El EXE no es la libjpeg 6b por defecto**: `MAX_COMPONENTS` 4, `RGB_PIXELSIZE` 4 y solo DCT flotante (arriba). Sin `exe_config.h` la biblioteca no coincidía en 297 de 300 casos de `rgb_ycc_convert`, en todos los de `start_pass_upsample` y en los de `start_pass_fdctmgr`. Quien reconstruya `jinit_*` con la biblioteca debe usar siempre los envoltorios.
2. **Las funciones `static` son las que enlaza el EXE**: sin los envoltorios no habría forma de sustituirlas sin parchear la fuente.
3. **x87 a 24 bits**: los divisores flotantes de `start_pass_fdctmgr` dependen de ello (el oráculo tuvo que emular el redondeo a float en cada operación). El banco no usa comparaciones `!(a<=b)`: ninguna función de esta tanda las necesita.
4. El bloque 0x5C8AF0–0x5C8E10 antes de `jdcolor` parece de **jdmerge** (por orden de archivo y por sus campos de `cinfo`; no está validado) y no de jdcolor; la identificación de `ycc_rgb_convert` en 0x5C8FB0 sí está validada por el banco.
5. `null_convert` de jdcolor (0x5C90D0) y `grayscale_convert` de jccolor (0x5C35F0) no figuran en las 58 raíces pero el código propio las usa; se incluyen. `0x5C35F0` ya estaba enganchada en la DLL.

## 5. Estado de la compilación del proyecto y lo que no se pudo

Ver el final de este archivo (sección 6, rellenada al terminar `build_lib3a.sh`).

**No hecho en esta tanda (siguiente):**
- Las **34 raíces restantes** (`mapa_funciones.csv`, estado «pendiente»): `jccoefct` (`start_pass_coef` 0x5C2FF0…), `jchuff` (`start_pass_huff` 0x5C5090, `encode_mcu_huff`…), `jcmainct`, `jcmarker`/`jcmaster`, `jcprepct`, `jcsample`, `jdhuff` (`decode_mcu` 0x5C9CE0, `start_pass_huff_decoder` 0x5CA110), `jdinput`, `jdmainct`, `jdmarker` (`get_interesting_appn` 0x5CBF00…), `jdmerge`, `jdpostct`. Necesitan fixtures con `cinfo` completo (tablas de Huffman, búferes de entrada, marcadores) y un oráculo propio (p. ej. `PIL`/`libjpeg-turbo` como decodificador y codificador de referencia). Las que fijan punteros (`jinit_*`) solo se podrán sustituir cuando el código propio las reconstruya con `LIBJPEG_CB`.
- Las ~300 funciones restantes de la biblioteca en el EXE (API pública `jpeg_*`, jmemmgr, jerror, jquant) no son raíces del plan: no se mapearon salvo lo que ya documentaba el código propio.
- Confirmar el soporte de `jddctmgr`/`jidct*` del EXE (la configuración «solo flotante» se dedujo para compresión).
- `validated=1`: requiere la corrida dentro del juego con `register_hook.py` (sonda en `CGame::Process`); no se ha hecho.
- Compilar y probar Release del proyecto con la opción encendida.

## 6. Compilaciones del proyecto (Debug, bajo `compile_lock`, `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1`, `build_lib3a.sh` → `build_lib3a.out`)

- **GTASA_WITH_LIBJPEG apagada (por defecto): correcta** (23:48–23:55, `build-project.ps1`; `gta_reversed.asi` 50.545.664 B, SHA256 `97882e78…1ec8`). Una primera pasada falló por falta de `#include "LibJpegCallback.h"` en dos `.cpp` (corregido).
- **GTASA_WITH_LIBJPEG encendida: correcta** (23:55–00:03; `gta_reversed.asi` 49.679.872 B, SHA256 `f5082049…c886`). Sin errores de compilación ni de enlace; sin símbolos duplicados con libjpeg-turbo de la DLL (el directorio de includes de la biblioteca es privado).
- No se ha cargado ni ejecutado ninguno de los dos `.asi` en el juego.

## 7. Tanda 2 (parcial, 20/11/2026): configuración de descompresión

- **Confirmado: la descompresión también es solo coma flotante.** `0x5CDA90` (4.364 B, 1.121 instrucciones) es `jpeg_idct_float`: usa exactamente las constantes de `jidctflt.c` (1,414213562, 1,847759065, 1,082392200, 2,613125930). En todo el rango 0x5C2600–0x5D8000 no aparece ninguna constante entera de `jidctint.c` (FIX_1_175875602 = 9633, FIX_2_562915447 = 20995) ni de `jidctfst.c` (277, 473). Coherente con `DCT_ISLOW/IFAST_SUPPORTED` sin definir de `exe_config.h` (la biblioteca compilada así ya no trae `jpeg_idct_islow/ifast`). Falta ubicar `start_pass`/`jinit_inverse_dct` de `jddctmgr` (no es 0x5CD740, que es `format_message` de `jerror`) y comprobar `jidctred`.
- `0x5CDA90` queda identificada como `jpeg_idct_float` pero **sin banco todavía**.
- **Las 36 raíces restantes siguen sin hacerse** (no se construyeron los fixtures de `jccoefct`, `jchuff`, `jcmainct`, `jdhuff`, `jdinput`, `jdmainct`, `jdmarker`, `jdmerge`, `jdpostct` ni su oráculo). Siguen pendientes y sin cambios en `mapa_funciones.csv`; los `jinit_*` no se tocan salvo donde el código propio ya usa `LIBJPEG_CB` (hoy: `jinit_color_converter`, `jinit_forward_dct`, `jinit_upsampler`, `jinit_color_deconverter`).


## 8. Tanda 3 (LIB3B, 20/11/2026): las raíces que quedaban «pendiente»

Continuación de LIB3A con el mismo banco (`bench.cpp`, ahora con 62 pruebas) y el mismo oráculo (`oraculo.py`). **38 funciones nuevas validadas par a par contra el EXE** (62 en total con las 24 de la tanda 1; las 34 raíces «pendiente» del README original más `jpeg_idct_float` y `dummy_consume_data`, que no estaban en la lista). Las dos filas que seguían sin identificar en `mapa_funciones.csv` quedan resueltas: `0x5C8E10` = `dummy_consume_data` (jdcoefct) y `0x5CF690` = `self_destruct` (jmemmgr). Las 4 funciones de `jdmarker`/`jdhuff`/`jchuff` que sólo se ejercitan dentro de otra quedan como «validada indirectamente».

| | Resultado |
|---|---|
| Pruebas del banco | **62 de 62 sin diferencias**: 20.000 pares por función (≥ 16.384), 1.240.000 pares, 0 diferencias (`resultados/corrida_completa_3b.txt`), x87 a 24 bits |
| Oráculo independiente | 0 fallos en las 62 (`resultados/oraculo_3b.txt`); aritmética en Python para IDCT, tabla de multiplicadores, submuestreo, preproceso; **codificador Huffman de referencia propio** (oráculo de `decode_mcu`); analizador de marcadores en Python; flujo de bytes de DQT/DHT/DRI/SOS; códigos canónicos del estándar |
| Prueba negativa | **126 de 126 mutaciones detectadas** (63 por ronda × 2 rondas; `resultados/negativas_3b.txt`, `negative-runs/ronda1.json`, `ronda2.json`); mutaciones de 2 rondas por función, activadas sólo con datos concretos |
| `mapa_funciones.csv` | 60 raíces: **todas validadas** (22 de LIB3A + 38 de LIB3B) y 4 funciones internas «validadas indirectamente» |
| `validated=1` en `hooks.csv` | **ninguna** (el juego no se lanza desde este lote; falta la corrida dentro del juego) |
| Código propio | 38 asignaciones de callbacks ya validados pasan por `LIBJPEG_CB` (`JPegCompress.cpp`, `LibJpegCompress*.cpp`, `LibJpegDecompress*.cpp`, `LibJpegError.cpp`, `LibJpegMemory.cpp`); con la opción apagada siguen siendo las direcciones del EXE |

### 8.1 Grupos (un commit por grupo)

| Grupo | Funciones (dirección) | Fixture y oráculo |
|---|---|---|
| 1 jidctflt | `jpeg_idct_float` 0x5CDA90 | `cinfo` con tabla de recorte, tabla AAN real (oráculo: IDCT exacta en doble precisión, máx. ±1 por el truncamiento de la biblioteca; 154.551 de 161.024 muestras idénticas al redondeo exacto) y tablas con NaN/inf/denormales (sólo EXE frente a biblioteca) |
| 2 estado | `emit_message` 0x5CD700, `format_message` 0x5CD740, `reset_error_mgr` 0x5CD7F0, `finish_output_pass` 0x5CCC90, `reset_marker_reader` 0x5CC7A0, `finish_input_pass` 0x5CA820, `start_pass_dpost` 0x5CCD70, `start_pass_main` (d) 0x5CADC0 y (c) 0x5C53D0, `start_input_pass` 0x5C8B40, `start_output_pass` 0x5C8B60, `start_pass_coef` 0x5C2FF0, `start_pass_prep` 0x5C76A0, `finish_pass_master` 0x5C6A10, `emit_byte` 0x5C6070, `write_marker_header` 0x5C6030 | estructuras privadas con palabras aleatorias; los punteros a función que escribe cada lado se comparan por **identidad** (`Ctx::fps`: dirección del EXE ↔ función de la biblioteca); oráculo Python de las transiciones de estado, de `sprintf` (subconjunto C) y del búfer de salida con vaciado |
| 3 jcsample | `start_pass_downsample` 0x5C79E0, `sep_downsample` 0x5C7A50, `int_downsample` 0x5C7AE0, `fullsize_downsample` 0x5C7C10, `h2v1_downsample` 0x5C7C60, `h2v2_downsample` 0x5C7D10 | filas aleatorias, relaciones de muestreo 1–4, anchos no múltiplos de 8; oráculo Python del promedio con sesgo alterno y de la extensión del borde derecho |
| 4 jddctmgr | `start_pass` 0x5C9510 | 1–4 componentes, métodos DCT 0–2, tamaños DCT erróneos; oráculo: tabla `q·aan[f]·aan[c]` **emulando el x87 a 24 bits** (bit a bit) |
| 5 jcprepct | `pre_process_data` 0x5C7710 | simulación completa en Python con conversión de color y submuestreo simulados (hash determinista), relleno inferior y registro de argumentos |
| 6 jdcoefct | `decompress_onepass` 0x5C8B70 | `decode_mcu` e IDCT simulados con registro; suspensión a mitad de fila; simulación Python de contadores y retorno |
| 7 jcmarker | `write_tables_only` 0x5C62F0, `write_scan_header` 0x5C61E0 | tablas DQT (8/16 bits) y DHT reales, modo progresivo, aritmético, `restart_interval`; oráculo: el flujo de bytes y las banderas `sent_table` |
| 8 jmemmgr | `access_virt_sarray` 0x5CF2F0, `access_virt_barray` 0x5CF430 | almacenamiento de respaldo simulado (lectura/escritura registradas); oráculo: máquina de estados completa (ventana, volcado, relleno a cero) |
| 9 jdmarker | `read_markers` 0x5CC290, `get_interesting_appn` 0x5CBF00, `jinit_marker_reader` 0x5CC7D0 | cabeceras JPEG generadas (SOI, JFIF/JFXX/Adobe, COM, APPn, DQT, SOF0/1/2/9, DHT, DRI, SOS) y mutadas (bits, bytes, truncado, inserción), repartidas en trozos con **suspensión** de la fuente (`fill_input_buffer`/`skip_input_data` simulados); oráculo: analizador Python del estándar (dimensiones, componentes, tablas, JFIF/Adobe, reinicio, SOS, bytes consumidos) |
| 10 jdhuff | `decode_mcu` 0x5C9CE0, `start_pass_huff_decoder` 0x5CA110 | **codificador Huffman de referencia propio** (tablas del Anexo K con `huffval` permutado, bits de relleno 1, byte stuffing, RSTn, bloques densos); en `decode_mcu` las tablas derivadas se construyen con la `start_pass` de cada lado; 2.295 casos válidos (23.618 bloques) idénticos a los codificados, más flujos corruptos/truncados/suspendidos (EXE frente a biblioteca) |
| 11 jchuff | `start_pass_huff` 0x5C5090 | modo estadísticas y modo normal, índices fuera de rango, tablas ausentes o corruptas, tablas ya reservadas; oráculo: códigos canónicos del estándar |
| 12 jmemmgr / jdcoefct | `self_destruct` 0x5CF690, `dummy_consume_data` 0x5C8E10 | `malloc`/`free` de la biblioteca (`w_jmemnobs.c` con `-Dmalloc=lib3b_malloc -Dfree=lib3b_free`, sólo en el banco) y los 4 `call` del CRT del EXE (0x5CF807/0x5CF815/0x5CF827/0x5CF835) desviados a un asignador del banco con registro; oráculo Python del orden de liberación (pools en orden inverso, listas LIFO, arrays virtuales cerrados al liberar el pool de imagen) |

### 8.2 Hallazgos de esta tanda

1. **Más diferencias de configuración EXE ↔ oficial** (en `exe_config.h`; `JPEG6B_KEEP_2PASS` mantiene `jquant2.c` aparte): sin cuantización de 2 pasadas (`start_pass_dpost`, `start_pass_main` y `start_pass_main` de compresión dan `JERR_BAD_BUFFER_MODE` con cualquier modo distinto de `JBUF_PASS_THRU`), sin suavizado de bloques (`start_output_pass` de jdcoefct sólo pone `output_iMCU_row = 0`), sin suavizado de entrada ni filas de contexto en compresión (`start_pass_prep` sólo escribe `rows_to_go` y `next_buf_row`), sin IDCT reducidas (`start_pass` de jddctmgr sólo admite `DCT_scaled_size == 8`) y sin aritmética (`write_scan_header` se la salta). Los wrappers exportan sólo lo que existe con esa configuración (guardas en `wrap/w_jdcoefct.c`, `w_jdmainct.c`, `w_jdpostct.c`, `w_jcprepct.c`, `w_jcsample.c`, `w_jidctred.c`; `gen_wrappers.py` actualizado).
2. **`jpeg_inverse_dct` de 4 componentes**: `jpegint.h` declara `inverse_DCT[MAX_COMPONENTS]` antes de que `exe_config.h` fije `MAX_COMPONENTS = 4`; `w_jddctmgr.c` redeclara la estructura con la disposición del EXE (`cur_method` en +0x14, como lee `start_pass` 0x5C9510). Sólo `jddctmgr` depende de ese tamaño.
3. **`boolean` = `unsigned char`** (EXE y biblioteca): las funciones que devuelven `boolean` sólo comparan `AL` (`Ctx::ret_mask`); EAX alto es basura distinta en cada lado.
4. **`get_dht` copia `huffval` de la pila sin inicializar**: los bytes de `huffval` más allá de `sum(bits)` son indeterminados en la biblioteca original; el banco los normaliza a 0 antes de comparar (`Ctx::norm`) y el oráculo sólo los compara hasta `sum(bits)`.
5. **El EXE está compilado con optimización de programa completo**: los auxiliares internos usan convenciones por registro (`emit_marker`, `emit_2bytes`, `emit_dht`, `get_sof`, `get_dht`, `expand_right_edge`…), por eso sólo se llaman por dirección las raíces con entrada `cdecl` y los auxiliares se validan por flujo dentro de ellas (marcadores, Huffman, escritura de cabeceras).
6. **Mejoras del banco** (todas las pruebas anteriores se repitieron: 0 diferencias): se restaura el arena completo (hasta `g_hi + 64 KB`) entre un lado y el otro, no sólo hasta el último byte reservado (las reservas hechas durante la llamada heredaban restos del otro lado); `Ctx::fps` (identidad de punteros a función), `Ctx::norm`, `Ctx::ret_mask`; error durante el montaje de un caso (no `longjmp` suelto); `sprintf`, `malloc` y `free` del EXE desviados a ganchos del banco.
7. `read_markers` del EXE también acepta SOF aritméticos (0xC9…) con `get_sof` por registro; las ramas se ejercitan con `SOF9` en el 8 % de los flujos.

### 8.3 Validadas indirectamente (sin prueba propia)

Se ejecutan nativamente dentro de las raíces anteriores y cualquier diferencia habría roto la comparación del arena completo: `skip_variable` 0x5CC050 y `read_restart_marker` 0x5CC610 (como punteros de cada lado en `read_markers`/`decode_mcu`), `jpeg_make_d_derived_tbl` 0x5C9790 (`start_pass_huff_decoder`), `jpeg_make_c_derived_tbl` 0x5C43F0 (`start_pass_huff`) y, sin fila propia en el mapa, `get_soi` 0x5CAF00, `get_sof` 0x5CAFB0, `get_sos` 0x5CB2C0, `get_dht` 0x5CB8C0, `get_dqt` 0x5CBB20, `examine_app0` 0x5CBC00, `examine_app14` 0x5CBE50, `next_marker` 0x5CC0F0, `first_marker` 0x5CC1F0 (dentro de `read_markers`; los nombres salen del orden del archivo y del flujo, no de símbolos), `jpeg_fill_bit_buffer` 0x5C9A60, `jpeg_huff_decode` 0x5C9B80, `process_restart` 0x5C9C60, `start_iMCU_row` 0x5C8AF0, `expand_right_edge` 0x5C79F0, `free_pool` 0x5CF580, `do_sarray_io` 0x5CF1B0.

### 8.4 Qué falta

- `jinit_*` (jcinit, jcprepct, jdmaster…, 0x5C5210, 0x5C30D0, 0x5CA260…), `jdmaster`/`jdinput` completos, `jdapimin`/`jdapistd`, `jcapimin`, `jerror` restante (`output_message`, `error_exit`), `jmemmgr` (`alloc_*`, `request_*`, `realize_virt_arrays`), `jcmaster` salvo `finish_pass_master` y las ~250 funciones de la biblioteca que no son raíces del plan: sin banco.
- **`jmemmgr`: `self_destruct` está validada, pero `LIBJPEG_CB` NO se aplica a `self_destruct` (0x5CF690) en `LibJpegMemory.cpp`**: la versión de la biblioteca libera con el `free` de la DLL bloques reservados con el asignador del EXE (`jpeg_get_small` 0x5CF800), y con la opción encendida mezclaría montones.
- `read_restart_marker` (0x5CC610) y `jinit_marker_reader`, `jinit_huff_*`, etc. siguen instalándose por dirección del EXE donde no hay prueba propia.
- `validated=1`: requiere la corrida dentro del juego con `register_hook.py`; no se ha hecho.
- Compilación del proyecto con la opción encendida/apagada tras estos cambios: ver el final de este apartado.
