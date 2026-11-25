<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnIND1A: probar el cargador del exe propio (19/11/2026)

Primer hito real hacia el EXE independiente: **`gta_reversed_exe.exe` (estrategia «caparazón») carga la imagen original, instala los hooks y llega al menú principal del juego, estable más de 60 s.** Informe corto en [docs/ExeIndependiente/README.md](../../docs/ExeIndependiente/README.md) (sección «Estado del cargador»); antecedentes en [FnIND0A](../FnIND0A/README.md).

## Resultado en una tabla

| Modo | Comando (resumen) | Resultado |
|---|---|---|
| Solo cargar | `--ruta-original "<gta_sa.exe>"` | Termina limpio (código 0, ~0 ms). 7 secciones copiadas a sus direcciones; hash de cada sección en memoria = archivo (salvo la IAT, 800 bytes); 214 imports resueltos, 0 sin resolver, 9 DLL cargadas. Comprobación automática: [`verificar_carga.py`](verificar_carga.py) → [`verificacion_carga.json`](verificacion_carga.json) (`todo_ok: true`). |
| `--ejecutar --sin-hooks` | el original corre solo sobre la cáscara (sin CRT propio ni hooks) | Llega al menú; el contador de fotogramas avanza (~60/s) desde ~23 s, 40-75 s sin caer, sin excepciones. |
| `--ejecutar` (con hooks) | DllMain propio (12.445 hooks, ~0,5 s) y WinMain original ya enganchado | Llega al menú (fotogramas desde ~110-140 s; el arranque con la DLL Debug es mucho más lento), **170-200 s sin caer, 0 excepciones**, ~145 fotogramas/s. Los hooks se aplican **una sola vez** (no hay `gta_reversed.asi`). |
| `--ejecutar --contador-int3` | cuenta cada primera entrada a una función original | Sin hooks: 2.832 funciones originales hasta el menú. **Con hooks: 148.** |
| `--ejecutar --contador-paginas` | `.text` original no ejecutable, una entrada por página | Con hooks: 79 páginas del `.text` original tocadas hasta el menú (33 de ellas solo por un enganche). Sirve de contraste de completitud (ver abajo). |
| `-FastLoader` (INI) | arranque directo a la partida 2 | **Falla**: excepción no controlada (lectura de 0) en el código original `0x004CA5B5` a ~58 s, ya cargada la partida; no es del cargador, ver «Pendiente». |

Todas las ejecuciones con el juego (en [`ejecuciones/`](ejecuciones/)) se hicieron en `E:\JUEGOS\GTA San Andreas - CVehicle Test - IND1A`, bajo `D:\Original\gta_lock`, con límite de tiempo, desde `gta_reversed_exe.exe` copiado ahí.

## Cómo reproducir

```
bash reversing/FnIND1A/construir.sh                          # re-enlaza gta_reversed_exe.exe (25 s; usa D:\Original\compile_lock; los .obj ya compilados de build\source\gta_reversed.dir\Debug)
python -I reversing/FnIND1A/verificar_carga.py --exe reversing/FnIND1A/salida/gta_reversed_exe.exe --original "E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"      # solo-cargar + hashes (no hace falta el juego)
pwsh -File reversing/FnIND1A/crear_carpeta_ind1a.ps1         # crea la carpeta de pruebas (junctions + copias pequeñas); -FastLoader para FastLoader; -Quitar para borrarla
bash reversing/FnIND1A/ejecutar.sh <etiqueta> <limite_s> [args del exe]   # ejecuta bajo gta_lock con limite; deja todo en ejecuciones/<etiqueta>-<hora>/
pwsh -File reversing/FnIND1A/probar_a_mano.ps1               # PRUEBA A MANO sin limite (ver guion abajo)
python -I reversing/FnIND1A/gen_lista_funciones.py "<gta_sa.exe>"        # regenera datos/funciones_originales.bin (lista de entradas de función)
python -I reversing/FnIND1A/resumen_contador.py <csv> <.map> <salida.md> # tabla con la función propia que llama
```

Comando exacto de cada modo (el exe se ejecuta con el directorio actual donde se quiera el informe, o con `--salida`):

