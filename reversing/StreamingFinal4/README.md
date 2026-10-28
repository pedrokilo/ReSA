<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# StreamingFinal4 - banco por pares de CStreaming

Banco diferencial dentro del juego: el original (hooks desactivados, `State(false)`) frente al candidato (hooks activados, `State(true)`).
Cada caso se ejecuta dos veces con la misma semilla y se comparan los `Summary` (ret, eventos, trace_fnv, hash del mundo, arena_top, arena_fnv).

## Ficheros
- `Support.hpp`, `World.hpp`, `Stubs.hpp`, `Ctx.hpp`: utilidades, mundo falso (model infos, entidades, pools, sectores, camara), dobles de las dependencias externas.
- `CasesA..D.hpp`: familias de casos (A: listas y estado, B: recorridos del mundo, C: canales y bucles principales, D: vehiculos, peatones, arranque y ficheros).
- `LiveComparison.hpp`: tabla de funciones (94) con el numero de pares por funcion (64 a 512, 20992 en total) y el bucle de comparacion.
- `build_probe.py`, `run_local.py`: compilacion y ejecucion. Ambos redirigen en memoria `runtime_runner.GAME` a `E:/JUEGOS/GTA San Andreas - CVehicle Test` (no se edita `runtime_runner.py`).

## Metodo
- Nunca se opera sobre el streaming real: pools falsos, tablas de informacion de streaming reconstruidas por semilla, instantanea y restauracion de globales.
- `Summary()` nunca es nulo; el conjunto de parches de `operator new/delete` se cierra antes de construir el JSON de salida.
- Se filtra por el id del hilo del banco; en fallo se conservan `diff_values` y `diff_blocks`.
- Las fixtures se mantienen en el dominio real del EXE (por ejemplo, modelos pedidos con tamano distinto de cero, solo DFF/TXD en `FinishLoadingLargeFile`).

## Correcciones en Streaming.cpp / Streaming.h
Ver el informe de la sesion: unas 35 funciones corregidas frente al EXE 1.0 US (aritmetica de sectores `x*0.02f+60`, sectores LOD `trunc(x*0.005f+15)`, comparaciones estrictas en `ProcessEntitiesInSectorList`, `SetModelIsDeletable` de un argumento mas `_Impl`, etc.).

## Ejecucion
```
CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh StreamingFinal4 3
```

## Supuestos a revisar al compilar
- Campos protegidos (`m_pStreamingLink`, `m_ScanCode`) accedidos por offset sin comprobar el layout.
- Offset de `IsLoaded` de CAnimBlock asumido en `0xB5D4B0 + 0x20*idx`; radio del CColModel falso en `+0x24`.
- `ReadIniFile`: diferencias de `atof` entre la CRT antigua y la UCRT pueden dar falsos fallos.
- `LoadCdDirectory(void)` usa `GetFileSize` sobre un handle real (via 0x406360).
- Los nombres de los stubs de hooks se toman de `docs/hooks.csv`.

## Aprobación (10/10/2026)

