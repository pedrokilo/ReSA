#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "MonsterTruck.h"
#include "../ProbeRuntime.hpp"
namespace MonsterControl355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline void __fastcall BaseControl(void*,void*){trace.push_back({"base_control"});}
inline void __fastcall BaseReset(void*,void*){trace.push_back({"base_reset"});}
inline void __fastcall BaseCheck(void*,void*,bool speed){trace.push_back({"base_check",speed});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,4> fns{{{"CMonsterTruck","ExtendSuspension",0x6C7D80,0,-1,1024},{"CMonsterTruck","ResetSuspension",0,0x8717D8,67,1024},{"CMonsterTruck","ProcessControl",0,0x8717D8,10,1024},{"CMonsterTruck","ProcessControlCollisionCheck",0,0x8717D8,23,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"monster-control355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"monster-control355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("monster-control355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"monster-control355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,4> hooks{};std::array<bool,4> enabled{},locked{};
 for(size_t n=0;n<4;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldStep=CTimer::GetTimeStep(),oldRate=fWheelExtensionRate;const auto oldClamp=StaticRef<uint8>(0x8D33B0);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto controlTarget=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CAutomobile","ProcessControl"))->GetHookOurAddress());
 EntryPatch c(0x6B1880,BaseControl),cd(controlTarget,BaseControl);
 const auto resetTarget=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CAutomobile","ResetSuspension"))->GetHookOurAddress());
 EntryPatch r(0x6A2AE0,BaseReset),rd(resetTarget,BaseReset);
 const auto checkTarget=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CAutomobile","ProcessControlCollisionCheck"))->GetHookOurAddress());
 EntryPatch k(0x6A29C0,BaseCheck),kd(checkTarget,BaseCheck);
 for(size_t n=0;n<4;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CMonsterTruck)> object{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());auto* self=reinterpret_cast<CMonsterTruck*>(object.data());*reinterpret_cast<uintptr_t*>(object.data())=0x8717D8;
    CTimer::ms_fTimeStep=float(i%9)*.25f;fWheelExtensionRate=float(i%7)*.03125f;StaticRef<uint8>(0x8D33B0)=uint8(i%2);
    self->m_fSuspensionRadius=float(i%13-3)*.125f;self->m_bWasPostponed=(i/17)%2;
    self->m_vecMoveSpeed=CVector{float(i%5-2)*.25f,0.f,0.f};self->m_vecTurnSpeed=CVector{0.f,float((i/5)%3-1)*.25f,0.f};
    for(size_t w=0;w<4;++w){self->m_wheelPosition[w]=float((i+int(w))%17-4)*.125f;self->m_aSuspensionLineLength[w]=float((i+int(w))%5)*.125f;self->m_aSuspensionSpringLength[w]=1.f+float((i+int(w))%5)*.25f;self->m_fWheelsSuspensionCompression[w]=float((i+int(w))%7)*.25f;}
    hooks[n]->State(candidate);const auto address=fn.address?fn.address:reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];const bool switched=fn.address?ReversingProbe::JumpsToDll(address)==candidate:(address!=std::array<uintptr_t,4>{0x6C7D80,0x6C7D40,0x6C8250,0x6C8330}[n])==candidate;
    if(!switched)return json{{"switch",false}};
    if(n==3)reinterpret_cast<void(__thiscall*)(void*,bool)>(address)(self,i%2);else reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 }
 CTimer::ms_fTimeStep=oldStep;fWheelExtensionRate=oldRate;StaticRef<uint8>(0x8D33B0)=oldClamp;bool restored=CTimer::GetTimeStep()==oldStep&&fWheelExtensionRate==oldRate&&StaticRef<uint8>(0x8D33B0)==oldClamp;
 for(size_t n=0;n<4;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


