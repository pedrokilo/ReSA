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
#include "Entity/Object/Object.h"
#include "ObjectData.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "Fx/Fx.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FightHitObj72 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CTaskSimpleFight::FightHitObj (0x61D400, hook S). Siete colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y en el
// cuerpo de la DLL): CTaskSimpleFight::FightStrikeDamage 0x61C740, CPhysical::AddToMovingList 0x542800, CPhysical::ApplyForce 0x542B50, CObject::ObjectDamage 0x5A0D90,
// CAEPedWeaponAudioEntity::AddAudioEvent 0x4E69F0 y CAEPedAudioEntity::AddAudioEvent 0x4E2BB0 (sin hook: --unhook=Audio) y Fx_c::AddPunchImpact 0x49F670 (con hook). CEntity::SetIsStatic es una llamada virtual
// por la tabla del objeto (vtable falsa del banco).

namespace Fo72Layout {
struct FoPed : CPed {
    static constexpr size_t OffPedAE() { return offsetof(FoPed, m_pedAudio); }
    static constexpr size_t OffWeaponAE() { return offsetof(FoPed, m_weaponAudio); }
};
static_assert(FoPed::OffPedAE() == 0x138 && FoPed::OffWeaponAE() == 0x394, "CPed: m_pedAudio en +0x138 y m_weaponAudio en +0x394");
static_assert(offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_aWeapons) == 0x5A0 && sizeof(CWeapon) == 0x1C, "CPed: ranura activa en +0x718, armas en +0x5A0 de 0x1C bytes");
static_assert(offsetof(CObject, m_nFlags) == 0x1C && offsetof(CObject, m_nPhysicalFlags) == 0x40 && offsetof(CObject, m_nColDamageEffect) == 0x144 && offsetof(CObject, m_pObjectInfo) == 0x160, "CObject");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CObjectData, m_fUprootLimit) == 0x14 && offsetof(CObjectData, m_fColDamageMultiplier) == 0x18, "CObjectData");
static_assert(offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nCurrentMove) == 0x25, "CTaskSimpleFight");
static_assert(sizeof(CObject) <= Fo72Fix::kObjWin && sizeof(CTaskSimpleFight) <= Fo72Fix::kTaskWin, "ventanas de las arenas");
}

