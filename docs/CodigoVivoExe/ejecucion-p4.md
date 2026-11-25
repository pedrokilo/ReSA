<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución de P4 (enlazar bibliotecas): E1-1, E1-2 (libpng + zlib) y E3 (D3DX9)

Rama `vivo-p4` (desde `origin/main` 410d9f2d84, 21/11/2026). El CRT (E2-*) lo lleva otra rama y no se toca aquí. Todo el trabajo es estático más el humo en juego; sin banco por función (decisión del usuario: basta lo que ya existe y el humo).

## 1. Qué significa «enlazar» aquí

El código propio llama a la biblioteca enlazada (`libs/libpng-1.0.12`, `libs/zlib-1.1.3`, o `d3dx9_43.dll`) en lugar de a direcciones del EXE. Con eso las funciones del EXE dejan de tener llamadores y se descartan (el cuerpo embebido no se ejecuta ni se reconstruye).

## 2. ¿La biblioteca de `libs/` es la que usa el EXE? (decisión con evidencia)

**Sí: libpng 1.0.12 (8-jun-2001) y zlib 1.1.3.** Cuatro pruebas independientes:

| Prueba | EXE canónico (`72ae59e4…ecc9b`) | `libs/` |
| --- | --- | --- |
| Cadena de copyright de libpng | « libpng version 1.0.12 - June 8, 2001 … Glenn Randers-Pehrson » (`.rdata`, y la cadena «1.0.12» que pasa `png_create_*_struct`, 0x8DEDF4) | `png.h`: «libpng version 1.0.12 - June 8, 2001», `PNG_LIBPNG_VER_STRING "1.0.12"` |
| Cadenas de zlib | « inflate 1.1.3 Copyright 1995-1998 Mark Adler », « deflate 1.1.3 Copyright 1995-1998 Jean-loup Gailly », «1.1.3» (`zlibVersion`) | `inftrees.c`, `deflate.c` y `ZLIB_VERSION "1.1.3"` con los mismos textos |
| Disposición de datos | `png_struct` de 0x1F4 bytes y `png_info` de 0x120 (`png_create_struct_2` y accesos a campos) | el `CMakeLists.txt` de la biblioteca define `PNG_USER_MEM_SUPPORTED`, `PNG_NO_PROGRESSIVE_READ`, `PNG_NO_STDIO` (y sin iTXt/tIME de escritura) y el banco de LIB2A comprueba `sizeof(png_struct) == 0x1F4` |
| Comportamiento | LIB2A (`reversing/FnLIB2A`): 107 funciones de lectura comparadas con la biblioteca (16.384 pares cada una, FPU a 24 bits, 0 diferencias, prueba negativa 107/107 en dos rondas); LIB1A/LIB4A: inflate y escritura | — |

Diferencias conocidas y aceptadas (LIB2A): `png_get_pixel_aspect_ratio` (el EXE, MSVC 7.1, convierte `png_uint_32` a `float` con signo; idénticas con valores < 2^24, imposible fuera de eso en un PNG real) y la configuración de compilación (no es la de defecto de la fuente, pero sí reproducible con las definiciones anteriores). La versión es la misma; lo que difiere es el compilador y las opciones del EXE.

## 3. E1-1 y E1-2: libpng/zlib (48 funciones, 0x7D26E0–0x7E77E0)

Las 48 funciones eran la parte de **lectura** de libpng que LIB2A no había podido enganchar (`png_create_read_struct_2`, `png_read_info`, `png_read_image`, `png_read_end`, `png_destroy_read_struct`, `png_set_strip_16`/`gray_to_rgb`, los `png_handle_*`, `inflate`, `png_build_gamma_table`, `png_init_read_transformations`, `png_read_start_row`…). Su única raíz era `rtpng.cpp` (`RtPNGImageRead`), que las llamaba por dirección del EXE; el resto eran llamadas entre ellas.

### Cambio

`source/game_sa/RenderWare/rw/rtpng.cpp`: `RtPNGImageWrite` y `RtPNGImageRead` llaman **directamente** a las funciones de la libpng enlazada (prototipos `extern "C"` con punteros opacos, como `libpngread.cpp` y `libpngwrite.cpp`), no a las 26 direcciones del EXE que había. Los callbacks de E/S obtienen el flujo con `png_get_io_ptr` (antes leían el desplazamiento 0x54 del `png_struct`). `png_create_read_struct_2` crea ya el `png_struct` de la biblioteca (el mismo tamaño, 0x1F4, y con la memoria de RenderWare como asignador), de modo que no se mezclan estructuras del EXE y de la biblioteca. El `setjmp` sigue siendo el de la DLL, que es el que usa `png_error` de la biblioteca. Todo bajo `GTASA_WITH_ZLIB_LIBPNG` (ON por defecto).

### Resultado estático

`contrib/codigo_vivo_cierre.py --lotes E1-1,E1-2`: **38 de 48 cerradas** (sin llamador desde código vivo, referencia C++ ni búsqueda bruta), 10 condicionadas: solo las llaman funciones **sin entrada de Ghidra** del mismo bloque libpng (huecos del `.text`: `png_create_read_struct` 0x7D3690, `png_read_png` ~0x7D49B0…, llamadores de 0x7D36D0, 0x7D3AB0, 0x7D3E90, 0x7D43E0, 0x7D5790, 0x7DEB60, 0x7E11E0, 0x7E13F0, 0x7E7530 y 0x7D3550) que a su vez no tienen más raíz que `RtPNGImageRead`. Se cierran con la parte 6 (huecos, H*).

