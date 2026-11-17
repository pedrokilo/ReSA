# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 17/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 12.441 |
| Reconstruidas | 12.441 |
| Validadas | 11.306 |
| Reconstruidas sin validar | 1.135 |
| De ellas, en esta cola (validables por pares) | 1.135 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 70 | 195 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 1 | 27 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 39 | 856 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 10 | 57 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **120** | **1135** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (70 clases, 195 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskSimpleSetCharIgnoreWeaponRangeFlag | 7 | 1 |  | 8 |
| 2 | CTaskComplexBeCop | 5 | 1 |  | 11 |
| 3 | CBuilding | 7 | 1 |  | 12 |
| 4 | CAnimBlendClumpData | 7 | 1 |  | 16 |
| 5 | CQuaternion | 11 | 1 |  | 17 |
| 6 | CPedModelInfo | 12 | 1 |  | 21 |
| 7 | CPlayerInfo | 32 | 1 |  | 26 |
| 8 | CAudioZones | 9 | 1 |  | 31 |
| 9 | CPlayerPedData | 6 | 1 |  | 31 |
| 10 | CGamma | 3 | 1 |  | 33 |
| 11 | FxFrustumInfo_c | 2 | 1 |  | 33 |
| 12 | CTaskComplexSeekEntityRadiusAngleOffset | 11 | 2 |  | 4 |
| 13 | CAEAudioEntity | 8 | 2 |  | 10 |
| 14 | CDecisionMaker | 5 | 2 |  | 19 |
| 15 | IKChainManager_c | 19 | 2 |  | 20 |
| 16 | WaterCreatureManager_c | 9 | 2 |  | 29 |
| 17 | CPedLookAtAttractor | 3 | 3 |  | 18 |
| 18 | CPedParkAttractor | 3 | 3 |  | 18 |
| 19 | CPedScriptedAttractor | 3 | 3 |  | 18 |
| 20 | CPedStepAttractor | 3 | 3 |  | 18 |
| 21 | CPedATMAttractor | 3 | 3 |  | 19 |
| 22 | CPedPizzaAttractor | 3 | 3 |  | 19 |
| 23 | CPedSeatAttractor | 3 | 3 |  | 19 |
| 24 | CPedStopAttractor | 3 | 3 |  | 19 |
| 25 | CPedTriggerScriptAttractor | 3 | 3 |  | 19 |
| 26 | CPedAttractorManager | 25 | 3 |  | 22 |
| 27 | FxInfoEmAngle_c | 3 | 3 |  | 22 |
| 28 | FxInfoEmDir_c | 3 | 3 |  | 22 |
| 29 | FxInfoEmLife_c | 3 | 3 |  | 22 |
| 30 | FxInfoEmPos_c | 3 | 3 |  | 22 |
| 31 | CProtectionStub | 3 | 3 |  | 23 |
| 32 | FxInfoHeatHaze_c | 3 | 3 |  | 23 |
| 33 | FxInfoSelfLit_c | 3 | 3 |  | 23 |
| 34 | FxInfoEmRotation_c | 3 | 3 |  | 24 |
| 35 | FxInfoEmSpeed_c | 3 | 3 |  | 24 |
| 36 | FxInfoEmSize_c | 3 | 3 |  | 25 |
| 37 | FxInfoEmWeather_c | 3 | 3 |  | 25 |
| 38 | FxInfoFloat_c | 3 | 3 |  | 25 |
| 39 | FxInfoForce_c | 3 | 3 |  | 25 |
| 40 | FxInfoFriction_c | 3 | 3 |  | 25 |
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
| 55 | FxInfoAttractPt_c | 3 | 3 |  | 28 |
| 56 | FxInfoNoise_c | 3 | 3 |  | 28 |
| 57 | FxInfoSmoke_c | 3 | 3 |  | 28 |
| 58 | FxInfoSpriteRect_c | 3 | 3 |  | 28 |
| 59 | FxInfoWind_c | 3 | 3 |  | 28 |
| 60 | FxInfoJitter_c | 3 | 3 |  | 29 |
| 61 | FxInfoRotSpeed_c | 3 | 3 |  | 29 |
| 62 | FxInfoSize_c | 3 | 3 |  | 32 |
| 63 | FxInfoAttractLine_c | 3 | 3 |  | 34 |
| 64 | CEntity | 76 | 3 |  | 35 |
| 65 | FxInfoGroundCollide_c | 3 | 3 |  | 35 |
| 66 | CPedShelterAttractor | 11 | 4 |  | 17 |
| 67 | CScriptsForBrains | 15 | 4 |  | 22 |
| 68 | FxEmitterBP_c | 13 | 4 |  | 31 |
| 69 | FxEmitter_c | 9 | 5 |  | 34 |
| 70 | CTheScripts | 82 | 10 |  | 31 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (1 clases, 27 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedAttractor | 85 | 27 |  | 12 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (39 clases, 856 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CStreaming | 110 | 1 |  | 36 |
| 2 | CReplay | 55 | 1 |  | 38 |
| 3 | CEntryInfoList | 2 | 1 |  | 40 |
| 4 | tReplayTrainBlock | 1 | 1 |  | 40 |
| 5 | FxInfo_c | 1 | 1 |  | 41 |
| 6 | CWaterLevel | 49 | 1 |  | 42 |
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
| 17 | LegacyRuntimeError | 13 | 5 |  | ? |
| 18 | RpAnimBlend | 52 | 5 |  | ? |
| 19 | TList_c | 8 | 5 |  | ? |
| 20 | QTMLRuntime | 6 | 6 |  | ? |
| 21 | RpHAnim | 7 | 6 |  | ? |
| 22 | RtQuat | 6 | 6 |  | ? |
| 23 | RtAnim | 10 | 7 |  | 48 |
| 24 | RtDict | 12 | 10 |  | ? |
| 25 | LibJpegCompressEncode | 11 | 11 |  | 47 |
| 26 | D3DX9 | 11 | 11 |  | ? |
| 27 | RwD3D9RasterDriver | 14 | 14 |  | 29 |
| 28 | RpUVAnim | 16 | 14 |  | ? |
| 29 | ZlibInflate | 14 | 14 |  | ? |
| 30 | ExeLibJpeg | 15 | 15 |  | ? |
| 31 | ImportThunks | 17 | 17 |  | ? |
| 32 | LibJpegCompress | 18 | 18 |  | 38 |
| 33 | LibJpegMemory | 21 | 21 |  | 26 |
| 34 | RpMatFX | 28 | 28 |  | ? |
| 35 | RpSkin | 34 | 33 |  | ? |
| 36 | RwD3D9 | 115 | 115 |  | ? |
| 37 | RwPlCore | 132 | 118 |  | ? |
| 38 | RpWorld | 197 | 172 |  | ? |
| 39 | RwCore | 211 | 182 |  | ? |

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
