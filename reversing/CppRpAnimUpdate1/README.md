<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppRpAnimUpdate1: RpAnimBlendClumpUpdateAnimations (0x4D34F0) y FrameUpdateCallBackOffscreen (0x4D2E10)

Estado: PREPARADO, SIN COMPILAR NI EJECUTAR. Requiere aplicar antes `rpw.patch` (reescribe `FrameUpdateCallBackOffscreen` y lo registra como hook; sin el parche la sonda se detiene con "hook not found").

Ejecutar: `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta> CppRpAnimUpdate1`.

## QuÃ© cubre y por quÃ©

El banco `RpAnim43` solo valida las cuatro `FrameUpdateCallBack{Compressed,}{Skinned,NonSkinned}` (0x4D32D0/0x4D2E40/0x4D30A0/0x4D2B90) con sus seis puentes de velocidad. El banco antiguo `RpAnimBlend` (652 pares) ejerce `RpAnimBlendClumpUpdateAnimations`, pero con tolerancia 1e-5, valores benignos y sin estrÃ©s numÃ©rico: no distingue la cadena x87 de 24 bits de una cadena en doubles de SSE. Este banco compara el EXE con la DLL con igualdad exacta de bytes.

| FunciÃ³n | Variante | Pares |
|---|---|---:|
| `RpAnimBlendClumpUpdateAnimations` | callbacks del EXE en ambos lados | 4096 |
| `RpAnimBlendClumpUpdateAnimations-CppCallbacks` | el candidato activa ademÃ¡s los cinco callbacks en C++ | 2048 |
| `FrameUpdateCallBackOffscreen` | llamada directa a 0x4D2E10 | 1024 |

`RpAnimBlendNodeUpdateKeyFrames` (0x4D1570, argumento en EAX, no hookable) queda cubierta dentro de UpdateAnimations (visible, comprimida=no, `NeedsKeyFrameUpdate`, con y sin `m_PedPosition` y `HasVelocity` por frame). `FrameUpdateCallBackW` y `FrameUpdateCallBackT` se eliminan en `rpw.patch` (cÃ³digo muerto, no equivalente al EXE).

## Fixtures

- Todo en bÃºferes estÃ¡ticos (reutiliza los de `../RpAnim43/LiveComparison.hpp`): clump, `CAnimBlendClumpData`, 1 a 13 asociaciones (13 supera el tope de 11 arrays), 1 a 4 frames, jerarquÃ­as (algunas sin secuencias), nodos, secuencias y keyframes (normales y comprimidos), skinned y no skinned (`GetFirstAtomic`/`RpSkinGeometryGetSkin` simulados, con y sin skin).
- Flags aleatorios: playing, looped, blend-auto-remove, finish-auto-remove, partial, sync, CAN_EXTRACT_VELOCITY/X, DONT_ADD_TO_PARTIAL_BLEND, IGNORE_ROOT_TRANSLATION; callbacks de tipo 1/2 que ademÃ¡s desenlazan otra asociaciÃ³n (la lista cambia durante la actualizaciÃ³n).
- Una asociaciÃ³n "superviviente" (sin blend-auto-remove, con secuencias y nodos vÃ¡lidos en todos los frames) evita las entradas invÃ¡lidas en las que el EXE se cae.
- EstrÃ©s (1 de cada 3 casos): NaN con signo y carga distintos (tambiÃ©n seÃ±alizantes), subnormales, ceros con signo, Â±1e-30, -1e30 y negativos en velocidad, blend, delta, duraciÃ³n de la jerarquÃ­a, tiempo actual y paso de tiempo. Se descartan los casos cuyo paso previsto serÃ­a enorme y positivo (el bucle de NextKeyFrame no terminarÃ­a); nunca +inf ni tiempo actual grande.

## Cortafuegos contra cuelgues (corrida 20261015-152744: Â«GTA no respondeÂ» en el caso 25)

Causa (comprobada reconstruyendo el generador en Python y con el desensamblado de 0x4D34F0/0x4D0570): en el caso 25 de la primera variante (13 asociaciones, comprimida, no skinned, frame 0 con velocidad 3D) la asociaciÃ³n 11 es blend-auto-remove con callback de borrado cuyo objetivo es la asociaciÃ³n 0. El `UpdateBlend` de la 11 se ejecuta despuÃ©s de que la 0 entrara en `BlendNodeArrays`, y `FakeCb` la desenlaza de la lista; el segundo recorrido (0x4D35B0..0x4D3608, que recalcula `m_TimeStep` solo de las asociaciones enlazadas) ya no la ve y conserva el relleno aleatorio de `0x28` del fixture (4,69e12). Sus nodos siguen en los arrays, la asociaciÃ³n estÃ¡ reproduciÃ©ndose y es looped (flags 0x4B), asÃ­ que `NextKeyFrameCompressed` (0x4D0570; en la DLL `I_NextKeyFrame`) entra con `restante = 0,38 - 4,69e12` y `restante += delta/60` no cambia un float de ese tamaÃ±o (ulp = 524288): bucle sin fin idÃ©ntico en el EXE y en la DLL (ninguno tiene guarda). Es el primer caso no estresado con esa estructura (el siguiente es el 82, con basura 0,0, inocua). No es un fallo de la funciÃ³n sino del fixture: el juego real nunca deja un `m_TimeStep` arbitrario.

