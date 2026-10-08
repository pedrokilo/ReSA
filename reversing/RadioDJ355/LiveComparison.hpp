#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEVehicleAudioEntity.h"
#include "AEAudioUtility.h"
#include "AEUserRadioTrackManager.h"
#include "../ProbeRuntime.hpp"
namespace RadioDJ355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},draw{};inline tVehicleAudioSettings options{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto result=hi>=lo?lo+(scenario/7+draw++)%(hi-lo+1):lo;trace.push_back({"random",lo,hi,result});return result;}
inline float __cdecl Stat(eStats stat){const auto value=float(((scenario>>((int(stat)*7)%10))&1)!=0)*(stat==static_cast<eStats>(181)?4.f:1.f);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(value)});return value;}
inline const tVehicleAudioSettings* __cdecl Options(){trace.push_back({"options"});return scenario%7==0?nullptr:&options;}
struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseDJBanterIndexFromList;};
inline bool __cdecl Probability(float probability){const auto r=(scenario/7+draw++)%4!=0;trace.push_back({"probability",std::bit_cast<uint32>(probability),r});return r;}
inline bool __cdecl Forecast(eWeatherType weather,int32 steps){const auto r=(scenario/11)%19==int(weather);trace.push_back({"forecast",int(weather),steps,r});return r;}
inline bool __cdecl Riot(){const auto r=scenario%3==0;trace.push_back({"riots",r});return r;}
inline int32 __fastcall List(void*,void*,eRadioID id,int32** list){const auto r=(scenario+draw++)%3==0?-1:600+scenario%17;trace.push_back({"list",int(id),uintptr_t(list),r});return r;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","ChooseDJBanterIndex",0x4EA2D0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-dj355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-dj355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-dj355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-dj355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldSpecial=StaticRef<uint16>(0xB62C72);const auto oldHour=StaticRef<uint8>(0xB70153);const auto oldChase=StaticRef<int32>(0xB72914);std::array<uint8,15*4*13> oldHistory{};memcpy(oldHistory.data(),reinterpret_cast<void*>(0xB61D78),oldHistory.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p(0x4D9C80,Probability),pd(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),Probability);
 EntryPatch f(0x72A590,Forecast),fd(bit_cast<uintptr_t>(&CWeather::ForecastWeather),Forecast);
 EntryPatch r(0x441C10,Riot),rd(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),Riot);
 EntryPatch l(0x4E95E0,List),ld(bit_cast<uintptr_t>(&Access::ChooseDJBanterIndexFromList),List);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(reinterpret_cast<void*>(0xB61D78),0xFF,oldHistory.size());
    const auto station=(i/5)%12;StaticRef<int8>(0xB62C73)=int8(i%5-1);StaticRef<int8>(0xB62C72)=int8((i/48)%22);StaticRef<uint8>(0xB70153)=uint8(i%24);StaticRef<int32>(0xB72914)=i%9==0?1:0;
    if(i%2)(&StaticRef<int32>(0xB61D78))[station*15]=(&StaticRef<int32>(0x8C8BF0))[station*2];
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<int32(__thiscall*)(void*,eRadioID)>(fn.address)(self,static_cast<eRadioID>(station));
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"history",Hash(reinterpret_cast<void*>(0xB61D78),oldHistory.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<uint16>(0xB62C72)=oldSpecial;StaticRef<uint8>(0xB70153)=oldHour;StaticRef<int32>(0xB72914)=oldChase;memcpy(reinterpret_cast<void*>(0xB61D78),oldHistory.data(),oldHistory.size());bool restored=StaticRef<uint16>(0xB62C72)==oldSpecial&&StaticRef<uint8>(0xB70153)==oldHour&&StaticRef<int32>(0xB72914)==oldChase&&memcmp(reinterpret_cast<void*>(0xB61D78),oldHistory.data(),oldHistory.size())==0;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


