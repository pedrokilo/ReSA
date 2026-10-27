<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ensamblador x87 en nueve archivos (12/10/2026)

Conversión del único bloque de ensamblador de `BouncingPanel.cpp`, `Checkpoints.cpp`, `Coronas.cpp`, `CutsceneMgr.cpp`, `DecisionMakers/Decision.cpp`, `EntryExitManager.cpp`, `FileLoader.cpp`, `GameLogic.cpp` y `Gamma.cpp`. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md).

## Qué se hizo

| Archivo / función | Fragmento original | Solución en C++ |
| --- | --- | --- |
| `BouncingPanel.cpp`, `MultiplyByPow` (usada por `CBouncingPanel::ProcessPanel`, 0x6F49A0) | `fld damp; fld step; call 0x822130 (_CIpow); fld st(0); fmul/fstp` ×3 | `PowScaleVector3Cpp(float*, damp, step)` en `X87Math`; el factor queda en ST0 con la precisión completa y la tercera multiplicación lo consume |
| `Checkpoints.cpp`, `CCheckpoints::SetHeading` (0x722970) | `fcos` / `fsin` | `TrigSimpleCosCpp` y `TrigSimpleSinCpp` (existentes) |
| `Coronas.cpp`, `SinTimesTenX87` | `fsin; fmul 10` | `TrigSimpleSinMulCpp(angle, 10.0f)` (existente) |
| `CutsceneMgr.cpp`, `CCutsceneMgr::FinishCutscene` (0x5B04D0) | `fild` + suma de 2^32 si es negativo + `fmul 0.001f` | `UnsignedMillisToSecondsCpp(uint32)`; se eliminaron las dos constantes locales |
| `Decision.cpp`, `ProbabilityByte` (usada por `CDecision::Set`, 0x600570) | `fnstcw; or 0C00h; fldcw; fistp qword; fldcw` | `static_cast<uint8>(static_cast<int64>(value))` en C++ puro |
| `EntryExitManager.cpp`, `CEntryExitManager::AddOne` | `fcos` / `fsin` del ángulo en grados | `TrigSimpleCosCpp` y `TrigSimpleSinCpp` (existentes) |
| `FileLoader.cpp`, `ExeCIacos` (usada por `LoadObjectInstance`, 0x538090) | `call 0x822380 (_CIacos)` | `ExeAcosCpp(float)` en `X87Math`: reproduce `_CIacos` sin llamar al EXE |
| `GameLogic.cpp`, `GetHeadingMinusHalfPi` (usada por `UpdateSkip`, 0x442480) | `fld -fx; fld fy; fpatan; fsub pi/2` | `HeadingMinusHalfPiCpp(fx, fy, pi/2)` |
| `Gamma.cpp`, `CGamma::SetGamma` (0x747200) | `fld p; fmul 65535.0; fstp float` | `GammaScaleCpp(double)` |

Los cinco helpers nuevos viven en el único módulo `X87Math.cpp/.h`, con `/O2 /Oi /arch:IA32 /fp:fast /Zi` por archivo (opciones que ya tenía), y devuelven un `float` guardado y recargado (`volatile float`) como las funciones originales.

## Comprobaciones en los objetos reales

Desensamblado de `X87Math.obj` (`dumpbin /disasm`), sin llamadas a la CRT salvo las esperadas:

- `PowScaleVector3Cpp`: `fld damp; fld step; call __CIpow; fld [x]; fmul st,st(1); fstp [x]` ×2 y `fmul [z]; fstp [z]` consumiendo ST0: la misma secuencia que el ensamblador.
- `UnsignedMillisToSecondsCpp`: `fild dword; test; jns; fadd 2^32 (double); fmul 0.001f (double exacto); fstp; fld`.
- `HeadingMinusHalfPiCpp`: `fld fx; fchs; fld fy; fpatan; fsub pi/2; fstp; fld`.
- `GammaScaleCpp`: `fld qword; fmul 65535.0; fstp dword; fld`.
- `ExeAcosCpp`: dos `_control87` (palabra `(anterior & PC) | RC_NEAR | MCW_EM`), `fld1; fadd; fsub; fmulp; fsqrt; fxch; fpatan`, `fstp` y restauración.
- `Decision.obj`: la conversión usa `__ftol3` (SSE2) en vez de `fldcw`/`fistp`; para NaN y desbordes ambas dan el entero indefinido con byte bajo 0.

## Revalidación

Bancos históricos completos con `herramientas/rerun_hist.py`; evidencia en `reversing/CppNineFilesUnified/runs/`. Sin diferencias en ninguno.

| Banco | Función(es) afectadas | Pares | Corrida |
| --- | --- | ---: | --- |
| World52 | `ProcessPanel` (pow) | 18384 | 20261011-235740-793351 |
| Data8a | `SetHeading` | 18176 | 20261011-235910-438220 |
| World11 | `SinTimesTenX87` | 16704 | 20261012-001007-834422 |
| Cutscene35 | `FinishCutscene` | 13184 | 20261012-000047-638033 |
| DecisionPending | `CDecision::Set` | 3072 | 20261012-000217-136295 |
| World18 (+ FixBugs 10304, 20261012-000316-690576) | `AddOne` | 7808 | 20261012-000429-084424 |
| FileLoader47 | `LoadObjectInstance` | 11968 | 20261012-000828-667078 |
| GameLogic27 | `UpdateSkip` | 13888 | 20261012-000558-949305 |
| Misc70 | `SetGamma` | 12416 | 20261012-000721-911291 |

