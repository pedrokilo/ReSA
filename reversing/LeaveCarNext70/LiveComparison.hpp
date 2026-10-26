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
#include "Entity/Vehicle/Automobile.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "CarEnterExit.h"
#include "DamageManager.h"
#include "tHandlingData.h"
#include "CutsceneMgr.h"
#include "World.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCarNext70 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CTaskComplexLeaveCar::CreateNextSubTask (0x6419F0, hook VIRTUAL, slot 8 de la vtable 0x86E828). Los colaboradores se sustituyen por falsos identicos en ambos lados
// (parche de entrada en la direccion del EXE y en el cuerpo de la DLL); CTaskComplexLeaveCar::CreateSubTask (0x641530) se trata como colaborador aunque otro agente la reconstruya:
// si tiene hook se parchea su cuerpo de hook, si no, el cuerpo de la envoltura de la DLL. CanPedStepOutCar se llama por la vtable del vehiculo falso (igual en ambos lados).
// Las ayudantes en linea del EXE (0x63BA00, 0x63BAB0, 0x63BAE0, 0x646CF0) corren reales en el EXE; en la DLL son funciones locales: sus colaboradores son los mismos falsos.

namespace Lc70Layout {
struct LcTask : CTaskComplex { static constexpr size_t OffSub() { return offsetof(LcTask, m_pSubTask); } };
static_assert(LcTask::OffSub() == 8, "CTaskComplex::m_pSubTask en +8");
static_assert(offsetof(CTaskComplexLeaveCar, m_pTargetVehicle) == 0xC && offsetof(CTaskComplexLeaveCar, m_nTargetDoor) == 0x10 && offsetof(CTaskComplexLeaveCar, m_nDelayTime) == 0x14
    && offsetof(CTaskComplexLeaveCar, m_bSensibleLeaveCar) == 0x18 && offsetof(CTaskComplexLeaveCar, m_bForceGetOut) == 0x19 && offsetof(CTaskComplexLeaveCar, m_bDie) == 0x1A
    && offsetof(CTaskComplexLeaveCar, m_pTaskUtilityLineUpPedWithCar) == 0x1C && offsetof(CTaskComplexLeaveCar, m_nDoorFlagsSet) == 0x20 && offsetof(CTaskComplexLeaveCar, m_nNumGettingInSet) == 0x21
    && offsetof(CTaskComplexLeaveCar, m_nDieAnimID) == 0x24 && offsetof(CTaskComplexLeaveCar, m_fDieAnimBlendDelta) == 0x28 && offsetof(CTaskComplexLeaveCar, m_fDieAnimSpeed) == 0x2C
    && offsetof(CTaskComplexLeaveCar, m_bIsInAir) == 0x30 && sizeof(CTaskComplexLeaveCar) == 0x34, "CTaskComplexLeaveCar");
static_assert(offsetof(CTaskUtilityLineUpPedWithCar, m_nDoorOpenPosType) == 0x14 && sizeof(CTaskUtilityLineUpPedWithCar) == 0x1C, "CTaskUtilityLineUpPedWithCar");
static_assert(offsetof(CPed, m_pVehicle) == 0x58C, "CPed::m_pVehicle en +0x58C");
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_apPassengers) == 0x464 && offsetof(CVehicle, m_nNumGettingIn) == 0x485
    && offsetof(CVehicle, m_nGettingInFlags) == 0x486 && offsetof(CVehicle, m_nGettingOutFlags) == 0x487 && offsetof(CVehicle, m_nDoorLock) == 0x4F8 && offsetof(CVehicle, m_nVehicleType) == 0x590
    && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CAutomobile, m_damageManager) == 0x5A0 && offsetof(CAutomobile, m_aCarNodes) == 0x648 && offsetof(tHandlingData, m_nModelFlags) == 0xCC, "CVehicle");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0, "autopiloto");
static_assert(sizeof(CVehicle) == 0x5A0 && Lc70Fix::kVehWin >= 0x680 && Lc70Fix::kPedWin >= 0x590, "ventanas de las arenas");
}

