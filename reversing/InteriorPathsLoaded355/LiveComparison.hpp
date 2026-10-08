#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "PathFind.h"
#include "BaseModelInfo.h"
#include "ColModel.h"
#include "../ProbeRuntime.hpp"
namespace InteriorPathsLoaded355Live {
inline json trace;inline uint32 rngValue{};
inline bool __fastcall Paths(CPathFind* p,void*,float minx,float maxx,float miny,float maxy){trace.push_back({"paths",uintptr_t(p),std::bit_cast<uint32>(minx),std::bit_cast<uint32>(maxx),std::bit_cast<uint32>(miny),std::bit_cast<uint32>(maxy)});return rngValue%2!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"InteriorGroup_c","ArePathsLoaded",0x595380,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-paths-loaded355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-paths-loaded355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-paths-loaded355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);
 EntryPatch p(0x44DD10,Paths),d(bit_cast<uintptr_t>(&CPathFind::AreNodesLoadedForArea),Paths);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(InteriorGroup_c) std::array<uint8,sizeof(InteriorGroup_c)> raw{};auto* self=reinterpret_cast<InteriorGroup_c*>(raw.data());
    alignas(CEntity) std::array<uint8,sizeof(CEntity)> entity{};
    alignas(CBaseModelInfo) std::array<uint8,sizeof(CBaseModelInfo)> model{};
    alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};
    alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};
    self->m_pEntity=reinterpret_cast<CEntity*>(entity.data());At<int16>(entity.data(),0x22)=2000;At<uintptr_t>(entity.data(),0x14)=uintptr_t(matrix.data());At<uintptr_t>(model.data(),0x14)=uintptr_t(col.data());StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());
    At<CVector>(col.data(),0)=CVector{float(i%13)-7,float(i%11)-6,float(i%9)-5};At<CVector>(col.data(),12)=CVector{float(i%17),float(i%15),float(i%14)};
    At<CVector>(matrix.data(),0)=CVector{1.f,float(i%3)*0.25f,0};At<CVector>(matrix.data(),0x10)=CVector{float(i%5)*0.25f,1.f,0};At<CVector>(matrix.data(),0x20)=CVector{float(i%7)*0.25f,0,1.f};At<CVector>(matrix.data(),0x30)=CVector{float(i)-500,float(i)*0.5f,-3};
    rngValue=uint32(i);trace=json::array();const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    const auto result=reinterpret_cast<int8(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"group",Digest(raw.data(),raw.size())},{"entity",Digest(entity.data(),entity.size())},{"matrix",Digest(matrix.data(),matrix.size())},{"result",int(result)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
