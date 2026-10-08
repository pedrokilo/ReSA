#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskGangHassleVehicle.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleFight.h"
#include "Ragdoll/IKChainManager.h"
#include "../ProbeRuntime.hpp"
namespace HassleVehicle355Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskGangHassleVehicle","GetTargetHeading",0x65FDD0,0,-1,512},
{"CTaskGangHassleVehicle","CalcTargetOffset",0x6641A0,0,-1,512},
{"CTaskGangHassleVehicle","Clone",0x65FC00,0x86F9D4,1,256},
{"CTaskGangHassleVehicle","CreateNextSubTask",0x65FC80,0x86F9D4,8,1024},
{"CTaskGangHassleVehicle","CreateFirstSubTask",0x664BA0,0x86F9D4,9,1024},
{"CTaskGangHassleVehicle","ControlSubTask",0x6637C0,0x86F9D4,10,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped,player;
alignas(16) inline std::array<uint8,sizeof(CTaskGangHassleVehicle)> task;
alignas(16) inline std::array<uint8,128> arena,subtask;
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted;
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehicle;
alignas(16) inline std::array<uint8,sizeof(CMatrix)> matrix,pedMatrix;alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel;alignas(16) inline std::array<uint8,sizeof(CBaseModelInfo)> model;alignas(16) inline std::array<uint8,sizeof(CColModel)> col;alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;inline std::array<void*,11> subVT{};inline int mode; inline int subType{},scenario{};inline size_t allocated{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Ptr(uintptr_t p){if(!p)return nullptr;
#define P(x) if(p>=uintptr_t(x.data())&&p<uintptr_t(x.data())+x.size())return json::array({#x,p-uintptr_t(x.data())});
P(ped) P(player) P(task) P(arena) P(subtask) P(wanted) P(vehicle) P(matrix) P(pedMatrix) P(intel) P(model) P(col) P(assoc)
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
inline CPed* __cdecl Player(int id){trace.push_back({"player",id});return reinterpret_cast<CPed*>(player.data());}
inline bool __fastcall IsPlayer(const CPed*,void*){bool b=(scenario&1)!=0;trace.push_back({"is_player",b});return b;}
inline bool __fastcall Abort(CTask*,void*,CPed*,int priority,const CEvent*){bool b=mode!=1&&(scenario&1)!=0;trace.push_back({"abort",priority,b});return b;}
inline void __fastcall Ref(CEntity* p,void*,CEntity**){trace.push_back({"reference",Ptr(uintptr_t(p))});}
inline void __fastcall Clean(CEntity* p,void*,CEntity**){trace.push_back({"cleanup",Ptr(uintptr_t(p))});}
inline bool __cdecl Should(){bool b=mode!=15&&(scenario&2)!=0;trace.push_back({"should",b});return b;}
inline int __cdecl Block(const char* name){trace.push_back({"block",name});return 0;}
inline void __cdecl Add(int index){trace.push_back({"add_ref",index});}
inline void __cdecl Remove(int index){trace.push_back({"remove_ref",index});}
inline void __cdecl Request(int index,int flags){trace.push_back({"request",index,flags});}
inline int __cdecl Random(){int r=(scenario*701)%32768;trace.push_back({"rand",r});return r;}
inline int __cdecl Range(int low,int high){int r=low+(scenario%(high-low));if(low==0&&high==100)r=(scenario&4)?61:60;trace.push_back({"range",low,high,r});return r;}
inline int __fastcall Spare(const CVehicle*,void*){int r=scenario%8==7?-1:scenario%6;trace.push_back({"spare",r});return r;}
inline void __fastcall SetHassle(CVehicle*,void*,int pos,bool enable){trace.push_back({"reserve",pos,enable});}
inline void __fastcall Stats(CPed*,void*,int stats){trace.push_back({"stats",stats});}
inline CTask* __fastcall Next(CTaskGangHassleVehicle*,void*,CPed*){trace.push_back({"next"});return reinterpret_cast<CTask*>(arena.data());}
inline bool __fastcall Visible(CEntity*,void*){bool b=(scenario&8)!=0;trace.push_back({"visible",b});return b;}
inline bool __stdcall Looking(CPed*){bool b=(scenario&16)!=0;trace.push_back({"looking",b});return b;}
inline void __fastcall Look(IKChainManager_c*,void*,const char* label,CPed*,CEntity* target,int time,int bone,CVector*,bool torso,float speed,int blend,int priority,bool force){trace.push_back({"look",label,Ptr(uintptr_t(target)),time,bone,torso,std::bit_cast<uint32>(speed),blend,priority,force});}
inline CTask* __fastcall Secondary(CTaskManager*,void*,int slot){trace.push_back({"secondary",slot});return mode>=8&&mode<=11?reinterpret_cast<CTask*>(player.data()):nullptr;}
inline void __fastcall SetSecondary(CTaskManager*,void*,CTask* t,int slot){trace.push_back({"set_secondary",Ptr(uintptr_t(t)),slot});}
inline int16 __fastcall Say(CPed*,void*,int context,uint32 delay,float probability,bool a,bool b,bool c){trace.push_back({"say",context,delay,std::bit_cast<uint32>(probability),a,b,c});return 0;}
inline bool __fastcall HandSignal(CPed*,void*){bool b=(scenario&32)!=0;trace.push_back({"hand_signal",b});return b;}
inline float __fastcall Heading(CPlaceable*,void*){trace.push_back({"heading"});return 0.f;}
inline CAnimBlendAssociation* __cdecl Association(RpClump*,int anim){trace.push_back({"association",anim});return mode>=9&&mode<=11&&anim==0x128+(mode-9)?reinterpret_cast<CAnimBlendAssociation*>(assoc.data()):nullptr;}
inline void __fastcall Force(CPhysical*,void*,CVector a,CVector b){trace.push_back({"force",std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z)});}
inline void __fastcall FightSetup(CTaskSimpleFight*,void*,float r){trace.push_back({"fight_setup",std::bit_cast<uint32>(r)});}
inline int __cdecl Collision(const CMatrix&,CColModel&,const CMatrix&,CColModel&,std::array<CColPoint,32>&,CColPoint*,float*,bool){trace.push_back({"collision"});return (scenario&1)!=0?1:0;}
inline void __fastcall FightHit(CTaskSimpleFight*,void*,CPed*,CVehicle*,CVector*,CVector*,int16 a,int8 b){trace.push_back({"fight_hit",a,b});}
inline void __fastcall Audio(CAEVehicleAudioEntity*,void*,int event,float volume){trace.push_back({"audio",event,std::bit_cast<uint32>(volume)});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversingProbe::PumpMessages();ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"hassle-vehicle355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"hassle-vehicle355-runtime.jsonl");ReversingProbe::InstallFailureCapture("hassle-vehicle355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"hassle-vehicle355-only.txt");if(f)std::getline(f,only);}subVT[4]=reinterpret_cast<void*>(&Type);subVT[6]=reinterpret_cast<void*>(&Abort);
const auto savedTime=CTimer::GetTimeInMS();auto*savedModel=CModelInfo::GetModelInfo(100);auto& block=CAnimManager::GetAnimBlocks()[0];const auto savedLoaded=block.IsLoaded;bool pass=true,selected=false;int total=0;
for(const auto& fn:fns){auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto*h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",full}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ne(0x61A5A0,New),nd(bit_cast<uintptr_t>(&CTask::operator new),New);
EntryPatch refe(0x571B70,Ref),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
EntryPatch cle(0x571A00,Clean),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
EntryPatch she(0x65E7F0,Should),shd(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),Should);
EntryPatch be(0x4D3990,Block),bd(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),Block);
EntryPatch ae(0x4D3FB0,Add),ad(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),Add);
EntryPatch rme(0x4D3FD0,Remove),rmd(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),Remove);
EntryPatch qe(0x4087E0,Request),qd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
EntryPatch re(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch rge(0x407180,Range),rgd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CGeneral::GetRandomIntInRange)),Range);
EntryPatch spe(0x6D3AE0,Spare),spd(bit_cast<uintptr_t>(&CVehicle::GetSpareHasslePosId),Spare);
EntryPatch ste(0x6D3B30,SetHassle),std_(bit_cast<uintptr_t>(&CVehicle::SetHasslePosId),SetHassle);
EntryPatch sate(0x5E0360,Stats),satd(bit_cast<uintptr_t>(&CPed::SetPedStats),Stats);
EntryPatch ve(0x536BC0,Visible),vd(bit_cast<uintptr_t>(&CEntity::IsVisible),Visible);
EntryPatch le(0x6181A0,Looking),ld(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),Looking);
EntryPatch loe(0x618970,Look),lod(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),Look);
EntryPatch sce(0x681810,Secondary),scd(bit_cast<uintptr_t>(&CTaskManager::GetTaskSecondary),Secondary);
EntryPatch sse(0x681B60,SetSecondary),ssd(bit_cast<uintptr_t>(&CTaskManager::SetTaskSecondary),SetSecondary);
EntryPatch sae(0x5EFFE0,Say),sad(bit_cast<uintptr_t>(&CPed::Say),Say);
EntryPatch hse(0x5E0460,HandSignal),hsd(bit_cast<uintptr_t>(&CPed::IsPlayingHandSignal),HandSignal);
EntryPatch he(0x441DB0,Heading),hd(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
EntryPatch ase(0x4D68B0,Association),asd(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),Association);
EntryPatch fe(0x542A50,Force),fd(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),Force);
EntryPatch fse(0x61D5F0,FightSetup),fhe(0x61D0B0,FightHit);
EntryPatch ce(0x4185C0,Collision),cd(bit_cast<uintptr_t>(&CCollision::ProcessColModels),Collision);
EntryPatch aue(0x4F6420,Audio),aud(bit_cast<uintptr_t>(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),Audio);
std::optional<EntryPatch> nextE,nextD;if(fn.slot==9||fn.slot==10){nextE.emplace(0x65FC80,Next);nextD.emplace(bit_cast<uintptr_t>(&CTaskGangHassleVehicle::CreateNextSubTask),Next);}
for(int i=0;i<fn.cases;++i){scenario=i%64;mode=(i/64)%16;const int types[]{0x38F,0x4BC,0x39D,0x38A,1000,12345};subType=fn.slot==10?(mode==13?12345:0x39D):types[(i/16)%6];json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);auto address=fn.slot<0?fn.address:*reinterpret_cast<uintptr_t*>(fn.vt+4*fn.slot);bool switches=fn.slot<0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(address<0x400000||address>=0x1000000)==(candidate!=0);pass&=switches;
task.fill(0xCD);arena.fill(0xCD);ped.fill(0);player.fill(0);subtask.fill(0);allocated=0;trace=json::array();auto*t=reinterpret_cast<CTaskGangHassleVehicle*>(task.data());auto*p=reinterpret_cast<CPed*>(ped.data());auto*q=reinterpret_cast<CPed*>(player.data());Put(task.data(),0,fn.vt);Put<uint32>(task.data(),4,0);Put(task.data(),8,subtask.data());Put(subtask.data(),0,subVT.data());vehicle.fill(0);matrix.fill(0);pedMatrix.fill(0);intel.fill(0);model.fill(0);col.fill(0);assoc.fill(0);auto*v=reinterpret_cast<CVehicle*>(vehicle.data());Put(vehicle.data(),0x14,matrix.data());Put(ped.data(),0x14,pedMatrix.data());auto*m=reinterpret_cast<CMatrix*>(matrix.data());auto*pm=reinterpret_cast<CMatrix*>(pedMatrix.data());m->GetRight()=CVector{1.f,0.f,0.f};m->GetForward()=CVector{0.f,1.f,0.f};m->GetUp()=CVector{0.f,0.f,1.f};m->GetPosition()=CVector{0.f,0.f,0.f};pm->GetRight()=m->GetRight();pm->GetForward()=m->GetForward();pm->GetUp()=m->GetUp();pm->GetPosition()=CVector{0.f,0.f,0.f};Put(ped.data(),0x47C,intel.data());p->m_nMoveState=static_cast<eMoveState>(i%8);v->m_nModelIndex=100;Put(model.data(),0x14,col.data());CModelInfo::SetModelInfo(100,reinterpret_cast<CBaseModelInfo*>(model.data()));auto&box=reinterpret_cast<CColModel*>(col.data())->m_boundBox;box.m_vecMin=CVector{-1.f,-2.f,-.5f};box.m_vecMax=CVector{(i&128)?4.f:1.f,(i&256)?7.f:2.f,1.f};t->m_Vehicle=(fn.slot==10?(mode<2):bool(i&512))?nullptr:v;t->m_nHasslePosId=i%8-1;t->byte18=uint8(mode==2||bool(i&1));t->dword1C=2.f;t->m_fOffsetX=float(i%5)/4.f;t->m_vecPosn=CVector{1.f,-.5f,0.f};t->m_bRemoveAnim=mode==15||bool(i&2);t->m_b31=uint8(mode==5||mode==12||mode==13?0:mode==6?1:2);t->m_pEntity=mode==3?static_cast<CEntity*>(q):nullptr;t->m_nTime=3000;t->dword3C=mode==2?0:100000;t->byte40=mode==2;t->byte41=uint8(i&32);v->m_pDriver=mode==3?nullptr:q;v->m_fHealth=mode==4?249.f:float(249+i%3);if(fn.slot!=10)v->m_pDriver=(i&2)?q:nullptr;v->m_fMass=1000.f;p->bInVehicle=(i&4)!=0;p->m_pVehicle=(i&8)?v:nullptr;Put<float>(subtask.data(),0x50,mode==12?1.f:float(i%9));block.IsLoaded=(i&4)!=0;auto*a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());a->m_CurrentTime=(i&8)?.8f:.4f;a->m_TimeStep=.4f;CTimer::m_snTimeInMilliseconds=4000;

