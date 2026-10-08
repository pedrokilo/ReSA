<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-10-08 23:28 UTC

## Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. Completar este catálogo es un hito intermedio hacia **ReSA — Zero Original Function Dependency**.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas | `████████████████████` | 771/771 · 100.00% |
| Clases validadas (de las completas) | `██████████████████░░` | 713/771 · 92.48% |
| Funciones reconstruidas | `████████████████████` | 8,446/8,446 · 100.00% |
| Funciones validadas (de las reconstruidas) | `████████████████░░░░` | 6,867/8,446 · 81.30% |

Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL.

Hay 8,446 entradas catalogadas, 421 bloqueadas y 8,025 sin bloqueo. No constituyen un inventario exhaustivo del EXE. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Los extras NOTSA sin equivalente en el EXE (3, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

### Siguientes mediciones

Estas dimensiones todavía no tienen un porcentaje útil: indican el trabajo que falta para poder medirlas.

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | 0/6,867 validadas confirmadas como vigentes | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | No medido | Auditar, para cada función reconstruida, las llamadas directas y transitivas que todavía saltan a código del EXE (`plugin::Call*`, direcciones fijas, hooks sin reconstruir) y contar las que ya no dependen de ninguna. |

**Validación consolidada parcialmente:** 6867 funciones con evidencia aprobada vinculada, 982 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias. Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

## Vista general

| Clases validadas | Completas por validar | Funciones validadas | Reconstruidas por validar | Funciones por reconstruir |
| :---: | :---: | :---: | :---: | :---: |
| **713 / 771** | **58 / 771** | **6,867 / 8,446** | **1,579 / 8,446** | **0 / 8,446** |
| 92.48% | 7.52% | 81.30% | 18.70% | 0.00% |

Reconstrucción por clases: 771 completas, 0 parciales y 0 sin revertir de 771.

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="completas"></a>
## 🟢 Completas · 771 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 señala bloqueos.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (7) ✅ 7 | [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) ✅ 9 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (17) ✅ 17 | [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (13) ✅ 13 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) ✅ 10 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (7) ✅ 7 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (6) ✅ 6 | [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (9) ✅ 9 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) ✅ 3 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (7) ✅ 7 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (12) ✅ 12 🔒 10 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CMaths](../source/game_sa/Maths.cpp) (3) ✅ 3 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) ✅ 7 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (8) ✅ 8 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (5) ✅ 5 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) ✅ 10 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (6) ✅ 6 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (5) ✅ 5 | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) ✅ 61 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (8) ✅ 8 🔒 6 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) ✅ 11 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) ✅ 7 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (7) ✅ 7 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) ✅ 12 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (2) ✅ 2 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (28) ✅ 28 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) ✅ 7 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (11) ✅ 6 🔒 6 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (8) ✅ 8 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (54) ✅ 53 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (4) ✅ 4 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (4) ✅ 4 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (6) ✅ 6 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) ✅ 8 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) ✅ 2 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (2) ✅ 2 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) ✅ 8 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (2) ✅ 2 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) ✅ 20 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (48) ✅ 48 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (5) ✅ 5 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (11) ✅ 11 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) ✅ 3 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (7) ✅ 7 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) ✅ 8 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (6) ✅ 3 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (6) ✅ 6 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) ✅ 6 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) ✅ 7 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (10) ✅ 10 | [Common](../source/game_sa/common.cpp) (9) ✅ 9 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) ✅ 5 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (13) ✅ 13 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) ✅ 7 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (6) ✅ 6 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) ✅ 8 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (20) ✅ 5 | [CPad](../source/game_sa/Pad.cpp) (75) ✅ 70 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) ✅ 7 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (11) ✅ 11 | [CPathFind](../source/game_sa/PathFind.cpp) (56) ✅ 2 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) ✅ 7 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) ✅ 58 | [CPathNode](../source/game_sa/PathFind.cpp) (1) ✅ 1 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) ✅ 9 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (145) ✅ 145 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (12) ✅ 12 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 4 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (20) ✅ 20 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (13) ✅ 13 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 14 🔒 14 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) ✅ 14 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) ✅ 8 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (19) ✅ 11 🔒 11 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (1) ✅ 1 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (10) ✅ 10 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (14) ✅ 14 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (18) ✅ 18 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (7) ✅ 7 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) ✅ 9 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) ✅ 14 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (6) ✅ 6 🔒 4 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (6) ✅ 6 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (8) ✅ 8 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 36 🔒 29 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) ✅ 6 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (24) ✅ 6 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (4) ✅ 4 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (2) ✅ 2 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (2) ✅ 2 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (4) ✅ 4 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) ✅ 14 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) ✅ 2 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (5) ✅ 5 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) ✅ 2 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (2) ✅ 2 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (9) ✅ 9 🔒 6 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (2) ✅ 2 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (5) ✅ 3 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) ✅ 7 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (2) ✅ 2 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (8) ✅ 6 🔒 6 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (2) ✅ 2 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (7) ✅ 5 🔒 5 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (106) ✅ 4 🔒 3 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (26) ✅ 26 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (8) ✅ 8 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (13) ✅ 13 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) ✅ 18 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (8) ✅ 8 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) ✅ 7 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) ✅ 6 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (9) ✅ 9 | [CPedGroups](../source/game_sa/PedGroups.cpp) (2) ✅ 2 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (7) ✅ 7 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (5) ✅ 5 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (52) ✅ 52 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) ✅ 7 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) ✅ 11 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (9) ✅ 9 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) ✅ 3 | [CPedList](../source/game_sa/PedList.cpp) (7) ✅ 7 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (8) ✅ 8 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (5) ✅ 5 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) ✅ 20 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (3) ✅ 3 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (18) ✅ 18 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedPool](../source/game_sa/Pools/PedPool.h) (2) ✅ 2 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (2) ✅ 2 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (5) ✅ 5 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) ✅ 14 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (4) ✅ 4 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (5) ✅ 5 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) ✅ 6 | [CPedStats](../source/game_sa/PedStats.cpp) (4) ✅ 4 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (1) ✅ 1 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (31) ✅ 31 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (3) ✅ 3 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) ✅ 9 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) | [CPedType](../source/game_sa/PedType.cpp) (13) ✅ 13 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) ✅ 8 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) ✅ 5 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (56) ✅ 56 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (10) ✅ 10 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (99) | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (10) ✅ 10 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) ✅ 34 | [CPickups](../source/game_sa/Pickups.cpp) (37) ✅ 2 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (10) ✅ 10 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (40) ✅ 18 🔒 18 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (16) ✅ 16 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (13) ✅ 13 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (5) ✅ 5 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (10) ✅ 10 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (23) ✅ 9 🔒 9 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (9) ✅ 9 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (23) ✅ 23 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (9) ✅ 9 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (10) ✅ 10 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) ✅ 6 |
| [CBridge](../source/game_sa/Bridge.cpp) (4) ✅ 4 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) ✅ 10 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (19) ✅ 19 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (10) ✅ 10 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) ✅ 2 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (9) ✅ 9 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (28) ✅ 28 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (5) ✅ 5 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (51) ✅ 50 | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (5) ✅ 5 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (5) ✅ 5 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (9) ✅ 9 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) ✅ 6 |
| [CCam](../source/game_sa/Cam.cpp) (44) ✅ 30 🔒 30 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (6) ✅ 6 |
| [CCamera](../source/game_sa/Camera.cpp) (110) ✅ 19 🔒 19 | [CPools](../source/game_sa/Pools.cpp) (19) ✅ 10 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (9) ✅ 9 |
| [CCarAI](../source/game_sa/CarAI.cpp) (25) ✅ 25 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (9) ✅ 9 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (22) ✅ 22 | [CPopulation](../source/game_sa/Population.cpp) (66) ✅ 2 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) ✅ 10 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 5 🔒 5 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 3 🔒 1 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) ✅ 7 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (6) ✅ 6 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) ✅ 8 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 🔒 11 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (3) ✅ 3 |
| [CCheat](../source/game_sa/Cheat.cpp) (72) | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (1) ✅ 1 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (7) ✅ 7 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (1) ✅ 1 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) ✅ 6 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (12) ✅ 12 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) ✅ 15 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (3) ✅ 3 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) ✅ 12 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) ✅ 4 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CRadar](../source/game_sa/Radar.cpp) (63) | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) ✅ 6 |
| [CClothes](../source/game_sa/Clothes.cpp) (11) ✅ 11 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (8) ✅ 8 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (6) ✅ 6 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (23) ✅ 8 🔒 8 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (7) ✅ 3 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) ✅ 7 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CRect](../source/game_sa/Core/Rect.cpp) (8) ✅ 8 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (5) ✅ 5 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) ✅ 24 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (6) ✅ 6 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (1) ✅ 1 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (1) ✅ 1 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) ✅ 1 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (59) ✅ 58 🔒 59 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (15) ✅ 15 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (8) ✅ 8 | [CReplay](../source/game_sa/Replay.cpp) (47) ✅ 47 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) ✅ 6 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (8) ✅ 8 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (9) ✅ 9 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 1 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (8) ✅ 8 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 2 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (5) ✅ 5 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) ✅ 6 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (39) ✅ 15 🔒 15 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (7) ✅ 4 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) ✅ 23 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (5) ✅ 5 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) ✅ 2 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (4) ✅ 4 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) ✅ 5 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) ✅ 6 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (6) ✅ 6 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (4) ✅ 4 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CSetPiece](../source/game_sa/SetPiece.cpp) (3) ✅ 3 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (8) ✅ 8 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (10) ✅ 10 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (19) ✅ 19 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) ✅ 6 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) ✅ 7 | [CShadows](../source/game_sa/Shadows.cpp) (31) ✅ 10 🔒 10 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) ✅ 4 |
| [CCoronas](../source/game_sa/Coronas.cpp) (11) ✅ 11 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) ✅ 3 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CShopping](../source/game_sa/Shopping.cpp) (30) ✅ 30 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) ✅ 3 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) ✅ 3 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) ✅ 3 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) ✅ 3 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) ✅ 3 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) ✅ 3 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) ✅ 3 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (33) ✅ 33 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) ✅ 3 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) ✅ 4 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (14) ✅ 14 | [CStats](../source/game_sa/Stats.cpp) (32) ✅ 32 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) ✅ 3 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) ✅ 3 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) ✅ 14 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (13) ✅ 13 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (6) ✅ 6 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (9) ✅ 9 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (14) ✅ 14 | [CStreaming](../source/game_sa/Streaming.cpp) (94) | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) ✅ 8 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (9) ✅ 9 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) ✅ 8 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (35) ✅ 35 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (8) ✅ 8 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) ✅ 6 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (6) ✅ 6 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) ✅ 7 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (4) ✅ 4 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) ✅ 4 | [CTagManager](../source/game_sa/TagManager.cpp) (19) ✅ 19 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) ✅ 14 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CTask](../source/game_sa/Tasks/Task.cpp) (4) ✅ 4 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) ✅ 8 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (3) ✅ 3 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) ✅ 7 |
| [CData](../source/game_sa/Text/Text.cpp) (2) ✅ 2 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) ✅ 5 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) ✅ 6 |
| [CDebug](../source/game_sa/Debug.cpp) (1) ✅ 1 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) ✅ 4 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (3) ✅ 3 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) ✅ 6 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) ✅ 3 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) ✅ 2 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (4) ✅ 4 |
| [CDirectory](../source/game_sa/Directory.cpp) (10) ✅ 10 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) ✅ 5 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) ✅ 4 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (3) ✅ 3 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (9) ✅ 9 |
| [CDraw](../source/game_sa/Draw.cpp) (2) ✅ 2 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) ✅ 4 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (9) ✅ 9 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (7) ✅ 7 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) ✅ 6 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (1) ✅ 1 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (5) ✅ 5 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (12) ✅ 2 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (2) ✅ 2 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) ✅ 15 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (2) ✅ 2 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (3) ✅ 3 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) ✅ 3 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) ✅ 70 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (9) ✅ 9 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) ✅ 5 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) ✅ 10 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (6) ✅ 6 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (11) ✅ 11 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (2) ✅ 2 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) ✅ 6 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (11) ✅ 11 🔒 1 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (18) ✅ 18 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (5) ✅ 5 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) ✅ 6 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) ✅ 11 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) ✅ 6 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (9) ✅ 9 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (5) ✅ 5 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (7) ✅ 7 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) ✅ 8 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (4) ✅ 4 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) ✅ 8 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (9) ✅ 9 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (7) ✅ 7 |
| [CEvent](../source/game_sa/Events/Event.cpp) (3) ✅ 3 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (9) ✅ 9 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (3) ✅ 3 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (4) ✅ 4 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (9) ✅ 9 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (8) ✅ 8 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) ✅ 1 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (10) ✅ 10 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (7) ✅ 7 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) ✅ 2 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (10) ✅ 10 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) ✅ 11 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (3) ✅ 3 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) ✅ 5 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (4) ✅ 4 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) ✅ 6 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (9) ✅ 9 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (4) ✅ 4 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) ✅ 7 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (3) ✅ 3 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (2) ✅ 2 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) ✅ 2 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (5) ✅ 5 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (2) ✅ 2 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) ✅ 6 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) ✅ 6 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (20) ✅ 20 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) ✅ 7 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) ✅ 1 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (4) ✅ 4 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (8) ✅ 8 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) ✅ 4 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (1) ✅ 1 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) ✅ 5 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) ✅ 19 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (11) ✅ 11 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (8) ✅ 8 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) ✅ 6 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (1) ✅ 1 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (14) ✅ 14 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) ✅ 3 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (3) ✅ 3 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) ✅ 4 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) ✅ 12 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (9) ✅ 9 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) ✅ 6 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (5) ✅ 5 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (3) ✅ 3 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) ✅ 6 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) ✅ 2 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (9) ✅ 9 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (1) ✅ 1 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (8) ✅ 8 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (5) ✅ 3 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) ✅ 10 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (8) ✅ 8 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (2) ✅ 2 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (9) ✅ 9 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (2) ✅ 2 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (8) ✅ 8 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (8) ✅ 8 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (8) ✅ 8 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (69) |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (9) ✅ 9 | [CTheZones](../source/game_sa/TheZones.cpp) (21) ✅ 21 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) ✅ 1 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) ✅ 9 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (6) ✅ 6 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) ✅ 8 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) ✅ 2 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) ✅ 2 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) ✅ 8 | [CTimer](../source/game_sa/Timer.cpp) (13) ✅ 13 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) ✅ 1 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) ✅ 11 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (2) ✅ 2 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) ✅ 8 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) ✅ 13 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) ✅ 1 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) ✅ 8 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (40) ✅ 20 🔒 20 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (2) ✅ 2 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (7) ✅ 7 🔒 6 | [cTransmission](../source/game_sa/cTransmission.cpp) (4) ✅ 4 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (3) ✅ 3 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) ✅ 19 | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) ✅ 26 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) ✅ 1 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (1) ✅ 1 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) ✅ 1 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) ✅ 13 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (2) ✅ 2 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (8) ✅ 8 🔒 7 | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (142) ✅ 142 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) ✅ 2 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (7) ✅ 7 🔒 5 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) ✅ 1 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (13) ✅ 13 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) ✅ 8 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (75) |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (2) ✅ 2 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (5) ✅ 5 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (1) ✅ 1 |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (2) ✅ 2 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (5) ✅ 5 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (3) ✅ 3 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) ✅ 6 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (2) ✅ 2 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) ✅ 11 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (8) ✅ 8 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (7) ✅ 7 | [CWanted](../source/game_sa/Wanted.cpp) (33) ✅ 33 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (8) ✅ 8 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) ✅ 7 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (2) ✅ 2 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (7) ✅ 7 | [CWaterLevel](../source/game_sa/WaterLevel.cpp) (30) ✅ 5 🔒 5 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) ✅ 3 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 6 🔒 6 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (4) ✅ 4 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (7) ✅ 7 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (9) ✅ 9 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (3) ✅ 3 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) ✅ 3 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (10) ✅ 10 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (5) ✅ 5 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) ✅ 8 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) ✅ 3 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) ✅ 2 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (10) ✅ 10 🔒 8 | [CWeather](../source/game_sa/Weather.cpp) (13) ✅ 13 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) ✅ 2 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) ✅ 7 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (3) ✅ 3 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) ✅ 4 | [CWorld](../source/game_sa/World.cpp) (95) ✅ 95 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (3) ✅ 3 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (9) ✅ 9 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) ✅ 6 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (3) ✅ 3 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) ✅ 7 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (2) ✅ 2 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) ✅ 3 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (6) ✅ 6 | [Fx_c](../source/game_sa/Fx/Fx.cpp) (8) ✅ 8 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (11) ✅ 3 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (9) ✅ 9 🔒 7 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (47) ✅ 47 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (7) ✅ 7 🔒 5 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) ✅ 1 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) ✅ 9 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (4) ✅ 4 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (1) ✅ 1 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (11) ✅ 3 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) ✅ 5 |
| [CFont](../source/game_sa/Font.cpp) (36) ✅ 4 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) ✅ 2 |
| [CFormation](../source/game_sa/Formation.cpp) (9) ✅ 9 🔒 8 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) ✅ 4 |
| [CGame](../source/game_sa/Game.cpp) (22) ✅ 22 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (7) ✅ 7 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (35) ✅ 4 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) ✅ 8 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (3) ✅ 3 | [Game](../source/app/app_game.cpp) (12) ✅ 12 🔒 3 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (8) ✅ 8 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CGangWars](../source/game_sa/GangWars.cpp) (35) ✅ 3 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) ✅ 8 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CGarage](../source/game_sa/Garage.cpp) (4) ✅ 4 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (10) ✅ 10 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (5) ✅ 5 |
| [CGarages](../source/game_sa/Garages.cpp) (21) ✅ 21 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) ✅ 9 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (52) ✅ 52 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) ✅ 8 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (24) ✅ 24 🔒 22 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (19) ✅ 14 🔒 14 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (8) ✅ 8 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) ✅ 21 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) ✅ 5 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (5) ✅ 5 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (9) ✅ 9 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (4) ✅ 4 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) ✅ 2 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 2 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (4) ✅ 4 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) ✅ 11 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (1) ✅ 1 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) ✅ 3 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (1) ✅ 1 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) ✅ 8 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (3) ✅ 3 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (1) ✅ 1 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) ✅ 4 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (11) ✅ 11 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) ✅ 7 | [Ps](../source/app/platform/win/WinPs.cpp) (22) ✅ 22 |
| [CHud](../source/game_sa/Hud.cpp) (38) ✅ 8 🔒 8 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (1) ✅ 1 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) ✅ 9 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (43) ✅ 43 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (14) ✅ 1 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (9) ✅ 7 🔒 6 | [Rs](../source/app/platform/platform.cpp) (27) ✅ 27 |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) ✅ 13 | [RtAnim](../source/game_sa/RenderWare/rw/rtanim.cpp) (1) ✅ 1 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (11) ✅ 7 | [RwCore](../source/game_sa/RenderWare/rw/rwcore.cpp) (4) ✅ 4 |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (8) ✅ 8 | [RwHelper](../source/game_sa/RwHelper.cpp) (24) ✅ 24 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (9) ✅ 9 | [Securom](../source/app/securom/Securom.cpp) (2) ✅ 2 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 🔒 5 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (9) ✅ 9 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) ✅ 47 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (1) ✅ 1 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) ✅ 10 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (9) ✅ 9 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) ✅ 6 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (4) ✅ 4 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) ✅ 7 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) ✅ 3 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) ✅ 5 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (6) ✅ 5 |

