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
#include "Entity/Vehicle/Vehicle.h"
#include "CarEnterExit.h"
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par ForcePedOutVt71 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco funciones de CTaskSimpleCarForcePedOut (vtable 0x86EE94): cuerpo del destructor 0x647790 (hook S), Clone 0x649EE0 / MakeAbortable 0x647780 / ProcessPed 0x6477F0 (hooks V; se llaman por la
// ranura de la vtable del EXE) y el destructor con borrado (ranura 0, 0x64BF50 del EXE contra el que genera el compilador en la DLL; sin hook). Los colaboradores con hook se sustituyen por falsos
// identicos en la direccion del EXE y en el cuerpo de la DLL. CTask::~CTask (0x61A3A0) no tiene llamada ni cuerpo propio en la DLL (es en linea): falso solo en el EXE con el mismo efecto.

namespace Vt71Layout {
static_assert(offsetof(CTaskSimpleCarForcePedOut, m_pVehicle) == 0x8 && offsetof(CTaskSimpleCarForcePedOut, m_nDoor) == 0xC && sizeof(CTaskSimpleCarForcePedOut) == 0x10, "CTaskSimpleCarForcePedOut");
static_assert(offsetof(CVehicle, m_pDriver) == Vt71Fix::kOffDriver && offsetof(CVehicle, m_apPassengers) == Vt71Fix::kOffPassengers && offsetof(CVehicle, m_nGettingOutFlags) == Vt71Fix::kOffGettingOut, "CVehicle");
}

namespace Vt71Live {
using namespace Vt71Fix;

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
inline Arena<kVehArena> vehA;
inline Arena<kPedArena> pedA;
inline Arena<kAllocArena> allocA;

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

// ================================================================================================ las cinco funciones
struct State {const C* c{};};
inline State g;
inline u32 dllVptr{};
inline constexpr u32 kMarker=0xC0DE0000u;
// Falsos: miembros thiscall como __fastcall (this en ECX, EDX de relleno, resto en la pila con `ret n` del callee); los cdecl como __cdecl.
inline void* __cdecl FakeNew(size_t size) {Ev& e=PushEv(1,1);e.v[0]=u32(size);if(g.c->allocNull)return nullptr;return size==0x10?reinterpret_cast<void*>(allocA.Obj()):nullptr;}   // 0x61A5A0
inline void __cdecl FakeDelete(void* p) {Ev& e=PushEv(2,1);e.v[0]=Tag(p);}                                                                                              // 0x61A5B0, cdecl
inline void __fastcall FakeCleanUp(void* self,void*,u32 ref) {Ev& e=PushEv(3,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ref));}                                              // 0x571A00, `ret 4`
inline void __fastcall FakeBaseDtor(void* self,void*) {Ev& e=PushEv(40,1);e.v[0]=Tag(self);*static_cast<u32*>(self)=0x86D48Cu;}                                             // 0x61A3A0, `ret` (mov [ecx], 0x86D48C)
inline void __fastcall FakeDtorBody(void* self,void*) {Ev& e=PushEv(41,1);e.v[0]=Tag(self);*static_cast<u32*>(self)=kMarker|41u;}                                         // 0x647790, `ret`
inline void* __fastcall FakeCtor(void* self,void*,u32 veh,u32 door) {Ev& e=PushEv(20,3);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;*static_cast<u32*>(self)=kMarker|20u;return self;}   // 0x647710, `ret 8`
inline u32 __fastcall FakeIsDriver(void* self,void*,u32 ped) {Ev& e=PushEv(10,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));return g.c->isDriverRet;}                  // 0x6D1C40, `ret 4`
inline u32 __fastcall FakePosOut(void* self,void*,u32 door,u32 veh,u32 flag) {Ev& e=PushEv(11,4);e.v[0]=Tag(self);e.v[1]=door;e.v[2]=Tag(uintptr_t(veh));e.v[3]=flag;return g.c->posRet;}   // 0x5E0820, `ret 0Ch`
inline u32 __cdecl FakeOpp(u32 veh,u32 door,u32 flag) {Ev& e=PushEv(12,3);e.v[0]=Tag(uintptr_t(veh));e.v[1]=door;e.v[2]=flag;return g.c->oppRet;}                          // 0x64E610, cdecl
inline u32 __cdecl FakeSeats(u32 veh,u32 door) {Ev& e=PushEv(13,2);e.v[0]=Tag(uintptr_t(veh));e.v[1]=door;return g.c->seatsRet;}                                           // 0x64F1E0, cdecl