uintptr_t ret=0;json value=nullptr;if(std::string(fn.name)=="GetTargetHeading"){float r=reinterpret_cast<float(__thiscall*)(void*,CPed*)>(address)(t,p);value=std::bit_cast<uint32>(r);}else if(std::string(fn.name)=="CalcTargetOffset")reinterpret_cast<void(__thiscall*)(void*)>(address)(t);else if(fn.slot==1)ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(address)(t));else ret=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(t,p));

if(candidate){coverage[ret?"nonnull":"null"]=coverage.value(ret?"nonnull":"null",0)+1;for(auto&e:trace){auto n=e[0].get<std::string>();coverage[n]=coverage.value(n,0)+1;}}
result[candidate]={{"return",fn.slot==4?json(ret):Ptr(ret)},{"value",value},{"vehicle",Words(vehicle.data(),vehicle.size())},{"subtask",Words(subtask.data(),128)},{"task",Words(task.data(),task.size())},{"ped",Words(ped.data(),ped.size())},{"allocation",allocated?Words(arena.data(),allocated):json(nullptr)},{"trace",trace},{"switch",switches}};
}bool equal=result[0]==result[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total++},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}h->State(hs);h->LockState(hl);auto require=[&](const char*n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};if(fn.slot==9){require("spare");require("reserve");require("stats");require("next");}if(fn.slot==10){require("say");require("set_secondary");require("look");require("force");require("collision");require("fight_hit");require("audio");require("new");}Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});}
CTimer::m_snTimeInMilliseconds=savedTime;CModelInfo::SetModelInfo(100,savedModel);block.IsLoaded=savedLoaded;Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",true}});
}
}
