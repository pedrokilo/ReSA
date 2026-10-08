#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEVehicleAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "AEUserRadioTrackManager.h"
#include "../ProbeRuntime.hpp"
namespace RadioTrack355Live {
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
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","TrackRadioStation",0x4EAC30,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-track355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-track355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-track355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-track355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto savedModeByte=StaticRef<uint8>(0xBA67F8);const auto savedPause=StaticRef<uint32>(0xB7CB84);std::array<uint8,3> clock{};memcpy(clock.data(),reinterpret_cast<void*>(0xB70153),3);
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch u(0x4F3250,User),ud(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SelectUserTrackIndex),User);
 EntryPatch r(0x4D9C80,Probability),rd(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),Probability);
 EntryPatch q(0x4EA670,Queue),qd(bit_cast<uintptr_t>(&Access::QueueUpTracksForStation),Queue);
 EntryPatch h(0x4E96C0,History),hd(bit_cast<uintptr_t>(&Access::AddMusicTrackIndexToHistory),History);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;draw=modeCalls=userCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());const auto station=static_cast<eRadioID>(i%13);auto& state=self->m_aRadioState[size_t(int8(station))];
    state.m_nGameClockDays=int8((i/13)%6==0?-1:10);state.m_nGameClockHours=int8((i/29)%6==0?-1:12);StaticRef<uint8>(0xB70154)=uint8(i%7==0?9:10);StaticRef<uint8>(0xB70153)=uint8((i/23)%24);StaticRef<uint8>(0xB70155)=uint8((i/17)%12);StaticRef<uint8>(0xBA67F8)=uint8(i%2);
    state.m_iTimeInMs=10000;state.m_iTimeInPauseModeInMs=-1;static constexpr int32 elapsed[]{-1,0,6999,7000,7001,14999,15000,29999,30000,44999,45000,50000,51999,52000,200000,200001,204999,205000,205001};StaticRef<uint32>(0xB7CB84)=uint32(10000+elapsed[(i/3)%std::size(elapsed)]);
    state.m_iTrackPlayTime=(i/31)%2?200:-1;for(size_t k=0;k<3;++k){state.m_aElapsed[k]=15000;state.m_aTrackQueue[k]=int32(100+k);state.m_aTrackTypes[k]=int8((i/7+k)%8);}if(i%5==0)state.m_aTrackQueue[1]=-1;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<bool(__thiscall*)(void*,eRadioID,bool)>(fn.address)(self,station,i%2!=0);
    if(!candidate&&trace.size()>1)++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"mode_global",StaticRef<uint8>(0xBA67F8)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<uint8>(0xBA67F8)=savedModeByte;StaticRef<uint32>(0xB7CB84)=savedPause;memcpy(reinterpret_cast<void*>(0xB70153),clock.data(),3);
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


