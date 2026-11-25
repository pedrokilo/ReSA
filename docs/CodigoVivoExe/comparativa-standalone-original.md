# Comparativa medida: `gta_sa_standalone.exe` frente al `gta-sa.exe` original (24/11/2026, rama `comparativa-original`)

Herramienta: `contrib/comparativa_standalone.py` (subcomandos `pe`, `codigo`, `escaneo`, `datos`, `volcados`, `ejecuciones`). CSV de apoyo en `docs/CodigoVivoExe/comparativa-standalone-original/`. Todo lo estático es de solo lectura sobre binarios, PDB, mapa y CSV; el `gta-sa.exe` (SHA256 `72ae59e4…ecc9b`, 5.189.632 B) solo se lee.

**Estado de la medición.** Las partes 1 (código, estático) y 2 (datos, estático) y la estructura del binario están hechas. Las partes dinámicas de la sección 3 (módulos tras 60 s, memoria, FPS, GPU, archivos abiertos por el original, tiempo a `gGameState` 9 con el binario actual) **NO se han ejecutado**: el coordinador pidió parar las ejecuciones de juego porque el usuario va a jugar, y la que estaba en cola se canceló antes de arrancar. La herramienta está lista (ver sección 3.4) y falta correrla. Esas filas van marcadas PENDIENTE; las cifras de ejecución que aparecen son de documentos anteriores y se citan como tales.

**Binario medido.** `D:\Original\wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe` reenlazado el 24/11 19:39 con `enlazar.cmd` (el anterior era del 23/11 23:21 y no incluía las fusiones CRT/x87, DirectDraw y datos de ese día): SHA256 `decee15e…8400`, 50.463.232 B. Se enlazó con los `.obj` del ASI Debug de `wv-int` (`build/source/gta_reversed.dir/Debug`), sin recompilar: ningún `.cpp` de `source/` es más nuevo que el ASI y el único cambio posterior del HEAD son docs. Incluye el `.obj` de superposición `over\AutoSteal.obj` (recompilación diagnóstica de un fuente sin cambios desde el 19/11). Es una compilación **Debug** con `/DEBUG`: los tamaños no son comparables con un Release.

## 1. Código

### 1.1 Tabla única de bytes por categoría (reproducida)

Base: las funciones de `docs/MemoryMap/binary_function_ranges.csv` agrupadas por `function_va` (17.765 funciones, 3.724.559 B; el README cuenta 19.169 *tramos*: una función puede tener varios). Las categorías salen de `docs/JuegoExe/funciones.csv`, `docs/JuegoExe/excluidas.csv` y `docs/RenderWareExe/funciones.csv`. Archivo: `codigo-tabla.csv` (y `codigo-tabla-detalle.csv`).

| Categoría | Funciones | Bytes | % de bytes de función |
|---|---:|---:|---:|
| A. Reemplazada por C++ (juego 11.684 / 2.978.807 B; RenderWare y D3D 795 / 208.780 B) | 12.479 | 3.187.587 | 85,58 % |
| B. Biblioteca de terceros "enlazada" (D3DX9 922 / 327.199 B; libpng+zlib 110 / 53.518 B) | 1.032 | 380.717 | 10,22 % |
| C. Excluida pero cubierta: `usercall-cubierta`, la lógica ya la ejecuta C++ desde sus llamadores | 52 | 18.713 | 0,50 % |
| D. Excluida / no reconstruible en C++ puro (excluidas del juego y RW no reconstruibles, sin las 52 cubiertas; protección 2.551 B, QuickTime 2.871 B, `usercall-renderware` 7.188 B, 3DNow! de D3DX9, etc.) | 159 | 25.476 | 0,68 % |
| E. CRT y biblioteca estática del EXE **fuera de los dos mapas** (0x820000–0x857FFF) | 4.043 | 112.066 | 3,01 % |
| Por reconstruir (A–D del mapa) | 0 | 0 | 0 % |
| **Total** | **17.765** | **3.724.559** | 100 % |

Cómo cuadra con lo publicado: sobre la base de los dos mapas (juego + RW, 0x401000–0x81FFFF) lo reemplazado es 3.187.587 / 3.612.849 B = **88,2 %** (el script da 88,33 % porque suma tramos del mapa binario y no los `bytes` de los CSV; diferencia de 4.001 B por cinco VA de los CSV sin rango y por el redondeo de tramos). Terceros = 10,5 %, excluidas = 1,2 % sobre esa base. **Lo que el README no decía**: existe una categoría E, 4.043 funciones y 112.066 B de CRT en 0x820000–0x857FFF que ningún CSV cubre; en el independiente las sustituye el CRT del enlazador, no una reconstrucción función a función. Con ella en el denominador lo reemplazado por C++ baja a 85,6 %. Además las funciones suman el 81,8 % de los 4.550.656 B de la región de código; el resto es relleno, tablas de salto y datos en código (no medido por separado).

