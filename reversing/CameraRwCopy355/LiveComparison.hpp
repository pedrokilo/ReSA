#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraRwCopy355Live {
inline json trace;inline uint32 scenario{};
inline void __fastcall Copy(CMatrix* out,void*,RwMatrix* matrix){trace.push_back({"copy",uintptr_t(out),uintptr_t(matrix)});std::memcpy(out,matrix,64);}
inline RwMatrix* __cdecl MatrixUpdate(RwMatrix* matrix){trace.push_back({"matrix_update",uintptr_t(matrix)});return matrix;}
inline RwFrame* __cdecl FrameUpdate(RwFrame* frame){trace.push_back({"frame_update",uintptr_t(frame)});return frame;}
inline RwFrame* __cdecl Ortho(RwFrame* frame){trace.push_back({"ortho",uintptr_t(frame)});return frame;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","CopyCameraMatrixToRWCam",0x50AFA0,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-rw-copy355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-rw-copy355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-rw-copy355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,52> savedGlobals{};std::memcpy(savedGlobals.data(),reinterpret_cast<void*>(0xB6FF90),52);const auto savedPosition=StaticRef<float>(0x8CCC7C),savedDirection=StaticRef<float>(0x8CCC78);
 EntryPatch a(0x59AD20,Copy),ad(bit_cast<uintptr_t>(&CMatrix::UpdateMatrix),Copy);
 EntryPatch b(0x7F18A0,MatrixUpdate),bd(bit_cast<uintptr_t>(&RwMatrixUpdate),MatrixUpdate);
 EntryPatch c(0x7F0910,FrameUpdate),cd(bit_cast<uintptr_t>(&RwFrameUpdateObjects),FrameUpdate);
 EntryPatch d(0x7F1170,Ortho),dd(bit_cast<uintptr_t>(&RwFrameOrthoNormalize),Ortho);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    alignas(RwCamera) std::array<uint8,sizeof(RwCamera)> camera{};alignas(RwFrame) std::array<uint8,sizeof(RwFrame)> frame{};At<void*>(camera.data(),4)=frame.data();self->m_pRwCamera=reinterpret_cast<RwCamera*>(camera.data());frame.fill(uint8(i*13));
    memset(&self->m_mCameraMatrix,0,sizeof(CMatrix));memset(&self->m_mCameraMatrixOld,0,sizeof(CMatrix));self->m_bResetOldMatrix=(i/2)%2!=0;
    self->m_mCameraMatrix.GetRight()=CVector{1,float(i%7)*.125f,0};self->m_mCameraMatrix.GetForward()=CVector{0,1,float(i%5)*.125f};self->m_mCameraMatrix.GetUp()=CVector{float(i%3)*.125f,0,1};self->m_mCameraMatrix.GetPosition()=CVector{float(i%13)-7,float(i%11)-6,float(i%9)-5};
    const std::array<float,6> thresholds{0,.00001f,.1f,-.1f,1,std::bit_cast<float>(0x7FC00001u)};StaticRef<float>(0x8CCC7C)=thresholds[(i/64)%6];StaticRef<float>(0x8CCC78)=thresholds[(i/384)%6];StaticRef<uint32>(0xB6FFC0)=uint32((i/4)%16)|0xABCD0000u;
    StaticRef<CVector>(0xB6FFB4)=self->m_mCameraMatrix.GetPosition()+CVector{i%3==0?10.f:i%3==1?.001f:0,0,0};StaticRef<CVector>(0xB6FFA8)=self->m_mCameraMatrix.GetForward()+CVector{0,i%3==0?10.f:i%3==1?.001f:0,0};StaticRef<CVector>(0xB6FF9C)=self->m_mCameraMatrix.GetUp();StaticRef<CVector>(0xB6FF90)=self->m_mCameraMatrix.GetRight();
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCamera*,bool)>(fn.address)(self,i%2!=0);
    return json{{"camera",Digest(raw.data(),raw.size())},{"frame",Digest(frame.data(),frame.size())},{"globals",Digest(reinterpret_cast<void*>(0xB6FF90),52)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 std::memcpy(reinterpret_cast<void*>(0xB6FF90),savedGlobals.data(),52);StaticRef<float>(0x8CCC7C)=savedPosition;StaticRef<float>(0x8CCC78)=savedDirection;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
