# FnEXT1A: sustitución de lo externo, D3DX9 por la DLL del sistema y retirada de QuickTime (19/11/2026)

Dos decisiones del usuario aplicadas en la rama `ext1a` (sobre `origin/main` `cff02cea81`):

1. **D3DX9**: las 19 funciones de D3DX9 enlazadas dentro del EXE a las que llamaba el código propio pasan a usar `d3dx9_43.dll` (SDK de junio de 2010, en SysWOW64).
2. **QuickTime**: se quita QuickTime/QTML: el decodificador `CAEMFDecoder`, sus ayudantes y los 31 puentes QTML dejan de usarse.

## 1. D3DX9

Las 19 funciones son las filas de la categoría «biblioteca: D3DX9» / módulo «D3DX9 (compilador de shaders)» de `docs/ExeIndependiente/raices.csv`. Se identifican por el cuerpo del EXE y por las llamadas del código propio:

| # | Dirección | Qué es (EXE) | Sustituto | Banco EXE vs DLL |
|---|---|---|---|---|
| 1 | `0x76626B` | conversor escalar de medios flotantes (lo llama `0x7C1D80` con `count < 4`) | `D3DXFloat16To32Array` | 20.000 pares, 0 dif. |
| 2 | `0x766A32` | `jmp [0x8D74B8]` (tabla de despacho de CPU) | `D3DXVec3TransformCoord` | 20.000, 0 dif. |
| 3 | `0x766AB7` | `jmp [0x8D74A8]` | `D3DXVec3TransformNormal` | 20.000, 0 dif. |
| 4 | `0x7672E1` | `jmp [0x8D749C]` | `D3DXMatrixMultiply` | 20.000, 0 dif. |
| 5 | `0x7675EE` | `jmp [0x8D7524]` | `D3DXMatrixTranspose` | 20.000, 0 dif. |
| 6 | `0x7676C7` | `jmp [0x8D74A0]` | **rutina propia** (`D3DX9MatrixMultiplyTransposeSse.h`; la DLL redondea distinto) | 20.000 y 4.000 de borde, 0 dif. contra el EXE |
| 7–10 | `0x76B89B`, `0x76B940`, `0x76E5FB`, `0x76E664` | objeto «fuente» del compilador de shaders (ctor, dtor, copia de errores, errores pendientes) | sin exportación propia; solo las usaban los envoltorios 1–4 de abajo | n/a |
| 11–13 | `0x76DF03`, `0x76DF6C`, `0x76DFD1` | cargadores de la fuente (fichero ANSI/Unicode, recurso, memoria) | idem | n/a |
| 14–16 | `0x76FAEB`, `0x772BB1`, `0x771F63` | ensamblador de shaders (ctor, ejecución, dtor) | idem | n/a |
| 17–19 | `0x773022`, `0x77EE77`, `0x77303B` | compilador HLSL (ctor, ejecución, dtor) | idem | n/a |

Los envoltorios que ya reconstruía el proyecto sobre las rutinas 7–19 (hooks de `D3DX9`, `d3dx9math.cpp`) llaman ahora a la exportación del mismo nombre:

| Envoltorio (EXE) | Exportación de la DLL |
|---|---|
| `0x7652AE` | `D3DXAssembleShader` |
| `0x76534D` | `D3DXCompileShaderFromFileA` |
| `0x765402` | `D3DXCompileShaderFromFileW` |
| `0x7654B8` | `D3DXCompileShaderFromResourceA` |

Funciones sin equivalente en la DLL: **ninguna de las 19 queda sin sustituto**; las 13 rutinas internas (7–19) no existen como exportación porque son partes del compilador, y el código propio ya no las llama (el único código que las usaba eran esos cuatro envoltorios).

### Cambios de código

