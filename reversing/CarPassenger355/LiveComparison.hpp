#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CarEnterExit.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventGroup.h"
#include "../ProbeRuntime.hpp"
namespace CarPassenger355Live {
inline json trace;inline uintptr_t base{},groupBase{};
inline int32 __cdecl Door(const CVehicle* vehicle,const CPed* ped){const auto index=(uintptr_t(ped)-base)/sizeof(CPed);trace.push_back({"door",index});return int32(index%4)+8;}
inline void __fastcall Ref(CEntity* e,void*,CEntity**){trace.push_back({"register",uintptr_t(e)});}
inline void __fastcall Clean(CEntity* e,void*,CEntity**){trace.push_back({"clean",uintptr_t(e)});}
inline CEvent* __fastcall Add(CEventGroup* self,void*,CEvent* event,bool valid){auto* e=static_cast<CEventPedEnteredMyVehicle*>(event);trace.push_back({"add",(uintptr_t(self)-groupBase)/0x300,uintptr_t(e->m_PedThatEntered),uintptr_t(e->m_Vehicle),int(e->m_TargetDoor),valid});return nullptr;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCarEnterExit","MakeUndraggedPassengerPedsLeaveCar",0x64F540,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"car-passenger355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"car-passenger355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car-passenger355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x64F110,Door),ad(bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit),Door);
 EntryPatch b(0x571B70,Ref),bd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
 EntryPatch c(0x571A00,Clean),cd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
 EntryPatch d(0x4AB420,Add),dd(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),Add);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};
    alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,9> peds{};
    alignas(16) std::array<std::array<uint8,0x300>,9> intelligence{};
    auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());veh->m_nMaxPassengers=uint8(i%9);
    for(size_t k=0;k<9;++k){auto* ped=reinterpret_cast<CPed*>(peds[k].data());*reinterpret_cast<void**>(peds[k].data()+0x47C)=intelligence[k].data();ped->bStayInCarOnJack=(i+k)%3==0;if(k<8)veh->m_apPassengers[k]=(i+k)%5==0?nullptr:ped;}
    base=uintptr_t(peds.data());groupBase=uintptr_t(intelligence.data())+0x68;trace=json::array();
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__cdecl*)(const CVehicle*,const CPed*,const CPed*)>(fn.address)(veh,i%2==0?reinterpret_cast<CPed*>(peds[i%8].data()):nullptr,reinterpret_cast<CPed*>(peds[8].data()));
    return json{{"vehicle",Digest(vehicle.data(),vehicle.size())},{"peds",Digest(peds.data(),sizeof(peds))},{"intelligence",Digest(intelligence.data(),sizeof(intelligence))},{"trace",trace},{"switches",switched}};
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
