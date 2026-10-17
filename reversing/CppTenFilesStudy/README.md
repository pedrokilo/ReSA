<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ensamblador x87 en diez archivos (12/10/2026)

Conversión del único bloque de ensamblador de `InterestingEvents.cpp`, `LoadingScreen.cpp`, `Maths.cpp`, `Models/VehicleModelInfo.cpp`, `PedDamageResponseCalculator.cpp`, `Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp`, `PlantMgr.cpp`, `Renderer.cpp`, `Scripts/RunningScript.cpp` y `Sprite2d.cpp`. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md). Continúa el [lote anterior](../CppNineFilesStudy/README.md).

## Qué se hizo

| Archivo / función | Fragmento original | Solución en C++ |
| --- | --- | --- |
| `InterestingEvents.cpp`, `InterestingSector` (0x605A30) | `fld; fadd/fsub radio; fmul 0.02; fadd 60; fstp qword` | `InterestingSectorCpp` (devuelve `double`) |
| `LoadingScreen.cpp`, `ClockSeconds` | `fild` + 2^32 si es negativo + `fmul 0.001f` | `UnsignedMillisToSecondsCpp` (ya existía) |
| `Maths.cpp`, `CMaths::InitMathsTables` (0x59AC90) | `fild i; fmul paso; fsin; fstp` | `TrigIntSinMulCpp(i, paso)` |
| `VehicleModelInfo.cpp`, `SetEnvMapCoeffCB` (0x4C88B0) | `fild` + 2^32 + `fmul 0.001f` | `UnsignedMillisToSecondsCpp` |
| `PedDamageResponseCalculator.cpp`, `ComputeWillKillPed` (0x4B3210), caída | resto x87 sin redondear, `fcomp`, `fst`, `fsub` | `FallRemainingHealthCpp` (modo `precise`) |
| `CustomCarEnvMapPipeline.cpp`, `FildMul` | `fild n; fmul f; fstp` | `IntTimesFloatCpp` |
| `PlantMgr.cpp`, `CalculateWindBending` (0x5DB3D0) | `fsin; fmul escala` | `TrigSimpleSinMulCpp` (ya existía) |
| `Renderer.cpp`, `FpatanMinus` | `fpatan; fsub rumbo` | `Atan2MinusCpp` |
| `RunningScript.cpp`, `MakeScriptAngledBox` (0x487F60, 0x488780, 0x4883F0) | llamada a `GetRadianAngleBetweenPoints`, normalización a [0, 2pi] con `fcom`, `fsin`/`fcos` | `ScriptAngledCornersCpp(ángulo, …)`; la llamada se hace en C++ |
| `Sprite2d.cpp`, `OffsetTexCoordForBilinearFiltering` (0x728150) | ocho UV con `1/(2·ancho)` y `1/(2·alto)` | `SpriteBilinearOffsetCpp` |

Siete helpers nuevos en el único módulo `X87Math.cpp/.h`. `Atan2MinusCpp` **sustituye** a `HeadingMinusHalfPiCpp` del lote anterior (era la misma operación con el resto fijo en pi/2): `GameLogic.cpp` llama ahora `Atan2MinusCpp(-forwardX, forwardY, pi/2)` y `Renderer.cpp` lo usa directamente, sin duplicar código.

## Comprobaciones en los objetos reales

Desensamblado de `X87Math.obj` (`dumpbin /disasm`):

- `Atan2MinusCpp`: `fld num; fld den; fpatan; fsub sub; fstp; fld`, igual que `FpatanMinus` y que el antiguo `HeadingMinusHalfPiCpp` salvo el `fchs`, que ahora hace el llamador sobre el mismo `float`.
- `InterestingSectorCpp`: `fld coord; fld radio; faddp/fsubp; fmul 0.02f; fadd 60.0f; fstp qword; fld qword`.
- `IntTimesFloatCpp`: `fild; fmul; fstp; fld`. `TrigIntSinMulCpp`: `fld k; fimul i; fsin`.
- `FallRemainingHealthCpp`: `fsubr; fcom; fst remaining; fsubp; fstp damage`, idéntico al ensamblador. Con `/fp:fast` el compilador habría podido simplificar `prevHP - (prevHP - factor)`, de ahí `#pragma float_control(precise, on)`.
- `ScriptAngledCornersCpp`: el compilador reescribió los bucles con otras ramas pero con la misma semántica (se comprobó rama por rama, incluida NaN: ni suma ni resta); `fsin`/`fcos` en línea; `fst` a float del `-cos·ancho` y reutilización de ese `float` para la esquina C, como el original.
- `SpriteBilinearOffsetCpp`: calcula `0.5/ancho` en lugar de `1/(ancho+ancho)`; es el mismo valor (multiplicar por 2 es exacto) salvo desbordes extremos.

