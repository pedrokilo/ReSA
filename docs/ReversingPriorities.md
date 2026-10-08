# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 08/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.446 |
| Reconstruidas | 8.446 |
| Validadas | 6.736 |
| Reconstruidas sin validar | 1.710 |
| De ellas, en esta cola (validables por pares) | 1.706 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 18 | 49 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 10 | 536 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 25 | 548 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 20 | 573 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **73** | **1706** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (18 clases, 49 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | InteriorGroup_c | 24 | 1 |  | 23 |
| 2 | CTaskComplexFollowLeaderInFormation | 7 | 1 |  | 24 |
| 3 | CDecision | 3 | 1 |  | 35 |
| 4 | CTaskComplexHitPedWithCar | 10 | 2 |  | 14 |
| 5 | CTaskComplexKillPedFromBoat | 7 | 2 |  | 14 |
| 6 | CTaskComplexGangJoinRespond | 7 | 2 |  | 15 |
| 7 | CTaskComplexGoToAttractor | 5 | 2 |  | 17 |
| 8 | CTaskComplexSmartFleePoint | 12 | 2 |  | 18 |
| 9 | CTaskGoToVehicleAndLean | 7 | 2 |  | 19 |
| 10 | CTaskComplexSunbathe | 8 | 2 |  | 29 |
| 11 | CTaskGangHasslePed | 5 | 2 |  | 29 |
| 12 | CTaskGangHassleVehicle | 8 | 2 |  | 32 |
| 13 | CCustomBuildingRenderer | 7 | 3 |  | 16 |
| 14 | CTaskComplexPlayHandSignalAnim | 9 | 3 |  | 24 |
| 15 | CTaskComplexWanderStandard | 9 | 3 |  | 24 |
| 16 | CMonsterTruck | 11 | 5 |  | 30 |
| 17 | CRealTimeShadowManager | 7 | 5 |  | 30 |
| 18 | CPedGeometryAnalyser | 38 | 9 |  | 25 |

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

## 🟠 Nivel 3: moderadas (25 clases, 548 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CProjectileInfo | 12 | 1 |  | 50 |
| 2 | FxEmitterBP_c | 9 | 2 |  | 42 |
| 3 | CInterestingEvents | 7 | 2 |  | 49 |
| 4 | CAEFrontendAudioEntity | 6 | 4 |  | 42 |
| 5 | CTaskComplexArrestPed | 7 | 4 |  | 45 |
| 6 | CRope | 7 | 5 |  | 42 |
| 7 | CGenericGameStorage | 19 | 5 |  | 47 |
| 8 | CTaskSimpleFightingControl | 7 | 6 |  | 46 |
| 9 | CTaskComplexProstituteSolicit | 11 | 7 |  | 39 |
| 10 | CIdleCam | 14 | 13 |  | 33 |
| 11 | CAEPedAudioEntity | 20 | 15 |  | 40 |
| 12 | CClothesBuilder | 23 | 15 |  | 42 |
| 13 | CPools | 19 | 16 |  | 26 |
| 14 | CTrain | 40 | 20 |  | 38 |
| 15 | CRunningScript | 39 | 24 |  | 33 |
| 16 | CWaterLevel | 30 | 25 |  | 48 |
| 17 | CCarEnterExit | 32 | 27 |  | 29 |
| 18 | CAERadioTrackManager | 43 | 29 |  | 40 |
| 19 | CGroupEventHandler | 32 | 30 |  | 29 |
| 20 | CGangWars | 35 | 32 |  | 34 |
| 21 | CPickups | 37 | 35 |  | 26 |
| 22 | CPostEffects | 38 | 35 |  | 32 |
| 23 | CRadar | 63 | 63 |  | 34 |
| 24 | CPopulation | 66 | 64 |  | 34 |
| 25 | CTheScripts | 69 | 69 |  | 30 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (20 clases, 573 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedGroupPlacer | 4 | 1 |  | 51 |
| 2 | CFormation | 9 | 1 |  | 63 |
| 3 | CTaskSimpleGangDriveBy | 4 | 1 |  | 68 |
| 4 | CPedDamageResponseCalculator | 6 | 2 |  | 54 |
| 5 | CTaskUtilityLineUpPedWithCar | 5 | 2 |  | 60 |
| 6 | CRoadBlocks | 8 | 7 |  | 70 |
| 7 | CAEScriptAudioEntity | 19 | 8 |  | 54 |
| 8 | CTaskComplexKillPedOnFootArmed | 11 | 9 |  | 62 |
| 9 | CTaskComplexKillCriminal | 11 | 9 |  | 64 |
| 10 | CTaskSimplePlayerOnFoot | 12 | 11 |  | 100 |
| 11 | CCam | 44 | 14 |  | 138 |
| 12 | CPlane | 23 | 14 |  | 142 |
| 13 | CShadows | 31 | 21 |  | 56 |
| 14 | CBike | 40 | 22 |  | 151 |
| 15 | CHud | 38 | 30 |  | 52 |
| 16 | CWeapon | 41 | 35 |  | 63 |
| 17 | CCamera | 110 | 91 |  | 37 |
| 18 | CStreaming | 94 | 94 |  | 40 |
| 19 | CAutomobile | 99 | 99 |  | 65 |
| 20 | CAEVehicleAudioEntity | 106 | 102 |  | 47 |

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
