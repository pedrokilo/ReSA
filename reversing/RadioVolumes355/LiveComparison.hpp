#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEAudioHardware.h"
#include "AEAudioEnvironment.h"
#include "AudioEngine.h"
#include "../ProbeRuntime.hpp"
namespace RadioVolumes355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline CVector position;
inline int factorCalls{};
inline float Factor(bool effects){static constexpr float values[]{0.f,.1f,.5f,1.f,2.f,10.f,std::bit_cast<float>(0x7FC00000u)};const auto r=values[(scenario/5+(effects?0:1)+(factorCalls++/2)%3)%7];trace.push_back({effects?"effects":"music",std::bit_cast<uint32>(r)});return r;}
inline float __fastcall Effects(void*,void*){return Factor(true);}
inline float __fastcall Music(void*,void*){return Factor(false);}
inline bool __cdecl Cutscene(){const auto r=scenario%13==0;trace.push_back({"cutscene",r});return r;}
inline bool __fastcall Finished(void*,void*,uint8 slot){const auto r=(scenario+slot)%4==0;trace.push_back({"finished",slot,r});return r;}
inline int32 __fastcall Event(void*,void*,uint8 slot){const auto r=(scenario+slot)%5==0?65535:104;trace.push_back({"event",slot,r});return r;}
inline CVector* __fastcall Position(void*,void*,uint8 slot){const auto present=(scenario+slot)%3!=0;trace.push_back({"position",slot,present});return present?&position:nullptr;}
inline void __cdecl Relative(CVector* out,const CVector* source){static constexpr float values[]{0.f,14.9f,15.f,15.1f,30.f,std::bit_cast<float>(0x7FC00000u)};*out=CVector{values[(scenario/9)%6],0.f,0.f};trace.push_back({"relative",source==&position,std::bit_cast<uint32>(out->x)});}
inline bool __cdecl Ambience(){const auto r=scenario%3==0;trace.push_back({"ambience",r});return r;}
inline void __fastcall Channel(void*,void*,int16 channel,uint16 id,float volume,uint8 unused){trace.push_back({"channel",channel,id,std::bit_cast<uint32>(volume),unused});}
inline constexpr std::array<uintptr_t,4> globalAddresses{0xB7CB49,0xB7CB48,0xB6F065,0xB613E4};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","UpdateRadioVolumes",0x4EA010,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"radio-volumes355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"radio-volumes355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("radio-volumes355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"radio-volumes355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;std::array<uint8,4> saved{};for(size_t g=0;g<4;++g)saved[g]=StaticRef<uint8>(globalAddresses[g]);
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x4D9570,Effects),ad(bit_cast<uintptr_t>(&CAEAudioHardware::GetEffectsMasterScalingFactor),Effects);
 EntryPatch b(0x4D9580,Music),bd(bit_cast<uintptr_t>(&CAEAudioHardware::GetMusicMasterScalingFactor),Music);
 EntryPatch c(0x561AD0,Cutscene),cd(bit_cast<uintptr_t>(&CCutsceneMgr::IsRunning),Cutscene);
 EntryPatch d(0x5072C0,Finished),dd(bit_cast<uintptr_t>(&CAudioEngine::IsMissionAudioSampleFinished),Finished);
 EntryPatch e(0x5072D0,Event),ed(bit_cast<uintptr_t>(&CAudioEngine::GetMissionAudioEvent),Event);
 EntryPatch f(0x5072E0,Position),fd(bit_cast<uintptr_t>(&CAudioEngine::GetMissionAudioPosition),Position);
 EntryPatch g(0x4D80B0,Relative),gd(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetPositionRelativeToCamera_Vector),Relative);
 EntryPatch h(0x507280,Ambience),hd(bit_cast<uintptr_t>(&CAudioEngine::IsAmbienceRadioActive),Ambience);
 EntryPatch j(0x4D8870,Channel),jd(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelVolume),Channel);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;factorCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());
    for(size_t g=0;g<4;++g)StaticRef<uint8>(globalAddresses[g])=uint8((i/(1<<g))%2);
    self->m_bEnabledInPauseMode=(i/16)%2!=0;self->m_bBassEnhance=(i/32)%2!=0;self->m_ActiveSettings.BassSetting=static_cast<eBassSetting>((i/64)%5-1);self->m_HwClientHandle=i%17-8;
    static constexpr float levels[]{-50.f,-4.f,-.05f,0.f,1.f,std::bit_cast<float>(0x7FC00000u)};self->m_f80=levels[(i/3)%6];self->m_f84=levels[(i/18)%6];position=CVector{3.f,4.f,5.f};
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
    if(!candidate&&!trace.empty())++positiveCases;
    std::array<uint8,4> globals{};for(size_t g=0;g<4;++g)globals[g]=StaticRef<uint8>(globalAddresses[g]);
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"globals",globals},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 for(size_t g=0;g<4;++g)StaticRef<uint8>(globalAddresses[g])=saved[g];bool restored=true;for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