</details>

<a id="parciales"></a>
## 🟡 Parciales · 0 clases

Orden alfabético para localizar clases. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

| Clase | Revertidas / total | Pendientes | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: |

<a id="sin-revertir"></a>
## 🔴 Sin revertir · 0 clases

| Clase | Revertidas / total | Pendientes | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: |

<a id="metodo"></a>
## Cómo leer y actualizar

- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.
- **Validated:** `1` indica evidencia aprobada vinculada para esa entrada. No implica vigencia actual ni independencia del EXE.
- **Locked:** indicador de bloqueo del hook; hay 421 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.
- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.

- **Acceso al código:** pulsa el nombre para abrir el archivo que registra sus hooks. Si el registro está repartido, los enlaces adicionales abren los demás archivos. Algunas clases registran sus hooks en una cabecera.
- **Comentarios de estado:** resumen por clase/grupo, con nombres de pendientes y bloqueadas. Los conteos abarcan la clase completa aunque su implementación esté repartida entre archivos.

Sincronizar primero `validated` desde el registro de evidencia:

```powershell
python contrib/sync_validated_flags.py
```

Actualizar los flags de reconstrucción solo después de comparar con el original dentro del motor y regenerar desde la raíz:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

Para sincronizar también los comentarios de los archivos fuente:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md --update-source-status
python contrib/validation_inventory.py
```

Los comentarios se generan desde el CSV y la evidencia vinculada; no modifican flags ni acreditan validación. La vigencia se consulta en el [inventario](ValidationInventory.md). Añadir comentarios cambia las huellas de los archivos: el auditor conserva la evidencia histórica y exige revisar su vigencia, sin declarar fallidas las pruebas ni actualizar sus hashes de referencia.

El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.
