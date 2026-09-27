# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Sincronizado:** 2026-09-27 UTC

## Objetivo y dimensiones de progreso

La [hoja de ruta de PGTASA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. Completar este catálogo es un hito intermedio hacia **PGTASA — Zero Original Function Dependency**.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas | `█████████████████░░░` | 621/712 · 87.22% |
| Funciones reconstruidas | `███████████████████░` | 7,545/8,080 · 93.38% |
| Funciones validadas, vigencia confirmada | `░░░░░░░░░░░░░░░░░░░░` | 0/8,080 · 0.00%; inventario parcial |
| Dependencias de funciones originales eliminadas | Sin barra | No medido; requiere auditoría directa y transitiva |

Hay 8,080 entradas catalogadas, 625 bloqueadas y 7,455 sin bloqueo. No constituyen un inventario exhaustivo del EXE. Desbloqueada no significa validada, activa ni independiente; el número de funciones que usan código original sigue sin medir. No se deducen hooks eliminados del contador de locks.

**Validación consolidada parcialmente:** 34 funciones con evidencia aprobada vinculada, 0 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias. Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md). Esta barra mide acreditación en el registro, no ausencia de pruebas previas ni cobertura exhaustiva.

## Vista general

| Clases completas | Clases parciales | Sin revertir | Funciones revertidas | Funciones pendientes |
| :---: | :---: | :---: | :---: | :---: |
| **621 / 712** | **88 / 712** | **3 / 712** | **7,545 / 8,080** | **535** |
| 87.22% | 12.36% | 0.42% | 93.38% | 6.62% |

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Estos porcentajes miden entradas registradas como `reversed`, no cobertura de pruebas ni estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.
>
> La integridad del catálogo activo se comprueba con `python contrib/audit_hook_catalog.py --strict`; el corte actual no presenta inconsistencias críticas.

