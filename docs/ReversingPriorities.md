# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 05/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.441 |
| Reconstruidas | 7.946 |
| Validadas | 5.662 |
| Reconstruidas sin validar | 2.284 |
| De ellas, en esta cola (validables por pares) | 2.280 |
| Sin reconstruir (en 70 clases incompletas) | 495 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 0 | 0 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 9 | 569 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 38 | 777 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 34 | 934 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **81** | **2280** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (0 clases, 0 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|

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

## 🟠 Nivel 3: moderadas (38 clases, 777 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CQuadBike | 12 | 1 |  | 42 |
| 2 | CTaskUtilityLineUpPedWithCar | 5 | 2 | 3 | 37 |
| 3 | CTaskComplexGoToAttractor | 5 | 2 | 3 | 38 |
| 4 | CTaskComplexKillPedFromBoat | 7 | 2 | 5 | 39 |
| 5 | CTaskComplexGangJoinRespond | 7 | 2 | 5 | 40 |
| 6 | CTaskGoToVehicleAndLean | 7 | 2 | 5 | 46 |
| 7 | CTaskGangHasslePed | 5 | 2 | 3 | 50 |
| 8 | CTaskComplexWanderStandard | 9 | 3 | 6 | 38 |
| 9 | CCustomBuildingRenderer | 7 | 3 | 4 | 41 |
| 10 | CTaskComplexPlayHandSignalAnim | 9 | 3 | 6 | 41 |
| 11 | CRealTimeShadowManager | 7 | 5 | 2 | 38 |
| 12 | CMonsterTruck | 11 | 5 | 6 | 43 |
| 13 | CRope | 7 | 5 | 2 | 45 |
| 14 | CTaskComplexProstituteSolicit | 11 | 7 | 4 | 44 |
| 15 | CPedGeometryAnalyser | 38 | 9 | 29 | 44 |
| 16 | CTaskComplexKillCriminal | 11 | 9 | 2 | 44 |
| 17 | CIdleCam | 14 | 13 | 1 | 28 |
| 18 | CAEPoliceScannerAudioEntity | 17 | 13 | 4 | 29 |
| 19 | CPlane | 23 | 14 | 9 | 34 |
| 20 | CAEPedAudioEntity | 20 | 15 | 4 | 37 |
| 21 | CClothesBuilder | 23 | 15 | 8 | 47 |
| 22 | CPools | 19 | 16 | 3 | 27 |
| 23 | CAEStreamingChannel | 24 | 18 | 3 | 26 |
| 24 | CTrain | 40 | 20 | 20 | 49 |
| 25 | CBike | 40 | 22 | 18 | 36 |
| 26 | CRunningScript | 39 | 24 | 15 | 42 |
| 27 | CWaterLevel | 30 | 25 | 5 | 40 |
| 28 | CCarEnterExit | 32 | 27 | 5 | 30 |
| 29 | CAERadioTrackManager | 43 | 29 | 14 | 33 |
| 30 | CGroupEventHandler | 32 | 30 | 2 | 27 |
| 31 | CFont | 36 | 32 | 4 | 26 |
| 32 | CGangWars | 35 | 32 | 3 | 33 |
| 33 | CPostEffects | 38 | 35 | 3 | 33 |
| 34 | CRadar | 63 | 63 |  | 34 |
| 35 | CPopulation | 66 | 64 | 2 | 31 |
| 36 | CTheScripts | 69 | 69 |  | 29 |
| 37 | CEventHandler | 72 | 69 | 3 | 45 |
| 38 | CEntity | 70 | 70 |  | 35 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (34 clases, 934 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPedGroupPlacer | 4 | 1 | 3 | 66 |
| 2 | CDecision | 3 | 1 | 2 | 82 |
| 3 | CTaskComplexFollowLeaderInFormation | 7 | 1 | 6 | 86 |
| 4 | CTaskSimpleGangDriveBy | 4 | 1 | 3 | 96 |
| 5 | CBmx | 10 | 1 |  | 114 |
| 6 | CFormation | 9 | 1 | 8 | 147 |
| 7 | CProjectileInfo | 12 | 1 | 11 | 158 |
| 8 | CTaskComplexHitPedWithCar | 10 | 2 | 8 | 54 |
| 9 | CTaskGangHassleVehicle | 8 | 2 | 6 | 54 |
| 10 | CTaskComplexSmartFleePoint | 12 | 2 | 10 | 59 |
| 11 | CTaskComplexSunbathe | 8 | 2 | 6 | 59 |
| 12 | CPedDamageResponseCalculator | 7 | 2 | 5 | 114 |
| 13 | FxEmitterBP_c | 9 | 2 | 7 | 116 |
| 14 | InteriorGroup_c | 24 | 2 | 22 | 122 |
| 15 | CInterestingEvents | 7 | 2 | 5 | 177 |
| 16 | CTaskComplexArrestPed | 7 | 4 | 3 | 83 |
| 17 | CAEFrontendAudioEntity | 6 | 4 | 2 | 211 |
| 18 | CGenericGameStorage | 19 | 5 | 14 | 183 |
| 19 | CTaskSimpleFightingControl | 7 | 6 | 1 | 54 |
| 20 | CRoadBlocks | 8 | 7 | 1 | 57 |
| 21 | CAEScriptAudioEntity | 19 | 8 | 11 | 107 |
| 22 | CTaskComplexKillPedOnFootArmed | 11 | 9 | 2 | 51 |
| 23 | CTaskSimplePlayerOnFoot | 12 | 11 | 1 | 103 |
| 24 | CCam | 44 | 14 | 30 | 82 |
| 25 | CShadows | 31 | 21 | 10 | 59 |
| 26 | CHud | 38 | 30 | 8 | 57 |
| 27 | CWeapon | 41 | 35 | 6 | 60 |
| 28 | CCamera | 110 | 91 | 19 | 23 |
| 29 | CWorld | 94 | 94 |  | 35 |
| 30 | CStreaming | 94 | 94 |  | 40 |
| 31 | CAutomobile | 99 | 99 |  | 65 |
| 32 | CAEVehicleAudioEntity | 106 | 102 | 4 | 48 |
| 33 | CPed | 145 | 136 |  | 31 |
| 34 | CVehicle | 142 | 141 |  | 33 |

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
