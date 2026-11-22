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
