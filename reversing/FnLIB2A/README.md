# FnLIB2A: libpng 1.0.12 (lectura) del EXE sustituida por la biblioteca oficial

Segundo lote del hito «EXE independiente» para PNG. Parte de LIB1A (`reversing/FnLIB1A`: bibliotecas compiladas desde fuente, opción CMake `GTASA_WITH_ZLIB_LIBPNG` apagada por defecto, 14 funciones de inflate). Aquí se sustituyen las funciones de **lectura** de libpng (`png.c`, `pngerror.c`, `pngget.c`, `pngmem.c`, `pngread.c`, `pngrio.c`, `pngrtran.c`, `pngrutil.c`, `pngset.c`) por `libs/libpng-1.0.12` y se validan **fuera del juego**, par a par contra el EXE original.

## Resumen

| | Resultado |
|---|---|
| Funciones comparadas con el EXE | **110** (107 sin diferencias, 3 con ellas; ver «Sin concluir»); otras 4 de la biblioteca no están en el EXE |
| Pares por función | **16.384** |
| Diferencias en las 107 validadas | **0** |
| Oráculo independiente en Python | 28 funciones (captadores de `pngget.c`), 4.000 registros cada una, 0 fallos |
| Prueba negativa (2 rondas) | **107 de 107 detectadas en la ronda 1 y 107 de 107 en la ronda 2** |
| Funciones enganchadas (detrás de `GTASA_WITH_ZLIB_LIBPNG`) | **104** (`source/game_sa/RenderWare/rw/libpngread.cpp`) |
| `validated=1` en `hooks.csv` | **ninguna**: evidencia fuera del juego; no se ejecutó `register_hook.py` (necesita corrida en juego y el juego está vedado a los agentes); no hay filas nuevas en `hooks.csv` |
| Opción por defecto | APAGADA (con ella apagada `libpngread.cpp` no engancha nada) |

Detalle por función en `estado_funciones.csv`; salida completa en `resultados/corrida_completa.txt` (cada función en su propio proceso, 16.384 pares); oráculo en `resultados/oraculo.txt`; rondas negativas en `resultados/negativas_ronda1.txt`, `negativas_ronda2.txt` y `negative-runs/*.json`.

## Método

`bench.cpp` (32 bits, enlazado en 0x400000 con `reserva_imagen.asm` de IND0A) mapea en memoria el `gta_sa.exe` original (copia `E:\JUEGOS\GTA San Andreas - CVehicle Test`, solo lectura) y llama a sus funciones por dirección. Cada caso se ejecuta **dos veces con la misma entrada y la misma arena de 16 MB** (dirección fija): con la función del EXE y con la de la biblioteca. Se compara: valor devuelto (entero o `float`, por bits), si hubo `png_error` (el callback del banco hace `longjmp`), el registro de callbacks (error, aviso, reserva/liberación con tamaños, lecturas), la **memoria entera de la arena** (png_struct, png_info, buffers, salidas), la palabra de control del FPU y la pila x87. Los punteros a código de EXE y biblioteca y las direcciones de pila se consideran iguales (`ptrnorm`). Cada función se prueba con estado aleatorio coherente (png_struct/png_info con campos de todo tipo, buffers, cadenas, tablas de texto, trozos desconocidos, paletas, `row_info`, flujo de lectura) y límites de dominio por función (`gen_table.py`, letras de `bench.cpp`).

**FPU a 24 bits** (`_controlfp(_PC_24)`, como lo deja Direct3D en el juego) en las dos ejecuciones; biblioteca compilada con `/arch:IA32` (x87 puro), `/O2 /Ob2 /Oy- /GS- /Gd /Zp8`. No se usa ninguna comparación `!(a<=b)`.

### Configuración con la que el EXE coincide (hallazgo clave)

El EXE no usa libpng 1.0.12 con la configuración por defecto: el `png_struct` que reserva `png_create_struct_2` mide **0x1F4** bytes y el `png_info` **0x120** (con la configuración por defecto de la fuente: 0x258 y 0x120). Se descubrió porque `png_malloc` del EXE lee `malloc_fn` en +0x1EC y por los accesos a campos de `png_struct` de las funciones emparejadas. La disposición exacta se obtiene **sin tocar la fuente**, solo con definiciones de compilación (`libs/libpng-1.0.12/CMakeLists.txt`, `build_bench.cmd`):

