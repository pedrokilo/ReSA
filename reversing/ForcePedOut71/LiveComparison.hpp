#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Ped/Ped.h"
#include "Tasks/TaskTypes/TaskSimpleCarForcePedOut.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par ForcePedOut71 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: el constructor de CTaskSimpleCarForcePedOut (0x647710, hook SIMPLE; thiscall con (vehiculo, puerta) en la pila). Colaboradores del EXE: CTaskSimple::CTaskSimple 0x61A390
// (padre a 0 y vtable 0x86D4A8; el constructor de la DLL integra ese cuerpo en linea, sin llamada ni cuerpo propio: se sustituye solo en el EXE por un falso que hace el mismo trabajo
// y anota el evento) y CEntity::RegisterReference 0x571B70 (falso identico en la direccion del EXE y en el cuerpo de la DLL).

namespace Fp71Layout {
static_assert(offsetof(CTaskSimpleCarForcePedOut, m_pVehicle) == 0x8 && offsetof(CTaskSimpleCarForcePedOut, m_nDoor) == 0xC && sizeof(CTaskSimpleCarForcePedOut) == 0x10, "CTaskSimpleCarForcePedOut");
}

namespace Fp71Live {
using namespace Fp71Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kSmallArena> vehA;

inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
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

// ================================================================================================ constructor (0x647710)
struct State {const C* c{};};
inline State g;
// Falsos: miembros thiscall como __fastcall (this en ECX, EDX de relleno, resto en la pila con `ret n` del callee).
// 0x61A390 CTaskSimple::CTaskSimple (sin pila, `ret`): hace lo mismo que el original (padre a 0, vtable 0x86D4A8, EAX = this) y anota el evento 30.
inline void* __fastcall FakeBaseCtor(void* self,void*) {Ev& e=PushEv(30,1);e.v[0]=Tag(self);auto* w=static_cast<u32*>(self);w[1]=0;w[0]=0x86D4A8u;return self;}
inline void __fastcall FakeRegRef(void* self,void*,u32 ref) {Ev& e=PushEv(3,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ref));}   // 0x571B70, `ret 4`

inline void CaseCtor(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);vehA.Fill(c.seed*13u+2u);
        if(c.prefill)memset(taskA.m.data()+kGuard,c.prefill==1?0xCD:c.prefill==2?0x00:0xFF,kTaskWin);
        taskA.Snap();vehA.Snap();
        TagClear();taskA.Tagged();vehA.Tagged();
        const u32 veh=c.vehKind==0?0u:c.vehKind==1?u32(vehA.Obj()+c.vehObjOff*4):c.vehRaw;
        g.c=&c;nev=0;
        const u32 args[2]{veh,c.door};
        sides[s]=Execute(env,0,0x647710,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        const u32 ret=sides[s].reg.eax;
        obs["eax"]=Tag(uintptr_t(ret));
        // El puntero a la vtable difiere entre el EXE (0x86EE94) y la DLL (su propia vtable en el ASI): se comprueba el origen de cada lado y se compara el tipo de tarea (GetTaskType, ranura 4)
        const u32 w=*reinterpret_cast<const u32*>(taskA.m.data()+kGuard);
        const bool vptrChanged=w!=*reinterpret_cast<const u32*>(taskA.r.data()+kGuard);
        const bool vptrOk=vptrChanged&&(candidate?ModuleOf(w)==env.asi:w==0x86EE94u);
        obs["vptr_ok"]=vptrOk;
        if(vptrOk) {
            obs["type"]=reinterpret_cast<int(__thiscall*)(void*)>(reinterpret_cast<const uintptr_t*>(w)[4])(reinterpret_cast<void*>(taskA.Obj()));
            memcpy(taskA.m.data()+kGuard,taskA.r.data()+kGuard,4);
        }
        // CTaskSimple::CTaskSimple: en el EXE una sola llamada, la primera, con this = el objeto; en la DLL esta integrada: se comprueba su efecto (padre a 0)
        u32 parent{};memcpy(&parent,taskA.m.data()+kGuard+4,4);
        bool baseOk=parent==0;
        if(!candidate) {
            unsigned count=0;for(unsigned i=0;i<nev&&i<evs.size();++i)count+=evs[i].kind==30;
            baseOk&=count==1&&nev>0&&evs[0].kind==30&&evs[0].v[0]==Tag(taskA.Obj());
        }
        obs["base_ctor_ok"]=baseOk;
        json ev=json::array();for(unsigned i=0;i<nev&&i<evs.size();++i)if(evs[i].kind!=30)ev.push_back(EvJson(evs[i]));
        obs["events"]=ev;
        obs["changes"]=json::array({taskA.Changes(),vehA.Changes()});
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
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseCtor(env,totals,c);
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
inline bool Prologue(uintptr_t f){const auto* b=reinterpret_cast<const uint8*>(f);if(b[0]==0x8B&&b[1]==0xFF)b+=2;return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC;}
// Primera llamada (E8) de `fn` (dentro del ASI) a una funcion del ASI con prologo push ebp / mov ebp,esp: el cuerpo real del constructor que llama la envoltura `Constructor` del hook.
inline uintptr_t FirstDllCall(uintptr_t asi,uintptr_t fn) {
    fn=Body(fn);if(!fn||ModuleOf(fn)!=asi)return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p) {
        if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue;
        const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(ModuleOf(t)==asi&&Prologue(Body(t)))return Body(t);
    }
    return 0;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"forcepedout71-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"forcepedout71-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("forcepedout71");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpo de la DLL de CEntity::RegisterReference: se resuelve ANTES de crear la ruta y de escribir ningun parche
    const uintptr_t refBody=DllBodyOf("CEntity","RegisterReference-");
    {
        env.route[0].reset(new HookRoute("CTaskSimpleCarForcePedOut","Constructor",0x647710,HookRoute::RegisteredTarget{}));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    ready&=refBody!=0;
    auto fk=[](auto* f){return reinterpret_cast<void*>(f);};
    env.stubs[0]={{Pair{0x61A390,0,false},fk(&FakeBaseCtor)},
                  {Pair{0x571B70,refBody,true},fk(&FakeRegRef)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTaskSimple::CTaskSimple 0x61A390: sin hook ni cuerpo propio en la DLL (integrado en linea en el constructor); falso solo en el EXE con el mismo efecto"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x647710,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
