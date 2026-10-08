# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 07/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.447 |
| Reconstruidas | 8.040 |
| Validadas | 6.329 |
| Reconstruidas sin validar | 1.711 |
| De ellas, en esta cola (validables por pares) | 1.707 |
| Sin reconstruir (en 35 clases incompletas) | 407 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 5 | 13 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 10 | 536 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 30 | 600 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 28 | 558 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **73** | **1707** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (5 clases, 13 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CDecision | 3 | 1 |  | 35 |
| 2 | CTaskComplexGoToAttractor | 5 | 2 |  | 17 |
| 3 | CTaskGangHasslePed | 5 | 2 |  | 29 |
| 4 | CCustomBuildingRenderer | 7 | 3 |  | 16 |
| 5 | CRealTimeShadowManager | 7 | 5 |  | 30 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (10 clases, 536 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CAEPoliceScannerAudioEntity | 17 | 13 |  | 25 |
| 2 | CAEStreamingChannel | 24 | 18 |  | 25 |
| 3 | FxSystem_c | 35 | 31 |  | 15 |
| 4 | CFont | 36 | 32 |  | 25 |
| 5 | CPathFind | 56 | 54 |  | 23 |
| 6 | CCheat | 72 | 72 |  | 17 |
| 7 | CPad | 75 | 75 |  | 19 |
| 8 | CVehicleModelInfo | 75 | 75 |  | 22 |
| 9 | CAudioEngine | 82 | 82 |  | 10 |
| 10 | CVisibilityPlugins | 84 | 84 |  | 14 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (30 clases, 600 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskComplexKillPedFromBoat | 7 | 2 | 5 | 39 |
| 2 | CTaskComplexGangJoinRespond | 7 | 2 | 5 | 40 |
| 3 | CTaskGoToVehicleAndLean | 7 | 2 | 5 | 46 |
| 4 | CTaskComplexWanderStandard | 9 | 3 | 6 | 38 |
| 5 | CTaskComplexPlayHandSignalAnim | 9 | 3 | 6 | 41 |
| 6 | CAEFrontendAudioEntity | 6 | 4 |  | 42 |
| 7 | CTaskComplexArrestPed | 7 | 4 |  | 45 |
| 8 | CRope | 7 | 5 |  | 42 |
| 9 | CMonsterTruck | 11 | 5 | 6 | 43 |
| 10 | CTaskSimpleFightingControl | 7 | 6 |  | 46 |
| 11 | CTaskComplexProstituteSolicit | 11 | 7 |  | 39 |
| 12 | CPedGeometryAnalyser | 38 | 9 | 29 | 44 |
| 13 | CIdleCam | 14 | 13 |  | 33 |
| 14 | CPlane | 23 | 14 | 9 | 34 |
| 15 | CAEPedAudioEntity | 20 | 15 |  | 40 |
| 16 | CClothesBuilder | 23 | 15 | 8 | 47 |
| 17 | CPools | 19 | 16 |  | 26 |
| 18 | CTrain | 40 | 20 | 20 | 49 |
| 19 | CBike | 40 | 22 | 18 | 36 |
| 20 | CRunningScript | 39 | 24 | 15 | 42 |
| 21 | CWaterLevel | 30 | 25 | 5 | 40 |
| 22 | CCarEnterExit | 32 | 27 | 5 | 30 |
| 23 | CAERadioTrackManager | 43 | 29 | 14 | 33 |
| 24 | CGroupEventHandler | 32 | 30 |  | 29 |
| 25 | CGangWars | 35 | 32 |  | 34 |
| 26 | CPickups | 37 | 35 |  | 26 |
| 27 | CPostEffects | 38 | 35 |  | 32 |
| 28 | CRadar | 63 | 63 |  | 34 |
| 29 | CPopulation | 66 | 64 |  | 34 |
| 30 | CTheScripts | 69 | 69 |  | 30 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (28 clases, 558 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedGroupPlacer | 4 | 1 |  | 51 |
| 2 | CTaskSimpleGangDriveBy | 4 | 1 |  | 68 |
| 3 | CTaskComplexFollowLeaderInFormation | 7 | 1 | 6 | 86 |
| 4 | CFormation | 9 | 1 | 8 | 147 |
| 5 | CProjectileInfo | 12 | 1 | 11 | 158 |
| 6 | CTaskComplexHitPedWithCar | 10 | 2 | 8 | 54 |
| 7 | CTaskGangHassleVehicle | 8 | 2 | 6 | 54 |
| 8 | CTaskComplexSmartFleePoint | 12 | 2 | 10 | 59 |
| 9 | CTaskComplexSunbathe | 8 | 2 | 6 | 59 |
| 10 | CTaskUtilityLineUpPedWithCar | 5 | 2 |  | 60 |
| 11 | CPedDamageResponseCalculator | 7 | 2 | 5 | 114 |
| 12 | FxEmitterBP_c | 9 | 2 | 7 | 116 |
| 13 | InteriorGroup_c | 24 | 2 | 22 | 122 |
| 14 | CInterestingEvents | 7 | 2 | 5 | 177 |
| 15 | CGenericGameStorage | 19 | 5 | 14 | 183 |
| 16 | CRoadBlocks | 8 | 7 |  | 70 |
| 17 | CAEScriptAudioEntity | 19 | 8 | 11 | 107 |
| 18 | CTaskComplexKillPedOnFootArmed | 11 | 9 |  | 62 |
| 19 | CTaskComplexKillCriminal | 11 | 9 |  | 64 |
| 20 | CTaskSimplePlayerOnFoot | 12 | 11 |  | 100 |
| 21 | CCam | 44 | 14 | 30 | 82 |
| 22 | CShadows | 31 | 21 | 10 | 59 |
| 23 | CHud | 38 | 30 | 8 | 57 |
| 24 | CWeapon | 41 | 35 | 6 | 60 |
| 25 | CCamera | 110 | 91 | 19 | 23 |
| 26 | CStreaming | 94 | 94 |  | 40 |
| 27 | CAutomobile | 99 | 99 |  | 65 |
| 28 | CAEVehicleAudioEntity | 106 | 102 |  | 47 |

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
