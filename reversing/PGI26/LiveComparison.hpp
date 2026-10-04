#pragma once
// Banco dentro del motor: CPedGroupIntelligence (lista de 25; FlushTasks ya validada en FixPeds). Original (hooks
// de la clase desactivados) frente a candidato con fixtures deterministas e identicos: la inteligencia en un buffer
// propio (0x2A0 + 0x20 de cola), grupos falsos (pertenencia en +8 con los 8 huecos), peatones falsos (solo el tipo
// de entidad en +0x36), eventos falsos (tipo, banderas y entidad origen en el propio objeto; GetEvent en +0x10),
// tareas falsas (tipo en +4), asignadores de tareas y de tareas por defecto falsos, todos con vtables trazadas y
// clones sacados de pools fijos. Se simulan y trazan: FindPlayerPed, FindPlayerWanted, CPedGroups::GetPedsGroup,
// CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent, CEventEditableResponse::ComputeResponseTaskType(grupo) y
// WillRespond, y CGroupEventHandler::ComputeEventResponseTasks (en la direccion del EXE y en el cuerpo de la DLL,
// resueltos antes de parchear). En SetDefaultTaskAllocatorType se cambia la vtable de los 6 asignadores estaticos
// del EXE (0xC09908-0xC0991C) durante la llamada y se restaura. Nunca se tocan grupos reales. La traza es un array
// fijo (sin reservas durante la llamada) que se convierte a json despues.
// Se compara la inteligencia (punteros traducidos), la cola, los clones creados, los objetos falsos, el retorno,
// la traza y el pool de tareas (m_LastFreeSlot restaurado y huecos libres con 0xCD en cada ejecucion).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGroupIntelligence.h"
#include "PedGroup.h"
#include "PedGroups.h"
#include "World.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventEditableResponse.h"
#include "Events/GroupEventHandler.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace PGI26Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }

