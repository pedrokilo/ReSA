# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 05/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.441 |
| Reconstruidas | 7.858 |
| Validadas | 5.382 |
| Reconstruidas sin validar | 2.476 |
| De ellas, en esta cola (validables por pares) | 2.472 |
| Sin reconstruir (en 105 clases incompletas) | 583 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 36 | 194 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 9 | 569 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 37 | 776 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 33 | 933 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **115** | **2472** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (36 clases, 194 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskComplexBeCop | 3 | 2 | 1 | 22 |
| 2 | CTaskComplexCarSlowBeDraggedOutAndStandUp | 9 | 2 | 6 | 29 |
| 3 | CPedShelterAttractor | 4 | 2 | 2 | 30 |
| 4 | CTaskComplexGoToPointAndStandStillAndAchieveHeading | 7 | 2 | 4 | 33 |
| 5 | CTaskSimpleCarSetPedOut | 5 | 2 | 3 | 34 |
| 6 | CTaskComplexOpenPassengerDoor | 4 | 3 | 1 | 13 |
| 7 | CTaskSimpleRunNamedAnim | 7 | 3 | 3 | 25 |
| 8 | CTaskComplexWanderCriminal | 6 | 3 | 3 | 28 |
| 9 | CTaskComplexGoToPointAnyMeans | 7 | 3 | 4 | 33 |
| 10 | CScriptsForBrains | 6 | 3 | 3 | 34 |
| 11 | CTaskComplexGotoDoorAndOpen | 7 | 3 | 4 | 35 |
| 12 | CTaskLeanOnVehicle | 5 | 3 | 2 | 35 |
| 13 | CTaskComplexWanderProstitute | 5 | 4 | 1 | 12 |
| 14 | CTaskSimpleCarDriveTimed | 5 | 4 | 1 | 14 |
| 15 | CTaskSimpleCarSetPedInAsPassenger | 5 | 4 | 1 | 18 |
| 16 | CTaskComplexSignalAtPed | 7 | 4 | 3 | 19 |
| 17 | CTaskSimpleCarSetPedInAsDriver | 5 | 4 | 1 | 19 |
| 18 | CTaskComplexEnterCarAsPassengerWait | 8 | 4 | 4 | 24 |
| 19 | CTaskSimpleChoking | 7 | 5 | 2 | 18 |
| 20 | CTaskComplexGoToPointAiming | 8 | 5 | 3 | 22 |
| 21 | CTaskComplexDriveToPoint | 8 | 5 | 3 | 32 |
| 22 | CTaskComplexUseClosestFreeScriptedAttractor | 8 | 6 | 2 | 10 |
| 23 | CTaskComplexEvasiveCower | 8 | 7 | 1 | 11 |
| 24 | CTaskComplexEvasiveStep | 8 | 7 | 1 | 13 |
| 25 | CTaskComplexBeInCouple | 9 | 7 | 2 | 19 |
| 26 | CTaskSimpleCarDrive | 13 | 7 | 6 | 31 |
| 27 | CBmx | 10 | 7 | 3 | 35 |
| 28 | CTaskComplexCrossRoadLookAndAchieveHeading | 9 | 8 | 1 | 12 |
| 29 | CTaskComplexDestroyCar | 9 | 8 | 1 | 19 |
| 30 | CCarFXRenderer | 10 | 9 | 1 | 14 |
| 31 | CTaskComplexWalkRoundCar | 13 | 9 | 4 | 16 |
| 32 | CRopes | 11 | 9 | 2 | 17 |
| 33 | CStencilShadows | 13 | 9 | 4 | 35 |
| 34 | CPedGroup | 12 | 10 | 2 | 18 |
| 35 | CWeather | 13 | 10 | 3 | 34 |
| 36 | CQuadBike | 12 | 11 | 1 | 30 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (9 clases, 569 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | FxSystem_c | 35 | 31 | 4 | 15 |
| 2 | CPickups | 37 | 35 | 2 | 23 |
| 3 | CPathFind | 56 | 54 | 2 | 25 |
| 4 | CMenuManager | 61 | 61 |  | 20 |
| 5 | CCheat | 72 | 72 |  | 17 |
| 6 | CPad | 75 | 75 |  | 19 |
| 7 | CVehicleModelInfo | 75 | 75 |  | 22 |
| 8 | CAudioEngine | 82 | 82 |  | 10 |
| 9 | CVisibilityPlugins | 84 | 84 |  | 14 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (37 clases, 776 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskUtilityLineUpPedWithCar | 5 | 2 | 3 | 37 |
| 2 | CTaskComplexGoToAttractor | 5 | 2 | 3 | 38 |
| 3 | CTaskComplexKillPedFromBoat | 7 | 2 | 5 | 39 |
| 4 | CTaskComplexGangJoinRespond | 7 | 2 | 5 | 40 |
| 5 | CTaskGoToVehicleAndLean | 7 | 2 | 5 | 46 |
| 6 | CTaskGangHasslePed | 5 | 2 | 3 | 50 |
| 7 | CTaskComplexWanderStandard | 9 | 3 | 6 | 38 |
| 8 | CCustomBuildingRenderer | 7 | 3 | 4 | 41 |
| 9 | CTaskComplexPlayHandSignalAnim | 9 | 3 | 6 | 41 |
| 10 | CRealTimeShadowManager | 7 | 5 | 2 | 38 |
| 11 | CMonsterTruck | 11 | 5 | 6 | 43 |
| 12 | CRope | 7 | 5 | 2 | 45 |
| 13 | CTaskComplexProstituteSolicit | 11 | 7 | 4 | 44 |
| 14 | CPedGeometryAnalyser | 38 | 9 | 29 | 44 |
| 15 | CTaskComplexKillCriminal | 11 | 9 | 2 | 44 |
| 16 | CIdleCam | 14 | 13 | 1 | 28 |
| 17 | CAEPoliceScannerAudioEntity | 17 | 13 | 4 | 29 |
| 18 | CPlane | 23 | 14 | 9 | 34 |
| 19 | CAEPedAudioEntity | 20 | 15 | 4 | 37 |
| 20 | CClothesBuilder | 23 | 15 | 8 | 47 |
| 21 | CPools | 19 | 16 | 3 | 27 |
| 22 | CAEStreamingChannel | 24 | 18 | 3 | 26 |
| 23 | CTrain | 40 | 20 | 20 | 49 |
| 24 | CBike | 40 | 22 | 18 | 36 |
| 25 | CRunningScript | 39 | 24 | 15 | 42 |
| 26 | CWaterLevel | 30 | 25 | 5 | 40 |
| 27 | CCarEnterExit | 32 | 27 | 5 | 30 |
| 28 | CAERadioTrackManager | 43 | 29 | 14 | 33 |
| 29 | CGroupEventHandler | 32 | 30 | 2 | 27 |
| 30 | CFont | 36 | 32 | 4 | 26 |
| 31 | CGangWars | 35 | 32 | 3 | 33 |
| 32 | CPostEffects | 38 | 35 | 3 | 33 |
| 33 | CRadar | 63 | 63 |  | 34 |
| 34 | CPopulation | 66 | 64 | 2 | 31 |
| 35 | CTheScripts | 69 | 69 |  | 29 |
| 36 | CEventHandler | 72 | 69 | 3 | 45 |
| 37 | CEntity | 70 | 70 |  | 35 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (33 clases, 933 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedGroupPlacer | 4 | 1 | 3 | 66 |
| 2 | CDecision | 3 | 1 | 2 | 82 |
| 3 | CTaskComplexFollowLeaderInFormation | 7 | 1 | 6 | 86 |
| 4 | CTaskSimpleGangDriveBy | 4 | 1 | 3 | 96 |
| 5 | CFormation | 9 | 1 | 8 | 147 |
| 6 | CProjectileInfo | 12 | 1 | 11 | 158 |
| 7 | CTaskComplexHitPedWithCar | 10 | 2 | 8 | 54 |
| 8 | CTaskGangHassleVehicle | 8 | 2 | 6 | 54 |
| 9 | CTaskComplexSmartFleePoint | 12 | 2 | 10 | 59 |
| 10 | CTaskComplexSunbathe | 8 | 2 | 6 | 59 |
| 11 | CPedDamageResponseCalculator | 7 | 2 | 5 | 114 |
| 12 | FxEmitterBP_c | 9 | 2 | 7 | 116 |
| 13 | InteriorGroup_c | 24 | 2 | 22 | 122 |
| 14 | CInterestingEvents | 7 | 2 | 5 | 177 |
| 15 | CTaskComplexArrestPed | 7 | 4 | 3 | 83 |
| 16 | CAEFrontendAudioEntity | 6 | 4 | 2 | 211 |
| 17 | CGenericGameStorage | 19 | 5 | 14 | 183 |
| 18 | CTaskSimpleFightingControl | 7 | 6 | 1 | 54 |
| 19 | CRoadBlocks | 8 | 7 | 1 | 57 |
| 20 | CAEScriptAudioEntity | 19 | 8 | 11 | 107 |
| 21 | CTaskComplexKillPedOnFootArmed | 11 | 9 | 2 | 51 |
| 22 | CTaskSimplePlayerOnFoot | 12 | 11 | 1 | 103 |
| 23 | CCam | 44 | 14 | 30 | 82 |
| 24 | CShadows | 31 | 21 | 10 | 59 |
| 25 | CHud | 38 | 30 | 8 | 57 |
| 26 | CWeapon | 41 | 35 | 6 | 60 |
| 27 | CCamera | 110 | 91 | 19 | 23 |
| 28 | CWorld | 94 | 94 |  | 35 |
| 29 | CStreaming | 94 | 94 |  | 40 |
| 30 | CAutomobile | 99 | 99 |  | 65 |
| 31 | CAEVehicleAudioEntity | 106 | 102 | 4 | 48 |
| 32 | CPed | 145 | 136 |  | 31 |
| 33 | CVehicle | 142 | 141 |  | 33 |

<a id="fuera-de-la-cola"></a>

## Fuera de la cola (no validables por pares)

| Clase | Función | Motivo |
|---|---|---|
| CAEAudioHardware | `Initialise` | crea canales y cargadores reales, llama a DirectSoundCreate8 y arranca hilos |
| CPlayerPed | `SetupPlayerPed` | construye un CPlayerPed real con pools, audio, inteligencia y grupos |
| Win | `NOTSA_WinMain` | punto de entrada del juego: no vuelve hasta cerrar |
| CCollision | `SphereVsEntity` | el EXE no la llama y lee una esfera local sin inicializar |

Se comprueban de forma indirecta arrancando el juego o con una prueba en partida.

<a id="como-se-ordena"></a>

## Cómo se ordena

Los niveles salen de una regla fija, no de una opinión:

- **Nivel 1:** hasta 12 funciones por validar y hasta 35 líneas por función.
- **Nivel 2:** más de 12 funciones y hasta 25 líneas por función.
- **Nivel 4:** 90 o más funciones, o más de 50 líneas por función.
- **Nivel 3:** el resto.

"Líneas/func." es una **estimación**: líneas del `.cpp` que más define la clase, repartidas entre las clases que define ese fichero y divididas por las funciones reconstruidas de la clase. No es un coste medido y no promete horas ni fechas; `?` indica que no se pudo medir. Dentro de cada nivel el orden es por funciones por validar y después por líneas por función.

Para regenerar este documento: `python contrib/validation_queue.py`, o `python contrib/sync_docs.py`, que además sincroniza el resto de Markdown derivado de `hooks.csv`.
