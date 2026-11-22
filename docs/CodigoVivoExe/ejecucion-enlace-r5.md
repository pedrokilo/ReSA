# Enlace de terceros y retirada de dependencias del EXE, ronda 5 (22/11/2026, rama `enlace-r5`)

Punto de partida: `docs/CodigoVivoExe/ejecucion-ronda4.md` sec. 5, puntos 4 a 7. Decisión del usuario: las bibliotecas de terceros se **enlazan**, no se reconstruyen. Base: `745cee8725`.

## Resultado

| | Antes (ronda 4) | Después |
|---|---|---|
| Cierre transitivo del código original necesario (censo estático) | 521 funciones / 264.358 B | **13 funciones / 9.188 B** (RpWorld streams y mallas, módulo de texturas de rwcore y `0x820A10`, todo de otros agentes) |
| Raíces sin gancho (A+B) | 112 | 15 |
| libjpeg 6b (raíces `0x5C2xxx-0x5CFxxx`) | 67 funciones / 16.938 B | **0** |
| D3DX9 (compilador de shaders) | 303 funciones / 129.610 B | **0** |
| CRT (las 10 raíces pedidas) | 1.776 B | 626 B (solo `0x820A10`, que no es del CRT, ver 3) |
| QuickTime `0x819700` | 108 B | **0** |

Censo: `python -I reversing/FnFase8/ganchos_fuente.py . ; python -I reversing/FnIND0A/scan_obj.py --obj-dir <obj de build/> --out reversing/FnIND0A/datos/refs_codigo.csv ; python -I reversing/FnIND0A/analizar.py --exe <gta-sa.exe>` sobre los `.obj` de esta rama (1.178 objetos, 47.861 referencias). Los CSV nuevos están en `reversing/FnIND0A/datos/` (la instantánea de la ronda 4 sigue en `docs/CodigoVivoExe/fase8-experimento/*-ronda4.*`).

## 1. libjpeg 6b: enlazada, sin direcciones del EXE

`libs/libjpeg-6b` (fuente oficial sin cambios, 46 `.c` con envoltorios y `exe_config.h`) ya se enlazaba con `GTASA_WITH_LIBJPEG` (encendida por defecto en `source/CMakeLists.txt`) tanto en el ASI como en el independiente (`enlazar.cmd` añade `libjpeg6b.lib`, `libpng1012.lib` y `zlib113.lib`). Lo que mantenía las 67 funciones como dependencia era el **respaldo**: `LIBJPEG_CB(direccionExe, archivo, nombre)` devolvía `fn ? fn : direccionExe`, y la dirección quedaba como inmediato en el `.obj`.

* `LibJpegCallback.h`: con la opción encendida la macro descarta la dirección en el preprocesador (`LibJpegExe::Callback(archivo, nombre)`), y si la biblioteca no exporta el nombre ejecuta `__debugbreak()`. Se comprobó con un script que los 63 usos de `LIBJPEG_CB` resuelven en las tablas `wrap/w_<archivo>.c`.
* Una dirección no pasaba por la macro: `self_destruct` (`0x5CF690`, 60 B) en `LibJpegMemory.cpp`. No se puede usar la de la biblioteca: libera el objeto con `free()` del CRT de la DLL y este código lo reserva con el `malloc` del EXE (`jpeg_get_small`). Se reconstruyó como `ExeSelfDestruct` (`free_pool(1)`, `free_pool(0)`, `jpeg_free_small(mem, 0x54)`, `cinfo->mem = 0`, `jpeg_mem_term`, leído de los bytes del EXE) y se engancha en `0x5CF690` (fila nueva en `hooks.csv`, sin validar).
* Verificación de la captura JPEG: `reversing/FnLNK5A/prueba_jpeg.py` (ver sec. 6).

## 2. D3DX9: ya iba por `d3dx9_43.dll`; se retira el ensamblador propio

El generador de vertex shaders y las funciones de D3DX9 (`D3DXAssembleShader`, `D3DXCompileShaderFromFileA/W/ResourceA`, matemática de matrices, medios flotantes) ya llamaban a `d3dx9_43.dll` desde el lote EXT1A (`D3DX9System` en `d3dx9math.cpp`, carga dinámica con `LoadLibraryEx`, que guarda y restaura el estado x87/SSE que la DLL reinicia al cargarse; por eso no se añadió `d3dx9.lib` con enlace de importación: la vía ya existente cubre las 19 funciones). Los cuatro envoltorios están enganchados en `0x7652AE`, `0x76534D`, `0x765402` y `0x7654B8`, así que **nada llega** a las rutinas internas del D3DX9 estático del EXE.

Esas rutinas internas tenían su reconstrucción propia (lote NEWDEL: `D3dxErrorLog`, `D3dxShaderSource`, `D3dxShaderAssembler`, `D3dxHlslCompiler`; 8 archivos, 808 líneas), código muerto que citaba 29 direcciones del EXE (`0x7725E3` 1.238 B, `0x76BA49`, `0x7716DD`, `0x76BA24`, `0x770835`, ...; las 303 funciones del cierre) y además `_controlfp` (`0x825136`) y `setlocale` (`0x825A79`) del CRT.

