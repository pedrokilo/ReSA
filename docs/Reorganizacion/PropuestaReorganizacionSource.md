<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Propuesta teórica de reorganización de `source/game_sa`

**Solo propuesta: no se movió ningún archivo.** Fecha: 9 de octubre de 2026. Se generó con `reversing/_handoff/reorg_plan.py`; el mapa archivo por archivo está en [reorg-map.csv](reorg-map.csv).

## Criterio

- Los nombres de carpeta de `game/` siguen los de `E:/Original/gta5/src/dev_ng/game` (por ejemplo `task/Combat`, `Peds`, `Vehicles`, `scene`, `renderer`, `vfx`, `weapons`), igual que se hizo en ReIV. No se copia código, solo la organización.
- El motor de `dev_ng/rage` se reemplaza por dos capas propias de SA: `base/` (contenedores, matemáticas, memoria: equivalente a `rage/base`) y `renderware/` (RenderWare, plugins, pipelines).
- Los enums y los pools dejan de estar en carpetas globales (`Enums/`, `Pools/`) y van junto a su dueño, como en `dev_ng`.
- Los 525 archivos sueltos de la raíz desaparecen: todos tienen carpeta.
- Total: **2163 archivos en 118 carpetas nuevas**.

## Resumen por carpeta de primer nivel

| Carpeta | Archivos | Carpetas nuevas |
| --- | ---: | ---: |
| `base` | 68 | 4 |
| `game/Cutscene` | 4 | 1 |
| `game/Objects` | 30 | 2 |
| `game/PedGroup` | 42 | 3 |
| `game/Peds` | 75 | 6 |
| `game/SaveLoad` | 14 | 1 |
| `game/Stats` | 6 | 1 |
| `game/Vehicles` | 84 | 2 |
| `game/ai` | 35 | 4 |
| `game/animation` | 32 | 1 |
| `game/audio` | 124 | 13 |
| `game/camera` | 9 | 5 |
| `game/control` | 12 | 1 |
| `game/event` | 178 | 1 |
| `game/frontend` | 39 | 3 |
| `game/ik` | 8 | 1 |
| `game/modelinfo` | 31 | 1 |
| `game/pathserver` | 12 | 1 |
| `game/physics` | 59 | 3 |
| `game/pickups` | 5 | 1 |
| `game/renderer` | 62 | 7 |
| `game/replaycoordinator` | 5 | 1 |
| `game/scene` | 80 | 7 |
| `game/script` | 72 | 4 |
| `game/streaming` | 15 | 1 |
| `game/system` | 28 | 1 |
| `game/task` | 700 | 20 |
| `game/text` | 11 | 1 |
| `game/timecycle` | 12 | 1 |
| `game/vehicleAi` | 17 | 1 |
| `game/vfx` | 150 | 6 |
| `game/weapons` | 26 | 1 |
| `renderware` | 118 | 12 |

## Contenido completo

Cada línea es una carpeta nueva con las clases (archivos sin extensión) que irían dentro.


### `base`

- **`base/`** (5 archivos): General, common, constants
- **`base/containers/`** (28 archivos): KeyGen, Link, LinkList, ListItem_c, List_c, MatrixLink, MatrixLinkList, OctTree, OctTreeBase, Pool, PtrList, PtrListDoubleLink, PtrListSingleLink, PtrNode, PtrNodeDoubleLink, PtrNodeSingleLink, QuadTreeNode, Queue, SArray, Store
- **`base/maths/`** (27 archivos): CompressedBox, CompressedMatrixNotAligned, CompressedVector, Curves, Maths, Matrix, Quaternion, RGBA, Range2D, Range3D, Rect, SimpleTransform, Vector, Vector2D, eDirection
- **`base/memory/`** (8 archivos): CommonSize, HeapBlockDesc, HeapFreeBlockDesc, MemoryHeap

### `game/Cutscene`

- **`game/Cutscene/`** (4 archivos): CutsceneMgr, CutsceneObject

### `game/Objects`

- **`game/Objects/`** (24 archivos): BouncingPanel, BreakManager_c, BreakObject_c, Cranes, Door, DummyPool, FallingGlassPane, Glass, HandObject, MovingThings, Object, ObjectData, ObjectPool
- **`game/Objects/Dummy/`** (6 archivos): Dummy, DummyObject, DummyPed

### `game/PedGroup`

- **`game/PedGroup/`** (14 archivos): Formation, GroupEventHandler, PedGroup, PedGroupIntelligence, PedGroupMembership, PedGroupPlacer, PedGroups
- **`game/PedGroup/Allocators/`** (19 archivos): PedGroupDefaultTaskAllocators, TaskAllocator, TaskAllocatorAttack, TaskAllocatorKillOnFoot, TaskAllocatorKillThreatsBasic, TaskAllocatorKillThreatsBasicRandomGroup, TaskAllocatorKillThreatsDriveby, TaskAllocatorPlayerCommandAttack, TaskAllocatorPlayerCommandRetreat, TaskAllocatorRetreat
- **`game/PedGroup/Allocators/PedGroup/`** (9 archivos): PedGroupDefaultTaskAllocator, PedGroupDefaultTaskAllocatorChat, PedGroupDefaultTaskAllocatorFollowAnyMeans, PedGroupDefaultTaskAllocatorFollowLimited, PedGroupDefaultTaskAllocatorRandom, PedGroupDefaultTaskAllocatorSitInLeaderCar, PedGroupDefaultTaskAllocatorStandStill, PedGroupDefaultTaskAllocatorType, PedGroupDefaultTaskAllocators

### `game/Peds`

