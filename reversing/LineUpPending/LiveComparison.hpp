#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskUtilityLineUpPedWithCar.h"
#include "VehicleAnimGroupData.h"
#include "Models/VehicleModelInfo.h"
#include "../ProbeRuntime.hpp"
namespace LineUpPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CTaskUtilityLineUpPedWithCar","GetLocalPositionToOpenCarDoor",0x64FC10,0,-1,1024},
{"CTaskUtilityLineUpPedWithCar","GetPositionToOpenCarDoor",0x650A80,0,-1,1024}};
alignas(16) inline std::array<uint8,sizeof(CBike)> vehicle;
alignas(16) inline std::array<uint8,sizeof(CTaskUtilityLineUpPedWithCar)> task;
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc;
alignas(16) inline std::array<uint8,sizeof(tHandlingData)> handling;
alignas(16) inline std::array<uint8,sizeof(CVehicleModelInfo)> model;
alignas(16) inline std::array<uint8,sizeof(CVehicleModelInfo::CVehicleStructure)> structure;
inline CMatrix matrix;
inline int caseId{},scenario{},door{};
template<class T>inline void Put(void* p,size_t o,T v){std::memcpy(static_cast<uint8*>(p)+o,&v,sizeof(v));}
inline json Vec(const CVector& v){return json::array({std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)});}
inline json Words(const void* p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(static_cast<const uint32*>(p)[i]);return j;}
inline CVector* __fastcall Offset(CVehicleAnimGroup* group,void*,CVector* out,eVehAnimDoorOffset id){trace.push_back({"offset",(uintptr_t(group)-0xC1CDC0)/0x94,int(id)});const float a=group==reinterpret_cast<CVehicleAnimGroup*>(0xC1CDC0)?1.f:2.f;*out=CVector{a+float(id)*.125f,a*.75f,.125f*float(id+1)};return out;}
inline void __fastcall Bike(CBike* v,void*,CVector& out,CVector offset,CVector seat){trace.push_back({"bike",Vec(offset),Vec(seat)});const auto p=matrix.TransformPoint(seat-offset);out=p;}
inline CVector* __fastcall Local(CTaskUtilityLineUpPedWithCar*,void*,CVector* out,CVehicle*,float progress,CAnimBlendAssociation*){trace.push_back({"local",std::bit_cast<uint32>(progress)});*out=CVector{float(caseId%7)*.125f,-float(caseId%11)*.25f,1.25f};return out;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* c,const char* n){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==c)for(auto& h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"line-up-pending-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"line-up-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("line-up-pending");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::string only;{std::ifstream f(dir/"line-up-pending-only.txt");if(f)std::getline(f,only);}const uintptr_t registry=0xA9B0C8+400*4;const auto savedModel=*reinterpret_cast<uintptr_t*>(registry);std::array<uint8,0x128> savedGroups;std::memcpy(savedGroups.data(),reinterpret_cast<void*>(0xC1CDC0),savedGroups.size());bool pass=true,selected=false;int total=0;
for(const auto& fn:fns){const auto full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;selected=true;auto* h=FindHook(fn.cls,fn.name);if(!h){Write({{"event","error"},{"function",full},{"reason","missing hook"}});return;}const bool hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch oe(0x6E3D10,Offset),od(bit_cast<uintptr_t>(&CVehicleAnimGroup::ComputeAnimDoorOffsets),Offset);
EntryPatch be(0x6BF230,Bike),bd(bit_cast<uintptr_t>(&CBike::GetCorrectedWorldDoorPosition),Bike);
std::optional<EntryPatch> localE,localD;if(fn.address==0x650A80){localE.emplace(0x64FC10,Local);localD.emplace(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor),Local);}
for(int i=0;i<fn.cases;++i){caseId=i;scenario=i%32;json result[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);const bool switches=ReversingProbe::JumpsToDll(fn.address)==(candidate!=0);pass&=switches;
vehicle.fill(0);task.fill(0xCD);assoc.fill(0);handling.fill(0);model.fill(0);structure.fill(0);trace=json::array();
auto* t=reinterpret_cast<CTaskUtilityLineUpPedWithCar*>(task.data());auto* v=reinterpret_cast<CVehicle*>(vehicle.data());auto* a=reinterpret_cast<CAnimBlendAssociation*>(assoc.data());auto* hd=reinterpret_cast<tHandlingData*>(handling.data());auto* mi=reinterpret_cast<CVehicleModelInfo*>(model.data());auto* st=reinterpret_cast<CVehicleModelInfo::CVehicleStructure*>(structure.data());
*reinterpret_cast<uintptr_t*>(registry)=uintptr_t(model.data());v->m_nModelIndex=400;v->m_pHandlingData=hd;v->m_nVehicleType=eVehicleType((scenario&8)?9:0);Put<uint8>(vehicle.data(),0x429,uint8((scenario&4)!=0));mi->m_pVehicleStruct=st;mi->m_nVehicleType=eVehicleType((scenario&16)?5:0);st->m_avDummyPos[0]=CVector{.5f,1.f,2.f};st->m_avDummyPos[4]=CVector{1.125f,2.25f,.75f};st->m_avDummyPos[5]=CVector{1.5f,-1.125f,.5f};hd->m_fSeatOffsetDistance=.25f+float(i%5)*.125f;hd->m_nAnimGroup=1;
const int doors[]{8,9,10,11,18,123};door=doors[(i/32)%6];t->m_nDoorIdx=door;t->m_nDoorOpenPosType=0;t->m_Offset=CVector{0.f,0.f,0.f};t->m_fDoorOpenPosZ=-999.99f;t->m_fTime=1000;
const int anims[]{0x6C,0x175,0x17A,0x17C,0x185,0x186,0x15F};a->m_AnimId=AnimationId(anims[(i/192)%7]);a->m_BlendAmount=float(scenario%5)*.25f;
CVehicleAnimGroupData::GetVehicleAnimGroup(1).m_specialFlags.bRunSpecialLockedDoor=(scenario&2)!=0;
matrix.SetUnity();matrix.GetRight()=CVector{.75f,.25f,0.f};matrix.GetForward()=CVector{-.25f,.75f,0.f};matrix.GetUp()=CVector{0.f,0.f,1.f};matrix.GetPosition()=CVector{float(i%7),float(i%11),float(i%3)};Put(vehicle.data(),0x14,&matrix);
const float progress=float(i%9)*.125f;CVector out;reinterpret_cast<CVector*(__thiscall*)(void*,CVector*,CVehicle*,float,CAnimBlendAssociation*)>(fn.address)(t,&out,v,progress,(scenario&1)?a:nullptr);
if(candidate){auto hit=[&](const char* n){coverage[n]=coverage.value(n,0)+1;};hit(("door_"+std::to_string(door)).c_str());hit((scenario&1)?"association":"null_association");hit((scenario&16)?"boat_model":"car_model");for(const auto& e:trace)if(e[0].is_string())hit(e[0].get<std::string>().c_str());if(trace.size()>1&&trace[0][0]=="offset"&&trace[1][0]=="offset")hit("blend_offsets");}
result[candidate]={{"out",Vec(out)},{"task",Words(task.data(),task.size())},{"trace",trace},{"switch",switches}};
}const bool equal=result[0]==result[1];pass&=equal;failures+=!equal;++total;Write({{"event","comparison"},{"function",full},{"case",i},{"pair",total-1},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
}
*reinterpret_cast<uintptr_t*>(registry)=savedModel;std::memcpy(reinterpret_cast<void*>(0xC1CDC0),savedGroups.data(),savedGroups.size());h->State(hs);h->LockState(hl);auto require=[&](const char* n){if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",full},{"reason","missing coverage"},{"branch",n}});}};
if(fn.address==0x64FC10){for(const auto* n:{"offset","bike","blend_offsets","null_association","association","boat_model","car_model","door_8","door_9","door_10","door_11","door_18","door_123"})require(n);}else require("local");
Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});
}
Write({{"event",pass&&selected?"complete":"error"},{"pairs",total},{"state_restored",*reinterpret_cast<uintptr_t*>(registry)==savedModel&&std::memcmp(reinterpret_cast<void*>(0xC1CDC0),savedGroups.data(),savedGroups.size())==0}});
}
}