Qué significa "enlazada" aquí (hallazgo): **libpng/zlib** vienen de `libpng1012.lib` / `zlib113.lib` (fuentes propias compiladas, enlazadas estáticamente). **D3DX9 (922 funciones, 327 KB) no está enlazada**: el independiente no la contiene; el C++ llama por `GetProcAddress` a `d3dx9_43.dll` (SDK de junio de 2010, instalada en SysWOW64; `d3dx9math.cpp`, lotes EXT1A/R21A). Es una dependencia externa que no es del sistema. No aparece en la tabla de importación (se carga en ejecución): compruébese en la sección 3.4. **Actualización 24/11/2026 (rama `enlazar-d3dx9`, [d3dx9-enlace.md](../RenderWareExe/d3dx9-enlace.md)):** enlazar D3DX9 estáticamente no es posible (`d3dx9.lib` del SDK es solo de importación: 329 miembros de importación, sin código); lo que el juego usa de verdad (matrices, `Vec3Transform*` y el ensamblador de *vertex shaders*, 12 shaders `vs_2_x` en la sesión medida) es ahora C++ propio, bit a bit igual que la DLL, y **el independiente no carga ninguna `d3dx9*.dll`/`d3dcompiler*.dll` en una partida normal** (módulos a los 20/60/120 s y espía de archivos: 0 accesos, también con ambas DLL inexistentes para el proceso). La DLL queda solo de respaldo (`vs_1_1`, HLSL; `contrib/copiar_d3dx9_redist.ps1`).

### 1.2 Verificación independiente sobre el binario del independiente

| Prueba | Resultado | Archivo |
|---|---|---|
| Reserva `0x401000–0xCB0FFF` en el archivo (9.109.504 B) | **0 bytes no nulos**: no hay ningún byte de código del original en el archivo; el cargador la rellena con INT3 (0xCC) en 0x401000–0x857FFF | `escaneo.log` |
| Copias idénticas de funciones del original dentro del archivo (12.321 tramos ≥ 24 B, coincidencia exacta del cuerpo) | **19 coincidencias, 1.446 B, todas de la categoría E** (rutinas de CRT de 24 a 292 B, en el código propio del exe fuera de la reserva): código idéntico que genera el mismo compilador/CRT, no bytes copiados. **Ninguna** de las categorías A–D | `codigo-copias-identicas.csv` |
| Ganchos del catálogo (`docs/hooks.csv`, 11.541 direcciones hexadecimales) frente al mapa | 10.717 funciones tienen gancho en su inicio (2.780.906 B = 74,7 % de los bytes de función). 1.977 funciones "reemplazadas" (443.071 B, todas del juego) no tienen gancho en el catálogo; 215 con gancho no están reemplazadas (36.390 B, casi todas CRT, E) | `codigo-reemplazadas-sin-gancho.csv`, `codigo-gancho-no-reemplazada.csv` |
| Contador `codigo-original-distinto` / `fase8_codigo_original.csv` en las ejecuciones archivadas (`ejecuciones-contador-codigo-original.csv`) | 54 carpetas con registro, 32 con latido final: **97.424 fotogramas sumados, `codigo-original-distinto=0`, `hits=0`, 0 excepciones en las 32**; 4 carpetas antiguas tienen filas en el CSV (versiones previas al cierre). El contador cuenta solo los INT3 *alcanzados*: es prueba de "no se ejecutó", no de "no hay" | CSV |

Interpretación honesta: *"¿cuántas funciones del EXE tienen implementación equivalente enlazada?"* Por catálogo: 12.479 reemplazadas + 1.032 de bibliotecas = 13.511 de 17.765 funciones (76,1 % de las funciones; 95,8 % de los bytes); el resto son 52 cubiertas, 159 excluidas y 4.043 de CRT sustituidas globalmente. *"¿Siguen siendo bytes copiados del EXE los que se ejecuten?"* En el archivo no hay ninguno (reserva a cero) y en las 32 ejecuciones archivadas ningún INT3 fue alcanzado; el contador no demuestra que las zonas **no ejercitadas** (ciudad, misiones, interiores, agua, guardado) estén libres: en esas el código original sería un INT3 y el proceso terminaría con `0xDEAD0003` (modo estricto). Las ejecuciones archivadas cubren sobre todo arranque, apertura, callejón y la casa (sección 4).

