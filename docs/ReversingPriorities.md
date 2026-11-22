# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 22/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 13.497 |
| Reconstruidas | 13.497 |
| Validadas | 11.792 |
| Reconstruidas sin validar | 1.705 |
| De ellas, en esta cola (validables por pares) | 1.705 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 16 | 25 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 0 | 0 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 32 | 1441 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 6 | 239 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **54** | **1705** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (16 clases, 25 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CBaseModelInfo | 38 | 1 |  | 9 |
| 2 | CTaskSimpleTurn180 | 8 | 1 |  | 11 |
| 3 | CQuaternion | 13 | 1 |  | 15 |
| 4 | CPad | 103 | 1 |  | 16 |
| 5 | tUserList | 3 | 1 |  | 16 |
| 6 | CSprite2d | 36 | 1 |  | 17 |
| 7 | CScripted2dEffects | 7 | 1 |  | 21 |
| 8 | CGarages | 31 | 1 |  | 23 |
| 9 | CTaskSimpleArrestPed | 9 | 1 |  | 23 |
| 10 | LibJpegMemory | 22 | 1 |  | 25 |
| 11 | FxInfoAttractLine_c | 4 | 1 |  | 30 |
| 12 | CTaskSimpleAnim | 5 | 1 |  | 34 |
| 13 | FxInterpInfoFloat_c | 5 | 1 |  | 35 |
| 14 | FxManager_c | 25 | 2 |  | 19 |
| 15 | CAEMFDecoder | 13 | 5 |  | 16 |
| 16 | CAudioZones | 14 | 5 |  | 27 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (0 clases, 0 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (32 clases, 1441 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CGame | 26 | 1 |  | 37 |
| 2 | CPed | 157 | 1 |  | 37 |
| 3 | CCamera | 127 | 1 |  | 38 |
| 4 | CSpecialFX | 8 | 1 |  | 38 |
| 5 | CPhoneInfo | 3 | 1 |  | 40 |
| 6 | CCollisionPlugin | 3 | 1 |  | 45 |
| 7 | CVehicle | 204 | 1 |  | 47 |
| 8 | RtSlerp | 1 | 1 |  | ? |
| 9 | StdException | 3 | 3 |  | 40 |
| 10 | DelayLoadThunks | 3 | 3 |  | ? |
| 11 | Input | 14 | 3 |  | ? |
| 12 | RwHelper | 49 | 3 |  | ? |
| 13 | RtBMP | 4 | 4 |  | ? |
| 14 | RtPNG | 7 | 7 |  | ? |
| 15 | RwMathX87 | 7 | 7 |  | ? |
| 16 | QTMLRuntime | 8 | 8 |  | ? |
| 17 | RtQuat | 8 | 8 |  | ? |
| 18 | ZlibInflate | 14 | 14 |  | ? |
| 19 | RtDict | 18 | 16 |  | ? |
| 20 | D3DX9 | 17 | 17 |  | ? |
| 21 | ImportThunks | 17 | 17 |  | ? |
| 22 | ZlibDeflate | 25 | 25 |  | ? |
| 23 | RtAnim | 30 | 27 |  | 27 |
| 24 | RpHAnim | 35 | 34 |  | ? |
| 25 | RpUVAnim | 42 | 40 |  | ? |
| 26 | RpMatFX | 73 | 73 |  | ? |
| 27 | RpSkin | 76 | 75 |  | ? |
| 28 | LibPngWrite | 78 | 78 |  | ? |
| 29 | RwD3D9 | 147 | 147 |  | ? |
| 30 | RwPlCore | 191 | 177 |  | ? |
| 31 | RwCore | 339 | 303 |  | ? |
| 32 | RpWorld | 447 | 344 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (6 clases, 239 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CLines | 1 | 1 |  | 107 |
| 2 | RwD3D9RasterDriver | 16 | 2 |  | 52 |
| 3 | CPhysical | 65 | 3 |  | 79 |
| 4 | D3D9RasterStandards | 12 | 12 |  | 71 |
| 5 | CRunningScript | 66 | 23 |  | 51 |
| 6 | ExeCrt | 198 | 198 |  | 0 |

<a id="fuera-de-la-cola"></a>

## Fuera de la cola (no validables por pares)

| Clase | Función | Motivo |
|---|---|---|

Se comprueban de forma indirecta arrancando el juego o con una prueba en partida.

Estaban en esta lista y ya se validaron después con bancos de dobles: `CAEAudioHardware::Initialise`, `CPlayerPed::SetupPlayerPed`, `Win::NOTSA_WinMain`, `CCollision::SphereVsEntity`.

<a id="como-se-ordena"></a>

## Cómo se ordena

Los niveles salen de una regla fija, no de una opinión:

- **Nivel 1:** hasta 12 funciones por validar y hasta 35 líneas por función.
- **Nivel 2:** más de 12 funciones y hasta 25 líneas por función.
- **Nivel 4:** 90 o más funciones, o más de 50 líneas por función.
- **Nivel 3:** el resto.

"Líneas/func." es una **estimación**: líneas del `.cpp` que más define la clase, repartidas entre las clases que define ese fichero y divididas por las funciones reconstruidas de la clase. No es un coste medido y no promete horas ni fechas; `?` indica que no se pudo medir. Dentro de cada nivel el orden es por funciones por validar y después por líneas por función.

Para regenerar este documento: `python contrib/validation_queue.py`, o `python contrib/sync_docs.py`, que además sincroniza el resto de Markdown derivado de `hooks.csv`.
