<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ropes, ShotInfo, SimpleTransform, Sprite y EntitySeekPosCalculatorRadiusAngleOffset sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `Ropes.cpp`, `ShotInfo.cpp`, `SimpleTransform.cpp`, `Sprite.cpp` y `EntitySeekPosCalculatorRadiusAngleOffset.cpp` (10 apariciones, 8 funciones fuente). El análisis de cada archivo lo preparó un subagente (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CRopes::FindCoorsAlongRope` (0x555E40) | `RopesInterpolateCpp` (nuevo, `float_control(precise)`) |
| `CShotInfo::AddShot` (0x739C30) | `ShotInfoDestroyTimeCpp` (nuevo) |
| `CShotInfo::Update` (0x739E60) | `PowScaleVector3Cpp` (ya existía) |
| `CSimpleTransform::UpdateRwMatrix` (0x54EF40) | `TrigSimpleSinCpp` / `TrigSimpleCosCpp` (ya existían) |
| `CSimpleTransform::Invert` (0x54EF90) | `SimpleTransformInvertCpp` (nuevo; escribe x antes de releer base.x como el EXE) |
| Quad de sprites (0x70D490, 0x70E780, 0x70EDE0) | `SpriteQuadCoeffsCpp` (nuevo) |
| `RenderBufferedOneXLUSprite_Rotate_Dimension` (0x70EAB0) | `SpriteDimensionTermsCpp` (nuevo) |
| `CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos` (0x6946F0) | `TrigSimpleSinCpp`, `TrigSimpleCosCpp`, `TrigSimpleSinMulCpp` (ya existían) |

## Bancos (cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| Misc61a | 20992 | FindCoorsAlongRope (768) |
| Fx43 | 15552 | AddShot (1024) y Update (512) |
| CEntA | 16384 | Invert vía GetIsBoundingBoxOnScreen (1024) y UpdateRwMatrix vía CEntity::UpdateRwMatrix (512), con el cuerpo real |
| World15 | 17312 | las cuatro funciones de sprites (1024 cada una) |
| Misc56 | 6464 | ComputeEntitySeekPos (1024) |

`CSimpleTransform` no tiene banco propio: CEntA ejerce `Invert` y `UpdateRwMatrix` con su cuerpo real a través de `CEntity`, y esa cobertura es indirecta.

## Lo que reveló la validación

- **Misc61a fallaba en HEAD** en `CPedGroup::PlayerGaveCommand_Gather` (512/512), ajena al lote: el banco solo interceptaba las direcciones del EXE de `CFormation::GenerateGatherDestinations`, `GenerateGatherDestinations_AroundCar`, `DistributeDestinations` y `ReturnDestinationForPed`, que `PedGroup.cpp` llama directo. Se añadieron los `EntryPatch` de las entradas de la DLL con los mismos Fakes en `reversing/Misc61a/LiveComparison.hpp` y el banco aprobó con 20992 pares. La corrida fallida queda en `CppFiveFilesBUnified/runs/Misc61a-20261014-190137-536820`.
- Los otros cuatro bancos aprobaron a la primera.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (denormales, desbordes). Los colaboradores no se validan por probar a sus consumidores. No se revisó el código máquina de los ayudantes nuevos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| EntitySeekPosCalculatorRadiusAngleOffset.actual-after.obj | dfabf967d729ccbd719d3362ca2233e0b975fb245132539bb1b80a95c070a5c0 |
| EntitySeekPosCalculatorRadiusAngleOffset.actual-before.obj | ff0a94b35526264104a8fa10622141299710e130a10940275e523b2d72a3bcaa |
| Ropes.actual-after.obj | 94054da8e756fe50f144d438063e90fb09619b4748f4a31148869aa7583adeb7 |
| Ropes.actual-before.obj | 725e223ea3145ca5889f0de4599df90c3e92e5a85b7439350a15b414837a4d9d |
| ShotInfo.actual-after.obj | f5736ef61b04acf9df4e582b0bbcb781af03cb7d5645959a7bc747ae94c7df4a |
| ShotInfo.actual-before.obj | e27b6a4674699f310af4a150e133db2bc1c7de73588fdc9bd388210d3ff9c524 |
| SimpleTransform.actual-after.obj | 73365d41195a4e486bd5b4dc86c5c3733dde32d9407954ac84194f46e325b883 |
| SimpleTransform.actual-before.obj | 43a53fcd0581dc08d6ab0449dcf67529e25e021a6e48cd434567fc0d66c32146 |
| Sprite.actual-after.obj | d5a719090e7229430d9d466aa06886226ecfe740533a3db9d77e064a6876a367 |
| Sprite.actual-before.obj | fd64603c36ebb61ee6fa3a31897cfa7be2f5d41556665147b3362e8b202fb66c |
| X87Math.actual-after.obj | b1ace32161e650230d647f0a20abc18cdda3a4d1fb082c7be2e562a646d8ce30 |
| X87Math.actual-before.obj | 1c6ee3d5fea2fa37b3c8ecb9fb47365964bd07ed2abf6899bdd7a1a3cccb4cc5 |
