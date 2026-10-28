<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dos archivos de nivel 5 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp` y `Entity/Ped/Ped.cpp` (13 funciones fuente). Un subagente por archivo preparó el análisis; otro escribió un banco nuevo para los dos emisores de partículas de pisada; la compilación y los bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

- `RpAnimBlend`: los seis puentes al EXE (`FrameUpdateCallBack*WithVelocityExtraction*`, que solo corrían en la ruta candidata del banco, porque los envoltorios de producción están desactivados) pasan a una plantilla C++ `FrameUpdateCallBackExeVelocity<IsCompressed, IsSkinned, Is3D>` reescrita desde el desensamblado de las seis funciones del EXE. El cuerpo C++ que había tras el `return` de los puentes no servía de base: asignaba la posición con `+=`, sumaba `t` en lugar de `GetEndTranslation`, restaba el cuaternión también donde el EXE siempre suma, y mezclaba banderas entre variantes.
- `Ped`: `GetLocalDirection`, los dos `naked` de `atan2` de `SetPedPositionInCar` y `PositionAttachedPed` (con `fpatan` sin redondear mediante ayudantes `double` y alias `/alternatename`), el bloque `pow` de `ProcessBuoyancy`, la oscilación de `PreRenderAfterTest` y los dos emisores de partículas de `DoFootLanded`, que eran puentes al EXE y se reescribieron en C++ (la parte menos verificada del lote: ver abajo).

## Qué se encontró

- Los emisores de pisada no tenían ningún banco que validara su cuerpo; el banco de `DoFootLanded` los contaba con falsos. Se derivó `reversing/CppPedFootEmitters1`. Su primera versión no compilaba (varios `auto` con tipos distintos en una declaración) y su comprobación previa exigía una dirección de DLL para `SurfaceInfos_c::ProducesFootDust`, que no está reconstruida (su símbolo es la propia dirección del EXE). Con el banco ya ejecutándose dio 77 de 1536 pares de diferencia en `EmitFootRainParticles`: `forward·1,5` se multiplicaba en SSE y descartaba subnormales que el EXE conserva; se corrigió con `X87TimesFloatCpp` (el ayudante antes llamado `HandlingTimesFloatCpp`, renombrado a un nombre genérico). Aprobó con 3072 pares.
- `CPedC`: `SetMoveAnim` (63 de 1024) por el patrón de siempre: la DLL llama directamente a `CPedGroups::IsInPlayersGroup` y `GetPedsGroup` y el banco solo los interceptaba por el EXE. Se añadieron los `EntryPatch` de la DLL.
- `reversing/CPed` (completion, 28672 pares): tres fallos de instrumentación del mismo tipo, ninguno causado por el código convertido: `CShadows::AddPermanentShadow` (el fake de sombras solo estaba en la dirección del EXE, falló en la entrada 5 de `DoFootLanded`), `CCarEnterExit::GetPositionToOpenCarDoor` (la DLL la llamaba directamente sobre un vehículo falso, el juego se cayó con acceso inválido en `PositionPedOutOfCollision`; se localizó resolviendo el volcado con el PDB de la sonda) y los emisores (contadores de la sonda, con los parches de `CompletionProbe.hpp`, `FootLandedComparison.hpp` y `build_completion_probe.py`). La sonda se reconstruye y su hash se actualiza en `run_completion.py`.

## Seguimiento: los límites que se cerraron (15/10/2026)

Tras la primera versión quedaban cuatro límites escritos; los cuatro se resolvieron.

- **Tolerancia de `reversing/CPed`.** `run_completion.py` compara ahora en bits exactos (incluido el signo del cero), sin tolerancia. Medido sobre el log completo, había diferencias reales en tres funciones, todas de redondeo del código C++ (SSE en Debug) frente al x87 del EXE, y se corrigieron:
  - `CalculateNewVelocity` (202 de 4096 pares): el arcoseno se redondeaba a float antes de multiplicar (el EXE lo deja en ST0), el de `m_fSlopeRoll` usaba el `asinf` de la DLL en lugar del modelo de `_CIasin`, y `m_vecAnimMovingShift` conservaba un `-0.0` donde el EXE suma sobre `+0.0`. Ayudantes `CnvBlendedAsinCpp`, `CnvAsinFloatCpp`, `CnvZeroPlusSumCpp` y `CnvDecayPowCpp`.
  - `PlayFootSteps` (34 de 8192): `std::log10(adhesion) * 20` redondeaba dos veces; ahora `AudioLog10Times20Cpp`, que ya existía.
  - `KillPedWithCar` (25 de 8192): el `lift` del rebote se calculaba con un solo redondeo en double y el EXE redondea cada paso a 24 bits; ayudante `KpcBounceLiftScaleCpp`.
  La sonda del banco se reconstruyó con el texto nuevo de `PlayFootSteps` en su sustitución (`build_completion_probe.py`) y su hash se actualizó en `run_completion.py`. Aprobó con 28672 pares y error máximo 0.
- **Fixtures de `RpAnim43` sin NaN.** El generador de entradas ejerce ahora NaN (signo y carga distintos, señalizantes), ±inf, ±0, subnormales, magnitudes extremas, cuaterniones sin normalizar o nulos y tiempos extremos en uno de cada tres casos (2048 casos por envoltorio, sin quitar compuertas). Para que la plantilla coincida con el EXE en esos casos, sus sumas, restas y productos pasan por ayudantes x87 (`ExeAdd`, `ExeSub`, `ExeMul` y `RpNan*`, incluido `RpNanQuatToMatrixCpp`, que sigue paso a paso 0x59C080), y la ruta sin velocidad usa `AnimQuaternionDotNegativeCpp`. Aprobó (ver tabla).
- **Residuo de `_ftol2` en `GetLocalDirection`.** `GetLocalDirection` termina en `ResidueRegsCpp` con los EAX, ECX y EDX del EXE, incluida la rama n > 3; el emulador de `_ftol2` se movió a `X87Math` (`Ftol2RegsCpp`) y lo comparten `TheZones` y `Ped`. `CPedC` compara ahora los tres registros con semillas por caso, y el auditor `contrib/audit_register_residue.py` vigila el objeto `Ped`.
- **Registro de `reversing/CPed`.** No era cierto que solo se conservara el resumen: el log completo (25 MB, 2 MB comprimido) está en `reversing/CppTwoLevel5Unified/runs/CPedCompletion-*/comparisons.jsonl.gz`.

## Segundo seguimiento: los límites restantes (15/10/2026)

Tras el primer seguimiento quedaban tres límites; esto es lo que se hizo con cada uno.

- **`FrameUpdateCallBackW` y `Offscreen` sin ejercer.** La descripción anterior era inexacta. Lo que no se compila es la alternativa copy-paste de esas dos funciones; el análisis encontró que `FrameUpdateCallBackW` y `FrameUpdateCallBackT` eran código muerto (no se instanciaban en ningún sitio, salvo `T` desde `Offscreen`, que a su vez no se llamaba ni se registraba) y se eliminaron. Lo que de verdad no tenía banco con pares exactos era `RpAnimBlendClumpUpdateAnimations` (0x4D34F0; solo el banco antiguo `reversing/RpAnimBlend`, 652 pares con tolerancia 1e-5) y `FrameUpdateCallBackOffscreen` (0x4D2E10, sin hook). `FrameUpdateCallBackOffscreen` se reescribió como el EXE (llama a 0x4D1680, siempre 2D) y se registró como hook desactivado como los otros cuatro. Los dos bucles de `UpdateAnimations` encadenaban en el EXE `fdiv`/`fmul`/`fadd` en x87 con un redondeo a 24 bits por paso y el C++ los hacía en `double` con SSE; ahora pasan por ayudantes `RpUpdateSync*` y por `ExeAdd`/`ExeMul`. El banco nuevo `reversing/CppRpAnimUpdate1` (7168 pares: 4096 con los callbacks del EXE, 2048 con los cinco callbacks en C++ y 1024 de `Offscreen`) comparó en bits y aprobó tras corregir tres fallos de su fixture: la vtable falsa de las asociaciones no se rellenaba, el relleno aleatorio del paso de tiempo de una asociación desenlazada (4,7e12) colgaba `NextKeyFrame` en el EXE y la DLL, y un callback podía desenlazar a la superviviente dejando cero arrays de nodos, con lo que el callback del EXE desreferenciaba un puntero nulo. El banco tiene ahora predictor de iteraciones, cortafuegos y vigilante de 12 s.
- **Dependencia del x87 a 24 bits.** En vez de dejarla como supuesto se midió: el banco nuevo `reversing/CppX87Precision1` compara el EXE con la DLL forzando las 3 precisiones (24, 53 y 64 bits) y los 4 redondeos en 14 funciones (172032 pares, ST0 de 80 bits, ABI y palabra de control restauradas, cortafuegos contra bucles). Mostró diferencias reales en el punto de operación del juego (24 bits, redondeo cercano) que ningún banco anterior había visto: `GetRandomNumberInRange<float>` (un resultado menor que 2^-126 se redondeaba a subnormal de float en vez de conservar el exponente extendido), `ConvertDataToGameUnits` (127 a 143 casos de 1024: `accel`, la velocidad y 1/masa se redondeaban a float en cada paso y el EXE los lleva sin redondear) y, a 53 bits, `GetRadianAngleBetweenPoints` (`float(...)` frente a ST0 sin guardar). Se corrigieron con ayudantes `double` enlazados como `float` y con `HandlingConvertGameUnitsCpp`; además se corrigieron por análisis `Ftol2RegsCpp` (usa el redondeo vigente), `PedGeomShotAngleCpp`, `GeneralATanOfXYDblCpp`, `RwPlCoreFastRealToUInt32Cpp`, `PedGroupPlacerCircleCpp` y `TrainProjectionCpp`, y las rutinas que tocan la palabra de control usan `__control87_2` sin tocar MXCSR. Resultado: exacto a 24 y 53 bits y con los cuatro redondeos en las 14 funciones; el banco aprueba. Lo que no se puede cerrar: a 64 bits MSVC no tiene un tipo de 80 bits y un valor que cruza una llamada o una variable `double` pierde bits, así que hay diferencias exploratorias a 64 bits (1408 en `GetATanOfXY`, 1002 en `GetRadianAngleBetweenPoints`, 7416 en `GetAngleBetweenPoints`, 476 en `SolveQuadratic`, 124 en `GetLocalDirection`). El juego no corre a 64 bits: el proceso fija 24 bits.
- **`GetLocalDirection` con -inf.** No es un límite de la conversión: el bucle «sumar 2π mientras sea negativo» no termina con -inf ni con valores de ±2^27 o más en el EXE tampoco, y la conversión reproduce ese comportamiento a propósito. El banco de precisión acota esas entradas (y conserva NaN) con un cortafuegos de predicción y un vigilante para no colgar el juego.

## Bancos

| Banco | Pares |
| --- | ---: |
| BoneNodes | 12256 |
| CGeneral | 65536 |
| CPedB | 7232 |
| CPedC | 15552 |
| CPedCompletion | 28672 |
| CPedE | 10368 |
| CTaskDead | 1792 |
| CTasks14b | 12352 |
| CTasks15 | 19712 |
| Core9 | 19968 |
| CppPedFootEmitters1 | 3072 |
| CppRpAnimUpdate1 | 7168 |
| CppTrigBirdsRender | 512 |
| CppTrigClimb | 4608 |
| CppTrigCompound6Birds | 2560 |
| CppX87Precision1 | 172032 |
| Data23 | 13904 |
| DependenciesTask10 | 6144 |
| DependenciesTaskCtor4 | 4096 |
| DependenciesTaskPed5 | 8192 |
| DependenciesZones13 | 2048 |
| FxPost1 | 15488 |
| GeometryCore355 | 4608 |
| GeometryRoutes355 | 3072 |
| GroupPlacerPending | 3072 |
| IK20 | 11264 |
| Object48 | 18368 |
| PickWeap1 | 11020 |
| RequestedGeometryPlanesV2 | 2048 |
| RequestedGroupPlacerCorrectedV2 | 1536 |
| RpAnim43 | 17344 |
| TrainPosition355 | 1024 |
| TrainWave3 | 1024 |
| WeaponFinal2 | 7808 |
| WeaponInstant355 | 20480 |
| World11 | 16704 |
| World16 | 19680 |

## Límites

- A 64 bits de precisión de la x87 hay diferencias exploratorias en cinco ayudantes que pasan un `double` por la pila entre dos operaciones del EXE (cifras arriba); cerrarlas exigiría un tipo de 80 bits que MSVC no tiene. El proceso del juego corre a 24 bits.
- Hay unas veinticinco cadenas de `float` bajo `float_control(precise)` (`FightCtl*`, `ProcSurface*`, `ProjInfo*`, `BoneNode*`...) que el análisis marcó como sospechosas a 53 y 64 bits sin demostrar; el banco de precisión no las cubre.
- `GetLocalDirection` no termina con rotaciones de ±2^27 o más ni con +inf, igual que el EXE.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.
- Fallos transitorios de arranque del juego (`0xC0000409` sin ninguna comparación) en `BoneNodes` y `World16`, que aprobaron al repetir; la causa no se determinó.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| General.actual-after.obj | d210df9934de3478b5e3fa15c3ed1a6ca335217f48b2ae3ce1afc380d7fccdbc |
| Ped.actual-after.obj | 4e061a1037e2b0eef856d36d81da0939d3d7548653d210a73ccc73c4dba4607a |
| Ped.actual-before-exact.obj | f3e187d8d52162bc60f67e69d8f0e1581c996058694f16e2780c65506618f2a9 |
| Ped.actual-before-final.obj | c43d811eb6776ba7ed08342ca8310e3828603889502980457771a8135f489ae3 |
| Ped.actual-before.obj | 6fb96869e0d072fb16409851fb852b985ac5a0fa6ce9c180b6221caa73ebdca4 |
| RpAnimBlend.actual-after.obj | 3e2b90ae4271339ea16c4325dd0cb3002d5a76ca02d161d96357d30d03497f94 |
| RpAnimBlend.actual-before-exact.obj | ee0aa81153b1de162c3f406511359d68244f20cae97c9d851f5f5048502df2ce |
| RpAnimBlend.actual-before-final.obj | cc2ac427b3098f25a7a942179d34e1d79664af478dcbfd0f66d90e3b30b68270 |
| RpAnimBlend.actual-before.obj | 37fdd7956bca48a63209f2eafda90842e32bbad6eebcc8ffbc7bd8b7b81dabc7 |
| TheZones.actual-after.obj | cb3623e57e9511e51ec71cb83fb54072b7e2eb0212e463204ff709bfc9a1ba93 |
| TheZones.actual-before-exact.obj | 9e5bf61651a422ed7e756288eeffe3cb9c61422c86f3e26b434486d11616007f |
| TheZones.actual-before-final.obj | 79092b7101d833ea527cc92ff00d7332fd5d7c5d7e1e8d0f8d101a2e07319440 |
| X87Math.actual-after.obj | 28c9a0269d4c7e1df13eed59ee87e7f8c44fad947aead7cbbc9b2a9f24652ac7 |
| X87Math.actual-before-exact.obj | e6ffe2e45e7977810c08e997e8b7d51b70bceb82e188d28db6d7670595026425 |
| X87Math.actual-before-final.obj | 0143be7ea2dad051b83e20a08b91d8504256c55dbed8f1f647395b7ceac58966 |
| X87Math.actual-before.obj | b9a6c03cd0a62daa596c043bcb08c74e2f329c6b31ba714c62af4d4634d55c84 |
| cHandlingDataMgr.actual-after.obj | 0f3adec575f8c500e9f7b8e9f6db8714252d7536ccb4f797b20528fb125bce3c |
