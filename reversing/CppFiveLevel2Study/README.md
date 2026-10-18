<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cinco archivos de nivel 2 más (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `Font.cpp`, `Tasks/TaskTypes/TaskComplexWaitForPed.cpp`, `Birds.cpp`, `Entity/Placeable.cpp` y `Tasks/TaskTypes/TaskSimpleHoldEntity.cpp` (12 funciones fuente). Un subagente por archivo preparó el análisis; la compilación y los bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

- `Font`, `Birds` y `TaskSimpleHoldEntity`: C++ plano o ayudantes nuevos de `X87Math` (`BirdsAddAtan2Cpp`, `BirdsRotateCpp`, `HoldEntityRotateZCpp`) y los ya existentes (`TrigAtan2Cpp`, `Atan2MinusCpp`).
- `Placeable`: `GetHeading` y `GetRoll` devuelven el `fpatan` sin redondear en ST0 (World16 compara 80 bits). Los ayudantes `PlaceableHeadingDblCpp` y `PlaceableRollDblCpp` devuelven `double` y se enlazan desde `Placeable.cpp` como `float` con `/alternatename`; no hizo falta tocar las opciones de compilación del archivo: el objeto sin optimizar muestra `call`, `add esp` y `ret` sin `fstp` ni `fld` tras la llamada (el retorno `float` ya llega en ST0). Un intento previo de compilar todo el archivo con `/O2 /arch:IA32 /fp:fast` se descartó por innecesario.
- `TaskComplexWaitForPed::Constructor`: marco SEH manual, `ResidueRegsCpp` y `CEntity::RegisterReferenceWithResidue` (nuevo, en `Entity.cpp`), que hace lo mismo que `RegisterReference` y devuelve el EDX con que sale 0x571B70 en todas sus salidas, incluida la poda de las tres pools (`RegisterReference` lo llama y descarta el valor). `contrib/audit_register_residue.py` incluye ahora el objeto.

## Qué se encontró

- `DependenciesTaskCtor4` falló la primera vez (512 de 2048 pares de `CTaskComplexWaitForPed::Constructor`, solo el EDX): el constructor llama ahora al `RegisterReference` en C++, que deja otro EDX que la rutina del EXE. Se reprodujo el EDX de 0x571B70 sin tocar el banco; repetido, aprobó con 4096 pares. La corrida fallida se conserva en `reversing/CppFiveLevel2Unified/runs`.
- Al guardar los objetos «antes» sobrescribí por error `X87Math.actual-before.obj` de la carpeta ignorada `reversing/CppFiveFilesStudy/artifacts` con el de este lote; el original no se pudo recuperar (el estado intermedio de `X87Math.cpp` de entonces no está en git). Lo anoté en el README de ese estudio.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
| CEntB | 13824 |
| CTasks14b | 12352 |
| CppTrigBirdsRender | 512 |
| CppTrigCompound6Birds | 2560 |
| DependenciesTaskCtor4 | 4096 |
| DependenciesWorldFxFont5 | 15360 |
| World16 | 19680 |

## Límites

- El EDX de la rama en que `RegisterReference` poda las tres pools sin conseguir una referencia se reproduce a partir del desensamblado (0x571BC3..0x571C95 y 0x571A90), pero el banco nunca poda las pools del juego, así que esa rama no está ejercida por ningún par.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| Birds.actual-after.obj | c5073c22be2fbc05d7868327e098c1c520699b7324e284dda5b9a79ef7b360a5 |
| Birds.actual-before.obj | 88faa01dd9e6ded9c5a7e3b4a8352cdfa24b9b9ec5002fb12a4c670e736fec1f |
| Entity.actual-after.obj | f76a852573eb6a5726f0cbd6c8ebcf68b06295da1d0ab2fd90c7fea51920a6a6 |
| Font.actual-after.obj | d2c15b6c0324b2ccf4c76e5fcc1c83469b96c7c0a7b99bd00b9d2a59f92326bb |
| Font.actual-before.obj | 5d1506fee6c0327d6f0fd61cf1b098544013fb0e39625d594421e9e3d8d0efe6 |
| Placeable.actual-after.obj | 500d7349a4b841974f82a1d3f483ebeebb487bfd059fe56aecadb34a637fa2bc |
| Placeable.actual-before.obj | 99a99cb10e13de33867542e19e9db541acd3e6addbefa0ef58a103daf56183d2 |
| TaskComplexWaitForPed.actual-after.obj | 14bc73d5bdc7f0e81cfe3f139f82c69628d3ae847d27eac58decc5b45dfd4745 |
| TaskComplexWaitForPed.actual-before.obj | cdf65c35b09fec6fa61897874fcd3b85222a7251a25cb700dda1325772a136e0 |
| TaskSimpleHoldEntity.actual-after.obj | c45f1153decad6696730c458648c9d8da234fd37f82dd0aae399399b81ec8617 |
| TaskSimpleHoldEntity.actual-before.obj | 5d6f4bed788f5152c44f039827e272fcbb98d56bf52bf9c7387159b294127824 |
| X87Math.actual-after.obj | 7319ad5a88af1e61629b762e2b53e3b448112221f170ac835ca450f138473efb |
| X87Math.actual-before.obj | eb51d423a9638ab7e2e24e748fe86708b3b7f33d476535b845cdf3efd9b72e62 |
