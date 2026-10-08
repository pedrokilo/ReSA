#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "BaseModelInfo.h"
#include "ColModel.h"
#include "WaterLevel.h"
#include "World.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamDead355Live {
inline json trace;inline uint32 scenario{},sphereCalls{};
inline void __fastcall Avoid(CCamera* cam,void*,const CVector* a,const CVector* b,CVector* out,float fov){trace.push_back({"avoid",uintptr_t(cam),std::bit_cast<uint32>(fov),std::bit_cast<uint32>(a->x),std::bit_cast<uint32>(a->y),std::bit_cast<uint32>(a->z),std::bit_cast<uint32>(b->z)});out->x+=float(scenario%3)*.125f;}
inline void __fastcall Bone(CPed*,void*,CVector* out,eBoneTag bone,bool update){trace.push_back({"bone",int(bone),update});*out=CVector{float(scenario%11)-5,float(scenario%7),float(scenario%5)};}
inline CEntity* __cdecl Sphere(CVector center,float radius,CEntity* ignore,bool b,bool v,bool p,bool o,bool d,bool c){trace.push_back({"sphere",std::bit_cast<uint32>(center.x),std::bit_cast<uint32>(center.y),std::bit_cast<uint32>(center.z),std::bit_cast<uint32>(radius),uintptr_t(ignore),b,v,p,o,d,c});return ((sphereCalls+++scenario)%3)==0?ignore:nullptr;}
inline bool __cdecl Water(float x,float y,float z,float& out,bool touch,CVector* normal){trace.push_back({"water",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z),touch,uintptr_t(normal)});out=float(scenario%9)-1;return scenario%3!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","ProcessPedsDeadBaby",0x519250,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-dead355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-dead355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-dead355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedTarget=StaticRef<uintptr_t>(0xB6F980),savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedTime=CTimer::GetTimeInMS(),savedDeath=StaticRef<uint32>(0xB6F094);const auto savedStep=CTimer::GetTimeStep();const auto savedFlag=StaticRef<uint8>(0xB6F055);std::array<uint8,8> savedState;std::memcpy(savedState.data(),reinterpret_cast<void*>(0xB70050),8);
 EntryPatch a(0x514030,Avoid);
 EntryPatch b(0x5E4280,Bone),bd(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),Bone);
 EntryPatch c(0x569E20,Sphere),cd(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),Sphere);
 EntryPatch w(0x6E8580,Water),wd(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    alignas(CPed) std::array<uint8,sizeof(CPed)> entity{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(CBaseModelInfo) std::array<uint8,sizeof(CBaseModelInfo)> model{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};
    auto* target=reinterpret_cast<CEntity*>(entity.data());entity[0x36]=uint8(i%5==0?4:i%2?2:3);At<int16>(entity.data(),0x22)=2000;At<uintptr_t>(entity.data(),0x14)=uintptr_t(matrix.data());At<uintptr_t>(model.data(),0x14)=uintptr_t(col.data());At<float>(col.data(),0x14)=float(i%7)*.25f;
    At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),0x10)=CVector{float(i%3)*.25f,1,float(i%2)*.125f};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%11)-5,float(i%7),float(i%5)};
    StaticRef<uintptr_t>(0xB6F980)=uintptr_t(target);StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());StaticRef<uint32>(0xB7CB84)=uint32(i%19==0?0x80000080u:5000u+uint32(i));StaticRef<uint32>(0xB6F094)=uint32(CTimer::GetTimeInMS()-uint32(i%4000));StaticRef<float>(0xB7CB5C)=.5f+float(i%5)*.25f;StaticRef<uint8>(0xB6F055)=1;std::memcpy(reinterpret_cast<void*>(0xB70050),savedState.data(),8);
    self->m_bResetStatics=i%3==0;self->m_vecSource=CVector{float(i%13)-6,float(i%17)-8,2.0f+float(i%7)*.25f};self->m_fFOV=float(i%80)+10;sphereCalls=0;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCam*)>(fn.address)(self);
    return json{{"cam",Digest(raw.data(),raw.size())},{"entity",Digest(entity.data(),entity.size())},{"state",Digest(reinterpret_cast<void*>(0xB70050),8)},{"death",StaticRef<uint32>(0xB6F094)},{"flag",StaticRef<uint8>(0xB6F055)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xB6F980)=savedTarget;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;StaticRef<uint32>(0xB7CB84)=savedTime;StaticRef<uint32>(0xB6F094)=savedDeath;StaticRef<float>(0xB7CB5C)=savedStep;StaticRef<uint8>(0xB6F055)=savedFlag;std::memcpy(reinterpret_cast<void*>(0xB70050),savedState.data(),8);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
