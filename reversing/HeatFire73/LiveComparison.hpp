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
#include "Entity/Ped/Ped.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeatFire73 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::FireHeatSeakingMissile (0x6E05C0, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado durante el banco):
//   CVehicle::GetPlaneOrdnanceRateOfFire 0x6D4590 (thiscall ret 4), CVehicle::GetPlaneOrdnancePosition 0x6D46E0 (thiscall ret 8, resultado por puntero), CProjectileInfo::AddProjectile 0x737C80 (cdecl) y CPad::StartShake 0x53F920 (thiscall ret 0Ch).
// CPed::IsPlayer (0x5DF8F0) y CPad::GetPad (0x53FB70) se ejecutan de verdad en ambos lados. El vehiculo es un CPlane completo (0xA04 bytes) en una arena.
// El EXE lee matriz + 0x10 (direccion) y la velocidad sin comprobar nulo y se caeria con la matriz nula: solo se prueba sin matriz cuando el intervalo del arma corta el disparo antes de leerla.

namespace Hf73Layout {
struct HfVeh : CVehicle {
    static constexpr size_t OffT1() { return offsetof(HfVeh, m_nProjectileWeaponFiringTime); }
    static constexpr size_t OffT2() { return offsetof(HfVeh, m_nAdditionalProjectileWeaponFiringTime); }
    static constexpr size_t OffDriver() { return offsetof(HfVeh, m_pDriver); }
    static constexpr size_t OffSpeed() { return offsetof(HfVeh, m_vecMoveSpeed); }
};
static_assert(HfVeh::OffT1() == 0x4FC && HfVeh::OffT2() == 0x500, "CVehicle: tiempos de disparo en +0x4FC y +0x500");
static_assert(HfVeh::OffDriver() == 0x460 && HfVeh::OffSpeed() == 0x44, "CVehicle: conductor en +0x460 y velocidad en +0x44");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CPlane) == 0xA04 && Hf73Fix::kVehWin == sizeof(CPlane), "la arena del vehiculo es un CPlane completo");
static_assert(std::is_base_of_v<CVehicle, CPlane>, "CPlane es un CVehicle");
struct HfPed : CPed {
    static constexpr size_t OffType() { return offsetof(HfPed, m_nPedType); }
};
static_assert(HfPed::OffType() == 0x598 && Hf73Fix::kPedWin >= HfPed::OffType() + sizeof(ePedType), "CPed::m_nPedType en +0x598 dentro de la ventana del ped");
// m_nOrdnanceCycleIndex (bits 2-3 del byte +0x4A9): se comprueba en ejecucion (no hay offsetof de un campo de bits)
inline bool CycleLayoutOK() {
    alignas(16) static std::array<uint8, sizeof(CPlane) + 16> mem{};
    mem.fill(0);
    auto* veh = reinterpret_cast<CVehicle*>(mem.data());
    veh->m_nOrdnanceCycleIndex = 3;
    bool ok = mem[0x4A9] == 0x0C;
    mem.fill(0);
    veh->m_nOrdnanceCycleIndex = 1;
    ok &= mem[0x4A9] == 0x04;
    mem.fill(0xFF);
    veh->m_nOrdnanceCycleIndex = 0;
    ok &= mem[0x4A9] == 0xF3;
    return ok;
}
}

