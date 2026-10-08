#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEVehicleAudioEntity.h"
#include "AEAudioUtility.h"
#include "AEUserRadioTrackManager.h"
#include "../ProbeRuntime.hpp"
namespace RadioQueue355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},draw{};inline tVehicleAudioSettings options{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto result=hi>=lo?lo+(scenario/7+draw++)%(hi-lo+1):lo;trace.push_back({"random",lo,hi,result});return result;}
inline float __cdecl Stat(eStats stat){const auto value=float(((scenario>>((int(stat)*7)%10))&1)!=0)*(stat==static_cast<eStats>(181)?4.f:1.f);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(value)});return value;}
inline const tVehicleAudioSettings* __cdecl Options(){trace.push_back({"options"});return scenario%7==0?nullptr:&options;}
struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseIdentIndex;using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::ChooseDJBanterIndex;using CAERadioTrackManager::ChooseMusicTrackIndex;};
inline int32 __fastcall Ident(void*,void*,eRadioID id){const auto r=scenario%4==0?-1:100+scenario%9;trace.push_back({"ident",int(id),r});return r;}
inline int32 __fastcall Advert(void*,void*,eRadioID id){const auto r=200+scenario%9;trace.push_back({"advert",int(id),r});return r;}
inline int32 __fastcall DJ(void*,void*,eRadioID id){const auto r=scenario%3==0?-1:300+scenario%9;trace.push_back({"dj",int(id),r});return r;}
inline int8 __fastcall Music(void*,void*,eRadioID id){const auto r=int8(scenario%31);trace.push_back({"music",int(id),int(r)});return r;}
inline int32 __fastcall User(void*,void*){const auto r=scenario%4==0?-1:scenario%32;trace.push_back({"user",r});return r;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","QueueUpTracksForStation",0x4EA670,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-queue355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-queue355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-queue355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-queue355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x4D9C10,Random),rd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CAEAudioUtility::GetRandomNumberInRange<int32>)),Random);
 EntryPatch a(0x4E94C0,Ident),ad(bit_cast<uintptr_t>(&Access::ChooseIdentIndex),Ident);
 EntryPatch b(0x4E9570,Advert),bd(bit_cast<uintptr_t>(&Access::ChooseAdvertIndex),Advert);
 EntryPatch c(0x4EA2D0,DJ),cd(bit_cast<uintptr_t>(&Access::ChooseDJBanterIndex),DJ);
 EntryPatch d(0x4EA270,Music),dd(bit_cast<uintptr_t>(&Access::ChooseMusicTrackIndex),Music);
 EntryPatch u(0x4F3250,User),ud(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SelectUserTrackIndex),User);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};tRadioSettings settings;int8 count{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(&settings,0xCD,sizeof(settings));count=int8(i%3);
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<bool(__thiscall*)(void*,eRadioID,int8*,int8,tRadioSettings&)>(fn.address)(self,static_cast<eRadioID>((i/3)%13),&count,int8((i/39)%9-1),settings);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"count",count},{"object",Hash(object.data(),object.size())},{"settings",Hash(&settings,sizeof(settings))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