<a id="completas"></a>
## 🟢 Completas · 621 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; 🔒 señala que al menos una sigue bloqueada.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (7) | [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (17) | [CGridRef](../source/game_sa/GridRef.cpp) (5) | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (13) |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) | [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) | [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (9) |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (6) | [CHandShaker](../source/game_sa/HandShaker.cpp) (3) | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (7) |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) | [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (11) | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (5) | [CHudColours](../source/game_sa/HudColours.cpp) (5) | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (8) |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) | [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (6) |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) | [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) | [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (5) | [CIniFile](../source/game_sa/IniFile.cpp) (1) | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) | [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (1) | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) | [CIplStore](../source/game_sa/IplStore.cpp) (34) | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) | [CKeyArray](../source/game_sa/Text/Text.cpp) (4) | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (4) |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) | [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (4) |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (54) | [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) | [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (2) |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (6) | [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (2) |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (17) | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (5) |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (7) |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) | [CLocalisation](../source/game_sa/Localisation.cpp) (18) | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (10) | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (13) | [CMessages](../source/game_sa/Messages.cpp) (28) | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) | [CMirrors](../source/game_sa/Mirrors.cpp) (8) | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (11) | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (12) |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (28) | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (10) | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (1) |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (18) | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (14) |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (2) | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (8) |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) | [CObject](../source/game_sa/Entity/Object/Object.cpp) (48) | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (8) |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) | [CObjectData](../source/game_sa/ObjectData.cpp) (2) | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (7) |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (13) | [COccluder](../source/game_sa/Occluder.cpp) (3) | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) | [COcclusion](../source/game_sa/Occlusion.cpp) (5) | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (18) |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (9) | [Common](../source/game_sa/common.cpp) (9) | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (20) | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (5) |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (6) | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (5) |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) | [CPad](../source/game_sa/Pad.cpp) (75) | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (1) |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) | [CPathNode](../source/game_sa/PathFind.cpp) (1) | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (3) |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (145) | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (10) |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (10) |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (7) | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (10) |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (2) | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (9) |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (31) | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (9) |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (2) | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (10) |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (2) | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (26) | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (99) | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (10) |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) | [CPedGroups](../source/game_sa/PedGroups.cpp) (2) | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (9) |
| [CBirds](../source/game_sa/Birds.cpp) (6) | [CPedIK](../source/game_sa/PedIK.cpp) (9) | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (23) | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (52) | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (6) |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (9) |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) | [CPedList](../source/game_sa/PedList.cpp) (7) | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (9) |
| [CBridge](../source/game_sa/Bridge.cpp) (4) | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (3) | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) | [CPedPool](../source/game_sa/Pools/PedPool.h) (2) | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (6) |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) | [CPedStats](../source/game_sa/PedStats.cpp) (4) | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (9) | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) |
| [CCarAI](../source/game_sa/CarAI.cpp) (25) | [CPedType](../source/game_sa/PedType.cpp) (13) | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (22) | [CPhysical](../source/game_sa/Entity/Physical.cpp) (56) | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (1) |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) | [CPickup](../source/game_sa/Pickup.cpp) (12) | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) |
| [CCheat](../source/game_sa/Cheat.cpp) (72) | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (16) | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (15) |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (5) |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (3) | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) |
| [CClock](../source/game_sa/Clock.cpp) (9) | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) |
| [CClothes](../source/game_sa/Clothes.cpp) (11) | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) |
| [CClouds](../source/game_sa/Clouds.cpp) (21) | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (19) | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (4) |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (28) | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (1) | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (51) | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (8) |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (59) 🔒 59 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (8) | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (9) | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (1) | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (1) | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) | [CRadar](../source/game_sa/Radar.cpp) (63) | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (8) | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) | [CRect](../source/game_sa/Core/Rect.cpp) (8) | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) | [CReferences](../source/game_sa/References.cpp) (4) | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (11) | [CRenderer](../source/game_sa/Renderer.cpp) (33) | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) | [CReplay](../source/game_sa/Replay.cpp) (47) | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (6) |
| [CCoronas](../source/game_sa/Coronas.cpp) (11) | [CRestart](../source/game_sa/Restart.cpp) (11) | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (9) |
| [CCover](../source/game_sa/Cover.cpp) (16) | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (5) | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) |
| [CCranes](../source/game_sa/Cranes.cpp) (5) | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) |
| [CCrime](../source/game_sa/Crime.cpp) (2) | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (4) |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) | [CSetPiece](../source/game_sa/SetPiece.cpp) (3) | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) |
| [CCurves](../source/game_sa/Curves.cpp) (6) | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (19) | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (14) | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) | [CShopping](../source/game_sa/Shopping.cpp) (30) | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (14) | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (9) | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (9) |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (35) | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (6) | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) | [CSprite](../source/game_sa/Sprite.cpp) (15) | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (2) |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) | [CSprite2d](../source/game_sa/Sprite2d.cpp) (33) | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) |
| [CData](../source/game_sa/Text/Text.cpp) (2) | [CStats](../source/game_sa/Stats.cpp) (32) | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) |
| [CDebug](../source/game_sa/Debug.cpp) (1) | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (6) |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) |
| [CDirectory](../source/game_sa/Directory.cpp) (10) | [CStreaming](../source/game_sa/Streaming.cpp) (94) | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) |
| [CDoor](../source/game_sa/Door.cpp) (6) | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) |
| [CDraw](../source/game_sa/Draw.cpp) (2) | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (8) | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (9) | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (1) | [CTagManager](../source/game_sa/TagManager.cpp) (19) | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (2) | [CTask](../source/game_sa/Tasks/Task.cpp) (4) | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (7) |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (3) | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (3) |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (8) |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (7) |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (9) |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (3) | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (3) |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (18) | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (5) | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (11) | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) |
| [CEvent](../source/game_sa/Events/Event.cpp) (3) | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (4) | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (9) | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (10) | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (10) | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (3) | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) | [CText](../source/game_sa/Text/Text.cpp) (9) |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (4) | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (4) | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (69) |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (2) | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) | [CTheZones](../source/game_sa/TheZones.cpp) (21) |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (2) | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (20) | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (4) | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (1) | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (8) | [CTimer](../source/game_sa/Timer.cpp) (13) |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (11) | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (14) | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (1) | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (9) | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (9) | [cTransmission](../source/game_sa/cTransmission.cpp) (4) |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (9) | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (5) | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (8) | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (9) | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (142) |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (2) | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (6) | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (2) | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (8) | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (75) |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (1) |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) | [CWanted](../source/game_sa/Wanted.cpp) (33) |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (1) | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (2) | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (13) 🔒 10 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (9) |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (10) |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (2) | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (5) | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (3) | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) | [CWorld](../source/game_sa/World.cpp) (94) |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (2) | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) | [Fx_c](../source/game_sa/Fx/Fx.cpp) (8) |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (2) | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (4) |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (2) | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (9) | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (3) | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (2) | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (8) | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (6) | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (1) | [Game](../source/app/app_game.cpp) (12) 🔒 3 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (2) | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (4) | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (7) | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (5) |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (3) | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (5) | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (3) | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (5) |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (8) | [Light](../source/app/app_light.cpp) (27) |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (3) | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (10) | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (3) | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (3) | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (2) | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (9) | [Ps](../source/app/platform/win/WinPs.cpp) (22) |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (1) | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (47) | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (1) | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (43) |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) 🔒 13 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) | [Rs](../source/app/platform/platform.cpp) (27) |
| [CFire](../source/game_sa/Fire.cpp) (5) | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (1) | [RtAnim](../source/game_sa/RenderWare/rw/rtanim.cpp) (1) |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) | [RwCore](../source/game_sa/RenderWare/rw/rwcore.cpp) (4) |
| [CGame](../source/game_sa/Game.cpp) (22) | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) | [RwHelper](../source/game_sa/RwHelper.cpp) (24) |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (8) | [Securom](../source/app/securom/Securom.cpp) (2) |
| [CGamma](../source/game_sa/Gamma.cpp) (2) | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (9) | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) |
| [CGangs](../source/game_sa/Gangs.cpp) (8) | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) |
| [CGarage](../source/game_sa/Garage.cpp) (4) | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (9) | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) |
| [CGarages](../source/game_sa/Garages.cpp) (21) | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) |
| [CGeneral](../source/game_sa/General.cpp) (9) | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) |
| [CGlass](../source/game_sa/Glass.cpp) (19) | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (6) |
</details>

