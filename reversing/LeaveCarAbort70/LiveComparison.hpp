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
#include "Entity/Vehicle/Vehicle.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskSimpleCarSetPedOut.h"
#include "CarEnterExit.h"
#include "EventDamage.h"
#include "Animation/AnimManager.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCarAbort70 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion VIRTUAL: CTaskComplexLeaveCar::MakeAbortable (0x641100, vtable 0x86E828 ranura 6). El cuerpo del candidato sale de ReversibleHook::Virtual::GetHookOurAddress().
// Los colaboradores se sustituyen por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL).
// Los constructores y destructores de las dos tareas temporales de la pila se sustituyen tambien: el de CTaskSimpleCarCloseDoorFromOutside tiene hook (cuerpo = primera llamada de la
// envoltura, como en PickUpNext68); el de CTaskSimpleCarSetPedOut no tiene hook y su cuerpo en la DLL se halla ejecutando una funcion sonda que lo construye y destruye (ver Resolve*).

namespace Lc70Layout {
struct TaskProbe : CTaskComplexLeaveCar {
    static constexpr size_t OffSub() {return offsetof(TaskProbe, m_pSubTask);}
    static constexpr size_t OffVeh() {return offsetof(TaskProbe, m_pTargetVehicle);}
    static constexpr size_t OffDoor() {return offsetof(TaskProbe, m_nTargetDoor);}
    static constexpr size_t OffSens() {return offsetof(TaskProbe, m_bSensibleLeaveCar);}
    static constexpr size_t OffFlags() {return offsetof(TaskProbe, m_nDoorFlagsSet);}
    static constexpr size_t OffNum() {return offsetof(TaskProbe, m_nNumGettingInSet);}
    static constexpr size_t OffAir() {return offsetof(TaskProbe, m_bIsInAir);}
};
static_assert(TaskProbe::OffSub() == 8 && TaskProbe::OffVeh() == 0xC && TaskProbe::OffDoor() == 0x10 && TaskProbe::OffSens() == 0x18 && TaskProbe::OffFlags() == 0x20 && TaskProbe::OffNum() == 0x21
              && TaskProbe::OffAir() == 0x30 && sizeof(CTaskComplexLeaveCar) == 0x34, "CTaskComplexLeaveCar");
struct PedProbe : CPed {
    static constexpr size_t OffMatrix() {return offsetof(PedProbe, m_matrix);}
    static constexpr size_t OffSpeed() {return offsetof(PedProbe, m_vecMoveSpeed);}
    static constexpr size_t OffWeapons() {return offsetof(PedProbe, m_aWeapons);}
    static constexpr size_t OffSlot() {return offsetof(PedProbe, m_nActiveWeaponSlot);}
};
static_assert(PedProbe::OffMatrix() == 0x14 && PedProbe::OffSpeed() == 0x44 && PedProbe::OffWeapons() == 0x5A0 && PedProbe::OffSlot() == 0x718 && sizeof(CWeapon) == 0x1C, "CPed");
struct VehProbe : CVehicle {
    static constexpr size_t OffSpeed() {return offsetof(VehProbe, m_vecMoveSpeed);}
    static constexpr size_t OffNum() {return offsetof(VehProbe, m_nNumGettingIn);}
    static constexpr size_t OffSub() {return offsetof(VehProbe, m_nVehicleSubType);}
};
static_assert(VehProbe::OffSpeed() == 0x44 && VehProbe::OffNum() == 0x485 && VehProbe::OffSub() == 0x594, "CVehicle");
struct EvProbe : CEventDamage {
    static constexpr size_t OffGroup() {return offsetof(EvProbe, m_bAddToEventGroup);}
    static constexpr size_t OffKilled() {return offsetof(EvProbe, m_damageResponse) + offsetof(CPedDamageResponse, m_bHealthZero);}
};
static_assert(EvProbe::OffGroup() == 0xC && EvProbe::OffKilled() == 0x40, "CEventDamage");
static_assert(TASK_SIMPLE_DIE == 0xD4 && TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN == 0x329 && TASK_COMPLEX_GET_UP_AND_STAND_STILL == 0xCE && TASK_SIMPLE_CAR_JUMP_OUT == 0x32E && TASK_SIMPLE_CAR_DRIVE_TIMED == 0x2CF, "eTaskType");
static_assert(EVENT_DAMAGE == 9 && EVENT_IN_AIR == 0x21 && EVENT_IN_WATER == 0x42 && WEAPON_PARACHUTE == 0x2E && ABORT_PRIORITY_URGENT == 1 && ABORT_PRIORITY_IMMEDIATE == 2, "enums");
}

