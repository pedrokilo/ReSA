# Fase 8: experimento real del ejecutable independiente (rama `fase8-exp`, 21/11/2026)

Base: `origin/main` d911da633b. Objetivo: construir y **ejecutar** un EXE generado desde este código, con el arranque del CRT y `WinMain` propios, usando el `gta_sa.exe` original **solo como fuente de datos** (se lee el archivo y se copian a sus direcciones fijas las secciones de datos; **no se ejecuta ni una instrucción suya**), y anotar qué se rompe de verdad en vez de estimarlo. Todo lo de abajo está medido salvo donde se dice «estimación».

Herramientas y datos: `source/app/platform/win/StandaloneExe.cpp` (cargador, entrada, `WinMain`, diagnóstico; solo compila con `GTASA_STANDALONE_EXE`, el ASI lo compila vacío), `reversing/FnFase8/` (scripts y registros de cada intento en `ejecuciones/`), `docs/CodigoVivoExe/fase8-experimento/` (CSV y `resumen.json` de dependencias actuales). Objetivo CMake opcional `GTASA_STANDALONE_EXE` → `gta_sa_standalone` (no toca el objetivo ASI). El experimento corre aparte de la prueba de humo del ASI: no se modificaron `prueba_ropero.py` ni nada de `wv-8`.

## 1. Cómo está construido el EXE de prueba

| Pieza | Qué hace |
| --- | --- |
| Enlace | Los `.obj` ya compilados del ASI Debug (mismo commit; copiados de `wv-int`) + `StandaloneExe.cpp` (compilado con `/DGTASA_STANDALONE_EXE`) + `reserva_imagen.asm` (reserva de 0x8B0000 bytes que deja el código propio desde 0xCB1000) como EXE de **base fija 0x400000** (`/FIXED`, `/ENTRY:FaseEntrada`). Enlaza con el CRT de MSVC (`/MTd`), zlib 1.1.3 / libpng 1.0.12 / libjpeg 6b del proyecto y, **por importación normal** (no por `delay-load` de los ganchos), `d3d9`, `ddraw`, `dsound`, `dinput8`, `winmm`, `dbghelp`, `ws2_32`, etc. (16 DLL en la tabla de importación del EXE). No lleva `d3dx9.lib` todavía (nada del código propio la referencia; los 12 envoltorios de D3DX9 siguen desactivados) |
| Mapa de memoria | Un callback TLS (`.CRT$XLB`, antes del CRT propio y sin CRT) lee el `gta_sa.exe`, comprueba tamaño y PE, copia **solo** `.rdata`, `.data`, `_TEXT_HA`, `_rwdseg` y `.rsrc` (641.024 bytes con valor inicial; los 3,9 MB de `.bss` quedan a cero) y rellena con **INT3** el rango de código original 0x401000–0x858000 (4.550.656 bytes; `.text` y `_rwcseg` **no se copian**). Resuelve la IAT original (214 importaciones, 0 sin resolver: el código propio lee ranuras de la IAT por dirección) |
| Arranque | `FaseEntrada`: `ExeCrt::StandaloneStartup()` (montón, TLS, bloqueos, E/S, `atexit`, constantes x87, en las direcciones del EXE) + `ExtraTablaC` (ver §3, ruptura 1) → `WinMainCRTStartup` del CRT propio (inicializadores estáticos propios, que ya usan `operator new` sobre el montón reconstruido) → `WinMain` propio |
| `WinMain` propio | `DllMain` del proyecto (lee INI y línea de comandos e instala los ganchos: **escriben su `JMP` en el rango de INT3**; las 11.378 rutas sobre direcciones del EXE y las 2.168 de vtable) → protecciones por sección → `Win::NOTSA_WinMain` (el `WinMain` reconstruido) |
| Diagnóstico | Manejador vectorizado: un INT3 en el rango original = el código propio llamó (o saltó) a una dirección del EXE sin reemplazo. Registra dirección, quien llama (`[esp]`), ECX y argumentos, registros y pila, y termina (**modo estricto**). Modo `--censo`: simula el retorno (EAX=0, `ret n` de una tabla calculada con capstone, `--stubs` explícitos) y cuenta; guardia contra bucles (400 aciertos). Latido y estado del juego en la ruptura (dispositivo D3D9, cámara, ventanas) |

Comprobación de que no se ejecuta código original: el rango entero es INT3, el manejador marca cada entrada como fatal (estricto) y las 5 rupturas observadas aparecen exactamente como «llamada a código original sin reemplazo» (`fase8_fallo.txt` de cada intento).

## 2. (a) Qué funciona (medido)

Tiempos de `ejecuciones/t4-*/fase8_arranque.log` (Debug, 13.546 ganchos):

| Hito | Resultado | Cuándo |
| --- | --- | ---: |
| Enlace con el CRT de MSVC y todas las bibliotecas | **0 símbolos sin resolver**; el EXE son 49 MB (`.text` 0x401000–0x2C97000; código propio desde 0xCB1000). Dos problemas triviales: `__tls_used` (nombre decorado x86) y el orden de `libjpeg6b` frente a `jpeg-static` | — |
| Carga de datos desde el original | 7 secciones: 5 de datos copiadas, 2 de código omitidas; IAT 214/214 | 0–94 ms |
| `StandaloneStartup` + CRT propio | correctos (montón, hilos, E/S, constantes x87 en las direcciones del EXE; inicializadores estáticos propios) | 94–203 ms |
| `DllMain` / instalación de ganchos en el INT3 | correcto: **13.546 funciones** reemplazadas, 13.541 activas | 203–1.062 ms (859 ms) |
| `NOTSA_WinMain` | alcanzado y ejecutado: registra la clase de ventana, `RsEventHandler(rsINITIALIZE)`, crea la ventana (SDL conectado al `HWND`), DirectInput (el `Acquire` del ratón se rechaza: ventana sin foco), carga `SPANISH.GXT`, abre `MODELS\GTA3.IMG`, crea el diccionario TXD | 1.062 ms → ~1,5 s |
| RenderWare y Direct3D 9 (todo código propio) | **dispositivo D3D9 creado** (`[0xC97C28]` ≠ 0), `RwCamera` creada, ventana «GTA: San Andreas» visible 1048×832; `malloc/new/free` sobre el montón reconstruido sin ninguna ruptura | ~1,5 s |
| Bucle principal | llega a ejecutar el bucle del juego (en el censo, `0x7F98D0` se llama >400 veces en 16 ms) | ~1,7 s |