<a id="parciales"></a>
## 🟡 Parciales · 88 clases

Orden alfabético para localizar clases. Para decidir qué abordar primero, consultar la [estimación por dificultad](ReversingPriorities.md).

| Clase | Revertidas / total | Pendientes | Avance | Locked |
| --- | ---: | ---: | ---: | ---: |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) | 6/7 | **1** | 85.71% | 1 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) | 4/6 | **2** | 66.67% | 2 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) | 16/20 | **4** | 80.00% | 4 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) | 13/17 | **4** | 76.47% | 4 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) | 29/43 | **14** | 67.44% | 14 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) | 8/19 | **11** | 42.11% | 11 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) | 7/8 | **1** | 87.50% | 1 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) | 21/24 | **3** | 87.50% | 3 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) | 102/106 | **4** | 96.23% | 7 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) | 4/5 | **1** | 80.00% | 1 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) | 22/40 | **18** | 55.00% | 18 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) | 7/10 | **3** | 70.00% | 3 |
| [CCam](../source/game_sa/Cam.cpp) | 14/44 | **30** | 31.82% | 30 |
| [CCamera](../source/game_sa/Camera.cpp) | 91/110 | **19** | 82.73% | 19 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) | 27/32 | **5** | 84.38% | 5 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) | 9/10 | **1** | 90.00% | 1 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) | 15/23 | **8** | 65.22% | 8 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) | 3/7 | **4** | 42.86% | 4 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) | 10/11 | **1** | 90.91% | 2 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) | 69/72 | **3** | 95.83% | 3 |
| [CFont](../source/game_sa/Font.cpp) | 32/36 | **4** | 88.89% | 4 |
| [CFormation](../source/game_sa/Formation.cpp) | 1/9 | **8** | 11.11% | 8 |
| [CGangWars](../source/game_sa/GangWars.cpp) | 32/35 | **3** | 91.43% | 3 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) | 5/19 | **14** | 26.32% | 14 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) | 30/32 | **2** | 93.75% | 2 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) | 2/7 | **5** | 28.57% | 5 |
| [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) | 5/11 | **6** | 45.45% | 6 |
| [CPathFind](../source/game_sa/PathFind.cpp) | 54/56 | **2** | 96.43% | 2 |
| [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) | 2/7 | **5** | 28.57% | 5 |
| [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) | 9/38 | **29** | 23.68% | 29 |
| [CPedGroup](../source/game_sa/PedGroup.cpp) | 10/12 | **2** | 83.33% | 2 |
| [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) | 1/2 | **1** | 50.00% | 1 |
| [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) | 1/2 | **1** | 50.00% | 1 |
| [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) | 1/4 | **3** | 25.00% | 3 |
| [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) | 2/3 | **1** | 66.67% | 1 |
| [CPickups](../source/game_sa/Pickups.cpp) | 35/37 | **2** | 94.59% | 2 |
| [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) | 14/23 | **9** | 60.87% | 9 |
| [CPools](../source/game_sa/Pools.cpp) | 16/19 | **3** | 84.21% | 3 |
| [CPopulation](../source/game_sa/Population.cpp) | 64/66 | **2** | 96.97% | 2 |
| [CPostEffects](../source/game_sa/PostEffects.cpp) | 35/38 | **3** | 92.11% | 4 |
| [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) | 1/12 | **11** | 8.33% | 11 |
| [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) | 11/12 | **1** | 91.67% | 1 |
| [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) | 5/7 | **2** | 71.43% | 2 |
| [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) | 7/8 | **1** | 87.50% | 1 |
| [CRope](../source/game_sa/Rope.cpp) | 5/7 | **2** | 71.43% | 2 |
| [CRopes](../source/game_sa/Ropes.cpp) | 9/11 | **2** | 81.82% | 2 |
| [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) | 24/39 | **15** | 61.54% | 15 |
| [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) | 3/6 | **3** | 50.00% | 3 |
| [CShadows](../source/game_sa/Shadows.cpp) | 21/31 | **10** | 67.74% | 10 |
| [CStencilShadows](../source/game_sa/StencilShadows.cpp) | 9/13 | **4** | 69.23% | 4 |
| [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) | 4/7 | **3** | 57.14% | 3 |
| [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) | 3/9 | **6** | 33.33% | 6 |
| [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) | 8/9 | **1** | 88.89% | 1 |
| [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) | 1/7 | **6** | 14.29% | 6 |
| [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) | 2/7 | **5** | 28.57% | 5 |
| [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) | 2/5 | **3** | 40.00% | 3 |
| [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) | 3/7 | **4** | 42.86% | 4 |
| [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) | 5/8 | **3** | 62.50% | 3 |
| [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) | 3/7 | **4** | 42.86% | 4 |
| [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) | 3/7 | **4** | 42.86% | 4 |
| [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) | 9/11 | **2** | 81.82% | 2 |
| [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) | 2/7 | **5** | 28.57% | 5 |
| [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) | 9/11 | **2** | 81.82% | 2 |
| [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) | 3/9 | **6** | 33.33% | 6 |
| [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) | 4/7 | **3** | 57.14% | 3 |
| [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) | 2/12 | **10** | 16.67% | 10 |
| [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) | 2/8 | **6** | 25.00% | 6 |
| [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) | 6/8 | **2** | 75.00% | 2 |
| [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) | 9/13 | **4** | 69.23% | 4 |
| [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) | 2/5 | **3** | 40.00% | 3 |
| [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) | 2/8 | **6** | 25.00% | 6 |
| [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) | 2/7 | **5** | 28.57% | 5 |
| [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) | 8/9 | **1** | 88.89% | 1 |
| [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) | 6/8 | **2** | 75.00% | 2 |
| [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) | 3/5 | **2** | 60.00% | 2 |
| [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) | 7/13 | **6** | 53.85% | 6 |
| [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) | 5/7 | **2** | 71.43% | 2 |
| [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) | 6/7 | **1** | 85.71% | 1 |
| [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) | 11/12 | **1** | 91.67% | 1 |
| [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) | 4/7 | **3** | 57.14% | 3 |
| [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) | 2/5 | **3** | 40.00% | 3 |
| [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) | 20/40 | **20** | 50.00% | 20 |
| [CWaterLevel](../source/game_sa/WaterLevel.cpp) | 25/30 | **5** | 83.33% | 5 |
| [CWeapon](../source/game_sa/Weapon.cpp) | 35/41 | **6** | 85.37% | 6 |
| [CWeather](../source/game_sa/Weather.cpp) | 10/13 | **3** | 76.92% | 3 |
| [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) | 2/9 | **7** | 22.22% | 7 |
| [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) | 31/35 | **4** | 88.57% | 4 |
| [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) | 2/24 | **22** | 8.33% | 22 |
<a id="sin-revertir"></a>
## 🔴 Sin revertir · 3 clases

| Clase | Revertidas / total | Pendientes | Avance | Locked |
| --- | ---: | ---: | ---: | ---: |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) | 0/2 | **2** | 0.00% | 2 |
| [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) | 0/7 | **7** | 0.00% | 7 |
| [Interior_c](../source/game_sa/Interior/Interior_c.cpp) | 0/52 | **52** | 0.00% | 52 |
<a id="metodo"></a>
## Cómo leer y actualizar

- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.
- **Locked:** indicador de bloqueo del hook; hay 625 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.
- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.

- **Acceso al código:** pulsa el nombre para abrir el archivo que registra sus hooks. Si el registro está repartido, los enlaces adicionales abren los demás archivos. Algunas clases registran sus hooks en una cabecera.
- **Comentarios de estado:** resumen por clase/grupo, con nombres de pendientes y bloqueadas. Los conteos abarcan la clase completa aunque su implementación esté repartida entre archivos.

Actualizar los flags solo después de comparar con el original dentro del motor y regenerar desde la raíz:

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