- **`game/Peds/`** (52 archivos): CivilianPed, CopPed, EmergencyPed, MentalHealth, Ped, PedClothesDesc, PedDamageResponse, PedDamageResponseCalculator, PedGeometryAnalyser, PedIntelligence, PedList, PedPlacement, PedPool, PedStats, PedStdBonePositions, PedStuckChecker, PedTaskPair, PedType, PlayerInfo, PlayerInfoProcess.cpp, PlayerPed, PlayerPedData, PlayerRelationshipRecorder, PlayerSkin, Remote, Shopping, ePedRace, ePedState, ePedType
- **`game/Peds/Acquaintance/`** (2 archivos): Acquaintance
- **`game/Peds/Clothes/`** (7 archivos): Clothes, ClothesBuilder, eClothesModelPart, eClothesRules, eClothesTexturePart
- **`game/Peds/Conversations/`** (8 archivos): ConversationForPed, ConversationNode, Conversations, PedToPlayerConversations
- **`game/Peds/Gangs/`** (5 archivos): GangInfo, GangWars, Gangs
- **`game/Peds/PedIntelligence/`** (1 archivos): PedIntelligencePool

### `game/SaveLoad`

- **`game/SaveLoad/`** (14 archivos): C_PcSave, GangWarsSaveStructure, GenericGameStorage, ObjectSaveStructure, PedSaveStructure, SimpleVariablesSaveStructure, VehicleSaveStructure

### `game/Stats`

- **`game/Stats/`** (6 archivos): Stats, ePedStats, eStatModAbilities, eStats, eStatsReactions

### `game/Vehicles`

- **`game/Vehicles/`** (72 archivos): Automobile, Bike, Bmx, Boat, Bridge, CarEnterExit, CarGenerator, CurrentVehicle, CustomCarPlateMgr, CustomRoadsignMgr, Escalator, Escalators, Garage, Garages, Heli, LoadedCarGroup, MonsterTruck, Plane, PlaneBanners, PlaneTrail, PlaneTrails, QuadBike, SpecialPlateHandler, StoredCar, TheCarGenerators, Trailer, Train, TrainNode, Vehicle, VehicleAnimGroupData, VehiclePool, VehicleRecording, WaterCannon, WaterCannons, eCarWheel, eTargetDoor, eVehicleClass, eVehicleType, eWinchType
- **`game/Vehicles/Metadata/`** (12 archivos): HandlingDataType, cHandlingDataMgr, cTransmission, eVehicleHandlingFlags, eVehicleHandlingModelFlags, tBikeHandlingData, tBoatHandlingData, tFlyingHandlingData, tHandlingData, tTransmissionGear

### `game/ai`

- **`game/ai/`** (2 archivos): EntityScanner
- **`game/ai/ambient/`** (7 archivos): Birds, PopCycle, Population, eBirdsBiome
- **`game/ai/decisionmakers/`** (11 archivos): Decision, DecisionMaker, DecisionMakerTypes, DecisionMakerTypesFileLoader, DecisionSimple, eDecisionMakerEvents
- **`game/ai/scanners/`** (15 archivos): AttractorScanner, EventScanner, NearbyFireScanner, ObjectPotentialCollisionScanner, ObjectScanner, PedAcquaintanceScanner, PedScanner, SexyPedScanner, VehiclePotentialCollisionScanner, VehicleScanner

### `game/animation`

- **`game/animation/`** (32 archivos): AnimAssocAnimations, AnimAssocDefinitions, AnimAssocDescriptions, AnimAssociationData, AnimBlendAssocGroup, AnimBlendAssociation, AnimBlendClumpData, AnimBlendFrameData, AnimBlendHierarchy, AnimBlendNode, AnimBlendSequence, AnimBlendStaticAssociation, AnimBlock, AnimManager, AnimSequenceFrames, AnimationEnums, AnimationStyleDescriptor, README.md, RideAnimData, RideAnims, RpHAnimBlendInterpFrame, eAnimBlendCallbackType, eBoneTag

### `game/audio`

- **`game/audio/`** (10 archivos): AEAudioUtility, AESmoothFadeThread, AESound, AEStreamThread, AudioEngine
- **`game/audio/ambience/`** (4 archivos): AEAmbienceTrackManager, AEWeatherAudioEntity
- **`game/audio/config/`** (1 archivos): RadioStreamsPC
- **`game/audio/data/`** (1 archivos): SoundAttenuationTable
- **`game/audio/entities/`** (34 archivos): AEAudioEntity, AECollisionAudioEntity, AECollisionAudioEntity.CollisionLookup, AEDoorAudioEntity, AEExplosionAudioEntity, AEFireAudioEntity, AEFrontendAudioEntity, AEGlobalWeaponAudioEntity, AEPedAudioEntity, AEPedSpeechAudioEntity, AEPedWeaponAudioEntity, AEPedlessSpeechAudioEntity, AEPoliceScannerAudioEntity, AEScriptAudioEntity, AETwinLoopSoundEntity, AEWaterCannonAudioEntity, AEWeaponAudioEntity, PedSpeechAudioEntityLUTs.inc
- **`game/audio/enums/`** (21 archivos): BankSlotSoundIDs, PedSpeechContexts, PedSpeechVoices, SoundIDs, eAERadioType, eAEVehicleAudioType, eAEVehicleAudioTypeForName, eAEVehicleDoorType, eAEVehicleHornType, eAEVehicleSoundType, eAudioEvents, eAudioFileType, eBassSetting, ePedAudioType, eRadioID, eSoundBank, eSoundBankSlot, eSoundID, eUserTrackPlayMode, eUserTrackScanState, tAudioExtensionType
- **`game/audio/environment/`** (4 archivos): AEAudioEnvironment, AudioZones
- **`game/audio/hardware/`** (8 archivos): AEAudioChannel, AEAudioHardware, AEStaticChannel, AEStreamingChannel
- **`game/audio/loaders/`** (22 archivos): AEBankLoader, AEDataStream, AEMFDecoder, AEMP3BankLoader, AEMP3TrackLoader, AEStreamTransformer, AEStreamingDecoder, AETrackLoader, AEVorbisDecoder, AEWMADecoder, AEWaveDecoder
- **`game/audio/managers/`** (2 archivos): AESoundManager
- **`game/audio/music/`** (8 archivos): AECutsceneTrackManager, AERadioTrackManager, AEUserRadioTrackManager, CutSceneStreamsPC, tUserTracksInfo
- **`game/audio/openal/`** (6 archivos): OALBase, OALBuffer, OALSource
- **`game/audio/vehicleengine/`** (3 archivos): AEVehicleAudioEntity, AEVehicleAudioEntity.VehicleAudioSettings