namespace Lc70Live {
using namespace Lc70Fix;

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
inline Arena<kHandArena> handA;
inline Arena<kLineOldArena> lineOldA;
inline Arena<kLineNewArena> lineNewA;
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

// ================================================================================================ CreateNextSubTask (0x6419F0)
struct State {const C* c{};unsigned player{},room{},hasDoor{},status{};};
inline State g;
inline bool Bit(u32 bits,unsigned i) {return (bits>>(i&31))&1u;}
// Tabla virtual falsa de la subtarea: la ranura 4 (GetTaskType) devuelve el tipo del caso; las demas son trampas (no deben llamarse)
inline u32 __fastcall FakeSubType(void* self,void*) {Ev& e=PushEv(17,1);e.v[0]=Tag(self);return g.c->subType;}                                                    // CTask::GetTaskType (vtable +0x10), `ret`
inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);return 0;}
inline std::array<void*,11> fakeVt{};
inline std::array<void*,64> vehVt{};
// Todos cdecl salvo los miembros (thiscall: this en ECX, `ret n`).
inline u32 __fastcall FakeCreate(void* self,void*,u32 type,u32 ped) {                                                                                            // 0x641530, `ret 8`
    Ev& e=PushEv(1,3);e.v[0]=Tag(self);e.v[1]=type;e.v[2]=Tag(uintptr_t(ped));
    return g.c->createNull?0u:(0xC0DE0000u|(type&0xFFFFu));
}
inline void* __cdecl FakeNew(size_t size) {                                                                                                                       // 0x82119A, operator new
    Ev& e=PushEv(2,1);e.v[0]=u32(size);
    return g.c->allocNull?nullptr:reinterpret_cast<void*>(lineNewA.Obj());
}
inline void* __fastcall FakeLine(void* self,void*,u32 off,u32 time,u32 type,u32 door) {                                                                           // 0x64FBB0, `ret 10h`
    Ev& e=PushEv(3,7);e.v[0]=Tag(self);
    const u32* v=reinterpret_cast<const u32*>(uintptr_t(off));
    e.v[1]=v[0];e.v[2]=v[1];e.v[3]=v[2];e.v[4]=time;e.v[5]=type;e.v[6]=door;
    *reinterpret_cast<u32*>(self)=0xC0DE0003u;return self;
}
inline u32 __fastcall FakePlayer(void* self,void*) {Ev& e=PushEv(4,1);e.v[0]=Tag(self);return Bit(g.c->playerBits,g.player++);}                                    // 0x5DF8F0
inline u32 __cdecl FakeDoorFlag(u32 veh,u32 door,u32 check) {Ev& e=PushEv(5,3);e.v[0]=Tag(uintptr_t(veh));e.v[1]=door;e.v[2]=check&0xFFu;return g.c->doorFlagRet;}   // 0x64E550
inline void __fastcall FakeSetOut(void* self,void*,u32 flags) {Ev& e=PushEv(6,2);e.v[0]=Tag(self);e.v[1]=flags&0xFFu;}                                            // 0x6D3020, `ret 4` (el EXE deja basura en los bytes altos)
inline void __fastcall FakeClearOut(void* self,void*,u32 flags) {Ev& e=PushEv(7,2);e.v[0]=Tag(self);e.v[1]=flags&0xFFu;}                                          // 0x6D3060, `ret 4`
inline void __fastcall FakeState(void* self,void*,u32 state) {Ev& e=PushEv(8,2);e.v[0]=Tag(self);e.v[1]=state;}                                                  // 0x5E4500, `ret 4`
inline u32 __cdecl FakeTargetDoor(u32 veh,u32 ped) {Ev& e=PushEv(9,2);e.v[0]=Tag(uintptr_t(veh));e.v[1]=Tag(uintptr_t(ped));return g.c->targetDoorRet;}           // 0x64F110
inline u32 __fastcall FakePassenger(void* self,void*,u32 ped) {Ev& e=PushEv(10,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));return g.c->passengerRet;}          // 0x6D1BD0, `ret 4`
inline u32 __fastcall FakeDriver(void* self,void*,u32 ped) {Ev& e=PushEv(11,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));return g.c->driverRet;}                // 0x6D1C40, `ret 4`
inline u32 __cdecl FakeRoom(u32 veh,u32 door,u32 pos) {Ev& e=PushEv(12,3);e.v[0]=Tag(uintptr_t(veh));e.v[1]=door;e.v[2]=Tag(uintptr_t(pos));return Bit(g.c->roomBits,g.room++);}   // 0x6504C0
inline u32 __cdecl FakeHasDoor(u32 veh,u32 door) {Ev& e=PushEv(13,2);e.v[0]=Tag(uintptr_t(veh));e.v[1]=door;return Bit(g.c->hasDoorBits,g.hasDoor++);}              // 0x64EE10
inline u32 __fastcall FakeGetStatus(void* self,void*,u32 comp) {Ev& e=PushEv(14,2);e.v[0]=Tag(self);e.v[1]=comp;const unsigned i=g.status++;return g.c->statusSeq[i<3?i:2];}   // 0x6C2250, `ret 4`
inline void __fastcall FakeSetStatus(void* self,void*,u32 comp,u32 status) {Ev& e=PushEv(15,3);e.v[0]=Tag(self);e.v[1]=comp;e.v[2]=status;}                        // 0x6C21E0, `ret 8`
inline u32 __fastcall FakeCanStep(void* self,void*,u32 ignore) {Ev& e=PushEv(16,2);e.v[0]=Tag(self);e.v[1]=ignore&0xFFu;return g.c->canStep;}                    // CVehicle::CanPedStepOutCar (vtable +0xE4), `ret 4`

