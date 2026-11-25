<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Prioridades técnicas de reversing

[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)
<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->

> **Corte 24/11/2026.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).

**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada vinculada y no acredita por sí sola que siga vigente para el árbol actual.

| Dimensión | Cifra |
| --- | ---: |
| Entradas en el catálogo | 13.511 |
| Reconstruidas | 13.511 |
| Validadas | 11.800 |
| Reconstruidas sin validar | 1.711 |
| De ellas, en esta cola (validables por pares) | 1.711 |
| Sin reconstruir (en 0 clases incompletas) | 0 |

Validar no exige que la clase esté completa: se valida cada función ya reconstruida. "Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.

**Contexto del 22 al 24/11/2026.** La cola es la lista de pendientes de validación tal como la dice `hooks.csv`; en esos días no se añadieron bancos nuevos: el trabajo fue el ejecutable independiente, las auditorías de «dato en dos sitios» y la comparación con el original (resumen con cada documento de ejecución en [la ruta hacia el EXE independiente](RutaExeIndependiente.md#actualización-del-24112026-estado-medido-del-ejecutable-independiente)). Las funciones tocadas por ese trabajo (CRT del montón, ramas x87 de `floor`/`ceil`/`pow`, ayudantes de RenderWare, `CAEMFDecoder` con Media Foundation, combos de melee, lista libre de muebles, fundidos del HUD, ruta de scripts, comandos de gimnasio) cambiaron de código (tras su evidencia, si la tenían): su validación futura debe repetirse sobre el árbol actual. Quien valide una función de esa lista debe leer antes el documento de ejecución correspondiente: ahí está lo que se midió y lo que no. Como esta guía sale de `contrib/validation_queue.py`, este párrafo se edita allí.

**Navegación:** [🟢 Nivel 1](#nivel-1) · [🟡 Nivel 2](#nivel-2) · [🟠 Nivel 3](#nivel-3) · [🔴 Nivel 4](#nivel-4) · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)

| Nivel | Clases | Por validar | Enfoque |
| --- | :---: | :---: | --- |
| [🟢 Nivel 1: muy barato](#nivel-1) | 15 | 25 | Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda. |
| [🟡 Nivel 2: baratas por función, pero muchas](#nivel-2) | 0 | 0 | Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos. |
| [🟠 Nivel 3: moderadas](#nivel-3) | 31 | 1447 | Estado compartido, pools, cámara, audio o RenderWare; fixtures propios. |
| [🔴 Nivel 4: caras](#nivel-4) | 6 | 239 | Muchas funciones o muy largas; partir en varios bancos y dejar para el final. |
| **Total** | **52** | **1711** | |

<a id="nivel-1"></a>

## 🟢 Nivel 1: muy barato (15 clases, 25 funciones)

Criterio: hasta 12 funciones por validar y hasta 35 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CTaskSimpleTurn180 | 8 | 1 |  | 11 |
| 2 | CQuaternion | 13 | 1 |  | 15 |
| 3 | tUserList | 3 | 1 |  | 16 |
| 4 | CSprite2d | 36 | 1 |  | 17 |
| 5 | CPad | 103 | 1 |  | 18 |
| 6 | CScripted2dEffects | 7 | 1 |  | 21 |
| 7 | CGarages | 31 | 1 |  | 23 |
| 8 | CTaskSimpleArrestPed | 9 | 1 |  | 23 |
| 9 | LibJpegMemory | 22 | 1 |  | 25 |
| 10 | FxInfoAttractLine_c | 4 | 1 |  | 30 |
| 11 | CTaskSimpleAnim | 5 | 1 |  | 34 |
| 12 | FxInterpInfoFloat_c | 5 | 1 |  | 35 |
| 13 | StdException | 3 | 3 |  | 31 |
| 14 | CAEMFDecoder | 12 | 5 |  | 17 |
| 15 | CAudioZones | 14 | 5 |  | 27 |

<a id="nivel-2"></a>

## 🟡 Nivel 2: baratas por función, pero muchas (0 clases, 0 funciones)

Criterio: más de 12 funciones y hasta 25 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|

<a id="nivel-3"></a>

## 🟠 Nivel 3: moderadas (31 clases, 1447 funciones)

Criterio: el resto.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CGame | 26 | 1 |  | 37 |
| 2 | CPed | 157 | 1 |  | 37 |
| 3 | CCamera | 127 | 1 |  | 38 |
| 4 | CSpecialFX | 8 | 1 |  | 38 |
| 5 | CPhoneInfo | 3 | 1 |  | 40 |
| 6 | CCollisionPlugin | 3 | 1 |  | 45 |
| 7 | CVehicle | 204 | 1 |  | 47 |
| 8 | RtSlerp | 1 | 1 |  | ? |
| 9 | DelayLoadThunks | 2 | 2 |  | ? |
| 10 | Input | 14 | 3 |  | ? |
| 11 | RwHelper | 49 | 3 |  | ? |
| 12 | RtBMP | 4 | 4 |  | ? |
| 13 | RtPNG | 7 | 7 |  | ? |
| 14 | RwMathX87 | 7 | 7 |  | ? |
| 15 | RtQuat | 8 | 8 |  | ? |
| 16 | D3DX9 | 17 | 11 |  | ? |
| 17 | ExeCrtHeap | 13 | 13 |  | ? |
| 18 | ZlibInflate | 14 | 14 |  | ? |
| 19 | RtDict | 18 | 16 |  | ? |
| 20 | ImportThunks | 17 | 17 |  | ? |
| 21 | ZlibDeflate | 25 | 25 |  | ? |
| 22 | RtAnim | 30 | 27 |  | 27 |
| 23 | RpHAnim | 35 | 34 |  | ? |
| 24 | RpUVAnim | 42 | 40 |  | ? |
| 25 | RpMatFX | 73 | 73 |  | ? |
| 26 | RpSkin | 76 | 75 |  | ? |
| 27 | LibPngWrite | 78 | 78 |  | ? |
| 28 | RwD3D9 | 147 | 147 |  | ? |
| 29 | RwPlCore | 192 | 178 |  | ? |
| 30 | RwCore | 342 | 306 |  | ? |
| 31 | RpWorld | 454 | 351 |  | ? |

<a id="nivel-4"></a>

## 🔴 Nivel 4: caras (6 clases, 239 funciones)

Criterio: 90 o más funciones, o más de 50 líneas por función.

| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |
|--:|---|--:|--:|--:|--:|
| 1 | CLines | 1 | 1 |  | 107 |
| 2 | RwD3D9RasterDriver | 16 | 2 |  | 52 |
| 3 | CPhysical | 65 | 3 |  | 79 |
| 4 | D3D9RasterStandards | 12 | 12 |  | 71 |
| 5 | CRunningScript | 66 | 23 |  | 51 |
| 6 | ExeCrt | 198 | 198 |  | 0 |

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
