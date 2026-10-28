#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "Tasks/TaskManager.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "Fire.h"
#include "MBlur.h"
#include "General.h"
#include "TagManager.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "GameLogic.h"
#include "Entity/Object/Object.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Pools/PedPool.h"
#include "Pools/ObjectPool.h"
#include "Pools/VehiclePool.h"
#include "Events/Event.h"
#include "Events/EventEditableResponse.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventPlayerCommandToGroupAttack.h"
#include "Events/EventPlayerCommandToGroupGather.h"
#include "Tasks/TaskTypes/TaskSimplePlayerOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlayerPed79 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funcion 0: CPlayerPed::SetInitialState (0x60CD20, hook S). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y, si tienen hook, en el cuerpo
// de la DLL): CMBlur::ClearDrunkBlur 0x71D780 (sin hook), CPed::ClearAimFlag 0x5DEF20, ClearLookFlag 0x5E1950, CFire::Extinguish 0x5393F0, CPed::SetPedState 0x5E4500, SetMoveState 0x5DEC00,
// CPedIntelligence::FlushIntelligence 0x601DA0, RpAnimBlendClumpRemoveAllAssociations 0x4D6C00, CTask::operator new 0x61A5A0, CTaskManager::SetTask 0x681AF0 y SetTaskSecondary 0x681B60,
// CPlayerPed::SetRealMoveAnim 0x60A9C0 (sin hook), CPedGroupIntelligence::SetDefaultTaskAllocatorType 0x5FBB70, CEventEditableResponse::ComputeResponseTaskType(grupo) 0x4B57A0 y WillRespond 0x4AC490,
// CEvent::operator new/delete 0x4B5620 / 0x4B5630 (sin hook), CPedGroupIntelligence::AddEvent 0x5F7470, CPlayerPedData::SetInitialState 0x56F210, CEntity::RegisterReference 0x571B70 y
// CleanUpOldReference 0x571A00, rand() 0x821B1E (sin hook; la DLL: CGeneral::GetRandomNumber). Corren de verdad en ambos lados: los constructores de CTaskSimplePlayerOnFoot, CTaskComplexFacial,
// CEventPlayerCommandToGroupAttack/Gather y CEventGroupEvent (sobre las arenas / la pila).

namespace Pp79Layout {
struct PpPed : CPed {
    static constexpr size_t OffPlayerData() { return offsetof(PpPed, m_pPlayerData); }
    static constexpr size_t OffIntelligence() { return offsetof(PpPed, m_pIntelligence); }
    static constexpr size_t OffPedType() { return offsetof(PpPed, m_nPedType); }
};
static_assert(PpPed::OffPlayerData() == 0x480 && PpPed::OffIntelligence() == 0x47C && PpPed::OffPedType() == 0x598, "CPed: datos +0x480, inteligencia +0x47C, tipo +0x598");
static_assert(offsetof(CPlayerPedData, m_nPlayerFlags) == 0x34 && offsetof(CPlayerPedData, m_nPlayerGroup) == 0x38, "CPlayerPedData: banderas +0x34, grupo +0x38");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPedIntelligence: gestor de tareas +4");
static_assert(sizeof(CPedGroup) == 0x2D4 && sizeof(CPlayerPed) <= Pp79Fix::kPlayerWin, "tamanos");
static_assert(sizeof(CTaskSimplePlayerOnFoot) == 0x1C && sizeof(CTaskComplexFacial) == 0x20 && sizeof(CEventPlayerCommandToGroupGather) == 0x1C, "tamanos de tareas y evento");
}

