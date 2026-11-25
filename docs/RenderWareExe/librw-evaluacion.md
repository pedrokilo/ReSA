# librw frente al RenderWare propio: evaluación, oráculo y plan de integración

[Mapa de la región](README.md) · [Cruce por nombre](librw-brechas.md) · [Las 61 funciones frente a librw (CSV)](no-reconstruibles-librw.csv) · [Banco oráculo](../../contrib/oraculo_librw/)

Rama `agregar-librw` (24/11/2026). Alcance: **fase 1** (evaluar), **fase 2** (banco oráculo) y **fase 3** (integrar solo lo útil). Todo lo medido sale de la copia local `D:\Original\librw-test\librw`; no se descargó nada.

## Fase 1: evaluación

### Licencia, versión y dependencias

| Punto | Resultado | Evidencia |
| --- | --- | --- |
| Licencia | **MIT**, «Copyright (c) 2014 aap». Permite copiar y adaptar código con la nota de copyright y el texto de la licencia en las copias. | `D:\Original\librw-test\librw\LICENSE` |
| Versión | `project(librw VERSION 0.0.1)`. **El commit exacto no se puede fijar**: la copia local no es un repositorio git (sin `.git`). | `CMakeLists.txt` línea 7 |
| Qué trae | 39 623 líneas de C++ en `src/` (clump, geometry, skin, hanim, anim, matfx, uvanim, texture, raster, base con matrices/cuaterniones), backends `d3d` (D3D8/D3D9/Xbox), `gl`, `ps2` y plataforma `NULL`. Para PNG trae `lodepng` dentro del árbol. | `src/`, `src/lodepng` |
| Dependencias | Plataforma `NULL`: ninguna. D3D9: cabeceras de DirectX; GL3: OpenGL (y GLFW/GLEW para los ejemplos). Se genera con premake5 o CMake. Las dependencias de D3D9 y GL3 **no se comprobaron compilándolas**. | `CMakeLists.txt`, `README.md` |
| Compilación probada | `D:\Original\librw-test\build_null\src\librw.lib` (x86, Release, `/MD`, plataforma `NULL`): es la que usa el banco. | `build.bat` |
| Riesgo de choque de símbolos | librw vive en el espacio `rw::`, sin `extern "C"`; el choque con nuestros `Rw*` (globales, C) no existe. Sí define **macros** globales (`nil`, `nelem`) en `rwbase.h`: solo se debe incluir en unidades de compilación propias, nunca junto a `StdInc.h`. | `src/rwbase.h` líneas 74 y 78 |

**Pedido**: para fijar el commit y poder auditar una actualización hace falta `git ls-remote https://github.com/aap/librw HEAD` (o clonar esa URL en una carpeta nueva). Motivo: anotar el commit en el CMake y en la cabecera de atribución. No lo hice sin tu aviso.

### API relevante en librw

| Área | Dónde | Notas frente a lo nuestro |
| --- | --- | --- |
| Cuaterniones | `rwbase.h`/`base.cpp` (`Quat`, `slerp`, `lerp`, `Matrix::rotate(Quat)`) | `slerp` usa `acosf`/`sinf` y umbral 1e-5 rad; el del EXE usa `acos` y seno propios y cae a interpolación lineal con producto escalar ≥ 0,999 y niega el segundo cuaternión (no el primero). Las rotaciones coinciden salvo ~1e-4 (ver fase 2). |
| Matrices | `Matrix::mult` con atajo `IDENTITY` (0x20000) igual que el EXE; `RawMatrix` | `mult_` no usa la fila w. |
| HAnim | `hanim.cpp`: `HAnimHierarchy::updateMatrices`, `hanimApplyCB`, `hanimInterpCB` | Solo cubre los flags básicos (su propio `TODO: handle more (all!) cases`): no actualiza matrices de modelado ni LTMs, ni `NOMATRICES`, ni `SUBHIERARCHY`; el nuestro (`rphanim.cpp`) sí. No trae `BlendCB`, `AddCB` ni `MulRecipCB` (están en `nil`). |
| Skin | `skin.cpp`; `d3d/d3d9skin.cpp` (`uploadSkinMatrices`) | Para subir huesos pone `invMats[i].flags = 0` antes de multiplicar (evita el atajo de identidad con flags viejos). |
| Animación | `anim.cpp`, `rwanim.h` | `Animation`, `AnimInterpolator::setCurrentAnim`, `addTime`. |
| MatFX, UVAnim | `matfx.cpp`, `uvanim.cpp`, `d3d/d3d9matfx.cpp` | No es el MatFX de SA (ver `librw-brechas.md`). |
| Renderizador D3D9 («rwd3d9») | `d3d/d3d9render.cpp`, `d3d9.cpp`, `d3ddevice.cpp` | Otro motor de dispositivo; no expone las rutinas `_rwD3D9*` del RenderWare original. |

### Las 61 funciones no reconstruibles frente a librw

