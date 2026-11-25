<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RenderWare, ronda 5: rwcore (texturas, matriz), clumps, sectores de RpWorld y generador de tiras (22/11/2026, rama `rw5`)

Origen: `ejecucion-ronda4.md` sec. 4, puntos 2 y 3 (llamadas del C++ al EXE por direccion fija: 8,0 KB de RpWorld y 2,1 KB de rwcore). Se reconstruyen las 11 funciones siguientes, todas en C++ puro (sin `__asm`/`naked`), con hook `RH_ScopedGlobalInstall` y fila en `docs/hooks.csv`.

## 1. Papel de cada funcion (confirmado con los llamadores del EXE)

`rea` (`mcp__rea__procedure_*`, documento `target-72ae59e44c76.exe`) respondio `invalid_request` a todas las consultas por direccion que se probaron (`0x7F2930`, `7F2930`, `FUN_007f2930`), asi que los llamadores se buscaron con `reversing/FnRW5/llamadores.py` (barrido de `call`/`jmp` rel32 y de palabras de 32 bits en las secciones de `gta-sa.exe`) y con el desensamblado (capstone, `reversing/FnVertexShader/exedump.py`).

| Direccion | Bytes | Funcion C++ (archivo) | Papel | Llamadores / tablas del EXE |
| --- | ---: | --- | --- | --- |
| `0x75A960` | 2.933 | `_rpBuildMeshGenerateExhaustiveTriStrip` (`rpworld.cpp`) | generador de tiras exhaustivo de una `RpBuildMesh` (tercer argumento = modo de union de `0x75A660`) | `0x75A94C` (`RpBuildMeshGenerateExhaustiveTriStrip`, `0x75A940`, modo 1) y `0x75B4EC` (`...IgnoreWinding...`, `0x75B4E0`, modo 0) |
| `0x762F30` / `0x763310` | 980 / 663 | `_rpWorldAtomicSectorStreamRead` / `_rpWorldPlaneSectorStreamRead` (`rpworld_s30a31a.cpp`) | lectura de un sector hoja / de un plano y sus subarboles | `0x762CB7` y `0x762D5D` (`RpWorldStreamRead`, `0x762960`) y llamadas mutuas |
| `0x7624A0` / `0x762740` | 672 / 533 | `_rpWorldAtomicSectorStreamWrite` / `_rpWorldPlaneSectorStreamWrite` (`rpworld_s30a31a.cpp`) | escritura de un sector hoja / de un plano | `0x7623ED` y `0x762404` (`RpWorldStreamWrite`, `0x762150`) y llamadas mutuas |
| `0x74A880` | 397 | `_rpClumpAtomicStreamWrite` (`rpworld_s30a31a.cpp`) | escribe un atomo del clump (o suelto) | `0x74A877` (`RpAtomicStreamWrite`, `0x74A850`) y `0x74AD10` (`RpClumpStreamWrite`) |
| `0x74A770` | 224 | `_rpClumpBuildGeometryList` (`rpworld_s30a31a.cpp`) | reune las geometrias distintas de los atomos de un clump | `0x74A62B`, `0x74AA68`, `0x74AC26` (tamano, `RpClumpStreamGetSize`/`Write` del clump) |
| `0x7F40F0` | 966 | `_rwTextureDefaultRead` (`rwcore.cpp`) | lectura de textura por defecto (`RwTextureCallBackRead`); sin mipmapping delega en `0x7F4AE0` | sin llamadas directas: puntero `+0x14` del modulo de texturas (`_rwTextureModuleOpen`, `0x7F3EA0`) |
| `0x7F5140` | 652 | `_rwTextureDefaultMipmapGeneration` (`rwcore.cpp`) | generacion de mipmaps por defecto (`RwTextureCallBackMipmapGeneration`) | puntero `+0x2C` del mismo modulo |
| `0x7F53D0` | 292 | `_rwTextureDefaultFindName` (`rwcore.cpp`) | busqueda de textura por nombre sin distinguir mayusculas (`RwTextureCallBackFind`) | puntero `+0x18` del mismo modulo (se cita en `0x7F4014`) |
| `0x7F2930` | 234 | `_rwMatrixQueryRotateHalfTurnAxis` (`rwplcore.cpp`) | eje de un giro de 180 grados dentro de `RwMatrixQueryRotate` (`0x7F2720`) | `0x7F27EC` (solo esa funcion) |