No se ha visto ningún acceso inválido ni ninguna excepción de primera oportunidad en datos antes de las rupturas por código (la `.bss` a cero y la imagen de datos copiada bastan para todo lo que se ejecutó). Todavía **no** hay imagen en pantalla de ningún marco, ni menú, ni partida.

## 3. (b) Qué se rompe, en orden (cada fila es una ejecución real)

| # | Ejecución | Dirección original | Quién la llama (propia) | Qué es | Estado y causa |
| ---: | --- | --- | --- | --- | --- |
| 1 | `t1` | `0x822184` (rama x87 de `pow`, 0x8220F0) | `ExeCrt::Pow` | CRT: `pow` elige núcleo SSE2 o x87 según `[0xC9C400]` | **Arreglado en el experimento.** `StandaloneStartup` solo reconstruye 3 de las 7 entradas de la tabla C de `__cinit`; falta `0x8289F5` (detección SSE2: cpuid bit 26 + `IsProcessorFeaturePresent`) y `0x826A72` (8 bytes a 0xFF en 0x8E37B4). Ya van en `ExtraTablaC`. **Siguen sin hacerse:** `0x833494` (`_setmbcp(-3)`, tablas MBCS 0xC9AF8C…/0xC9C1A4…; agente de locale) y `0x82AC54` (filtro de excepciones no manejadas del CRT; lo cubre el CRT propio) |
| 2 | `t2` | `0x4CD820` (1.460 B, `rwSTANDARDNATIVETEXTUREREAD`) | `RwTextureGtaStreamRead` (`rwtexdict.cpp`) | puntero de la tabla estándar de rasters D3D9 (`0x7F72E0` escribe direcciones del EXE) | El plan lo clasifica **«muerto»** (`huecos-p6.csv`, `citada-en-cpp`/`muerto`); en realidad es la primera función grande que ejecuta el juego tras crear el diccionario TXD. La reconstruye `vivo-d3dstd` (sin integrar: usada como **superposición** de sus archivos de trabajo en `t3` en adelante; no es parte de `main`) |
| 3 | `t3` | `0x750430` (40 B), `0x750460` (30 B), `0x750480` (650 B) | `RwCameraBeginUpdate` y `RwCameraEndUpdate` (`rwcore.cpp`), constructor del plugin 0x509 (`rpworld.cpp`, escribe esas direcciones en la cámara) | envolturas del plugin de mundo de `RwCamera` (RpWorld): inicio y fin de actualización, y sincronización del BSP con el frustum | Plan: `0x750430`/`0x750460` «muerto»; `0x750480` ni figura. **Shims del experimento:** las dos pequeñas, exactas; la tercera solo hace la primera llamada y no recolecta sectores (el mundo no se dibujaría) |
| 4 | `t4` | `0x750AC0` (513 B) | `FrameSyncObjects` (`rwcore.cpp`) | callback de sincronización de marco con el mundo (RpWorld, mismo grupo) | no figura en ningún CSV del plan (está dentro de un hueco de Ghidra). **Sin shim; es donde termina el modo estricto** |
| 5 | `c3` (censo) | `0x7F98D0` (223 B), `0x7F99B0` (621 B) | `_rwD3D9…` de `rwcore.cpp` (puntero de tabla estándar 0x0A y 0x14) | `0x7F98D0`: copia el backbuffer (`StretchRect`) al raster-textura; `0x7F99B0`: cambio de modo de video / reinicio del dispositivo (`Reset`) | huecos de Ghidra del controlador D3D9; no están en el plan |

Resumen de lo que dicen estas 5 rupturas sobre el **plan** (esto es lo más importante del experimento):

* **Ninguna de las 8 direcciones rotas está entre las 5.087 «funciones vivas» del plan.** Todas son código que Ghidra no marcó como función (`huecos`): 3 están catalogadas como «muerto» (`0x4CD820`, `0x750430`, `0x750460`) aunque se ejecutan en los dos primeros segundos, y 5 no figuran en ninguna lista. «Muerto» se decidió por falta de llamadores directos; no vale para direcciones que el C++ **escribe como valores en tablas de punteros** (tabla estándar de rasters, callbacks de `RwCamera`).
* El orden real de ruptura lo marcan las **tablas de punteros a código** del motor (3.769 ranuras `.rdata` y 95 `.data` sin gancho según IND0A, 52 en vtables), no las llamadas directas.
* Coincide con lo que ya vio `datos-exe` (DE04-R: «35 objetos de `.data` que toca código del EXE no catalogado (huecos sin función)»).

> **Actualización 21/11/2026 (rama `vivo-crtb`, [ejecucion-crt-fase8b.md](ejecucion-crt-fase8b.md)):** la ruptura 1 (SSE2/`pow`) y `__initmbctable` quedan resueltas dentro de `ExeCrt` (`ExtraTablaC` desaparece de `StandaloneExe.cpp`; `StandaloneStartup` ejecuta `__cinit` entero: `__fpmath`, tabla C, cookie y constantes x87). Con el ejecutable independiente reconstruido el primer INT3 sigue siendo de RpWorld (0x750430) y el censo no registra ninguna llamada a código original del CRT.

