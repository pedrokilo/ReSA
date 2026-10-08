#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraFrustum355Live {
inline json trace;inline uint32 scenario{};
inline void __cdecl Vectors(RwV3d* out,int32 count,const CMatrix&,const RwV3d* in){json input=json::array();for(int32 j=0;j<count;++j){input.push_back({std::bit_cast<uint32>(in[j].x),std::bit_cast<uint32>(in[j].y),std::bit_cast<uint32>(in[j].z)});out[j]={in[j].y+float(j),in[j].z-float(j),in[j].x+float(j)*.25f};}trace.push_back({"vectors",count,input});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","CalculateFrustumPlanes",0x514D60,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-frustum355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-frustum355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-frustum355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedFov=CDraw::GetFOV();const auto savedWidth=StaticRef<int32>(0xC17044),savedHeight=StaticRef<int32>(0xC17048);
 EntryPatch a(0x54EEA0,Vectors),ad(bit_cast<uintptr_t>(static_cast<void(*)(RwV3d*,int32,const CMatrix&,const RwV3d*)>(&TransformVectors)),Vectors);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};
    At<uintptr_t>(raw.data(),20)=(i/2)%2?uintptr_t(matrix.data()):0;At<CVector>(raw.data(),4)=CVector{float(i%17),float(i%11),float(i%7)};At<CVector>(matrix.data(),48)=CVector{-float(i%13),float(i%19),float(i%23)};
    const std::array<float,8> values{0,45,90,120,180,360,std::bit_cast<float>(uint32(0x7FC00001)),std::bit_cast<float>(uint32(0x7F800000))};CDraw::ms_fFOV=values[(i/4)%8];const std::array<int32,8> sizes{0,1,320,640,1024,1920,2147483647,-1};StaticRef<int32>(0xC17044)=sizes[(i/32)%8];StaticRef<int32>(0xC17048)=sizes[(i/256)%8];
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCamera*,bool)>(fn.address)(self,i%2!=0);
    return json{{"camera",Digest(raw.data(),raw.size())},{"normals",ReversingProbe::Bytes(self->m_avecFrustumNormals.data(),sizeof(self->m_avecFrustumNormals))},{"offsets",ReversingProbe::Bytes(self->m_fFrustumPlaneOffsets.data(),sizeof(self->m_fFrustumPlaneOffsets))},{"mirror_offsets",ReversingProbe::Bytes(self->m_fFrustumPlaneOffsets_Mirror.data(),sizeof(self->m_fFrustumPlaneOffsets_Mirror))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 CDraw::ms_fFOV=savedFov;StaticRef<int32>(0xC17044)=savedWidth;StaticRef<int32>(0xC17048)=savedHeight;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