* **Retiradas** las 4 clases (8 archivos), su `InjectHooks` en `InjectHooksMain.cpp` y 12 filas de `hooks.csv` (las 11 de las clases y `D3DX9,D3dxCompileShaderEntry`).
* Raíces D3DX9 que siguen vivas: **ninguna** del compilador de shaders. De la lista de la ronda 4 siguen en el cierre solo `0x7F2930` (`RwMatrixQueryRotate`, rwplcore) y las de RpWorld, que no son D3DX9 (otro agente).
* Las 6 funciones matemáticas de `d3dx9math.cpp` (`0x76626B`, `0x766A32`, ...) siguen enganchadas, y `D3DXMatrixMultiplyTranspose` sigue siendo propia (la DLL redondea distinto). `sync_validated_flags.py` bajó a `validated=0` esas 6 filas: su evidencia está atada al hash de `InjectHooksMain.cpp`, que cambia con esta rama (la validación de las funciones no cambió).

## 3. CRT: las 10 raíces

| Raíz | Qué es | Resolución |
|---|---|---|
| `0x820A10` (626 B) | **No es del CRT**: `RwD3D9DDSTextureRead` (rtdds de RenderWare), la llama `rwcore.cpp` por dirección; Ghidra no la reconoce como función | **Sigue viva.** `rwcore.cpp` y las texturas son del otro agente; ver riesgos |
| `0x824570` (466 B) | `entry` (WinMainCRTStartup); `NOTSA_WinMain` leía su primer byte para saber si el `.text` está relleno de INT3 | `ExeCrt::StandaloneStartupRan()` (marca que pone `StandaloneStartup`; en el ASI sobre el EXE siempre false) |
| `0x825A79` (346 B) | `setlocale`, solo citada por `D3dxShaderSource` | desaparece con la sec. 2 |
| `0x824046` (129 B) | `_getenv_helper_nolock`, la llamaba `ExeCrt::Getenv` por dirección | `ExeCrt::GetenvHelperNolock` (hook `0x824046`). No reconstruye la rama `_wsetenvp` (`0x82C3EC`, solo con `_environ` nulo y `_wenviron` presente): devuelve NULL |
| `0x8212ED` (98 B) | lanza `std::bad_alloc` (objeto estático del EXE + `_CxxThrowException` con ThrowInfo `0x8A1ED4`) | `ExeCrt::ThrowBadAlloc` (hook `0x8212ED`): `throw std::bad_alloc()` del CRT enlazado (mismo nombre decorado, un `catch` del EXE lo atrapa) |
| `0x8244D1` (37 B) | `_amsg_exit` | `ExeCrt::AmsgExit` (hook) con `FfMsgBanner` (`0x82D6C1`) y `NmsgWrite` (`0x82D520`), también reconstruidas y enganchadas. La ventana del error usa `MessageBoxA` en lugar de `__crtMessageBoxA` (`0x835177`) |
| `0x826B26` (26 B) | rutina nº 4 de la tabla de intrínsecas `0x8E30F0`: `return *(double*)p >= 0.0` (compara con la constante 0.0 de `0x859EF8`) | `ExeCrt::FpIntrinsicNonNegative`; `FpInitDispatch` guarda su dirección en la tabla |
| `0x821397` (23 B) | `__fpmath`, que `Cinit` comparaba con el valor de `_FPinit` | `Cinit` llama directamente a `FpMath()` (único valor posible en este EXE) |
| `0x825136` (22 B) | `_controlfp`, citada por `D3dxShaderSource` | desaparece con la sec. 2 |
| `0x833605` (3 B) | `matherr` por defecto (`xor eax, eax; ret`) | `CrtMathInjectHooks` sustituye el puntero `0x8E3128` sin comparar con la dirección del EXE |

Quedan tres inmediatos de CRT en `ExeCrt::Floor/Ceil/Pow` (`0x828A60`, `0x82BA31`, `0x822184`): son las ramas x87 sin SSE2/SSE4.1 que no se ejecutan en equipos actuales y no entran en el cierre; no estaban en la lista.

## 4. QuickTime

`QtmlInitialize` (`0x8195C0`) y `QtmlTerminate` (`0x819770`) escribían la dirección del gancho `0x819700` en `0x8E2A08`. Nada del juego las llama desde EXT1A (`CAEMFDecoder::InitLibrary` devuelve false). Retiradas con sus ganchos y sus filas de `hooks.csv`; se conservan los tres puentes y `QtmlConverterFillBridge` de `QTMLRuntime.cpp` (no llevan direcciones sin gancho).

## 5. Regeneración de documentos

`python contrib/audit_hook_catalog.py --strict`: OK. `python contrib/sync_validated_flags.py` y `python contrib/sync_docs.py` aplicados (13.497 funciones reversed=1, 11.792 validadas).

## 6. Verificación

Compilación completa del ASI (`/d/Original/_build_LNK5A.sh`, bloqueo `compile_lock`): `EXIT=0`. Independiente: `/d/Original/_build_LNK5S.sh` (`reversing/FnFase8/enlazar.cmd` con los `.obj` de esta rama): enlace `exit=0`, sin símbolos sin resolver.

