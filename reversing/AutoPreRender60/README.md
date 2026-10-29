<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco AutoPreRender60: CAutomobile::PreRender (0x6AAB50..0x6ACCBA, 0x216B bytes)

**Estado: APROBADO EN JUEGO (19/10/2026, 8192 pares, cero diferencias) con el banco `reversing/AutoPreRender60`; sin registrar (`register_hook.py` lo ejecuta el coordinador).**
La fuente C++ de `Automobile.cpp` se contrastó antes fuera del juego (`offgame/`, 3,1 M de pares) y ahora en el juego real: el banco compara la entrada del EXE con el gancho RH virtual apagado (ORIGINAL) frente a la misma entrada con el gancho encendido (CANDIDATO).
La corrida en juego halló dos diferencias reales que el arnés fuera del juego no podía ver (ver «Banco en juego»), ambas corregidas en la fuente.

## Archivos

| Archivo | Contenido |
| --- | --- |
| `source.patch` | `git diff HEAD` de SOLO esta función: `Automobile.cpp` (include de `Shadows.h`, `RH_ScopedVMTInstall(PreRender, 0x6AAB50)`, bloque de subfunciones privadas + `CAutomobile::PreRender`). Los ayudantes `AutoPre*` de `X87Math.cpp/.h` ya están en `HEAD` (commit 4a489bbc3) y no cambian. `git apply --check` limpio contra el `Automobile.cpp` de `HEAD` (d349867fb). |
| `Core.hpp`, `Fixtures.hpp`, `LiveComparison.hpp` | Banco en juego (mundo y falsos portados de `offgame/harness.cpp`). |
| `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `dump_fixtures.cpp`, `register_hook.py` | Construcción de la sonda, corrida + análisis, oráculo de fixtures/cobertura, volcador para la autoprueba cruzada, registro (solo el coordinador). |
| `exe_disassembly_0x6AAB50.txt` | Desensamblado completo del EXE (2307 líneas) con las tablas de saltos y de nodos, para quien escriba el banco en juego. |
| `offgame/` | Banco fuera del juego: `harness.cpp`, `build_offgame.py`, `run_offgame.py`, `mutate.py` (ver abajo). |

## Qué hace la función (mapa de bloques → subfunción privada en `Automobile.cpp`)

| Dirección | Comportamiento | Subfunción / ayudante |
| --- | --- | --- |
| 0x6AAB71..0x6AAB8B | `CVehicle::PreRender()` (0x6D6480); si el truco 34 (`0x969152`, coches flotantes) virtual +0x114 (`DoHoverSuspensionRatios`); `GetColModel()` (0x535300) → ColData `[+0x2C]` | cuerpo de `PreRender` |
| 0x6AAB93..0x6AADCF | si `vehicleFlags` byte 0x42B bit 0 y subtipo (+0x594) != monster truck: virtual +0xD0 (`DoBurstAndSoftGroundRatios`) y, por rueda, altura suavizada `m_wheelPosition` | `AutoPreRenderSuspension` + `AutoPreSuspensionHeightCpp` |
| 0x6AADCF..0x6AAE1B | `speedRatio = ((vz·fz + vy·fy) + vx·fx) / [0xC1BFF0]` y `|v|` (x87 sin guardados intermedios) | `AutoPreForwardSpeedCpp`, `AutoPreSpeedMagnitudeCpp` |
| 0x6AAE1F..0x6AB154 | (modelo ≠ 0xFFFE, 539, 441) estado SIMPLE: puntos de contacto de las cuatro ruedas desde los marcos 4, 7, 2, 5; estado 0..3: `AddSingleWheelParticles` ×4 | `AutoPreRenderSimpleWheelPoints`, `AutoPreRenderWheelParticles` |
| 0x6AB154..0x6AB2BC | si el nodo 3 existe: partículas de las ruedas 1 y 3 en `contacto + 2·forward` (ranuras 5 y 6) | `AutoPreRenderMiddleWheelParticles`, `AutoPreDoublePlusCpp` |
| 0x6AB2BC..0x6AB349 | salpicadura de lluvia y escape | `AutoPreRenderRainAndExhaust`; `AddDamagedVehicleParticles` |
| 0x6AB350..0x6ABC71 | tabla de saltos por modelo: sirenas (407, 416, 427, 596..599: luz puntual + 4 coronas por turno), taxi 420 / cabbie 438 (corona + luz), FBI Rancher 490 | `AutoPreRenderModelLights` → `...EmergencyLights`, `...TaxiLight`, `...RancherLight` |
| 0x6ABC71..0x6ABD63 | `DoVehicleLights` (no 441, 0xFFFE, 432, RC, helicóptero), sombra del vehículo, 2.ª `CMatrix` local, Rhino: `SetComponentRotation(nodo 16, Z, [0x94C], true)` | cuerpo de `PreRender` |
| 0x6ABD63..0x6ABE83 | direcciones de las ruedas delanteras/traseras (dirección, `bSteerRearwheels` 0x20, `bHbRearwheelSteer` 0x40) | `AutoPreRenderWheelDirections` |
| 0x6ABE85..0x6ABFC0 | giro de las cuatro ruedas (`GetSpeed`, `ProcessWheelRotation`) | `AutoPreRenderWheelRotation`, `AutoPreWheelRotationCpp` |
| 0x6ABFC6..0x6AC22D | `UpdateWheelMatrix` ×4 (+2 intermedias), 6 puertas, 3 paneles, cadera oscilante (los tres últimos no se ejecutan con el Rhino) | `AutoPreRenderBodyParts` |
| 0x6AC232..0x6AC297 | `m_moveForce = vel + vel. fricción`, `m_turnForce` análogo | `AutoPreRenderForces` |
| 0x6AC297..0x6ACBC7 | componentes por modelo: 424 (BF Injection), 486, 524, 443, 525 (grúa), 531, 530 (montacargas), 532 (cosechadora), 568/434, 432/601, 407, 477 | `AutoPreRenderModelComponents` → `...BfInjection`, `...TiltAxles`, `...TowTruck`, `...Forklift`, `...Harvester` |
| 0x6ACBC7..0x6ACC8B | visual de la hidráulica (modelo 495 o banderas 0x10000+0x20000 con subtipo 0) | `AutoPreRenderHydraulics` |

Tablas de datos del EXE usadas: bytes de modelo `0x6ACCD0`, saltos `0x6ACCBC`, subtabla de sirenas `0x6ACD94/0x6ACDAC`, nodos de ejes `0x8D31D8` ({20,22,21,23}) y `0x8D31F0` ({21,0,22}).

## Ayudantes x87 (`X87Math.cpp/.h`, bloque «Lote 28», prefijo `AutoPre`)

`AutoPreForwardSpeedCpp`, `AutoPreSpeedMagnitudeCpp`, `AutoPreSuspensionHeightCpp`, `AutoPreDoublePlusCpp`, `AutoPreScaleByteCpp`, `AutoPreFadeByteCpp`, `AutoPreCoronaByteCpp`, `AutoPreDotNegativeCpp`,
`AutoPreWheelRotationCpp`, `AutoPreMixerPropCpp`, `AutoPreTowAngleCpp`, `AutoPreHarvesterPropCpp`, `AutoPreClampedAsinCpp`, `AutoPreTowAnglesCpp`, `AutoPreAxleTiltCpp`, `AutoPreForkliftZCpp`,
`AutoPreMultiply3x3Cpp` (0x59C790), `AutoPreMultiplyMatrixWithVectorCpp` (0x59C890), `AutoPreMulCpp`, `AutoPreAddCpp`, `AutoPreSubCpp`.
Reutiliza `TrigSimpleSinCpp/CosCpp`, `AutomobileWheelSpinAudioCpp` (mezcla con `pow(0.95f, ts)`), `VehicleTransmissionAnglesCpp` (el ángulo de la cosechadora es una copia en línea de `SetTransmissionRotation` pero con `SetComponentRotation`) y `ExeAsinDoubleCpp`.
Se inspeccionó el código generado (`/FAs`) de los ayudantes principales para comprobar que salen las mismas operaciones x87 en el mismo orden que el EXE (sin reasociación, `fsqrt` en línea, `__ftol2`); el resto lo cubre el banco.

## Fase 2: revisión contra el EXE (discrepancias halladas y corregidas)

Las halló el banco fuera del juego al ejecutar el EXE de verdad (ninguna se veía a simple lectura):

1. **Matrices (0x59C790 / 0x59C890).** `G2Multiply3x3` / `G1MultiplyMatrixWithVector` (float SSE por operación) difieren del EXE con desbordes y NaN: los tres productos se suman en la pila x87 con exponente extendido (`1e30·1e30 + (-1e30·1e30)` da 0 en el EXE y NaN en SSE). Sustituidos por `AutoPreMultiply3x3Cpp` / `AutoPreMultiplyMatrixWithVectorCpp` (estas dos funciones locales siguen siendo válidas para sus otros consumidores en entradas normales).
2. **`contacto + 2·forward` y `posición + 2·up` (0x6AB186, 0x6AB229, 0x6AB715).** El EXE guarda el doble de x e y como float (`fstp`) antes de sumar y deja el de z en la pila: con `forward.x = -FLT_MAX` el doble se satura a `-inf` (y `-inf + inf = NaN`). `AutoPreDoublePlusCpp` lo reproduce.
3. **Subnormales.** `timeStep · 0.3f` (BF Injection, 568/434) y `lodMultiplier · 150` redondean dos veces en el x87 (24 bits y luego `fstp` a subnormal); `AutoPreMulCpp`.
4. **Dos NaN distintos.** `fadd`/`fsub` con dos NaN silenciosos conservan el de mayor mantisa; SSE devuelve el primer operando (velocidad relativa de la rueda, fuerzas, posición de la cadera, panel de la grúa): `AutoPreSubCpp` / `AutoPreAddCpp`.
5. **`pow`.** `AutomobileWheelSpinAudioCpp` (→ `ExeCompatiblePowCpp`) coincide con `_CIpow` del EXE **solo si la bandera SSE2 de la CRT del EXE (`0xC9C400`) vale 1**, que es el estado del juego (PowEquivalence). Sin ejecutar el arranque de la CRT esa bandera está a 0 y el EXE usa su ruta x87 de 24 bits (resultados distintos); el banco fuera del juego la fija a 1. No es un cambio de fuente.
6. Comprobado a mano contra el desensamblado y confirmado por el banco: tabla de modelos (`cmp ax` + saltos), orden de llamadas (p. ej. `GetColModel` sin uso en 424/568/434), ranuras y banderas de `AddSingleWheelParticles` (4 por ruedas delanteras si ninguna trasera derrapa, +1 sangre, +2 barro), índices de nodos (0x648 + 4·n), `m_nStatus` (SIMPLE y ≤ PHYSICS), la sirena (`al/6`, desvanecimiento 100 / 0x19C / 0x200, 4 luces entre `b` (i = 0) y `a` (i = 3), selector `((t + 0x40·i) >> 8) & 3`), `ftol` de bytes (`M·0.0` siempre 0), etc.
7. Comprobaciones de diseño hechas con `cl` en el scratchpad (no se versionan): `offsetof` de los campos usados (todos coinciden con el EXE) y posición real de los bits de `vehicleFlags` / `autoFlags` / `m_nModelFlags` (leyendo el código generado).

## Banco fuera del juego (`offgame/`)

Proceso propio que mapea el `gta-sa.exe` canónico en 0x400000 (solo lectura del archivo; el lanzador `run_offgame.py` reserva la zona en el proceso suspendido porque Windows ya ocupa la zona baja), sustituye por falsos (JMP con la convención exacta del EXE) los 25 colaboradores sin entorno (CVehicle::PreRender, ColModel, partículas, coronas/luces, sombras, SetComponentRotation, ProcessWheelRotation, UpdateWheelMatrix, puertas, paneles, `GetWheelPosn`, `GetFrameFromId`, `SetTransmissionRotation`, 4 virtuales por una vtable falsa, clima, etc.) y ejecuta cada caso dos veces:
**original = los bytes del EXE en 0x6AAB50** y **candidato = la fuente C++ extraída de `Automobile.cpp`** (`extract.inc`). Matrices (`Attach`, `UpdateRW`, `SetRotate`, `RotateX`, `SetTranslateOnly`, destructor), `GetSpeed`, `GetWheelStatus`, `_CIpow`, `_CIasin`, `_ftol2` y `0x41CCB0` se ejecutan **del EXE en ambos lados o, en el caso de `0x41CCB0`, de verdad en el original y con la fuente C++ en el candidato**.
Los dos lados llaman al mismo núcleo de cada falso y se comparan: memoria completa (objeto con 8 KB de guardas por lado, 25 marcos, matrices, ColModel/ColData/líneas, manejo, modelo, cámara `0xB6F028`, globales), traza de colaboradores (argumentos en bits, punteros normalizados), ESP, EBX/ESI/EDI/EBP, DF, x87 normalizado (FCW completa, TOP, etiquetas, ST válidos), MXCSR; EAX/ECX/EDX solo se registran.

```
python -B reversing/AutoPreRender60/offgame/build_offgame.py [--out DIR]           # compila (cl; ~25 s); 545 simbolos sin resolver de clases dllexport son esperados (/FORCE:UNRESOLVED)
python -B reversing/AutoPreRender60/offgame/run_offgame.py --exe-harness DIR/harness.exe [casos] [primera] [ruta_exe]
python -B reversing/AutoPreRender60/offgame/run_offgame.py --exe-harness DIR/harness.exe 1 0 "D:/Grand Theft Auto San Andreas/gta-sa.exe" selftest   # _CIpow/_CIasin del EXE frente a los ayudantes
python -B reversing/AutoPreRender60/offgame/mutate.py --out DIR                    # sensibilidad: 29 mutaciones deliberadas de la fuente (restaura Automobile.cpp al terminar)
```
Fixtures: 31 modelos (los 28 con rama propia + 400, 503 y -2 = 0xFFFE), estados 0..5, banderas y bits de manejo sesgados hacia cada rama, sirenas/taxi/Rancher/cosechadora/grúa con y sin remolque/montacargas/hormigonera/BF/Bandito/Rhino, nodos nulos donde el EXE los comprueba, paneles con índices negativos/21, 40 % de «salvajes» (0, -0, ±inf, NaN con carga y signo cualesquiera, subnormales, FLT_MAX/MIN, 1e30, bits crudos sin sNaN) y 1 de cada 11 casos «de frontera» (matrices alineadas con los ejes, velocidades exactas en los umbrales 20, 130, 0.1, 0 y ±1 ulp, lluvia = 0.02f).

Resultados (última versión de la fuente): ver «Resultados» al final del archivo (se actualiza con cada corrida).

### Límites del banco fuera del juego (lo que NO prueba)
- Los 25 colaboradores sustituidos se prueban con falsos idénticos en ambos lados; no se prueba su implementación real (ni el EXE ni la C++ del proyecto) ni que el hook de cada uno esté encendido o apagado.
- No usa el entorno real del juego (CRT inicializada, FCW de Direct3D…): fija FCW a 24 bits, redondeo al más cercano, excepciones enmascaradas, `0xC9C400 = 1`.
- sNaN no se generan (como en el resto de bancos).
- `0x41CCB0` solo se comprueba con el estado de cámara generado (matriz o `m_placement`).

## Banco en juego

```
bash reversing/_handoff/bench_here.sh AutoPreRender60        # compila la sonda, corre en el juego (candado global) y resume
python -B reversing/AutoPreRender60/build_probe.py          # a mano
python -B reversing/AutoPreRender60/run_local.py            # a mano (en serie)
python -B reversing/AutoPreRender60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # solo análisis (None si el registro aún no existe)
python -B reversing/AutoPreRender60/register_hook.py <carpetaEvidencia>                            # SOLO tras APROBADO (coordinador)
```
Requiere `RH_ScopedVMTInstall(PreRender, 0x6AAB50)` (`source.patch`). Fila de `docs/hooks.csv` (la añade el coordinador): `CAutomobile,PreRender,0x00000000,1,0,1,V`. Marcador/registro: `autoprerender60-runtime.start` / `.jsonl`.

- **Pares**: 8192 (`Fixtures.hpp`): 31 modelos (264 casos por modelo; los 28 con rama propia + 400, 503 y -2), 40 % «salvajes» (0, -0, ±inf, NaN con carga y signo cualesquiera, subnormales, FLT_MAX/MIN, 1e30, bits crudos sin sNaN), 1 de cada 11 casos «de frontera» (matrices alineadas con los ejes, umbrales exactos 20, 130, 0.1, 0 y ±1 ulp).
- **Mundo** (`ApplyWorld`, portado del arnés): objeto de 0x988 bytes con guardas de 8 KB, 28 marcos, matrices, ColModel → ColData → líneas, manejo, información de modelo (con `ms_modelInfoPtrs[380..639]`, `[-2]`, `[503]`, `[530]` redirigidos), cámara 0xB6F028, globales (tiempo 0xB7CB84, paso 0xB7CB5C, brillo de coronas 0xB7C4E4, 0xB6F118, `currArea` 0xB72914, lluvia 0xC81324, truco 34 0x969152, divisor 0xC1BFF0) y vtable falsa (+0xD0, +0xEC, +0xF0, +0x114; el resto da `BadVirtual`, que debe valer 0). Todo lo que se escribe fuera de las arenas se guarda antes y se restaura tras cada lado (`GameState`).
- **Colaboradores sustituidos por falsos idénticos** (misma función núcleo en ambos lados; traza en bits): 24 direcciones del EXE (0x6D6480, 0x535300, 0x6DE880, 0x72DDB0, 0x72DDC0, 0x6DDF60, 0x6DE240, 0x6D2A80, 0x6FC580, 0x7000E0, 0x6D1080, 0x6E1A60, 0x56E0D0, 0x50AED0, 0x70BDA0, 0x6DBA30, 0x6D1230, 0x6AA290, 0x6A9D70, 0x6F49A0, 0x6F4040, 0x4C7D20, 0x4C53C0, 0x6DBBB0) y el cuerpo de la DLL de cada uno, resuelto con el gancho RH registrado (`HookBody`: simple o virtual, siguiendo saltos solo dentro del ASI). Excepción documentada: 0x6DE880 (`AddSingleWheelParticles`) no tiene gancho (la DLL llama a la dirección del EXE). 0x6D1080: el gancho RH es la envoltura `GetVehicleAppearance_Reversed`; la fuente llama al miembro real, que también se sustituye (`env.extra`).
- **Se ejecutan de verdad** en ambos lados: `GetSpeed` (0x542CE0), `GetWheelStatus` (0x6C21B0), `Attach/UpdateRW/SetRotate/RotateX/SetTranslateOnly` de `CMatrix`, `_CIpow/_CIasin/_ftol2` del EXE y el destructor de matrices (0x59ACD0). Sus ganchos quedan en el estado normal del juego (encendidos: el ORIGINAL pasa por la DLL en los validados). La fuente no llama a 0x59C790/0x59C890 sino a los ayudantes x87; el EXE sí los llama (sin gancho).
- **Qué se compara por lado** (en `observed`, que debe ser idéntico): hash FNV-64 de la memoria completa (arenas + cámara + globales, `mem_size` constante), hash de la traza de colaboradores, longitud de la traza y conteos por colaborador; más compuertas ABI (ESP, EBX/ESI/EDI/EBP, DF), ruta (`route_ok`), x87 normalizado (FCW completa, TOP, etiquetas, ST válidos), MXCSR sin banderas, `globals_restored` (se comprueba tras quitar los parches de falsos), `floating_state_restored`. EAX/ECX/EDX solo se registran (`original_residue`/`candidate_residue`). Si un par difiere, se escribe un evento `diff_detail` con la primera diferencia de memoria y de traza.
- **Oráculo**: de la función, el propio EXE (gancho apagado); de los fixtures, `fixture_oracle.py` (regenera en Python los parámetros de cada caso y exige que la fila `input` coincida; autoprueba cruzada con `dump_fixtures.cpp` compilado con `cl`: 8192 filas idénticas, `python -I fixture_oracle.py --selftest rows.jsonl`); `run_local.py` exige además cobertura mínima de modelos, valores salvajes/frontera y de llamadas a cada colaborador.
- **Tamaños de arena**: solo viven en `Core.hpp` (`kGuard`=0x2000, `kCarSize`=0x988, 28 marcos de 0x60); `mem_size` es constante en las 8192 filas y el oráculo no los repite.

### Resultado (corrida APROBADA)
Cobertura de llamadas (casos con ≥1 llamada, de 8192): ColModel, CamNoRain, DamagedParticles, FindPlayerVehicle, UpdateWheelMatrix y CVehicle::PreRender 8192; ProcessWheelRotation 8029; SwingingDoor 7927; ProcessPanel 7809; AddSingleWheel 7427; Appearance 6510; PlayerNoRain 6219; StoreShadow 6071; DoorProcess 5088; DoBurst 4682; LookingForward 4102; VehicleLights 3995; SetComponentRotation 3413; GetWheelPosn 2550; SetTransmissionRotation 2348; DoHover 2084; AddLight 1683; Corona 1547; WaterSplash 1496; Exhaust 1402; GetFrameFromId 223; TowHitch 131; TowBar 102; BadVirtual 0.

### Defectos reales hallados por el banco en juego (corregidos en `source.patch`)
El juego corre con MXCSR FTZ (0x9F80): una multiplicación SSE cuyo resultado es subnormal lo vacía a 0, mientras que el `fmul` x87 del EXE lo conserva al guardarlo como float. El arnés fuera del juego (MXCSR por defecto) no podía verlo.
1. **Radio de la rueda** (0x6ABF76: `fld size; fmul [0.5f]; fstp`): `size * 0.5f` en SSE vaciaba el subnormal; ahora `AutoPreMulCpp(size, 0.5f)` (148 de 8192 pares, diferencia en el argumento `radius` de `ProcessWheelRotation`).
2. **Punto local de la rueda en estado SIMPLE** (0x6AAE76 y siguientes: `fld wheelPos; fmul [1.5f]; fstp`): `m_wheelPosition[wheel] * 1.5f` igual; ahora `AutoPreMulCpp(..., 1.5f)` (1 par: el subnormal vaciado a 0 convertía `inf * subnormal = +inf` en `inf * 0 = NaN` en la multiplicación posterior con la matriz).
3. Fallo del banco (no de la función) corregido en el camino: el gancho de `GetVehicleAppearance` es la envoltura `*_Reversed`, y la fuente llama al miembro real (6541 pares al principio).

### Notas de la preparación (siguen vigentes)

- Estado de los hooks de los colaboradores en `HEAD` (resolver cada cuerpo con el hook ENCENDIDO antes de parchear falsos, ver `reversing/CTaskSimpleJetPack` / `PickWeap1`): C++ con hook: 0x6D6480, 0x535300, 0x59BD10, 0x59BBB0, 0x59B120, 0x59B1E0, 0x59AF80, 0x6C21B0, 0x72DDB0, 0x72DDC0, 0x6DDF60, 0x6DE240, 0x6D2A80, 0x6FC580 (sobrecarga eCoronaType), 0x7000E0, 0x6D1080, 0x6E1A60, 0x56E0D0, 0x50AED0, 0x70BDA0, 0x6DBA30, 0x6D1230, 0x6A9D70, 0x6F49A0, 0x6F4040, 0x542CE0, 0x4C7D20, 0x4C53C0, 0x6DBBB0, 0x6A45C0 y 0x6A47F0 (virtuales). Sin hook (envoltura al EXE): 0x6DE880 (`AddSingleWheelParticles`), 0x6AA290 (`UpdateWheelMatrix`, en trabajo en otro worktree), 0x59ACD0 (destructor de `CMatrix`), 0x59C790/0x59C890 (la fuente ya no los llama: usa los ayudantes), 0x41CCB0 (la fuente usa su copia C++ `AutoPreRenderPlaceableForward`), 0x821B40/0x821E70/0x822130 (CRT del EXE; la fuente usa `__ftol2`/`ExeAsinDoubleCpp`/`ExeCompatiblePowCpp`).
- Llamadas virtuales (se resuelven por la vtable del objeto): +0xD0, +0x114, y `GetTowHitchPos`/`GetTowBarPos` (+0xEC/+0xF0) del vehículo remolcado.
- Registros: `python -I reversing/Dependencies25A/residue_scan.py 0x6AAB50` → 3 llamadores directos (0x6C7E7A, 0x6CEAEC, 0x6CFADC: Monster truck, quad, tráiler); ninguno lee EAX/ECX/EDX tras la llamada (desensamblados a mano: `movsx eax,[esi+22h]`, `mov ecx,...`); las llamadas por vtable (CRenderer) no se escanean pero tratan la función como `void`. Solo registrarlos.
- No usa `rand` ni `sscanf`.

## Suposiciones y riesgos

- La fuente depende de los ayudantes `AutoPre*` compilados con las opciones de `X87Math.cpp` (/arch:IA32, FCW 24 bits en el proceso).
- `RH_ScopedVMTInstall(PreRender, 0x6AAB50)` redirige también a `CMonsterTruck/CQuadBike/CTrailer::PreRender` (llaman a `CAutomobile::PreRender` por dirección) y a los llamadores del EXE; `CHeli` no pasa por aquí.
- Nombres de campos asumidos (`m_fDoomVerticalRotation` = 0x94C, `m_fDoomHorizontalRotation` = 0x950, `m_fPropRotate` = 0x958) son los de la cabecera; la función los usa como ángulos de varios componentes.
- La lluvia/escape usan `CGame::currArea` por dirección (`0xB72914`) y el truco 34 por dirección (`0x969152`).
- Los marcos nulos solo se prueban donde el EXE los comprueba (nodos 1/21 de la cadera, 3, 6, 20 del montacargas y los de los ejes inclinables); en los demás la fuente desreferencia igual que el EXE.

## Resultados

Con la fuente final (la del `source.patch` de este directorio; banco reconstruido desde cero con `build_offgame.py`):

- Autoprueba de la CRT del EXE (`_CIpow(0.95f, t)` y `_CIasin`) frente a los ayudantes de X87Math: 20000 valores, 0 diferencias (con `0xC9C400 = 1`; con 0 el `pow` del EXE difiere en los 20000).
- 8192 casos + 2000000 + 100000 + 1000000 casos más (rangos 0, 100000 y 5000000): **cero diferencias** en memoria, traza, ABI y x87 (3,1 M de pares con la fuente final).
- Versiones anteriores de la fuente: 8192 casos con 132 diferencias (pow del EXE sin bandera SSE2, matrices SSE, `+2·forward`, NaN con dos cargas, subnormales) y 1,5 M con 1 diferencia (dos NaN en la velocidad relativa); todas corregidas (ver «Fase 2»).
- Sensibilidad (`mutate.py`, 8192 casos por mutación): 29 mutaciones deliberadas, **29 detectadas** (la más débil, el umbral de escape 130 → 100, la detectan 16 casos; el umbral exacto lo cubre la familia de frontera).
- Cobertura de colaboradores en 1 M de casos (2 M de ejecuciones, ambos lados; cifras de la corrida final): todos los falsos se llaman decenas de miles de veces (p. ej. `GetTowHitchPos` 32728, `GetFrameFromId` 53896, `AddLight` 403346, `Corona` 752972), `BadVirtual` = 0.
- Compilaciones sueltas (no el proyecto): `Automobile.cpp` con las opciones Debug del proyecto (/Od /RTC1 /arch:SSE2, avisos de nivel 3): 0 errores y 0 avisos propios; `X87Math.cpp` con sus opciones: sin errores.