## Una tanda inválida (error mío)

La primera tanda se lanzó con `nohup … &` y, al creer que había muerto, se lanzó otra mientras la primera seguía; las dos compartían el clon, el candidato de bloqueo y la DLL, y fallaron con «Cerrar GTA antes de la prueba», `reversed-tests.lock` ya existente, DLL en uso y un `FALLO` de FileLoader47. Esas corridas (`FileLoader47-20261011-235243…`, `FixBugs-20261011-235035…`, `World52-20261011-234619…`, `World11-20261011-234811…`) se conservan en `runs/` pero **no cuentan**; las cifras de arriba son de la tanda única, en serie, que las repitió.

## Límites

- Los bancos no tienen auditor propio: la comprobación es el analizador de cada banco y el resumen de la corrida; sin cobertura exhaustiva de x87 más allá de sus fixtures.
- `PowScaleVector3Cpp` usa `ExeCompatiblePowCpp` (pow de la CRT de la DLL con las excepciones de C99 del EXE). El algoritmo y las tablas del `_CIpow` del EXE y del `__CIpow` de la DLL son idénticos (76 de unos 80 bloques iguales; los demás sólo se activan con entradas especiales) y el banco [PowEquivalence](../PowEquivalence/README.md) lo confirma bit a bit: rejilla de 28224 pares de especiales, 536 870 912 casos por regiones de (signo, exponente) y 2^26 casos de la función; el dominio completo de dos floats (2^64) no se enumera. Las banderas de excepción difieren en el desbordamiento a ±infinito y en NaN^±0 (inobservables; detalle en ese README).
- `ExeAcosCpp` repite la fórmula de acos que ya contiene `AnimCalcThetaCpp` (que la necesita sin redondear); no se unificaron para no alterar una función validada. Fuera de [-1, 1] el EXE entra en una rama de error y puede devolver otro patrón de NaN.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

Copias «antes» y «después» en `artifacts/` (ignoradas por git):

| Objeto | SHA256 |
| --- | --- |
| BouncingPanel.actual-before.obj | 53be1747d0a4a3afb6ee11360fa517e807af91d20628d3dc0b2f207a68f608f5 |
| BouncingPanel.actual-after.obj | cfb81210891508fb3f5a23f9e4fa6db77a0b74daef6b106a3d019ec239e51283 |
| Checkpoints.actual-before.obj | 1e3d038f9e250162dbefa1d6a4d7c6f1bae1b47a27f473ef70bdf2882c4587b3 |
| Checkpoints.actual-after.obj | 58223b119e227d38741d26deac1dad130eba54b4bf8bc94f4da208578b557bea |
| Coronas.actual-before.obj | 8b7d49b8fa24f7072a5b972f307b6d683e4a6535352cecf6e3d8330c63da7c64 |
| Coronas.actual-after.obj | 667263e1301887af85f257f847838a9cd105f635620a3f1b129ca54f0e11b27d |
| CutsceneMgr.actual-before.obj | 35a89606fe865f49e5a5377e572e0d1bfc5919c086ccb459fcf1b0e65630ae6e |
| CutsceneMgr.actual-after.obj | 0f2aa9a0b10f21b72d586137d129bdd50d5b7d7e7555155f4bad1fd90fee314c |
| Decision.actual-before.obj | 4d48fc7ff44ddc4257045367d470ea9b8cbea110a33fe53678d2a76ec32a48f7 |
| Decision.actual-after.obj | 26d8a5450a5695ae72f1e07bed0c6e5add84e8e229aea4710b8a36c7e7daf42e |
| EntryExitManager.actual-before.obj | 4c090d83498a3506620a82231a0f511b0033400a3715b726a953fc6e41b2dd0d |
| EntryExitManager.actual-after.obj | 0e2268ed97dead3206f095ae7a3453b108a32ac2abb611c19807a8a80bb9062c |
| FileLoader.actual-before.obj | 50d9c1690fe755ab743ebe692ae06cd10e335f3e36e7b0be159e9c49f25fc3ea |
| FileLoader.actual-after.obj | 5da9d120ca840875f026775b0372b7ce725f709a87dcd2e3fb785c046d9a03c1 |
| GameLogic.actual-before.obj | 3a3abdfe121a35e9dc4a8108ca1081b60a7361c77a20e96b5b456c53088a1dcb |
| GameLogic.actual-after.obj | 28219546defb04cde160301392a34693217644e1863ac399f696cec94e6217c5 |
| Gamma.actual-before.obj | 479b31496f5ecb4c0eb8a54530bb2604801053ba3f7794f16525b0f7f6c8f740 |
| Gamma.actual-after.obj | 573d0be81cd6b98ab2e5370226b4b50cd505ec72a8a419a43483cec84783b635 |
| X87Math.actual-before.obj | ba5106908976069dbaca04a9ac54248fbab513b4c0855c98e44083d0f9bb0699 |
| X87Math.actual-after.obj | 9c6f3b1deb1965ec35245be64173f5eb3024a038afe1a14e9e1bec9e4b5c0f02 |
