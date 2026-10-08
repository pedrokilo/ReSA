#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "../ProbeRuntime.hpp"
namespace CamCore355Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
{"CCam","Using3rdPersonMouseCam",0x50A850,0,-1,1024},
{"CCam","KeepTrackOfTheSpeed",0x509DF0,0,-1,1024}
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-core355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-core355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-core355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 std::array<uint8,0x34> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xB6FF5C),saved.size());const auto savedReset=*reinterpret_cast<uint8*>(0xB6F052),savedMouse=*reinterpret_cast<uint8*>(0xB6EC2E);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    for(size_t k=0;k<12;++k)*reinterpret_cast<float*>(0xB6FF5C+k*4)=float((i*19+int(k)*7)%73-36)*.125f;
    *reinterpret_cast<uint32*>(0xB6FF8C)=uint32(i%64)|0xABCD0000u;*reinterpret_cast<uint8*>(0xB6F052)=uint8((i/64)%2);*reinterpret_cast<uint8*>(0xB6EC2E)=uint8(i%2);
    self->m_nMode=eCamMode((i/2)%12);
    const CVector source{float(i)*.25f,float(i%31),-float(i%7)},target{-float(i)*.125f,float(i%17),float(i%13)},up{float(i%19)*.0625f,float(i%23),float(i%29)};
    const float alpha=float(i%127-63)*.125f,beta=float(i%89-44)*.25f,fov=float(i%97)*.125f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;bool result=false;
    if(fn.address==0x50A850)result=reinterpret_cast<bool(__thiscall*)(const CCam*)>(fn.address)(self);
    else reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,const CVector&,const CVector&,const float&,const float&,const float&)>(fn.address)(self,source,target,up,alpha,beta,fov);
    return json{{"bytes",ReversingProbe::Bytes(raw.data(),raw.size())},{"globals",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6FF5C),0x34)},{"result",result},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 memcpy(reinterpret_cast<void*>(0xB6FF5C),saved.data(),saved.size());*reinterpret_cast<uint8*>(0xB6F052)=savedReset;*reinterpret_cast<uint8*>(0xB6EC2E)=savedMouse;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
