# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-09-23 18:20 UTC

## Vista general

| Clases completas | Clases parciales | Sin revertir | Funciones revertidas | Funciones pendientes |
| :---: | :---: | :---: | :---: | :---: |
| **614 / 701** | **85 / 701** | **2 / 701** | **7,485 / 8,014** | **529** |
| 87.59% | 12.13% | 0.29% | 93.40% | 6.60% |

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Estos porcentajes miden entradas registradas como `reversed`, no cobertura de pruebas ni estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="completas"></a>
## 🟢 Completas · 614 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; 🔒 señala que al menos una sigue bloqueada.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| `App` (7) | `CGlass` (19) | `CTaskComplexSeekEntityAiming` (7) |
| `BoneNode_c` (17) | `CGrassRenderer` (12) | `CTaskComplexSeekEntityAnyMeansXYOffset` (5) |
| `BoneNodeManager_c` (8) | `CGridRef` (5) | `CTaskComplexSeekEntityRadiusAngleOffset` (9) |
| `BreakablePlugin` (6) | `cHandlingDataMgr` (11) | `CTaskComplexSeekEntityStandard` (9) |
| `BreakManager_c` (6) | `CHandObject` (3) | `CTaskComplexSequence` (13) |
| `BreakObject_c` (9) | `CHandShaker` (3) | `CTaskComplexShuffleSeats` (10) |
| `C2dEffect` (14) | `CHeli` (11) | `CTaskComplexSitDownThenIdleThenStandUp` (9) |
| `C3dMarker` (6) | `CHudColours` (5) | `CTaskComplexSmartFleeEntity` (7) |
| `C3dMarkers` (20) | `CInformFriendsEventQueue` (4) | `CTaskComplexStareAtPed` (7) |
| `C_PcSave` (5) | `CInformGroupEvent` (3) | `CTaskComplexStealCar` (8) |
| `CAccidentManager` (5) | `CInformGroupEventQueue` (4) | `CTaskComplexStuckInAir` (6) |
| `CAcquaintance` (5) | `CIniFile` (1) | `CTaskComplexTrackEntity` (11) 🔒 4 |
| `CActiveOccluder` (2) | `CIplDefPool` (1) | `CTaskComplexTreatAccident` (7) |
| `CAEAudioChannel` (12) | `CIplStore` (34) | `CTaskComplexTurnToFaceEntityOrCoord` (9) |
| `CAEAudioEnvironment` (6) | `CKeyArray` (4) | `CTaskComplexUseAttractor` (7) |
| `CAEAudioHardware` (54) | `CKeyboardState` (1) | `CTaskComplexUseAttractorPartner` (7) |
| `CAEAudioUtility` (11) | `CKeyGen` (4) | `CTaskComplexUseClosestFreeScriptedAttractorRun` (4) |
| `CAEBankLoader` (6) | `CLinkedUpgradeList` (2) | `CTaskComplexUseClosestFreeScriptedAttractorSprint` (4) |
| `CAECollisionAudioEntity` (17) | `CLoadedCarGroup` (8) | `CTaskComplexUseEffect` (8) |
| `CAECutsceneTrackManager` (8) | `CLoadingScreen` (20) | `CTaskComplexUseEffectRunning` (2) |
| `CAEDataStream` (20) 🔒 6 | `CLoadMonitor` (7) | `CTaskComplexUseEffectSprinting` (2) |
| `CAEDoorAudioEntity` (6) | `CLocalisation` (18) | `CTaskComplexUseGoggles` (5) |
| `CAEExplosionAudioEntity` (3) | `CLodAtomicModelInfo` (3) | `CTaskComplexUseScriptedAttractor` (7) |
| `CAEFireAudioEntity` (7) | `CMatrixLink` (2) | `CTaskComplexUseScriptedBrain` (8) |
| `CAEGlobalWeaponAudioEntity` (6) | `CMatrixLinkList` (10) | `CTaskComplexUseSwatRope` (7) |
| `CAEMFDecoder` (10) | `CMenuManager` (61) | `CTaskComplexUseWaterCannon` (5) |
| `CAEMP3BankLoader` (13) | `CMenuSystem` (21) | `CTaskComplexWaitAtAttractor` (7) |
| `CAEMP3TrackLoader` (7) | `CMessages` (28) | `CTaskComplexWaitForBackup` (8) |
| `CAEPedlessSpeechAudioEntity` (11) | `CMirrors` (8) | `CTaskComplexWaitForBus` (7) |
| `CAEPedSpeechAudioEntity` (58) | `CMissionTextOffsets` (1) | `CTaskComplexWaitForDryWeather` (7) |
| `CAESmoothFadeThread` (10) | `CModelInfo` (28) | `CTaskComplexWalkAlongsidePed` (9) |
| `CAESound` (18) | `CMotionBlurStreaks` (4) | `CTaskComplexWalkRoundBuildingAttempt` (12) |
| `CAESoundManager` (14) | `CMouseControllerState` (3) | `CTaskComplexWalkRoundFire` (8) |
| `CAEStreamingDecoder` (2) | `CMovingThings` (5) | `CTaskComplexWalkRoundObject` (1) |
| `CAEStreamThread` (14) | `CNodeRoute` (2) | `CTaskComplexWander` (14) |
| `CAEStreamTransformer` (2) | `CNodeRoutePool` (1) | `CTaskComplexWanderCop` (9) |
| `CAETrackLoader` (1) | `CObject` (48) | `CTaskComplexWanderFlee` (6) |
| `CAETwinLoopSoundEntity` (7) | `CObjectData` (2) | `CTaskManager` (18) |
| `CAEUserRadioTrackManager` (15) | `CObjectPool` (2) | `CTaskPool` (1) |
| `CAEVorbisDecoder` (13) | `CObjectSaveStructure` (2) | `CTaskSimpleAchieveHeading` (5) |
| `CAEWaterCannonAudioEntity` (7) | `COccluder` (3) | `CTaskSimpleAffectSecondaryBehaviour` (5) |
| `CAEWaveDecoder` (9) | `COcclusion` (5) | `CTaskSimpleAnim` (1) |
| `CAEWeaponAudioEntity` (20) | `Common` (9) | `CTaskSimpleArrestPed` (3) |
| `CAEWMADecoder` (11) | `COnscreenCounterEntry` (2) | `CTaskSimpleBeHit` (8) |
| `CAnimatedBuilding` (3) | `COnscreenTimer` (6) | `CTaskSimpleBikeJacked` (8) |
| `CAnimBlendAssocGroup` (15) | `CPad` (75) | `CTaskSimpleCarAlign` (10) |
| `CAnimBlendAssociation` (20) | `CPathNode` (1) | `CTaskSimpleCarCloseDoorFromInside` (10) |
| `CAnimBlendClumpData` (6) | `CPed` (145) | `CTaskSimpleCarCloseDoorFromOutside` (10) |
| `CAnimBlendHierarchy` (16) | `CPedAttractorManager` (14) | `CTaskSimpleCarFallOut` (9) |
| `CAnimBlendNode` (16) | `CPedAttractorPedPlacer` (1) | `CTaskSimpleCarGetIn` (9) |
| `CAnimBlendSequence` (14) | `CPedAttractorPool` (1) | `CTaskSimpleCarGetOut` (10) |
| `CAnimBlendStaticAssociation` (6) | `CPedClothesDesc` (7) | `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse` (6) |
| `CAnimManager` (31) | `CPedGroupDefaultTaskAllocatorChat` (2) | `CTaskSimpleCarJumpOut` (10) |
| `CAtomicModelInfo` (9) | `CPedGroupDefaultTaskAllocatorFollowAnyMeans` (2) | `CTaskSimpleCarOpenDoorFromOutside` (10) |
| `CAudioEngine` (82) | `CPedGroupDefaultTaskAllocatorFollowLimited` (2) | `CTaskSimpleCarOpenLockedDoorFromOutside` (9) |
| `CAudioZones` (5) | `CPedGroupDefaultTaskAllocatorStandStill` (2) | `CTaskSimpleCarSetPedSlowDraggedOut` (6) |
| `CAutomobile` (99) | `CPedGroupIntelligence` (26) | `CTaskSimpleCarSetTempAction` (6) |
| `CBaseModelInfo` (34) | `CPedGroupMembership` (18) | `CTaskSimpleCarShuffle` (9) |
| `CBirds` (6) | `CPedGroups` (2) | `CTaskSimpleCarSlowBeDraggedOut` (9) |
| `CBoat` (23) | `CPedIK` (9) | `CTaskSimpleCarSlowDragPedOut` (10) |
| `CBouncingPanel` (3) | `CPedIntelligence` (52) | `CTaskSimpleCarWaitForDoorNotToBeInUse` (7) |
| `CBox` (2) | `CPedIntelligencePool` (1) | `CTaskSimpleCarWaitToSlowDown` (6) |
| `CBridge` (4) | `CPedList` (7) | `CTaskSimpleClearLookAt` (6) |
| `CBrightLights` (4) | `CPedModelInfo` (9) | `CTaskSimpleClimb` (15) |
| `CBuilding` (2) | `CPedPlacement` (3) | `CTaskSimpleCower` (4) |
| `CBuildingPool` (1) | `CPedPool` (2) | `CTaskSimpleCreateCarAndGetIn` (6) |
| `CBulletInfo` (4) | `CPedScriptedTaskRecord` (5) | `CTaskSimpleDie` (7) |
| `CBulletTraces` (5) | `CPedScriptedTaskRecordData` (8) | `CTaskSimpleDrown` (1) |
| `cBuoyancy` (9) | `CPedStats` (4) | `CTaskSimpleDrownInCar` (1) |
| `CCarAI` (25) | `CPedTaskPair` (1) | `CTaskSimpleDuck` (15) |
| `CCarCtrl` (22) | `CPedType` (13) | `CTaskSimpleDuckToggle` (6) |
| `CCarGenerator` (8) | `CPhysical` (56) | `CTaskSimpleFacial` (5) |
| `CCheat` (72) | `CPickup` (12) | `CTaskSimpleFall` (6) |
| `CCheckpoint` (1) | `CPlaceable` (16) | `CTaskSimpleFinishBrain` (2) |
| `CCheckpoints` (8) | `CPlaceName` (2) | `CTaskSimpleGetUp` (5) |
| `CCivilianPed` (3) | `CPlaneTrail` (3) | `CTaskSimpleGiveCPR` (6) |
| `CClock` (9) | `CPlaneTrails` (4) | `CTaskSimpleGoTo` (4) |
| `CClothes` (11) | `CPlantColEntEntry` (2) | `CTaskSimpleGoToPoint` (6) |
| `CClouds` (21) | `CPlantLocTri` (3) | `CTaskSimpleGoToPointFine` (8) |
| `CClumpModelInfo` (24) | `CPlantMgr` (19) | `CTaskSimpleGunControl` (6) |
| `CColAccel` (11) | `CPlantSurfPropMgr` (4) | `CTaskSimpleHandsUp` (4) |
| `CColLine` (1) | `CPlayerInfo` (28) | `CTaskSimpleHitByGunFromFront` (3) |
| `CCollision` (59) 🔒 59 | `CPlayerPed` (51) | `CTaskSimpleHitByGunFromLeft` (3) |
| `CCollisionData` (8) | `CPlayerPedData` (5) | `CTaskSimpleHitByGunFromRear` (3) |
| `CCollisionPlugin` (2) | `CPointRoutePool` (1) | `CTaskSimpleHitByGunFromRight` (3) |
| `CColModel` (9) | `CPopCycle` (15) | `CTaskSimpleHitFromBack` (3) |
| `CColModelPool` (1) | `CPPTriPlantBuffer` (7) | `CTaskSimpleHitFromBehind` (3) |
| `CColourSet` (2) | `CPtrNodeDoubleLinkPool` (1) | `CTaskSimpleHitFromFront` (3) |
| `CColSphere` (5) | `CPtrNodeSingleLinkPool` (1) | `CTaskSimpleHitFromLeft` (3) |
| `CColStore` (23) | `CQuadTreeNode<T>` (12) | `CTaskSimpleHitFromRight` (3) |
| `CColTrianglePlane` (2) | `CRadar` (63) | `CTaskSimpleHitHead` (4) |
| `CCompressedMatrixNotAligned` (2) | `CRealTimeShadow` (8) | `CTaskSimpleHitWall` (3) |
| `CControllerConfigManager` (58) | `CRect` (8) | `CTaskSimpleHoldEntity` (14) |
| `CControllerState` (2) | `CReferences` (4) | `CTaskSimpleHurtPedWithCar` (6) |
| `CConversations` (11) | `CRegisteredCorona` (1) | `CTaskSimpleIKChain` (9) |
| `CCopPed` (7) | `CRegisteredMotionBlurStreak` (2) | `CTaskSimpleIKLookAt` (8) |
| `CCoronas` (11) | `CRenderer` (33) | `CTaskSimpleIKManager` (8) |
| `CCover` (16) | `CReplay` (47) | `CTaskSimpleIKPointArm` (6) |
| `CCranes` (5) | `CRestart` (11) | `CTaskSimpleInAir` (4) |
| `CCreepingFire` (3) | `CScripted2dEffects` (5) | `CTaskSimpleJetPack` (14) |
| `CCrime` (2) | `CScriptedBrainTaskEntry` (1) | `CTaskSimpleJump` (8) |
| `CCullZones` (21) | `CScriptedBrainTaskStore` (4) | `CTaskSimpleKillPedWithCar` (7) |
| `CCurrentVehicle` (2) | `CScriptResourceManager` (4) | `CTaskSimpleLand` (6) |
| `CCurves` (6) | `CSetPiece` (3) | `CTaskSimpleLeaveGroup` (2) |
| `CCustomBuildingDNPipeline` (14) | `CSetPieces` (5) | `CTaskSimpleLookAbout` (3) |
| `CCustomBuildingPipeline` (7) | `CShadowCamera` (19) | `CTaskSimplePause` (4) |
| `CCustomCarEnvMapPipeline` (37) | `CShinyTexts` (4) | `CTaskSimplePickUpBike` (9) |
| `CCustomCarPlateMgr` (14) | `CShopping` (30) | `CTaskSimplePickUpEntity` (4) |
| `CCustomRoadsignMgr` (9) | `CShotInfo` (5) | `CTaskSimplePlayerOnFire` (6) |
| `CCutsceneMgr` (35) | `CSimpleVariablesSaveStructure` (2) | `CTaskSimplePlayHandSignalAnim` (2) |
| `CCutsceneObject` (6) | `CSkidmarks` (7) | `CTaskSimplePutDownEntity` (3) |
| `CDamageAtomicModelInfo` (4) | `CSpecialFX` (7) | `CTaskSimpleRunAnim` (5) |
| `CDamageManager` (27) | `CSpecialPlateHandler` (4) | `CTaskSimpleRunTimedAnim` (7) |
| `CDarkel` (19) | `CSprite` (15) | `CTaskSimpleSay` (6) |
| `CData` (2) | `CSprite2d` (33) | `CTaskSimpleSetCharDecisionMaker` (6) |
| `CDebug` (1) | `CStaticShadow` (1) | `CTaskSimpleSetCharIgnoreWeaponRangeFlag` (6) |
| `CDecisionMakerTypesFileLoader` (7) | `CStats` (32) | `CTaskSimpleSetKindaStayInSamePlace` (6) |
| `CDirectory` (10) | `CStencilShadowObject` (3) | `CTaskSimpleSetPedAsAutoDriver` (6) |
| `CDoor` (6) | `CStreamedScripts` (12) | `CTaskSimpleShakeFist` (8) |
| `CDraw` (2) | `CStreaming` (94) | `CTaskSimpleSitDown` (8) |
| `CdStream` (9) | `CStreamingInfo` (7) | `CTaskSimpleSitIdle` (7) |
| `CDummy` (1) | `CStuckCarCheck` (8) | `CTaskSimpleStandStill` (3) |
| `CDummyObject` (2) | `CStuntJumpManager` (7) | `CTaskSimpleStandUp` (8) |
| `CDummyPool` (1) | `CTagManager` (19) | `CTaskSimpleStealthKill` (7) |
| `CEntity` (70) | `CTask` (4) | `CTaskSimpleSwim` (11) |
| `CEntityScanner` (4) | `CTaskAllocator` (3) | `CTaskSimpleThrowControl` (6) |
| `CEntitySeekPosCalculatorFixedPos` (1) | `CTaskAllocatorAttack` (5) | `CTaskSimpleThrowProjectile` (9) |
| `CEntitySeekPosCalculatorRadiusAngleOffset` (1) | `CTaskAllocatorKillOnFoot` (4) | `CTaskSimpleTired` (3) |
| `CEntitySeekPosCalculatorStandard` (1) | `CTaskAllocatorKillThreatsBasic` (6) | `CTaskSimpleTurn180` (6) |
| `CEntitySeekPosCalculatorXYOffset` (1) | `CTaskAllocatorKillThreatsBasicRandomGroup` (2) | `CTaskSimpleUninterruptable` (1) |
| `CEntryExitManager` (18) | `CTaskAllocatorKillThreatsDriveby` (5) | `CTaskSimpleUseAtm` (4) |
| `CEntryInfoList` (1) | `CTaskAllocatorPlayerCommandAttack` (3) | `CTaskSimpleUseGun` (19) |
| `CEntryInfoNode` (2) | `CTaskAllocatorPool` (1) | `CTaskSimpleWaitForBus` (6) |
| `CEntryInfoNodePool` (1) | `CTaskComplexAttractorPartnerWait` (5) | `CTaskSimpleWaitUntilAreaCodesMatch` (6) |
| `CEscalator` (5) | `CTaskComplexAvoidOtherPedWhileWandering` (15) | `CTaskSimpleWaitUntilLeaderAreaCodesMatch` (4) |
| `CEscalators` (4) | `CTaskComplexBeInGroup` (10) | `CTaskSimpleWaitUntilPedIsInCar` (6) |
| `CEvent` (3) | `CTaskComplexCarDrive` (11) | `CTaskSimpleWaitUntilPedIsOutCar` (6) |
| `CEventAcquaintancePed` (4) | `CTaskComplexCarDriveMission` (5) | `CTempColModels` (2) |
| `CEventAcquaintancePedHate` (1) | `CTaskComplexCarSlowBeDraggedOut` (11) | `CText` (9) |
| `CEventAcquaintancePedHateBadlyLit` (2) | `CTaskComplexCopInCar` (9) | `CTheCarGenerators` (7) |
| `CEventAreaCodes` (3) | `CTaskComplexDestroyCarArmed` (10) | `CTheScripts` (69) |
| `CEventAttractor` (4) | `CTaskComplexDestroyCarMelee` (10) | `CTheZones` (21) |
| `CEventBuildingCollision` (4) | `CTaskComplexDie` (5) | `CTimeCycle` (20) |
| `CEventCarUpsideDown` (2) | `CTaskComplexDieInCar` (6) | `CTimeInfo` (6) |
| `CEventCopCarBeingStolen` (2) | `CTaskComplexDiveFromAttachedEntityAndGetUp` (7) | `CTimeModelInfo` (2) |
| `CEventDamage` (20) | `CTaskComplexDragPedFromCar` (2) | `CTimer` (13) |
| `CEventDanger` (4) | `CTaskComplexDriveFireTruck` (6) | `CTrafficLights` (12) |
| `CEventDontJoinPlayerGroup` (1) | `CTaskComplexDrivePointRoute` (7) | `CTrailer` (13) |
| `CEventEditableResponse` (11) | `CTaskComplexEnterAnyCarAsDriver` (5) | `cTransmission` (4) |
| `CEventEscalator` (1) | `CTaskComplexEnterBoatAsDriver` (8) | `CTxdStore` (26) |
| `CEventGlobalGroup` (3) | `CTaskComplexEnterCar` (14) | `CUpsideDownCarCheck` (5) |
| `CEventGroup` (12) | `CTaskComplexEnterCarAsDriverTimed` (9) | `CUserDisplay` (2) |
| `CEventGunShot` (5) | `CTaskComplexEnterCarAsPassengerTimed` (9) | `CVehicle` (142) |
| `CEventGunShotWhizzedBy` (2) | `CTaskComplexEvasiveDiveAndGetUp` (9) | `CVehicleAnimGroup` (5) |
| `CEventHandlerHistory` (10) | `CTaskComplexExtinguishFires` (8) | `CVehicleAnimGroupData` (5) |
| `CEventHitByWaterCannon` (2) | `CTaskComplexFacial` (9) | `CVehicleModelInfo` (75) |
| `CEventInAir` (2) | `CTaskComplexFallAndGetUp` (9) | `CVehiclePool` (1) |
| `CEventKnockOffBike` (8) | `CTaskComplexFallToDeath` (6) | `CVehicleRecording` (29) |
| `CEventLeaderEnteredCarAsDriver` (2) | `CTaskComplexFleeAnyMeans` (8) | `CVisibilityPlugins` (84) |
| `CEventLeaderEntryExit` (1) | `CTaskComplexFleeEntity` (8) | `CWanted` (33) |
| `CEventLeaderExitedCarAsDriver` (2) | `CTaskComplexFleePoint` (11) | `CWaterCannon` (5) |
| `CEventLeaderQuitEnteringCarAsDriver` (2) | `CTaskComplexFleeShooting` (8) | `CWaterCannons` (4) |
| `CEventLeanOnVehicle` (2) | `CTaskComplexFollowLeaderAnyMeans` (8) | `CWeaponEffects` (9) |
| `CEventNewGangMember` (1) | `CTaskComplexFollowNodeRoute` (19) | `CWeaponInfo` (10) |
| `CEventObjectCollision` (2) | `CTaskComplexFollowPedFootsteps` (1) | `CWeaponModelInfo` (3) |
| `CEventOnFire` (1) | `CTaskComplexFollowPointRoute` (13) | `CWindModifiers` (2) |
| `CEventPassObject` (2) | `CTaskComplexGangLeader` (13) 🔒 11 | `CWorld` (94) |
| `CEventPedCollisionWithPed` (3) | `CTaskComplexGetOnBoatSeat` (8) | `FurnitureGroup_c` (6) |
| `CEventPedCollisionWithPlayer` (1) | `CTaskComplexGetUpAndStandStill` (5) | `FurnitureManager_c` (7) |
| `CEventPlayerCollisionWithPed` (1) | `CTaskComplexGoToBoatSteeringWheel` (6) | `FurnitureSubGroup_c` (3) |
| `CEventPlayerCommandToGroup` (2) | `CTaskComplexGoToCarDoorAndStandStill` (11) | `Fx_c` (8) |
| `CEventPlayerCommandToGroupAttack` (2) | `CTaskComplexGoToPointAndStandStill` (7) | `FxEmitterPrt_c` (1) |
| `CEventPlayerCommandToGroupGather` (1) | `CTaskComplexGoToPointAndStandStillTimed` (6) | `FxInfoManager_c` (4) |
| `CEventPool` (1) | `CTaskComplexGoToPointShooting` (3) | `FxManager_c` (25) |
| `CEventPotentialWalkIntoFire` (2) | `CTaskComplexHitByGunResponse` (8) | `FxMemoryPool_c` (5) |
| `CEventPotentialWalkIntoObject` (2) | `CTaskComplexHitResponse` (7) | `FxPrimBP_c` (2) |
| `CEventPotentialWalkIntoPed` (3) | `CTaskComplexInAirAndLand` (4) | `FxSphere_c` (4) |
| `CEventPotentialWalkIntoVehicle` (2) | `CTaskComplexInvestigateDeadPed` (9) | `FxSystemBP_c` (10) |
| `CEventScriptCommand` (8) | `CTaskComplexInvestigateDisturbance` (8) | `Game` (12) 🔒 3 |
| `CEventScriptedAttractor` (1) | `CTaskComplexInWater` (7) | `IKChain_c` (20) |
| `CEventSeenCop` (1) | `CTaskComplexJump` (6) | `IKChainManager_c` (17) |
| `CEventSeenPanickedPed` (2) | `CTaskComplexKillPedGroupOnFoot` (9) | `Input` (5) |
| `CEventSoundQuiet` (3) | `CTaskComplexKillPedOnFoot` (1) | `InteriorManager_c` (21) |
| `CEventStuckInAir` (4) | `CTaskComplexKillPedOnFootMelee` (10) | `JPegPlugin` (5) |
| `CEventVehicleCollision` (3) | `CTaskComplexKillPedOnFootStealth` (7) | `Light` (27) |
| `CEventVehicleDamage` (5) | `CTaskComplexLeaveAnyCar` (7) | `ModelIndices` (2) |
| `CEventVehicleDamageCollision` (2) | `CTaskComplexLeaveBoat` (8) | `NodeNamePlugin` (3) |
| `CEventVehicleDamageWeapon` (2) | `CTaskComplexLeaveCarAndDie` (3) | `PipelinePlugin` (3) |
| `CEventVehicleDied` (3) | `CTaskComplexLeaveCarAndFlee` (8) | `ProcObjectMan_c` (8) |
| `CEventVehicleHitAndRun` (3) | `CTaskComplexLeaveCarAndWander` (8) | `ProcSurfaceInfo_c` (4) |
| `CEventVehicleOnFire` (3) | `CTaskComplexLeaveCarAsPassengerWait` (10) | `Ps` (22) |
| `CEventVehicleToSteal` (2) | `CTaskComplexMedicTreatInjuredPed` (9) | `RenderBuffer` (4) |
| `CExplosion` (12) | `CTaskComplexMoveBackAndJump` (8) | `RpAnimBlend` (43) |
| `CFallingGlassPane` (2) | `CTaskComplexObserveTrafficLightsAndAchieveHeading` (5) | `Rs` (27) |
| `CFileLoader` (47) | `CTaskComplexOnFire` (9) | `RtAnim` (1) |
| `CFileMgr` (17) 🔒 13 | `CTaskComplexPartner` (1) | `RwCore` (4) |
| `CFire` (5) | `CTaskComplexPartnerChat` (1) | `RwHelper` (24) |
| `CFireManager` (24) | `CTaskComplexPartnerGreet` (7) | `Securom` (2) |
| `CGame` (22) | `CTaskComplexPartnerShove` (1) | `SurfaceInfos_c` (47) |
| `CGameLogic` (27) | `CTaskComplexPassObject` (9) | `VideoMode` (4) |
| `CGamma` (2) | `CTaskComplexPolicePursuit` (13) | `VideoPlayer` (6) |
| `CGangs` (8) | `CTaskComplexReactToGunAimedAt` (8) | `WaterCreature_c` (3) |
| `CGarage` (4) | `CTaskComplexRoadRage` (9) | `WaterCreatureManager_c` (7) |
| `CGarages` (21) | `CTaskComplexSeekCoverUntilTargetDead` (10) | `Win` (6) |
| `CGeneral` (9) | `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>` (9) |  |

