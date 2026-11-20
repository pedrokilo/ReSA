#include "StdInc.h"
#include "ModelInfoAccelerator.h"
#include "PatrolRoute.h"
#include "AttractorScanner.h"
#include "ObjectScanner.h"
#include "Events/EventSource.h"
#include "Events/EventPotentialGetRunOver.h"
#include "Events/EventRevived.h"
#include "Events/EventInWater.h"
#include "Events/EventFireNearby.h"
#include "Events/EventSexyVehicle.h"
#include "Events/EventChatPartner.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventSexyPed.h"
#include "RenderWare/D3DResourceSystem.h"
#include "PedSaveStructure.h"
#include "Interior/TaskInteriorSitInChair.h"
#include "MBlur.h"
#include "Rubbish.h"
#include "PlaneBanners.h"
// Lote 216A (sin validar): constructores y destructores de los FxInfo*_c
#include "FxInfoGroundCollide.h"
#include "FxInfoWind.h"
#include "FxInfoJitter.h"
#include "FxInfoRotSpeed.h"
#include "FxInfoFloat.h"
#include "FxInfoUnderwater.h"
#include "FxInfoColour.h"
#include "FxInfoSize.h"
#include "FxInfoSpriteRect.h"
#include "FxInfoHeatHaze.h"
#include "FxInfoTrail.h"
#include "FxInfoFlat.h"
#include "FxInfoDir.h"
#include "FxInfoAnimTexture.h"
#include "FxInfoColourRange.h"
#include "FxInfoSelfLit.h"
#include "FxInfoColourBright.h"
#include "FxInfoSmoke.h"
#include "FxInfoEmRate.h"
#include "FxInfoEmSize.h"
#include "FxInfoEmSpeed.h"
#include "FxInfoEmDir.h"
#include "FxInfoEmAngle.h"
#include "FxInfoEmLife.h"
#include "FxInfoEmPos.h"

#include <extensions/CommandLine.h>

// Audio
// -- General
#include "AEAudioEnvironment.h"
#include "AEAudioUtility.h"
#include "AESmoothFadeThread.h"
#include "AESound.h"
#include "AEStreamThread.h"
#include "AudioEngine.h"
#include "AudioZones.h"
// -- Entities
#include "AEAudioEntity.h"
#include "AECollisionAudioEntity.h"
#include "AEDoorAudioEntity.h"
#include "AEExplosionAudioEntity.h"
#include "AEFireAudioEntity.h"
#include "AEFrontendAudioEntity.h"
#include "AEGlobalWeaponAudioEntity.h"
#include "AEPedAudioEntity.h"
#include "AEPedlessSpeechAudioEntity.h"
#include "AEPedSpeechAudioEntity.h"
#include "AEPedWeaponAudioEntity.h"
#include "AEPoliceScannerAudioEntity.h"
#include "AEScriptAudioEntity.h"
#include "AETwinLoopSoundEntity.h"
#include "AEVehicleAudioEntity.h"
#include "AEWaterCannonAudioEntity.h"
#include "AEWeaponAudioEntity.h"
#include "AEWeatherAudioEntity.h"
// -- Hardware
#include "AEAudioChannel.h"
#include "AEAudioHardware.h"
#include "AEStaticChannel.h"
#include "AEStreamingChannel.h"
// -- Loaders
#include "AEBankLoader.h"
#include "AEDataStream.h"
#include "AEMFDecoder.h"
#include "AEMP3BankLoader.h"
#include "AEMP3TrackLoader.h"
#include "AEStreamingDecoder.h"
#include "AEStreamTransformer.h"
#include "AETrackLoader.h"
#include "AEVorbisDecoder.h"
#include "AEWaveDecoder.h"
#include "AEWMADecoder.h"
// -- Managers
#include "AEAmbienceTrackManager.h"
#include "AECutsceneTrackManager.h"
#include "AERadioTrackManager.h"
#include "AESoundManager.h"
#include "AEUserRadioTrackManager.h"


#include "Garage.h"
#include "Garages.h"
#include "MissionCleanup.h"
#include "Remote.h"

// FX
#include "FxSystemBP.h"
#include "FxSystem.h"
#include "FxSphere.h"
#include "FxPrimBP.h"
#include "FxPrim.h"
#include "FxMemoryPool.h"
#include "FxManager.h"
#include "FxFrustumInfo.h"
#include "FxEmitterPrt.h"
#include "FxEmitterBP.h"
#include "FxInterpInfoFloat.h"
#include "FxInterpInfoU255.h"
#include "FxInterpInfo32.h"
#include "FxInterpInfo255.h"
#include "FxInfoEmWeather.h"
#include "FxInfoEmRotation.h"
#include "FxInfoNoise.h"
#include "FxInfoForce.h"
#include "FxInfoFriction.h"
#include "FxInfoAttractPt.h"
#include "FxInfoAttractLine.h"
#include "FxEmitter.h"
#include "Fx.h"

#include "UIRenderer.h"

#include "ControllerConfigManager.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "Radar.h"
#include "WaterCannons.h"
#include "Lines.h"
#include "Escalators.h"
#include "MovingThings.h"
#include "PlaneTrail.h"
#include "PlaneTrails.h"
#include "Gamma.h"
#include "CustomBuildingPipeline.h"
#include "CustomBuildingDNPipeline.h"
#include "CustomCarEnvMapPipeline.h"
#include "CustomBuildingRenderer.h"
#include "PedType.h"
#include "Occlusion.h"
#include "Occluder.h"
#include "ActiveOccluder.h"
#include "MotionBlurStreaks.h"
#include "GroupEventHandler.h"
#include "SimpleVariablesSaveStructure.h"
#include "BreakManager_c.h"
#include "Buoyancy.h"
#include "CreepingFire.h"
#include "Restart.h"
#include "SetPiece.h"
#include "SetPieces.h"
#include "BulletInfo.h"
#include "Explosion.h"
#include "C_PcSave.h"
#include "FireManager.h"
#include "Skidmarks.h"
#include "CarCtrl.h"
#include "TagManager.h"
#include "Clouds.h"
#include "StuntJumpManager.h"
#include "CustomCarPlateMgr.h"
#include "WindModifiers.h"
#include "PedPlacement.h"
#include "UpsideDownCarCheck.h"
#include "PedClothesDesc.h"
#include "OctTree.h"
#include "OctTreeBase.h"
#include "WaterCannon.h"
#include "EntryExitManager.h"
#include "ObjectSaveStructure.h"
#include "PlayerInfoSaveStructure.h"
#include "GangWarsSaveStructure.h"
#include "PedStats.h"
#include "TrafficLights.h"
#include "CurrentVehicle.h"
#include "PlaceName.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "OnscreenCounterEntry.h"
#include "PostEffects.h"
#include "Rope.h"
#include "Ropes.h"
#include "FallingGlassPane.h"
#include "Glass.h"
#include "FurnitureGroup_c.h"
#include "Furniture_c.h"
#include "FurnitureManager_c.h"
#include "FurnitureSubGroup_c.h"
#include "Interior_c.h"
#include "InteriorGroup_c.h"
#include "InteriorManager_c.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "Ragdoll/IKChain.h"
#include "Ragdoll/IKChainManager.h"
#include "Ragdoll/BoneNode.h"
#include "Ragdoll/BoneNodeManager.h"
#include "TheScripts.h"
#include "SearchLight.h"
#include "TrainNode.h"
#include "RunningScript.h"
#include "Scripted2dEffects.h"
#include "ScriptResourceManager.h"
#include "PedAttractorManager.h"
#include "Attractors/PedAttractorPedPlacer.h"
#include "LoadingScreen.h"
#include "Securom.h"
#include "GridRef.h"
#include "MenuSystem.h"
#include "InterestingEvents.h"
#include "Shadows.h"
#include "ShadowCamera.h"
#include "VehicleRecording.h"
#include "Birds.h"
#include "Hud.h"
#include "Attractors/PedAttractor.h"
#include "Attractors/PedShelterAttractor.h"
#include "Attractors/PedAtmAttractor.h"
#include "Attractors/PedSeatAttractor.h"
#include "Attractors/PedStopAttractor.h"
#include "Attractors/PedPizzaAttractor.h"
#include "ProtectionStub.h"
#include "RenderWare/D3D9RasterDriver.h"
#include "Plugins/JPegCompressPlugin/LibJpegCompress.h"
#include "Plugins/JPegCompressPlugin/LibJpegDecompress.h"
#include "Plugins/JPegCompressPlugin/LibJpegMemory.h"
#include "Plugins/JPegCompressPlugin/LibJpegError.h"
#include "Plugins/JPegCompressPlugin/LibJpegDecompressUpsample.h"
#include "Plugins/JPegCompressPlugin/LibJpegCompressApi.h"
#include "Plugins/JPegCompressPlugin/LibJpegDecompressApi.h"
#include "Attractors/PedTriggerScriptAttractor.h"
#include "Attractors/PedLookAtAttractor.h"
#include "Attractors/PedScriptedAttractor.h"
#include "Attractors/PedParkAttractor.h"
#include "Attractors/PedStepAttractor.h"
#include "ExeProtectionStub.h"
#include "RenderWare/D3D9RasterFormat.h"
#include "Plugins/JPegCompressPlugin/LibJpegCompressEncode.h"
#include "IdleCam.h"
#include "Tasks/TaskTypes/TaskComplexChat.h"
#include "Credits.h"
#include "Tasks/TaskComplexUseSequence.h"
#include "Entity/Ped/PlayerSkin.h"
#include "CoverPoint.h"
#include "DecisionMakers/Decision.h"
#include "Maths.h"
#include "Tasks/TaskSequences.h"
#include "Models/LodTimeModelInfo.h"
#include "VehicleSaveStructure.h"
#include "Tasks/TaskTimer.h"
#include "CarFXRenderer.h"
#include "Curves.h"
#include "ProcObjectMan.h"
#include "ProcSurfaceInfo.h"
#include "Pickup.h"
#include "Pickups.h"
#include "PhoneInfo.h"
#include "PedIK.h"
#include "HandShaker.h"
#include "TempColModels.h"
#include "Conversations.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "PlayerRelationshipRecorder.h"

// Plant
#include "PlantMgr.h"
#include "PlantColEntEntry.h"
#include "PlantLocTri.h"
#include "GrassRenderer.h"
#include "PPTriPlantBuffer.h"
#include "PlantSurfPropMgr.h"

// Task ped group default allocators
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h>
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h>
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h>
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h>
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h>
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h>
#include <Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h>

#include <Tasks/Allocators/TaskAllocatorKillThreatsBasic.h>
#include <Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h>
#include <Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h>
#include <Tasks/Allocators/TaskAllocatorKillOnFoot.h>
#include <Tasks/Allocators/TaskAllocatorAttack.h>
#include <Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h>

