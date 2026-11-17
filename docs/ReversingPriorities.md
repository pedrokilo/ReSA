# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 16/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.441 |
| Reconstruidas | 12.441 |
| Validadas | 11.200 |
| Reconstruidas sin validar | 1.241 |
| De ellas, en esta cola (validables por pares) | 1.241 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 76 | 206 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 3 | 101 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 41 | 867 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 11 | 67 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **131** | **1241** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (76 clases, 206 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 1 |  | 8 |
| 2 | CTaskComplexBeCop | 5 | 1 |  | 11 |
| 3 | CBuilding | 7 | 1 |  | 12 |
| 4 | CTaskComplexTurnToFaceEntityOrCoord | 10 | 1 |  | 12 |
| 5 | CAnimBlendClumpData | 7 | 1 |  | 16 |
| 6 | CQuaternion | 11 | 1 |  | 17 |
| 7 | CTaskComplexInvestigateDisturbance | 9 | 1 |  | 17 |
| 8 | CPedModelInfo | 12 | 1 |  | 21 |
| 9 | CTaskComplexGoToPointAnyMeans | 10 | 1 |  | 21 |
| 10 | CTaskComplexGotoDoorAndOpen | 11 | 1 |  | 23 |
| 11 | CTaskSimpleSlideToCoord | 6 | 1 |  | 24 |
| 12 | CPlayerInfo | 32 | 1 |  | 26 |
| 13 | CAudioZones | 9 | 1 |  | 31 |
| 14 | CPlayerPedData | 6 | 1 |  | 31 |
| 15 | CGamma | 3 | 1 |  | 33 |
| 16 | FxFrustumInfo_c | 2 | 1 |  | 33 |
| 17 | CTaskComplexSeekEntityRadiusAngleOffset | 11 | 2 |  | 4 |
| 18 | CAEAudioEntity | 8 | 2 |  | 10 |
| 19 | CDecisionMaker | 5 | 2 |  | 19 |
| 20 | IKChainManager_c | 19 | 2 |  | 20 |
| 21 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 22 | CPedLookAtAttractor | 3 | 3 |  | 18 |
| 23 | CPedParkAttractor | 3 | 3 |  | 18 |
| 24 | CPedScriptedAttractor | 3 | 3 |  | 18 |
| 25 | CPedStepAttractor | 3 | 3 |  | 18 |
| 26 | CPedATMAttractor | 3 | 3 |  | 19 |
| 27 | CPedPizzaAttractor | 3 | 3 |  | 19 |
| 28 | CPedSeatAttractor | 3 | 3 |  | 19 |
| 29 | CPedStopAttractor | 3 | 3 |  | 19 |
| 30 | CPedTriggerScriptAttractor | 3 | 3 |  | 19 |
| 31 | CPedAttractorManager | 25 | 3 |  | 22 |
| 32 | FxInfoEmAngle_c | 3 | 3 |  | 22 |
| 33 | FxInfoEmDir_c | 3 | 3 |  | 22 |
| 34 | FxInfoEmLife_c | 3 | 3 |  | 22 |
| 35 | FxInfoEmPos_c | 3 | 3 |  | 22 |
| 36 | CProtectionStub | 3 | 3 |  | 23 |
| 37 | FxInfoHeatHaze_c | 3 | 3 |  | 23 |
| 38 | FxInfoSelfLit_c | 3 | 3 |  | 23 |
| 39 | FxInfoEmRotation_c | 3 | 3 |  | 24 |
| 40 | FxInfoEmSpeed_c | 3 | 3 |  | 24 |
| 41 | FxInfoEmSize_c | 3 | 3 |  | 25 |
| 42 | FxInfoEmWeather_c | 3 | 3 |  | 25 |
| 43 | FxInfoFloat_c | 3 | 3 |  | 25 |
| 44 | FxInfoForce_c | 3 | 3 |  | 25 |
| 45 | FxInfoFriction_c | 3 | 3 |  | 25 |
| 46 | FxInfoUnderwater_c | 3 | 3 |  | 25 |
| 47 | FxInterpInfo255_c | 4 | 3 |  | 25 |
| 48 | FxInterpInfo32_c | 4 | 3 |  | 25 |
| 49 | FxInterpInfoU255_c | 4 | 3 |  | 25 |
| 50 | FxInfoAnimTexture_c | 3 | 3 |  | 26 |
| 51 | FxInfoColourBright_c | 3 | 3 |  | 26 |
| 52 | FxInfoColourRange_c | 3 | 3 |  | 26 |
| 53 | FxInterpInfoFloat_c | 4 | 3 |  | 26 |
| 54 | LibJpegError | 3 | 3 |  | 26 |
| 55 | FxInfoColour_c | 3 | 3 |  | 27 |
| 56 | FxInfoDir_c | 3 | 3 |  | 27 |
| 57 | FxInfoEmRate_c | 3 | 3 |  | 27 |
| 58 | FxInfoFlat_c | 3 | 3 |  | 27 |
| 59 | FxInfoTrail_c | 3 | 3 |  | 27 |
| 60 | FxInfoAttractPt_c | 3 | 3 |  | 28 |
| 61 | FxInfoNoise_c | 3 | 3 |  | 28 |
| 62 | FxInfoSmoke_c | 3 | 3 |  | 28 |
| 63 | FxInfoSpriteRect_c | 3 | 3 |  | 28 |
| 64 | FxInfoWind_c | 3 | 3 |  | 28 |
| 65 | FxInfoJitter_c | 3 | 3 |  | 29 |
| 66 | FxInfoRotSpeed_c | 3 | 3 |  | 29 |
| 67 | FxInfoSize_c | 3 | 3 |  | 32 |
| 68 | FxInfoAttractLine_c | 3 | 3 |  | 34 |
| 69 | CEntity | 76 | 3 |  | 35 |
| 70 | FxInfoGroundCollide_c | 3 | 3 |  | 35 |
| 71 | CPedShelterAttractor | 11 | 4 |  | 17 |
| 72 | CScriptsForBrains | 15 | 4 |  | 22 |
| 73 | FxEmitterBP_c | 13 | 4 |  | 31 |
| 74 | FxEmitter_c | 9 | 5 |  | 34 |
| 75 | CTaskComplexFollowPatrolRoute | 11 | 6 |  | 33 |
| 76 | CTheScripts | 82 | 10 |  | 31 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (3 clases, 101 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CCustomCarEnvMapPipeline | 60 | 23 |  | 4 |
| 2 | CPedAttractor | 85 | 27 |  | 12 |
| 3 | CPools | 72 | 51 |  | 10 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (41 clases, 867 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CStreaming | 110 | 1 |  | 36 |
| 2 | CEntryInfoList | 2 | 1 |  | 40 |
| 3 | tReplayTrainBlock | 1 | 1 |  | 40 |
| 4 | FxInfo_c | 1 | 1 |  | 41 |
| 5 | CWaterLevel | 49 | 1 |  | 42 |
| 6 | CCover | 17 | 1 |  | 43 |
| 7 | Ps | 26 | 1 |  | ? |
| 8 | RpAnisot | 4 | 1 |  | ? |
| 9 | RtSlerp | 1 | 1 |  | ? |
| 10 | Win | 8 | 1 |  | ? |
| 11 | CCamera | 126 | 2 |  | 38 |
| 12 | CPtrNodeDoubleLinkPool | 7 | 2 |  | ? |
| 13 | CPtrNodeSingleLinkPool | 7 | 2 |  | ? |
| 14 | ListItem_c | 2 | 2 |  | ? |
| 15 | RtBMP | 2 | 2 |  | ? |
| 16 | DelayLoadThunks | 3 | 3 |  | ? |
| 17 | CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos> | 7 | 4 |  | ? |
| 18 | LegacyRuntimeError | 13 | 5 |  | ? |
| 19 | RpAnimBlend | 52 | 5 |  | ? |
| 20 | TList_c | 8 | 5 |  | ? |
| 21 | QTMLRuntime | 6 | 6 |  | ? |
| 22 | RpHAnim | 7 | 6 |  | ? |
| 23 | RtQuat | 6 | 6 |  | ? |
| 24 | CReplay | 55 | 7 |  | 38 |
| 25 | RtAnim | 10 | 7 |  | 48 |
| 26 | RtDict | 12 | 10 |  | ? |
| 27 | LibJpegCompressEncode | 11 | 11 |  | 47 |
| 28 | D3DX9 | 11 | 11 |  | ? |
| 29 | RwD3D9RasterDriver | 14 | 14 |  | 29 |
| 30 | RpUVAnim | 16 | 14 |  | ? |
| 31 | ZlibInflate | 14 | 14 |  | ? |
| 32 | ExeLibJpeg | 15 | 15 |  | ? |
| 33 | ImportThunks | 17 | 17 |  | ? |
| 34 | LibJpegCompress | 18 | 18 |  | 38 |
| 35 | LibJpegMemory | 21 | 21 |  | 26 |
| 36 | RpMatFX | 28 | 28 |  | ? |
| 37 | RpSkin | 34 | 33 |  | ? |
| 38 | RwD3D9 | 115 | 115 |  | ? |
| 39 | RwPlCore | 132 | 118 |  | ? |
| 40 | RpWorld | 197 | 172 |  | ? |
| 41 | RwCore | 211 | 182 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (11 clases, 67 funciones)

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
| 9 | CAutomobile | 114 | 10 |  | 71 |
| 10 | D3D9RasterFormat | 10 | 10 |  | 88 |
| 11 | LibJpegDecompress | 22 | 22 |  | 51 |

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
