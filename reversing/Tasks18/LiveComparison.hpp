#pragma once
// Banco dentro del motor: CTaskManager (lista de 18). Original (hooks de CTaskManager desactivados) frente a
// candidato con fixtures identicos: gestor falso en un buffer propio (0x30 bytes + 0x10 de guarda a 0xCD),
// peaton y evento falsos (solo se pasan como punteros) y 12 tareas falsas con vtable trazada (simples y complejas).
// Las complejas usan SetSubTask (0x61A430), GetSubTask e IsSimple reales del EXE; Create*SubTask, ControlSubTask,
// ProcessPed, MakeAbortable, StopTimer, GetTaskType y el destructor estan trazados y se configuran por tarea con
// presupuestos finitos (los arboles son aciclicos: una tarea solo crea o devuelve tareas de indice mayor).
// El destructor falso anota el contenido del gestor en el momento del borrado (orden borrar/poner a nulo).
// CTask::IsTaskPtr (assert de AddSubTasks en Debug) se sustituye por cierto sin traza: las tareas falsas no estan en el pool.
// Se compara el retorno, la memoria del gestor y de las tareas (punteros traducidos), la traza y el pool de tareas.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskManager.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Tasks18Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
constexpr int NF=12;
constexpr size_t FK=0x20;                     // tamano de cada tarea falsa
// configuracion por tarea falsa
constexpr size_t FK_TYPE=0x10, FK_ABORT=0x14, FK_FIRST=0x15, FK_NEXT=0x16, FK_CTRL=0x17,
                 FK_FIRST_N=0x18, FK_NEXT_N=0x19, FK_CTRL_N=0x1A, FK_PROC_BITS=0x1B, FK_PROC_CNT=0x1C;
