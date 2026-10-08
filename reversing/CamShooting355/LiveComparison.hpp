#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Collision/ColModel.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Pad.h"
#include "Camera.h"
#include "Tasks/TaskTypes/TaskSimpleGangDriveBy.h"
#include "../ProbeRuntime.hpp"
namespace CamShooting355Live {
template<class T> T& At(void* b,size_t o){return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o);}
inline json Bytes(const void* p,size_t n){json r=json::array();auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)r.push_back(b[i]);return r;}
inline json trace;inline uint32 scenario;inline CEntity* aimTarget;inline CWeaponInfo* weaponInfo;
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline CPad* __cdecl GetPad(int32 n){return &CPad::Pads[n];}
inline float __fastcall Heading(const CPlaceable*,void*){trace.push_back({"heading"});return float(int32(scenario%13)-6)*.25f;}
inline int16 __fastcall AimX(const CPad* pad,void*,CPed*){trace.push_back({"aimX",pad==&CPad::Pads[1]?1:0});return int16(int32((scenario/4)%9)*32-128);}
inline int16 __fastcall AimY(const CPad* pad,void*,CPed*){trace.push_back({"aimY",pad==&CPad::Pads[1]?1:0});return int16(int32((scenario/36)%9)*32-128);}
inline eWeaponSkill __fastcall Skill(CPed*,void*){trace.push_back({"skill"});return eWeaponSkill::STD;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"info",int32(type),int32(skill)});return weaponInfo;}
inline CEntity* __cdecl Target(float x,float y,float range,CVector p,float* outX,float* outY){trace.push_back({"target",F(x),F(y),F(range),F(p.x),F(p.y),F(p.z)});*outX=float(int32(scenario%7)-3)*.375f;*outY=float(int32((scenario/7)%7)-3)*.375f;return scenario%3==0?nullptr:aimTarget;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back({"appearance"});return eVehicleAppearance(scenario%4);}
inline bool __fastcall Rotate(CCam* c,void*,const CVector& target,float heading){trace.push_back({"rotate",F(target.x),F(target.y),F(target.z),F(heading),F(c->m_vecSource.x),F(c->m_vecSource.y),F(c->m_vecSource.z)});c->m_vecSource.x+=.125f;return true;}
inline void __fastcall Avoid(CCamera*,void*,const CVector* from,const CVector* target,CVector* output,float fov){trace.push_back({"avoid",F(from->x),F(from->y),F(from->z),F(target->x),F(target->y),F(target->z),F(fov)});*output=*from;output->z+=float(scenario%5)*.125f;}
inline void __fastcall Vectors(CCam* c,void*){trace.push_back({"vectors",F(c->m_vecFront.x),F(c->m_vecFront.y),F(c->m_vecFront.z)});c->m_vecFront.Normalise();c->m_vecUp=CVector{0,0,1};}
inline bool __fastcall Lean(CVehicle*,void*,CPed*){trace.push_back({"lean"});return scenario%4==0;}
inline void __fastcall WeaponUpdate(CWeapon* w,void*,CPed*){trace.push_back({"weaponUpdate",int32(w->m_Type),w->m_AmmoInClip});w->m_AmmoInClip+=1;}
inline bool __fastcall Melee(CWeapon*,void*){trace.push_back({"melee"});return scenario%5==0;}
inline int16 __fastcall Fire(const CPad*,void*){trace.push_back({"fire"});return int16(scenario%4!=3);}
inline bool __fastcall Drive(CTaskSimpleGangDriveBy* task,void*,CPed*){trace.push_back({"drive",task->m_bIsFinished,task->m_bAnimsReferenced,task->m_bSeatRHS,task->m_bFromScriptCommand,int32(task->m_nFrequencyPercentage),int32(task->m_nDrivebyStyle),int32(task->m_nFakeShootDirn),F(task->m_fAbortRange),uintptr_t(task->m_pWeaponInfo),F(task->m_vecCoords.x),F(task->m_vecCoords.y),F(task->m_vecCoords.z)});return true;}
inline void __cdecl Shake(CCamera* camera,float force){trace.push_back({"shake",F(force)});camera->m_fCamShakeForce=force;camera->m_nCamShakeStart=123;}
inline bool done;inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_Cam_TwoPlayer_InCarAndShooting",0x519810,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CCam")for(auto& h:c.Items())if(h->Name()=="Process_Cam_TwoPlayer_InCarAndShooting")result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"cam-shooting355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-shooting355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-shooting355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch pad(0x53FB70,GetPad),heading0(0x441DB0,Heading),heading1(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
 EntryPatch x0(0x541040,AimX),x1(bit_cast<uintptr_t>(&CPad::AimWeaponLeftRight),AimX),y0(0x5410C0,AimY),y1(bit_cast<uintptr_t>(&CPad::AimWeaponUpDown),AimY);
 EntryPatch skill0(0x5E6580,Skill),skill1(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),Skill);
 EntryPatch info0(0x743C60,Info),info1(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
 EntryPatch target0(0x73E240,Target),target1(bit_cast<uintptr_t>(&CWeapon::FindNearestTargetEntityWithScreenCoors),Target);
 EntryPatch appearance0(0x6D1080,Appearance),appearance1(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance),appearance2(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance_Reversed),Appearance);
 EntryPatch rot0(0x50A4F0,Rotate),rot1(bit_cast<uintptr_t>(&CCam::RotCamIfInFrontCar),Rotate),avoid0(0x514030,Avoid);
 EntryPatch vectors0(0x509CE0,Vectors),vectors1(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Vectors),lean0(0x6D5CF0,Lean),lean1(bit_cast<uintptr_t>(&CVehicle::CanPedLeanOut),Lean);
 EntryPatch update0(0x73DB40,WeaponUpdate),update1(bit_cast<uintptr_t>(&CWeapon::Update),WeaponUpdate),melee0(0x73B1C0,Melee),melee1(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),Melee);
 EntryPatch fire0(0x53FF90,Fire),fire1(bit_cast<uintptr_t>(&CPad::GetCarGunFired),Fire),drive0(0x627CC0,Drive),shake0(0x50A970,Shake),shake1(bit_cast<uintptr_t>(&CamShakeNoPos),Shake);
 struct Region{uintptr_t address;std::vector<uint8> bytes;};std::array<Region,1> regions{{{0xB6F028,std::vector<uint8>(sizeof(CCamera))}}};for(auto& r:regions)std::memcpy(r.bytes.data(),reinterpret_cast<void*>(r.address),r.bytes.size());
 std::array<uint8,800> savedPlayers{};std::memcpy(savedPlayers.data(),reinterpret_cast<void*>(0xB7CD98),800);
 std::array<uint8,8> savedModels{};std::memcpy(savedModels.data(),reinterpret_cast<void*>(0xA9B0CC),8);const auto savedStep=StaticRef<float>(0xB7CB5C);
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);bool passed=true;int failures=0;
 for(int test=0;test<1024;++test){
  auto run=[&](bool candidate){
   hook->State(candidate);scenario=test;trace=json::array();for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.bytes.data(),r.bytes.size());std::memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);
   alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};camBytes.fill(0x5A);auto* c=reinterpret_cast<CCam*>(camBytes.data());c->m_fFOV=65.f+float(test%46);c->m_fVerticalAngle=float(int32((test/4)%9)-4)*.375f;c->m_fX_Targetting=float(int32(test%13)-6)*.25f;c->m_fY_Targetting=float(int32((test/13)%13)-6)*.25f;c->m_vecSource=CVector{float(test%9),float(test%11),float(test%7)};c->m_bResetStatics=(test%2)!=0;
   alignas(CEntity) std::array<uint8,sizeof(CEntity)> aimBytes{};aimTarget=reinterpret_cast<CEntity*>(aimBytes.data());aimTarget->GetPosition()=CVector{3.25f,4.5f,5.75f};
   alignas(CWeaponInfo) std::array<uint8,sizeof(CWeaponInfo)> infoBytes{};weaponInfo=reinterpret_cast<CWeaponInfo*>(infoBytes.data());weaponInfo->m_fWeaponRange=10.f+float(test%31);
   alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,2> pedBytes{};
   alignas(CVehicle) std::array<std::array<uint8,sizeof(CVehicle)>,2> vehicleBytes{};
   alignas(CMatrix) std::array<std::array<uint8,sizeof(CMatrix)>,2> matrixBytes{};
   alignas(CBaseModelInfo) std::array<std::array<uint8,sizeof(CBaseModelInfo)>,2> modelBytes{};
   alignas(CColModel) std::array<std::array<uint8,sizeof(CColModel)>,2> colBytes{};
   for(int j=0;j<2;++j){
    auto* ped=reinterpret_cast<CPed*>(pedBytes[j].data());auto* v=reinterpret_cast<CVehicle*>(vehicleBytes[j].data());auto* matrix=reinterpret_cast<CMatrix*>(matrixBytes[j].data());
    ped->m_pVehicle=v;ped->m_nActiveWeaponSlot=0;auto& weapon=ped->GetActiveWeapon();weapon.m_Type=WEAPON_PISTOL;weapon.m_State=eWeaponState(test%3);weapon.m_AmmoInClip=uint32(test%17);StaticRef<CPed*>(0xB7CD98+j*400)=ped;At<uintptr_t>(v,0x14)=uintptr_t(matrix);At<uint8>(v,0x36)=uint8(test%16==0?3:(test%4==0?0x42:2));At<uint32>(v,0x594)=uint32((test/4)%10);At<int16>(v,0x22)=int16(j+1);
    const CVector pos{float(int32((test/4+j*3)%13)-6)*2.f,float(int32((test/52+j*5)%11)-5)*2.f,float((test/572+j)%3)};
    At<CVector>(v,4)=pos;matrix->GetPosition()=pos;matrix->GetRight()=CVector{1,0,0};matrix->GetForward()=CVector{0,1,0};matrix->GetUp()=CVector{0,0,1};
    v->m_vecMoveSpeed=CVector{float(int32((test/8+j)%7)-3)*.125f,float(int32((test/56+j*2)%7)-3)*.125f,float(j)*.0625f};
    At<uintptr_t>(modelBytes[j].data(),0x14)=uintptr_t(colBytes[j].data());At<float>(colBytes[j].data(),0x14)=.5f+float((test/7+j)%9)*.5f;At<float>(colBytes[j].data(),4)=-float(1+(test/9)%7)*.5f;
    StaticRef<uintptr_t>(0xA9B0CC+j*4)=uintptr_t(modelBytes[j].data());
   }
   if(test%32==0){auto* v=reinterpret_cast<CVehicle*>(vehicleBytes[1].data());At<CVector>(v,4)=At<CVector>(vehicleBytes[0].data(),4);reinterpret_cast<CMatrix*>(matrixBytes[1].data())->GetPosition()=At<CVector>(vehicleBytes[0].data(),4);}
   auto* vehicle=reinterpret_cast<CVehicle*>(vehicleBytes[0].data());vehicle->m_pDriver=reinterpret_cast<CPed*>(pedBytes[test%2].data());vehicle->m_apPassengers[0]=reinterpret_cast<CPed*>(pedBytes[(test/2)%2].data());c->m_pCamTargetEntity=vehicle;StaticRef<float>(0xB7CB5C)=float(test%7)*.25f;
   const bool switched=ReversingProbe::JumpsToDll(0x519810)==candidate;reinterpret_cast<void(__thiscall*)(CCam*)>(0x519810)(c);
   json regionState=json::array();for(auto& r:regions)regionState.push_back(Bytes(reinterpret_cast<void*>(r.address),r.bytes.size()));
   return json{{"regions",regionState},{"trace",trace},{"info",Bytes(infoBytes.data(),infoBytes.size())},{"target",Bytes(aimBytes.data(),aimBytes.size())},{"cam",Bytes(camBytes.data(),camBytes.size())},{"peds",Bytes(pedBytes.data(),sizeof(pedBytes))},{"vehicles",Bytes(vehicleBytes.data(),sizeof(vehicleBytes))},{"matrices",Bytes(matrixBytes.data(),sizeof(matrixBytes))},{"models",Bytes(modelBytes.data(),sizeof(modelBytes))},{"colModels",Bytes(colBytes.data(),sizeof(colBytes))},{"players",Bytes(reinterpret_cast<void*>(0xB7CD98),800)},{"modelPointers",Bytes(reinterpret_cast<void*>(0xA9B0CC),8)},{"step",std::bit_cast<uint32>(StaticRef<float>(0xB7CB5C))},{"switches",switched}};
  };
  const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",test},{"function","CCam::Process_Cam_TwoPlayer_InCarAndShooting"},{"case",test},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.bytes.data(),r.bytes.size());std::memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);std::memcpy(reinterpret_cast<void*>(0xA9B0CC),savedModels.data(),8);StaticRef<float>(0xB7CB5C)=savedStep;
 Write({{"event","function_summary"},{"function","CCam::Process_Cam_TwoPlayer_InCarAndShooting"},{"cases",1024},{"failures",failures}});Write({{"event",passed?"complete":"error"},{"pairs",1024},{"state_restored",true}});
}
}