### `game/camera`

- **`game/camera/`** (1 archivos): eCamMode
- **`game/camera/base/`** (2 archivos): Camera
- **`game/camera/gameplay/`** (4 archivos): Cam, IdleCam
- **`game/camera/queued/`** (1 archivos): QueuedMode
- **`game/camera/scripted/`** (1 archivos): CamPathSplines

### `game/control`

- **`game/control/`** (12 archivos): ControllerConfigManager, ControllerState, KeyboardState, MouseControllerState, Pad, Pad_SDL, eControllerType

### `game/event`

- **`game/event/`** (178 archivos): Event, EventAcquaintancePed, EventAcquaintancePedDislike, EventAcquaintancePedHate, EventAcquaintancePedHateBadlyLit, EventAcquaintancePedLike, EventAcquaintancePedRespect, EventAreaCodes, EventAttractor, EventBuildingCollision, EventCarUpsideDown, EventChatPartner, EventCopCarBeingStolen, EventCreatePartnerTask, EventDamage, EventDanger, EventDeadPed, EventDeath, EventDontJoinPlayerGroup, EventDraggedOutCar, EventEditableResponse, EventEscalator, EventFireNearby, EventGlobalGroup, EventGotKnockedOverByCar, EventGroup, EventGroupEvent, EventGunAimedAt, EventGunShot, EventGunShotWhizzedBy, EventHandler, EventHandlerHistory, EventHealthLow, EventHealthReallyLow, EventHighAngerAtPlayer, EventHitByWaterCannon, EventInAir, EventInWater, EventInteriorUseInfo, EventKnockOffBike, EventLeaderEnteredCarAsDriver, EventLeaderEntryExit, EventLeaderExitedCarAsDriver, EventLeaderQuitEnteringCarAsDriver, EventLeanOnVehicle, EventLowAngerAtPlayer, EventNewGangMember, EventObjectCollision, EventOnFire, EventPassObject, EventPedCollisionWithPed, EventPedCollisionWithPlayer, EventPedEnteredMyVehicle, EventPedToChase, EventPedToFlee, EventPlayerCollisionWithPed, EventPlayerCommandToGroup, EventPlayerCommandToGroupAttack, EventPlayerCommandToGroupGather, EventPool, EventPotentialGetRunOver, EventPotentialWalkIntoBuilding, EventPotentialWalkIntoFire, EventPotentialWalkIntoObject, EventPotentialWalkIntoPed, EventPotentialWalkIntoVehicle, EventRevived, EventScriptCommand, EventScriptedAttractor, EventSeenCop, EventSeenPanickedPed, EventSexyPed, EventSexyVehicle, EventSignalAtPed, EventSoundLoud, EventSoundQuiet, EventSource, EventSpecial, EventStuckInAir, EventVehicleCollision, EventVehicleDamage, EventVehicleDamageCollision, EventVehicleDamageWeapon, EventVehicleDied, EventVehicleHitAndRun, EventVehicleOnFire, EventVehicleThreat, EventVehicleToSteal, InformFriendsEventQueue, InformGroupEvent, InformGroupEventQueue, InterestingEvents, eEventType

### `game/frontend`

- **`game/frontend/`** (29 archivos): Credits, Font, Hud, HudColours, LoadingScreen, MenuManager, MenuSystem, Messages, OnscreenCounterEntry, OnscreenTimer, OnscreenTimerEntry, PlaceName, UserDisplay, eFontAlignment, eHud, eOnscreenCounter
- **`game/frontend/menu/`** (8 archivos): FrontendScreensPC, MenuManager_Draw, MenuManager_Gallery, MenuManager_Input, MenuManager_Internal, MenuManager_Pages, MenuManager_Process
- **`game/frontend/minimap/`** (2 archivos): Radar

### `game/ik`

- **`game/ik/`** (8 archivos): IKChain, IKChainManager, PedIK, eIKArm, eIKChainSlot

### `game/modelinfo`

- **`game/modelinfo/`** (31 archivos): AtomicModelInfo, BaseModelInfo, ClumpModelInfo, DamageAtomicModelInfo, LodAtomicModelInfo, LodTimeModelInfo, ModelIndices, ModelInfo, ModelInfoAccelerator, PedModelInfo, TimeInfo, TimeModelInfo, VehicleModelInfo, WeaponModelInfo, eItemDefinitionFlags, eModelID, ePedModel

### `game/pathserver`

- **`game/pathserver/`** (12 archivos): NodeAddress, NodeRoute, NodeRoutePool, PathFind, PatrolRoutePool, PointList, PointRoutePool, Route

### `game/physics`

