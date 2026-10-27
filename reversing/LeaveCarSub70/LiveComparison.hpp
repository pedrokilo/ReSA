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
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"
#include "Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"
#include "Tasks/TaskTypes/TaskComplexLeaveBoat.h"
#include "Tasks/TaskTypes/TaskSimpleCarDriveTimed.h"
#include "Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h"
#include "Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h"
#include "Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "Tasks/TaskTypes/TaskSimpleCarGetOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarJumpOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedOut.h"
#include "Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarForcePedOut.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCarSub70 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CTaskComplexLeaveCar::CreateSubTask (0x641530, hook SIMPLE; thiscall con (eTaskType, CPed*) en la pila). Los colaboradores se sustituyen por falsos identicos en ambos lados
// (parche de entrada en la direccion del EXE y en el cuerpo de la DLL): CTask::operator new 0x61A5A0, CPed::IsPlayer 0x5DF8F0, CEntity::RegisterReference 0x571B70 y los constructores con hook
// (el cuerpo real de la DLL se halla siguiendo la primera llamada de la envoltura `Constructor` del hook). CTaskSimpleCarForcePedOut (constructor 0x647710) se reconstruyo despues (2026-10-26, lote 71): ahora tambien se
// sustituye en el cuerpo de la DLL, como el resto de constructores con hook. El constructor de CTaskSimpleCarSetPedOut (0x6478B0, sin hook) corre real en ambos lados (RegisterReference ya es falso) y se compara el objeto.

namespace Ls70Layout {
static_assert(offsetof(CTaskComplexLeaveCar, m_pTargetVehicle) == 0xC && offsetof(CTaskComplexLeaveCar, m_nTargetDoor) == 0x10 && offsetof(CTaskComplexLeaveCar, m_nDelayTime) == 0x14
    && offsetof(CTaskComplexLeaveCar, m_bSensibleLeaveCar) == 0x18 && offsetof(CTaskComplexLeaveCar, m_bForceGetOut) == 0x19 && offsetof(CTaskComplexLeaveCar, m_bDie) == 0x1A
    && offsetof(CTaskComplexLeaveCar, m_pTaskUtilityLineUpPedWithCar) == 0x1C && offsetof(CTaskComplexLeaveCar, m_nDoorFlagsSet) == 0x20 && offsetof(CTaskComplexLeaveCar, m_nNumGettingInSet) == 0x21
    && offsetof(CTaskComplexLeaveCar, m_nDieAnimID) == 0x24 && offsetof(CTaskComplexLeaveCar, m_fDieAnimBlendDelta) == 0x28 && offsetof(CTaskComplexLeaveCar, m_fDieAnimSpeed) == 0x2C
    && offsetof(CTaskComplexLeaveCar, m_bIsInAir) == 0x30 && sizeof(CTaskComplexLeaveCar) == 0x34, "CTaskComplexLeaveCar");
static_assert(sizeof(CTaskSimplePause) == 0x18 && sizeof(CTaskComplexGetUpAndStandStill) == 0xC && sizeof(CTaskSimpleDie) == 0x28 && sizeof(CTaskSimpleCarDriveTimed) == 0x70 && sizeof(CTaskSimpleCarWaitToSlowDown) == 0x14
    && sizeof(CTaskSimpleCarSetPedOut) == 0x18 && sizeof(CTaskComplexCarSlowBeDraggedOut) == 0x1C, "tamanos de las tareas creadas");
}