alignas(16) inline std::array<uint8,0x40> mgr{};
alignas(16) inline std::array<uint8,0x10> ped{};
alignas(16) inline std::array<uint8,0x10> event{};
alignas(16) inline std::array<std::array<uint8,FK>,NF> fakes{};
inline std::array<void*,12> vtSimple{}, vtComplex{};
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(mgr.data(),mgr.size(),"mgr"))return *s; if(auto s=in(ped.data(),ped.size(),"ped"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s;
    for(int n=0;n<NF;++n)if(auto s=in(fakes[n].data(),FK,"fk"))return *s+"#"+std::to_string(n);
    if(v==uintptr_t(vtSimple.data()))return "vt_simple"; if(v==uintptr_t(vtComplex.data()))return "vt_complex";
    return v;
}
inline json Region(const void* p,size_t n,size_t from=0) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=from/4;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

inline uint8& B(CTask* t,size_t off) { return *reinterpret_cast<uint8*>(uintptr_t(t)+off); }
inline CTask* Sub(CTask* t) { return *reinterpret_cast<CTask**>(uintptr_t(t)+8); }
inline CTask* Pick(int8 idx) { return idx>=0&&idx<NF ? Fake(idx) : nullptr; }
// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete",Name(uintptr_t(t)),f,Region(mgr.data(),0x30)}); return t; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline void __fastcall FStop(CTask* t,void*,const void* e) { trace.push_back({"stop_timer",Name(uintptr_t(t)),Name(uintptr_t(e))}); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const void* e) { trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return B(t,FK_ABORT)!=0; }
inline bool __fastcall FProcess(CTask* t,void*,CPed* p) {
    const uint8 c=B(t,FK_PROC_CNT); const bool r=c<8&&((B(t,FK_PROC_BITS)>>c)&1); if(c<8)B(t,FK_PROC_CNT)=uint8(c+1);
    trace.push_back({"process_ped",Name(uintptr_t(t)),Name(uintptr_t(p)),r}); return r;
}
inline CTask* __fastcall FFirst(CTask* t,void*,CPed* p) {
    CTask* r{}; if(B(t,FK_FIRST_N)){--B(t,FK_FIRST_N); r=Pick(int8(B(t,FK_FIRST)));}
    trace.push_back({"create_first",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(r))}); return r;
}
inline CTask* __fastcall FNext(CTask* t,void*,CPed* p) {
    CTask* r{}; if(B(t,FK_NEXT_N)){--B(t,FK_NEXT_N); r=Pick(int8(B(t,FK_NEXT)));}
    trace.push_back({"create_next",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(r))}); return r;
}
// con subtarea nula devuelve nulo (el EXE llamaria a MakeAbortable sobre nulo)
inline CTask* __fastcall FControl(CTask* t,void*,CPed* p) {
    CTask* r=Sub(t);
    if(r&&B(t,FK_CTRL_N)){--B(t,FK_CTRL_N); const int8 c=int8(B(t,FK_CTRL)); r=c==-2 ? Sub(t) : Pick(c);}
    trace.push_back({"control_sub_task",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(r))}); return r;
}
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((vtSimple[N]=reinterpret_cast<void*>(&FTrap<N>)),...); ((vtComplex[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline bool __cdecl AlwaysTaskPtr(CTask*) { return true; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=18;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskManager","Constructor",0x6816A0,0,-1,128},
    {"CTaskManager","Destructor",0x6816D0,0,-1,256},
    {"CTaskManager","GetActiveTask",0x681720,0,-1,256},
    {"CTaskManager","FindActiveTaskByType",0x681740,0,-1,1024},
    {"CTaskManager","FindTaskByType",0x6817D0,0,-1,1024},
    {"CTaskManager","GetTaskSecondary",0x681810,0,-1,256},
    {"CTaskManager","HasTaskSecondary",0x681820,0,-1,512},
    {"CTaskManager","Flush",0x681850,0,-1,256},
    {"CTaskManager","FlushImmediately",0x6818A0,0,-1,512},
    {"CTaskManager","SetNextSubTask",0x681920,0,-1,1024},
    {"CTaskManager","StopTimers",0x6819A0,0,-1,256},
    {"CTaskManager","GetSimplestActiveTask",0x6819D0,0,-1,256},
    {"CTaskManager","AddSubTasks",0x681A30,0,-1,1024},
    {"CTaskManager","ParentsControlChildren",0x681A80,0,-1,1024},
    {"CTaskManager","SetTask",0x681AF0,0,-1,1024},
    {"CTaskManager","SetTaskSecondary",0x681B60,0,-1,1024},
    {"CTaskManager","ClearTaskEventResponse",0x681BD0,0,-1,256},
    {"CTaskManager","ManageTasks",0x681C10,0,-1,2048},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks18-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks18-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks18");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"tasks18-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    vtSimple[0]=reinterpret_cast<void*>(&FDtor); vtSimple[2]=reinterpret_cast<void*>(0x43E300); vtSimple[3]=reinterpret_cast<void*>(0x43E310);
    vtSimple[4]=reinterpret_cast<void*>(&FType); vtSimple[5]=reinterpret_cast<void*>(&FStop); vtSimple[6]=reinterpret_cast<void*>(&FAbort);
    vtSimple[7]=reinterpret_cast<void*>(&FProcess);
    vtComplex[0]=reinterpret_cast<void*>(&FDtor); vtComplex[2]=reinterpret_cast<void*>(0x421190); vtComplex[3]=reinterpret_cast<void*>(0x4211A0);
    vtComplex[4]=reinterpret_cast<void*>(&FType); vtComplex[5]=reinterpret_cast<void*>(&FStop); vtComplex[6]=reinterpret_cast<void*>(&FAbort);
    vtComplex[7]=reinterpret_cast<void*>(0x61A430); /* CTaskComplex::SetSubTask del EXE (la DLL la llama directamente: es final) */
    vtComplex[8]=reinterpret_cast<void*>(&FNext); vtComplex[9]=reinterpret_cast<void*>(&FFirst); vtComplex[10]=reinterpret_cast<void*>(&FControl);
    const auto occupancyStart=Occupancy();
    auto* poolBase=reinterpret_cast<uint8*>(GetTaskPool()); const int32 lf0=*reinterpret_cast<int32*>(poolBase+0xC);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch p1(bit_cast<uintptr_t>(&CTask::IsTaskPtr),AlwaysTaskPtr);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura; slots libres a 0xCD
                    { *reinterpret_cast<int32*>(poolBase+0xC)=lf0;
                      auto* stor=*reinterpret_cast<uint8**>(poolBase); auto* flags=*reinterpret_cast<uint8**>(poolBase+4); const int32 cap=*reinterpret_cast<int32*>(poolBase+8);
                      for(int32 k=0;k<cap;++k)if(flags[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1818u; if(!st)st=1; trace=json::array();
                    for(auto& b:ped)b=uint8(next()); for(auto& b:event)b=uint8(next());
                    // tareas falsas: simples o complejas; subtareas y tareas creadas siempre de indice mayor
                    std::array<bool,NF> complex{};
                    for(int n=0;n<NF;++n)complex[n]=n<NF-2 ? next()%5<3 : next()%4==0;
                    auto later=[&](int n)->int8{ return n<NF-1 ? int8(n+1+int(next()%uint32(NF-1-n))) : int8(-1); };
                    for(int n=0;n<NF;++n){
                        auto* t=Fake(n); std::memset(fakes[n].data(),0,FK);
                        *reinterpret_cast<void**>(fakes[n].data())=complex[n] ? vtComplex.data() : vtSimple.data();
                        *reinterpret_cast<int32*>(fakes[n].data()+FK_TYPE)=100+int32(next()%6);
                        B(t,FK_ABORT)=uint8(next()%3!=0);
                        B(t,FK_PROC_BITS)=uint8(next()); if(next()%4==0)B(t,FK_PROC_BITS)=0; B(t,FK_PROC_CNT)=0;
                        if(complex[n]){
                            B(t,FK_FIRST)=uint8(next()%5 ? later(n) : int8(-1)); B(t,FK_FIRST_N)=uint8(next()%4);
                            B(t,FK_NEXT)=uint8(next()%5 ? later(n) : int8(-1)); B(t,FK_NEXT_N)=uint8(next()%4);
                            { const uint32 m=next()%6; B(t,FK_CTRL)=uint8(m<2 ? int8(-2) : m==2 ? int8(-1) : later(n)); } B(t,FK_CTRL_N)=uint8(next()%3);
                        }
                    }
                    // arboles iniciales: subtarea de indice mayor y padre apuntando a la tarea compleja
                    for(int n=0;n<NF;++n)if(complex[n]&&next()%4){ const int8 j=later(n); if(j>=0){ *reinterpret_cast<CTask**>(fakes[n].data()+8)=Fake(j); *reinterpret_cast<CTask**>(fakes[j].data()+4)=Fake(n); } }
                    // gestor: huecos primarios y secundarios, peaton y guarda
                    std::memset(mgr.data(),0xCD,mgr.size());
                    auto M=[&](int k)->uint32&{return *reinterpret_cast<uint32*>(mgr.data()+4*k);};
                    for(int k=0;k<11;++k)M(k)=next()%5<2 ? uint32(uintptr_t(Fake(int(next()%NF)))) : 0;
                    if(next()%8==0)for(int k=0;k<11;++k)M(k)=0;
                    M(11)=uint32(uintptr_t(ped.data()));
                    // argumentos
                    auto anyFake=[&]()->CTask*{ return next()%6==0 ? nullptr : Fake(int(next()%NF)); };
                    auto complexFake=[&]()->CTask*{ std::vector<int> c; for(int n=0;n<NF;++n)if(complex[n])c.push_back(n); return c.empty()||next()%8==0 ? nullptr : Fake(c[next()%c.size()]); };
                    const int32 typeArg=next()%8==0 ? 999 : 100+int32(next()%6);
                    const int32 primIdx=int32(next()%5), secIdx=int32(next()%6);
                    CTask* taskArg=nullptr;
                    if(nm=="SetNextSubTask")taskArg=complexFake();
                    else if(nm=="HasTaskSecondary"){ const uint32 m=next()%4; taskArg=m==0 ? nullptr : m==1 ? reinterpret_cast<CTask*>(uintptr_t(M(int(5+next()%6)))) : Fake(int(next()%NF)); }
                    else if(nm=="SetTask"){ taskArg=next()%5==0 ? reinterpret_cast<CTask*>(uintptr_t(M(primIdx))) : anyFake(); }
                    else if(nm=="SetTaskSecondary"){ taskArg=next()%5==0 ? reinterpret_cast<CTask*>(uintptr_t(M(int(5+secIdx)))) : anyFake(); }
                    else taskArg=anyFake();
                    const void* evArg=next()%4 ? event.data() : nullptr;
                    const uint32 ctorPed=next()%4 ? uint32(uintptr_t(ped.data())) : next()%2 ? 0 : next();
                    const std::string fakesBefore=Hash(fakes.data(),sizeof(fakes));
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    void* self=mgr.data(); const uintptr_t target=fn.address; json ret=nullptr;
                    if(nm=="Constructor"){ std::memset(mgr.data(),0xCD,mgr.size()); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(self,ctorPed))); }
                    else if(nm=="Destructor"||nm=="Flush"||nm=="FlushImmediately"||nm=="ClearTaskEventResponse"||nm=="ManageTasks")reinterpret_cast<void(__thiscall*)(void*)>(target)(self);
                    else if(nm=="GetActiveTask"||nm=="GetSimplestActiveTask")ret=Name(uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self)));
                    else if(nm=="FindActiveTaskByType")ret=Name(uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(self,typeArg)));
                    else if(nm=="FindTaskByType")ret=Name(uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int32,int32)>(target)(self,primIdx,typeArg)));
                    else if(nm=="GetTaskSecondary")ret=Name(uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(self,secIdx)));
                    else if(nm=="HasTaskSecondary")ret=reinterpret_cast<bool(__thiscall*)(void*,CTask*)>(target)(self,taskArg);
                    else if(nm=="StopTimers")reinterpret_cast<void(__thiscall*)(void*,const void*)>(target)(self,evArg);
                    else if(nm=="SetTask")reinterpret_cast<void(__thiscall*)(void*,CTask*,int32,uint32)>(target)(self,taskArg,primIdx,next()%2);
                    else if(nm=="SetTaskSecondary")reinterpret_cast<void(__thiscall*)(void*,CTask*,int32)>(target)(self,taskArg,secIdx);
                    else reinterpret_cast<void(__thiscall*)(void*,CTask*)>(target)(self,taskArg); // SetNextSubTask, AddSubTasks, ParentsControlChildren
                    for(auto* h:hooks)h->State(false);
                    json fakeWords=json::array(); for(int n=0;n<NF;++n)fakeWords.push_back(Region(fakes[n].data(),FK));
                    json r={{"ret",ret},{"mgr",Region(mgr.data(),mgr.size())},{"fakes",fakeWords},{"fakes_changed",Hash(fakes.data(),sizeof(fakes))!=fakesBefore},
                            {"ped",Hash(ped.data(),ped.size())},{"event",Hash(event.data(),event.size())},{"trace",trace},
                            {"clean",Occupancy()==occ},{"switches",switches}};
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    *reinterpret_cast<int32*>(poolBase+0xC)=lf0;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart}});
}
}