- `source/game_sa/RenderWare/rw/d3dx9math.cpp/.h`: nuevo `namespace D3DX9System` (carga perezosa con `LoadLibraryEx(..., LOAD_LIBRARY_SEARCH_SYSTEM32)` y, si falla, `LoadLibrary` normal; `GetProcAddress` por nombre; si falta la DLL o la exportación se registra el error con `NOTSA_LOG_ERR` una vez y las funciones de matemática devuelven `out` sin escribir y las de shaders `D3DERR_NOTAVAILABLE`). Los cuatro envoltorios usan esas exportaciones; se borra el código que llamaba a las rutinas 7–19 por dirección. `D3DXFloat16To32Array_3DNow` (0x7C1D80) llama a la exportación en lugar de `0x76626B`.
- `source/game_sa/RenderWare/rw/rpworld.cpp`: los cinco ayudantes `VS*` (matrices del vertex shader y transformación de puntos) llaman a `D3DX9System::*` en vez de a `0x7672E1`, `0x7675EE`, `0x7676C7`, `0x766AB7` y `0x766A32`.
- Requisito de ejecución nuevo: `d3dx9_43.dll` instalada (Runtime de usuario final de DirectX de junio de 2010) o junto al juego.

### Riesgo numérico y banco (fuera del juego)

`reversing/FnEXT1A/bench.cpp` (base 0x400000 con la reserva de `FnIND0A`, como `FnLIB1A`) mapea `gta_sa.exe` (solo lectura, copia `E:\JUEGOS\GTA San Andreas - CVehicle Test`, 5.189.632 bytes), reproduce el selector de variante de CPU del EXE (`0x781A87`: copia de la tabla por defecto `0x8D75B0 -> 0x8D7490`, `0x7ACF59` y después `0x7AC5E2` = SSE2 o `0x7AC441` = SSE; el camino 3DNow! no existe en las CPU actuales) y llama a la función del EXE y a la de `C:\WINDOWS\SYSTEM32\d3dx9_43.dll` con los mismos operandos. Casos de `gen_cases.py` (semilla 20261119, 144.000 casos): 20.000 normales + 4.000 de borde por función (infinitos, NaN, denormales, 3e38), con salida distinta de las entradas, salida igual al primer o segundo operando (alias) y desalineos de 0/4/8 bytes. Comparación bit a bit de la salida completa, del valor devuelto (puntero de salida) y de guardas de memoria; **oráculo independiente en Python** (doble precisión, desigualdades de tolerancia 2^-20·Σ|términos|; las medias palabras exactas) aplicado a las dos versiones: **0 fallos del oráculo en EXE y en DLL** en las seis funciones.

Resultado con la variante del EXE que elige esta CPU (SSE2, `resultados/corrida_sse2.txt`; con SSE, `corrida_sse.txt`, sale igual):

| Función | Pares normales | Dif. | Pares de borde | Dif. de borde |
|---|---:|---:|---:|---:|
| D3DXFloat16To32Array | 20.000 | 0 | 4.000 | 0 |
| D3DXVec3TransformCoord | 20.000 | 0 | 4.000 | 0 (31 casos con NaN en ambos lados, solo difiere la carga útil del NaN) |
| D3DXVec3TransformNormal | 20.000 | 0 | 4.000 | 0 |
| D3DXMatrixMultiply | 20.000 | 0 | 4.000 | 0 |
| D3DXMatrixTranspose | 20.000 | 0 | 4.000 | 0 |
| D3DXMatrixMultiplyTranspose (rutina propia) | 20.000 | 0 | 4.000 | 0 |