namespace Pp79Live {
using namespace Pp79Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void FillByte(u8 b) {m.fill(b);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(ptrdiff_t off,u8 v) {m[kGuard+off]=v;}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPlayerArena> playerA;
inline Arena<kDataArena> dataA,otherDataA;
inline Arena<kIntelArena> intelA,otherIntelA;
inline Arena<kStatArena> statA;
inline Arena<kFireArena> fireA;
inline Arena<kTaskArena> taskA,taskB;
inline Arena<kEvArena> evA;

inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}

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
// Puntero de argumento: dentro de una arena se etiqueta; si no es un local de la pila del llamador (cambia de una pasada a otra).
inline u32 TagP(const void* p) {
    const uintptr_t v=uintptr_t(p);
    for(const auto& e:tagTable)if(v>=e.base&&v<e.base+e.size)return Tag(v);
    return p?0xDEADC0DEu:0u;
}

// ================================================================================================ SetInitialState (0x60CD20)
// Indices de arena para las etiquetas: 0 jugador, 1 sus datos, 2 los otros datos, 3 inteligencia, 4 otra inteligencia, 5 estadisticas, 6 fuego, 7 tarea A, 8 tarea B, 9 evento, 10 grupos (sin guarda).
struct State0 {const C0* c{};unsigned taskNews{};std::array<bool,kSites+1> mutDone{};};
inline State0 g0;
inline void Mutate0(u32 site) {
    if(g0.c->mutSite!=site||g0.mutDone[site])return;
    g0.mutDone[site]=true;
    const u32 k=g0.c->mutKind;
    if(k&1)playerA.Put32(0x480,playerA.Get32(0x480)==u32(dataA.Obj())?u32(otherDataA.Obj()):u32(dataA.Obj()));
    if(k&2)playerA.Put32(0x47C,playerA.Get32(0x47C)==u32(intelA.Obj())?u32(otherIntelA.Obj()):u32(intelA.Obj()));
    if(k&4){playerA.Put32(0x46C,g0.c->mutF46C);playerA.Put32(0x474,g0.c->mutF474);}
    if(k&8){playerA.Put32(0x1C,g0.c->mutF1C);playerA.Put32(0xFC,g0.c->mutFC);}
    if(k&16){const u32 d=playerA.Get32(0x480);if(d==u32(dataA.Obj()))dataA.Put32(0x34,g0.c->mutDFlags);else if(d==u32(otherDataA.Obj()))otherDataA.Put32(0x34,g0.c->mutDFlags);}
}

// Todos __fastcall (ECX = this, EDX sin uso, pila) con la limpieza `ret n` del original, o __cdecl.
inline void __cdecl FakeDrunk() {PushEv(22,0);}                                                                                                    // 0x71D780 (sin sitio de mutacion)
inline void __fastcall FakeClearAim(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=Tag(self);Mutate0(1);}                                                       // 0x5DEF20
inline void __fastcall FakeClearLook(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);Mutate0(2);}                                                      // 0x5E1950
inline void __fastcall FakeExtinguish(void* self,void*) {Ev& e=PushEv(3,1);e.v[0]=Tag(self);Mutate0(3);}                                                     // 0x5393F0
inline void __fastcall FakeSetState(void* self,void*,u32 s) {Ev& e=PushEv(4,2);e.v[0]=Tag(self);e.v[1]=s;Mutate0(4);}                                        // 0x5E4500, `ret 4`
inline void __fastcall FakeSetMove(void* self,void*,u32 s) {Ev& e=PushEv(5,2);e.v[0]=Tag(self);e.v[1]=s;Mutate0(5);}                                         // 0x5DEC00, `ret 4`
inline void __fastcall FakeFlush(void* self,void*) {Ev& e=PushEv(6,1);e.v[0]=Tag(self);Mutate0(6);}                                                          // 0x601DA0
inline void __cdecl FakeRemoveAssoc(u32 clump) {Ev& e=PushEv(7,1);e.v[0]=clump;Mutate0(7);}                                                                  // 0x4D6C00
inline void* __cdecl FakeTaskNew(u32 size) {                                                                                                                 // 0x61A5A0
    Ev& e=PushEv(8,1);e.v[0]=size;
    const unsigned k=g0.taskNews++;
    Mutate0(k==0?8:11);
    if(k==0)return g0.c->taskFail1?nullptr:reinterpret_cast<void*>(taskA.Obj());
    return g0.c->taskFail2?nullptr:reinterpret_cast<void*>(taskB.Obj());
}
inline void __fastcall FakeSetTask(void* self,void*,void* task,u32 idx,u32 unused) {Ev& e=PushEv(9,4);e.v[0]=Tag(self);e.v[1]=Tag(task);e.v[2]=idx;e.v[3]=unused&0xFFu;Mutate0(9);}   // 0x681AF0, `ret 0Ch`
inline void __fastcall FakeRealMove(void* self,void*) {Ev& e=PushEv(10,1);e.v[0]=Tag(self);Mutate0(10);}                                                     // 0x60A9C0
inline void __fastcall FakeSetTask2(void* self,void*,void* task,u32 idx) {Ev& e=PushEv(11,3);e.v[0]=Tag(self);e.v[1]=Tag(task);e.v[2]=idx;Mutate0(12);}           // 0x681B60, `ret 8`
inline void __fastcall FakeAllocType(void* self,void*,u32 type) {Ev& e=PushEv(12,2);e.v[0]=Tag(self);e.v[1]=type;Mutate0(13);}                                 // 0x5FBB70, `ret 4`
inline void __fastcall FakeCompute(void* self,void*,void* group) {                                                                                           // 0x4B57A0, `ret 4`
    Ev& e=PushEv(13,2);e.v[0]=TagP(self);e.v[1]=Tag(group);
    const u16 w=u16(g0.c->taskId);memcpy(static_cast<uint8*>(self)+0xE,&w,2);
    Mutate0(14);
}
inline u32 __fastcall FakeWill(void* self,void*) {Ev& e=PushEv(14,1);e.v[0]=TagP(self);Mutate0(15);return g0.c->will;}                                        // 0x4AC490
inline void* __cdecl FakeEventNew(u32 size) {Ev& e=PushEv(15,1);e.v[0]=size;Mutate0(16);return reinterpret_cast<void*>(evA.Obj());}                          // 0x4B5620
inline void __cdecl FakeEventDelete(void* p) {Ev& e=PushEv(20,1);e.v[0]=Tag(p);}                                                                             // 0x4B5630
inline u32 __fastcall FakeAddEvent(void* self,void*,void* event) {                                                                                           // 0x5F7470, `ret 4`
    const auto* ge=static_cast<const CEventGroupEvent*>(event);
    Ev& e=PushEv(16,6);
    e.v[0]=Tag(self);e.v[1]=u32(static_cast<CEvent*>(event)->GetEventType());e.v[2]=Tag(uintptr_t(ge->m_ped));e.v[3]=Tag(uintptr_t(ge->m_event));
    if(ge->m_event) {e.v[4]=u32(ge->m_event->GetEventType());u16 w{};memcpy(&w,reinterpret_cast<const uint8*>(ge->m_event)+0xE,2);e.v[5]=w;} else {e.v[4]=0xFFFFFFFFu;e.v[5]=0;}
    Mutate0(17);
    return 0xA5A5A500u;
}
inline void __fastcall FakePlayerData(void* self,void*) {Ev& e=PushEv(17,1);e.v[0]=Tag(self);Mutate0(18);}                                                   // 0x56F210
inline void __fastcall FakeRegister(void* self,void*,void** ref) {Ev& e=PushEv(18,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}   // 0x571B70, `ret 4`
inline void __fastcall FakeCleanUp(void* self,void*,void** ref) {Ev& e=PushEv(19,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}    // 0x571A00, `ret 4`
inline u32 __cdecl FakeRand() {PushEv(21,0);return g0.c->randRet&0xFFFFu;}                                                                                     // 0x821B1E y CGeneral::GetRandomNumber de la DLL

inline void CaseInit0(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        playerA.Fill(c.seed*29u+1u);dataA.Fill(c.seed*29u+2u);otherDataA.Fill(c.seed*29u+3u);intelA.Fill(c.seed*29u+4u);otherIntelA.Fill(c.seed*29u+5u);statA.Fill(c.seed*29u+6u);
        fireA.Fill(c.seed*29u+7u);taskA.FillByte(0xCC);taskB.FillByte(0xCC);evA.FillByte(0xCC);
        playerA.Put32(0x1C,c.f1C);playerA.Put32(0x40,c.f40);playerA.Put32(0x46C,c.f46C);playerA.Put32(0x474,c.f474);playerA.Put32(0xFC,c.fFC);playerA.Put32(0x784,c.f784);playerA.Put32(0x4D4,c.f4D4);
        playerA.Put32(0x730,c.fire?u32(fireA.Obj()):0u);playerA.Put32(0x18,c.clump);playerA.Put32(0x47C,u32(intelA.Obj()));playerA.Put32(0x480,c.hasData?u32(dataA.Obj()):0u);playerA.Put32(0x59C,u32(statA.Obj()));
        dataA.Put32(0x34,c.dflags);dataA.Put32(0x38,c.group);otherDataA.Put32(0x34,c.odflags);otherDataA.Put32(0x38,c.ogroup);
        *reinterpret_cast<uint32*>(0xB7CB64)=c.timeScale;*reinterpret_cast<uint32*>(0xC73C58)=c.drunk;
        u8 before[8];for(int i=0;i<8;++i){before[i]=u8(Rand(c.grpFlags,u32(i))&0xFFu);*reinterpret_cast<u8*>(kGroups+i*0x2D4+4)=before[i];}
        playerA.Snap();dataA.Snap();otherDataA.Snap();intelA.Snap();otherIntelA.Snap();statA.Snap();fireA.Snap();taskA.Snap();taskB.Snap();evA.Snap();
        TagClear();playerA.Tagged();dataA.Tagged();otherDataA.Tagged();intelA.Tagged();otherIntelA.Tagged();statA.Tagged();fireA.Tagged();taskA.Tagged();taskB.Tagged();evA.Tagged();
        TagAdd(reinterpret_cast<void*>(kGroups),kGroupsSize);
        g0=State0{};g0.c=&c;nev=0;
        const u32 args[1]{c.arg};
        sides[s]=Execute(env,0,0x60CD20,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(playerA.Obj()),c.edx,c.stack,0);});
        memset(evA.m.data()+kGuard,0xCC,4);   // el evento ya esta destruido (vptr de objeto muerto: la vtable base del EXE o de la DLL segun el lado; el vptr de un objeto vivo tampoco es comparable: el EXE escribe 0x86D120 y la DLL su propia vtable)
        json obs;
        obs["changes"]=json::array({playerA.Changes(),dataA.Changes(),otherDataA.Changes(),intelA.Changes(),otherIntelA.Changes(),statA.Changes(),fireA.Changes(),taskA.Changes(),taskB.Changes(),evA.Changes()});
        obs["events"]=EvsJson();
        json gb=json::array(),ga=json::array();
        for(int i=0;i<8;++i){gb.push_back(before[i]);ga.push_back(*reinterpret_cast<u8*>(kGroups+i*0x2D4+4));}
        obs["groups_before"]=gb;obs["groups_after"]=ga;
        obs["time_scale"]=*reinterpret_cast<uint32*>(0xB7CB64);obs["drunk"]=*reinterpret_cast<uint32*>(0xC73C58);
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

}
#include "LiveComparison1.hpp"
namespace Pp79Live {
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
    json cases=json::object();cases[kFn[0]]=kCases0;cases[kFn[1]]=kCases1;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kCases0+kCases1},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        unsigned before=totals.failures;
        for(unsigned c=0;c<kCases0;++c)CaseInit0(env,totals,c);
        Write({{"event","function_summary"},{"function",kFn[0]},{"cases",kCases0},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        before=totals.failures;
        for(unsigned c=0;c<kCases1;++c)CaseLock1(env,totals,c);
        Write({{"event","function_summary"},{"function",kFn[1]},{"cases",kCases1},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
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
    if(!std::filesystem::exists(dir/"playerped79-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playerped79-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playerped79");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t drunkBody=Addr(&CMBlur::ClearDrunkBlur);
    const uintptr_t aimBody=Addr(&CPed::ClearAimFlag);
    const uintptr_t lookBody=Addr(&CPed::ClearLookFlag);
    const uintptr_t fireBody=Addr(&CFire::Extinguish);
    const uintptr_t stateBody=Addr(&CPed::SetPedState);
    const uintptr_t moveBody=Addr(&CPed::SetMoveState);
    const uintptr_t flushBody=Addr(&CPedIntelligence::FlushIntelligence);
    const uintptr_t assocBody=Addr(&RpAnimBlendClumpRemoveAllAssociations);
    const uintptr_t taskNewBody=Addr(static_cast<void*(*)(size_t)>(&CTask::operator new));
    const uintptr_t setTaskBody=Addr(&CTaskManager::SetTask);
    const uintptr_t realMoveBody=Addr(&CPlayerPed::SetRealMoveAnim);
    const uintptr_t setTask2Body=Addr(&CTaskManager::SetTaskSecondary);
    const uintptr_t allocBody=Addr(&CPedGroupIntelligence::SetDefaultTaskAllocatorType);
    const uintptr_t computeBody=Addr(static_cast<void(CEventEditableResponse::*)(CPedGroup*)>(&CEventEditableResponse::ComputeResponseTaskType));
    const uintptr_t willBody=Addr(&CEventEditableResponse::WillRespond);
    const uintptr_t eventNewBody=Addr(static_cast<void*(*)(unsigned)>(&CEvent::operator new));
    const uintptr_t eventDeleteBody=Addr(static_cast<void(*)(void*)>(&CEvent::operator delete));
    const uintptr_t addEventBody=Addr(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent));
    const uintptr_t playerDataBody=Addr(&CPlayerPedData::SetInitialState);
    const uintptr_t registerBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t cleanBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t randBody=Addr(&CGeneral::GetRandomNumber);
    env.route[0].reset(new HookRoute("CPlayerPed","SetInitialState",0x60CD20,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=drunkBody&&aimBody&&lookBody&&fireBody&&stateBody&&moveBody&&flushBody&&assocBody&&taskNewBody&&setTaskBody&&realMoveBody&&setTask2Body&&allocBody&&computeBody&&willBody&&eventNewBody
         &&eventDeleteBody&&addEventBody&&playerDataBody&&registerBody&&cleanBody&&randBody;
    env.stubs[0]={{Pair{0x71D780,drunkBody,false},reinterpret_cast<void*>(&FakeDrunk)},
                  {Pair{0x5DEF20,aimBody,true},reinterpret_cast<void*>(&FakeClearAim)},
                  {Pair{0x5E1950,lookBody,true},reinterpret_cast<void*>(&FakeClearLook)},
                  {Pair{0x5393F0,fireBody,true},reinterpret_cast<void*>(&FakeExtinguish)},
                  {Pair{0x5E4500,stateBody,true},reinterpret_cast<void*>(&FakeSetState)},
                  {Pair{0x5DEC00,moveBody,true},reinterpret_cast<void*>(&FakeSetMove)},
                  {Pair{0x601DA0,flushBody,true},reinterpret_cast<void*>(&FakeFlush)},
                  {Pair{0x4D6C00,assocBody,true},reinterpret_cast<void*>(&FakeRemoveAssoc)},
                  {Pair{0x61A5A0,taskNewBody,true},reinterpret_cast<void*>(&FakeTaskNew)},
                  {Pair{0x681AF0,setTaskBody,true},reinterpret_cast<void*>(&FakeSetTask)},
                  {Pair{0x60A9C0,realMoveBody,false},reinterpret_cast<void*>(&FakeRealMove)},
                  {Pair{0x681B60,setTask2Body,true},reinterpret_cast<void*>(&FakeSetTask2)},
                  {Pair{0x5FBB70,allocBody,true},reinterpret_cast<void*>(&FakeAllocType)},
                  {Pair{0x4B57A0,computeBody,true},reinterpret_cast<void*>(&FakeCompute)},
                  {Pair{0x4AC490,willBody,true},reinterpret_cast<void*>(&FakeWill)},
                  {Pair{0x4B5620,eventNewBody,false},reinterpret_cast<void*>(&FakeEventNew)},
                  {Pair{0x4B5630,eventDeleteBody,false},reinterpret_cast<void*>(&FakeEventDelete)},
                  {Pair{0x5F7470,addEventBody,true},reinterpret_cast<void*>(&FakeAddEvent)},
                  {Pair{0x56F210,playerDataBody,true},reinterpret_cast<void*>(&FakePlayerData)},
                  {Pair{0x571B70,registerBody,true},reinterpret_cast<void*>(&FakeRegister)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&FakeCleanUp)},
                  {Pair{0x821B1E,randBody,false},reinterpret_cast<void*>(&FakeRand)}};
    RegisterFn1(env,ready,registered,bodies);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked},{"state_jumps",ReversingProbe::JumpsToDll(s.pair.exe)},{"state_module_ok",s.pair.exe?ModuleOf(Resolve(s.pair.exe))==env.asi:false}});
        }
        collaborators[kFn[f]]={{"dll_real",f==0?json::array({"CTaskSimplePlayerOnFoot (constructor)","CTaskComplexFacial (constructor)","CEventPlayerCommandToGroupAttack/Gather (constructor y destructor)","CEventGroupEvent (constructor y destructor)"})
                               :json::array({"CPad::GetPad (devuelve &Pads[n])","aritmetica en linea: restas, productos, comparaciones y bucles de normalizacion del angulo","ayudantes x87 de X87Math.cpp (Pp79*)","PlayerPedATanMinus"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60CD20,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
