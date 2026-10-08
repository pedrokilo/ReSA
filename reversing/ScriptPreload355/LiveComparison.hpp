#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEScriptAudioEntity.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "../ProbeRuntime.hpp"
namespace ScriptPreload355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x200> physical{};alignas(16) inline std::array<uint8,0x80> matrix{};
inline void __fastcall Cancel(void*,void*,int16 slot,bool stop){trace.push_back({"cancel",slot,stop});}
inline int16 __fastcall Playing(void*,void*,int16 slot){trace.push_back({"playing",slot});return int16(scenario%3-1);}
inline int8 __fastcall Bank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"bank",int(bank),int(slot)});return int8(scenario%4-1);}
inline bool __fastcall Sound(void*,void*,eSoundBank bank,eSoundID sound,eSoundBankSlot slot){trace.push_back({"sound",int(bank),int(sound),int(slot)});return scenario%2!=0;}
inline bool __cdecl Map(const eAudioEvents& event,eSoundBankS32& bank,int32& sound,int32 slot){trace.push_back({"map",int(event),slot});const_cast<eAudioEvents&>(event)=static_cast<eAudioEvents>(int(event)+7);bank=eSoundBankS32{static_cast<eSoundBank>(50+scenario%9)};sound=scenario%4==0?-1:scenario%13;return scenario%5!=0;}
inline void __fastcall LoadBank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"loadbank",int(bank),int(slot)});}
inline void __fastcall LoadSound(void*,void*,eSoundBank bank,eSoundID sound,eSoundBankSlot slot){trace.push_back({"loadsound",int(bank),int(sound),int(slot)});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","PreloadMissionAudio",0x4EC190,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-preload355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-preload355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-preload355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"script-preload355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch b(0x4EF520,Playing),bd(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsPlayingInBankSlot),Playing);
 EntryPatch m(0x4D9CC0,Map),md(bit_cast<uintptr_t>(&CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent),Map);
 EntryPatch l(0x4D88A0,LoadBank),ld(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),LoadBank);
 EntryPatch q(0x4D8ED0,LoadSound),qd(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSound),LoadSound);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAEScriptAudioEntity)> object{};CVector position{float(i%11-5),float(i%7-3),float(i%5)};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(physical.data(),0,physical.size());memset(matrix.data(),0,matrix.size());auto* self=reinterpret_cast<CAEScriptAudioEntity*>(object.data());
    *reinterpret_cast<uintptr_t*>(physical.data()+0x14)=(i/4)%2?uintptr_t(matrix.data()):0;*reinterpret_cast<CVector*>(physical.data()+4)=CVector{11.f,12.f,13.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{21.f,22.f,23.f};
    for(size_t w=0;w<4;++w){auto& link=self->wavLinks[w];link.m_pEntity=(i/13)%3==0?reinterpret_cast<CPhysical*>(physical.data()):nullptr;link.m_Sound=(i/7)%2?reinterpret_cast<CAESound*>(0x12345678):nullptr;link.m_nBankId=(i/3)%4==0?-1:100+i%3;link.m_nBankSlotId=(i/5)%3==0?-1:i%11;link.m_nAudioEvent=400+i%9;link.m_vPosition=(i/11)%3==0?CVector{-1000.f,-1000.f,-1000.f}:(i/11)%3==1?CVector{0.f,-0.f,0.f}:position;}
    const uint8 slot=uint8(i%8);const auto validSlot=uint8(i%4);hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};intptr_t ret=0;
    reinterpret_cast<void(__thiscall*)(void*,uint8,int32)>(fn.address)(self,slot,1000+i);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"physical",Hash(physical.data(),physical.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