## Revalidación

Bancos históricos completos con `herramientas/rerun_hist.py`; evidencia en `reversing/CppTenFilesUnified/runs/`, línea base en `reversing/CppTenFilesBaseline/runs/`.

| Banco | Función(es) afectadas | Pares | Resultado |
| --- | --- | ---: | --- |
| InterestingScan355 | `ScanForNearbyEntities` | 2048 | aprobado |
| Loading | `CLoadingScreen` | 9328 | aprobado |
| Small2 | `InitMathsTables` | 32016 | aprobado |
| VehModelInfoFinal4 | `SetEnvMapCoeffCB` | 256 propios (16384 tras corregir el banco) | aprobado tras corregir la instrumentación (ver abajo) |
| PedDamagePending | `ComputeWillKillPed` | 6144 | aprobado |
| EnvMap37 | `CCustomCarEnvMapPipeline` | 10880 | aprobado |
| Plant19 | `CalculateWindBending` | 1024 propios (10688 tras corregir el banco) | aprobado tras corregir la instrumentación (ver abajo) |
| Renderer33 | visibilidad y streaming | 18112 | aprobado |
| ScriptAngled355 | tres comandos de área angulada | 6144 | aprobado |
| Sprite2d33 | `OffsetTexCoordForBilinearFiltering` | 11136 | aprobado |
| DependenciesMathSprite2 | el mismo | 3072 | aprobado |
| GameLogic27 | `UpdateSkip` (tras cambiar a `Atan2MinusCpp`) | 13888 | aprobado |

## Fallos de bancos encontrados y corregidos

Tres bancos fallaban en funciones sin ensamblador, con los mismos recuentos antes y después de este lote (línea base con el código de `HEAD`). Los tres eran defectos de instrumentación, no de las funciones: el banco interceptaba a los colaboradores sólo en las direcciones del EXE y el candidato, ya en C++, llama a las funciones de la DLL. No se retiró ni relajó ninguna compuerta; los colaboradores sustituidos no quedan validados por ellos.

- **Plant19** (`CPlantMgr::Render` 429/512): el original llamaba tres veces por triángulo a `CCamera::IsSphereVisible` en 0x420C40 y el candidato a la versión naked de la DLL, que nadie registraba. Ahora el banco parchea también `&CCamera::IsSphereVisible` (versión con matriz). 10688 pares, cero diferencias (`reversing/CppBankFixes/runs/Plant19-20261012-005302-527936`).
- **VehModelInfoFinal4** (`PreprocessHierarchy` 153/512, `ReduceMaterialsInVehicle` 256/256): el original llamaba a `CMemoryMgr::InitScratchPad` (0x72F480) y a `CQuaternion::Set` (0x59C3E0), cuyos sustitutos del banco tenían dirección de la DLL a 0, así que el candidato ejecutaba las funciones reales sin traza ni efecto marcado. Ahora `AddStub` recibe `&CMemoryMgr::InitScratchPad` y `&CQuaternion::Set`. 16384 pares, cero diferencias (`reversing/CppBankFixes/runs/VehModelInfoFinal4-20261012-005514-284244`).
- **Game22:** la sonda no compilaba: el banco lista `Game.cpp` entre las fuentes vigiladas, pero la propia sonda lo modifica durante la compilación y la comprobación de hash fallaba siempre. `probe_build.py` exige ahora que `Game.cpp` sea exactamente lo que la sonda escribió (sigue vigilando el resto de las fuentes y el banco). 5824 pares, cero diferencias (`reversing/CppTenFilesUnified/runs/Game22-20261012-005053-648050`); cubre `InitMathsTables` además de Small2.

