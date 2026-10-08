#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEVehicleAudioEntity.h"
#include "AEAudioHardware.h"
#include "AudioEngine.h"
#include "AEAudioUtility.h"
#include "AEUserRadioTrackManager.h"
#include "../ProbeRuntime.hpp"
namespace RadioStop355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},draw{};inline tVehicleAudioSettings options{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto result=hi>=lo?lo+(scenario/7+draw++)%(hi-lo+1):lo;trace.push_back({"random",lo,hi,result});return result;}
inline float __cdecl Stat(eStats stat){const auto value=float(((scenario>>((int(stat)*7)%10))&1)!=0)*(stat==static_cast<eStats>(181)?4.f:1.f);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(value)});return value;}
inline const tVehicleAudioSettings* __cdecl Options(){trace.push_back({"options"});return scenario%7==0?nullptr:&options;}
struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::AddMusicTrackIndexToHistory;using CAERadioTrackManager::QueueUpTracksForStation;};
inline int modeCalls{}, userCalls{}, active{};
inline uint8 __fastcall Mode(void*,void*){const auto r=uint8((scenario/3+modeCalls++)%3);trace.push_back({"mode",r});return r;}
inline void __fastcall SetIndex(void*,void*,int32 id){trace.push_back({"set_index",id});}
inline int32 __fastcall User(void*,void*){const auto r=600+(scenario+userCalls++)%101;trace.push_back({"user",r});return r;}
inline int32 __fastcall Active(void*,void*){trace.push_back({"active",active});return active;}
inline void __fastcall Play(void*,void*,uint32 id,int32 next,uint32 offset,uint8 flags,bool user,bool nextUser){trace.push_back({"play",id,next,offset,flags,user,nextUser});}
inline bool __cdecl Probability(float chance){const auto r=(scenario/7+draw++)%3!=0;trace.push_back({"probability",std::bit_cast<uint32>(chance),r});return r;}
inline int32 __fastcall Advert(void*,void*,eRadioID station){const auto r=200+scenario%13;trace.push_back({"advert",int(station),r});return r;}
inline bool __fastcall Queue(void*,void*,eRadioID station,int8* count,int8 state,tRadioSettings& settings){const auto r=state!=2||scenario%3!=0;trace.push_back({"queue",int(station),int(*count),int(state),r});if(r&&*count>=0&&*count<5){settings.TrackQueue[size_t(*count)]=300+state;settings.TrackTypes[size_t(*count)]=state;settings.TrackIndices[size_t(*count)]=int8(40+state);++*count;}return r;}
inline void __fastcall History(void*,void*,eRadioID station,int8 index){trace.push_back({"history",int(station),int(index)});}
inline CAERadioTrackManager* manager{};
inline void __fastcall Report(void*,void*,eAudioEvents event,float volume,float speed){trace.push_back({"frontend",int(event),std::bit_cast<uint32>(volume),std::bit_cast<uint32>(speed)});StaticRef<uint32>(0xB7CB84)+=7;if(scenario%7==0)manager->m_bInitialised=true;if(scenario%11==0)manager->m_ActiveSettings.StationID=static_cast<eRadioID>(0);}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","StopRadio",0x4E9820,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-stop355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-stop355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-stop355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-stop355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,0x40> timer{};memcpy(timer.data(),reinterpret_cast<void*>(0xB7CB48),timer.size());std::array<uint8,3> clock{};memcpy(clock.data(),reinterpret_cast<void*>(0xB70153),3);
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x4D9C10,Random),rd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CAEAudioUtility::GetRandomNumberInRange<int32>)),Random);
 EntryPatch f(0x506EA0,Report),fd(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Report);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;draw=modeCalls=userCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());manager=self;auto& settings=self->m_ActiveSettings;self->m_nMode=static_cast<eRadioTrackMode>((i/14)%8);settings.StationID=static_cast<eRadioID>(i%14);if(i%29==0&&int(self->m_nMode)>=3)settings.StationID=static_cast<eRadioID>(-1);
    settings.BassSetting=static_cast<eBassSetting>((i/11)%3);settings.PlayTime=int32((i/3)%50000);settings.TrackLengthMs=int32((i/5)%70000);settings.CurrTrackType=int8((i/7)%9);settings.CurrTrackID=i%2?100:400;
    for(size_t k=0;k<5;++k){settings.TrackQueue[k]=int32(100+k);settings.TrackTypes[k]=int8((i/17+k)%8);settings.TrackIndices[k]=int8(k);}
    for(size_t k=0;k<14;++k){self->m_aListenTimes[k]=int32(0x7FFFFFF0u+k);self->m_aRadioState[k].m_iTimeInPauseModeInMs=i%5==0?-1:20000;self->m_aRadioState[k].m_iTimeInMs=-1;}
    memset(&options,0xCD,sizeof(options));StaticRef<uint8>(0xB7CB48)=uint8((i/23)%2);StaticRef<uint8>(0xB7CB49)=uint8((i/47)%2);StaticRef<uint32>(0xB7CB84)=uint32(i*17);StaticRef<uint32>(0xB7CB7C)=uint32(i*31);StaticRef<uint8>(0xB70154)=uint8(i%31);StaticRef<uint8>(0xB70153)=uint8(i%24);
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*,tVehicleAudioSettings*,bool)>(fn.address)(self,i%3==0?nullptr:&options,i%2!=0);
    if(!candidate&&(int(self->m_nMode)==3||!trace.empty()))++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"options",Hash(&options,sizeof(options))},{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),0x40)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xB7CB48),timer.data(),timer.size());memcpy(reinterpret_cast<void*>(0xB70153),clock.data(),3);
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


