# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 17/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.441 |
| Reconstruidas | 12.441 |
| Validadas | 11.335 |
| Reconstruidas sin validar | 1.106 |
| De ellas, en esta cola (validables por pares) | 1.106 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 66 | 174 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 1 | 27 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 35 | 848 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 10 | 57 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **112** | **1106** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (66 clases, 174 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 1 |  | 8 |
| 2 | CTaskComplexBeCop | 5 | 1 |  | 11 |
| 3 | CBuilding | 7 | 1 |  | 12 |
| 4 | CPedModelInfo | 12 | 1 |  | 21 |
| 5 | CScriptsForBrains | 15 | 1 |  | 22 |
| 6 | CAudioZones | 9 | 1 |  | 31 |
| 7 | CGamma | 3 | 1 |  | 33 |
| 8 | FxFrustumInfo_c | 2 | 1 |  | 33 |
| 9 | CTaskComplexSeekEntityRadiusAngleOffset | 11 | 2 |  | 4 |
| 10 | CAEAudioEntity | 8 | 2 |  | 10 |
| 11 | CDecisionMaker | 5 | 2 |  | 19 |
| 12 | IKChainManager_c | 19 | 2 |  | 20 |
| 13 | FxInfoEmAngle_c | 3 | 2 |  | 22 |
| 14 | FxInfoEmDir_c | 3 | 2 |  | 22 |
| 15 | FxInfoEmLife_c | 3 | 2 |  | 22 |
| 16 | FxInfoEmPos_c | 3 | 2 |  | 22 |
| 17 | FxInfoEmRotation_c | 3 | 2 |  | 24 |
| 18 | FxInfoEmSpeed_c | 3 | 2 |  | 24 |
| 19 | FxInfoEmSize_c | 3 | 2 |  | 25 |
| 20 | FxInfoEmWeather_c | 3 | 2 |  | 25 |
| 21 | FxInfoForce_c | 3 | 2 |  | 25 |
| 22 | FxInfoFriction_c | 3 | 2 |  | 25 |
| 23 | FxInfoAttractPt_c | 3 | 2 |  | 28 |
| 24 | FxInfoNoise_c | 3 | 2 |  | 28 |
| 25 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 26 | FxInfoAttractLine_c | 3 | 2 |  | 34 |
| 27 | CPedLookAtAttractor | 3 | 3 |  | 18 |
| 28 | CPedParkAttractor | 3 | 3 |  | 18 |
| 29 | CPedScriptedAttractor | 3 | 3 |  | 18 |
| 30 | CPedStepAttractor | 3 | 3 |  | 18 |
| 31 | CPedATMAttractor | 3 | 3 |  | 19 |
| 32 | CPedPizzaAttractor | 3 | 3 |  | 19 |
| 33 | CPedSeatAttractor | 3 | 3 |  | 19 |
| 34 | CPedStopAttractor | 3 | 3 |  | 19 |
| 35 | CPedTriggerScriptAttractor | 3 | 3 |  | 19 |
| 36 | CPedAttractorManager | 25 | 3 |  | 22 |
| 37 | CProtectionStub | 3 | 3 |  | 23 |
| 38 | FxInfoHeatHaze_c | 3 | 3 |  | 23 |
| 39 | FxInfoSelfLit_c | 3 | 3 |  | 23 |
| 40 | FxInfoFloat_c | 3 | 3 |  | 25 |
| 41 | FxInfoUnderwater_c | 3 | 3 |  | 25 |
| 42 | FxInterpInfo255_c | 4 | 3 |  | 25 |
| 43 | FxInterpInfo32_c | 4 | 3 |  | 25 |
| 44 | FxInterpInfoU255_c | 4 | 3 |  | 25 |
| 45 | FxInfoAnimTexture_c | 3 | 3 |  | 26 |
| 46 | FxInfoColourBright_c | 3 | 3 |  | 26 |
| 47 | FxInfoColourRange_c | 3 | 3 |  | 26 |
| 48 | FxInterpInfoFloat_c | 4 | 3 |  | 26 |
| 49 | LibJpegError | 3 | 3 |  | 26 |
| 50 | FxInfoColour_c | 3 | 3 |  | 27 |
| 51 | FxInfoDir_c | 3 | 3 |  | 27 |
| 52 | FxInfoEmRate_c | 3 | 3 |  | 27 |
| 53 | FxInfoFlat_c | 3 | 3 |  | 27 |
| 54 | FxInfoTrail_c | 3 | 3 |  | 27 |
| 55 | FxInfoSmoke_c | 3 | 3 |  | 28 |
| 56 | FxInfoSpriteRect_c | 3 | 3 |  | 28 |
| 57 | FxInfoWind_c | 3 | 3 |  | 28 |
| 58 | FxInfoJitter_c | 3 | 3 |  | 29 |
| 59 | FxInfoRotSpeed_c | 3 | 3 |  | 29 |
| 60 | FxInfoSize_c | 3 | 3 |  | 32 |
| 61 | CEntity | 76 | 3 |  | 35 |
| 62 | FxInfoGroundCollide_c | 3 | 3 |  | 35 |
| 63 | CPedShelterAttractor | 11 | 4 |  | 17 |
| 64 | FxEmitterBP_c | 13 | 4 |  | 31 |
| 65 | FxEmitter_c | 9 | 5 |  | 34 |
| 66 | CTheScripts | 82 | 9 |  | 31 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (1 clases, 27 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedAttractor | 85 | 27 |  | 12 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (35 clases, 848 funciones)

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
| 9 | CPtrNodeDoubleLinkPool | 7 | 2 |  | ? |
| 10 | CPtrNodeSingleLinkPool | 7 | 2 |  | ? |
| 11 | ListItem_c | 2 | 2 |  | ? |
| 12 | RtBMP | 2 | 2 |  | ? |
| 13 | DelayLoadThunks | 3 | 3 |  | ? |
| 14 | LegacyRuntimeError | 13 | 5 |  | ? |
| 15 | TList_c | 8 | 5 |  | ? |
| 16 | QTMLRuntime | 6 | 6 |  | ? |
| 17 | RpHAnim | 7 | 6 |  | ? |
| 18 | RtQuat | 6 | 6 |  | ? |
| 19 | RtAnim | 10 | 7 |  | 48 |
| 20 | RtDict | 12 | 10 |  | ? |
| 21 | LibJpegCompressEncode | 11 | 11 |  | 47 |
| 22 | D3DX9 | 11 | 11 |  | ? |
| 23 | RwD3D9RasterDriver | 14 | 14 |  | 29 |
| 24 | RpUVAnim | 16 | 14 |  | ? |
| 25 | ZlibInflate | 14 | 14 |  | ? |
| 26 | ExeLibJpeg | 15 | 15 |  | ? |
| 27 | ImportThunks | 17 | 17 |  | ? |
| 28 | LibJpegCompress | 18 | 18 |  | 38 |
| 29 | LibJpegMemory | 21 | 21 |  | 26 |
| 30 | RpMatFX | 28 | 28 |  | ? |
| 31 | RpSkin | 34 | 33 |  | ? |
| 32 | RwD3D9 | 115 | 115 |  | ? |
| 33 | RwPlCore | 132 | 118 |  | ? |
| 34 | RpWorld | 197 | 172 |  | ? |
| 35 | RwCore | 211 | 182 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (10 clases, 57 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | FxInfoManager_c | 6 | 1 |  | 52 |
| 2 | CAutoPilot | 4 | 1 |  | 60 |
| 3 | CMissionCleanup | 4 | 1 |  | 133 |
| 4 | LibJpegCompressApi | 3 | 3 |  | 51 |
| 5 | LibJpegDecompressUpsample | 3 | 3 |  | 65 |
| 6 | LibJpegDecompressApi | 4 | 4 |  | 73 |
| 7 | CRunningScript | 43 | 4 |  | 77 |
| 8 | ExeProtectionStub | 8 | 8 |  | 57 |
| 9 | D3D9RasterFormat | 10 | 10 |  | 88 |
| 10 | LibJpegDecompress | 22 | 22 |  | 51 |

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