## 4. (c) Qué falta, por categoría (cifras reales)

Medido de nuevo con los `.obj` actuales (`contrib`/IND0A `scan_obj.py` + `analizar.py`; `docs/CodigoVivoExe/fase8-experimento/resumen.json`):

| Categoría | Cifra actual | Antes (IND0A, 18/11) |
| --- | ---: | ---: |
| Referencias a direcciones fijas del EXE en el código propio | 66.277 en 18.087 direcciones | — |
| Direcciones de **código** citadas por el C++: con gancho / sin gancho | 13.419 / **379** (confianza A 119, B 147, ruido C 113) | 890 sin cuerpo propio |
| Cierre por llamadas del código original que sigue alcanzable | **691 funciones, 309.142 B** (RenderWare 459 / 181.547 B; CRT 138 / 102.946 B; juego 94 / 24.649 B) + 668 llamadas indirectas sin resolver + 39 funciones de la IAT | ≥ 1.572 funciones, ≥ 576.757 B |
| Raíces de ese cierre | 266 | — |
| Datos (todos por dirección fija) | 4.287 objetos / 49.528 referencias: `.bss` 2.879, `.data` inicial 687, vtables 475, constantes 191, cadenas 15, `_TEXT_HA` 38; 20.265 direcciones de datos usa todo el código original, 4.258 las comparte con el propio | 4.949 / 54.225 |
| Inicializadores `_initterm` | 1.673; 1.489 constantes x87 hechas; **sin hacer: 140 constructores con `atexit`, 35 asignaciones, 9 llamadas** (el experimento no los ejecuta; no se ha visto aún efecto, probablemente porque todavía no se pasa del primer marco) | igual |
| Imagen de datos que el EXE propio sigue leyendo del original | 641.024 B con valor inicial (311 KB `.rdata`, 262 KB `.data`, 69 KB `_TEXT_HA`, 0,5 KB `_rwdseg`, 1,5 KB `.rsrc`) | — |

Lectura honesta:

* **Código.** El cierre bajó un 56 % en funciones (1.572 → 691) y un 46 % en bytes desde IND0A. El resto es **RenderWare (459 funciones, 59 % de los bytes)**: plugins de `RwCamera`/marcos/mundo (RpWorld), controlador D3D9 (rasters, `Present`, `Reset`) y bibliotecas. En el experimento 7 de las 8 rupturas son de RenderWare/D3D9 y la otra del CRT; el bloque de CRT es pequeño (138 funciones, 103 KB) y en gran parte lo aportaría el CRT de MSVC enlazado (decisión `ENLAZAR` del plan).
* **Lo que no era problema.** Enlace, carga de datos, importaciones, CRT propio, montón, ganchos sobre INT3 (los `JMP` quedan donde deben) y la arquitectura de «datos del original + código propio» funcionan sin cambios en el código del juego.
* **Datos.** La dependencia del original es de datos, no de ejecución: mientras `DatosExe` no defina los 4.287 objetos (y los 1.673 inicializadores), el EXE propio **necesita el `gta_sa.exe` del usuario como archivo** de 641 KB de imagen inicial.
* **No medido.** Cuántas rupturas más hay entre el primer marco y el menú (cada marco nuevo trae más RW; IND1A dio 148 funciones originales hasta el menú el 19/11, sin las rutas de arranque propias de aquí). Con 5 rupturas en ~2 s se puede decir que el menú está a más de una decena de funciones de distancia en RpWorld y D3D9, pero no cuántas; el censo con retorno simulado no sirve más allá del primer marco porque `0x7F98D0` y similares devuelven estado y se llaman en bucle.

## 5. (d) Plan de siguientes pasos (ordenado)

1. **Cerrar RpWorld/camara y D3D9 estándar, que bloquean el primer marco:** integrar `vivo-d3dstd` (`0x4CD820` y los 12 manejadores estándar) y añadir los tres callbacks de la cámara `0x750430/0x750460/0x750480` (el último, 650 B de BSP+frustum x87), `0x750AC0` (513 B), `0x7F98D0` (223 B, `StretchRect`) y `0x7F99B0` (621 B, `Reset`). Los dos pequeños del §3 pueden pasar tal cual de shim a gancho con catálogo.
2. **Reclasificar los «muertos» de `huecos-p6.csv`**: ninguna dirección citada por el C++ como valor en tabla o estructura puede ser «muerta». Son 147 huecos `citada-en-cpp`/`muerto` (19.342 B; 136 del juego, 10 de RenderWare); ya han salido 3 en dos segundos.
3. **Tabla C de `__cinit` completa en `StandaloneStartup`:** pasar `ExtraTablaC` a `ExeCrt` (`0x8289F5`, `0x826A72`) y reconstruir `0x833494` (`_setmbcp`, agente de locale). Tocar `CrtStartup.cpp` lo evité por si `vivo-crtf8` lo está modificando.
4. **Constructores y asignaciones de `_initterm` (175 filas)** como globales C++ de `DatosExe`: hasta entonces ese estado queda a cero en el EXE propio.
5. **Datos:** sustituir la carga de los 641 KB del original por las definiciones de `DatosExe` (empezando por `.rdata`/vtables, 475 vtables sin hueco de tamaño) y volver a medir con este mismo EXE: debe seguir sin una sola instrucción original y sin ruptura nueva. Entonces `--ruta-original` deja de hacer falta.
6. **Repetir este experimento tras cada ola** (30 s de enlace + 5 s de ejecución): el primer INT3 es la lista de pendientes real.
7. Fuera de este experimento: D3DX9 (`d3dx9.lib` y activar los 12 envoltorios), libpng/zlib/libjpeg ya enlazadas, QuickTime (retirar), CLEO/mods (solo plan).