- **`game/physics/`** (15 archivos): Buoyancy, DamageManager, PolyBunch, Rope, Ropes, StoredCollPoly, SurfaceInfo_c, SurfaceInfos_c, eSurfaceType
- **`game/physics/collision/`** (39 archivos): BoundingBox, Box, ColAccel, ColBox, ColDisk, ColHelpers, ColLine, ColModel, ColModelPool, ColPoint, ColSphere, ColStore, ColSurface, ColTriangle, ColTrianglePlane, Collision, CollisionData, CollisionEventScanner, ColourSet, Sphere, TempColModels
- **`game/physics/ragdoll/`** (5 archivos): BoneInfo, BoneNode, BoneNodeManager

### `game/pickups`

- **`game/pickups/`** (5 archivos): Pickup, Pickups, tPickupMessage

### `game/renderer`

- **`game/renderer/`** (22 archivos): Draw, Gamma, Lines, Mirrors, PostEffects, RenderBuffer, Renderer, ShinyTexts, SpecialFX, Sprite, Sprite2d
- **`game/renderer/Lights/`** (10 archivos): BrightLights, Coronas, PointLights, RegisteredCorona, SearchLight
- **`game/renderer/MotionBlur/`** (6 archivos): MBlur, MotionBlurStreaks, RegisteredMotionBlurStreak
- **`game/renderer/Shadows/`** (12 archivos): RealTimeShadow, RealTimeShadowManager, ShadowCamera, Shadows, StencilShadowObject, StencilShadows
- **`game/renderer/markers/`** (4 archivos): 3dMarker, 3dMarkers
- **`game/renderer/occlusion/`** (6 archivos): ActiveOccluder, Occluder, Occlusion
- **`game/renderer/visibility/`** (2 archivos): VisibilityPlugins

### `game/replaycoordinator`

- **`game/replaycoordinator/`** (5 archivos): Replay, ReplayBlockType, eReplay

### `game/scene`

- **`game/scene/`** (25 archivos): BuildingPool, EntryInfoList, EntryInfoNode, EntryInfoNodePool, GridRef, IplDefPool, PtrNodeDoubleLinkPool, PtrNodeSingleLinkPool, Reference, References, RepeatSector, Scene, Sector, TagManager, World, World_Helper, eAreaCodes, eLevelName
- **`game/scene/EntryExit/`** (4 archivos): EntryExit, EntryExitManager
- **`game/scene/Interior/`** (19 archivos): FurnitureEntity_c, FurnitureGroup_c, FurnitureItem, FurnitureManager_c, FurnitureSubGroup_c, Furniture_c, InteriorEffectInfo_t, InteriorGroup_c, InteriorInfo_t, InteriorManager_c, InteriorObject, Interior_c
- **`game/scene/entities/`** (12 archivos): AnimatedBuilding, Building, Entity, Physical, Placeable, eEntityStatus, eEntityType
- **`game/scene/ipl/`** (7 archivos): FileLoader, FileObjectInstance, IplDef, IplStore, tBinaryIplFile
- **`game/scene/water/`** (6 archivos): WaterCreatureManager_c, WaterCreature_c, WaterLevel
- **`game/scene/zones/`** (7 archivos): CullZones, TheZones, Zone, ZoneInfo

### `game/script`

- **`game/script/`** (30 archivos): Checkpoint, Checkpoints, Darkel, MissionCleanup, OpcodeResult, RunningScript, SCMChunks, ScriptParam, ScriptResourceManager, Scripted2dEffects, ScriptsForBrains, SetPiece, SetPieces, StreamedScripts, StuntJumpManager, TheScripts, eScriptCommands
- **`game/script/CommandParser/`** (5 archivos): LUTGenerator, Parser, ReadArg, StoreArg, Utility
- **`game/script/Commands/`** (25 archivos): BasicCommands, CameraCommands, CharacterCommands, ClockCommands, Commands, ComparasionCommands, ConversationCommands, DrawCommands, GameCommands, GenericCommands, MathCommands, MissionCommands, ObjectCommands, PadCommands, PathCommands, PedCommands, PlayerCommands, ScriptCommands, SequenceCommands, StatCommands, TextCommands, UnusedCommands, UtilityCommands, VehicleCommands, ZoneCommands
- **`game/script/Commands/CLEO/`** (12 archivos): CLEOAudioStreamCommands, CLEOCharacterCommands, CLEOCommands, CLEODynamicLibraryCommands, CLEOFsCommands, CLEOGameCommands, CLEOGenericCommands, CLEOMemoryCommands, CLEOPadCommands, CLEOScriptCommands, CLEOVehicleCommands, CLEOWorldCommands

### `game/streaming`

- **`game/streaming/`** (15 archivos): CdStreamInfo, Directory, LoadMonitor, Streaming, StreamingInfo, TxdDef, TxdStore, TxdStore_Plugin

### `game/system`

- **`game/system/`** (28 archivos): Cheat, Debug, FileMgr, Game, GameLogic, HandShaker, IniFile, Localisation, MemoryMgr, PedDebugVisualiser, Pools, Restart, Timer, eCheats, eGameState

### `game/task`