// ======================= objetos falsos =======================
constexpr int NPED=12, NGRP=3, NEV=16, NEVC=32, NT=12, NTC=32, NTA=6, NDA=3;
constexpr size_t PEDSZ=0x40, GRPSZ=sizeof(CPedGroup), EVSZ=0x20, TSZ=0x20, TASZ=0x10, DASZ=0x10, INTEL=0x2A0, INTEL_BUF=0x2C0;
constexpr uintptr_t SDA=0xC09908; constexpr size_t SDA_N=6;   // asignadores por defecto estaticos del EXE
alignas(16) inline std::array<uint8,INTEL_BUF> intel{};
alignas(16) inline std::array<uint8,NPED*PEDSZ> peds{};
alignas(16) inline std::array<uint8,NGRP*GRPSZ> grps{};
alignas(16) inline std::array<uint8,NEV*EVSZ> evs{};
alignas(16) inline std::array<uint8,NEVC*EVSZ> evc{};
alignas(16) inline std::array<uint8,NT*TSZ> tasks{};
alignas(16) inline std::array<uint8,NTC*TSZ> tc{};
alignas(16) inline std::array<uint8,NTA*TASZ> tas{};
alignas(16) inline std::array<uint8,NDA*DASZ> das{};
alignas(16) inline std::array<uint8,0x40> wanted{};
inline uint8* PedB(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline CPed* Ped(int k) { return reinterpret_cast<CPed*>(PedB(k)); }
inline uint8* Grp(int k) { return grps.data()+size_t(k)*GRPSZ; }
inline uint8* Ev(int k) { return evs.data()+size_t(k)*EVSZ; }
inline uint8* Tk(int k) { return tasks.data()+size_t(k)*TSZ; }
inline uint8* TA(int k) { return tas.data()+size_t(k)*TASZ; }
inline uint8* DA(int k) { return das.data()+size_t(k)*DASZ; }
inline CPed*& Member(int g,int i) { return At<CPed*>(Grp(g),0xC+4*size_t(i)); }   // pertenencia en +8, huecos en +4
inline uint32& IW(size_t off) { return At<uint32>(intel.data(),off); }
inline size_t PairOff(int arr,int i) { return 0xC+size_t(arr)*0xA0+size_t(i)*0x14; }  // principal, secundario, script, por defecto

struct Rg { const void* p; size_t n; size_t stride; const char* nm; };
inline const std::array<Rg,11> regions{{
    {intel.data(),INTEL_BUF,0,"intel"},{peds.data(),peds.size(),PEDSZ,"ped"},{grps.data(),grps.size(),GRPSZ,"grp"},
    {evs.data(),evs.size(),EVSZ,"ev"},{evc.data(),evc.size(),EVSZ,"evc"},{tasks.data(),tasks.size(),TSZ,"task"},
    {tc.data(),tc.size(),TSZ,"tc"},{tas.data(),tas.size(),TASZ,"ta"},{das.data(),das.size(),DASZ,"da"},
    {wanted.data(),wanted.size(),0,"wanted"},{reinterpret_cast<const void*>(SDA),SDA_N*4,4,"sda"}}};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    for(auto& r:regions){ const auto b=uintptr_t(r.p); if(v<b||v>=b+r.n)continue; const auto o=v-b;
        if(r.stride)return std::string(r.nm)+"#"+std::to_string(o/r.stride)+"+"+std::to_string(o%r.stride);
        return std::string(r.nm)+"+"+std::to_string(o); }
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 { C_TDTOR, C_TCLONE, C_TTYPE, C_EDTOR, C_ECLONE, C_ETYPE, C_EAFFECT, C_EEDIT, C_ESRC, C_EINTR,
                     C_ADTOR, C_AALLOC, C_APROC, C_DALLOC, C_PRIO, C_CRTT, C_WILL, C_CERT, C_PGRP, C_FPP, C_FPW, C_TRAP };
inline constexpr const char* CODE_NAMES[]{"task_delete","task_clone","task_type","ev_delete","ev_clone","ev_type","ev_affects_group",
    "ev_editable","ev_source","ev_interruptible","ta_delete","ta_allocate","ta_process","da_allocate","takes_priority",
    "compute_response_type","will_respond","compute_event_response_tasks","peds_group","find_player_ped","find_player_wanted","trap"};
struct TR { uint32 code,a,b,c; };
inline std::array<TR,1024> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 code,uint32 a=0,uint32 b=0,uint32 c=0) { if(ntr<int(tr.size()))tr[ntr++]={code,a,b,c}; else trOverflow=true; }
inline json TraceJson() { json r=json::array(); for(int k=0;k<ntr;++k){ const auto& e=tr[k]; r.push_back({CODE_NAMES[e.code],Name(e.a),Name(e.b),Name(e.c)}); } return r; }

// estado simulado de cada ejecucion
inline int nEvc{}, nTc{};
inline uint32 maskA{}, callsA{}, maskB{}, callsB{};
inline bool BitA() { return (maskA>>(callsA++%32))&1; }
inline bool BitB() { return (maskB>>(callsB++%32))&1; }
inline CPed* playerPed{};
inline std::array<uint8*,NPED> pedGroup{};

