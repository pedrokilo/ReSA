#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEVehicleAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "../ProbeRuntime.hpp"
namespace AudioVehiclePendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},positive{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0xA00> vehicle{};alignas(16) inline std::array<uint8,0x80> transmission{},matrix{};
inline bool __fastcall Bank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"bank",int(bank),int(slot)});return scenario%5!=0;}
inline eSoundBankSlot __cdecl Slot(eSoundBank bank){++positive;trace.push_back({"slot",int(bank)});return static_cast<eSoundBankSlot>(scenario%11==0?-1:7+scenario%10);}
inline void __fastcall Cancel(void*,void*,int16 sound){++positive;trace.push_back({"cancel",sound});}
inline void __fastcall Aircraft(void*,void*,int16 kind,eSoundBankSlot slot,eSoundID sound,float volume,float speed){++positive;trace.push_back({"aircraft",kind,int(slot),int(sound),F(volume),F(speed)});}
inline void __fastcall Bicycle(void*,void*,int16 kind,eSoundBankSlot slot,eSoundID sound,float volume,float speed){++positive;trace.push_back({"bicycle",kind,int(slot),int(sound),F(volume),F(speed)});}
inline void __fastcall Generic(void*,void*,int16 kind,eSoundBankSlot slot,eSoundBank bank,eSoundID sound,float speed,float volume,float roll){++positive;trace.push_back({"generic",kind,int(slot),int(bank),int(sound),F(speed),F(volume),F(roll)});}
inline float __fastcall Tyre(void*,void*,float ratio){trace.push_back({"tyre",F(ratio)});return .25f+ratio*.5f;}
inline float __cdecl Log10(float value){trace.push_back({"log",F(value)});return value>0.f?value*.125f-2.f:-5.f;}
inline int16 __fastcall Brake(void*,void*){trace.push_back({"brake"});return int16((scenario/13)%3*127);}
inline int16 __fastcall Accel(void*,void*){trace.push_back({"accel"});return int16((scenario/17)%3*127);}
inline void __fastcall Normalise(CVector* p,void*){trace.push_back({"normalise",F(p->x),F(p->y),F(p->z)});const auto mag=std::sqrt(p->x*p->x+p->y*p->y+p->z*p->z);if(mag>0.f){p->x/=mag;p->y/=mag;p->z/=mag;}else p->x=1.f;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,4> fns{{{"CAEVehicleAudioEntity","ProcessDummyBicycle",0x4FFDC0,0,-1,1024},{"CAEVehicleAudioEntity","ProcessPlayerBicycle",0x500040,0,-1,1024},{"CAEVehicleAudioEntity","ProcessGenericJet",0x4FF900,0,-1,1024},{"CAEVehicleAudioEntity","ProcessDummyHovercraft",0x500F50,0,-1,1024}}};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio-vehicle-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio-vehicle-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio-vehicle-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"audio-vehicle-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,4> hooks{};std::array<bool,4> enabled{},locked{};
    for(size_t n=0;n<4;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    std::array<uint8,0x138> globals{};memcpy(globals.data(),reinterpret_cast<void*>(0xB6B9A0),globals.size());std::array<uint8,0x14> camera{};memcpy(camera.data(),reinterpret_cast<void*>(0xB6F02C),camera.size());
    uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x4D88C0,Bank),ad(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),Bank);
        EntryPatch b(0x4F4D10,Slot),bd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::RequestBankSlot),Slot);
        EntryPatch c(0x4F55C0,Cancel),cd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::CancelVehicleEngineSound),Cancel);
        EntryPatch d(0x4F93C0,Aircraft),dd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::PlayAircraftSound),Aircraft);
        EntryPatch e(0x4F9710,Bicycle),ed(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::PlayBicycleSound),Bicycle);
        EntryPatch f(0x4FAD40,Generic),fd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::UpdateGenericVehicleSound),Generic);
        EntryPatch g(0x4F60B0,Tyre),gd(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::GetBaseVolumeForBicycleTyre),Tyre);
        EntryPatch h(0x4D9E50,Log10),hd(bit_cast<uintptr_t>(&CAEAudioUtility::AudioLog10),Log10);
        EntryPatch j(0x540080,Brake),jd(bit_cast<uintptr_t>(&CPad::GetBrake),Brake);
        EntryPatch k(0x5403F0,Accel),kd(bit_cast<uintptr_t>(&CPad::GetAccelerate),Accel);
        EntryPatch l(0x59C910,Normalise),ld(bit_cast<uintptr_t>(&CVector::Normalise),Normalise);
        for(size_t n=0;n<4;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
            for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAEVehicleAudioEntity)> object{};alignas(16) std::array<uint8,sizeof(CAEVehicleAudioEntity::tVehicleParams)> params{};
                auto run=[&](bool candidate){scenario=uint32(i);positive=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(params.data(),0,params.size());memset(vehicle.data(),0,vehicle.size());memset(transmission.data(),0,transmission.size());memset(matrix.data(),0,matrix.size());memset(reinterpret_cast<void*>(0xB6B9A0),0,globals.size());memset(reinterpret_cast<void*>(0xB6F02C),0,camera.size());
                    auto* self=reinterpret_cast<CAEVehicleAudioEntity*>(object.data());auto* vp=reinterpret_cast<CAEVehicleAudioEntity::tVehicleParams*>(params.data());vp->Vehicle=reinterpret_cast<CVehicle*>(vehicle.data());vp->Transmission=reinterpret_cast<cTransmission*>(transmission.data());vp->Speed=float(i%41-20)*.25f;self->m_Entity=vp->Vehicle;self->m_DummyEngineBank=static_cast<eSoundBank>(100+i%4);self->m_PlayerEngineBank=static_cast<eSoundBank>(110);self->m_DummySlot=static_cast<eSoundBankSlot>(i%13==0?-1:5+i%14);self->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(i%4);self->m_IsWreckedVehicle=(i/47)%2;self->m_EventVolume=float(i%9-4);self->m_CurrentRotorFrequency=i%3==0?-1.f:float(i%13)*.25f;self->m_CurrentDummyEngineVolume=-5.f;
                    *reinterpret_cast<float*>(transmission.data()+0x58)=3.f;*reinterpret_cast<uintptr_t*>(vehicle.data()+0x14)=uintptr_t(matrix.data());*reinterpret_cast<uint16*>(vehicle.data()+0x22)=std::array<uint16,5>{519,520,577,592,400}[i%5];vehicle[0x36]=uint8((i/5)%2?0:8);vehicle[0x428]=uint8(i%7?0x10:0);vehicle[0x804]=uint8(i%5);vehicle[0x834]=uint8((i/9)%2);*reinterpret_cast<float*>(vehicle.data()+0x648)=float(i%11-5)*.25f;*reinterpret_cast<float*>(vehicle.data()+0x818)=i%2?6.f:0.f;*reinterpret_cast<float*>(vehicle.data()+0x820)=i%3?-.5f:.5f;*reinterpret_cast<float*>(vehicle.data()+0x988)=float(i%31-15)*.125f;*reinterpret_cast<float*>(vehicle.data()+0x9C4)=float(i%9-2)*.1f;
                    const float axis=float(i%9-4)*.125f;*reinterpret_cast<CVector*>(matrix.data())=CVector{1.f,0.f,axis};*reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{0.f,1.f,-axis};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{1.f+float(i%3),2.f,3.f};
                    for(int s=0;s<10;++s)*reinterpret_cast<int16*>(0xB6B9A0+s*4)=int16((i/5)%2?int(self->m_DummyEngineBank):-1);
                    StaticRef<float>(0xB6BA6C)=.5f;StaticRef<float>(0xB6BA70)=.75f;StaticRef<float>(0xB6BA90)=3.f;StaticRef<float>(0xB6BA94)=.5f;StaticRef<float>(0xB6B9F4)=2.f;StaticRef<float>(0xB6B9F8)=3.f;StaticRef<float>(0xB6B9FC)=4.f;StaticRef<uint8>(0xB6BAC8)=uint8(i%2);StaticRef<uint8>(0xB6BAC9)=uint8(i%2);StaticRef<float>(0xB6BACC)=i%4?0.f:2.f;StaticRef<float>(0xB6BAD0)=.5f;
                    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
                    if(n==2)reinterpret_cast<void(__thiscall*)(void*,bool,CAEVehicleAudioEntity::tVehicleParams&,float,float,float,float,float)>(fn.address)(self,i%11!=0,*vp,float(i%9)*.125f,i%3==0?.5f:0.f,i%7==0?.5f:0.f,-float(i%4),.75f+float(i%3)*.125f);
                    else reinterpret_cast<void(__thiscall*)(void*,CAEVehicleAudioEntity::tVehicleParams&)>(fn.address)(self,*vp);
                    if(!candidate&&positive)++positiveCases;return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"params",Hash(params.data(),params.size())},{"vehicle",Hash(vehicle.data(),vehicle.size())},{"transmission",Hash(transmission.data(),transmission.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"globals",Hash(reinterpret_cast<void*>(0xB6B9A0),globals.size())},{"camera",Hash(reinterpret_cast<void*>(0xB6F02C),camera.size())},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
        }
    }
    memcpy(reinterpret_cast<void*>(0xB6B9A0),globals.data(),globals.size());memcpy(reinterpret_cast<void*>(0xB6F02C),camera.data(),camera.size());bool restored=memcmp(reinterpret_cast<void*>(0xB6B9A0),globals.data(),globals.size())==0&&memcmp(reinterpret_cast<void*>(0xB6F02C),camera.data(),camera.size())==0;
    for(size_t n=0;n<4;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
    Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}
