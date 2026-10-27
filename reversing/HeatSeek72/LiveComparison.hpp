#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Plane.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeatSeek72 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::ScanAndMarkTargetForHeatSeekingMissile (0x6E0400, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL:
//   CWeapon::PickTargetForHeatSeekingMissile 0x73F910, CWorld::GetIsLineOfSightClear 0x56A490, CWeaponEffects::MarkTarget 0x742BF0, CWeaponEffects::ClearCrossHairImmediately 0x742CA0 (todos cdecl).
// El vehiculo siempre tiene matriz (el EXE lee matriz + 0x10 sin comprobar nulo y se caeria): la rama de posicion compacta solo se ejercita en el objetivo.

namespace Hs72Layout {
struct HsVeh : CVehicle {
    static constexpr size_t OffSub() { return offsetof(HsVeh, m_nVehicleSubType); }
};
static_assert(HsVeh::OffSub() == 0x594, "CVehicle::m_nVehicleSubType en +0x594");
static_assert(offsetof(CPlane, field_9E4) == 0x9E4, "CPlane::field_9E4 en +0x9E4");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CPlane, field_9E4) + 4 <= Hs72Fix::kVehWin, "ventana del vehiculo");
}

namespace Hs72Live {
using namespace Hs72Fix;
inline constexpr uintptr_t kEntry=0x6E0400;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kTgtArena> tgtA;
inline Arena<kOthArena> othA;
inline Arena<kMatArena> matV,matT;

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

// ================================================================================================ ScanAndMarkTargetForHeatSeekingMissile (0x6E0400)
struct State {const C* c{};};
inline State g;
inline u32 TargetPtr() {return g.c->tgtKind==0?0u:g.c->tgtKind==1?u32(tgtA.Obj()):u32(vehA.Obj());}
// Todos cdecl: los argumentos (CVector por valor = tres dwords) viajan como dwords en la pila.
// 1: PickTargetForHeatSeekingMissile 0x73F910 (origen, direccion, multiplicador, ignorar, prioridad de aviones, objetivo previo)
inline u32 __cdecl FakePick(u32 ox,u32 oy,u32 oz,u32 dx,u32 dy,u32 dz,u32 mult,u32 ignore,u32 prio,u32 prev) {
    Ev& e=PushEv(1,10);e.v[0]=ox;e.v[1]=oy;e.v[2]=oz;e.v[3]=dx;e.v[4]=dy;e.v[5]=dz;e.v[6]=mult;e.v[7]=Tag(uintptr_t(ignore));e.v[8]=prio;e.v[9]=Tag(uintptr_t(prev));
    return TargetPtr();
}
// 2: GetIsLineOfSightClear 0x56A490 (origen*, destino*, 7 bool). Registra los valores apuntados y los dwords de banderas (+0x1C) de ambos en ese momento; despues los altera (mutSelf / mutTgt).
inline u32 __cdecl FakeLos(u32 from,u32 to,u32 a2,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8) {
    Ev& e=PushEv(2,17);e.v[0]=Tag(uintptr_t(from));e.v[1]=Tag(uintptr_t(to));
    const u32* pf=reinterpret_cast<const u32*>(from);const u32* pt=reinterpret_cast<const u32*>(to);
    for(int i=0;i<3;++i){e.v[2+i]=pf[i];e.v[5+i]=pt[i];}
    e.v[8]=a2;e.v[9]=a3;e.v[10]=a4;e.v[11]=a5;e.v[12]=a6;e.v[13]=a7;e.v[14]=a8;
    e.v[15]=vehA.Get32(0x1C);e.v[16]=g.c->tgtKind==1?tgtA.Get32(0x1C):0u;
    vehA.Put32(0x1C,vehA.Get32(0x1C)^g.c->mutSelf);
    if(g.c->tgtKind==1)tgtA.Put32(0x1C,tgtA.Get32(0x1C)^g.c->mutTgt);
    else if(g.c->tgtKind==2)vehA.Put32(0x1C,vehA.Get32(0x1C)^g.c->mutTgt);
    return (g.c->los&1u)|(g.c->losHi<<8);
}
// 3: ClearCrossHairImmediately 0x742CA0 (id)
inline void __cdecl FakeClear(u32 id) {Ev& e=PushEv(3,1);e.v[0]=id;}
// 4: MarkTarget 0x742BF0 (id, posicion, r, g, b, a, tamano, limpiar)
inline void __cdecl FakeMark(u32 id,u32 px,u32 py,u32 pz,u32 r,u32 gr,u32 b,u32 a,u32 size,u32 clear) {
    Ev& e=PushEv(4,10);e.v[0]=id;e.v[1]=px;e.v[2]=py;e.v[3]=pz;e.v[4]=r;e.v[5]=gr;e.v[6]=b;e.v[7]=a;e.v[8]=size;e.v[9]=clear;
}

inline void CaseScan(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);tgtA.Fill(c.seed*13u+2u);othA.Fill(c.seed*13u+3u);matV.Fill(c.seed*13u+4u);matT.Fill(c.seed*13u+5u);
        // vehiculo: siempre con matriz (posicion +0x30, direccion +0x10)
        vehA.Put32(0x14,u32(matV.Obj()));vehA.Put32(0x1C,c.vflags);vehA.Put8(0x36,c.status);vehA.Put32(0x594,c.subType);vehA.Put32(0x9E4,c.pstate);
        for(int i=0;i<3;++i){matV.Put32(0x30+4*i,c.vpos[i]);matV.Put32(0x10+4*i,c.vdir[i]);}
        // objetivo: con matriz (+0x30) o con posicion compacta (+4)
        tgtA.Put32(0x14,c.tgtMat?u32(matT.Obj()):0u);tgtA.Put32(0x1C,c.tflags);
        for(int i=0;i<3;++i){if(c.tgtMat)matT.Put32(0x30+4*i,c.tpos[i]);else tgtA.Put32(4+4*i,c.tpos[i]);}
        vehA.Snap();tgtA.Snap();othA.Snap();matV.Snap();matT.Snap();
        TagClear();vehA.Tagged();tgtA.Tagged();othA.Tagged();matV.Tagged();matT.Tagged();
        G32(kTimeAddr)=c.time;
        g.c=&c;nev=0;
        const u32 entArg=c.entKind==0?0u:c.entKind==1?u32(tgtA.Obj()):c.entKind==2?u32(othA.Obj()):c.entRaw;
        const u32 args[1]{entArg};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["eax"]=Tag(uintptr_t(sides[s].reg.eax));
        obs["changes"]=json::array({vehA.Changes(),tgtA.Changes(),othA.Changes(),matV.Changes(),matT.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseScan(env,totals,c);
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
    if(!std::filesystem::exists(dir/"heatseek72-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"heatseek72-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("heatseek72");}
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
    const Pair pPick=MakeCollab(env,0x73F910,hooksFound),pLos=MakeCollab(env,0x56A490,hooksFound),pMark=MakeCollab(env,0x742BF0,hooksFound),pClear=MakeCollab(env,0x742CA0,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pPick,reinterpret_cast<void*>(&FakePick)),S(pLos,reinterpret_cast<void*>(&FakeLos)),S(pMark,reinterpret_cast<void*>(&FakeMark)),S(pClear,reinterpret_cast<void*>(&FakeClear))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
