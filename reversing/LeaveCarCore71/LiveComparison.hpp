#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "CutsceneMgr.h"
#include "World.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCarCore71 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion VIRTUAL: CTaskComplexLeaveCar::ControlSubTask (0x6421B0, hook V, ranura 10 de la vtable 0x86E828). Los colaboradores se sustituyen por falsos identicos en ambos lados:
// CreateSubTask (0x641530; si tiene hook se parchea el cuerpo del hook, si no la envoltura de la DLL), y por la vtable falsa de la subtarea GetTaskType (ranura 4) y MakeAbortable (ranura 6).

namespace Lc71Layout {
struct LcTask : CTaskComplex { static constexpr size_t OffSub() { return offsetof(LcTask, m_pSubTask); } };
static_assert(LcTask::OffSub() == 8, "CTaskComplex::m_pSubTask en +8");
static_assert(offsetof(CTaskComplexLeaveCar, m_pTargetVehicle) == 0xC && offsetof(CTaskComplexLeaveCar, m_bSensibleLeaveCar) == 0x18 && sizeof(CTaskComplexLeaveCar) == 0x34, "CTaskComplexLeaveCar");
}

namespace Lc71Live {
using namespace Lc71Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    u32& W(size_t off) {return *reinterpret_cast<u32*>(m.data()+kGuard+off);}
    void Put32(size_t off,u32 v) {W(off)=v;}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kSubArena> subA;
inline Arena<kPedArena> pedA;
inline Arena<kVehArena> vehA;
inline Arena<kOthArena> othA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ ControlSubTask (0x6421B0)
struct State {const C* c{};unsigned typeIdx{};};
inline State g;
// Tabla virtual falsa de la subtarea: ranura 4 (GetTaskType) devuelve el tipo de esa llamada; ranura 6 (MakeAbortable) registra sus argumentos; las demas son trampas (no deben llamarse)
inline u32 __fastcall FakeSubType(void* self,void*) {                                                                                                          // CTask::GetTaskType (vtable +0x10), `ret`
    const unsigned i=g.typeIdx++;
    Ev& e=PushEv(17,2);e.v[0]=Tag(self);e.v[1]=i;
    return g.c->types[i<5?i:5];
}
inline u32 __fastcall FakeAbort(void* self,void*,u32 ped,u32 priority,u32 event) {                                                                            // CTask::MakeAbortable (vtable +0x18), `ret 0Ch`
    Ev& e=PushEv(18,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));e.v[2]=priority;e.v[3]=Tag(uintptr_t(event));
    return g.c->abortRet;
}
inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);return 0;}
inline std::array<void*,11> fakeVt{};
inline u32 __fastcall FakeCreate(void* self,void*,u32 type,u32 ped) {                                                                                            // 0x641530, `ret 8`
    Ev& e=PushEv(1,3);e.v[0]=Tag(self);e.v[1]=type;e.v[2]=Tag(uintptr_t(ped));
    return g.c->createNull?0u:(0xC0DE0000u|(type&0xFFFFu));
}

inline void CaseControl(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);subA.Fill(c.seed*13u+2u);pedA.Fill(c.seed*13u+3u);vehA.Fill(c.seed*13u+4u);othA.Fill(c.seed*13u+5u);
        subA.Put32(0,u32(reinterpret_cast<uintptr_t>(fakeVt.data())));
        // tarea
        taskA.Put32(8,u32(subA.Obj()));
        taskA.Put32(0xC,c.vehKind==0?0u:c.vehKind==1?u32(vehA.Obj()):c.vehRaw);
        taskA.Put8(0x18,c.sensible);taskA.Put8(0x19,c.force);taskA.Put8(0x1A,c.die);
        // peaton: bInVehicle (bit 8 de +0x46C)
        pedA.W(0x46C)=(c.pedBits&~0x100u)|(c.inVeh?0x100u:0u);
        pedA.W(0x470)=c.pedBits2;
        taskA.Snap();subA.Snap();pedA.Snap();vehA.Snap();othA.Snap();
        TagClear();taskA.Tagged();subA.Tagged();pedA.Tagged();vehA.Tagged();othA.Tagged();
        g.c=&c;g.typeIdx=0;nev=0;
        const u32 args[1]{u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x6421B0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["eax"]=Tag(uintptr_t(sides[s].reg.eax));
        obs["changes"]=json::array({taskA.Changes(),subA.Changes(),pedA.Changes(),vehA.Changes(),othA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"vtable_slot",env.vtSlot},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseControl(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Cuerpo de la DLL de una funcion con hook S (Simple) o V (virtual): el destino vivo del hook
inline uintptr_t DllBodyOf(const char* cls,const char* fn) {
    auto* h=FindHook(cls,fn);if(!h)return 0;uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a?Body(a):0;
}
// Cuerpo de una funcion miembro no virtual de la DLL (la que llama el codigo reconstruido, aunque el hook del EXE apunte a un envoltorio)
template<class M> inline uintptr_t MemberBody(M m) {static_assert(sizeof(M)==sizeof(uintptr_t));uintptr_t a;memcpy(&a,&m,sizeof(a));return Body(a);}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"leavecarcore71-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecarcore71-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecarcore71");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot=0x86E828+10*4;
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // CreateSubTask (0x641530): con hook se parchea el cuerpo del hook; sin hook, la envoltura de la DLL que llama a 0x641530. Se resuelve ANTES de crear la ruta y de escribir ningun parche.
    const bool createHooked=FindHook("CTaskComplexLeaveCar","CreateSubTask")!=nullptr;
    const uintptr_t createBody=createHooked?DllBodyOf("CTaskComplexLeaveCar","CreateSubTask"):MemberBody(&CTaskComplexLeaveCar::CreateSubTask);
    // Ruta VIRTUAL: el cuerpo del candidato es el destino vivo del hook V
    {
        auto* hook=FindHook("CTaskComplexLeaveCar","ControlSubTask");
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready=false;
        env.route[0].reset(new HookRoute("CTaskComplexLeaveCar","ControlSubTask",0x6421B0,body));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    ready&=createBody!=0;
    env.stubs[0]={{Pair{0x641530,createBody,createHooked},reinterpret_cast<void*>(&FakeCreate)}};
    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap);
    fakeVt[4]=reinterpret_cast<void*>(&FakeSubType);
    fakeVt[6]=reinterpret_cast<void*>(&FakeAbort);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"GetTaskType (ranura 4) y MakeAbortable (ranura 6) de la subtarea: llamadas virtuales por la vtable falsa (igual en ambos lados)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x6421B0,16);env.watches.emplace_back(env.vtSlot,4);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