namespace Ls70Live {
using namespace Ls70Fix;

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
inline Arena<kSmallArena> vehA,lineA;
inline Arena<kPedArena> pedA;
inline std::array<Arena<kAllocArena>,kAllocArenas> allocA;

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

// ================================================================================================ CreateSubTask (0x641530)
struct State {const C* c{};};
inline State g;
inline constexpr u32 kMarker=0xC0DE0000u;
// Todos los falsos: miembros thiscall como __fastcall (this en ECX, EDX de relleno, resto en la pila con `ret n` del callee); floats como dword.
inline void* __cdecl FakeNew(size_t size) {                                                                                                                    // 0x61A5A0, CTask::operator new
    Ev& e=PushEv(1,1);e.v[0]=u32(size);
    if(g.c->allocNull)return nullptr;
    for(unsigned i=0;i<kAllocArenas;++i)if(size==kAllocSizes[i])return reinterpret_cast<void*>(allocA[i].Obj());
    return nullptr;
}
inline u32 __fastcall FakeIsPlayer(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);return g.c->isPlayerRet;}                                          // 0x5DF8F0, CPed::IsPlayer
inline void __fastcall FakeRegRef(void* self,void*,u32 ref) {Ev& e=PushEv(3,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ref));}                                  // 0x571B70, `ret 4`
inline void* Done(void* self,u32 kind) {*reinterpret_cast<u32*>(self)=kMarker|kind;return self;}
inline void* __fastcall FakePause(void* self,void*,u32 time) {Ev& e=PushEv(10,2);e.v[0]=Tag(self);e.v[1]=time;return Done(self,10);}                            // 0x48E750, `ret 4`
inline void* __fastcall FakeGetUp(void* self,void*) {Ev& e=PushEv(11,1);e.v[0]=Tag(self);return Done(self,11);}                                                // 0x678130, `ret`
inline void* __fastcall FakeDie(void* self,void*,u32 grp,u32 anim,u32 blend,u32 speed) {Ev& e=PushEv(12,5);e.v[0]=Tag(self);e.v[1]=grp;e.v[2]=anim;e.v[3]=blend;e.v[4]=speed;return Done(self,12);}   // 0x62FA00, `ret 10h`
inline void* __fastcall FakeBoat(void* self,void*,u32 veh,u32 delay) {Ev& e=PushEv(13,3);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=delay;return Done(self,13);}          // 0x63BB60, `ret 8`
inline void* __fastcall FakeDrive(void* self,void*,u32 veh,u32 time) {Ev& e=PushEv(14,3);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=time;return Done(self,14);}         // 0x5FF940, `ret 8`
inline void* __fastcall FakeCloseDoor(void* self,void*,u32 veh,u32 door,u32 line) {Ev& e=PushEv(15,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=Tag(uintptr_t(line));return Done(self,15);}   // 0x6464F0, `ret 0Ch`
inline void* __fastcall FakeWaitSlow(void* self,void*,u32 veh,u32 type) {Ev& e=PushEv(16,3);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=type;return Done(self,16);}    // 0x646990, `ret 8`
inline void* __fastcall FakeWaitDoor(void* self,void*,u32 veh,u32 d1,u32 d2) {Ev& e=PushEv(17,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=d1;e.v[3]=d2;return Done(self,17);}   // 0x646B70, `ret 0Ch`
inline void* __fastcall FakeGetOut(void* self,void*,u32 veh,u32 door,u32 line) {Ev& e=PushEv(18,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=Tag(uintptr_t(line));return Done(self,18);}   // 0x6471D0, `ret 0Ch`
inline void* __fastcall FakeJumpOut(void* self,void*,u32 veh,u32 door,u32 line) {Ev& e=PushEv(19,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=Tag(uintptr_t(line));return Done(self,19);}  // 0x64BE40, `ret 0Ch`
inline void* __fastcall FakeForceOut(void* self,void*,u32 veh,u32 door) {Ev& e=PushEv(20,3);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;return Done(self,20);}        // 0x647710 (CTaskSimpleCarForcePedOut::Constructor, con hook desde 2026-10-26), `ret 8`
inline void* __fastcall FakeSlowDragged(void* self,void*,u32 veh,u32 door,u32 flag) {Ev& e=PushEv(21,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=flag;return Done(self,21);}   // 0x648490, `ret 0Ch`

inline void CaseSub(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);vehA.Fill(c.seed*13u+2u);lineA.Fill(c.seed*13u+3u);pedA.Fill(c.seed*13u+4u);
        for(unsigned i=0;i<kAllocArenas;++i)allocA[i].Fill(c.seed*13u+10u+i);
        taskA.Put32(4,c.parentRaw);taskA.Put32(8,c.subRaw);
        taskA.Put32(0xC,c.vehKind==0?0u:c.vehKind==1?u32(vehA.Obj()):c.vehRaw);
        taskA.Put32(0x10,c.door);taskA.Put32(0x14,c.delay);
        taskA.Put8(0x18,c.sensible);taskA.Put8(0x19,c.force);taskA.Put8(0x1A,c.die);
        taskA.Put32(0x1C,c.lineKind==0?0u:c.lineKind==1?u32(lineA.Obj()):c.lineRaw);
        taskA.Put8(0x20,c.doorFlags);taskA.Put8(0x21,c.numGetting);
        taskA.Put32(0x24,c.dieAnim);taskA.Put32(0x28,c.blend);taskA.Put32(0x2C,c.speed);taskA.Put8(0x30,c.inAir);
        taskA.Snap();vehA.Snap();lineA.Snap();pedA.Snap();
        for(auto& a:allocA)a.Snap();
        TagClear();taskA.Tagged();vehA.Tagged();lineA.Tagged();pedA.Tagged();
        for(auto& a:allocA)a.Tagged();
        g.c=&c;nev=0;
        const u32 args[2]{c.type,u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x641530,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        const u32 ret=sides[s].reg.eax;
        obs["eax"]=Tag(uintptr_t(ret));
        // CTaskSimpleCarSetPedOut real: el puntero a la vtable difiere entre el EXE y la DLL: se compara el tipo de tarea y el resto de bytes del objeto
        {
            auto& a=allocA[3];   // tamano 0x18
            const u32 w=*reinterpret_cast<const u32*>(a.m.data()+kGuard);
            if(ret==u32(a.Obj())&&(w&0xFFFF0000u)!=kMarker&&w!=*reinterpret_cast<const u32*>(a.r.data()+kGuard)) {
                obs["type_real"]=reinterpret_cast<int(__thiscall*)(void*)>(reinterpret_cast<const uintptr_t*>(w)[4])(reinterpret_cast<void*>(a.Obj()));
                memcpy(a.m.data()+kGuard,a.r.data()+kGuard,4);
            }
        }
        json ch=json::array({taskA.Changes(),vehA.Changes(),lineA.Changes(),pedA.Changes()});
        for(auto& a:allocA)ch.push_back(a.Changes());
        obs["changes"]=ch;
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
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseSub(env,totals,c);
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
    if(!std::filesystem::exists(dir/"leavecarsub70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecarsub70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecarsub70");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche
    const uintptr_t newBody=DllBodyOf("CTask","operator new");
    const uintptr_t playerBody=DllBodyOf("CPed","IsPlayer");
    const uintptr_t refBody=DllBodyOf("CEntity","RegisterReference-");
    const uintptr_t pauseBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimplePause","Constructor"));
    const uintptr_t getUpBody=FirstDllCall(env.asi,DllBodyOf("CTaskComplexGetUpAndStandStill","Constructor"));
    const uintptr_t dieBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleDie","Constructor-1"));
    const uintptr_t boatBody=FirstDllCall(env.asi,DllBodyOf("CTaskComplexLeaveBoat","Constructor"));
    const uintptr_t driveBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarDriveTimed","Constructor"));
    const uintptr_t closeBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarCloseDoorFromOutside","Constructor"));
    const uintptr_t waitSlowBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarWaitToSlowDown","Constructor"));
    const uintptr_t waitDoorBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarWaitForDoorNotToBeInUse","Constructor"));
    const uintptr_t getOutBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarGetOut","Constructor"));
    const uintptr_t jumpBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarJumpOut","Constructor"));
    const uintptr_t slowBody=FirstDllCall(env.asi,DllBodyOf("CTaskComplexCarSlowBeDraggedOut","Constructor"));
    const uintptr_t forceBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarForcePedOut","Constructor"));   // 2026-10-26: el constructor 0x647710 ya esta reconstruido y enganchado
    {
        env.route[0].reset(new HookRoute("CTaskComplexLeaveCar","CreateSubTask",0x641530,HookRoute::RegisteredTarget{}));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    ready&=newBody&&playerBody&&refBody&&pauseBody&&getUpBody&&dieBody&&boatBody&&driveBody&&closeBody&&waitSlowBody&&waitDoorBody&&getOutBody&&jumpBody&&slowBody&&forceBody;
    auto fk=[](auto* f){return reinterpret_cast<void*>(f);};
    env.stubs[0]={{Pair{0x61A5A0,newBody,true},fk(&FakeNew)},
                  {Pair{0x5DF8F0,playerBody,true},fk(&FakeIsPlayer)},
                  {Pair{0x571B70,refBody,true},fk(&FakeRegRef)},
                  {Pair{0x48E750,pauseBody,true},fk(&FakePause)},
                  {Pair{0x678130,getUpBody,true},fk(&FakeGetUp)},
                  {Pair{0x62FA00,dieBody,true},fk(&FakeDie)},
                  {Pair{0x63BB60,boatBody,true},fk(&FakeBoat)},
                  {Pair{0x5FF940,driveBody,true},fk(&FakeDrive)},
                  {Pair{0x6464F0,closeBody,true},fk(&FakeCloseDoor)},
                  {Pair{0x646990,waitSlowBody,true},fk(&FakeWaitSlow)},
                  {Pair{0x646B70,waitDoorBody,true},fk(&FakeWaitDoor)},
                  {Pair{0x6471D0,getOutBody,true},fk(&FakeGetOut)},
                  {Pair{0x64BE40,jumpBody,true},fk(&FakeJumpOut)},
                  {Pair{0x647710,forceBody,true},fk(&FakeForceOut)},
                  {Pair{0x648490,slowBody,true},fk(&FakeSlowDragged)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut 0x6478B0 (sin hook: real en ambos lados, con RegisterReference falso)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x641530,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
