#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/Ped.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Tasks/TaskManager.h"
#include "PedIntelligence.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventGroup.h"
#include "RwHelper.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "Fx/Fx.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FightHitPed75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CTaskSimpleFight::FightHitPed (0x61CBA0, hook S). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y, si tienen hook,
// en el cuerpo de la DLL): CPed::IsPlayer 0x5DF8F0, CTaskManager::GetActiveTask 0x681720, CPedIntelligence::GetTaskFighting 0x600F30, CWeaponInfo::GetWeaponInfo 0x743C60,
// CTaskSimpleFight::FightStrikeDamage 0x61C740, CPedGeometryAnalyser::ComputePedShotSide 0x5F13F0, CWeapon::GenerateDamageEvent 0x73A530, CPed::Say 0x5EFFE0, CPed::IsAlive 0x5E0170,
// GetEventGlobalGroup 0x4ABA50, CEventGroup::Add 0x4AB420, CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00 (con hook); y SIN hook en ningun lado (la DLL llama a la direccion
// del EXE): BeHitWhileBlocking 0x61C650, CAEPedAudioEntity::AddAudioEvent 0x4E2BB0 y CAEPedWeaponAudioEntity::AddAudioEvent 0x4E69F0 (--unhook=Audio), CVector::Normalise 0x59C910, Fx_c::AddBlood
// 0x49EB00 y rand() 0x821B1E. CTask::GetTaskType es una llamada virtual por la tabla de la tarea activa (vtable falsa). CEventSoundQuiet (constructor y destructor) corre de verdad en ambos lados.

namespace Fp75Layout {
struct FpPed : CPed {
    static constexpr size_t OffPedAE() { return offsetof(FpPed, m_pedAudio); }
    static constexpr size_t OffWeaponAE() { return offsetof(FpPed, m_weaponAudio); }
    static constexpr size_t OffIntelligence() { return offsetof(FpPed, m_pIntelligence); }
    static constexpr size_t OffHealth() { return offsetof(FpPed, m_fHealth); }
    static constexpr size_t OffActiveSlot() { return offsetof(FpPed, m_nActiveWeaponSlot); }
};
static_assert(FpPed::OffPedAE() == 0x138 && FpPed::OffWeaponAE() == 0x394, "CPed: m_pedAudio en +0x138 y m_weaponAudio en +0x394");
static_assert(FpPed::OffActiveSlot() == 0x718 && offsetof(CPed, m_aWeapons) == 0x5A0 && sizeof(CWeapon) == 0x1C, "CPed: ranura activa en +0x718, armas en +0x5A0 de 0x1C bytes");
static_assert(FpPed::OffIntelligence() == 0x47C && FpPed::OffHealth() == 0x540, "CPed: inteligencia en +0x47C y vida en +0x540");
static_assert(offsetof(CPhysical, m_fContactSurfaceBrightness) == 0x12C, "CPhysical: brillo en +0x12C");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CTaskSimpleFight, m_nContinueStrike) == 0x14 && offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nCurrentMove) == 0x25, "CTaskSimpleFight");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPedIntelligence: gestor de tareas en +4");
static_assert(sizeof(CPed) <= Fp75Fix::kPedWin && sizeof(CTaskSimpleFight) <= Fp75Fix::kTaskWin, "ventanas de las arenas");
}

