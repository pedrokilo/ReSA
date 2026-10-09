# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 09/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.446 |
| Reconstruidas | 8.446 |
| Validadas | 7.155 |
| Reconstruidas sin validar | 1.291 |
| De ellas, en esta cola (validables por pares) | 1.290 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 4 | 19 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 7 | 430 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 10 | 359 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 12 | 482 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **33** | **1290** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (4 clases, 19 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CRunningScript | 39 | 2 |  | 34 |
| 2 | CCarEnterExit | 32 | 5 |  | 32 |
| 3 | CIdleCam | 14 | 5 |  | 34 |
| 4 | CPools | 19 | 7 |  | 26 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (7 clases, 430 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | FxSystem_c | 35 | 31 |  | 15 |
| 2 | CFont | 36 | 32 |  | 25 |
| 3 | CPathFind | 56 | 54 |  | 23 |
| 4 | CCheat | 72 | 72 |  | 17 |
| 5 | CVehicleModelInfo | 75 | 75 |  | 22 |
| 6 | CAudioEngine | 82 | 82 |  | 10 |
| 7 | CVisibilityPlugins | 84 | 84 |  | 14 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (10 clases, 359 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTrain | 40 | 1 |  | 38 |
| 2 | CClothesBuilder | 23 | 1 |  | 44 |
| 3 | CAERadioTrackManager | 43 | 29 |  | 40 |
| 4 | CGroupEventHandler | 32 | 30 |  | 29 |
| 5 | CGangWars | 35 | 32 |  | 34 |
| 6 | CPickups | 37 | 35 |  | 26 |
| 7 | CPostEffects | 38 | 35 |  | 32 |
| 8 | CRadar | 63 | 63 |  | 34 |
| 9 | CPopulation | 66 | 64 |  | 34 |
| 10 | CTheScripts | 69 | 69 |  | 30 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (12 clases, 482 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CShadows | 31 | 2 |  | 59 |
| 2 | CRoadBlocks | 8 | 2 |  | 76 |
| 3 | CWaterLevel | 30 | 4 |  | 54 |
| 4 | CBike | 40 | 4 |  | 152 |
| 5 | CTaskSimplePlayerOnFoot | 12 | 5 |  | 100 |
| 6 | CCam | 44 | 14 |  | 138 |
| 7 | CHud | 38 | 30 |  | 52 |
| 8 | CWeapon | 41 | 35 |  | 63 |
| 9 | CCamera | 110 | 91 |  | 37 |
| 10 | CStreaming | 94 | 94 |  | 40 |
| 11 | CAutomobile | 99 | 99 |  | 65 |
| 12 | CAEVehicleAudioEntity | 106 | 102 |  | 47 |

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
