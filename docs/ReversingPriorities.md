<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 09/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.446 |
| Reconstruidas | 8.446 |
| Validadas | 6.881 |
| Reconstruidas sin validar | 1.565 |
| De ellas, en esta cola (validables por pares) | 1.561 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 8 | 24 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 9 | 461 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 19 | 511 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 16 | 565 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **52** | **1561** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (8 clases, 24 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPad | 75 | 1 |  | 21 |
| 2 | CTaskGoToVehicleAndLean | 7 | 2 |  | 19 |
| 3 | CTaskComplexPlayHandSignalAnim | 9 | 2 |  | 24 |
| 4 | CPedGeometryAnalyser | 38 | 2 |  | 26 |
| 5 | CTaskGangHasslePed | 5 | 2 |  | 29 |
| 6 | CTaskGangHassleVehicle | 8 | 2 |  | 32 |
| 7 | CRealTimeShadowManager | 7 | 4 |  | 30 |
| 8 | CPools | 19 | 9 |  | 26 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (9 clases, 461 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CAEPoliceScannerAudioEntity | 17 | 13 |  | 25 |
| 2 | CAEStreamingChannel | 24 | 18 |  | 25 |
| 3 | FxSystem_c | 35 | 31 |  | 15 |
| 4 | CFont | 36 | 32 |  | 25 |
| 5 | CPathFind | 56 | 54 |  | 23 |
| 6 | CCheat | 72 | 72 |  | 17 |
| 7 | CVehicleModelInfo | 75 | 75 |  | 22 |
| 8 | CAudioEngine | 82 | 82 |  | 10 |
| 9 | CVisibilityPlugins | 84 | 84 |  | 14 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (19 clases, 511 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CAEFrontendAudioEntity | 6 | 3 |  | 44 |
| 2 | CTaskSimpleFightingControl | 7 | 3 |  | 50 |
| 3 | CTaskComplexProstituteSolicit | 11 | 4 |  | 40 |
| 4 | CGenericGameStorage | 19 | 5 |  | 48 |
| 5 | CIdleCam | 14 | 13 |  | 33 |
| 6 | CAEPedAudioEntity | 20 | 15 |  | 40 |
| 7 | CClothesBuilder | 23 | 15 |  | 42 |
| 8 | CTrain | 40 | 20 |  | 38 |
| 9 | CRunningScript | 39 | 24 |  | 33 |
| 10 | CWaterLevel | 30 | 25 |  | 48 |
| 11 | CCarEnterExit | 32 | 27 |  | 29 |
| 12 | CAERadioTrackManager | 43 | 29 |  | 40 |
| 13 | CGroupEventHandler | 32 | 30 |  | 29 |
| 14 | CGangWars | 35 | 32 |  | 34 |
| 15 | CPickups | 37 | 35 |  | 26 |
| 16 | CPostEffects | 38 | 35 |  | 32 |
| 17 | CRadar | 63 | 63 |  | 34 |
| 18 | CPopulation | 66 | 64 |  | 34 |
| 19 | CTheScripts | 69 | 69 |  | 30 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (16 clases, 565 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskUtilityLineUpPedWithCar | 5 | 2 |  | 60 |
| 2 | CRoadBlocks | 8 | 7 |  | 70 |
| 3 | CAEScriptAudioEntity | 19 | 8 |  | 54 |
| 4 | CTaskComplexKillPedOnFootArmed | 11 | 8 |  | 62 |
| 5 | CTaskComplexKillCriminal | 11 | 8 |  | 64 |
| 6 | CTaskSimplePlayerOnFoot | 12 | 10 |  | 100 |
| 7 | CCam | 44 | 14 |  | 138 |
| 8 | CPlane | 23 | 14 |  | 142 |
| 9 | CShadows | 31 | 21 |  | 56 |
| 10 | CBike | 40 | 22 |  | 151 |
| 11 | CHud | 38 | 30 |  | 52 |
| 12 | CWeapon | 41 | 35 |  | 63 |
| 13 | CCamera | 110 | 91 |  | 37 |
| 14 | CStreaming | 94 | 94 |  | 40 |
| 15 | CAutomobile | 99 | 99 |  | 65 |
| 16 | CAEVehicleAudioEntity | 106 | 102 |  | 47 |

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
