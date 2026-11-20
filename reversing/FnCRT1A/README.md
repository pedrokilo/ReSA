# FnCRT1A: sustitución del CRT y de los asignadores del EXE (19/11/2026)

Lote CRT1A del hito «EXE independiente» (IND1A, sección 8: 78 de las 148 funciones originales que se ejecutan hasta el menú son del CRT). Rama `crt1a` (sin subir). Sin ejecutar el juego: el banco carga el `gta_sa.exe` original (SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`, solo lectura) en un proceso de prueba y llama a sus funciones por dirección (método de [FnLIB1A](../FnLIB1A/README.md)).

| Tarea | Resultado |
|---|---|
| 1. Diseño del montón compartido | **Hecho**: [DISENO.md](DISENO.md). Un solo montón, el `_crtheap` del EXE (en Windows 2000+ `__active_heap == 1`: `HeapAlloc/HeapFree` sin cabecera); las versiones propias trabajan sobre ese mismo manejador, de modo que mezclar new/delete original y propio es seguro y los ganchos se pueden instalar de uno en uno |
| 2. `qsort` exacto (0x8247E0 + ayudante 0x824770) | **Hecho y validado**: `source/game_sa/Crt/CrtQsort.{h,cpp}`, **300.000 casos + 60.000 con comparadores reales, 0 diferencias**, comparando la secuencia completa de llamadas al comparador y el vector final |
| 3. CRT pequeño sin montón ni x87 | **8 funciones de cadenas** hechas y validadas (`CrtString.{h,cpp}`: strlen, strcmp, strncmp, strcpy, strcat, strncpy, strncat, strstr; 75.000 pares cada una, 0 diferencias). **x87/SSE (pow, floor, fmod, sqrt…, 0x8219F0, 0x8220F0, 0x823820, 0x829B99): no se hicieron** (sección 4) |
| 4. operator new/delete, malloc, free, realloc, calloc | **Reconstruidos y validados fuera del juego** (`CrtHeap.{h,cpp}`, 75.000 secuencias, 0 diferencias), **ganchos APAGADOS** (`GTASA_CRT_HEAP_HOOKS`); falta la prueba dentro del juego: [EJECUTAR.md](EJECUTAR.md) |

## 1. Archivos

- Código del proyecto: `source/game_sa/Crt/CrtQsort.{h,cpp}`, `CrtString.{h,cpp}`, `CrtHeap.{h,cpp}` (dos archivos por unidad; sin `__asm`, sin intrínsecos de cadena, sin x87). Registro en `source/InjectHooksMain.cpp` (`CrtQsortInjectHooks`, `CrtStringInjectHooks`, `CrtHeapInjectHooks`) y 9 filas en `docs/hooks.csv` (clase `ExeCrt`, `validated=0`).
- Banco (`reversing/FnCRT1A`): `bench_crt.cpp` (incluye los `.cpp` del proyecto tal cual, con `-DCRT_BENCH`), `build_bench.cmd`, `reserva_imagen.asm` (copia de IND0A/LIB1A: reserva `0x401000–0xCB1000` en el propio ejecutable de prueba), `neg_run.sh` (prueba negativa), `resultados/`.
- Reconstruido desde el desensamblado (capstone) del EXE. La primera pasada del qsort tenía una condición mal (`if (mid > lo)` en vez de `if (mid > loguy)`): el banco la detectó por la secuencia de comparaciones.
- Comandos: `build_bench.cmd <dir salida>` y después `<dir salida>\bench_crt.exe <ruta de gta_sa.exe> --casos 300000` (opciones `--solo qsort|cadenas|monton`, `--reales`, `--semilla S`; devuelve 0 si todo coincide).

## 2. Método del banco

Cada caso se ejecuta **dos veces con las mismas entradas y a la misma dirección de memoria** (primero el EXE, después el C++ sobre la arena restaurada) y se compara:

- **qsort:** secuencia completa de pares comparados (posiciones relativas) + contenido final del vector. Anchos de elemento 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 24 y 33; de 0 a 23.000 elementos (límites 0, 1, 2, 8 y 9 incluidos); patrones aleatorio, ordenado, inverso, órgano y muchas claves iguales; comparadores: u32 asc/desc, primer byte, `byte&7` (empates masivos), «todo igual», inconsistente (hash del contenido), diferencia con desbordamiento, y **los tres comparadores reales del EXE** 0x7C8720 (1 byte), 0x7567A0 (triángulos de 6 bytes) y 0x758240 (8 bytes). El cuarto uso real (0x759640 de `rpworld`) desreferencia punteros a materiales y no se pudo aislar.
- **cadenas:** valor devuelto (punteros normalizados) y arena completa (64 KiB con relleno `0xCD`, basura tras el cero del destino, direcciones sin alinear, alfabetos pequeños para parecidos parciales, subcadenas, longitudes de 0 a 300, `n` de 0 a 400).
- **montón:** el EXE llama a `HeapAlloc/HeapReAlloc/HeapFree` por su IAT (0x8581F8/F4/FC; se rellenan con las funciones reales) y lee 0xC9C2F4/F8, 0xC9ADCC y 0xC9AC68 de su propia imagen. Cada implementación trabaja en su montón (dos montones idénticos; se cambia el manejador global antes de cada llamada) y se compara nulo/no nulo, `HeapSize` del bloque (prueba el tamaño pedido al sistema, incluido `size == 0 → 1` y los rechazos `> 0xFFFFFFE0`), calloc a cero, realloc con el mismo contenido, nº de llamadas al manejador de new (sin manejador, manejador que rechaza, manejador que reintenta 0–3 veces) y `HeapValidate` de los dos montones. Tamaños de 0 a 2 GiB (fallos incluidos). `operator new` solo en la ruta de éxito (sin memoria y sin manejador lanza una excepción).

## 3. Resultados (`resultados/`)

Corrida completa (`bench_crt.exe gta_sa.exe --casos 300000`, semilla 12345; `resultados/corrida_completa.txt`):

| Función(es) | Casos / pares | Diferencias |
|---|---:|---:|
| qsort (0x8247E0) | 300.000 casos (por ancho 22.928–30.627; por comparador 39.371–39.828 los sintéticos y 7.465–7.506 los reales) | **0** |
| qsort, solo comparadores reales (`--reales --casos 60000`; `resultados/comparadores_reales.txt`) | 19.874 (0x7C8720) + 20.050 (0x7567A0) + 20.076 (0x758240) | **0** |
| strlen, strcmp, strncmp, strcpy, strcat, strncpy, strncat, strstr | 75.000 pares cada una | **0** |
| malloc, calloc, realloc, free, operator delete, operator new, malloc(0) | 75.000 secuencias; ≈ 327.000 operaciones de cada clase | **0** |

**Prueba negativa** (`neg_run.sh`, `resultados/prueba_negativa.txt`): 13 mutaciones de una línea (qsort: corte en 7, primer bucle estricto, ordenación corta con `>=`, sin la tercera comparación de la mediana, lado menor cambiado; cadenas: strncpy sin relleno, strncat sin cero final, strcmp con diferencia, strstr con aguja vacía nula; montón: calloc sin ceros, malloc(0) sin 1, realloc(p,0) sin liberar, free con la condición invertida): **las 13 detectadas** (12 con líneas `DIFERENCIA`; la de `free` rompe el montón y el proceso de prueba termina con error). Una mutación inicial (omitir el intercambio cuando los dos punteros son iguales) resultó equivalente (intercambiar un elemento consigo mismo no cambia nada) y se sustituyó. `prueba_negativa.txt` conserva las pasadas parciales (hubo que repetir tres mutaciones porque el disco D: llegó a 0 bytes libres durante la compilación).

**Compilación:** cada unidad (`CrtQsort.cpp`, `CrtString.cpp`, `CrtHeap.cpp` también con `GTASA_CRT_HEAP_HOOKS`, y `InjectHooksMain.cpp`) pasa `cl /Zs` con las opciones del `gta_reversed.vcxproj` de `wv-int` (C++ más reciente, `/Zc:preprocessor`, mismas definiciones e inclusiones): sin errores. **No se compiló ni enlazó el proyecto entero** (el worktree no tiene `build/` y D: se quedó sin espacio por otras compilaciones): lo hará el agente principal con el patrón del candado.

## 4. Estado de las 57 raíces CRT y pendiente

| Dirección | Nombre | Bytes | Estado |
|---|---|---:|---|
| 0x00820A10 | (sin nombre) | 626 | pendiente: sin motivo tecnico, falta tiempo |
| 0x00821195 | operator_new | 5 | HECHA, cubierta: salta a 0x82119A |
| 0x0082119A | operator_new | 36 | HECHA, banco OK (CrtHeap.cpp operator new), gancho apagado |
| 0x008211FD | FUN_008211fd | 63 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00821255 | FUN_00821255 | 63 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x008213AE | _free | 5 | HECHA, cubierta: salta a 0x8214BD |
| 0x008214BD | _free | 5 | HECHA, cubierta: salta a 0x82413F |
| 0x008214D0 | _strncmp | 57 | HECHA, CrtString.cpp: 0 dif. |
| 0x008219F0 | FUN_008219f0 | 498 | pendiente: usa x87/SSE (cierre: seh,sse,x87) |
| 0x00821B11 | FUN_00821b11 | 13 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00821B1E | _rand | 34 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00821BB5 | FUN_00821bb5 | 88 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00821CE6 | __onexit | 50 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00821F40 | _strncpy | 292 | HECHA, CrtString.cpp: 0 dif. |
| 0x008220AD | _sscanf | 52 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x008220F0 | _pow | 72 | pendiente: usa x87/SSE (cierre: seh,sse,x87) |
| 0x0082244B | FUN_0082244b | 187 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00822650 | _strstr | 134 | HECHA, CrtString.cpp: 0 dif. |
| 0x008228B5 | _isprint | 63 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00822D5D | __CxxThrowException@8 | 58 | pendiente: sin motivo tecnico, falta tiempo |
| 0x00822D97 | FUN_00822d97 | 17 | pendiente: sin motivo tecnico, falta tiempo |
| 0x00822DE5 | exception::exception | 74 | pendiente: sin motivo tecnico, falta tiempo |
| 0x00822E2F | FUN_00822e2f | 21 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x008230B8 | FUN_008230b8 | 87 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x0082318B | _fclose | 70 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x008232D8 | FUN_008232d8 | 19 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00823521 | _fread | 66 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00823674 | _fwrite | 66 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x0082374F | _fseek | 63 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00823798 | FUN_00823798 | 123 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00823820 | FUN_00823820 | 494 | pendiente: usa x87/SSE (cierre: seh,sse,x87) |
| 0x00823A30 | FID_conflict:_fwprintf | 84 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00823CAE | FUN_00823cae | 17 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00823E86 | _fflush | 70 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x008240C7 | _getenv | 54 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x0082413F | _free | 104 | HECHA, banco OK (CrtHeap.cpp), gancho apagado |
| 0x0082422B | __nh_malloc | 44 | HECHA, banco OK (CrtHeap.cpp, _nh_malloc), gancho apagado |
| 0x00824269 | FUN_00824269 | 406 | HECHA, banco OK (CrtHeap.cpp), gancho apagado |
| 0x00824416 | _calloc | 175 | HECHA, banco OK (CrtHeap.cpp), gancho apagado |
| 0x008247E0 | FUN_008247e0 | 643 | HECHA, qsort (CrtQsort.cpp): 0 diferencias |
| 0x00826261 | (sin nombre) | 55 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x008262A2 | (sin nombre) | 11 | pendiente: sin motivo tecnico, falta tiempo |
| 0x008262B8 | (sin nombre) | 103 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00826330 | _strlen | 139 | HECHA, CrtString.cpp: 0 dif. |
| 0x008263C0 | _strcmp | 135 | HECHA, CrtString.cpp: 0 dif. |
| 0x00826450 | (sin nombre) | 309 | HECHA, CrtString.cpp: 0 dif. |
| 0x00826590 | FUN_00826590 | 7 | HECHA, CrtString.cpp: 0 dif. |
| 0x008265A0 | FUN_008265a0 | 232 | HECHA, CrtString.cpp: 0 dif. |
| 0x00827B3D | __getptd | 113 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00827DEC | __stbuf | 136 | pendiente: sin motivo tecnico, falta tiempo |
| 0x00827E74 | __ftbuf | 42 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00827F4B | FUN_00827f4b | 1904 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00829B99 | FUN_00829b99 | 2861 | pendiente: usa x87/SSE (cierre: seh,sse,x87) |
| 0x0082AD12 | FUN_0082ad12 | 21 | pendiente: sin motivo tecnico, falta tiempo |
| 0x0082ADBE | __lock | 49 | pendiente: depende del estado por hilo, de bloqueos o de SEH del CRT (cierre: seh) |
| 0x00856BC0 | (sin nombre) | 10 | pendiente: sin motivo tecnico, falta tiempo |
| 0x0085742B | FUN_0085742b | 7 | pendiente: usa x87/SSE (cierre: x87) |

- **x87/SSE** (`pow` 0x8220F0, 0x8219F0, 0x823820, 0x829B99 y similares; el CRT elige SSE2 o x87 con la variable 0xC9C400): no se hicieron en este lote. Requieren el banco de flotantes con NaN/inf/0/denormales (`fcom` + `test ah`, no `!(a<=b)`) y los ayudantes `Veh83D*/X87Math.h`; no se empezaron para no dejar una función numérica a medias.
- **Estado por hilo, bloqueos y SEH** (`strtok` 0x82244B, `isprint` 0x8228B5, `__getptd` 0x827B3D, `rand`/`srand` 0x821B1E/0x821B11, `_lock`, `sscanf`/`sprintf`/`vsprintf`, E/S de archivos `fread/fwrite/fseek/fclose/fflush`): dependen de la tabla de datos por hilo del CRT (`_ptiddata`), del arranque (`_mtinit`) y de `FILE`. Son el siguiente lote lógico; `rand` es crítico (el RNG del EXE) y conviene hacerlo con su estado de hilo explícito.
- Ganchos del montón por defecto y fases (c) y (d) de `DISENO.md`: tras la prueba de `EJECUTAR.md`.
- `docs/hooks.csv` lleva 9 filas nuevas; no se regeneraron README/CurrentStatus ni sus recuentos.
