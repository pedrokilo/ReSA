# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 09/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.446 |
| Reconstruidas | 8.446 |
| Validadas | 6.916 |
| Reconstruidas sin validar | 1.530 |
| De ellas, en esta cola (validables por pares) | 1.529 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 1 | 9 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 9 | 461 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 15 | 496 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 15 | 563 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **40** | **1529** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (1 clases, 9 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CPools | 19 | 9 |  | 26 |

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

## 🟠 Nivel 3: moderadas (15 clases, 496 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CIdleCam | 14 | 13 |  | 33 |
| 2 | CAEPedAudioEntity | 20 | 15 |  | 40 |
| 3 | CClothesBuilder | 23 | 15 |  | 42 |
| 4 | CTrain | 40 | 20 |  | 38 |
| 5 | CRunningScript | 39 | 24 |  | 33 |
| 6 | CWaterLevel | 30 | 25 |  | 48 |
| 7 | CCarEnterExit | 32 | 27 |  | 29 |
| 8 | CAERadioTrackManager | 43 | 29 |  | 40 |
| 9 | CGroupEventHandler | 32 | 30 |  | 29 |
| 10 | CGangWars | 35 | 32 |  | 34 |
| 11 | CPickups | 37 | 35 |  | 26 |
| 12 | CPostEffects | 38 | 35 |  | 32 |
| 13 | CRadar | 63 | 63 |  | 34 |
| 14 | CPopulation | 66 | 64 |  | 34 |
| 15 | CTheScripts | 69 | 69 |  | 30 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (15 clases, 563 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CRoadBlocks | 8 | 7 |  | 70 |
| 2 | CAEScriptAudioEntity | 19 | 8 |  | 54 |
| 3 | CTaskComplexKillPedOnFootArmed | 11 | 8 |  | 62 |
| 4 | CTaskComplexKillCriminal | 11 | 8 |  | 64 |
| 5 | CTaskSimplePlayerOnFoot | 12 | 10 |  | 100 |
| 6 | CCam | 44 | 14 |  | 138 |
| 7 | CPlane | 23 | 14 |  | 142 |
| 8 | CShadows | 31 | 21 |  | 56 |
| 9 | CBike | 40 | 22 |  | 151 |
| 10 | CHud | 38 | 30 |  | 52 |
| 11 | CWeapon | 41 | 35 |  | 63 |
| 12 | CCamera | 110 | 91 |  | 37 |
| 13 | CStreaming | 94 | 94 |  | 40 |
| 14 | CAutomobile | 99 | 99 |  | 65 |
| 15 | CAEVehicleAudioEntity | 106 | 102 |  | 47 |

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