Corrida `20261010-143008-897454`: passed=true, 20992 pares, 94 funciones registradas con 0 fallos, evento complete (state_restored), saves y configuraciones intactos. Registrada con `register_streamingfinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CStreaming queda sin pendientes.

### Alcance y límites
- Dominio del fixture: cámara dentro de ±2900, tamaños de modelo pedidos no nulos, enlaces de lista válidos y tabla de conductores de taxi real.
- Excepciones por lectura de pila sin inicializar del EXE: quedan fuera del dominio comparado.
- Mundo falso y determinista; CdStream, RenderWare, cargadores, almacenes, población, ficheros, directorio y memoria doblados con traza sin crédito.
- Aprobación histórica con `dependency_review=null`; sin acreditación vigente de dependencias.

### Funciones (94)
| Función | Pares |
| --- | ---: |
| CStreaming::AddEntity | 256 |
| CStreaming::AddImageToList | 256 |
| CStreaming::AddLodsToRequestList | 256 |
| CStreaming::AddModelsToRequestList | 256 |
| CStreaming::AddToLoadedVehiclesList | 256 |
| CStreaming::AreAnimsUsedByRequestedModels | 256 |
| CStreaming::AreTexturesUsedByRequestedModels | 256 |
| CStreaming::ClearFlagForAll | 128 |
| CStreaming::ClearSlots | 256 |
| CStreaming::ConvertBufferToObject | 512 |
| CStreaming::DeleteAllRwObjects | 128 |
| CStreaming::DeleteLeastUsedEntityRwObject | 256 |
| CStreaming::DeleteRwObjectsAfterDeath | 128 |
| CStreaming::DeleteRwObjectsBehindCamera | 256 |
| CStreaming::DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>> | 256 |
| CStreaming::DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>> | 256 |
| CStreaming::DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>> | 256 |
| CStreaming::DisableCopBikes | 64 |
| CStreaming::FindMIPedSlotForInterior | 128 |
| CStreaming::FinishLoadingLargeFile | 256 |
| CStreaming::FlushChannels | 128 |
| CStreaming::FlushRequestList | 128 |
| CStreaming::GetDefaultCabDriverModel | 128 |
| CStreaming::GetDefaultCopCarModel | 256 |
| CStreaming::GetDefaultCopModel | 128 |
| CStreaming::GetDefaultFiremanModel | 64 |
| CStreaming::GetDefaultMedicModel | 64 |
| CStreaming::GetNextFileOnCd | 512 |
| CStreaming::HasSpecialCharLoaded | 64 |
| CStreaming::HasVehicleUpgradeLoaded | 256 |
| CStreaming::Init2 | 64 |
| CStreaming::InitImageList | 64 |
| CStreaming::InstanceLoadedModels | 128 |
| CStreaming::IsCarModelNeededInCurrentZone | 512 |
| CStreaming::IsVeryBusy | 64 |
| CStreaming::Load | 64 |
| CStreaming::LoadAllRequestedModels | 256 |
| CStreaming::LoadCdDirectory-char | 512 |
| CStreaming::LoadCdDirectory-void | 128 |
| CStreaming::LoadInitialPeds | 64 |
| CStreaming::LoadInitialWeapons | 64 |
| CStreaming::LoadRequestedModels | 256 |
| CStreaming::LoadScene | 128 |
| CStreaming::LoadSceneCollision | 64 |
| CStreaming::LoadZoneVehicle | 128 |
| CStreaming::MakeSpaceFor | 256 |
| CStreaming::PossiblyStreamCarOutAfterCreation | 128 |
| CStreaming::ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>- | 512 |
| CStreaming::ProcessLoadingChannel | 512 |
| CStreaming::PurgeRequestList | 128 |
| CStreaming::ReInit | 64 |
| CStreaming::ReadIniFile | 256 |
| CStreaming::ReclassifyLoadedCars | 128 |
| CStreaming::RemoveAllUnusedModels | 128 |
| CStreaming::RemoveBigBuildings | 128 |
| CStreaming::RemoveBuildingsNotInArea | 256 |
| CStreaming::RemoveCarModel | 256 |
| CStreaming::RemoveCurrentZonesModels | 128 |
| CStreaming::RemoveDodgyPedsFromRandomSlots | 128 |
| CStreaming::RemoveEntity | 256 |
| CStreaming::RemoveInappropriatePedModels | 128 |
| CStreaming::RemoveLeastUsedModel | 256 |
| CStreaming::RemoveLoadedVehicle | 256 |
| CStreaming::RemoveLoadedZoneModel | 128 |
| CStreaming::RemoveModel | 512 |
| CStreaming::RemoveReferencedTxds | 128 |
| CStreaming::RemoveTxdModel | 128 |
| CStreaming::RenderEntity | 256 |
| CStreaming::RequestBigBuildings | 128 |
| CStreaming::RequestFile | 256 |
| CStreaming::RequestFilesInChannel | 128 |
| CStreaming::RequestModel | 512 |
| CStreaming::RequestModelStream | 512 |
| CStreaming::RequestSpecialChar | 256 |
| CStreaming::RequestSpecialModel | 512 |
| CStreaming::RequestTxdModel | 256 |
| CStreaming::RequestVehicleUpgrade | 128 |
| CStreaming::RetryLoadFile | 256 |
| CStreaming::Save | 64 |
| CStreaming::SetMissionDoesntRequireModel | 256 |
| CStreaming::SetModelIsDeletable | 256 |
| CStreaming::Shutdown | 64 |
| CStreaming::StreamAmbulanceAndMedic | 128 |
| CStreaming::StreamCopModels | 256 |
| CStreaming::StreamFireEngineAndFireman | 128 |
| CStreaming::StreamOneNewCar | 512 |
| CStreaming::StreamPedsForInterior | 256 |
| CStreaming::StreamPedsIntoRandomSlots | 256 |
| CStreaming::StreamVehiclesAndPeds | 256 |
| CStreaming::StreamVehiclesAndPeds_Always | 256 |
| CStreaming::StreamZoneModels | 512 |
| CStreaming::StreamZoneModels_Gangs | 512 |
| CStreaming::Update | 256 |
| CStreaming::WeAreTryingToPhaseVehicleOut | 128 |

## Actualización 11/10/2026

`CStreaming::Shutdown` fallaba 64/64: el banco interceptaba `CMemoryMgr::FreeAlign` sólo en 0x72F4F0 y la `FreeAlign` de la DLL pasó a ser una transcripción `naked` que no pasa por esa dirección. `LiveComparison.hpp` pasa ahora también `&CMemoryMgr::FreeAlign` a `AddStub`. Corrida 20261011-211139-086170: 20992 pares, aprobado. Evidencia en `reversing/CppStreamingFix`.