Detalle por función: [no-reconstruibles-librw.csv](no-reconstruibles-librw.csv) (generado por `contrib/oraculo_librw/clasificar_61.py`). Tamaños de `funciones.csv`.

| Clasificación | Funciones | Bytes | Qué son |
| --- | ---: | ---: | --- |
| Equivalente directo | **0** | 0 | Ninguna. |
| Equivalente con adaptación | **0** | 0 | Ninguna. |
| Sin equivalente | 13 | 8 202 | 10 rutinas internas de D3DX9 (3DNow!/MMX/XMM, 3 404 B; librw no incluye D3DX9) y 3 rutinas del dispositivo D3D9 de RenderWare (4 798 B, ya enganchadas en `WindowedMode.cpp`). |
| Irrelevante | 48 | 3 268 | 38 puentes de QuickTime/QTML (incluidas las 7 retiradas, 2 388 B), 6 ayudantes de carga retardada de DLL (120 B), 3 piezas de `RpSkin` que son subrutinas/epílogo de `_rpSkinGenericMatrixBlend` ya reconstruida (270 B) y 1 fragmento de `RtAnimAnimationRead` ya reconstruida (490 B). |

**Conclusión honesta**: librw no cubre ninguna de las 61. Son ABI de registros, instrucciones 3DNow!, fragmentos y puentes ajenos a RenderWare; la causa que las hace «no reconstruibles» (no expresables en C++ sin ensamblador) no desaparece con librw. Tampoco «dependen de bytes del EXE» en el sentido útil: las de QuickTime/DirectDraw ya no las llama nadie y las demás están cubiertas por la función completa que las contiene o por D3DX9 enlazado. Lo que sí podría cubrir librw son las funciones del SDK **ya reconstruidas** (795) como segunda fuente de contraste, que es lo que hace el banco.

### Cómo se integraría (no hecho; no hay función que lo justifique)

- Biblioteca estática compilada por CMake detrás de `option(GTASA_WITH_LIBRW "..." OFF)`: `add_subdirectory(libs/librw)` (o `FetchContent` con el commit fijado) con `LIBRW_PLATFORM=NULL`, `LIBRW_TOOLS=OFF`, `LIBRW_INSTALL=OFF`, runtime del mismo tipo que el resto (`/MT` o `/MD`).
- Espacio de nombres: ya es `rw::`; basta enlazar y llamar con `rw::`. Prohibido incluir `rw.h` en archivos que incluyan `StdInc.h` (macros `nil`, `nelem`).
- Con la opción apagada no cambia nada: el CI y la compilación por defecto no la ven. **No toqué ningún `CMakeLists.txt`** (hay otro agente reparando el CI); el hunk mínimo sería una línea `option(...)` y un `if()` que añada el subdirectorio.

## Fase 2: librw como oráculo

Banco independiente, sin ejecutar el juego: `contrib/oraculo_librw/`.

- `extraer.py` extrae **sin copiar al repositorio** el código bajo prueba directamente de `X87Math.cpp` (lote S11A: `HAnimKeyFrame{Interpolate,Blend,Apply,Add,MulRecip}Cpp`) y de `rphanim.cpp` (`RpHAnimHierarchyUpdateMatrices` con sus ayudantes).
- `nuestro.cpp` lo compila con las cabeceras reales del proyecto (`rwcore.h`, `rpworld.h`, `rphanim.h`) y sustituye solo las dependencias externas (`RwMatrixMultiply` con el atajo de identidad del EXE, `RwFrame*`).
- `banco.cpp` lo compara con librw (`librw.lib`, plataforma `NULL`) y hace la prueba de pila: cada llamada va precedida de un relleno de 12 KB de pila con `00`, `CC`, `FF`, `5A` y `A5`; si la salida (con relleno entre medias) no es idéntica bit a bit, la rutina lee algo sin inicializar. Control: una función con una lectura sin inicializar a propósito **sí** se detecta (60 diferencias), así que el método funciona con `/O2` y con `/Od`.
- Compilar y ejecutar: `D:\Original\librw-test\oraculo\compilar_banco.cmd O2` (o `Od`) y `banco_O2.exe`. Los scripts de compilación viven fuera del repositorio (rutas locales); el código fuente del banco sí está versionado.

### Resultados (24/11/2026; `O2` y `Od` dan lo mismo)

