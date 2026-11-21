<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 21/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 13.311 |
| Reconstruidas | 13.311 |
| Validadas | 11.801 |
| Reconstruidas sin validar | 1.510 |
| De ellas, en esta cola (validables por pares) | 1.510 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 8 | 15 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 0 | 0 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 28 | 1354 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 3 | 141 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **39** | **1510** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (8 clases, 15 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CQuaternion | 13 | 1 |  | 15 |
| 2 | CPad | 103 | 1 |  | 16 |
| 3 | CSprite2d | 36 | 1 |  | 17 |
| 4 | CGarages | 31 | 1 |  | 23 |
| 5 | FxInfoAttractLine_c | 4 | 1 |  | 30 |
| 6 | FxInterpInfoFloat_c | 5 | 1 |  | 35 |
| 7 | CAudioZones | 13 | 4 |  | 28 |
| 8 | CAEMFDecoder | 13 | 5 |  | 16 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (0 clases, 0 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (28 clases, 1354 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CGame | 26 | 1 |  | 37 |
| 2 | CPed | 157 | 1 |  | 37 |
| 3 | CCamera | 127 | 1 |  | 38 |
| 4 | CCollisionPlugin | 3 | 1 |  | 45 |
| 5 | CVehicle | 204 | 1 |  | 47 |
| 6 | RtSlerp | 1 | 1 |  | ? |
| 7 | StdException | 3 | 3 |  | 39 |
| 8 | DelayLoadThunks | 3 | 3 |  | ? |
| 9 | Input | 14 | 3 |  | ? |
| 10 | RwHelper | 49 | 3 |  | ? |
| 11 | RtBMP | 4 | 4 |  | ? |
| 12 | RtPNG | 7 | 7 |  | ? |
| 13 | RtQuat | 8 | 8 |  | ? |
| 14 | D3DX9 | 17 | 11 |  | ? |
| 15 | ZlibInflate | 14 | 14 |  | ? |
| 16 | RtDict | 18 | 16 |  | ? |
| 17 | ImportThunks | 17 | 17 |  | ? |
| 18 | ZlibDeflate | 25 | 25 |  | ? |
| 19 | RtAnim | 30 | 27 |  | 27 |
| 20 | RpHAnim | 35 | 34 |  | ? |
| 21 | RpUVAnim | 42 | 40 |  | ? |
| 22 | RpSkin | 59 | 58 |  | ? |
| 23 | RpMatFX | 73 | 73 |  | ? |
| 24 | LibPngWrite | 78 | 78 |  | ? |
| 25 | RwD3D9 | 143 | 143 |  | ? |
| 26 | RwPlCore | 191 | 177 |  | ? |
| 27 | RpWorld | 404 | 301 |  | ? |
| 28 | RwCore | 339 | 303 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (3 clases, 141 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CLines | 1 | 1 |  | 107 |
| 2 | CPhysical | 65 | 3 |  | 79 |
| 3 | ExeCrt | 137 | 137 |  | 0 |

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
