<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Diecinueve archivos de nivel 1 sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `3dMarkers.cpp`, `Audio/Entities/AEPedAudioEntity.cpp`, `Audio/Loaders/AEMFDecoder.cpp`, `CarAI.cpp`, `ClothesBuilder.cpp`, `Cover.cpp`, `Door.cpp`, `Entity/Entity.cpp`, `EntityScanner.cpp`, `FireManager.cpp`, `Plant/ProcSurfaceInfo.cpp`, `RenderWare/rw/rwplcore.h`, `Scripts/TheScripts.cpp`, `Audio/AEAudioUtility.cpp`, `Audio/Hardware/AEAudioHardware.cpp`, `IdleCam.cpp`, `Tasks/TaskTypes/TaskSimpleFightingControl.cpp`, `Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp` y `WeaponEffects.cpp` (54 funciones fuente). Un subagente por archivo preparó el análisis (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó y los 26 bancos aprobaron a la primera, sin tocar ninguno; tras simplificar los ayudantes de `AEMFDecoder` (sin `fma`/`nextafter` ni `<cmath>`) se repitieron los 26 con el código final.

## Qué se hizo

Cada función se comparó con el desensamblado del EXE y se convirtió a float puro en el mismo orden (con `float_control(precise)` cuando `/fp:fast` podía reasociar), a ayudantes ya existentes de `X87Math` o a ayudantes nuevos en `X87Math` (los nombres están en `approvals.json`). `rwplcore.h` tenía dos macros con ensamblador (`RwFastRealToUInt32Inline` con `__asm` y `int32fromreal` con `_asm`; el recuento léxico cuenta las tres apariciones) y se convirtieron las dos.

Correcciones sobre el código anterior que encontró el análisis:
- `AEPedAudioEntity::Service`: el volumen logarítmico daba `log10(NaN)` en lugar de −5 con NaN y redondeaba el logaritmo a float antes del producto.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
| AnimAudio6-20261014 | 13568 |
| AudioHw54-20261014 | 9280 |
| AudioPedPending-20261014 | 4608 |
| AudioWave2-20261014 | 18496 |
| CEntA-20261014 | 16384 |
| CEntC-20261014 | 4608 |
| CarAI25-20261014 | 11520 |
| ClothesArray355-20261014 | 2048 |
| ClothesGeometry355-20261014 | 5120 |
| FightingControlPending-20261014 | 1024 |
| Fire24-20261014 | 9344 |
| FootWave4-20261014 | 5120 |
| FxPost1-20261014 | 15488 |
| IdleAttractorPending-20261014 | 2048 |
| IdleCamWave4-20261014 | 3584 |
| MFDec-20261014 | 4096 |
| Markers-20261014 | 17056 |
| Misc8-20261014 | 11712 |
| Ped9-20261014 | 14720 |
| PlayerFighterPending-20261014 | 4096 |
| ScannerDragFx-20261014 | 9216 |
| ScriptsFinal3-20261014 | 16384 |
| Small2-20261014 | 32016 |
| Sys10-20261014 | 17088 |
| TaskFinal2-20261014 | 3840 |
| World16-20261014 | 19680 |

## Límites

- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (denormales, desbordes, NaN en la mayoría de los fixtures).
- `CarAI::ExeForward` y `ExeRight`: su rama sin matriz (la que tenía el ensamblador) es código muerto, porque `UpdateCarAI` lee `m_matrix->GetUp().z` sin comprobar nulo antes de llamarlas y el EXE se cae con `m_matrix` nulo; ningún fixture puede alcanzarla. Se convirtió igualmente para eliminar el ensamblador, y CarAI25 ejerce la rama con matriz.
- Código máquina: se listó la secuencia x87 de los 98 ayudantes de `X87Math.obj` con `reversing/CppOnlyMigration/herramientas/coff_x87_listar.py` (resultado en `x87-helpers-sequences.txt`). Los ayudantes con seno, coseno, `fpatan` o `fyl2x` salen en línea (sin `fsincos`, sin llamadas a la CRT para sin/cos/atan2/log10) y sin derrames de 64 bits entre el transcendental y su uso; los únicos `fstp`/`fld` de 64 bits intermedios son argumentos de `ExeCompatiblePowCpp` o valores ya redondeados a 24 bits. Eso comprueba la forma de las secuencias, no su igualdad con el EXE: esa la dan los bancos.
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| 3dMarkers.actual-after.obj | d09fc7151b77749b2f0ab3a09627f90d18affd1174445fdaefc86b5f1dc6c4fc |
| 3dMarkers.actual-before.obj | 5f730ffe70596a53af5eabdc43ba6ef31fc9ea08a3e5c5d78c511a516aad601c |
| AEAudioHardware.actual-after.obj | bff3643452c5581a459aa5a55f15428699fd9f776435c94cc59ac00e375f5c21 |
| AEAudioHardware.actual-before.obj | a4110f482575ce866ba0095d9b601cbd0abda1d98fa5b7ad5cf8474959db0909 |
| AEAudioUtility.actual-after.obj | ffb3dd6fb76ced1010fd69d656f7de65c8c838e8896a1fcf9d049843af414578 |
| AEAudioUtility.actual-before.obj | 9a2c97584ad1fb7c7388595bc6b2dd9b99f8add06d9f0ceb01d7a09bbe170faa |
| AEMFDecoder.actual-after.obj | a87d88aa2151d49540a3f0aff6f93be59f479024e392022e120664564444b4ef |
| AEMFDecoder.actual-before.obj | 118616e3a8af3da6898aedf54b413d39a1c513dec2ca148d992f05ff3ce34950 |
| AEPedAudioEntity.actual-after.obj | 8c5781bb39629785d04855314c3f0bd5724b49eab1650e98ea1a10dac9f379a5 |
| AEPedAudioEntity.actual-before.obj | ef0162ed90768b92377a17b652667bd655eedae37ddb72c84c5d35aa114e0024 |
| CarAI.actual-after.obj | ade4ecde4e6a806782f6a1e406a41462eaf019017664b269a9dc10c0cc35e3e4 |
| CarAI.actual-before.obj | 5920140d1bc2a154000d06c2af646a0469f9a0cb129fa6c8bcf37d4843c758d0 |
| ClothesBuilder.actual-after.obj | 5738f3375b0f6b9622357c1ccf5b132b68b3a6cccee2cc1a3f4053b576c0f90c |
| ClothesBuilder.actual-before.obj | 509b0ddf3eac603a5cb308167f2dec192a919d18e117c638d55b552a6e1f6e2f |
| Cover.actual-after.obj | fe594d6371e51e409de2b7fc7a650b71ba032d4af9178e3c24eff4d160d0f907 |
| Cover.actual-before.obj | 9c7a2112672893a6752d81bb0ca7e5e20e7fa0f06182f5135059d27490a9a148 |
| Door.actual-after.obj | 1564291eadb7e9016d73cb188b780df36286591ed5238f0884dc38feee8f2395 |
| Door.actual-before.obj | 524d3bdd2cd1f86c92fe60edfbcc96d4bcdeae06b1b0be38e8aeeae12aa2c18b |
| Entity.actual-after.obj | 537029fee44d4c86ee3ae187b6f99f19e5085bf29974a10c1be524e7604704e5 |
| Entity.actual-before.obj | fa1d8347a41420b1583e69c13ed8110dddd6ff6e6b91e5ff88a704319b82b6b9 |
| EntityScanner.actual-after.obj | f66c1a70abe9b73bdc06e69ea2ec523ae7ffaae48359e34bbb966efe72507b49 |
| EntityScanner.actual-before.obj | d96668bb045e88b8a9d3426e84d9c7a9698e367ff00e159cbe6eb3d84209f5b7 |
| FireManager.actual-after.obj | dbb0a1c5e6891dc467096de189a14976de0af6a64ee76f73c9791a0da182201a |
| FireManager.actual-before.obj | 8a473a21ada0952bf6b8dead981418205fe934c9a758c0b16abf0de22f90b336 |
| IdleCam.actual-after.obj | 8b7f82156a727841f6b2422bb2be13dc3db1c15a4021d62dc8ce2d6e64206a7e |
| IdleCam.actual-before.obj | 3c79fca3b53aa169b6ead85c33f4ea8703ea9ec8b58affd0d791ab134cde3b03 |
| PostEffects.actual-after.obj | 44fd5c26827168b41a5eb87a03f424ccf87af90555664f6acb99b8aa7fe979a0 |
| PostEffects.actual-before.obj | 470f0a34c2b195191a12bbeb00cc1935a2f23a820987c961709afd5e11233570 |
| ProcSurfaceInfo.actual-after.obj | 03bedd01f4705bd711e1e4a565923eda355b8b6f0beced1db0b9ea731e95f96a |
| ProcSurfaceInfo.actual-before.obj | 46d2b3d4ae8086149066fefa26d122e94273502d1f420a0891d98a3a3e5bb5b1 |
| TaskSimpleFightingControl.actual-after.obj | 3904cf8472cf92713ad134ff1424b24d90176eef2e6b4258e45dd93b8a31d893 |
| TaskSimpleFightingControl.actual-before.obj | 9e6740afa4115aa340a4537f07fa40278df63c67bfb280e1a375f00af801cea9 |
| TaskSimplePlayerOnFoot.actual-after.obj | d5b39d568c308201748aa76f4c83ea320fb8eb05ffe87c0e3798062d74f7bb19 |
| TaskSimplePlayerOnFoot.actual-before.obj | 8a21b8a4375a5e760f5bb0a936b77e028017c42e247814dfcef7dfbd33d40957 |
| TheScripts.actual-after.obj | 2863a57785e2eca0d1fff95c3f61decb3515e9591bebae8d29efc8f5564614d0 |
| TheScripts.actual-before.obj | b55ddc12c8dedc3ae29589bd61208877d6d32922a2a3ed3ed20814a4e13865b2 |
| WeaponEffects.actual-after.obj | f7bd3c9bf78d0545121b949227aa3d2055271d9ef03eda18f9b76336eca1518c |
| WeaponEffects.actual-before.obj | 7c22102cdb6453f3a9ff5aeee3d5e3522fdf999a822b1533ef20aa62b7c5bcb1 |
| X87Math.actual-after.obj | 58892371ba45aee6837c2053c7db9a8b16fbf84ca1fd43fa1d8bc04a394c5248 |
| X87Math.actual-before.obj | 490fe086c934a416b741cc92c1e5379dfcc98e220ceacebfd2773c4afa055ee0 |