## 6. Cómo reproducir

```
cp <build ASI Debug>\*.obj reversing\FnFase8\obj\            # una vez (o pasar la carpeta como 3.er argumento de enlazar.cmd)
python -I reversing/FnFase8/gen_cl_rsp.py <gta_reversed.vcxproj> <raiz_origen> <raiz_destino> reversing/FnFase8/cl.rsp   # una vez
bash /d/Original/_build_FASE8.sh                             # compila StandaloneExe.cpp y enlaza (30 s); log termina en EXIT=0
pwsh -File reversing/FnFase8/crear_carpeta_fase8.ps1         # carpeta de pruebas E:\JUEGOS\GTA San Andreas - CVehicle Test - FASE8 (junctions; -Quitar la borra)
python -I reversing/FnFase8/gen_tabla_ret.py "<gta_sa.exe>"  # solo para --censo
bash reversing/FnFase8/ejecutar.sh <etiqueta> <limite_s> [--shims] [--censo --tabla-ret <retn.bin> --stubs <stubs.txt>]
python -I reversing/FnFase8/resolver.py reversing/FnFase8/salida/gta_sa_standalone.map <dirección propia>...   # función propia que llamó
python -I reversing/FnFase8/dis.py "<gta_sa.exe>" 0x750430 100                                                  # desensamblado de una dirección original
```

`ejecutar.sh` toma `D:\Original\gta_lock` y espera a que no haya `gta_sa.exe` ni otros ejecutables de prueba; solo mata su propio proceso. Resultado de cada intento en `reversing/FnFase8/ejecuciones/<etiqueta>-<hora>/` (`fase8_arranque.log`, `fase8_fallo.txt`, `fase8_codigo_original.csv`, `log.log` del proyecto).

Intentos conservados: `t1` (ruptura 1), `t2` (ruptura 2, `main` limpio), `t3` (superposición de `vivo-d3dstd`: ruptura 3), `t4`/`t5` (con `--shims`: ruptura 4 y estado del juego), `c1`–`c3` (censo). Las superposiciones (`reversing/FnFase8/over_src`, ignorada por git) son copias de trabajo de otra rama y **no** forman parte de este commit.

## 7. Segunda ronda (21/11/2026, rama `fase8-exp2`, base `origin/main` 59b1028202)

Mismo método (datos del `gta_sa.exe` original solo como archivo, `.text` relleno de INT3, modo estricto), con el código de `main` actual: arranque del CRT propio completo, 26 rutinas de cámara/D3D9/RpWorld, manejadores `rwSTANDARD`, datos DE04/DE05. Los `.obj` son los de `wv-int` (mismo commit); `_build_FASE8B.sh` enlaza en 25 s (`reversing/FnFase8/salida`), carpeta de pruebas propia `D:\Original\_fase8b_run` (junctions de solo lectura a los datos de `D:\Grand Theft Auto San Andreas`; fuente de datos `gta-sa.exe`). Registros de cada intento en `reversing/FnFase8/ejecuciones/` (`r1`-`r9`, `g1`, `c4`).

### 7.1 Qué funciona (medido)

* Enlace sin símbolos sin resolver; carga de datos (7 secciones, IAT 214/214); CRT propio; 13.589 ganchos instalados; `NOTSA_WinMain`; dispositivo D3D9 y `RwCamera` creados; ventana viva.
* **Menú principal**: con `FastLoader` (sin logo ni títulos) el juego llega a `gGameState = 7` (FRONTEND_IDLE) y **se queda dibujando el menú a 30 fps sin una sola instrucción original ejecutada** (`codigo-original-distinto=0 hits=0`). Contadores del vtable del dispositivo (envolturas propias en `StandaloneExe.cpp`): en 30 s, 787 `Present`, 787 `EndScene`, 1.574 `Clear` y 7.083 llamadas de dibujo (9 por marco). Captura de pantalla de la ventana: `ejecuciones/r9-captura.png` (título «Menú principal», texto de `SPANISH.GXT`, fondo del cartel de Vinewood). `CTimer::m_FrameCounter` sube de 13 a 859.
* **Nueva partida**: arranca la carga (`gGameState = 8`, abre CARREC.IMG, SCRIPT.IMG, CUTSCENE.IMG, PLAYER.IMG; 52 `Present` por 2 s durante la carga) y se rompe a los 7 s en el motor de scripts (ruptura 5).

### 7.2 Qué se rompe, en orden (cada fila es una ejecución real)