template<class M> inline uintptr_t AddrOfMember(M m) {static_assert(sizeof(M)==sizeof(uintptr_t));uintptr_t a{};memcpy(&a,&m,sizeof a);return Body(a);}

inline void CaseFn(Env& env,Totals& t,unsigned fn,unsigned n) {
    const C c=MakeC(fn,n);
    const json input=json::parse(JsonC(c));
    static constexpr uintptr_t kEntry[kFunctions]{0x647790,0x649EE0,0x647780,0x6477F0,0x64BF50};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);vehA.Fill(c.seed*13u+2u);pedA.Fill(c.seed*13u+3u);allocA.Fill(c.seed*13u+4u);
        if(c.prefill) {
            memset(taskA.m.data()+kGuard,c.prefill==1?0xCD:c.prefill==2?0x00:0xFF,kTaskWin);
        }
        const u32 veh=c.vehKind==0?0u:c.vehKind==1?u32(vehA.Obj()+c.vehOff*4):c.vehRaw;
        const u32 ped=c.pedKind==0?0u:c.pedKind==1?u32(pedA.Obj()):c.pedRaw;
        const u32 evt=c.evtKind==0?0u:c.evtKind==1?u32(pedA.Obj()+8):c.evtRaw;
        taskA.Put32(0,(fn==4&&candidate)?dllVptr:0x86EE94u);taskA.Put32(8,veh);taskA.Put32(0xC,c.door);
        if(fn==3&&c.vehKind==1) {
            // campos del vehiculo que lee ProcessPed (relativos al puntero del vehiculo)
            const size_t o=c.vehOff*4;
            vehA.Put32(o+kOffDriver,c.driver);
            for(unsigned i=0;i<8;++i)vehA.Put32(o+kOffPassengers+4*i,(c.passMask>>i)&1?(Rand(c.passVals,i)|1u):0u);
            vehA.Put8(o+kOffGettingOut,c.gettingOut);
        }
        taskA.Snap();vehA.Snap();pedA.Snap();allocA.Snap();
        TagClear();taskA.Tagged();vehA.Tagged();pedA.Tagged();allocA.Tagged();
        g.c=&c;nev=0;
        const u32 argsMake[3]{ped,c.prio,evt};
        const u32 argsProc[1]{ped};
        const u32 argsDel[1]{c.delFlags};
        sides[s]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t at){
            const u32* a=fn==2?argsMake:fn==3?argsProc:fn==4?argsDel:argsMake;
            const u32 cnt=fn==2?3:(fn==3||fn==4)?1:0;
            return Gate(at,a,cnt,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);
        });
        json obs;
        const u32 ret=sides[s].reg.eax;
        if(fn==1||fn==4)obs["eax"]=Tag(uintptr_t(ret));
        if(fn==2||fn==3)obs["al"]=ret&0xFFu;
        const u32 w=*reinterpret_cast<const u32*>(taskA.m.data()+kGuard);
        const u32 w0=*reinterpret_cast<const u32*>(taskA.r.data()+kGuard);
        if(fn==0) {
            // Destructor: el EXE deja la vtable de CTask (0x86D48C) tras una sola llamada a 0x61A3A0 (la ultima, con this = el objeto); la DLL deja la de CTask de la DLL (en el ASI)
            bool ok=candidate?(w!=w0&&ModuleOf(w)==env.asi):(w==0x86D48Cu);
            if(!candidate) {
                unsigned count=0;for(unsigned i=0;i<nev&&i<evs.size();++i)count+=evs[i].kind==40;
                ok&=count==1&&nev>0&&evs[nev-1].kind==40&&evs[nev-1].v[0]==Tag(taskA.Obj());
            }
            obs["vptr_ok"]=ok;
            memcpy(taskA.m.data()+kGuard,taskA.r.data()+kGuard,4);
        } else {
            obs["vptr_unchanged"]=(fn==4)?true:w==w0;
            if(fn==4)memcpy(taskA.m.data()+kGuard,taskA.r.data()+kGuard,4);
        }
        json ev=json::array();for(unsigned i=0;i<nev&&i<evs.size();++i)if(evs[i].kind!=40)ev.push_back(EvJson(evs[i]));
        obs["events"]=ev;
        obs["changes"]=json::array({taskA.Changes(),vehA.Changes(),pedA.Changes(),allocA.Changes()});
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[fn],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        if(env.route[f]) {
            const uintptr_t body=env.route[f]->Body();
            ready&=body&&ModuleOf(body)==env.asi;
        }
        for(const auto& s:env.stubs[f]) {
            ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"vtable_slots",env.vtSlot},{"live_target_in_asi",env.liveTarget!=0&&ModuleOf(env.liveTarget)==env.asi},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseFn(env,totals,f,c);
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
    if(!std::filesystem::exists(dir/"forcepedoutvt71-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"forcepedoutvt71-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("forcepedoutvt71");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear las rutas y de escribir ningun parche
    const uintptr_t newBody=DllBodyOf("CTask","operator new");
    const uintptr_t deleteBody=DllBodyOf("CTask","operator delete");
    const uintptr_t forceBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarForcePedOut","Constructor"));
    const uintptr_t dtorBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarForcePedOut","Destructor"));
    const uintptr_t cleanBody=AddrOfMember(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t isDriverBody=AddrOfMember(static_cast<bool(CVehicle::*)(const CPed*) const>(&CVehicle::IsDriver));
    const uintptr_t posBody=AddrOfMember(&CPed::PositionPedOutOfCollision);
    const uintptr_t oppBody=Body(uintptr_t(&CCarEnterExit::ComputeOppositeDoorFlag));
    const uintptr_t seatBody=Body(uintptr_t(&CCarEnterExit::ComputePassengerIndexFromCarDoor));
    {   // vtable de la DLL: se lee de un objeto con vehiculo nulo (no llama a ningun colaborador); la ranura 0 es el destructor con borrado de la DLL
        alignas(16) uint8 buf[0x20]{};
        auto* t=::new (buf) CTaskSimpleCarForcePedOut(nullptr,0);
        dllVptr=*reinterpret_cast<u32*>(t);
        env.liveTarget=Body(reinterpret_cast<const uintptr_t*>(dllVptr)[0]);
    }
    // F0: hook simple del cuerpo del destructor; F1..F3: hooks virtuales (ranuras 1, 6 y 7 de la vtable 0x86EE94); F4: sin hook
    {
        env.route[0].reset(new HookRoute("CTaskSimpleCarForcePedOut","Destructor",0x647790,HookRoute::RegisteredTarget{}));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
        static constexpr const char* names[3]{"Clone","MakeAbortable","ProcessPed"};
        static constexpr uintptr_t entries[3]{0x649EE0,0x647780,0x6477F0};
        static constexpr unsigned slots[3]{1,6,7};
        for(unsigned i=0;i<3;++i) {
            auto* hook=FindHook("CTaskSimpleCarForcePedOut",names[i]);
            uintptr_t body=0;
            if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
            else ready=false;
            env.route[1+i].reset(new HookRoute("CTaskSimpleCarForcePedOut",names[i],entries[i],body));
            registered[kFn[1+i]]=env.route[1+i]->Registered();bodies[kFn[1+i]]=env.route[1+i]->Body()!=0;
            ready&=env.route[1+i]->Registered()&&env.route[1+i]->Body()&&ModuleOf(env.route[1+i]->Body())==env.asi;
            env.vtSlot[1+i]=0x86EE94+4*slots[i];
        }
        registered[kFn[4]]=true;bodies[kFn[4]]=env.liveTarget!=0;   // sin hook: la ruta viva es la ranura 0 de la vtable de la DLL
        ready&=env.liveTarget&&ModuleOf(env.liveTarget)==env.asi;
    }
    ready&=newBody&&deleteBody&&forceBody&&dtorBody&&cleanBody&&isDriverBody&&posBody&&oppBody&&seatBody;
    auto fk=[](auto* f){return reinterpret_cast<void*>(f);};
    env.stubs[0]={{Pair{0x571A00,cleanBody,true},fk(&FakeCleanUp)},
                  {Pair{0x61A3A0,0,true},fk(&FakeBaseDtor)}};
    env.stubs[1]={{Pair{0x61A5A0,newBody,true},fk(&FakeNew)},
                  {Pair{0x647710,forceBody,true},fk(&FakeCtor)}};
    env.stubs[3]={{Pair{0x6D1C40,isDriverBody,true},fk(&FakeIsDriver)},
                  {Pair{0x5E0820,posBody,true},fk(&FakePosOut)},
                  {Pair{0x64E610,oppBody,true},fk(&FakeOpp)},
                  {Pair{0x64F1E0,seatBody,true},fk(&FakeSeats)}};
    env.stubs[4]={{Pair{0x647790,dtorBody,false},fk(&FakeDtorBody)},
                  {Pair{0x61A5B0,deleteBody,true},fk(&FakeDelete)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    for(uintptr_t a:{0x647790u,0x649EE0u,0x647780u,0x6477F0u,0x64BF50u})env.watches.emplace_back(a,16);
    env.watches.emplace_back(0x86EE94,36);   // vtable completa
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