</details>

<a id="parciales"></a>
## 🟡 Parciales · 85 clases

Orden alfabético para localizar clases. Para decidir qué abordar primero, consultar la [estimación por dificultad](ReversingPriorities.md).

| Clase | Revertidas / total | Pendientes | Avance | Locked |
| --- | ---: | ---: | ---: | ---: |
| `CAEAmbienceTrackManager` | 6/7 | **1** | 85.71% | 1 |
| `CAEFrontendAudioEntity` | 4/6 | **2** | 66.67% | 2 |
| `CAEPedAudioEntity` | 16/20 | **4** | 80.00% | 4 |
| `CAEPoliceScannerAudioEntity` | 13/17 | **4** | 76.47% | 4 |
| `CAERadioTrackManager` | 29/43 | **14** | 67.44% | 14 |
| `CAEScriptAudioEntity` | 8/19 | **11** | 42.11% | 11 |
| `CAEStaticChannel` | 7/8 | **1** | 87.50% | 1 |
| `CAEStreamingChannel` | 21/24 | **3** | 87.50% | 3 |
| `CAEVehicleAudioEntity` | 102/106 | **4** | 96.23% | 7 |
| `CAEWeatherAudioEntity` | 4/5 | **1** | 80.00% | 1 |
| `CBike` | 22/40 | **18** | 55.00% | 18 |
| `CBmx` | 7/10 | **3** | 70.00% | 3 |
| `CCam` | 14/44 | **30** | 31.82% | 30 |
| `CCamera` | 91/110 | **19** | 82.73% | 19 |
| `CCarEnterExit` | 27/32 | **5** | 84.38% | 5 |
| `CCarFXRenderer` | 9/10 | **1** | 90.00% | 1 |
| `CClothesBuilder` | 15/23 | **8** | 65.22% | 8 |
| `CCustomBuildingRenderer` | 3/7 | **4** | 42.86% | 4 |
| `CEntryExit` | 10/11 | **1** | 90.91% | 2 |
| `CEventHandler` | 69/72 | **3** | 95.83% | 3 |
| `CFont` | 32/36 | **4** | 88.89% | 4 |
| `CFormation` | 1/9 | **8** | 11.11% | 8 |
| `CGangWars` | 32/35 | **3** | 91.43% | 3 |
| `CGenericGameStorage` | 5/19 | **14** | 26.32% | 14 |
| `CGroupEventHandler` | 30/32 | **2** | 93.75% | 2 |
| `CInterestingEvents` | 2/7 | **5** | 28.57% | 5 |
| `CMonsterTruck` | 5/11 | **6** | 45.45% | 6 |
| `CPathFind` | 54/56 | **2** | 96.43% | 2 |
| `CPedDamageResponseCalculator` | 2/7 | **5** | 28.57% | 5 |
| `CPedGeometryAnalyser` | 9/38 | **29** | 23.68% | 29 |
| `CPedGroup` | 10/12 | **2** | 83.33% | 2 |
| `CPedGroupDefaultTaskAllocatorRandom` | 1/2 | **1** | 50.00% | 1 |
| `CPedGroupDefaultTaskAllocatorSitInLeaderCar` | 1/2 | **1** | 50.00% | 1 |
| `CPedGroupPlacer` | 1/4 | **3** | 25.00% | 3 |
| `CPickups` | 35/37 | **2** | 94.59% | 2 |
| `CPlane` | 14/23 | **9** | 60.87% | 9 |
| `CPools` | 16/19 | **3** | 84.21% | 3 |
| `CPopulation` | 64/66 | **2** | 96.97% | 2 |
| `CPostEffects` | 35/38 | **3** | 92.11% | 4 |
| `CProjectileInfo` | 1/12 | **11** | 8.33% | 11 |
| `CQuadBike` | 11/12 | **1** | 91.67% | 1 |
| `CRealTimeShadowManager` | 5/7 | **2** | 71.43% | 2 |
| `CRoadBlocks` | 7/8 | **1** | 87.50% | 1 |
| `CRope` | 5/7 | **2** | 71.43% | 2 |
| `CRopes` | 9/11 | **2** | 81.82% | 2 |
| `CRunningScript` | 24/39 | **15** | 61.54% | 15 |
| `CScriptsForBrains` | 3/6 | **3** | 50.00% | 3 |
| `CShadows` | 21/31 | **10** | 67.74% | 10 |
| `CStencilShadows` | 9/13 | **4** | 69.23% | 4 |
| `CTaskComplexArrestPed` | 4/7 | **3** | 57.14% | 3 |
| `CTaskComplexCarSlowBeDraggedOutAndStandUp` | 3/9 | **6** | 33.33% | 6 |
| `CTaskComplexDestroyCar` | 8/9 | **1** | 88.89% | 1 |
| `CTaskComplexFollowLeaderInFormation` | 1/7 | **6** | 14.29% | 6 |
| `CTaskComplexGangJoinRespond` | 2/7 | **5** | 28.57% | 5 |
| `CTaskComplexGoToAttractor` | 2/5 | **3** | 40.00% | 3 |
| `CTaskComplexGotoDoorAndOpen` | 3/7 | **4** | 42.86% | 4 |
| `CTaskComplexGoToPointAiming` | 5/8 | **3** | 62.50% | 3 |
| `CTaskComplexGoToPointAndStandStillAndAchieveHeading` | 3/7 | **4** | 42.86% | 4 |
| `CTaskComplexGoToPointAnyMeans` | 3/7 | **4** | 42.86% | 4 |
| `CTaskComplexKillCriminal` | 9/11 | **2** | 81.82% | 2 |
| `CTaskComplexKillPedFromBoat` | 2/7 | **5** | 28.57% | 5 |
| `CTaskComplexKillPedOnFootArmed` | 9/11 | **2** | 81.82% | 2 |
| `CTaskComplexPlayHandSignalAnim` | 3/9 | **6** | 33.33% | 6 |
| `CTaskComplexSignalAtPed` | 4/7 | **3** | 57.14% | 3 |
| `CTaskComplexSmartFleePoint` | 2/12 | **10** | 16.67% | 10 |
| `CTaskComplexSunbathe` | 2/8 | **6** | 25.00% | 6 |
| `CTaskComplexUseClosestFreeScriptedAttractor` | 6/8 | **2** | 75.00% | 2 |
| `CTaskComplexWalkRoundCar` | 9/13 | **4** | 69.23% | 4 |
| `CTaskGangHasslePed` | 2/5 | **3** | 40.00% | 3 |
| `CTaskGangHassleVehicle` | 2/8 | **6** | 25.00% | 6 |
| `CTaskGoToVehicleAndLean` | 2/7 | **5** | 28.57% | 5 |
| `CTaskLeanOnVehicle` | 3/5 | **2** | 60.00% | 2 |
| `CTaskSimpleCarDrive` | 7/13 | **6** | 53.85% | 6 |
| `CTaskSimpleChoking` | 5/7 | **2** | 71.43% | 2 |
| `CTaskSimpleFightingControl` | 6/7 | **1** | 85.71% | 1 |
| `CTaskSimplePlayerOnFoot` | 11/12 | **1** | 91.67% | 1 |
| `CTaskSimpleRunNamedAnim` | 4/7 | **3** | 57.14% | 3 |
| `CTaskUtilityLineUpPedWithCar` | 2/5 | **3** | 40.00% | 3 |
| `CTrain` | 20/40 | **20** | 50.00% | 20 |
| `CWaterLevel` | 25/30 | **5** | 83.33% | 5 |
| `CWeapon` | 35/41 | **6** | 85.37% | 6 |
| `CWeather` | 10/13 | **3** | 76.92% | 3 |
| `FxEmitterBP_c` | 2/9 | **7** | 22.22% | 7 |
| `FxSystem_c` | 31/35 | **4** | 88.57% | 4 |
| `InteriorGroup_c` | 2/24 | **22** | 8.33% | 22 |

<a id="sin-revertir"></a>
## 🔴 Sin revertir · 2 clases

| Clase | Revertidas / total | Pendientes | Avance | Locked |
| --- | ---: | ---: | ---: | ---: |
| `CTaskComplexGangFollower` | 0/7 | **7** | 0.00% | 7 |
| `Interior_c` | 0/52 | **52** | 0.00% | 52 |

<a id="metodo"></a>
## Cómo leer y actualizar

- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.
- **Locked:** indicador de bloqueo del hook; hay 630 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.
- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.

Actualizar los flags solo después de comparar con el original dentro del motor y regenerar desde la raíz:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.