namespace Hf73Live {
using namespace Hf73Fix;
inline constexpr uintptr_t kEntry=0x6E05C0;
inline constexpr uintptr_t kRate=0x6D4590,kOrd=0x6D46E0,kProj=0x737C80,kShake=0x53F920;
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
inline Arena<kMatArena> matV;
inline Arena<kPedArena> pedA;

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

// ================================================================================================ FireHeatSeakingMissile (0x6E05C0)
struct State {const C* c{};};
inline State g;
// Eventos: 1 GetPlaneOrdnanceRateOfFire(this, tipo); 2 GetPlaneOrdnancePosition(this, tipo) (el puntero de salida es de pila y no se registra); 3 AddProjectile(creador, arma, x, y, z, fuerza, direccion, victima, direccion[0..2] en ese momento);
// 4 StartShake(indice de pad, tiempo, frecuencia, arg). Todos los punteros dentro de una arena se etiquetan.
inline u32 __fastcall FakeRate(void* self,void*,u32 type) {Ev& e=PushEv(1,2);e.v[0]=Tag(self);e.v[1]=type;return g.c->rate;}
inline void* __fastcall FakeOrd(void* self,void*,u32* out,u32 type) {Ev& e=PushEv(2,2);e.v[0]=Tag(self);e.v[1]=type;out[0]=g.c->ord[0];out[1]=g.c->ord[1];out[2]=g.c->ord[2];return out;}
inline u32 __cdecl FakeProj(void* creator,u32 weapon,u32 x,u32 y,u32 z,u32 force,const u32* dir,void* victim) {
    Ev& e=PushEv(3,11);e.v[0]=Tag(creator);e.v[1]=weapon;e.v[2]=x;e.v[3]=y;e.v[4]=z;e.v[5]=force;e.v[6]=(Tag(dir)==u32(uintptr_t(dir)))?0xF00D0001u:Tag(dir);e.v[7]=Tag(victim);   // el EXE pasa un local de pila (direccion variable): se registra como «fuera de las arenas»
    e.v[8]=dir?dir[0]:0u;e.v[9]=dir?dir[1]:0u;e.v[10]=dir?dir[2]:0u;
    return g.c->projRet;
}
inline void __fastcall FakeShake(void* self,void*,u32 time,u32 freq,u32 arg) {Ev& e=PushEv(4,4);e.v[0]=u32(uintptr_t(self))-u32(kPads0);e.v[1]=time;e.v[2]=freq;e.v[3]=arg;}

inline void CaseFire(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);tgtA.Fill(c.seed*13u+2u);othA.Fill(c.seed*13u+3u);matV.Fill(c.seed*13u+4u);pedA.Fill(c.seed*13u+5u);
        // vehiculo: con matriz (direccion +0x10, posicion +0x30) o sin ella (solo si el intervalo corta el disparo); velocidad +0x44; conductor +0x460
        vehA.Put32(0x14,c.hasMat?u32(matV.Obj()):0u);
        vehA.Put32(0x4FC,c.t1);vehA.Put32(0x500,c.t2);vehA.Put8(0x4A9,c.cycle);
        vehA.Put32(0x460,c.driver?u32(pedA.Obj()):0u);pedA.Put32(0x598,c.pedType);
        for(int i=0;i<3;++i){vehA.Put32(0x44+4*i,c.ms[i]);vehA.Put32(4+4*i,c.ppos[i]);matV.Put32(0x30+4*i,c.mpos[i]);matV.Put32(0x10+4*i,c.fwd[i]);}
        vehA.Snap();tgtA.Snap();othA.Snap();matV.Snap();pedA.Snap();
        TagClear();vehA.Tagged();tgtA.Tagged();othA.Tagged();matV.Tagged();pedA.Tagged();
        G32(kTimeAddr)=c.time;G32(kStepAddr)=c.step;
        g.c=&c;nev=0;
        const u32 tgtArg=c.tgtKind==0?0u:c.tgtKind==1?u32(tgtA.Obj()):c.tgtKind==2?u32(othA.Obj()):c.entRaw;
        const u32 args[3]{tgtArg,c.type,c.arg2};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["cycle"]=u32(reinterpret_cast<const CVehicle*>(vehA.Obj())->m_nOrdnanceCycleIndex);   // leido por el miembro real
        obs["changes"]=json::array({vehA.Changes(),tgtA.Changes(),othA.Changes(),matV.Changes(),pedA.Changes()});
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
    const bool layout=Hf73Layout::CycleLayoutOK();
    ready&=layout;
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},{"cycle_layout_ok",layout},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseFire(env,totals,c);
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
    if(!std::filesystem::exists(dir/"heatfire73-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"heatfire73-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("heatfire73");}
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
    const Pair pRate=MakeCollab(env,kRate,hooksFound),pOrd=MakeCollab(env,kOrd,hooksFound),pProj=MakeCollab(env,kProj,hooksFound),pShake=MakeCollab(env,kShake,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pRate,reinterpret_cast<void*>(&FakeRate)),S(pOrd,reinterpret_cast<void*>(&FakeOrd)),S(pProj,reinterpret_cast<void*>(&FakeProj)),S(pShake,reinterpret_cast<void*>(&FakeShake))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CPed::IsPlayer 0x5DF8F0","CPad::GetPad 0x53FB70"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