inline void CaseNext(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);subA.Fill(c.seed*13u+2u);pedA.Fill(c.seed*13u+3u);vehA.Fill(c.seed*13u+4u);handA.Fill(c.seed*13u+5u);
        lineOldA.Fill(c.seed*13u+6u);lineNewA.Fill(c.seed*13u+7u);othA.Fill(c.seed*13u+8u);
        subA.Put32(0,u32(reinterpret_cast<uintptr_t>(fakeVt.data())));
        vehA.Put32(0,u32(reinterpret_cast<uintptr_t>(vehVt.data())));
        // tarea
        taskA.Put32(8,u32(subA.Obj()));taskA.Put32(0xC,c.vehNull?0u:u32(vehA.Obj()));taskA.Put32(0x10,c.door);taskA.Put32(0x14,c.delay);
        taskA.Put8(0x18,c.sensible);taskA.Put8(0x19,c.force);taskA.Put8(0x1A,c.die);
        taskA.Put32(0x1C,c.lineKind==0?u32(lineOldA.Obj()):c.lineKind==1?0u:c.lineRaw);
        taskA.Put8(0x20,c.flagsSet);taskA.Put8(0x21,c.numIn);taskA.Put32(0x24,c.dieAnim);taskA.Put32(0x28,c.dieBlend);taskA.Put32(0x2C,c.dieSpeed);taskA.Put8(0x30,c.inAir);
        // peaton: bInVehicle (bit 8 de +0x46C), bIsDrowning (bit 19 de +0x470) y el vehiculo
        pedA.W(0x46C)=(pedA.W(0x46C)&~0x100u)|(c.inVeh?0x100u:0u);
        pedA.W(0x470)=(pedA.W(0x470)&~0x80000u)|(c.drowning?0x80000u:0u);
        pedA.Put32(0x58C,c.pedVehKind==0?u32(vehA.Obj()):c.pedVehKind==1?u32(othA.Obj()):0u);
        // vehiculo
        vehA.Put8(0x36,c.status);vehA.Put32(0x384,u32(handA.Obj()));
        vehA.Put32(0x460,c.drvKind==0?0u:c.drvKind==1?u32(pedA.Obj()):u32(othA.Obj()));
        for(int i=0;i<3;++i)vehA.Put32(0x464+4*i,c.pass[i]?u32(othA.Obj()):0u);
        vehA.Put8(0x485,c.numGetting);vehA.Put8(0x486,c.inFlags);vehA.Put8(0x487,c.outFlags);vehA.Put32(0x4F8,c.lock);vehA.Put32(0x590,c.vType);vehA.Put32(0x594,c.vSub);
        for(int i=0;i<12;++i)vehA.Put32(0x648+4*i,(c.nodes>>i)&1u?u32(othA.Obj()):0u);
        handA.Put32(0xCC,c.modelFlags);
        taskA.Snap();subA.Snap();pedA.Snap();vehA.Snap();handA.Snap();lineOldA.Snap();lineNewA.Snap();othA.Snap();
        TagClear();taskA.Tagged();subA.Tagged();pedA.Tagged();vehA.Tagged();handA.Tagged();lineOldA.Tagged();lineNewA.Tagged();othA.Tagged();
        g.c=&c;g.player=g.room=g.hasDoor=g.status=0;nev=0;
        const u32 args[1]{u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x6419F0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["eax"]=Tag(uintptr_t(sides[s].reg.eax));
        obs["changes"]=json::array({taskA.Changes(),subA.Changes(),pedA.Changes(),vehA.Changes(),handA.Changes(),lineOldA.Changes(),lineNewA.Changes(),othA.Changes()});
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
// Cuerpo de una funcion miembro no virtual de la DLL (la que llama el codigo reconstruido, aunque el hook del EXE apunte a un envoltorio)
template<class M> inline uintptr_t MemberBody(M m) {static_assert(sizeof(M)==sizeof(uintptr_t));uintptr_t a;memcpy(&a,&m,sizeof(a));return Body(a);}
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
    if(!std::filesystem::exists(dir/"leavecarnext70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecarnext70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecarnext70");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot=0x86E828+8*4;
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t playerBody=DllBodyOf("CPed","IsPlayer");
    const uintptr_t doorFlagBody=DllBodyOf("CCarEnterExit","ComputeDoorFlag");
    const uintptr_t setOutBody=DllBodyOf("CVehicle","SetGettingOutFlags");
    const uintptr_t clearOutBody=DllBodyOf("CVehicle","ClearGettingOutFlags");
    const uintptr_t stateBody=DllBodyOf("CPed","SetPedState");
    const uintptr_t targetDoorBody=DllBodyOf("CCarEnterExit","ComputeTargetDoorToExit");
    const uintptr_t passengerBody=DllBodyOf("CVehicle","IsPassenger-Ped");
    const uintptr_t driverBody=DllBodyOf("CVehicle","IsDriver-Ped");
    const uintptr_t roomBody=DllBodyOf("CCarEnterExit","IsRoomForPedToLeaveCar");
    const uintptr_t hasDoorBody=DllBodyOf("CCarEnterExit","CarHasDoorToClose");
    const uintptr_t lineBody=FirstDllCall(env.asi,DllBodyOf("CTaskUtilityLineUpPedWithCar","Constructor"));
    // El hook de CDamageManager apunta a envoltorios locales (Zx/Hook): el codigo reconstruido llama al miembro, que es lo que se parchea en la DLL
    const uintptr_t getStatusBody=MemberBody(&CDamageManager::GetDoorStatus_Component);
    const uintptr_t setStatusBody=MemberBody(&CDamageManager::SetDoorStatus_Component);
    const bool getStatusHooked=FindHook("CDamageManager","GetDoorStatus_Component")!=nullptr,setStatusHooked=FindHook("CDamageManager","SetDoorStatus_Component")!=nullptr;
    // CreateSubTask (0x641530): con hook (lo reconstruye otro agente) se parchea el cuerpo del hook; sin hook, la envoltura de la DLL que llama a 0x641530
    const bool createHooked=FindHook("CTaskComplexLeaveCar","CreateSubTask")!=nullptr;
    const uintptr_t createBody=createHooked?DllBodyOf("CTaskComplexLeaveCar","CreateSubTask"):MemberBody(&CTaskComplexLeaveCar::CreateSubTask);
    // Ruta VIRTUAL: el cuerpo del candidato es el destino vivo del hook V
    {
        auto* hook=FindHook("CTaskComplexLeaveCar","CreateNextSubTask");
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready=false;
        env.route[0].reset(new HookRoute("CTaskComplexLeaveCar","CreateNextSubTask",0x6419F0,body));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    ready&=createBody&&playerBody&&doorFlagBody&&setOutBody&&clearOutBody&&stateBody&&targetDoorBody&&passengerBody&&driverBody&&roomBody&&hasDoorBody&&lineBody&&getStatusBody&&setStatusBody&&getStatusHooked&&setStatusHooked;
    env.stubs[0]={{Pair{0x641530,createBody,createHooked},reinterpret_cast<void*>(&FakeCreate)},
                  {Pair{0x64FBB0,lineBody,true},reinterpret_cast<void*>(&FakeLine)},
                  {Pair{0x5DF8F0,playerBody,true},reinterpret_cast<void*>(&FakePlayer)},
                  {Pair{0x64E550,doorFlagBody,true},reinterpret_cast<void*>(&FakeDoorFlag)},
                  {Pair{0x6D3020,setOutBody,true},reinterpret_cast<void*>(&FakeSetOut)},
                  {Pair{0x6D3060,clearOutBody,true},reinterpret_cast<void*>(&FakeClearOut)},
                  {Pair{0x5E4500,stateBody,true},reinterpret_cast<void*>(&FakeState)},
                  {Pair{0x64F110,targetDoorBody,true},reinterpret_cast<void*>(&FakeTargetDoor)},
                  {Pair{0x6D1BD0,passengerBody,true},reinterpret_cast<void*>(&FakePassenger)},
                  {Pair{0x6D1C40,driverBody,true},reinterpret_cast<void*>(&FakeDriver)},
                  {Pair{0x6504C0,roomBody,true},reinterpret_cast<void*>(&FakeRoom)},
                  {Pair{0x64EE10,hasDoorBody,true},reinterpret_cast<void*>(&FakeHasDoor)},
                  {Pair{0x6C2250,getStatusBody,true},reinterpret_cast<void*>(&FakeGetStatus)},
                  {Pair{0x6C21E0,setStatusBody,true},reinterpret_cast<void*>(&FakeSetStatus)},
                  {Pair{0x82119A,0,false},reinterpret_cast<void*>(&FakeNew)}};   // el ultimo: ningun otro parche puede reservar memoria con operator new ya sustituido
    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap);
    fakeVt[4]=reinterpret_cast<void*>(&FakeSubType);
    for(auto& v:vehVt)v=reinterpret_cast<void*>(&FakeTrap);
    vehVt[57]=reinterpret_cast<void*>(&FakeCanStep);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CanPedStepOutCar: llamada virtual por la vtable falsa del vehiculo (igual en ambos lados)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x6419F0,16);env.watches.emplace_back(env.vtSlot,4);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