// ======================= vtables falsas =======================
template<int N> void* __fastcall Trap(void* self,void*) { T(C_TRAP,uint32(N),U(self)); return nullptr; }
inline std::array<void*,24> evVt{}, taskVt{}, taVt{}, daVt{};
template<int... N> void FillTraps(std::array<void*,24>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&Trap<N>)),...); }
// tareas: [4] = tipo
inline void* __fastcall TDtor(uint8* self,void*,uint32 flag) { T(C_TDTOR,U(self),flag); return self; }
inline void* __fastcall TClone(uint8* self,void*) { uint8* r=nullptr; if(nTc<NTC){ r=tc.data()+size_t(nTc++)*TSZ; std::memcpy(r,self,TSZ); } T(C_TCLONE,U(self),U(r)); return r; }
inline int32 __fastcall TType(uint8* self,void*) { T(C_TTYPE,U(self)); return At<int32>(self,4); }
// eventos: [4] = tipo, [8] = banderas (0 afecta al grupo, 1 respuesta editable, 2 interrumpible, 3 responde), [0xC] = origen, [0x10] = evento base
inline void* __fastcall EDtor(uint8* self,void*,uint32 flag) { T(C_EDTOR,U(self),flag); return self; }
inline void* __fastcall EClone(uint8* self,void*) { uint8* r=nullptr; if(nEvc<NEVC){ r=evc.data()+size_t(nEvc++)*EVSZ; std::memcpy(r,self,EVSZ); } T(C_ECLONE,U(self),U(r)); return r; }
inline int32 __fastcall EType(uint8* self,void*) { T(C_ETYPE,U(self)); return At<int32>(self,4); }
inline bool __fastcall EAffects(uint8* self,void*,void* g) { T(C_EAFFECT,U(self),U(g)); return At<uint32>(self,8)&1; }
inline bool __fastcall EEdit(uint8* self,void*) { T(C_EEDIT,U(self)); return (At<uint32>(self,8)>>1)&1; }
inline void* __fastcall ESrc(uint8* self,void*) { T(C_ESRC,U(self)); return At<void*>(self,0xC); }
inline bool __fastcall EIntr(uint8* self,void*) { T(C_EINTR,U(self)); return (At<uint32>(self,8)>>2)&1; }
// asignadores de tareas: [4] = lo que devuelve ProcessGroup
inline void* __fastcall ADtor(uint8* self,void*,uint32 flag) { T(C_ADTOR,U(self),flag); return self; }
inline void __fastcall AAlloc(uint8* self,void*,void* in) { T(C_AALLOC,U(self),U(in)); }
inline void* __fastcall AProc(uint8* self,void*,void* in) { T(C_APROC,U(self),U(in)); return At<void*>(self,4); }
// asignadores de tareas por defecto
inline void __fastcall DAlloc(void* self,void*,void* g,void* ped) { T(C_DALLOC,U(self),U(g),U(ped)); }

