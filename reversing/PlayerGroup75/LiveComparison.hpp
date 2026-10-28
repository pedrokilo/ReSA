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
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "Stats.h"
#include "Radar.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Events/Event.h"
#include "Events/EventGroup.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventNewGangMember.h"
#include "Events/EventDontJoinPlayerGroup.h"
#include "Tasks/TaskTypes/TaskComplexBeInGroup.h"
#include "Pools/PedPool.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlayerGroup75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::MakeThisPedJoinOurGroup (0x60C840, hook S). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y, si tienen
// hook, en el cuerpo de la DLL): CPed::Say 0x5EFFE0, CTaskManager::FindActiveTaskByType 0x681740, CPedGroupMembership::IsMember 0x5F6A10, CAEPedSpeechAudioEntity::SetCJMood 0x4E3ED0 (sin hook en todos los bancos: --unhook=Audio; falso en la direccion del EXE y en el cuerpo de la DLL),
// CStats::FindMaxNumberOfGroupMembers 0x559A50 y GetStatValue 0x558E40, CPedGroupMembership::CountMembersExcludingLeader 0x5F6AA0, CPedGroups::GetPedsGroup 0x5F7E80,
// CPedGroupMembership::RemoveMember(CPed*) 0x5FB210, CTask::operator new 0x61A5A0 y delete 0x61A5B0, FindPlayerPed 0x56E210, CEventGroup::Add 0x4AB420, CPedGroupMembership::AddFollower 0x5F8020,
// CPedGroup::Process 0x5FC7E0, CPed::GiveWeaponWhenJoiningGang 0x5E8BE0, CPedGroupIntelligence::AddEvent 0x5F7470, CStats::IncrementStat 0x55C180 y DisplayScriptStatUpdateMessage 0x55B980,
// CPedPool::GetRef 0x4442D0, CRadar::SetEntityBlip 0x5839A0, ChangeBlipScale 0x583CC0, ChangeBlipColour 0x583AB0 y SetBlipFriendly 0x583EB0, CEntity::RegisterReference 0x571B70 y
// CleanUpOldReference 0x571A00 (todos con hook); y sin hook: la funcion 0x609380 (sin reconstruir) y CEvent::operator new 0x4B5620 / delete 0x4B5630 (cuerpo propio en la DLL).
// Corren de verdad en ambos lados: los constructores y destructores de CTaskComplexBeInGroup, CEventScriptCommand, CEventNewGangMember, CEventGroupEvent y CEventDontJoinPlayerGroup (sobre las arenas).

namespace Pg75Layout {
struct PgPed : CPed {
    static constexpr size_t OffPlayerData() { return offsetof(PgPed, m_pPlayerData); }
    static constexpr size_t OffIntelligence() { return offsetof(PgPed, m_pIntelligence); }
    static constexpr size_t OffPedType() { return offsetof(PgPed, m_nPedType); }
};
static_assert(PgPed::OffPlayerData() == 0x480 && PgPed::OffIntelligence() == 0x47C && PgPed::OffPedType() == 0x598, "CPed: datos del jugador en +0x480, inteligencia en +0x47C y tipo en +0x598");
static_assert(offsetof(CPlayerPedData, m_nPlayerGroup) == 0x38 && offsetof(CPlayerPedData, m_nScriptLimitToGangSize) == 0x43, "CPlayerPedData: grupo en +0x38 y limite del script en +0x43");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4 && offsetof(CPedIntelligence, m_eventGroup) == 0x68, "CPedIntelligence: gestor de tareas en +4 y grupo de eventos en +0x68");
static_assert(sizeof(CPedGroup) == 0x2D4 && sizeof(CPed) <= Pg75Fix::kPedWin && sizeof(CPlayerPed) <= Pg75Fix::kPlayerWin && sizeof(CTaskComplexBeInGroup) <= Pg75Fix::kTaskWin, "tamanos");
static_assert(sizeof(CEventNewGangMember) <= Pg75Fix::kEvWin, "tamano del evento");
}

