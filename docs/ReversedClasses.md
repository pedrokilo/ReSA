# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-10-06 00:35 UTC

## Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. Completar este catálogo es un hito intermedio hacia **ReSA — Zero Original Function Dependency**.

| Dimensión | Progreso | Medición |
| --- | --- | --- |
| Clases completas | `█████████████████░░░` | 666/771 · 86.38% |
| Clases validadas (de las completas) | `███████████████████░` | 648/666 · 97.30% |
| Funciones reconstruidas | `███████████████████░` | 7,858/8,441 · 93.09% |
| Funciones validadas (de las reconstruidas) | `██████████████░░░░░░` | 5,382/7,858 · 68.49% |

Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL.

Hay 8,441 entradas catalogadas, 650 bloqueadas y 7,791 sin bloqueo. No constituyen un inventario exhaustivo del EXE. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Los extras NOTSA sin equivalente en el EXE (3, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

### Siguientes mediciones

Estas dimensiones todavía no tienen un porcentaje útil: indican el trabajo que falta para poder medirlas.

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validaciones vigentes para el árbol actual | 0/5,382 validadas confirmadas como vigentes | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |
| Dependencias de funciones originales eliminadas | No medido | Auditar, para cada función reconstruida, las llamadas directas y transitivas que todavía saltan a código del EXE (`plugin::Call*`, direcciones fijas, hooks sin reconstruir) y contar las que ya no dependen de ninguna. |

**Validación consolidada parcialmente:** 5382 funciones con evidencia aprobada vinculada, 840 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias. Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

## Vista general

| Clases validadas | Completas por validar | Funciones validadas | Reconstruidas por validar | Funciones por reconstruir |
| :---: | :---: | :---: | :---: | :---: |
| **648 / 666** | **18 / 666** | **5,382 / 7,858** | **2,476 / 7,858** | **583 / 8,441** |
| 97.30% | 2.70% | 68.49% | 31.51% | 6.91% |

Reconstrucción por clases: 666 completas, 103 parciales y 2 sin revertir de 771.

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="completas"></a>
## 🟢 Completas · 666 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 señala bloqueos.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (7) ✅ 7 | [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (9) ✅ 9 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (17) ✅ 17 | [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (7) ✅ 7 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) ✅ 7 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (8) ✅ 8 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (6) ✅ 6 | [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (6) ✅ 6 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (1) ✅ 1 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) ✅ 11 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) ✅ 7 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CKeyArray](../source/game_sa/Text/Text.cpp) (4) ✅ 4 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (5) ✅ 5 | [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) ✅ 7 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (5) ✅ 5 | [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (4) ✅ 4 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (4) ✅ 4 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) ✅ 8 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (7) ✅ 7 | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (2) ✅ 2 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) ✅ 12 | [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (2) ✅ 2 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (2) ✅ 2 | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) ✅ 3 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (5) ✅ 5 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (2) ✅ 2 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (11) ✅ 11 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (54) ✅ 53 | [CMaths](../source/game_sa/Maths.cpp) (3) ✅ 3 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (7) ✅ 7 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) ✅ 8 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (6) ✅ 6 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) ✅ 10 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (6) ✅ 6 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) ✅ 7 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) ✅ 8 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) ✅ 5 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) ✅ 20 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) ✅ 8 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) ✅ 3 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) ✅ 7 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (28) ✅ 28 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) ✅ 7 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) ✅ 6 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) ✅ 9 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (10) ✅ 10 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (12) ✅ 12 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (13) ✅ 13 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) ✅ 8 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) ✅ 7 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) ✅ 2 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (1) ✅ 1 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (11) ✅ 11 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (14) ✅ 14 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) ✅ 58 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (48) ✅ 48 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) ✅ 9 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) ✅ 6 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (10) ✅ 10 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (4) ✅ 4 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (18) ✅ 18 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (4) ✅ 4 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) ✅ 14 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (8) ✅ 8 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (8) ✅ 8 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (8) ✅ 8 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (2) ✅ 2 | [Common](../source/game_sa/common.cpp) (9) ✅ 9 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) ✅ 6 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) ✅ 14 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (7) ✅ 7 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) ✅ 2 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (6) ✅ 6 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPad](../source/game_sa/Pad.cpp) (75) | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) ✅ 7 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) ✅ 7 | [CPathNode](../source/game_sa/PathFind.cpp) (1) ✅ 1 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (9) ✅ 9 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (145) ✅ 9 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (8) ✅ 8 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (13) ✅ 13 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) ✅ 14 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (18) ✅ 18 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) ✅ 7 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (9) ✅ 9 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (2) ✅ 2 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (7) ✅ 7 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (5) ✅ 5 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (5) ✅ 5 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (2) ✅ 2 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (5) ✅ 5 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) ✅ 11 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) ✅ 2 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (1) ✅ 1 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (2) ✅ 2 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (3) ✅ 3 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (2) ✅ 2 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) ✅ 20 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (2) ✅ 2 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) ✅ 8 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (2) ✅ 2 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (10) ✅ 10 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (26) ✅ 26 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (10) ✅ 10 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) ✅ 18 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (10) ✅ 10 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) ✅ 14 | [CPedGroups](../source/game_sa/PedGroups.cpp) (2) ✅ 2 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (9) ✅ 9 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) ✅ 6 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (9) ✅ 9 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (31) ✅ 31 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (52) ✅ 52 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (10) ✅ 10 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) ✅ 9 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) ✅ 6 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) | [CPedList](../source/game_sa/PedList.cpp) (7) ✅ 7 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) ✅ 10 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) ✅ 5 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (10) ✅ 10 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (99) | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (3) ✅ 3 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (9) ✅ 9 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) ✅ 34 | [CPedPool](../source/game_sa/Pools/PedPool.h) (2) ✅ 2 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) ✅ 6 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (6) ✅ 6 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (23) ✅ 23 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (9) ✅ 9 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPedStats](../source/game_sa/PedStats.cpp) (4) ✅ 4 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (9) ✅ 9 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) ✅ 10 |
| [CBridge](../source/game_sa/Bridge.cpp) (4) ✅ 4 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) ✅ 7 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPedType](../source/game_sa/PedType.cpp) (13) ✅ 13 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (6) ✅ 6 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) ✅ 2 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (56) ✅ 56 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (3) ✅ 3 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) ✅ 6 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (16) ✅ 16 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) ✅ 15 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) ✅ 4 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (9) ✅ 9 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) ✅ 6 |
| [CCarAI](../source/game_sa/CarAI.cpp) (25) ✅ 25 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) ✅ 7 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (22) ✅ 22 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (5) ✅ 5 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) ✅ 8 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (6) ✅ 6 |
| [CCheat](../source/game_sa/Cheat.cpp) (72) | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (19) ✅ 19 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (1) ✅ 1 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) ✅ 1 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (28) ✅ 28 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (15) ✅ 15 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (3) ✅ 3 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (51) ✅ 50 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) ✅ 6 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (8) ✅ 8 |
| [CClothes](../source/game_sa/Clothes.cpp) (11) ✅ 11 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (8) ✅ 8 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (5) ✅ 5 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) ✅ 24 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) ✅ 6 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) ✅ 2 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (1) ✅ 1 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (1) ✅ 1 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) ✅ 5 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (59) ✅ 58 🔒 59 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (1) ✅ 1 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) ✅ 6 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (8) ✅ 8 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) ✅ 12 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (4) ✅ 4 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CRadar](../source/game_sa/Radar.cpp) (63) | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (9) ✅ 9 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (8) ✅ 8 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (8) ✅ 8 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CRect](../source/game_sa/Core/Rect.cpp) (8) ✅ 8 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) ✅ 6 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) ✅ 4 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) ✅ 3 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) ✅ 23 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) ✅ 3 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) ✅ 3 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CReplay](../source/game_sa/Replay.cpp) (47) ✅ 47 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) ✅ 3 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) ✅ 3 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (5) ✅ 5 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) ✅ 3 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) ✅ 3 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) ✅ 3 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (10) ✅ 10 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) ✅ 3 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) ✅ 7 | [CSetPiece](../source/game_sa/SetPiece.cpp) (3) ✅ 3 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) ✅ 4 |
| [CCoronas](../source/game_sa/Coronas.cpp) (11) ✅ 11 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) ✅ 3 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (19) ✅ 19 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) ✅ 14 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (6) ✅ 6 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CShopping](../source/game_sa/Shopping.cpp) (30) ✅ 30 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (9) ✅ 9 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) ✅ 8 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) ✅ 8 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) ✅ 6 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (4) ✅ 4 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) ✅ 14 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) ✅ 8 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (14) ✅ 14 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (33) ✅ 33 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) ✅ 7 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) ✅ 6 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CStats](../source/game_sa/Stats.cpp) (32) ✅ 32 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (14) ✅ 14 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) ✅ 3 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) ✅ 3 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (9) ✅ 9 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (4) ✅ 4 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (35) ✅ 35 | [CStreaming](../source/game_sa/Streaming.cpp) (94) | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) ✅ 4 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (6) ✅ 6 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (9) ✅ 9 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) ✅ 4 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (8) ✅ 8 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) ✅ 4 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) ✅ 7 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) ✅ 6 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CTagManager](../source/game_sa/TagManager.cpp) (19) ✅ 19 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (2) ✅ 2 |
| [CData](../source/game_sa/Text/Text.cpp) (2) ✅ 2 | [CTask](../source/game_sa/Tasks/Task.cpp) (4) ✅ 4 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) ✅ 3 |
| [CDebug](../source/game_sa/Debug.cpp) (1) ✅ 1 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (3) ✅ 3 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) ✅ 5 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) ✅ 5 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) ✅ 7 |
| [CDirectory](../source/game_sa/Directory.cpp) (10) ✅ 10 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) ✅ 4 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (6) ✅ 6 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) ✅ 6 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (2) ✅ 2 |
| [CDraw](../source/game_sa/Draw.cpp) (2) ✅ 2 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) ✅ 2 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) ✅ 6 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (9) ✅ 9 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) ✅ 5 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (1) ✅ 1 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (3) ✅ 3 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) ✅ 6 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (2) ✅ 2 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) ✅ 6 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (5) ✅ 5 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (5) ✅ 5 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) ✅ 15 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) ✅ 8 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) ✅ 10 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) ✅ 8 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (6) ✅ 6 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (11) ✅ 11 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (3) ✅ 3 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (8) ✅ 8 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (7) ✅ 7 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (11) ✅ 11 🔒 1 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (4) ✅ 4 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) ✅ 11 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (18) ✅ 18 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (5) ✅ 5 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) ✅ 11 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (9) ✅ 9 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (7) ✅ 7 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (3) ✅ 3 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (4) ✅ 4 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (5) ✅ 5 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (9) ✅ 9 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) ✅ 6 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (10) ✅ 10 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) ✅ 1 |
| [CEvent](../source/game_sa/Events/Event.cpp) (3) ✅ 3 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (10) ✅ 10 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) ✅ 4 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (4) ✅ 4 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) ✅ 5 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) ✅ 19 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) ✅ 1 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) ✅ 6 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) ✅ 6 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) ✅ 2 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) ✅ 7 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (3) ✅ 3 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) ✅ 2 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) ✅ 4 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (4) ✅ 4 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) ✅ 6 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) ✅ 6 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (4) ✅ 4 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) ✅ 7 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) ✅ 6 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (2) ✅ 2 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) ✅ 5 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (1) ✅ 1 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (2) ✅ 2 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (8) ✅ 8 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (20) ✅ 20 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (14) ✅ 14 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (4) ✅ 4 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (3) ✅ 3 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (1) ✅ 1 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (9) ✅ 9 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (69) |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (11) ✅ 11 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (3) ✅ 3 | [CTheZones](../source/game_sa/TheZones.cpp) (21) ✅ 21 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (1) ✅ 1 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (9) ✅ 9 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) ✅ 3 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (9) ✅ 9 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) ✅ 12 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (8) ✅ 8 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) ✅ 2 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (5) ✅ 5 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (9) ✅ 9 | [CTimer](../source/game_sa/Timer.cpp) (13) ✅ 13 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) ✅ 2 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) ✅ 9 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) ✅ 10 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (6) ✅ 6 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) ✅ 13 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (2) ✅ 2 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) ✅ 8 | [cTransmission](../source/game_sa/cTransmission.cpp) (4) ✅ 4 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (2) ✅ 2 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) ✅ 8 | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) ✅ 26 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (8) ✅ 8 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) ✅ 11 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) ✅ 8 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) ✅ 1 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) ✅ 8 | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (142) ✅ 1 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) ✅ 19 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (1) ✅ 1 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) ✅ 2 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) ✅ 13 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (75) |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) ✅ 1 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (13) ✅ 13 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (1) ✅ 1 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (2) ✅ 2 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) ✅ 8 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) ✅ 1 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (5) ✅ 5 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (2) ✅ 2 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) ✅ 6 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (3) ✅ 3 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) ✅ 11 | [CWanted](../source/game_sa/Wanted.cpp) (33) ✅ 33 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) ✅ 7 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (2) ✅ 2 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) ✅ 3 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (9) ✅ 9 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) ✅ 2 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) ✅ 8 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (10) ✅ 10 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) ✅ 1 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) ✅ 7 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) ✅ 3 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) ✅ 4 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (2) ✅ 2 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (9) ✅ 9 | [CWorld](../source/game_sa/World.cpp) (94) |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (2) ✅ 2 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) ✅ 6 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (3) ✅ 3 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) ✅ 7 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (2) ✅ 2 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (6) ✅ 6 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) ✅ 3 |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (8) ✅ 8 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) ✅ 9 | [Fx_c](../source/game_sa/Fx/Fx.cpp) (8) ✅ 8 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) ✅ 1 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (1) ✅ 1 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) ✅ 1 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) ✅ 1 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (4) ✅ 4 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (2) ✅ 2 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) ✅ 3 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (7) ✅ 7 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) ✅ 5 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (4) ✅ 4 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) ✅ 8 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) ✅ 2 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (3) ✅ 3 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (3) ✅ 3 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) ✅ 4 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (5) ✅ 5 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (8) ✅ 8 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) ✅ 2 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) ✅ 8 | [Game](../source/app/app_game.cpp) (12) ✅ 12 🔒 3 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) ✅ 2 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (10) ✅ 10 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (3) ✅ 3 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) ✅ 9 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (3) ✅ 3 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) ✅ 8 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (5) ✅ 5 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (3) ✅ 3 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (8) ✅ 8 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) ✅ 21 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (2) ✅ 2 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) ✅ 5 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (5) ✅ 5 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (9) ✅ 9 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (4) ✅ 4 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) ✅ 2 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (47) ✅ 47 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (1) ✅ 1 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (1) ✅ 1 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (1) ✅ 1 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) ✅ 8 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) ✅ 7 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) ✅ 4 |
| [CGame](../source/game_sa/Game.cpp) (22) ✅ 22 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (1) ✅ 1 | [Ps](../source/app/platform/win/WinPs.cpp) (22) ✅ 22 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) ✅ 9 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) ✅ 13 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (43) ✅ 43 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (8) ✅ 8 | [Rs](../source/app/platform/platform.cpp) (27) ✅ 27 |
| [CGarage](../source/game_sa/Garage.cpp) (4) ✅ 4 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (9) ✅ 9 | [RtAnim](../source/game_sa/RenderWare/rw/rtanim.cpp) (1) ✅ 1 |
| [CGarages](../source/game_sa/Garages.cpp) (21) ✅ 21 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (9) ✅ 9 | [RwCore](../source/game_sa/RenderWare/rw/rwcore.cpp) (4) ✅ 4 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) ✅ 10 | [RwHelper](../source/game_sa/RwHelper.cpp) (24) ✅ 24 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (9) ✅ 9 | [Securom](../source/app/securom/Securom.cpp) (2) ✅ 2 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) ✅ 7 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) ✅ 47 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) ✅ 5 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) ✅ 11 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) ✅ 6 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) ✅ 3 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) ✅ 9 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) ✅ 3 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (3) ✅ 3 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (13) ✅ 13 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (11) ✅ 11 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) ✅ 10 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (6) ✅ 5 |

