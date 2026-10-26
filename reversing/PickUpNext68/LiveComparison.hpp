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
#include "Tasks/TaskTypes/TaskComplexGoPickUpEntity.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskSimplePickUpEntity.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskManager.h"
#include "PedIntelligence.h"
#include "Collision/ColModel.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PickUpNext68 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CTaskComplexGoPickUpEntity::CreateNextSubTask (0x691AE0, hook VIRTUAL, slot 8 de la vtable 0x870B98). Los colaboradores se sustituyen por falsos identicos en ambos lados
// (parche de entrada en la direccion del EXE y en el cuerpo de la DLL). Los constructores de CTaskSimpleHoldEntity (0x6913A0) y CTaskSimplePickUpEntity (0x6917B0) tienen hook: su cuerpo
// real en la DLL se halla siguiendo la primera llamada de la envoltura del hook. El constructor de CTaskSimpleAchieveHeading (0x667E20) no tiene hook: corre real en ambos lados y se compara
// el objeto construido (con el tipo en lugar del puntero a la vtable).

namespace Pu68Layout {
struct PuPed : CPed { static constexpr size_t OffIntel() { return offsetof(PuPed, m_pIntelligence); } };
struct PuEntity : CEntity {};
static_assert(PuPed::OffIntel() == 0x47C, "CPed::m_pIntelligence en +0x47C");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPedIntelligence::m_TaskMgr en +4 (el EXE pasa intel + 4)");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CTaskComplexGoPickUpEntity, m_pEntity) == 0xC && offsetof(CTaskComplexGoPickUpEntity, m_vecPosition) == 0x10 && offsetof(CTaskComplexGoPickUpEntity, m_vecPickupPosition) == 0x1C
    && offsetof(CTaskComplexGoPickUpEntity, m_nTimePassedSinceLastSubTaskCreatedInMs) == 0x28 && offsetof(CTaskComplexGoPickUpEntity, m_nAnimGroupId) == 0x2C && sizeof(CTaskComplexGoPickUpEntity) == 0x34, "CTaskComplexGoPickUpEntity");
static_assert(offsetof(CTaskSimplePickUpEntity, m_vecPickuposn) == 0x40 && sizeof(CTaskSimplePickUpEntity) == 0x4C && sizeof(CTaskSimpleHoldEntity) == 0x3C && sizeof(CTaskSimpleAchieveHeading) == 0x18, "tamanos de las tareas creadas");
static_assert(offsetof(CColModel, m_boundBox) == 0 && offsetof(CBox, m_vecMin) == 0, "CColModel: caja en +0, minimo en +0 (z en +8)");
static_assert(sizeof(CEntity) <= Pu68Fix::kEntWin && sizeof(CPed) <= Pu68Fix::kPedWin + 0x400, "ventanas de las arenas");
}

namespace Pu68Live {
using namespace Pu68Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kSubArena> subA;
inline Arena<kEntArena> entA;
inline Arena<kPedArena> pedA;
inline Arena<kIntelArena> intelA;
inline Arena<kMatArena> matP,matE;
inline Arena<kColArena> colA;
inline Arena<kSecArena> secA;
inline Arena<kAllocAArena> allocA;
inline Arena<kAllocBArena> allocB;
inline Arena<kAllocCArena> allocC;

inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
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

// ================================================================================================ CreateNextSubTask (0x691AE0)
struct State {const C* c{};};
inline State g;
// Tabla virtual falsa de la subtarea: la ranura 4 (GetTaskType) devuelve el tipo del caso; las demas son trampas (no deben llamarse)
inline u32 __fastcall FakeSubType(void* self,void*) {Ev& e=PushEv(7,1);e.v[0]=Tag(self);return g.c->subType;}                                                 // CTask::GetTaskType (vtable +0x10), `ret`
inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);return 0;}
inline std::array<void*,11> fakeVt{};
// Todos cdecl salvo los miembros (thiscall: this en ECX, `ret n`). Los floats viajan como dword en la pila.
inline void* __cdecl FakeNew(size_t size) {                                                                                                                       // 0x61A5A0, CTask::operator new
    Ev& e=PushEv(1,1);e.v[0]=u32(size);
    if(g.c->allocNull)return nullptr;
    return size==0x18?reinterpret_cast<void*>(allocA.Obj()):size==0x4C?reinterpret_cast<void*>(allocB.Obj()):size==0x3C?reinterpret_cast<void*>(allocC.Obj()):nullptr;
}
inline void* __fastcall FakeCol(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);return reinterpret_cast<void*>(colA.Obj());}                          // 0x535300, CEntity::GetColModel
inline u32 __fastcall FakeGetSec(void* self,void*,u32 idx) {Ev& e=PushEv(3,2);e.v[0]=Tag(self);e.v[1]=idx;                                                       // 0x681810, `ret 4`
    return g.c->secNull?0u:g.c->secKind==0?u32(secA.Obj()):g.c->secRaw;}