### Callers de `RtPNGImageRead` (pendiente de fase 7: revisado)

Búsqueda de `call`/`jmp` rel32 y del dword en toda la imagen del EXE: **`0x7CF9B0` no tiene ningún llamador ni puntero** (no se registra con `RwImageRegisterImageFormat`; el juego no lee PNG). Solo `0x7CF600` (`RtPNGImageWrite`) tiene un llamador, `0x5A417C` (`WriteRaster`), que ya es C++ (`common.cpp`). El lado de lectura de libpng está por tanto muerto en el juego; que siga compilado y enlazado es solo para completar RtPNG.

## 4. E3: D3DX9

Los 4 enganches desactivados (`D3DXAssembleShader` 0x7652AE, `D3DXCompileShaderFromFileA` 0x76534D, `…FileW` 0x765402, `…FromResourceA` 0x7654B8) nacieron apagados el 20/11/2026 porque `d3dx9_43.dll` devolvía `D3DERR_INVALIDCALL` con `Flags = 4` y los modelos con esqueleto salían invisibles. FnD3DX9B ya había corregido la causa (banderas enmascaradas, reintento con `SKIPVALIDATION`) y `LoadSystemDll` restaura el estado x87/SSE tras cargar la DLL, pero los dejó apagados a falta de prueba en juego. Aquí se **activan** (se quita `{.enabled = false}`); quedan por defecto como el resto de los enganches.

### Prueba en juego (`--d3dx9-log`, todos los enganches activos, `cal_p4.args`)

* Con los cuatro activos el registro muestra los ensamblados del juego (`0x760C66`): fuente `vs_2_x` de piel (`mova a0.xyz`, `m4x3 … c[a0.x + 5]`), `flagsEXE=0x4 flagsDLL=0x0`, **`hr=0x00000000` en los 7 ensamblados registrados**, bytecode de 236 a 696 bytes, sin errores, **ningún reintento con `SKIPVALIDATION`**, sin aviso de cambio del estado de coma flotante al cargar la DLL. Las tres funciones de compilación HLSL no se llaman (nadie las usa en el juego).
* Aspecto: la captura del ropero (`sim_out/ropero_p4rop.png`) muestra a CJ completo, con esqueleto, textura y tatuaje; antes (con la DLL y `Flags = 4`) salía cortado o invisible.
* No se comparó el bytecode con el del ensamblador del EXE (ya no se ejecuta); la prueba es que el controlador acepta los shaders (`RwD3D9CreateVertexShader`) y el modelo se dibuja bien.

## 5. Comprobaciones

* Compilación completa de la DLL (`_build_VIVOP4.sh`, candado `compile_lock`, `reversing/CVehicle/build-project.ps1`): **EXIT=0**, sin errores (libpng y zlib se compilan y enlazan con la opción ON).
* Humo con la DLL propia (`AGUA_ASI`, todos los enganches activos, más `--d3dx9-log`): `prueba_ropero.py` → **BIEN** (puerta 2877 y marcador presentes); `prueba_agua.py` → **BIEN** (brillo medio 43,4, bajo el agua).
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 archivos con ensamblador o `naked`.
* Docs: `cierre.csv` (E1-1 y E1-2 «enlazado (parcial)», E3 «cerrado»), `cierre-p1.csv` y la columna `cierre` de `funciones-vivas.csv` (48 filas de E1), `plan.csv` y `plan-lotes.md` (con `contrib/codigo_vivo_plan.py`).
* Sin banco por función (decisión del usuario). La rama de escritura PNG (`RtPNGImageWrite`) no se ejecutó en el humo (hace falta una captura de pantalla del juego); su cambio es solo de enlace (las funciones de la biblioteca a las que ya enviaban los enganches de `libpngwrite.cpp`).

## 6. Pendiente

* **E1: 10 funciones condicionadas** (0x7D3550, 0x7D36D0, 0x7D3AB0, 0x7D3E90, 0x7D43E0, 0x7D5790, 0x7DEB60, 0x7E11E0, 0x7E13F0, 0x7E7530): las llaman funciones sin entrada de Ghidra del mismo bloque libpng (huecos del `.text`, parte 6). Cuando H* las nombre, las 48 se descartan juntas. Mientras tanto no tienen ninguna raíz fuera del bloque libpng.
* **Enganches LIB2A de lectura** (104, a direcciones del EXE) siguen instalados: son inertes ahora que nadie llama a la lectura de libpng del EXE; se retiran junto con el EXE (fase 8).
* **zlib**: los 14 enganches de `ZlibInflate` (0x7E9430–0x7EB260) ya enlazan la biblioteca; sin cambios.
* **CRT** (E2-*): no tocado, lo lleva otra rama.
* **D3DX9 interno** (V4-1…V4-4, 932 funciones): sigue «verificado sin cerrar» (llamadas por registro sin descartar con garantías); E3 era su dependencia y queda cumplida.