- `PNG_USER_MEM_SUPPORTED` (el EXE tiene `png_create_read_struct_2`; añade `mem_ptr/malloc_fn/free_fn`),
- `PNG_NO_PROGRESSIVE_READ` (sin lectura progresiva: –80 bytes),
- `PNG_NO_STDIO` (sin `time_buffer` en `png_struct` ni `png_init_io`/RFC 1123: –4 bytes).

El banco lo comprueba con un `static_assert` (`sizeof(png_struct) == 0x1F4 && sizeof(png_info) == 0x120`). **Es imprescindible para enganchar**: con otra disposición, las funciones de la biblioteca leerían los campos de un `png_struct` creado por el EXE en otro desplazamiento. Ya está aplicado en el `CMakeLists.txt` de la biblioteca (PUBLIC).

### Mapa EXE ↔ fuente (confianza)

El mapa de LIB1A (`mapa_funciones.csv`, confianza media/baja) estaba desplazado en varios bloques (`pngget.c`: captadores permutados; `pngset.c`: `png_set_read_user_chunk_fn`, `png_set_rows`, `png_set_compression_buffer_size`, `png_set_keep_unknown_chunks` en otras direcciones; `png.c`/`pngmem.c`: `png_zalloc` estaba en la dirección de «`png_check_sig`», `png_error`/`png_warning` en otras…). Se sustituyó por una resolución **por comportamiento**: `resolve.py` prueba cada función de la biblioteca contra cada una de las 343 funciones descubiertas del EXE (cada candidata en su propio proceso, 60–150 casos aleatorios) y `disambig.py` repite las candidatas múltiples con 4.000 casos. Las direcciones resultantes están en `confirmadas.csv` y en la columna 5 de `tabla.txt`; **cada una está respaldada por 16.384 pares sin diferencias**, así que para estas 107 funciones la confianza del mapa pasa a *alta* (equivalencia de comportamiento, no solo de forma). Correcciones destacadas: `png_get_channels` 0x7D4E60 (no 0x7D4BC0), `png_get_rows` 0x7D4BC0, `png_get_sRGB` 0x7D5090, `png_get_PLTE` 0x7D5440, `png_get_sBIT` 0x7D5480, `png_zfree` 0x7D2E00 (un `jmp png_free`), `png_set_expand`/`png_set_palette_to_rgb`/`png_set_gray_1_2_4_to_8` = 0x7D5FE0/0x7D5FF0/0x7D6000 (las cuatro de 0x7D5FE0..0x7D6010 son idénticas, `transformations |= PNG_EXPAND`; la asignación por orden del fuente es indistinguible).

Funciones de la biblioteca que **no están en el EXE** (las descartó el enlazador): `png_check_sig`, `png_get_iCCP`, `png_get_user_chunk_ptr`, `png_set_sPLT`. Las `*_inch*`/`png_get_pHYs_dpi` no se compilan en esta biblioteca. `png_set_tRNS_to_alpha` es una macro de `png.h`.

### Tandas

- **Tanda 1 – `pngget.c` + `png_sig_cmp` + `png_set_sig_bytes` (42 funciones)**, commit propio.
- **Tanda 2 – `pngset.c` (26 funciones)**: `png_set_bKGD/cHRM/cHRM_fixed/gAMA/gAMA_fixed/hIST/IHDR/oFFs/pCAL/sCAL/pHYs/PLTE/sBIT/sRGB/sRGB_gAMA_and_cHRM/iCCP/text/tIME/tRNS/unknown_chunks/unknown_chunk_location/keep_unknown_chunks/read_user_chunk_fn/rows/compression_buffer_size/invalid`. Incluyen la conversión x87 `(png_fixed_point)(x*100000.+0.5)` y `(float)double`: 0 diferencias con FPU a 24 bits.
- **Tanda 3 – `png.c`, `pngmem.c`, `pngerror.c`, `pngrio.c`, `pngread.c`, `pngrtran.c`, `pngrutil.c` (≈ 39 funciones)**: `png_create_info_struct`, `png_destroy_info_struct`, `png_info_destroy`, `png_free_data`, `png_zalloc/zfree`, `png_reset_crc`, `png_calculate_crc`, `png_handle_as_unknown`, `png_get_copyright/header_ver/header_version/io_ptr`, `png_reset_zstream`, `png_malloc`, `png_free`, `png_memcpy_check`, `png_memset_check`, `png_create_struct_2`, `png_destroy_struct_2`, `png_read_data`, `png_set_read_fn`, `png_error`, `png_warning`, `png_crc_read/finish/error`, `png_check_chunk_name`, `png_read_filter_row`, `png_read_finish_row`, `png_set_read_status_fn`, `png_set_expand` (×3), `png_set_crc_action`, `png_set_rgb_to_gray_fixed`, `png_set_read_user_transform_fn`, `png_do_gray_to_rgb`, `png_do_read_transformations`.