// Tasks
#include "TaskSimpleKillPedWithCar.h"
#include "TaskComplexSitDownThenIdleThenStandUp.h"
#include "TaskComplexLeaveCarAsPassengerWait.h"
#include "TaskComplexInvestigateDeadPed.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarFallOut.h"
#include "TaskSimpleCarForcePedOut.h"
#include "TaskSimpleCarGetIn.h"
#include "TaskSimpleBeHit.h"
#include "TaskSimpleHitByGunFromFront.h"
#include "TaskSimpleHitByGunFromLeft.h"
#include "TaskSimpleHitByGunFromRear.h"
#include "TaskSimpleHitByGunFromRight.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculator.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.h"
#include "TaskComplexSeekEntityAnyMeansXYOffset.h"
#include "TaskComplexFleeEntity.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskSimpleArrestPed.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexDestroyCarMelee.h"
#include "TaskComplexDestroyCarArmed.h"
#include "TaskSimpleAbseil.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"
#include "TaskComplexUseMobilePhone.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleChoking.h"
#include "TaskComplexPartnerChat.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"
#include "Interior/TaskInteriorGoToInfo.h"
#include "TaskSimpleInAir.h"
#include "TaskComplexHitPedWithCar.h"
#include "TaskSimpleEvasiveStep.h"
#include "TaskSimpleEvasiveDive.h"
#include "TaskComplexWanderGang.h"
#include "TaskComplexOpenPassengerDoor.h"
#include "TaskComplexOpenDriverDoor.h"
#include "TaskComplexObserveTrafficLights.h"
#include "TaskComplexEvasiveStep.h"
#include "TaskComplexEvasiveCower.h"
#include "TaskComplexClimb.h"
#include "TaskComplexBeStill.h"
#include "TaskComplexBeInCouple.h"
#include "Tasks/TaskCategories.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskSimpleDead.h"
#include "TaskComplexUsePairedAttractor.h"
#include "TaskComplexScreamInCarThenLeave.h"
#include "TaskComplexEnterCarAsPassengerWait.h"
#include "TaskSimplePlayerOnFoot.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexFollowPedFootsteps.h"
#include "Interior/TaskInteriorSitAtDesk.h"
#include "TaskComplexFollowLeaderAnyMeans.h"
#include "TaskSimpleFightingControl.h"
#include "TaskSimpleFinishBrain.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "TaskComplexGoPickUpEntity.h"
#include "TaskSimpleDie.h"
#include "TaskSimpleDuck.h"
#include "TaskComplexPolicePursuit.h"
#include "TaskSimpleFacial.h"
#include "TaskComplexCopInCar.h"
#include "TaskComplexFacial.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexInWater.h"
#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleUninterruptable.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskComplexFallAndStayDown.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskSimpleTriggerLookAt.h"
#include "TaskSimpleHitHead.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.h"
#include "TaskSimpleLand.h"
#include "TaskSimpleJetPack.h"
#include "TaskSimpleSitIdle.h"
#include "Interior/TaskInteriorUseInfo.h"
#include "TaskSimpleSetStayInSamePlace.h"
#include "TaskSimpleJump.h"
#include "TaskSimpleFall.h"
#include "TaskSimpleClimb.h"
#include "TaskComplexWaitForPed.h"
#include "TaskComplexJump.h"
#include "TaskComplexWanderStandard.h"
#include "TaskComplexSunbathe.h"
#include "TaskComplexEnterCar.h"
#include "Interior/TaskInteriorBeInShop.h"
#include "TaskSimpleTogglePedThreatScanner.h"
#include "TaskComplexUseGoggles.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"
#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexWalkAlongsidePed.h"
#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "Interior/TaskInteriorBeInOffice.h"
#include "TaskSimpleGiveCPR.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskComplexDriveFireTruck.h"
#include "Interior/TaskInteriorLieInBed.h"
#include "TaskSimpleSwim.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskSimplePause.h"
#include "TaskComplexShuffleSeats.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskSimpleHurtPedWithCar.h"
#include "TaskComplexPassObject.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleCarSlowBeDraggedOut.h"
#include "TaskComplexReactToAttack.h"
#include "TaskComplexReactToGunAimedAt.h"
#include "Tasks/TaskTypes/TaskSimpleNone.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleThrowProjectile.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexWanderMedic.h"
#include "TaskComplexEvasiveDiveAndGetUp.h"
#include "TaskComplexMoveBackAndJump.h"
#include "TaskComplexSeekEntityAiming.h"
#include "Tasks/TaskTypes/TaskSimpleCarDriveTimed.h"
#include "TaskComplexKillPedOnFootStealth.h"
#include "TaskComplexDriveWander.h"
#include "TaskSimpleStealthKill.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleAnim.h"
#include "TaskSimpleRunAnim.h"
#include "Tasks/TaskTypes/TaskComplexInvestigateDisturbance.h"
#include "TaskComplexWanderCriminal.h"
#include "TaskComplexWanderProstitute.h"
#include "TaskComplexWalkRoundBuildingAttempt.h"
#include "TaskSimpleLookAbout.h"
#include "TaskSimpleOnEscalator.h"
#include "TaskComplexPartnerGreet.h"
#include "Interior/TaskInteriorShopKeeper.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskSimpleShakeFist.h"
#include "TaskComplexFleeShooting.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexLeaveCarAndFlee.h"
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskComplexMedicTreatInjuredPed.h"
#include "TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "TaskComplexKillCriminal.h"
#include "TaskComplexKillAllThreats.h"
#include "TaskComplexFollowPatrolRoute.h"
#include "SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h"
#include "SeekEntity/TaskComplexSeekEntityXYOffset.h"
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "TaskComplexUseScriptedBrain.h"
#include "TaskSimplePlayHandSignalAnim.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexKillPedFromBoat.h"
#include "TaskSimpleHitWall.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarSetTempAction.h"
#include "TaskComplexWanderFlee.h"
#include "TaskComplexWaitAtAttractor.h"
#include "TaskComplexTreatAccident.h"
#include "TaskComplexGoToPointAndStandStillTimed.h"
#include "TaskComplexPartnerShove.h"
#include "TaskComplexFleeAnyMeans.h"
#include "TaskSimpleRunNamedAnim.h"
#include "TaskComplexProstituteSolicit.h"
#include "TaskComplexStuckInAir.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleCower.h"
#include "TaskSimpleHitFromBack.h"
#include "TaskSimpleHitFromBehind.h"
#include "TaskSimpleHitFromFront.h"
#include "TaskSimpleHitFromLeft.h"
#include "TaskSimpleHitFromRight.h"
#include "TaskSimpleGoTo.h"
#include "TaskSimpleUseAtm.h"
#include "TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "TaskSimpleCarSetPedSlowDraggedOut.h"
#include "TaskComplexPartner.h"
#include "TaskComplexFollowPatrolRoute.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexDrivePointRoute.h"
#include "TaskComplexStareAtPed.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskComplexWaitForBus.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskSimpleWaitForPizza.h"
#include "TaskSimpleStandUp.h"
#include "TaskComplexWander.h"
#include "TaskSimplePutDownEntity.h"
#include "TaskComplexUseSwatRope.h"
#include "TaskComplexRoadRage.h"
#include "TaskSimpleGoToPointFine.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskComplexDie.h"
#include "TaskComplexEnterBoatAsDriver.h"
#include "TaskComplexUseAttractor.h"
#include "TaskComplexUseAttractorPartner.h"
#include "TaskComplexAttractorPartnerWait.h"
#include "TaskSimpleFight.h"
#include "TaskComplexUseWaterCannon.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskSimpleSetPedAsAutoDriver.h"
#include "TaskComplexWalkRoundFire.h"
#include "TaskSimpleSlideToCoord.h"
#include "TaskComplexPartnerDeal.h"
#include "TaskSimplePickUpEntity.h"
#include "TaskComplexEnterCarAsDriverTimed.h"
#include "TaskComplexBeInGroup.h"
#include "Tasks/TaskTypes/TaskComplexBeCop.h"
#include "TaskComplexAvoidOtherPedWhileWandering.h"
#include "TaskComplexAvoidEntity.h"
#include "TaskComplexArrestPed.h"
#include "TaskComplexEnterAnyCarAsDriver.h"
#include "TaskSimpleSetKindaStayInSamePlace.h"
#include "TaskSimpleSetCharIgnoreWeaponRangeFlag.h"
#include "Tasks/TaskTypes/TaskComplexCarDriveMission.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimpleCreateCarAndGetIn.h"
#include "TaskComplexWaitForBackup.h"
#include "TaskSimpleIKChain.h"
#include "TaskSimpleIKLookAt.h"
#include "TaskSimplePickUpBike.h"
#include "TaskSimpleIKManager.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleIKPointArm.h"
#include "TaskSimpleCarSlowDragPedOut.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"
#include "TaskGangHasslePed.h"
#include "TaskGangHassleVehicle.h"
#include "TaskGoToVehicleAndLean.h"
#include "TaskSimpleSetCharDecisionMaker.h"
#include "TaskLeanOnVehicle.h"
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "TaskSimpleTurn180.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexGangFollower.h"
#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexGetOnBoatSeat.h"
#include "TaskComplexGoToAttractor.h"
#include "TaskComplexGoToBoatSteeringWheel.h"
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskSimpleSitDown.h"
#include "TaskComplexGoToCarDoorAndStandStill.h"
#include "TaskComplexGoToPointAiming.h"
#include "TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "TaskComplexGoToPointAnyMeans.h"
#include "TaskComplexGoToPointShooting.h"
#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskComplexHitByGunResponse.h"
#include "TaskComplexHitResponse.h"
#include "TaskSimpleDrown.h"
#include "TaskComplexDieInCar.h"
#include "TaskComplexFallToDeath.h"
#include "TaskSimpleDrownInCar.h"
#include "TaskSimpleDuckToggle.h"
#include "TaskSimpleDuckWhileShotsWhizzing.h"
#include "TaskSimpleBeHitWhileMoving.h"
#include "TaskComplexPresentIDToCop.h"
#include "TaskComplexExtinguishFireOnFoot.h"
#include "TaskSimpleBeKickedOnGround.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleDieInCar.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexTrackEntity.h"
#include "TaskSimpleTired.h"
#include "TaskSimpleAffectSecondaryBehaviour.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleCarWaitToSlowDown.h"
#include "TaskSimpleCarAlign.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarGetOut.h"
#include "TaskSimpleCarOpenDoorFromOutside.h"
#include "TaskSimpleCarJumpOut.h"
#include "TaskSimpleCarCloseDoorFromInside.h"
#include "TaskSimpleClearLookAt.h"
#include "TaskComplexFleePoint.h"
#include "TaskSimpleWaitUntilAreaCodesMatch.h"
#include "TaskSimplePlayerOnFire.h"
#include "TaskComplexOnFire.h"
#include "TaskSimpleBikeJacked.h"
#include "TaskSimpleRunAnimLoopedMiddle.h" // lote 202A (sin validar)
#include "TaskSimpleRunTimedAnim.h"
#include "TaskSimpleSay.h"
#include "TaskComplexLeaveCarAndDie.h"
#include "TaskSimpleHandsUp.h"
#include "TaskComplexSmartFleePoint.h"
#include "Interior/TaskInteriorBeInHouse.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.h"

// Interior Tasks
#include "Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h"

#include "EventSeenPanickedPed.h"
#include "EventCarUpsideDown.h"
#include "EventPassObject.h"
#include "EventLeanOnVehicle.h"
#include "EventCreatePartnerTask.h"
#include "EventOnFire.h"
#include "EventEscalator.h"
#include "EventCopCarBeingStolen.h"
#include "EventDanger.h"
#include "EventSeenCop.h"
#include "EventHealthLow.h"
#include "EventHealthReallyLow.h"
#include "EventLowAngerAtPlayer.h"
#include "EventHighAngerAtPlayer.h"
#include "EventScriptedAttractor.h"
#include "EventStuckInAir.h"
#include "EventInteriorUseInfo.h"
#include "EventDontJoinPlayerGroup.h"
#include "EventNewGangMember.h"
#include "EventAcquaintancePedHate.h"
#include "EventAcquaintancePedRespect.h"
#include "EventAcquaintancePedLike.h"
#include "EventAcquaintancePedDislike.h"
#include "EventAcquaintancePedHateBadlyLit.h"
#include "EventPedCollisionWithPed.h"
#include "EventPedCollisionWithPlayer.h"
#include "EventPlayerCollisionWithPed.h"
#include "EventObjectCollision.h"
#include "EventBuildingCollision.h"
#include "EventLeaderEnteredCarAsDriver.h"
#include "EventLeaderExitedCarAsDriver.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"
#include "EventAreaCodes.h"
#include "EventLeaderEntryExit.h"
#include "EventSpecial.h"
#include "Formation.h"
#include "PointList.h"
#include "ProjectileInfo.h"

#include "Plugins/BreakablePlugin/BreakablePlugin.h"