**PENDIENTE (dinámico):** volcado en ejecución de 0x401000–0x857FFF del binario actual para contar, función a función, `E9`→C++ / INT3 / otro, y confirmar que fuera de los ganchos de 5 B todo es 0xCC (`codigo-ejecucion-*.csv`, hecho por `analizar_volcado_codigo` cuando exista `volcado_codigo_std.bin`). Hay que correr `comparar.py --fase medir` (3.4).

## 2. Datos

Región de datos del original: 0x858000–0xCB0FFF = 4.526.080 B (`.rdata` 311.296, `.data`+`.bss` 4.169.728, `_TEXT_HA` 69.632, `_rwdseg` 4.096, `.rsrc` 4.096). Inicializado en el archivo: 641.024 B.

### 2.1 Qué copia hoy el cargador (rangos exactos)

16 trozos, 640.512 B (512 menos que 641.024: `_rwdseg`, íntegra a ceros, no se emite). Hallados byte a byte en el archivo del independiente (`datos-trozos-copiados.csv`): 0x858000 y siguientes en pasos de 0xC000 hasta 0x8A0000 (cinco de 49.152 B, uno de 13.312 B), 0x8A4000–0x8E0000 (siete trozos, 262.144 B: todo el `.data` inicializado), 0x8E0000 (16.384 B), 0xC9E000 (49.152 B) y 0xCAA000 (19.456 B) = `_TEXT_HA` completo (68.608 B), 0xCB0000 (1.536 B, `.rsrc`). El `.bss` (3.907.584 B) no se copia: es la reserva a cero.

**32 % de lo copiado son ceros** (204.674 de 640.512 B): equivalen a no copiar.

### 2.2 Propios frente a copiados (medido con el PDB y el escaneo del código del exe)

Objetos propios: variables `DatosExe::Bss|Data|Rdata|R5` del PDB del independiente con su tamaño exacto (2.249 con la dirección original en el nombre, `datos-resumen.json`; otros 179 sin dirección legible en el nombre quedan fuera: cota inferior). Unión de intervalos sobre las direcciones originales:

| Sección | Tamaño | Copiado (incrustado) | Cubierto por objeto propio | % propio | Referenciado por dirección y sin objeto |
|---|---:|---:|---:|---:|---:|
| `.rdata` | 311.296 | 308.224 | 1.417 | 0,46 % | 23.623 |
| `.data` inicializado | 262.144 | 262.144 | 8.859 | 3,38 % | 5.891 |
| `.bss` | 3.907.584 | 0 (reserva a cero) | 245.167 | 6,27 % | 57.775 |
| `_TEXT_HA` | 69.632 | 68.608 | 0 | 0 % | 213 |
| `_rwdseg`, `.rsrc` | 8.192 | 1.536 | 0 | 0 % | 8 |
| **Total** | **4.526.080** | **640.512** | **255.443** | **5,64 %** | |

- **Datos inicializados (lo que sí se copia):** propios 10.276 B (1,6 % de los 640.512 B); **98,4 % es imagen copiada del EXE**.
- **Sobre la región de datos completa:** propio 5,64 %; copiado 14,15 %; el 80,2 % restante es `.bss` reservado a cero que nadie ha sustituido (3.604.642 B sin objeto ni referencia por dirección en el código del exe: estado que se usa por registro base/punteros o que nadie usa; no distinguible estáticamente).
- **Sobre el archivo `gta-sa.exe` (5.189.632 B):** los 640.512 B copiados son el 12,3 %, y los propios inicializados (10.276 B) el 0,2 %.
- Las `.bss` propias son sobre todo `sl_BD0B78` (204.800 B) y `sl_A8B430` (21.504 B).

### 2.3 Clasificación de los 640.512 B copiados

Prioridad de asignación por byte: f > e > o > a > c > b > d. `datos-clasificacion.csv` y `datos-clasificacion-por-seccion.csv`.

