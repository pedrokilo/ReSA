#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "WeaponInfo.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamFlyBy355Live {
inline json trace;inline uint32 scenario{};inline uintptr_t pathBase{};
inline void __cdecl Vector(CVector* out,float* path,float time,int32* index){const auto kind=(uintptr_t(path)-pathBase)/(sizeof(float)*64);trace.push_back({"vector",kind,std::bit_cast<uint32>(time),*index});*out=CVector{float(kind)*3+time*.001f,float(*index)*.125f,float(scenario%7)-3};}
inline void __cdecl Scalar(float* out,float* path,float time,int32* index){const auto kind=(uintptr_t(path)-pathBase)/(sizeof(float)*64);trace.push_back({"scalar",kind,std::bit_cast<uint32>(time),*index});*out=kind==0?70+float(scenario%7):float(int32(scenario%9)-4)*15;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","Process_FlyBy",0x5B25F0,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-flyby355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-flyby355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-flyby355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,16> savedPaths{};std::memcpy(savedPaths.data(),reinterpret_cast<void*>(0xB6F988),16);std::array<uint8,24> savedIndices{};std::memcpy(savedIndices.data(),reinterpret_cast<void*>(0xBC4074),24);const auto savedStep=CTimer::GetTimeStepNonClipped();const auto savedFinished=StaticRef<uint8>(0xB6F045),savedStarted=StaticRef<uint8>(0xB6F05B);const auto savedProgress=StaticRef<float>(0xB6F168);
 EntryPatch a(0x5B2090,Vector);EntryPatch b(0x5B2330,Scalar);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());std::array<std::array<float,64>,4> paths{};pathBase=uintptr_t(paths.data());
    for(size_t k=0;k<4;++k){paths[k][0]=3;paths[k][1]=0;for(size_t j=2;j<64;++j)paths[k][j]=float(j)*.5f;paths[k][2]=70;StaticRef<float*>(0xB6F988+k*4)=paths[k].data();}
    StaticRef<uint8>(0xB6F045)=uint8(i%7==0);StaticRef<uint8>(0xB6F05B)=uint8(i%3!=0);StaticRef<float>(0xBC4074)=45;StaticRef<float>(0xBC4078)=45;StaticRef<int32>(0xBC4080)=11;StaticRef<int32>(0xBC407C)=11;StaticRef<int32>(0xBC4088)=5;StaticRef<int32>(0xBC4084)=5;StaticRef<float>(0xB6F168)=-1;self->m_fTimeElapsedFloat=float((i/64)%7)*1000;self->m_nFinishTime=i%5==0?0:5000;CTimer::SetTimeStepNonClipped(float(i%5)*.5f);
    const CVector target{0,0,0};const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,float,float,float)>(fn.address)(self,target,1,2,3);
    return json{{"cam",Digest(raw.data(),raw.size())},{"paths",Digest(paths.data(),sizeof(paths))},{"indices",Digest(reinterpret_cast<void*>(0xBC4074),24)},{"started",StaticRef<uint8>(0xB6F05B)},{"progress",F(StaticRef<float>(0xB6F168))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 std::memcpy(reinterpret_cast<void*>(0xB6F988),savedPaths.data(),16);std::memcpy(reinterpret_cast<void*>(0xBC4074),savedIndices.data(),24);CTimer::SetTimeStepNonClipped(savedStep);StaticRef<uint8>(0xB6F045)=savedFinished;StaticRef<uint8>(0xB6F05B)=savedStarted;StaticRef<float>(0xB6F168)=savedProgress;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
