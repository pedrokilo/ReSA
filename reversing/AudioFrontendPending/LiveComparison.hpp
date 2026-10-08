#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEFrontendAudioEntity.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "../ProbeRuntime.hpp"

namespace AudioFrontendPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){ uint64 h=1469598103934665603ull; const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline uint32 scenario{}, requests{}, positions{}, stops{}, loads{}, twins{};
inline uintptr_t base{};
alignas(16) inline std::array<uint8,sizeof(CAESound)> resultSound{};
inline bool __fastcall Bank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"bank",int(bank),int(slot)});return (scenario/102)%2==0;}
inline bool __fastcall Loaded(void*,void*,eSoundBank bank,eSoundID id,eSoundBankSlot slot){trace.push_back({"loaded",int(bank),int(id),int(slot)});return (scenario/102)%3!=1;}
inline void __fastcall Load(void*,void*,eSoundBank bank,eSoundBankSlot slot){++loads;trace.push_back({"load",int(bank),int(slot)});}
inline int16 __fastcall Playing(void*,void*,int16 event,CAEAudioEntity* e){trace.push_back({"playing",event,uintptr_t(e)-base});return int16((scenario/204)%2);}
inline int16 __fastcall PlayingBank(void*,void*,int16 slot){trace.push_back({"playing_bank",slot});return int16((scenario/204)%2);}
inline void __fastcall Cancel(void*,void*,int16 event,CAEAudioEntity* e){trace.push_back({"cancel",event,uintptr_t(e)-base});}
inline bool __cdecl Probability(float f){trace.push_back({"probability",F(f)});return scenario&1;}
inline int32 __cdecl Random(int32 low,int32 high){trace.push_back({"random",low,high});return low+int32(scenario%uint32(high-low+1));}
inline CAESound* __fastcall Request(void*,void*,CAESound* s){++requests;trace.push_back({"request",int(s->m_BankSlot),int(s->m_SoundID),uintptr_t(s->m_AudioEntity)-base,F(s->m_CurrPos.x),F(s->m_CurrPos.y),F(s->m_CurrPos.z),F(s->m_Volume),F(s->m_RollOffFactor),F(s->m_Speed),F(s->m_Doppler),int(s->m_FrameDelay),int(s->m_Flags),F(s->m_SpeedVariance),int(s->m_PlayTime),int(s->m_Event),F(s->m_ClientVariable)});return scenario&1 ? reinterpret_cast<CAESound*>(resultSound.data()) : nullptr;}
inline void __fastcall Position(CAESound* s,void*,CVector p){++positions;trace.push_back({"position",F(p.x),F(p.y),F(p.z)});s->m_CurrPos=p;}
inline void __fastcall Stop(CAESound* s,void*){++stops;trace.push_back({"stop"});s->m_HasRequestedStopped=true;}
inline void __fastcall TwinInit(void* self,void*,eSoundBankSlot b,eSoundID a,eSoundID c,CAEAudioEntity* e,uint16 lo,uint16 hi,int16 pa,int16 pb){++twins;trace.push_back({"twin_init",uintptr_t(self)-base,int(b),int(a),int(c),uintptr_t(e)-base,lo,hi,pa,pb});*reinterpret_cast<int16*>(static_cast<uint8*>(self)+0x88)=1;}
inline void __fastcall TwinPlay(void* self,void*,CVector p,float v,float speed,float roll,float doppler,eSoundEnvironment flags){++twins;trace.push_back({"twin_play",uintptr_t(self)-base,F(p.x),F(p.y),F(p.z),F(v),F(speed),F(roll),F(doppler),int(flags)});}
inline void __fastcall TwinStop(void* self,void*){++twins;trace.push_back({"twin_stop",uintptr_t(self)-base});*reinterpret_cast<int16*>(static_cast<uint8*>(self)+0x88)=0;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,2> fns{{{"CAEFrontendAudioEntity","AddAudioEvent",0x4DD4A0,0,-1,2048},{"CAEFrontendAudioEntity","UpdateParameters",0,0x862E54,0,1024}}};
inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio-frontend-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio-frontend-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio-frontend-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    std::string only;{std::ifstream f(dir/"audio-frontend-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};std::array<bool,2> enabled{},locked{};
    for(size_t n=0;n<2;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    auto* clock=reinterpret_cast<uint8*>(0xB7CB48);std::array<uint8,0x40> savedClock{};memcpy(savedClock.data(),clock,savedClock.size());
    auto& volumes=*reinterpret_cast<int8**>(0xBD00F8);auto* savedVolumes=volumes;std::array<int8,45401> fakeVolumes{};for(size_t n=0;n<fakeVolumes.size();++n)fakeVolumes[n]=int8(int(n%61)-45);volumes=fakeVolumes.data();
    uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x4D88C0,Bank),ad(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),Bank);
        EntryPatch b(0x4D8EF0,Loaded),bd(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundLoaded),Loaded);
        EntryPatch c(0x4D88A0,Load),cd(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),Load);
        EntryPatch d(0x4EFB10,Request),dd(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),Request);
        EntryPatch e(0x4EF570,Playing),ed(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity),Playing);
        EntryPatch f(0x4EF520,PlayingBank),fd(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsPlayingInBankSlot),PlayingBank);
        EntryPatch g(0x4EFB90,Cancel),gd(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity),Cancel);
        EntryPatch h(0x4D9C80,Probability),hd(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),Probability);
        EntryPatch j(0x4D9C10,Random),jd(bit_cast<uintptr_t>(&CAEAudioUtility::GetRandomNumberInRange<int32>),Random);
        EntryPatch k(0x4EF880,Position),kd(bit_cast<uintptr_t>(&CAESound::SetPosition),Position);
        EntryPatch l(0x4EF850,Stop),ld(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),Stop);
        EntryPatch m(0x4F28A0,TwinInit),md(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::Initialise),TwinInit);
        EntryPatch o(0x4F2CB0,TwinPlay),od(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::PlayTwinLoopSound),TwinPlay);
        EntryPatch p(0x4F2960,TwinStop),pd(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::StopSoundAndForget),TwinStop);
        for(size_t n=0;n<2;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;
            uint32 failures=0,positiveRequests=0,positivePositions=0,positiveStops=0,positiveLoads=0,positiveTwins=0;
            for(int i=0;i<fn.cases;++i){
                alignas(16) std::array<uint8,sizeof(CAEFrontendAudioEntity)> object{};alignas(16) std::array<uint8,sizeof(CAESound)> voice{};
                auto run=[&](bool candidate){scenario=uint32(i);requests=positions=stops=loads=twins=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(voice.data(),0xCD,voice.size());base=uintptr_t(object.data());
                    memcpy(clock,savedClock.data(),savedClock.size());*reinterpret_cast<uint32*>(0xB7CB4C)=1000+uint32(i%11);*reinterpret_cast<uint32*>(0xB7CB84)=2000+uint32(i%2000);*reinterpret_cast<uint8*>(0xB7CB48)=uint8((i/102)%2);*reinterpret_cast<uint8*>(0xB7CB49)=uint8((i/204)%2);
                    object[0x7C]=uint8((i/306)%2);object[0x7D]=uint8(i%4);*reinterpret_cast<int16*>(object.data()+0x80)=int16((i/102)%14);*reinterpret_cast<int16*>(object.data()+0x82)=int16(28+(i/102)%4);
                    for(size_t offset=0x84;offset<=0x94;offset+=4)*reinterpret_cast<uint32*>(object.data()+offset)=offset<=0x8C ? 998u+uint32((i/102)%6) : 1500u+uint32((i/102)%5)*500;
                    *reinterpret_cast<uintptr_t*>(object.data()+0x98)=uintptr_t(voice.data());*reinterpret_cast<int16*>(object.data()+0x124)=int16((i/102)%2);*reinterpret_cast<int16*>(object.data()+0x1CC)=int16((i/204)%2);
                    auto* sound=reinterpret_cast<CAESound*>(voice.data());sound->m_Event=static_cast<eAudioEvents>(16+i%18);memset(voice.data()+8,0,4);
                    hooks[n]->State(candidate);const uintptr_t target=fn.slot<0 ? fn.address : reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];
                    const bool switches=fn.slot<0 ? ReversingProbe::JumpsToDll(fn.address)==candidate : ((target<0x400000||target>=0x1000000)==candidate);
                    if(!switches)return json{{"switch",false}};
                    if(n==0)reinterpret_cast<void(__thiscall*)(void*,eAudioEvents,float,float)>(target)(object.data(),static_cast<eAudioEvents>(i%102),float(i%17)-8.5f,float(i%9)*0.125f+0.5f);
                    else {const std::array<int16,12> pos{-1,0,1,349,350,351,100,175,299,32767,-32768,44};reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(target)(object.data(),i%31==0 ? nullptr : sound,pos[(i/18)%pos.size()]);}
                    if(!candidate){positiveRequests+=requests;positivePositions+=positions;positiveStops+=stops;positiveLoads+=loads;positiveTwins+=twins;}
                    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"sound",Hash(voice.data(),voice.size())},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;
                Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            const bool coverage=n==0 ? positiveRequests&&positiveLoads&&positiveTwins : positivePositions&&positiveStops;
            if(!coverage)++failures;totalFailures+=failures;
            Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",{{"accepted",coverage},{"requests",positiveRequests},{"positions",positivePositions},{"stops",positiveStops},{"loads",positiveLoads},{"twins",positiveTwins}}}});
        }
    }
    for(size_t n=0;n<2;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);}
    volumes=savedVolumes;memcpy(clock,savedClock.data(),savedClock.size());
    Write({{"event",totalFailures ? "error" : "complete"},{"pairs",pairs},{"state_restored",volumes==savedVolumes&&memcmp(clock,savedClock.data(),savedClock.size())==0},{"failures",totalFailures}});log.close();
}
}
