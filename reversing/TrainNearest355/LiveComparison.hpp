#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Train.h"
#include "Pools/VehiclePool.h"
#include "../ProbeRuntime.hpp"
namespace TrainNearest355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","FindNearestTrain",0x6F7090,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-nearest355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-nearest355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-nearest355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-nearest355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& poolSlot=StaticRef<CVehiclePool*>(0xB74494);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0xA18*8> objects{};std::array<uint8,8> flags{};std::array<uintptr_t,6> pool{};alignas(16) std::array<std::array<uint8,0x80>,8> matrices{};
   auto run=[&](bool candidate){memset(objects.data(),0xCD,objects.size());memset(matrices.data(),0,sizeof(matrices));const auto count=i%9;auto* storage=objects.data();
    pool={std::bit_cast<uintptr_t>(storage),std::bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CVehiclePool*>(pool.data());
    for(int j=0;j<8;++j){auto* train=reinterpret_cast<CTrain*>(storage+j*0xA18);train->m_nVehicleType=static_cast<eVehicleType>((i+j)%7);train->trainFlags.bIsFrontCarriage=(i/3+j)%2;flags[j]=uint8((i+j)%3?0:0x80);const CVector position{i%11==0?1.f:float((i+j*3)%17-8),i%11==0?2.f:float(j%5-2),float(i+j)};*reinterpret_cast<CVector*>(storage+j*0xA18+4)=position;*reinterpret_cast<CVector*>(matrices[j].data()+0x30)=position;*reinterpret_cast<uintptr_t*>(storage+j*0xA18+0x14)=(i+j)%2?uintptr_t(matrices[j].data()):0;}
    if(i%17==0&&count)*reinterpret_cast<float*>(matrices[count-1].data()+0x30)=std::numeric_limits<float>::quiet_NaN();
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto* ret=reinterpret_cast<CTrain*(__cdecl*)(CVector,bool)>(fn.address)(CVector{float(i%13-6),float(i%7-3),float(i%19)},(i/9)%2);
    const auto index=ret?int32((reinterpret_cast<const uint8*>(ret)-storage)/0xA18)+1:0;
    return json{{"switch",true},{"return",index},{"objects",Hash(objects.data(),sizeof(objects))},{"matrices",Hash(matrices.data(),sizeof(matrices))},{"flags",Hash(flags.data(),flags.size())}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 poolSlot=oldPool;bool restored=poolSlot==oldPool;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