inline void __fastcall FakeSetSec(void* self,void*,u32 task,u32 idx) {Ev& e=PushEv(4,3);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(task));e.v[2]=idx;}             // 0x681B60, `ret 8`
inline void* __fastcall FakeHold(void* self,void*,u32 entity,u32 pos,u32 bone,u32 flags,u32 anim,u32 group,u32 dis) {                                           // 0x6913A0, `ret 1Ch`
    Ev& e=PushEv(5,8);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(entity));e.v[2]=Tag(uintptr_t(pos));e.v[3]=bone;e.v[4]=flags;e.v[5]=anim;e.v[6]=group;e.v[7]=dis;
    *reinterpret_cast<u32*>(self)=0xC0DE0005u;return self;
}
inline void* __fastcall FakePick(void* self,void*,u32 entity,u32 pos,u32 bone,u32 flags,u32 anim,u32 group,u32 blend) {                                         // 0x6917B0, `ret 1Ch`
    Ev& e=PushEv(6,8);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(entity));e.v[2]=Tag(uintptr_t(pos));e.v[3]=bone;e.v[4]=flags;e.v[5]=anim;e.v[6]=group;e.v[7]=blend;
    *reinterpret_cast<u32*>(self)=0xC0DE0006u;return self;
}

inline void CaseNext(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);subA.Fill(c.seed*13u+2u);entA.Fill(c.seed*13u+3u);pedA.Fill(c.seed*13u+4u);intelA.Fill(c.seed*13u+5u);matP.Fill(c.seed*13u+6u);matE.Fill(c.seed*13u+7u);
        colA.Fill(c.seed*13u+8u);secA.Fill(c.seed*13u+9u);allocA.Fill(c.seed*13u+10u);allocB.Fill(c.seed*13u+11u);allocC.Fill(c.seed*13u+12u);
        // sub tarea: solo la vtable falsa
        subA.Put32(0,u32(reinterpret_cast<uintptr_t>(fakeVt.data())));
        // entidad y peton: posicion en la matriz (+0x30) o en la posicion compacta (+4)
        entA.Put32(0x14,c.entMat?u32(matE.Obj()):0u);
        pedA.Put32(0x14,c.pedMat?u32(matP.Obj()):0u);
        for(int i=0;i<3;++i) {
            if(c.entMat)matE.Put32(0x30+4*i,c.ent[i]);else entA.Put32(4+4*i,c.ent[i]);
            if(c.pedMat)matP.Put32(0x30+4*i,c.ped[i]);else pedA.Put32(4+4*i,c.ped[i]);
        }
        pedA.Put32(0x47C,u32(intelA.Obj()));
        colA.Put32(8,c.colZ);
        // tarea
        taskA.Put32(8,u32(subA.Obj()));taskA.Put32(0xC,c.entNull?0u:u32(entA.Obj()));
        for(int i=0;i<3;++i) {taskA.Put32(0x10+4*i,c.vec[i]);taskA.Put32(0x1C+4*i,c.pick[i]);}
        taskA.Put32(0x2C,c.group);
        taskA.Snap();subA.Snap();entA.Snap();pedA.Snap();intelA.Snap();matP.Snap();matE.Snap();colA.Snap();secA.Snap();allocA.Snap();allocB.Snap();allocC.Snap();
        TagClear();taskA.Tagged();subA.Tagged();entA.Tagged();pedA.Tagged();intelA.Tagged();matP.Tagged();matE.Tagged();colA.Tagged();secA.Tagged();allocA.Tagged();allocB.Tagged();allocC.Tagged();
        G32(kTimeAddr)=c.time;
        g.c=&c;nev=0;
        const u32 args[1]{u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x691AE0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        const u32 ret=sides[s].reg.eax;
        obs["eax"]=Tag(uintptr_t(ret));
        // CTaskSimpleAchieveHeading real: el puntero a la vtable difiere entre el EXE y la DLL: se compara el tipo de tarea y el resto de bytes del objeto
        if(ret==u32(allocA.Obj())) {
            const u32 vptr=*reinterpret_cast<const u32*>(allocA.m.data()+kGuard);
            obs["type_a"]=reinterpret_cast<int(__thiscall*)(void*)>(reinterpret_cast<const uintptr_t*>(vptr)[4])(reinterpret_cast<void*>(allocA.Obj()));
            memcpy(allocA.m.data()+kGuard,allocA.r.data()+kGuard,4);
        }
        obs["changes"]=json::array({taskA.Changes(),subA.Changes(),entA.Changes(),pedA.Changes(),intelA.Changes(),matP.Changes(),matE.Changes(),colA.Changes(),secA.Changes(),allocA.Changes(),allocB.Changes(),allocC.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseNext(env,totals,c);
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
    if(!std::filesystem::exists(dir/"pickupnext68-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pickupnext68-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pickupnext68");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot=0x870B98+8*4;
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t newBody=DllBodyOf("CTask","operator new");
    const uintptr_t colBody=DllBodyOf("CEntity","GetColModel");
    const uintptr_t getSecBody=DllBodyOf("CTaskManager","GetTaskSecondary");
    const uintptr_t setSecBody=DllBodyOf("CTaskManager","SetTaskSecondary");
    const uintptr_t holdBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleHoldEntity","Constructor-1"));
    const uintptr_t pickBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimplePickUpEntity","Constructor-2"));
    // Ruta VIRTUAL: el cuerpo del candidato es el destino vivo del hook V
    {
        auto* hook=FindHook("CTaskComplexGoPickUpEntity","CreateNextSubTask");
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready=false;
        env.route[0].reset(new HookRoute("CTaskComplexGoPickUpEntity","CreateNextSubTask",0x691AE0,body));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    ready&=newBody&&colBody&&getSecBody&&setSecBody&&holdBody&&pickBody;
    env.stubs[0]={{Pair{0x61A5A0,newBody,true},reinterpret_cast<void*>(&FakeNew)},
                  {Pair{0x535300,colBody,true},reinterpret_cast<void*>(&FakeCol)},
                  {Pair{0x681810,getSecBody,true},reinterpret_cast<void*>(&FakeGetSec)},
                  {Pair{0x681B60,setSecBody,true},reinterpret_cast<void*>(&FakeSetSec)},
                  {Pair{0x6913A0,holdBody,true},reinterpret_cast<void*>(&FakeHold)},
                  {Pair{0x6917B0,pickBody,true},reinterpret_cast<void*>(&FakePick)}};
    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap);
    fakeVt[4]=reinterpret_cast<void*>(&FakeSubType);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading 0x667E20 (sin hook: real en ambos lados)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x691AE0,16);env.watches.emplace_back(env.vtSlot,4);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
