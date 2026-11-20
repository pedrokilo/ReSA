# FnLIB2A: libpng 1.0.12 (lectura) del EXE sustituida por la biblioteca oficial

Segundo lote del hito «EXE independiente» para PNG. Parte de LIB1A (`reversing/FnLIB1A`). Banco par a par FUERA DEL JUEGO: `bench.exe` (32 bits) mapea el `gta_sa.exe` original en 0x400000 (el archivo no se toca) y llama a sus funciones por dirección; cada caso se ejecuta dos veces con la misma arena de memoria (misma dirección): con la función del EXE y con la de `libs/libpng-1.0.12` + `libs/zlib-1.1.3` compiladas con `/arch:IA32` y con el FPU a 24 bits de mantisa (`_controlfp(_PC_24)`, como lo deja Direct3D en el juego).

Estado de la entrega por tandas: ver sección «Tandas» (se actualiza en cada commit).

## Tandas

### Tanda 1: `pngget.c` (captadores) + `png_sig_cmp` + `png_set_sig_bytes`

- 42 funciones con equivalente en el EXE, **16.384 pares por función, 0 diferencias** (`resultados/tanda1_corrida.txt`). 20 de ellas (los captadores escalares, de desplazamientos, de resolución, `png_get_rows`, `png_get_signature`) además contra un modelo en Python escrito desde la documentación (`oracle_get.py`, 4.000 registros por función, 0 fallos: `resultados/tanda1_oraculo.txt`); el resto solo EXE vs biblioteca.
- Prueba negativa de 2 rondas (`mutate.py`, `neg_run.py`, `negative-runs/`): una mutación intermitente por función; **42 de 42 detectadas en la ronda 1 y 42 de 42 en la ronda 2**.
- Mapa EXE ↔ fuente corregido por el propio banco (`resolve.py`, `confirmadas.csv`): el mapa de LIB1A estaba desplazado en el bloque de `pngget.c` (el orden del EXE sigue el de la fuente, con `png_get_rows` y `png_get_channels` en otro sitio y `png_get_pixels_per_meter`/`x`/`y` permutados). Candidatas únicas por comportamiento, 60 casos aleatorios.
- No están en el EXE (el enlazador las descartó): `png_check_sig`, `png_get_iCCP`, `png_get_user_chunk_ptr` (`ausentes.txt`). Las funciones `*_inch*`/`png_get_pHYs_dpi` no se compilan en esta biblioteca (`PNG_INCH_CONVERSIONS` no definido).
- **Configuración de compilación:** el EXE lleva libpng con `PNG_USER_MEM_SUPPORTED` (`png_create_read_struct_2`, `png_struct` con `mem_ptr/malloc_fn/free_fn`). Por defecto 1.0.12 no lo activa, así que `libs/libpng-1.0.12/CMakeLists.txt` define `PNG_USER_MEM_SUPPORTED` (PUBLIC). Sin parches a la fuente.
- **Diferencia conocida (1 función):** `png_get_pixel_aspect_ratio` (0x7D4D50): el EXE convierte `png_uint_32` a `float` con `fild` con signo (MSVC 7.1) y no redondea los operandos a `float` antes de dividir; la biblioteca (MSVC 19.5) convierte sin signo y redondea. Resultados idénticos cuando `x_pixels_per_unit` e `y_pixels_per_unit` < 2^24 (el banco compara 16.384 pares en ese dominio, 0 diferencias); fuera de él (valores ≥ 2^24, imposibles en un PNG real) difieren (≈12 % de los casos con el dominio completo, `--ancho`). **No se engancha** (queda fuera de `enganchables.txt`).
- Hooks: `source/game_sa/RenderWare/rw/libpngread.cpp` (generado por `gen_hooks.py`) engancha las 41 funciones restantes SOLO si `GTASA_WITH_ZLIB_LIBPNG=ON` (apagada por defecto); con la opción apagada no engancha nada.
