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
#include "Pad.h"
#include "Acquaintance.h"
#include "PedType.h"
#include "Wanted.h"
#include "Timer.h"
#include "Pools/PedPool.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlayerGroup78 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::ProcessGroupBehaviour (0x60D350, hook S). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y, si tienen
// hook, en el cuerpo de la DLL): FindPlayerVehicle 0x56E0D0, CPad::GetGroupControlForward 0x541190 y GetGroupControlBack 0x5411B0, CPlayerPed::DisbandPlayerGroup 0x60A0A0,
// TellGroupToStartFollowingPlayer 0x60A1D0 y MakeThisPedJoinOurGroup 0x60C840, FindPlayerPed 0x56E210, CPedType::GetPedFlag 0x608830, CAcquaintance::SetAsAcquaintance 0x608DA0 y
// ClearAsAcquaintance 0x608980 (todos con hook); y sin hook: la funcion 0x609380 (sin reconstruir; la DLL la llama por la direccion del EXE).
// El pool de peatones (0xB74490) se apunta a un pool falso en arenas (cabecera, estados de ranura y 6 peatones de 0x7C4 bytes); el EXE lo lee del mismo puntero global.
// Mutaciones: en el sitio elegido, un falso cambia el puntero a los datos del jugador (+0x480), las banderas/contadores de los datos, el objetivo (+0x71C/+0x79C) o el nivel de busqueda,
// para distinguir lecturas repetidas de lecturas conservadas (p. ej. el puntero edi de las pulsaciones).

namespace Pg78Layout {
struct PgPlayer : CPlayerPed {
    static constexpr size_t OffPlayerData() { return offsetof(PgPlayer, m_pPlayerData); }
    static constexpr size_t OffTargeted() { return offsetof(PgPlayer, m_pTargetedObject); }
    static constexpr size_t OffMouse() { return offsetof(PgPlayer, m_p3rdPersonMouseTarget); }
    static constexpr size_t OffAcquaintance() { return offsetof(PgPlayer, m_acquaintance); }
    static constexpr size_t OffPedType() { return offsetof(PgPlayer, m_nPedType); }
};
static_assert(PgPlayer::OffPlayerData() == 0x480 && PgPlayer::OffTargeted() == 0x71C && PgPlayer::OffMouse() == 0x79C && PgPlayer::OffAcquaintance() == 0x4E0 && PgPlayer::OffPedType() == 0x598,
              "CPed/CPlayerPed: datos +0x480, objetivo +0x71C, objetivo del raton +0x79C, relaciones +0x4E0, tipo +0x598");
static_assert(offsetof(CPlayerPedData, m_nPlayerFlags) == 0x34 && offsetof(CPlayerPedData, m_nPadDownPressedInMilliseconds) == 0x88 && offsetof(CPlayerPedData, m_nPadUpPressedInMilliseconds) == 0x8A && offsetof(CPlayerPedData, m_pWanted) == 0,
              "CPlayerPedData: banderas +0x34, abajo +0x88, arriba +0x8A, CWanted +0");
static_assert(offsetof(CWanted, m_WantedLevel) == 0x2C, "CWanted: nivel +0x2C");
static_assert(sizeof(CCopPed) == 0x7C4 && sizeof(CPlayerPed) <= Pg78Fix::kPlayerWin && sizeof(CPed) <= Pg78Fix::kPedWin, "tamanos");
}

