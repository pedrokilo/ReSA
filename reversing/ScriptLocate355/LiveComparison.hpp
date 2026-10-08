#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RunningScript.h"
#include "Pools/ObjectPool.h"
#include "Pools/VehiclePool.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace ScriptLocate355Live {
inline json trace;inline uint32 scenario{};
inline void __fastcall Collect(CRunningScript* s,void*,int16 n){trace.push_back({"collect",n});s->m_IP+=n;}
inline void __fastcall Compare(CRunningScript*,void*,bool b){trace.push_back({"compare",b});}
inline void __cdecl Area(uint32 id,float x1,float y1,float x2,float y2,float z){trace.push_back({"area",id,std::bit_cast<uint32>(x1),std::bit_cast<uint32>(y1),std::bit_cast<uint32>(x2),std::bit_cast<uint32>(y2),std::bit_cast<uint32>(z)});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{{"CRunningScript","LocateCarCommand",0x487A20,0,-1,4096},{"CRunningScript","LocateObjectCommand",0x487D10,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-locate355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-locate355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-locate355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedObject=StaticRef<CObjectPool*>(0xB7449C);const auto savedVehicle=StaticRef<CVehiclePool*>(0xB74494);const auto savedParams=ScriptParams;const auto savedStep=CTimer::GetTimeStep(),savedUnclip=StaticRef<float>(0xB7CB54);
 EntryPatch a(0x464080,Collect),ad(bit_cast<uintptr_t>(&CRunningScript::CollectParameters),Collect);
 EntryPatch b(0x4859D0,Compare),bd(bit_cast<uintptr_t>(&CRunningScript::UpdateCompareFlag),Compare);
 EntryPatch c(0x485E00,Area),cd(bit_cast<uintptr_t>(static_cast<void(*)(uint32,float,float,float,float,float)>(&CTheScripts::HighlightImportantArea)),Area);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CRunningScript) std::array<uint8,sizeof(CRunningScript)> raw{};auto* self=reinterpret_cast<CRunningScript*>(raw.data());self->m_IP=reinterpret_cast<uint8*>(0x1200);
    alignas(16) std::array<uint8,0xA18*4> objects{};
    alignas(16) std::array<uint8,sizeof(CVehiclePool)> pool{};std::array<uint8,4> states{};
    const auto stride=fn.address==0x487A20?0xA18:0x19C;
    At<void*>(pool.data(),0)=objects.data();At<void*>(pool.data(),4)=states.data();At<int32>(pool.data(),8)=4;
    StaticRef<CObjectPool*>(0xB7449C)=reinterpret_cast<CObjectPool*>(pool.data());StaticRef<CVehiclePool*>(0xB74494)=reinterpret_cast<CVehiclePool*>(pool.data());
    alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};
    const int idx=i%4;states[idx]=uint8(i%128);ScriptParams.fill({});ScriptParams[0].iParam=idx*256+states[idx];
    const auto command=fn.address==0x487A20?0x1AD+(i/4)%4:(i/4)%2==0?0x4E5:0x4E6;const auto threeD=fn.address==0x487A20?command>=0x1AF:command==0x4E6;
    ScriptParams[1].fParam=float(10);ScriptParams[2].fParam=float(20);ScriptParams[3].fParam=float(threeD?30:5);ScriptParams[4].fParam=float(5);ScriptParams[5].fParam=float(threeD?5:0);ScriptParams[6].fParam=float(5);ScriptParams[threeD?7:5].uParam=(i/32)%3==0?0:(i/32)%3==1?1:0x80000000u;
    const std::array<float,7> xs{4.5f,5,5.5f,10,14.5f,15,15.5f};const std::array<float,7> ys{14.5f,15,15.5f,20,24.5f,25,25.5f};const std::array<float,7> zs{24.5f,25,25.5f,30,34.5f,35,35.5f};CVector pos{xs[(i/8)%7],ys[(i/56)%7],zs[(i/392)%7]};
    if(i%53==0)pos.x=std::bit_cast<float>(uint32(0x7FC00001));if(i%59==0)pos.y=std::bit_cast<float>(uint32(0x7FC00001));if(i%61==0)pos.z=std::bit_cast<float>(uint32(0x7FC00001));
    auto* object=objects.data()+stride*idx;At<CVector>(object,4)=pos;At<CVector>(matrix.data(),48)=pos;At<uintptr_t>(object,20)=(i/16)%2?uintptr_t(matrix.data()):0;
    CTimer::SetTimeStep(2);StaticRef<float>(0xB7CB54)=1;At<float>(object,0xD4)=float((i/128)%4)*.01f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CRunningScript*,int32)>(fn.address)(self,command);
    return json{{"script",Digest(raw.data(),raw.size())},{"objects",Digest(objects.data(),objects.size())},{"params",Digest(ScriptParams.data(),sizeof(ScriptParams))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<CObjectPool*>(0xB7449C)=savedObject;StaticRef<CVehiclePool*>(0xB74494)=savedVehicle;ScriptParams=savedParams;CTimer::SetTimeStep(savedStep);StaticRef<float>(0xB7CB54)=savedUnclip;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