| # | Intento | Dirección / lugar | Quién la llama | Qué es | Estado |
| ---: | --- | --- | --- | --- | --- |
| 0 | `r1` (sin FastLoader) | **ningún INT3.** A los 23 s, `0xC0000005` en `DDRAW` dentro de un hilo de `quartz` | reproductor DirectShow de `Logo.mpg` | el vídeo del logo (dos esperas de 10 s en la pantalla `loadscs`, luego fallo de DirectShow/DDraw) | **Sin resolver; no es código original.** Se evita con `NoLogo`/`NoTitleOrIntro`. Pendiente: reproductor de vídeo en este EXE |
| 1 | `r2` | `0x7FBD30` (1.202 B) | `CSprite2d::RenderVertexBuffer` | `RwIm2DRenderIndexedPrimitive` del controlador D3D9 (puntero de la tabla estándar del dispositivo; hueco de Ghidra) | **Arreglado** (`_rwD3D9Im2DRenderIndexedPrimitive`, `rwcore.cpp`, con gancho): desindexa a un búfer dinámico o usa el búfer de índices. Sin validar contra el original: solo desensamblado completo y humo |
| 2 | `r3`-`r5` | acceso a 0 en `CAESound::UpdateParameters` (`+0xA8`) | `CAESoundManager::Service` | la entidad de audio estática (`AudioEngine`, `0xB6BD44`) tiene la vtable a cero: el EXE propio **no ejecuta la tabla `_initterm`** (140 constructores con `atexit`, 32 asignaciones, 5 llamadas «pendiente-DatosExe») | **Puenteado** en `StandaloneExe.cpp` (7.3) hasta que `DatosExe` defina esos objetos |
| 3 | `r6`, `r7` | `0x441E00` (13 B) y `0x563190` (constructores sin gancho llamados desde el puente) | puente de inicializadores | constructores triviales de objetos globales | Resuelto: los constructores sin gancho se **emulan fuera de línea** (unicorn) y entran como escrituras de datos |
| 4 | `r8`, `r9` | - | - | **menú principal estable** (7.1) | - |
| 5 | `g1`, `c4` | `0x489500` (unos 3,5 KB + tabla de salto) | `RunningScript` (`std::invoke` de un despachador) | `CRunningScript::ProcessCommands1000To1099` (opcodes 1000-1099) | **Cola** (grande, mucho más de 300 B). Solo 8 despachadores de 100 opcodes están reconstruidos (300, 400, 800, 1200, 1500, 1700, 1900, 2000); los demás romperán uno a uno al avanzar el script de arranque |

### 7.3 Herramientas nuevas

* `StandaloneExe.cpp`: volcado del hilo principal (EIP, registros, cadena de EBP, pila) antes de que la guardia mate el proceso; contadores D3D9 (Present/EndScene/Clear/Draw) y `gGameState` en el latido; opción `--sin-init`; puente `EjecutarInicializadoresEstaticos()` que lee `StandaloneInit.inc`.
* `reversing/FnFase8/gen_init_estaticos.py` genera `StandaloneInit.inc` (4.299 filas) desde `fase8-inicializadores.csv` y el archivo original: constructores con gancho (se llama al C++), rellenos `0xFFFF`, copias de valores iniciales y las escrituras de los constructores **sin** gancho obtenidas por emulación. Se omiten `atexit`, puestas a cero de `.bss` y las 4 conversiones `fild`/`_ftol` que dependen de datos de ejecución. Es un **puente temporal** del experimento: cuando `DatosExe` defina los objetos y sus inicializadores hay que retirarlo (no se tocó `DatosExe`).
* `compilar_over.cmd` y `_cc_FASE8B.sh` (recompilan un `.cpp` con las opciones del ASI y reenlazan, unos 35 s), `ejecutar_f8b.ps1`/`correr_f8b.ps1` (esperan el candado, limpian `logs`, captura de pantalla con `captura_f8b.ps1`), `dis_rango.py`.

### 7.4 Censo del código original alcanzable (cierre estático, recalculado con `FnIND0A`)

| | Primer experimento | Segunda ronda |
| --- | ---: | ---: |
| Funciones del cierre | 691 | **649** |
| Bytes | 309.142 | **297.819** |
| RenderWare | 459 / 181.547 B | **444 / 177.391 B** |
| CRT | 138 / 102.946 B | **136 / 102.869 B** |
| Juego | 94 / 24.649 B | **69 / 17.559 B** |
| Llamadas indirectas sin resolver / funciones de la IAT | 668 / 39 | 597 / 39 |
| Direcciones de código citadas sin gancho (A / B / ruido C) | 379 (119 / 147 / 113) | 338 (107 / 121 / 110) |
| Referencias a direcciones fijas / destinos distintos | 66.277 / 18.087 | 50.196 / 17.199 |

Datos: `fase8-experimento/resumen-ronda2.json`, `cierre_codigo-ronda2.csv`, `dependencias_codigo-ronda2.csv`. Los `.obj` usados no incluyen `_rwD3D9Im2DRenderIndexedPrimitive` (1.202 B más de código propio). El cierre estático **no** ve el despachador `0x489500` (se llama por tabla de punteros de script) ni los demás `ProcessCommands` sin reconstruir: el censo dinámico es la fuente fiable de lo que falta. Dinámico: **0 funciones originales ejecutadas hasta el menú**; 1 al cargar la partida nueva (ruptura 5). El modo `--censo` no sirve para avanzar: al simular el retorno de `0x489500` (EAX=0) la pila se corrompe (`0xC0000409`).

### 7.5 Siguiente paso (ordenado)

1. **Script**: reconstruir los despachadores `ProcessCommands` que faltan (3-5 KB cada uno, unos 100 opcodes; los comandos individuales ya son en gran parte C++), empezando por `1000To1099` (`0x489500`), y repetir `g1` para ver el siguiente.
2. **`DatosExe`**: definir los 140 constructores estáticos, 32 asignaciones y 5 llamadas pendientes y retirar `StandaloneInit.inc`.
3. **Vídeo del logo/intro** (DirectShow + DDraw): fallo en un hilo de `quartz`; o se reconstruye el reproductor o se mantiene `NoLogo`.
4. Validar `_rwD3D9Im2DRenderIndexedPrimitive` con un banco (no hecho; el menú dibuja con él).
5. Tras cada ola repetir `r9` (menú, 30 s) y `g1` (partida nueva): el primer INT3 sigue siendo la lista de pendientes real.

### 7.6 Cómo reproducir la segunda ronda