namespace Pg75Live {
using namespace Pg75Fix;

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
inline Arena<kPlayerArena> playerA,otherPlayerA;
inline Arena<kDataArena> dataA,otherDataA;
inline Arena<kPedArena> pedA;
inline Arena<kIntelArena> intelA;
inline Arena<kTaskArena> taskA;
inline Arena<kEvArena> evA;
inline Arena<kOtherGroupArena> otherGroupA;
constexpr uintptr_t kGroups=0xC09920;
constexpr size_t kGroupsSize=0x16A0;

inline uintptr_t Body2(uintptr_t p) {return Body(p);}
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

// ================================================================================================ MakeThisPedJoinOurGroup (0x60C840)
struct State {const C* c{};unsigned stat{};unsigned count{};};
inline State g;
// Puntero de argumento: dentro de una arena se etiqueta; si no es un local de la pila del llamador (cambia de una pasada a otra).
inline u32 TagP(const void* p) {
    const uintptr_t v=uintptr_t(p);
    for(const auto& e:tagTable)if(v>=e.base&&v<e.base+e.size)return Tag(v);
    return p?0xDEADC0DEu:0u;
}
inline void PutVec(Ev& e,u32 at,const void* v) {const u32* w=static_cast<const u32*>(v);e.v[at]=w[0];e.v[at+1]=w[1];e.v[at+2]=w[2];}
// Mutacion del indice de grupo y del limite del jugador (datos del jugador `this`) en el sitio `site`
inline void Mutate(u32 site) {if(g.c->mutSite==site){dataA.Put32(0x38,g.c->mutGroup);dataA.Put8(0x43,u8(g.c->mutLimit));}}

// Todos __fastcall (ECX = this, EDX sin uso, pila) con la limpieza `ret n` del original. Los floats viajan como dword en la pila. Los que devuelven bool/AL llevan basura en los bits altos de EAX.
inline u32 __fastcall FakeSay(void* self,void*,u32 ctx,u32 delay,u32 prob,u32 a,u32 b,u32 c) {                                                                               // 0x5EFFE0, `ret 18h`
    Ev& e=PushEv(1,7);e.v[0]=Tag(self);e.v[1]=ctx;e.v[2]=delay;e.v[3]=prob;e.v[4]=a&0xFFu;e.v[5]=b&0xFFu;e.v[6]=c&0xFFu;return 0xA5A50000u;
}
inline void* __fastcall FakeFindTask(void* self,void*,u32 type) {Ev& e=PushEv(2,2);e.v[0]=Tag(self);e.v[1]=type;return g.c->hasTask?reinterpret_cast<void*>(intelA.Obj(0x10)):nullptr;}  // 0x681740, `ret 4`
inline u32 __fastcall FakeIsMember(void* self,void*,void* ped) {Ev& e=PushEv(3,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);Mutate(1);return (g.c->isMember&1u)|(g.c->memberHi&~0xFFu);}   // 0x5F6A10, `ret 4`
inline void __cdecl FakeMood(u32 a1,u32 a2,u32 a3,u32 a4,u32 a5) {Ev& e=PushEv(4,5);e.v[0]=a1;e.v[1]=a2;e.v[2]=a3;e.v[3]=a4;e.v[4]=a5;Mutate(2);}                        // 0x4E3ED0
inline int __cdecl FakeFindMax() {PushEv(5,0);Mutate(3);return int(g.c->maxMembers);}                                                                                         // 0x559A50
inline float __cdecl FakeStat(u32 stat) {Ev& e=PushEv(6,1);e.v[0]=stat;return F(g.stat++==0?g.c->stat1:g.c->stat2);}                                                          // 0x558E40
inline u32 __cdecl FakeJoinFlags() {PushEv(7,0);return (g.c->joinFlags&1u)|(g.c->joinHi&~0xFFu);}                                                                              // 0x609380
inline u32 __fastcall FakeCount(void* self,void*) {Ev& e=PushEv(8,1);e.v[0]=Tag(self);const u32 r=g.c->counts[g.count<3?g.count:3];if(g.count++==0)Mutate(4);return r;}     // 0x5F6AA0
inline void* __cdecl FakePedsGroup(void* ped) {Ev& e=PushEv(9,1);e.v[0]=Tag(ped);return g.c->pedsGroup?reinterpret_cast<void*>(otherGroupA.Obj()):nullptr;}                  // 0x5F7E80
inline void __fastcall FakeRemove(void* self,void*,void* ped) {Ev& e=PushEv(10,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);}                                                            // 0x5FB210, `ret 4`
inline void* __cdecl FakeTaskNew(u32 size) {Ev& e=PushEv(11,1);e.v[0]=size;return g.c->taskFail?nullptr:reinterpret_cast<void*>(taskA.Obj());}                                // 0x61A5A0
inline void __cdecl FakeTaskDelete(void* p) {Ev& e=PushEv(28,1);e.v[0]=Tag(p);}                                                                                                // 0x61A5B0
inline void* __cdecl FakePlayer(u32 id) {Ev& e=PushEv(12,1);e.v[0]=id;return reinterpret_cast<void*>(g.c->otherPlayer?otherPlayerA.Obj():playerA.Obj());}                  // 0x56E210
// Un evento temporal (en la pila) visto desde CEventGroup::Add: tipo (por la tabla virtual, igual en el EXE y la DLL) y los campos propios
inline void* __fastcall FakeAdd(void* group,void*,void* event,u32 valid) {                                                                                                   // 0x4AB420, `ret 8`
    CEvent* const ev=static_cast<CEvent*>(event);
    Ev& e=PushEv(13,8);
    e.v[0]=Tag(group);e.v[1]=u32(ev->GetEventType());e.v[2]=u32(ev->m_nTimeActive);e.v[3]=ev->m_bValid?1u:0u;
    if(ev->GetEventType()==EVENT_SCRIPT_COMMAND) {
        const auto* sc=static_cast<const CEventScriptCommand*>(ev);
        e.v[4]=u32(sc->m_primaryTaskIndex);e.v[5]=Tag(uintptr_t(sc->m_task));e.v[6]=sc->m_affectsDeadPeds?1u:0u;
    } else {
        e.v[4]=Tag(uintptr_t(static_cast<const CEventDontJoinPlayerGroup*>(ev)->m_player));
    }
    e.v[7]=valid&0xFFu;
    return event;
}
inline void __fastcall FakeAddFollower(void* self,void*,void* ped) {Ev& e=PushEv(14,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);Mutate(5);}                                            // 0x5F8020, `ret 4`
inline void __fastcall FakeProcess(void* self,void*) {Ev& e=PushEv(15,1);e.v[0]=Tag(self);}                                                                                     // 0x5FC7E0
inline void __fastcall FakeGiveWeapon(void* self,void*) {Ev& e=PushEv(16,1);e.v[0]=Tag(self);if(g.c->mutFlags)pedA.Put32(0x470,g.c->mutFlagValue470);}                        // 0x5E8BE0
inline void* __cdecl FakeEventNew(u32 size) {Ev& e=PushEv(17,1);e.v[0]=size;return g.c->eventFail?nullptr:reinterpret_cast<void*>(evA.Obj());}                              // 0x4B5620
inline void __cdecl FakeEventDelete(void* p) {Ev& e=PushEv(29,1);e.v[0]=Tag(p);}                                                                                               // 0x4B5630
inline void __fastcall FakeRegister(void* self,void*,void** ref) {Ev& e=PushEv(18,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}              // 0x571B70, `ret 4`
inline void __fastcall FakeCleanUp(void* self,void*,void** ref) {Ev& e=PushEv(19,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}              // 0x571A00, `ret 4`
inline u32 __fastcall FakeAddEvent(void* self,void*,void* event) {                                                                                                             // 0x5F7470, `ret 4`
    const auto* ge=static_cast<const CEventGroupEvent*>(event);
    Ev& e=PushEv(20,7);
    e.v[0]=Tag(self);e.v[1]=u32(static_cast<CEvent*>(event)->GetEventType());e.v[2]=Tag(uintptr_t(ge->m_ped));e.v[3]=Tag(uintptr_t(ge->m_event));
    if(ge->m_event) {
        e.v[4]=u32(ge->m_event->GetEventType());e.v[5]=Tag(uintptr_t(static_cast<const CEventNewGangMember*>(ge->m_event)->m_member));
    } else {
        e.v[4]=0xFFFFFFFFu;e.v[5]=0;
    }
    return 0xA5A5A500u;
}
inline void __cdecl FakeIncrement(u32 stat,u32 value) {Ev& e=PushEv(21,2);e.v[0]=stat;e.v[1]=value;}                                                                          // 0x55C180
inline void __cdecl FakeDisplay(u32 a1,u32 a2,u32 a3) {Ev& e=PushEv(22,3);e.v[0]=a1;e.v[1]=a2;e.v[2]=a3;}                                                                       // 0x55B980
inline u32 __fastcall FakeGetRef(void* pool,void*,void* ped) {Ev& e=PushEv(23,2);e.v[0]=u32(uintptr_t(pool));e.v[1]=Tag(ped);Mutate(6);return g.c->ref;}                        // 0x4442D0, `ret 4`
inline u32 __cdecl FakeBlip(u32 type,u32 ref,u32 color,u32 display) {Ev& e=PushEv(24,4);e.v[0]=type;e.v[1]=ref;e.v[2]=color;e.v[3]=display;Mutate(7);return g.c->blip;}         // 0x5839A0
inline void __cdecl FakeBlipScale(u32 blip,u32 size) {Ev& e=PushEv(25,2);e.v[0]=blip;e.v[1]=size;}                                                                              // 0x583CC0
inline void __cdecl FakeBlipColour(u32 blip,u32 color) {Ev& e=PushEv(26,2);e.v[0]=blip;e.v[1]=color;}                                                                          // 0x583AB0
inline void __cdecl FakeBlipFriendly(u32 blip,u32 friendly) {Ev& e=PushEv(27,2);e.v[0]=blip;e.v[1]=friendly&0xFFu;if(g.c->mutFlags)pedA.Put32(0x474,g.c->mutFlagValue474);}     // 0x583EB0

inline void CaseJoin(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        playerA.Fill(c.seed*19u+1u);otherPlayerA.Fill(c.seed*19u+2u);dataA.Fill(c.seed*19u+3u);otherDataA.Fill(c.seed*19u+4u);pedA.Fill(c.seed*19u+5u);intelA.Fill(c.seed*19u+6u);
        otherGroupA.Fill(c.seed*19u+7u);taskA.FillByte(0xCC);evA.FillByte(0xCC);
        playerA.Put32(0x480,u32(dataA.Obj()));otherPlayerA.Put32(0x480,u32(otherDataA.Obj()));
        dataA.Put32(0x38,c.group);dataA.Put8(0x43,u8(c.limit));otherDataA.Put32(0x38,c.otherGroup);otherDataA.Put8(0x43,u8(c.otherLimit));
        pedA.Put32(0x478,c.flags478);pedA.Put32(0x470,c.flags470);pedA.Put32(0x474,c.flags474);pedA.Put32(0x47C,u32(intelA.Obj()));pedA.Put32(0x598,c.pedType);
        *reinterpret_cast<uint8*>(0x96917C)=u8(c.flagA);*reinterpret_cast<uint8*>(0x96917D)=u8(c.flagB);*reinterpret_cast<uint8*>(0x96917E)=u8(c.flagC);
        *reinterpret_cast<uint8*>(0x8D1345)=u8(c.colorA);*reinterpret_cast<uint8*>(0x8D1351)=u8(c.colorB);*reinterpret_cast<uint8*>(0x8D135D)=u8(c.colorC);
        playerA.Snap();otherPlayerA.Snap();dataA.Snap();otherDataA.Snap();pedA.Snap();intelA.Snap();taskA.Snap();evA.Snap();otherGroupA.Snap();
        TagClear();playerA.Tagged();dataA.Tagged();otherPlayerA.Tagged();otherDataA.Tagged();pedA.Tagged();intelA.Tagged();taskA.Tagged();evA.Tagged();otherGroupA.Tagged();
        TagAdd(reinterpret_cast<void*>(kGroups),kGroupsSize);
        g=State{};g.c=&c;nev=0;
        u32 before[8];for(int i=0;i<8;++i)before[i]=*reinterpret_cast<u32*>(kGroups+i*0x2D4+0x2C);
        const u32 args[1]{u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x60C840,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(playerA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({playerA.Changes(),dataA.Changes(),otherPlayerA.Changes(),otherDataA.Changes(),pedA.Changes(),intelA.Changes(),taskA.Changes(),evA.Changes(),otherGroupA.Changes()});
        obs["events"]=EvsJson();
        json sepBefore=json::array(),sepAfter=json::array();
        for(int i=0;i<8;++i){sepBefore.push_back(before[i]);sepAfter.push_back(*reinterpret_cast<u32*>(kGroups+i*0x2D4+0x2C));}
        obs["separation_before"]=sepBefore;obs["separation_after"]=sepAfter;
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
            for(unsigned c=0;c<kCases;++c)CaseJoin(env,totals,c);
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

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playergroup75-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playergroup75-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playergroup75");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t sayBody=Addr(&CPed::Say);
    const uintptr_t findTaskBody=Addr(&CTaskManager::FindActiveTaskByType);
    const uintptr_t isMemberBody=Addr(&CPedGroupMembership::IsMember);
    const uintptr_t moodBody=Addr(&CAEPedSpeechAudioEntity::SetCJMood);
    const uintptr_t findMaxBody=Addr(&CStats::FindMaxNumberOfGroupMembers);
    const uintptr_t statBody=Addr(static_cast<float(*)(eStats)>(&CStats::GetStatValue));
    const uintptr_t countBody=Addr(&CPedGroupMembership::CountMembersExcludingLeader);
    const uintptr_t pedsGroupBody=Addr(&CPedGroups::GetPedsGroup);
    const uintptr_t removeBody=Addr(static_cast<void(CPedGroupMembership::*)(CPed*)>(&CPedGroupMembership::RemoveMember));
    const uintptr_t taskNewBody=Addr(static_cast<void*(*)(size_t)>(&CTask::operator new));
    const uintptr_t taskDeleteBody=Addr(static_cast<void(*)(void*)>(&CTask::operator delete));
    const uintptr_t playerBody=Addr(&FindPlayerPed);
    const uintptr_t addBody=Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add));
    const uintptr_t followerBody=Addr(&CPedGroupMembership::AddFollower);
    const uintptr_t processBody=Addr(&CPedGroup::Process);
    const uintptr_t giveBody=Addr(&CPed::GiveWeaponWhenJoiningGang);
    const uintptr_t eventNewBody=Addr(static_cast<void*(*)(unsigned)>(&CEvent::operator new));
    const uintptr_t eventDeleteBody=Addr(static_cast<void(*)(void*)>(&CEvent::operator delete));
    const uintptr_t registerBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t cleanBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t addEventBody=Addr(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent));
    const uintptr_t incrementBody=Addr(&CStats::IncrementStat);
    const uintptr_t displayBody=Addr(&CStats::DisplayScriptStatUpdateMessage);
    const uintptr_t getRefBody=Addr(&CPedPool::GetRef);
    const uintptr_t blipBody=Addr(&CRadar::SetEntityBlip);
    const uintptr_t scaleBody=Addr(&CRadar::ChangeBlipScale);
    const uintptr_t colourBody=Addr(&CRadar::ChangeBlipColour);
    const uintptr_t friendlyBody=Addr(&CRadar::SetBlipFriendly);
    env.route[0].reset(new HookRoute("CPlayerPed","MakeThisPedJoinOurGroup",0x60C840,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=sayBody&&findTaskBody&&isMemberBody&&moodBody&&findMaxBody&&statBody&&countBody&&pedsGroupBody&&removeBody&&taskNewBody&&taskDeleteBody&&playerBody&&addBody&&followerBody&&processBody&&giveBody
         &&eventNewBody&&eventDeleteBody&&registerBody&&cleanBody&&addEventBody&&incrementBody&&displayBody&&getRefBody&&blipBody&&scaleBody&&colourBody&&friendlyBody;
    env.stubs[0]={{Pair{0x5EFFE0,sayBody,true},reinterpret_cast<void*>(&FakeSay)},
                  {Pair{0x681740,findTaskBody,true},reinterpret_cast<void*>(&FakeFindTask)},
                  {Pair{0x5F6A10,isMemberBody,true},reinterpret_cast<void*>(&FakeIsMember)},
                  {Pair{0x4E3ED0,moodBody,false},reinterpret_cast<void*>(&FakeMood)},
                  {Pair{0x559A50,findMaxBody,true},reinterpret_cast<void*>(&FakeFindMax)},
                  {Pair{0x558E40,statBody,true},reinterpret_cast<void*>(&FakeStat)},
                  {Pair{0x609380,0,false},reinterpret_cast<void*>(&FakeJoinFlags)},
                  {Pair{0x5F6AA0,countBody,true},reinterpret_cast<void*>(&FakeCount)},
                  {Pair{0x5F7E80,pedsGroupBody,true},reinterpret_cast<void*>(&FakePedsGroup)},
                  {Pair{0x5FB210,removeBody,true},reinterpret_cast<void*>(&FakeRemove)},
                  {Pair{0x61A5A0,taskNewBody,true},reinterpret_cast<void*>(&FakeTaskNew)},
                  {Pair{0x61A5B0,taskDeleteBody,true},reinterpret_cast<void*>(&FakeTaskDelete)},
                  {Pair{0x56E210,playerBody,true},reinterpret_cast<void*>(&FakePlayer)},
                  {Pair{0x4AB420,addBody,true},reinterpret_cast<void*>(&FakeAdd)},
                  {Pair{0x5F8020,followerBody,true},reinterpret_cast<void*>(&FakeAddFollower)},
                  {Pair{0x5FC7E0,processBody,true},reinterpret_cast<void*>(&FakeProcess)},
                  {Pair{0x5E8BE0,giveBody,true},reinterpret_cast<void*>(&FakeGiveWeapon)},
                  {Pair{0x4B5620,eventNewBody,false},reinterpret_cast<void*>(&FakeEventNew)},
                  {Pair{0x4B5630,eventDeleteBody,false},reinterpret_cast<void*>(&FakeEventDelete)},
                  {Pair{0x571B70,registerBody,true},reinterpret_cast<void*>(&FakeRegister)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&FakeCleanUp)},
                  {Pair{0x5F7470,addEventBody,true},reinterpret_cast<void*>(&FakeAddEvent)},
                  {Pair{0x55C180,incrementBody,true},reinterpret_cast<void*>(&FakeIncrement)},
                  {Pair{0x55B980,displayBody,true},reinterpret_cast<void*>(&FakeDisplay)},
                  {Pair{0x4442D0,getRefBody,true},reinterpret_cast<void*>(&FakeGetRef)},
                  {Pair{0x5839A0,blipBody,true},reinterpret_cast<void*>(&FakeBlip)},
                  {Pair{0x583CC0,scaleBody,true},reinterpret_cast<void*>(&FakeBlipScale)},
                  {Pair{0x583AB0,colourBody,true},reinterpret_cast<void*>(&FakeBlipColour)},
                  {Pair{0x583EB0,friendlyBody,true},reinterpret_cast<void*>(&FakeBlipFriendly)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTaskComplexBeInGroup (constructor y destructor)","CEventScriptCommand (constructor y destructor)","CEventNewGangMember (constructor y destructor)","CEventGroupEvent (constructor y destructor)","CEventDontJoinPlayerGroup (constructor y destructor)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60C840,16);
    env.watches.emplace_back(0x86C6C0,4);   // separacion maxima del grupo del jugador (120.0f, .rdata): lo que la funcion copia al grupo
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
