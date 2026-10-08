#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "../ProbeRuntime.hpp"
namespace BikeBurst355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int scenario{},draw{};
inline float __cdecl RandomFloat(float lo,float hi){const auto result=(scenario%5-2)*.125f;trace.push_back({"random",std::bit_cast<uint32>(lo),std::bit_cast<uint32>(hi),std::bit_cast<uint32>(result)});return result;}
inline void __fastcall Audio(void*,void*,eAudioEvents event,float volume){trace.push_back({"audio",int(event),std::bit_cast<uint32>(volume)});}
inline void __cdecl RealPhysics(CVehicle* vehicle){trace.push_back({"real_physics"});vehicle->SetStatus(STATUS_PHYSICS);}
inline void __fastcall Move(void* raw,void*,CVector force){trace.push_back({"move",std::bit_cast<uint32>(force.x),std::bit_cast<uint32>(force.y),std::bit_cast<uint32>(force.z)});static_cast<CBike*>(raw)->m_vecMoveSpeed+=force*.125f;}
inline void __fastcall Turn(void* raw,void*,CVector force,CVector point){trace.push_back({"turn",std::bit_cast<uint32>(force.x),std::bit_cast<uint32>(force.y),std::bit_cast<uint32>(force.z),std::bit_cast<uint32>(point.x),std::bit_cast<uint32>(point.y),std::bit_cast<uint32>(point.z)});static_cast<CBike*>(raw)->m_vecTurnSpeed+=force*.125f;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","BurstTyre",0,0x871360,44,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-burst355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-burst355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-burst355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-burst355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x41BD90,RandomFloat),rd(bit_cast<uintptr_t>(static_cast<float(*)(float,float)>(&CGeneral::GetRandomNumberInRange<float>)),RandomFloat);
 EntryPatch a(0x4F6420,Audio),ad(bit_cast<uintptr_t>(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),Audio);
 EntryPatch p(0x423FC0,RealPhysics),pd(bit_cast<uintptr_t>(&CCarCtrl::SwitchVehicleToRealPhysics),RealPhysics);
 EntryPatch m(0x5429F0,Move),md(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),Move);
 EntryPatch t(0x542A50,Turn),td(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),Turn);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CPed)> driver{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(driver.data(),0,driver.size());auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data())=CVector{1.f,.25f,.125f};*reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{0.f,1.f,.25f};
    self->m_nPhysicalFlags=i%17==0?0x20000000u:0;self->vehicleFlags.bTyresDontBurst=i%19==0;self->SetStatus(static_cast<eEntityStatus>((i/7)%7));self->m_fMass=8.f;self->m_fTurnMass=16.f;self->m_vecMoveSpeed=CVector{float(i%9)*.125f,float(i%7)*.125f,i%31==0?std::bit_cast<float>(0x7FC00000u):.25f};self->m_vecTurnSpeed=CVector{0.f,0.f,0.f};self->m_pDriver=i%3==0?nullptr:reinterpret_cast<CPed*>(driver.data());
    static constexpr uint8 components[]{0,1,13,15,14,2};const auto component=components[(i/3)%std::size(components)];const auto mapped=component==13?0:component==15?1:component;object[0x65C+mapped]=uint8((i/11)%3);self->m_aRatioHistory[2]=i%5==0?std::bit_cast<float>(0x7FC00000u):float(i%3)*.5f;self->m_aRatioHistory[3]=float((i/13)%3)*.5f;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6BEB20)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<bool(__thiscall*)(void*,uint8,bool)>(address)(self,component,i%2!=0);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




