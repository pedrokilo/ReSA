#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Formation.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace FormationAssign355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline CMatrixLink matrix{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,8> members{},targets{};
inline std::array<CMatrixLink,16> matrices{};
inline json trace;inline int scenario{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline int32 __cdecl Choose(CVector pos,float* total){
 int32 chosen=scenario%int32(CFormation::m_Destinations.m_Count);
 while(CFormation::m_Destinations.m_PointHasBeenClaimed[chosen]) chosen=(chosen+1)%int32(CFormation::m_Destinations.m_Count);
 trace.push_back({"choose",Bits(pos.x),Bits(pos.y),Bits(pos.z),Bits(*total),chosen});
 *total=float(double(*total)+double(chosen+1)*.125);return chosen;
}

inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto a=uintptr_t(p);if(a>=uintptr_t(targetTokens.data())&&a<uintptr_t(targetTokens.data())+sizeof(targetTokens))return json::array({"target",(a-uintptr_t(targetTokens.data()))/4});return a;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CFormation","DistributeDestinations",0x69B240,0,-1,1024},
{"CFormation","DistributeDestinations_CoverPoints",0x69B5B0,0,-1,1024},
{"CFormation","DistributeDestinations_PedsToAttack",0x69B700,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CFormation")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"formation-assign355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"formation-assign355-runtime.jsonl");ReversingProbe::InstallFailureCapture("formation-assign355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xC1A2C0);std::array<uint8,0x294> saved{};std::memcpy(saved.data(),globals,saved.size());int pairs=0;bool pass=true;ReversingProbe::EntryPatch choosePatch(0x69B1B0,Choose),chooseDll(std::bit_cast<uintptr_t>(&CFormation::FindNearestFreeDestination),Choose);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(globals,0xCD,saved.size());
CFormation::m_aPedLinkToDestinations.fill(-1);
auto& p=CFormation::m_Peds;p.m_count=uint32(i%8);
auto& d=CFormation::m_DestinationPeds;d.m_count=uint32(i%7+1);
for(int k=0;k<8;++k){members[k].fill(0);targets[k].fill(0);auto* m=reinterpret_cast<CPed*>(members[k].data());auto* t=reinterpret_cast<CPed*>(targets[k].data());matrices[k].SetUnity();matrices[k+8].SetUnity();CVector a{float(k)*.25f+float(i%11),float(k*k)*.125f-float(i%7),float(k%3)};CVector b{float(k)*-1.5f,float(k%3)*2.f,float(i%5)};matrices[k].GetPosition()=a;matrices[k+8].GetPosition()=b;m->m_matrix=i%2?&matrices[k]:nullptr;t->m_matrix=i%3?&matrices[k+8]:nullptr;m->m_placement.m_vPosn=a;t->m_placement.m_vPosn=b;p.m_peds[k]=m;d.m_peds[k]=t;}
auto& points=CFormation::m_Destinations;points.m_Count=uint32(i%17+8);std::memset(points.m_PointHasBeenClaimed,0,sizeof(points.m_PointHasBeenClaimed));for(int k=0;k<24;++k)points.m_Points[k]=CVector{float(k%5)*1.25f,float(k/5)*.75f,float(k%3)*.125f};
CPedList list=p;CVector center{float(i%11-5),float(i%7-3),float(i%3)};
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
if(fn.address==0x69B5B0)reinterpret_cast<void(__cdecl*)(const CPedList&,CVector)>(fn.address)(list,center);
else reinterpret_cast<void(__cdecl*)(const CPedList&)>(fn.address)(list);
return json{{"globals",Hash(globals,saved.size())},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=i%8!=0;yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(globals,saved.data(),saved.size());Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
