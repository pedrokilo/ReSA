#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "InterestingEvents.h"
#include "../ProbeRuntime.hpp"
namespace InterestingCore355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},rng{};
alignas(16) inline std::array<uint8,sizeof(CEntity)> entity{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> player{};
inline CMatrixLink matrix{};
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto n=uintptr_t(p);if(n>=uintptr_t(&g_InterestingEvents)&&n<uintptr_t(&g_InterestingEvents)+sizeof(g_InterestingEvents))return json::array({"events",n-uintptr_t(&g_InterestingEvents)});if(p==entity.data())return "entity";return n;}
inline void __fastcall Cleanup(CEntity* p,void*,CEntity** slot){trace.push_back({"cleanup",Ptr(p),Ptr(slot)});}
inline CPlayerPed* __cdecl Player(int){trace.push_back({"player"});return reinterpret_cast<CPlayerPed*>(player.data());}
inline json Vec(const CVector& v){return json::array({std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)});}
inline bool __cdecl Los(const CVector& a,const CVector& b,bool c,bool d,bool e,bool f,bool g,bool h,bool j){bool yes=scenario%3!=0;trace.push_back({"los",Vec(a),Vec(b),c,d,e,f,g,h,j,yes});return yes;}
inline void __fastcall Ref(CEntity* p,void*,CEntity** slot){trace.push_back({"reference",Ptr(p),Ptr(slot)});}
inline int __cdecl Random(){constexpr int vals[]{0,127,128,65535,65536,-1};int v=vals[(scenario+rng++)%6];trace.push_back({"random",v});return v;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{
{"CInterestingEvents","Constructor",0x6023A0,0,-1,1024},
{"CInterestingEvents","Destructor",0x856880,0,-1,1024},
{"CInterestingEvents","GetInterestingEvent",0x6028A0,0,-1,2048},
{"CInterestingEvents","Add",0x602590,0,-1,2048}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CInterestingEvents")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"interesting-core355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"interesting-core355-runtime.jsonl");ReversingProbe::InstallFailureCapture("interesting-core355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,sizeof(CInterestingEvents)> saved{};std::memcpy(saved.data(),&g_InterestingEvents,saved.size());auto time=CTimer::m_snTimeInMilliseconds,frame=CTimer::m_FrameCounter;std::array<uint8,sizeof(TheCamera)> cameraSaved{};std::memcpy(cameraSaved.data(),&TheCamera,cameraSaved.size());int pairs=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ce(0x571A00,Cleanup),cd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
EntryPatch re(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch pe(0x56E210,Player),pd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
EntryPatch le(0x56A490,Los),ld(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Los);
EntryPatch fe(0x571B70,Ref),fd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,positive=0,nulls=0,nonnull=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;rng=0;trace=json::array();entity.fill(0);std::memset(&g_InterestingEvents,0xCD,sizeof(g_InterestingEvents));CTimer::m_snTimeInMilliseconds=uint32(i%4==0?0xFFFFFF00u:1000u)+uint32(i%257);CTimer::m_FrameCounter=uint32(i%3==0?0:77);
if(fn.address!=0x6023A0){auto& g=g_InterestingEvents;g.m_nFlags=uint8(i);g.m_nInterestingEvent=i%10<8?int8(i%10):-1;for(int t=0;t<29;++t){g.m_nPriorities[t]=uint8((i+t)%11);g.m_nDelays[t]=uint16((i*17+t)%65536);g.m_nEndsOfTime[t]=uint32(i*t);}for(int k=0;k<8;++k){g.m_Events[k].type=(i+k)%29;g.m_Events[k].entity=((i>>(k%5))&1)?reinterpret_cast<CEntity*>(entity.data()):nullptr;g.m_Events[k].time=CTimer::GetTimeInMS()-uint32((i*31+k*13)%65536);}}
if(fn.address==0x602590){player.fill(0);auto* pl=reinterpret_cast<CPlayerPed*>(player.data());pl->m_matrix=&matrix;matrix.SetUnity();matrix.GetPosition()=CVector{float(i%5)*10.f,float(i%7)*5.f,2.f};auto* en=reinterpret_cast<CEntity*>(entity.data());en->m_placement.m_vPosn=CVector{float(i%17-8)*5.f,float(i%13-6)*5.f,float(i%3)};TheCamera.m_nActiveCam=0;TheCamera.m_aCams[0].m_vecSource=i%4==0?matrix.GetPosition():CVector{1.f,2.f,3.f};auto& g=g_InterestingEvents;g.m_CurrentFrameCounter=i%2?CTimer::GetFrameCounter():CTimer::GetFrameCounter()-1;g.m_vecCenter=CVector{0.f,0.f,0.f};g.vec148=CVector{1.f,0.f,0.f};g.m_fRadius=i%4==0?0.f:30.f;}
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;void* ret{};if(fn.address==0x856880)reinterpret_cast<void(__cdecl*)()>(fn.address)();else if(fn.address==0x602590)reinterpret_cast<void(__thiscall*)(CInterestingEvents*,CInterestingEvents::EType,CEntity*)>(fn.address)(&g_InterestingEvents,static_cast<CInterestingEvents::EType>(i%29),i%11==0?nullptr:reinterpret_cast<CEntity*>(entity.data()));else ret=reinterpret_cast<void*(__thiscall*)(CInterestingEvents*)>(fn.address)(&g_InterestingEvents);
return json{{"return",Ptr(ret)},{"state",ReversingProbe::Bytes(&g_InterestingEvents,sizeof(g_InterestingEvents))},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();nulls+=a["return"].is_null();nonnull+=!a["return"].is_null();Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);if(fn.address!=0x6023A0)pass&=positive>0;if(fn.address==0x6028A0)pass&=nulls>0&&nonnull>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive",positive},{"null",nulls},{"nonnull",nonnull}});
}}
std::memcpy(&g_InterestingEvents,saved.data(),saved.size());std::memcpy(&TheCamera,cameraSaved.data(),cameraSaved.size());CTimer::m_snTimeInMilliseconds=time;CTimer::m_FrameCounter=frame;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