Humo (DLL de la rama sobre la copia de pruebas, `D:\Original\wv-8eversing\FnBIS1`, `AGUA_ASI` = `bin/Debug/gta_reversed.asi` de `wv-lnk5`):

| Prueba | Resultado |
|---|---|
| `prueba_ropero.py cal_mal.args` | **BIEN** (puerta True, marcador True, 15 entidades, 12 marcadores). Captura revisada con la herramienta de imagen: la puerta con su textura, el espejo con CJ de espaldas (gorra verde, cadena de oro, tatuaje) y la tabla de surf; nada negro ni sin textura |
| `prueba_marcadores.py cal_mal.args` | **BIEN** (12 marcadores en memoria y los 12 cambian en 1 s) |
| `prueba_partida_nueva.py --segundos 300 --entrada` | **BIEN** (`gGameState` 9, frames suben, sin popups; solo el error conocido del pool `CEntryExit`) |
| `prueba_agua.py` (primer ASI de la rama, antes del arreglo de sec. 7) | BIEN (`UnderWaterness` 1,0, brillo medio 48,1); no se repitió por indicación del coordinador |
| Captura JPEG, `reversing/FnLNK5A/prueba_jpeg.py` (pone `m_bSavePhotos` y `CWeapon::ms_bTakePhoto` desde fuera, como `prueba_partida_nueva.py`) | ASI de wv-int (sin el arreglo): **MAL**, `gallery*.jpg` de 0 B y `JPegErrorExit`. Con esta rama: **BIEN**, `gallery3.jpg` de 285.619 B, empieza por `FF D8 FF`, acaba en `FF D9` y se abre como imagen |
| Independiente (`reversing/FnFase8/ejecutar_f8b.ps1 -Limite 330`, partida nueva) | `gGameState` 9, 8.094 frames en 330 s, **0 INT3** (`codigo-original-distinto=0 hits=0 excepciones=0`, `fase8_codigo_original.csv` vacío); sale por la guardia de tiempo (`0xDEAD0002`) |

Al arreglo de la captura JPEG (sec. 7) le sigue una segunda compilación completa y un segundo enlace del independiente; el humo de la tabla es el de la versión final, salvo la fila de agua.

## 7. Hallazgo: la captura JPEG estaba rota desde que se encendió `GTASA_WITH_LIBJPEG`

`JPegCompressScreenToFile` (`JPegCompress.cpp`) usa la API de libjpeg-turbo de la DLL (`jpeg_CreateCompress(..., JPEG_LIB_VERSION = 80, ...)`). La libjpeg 6b de `libs/libjpeg-6b` exporta los mismos nombres globales `jpeg_*` (por sus envoltorios) y el enlazador los resolvía contra la 6b (`_jpeg_CreateCompress` en `libjpeg6b:w_jcapimin.obj` en el `.map`): «Wrong JPEG library version: library is 62, caller expects 80», archivo de 0 B. Arreglo: `libs/libjpeg-6b/jconfig.h` define `NEED_SHORT_EXTERNAL_NAMES` (mecanismo estándar de la IJG: `jCreaCompress`, ...), de modo que los símbolos de la 6b ya no chocan con los de turbo (en el `.map` ahora `_jpeg_CreateCompress` viene de `jpeg-static:jcapimin.obj`). Las funciones de la 6b se siguen alcanzando por `jpeg6b_find` (por nombre de cadena).

Además `reversing/FnFase8/enlazar.cmd` tenía un carácter de retroceso (0x08) en lugar de la barra invertida en la ruta `%REPO%\build\libs`, así que nunca refrescaba `lib\*.lib` desde la compilación (el independiente se enlazaba con copias viejas): corregido.

Pendiente análogo: zlib 1.1.3/libpng 1.0.12 (LIB1A/LIB4A) exportan `inflate`, `png_*`, etc.; si otra biblioteca del proyecto trae las suyas puede ocurrir lo mismo (no se comprobó).

## 8. Riesgos

* El humo es manual por naturaleza y comparte la copia de pruebas con otros agentes: los arranques simultáneos se pisan (las primeras rondas dieron «el juego se cerró» o «sin juego» por esa colisión, no por el código).
* `0x820A10` queda como única raíz de la lista que no se resolvió: `RwD3D9DDSTextureRead` es de RenderWare/texturas (archivo reservado a otro agente) y Ghidra no la delimita; hay que reconstruirla con gancho junto a `rwcore_dds.cpp`.
* `AmsgExit/NmsgWrite/FfMsgBanner/GetenvHelperNolock/ThrowBadAlloc` son caminos de error o poco usados, sin validar y sin banco.
* Seis filas de D3DX9 pierden `validated=1` por el hash de `InjectHooksMain.cpp` (no por cambio de código de esas funciones).
* `cl.rsp` de `reversing/FnFase8` se reescribió para este worktree (`wv-fase8c` -> `wv-lnk5`) y NO se commitea (`git update-index --assume-unchanged`).