| Clase | Bytes | % de lo copiado | Medido / estimado |
|---|---:|---:|---|
| f. Protección del EXE (`_TEXT_HA`, `_rwdseg`) | 68.608 | 10,71 % | medido (secciones) |
| f. IAT (888 B, la reescribe el cargador) y `.rsrc` (1.536 B) | 2.424 | 0,38 % | medido |
| e. Vtables y tablas de ≥ 2 punteros consecutivos a código original (697 tramos) | 42.056 | 6,57 % | medido (heurística; 16.713 punteros a código en total) |
| o. Con objeto propio `DatosExe::*` | 10.276 | 1,60 % | medido |
| a. Leído por C++ por dirección fija (`push/mov imm`, operando absoluto en el código del exe) sin objeto propio | 7.382 | 1,15 % | medido, **cota inferior**: la extensión de tablas indexadas y `StaticRef<T>` por `push` se toma del ancho de acceso o del tamaño de `docs/DatosExe/objetos.csv` |
| c. Apuntado desde otras tablas de la imagen | 15.506 | 2,42 % | medido (palabras alineadas con valor en el rango) |
| b. Solo lo leen funciones del EXE **no** sustituidas (RW sin reconstruir, CRT, libs): en el independiente esas funciones son INT3 y no se ejecutan | 29.913 | 4,67 % | del mapa estático `objetos.csv` (previo a los lotes de datos recientes); estimado |
| b2. Solo lo leen funciones del EXE que ya son C++ (copia probablemente muerta) | 51.717 | 8,07 % | estimado |
| d1. Sin referencia estática, ceros | 149.909 | 23,40 % | medido |
| d2. Sin referencia estática, no nulo (base+registro, tablas indexadas o sin lector: **no separables** estáticamente) | 262.721 | 41,02 % | medido el total; la mezcla es incógnita |
| **Total** | **640.512** | 100 % | |

Notas: (1) **La clase (d) no se puede partir en "c: leído por registro base" y "d: no leído por nadie" sin trazar lecturas en ejecución**; los 142.249 B (`.rdata`) y 185.579 B (`.data`) "sin clasificar" del informe anterior son el mismo concepto y las cifras no son comparables (distinto método). (2) Hay 83 direcciones del código del exe que apuntan **dentro de un objeto propio** (`datos-refs-en-objeto-propio.csv`): lectura por dirección al lado de un dato propio, el patrón de los fallos de `auditoria-datos.md` (GenerateOneRandomCar, trucos); es una lista de revisión, no de fallos confirmados. (3) No se midieron lecturas dinámicas de la imagen; la única cota dinámica prevista es comparar el volcado de memoria con la imagen inicial (subcomando `volcados`, PENDIENTE).

### 2.4 Límites

Se ven: operandos absolutos e inmediatos del código propio (10.934.134 instrucciones desensambladas, 3.151 direcciones distintas en el rango). No se ven: registro base, punteros calculados, tablas guardadas fuera de la imagen, accesos de CLEO/mods; el desensamblado lineal puede desincronizarse con datos incrustados en `.text` (se usó `skipdata`). Los 179 objetos sin dirección en el nombre no se contaron.

## 3. Comparativa del binario y de la ejecución

### 3.1 Estructura (medido, `estructura-*.csv`)

| | Original | Independiente (Debug) |
|---|---:|---:|
| Archivo | 5.189.632 B | 50.463.232 B (9,7×) |
| Tamaño de imagen | 9.113.600 B | 51.023.872 B |
| Secciones | `.text` 4.546.560, `_rwcseg` 4.096, `.rdata` 311.296, `.data` 4.169.728 (262.144 en archivo), `_TEXT_HA` 69.632, `_rwdseg` 4.096, `.rsrc` 4.096 | `.text` 43.013.472 (empieza con 9.109.504 B de reserva a cero), `.rdata` 6.598.142, `.data` 1.397.068, `.fptable` 128 |
| DLL importadas / funciones | 9 / 214: WINMM, vorbisfile, WS2_32, EAX, KERNEL32, USER32, GDI32, ADVAPI32, ole32 | 15 / 503: DSOUND, d3d9 (`Direct3DCreate9`), WINMM, dbghelp, KERNEL32, USER32, GDI32, IMM32, ole32, OLEAUT32, VERSION, ADVAPI32, SETUPAPI, SHELL32, DINPUT8 |
| Cargas en ejecución (según fuentes) | d3d9, dinput8, ddraw, QuickTime, DirectX por `LoadLibrary` | ~~`d3dx9_43.dll` por `GetProcAddress`~~ ya no en el camino normal (solo respaldo, perezoso; 24/11/2026); `eax.dll`, `ogg/vorbis` según carpeta |

