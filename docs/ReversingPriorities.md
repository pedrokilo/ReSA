<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 18/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11695 con evidencia histórica y 746 pendientes de validación en 32 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 18/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.441 |
| Reconstruidas | 12.441 |
| Validadas | 11.695 |
| Reconstruidas sin validar | 746 |
| De ellas, en esta cola (validables por pares) | 746 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 4 | 8 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 0 | 0 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 21 | 697 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 7 | 41 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **32** | **746** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (4 clases, 8 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CAudioZones | 9 | 1 |  | 31 |
| 2 | IKChainManager_c | 19 | 2 |  | 20 |
| 3 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 4 | LibJpegError | 3 | 3 |  | 26 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (0 clases, 0 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (21 clases, 697 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CWaterLevel | 49 | 1 |  | 42 |
| 2 | RtSlerp | 1 | 1 |  | ? |
| 3 | RtBMP | 2 | 2 |  | ? |
| 4 | DelayLoadThunks | 3 | 3 |  | ? |
| 5 | QTMLRuntime | 6 | 6 |  | ? |
| 6 | RpHAnim | 7 | 6 |  | ? |
| 7 | RtQuat | 6 | 6 |  | ? |
| 8 | RtAnim | 10 | 7 |  | 48 |
| 9 | RtDict | 12 | 10 |  | ? |
| 10 | D3DX9 | 11 | 11 |  | ? |
| 11 | RpUVAnim | 16 | 14 |  | ? |
| 12 | ZlibInflate | 14 | 14 |  | ? |
| 13 | ExeLibJpeg | 15 | 15 |  | ? |
| 14 | ImportThunks | 17 | 17 |  | ? |
| 15 | LibJpegMemory | 21 | 21 |  | 26 |
| 16 | RpMatFX | 28 | 28 |  | ? |
| 17 | RpSkin | 34 | 33 |  | ? |
| 18 | RpWorld | 197 | 94 |  | ? |
| 19 | RwD3D9 | 115 | 115 |  | ? |
| 20 | RwPlCore | 132 | 118 |  | ? |
| 21 | RwCore | 211 | 175 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (7 clases, 41 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | ExeProtectionStub | 8 | 1 |  | 57 |
| 2 | LibJpegCompressApi | 3 | 3 |  | 51 |
| 3 | LibJpegDecompressUpsample | 3 | 3 |  | 65 |
| 4 | LibJpegDecompressApi | 4 | 4 |  | 73 |
| 5 | CRunningScript | 43 | 4 |  | 77 |
| 6 | D3D9RasterFormat | 10 | 4 |  | 88 |
| 7 | LibJpegDecompress | 22 | 22 |  | 51 |

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