- **`game/task/Animation/`** (50 archivos): TaskComplexFacial, TaskComplexPlayHandSignalAnim, TaskComplexUseGoggles, TaskSimpleAnim, TaskSimpleChoking, TaskSimpleClearLookAt, TaskSimpleCower, TaskSimpleDoHandSignal, TaskSimpleFacial, TaskSimpleHandsUp, TaskSimpleIKChain, TaskSimpleIKLookAt, TaskSimpleIKManager, TaskSimpleIKPointArm, TaskSimpleLookAbout, TaskSimplePlayHandSignalAnim, TaskSimpleRunAnim, TaskSimpleRunAnimLoopedMiddle, TaskSimpleRunNamedAnim, TaskSimpleRunTimedAnim, TaskSimpleSay, TaskSimpleScratchHead, TaskSimpleShakeFist, TaskSimpleTired, TaskSimpleTriggerLookAt
- **`game/task/Combat/`** (64 archivos): TaskComplexDestroyCar, TaskComplexDestroyCarArmed, TaskComplexDestroyCarMelee, TaskComplexFollowNodeRouteShooting, TaskComplexGoToPointAiming, TaskComplexGoToPointShooting, TaskComplexHitPedWithCar, TaskComplexKillAllThreats, TaskComplexKillCriminal, TaskComplexKillPedFromBoat, TaskComplexKillPedGroupOnFoot, TaskComplexKillPedOnFoot, TaskComplexKillPedOnFootArmed, TaskComplexKillPedOnFootMelee, TaskComplexKillPedOnFootStealth, TaskComplexSeekCoverUntilTargetDead, TaskComplexSeekEntityAiming, TaskGangHasslePed, TaskGangHassleVehicle, TaskSimpleDetonate, TaskSimpleFight, TaskSimpleFightingControl, TaskSimpleGangDriveBy, TaskSimpleGunControl, TaskSimpleHurtPedWithCar, TaskSimpleKillPedWithCar, TaskSimpleSetCharIgnoreWeaponRangeFlag, TaskSimpleStealthKill, TaskSimpleThrowControl, TaskSimpleThrowProjectile, TaskSimpleTogglePedThreatScanner, TaskSimpleUseGun
- **`game/task/Combat/Cover/`** (4 archivos): Cover, CoverPoint
- **`game/task/Crimes/`** (15 archivos): Crime, CrimeBeingQd, TaskComplexRoadRage, TaskComplexScreamInCarThenLeave, TaskComplexStealCar, TaskComplexWanderCriminal, Wanted, eCrimeType, eWantedLevel
- **`game/task/Default/`** (32 archivos): TaskComplexBeInGroup, TaskComplexGangFollower, TaskComplexGangJoinRespond, TaskComplexGangLeader, TaskComplexSignalAtPed, TaskComplexStareAtPed, TaskComplexUseScriptedBrain, TaskSimpleAffectSecondaryBehaviour, TaskSimpleFinishBrain, TaskSimpleLeaveGroup, TaskSimpleNone, TaskSimplePause, TaskSimplePlayerOnFoot, TaskSimpleSetCharDecisionMaker, TaskSimpleTriggerEvent, TaskSimpleUninterruptable
- **`game/task/Helpers/`** (15 archivos): TaskComplexGoPickUpEntity, TaskComplexPassObject, TaskSimpleHoldEntity, TaskSimplePickUpEntity, TaskSimplePutDownEntity, TaskSimpleWaitUntilAreaCodesMatch, TaskSimpleWaitUntilLeaderAreaCodesMatch, TaskUtilityLineUpPedWithCar
- **`game/task/Movement/`** (92 archivos): TaskComplexAvoidEntity, TaskComplexAvoidOtherPedWhileWandering, TaskComplexBeStill, TaskComplexClimb, TaskComplexFollowLeaderAnyMeans, TaskComplexFollowLeaderInFormation, TaskComplexFollowNodeRoute, TaskComplexFollowPatrolRoute, TaskComplexFollowPedFootsteps, TaskComplexFollowPointRoute, TaskComplexGoToPointAndStandStill, TaskComplexGoToPointAndStandStillAndAchieveHeading, TaskComplexGoToPointAndStandStillTimed, TaskComplexGoToPointAnyMeans, TaskComplexGoToPointUntilCollisionAndStandStill, TaskComplexGotoDoorAndOpen, TaskComplexInWater, TaskComplexJump, TaskComplexMoveBackAndJump, TaskComplexTrackEntity, TaskComplexTurnToFaceEntityOrCoord, TaskComplexWalkAlongsidePed, TaskComplexWalkRoundBuildingAttempt, TaskComplexWalkRoundCar, TaskComplexWalkRoundFire, TaskComplexWalkRoundObject, TaskComplexWander, TaskComplexWanderGang, TaskComplexWanderStandard, TaskSimpleAbseil, TaskSimpleAchieveHeading, TaskSimpleClimb, TaskSimpleClimb_models, TaskSimpleGoTo, TaskSimpleGoToPoint, TaskSimpleGoToPointFine, TaskSimpleJetPack, TaskSimpleJump, TaskSimpleLand, TaskSimpleSetKindaStayInSamePlace, TaskSimpleSetStayInSamePlace, TaskSimpleSlideToCoord, TaskSimpleStandStill, TaskSimpleStandUp, TaskSimpleSwim, TaskSimpleTurn180, eMoveState, eSprintType
- **`game/task/Movement/SeekEntity/`** (10 archivos): TaskComplexSeekEntity, TaskComplexSeekEntityAnyMeans, TaskComplexSeekEntityAnyMeansXYOffset, TaskComplexSeekEntityRadiusAngleOffset, TaskComplexSeekEntityStandard, TaskComplexSeekEntityXYOffset
- **`game/task/Movement/SeekEntity/PosCalculators/`** (10 archivos): EntitySeekPosCalculator, EntitySeekPosCalculatorFixedPos, EntitySeekPosCalculatorRadiusAngleOffset, EntitySeekPosCalculatorStandard, EntitySeekPosCalculatorXYOffset
- **`game/task/Physics/`** (8 archivos): TaskComplexDiveFromAttachedEntityAndGetUp, TaskComplexInAirAndLand, TaskComplexStuckInAir, TaskSimpleInAir
- **`game/task/Response/`** (104 archivos): TaskComplexCrossRoadLookAndAchieveHeading, TaskComplexDie, TaskComplexDieInCar, TaskComplexEvasiveCower, TaskComplexEvasiveDiveAndGetUp, TaskComplexEvasiveStep, TaskComplexFallAndGetUp, TaskComplexFallAndStayDown, TaskComplexFallToDeath, TaskComplexFleeAnyMeans, TaskComplexFleeEntity, TaskComplexFleePoint, TaskComplexFleeShooting, TaskComplexGetUpAndStandStill, TaskComplexHitByGunResponse, TaskComplexHitResponse, TaskComplexInvestigateDeadPed, TaskComplexInvestigateDisturbance, TaskComplexLeaveCarAndFlee, TaskComplexOnFire, TaskComplexReactToAttack, TaskComplexReactToGunAimedAt, TaskComplexSmartFleeEntity, TaskComplexSmartFleePoint, TaskComplexWanderFlee, TaskSimpleBeHit, TaskSimpleBeHitWhileMoving, TaskSimpleBeKickedOnGround, TaskSimpleDead, TaskSimpleDie, TaskSimpleDieInCar, TaskSimpleDrown, TaskSimpleDrownInCar, TaskSimpleDuck, TaskSimpleDuckToggle, TaskSimpleDuckWhileShotsWhizzing, TaskSimpleEvasiveDive, TaskSimpleEvasiveStep, TaskSimpleFall, TaskSimpleGetUp, TaskSimpleHitByGunFromFront, TaskSimpleHitByGunFromLeft, TaskSimpleHitByGunFromRear, TaskSimpleHitByGunFromRight, TaskSimpleHitFromBack, TaskSimpleHitFromBehind, TaskSimpleHitFromFront, TaskSimpleHitFromLeft, TaskSimpleHitFromRight, TaskSimpleHitHead, TaskSimpleHitWall, TaskSimplePlayerOnFire
- **`game/task/Scenario/`** (79 archivos): PedAttractorManager, PedAttractorPool, TaskComplexAttractorPartnerWait, TaskComplexBeInCouple, TaskComplexChat, TaskComplexGoToAttractor, TaskComplexPartner, TaskComplexPartnerChat, TaskComplexPartnerDeal, TaskComplexPartnerGreet, TaskComplexPartnerShove, TaskComplexProstituteSolicit, TaskComplexSitDownThenIdleThenStandUp, TaskComplexSunbathe, TaskComplexUseAttractor, TaskComplexUseAttractorPartner, TaskComplexUseClosestFreeScriptedAttractor, TaskComplexUseClosestFreeScriptedAttractorRun, TaskComplexUseClosestFreeScriptedAttractorSprint, TaskComplexUseEffect, TaskComplexUseEffectRunning, TaskComplexUseEffectSprinting, TaskComplexUseEntryExit, TaskComplexUseMobilePhone, TaskComplexUsePairedAttractor, TaskComplexUseScriptedAttractor, TaskComplexUseWaterCannon, TaskComplexWaitAtAttractor, TaskComplexWaitForBus, TaskComplexWaitForDryWeather, TaskComplexWaitForPed, TaskComplexWanderProstitute, TaskSimpleChat, TaskSimpleHailTaxi, TaskSimpleHailTaxiAndPause, TaskSimpleSitDown, TaskSimpleSitIdle, TaskSimpleUseAtm, TaskSimpleWaitForBus, TaskSimpleWaitForPizza
- **`game/task/Scenario/Attractors/`** (15 archivos): PedAtmAttractor, PedAttractor, PedAttractorPedPlacer, PedLookAtAttractor, PedParkAttractor, PedPizzaAttractor, PedScriptedAttractor, PedSeatAttractor, PedShelterAttractor, PedStepAttractor, PedStopAttractor, PedTriggerScriptAttractor
- **`game/task/Scenario/Interior/`** (18 archivos): TaskInteriorBeInHouse, TaskInteriorBeInOffice, TaskInteriorBeInShop, TaskInteriorGoToInfo, TaskInteriorLieInBed, TaskInteriorShopKeeper, TaskInteriorSitAtDesk, TaskInteriorSitInChair, TaskInteriorUseInfo
- **`game/task/Service/Fire/`** (6 archivos): TaskComplexDriveFireTruck, TaskComplexExtinguishFireOnFoot, TaskComplexExtinguishFires
- **`game/task/Service/Medic/`** (12 archivos): Accident, AccidentManager, TaskComplexMedicTreatInjuredPed, TaskComplexTreatAccident, TaskComplexWanderMedic, TaskSimpleGiveCPR
- **`game/task/Service/Police/`** (16 archivos): TaskComplexArrestPed, TaskComplexBeCop, TaskComplexCopInCar, TaskComplexPolicePursuit, TaskComplexPresentIDToCop, TaskComplexWaitForBackup, TaskComplexWanderCop, TaskSimpleArrestPed
- **`game/task/Service/Swat/`** (2 archivos): TaskComplexUseSwatRope
- **`game/task/System/`** (24 archivos): PedScriptedTaskRecord, ScriptedBrainTaskStore, Task, TaskAllocatorPool, TaskCategories, TaskComplex, TaskComplexSequence, TaskComplexUseSequence, TaskManager, TaskPool, TaskSequences, TaskSimple, TaskTimer, TickCounter, eTaskType
- **`game/task/Vehicle/`** (124 archivos): TaskComplexCarDrive, TaskComplexCarDriveMission, TaskComplexCarSlowBeDraggedOut, TaskComplexCarSlowBeDraggedOutAndStandUp, TaskComplexDragPedFromCar, TaskComplexDrivePointRoute, TaskComplexDriveToPoint, TaskComplexDriveWander, TaskComplexEnterAnyCarAsDriver, TaskComplexEnterBoatAsDriver, TaskComplexEnterCar, TaskComplexEnterCarAsDriver, TaskComplexEnterCarAsDriverTimed, TaskComplexEnterCarAsPassenger, TaskComplexEnterCarAsPassengerTimed, TaskComplexEnterCarAsPassengerWait, TaskComplexGetOnBoatSeat, TaskComplexGoToBoatSteeringWheel, TaskComplexGoToCarDoorAndStandStill, TaskComplexLeaveAnyCar, TaskComplexLeaveBoat, TaskComplexLeaveCar, TaskComplexLeaveCarAndDie, TaskComplexLeaveCarAndWander, TaskComplexLeaveCarAsPassengerWait, TaskComplexObserveTrafficLights, TaskComplexObserveTrafficLightsAndAchieveHeading, TaskComplexOpenDriverDoor, TaskComplexOpenPassengerDoor, TaskComplexShuffleSeats, TaskGoToVehicleAndLean, TaskLeanOnVehicle, TaskSimpleBikeJacked, TaskSimpleCarAlign, TaskSimpleCarCloseDoorFromInside, TaskSimpleCarCloseDoorFromOutside, TaskSimpleCarDrive, TaskSimpleCarDriveTimed, TaskSimpleCarFallOut, TaskSimpleCarForcePedOut, TaskSimpleCarGetIn, TaskSimpleCarGetOut, TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse, TaskSimpleCarJumpOut, TaskSimpleCarOpenDoorFromOutside, TaskSimpleCarOpenLockedDoorFromOutside, TaskSimpleCarSetPedInAsDriver, TaskSimpleCarSetPedInAsPassenger, TaskSimpleCarSetPedOut, TaskSimpleCarSetPedSlowDraggedOut, TaskSimpleCarSetTempAction, TaskSimpleCarShuffle, TaskSimpleCarSlowBeDraggedOut, TaskSimpleCarSlowDragPedOut, TaskSimpleCarWaitForDoorNotToBeInUse, TaskSimpleCarWaitToSlowDown, TaskSimpleCreateCarAndGetIn, TaskSimpleOnEscalator, TaskSimplePickUpBike, TaskSimpleSetPedAsAutoDriver, TaskSimpleWaitUntilPedIsInCar, TaskSimpleWaitUntilPedIsOutCar