namespace Fo72Live {
using namespace Fo72Fix;

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
inline Arena<kPedArena> pedA;
inline Arena<kObjArena> objA;
inline Arena<kInfoArena> infoA;
inline Arena<kMatArena> matA;
inline Arena<kVecArena> p1A,n2A;

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

// ================================================================================================ FightHitObj (0x61D400)
struct State {const C* c{};};
inline State g;
// Puntero de argumento: dentro de una arena se etiqueta; si no es un local de la pila del llamador (cambia de una pasada a otra).
inline u32 TagP(const void* p) {
    const uintptr_t v=uintptr_t(p);
    for(const auto& e:tagTable)if(v>=e.base&&v<e.base+e.size)return Tag(v);
    return p?0xDEADC0DEu:0u;
}
inline void PutVec(Ev& e,u32 at,const void* v) {const u32* w=static_cast<const u32*>(v);e.v[at]=w[0];e.v[at+1]=w[1];e.v[at+2]=w[2];}
inline void WritePed(ptrdiff_t off,u32 v) {pedA.Put32(off,v);}
// Tabla virtual falsa del objeto: ranura 4 = CEntity::SetIsStatic (vtable +0x10, `ret 4`); las demas son trampas (no deben llamarse)
inline void __fastcall FakeSetStatic(void* self,void*,u32 arg) {Ev& e=PushEv(2,2);e.v[0]=Tag(self);e.v[1]=arg&0xFFu;if(g.c->mutFlags==1)objA.Put32(0x1C,g.c->flagsAfter);}
inline void __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);}
inline std::array<void*,16> fakeVt{};
// Todos __fastcall (ECX = this, EDX sin uso, pila) con la limpieza `ret n` del original. Los floats viajan como dword en la pila.
inline float __fastcall FakeDamage(void* self,void*,void* ped) {                                                                            // 0x61C740, `ret 4`, devuelve ST0
    Ev& e=PushEv(1,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);
    if(g.c->mutNormal)for(int i=0;i<3;++i)n2A.Put32(4*i,g.c->mutN[i]);
    if(g.c->mutPosn1)for(int i=0;i<3;++i)p1A.Put32(4*i,g.c->mutP[i]);
    return F(g.c->damage);
}
inline void __fastcall FakeMoving(void* self,void*) {Ev& e=PushEv(3,1);e.v[0]=Tag(self);if(g.c->mutFlags==2)objA.Put32(0x1C,g.c->flagsAfter);}                        // 0x542800
inline void __fastcall FakeApply(void* self,void*,u32 fx,u32 fy,u32 fz,u32 px,u32 py,u32 pz,u32 flag) {                                         // 0x542B50, `ret 1Ch`
    Ev& e=PushEv(4,8);e.v[0]=Tag(self);e.v[1]=fx;e.v[2]=fy;e.v[3]=fz;e.v[4]=px;e.v[5]=py;e.v[6]=pz;e.v[7]=flag&0xFFu;
}
inline void __fastcall FakeObjDamage(void* self,void*,u32 damage,void* origin,void* dir,void* damager,u32 weapon) {                              // 0x5A0D90, `ret 14h`
    Ev& e=PushEv(5,12);e.v[0]=Tag(self);e.v[1]=damage;e.v[2]=TagP(origin);PutVec(e,3,origin);e.v[6]=TagP(dir);PutVec(e,7,dir);e.v[10]=Tag(damager);e.v[11]=weapon;
    if(g.c->mutWeapon) {   // el arma activa cambia aqui: el EXE relee el tipo de arma despues
        const int slot=int(int8(pedA.m[kGuard+0x718]));
        const ptrdiff_t at=0x5A0+slot*0x1C;
        const u32 cur=pedA.Get32(at);
        pedA.Put32(at,g.c->mutWeapon==1?(cur==kChainsaw?7u:u32(kChainsaw)):u32(kChainsaw));
    }
}
inline void __fastcall FakeWeaponAE(void* self,void*,u32 event) {Ev& e=PushEv(6,2);e.v[0]=Tag(self);e.v[1]=event;}                                // 0x4E69F0, `ret 4`
inline void __fastcall FakePedAE(void* self,void*,u32 event,u32 volume,u32 speed,void* entity,u32 surface,u32 a7,u32 maxVol) {                   // 0x4E2BB0, `ret 1Ch`
    Ev& e=PushEv(7,8);e.v[0]=Tag(self);e.v[1]=event;e.v[2]=volume;e.v[3]=speed;e.v[4]=Tag(entity);e.v[5]=surface;e.v[6]=a7;e.v[7]=maxVol;
}
inline void __fastcall FakePunch(void* self,void*,void* pos,void* vel,u32 type) {                                                              // 0x49F670, `ret 0Ch`
    Ev& e=PushEv(8,10);e.v[0]=u32(uintptr_t(self));e.v[1]=TagP(pos);PutVec(e,2,pos);e.v[5]=TagP(vel);PutVec(e,6,vel);e.v[9]=type;
}

