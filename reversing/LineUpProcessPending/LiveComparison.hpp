#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskUtilityLineUpPedWithCar.h"
#include "VehicleAnimGroupData.h"
#include "PedPlacement.h"
#include "../ProbeRuntime.hpp"
namespace LineUpProcessPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskUtilityLineUpPedWithCar","ProcessPed",0x6513A0,0,-1,1024}};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped;
alignas(16) inline std::array<uint8,sizeof(CBike)> vehicle;
alignas(16) inline std::array<uint8,sizeof(CTaskUtilityLineUpPedWithCar)> task;
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;
alignas(16) inline std::array<uint8,sizeof(CAnimBlendHierarchy)> hierarchy;
alignas(16) inline std::array<uint8,sizeof(tHandlingData)> handling;
inline CMatrix carMatrix,pedMatrix;inline std::array<void*,64> vehicleVT{};
inline int caseId{},scenario{},animId{};inline bool upright{},correctZ{};inline float timing{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Vec(const CVector& v){return json::array({std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)});}
inline json Matrix(const CMatrix& m){return json::array({Vec(m.GetRight()),Vec(m.GetForward()),Vec(m.GetUp()),Vec(m.GetPosition())});}
inline json Words(const void* p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(static_cast<const uint32*>(p)[i]);return j;}
inline CVector* __fastcall Position(CTaskUtilityLineUpPedWithCar*,void*,CVector* out,CVehicle*,float progress,CAnimBlendAssociation*){trace.push_back({"position",std::bit_cast<uint32>(progress)});*out=CVector{progress+2.f,-progress+1.f,progress+.75f};return out;}
inline CVector* __fastcall Local(CTaskUtilityLineUpPedWithCar*,void*,CVector* out,CVehicle*,float progress,CAnimBlendAssociation*){trace.push_back({"local",std::bit_cast<uint32>(progress)});*out=CVector{.25f,.5f,1.f};return out;}
inline float __fastcall Height(CVehicle*,void*){trace.push_back({"height"});return .5f;}
inline bool __cdecl ZExe(CVector* p){trace.push_back({"find_z",Vec(*p),correctZ});if(correctZ)p->z=2.25f;return correctZ;}
inline std::pair<CVector,bool> __cdecl ZDll(CVector p){ZExe(&p);return {p,correctZ};}
inline bool __fastcall Upside(const CVehicle*,void*){trace.push_back({"upside",!upright});return !upright;}
inline AssocGroupId __fastcall Group(const CVehicleAnimGroup*,void*,AnimationId id){trace.push_back({"anim_group",int(id)});return AssocGroupId(0);}
inline float __fastcall Timing(CVehicleAnimGroup*,void*,AnimationId id){trace.push_back({"critical",int(id),std::bit_cast<uint32>(timing)});return timing;}
inline void __fastcall Set(CPlaceable* p,void*,CMatrix& m){trace.push_back({"set_matrix",Matrix(m)});auto* matrix=p==reinterpret_cast<CPlaceable*>(ped.data())?&pedMatrix:&carMatrix;*matrix=m;}
inline void __fastcall Rotate(CPlaceable*,void*,float x,float y,float z){trace.push_back({"orientation",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z)});pedMatrix.SetRotateZOnly(z);}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"line-up-process-pending-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"line-up-process-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("line-up-process-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"line-up-process-pending-only.txt");if(f)std::getline(f,only);}const auto savedTime=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();std::array<uint8,0x94> savedGroup;std::memcpy(savedGroup.data(),reinterpret_cast<void*>(0xC1CDC0),savedGroup.size());bool pass=true,selected=false;int total=0;
vehicleVT[53]=reinterpret_cast<void*>(&Height);
for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}const bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch pe(0x650A80,Position),pd(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::GetPositionToOpenCarDoor),Position);
EntryPatch le(0x64FC10,Local),ld(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor),Local);
EntryPatch ze(0x616920,ZExe),zd(bit_cast<uintptr_t>(&CPedPlacement::FindZCoorForPed),ZDll);
EntryPatch ue(0x6D1D90,Upside),ud(bit_cast<uintptr_t>(&CVehicle::IsUpsideDown),Upside);
EntryPatch ge(0x6E3B00,Group),gd(bit_cast<uintptr_t>(&CVehicleAnimGroup::GetGroup),Group);
EntryPatch te(0x6E3C80,Timing),td(bit_cast<uintptr_t>(&CVehicleAnimGroup::ComputeCriticalBlendTime),Timing);
EntryPatch se(0x54F610,Set),sd(bit_cast<uintptr_t>(&CPlaceable::SetMatrix),Set);
EntryPatch ore(0x439A80,Rotate),ord(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),Rotate);
const int anims[]{0x6C,0x15F,0x163,0x167,0x16B,0x175,0x17A,0x185};
for(int i=0;i<fn.cases;++i){caseId=i;scenario=i%32;animId=anims[(i/128)%8];json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const bool switches=ReversingProbe::JumpsToDll(fn.address)==(candidate!=0);pass&=switches;
ped.fill(0);vehicle.fill(0);task.fill(0xCD);assoc.fill(0);hierarchy.fill(0);handling.fill(0);trace=json::array();
auto* t=reinterpret_cast<CTaskUtilityLineUpPedWithCar*>(task.data());auto* p=reinterpret_cast<CPed*>(ped.data());auto* v=reinterpret_cast<CVehicle*>(vehicle.data());auto* a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());auto* hd=reinterpret_cast<tHandlingData*>(handling.data());
Put(vehicle.data(),0,vehicleVT.data());v->m_nModelIndex=scenario==31?577:400;v->m_pHandlingData=hd;v->m_nVehicleType=eVehicleType((scenario&8)?9:(scenario&16)?5:0);v->m_nVehicleSubType=eVehicleType(scenario==7?2:0);v->physicalFlags.bSubmergedInWater=(scenario&4)!=0;hd->m_nAnimGroup=0;
upright=(scenario&2)!=0;carMatrix.SetUnity();carMatrix.GetUp().z=scenario==0?1.f:upright?1.f:-1.f;carMatrix.GetPosition()=CVector{.25f,.5f,1.25f};pedMatrix.SetUnity();pedMatrix.GetPosition()=CVector{.5f,.75f,.75f};Put(vehicle.data(),0x14,&carMatrix);Put(ped.data(),0x14,&pedMatrix);p->m_fCurrentRotation=.125f;p->m_fAimingRotation=.25f;p->m_vecMoveSpeed=CVector{.25f,.5f,-.1f};
const int doors[]{8,9,10,11,18};t->m_nDoorIdx=doors[scenario%5];t->m_nDoorOpenPosType=(i/32)%4;t->m_Offset=CVector{.25f,-.5f,.75f};t->m_fDoorOpenPosZ=-999.99f;t->m_fTime=(scenario&16)?1300:900;
a->m_AnimId=AnimationId(animId);a->m_BlendHier=reinterpret_cast<CAnimBlendHierarchy*>(hierarchy.data());a->m_BlendHier->m_fTotalTime=1.f;a->m_CurrentTime=float(scenario%5)*.25f;timing=scenario%4==0?-.25f:scenario%4==1?.5f:scenario%4==2?11.25f:-11.25f;correctZ=(scenario&1)!=0;CVehicleAnimGroupData::GetVehicleAnimGroup(0).m_specialFlags.bRunSpecialLockedDoor=(scenario&1)!=0;
*reinterpret_cast<uint32*>(0xB7CB84)=1000;CTimer::SetTimeStep(float(scenario%3)+.5f);
const bool ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,CVehicle*,CAnimBlendAssociation*)>(fn.address)(t,p,v,scenario==0?nullptr:a);
if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(("animation_"+std::to_string(animId)).c_str());hit(("position_type_"+std::to_string(t->m_nDoorOpenPosType)).c_str());hit((scenario&16)?"fade_timer":"timer_finished");for(const auto& e:trace)if(e[0].is_string())hit(e[0].get<std::string>().c_str());}
result[candidate]={{"return",ret},{"task",Words(task.data(),task.size())},{"ped",Words(ped.data()+0x44,12)},{"rot",Words(ped.data()+0x558,8)},{"matrix",Matrix(pedMatrix)},{"trace",trace},{"switch",switches}};
}const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}
*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;CTimer::SetTimeStep(savedStep);std::memcpy(reinterpret_cast<void*>(0xC1CDC0),savedGroup.data(),savedGroup.size());h->State(hs);h->LockState(hl);auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
for(const auto* n:{"position","local","height","find_z","critical","set_matrix","orientation","fade_timer","timer_finished","position_type_0","position_type_1","position_type_2","position_type_3"})require(n);
Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",CTimer::GetTimeInMS()==savedTime&&CTimer::GetTimeStep()==savedStep&&std::memcmp(reinterpret_cast<void*>(0xC1CDC0),savedGroup.data(),savedGroup.size())==0}});
}
}