#include "platform/win/VideoPlayer/VideoPlayer.h"
#include "platform/win/VideoMode.h"
#include "platform/win/WinPlatform.h"
#include "platform/win/DelayLoadThunks.h"
#include "platform/win/ImportThunks.h"

#include "app/app.h"
#include <RealTimeShadowManager.h>

#include "extensions/utility.hpp"
#include "extensions/CommandLine.h"
#include <RenderBuffer.hpp>

#include "reversiblehooks/RootHookCategory.h"

#include "WindowedMode.hpp"
#include "game_sa/Crt/CrtQsort.h"
#include "game_sa/Crt/CrtString.h"
#include "game_sa/Crt/CrtHeap.h"

// Hooks de biblioteca del grupo g25b (RenderWare SDK / controlador D3D9); el sufijo evita colisiones con otros registradores.
void RwCoreInjectHooks25b();
void RpWorldInjectHooks25b();
void RpWorldInjectHooks227A();
void RpWorldInjectHooks228A(); // rpworld.cpp (lote 228A)
void RpWorldInjectHooks229A();
void RpWorldInjectHooks230A();
void ExeLibJpegInjectHooks232A(); // JPegCompress.cpp (lote 232A, libjpeg 6b del EXE)
void RpAnisotInjectHooks();
void CMatrixInjectHooksLote101A(); // Matrix.cpp (lote 101A)
void CMatrixInjectHooksLote103A(); // Matrix.cpp (lote 103A)
void CMatrixInjectHooksLote105A(); // Matrix.cpp (lote 105A)
void CMatrixInjectHooksLote166A(); // Matrix.cpp (lote 166A)
void CColPointInjectHooksLote105A(); // ColPoint.cpp (lote 105A)
void tColLightingInjectHooksLote116A(); // ColPoint.cpp (lote 116A)
void CompressedVectorInjectHooksLote105A(); // CompressedVector.cpp (lote 105A)
void TListInjectHooksLote105A(); // List_c.cpp (lote 105A)
void ListItemInjectHooksLote218A(); // ListItem_c.cpp (lote 218A, sin validar)
void RuntimeHelpersInjectHooksLote105A(); // RuntimeHelpers.cpp (lote 105A)
void CZoneInjectHooksLote124A(); // Zone.cpp (lote 124A)
void CPedScannerInjectHooksLote109A(); // PedScanner.cpp (lote 109A)
void CVehicleScannerInjectHooksLote109A(); // VehicleScanner.cpp (lote 109A)
void CPointRouteInjectHooksLote109A(); // PedGeometryAnalyser.cpp (lote 109A)

// Lote R14A (sin validar): se declaran aqui (no en RenderWare.h) para no recompilar todas las unidades por el encabezado precompilado.
void RwD3D9InjectHooksR14A();    // 0x801390: imagen -> raster del controlador D3D9 (rwcore.cpp)
void RwPlCoreInjectHooksR14A();  // listas libres, memoria, ficheros, registro de plugins, error y arena de recursos (rwplcore.cpp)

// Lote R15A (sin validar).
void RwPlCoreInjectHooksR15A();  // registro de plugins (datos de flujo), SList, cadenas del motor y liberacion en el arena (rwplcore.cpp)
void RwCoreInjectHooksR15A();    // tuberias de Rx: Im3D por defecto y fases finales del desbloqueo (rwcore.cpp)

// Lote R16A (sin validar).
void RpWorldInjectHooksR16A();    // plugin 0x509 (mundo y ties), mallas 0x50E/0x510, luces, camaras, clumps y pesos de vertices D3D9 (rpworld.cpp)
void RpWorldInjectHooksR17A();    // vertices D3D9 (instanciar, leer), formato de texcoord, plugin y estado de luces, utilidades de indices, recorte y callbacks AllInOne (rpworld.cpp)
void RpWorldInjectHooksR19A();    // generador de tiras por defecto de RpBuildMesh (rpworld.cpp)
void RpWorldInjectHooksR20A();    // union de tiras, listas de triangulos, cache de vertex shaders D3D9, tuberias, plugin 0x1F y flujo nativo de geometrias (rpworld.cpp)
void RpWorldInjectHooksR21A();    // matrices del vertex shader D3D9 del modulo de mundos y flujo nativo (D3D9) de los sectores (rpworld.cpp)
void RpWorldInjectHooksR18A();    // plugin de geometria D3D9, callback de instanciado de atomos, mallas RpBuildMesh/RpMeshHeader y accesores (rpworld.cpp)

// Lote R23A (sin validar).
void RwPlCoreInjectHooksR23A();   // RwV3dNormalize (0x7ED9B0) y RwV3dTransformPoints (0x7EDD90) (rwplcore.cpp)
void RwCoreInjectHooksR23A();     // camaras (0x7EE1D0..0x7EF1F0) e Im3D (0x7EF450..0x7EF6B0) (rwcore.cpp)
void RwPlCoreInjectHooksR24A();   // matrices: RwMatrixUpdate, RwMatrixMultiply, RwMatrixDestroy y _rwMatrixSetMultFn (rwplcore.cpp)
void RwCoreInjectHooksR24A();     // tuberias Im3D, RwFrame y callbacks/modos de RwTexture (0x7EFAC0..0x7F35D0) (rwcore.cpp)
void RwPlCoreInjectHooksR25A();   // _rwSqrt (0x7EDB30), RwErrorSet (0x808820) y RwBBoxCalculate (0x808F60) (rwplcore.cpp)
void RwCoreInjectHooksR25A();     // diccionarios y texturas, lectura de imagenes, RwFrameUpdateObjects, _rwObjectHasFrameSetFrame y RxPipelineExecute (rwcore.cpp)
void RwD3D9InjectHooksR25A();     // controlador D3D9: gestor de buferes de vertices, render target, caches de estado, multimuestreo, CreateVertexShader y caja en frustum (rwcore.cpp)
void RwD3D9InjectHooksR26A();     // controlador D3D9: parametros de presentacion, parada, estandares, modos de video, liberacion/restauracion del dispositivo, multimuestreo, cambio de modo y camara con ventana (rwcore.cpp)
void RwD3D9InjectHooksR27A();     // controlador D3D9: soporte DXT, caches de estado, render target, declaracion, shaders, flujos, indices, dibujo, transformaciones y luces (rwcore.cpp)
void RwCoreInjectHooksR28A();      // rasters de RwCore (baraster.c): crear, destruir, contexto, bloqueo, niveles, dibujo y mostrar (rwcore.cpp)
void RwD3D9InjectHooksR28A();     // controlador D3D9: luces, declaraciones, Im2D, caches de estados, material, reinicio de estados, RwD3D9SetTexture y formato de imagen (rwcore.cpp)
void RwCoreInjectHooksR29A();      // imagenes de RwCore (baimage.c): crear, destruir, reservar y liberar pixeles y mascara (rwcore.cpp)
void RwD3D9InjectHooksR29A();     // controlador D3D9: imagen -> raster (0x8001E0), conversores RGBA -> pixel y raster de 4/8 bits o luminancia (rwcore.cpp)
// Lote R30A (sin validar).
void RwPlCoreInjectHooksR30A();  // RwOsGetFileInterface (0x804130) (rwplcore.cpp)
void RwCoreInjectHooksR30A();    // imagenes (mascara, lectura, gamma, copia), RwImageSetFromRaster, flujos de textura y cierre de Rx (0x802AF0..0x804F60) (rwcore.cpp)
// Lote R31A (sin validar).
void RwCoreInjectHooksR31A();    // tuberias de Rx: apertura, nodos, almacen temporal, bloqueo, desbloqueo y clonado de definiciones (0x804FE0..0x806990) (rwcore.cpp)
void RwCoreInjectHooksR32A();    // tuberias de Rx (busqueda y alta de nodos), listas de marcos y camaras en flujos, marcos sucios, monton de Rx, Im3D por defecto y cuantificador de paleta (0x806B30..0x80AA80) (rwcore.cpp)
void RwCoreInjectHooksR33A();    // cuantificador de paleta (arbol, cortes, indices) y reescalado de imagenes (0x80AD90..0x80CD10) (rwcore.cpp)
void RwPlCoreInjectHooksR33A();  // transformaciones SSE de puntos/vectores y producto de matrices (0x80CE10..0x80D370) (rwplcore.cpp)
void RwPlCoreInjectHooksR34A();  // transformaciones SSE2 de puntos/vectores con prueba de alineacion (0x80D440..0x80D600) (rwplcore.cpp)
void RwD3D9InjectHooksR34A();    // deteccion de CPU, Im3D de D3D9 y nodos de Rx de D3D9 (0x80D680..0x80E9A0) (rwcore.cpp)
void RwCoreInjectHooksR34A();    // vector de estados de render por defecto, paquetes y asignacion de clusters de Rx (0x80E9B0..0x80F690) (rwcore.cpp)

