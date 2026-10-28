<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FIX_BUGS quitados en funciones validadas

Validadas el 03/10/2026 con **10.304 pares original/candidato dentro de GTA**, todos exactos: 21 funciones de 20 clases con `validated=1`.

Decisión: una función validada se comporta exactamente como `gta-sa.exe` 1.0 US, así que sus bloques `FIX_BUGS` (y la macro equivalente `APPLY_CD_STREAM_DEADLOCK_FIX`) se quitan y queda la ruta del EXE. Los arreglos reversibles de `source/reversiblebugfixes/Bugs.hpp` no se tocan (se listan al final).

| Clase | Funciones del banco (ruta que antes era `FIX_BUGS`) |
|---|---|
| `CGlass` | `CalcAlphaWithNormal` (luz 0,57, potencia con 5 productos) |
| `CTaskManager` | `GetTaskSecondary` (índice -1 y fuera de rango) |
| `CPopCycle` | `PickARandomGroupOfOtherPeds` (sorteo sobre 100 con tablas que no suman 100) |
| `CCoverPoint` | `CanAccommodateAnotherPed` (vehículo con los dos huecos ocupados) |
| `CCover` | `AddCoverPoint` (puntos de objeto y vehículo también por distancia) |
| `RtAnim` | `RtAnimInterpolatorSetCurrentAnim` (animaciones de RpAnimBlend) |
| `CAnimBlendHierarchy` | `CalcTotalTime`, `CalcTotalTimeCompressed` (secuencias sin keyframes) |
| `CdStream` | `CdStreamRead` (posición con FileID ≠ 0) |
| `CDirectory` | `AddItem` (nombres repetidos) |
| `CEntryExitManager` | `AddOne` (pool lleno) |
| `CLoadedCarGroup` | `PickRandomCar` (pesos nulos) |
| `CCheckpoint` | `Render` (toro con alfa < 48) |
| `CTaskSimpleCarFallOut` | Destructor (callback de la animación) |
| `CTaskComplexFleeAnyMeans` | Destructor (referencia de `m_stealVeh`) |
| `CTaskComplexPassObject` | Destructor (referencia de `m_entityToPass`) |
| `CTaskComplexDestroyCarArmed` | Constructor (referencia sobre la pila) |
| `CTaskInteriorLieInBed` | Constructor (`m_bRghtHandSide` sin escribir) |
| `CPedGroupDefaultTaskAllocatorStandStill`, `...FollowLimited`, `...FollowAnyMeans` | `AllocateDefaultTasks` (peatón repetido en los pares) |

**Diferencias reales corregidas (FIX_BUGS quitados):**
- Streaming:
  - `CdStreamRead` guarda en `gLastCdStreamPosn` la posición completa (FileID en el byte alto); `CStreaming::GetNextFileOnCd` (no validada, mismo arreglo `CD_STREAM_READ_POS_FIX`) compara igual para no desemparejarlas;
  - `CdStreamInit` vuelve a poner `FILE_FLAG_NO_BUFFERING` si el sector del disco es ≤ 2048 bytes;
  - `CdStreamSync`/`CdStreamThread` sin la sección crítica de `APPLY_CD_STREAM_DEADLOCK_FIX` (vuelve el bloqueo raro del PC).
- Mundo y datos:
  - `CCarGenerator::DoInternalProcessing` nunca decrementa el contador (`movzx` + `cmp -1` + `jge`);
  - `CCover::Update` sin referencias registradas (los nodos se enlazan y desenlazan a mano, como el EXE, sin las comprobaciones de depuración de la lista); `AddCoverPoint` mira la distancia de todo punto ocupado; `CCoverPoint::CanAccommodateAnotherPed` es `!p0 || p1`;
  - `CDirectory::AddItem` añade repetidos; `CEntryExitManager::AddOne` devuelve 0 con el pool lleno; `DeleteOne` sin `destroy_at` (era trivial);
  - `CPopCycle::Initialise` reescala los porcentajes a 100 y `PickARandomGroupOfOtherPeds` sortea sobre 100;
  - `CLoadedCarGroup::PickRandomCar` devuelve -1 si la suma de pesos es 0 (0x611D16, faltaba fuera de `FIX_BUGS`); la rama "no `FIX_BUGS`" leía los huecos -1 y no era el EXE;
  - `CLoadingScreen::DoPCScreenChange` siempre avanza la pantalla; `CCheckpoint::Render` dibuja los toros con el alfa desbordado.