inline void CaseHit(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*13u+1u);pedA.Fill(c.seed*13u+2u);objA.Fill(c.seed*13u+3u);infoA.Fill(c.seed*13u+4u);matA.Fill(c.seed*13u+5u);p1A.Fill(c.seed*13u+6u);n2A.Fill(c.seed*13u+7u);
        // tarea: combo y movimiento (bytes con signo)
        taskA.Put8(0x24,u8(c.combo));taskA.Put8(0x25,u8(c.move));
        // ped: ranura activa (byte +0x718) y tipo de arma de la ranura (+0x5A0 + ranura * 0x1C)
        pedA.Put8(0x718,u8(c.slot));
        pedA.Put32(0x5A0+ptrdiff_t(int(int8(c.slot)))*0x1C,c.weaponType);
        // objeto: vtable falsa, banderas, banderas fisicas, efecto de danio, informacion y posicion (matriz o posicion compacta)
        objA.Put32(0,u32(reinterpret_cast<uintptr_t>(fakeVt.data())));
        objA.Put32(0x1C,c.flags);objA.Put32(0x40,c.physFlags);objA.Put8(0x144,u8(c.effect));objA.Put32(0x160,u32(infoA.Obj()));
        objA.Put32(0x14,c.objMat?u32(matA.Obj()):0u);
        for(int i=0;i<3;++i) {
            if(c.objMat)matA.Put32(0x30+4*i,c.pos[i]);else objA.Put32(4+4*i,c.pos[i]);
            p1A.Put32(4*i,c.p1[i]);n2A.Put32(4*i,c.n2[i]);
        }
        infoA.Put32(0x14,c.uproot);infoA.Put32(0x18,c.mult);
        taskA.Snap();pedA.Snap();objA.Snap();infoA.Snap();matA.Snap();p1A.Snap();n2A.Snap();
        TagClear();taskA.Tagged();pedA.Tagged();objA.Tagged();infoA.Tagged();matA.Tagged();p1A.Tagged();n2A.Tagged();
        g.c=&c;nev=0;
        const u32 args[6]{u32(pedA.Obj()),u32(objA.Obj()),u32(p1A.Obj()),u32(n2A.Obj()),c.component,c.surface};
        sides[s]=Execute(env,0,0x61D400,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({taskA.Changes(),pedA.Changes(),objA.Changes(),infoA.Changes(),matA.Changes(),p1A.Changes(),n2A.Changes()});
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
    if(!std::filesystem::exists(dir/"fighthitobj72-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fighthitobj72-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fighthitobj72");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t damageBody=DllBodyOf("CTaskSimpleFight","FightStrikeDamage");
    const uintptr_t movingBody=DllBodyOf("CPhysical","AddToMovingList");
    const uintptr_t applyBody=DllBodyOf("CPhysical","ApplyForce");
    const uintptr_t objDamageBody=DllBodyOf("CObject","ObjectDamage");
    // Las dos AddAudioEvent de audio (categoria Audio/Entities) quedan SIN hook en todos los bancos (runtime_runner pasa --unhook=Audio): su cuerpo en la DLL salta al EXE, asi que el falso
    // de la direccion del EXE sirve a ambos lados (la DLL llama a la direccion del EXE).
    const uintptr_t punchBody=DllBodyOf("Fx_c","AddPunchImpact");
    env.route[0].reset(new HookRoute("CTaskSimpleFight","FightHitObj",0x61D400,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=damageBody&&movingBody&&applyBody&&objDamageBody&&punchBody;
    env.stubs[0]={{Pair{0x61C740,damageBody,true},reinterpret_cast<void*>(&FakeDamage)},
                  {Pair{0x542800,movingBody,true},reinterpret_cast<void*>(&FakeMoving)},
                  {Pair{0x542B50,applyBody,true},reinterpret_cast<void*>(&FakeApply)},
                  {Pair{0x5A0D90,objDamageBody,true},reinterpret_cast<void*>(&FakeObjDamage)},
                  {Pair{0x4E69F0,0,false},reinterpret_cast<void*>(&FakeWeaponAE)},
                  {Pair{0x4E2BB0,0,false},reinterpret_cast<void*>(&FakePedAE)},
                  {Pair{0x49F670,punchBody,true},reinterpret_cast<void*>(&FakePunch)}};
    for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap);
    fakeVt[4]=reinterpret_cast<void*>(&FakeSetStatic);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"virtual_fake","CEntity::SetIsStatic (vtable +0x10 del objeto falso)"},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x61D400,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