namespace Lc70Live {
using namespace Lc70Fix;
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
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
// Llamadas (E8) de `fn` (dentro del ASI) a funciones del ASI con prologo push ebp / mov ebp,esp, en orden de aparicion (sin repetir)
inline std::vector<uintptr_t> DllCalls(uintptr_t asi,uintptr_t fn,size_t span=0x300) {
    std::vector<uintptr_t> out;fn=Body(fn);
    if(!fn||ModuleOf(fn)!=asi)return out;
    for(uintptr_t p=fn;p<fn+span;++p) {
        if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue;
        const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(ModuleOf(t)==asi&&Prologue(Body(t))&&std::ranges::find(out,Body(t))==out.end())out.push_back(Body(t));
    }
    return out;
}
inline uintptr_t FirstDllCall(uintptr_t asi,uintptr_t fn) {const auto v=DllCalls(asi,fn);return v.empty()?0:v[0];}

// ================================================================================================ arenas
template<size_t N,size_t Off=kGuard> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=0) const {return uintptr_t(m.data())+Off+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(int64 off,u32 v) {memcpy(m.data()+Off+off,&v,4);}
    void Put8(int64 off,u32 v) {m[size_t(Off+off)]=uint8(v);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,Off);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kSubArena> subA;
inline Arena<kVehArena> vehA;
inline Arena<kPedArena,kPedPrefix> pedA;
inline Arena<kMatArena> matA;
inline Arena<kEvArena> evA;

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

// ================================================================================================ MakeAbortable (0x641100)
struct State {const C* c{};unsigned stIdx{};unsigned etIdx{};std::array<uintptr_t,4> locals{};unsigned nloc{};};
inline State g;
enum EvKind : u32 {EvSubType=1,EvSubAbort,EvEvType,EvClearOut,EvWheels,EvMag,EvIsPlayer,EvVertLine,EvBlend,EvCDoor,EvCdCtor,EvCdDtor,EvCdAbort,EvSpCtor,EvSpDtor,EvSpProcess,EvTrap=99};
// Los objetos temporales de la pila no tienen la misma direccion en el EXE y en la DLL: se numeran por orden de construccion
inline u32 LocalTag(const void* self,bool construct) {
    for(unsigned i=0;i<g.nloc;++i)if(g.locals[i]==uintptr_t(self))return 0x20000000u+i;
    if(construct&&g.nloc<g.locals.size()) {g.locals[g.nloc]=uintptr_t(self);return 0x20000000u+g.nloc++;}
    return u32(uintptr_t(self));
}
inline bool OnStack(const void* p) {const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb());return uintptr_t(p)>=uintptr_t(tib->StackLimit)&&uintptr_t(p)<uintptr_t(tib->StackBase);}
// Todos cdecl salvo los miembros (thiscall: this en ECX, el que llama no limpia salvo `ret n`). Los floats viajan como dword en la pila.
inline u32 __fastcall FakeSubType(void* self,void*) {Ev& e=PushEv(EvSubType,1);e.v[0]=Tag(self);return g.c->St(g.stIdx++);}                                  // ranura 4 de la vtable de la subtarea
inline u32 __fastcall FakeSubAbort(void* self,void*,u32 ped,u32 prio,u32 evt) {Ev& e=PushEv(EvSubAbort,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));e.v[2]=prio;e.v[3]=Tag(uintptr_t(evt));return g.c->abortRet;}   // ranura 6, `ret 0xC`
inline u32 __fastcall FakeEvType(void* self,void*) {Ev& e=PushEv(EvEvType,1);e.v[0]=Tag(self);return g.c->Et(g.etIdx++);}                                    // ranura 1 de la vtable del evento
inline void __fastcall FakeClearOut(void* self,void*,u32 flags) {Ev& e=PushEv(EvClearOut,2);e.v[0]=Tag(self);e.v[1]=flags;}                               // 0x6D3060, CVehicle::ClearGettingOutFlags, `ret 4`
inline u32 __fastcall FakeWheels(void* self,void*) {Ev& e=PushEv(EvWheels,1);e.v[0]=Tag(self);return g.c->wheels;}                                         // ranura 55 de la vtable del vehiculo (GetNumContactWheels)
inline float __fastcall FakeMag(void* self,void*) {Ev& e=PushEv(EvMag,1);e.v[0]=Tag(self);return F(g.c->mag);}                                             // 0x4082C0, CVector::Magnitude (float en ST0)
inline u32 __fastcall FakeIsPlayer(void* self,void*) {Ev& e=PushEv(EvIsPlayer,1);e.v[0]=Tag(self);return g.c->isPlayer;}                                   // 0x5DF8F0, CPed::IsPlayer
inline u32 __cdecl FakeVertLine(const u32* origin,u32 distance,const void* colPoint,const void* entity,u32 b,u32 v,u32 p,u32 o,u32 d,u32 st,u32 poly) {   // 0x5674E0, CWorld::ProcessVerticalLine
    Ev& e=PushEv(EvVertLine,13);e.v[0]=Tag(uintptr_t(origin));e.v[1]=origin[0];e.v[2]=origin[1];e.v[3]=origin[2];e.v[4]=distance;e.v[5]=OnStack(colPoint)?1u:0u;e.v[6]=OnStack(entity)?1u:0u;
    e.v[7]=b;e.v[8]=v;e.v[9]=p;e.v[10]=o;e.v[11]=d;e.v[12]=st|(poly?0x100u:0u);
    return g.c->pvl;
}
inline u32 __cdecl FakeBlend(u32 clump,u32 group,u32 animId,u32 deltaBits) {Ev& e=PushEv(EvBlend,4);e.v[0]=clump;e.v[1]=group;e.v[2]=animId;e.v[3]=deltaBits;return 0;}   // 0x4D4610, CAnimManager::BlendAnimation (id)
inline u32 __cdecl FakeCDoor(u32 veh,u32 ped) {Ev& e=PushEv(EvCDoor,2);e.v[0]=Tag(uintptr_t(veh));e.v[1]=Tag(uintptr_t(ped));return g.c->cdoor;}           // 0x64F110, CCarEnterExit::ComputeTargetDoorToExit
inline void __fastcall FakeCdCtor(void* self,void*,u32 veh,u32 door,u32 lineUp) {Ev& e=PushEv(EvCdCtor,4);e.v[0]=LocalTag(self,true);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=Tag(uintptr_t(lineUp));}   // 0x6464F0 / ctor de la DLL, `ret 0xC`
inline void __fastcall FakeCdDtor(void* self,void*) {Ev& e=PushEv(EvCdDtor,1);e.v[0]=LocalTag(self,false);}                                                 // 0x646570 / dtor de la DLL
inline u32 __fastcall FakeCdAbort(void* self,void*,u32 ped,u32 prio,u32 evt) {Ev& e=PushEv(EvCdAbort,4);e.v[0]=LocalTag(self,false);e.v[1]=Tag(uintptr_t(ped));e.v[2]=prio;e.v[3]=Tag(uintptr_t(evt));return 0xC0DE0001u;}   // 0x64B020 / MakeAbortable de la DLL
inline void __fastcall FakeSpCtorExe(void* self,void*,u32 veh,u32 door,u32 sw) {Ev& e=PushEv(EvSpCtor,5);e.v[0]=LocalTag(self,true);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=sw;e.v[4]=0;}   // 0x6478B0, 3 argumentos, `ret 0xC`
inline void __fastcall FakeSpCtorDll(void* self,void*,u32 veh,u32 door,u32 sw,u32 warp) {Ev& e=PushEv(EvSpCtor,5);e.v[0]=LocalTag(self,true);e.v[1]=Tag(uintptr_t(veh));e.v[2]=door;e.v[3]=sw;e.v[4]=warp;}   // ctor de la DLL con el 4o argumento (warpingOutOfCar)
inline void __fastcall FakeSpDtor(void* self,void*) {Ev& e=PushEv(EvSpDtor,1);e.v[0]=LocalTag(self,false);}                                                 // 0x647950 / dtor de la DLL
inline u32 __fastcall FakeSpProcess(void* self,void*,u32 ped) {Ev& e=PushEv(EvSpProcess,2);e.v[0]=LocalTag(self,false);e.v[1]=Tag(uintptr_t(ped));return 0xC0DE0002u;}   // 0x647D10 / ProcessPed de la DLL, `ret 4`
inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(EvTrap,1);e.v[0]=Tag(self);return 0;}
alignas(16) inline std::array<u32,16> subVtbl{},evVtbl{};
alignas(16) inline std::array<u32,80> vehVtbl{};
inline void BuildVtbls() {
    for(auto& v:subVtbl)v=u32(uintptr_t(&FakeTrap));
    for(auto& v:evVtbl)v=u32(uintptr_t(&FakeTrap));
    for(auto& v:vehVtbl)v=u32(uintptr_t(&FakeTrap));
    subVtbl[4]=u32(uintptr_t(&FakeSubType));
    subVtbl[6]=u32(uintptr_t(&FakeSubAbort));
    evVtbl[1]=u32(uintptr_t(&FakeEvType));
    vehVtbl[55]=u32(uintptr_t(&FakeWheels));
}

