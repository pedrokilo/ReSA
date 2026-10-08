#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "WeaponInfo.h"
#include "WaterLevel.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamAttached355Live {
inline json trace;inline uint32 scenario{};
inline bool __cdecl Water(float x,float y,float z,float& out,uint8 touching,CVector* normals){trace.push_back({"water",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z),touching,uintptr_t(normals)});out=float((scenario/32)%7)-3;return scenario%3!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","Process_AttachedCam",0x512B10,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-attached355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-attached355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-attached355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,40> savedAttachment{};std::memcpy(savedAttachment.data(),reinterpret_cast<void*>(0xB6F960),40);std::array<uint8,32> savedWater{};std::memcpy(savedWater.data(),reinterpret_cast<void*>(0xB6F0AC),32);std::array<uint8,12> savedVelocity{};std::memcpy(savedVelocity.data(),reinterpret_cast<void*>(0xB7C508),12);const auto savedLook=StaticRef<uint8>(0xB6F054);const auto savedFlag=StaticRef<uint32>(0xB7CD68);
 EntryPatch a(0x6EB690,Water),ad(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());alignas(CEntity) std::array<std::array<uint8,sizeof(CEntity)>,2> entities{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};auto* attached=reinterpret_cast<CEntity*>(entities[0].data());auto* target=reinterpret_cast<CEntity*>(entities[1].data());At<uintptr_t>(entities[0].data(),0x14)=uintptr_t(matrix.data());At<CVector>(matrix.data(),0)=CVector{1,.125f,0};At<CVector>(matrix.data(),16)=CVector{-.125f,1,0};At<CVector>(matrix.data(),32)=CVector{0,0,1};At<CVector>(matrix.data(),48)=CVector{float(i%5),2,float(i%7)-3};target->GetPosition()=CVector{float((i/5)%7)-3,float((i/35)%7)-3,float((i/245)%7)-3};
    StaticRef<CEntity*>(0xB6F984)=attached;StaticRef<CEntity*>(0xB6F980)=target;StaticRef<CVector>(0xB6F960)=CVector{float(i%3)*.125f,.5f,.25f};StaticRef<CVector>(0xB6F96C)=CVector{float((i/3)%3)-1,float((i/9)%3)-1,1};StaticRef<float>(0xB6F978)=float(int32((i/27)%9)-4)*15;StaticRef<uint8>(0xB6F054)=uint8(i%2);StaticRef<CVector>(0xB7C508)=CVector{float(int32((i/64)%7)-3),float((i/448)%3),float(i%5)};std::memset(reinterpret_cast<void*>(0xB6F0AC),0x5A,32);StaticRef<uint32>(0xB7CD68)=uint32(i%3);
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCam*)>(fn.address)(self);
    return json{{"cam",Digest(raw.data(),raw.size())},{"entities",Digest(entities.data(),sizeof(entities))},{"matrix",Digest(matrix.data(),matrix.size())},{"waterGlobals",Digest(reinterpret_cast<void*>(0xB6F0AC),32)},{"flag",StaticRef<uint32>(0xB7CD68)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 std::memcpy(reinterpret_cast<void*>(0xB6F960),savedAttachment.data(),40);std::memcpy(reinterpret_cast<void*>(0xB6F0AC),savedWater.data(),32);std::memcpy(reinterpret_cast<void*>(0xB7C508),savedVelocity.data(),12);StaticRef<uint8>(0xB6F054)=savedLook;StaticRef<uint32>(0xB7CD68)=savedFlag;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