- **`D3DXMatrixMultiplyTranspose` (0x7676C7): rutina propia, 0 diferencias.** La DLL redondea distinto (en el primer banco 12.488 de 20.000 casos difirieron 1–2 ulps), así que se reproduce el EXE: el desensamblado de la variante SSE2 (`0x7BA139`) y de la SSE (`0x7B7FF3`) es **la misma secuencia de instrucciones**, de modo que el selector de CPU no cambia el resultado. Orden exacto: se cargan las 4 filas de B (`movlps/movhps`), la fila i de P = A·B es `((A[i][0]·B0 + A[i][1]·B1) + A[i][2]·B2) + A[i][3]·B3` (`mulps` con el escalar repartido por `shufps` y tres `addps`), y la salida es la transpuesta (`unpcklps/unpckhps`); se lee todo antes de escribir (la salida puede ser A o B). Como MSVC puede intercambiar los operandos de `_mm_add_ps/_mm_mul_ps` (cambia el signo/carga útil de un NaN propagado), la regla de x86 (gana el NaN del primer operando, silenciado; si no, el del segundo) se fija a mano en `D3DX9MatrixMultiplyTransposeSse.h`. Resultado del banco contra el EXE: 20.000 pares normales y 4.000 de borde (inf, NaN, denormales, 3e38, con alias y desalineos), 0 diferencias, 0 diferencias de NaN, 0 de retorno, 0 guardas. El hook de `0x7676C7` usa la rutina propia y `D3DX9System` solo usa la DLL para las otras 18 funciones. La variante x87 de reserva (`0x7ACECB`) y la 3DNow! no se reproducen: no las elige ninguna CPU actual (con la variante x87 del EXE el banco da diferencias, ver `corrida_x87.txt`).
- Con la variante x87 del EXE (la de reserva cuando la CPU no tiene SSE; `corrida_x87.txt`) cinco funciones (las cuatro de antes y la rutina propia) difieren de la DLL (Coord, Normal, Multiply, MultiplyTranspose): esa variante no se elige en ningún equipo actual.
- La DLL elige su propia variante por CPU, así que en CPU antiguas (sin SSE2) el resultado de D3DX9 puede cambiar respecto al EXE; no medido.

Prueba negativa de 2 rondas (`resultados/negativas.txt`): seis mutaciones por ronda aplicadas al lado DLL/propio solo en entradas concretas (una por función): **12 de 12 detectadas** (ronda 1 y ronda 2), con 10–1.789 casos afectados cada una.

**Shaders (`0x7652AE`, `0x76534D`, `0x765402`, `0x7654B8`): sin banco EXE vs DLL.** El código del EXE usa el montón y el CRT del propio EXE, que no se inicializan en el proceso de prueba, y el compilador HLSL de la DLL es otra versión (d3dcompiler_43) cuyo bytecode no tiene por qué coincidir bit a bit con el del D3DX9 del EXE. Solo hay una prueba de humo (`smoke_shader.cpp`, `resultados/humo_shaders.txt`): las cuatro exportaciones existen y `D3DXAssembleShader` (vs_1_1) y `D3DXCompileShaderFromFileA/W` (vs_2_0) devuelven `S_OK` con un búfer. Ningún camino del juego que se sepa usa estos envoltorios en ejecución normal (solo los llama el código interno del D3DX9 del EXE, fuera de las 19).

**Registro:** las seis funciones matemáticas (ahora enganchadas: `0x76626B`, `0x766A32`, `0x766AB7`, `0x7672E1`, `0x7675EE`, `0x7676C7`) se registran con `register_hook.py` y evidencia fuera del juego archivada en `reversing/CppTwelveLevel6Final/runs/FnEXT1A-20261119-bench/` (`validated=1` en `docs/hooks.csv`, con ese alcance anotado). Los cuatro envoltorios de shaders y las `CAEMFDecoder` modificadas siguen en `validated=0`. Prueba en juego: `EJECUTAR.md`.

## 2. QuickTime

Qué dependía de QuickTime: solo el decodificador de **user tracks** (pistas propias de la radio, «User Tracks»): `CAEUserRadioTrackManager::LoadUserTrack` construye `CAEMFDecoder` para las extensiones con tipo `AUDIO_FILE_TYPE_QUICKTIME` de la tabla `0x8CBB28`: **`.mp3`, `.aac`, `.m4a` y `.mov`**. No hay ningún otro uso en el proyecto (búsqueda en `source/`): ni la radio normal, ni las bandas sonoras, ni el vídeo.

