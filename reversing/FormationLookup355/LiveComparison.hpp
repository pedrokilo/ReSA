#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Formation.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace FormationLookup355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline CMatrixLink matrix{};
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto a=uintptr_t(p);if(a>=uintptr_t(targetTokens.data())&&a<uintptr_t(targetTokens.data())+sizeof(targetTokens))return json::array({"target",(a-uintptr_t(targetTokens.data()))/4});return a;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CFormation","ReturnTargetPedForPed",0x699F50,0,-1,2048},
{"CFormation","ReturnDestinationForPed",0x699FA0,0,-1,2048},
{"CFormation","GenerateGatherDestinations",0x69A620,0,-1,2048}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CFormation")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"formation-lookup355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"formation-lookup355-runtime.jsonl");ReversingProbe::InstallFailureCapture("formation-lookup355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xC1A2C0);std::array<uint8,0x294> saved{};std::memcpy(saved.data(),globals,saved.size());int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){std::memset(globals,0xCD,saved.size());auto* links=reinterpret_cast<int32*>(globals);for(int k=0;k<24;++k)links[k]=((i+k)%5==0)?-1:(i+k)%24;
auto& p=CFormation::m_Peds;p.m_count=uint32(i%10);for(int k=0;k<30;++k)p.m_peds[k]=reinterpret_cast<CPed*>(&pedTokens[(i%7==0)?k%4:k]);auto& d=CFormation::m_DestinationPeds;d.m_count=uint32(i%25);for(int k=0;k<30;++k)d.m_peds[k]=reinterpret_cast<CPed*>(&targetTokens[k]);auto& points=CFormation::m_Destinations;points.m_Count=uint32(i%24);for(int k=0;k<24;++k)points.m_Points[k]=CVector{float(k)*.125f,float(i-k),std::bit_cast<float>(uint32(0x3f800000+k))};
auto* query=reinterpret_cast<CPed*>(&pedTokens[i%32]);alignas(16) std::array<uint8,16> output{};output.fill(0xCD);bool ret=false;h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
if(fn.address==0x69A620){anchor.fill(0);auto* leader=reinterpret_cast<CPed*>(anchor.data());leader->m_matrix=i%2?&matrix:nullptr;matrix.SetUnity();matrix.GetPosition()=CVector{float(i%17-8)*.5f,float(i%11)*.25f,3.f};leader->m_placement.m_vPosn=CVector{1.f,2.f,3.f};constexpr float angles[]{0.f,PI/2.f,PI,-PI,.125f,10000.f};leader->m_fCurrentRotation=angles[(i/31)%6];CPedList list{};list.m_count=i%31;reinterpret_cast<void(__cdecl*)(CPedList&,CPed*)>(fn.address)(list,leader);}
else if(fn.address==0x699F50)reinterpret_cast<void(__cdecl*)(CPed*,CPed**)>(fn.address)(query,reinterpret_cast<CPed**>(output.data()));else ret=reinterpret_cast<bool(__cdecl*)(CPed*,CVector*)>(fn.address)(query,reinterpret_cast<CVector*>(output.data()));
return json{{"bool",ret},{"out",ReversingProbe::Bytes(output.data(),output.size())},{"pointer",fn.address==0x699F50?Ptr(*reinterpret_cast<CPed**>(output.data())):json(nullptr)},{"globals",Hash(globals,saved.size())},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=fn.address==0x69A620?i%31!=0:fn.address==0x699FA0?a["bool"].get<bool>():a["pointer"]!=json(uintptr_t(0xCDCDCDCD));yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(globals,saved.data(),saved.size());Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
