# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 17/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.441 |
| Reconstruidas | 12.441 |
| Validadas | 11.531 |
| Reconstruidas sin validar | 910 |
| De ellas, en esta cola (validables por pares) | 910 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 12 | 29 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 0 | 0 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 30 | 832 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 9 | 49 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **51** | **910** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (12 clases, 29 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 1 |  | 8 |
| 2 | CDecisionMaker | 5 | 1 |  | 19 |
| 3 | CPedModelInfo | 12 | 1 |  | 21 |
| 4 | CScriptsForBrains | 15 | 1 |  | 22 |
| 5 | CAudioZones | 9 | 1 |  | 31 |
| 6 | CGamma | 3 | 1 |  | 33 |
| 7 | IKChainManager_c | 19 | 2 |  | 20 |
| 8 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 9 | LibJpegError | 3 | 3 |  | 26 |
| 10 | FxEmitterBP_c | 13 | 4 |  | 31 |
| 11 | FxEmitter_c | 9 | 5 |  | 34 |
| 12 | CTheScripts | 82 | 7 |  | 31 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (0 clases, 0 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (30 clases, 832 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CEntryInfoList | 2 | 1 |  | 40 |
| 2 | FxInfo_c | 1 | 1 |  | 41 |
| 3 | CWaterLevel | 49 | 1 |  | 42 |
| 4 | Ps | 26 | 1 |  | ? |
| 5 | RpAnisot | 4 | 1 |  | ? |
| 6 | RtSlerp | 1 | 1 |  | ? |
| 7 | Win | 8 | 1 |  | ? |
| 8 | CCamera | 126 | 2 |  | 38 |
| 9 | RtBMP | 2 | 2 |  | ? |
| 10 | DelayLoadThunks | 3 | 3 |  | ? |
| 11 | QTMLRuntime | 6 | 6 |  | ? |
| 12 | RpHAnim | 7 | 6 |  | ? |
| 13 | RtQuat | 6 | 6 |  | ? |
| 14 | RtAnim | 10 | 7 |  | 48 |
| 15 | RtDict | 12 | 10 |  | ? |
| 16 | LibJpegCompressEncode | 11 | 11 |  | 47 |
| 17 | D3DX9 | 11 | 11 |  | ? |
| 18 | RwD3D9RasterDriver | 14 | 14 |  | 29 |
| 19 | RpUVAnim | 16 | 14 |  | ? |
| 20 | ZlibInflate | 14 | 14 |  | ? |
| 21 | ExeLibJpeg | 15 | 15 |  | ? |
| 22 | ImportThunks | 17 | 17 |  | ? |
| 23 | LibJpegCompress | 18 | 18 |  | 38 |
| 24 | LibJpegMemory | 21 | 21 |  | 26 |
| 25 | RpMatFX | 28 | 28 |  | ? |
| 26 | RpSkin | 34 | 33 |  | ? |
| 27 | RwD3D9 | 115 | 115 |  | ? |
| 28 | RwPlCore | 132 | 118 |  | ? |
| 29 | RpWorld | 197 | 172 |  | ? |
| 30 | RwCore | 211 | 182 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (9 clases, 49 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | FxInfoManager_c | 6 | 1 |  | 52 |
| 2 | ExeProtectionStub | 8 | 1 |  | 57 |
| 3 | CMissionCleanup | 4 | 1 |  | 133 |
| 4 | LibJpegCompressApi | 3 | 3 |  | 51 |
| 5 | LibJpegDecompressUpsample | 3 | 3 |  | 65 |
| 6 | LibJpegDecompressApi | 4 | 4 |  | 73 |
| 7 | CRunningScript | 43 | 4 |  | 77 |
| 8 | D3D9RasterFormat | 10 | 10 |  | 88 |
| 9 | LibJpegDecompress | 22 | 22 |  | 51 |

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
