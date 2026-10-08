#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEScriptAudioEntity.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "../ProbeRuntime.hpp"
namespace ScriptLoaded355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x200> physical{};alignas(16) inline std::array<uint8,0x80> matrix{};
inline void __fastcall Cancel(void*,void*,int16 slot,bool stop){trace.push_back({"cancel",slot,stop});}
inline int16 __fastcall Playing(void*,void*,int16 slot){trace.push_back({"playing",slot});return int16(scenario%3-1);}
inline int8 __fastcall Bank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"bank",int(bank),int(slot)});return int8(scenario%4-1);}
inline bool __fastcall Sound(void*,void*,eSoundBank bank,eSoundID sound,eSoundBankSlot slot){trace.push_back({"sound",int(bank),int(sound),int(slot)});return (scenario/8)%2!=0;}
inline std::array<int8,65536> volumes;alignas(16) inline std::array<uint8,sizeof(CAESound)> requested;inline void* activeSelf{};
inline void __fastcall Init(CAESound* sound,void*,eSoundBankSlot bank,eSoundID id,CAEAudioEntity* owner,CVector pos,float volume,float rolloff,float speed,float doppler,uint8 delay,uint16 flags,float variance,int16 time){trace.push_back({"initialise",int(bank),int(id),owner==activeSelf,std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),std::bit_cast<uint32>(volume),std::bit_cast<uint32>(rolloff),std::bit_cast<uint32>(speed),std::bit_cast<uint32>(doppler),delay,flags,std::bit_cast<uint32>(variance),time});memset(sound,0,sizeof(*sound));sound->m_BankSlot=bank;sound->m_SoundID=id;sound->m_AudioEntity=owner;sound->m_CurrPos=pos;sound->m_Volume=volume;}
inline void __fastcall Flags(CAESound* sound,void*,uint16 flag,uint16 enabled){trace.push_back({"flags",flag,enabled,sound->m_Flags});if(enabled)sound->m_Flags|=flag;else sound->m_Flags&=uint16(~flag);}
inline CAESound* __fastcall Request(void*,void*,CAESound* sound){trace.push_back({"request",int(sound->m_BankSlot),int(sound->m_SoundID),std::bit_cast<uint32>(sound->m_Volume),std::bit_cast<uint32>(sound->m_CurrPos.x),std::bit_cast<uint32>(sound->m_CurrPos.y),std::bit_cast<uint32>(sound->m_CurrPos.z),sound->m_Flags});return reinterpret_cast<CAESound*>(requested.data());}
inline void __fastcall Destructor(void*,void*){trace.push_back({"destructor"});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","PlayLoadedMissionAudio",0x4EC270,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-loaded355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-loaded355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-loaded355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"script-loaded355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldVolumes=StaticRef<int8*>(0xBD00F8);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch d(0x4D8F00,Sound),dd(bit_cast<uintptr_t>(&CAEAudioHardware::GetSoundLoadingStatus),Sound);
 EntryPatch i(0x4EFE50,Init),id(bit_cast<uintptr_t>(&CAESound::Initialise),Init);
 EntryPatch f(0x4EF2B0,Flags),fd(bit_cast<uintptr_t>(&CAESound::SetFlags),Flags);
 EntryPatch r(0x4EFB10,Request),rd(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),Request);
 EntryPatch q(0x4EF660,Destructor),qu(0x4EF1A0,Destructor),qd(bit_cast<uintptr_t>(&CAESound::UnregisterWithPhysicalEntity),Destructor);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAEScriptAudioEntity)> object{};CVector position{float(i%11-5),float(i%7-3),float(i%5)};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(physical.data(),0,physical.size());memset(matrix.data(),0,matrix.size());auto* self=reinterpret_cast<CAEScriptAudioEntity*>(object.data());activeSelf=self;StaticRef<int8*>(0xBD00F8)=volumes.data();volumes.fill(int8(i%7==0?-128:i%9-5));
    *reinterpret_cast<uintptr_t*>(physical.data()+0x14)=(i/4)%2?uintptr_t(matrix.data()):0;*reinterpret_cast<CVector*>(physical.data()+4)=CVector{11.f,12.f,13.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{21.f,22.f,23.f};
    for(size_t w=0;w<4;++w){auto& link=self->wavLinks[w];link.m_pEntity=(i/13)%3==0?reinterpret_cast<CPhysical*>(physical.data()):nullptr;link.m_Sound=(i/7)%2?reinterpret_cast<CAESound*>(0x12345678):nullptr;link.m_nBankId=(i/3)%4==0?-1:100+i%3;link.m_nBankSlotId=(i/5)%3==0?-1:i%11;link.m_nAudioEvent=i%11==0?65535:400+i%9;link.m_vPosition=(i/11)%3==0?CVector{-1000.f,-1000.f,-1000.f}:(i/11)%3==1?CVector{0.f,-0.f,0.f}:position;}
    const uint8 slot=uint8(i%8);const auto validSlot=uint8(i%4);hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};intptr_t ret=0;
    reinterpret_cast<void(__thiscall*)(void*,uint8)>(fn.address)(self,slot);
    if(!candidate&&std::any_of(trace.begin(),trace.end(),[](const auto& e){return e[0]=="initialise";}))++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"physical",Hash(physical.data(),physical.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<int8*>(0xBD00F8)=oldVolumes;bool restored=StaticRef<int8*>(0xBD00F8)==oldVolumes;for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