namespace Pg78Live {
using namespace Pg78Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(ptrdiff_t off,u32 v) {const u16 w=u16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(ptrdiff_t off,u32 v) {m[kGuard+off]=u8(v);}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPlayerArena> playerA,fpPlayerA;
inline Arena<kDataArena> dataA,otherDataA,fpDataA;
inline Arena<kPedArena> tgtA,mouseA;
inline Arena<kPadArena> padA;
inline Arena<kWantedArena> wantedA;
inline Arena<kPoolArena> poolA;
inline Arena<kSlotsArena> slotsA;
inline Arena<kPedsArena> pedsA;

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

// ================================================================================================ ProcessGroupBehaviour (0x60D350)
struct State {const C* c{};unsigned veh{};std::array<bool,10> mutDone{};};
inline State g;
// Mutacion en el sitio `site` (una sola vez por sitio): kind&1 cambia el puntero a los datos del jugador (+0x480) al otro; kind&2 fija las banderas de los datos vigentes (tras el cambio);
// kind&4 fija los contadores arriba/abajo de los datos vigentes; kind&8 intercambia el objetivo (+0x71C) y el del raton (+0x79C). El sitio 9 ademas escribe el nivel de busqueda.
inline void Mutate(u32 site) {
    if(g.c->mutSite!=site||g.mutDone[site])return;
    g.mutDone[site]=true;
    const u32 k=g.c->mutKind;
    if(k&1)playerA.Put32(0x480,playerA.Get32(0x480)==u32(dataA.Obj())?u32(otherDataA.Obj()):u32(dataA.Obj()));
    auto& cur=playerA.Get32(0x480)==u32(dataA.Obj())?dataA:otherDataA;
    if(k&2)cur.Put32(0x34,g.c->mutFlags);
    if(k&4){cur.Put16(0x8A,g.c->mutUp);cur.Put16(0x88,g.c->mutDown);}
    if(k&8){const u32 a=playerA.Get32(0x71C),b=playerA.Get32(0x79C);playerA.Put32(0x71C,b);playerA.Put32(0x79C,a);}
    if(site==9)wantedA.Put32(0x2C,g.c->mutWanted);
}

// Todos __fastcall (ECX = this, EDX sin uso, pila) con la limpieza `ret n` del original, o __cdecl.
inline u32 __cdecl FakeVehicle(u32 id,u32 remote) {                                                                                           // 0x56E0D0 (FindPlayerVehicle)
    Ev& e=PushEv(1,2);e.v[0]=id;e.v[1]=remote&0xFFu;
    const bool first=g.veh++==0;
    Mutate(first?1:7);
    return first?g.c->veh1:g.c->veh2;
}
inline u32 __fastcall FakePadFwd(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);Mutate(2);return (g.c->up&1u)|(g.c->upHi&~0xFFu);}                  // 0x541190
inline u32 __fastcall FakePadBack(void* self,void*) {Ev& e=PushEv(3,1);e.v[0]=Tag(self);Mutate(8);return (g.c->down&1u)|(g.c->downHi&~0xFFu);}          // 0x5411B0
inline void __fastcall FakeDisband(void* self,void*) {Ev& e=PushEv(4,1);e.v[0]=Tag(self);Mutate(3);}                                                       // 0x60A0A0
inline void __fastcall FakeTell(void* self,void*,u32 a,u32 b,u32 c) {Ev& e=PushEv(5,4);e.v[0]=Tag(self);e.v[1]=a&0xFFu;e.v[2]=b&0xFFu;e.v[3]=c&0xFFu;Mutate(6);}   // 0x60A1D0, `ret 0Ch`
inline void __fastcall FakeJoin(void* self,void*,void* ped) {Ev& e=PushEv(6,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);Mutate(5);}                              // 0x60C840, `ret 4`
inline u32 __cdecl FakeJoinFlags() {PushEv(7,0);Mutate(4);return (g.c->jf&1u)|(g.c->jfHi&~0xFFu);}                                                         // 0x609380
inline void* __cdecl FakePlayer(u32 id) {Ev& e=PushEv(8,1);e.v[0]=id;return reinterpret_cast<void*>(fpPlayerA.Obj());}                                    // 0x56E210
inline u32 __cdecl FakePedFlag(u32 type) {Ev& e=PushEv(9,1);e.v[0]=type;Mutate(9);return g.c->pedFlag;}                                                    // 0x608830
inline void __fastcall FakeSetAcq(void* self,void*,u32 id,u32 flag) {Ev& e=PushEv(10,3);e.v[0]=Tag(self);e.v[1]=id;e.v[2]=flag;}                           // 0x608DA0, `ret 8`
inline void __fastcall FakeClearAcq(void* self,void*,u32 id,u32 flag) {Ev& e=PushEv(11,3);e.v[0]=Tag(self);e.v[1]=id;e.v[2]=flag;}                        // 0x608980, `ret 8`

inline void CaseGroup(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        playerA.Fill(c.seed*23u+1u);fpPlayerA.Fill(c.seed*23u+2u);dataA.Fill(c.seed*23u+3u);otherDataA.Fill(c.seed*23u+4u);fpDataA.Fill(c.seed*23u+5u);tgtA.Fill(c.seed*23u+6u);mouseA.Fill(c.seed*23u+7u);
        padA.Fill(c.seed*23u+8u);wantedA.Fill(c.seed*23u+9u);poolA.Fill(c.seed*23u+10u);slotsA.Fill(c.seed*23u+11u);pedsA.Fill(c.seed*23u+12u);
        playerA.Put32(0x480,u32(dataA.Obj()));playerA.Put32(0x71C,c.tgt?u32(tgtA.Obj()):0u);playerA.Put32(0x79C,c.mtgt?u32(mouseA.Obj()):0u);
        for(auto* d:{&dataA,&otherDataA}) {const bool main=d==&dataA;d->Put32(0x34,main?c.flags:c.oflags);d->Put16(0x8A,main?c.upT:c.oupT);d->Put16(0x88,main?c.downT:c.odownT);}
        tgtA.Put8(0x36,c.tType);tgtA.Put32(0x598,c.tPed);mouseA.Put8(0x36,c.mType);mouseA.Put32(0x598,c.mPed);
        fpPlayerA.Put32(0x480,u32(fpDataA.Obj()));fpDataA.Put32(0,u32(wantedA.Obj()));wantedA.Put32(0x2C,c.wanted);
        poolA.Put32(0,u32(pedsA.Obj()));poolA.Put32(4,u32(slotsA.Obj()));poolA.Put32(8,c.poolSize);poolA.Put32(0xC,0xFFFFFFFFu);
        for(unsigned i=0;i<kPoolPeds;++i){slotsA.Put8(i,c.slot[i]);pedsA.Put32(i*0x7C4+0x598,c.spt[i]);}
        *reinterpret_cast<uint8*>(0xB6EC2E)=u8(c.mouse);*reinterpret_cast<uint32*>(0xB7CB4C)=c.frame;*reinterpret_cast<uint32*>(0xB7CB58)=c.ts;*reinterpret_cast<uint32*>(0xB74490)=u32(poolA.Obj());
        playerA.Snap();fpPlayerA.Snap();dataA.Snap();otherDataA.Snap();fpDataA.Snap();tgtA.Snap();mouseA.Snap();padA.Snap();wantedA.Snap();poolA.Snap();slotsA.Snap();pedsA.Snap();
        TagClear();playerA.Tagged();dataA.Tagged();otherDataA.Tagged();tgtA.Tagged();mouseA.Tagged();padA.Tagged();fpPlayerA.Tagged();fpDataA.Tagged();wantedA.Tagged();poolA.Tagged();slotsA.Tagged();pedsA.Tagged();
        g=State{};g.c=&c;nev=0;
        const u32 args[1]{u32(padA.Obj())};
        sides[s]=Execute(env,0,0x60D350,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(playerA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({playerA.Changes(),dataA.Changes(),otherDataA.Changes(),tgtA.Changes(),mouseA.Changes(),padA.Changes(),fpPlayerA.Changes(),fpDataA.Changes(),wantedA.Changes(),
                                    poolA.Changes(),slotsA.Changes(),pedsA.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseGroup(env,totals,c);
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
    if(!std::filesystem::exists(dir/"playergroup78-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playergroup78-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playergroup78");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t vehicleBody=Addr(&FindPlayerVehicle);
    const uintptr_t padFwdBody=Addr(&CPad::GetGroupControlForward);
    const uintptr_t padBackBody=Addr(&CPad::GetGroupControlBack);
    const uintptr_t disbandBody=Addr(&CPlayerPed::DisbandPlayerGroup);
    const uintptr_t tellBody=Addr(&CPlayerPed::TellGroupToStartFollowingPlayer);
    const uintptr_t joinBody=Addr(&CPlayerPed::MakeThisPedJoinOurGroup);
    const uintptr_t playerBody=Addr(&FindPlayerPed);
    const uintptr_t pedFlagBody=Addr(&CPedType::GetPedFlag);
    const uintptr_t setAcqBody=Addr(&CAcquaintance::SetAsAcquaintance);
    const uintptr_t clearAcqBody=Addr(&CAcquaintance::ClearAsAcquaintance);
    env.route[0].reset(new HookRoute("CPlayerPed","ProcessGroupBehaviour",0x60D350,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=vehicleBody&&padFwdBody&&padBackBody&&disbandBody&&tellBody&&joinBody&&playerBody&&pedFlagBody&&setAcqBody&&clearAcqBody;
    env.stubs[0]={{Pair{0x56E0D0,vehicleBody,true},reinterpret_cast<void*>(&FakeVehicle)},
                  {Pair{0x541190,padFwdBody,true},reinterpret_cast<void*>(&FakePadFwd)},
                  {Pair{0x5411B0,padBackBody,true},reinterpret_cast<void*>(&FakePadBack)},
                  {Pair{0x60A0A0,disbandBody,true},reinterpret_cast<void*>(&FakeDisband)},
                  {Pair{0x60A1D0,tellBody,true},reinterpret_cast<void*>(&FakeTell)},
                  {Pair{0x60C840,joinBody,true},reinterpret_cast<void*>(&FakeJoin)},
                  {Pair{0x609380,0,false},reinterpret_cast<void*>(&FakeJoinFlags)},
                  {Pair{0x56E210,playerBody,true},reinterpret_cast<void*>(&FakePlayer)},
                  {Pair{0x608830,pedFlagBody,true},reinterpret_cast<void*>(&FakePedFlag)},
                  {Pair{0x608DA0,setAcqBody,true},reinterpret_cast<void*>(&FakeSetAcq)},
                  {Pair{0x608980,clearAcqBody,true},reinterpret_cast<void*>(&FakeClearAcq)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"conversion a entero (_ftol2 / int64) del paso de tiempo","GetAt / GetSize del pool de peatones (CPool, sobre el pool falso)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60D350,16);
    env.watches.emplace_back(0x858B38,4);   // 0.02f (.rdata): constante que multiplica el paso de tiempo
    env.watches.emplace_back(0x858C4C,4);   // 1000.0f (.rdata): segunda constante
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
