# Funciones pendientes del EXE, por clase

Generado de `docs/JuegoExe/funciones.csv` (07/11/2026). Total: **2,617** funciones en 521 clases/módulos (rango 0x401000–0x74FFFF): 2,595 sin reemplazar (A–D) + 22 envolturas pendientes. Ordenado de menor a mayor tamaño (bytes pendientes). Se resume en la [sección 11 de ReversedClasses](../ReversedClasses.md#parciales).

Estados: **A** llamada desde código ya en C++ · **B** método virtual · **C** solo la llaman funciones pendientes · **D** sin llamadas ni vtable · **envoltura** ya tiene hook pero llama al EXE.

Aviso: la clase se atribuye por cercanía de direcciones y puede ser errónea cuando la confianza es baja; un nombre de catálogo sí es fiable.

## Índice

| Clase | Funciones | Bytes |
|---|---:|---:|
| CAEPedlessSpeechAudioEntity | 1 | 3 |
| CTaskComplexGangJoinRespond | 1 | 6 |
| CTaskComplexPartner | 1 | 6 |
| CTaskManager | 1 | 6 |
| CFormation | 1 | 7 |
| CTaskSimpleSetCharDecisionMaker | 1 | 7 |
| CPedGroupDefaultTaskAllocatorFollowAnyMeans | 1 | 9 |
| CPedGroupDefaultTaskAllocatorFollowLimited | 1 | 9 |
| CPedGroupDefaultTaskAllocatorRandom | 1 | 9 |
| CPedGroupDefaultTaskAllocatorStandStill | 1 | 9 |
| CCrime | 1 | 10 |
| CCivilianPed | 1 | 11 |
| CDamageManager | 1 | 11 |
| CTaskComplexEnterCarAsDriver | 1 | 11 |
| CTaskComplexEnterCarAsPassenger | 1 | 11 |
| CTaskComplexFacial | 1 | 11 |
| CTaskComplexFollowNodeRoute | 1 | 11 |
| CTaskComplexPartnerDeal | 1 | 11 |
| CTaskSimpleAchieveHeading | 1 | 11 |
| CTaskSimpleGoTo | 1 | 11 |
| CTaskSimpleSetStayInSamePlace | 1 | 11 |
| BoneNode_c | 1 | 12 |
| CdStream | 2 | 13 |
| CUserDisplay | 1 | 15 |
| CEventCarUpsideDown | 3 | 15 |
| CEventCopCarBeingStolen | 3 | 15 |
| CEventPedCollisionWithPed | 3 | 15 |
| CEventVehicleToSteal | 3 | 15 |
| CGenericGameStorage | 1 | 16 |
| CTaskComplexFallToDeath | 2 | 17 |
| CTaskComplexGetUpAndStandStill | 2 | 17 |
| CEventPlayerCommandToGroup | 4 | 17 |
| CTaskSimpleChat | 1 | 18 |
| CEventAcquaintancePedHate | 4 | 18 |
| CFileMgr | 1 | 19 |
| CTaskComplexUseGoggles | 2 | 19 |
| CEventPotentialWalkIntoPed | 4 | 19 |
| CEventSeenPanickedPed | 4 | 19 |
| CEventPotentialWalkIntoVehicle | 4 | 20 |
| CEventVehicleDied | 4 | 20 |
| CTaskSimpleCarWaitToSlowDown | 1 | 21 |
| CPlayerInfo | 3 | 21 |
| CCarAI | 1 | 22 |
| CEventInAir | 4 | 22 |
| CTaskComplexDie | 3 | 23 |
| CEventVehicleDamage | 4 | 23 |
| CLodTimeModelInfo | 2 | 25 |
| CTimeCycle | 1 | 26 |
| CMemoryMgr | 4 | 26 |
| BreakObject_c | 1 | 27 |
| CAEVorbisDecoder | 1 | 27 |
| CAEWaveDecoder | 1 | 27 |
| CPlayerPedData | 1 | 27 |
| CRealTimeShadowManager | 1 | 27 |
| CEventGunShotWhizzedBy | 5 | 29 |
| CBoat | 1 | 30 |
| CCutsceneObject | 1 | 30 |
| CDummyObject | 1 | 30 |
| CMonsterTruck | 1 | 30 |
| CTaskComplexBeCop | 1 | 30 |
| CTaskComplexBeInGroup | 1 | 30 |
| CTaskComplexBeStill | 1 | 30 |
| CTaskComplexCarDriveMissionFleeScene | 1 | 30 |
| CTaskComplexCarDriveMissionKillPed | 1 | 30 |
| CTaskComplexClimb | 1 | 30 |
| CTaskComplexDestroyCar | 1 | 30 |
| CTaskComplexDestroyCarArmed | 1 | 30 |
| CTaskComplexDestroyCarMelee | 1 | 30 |
| CTaskComplexDragPedFromCar | 1 | 30 |
| CTaskComplexEnterBoatAsDriver | 1 | 30 |
| CTaskComplexEnterCarAsDriverTimed | 1 | 30 |
| CTaskComplexEnterCarAsPassengerTimed | 1 | 30 |
| CTaskComplexEnterCarAsPassengerWait | 1 | 30 |
| CTaskComplexEvasiveDiveAndGetUp | 1 | 30 |
| CTaskComplexExtinguishFires | 1 | 30 |
| CTaskComplexGangLeader | 1 | 30 |
| CTaskComplexGoToCarDoorAndStandStill | 1 | 30 |
| CTaskComplexInvestigateDeadPed | 1 | 30 |
| CTaskComplexKillCriminal | 1 | 30 |
| CTaskComplexKillPedGroupOnFoot | 1 | 30 |
| CTaskComplexKillPedOnFootMelee | 1 | 30 |
| CTaskComplexLeaveAnyCar | 1 | 30 |
| CTaskComplexLeaveCarAndFlee | 1 | 30 |
| CTaskComplexLeaveCarAndWander | 1 | 30 |
| CTaskComplexLeaveCarAsPassengerWait | 1 | 30 |
| CTaskComplexMoveBackAndJump | 1 | 30 |
| CTaskComplexPolicePursuit | 1 | 30 |
| CTaskComplexShuffleSeats | 1 | 30 |
| CTaskComplexTrackEntity | 1 | 30 |
| CTaskComplexUseAttractorPartner | 1 | 30 |
| CTaskComplexUseClosestFreeScriptedAttractorRun | 1 | 30 |
| CTaskComplexUseClosestFreeScriptedAttractorSprint | 1 | 30 |
| CTaskComplexUseScriptedAttractor | 1 | 30 |
| CTaskComplexWaitForBackup | 1 | 30 |
| CTaskComplexWalkRoundCar | 1 | 30 |
| CTaskInteriorBeInHouse | 1 | 30 |
| CTaskInteriorGoToInfo | 1 | 30 |
| CTaskInteriorLieInBed | 1 | 30 |
| CTaskInteriorShopKeeper | 1 | 30 |
| CTaskInteriorSitAtDesk | 1 | 30 |
| CTaskSimpleBikeJacked | 1 | 30 |
| CTaskSimpleCarAlign | 1 | 30 |
| CTaskSimpleCarCloseDoorFromInside | 1 | 30 |
| CTaskSimpleCarCloseDoorFromOutside | 1 | 30 |
| CTaskSimpleCarDrive | 1 | 30 |
| CTaskSimpleCarDriveTimed | 1 | 30 |
| CTaskSimpleCarGetIn | 1 | 30 |
| CTaskSimpleCarOpenDoorFromOutside | 1 | 30 |
| CTaskSimpleCarOpenLockedDoorFromOutside | 1 | 30 |
| CTaskSimpleCarSetPedInAsPassenger | 1 | 30 |
| CTaskSimpleCarSlowDragPedOut | 1 | 30 |
| CTaskSimpleEvasiveDive | 1 | 30 |
| CTaskSimpleGunControl | 1 | 30 |
| CTaskSimpleKillPedWithCar | 1 | 30 |
| CTaskSimpleNone | 1 | 30 |
| CTaskSimplePickUpBike | 1 | 30 |
| CTaskSimplePlayerOnFire | 1 | 30 |
| CTaskSimpleSay | 1 | 30 |
| CTaskSimpleSitDown | 1 | 30 |
| CTaskSimpleSitIdle | 1 | 30 |
| CTaskSimpleStandUp | 1 | 30 |
| CRoadBlocks | 1 | 31 |
| CLodAtomicModelInfo | 2 | 31 |
| CEventEscalator | 1 | 32 |
| CPhysical | 2 | 32 |
| CAECollisionAudioEntity | 1 | 33 |
| CTrailer | 2 | 33 |
| CAnimatedBuilding | 2 | 35 |
| CBridge | 7 | 35 |
| CTaskComplexArrestPed | 2 | 36 |
| CTaskComplexCarDrive | 2 | 36 |
| CTaskGangHassleVehicle | 2 | 36 |
| CTaskComplexUseEffect | 1 | 39 |
| CCustomBuildingDNPipeline | 2 | 39 |
| CDirectory | 2 | 39 |
| CPedType | 2 | 40 |
| CTaskComplexObserveTrafficLights | 2 | 40 |
| CQuadBike | 2 | 41 |
| CTaskAllocatorKillThreatsBasicRandomGroup | 2 | 41 |
| CTaskComplexAttractorPartnerWait | 2 | 41 |
| CTaskComplexDriveToPoint | 2 | 41 |
| CTaskComplexEnterAnyCarAsDriver | 2 | 41 |
| CTaskComplexGoToPointAndStandStillTimed | 2 | 41 |
| CTaskComplexUseEffectSprinting | 2 | 41 |
| CTaskComplexWanderMedic | 2 | 41 |
| CTaskComplexWanderStandard | 2 | 41 |
| CTaskSimpleFinishBrain | 2 | 41 |
| CTaskSimpleTogglePedThreatScanner | 2 | 41 |
| CTimeModelInfo | 2 | 41 |
| ProcObjectMan_c | 2 | 41 |
| CAEStaticChannel | 3 | 41 |
| CTaskComplexReactToGunAimedAt | 2 | 42 |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | 2 | 42 |
| CEventDanger | 4 | 43 |
| RwHelper | 3 | 45 |
| C2dEffect | 3 | 46 |
| CEventGunShot | 7 | 46 |
| CGeneral | 1 | 47 |
| CEventVehicleDamageCollision | 3 | 47 |
| CEventVehicleDamageWeapon | 3 | 47 |
| CTaskComplexTreatAccident | 3 | 47 |
| CAEMP3BankLoader | 1 | 48 |
| CTaskSimpleJetPack | 2 | 48 |
| CText | 2 | 48 |
| CTaskAllocatorAttack | 1 | 50 |
| CWorld | 1 | 51 |
| CAEScriptAudioEntity | 1 | 52 |
| CCollisionData | 1 | 52 |
| cBuoyancy | 1 | 54 |
| CPedPool | 1 | 54 |
| CAEAudioHardware | 2 | 54 |
| CConversations | 1 | 55 |
| CTheCarGenerators | 3 | 55 |
| FxManager_c | 5 | 55 |
| CEventAreaCodes | 4 | 56 |
| CShadowCamera | 1 | 57 |
| CSprite | 1 | 57 |
| CAEPedAudioEntity | 2 | 57 |
| CTaskSimpleTired | 2 | 57 |
| CColLine | 1 | 58 |
| CVehicleAnimGroup | 2 | 58 |
| CAudioEngine | 3 | 58 |
| CAEWMADecoder | 2 | 59 |
| CTaskComplexCarSlowBeDraggedOut | 2 | 60 |
| CTaskComplexFleeAnyMeans | 2 | 60 |
| CTaskComplexFollowPointRoute | 2 | 60 |
| CTaskComplexHitResponse | 2 | 60 |
| CTaskComplexKillPedOnFootArmed | 2 | 60 |
| CTaskComplexOnFire | 2 | 60 |
| CTaskComplexPassObject | 2 | 60 |
| CTaskComplexUseAttractor | 2 | 60 |
| CTaskComplexWanderCriminal | 2 | 60 |
| CTaskInteriorBeInOffice | 2 | 60 |
| CTaskSimpleCreateCarAndGetIn | 2 | 60 |
| CTaskSimpleEvasiveStep | 2 | 60 |
| CAEMFDecoder | 3 | 60 |
| CRealTimeShadow | 3 | 60 |
| CVehicleAnimGroupData | 1 | 61 |
| CBuildingPool | 2 | 62 |
| CDummyPool | 2 | 62 |
| CNodeRoutePool | 2 | 62 |
| CObjectData | 2 | 62 |
| CAEBankLoader | 1 | 63 |
| CTimeInfo | 1 | 63 |
| CEventGlobalGroup | 3 | 63 |
| CEntryInfoList | 1 | 65 |
| CPedShelterAttractor | 2 | 65 |
| CTaskComplexGoToPointAiming | 3 | 65 |
| CTaskComplexSequence | 4 | 65 |
| CTaskComplexUseSequence | 3 | 66 |
| FxSystem_c | 4 | 66 |
| CRadar | 1 | 67 |
| CTaskComplexDiveFromAttachedEntityAndGetUp | 3 | 67 |
| CTaskSimpleCower | 4 | 70 |
| FxPrimBP_c | 5 | 70 |
| CIplDefPool | 1 | 71 |
| CPedGroupIntelligence | 2 | 71 |
| CTaskComplexCarDriveWander | 3 | 71 |
| CTaskSimpleGoToPoint | 3 | 71 |
| CPedGroups | 2 | 72 |
| CPedIK | 2 | 72 |
| CWanted | 1 | 76 |
| CCollision | 2 | 76 |
| CTaskComplexInAirAndLand | 4 | 77 |
| CTaskSimplePause | 4 | 77 |
| CDummy | 1 | 78 |
| CTaskSimpleShakeFist | 2 | 78 |
| Securom | 2 | 78 |
| CTaskComplexDrivePointRoute | 3 | 78 |
| CTaskSequences | 1 | 79 |
| CTagManager | 1 | 80 |
| CTaskComplexUseEffectRunning | 3 | 80 |
| CEventBuildingCollision | 6 | 82 |
| CBuilding | 2 | 83 |
| CPedGroupDefaultTaskAllocatorChat | 2 | 86 |
| CTaskSimpleAffectSecondaryBehaviour | 1 | 87 |
| CAEAudioChannel | 2 | 88 |
| CAEStreamingChannel | 2 | 88 |
| CBaseModelInfo | 3 | 88 |
| CRect | 5 | 88 |
| CTaskSimpleCarSetPedOut | 1 | 90 |
| CTaskComplexPlayHandSignalAnim | 3 | 90 |
| CTaskSimpleCarShuffle | 3 | 90 |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | 2 | 91 |
| CGangs | 1 | 92 |
| CGarage | 2 | 92 |
| CTaskComplexUsePairedAttractor | 3 | 94 |
| CColModel | 1 | 96 |
| CTaskComplexKillPedOnFoot | 2 | 96 |
| CTaskSimpleGiveCPR | 2 | 99 |
| CTaskSimpleHitHead | 2 | 99 |
| CInformGroupEvent | 2 | 101 |
| CTaskComplexPartnerShove | 4 | 101 |
| CTaskSimpleFacial | 4 | 101 |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | 2 | 105 |
| CPointRoutePool | 1 | 106 |
| CTaskSimplePutDownEntity | 2 | 106 |
| JPegPlugin | 1 | 108 |
| CTaskComplexWalkAlongsidePed | 1 | 110 |
| CInterestingEvents | 2 | 110 |
| CPlayerRelationshipRecorder | 3 | 111 |
| CTaskSimpleHandsUp | 4 | 113 |
| CTaskSimpleInAir | 2 | 115 |
| CScripted2dEffects | 1 | 116 |
| App | 6 | 116 |
| CEventPotentialWalkIntoObject | 6 | 116 |
| CCoronas | 1 | 117 |
| CTaskComplexScreamInCarThenLeave | 2 | 118 |
| CBmx | 3 | 118 |
| CTaskSimpleCarSetPedInAsDriver | 2 | 120 |
| CTaskComplexLeaveCarAndDie | 4 | 120 |
| CEventStuckInAir | 5 | 120 |
| CAnimBlendStaticAssociation | 2 | 121 |
| CAEVehicleAudioEntity | 5 | 121 |
| CTaskComplexPartnerChat | 2 | 123 |
| CColSphere | 3 | 124 |
| CEventPool | 2 | 126 |
| CTaskComplexUseSwatRope | 2 | 126 |
| CTaskGangHasslePed | 5 | 126 |
| CTaskSimpleDead | 1 | 127 |
| CTaskSimpleFall | 3 | 129 |
| CEventHandler | 2 | 130 |
| CEventObjectCollision | 5 | 130 |
| CCompressedMatrixNotAligned | 2 | 131 |
| CAEGlobalWeaponAudioEntity | 2 | 135 |
| CTaskSimpleUseGun | 2 | 139 |
| CTaskSimpleCarSlowBeDraggedOut | 3 | 141 |
| CEventLeaderExitedCarAsDriver | 7 | 141 |
| CTaskLeanOnVehicle | 3 | 142 |
| VideoPlayer | 2 | 145 |
| CTaskSimpleJump | 3 | 145 |
| CEventLeaderQuitEnteringCarAsDriver | 8 | 146 |
| CTaskComplexFleePoint | 5 | 150 |
| CTaskComplexSitDownThenIdleThenStandUp | 5 | 150 |
| CTaskSimpleIKChain | 5 | 150 |
| CEventPlayerCommandToGroupAttack | 6 | 151 |
| CLoadedCarGroup | 1 | 152 |
| CTaskSimpleClimb | 3 | 152 |
| CEventLeaderEntryExit | 9 | 153 |
| CTaskSimpleUninterruptable | 9 | 154 |
| CTaskSimpleGoToPointFine | 4 | 155 |
| CTaskSimpleCarJumpOut | 6 | 156 |
| CDecision | 1 | 157 |
| CFileLoader | 6 | 158 |
| CTaskSimpleDie | 4 | 159 |
| CTaskSimpleGetUp | 4 | 159 |
| CTaskSimpleLand | 4 | 159 |
| CIplStore | 3 | 160 |
| CTaskSimplePlayerOnFoot | 3 | 160 |
| CTaskComplexWaitForBus | 4 | 160 |
| CTaskComplexGoToAttractor | 5 | 162 |
| BreakManager_c | 3 | 164 |
| CMatrixLinkList | 2 | 166 |
| CTaskComplexProstituteSolicit | 3 | 166 |
| CAtomicModelInfo | 4 | 167 |
| CEventPedCollisionWithPlayer | 4 | 167 |
| CEventPlayerCollisionWithPed | 4 | 167 |
| CBike | 22 | 168 |
| CTaskSimpleStealthKill | 3 | 169 |
| CAEDataStream | 3 | 170 |
| CAETwinLoopSoundEntity | 2 | 174 |
| CTaskComplexJump | 5 | 175 |
| CAEAudioEntity | 9 | 176 |
| CAnimBlendAssociation | 4 | 177 |
| CTaskComplexEnterCar | 6 | 177 |
| CLoadingScreen | 5 | 180 |
| CTaskComplexSeekCoverUntilTargetDead | 4 | 184 |
| CTaskComplexFollowPedFootsteps | 3 | 185 |
| D3DResourceSystem | 3 | 188 |
| FxMemoryPool_c | 2 | 194 |
| CTaskSimpleRunAnim | 2 | 197 |
| CTaskComplexSeekEntityStandard | 4 | 197 |
| CTaskComplexDriveFireTruck | 7 | 202 |
| CPedList | 2 | 203 |
| CCutsceneMgr | 3 | 204 |
| IKChain_c | 2 | 205 |
| CVisibilityPlugins | 7 | 205 |
| CMouseControllerState | 1 | 208 |
| CGamma | 1 | 210 |
| CDamageAtomicModelInfo | 5 | 210 |
| WaterCreature_c | 2 | 211 |
| CMenuManager | 5 | 214 |
| CTaskSimpleFightingControl | 2 | 215 |
| CAECutsceneTrackManager | 4 | 219 |
| CAESoundManager | 3 | 220 |
| CTaskSimpleFight | 2 | 223 |
| CTaskComplexWanderProstitute | 3 | 223 |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | 2 | 229 |
| CEventDontJoinPlayerGroup | 6 | 230 |
| CEventNewGangMember | 6 | 230 |
| CEventLeanOnVehicle | 6 | 234 |
| CEventHitByWaterCannon | 9 | 234 |
| CTaskSimpleRunNamedAnim | 6 | 235 |
| CTaskSimpleWaitForBus | 6 | 239 |
| CTaskSimpleStandStill | 6 | 241 |
| CAnimBlendHierarchy | 3 | 244 |
| CEventLeaderEnteredCarAsDriver | 9 | 244 |
| CCheat | 2 | 246 |
| CMaths | 4 | 246 |
| CTaskComplexGoToBoatSteeringWheel | 4 | 249 |
| CFont | 3 | 251 |
| CTaskComplexCopInCar | 4 | 252 |
| CWaterLevel | 3 | 253 |
| CTaskSimpleGangDriveBy | 2 | 254 |
| CClumpModelInfo | 4 | 255 |
| Common | 3 | 260 |
| CPedClothesDesc | 1 | 263 |
| CAEWeatherAudioEntity | 4 | 264 |
| PipelinePlugin | 1 | 273 |
| CTaskComplexWalkRoundObject | 3 | 273 |
| CAudioZones | 4 | 275 |
| CDecisionMakerTypes | 2 | 278 |
| CTaskComplexBeInCouple | 2 | 279 |
| CEvent | 8 | 280 |
| CPedPlacement | 1 | 283 |
| CEventPassObject | 10 | 291 |
| CStuntJumpManager | 4 | 296 |
| cHandlingDataMgr | 6 | 299 |
| Input | 2 | 302 |
| CTaskSimpleHoldEntity | 8 | 302 |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | 3 | 304 |
| CTaskInteriorSitInChair | 4 | 304 |
| FurnitureManager_c | 3 | 306 |
| CAEWaterCannonAudioEntity | 2 | 309 |
| CPedDamageResponseCalculator | 3 | 319 |
| CTaskComplexHitPedWithCar | 4 | 319 |
| CStats | 2 | 320 |
| CTaskComplexOpenPassengerDoor | 4 | 327 |
| CTaskComplexSmartFleeEntity | 1 | 328 |
| CTaskGoToVehicleAndLean | 3 | 331 |
| CEntitySeekPosCalculatorFixedPos | 1 | 335 |
| CCopPed | 8 | 335 |
| CTask | 7 | 336 |
| CPedGeometryAnalyser | 4 | 338 |
| InteriorManager_c | 2 | 340 |
| CQuaternion | 3 | 341 |
| CEventAcquaintancePedHateBadlyLit | 8 | 342 |
| CPlane | 3 | 346 |
| CWeather | 2 | 352 |
| CTaskAllocatorPool | 7 | 362 |
| CTaskComplexGangFollower | 3 | 365 |
| CEventPlayerCommandToGroupGather | 8 | 372 |
| CColStore | 5 | 373 |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | 2 | 382 |
| VideoMode | 2 | 382 |
| CTaskComplexWalkRoundFire | 9 | 385 |
| CHandObject | 4 | 386 |
| CAEAmbienceTrackManager | 5 | 386 |
| CTaskSimpleCarFallOut | 5 | 388 |
| CTaskComplexGotoDoorAndOpen | 6 | 388 |
| CTxdStore | 6 | 388 |
| CEventKnockOffBike | 7 | 395 |
| CPopulation | 8 | 399 |
| CTaskSimpleDieInCar | 5 | 403 |
| CTaskSimpleSwim | 5 | 406 |
| CTaskComplexDieInCar | 15 | 406 |
| CVehicleRecording | 3 | 417 |
| CPedIntelligence | 2 | 419 |
| CTaskComplexWalkRoundBuildingAttempt | 4 | 421 |
| CTheZones | 5 | 422 |
| CGame | 9 | 428 |
| CTaskSimpleHitFromFront | 21 | 430 |
| CAnimManager | 6 | 434 |
| Fx_c | 9 | 434 |
| CPtrNodeDoubleLinkPool | 7 | 436 |
| CTaskComplexWanderCop | 9 | 442 |
| CObjectPool | 5 | 446 |
| COnscreenCounterEntry | 3 | 450 |
| CTaskComplexSeekEntityAiming | 6 | 451 |
| CTaskComplexPartnerGreet | 9 | 458 |
| Game | 3 | 460 |
| CShopping | 5 | 460 |
| CTaskSimpleSetCharIgnoreWeaponRangeFlag | 1 | 481 |
| CPlantMgr | 3 | 484 |
| CTaskSimpleDuckToggle | 3 | 488 |
| ProcSurfaceInfo_c | 4 | 489 |
| CEventAttractor | 8 | 490 |
| CTaskComplexStealCar | 7 | 495 |
| CPlayerPed | 3 | 496 |
| CTaskSimpleDuck | 8 | 496 |
| CEventPotentialWalkIntoFire | 12 | 514 |
| CTaskSimpleIKPointArm | 6 | 516 |
| CCamera | 1 | 527 |
| CEventScriptedAttractor | 13 | 527 |
| CPedAttractorPool | 12 | 530 |
| CStreaming | 17 | 535 |
| CStencilShadows | 5 | 543 |
| CTaskComplexRoadRage | 9 | 585 |
| CDecisionMakerTypesFileLoader | 1 | 606 |
| CEventEditableResponse | 14 | 607 |
| Rs | 4 | 661 |
| CEntryExitManager | 11 | 675 |
| CTaskComplexFallAndGetUp | 13 | 719 |
| CQuadTreeNode<T> | 10 | 720 |
| CObject | 7 | 728 |
| CEventScriptCommand | 11 | 766 |
| CTaskSimpleThrowProjectile | 11 | 767 |
| CTaskComplexUseWaterCannon | 15 | 769 |
| CTrafficLights | 5 | 770 |
| CTaskComplexWaitForPed | 6 | 772 |
| CTaskComplexWander | 4 | 797 |
| CModelInfo | 22 | 814 |
| CCustomCarPlateMgr | 1 | 829 |
| CShadows | 1 | 837 |
| CVehicle | 33 | 861 |
| CVehicleSaveStructure | 4 | 874 |
| Ps | 6 | 876 |
| CPad | 11 | 895 |
| CEventSeenCop | 35 | 902 |
| CTaskComplexSunbathe | 6 | 935 |
| CTaskComplexMedicTreatInjuredPed | 13 | 974 |
| CTaskComplexGoToPointAndStandStill | 8 | 977 |
| CTaskSimpleBeHit | 8 | 1022 |
| CCam | 7 | 1059 |
| CTaskComplexStuckInAir | 11 | 1075 |
| CTrain | 6 | 1076 |
| CPed | 7 | 1093 |
| CTaskComplexInvestigateDisturbance | 9 | 1125 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 4 | 1138 |
| CEventVehicleOnFire | 17 | 1145 |
| CEventVehicleCollision | 15 | 1149 |
| CPlaceable | 25 | 1157 |
| Interior_c | 4 | 1200 |
| CTaskSimpleRunTimedAnim | 13 | 1251 |
| CAttractorScanner | 3 | 1273 |
| CClothesBuilder | 13 | 1285 |
| CVehicleModelInfo | 15 | 1356 |
| CTaskComplexAvoidOtherPedWhileWandering | 10 | 1382 |
| CEntityScanner | 8 | 1443 |
| CTaskInteriorUseInfo | 3 | 1483 |
| CPedIntelligencePool | 6 | 1552 |
| CEventDamage | 22 | 1596 |
| CTaskComplexWanderGang | 6 | 1750 |
| CPedAttractor | 28 | 1774 |
| CTaskSimpleThrowControl | 14 | 1802 |
| CEventVehicleHitAndRun | 25 | 1806 |
| CTaskComplexUseClosestFreeScriptedAttractor | 14 | 1876 |
| CCarCtrl | 4 | 1928 |
| CCustomRoadsignMgr | 1 | 1939 |
| CPathFind | 14 | 2141 |
| SurfaceInfos_c | 2 | 2174 |
| CCullZones | 13 | 2476 |
| CAEUserRadioTrackManager | 30 | 2533 |
| CTaskComplexTurnToFaceEntityOrCoord | 14 | 2607 |
| CTaskComplexGoToPointAnyMeans | 6 | 2633 |
| CCustomCarEnvMapPipeline | 25 | 3547 |
| CPools | 52 | 3733 |
| CAutomobile | 11 | 3922 |
| CHeli | 6 | 4141 |
| CReplay | 10 | 4172 |
| CRunningScript | 4 | 5919 |
| RpAnimBlend | 16 | 6102 |
| CScriptsForBrains | 4 | 6307 |
| FxInfoManager_c | 116 | 6591 |
| CPedAttractorManager | 71 | 8097 |
| CTaskComplexSeekEntityRadiusAngleOffset | 5 | 8771 |
| CTheScripts | 14 | 9864 |
| CEntity | 43 | 10300 |
| CWeaponModelInfo | 41 | 13554 |
| Win | 85 | 19907 |
| FxEmitterBP_c | 141 | 28395 |

## CAEPedlessSpeechAudioEntity (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004E6080 | 3 | B |  | 0 | 0 | sí | alta |

## CTaskComplexGangJoinRespond (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00661710 | 6 | B |  | 0 | 0 | sí | alta |

## CTaskComplexPartner (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00681F10 | 6 | B |  | 0 | 0 | sí | baja |

## CTaskManager (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00681DF0 | 6 | B |  | 0 | 0 | sí | baja |

## CFormation (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0069BB40 | 7 | C |  | 8 | 0 |  | baja |

## CTaskSimpleSetCharDecisionMaker (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046A610 | 7 | C |  | 3 | 0 |  | baja |

## CPedGroupDefaultTaskAllocatorFollowAnyMeans (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6480 | 9 | C |  | 1 | 0 |  | baja |

## CPedGroupDefaultTaskAllocatorFollowLimited (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F64B0 | 9 | C |  | 1 | 0 |  | baja |

## CPedGroupDefaultTaskAllocatorRandom (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6540 | 9 | C |  | 1 | 0 |  | baja |

## CPedGroupDefaultTaskAllocatorStandStill (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F64E0 | 9 | C |  | 1 | 0 |  | baja |

## CCrime (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00532240 | 10 | A |  | 1 | 1 |  | baja |

## CCivilianPed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DDBE0 | 11 | A |  | 1 | 1 |  | alta |

## CDamageManager (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006C2740 | 11 | C |  | 1 | 0 |  | baja |

## CTaskComplexEnterCarAsDriver (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640330 | 11 | C |  | 1 | 0 |  | baja |

## CTaskComplexEnterCarAsPassenger (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640380 | 11 | C |  | 3 | 0 |  | baja |

## CTaskComplexFacial (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00690D90 | 11 | C |  | 1 | 0 |  | alta |

## CTaskComplexFollowNodeRoute (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00669D20 | 11 | A |  | 8 | 3 |  | baja |

## CTaskComplexPartnerDeal (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00684200 | 11 | A |  | 3 | 1 |  | baja |

## CTaskSimpleAchieveHeading (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00667E70 | 11 | C |  | 1 | 0 |  | alta |

## CTaskSimpleGoTo (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00667A00 | 11 | C |  | 2 | 0 |  | alta |

## CTaskSimpleSetStayInSamePlace (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062F5D0 | 11 | C |  | 3 | 0 |  | alta |

## BoneNode_c (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00617850 | 12 | D |  | 0 | 0 |  | baja |

## CdStream (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00406450 | 6 | A |  | 1 | 1 |  | alta |
| 0x00406D10 | 7 | A |  | 5 | 5 |  | baja |

## CUserDisplay (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005720C0 | 15 | A |  | 2 | 2 |  | baja |

## CEventCarUpsideDown (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1D20 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B1D30 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B1D40 | 6 | B |  | 0 | 0 | sí | alta |

## CEventCopCarBeingStolen (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B17C0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B17D0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B17E0 | 6 | B |  | 0 | 0 | sí | alta |

## CEventPedCollisionWithPed (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ACA40 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004ACA50 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004ACA60 | 6 | B |  | 0 | 0 | sí | alta |

## CEventVehicleToSteal (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AF6D0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AF6E0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AF6F0 | 6 | B |  | 0 | 0 | sí | alta |

## CGenericGameStorage (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D0F40 | 16 | A |  | 1 | 1 |  | alta |

## CTaskComplexFallToDeath (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006790A0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006790B0 | 11 | C |  | 1 | 0 |  | alta |

## CTaskComplexGetUpAndStandStill (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00678150 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00678160 | 11 | C |  | 1 | 0 |  | alta |

## CEventPlayerCommandToGroup (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B2440 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B2450 | 5 | B |  | 0 | 0 | sí | alta |
| 0x004B2460 | 4 | B |  | 0 | 0 | sí | alta |
| 0x004B2520 | 5 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleChat (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6660 | 18 | D |  | 0 | 0 |  | baja |

## CEventAcquaintancePedHate (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00420E90 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00420EA0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00420EB0 | 7 | B |  | 0 | 0 | sí | baja |
| 0x00420EC0 | 5 | B |  | 0 | 0 | sí | baja |

## CFileMgr (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00538AD0 | 19 | A |  | 1 | 1 |  | baja |

## CTaskComplexUseGoggles (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00634F20 | 11 | C |  | 1 | 0 |  | alta |
| 0x00634F30 | 8 | B |  | 0 | 0 | sí | alta |

## CEventPotentialWalkIntoPed (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AE760 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AE770 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AE780 | 4 | B |  | 0 | 0 | sí | alta |
| 0x004AE790 | 6 | B |  | 0 | 0 | sí | alta |

## CEventSeenPanickedPed (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B20F0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B2100 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B2110 | 4 | B |  | 0 | 0 | sí | baja |
| 0x004B2120 | 6 | B |  | 0 | 0 | sí | baja |

## CEventPotentialWalkIntoVehicle (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AE390 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AE3A0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AE3B0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AE5C0 | 5 | B |  | 0 | 0 | sí | baja |

## CEventVehicleDied (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B0D70 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B0D80 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B0D90 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B0EE0 | 5 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarWaitToSlowDown (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00646AB0 | 21 | B |  | 0 | 0 | sí | alta |

## CPlayerInfo (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0056F200 | 11 | A |  | 3 | 2 |  | baja |
| 0x00571970 | 5 | C |  | 1 | 0 |  | baja |
| 0x00571980 | 5 | C |  | 1 | 0 |  | baja |

## CCarAI (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00420860 | 22 | D |  | 0 | 0 |  | baja |

## CEventInAir (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B0CD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B0CE0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B0CF0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B0D00 | 7 | A |  | 3 | 1 |  | baja |

## CTaskComplexDie (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006300B0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006300C0 | 11 | C |  | 1 | 0 |  | alta |
| 0x00630580 | 6 | B |  | 0 | 0 | sí | baja |

## CEventVehicleDamage (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1960 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B1970 | 7 | B |  | 0 | 0 | sí | alta |
| 0x004B1980 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B1B50 | 7 | C |  | 1 | 0 |  | baja |

## CLodTimeModelInfo (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C56E0 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C56F0 | 18 | D |  | 0 | 0 |  | baja |

## CTimeCycle (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005617A0 | 26 | A |  | 1 | 1 |  | baja |

## CMemoryMgr (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0072F460 | 19 | A |  | 3 | 3 |  | alta |
| 0x0072F4A0 | 1 | A |  | 2 | 1 |  | alta |
| 0x0072F4B0 | 1 | A |  | 2 | 1 |  | alta |
| 0x0072F500 | 5 | A |  | 2 | 2 |  | baja |

## BreakObject_c (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059E620 | 27 | C |  | 1 | 0 |  | baja |

## CAEVorbisDecoder (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00502700 | 27 | B |  | 0 | 0 | sí | baja |

## CAEWaveDecoder (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00503430 | 27 | B |  | 0 | 0 | sí | baja |

## CPlayerPedData (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0056F850 | 27 | D |  | 0 | 0 |  | alta |

## CRealTimeShadowManager (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00706850 | 27 | D |  | 0 | 0 |  | alta |

## CEventGunShotWhizzedBy (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B2030 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B2040 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B2050 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B2060 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B2070 | 11 | A |  | 5 | 2 |  | baja |

## CBoat (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006F1120 | 30 | B |  | 0 | 0 | sí | alta |

## CCutsceneObject (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005B1DE0 | 30 | B |  | 0 | 0 | sí | alta |

## CDummyObject (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059EB50 | 30 | B |  | 0 | 0 | sí | alta |

## CMonsterTruck (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006C7F90 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexBeCop (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004994C0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexBeInGroup (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00638AC0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexBeStill (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F66E0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexCarDriveMissionFleeScene (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B89D0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexCarDriveMissionKillPed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B8AB0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexClimb (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046A6C0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexDestroyCar (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006289D0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexDestroyCarArmed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00629060 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexDestroyCarMelee (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00628C60 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexDragPedFromCar (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640590 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexEnterBoatAsDriver (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640F10 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexEnterCarAsDriverTimed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640640 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexEnterCarAsPassengerTimed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640710 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexEnterCarAsPassengerWait (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640BC0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexEvasiveDiveAndGetUp (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006560C0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexExtinguishFires (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00697820 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGangLeader (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006629F0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGoToCarDoorAndStandStill (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064ABE0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexInvestigateDeadPed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00697B50 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexKillCriminal (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0068F9F0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexKillPedGroupOnFoot (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006296B0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexKillPedOnFootMelee (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00626FA0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexLeaveAnyCar (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00421250 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexLeaveCarAndFlee (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006424F0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexLeaveCarAndWander (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00642370 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexLeaveCarAsPassengerWait (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640E40 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexMoveBackAndJump (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006577D0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexPolicePursuit (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0068DAB0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexShuffleSeats (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00643760 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexTrackEntity (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006637A0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseAttractorPartner (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006390A0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseClosestFreeScriptedAttractorRun (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B8820 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexUseClosestFreeScriptedAttractorSprint (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B88E0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskComplexUseScriptedAttractor (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00639190 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWaitForBackup (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006980F0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWalkRoundCar (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00656E50 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorBeInHouse (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00676330 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorGoToInfo (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00676860 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorLieInBed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00677760 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorShopKeeper (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00676620 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorSitAtDesk (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006761E0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleBikeJacked (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064CC40 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarAlign (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064ACE0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarCloseDoorFromInside (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064B000 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarCloseDoorFromOutside (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064B2B0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarDrive (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00642F80 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarDriveTimed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046F670 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskSimpleCarGetIn (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064B3C0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarOpenDoorFromOutside (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064ADE0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarOpenLockedDoorFromOutside (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064AE70 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarSetPedInAsPassenger (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064B930 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarSlowDragPedOut (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064C060 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleEvasiveDive (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00655FE0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleGunControl (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006259C0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleKillPedWithCar (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00656AE0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleNone (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004635F0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimplePickUpBike (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064AF30 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimplePlayerOnFire (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00636F00 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleSay (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048E4C0 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskSimpleSitDown (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00637A40 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleSitIdle (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00637D40 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleStandUp (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00637D90 | 30 | B |  | 0 | 0 | sí | baja |

## CRoadBlocks (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00463370 | 31 | A |  | 5 | 2 |  | baja |

## CLodAtomicModelInfo (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C5620 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C5640 | 24 | D |  | 0 | 0 |  | baja |

## CEventEscalator (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B25E0 | 32 | B |  | 0 | 0 | sí | baja |

## CPhysical (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00544990 | 27 | B |  | 0 | 0 | sí | alta |
| 0x0054E8E0 | 5 | C |  | 5 | 0 |  | baja |

## CAECollisionAudioEntity (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004DA510 | 33 | A |  | 2 | 2 |  | alta |

## CTrailer (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006CF180 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006D0450 | 3 | C |  | 2 | 0 |  | baja |

## CAnimatedBuilding (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00537A50 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00537A70 | 5 | C |  | 1 | 0 |  | baja |

## CBridge (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0041BCB0 | 3 | A |  | 1 | 1 |  | baja |
| 0x0041BCE0 | 7 | A |  | 1 | 1 |  | baja |
| 0x0041BCF0 | 7 | A |  | 1 | 1 |  | baja |
| 0x0041BD00 | 3 | A |  | 1 | 1 |  | baja |
| 0x0041BD10 | 4 | A |  | 2 | 2 |  | baja |
| 0x0041BD50 | 7 | A |  | 6 | 6 |  | baja |
| 0x0041BD80 | 4 | A |  | 3 | 3 |  | baja |

## CTaskComplexArrestPed (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0068B9F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006908C0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexCarDrive (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063C9C0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00643220 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskGangHassleVehicle (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065FB50 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006642A0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseEffect (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00636780 | 39 | A |  | 1 | 1 |  | baja |

## CCustomBuildingDNPipeline (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D6830 | 25 | A |  | 1 | 1 |  | alta |
| 0x005D6E90 | 14 | A |  | 2 | 2 |  | baja |

## CDirectory (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00532540 | 28 | A |  | 3 | 0 |  | baja |
| 0x00532560 | 11 | C |  | 4 | 0 |  | baja |

## CPedType (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00608F00 | 20 | A |  | 2 | 2 |  | baja |
| 0x00608F20 | 20 | A |  | 1 | 1 |  | baja |

## CTaskComplexObserveTrafficLights (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00631910 | 34 | A |  | 1 | 1 |  | baja |
| 0x00631940 | 6 | B |  | 0 | 0 | sí | baja |

## CQuadBike (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006CE440 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006CED10 | 11 | C |  | 1 | 0 |  | baja |

## CTaskAllocatorKillThreatsBasicRandomGroup (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6900 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005F6920 | 11 | C |  | 1 | 0 |  | baja |

## CTaskComplexAttractorPartnerWait (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00633290 | 11 | C |  | 1 | 0 |  | alta |
| 0x00638FD0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexDriveToPoint (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063CE70 | 11 | C |  | 1 | 0 |  | alta |
| 0x00645350 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexEnterAnyCarAsDriver (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063D100 | 11 | C |  | 1 | 0 |  | alta |
| 0x006436E0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGoToPointAndStandStillTimed (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00668630 | 11 | C |  | 1 | 0 |  | alta |
| 0x0066DD50 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseEffectSprinting (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00636910 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00636930 | 11 | C |  | 1 | 0 |  | baja |

## CTaskComplexWanderMedic (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00658830 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00658850 | 11 | C |  | 1 | 0 |  | baja |

## CTaskComplexWanderStandard (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048E5E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0048E600 | 11 | C |  | 1 | 0 |  | baja |

## CTaskSimpleFinishBrain (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004636C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x004636E0 | 11 | C |  | 1 | 0 |  | baja |

## CTaskSimpleTogglePedThreatScanner (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00492D40 | 11 | C |  | 2 | 0 |  | alta |
| 0x00492DF0 | 30 | B |  | 0 | 0 | sí | baja |

## CTimeModelInfo (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C5680 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C5690 | 34 | D |  | 0 | 0 |  | baja |

## ProcObjectMan_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A40E0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005A40F0 | 30 | D |  | 0 | 0 |  | baja |

## CAEStaticChannel (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F1020 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004F11D0 | 3 | A |  | 1 | 1 |  | baja |
| 0x004F11E0 | 11 | A |  | 1 | 1 |  | baja |

## CTaskComplexReactToGunAimedAt (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00697EF0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006986E0 | 12 | D |  | 0 | 0 |  | baja |

## CTaskSimpleWaitUntilLeaderAreaCodesMatch (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6880 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005F68A0 | 12 | D |  | 0 | 0 |  | baja |

## CEventDanger (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B2670 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B2680 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B2690 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B27B0 | 28 | B |  | 0 | 0 | sí | baja |

## RwHelper (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ABAA0 | 27 | B |  | 0 | 0 | sí | baja |
| 0x00734EA0 | 9 | A |  | 6 | 6 |  | baja |
| 0x00734EC0 | 9 | A |  | 2 | 2 |  | baja |

## C2dEffect (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C4A60 | 21 | C |  | 9 | 0 |  | baja |
| 0x004C5540 | 18 | D |  | 0 | 0 |  | baja |
| 0x004C5920 | 7 | C |  | 1 | 0 |  | baja |

## CEventGunShot (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AC6B0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AC6C0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AC6D0 | 4 | B |  | 0 | 0 | sí | alta |
| 0x004AC6E0 | 21 | B |  | 0 | 0 | sí | alta |
| 0x004AC700 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AC710 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AC830 | 3 | B |  | 0 | 0 | sí | baja |

## CGeneral (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0041BDD0 | 47 | A |  | 10 | 2 |  | baja |

## CEventVehicleDamageCollision (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006A0650 | 11 | C |  | 2 | 0 |  | alta |
| 0x006A0660 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006A06D0 | 30 | B |  | 0 | 0 | sí | baja |

## CEventVehicleDamageWeapon (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061C310 | 11 | C |  | 9 | 0 |  | alta |
| 0x0061C320 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0061C390 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexTreatAccident (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00658AD0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00658AE0 | 11 | C |  | 1 | 0 |  | alta |
| 0x00659FC0 | 30 | B |  | 0 | 0 | sí | baja |

## CAEMP3BankLoader (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004E0380 | 48 | A |  | 1 | 1 |  | alta |

## CTaskSimpleJetPack (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0067BA40 | 18 | A |  | 1 | 1 |  | baja |
| 0x0067FA60 | 30 | B |  | 0 | 0 | sí | baja |

## CText (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0069FFA0 | 24 | D |  | 0 | 0 |  | baja |
| 0x0069FFD0 | 24 | D |  | 0 | 0 |  | baja |

## CTaskAllocatorAttack (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0069C480 | 50 | A |  | 1 | 1 |  | baja |

## CWorld (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0056CE30 | 51 | A |  | 1 | 1 |  | baja |

## CAEScriptAudioEntity (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004EE9A0 | 52 | C |  | 1 | 0 |  | baja |

## CCollisionData (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0040F700 | 52 | A |  | 30 | 7 |  | baja |

## cBuoyancy (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006C4130 | 54 | A |  | 2 | 2 |  | baja |

## CPedPool (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004049C0 | 54 | D |  | 0 | 0 |  | baja |

## CAEAudioHardware (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D8670 | 27 | D |  | 0 | 0 |  | alta |
| 0x004D8690 | 27 | D |  | 0 | 0 |  | alta |

## CConversations (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0043AA40 | 55 | C |  | 2 | 0 |  | alta |

## CTheCarGenerators (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0047C0E0 | 21 | C |  | 1 | 0 |  | baja |
| 0x0047E050 | 4 | A |  | 6 | 6 |  | baja |
| 0x0047E070 | 30 | C |  | 1 | 0 |  | baja |

## FxManager_c (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A9110 | 12 | D |  | 0 | 0 |  | alta |
| 0x004A9340 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A94E0 | 12 | D |  | 0 | 0 |  | alta |
| 0x004A9C10 | 3 | A |  | 1 | 1 |  | baja |
| 0x004A9C20 | 1 | A |  | 1 | 1 |  | baja |

## CEventAreaCodes (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B21F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B2200 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B2390 | 5 | B |  | 0 | 0 | sí | baja |
| 0x004B23A0 | 42 | B |  | 0 | 0 | sí | baja |

## CShadowCamera (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00705480 | 57 | D |  | 0 | 0 |  | alta |

## CSprite (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0070F890 | 57 | D |  | 0 | 0 |  | baja |

## CAEPedAudioEntity (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004E3350 | 43 | A |  | 2 | 2 |  | baja |
| 0x004E34C0 | 14 | C |  | 2 | 0 |  | baja |

## CTaskSimpleTired (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00630F50 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00630FF0 | 51 | B |  | 0 | 0 | sí | baja |

## CColLine (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0040F030 | 58 | A |  | 5 | 3 |  | baja |

## CVehicleAnimGroup (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005BA020 | 39 | D |  | 0 | 0 |  | baja |
| 0x006E3E30 | 19 | D |  | 0 | 0 |  | baja |

## CAudioEngine (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00506DC0 | 27 | D |  | 0 | 0 |  | alta |
| 0x00507400 | 14 | A |  | 1 | 1 |  | alta |
| 0x005074B0 | 17 | C |  | 1 | 0 |  | baja |

## CAEWMADecoder (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00502BF0 | 32 | A |  | 1 | 1 |  | alta |
| 0x00502C40 | 27 | B |  | 0 | 0 | sí | alta |

## CTaskComplexCarSlowBeDraggedOut (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064C930 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064C950 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexFleeAnyMeans (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065D0C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065D0E0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexFollowPointRoute (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00671610 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00671730 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexHitResponse (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00638160 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00638180 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexKillPedOnFootArmed (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006275E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00628780 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexOnFire (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00639310 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639330 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexPassObject (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00664A80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00664AA0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseAttractor (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00638900 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00638920 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWanderCriminal (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048E700 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00670400 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorBeInOffice (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00676480 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006764A0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCreateCarAndGetIn (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064D1E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064D200 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleEvasiveStep (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00655EE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655F00 | 30 | B |  | 0 | 0 | sí | baja |

## CAEMFDecoder (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004E7850 | 10 | A |  | 1 | 1 |  | alta |
| 0x004E7C50 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004E7F20 | 23 | A |  | 1 | 1 |  | baja |

## CRealTimeShadow (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x007059E0 | 4 | A |  | 1 | 1 |  | alta |
| 0x00706770 | 36 | C |  | 1 | 0 |  | baja |
| 0x007067A0 | 20 | C |  | 1 | 0 |  | baja |

## CVehicleAnimGroupData (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063A040 | 61 | A |  | 4 | 4 |  | baja |

## CBuildingPool (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00404090 | 11 | A |  | 4 | 4 |  | baja |
| 0x004040A0 | 51 | C |  | 5 | 0 |  | baja |

## CDummyPool (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005326E0 | 11 | A |  | 1 | 1 |  | baja |
| 0x005326F0 | 51 | C |  | 3 | 0 |  | baja |

## CNodeRoutePool (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0041B810 | 11 | C |  | 1 | 0 |  | baja |
| 0x0041B820 | 51 | C |  | 3 | 0 |  | baja |

## CObjectData (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A2E60 | 47 | A |  | 2 | 2 |  | baja |
| 0x005A2E90 | 15 | D |  | 0 | 0 |  | baja |

## CAEBankLoader (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004DFB60 | 63 | A |  | 1 | 1 |  | alta |

## CTimeInfo (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005B3680 | 63 | A |  | 9 | 9 |  | baja |

## CEventGlobalGroup (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AB980 | 14 | A |  | 2 | 1 |  | alta |
| 0x004AB9B0 | 14 | C |  | 1 | 0 |  | alta |
| 0x004ABA20 | 35 | B |  | 0 | 0 | sí | baja |

## CEntryInfoList (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00536E90 | 65 | A |  | 3 | 3 |  | baja |

## CPedShelterAttractor (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005E9800 | 33 | A |  | 1 | 0 |  | baja |
| 0x005E9850 | 32 | A |  | 1 | 0 |  | baja |

## CTaskComplexGoToPointAiming (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066DF80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066DFA0 | 5 | C |  | 1 | 0 |  | baja |
| 0x0066DFB0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSequence (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00463640 | 18 | A |  | 1 | 1 |  | baja |
| 0x005F67E0 | 11 | C |  | 2 | 0 |  | baja |
| 0x00632C60 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00638A80 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseSequence (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00635490 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006397A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006397C0 | 30 | B |  | 0 | 0 | sí | baja |

## FxSystem_c (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AB240 | 5 | A |  | 1 | 1 |  | baja |
| 0x004AB250 | 5 | A |  | 59 | 59 |  | baja |
| 0x004AB260 | 5 | A |  | 1 | 1 |  | baja |
| 0x004AB2D0 | 51 | A |  | 1 | 1 |  | baja |

## CRadar (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00583AF0 | 67 | A |  | 2 | 2 |  | alta |

## CTaskComplexDiveFromAttachedEntityAndGetUp (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00492EF0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00492F60 | 7 | C |  | 5 | 0 |  | baja |
| 0x006579F0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCower (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048DEA0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x0048E910 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0048E930 | 5 | C |  | 1 | 0 |  | baja |
| 0x0048E940 | 30 | B |  | 0 | 0 | sí | baja |

## FxPrimBP_c (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A9F20 | 21 | A |  | 2 | 1 |  | baja |
| 0x004A9F40 | 15 | D |  | 0 | 0 |  | baja |
| 0x004A9F50 | 10 | A |  | 1 | 1 |  | baja |
| 0x004A9F60 | 14 | C |  | 1 | 0 |  | baja |
| 0x004A9F70 | 10 | A |  | 3 | 2 |  | baja |

## CIplDefPool (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00405A60 | 71 | D |  | 0 | 0 |  | baja |

## CPedGroupIntelligence (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F8580 | 29 | C |  | 1 | 0 |  | alta |
| 0x005F8750 | 42 | D |  | 0 | 0 |  | alta |

## CTaskComplexCarDriveWander (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063CB50 | 11 | C |  | 1 | 0 |  | alta |
| 0x00643260 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00643280 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleGoToPoint (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00667D50 | 11 | A |  | 3 | 1 |  | alta |
| 0x0066DB60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066DB80 | 30 | B |  | 0 | 0 | sí | baja |

## CPedGroups (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F7FB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FB990 | 42 | C |  | 1 | 0 |  | baja |

## CPedIK (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FE960 | 49 | A |  | 1 | 1 |  | baja |
| 0x005FE9C0 | 23 | A |  | 6 | 4 |  | baja |

## CWanted (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00563130 | 76 | D |  | 0 | 0 |  | baja |

## CCollision (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004161D0 | 12 | D |  | 0 | 0 |  | alta |
| 0x004161F0 | 64 | A |  | 2 | 2 |  | alta |

## CTaskComplexInAirAndLand (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00678CB0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00678CC0 | 11 | C |  | 1 | 0 |  | baja |
| 0x0067D340 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0067D360 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimplePause (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048E800 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0048E880 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0048E8A0 | 11 | C |  | 1 | 0 |  | baja |
| 0x0048E8E0 | 30 | B |  | 0 | 0 | sí | baja |

## CDummy (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00532770 | 78 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleShakeFist (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00690C70 | 48 | A |  | 3 | 3 |  | baja |
| 0x00692E30 | 30 | B |  | 0 | 0 | sí | baja |

## Securom (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00744D50 | 49 | C |  | 1 | 0 |  | baja |
| 0x00744D90 | 29 | C |  | 1 | 0 |  | baja |

## CTaskComplexDrivePointRoute (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063D0D0 | 18 | D |  | 0 | 0 |  | baja |
| 0x006434F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00643A40 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSequences (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00632E00 | 79 | C |  | 1 | 0 |  | baja |

## CTagManager (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049CD50 | 80 | A |  | 1 | 1 |  | alta |

## CTaskComplexUseEffectRunning (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00636830 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00636850 | 11 | C |  | 1 | 0 |  | baja |
| 0x00636860 | 39 | A |  | 1 | 1 |  | baja |

## CEventBuildingCollision (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ACFB0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004ACFC0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004ACFD0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004ACFE0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AD050 | 32 | B |  | 0 | 0 | sí | alta |
| 0x004AD3A0 | 32 | D |  | 0 | 0 |  | baja |

## CBuilding (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00404130 | 78 | B |  | 0 | 0 | sí | baja |
| 0x00404180 | 5 | C |  | 1 | 0 |  | baja |

## CPedGroupDefaultTaskAllocatorChat (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6510 | 9 | C |  | 1 | 0 |  | baja |
| 0x005F8510 | 77 | A |  | 11 | 11 |  | baja |

## CTaskSimpleAffectSecondaryBehaviour (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006912C0 | 87 | C |  | 1 | 0 |  | alta |

## CAEAudioChannel (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D7990 | 7 | A |  | 3 | 3 |  | alta |
| 0x004D7C00 | 81 | B |  | 0 | 0 | sí | alta |

## CAEStreamingChannel (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F21C0 | 61 | B | CAEStreamingChannel::Stop-virtual | 0 | 0 | sí | alta |
| 0x004F27F0 | 27 | B |  | 0 | 0 | sí | baja |

## CBaseModelInfo (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C4AF0 | 28 | B |  | 0 | 0 | sí | alta |
| 0x004C4C00 | 30 | D |  | 0 | 0 |  | alta |
| 0x005329A0 | 30 | A |  | 2 | 2 |  | baja |

## CRect (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00404350 | 3 | A |  | 3 | 3 |  | baja |
| 0x004043E0 | 8 | A |  | 2 | 2 |  | baja |
| 0x00404420 | 5 | A |  | 1 | 1 |  | baja |
| 0x00404590 | 30 | D |  | 0 | 0 |  | baja |
| 0x0043E090 | 42 | C |  | 1 | 0 |  | baja |

## CTaskSimpleCarSetPedOut (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00647950 | 90 | A |  | 25 | 11 |  | alta |

## CTaskComplexPlayHandSignalAnim (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061BA80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0061BAA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0061BEF0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarShuffle (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0064B440 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064B460 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064B480 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00661F10 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00665DC0 | 61 | A |  | 2 | 2 |  | baja |

## CGangs (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DE920 | 92 | A |  | 3 | 1 |  | baja |

## CGarage (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00448650 | 6 | A |  | 2 | 2 |  | alta |
| 0x00448660 | 86 | A |  | 1 | 1 |  | alta |

## CTaskComplexUsePairedAttractor (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00633250 | 34 | A |  | 1 | 1 |  | baja |
| 0x00638F00 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639F90 | 30 | B |  | 0 | 0 | sí | baja |

## CColModel (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0040FAF0 | 96 | A |  | 8 | 4 |  | baja |

## CTaskComplexKillPedOnFoot (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00620EE0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00620EF0 | 90 | A |  | 4 | 2 |  | baja |

## CTaskSimpleGiveCPR (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00658890 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006588A0 | 93 | C |  | 1 | 0 |  | alta |

## CTaskSimpleHitHead (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00653080 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00653090 | 93 | C |  | 1 | 0 |  | alta |

## CInformGroupEvent (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AC370 | 50 | D |  | 0 | 0 |  | baja |
| 0x004AC3B0 | 51 | D |  | 0 | 0 |  | baja |

## CTaskComplexPartnerShove (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00684750 | 11 | C |  | 1 | 0 |  | baja |
| 0x00684760 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684780 | 30 | D |  | 0 | 0 |  | baja |
| 0x006847A0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleFacial (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00690CB0 | 11 | C |  | 1 | 0 |  | alta |
| 0x00692F90 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00692FB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00692FD0 | 30 | B |  | 0 | 0 | sí | baja |

## CPedGroupDefaultTaskAllocatorSitInLeaderCar (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F65A0 | 51 | A |  | 6 | 6 |  | baja |
| 0x005F7200 | 54 | A |  | 1 | 1 |  | baja |

## CPointRoutePool (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0041B660 | 106 | C |  | 1 | 0 |  | baja |

## CTaskSimplePutDownEntity (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00691A40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00692C80 | 100 | B |  | 0 | 0 | sí | baja |

## JPegPlugin (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D0570 | 108 | A |  | 1 | 1 |  | alta |

## CTaskComplexWalkAlongsidePed (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00683950 | 110 | D |  | 0 | 0 |  | baja |

## CInterestingEvents (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00602B90 | 80 | B |  | 0 | 0 | sí | baja |
| 0x00602BE0 | 30 | B |  | 0 | 0 | sí | baja |

## CPlayerRelationshipRecorder (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061A2E0 | 64 | A |  | 2 | 2 |  | baja |
| 0x0061A350 | 7 | C |  | 1 | 0 |  | baja |
| 0x0061A430 | 40 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleHandsUp (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048EA40 | 30 | B |  | 0 | 0 | sí | alta |
| 0x0048EA70 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0048EA90 | 5 | C |  | 1 | 0 |  | baja |
| 0x00492CB0 | 48 | C |  | 1 | 0 |  | baja |

## CTaskSimpleInAir (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00678D40 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00678D50 | 109 | C |  | 1 | 0 |  | alta |

## CScripted2dEffects (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006F9EB0 | 116 | A |  | 3 | 2 |  | alta |

## App (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0053ED30 | 1 | A |  | 1 | 1 |  | baja |
| 0x0053EDC0 | 23 | A |  | 4 | 4 |  | baja |
| 0x0053EE20 | 23 | A |  | 2 | 2 |  | baja |
| 0x0053EE60 | 23 | A |  | 1 | 1 |  | baja |
| 0x0053EF40 | 23 | A |  | 1 | 1 |  | baja |
| 0x0053EF60 | 23 | A |  | 1 | 1 |  | baja |

## CEventPotentialWalkIntoObject (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AE640 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AE650 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004AE660 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AE6D0 | 5 | B |  | 0 | 0 | sí | baja |
| 0x004B4A50 | 77 | D |  | 0 | 0 |  | baja |
| 0x004B4AA0 | 19 | A |  | 1 | 1 |  | baja |

## CCoronas (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006FBA20 | 117 | A |  | 5 | 5 |  | alta |

## CTaskComplexScreamInCarThenLeave (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006426E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00642700 | 88 | A |  | 1 | 1 |  | baja |

## CBmx (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006BF9D0 | 85 | C |  | 1 | 0 |  | alta |
| 0x006C0560 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006C0570 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskSimpleCarSetPedInAsDriver (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00647170 | 90 | A |  | 9 | 2 |  | baja |
| 0x0064BDC0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexLeaveCarAndDie (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062F250 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0062F270 | 11 | C |  | 1 | 0 |  | baja |
| 0x0062F2D0 | 49 | A |  | 1 | 1 |  | baja |
| 0x006422A0 | 30 | B |  | 0 | 0 | sí | baja |

## CEventStuckInAir (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B14F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B1500 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B1510 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B1640 | 54 | D |  | 0 | 0 |  | baja |
| 0x004B1680 | 54 | D |  | 0 | 0 |  | baja |

## CAnimBlendStaticAssociation (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004CECF0 | 94 | C |  | 1 | 0 |  | baja |
| 0x004CEFA0 | 27 | D |  | 0 | 0 |  | baja |

## CAEVehicleAudioEntity (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F56D0 | 44 | A |  | 9 | 9 |  | alta |
| 0x004F63B0 | 33 | C |  | 2 | 0 |  | alta |
| 0x006D0AF0 | 18 | A |  | 1 | 1 |  | baja |
| 0x006D0B10 | 25 | A |  | 2 | 2 |  | baja |
| 0x006D0B30 | 1 | A |  | 5 | 1 |  | baja |

## CTaskComplexPartnerChat (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00684320 | 93 | C |  | 1 | 0 |  | baja |
| 0x006846D0 | 30 | B |  | 0 | 0 | sí | baja |

## CColSphere (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0040FD50 | 85 | A |  | 1 | 1 |  | alta |
| 0x0040FDD0 | 35 | D |  | 0 | 0 |  | alta |
| 0x00410390 | 4 | A |  | 1 | 1 |  | baja |

## CEventPool (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B5630 | 49 | C |  | 119 | 0 |  | baja |
| 0x004B5670 | 77 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseSwatRope (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006594D0 | 96 | C |  | 1 | 0 |  | alta |
| 0x0065A620 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskGangHasslePed (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065FF50 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00664480 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006644A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006644C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006644E0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleDead (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00636180 | 127 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleFall (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00678300 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00678310 | 93 | C |  | 1 | 0 |  | alta |
| 0x0067CB50 | 30 | B |  | 0 | 0 | sí | baja |

## CEventHandler (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C3740 | 65 | D |  | 0 | 0 |  | alta |
| 0x004C3E30 | 65 | D |  | 0 | 0 |  | alta |

## CEventObjectCollision (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ACDA0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004ACDB0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004ACDC0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004ACE80 | 110 | A |  | 1 | 1 |  | baja |
| 0x004ACEF0 | 5 | B |  | 0 | 0 | sí | baja |

## CCompressedMatrixNotAligned (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059BB60 | 74 | A |  | 5 | 5 |  | baja |
| 0x0059BD90 | 57 | D |  | 0 | 0 |  | baja |

## CAEGlobalWeaponAudioEntity (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004DEFE0 | 121 | A |  | 2 | 2 |  | alta |
| 0x00506CC0 | 14 | C |  | 1 | 0 |  | baja |

## CTaskSimpleUseGun (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061DF30 | 109 | C |  | 1 | 0 |  | alta |
| 0x00625250 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarSlowBeDraggedOut (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00648330 | 81 | A |  | 4 | 4 |  | alta |
| 0x0064C200 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064E2A0 | 30 | B |  | 0 | 0 | sí | baja |

## CEventLeaderExitedCarAsDriver (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B8320 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B8330 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B8340 | 80 | B |  | 0 | 0 | sí | baja |
| 0x004B8390 | 5 | B |  | 0 | 0 | sí | baja |
| 0x004B83A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x004B83C0 | 11 | C |  | 1 | 0 |  | baja |
| 0x004B84D0 | 6 | B |  | 0 | 0 | sí | baja |

## CTaskLeanOnVehicle (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00661020 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006610A0 | 106 | B |  | 0 | 0 | sí | alta |
| 0x00664A30 | 30 | B |  | 0 | 0 | sí | baja |

## VideoPlayer (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x007467D0 | 35 | C |  | 1 | 0 |  | baja |
| 0x00746800 | 110 | D |  | 0 | 0 |  | baja |

## CTaskSimpleJump (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00679AE0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00679AF0 | 109 | C |  | 1 | 0 |  | alta |
| 0x0067D960 | 30 | B |  | 0 | 0 | sí | baja |

## CEventLeaderQuitEnteringCarAsDriver (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1040 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0063A130 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0063A140 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0063A150 | 80 | B |  | 0 | 0 | sí | baja |
| 0x0063A1A0 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0063A1B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0063A1D0 | 11 | C |  | 1 | 0 |  | baja |
| 0x0063A1E0 | 6 | B |  | 0 | 0 | sí | baja |

## CTaskComplexFleePoint (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065D240 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065D260 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065D280 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065D2A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065D2C0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSitDownThenIdleThenStandUp (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00637F60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637F80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637FA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637FC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00638020 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleIKChain (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00639490 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006394B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006394D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006394F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639510 | 30 | B |  | 0 | 0 | sí | baja |

## CEventPlayerCommandToGroupAttack (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6360 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005F6370 | 89 | B |  | 0 | 0 | sí | baja |
| 0x005F63D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005F63F0 | 11 | C |  | 3 | 0 |  | baja |
| 0x005F6400 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005F6450 | 9 | C |  | 1 | 0 |  | baja |

## CLoadedCarGroup (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00611F20 | 152 | C |  | 1 | 0 |  | baja |

## CTaskSimpleClimb (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0067A1C0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0067A1D0 | 116 | C |  | 1 | 0 |  | alta |
| 0x006801D0 | 30 | B |  | 0 | 0 | sí | baja |

## CEventLeaderEntryExit (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0043E1E0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0043E1F0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0043E200 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0043E210 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0043E220 | 88 | B |  | 0 | 0 | sí | baja |
| 0x0043E280 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0043E290 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0043E2B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x0043E2C0 | 6 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleUninterruptable (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0043E300 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0043E310 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0043E320 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0043E330 | 80 | B |  | 0 | 0 | sí | baja |
| 0x0043E380 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0043E390 | 11 | B |  | 0 | 0 | sí | baja |
| 0x0043E3A0 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0043E3B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0043E3D0 | 11 | C |  | 1 | 0 |  | baja |

## CTaskSimpleGoToPointFine (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065EEE0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0065EEF0 | 11 | C |  | 1 | 0 |  | alta |
| 0x006620D0 | 108 | B |  | 0 | 0 | sí | baja |
| 0x00663620 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarJumpOut (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00647770 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0064BEE0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x0064BF50 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064BF70 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064BF90 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064D290 | 30 | B |  | 0 | 0 | sí | baja |

## CDecision (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006007A0 | 157 | A |  | 1 | 1 |  | baja |

## CFileLoader (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00537290 | 49 | D |  | 0 | 0 |  | alta |
| 0x00537510 | 55 | A |  | 2 | 2 |  | alta |
| 0x00537550 | 12 | D |  | 0 | 0 |  | alta |
| 0x00537560 | 12 | D |  | 0 | 0 |  | alta |
| 0x00537570 | 12 | D |  | 0 | 0 |  | alta |
| 0x00537A10 | 18 | A |  | 1 | 1 |  | baja |

## CTaskSimpleDie (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062FA50 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0062FB40 | 93 | A |  | 3 | 1 |  | alta |
| 0x006375B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006375D0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleGetUp (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00677F70 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00677F80 | 93 | C |  | 1 | 0 |  | alta |
| 0x0067CA00 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0067CA20 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleLand (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00678ED0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00678EE0 | 93 | C |  | 1 | 0 |  | alta |
| 0x0067D550 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0067D570 | 30 | B |  | 0 | 0 | sí | baja |

## CIplStore (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00405900 | 91 | A |  | 1 | 1 |  | baja |
| 0x00405960 | 54 | D |  | 0 | 0 |  | baja |
| 0x00406360 | 15 | A |  | 1 | 1 |  | baja |

## CTaskSimplePlayerOnFoot (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0068B050 | 100 | B |  | 0 | 0 | sí | baja |
| 0x0068B0C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0068B0E0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWaitForBus (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006389B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006389D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006389F0 | 79 | A |  | 27 | 12 |  | baja |
| 0x00638A40 | 21 | B | CTaskComplexSequence::CreateNextSubTask-V | 0 | 0 | sí | baja |

## CTaskComplexGoToAttractor (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066B690 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0066B6B0 | 8 | B |  | 0 | 0 | sí | alta |
| 0x0066B710 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0066D1E0 | 112 | B |  | 0 | 0 | sí | baja |
| 0x006705B0 | 30 | B |  | 0 | 0 | sí | baja |

## BreakManager_c (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059EA00 | 18 | A |  | 1 | 1 |  | baja |
| 0x0059EA20 | 135 | D |  | 0 | 0 |  | baja |
| 0x0059EAB0 | 11 | C |  | 1 | 0 |  | baja |

## CMatrixLinkList (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0054EB70 | 140 | C |  | 1 | 0 |  | baja |
| 0x0054F1E0 | 26 | A |  | 2 | 2 |  | baja |

## CTaskComplexProstituteSolicit (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00662350 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00667310 | 99 | A |  | 4 | 4 |  | baja |
| 0x00667460 | 37 | D |  | 0 | 0 |  | baja |

## CAtomicModelInfo (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C44D0 | 82 | B |  | 1 | 0 | sí | alta |
| 0x004C4530 | 60 | B |  | 0 | 0 | sí | alta |
| 0x004C5590 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C55A0 | 18 | D |  | 0 | 0 |  | baja |

## CEventPedCollisionWithPlayer (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FED80 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FED90 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FEDA0 | 120 | B |  | 0 | 0 | sí | baja |
| 0x005FEE20 | 30 | B |  | 0 | 0 | sí | baja |

## CEventPlayerCollisionWithPed (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FEE80 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FEE90 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FEEA0 | 120 | B |  | 0 | 0 | sí | baja |
| 0x005FEF20 | 30 | B |  | 0 | 0 | sí | baja |

## CBike (22)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006B5820 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006B5830 | 9 | B |  | 0 | 0 | sí | alta |
| 0x006B5840 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006B5850 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006B5860 | 17 | B |  | 0 | 0 | sí | alta |
| 0x006B5880 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5890 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006B58A0 | 8 | B |  | 0 | 0 | sí | alta |
| 0x006B58B0 | 7 | B |  | 0 | 0 | sí | alta |
| 0x006B58C0 | 7 | B |  | 0 | 0 | sí | alta |
| 0x006B58E0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B58F0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5900 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5910 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5920 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5930 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5940 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B5950 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B59E0 | 19 | B |  | 0 | 0 | sí | alta |
| 0x006B7270 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006B8940 | 9 | B |  | 0 | 0 | sí | alta |
| 0x006B8950 | 30 | B |  | 0 | 0 | sí | alta |

## CTaskSimpleStealthKill (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00622680 | 109 | C |  | 1 | 0 |  | alta |
| 0x006298E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00629900 | 30 | B |  | 0 | 0 | sí | baja |

## CAEDataStream (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004DC2A0 | 15 | A |  | 1 | 1 |  | alta |
| 0x004DC340 | 93 | B | CAEDataStream::Seek-istream | 0 | 0 | sí | alta |
| 0x004DC550 | 62 | D |  | 0 | 0 |  | alta |

## CAETwinLoopSoundEntity (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F2930 | 35 | D |  | 0 | 0 |  | alta |
| 0x004F2B80 | 139 | A |  | 8 | 1 |  | alta |

## CTaskComplexJump (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0067A050 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0067A060 | 11 | A |  | 2 | 1 |  | alta |
| 0x0067A100 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0067C610 | 122 | B |  | 0 | 0 | sí | baja |
| 0x0067DBC0 | 30 | B |  | 0 | 0 | sí | baja |

## CAEAudioEntity (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DEA00 | 90 | A |  | 26 | 11 |  | baja |
| 0x005DEA60 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005DEA70 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005DEA80 | 4 | B |  | 0 | 0 | sí | baja |
| 0x005DEA90 | 4 | B |  | 0 | 0 | sí | baja |
| 0x005DEAA0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005DEAB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005DEAD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005DEB90 | 30 | D |  | 0 | 0 |  | baja |

## CAnimBlendAssociation (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004CEB60 | 16 | A |  | 2 | 1 |  | alta |
| 0x004CEC00 | 20 | A |  | 1 | 1 |  | baja |
| 0x004CEF30 | 41 | D |  | 0 | 0 |  | baja |
| 0x004D1570 | 100 | A |  | 1 | 1 |  | baja |

## CTaskComplexEnterCar (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063A2E0 | 27 | D |  | 0 | 0 |  | alta |
| 0x00643C60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00643C80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00643CA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00643CC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00643CE0 | 30 | B |  | 0 | 0 | sí | baja |

## CLoadingScreen (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00590DE0 | 12 | D |  | 0 | 0 |  | baja |
| 0x00590E00 | 12 | D |  | 0 | 0 |  | baja |
| 0x00590E20 | 68 | D |  | 0 | 0 |  | baja |
| 0x00590E70 | 76 | D |  | 0 | 0 |  | baja |
| 0x00590EC0 | 12 | D |  | 0 | 0 |  | baja |

## CTaskComplexSeekCoverUntilTargetDead (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00622E40 | 106 | B |  | 0 | 0 | sí | baja |
| 0x006239D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00624B30 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0062EC10 | 18 | D |  | 0 | 0 |  | baja |

## CTaskComplexFollowPedFootsteps (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00694EC0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006955D0 | 44 | A |  | 1 | 1 |  | baja |
| 0x00695600 | 135 | A |  | 2 | 2 |  | baja |

## D3DResourceSystem (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00730B30 | 55 | D |  | 0 | 0 |  | baja |
| 0x007315A0 | 87 | D |  | 0 | 0 |  | baja |
| 0x00731650 | 46 | A |  | 1 | 1 |  | baja |

## FxMemoryPool_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A9CF0 | 35 | A |  | 1 | 1 |  | baja |
| 0x004A9D20 | 159 | C |  | 1 | 0 |  | baja |

## CTaskSimpleRunAnim (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061BB10 | 167 | A |  | 1 | 1 |  | baja |
| 0x0061BBC0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSeekEntityStandard (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046AD50 | 30 | B |  | 0 | 0 | sí | alta |
| 0x0046AE00 | 33 | A |  | 2 | 1 |  | baja |
| 0x0046AE80 | 104 | C |  | 2 | 0 |  | baja |
| 0x0046AEF0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexDriveFireTruck (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00659390 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006593A0 | 106 | C |  | 1 | 0 |  | baja |
| 0x00659410 | 44 | C |  | 1 | 0 |  | baja |
| 0x00659440 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00659450 | 5 | B |  | 0 | 0 | sí | baja |
| 0x00659460 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0065A3C0 | 30 | B |  | 0 | 0 | sí | baja |

## CPedList (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00699E20 | 179 | A |  | 3 | 3 |  | baja |
| 0x00699F00 | 24 | A |  | 3 | 2 |  | baja |

## CCutsceneMgr (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005B19D0 | 175 | A |  | 1 | 1 |  | baja |
| 0x005B1A80 | 11 | C |  | 1 | 0 |  | baja |
| 0x005B1A90 | 18 | D |  | 0 | 0 |  | baja |

## IKChain_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00617FC0 | 102 | C |  | 1 | 0 |  | baja |
| 0x00618030 | 103 | C |  | 1 | 0 |  | baja |

## CVisibilityPlugins (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00732D60 | 12 | D |  | 0 | 0 |  | alta |
| 0x00732D80 | 12 | D |  | 0 | 0 |  | alta |
| 0x00733910 | 115 | A |  | 13 | 13 |  | alta |
| 0x00733F40 | 63 | A |  | 5 | 5 |  | alta |
| 0x00734610 | 1 | A |  | 1 | 1 |  | baja |
| 0x00734620 | 1 | A |  | 1 | 1 |  | baja |
| 0x00734640 | 1 | A |  | 3 | 3 |  | baja |

## CMouseControllerState (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0053F2D0 | 208 | A |  | 2 | 1 |  | baja |

## CGamma (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00747300 | 210 | A |  | 2 | 2 |  | baja |

## CDamageAtomicModelInfo (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C4910 | 24 | B |  | 0 | 0 | sí | alta |
| 0x004C4923 | 48 | D |  | 0 | 0 |  | alta |
| 0x004C4960 | 103 | B |  | 0 | 0 | sí | alta |
| 0x004C55D0 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C55E0 | 28 | D |  | 0 | 0 |  | baja |

## WaterCreature_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006E3EB0 | 105 | C |  | 1 | 0 |  | baja |
| 0x006E3F20 | 106 | C |  | 1 | 0 |  | baja |

## CMenuManager (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0057C400 | 24 | A |  | 2 | 2 |  | alta |
| 0x0057C420 | 24 | A |  | 2 | 2 |  | alta |
| 0x0057C440 | 24 | A |  | 1 | 1 |  | alta |
| 0x0057C460 | 24 | A |  | 1 | 1 |  | alta |
| 0x0057C5E0 | 118 | A |  | 2 | 2 |  | alta |

## CTaskSimpleFightingControl (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061DE60 | 185 | A |  | 9 | 9 |  | baja |
| 0x00624DA0 | 30 | B |  | 0 | 0 | sí | baja |

## CAECutsceneTrackManager (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004DBF10 | 71 | A |  | 1 | 1 |  | alta |
| 0x004DBF60 | 66 | A |  | 1 | 1 |  | alta |
| 0x004DC0E0 | 70 | A |  | 4 | 4 |  | alta |
| 0x004DC190 | 12 | D |  | 0 | 0 |  | baja |

## CAESoundManager (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004EFD30 | 33 | C |  | 1 | 0 |  | baja |
| 0x004F0B10 | 60 | A |  | 1 | 1 |  | baja |
| 0x004F0B50 | 127 | C |  | 1 | 0 |  | baja |

## CTaskSimpleFight (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061C520 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0061DB30 | 217 | A |  | 1 | 1 |  | baja |

## CTaskComplexWanderProstitute (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006726D0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006726F0 | 11 | C |  | 1 | 0 |  | alta |
| 0x00674930 | 182 | C |  | 2 | 0 |  | baja |

## CTaskComplexObserveTrafficLightsAndAchieveHeading (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00631980 | 11 | C |  | 1 | 0 |  | alta |
| 0x00631990 | 218 | A |  | 3 | 3 |  | alta |

## CEventDontJoinPlayerGroup (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00609140 | 90 | A |  | 3 | 1 |  | baja |
| 0x006091A0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006091B0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x006091C0 | 96 | B |  | 0 | 0 | sí | baja |
| 0x00609220 | 5 | B |  | 0 | 0 | sí | baja |
| 0x00609230 | 30 | B |  | 0 | 0 | sí | baja |

## CEventNewGangMember (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00608FD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00608FE0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00608FF0 | 96 | B |  | 0 | 0 | sí | baja |
| 0x00609050 | 5 | B |  | 0 | 0 | sí | baja |
| 0x00609060 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00609080 | 90 | C |  | 1 | 0 |  | baja |

## CEventLeanOnVehicle (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065DB50 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0065DB60 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0065DB70 | 100 | B |  | 0 | 0 | sí | baja |
| 0x0065DBE0 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0065DBF0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065DC10 | 90 | C |  | 1 | 0 |  | baja |

## CEventHitByWaterCannon (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B12E0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B12F0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B1300 | 7 | B |  | 0 | 0 | sí | alta |
| 0x004B1310 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B1320 | 7 | A |  | 3 | 1 |  | alta |
| 0x004B1370 | 26 | A |  | 2 | 2 |  | baja |
| 0x004B13C0 | 7 | A |  | 5 | 2 |  | baja |
| 0x004B13D0 | 75 | D |  | 0 | 0 |  | baja |
| 0x004B1420 | 97 | D |  | 0 | 0 |  | baja |

## CTaskSimpleRunNamedAnim (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061C0D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0061C150 | 20 | A |  | 1 | 1 |  | baja |
| 0x00667720 | 116 | A |  | 1 | 1 |  | baja |
| 0x006677A0 | 19 | C |  | 2 | 0 |  | baja |
| 0x00667900 | 43 | A |  | 1 | 1 |  | baja |
| 0x00667980 | 7 | C |  | 2 | 0 |  | baja |

## CTaskSimpleWaitForBus (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00632B30 | 32 | D |  | 0 | 0 |  | baja |
| 0x00632B50 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00632B60 | 5 | B |  | 0 | 0 | sí | baja |
| 0x00632B70 | 11 | C |  | 1 | 0 |  | baja |
| 0x00632B80 | 72 | B |  | 0 | 0 | sí | baja |
| 0x00636B40 | 113 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleStandStill (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B86C0 | 89 | B |  | 0 | 0 | sí | baja |
| 0x004B8720 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B8730 | 30 | B |  | 0 | 0 | sí | baja |
| 0x004B8750 | 11 | C |  | 1 | 0 |  | baja |
| 0x0062F350 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0062F520 | 99 | B |  | 0 | 0 | sí | baja |

## CAnimBlendHierarchy (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004CFA30 | 83 | A |  | 1 | 1 |  | baja |
| 0x004CFA90 | 91 | A |  | 4 | 4 |  | baja |
| 0x004CFB10 | 70 | A |  | 5 | 5 |  | baja |

## CEventLeaderEnteredCarAsDriver (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0048E220 | 90 | C |  | 3 | 0 |  | baja |
| 0x0048E280 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0048E290 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0048E2A0 | 96 | B |  | 0 | 0 | sí | baja |
| 0x0048E300 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0048E310 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0048E320 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0048E340 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B0F70 | 5 | B |  | 0 | 0 | sí | baja |

## CCheat (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00438D60 | 223 | A |  | 1 | 1 |  | alta |
| 0x0043A6A0 | 23 | A |  | 4 | 3 |  | baja |

## CMaths (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059ACF0 | 35 | A |  | 2 | 2 |  | baja |
| 0x0059AD20 | 77 | A |  | 9 | 9 |  | baja |
| 0x0059ADD0 | 20 | A |  | 1 | 1 |  | baja |
| 0x0059ADF0 | 114 | A |  | 3 | 2 |  | baja |

## CTaskComplexGoToBoatSteeringWheel (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006490F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00649160 | 172 | A |  | 2 | 1 |  | baja |
| 0x0064E480 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064E4B0 | 41 | A |  | 3 | 3 |  | baja |

## CFont (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00718E50 | 117 | A |  | 2 | 2 |  | alta |
| 0x00719240 | 115 | A |  | 3 | 3 |  | alta |
| 0x00719360 | 19 | A |  | 2 | 2 |  | alta |

## CTaskComplexCopInCar (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0068CD10 | 96 | B |  | 0 | 0 | sí | baja |
| 0x0068CD70 | 96 | B |  | 0 | 0 | sí | baja |
| 0x0068CF40 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0068CF60 | 30 | B |  | 0 | 0 | sí | baja |

## CWaterLevel (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006E67A0 | 208 | D |  | 0 | 0 |  | alta |
| 0x006E76E0 | 11 | C |  | 1 | 0 |  | alta |
| 0x006E8550 | 34 | A |  | 4 | 4 |  | alta |

## CTaskSimpleGangDriveBy (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006218C0 | 148 | A |  | 7 | 3 |  | baja |
| 0x00623750 | 106 | B |  | 0 | 0 | sí | baja |

## CClumpModelInfo (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C5110 | 41 | B | CClumpModelInfo::CreateInstance-mat | 0 | 0 | sí | alta |
| 0x004C5140 | 189 | B | CClumpModelInfo::CreateInstance-void | 1 | 0 | sí | alta |
| 0x004C5750 | 7 | C |  | 2 | 0 |  | baja |
| 0x004C5760 | 18 | D |  | 0 | 0 |  | baja |

## Common (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0054ED20 | 105 | D |  | 0 | 0 |  | alta |
| 0x0054EEA0 | 79 | A |  | 3 | 3 |  | alta |
| 0x0071DB30 | 76 | A |  | 2 | 2 |  | alta |

## CPedClothesDesc (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A7A20 | 263 | A |  | 2 | 2 |  | baja |

## CAEWeatherAudioEntity (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00506B20 | 108 | A |  | 3 | 1 |  | baja |
| 0x00506BA0 | 128 | A |  | 3 | 1 |  | baja |
| 0x00506C30 | 14 | C |  | 2 | 0 |  | baja |
| 0x00506C40 | 14 | C |  | 2 | 0 |  | baja |

## PipelinePlugin (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0072FC70 | 273 | A |  | 6 | 6 |  | baja |

## CTaskComplexWalkRoundObject (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006550C0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006550D0 | 106 | C |  | 1 | 0 |  | baja |
| 0x00655290 | 161 | A |  | 1 | 1 |  | baja |

## CAudioZones (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00508930 | 102 | C |  | 1 | 0 |  | baja |
| 0x00508A10 | 17 | D |  | 0 | 0 |  | baja |
| 0x00509140 | 17 | C |  | 1 | 0 |  | baja |
| 0x00509230 | 139 | D |  | 0 | 0 |  | baja |

## CDecisionMakerTypes (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00605300 | 204 | A |  | 1 | 1 |  | baja |
| 0x006070F0 | 74 | C |  | 3 | 0 |  | baja |

## CTaskComplexBeInCouple (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00683A40 | 142 | A |  | 6 | 1 |  | baja |
| 0x00684D10 | 137 | B |  | 0 | 0 | sí | baja |

## CEvent (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ABFE0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x004ABFF0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x004AC000 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AC020 | 35 | B |  | 0 | 0 | sí | alta |
| 0x004AC0F0 | 50 | D |  | 0 | 0 |  | baja |
| 0x004AC130 | 45 | D |  | 0 | 0 |  | baja |
| 0x004AC160 | 87 | D |  | 0 | 0 |  | baja |
| 0x004B2950 | 50 | D |  | 0 | 0 |  | baja |

## CPedPlacement (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00616920 | 283 | A |  | 15 | 14 |  | alta |

## CEventPassObject (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065DCF0 | 90 | A |  | 5 | 2 |  | baja |
| 0x0065DD50 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0065DD60 | 3 | B |  | 0 | 0 | sí | baja |
| 0x0065DD70 | 102 | B |  | 0 | 0 | sí | baja |
| 0x0065DDE0 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0065DDF0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065DE10 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0065DE20 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0065DE60 | 36 | A |  | 1 | 1 |  | baja |
| 0x0065DE90 | 7 | C |  | 5 | 0 |  | baja |

## CStuntJumpManager (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049C8F0 | 91 | A |  | 1 | 1 |  | alta |
| 0x0049C950 | 54 | D |  | 0 | 0 |  | alta |
| 0x0049C9A0 | 48 | A |  | 1 | 1 |  | alta |
| 0x0049C9E0 | 103 | A |  | 2 | 2 |  | alta |

## cHandlingDataMgr (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006F4FA0 | 15 | D |  | 0 | 0 |  | alta |
| 0x006F5440 | 70 | A |  | 4 | 4 |  | baja |
| 0x006F54D0 | 85 | C |  | 1 | 0 |  | baja |
| 0x006F5530 | 49 | B |  | 0 | 0 | sí | baja |
| 0x006F5570 | 75 | B |  | 0 | 0 | sí | baja |
| 0x006F55C0 | 5 | B |  | 0 | 0 | sí | baja |

## Input (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x007448B0 | 123 | A |  | 1 | 1 |  | alta |
| 0x00744930 | 179 | A |  | 1 | 1 |  | alta |

## CTaskSimpleHoldEntity (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00691460 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00691630 | 166 | C |  | 1 | 0 |  | alta |
| 0x00693590 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006935B0 | 5 | C |  | 1 | 0 |  | baja |
| 0x006935C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006935E0 | 5 | C |  | 1 | 0 |  | baja |
| 0x006935F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006939D0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00646F40 | 158 | A |  | 1 | 1 |  | baja |
| 0x00646FE0 | 116 | A |  | 9 | 6 |  | baja |
| 0x0064B5B0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskInteriorSitInChair (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00675C80 | 6 | D |  | 0 | 0 |  | baja |
| 0x00675C90 | 93 | C |  | 1 | 0 |  | baja |
| 0x00675CF0 | 106 | D |  | 0 | 0 |  | baja |
| 0x00675D60 | 99 | D |  | 0 | 0 |  | baja |

## FurnitureManager_c (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00591260 | 117 | C |  | 1 | 0 |  | baja |
| 0x005912E0 | 113 | C |  | 1 | 0 |  | baja |
| 0x00591360 | 76 | D |  | 0 | 0 |  | baja |

## CAEWaterCannonAudioEntity (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00502F80 | 223 | A |  | 4 | 4 |  | alta |
| 0x00728AB0 | 86 | C |  | 1 | 0 |  | baja |

## CPedDamageResponseCalculator (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AD800 | 46 | D |  | 0 | 0 |  | baja |
| 0x004B5C2A | 197 | D |  | 0 | 0 |  | baja |
| 0x004B5CF0 | 76 | B |  | 0 | 0 | sí | baja |

## CTaskComplexHitPedWithCar (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00653AA0 | 51 | D |  | 0 | 0 |  | alta |
| 0x006563B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006563D0 | 208 | A |  | 1 | 1 |  | baja |
| 0x00657B60 | 30 | B |  | 0 | 0 | sí | baja |

## CStats (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00558F90 | 6 | A |  | 2 | 2 |  | alta |
| 0x0055D0E0 | 314 | A |  | 1 | 1 |  | baja |

## CTaskComplexOpenPassengerDoor (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00640430 | 131 | A |  | 5 | 4 |  | baja |
| 0x006404C0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006404D0 | 90 | C |  | 1 | 0 |  | baja |
| 0x00643950 | 100 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSmartFleeEntity (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065C530 | 328 | A |  | 3 | 3 |  | alta |

## CTaskGoToVehicleAndLean (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00660ED0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00664770 | 295 | A |  | 2 | 2 |  | baja |
| 0x006648A0 | 30 | B |  | 0 | 0 | sí | baja |

## CEntitySeekPosCalculatorFixedPos (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00694850 | 335 | C |  | 1 | 0 |  | baja |

## CCopPed (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DE2C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005DE2E0 | 18 | D |  | 0 | 0 |  | baja |
| 0x005DE300 | 11 | C |  | 1 | 0 |  | baja |
| 0x005DE310 | 30 | D |  | 0 | 0 |  | baja |
| 0x005DE340 | 97 | A |  | 6 | 2 |  | baja |
| 0x005DE3B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005DE3E0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005DE400 | 108 | D |  | 0 | 0 |  | baja |

## CTask (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00421190 | 4 | B |  | 0 | 0 | sí | baja |
| 0x004211A0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004211B0 | 8 | B |  | 0 | 0 | sí | baja |
| 0x0061A620 | 59 | D |  | 0 | 0 |  | baja |
| 0x0061A660 | 56 | B |  | 0 | 0 | sí | baja |
| 0x0061A6C0 | 47 | A |  | 2 | 1 |  | baja |
| 0x0061A6F0 | 159 | C |  | 6 | 0 |  | baja |

## CPedGeometryAnalyser (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005F6190 | 108 | A |  | 5 | 4 |  | baja |
| 0x005F6200 | 90 | A |  | 9 | 4 |  | baja |
| 0x005F6280 | 110 | D |  | 0 | 0 |  | baja |
| 0x005F6300 | 30 | D |  | 0 | 0 |  | baja |

## InteriorManager_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005984C0 | 189 | C |  | 1 | 0 |  | alta |
| 0x00598580 | 151 | C |  | 1 | 0 |  | alta |

## CQuaternion (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059CA10 | 307 | A |  | 1 | 0 |  | baja |
| 0x0059CBA0 | 7 | C |  | 1 | 0 |  | baja |
| 0x0059CD50 | 27 | D |  | 0 | 0 |  | baja |

## CEventAcquaintancePedHateBadlyLit (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AFB40 | 5 | B |  | 0 | 0 | sí | baja |
| 0x004AFB50 | 102 | C |  | 1 | 0 |  | baja |
| 0x004AFBF0 | 81 | C |  | 1 | 0 |  | baja |
| 0x005FF2C0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF2D0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF2E0 | 104 | B |  | 0 | 0 | sí | baja |
| 0x005FF350 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005FF360 | 30 | B |  | 0 | 0 | sí | baja |

## CPlane (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006C9160 | 231 | C |  | 1 | 0 |  | alta |
| 0x006CADB0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006CDC30 | 85 | C |  | 1 | 0 |  | baja |

## CWeather (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0072A620 | 17 | C |  | 1 | 0 |  | alta |
| 0x0072A640 | 335 | A |  | 3 | 3 |  | alta |

## CTaskAllocatorPool (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0069D9C0 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DA00 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DA40 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DA80 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DAC0 | 56 | B |  | 0 | 0 | sí | baja |
| 0x0069DB00 | 59 | B |  | 0 | 0 | sí | baja |
| 0x0069DB40 | 11 | C |  | 1 | 0 |  | baja |

## CTaskComplexGangFollower (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0065EBA0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0065ED40 | 329 | A |  | 2 | 2 |  | alta |
| 0x006634E0 | 30 | B |  | 0 | 0 | sí | baja |

## CEventPlayerCommandToGroupGather (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00609270 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609280 | 90 | B |  | 0 | 0 | sí | baja |
| 0x006092E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00609300 | 11 | C |  | 1 | 0 |  | baja |
| 0x00609310 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609320 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00609330 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006093B0 | 217 | C |  | 1 | 0 |  | baja |

## CColStore (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00410610 | 54 | D |  | 0 | 0 |  | alta |
| 0x00410F80 | 91 | A |  | 1 | 1 |  | alta |
| 0x00410FE0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00411030 | 103 | A |  | 1 | 1 |  | alta |
| 0x004110E0 | 71 | D |  | 0 | 0 |  | alta |

## CTaskComplexGoToPointAndStandStillAndAchieveHeading (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00668D50 | 292 | A |  | 4 | 4 |  | alta |
| 0x0066E050 | 90 | C |  | 1 | 0 |  | baja |

## VideoMode (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00745CC0 | 252 | C |  | 1 | 0 |  | baja |
| 0x00745DC0 | 130 | C |  | 1 | 0 |  | baja |

## CTaskComplexWalkRoundFire (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006557A0 | 111 | B |  | 0 | 0 | sí | baja |
| 0x00655DB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655DD0 | 5 | C |  | 1 | 0 |  | baja |
| 0x00655DE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655E00 | 5 | C |  | 1 | 0 |  | baja |
| 0x00655E30 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00655E80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006586A0 | 42 | A |  | 1 | 1 |  | baja |
| 0x006586D0 | 102 | D |  | 0 | 0 |  | baja |

## CHandObject (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059EEB0 | 297 | A |  | 6 | 6 |  | baja |
| 0x0059EFE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0059F000 | 5 | C |  | 1 | 0 |  | baja |
| 0x0059F050 | 54 | A |  | 1 | 1 |  | baja |

## CAEAmbienceTrackManager (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D6CC0 | 40 | A |  | 5 | 5 |  | alta |
| 0x004D6DA0 | 70 | A |  | 1 | 1 |  | alta |
| 0x004D6DF0 | 97 | A |  | 1 | 1 |  | alta |
| 0x004D7890 | 115 | A |  | 2 | 1 |  | baja |
| 0x004D7910 | 64 | A |  | 4 | 1 |  | baja |

## CTaskSimpleCarFallOut (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00648E90 | 80 | A |  | 2 | 2 |  | alta |
| 0x0064CD60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064CD80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0064CDA0 | 218 | A |  | 3 | 3 |  | baja |
| 0x0064CE80 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGotoDoorAndOpen (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066BB90 | 6 | B |  | 0 | 0 | sí | alta |
| 0x0066BC80 | 28 | B |  | 0 | 0 | sí | alta |
| 0x0066C280 | 151 | D |  | 0 | 0 |  | baja |
| 0x0066C3E0 | 86 | A |  | 3 | 1 |  | baja |
| 0x0066C450 | 106 | C |  | 3 | 0 |  | baja |
| 0x0066C4C0 | 11 | C |  | 1 | 0 |  | baja |

## CTxdStore (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x007316D0 | 13 | A |  | 1 | 1 |  | alta |
| 0x00731800 | 54 | D |  | 0 | 0 |  | alta |
| 0x00731AD0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00731B30 | 54 | D |  | 0 | 0 |  | alta |
| 0x00731B80 | 104 | A |  | 2 | 2 |  | alta |
| 0x00731C20 | 71 | D |  | 0 | 0 |  | alta |

## CEventKnockOffBike (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AFCB0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AFCC0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AFCD0 | 7 | B |  | 0 | 0 | sí | alta |
| 0x004AFCE0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004B06F0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B0810 | 138 | D |  | 0 | 0 |  | baja |
| 0x004B4EE0 | 232 | D |  | 0 | 0 |  | baja |

## CPopulation (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00407000 | 26 | A |  | 1 | 1 |  | baja |
| 0x005BD420 | 113 | A |  | 1 | 1 |  | baja |
| 0x005BD4E0 | 45 | D |  | 0 | 0 |  | baja |
| 0x005BD510 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD540 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD570 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD5A0 | 43 | D |  | 0 | 0 |  | baja |
| 0x005BD5D0 | 43 | D |  | 0 | 0 |  | baja |

## CTaskSimpleDieInCar (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062FC80 | 41 | D |  | 0 | 0 |  | baja |
| 0x0062FCB0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00635F90 | 124 | B |  | 0 | 0 | sí | baja |
| 0x00636010 | 80 | B |  | 0 | 0 | sí | baja |
| 0x00636060 | 152 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleSwim (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006889F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00688A00 | 141 | C |  | 1 | 0 |  | alta |
| 0x0068B620 | 91 | A |  | 2 | 1 |  | baja |
| 0x0068B680 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0068B690 | 162 | C |  | 1 | 0 |  | baja |

## CTaskComplexDieInCar (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0062FCF0 | 11 | C |  | 1 | 0 |  | alta |
| 0x0062FEC0 | 62 | A |  | 1 | 1 |  | baja |
| 0x0062FF00 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0062FF10 | 11 | C |  | 1 | 0 |  | baja |
| 0x0062FF20 | 18 | D |  | 0 | 0 |  | baja |
| 0x0062FF40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0062FF50 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0062FF60 | 11 | C |  | 1 | 0 |  | baja |
| 0x006378D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006378F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637910 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637930 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637950 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637970 | 96 | B |  | 0 | 0 | sí | baja |
| 0x006379D0 | 30 | B |  | 0 | 0 | sí | baja |

## CVehicleRecording (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0045AD40 | 316 | C |  | 1 | 0 |  | baja |
| 0x0045AF40 | 47 | A |  | 4 | 2 |  | baja |
| 0x0045AFB0 | 54 | A |  | 5 | 1 |  | baja |

## CPedIntelligence (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00602240 | 93 | D |  | 0 | 0 |  | alta |
| 0x00607560 | 326 | A |  | 1 | 0 |  | baja |

## CTaskComplexWalkRoundBuildingAttempt (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00654840 | 257 | A |  | 4 | 4 |  | alta |
| 0x00655C10 | 104 | B |  | 0 | 0 | sí | baja |
| 0x00657200 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00657750 | 30 | B |  | 0 | 0 | sí | baja |

## CTheZones (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00572130 | 69 | A |  | 3 | 2 |  | alta |
| 0x00572DB0 | 26 | A |  | 3 | 3 |  | baja |
| 0x00572E70 | 24 | A |  | 1 | 1 |  | baja |
| 0x005D3020 | 149 | A |  | 1 | 1 |  | baja |
| 0x005D30C0 | 154 | A |  | 1 | 1 |  | baja |

## CGame (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0053BC60 | 26 | A |  | 1 | 1 |  | alta |
| 0x0053C860 | 39 | D |  | 0 | 0 |  | alta |
| 0x0053C890 | 39 | D |  | 0 | 0 |  | alta |
| 0x0072FEC0 | 46 | D |  | 0 | 0 |  | baja |
| 0x0072FEF0 | 66 | D |  | 0 | 0 |  | baja |
| 0x007301D0 | 46 | D |  | 0 | 0 |  | baja |
| 0x00730200 | 66 | D |  | 0 | 0 |  | baja |
| 0x00730450 | 25 | D |  | 0 | 0 |  | baja |
| 0x007304B0 | 75 | C |  | 1 | 0 |  | baja |

## CTaskSimpleHitFromFront (21)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00637300 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637320 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637330 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637350 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637360 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637380 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637390 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006373B0 | 5 | C |  | 1 | 0 |  | baja |
| 0x006373C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006373E0 | 5 | C |  | 1 | 0 |  | baja |
| 0x006373F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637410 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637420 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637440 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637450 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637470 | 5 | C |  | 1 | 0 |  | baja |
| 0x00637480 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006374A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006374C0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006374E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00637500 | 30 | B |  | 0 | 0 | sí | baja |

## CAnimManager (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D40A0 | 12 | D |  | 0 | 0 |  | alta |
| 0x004D40C0 | 64 | A |  | 2 | 2 |  | alta |
| 0x004D56F0 | 249 | C |  | 2 | 0 |  | baja |
| 0x004D5980 | 46 | C |  | 1 | 0 |  | baja |
| 0x004D59B0 | 40 | C |  | 1 | 0 |  | baja |
| 0x004D59E0 | 23 | A |  | 2 | 2 |  | baja |

## Fx_c (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049EA70 | 5 | A |  | 3 | 0 |  | alta |
| 0x0049EA80 | 12 | D |  | 0 | 0 |  | alta |
| 0x0049EA90 | 66 | A |  | 1 | 1 |  | alta |
| 0x0049EAE0 | 21 | A |  | 1 | 1 |  | alta |
| 0x004A11C0 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A11E0 | 153 | A |  | 1 | 1 |  | baja |
| 0x004A1280 | 67 | A |  | 1 | 1 |  | baja |
| 0x004A12D0 | 57 | A |  | 1 | 0 |  | baja |
| 0x004A1320 | 26 | A |  | 1 | 1 |  | baja |

## CPtrNodeDoubleLinkPool (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00552380 | 11 | A |  | 2 | 1 |  | baja |
| 0x00552390 | 41 | A |  | 3 | 1 |  | baja |
| 0x005523C0 | 11 | A |  | 8 | 7 |  | baja |
| 0x005523D0 | 48 | A |  | 7 | 6 |  | baja |
| 0x00552400 | 99 | A |  | 9 | 7 |  | baja |
| 0x00552470 | 112 | A |  | 15 | 10 |  | baja |
| 0x00552520 | 114 | A |  | 5 | 3 |  | baja |

## CTaskComplexWanderCop (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00460CD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00460CE0 | 102 | B |  | 0 | 0 | sí | baja |
| 0x00460D50 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00460D60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00460D80 | 87 | A |  | 2 | 1 |  | baja |
| 0x00674EA0 | 48 | A |  | 2 | 2 |  | baja |
| 0x00674ED0 | 11 | C |  | 3 | 0 |  | baja |
| 0x00674F00 | 122 | D |  | 0 | 0 |  | baja |
| 0x00674F90 | 30 | D |  | 0 | 0 |  | baja |

## CObjectPool (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004048E0 | 41 | A |  | 297 | 1 |  | baja |
| 0x005A1D10 | 90 | A |  | 5 | 3 |  | baja |
| 0x005A1DF0 | 230 | A |  | 2 | 2 |  | baja |
| 0x005A1EF0 | 36 | A |  | 2 | 2 |  | baja |
| 0x005A1F20 | 49 | C |  | 33 | 0 |  | baja |

## COnscreenCounterEntry (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0044CB10 | 167 | A |  | 1 | 0 |  | baja |
| 0x0044CC20 | 258 | A |  | 1 | 1 |  | baja |
| 0x0044CD30 | 25 | A |  | 1 | 1 |  | baja |

## CTaskComplexSeekEntityAiming (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00695890 | 96 | B |  | 0 | 0 | sí | alta |
| 0x00695B20 | 152 | D |  | 0 | 0 |  | alta |
| 0x00695BC0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00695BE0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00695C00 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00695DC0 | 113 | C |  | 1 | 0 |  | baja |

## CTaskComplexPartnerGreet (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00681E10 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00681E20 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00684E30 | 154 | B |  | 0 | 0 | sí | baja |
| 0x00684ED0 | 142 | B |  | 0 | 0 | sí | baja |
| 0x00684F60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684F80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684FA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684FC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00684FE0 | 30 | B |  | 0 | 0 | sí | baja |

## Game (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0053E600 | 353 | A |  | 2 | 2 |  | alta |
| 0x0053E8D0 | 63 | A |  | 3 | 3 |  | alta |
| 0x005BF3D0 | 44 | D |  | 0 | 0 |  | baja |

## CShopping (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049ABD0 | 184 | A |  | 2 | 2 |  | alta |
| 0x0049ACD0 | 74 | C |  | 1 | 0 |  | alta |
| 0x0049BDD0 | 120 | C |  | 1 | 0 |  | alta |
| 0x0049C3B0 | 28 | A |  | 1 | 1 |  | baja |
| 0x0049C440 | 54 | D |  | 0 | 0 |  | baja |

## CTaskSimpleSetCharIgnoreWeaponRangeFlag (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00477DA0 | 481 | C |  | 1 | 0 |  | baja |

## CPlantMgr (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005DBE40 | 104 | A |  | 2 | 2 |  | alta |
| 0x005DD1E0 | 57 | A |  | 1 | 0 |  | baja |
| 0x005DD220 | 323 | A |  | 2 | 2 |  | baja |

## CTaskSimpleDuckToggle (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00635CF0 | 168 | B |  | 0 | 0 | sí | baja |
| 0x00635DA0 | 178 | B |  | 0 | 0 | sí | baja |
| 0x00635E60 | 142 | B |  | 0 | 0 | sí | baja |

## ProcSurfaceInfo_c (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A2F80 | 229 | A |  | 1 | 1 |  | baja |
| 0x005A3070 | 12 | D |  | 0 | 0 |  | baja |
| 0x005A3090 | 122 | C |  | 1 | 0 |  | baja |
| 0x005A3E20 | 126 | C |  | 1 | 0 |  | baja |

## CEventAttractor (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AF3D0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x004AF3E0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004AF3F0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B74B0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7520 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B7590 | 154 | B |  | 0 | 0 | sí | baja |
| 0x004B7630 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B76A0 | 33 | B |  | 0 | 0 | sí | baja |

## CTaskComplexStealCar (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0063B960 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0063BA00 | 157 | A |  | 3 | 3 |  | baja |
| 0x0063BAB0 | 34 | A |  | 2 | 2 |  | baja |
| 0x0063BAE0 | 122 | A |  | 2 | 2 |  | baja |
| 0x0063D9E0 | 116 | B |  | 0 | 0 | sí | baja |
| 0x006410E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00642250 | 30 | B |  | 0 | 0 | sí | baja |

## CPlayerPed (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0060A9A0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x0060D5B0 | 465 | A |  | 3 | 3 |  | alta |
| 0x0060F8C0 | 1 | A |  | 1 | 1 |  | baja |

## CTaskSimpleDuck (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00692680 | 85 | C |  | 1 | 0 |  | baja |
| 0x006926E0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006926F0 | 11 | C |  | 1 | 0 |  | baja |
| 0x00692700 | 167 | B |  | 0 | 0 | sí | baja |
| 0x00692D60 | 99 | B |  | 0 | 0 | sí | baja |
| 0x00692DD0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00693B80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00694640 | 68 | B |  | 0 | 0 | sí | baja |

## CEventPotentialWalkIntoFire (12)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1ED0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1EE0 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B1EF0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1F00 | 7 | A |  | 3 | 1 |  | baja |
| 0x004B1F10 | 60 | A |  | 1 | 1 |  | baja |
| 0x004B1F80 | 7 | A |  | 3 | 1 |  | baja |
| 0x004B1F90 | 45 | D |  | 0 | 0 |  | baja |
| 0x004B6990 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B69E0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6A30 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6A80 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6AD0 | 76 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleIKPointArm (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00634440 | 163 | C |  | 5 | 0 |  | baja |
| 0x006344F0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00634500 | 90 | C |  | 1 | 0 |  | baja |
| 0x00634560 | 176 | B |  | 0 | 0 | sí | baja |
| 0x00634610 | 5 | B |  | 0 | 0 | sí | baja |
| 0x00634620 | 76 | B |  | 0 | 0 | sí | baja |

## CCamera (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0050D590 | 527 | A |  | 1 | 1 |  | baja |

## CEventScriptedAttractor (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FEF70 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FEF80 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FEF90 | 93 | B |  | 0 | 0 | sí | baja |
| 0x005FEFF0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FF030 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF050 | 88 | D |  | 0 | 0 |  | baja |
| 0x005FF0B0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF0F0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF110 | 88 | D |  | 0 | 0 |  | baja |
| 0x005FF170 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF1B0 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF1D0 | 88 | D |  | 0 | 0 |  | baja |
| 0x005FF230 | 30 | D |  | 0 | 0 |  | baja |

## CPedAttractorPool (12)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005EAAA0 | 30 | A |  | 1 | 0 |  | baja |
| 0x005EAAC0 | 41 | A |  | 1 | 0 |  | baja |
| 0x005EAAF0 | 30 | A |  | 1 | 0 |  | baja |
| 0x005EAB10 | 30 | A |  | 1 | 0 |  | baja |
| 0x005EAB80 | 62 | A |  | 1 | 1 |  | baja |
| 0x005EAC50 | 62 | A |  | 1 | 0 |  | baja |
| 0x005EACC0 | 59 | A |  | 1 | 0 |  | baja |
| 0x005EAE10 | 37 | A |  | 3 | 0 |  | baja |
| 0x005EAE40 | 48 | A |  | 3 | 0 |  | baja |
| 0x005EAE70 | 37 | A |  | 3 | 0 |  | baja |
| 0x005EAEA0 | 57 | A |  | 3 | 0 |  | baja |
| 0x005EAEE0 | 37 | A |  | 3 | 0 |  | baja |

## CStreaming (17)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00407670 | 27 | D |  | 0 | 0 |  | alta |
| 0x00407800 | 20 | C |  | 1 | 0 |  | alta |
| 0x00407A10 | 17 | A |  | 1 | 1 |  | alta |
| 0x00407A30 | 10 | A |  | 4 | 4 |  | alta |
| 0x00407AC0 | 1 | A |  | 2 | 2 |  | alta |
| 0x00407BE0 | 1 | A |  | 10 | 10 |  | alta |
| 0x00407BF0 | 1 | A |  | 10 | 10 |  | alta |
| 0x00407D30 | 13 | A |  | 1 | 1 |  | alta |
| 0x00407DC0 | 13 | A |  | 1 | 1 |  | alta |
| 0x00407F20 | 1 | A |  | 2 | 2 |  | alta |
| 0x00408230 | 64 | A |  | 3 | 3 |  | baja |
| 0x004082A0 | 31 | A |  | 6 | 1 |  | baja |
| 0x004096C0 | 11 | A |  | 1 | 1 |  | alta |
| 0x0040B490 | 18 | A |  | 2 | 1 |  | alta |
| 0x0040C1C0 | 18 | C |  | 1 | 0 |  | alta |
| 0x0040C450 | 195 | A |  | 3 | 3 |  | alta |
| 0x005B8340 | 94 | A |  | 1 | 1 |  | baja |

## CStencilShadows (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0070FA70 | 107 | A |  | 6 | 6 |  | alta |
| 0x00710BA0 | 30 | A |  | 1 | 1 |  | alta |
| 0x00711160 | 136 | A |  | 3 | 3 |  | baja |
| 0x007111F0 | 136 | A |  | 2 | 2 |  | baja |
| 0x00711280 | 134 | A |  | 3 | 3 |  | baja |

## CTaskComplexRoadRage (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00622180 | 41 | D |  | 0 | 0 |  | baja |
| 0x006221B0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006221C0 | 11 | C |  | 1 | 0 |  | baja |
| 0x006221D0 | 89 | C |  | 2 | 0 |  | baja |
| 0x00622230 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006293A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006293C0 | 178 | B |  | 0 | 0 | sí | baja |
| 0x00629480 | 194 | B |  | 0 | 0 | sí | baja |
| 0x00629550 | 30 | B |  | 0 | 0 | sí | baja |

## CDecisionMakerTypesFileLoader (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00607A90 | 606 | A |  | 2 | 1 |  | alta |

## CEventEditableResponse (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00420F00 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00420F10 | 4 | B |  | 0 | 0 | sí | baja |
| 0x00420F20 | 3 | B |  | 0 | 0 | sí | baja |
| 0x00420F30 | 11 | C |  | 14 | 0 |  | baja |
| 0x00420F40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00420F50 | 88 | B |  | 0 | 0 | sí | baja |
| 0x00420FB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00420FD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B58E0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5930 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5980 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B59D0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5A20 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5A70 | 76 | D |  | 0 | 0 |  | baja |

## Rs (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00619440 | 5 | A |  | 12 | 12 |  | alta |
| 0x00619780 | 181 | A |  | 1 | 1 |  | alta |
| 0x006198C0 | 65 | A |  | 1 | 1 |  | alta |
| 0x00619910 | 410 | A |  | 1 | 1 |  | alta |

## CEntryExitManager (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0043EEB0 | 54 | D |  | 0 | 0 |  | alta |
| 0x0043EF00 | 30 | C |  | 3 | 0 |  | alta |
| 0x0043EF20 | 112 | C |  | 2 | 0 |  | alta |
| 0x0043EF90 | 59 | C |  | 1 | 0 |  | alta |
| 0x0043F720 | 91 | A |  | 1 | 1 |  | alta |
| 0x0043F780 | 54 | D |  | 0 | 0 |  | alta |
| 0x0043F7D0 | 103 | A |  | 1 | 1 |  | alta |
| 0x0043F950 | 71 | D |  | 0 | 0 |  | alta |
| 0x0043F9B0 | 24 | C |  | 2 | 0 |  | alta |
| 0x004410E0 | 23 | A |  | 1 | 1 |  | baja |
| 0x00441130 | 54 | A |  | 8 | 4 |  | baja |

## CTaskComplexFallAndGetUp (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006786F0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00678790 | 11 | C |  | 1 | 0 |  | alta |
| 0x006789D0 | 34 | D |  | 0 | 0 |  | baja |
| 0x00678A00 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00678A10 | 110 | D |  | 0 | 0 |  | baja |
| 0x00678A90 | 11 | C |  | 1 | 0 |  | baja |
| 0x00678AA0 | 118 | B |  | 0 | 0 | sí | baja |
| 0x00678B20 | 145 | B |  | 0 | 0 | sí | baja |
| 0x00678BC0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00678BD0 | 167 | C |  | 1 | 0 |  | baja |
| 0x0067CBC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0067CBE0 | 45 | B |  | 0 | 0 | sí | baja |
| 0x0067CC10 | 30 | B |  | 0 | 0 | sí | baja |

## CQuadTreeNode<T> (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00552830 | 63 | A |  | 3 | 3 |  | alta |
| 0x00552AF0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00552B50 | 104 | A |  | 2 | 1 |  | alta |
| 0x00552BC0 | 49 | A |  | 1 | 0 |  | alta |
| 0x00552C80 | 11 | A |  | 3 | 3 |  | alta |
| 0x00552C90 | 49 | A |  | 7 | 3 |  | alta |
| 0x00552EE0 | 144 | D |  | 0 | 0 |  | baja |
| 0x00553010 | 206 | D |  | 0 | 0 |  | baja |
| 0x005531C0 | 1 | A |  | 1 | 1 |  | baja |
| 0x005531D0 | 1 | A |  | 1 | 1 |  | baja |

## CObject (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0059F610 | 74 | A |  | 1 | 0 |  | alta |
| 0x0059F660 | 480 | C |  | 6 | 0 |  | alta |
| 0x005A2BD0 | 76 | B |  | 0 | 0 | sí | alta |
| 0x005A2CB0 | 59 | A |  | 5 | 5 |  | baja |
| 0x005D28B0 | 19 | C |  | 2 | 0 |  | baja |
| 0x005D28D0 | 15 | D |  | 0 | 0 |  | baja |
| 0x005D28F0 | 5 | C |  | 2 | 0 |  | baja |

## CEventScriptCommand (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B0A40 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004B64D0 | 77 | B |  | 0 | 0 | sí | baja |
| 0x004B6520 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6570 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B65C0 | 77 | B |  | 0 | 0 | sí | baja |
| 0x004B6660 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B66B0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6700 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B67A0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B67F0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6840 | 77 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleThrowProjectile (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00625B60 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625B80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625BA0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625BC0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625BE0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625C00 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00625C20 | 68 | B |  | 0 | 0 | sí | baja |
| 0x00625C70 | 429 | B |  | 0 | 0 | sí | baja |
| 0x00625E20 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00626A80 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00626AA0 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexUseWaterCannon (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006597A0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006597B0 | 11 | C |  | 1 | 0 |  | alta |
| 0x00659870 | 43 | D |  | 0 | 0 |  | baja |
| 0x006598A0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006598B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x006598C0 | 180 | B |  | 0 | 0 | sí | baja |
| 0x00659980 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00659990 | 120 | C |  | 1 | 0 |  | baja |
| 0x00659D70 | 106 | B |  | 0 | 0 | sí | baja |
| 0x00659DE0 | 30 | D |  | 0 | 0 |  | baja |
| 0x00659E00 | 5 | C |  | 1 | 0 |  | baja |
| 0x00659E10 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00659E70 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065A700 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065A720 | 155 | B |  | 0 | 0 | sí | baja |

## CTrafficLights (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049E620 | 15 | C |  | 1 | 0 |  | baja |
| 0x0049E640 | 10 | A |  | 1 | 1 |  | baja |
| 0x0049E650 | 10 | A |  | 2 | 2 |  | baja |
| 0x0049E660 | 493 | A |  | 2 | 0 |  | baja |
| 0x0049E850 | 242 | A |  | 1 | 0 |  | baja |

## CTaskComplexWaitForPed (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006833C0 | 6 | D |  | 0 | 0 |  | baja |
| 0x006833D0 | 90 | C |  | 1 | 0 |  | baja |
| 0x00683430 | 5 | D |  | 0 | 0 |  | baja |
| 0x00683440 | 5 | D |  | 0 | 0 |  | baja |
| 0x00683450 | 138 | D |  | 0 | 0 |  | baja |
| 0x006834E0 | 528 | D |  | 0 | 0 |  | baja |

## CTaskComplexWander (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00669FB0 | 54 | A |  | 1 | 1 |  | baja |
| 0x00669FF0 | 265 | A |  | 1 | 1 |  | baja |
| 0x0066F4A0 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00673EF0 | 448 | D |  | 0 | 0 |  | alta |

## CModelInfo (22)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C5CB0 | 39 | C |  | 1 | 0 |  | alta |
| 0x004C5D10 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5D70 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5DD0 | 39 | C |  | 1 | 0 |  | alta |
| 0x004C5E30 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5E90 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5EF0 | 36 | C |  | 1 | 0 |  | alta |
| 0x004C5F50 | 42 | C |  | 1 | 0 |  | alta |
| 0x004C6200 | 5 | A |  | 9 | 9 |  | alta |
| 0x004C6210 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6230 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6250 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6270 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C6290 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C62B0 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C62D0 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C62F0 | 90 | C |  | 1 | 0 |  | alta |
| 0x004C6350 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C67D0 | 39 | C |  | 1 | 0 |  | alta |
| 0x004C6B10 | 11 | D |  | 0 | 0 |  | baja |
| 0x004C6B70 | 92 | D |  | 0 | 0 |  | baja |
| 0x004C6BD0 | 61 | D |  | 0 | 0 |  | baja |

## CCustomCarPlateMgr (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006FD7C0 | 829 | A |  | 1 | 1 |  | alta |

## CShadows (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00709CF0 | 837 | A | CShadows::ShadowRenderTriangleCB | 1 | 1 |  | alta |

## CVehicle (33)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0049B0E0 | 217 | A |  | 1 | 1 |  | baja |
| 0x006D0BC0 | 62 | D |  | 0 | 0 |  | alta |
| 0x006D3D50 | 9 | B |  | 0 | 0 | sí | alta |
| 0x006D56B0 | 9 | B |  | 0 | 0 | sí | alta |
| 0x006D5DD0 | 74 | A |  | 1 | 1 |  | alta |
| 0x006D5E20 | 227 | A |  | 1 | 1 |  | alta |
| 0x006D6270 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6280 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6290 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D62A0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62B0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62C0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62D0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62E0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D62F0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6300 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6310 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6320 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6330 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6340 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006D6350 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6360 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6370 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006D6380 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D6390 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63A0 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63B0 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63C0 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006D63D0 | 6 | B |  | 0 | 0 | sí | alta |
| 0x006D63E0 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006E2D90 | 49 | C |  | 72 | 0 |  | alta |
| 0x006E2DD0 | 49 | C |  | 21 | 0 |  | alta |
| 0x006E38A0 | 76 | B |  | 0 | 0 | sí | alta |

## CVehicleSaveStructure (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D24B0 | 172 | A |  | 1 | 1 |  | baja |
| 0x005D2560 | 186 | A |  | 1 | 1 |  | baja |
| 0x005D2620 | 282 | A |  | 1 | 1 |  | baja |
| 0x005D2740 | 234 | A |  | 1 | 1 |  | baja |

## Ps (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00745560 | 127 | C |  | 1 | 0 |  | alta |
| 0x007457C0 | 44 | D |  | 0 | 0 |  | alta |
| 0x007457F0 | 73 | D |  | 0 | 0 |  | alta |
| 0x00745920 | 327 | C |  | 2 | 0 |  | baja |
| 0x00745A70 | 5 | A |  | 2 | 2 |  | baja |
| 0x00746480 | 300 | C |  | 1 | 0 |  | alta |

## CPad (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0053F910 | 10 | A |  | 2 | 1 |  | alta |
| 0x005404F0 | 45 | A |  | 2 | 2 |  | alta |
| 0x00541D70 | 1 | A |  | 4 | 3 |  | alta |
| 0x00541DD0 | 113 | A |  | 5 | 3 |  | baja |
| 0x00541E50 | 5 | A |  | 1 | 1 |  | baja |
| 0x00541E90 | 23 | A |  | 1 | 1 |  | baja |
| 0x00541F40 | 38 | A |  | 6 | 6 |  | baja |
| 0x00746D80 | 322 | A |  | 2 | 2 |  | baja |
| 0x00746ED0 | 148 | A |  | 1 | 1 |  | baja |
| 0x00747000 | 32 | A |  | 1 | 1 |  | baja |
| 0x00747020 | 158 | D |  | 0 | 0 |  | baja |

## CEventSeenCop (35)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FF3A0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF3B0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF3C0 | 88 | B |  | 0 | 0 | sí | baja |
| 0x005FF420 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FF440 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF460 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF490 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF4E0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF500 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF520 | 11 | C |  | 5 | 0 |  | baja |
| 0x005FF550 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF5A0 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF5C0 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF5E0 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF610 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF660 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF680 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF6A0 | 11 | C |  | 3 | 0 |  | baja |
| 0x005FF6D0 | 80 | D |  | 0 | 0 |  | baja |
| 0x005FF720 | 30 | D |  | 0 | 0 |  | baja |
| 0x005FF740 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF760 | 11 | C |  | 2 | 0 |  | baja |
| 0x005FF770 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF780 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005FF790 | 80 | B |  | 0 | 0 | sí | baja |
| 0x005FF7E0 | 7 | B |  | 0 | 0 | sí | baja |
| 0x005FF7F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x005FF820 | 18 | D |  | 0 | 0 |  | baja |
| 0x005FF840 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF850 | 3 | B |  | 0 | 0 | sí | baja |
| 0x005FF860 | 5 | C |  | 2 | 0 |  | baja |
| 0x005FF870 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF880 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF890 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005FF8A0 | 6 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSunbathe (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00632190 | 90 | A |  | 4 | 3 |  | baja |
| 0x00638290 | 725 | A |  | 12 | 12 |  | baja |
| 0x00638570 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00638590 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006385B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639E20 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskComplexMedicTreatInjuredPed (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00658C20 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00658C30 | 129 | C |  | 1 | 0 |  | alta |
| 0x006591D0 | 83 | C |  | 1 | 0 |  | baja |
| 0x00659230 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00659240 | 90 | C |  | 1 | 0 |  | baja |
| 0x006592A0 | 99 | C |  | 1 | 0 |  | baja |
| 0x00659B60 | 96 | B |  | 0 | 0 | sí | baja |
| 0x0065A0F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065A110 | 259 | C |  | 4 | 0 |  | baja |
| 0x0065A220 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0065AFE0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x0065B050 | 18 | B |  | 0 | 0 | sí | baja |
| 0x0065B070 | 31 | B |  | 0 | 0 | sí | baja |

## CTaskComplexGoToPointAndStandStill (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046FEE0 | 61 | A |  | 2 | 2 |  | baja |
| 0x00470070 | 32 | C |  | 2 | 0 |  | baja |
| 0x004700E0 | 18 | C |  | 1 | 0 |  | baja |
| 0x00470100 | 48 | C |  | 2 | 0 |  | baja |
| 0x00668240 | 11 | C |  | 1 | 0 |  | alta |
| 0x00668250 | 116 | A |  | 2 | 1 |  | alta |
| 0x006682D0 | 661 | A |  | 8 | 8 |  | alta |
| 0x0066DC70 | 30 | B |  | 0 | 0 | sí | baja |

## CTaskSimpleBeHit (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00620B00 | 121 | C |  | 1 | 0 |  | baja |
| 0x00620B80 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00620B90 | 90 | C |  | 1 | 0 |  | baja |
| 0x00620BF0 | 64 | B |  | 0 | 0 | sí | baja |
| 0x00620C30 | 451 | C |  | 4 | 0 |  | baja |
| 0x00620E00 | 41 | B |  | 0 | 0 | sí | baja |
| 0x00623300 | 112 | B |  | 0 | 0 | sí | baja |
| 0x00623370 | 137 | B |  | 0 | 0 | sí | baja |

## CCam (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0050A870 | 253 | C |  | 2 | 0 |  | baja |
| 0x00513220 | 164 | A |  | 3 | 3 |  | alta |
| 0x005132D0 | 568 | A |  | 4 | 4 |  | alta |
| 0x00514010 | 27 | D |  | 0 | 0 |  | baja |
| 0x00517740 | 23 | D |  | 0 | 0 |  | baja |
| 0x00517760 | 12 | D |  | 0 | 0 |  | baja |
| 0x005B2AB0 | 12 | D |  | 0 | 0 |  | baja |

## CTaskComplexStuckInAir (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0067BA70 | 11 | C |  | 1 | 0 |  | alta |
| 0x0067C0C0 | 110 | B |  | 0 | 0 | sí | baja |
| 0x0067C130 | 80 | B |  | 0 | 0 | sí | baja |
| 0x0067C180 | 120 | B |  | 0 | 0 | sí | baja |
| 0x0067C200 | 106 | B |  | 0 | 0 | sí | baja |
| 0x0067C270 | 98 | B |  | 0 | 0 | sí | baja |
| 0x0067C2E0 | 120 | B |  | 0 | 0 | sí | baja |
| 0x0067C360 | 129 | B |  | 0 | 0 | sí | baja |
| 0x0067C3F0 | 141 | B |  | 0 | 0 | sí | baja |
| 0x0067C480 | 130 | B |  | 0 | 0 | sí | baja |
| 0x0067C750 | 30 | B |  | 0 | 0 | sí | baja |

## CTrain (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006F5930 | 103 | B |  | 0 | 0 | sí | alta |
| 0x006F59C0 | 22 | B |  | 0 | 0 | sí | alta |
| 0x006F5EE0 | 5 | B |  | 0 | 0 | sí | alta |
| 0x006F6300 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006F6370 | 719 | B |  | 0 | 0 | sí | alta |
| 0x006F7190 | 197 | B |  | 0 | 0 | sí | alta |

## CPed (7)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00459350 | 47 | A |  | 3 | 3 |  | baja |
| 0x005E0540 | 74 | A |  | 1 | 1 |  | baja |
| 0x005E3630 | 400 | A |  | 1 | 1 |  | alta |
| 0x005E37C0 | 404 | A |  | 1 | 1 |  | alta |
| 0x005E47A0 | 49 | C |  | 3 | 0 |  | alta |
| 0x005E88E0 | 43 | A |  | 1 | 1 |  | alta |
| 0x005E8C80 | 76 | B |  | 0 | 0 | sí | baja |

## CTaskComplexInvestigateDisturbance (9)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066CA40 | 209 | A |  | 2 | 1 |  | baja |
| 0x0066CB90 | 30 | D |  | 0 | 0 |  | baja |
| 0x0066CBB0 | 97 | C |  | 1 | 0 |  | baja |
| 0x0066D440 | 112 | C |  | 2 | 0 |  | baja |
| 0x0066D4B0 | 155 | C |  | 1 | 0 |  | baja |
| 0x0066D550 | 348 | C |  | 1 | 0 |  | baja |
| 0x0066D6D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066D6F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00674BB0 | 114 | B |  | 0 | 0 | sí | baja |

## CTaskComplexSeekEntityAnyMeansXYOffset (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006956B0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006956D0 | 97 | C |  | 1 | 0 |  | baja |
| 0x00696260 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006962A0 | 981 | A |  | 7 | 7 |  | baja |

## CEventVehicleOnFire (17)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004B1130 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B1140 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004B1150 | 7 | B |  | 0 | 0 | sí | baja |
| 0x004B1160 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004B77B0 | 84 | B |  | 0 | 0 | sí | baja |
| 0x004B7810 | 44 | D |  | 0 | 0 |  | baja |
| 0x004B7840 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B78B0 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7960 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B79D0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7A40 | 43 | B |  | 0 | 0 | sí | baja |
| 0x004B7A70 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B7AC0 | 111 | B |  | 0 | 0 | sí | baja |
| 0x004B7B30 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7BA0 | 97 | B |  | 0 | 0 | sí | baja |
| 0x004B7C10 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7C80 | 78 | B |  | 0 | 0 | sí | baja |

## CEventVehicleCollision (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AC8F0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AC900 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004AC910 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AC980 | 5 | B |  | 0 | 0 | sí | baja |
| 0x004B6C40 | 126 | B |  | 0 | 0 | sí | baja |
| 0x004B6CC0 | 121 | B |  | 0 | 0 | sí | baja |
| 0x004B6D40 | 121 | B |  | 0 | 0 | sí | baja |
| 0x004B6DC0 | 107 | D |  | 0 | 0 |  | baja |
| 0x004B6E30 | 53 | D |  | 0 | 0 |  | baja |
| 0x004B6E70 | 107 | D |  | 0 | 0 |  | baja |
| 0x004B6EE0 | 90 | D |  | 0 | 0 |  | baja |
| 0x004B6F40 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B6FB0 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7020 | 101 | B |  | 0 | 0 | sí | baja |
| 0x004B7090 | 105 | B |  | 0 | 0 | sí | baja |

## CPlaceable (25)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00411A30 | 35 | A |  | 2 | 1 |  | baja |
| 0x00411BA0 | 4 | A |  | 6 | 6 |  | baja |
| 0x00411BF0 | 10 | A |  | 2 | 2 |  | baja |
| 0x0043E110 | 19 | D |  | 0 | 0 |  | baja |
| 0x0043E1A0 | 5 | C |  | 5 | 0 |  | baja |
| 0x0054F440 | 77 | C |  | 1 | 0 |  | alta |
| 0x0054F490 | 36 | A |  | 5 | 1 |  | alta |
| 0x0054F5D0 | 53 | B |  | 0 | 0 | sí | alta |
| 0x0054FA10 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FA60 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FAB0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FB00 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FB50 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FBA0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FBF0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FC40 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FC90 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FCE0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FD30 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FD80 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FDD0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FE20 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FE70 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FEC0 | 54 | D |  | 0 | 0 |  | baja |
| 0x0054FF10 | 54 | D |  | 0 | 0 |  | baja |

## Interior_c (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005921D0 | 87 | D |  | 0 | 0 |  | alta |
| 0x00597FE0 | 12 | D |  | 0 | 0 |  | baja |
| 0x0059A840 | 510 | A |  | 2 | 2 |  | baja |
| 0x0059AA40 | 591 | A |  | 2 | 2 |  | baja |

## CTaskSimpleRunTimedAnim (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061AC90 | 112 | C |  | 1 | 0 |  | baja |
| 0x0061AD10 | 222 | C |  | 1 | 0 |  | baja |
| 0x0061ADF0 | 82 | C |  | 1 | 0 |  | baja |
| 0x0061AE50 | 76 | A |  | 2 | 1 |  | baja |
| 0x0061AEA0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0061AEB0 | 146 | C |  | 1 | 0 |  | baja |
| 0x0061AF50 | 5 | B |  | 0 | 0 | sí | baja |
| 0x0061B890 | 226 | D |  | 0 | 0 |  | baja |
| 0x0061B980 | 118 | B |  | 0 | 0 | sí | baja |
| 0x0061BC80 | 30 | D |  | 0 | 0 |  | baja |
| 0x0061BCA0 | 5 | C |  | 1 | 0 |  | baja |
| 0x0061BCB0 | 193 | D |  | 0 | 0 |  | baja |
| 0x0061BD80 | 30 | B |  | 0 | 0 | sí | baja |

## CAttractorScanner (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FFFD0 | 429 | A |  | 2 | 1 |  | baja |
| 0x00600180 | 364 | A |  | 1 | 1 |  | baja |
| 0x006002F0 | 480 | C |  | 2 | 0 |  | baja |

## CClothesBuilder (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005A42B0 | 203 | A |  | 1 | 1 |  | alta |
| 0x005A44A0 | 24 | D |  | 0 | 0 |  | alta |
| 0x005A47E0 | 46 | D |  | 0 | 0 |  | alta |
| 0x005A6DB0 | 41 | C |  | 1 | 0 |  | baja |
| 0x005A6E60 | 54 | D |  | 0 | 0 |  | baja |
| 0x005A6FC0 | 122 | C |  | 4 | 0 |  | baja |
| 0x005A72B0 | 54 | D |  | 0 | 0 |  | baja |
| 0x005A7350 | 104 | C |  | 1 | 0 |  | baja |
| 0x005A73C0 | 49 | C |  | 1 | 0 |  | baja |
| 0x005A7490 | 83 | C |  | 1 | 0 |  | baja |
| 0x005A74F0 | 125 | C |  | 1 | 0 |  | baja |
| 0x005A7570 | 41 | C |  | 1 | 0 |  | baja |
| 0x005A75B0 | 339 | C |  | 1 | 0 |  | baja |

## CVehicleModelInfo (15)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C7410 | 85 | C |  | 2 | 0 |  | alta |
| 0x004C75E0 | 75 | D |  | 0 | 0 |  | alta |
| 0x004C7630 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C7650 | 3 | B |  | 0 | 0 | sí | alta |
| 0x004C7660 | 7 | B |  | 0 | 0 | sí | alta |
| 0x004C7670 | 95 | B |  | 0 | 0 | sí | alta |
| 0x004C76D0 | 37 | B |  | 0 | 0 | sí | alta |
| 0x004C8D60 | 161 | C |  | 1 | 0 |  | alta |
| 0x004C8E10 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004C94C0 | 106 | C |  | 1 | 0 |  | alta |
| 0x004C9580 | 49 | C |  | 1 | 0 |  | alta |
| 0x004C95C0 | 183 | B |  | 0 | 0 | sí | alta |
| 0x004C9680 | 343 | B |  | 0 | 0 | sí | alta |
| 0x004C9840 | 76 | D |  | 0 | 0 |  | baja |
| 0x004C9890 | 82 | B |  | 0 | 0 | sí | baja |

## CTaskComplexAvoidOtherPedWhileWandering (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066AA20 | 161 | C |  | 1 | 0 |  | baja |
| 0x0066AAD0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0066AAE0 | 90 | C |  | 1 | 0 |  | baja |
| 0x0066AB40 | 455 | C |  | 1 | 0 |  | baja |
| 0x0066AD10 | 46 | C |  | 1 | 0 |  | baja |
| 0x0066AD40 | 48 | B |  | 0 | 0 | sí | baja |
| 0x0066AD70 | 48 | B |  | 0 | 0 | sí | baja |
| 0x0066ADE0 | 220 | B |  | 0 | 0 | sí | baja |
| 0x0066D0C0 | 104 | B |  | 0 | 0 | sí | baja |
| 0x00672530 | 204 | B |  | 0 | 0 | sí | baja |

## CEntityScanner (8)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005FFE60 | 113 | A |  | 1 | 1 |  | baja |
| 0x005FFF30 | 55 | A |  | 1 | 1 |  | baja |
| 0x006034B0 | 181 | A |  | 2 | 2 |  | baja |
| 0x00603570 | 422 | A |  | 1 | 1 |  | baja |
| 0x00603A30 | 187 | A |  | 1 | 1 |  | baja |
| 0x00603AF0 | 241 | A |  | 4 | 0 |  | baja |
| 0x006040C0 | 8 | A |  | 7 | 6 |  | baja |
| 0x006042B0 | 236 | A |  | 1 | 0 |  | baja |

## CTaskInteriorUseInfo (3)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00676D10 | 30 | D |  | 0 | 0 |  | baja |
| 0x00676D30 | 1423 | D |  | 0 | 0 |  | baja |
| 0x006772C0 | 30 | B |  | 0 | 0 | sí | baja |

## CPedIntelligencePool (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00605FB0 | 11 | C |  | 1 | 0 |  | baja |
| 0x00606000 | 11 | C |  | 1 | 0 |  | baja |
| 0x00606010 | 49 | A |  | 1 | 1 |  | baja |
| 0x00606050 | 11 | C |  | 1 | 0 |  | baja |
| 0x00606580 | 776 | A |  | 1 | 1 |  | baja |
| 0x00606BA0 | 694 | A |  | 1 | 0 |  | baja |

## CEventDamage (22)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004ADE90 | 7 | A |  | 7 | 3 |  | baja |
| 0x004AE2D0 | 69 | D |  | 0 | 0 |  | baja |
| 0x004B5E00 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B5E50 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B5EA0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B5EF0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5F40 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5F90 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B5FE0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6030 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B60D0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6120 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6170 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B61C0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6210 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6260 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B62B0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6300 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B6350 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B63A0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x004B63F0 | 76 | D |  | 0 | 0 |  | baja |
| 0x004B6440 | 76 | B |  | 0 | 0 | sí | baja |

## CTaskComplexWanderGang (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066F650 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066F670 | 11 | C |  | 1 | 0 |  | baja |
| 0x0066F680 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066F6A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x0066F6C0 | 1541 | C |  | 1 | 0 |  | baja |
| 0x0066FD30 | 108 | C |  | 1 | 0 |  | baja |

## CPedAttractor (28)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005E9680 | 5 | B |  | 0 | 0 | sí | baja |
| 0x005EAFE0 | 49 | C |  | 10 | 0 |  | alta |
| 0x005EB140 | 40 | A |  | 1 | 0 |  | alta |
| 0x005EB170 | 50 | A |  | 1 | 0 |  | alta |
| 0x005EB1B0 | 40 | A |  | 1 | 0 |  | alta |
| 0x005EB1E0 | 50 | A |  | 1 | 0 |  | alta |
| 0x005EB220 | 50 | A |  | 1 | 0 |  | alta |
| 0x005EB260 | 40 | A |  | 1 | 0 |  | alta |
| 0x005EB2A0 | 35 | A |  | 3 | 0 |  | alta |
| 0x005EB2E0 | 46 | A |  | 3 | 0 |  | alta |
| 0x005EB310 | 35 | A |  | 4 | 1 |  | alta |
| 0x005EB340 | 55 | A |  | 3 | 0 |  | alta |
| 0x005EB390 | 35 | A |  | 3 | 0 |  | alta |
| 0x005EB5F0 | 67 | A |  | 10 | 10 |  | baja |
| 0x005EB640 | 75 | A |  | 10 | 10 |  | baja |
| 0x005EB690 | 93 | A |  | 10 | 10 |  | baja |
| 0x005EB6F0 | 68 | A |  | 2 | 2 |  | baja |
| 0x005EB740 | 97 | A |  | 10 | 10 |  | baja |
| 0x005EB7B0 | 95 | A |  | 10 | 10 |  | baja |
| 0x005EB930 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB960 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB990 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EB9D0 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EBA00 | 40 | A |  | 2 | 0 |  | baja |
| 0x005EC4B0 | 76 | B |  | 0 | 0 | sí | alta |
| 0x005EC660 | 217 | A |  | 10 | 10 |  | baja |
| 0x005EC740 | 217 | A |  | 10 | 10 |  | baja |
| 0x005EC820 | 39 | D |  | 0 | 0 |  | baja |

## CTaskSimpleThrowControl (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0061FC20 | 26 | D |  | 0 | 0 |  | baja |
| 0x0061FC40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0061FC50 | 103 | C |  | 1 | 0 |  | baja |
| 0x0061FCC0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x0061FD20 | 219 | C |  | 1 | 0 |  | baja |
| 0x0061FE00 | 34 | B |  | 0 | 0 | sí | baja |
| 0x0061FE30 | 42 | D |  | 0 | 0 |  | baja |
| 0x0061FE60 | 6 | B |  | 0 | 0 | sí | baja |
| 0x0061FE70 | 103 | C |  | 1 | 0 |  | baja |
| 0x0061FEE0 | 76 | B |  | 0 | 0 | sí | baja |
| 0x0061FF40 | 836 | C |  | 1 | 0 |  | baja |
| 0x00620290 | 34 | B |  | 0 | 0 | sí | baja |
| 0x00623120 | 111 | B |  | 0 | 0 | sí | baja |
| 0x00623190 | 130 | B |  | 0 | 0 | sí | baja |

## CEventVehicleHitAndRun (25)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004AEA00 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AEA10 | 3 | B |  | 0 | 0 | sí | baja |
| 0x004AEA20 | 12 | B |  | 0 | 0 | sí | baja |
| 0x004AEA30 | 7 | B |  | 0 | 0 | sí | baja |
| 0x004AEA40 | 6 | B |  | 0 | 0 | sí | baja |
| 0x004AEAC0 | 124 | A |  | 3 | 2 |  | baja |
| 0x004AEB90 | 97 | A |  | 5 | 2 |  | baja |
| 0x004AEC00 | 65 | D |  | 0 | 0 |  | baja |
| 0x004AEC50 | 17 | A |  | 1 | 1 |  | baja |
| 0x004AECA0 | 7 | A |  | 3 | 1 |  | baja |
| 0x004AECD0 | 93 | A |  | 3 | 2 |  | baja |
| 0x004AED60 | 81 | A |  | 5 | 2 |  | baja |
| 0x004AEDC0 | 35 | D |  | 0 | 0 |  | baja |
| 0x004AEDF0 | 102 | A |  | 3 | 2 |  | baja |
| 0x004AEEA0 | 81 | A |  | 5 | 2 |  | baja |
| 0x004AEF00 | 272 | D |  | 0 | 0 |  | baja |
| 0x004AF010 | 86 | A |  | 2 | 1 |  | baja |
| 0x004AF0A0 | 81 | A |  | 3 | 1 |  | baja |
| 0x004AF100 | 35 | D |  | 0 | 0 |  | baja |
| 0x004B7170 | 105 | D |  | 0 | 0 |  | baja |
| 0x004B7210 | 103 | D |  | 0 | 0 |  | baja |
| 0x004B7280 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B72F0 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B7360 | 97 | D |  | 0 | 0 |  | baja |
| 0x004B73D0 | 97 | D |  | 0 | 0 |  | baja |

## CTaskComplexUseClosestFreeScriptedAttractor (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006348A0 | 47 | C |  | 1 | 0 |  | baja |
| 0x006348D0 | 6 | B |  | 0 | 0 | sí | baja |
| 0x006348E0 | 11 | C |  | 1 | 0 |  | baja |
| 0x006348F0 | 62 | C |  | 2 | 0 |  | baja |
| 0x00634930 | 262 | B |  | 0 | 0 | sí | baja |
| 0x00634A60 | 425 | B |  | 0 | 0 | sí | baja |
| 0x00634C10 | 330 | B |  | 0 | 0 | sí | baja |
| 0x00634D60 | 387 | B |  | 0 | 0 | sí | baja |
| 0x00634EF0 | 18 | A |  | 2 | 1 |  | baja |
| 0x00636FD0 | 130 | B |  | 0 | 0 | sí | baja |
| 0x00639660 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00639680 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006396A0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006396C0 | 108 | C |  | 1 | 0 |  | baja |

## CCarCtrl (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00421A50 | 1418 | A |  | 2 | 2 |  | alta |
| 0x00422E10 | 353 | A |  | 1 | 0 |  | alta |
| 0x00422F80 | 117 | A |  | 1 | 1 |  | alta |
| 0x00424160 | 40 | C |  | 16 | 0 |  | baja |

## CCustomRoadsignMgr (1)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006FE260 | 1939 | A |  | 1 | 1 |  | alta |

## CPathFind (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0044D0E0 | 1 | A |  | 1 | 1 |  | alta |
| 0x0044D2B0 | 1 | A |  | 1 | 1 |  | alta |
| 0x0044D2C0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044D2D0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044D2E0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044D2F0 | 3 | A |  | 1 | 1 |  | alta |
| 0x0044E480 | 62 | A |  | 12 | 10 |  | alta |
| 0x0044FA30 | 285 | A |  | 1 | 0 |  | alta |
| 0x0044FB60 | 374 | D |  | 0 | 0 |  | alta |
| 0x0044FEE0 | 1077 | C |  | 1 | 0 |  | alta |
| 0x00452090 | 190 | A |  | 2 | 2 |  | alta |
| 0x00453F40 | 81 | D |  | 0 | 0 |  | baja |
| 0x004541E0 | 30 | C |  | 1 | 0 |  | baja |
| 0x00454920 | 28 | A |  | 2 | 2 |  | baja |

## SurfaceInfos_c (2)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0055EB90 | 1814 | A |  | 1 | 1 |  | alta |
| 0x0055F2B0 | 360 | A |  | 1 | 1 |  | alta |

## CCullZones (13)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0072D850 | 273 | A |  | 4 | 4 |  | alta |
| 0x0072E080 | 180 | C |  | 1 | 0 |  | baja |
| 0x0072E140 | 217 | A |  | 1 | 1 |  | baja |
| 0x0072E220 | 79 | A |  | 1 | 1 |  | baja |
| 0x0072E270 | 236 | A |  | 2 | 1 |  | baja |
| 0x0072E360 | 60 | A |  | 6 | 3 |  | baja |
| 0x0072E3A0 | 455 | D |  | 0 | 0 |  | baja |
| 0x0072E870 | 96 | A |  | 1 | 0 |  | baja |
| 0x0072EC60 | 11 | D |  | 0 | 0 |  | baja |
| 0x0072EC80 | 238 | A |  | 1 | 0 |  | baja |
| 0x0072ED80 | 212 | A |  | 1 | 0 |  | baja |
| 0x0072F180 | 418 | A |  | 1 | 1 |  | baja |
| 0x0072F3B0 | 1 | A |  | 1 | 1 |  | baja |

## CAEUserRadioTrackManager (30)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004F3940 | 37 | A |  | 3 | 0 |  | alta |
| 0x004F39C0 | 85 | A |  | 3 | 0 |  | alta |
| 0x004F3A15 | 32 | D |  | 0 | 0 |  | alta |
| 0x004F3A3E | 102 | A |  | 1 | 0 |  | alta |
| 0x004F3AAD | 45 | D |  | 0 | 0 |  | alta |
| 0x004F3AE0 | 30 | A |  | 1 | 0 |  | alta |
| 0x004F3B00 | 40 | A |  | 1 | 0 |  | alta |
| 0x004F3B30 | 35 | A |  | 3 | 1 |  | alta |
| 0x004F3BA0 | 35 | C |  | 8 | 0 |  | alta |
| 0x004F3BD0 | 190 | A |  | 2 | 0 |  | alta |
| 0x004F3CE0 | 40 | A |  | 2 | 0 |  | alta |
| 0x004F3D10 | 47 | C |  | 2 | 0 |  | alta |
| 0x004F3D50 | 14 | D |  | 0 | 0 |  | alta |
| 0x004F3D60 | 27 | D |  | 0 | 0 |  | alta |
| 0x004F3D80 | 39 | A |  | 1 | 0 |  | alta |
| 0x004F3DB0 | 181 | A |  | 2 | 0 |  | alta |
| 0x004F3E80 | 166 | A |  | 3 | 0 |  | alta |
| 0x004F3F30 | 39 | D |  | 0 | 0 |  | alta |
| 0x004F3F60 | 159 | A |  | 7 | 0 |  | alta |
| 0x004F4030 | 39 | C |  | 1 | 0 |  | alta |
| 0x004F4090 | 95 | A |  | 8 | 0 |  | alta |
| 0x004F4110 | 27 | D |  | 0 | 0 |  | alta |
| 0x004F4130 | 47 | C |  | 1 | 0 |  | alta |
| 0x004F4170 | 57 | A |  | 2 | 0 |  | alta |
| 0x004F41B0 | 122 | A |  | 1 | 0 |  | alta |
| 0x004F4230 | 101 | D |  | 0 | 0 |  | alta |
| 0x004F4320 | 645 | A |  | 1 | 1 |  | alta |
| 0x004F44B3 | 9 | D |  | 0 | 0 |  | alta |
| 0x004F4CA0 | 12 | A |  | 1 | 1 |  | baja |
| 0x004F4CE0 | 36 | A |  | 2 | 2 |  | baja |

## CTaskComplexTurnToFaceEntityOrCoord (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066BA70 | 30 | D |  | 0 | 0 |  | baja |
| 0x0066BA90 | 104 | B |  | 0 | 0 | sí | baja |
| 0x0066BB00 | 23 | B |  | 0 | 0 | sí | baja |
| 0x0066D300 | 204 | D |  | 0 | 0 |  | baja |
| 0x006709F0 | 110 | C |  | 1 | 0 |  | baja |
| 0x00670A60 | 6 | B |  | 0 | 0 | sí | baja |
| 0x00670A70 | 121 | C |  | 2 | 0 |  | baja |
| 0x00670B00 | 631 | C |  | 6 | 0 |  | baja |
| 0x00670D80 | 146 | B |  | 0 | 0 | sí | baja |
| 0x00670E20 | 968 | B |  | 0 | 0 | sí | baja |
| 0x006711F0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x006712C0 | 30 | D |  | 0 | 0 |  | baja |
| 0x006712E0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00671330 | 174 | D |  | 0 | 0 |  | baja |

## CTaskComplexGoToPointAnyMeans (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0066B780 | 6 | B |  | 0 | 0 | sí | alta |
| 0x00670830 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00672BB0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00673070 | 878 | C |  | 8 | 0 |  | baja |
| 0x00673540 | 1628 | D |  | 0 | 0 |  | baja |
| 0x00673BA0 | 61 | C |  | 2 | 0 |  | baja |

## CCustomCarEnvMapPipeline (25)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005D84C0 | 207 | A |  | 1 | 1 |  | alta |
| 0x005D8590 | 723 | A |  | 1 | 1 |  | alta |
| 0x005D8980 | 77 | A |  | 1 | 1 |  | alta |
| 0x005D8BE0 | 108 | A |  | 1 | 1 |  | alta |
| 0x005D8C50 | 118 | A |  | 1 | 1 |  | alta |
| 0x005D8F70 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D8FC0 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9010 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9060 | 92 | A |  | 1 | 1 |  | baja |
| 0x005D90C0 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9110 | 104 | A |  | 4 | 4 |  | baja |
| 0x005D9180 | 48 | A |  | 1 | 1 |  | baja |
| 0x005D91B0 | 92 | A |  | 1 | 1 |  | baja |
| 0x005D9210 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D9260 | 104 | A |  | 3 | 3 |  | baja |
| 0x005D9300 | 93 | A |  | 1 | 1 |  | baja |
| 0x005D9360 | 54 | D |  | 0 | 0 |  | baja |
| 0x005D93B0 | 101 | A |  | 2 | 2 |  | baja |
| 0x005D9450 | 71 | D |  | 0 | 0 |  | baja |
| 0x005D94B0 | 71 | D |  | 0 | 0 |  | baja |
| 0x005D9510 | 71 | D |  | 0 | 0 |  | baja |
| 0x005DA640 | 82 | A |  | 3 | 2 |  | baja |
| 0x005DA6A0 | 236 | A |  | 3 | 2 |  | baja |
| 0x005DA790 | 804 | A |  | 1 | 1 |  | baja |
| 0x005DAAC0 | 21 | A |  | 1 | 1 |  | baja |

## CPools (52)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00550180 | 93 | A |  | 1 | 1 |  | alta |
| 0x005501E0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550250 | 92 | A |  | 1 | 1 |  | alta |
| 0x005502B0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550320 | 92 | A |  | 1 | 1 |  | alta |
| 0x00550380 | 54 | D |  | 0 | 0 |  | alta |
| 0x005503F0 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550450 | 54 | D |  | 0 | 0 |  | alta |
| 0x005504C0 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550520 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550570 | 91 | A |  | 1 | 1 |  | alta |
| 0x005505D0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550640 | 94 | A |  | 1 | 1 |  | alta |
| 0x005506A0 | 54 | D |  | 0 | 0 |  | alta |
| 0x005506F0 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550750 | 54 | D |  | 0 | 0 |  | alta |
| 0x005507C0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00550820 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550890 | 91 | A |  | 1 | 1 |  | alta |
| 0x005508F0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550960 | 91 | A |  | 1 | 1 |  | alta |
| 0x005509C0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550A30 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550A90 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550B00 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550B60 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550BD0 | 92 | A |  | 1 | 1 |  | alta |
| 0x00550C30 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550CA0 | 91 | A |  | 1 | 1 |  | alta |
| 0x00550D00 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550D70 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550DD0 | 54 | D |  | 0 | 0 |  | alta |
| 0x00550E40 | 94 | A |  | 1 | 1 |  | alta |
| 0x00550EA0 | 54 | D |  | 0 | 0 |  | alta |
| 0x005512F0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551350 | 71 | D |  | 0 | 0 |  | alta |
| 0x005513B0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551410 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551470 | 71 | D |  | 0 | 0 |  | alta |
| 0x005514D0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551530 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551590 | 71 | D |  | 0 | 0 |  | alta |
| 0x005515F0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551650 | 71 | D |  | 0 | 0 |  | alta |
| 0x005516B0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551710 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551770 | 71 | D |  | 0 | 0 |  | alta |
| 0x005517D0 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551830 | 71 | D |  | 0 | 0 |  | alta |
| 0x00551890 | 71 | D |  | 0 | 0 |  | alta |
| 0x005518F0 | 71 | D |  | 0 | 0 |  | alta |
| 0x005521D0 | 37 | A |  | 1 | 1 |  | baja |

## CAutomobile (11)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006A2B10 | 1005 | B |  | 0 | 0 | sí | alta |
| 0x006A2F80 | 167 | D |  | 0 | 0 |  | alta |
| 0x006A3030 | 35 | B |  | 0 | 0 | sí | alta |
| 0x006A3060 | 584 | B |  | 0 | 0 | sí | alta |
| 0x006A32B0 | 384 | B |  | 0 | 0 | sí | alta |
| 0x006A3BD0 | 646 | A |  | 3 | 3 |  | alta |
| 0x006A6280 | 3 | B |  | 0 | 0 | sí | alta |
| 0x006A6290 | 1 | B |  | 0 | 0 | sí | alta |
| 0x006A6590 | 49 | B |  | 0 | 0 | sí | alta |
| 0x006B0690 | 1018 | A |  | 2 | 2 |  | alta |
| 0x006B1330 | 30 | B |  | 0 | 0 | sí | alta |

## CHeli (6)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x006C4340 | 169 | C |  | 1 | 0 |  | alta |
| 0x006C4550 | 8 | B |  | 0 | 0 | sí | alta |
| 0x006C4810 | 30 | B |  | 0 | 0 | sí | alta |
| 0x006C4830 | 1571 | B |  | 0 | 0 | sí | alta |
| 0x006C7050 | 2352 | B |  | 0 | 0 | sí | alta |
| 0x006C7D10 | 11 | C |  | 1 | 0 |  | baja |

## CReplay (10)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0045B730 | 857 | A |  | 6 | 3 |  | alta |
| 0x0045BA90 | 1236 | A |  | 6 | 3 |  | alta |
| 0x0045BF90 | 81 | A |  | 1 | 1 |  | alta |
| 0x0045BFF0 | 116 | A |  | 1 | 1 |  | alta |
| 0x0045C070 | 81 | A |  | 1 | 1 |  | alta |
| 0x0045C0D0 | 116 | A |  | 1 | 1 |  | alta |
| 0x0045C4B0 | 537 | A |  | 1 | 0 |  | alta |
| 0x0045CD10 | 159 | A |  | 1 | 1 |  | alta |
| 0x0045CDB0 | 226 | A |  | 1 | 1 |  | alta |
| 0x0045DEF0 | 763 | A |  | 3 | 3 |  | alta |

## CRunningScript (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046B1A0 | 80 | C |  | 1 | 0 |  | baja |
| 0x0048B570 | 18 | C |  | 1 | 0 |  | baja |
| 0x0048B590 | 5816 | D |  | 0 | 0 |  | baja |
| 0x0048DE60 | 5 | C |  | 1 | 0 |  | baja |

## RpAnimBlend (16)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004D1610 | 101 | A |  | 10 | 4 |  | alta |
| 0x004D1680 | 971 | A |  | 2 | 1 |  | alta |
| 0x004D1A50 | 850 | A |  | 1 | 1 |  | alta |
| 0x004D1DB0 | 843 | A |  | 1 | 1 |  | alta |
| 0x004D2100 | 845 | A |  | 1 | 1 |  | alta |
| 0x004D2450 | 910 | A |  | 1 | 1 |  | alta |
| 0x004D27F0 | 910 | A |  | 1 | 1 |  | alta |
| 0x004D5F20 | 27 | D |  | 0 | 0 |  | alta |
| 0x004D6240 | 47 | D |  | 0 | 0 |  | alta |
| 0x004D6270 | 40 | D |  | 0 | 0 |  | alta |
| 0x004D6310 | 46 | D |  | 0 | 0 |  | alta |
| 0x004D6340 | 39 | D |  | 0 | 0 |  | alta |
| 0x004D6430 | 26 | D |  | 0 | 0 |  | alta |
| 0x004D6450 | 73 | A |  | 1 | 1 |  | alta |
| 0x004D6510 | 256 | A |  | 1 | 1 |  | alta |
| 0x004D66A0 | 118 | A |  | 1 | 1 |  | alta |

## CScriptsForBrains (4)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x0046A900 | 38 | C |  | 2 | 0 |  | alta |
| 0x0046A930 | 131 | C |  | 2 | 0 |  | alta |
| 0x0046A9C0 | 110 | C |  | 2 | 0 |  | alta |
| 0x0046B460 | 6028 | D |  | 0 | 0 |  | alta |

## FxInfoManager_c (116)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A4C20 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4D00 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4DC0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4E90 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A4F50 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5010 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A50E0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A51B0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5270 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5450 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5550 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5650 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A57C0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5980 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5A10 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5B20 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5CB0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5DA0 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A5E20 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A5EA0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A5FA0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A60B0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A61A0 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A6220 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6320 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6440 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6550 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6630 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6760 | 79 | A |  | 1 | 1 |  | alta |
| 0x004A67D0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A68E0 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6A00 | 101 | A |  | 1 | 1 |  | alta |
| 0x004A6A70 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6A90 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6AE0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6B00 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6B50 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6B70 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6BC0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6BE0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6C30 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6C50 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6CA0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6CC0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6D10 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6D30 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6D80 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6DA0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6DF0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6E10 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6E60 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6E80 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6ED0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6EF0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6F40 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6F60 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A6FB0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A6FD0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7020 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7040 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7090 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A70B0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7390 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A73B0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7400 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7420 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7470 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7490 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A74E0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7500 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7550 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7570 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A75C0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A75E0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7630 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7650 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A76A0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A76C0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7710 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7730 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7780 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A77A0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A77F0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7810 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7860 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7880 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A78D0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A78F0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7940 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7960 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A79B0 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A79D0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7A20 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7A40 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A7A90 | 27 | D |  | 0 | 0 |  | alta |
| 0x004A7AB0 | 67 | C |  | 1 | 0 |  | alta |
| 0x004A8440 | 20 | A |  | 9 | 0 |  | baja |
| 0x004A8460 | 7 | C |  | 18 | 0 |  | baja |
| 0x004A87D0 | 20 | A |  | 8 | 0 |  | baja |
| 0x004A87F0 | 7 | C |  | 15 | 0 |  | baja |
| 0x004A8990 | 20 | A |  | 13 | 0 |  | baja |
| 0x004A89B0 | 7 | C |  | 27 | 0 |  | baja |
| 0x004A8B50 | 20 | A |  | 2 | 0 |  | baja |
| 0x004A8B70 | 7 | C |  | 4 | 0 |  | baja |
| 0x004A8D30 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8D50 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8D70 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8D90 | 27 | D |  | 0 | 0 |  | baja |
| 0x004A8DB0 | 16 | A |  | 15 | 3 |  | baja |
| 0x004A8DC0 | 1 | A |  | 18 | 2 |  | baja |
| 0x004A8DD0 | 13 | A |  | 20 | 5 |  | baja |
| 0x004A8DE0 | 1 | A |  | 35 | 5 |  | baja |
| 0x004A8E30 | 59 | A |  | 16 | 14 |  | baja |
| 0x004A8E70 | 54 | A |  | 11 | 11 |  | baja |
| 0x004A9010 | 67 | A |  | 3 | 3 |  | baja |
| 0x004A9090 | 12 | D |  | 0 | 0 |  | baja |

## CPedAttractorManager (71)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x005EC040 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC070 | 39 | A |  | 1 | 0 |  | baja |
| 0x005EC0B0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC0E0 | 42 | A |  | 1 | 0 |  | baja |
| 0x005EC110 | 39 | A |  | 1 | 0 |  | baja |
| 0x005EC140 | 42 | A |  | 1 | 0 |  | baja |
| 0x005EC180 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC1B0 | 39 | A |  | 1 | 0 |  | baja |
| 0x005EC1F0 | 39 | C |  | 4 | 0 |  | baja |
| 0x005EC220 | 49 | A |  | 1 | 1 |  | baja |
| 0x005EC260 | 39 | C |  | 2 | 0 |  | baja |
| 0x005EC2D0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC340 | 39 | C |  | 9 | 0 |  | baja |
| 0x005EC3B0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005EC3E0 | 39 | D |  | 0 | 0 |  | baja |
| 0x005ECAB0 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECB30 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECBB0 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECC30 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECCB0 | 122 | A |  | 1 | 0 |  | baja |
| 0x005ECF10 | 645 | A |  | 1 | 0 |  | baja |
| 0x005ED0A3 | 9 | D |  | 0 | 0 |  | baja |
| 0x005ED1B0 | 739 | A |  | 1 | 0 |  | baja |
| 0x005ED388 | 9 | D |  | 0 | 0 |  | baja |
| 0x005ED4B0 | 645 | A |  | 2 | 1 |  | baja |
| 0x005ED643 | 9 | D |  | 0 | 0 |  | baja |
| 0x005ED750 | 739 | A |  | 1 | 0 |  | baja |
| 0x005ED92A | 9 | D |  | 0 | 0 |  | baja |
| 0x005EDA50 | 645 | A |  | 1 | 0 |  | baja |
| 0x005EDBE3 | 9 | D |  | 0 | 0 |  | baja |
| 0x005EDD90 | 111 | A |  | 1 | 0 |  | baja |
| 0x005EDE50 | 109 | A |  | 1 | 0 |  | baja |
| 0x005EDF10 | 148 | A |  | 1 | 1 |  | baja |
| 0x005EE200 | 102 | A |  | 2 | 2 |  | baja |
| 0x005EE270 | 138 | A |  | 1 | 1 |  | baja |
| 0x005EE300 | 102 | A |  | 1 | 1 |  | baja |
| 0x005EE370 | 136 | A |  | 1 | 1 |  | baja |
| 0x005EE400 | 102 | A |  | 2 | 1 |  | baja |
| 0x005EE470 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE4E0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE530 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE540 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE5B0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE600 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE610 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE680 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE6D0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE6E0 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE760 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE7B0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE7C0 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE820 | 6 | B |  | 0 | 0 | sí | baja |
| 0x005EE830 | 76 | B |  | 0 | 0 | sí | baja |
| 0x005EE880 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE890 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE900 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EE950 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EE960 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EE9D0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEA20 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEA30 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EEAA0 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEAF0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEB00 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EEB70 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEBC0 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EEBD0 | 85 | A |  | 1 | 0 |  | baja |
| 0x005EEC40 | 76 | D |  | 0 | 0 |  | baja |
| 0x005EEC90 | 11 | C |  | 1 | 0 |  | baja |
| 0x005EF920 | 95 | A |  | 10 | 10 |  | baja |
| 0x005EF980 | 659 | A |  | 8 | 8 |  | baja |

## CTaskComplexSeekEntityRadiusAngleOffset (5)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00493870 | 30 | B |  | 0 | 0 | sí | alta |
| 0x00493960 | 136 | C |  | 1 | 0 |  | baja |
| 0x00496DC0 | 61 | A |  | 2 | 2 |  | baja |
| 0x00496E00 | 8462 | D |  | 0 | 0 |  | baja |
| 0x004993D0 | 82 | A |  | 4 | 4 |  | baja |

## CTheScripts (14)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00465040 | 41 | A |  | 98 | 1 |  | baja |
| 0x00465070 | 40 | A |  | 5 | 1 |  | baja |
| 0x004650A0 | 54 | A |  | 1 | 1 |  | baja |
| 0x004654B0 | 226 | A |  | 22 | 4 |  | baja |
| 0x0046A7E0 | 33 | C |  | 1 | 0 |  | alta |
| 0x0046B260 | 12 | C |  | 1 | 0 |  | baja |
| 0x00474870 | 46 | C |  | 1 | 0 |  | baja |
| 0x004748A0 | 46 | C |  | 1 | 0 |  | baja |
| 0x004748D0 | 30 | B |  | 0 | 0 | sí | baja |
| 0x00481300 | 8957 | D |  | 0 | 0 |  | alta |
| 0x00486AE0 | 1 | C |  | 1 | 0 |  | alta |
| 0x00493360 | 185 | C |  | 1 | 0 |  | alta |
| 0x00493420 | 88 | C |  | 2 | 0 |  | alta |
| 0x00493480 | 105 | C |  | 1 | 0 |  | alta |

## CEntity (43)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00401000 | 74 | C |  | 1 | 0 |  | baja |
| 0x0040104A | 321 | C |  | 2 | 0 |  | baja |
| 0x0040118B | 350 | C |  | 1 | 0 |  | baja |
| 0x004012E9 | 3282 | C |  | 3 | 0 |  | baja |
| 0x00401FBB | 949 | C |  | 1 | 0 |  | baja |
| 0x00402370 | 336 | C |  | 1 | 0 |  | baja |
| 0x004024C0 | 581 | C |  | 1 | 0 |  | baja |
| 0x004027CD | 58 | C |  | 1 | 0 |  | baja |
| 0x004028A2 | 404 | C |  | 1 | 0 |  | baja |
| 0x00402A36 | 211 | C |  | 1 | 0 |  | baja |
| 0x00402DC5 | 9 | C |  | 4 | 0 |  | baja |
| 0x004032A4 | 15 | D |  | 0 | 0 |  | baja |
| 0x004032C5 | 18 | C |  | 1 | 0 |  | baja |
| 0x0040335F | 26 | C |  | 1 | 0 |  | baja |
| 0x00403379 | 29 | C |  | 1 | 0 |  | baja |
| 0x004033B2 | 11 | C |  | 18 | 0 |  | baja |
| 0x004033F5 | 525 | C |  | 1 | 0 |  | baja |
| 0x00403638 | 137 | C |  | 2 | 0 |  | baja |
| 0x004036C1 | 139 | C |  | 3 | 0 |  | baja |
| 0x0040374C | 437 | C |  | 1 | 0 |  | baja |
| 0x00403902 | 513 | C |  | 1 | 0 |  | baja |
| 0x00403B03 | 120 | C |  | 1 | 0 |  | baja |
| 0x00403B7B | 2 | C |  | 3 | 0 |  | baja |
| 0x00403B7D | 151 | C |  | 7 | 0 |  | baja |
| 0x00403C14 | 92 | C |  | 1 | 0 |  | baja |
| 0x00403C70 | 24 | C |  | 1 | 0 |  | baja |
| 0x00403C88 | 127 | C |  | 1 | 0 |  | baja |
| 0x00403D07 | 88 | D |  | 0 | 0 |  | baja |
| 0x00403E00 | 32 | A |  | 4 | 3 |  | baja |
| 0x00447000 | 14 | C |  | 7 | 0 |  | baja |
| 0x00447050 | 14 | C |  | 1 | 0 |  | baja |
| 0x00463490 | 21 | C |  | 1 | 0 |  | baja |
| 0x004634D0 | 32 | D |  | 0 | 0 |  | baja |
| 0x0046A2F0 | 18 | C |  | 2 | 0 |  | baja |
| 0x0046A340 | 49 | C |  | 1 | 0 |  | baja |
| 0x00533020 | 35 | A |  | 2 | 2 | sí | alta |
| 0x005335E0 | 47 | A |  | 5 | 3 |  | alta |
| 0x00533610 | 81 | A |  | 3 | 3 |  | alta |
| 0x00533670 | 54 | C |  | 1 | 0 |  | alta |
| 0x005336B0 | 79 | A |  | 2 | 2 |  | alta |
| 0x005347D0 | 763 | B | CEntity::Add-rect | 0 | 0 | sí | alta |
| 0x00536C20 | 27 | B |  | 0 | 0 | sí | baja |
| 0x00542080 | 5 | C |  | 2 | 0 |  | baja |

## CWeaponModelInfo (41)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004C5790 | 7 | C |  | 1 | 0 |  | baja |
| 0x004C57A0 | 25 | D |  | 0 | 0 |  | baja |
| 0x004C9990 | 202 | C |  | 4 | 0 |  | baja |
| 0x004C9AB0 | 967 | D |  | 0 | 0 |  | baja |
| 0x004C9EA0 | 25 | C |  | 4 | 0 |  | baja |
| 0x004C9F70 | 19 | C |  | 1 | 0 |  | baja |
| 0x004C9F90 | 744 | C |  | 1 | 0 |  | baja |
| 0x004CA290 | 559 | C |  | 1 | 0 |  | baja |
| 0x004CA540 | 85 | D |  | 0 | 0 |  | baja |
| 0x004CA5A0 | 1339 | C |  | 1 | 0 |  | baja |
| 0x004CAAE0 | 673 | C |  | 1 | 0 |  | baja |
| 0x004CAD90 | 104 | C |  | 3 | 0 |  | baja |
| 0x004CAEA0 | 1530 | C |  | 2 | 0 |  | baja |
| 0x004CB530 | 47 | C |  | 3 | 0 |  | baja |
| 0x004CB560 | 78 | C |  | 7 | 0 |  | baja |
| 0x004CB5E0 | 82 | C |  | 1 | 0 |  | baja |
| 0x004CB640 | 157 | A |  | 1 | 0 |  | baja |
| 0x004CB730 | 141 | C |  | 3 | 0 |  | baja |
| 0x004CB7C0 | 512 | C |  | 1 | 0 |  | baja |
| 0x004CB9C0 | 310 | C |  | 1 | 0 |  | baja |
| 0x004CBB00 | 402 | C |  | 1 | 0 |  | baja |
| 0x004CBD80 | 87 | A |  | 8 | 0 |  | baja |
| 0x004CBDE0 | 51 | C |  | 35 | 0 |  | baja |
| 0x004CBE20 | 93 | C |  | 8 | 0 |  | baja |
| 0x004CBE80 | 87 | C |  | 2 | 0 |  | baja |
| 0x004CBF20 | 93 | C |  | 1 | 0 |  | baja |
| 0x004CBF80 | 65 | C |  | 3 | 0 |  | baja |
| 0x004CBFD0 | 65 | C |  | 1 | 0 |  | baja |
| 0x004CC020 | 65 | C |  | 2 | 0 |  | baja |
| 0x004CC0C0 | 129 | C |  | 3 | 0 |  | baja |
| 0x004CC150 | 25 | C |  | 1 | 0 |  | baja |
| 0x004CC170 | 82 | C |  | 1 | 0 |  | baja |
| 0x004CC1D0 | 906 | C |  | 1 | 0 |  | baja |
| 0x004CC580 | 64 | C |  | 3 | 0 |  | baja |
| 0x004CC5C0 | 907 | C |  | 1 | 0 |  | baja |
| 0x004CC970 | 990 | envoltura |  | 12 | 0 |  | baja |
| 0x004CCD70 | 236 | C |  | 1 | 0 |  | baja |
| 0x004CCE60 | 457 | C |  | 1 | 0 |  | baja |
| 0x004CD050 | 512 | C |  | 1 | 0 |  | baja |
| 0x004CD250 | 265 | envoltura |  | 2 | 0 |  | baja |
| 0x004CD360 | 367 | C |  | 1 | 0 |  | baja |

## Win (85)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x00746070 | 11 | A |  | 1 | 1 |  | baja |
| 0x007460A0 | 205 | A |  | 1 | 1 |  | baja |
| 0x007469A0 | 109 | A |  | 6 | 4 |  | baja |
| 0x00747DB0 | 255 | D |  | 0 | 0 |  | baja |
| 0x007486A0 | 99 | C |  | 1 | 0 |  | baja |
| 0x00748710 | 1947 | C |  | 1 | 0 |  | baja |
| 0x00748F70 | 134 | A |  | 1 | 1 |  | baja |
| 0x00749120 | 60 | D |  | 0 | 0 |  | baja |
| 0x00749160 | 62 | D |  | 0 | 0 |  | baja |
| 0x007491F0 | 318 | A |  | 12 | 5 |  | baja |
| 0x00749330 | 327 | envoltura |  | 2 | 0 |  | baja |
| 0x00749520 | 372 | D |  | 0 | 0 |  | baja |
| 0x007496A0 | 122 | D |  | 0 | 0 |  | baja |
| 0x00749830 | 75 | A |  | 1 | 0 |  | baja |
| 0x00749B20 | 65 | A |  | 9 | 9 |  | baja |
| 0x00749BB0 | 67 | envoltura |  | 6 | 0 |  | baja |
| 0x00749C00 | 67 | envoltura |  | 5 | 0 |  | baja |
| 0x00749C50 | 201 | A |  | 4 | 1 |  | baja |
| 0x00749D20 | 32 | D |  | 0 | 0 |  | baja |
| 0x00749D40 | 124 | A |  | 5 | 2 |  | baja |
| 0x00749DC0 | 127 | A |  | 26 | 19 |  | baja |
| 0x00749E60 | 258 | A |  | 17 | 7 |  | baja |
| 0x00749F70 | 592 | C |  | 1 | 0 |  | baja |
| 0x0074A1C0 | 52 | C |  | 1 | 0 |  | baja |
| 0x0074A290 | 113 | A |  | 8 | 5 |  | baja |
| 0x0074A310 | 377 | A |  | 21 | 14 |  | baja |
| 0x0074A490 | 37 | A |  | 16 | 11 |  | baja |
| 0x0074A4C0 | 33 | A |  | 6 | 6 |  | baja |
| 0x0074A520 | 44 | envoltura |  | 1 | 0 |  | baja |
| 0x0074AE50 | 70 | A |  | 2 | 0 |  | baja |
| 0x0074B060 | 952 | A |  | 1 | 0 |  | baja |
| 0x0074B420 | 2207 | A |  | 6 | 5 |  | baja |
| 0x0074BCC0 | 124 | D |  | 0 | 0 |  | baja |
| 0x0074BDA0 | 39 | A |  | 7 | 3 |  | baja |
| 0x0074BDD0 | 39 | A |  | 4 | 3 |  | baja |
| 0x0074BE00 | 34 | A |  | 4 | 1 |  | baja |
| 0x0074BE70 | 34 | A |  | 1 | 1 |  | baja |
| 0x0074C200 | 269 | A |  | 2 | 2 |  | baja |
| 0x0074C310 | 443 | envoltura |  | 2 | 0 |  | baja |
| 0x0074C690 | 35 | A |  | 3 | 3 |  | baja |
| 0x0074C6C0 | 82 | A |  | 4 | 4 |  | baja |
| 0x0074C7D0 | 44 | A |  | 13 | 8 |  | baja |
| 0x0074C800 | 595 | A |  | 14 | 11 |  | baja |
| 0x0074CA90 | 543 | A |  | 4 | 3 |  | baja |
| 0x0074CCB0 | 9 | A |  | 7 | 3 |  | baja |
| 0x0074CCC0 | 165 | A |  | 23 | 3 |  | baja |
| 0x0074CD70 | 39 | A |  | 7 | 3 |  | baja |
| 0x0074CDA0 | 34 | A |  | 6 | 3 |  | baja |
| 0x0074D190 | 1332 | envoltura |  | 6 | 0 |  | baja |
| 0x0074D6D0 | 117 | A |  | 1 | 0 |  | baja |
| 0x0074D7D0 | 60 | D |  | 0 | 0 |  | baja |
| 0x0074D810 | 62 | D |  | 0 | 0 |  | baja |
| 0x0074D870 | 65 | envoltura |  | 2 | 0 |  | baja |
| 0x0074D990 | 131 | A |  | 2 | 2 |  | baja |
| 0x0074DA20 | 94 | A |  | 14 | 7 |  | baja |
| 0x0074DBC0 | 39 | A |  | 13 | 11 |  | baja |
| 0x0074DBF0 | 39 | A |  | 5 | 2 |  | baja |
| 0x0074DC20 | 34 | A |  | 5 | 2 |  | baja |
| 0x0074DD30 | 724 | envoltura |  | 1 | 0 |  | baja |
| 0x0074E010 | 52 | envoltura |  | 1 | 0 |  | baja |
| 0x0074E150 | 86 | A |  | 9 | 1 |  | baja |
| 0x0074E350 | 193 | A |  | 3 | 1 |  | baja |
| 0x0074E450 | 91 | envoltura |  | 0 | 0 |  | baja |
| 0x0074E600 | 870 | envoltura |  | 1 | 0 |  | baja |
| 0x0074E970 | 99 | envoltura |  | 2 | 0 |  | baja |
| 0x0074E9E0 | 85 | envoltura |  | 1 | 0 |  | baja |
| 0x0074EA40 | 55 | A |  | 1 | 0 |  | baja |
| 0x0074EAB0 | 6 | C |  | 1 | 0 |  | baja |
| 0x0074EAC0 | 1 | C |  | 2 | 0 |  | baja |
| 0x0074EAD0 | 110 | C |  | 1 | 0 |  | baja |
| 0x0074EB40 | 58 | C |  | 2 | 0 |  | baja |
| 0x0074EB80 | 124 | C |  | 1 | 0 |  | baja |
| 0x0074EC00 | 150 | C |  | 2 | 0 |  | baja |
| 0x0074ECA0 | 165 | envoltura |  | 3 | 0 |  | baja |
| 0x0074ED50 | 364 | envoltura |  | 4 | 0 |  | baja |
| 0x0074EEC0 | 72 | envoltura |  | 0 | 0 |  | baja |
| 0x0074F210 | 710 | envoltura |  | 1 | 0 |  | baja |
| 0x0074F4E0 | 129 | envoltura |  | 6 | 0 |  | baja |
| 0x0074F610 | 288 | A |  | 1 | 1 |  | baja |
| 0x0074F760 | 923 | A |  | 1 | 1 |  | baja |
| 0x0074FBC0 | 62 | envoltura |  | 0 | 0 |  | baja |
| 0x0074FC70 | 89 | envoltura |  | 1 | 0 |  | baja |
| 0x0074FDA0 | 237 | A |  | 1 | 1 |  | baja |
| 0x0074FFA0 | 68 | A |  | 2 | 0 |  | baja |
| 0x0074FFF0 | 81 | A |  | 2 | 0 |  | baja |

## FxEmitterBP_c (141)

| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |
|---|---:|---|---|---:|---:|---|---|
| 0x004A18F0 | 11 | C |  | 1 | 0 |  | alta |
| 0x004A1900 | 18 | A |  | 1 | 1 |  | alta |
| 0x004A1920 | 18 | D |  | 0 | 0 |  | alta |
| 0x004A24D0 | 50 | A |  | 2 | 2 |  | alta |
| 0x004A2550 | 26 | B |  | 0 | 0 | sí | alta |
| 0x004A2570 | 8 | B |  | 0 | 0 | sí | alta |
| 0x004A2B20 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004A2B90 | 27 | B |  | 0 | 0 | sí | alta |
| 0x004A2BB0 | 5 | C |  | 1 | 0 |  | alta |
| 0x004A4460 | 24 | B |  | 0 | 0 | sí | baja |
| 0x004A4940 | 7 | C |  | 64 | 0 |  | baja |
| 0x004A4950 | 1 | C |  | 1 | 0 |  | baja |
| 0x005C26B0 | 117 | A |  | 1 | 1 |  | baja |
| 0x005C2730 | 194 | A |  | 1 | 1 |  | baja |
| 0x005C30D0 | 259 | A |  | 1 | 0 |  | baja |
| 0x005C35F0 | 97 | D |  | 0 | 0 |  | baja |
| 0x005C3700 | 531 | A |  | 1 | 0 |  | baja |
| 0x005C4380 | 108 | A |  | 1 | 0 |  | baja |
| 0x005C43F0 | 520 | C |  | 2 | 0 |  | baja |
| 0x005C4640 | 226 | C |  | 8 | 0 |  | baja |
| 0x005C4760 | 387 | C |  | 1 | 0 |  | baja |
| 0x005C48F0 | 200 | C |  | 1 | 0 |  | baja |
| 0x005C4B90 | 224 | C |  | 1 | 0 |  | baja |
| 0x005C4D50 | 571 | C |  | 2 | 0 |  | baja |
| 0x005C5210 | 66 | A |  | 1 | 0 |  | baja |
| 0x005C5260 | 188 | A |  | 1 | 0 |  | baja |
| 0x005C5420 | 134 | A |  | 1 | 0 |  | baja |
| 0x005C54F0 | 104 | C |  | 12 | 0 |  | baja |
| 0x005C5560 | 102 | C |  | 18 | 0 |  | baja |
| 0x005C55D0 | 419 | C |  | 2 | 0 |  | baja |
| 0x005C5780 | 373 | C |  | 6 | 0 |  | baja |
| 0x005C5910 | 41 | D |  | 0 | 0 |  | baja |
| 0x005C5940 | 370 | C |  | 3 | 0 |  | baja |
| 0x005C5AC0 | 468 | C |  | 1 | 0 |  | baja |
| 0x005C5CA0 | 536 | C |  | 1 | 0 |  | baja |
| 0x005C5EC0 | 362 | C |  | 1 | 0 |  | baja |
| 0x005C60B0 | 70 | D |  | 0 | 0 |  | baja |
| 0x005C6100 | 224 | D |  | 0 | 0 |  | baja |
| 0x005C62D0 | 20 | D |  | 0 | 0 |  | baja |
| 0x005C63D0 | 81 | A |  | 1 | 0 |  | baja |
| 0x005C6430 | 460 | A |  | 1 | 0 |  | baja |
| 0x005C6600 | 127 | C |  | 3 | 0 |  | baja |
| 0x005C6680 | 453 | C |  | 3 | 0 |  | baja |
| 0x005C6850 | 396 | D |  | 0 | 0 |  | baja |
| 0x005C69E0 | 40 | D |  | 0 | 0 |  | baja |
| 0x005C6A90 | 186 | A |  | 1 | 0 |  | baja |
| 0x005C6B50 | 56 | A |  | 3 | 0 |  | baja |
| 0x005C6B90 | 31 | C |  | 1 | 0 |  | baja |
| 0x005C6BB0 | 28 | A |  | 2 | 0 |  | baja |
| 0x005C6BD0 | 28 | A |  | 4 | 0 |  | baja |
| 0x005C6BF0 | 801 | A |  | 2 | 0 |  | baja |
| 0x005C7010 | 191 | A |  | 4 | 0 |  | baja |
| 0x005C70D0 | 82 | A |  | 1 | 0 |  | baja |
| 0x005C7130 | 714 | A |  | 3 | 0 |  | baja |
| 0x005C7420 | 336 | A |  | 1 | 0 |  | baja |
| 0x005C7590 | 264 | A |  | 1 | 1 |  | baja |
| 0x005C7920 | 174 | A |  | 1 | 0 |  | baja |
| 0x005C79F0 | 88 | C |  | 4 | 0 |  | baja |
| 0x005C7DF0 | 259 | A |  | 1 | 0 |  | baja |
| 0x005C8620 | 100 | A |  | 2 | 0 |  | baja |
| 0x005C8690 | 149 | A |  | 1 | 1 |  | baja |
| 0x005C87E0 | 143 | A |  | 1 | 1 |  | baja |
| 0x005C88F0 | 105 | D |  | 0 | 0 |  | baja |
| 0x005C8E20 | 192 | A |  | 1 | 0 |  | baja |
| 0x005C8EE0 | 195 | A |  | 2 | 0 |  | baja |
| 0x005C90D0 | 120 | D |  | 0 | 0 |  | baja |
| 0x005C9350 | 416 | A |  | 1 | 0 |  | baja |
| 0x005C9710 | 127 | A |  | 1 | 0 |  | baja |
| 0x005C9790 | 704 | C |  | 2 | 0 |  | baja |
| 0x005C9A60 | 286 | C |  | 8 | 0 |  | baja |
| 0x005C9B80 | 223 | C |  | 3 | 0 |  | baja |
| 0x005C9C60 | 127 | C |  | 1 | 0 |  | baja |
| 0x005CA260 | 63 | A |  | 1 | 0 |  | baja |
| 0x005CA2A0 | 493 | C |  | 1 | 0 |  | baja |
| 0x005CA490 | 415 | C |  | 1 | 0 |  | baja |
| 0x005CA630 | 170 | C |  | 1 | 0 |  | baja |
| 0x005CA6E0 | 59 | C |  | 1 | 0 |  | baja |
| 0x005CA720 | 180 | D |  | 0 | 0 |  | baja |
| 0x005CA7E0 | 59 | D |  | 0 | 0 |  | baja |
| 0x005CA840 | 65 | A |  | 1 | 0 |  | baja |
| 0x005CA890 | 168 | A |  | 1 | 0 |  | baja |
| 0x005CA940 | 332 | C |  | 1 | 0 |  | baja |
| 0x005CAA90 | 210 | C |  | 1 | 0 |  | baja |
| 0x005CAB70 | 157 | C |  | 1 | 0 |  | baja |
| 0x005CAE30 | 207 | A |  | 1 | 0 |  | baja |
| 0x005CAF00 | 164 | C |  | 1 | 0 |  | baja |
| 0x005CAFB0 | 777 | C |  | 4 | 0 |  | baja |
| 0x005CB2C0 | 751 | C |  | 1 | 0 |  | baja |
| 0x005CB5B0 | 781 | C |  | 1 | 0 |  | baja |
| 0x005CB8C0 | 599 | C |  | 1 | 0 |  | baja |
| 0x005CBB20 | 223 | C |  | 1 | 0 |  | baja |
| 0x005CBC00 | 587 | C |  | 1 | 0 |  | baja |
| 0x005CBE50 | 169 | C |  | 1 | 0 |  | baja |
| 0x005CC050 | 157 | C |  | 1 | 0 |  | baja |
| 0x005CC0F0 | 256 | C |  | 3 | 0 |  | baja |
| 0x005CC1F0 | 153 | C |  | 1 | 0 |  | baja |
| 0x005CC610 | 136 | D |  | 0 | 0 |  | baja |
| 0x005CC7D0 | 132 | A |  | 1 | 0 |  | baja |
| 0x005CC8C0 | 121 | A |  | 1 | 0 |  | baja |
| 0x005CC950 | 119 | A |  | 1 | 0 |  | baja |
| 0x005CC9D0 | 373 | A |  | 1 | 0 |  | baja |
| 0x005CCB50 | 318 | D |  | 0 | 0 |  | baja |
| 0x005CCCC0 | 48 | A |  | 1 | 0 |  | baja |
| 0x005CCDF0 | 110 | A |  | 1 | 0 |  | baja |
| 0x005CCE80 | 222 | D |  | 0 | 0 |  | baja |
| 0x005CD350 | 477 | A |  | 1 | 0 |  | baja |
| 0x005CD6B0 | 80 | D |  | 0 | 0 |  | baja |
| 0x005CD810 | 76 | A |  | 2 | 2 |  | baja |
| 0x005CEBC0 | 331 | C |  | 4 | 0 |  | baja |
| 0x005CED10 | 188 | C |  | 2 | 0 |  | baja |
| 0x005CEDD0 | 167 | C |  | 1 | 0 |  | baja |
| 0x005CEE80 | 167 | C |  | 1 | 0 |  | baja |
| 0x005CEF30 | 104 | D |  | 0 | 0 |  | baja |
| 0x005CEFA0 | 104 | D |  | 0 | 0 |  | baja |
| 0x005CF010 | 403 | D |  | 0 | 0 |  | baja |
| 0x005CF1B0 | 148 | C |  | 2 | 0 |  | baja |
| 0x005CF250 | 151 | C |  | 2 | 0 |  | baja |
| 0x005CF580 | 263 | C |  | 1 | 0 |  | baja |
| 0x005CF6D0 | 297 | A |  | 2 | 0 |  | baja |
| 0x005CF800 | 16 | A |  | 3 | 0 |  | baja |
| 0x005CF810 | 10 | C |  | 2 | 0 |  | baja |
| 0x005CF820 | 16 | C |  | 1 | 0 |  | baja |
| 0x005CF830 | 10 | C |  | 1 | 0 |  | baja |
| 0x005CF840 | 5 | C |  | 1 | 0 |  | baja |
| 0x005CF850 | 20 | C |  | 2 | 0 |  | baja |
| 0x005CF870 | 3 | A |  | 1 | 0 |  | baja |
| 0x005CF880 | 1 | A |  | 2 | 0 |  | baja |
| 0x005CF890 | 16 | A |  | 14 | 0 |  | baja |
| 0x005CF8A0 | 24 | A |  | 3 | 0 |  | baja |
| 0x005CF8C0 | 70 | C |  | 6 | 0 |  | baja |
| 0x005CF940 | 27 | C |  | 5 | 0 |  | baja |
| 0x005CF960 | 198 | A |  | 1 | 1 |  | baja |
| 0x005CFA30 | 5 | A |  | 1 | 1 |  | baja |
| 0x005CFA50 | 102 | A |  | 1 | 0 |  | baja |
| 0x005CFAC0 | 260 | A |  | 1 | 1 |  | baja |
| 0x005CFD20 | 218 | A |  | 1 | 1 |  | baja |
| 0x005CFE00 | 5 | A |  | 3 | 3 |  | baja |
| 0x005CFE20 | 394 | A |  | 1 | 0 |  | baja |
| 0x005CFFB0 | 151 | A |  | 1 | 0 |  | baja |
| 0x005D0110 | 187 | A |  | 1 | 1 |  | baja |
| 0x005D01D0 | 112 | A |  | 1 | 1 |  | baja |
