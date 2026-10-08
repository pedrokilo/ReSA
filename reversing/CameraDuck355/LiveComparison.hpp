#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "../ProbeRuntime.hpp"
namespace CameraDuck355Live {
inline json trace;inline uint32 scenario{};alignas(CTaskSimpleDuck) inline std::array<uint8,sizeof(CTaskSimpleDuck)> task{};
inline CTaskSimpleDuck* __fastcall Duck(CPedIntelligence*,void*,bool simple){trace.push_back({"duck",simple});return scenario%3==0?nullptr:reinterpret_cast<CTaskSimpleDuck*>(task.data());}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
{"CCamera","HandleCameraMotionForDucking",0x50CFA0,0,-1,1024},
{"CCamera","HandleCameraMotionForDuckingDuringAim",0x50D090,0,-1,1024}
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-duck355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-duck355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-duck355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 const auto savedStep=CTimer::GetTimeStep(),savedStill=*reinterpret_cast<float*>(0x8CCB94),savedMoving=*reinterpret_cast<float*>(0x8CCB98);
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x6010A0,Duck),ad(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),Duck);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> camera{};camera.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(camera.data());
    alignas(CPed) std::array<uint8,sizeof(CPed)> raw{};auto* ped=reinterpret_cast<CPed*>(raw.data());*reinterpret_cast<void**>(raw.data()+0x47C)=reinterpret_cast<void*>(0x123400);ped->bIsDucking=(i/3)%2!=0;
    task.fill(0);task[0x19]=uint8((i/6)%2);scenario=uint32(i);trace=json::array();
    constexpr std::array<float,6> speeds{0.f,.0001f,.001f,.0010001f,.125f,.5f};ped->m_vecMoveSpeed=CVector{speeds[(i/12)%6],speeds[(i/24)%6],speeds[(i/48)%6]};
    *reinterpret_cast<float*>(0xB7CB5C)=float(i%9)*.125f;*reinterpret_cast<float*>(0x8CCB94)=float(i%7)*.125f;*reinterpret_cast<float*>(0x8CCB98)=float(i%11)*.0625f;
    self->m_fDuckCamMotionFactor=float(i%13-6)*.0625f;self->m_fDuckAimCamMotionFactor=float(i%17-8)*.0625f;
    CVector source{1.f,2.f,float(i)*.125f},target{-1.f,-2.f,-float(i)*.0625f};
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__thiscall*)(CCamera*,CPed*,CVector*,CVector*,bool)>(fn.address)(self,ped,i%4==0?nullptr:&source,i%5==0?nullptr:&target,(i/96)%2!=0);
    return json{{"camera",ReversingProbe::Bytes(camera.data()+0xC54,8)},{"source",ReversingProbe::Bytes(&source,sizeof(source))},{"target",ReversingProbe::Bytes(&target,sizeof(target))},{"ped",ReversingProbe::Bytes(raw.data(),raw.size())},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 *reinterpret_cast<float*>(0xB7CB5C)=savedStep;*reinterpret_cast<float*>(0x8CCB94)=savedStill;*reinterpret_cast<float*>(0x8CCB98)=savedMoving;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
