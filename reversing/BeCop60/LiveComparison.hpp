#pragma once
// Banco dentro del motor: CTaskComplexBeCop (Clone, Destructor, GetTaskType y constructor 0x4993D0).
// Original (hooks desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios.
// Clone: tarea con subtarea falsa (vtable trazada; el Clone falso anota cuantos slots del pool estan ocupados para
// comprobar que se clona ANTES de reservar la tarea nueva) o sin subtarea; m_nDir recorre 0..255 (byte, no bool).
// Destructor: subtarea y subtarea de policia falsas (se traza el borrado con indicador). Constructor: buffer con 0xCD,
// el EXE (0x4993D0) frente al constructor del DLL por new de colocacion; se compara el objeto entero sin vptr + tipo.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "TaskComplexBeCop.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace BeCop60Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int FAKES=4;
constexpr size_t FAKE_SIZE=0x50, FK_TYPE=0x48, FK_CLONE=0x4C, OBJ_SIZE=0x80;
constexpr uintptr_t VT_BECOP=0x85A428;
alignas(16) inline std::array<uint8,OBJ_SIZE> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
inline int32& PoolLast() { return At<int32>(PoolHdr(),0xC); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}
inline int OccupiedCount() { int n=0; for(int32 k=0;k<PoolCap();++k)n+=!(PoolStates()[k]&0x80); return n; }

inline std::array<void*,12> fakeVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(IsPoolTask(v))return "pool_task";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json SerTask(const void* p,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,bytes,true)}};
}

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t),OccupiedCount()}); return At<CTask*>(t,FK_CLONE); }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=4;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexBeCop","Clone",0,0x85A428,1,512},
    {"CTaskComplexBeCop","Constructor",0x4993D0,0,-1,512},
    {"CTaskComplexBeCop","Destructor",0x4994E0,0,-1,256},
    {"CTaskComplexBeCop","GetTaskType",0,0x85A428,4,256},
}};
// ganchos auxiliares que el codigo del EXE atraviesa (constructor de CTaskComplexWander, pool de tareas): se desactivan en el original
constexpr std::array<std::pair<const char*,const char*>,4> extraNames{{{"CTaskComplexWander","Constructor"},{"CTask","operator new"},{"CTask","operator delete"},{"CTaskPool","New"}}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"becop60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"becop60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("becop60");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"becop60-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f){ if(fns[f].address==0x4993D0)continue; /* constructor: no esta enganchado */
        if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} }
    std::vector<ReversibleHooks::ReversibleHook::Base*> all;
    for(auto* h:hooks)if(h)all.push_back(h);
    for(auto& [c,n]:extraNames)if(auto* h=FindHook(c,n))all.push_back(h);
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    const auto occupancyStart=Occupancy();
    const int32 lf0=PoolLast();
    const std::vector<uint8> statesStart(PoolStates(),PoolStates()+PoolCap());
    std::vector<bool> enabled,locked;
    for(auto* h:all){enabled.push_back(h->Hooked());locked.push_back(h->Locked());h->LockState(false);}
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
            auto run=[&](bool candidate)->json {
                PoolLast()=lf0;
                if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4B43u; if(!st)st=1; trace=json::array(); freshIds.clear();
                for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; }
                const auto occ=Occupancy();
                for(auto* h:all)h->State(candidate);
                json out; json ret=nullptr; std::vector<CTask*> roots;
                const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : fn.address==0x4993D0 ? true : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                out["switches"]=switches;
                auto* o=object.data();
                auto collectFresh=[&](CTask* r){
                    for(auto* h:all)h->State(false);
                    auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                    if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                };
                // objeto en el estado del EXE: bytes aleatorios, vptr del EXE, padre/subtarea y campos de la tarea
                auto setBeCop=[&](bool withSub){
                    for(size_t b=0;b<0x50;++b)o[b]=uint8(next());
                    At<uintptr_t>(o,0)=VT_BECOP; At<uint32>(o,4)=0; At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr;
                    { static const int32 ms[]{0,1,2,3,4,5,6,7,0x7FFFFFFF,-1}; At<int32>(o,0xC)=next()%4 ? int32(next()%8) : ms[next()%10]; }
                    o[0x10]=next()%2 ? uint8(next()) : uint8(next()%2);
                    At<CTask*>(o,0x28)=withSub ? Fake(1) : nullptr;
                };
                if(nm=="GetTaskType") {
                    setBeCop(next()%2);
                    ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                    out["object"]=Region(o,0x50,true);
                }
                else if(nm=="Clone") {
                    setBeCop(next()%4!=0);
                    { CTask* const cl[]{nullptr,Fake(2),Fake(3),Fake(0)}; At<CTask*>(Fake(1),FK_CLONE)=cl[next()%4]; }
                    auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o);
                    collectFresh(r); ret=Name(r);
                    out["object"]=Region(o,0x50,true); out["sub"]=Region(fakes[1].data(),FAKE_SIZE,true);
                }
                else if(nm=="Destructor") {
                    setBeCop(next()%4!=0);
                    reinterpret_cast<void(__thiscall*)(void*)>(target)(o);   // el valor de EAX no esta definido en el EXE (salta a 0x460D80)
                    for(auto* h:all)h->State(false);
                    out["object"]=Region(o,0x50,true);
                }
                else { // Constructor 0x4993D0 (EXE) frente al constructor del DLL
                    std::memset(o,0xCD,OBJ_SIZE);
                    const int32 ms=next()%3 ? int32(next()%8) : int32(next()); const uint8 dir=uint8(next()); CTask* sub=next()%3 ? Fake(next()%FAKES) : nullptr;
                    if(candidate){ ::new(static_cast<void*>(o)) CTaskComplexBeCop(static_cast<eMoveState>(ms),dir,sub); ret=Name(o); }
                    else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,CTask*)>(target)(o,ms,dir,sub));
                    for(auto* h:all)h->State(false);
                    const uintptr_t vp=At<uintptr_t>(o,0);
                    out["vptr_ok"]= candidate ? (vp!=VT_BECOP&&vp!=0) : vp==VT_BECOP;
                    out["object"]=Region(o,OBJ_SIZE,true); out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                }
                for(auto* h:all)h->State(false);
                const size_t traceBefore=trace.size();
                for(auto* t:roots)delete t;
                json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace;
                out["clean"]=Occupancy()==occ;
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    PoolLast()=lf0;
    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
    for(size_t k=0;k<all.size();++k){all[k]->State(enabled[k]);all[k]->LockState(locked[k]);}
    const bool restored=Occupancy()==occupancyStart;
    if(restored)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
