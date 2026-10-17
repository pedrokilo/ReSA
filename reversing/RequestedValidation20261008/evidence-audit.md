<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría de evidencia reutilizable fuera de Pending355

Corte: 2026-10-08. Auditoría de solo lectura; no se compiló, ejecutó GTA ni modificó catálogo o fuentes. La coincidencia de hashes es observación de archivos y no una validación vigente de ejecución.

## Bancos por evidencia registrada

| Banco / resumen | Identidades del alcance | Pares históricos | Fuentes coincidentes | Revisión de dependencias | Restauración literal en instrumento |
|---|---:|---:|---:|---:|---|
| `reversing/ArmedNextPending/validation-summary.json` | 1 | 1024 | 1/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/ArmedPending/validation-summary.json` | 1 | 1024 | 1/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/ArrestPending/validation-summary.json` | 3 | 2560 | 3/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/AttractorTaskPending/validation-summary.json` | 3 | 1024 | 3/3 | 0/3 | sí: requiere corregir comprobación |
| `reversing/AudioFrontendPending/validation-summary.json` | 2 | 3072 | 2/2 | 0/2 | no literal detectado; revisar cobertura |
| `reversing/AudioHw54/validation-summary.json` | 53 | 9280 | 0/53 | 0/53 | sí: requiere corregir comprobación |
| `reversing/AudioPedPending/validation-summary.json` | 4 | 4608 | 4/4 | 0/4 | no literal detectado; revisar cobertura |
| `reversing/AudioScannerPending/validation-summary.json` | 4 | 4096 | 4/4 | 0/4 | no literal detectado; revisar cobertura |
| `reversing/AudioStreamingPending/validation-summary.json` | 3 | 2560 | 3/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/AudioVehiclePending/validation-summary.json` | 4 | 4096 | 4/4 | 0/4 | no literal detectado; revisar cobertura |
| `reversing/ColGeom59a/validation-summary.json` | 25 | 19712 | 0/25 | 0/25 | no literal detectado; revisar cobertura |
| `reversing/ColProc59b/validation-summary.json` | 13 | 8192 | 0/13 | 0/13 | no literal detectado; revisar cobertura |
| `reversing/ColWorld59c/validation-summary.json` | 20 | 13568 | 0/20 | 0/20 | no literal detectado; revisar cobertura |
| `reversing/CriminalPending/validation-summary.json` | 2 | 2048 | 2/2 | 0/2 | no literal detectado; revisar cobertura |
| `reversing/DecisionPending/validation-summary.json` | 2 | 3072 | 2/2 | 0/2 | sí: requiere corregir comprobación |
| `reversing/DriveByPending/validation-summary.json` | 3 | 2560 | 3/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/FightingControlPending/validation-summary.json` | 1 | 1024 | 1/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/FixAudio/validation-summary.json` | 3 | 5120 | 0/3 | 0/3 | sí: requiere corregir comprobación |
| `reversing/FixHooks/validation-summary.json` | 1 | 6144 | 0/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/FontPending/validation-summary.json` | 4 | 6144 | 4/4 | 0/4 | no literal detectado; revisar cobertura |
| `reversing/FxSystemPending/validation-summary.json` | 4 | 5120 | 4/4 | 0/4 | sí: requiere corregir comprobación |
| `reversing/GangHasslePending/validation-summary.json` | 3 | 2048 | 0/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/GangWarsPending/validation-summary.json` | 3 | 3072 | 3/3 | 0/3 | sí: requiere corregir comprobación |
| `reversing/GroupEventPending/validation-summary.json` | 2 | 2048 | 2/2 | 0/2 | sí: requiere corregir comprobación |
| `reversing/GroupPlacerPending/validation-summary.json` | 3 | 3072 | 3/3 | 0/3 | sí: requiere corregir comprobación |
| `reversing/IdleAttractorPending/validation-summary.json` | 1 | 2048 | 1/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/LineUpPending/validation-summary.json` | 2 | 2048 | 0/2 | 0/2 | no literal detectado; revisar cobertura |
| `reversing/LineUpProcessPending/validation-summary.json` | 1 | 1024 | 0/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/Misc6/validation-summary.json` | 5 | 19456 | 0/5 | 0/5 | no literal detectado; revisar cobertura |
| `reversing/PathFindPending/validation-summary.json` | 2 | 2048 | 2/2 | 0/2 | sí: requiere corregir comprobación |
| `reversing/PedDamagePending/validation-summary.json` | 4 | 6144 | 0/4 | 0/4 | sí: requiere corregir comprobación |
| `reversing/PickupsPending/validation-summary.json` | 2 | 2048 | 2/2 | 0/2 | sí: requiere corregir comprobación |
| `reversing/PlayerFighterPending/validation-summary.json` | 1 | 4096 | 1/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/PlayerPed51/validation-summary.json` | 50 | 11648 | 0/50 | 0/50 | no literal detectado; revisar cobertura |
| `reversing/PoolsPending/validation-summary.json` | 3 | 3072 | 3/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/PopulationPending/validation-summary.json` | 2 | 2048 | 2/2 | 0/2 | sí: requiere corregir comprobación |
| `reversing/PostEffectsPending/validation-summary.json` | 3 | 2304 | 3/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/ProstituteControlPending/validation-summary.json` | 1 | 1024 | 1/1 | 0/1 | no literal detectado; revisar cobertura |
| `reversing/ProstitutePending/validation-summary.json` | 3 | 2048 | 3/3 | 0/3 | no literal detectado; revisar cobertura |
| `reversing/RendererPending/validation-summary.json` | 4 | 6144 | 4/4 | 0/4 | no literal detectado; revisar cobertura |
| `reversing/RoadBlockPending/validation-summary.json` | 1 | 1024 | 1/1 | 0/1 | sí: requiere corregir comprobación |
| `reversing/RopePending/validation-summary.json` | 2 | 2048 | 2/2 | 0/2 | no literal detectado; revisar cobertura |
| `reversing/ShadowManagerPending/validation-summary.json` | 2 | 2560 | 0/2 | 0/2 | no literal detectado; revisar cobertura |

Los pares pertenecen al resumen completo y pueden incluir identidades fuera del alcance; no deben sumarse como cobertura nueva. La búsqueda de literales no demuestra por sí sola que se restauren todos los hooks, parches y globales.

## Fuentes discrepantes observadas

- `CAEAudioHardware::Constructor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::Destructor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::AllocateChannels`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::RequestVirtualChannelSoundInfo`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::Query3DSoundEffects`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetNumAvailableChannels`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetChannelPlayTimes`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetChannelVolume`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::LoadSoundBank`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::IsSoundBankLoaded`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetSoundBankLoadingStatus`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::LoadSound`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::IsSoundLoaded`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetSoundLoadingStatus`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::StopSound`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetChannelPosition`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetChannelFrequencyScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::RescaleChannelVolumes`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::UpdateReverbEnvironment`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetSoundHeadroom`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::EnableEffectsLoading`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::DisableEffectsLoading`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetVirtualChannelSoundLengths`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetVirtualChannelSoundLoopStartTimes`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::PlayTrack`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::StartTrackPlayback`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::StopTrack`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetTrackPlayTime`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetTrackLengthMs`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetActiveTrackID`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetPlayingTrackID`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetBeatInfo`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetBassSetting`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::DisableBassEq`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::EnableBassEq`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetChannelFlags`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetMusicMasterScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetEffectsMasterScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetMusicFaderScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetEffectsFaderScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetEffectsMasterScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetMusicMasterScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetEffectsFaderScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetNonStreamFaderScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::SetStreamFaderScalingFactor`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::IsStreamingFromDVD`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::GetDVDDriveLetter`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::CheckDVD`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::PauseAllSounds`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::ResumeAllSounds`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::InitDirectSoundListener`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::Terminate`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CAEAudioHardware::Service`: `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`
- `CCollision::TestSphereSphere`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CalculateColPointInsideBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestSphereBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::PointInTriangle`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::DistToLineSqr`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::DistToMathematicalLine`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::DistToMathematicalLine2D`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::DistAlongLine2D`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestLineBox_DW`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestLineBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestVerticalLineBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::Test2DLineAgainst2DLine`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestLineTriangle`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::GetBoundingBoxFromTwoSpheres`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::RayPolyPOP`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::GetPrincipleAxis`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::PointInPoly`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::Closest3`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ClosestSquaredDistanceBetweenFiniteLines`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestSphereTriangle`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestLineSphere`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::DistToLine`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ClosestPointOnLine`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ClosestPointsOnPoly`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ClosestPointOnPoly`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessSphereBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessLineSphere`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessLineBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessDiscCollision`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessLineTriangle`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessVerticalLineTriangle`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::IsStoredPolyStillValidVerticalLine`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CalculateTrianglePlanes-colData`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::RemoveTrianglePlanes-colData`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessSphereSphere`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessSphereTriangle`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessVerticalLine`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessColModels`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::Init`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::Shutdown`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::Update`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::SortOutCollisionAfterLoad`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::IsThisVehicleSittingOnMe`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ResetMadeInvisibleObjects`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CheckCameraCollisionPeds`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CheckPeds`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::SphereCastVsBBox`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::SphereCastVersusVsPoly`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::SphereCastVsSphere`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::SphereCastVsEntity`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::SphereCastVsCaches`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CheckCameraCollisionBuildings`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CheckCameraCollisionVehicles`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CheckCameraCollisionObjects`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::BuildCacheOfCameraCollision`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::CameraConeCastVsWorldCollision`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::TestLineOfSight`: `source/game_sa/Collision/Collision.cpp`
- `CCollision::ProcessLineOfSight`: `source/game_sa/Collision/Collision.cpp`
- `CAEStreamingChannel::GetPlayTime`: `source/game_sa/Audio/AESound.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`, `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/AEStreamThread.cpp`, `source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp`, `source/game_sa/Audio/Entities/AEPedAudioEntity.cpp`
- `CAEStreamingChannel::GetLength`: `source/game_sa/Audio/AESound.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`, `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/AEStreamThread.cpp`, `source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp`, `source/game_sa/Audio/Entities/AEPedAudioEntity.cpp`
- `CAEPedAudioEntity::UpdateParameters`: `source/game_sa/Audio/AESound.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`, `source/game_sa/Audio/Hardware/AEAudioHardware.cpp`, `source/game_sa/Audio/AEStreamThread.cpp`, `source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp`, `source/game_sa/Audio/Entities/AEPedAudioEntity.cpp`
- `CAEStreamingChannel::SynchPlayback`: `source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp`, `source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp`, `source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp`, `source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp`, `source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp`, `source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.h`, `source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp`, `source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h`, `source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp`, `source/game_sa/Tasks/TaskTypes/TaskSimpleDead.h`, `source/game_sa/Audio/Hardware/AEStreamingChannel.cpp`, `source/game_sa/Audio/Hardware/AEStreamingChannel.h`, `source/game_sa/Entity/Entity.cpp`, `source/game_sa/Entity/Vehicle/Bike.cpp`
- `CTaskGangHasslePed::CreateFirstSubTask`: `source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp`, `source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h`
- `CTaskGangHasslePed::CreateNextSubTask`: `source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp`, `source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h`
- `CTaskGangHasslePed::ControlSubTask`: `source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp`, `source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h`
- `CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor`: `source/game_sa/Entity/Vehicle/Bike.cpp`
- `CTaskUtilityLineUpPedWithCar::GetPositionToOpenCarDoor`: `source/game_sa/Entity/Vehicle/Bike.cpp`
- `CTaskUtilityLineUpPedWithCar::ProcessPed`: `source/game_sa/Entity/Vehicle/Bike.cpp`
- `Win::GTATranslateKey`: `source/game_sa/Birds.cpp`, `source/game_sa/Credits.cpp`, `source/game_sa/Curves.cpp`, `source/game_sa/OnscreenTimer.cpp`, `source/game_sa/Models/TimeInfo.cpp`, `source/game_sa/Interior/FurnitureGroup_c.cpp`, `source/app/platform/win/WinPlatform.cpp`, `source/app/platform/win/WinMain.cpp`
- `Win::GTATranslateShiftKey`: `source/game_sa/Birds.cpp`, `source/game_sa/Credits.cpp`, `source/game_sa/Curves.cpp`, `source/game_sa/OnscreenTimer.cpp`, `source/game_sa/Models/TimeInfo.cpp`, `source/game_sa/Interior/FurnitureGroup_c.cpp`, `source/app/platform/win/WinPlatform.cpp`, `source/app/platform/win/WinMain.cpp`
- `Win::IsAlreadyRunning`: `source/game_sa/Birds.cpp`, `source/game_sa/Credits.cpp`, `source/game_sa/Curves.cpp`, `source/game_sa/OnscreenTimer.cpp`, `source/game_sa/Models/TimeInfo.cpp`, `source/game_sa/Interior/FurnitureGroup_c.cpp`, `source/app/platform/win/WinPlatform.cpp`, `source/app/platform/win/WinMain.cpp`
- `Win::IsForegroundApp`: `source/game_sa/Birds.cpp`, `source/game_sa/Credits.cpp`, `source/game_sa/Curves.cpp`, `source/game_sa/OnscreenTimer.cpp`, `source/game_sa/Models/TimeInfo.cpp`, `source/game_sa/Interior/FurnitureGroup_c.cpp`, `source/app/platform/win/WinPlatform.cpp`, `source/app/platform/win/WinMain.cpp`
- `Win::MessageLoop`: `source/game_sa/Birds.cpp`, `source/game_sa/Credits.cpp`, `source/game_sa/Curves.cpp`, `source/game_sa/OnscreenTimer.cpp`, `source/game_sa/Models/TimeInfo.cpp`, `source/game_sa/Interior/FurnitureGroup_c.cpp`, `source/app/platform/win/WinPlatform.cpp`, `source/app/platform/win/WinMain.cpp`
- `CPedDamageResponseCalculator::AccountForPedDamageStats`: `source/game_sa/PedDamageResponseCalculator.cpp`
- `CPedDamageResponseCalculator::AccountForPedArmour`: `source/game_sa/PedDamageResponseCalculator.cpp`
- `CPedDamageResponseCalculator::ComputeWillForceDeath`: `source/game_sa/PedDamageResponseCalculator.cpp`
- `CPedDamageResponseCalculator::ComputeDamageResponse`: `source/game_sa/PedDamageResponseCalculator.cpp`
- `CPlayerPed::AnnoyPlayerPed`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::Busted`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::CanIKReachThisTarget`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::CanPlayerStartMission`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::CheatWantedLevel`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::Clear3rdPersonMouseTarget`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ClearAdrenaline`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ClearWeaponTarget`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::DeactivatePlayerPed`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::DisbandPlayerGroup`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::DoStuffToGoOnFire`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::DoesPlayerWantNewWeapon`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::DoesTargetHaveToBeBroken`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::EvaluateTarget`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::FindPedToAttack`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::FindTargetPriority`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ForceGroupToAlwaysFollow`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ForceGroupToNeverFollow`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::GetButtonSprintResults`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::GetPadFromPlayer`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::GetPlayerInfoForThisPlayerPed`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::GetWantedLevel`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::GetWeaponRadiusOnScreen`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::HandlePlayerBreath`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::HandleSprintEnergy`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::IsHidden`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::LOSBlockedBetweenPeds`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::Load`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::MakeChangesForNewWeapon-`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::MakeChangesForNewWeapon-BySlot`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::MakeGroupRespondToPlayerTakingDamage`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::MakePlayerGroupDisappear`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::MakePlayerGroupReappear`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::PedCanBeTargettedVehicleWise`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::PickWeaponAllowedFor2Player`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::PlayerHasJustAttackedSomeone`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ProcessControl`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ProcessPlayerWeapon`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ReApplyMoveAnims`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ReactivatePlayerPed`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::RemovePlayerPed`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ResetPlayerBreath`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::ResetSprintEnergy`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::Save`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::SetMoveAnim`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::SetPlayerMoveBlendRatio`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::SetWantedLevel`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::SetWantedLevelNoDrop`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::TellGroupToStartFollowingPlayer`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CPlayerPed::UpdateCameraWeaponModes`: `source/game_sa/Entity/Ped/PlayerPed.cpp`
- `CRealTimeShadowManager::ReInit`: `source/game_sa/RealTimeShadow.h`
- `CRealTimeShadowManager::GetRealTimeShadow`: `source/game_sa/RealTimeShadow.h`

## Banco preparado a reutilizar y límites

- `PendingBatch/summary.json`: 30 bancos, 78 identidades, 81.664 pares históricos. `scope.json` fija identidades únicas; construir cada banco con `build_probe.py`, comparar su nuevo manifiesto y ejecutar `run_local.py` en serie. No repetir binarios archivados como si fueran fuentes actuales.
- `IdleAttractorPending`, `FightingControlPending`, `PlayerFighterPending`, `RoadBlockPending` aportan las cinco validaciones anteriores; el primero incluye CPedAttractor fuera del alcance y debe mantenerse explícito. Regresión previa de cinco bancos: 9.984 pares, incluyendo CTaskDead fuera del alcance.
- `probe_build.py` archiva ASI/PDB, hash de Game.cpp original, fuentes explícitas e instrumentos locales. No registra todo el árbol/transitivas ni fija /MP6. El coordinador debe imponer `_CL_=/MP6`, congelar dependencias/configuración y verificar hash del EXE y ASI efectivamente ejecutados.
- AudioVehiclePending compara bytes/argumentos y trazas con slots, bancos, generación de sonidos y otros colaboradores aislados: no reproduce audio real. ShadowManagerPending sustituye cámaras/rasters y conserva SetupForThisEntity como llamada nativa no validada; no acredita dibujo de sombras.
- LineUpProcessPending documenta diferencias históricas corregidas en orientación x87. No atribuir esa corrección a una ejecución actual. Bancos de tareas aíslan helpers/peds/tareas; validan controlador dentro del fixture, no integración real de IA/animaciones/pools.

## Bloqueos concretos y trabajo que permanece

- Los registros sin revisión de dependencias no acreditan vigencia aunque todos sus archivos declarados coincidan. Una reconstrucción completa del catálogo no acredita todas las funciones preexistentes de las clases.
- Las discrepancias listadas requieren renovar bancos afectados; no sustituir hashes antiguos por hashes presentes para conceder aprobación. Fuentes completas se cotejan conservadoramente, aun cuando el cambio sea documental.
- `docs/EstadoListasSolicitadas.md` informa 14 identidades caducadas del lote previo: Dead 6 fuera de alcance, ShadowManager 2, GangHasslePed 3, LineUp 3. Interior_c posterior también obliga a revisión de dependencias.
- GangFollower::Destructor presenta fallo histórico en vptr task[0], 256/256 casos. No pertenece al pedido; existe dependencia fuente indirecta en TaskComplexAvoidOtherPedWhileWandering.cpp y referencias de tabla en bancos Wander355/VehicleLean355. Una referencia de tabla no prueba ejecución del destructor. Investigar únicamente si una rama cubierta lo ejecuta o si modifica un colaborador relevante del alcance.
- Conservar los fallos de arranque como intentos sin comparaciones; no son diferencias funcionales ni aprobaciones. Verificar disponibilidad/hash de logs y ASI archivados antes de confiar en resumen.

## Fuentes de esta auditoría

`ValidationRegistry/README.md`, `PendingBatch/README.md`, `PendingBatch/scope.json`, `PendingBatch/summary.json`, `docs/EstadoListasSolicitadas.md`, `validation-registry.json`, `probe_build.py`, README de AudioVehiclePending, ShadowManagerPending, GangHasslePending y LineUpProcessPending. Skill REA leída: `C:/Users/pedro/.agents/skills/reverse-engineer-anything/SKILL.md`; no fue necesario abrir sesión binaria para esta auditoría de evidencia archivada.