### `game/text`

- **`game/text/`** (11 archivos): Data, GxtChar, KeyArray, MissionTextOffsets, Text, eLanguage

### `game/timecycle`

- **`game/timecycle/`** (12 archivos): Clock, Date, TimeCycle, Weather, Weather.def, WindModifiers, eWeatherType

### `game/vehicleAi`

- **`game/vehicleAi/`** (17 archivos): AutoPilot, CarAI, CarCtrl, RoadBlocks, StuckCarCheck, TrafficLights, UpsideDownCarCheck, eCarDrivingStyle, eCarMission, eCarNodes

### `game/vfx`

- **`game/vfx/fire/`** (6 archivos): CreepingFire, Fire, FireManager
- **`game/vfx/misc/`** (12 archivos): CarFXRenderer, Clouds, GrassRenderer, Rubbish, Skidmark, Skidmarks
- **`game/vfx/particles/`** (39 archivos): EmissionInfo, Fx, FxBox, FxEmitter, FxEmitterBP, FxEmitterPrt, FxFrustumInfo, FxInfoManager, FxManager, FxMemoryPool, FxPlane, FxPlane_c, FxPrim, FxPrimBP, FxPrtMult, FxSphere, FxSystem, FxSystemBP, FxTools, MovementInfo, Particle, RenderInfo, eFxInfoType
- **`game/vfx/particles/Blueprints/`** (69 archivos): FxInfo, FxInfoAnimTexture, FxInfoAttractLine, FxInfoAttractPt, FxInfoColour, FxInfoColourBright, FxInfoColourRange, FxInfoDir, FxInfoEmAngle, FxInfoEmDir, FxInfoEmLife, FxInfoEmPos, FxInfoEmRate, FxInfoEmRotation, FxInfoEmSize, FxInfoEmSpeed, FxInfoEmWeather, FxInfoFlat, FxInfoFloat, FxInfoForce, FxInfoFriction, FxInfoGroundCollide, FxInfoHeatHaze, FxInfoJitter, FxInfoNoise, FxInfoRotSpeed, FxInfoRotate, FxInfoRotateOffset, FxInfoSelfLit, FxInfoSize, FxInfoSmoke, FxInfoSpriteRect, FxInfoTexCoords, FxInfoTrail, FxInfoUnderwater, FxInfoWind
- **`game/vfx/particles/Blueprints/Interp/`** (10 archivos): FxInterpInfo, FxInterpInfo255, FxInterpInfo32, FxInterpInfoFloat, FxInterpInfoU255
- **`game/vfx/plants/`** (14 archivos): PPTriPlantBuffer, PlantColEntEntry, PlantLocTri, PlantMgr, PlantSurfPropMgr, ProcObjectMan, ProcSurfaceInfo

