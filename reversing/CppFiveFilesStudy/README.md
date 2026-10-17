<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fire, PlayerInfo, PointLights, Population y Replay sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `Fire.cpp`, `PlayerInfo.cpp`, `PointLights.cpp`, `Population.cpp` y `Replay.cpp` (10 apariciones, 7 funciones fuente). El análisis de cada archivo lo preparó un subagente (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó y los 7 aprobaron a la primera.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CFireManager::StartFire` sin objetivo (0x539F00) | `FireBurnTimeCpp` (nuevo, `float_control(precise)`) |
| `CFire::ProcessFire` (0x53A570) | `IntFractionCpp` (nuevo) y `UnsignedTimesFloatCpp` |
| `CPlayerInfo::AddHealth` (0x56EAB0) | `FloatPlusIntCpp` (ya existía) |
| `CPlayerInfo::EvaluateCarPosition` (0x56DAD0) | `PlayerInfoAtanFromCpp` (nuevo; recibe `GetATanOfXY` por puntero) |
| `CPointLights::AddLight` (0x7000E0) | `PointLightsInRangeXYCpp` y `PointLightsDistanceFadeCpp` (nuevos, `precise`) |
| `CPopulation::CreateWaitingCoppers` (0x6133F0) | `TrigCompoundSinMulAdd`, `TrigCompoundCosMulAdd` y `TrigAtan2Cpp` (ya existían) |
| `CReplay::ProcessLookAroundCam` (0x45D760) | `TrigAtan2Cpp` y `ReplayLookAroundOffsetsCpp` (nuevo) |

Los ayudantes de Fire, FireBurnTimeCpp e IntFractionCpp, y el de Replay salen del stash `lote-28-archivos-sin-terminar`, verificados contra el desensamblado del EXE. Los de PlayerInfo y PointLights son nuevos.

## Bancos (cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| Fire24 | 9344 | StartFire sin objetivo (1024) |
| Fx43 | 15552 | ProcessFire (1024) |
| Player28 | 7552 | AddHealth (1024) y EvaluateCarPosition (1024) |
| DependenciesPointLights1 | 2560 | AddLight (2560) |
| DependenciesPointLights2 | 1024 | Init y RemoveLightsAffectingObject (hash de PointLights.cpp cambiado) |
| PopFinal4 | 4096 | CreateWaitingCoppers (512) |
| Replay47 | 18208 | ProcessLookAroundCam (1024) |

Antes de convertir, los subagentes revisaron cada banco contra el código fuente buscando colaboradores que la DLL llame directo y el banco intercepte solo por la dirección del EXE (el fallo que apareció en lotes anteriores); no encontraron ninguno y no hizo falta tocar ningún banco.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (por ejemplo, desbordes o denormales). Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| Fire.actual-after.obj | d7c33d9207942a2ccef17655cfd39465e45b94440601ee185e557fb531014fea |
| Fire.actual-before.obj | 83893dac429791e5712c3caee18686195abb4afe90afc9a16db39fc2622d00d8 |
| PlayerInfo.actual-after.obj | 1d209e7016cfb828938a3c2f67bbc097bacd0aaca8925e85ea3ca869745428c1 |
| PlayerInfo.actual-before.obj | e7e6071c59908f44364b357bf3c8b37f890d5e2d76bb9cb20d3b25d4397534fd |
| PointLights.actual-after.obj | 5009cfeb9987e0480990e2794afef284d15409ddaaeb648d9bc5fa36c0f775fd |
| PointLights.actual-before.obj | fcde7b514569f7fa913089a88b0d334ef0de34804add6df887c80ca7230b521d |
| Population.actual-after.obj | 6970c52f773ec30a5b8497418b459af36921b39c9dcf25f1ae4e9eb94d91788b |
| Population.actual-before.obj | b2b92a33829e23ac589736bc837099e1009e722c43f8c95a7b18a00a6dc297fd |
| Replay.actual-after.obj | b4f845f2d627a6b2d68af0caeb0f139ac8c6a1f09bdd999ce8224993b241cb2d |
| Replay.actual-before.obj | 0d1c523e9ab19ac28bb8201fc5e227c28e8031470b0af2c1a8b9065a1eea50f5 |
| X87Math.actual-after.obj | 1c6ee3d5fea2fa37b3c8ecb9fb47365964bd07ed2abf6899bdd7a1a3cccb4cc5 |
| X87Math.actual-before.obj | 496bfc3d2949737b2756d327b6f42def29e3ef239a519694090eb9782133d024 |

Nota (15/10/2026): el archivo local `X87Math.actual-before.obj` de esta carpeta (ignorada por git) se sobrescribió por error al preparar otro lote y no se pudo recuperar (el estado intermedio de `X87Math.cpp` de entonces no está en git); el hash de arriba es el que se registró entonces. Lo que había en su lugar se conserva con la extensión `.sobrescrito` y es el objeto «antes» del lote de [cinco archivos de nivel 2](../CppFiveLevel2Study/README.md).
