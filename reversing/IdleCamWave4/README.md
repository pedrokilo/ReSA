<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# IdleCamWave4: banco por pares de las 5 funciones pendientes de CIdleCam

Banco simple (flujo viejo, copiado de `WorldWave2`/`PadMouse1`/`Small2`) para `CIdleCam::FinaliseIdleCamera` (0x50E760), `Process` (0x522C80), `ProcessSlerp` (0x5179E0),
`ProcessTargetSelection` (0x517870) y `Run` (0x51D3E0) de GTA SA 1.0 US. Con ellas la clase queda completa (las otras 9 se aprobaron en `WorldWave2` y `GfxWave2`).
**Sin compilar ni ejecutar todavia** (encargo: no compilar, no ejecutar GTA).

Uso: `bash reversing/_handoff/bench.sh IdleCamWave4 3`. Filtro opcional: `idlecamwave4-only.txt` en la carpeta del juego con `CIdleCam::Funcion`.
`build_probe.py` y `run_local.py` redirigen `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` (el junction D: ya no existe).

## Corpus: 3.584 pares

| Funcion | EXE | Pares |
|---|---|---|
| `CIdleCam::FinaliseIdleCamera` | 0x50E760 | 640 |
| `CIdleCam::Process` | 0x522C80 | 512 |
| `CIdleCam::ProcessSlerp` | 0x5179E0 | 1024 |
| `CIdleCam::ProcessTargetSelection` | 0x517870 | 1024 |
| `CIdleCam::Run` | 0x51D3E0 | 384 |

## Qué se compara (original = hook desactivado, candidato = hook activo)

Cada caso reconstruye el mismo fixture desde la misma semilla en los dos lados y compara: valor de retorno / salidas, hash de todo el estado tocado (CIdleCam falso con guardas,
CCam falsa, 4 entidades falsas con sus matrices, eventos falsos, CPad 0, reloj, 3 CCam reales de `TheCamera`, `gHandShaker[0]`, `gbCineyCamProcessedOnFrame`, byte de
InterestingEvents, ticker de `gIdleCam`, `pIgnoreEntity`), control x87 y la traza de todas las llamadas salientes con argumentos. Los registros que pasan llevan `original` y `candidate`.

**Aislamiento:** durante toda la corrida los otros 13 hooks de CIdleCam se dejan desactivados (se restauran al final con `state_restored`). Un hook desactivado reescribe el inicio
de la funcion de la DLL con un `jmp` al EXE, de modo que `IsTargetValid`, `SetTarget`, `SetTargetPlayer`, `GetLookAtPositionOnTarget`, `ProcessIdleCamTicker`, `Reset`... ejecutan el
codigo del EXE en los dos lados y solo el cuerpo de la funcion probada cambia entre original y candidato. Los dos lados llaman a `CGeneral::GetATanOfXY` y a `CVector::Normalise/CrossProduct` tal como esten hookeados
(son funciones puras y son la misma en ambos lados).

### Colaboradores doblados (parche `EntryPatch` en la direccion del EXE y en el cuerpo de la DLL; cuerpos calculados ANTES de crear ningun parche)

| Colaborador | Direccion | Doble |
|---|---|---|
| `CHandShaker::Process(float)` (`ret 4`) | 0x50D930 | traza (this, grado) y escribe `m_ang.z` y la matriz de resultado de `gHandShaker[0]` con valores deterministas del caso |
| `CInterestingEvents::InvalidateNonVisibleEvents` | 0x6029C0 | traza (this) |
| `CInterestingEvents::GetInterestingEvent` | 0x6028A0 | traza y devuelve el evento falso del caso (nulo o `TInterestingEvent` con la entidad en +8) |
| `CInterestingEvents::InvalidateEvent` (`ret 4`) | 0x602960 | traza (this, evento) |
| `CEntity::RegisterReference` / `CleanUpOldReference` | 0x571B70 / 0x571A00 | traza (entidad, referencia) |
| `FindPlayerPed(int)` | 0x56E210 | devuelve el jugador falso (o nulo en ~5-8 % de los casos); en otros hilos devuelve el jugador real |
| `CWorld::GetIsLineOfSightClear` (cdecl, 9 args) | 0x56A490 | resultado programado por el caso, traza los argumentos y `pIgnoreEntity` |
| `Run`: `ProcessTargetSelection` / `ProcessSlerp` / `ProcessFOVZoom` / `FinaliseIdleCamera` | 0x517870 / 0x5179E0 / 0x517BF0 / 0x50E760 | trazan; `ProcessSlerp` escribe dos salidas distintas y devuelve un flotante en ST0; comprueban el reparto (X, Z, tiempo) y que `m_SlerpTime` ya esta escrito al llamar a `ProcessFOVZoom` |
| `Process`: `Run` | 0x51D3E0 | traza y suma 7 a `m_LastFrameProcessed` (comprueba que `gbCineyCamProcessedOnFrame` se lee del campo y no del contador de fotogramas) |