El original **no** importa d3d9/ddraw/dinput8/QuickTime de forma estática (carga retardada). El independiente ya no necesita WS2_32 ni vorbisfile.dll. El original lleva dinput8.dll en la carpeta de pruebas. Origen del dato de ddraw/QuickTime: `docs/RenderWareExe/directdraw-sustitucion.md` y estado (no medido aquí).

### 3.2 Ejecución: lo ya medido en documentos anteriores (no repetido)

De `ejecucion-comparacion-original.md` (24/11, binario del 23/11 23:21, ventana 800×600, partida 2): FPS 24,4–25,2 en ambos (limitador a 25); carga `gGameState` 8→9 en 2,2 s el independiente y 11,5 s el original; tiempo total hasta 9 desde el inicio del proceso: 4,2 s (independiente) y 51 s (original, con logos y menú por teclado), 13,3 s ASI; interior de la casa y marcadores idénticos; exterior despejado/nublado casi idéntico; **diferencias reales: espejo del ropero, lluvia intermitente, un amanecer, no corregidas**. De `sin-original.md`: 1.896 accesos a archivos del independiente con el espía de `ntdll`, 0 a `gta-sa.exe`, vivo y en estado 9 durante 330 s sin original ni `--imagen-externa`. Eso no es una medición con el binario actual.

### 3.3 PENDIENTE (requiere ejecutar)

| Medida | Estado |
|---|---|
| Módulos cargados tras 60 s (`EnumProcessModulesEx`): ddraw, QuickTime, Media Foundation, d3dx9 | **d3dx9: medido el 24/11/2026 con el exe de `enlazar-d3dx9`: 116 módulos a los 20/60/120 s, ninguno `d3dx9*`/`d3dcompiler*` (con `d3dx9_43.dll` y `D3DCompiler_43.dll` presentes en la carpeta o inexistentes para el proceso); con el exe anterior: 117 módulos con `C:\WINDOWS\SYSTEM32\d3dx9_43.dll`.** ddraw, QuickTime y Media Foundation: PENDIENTE |
| Archivos que abre cada uno (espía; el original bajo el espía no se ha probado: el protector del EXE podría detectar el depurador de la inyección) | PENDIENTE (independiente: parcial, ver 3.2) |
| Memoria (working set, privada), FPS en el mismo punto | PENDIENTE |
| GPU/adaptador (`logs/log.log`) | PENDIENTE |
| Arranque a `gGameState` 9 con el binario reenlazado | PENDIENTE |

### 3.4 Cómo ejecutarlo (preparado, sin lanzar)

`reversing/FnComparar/comparar.py --fase medir --t-medir 60 --volcado-dir <dir>` (nueva fase: módulos con ruta y tamaño, memoria, 3 medidas de FPS de 10 s, handles, cliente de la ventana, y volcado de 0x401000–0xCB0FFF) y `ejecutar_comparar.ps1 -Espia` (nuevo: lanza el exe con `espia_archivos.dll` por `reversing/FnSinOriginal/lanzar_espiado.py`; quita `--ruta-original`, ya inútil). Ejemplo, una vez por modo `std` y `orig` con el candado propio del script: `pwsh -NoProfile -Command "& .\ejecutar_comparar.ps1 -Modo std -Etiqueta med_std1 -Limite 150 -Espia -Sonda comparar.py -SondaArgs '--fase medir --t-medir 60 --volcado-dir D:\Original\_comparar\volcados'"`. Después `python -I contrib/comparativa_standalone.py codigo` y `... volcados`. Aviso: en el primer intento `-SondaArgs` con `Start-Process -File` falla por el entrecomillado; usar `-Command`. El script de ejecución corrige una ruta que apuntaba a `D:\Original\wv-comp` (inexistente).

## 4. Lo que falta (priorizado, con evidencia)

Para **independiente del EXE en sentido estricto** (no leer ni ejecutar nada del original en ejecución) el binario ya lo es en lo medido: reserva a cero, imagen de datos incrustada, ningún `gta-sa.exe` abierto. Lo que sigue es lo que lo separa de "sin deuda de datos" y de "fiel".