| Prueba | Casos | Diferencias | Máx. \|d\| | Lectura |
| --- | ---: | ---: | ---: | --- |
| Interpolación, cuaterniones unitarios (cualquier signo de producto) | 20 000 | 0 | 7,7e-7 | coincide con librw |
| Casi iguales (producto > 0,999: nuestro lineal, librw `slerp`) | 4 000 | 19 > 1e-4 | 1,5e-4 | diferencia de algoritmo, no error |
| Casi opuestos (producto < −0,999) | 4 000 | 13 > 1e-4 | 1,4e-4 | ídem |
| **No normalizados** (norma 0,2 a 3) | 4 000 | 1 451 | 14,2 | esperado: el EXE y librw tratan distinto un cuaternión no unitario (el nuestro es fiel al EXE) |
| **Tiempos de clave iguales** (t2 == t1) | 1 000 | 0 | — | **ambos producen NaN** en todos los casos (0/0); librw lo evita con un `assert` en depuración |
| Tiempo fuera de [t1, t2] | 1 000 | 0 | 2,5e-5 | coincide |
| Un cuaternión cero / dos / un solo fotograma | 1 500 | 0 | 3e-7 | coincide, sin NaN |
| Cuaternión a matriz (unitarios / no unitarios) | 25 000 | 0 | 6e-8 / 2e-7 | coincide |
| `RpHAnimHierarchyUpdateMatrices` frente a `updateMatrices` (jerarquías aleatorias de 2 a 63 nodos con push/pop) | 2 000 | 0 | 3,8e-6 | coincide |
| Prueba de pila (5 rellenos) de las 5 rutinas de fotograma y de `UpdateMatrices` | 59 170 + 300 | **0** | — | **ninguna lee pila sin inicializar** |

### Qué dice sobre la basura 0xCCCCCCCC (honesto)

- **Nuestras rutinas de interpolación, mezcla, aplicación a matriz y `RpHAnimHierarchyUpdateMatrices` NO producen 0xCC ni dependen de la pila**: salida idéntica con cualquier relleno, y coinciden con librw dentro de tolerancia. Por tanto la causa **no está en estas rutinas**.
- La evidencia del agente `skin` (informe por mensaje, el log vivo se sobrescribió y no hay copia) es un keyframe del nodo 6 con `q = (CC…, 0,0132, 0,0044, 0,9999)`, `t.x` y `t.z` en CC, y el nodo 7 con `q.y` en CC: **componentes sueltas** en CC con las demás plausibles. Eso no es un keyframe sin inicializar entero: parece una pose válida a la que **otro código sobrescribió componentes** (o memoria reutilizada). La hipótesis queda en el escritor del keyframe.
- Escritores de `KeyFrame->q/t` identificados en el código (no probados como culpables): `FrameUpdateCallBack*` de `RpAnimBlend.cpp` (escriben `q` entero y `t` entero), `CPedIK` (`RotateTorso`, `RotateTorsoForArm`: `RtQuatRotate` sobre el cuaternión del hueso), `CTaskSimpleCarDrive` (cabeceo, estado 50 = conduciendo, coincide con la observación), `CTaskSimpleJetPack`, `IKChain`. `RtQuatRotate` con `REPLACE` escribe `w`, luego `imag` por separado, luego escala: una interrupción o un cuaternión de entrada con CC lo propaga.
- Nota del agente `skin`: los objetos de `wv-int\build` (CMake Debug) se compilan con `/RTC1`, que rellena locales con CC; sus `.obj` de `over\` no. Un local `float`/`CQuaternion` sin inicializar en cualquier ruta de la ASI que acabe en un keyframe daría justo este patrón.
- **Siguiente paso recomendado (no hecho)**: con el campo `keyframe_ptr` que ya añade la vigilancia, poner un guardián de escritura (`VirtualProtect` con `PAGE_GUARD` sobre la página del keyframe o un punto de interrupción de datos) en los keyframes de los nodos 6 y 7 del jugador y registrar el `EIP` del escritor cuando aparezca un valor 0xCCCCCCCC. El banco no puede decir más sin entrada real.

### Lo que el banco NO cubre

- Las funciones de `RpAnimBlend`/`CAnimBlendNode` (bloques de animación comprimidos de GTA, `CalcTheta`, `FrameUpdateCallBack*`): no están en librw y dependen de la clase del juego; no se probaron. Quedan para ampliar el banco con un stub de `CAnimBlendNode`.
- La subida de matrices de hueso del D3D9 (`_rpSkinD3D9UpdateBoneConstants`): leída frente a `uploadSkinMatrices` de librw (misma fórmula: `inversa·hueso·inversaMundo`, transpuesta), pero **no ejecutada**; librw pone `invMats[i].flags = 0` antes de multiplicar y el EXE no.
- `RwMatrixMultiply`/`RwMatrixInvert`/`RwMatrixOptimize` reales del proyecto: en el banco `RwMatrixMultiply` es un sustituto con la misma semántica documentada (no el código del EXE con su callback de módulo).
- Ejecución real del juego: no se lanzó (no se tocó `gta_lock`).

## Fase 3: integración

**Nada que integrar en el código del juego**: ninguna de las 61 tiene equivalente en librw y la fase 2 no encontró un defecto que corregir en nuestro C++. No se modificó código común (no hubo ASI ni humo), ni `CMakeLists.txt`, ni se copió código de librw: **no hay atribución pendiente**. Si más adelante se copia código de librw, debe llevar «Copyright (c) 2014 aap, licencia MIT» y el texto de la licencia en la cabecera del archivo.

Archivos nuevos: `docs/RenderWareExe/librw-evaluacion.md`, `docs/RenderWareExe/no-reconstruibles-librw.csv`, `contrib/oraculo_librw/*`.
