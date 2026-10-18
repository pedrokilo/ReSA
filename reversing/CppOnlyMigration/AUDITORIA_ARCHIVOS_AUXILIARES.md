<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría de archivos auxiliares de las conversiones a C++

11/10/2026. Lectura de los archivos `*Cpp.*` de `source/game_sa` creados al convertir ensamblador. El criterio pedido: **dos archivos por clase (`.cpp` y `.h`)**, sin código duplicado. Las conversiones anteriores no lo cumplen.

La tabla «Estado tras la limpieza» dice qué quedó corregido. Los hallazgos originales se conservan debajo como registro. Los bancos aprobados de cada conversión hashean estas rutas, así que mover un archivo obliga a repetir sus bancos.

## Por qué existen

El proyecto compila con `/Od /ZI /RTC1` y SSE, y usa PCH. Para obtener el x87 exacto del EXE (sin guardados intermedios de `float`) hace falta `/O2 /arch:IA32 /fp:fast` sin PCH. Se comprobó que `#pragma optimize` dentro del mismo archivo no sirve con `/ZI` y `/RTC1` (deja código Debug con relleno `0xCC` y `__CIsqrt`). Las conversiones anteriores resolvieron esto creando un archivo aparte por conversión.

## Estado tras la limpieza (11/10/2026)

| Hallazgo | Estado |
| --- | --- |
| Funciones idénticas en `CloudsMathCpp` y `TrigSimpleCpp` | **Corregido.** Los tres helpers de Clouds eran idénticos byte a byte y se eliminaron; `Clouds.cpp` usa los de `X87Math`. |
| `CloudsWeatherCpp.h` (10 globales duplicados de `Weather.h`) | **Corregido.** Eliminado; `Clouds.cpp` usa `CWeather::` (mismas direcciones y tipos). |
| `CloudsAccessCpp.cpp` (4 envoltorios) | **Corregido.** Eliminado; las llamadas son directas. |
| Declaraciones de `sin`/`cos`/`sqrt` repetidas en seis archivos | **Corregido.** Una sola vez en `X87Math.cpp`. |
| `Clouds`, `CShadows` y los módulos trigonométricos con varios archivos auxiliares | **Corregido.** `TrigSimpleCpp`, `TrigCompound6Cpp`, `CloudsMathCpp`, `ShadowSqrtCpp` y `ShadowMathCpp` se fundieron en `X87Math.cpp/.h`, un único módulo compartido. |
| `IntegerMinimalCpp.cpp` con métodos de dos clases | **Corregido.** `SetUseD3DResourceBuffering` volvió a `D3DResourceSystem.cpp` (compilado completo con `/O2`) y `Native_00424100` a `CarCtrl.cpp`, optimizada por `#pragma`; se eliminó `NativeStubs.cpp`. Revalidado con 11 bancos (ver [REVALIDACION_INTERIOR_CARCTRL.md](REVALIDACION_INTERIOR_CARCTRL.md)). |
| `Interior_c` con 4 archivos y un prólogo de includes copiado | **Corregido.** `InteriorCpp.cpp` e `InteriorShelfCpp.cpp` se fundieron en `Interior_c.cpp` (compilado con `/Zi`, sin `/O2` global; sólo `Shop_AddShelfInfo` optimizada por pragma). Revalidado con 11 bancos, todos aprobados. |

Archivos eliminados: `InteriorCpp.cpp`, `InteriorShelfCpp.cpp`, `NativeStubs.cpp`, `TrigSimpleCpp.cpp/.h`, `TrigCompound6Cpp.cpp/.h`, `CloudsMathCpp.cpp/.h`, `CloudsAccessCpp.cpp/.h`, `CloudsWeatherCpp.h`, `ShadowSqrtCpp.cpp/.h`, `ShadowMathCpp.cpp/.h`, `IntegerMinimalCpp.cpp`, `ConversationMathCpp.cpp/.h` y `ConversationNodeCpp.cpp`. Archivo nuevo: `X87Math.cpp/.h` (módulo compartido).

Revalidación: 16 bancos repetidos con cero diferencias (detalle en [README.md](README.md), sección «Consolidación sin archivos auxiliares duplicados»).

## Hallazgos originales



### Duplicados confirmados