| # | Falta | Tamaño | Qué haría falta | ¿Cerrable en C++ puro? |
|---|---|---:|---|---|
| 1 | Zonas del juego no ejercitadas con el independiente: cualquier INT3 alcanzado mata el proceso (modo estricto). Las 32 ejecuciones archivadas son arranque, casa, callejón, calle | no medible en bytes (desconocido) | barrido por escenas (ciudad, misiones, interiores, agua, guardado/carga, vehículos); `--censo` para listar los INT3 sin morir | sí |
| 2 | Validación pendiente: 1.711 entradas sin evidencia en 52 grupos (cabecera de estado); vigencia acreditada 0 | 1.711 funciones | seguir por lotes | sí |
| 3 | Imagen de datos copiada: 640.512 B, 98,4 % sin objeto propio; 262.721 B no nulos sin referencia estática (¿lectura por base+registro?) | 640.512 B | trazar lecturas en ejecución (página protegida/depuración) para partir (c)/(d); reemplazar objetos por lotes G5–G8; quitar b y b2 (81.630 B probablemente muertos) tras comprobar | sí, salvo (f) |
| 4 | `.bss` en direcciones originales (reserva a cero): 3.907.584 B, 245.167 B con objeto propio, 57.775 B por dirección fija sin objeto | 3,6 MB sin sustituir | sustituir G5 restante; los grandes (pools, streaming) por registro base necesitan el tipo | sí |
| 5 | Vtables y tablas de punteros a código original | 42.056 B (697 tramos) | que el compilador genere las vtables y retirar `*(uint32*)self = 0x85…`; los 469 `RH_Scoped…` | sí, pero es un cambio amplio (G9) |
| 6 | Protección del EXE (`_TEXT_HA`, `_rwdseg`) copiada | 68.608 B copiados; 18 funciones / 2.551 B de código excluidas | decisión de proyecto: descartada; quitar la copia si nadie la lee (213 B referenciados por dirección) | la copia sí; el código del protector no se reconstruye |
| 7 | D3DX9: 922 funciones (327.199 B) + 10 de 3DNow! (3.404 B) no reconstruibles; el binario dependía de `d3dx9_43.dll` (SDK 2010), que no viene con Windows | 330 KB de código original | **resuelto el 24/11/2026 para lo que el juego usa** (C++ propio; DLL solo de respaldo): ver [d3dx9-enlace.md](../RenderWareExe/d3dx9-enlace.md); enlace estático real imposible; los 3DNow! no los llama nadie | dependencia externa **eliminada del camino normal**; queda el respaldo para `vs_1_1`/HLSL |
| 8 | RenderWare no reconstruible: 61 funciones, 11.470 B (QuickTime 38/2.388 B retirada; D3D9 3; `usercall-renderware` 17/7.188 B) | 11.470 B | librw u otra implementación; ABI de registros | parcial |
| 9 | Excluidas del juego 155 funciones, 33.075 B (arranque 27/2.895 B, protección, saltos puros, `usercall-cubierta` 52/18.713 B cubiertas) | 33.075 B | las cubiertas ya se ejecutan por sus llamadores; el resto no tiene equivalente | no (ABI de registros, `__asm`) |
| 10 | CRT y bibliotecas del EXE fuera de los mapas: 4.043 funciones, 112.066 B, sustituidas por el CRT del enlazador (no función a función); 215 tienen gancho del catálogo | 112.066 B | decidir si "reemplazado" incluye CRT sustituido por otro; fidelidad bit a bit en x87 ya tratada en `ejecucion-sustitucion-crt-rw-x87.md` | sí, con diferencias posibles de redondeo |
| 11 | Diferencias visuales y de comportamiento sin resolver contra el original: espejo del ropero, lluvia, un amanecer (ver 3.2); fallo gráfico que investiga la rama `fix-skin-expansion` | n/a | bisección; `CWeather::LightningFlash` etc. | sí |
| 12 | 83 lecturas por dirección junto a datos propios (riesgo de dos copias) | 83 direcciones | revisión y repunteo | sí |

## 5. Qué se midió y qué se estimó

Medido: tabla de código (CSV), reserva a cero, 19 coincidencias de cuerpo, estructura PE e importaciones, trozos copiados y su presencia en el exe, intervalos de objetos propios del PDB, 3.151 referencias por dirección en el código del exe, tablas de punteros a código, contador de 32 ejecuciones archivadas. Estimado o parcial: clases (a) por extensión, (b)/(b2) del mapa previo, (d2) indistinguible, objetos sin dirección en el nombre. No medido: todo lo de ejecución del binario actual (3.3), lecturas dinámicas de datos, zonas no jugadas. Disco: D: llegó a 224 MB libres durante la sesión (de ahí que se rehicieran el escaneo y los CSV); al cierre quedan 156 GB.