```
gta_reversed_exe.exe --ruta-original "E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"                       (solo cargar; escribe ind1a_informe.txt)
gta_reversed_exe.exe --ruta-original "<gta_sa.exe>" --volcar                                                      (además ind1a_volcado.bin, usado por verificar_carga.py)
gta_reversed_exe.exe --ruta-original "<gta_sa.exe>" --directorio-juego "<carpeta del juego>" --salida "<dir informes>" --ejecutar --limite 90
        [--sin-hooks] [--contador-int3 --lista-funciones "<...\datos\funciones_originales.bin>"] [--contador-paginas]
```

Opciones: `--limite <s>` (por defecto 90; **0 = sin límite ni guardia de latido**, para jugar a mano), `--directorio-juego` (carpeta de trabajo y de las DLL; por defecto la del `--ruta-original`), `--salida` (informes), `--sin-ejecutar-texto`. Salidas: `ind1a_informe.txt` (solo-cargar), `ind1a_arranque.log` (ejecutar: pasos con milisegundos, latido cada 2 s con fotogramas y ventanas del proceso), `ind1a_fallo.txt` (si algo cae sin que lo atrape el propio juego), `ind1a_funciones.csv` (contador).

## Diseño final del cargador (`FnIND0A/prototipo/cargador_imagen.cpp`)

El diseño de IND0A (cargar en `WinMain` y saltar a `0x824570`) **no podía arrancar** (ver iteraciones). Orden final:

1. **Callback TLS `.CRT$XLB` del exe** (antes del punto de entrada y de cualquier inicializador): sin CRT (solo API de Windows, `/GS- /Od`), lee los argumentos, abre el registro, **anula la ranura del callback TLS dinámico del CRT propio** (`__dyn_tls_init`, ranura `.CRT$XLC`), lee el `gta_sa.exe` del usuario, comprueba tamaño/PE, copia las 7 secciones a sus direcciones fijas, resuelve los 214 imports, compara memoria y archivo por sección y (con `--ejecutar`) cambia la llamada del arranque original a WinMain. En modo solo-cargar termina aquí con `TerminateProcess` (nunca arranca el CRT propio).
2. **Entrada del exe `IndEntrada`** → CRT **original** `0x824570` (`_heap_init`, `_mtinit`, `_ioinit`, `_cinit`…, como cuando era un ASI cargado después de que el juego inicia su montón).
3. En `0x8246EC` el cargador cambió `call 0x748710` (WinMain) por `call IndWinMainOriginal`: **restaura el callback TLS dinámico, inicia el CRT propio** (`WinMainCRTStartup`: inicializadores estáticos del código propio, que ya pueden usar el `operator new` del juego) → **WinMain propio**: DllMain del proyecto (instala los hooks) → `AplicarProtecciones` (R/RX/RW por sección) → **WinMain original** `0x748710` (enganchado).
4. Diagnóstico: VEH propio (registra excepciones de primera oportunidad con EIP, módulo, registros, pila y últimas entradas; vuelve a ponerse el primero tras `DllMain`, porque el manejador del proyecto registraba cada INT3 con una traza completa de ~100 ms), `SetUnhandledExceptionFilter` (informe + muerte; el CRT original lo sustituye más tarde, por eso el VEH es lo fiable), hilo **guardia** (límite de tiempo, latido = `CTimer::m_FrameCounter` 0xB7CB4C, vuelca el CSV del contador, enumera las ventanas del proceso, avisa si el cerrojo del contador queda retenido) y registro de arranque en archivo.

Decisiones documentadas:

