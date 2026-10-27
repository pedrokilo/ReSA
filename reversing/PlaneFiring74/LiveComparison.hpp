#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Plane.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlaneFiring74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::GetPlaneWeaponFiringStatus (0x6E3440, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado durante el banco):
//   CWorld::FindPlayerSlotWithVehiclePointer 0x564000 (cdecl), CPad::GetCarGunFired 0x53FF90 y CPad::CarGunJustDown 0x53FFE0 (thiscall, AX), CPad::GetEnterTargeting 0x5406B0 y CPad::GetTarget 0x540670 (thiscall, AL),
//   CWeaponEffects::IsLockedOn 0x742BD0 (cdecl, AL), CVehicle::ScanAndMarkTargetForHeatSeekingMissile 0x6E0400 (thiscall ret 4) y CProjectileInfo::AddProjectile 0x737C80 (cdecl).
// CPad::GetPad (0x53FB70, inline sin hook) se ejecuta de verdad en ambos lados. El vehiculo es un CPlane completo (0xA04 bytes) en una arena; las salidas (bool de un byte y tipo de ordenanza de un dword) viven en una arena propia
// con vecinos conocidos: la funcion debe escribir exactamente un byte en el bool (ProcessWeapons lee ese dword entero y reenvia sus bytes altos) y un dword en el tipo, y solo en los caminos del EXE.
// El EXE lee matriz + 0x10/+0x20 sin comprobar nulo en la bengala del Hydra y se caeria con la matriz nula: solo se prueba sin matriz cuando no hay bengala.

namespace Pf74Layout {
struct PfVeh : CVehicle {
    static constexpr size_t OffGun() { return offsetof(PfVeh, m_nGunFiringTime); }
    static constexpr size_t OffModel() { return offsetof(PfVeh, m_nModelIndex); }
};
static_assert(PfVeh::OffGun() == 0x4D4 && PfVeh::OffModel() == 0x22, "CVehicle: ultima bengala en +0x4D4 y modelo en +0x22");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CPlane) == 0xA04 && Pf74Fix::kVehWin == sizeof(CPlane), "la arena del vehiculo es un CPlane completo");
static_assert(offsetof(CPlane, field_9DC) == 0x9DC && offsetof(CPlane, field_9E0) == 0x9E0, "CPlane: inicio y objetivo de la fijacion en +0x9DC y +0x9E0");
static_assert(sizeof(CPad) == 0x134, "CPad mide 0x134 bytes (la ranura del pad es Pads + slot * 0x134)");
}