// ======================= stubs de dependencias =======================
inline bool __fastcall FakePrio(void* self,void*,const void* other) { const bool r=BitA(); T(C_PRIO,U(self),U(other),r); return r; }
inline void __fastcall FakeCrtt(void* self,void*,void* g) { T(C_CRTT,U(self),U(g)); }
inline bool __fastcall FakeWill(uint8* self,void*) { T(C_WILL,U(self)); return (At<uint32>(self,8)>>3)&1; }
inline CTaskAllocator* __cdecl FakeCert(const void* ev,void* g) { void* r=BitB() ? TA(int(callsB%NTA)) : nullptr; T(C_CERT,U(ev),U(g),U(r)); return static_cast<CTaskAllocator*>(r); }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* ped) { uint8* r=nullptr;
    for(int k=0;k<NPED;++k)if(reinterpret_cast<const uint8*>(ped)==PedB(k))r=pedGroup[k];
    T(C_PGRP,U(ped),U(r)); return reinterpret_cast<CPedGroup*>(r); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { T(C_FPP,uint32(id),U(playerPed)); return reinterpret_cast<CPlayerPed*>(playerPed); }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { T(C_FPW,uint32(id)); return reinterpret_cast<CWanted*>(wanted.data()); }

// ======================= hooks y parches =======================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
// cuerpo de una funcion de la DLL: sigue los saltos del ILT mientras no vuelvan al EXE
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class M> inline uintptr_t MAddr(M m) { static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=25;
inline const std::array<Fn,NFNS> fns{{
    {"CPedGroupIntelligence","Constructor",0x5F7250,0,-1,128},{"CPedGroupIntelligence","Flush",0x5F7350,0,-1,512},
    {"CPedGroupIntelligence","AddEvent-",0x5F7470,0,-1,1024},{"CPedGroupIntelligence","SetTask",0x5F7540,0,-1,1024},
    {"CPedGroupIntelligence","ReportFinishedTask-Impl",0x5F76C0,0,-1,512},{"CPedGroupIntelligence","ReportAllTasksFinished",0x5F7730,0,-1,256},
    {"CPedGroupIntelligence","IsGroupResponding",0x5F7760,0,-1,256},{"CPedGroupIntelligence","IsCurrentEventValid",0x5F77A0,0,-1,512},
    {"CPedGroupIntelligence","SetGroupDecisionMakerType",0x5F7340,0,-1,64},{"CPedGroupIntelligence","SetPrimaryTaskAllocator",0x5F7410,0,-1,256},
    {"CPedGroupIntelligence","SetEventResponseTaskAllocator",0x5F7440,0,-1,256},{"CPedGroupIntelligence","SetScriptCommandTask",0x5F8560,0,-1,512},
    {"CPedGroupIntelligence","GetTaskMain",0x5F85A0,0,-1,512},{"CPedGroupIntelligence","GetTaskSecondary",0x5F8620,0,-1,256},
    {"CPedGroupIntelligence","GetTaskSecondarySlot",0x5F8650,0,-1,256},{"CPedGroupIntelligence","GetTaskScriptCommand",0x5F8690,0,-1,256},
    {"CPedGroupIntelligence","GetTaskDefault",0x5F86C0,0,-1,256},{"CPedGroupIntelligence","ReportFinishedTask-Wrapper",0x5F86F0,0,-1,512},
    {"CPedGroupIntelligence","ReportAllBarScriptTasksFinished",0x5F8780,0,-1,256},{"CPedGroupIntelligence","ProcessIgnorePlayerGroup",0x5F87A0,0,-1,1024},
    {"CPedGroupIntelligence","ComputeDefaultTasks",0x5F88D0,0,-1,256},{"CPedGroupIntelligence","SetDefaultTaskAllocator",0x5FB280,0,-1,256},
    {"CPedGroupIntelligence","SetDefaultTaskAllocatorType",0x5FBB70,0,-1,256},{"CPedGroupIntelligence","ComputeEventResponseTasks",0x5FC440,0,-1,256},
    {"CPedGroupIntelligence","Process",0x5FC4A0,0,-1,1024},
}};
// hooks de la clase que no se prueban aqui pero se conmutan con el resto (ya validada en FixPeds)
inline constexpr const char* EXTRA_HOOKS[]{"FlushTasks"};

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pgi26-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pgi26-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pgi26");}
    if(!FindPlayerPed() || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"pgi26-only.txt"); if(f)std::getline(f,only); }
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(auto& fn:fns){ auto* h=FindHook(fn.cls,fn.name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fn.cls)+"::"+fn.name}});return;} hooks.push_back(h); }
    for(auto* nm:EXTRA_HOOKS){ auto* h=FindHook("CPedGroupIntelligence",nm); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string("CPedGroupIntelligence::")+nm}});return;} hooks.push_back(h); }
    // vtables falsas
    FillTraps(evVt,std::make_integer_sequence<int,24>{}); FillTraps(taskVt,std::make_integer_sequence<int,24>{});
    FillTraps(taVt,std::make_integer_sequence<int,24>{}); FillTraps(daVt,std::make_integer_sequence<int,24>{});
    taskVt[0]=reinterpret_cast<void*>(&TDtor); taskVt[1]=reinterpret_cast<void*>(&TClone); taskVt[4]=reinterpret_cast<void*>(&TType);
    evVt[0]=reinterpret_cast<void*>(&EDtor); evVt[1]=reinterpret_cast<void*>(&EType); evVt[4]=reinterpret_cast<void*>(&EClone);
    evVt[6]=reinterpret_cast<void*>(&EAffects); evVt[9]=reinterpret_cast<void*>(&EEdit); evVt[10]=reinterpret_cast<void*>(&ESrc); evVt[15]=reinterpret_cast<void*>(&EIntr);
    taVt[0]=reinterpret_cast<void*>(&ADtor); taVt[1]=reinterpret_cast<void*>(&AAlloc); taVt[2]=reinterpret_cast<void*>(&AProc);
    daVt[0]=reinterpret_cast<void*>(&DAlloc);
    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,void* stub){ plan.push_back({exeAddr,stub}); const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,stub}); };
    add(0x56E210,uintptr_t(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),reinterpret_cast<void*>(&FakeFindPlayerPed));
    add(0x56E230,uintptr_t(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),reinterpret_cast<void*>(&FakeFindPlayerWanted));
    add(0x4AE100,MAddr(&CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent),reinterpret_cast<void*>(&FakePrio));
    add(0x4B57A0,MAddr(static_cast<void(CEventEditableResponse::*)(CPedGroup*)>(&CEventEditableResponse::ComputeResponseTaskType)),reinterpret_cast<void*>(&FakeCrtt));
    add(0x4AC490,MAddr(&CEventEditableResponse::WillRespond),reinterpret_cast<void*>(&FakeWill));
    add(0x5FC200,uintptr_t(&CGroupEventHandler::ComputeEventResponseTasks),reinterpret_cast<void*>(&FakeCert));
    plan.push_back({0x5F7E80,reinterpret_cast<void*>(&FakePedsGroup)});   // la DLL llama al EXE (plugin::CallAndReturn)
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }
    const auto occupancyStart=Occupancy();
    std::array<uint32,SDA_N> sdaSaved{}; std::memcpy(sdaSaved.data(),reinterpret_cast<void*>(SDA),sizeof(sdaSaved));
    std::vector<bool> enabled(hooks.size()),locked(hooks.size());
    for(size_t f=0;f<hooks.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);hooks[f]->State(false);}
    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    // el New del pool de tareas del EXE busca desde m_LastFreeSlot+1: mismo slot en las dos ejecuciones
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sb=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sb[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x26A5u; if(!st)st=1;
                    ntr=0; trOverflow=false; nEvc=0; nTc=0; callsA=0; callsB=0;
                    const auto occ=Occupancy();
                    // ---- fixtures ----
                    auto fillRand=[&](uint8* p,size_t n){ for(size_t k=0;k<n/4;++k)At<uint32>(p,k*4)=next(); };
                    for(int k=0;k<NPED;++k){ std::memset(PedB(k),0x11,PEDSZ); PedB(k)[0x36]=uint8((next()&0xF8)|(next()%6==0 ? (next()%2 ? 2u : 4u) : 3u)); }
                    for(int g=0;g<NGRP;++g){ fillRand(Grp(g),GRPSZ); At<uint8*>(Grp(g),8)=Grp(g);
                        for(int m=0;m<8;++m)Member(g,m)=next()%10<4 ? nullptr : Ped(int(next()%NPED)); }
                    for(int k=0;k<NPED;++k){ const uint32 m=next()%4; pedGroup[k]=m==0 ? nullptr : Grp(int(m-1)); }
                    constexpr int32 EV_TYPES[]{47,47,47,0x47,0x47,0x4D,0x36,9};
                    for(int k=0;k<NEV;++k){ uint8* e=Ev(k); fillRand(e,EVSZ); At<void*>(e,0)=evVt.data(); At<int32>(e,4)=EV_TYPES[next()%8];
                        At<uint32>(e,8)=(next()&0xFFFFFFF0u)|(next()%8 ? 1u : 0u)|(next()%3==0 ? 2u : 0u)|(next()%2 ? 4u : 0u)|(next()%4 ? 8u : 0u);
                        const uint32 m=next()%5; At<void*>(e,0xC)=m==0 ? nullptr : m==1 ? static_cast<void*>(Member(0,int(next()%8))) : static_cast<void*>(Ped(int(next()%NPED)));
                        At<void*>(e,0x10)=Ev(int(next()%NEV)); }
                    std::memset(evc.data(),0xCD,evc.size()); std::memset(tc.data(),0xCD,tc.size());
                    for(int k=0;k<NT;++k){ fillRand(Tk(k),TSZ); At<void*>(Tk(k),0)=taskVt.data(); At<int32>(Tk(k),4)=200+int32(next()%4); }
                    for(int k=0;k<NTA;++k){ fillRand(TA(k),TASZ); At<void*>(TA(k),0)=taVt.data(); At<void*>(TA(k),4)=next()%2 ? nullptr : TA(int(next()%NTA)); }
                    for(int k=0;k<NDA;++k){ fillRand(DA(k),DASZ); At<void*>(DA(k),0)=daVt.data(); }
                    fillRand(wanted.data(),wanted.size());
                    { const bool back=nm=="ProcessIgnorePlayerGroup" ? next()%4!=0 : next()%2==0; wanted[0x1E]=uint8((wanted[0x1E]&~4u)|(back ? 4u : 0u)); }
                    { const uint32 m=next()%5; playerPed=m==0 ? nullptr : m<=2 ? Member(0,7) : m==3 ? Member(int(1+next()%2),7) : Ped(int(next()%NPED)); }
                    maskA=next(); maskB=next();
                    // inteligencia
                    fillRand(intel.data(),INTEL_BUF);
                    IW(0)=U(Grp(0));
                    { IW(4)=next()%10<3 ? 0 : U(Ev(int(next()%NEV))); const uint32 m=next()%10; IW(8)=m<4 ? 0 : m<6 ? IW(4) : U(Ev(int(next()%NEV))); }
                    for(int a=0;a<4;++a)for(int k=0;k<8;++k){ const size_t o=PairOff(a,k); const uint32 m=next()%10;
                        IW(o)=m<2 ? 0 : m<6 ? U(Member(0,k)) : m<8 ? U(Ped(int(next()%NPED))) : U(Member(0,int(next()%8)));
                        IW(o+4)=next()%3==0 ? 0 : U(Tk(int(next()%NT)));
                        IW(o+8)=next()%4==0 ? 0xFFFFFFFFu : next()%16; IW(o+0xC)=next(); IW(o+0x10)=next(); }
                    IW(0x28C)=U(DA(int(next()%NDA)));
                    IW(0x290)=next()%2 ? 0 : U(TA(int(next()%NTA)));
                    { const uint32 m=next()%10; IW(0x294)=m<4 ? 0 : m<5 ? IW(0x290) : U(TA(int(next()%NTA))); }
                    IW(0x298)=uint32(int32(next()%30)-1); IW(0x29C)=next();
                    // ---- argumentos ----
                    auto pedArg=[&]()->CPed*{ const uint32 m=next()%8; if(m==0)return nullptr;
                        if(m<=4)return reinterpret_cast<CPed*>(uintptr_t(IW(PairOff(int(next()%4),int(next()%8)))));
                        return Ped(int(next()%NPED)); };
                    void* const self=intel.data(); const uintptr_t A=fn.address;
                    json ret=nullptr; bool switches=false;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(A)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    if(nm=="Constructor"){ std::memset(intel.data(),0xCD,INTEL_BUF); on(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(A)(self))); off(); }
                    else if(nm=="Flush"||nm=="Process"||nm=="ProcessIgnorePlayerGroup"||nm=="ReportAllBarScriptTasksFinished"){ on(); reinterpret_cast<void(__thiscall*)(void*)>(A)(self); off(); }
                    else if(nm=="IsGroupResponding"||nm=="IsCurrentEventValid"){ on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(self); off(); }
                    else if(nm=="ComputeEventResponseTasks"){ on(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(A)(self))); off(); }
                    else if(nm=="AddEvent-"){ const uint32 m=next()%8; void* ev=m==0&&IW(8) ? reinterpret_cast<void*>(uintptr_t(IW(8))) : Ev(int(next()%NEV));
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(A)(self,ev); off(); }
                    else if(nm=="SetTask"){ CPed* ped=pedArg(); void* t=Tk(int(next()%NT)); const int arr=int(next()%4);
                        const uint32 slot=next()%4==0 ? 0xFFFFFFFFu : next()%16; const bool force=next()%3==0;
                        on(); reinterpret_cast<void(__thiscall*)(void*,CPed*,void*,void*,uint32,bool)>(A)(self,ped,t,intel.data()+PairOff(arr,0),slot,force); off(); }
                    else if(nm=="SetScriptCommandTask"){ CPed* ped=pedArg(); void* t=Tk(int(next()%NT));
                        on(); reinterpret_cast<void(__thiscall*)(void*,CPed*,void*)>(A)(self,ped,t); off(); }
                    else if(nm=="ReportFinishedTask-Impl"){ CPed* ped=pedArg(); void* t=Tk(int(next()%NT)); const int arr=int(next()%4);
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,void*,void*)>(A)(self,ped,t,intel.data()+PairOff(arr,0)); off(); }
                    else if(nm=="ReportFinishedTask-Wrapper"){ CPed* ped=pedArg(); void* t=Tk(int(next()%NT));
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,void*)>(A)(self,ped,t); off(); }
                    else if(nm=="ReportAllTasksFinished"){ const int arr=int(next()%4);
                        on(); reinterpret_cast<void(__thiscall*)(void*,void*)>(A)(self,intel.data()+PairOff(arr,0)); off(); }
                    else if(nm=="GetTaskMain"||nm=="GetTaskSecondary"||nm=="GetTaskScriptCommand"||nm=="GetTaskDefault"){ CPed* ped=pedArg();
                        on(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(A)(self,ped))); off(); }
                    else if(nm=="GetTaskSecondarySlot"){ CPed* ped=pedArg(); on(); ret=reinterpret_cast<uint32(__thiscall*)(void*,CPed*)>(A)(self,ped); off(); }
                    else if(nm=="SetGroupDecisionMakerType"){ const int32 t=int32(next()%30)-1; on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(A)(self,t); off(); }
                    else if(nm=="SetPrimaryTaskAllocator"||nm=="SetEventResponseTaskAllocator"){ const uint32 m=next()%6;
                        void* ta=m==0 ? nullptr : m==1 ? reinterpret_cast<void*>(uintptr_t(IW(0x290))) : m==2 ? reinterpret_cast<void*>(uintptr_t(IW(0x294))) : static_cast<void*>(TA(int(next()%NTA)));
                        on(); reinterpret_cast<void(__thiscall*)(void*,void*)>(A)(self,ta); off(); }
                    else if(nm=="ComputeDefaultTasks"){ CPed* ped=pedArg(); on(); reinterpret_cast<void(__thiscall*)(void*,CPed*)>(A)(self,ped); off(); }
                    else if(nm=="SetDefaultTaskAllocator"){ void* da=DA(int(next()%NDA)); on(); reinterpret_cast<void(__thiscall*)(void*,void*)>(A)(self,da); off(); }
                    else if(nm=="SetDefaultTaskAllocatorType"){ const uint32 t=next()%6;
                        // vtable falsa en los 6 asignadores estaticos del EXE solo durante la llamada
                        auto* s=reinterpret_cast<uint32*>(SDA); for(size_t k=0;k<SDA_N;++k)s[k]=U(daVt.data());
                        on(); reinterpret_cast<void(__thiscall*)(void*,uint32)>(A)(self,t); off();
                        std::memcpy(s,sdaSaved.data(),sizeof(sdaSaved)); }
                    // ---- volcado ----
                    json r={{"ret",ret},{"switches",switches},{"intel",Region(intel.data(),INTEL)},{"tail",Hash(intel.data()+INTEL,INTEL_BUF-INTEL)},
                            {"evc",Region(evc.data(),size_t(nEvc)*EVSZ)},{"tc",Region(tc.data(),size_t(nTc)*TSZ)},
                            {"fixtures",Hash(peds.data(),peds.size())+Hash(grps.data(),grps.size())+Hash(evs.data(),evs.size())+Hash(tasks.data(),tasks.size())
                                       +Hash(tas.data(),tas.size())+Hash(das.data(),das.size())+Hash(wanted.data(),wanted.size())},
                            {"trace",TraceJson()},{"overflow",trOverflow},{"rand_a",callsA},{"rand_b",callsB}};
                    r["clean"]=Occupancy()==occ;
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true&&original["overflow"]==false;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    std::memcpy(reinterpret_cast<void*>(SDA),sdaSaved.data(),sizeof(sdaSaved));
    for(size_t f=0;f<hooks.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},
           {"state_restored",Occupancy()==occupancyStart&&std::memcmp(reinterpret_cast<void*>(SDA),sdaSaved.data(),sizeof(sdaSaved))==0}});
}
}
