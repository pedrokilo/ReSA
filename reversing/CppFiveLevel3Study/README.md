<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cinco archivos de nivel 3 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `Events/EventSource.cpp`, `General.cpp`, `PlayerRelationshipRecorder.cpp`, `Ragdoll/BoneNode.cpp` y `Tasks/TaskTypes/TaskSimpleClimb.cpp` (20 funciones fuente). Un subagente por archivo preparó el análisis y otros diagnosticaron los fallos de banco (sin ejecutar nada); compilación y bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

- `General`: `GetRandomNumberInRange<float>`, `GetRadianAngleBetweenPoints`, `GetATanOfXY` y `SolveQuadratic` con ayudantes `General*Cpp` de `X87Math`. `GetATanOfXY` devuelve el `fpatan` puro sin redondear en ST0 con el mismo mecanismo que `Placeable` (ayudante `extern "C"` que devuelve `double` y alias `/alternatename` como `float`).
- `BoneNode`: cuatro ayudantes `BoneNode*Cpp` (`EulerToQuat`, `QuatToEuler`, `Limit`, `CalcWldMat`).
- `TaskSimpleClimb`: los cuatro ayudantes locales pasan a `TrigAtan2Cpp`, `BirdsAddAtan2Cpp`, `Atan2FromCpp` y `Atan2MinusCpp`, sin ayudantes nuevos.
- `EventSource` y `PlayerRelationshipRecorder`: residuo de registros con `ResidueRegsCpp`, `CarEnterExitEntryEdxCpp` y llamadas `uint64(__fastcall*)`; `contrib/audit_register_residue.py` ahora incluye estos objetos y una comprobación nueva de que nada escribe ECX/EDX tras las llamadas de `EventSource`.

## Qué se encontró

- `GetATanOfXY`: la primera versión devolvía `float` y `Core9` dio 97/1024 diferencias de ST0 (el resultado salía redondeado a 24 bits). Se corrigió devolviendo `double` y enlazando como `float`.
- `CGeneral` falló porque su `build_probe.py` contaba llamadas insertando un `inc` junto al `push ecx` del ensamblador, que ya no existe. Ahora cuenta con un `++` al principio de la función.
- `EventSource`: 1504/2048 pares; el código sin optimizar escribía ECX y EDX tras las llamadas (`movzx ecx,al`). Se reformuló sin escribirlos; 8192 pares sin diferencias.
- `RecordRelationshipWithPlayer`: 410/2048, solo ECX en los casos sin tarea. El hook del banco apuntaba al envoltorio y la ruta original ejecutaba el `GetActiveTask` de la DLL (ECX=0) en vez del del EXE; se corrigió la instrumentación del banco con una copia exacta del bucle del EXE. 6272 pares sin diferencias.
- `BoneNodes`: el primer intento se cayó (0xC0000409) durante el arranque del juego, en `CPopCycle::Initialise` (`POPCYCLE.DAT` no se pudo abrir en ese momento), antes de empezar el banco; el análisis estático de las conversiones no encontró ningún defecto y el segundo intento aprobó con 12256 pares. La causa exacta del fallo de entorno no se determinó.
- `CTasks15` fallaba con 5 funciones de `CTaskComplexAvoidOtherPedWhileWandering`: la DLL llamaba directamente a `CPedGeometryAnalyser::ComputeRouteRoundSphere` y a `CTaskComplexGangFollower::CalculateOffsetPosition`, que el banco solo interceptaba por la dirección del EXE; se añadieron en `reversing/CTasks15/LiveComparison.hpp` los `EntryPatch` en la entrada de la DLL con el mismo Fake.
- Después quedaba `CTaskSimpleClimb::ProcessPed` con 1 par de 2048 (caso 285), solo en la traza, también con la fuente anterior a la conversión (corrida `CTasks15-20261015-095116-951158`): el EXE alcanzaba cuatro entidades reales del mundo con `GetBoundCentre` porque el banco solo sustituía nueve de las 14400 celdas regulares. Se aplicó al banco el aislamiento de cabezas de `CppClimbWorldIsolationProposal` (adaptado, porque su parche no aplicaba): se guardan todas las cabezas regulares y en cada ejecución se vacían las regulares y las repetidas antes de inyectar el fixture, sin recorrer ni modificar entidades reales; todo se restaura al terminar. `CTasks15` aprobó entero con 19712 pares.

## Bancos

| Banco | Pares | Resultado |
| --- | ---: | --- |
| BoneNodes | 12256 | aprobado |
| CGeneral | 65536 | aprobado |
| CTasks15 | 19712 | aprobado |
| Core9 | 19968 | aprobado |
| CppTrigClimb | 4608 | aprobado |
| DependenciesDecisionRelationship6 | 6272 | aprobado |
| DependenciesTaskPed5 | 8192 | aprobado |
| IK20 | 11264 | aprobado |

`CTasks15` aprueba entero tras el aislamiento del mundo (ver arriba). La corrida fallida anterior y la intermedia se conservan en `reversing/CppFiveLevel3Unified/runs`.

## Límites

- `GetRadianAngleBetweenPoints`: el EXE decide el signo de `dy` con las banderas de `fcom` leídas tras `fdiv`/`fpatan`, y se tradujo a `dy>0`; `Core9` (1024 pares) lo compara con el EXE y no dio diferencias, pero Intel documenta esas banderas como indefinidas.
- `CalcWldMat` deja campos de relleno sin inicializar como el EXE.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| BoneNode.actual-after.obj | 52f85b4117b654c2a5151a6233a121cf87710e64b3757407dde43bae55ba66f4 |
| BoneNode.actual-before.obj | 398da24d9886ba768e4e33157a0677ae9d3dbda7d01b90e460e84e4d152b9001 |
| EventSource.actual-after.obj | fd5297929b5c793684c60adc5381d93f01d96cadc8884c2424d2b17c43ba9b34 |
| EventSource.actual-before.obj | b679838d312c83e5134b72a4ce31f3fa730d48ae49ad4c458995dec5480347db |
| General.actual-after.obj | ccbac3a33be206732dead32b972704d82be4b6af759f422352d48c366ba29069 |
| General.actual-before.obj | eb8a458e383ee29a31573b0f59062747019500e52439a403533291b60cd37d18 |
| PlayerRelationshipRecorder.actual-after.obj | 511bd2ab3798510169d0204d471937c5c1c659faf7439aa12a778a6d79928b21 |
| PlayerRelationshipRecorder.actual-before.obj | 9b79ae38405bb55b963e203738b47e42bd0ebbca11fab36cc9eff41d5b93bb67 |
| TaskSimpleClimb.actual-after.obj | 8c257e65ec7b12b89c92bf832d17b01a5cc763f043d5b398cb0b5d50fa743061 |
| TaskSimpleClimb.actual-before.obj | 7332c0572a8dedda066890bd7e989b6272a6699938a00cf07046a224bcc259f6 |
| X87Math.actual-after.obj | 01466ccb3f9b4e78f182a0321b3454b3598b94d02fadf434fad6c293067aef2e |
| X87Math.actual-before.obj | 7319ad5a88af1e61629b762e2b53e3b448112221f170ac835ca450f138473efb |