* **INI**: `gta-reversed.ini` propio en la carpeta IND1A con **FastLoader desactivado** (arranque normal por menú, hito del lote), `SkipDeviceSelection` y `SaveDirectoryPath` en la propia carpeta (`userfiles` copiado, no compartido). Con `-FastLoader` se genera el INI de la copia de pruebas (partida 2).
* **Sin doble inyección**: la carpeta IND1A **no tiene `dinput8.dll` ni `scripts\`** (el cargador de ASI de la copia de pruebas habría cargado `gta_reversed.asi` y duplicado los hooks) ni `gta_sa.exe`. Los directorios de datos (`anim audio data models movies text`) son *junctions* a la copia de pruebas; `eax.dll`, `ogg.dll`, `vorbis*.dll`, `stream.ini` y `userfiles` se copian. **La copia de pruebas no se modifica** (por eso no hubo copias de seguridad que restaurar); `E:\JUEGOS\GTA San Andreas` no se tocó.
* El exe propio importa `dinput8.dll` por su cuenta: al no haber un `dinput8.dll` junto al exe, carga el del sistema.

## Iteraciones (fallo → causa → arreglo)

| # | Fallo | Causa | Arreglo |
|---|---|---|---|
| 0 | `gta_reversed_exe.exe` del IND0A: `Segmentation fault` (código 0xC0000005) sin ninguna salida | No se sabía dónde: ni informe ni VEH. | Reescritura con VEH, registro en archivo y entrada propia (`IndEntrada`); el volcado de la cadena EBP apuntó a `__dyn_tls_init` → `CGeneral::randomEngine` (`thread_local` con `std::random_device`, `AnimManager.obj`) → `rand_s` → `__acrt_lock` con los cerrojos del CRT sin iniciar. |
| 1 | Revienta también al salir por una ruta de error | `ExitProcess` antes de iniciar el CRT propio ejecuta `DLL_PROCESS_DETACH`/callbacks TLS del CRT estático | `Salir()` = `TerminateProcess` mientras el CRT propio no corre. |
| 2 | `ERROR: no se pudo abrir E:\JUEGOS\GTA` | `Start-Process -ArgumentList` no entrecomilla; `pwsh -File` toma `--x` como parámetro | `lanzar_carga.ps1` con los argumentos por variable de entorno. |
| 3 | Cargador en `WinMain` (IND0A) → los inicializadores estáticos propios corren antes | El cargador debe correr antes de todo y sin CRT | Callback TLS `.CRT$XLB` (sin CRT) y solo-cargar termina ahí. **Solo-cargar queda limpio** (hashes iguales, 214/214 imports). |
| 4 | Con el CRT propio primero: `HeapAlloc` con montón NULL en `operator new` (`0x82119A`) durante los estáticos propios | `MemoryMgr.cpp` hace que el `operator new`/`delete` del proyecto usen el montón **del juego** (`0x82119A`/`0x821195`…), inexistente hasta que el CRT original ejecuta `_heap_init` | Cambio de orden: entrada → CRT original `0x824570`; la llamada a WinMain del arranque original (`0x8246EC`) se redirige a `IndWinMainOriginal`, que inicia el CRT propio y su WinMain instala los hooks y llama a `0x748710`. |
| 5 | Excepción en `ntdll` (escritura en 0x14) justo al entrar en el original, en otro hilo | Una DLL cargada por el cargador crea un hilo; su `DLL_THREAD_ATTACH` ejecuta el callback TLS dinámico del CRT propio (inicializa los `thread_local`) con el CRT propio sin iniciar | Se anula la ranura de `__dyn_tls_init` en la tabla de callbacks (el símbolo `__xl_c` resuelve a 0x400000 y `__dyn_tls_init_callback` no es la ranura: se busca la ranura **por valor** en el directorio TLS) **antes de cargar ninguna DLL**, y se restaura justo antes de iniciar el CRT propio (que inicializa el hilo principal por su cuenta). |
| 6 | Contador por páginas: ~1,1 millones de fallos en 40 s y luego cuelgue total | `DescribirDir` (`GetModuleHandleEx`, toma el cerrojo del cargador de DLL) dentro del cerrojo del contador → interbloqueo con un hilo que cargaba una DLL; además cada paso entre código propio y original es un par de fallos (ping-pong de páginas) | Clasificación por rangos en la ruta caliente (`Clase()`), cerrojo con `Sleep(1)`, `TryLock` en la guardia, aviso con volcado del hilo retenedor; y se añadió el modo **INT3** (sin ping-pong) como contador principal. |
| 7 | INT3: `Sin hooks` cae en `0x00855B8C` (escritura en 0xCCA7C4) | Un candidato falso a inicio de función (`0x855B90`, alineado a 16 pero **dentro de un `fstp [mem]`**): el `0xCC` era el byte de operando | `gen_lista_funciones.py`: exige relleno (`ret`/`int3`/`nop`) antes y descarta lo que cae **dentro de una instrucción** del descenso recursivo (65 descartes). El informe de fallo compara además el `.text` en memoria con el archivo. |
| 8 | Con INT3 cada primera entrada tardaba ~100 ms | El manejador vectorizado del proyecto (`app_debug`, instalado en `DllMain`) quedaba primero y registraba cada INT3 como «UNHANDLED EXCEPTION» con traza | El VEH del cargador se vuelve a registrar el primero tras `DllMain`. |
| 9 | `HEAP_CORRUPTION` (0xC0000374) una vez, con INT3 + hooks a los 10 s, con la lista antigua | Causa **no demostrada** (un `fail-fast` no pasa por el VEH); probable consecuencia de los INT3 falsos de la lista antigua (iteración 7). | Lista corregida; no ha vuelto a ocurrir en 5 ejecuciones con hooks e INT3 (150-200 s). |

Resultado: se alcanzó el menú sin quedarse atascado (9 iteraciones; ninguna superó 3 intentos sin progreso); desde la corrección 8 el arranque con hooks es estable (5 ejecuciones seguidas de 150-200 s sin caer).

## Contador de funciones originales ejecutadas

Modos (el primero es el principal):

* **`--contador-int3`**: pone un `INT3` en cada inicio de función de [`datos/funciones_originales.bin`](datos/funciones_originales.bin) (20.093 direcciones: destinos de `call`/`jmp rel32` y punteros en datos con relleno previo, mapas del proyecto y de IND0A, más 7 entradas halladas por el modo de páginas), **después de instalar los hooks** (las direcciones con un `jmp` a código propio no se tocan: 12.289) y las **cuenta en su primera entrada** (el VEH restaura el byte y registra dirección, llamador `[esp]` y milisegundos). Coste nulo después de la primera entrada.
* **`--contador-paginas`**: `.text` original en solo-lectura (DEP de proceso activado), una página ejecutable (`--contador`: solo la actual; `--contador-paginas`: las ya vistas siguen ejecutables), cada fallo de ejecución se registra. Contraste: cualquier entrada a una página que el modo INT3 no vea indica una función fuera de la lista. Hallazgo: 7 direcciones nuevas, ya incorporadas.

Límite: **cota inferior**: una función que no esté en la lista (entradas por `jmp` o tablas de datos que no se vieron) no cuenta. El modo de páginas (con hooks) vio 3 páginas de entrada (0x4CD000, 0x7F4000, 0x7F9000) que la lista inicial no cubría; sus direcciones de entrada se añadieron a la lista (7 en total) y la repetición con INT3 sube de 141 a 148 funciones.

### Resultados hasta el menú (datos completos en [`datos/`](datos/))

| Medida | Sin hooks (solo el original) | Con hooks (reconstruido) |
|---|---:|---:|
| Funciones originales distintas ejecutadas | **2.832** | **148** |
| CRT y bibliotecas (0x820000+) | 1.839 | 78 |
| RenderWare/D3D (0x750000–0x81FFFF) | 292 | 53 |
| Código del juego (0x401000–0x74FFFF) | 701 | 17 |
| Direcciones de la lista con INT3 | 20.067 | 7.786 (12.289 enganchadas) |

* [`datos/funciones_ejecutadas_hasta_menu_hooks.csv` / `.md`](datos/funciones_ejecutadas_hasta_menu_hooks.md): las **148** con la función propia que las llama (resuelta con el `.map`) y el milisegundo. Primeras entradas (las 9 primeras son el `operator new/delete` y `malloc` del CRT que usa el código propio): `0x0082119A`, `0x0082422B`, `0x008241B0`, `0x008286E0`, `0x0082871B`, `0x008214BD`, `0x0082413F`, `0x008213AE`, `0x00821195`; luego la inicialización de RenderWare (`__rwPluginRegistryAddPlugin` → `0x00801C30`, `_rwDeviceSystemRequest` → `0x007F5F70`, `__rwPluginRegistryInitObject` → ~15 funciones `0x007EDE90…0x0080AA40`, …).
* [`datos/funciones_ejecutadas_hasta_menu_sin_hooks.csv` / `.md`](datos/funciones_ejecutadas_hasta_menu_sin_hooks.md): las 2.832 sin hooks (el original completo).
* Lectura: **148 funciones originales siguen ejecutándose** (frente a 2.832 sin hooks): 78 son del CRT (`operator new/delete`, `malloc`, `memcpy`…), 53 de RenderWare/D3D y solo **17 del código del juego**. Eso es lo que falta para «0 originales hasta el menú» **en este camino** (menú principal, sin partida).

## Qué falta para llegar a 0 (y cómo seguir)

1. Las 17 del juego y las 53 de RenderWare de la lista: son los destinos que ya mapeó IND0A (envolturas, RW interno, callbacks); con la lista y el CSV de este lote se pueden atacar en olas pequeñas (son pocas).
2. Las 78 del CRT/bibliotecas: sustituir `operator new/delete` (hoy el código propio las reenvía al montón del juego), `malloc/free/realloc/calloc`, `_lock/_unlock`, `_output`… por el CRT propio. Requiere cambiar el `operator new` del proyecto (hoy a propósito reenviado al EXE) **sin avisar al juego**: es el primer paso real hacia el exe independiente (hay que decidir qué montón compartirán el código propio y las estructuras que el original sigue liberando).
3. Ampliar el contador a más caminos: cargar una partida (FastLoader), conducir, misión… El resultado de FastLoader (excepción en `0x004CA5B5`) debe resolverse primero.
4. La cobertura del contador mejora con una lista de funciones más completa (hoy 20.093 de las ~25.000 que se estiman; Ghidra no las tiene como función sin nombre) y/o con el modo de páginas como contraste.

## Pendiente / hallazgos fuera de alcance

* **FastLoader (partida 2) → excepción en el original `0x004CA5B5`** (lectura de dirección 0) tras ~58 s: el código original recorre una lista enlazada (`mov eax,[edi]; cmp edi,eax; jne`) con un puntero nulo. No se comprobó si ocurre también con el ASI en esa misma partida (SIM1 ya investiga un fallo con FastLoader/ranura 2). Registro: `ejecuciones/fastloader-*/`.
* La carga con hooks hasta el menú tarda ~110-140 s frente a ~23 s sin hooks (DLL Debug con 12.445 hooks, trazas y comprobaciones; no es del cargador).
* `ind1a_funciones.csv` llama «otro» a los llamadores en la pila o en memoria dinámica.
* El cierre del proceso al terminar el juego no pasa por los destructores del CRT original (el CRT propio termina con `exit`); sin efecto visible hoy.

## Guion de prueba a mano

```
pwsh -File D:\Original\wv-6\reversing\FnIND1A\probar_a_mano.ps1
```

(Antes: `bash D:/Original/wv-6/reversing/FnIND1A/construir.sh` si no existe `salida\gta_reversed_exe.exe`.) El script crea la carpeta `E:\JUEGOS\GTA San Andreas - CVehicle Test - IND1A`, copia el exe, toma `D:\Original\gta_lock`, lo lanza con `--limite 0` y espera a que lo cierres. Qué mirar:

1. Se abre una ventana de consola (los registros del proyecto) y, tras los vídeos de inicio (puede tardar **1-2 minutos** con la DLL Debug), el **menú principal** de GTA San Andreas. Debe responder al ratón/teclado y no cerrarse.
2. En la consola aparece **una sola vez** «Funciones del juego reemplazadas por código C++: 12445…» (si apareciera dos veces, algo cargó el ASI).
3. Cierra la ventana: el script muestra el código de salida. Si algo cae: `reversing\FnIND1A\ejecuciones\manual-*\ind1a_fallo.txt` y `ind1a_arranque.log`.
4. Para probar FastLoader (hoy falla): `pwsh -File ...\probar_a_mano.ps1 -FastLoader`.

El script no modifica la copia de pruebas; al terminar suelta el candado. Para borrar la carpeta IND1A: `pwsh -File ...\crear_carpeta_ind1a.ps1 -Quitar`.
