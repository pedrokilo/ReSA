# Auditoría de archivos auxiliares de las conversiones a C++

11/10/2026. Lectura de los archivos `*Cpp.*` de `source/game_sa` creados al convertir ensamblador. El criterio pedido: **dos archivos por clase (`.cpp` y `.h`)**, sin código duplicado. Las conversiones anteriores no lo cumplen.

Este documento describe hallazgos; no cambia código ni evidencia. Los bancos aprobados de cada conversión hashean estas rutas, así que moverlas exige repetir esos bancos.

## Por qué existen

El proyecto compila con `/Od /ZI /RTC1` y SSE, y usa PCH. Para obtener el x87 exacto del EXE (sin guardados intermedios de `float`) hace falta `/O2 /arch:IA32 /fp:fast` sin PCH. Se comprobó que `#pragma optimize` dentro del mismo archivo no sirve con `/ZI` y `/RTC1` (deja código Debug con relleno `0xCC` y `__CIsqrt`). Las conversiones anteriores resolvieron esto creando un archivo aparte por conversión.

## Duplicados confirmados

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

## Propuesta de limpieza (no aplicada)

Con `StdInc.h` utilizable sin PCH, las listas de includes copiadas pueden sustituirse por una línea, y estos archivos pueden volver a su clase cuando ella no tenga otras funciones validadas que cambiar de opciones. En orden de menor a mayor coste:

1. **Duplicados de seno/coseno:** que `Clouds.cpp` use `TrigSimpleCpp` y eliminar las tres funciones de `CloudsMathCpp`. Requiere repetir los bancos de Clouds (CppCloudsMath3 y CppCloudsRegression20).
2. **`IntegerMinimalCpp.cpp`:** devolver cada método a `D3DResourceSystem.cpp` y `CarCtrl.cpp` (archivos pequeños; el de `D3DResourceSystem` ya tiene el hook). Repetir CppIntegerWorld4 y CppIntegerWorld10.
3. **`CloudsAccessCpp` y `CloudsWeatherCpp.h`:** eliminar al usar `StdInc.h`.
4. **`ShadowSqrtCpp` y `ShadowMathCpp`:** fundirlos en uno, o llevarlos a `Shadows.cpp` si todo `Shadows.cpp` se pasa a `/O2` (esto último cambiaría funciones validadas de la clase y exige revalidarlas).
5. **`Interior_c`:** fundir los dos `.cpp` en `Interior_c.cpp` con las opciones de `/O2`, con revalidación de toda la clase.
6. **Módulo trigonométrico compartido:** fundir `TrigSimpleCpp` y `TrigCompound6Cpp` en un único par `.cpp/.h` de helpers x87, que sí es un módulo común y no una clase.

Cada paso exige repetir los bancos afectados dentro del juego y actualizar `approvals.json` y el inventario.
