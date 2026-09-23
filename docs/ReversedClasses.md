# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-09-23 06:18 UTC

## Vista general

| Clases completas | Clases parciales | Sin revertir | Funciones revertidas | Funciones pendientes |
| :---: | :---: | :---: | :---: | :---: |
| **604 / 701** | **95 / 701** | **2 / 701** | **7,463 / 8,014** | **551** |
| 86.16% | 13.55% | 0.29% | 93.12% | 6.88% |

**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cómo leer y actualizar](#metodo)

> [!NOTE]
> Estos porcentajes miden entradas registradas como `reversed`, no cobertura de pruebas ni estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="completas"></a>
## 🟢 Completas · 604 clases

Orden alfabético. El número entre paréntesis indica funciones registradas; 🔒 señala que al menos una sigue bloqueada.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| `App` (7) | `CGeneral` (9) 🔒 1 | `CTaskComplexSeekEntityRadiusAngleOffset` (9) |
| `BoneNode_c` (17) | `CGlass` (19) | `CTaskComplexSeekEntityStandard` (9) |
| `BoneNodeManager_c` (8) | `CGrassRenderer` (12) | `CTaskComplexSequence` (13) |
| `BreakablePlugin` (6) | `CGridRef` (5) | `CTaskComplexShuffleSeats` (10) |
| `BreakManager_c` (6) | `cHandlingDataMgr` (11) | `CTaskComplexSitDownThenIdleThenStandUp` (9) |
| `BreakObject_c` (9) | `CHandObject` (3) | `CTaskComplexSmartFleeEntity` (7) |
| `C2dEffect` (14) | `CHandShaker` (3) | `CTaskComplexStareAtPed` (7) |
| `C3dMarker` (6) | `CHeli` (11) | `CTaskComplexStealCar` (8) |
| `C3dMarkers` (20) | `CHudColours` (5) | `CTaskComplexStuckInAir` (6) |
| `C_PcSave` (5) | `CInformFriendsEventQueue` (4) | `CTaskComplexTrackEntity` (11) 🔒 4 |
| `CAccidentManager` (5) | `CInformGroupEvent` (3) | `CTaskComplexTreatAccident` (7) |
| `CAcquaintance` (5) | `CInformGroupEventQueue` (4) | `CTaskComplexTurnToFaceEntityOrCoord` (9) |
| `CActiveOccluder` (2) | `CIniFile` (1) | `CTaskComplexUseAttractor` (7) |
| `CAEAudioChannel` (12) | `CIplDefPool` (1) | `CTaskComplexUseAttractorPartner` (7) |
| `CAEAudioEnvironment` (6) 🔒 1 | `CIplStore` (34) | `CTaskComplexUseClosestFreeScriptedAttractorRun` (4) |
| `CAEAudioHardware` (54) | `CKeyArray` (4) | `CTaskComplexUseClosestFreeScriptedAttractorSprint` (4) |
| `CAEAudioUtility` (11) | `CKeyboardState` (1) | `CTaskComplexUseEffect` (8) |
| `CAEBankLoader` (6) | `CKeyGen` (4) | `CTaskComplexUseEffectRunning` (2) |
| `CAECollisionAudioEntity` (17) | `CLinkedUpgradeList` (2) | `CTaskComplexUseEffectSprinting` (2) |
| `CAECutsceneTrackManager` (8) | `CLoadedCarGroup` (8) | `CTaskComplexUseGoggles` (5) |
| `CAEDataStream` (20) 🔒 6 | `CLoadingScreen` (20) | `CTaskComplexUseScriptedAttractor` (7) |
| `CAEDoorAudioEntity` (6) | `CLoadMonitor` (7) | `CTaskComplexUseScriptedBrain` (8) |
| `CAEExplosionAudioEntity` (3) | `CLocalisation` (18) | `CTaskComplexUseSwatRope` (7) |
| `CAEFireAudioEntity` (7) | `CLodAtomicModelInfo` (3) | `CTaskComplexUseWaterCannon` (5) |
| `CAEGlobalWeaponAudioEntity` (6) | `CMatrixLink` (2) | `CTaskComplexWaitAtAttractor` (7) |
| `CAEMFDecoder` (10) | `CMatrixLinkList` (10) | `CTaskComplexWaitForBackup` (8) |
| `CAEMP3BankLoader` (13) | `CMenuManager` (61) 🔒 1 | `CTaskComplexWaitForDryWeather` (7) |
| `CAEMP3TrackLoader` (7) | `CMenuSystem` (21) | `CTaskComplexWalkAlongsidePed` (9) |
| `CAEPedlessSpeechAudioEntity` (11) | `CMessages` (28) | `CTaskComplexWalkRoundBuildingAttempt` (12) |
| `CAEPedSpeechAudioEntity` (58) | `CMirrors` (8) | `CTaskComplexWalkRoundFire` (8) |
| `CAESmoothFadeThread` (10) | `CMissionTextOffsets` (1) | `CTaskComplexWalkRoundObject` (1) |
| `CAESound` (18) | `CModelInfo` (28) | `CTaskComplexWander` (14) |
| `CAESoundManager` (14) | `CMotionBlurStreaks` (4) | `CTaskComplexWanderCop` (9) |
| `CAEStreamingDecoder` (2) | `CMouseControllerState` (3) | `CTaskComplexWanderFlee` (6) |
| `CAEStreamThread` (14) | `CMovingThings` (5) | `CTaskManager` (18) |
| `CAEStreamTransformer` (2) | `CNodeRoute` (2) | `CTaskPool` (1) |
| `CAETrackLoader` (1) | `CNodeRoutePool` (1) | `CTaskSimpleAchieveHeading` (5) |
| `CAETwinLoopSoundEntity` (7) | `CObject` (48) | `CTaskSimpleAffectSecondaryBehaviour` (5) |
| `CAEUserRadioTrackManager` (15) 🔒 2 | `CObjectData` (2) | `CTaskSimpleAnim` (1) |
| `CAEVorbisDecoder` (13) | `CObjectPool` (2) | `CTaskSimpleArrestPed` (3) |
| `CAEWaterCannonAudioEntity` (7) | `CObjectSaveStructure` (2) | `CTaskSimpleBeHit` (8) |
| `CAEWaveDecoder` (9) | `COccluder` (3) | `CTaskSimpleBikeJacked` (8) |
| `CAEWeaponAudioEntity` (20) | `COcclusion` (5) | `CTaskSimpleCarAlign` (10) |
| `CAEWMADecoder` (11) | `Common` (9) | `CTaskSimpleCarCloseDoorFromInside` (10) |
| `CAnimatedBuilding` (3) | `COnscreenCounterEntry` (2) | `CTaskSimpleCarCloseDoorFromOutside` (10) |
| `CAnimBlendAssocGroup` (15) | `COnscreenTimer` (6) | `CTaskSimpleCarFallOut` (9) |
| `CAnimBlendAssociation` (20) | `CPad` (75) 🔒 2 | `CTaskSimpleCarGetIn` (9) |
| `CAnimBlendClumpData` (6) | `CPathNode` (1) | `CTaskSimpleCarGetOut` (10) |
| `CAnimBlendHierarchy` (16) | `CPed` (145) | `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse` (6) |
| `CAnimBlendNode` (16) | `CPedAttractorManager` (14) | `CTaskSimpleCarJumpOut` (10) |
| `CAnimBlendSequence` (14) | `CPedAttractorPedPlacer` (1) | `CTaskSimpleCarOpenDoorFromOutside` (10) |
| `CAnimBlendStaticAssociation` (6) | `CPedAttractorPool` (1) | `CTaskSimpleCarOpenLockedDoorFromOutside` (9) |
| `CAnimManager` (31) | `CPedClothesDesc` (7) | `CTaskSimpleCarSetPedSlowDraggedOut` (6) |
| `CAtomicModelInfo` (9) | `CPedGroupDefaultTaskAllocatorChat` (2) | `CTaskSimpleCarSetTempAction` (6) |
| `CAudioEngine` (82) | `CPedGroupDefaultTaskAllocatorFollowAnyMeans` (2) | `CTaskSimpleCarShuffle` (9) |
| `CAudioZones` (5) | `CPedGroupDefaultTaskAllocatorFollowLimited` (2) | `CTaskSimpleCarSlowBeDraggedOut` (9) |
| `CAutomobile` (99) | `CPedGroupDefaultTaskAllocatorStandStill` (2) | `CTaskSimpleCarSlowDragPedOut` (10) |
| `CBaseModelInfo` (34) | `CPedGroupIntelligence` (26) | `CTaskSimpleCarWaitForDoorNotToBeInUse` (7) |
| `CBirds` (6) | `CPedGroupMembership` (18) | `CTaskSimpleCarWaitToSlowDown` (6) |
| `CBoat` (23) | `CPedGroups` (2) | `CTaskSimpleClearLookAt` (6) |
| `CBouncingPanel` (3) | `CPedIK` (9) | `CTaskSimpleClimb` (15) |
| `CBox` (2) | `CPedIntelligence` (52) | `CTaskSimpleCower` (4) |
| `CBridge` (4) | `CPedIntelligencePool` (1) | `CTaskSimpleCreateCarAndGetIn` (6) |
| `CBrightLights` (4) | `CPedList` (7) | `CTaskSimpleDie` (7) |
| `CBuilding` (2) | `CPedModelInfo` (9) | `CTaskSimpleDrown` (1) |
| `CBuildingPool` (1) | `CPedPlacement` (3) | `CTaskSimpleDrownInCar` (1) |
| `CBulletInfo` (4) | `CPedPool` (2) | `CTaskSimpleDuck` (15) |
| `CBulletTraces` (5) | `CPedScriptedTaskRecord` (5) | `CTaskSimpleDuckToggle` (6) |
| `cBuoyancy` (9) | `CPedScriptedTaskRecordData` (8) | `CTaskSimpleFacial` (5) |
| `CCarAI` (25) | `CPedStats` (4) | `CTaskSimpleFall` (6) |
| `CCarGenerator` (8) | `CPedTaskPair` (1) | `CTaskSimpleFinishBrain` (2) |
| `CCheat` (72) | `CPedType` (13) | `CTaskSimpleGetUp` (5) |
| `CCheckpoint` (1) | `CPhysical` (56) 🔒 2 | `CTaskSimpleGiveCPR` (6) |
| `CCheckpoints` (8) | `CPickup` (12) | `CTaskSimpleGoTo` (4) |
| `CCivilianPed` (3) | `CPlaceable` (16) | `CTaskSimpleGoToPoint` (6) |
| `CClock` (9) | `CPlaceName` (2) | `CTaskSimpleGoToPointFine` (8) |
| `CClothes` (11) | `CPlaneTrail` (3) | `CTaskSimpleGunControl` (6) |
| `CClouds` (21) | `CPlaneTrails` (4) | `CTaskSimpleHandsUp` (4) |
| `CClumpModelInfo` (24) | `CPlantColEntEntry` (2) | `CTaskSimpleHitByGunFromFront` (3) |
| `CColAccel` (11) | `CPlantLocTri` (3) | `CTaskSimpleHitByGunFromLeft` (3) |
| `CColLine` (1) | `CPlantMgr` (19) | `CTaskSimpleHitByGunFromRear` (3) |
| `CCollision` (59) 🔒 59 | `CPlantSurfPropMgr` (4) | `CTaskSimpleHitByGunFromRight` (3) |
| `CCollisionData` (8) | `CPlayerInfo` (28) | `CTaskSimpleHitFromBack` (3) |
| `CCollisionPlugin` (2) | `CPlayerPed` (51) | `CTaskSimpleHitFromBehind` (3) |
| `CColModel` (9) | `CPlayerPedData` (5) | `CTaskSimpleHitFromFront` (3) |
| `CColModelPool` (1) | `CPointRoutePool` (1) | `CTaskSimpleHitFromLeft` (3) |
| `CColourSet` (2) | `CPPTriPlantBuffer` (7) | `CTaskSimpleHitFromRight` (3) |
| `CColSphere` (5) | `CPtrNodeDoubleLinkPool` (1) | `CTaskSimpleHitHead` (4) |
| `CColStore` (23) | `CPtrNodeSingleLinkPool` (1) | `CTaskSimpleHitWall` (3) |
| `CColTrianglePlane` (2) | `CQuadTreeNode<T>` (12) | `CTaskSimpleHoldEntity` (14) |
| `CCompressedMatrixNotAligned` (2) | `CRadar` (63) | `CTaskSimpleHurtPedWithCar` (6) |
| `CControllerConfigManager` (58) | `CRealTimeShadow` (8) | `CTaskSimpleIKChain` (9) |
| `CControllerState` (2) | `CRect` (8) | `CTaskSimpleIKLookAt` (8) |
| `CConversations` (11) | `CReferences` (4) | `CTaskSimpleIKManager` (8) |
| `CCopPed` (7) | `CRegisteredCorona` (1) | `CTaskSimpleIKPointArm` (6) |
| `CCoronas` (11) | `CRegisteredMotionBlurStreak` (2) | `CTaskSimpleInAir` (4) |
| `CCover` (16) 🔒 2 | `CRenderer` (33) | `CTaskSimpleJetPack` (14) |
| `CCranes` (5) | `CReplay` (47) | `CTaskSimpleJump` (8) |
| `CCreepingFire` (3) | `CRestart` (11) | `CTaskSimpleKillPedWithCar` (7) |
| `CCrime` (2) | `CScripted2dEffects` (5) | `CTaskSimpleLand` (6) |
| `CCullZones` (21) | `CScriptedBrainTaskEntry` (1) | `CTaskSimpleLeaveGroup` (2) |
| `CCurrentVehicle` (2) | `CScriptedBrainTaskStore` (4) | `CTaskSimpleLookAbout` (3) |
| `CCurves` (6) 🔒 1 | `CSetPiece` (3) | `CTaskSimplePause` (4) |
| `CCustomBuildingDNPipeline` (14) | `CSetPieces` (5) | `CTaskSimplePickUpBike` (9) |
| `CCustomBuildingPipeline` (7) | `CShadowCamera` (19) | `CTaskSimplePickUpEntity` (4) |
| `CCustomCarEnvMapPipeline` (37) | `CShinyTexts` (4) | `CTaskSimplePlayerOnFire` (6) |
| `CCustomCarPlateMgr` (14) | `CShopping` (30) | `CTaskSimplePlayHandSignalAnim` (2) |
| `CCustomRoadsignMgr` (9) | `CShotInfo` (5) | `CTaskSimplePutDownEntity` (3) |
| `CCutsceneMgr` (35) | `CSimpleVariablesSaveStructure` (2) | `CTaskSimpleRunAnim` (5) |
| `CCutsceneObject` (6) | `CSkidmarks` (7) | `CTaskSimpleRunTimedAnim` (7) |
| `CDamageAtomicModelInfo` (4) | `CSpecialFX` (7) | `CTaskSimpleSay` (6) |
| `CDamageManager` (27) | `CSpecialPlateHandler` (4) | `CTaskSimpleSetCharDecisionMaker` (6) |
| `CDarkel` (19) | `CSprite` (15) | `CTaskSimpleSetCharIgnoreWeaponRangeFlag` (6) |
| `CData` (2) | `CSprite2d` (33) | `CTaskSimpleSetKindaStayInSamePlace` (6) |
| `CDebug` (1) | `CStaticShadow` (1) | `CTaskSimpleSetPedAsAutoDriver` (6) |
| `CDecisionMakerTypesFileLoader` (7) 🔒 1 | `CStats` (32) | `CTaskSimpleShakeFist` (8) |
| `CDirectory` (10) | `CStencilShadowObject` (3) | `CTaskSimpleSitDown` (8) |
| `CDoor` (6) | `CStreamedScripts` (12) | `CTaskSimpleSitIdle` (7) |
| `CDraw` (2) | `CStreaming` (94) | `CTaskSimpleStandStill` (3) |
| `CdStream` (9) | `CStreamingInfo` (7) | `CTaskSimpleStandUp` (8) |
| `CDummy` (1) | `CStuckCarCheck` (8) | `CTaskSimpleStealthKill` (7) |
| `CDummyObject` (2) | `CStuntJumpManager` (7) | `CTaskSimpleSwim` (11) |
| `CDummyPool` (1) | `CTagManager` (19) | `CTaskSimpleThrowControl` (6) 🔒 1 |
| `CEntity` (70) | `CTask` (4) | `CTaskSimpleThrowProjectile` (9) |
| `CEntitySeekPosCalculatorFixedPos` (1) | `CTaskAllocator` (3) | `CTaskSimpleTired` (3) |
| `CEntitySeekPosCalculatorRadiusAngleOffset` (1) | `CTaskAllocatorAttack` (5) | `CTaskSimpleTurn180` (6) |
| `CEntitySeekPosCalculatorStandard` (1) | `CTaskAllocatorKillOnFoot` (4) | `CTaskSimpleUninterruptable` (1) |
| `CEntitySeekPosCalculatorXYOffset` (1) | `CTaskAllocatorKillThreatsBasicRandomGroup` (2) | `CTaskSimpleUseAtm` (4) |
| `CEntryExitManager` (18) 🔒 1 | `CTaskAllocatorKillThreatsDriveby` (5) | `CTaskSimpleUseGun` (19) |
| `CEntryInfoList` (1) | `CTaskAllocatorPlayerCommandAttack` (3) | `CTaskSimpleWaitForBus` (6) |
| `CEntryInfoNode` (2) 🔒 2 | `CTaskAllocatorPool` (1) | `CTaskSimpleWaitUntilAreaCodesMatch` (6) |
| `CEntryInfoNodePool` (1) | `CTaskComplexAttractorPartnerWait` (5) | `CTaskSimpleWaitUntilLeaderAreaCodesMatch` (4) |
| `CEscalator` (5) | `CTaskComplexAvoidOtherPedWhileWandering` (15) | `CTaskSimpleWaitUntilPedIsInCar` (6) |
| `CEscalators` (4) | `CTaskComplexBeInGroup` (10) | `CTaskSimpleWaitUntilPedIsOutCar` (6) |
| `CEvent` (3) | `CTaskComplexCarDrive` (11) | `CTempColModels` (2) |
| `CEventAcquaintancePed` (4) | `CTaskComplexCarDriveMission` (5) | `CText` (9) |
| `CEventAcquaintancePedHate` (1) | `CTaskComplexCarSlowBeDraggedOut` (11) | `CTheCarGenerators` (7) |
| `CEventAcquaintancePedHateBadlyLit` (2) | `CTaskComplexCopInCar` (9) | `CTheScripts` (69) |
| `CEventAreaCodes` (3) | `CTaskComplexDestroyCarArmed` (10) 🔒 2 | `CTheZones` (21) |
| `CEventAttractor` (4) | `CTaskComplexDestroyCarMelee` (10) | `CTimeCycle` (20) |
| `CEventBuildingCollision` (4) | `CTaskComplexDie` (5) | `CTimeInfo` (6) |
| `CEventCarUpsideDown` (2) | `CTaskComplexDieInCar` (6) | `CTimeModelInfo` (2) |
| `CEventCopCarBeingStolen` (2) | `CTaskComplexDiveFromAttachedEntityAndGetUp` (7) | `CTimer` (13) |
| `CEventDamage` (20) | `CTaskComplexDriveFireTruck` (6) | `CTrafficLights` (12) |
| `CEventDanger` (4) | `CTaskComplexDrivePointRoute` (7) | `CTrailer` (13) |
| `CEventDontJoinPlayerGroup` (1) | `CTaskComplexEnterAnyCarAsDriver` (5) | `cTransmission` (4) |
| `CEventEditableResponse` (11) | `CTaskComplexEnterBoatAsDriver` (8) | `CTxdStore` (26) |
| `CEventEscalator` (1) | `CTaskComplexEnterCar` (14) | `CUpsideDownCarCheck` (5) |
| `CEventGlobalGroup` (3) | `CTaskComplexEnterCarAsDriverTimed` (9) | `CUserDisplay` (2) |
| `CEventGroup` (12) | `CTaskComplexEnterCarAsPassengerTimed` (9) | `CVehicle` (142) 🔒 1 |
| `CEventGunShot` (5) | `CTaskComplexEvasiveDiveAndGetUp` (9) | `CVehicleAnimGroup` (5) |
| `CEventGunShotWhizzedBy` (2) | `CTaskComplexExtinguishFires` (8) | `CVehicleAnimGroupData` (5) |
| `CEventHandlerHistory` (10) | `CTaskComplexFacial` (9) | `CVehicleModelInfo` (75) |
| `CEventHitByWaterCannon` (2) | `CTaskComplexFallAndGetUp` (9) | `CVehiclePool` (1) |
| `CEventInAir` (2) | `CTaskComplexFallToDeath` (6) | `CVehicleRecording` (29) 🔒 1 |
| `CEventKnockOffBike` (8) | `CTaskComplexFleeAnyMeans` (8) | `CVisibilityPlugins` (84) |
| `CEventLeaderEnteredCarAsDriver` (2) | `CTaskComplexFleeEntity` (8) | `CWanted` (33) |
| `CEventLeaderEntryExit` (1) | `CTaskComplexFleePoint` (11) | `CWaterCannon` (5) |
| `CEventLeaderExitedCarAsDriver` (2) | `CTaskComplexFleeShooting` (8) | `CWaterCannons` (4) |
| `CEventLeaderQuitEnteringCarAsDriver` (2) | `CTaskComplexFollowLeaderAnyMeans` (8) | `CWeaponEffects` (9) |
| `CEventLeanOnVehicle` (2) | `CTaskComplexFollowNodeRoute` (19) | `CWeaponInfo` (10) |
| `CEventNewGangMember` (1) | `CTaskComplexFollowPedFootsteps` (1) | `CWeaponModelInfo` (3) |
| `CEventObjectCollision` (2) | `CTaskComplexFollowPointRoute` (13) | `CWindModifiers` (2) |
| `CEventOnFire` (1) | `CTaskComplexGangLeader` (13) 🔒 11 | `CWorld` (94) |
| `CEventPassObject` (2) | `CTaskComplexGetOnBoatSeat` (8) | `FurnitureGroup_c` (6) |
| `CEventPedCollisionWithPed` (3) | `CTaskComplexGetUpAndStandStill` (5) | `FurnitureManager_c` (7) |
| `CEventPedCollisionWithPlayer` (1) | `CTaskComplexGoToBoatSteeringWheel` (6) | `FurnitureSubGroup_c` (3) |
| `CEventPlayerCollisionWithPed` (1) | `CTaskComplexGoToCarDoorAndStandStill` (11) | `Fx_c` (8) |
| `CEventPlayerCommandToGroup` (2) | `CTaskComplexGoToPointAndStandStill` (7) | `FxEmitterPrt_c` (1) |
| `CEventPlayerCommandToGroupAttack` (2) | `CTaskComplexGoToPointAndStandStillTimed` (6) | `FxInfoManager_c` (4) |
| `CEventPlayerCommandToGroupGather` (1) | `CTaskComplexHitByGunResponse` (8) | `FxManager_c` (25) |
| `CEventPool` (1) | `CTaskComplexHitResponse` (7) | `FxMemoryPool_c` (5) |
| `CEventPotentialWalkIntoFire` (2) | `CTaskComplexInAirAndLand` (4) | `FxSphere_c` (4) |
| `CEventPotentialWalkIntoObject` (2) | `CTaskComplexInvestigateDeadPed` (9) | `FxSystemBP_c` (10) |
| `CEventPotentialWalkIntoPed` (3) | `CTaskComplexInvestigateDisturbance` (8) | `Game` (12) 🔒 3 |
| `CEventPotentialWalkIntoVehicle` (2) | `CTaskComplexInWater` (7) | `IKChain_c` (20) |
| `CEventScriptCommand` (8) | `CTaskComplexJump` (6) | `IKChainManager_c` (17) |
| `CEventScriptedAttractor` (1) | `CTaskComplexKillPedGroupOnFoot` (9) | `Input` (5) |
| `CEventSeenCop` (1) | `CTaskComplexKillPedOnFoot` (1) | `InteriorManager_c` (21) |
| `CEventSeenPanickedPed` (2) | `CTaskComplexKillPedOnFootMelee` (10) | `JPegPlugin` (5) |
| `CEventSoundQuiet` (3) | `CTaskComplexKillPedOnFootStealth` (7) | `Light` (27) |
| `CEventStuckInAir` (4) | `CTaskComplexLeaveAnyCar` (7) | `ModelIndices` (2) |
| `CEventVehicleCollision` (3) | `CTaskComplexLeaveBoat` (8) | `NodeNamePlugin` (3) |
| `CEventVehicleDamage` (5) | `CTaskComplexLeaveCarAndDie` (3) | `PipelinePlugin` (3) |
| `CEventVehicleDamageCollision` (2) | `CTaskComplexLeaveCarAndFlee` (8) | `ProcObjectMan_c` (8) |
| `CEventVehicleDamageWeapon` (2) | `CTaskComplexLeaveCarAsPassengerWait` (10) | `ProcSurfaceInfo_c` (4) |
| `CEventVehicleDied` (3) | `CTaskComplexMedicTreatInjuredPed` (9) | `Ps` (22) 🔒 2 |
| `CEventVehicleHitAndRun` (3) | `CTaskComplexMoveBackAndJump` (8) | `RenderBuffer` (4) |
| `CEventVehicleOnFire` (3) | `CTaskComplexObserveTrafficLightsAndAchieveHeading` (5) | `RpAnimBlend` (43) |
| `CEventVehicleToSteal` (2) | `CTaskComplexOnFire` (9) | `Rs` (27) |
| `CExplosion` (12) | `CTaskComplexPartner` (1) | `RtAnim` (1) |
| `CFallingGlassPane` (2) | `CTaskComplexPartnerChat` (1) | `RwCore` (4) |
| `CFileLoader` (47) | `CTaskComplexPartnerGreet` (7) | `RwHelper` (24) |
| `CFileMgr` (17) 🔒 13 | `CTaskComplexPartnerShove` (1) | `Securom` (2) |
| `CFire` (5) | `CTaskComplexPassObject` (9) | `SurfaceInfos_c` (47) |
| `CFireManager` (24) | `CTaskComplexPolicePursuit` (13) | `VideoMode` (4) |
| `CGame` (22) | `CTaskComplexReactToGunAimedAt` (8) | `VideoPlayer` (6) |
| `CGameLogic` (27) | `CTaskComplexRoadRage` (9) | `WaterCreature_c` (3) |
| `CGamma` (2) | `CTaskComplexSeekCoverUntilTargetDead` (10) | `WaterCreatureManager_c` (7) |
| `CGangs` (8) | `CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>` (9) | `Win` (6) 🔒 2 |
| `CGarage` (4) | `CTaskComplexSeekEntityAiming` (7) |  |
| `CGarages` (21) | `CTaskComplexSeekEntityAnyMeansXYOffset` (5) |  |

</details>

<a id="parciales"></a>
## 🟡 Parciales · 95 clases

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
| `CCarCtrl` | 21/22 | **1** | 95.45% | 1 |
| `CCarEnterExit` | 27/32 | **5** | 84.38% | 5 |
| `CCarFXRenderer` | 9/10 | **1** | 90.00% | 1 |
| `CClothesBuilder` | 15/23 | **8** | 65.22% | 8 |
| `CCustomBuildingRenderer` | 3/7 | **4** | 42.86% | 4 |
| `CEntityScanner` | 3/4 | **1** | 75.00% | 1 |
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
| `CPopCycle` | 14/15 | **1** | 93.33% | 1 |
| `CPopulation` | 64/66 | **2** | 96.97% | 2 |
| `CPostEffects` | 35/38 | **3** | 92.11% | 4 |
| `CProjectileInfo` | 1/12 | **11** | 8.33% | 11 |
| `CQuadBike` | 11/12 | **1** | 91.67% | 1 |
| `CRealTimeShadowManager` | 5/7 | **2** | 71.43% | 2 |
| `CRoadBlocks` | 7/8 | **1** | 87.50% | 1 |
| `CRope` | 5/7 | **2** | 71.43% | 2 |
| `CRopes` | 9/11 | **2** | 81.82% | 2 |
| `CRunningScript` | 24/39 | **15** | 61.54% | 15 |
| `CScriptResourceManager` | 1/4 | **3** | 25.00% | 3 |
| `CScriptsForBrains` | 3/6 | **3** | 50.00% | 3 |
| `CShadows` | 21/31 | **10** | 67.74% | 10 |
| `CStencilShadows` | 9/13 | **4** | 69.23% | 4 |
| `CTaskAllocatorKillThreatsBasic` | 5/6 | **1** | 83.33% | 1 |
| `CTaskComplexArrestPed` | 4/7 | **3** | 57.14% | 3 |
| `CTaskComplexCarSlowBeDraggedOutAndStandUp` | 3/9 | **6** | 33.33% | 6 |
| `CTaskComplexDestroyCar` | 8/9 | **1** | 88.89% | 1 |
| `CTaskComplexDragPedFromCar` | 1/2 | **1** | 50.00% | 1 |
| `CTaskComplexFollowLeaderInFormation` | 1/7 | **6** | 14.29% | 6 |
| `CTaskComplexGangJoinRespond` | 2/7 | **5** | 28.57% | 5 |
| `CTaskComplexGoToAttractor` | 2/5 | **3** | 40.00% | 3 |
| `CTaskComplexGotoDoorAndOpen` | 3/7 | **4** | 42.86% | 4 |
| `CTaskComplexGoToPointAiming` | 5/8 | **3** | 62.50% | 3 |
| `CTaskComplexGoToPointAndStandStillAndAchieveHeading` | 3/7 | **4** | 42.86% | 4 |
| `CTaskComplexGoToPointAnyMeans` | 3/7 | **4** | 42.86% | 4 |
| `CTaskComplexGoToPointShooting` | 1/3 | **2** | 33.33% | 2 |
| `CTaskComplexKillCriminal` | 9/11 | **2** | 81.82% | 2 |
| `CTaskComplexKillPedFromBoat` | 2/7 | **5** | 28.57% | 5 |
| `CTaskComplexKillPedOnFootArmed` | 9/11 | **2** | 81.82% | 2 |
| `CTaskComplexLeaveCarAndWander` | 2/8 | **6** | 25.00% | 6 |
| `CTaskComplexPlayHandSignalAnim` | 3/9 | **6** | 33.33% | 6 |
| `CTaskComplexSignalAtPed` | 4/7 | **3** | 57.14% | 3 |
| `CTaskComplexSmartFleePoint` | 2/12 | **10** | 16.67% | 10 |
| `CTaskComplexSunbathe` | 2/8 | **6** | 25.00% | 6 |
| `CTaskComplexUseClosestFreeScriptedAttractor` | 6/8 | **2** | 75.00% | 2 |
| `CTaskComplexWaitForBus` | 2/7 | **5** | 28.57% | 5 |
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
| `FxPrimBP_c` | 1/2 | **1** | 50.00% | 1 |
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
- **Locked:** indicador de bloqueo del hook; hay 677 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.
- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.

Actualizar los flags solo después de comparar con el original dentro del motor y regenerar desde la raíz:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.
