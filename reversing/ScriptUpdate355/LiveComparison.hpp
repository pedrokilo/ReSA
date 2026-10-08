#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEScriptAudioEntity.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "../ProbeRuntime.hpp"
namespace ScriptUpdate355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x200> physical{};alignas(16) inline std::array<uint8,0x80> matrix{};
inline std::array<int8,65536> volumes;
alignas(16) inline std::array<uint8,sizeof(CAESound)> soundBuffer;
inline void* activeSelf{};
inline void __fastcall Stop(CAESound* sound,void*){trace.push_back({"stop",sound==reinterpret_cast<CAESound*>(soundBuffer.data())});sound->m_Speed=-.25f;}
inline void __fastcall Position(CAESound* sound,void*,CVector position){trace.push_back({"position",std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z)});sound->m_CurrPos=position;}
inline void __fastcall Resident(void* owner,void*,eSoundBankSlot slot,eSoundBank bank,eSoundID id,eAudioEvents event,CVector& position,CPhysical* physicalEntity,float volume,float speed,int16 time,float distance){trace.push_back({"resident",owner==activeSelf,int(slot),int(bank),int(id),int(event),physicalEntity==reinterpret_cast<CPhysical*>(physical.data()),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),std::bit_cast<uint32>(volume),std::bit_cast<uint32>(speed),time,std::bit_cast<uint32>(distance)});}
inline void __fastcall BankSound(void* owner,void*,eAudioEvents event,CVector& position,CPhysical* physicalEntity,int16 id,uint8 slot,uint8 only,float volume,float distance,float speed){trace.push_back({"banksound",owner==activeSelf,int(event),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),physicalEntity==nullptr,id,slot,only,std::bit_cast<uint32>(volume),std::bit_cast<uint32>(distance),std::bit_cast<uint32>(speed)});}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto r=lo+scenario%(hi-lo+1);trace.push_back({"rng",lo,hi,r});return r;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","UpdateParameters",0,0x862E58,0,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-update355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-update355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-update355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"script-update355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;const auto oldVolumes=StaticRef<int8*>(0xBD00F8);const auto oldTime=StaticRef<uint32>(0xB7CB84);
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x4EF850,Stop),ad(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),Stop);
 EntryPatch b(0x4EF880,Position),bd(bit_cast<uintptr_t>(&CAESound::SetPosition),Position);
 EntryPatch c(0x4EC550,Resident),cd(bit_cast<uintptr_t>(&CAEScriptAudioEntity::PlayResidentSoundEvent),Resident);
 EntryPatch d(0x4EC6D0,BankSound),dd(bit_cast<uintptr_t>(&CAEScriptAudioEntity::PlayMissionBankSound),BankSound);
 EntryPatch e(0x4D9C10,Random),ed(bit_cast<uintptr_t>(&CAEAudioUtility::GetRandomNumberInRange<int32>),Random);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAEScriptAudioEntity)> object{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(physical.data(),0,physical.size());memset(matrix.data(),0,matrix.size());auto* self=reinterpret_cast<CAEScriptAudioEntity*>(object.data());
    *reinterpret_cast<uintptr_t*>(physical.data()+0x14)=(i/4)%2?uintptr_t(matrix.data()):0;*reinterpret_cast<CVector*>(physical.data()+4)=CVector{11.f,12.f,13.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{21.f,22.f,23.f};
    activeSelf=self;memset(soundBuffer.data(),0xCD,soundBuffer.size());auto* sound=reinterpret_cast<CAESound*>(soundBuffer.data());
    static constexpr int events[]{104,1086,1113,1118,1042,1167,1177,1147,900};sound->m_Event=events[i%9];sound->m_AudioEntity=reinterpret_cast<CAEAudioEntity*>(physical.data());sound->m_CurrPos=CVector{3.f,4.f,5.f};
    static constexpr float levels[]{-50.f,-40.f,-20.f,-18.f,-.05f,0.f,.05f,1.f,std::bit_cast<float>(0x7FC00000u)};sound->m_Volume=levels[(i/9)%9];sound->m_Speed=levels[(i/81)%9];
    for(size_t w=0;w<4;++w){auto& link=self->wavLinks[w];link.m_pEntity=(i/13)%2?reinterpret_cast<CPhysical*>(physical.data()):nullptr;link.m_Sound=size_t((i/4)%6)==w?sound:nullptr;}
    self->m_nLastTimeHornPlayed=(i/3)%2?1000u:0xFFFFFF00u;self->field_7C=uint8((i/7)%2);self->field_8C=std::array<float,4>{0.f,1.f,2.f,std::bit_cast<float>(0x7FC00000u)}[(i/11)%4];self->m_Volume=-7.f;self->m_Speed=.75f;
    for(size_t v=0;v<volumes.size();++v)volumes[v]=int8(int(v%41)-20);StaticRef<int8*>(0xBD00F8)=volumes.data();
    static constexpr uint32 offsets[]{0,299,300,301,499,500,501,4500,4501,4800,4801,5000};StaticRef<uint32>(0xB7CB84)=self->m_nLastTimeHornPlayed+offsets[(i/5)%12];
    hooks[n]->State(candidate);const auto entry=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((entry!=0x4EC970)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(entry)(self,(i/17)%8==0?nullptr:sound,(i/19)%3==0?-1:2);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"sound",Hash(soundBuffer.data(),soundBuffer.size())},{"volume_table",Hash(volumes.data(),volumes.size())},{"time",StaticRef<uint32>(0xB7CB84)},{"physical",Hash(physical.data(),physical.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<int8*>(0xBD00F8)=oldVolumes;StaticRef<uint32>(0xB7CB84)=oldTime;bool restored=true;for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

