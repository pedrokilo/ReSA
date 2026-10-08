#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Train.h"
#include "Pools/VehiclePool.h"
#include "../ProbeRuntime.hpp"
namespace TrainRemove355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline uint8* storage{};inline uint8* slotFlags{};inline CVehicle* player{};
inline int Index(void* p){return int((static_cast<uint8*>(p)-storage)/0xA18);}
inline void __cdecl Remove(CEntity* p){trace.push_back({"remove",Index(p)});}
inline void __fastcall Delete(void* p,void*,uint32 flags){const auto index=Index(p);trace.push_back({"delete",index,flags});slotFlags[index]|=0x80;auto* train=static_cast<CTrain*>(p);if(train->m_pPrevCarriage)train->m_pPrevCarriage->m_pNextCarriage=train->m_pNextCarriage;if(train->m_pNextCarriage)train->m_pNextCarriage->m_pPrevCarriage=train->m_pPrevCarriage;train->m_pPrevCarriage=nullptr;train->m_pNextCarriage=nullptr;}
inline CVehicle* __cdecl Player(int32 id,bool remote){trace.push_back({"player",id,remote,player?Index(player):-1});return player;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","RemoveAllTrains",0x6F6AA0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-remove355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-remove355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-remove355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-remove355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& poolSlot=StaticRef<CVehiclePool*>(0xB74494);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x563280,Remove),rd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch p(0x56E0D0,Player),pd(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),Player);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0xA18*8> objects{};std::array<uint8,8> flags{};std::array<uintptr_t,6> pool{};std::array<uintptr_t,71> vtable{};vtable[0]=bit_cast<uintptr_t>(&Delete);storage=objects.data();
   auto run=[&](bool candidate){trace=json::array();memset(objects.data(),0xCD,objects.size());const auto count=i%9;
    pool={bit_cast<uintptr_t>(storage),bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CVehiclePool*>(pool.data());
    slotFlags=flags.data();
    for(int j=0;j<8;++j){auto* t=reinterpret_cast<CTrain*>(storage+j*0xA18);*reinterpret_cast<uintptr_t*>(t)=bit_cast<uintptr_t>(vtable.data());t->m_nVehicleType=(i+j)%7==0?VEHICLE_TYPE_AUTOMOBILE:VEHICLE_TYPE_TRAIN;flags[j]=uint8((i+j)%5?0:0x80);t->m_pPrevCarriage=nullptr;t->m_pNextCarriage=nullptr;}
    for(int j=0;j<count;++j){auto* t=reinterpret_cast<CTrain*>(storage+j*0xA18);if(flags[j]&0x80||!t->IsTrain())continue;if(j>0&&!(flags[j-1]&0x80)&&reinterpret_cast<CTrain*>(storage+(j-1)*0xA18)->IsTrain())t->m_pPrevCarriage=reinterpret_cast<CTrain*>(storage+(j-1)*0xA18);if(j+1<count&&!(flags[j+1]&0x80)&&reinterpret_cast<CTrain*>(storage+(j+1)*0xA18)->IsTrain())t->m_pNextCarriage=reinterpret_cast<CTrain*>(storage+(j+1)*0xA18);}
    const auto pi=(i/9)%10;player=pi<count&&!(flags[pi]&0x80)?reinterpret_cast<CVehicle*>(storage+pi*0xA18):nullptr;
    hooks[n]->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;if(!switched)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)()>(fn.address)();
    return json{{"switch",true},{"objects",Hash(objects.data(),sizeof(objects))},{"trace",trace},{"flags",Hash(flags.data(),flags.size())}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 }
 poolSlot=oldPool;bool restored=poolSlot==oldPool;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


