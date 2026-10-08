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
namespace RadioService355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},draw{};inline tVehicleAudioSettings options{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto result=hi>=lo?lo+(scenario/7+draw++)%(hi-lo+1):lo;trace.push_back({"random",lo,hi,result});return result;}
inline float __cdecl Stat(eStats stat){const auto value=float(((scenario>>((int(stat)*7)%10))&1)!=0)*(stat==static_cast<eStats>(181)?4.f:1.f);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(value)});return value;}
inline const tVehicleAudioSettings* __cdecl Options(){trace.push_back({"options"});return scenario%7==0?nullptr:&options;}
struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::CheckForMissionStatsChanges;using CAERadioTrackManager::CheckForStationRetune;using CAERadioTrackManager::CheckForStationRetuneDuringPause;using CAERadioTrackManager::CheckForPause;using CAERadioTrackManager::CheckForTrackConcatenation;using CAERadioTrackManager::AddMusicTrackIndexToHistory;using CAERadioTrackManager::AddIdentIndexToHistory;using CAERadioTrackManager::AddAdvertIndexToHistory;using CAERadioTrackManager::AddDJBanterIndexToHistory;};
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
inline int optionCalls{};
inline int8 __fastcall Cutscene(void*,void*){const auto value=int8(scenario%9==0?1:0);trace.push_back({"cutscene",int(value)});return value;}
inline tVehicleAudioSettings* __cdecl VehicleOptions(){const auto call=optionCalls++;options.RadioType=static_cast<eAERadioType>(call==0?scenario%4:(scenario%6==0?1:0));trace.push_back({"options",call,int(options.RadioType)});return call==0&&scenario%17==0?nullptr:&options;}
inline void __fastcall Stop(void* raw,void*,tVehicleAudioSettings* settings,bool pause){trace.push_back({"stop",settings!=nullptr,pause});auto* self=static_cast<CAERadioTrackManager*>(raw);self->m_nMode=scenario%5==0?eRadioTrackMode::RADIO_STOPPING:eRadioTrackMode::RADIO_STOPPED;self->m_bInitialised=false;self->m_nStationsListed=self->m_nStationsListDown=0;}
inline void __fastcall Start(void* raw,void*,eRadioID station,eBassSetting bass,float gain,bool skip){trace.push_back({"start",int(station),int(bass),std::bit_cast<uint32>(gain),skip});static_cast<CAERadioTrackManager*>(raw)->m_ActiveSettings.StationID=station;}
inline void __cdecl Mission(){trace.push_back({"mission_stats"});}
inline void __fastcall Retune(void*,void*){trace.push_back({"retune"});}
inline void __fastcall PauseRetune(void*,void*){trace.push_back({"pause_retune"});}
inline void __fastcall Pause(void*,void*){trace.push_back({"pause"});}
inline void __fastcall Volume(void*,void*){trace.push_back({"volume"});}
inline void __fastcall Concat(void*,void*){trace.push_back({"concat"});}
inline int32 __fastcall Length(void*,void*){trace.push_back({"length"});return 70000;}
inline int32 __fastcall Playing(void*,void*){const auto r=scenario%3==0?manager->m_ActiveSettings.TrackQueue[0]:scenario%3==1?manager->m_ActiveSettings.PrevTrackID:900;trace.push_back({"playing",r});return r;}
inline bool __fastcall Ambience(void*,void*){const auto r=scenario%5==0;trace.push_back({"ambience",r});return r;}
inline bool __cdecl VehicleActive(){const auto r=scenario%3!=0;trace.push_back({"vehicle_active",r});return r;}
inline void __fastcall Bass(void*,void*,eBassSetting setting,float gain){trace.push_back({"bass",int(setting),std::bit_cast<uint32>(gain)});}
inline void __fastcall Playback(void*,void*){trace.push_back({"playback"});}
inline void __fastcall StopTrack(void*,void*){trace.push_back({"stop_track"});}
inline void __fastcall HardwareService(void*,void*){trace.push_back({"hardware_service"});++StaticRef<uint32>(0xB7CB84);}
inline int32 __fastcall Time(void*,void*){trace.push_back({"time",-6});return -6;}
inline void __fastcall MusicHistory(void*,void*,eRadioID station,int8 index){trace.push_back({"music_history",int(station),int(index)});}
inline void __fastcall IdentHistory(void*,void*,eRadioID station,int32 index){trace.push_back({"ident_history",int(station),index});}
inline void __fastcall AdvertHistory(void*,void*,eRadioID station,int32 index){trace.push_back({"advert_history",int(station),index});}
inline void __fastcall DJHistory(void*,void*,eRadioID station,int32 index){trace.push_back({"dj_history",int(station),index});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","Service",0x4EB9A0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-service355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-service355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-service355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-service355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,0x40> timer{};memcpy(timer.data(),reinterpret_cast<void*>(0xB7CB48),timer.size());const auto oldPrevious=StaticRef<int32>(0x8CBA68);std::array<uint8,sizeof(CPad)> pad{};memcpy(pad.data(),CPad::GetPad(0),pad.size());
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch l(0x4D8F70,Length),ld(bit_cast<uintptr_t>(&CAEAudioHardware::GetTrackLengthMs),Length);
 EntryPatch p(0x4D8F90,Playing),pd(bit_cast<uintptr_t>(&CAEAudioHardware::GetPlayingTrackID),Playing);
 EntryPatch m(0x4E8410,Mission),md(bit_cast<uintptr_t>(&Access::CheckForMissionStatsChanges),Mission);
 EntryPatch r(0x4EB660,Retune),rd(bit_cast<uintptr_t>(&Access::CheckForStationRetune),Retune);
 EntryPatch pr(0x4EB890,PauseRetune),prd(bit_cast<uintptr_t>(&Access::CheckForStationRetuneDuringPause),PauseRetune);
 EntryPatch o(0x4F4ED0,VehicleOptions),od(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio),VehicleOptions);
 EntryPatch a(0x507210,Ambience),ad(bit_cast<uintptr_t>(&CAudioEngine::IsAmbienceTrackActive),Ambience);
 EntryPatch va(0x4E9800,VehicleActive),vad(bit_cast<uintptr_t>(&CAERadioTrackManager::IsVehicleRadioActive),VehicleActive);
 EntryPatch b(0x4D94A0,Bass),bd(bit_cast<uintptr_t>(&CAEAudioHardware::SetBassSetting),Bass);
 EntryPatch pl(0x4D8F10,Play),pld(bit_cast<uintptr_t>(&CAEAudioHardware::PlayTrack),Play);
 EntryPatch f(0x506EA0,Report),fd(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Report);
 EntryPatch pb(0x4EA640,Playback),pbd(bit_cast<uintptr_t>(&CAERadioTrackManager::StartTrackPlayback),Playback);
 EntryPatch v(0x4E9570,Advert),vd(bit_cast<uintptr_t>(&Access::ChooseAdvertIndex),Advert);
 EntryPatch u(0x4F3250,User),ud(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SelectUserTrackIndex),User);
 EntryPatch ac(0x4D8F80,Active),acd(bit_cast<uintptr_t>(&CAEAudioHardware::GetActiveTrackID),Active);
 EntryPatch mo(0x4F3330,Mode),mod(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::GetUserTrackPlayMode),Mode);
 EntryPatch mh(0x4E96C0,MusicHistory),mhd(bit_cast<uintptr_t>(&Access::AddMusicTrackIndexToHistory),MusicHistory);
 EntryPatch ih(0x4E9720,IdentHistory),ihd(bit_cast<uintptr_t>(&Access::AddIdentIndexToHistory),IdentHistory);
 EntryPatch ah(0x4E9760,AdvertHistory),ahd(bit_cast<uintptr_t>(&Access::AddAdvertIndexToHistory),AdvertHistory);
 EntryPatch dh(0x4E97B0,DJHistory),dhd(bit_cast<uintptr_t>(&Access::AddDJBanterIndexToHistory),DJHistory);
 EntryPatch st(0x4E9820,Stop),std(bit_cast<uintptr_t>(&CAERadioTrackManager::StopRadio),Stop);
 EntryPatch tm(0x4D8F60,Time),tmd(bit_cast<uintptr_t>(&CAEAudioHardware::GetTrackPlayTime),Time);
 EntryPatch hs(0x4D9870,HardwareService),hsd(bit_cast<uintptr_t>(&CAEAudioHardware::Service),HardwareService);
 EntryPatch pa(0x4EA590,Pause),pad(bit_cast<uintptr_t>(&Access::CheckForPause),Pause);
 EntryPatch vol(0x4EA010,Volume),vold(bit_cast<uintptr_t>(&CAERadioTrackManager::UpdateRadioVolumes),Volume);
 EntryPatch cc(0x4EA930,Concat),ccd(bit_cast<uintptr_t>(&Access::CheckForTrackConcatenation),Concat);
 EntryPatch sp(0x4D8F50,StopTrack),spd(bit_cast<uintptr_t>(&CAEAudioHardware::StopTrack),StopTrack);
 EntryPatch sr(0x4EB3C0,Start),srd(bit_cast<uintptr_t>(static_cast<void(CAERadioTrackManager::*)(eRadioID,eBassSetting,float,bool)>(&CAERadioTrackManager::StartRadio)),Start);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;draw=modeCalls=userCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());
    optionCalls=0;auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());manager=self;self->m_nMode=static_cast<eRadioTrackMode>(i%8);self->m_bInitialised=(i/8)%3==0;self->m_bBassEnhance=i%2;self->m_nStationsListed=self->m_nStationsListDown=0;auto& settings=self->m_ActiveSettings;settings=tRadioSettings(static_cast<eRadioID>((i/7)%14));self->m_RequestedSettings=tRadioSettings(static_cast<eRadioID>((i/11)%14));self->m_RequestedSettings.PlayTime=i%3==0?-1:123;self->m_RequestedSettings.BassGain=.75f;self->m_RequestedSettings.BassSetting=static_cast<eBassSetting>(i%3);
    for(size_t k=0;k<5;++k){settings.TrackQueue[k]=100+int32(k);settings.TrackTypes[k]=int8((i/13+k)%8);settings.TrackIndices[k]=int8(k+10);self->m_RequestedSettings.TrackQueue[k]=200+int32(k);self->m_RequestedSettings.TrackTypes[k]=int8((i/19+k)%8);self->m_RequestedSettings.TrackIndices[k]=int8(k+20);}
    if(i%5==0)settings.TrackQueue[1]=-1;settings.PrevTrackID=555;settings.PrevTrackType=int8((i/17)%8);settings.PrevTrackIdx=33;settings.PlayTime=-123;settings.BassGain=.5f;settings.BassSetting=static_cast<eBassSetting>(i%3);active=i%2?settings.TrackQueue[0]:900;
    static constexpr int32 times[]{-8,-7,-6,-2,-1,0,100};const auto input=times[(i/8)%std::size(times)];memset(&options,0xCD,sizeof(options));auto* gamepad=CPad::GetPad(0);memset(gamepad,0,sizeof(CPad));gamepad->NewState.m_bRadioTrackSkip=i%3==0?128:0;
    StaticRef<uint8>(0xB7CB48)=uint8((i/23)%2);StaticRef<uint8>(0xB7CB49)=uint8((i/47)%2);StaticRef<uint32>(0xB7CB84)=10000+uint32(i);StaticRef<uint32>(0xB7CB7C)=20000+uint32(i);StaticRef<int32>(0x8CBA68)=-999;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*,int32)>(fn.address)(self,input);
    if(!candidate&&(int(self->m_nMode)==3||!trace.empty()))++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"options",Hash(&options,sizeof(options))},{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),0x40)},{"previous",StaticRef<int32>(0x8CBA68)},{"pad",Hash(CPad::GetPad(0),sizeof(CPad))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xB7CB48),timer.data(),timer.size());StaticRef<int32>(0x8CBA68)=oldPrevious;memcpy(CPad::GetPad(0),pad.data(),pad.size());
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


