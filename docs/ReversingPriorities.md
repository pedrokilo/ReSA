# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 16/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.438 |
| Reconstruidas | 12.438 |
| Validadas | 11.156 |
| Reconstruidas sin validar | 1.282 |
| De ellas, en esta cola (validables por pares) | 1.282 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 79 | 214 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 3 | 108 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 45 | 885 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 15 | 75 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **142** | **1282** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (79 clases, 214 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CZoneDef | 2 | 1 |  | 6 |
| 2 | CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 1 |  | 8 |
| 3 | CTaskComplexBeCop | 5 | 1 |  | 11 |
| 4 | CBuilding | 7 | 1 |  | 12 |
| 5 | CTaskComplexTurnToFaceEntityOrCoord | 10 | 1 |  | 12 |
| 6 | CAnimBlendClumpData | 7 | 1 |  | 16 |
| 7 | CQuaternion | 11 | 1 |  | 17 |
| 8 | CTaskComplexInvestigateDisturbance | 9 | 1 |  | 17 |
| 9 | CPatrolRoute | 7 | 1 |  | 20 |
| 10 | CPedModelInfo | 12 | 1 |  | 21 |
| 11 | CTaskComplexGoToPointAnyMeans | 10 | 1 |  | 21 |
| 12 | CTaskComplexGotoDoorAndOpen | 11 | 1 |  | 23 |
| 13 | CPlayerInfo | 32 | 1 |  | 26 |
| 14 | CAudioZones | 9 | 1 |  | 31 |
| 15 | CPlayerPedData | 6 | 1 |  | 31 |
| 16 | CGamma | 3 | 1 |  | 33 |
| 17 | FxFrustumInfo_c | 2 | 1 |  | 33 |
| 18 | CTaskComplexSeekEntityRadiusAngleOffset | 11 | 2 |  | 4 |
| 19 | CAEAudioEntity | 8 | 2 |  | 10 |
| 20 | SurfaceInfos_c | 50 | 2 |  | 14 |
| 21 | CDecisionMaker | 5 | 2 |  | 19 |
| 22 | IKChainManager_c | 19 | 2 |  | 20 |
| 23 | CTaskSimpleSlideToCoord | 6 | 2 |  | 24 |
| 24 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 25 | CPedLookAtAttractor | 3 | 3 |  | 18 |
| 26 | CPedParkAttractor | 3 | 3 |  | 18 |
| 27 | CPedScriptedAttractor | 3 | 3 |  | 18 |
| 28 | CPedStepAttractor | 3 | 3 |  | 18 |
| 29 | CPedATMAttractor | 3 | 3 |  | 19 |
| 30 | CPedPizzaAttractor | 3 | 3 |  | 19 |
| 31 | CPedSeatAttractor | 3 | 3 |  | 19 |
| 32 | CPedStopAttractor | 3 | 3 |  | 19 |
| 33 | CPedTriggerScriptAttractor | 3 | 3 |  | 19 |
| 34 | CPedAttractorManager | 25 | 3 |  | 22 |
| 35 | FxInfoEmAngle_c | 3 | 3 |  | 22 |
| 36 | FxInfoEmDir_c | 3 | 3 |  | 22 |
| 37 | FxInfoEmLife_c | 3 | 3 |  | 22 |
| 38 | FxInfoEmPos_c | 3 | 3 |  | 22 |
| 39 | CProtectionStub | 3 | 3 |  | 23 |
| 40 | FxInfoHeatHaze_c | 3 | 3 |  | 23 |
| 41 | FxInfoSelfLit_c | 3 | 3 |  | 23 |
| 42 | FxInfoEmRotation_c | 3 | 3 |  | 24 |
| 43 | FxInfoEmSpeed_c | 3 | 3 |  | 24 |
| 44 | FxInfoEmSize_c | 3 | 3 |  | 25 |
| 45 | FxInfoEmWeather_c | 3 | 3 |  | 25 |
| 46 | FxInfoFloat_c | 3 | 3 |  | 25 |
| 47 | FxInfoForce_c | 3 | 3 |  | 25 |
| 48 | FxInfoFriction_c | 3 | 3 |  | 25 |
| 49 | FxInfoUnderwater_c | 3 | 3 |  | 25 |
| 50 | FxInterpInfo255_c | 4 | 3 |  | 25 |
| 51 | FxInterpInfo32_c | 4 | 3 |  | 25 |
| 52 | FxInterpInfoU255_c | 4 | 3 |  | 25 |
| 53 | FxInfoAnimTexture_c | 3 | 3 |  | 26 |
| 54 | FxInfoColourBright_c | 3 | 3 |  | 26 |
| 55 | FxInfoColourRange_c | 3 | 3 |  | 26 |
| 56 | FxInterpInfoFloat_c | 4 | 3 |  | 26 |
| 57 | LibJpegError | 3 | 3 |  | 26 |
| 58 | FxInfoColour_c | 3 | 3 |  | 27 |
| 59 | FxInfoDir_c | 3 | 3 |  | 27 |
| 60 | FxInfoEmRate_c | 3 | 3 |  | 27 |
| 61 | FxInfoFlat_c | 3 | 3 |  | 27 |
| 62 | FxInfoTrail_c | 3 | 3 |  | 27 |
| 63 | FxInfoAttractPt_c | 3 | 3 |  | 28 |
| 64 | FxInfoNoise_c | 3 | 3 |  | 28 |
| 65 | FxInfoSmoke_c | 3 | 3 |  | 28 |
| 66 | FxInfoSpriteRect_c | 3 | 3 |  | 28 |
| 67 | FxInfoWind_c | 3 | 3 |  | 28 |
| 68 | FxInfoJitter_c | 3 | 3 |  | 29 |
| 69 | FxInfoRotSpeed_c | 3 | 3 |  | 29 |
| 70 | FxInfoSize_c | 3 | 3 |  | 32 |
| 71 | FxInfoAttractLine_c | 3 | 3 |  | 34 |
| 72 | CEntity | 76 | 3 |  | 35 |
| 73 | FxInfoGroundCollide_c | 3 | 3 |  | 35 |
| 74 | CPedShelterAttractor | 11 | 4 |  | 17 |
| 75 | CScriptsForBrains | 15 | 4 |  | 22 |
| 76 | FxEmitterBP_c | 13 | 4 |  | 31 |
| 77 | FxEmitter_c | 9 | 5 |  | 34 |
| 78 | CTaskComplexFollowPatrolRoute | 11 | 9 |  | 33 |
| 79 | CTheScripts | 82 | 10 |  | 31 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (3 clases, 108 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CCustomCarEnvMapPipeline | 60 | 23 |  | 4 |
| 2 | CPedAttractor | 85 | 34 |  | 12 |
| 3 | CPools | 72 | 51 |  | 10 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (45 clases, 885 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CStreaming | 110 | 1 |  | 36 |
| 2 | CEntryInfoList | 2 | 1 |  | 40 |
| 3 | tReplayTrainBlock | 1 | 1 |  | 40 |
| 4 | FxInfo_c | 1 | 1 |  | 41 |
| 5 | CWaterLevel | 49 | 1 |  | 42 |
| 6 | CCover | 17 | 1 |  | 43 |
| 7 | UserTracksLegacyVector | 2 | 1 |  | 46 |
| 8 | Ps | 26 | 1 |  | ? |
| 9 | RpAnisot | 4 | 1 |  | ? |
| 10 | RtSlerp | 1 | 1 |  | ? |
| 11 | RwHelper | 43 | 1 |  | ? |
| 12 | Win | 8 | 1 |  | ? |
| 13 | CCamera | 126 | 2 |  | 38 |
| 14 | CVehicle | 202 | 2 |  | 47 |
| 15 | CPtrNodeDoubleLinkPool | 7 | 2 |  | ? |
| 16 | CPtrNodeSingleLinkPool | 7 | 2 |  | ? |
| 17 | ListItem_c | 2 | 2 |  | ? |
| 18 | RtBMP | 2 | 2 |  | ? |
| 19 | DelayLoadThunks | 3 | 3 |  | ? |
| 20 | CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos> | 7 | 4 |  | ? |
| 21 | RpAnimBlend | 52 | 5 |  | ? |
| 22 | TList_c | 8 | 5 |  | ? |
| 23 | LegacyStdString | 6 | 6 |  | ? |
| 24 | QTMLRuntime | 6 | 6 |  | ? |
| 25 | RpHAnim | 7 | 6 |  | ? |
| 26 | RtQuat | 6 | 6 |  | ? |
| 27 | CReplay | 55 | 7 |  | 38 |
| 28 | RtAnim | 10 | 7 |  | 48 |
| 29 | RtDict | 12 | 10 |  | ? |
| 30 | LibJpegCompressEncode | 11 | 11 |  | 47 |
| 31 | D3DX9 | 11 | 11 |  | ? |
| 32 | LegacyRuntimeError | 13 | 13 |  | ? |
| 33 | RwD3D9RasterDriver | 14 | 14 |  | 29 |
| 34 | RpUVAnim | 16 | 14 |  | ? |
| 35 | ZlibInflate | 14 | 14 |  | ? |
| 36 | ExeLibJpeg | 15 | 15 |  | ? |
| 37 | ImportThunks | 17 | 17 |  | ? |
| 38 | LibJpegCompress | 18 | 18 |  | 38 |
| 39 | LibJpegMemory | 21 | 21 |  | 26 |
| 40 | RpMatFX | 28 | 28 |  | ? |
| 41 | RpSkin | 34 | 33 |  | ? |
| 42 | RwD3D9 | 115 | 115 |  | ? |
| 43 | RwPlCore | 132 | 118 |  | ? |
| 44 | RpWorld | 197 | 172 |  | ? |
| 45 | RwCore | 211 | 182 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (15 clases, 75 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CMemoryMgr | 11 | 1 |  | 51 |
| 2 | FxInfoManager_c | 6 | 1 |  | 52 |
| 3 | CPickup | 13 | 1 |  | 57 |
| 4 | CAutoPilot | 4 | 1 |  | 60 |
| 5 | CAEUserRadioTrackManager | 16 | 1 |  | 61 |
| 6 | CMissionCleanup | 4 | 1 |  | 133 |
| 7 | LibJpegCompressApi | 3 | 3 |  | 51 |
| 8 | LibJpegDecompressUpsample | 3 | 3 |  | 65 |
| 9 | LibJpegDecompressApi | 4 | 4 |  | 73 |
| 10 | CRunningScript | 43 | 4 |  | 77 |
| 11 | CMemoryHeap | 6 | 5 |  | 83 |
| 12 | ExeProtectionStub | 8 | 8 |  | 57 |
| 13 | CAutomobile | 114 | 10 |  | 71 |
| 14 | D3D9RasterFormat | 10 | 10 |  | 88 |
| 15 | LibJpegDecompress | 22 | 22 |  | 51 |

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