## Límites

- Sin auditor propio por banco: la comprobación es el analizador de cada banco y el resumen de la corrida; sin cobertura exhaustiva de x87 más allá de sus fixtures.
- `ScriptAngledCornersCpp` recibe el ángulo ya como `float`: en el original llegaba en ST0 desde la función del EXE; aquí viene de la reconstrucción en la DLL (hookeada), cuyo retorno es un `float` y por tanto idéntico.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| CustomCarEnvMapPipeline.actual-after.obj | 513b027ec896f16753e20acf29d798fe4debccc69b2b1956bc0e33f3302d05cb |
| CustomCarEnvMapPipeline.actual-before.obj | 13589b0ac01ee5ba5150416ce4e40471fb26047769077932be26a21c1227db06 |
| GameLogic.actual-after.obj | 5f54f4c080af772df9e29870755f83b12e88ed693bc2efdd49b99c403b1ce37b |
| GameLogic.actual-before.obj | 28219546defb04cde160301392a34693217644e1863ac399f696cec94e6217c5 |
| InterestingEvents.actual-after.obj | c7028cf6d0c34cd3a3c1bc90433382f867ea2c80727540edb1aa648f915dcd9b |
| InterestingEvents.actual-before.obj | 9efed722678513a76143f31fc1b6cb5c903509d1d59f5bb2452d8853cd93c21b |
| LoadingScreen.actual-after.obj | 58a910bde4bd0f3b86d307446d9ca6de082d500710f5d403781822b13e3046f6 |
| LoadingScreen.actual-before.obj | acfbb95894e3434fdfb387fb197eac1e436380303c61a2ea74acd0dabfa6d5cc |
| Maths.actual-after.obj | 4d1dadf1db32185ee8f230c087f8311af17279e4275cd40bd66fa1aab7d54245 |
| Maths.actual-before.obj | abb5501bd3d066043c93fa984d904682b33bfa4cb79301e677f0f60102fee9b0 |
| PedDamageResponseCalculator.actual-after.obj | 464ad25090a1aea1cfb820df74689581ffe5d4f138fe2a58f6e5e44dbd094325 |
| PedDamageResponseCalculator.actual-before.obj | d62a98510f77b02cacf15da993e9bc3cc3360b0cf7ec52b5e3766e284458f52b |
| PlantMgr.actual-after.obj | fdd61c4437ab29e59fcb00941c749f9ad7f7dffb6f7a7d3c3044ab2dbb5921a1 |
| PlantMgr.actual-before.obj | a1d945e9c1112c9843d8efcfebf5325f9c9c43b58d390b774102eb3ea3505653 |
| Renderer.actual-after.obj | 7774c40d812c330567ffa564e58b95f79bc68722acf090f1ea9c994e3459a7d6 |
| Renderer.actual-before.obj | 95a405b054c0bafe9e0852eb974d4f3bd25d72e493b62f667c9a8ffcc436e21f |
| RunningScript.actual-after.obj | cd30fe63c0b8517e5280dc5f7b5aa0e81e95852997634e28e517fba8a5a4aa76 |
| RunningScript.actual-before.obj | d382f1d967182b4abc2cf894767353fe1fe630017bfeb1047c621a4b0ac77be5 |
| Sprite2d.actual-after.obj | fd203cfee10a8d558c459bdf224bcff7e8bcdb52ea4d0e5e9547f12a1917a59f |
| Sprite2d.actual-before.obj | 80e2854a1ccadc8647fe9ca948ba35d5bb56d17bb256f9edbbda81e09bb55e06 |
| VehicleModelInfo.actual-after.obj | 636bbf8c7ef0c58c44fec96dc5a0934e47caaf76cd7001196330324d44327031 |
| VehicleModelInfo.actual-before.obj | 69aca7848cef7cf1b7e70b34efbc1232eb7482347ec8dc5a36449e63ac6e5d37 |
| X87Math.actual-after.obj | 618cc9e1c574a419ee7c238b9ebf16eafd959d37788df1cb183663c0daf89ad4 |
| X87Math.actual-before.obj | 9c6f3b1deb1965ec35245be64173f5eb3024a038afe1a14e9e1bec9e4b5c0f02 |