Los stubs respetan la convencion de pila del EXE (`__fastcall(this, edx, args)`, que equivale al `ret N` de cada original) y solo actuan en el hilo del banco (`owner`/`Mine()`).

Que NO se dobla y por que: `CGeneral::GetATanOfXY` (0x53CC70) y `CVector::Normalise/CrossProduct` son puras y deterministas (se llaman igual en los dos lados);
`CCam::GetVectorsReadyForRW` (0x509CE0) la lleva en linea el EXE (0x50E9D0..0x50EAD7) y la DLL la llama: se compara el efecto en la camara falsa; Multiply3x3 (0x59C810) no tiene hook y la DLL la transcribe.

## Dominio de los fixtures

- Nunca NaN/Inf ni flotantes que sean enteros pequenos (denormales: el SSE de la DLL los trata como 0 con DAZ/FTZ y el x87 del EXE no). Todos los flotantes salen de `Sane()` (0, valores notables o uniforme en [-300, 300]).
- Relojes: `now` en {0, 1000, 5000, 123456, 0x7FFFFFFF, 0x80000000, 0xFFFFFFF0, aleatorio} (cubre la correccion de FILD sin signo de 0x858C54); contador de fotogramas con `frame-1`, `frame`, `frame-2`, 0, 0xFFFFFFFF.
- Entidades a distancias 0, 2, 3.99, 4, 10, 50, 79, 80, 81, 200 de la fuente de la camara (limites de `IsTargetValid`), a lo largo de un eje (distancia exacta) o en direccion aleatoria; tipos de entidad y de ped aleatorios; posicion en la matriz propia o en la ficha.
- `ProcessSlerp`: objetivo siempre no nulo (el EXE lo desreferencia en 0x517A09); casos con `m_PositionToSlerpFrom == fuente` (vector nulo), objetivo justo en la camara, contador >= limite (usa `m_LastIdlePos`), duracion 0 (3 %: t = +inf o NaN-> los dos lados usan el mismo x87).
- `FinaliseIdleCamera`: matriz de resultado con derecha = adelante = 0 (rama `front.x == front.y == 0` -> 0.0001), matriz nula, angulos 0, +-pi/2, pi y aleatorios en [-7, 7], grado de temblor 0, 1, 0.5, 2, -1.
- `ProcessTargetSelection`: sin objetivo, objetivo = jugador, objetivo ajeno; con y sin evento (entidad nula, igual al objetivo, ajena); `ZoomState` 0..3; contador de LOS y limite en torno al borde (13/14/15); factor de zoom 0 (3 %); jugador nulo (5 %).
- `Process`: ticker por debajo/encima/en el umbral, `LastTimePadTouched` igual o distinto al del pad, camara activa 0..2 (se escriben los vectores de las tres CCam reales de `TheCamera`, restauradas), byte de InterestingEvents aleatorio.

## Correcciones de la fuente (`source/game_sa/IdleCam.cpp`), cada una con su evidencia

1. **FinaliseIdleCamera** (0x50E760):
   - `hs.Process(shakeDegree * m_DegreeShakeIdleCam)`: el EXE hace `fld [esp+38h]; fmul [esi+88h]; ... fstp [esp]; call 0050D930` (0x50E80B..0x50E825). La DLL pasaba solo `shakeDegree`.
   - `Multiply3x3` vector*matriz (0x59C810: `fld [ecx+4]; fmul [eax+4]; ...`, producto con las filas, sin posicion) en vez de `m_resultMat.TransformPoint` (columnas + posicion), con el orden de sumas del EXE (0x50E855).
   - `fcos/fsin/fcos/fsin` del x87 con precision completa (0x50E768..0x50E7A9 y 0x50E87D..0x50E8AA) en lugar de `std::cos/sin` (CRT con SSE, distinto en el ultimo bit); `__asm` transcrito.
   - `Normalise` en sitio tras guardar los tres componentes (0x50E7C0) y constante 0.0001f como bits 0x38D1B717 (0x50E949).
