#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEVehicleAudioEntity.h"
#include "AEAudioUtility.h"
#include "../ProbeRuntime.hpp"
namespace RadioSelection355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},draw{};inline tVehicleAudioSettings options{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto result=hi>=lo?lo+(scenario/7+draw++)%(hi-lo+1):lo;trace.push_back({"random",lo,hi,result});return result;}
inline float __cdecl Stat(eStats stat){const auto value=float(((scenario>>((int(stat)*7)%10))&1)!=0)*(stat==static_cast<eStats>(181)?4.f:1.f);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(value)});return value;}
inline const tVehicleAudioSettings* __cdecl Options(){trace.push_back({"options"});return scenario%7==0?nullptr:&options;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,6> fns{{{"CAERadioTrackManager","ChooseIdentIndex",0x4E94C0,0,-1,1024},{"CAERadioTrackManager","ChooseAdvertIndex",0x4E9570,0,-1,1024},{"CAERadioTrackManager","ChooseMusicTrackIndex",0x4EA270,0,-1,1024},{"CAERadioTrackManager","ChooseDJBanterIndexFromList",0x4E95E0,0,-1,1024},{"CAERadioTrackManager","ChooseTalkRadioShow",0x4E8E40,0,-1,1024},{"CAERadioTrackManager","IsVehicleRadioActive",0x4E9800,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-selection355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-selection355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-selection355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-selection355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,6> hooks{};std::array<bool,6> enabled{},locked{};
 for(size_t n=0;n<6;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,0xEBC> oldHistory{};memcpy(oldHistory.data(),reinterpret_cast<void*>(0xB61D78),oldHistory.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x4D9C10,Random),ad(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CAEAudioUtility::GetRandomNumberInRange<int32>)),Random);
 EntryPatch b(0x558E40,Stat),bd(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),Stat);
 EntryPatch c(0x4F4ED0,Options),cd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio),Options);
 for(size_t n=0;n<6;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};std::array<int32,28> ranges{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(reinterpret_cast<void*>(0xB61D78),0xFF,oldHistory.size());memset(&options,0,sizeof(options));options.RadioType=static_cast<eAERadioType>(i%6-1);
    const auto station=int8(i%12);for(int r=0;r<14;++r){ranges[r*2]=100+r*32;ranges[r*2+1]=ranges[r*2]+i%19;}if(i%7==0)ranges[station*2]=1922;
    if(i%2){const auto identLo=(&StaticRef<int32>(0x8C8FB0))[station*2];const auto identHi=(&StaticRef<int32>(0x8C8FB0))[station*2+1];if(identHi>=identLo)(&StaticRef<int32>(0xB62980))[station*8]=identLo+(i/7)%(identHi-identLo+1);(&StaticRef<int8>(0xB62B40))[station*20]=int8(i/7%8);(&StaticRef<int32>(0xB61D78))[station*15]=ranges[station*2]+i/7%(i%19+1);}
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};int32 ret;
    if(n==3)ret=reinterpret_cast<int32(__thiscall*)(void*,eRadioID,int32**)>(fn.address)(self,static_cast<eRadioID>(station),reinterpret_cast<int32**>(ranges.data()));
    else if(n==4)ret=reinterpret_cast<int8(__cdecl*)()>(fn.address)();
    else if(n==5)ret=reinterpret_cast<bool(__thiscall*)(void*)>(fn.address)(self);
    else if(n==2)ret=reinterpret_cast<int8(__thiscall*)(void*,eRadioID)>(fn.address)(self,static_cast<eRadioID>(station));
    else ret=reinterpret_cast<int32(__thiscall*)(void*,eRadioID)>(fn.address)(self,static_cast<eRadioID>(station));
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"ranges",Hash(ranges.data(),sizeof(ranges))},{"history",Hash(reinterpret_cast<void*>(0xB61D78),oldHistory.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xB61D78),oldHistory.data(),oldHistory.size());bool restored=memcmp(reinterpret_cast<void*>(0xB61D78),oldHistory.data(),oldHistory.size())==0;
 for(size_t n=0;n<6;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

