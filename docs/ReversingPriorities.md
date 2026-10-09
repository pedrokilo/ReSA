# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 09/10/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 8.446 |
| Reconstruidas | 8.446 |
| Validadas | 7.047 |
| Reconstruidas sin validar | 1.399 |
| De ellas, en esta cola (validables por pares) | 1.398 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 4 | 19 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 8 | 443 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 12 | 406 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 13 | 530 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **37** | **1398** | |

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

## 🟡 Nivel 2: baratas por función, pero muchas (8 clases, 443 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CAEPoliceScannerAudioEntity | 17 | 13 |  | 25 |
| 2 | FxSystem_c | 35 | 31 |  | 15 |
| 3 | CFont | 36 | 32 |  | 25 |
| 4 | CPathFind | 56 | 54 |  | 23 |
| 5 | CCheat | 72 | 72 |  | 17 |
| 6 | CVehicleModelInfo | 75 | 75 |  | 22 |
| 7 | CAudioEngine | 82 | 82 |  | 10 |
| 8 | CVisibilityPlugins | 84 | 84 |  | 14 |

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (12 clases, 406 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTrain | 40 | 1 |  | 38 |
| 2 | CAEPedAudioEntity | 20 | 15 |  | 41 |
| 3 | CClothesBuilder | 23 | 15 |  | 44 |
| 4 | CAEStreamingChannel | 24 | 18 |  | 26 |
| 5 | CAERadioTrackManager | 43 | 29 |  | 40 |
| 6 | CGroupEventHandler | 32 | 30 |  | 29 |
| 7 | CGangWars | 35 | 32 |  | 34 |
| 8 | CPickups | 37 | 35 |  | 26 |
| 9 | CPostEffects | 38 | 35 |  | 32 |
| 10 | CRadar | 63 | 63 |  | 34 |
| 11 | CPopulation | 66 | 64 |  | 34 |
| 12 | CTheScripts | 69 | 69 |  | 30 |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (13 clases, 530 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CRoadBlocks | 8 | 2 |  | 76 |
| 2 | CBike | 40 | 4 |  | 152 |
| 3 | CTaskSimplePlayerOnFoot | 12 | 5 |  | 100 |
| 4 | CAEScriptAudioEntity | 19 | 8 |  | 54 |
| 5 | CCam | 44 | 14 |  | 138 |
| 6 | CShadows | 31 | 21 |  | 59 |
| 7 | CWaterLevel | 30 | 25 |  | 54 |
| 8 | CHud | 38 | 30 |  | 52 |
| 9 | CWeapon | 41 | 35 |  | 63 |
| 10 | CCamera | 110 | 91 |  | 37 |
| 11 | CStreaming | 94 | 94 |  | 40 |
| 12 | CAutomobile | 99 | 99 |  | 65 |
| 13 | CAEVehicleAudioEntity | 106 | 102 |  | 47 |

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