// Funciones sonda: construyen y destruyen un objeto en la pila para localizar el constructor y el destructor reales de la DLL de una clase sin hook
__declspec(noinline) inline void ProbeSetPedOut() {CTaskSimpleCarSetPedOut t{nullptr,TARGET_DOOR_DRIVER,false,false};(void)t;}

inline void CaseAbort(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*7u+1u);subA.Fill(c.seed*7u+2u);vehA.Fill(c.seed*7u+3u);pedA.Fill(c.seed*7u+4u);matA.Fill(c.seed*7u+5u);evA.Fill(c.seed*7u+6u);
        taskA.Put32(0,0x86E828);taskA.Put32(8,c.subNull?0u:u32(subA.Obj()));taskA.Put32(0xC,c.vehNull?0u:u32(vehA.Obj()));taskA.Put32(0x10,c.door);taskA.Put8(0x18,c.sens);
        taskA.Put8(0x20,c.dfs);taskA.Put8(0x21,c.ngs);taskA.Put8(0x30,c.inAir);
        subA.Put32(0,u32(uintptr_t(subVtbl.data())));
        vehA.Put32(0,u32(uintptr_t(vehVtbl.data())));vehA.Put8(0x485,c.vng);vehA.Put32(0x594,c.vsub);
        evA.Put32(0,u32(uintptr_t(evVtbl.data())));evA.Put8(0xC,c.grp);evA.Put8(0x40,c.killed);
        pedA.Put32(0x14,c.hasMat?u32(matA.Obj()):0u);pedA.Put32(0x18,c.clump);pedA.Put32(0x46C,c.pedFlags);pedA.Put32(0x4C,c.vz);pedA.Put8(0x718,c.slot);
        pedA.Put32(0x5A0+int64(i8(c.slot))*0x1C,c.wtype);
        pedA.Put32(4,c.px);pedA.Put32(8,c.py);pedA.Put32(0xC,c.pz);
        matA.Put32(0x30,c.mx);matA.Put32(0x34,c.my);matA.Put32(0x38,c.mz);
        taskA.Snap();subA.Snap();vehA.Snap();pedA.Snap();matA.Snap();evA.Snap();
        TagClear();taskA.Tagged();subA.Tagged();vehA.Tagged();pedA.Tagged();matA.Tagged();evA.Tagged();
        g.c=&c;g.stIdx=0;g.etIdx=0;g.nloc=0;nev=0;
        const u32 args[3]{u32(pedA.Obj()),c.prio,c.evNull?0u:u32(evA.Obj())};
        sides[s]=Execute(env,0,0x641100,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;   // los llamadores virtuales del EXE hacen test al,al; EAX alto, ECX y EDX solo registrados
        obs["changes"]=json::array({taskA.Changes(),subA.Changes(),vehA.Changes(),pedA.Changes(),matA.Changes(),evA.Changes()});
        obs["events"]=EvsJson();
        obs["objs"]=json::array({u32(taskA.Obj()),u32(subA.Obj()),u32(vehA.Obj()),u32(pedA.Obj()),u32(matA.Obj()),u32(evA.Obj())});
        obs["sub_calls"]=g.stIdx;obs["event_calls"]=g.etIdx;
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
        for(const auto& s:env.dllOnly)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
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
            for(unsigned c=0;c<kCases;++c)CaseAbort(env,totals,c);
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

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"leavecarabort70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecarabort70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecarabort70");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    BuildVtbls();
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t blendBody=DllBody(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation));
    const uintptr_t vertBody=DllBody(&CWorld::ProcessVerticalLine);
    const uintptr_t clearBody=DllBody(&CVehicle::ClearGettingOutFlags);
    const uintptr_t magBody=DllBody(&CVector::Magnitude);
    const uintptr_t playerBody=DllBody(&CPed::IsPlayer);
    const uintptr_t doorBody=DllBody(&CCarEnterExit::ComputeTargetDoorToExit);
    const uintptr_t cdCtorBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarCloseDoorFromOutside","Constructor"));
    const uintptr_t cdDtorBody=FirstDllCall(env.asi,DllBodyOf("CTaskSimpleCarCloseDoorFromOutside","Destructor"));
    const uintptr_t cdAbortBody=DllBodyOf("CTaskSimpleCarCloseDoorFromOutside","MakeAbortable");
    const uintptr_t spProcessBody=DllBodyOf("CTaskSimpleCarSetPedOut","ProcessPed");
    // Constructor y destructor de CTaskSimpleCarSetPedOut (sin hook): primeras llamadas de la funcion sonda a funciones del ASI con prologo
    uintptr_t spCtorBody=0,spDtorBody=0;
    {
        const auto calls=DllCalls(env.asi,uintptr_t(&ProbeSetPedOut),0x100);
        if(calls.size()>=2) {spCtorBody=calls[0];spDtorBody=calls[1];}
        collaborators["probe_calls"]=calls.size();
    }
    // Ruta VIRTUAL: el cuerpo del candidato es ReversibleHook::Virtual::GetHookOurAddress()
    {
        auto* hook=FindHook("CTaskComplexLeaveCar","MakeAbortable");
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready=false;
        env.route[0].reset(new HookRoute("CTaskComplexLeaveCar","MakeAbortable",0x641100,body));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    ready&=blendBody&&vertBody&&clearBody&&magBody&&playerBody&&doorBody&&cdCtorBody&&cdDtorBody&&cdAbortBody&&spProcessBody&&spCtorBody&&spDtorBody;
    ready&=spCtorBody!=spDtorBody&&cdCtorBody!=cdDtorBody;
    env.stubs[0]={{Pair{0x4D4610,blendBody,true},reinterpret_cast<void*>(&FakeBlend)},
                  {Pair{0x5674E0,vertBody,true},reinterpret_cast<void*>(&FakeVertLine)},
                  {Pair{0x6D3060,clearBody,true},reinterpret_cast<void*>(&FakeClearOut)},
                  {Pair{0x4082C0,magBody,false},reinterpret_cast<void*>(&FakeMag)},
                  {Pair{0x5DF8F0,playerBody,true},reinterpret_cast<void*>(&FakeIsPlayer)},
                  {Pair{0x64F110,doorBody,true},reinterpret_cast<void*>(&FakeCDoor)},
                  {Pair{0x6464F0,cdCtorBody,true},reinterpret_cast<void*>(&FakeCdCtor)},
                  {Pair{0x646570,cdDtorBody,true},reinterpret_cast<void*>(&FakeCdDtor)},
                  {Pair{0x64B020,cdAbortBody,true},reinterpret_cast<void*>(&FakeCdAbort)},         // virtual con hook V: el hook tambien salta en la entrada del EXE
                  {Pair{0x6478B0,0,false},reinterpret_cast<void*>(&FakeSpCtorExe)},                // sin hook: la DLL tiene su propio constructor (4 argumentos), parche aparte en dllOnly
                  {Pair{0x647950,spDtorBody,false},reinterpret_cast<void*>(&FakeSpDtor)},
                  {Pair{0x647D10,spProcessBody,true},reinterpret_cast<void*>(&FakeSpProcess)}};
    env.dllOnly={{Pair{0,spCtorBody,false},reinterpret_cast<void*>(&FakeSpCtorDll)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CEventDamage::HasKilledPed 0x4ABCA0 (en linea en la DLL, codigo real del EXE: lee +0x40 y +0xC del evento)","CEntity::GetPosition (en linea)"})},{"stubbed",stubbed}};
        collaborators["dll_only"]=json::array({json{{"dll",spCtorBody},{"what","CTaskSimpleCarSetPedOut ctor de la DLL (4 argumentos)"}}});
    }
    env.watches.emplace_back(0x641100,16);env.watches.emplace_back(0x86E828+6*4,4);
    SaveRegions();
    // Autoprueba de la resolucion de los cuerpos sin hook: la sonda, con los parches puestos, debe pasar exactamente por el falso del constructor y el del destructor
    {
        g.c=nullptr;nev=0;g.nloc=0;
        ready&=spCtorBody&&spDtorBody;
        if(ready) {
            C dummy{};g.c=&dummy;
            {
                Patches p;
                p.Add(Pair{0,spCtorBody,false},reinterpret_cast<void*>(&FakeSpCtorDll));
                p.Add(Pair{0,spDtorBody,false},reinterpret_cast<void*>(&FakeSpDtor));
                ProbeSetPedOut();
            }
            ready&=nev==2&&evs[0].kind==EvSpCtor&&evs[1].kind==EvSpDtor&&evs[0].v[1]==0&&evs[0].v[2]==10&&evs[0].v[3]==0&&evs[0].v[4]==0&&evs[0].v[0]==evs[1].v[0];
            collaborators["probe_selftest"]=ready;
            nev=0;g.nloc=0;g.c=nullptr;
        }
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