La lista completa por función (archivo fuente, dirección, estado, pares, diferencias, enganchada, motivo) está en `estado_funciones.csv`.

## Hallazgos del banco

1. **`png_get_pixel_aspect_ratio` (0x7D4D50)**: el EXE (MSVC 7.1) convierte `png_uint_32` a `float` con `fild` con signo y no redondea a `float` los operandos antes de dividir; la biblioteca (MSVC 19.5) convierte sin signo y redondea. Idénticas con `x_pixels_per_unit` e `y_pixels_per_unit` < 2^24 (16.384 pares sin diferencias en ese dominio); fuera de él (≥ 2^24, imposible en un PNG real; la opción `--ancho` lo activa) difieren en ≈ 12 % de los casos. **No se engancha.**
2. **Entradas fuera de dominio**: `png_set_keep_unknown_chunks` con `num_chunks` mayor que la lista pasada lee más allá del buffer y, al estar el destino en la misma arena, `rep movs` y `memcpy` difieren por solape (comportamiento indefinido); el banco limita `num_chunks` al tamaño del buffer. Los caminos con `NULL` en salidas sin comprobar (`png_get_PLTE` con `num_palette == NULL`, `png_get_IHDR`…) fallan igual en el EXE y en la biblioteca (el banco cuenta los fallos de ambos lados: columna «fallos»).
3. **Direcciones de pila**: algunas funciones guardan en el `png_struct` un puntero a una variable local (`png_read_finish_row`); se normalizan como punteros.
4. **`png_info_init`/`png_info_init_3`/`png_create_struct`** usan `malloc`/`free` del CRT sin pasar por el asignador del usuario: no se prueban (corromperían el montón del propio banco).

## Sin concluir

| Función | Motivo |
|---|---|
| `png_read_start_row` | la mejor candidata es 0x7E7530 (606 bytes; el mapa de LIB1A decía 0x7E1C90, que es otra función con `inflate`): coincide en ~80 % de los casos aleatorios, pero difiere en el resto (bytes de buffers de fila ±1, un campo de 16/32 bits y fallos del EXE en más casos que la biblioteca). No se engancha. Hace falta un estudio función a función del desensamblado frente a `pngrutil.c` |
| `png_build_gamma_table` (0x7D61A0), `png_init_read_transformations` (0x7DA080) | dependen de `pow()` del CRT. En el banco el CRT estático del EXE no está inicializado (el EXE falla en 2.743 de 16.384 casos) y los valores no son comparables; hace falta una corrida dentro del juego (con el CRT del EXE vivo) para decidir si la `pow` de la biblioteca (UCRT) da las mismas tablas. No se enganchan |
| Resto de `pngrutil.c`/`pngrtran.c`/`pngread.c` (`png_handle_*`, `png_do_*` salvo `gray_to_rgb`, `png_read_png/info/rows/end/image/update_info/destroy`…) | no incluidas en esta entrega: cada una necesita un flujo de chunks coherente en el banco; ver «Qué falta» |
| Las 63 «sin equivalencia» del cierre de LIB1A | sin tocar; la mayoría son de escritura (`pngw*`, `deflate`), `rtpng` y fragmentos de la descubierta |

## Prueba negativa de 2 rondas (`mutate.py`, `neg_run.py`)

Una mutación intermitente (cada 5 llamadas, `lib2a_mutq(k)`) por función, en una copia de `libs/libpng-1.0.12` (la buena nunca se toca; `LIB2A_MUT=k` selecciona la mutación). Ronda 1: se perturba el primer `return` (`^ 1`, `+ 1.0f`, puntero `+ 1`) o, en las `void`, `png_ptr->flags ^= 0x80000000` (o `row[0]` / salto del cuerpo si no hay `png_ptr`); ronda 2: se perturban **todos** los `return` (`^ 2`…) o `png_ptr->mode ^= 0x40000000`. Se aplican a todas las definiciones de la función (hay variantes bajo `#ifdef`). La primera versión de las mutaciones dejó 5 y 6 sin detectar, lo que reveló defectos de la propia prueba (no del banco): `png_reset_crc` (la perturbación de `crc` se sobrescribía: mutante equivalente), `png_malloc/free/create_struct_2/destroy_struct_2` (se mutaba la primera definición del `#ifdef`, no la activa), `png_info_destroy` (borra el `info`) y `png_get_copyright` (el último `return` estaba en una rama inactiva); se corrigió el generador y se repitió todo.