Sustitución elegida (la mínima segura): **desactivar el decodificador QuickTime con registro claro**, que es el camino que ya seguía el juego en un equipo sin QuickTime.

- `CAEMFDecoder::InitLibrary` devuelve siempre `false` (registra una línea con `NOTSA_LOG_INFO`) sin cargar nada. `CAEUserRadioTrackManager::Initialise` ya reencamina entonces los `.mp3` al decodificador WMA (`audioExtensionTypes[1].type = AUDIO_FILE_TYPE_WMA`) y marca el tipo QuickTime como no soportado (`LoadUserTrack` devuelve `nullptr` para él).
- `CAEMFDecoder` conserva su disposición (0xC4 bytes), su vtable (`0x85F370`) y el hook del constructor/destructor; `Initialise` devuelve `false`, `FillBuffer` devuelve `-1`, `Shutdown` no hace nada.
- Eliminados: `source/app/platform/win/QTMLRuntime.cpp/.h` (`InitializeQTML`, `TerminateQTML`, carga de `QuickTime.qts`, 6 hooks: `0x8194B0`, `0x8194C0`, `0x819520`, `0x8195C0`, `0x819770`, `0x81DAE0`), las llamadas a los **31 puentes** `0x818430..0x81DAA0` (más el gancho `0x819700` y el stub `0x8194D0`, que solo estaban como constantes en `QTMLRuntime.cpp`) y a los ayudantes `0x4E7990`, `0x4E7A80` y `0x4E7B80` (hooks `OpenSoundMedia` y `GetMediaSoundFormat` borrados). Búsqueda en `source/`: no queda ninguna referencia a direcciones de puentes (`0x818xxx`–`0x81Dxxx` solo aparecen en los thunks de importación de `ImportThunks.cpp`/`DelayLoadThunks.cpp`, que no son QuickTime).
- `docs/hooks.csv`: 8 filas eliminadas (6 de `QTMLRuntime`, `OpenSoundMedia`, `GetMediaSoundFormat`) y `validated=0` en `CAEMFDecoder` `Initialise`, `FillBuffer`, `InitLibrary`, `Shutdown` y `Destructor` (el cuerpo cambió: la evidencia anterior ya no vale).

**Funcionalidad que se pierde:** las pistas de usuario `.aac`, `.m4a` y `.mov` ya no se reproducen (solo las abría QuickTime; el original también las perdía sin QuickTime instalado). Los `.mp3` siguen por WMA, con su calidad y con sus requisitos (Windows Media Foundation/WMF SDK de Windows; en ediciones «N» sin el paquete de funciones multimedia no sonarán). Ogg, WAV y WMA no cambian. Una sustitución completa (decodificar `.mp3/.aac/.m4a` con Media Foundation, como insinúan los comentarios de NOTSA) queda como lote aparte.

## 3. Compilación y estado

Ver el final de este archivo (sección «Compilación»). Archivos del lote: `bench.cpp`, `gen_cases.py`, `build.cmd`, `reserva_imagen.asm`, `smoke_shader.cpp`, `resultados/` (`corrida_sse2.txt`, `corrida_sse.txt`, `corrida_x87.txt`, `detalle_*.txt` con las primeras diferencias de `MultiplyTranspose`, `negativas.txt`, `humo_shaders.txt`, `sha256.txt`: SHA256 de `cases.bin` y de `bench.exe`). Regenerar: `python -I gen_cases.py cases.bin 20000`, `build.cmd out`, `out\bench.exe cases.bin <gta_sa.exe> sse2 0`.

## Compilación

Proyecto completo en Debug (`reversing/CVehicle/build-project.ps1`, bajo `compile_lock`, `CMAKE_BUILD_PARALLEL_LEVEL=1`, `_CL_=-MP1`, worktree `wv-13`): **correcto** (EXIT=0, sin errores de compilación ni de enlace; el primer intento falló solo en el enlace por falta de espacio en D:, LNK1108, y se repitió). Sin ejecutar el juego: ni la prueba en juego ni `register_hook.py` se han hecho.