```
bash /d/Original/_build_FASE8B.sh                                        # enlaza con los .obj de wv-int
bash /d/Original/_cc_FASE8B.sh source/game_sa/RenderWare/rw/rwcore.cpp   # recompila un .cpp a reversing/FnFase8/over y reenlaza
pwsh -File reversing/FnFase8/correr_f8b.ps1 -Et r9 -Lim 30 -Cap 15       # menu: ini con FastLoader (NoLogo, NoTitleOrIntro, SaveGameToLoad=-2)
pwsh -File reversing/FnFase8/correr_f8b.ps1 -Et g1 -Lim 60 -Cap 45       # partida nueva: SaveGameToLoad=0
python -I reversing/FnFase8/gen_init_estaticos.py                         # regenera StandaloneInit.inc (unicorn y capstone)
```

## 8. Tercera ronda (22/11/2026, rama `fase8-exp3`, base `origin/main` 72437c24e9)

Mismo método (datos del `gta_sa.exe` original solo como archivo, `.text` relleno de INT3, modo estricto). Los `.obj` son los de `wv-int` (mismo commit) con `over\` para lo recompilado en esta rama (`rwcore`, `RunningScript`, `RpAnimBlend`). `_build_FASE8C.sh` enlaza en 30 s, `_cc_FASE8C.sh <archivo.cpp>` recompila uno y reenlaza, `_run_f8c.sh <etiqueta> <límite> <captura_s> <líneas> [--shims]` ejecuta (carpeta propia `D:\Original\_fase8b_run`, candado `gta_lock`). Registros de cada intento en `reversing/FnFase8/ejecuciones/` (`h1`, `h2`, `h4`, `h6`, `h11`, `h13`, `h16`; capturas `h4-captura.png` y `h16-captura.png`).

### 8.1 DatosExe y el puente `StandaloneInit.inc`

> **Actualización 22/11/2026 (rama `vivo-init`):** el puente y `gen_init_estaticos.py` ya no existen; las 56 rutinas pendientes son C++ en `DatosExe::RunStaticInitializers`. Ver [ejecucion-init-estatica.md](ejecucion-init-estatica.md). Lo que sigue es el histórico.

* `DatosExe::RunStaticInitializers()` (93 de los 175 inicializadores no-CRT) **se llama desde `StandaloneExe.cpp`, justo antes del puente y de `NOTSA_WinMain`, no desde `ExeCrt::Cinit`** como proponía `ejecucion-datos-b.md`. Motivo medido (`h1`): en `Cinit` aún no han corrido los inicializadores dinámicos del C++ propio y las referencias `static inline auto& X = StaticRef<...>(dir)` (por ejemplo `CStats::StatTypesFloat`) son nulas: `CPlayerPedData::SetInitialState` -> `CStats::GetFatAndMuscleModifier` leyó `0xA0` a los 172 ms. `CrtStartup.cpp` queda sin cambios (es del agente del CRT).
* `reversing/FnFase8/gen_init_estaticos.py` ya no tiene rutas absolutas y **solo puentea las filas con estado `pendiente` en `docs/DatosExe/inicializadores-estado.csv`**: `StandaloneInit.inc` pasa de 4.299 a **4.216 filas** (55 rutinas pendientes: constructores y constructores de elemento del EXE sin gancho, 3 destructores no de clase y 1 `fild/fdivr` x87, `0x84CFC0`, que no se puentea). Casi todas las filas salen de dos rutinas: `0x8545E0` (3.584 escrituras emuladas) y `0x8545B0` (512), constructores de elemento `0x6FA790` y `0x6FA6D0` sin gancho. `--sin-init` apaga las dos cosas. Para retirar el `.inc` del todo hay que reconstruir esos constructores (lista con el motivo por fila en `inicializadores-estado.csv`: `0x441E00`, `0x4470E0`, `0x463750`, `0x463770`, `0x4AC0D0`, los nueve `*ModelInfoStoreConstructor` `0x4C5CB0..0x4C67D0`, `0x4F1750`, `0x4E3330`, `0x4F63B0`, `0x50E6D0`, `0x5173F0`, `0x541D80`, `0x59AED0`, `0x564040`, `0x563190`, `0x43E3F0`, `0x463360`, `0x5A7570`, `0x532290`, `0x5DB2B0`, `0x5DB290`, `0x5DE520`, `0x5F6450..0x5F6540`, `0x632BD0`, `0x694850`, `0x6FA6D0`, `0x6FA790`, `0x71A8B0`, `0x720F60`, `0x728B10`, `0x72A620`, `0x72E860`, `0x5074B0`, `0x742A90`, `0x743C30`).

### 8.2 Qué se rompe, en orden (cada fila es una ejecución real, partida nueva)

| # | Intento | Dirección / lugar | Causa | Estado |
| ---: | --- | --- | --- | --- |
| 1 | `h1` | acceso a `0xA0` en `CStats::GetFatAndMuscleModifier` a los 172 ms | `RunStaticInitializers` llamada desde `Cinit` (referencias `StaticRef` aún nulas) | **Arreglado** (se llama desde `StandaloneExe.cpp`) |
| 2 | `h2` | `0x7FB650` (432 B) desde `CShadowCamera::MakeGradientRaster` (a los 12 s) | `RwIm2DRenderLine` del controlador D3D9 (hueco de Ghidra; puntero de la tabla estándar) | **Arreglado**: `_rwD3D9Im2DRenderLine` (`rwcore.cpp`, con gancho; desensamblado completo, sin banco) |
| 3 | `h3` | `0x4D2E10` (33 B) desde `CAnimBlendClumpData::ForAllFrames` | callback de frames fuera de pantalla. Las cinco rutinas de callback (`0x4D2B90`, `0x4D2E10`, `0x4D2E40`, `0x4D30A0`, `0x4D32D0`) estaban reconstruidas pero **con el gancho desactivado** (`DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS`): con la DLL se ejecutaba el original | **Arreglado**: se activan solo si el `.text` original no existe (`ExeCodeAbsent`: primer byte `0xCC`); con la DLL no cambia nada |
| 4 | `h4` | **`Unknown param type: 64`** (ventana modal, el juego se cuelga) en el script `main` | ver 8.3 | **Arreglado** (`c::path::RegisterHandlers()` no se llamaba) |
| 5 | `h11` | `0x80E970` (18 B) desde `_RxPipelineExecute` | nodeBody del nodo Rx `ImmInstance.csl` (definición `0x8E2918`) | **Arreglado**: `_rxD3D9ImmInstanceNodeBody` (guarda `params[0]` en `0xC9AB30`) |
| 6 | `h13` (sin shim) | `0x80E270` (unos 1.380 B) desde `_RxPipelineExecute` | nodeBody de `nodeD3D9SubmitNoLight.csl` (definición `0x8E2978`): dibujo de Im3D indexado sin luz (x87 y estados D3D) | **Cola** (grande). **Shim** del experimento (`--shims`): devuelve 1 y no dibuja |
| 7 | `h13` | `0x7C7B90` (varios KB; llama a `0x807DE0` y `0x7C8D60`) desde `_RxPipelineExecute` | nodeBody de `nodeD3D9SkinAtomicAllInOne.csl` (definición `0x8DED08`): instancia y dibujo de los atomics con skin (CJ y los peatones) | **Cola** (grande). **Shim**: devuelve 1; los atomics con skin no se dibujan |
| 8 | `h16` | **ninguna** | con los dos shims el juego corre 80 s sin ruptura ni excepción | ver 8.4 |

De las cinco definiciones de nodos Rx con `.csl` del EXE (`0x8D6338` AtomicAllInOne, `0x8D63B8` WorldSectorAllInOne, `0x8DED08` Skin, `0x8E2918` ImmInstance, `0x8E2978` SubmitNoLight) tienen gancho las dos primeras y ImmInstance; **Skin y SubmitNoLight quedan pendientes**. Lo que dice sobre el plan: otra vez el primer INT3 no figura en ningún CSV del plan; son huecos de Ghidra alcanzables por punteros de tablas (aquí, de `RxNodeDefinition`).

### 8.3 `Unknown param type: 64`: causa y arreglo

El popup `Unreachable code reached! ReadArg.hpp notsa::script::Read:215` era del propio código del juego, no del EXE independiente. Se añadió al EXE propio un volcado (`VolcarScripts` en `StandaloneExe.cpp`, con dos anillos de depuración en `ProcessOneCommand`: `g_DbgCmdTrace` y `g_DbgMainTrace`) con los últimos comandos y el IP de cada script. En `h6`: el script `main` ejecutó el comando n.º 3069 en `E259` (**`0x01E8` `SWITCH_ROADS_OFF`**) y el siguiente se leyó en `E25B` (solo avanzó los 2 bytes del opcode, sin sus 6 flotantes): al reanudarse leyó a mitad de un flotante (`0x40`).

Causa: `ProcessOneCommand` busca primero un manejador propio en C++ (`CustomCommandHandlerOf`); si no lo hay, llama al despachador por centenas. `SWITCH_ROADS_ON/OFF` (`0x01E7/0x01E8`) tienen manejador en `Commands/PathCommands.cpp`, pero **`c::path::RegisterHandlers()` no se llamaba nunca** desde `InjectCustomCommandHooks`, y el despachador reconstruido `ProcessCommands400To499` (`0x47D210`) no tiene caso para 487/488 (el EXE sí: tabla de bytes `0x47DFE0`, destino `0x47DC4B`, `CollectParameters(6)`), así que cae en `default -> OR_INTERRUPT` sin leer parámetros. Se registra `path`. Comprobación estática: de los 14 comandos de `400..499` que el EXE atiende y el C++ no tiene como caso, 12 tienen manejador registrado en otros archivos y los otros 2 eran estos; de los espacios de nombres declarados en `Commands.hpp` solo faltaba `draw`, que no se puede registrar sin más (duplica `SET_TEXT_BACKGROUND_COLOUR`, también en `TextCommands.cpp`; sus comandos ya están como casos numéricos en los despachadores). Ningún otro comando de `0..2698` queda sin mención en C++.

**Modo DLL:** no se reprodujo con el ASI (no se tomó el candado del juego compartido); el defecto es de `main`, no del EXE independiente: con el ASI `ProcessCommands400To499` también está enganchado y también devuelve `OR_INTERRUPT` para `0x01E7/0x01E8`. Pendiente confirmarlo con una partida nueva del ASI.

### 8.4 Estado alcanzado (con los dos shims) y comprobaciones

* **El juego llega a CJ en el mundo y corre**: `gGameState = 9` (jugando), `CTimer::m_FrameCounter` sube sin parar (1.353 fotogramas a los 58 s), 251.403 llamadas de dibujo, **0 funciones originales ejecutadas** (`codigo-original-distinto=0`, `hits=0`, `excepciones=0`), sin guardia ni popup en 80 s (`h16`).
* **Prueba por memoria** (`LogMundo`, cada 4 s): jugador `0x09308198`, posición **(2488, -1666, 13)** (Grove Street), área 14 -> 0 (secuencia de apertura), tiempo de juego +74 s, pool de peds 1 -> 5 -> 1 de 140 y **vehículos 0 de 110**.
* **Captura de pantalla** (`h16-captura.png`, revisada con la herramienta de imagen; `h4-captura.png` es anterior al arreglo): se ve Grove Street, una patrulla en primer plano, palmeras y casas. **No es una verificación limpia**: el cielo sale blanco, el taxi del fondo tiene polígonos negros/blancos y los atomics con skin no se dibujan (shim): no se ve a CJ ni a los peatones. En otra captura intermedia la cámara estaba sobre un taxi mirando hacia abajo. Por tanto «CJ en el mundo» está confirmado por memoria (posición, estado, tiempo) y solo a medias por pantalla.
* **Sin resolver, con causa probable**: (a) cielo blanco: el log trae **184 avisos «Línea de timecyc incorrecta»** (`TimeCycle.cpp:183`, `sscanf` devuelve menos de 52 campos; el código lo atribuye a una línea 320 de R*, pero son 184: probable diferencia del `sscanf` del CRT propio, a medir frente al ASI); (b) el jugador no se mueve y no hay vehículos: coherente con que el script de apertura no avanza o con los atomics sin dibujar; aún sin pruebas de entrada.
* **Cola por tamaño** (lo que hay que reconstruir para quitar los shims): `0x80E270` (unos 1.380 B) y `0x7C7B90` más sus ayudantes (varios KB).

### 8.5 Cifras (cierre estático recalculado con `FnIND0A`, `.obj` de `wv-int` + `over\`)

| | Primer experimento | Segunda ronda | Tercera ronda |
| --- | ---: | ---: | ---: |
| Funciones del cierre | 691 | 649 | **652** |
| Bytes | 309.142 | 297.819 | **299.480** |
| RenderWare | 459 / 181.547 B | 444 / 177.391 B | **447 / 179.052 B** |
| CRT | 138 / 102.946 B | 136 / 102.869 B | **136 / 102.869 B** |
| Juego | 94 / 24.649 B | 69 / 17.559 B | **69 / 17.559 B** |
| Llamadas indirectas sin resolver | 668 | 597 | 612 |
| Direcciones de código citadas sin gancho (A / B / ruido C) | 379 (119 / 147 / 113) | 338 (107 / 121 / 110) | **340 (107 / 124 / 109)** |
| Referencias a direcciones fijas / destinos distintos | 66.277 / 18.087 | 50.196 / 17.199 | **49.736 / 17.010** |

El cierre **sube 3 funciones** (+1.661 B) respecto a la segunda ronda: `docs/hooks.csv` de esta rama no incluye los ganchos nuevos (`_rwD3D9Im2DRenderLine`, `_rxD3D9ImmInstanceNodeBody`; los cinco callbacks de animación entran con el gancho dependiente del `.text`) y los `.obj` de `main` incorporan despachadores que llaman a más RenderWare. El cierre estático no ve los nodos Rx que se llaman por tabla (`0x80E270`, `0x7C7B90`): el censo dinámico es la fuente fiable (**dinámico**: 0 funciones originales hasta el menú; 1 en la carga; en juego 2 con el estado actual de `main`, los dos nodos, y 0 con los shims). Datos: `fase8-experimento/resumen-ronda3.json`.

### 8.6 Vídeo del logo (`Logo.mpg`)

Sin cambios: el reproductor DirectShow cae en un hilo de `quartz`/DDRAW; se evita con `NoLogo`/`NoTitleOrIntro` en `gta-reversed.ini`. No se probó de nuevo esta ronda.

### 8.7 Lo que falló por datos

* **Orden de inicialización**: ver 8.1. Las variables `static inline auto& X = StaticRef<...>(dir)` de las clases son globales C++ con inicialización dinámica: nada que dependa de ellas puede ejecutarse antes de que el CRT propio corra sus inicializadores (por eso `Cinit` no vale para `RunStaticInitializers`).
* **Copias desincronizadas**: `CTheScripts::MainScriptSize` y `LargestMissionScriptSize` (`DatosExe::Bss::bA444C4` y `bA444C0`, objetos propios del exe, no residen en `0xA444C4`/`0xA444C0`) valen 0 si se leen por la dirección fija del EXE (el volcado de scripts mostró `MainScriptSize=0`). No falla mientras solo C++ los use; cualquier código original o sonda que lea esas direcciones verá cero.
* Ninguna variable sin definir en `DatosExe` produjo ruptura en esta ronda.

### 8.8 Siguiente paso (ordenado)

1. Reconstruir `nodeD3D9SkinAtomicAllInOne.csl` (`0x7C7B90` y ayudantes `0x7C8D60`, `0x807DE0`...) y `nodeD3D9SubmitNoLight.csl` (`0x80E270`); quitar `--shims` y volver a mirar CJ y los peatones.
2. Reconstruir los constructores de elemento restantes y retirar `StandaloneInit.inc`.
3. Medir el cielo blanco / timecyc frente al ASI (`sscanf` del CRT propio) y por qué el jugador no se mueve en la apertura.
4. Confirmar con el ASI el defecto de `SWITCH_ROADS_*` (partida nueva sin FastLoader).
5. Repetir `h16` tras cada ola (80 s) y la captura.

### 8.9 Cómo reproducir la tercera ronda

```
bash /d/Original/_build_FASE8C.sh                                          # enlaza con los .obj de wv-int
bash /d/Original/_cc_FASE8C.sh source/game_sa/RenderWare/rw/rwcore.cpp     # recompila un .cpp a over\ y reenlaza
bash /d/Original/_run_f8c.sh h16 80 70 4 "--shims"                         # partida nueva con los dos shims; captura a los 70 s
bash /d/Original/_run_f8c.sh h13 60 45 4                                   # sin shims: primer INT3 = 0x80E270 / 0x7C7B90
python -I reversing/FnFase8/gen_init_estaticos.py                          # regenera StandaloneInit.inc (solo las filas pendientes de DatosExe)
```
