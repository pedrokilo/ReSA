# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 17/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.441 |
| Reconstruidas | 12.441 |
| Validadas | 11.313 |
| Reconstruidas sin validar | 1.128 |
| De ellas, en esta cola (validables por pares) | 1.128 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 67 | 191 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 1 | 27 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 36 | 853 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 10 | 57 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **114** | **1128** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (67 clases, 191 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 1 |  | 8 |
| 2 | CTaskComplexBeCop | 5 | 1 |  | 11 |
| 3 | CBuilding | 7 | 1 |  | 12 |
| 4 | CAnimBlendClumpData | 7 | 1 |  | 16 |
| 5 | CPedModelInfo | 12 | 1 |  | 21 |
| 6 | CAudioZones | 9 | 1 |  | 31 |
| 7 | CGamma | 3 | 1 |  | 33 |
| 8 | FxFrustumInfo_c | 2 | 1 |  | 33 |
| 9 | CTaskComplexSeekEntityRadiusAngleOffset | 11 | 2 |  | 4 |
| 10 | CAEAudioEntity | 8 | 2 |  | 10 |
| 11 | CDecisionMaker | 5 | 2 |  | 19 |
| 12 | IKChainManager_c | 19 | 2 |  | 20 |
| 13 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 14 | CPedLookAtAttractor | 3 | 3 |  | 18 |
| 15 | CPedParkAttractor | 3 | 3 |  | 18 |
| 16 | CPedScriptedAttractor | 3 | 3 |  | 18 |
| 17 | CPedStepAttractor | 3 | 3 |  | 18 |
| 18 | CPedATMAttractor | 3 | 3 |  | 19 |
| 19 | CPedPizzaAttractor | 3 | 3 |  | 19 |
| 20 | CPedSeatAttractor | 3 | 3 |  | 19 |
| 21 | CPedStopAttractor | 3 | 3 |  | 19 |
| 22 | CPedTriggerScriptAttractor | 3 | 3 |  | 19 |
| 23 | CPedAttractorManager | 25 | 3 |  | 22 |
| 24 | FxInfoEmAngle_c | 3 | 3 |  | 22 |
| 25 | FxInfoEmDir_c | 3 | 3 |  | 22 |
| 26 | FxInfoEmLife_c | 3 | 3 |  | 22 |
| 27 | FxInfoEmPos_c | 3 | 3 |  | 22 |
| 28 | CProtectionStub | 3 | 3 |  | 23 |
| 29 | FxInfoHeatHaze_c | 3 | 3 |  | 23 |
| 30 | FxInfoSelfLit_c | 3 | 3 |  | 23 |
| 31 | FxInfoEmRotation_c | 3 | 3 |  | 24 |
| 32 | FxInfoEmSpeed_c | 3 | 3 |  | 24 |
| 33 | FxInfoEmSize_c | 3 | 3 |  | 25 |
| 34 | FxInfoEmWeather_c | 3 | 3 |  | 25 |
| 35 | FxInfoFloat_c | 3 | 3 |  | 25 |
| 36 | FxInfoForce_c | 3 | 3 |  | 25 |
| 37 | FxInfoFriction_c | 3 | 3 |  | 25 |
| 38 | FxInfoUnderwater_c | 3 | 3 |  | 25 |
| 39 | FxInterpInfo255_c | 4 | 3 |  | 25 |
| 40 | FxInterpInfo32_c | 4 | 3 |  | 25 |
| 41 | FxInterpInfoU255_c | 4 | 3 |  | 25 |
| 42 | FxInfoAnimTexture_c | 3 | 3 |  | 26 |
| 43 | FxInfoColourBright_c | 3 | 3 |  | 26 |
| 44 | FxInfoColourRange_c | 3 | 3 |  | 26 |
| 45 | FxInterpInfoFloat_c | 4 | 3 |  | 26 |
| 46 | LibJpegError | 3 | 3 |  | 26 |
| 47 | FxInfoColour_c | 3 | 3 |  | 27 |
| 48 | FxInfoDir_c | 3 | 3 |  | 27 |
| 49 | FxInfoEmRate_c | 3 | 3 |  | 27 |
| 50 | FxInfoFlat_c | 3 | 3 |  | 27 |
| 51 | FxInfoTrail_c | 3 | 3 |  | 27 |
| 52 | FxInfoAttractPt_c | 3 | 3 |  | 28 |
| 53 | FxInfoNoise_c | 3 | 3 |  | 28 |
| 54 | FxInfoSmoke_c | 3 | 3 |  | 28 |
| 55 | FxInfoSpriteRect_c | 3 | 3 |  | 28 |
| 56 | FxInfoWind_c | 3 | 3 |  | 28 |
| 57 | FxInfoJitter_c | 3 | 3 |  | 29 |
| 58 | FxInfoRotSpeed_c | 3 | 3 |  | 29 |
| 59 | FxInfoSize_c | 3 | 3 |  | 32 |
| 60 | FxInfoAttractLine_c | 3 | 3 |  | 34 |
| 61 | CEntity | 76 | 3 |  | 35 |
| 62 | FxInfoGroundCollide_c | 3 | 3 |  | 35 |
| 63 | CPedShelterAttractor | 11 | 4 |  | 17 |
| 64 | CScriptsForBrains | 15 | 4 |  | 22 |
| 65 | FxEmitterBP_c | 13 | 4 |  | 31 |
| 66 | FxEmitter_c | 9 | 5 |  | 34 |
| 67 | CTheScripts | 82 | 9 |  | 31 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (1 clases, 27 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedAttractor | 85 | 27 |  | 12 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (36 clases, 853 funciones)

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
| 15 | RpAnimBlend | 52 | 5 |  | ? |
| 16 | TList_c | 8 | 5 |  | ? |
| 17 | QTMLRuntime | 6 | 6 |  | ? |
| 18 | RpHAnim | 7 | 6 |  | ? |
| 19 | RtQuat | 6 | 6 |  | ? |
| 20 | RtAnim | 10 | 7 |  | 48 |
| 21 | RtDict | 12 | 10 |  | ? |
| 22 | LibJpegCompressEncode | 11 | 11 |  | 47 |
| 23 | D3DX9 | 11 | 11 |  | ? |
| 24 | RwD3D9RasterDriver | 14 | 14 |  | 29 |
| 25 | RpUVAnim | 16 | 14 |  | ? |
| 26 | ZlibInflate | 14 | 14 |  | ? |
| 27 | ExeLibJpeg | 15 | 15 |  | ? |
| 28 | ImportThunks | 17 | 17 |  | ? |
| 29 | LibJpegCompress | 18 | 18 |  | 38 |
| 30 | LibJpegMemory | 21 | 21 |  | 26 |
| 31 | RpMatFX | 28 | 28 |  | ? |
| 32 | RpSkin | 34 | 33 |  | ? |
| 33 | RwD3D9 | 115 | 115 |  | ? |
| 34 | RwPlCore | 132 | 118 |  | ? |
| 35 | RpWorld | 197 | 172 |  | ? |
| 36 | RwCore | 211 | 182 |  | ? |

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