Medidas (todas en `LiveComparison.hpp`):

1. La basura de `m_TimeStep` (0x28) se acota: un valor mayor que `kMaxStep` (50 s) cambia de signo (sigue siendo basura, con NaN, subnormales y Â±inf; un `restante` positivo siempre termina). Afecta tambiÃ©n a `FrameUpdateCallBackOffscreen`, que no recalcula nada y usaba esa basura tal cual.
2. `WorstStep()` simula en doubles `UpdateBlend` con los desenlaces de `FakeCb`, el tope de 11 arrays y el segundo recorrido, y predice el mayor paso positivo con que se llamarÃ¡ a los nodos. Un caso de estrÃ©s que lo supere (> `kMaxStep`) se descarta como los demÃ¡s; si aun asÃ­ `paso / (1/60)` o `tiempo actual / 0,05` superan `kMaxLoopIterations` (1.000.000) no se llama: se registra un evento `error` Â«firewallÂ» y el par falla (el banco falla, no se cuelga). Se acotan asÃ­ `NextKeyFrame`, `NextKeyFrameCompressed` y `FindKeyFrame`; la lista solo pierde enlaces, no puede formar ciclos.
3. `EnsureSurvivorRegistered()` garantiza que la superviviente entra en los arrays de nodos (el EXE desreferencia sin guarda el primer array en los callbacks y en las extracciones de velocidad: con `updData[4]==0` cae en 0x4D2EF7). El recorrido de la lista lee el siguiente antes de `UpdateBlend`, así que el desenlace de `FakeCb` sobre ese siguiente lo corta (caso 3190); se anulan entonces los `cbTarget` y, si el tope de 11 deja fuera a la superviviente (13 asociaciones), se quitan las secuencias de las anteriores. No consume números aleatorios.
3. Un hilo vigilante (como el de `CppX87Precision1`) cierra el proceso con `ReversingProbe::Fail` (Â«watchdog: una llamada del banco no vuelveÂ», con funciÃ³n, caso y lado) si una sola llamada tarda mÃ¡s de 12 s.

El estrÃ©s (NaN, subnormales, ceros con signo, Â±1e-30, -1e30) se sigue aplicando a velocidad, blend, delta, duraciÃ³n, tiempo actual y paso donde el EXE termina; solo se descartan, ademÃ¡s de los de antes, los casos cuyo paso previsto con los desenlaces reales supere 50 s.
- Simulados y trazados en ambos lados (EntryPatch en la entrada del EXE y en el cuerpo de la DLL con el mismo Fake): GetFirstAtomic, RpSkinGeometryGetSkin, CAnimManager::UncompressAnimation, RwFrameUpdateObjects y el callback de asociaciones.
- Se comparan los bytes de clump, datos del clump, asociaciones (incluye `m_TimeStep`, blend, tiempo, flags), frame datas, keyframes de pose, posiciÃ³n del ped, nodos, contexto de actualizaciÃ³n (Offscreen), hashes de jerarquÃ­as/secuencias/RwFrames, la traza de colaboradores y los globales 0xB5F878/0xB5F87C/0xB4EA0C.

## QuÃ© comprobar al compilar

1. Que `RpUpdateSyncAccumCpp` y `RpUpdateSyncStepCpp` (X87Math.cpp) generen `fld/fdiv/fmul/fadd/fstp` (y `fdiv/fmul/fmul`) con operandos de memoria, sin llamadas al CRT, sin `fstp`/`fld` intermedios y sin `movss/cvtss2sd`.
2. Que `RpAnimBlendClumpUpdateAnimations` llame a `RpUpdateSync*Cpp`, `CloudsEqualOrderedCpp` y `X87TimesFloatCpp`, y que no queden `cvtss2sd`/`mulsd` sobre `m_TimeStep`.
3. Que `FrameUpdateCallBackOffscreen` solo haga `test byte [fd], 8`, la lectura de `m_PedPosition` y la llamada a la extracciÃ³n 2D skinned.
