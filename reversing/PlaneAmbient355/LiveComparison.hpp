#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Plane.h"
#include "Pools/VehiclePool.h"
#include "../ProbeRuntime.hpp"
namespace PlaneAmbient355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline uint8* storage{};
inline int Index(void* p){return int((static_cast<uint8*>(p)-storage)/0xA18);}
inline void __cdecl Remove(CEntity* p){trace.push_back({"remove",Index(p)});}
inline void __fastcall Delete(void* p,void*,uint32 flags){trace.push_back({"delete",Index(p),flags});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CPlane","SwitchAmbientPlanes",0x6CCC50,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"plane-ambient355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-ambient355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-ambient355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"plane-ambient355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldActive=CPlane::GenPlane_Active;auto& poolSlot=StaticRef<CVehiclePool*>(0xB74494);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x563280,Remove),rd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0xA18*8> objects{};std::array<uint8,8> flags{};std::array<uintptr_t,6> pool{};std::array<uintptr_t,71> vtable{};vtable[0]=bit_cast<uintptr_t>(&Delete);storage=objects.data();
   auto run=[&](bool candidate){trace=json::array();memset(objects.data(),0xCD,objects.size());const auto count=i%9;
    pool={bit_cast<uintptr_t>(storage),bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CVehiclePool*>(pool.data());
    CPlane::GenPlane_Active=(i/9)%2;
    for(int j=0;j<8;++j){auto* v=reinterpret_cast<CVehicle*>(storage+j*0xA18);*reinterpret_cast<uintptr_t*>(v)=bit_cast<uintptr_t>(vtable.data());v->m_nVehicleSubType=static_cast<eVehicleType>((i+j)%6);v->m_nCreatedBy=static_cast<eVehicleCreatedBy>((i/3+j)%3);flags[j]=uint8((i+j)%3?0:0x80);}
    hooks[n]->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;if(!switched)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(bool)>(fn.address)((i/18)%2);
    return json{{"switch",true},{"active",CPlane::GenPlane_Active},{"trace",trace},{"flags",Hash(flags.data(),flags.size())}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 }
 CPlane::GenPlane_Active=oldActive;poolSlot=oldPool;bool restored=CPlane::GenPlane_Active==oldActive&&poolSlot==oldPool;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


