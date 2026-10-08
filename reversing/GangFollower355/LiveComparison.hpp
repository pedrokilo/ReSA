#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexGangFollower.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleHoldEntity.h"
#include "EventPassObject.h"
#include "Fx/FxManager.h"
#include "Ragdoll/IKChainManager.h"
#include "../ProbeRuntime.hpp"
namespace GangFollower355Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskComplexGangFollower","Constructor",0x65EAA0,0,-1,256},
{"CTaskComplexGangFollower","Destructor",0x65EBB0,0,-2,256},
{"CTaskComplexGangFollower","Clone",0x65ECB0,0x86F938,1,256},
{"CTaskComplexGangFollower","MakeAbortable",0x65EC30,0x86F938,6,512},
{"CTaskComplexGangFollower","CreateNextSubTask",0x665E00,0x86F938,8,1024},
{"CTaskComplexGangFollower","CreateFirstSubTask",0x666160,0x86F938,9,1024},
{"CTaskComplexGangFollower","ControlSubTask",0x662A10,0x86F938,10,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped,player;
alignas(16) inline std::array<uint8,sizeof(CTaskComplexGangFollower)> task;
alignas(16) inline std::array<uint8,256> arena,subtask;
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted;
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;
alignas(16) inline std::array<uint8,sizeof(CPed)> receiver;alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group;alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix,pedMatrix;alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel,leaderIntel,receiverIntel;alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;alignas(16) inline std::array<uint8,sizeof(CObject)> held;alignas(16) inline std::array<uint8,128> gotoTask;alignas(16) inline std::array<uint8,sizeof(RpClump)> clump;alignas(16) inline std::array<uint8,sizeof(RwFrame)> frame;alignas(16) inline std::array<uint8,sizeof(FxSystem_c)> fx;inline CVector constructorPos;inline int mode;inline std::array<void*,11> subVT{}; inline int subType{},scenario{};inline size_t allocated{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+x.size())return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(player) P(task) P(arena) P(subtask) P(wanted) P(vehicle) P(receiver) P(group) P(matrix) P(pedMatrix) P(intel) P(leaderIntel) P(receiverIntel) P(assoc) P(held) P(gotoTask) P(clump) P(frame) P(fx)
#undef P
return p;}
inline json VmtIdentity355(uintptr_t value) {
    struct Table { const char* name; uintptr_t canonical; };
    static constexpr Table tables[]{
{"CTaskAllocator",0x870E30},
{"CTaskAllocatorAttack",0x870e60},
{"CTaskAllocatorKillOnFoot",0x870e48},
{"CTaskAllocatorKillThreatsBasic",0x870e90},
{"CTaskAllocatorKillThreatsBasicRandomGroup",0x86C80C},
{"CTaskAllocatorKillThreatsDriveby",0x870ea8},
{"CTaskAllocatorPlayerCommandAttack",0x870e78},
{"CTaskComplexArrestPed",0x8709A8},
{"CTaskComplexAttractorPartnerWait",0x86e284},
{"CTaskComplexAvoidEntity",0x86FF00},
{"CTaskComplexAvoidOtherPedWhileWandering",0x86FEC4},
{"CTaskComplexBeCop",0x85A428},
{"CTaskComplexBeInCouple",0x870724},
{"CTaskComplexBeInGroup",0x86E22C},
{"CTaskComplexBeStill",0x86C7BC},
{"CTaskComplexCarDrive",0x86E934},
{"CTaskComplexCarDriveMission",0x86E9A4},
{"CTaskComplexCarDriveMissionFleeScene",0x85B95C},
{"CTaskComplexCarDriveMissionKillPed",0x85B994},
{"CTaskComplexCarDriveWander",0x86E96C},
{"CTaskComplexCarSlowBeDraggedOut",0x86EF54},
{"CTaskComplexCarSlowBeDraggedOutAndStandUp",0x86EF80},
{"CTaskComplexChat",0x8706A0},
{"CTaskComplexClimb",0x859DCC},
{"CTaskComplexCopInCar",0x870A2C},
{"CTaskComplexCrossRoadLookAndAchieveHeading",0x86DFF0},
{"CTaskComplexDestroyCar",0x86d968},
{"CTaskComplexDestroyCarArmed",0x86d9c4},
{"CTaskComplexDestroyCarMelee",0x86d994},
{"CTaskComplexDie",0x86DE78},
{"CTaskComplexDieInCar",0x86DE04},
{"CTaskComplexDiveFromAttachedEntityAndGetUp",0x85a358},
{"CTaskComplexDragPedFromCar",0x86EB6C},
{"CTaskComplexDriveFireTruck",0x86F570},
{"CTaskComplexDrivePointRoute",0x86ebf8},
{"CTaskComplexDriveToPoint",0x86E9DC},
{"CTaskComplexEnterAnyCarAsDriver",0x86EA14},
{"CTaskComplexEnterBoatAsDriver",0x86e7d0},
{"CTaskComplexEnterCar",0x86e6f0},
{"CTaskComplexEnterCarAsDriver",0x86EAAC},
{"CTaskComplexEnterCarAsDriverTimed",0x86e720},
{"CTaskComplexEnterCarAsPassenger",0x86EADC},
{"CTaskComplexEnterCarAsPassengerTimed",0x86e74c},
{"CTaskComplexEnterCarAsPassengerWait",0x86E778},
{"CTaskComplexEvasiveCower",0x86F3BC},
{"CTaskComplexEvasiveDiveAndGetUp",0x86f268},
{"CTaskComplexEvasiveStep",0x86F218},
{"CTaskComplexExtinguishFireOnFoot",0x86F5FC},
{"CTaskComplexExtinguishFires",0x870dd0},
{"CTaskComplexFacial",0x870ADC},
{"CTaskComplexFallAndGetUp",0x870454},
{"CTaskComplexFallAndStayDown",0x870480},
{"CTaskComplexFallToDeath",0x870520},
{"CTaskComplexFleeAnyMeans",0x86f7f0},
{"CTaskComplexFleeEntity",0x86f708},
{"CTaskComplexFleePoint",0x86f6cc},
{"CTaskComplexFleeShooting",0x86f7c4},
{"CTaskComplexFollowLeaderAnyMeans",0x870c68},
{"CTaskComplexFollowLeaderInFormation",0x870c3c},
{"CTaskComplexFollowNodeRoute",0x8700a8},
{"CTaskComplexFollowNodeRouteShooting",0x0},
{"CTaskComplexFollowPatrolRoute",0x870114},
{"CTaskComplexFollowPedFootsteps",0x870CC0},
{"CTaskComplexFollowPointRoute",0x870078},
{"CTaskComplexGangFollower",0x86F938},
{"CTaskComplexGangJoinRespond",0x86FB5C},
{"CTaskComplexGangLeader",0x86F8FC},
{"CTaskComplexGetOnBoatSeat",0x86f044},
{"CTaskComplexGetUpAndStandStill",0x870404},
{"CTaskComplexGoToAttractor",0x86FF3C},
{"CTaskComplexGoToBoatSteeringWheel",0x86f018},
{"CTaskComplexGoToCarDoorAndStandStill",0x86ec4c},
{"CTaskComplexGoToPointAiming",0x86fe00},
{"CTaskComplexGoToPointAndStandStill",0x86fda8},
{"CTaskComplexGoToPointAndStandStillAndAchieveHeading",0x86FE58},
{"CTaskComplexGoToPointAndStandStillTimed",0x86FDD4},
{"CTaskComplexGoToPointAnyMeans",0x86FF68},
{"CTaskComplexGoToPointShooting",0x86fe2c},
{"CTaskComplexGoToPointUntilCollisionAndStandStill",0x0},
{"CTaskComplexGotoDoorAndOpen",0x86FFC0},
{"CTaskComplexHitByGunResponse",0x86e06c},
{"CTaskComplexHitPedWithCar",0x86f294},
{"CTaskComplexHitResponse",0x86e040},
{"CTaskComplexInAirAndLand",0x8704AC},
{"CTaskComplexInWater",0x86e4ec},
{"CTaskComplexInvestigateDeadPed",0x870d3c},
{"CTaskComplexInvestigateDisturbance",0x870010},
{"CTaskComplexJump",0x870570},
{"CTaskComplexKillAllThreats",0x86DA1C},
{"CTaskComplexKillCriminal",0x870a00},
{"CTaskComplexKillPedFromBoat",0x86da98},
{"CTaskComplexKillPedGroupOnFoot",0x86da48},
{"CTaskComplexKillPedOnFoot",0x86D894},
{"CTaskComplexKillPedOnFootArmed",0x86d918},
{"CTaskComplexKillPedOnFootMelee",0x86d8ec},
{"CTaskComplexKillPedOnFootStealth",0x86d8c0},
{"CTaskComplexLeaveAnyCar",0x858EB0},
{"CTaskComplexLeaveBoat",0x86e854},
{"CTaskComplexLeaveCar",0x86E828},
{"CTaskComplexLeaveCarAndDie",0x86dd00},
{"CTaskComplexLeaveCarAndFlee",0x86e8ac},
{"CTaskComplexLeaveCarAndWander",0x86e880},
{"CTaskComplexLeaveCarAsPassengerWait",0x86e7a4},
{"CTaskComplexMedicTreatInjuredPed",0x86F518},
{"CTaskComplexMoveBackAndJump",0x86f390},
{"CTaskComplexObserveTrafficLights",0x86DF98},
{"CTaskComplexOnFire",0x86e308},
{"CTaskComplexOpenDriverDoor",0x86EB0C},
{"CTaskComplexOpenPassengerDoor",0x86EB3C},
{"CTaskComplexPartner",0x870664},
{"CTaskComplexPartnerChat",0x8707C4},
{"CTaskComplexPartnerDeal",0x870754},
{"CTaskComplexPartnerGreet",0x87078c},
{"CTaskComplexPartnerShove",0x870800},
{"CTaskComplexPassObject",0x86fb18},
{"CTaskComplexPlayHandSignalAnim",0x86d5dc},
{"CTaskComplexPolicePursuit",0x8709d4},
{"CTaskComplexPresentIDToCop",0x86F544},
{"CTaskComplexProstituteSolicit",0x86FB88},
{"CTaskComplexReactToAttack",0x86D868},
{"CTaskComplexReactToGunAimedAt",0x870d78},
{"CTaskComplexRoadRage",0x86d9f0},
{"CTaskComplexScreamInCarThenLeave",0x86e8d8},
{"CTaskComplexSeekCoverUntilTargetDead",0x86dac4},
{"CTaskComplexSeekEntityAiming",0x870c94},
{"CTaskComplexSeekEntityAnyMeansXYOffset",0x870CF0},
{"CTaskComplexSeekEntityRadiusAngleOffset",0x0085A388},
{"CTaskComplexSeekEntityStandard",0x00859DF8},
{"CTaskComplexSequence",0x86e200},
{"CTaskComplexShuffleSeats",0x86ea40},
{"CTaskComplexSignalAtPed",0x86fa8c},
{"CTaskComplexSitDownThenIdleThenStandUp",0x86df60},
{"CTaskComplexSmartFleeEntity",0x86f784},
{"CTaskComplexSmartFleePoint",0x86f744},
{"CTaskComplexStareAtPed",0x86fa2c},
{"CTaskComplexStealCar",0x86e7fc},
{"CTaskComplexStuckInAir",0x870608},
{"CTaskComplexSunbathe",0x86e0ac},
{"CTaskComplexTrackEntity",0x86F998},
{"CTaskComplexTreatAccident",0x86F4EC},
{"CTaskComplexTurnToFaceEntityOrCoord",0x86ff94},
{"CTaskComplexUseAttractor",0x86e134},
{"CTaskComplexUseAttractorPartner",0x86e2b0},
{"CTaskComplexUseClosestFreeScriptedAttractor",0x86e428},
{"CTaskComplexUseClosestFreeScriptedAttractorRun",0x85b904},
{"CTaskComplexUseClosestFreeScriptedAttractorSprint",0x85b930},
{"CTaskComplexUseEffect",0x86e0d8},
{"CTaskComplexUseEffectRunning",0x86e594},
{"CTaskComplexUseEffectSprinting",0x86e5c0},
{"CTaskComplexUseEntryExit",0x0},
{"CTaskComplexUseGoggles",0x86E4A8},
{"CTaskComplexUseMobilePhone",0x86E454},
{"CTaskComplexUsePairedAttractor",0x86e258},
{"CTaskComplexUseScriptedAttractor",0x86e2dc},
{"CTaskComplexUseScriptedBrain",0x86e568},
{"CTaskComplexUseSequence",0x86e518},
{"CTaskComplexUseSwatRope",0x86F59C},
{"CTaskComplexUseWaterCannon",0x86F5D0},
{"CTaskComplexWaitAtAttractor",0x86e108},
{"CTaskComplexWaitForBackup",0x870da4},
{"CTaskComplexWaitForBus",0x86e18c},
{"CTaskComplexWaitForDryWeather",0x86e160},
{"CTaskComplexWaitForPed",0x8706F8},
{"CTaskComplexWalkAlongsidePed",0x8706cc},
{"CTaskComplexWalkRoundBuildingAttempt",0x86f334},
{"CTaskComplexWalkRoundCar",0x86f308},
{"CTaskComplexWalkRoundFire",0x86f3e8},
{"CTaskComplexWalkRoundObject",0x86F364},
{"CTaskComplexWander",0x86FE84},
{"CTaskComplexWanderCop",0x859C80},
{"CTaskComplexWanderCriminal",0x85A23C},
{"CTaskComplexWanderFlee",0x86f690},
{"CTaskComplexWanderGang",0x8700D4},
{"CTaskComplexWanderMedic",0x86F48C},
{"CTaskComplexWanderProstitute",0x870148},
{"CTaskComplexWanderStandard",0x85A200},
{"CTaskGangHasslePed",0x86FA00},
{"CTaskGangHassleVehicle",0x86F9D4},
{"CTaskGoToVehicleAndLean",0x86FAC8},
{"CTaskInteriorBeInHouse",0x8701e8},
{"CTaskInteriorBeInOffice",0x870218},
{"CTaskInteriorBeInShop",0x870244},
{"CTaskInteriorGoToInfo",0x8702bc},
{"CTaskInteriorLieInBed",0x870338},
{"CTaskInteriorShopKeeper",0x870290},
{"CTaskInteriorSitAtDesk",0x87035c},
{"CTaskInteriorSitInChair",0x870314},
{"CTaskInteriorUseInfo",0x8702e8},
{"CTaskLeanOnVehicle",0x86FAF4},
{"CTaskSimpleAchieveHeading",0x86fd74},
{"CTaskSimpleAffectSecondaryBehaviour",0x870b08},
{"CTaskSimpleAnim",0x86D504},
{"CTaskSimpleArrestPed",0x870984},
{"CTaskSimpleBeHit",0x86D844},
{"CTaskSimpleBeHitWhileMoving",0x86D7FC},
{"CTaskSimpleBeKickedOnGround",0x86D7D8},
{"CTaskSimpleBikeJacked",0x86EFAC},
{"CTaskSimpleCarAlign",0x86EC78},
{"CTaskSimpleCarCloseDoorFromInside",0x86ed08},
{"CTaskSimpleCarCloseDoorFromOutside",0x86ed2c},
{"CTaskSimpleCarDrive",0x86E904},
{"CTaskSimpleCarDriveTimed",0x859E50},
{"CTaskSimpleCarFallOut",0x86efd0},
{"CTaskSimpleCarForcePedOut",0x86EE94},
{"CTaskSimpleCarGetIn",0x86ed50},
{"CTaskSimpleCarGetOut",0x86ee4c},
{"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse",0x86ede0},
{"CTaskSimpleCarJumpOut",0x86ee70},
{"CTaskSimpleCarOpenDoorFromOutside",0x86ec9c},
{"CTaskSimpleCarOpenLockedDoorFromOutside",0x86ecc0},
{"CTaskSimpleCarSetPedInAsDriver",0x86EE28},
{"CTaskSimpleCarSetPedInAsPassenger",0x86EE04},
{"CTaskSimpleCarSetPedOut",0x86EEB8},
{"CTaskSimpleCarSetPedSlowDraggedOut",0x86EF30},
{"CTaskSimpleCarSetTempAction",0x86ea6c},
{"CTaskSimpleCarShuffle",0x86ed74},
{"CTaskSimpleCarSlowBeDraggedOut",0x86ef0c},
{"CTaskSimpleCarSlowDragPedOut",0x86EEE8},
{"CTaskSimpleCarWaitForDoorNotToBeInUse",0x86edbc},
{"CTaskSimpleCarWaitToSlowDown",0x86ed98},
{"CTaskSimpleChat",0x86C78C},
{"CTaskSimpleChoking",0x86d820},
{"CTaskSimpleClearLookAt",0x86E404},
{"CTaskSimpleClimb",0x87059C},
{"CTaskSimpleCower",0x85a0d0},
{"CTaskSimpleCreateCarAndGetIn",0x86F070},
{"CTaskSimpleDead",0x86DEA4},
{"CTaskSimpleDetonate",0x0},
{"CTaskSimpleDie",0x86DDBC},
{"CTaskSimpleDieInCar",0x86DDE0},
{"CTaskSimpleDoHandSignal",0x86fa68},
{"CTaskSimpleDrown",0x86DE30},
{"CTaskSimpleDrownInCar",0x86DE54},
{"CTaskSimpleDuck",0x870bc4},
{"CTaskSimpleDuckToggle",0x86dd98},
{"CTaskSimpleDuckWhileShotsWhizzing",0x870BEC},
{"CTaskSimpleEvasiveDive",0x86F244},
{"CTaskSimpleEvasiveStep",0x86F1F4},
{"CTaskSimpleFacial",0x870ab8},
{"CTaskSimpleFall",0x870430},
{"CTaskSimpleFight",0x86D684},
{"CTaskSimpleFightingControl",0x86d700},
{"CTaskSimpleFinishBrain",0x859d20},
{"CTaskSimpleGangDriveBy",0x86D944},
{"CTaskSimpleGetUp",0x8703E0},
{"CTaskSimpleGiveCPR",0x86F4C8},
{"CTaskSimpleGoTo",0x86fd1c},
{"CTaskSimpleGoToPoint",0x86fd50},
{"CTaskSimpleGoToPointFine",0x86F964},
{"CTaskSimpleGunControl",0x86d76c},
{"CTaskSimpleHailTaxi",0x0},
{"CTaskSimpleHailTaxiAndPause",0x0},
{"CTaskSimpleHandsUp",0x85a29c},
{"CTaskSimpleHitByGunFromFront",0x86dc10},
{"CTaskSimpleHitByGunFromLeft",0x86dc88},
{"CTaskSimpleHitByGunFromRear",0x86dc4c},
{"CTaskSimpleHitByGunFromRight",0x86dcc4},
{"CTaskSimpleHitFromBack",0x86db70},
{"CTaskSimpleHitFromBehind",0x86f168},
{"CTaskSimpleHitFromFront",0x86e5ec},
{"CTaskSimpleHitFromLeft",0x86dba4},
{"CTaskSimpleHitFromRight",0x86dbd8},
{"CTaskSimpleHitHead",0x86F1D0},
{"CTaskSimpleHitWall",0x86F1A0},
{"CTaskSimpleHoldEntity",0x870B2C},
{"CTaskSimpleHurtPedWithCar",0x86f2e4},
{"CTaskSimpleIKChain",0x86DB48},
{"CTaskSimpleIKLookAt",0x86E37C},
{"CTaskSimpleIKManager",0x86E358},
{"CTaskSimpleIKPointArm",0x86E3A4},
{"CTaskSimpleInAir",0x8704D8},
{"CTaskSimpleJetPack",0x8705c4},
{"CTaskSimpleJump",0x87054C},
{"CTaskSimpleKillPedWithCar",0x86f2c0},
{"CTaskSimpleLand",0x8704FC},
{"CTaskSimpleLookAbout",0x0085A164},
{"CTaskSimpleNone",0x859CFC},
{"CTaskSimpleOnEscalator",0x85B8DC},
{"CTaskSimplePause",0x85A278},
{"CTaskSimplePickUpBike",0x86ece4},
{"CTaskSimplePickUpEntity",0x870b50},
{"CTaskSimplePlayHandSignalAnim",0x86D5B8},
{"CTaskSimplePlayerOnFire",0x86e334},
{"CTaskSimplePlayerOnFoot",0x8708ec},
{"CTaskSimplePutDownEntity",0x870b74},
{"CTaskSimpleRunAnim",0x86D528},
{"CTaskSimpleRunAnimLoopedMiddle",0x0},
{"CTaskSimpleRunNamedAnim",0x86D54C},
{"CTaskSimpleRunTimedAnim",0x86d570},
{"CTaskSimpleSay",0x85a1dc},
{"CTaskSimpleScratchHead",0x85A100},
{"CTaskSimpleSetCharDecisionMaker",0x859da0},
{"CTaskSimpleSetCharIgnoreWeaponRangeFlag",0x859ebc},
{"CTaskSimpleSetKindaStayInSamePlace",0x86dd74},
{"CTaskSimpleSetPedAsAutoDriver",0x86eff4},
{"CTaskSimpleSetStayInSamePlace",0x86DD50},
{"CTaskSimpleShakeFist",0x870a94},
{"CTaskSimpleSitDown",0x86def4},
{"CTaskSimpleSitIdle",0x86df18},
{"CTaskSimpleSlideToCoord",0x86FFEC},
{"CTaskSimpleStandStill",0x86DD2C},
{"CTaskSimpleStandUp",0x86df3c},
{"CTaskSimpleStealthKill",0x86da74},
{"CTaskSimpleSwim",0x870920},
{"CTaskSimpleThrowControl",0x86D7B4},
{"CTaskSimpleThrowProjectile",0x86d790},
{"CTaskSimpleTired",0x86DED0},
{"CTaskSimpleTogglePedThreatScanner",0x85A334},
{"CTaskSimpleTriggerEvent",0x0},
{"CTaskSimpleTriggerLookAt",0x86E3CC},
{"CTaskSimpleTurn180",0x86e01c},
{"CTaskSimpleUninterruptable",0x859580},
{"CTaskSimpleUseAtm",0x85a134},
{"CTaskSimpleUseGun",0x86D724},
{"CTaskSimpleWaitForBus",0x86e1b8},
{"CTaskSimpleWaitForPizza",0x86E1DC},
{"CTaskSimpleWaitUntilAreaCodesMatch",0x86e544},
{"CTaskSimpleWaitUntilLeaderAreaCodesMatch",0x86c7e8},
{"CTaskSimpleWaitUntilPedIsInCar",0x86f0b8},
{"CTaskSimpleWaitUntilPedIsOutCar",0x86F094}
    };
    static const auto dllTables=[] {
        std::array<uintptr_t,std::size(tables)> result{};
        for(size_t i=0;i<result.size();++i)result[i]=uintptr_t(ReversibleHooks::detail::GetVTableAddress(tables[i].name));
        return result;
    }();
    for(size_t i=0;i<std::size(tables);++i)if(value==tables[i].canonical||value==dllTables[i])return json::array({"vtable",tables[i].name});
    return Ptr(value);
}
inline json Words(const void* p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(i==0?VmtIdentity355(static_cast<const uint32*>(p)[i]):Ptr(static_cast<const uint32*>(p)[i]));return j;}
inline void* __cdecl New(size_t n){allocated=n;trace.push_back({"new",n});return arena.data();}
inline int __fastcall Type(CTask*,void*){trace.push_back({"type",subType});return subType;}
inline CPed* __cdecl Player(int id){trace.push_back({"player",id});return reinterpret_cast<CPed*>((scenario&2)?player.data():receiver.data());}
inline bool __fastcall IsPlayer(const CPed*,void*){bool b=(scenario&1)!=0;trace.push_back({"is_player",b});return b;}
inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=(scenario&1)!=0;trace.push_back({"abort",priority,b});return b;}
inline void __fastcall Ref(CEntity* p,void*,CEntity**){trace.push_back({"reference",Ptr(uintptr_t(p))});}
inline void __fastcall Clean(CEntity* p,void*,CEntity**){trace.push_back({"cleanup",Ptr(uintptr_t(p))});}
inline bool __cdecl Should(){bool b=mode!=7&&(scenario&2)!=0;trace.push_back({"should",b});return b;}
inline int __cdecl Block(const char* name){trace.push_back({"block",name});return 0;}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Remove(int index){trace.push_back({"remove_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
inline int __cdecl Random(){int r=(scenario*701)%32768;trace.push_back({"rand",r});return r;}
inline int __cdecl Range(int low,int high){int r=low+scenario%(high-low);if(low==0&&high==500)r=mode==10?51+scenario%5:mode==11?100:mode==13?200:201;if(low==0&&high==100)r=mode==9?96:mode==14?50:0;if(low==0&&high==30)r=20;if(low==0&&high==2000)r=500;trace.push_back({"range",low,high,r});return r;}
inline int __fastcall Count(CPedGroupMembership*,void*){trace.push_back({"count"});return 5;}
inline CPed* __fastcall Member(CPedGroupMembership*,void*,int index){trace.push_back({"member",index});return reinterpret_cast<CPed*>(index%2?ped.data():receiver.data());}
inline CPed* __fastcall Leader(CPedGroupMembership*,void*){trace.push_back({"leader"});return reinterpret_cast<CPed*>(player.data());}
inline void __fastcall RemoveMember(CPedGroupMembership*,void*,CPed*){trace.push_back({"remove_member"});}
inline void __fastcall SetPrimary(CTaskManager*,void*,CTask* t,int slot,bool extra){trace.push_back({"set_primary",Ptr(uintptr_t(t)),slot,extra});}
inline void __fastcall SetSecondary(CTaskManager*,void*,CTask* t,int slot){trace.push_back({"set_secondary",Ptr(uintptr_t(t)),slot});}
inline CTask* __fastcall Secondary(CTaskManager*,void*,int slot){trace.push_back({"secondary",slot});return (scenario&4)?reinterpret_cast<CTask*>(gotoTask.data()):nullptr;}
inline int16 __fastcall Say(CPed* speaker,void*,int context,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",Ptr(uintptr_t(speaker)),context,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline bool __fastcall Visible(CEntity*,void*){bool b=mode>=8||bool(scenario&8);trace.push_back({"visible",b});return b;}
inline bool __stdcall Looking(CPed*){bool b=(scenario&16)!=0;trace.push_back({"looking",b});return b;}
inline void __fastcall Look(IKChainManager_c*,void*,const char* label,CPed*,CEntity* target,int time,int bone,CVector*,bool torso,float speed,int blend,int priority,bool force){trace.push_back({"look",label,Ptr(uintptr_t(target)),time,bone,torso,std::bit_cast<uint32>(speed),blend,priority,force});}
inline void __fastcall AbortLook(IKChainManager_c*,void*,CPed*,uint32 time){trace.push_back({"abort_look",time});}
inline CAnimBlendAssociation* __cdecl Association(RpClump*,int anim){trace.push_back({"association",anim});if(anim==0&&mode>=3&&mode<=4)return reinterpret_cast<CAnimBlendAssociation*>(assoc.data());if(mode==12&&(anim==300||anim==302))return reinterpret_cast<CAnimBlendAssociation*>(assoc.data());return nullptr;}
inline int __fastcall Move(CPedIntelligence*,void*){int r=mode>=3&&mode<=4?4:1;trace.push_back({"move",r});return r;}
inline void __fastcall MoveSpeed(CPed*,void*,CAnimBlendAssociation* a){trace.push_back({"move_speed"});a->m_Speed=(scenario&32)?.9f:1.1f;}
inline CTask* __fastcall Find(CTaskManager*,void*,int type){trace.push_back({"find",type});if(type==900&&mode>=3&&mode<=4)return reinterpret_cast<CTask*>(gotoTask.data());if(type==0x133&&mode==14)return reinterpret_cast<CTask*>(gotoTask.data());return nullptr;}
inline void __fastcall UpdatePoint(CTaskSimpleGoToPoint* t,void*,const CVector& target,float radius,bool dont){trace.push_back({"update_point",std::bit_cast<uint32>(target.x),std::bit_cast<uint32>(target.y),std::bit_cast<uint32>(target.z),std::bit_cast<uint32>(radius),dont});t->m_vecTargetPoint=target;t->m_fRadius=radius;}
inline bool __fastcall Out(CTaskTimer*,void*){trace.push_back({"out"});return (scenario&1)!=0;}
inline FxSystem_c* __fastcall CreateFx(FxManager_c*,void*,const char* name,const CVector&,RwMatrix*,bool){trace.push_back({"fx",name});return reinterpret_cast<FxSystem_c*>(fx.data());}
inline void __fastcall AttachFx(FxSystem_c*,void*,CEntity*,int bone){trace.push_back({"fx_attach",bone});}
inline void __fastcall KillFx(FxSystem_c*,void*){trace.push_back({"fx_play_kill"});}
inline bool __fastcall HandSignal(CPed*,void*){bool b=(scenario&32)!=0;trace.push_back({"hand_signal",b});return b;}
inline CEntity* __fastcall Held(CPed* p,void*){trace.push_back({"held",Ptr(uintptr_t(p))});return p==reinterpret_cast<CPed*>(ped.data())&&mode>=12?reinterpret_cast<CEntity*>(held.data()):nullptr;}
inline CPed* __cdecl Pass(CPed*,CPedGroup*){trace.push_back({"pass_target"});return reinterpret_cast<CPed*>(receiver.data());}
inline CEvent* __fastcall EventAdd(CEventGroup*,void*,CEvent* e,bool){auto*p=static_cast<CEventPassObject*>(e);trace.push_back({"pass_event",Ptr(uintptr_t(p->m_giver)),p->m_dontPassObject});return nullptr;}
inline int __cdecl Ambient(CPed*,CEntity*){trace.push_back({"ambient"});return 0x117;}
inline void __fastcall HoldAnim(CTaskSimpleHoldEntity*,void*,int anim,int group){trace.push_back({"hold_anim",anim,group});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversingProbe::PumpMessages();ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"gang-follower355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"gang-follower355-runtime.jsonl");ReversingProbe::InstallFailureCapture("gang-follower355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"gang-follower355-only.txt");if(f)std::getline(f,only);}subVT[4]=reinterpret_cast<void*>(&Type);subVT[6]=reinterpret_cast<void*>(&Abort);
const auto savedTime=CTimer::GetTimeInMS();auto& block=CAnimManager::GetAnimBlocks()[0];const auto savedLoaded=block.IsLoaded;bool pass=true,selected=false;int total=0;
for(const auto& fn:fns){auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto*h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",full}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
EntryPatch pe(0x56E210,Player),pd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
EntryPatch ie(0x5DF8F0,IsPlayer),id(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch cle(0x571A00,Clean),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
EntryPatch she(0x65E7F0,Should),shd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),Should);
EntryPatch be(0x4D3990,Block),bd(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch rme(0x4D3FD0,Remove),rmd(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Remove);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
EntryPatch re(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch rge(0x407180,Range),rgd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CGeneral::GetRandomIntInRange)),Range);
EntryPatch ce(0x5F6A50,Count),cd(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembers),Count);
EntryPatch me(0x5F69B0,Member),md(bit_cast<uintptr_t>(&CPedGroupMembership::GetMember),Member);
EntryPatch mle(0x5F69A0,Leader),mld(bit_cast<uintptr_t>(&CPedGroupMembership::GetLeader),Leader);
EntryPatch mree(0x5FB210,RemoveMember),mred(bit_cast<uintptr_t>(static_cast<void(CPedGroupMembership::*)(CPed*)>(&CPedGroupMembership::RemoveMember)),RemoveMember);
EntryPatch pre(0x681AF0,SetPrimary),prd(bit_cast<uintptr_t>(&CTaskManager::SetTask),SetPrimary);
EntryPatch sse(0x681B60,SetSecondary),ssd(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),SetSecondary);
EntryPatch sce(0x681810,Secondary),scd(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
EntryPatch sae(0x5EFFE0,Say),sad(bit_cast<uintptr_t>(&CPed::Say),Say);
EntryPatch ve(0x536BC0,Visible),vd(bit_cast<uintptr_t>(&CEntity::IsVisible),Visible);
EntryPatch le(0x6181A0,Looking),ld(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),Looking);
EntryPatch loe(0x618970,Look),lod(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),Look);
EntryPatch ale(0x618280,AbortLook),ald(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLook);
EntryPatch ase(0x4D68B0,Association),asd(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),Association);
EntryPatch movee(0x601D70,Move),moved(bit_cast<uintptr_t>(&CPedIntelligence::GetMoveStateFromGoToTask),Move);
EntryPatch mse(0x5DEC10,MoveSpeed),msd(bit_cast<uintptr_t>(&CPed::SetMoveAnimSpeed),MoveSpeed);
EntryPatch fte(0x681740,Find),ftd(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),Find);
EntryPatch upe(0x645700,UpdatePoint),upd(bit_cast<uintptr_t>(&CTaskSimpleGoToPoint::UpdatePoint),UpdatePoint);
EntryPatch oe(0x420E30,Out),od(bit_cast<uintptr_t>(&CTaskTimer::IsOutOfTime),Out);
EntryPatch fxe(0x4A9BE0,CreateFx),fxd(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),CreateFx);
EntryPatch fae(0x4AA400,AttachFx),fad(bit_cast<uintptr_t>(&FxSystem_c::AttachToBone),AttachFx);
EntryPatch fke(0x4AA3D0,KillFx),fkd(bit_cast<uintptr_t>(&FxSystem_c::PlayAndKill),KillFx);
EntryPatch hse(0x5E0460,HandSignal),hsd(bit_cast<uintptr_t>(&CPed::IsPlayingHandSignal),HandSignal);
EntryPatch helde(0x5E02E0,Held),heldd(bit_cast<uintptr_t>(&CPed::GetEntityThatThisPedIsHolding),Held);
EntryPatch pae(0x65EA50,Pass),pad(bit_cast<uintptr_t>(&CTaskComplexGangLeader::TryToPassObject),Pass);
EntryPatch eve(0x4AB420,EventAdd),evd(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),EventAdd);
EntryPatch ame(0x65E730,Ambient),amd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::GetRandomGangAmbientAnim),Ambient);
EntryPatch hae(0x691720,HoldAnim),had(bit_cast<uintptr_t>(&CTaskSimpleHoldEntity::PlayAnim),HoldAnim);
for(int i=0;i<fn.cases;++i){scenario=i%64;mode=(i/64)%16;const int types[]{0x4BA,0xCB,0x1AA,0x2C5,0x38B,12345};subType=fn.slot==10?(mode==1?0x2C5:mode>=2&&mode<=5?0x38B:0xCB):types[(i/16)%6];json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
task.fill(0xCD);arena.fill(0xCD);ped.fill(0);player.fill(0);subtask.fill(0);allocated=0;trace=json::array();auto*t=reinterpret_cast<CTaskComplexGangFollower*>(task.data());auto*p=reinterpret_cast<CPed*>(ped.data());auto*q=reinterpret_cast<CPed*>(player.data());Put(task.data(),0,fn.vt);Put<uint32>(task.data(),4,0);Put(task.data(),8,subtask.data());Put(subtask.data(),0,subVT.data());vehicle.fill(0);matrix.fill(0);pedMatrix.fill(0);intel.fill(0);leaderIntel.fill(0);receiverIntel.fill(0);group.fill(0);receiver.fill(0);assoc.fill(0);held.fill(0);gotoTask.fill(0);clump.fill(0);frame.fill(0);fx.fill(0);auto*r=reinterpret_cast<CPed*>(receiver.data());Put(ped.data(),0x47C,intel.data());Put(player.data(),0x47C,leaderIntel.data());Put(receiver.data(),0x47C,receiverIntel.data());Put(ped.data(),0x14,pedMatrix.data());Put(player.data(),0x14,matrix.data());auto*m=reinterpret_cast<CMatrix*>(matrix.data());auto*pm=reinterpret_cast<CMatrix*>(pedMatrix.data());m->GetRight()=CVector{1.f,0.f,0.f};m->GetForward()=CVector{0.f,1.f,0.f};m->GetUp()=CVector{0.f,0.f,1.f};m->GetPosition()=CVector{0.f,mode==4?10.f:float(i%3),0.f};pm->GetRight()=m->GetRight();pm->GetForward()=m->GetForward();pm->GetUp()=m->GetUp();pm->GetPosition()=CVector{0.f,-1.f,0.f};auto*v=reinterpret_cast<CVehicle*>(vehicle.data());p->bInVehicle=(i&4)!=0;p->m_pVehicle=(i&8)?v:nullptr;q->m_pVehicle=(i&16)?v:nullptr;q->m_nMoveState=mode>=3&&mode<=4?PEDMOVE_WALK:static_cast<eMoveState>(i%8);p->m_nMoveState=PEDMOVE_STILL;Put<int32>(intel.data(),0x274,mode==5?9:(i&32)?31:0);t->m_PedGroup=reinterpret_cast<CPedGroup*>(group.data());t->m_PedGroup->m_bIsMissionGroup=(scenario&4)!=0;t->m_Leader=(fn.slot==10?mode==0:bool(i&128))?nullptr:q;t->m_PedPosn=CVector{0.f,0.f,0.f};t->dword20=CVector{.25f,-.5f,0.f};t->dword2C=t->dword20;t->dword38=3.f;t->byte3C=uint8(i%8);t->m_Flags=uint8(fn.slot==10?(mode>=2&&mode<=5?4:mode==6?0:mode==7?1:1):i%32);t->dword40=3000;t->dword44=2700;t->byte48=uint8(mode==8);t->byte49=0;auto*a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());a->m_Speed=1.f;a->m_CurrentTime=.4f;reinterpret_cast<CEntity*>(held.data())->m_nModelIndex=int16(eModelID(ModelIndices::MI_GANG_SMOKE));Put<float>(gotoTask.data(),0x18,1.f);Put<uint8>(clump.data(),0,2);Put(clump.data(),4,frame.data());Put(ped.data(),0x18,clump.data());if(fn.slot==-2)Put<uint32>(task.data(),8,0);if(fn.slot==6&&(i&4))Put<uint32>(task.data(),8,0);block.IsLoaded=(i&4)!=0;constructorPos=CVector{.25f,-.5f,1.f};CTimer::m_snTimeInMilliseconds=4000;