Las tres de textura eran raices vivas en la tabla del modulo (el C++ de `_rwTextureModuleOpen` escribia sus direcciones): ahora escribe los punteros a las funciones C++. Las de sectores, clump y tiras las llamaba el C++ ya reconstruido por direccion (`reinterpret_cast<...>(0x...)`), y ahora llama a la funcion C++ directamente. Ninguna tiene vtable ni otro llamador desconocido.

## 2. Verificacion por pares contra el original (`reversing/FnRW5/`)

Arnes (`LEEME.md`): el codigo ORIGINAL de `gta-sa.exe` se mapea en un proceso x86 propio (el arnes se enlaza con base `0x400000` y reserva `0x401000..0x1201000` como una seccion de su imagen, y copia ahi las secciones del EXE en sus direcciones originales) y se ejecuta de forma nativa. Las llamadas del original al motor (reserva de memoria, listas libres, flujos, registros de plugins, errores, funciones de imagen y raster) pasan por `jmp` parcheados a las MISMAS funciones simuladas que usa el C++ nuevo, que `extraer.py` copia de las fuentes del proyecto (`core_*.inc`, nunca a mano). Cada prueba ejecuta original y C++ con la misma entrada y compara: valor devuelto, **registro completo de llamadas** (con los bytes escritos y los tamanos reservados, en orden) y la memoria (entradas y reservas). Se inyectan fallos en la k-esima llamada que puede fallar para cubrir los caminos de error. Se compruebo la sensibilidad con mutaciones (cambiar `>` por `>=` en la puntuacion de tiras: 1.604 de 2.000 diferencias; invertir el 4/8 de la paleta o el modo de bloqueo de texturas: 255, 140 y 209 de 900).

| Funcion | Entradas | Resultado |
| --- | --- | --- |
| `0x7F2930` (matriz, x87 a 24 bits con `_controlfp`) | 1.000.000 matrices (aleatorias, empates de diagonales, NaN, infinitos, 3,4e38) | 0 diferencias en los bits de los tres floats (con NaN se acepta distinto signo/carga del NaN: x87 propaga el operando, SSE devuelve el indefinido) |
| `0x7F53D0` (busqueda de texturas) | 13.000 (0-3 diccionarios, mayusculas/minusculas, bytes altos, vacios, 20 caracteres, con y sin diccionario actual) | 0 |
| `0x7F40F0` / `0x7F5140` (lectura y mipmaps, colaboradores simulados) | 600 + 600 por semilla (formatos paletizados 4/8 bits, con/sin imagen de origen, profundidad 8/32, 0-4 niveles, nombres de 300 caracteres, fallos en cada paso) | 0 |
| `0x7624A0` / `0x762740` (escritura de sectores y planos) | 12.000 (arboles de profundidad 3, mundo nativo o no, 0-4 juegos de coordenadas, flags 0x10/8, fallos inyectados) | 0 |
| `0x762F30` / `0x763310` (lectura de sectores y planos) | 12.000 (flujos sinteticos con versiones validas e invalidas, fallos, mundo nativo o no) | 0 |
| `0x74A880` / `0x74A770` (atomo de clump, lista de geometrias) | 12.000 (0-3 marcos y geometrias de contexto, fallos) | 0 |
| `0x75A960` (tiras) | 28.000 mallas (rejillas con diagonales al azar y triangulos al azar, 1-3 materiales, relleno aleatorio) | 0: mismas reservas, mismas llamadas a las listas libres y mismo `RpMeshHeader` byte a byte; las funciones auxiliares (`0x75A090`, `0x75A250`, `0x75A370`, `0x75A660`, `0x759640`, qsort `0x8247E0`) son ORIGINALES en ambos lados |

Diferencias del EXE que el arnes destapo y que la reconstruccion reproduce:

* `0x74A880`: el fallo de la cabecera de estructura (bloque 1) devuelve NULL **sin** poner `context->ok = 0` (los demas fallos si).
* `0x762F30` y `0x763310`: la limpieza elige `_rpWorldSectorDeinstanceAll` o `_rpWorldSectorDestroyRecurse` por el byte `+3` del mundo (`object.privateFlags & 1`), no por `world->flags & 0x1000000`.
* Los sectores guardan la caja **ajustada** (`tightBoundingBox`, `+0x60/+0x6C`), no la exterior.
* Banderas de geometria: `0x10` = normales (`sector+0xC`), `0x08` = pre-iluminacion (`sector+0x30`); en lectura las normales exigen todos los bytes y la pre-iluminacion solo un resultado distinto de cero.
* `0x75A960`: en la prueba de tira con rotacion 2 el segundo indice se lee de `v[rot + 1]` sin modulo (la palabra de relleno del triangulo), como el EXE; la puntuacion es `2 * (extendido + 1) - indices` y gana solo una estrictamente mayor.

Diferencias que NO se reproducen (basura de pila del original, enmascarada en el arnes): `collSectorPresent` (4 bytes en la cabecera de 0x2C de un sector) y los indices de marco/geometria de un atomo sin lista de marcos/geometrias (8 bytes); la reconstruccion escribe 0 en esos campos (no los lee ningun lector del juego). `0x763310` y `0x762F30` en el EXE no acotan la longitud del bloque de estructura de un plano y desbordarian la pila con un flujo corrupto; aqui devuelve NULL.

## 3. Compilacion y comprobaciones

* `_build_RW5.sh` (candado `compile_lock`, worktree `wv-rw5`): log `D:\Original\_build_RW5.log`, `EXIT=0` (0 errores `C`; el primer intento fallo por tres erratas que ya estan corregidas: una `/` perdida en dos comentarios, el `static_assert` de la caja del sector y la limpieza por `privateFlags`).
* `python contrib/audit_hook_catalog.py --strict`: OK.
* `docs/hooks.csv`: +11 filas (`RwCore` x3, `RwPlCore` x1, `RpWorld` x7), `reversed=1`, `validated=0` (validadas por pares con el arnes, no por banco).

## 4. Humo en el juego

ASI Debug (`D:\Original\wv-rw5\bin\Debug\gta_reversed.asi`) con todos los ganchos activos, `AGUA_ASI` = ese ASI, archivo de argumentos de una linea con un espacio. Desde `wv-8\reversing\FnBIS1` (no se corrio `prueba_agua.py`, por indicacion del coordinador):

| Prueba | Resultado |
| --- | --- |
| `prueba_ropero.py` | `BIEN` (puerta 2877 y marcador). Captura `D:\Original\sim_out\ropero_rw5.png` revisada a ojo: CJ de espaldas con gorra verde, cadena y tatuaje del AK, habitacion, puerta de madera y tabla de surf bien texturizadas, sin deformaciones |
| `prueba_marcadores.py` | `BIEN` (12 marcadores en memoria, 12 cambian en 1 s) |
| `prueba_partida_nueva.py` | `BIEN` (sin errores en el registro, sin `Unknown param`, sin `Unreachable`) |

Un primer intento de marcadores/partida nueva dio `MAL` solo porque `gta_lock` lo tenia otra prueba (`jugar.ps1` no espera el candado); repetidos con el candado libre dan `BIEN`. El humo no ejercita las funciones de sectores ni de clump (el juego no lee mundos ni escribe clumps) y apenas las de texturas por defecto: su validacion es la del arnes.

## 5. Riesgos

* Las lecturas/escrituras de mundos (`RpWorldStreamRead`/`Write`) no las usa el juego en ejecucion (mapas por IPL/IMG): solo las valida el arnes.
* `_rwTextureDefaultRead`/`_rwTextureDefaultMipmapGeneration` solo se ejecutan cuando el modulo de texturas apunta a los callbacks por defecto (`RwTextureSetReadCallBack` no los cambia en el juego); con mipmapping activo la primera delega en `_rwTextureReadMipmapped`.
* El generador de tiras y los helpers que llama son C++ en el juego (sin banco): el arnes prueba la funcion nueva con los helpers ORIGINALES; los helpers C++ (`0x75A090` y siguientes) siguen sin validar por pares.
* `0x7F2930`: con NaN la carga del NaN resultante puede diferir del x87.
* No se ejecuto en el EXE independiente (fase 8): no estaba en el alcance.