namespace Fp75Live {
using namespace Fp75Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(ptrdiff_t off,u8 v) {m[kGuard+off]=v;}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kPedArena> creatorA,victimA;
inline Arena<kMatArena> matCA,matVA;
inline Arena<kVecArena> p1A,p2A;
inline Arena<kIntelArena> intelA;
inline Arena<kBlockerArena> blockerA;
inline Arena<kActiveArena> activeA;
inline Arena<kGroupArena> groupA;

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

// ================================================================================================ FightHitPed (0x61CBA0)
struct State {const C* c{};unsigned peAe{};unsigned alive{};};
inline State g;
// Puntero de argumento: dentro de una arena se etiqueta; si no es un local de la pila del llamador (cambia de una pasada a otra).
inline u32 TagP(const void* p) {
    const uintptr_t v=uintptr_t(p);
    for(const auto& e:tagTable)if(v>=e.base&&v<e.base+e.size)return Tag(v);
    return p?0xDEADC0DEu:0u;
}
inline void PutVec(Ev& e,u32 at,const void* v) {const u32* w=static_cast<const u32*>(v);e.v[at]=w[0];e.v[at+1]=w[1];e.v[at+2]=w[2];}
// Mutacion de combo y movimiento de la tarea en el sitio `site` (el EXE y la DLL deben releer o no releer exactamente igual)
inline void Mutate(u32 site) {if(g.c->mutSite==site){taskA.Put8(0x24,u8(g.c->mutCombo));taskA.Put8(0x25,u8(g.c->mutMove));}}
inline bool IsVictim(const void* p) {return uintptr_t(p)==victimA.Obj();}

// Tabla virtual falsa de la tarea activa: ranura 4 (+0x10) = CTask::GetTaskType; las demas son trampas (no deben llamarse)
inline u32 __fastcall FakeTaskType(void* self,void*) {Ev& e=PushEv(3,1);e.v[0]=Tag(self);return g.c->taskType;}
inline void __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);}
inline std::array<void*,16> fakeVt{};