- Render y animación:
  - `CGlass::CalcAlphaWithNormal` usa solo la versión exacta (antes `FIX_BUGS` cambiaba toda la función);
  - `CGrassRenderer::DrawTriPlants`: `srand` con la semilla truncada (`_ftol2`), no con sus bits;
  - `RtAnimInterpolatorSetCurrentAnim` también reescribe las cabeceras de los frames de RpAnimBlend (lo que evita SilentPatch);
  - `CAnimBlendHierarchy::CalcTotalTime(Compressed)` lee el keyframe -1 en secuencias vacías.
- Memoria y tareas:
  - `CPool::Delete` (todas las `operator delete` de pool) y `CTask::operator delete` sin comprobar nulo;
  - `CTaskManager::GetTaskSecondary` sin comprobar el índice (`.data()[]`); `CTaskComplexBeInGroup::MonitorSecondaryGroupTask` lo llama también con -1;
  - destructores de `CTaskSimpleCarFallOut`, `CTaskComplexFleeAnyMeans` y `CTaskComplexPassObject` como el EXE (callback y referencias sin limpiar); constructores de `CTaskComplexDestroyCarArmed` (referencia sobre el parámetro) y `CTaskInteriorLieInBed` (`m_bRghtHandSide` sin escribir);
  - asignadores por defecto: recorren los 8 pares sin parar (y `StandStill` sin `assert` sobre la tarea previa);
  - `CTaskComplexChat` sortea [0, 3); `CTaskComplexEnterCar` pierde la utilidad anterior; `CTaskComplexFollowNodeRoute::ComputePathNodes` reutiliza la posición anterior con nodos no cargados; `CTaskComplexGangLeader::ScanForStuff` sigue tras apoyarse y mira el deambular del líder; `CTaskComplexMedicTreatInjuredPed` devuelve el peatón; `CTaskComplexSeekCoverUntilTargetDead` usa el objetivo; `CTaskComplexUseGoggles` escribe antes de comprobar el jugador; `CTaskSimpleCarOpenDoorFromOutside` usa el conductor; `CTaskSimpleClimb` sin limitar la velocidad; `CTaskSimpleJetPack` llama azul al frenar; `CTaskSimpleSwim` matriz 3; `CTaskSimpleUseGun` borra el mando;
  - `CPedAttractor::IsAtHeadOfQueue` y `CTrailer::ProcessEntityCollision` sin comprobar nulos.

**Método:**
- Fixtures sobre buferes propios; la cámara, las tablas de CPopCycle, los puntos de cobertura, `gCdStreams`/`gLastCdStreamPosn`, el pool de entradas (un pool lleno falso), los campos de los model infos de vehículos y `m_LastFreeSlot` del pool de tareas se guardan y restauran en cada ejecución.
- `CCover::AddCoverPoint` es `void` en el EXE (eax queda con basura): se comparan los 100 puntos y `m_NumPoints`, no el valor devuelto.
- `CdStreamRead` solo con canales ocupados y tras esperar a que el hilo de streaming esté parado.
- Tareas creadas: se rastrean en el pool (huecos libres con 0xCD) y se borran; referencias falsas limpiadas en cada ejecución. La dirección de pila que registra `DestroyCarArmed` sale como `other` en las dos.
- Se simulan y trazan `rand`, `C3dMarkers::PlaceMarker`, las consultas de supresión/bloqueo/descarga de vehículos y el `printf` de depuración del EXE.
- Fuera del banco (el EXE se cae, corrompe estado real o necesita el juego entero): nulos de `CPool::Delete`, `CTask::operator delete`, `IsAtHeadOfQueue`, `CTrailer::ProcessEntityCollision` y `UseGoggles`; `CdStreamInit`/`CdStreamSync`/`CdStreamThread` (hilo y ficheros reales); `GetNextFileOnCd`, `DoInternalProcessing`, `CCover::Update`, `PopCycle::Initialise`, `DoPCScreenChange`, `DrawTriPlants`, `DeleteOne` y las tareas complejas (cubiertas por sus bancos: Core9, Data8a, World16, World15, Mix12, World18, Data10, CTasks7a, CTasks14a, Route19, CTasks13, CTasks9b, CTasks10b, TasksBatch3, STasks10, CTasks15, CTasks14b, Mix11, Gun19, CTasks19).

**Bloques `FIX_BUGS` de `source/game_sa`:** 163 apariciones en 147 sitios; 55 están en funciones validadas: 50 quitados y 5 dejados (4 comentarios o código muerto y el comentario de `CTaskComplexGangLeader::ControlSubTask`). Además se quitan `CGlass::CalcAlphaWithNormal` y `CStreaming::GetNextFileOnCd` (52 en total).