### `game/weapons`

- **`game/weapons/`** (26 archivos): BulletInfo, BulletTrace, BulletTraces, Explosion, Projectile, ProjectileInfo, ShotInfo, Weapon, WeaponEffects, WeaponInfo, eMeleeCombo, eWeaponFire, eWeaponFlags, eWeaponModel, eWeaponSkill, eWeaponType

### `renderware`

- **`renderware/`** (10 archivos): D3DIndexDataBuffer, D3DResourceSystem, D3DTextureBuffer, RenderWare, RwHelper, RwObjectNameIdAssocation
- **`renderware/pipelines/`** (1 archivos): PipelinesCommon
- **`renderware/pipelines/CustomBuilding/`** (6 archivos): CustomBuildingDNPipeline, CustomBuildingPipeline, CustomBuildingRenderer
- **`renderware/pipelines/CustomCarEnvMap/`** (5 archivos): CustomCarEnvMapPipeline, CustomCarEnvMapPipeline_EnvAtm, CustomCarEnvMapPipeline_EnvMat, CustomCarEnvMapPipeline_SpecMat
- **`renderware/plugins/BreakablePlugin/`** (2 archivos): BreakablePlugin
- **`renderware/plugins/CollisionPlugin/`** (2 archivos): CollisionPlugin
- **`renderware/plugins/JPegCompressPlugin/`** (2 archivos): JPegCompress
- **`renderware/plugins/NodeNamePlugin/`** (2 archivos): NodeName
- **`renderware/plugins/PipelinePlugin/`** (2 archivos): PipelinePlugin
- **`renderware/plugins/RpAnimBlendPlugin/`** (2 archivos): RpAnimBlend
- **`renderware/plugins/TwoDEffectPlugin/`** (3 archivos): 2dEffect, 2dEffectStream
- **`renderware/rw/`** (81 archivos): rpadc, rpanisot, rpcollis, rpcriter, rpdbgerr, rpdmorph, rpenv, rperror, rphanim, rplodatm, rplogo, rpltmap, rpmatfx, rpmipkl, rpmorph, rpnormmap, rppatch, rpplugin, rpprtstd, rpptank, rppvs, rprandom, rpskin, rpspline, rpusrdat, rpuvanim, rpworld, rt2d, rt2danim, rtanim, rtbary, rtbezpat, rtbmp, rtcharse, rtcmpkey, rtdict, rtfsyst, rtgcond, rtgncpip, rtimport, rtintsec, rtltmap, rtltmapcnv, rtmipk, rtnormmap, rtpick, rtpitexd, rtpng, rtquat, rtras, rtray, rtskinsp, rtslerp, rtsplpvs, rttiff, rttilerd, rttoc, rtvcat, rtwing, rtworld, rwcore, rwplcore, rwtexdict, rwversion, skeleton