// Todos __fastcall (ECX = this, EDX sin uso, pila) con la limpieza `ret n` del original. Los floats viajan como dword en la pila. Los que devuelven bool/AL llevan basura en los bits altos de EAX.
inline u32 __fastcall FakeIsPlayer(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=Tag(self);const u32 r=IsVictim(self)?g.c->victimPlayer:g.c->creatorPlayer;return (r&1u)|(g.c->aliveHi&~0xFFu);}      // 0x5DF8F0
inline void* __fastcall FakeActiveTask(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);return reinterpret_cast<void*>(activeA.Obj());}                                                    // 0x681720
inline void* __fastcall FakeFighting(void* self,void*) {Ev& e=PushEv(4,1);e.v[0]=Tag(self);return g.c->hasFighting?reinterpret_cast<void*>(blockerA.Obj()):nullptr;}                      // 0x600F30
inline u32 __fastcall FakeBeHit(void* self,void*,void* victim,void* creator,u32 combo,u32 move) {                                                                                           // 0x61C650, `ret 10h`
    Ev& e=PushEv(5,5);e.v[0]=Tag(self);e.v[1]=Tag(victim);e.v[2]=Tag(creator);e.v[3]=combo;e.v[4]=move;
    Mutate(7);
    return (g.c->blocked&1u)|(g.c->blockedHi&~0xFFu);
}
inline void __fastcall FakePedAE(void* self,void*,u32 event,u32 volume,u32 speed,void* entity,u32 surface,u32 a7,u32 maxVol) {                                                              // 0x4E2BB0, `ret 1Ch`
    Ev& e=PushEv(6,8);e.v[0]=Tag(self);e.v[1]=event;e.v[2]=volume;e.v[3]=speed;e.v[4]=Tag(entity);e.v[5]=surface;e.v[6]=a7;e.v[7]=maxVol;
    if(g.peAe++==0)Mutate(3);
}
inline void* __cdecl FakeWeaponInfo(u32 type,u32 skill) {Ev& e=PushEv(7,2);e.v[0]=type;e.v[1]=skill&0xFFu;return nullptr;}                                                                  // 0x743C60
inline float __fastcall FakeDamage(void* self,void*,void* ped) {                                                                                                                            // 0x61C740, `ret 4`, devuelve ST0
    Ev& e=PushEv(8,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);
    Mutate(1);
    return F(g.c->damage);
}
inline u32 __cdecl FakeShotSide(void* victim,void* pos) {Ev& e=PushEv(9,5);e.v[0]=Tag(victim);e.v[1]=TagP(pos);PutVec(e,2,pos);return g.c->side;}                                                  // 0x5F13F0
inline u32 __cdecl FakeGenerate(void* victim,void* creator,u32 weapon,u32 damage,u32 piece,u32 dir) {                                                                                       // 0x73A530, cdecl
    Ev& e=PushEv(10,6);e.v[0]=Tag(victim);e.v[1]=Tag(creator);e.v[2]=weapon;e.v[3]=damage;e.v[4]=piece;e.v[5]=dir&0xFFu;
    Mutate(2);
    if(g.c->gdeHealthMut)victimA.Put32(0x540,g.c->gdeHealth);
    if(g.c->mutWeapon) {   // el arma activa cambia aqui: el EXE relee el tipo de arma despues
        const int slot=int(int8(creatorA.m[kGuard+0x718]));
        const ptrdiff_t at=0x5A0+slot*0x1C;
        const u32 cur=creatorA.Get32(at);
        creatorA.Put32(at,g.c->mutWeapon==1?(cur==kChainsaw?7u:u32(kChainsaw)):u32(kChainsaw));
    }
    return (g.c->gdeOk&1u)|(g.c->gdeHi&~0xFFu);
}
inline void __fastcall FakeWeaponAE(void* self,void*,u32 event) {Ev& e=PushEv(11,2);e.v[0]=Tag(self);e.v[1]=event;}                                                                           // 0x4E69F0, `ret 4`
inline u32 __fastcall FakeSay(void* self,void*,u32 ctx,u32 delay,u32 prob,u32 a,u32 b,u32 c) {                                                                                              // 0x5EFFE0, `ret 18h`
    Ev& e=PushEv(12,7);e.v[0]=Tag(self);e.v[1]=ctx;e.v[2]=delay;e.v[3]=prob;e.v[4]=a&0xFFu;e.v[5]=b&0xFFu;e.v[6]=c&0xFFu;return 0xA5A50000u|(g.c->creatorPlayer&1u);
}
inline void* __cdecl FakeGroup() {PushEv(13,0);return reinterpret_cast<void*>(groupA.Obj());}                                                                                                  // 0x4ABA50
inline json VtableName(u32 v) {
    static const u32 ours=u32(uintptr_t(ReversibleHooks::detail::GetVTableAddress("CEventSoundQuiet")));
    return v==0x86C2B0u||v==ours?json("CEventSoundQuiet"):json::array({"vtable?",v});
}
inline u32 NameCode(u32 v) {const json n=VtableName(v);return n.is_string()?1u:v;}
template<class T> inline T Get(const void* p,size_t o) {T v;std::memcpy(&v,static_cast<const uint8*>(p)+o,sizeof(T));return v;}
inline void* __fastcall FakeAdd(void* group,void*,void* event,u32 valid) {                                                                                                                  // 0x4AB420, `ret 8`
    const auto* b=static_cast<const uint8*>(event);
    Ev& e=PushEv(14,14);
    e.v[0]=Tag(group);e.v[1]=NameCode(Get<u32>(b,0));e.v[2]=Get<u32>(b,4);e.v[3]=b[8];e.v[4]=b[0xC];e.v[5]=Get<u16>(b,0xE);e.v[6]=Get<u16>(b,0x10);e.v[7]=Tag(uintptr_t(Get<u32>(b,0x14)));
    e.v[8]=Get<u32>(b,0x18);e.v[9]=Get<u32>(b,0x1C);PutVec(e,10,b+0x20);e.v[13]=valid&0xFFu;
    Mutate(4);
    return event;
}
inline void __fastcall FakeRegister(void* self,void*,void** ref) {Ev& e=PushEv(15,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}                                      // 0x571B70, `ret 4`
inline void __fastcall FakeCleanUp(void* self,void*,void** ref) {Ev& e=PushEv(16,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}                              // 0x571A00, `ret 4`
inline int __cdecl FakeRand() {                                                                                                                                                            // 0x821B1E
    PushEv(17,0);
    Mutate(5);
    if(g.c->mutP1)for(int i=0;i<3;++i)p1A.Put32(4*i,g.c->mutP[i]);
    return int(g.c->randValue);
}
inline u32 __fastcall FakeIsAlive(void* self,void*) {Ev& e=PushEv(18,1);e.v[0]=Tag(self);const u32 r=g.alive++==0?g.c->alive1:g.c->alive2;return (r&1u)|(g.c->aliveHi&~0xFFu);}                 // 0x5E0170
inline void __fastcall FakeNormalise(void* vec,void*) {                                                                                                                                      // 0x59C910, in situ
    Ev& e=PushEv(19,3);PutVec(e,0,vec);
    Mutate(6);
    if(g.c->normMode){u32* w=static_cast<u32*>(vec);for(int i=0;i<3;++i)w[i]=g.c->normOut[i];}
}
inline void __fastcall FakeBlood(void* fx,void*,void* origin,void* dir,u32 amount,u32 light) {                                                                                             // 0x49EB00, `ret 10h`
    Ev& e=PushEv(20,11);e.v[0]=u32(uintptr_t(fx));e.v[1]=TagP(origin);PutVec(e,2,origin);e.v[5]=TagP(dir);PutVec(e,6,dir);e.v[9]=amount;e.v[10]=light;
}

inline void CaseHit(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*17u+1u);creatorA.Fill(c.seed*17u+2u);victimA.Fill(c.seed*17u+3u);matCA.Fill(c.seed*17u+4u);matVA.Fill(c.seed*17u+5u);p1A.Fill(c.seed*17u+6u);p2A.Fill(c.seed*17u+7u);
        intelA.Fill(c.seed*17u+8u);blockerA.Fill(c.seed*17u+9u);activeA.Fill(c.seed*17u+10u);groupA.Fill(c.seed*17u+11u);
        // tarea: banderas de la tarea, combo y movimiento (bytes con signo), +0x14 sin el valor que escribe la funcion
        taskA.Put8(0x14,0);taskA.Put8(0x24,u8(c.combo));taskA.Put8(0x25,u8(c.move));
        // atacante: ranura activa (byte +0x718), tipo de arma de la ranura (+0x5A0 + ranura * 0x1C), matriz o posicion compacta
        creatorA.Put8(0x718,u8(c.slot));
        creatorA.Put32(0x5A0+ptrdiff_t(int(int8(c.slot)))*0x1C,c.weaponType);
        creatorA.Put32(0x14,c.creatorMat?u32(matCA.Obj()):0u);
        // victima: inteligencia, vida, brillo, matriz o posicion compacta
        victimA.Put32(0x47C,u32(intelA.Obj()));victimA.Put32(0x540,c.health);victimA.Put32(0x12C,c.brightness);
        victimA.Put32(0x14,c.victimMat?u32(matVA.Obj()):0u);
        for(int i=0;i<3;++i) {
            if(c.creatorMat)matCA.Put32(0x30+4*i,c.cpos[i]);else creatorA.Put32(4+4*i,c.cpos[i]);
            if(c.victimMat)matVA.Put32(0x30+4*i,c.vpos[i]);else victimA.Put32(4+4*i,c.vpos[i]);
            p1A.Put32(4*i,c.p1[i]);p2A.Put32(4*i,c.p2[i]);
        }
        activeA.Put32(0,u32(reinterpret_cast<uintptr_t>(fakeVt.data())));
        taskA.Snap();creatorA.Snap();victimA.Snap();matCA.Snap();matVA.Snap();p1A.Snap();p2A.Snap();intelA.Snap();blockerA.Snap();activeA.Snap();groupA.Snap();
        TagClear();taskA.Tagged();creatorA.Tagged();victimA.Tagged();matCA.Tagged();matVA.Tagged();p1A.Tagged();p2A.Tagged();intelA.Tagged();blockerA.Tagged();activeA.Tagged();groupA.Tagged();
        g=State{};g.c=&c;nev=0;
        const u32 args[5]{u32(creatorA.Obj()),u32(victimA.Obj()),u32(p1A.Obj()),u32(p2A.Obj()),c.component};
        sides[s]=Execute(env,0,0x61CBA0,candidate,[&](uintptr_t entry){return Gate(entry,args,5,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({taskA.Changes(),creatorA.Changes(),victimA.Changes(),matCA.Changes(),matVA.Changes(),p1A.Changes(),p2A.Changes(),intelA.Changes(),blockerA.Changes(),activeA.Changes(),groupA.Changes()});
        obs["events"]=EvsJson();
        obs["result"]=Tag(uintptr_t(sides[s].reg.eax));
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
            for(unsigned c=0;c<kCases;++c)CaseHit(env,totals,c);
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
// Cuerpo de la DLL de una funcion con hook S (Simple): el destino vivo del hook
inline uintptr_t DllBodyOf(const char* cls,const char* fn) {
    auto* h=FindHook(cls,fn);if(!h||strcmp(h->Symbol(),"S"))return 0;
    const uintptr_t a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a?Body(a):0;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fighthitped75-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fighthitped75-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fighthitped75");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores con hook: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t isPlayerBody=Addr(&CPed::IsPlayer);
    const uintptr_t activeBody=Addr(&CTaskManager::GetActiveTask);
    const uintptr_t fightingBody=Addr(&CPedIntelligence::GetTaskFighting);
    const uintptr_t weaponInfoBody=Addr(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo));
    const uintptr_t damageBody=DllBodyOf("CTaskSimpleFight","FightStrikeDamage");
    const uintptr_t shotSideBody=Addr(&CPedGeometryAnalyser::ComputePedShotSide);
    const uintptr_t generateBody=Addr(&CWeapon::GenerateDamageEvent);
    const uintptr_t sayBody=Addr(&CPed::Say);
    const uintptr_t aliveBody=Addr(&CPed::IsAlive);
    const uintptr_t groupBody=Addr(&GetEventGlobalGroup);
    const uintptr_t addBody=Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add));
    const uintptr_t registerBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t cleanBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t bloodBody=Addr(&Fx_c::AddBlood);   // lote 79A: AddBlood es C++ real (hook S) en la DLL
    env.route[0].reset(new HookRoute("CTaskSimpleFight","FightHitPed",0x61CBA0,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=isPlayerBody&&activeBody&&fightingBody&&weaponInfoBody&&damageBody&&shotSideBody&&generateBody&&sayBody&&aliveBody&&groupBody&&addBody&&registerBody&&cleanBody&&bloodBody;
    env.stubs[0]={{Pair{0x5DF8F0,isPlayerBody,true},reinterpret_cast<void*>(&FakeIsPlayer)},
                  {Pair{0x681720,activeBody,true},reinterpret_cast<void*>(&FakeActiveTask)},
                  {Pair{0x600F30,fightingBody,true},reinterpret_cast<void*>(&FakeFighting)},
                  {Pair{0x61C650,0,false},reinterpret_cast<void*>(&FakeBeHit)},
                  {Pair{0x4E2BB0,0,false},reinterpret_cast<void*>(&FakePedAE)},
                  {Pair{0x743C60,weaponInfoBody,true},reinterpret_cast<void*>(&FakeWeaponInfo)},
                  {Pair{0x61C740,damageBody,true},reinterpret_cast<void*>(&FakeDamage)},
                  {Pair{0x5F13F0,shotSideBody,true},reinterpret_cast<void*>(&FakeShotSide)},
                  {Pair{0x73A530,generateBody,true},reinterpret_cast<void*>(&FakeGenerate)},
                  {Pair{0x4E69F0,0,false},reinterpret_cast<void*>(&FakeWeaponAE)},
                  {Pair{0x5EFFE0,sayBody,true},reinterpret_cast<void*>(&FakeSay)},
                  {Pair{0x4ABA50,groupBody,true},reinterpret_cast<void*>(&FakeGroup)},
                  {Pair{0x4AB420,addBody,true},reinterpret_cast<void*>(&FakeAdd)},
                  {Pair{0x571B70,registerBody,true},reinterpret_cast<void*>(&FakeRegister)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&FakeCleanUp)},
                  {Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)},
                  {Pair{0x5E0170,aliveBody,true},reinterpret_cast<void*>(&FakeIsAlive)},
                  {Pair{0x59C910,0,false},reinterpret_cast<void*>(&FakeNormalise)},
                  {Pair{0x49EB00,bloodBody,true},reinterpret_cast<void*>(&FakeBlood)}};
    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap);
    fakeVt[4]=reinterpret_cast<void*>(&FakeTaskType);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CEventSoundQuiet::CEventSoundQuiet","CEventSoundQuiet::~CEventSoundQuiet"})},{"virtual_fake","CTask::GetTaskType (vtable +0x10 de la tarea activa falsa)"},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x61CBA0,16);
    env.watches.emplace_back(0xC16900,0x1200);   // tabla de combos y su entorno (el oraculo la lee de aqui)
    env.watches.emplace_back(0x8D2D00,0x300);    // retardos por movimiento (0x8D2E3C) y su entorno
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
