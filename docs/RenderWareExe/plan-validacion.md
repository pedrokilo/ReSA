# Plan de validación de RenderWare y D3D (0x750000–0x81FFFF)

[Mapa de la región](README.md) · [Pendientes por clase](pendientes-por-clase.md) · [Plan de reconstrucción](plan-lotes.md) · [Informe de progreso](../ReversedClasses.md)

> **Estado:** plan aprobado el 18/11/2026, sin ningún lote lanzado. No empieza hasta que cierren las oleadas V5 y V6 del código del juego.

La reconstrucción de esta zona está hecha (724 de 724). Falta validarla contra el EXE dentro del juego, con el mismo método que el resto del proyecto: banco par a par (≥16.384 pares por función, 0 diferencias), oráculo Python independiente, prueba negativa de 2 rondas y registro con `register_hook.py`.

## 1. Qué falta

Al 18/11/2026 hay **663 entradas sin validar** en 822 del catálogo de RenderWare.

| Módulo | Sin validar | Qué necesita el banco |
| --- | ---: | --- |
| RwCore | 175 | Mezcla: matrices, frames y listas puras; unas 60 de vertex buffer y estado D3D |
| RwPlCore | 118 | Flujos (`RwStream`), heap de arena, errores |
| RwD3D9 | 115 | Dispositivo D3D falso |
| RpWorld | 94 | Memoria y flujos falsos (ya resuelto en 227A–229A) |
| RpSkin | 33 | Sobre todo puro, más flujos |
| RpMatFX | 28 | Sobre todo puro, más flujos |
| ImportThunks | 17 | Saltos a DLLs externas |
| RpUVAnim | 14 | Puro y flujos |
| ZlibInflate | 14 | Puro (datos de entrada y salida) |
| D3DX9 | 11 | 4 de 3DNow! y 7 de compilación de shaders |
| RtDict | 10 | Puro y flujos |
| RtAnim | 7 | Puro y flujos |
| RpHAnim, RtQuat, QTMLRuntime | 18 | Puro (RpHAnim, RtQuat) y carga de QuickTime |
| DelayLoadThunks | 3 | Saltos a DLLs externas |
| RtBMP, RtSlerp | 3 | Puro |

## 2. Decisiones tomadas

1. **Thunks de DLLs externas (ImportThunks 17, DelayLoadThunks 3, QTMLRuntime 6):** no hay lógica que comparar. Se validan por inspección: se comprueba en el EXE y en la DLL que el salto o la llamada apunta a la dirección correcta, y se guarda esa evidencia. Falta definir el formato del registro de esta evidencia antes de lanzarlos.
2. **Funciones 3DNow! (4 de D3DX9):** los procesadores actuales no las ejecutan, así que ni el EXE ni la DLL pueden correrlas. Se excluyen con esa evidencia, como las demás no reconstruibles.
3. **Compilación de shaders de D3DX9 (7):** se dejan fuera de la validación par a par por ahora. Cada una arrastra miles de líneas de D3DX9 que ya se enlazan del original, y el juego casi no las usa. Quedan con `validated=0` y una nota; se retoman si se vuelven críticas.
4. **Orden:** primero las funciones puras, después flujos y memoria, y al final el dispositivo D3D. Así el método se prueba sin riesgo y un error del dispositivo falso no invalida lotes enteros.

Con estas decisiones quedan **unas 620 entradas validables**: las 663 menos las 4 de 3DNow! y las 7 de shaders, con los 26 thunks validados por inspección.

## 3. Infraestructura común

Los bancos de `rpworld` (227A, 228A y 229A) ya resolvieron dos piezas; la tercera es nueva.

| Pieza | Estado | Qué hay que hacer |
| --- | --- | --- |
| Memoria y listas libres falsas | Existe | Los falsos de `malloc`, `realloc`, `free` y las listas libres se instalan en el propio motor (+0x134, +0x138, +0x13C, +0x144, +0x148). Extraerlos a una cabecera común |
| Flujos falsos | Parcial | Hoy es una tira de palabras. Generalizar a `RwStream` en memoria y con callbacks propios, con fallos programables |
| Dispositivo D3D falso | Nueva | Un `IDirect3DDevice9` falso con tabla virtual propia que registra cada llamada (método y argumentos) y devuelve valores programables, incluidos fallos. Se inyecta en el puntero del dispositivo del motor; EXE y DLL lo ven igual. Se comparan el registro de llamadas y el estado interno, no los píxeles |

La cabecera común se prueba primero en un lote pequeño (el primero de la oleada 1) antes de usarla a escala.

## 4. Oleadas

| Oleada | Contenido | Entradas aprox. | Necesita |
| --- | --- | ---: | --- |
| 1 · Puras | RtQuat, RtSlerp, RtBMP, RtAnim, RtDict, RpUVAnim, RpHAnim, RpMatFX, RpSkin y la parte pura de RwCore (matrices, frames, listas) | 150 | Memoria falsa |
| 2 · Flujos y memoria | RwPlCore (flujos, heap de arena, errores), RpWorld y las lecturas y escrituras por flujo de los plugins | 250 | Memoria y flujos falsos |
| 3 · Dispositivo D3D | RwD3D9 y la parte D3D de RwCore (vertex buffers, estado de render) | 175 | Dispositivo D3D falso |
| 4 · Especiales | ZlibInflate (bancos puros) y los thunks por inspección | 40 | Datos de entrada y salida; inspección de bytes |

Con unas 25 entradas por lote salen unos 25 lotes más el de infraestructura. Con dos validaciones a la vez y una sola sesión de juego, la estimación es de unas 15 horas de reloj, sin contar caídas.

## 5. Reglas

- Un lote por banco `FnNNNA`, en el mismo flujo que el resto: worktree propio, candados `compile_lock` y `gta_lock`, evidencia archivada en `reversing/CppTwelveLevel6Final/runs`.
- Máximo dos validaciones a la vez y una sola sesión de juego.
- Los lotes de esta zona se numeran aparte (R1A en adelante) y se anotan en `reversing/PendienteValidar/en_curso.txt` mientras corren.
- Si una función resulta inviable en C++ puro, se documenta con evidencia y se excluye sin hook.
- Defectos que el banco encuentre en la fuente: se corrigen en esa misma validación, como en el resto del proyecto.

## 6. Pendiente antes de empezar

- Cerrar V5 y V6 (código del juego: 232A–239A).
- Diseñar y probar la cabecera común (memoria, flujos y dispositivo falsos).
- Definir cómo se registra la validación por inspección de los thunks.
- Generar los informes de lote R1A en adelante a partir de `hooks.csv`, con el reparto de la sección 4.