## Carpetas de `dev_ng/game` que no se crearían (sin equivalente en SA)

`ai/ambient` solo se crea con Birds/Population/PopCycle. No hay equivalente para: `apps`, `cloth`, `debug` (SA mezcla la depuración en `system`), `glassPaneSyncing`, `network`, `optimisations.h`, `performance`, `replaycoordinator` (se crea solo para `Replay`), `security`, `shader_source`, `shaders`, `test_ignore`, `tools`, `utils`, `VS_Project*`, `SaveMigration`.

## Carpetas nuevas que `dev_ng` no tiene tal cual (decisiones propias)

- `game/task/Vehicle`: `dev_ng` reparte las tareas de vehículo entre `Motion/Vehicle` y `vehicleAi`. En SA son muchas (más de 60) y claras, por eso se agrupan.
- `game/Peds/Conversations`, `game/Peds/Gangs`, `game/Peds/Clothes`, `game/Peds/Acquaintance`: sistemas propios de SA dentro de `Peds`.
- `game/scene/Interior`, `game/scene/ipl`, `game/scene/zones`, `game/scene/EntryExit`, `game/scene/water`: subdivisión de `scene`.
- `game/vfx/fire`, `game/vfx/plants`, `game/vfx/particles`: el sistema de partículas de SA (`Fx`) y sus planos van en `vfx/particles`.
- `game/PedGroup/Allocators`: los asignadores de tareas de grupo.
- `base/`, `renderware/`: ver criterio.

## Decisiones dudosas (conviene revisarlas)

- **Attractors** (`game/task/Scenario/Attractors`): en SA son puntos de interacción; en GTA V son escenarios. Es la equivalencia más discutible.
- **`Cover`/`CoverPoint`** van a `task/Combat/Cover` por el nombre; en SA son usados por IA de combate y de peatones.
- **Wanted/Crime** van a `task/Crimes` por ser el sistema de crímenes; `Wanted` es del jugador y podría ir a `Peds`.
- **Tareas**: se clasificaron por familia de nombre con reglas automáticas (ver `TASK_RULES` en el script). Hay tareas que podrían ir a dos sitios, por ejemplo `ComplexLeaveCarAndFlee` (Vehicle o Response), `ComplexUseWaterCannon`, `ComplexChat`.
- **Enums compartidos** (`eTaskType`, `ePedType`, `eVehicleType`...) van junto a su dueño principal; los incluyen otros sistemas.
- **`Pools.h`/`Pools.cpp`**: el contenedor central va a `game/system`.

## Qué habría que actualizar al aplicarlo

- Los 591 `build_probe.py` y `register_*.py` de `reversing/` con rutas de fuente escritas a mano.
- `reversing/validation-registry.json`: unas 2164 rutas de fuente con sus hashes. Las pruebas ya aprobadas dejarían de coincidir con la ruta.
- `contrib/correlate_sources.py`, `contrib/source_descriptions.py` y `sync_docs.py --update-source-status`.
- Los documentos congelados y los hashes históricos (no se deben tocar): hay que comprobar si citan rutas de fuente.
- `source/GUIA_DE_CARPETAS.md` y `docs/*.md` que citan rutas.
- Los `#include`: la mayoría son nombres cortos, pero depende de que CMake añada las carpetas nuevas como directorios de include.
- Se haría con `git mv` en un solo commit y se comprobaría con `audit_hook_catalog.py --strict`.

