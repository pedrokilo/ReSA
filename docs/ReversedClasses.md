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
| [App](../source/app/app.cpp) (7) | [CGlass](../source/game_sa/Glass.cpp) (19) | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (17) | [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) | [CGridRef](../source/game_sa/GridRef.cpp) (5) | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) | [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (6) | [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (13) |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) | [CHandShaker](../source/game_sa/HandShaker.cpp) (3) | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) | [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (11) | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (9) |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) | [CHudColours](../source/game_sa/HudColours.cpp) (5) | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (7) |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) | [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (5) | [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (8) |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (5) | [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (6) |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) | [CIniFile](../source/game_sa/IniFile.cpp) (1) | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) | [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (1) | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) | [CIplStore](../source/game_sa/IplStore.cpp) (34) | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) | [CKeyArray](../source/game_sa/Text/Text.cpp) (4) | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (54) | [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) | [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (4) |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (6) | [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (4) |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (17) | [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (2) |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (2) |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) | [CLocalisation](../source/game_sa/Localisation.cpp) (18) | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (5) |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (7) |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (10) | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (13) | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) | [CMessages](../source/game_sa/Messages.cpp) (28) | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (11) | [CMirrors](../source/game_sa/Mirrors.cpp) (8) | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (10) | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (28) | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (18) | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (12) |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (2) | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (1) |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (14) |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) | [CObject](../source/game_sa/Entity/Object/Object.cpp) (48) | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) | [CObjectData](../source/game_sa/ObjectData.cpp) (2) | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (18) |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (13) | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (5) |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) | [COccluder](../source/game_sa/Occluder.cpp) (3) | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (5) |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (9) | [COcclusion](../source/game_sa/Occlusion.cpp) (5) | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (1) |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (20) | [Common](../source/game_sa/common.cpp) (9) | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (3) |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (6) | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) | [CPad](../source/game_sa/Pad.cpp) (75) | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (10) |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) | [CPathNode](../source/game_sa/PathFind.cpp) (1) | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (10) |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (145) | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (10) |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (9) |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (9) |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (10) |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (7) | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (31) | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (2) | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (10) |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (2) | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (9) |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (2) | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (99) | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (26) | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (6) |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (9) |
| [CBirds](../source/game_sa/Birds.cpp) (6) | [CPedGroups](../source/game_sa/PedGroups.cpp) (2) | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (9) |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (23) | [CPedIK](../source/game_sa/PedIK.cpp) (9) | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (52) | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (6) |
| [CBridge](../source/game_sa/Bridge.cpp) (4) | [CPedList](../source/game_sa/PedList.cpp) (7) | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (3) | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) | [CPedPool](../source/game_sa/Pools/PedPool.h) (2) | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (1) |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (9) | [CPedStats](../source/game_sa/PedStats.cpp) (4) | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) |
| [CCarAI](../source/game_sa/CarAI.cpp) (25) | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (15) |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (22) | [CPedType](../source/game_sa/PedType.cpp) (13) | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) | [CPhysical](../source/game_sa/Entity/Physical.cpp) (56) | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (5) |
| [CCheat](../source/game_sa/Cheat.cpp) (72) | [CPickup](../source/game_sa/Pickup.cpp) (12) | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (16) | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (3) | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) |
| [CClock](../source/game_sa/Clock.cpp) (9) | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (4) |
| [CClothes](../source/game_sa/Clothes.cpp) (11) | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) |
| [CClouds](../source/game_sa/Clouds.cpp) (21) | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (8) |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (19) | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (1) | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (28) | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (59) 🔒 59 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (51) | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (8) | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (9) | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (1) | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (1) | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) | [CRadar](../source/game_sa/Radar.cpp) (63) | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (8) | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) | [CRect](../source/game_sa/Core/Rect.cpp) (8) | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) | [CReferences](../source/game_sa/References.cpp) (4) | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (6) |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (11) | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (9) |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) |
| [CCoronas](../source/game_sa/Coronas.cpp) (11) | [CRenderer](../source/game_sa/Renderer.cpp) (33) | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) |
| [CCover](../source/game_sa/Cover.cpp) (16) | [CReplay](../source/game_sa/Replay.cpp) (47) | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) |
| [CCranes](../source/game_sa/Cranes.cpp) (5) | [CRestart](../source/game_sa/Restart.cpp) (11) | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (4) |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (5) | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) |
| [CCrime](../source/game_sa/Crime.cpp) (2) | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) |
| [CCurves](../source/game_sa/Curves.cpp) (6) | [CSetPiece](../source/game_sa/SetPiece.cpp) (3) | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (14) | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (19) | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (9) |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (14) | [CShopping](../source/game_sa/Shopping.cpp) (30) | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (9) | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (35) | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (2) |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (6) | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) | [CSprite](../source/game_sa/Sprite.cpp) (15) | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (6) |
| [CData](../source/game_sa/Text/Text.cpp) (2) | [CSprite2d](../source/game_sa/Sprite2d.cpp) (33) | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) |
| [CDebug](../source/game_sa/Debug.cpp) (1) | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) | [CStats](../source/game_sa/Stats.cpp) (32) | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) |
| [CDirectory](../source/game_sa/Directory.cpp) (10) | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) |
| [CDoor](../source/game_sa/Door.cpp) (6) | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) |
| [CDraw](../source/game_sa/Draw.cpp) (2) | [CStreaming](../source/game_sa/Streaming.cpp) (94) | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (9) | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (7) |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (1) | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (8) | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (3) |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (2) | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (8) |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) | [CTagManager](../source/game_sa/TagManager.cpp) (19) | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (7) |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) | [CTask](../source/game_sa/Tasks/Task.cpp) (4) | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (3) | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (9) |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (3) |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (18) | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (3) | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (5) | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) |
| [CEvent](../source/game_sa/Events/Event.cpp) (3) | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (11) | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (4) | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) | [CText](../source/game_sa/Text/Text.cpp) (9) |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (9) | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (3) | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (10) | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (69) |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (4) | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (10) | [CTheZones](../source/game_sa/TheZones.cpp) (21) |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (4) | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (2) | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (2) | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (20) | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) | [CTimer](../source/game_sa/Timer.cpp) (13) |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (4) | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (1) | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (11) | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) | [cTransmission](../source/game_sa/cTransmission.cpp) (4) |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (1) | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (8) | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (14) | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (9) | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (5) | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (9) | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (142) |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (9) | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (8) | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (2) | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (9) | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (75) |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (2) | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (1) |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (8) | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (6) | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) | [CWanted](../source/game_sa/Wanted.cpp) (33) |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (9) |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (10) |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (2) | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (1) | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (2) | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (13) 🔒 10 | [CWorld](../source/game_sa/World.cpp) (94) |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (3) | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (5) | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (2) | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) | [Fx_c](../source/game_sa/Fx/Fx.cpp) (8) |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (4) |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (2) | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (2) | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (3) | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (2) | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (9) | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (8) | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) | [Game](../source/app/app_game.cpp) (12) 🔒 3 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (6) | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (2) | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (5) |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (1) | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (4) | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (5) |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (3) | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) | [Light](../source/app/app_light.cpp) (27) |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (5) | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (7) | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (3) | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (3) | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (8) | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (3) | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (3) | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (10) | [Ps](../source/app/platform/win/WinPs.cpp) (22) |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (2) | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (43) |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) | [Rs](../source/app/platform/platform.cpp) (27) |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (47) | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (9) | [RtAnim](../source/game_sa/RenderWare/rw/rtanim.cpp) (1) |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) 🔒 13 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (1) | [RwCore](../source/game_sa/RenderWare/rw/rwcore.cpp) (4) |
| [CFire](../source/game_sa/Fire.cpp) (5) | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (1) | [RwHelper](../source/game_sa/RwHelper.cpp) (24) |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) | [Securom](../source/app/securom/Securom.cpp) (2) |
| [CGame](../source/game_sa/Game.cpp) (22) | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (1) | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) |
| [CGamma](../source/game_sa/Gamma.cpp) (2) | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) |
| [CGangs](../source/game_sa/Gangs.cpp) (8) | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (8) | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) |
| [CGarage](../source/game_sa/Garage.cpp) (4) | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (9) | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) |
| [CGarages](../source/game_sa/Garages.cpp) (21) | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (6) |
| [CGeneral](../source/game_sa/General.cpp) (9) | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (9) |  |

| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) |  |  |

| [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (8) |  |  |
| [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (8) |  |  |
| [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) |  |  |
| [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (7) |  |  |
| [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) |  |  |
| [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) |  |  |
</details>

<a id="parciales"></a>
## 🟡 Parciales · 88 clases

Orden alfabético para localizar clases. Para decidir qué abordar primero, consultar la [estimación por dificultad](ReversingPriorities.md).

| Clase | Revertidas / total | Pendientes | Avance | Locked |
| --- | ---: | ---: | ---: | ---: |
| [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) | 6/8 | **2** | 75.00% | 2 |
| [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) | 8/9 | **1** | 88.89% | 1 |
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
| [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) | 2/3 | **1** | 66.67% | 1 |
| [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) | 9/38 | **29** | 23.68% | 29 |
| [CPedGroup](../source/game_sa/PedGroup.cpp) | 10/12 | **2** | 83.33% | 2 |
| [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) | 1/2 | **1** | 50.00% | 1 |
| [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) | 1/2 | **1** | 50.00% | 1 |
| [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) | 1/4 | **3** | 25.00% | 3 |
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
| [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) | 0/7 | **7** | 0.00% | 7 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) | 0/2 | **2** | 0.00% | 2 |
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