2. **ProcessSlerp** (0x5179E0):
   - `lerp(from, to, t)` de la DLL es `to*t + from*(1-t)` y se le pasaba `(destino - origen)` como destino: resultado distinto. El EXE hace `desde + (hasta - desde) * lerpT` (0x517BBD..0x517BD3: `fsub [esp+4]; fmul st,st(2); fadd [esp+4]`).
   - El ajuste de +-2PI es `diff > PI -> -2PI; si no diff < -PI -> +2PI` (`fcom; test ah,41h; jne` / `fcomp; test ah,5; jp`: con NaN no cambia); la DLL usaba `!(diff <= PI)` (con NaN restaba 2PI).
   - `lookAtDistAtan` (4a llamada a 0x53CC70) queda en ST0 sin redondear a float hasta el `fsub [esp+4]`; ahora todo el cuerpo es `__asm` transcrito (llamadas a 0x53CC70, `fsin` con precision completa, `x*180` -> `270-` -> `*0x8595EC` -> `fsin` -> `+1` -> `*0.5`) con las constantes exactas del EXE (PI 0x858CB8, 2PI 0x858CBC, -PI 0x858CC0).
   - `GetLookAtPositionOnTarget` se llama SIEMPRE (0x517A09) y luego se sobrescribe con `m_LastIdlePos` si el contador >= limite (0x517A14..0x517A2B); la DLL solo la llamaba en la otra rama.
3. **ProcessTargetSelection** (0x517870):
   - Sin objetivo (0x517944..0x517964): el EXE hace `m_nForceAZoomOut = 1; m_Target = jugador_cacheado` (store directo `mov [esi],ecx`, sin registrar referencia) y DESPUES `SetTarget(FindPlayerPed())`, de modo que `SetTarget` ve un objetivo previo (copia `m_LastIdlePos` en vez de `fuente+front` y llama a `CleanUpOldReference`). La DLL llamaba a `SetTargetPlayer()` con `m_Target == nullptr`.
   - `FindPlayerPed(-1)` se llama una vez al empezar (0x517876) y se reutiliza (EDI) en las comparaciones (0x5178F4 `cmp [esi],edi`); la condicion es `!(m_Target && valido) && m_Target != jugador` (con objetivo nulo y jugador nulo el EXE no hace nada; la DLL si).
   - 0x517983..0x5179B7: `SetTarget(jugador)` solo si `ZoomState == 3`, `m_nForceAZoomOut = 1`, y el contador se lee DESPUES (`SetTarget` lo pone a 0); equivalente al `||` de la DLL pero ahora reproduce las llamadas.
4. **Run** (0x51D3E0) y **Process** (0x522C80): MATCH sin cambios (orden de llamadas, `fcom + test ah,5 + jp` del temblor, `fild`+correccion sin signo, `or dl,1` sobre 0xC0B184, `cmp eax,edx; jae`, `gbCineyCamProcessedOnFrame = [esi+30h]`).

## Fuera del banco

Nada de las 5 funciones queda fuera. `CIdleCam::VectorToAnglesRotXRotZ` (cabecera, ayudante NOTSA) ya no lo usa `ProcessSlerp`; se deja sin tocar (no es codigo del EXE).

## Supuestos a revisar al compilar

- `__asm` en `IdleCam.cpp`: etiquetas `L_*` locales, `fsubr st, st(1)` (forma D8, sin la rareza de MASM con `st(i)` como destino), constantes de espacio de nombres anonimo referenciadas como `dword ptr [k...]` (como `General.cpp`) y `call atanFn` con `const uintptr_t` local (como `Camera.cpp`).
- Que `CMatrix::GetRight/GetForward/GetUp/GetPosition` existan con `this auto&&` (se usan en el evento `layout` y en `IdleMultiply3x3_VM`).
- Si algun colaborador cambia de hook (p. ej. `CGeneral::GetATanOfXY` desactivado) los dos lados siguen llamando a la misma funcion.

## Aprobación (09/10/2026)

Corrida `20261009-124211-091747`: passed=true, 3584 pares, 5 funciones con 0 fallos (FinaliseIdleCamera 640, Process 512, ProcessSlerp 1024, ProcessTargetSelection 1024 y Run 384), evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_idlecamwave4.py` (evidencia en `runs/`, `validation-summary.json`). Con esto CIdleCam queda COMPLETA. Correcciones de fuente: ProcessTargetSelection (FILD exacto y FSUB con un único redondeo a 24 bits: si now < 2^31, float(double(now)-double(T)); si no, float(now)-T; el objetivo nulo guarda el jugador cacheado directo, 0x517952, y llama a SetTarget, 0x517989), ProcessSlerp (transcripción x87 0x517A2D..0x517BD5) y FinaliseIdleCamera (FCOS/FSIN x87, factor de temblor, Multiply3x3 vector por matriz, GetVectorsReadyForRW en línea). Límites: entorno x87 PC24 verificado por banco; colaboradores doblados con traza de argumentos crudos, sin crédito.
