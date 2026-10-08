#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraDerived355Live {
inline json trace;inline uint32 scenario{};
inline CMatrix __cdecl Inverse(const CMatrix& in){trace.push_back({"inverse",std::bit_cast<uint32>(in.GetForward().x),std::bit_cast<uint32>(in.GetForward().y)});CMatrix out;memset(&out,0,sizeof(out));out.GetRight()=CVector{1,2,3};out.GetForward()=CVector{4,5,6};out.GetUp()=CVector{7,8,9};out.GetPosition()=CVector{10,11,12};return out;}
inline void __fastcall Frustum(CCamera* cam,void*,bool mirror){trace.push_back({"frustum",mirror});cam->m_fFrustumPlaneOffsets[0]=float(scenario)+float(mirror);}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","CalculateDerivedValues",0x5150E0,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-derived355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-derived355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-derived355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x59BDD0,Inverse),ad(bit_cast<uintptr_t>(static_cast<CMatrix(*)(const CMatrix&)>(&Invert)),Inverse);
 EntryPatch b(0x514D60,Frustum),bd(bit_cast<uintptr_t>(&CCamera::CalculateFrustumPlanes),Frustum);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    memset(&self->m_mCameraMatrix,0,sizeof(CMatrix));memset(&self->m_mMatInverse,0,sizeof(CMatrix));
    const std::array<float,8> values{0,-0.0f,1,-1,.0001f,100,std::bit_cast<float>(uint32(0x7FC00001)),std::bit_cast<float>(uint32(0x7F800000))};self->m_mCameraMatrix.GetForward()=CVector{values[(i/4)%8],values[(i/32)%8],float(i%17)};
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCamera*,bool,bool)>(fn.address)(self,i%2!=0,(i/2)%2!=0);
    return json{{"camera",Digest(raw.data(),raw.size())},{"norm_x",F(self->m_fCamFrontXNorm)},{"norm_y",F(self->m_fCamFrontYNorm)},{"orientation",F(self->m_fOrientation)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
