#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "../ProbeRuntime.hpp"
namespace CameraLinear355Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,7> fns{{
{"CCamera","VectorMoveLinear",0x50D160,0,-1,1024},
{"CCamera","VectorTrackLinear",0x50D1D0,0,-1,1024},
{"CCamera","ProcessVectorTrackLinear-0",0x50D350,0,-1,1024},
{"CCamera","ProcessVectorTrackLinear-1",0x516440,0,-1,1024},
{"CCamera","ProcessVectorMoveLinear-0",0x50D430,0,-1,1024},
{"CCamera","ProcessVectorMoveLinear-1",0x5164A0,0,-1,1024},
{"CCamera","ProcessFOVLerp-0",0x50D510,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-linear355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-linear355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-linear355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;const auto savedTime=CTimer::GetTimeInMS();
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    constexpr std::array<uint32,8> times{0,1,1000,0x7FFFFF,0x7FFFFFFF,0x80000000,0xFFFFFF00,0xFFFFFFFF};
    const auto time=times[i%times.size()];*reinterpret_cast<uint32*>(0xB7CB84)=time;
    const float now=float(time);
    self->m_fTrackLinearStartTime=now-float(i%7)*10.0f;self->m_fTrackLinearEndTime=now+float(i%5-2)*10.0f;
    self->m_fMoveLinearStartTime=self->m_fTrackLinearStartTime;self->m_fMoveLinearEndTime=self->m_fTrackLinearEndTime;
    self->m_vecTrackLinearEndPoint=CVector{float(i)*.125f,-float(i%31),float(i%19)};self->m_vecTrackLinearStartPoint=CVector{-float(i%73),float(i)*.0625f,float(i%29)};
    self->m_vecMoveLinearPosnStart=self->m_vecTrackLinearEndPoint;self->m_vecMoveLinearPosnEnd=self->m_vecTrackLinearStartPoint;
    self->m_bTrackLinearWithEase=self->m_bMoveLinearWithEase=i%2!=0;self->m_nZoomMode=uint8(i%3);
    self->m_fZoomInFactor=float(i%73)-10.f;self->m_fZoomOutFactor=float(i%37)*1.125f;
    self->m_bVecTrackLinearProcessed=self->m_bVecMoveLinearProcessed=self->m_bFOVLerpProcessed=false;
    self->m_bCameraPersistPosition=(i/2)%2!=0;self->m_bCameraPersistTrack=(i/4)%2!=0;
    CVector from=self->m_vecTrackLinearEndPoint,to=self->m_vecTrackLinearStartPoint;
    const float ratio=float(i%97-24)/48.f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    if(fn.address==0x50D160||fn.address==0x50D1D0)reinterpret_cast<void(__thiscall*)(CCamera*,CVector*,CVector*,float,bool)>(fn.address)(self,&to,&from,float(i%31-5)*20.f,i%2!=0);
    else if(fn.address==0x516440||fn.address==0x5164A0)reinterpret_cast<void(__thiscall*)(CCamera*)>(fn.address)(self);
    else reinterpret_cast<void(__thiscall*)(CCamera*,float)>(fn.address)(self,ratio);
    return json{{"bytes",ReversingProbe::Bytes(raw.data()+0xC5C,0xD00-0xC5C)},{"from",ReversingProbe::Bytes(&from,sizeof(from))},{"to",ReversingProbe::Bytes(&to,sizeof(to))},{"time",CTimer::GetTimeInMS()},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 *reinterpret_cast<uint32*>(0xB7CB84)=savedTime;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
