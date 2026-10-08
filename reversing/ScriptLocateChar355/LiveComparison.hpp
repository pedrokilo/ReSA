#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RunningScript.h"
#include "Pools/ObjectPool.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace ScriptLocateChar355Live {
inline json trace;inline uint32 scenario{};
inline void __fastcall Collect(CRunningScript* s,void*,int16 n){trace.push_back({"collect",n});s->m_IP+=n;}
inline void __fastcall CompareTrace(CRunningScript*,void*,bool b){trace.push_back({"compare",b});}
inline __declspec(naked) void Compare(CRunningScript*,void*,bool){__asm {
 pushad
 push dword ptr [esp+36]
 mov ecx,dword ptr [esp+28]
 mov edx,dword ptr [esp+24]
 call CompareTrace
 popad
 ret 4
}}
inline void __cdecl Area(uint32 id,float x1,float y1,float x2,float y2,float z){trace.push_back({"area",id,std::bit_cast<uint32>(x1),std::bit_cast<uint32>(y1),std::bit_cast<uint32>(x2),std::bit_cast<uint32>(y2),std::bit_cast<uint32>(z)});}
inline bool __cdecl Stopped(CPed* p){trace.push_back({"stopped",uintptr_t(p)});return scenario%5!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,4> fns{{{"CRunningScript","LocateCharCommand",0x486D80,0,-1,4096},{"CRunningScript","LocateCharCarCommand",0x487420,0,-1,2048},{"CRunningScript","LocateCharCharCommand",0x4870F0,0,-1,2048},{"CRunningScript","LocateCharObjectCommand",0x487720,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-locate-char355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-locate-char355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-locate-char355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedPed=StaticRef<CPedPool*>(0xB74490);
 const auto savedObject=StaticRef<CObjectPool*>(0xB7449C);const auto savedVehicle=StaticRef<CVehiclePool*>(0xB74494);const auto savedParams=ScriptParams;const auto savedStep=CTimer::GetTimeStep(),savedUnclip=StaticRef<float>(0xB7CB54);
 EntryPatch a(0x464080,Collect),ad(bit_cast<uintptr_t>(&CRunningScript::CollectParameters),Collect);
 EntryPatch b(0x4859D0,Compare),bd(bit_cast<uintptr_t>(&CRunningScript::UpdateCompareFlag),Compare);
 EntryPatch c(0x485E00,Area),cd(bit_cast<uintptr_t>(static_cast<void(*)(uint32,float,float,float,float,float)>(&CTheScripts::HighlightImportantArea)),Area);
 EntryPatch st(0x486110,Stopped),stdll(bit_cast<uintptr_t>(&CTheScripts::IsPedStopped),Stopped);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CRunningScript) std::array<uint8,sizeof(CRunningScript)> raw{};auto* self=reinterpret_cast<CRunningScript*>(raw.data());self->m_IP=reinterpret_cast<uint8*>(0x1200);
    alignas(16) std::array<uint8,0x7C4*2> peds{};alignas(16) std::array<uint8,0xA18> vehicle{};alignas(16) std::array<uint8,0x19C> object{};
    alignas(16) std::array<uint8,sizeof(CPedPool)> pp{},vp{},op{};std::array<uint8,2> ps{};std::array<uint8,1> vs{},os{};
    At<void*>(pp.data(),0)=peds.data();At<void*>(pp.data(),4)=ps.data();At<int32>(pp.data(),8)=2;At<void*>(vp.data(),0)=vehicle.data();At<void*>(vp.data(),4)=vs.data();At<int32>(vp.data(),8)=1;At<void*>(op.data(),0)=object.data();At<void*>(op.data(),4)=os.data();At<int32>(op.data(),8)=1;
    StaticRef<CPedPool*>(0xB74490)=reinterpret_cast<CPedPool*>(pp.data());StaticRef<CObjectPool*>(0xB7449C)=reinterpret_cast<CObjectPool*>(op.data());StaticRef<CVehiclePool*>(0xB74494)=reinterpret_cast<CVehiclePool*>(vp.data());
    for(int j=0;j<2;++j)ps[j]=uint8(j+i%100);vs[0]=os[0]=uint8(i%100);ScriptParams.fill({});ScriptParams[0].iParam=ps[0];
    int command;bool threeD;
    if(fn.address==0x486D80){const auto v=(i/2)%12;command=v<6?0xEC+v:0xFE+(v-6);threeD=v>=6;ScriptParams[1].fParam=float(10);ScriptParams[2].fParam=float(20);ScriptParams[3].fParam=float(threeD?30:5);ScriptParams[4].fParam=float(5);ScriptParams[5].fParam=float(threeD?5:0);ScriptParams[6].fParam=float(5);ScriptParams[threeD?7:5].iParam=i%3?1:0;}
    else{const auto mode=(i/2)%6;const auto base2=fn.address==0x487420?0x202:fn.address==0x4870F0?0xF2:0x471;const auto base3=fn.address==0x487420?0x205:fn.address==0x4870F0?0x104:0x474;threeD=mode>=3;command=threeD?base3+mode-3:base2+mode;ScriptParams[1].iParam=fn.address==0x4870F0?256+ps[1]:vs[0];ScriptParams[2].fParam=float(5);ScriptParams[3].fParam=float(5);ScriptParams[4].fParam=float(threeD?5:0);ScriptParams[threeD?5:4].iParam=i%3?1:0;}
    const std::array<float,7> xs{4.5f,5,5.5f,10,14.5f,15,15.5f},ys{14.5f,15,15.5f,20,24.5f,25,25.5f},zs{24.5f,25,25.5f,30,34.5f,35,35.5f};const CVector pos{xs[(i/12)%7],ys[(i/84)%7],zs[(i/588)%7]};
    At<CVector>(peds.data(),4)=pos;At<CVector>(peds.data()+0x7C4,4)=CVector{10,20,30};At<CVector>(vehicle.data(),4)=CVector{10,20,30};At<CVector>(object.data(),4)=CVector{10,20,30};
    auto* ped=reinterpret_cast<CPed*>(peds.data());ped->bInVehicle=i%2!=0;ped->m_pVehicle=(i/24)%3==0?nullptr:reinterpret_cast<CVehicle*>(vehicle.data());auto* second=reinterpret_cast<CPed*>(peds.data()+0x7C4);second->bInVehicle=(i/48)%2!=0;second->m_pVehicle=(i/72)%2?reinterpret_cast<CVehicle*>(vehicle.data()):nullptr;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CRunningScript*,int32)>(fn.address)(self,command);
    return json{{"script",Digest(raw.data(),raw.size())},{"peds",Digest(peds.data(),peds.size())},{"vehicle",Digest(vehicle.data(),vehicle.size())},{"object",Digest(object.data(),object.size())},{"params",Digest(ScriptParams.data(),sizeof(ScriptParams))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<CPedPool*>(0xB74490)=savedPed;StaticRef<CObjectPool*>(0xB7449C)=savedObject;StaticRef<CVehiclePool*>(0xB74494)=savedVehicle;ScriptParams=savedParams;CTimer::SetTimeStep(savedStep);StaticRef<float>(0xB7CB54)=savedUnclip;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
