#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEPedAudioEntity.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "PedIntelligence.h"
#include "AnimBlendAssociation.h"
#include "../ProbeRuntime.hpp"
namespace AudioPedPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},positive{};inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x800> ped{},vehicle{},target{};alignas(16) inline std::array<uint8,0x100> animA{},animB{},clump{};inline std::array<uintptr_t,64> vt{};
inline bool __fastcall Bank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"bank",int(bank),int(slot)});return scenario%7!=1;}
inline void __fastcall Load(void*,void*,eSoundBank bank,eSoundBankSlot slot){++positive;trace.push_back({"load",int(bank),int(slot)});}
inline bool __fastcall Loading(void*,void*){trace.push_back({"loading"});return scenario%3==0;}
inline int32 __cdecl Random(int32 lo,int32 hi){trace.push_back({"random",lo,hi});return lo+int32(scenario%uint32(hi-lo+1));}
inline void __fastcall Position(CAESound* s,void*,CVector p){s->m_CurrPos=p;}
inline void __fastcall Unregister(CAESound* s,void*){memset(reinterpret_cast<uint8*>(s)+8,0,4);}
inline void __fastcall Register(CAESound* s,void*,CEntity* e){trace.push_back({"register",uintptr_t(e)-uintptr_t(ped.data())});memcpy(reinterpret_cast<uint8*>(s)+8,&e,4);}
inline void __fastcall Collision(void*,void*,CEntity* a,CEntity* b,eSurfaceType sa,eSurfaceType sb,const CVector& p,const CVector* normal,float impact,float other,bool one,bool unknown){++positive;trace.push_back({"collision",uintptr_t(a)-uintptr_t(ped.data()),b?intptr_t(b)-intptr_t(target.data()):-1,int(sa),int(sb),F(p.x),F(p.y),F(p.z),normal==nullptr,F(impact),F(other),one,unknown});}
inline CAESound* __fastcall Request(void*,void*,CAESound* s){++positive;trace.push_back({"request",int(s->m_BankSlot),int(s->m_SoundID),uintptr_t(s->m_AudioEntity)-base,F(s->m_CurrPos.x),F(s->m_CurrPos.y),F(s->m_CurrPos.z),F(s->m_Volume),F(s->m_RollOffFactor),F(s->m_Speed),F(s->m_Doppler),int(s->m_FrameDelay),int(s->m_Flags),F(s->m_SpeedVariance),int(s->m_PlayTime),int(s->m_Event),F(s->m_ClientVariable)});return nullptr;}
inline bool __fastcall IsPlayer(void*,void*){trace.push_back({"is_player"});return scenario%11!=0;}
inline bool __fastcall Parachute(void*,void*){trace.push_back({"parachute"});return (scenario/16)%2;}
inline bool __fastcall Moving(void*,void*){trace.push_back({"vehicle_virtual39"});return scenario%2;}
inline CAnimBlendAssociation* __cdecl Association(RpClump*,const char* name){trace.push_back({"association",name});if(scenario%5==0)return nullptr;return reinterpret_cast<CAnimBlendAssociation*>(strcmp(name,"Para_decel")==0?animB.data():animA.data());}
inline void __fastcall Flap(void* self,void*,float v,float speed){++positive;trace.push_back({"flap",uintptr_t(self)-base,F(v),F(speed)});}
inline void __fastcall StopTwin(void* self,void*){++positive;trace.push_back({"stop_twin",uintptr_t(self)-base});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,4> fns{{{"CAEPedAudioEntity","HandlePedSwing",0x4E1A40,0,-1,1024},{"CAEPedAudioEntity","HandlePedHit",0x4E1CC0,0,-1,2048},{"CAEPedAudioEntity","HandlePedJacked",0x4E2350,0,-1,512},{"CAEPedAudioEntity","Service",0x4E2EE0,0,-1,1024}}};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio-ped-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio-ped-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio-ped-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"audio-ped-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,4> hooks{};std::array<bool,4> enabled{},locked{};
    for(size_t n=0;n<4;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    auto& clock=*reinterpret_cast<uint32*>(0xB7CB84);const auto savedClock=clock;auto& volumes=*reinterpret_cast<int8**>(0xBD00F8);auto* savedVolumes=volumes;std::array<int8,45401> fakeVolumes{};for(size_t i=0;i<fakeVolumes.size();++i)fakeVolumes[i]=int8(int(i%29)-20);volumes=fakeVolumes.data();
    uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x4D88C0,Bank),ad(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),Bank);
        EntryPatch b(0x4D88A0,Load),bd(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),Load);
        EntryPatch c(0x506D90,Loading),cd(bit_cast<uintptr_t>(&CAudioEngine::IsLoadingTuneActive),Loading);
        EntryPatch d(0x4D9C10,Random),dd(bit_cast<uintptr_t>(&CAEAudioUtility::GetRandomNumberInRange<int32>),Random);
        EntryPatch e(0x4EF880,Position),ed(bit_cast<uintptr_t>(&CAESound::SetPosition),Position);
        EntryPatch f(0x4EF1A0,Unregister),fd(bit_cast<uintptr_t>(&CAESound::UnregisterWithPhysicalEntity),Unregister);
        EntryPatch g(0x4EF820,Register),gd(bit_cast<uintptr_t>(&CAESound::RegisterWithPhysicalEntity),Register);
        EntryPatch h(0x4EFB10,Request),hd(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),Request);
        EntryPatch j(0x506EB0,Collision),jd(bit_cast<uintptr_t>(&CAudioEngine::ReportCollision),Collision);
        EntryPatch k(0x5DF8F0,IsPlayer),kd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
        EntryPatch l(0x6011B0,Parachute),ld(bit_cast<uintptr_t>(&CPedIntelligence::GetUsingParachute),Parachute);
        EntryPatch m(0x4D6870,Association),md(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,const char*)>(&RpAnimBlendClumpGetAssociation)),Association);
        EntryPatch o(0x4E2A90,Flap),od(bit_cast<uintptr_t>(&CAEPedAudioEntity::PlayShirtFlap),Flap);
        EntryPatch p(0x4F2960,StopTwin),pd(bit_cast<uintptr_t>(&CAETwinLoopSoundEntity::StopSoundAndForget),StopTwin);
        vt.fill(reinterpret_cast<uintptr_t>(&Moving));
        for(size_t n=0;n<4;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
            for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAEPedAudioEntity)> object{};base=uintptr_t(object.data());
                auto run=[&](bool candidate){scenario=uint32(i);positive=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(ped.data(),0,ped.size());memset(vehicle.data(),0,vehicle.size());memset(target.data(),0,target.size());memset(clump.data(),0,clump.size());clump[0]=2;memset(animA.data(),0,animA.size());memset(animB.data(),0,animB.size());
                    auto* self=reinterpret_cast<CAEPedAudioEntity*>(object.data());memset(reinterpret_cast<uint8*>(&self->m_tempSound)+8,0,4);self->m_pPed=reinterpret_cast<CPed*>(ped.data());clock=i%13==0?0xFFFFFF00u:100000u+uint32(i);*reinterpret_cast<int32*>(ped.data()+0x5A0)=n==1 ? (i/8)%48 : i%48;ped[0x718]=0;*reinterpret_cast<uintptr_t*>(ped.data()+0x18)=uintptr_t(clump.data());*reinterpret_cast<CVector*>(ped.data()+4)=CVector{1.f,2.f,3.f};
                    *reinterpret_cast<uintptr_t*>(ped.data()+0x47C)=i%7?uintptr_t(ped.data()):0;*reinterpret_cast<uint32*>(ped.data()+0x46C)=i%4==0?0x100:0;*reinterpret_cast<uintptr_t*>(ped.data()+0x58C)=i%5?uintptr_t(vehicle.data()):0;*reinterpret_cast<uintptr_t*>(ped.data()+0xFC)=i%6==0?uintptr_t(target.data()):0;
                    *reinterpret_cast<uintptr_t*>(vehicle.data())=uintptr_t(vt.data());*reinterpret_cast<int32*>(vehicle.data()+0x590)=(i/4)%3==0?9:(i/4)%3==1?0:4;vehicle[0x1B8]=uint8(i%3);target[0x36]=uint8(n==1 ? ((i/384)%3==0?3:2) :2);*reinterpret_cast<int32*>(ped.data()+0x530)=i%3?50:0;
                    const CVector motion{float(i%5)*0.125f,float((i/5)%7)*0.0625f,float((i/35)%5)*0.03125f};*reinterpret_cast<CVector*>(ped.data()+0x44)=motion;*reinterpret_cast<CVector*>(vehicle.data()+0x44)=motion;self->field_158=float(i%120)-105.f;
                    reinterpret_cast<CAnimBlendAssociation*>(animA.data())->m_BlendAmount=float(i%9)*0.25f-0.5f;reinterpret_cast<CAnimBlendAssociation*>(animB.data())->m_BlendAmount=float((i/9)%9)*0.25f-0.5f;
                    hooks[n]->State(candidate);const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;if(!switches)return json{{"switch",false}};
                    const uint32 delay=i%5?uint32(i%3000):0;
                    if(n==0)reinterpret_cast<void(__thiscall*)(void*,eAudioEvents,int32,uint32)>(fn.address)(self,static_cast<eAudioEvents>(60+i%9),60+(i/48)%12,delay);
                    else if(n==1)reinterpret_cast<void(__thiscall*)(void*,eAudioEvents,CPhysical*,uint8,float,uint32)>(fn.address)(self,static_cast<eAudioEvents>(61+i%8),(i/384)%3==2?nullptr:reinterpret_cast<CPhysical*>(target.data()),uint8(i%179),float(i%9)-4.5f,delay);
                    else if(n==2)reinterpret_cast<void(__thiscall*)(void*,eAudioEvents)>(fn.address)(self,static_cast<eAudioEvents>(68+i%7));else reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
                    if(!candidate&&positive)++positiveCases;
                    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"ped",Hash(ped.data(),ped.size())},{"vehicle",Hash(vehicle.data(),vehicle.size())},{"target",Hash(target.data(),target.size())},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;
                Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
        }
    }
    for(size_t n=0;n<4;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);}clock=savedClock;volumes=savedVolumes;
    Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",clock==savedClock&&volumes==savedVolumes}});log.close();
}
}