uintptr_t ret=0;if(fn.slot==-1)ret=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPedGroup*,CPed*,uint8,CVector,float)>(address)(t,reinterpret_cast<CPedGroup*>(group.data()),(i&1)?q:nullptr,uint8(i%8),constructorPos,float(i%6)));else if(fn.slot==-2)reinterpret_cast<void(__thiscall*)(void*)>(address)(t);else if(fn.slot==6)ret=uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(address)(t,p,i%3,nullptr));else if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));else ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));

if(candidate){coverage[ret?"nonnull":"null"]=coverage.value(ret?"nonnull":"null",0)+1;for(auto&e:trace){auto n=e[0].get<std::string>();coverage[n]=coverage.value(n,0)+1;}}
result[candidate]={{"return",fn.slot==6?json(ret):Ptr(ret)},{"association",Words(assoc.data(),assoc.size())},{"goto",Words(gotoTask.data(),128)},{"subtask",Words(subtask.data(),128)},{"task",Words(task.data(),task.size())},{"ped",Words(ped.data(),ped.size())},{"allocation",allocated?Words(arena.data(),allocated):json(nullptr)},{"trace",trace},{"switch",switches}};
}bool equal=result[0]==result[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total++},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}h->State(hs);h->LockState(hl);auto require=[&](const char*n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};if(fn.slot==8){require("set_primary");require("remove_member");require("member");require("new");}if(fn.slot==10){require("move_speed");require("update_point");require("fx");require("fx_attach");require("look");require("set_secondary");require("pass_event");require("hold_anim");}Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});}
CTimer::m_snTimeInMilliseconds=savedTime;block.IsLoaded=savedLoaded;Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",true}});
}
}
