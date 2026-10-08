#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Pools/Pools.h"
#include "Pools/ObjectPool.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "ObjectSaveStructure.h"
#include "GenericGameStorage.h"
#include "../ProbeRuntime.hpp"
namespace PoolsPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,3> fns{{
 {"CPools","SaveObjectPool",0x5D4940,0,-1,1024},
 {"CPools","SavePedPool",0x5D4B40,0,-1,1024},
 {"CPools","SaveVehiclePool",0x5D4800,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}


alignas(16) inline std::array<uint8,16*0xA18> storage{};inline std::array<uint8,16> states{};inline std::array<uintptr_t,65> vtable{};inline int stride{},caseIndex{};
inline uint64 Hash(const void* data,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
inline int ID(void* o){return int((uintptr_t(o)-uintptr_t(storage.data()))/stride);}
inline bool __cdecl Save(void* data,int size){trace.push_back({"save",size,ReversingProbe::Bytes(data,size)});return caseIndex%3!=0;}
inline bool __fastcall SaveEntity(void* self,void*){trace.push_back({"entity",ID(self)});At<uint32>(self,0x30)^=0x11223344;return false;}
inline void __fastcall Construct(CObjectSaveStructure* self,void*,CObject* o){trace.push_back({"construct",ID(o)});memset(self,0x10+ID(o),sizeof(*self));}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"pools-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"pools-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pools-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 auto& op=*reinterpret_cast<CObjectPool**>(0xB7449C);auto& pp=*reinterpret_cast<CPedPool**>(0xB74490);auto& vp=*reinterpret_cast<CVehiclePool**>(0xB74494);auto* so=op;auto* sp=pp;auto* sv=vp;
 bool passed=true;uint32 pairs=0;
 {
 using ReversingProbe::EntryPatch;
 EntryPatch e1(0x5D1270,Save),d1(std::bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),Save),e2(0x5D2160,Construct),d2(std::bit_cast<uintptr_t>(&CObjectSaveStructure::Construct),Construct);
 vtable.fill(uintptr_t(&SaveEntity));
 for(size_t f=0;f<fns.size();++f){const auto& fn=fns[f];auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0,hasEntity=0,empty=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);trace=json::array();caseIndex=i;storage.fill(uint8(i));states.fill(0x80);stride=f==0?0x19c:f==1?0x7c4:0xa18;
 alignas(16) std::array<uint8,sizeof(CObjectPool)> pool{};At<void*>(pool.data(),0)=storage.data();At<void*>(pool.data(),4)=states.data();At<uint32>(pool.data(),8)=i%17;At<int32>(pool.data(),12)=-1;
 if(f==0)op=reinterpret_cast<CObjectPool*>(pool.data());else if(f==1)pp=reinterpret_cast<CPedPool*>(pool.data());else vp=reinterpret_cast<CVehiclePool*>(pool.data());
 for(int k=0;k<16;++k){auto* raw=storage.data()+stride*k;At<uintptr_t>(raw,0)=uintptr_t(vtable.data());states[k]=uint8((i+k)%4==0?0x80:((i+k)&0x7f));At<int16>(raw,0x22)=int16((i+k)%7==0?-123:400+k);
 if(f==0)At<uint8>(raw,0x13c)=uint8((i+k)%5);else if(f==1){At<uint32>(raw,0x46c)=((i+k)%3==0?0x100:0)|0x80000000u;At<int32>(raw,0x598)=(i+k)%5==0?0:4;}else{At<uint8>(raw,0x4a4)=uint8((i+k)%4);At<uintptr_t>(raw,0x460)=((i+k)%5==0)?0x1234:0;for(int n=0;n<8;++n)At<uintptr_t>(raw,0x464+n*4)=((i+k)%11==n)?0x5678:0;}}
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
 return json{{"result",result},{"storage",Hash(storage.data(),16*stride)},{"states",ReversingProbe::Bytes(states.data(),states.size())},{"pool",ReversingProbe::Bytes(pool.data(),pool.size())},{"trace",trace},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;hasEntity+=original["trace"].size()>1;empty+=original["trace"].size()==1;
 Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);const bool coverage=hasEntity&&empty;passed&=coverage;Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"saved",hasEntity},{"empty",empty},{"coverage",coverage}});
 }
 }
 op=so;pp=sp;vp=sv;Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",op==so&&pp==sp&&vp==sv}});
}
}