void InjectHooksMain() {
    /**
    * We have `NOTSA_STANDALONE` macro to be able to dump all hooks without actually writing to memory,
    * it is used by the CI to automatically update the docs.
    * So don't do anything game related here if that macro is set.
    **/

    #ifndef NOTSA_STANDALONE
        // Render2dStuff se instala solo por su hook reversible: un segundo salto permanente causa un
        // bucle EXE/DLL cuando se desactiva. UpdatePads conserva aquí su instalación existente.
        HookInstall(0x541DD0, CPad::UpdatePads);

        #ifdef NOTSA_WINDOWED_MODE
            notsa::InjectWindowedModeHooks();
        #endif
    #endif

    CDoor::InjectHooks();
    CControllerConfigManager::InjectHooks();
    CFormation::InjectHooks();
    CHandShaker::InjectHooks();
    CCutsceneMgr::InjectHooks();
    CFileMgr::InjectHooks();
    CPedGroupPlacer::InjectHooks();
    CLoadedCarGroup::InjectHooks();
    RenderBuffer::InjectHooks();
    CStaticShadow::InjectHooks();
    CPedGroup::InjectHooks();
    CPedGroupMembership::InjectHooks();
    CRealTimeShadowManager::InjectHooks();
    CRealTimeShadow::InjectHooks();
    CPopCycle::InjectHooks();
    ProcObjectListItem::InjectHooks();
    ProcObjectMan_c::InjectHooks();
    ProcSurfaceInfo_c::InjectHooks();
    RwHelperInjectHooks();
    RwCoreInjectHooks();
    RwPlCoreInjectHooks();
    CPad::InjectHooks();
    CRGBA::InjectHooks();
    D3DResourceSystem::InjectHooks();
    D3DTextureBuffer::InjectHooks();
    D3DIndexDataBuffer::InjectHooks();
    RwCoreInjectHooks25d();
    RwPlCoreInjectHooks25d();
    InjectCommonHooks();
    CEscalator::InjectHooks();
    CShadowCamera::InjectHooks();
    CInterestingEvents::InjectHooks();
    CShadows::InjectHooks();
    RwCoreInjectHooks25b();
    RpWorldInjectHooks25b();
    RpWorldInjectHooks227A();
    RpWorldInjectHooks228A();
    RpWorldInjectHooks229A();
    RpWorldInjectHooks230A();
    ExeLibJpegInjectHooks232A();
    RpAnisotInjectHooks();
    LibJpegCompress::InjectHooks();        // lote 231A (sin validar)
    CCam::InjectHooks();
    CCarAI::InjectHooks();
    CMenuSystem::InjectHooks();
    CCarFXRenderer::InjectHooks();
    CPedAttractorManager::InjectHooks();
    CPedAttractor::InjectHooks();
    CPedShelterAttractor::InjectHooks();
    CPedATMAttractor::InjectHooks();
    CProtectionStub::InjectHooks();
    RwD3D9RasterDriver::InjectHooks();     // lote 224A (sin validar)
    LibJpegCompressEncode::InjectHooks();  // lote 230A (sin validar)
    LibJpegDecompress::InjectHooks();      // lote 233A (sin validar)
    LibJpegMemory::InjectHooks();                 // lote 235A (sin validar)
    LibJpegError::InjectHooks();                  // lote 235A (sin validar)
    LibJpegDecompressUpsample::InjectHooks();     // lote 235A (sin validar)
    LibJpegCompressApi::InjectHooks();            // lote 235A (sin validar)
    LibJpegDecompressApi::InjectHooks();   // lote 236A (sin validar)
    D3D9RasterFormat::InjectHooks();       // lote 225A (sin validar)
    CPedSeatAttractor::InjectHooks();
    CPedStopAttractor::InjectHooks();
    CPedPizzaAttractor::InjectHooks();
    CPedTriggerScriptAttractor::InjectHooks();
    CPedLookAtAttractor::InjectHooks();    // lote 221A (sin validar)
    CPedScriptedAttractor::InjectHooks();  // lote 221A (sin validar)
    CPedParkAttractor::InjectHooks();      // lote 221A (sin validar)
    CPedStepAttractor::InjectHooks();      // lote 221A (sin validar)
    ExeProtectionStub::InjectHooks();      // lote 221A (sin validar)
    CPedAttractorPedPlacer::InjectHooks();
    BoneNode_c::InjectHooks();
    BoneNodeManager_c::InjectHooks();
    IKChainManager_c::InjectHooks();
    IKChain_c::InjectHooks();
    CCheckpoint::InjectHooks();
    CCheckpoints::InjectHooks();
    CWeaponEffects::InjectHooks();
    CPedList::InjectHooks();
    CPointList::InjectHooks();
    CBouncingPanel::InjectHooks();
    CRope::InjectHooks();
    CRopes::InjectHooks();
    CWeaponInfo::InjectHooks();
    CCurrentVehicle::InjectHooks();
    CCurves::InjectHooks();
    CPlaceName::InjectHooks();
    CUserDisplay::InjectHooks();
    COnscreenTimer::InjectHooks();
    COnscreenCounterEntry::InjectHooks();
    CPedPlacement::InjectHooks();
    CWindModifiers::InjectHooks();
    CGenericGameStorage::InjectHooks();
    CVehicleSaveStructure::InjectHooks();
    C_PcSave::InjectHooks();
    CFileLoader::InjectHooks();
    CWorld::InjectHooks();
    CSector::InjectHooks();
    CStreamingInfo::InjectHooks();
    CStreaming::InjectHooks();
    InjectCdStreamHooks();
    CMirrors::InjectHooks();
    CFire::InjectHooks();
    CExplosion::InjectHooks();
    CClothesBuilder::InjectHooks();
    COctTree::InjectHooks();     // lote 203A (sin validar)
    COctTreeBase::InjectHooks(); // lote 203A (sin validar)
    CClothes::InjectHooks();
    CBulletInfo::InjectHooks();
    CRestart::InjectHooks();
    CSetPiece::InjectHooks();
    CSetPieces::InjectHooks();
    CCopPed::InjectHooks();
    CCivilianPed::InjectHooks();
    CEmergencyPed::InjectHooks();
    CDamageManager::InjectHooks();
    CCreepingFire::InjectHooks();
    BreakManager_c::InjectHooks();
    BreakObject_c::InjectHooks();
    CFireManager::InjectHooks();
    CGroupEventHandler::InjectHooks();
    CVehicleRecording::InjectHooks();
    CBrightLights::InjectHooks();
    CShinyTexts::InjectHooks();
    CPedTaskPair::InjectHooks();
    CDirectory::InjectHooks();
    CBulletTraces::InjectHooks();
    CDebug::InjectHooks();
    CGame::InjectHooks();
    CControllerState::InjectHooks();
    CKeyboardState::InjectHooks();
    CMouseControllerState::InjectHooks();
    CRect::InjectHooks();

    /******* Don't hook these, performance is bad ********
    /* CVector::InjectHooks();
    /* CVector2D::InjectHooks();
    /* CQuaternion::Get remains disabled; selective Set hooks are installed below.
    /* CMatrix::InjectHooks();
    /****************************************************/

    CVector::InjectHooksExactMath(); // Only Magnitude 0x4082C0, Magnitude2D 0x406D50, Normalise 0x59C910 and NormaliseAndMag 0x59C970.
    CVector::InjectHooksExactOperators(); // Only DotProduct 0x40FDB0, CrossProduct 0x59C730, Multiply3x3 0x59C790, +/-/scalar products 0x40FE30..0x40FEC0, += 0x411A00 and *= 0x40FEF0.
    CQuaternion::InjectHooks(); // Only the reconstructed Set dependencies.
    CMatrix::InjectHooksMultiplyVector(); // Only operator*(CMatrix, CVector) 0x59C890.
    CMatrix::InjectHooksAttachAssign(); // Only Attach 0x59BD10 and operator= 0x59BBC0.
    CSimpleTransform::InjectHooks(); // Only UpdateRwMatrix 0x54EF40 and UpdateMatrix 0x54F1B0.
    CMatrix::InjectHooksExactOps(); // Only UpdateRW 0x59BBB0, UpdateRwMatrix 0x59AD70, SetTranslate 0x59AF40, SetRotateZOnly 0x59B020, destructor 0x59ACD0 and constructors 0x59BCF0 / 0x59C050.
    CMatrix::InjectHooksLote99A(); // Only SetUnity 0x59AE70, ResetOrientation 0x59AEA0, SetScale 0x59AED0 / 0x59AF00, SetTranslateOnly 0x59AF80, SetRotate xyz 0x59B120, Invert 0x59B920 and operator*(matrix, matrix) 0x59BE30.
    CMatrixInjectHooksLote101A(); // Only SetRotateX/Y/Z 0x59B060/0x59B0A0/0x59B0E0, RotateX/Y/Z 0x59B1E0/0x59B2C0/0x59B390, Reorthogonalise 0x59B6A0, CopyToRwMatrix 0x59B8B0 and Invert(in) 0x59BDD0.
    CMatrixInjectHooksLote103A(); // Only SetRotateXOnly 0x59AFA0, SetRotateYOnly 0x59AFE0, Rotate 0x59B460, ForceUpVector 0x59B7E0, SetRotate(quat) 0x59BBF0 and operator+ 0x59BFA0.
    CMatrixInjectHooksLote105A(); // Only operator*= 0x411A80.
    CMatrixInjectHooksLote166A(); // Only AddTranslation 0x46AE00.
    CMatrix::InjectHooksLote162A(); // Only Detach 0x59ACF0, CopyOnlyMatrix 0x59ADD0, Update 0x59BB60, UpdateMatrix 0x59AD20, operator+= 0x59ADF0 and Scale 0x459350.
    CMatrix::InjectHooksLote202A(); // Only ConvertToEulerAngles 0x59A840 and ConvertFromEulerAngles 0x59AA40 (lote 202A, sin validar).
    CVector2D::InjectHooksLote162A(); // Only operator= 0x43E110.
    CColPointInjectHooksLote105A(); // Only CColPoint::operator= 0x40FC80.
    tColLightingInjectHooksLote116A(); // Only tColLighting::GetCurrentLighting 0x59F0C0.
    CompressedVectorInjectHooksLote105A(); // Only the CompressedVector to CVector conversion 0x411590.
    TListInjectHooksLote105A(); // Only TList_c AddItem 0x4A8DF0, RemoveAll 0x4A8EB0 and GetNumItems 0x4A8EC0.
    ListItemInjectHooksLote218A(); // Only ListItem_c constructor 0x4A8DB0 (lote 218A, sin validar).
    RuntimeHelpersInjectHooksLote105A(); // Only the vector constructor iterator 0x403D60.
    CPedScannerInjectHooksLote109A(); // Only CPedScanner::GetClosestPedInRange 0x5FFF20.
    CVehicleScannerInjectHooksLote109A(); // Only CVehicleScanner::GetClosestVehicleInRange 0x5FFEE0 y (lote 198A, sin validar) ScanForVehiclesInRange 0x5FFE60.
    CPointRouteInjectHooksLote109A(); // Only CPointRoute operator new 0x41B7C0 and operator delete 0x41B7D0.
    CMatrixLink::InjectHooks();
    CMatrixLinkList::InjectHooks();
    CEntryInfoNode::InjectHooks();
    CEntryInfoList::InjectHooks();

    //CPtrList::InjectHooks();

    //CPtrNodeDoubleLink::InjectHooks();
    //CPtrListDoubleLink::InjectHooks();
    //
    //CPtrNodeSingleLink::InjectHooks();
    //CPtrListSingleLink::InjectHooks();

    CReferences::InjectHooks();
    CPopulation::InjectHooks();
    CMemoryMgr::InjectHooks();
    CMemoryHeap::InjectHooks();
    CModelInfo::InjectHooks();
    CModelInfoAccelerator::InjectHooks(); // lote 194A (sin validar)
    CBaseModelInfo::InjectHooks();
    CAtomicModelInfo::InjectHooks();
    CLodAtomicModelInfo::InjectHooks();
    CVehicleModelInfo::InjectHooks();
    CClumpModelInfo::InjectHooks();
    CTimeModelInfo::InjectHooks();
    CLodTimeModelInfo::InjectHooks();
    CDamageAtomicModelInfo::InjectHooks();
    CWeaponModelInfo::InjectHooks();
    CShotInfo::InjectHooks();
    CPedModelInfo::InjectHooks();
    CTimeInfo::InjectHooks();
    SurfaceInfos_c::InjectHooks();
    CPlaceable::InjectHooks();
    CEntity::InjectHooks();
    CPhysical::InjectHooks();
    CRenderer::InjectHooks();
    CWorldScan::InjectHooks();
    CDummy::InjectHooks();
    CDummyObject::InjectHooks();
    CDummyPed::InjectHooks();
    CObject::InjectHooks();
    CProjectile::InjectHooks();
    CObjectData::InjectHooks();
    CObjectSaveStructure::InjectHooks();
    CCutsceneObject::InjectHooks();
    CHandObject::InjectHooks();
    CBuilding::InjectHooks();
    CAnimatedBuilding::InjectHooks();
    CCompressedMatrixNotAligned::InjectHooks();
    CVisibilityPlugins::InjectHooks();
    CPed::InjectHooks();
    CPedSaveStructure::InjectHooks();
    CAttractorScanner::InjectHooks();
    CObjectScanner::InjectHooks(); // lote 198A (sin validar): solo CObjectScanner::ScanForObjectsInRange 0x5FFF30
    CMentalState::InjectHooks();
    CEventScanner::InjectHooks();
    CVehiclePotentialCollisionScanner::InjectHooks();
    CObjectPotentialCollisionScanner::InjectHooks();
    CPedAcquaintanceScanner::InjectHooks();
    CSexyPedScanner::InjectHooks();
    CNearbyFireScanner::InjectHooks();
    CPedStuckChecker::InjectHooks();
    CCollisionEventScanner::InjectHooks();
    CEventSource::InjectHooks();
    CPedIntelligence::InjectHooks();
    CPedGroupIntelligence::InjectHooks();
    CCollision::InjectHooks();
    CTempColModels::InjectHooks();
    CSphere::InjectHooks();
    CColSphere::InjectHooks();
    CColDisk::InjectHooks();
    CColLine::InjectHooks();
    CColTrianglePlane::InjectHooks();
    CBox::InjectHooks();
    CBoundingBox::InjectHooks(); // Lote 116A: IsPointWithin 0x49C300.
    CColBox::InjectHooks(); // Lote 116A: Set 0x40EE70 y operator= 0x40EEC0.
    CColModel::InjectHooks();
    CCollisionData::InjectHooks();
    CQuadTreeNode<void*>::InjectHooks();
    CColStore::InjectHooks();
    CColAccel::InjectHooks();
    CTrafficLights::InjectHooks();
    CMotionBlurStreaks::InjectHooks();
    CRegisteredMotionBlurStreak::InjectHooks();
    CCover::InjectHooks();
    CCoverPoint::InjectHooks();
    CVehicleAnimGroup::InjectHooks();
    CVehicleAnimGroupData::InjectHooks();
    CPedDamageResponseCalculator::InjectHooks();
    CPedScriptedTaskRecord::InjectHooks();
    CPedScriptedTaskRecordData::InjectHooks();
    CPedClothesDesc::InjectHooks();
    CScriptedBrainTaskStore::InjectHooks();
    CTimer::InjectHooks();
    CTagManager::InjectHooks();
    CWaterLevel::InjectHooks();
    cBuoyancy::InjectHooks();
    CWanted::InjectHooks();
    CCrime::InjectHooks();
    CEscalators::InjectHooks();
    CWeapon::InjectHooks();
    cTransmission::InjectHooks();
    CPlayerPed::InjectHooks();
    CPlayerSkin::InjectHooks();
    CStats::InjectHooks();
    CCarCtrl::InjectHooks();
    CTheZones::InjectHooks();
    CZoneInjectHooksLote124A(); // Only CZone::GetTranslatedName 0x5720C0.
    CMenuManager::InjectHooks();
    CSprite2d::InjectHooks();
    CCheat::InjectHooks();
    CBirds::InjectHooks();
    WaterCreature_c::InjectHooks();
    WaterCreatureManager_c::InjectHooks();
    C2dEffect::InjectHooks();
    CCustomRoadsignMgr::InjectHooks();
    COcclusion::InjectHooks();
    COccluder::InjectHooks();
    CActiveOccluder::InjectHooks();
    CGarage::InjectHooks();
    CStoredCar::InjectHooks();
    CSaveGarage::InjectHooks();
    CGarages::InjectHooks();
    CMissionCleanup::InjectHooks();
    CRemote::InjectHooks();
    CPostEffects::InjectHooks();
    CIniFile::InjectHooks();
    CStencilShadowObject::InjectHooks();
    CStencilShadows::InjectHooks();
    CFont::InjectHooks();
    CGameLogic::InjectHooks();
    CStuckCarCheck::InjectHooks();
    CUpsideDownCarCheck::InjectHooks();
    CCamera::InjectHooks();
    CIdleCam::InjectHooks();
    CMessages::InjectHooks();
    CGangWars::InjectHooks();
    CGangWarsSaveStructure::InjectHooks(); // lote 196A (sin validar): Construct 0x5D2620 y Extract 0x5D2740
    CPlayerPedData::InjectHooks();
    CTimeCycle::InjectHooks();
    CColourSet::InjectHooks();
    CSkidmarks::InjectHooks();
    CMovingThings::InjectHooks();
    CRoadBlocks::InjectHooks();
    CBridge::InjectHooks();
    CCranes::InjectHooks();
    CGridRef::InjectHooks();
    CShopping::InjectHooks();
    CInformFriendsEventQueue::InjectHooks();
    C3dMarkers::InjectHooks();
    C3dMarker::InjectHooks();
    CSpecialFX::InjectHooks();
    CFallingGlassPane::InjectHooks();
    CGlass::InjectHooks();
    CPedGroups::InjectHooks();
    CPointLights::InjectHooks();
    CClock::InjectHooks();
    CClouds::InjectHooks();
    CCoronas::InjectHooks();
    CRegisteredCorona::InjectHooks();
    CCustomCarPlateMgr::InjectHooks();
    CDraw::InjectHooks();
    CEntryExitManager::InjectHooks();
    CEntryExit::InjectHooks();
    CInformGroupEvent::InjectHooks();
    CInformGroupEventQueue::InjectHooks();
    CGangs::InjectHooks();
    CPlayerInfo::InjectHooks();
    CPlayerInfoSaveStructure::InjectHooks(); // lote 196A (sin validar): Construct 0x5D24B0 y Extract 0x5D2560
    CReplay::InjectHooks();
    CWinsockLink::InjectHooks();
    CDarkel::InjectHooks();
    CGeneral::InjectHooks();
    CMaths::InjectHooks();
    CGamma::InjectHooks();
    CHudColours::InjectHooks();
    CHud::InjectHooks();
    CCredits::InjectHooks();
    CTxdStore::InjectHooks();
    CCarEnterExit::InjectHooks();
    CEntityScanner::InjectHooks();
    CPedType::InjectHooks();
    CAcquaintance::InjectHooks();
    CWeather::InjectHooks();
    CPathFind::InjectHooks();
    CPathNode::InjectHooks();
    CCarPathLink::InjectHooks();
    CNodeAddress::InjectHooks();
    CNodeRoute::InjectHooks();
    CPatrolRoute::InjectHooks(); // lote 146A
    CAutoPilot::InjectHooks();
    CLoadMonitor::InjectHooks();
    CDecisionMakerTypes::InjectHooks();
    CDecisionMaker::InjectHooks();
    CPlayerRelationshipRecorder::InjectHooks();
    CDecisionMakerTypesFileLoader::InjectHooks();
    CDecision::InjectHooks();
    CPedStats::InjectHooks();
    CCarGenerator::InjectHooks();
    CTheCarGenerators::InjectHooks();
    CCullZones::InjectHooks();
    CSpecialPlateHandler::InjectHooks();
    CKeyGen::InjectHooks();
    CAccidentManager::InjectHooks();
    CStuntJumpManager::InjectHooks();
    CLocalisation::InjectHooks();
    CSimpleVariablesSaveStructure::InjectHooks();
    CPedGeometryAnalyser::InjectHooks();

    NodeNamePlugin::InjectHooks();
    JPegPlugin::InjectHooks();
    PipelinePlugin::InjectHooks();
    RpAnimBlendPlugin::InjectHooks();
    CCollisionPlugin::InjectHooks();
    BreakablePlugin::InjectHooks();
    RtAnim::InjectHooks();
    RwCoreFrameCameraInjectHooks();
    RwPlCoreEngineStreamInjectHooks();
    RpWorldInjectHooks();
    RpHAnimInjectHooks();
    RpSkinInjectHooks();
    RtBMPInjectHooks();
    RpUVAnimInjectHooks();
    RtDictInjectHooks();
    RwD3D9InjectHooksR1A();
    RwD3D9InjectHooksR6A();
    RwD3D9InjectHooksR11A2();
    RwCoreInjectHooksR7A();
    RwCoreInjectHooksR11A();
    RwCoreInjectHooksR8A();
    RwPlCoreInjectHooksR8A();
    RwPlCoreInjectHooksR12A();
    RwPlCoreInjectHooksR13A();
    RwPlCoreInjectHooksR14A();
    RwD3D9InjectHooksR14A();
    RwPlCoreInjectHooksR15A();
    RwCoreInjectHooksR15A();
    RpWorldInjectHooksR16A();
    RpWorldInjectHooksR17A();
    RpWorldInjectHooksR18A();
    RpWorldInjectHooksR19A();
    RpWorldInjectHooksR20A();
    RpWorldInjectHooksR21A();
    RwPlCoreInjectHooksR23A();
    RwCoreInjectHooksR23A();
    RwPlCoreInjectHooksR24A();
    RwCoreInjectHooksR24A();
    RwPlCoreInjectHooksR25A();
    RwCoreInjectHooksR25A();
    RwD3D9InjectHooksR25A();
    RwD3D9InjectHooksR26A();
    RwD3D9InjectHooksR27A();
    RwCoreInjectHooksR28A();
    RwD3D9InjectHooksR28A();
    RwCoreInjectHooksR29A();
    RwD3D9InjectHooksR29A();
    RwPlCoreInjectHooksR30A();
    RwCoreInjectHooksR30A();
    RwCoreInjectHooksR31A();
    RwCoreInjectHooksR32A();
    RwCoreInjectHooksR33A();
    RwPlCoreInjectHooksR33A();
    RwPlCoreInjectHooksR34A();
    RwD3D9InjectHooksR34A();
    RwCoreInjectHooksR34A();
    RpMatFXInjectHooks();
    DelayLoadThunksInjectHooks();
    ImportThunksInjectHooks();
    RtSlerpInjectHooks();
    D3DX9InjectHooks();
    RtQuatInjectHooks();
    ZlibInflateInjectHooks();
    CrtQsortInjectHooks();   // lote CRT1A: qsort del CRT del EXE (0x8247E0), validado fuera del juego (reversing/FnCRT1A)
    CrtStringInjectHooks();  // lote CRT1A: strlen/strcmp/strncmp/strcpy/strcat/strncpy/strncat/strstr del CRT del EXE
    CrtHeapInjectHooks();    // lote CRT1A: malloc/free/realloc/calloc/operator new; SOLO con GTASA_CRT_HEAP_HOOKS (sin prueba dentro del juego)

    CIplStore::InjectHooks();
    cHandlingDataMgr::InjectHooks();
    CLoadingScreen::InjectHooks();
    CRadar::InjectHooks();
    CText::InjectHooks();
    ModelIndices::InjectHooks();
    CWaterCannons::InjectHooks();
    CWaterCannon::InjectHooks();
    CSprite::InjectHooks();
    CPlaneTrail::InjectHooks();
    CPlaneTrails::InjectHooks();
    CMBlur::InjectHooks();
    CRubbish::InjectHooks();
    CPlaneBanners::InjectHooks();
    CPickup::InjectHooks();
    CPickups::InjectHooks();
    CPhoneInfo::InjectHooks();
    CPedIK::InjectHooks();

    CCustomBuildingPipeline::InjectHooks();
    CCustomBuildingRenderer::InjectHooks();
    CCustomBuildingDNPipeline::InjectHooks();
    CCustomCarEnvMapPipeline::InjectHooks();
    CConversationForPed::InjectHooks();
    CConversationNode::InjectHooks();
    CPedToPlayerConversations::InjectHooks();
    CConversations::InjectHooks();
    CProjectileInfo::InjectHooks();

    const auto Pools = [] {
        CPools::InjectHooks();
        CIplDefPool::InjectHooks();
        CPedPool::InjectHooks();
        CVehiclePool::InjectHooks();
        CBuildingPool::InjectHooks();
        CObjectPool::InjectHooks();
        CDummyPool::InjectHooks();
        CColModelPool::InjectHooks();
        CTaskPool::InjectHooks();
        CPedIntelligencePool::InjectHooks();
        CPtrNodeSingleLinkPool::InjectHooks();
        CPtrNodeDoubleLinkPool::InjectHooks();
        CEntryInfoNodePool::InjectHooks();
        CPointRoutePool::InjectHooks();
        CPatrolRoutePool::InjectHooks();
        CEventPool::InjectHooks();
        CNodeRoutePool::InjectHooks();
        CTaskAllocatorPool::InjectHooks();
        CPedAttractorPool::InjectHooks();
    };
    Pools();

    const auto Audio = []() {
        // General
        CAEAudioEnvironment::InjectHooks();
        CAEAudioUtility::InjectHooks();
        CAESmoothFadeThread::InjectHooks();
        CAESound::InjectHooks();
        CAEStreamThread::InjectHooks();
        CAudioEngine::InjectHooks();
        CAudioZones::InjectHooks();

        // Entities
        CAEAudioEntity::InjectHooks();
        CAECollisionAudioEntity::InjectHooks();
        CAEDoorAudioEntity::InjectHooks();
        CAEExplosionAudioEntity::InjectHooks();
        CAEFireAudioEntity::InjectHooks();
        CAEFrontendAudioEntity::InjectHooks();
        CAEGlobalWeaponAudioEntity::InjectHooks();
        CAEPedAudioEntity::InjectHooks();
        CAEPedlessSpeechAudioEntity::InjectHooks();
        CAEPedSpeechAudioEntity::InjectHooks();
        // CAEPedWeaponAudioEntity ahora registra sus propios métodos (Initialise, Terminate,
        // AddAudioEvent, Service) con un nombre distinto al de CAEWeaponAudioEntity.
        CAEPedWeaponAudioEntity::InjectHooks();
        CAEPoliceScannerAudioEntity::InjectHooks();
        CAEScriptAudioEntity::InjectHooks();
        CAETwinLoopSoundEntity::InjectHooks();
        CAEVehicleAudioEntity::InjectHooks();
        CAEWaterCannonAudioEntity::InjectHooks();
        CAEWeaponAudioEntity::InjectHooks();
        CAEWeatherAudioEntity::InjectHooks();

        // Hardware
        CAEAudioChannel::InjectHooks();
        CAEAudioHardware::InjectHooks();
        CAEStaticChannel::InjectHooks();
        CAEStreamingChannel::InjectHooks();

        // Loaders
        CAEBankLoader::InjectHooks();
        CAEDataStream::InjectHooks();
        CAEMFDecoder::InjectHooks();
        CAEMP3BankLoader::InjectHooks();
        CAEMP3TrackLoader::InjectHooks();
        CAEStreamingDecoder::InjectHooks();
        // CAEStreamTransformer::InjectHooks(); -- injected by AEDataStream::InjectHooks.
        CAETrackLoader::InjectHooks();
        CAEVorbisDecoder::InjectHooks();
        CAEWaveDecoder::InjectHooks();
        CAEWMADecoder::InjectHooks();

        // Managers
        CAEAmbienceTrackManager::InjectHooks();
        CAECutsceneTrackManager::InjectHooks();
        CAERadioTrackManager::InjectHooks();
        CAESoundManager::InjectHooks();
        CAEUserRadioTrackManager::InjectHooks();
    };

    const auto Plant = []() {
        CPlantColEntEntry::InjectHooks();
        CPlantMgr::InjectHooks();
        CPlantLocTri::InjectHooks();
        CGrassRenderer::InjectHooks();
        CPPTriPlantBuffer::InjectHooks();
        CPlantSurfPropMgr::InjectHooks();
    };
    Plant();

    const auto Tasks = []() {
        CTask::InjectHooks();

        const auto Allocators = [] {
            const auto PedGroup = [] {
                CPedGroupDefaultTaskAllocatorFollowAnyMeans::InjectHooks();
                CPedGroupDefaultTaskAllocatorFollowLimited::InjectHooks();
                CPedGroupDefaultTaskAllocatorStandStill::InjectHooks();
                CPedGroupDefaultTaskAllocatorChat::InjectHooks();
                CPedGroupDefaultTaskAllocatorSitInLeaderCar::InjectHooks();
                CPedGroupDefaultTaskAllocatorRandom::InjectHooks();
                CPedGroupDefaultTaskAllocators::InjectHooks();
            };
            PedGroup();

            CTaskAllocator::InjectHooks();
            CTaskAllocatorKillThreatsBasic::InjectHooks();
            CTaskAllocatorKillThreatsDriveby::InjectHooks();
            CTaskAllocatorKillThreatsBasicRandomGroup::InjectHooks();
            CTaskAllocatorKillOnFoot::InjectHooks();
            CTaskAllocatorAttack::InjectHooks();
            CTaskAllocatorPlayerCommandAttack::InjectHooks();
        };
        Allocators();

        const auto Interior = [] {
            CTaskInteriorBeInHouse::InjectHooks();
            CTaskInteriorBeInOffice::InjectHooks();
            CTaskInteriorBeInShop::InjectHooks();
            CTaskInteriorGoToInfo::InjectHooks();
            CTaskInteriorLieInBed::InjectHooks();
            CTaskInteriorShopKeeper::InjectHooks();
            CTaskInteriorSitAtDesk::InjectHooks();
        CTaskInteriorSitInChair::InjectHooks();
            CTaskInteriorUseInfo::InjectHooks();
        };
        // NOTA (2026-10-01): este `Interior` no se invocaba; ninguna de las 8
        // clases CTaskInterior* tenía sus hooks instalados en tiempo de
        // ejecución, sin importar docs/hooks.csv. Al intentar invocarlo se
        // encontró que CTaskInteriorLieInBed registraba su FinishAnimCB
        // (estático, no virtual) con RH_ScopedVMTInstall, lo que abortaba el
        // proceso ("Couldn't find function [FinishAnimCB @ 0x675fc0] in
        // vtable"); corregido a RH_ScopedInstall en TaskInteriorLieInBed.cpp.
        Interior();

        CTaskComplexUseClosestFreeScriptedAttractorRun::InjectHooks();
        CTaskComplexUseClosestFreeScriptedAttractorSprint::InjectHooks();
        CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks();

        CTaskSimpleChoking::InjectHooks();
        CTaskComplexEvasiveDiveAndGetUp::InjectHooks();
        CTaskSimpleLeaveGroup::InjectHooks();
        CTaskComplexDiveFromAttachedEntityAndGetUp::InjectHooks();
        CTaskGangHasslePed::InjectHooks();
        CTaskGangHassleVehicle::InjectHooks();
        CTaskGoToVehicleAndLean::InjectHooks();
        CTaskLeanOnVehicle::InjectHooks();
        CTaskComplexCarSlowBeDraggedOut::InjectHooks();
        CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks();
        CTaskComplexDestroyCar::InjectHooks();
        CTaskComplexDestroyCarArmed::InjectHooks();
        CTaskComplexDestroyCarMelee::InjectHooks();
        CTaskComplexDieInCar::InjectHooks();
        CTaskComplexDragPedFromCar__InjectHooks();
        CTaskComplexDrivePointRoute::InjectHooks();
        CTaskComplexEnterCarAsDriverTimed::InjectHooks();
        CTaskComplexEnterCarAsPassengerTimed::InjectHooks();
        CTaskComplexEnterCarAsPassengerWait::InjectHooks();
        CTaskComplexEvasiveCower::InjectHooks();
        CTaskComplexEvasiveStep::InjectHooks();
        CTaskComplexExtinguishFireOnFoot::InjectHooks(); // lote 193A (sin validar)
        CTaskComplexExtinguishFires::InjectHooks();
        CTaskComplexFallAndStayDown::InjectHooks(); // lote 191A (sin validar)
        CTaskComplexFallToDeath::InjectHooks();
        CTaskComplexFleeAnyMeans::InjectHooks();
        CTaskComplexFleeEntity::InjectHooks();
        CTaskComplexFleePoint::InjectHooks();
        CTaskComplexFleeShooting::InjectHooks();
        CTaskComplexFollowLeaderAnyMeans::InjectHooks();
        // CTaskComplexFollowNodeRouteShooting::InjectHooks();
        CTaskComplexFollowPatrolRoute::InjectHooks(); // lote 175A: constructor 0x674930 y Clone 0x674BB0 (195A sin validar) (clase aun sin reconstruir)
        CTaskComplexFollowPointRoute::InjectHooks();
        CTaskComplexGangFollower::InjectHooks();
        CTaskComplexGangJoinRespond::InjectHooks();
        CTaskComplexGangLeader::InjectHooks();
        CTaskComplexGetOnBoatSeat::InjectHooks();
        CTaskComplexGoToAttractor::InjectHooks();
        CTaskComplexGoToBoatSteeringWheel::InjectHooks();
        CTaskComplexGoToCarDoorAndStandStill::InjectHooks();
        CTaskComplexGoToPointAiming::InjectHooks();
        CTaskComplexGoToPointAndStandStillAndAchieveHeading::InjectHooks();
        CTaskComplexGoToPointAnyMeans::InjectHooks();
        CTaskComplexGoToPointShooting::InjectHooks();
        // CTaskComplexGoToPointUntilCollisionAndStandStill::InjectHooks();
        CTaskComplexGotoDoorAndOpen::InjectHooks();
        CTaskComplexHitByGunResponse::InjectHooks();
        CTaskComplexHitResponse::InjectHooks();
        CTaskComplexInWater::InjectHooks();
        CTaskComplexInvestigateDeadPed::InjectHooks();
        CTaskComplexInvestigateDisturbance::InjectHooks();
        CTaskComplexKillAllThreats::InjectHooks(); // lote 170A: solo Clone 0x623750 (clase aun sin reconstruir)
        CTaskComplexKillCriminal::InjectHooks();

        CTaskComplexKillPedOnFootArmed::InjectHooks();
        CTaskComplexKillPedGroupOnFoot::InjectHooks();
        CTaskComplexKillPedOnFootMelee::InjectHooks();
        CTaskComplexKillPedOnFootStealth::InjectHooks();

        CTaskComplexLeaveCarAndDie::InjectHooks();
        CTaskComplexLeaveBoat::InjectHooks();
        CTaskComplexLeaveCarAndFlee::InjectHooks();
        CTaskComplexLeaveCarAsPassengerWait::InjectHooks();
        CTaskComplexLeaveCarAndWander::InjectHooks();
        CTaskComplexMoveBackAndJump::InjectHooks();
        CTaskComplexOnFire::InjectHooks();
        CTaskComplexOpenDriverDoor::InjectHooks();
        CTaskComplexOpenPassengerDoor::InjectHooks();
        CTaskComplexPassObject::InjectHooks();
        CTaskComplexPresentIDToCop::InjectHooks(); // lote 198A (sin validar): clase reconstruida (constructor, cuerpo del destructor, destructor con borrado, GetTaskType, Clone, CreateSubTask, ComputeHeading y las tres virtuales de subtareas)
        CTaskComplexReactToAttack::InjectHooks(); // lote 199A (sin validar): MakeAbortable, CreateSubTask, ControlSubTask y Clone; lote 193A (sin validar): cuerpo del destructor, destructor con borrado, CreateNextSubTask y CreateFirstSubTask
        CTaskComplexReactToGunAimedAt::InjectHooks();
        CTaskComplexRoadRage::InjectHooks();
        CTaskComplexScreamInCarThenLeave::InjectHooks();
        CTaskComplexSeekCoverUntilTargetDead::InjectHooks();

        // CTaskComplexSeekEntity<CEntitySeekPosCalculator>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorDriving>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorEntitySurface>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>::InjectHooks();
        // CTaskComplexSeekEntity<CEntitySeekPosCalculatorVehicleId>::InjectHooks();

        CTaskComplexSeekEntityRadiusAngleOffset::InjectHooks();
        CTaskComplexSeekEntityStandard::InjectHooks();
        CTaskComplexSeekEntityXYOffset::InjectHooks();
        CTaskComplexSeekEntityAiming::InjectHooks();
        CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::InjectHooks();
        CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>::InjectHooks(); // lote 194A (sin validar)

        CTaskComplexSeekEntityAnyMeansXYOffset::InjectHooks();
        CTaskComplexShuffleSeats::InjectHooks();
        CTaskComplexSignalAtPed::InjectHooks();
        CTaskComplexSitDownThenIdleThenStandUp::InjectHooks();
        CTaskComplexSmartFleeEntity::InjectHooks();
        CTaskComplexSmartFleePoint::InjectHooks();
        CTaskComplexStareAtPed::InjectHooks();
        CTaskComplexStealCar::InjectHooks();
        CTaskComplexTrackEntity::InjectHooks();
        CTaskComplexTurnToFaceEntityOrCoord::InjectHooks();
        CTaskComplexUseAttractor::InjectHooks();
        CTaskComplexUseAttractorPartner::InjectHooks();
        CTaskComplexAttractorPartnerWait::InjectHooks();
        CTaskComplexUseEffect::InjectHooks();
        CTaskComplexUseEffectRunning::InjectHooks();
        CTaskComplexUseEffectSprinting::InjectHooks();
        // CTaskComplexUseEntryExit::InjectHooks();
        CTaskComplexUsePairedAttractor::InjectHooks();
        CTaskComplexUseScriptedBrain::InjectHooks();
        CTaskComplexUseScriptedAttractor::InjectHooks();
        CTaskComplexWaitAtAttractor::InjectHooks();
        CTaskComplexWaitForBus::InjectHooks();
        CTaskComplexWaitForBackup::InjectHooks();
        CTaskComplexWaitForDryWeather::InjectHooks();
        CTaskComplexWalkAlongsidePed::InjectHooks();
        CTaskComplexWalkRoundBuildingAttempt::InjectHooks();
        CTaskComplexWalkRoundFire::InjectHooks();
        CTaskComplexWanderFlee::InjectHooks();
        CTaskSimpleAffectSecondaryBehaviour::InjectHooks();
        CTaskSimpleArrestPed__InjectHooks();
        CTaskSimpleBeHit::InjectHooks();
        CTaskSimpleBeHitWhileMoving::InjectHooks(); // lote 193A (sin validar): cuerpo del destructor y destructor con borrado
        CTaskSimpleBeKickedOnGround::InjectHooks(); // lote 192A (sin validar): cuerpo del destructor y destructor con borrado
        CTaskSimpleBikeJacked::InjectHooks();
        CTaskSimpleCarAlign::InjectHooks();
        CTaskSimpleCarCloseDoorFromInside::InjectHooks();
        CTaskSimpleCarCloseDoorFromOutside::InjectHooks();
        CTaskSimpleCarFallOut::InjectHooks();
        CTaskSimpleCarForcePedOut::InjectHooks();
        CTaskSimpleCarGetOut::InjectHooks();
        CTaskSimpleCarGetIn::InjectHooks();
        CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::InjectHooks();
        CTaskSimpleCarOpenDoorFromOutside::InjectHooks();
        CTaskSimpleCarJumpOut::InjectHooks();
        CTaskSimpleCarOpenLockedDoorFromOutside::InjectHooks();
        CTaskSimpleCarSetPedSlowDraggedOut::InjectHooks();
        CTaskSimpleCarSetTempAction::InjectHooks();
        CTaskSimpleCarShuffle::InjectHooks();
        CTaskSimpleCarSlowBeDraggedOut::InjectHooks();
        CTaskSimpleCarWaitToSlowDown::InjectHooks();
        CTaskSimpleCarWaitForDoorNotToBeInUse::InjectHooks();
        CTaskSimpleChat::InjectHooks();
        CTaskSimpleCower::InjectHooks();
        CTaskSimpleClearLookAt::InjectHooks();
        CTaskSimpleDead::InjectHooks();
        // CTaskSimpleDetonate::InjectHooks();
        // + CTaskSimpleDieInCar
        CTaskSimpleDoHandSignal::InjectHooks();
        CTaskSimpleDrown::InjectHooks();
        CTaskSimpleDrownInCar::InjectHooks();
        CTaskSimpleDuckToggle::InjectHooks();
        CTaskSimpleDuckWhileShotsWhizzing::InjectHooks(); // lote 187A (sin validar)
        CTaskSimpleEvasiveDive::InjectHooks();
        CTaskSimpleEvasiveStep::InjectHooks();
        CTaskSimpleFightingControl::InjectHooks();
        CTaskSimpleFinishBrain::InjectHooks();
        CTaskSimpleGunControl::InjectHooks();
        // CTaskSimpleHailTaxi::InjectHooks();
        // CTaskSimpleHailTaxiAndPause::InjectHooks();
        CTaskSimpleHandsUp::InjectHooks();
        CTaskSimpleHitByGunFromFront::InjectHooks();
        CTaskSimpleHitByGunFromLeft::InjectHooks();
        CTaskSimpleHitByGunFromRear::InjectHooks();
        CTaskSimpleHitByGunFromRight::InjectHooks();
        CTaskSimpleHitFromBack::InjectHooks();
        CTaskSimpleHitFromBehind::InjectHooks();
        CTaskSimpleHitFromFront::InjectHooks();
        CTaskSimpleHitFromLeft::InjectHooks();
        CTaskSimpleHitFromRight::InjectHooks();
        CTaskSimpleHitWall::InjectHooks();
        CTaskSimpleKillPedWithCar::InjectHooks();
        CTaskSimpleHurtPedWithCar::InjectHooks();
        CTaskSimpleLookAbout::InjectHooks();
        CTaskSimpleOnEscalator::InjectHooks(); // lote 171A (banco Fn171A)
        CTaskSimplePickUpBike::InjectHooks();
        CTaskSimplePlayerOnFire::InjectHooks();
        CTaskSimpleRunAnimLoopedMiddle::InjectHooks(); // lote 202A (sin validar): solo funciones sueltas por direccion
        CTaskSimpleRunTimedAnim::InjectHooks();
        CTaskSimpleSay::InjectHooks();
        CTaskSimpleSetCharDecisionMaker::InjectHooks();
        CTaskSimpleSetCharIgnoreWeaponRangeFlag::InjectHooks();
        CTaskSimpleSetKindaStayInSamePlace::InjectHooks();
        CTaskSimpleSetPedAsAutoDriver::InjectHooks();
        CTaskSimpleShakeFist::InjectHooks();
        CTaskSimpleSitDown::InjectHooks();
        CTaskSimpleSitIdle::InjectHooks();
        CTaskSimpleStandUp::InjectHooks();
        CTaskSimpleThrowControl::InjectHooks();
        CTaskSimpleTired::InjectHooks();
        // CTaskSimpleTriggerEvent::InjectHooks();
        CTaskSimpleUseAtm::InjectHooks();
        CTaskSimpleTurn180::InjectHooks();
        CTaskSimpleWaitForBus::InjectHooks();
        CTaskSimpleWaitForPizza::InjectHooks(); // lote 149A (banco Fn149A): destructor con borrado
        CTaskSimpleWaitUntilAreaCodesMatch::InjectHooks();
        CTaskSimpleWaitUntilLeaderAreaCodesMatch::InjectHooks();
        CTaskSimpleWaitUntilPedIsInCar::InjectHooks();
        CTaskSimpleWaitUntilPedIsOutCar::InjectHooks();
        CTaskComplexSequence::InjectHooks();
        CTaskComplexUseSequence::InjectHooks();
        CTaskSequences::InjectHooks();
        CTaskTimer::InjectHooks();
        CTaskSimpleCarSlowDragPedOut::InjectHooks();
        CTaskManager::InjectHooks();
        CTaskSimpleCreateCarAndGetIn::InjectHooks();
        CTaskComplexEnterAnyCarAsDriver::InjectHooks();
        CTaskComplexLeaveAnyCar::InjectHooks();
        CTaskSimpleAbseil::InjectHooks();
        CTaskComplexWanderCop::InjectHooks();
        CTaskSimpleDieInCar::InjectHooks();
        CTaskComplexWanderGang::InjectHooks();
        CTaskComplexObserveTrafficLights::InjectHooks();
        CTaskComplexClimb::InjectHooks();
        CTaskComplexBeStill::InjectHooks();
        CTaskComplexBeInCouple::InjectHooks();
        CTaskCategories::InjectHooks();
        CTaskComplexUseMobilePhone::InjectHooks();
        CTaskSimpleStandStill::InjectHooks();
        CTaskSimpleCarDrive::InjectHooks();
        CTaskSimpleScratchHead::InjectHooks();
        CTaskComplexPartnerChat::InjectHooks();
        CTaskComplexChat::InjectHooks();
        CTaskSimpleUseGun::InjectHooks();
        CTaskSimpleGangDriveBy::InjectHooks();
        CTaskComplexObserveTrafficLightsAndAchieveHeading::InjectHooks();
        CTaskSimpleInAir::InjectHooks();
        CTaskComplexHitPedWithCar::InjectHooks();
        CTaskSimplePlayerOnFoot::InjectHooks();
        CTaskComplexFollowPedFootsteps::InjectHooks();
        CTaskComplexGetUpAndStandStill::InjectHooks();
        CTaskComplexGoPickUpEntity::InjectHooks();
        CTaskSimpleDie__InjectHooks();
        CTaskSimpleDuck::InjectHooks();
        CTaskComplexPolicePursuit::InjectHooks();
        CTaskSimpleFacial::InjectHooks();
        CTaskComplexCopInCar::InjectHooks();
        CTaskComplexFacial::InjectHooks();
        CTaskComplexInAirAndLand::InjectHooks();
        CTaskSimpleGetUp::InjectHooks();
        CTaskSimpleUninterruptable::InjectHooks();
        CTaskComplexFallAndGetUp::InjectHooks();
        CTaskComplexFollowNodeRoute::InjectHooks();
        CTaskComplexFollowLeaderInFormation::InjectHooks();
        CTaskSimpleTriggerLookAt::InjectHooks();
        CTaskSimpleHitHead::InjectHooks();
        CTaskUtilityLineUpPedWithCar::InjectHooks();
        CTaskSimpleLand::InjectHooks();
        CTaskSimpleJetPack::InjectHooks();
        CTaskSimpleSetStayInSamePlace::InjectHooks();
        CTaskSimpleJump::InjectHooks();
        CTaskSimpleFall::InjectHooks();
        CTaskSimpleClimb::InjectHooks();
        CTaskComplexPlayHandSignalAnim::InjectHooks();
        CTaskComplexWaitForPed::InjectHooks();
        CTaskComplexJump::InjectHooks();
        CTaskComplexWanderStandard::InjectHooks();
        CTaskComplexSunbathe::InjectHooks();
        CTaskComplexEnterCar::InjectHooks();
        CTaskSimpleTogglePedThreatScanner::InjectHooks();
        CTaskComplexUseGoggles::InjectHooks();
        CTaskComplexCrossRoadLookAndAchieveHeading::InjectHooks();
        CTaskComplexGoToPointAndStandStill::InjectHooks();
        CTaskSimpleAchieveHeading::InjectHooks();
        CTaskSimpleGiveCPR::InjectHooks();
        CTaskSimpleCarSetPedInAsPassenger::InjectHooks();
        CTaskComplexDriveFireTruck::InjectHooks();
        CTaskSimpleSwim::InjectHooks();
        CTaskComplexWalkRoundObject::InjectHooks();
        CTaskSimplePause::InjectHooks();
        CTaskComplexEnterCarAsPassenger::InjectHooks();
        CTaskComplexEnterCarAsDriver::InjectHooks();
        CTaskSimpleNone::InjectHooks();
        CTaskComplexKillPedOnFoot::InjectHooks();
        CTaskSimpleThrowProjectile::InjectHooks();
        CTaskSimpleGoToPoint::InjectHooks();
        CTaskComplexWanderMedic::InjectHooks();
        CTaskSimpleCarDriveTimed::InjectHooks();
        CTaskComplexCarDriveWander::InjectHooks();
        CTaskSimpleStealthKill::InjectHooks();
        CTaskSimpleCarSetPedOut::InjectHooks();
        CTaskSimpleAnim::InjectHooks();
        CTaskSimpleRunAnim::InjectHooks();
        CTaskComplexWanderCriminal::InjectHooks();
        CTaskComplexWanderProstitute::InjectHooks();
        CTaskComplexPartnerGreet::InjectHooks();
        CTaskComplexWalkRoundCar::InjectHooks();
        CTaskComplexMedicTreatInjuredPed::InjectHooks();
        CTaskSimplePlayHandSignalAnim::InjectHooks();
        CTaskComplexCarDrive::InjectHooks();
        CTaskComplexCarDriveMission::InjectHooks();
        CTaskComplexCarDriveMissionFleeScene::InjectHooks();
        CTaskComplexCarDriveMissionKillPed::InjectHooks();
        CTaskComplexKillPedFromBoat::InjectHooks();
        CTaskComplexLeaveCar::InjectHooks();
        CTaskComplexTreatAccident::InjectHooks();
        CTaskComplexGoToPointAndStandStillTimed::InjectHooks();
        CTaskComplexPartnerShove::InjectHooks();
        CTaskSimpleRunNamedAnim::InjectHooks();
        CTaskComplexProstituteSolicit::InjectHooks();
        CTaskComplexStuckInAir::InjectHooks();
        CTaskSimpleHoldEntity::InjectHooks();
        CTaskSimpleGoTo::InjectHooks();
        CTaskComplexPartner::InjectHooks();
        CTaskSimpleCarSetPedInAsDriver::InjectHooks();
        CTaskComplexWander::InjectHooks();
        CTaskSimplePutDownEntity::InjectHooks();
        CTaskComplexUseSwatRope::InjectHooks();
        CTaskSimpleGoToPointFine::InjectHooks();
        CTaskSimpleIKChain::InjectHooks();
        CTaskSimpleIKManager::InjectHooks();
        CTaskSimpleIKPointArm::InjectHooks();
        CTaskSimpleIKLookAt::InjectHooks();
        CTaskComplexDie::InjectHooks();
        CTaskComplexEnterBoatAsDriver::InjectHooks();
        CTaskSimpleFight::InjectHooks();
        CTaskComplexUseWaterCannon::InjectHooks();
        CTaskComplexDriveToPoint::InjectHooks();
        CTaskSimpleSlideToCoord::InjectHooks();
        CTaskComplexPartnerDeal::InjectHooks();
        CTaskSimplePickUpEntity::InjectHooks();
        CTaskComplexBeInGroup::InjectHooks();
        CTaskComplexBeCop::InjectHooks();
        CTaskComplexAvoidOtherPedWhileWandering::InjectHooks();
        CTaskComplexAvoidEntity::InjectHooks();
        CTaskComplexArrestPed::InjectHooks();

        const auto EntitySeekPosCalculators = [] {
            CEntitySeekPosCalculator::InjectHooks();
            CEntitySeekPosCalculatorStandard::InjectHooks();
            CEntitySeekPosCalculatorRadiusAngleOffset::InjectHooks();
            CEntitySeekPosCalculatorXYOffset::InjectHooks();
            CEntitySeekPosCalculatorFixedPos::InjectHooks();
        };
        EntitySeekPosCalculators();
    };

    const auto Events = []() {
        CEvent::InjectHooks();
        CEventSeenPanickedPed::InjectHooks();
        CEventCarUpsideDown::InjectHooks();
        CEventPassObject::InjectHooks();
        CEventLeanOnVehicle::InjectHooks();
        CEventSeenCop::InjectHooks();
        CEventHealthLow::InjectHooks(); // lotes 196A y 197A (sin validar): constructor, cuerpo del destructor, CloneEditable y destructor con borrado
        CEventHealthReallyLow::InjectHooks(); // lote 197A (sin validar)
        CEventLowAngerAtPlayer::InjectHooks(); // lote 197A (sin validar)
        CEventHighAngerAtPlayer::InjectHooks(); // lote 197A (sin validar)
        CEventOnFire::InjectHooks();
        CEventRevived::InjectHooks();
        CEventHandlerHistory::InjectHooks();
        CEventEditableResponse::InjectHooks();
        CEventDamage::InjectHooks();
        CEventAttractor::InjectHooks();
        CEventScriptedAttractor::InjectHooks();
        CEventGunShot::InjectHooks();
        CEventGunShotWhizzedBy::InjectHooks();
        CEventGunAimedAt::InjectHooks();
        CEventDeath::InjectHooks();
        CEventDeadPed::InjectHooks();
        CEventDraggedOutCar::InjectHooks();
        CEventGotKnockedOverByCar::InjectHooks();
        CEventKnockOffBike::InjectHooks();
        CEventScriptCommand::InjectHooks();
        CEventSoundQuiet::InjectHooks();
        CEventPedToChase::InjectHooks();
        CEventPedToFlee::InjectHooks();
        CEventPotentialWalkIntoVehicle::InjectHooks();
        CEventPotentialWalkIntoObject::InjectHooks();
        CEventPotentialWalkIntoFire::InjectHooks();
        CEventPotentialWalkIntoPed::InjectHooks();
        CEventPedCollisionWithPed::InjectHooks();
        CEventPedCollisionWithPlayer::InjectHooks();
        CEventPlayerCollisionWithPed::InjectHooks();
        CEventObjectCollision::InjectHooks();
        CEventBuildingCollision::InjectHooks();
        CEventVehicleToSteal::InjectHooks();
        CEventVehicleDamage::InjectHooks();
        CEventVehicleDamageWeapon::InjectHooks();
        CEventVehicleDied::InjectHooks();
        CEventVehicleThreat::InjectHooks();
        CEventVehicleOnFire::InjectHooks();
        CEventVehicleHitAndRun::InjectHooks();
        CEventVehicleCollision::InjectHooks();
        CEventVehicleDamageCollision::InjectHooks();
        CEventHitByWaterCannon::InjectHooks();
        CEventInWater::InjectHooks();
        CEventCreatePartnerTask::InjectHooks();
        CEventInAir::InjectHooks();
        CEventStuckInAir::InjectHooks();
        CEventAcquaintancePed::InjectHooks();
        CEventLeaderEnteredCarAsDriver::InjectHooks();
        CEventLeaderExitedCarAsDriver::InjectHooks();
        CEventLeaderQuitEnteringCarAsDriver::InjectHooks();
        CEventAreaCodes::InjectHooks();
        CEventLeaderEntryExit::InjectHooks();
        CEventSpecial::InjectHooks();
        CEventFireNearby::InjectHooks();
        CEventGroupEvent::InjectHooks();
        CEventGroup::InjectHooks();
        CEventGlobalGroup::InjectHooks();
        CEventPlayerCommandToGroup::InjectHooks();
        CEventPlayerCommandToGroupAttack::InjectHooks();
        CEventPlayerCommandToGroupGather::InjectHooks();
        CEventDontJoinPlayerGroup::InjectHooks();
        CEventNewGangMember::InjectHooks();
        CEventEscalator::InjectHooks();
        CEventDanger::InjectHooks();
        CEventSexyVehicle::InjectHooks();
        CEventChatPartner::InjectHooks();
        CEventCopCarBeingStolen::InjectHooks();
        CEventHandler::InjectHooks();
        CEventAcquaintancePedHate::InjectHooks();
        CEventAcquaintancePedHateBadlyLit::InjectHooks();
        CEventAcquaintancePedRespect::InjectHooks();
        CEventAcquaintancePedLike::InjectHooks();
        CEventAcquaintancePedDislike::InjectHooks();
        CEventInteriorUseInfo::InjectHooks();
        CEventPedEnteredMyVehicle::InjectHooks();
        CEventPotentialGetRunOver::InjectHooks();
        // + CEventPotentialWalkIntoBuilding::InjectHooks();
        CEventSexyPed::InjectHooks();
        // + CEventSignalAtPed
        // + CEventSoundLoud
        // - CEventSource::InjectHooks();
        // - CEventScanner::InjectHooks();
    };

    const auto Fx = []() {
        FxSystemBP_c::InjectHooks();
        FxSystem_c::InjectHooks();
        FxSphere_c::InjectHooks();
        FxFrustumInfo_c::InjectHooks();
        FxPrimBP_c::InjectHooks();
        FxPrim_c::InjectHooks();
        FxMemoryPool_c::InjectHooks();
        FxInfoManager_c::InjectHooks();
        FxInfo_c::InjectHooks(); // lote 230A (sin validar)
        FxInfoGroundCollide_c::InjectHooks();
        FxInfoWind_c::InjectHooks();
        FxInfoJitter_c::InjectHooks();
        FxInfoRotSpeed_c::InjectHooks();
        FxInfoFloat_c::InjectHooks();
        FxInfoUnderwater_c::InjectHooks();
        FxInfoColour_c::InjectHooks();
        FxInfoSize_c::InjectHooks();
        FxInfoSpriteRect_c::InjectHooks();
        FxInfoHeatHaze_c::InjectHooks();
        FxInfoTrail_c::InjectHooks();
        FxInfoFlat_c::InjectHooks();
        FxInfoDir_c::InjectHooks();
        FxInfoAnimTexture_c::InjectHooks();
        FxInfoColourRange_c::InjectHooks();
        FxInfoSelfLit_c::InjectHooks();
        FxInfoColourBright_c::InjectHooks();
        FxInfoSmoke_c::InjectHooks();
        FxInfoEmRate_c::InjectHooks();
        FxInfoEmSize_c::InjectHooks();
        FxInfoEmSpeed_c::InjectHooks();
        FxInfoEmDir_c::InjectHooks();
        FxInfoEmAngle_c::InjectHooks();
        FxInfoEmLife_c::InjectHooks();
        FxInfoEmPos_c::InjectHooks();
        FxInfoEmWeather_c::InjectHooks();
        FxInfoEmRotation_c::InjectHooks();
        FxInfoNoise_c::InjectHooks();
        FxInfoForce_c::InjectHooks();
        FxInfoFriction_c::InjectHooks();
        FxInfoAttractPt_c::InjectHooks();
        FxInfoAttractLine_c::InjectHooks();
        FxInterpInfoFloat_c::InjectHooks();
        FxInterpInfoU255_c::InjectHooks();
        FxInterpInfo32_c::InjectHooks();
        FxInterpInfo255_c::InjectHooks();
        FxManager_c::InjectHooks();
        // ReversibleHooks::Install("FxFrustumInfo_c", "IsCollision", 0x4AA030, &FxFrustumInfo_c::IsCollision);
        FxEmitterPrt_c::InjectHooks();
        Particle_c::InjectHooks();
        FxEmitterBP_c::InjectHooks();
        FxEmitter_c::InjectHooks();
        Fx_c::InjectHooks();
        FxPrtMult_c::InjectHooks();
    };

    const auto Vehicle = []() {
        CAutomobile::InjectHooks();
        CBike::InjectHooks();
        CBmx::InjectHooks();
        CBoat::InjectHooks();
        CHeli::InjectHooks();
        CMonsterTruck::InjectHooks();
        CPlane::InjectHooks();
        CQuadBike::InjectHooks();
        CTrailer::InjectHooks();
        CRideAnimData::InjectHooks();
        CTrain::InjectHooks();
        CTrainNode::InjectHooks();
        CVehicle::InjectHooks();
    };

    const auto Interior = []() {
        Furniture_c::InjectHooks();
        FurnitureGroup_c::InjectHooks();
        FurnitureManager_c::InjectHooks();
        FurnitureSubGroup_c::InjectHooks();
        Interior_c::InjectHooks();
        InteriorGroup_c::InjectHooks();
        InteriorManager_c::InjectHooks();
    };

    const auto Scripts = []() {
        CScriptsForBrains::InjectHooks();
        CRunningScript::InjectHooks();
        CTheScripts::InjectHooks();
        CSearchLight::InjectHooks();
        CStreamedScripts::InjectHooks();
        CScripted2dEffects::InjectHooks();
        CScriptResourceManager::InjectHooks();
    };

    const auto App = []() {
        VideoPlayer::InjectHooks();
        Securom::InjectHooks();
        AppInjectHooks();
        Win32InjectHooks();
        RsInjectHooks();
        VideoModeInjectHooks();
    };

    const auto Animation = []() {
        CAnimBlendAssocGroup::InjectHooks();
        CAnimBlendAssociation::InjectHooks();
        CAnimBlendClumpData::InjectHooks();
        CAnimBlendHierarchy::InjectHooks();
        CAnimBlendNode::InjectHooks();
        CAnimBlendSequence::InjectHooks();
        CAnimBlendStaticAssociation::InjectHooks();
        CAnimManager::InjectHooks();
    };

    Animation();
    App();
    Audio();
    Tasks();
    Events();
    Fx();
    Vehicle();
    Interior();
    Scripts();
#if _DEBUG
    CCurves::TestCurves();
#endif
}

void InjectHooksMain(HMODULE hThisDLL) {
    ReversibleHooks::OnInjectionBegin(hThisDLL);
    InjectHooksMain();
    ReversibleHooks::OnInjectionEnd();
}