| Hallazgo | Detalle |
| --- | --- |
| Funciones idénticas | `CloudsSinX87`, `CloudsCosX87` y `CloudsSinMulX87` (`CloudsMathCpp.cpp`) hacen lo mismo que `TrigSimpleSinCpp`, `TrigSimpleCosCpp` y `TrigSimpleSinMulCpp` (`TrigSimpleCpp.cpp`): `float(sin(double(angle)) [* double(k)])`. |
| Globales duplicados | `CloudsWeatherCpp.h` redeclara 10 variables de `Weather.h` con las mismas direcciones (`CloudCoverage`, `Foggyness`, `Wind`, `WindDir`…) «para evitar su grafo de includes». |
| Envoltorios sin lógica | `CloudsAccessCpp.cpp`: cuatro funciones de una línea que sólo llaman a `FindPlayerPed`, `FindPlayerVehicle`, `FindPlayerCoors` y `g_fx` para esquivar includes. |
| Prólogo copiado | `InteriorCpp.cpp` e `InteriorShelfCpp.cpp` repiten el mismo bloque de 15 líneas de includes. `Clouds.cpp` lleva otra lista copiada de unas 60 líneas. |
| Declaraciones repetidas | `extern "C" double __cdecl sin/cos/sqrt` + `#pragma intrinsic` se copia en seis archivos (`CloudsMathCpp`, `TrigSimpleCpp`, `TrigCompound6Cpp`, `ShadowSqrtCpp`, `ShadowMathCpp` y el anterior `ConversationMathCpp`). |

## Clases partidas en más de dos archivos

| Clase / módulo | Archivos hoy | Observación |
| --- | --- | --- |
| `Interior_c` | `Interior_c.cpp/.h` + `InteriorCpp.cpp` + `InteriorShelfCpp.cpp` | Métodos de la clase definidos en otros dos `.cpp`. |
| `CClouds` | `Clouds.cpp/.h` + `CloudsMathCpp.cpp/.h` + `CloudsAccessCpp.cpp/.h` + `CloudsWeatherCpp.h` | 7 archivos. |
| `CShadows` | `Shadows.cpp/.h` + `ShadowSqrtCpp.cpp/.h` + `ShadowMathCpp.cpp/.h` | 6 archivos; los dos helpers son de una función cada uno. |
| `D3DResourceSystem` y `CCarCtrl` | `IntegerMinimalCpp.cpp` | Un método de cada clase en un tercer archivo; sus `RH_ScopedInstall` siguen en `D3DResourceSystem.cpp` y `CarCtrl.cpp`. |
| Helpers trigonométricos | `TrigSimpleCpp.*` (7 consumidores) y `TrigCompound6Cpp.*` (4 consumidores) | Módulos compartidos entre clases distintas (`Birds`, `BrightLights`, `PlaneTrails`, `3dMarkers` y cuatro tareas). No pertenecen a una clase. |

## Ya corregido en este lote

`ConversationNode` y `ConversationForPed` quedaron en `.cpp` y `.h` por clase. Para lograrlo se compila el archivo completo con las opciones de `/O2` e incluye `StdInc.h` directamente, tras dos arreglos de cabecera que eran errores reales ocultos por el PCH: `PoolOf<T>` de `Pools.h` y ocho funciones de `CompressedVector.h` se definían sin `inline`.

## Propuesta de limpieza original (aplicada completa)

Con `StdInc.h` utilizable sin PCH, las listas de includes copiadas pueden sustituirse por una línea, y estos archivos pueden volver a su clase cuando ella no tenga otras funciones validadas que cambiar de opciones. En orden de menor a mayor coste:

1. **Duplicados de seno/coseno:** que `Clouds.cpp` use `TrigSimpleCpp` y eliminar las tres funciones de `CloudsMathCpp`. Requiere repetir los bancos de Clouds (CppCloudsMath3 y CppCloudsRegression20).
2. **`IntegerMinimalCpp.cpp`:** devolver cada método a `D3DResourceSystem.cpp` y `CarCtrl.cpp` (archivos pequeños; el de `D3DResourceSystem` ya tiene el hook). Repetir CppIntegerWorld4 y CppIntegerWorld10.
3. **`CloudsAccessCpp` y `CloudsWeatherCpp.h`:** eliminar al usar `StdInc.h`.
4. **`ShadowSqrtCpp` y `ShadowMathCpp`:** fundirlos en uno, o llevarlos a `Shadows.cpp` si todo `Shadows.cpp` se pasa a `/O2` (esto último cambiaría funciones validadas de la clase y exige revalidarlas).
5. **`Interior_c`:** fundir los dos `.cpp` en `Interior_c.cpp` con las opciones de `/O2`, con revalidación de toda la clase.
6. **Módulo trigonométrico compartido:** fundir `TrigSimpleCpp` y `TrigCompound6Cpp` en un único par `.cpp/.h` de helpers x87, que sí es un módulo común y no una clase.

Cada paso exige repetir los bancos afectados dentro del juego y actualizar `approvals.json` y el inventario.