| Archivo | Función | ¿Validada? | Acción |
|---|---|---|---|
| CarGenerator.cpp | `CCarGenerator::DoInternalProcessing` | sí | quitado |
| CdStreamInfo.cpp | `CdStreamRead` | sí | quitado |
| CdStreamInfo.cpp | `CdStreamInit` | sí | quitado |
| CdStreamInfo.cpp | `CdStreamSync`, `CdStreamThread` (`APPLY_CD_STREAM_DEADLOCK_FIX`, 5 bloques) | sí | quitado |
| Streaming.cpp | `CStreaming::GetNextFileOnCd` | no | quitado (pareja de `CdStreamRead`) |
| Checkpoint.cpp | `CCheckpoint::Render` | sí | quitado |
| Cover.cpp | `CCover::Update` (3 sitios) | sí | quitado (+ `ClearProcessedBuildings`, NOTSA, libera los nodos a mano) |
| Cover.cpp | `CCover::AddCoverPoint` | sí | quitado |
| CoverPoint.cpp | `CCoverPoint::CanAccommodateAnotherPed` | sí | quitado |
| Directory.cpp | `CDirectory::AddItem` | sí | quitado |
| EntryExitManager.cpp | `CEntryExitManager::AddOne`, `DeleteOne` | sí | quitado |
| Glass.cpp | `CGlass::CalcAlphaWithNormal` | no (solo por `FIX_BUGS`) | quitado |
| GrassRenderer.cpp | `CGrassRenderer::DrawTriPlants` | sí | quitado |
| LoadedCarGroup.cpp | `CLoadedCarGroup::PickRandomCar` | sí | quitado (la rama `FIX_BUGS` era la del EXE) |
| LoadingScreen.cpp | `CLoadingScreen::DoPCScreenChange` | sí | quitado |
| LoadingScreen.cpp | `SkipCopyrightSplash` | NOTSA | dejado (sin hook) |
| PopCycle.cpp | `CPopCycle::Initialise`, `PickARandomGroupOfOtherPeds` | sí | quitado |
| Animation/AnimBlendHierarchy.cpp | `CalcTotalTime`, `CalcTotalTimeCompressed` | sí | quitado |
| Attractors/PedAttractor.cpp | `CPedAttractor::IsAtHeadOfQueue` | sí | quitado |
| Core/Pool.h | `CPool::Delete` (en línea en las `operator delete` validadas) | sí | quitado |
| Tasks/Task.cpp | `CTask::operator delete` | sí | quitado |
| Entity/Vehicle/Trailer.cpp | `CTrailer::ProcessEntityCollision` | sí | quitado |
| RenderWare/rw/rtanim.cpp | `RtAnimInterpolatorSetCurrentAnim` | sí | quitado |
| Tasks/TaskManager.cpp | `CTaskManager::GetTaskSecondary` | sí | quitado |
| Tasks/Allocators/PedGroup/*.h | `AllocateDefaultTasks` (StandStill, FollowLimited, FollowAnyMeans) | sí | quitado |
| Tasks/TaskTypes/TaskComplexBeInGroup.cpp | `MonitorSecondaryGroupTask` | sí | quitado |
| Tasks/TaskTypes/TaskComplexChat.cpp | `CreateFirstSubTask` | sí | quitado |
| Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp | Constructor | sí | quitado |
| Tasks/TaskTypes/TaskComplexEnterCar.cpp | `CreateTaskUtilityLineUpPedWithCar` | sí | quitado |
| Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp | Destructor | sí | quitado |
| Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp | `ComputePathNodes` | sí | quitado |
| Tasks/TaskTypes/TaskComplexGangLeader.cpp | `ScanForStuff` (2 bloques) | sí | quitado |
| Tasks/TaskTypes/TaskComplexGangLeader.cpp | `ControlSubTask` (comentario: devuelve nulo en vez de `StandStill`) | sí | dejado: no es bloque; el EXE llamaría a una virtual pura y abortaría |
| Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp | `CreateDealWithNextAccidentTask` | sí | quitado |
| Tasks/TaskTypes/TaskComplexPassObject.cpp | Destructor | sí | quitado |
| Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp | `ControlSubTask` | sí | quitado |
| Tasks/TaskTypes/TaskComplexUseGoggles.cpp | `ControlSubTask` (V595) | sí | quitado |
| Tasks/TaskTypes/TaskSimpleCarFallOut.cpp | Destructor | sí | quitado |
| Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp | `ProcessPed` | sí | quitado |
| Tasks/TaskTypes/TaskSimpleClimb.cpp | `ProcessPed` | sí | quitado |
| Tasks/TaskTypes/TaskSimpleJetPack.cpp | `DoJetPackEffect` | sí | quitado |
| Tasks/TaskTypes/TaskSimpleSwim.cpp | `ProcessEffects` | sí | quitado |
| Tasks/TaskTypes/TaskSimpleUseGun.cpp | `SetMoveAnim` | sí | quitado |
| Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp/.h | Constructor | sí | quitado |
| ObjectData.cpp | `CObjectData::Initialise` (`sscanf_s`) | sí | dejado: solo comentario, mismo resultado con datos válidos |
| PPTriPlantBuffer.cpp | `CPPTriPlantBuffer::Flush` | sí | dejado: solo comentario (el límite es de `GetPlantModelsTab`, igual que el EXE) |
| Plant/ProcSurfaceInfo.cpp | `ProcSurfaceInfo_c::AddObjects` | sí | dejado: solo comentario |
| TagManager.cpp | `CTagManager::Load` | sí | dejado: `#if 0`, ya corre la ruta del EXE |
| Draw.cpp | `DoFade` | sin hook | dejado |
| Events/EventCarUpsideDown.cpp | Destructor | sin hook | dejado (el EXE vuelve a registrar la referencia) |
| ModelInfoAccelerator.cpp, Core/OctTree.cpp, SurfaceInfos_c.cpp (3), Entity/Vehicle/Automobile.cpp (`SetRandomDamage`), Entity/Vehicle/Boat.cpp, Fx/FxPrimBP.cpp, TheZones.cpp, GxtChar.cpp, StencilShadows.h, SurfaceInfo_c.h, Tasks/TaskCategories.cpp | varias | sin hook / NOTSA / comentario | dejado |
| Camera.cpp, Cheat.cpp (4), ControllerConfigManager.cpp (6), FileLoader.cpp, Font.cpp (2), GameLogic.cpp, GangWars.cpp, Garages.cpp, MenuManager.cpp (3), PathFind.cpp, Population.cpp (3), Radar.cpp (3), Replay.cpp, Shopping.cpp, Sprite2d.cpp, Streaming.cpp (`LoadCdDirectory` 2, `Shutdown`, comentario de `ConvertBufferToObject`), WaterLevel.cpp, Weapon.cpp (3) | varias | no | dejado (no validadas) |
| Audio/Entities/AEVehicleAudioEntity.cpp, Audio/Hardware/AEAudioHardware.cpp, Collision/Collision.cpp, Entity/Ped/Ped.cpp, Entity/Vehicle/Automobile.cpp (5), Entity/Vehicle/Bike.cpp, Entity/Vehicle/Vehicle.cpp, Events/EventHandler.cpp (4), Frontend/MenuManager_Input.cpp (2), Frontend/MenuManager_Process.cpp (7), Models/VehicleModelInfo.cpp, Pipelines/CustomCarEnvMap (2), Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp (`IsFixBugs` local), Scripts/TheScripts.cpp, TaskComplexKillCriminal.cpp, TaskComplexKillPedOnFootArmed.cpp (2), TaskComplexWalkRoundCar.cpp | varias | no | dejado (no validadas) |

Clases que otros agentes editaban (sin tocar): `CAEDataStream`, `CAEWeaponAudioEntity`, `CAnimBlendAssociation`, `CEventDamage`, `CTimeCycle` no tienen bloques `FIX_BUGS`; `IKChain_c` tiene dos `BUGFIX` incondicionales (Ragdoll/IKChain.cpp, `BONE_UNKNOWN`).

**Arreglos reversibles (no se quitan):** todos empiezan activados (`Enabled = true`, ninguno bloqueado) y solo se cambian en el menú de depuración (Settings → Bugs); no se guardan en el ini. `GenericCrashing`, `GenericUB`, `GenericOOB`, `GenericFrameRate`, `PS2CoronaRendering`, `AnimBlendSequence_SetName_SetBoneTagFlag`, `AESound_UpdatePlayTime_DivisionByZero`, `CCarCtrl_RemoveDistantCars_UseAfterFree`, `CAEVehicleAudioEntity_PlayBicycleSound_VolumeFix`, `CAEVehicleAudioEntity_PlayAircraftSound_VolumeFix`, `CPathFind_SwitchRoadsOffInArea_StrayAreas`, `CCustomCarPlateMgr_GeneratePlateText_MissingLettersAndDigits`, `CTaskComplexLeaveCarAndFlee_MissingNullCheckForVehicleOnFlee`, `CAECollisionAudioEntity_PlayLoopingCollisionSound_InvalidSurfaceType`, `CAEUserRadioTrackManager_UnicodeUserTracks`.

Repetir: `python reversing/FixBugs/build_probe.py` y `python reversing/run.py --case fixbugs`. Evidencia: [validation-summary.json](validation-summary.json).