namespace Pf74Live {
using namespace Pf74Fix;
inline constexpr uintptr_t kEntry=0x6E3440;
inline constexpr uintptr_t kSlot=0x564000,kFired=0x53FF90,kJust=0x53FFE0,kEnter=0x5406B0,kTarget=0x540670,kLocked=0x742BD0,kScan=0x6E0400,kProj=0x737C80;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uint8& G8(uintptr_t a) {return *reinterpret_cast<uint8*>(a);}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const u16 w=u16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kTgtArena> tgtA;
inline Arena<kOthArena> othA;
inline Arena<kMatArena> matV;
inline Arena<kOutArena> outA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["cross"]=HexBytes(reinterpret_cast<void*>(kCross),kCrossBytes);
    obs["flags"]=json::array({G8(kFlagA),G8(kFlagB)});
    obs["time"]=G32(kTimeAddr);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ GetPlaneWeaponFiringStatus (0x6E3440)
struct State {const C* c{};unsigned gf{},jd{},scan{};};
inline State g;
inline u32 Ptr(u32 code) {
    switch(code) {
    case 0: return 0u;
    case 1: return u32(tgtA.Obj());
    case 2: return u32(othA.Obj());
    case 3: return u32(vehA.Obj());
    default: return kGarbageBase+0x10u*(code-4u);
    }
}
// Eventos: 1 FindPlayerSlotWithVehiclePointer(vehiculo); 2 GetCarGunFired(pad); 3 CarGunJustDown(pad); 4 GetEnterTargeting(pad); 5 GetTarget(pad); 6 IsLockedOn(id);
// 7 ScanAndMarkTargetForHeatSeekingMissile(this, entidad); 8 AddProjectile(creador, arma, x, y, z, fuerza, direccion, victima, direccion[0..2] en ese momento). Los pads se registran como desplazamiento respecto a Pads[0].
inline u32 __cdecl FakeSlot(void* veh) {Ev& e=PushEv(1,1);e.v[0]=Tag(veh);return g.c->slot;}
inline u32 __fastcall FakeFired(void* pad,void*) {Ev& e=PushEv(2,1);e.v[0]=u32(uintptr_t(pad))-kPads0;return g.c->gf[g.gf++?1:0];}
inline u32 __fastcall FakeJust(void* pad,void*) {Ev& e=PushEv(3,1);e.v[0]=u32(uintptr_t(pad))-kPads0;return g.c->jd[g.jd++?1:0];}
inline u32 __fastcall FakeEnter(void* pad,void*) {Ev& e=PushEv(4,1);e.v[0]=u32(uintptr_t(pad))-kPads0;return g.c->et;}
inline u32 __fastcall FakeTarget(void* pad,void*) {Ev& e=PushEv(5,1);e.v[0]=u32(uintptr_t(pad))-kPads0;return g.c->tg;}
inline u32 __cdecl FakeLocked(u32 id) {Ev& e=PushEv(6,1);e.v[0]=id;return g.c->locked;}
inline u32 __fastcall FakeScan(void* self,void*,void* entity) {Ev& e=PushEv(7,2);e.v[0]=Tag(self);e.v[1]=Tag(entity);return Ptr(g.c->scan[g.scan++?1:0]);}
inline u32 __cdecl FakeProj(void* creator,u32 weapon,u32 x,u32 y,u32 z,u32 force,const u32* dir,void* victim) {
    Ev& e=PushEv(8,11);e.v[0]=Tag(creator);e.v[1]=weapon;e.v[2]=x;e.v[3]=y;e.v[4]=z;e.v[5]=force;e.v[6]=Tag(dir);e.v[7]=Tag(victim);
    const bool inArena=Tag(dir)!=u32(uintptr_t(dir));   // si no apunta a una arena (basura o pila) no se lee
    e.v[8]=inArena?dir[0]:0u;e.v[9]=inArena?dir[1]:0u;e.v[10]=inArena?dir[2]:0u;
    return g.c->projRet;
}

inline void SetGlobals(const C& c) {
    G32(kTimeAddr)=c.now;G8(kFlagA)=uint8(c.flagA);G8(kFlagB)=uint8(c.flagB);
    memset(reinterpret_cast<void*>(kCross),kCrossFill,kCrossBytes);
}

inline void CaseStatus(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);tgtA.Fill(c.seed*13u+2u);othA.Fill(c.seed*13u+3u);matV.Fill(c.seed*13u+4u);outA.Fill(c.seed*13u+5u);
        vehA.Put16(0x22,c.model);vehA.Put32(0x14,c.hasMat?u32(matV.Obj()):0u);
        vehA.Put32(0x4D4,c.gun);vehA.Put32(0x9DC,c.lockStart);vehA.Put32(0x9E0,Ptr(c.lockTarget));
        for(int i=0;i<3;++i){vehA.Put32(4+4*i,c.ppos[i]);matV.Put32(0x10+4*i,c.fwd[i]);matV.Put32(0x20+4*i,c.up[i]);matV.Put32(0x30+4*i,c.mpos[i]);}
        outA.Put8(kOutStatus,c.stInit);outA.Put32(kOutOrdnance,c.ordInit);
        vehA.Snap();tgtA.Snap();othA.Snap();matV.Snap();outA.Snap();
        TagClear();vehA.Tagged();tgtA.Tagged();othA.Tagged();matV.Tagged();outA.Tagged();
        SetGlobals(c);
        g=State{};g.c=&c;nev=0;
        const u32 args[2]{u32(outA.Obj(kGuard+kOutStatus)),u32(outA.Obj(kGuard+kOutOrdnance))};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["ret"]=Tag(uintptr_t(sides[s].reg.eax));
        obs["lock_target"]=Tag(uintptr_t(vehA.Get32(0x9E0)));
        obs["changes"]=json::array({vehA.Changes(),tgtA.Changes(),othA.Changes(),matV.Changes(),outA.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseStatus(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe,std::string& cat,std::string& name) {
    bool found=false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for(auto& h:c.Items())
            if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
            }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env,uintptr_t exe,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
        if(env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
    }
    return p;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"planefiring74-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"planefiring74-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("planefiring74");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat,name;
        const bool found=FindHook(kEntry,cat,name);
        if(found)env.route[0].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry,HookRoute::RegisteredTarget{}));
        registered[kFn[0]]=found&&env.route[0]->Registered();bodies[kFn[0]]=found&&env.route[0]->Body()!=0;
        ready&=found&&env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[0]]=found?json({{"class",cat},{"name",name},{"address",kEntry}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    const Pair pSlot=MakeCollab(env,kSlot,hooksFound),pFired=MakeCollab(env,kFired,hooksFound),pJust=MakeCollab(env,kJust,hooksFound),pEnter=MakeCollab(env,kEnter,hooksFound),
        pTarget=MakeCollab(env,kTarget,hooksFound),pLocked=MakeCollab(env,kLocked,hooksFound),pScan=MakeCollab(env,kScan,hooksFound),pProj=MakeCollab(env,kProj,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pSlot,reinterpret_cast<void*>(&FakeSlot)),S(pFired,reinterpret_cast<void*>(&FakeFired)),S(pJust,reinterpret_cast<void*>(&FakeJust)),S(pEnter,reinterpret_cast<void*>(&FakeEnter)),
        S(pTarget,reinterpret_cast<void*>(&FakeTarget)),S(pLocked,reinterpret_cast<void*>(&FakeLocked)),S(pScan,reinterpret_cast<void*>(&FakeScan)),S(pProj,reinterpret_cast<void*>(&FakeProj))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CPad::GetPad 0x53FB70"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
