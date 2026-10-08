#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "Pad.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "WeaponEffects.h"
#include "../ProbeRuntime.hpp"
namespace CamProcess355Live {
template<class T> T& At(void* p,size_t n){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n);}
// Lossless byte encoding: exactly two lowercase hexadecimal characters per byte.
inline json Bytes(const void* p,size_t n){
    static constexpr char digits[]="0123456789abcdef";
    std::string value(n*2,'\0');
    const auto* bytes=static_cast<const uint8*>(p);
    for(size_t i=0;i<n;++i){value[i*2]=digits[bytes[i]>>4];value[i*2+1]=digits[bytes[i]&15];}
    return value;
}
inline uint32 F(float x){return std::bit_cast<uint32>(x);}
inline json trace;inline uint32 scenario;inline CPed* fixturePed;inline CTaskSimpleClimb* fixtureClimb;
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return reinterpret_cast<CPlayerPed*>(fixturePed);}
inline CPad* __cdecl Pad(int32 id){return &CPad::Pads[id];}
inline void __fastcall Reference(CEntity*,void*,CEntity** p){trace.push_back({"reference",*p==fixturePed});}
inline void __cdecl Crosshair(int32 id){trace.push_back({"crosshair",id});}
inline CTaskSimpleClimb* __fastcall Climb(CPedIntelligence*,void*){trace.push_back({"climb"});return scenario%4==0?fixtureClimb:nullptr;}
inline void __fastcall ClimbTarget(CTaskSimpleClimb*,void*,CPed*,CVector& target){trace.push_back({"climbTarget",Bytes(&target,sizeof(target))});target.z+=.25f;}
inline void* __fastcall Fight(CPedIntelligence*,void*){trace.push_back({"fight"});return scenario%3==0?reinterpret_cast<void*>(1):nullptr;}
inline bool __fastcall Mouse(const CCam*,void*){trace.push_back({"mouse"});return scenario%7==0;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back({"appearance"});return static_cast<eVehicleAppearance>(1+scenario%5);}
inline bool __fastcall Behind(const CPad*,void*){trace.push_back({"behind"});return scenario%5==0;}
inline bool __fastcall Left(const CPad*,void*){trace.push_back({"left"});return scenario%5==1;}
inline bool __fastcall Right(const CPad*,void*){trace.push_back({"right"});return scenario%5==2;}
inline void Change(CCam* c,int id){trace.push_back({"mode",id});c->m_vecSource=CVector{float(id)*.125f,float(scenario%7)*.25f,2.f};c->m_vecTargetCoorsForFudgeInter=CVector{1,2,3};c->m_vecFront=CVector{0,1,0};c->m_vecUp=CVector{0,0,1};c->m_fFOV=65.f+float(scenario%13);}
template<int id> bool __fastcall Mode4(CCam* c,void*,const CVector& target,float heading,float speed,float wanted){trace.push_back({"modeArgs",id,Bytes(&target,sizeof(target)),F(heading),F(speed),F(wanted)});Change(c,id);return true;}
template<int id> void __fastcall Mode5(CCam* c,void*,const CVector& target,float heading,float speed,float wanted,bool flag){trace.push_back({"modeFlag",flag});Mode4<id>(c,nullptr,target,heading,speed,wanted);}
template<int id> bool __fastcall Mode0(CCam* c,void*){Change(c,id);return true;}
template<int id> bool __fastcall ModeBool(CCam* c,void*,bool flag){trace.push_back({"cineFlag",flag});Change(c,id);return true;}
inline void __fastcall Track(CCam* c,void*,const CVector& source,const CVector& target,const CVector& up,const float& alpha,const float& beta,const float& fov){trace.push_back({"track",Bytes(&source,sizeof(source)),Bytes(&target,sizeof(target)),Bytes(&up,sizeof(up)),F(alpha),F(beta),F(fov)});c->m_vecSourceSpeedOverOneFrame=CVector{.125f,.25f,.5f};}
inline bool __fastcall LookBehind(CCam* c,void*){trace.push_back({"lookBehind"});c->m_bLookingBehind=true;return true;}
inline bool __fastcall LookRight(CCam* c,void*,bool right){trace.push_back({"lookRight",right});c->m_bLookingRight=right;c->m_bLookingLeft=!right;return true;}
inline void __fastcall Vectors(CCam* c,void*){trace.push_back({"vectors",Bytes(&c->m_vecFront,sizeof(c->m_vecFront))});c->m_vecUp=CVector{0,0,1};}
inline bool done;inline std::ofstream log;
inline void Write(const json& v){ReversingProbe::PumpMessages();log<<v.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process",0x526FC0,0,-1,8192}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CCam")for(auto& h:c.Items())if(h->Name()=="Process")result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto directory=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(directory/"cam-process355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(directory/"cam-process355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-process355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p0(0x56E210,Player),p1(bit_cast<uintptr_t>(&FindPlayerPed),Player),pad(0x53FB70,Pad);
 EntryPatch ref0(0x571B70,Reference),ref1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference),cross0(0x742CA0,Crosshair),cross1(bit_cast<uintptr_t>(&CWeaponEffects::ClearCrossHairImmediately),Crosshair);
 EntryPatch climb0(0x601180,Climb),climb1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskClimb),Climb),ct0(0x67A390,ClimbTarget),ct1(bit_cast<uintptr_t>(&CTaskSimpleClimb::GetCameraTargetPos),ClimbTarget);
 EntryPatch fight0(0x600F30,Fight),fight1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskFighting),Fight),mouse0(0x50A850,Mouse),mouse1(bit_cast<uintptr_t>(&CCam::Using3rdPersonMouseCam),Mouse);
 EntryPatch ap0(0x6D1080,Appearance),ap1(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance),be0(0x53FE70,Behind),be1(bit_cast<uintptr_t>(&CPad::GetLookBehindForCar),Behind),le0(0x53FDD0,Left),le1(bit_cast<uintptr_t>(&CPad::GetLookLeft),Left),ri0(0x53FE10,Right),ri1(bit_cast<uintptr_t>(&CPad::GetLookRight),Right);
 EntryPatch tr0(0x509DF0,Track),tr1(bit_cast<uintptr_t>(&CCam::KeepTrackOfTheSpeed),Track),lb0(0x520690,LookBehind),lb1(bit_cast<uintptr_t>(&CCam::LookBehind),LookBehind),lr0(0x520E40,LookRight),lr1(bit_cast<uintptr_t>(&CCam::LookRight),LookRight),v0(0x509CE0,Vectors),v1(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Vectors);
 EntryPatch mode0a(0x5245B0,Mode5<3>),mode0b(bit_cast<uintptr_t>(&CCam::Process_FollowCar_SA),Mode5<3>);
 EntryPatch mode1a(0x522D40,Mode5<4>),mode1b(bit_cast<uintptr_t>(&CCam::Process_FollowPed_SA),Mode5<4>);
 EntryPatch mode2a(0x50F970,Mode4<104>),mode2b(bit_cast<uintptr_t>(&CCam::Process_FollowPedWithMouse),Mode4<104>);
 EntryPatch mode3a(0x5105C0,Mode4<7>),mode3b(bit_cast<uintptr_t>(&CCam::Process_M16_1stPerson),Mode4<7>);
 EntryPatch mode4a(0x511B50,Mode5<8>),mode4b(bit_cast<uintptr_t>(&CCam::Process_Rocket),Mode5<8>);
 EntryPatch mode5a(0x512110,Mode4<14>),mode5b(bit_cast<uintptr_t>(&CCam::Process_WheelCam),Mode4<14>);
 EntryPatch mode6a(0x51D470,Mode4<15>),mode6b(bit_cast<uintptr_t>(&CCam::Process_Fixed),Mode4<15>);
 EntryPatch mode7a(0x517EA0,Mode4<16>),mode7b(bit_cast<uintptr_t>(&CCam::Process_1stPerson),Mode4<16>);
 EntryPatch mode8a(0x5B25F0,Mode4<17>),mode8b(bit_cast<uintptr_t>(&CCam::Process_FlyBy),Mode4<17>);
 EntryPatch mode9a(0x519250,Mode0<29>),mode9b(bit_cast<uintptr_t>(&CCam::ProcessPedsDeadBaby),Mode0<29>);
 EntryPatch mode10a(0x518500,Mode0<32>),mode10b(bit_cast<uintptr_t>(&CCam::ProcessArrestCamOne),Mode0<32>);
 EntryPatch mode11a(0x517500,Mode4<35>),mode11b(bit_cast<uintptr_t>(&CCam::Process_SpecialFixedForSyphon),Mode4<35>);
 EntryPatch mode12a(0x50EB70,Mode4<39>),mode12b(bit_cast<uintptr_t>(&CCam::Process_1rstPersonPedOnPC),Mode4<39>);
 EntryPatch mode13a(0x50F3F0,Mode4<44>),mode13b(bit_cast<uintptr_t>(&CCam::Process_Editor),Mode4<44>);
 EntryPatch mode14a(0x512B10,Mode0<47>),mode14b(bit_cast<uintptr_t>(&CCam::Process_AttachedCam),Mode0<47>);
 EntryPatch mode15a(0x525E50,Mode0<48>),mode15b(bit_cast<uintptr_t>(&CCam::Process_Cam_TwoPlayer),Mode0<48>);
 EntryPatch mode16a(0x519810,Mode0<49>),mode16b(bit_cast<uintptr_t>(&CCam::Process_Cam_TwoPlayer_InCarAndShooting),Mode0<49>);
 EntryPatch mode17a(0x513510,Mode0<50>),mode17b(bit_cast<uintptr_t>(&CCam::Process_Cam_TwoPlayer_Separate_Cars),Mode0<50>);
 EntryPatch mode18a(0x521500,Mode4<53>),mode18b(bit_cast<uintptr_t>(&CCam::Process_AimWeapon),Mode4<53>);
 EntryPatch mode19a(0x513BE0,Mode0<54>),mode19b(bit_cast<uintptr_t>(&CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown),Mode0<54>);
 EntryPatch mode20a(0x51A740,ModeBool<56>),mode20b(bit_cast<uintptr_t>(&CCam::Process_DW_HeliChaseCam),ModeBool<56>);
 EntryPatch mode21a(0x51B120,ModeBool<57>),mode21b(bit_cast<uintptr_t>(&CCam::Process_DW_CamManCam),ModeBool<57>);
 EntryPatch mode22a(0x51B850,ModeBool<58>),mode22b(bit_cast<uintptr_t>(&CCam::Process_DW_BirdyCam),ModeBool<58>);
 EntryPatch mode23a(0x51C250,ModeBool<59>),mode23b(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneSpotterCam),ModeBool<59>);
 EntryPatch mode24a(0x51C760,ModeBool<62>),mode24b(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneCam1),ModeBool<62>);
 EntryPatch mode25a(0x51CC30,ModeBool<63>),mode25b(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneCam2),ModeBool<63>);
 EntryPatch mode26a(0x51D100,ModeBool<64>),mode26b(bit_cast<uintptr_t>(&CCam::Process_DW_PlaneCam3),ModeBool<64>);
 struct Region{uintptr_t address;size_t count;std::vector<uint8> saved;};
 std::array<Region,12> regions{{{0xB6F028,sizeof(CCamera)},{0xB6EC7C,12},{0x8CCC3C,12},{0x8CC384,32},{0x8CC488,4},{0x8CCB84,4},{0x8D5109,1},{0xC0B184,1},{0xC8A860,1},{0xB6EC20,32},{0x8CCF00,1},{0xB7CB5C,4}}};for(auto& r:regions){r.saved.resize(r.count);std::memcpy(r.saved.data(),reinterpret_cast<void*>(r.address),r.count);}
 std::array<uint8,sizeof(CPad)*2> savedPads{};std::memcpy(savedPads.data(),CPad::Pads,savedPads.size());
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<8192;++test){auto run=[&](bool candidate){
 hook->State(candidate);scenario=test;trace=json::array();for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);std::memset(CPad::Pads,0,sizeof(CPad)*2);
 alignas(CCam) std::array<uint8,sizeof(CCam)> localCam{};const uint8 cameraIndex=uint8((test/256)%3);StaticRef<uint8>(0xB6F081)=cameraIndex;
 auto* c=(test&128)?reinterpret_cast<CCam*>(0xB6F19C+uintptr_t(cameraIndex)*sizeof(CCam)):reinterpret_cast<CCam*>(localCam.data());std::memset(c,0,sizeof(CCam));
 alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,2> pedBytes{};fixturePed=reinterpret_cast<CPed*>(pedBytes[0].data());auto* other=reinterpret_cast<CPed*>(pedBytes[1].data());
 alignas(CPedIntelligence) std::array<uint8,sizeof(CPedIntelligence)> intelligenceBytes{};At<CPedIntelligence*>(fixturePed,0x47C)=reinterpret_cast<CPedIntelligence*>(intelligenceBytes.data());
 alignas(CAutomobile) std::array<uint8,sizeof(CAutomobile)> vehicleBytes{};auto* vehicle=reinterpret_cast<CVehicle*>(vehicleBytes.data());At<uint8>(vehicle,0x36)=2;At<int16>(vehicle,0x22)=(test&512)?520:400;fixturePed->m_pVehicle=(test&256)?vehicle:nullptr;
 std::array<CMatrix,3> matrices{};std::memset(matrices.data(),0,sizeof(matrices));std::array<CEntity*,3> entities{fixturePed,other,vehicle};
 for(size_t n=0;n<3;++n){At<CMatrix*>(entities[n],0x14)=&matrices[n];matrices[n].GetRight()=CVector{1,0,0};matrices[n].GetForward()=(test%11==0)?CVector{0,0,1}:CVector{.125f,1,0};matrices[n].GetUp()=CVector{0,0,1};matrices[n].GetPosition()=CVector{1.f+float(test%5)*.25f,2.f+float((test/5)%5)*.25f,3};}
 At<uint8>(fixturePed,0x36)=At<uint8>(other,0x36)=3;vehicle->m_vecMoveSpeed=CVector{float(int32(test%9)-4)*.125f,float(int32((test/9)%9)-4)*.125f,0};
 alignas(CTaskSimpleClimb) std::array<uint8,sizeof(CTaskSimpleClimb)> climbBytes{};fixtureClimb=reinterpret_cast<CTaskSimpleClimb*>(climbBytes.data());
 StaticRef<CEntity*>(0xB6F980)=fixturePed;const int entityCase=(test/68)%8;c->m_pCamTargetEntity=entityCase==0?nullptr:entityCase<4?static_cast<CEntity*>(fixturePed):entityCase<6?static_cast<CEntity*>(other):static_cast<CEntity*>(vehicle);
 c->m_nMode=static_cast<eCamMode>(test%68);c->m_bCamLookingAtVector=(test&64)!=0;c->m_vecCamFixedModeVector=CVector{2,3,4};c->m_nFrameNumWereAt=uint32((test/3)%7);c->m_nDoCollisionCheckEveryNumOfFrames=5;c->m_nDoCollisionChecksOnFrameNum=uint32((test/7)%5+1);c->m_nDirectionWasLooking=uint32(test%4);c->m_fSpeedVar=.25f;c->m_fFOV=70.f;c->m_vecSource=CVector{2,3,4};c->m_vecTargetCoorsForFudgeInter=CVector{1,2,3};c->m_vecUp=CVector{0,0,1};
 auto* active=reinterpret_cast<CCam*>(0xB6F19C+uintptr_t(cameraIndex)*sizeof(CCam));if(active!=c)active->m_nMode=static_cast<eCamMode>(test%3==0?4:16);
 StaticRef<float>(0xB7CB5C)=test%13==0?.125f:float(1+test%5)*.25f;StaticRef<int32>(0xB6FE34)=int32(test%7);StaticRef<float>(0xB6FDC8)=float(test%5);StaticRef<uint8>(0xC0B184)=0xA5;StaticRef<uint8>(0xC8A860)=(test&32)?1:0;
 StaticRef<CVector>(0x8CCC3C)=(test&512)?CVector{-5,-7,1}:CVector{1.5f,2.5f,3};StaticRef<CVector>(0xB6EC7C)=CVector{.125f,-.0625f,0};StaticRef<int32>(0x8CC384)=int32((test/4)%4);
 StaticRef<bool>(0xB6F042)=(test&256)!=0;StaticRef<bool>(0xB6F043)=(test&512)!=0;StaticRef<bool>(0xB6F052)=(test&1024)!=0;StaticRef<bool>(0xB6F080)=(test&2048)!=0;StaticRef<bool>(0xB6EC2E)=(test&16)!=0;StaticRef<bool>(0x8CCF00)=(test&32)!=0;
 StaticRef<bool>(0xB6FCDD)=(test&512)!=0;StaticRef<float>(0xB6FCE0)=float(45+test%35);StaticRef<bool>(0xB6FD14)=(test&1024)!=0;StaticRef<CVector>(0xB6FD08)=CVector{4,5,6};StaticRef<bool>(0xB6FCB4)=(test&2048)!=0;StaticRef<CVector>(0xB6FCA8)=CVector{7,8,9};CPad::Pads[0].NewState.ShockButtonR=(test&32)?255:0;
 const bool switched=ReversingProbe::JumpsToDll(0x526FC0)==candidate;reinterpret_cast<void(__thiscall*)(CCam*)>(0x526FC0)(c);
 json globals=json::array();for(auto& r:regions)globals.push_back(Bytes(reinterpret_cast<void*>(r.address),r.count));return json{{"cam",Bytes(c,sizeof(*c))},{"peds",Bytes(pedBytes.data(),sizeof(pedBytes))},{"intelligence",Bytes(intelligenceBytes.data(),intelligenceBytes.size())},{"vehicle",Bytes(vehicleBytes.data(),vehicleBytes.size())},{"matrices",Bytes(matrices.data(),sizeof(matrices))},{"climb",Bytes(climbBytes.data(),climbBytes.size())},{"pads",Bytes(CPad::Pads,sizeof(CPad)*2)},{"globals",globals},{"trace",trace},{"switches",switched}};
 };auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true;failures+=!equal;Write({{"event","comparison"},{"pair",test},{"function","CCam::Process"},{"case",test},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
 hook->State(enabled);hook->LockState(locked);for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);std::memcpy(CPad::Pads,savedPads.data(),savedPads.size());
 Write({{"event","function_summary"},{"function","CCam::Process"},{"cases",8192},{"failures",failures}});Write({{"event",failures?"error":"complete"},{"pairs",8192},{"state_restored",true}});
}
}