</details>

<a id="parciales"></a>
## 🟡 Parciales · 103 clases

Orden alfabético para localizar clases. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

| Clase | Revertidas / total | Pendientes | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) | 4/6 | **2** | 66.67% | 0 | 2 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) | 16/20 | **4** | 80.00% | 1 | 4 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) | 13/17 | **4** | 76.47% | 0 | 4 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) | 29/43 | **14** | 67.44% | 0 | 14 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) | 8/19 | **11** | 42.11% | 0 | 11 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) | 21/24 | **3** | 87.50% | 3 | 3 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) | 102/106 | **4** | 96.23% | 0 | 7 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) | 22/40 | **18** | 55.00% | 0 | 18 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) | 7/10 | **3** | 70.00% | 0 | 3 |
| [CCam](../source/game_sa/Cam.cpp) | 14/44 | **30** | 31.82% | 0 | 30 |
| [CCamera](../source/game_sa/Camera.cpp) | 91/110 | **19** | 82.73% | 0 | 19 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) | 27/32 | **5** | 84.38% | 0 | 5 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) | 9/10 | **1** | 90.00% | 0 | 1 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) | 15/23 | **8** | 65.22% | 0 | 8 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) | 3/7 | **4** | 42.86% | 0 | 4 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) | 1/3 | **2** | 33.33% | 0 | 2 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) | 69/72 | **3** | 95.83% | 0 | 3 |
| [CFont](../source/game_sa/Font.cpp) | 32/36 | **4** | 88.89% | 0 | 4 |
| [CFormation](../source/game_sa/Formation.cpp) | 1/9 | **8** | 11.11% | 0 | 8 |
| [CGangWars](../source/game_sa/GangWars.cpp) | 32/35 | **3** | 91.43% | 0 | 3 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) | 5/19 | **14** | 26.32% | 0 | 14 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) | 30/32 | **2** | 93.75% | 0 | 2 |
| [CHud](../source/game_sa/Hud.cpp) | 30/38 | **8** | 78.95% | 0 | 8 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) | 13/14 | **1** | 92.86% | 0 | 1 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) | 2/7 | **5** | 28.57% | 0 | 5 |
| [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) | 5/11 | **6** | 45.45% | 0 | 6 |
| [CPathFind](../source/game_sa/PathFind.cpp) | 54/56 | **2** | 96.43% | 0 | 2 |
| [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) | 19/20 | **1** | 95.00% | 19 | 1 |
| [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) | 2/7 | **5** | 28.57% | 0 | 5 |
| [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) | 9/38 | **29** | 23.68% | 0 | 29 |
| [CPedGroup](../source/game_sa/PedGroup.cpp) | 10/12 | **2** | 83.33% | 0 | 2 |
| [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) | 1/4 | **3** | 25.00% | 0 | 3 |
| [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) | 2/4 | **2** | 50.00% | 0 | 2 |
| [CPickups](../source/game_sa/Pickups.cpp) | 35/37 | **2** | 94.59% | 0 | 2 |
| [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) | 14/23 | **9** | 60.87% | 0 | 9 |
| [CPools](../source/game_sa/Pools.cpp) | 16/19 | **3** | 84.21% | 0 | 3 |
| [CPopulation](../source/game_sa/Population.cpp) | 64/66 | **2** | 96.97% | 0 | 2 |
| [CPostEffects](../source/game_sa/PostEffects.cpp) | 35/38 | **3** | 92.11% | 0 | 4 |
| [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) | 1/12 | **11** | 8.33% | 0 | 11 |
| [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) | 11/12 | **1** | 91.67% | 0 | 1 |
| [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) | 5/7 | **2** | 71.43% | 0 | 2 |
| [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) | 7/8 | **1** | 87.50% | 0 | 1 |
| [CRope](../source/game_sa/Rope.cpp) | 5/7 | **2** | 71.43% | 0 | 2 |
| [CRopes](../source/game_sa/Ropes.cpp) | 9/11 | **2** | 81.82% | 0 | 2 |
| [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) | 24/39 | **15** | 61.54% | 0 | 15 |
| [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) | 3/6 | **3** | 50.00% | 0 | 3 |
| [CShadows](../source/game_sa/Shadows.cpp) | 21/31 | **10** | 67.74% | 0 | 10 |
| [CStencilShadows](../source/game_sa/StencilShadows.cpp) | 9/13 | **4** | 69.23% | 0 | 4 |
| [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) | 4/7 | **3** | 57.14% | 0 | 3 |
| [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) | 2/3 | **1** | 66.67% | 0 | 1 |
| [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) | 7/9 | **2** | 77.78% | 0 | 2 |
| [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) | 3/9 | **6** | 33.33% | 1 | 6 |
| [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) | 8/9 | **1** | 88.89% | 0 | 1 |
| [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) | 8/9 | **1** | 88.89% | 0 | 1 |
| [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) | 5/8 | **3** | 62.50% | 0 | 3 |
| [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) | 4/8 | **4** | 50.00% | 0 | 4 |
| [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) | 7/8 | **1** | 87.50% | 0 | 1 |
| [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) | 7/8 | **1** | 87.50% | 0 | 1 |
| [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) | 1/7 | **6** | 14.29% | 0 | 6 |
| [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) | 1/8 | **7** | 12.50% | 1 | 7 |
| [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) | 2/7 | **5** | 28.57% | 0 | 5 |
| [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) | 2/5 | **3** | 40.00% | 0 | 3 |
| [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) | 3/7 | **4** | 42.86% | 0 | 4 |
| [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) | 5/8 | **3** | 62.50% | 0 | 3 |
| [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) | 3/7 | **4** | 42.86% | 1 | 4 |
| [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) | 3/7 | **4** | 42.86% | 0 | 4 |
| [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) | 2/10 | **8** | 20.00% | 0 | 8 |
| [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) | 9/11 | **2** | 81.82% | 0 | 2 |
| [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) | 2/7 | **5** | 28.57% | 0 | 5 |
| [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) | 9/11 | **2** | 81.82% | 0 | 2 |
| [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) | 3/4 | **1** | 75.00% | 0 | 1 |
| [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) | 3/9 | **6** | 33.33% | 0 | 6 |
| [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) | 7/11 | **4** | 63.64% | 0 | 4 |
| [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) | 4/7 | **3** | 57.14% | 0 | 3 |
| [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) | 2/12 | **10** | 16.67% | 0 | 10 |
| [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) | 2/8 | **6** | 25.00% | 0 | 6 |
| [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) | 6/8 | **2** | 75.00% | 0 | 2 |
| [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) | 9/13 | **4** | 69.23% | 0 | 4 |
| [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) | 3/6 | **3** | 50.00% | 0 | 3 |
| [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) | 4/5 | **1** | 80.00% | 0 | 1 |
| [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) | 3/9 | **6** | 33.33% | 0 | 6 |
| [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) | 2/5 | **3** | 40.00% | 0 | 3 |
| [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) | 2/8 | **6** | 25.00% | 0 | 6 |
| [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) | 2/7 | **5** | 28.57% | 0 | 5 |
| [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) | 3/5 | **2** | 60.00% | 0 | 2 |
| [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) | 7/13 | **6** | 53.85% | 0 | 6 |
| [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) | 4/5 | **1** | 80.00% | 0 | 1 |
| [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) | 4/5 | **1** | 80.00% | 0 | 1 |
| [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) | 4/5 | **1** | 80.00% | 0 | 1 |
| [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) | 2/5 | **3** | 40.00% | 0 | 3 |
| [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) | 5/7 | **2** | 71.43% | 0 | 2 |
| [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) | 6/7 | **1** | 85.71% | 0 | 1 |
| [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) | 1/4 | **3** | 25.00% | 0 | 3 |
| [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) | 11/12 | **1** | 91.67% | 0 | 1 |
| [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) | 4/7 | **3** | 57.14% | 1 | 3 |
| [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) | 2/5 | **3** | 40.00% | 0 | 3 |
| [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) | 20/40 | **20** | 50.00% | 0 | 20 |
| [CWaterLevel](../source/game_sa/WaterLevel.cpp) | 25/30 | **5** | 83.33% | 0 | 5 |
| [CWeapon](../source/game_sa/Weapon.cpp) | 35/41 | **6** | 85.37% | 0 | 6 |
| [CWeather](../source/game_sa/Weather.cpp) | 10/13 | **3** | 76.92% | 0 | 3 |
| [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) | 2/9 | **7** | 22.22% | 0 | 7 |
| [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) | 31/35 | **4** | 88.57% | 0 | 4 |
| [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) | 2/24 | **22** | 8.33% | 0 | 22 |

<a id="sin-revertir"></a>
## 🔴 Sin revertir · 2 clases

| Clase | Revertidas / total | Pendientes | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: |
| [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) | 0/1 | **1** | 0.00% | 0 | 1 |
| [Interior_c](../source/game_sa/Interior/Interior_c.cpp) | 0/52 | **52** | 0.00% | 0 | 52 |

<a id="metodo"></a>
## Cómo leer y actualizar

- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.
- **Validated:** `1` indica evidencia aprobada vinculada para esa entrada. No implica vigencia actual ni independencia del EXE.
- **Locked:** indicador de bloqueo del hook; hay 650 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
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