Resultado: las 107 mutaciones se detectan en cada ronda (`resultados/negativas_ronda1.txt`, `negativas_ronda2.txt`; entre 9 y 600 pares con diferencias por función mutada de 3.000 probados).

## Hooks (opción apagada por defecto)

`gen_hooks.py` genera `source/game_sa/RenderWare/rw/libpngread.cpp` (+ `.h`): 104 wrappers `__cdecl` que reenvían a la biblioteca, instalados con `RH_ScopedGlobalInstall(…, dirección)` solo si `GTASA_WITH_ZLIB_LIBPNG=ON`; con la opción apagada el archivo no engancha nada. Se llama desde `InjectHooksMain.cpp` junto a `ZlibInflateInjectHooks()`. Quedan fuera: `png_get_pixel_aspect_ratio` (diferencia conocida), `png_error`/`png_warning` (la ruta por defecto de `png_error` hace `longjmp` con el `jmp_buf` del CRT del EXE; sin corrida en juego no se garantiza) y las 3 sin concluir. Las funciones de la biblioteca que llaman a `png_error` dependen de esa misma compatibilidad de `longjmp` (misma disposición de `jmp_buf` en x86; sin probar en juego).

Compilación del proyecto (Debug, bajo `compile_lock`, `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1`, `_build_LIB2A_proj.sh`): **Debug con la opción APAGADA (por defecto, `build-project.ps1`): correcto** (`gta_reversed.asi` SHA256 `436af8ef…b738c`, `build_proyecto.out`); **Debug con `GTASA_WITH_ZLIB_LIBPNG=ON`: correcto** (SHA256 `de325821…3948`, `build_proyecto2.out`). El primer intento con la opción encendida falló al enlazar: `inflate_copyright` (que `-Dlocal=` vuelve externa en `inftrees.c` de zlib 1.1.3) chocaba con la de la zlib 1.2 que lleva `turbojpeg-static.lib`; `libs/zlib-1.1.3/CMakeLists.txt` la renombra con una definición de compilación (`inflate_copyright=zlib113_inflate_copyright`, sin tocar la fuente). No se ejecuta el juego: el `.asi` encendido no se ha probado en juego.

## Qué falta

- **Corrida en juego** (sonda en `CGame::Process`, `register_hook.py`) de `ZlibInflate` + `libpngread` con la opción encendida; solo entonces `validated=1` y valorar activar la opción por defecto.
- `png_read_start_row`, `png_build_gamma_table`, `png_init_read_transformations` (pow del CRT), `png_read_png/info/rows/end/image/update_info/destroy`, los `png_handle_*` (banco con flujo de chunks), `png_do_*` restantes, `pngtrans.c`, `png_get_uint_16/int_32` (macros en la biblioteca, funciones en el EXE), `rtpng` (0x7CF600/0x7CF9B0).
- Escritura (`pngwrite/pngwutil/pngwtran` + `deflate` 1.1.3).
- Compilar y probar la configuración Release.

## Archivos

`bench.cpp`, `build_bench.cmd`, `syntax.cmd`, `reserva_imagen.asm` (banco); `gen_table.py`, `protos.py`, `tanda1.txt`…`tanda3.txt`, `tabla.txt`, `libsyms.inc` (tabla de funciones y símbolos); `resolve.py`, `disambig.py`, `rank.py`, `confirmadas.csv`, `candidatas.csv`, `ausentes.txt`, `exe_funcs.csv`, `exe_addrs.txt`, `exe_lib.py` (mapa); `runeach.py` (corrida por proceso); `oracle_get.py`, `oraculos.txt`, `layout.txt` (oráculo); `mutate.py`, `neg_run.py`, `negative-runs/` (prueba negativa); `gen_hooks.py`, `enganchables.txt`, `validadas.txt`, `gen_estado.py`, `estado_funciones.csv`; `resultados/`.

Comandos: `build_bench.cmd <dir> [libpng mutada]`; `python gen_table.py tanda1.txt tanda2.txt tanda3.txt`; `python runeach.py <bench.exe> <gta_sa.exe> 16384 resultados/corrida_completa.txt`; `python mutate.py <1|2> <dir>`; `python neg_run.py <ronda> <bench_mutado.exe> <gta_sa.exe> 3000`.
