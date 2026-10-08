#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RunningScript.h"
#include "ShotInfo.h"
#include "Pools/ObjectPool.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace ScriptAngled355Live {
inline json trace;inline uint32 scenario{};
inline void __fastcall Collect(CRunningScript* s,void*,int16 n){trace.push_back({"collect",n});s->m_IP+=n;}
inline void __fastcall Compare(CRunningScript*,void*,bool b){trace.push_back({"compare",b});}
inline void __cdecl Area(uint32 id,float ax,float ay,float bx,float by,float dx,float dy,float cx,float cy,float z){trace.push_back({"angled",id,std::bit_cast<uint32>(ax),std::bit_cast<uint32>(ay),std::bit_cast<uint32>(bx),std::bit_cast<uint32>(by),std::bit_cast<uint32>(dx),std::bit_cast<uint32>(dy),std::bit_cast<uint32>(cx),std::bit_cast<uint32>(cy),std::bit_cast<uint32>(z)});}
inline bool __cdecl Flame(uint8 index,CVector& pos){trace.push_back({"flame",index});if(scenario%7==0||index>scenario%6)return false;pos=CVector{float(int32(scenario%17)-8),float(int32((scenario/17)%17)-8),float(int32(index)-2)};return true;}
inline bool __cdecl Stopped(CPed* p){trace.push_back({"stopped",uintptr_t(p)});return scenario%5!=0;}
inline bool __cdecl VehicleStopped(CVehicle* p){trace.push_back({"vehicle_stopped",uintptr_t(p)});return scenario%5!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,3> fns{{{"CRunningScript","CharInAngledAreaCheckCommand",0x487F60,0,-1,2048},{"CRunningScript","FlameInAngledAreaCheckCommand",0x488780,0,-1,2048},{"CRunningScript","ObjectInAngledAreaCheckCommand",0x4883F0,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-angled355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-angled355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-angled355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedPed=StaticRef<CPedPool*>(0xB74490);
 const auto savedObject=StaticRef<CObjectPool*>(0xB7449C);const auto savedVehicle=StaticRef<CVehiclePool*>(0xB74494);const auto savedParams=ScriptParams;const auto savedStep=CTimer::GetTimeStep(),savedUnclip=StaticRef<float>(0xB7CB54);
 EntryPatch a(0x464080,Collect),ad(bit_cast<uintptr_t>(&CRunningScript::CollectParameters),Collect);
 EntryPatch b(0x4859D0,Compare),bd(bit_cast<uintptr_t>(&CRunningScript::UpdateCompareFlag),Compare);
 EntryPatch c(0x485EF0,Area),cd(bit_cast<uintptr_t>(&CTheScripts::HighlightImportantAngledArea),Area);
 EntryPatch fl(0x739DE0,Flame),fldll(bit_cast<uintptr_t>(&CShotInfo::GetFlameThrowerShotPosn),Flame);
 EntryPatch st(0x486110,Stopped),stdll(bit_cast<uintptr_t>(&CTheScripts::IsPedStopped),Stopped);
 EntryPatch vst(0x4861F0,VehicleStopped),vstdll(bit_cast<uintptr_t>(&CTheScripts::IsVehicleStopped),VehicleStopped);
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
    int command;bool threeD;int offset;
    if(fn.address==0x487F60){command=0x5F6+(i/2)%12;threeD=command>=0x5FC;offset=1;}
    else if(fn.address==0x4883F0){command=i%2?0x8E4:0x8E3;threeD=command==0x8E4;offset=1;ScriptParams[0].iParam=os[0];}
    else{command=i%2?0x72E:0x72D;threeD=command==0x72E;offset=0;}
    const int end=offset+(threeD?3:2);
    const std::array<float,8> widths{0,1,-1,5,-5,.125f,10,100};
    ScriptParams[offset].fParam=float(float(int32(i%5)-2));ScriptParams[offset+1].fParam=float(float(int32((i/5)%5)-2));
    ScriptParams[end].fParam=float(ScriptParams[offset].fParam+float(int32((i/25)%7)-3));ScriptParams[end+1].fParam=float(ScriptParams[offset+1].fParam+float(int32((i/175)%7)-3));
    if(threeD){ScriptParams[offset+2].fParam=float(i%4?-2:2);ScriptParams[end+2].fParam=float(i%4?2:-2);}
    ScriptParams[offset+(threeD?6:4)].fParam=float(widths[(i/7)%8]);ScriptParams[offset+(threeD?7:5)].iParam=i%3==0?0:i%3==1?1:std::bit_cast<int32>(0x80000000u);
    const CVector pos{float(int32((i/3)%17)-8),float(int32((i/51)%17)-8),float(int32((i/867)%5)-2)};
    At<CVector>(peds.data(),4)=pos;At<CVector>(peds.data()+0x7C4,4)=CVector{10,20,30};At<CVector>(vehicle.data(),4)=pos+CVector{.25f,-.25f,0};At<CVector>(object.data(),4)=pos;
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
