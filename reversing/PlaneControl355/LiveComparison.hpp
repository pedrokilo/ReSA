#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Plane.h"
#include "Pad.h"
#include "FxSystem.h"
#include "FxPrtMult.h"
#include "../ProbeRuntime.hpp"
namespace PlaneControl355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,sizeof(CPed)> driver{};alignas(16) inline std::array<uint8,sizeof(FxSystem_c)> smoke{},outputSmoke{};
inline RwMatrix particleMatrix{};
inline void __fastcall BaseControl(void*,void*){trace.push_back({"base_control"});}
inline void __fastcall Weapons(void*,void*){trace.push_back({"weapons"});}
inline void __fastcall Wheel(CDamageManager* self,void*,eCarWheel wheel,eCarWheelStatus status){trace.push_back({"wheel",int(wheel),int(status)});self->m_anWheelsStatus[wheel]=status;}
inline void __fastcall Composite(void*,void*,RwMatrix* out){trace.push_back({"composite"});*out=particleMatrix;}
inline void __fastcall Particle(void*,void*,const CVector& pos,const CVector& velocity,float time,const FxPrtMult_c& mult,float rotation,float light,float limit,bool local){trace.push_back({"particle",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),std::bit_cast<uint32>(velocity.x),std::bit_cast<uint32>(velocity.y),std::bit_cast<uint32>(velocity.z),std::bit_cast<uint32>(time),Hash(&mult,sizeof(mult)),std::bit_cast<uint32>(rotation),std::bit_cast<uint32>(light),std::bit_cast<uint32>(limit),local});}
inline void __fastcall Kill(void*,void*){trace.push_back({"kill"});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CPlane","ProcessControl",0,0x871948,10,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"plane-control355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-control355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-control355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"plane-control355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldStep=CTimer::GetTimeStep();const auto oldSmoke=StaticRef<FxSystem_c*>(0xA9AE20);std::array<uint8,2*sizeof(CPad)> oldPads{};memcpy(oldPads.data(),&CPad::Pads[0],oldPads.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto controlTarget=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CAutomobile","ProcessControl"))->GetHookOurAddress());
 EntryPatch c(0x6B1880,BaseControl),cd(controlTarget,BaseControl);
 EntryPatch w(0x6C21A0,Wheel),wd(bit_cast<uintptr_t>(&CDamageManager::SetWheelStatus),Wheel);
 EntryPatch v(0x6E3950,Weapons),vd(bit_cast<uintptr_t>(&CVehicle::ProcessWeapons),Weapons);
 EntryPatch m(0x4AA8C0,Composite),md(bit_cast<uintptr_t>(&FxSystem_c::GetCompositeMatrix),Composite);
 EntryPatch p(0x4AA440,Particle),pd(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),Particle);
 EntryPatch k(0x4AA3F0,Kill),kd(bit_cast<uintptr_t>(&FxSystem_c::Kill),Kill);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CPlane)> object{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(driver.data(),0,sizeof(driver));memset(&CPad::Pads[0],0,sizeof(CPad)*2);auto* self=reinterpret_cast<CPlane*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871948;
    self->SetStatus(i%3==0?STATUS_PLAYER:STATUS_PHYSICS);self->m_nModelIndex=std::array<uint16,5>{MODEL_CROPDUST,MODEL_STUNT,MODEL_SKIMMER,MODEL_VORTEX,MODEL_SHAMAL}[(i/3)%5];self->m_pDriver=i%7==0?nullptr:reinterpret_cast<CPed*>(driver.data());reinterpret_cast<CPed*>(driver.data())->m_nPedType=static_cast<ePedType>((i/7)%5);
    self->vehicleFlags.bEngineOn=(i/11)%2;self->vehicleFlags.bIsDrowning=(i/13)%2;self->m_bSmokeEjectorEnabled=(i/17)%2;
    for(int j=0;j<2;++j){CPad::Pads[j].NewState.ShockButtonR=int16((i+j)%2);CPad::Pads[j].OldState.ShockButtonR=int16((i/2+j)%2);}
    self->field_9A0=std::array<uint32,7>{0,1,65535,65536,0x7FC00000,0xFFFFFFFF,0x12345678}[i%7];self->m_pSmokeParticle=i%5==0?nullptr:reinterpret_cast<FxSystem_c*>(smoke.data());self->m_nSmokeTimer=std::array<uint32,6>{0,20,100,1000,0xFFFFFFFF,0x80000000}[(i/5)%6];
    self->m_vecMoveSpeed=CVector{float(i%11-5)*.125f,float(i%7-3)*.25f,float(i%5-2)*.125f};CTimer::ms_fTimeStep=float(i%9)*.25f;StaticRef<FxSystem_c*>(0xA9AE20)=reinterpret_cast<FxSystem_c*>(outputSmoke.data());particleMatrix.pos={float(i%13),-4.f,3.f};
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6C9260)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"pads",Hash(&CPad::Pads[0],sizeof(CPad)*2)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 }
 CTimer::ms_fTimeStep=oldStep;StaticRef<FxSystem_c*>(0xA9AE20)=oldSmoke;memcpy(&CPad::Pads[0],oldPads.data(),oldPads.size());bool restored=CTimer::GetTimeStep()==oldStep&&StaticRef<FxSystem_c*>(0xA9AE20)==oldSmoke&&memcmp(&CPad::Pads[0],oldPads.data(),oldPads.size())==0;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


