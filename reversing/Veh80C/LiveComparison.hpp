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
#include "General.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Veh80C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::FireFixedMachineGuns (0x73DF00, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado durante el banco, salvo rand):
//   operator*(CMatrix, CVector) 0x59C890 (cdecl, puntero de resultado oculto, devuelve ese puntero), CWeapon::DoTankDoomAiming 0x73D1E0 (cdecl, 4 argumentos), FireOneInstantHitRound 0x73AF00 (cdecl, 3 argumentos),
//   CAudioEngine::ReportWeaponEvent 0x506F40 (thiscall ret 0xC) y rand() 0x821B1E (sin hook; en la DLL CGeneral::GetRandomNumber).
// Los falsos registran sus argumentos EN ORDEN (punteros de pila por su contenido, punteros de arena por etiqueta) y pueden mutar el vehiculo (matriz +0x14, conductor +0x460, cargador +0x4AB, hora del arma +0x4D4) o el
// reloj para comprobar que la funcion relee esos campos tras cada llamada. El vehiculo es un CVehicle completo (0x5A0 bytes) en una arena. El EXE lee la matriz +0x10 sin comprobar nulo: la matriz nula solo se prueba sin disparo.

namespace V80Layout {
struct V80Veh : CVehicle {
    static constexpr size_t OffGun() { return offsetof(V80Veh, m_nGunFiringTime); }
    static constexpr size_t OffAmmo() { return offsetof(V80Veh, m_nAmmoInClip); }
    static constexpr size_t OffDriver() { return offsetof(V80Veh, m_pDriver); }
};
static_assert(V80Veh::OffGun() == 0x4D4 && V80Veh::OffAmmo() == 0x4AB && V80Veh::OffDriver() == 0x460, "CVehicle: hora del arma +0x4D4, cargador +0x4AB, conductor +0x460");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: matriz en +0x14");
static_assert(sizeof(CVehicle) == 0x5A0 && V80Fix::kVehWin == sizeof(CVehicle), "la arena del vehiculo es un CVehicle completo");
}

namespace V80Live {
using namespace V80Fix;
inline constexpr uintptr_t kEntry=0x73DF00;
inline constexpr uintptr_t kMul=0x59C890,kDoom=0x73D1E0,kFire=0x73AF00,kReport=0x506F40,kRand=0x821B1E;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    // Cambios respecto a la imagen de referencia, sin los campos de puntero que se comparan por etiqueta (mask = [[desplazamiento del objeto, bytes]])
    json Changes(std::initializer_list<std::pair<size_t,size_t>> mask={}) const {
        auto a=m,b=r;
        for(const auto& [off,len]:mask)for(size_t i=0;i<len;++i){a[kGuard+off+i]=0;b[kGuard+off+i]=0;}
        return ArenaChanges(a.data(),b.data(),N,kGuard);
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA,matB;
inline Arena<kPedArena> pedA,pedB;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["time"]=G32(kTimeAddr);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ FireFixedMachineGuns (0x73DF00)
struct State {const C* c{};unsigned ev{},mul{},rn{};};
inline State g;
inline u32 Ptr(u32 code) {
    switch(code) {
    case 0: return 0u;
    case 1: return u32(pedA.Obj());
    case 2: return u32(pedB.Obj());
    case 3: return u32(vehA.Obj());
    default: return kGarbageBase+0x10u*(code-4u);
    }
}
inline u32 GunTime() {return vehA.Get32(0x4D4);}
// Tras el evento numero mutAt, el falso muta el vehiculo o el reloj: 1 matriz -> matriz B, 2 conductor, 3 cargador, 4 hora del arma, 5 reloj global
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    switch(c.mutKind) {
    case 1: vehA.Put32(0x14,u32(matB.Obj()));break;
    case 2: vehA.Put32(0x460,Ptr(c.mutVal%7u));break;
    case 3: vehA.Put8(0x4AB,c.mutVal);break;
    case 4: vehA.Put32(0x4D4,c.mutVal);break;
    case 5: G32(kTimeAddr)=c.mutVal;break;
    default: break;
    }
}
// Eventos: 1 operator*(resultado, matriz, vector): [matriz, vx, vy, vz, hora]; 2 rand: [hora]; 3 DoTankDoomAiming: [vehiculo, propietario, inicio xyz, extremo xyz, hora];
// 4 FireOneInstantHitRound: [inicio xyz, extremo xyz, intensidad, hora]; 5 ReportWeaponEvent: [ecx, evento, arma, fisico, hora]
inline u32* __cdecl FakeMul(u32* out,const void* matrix,const u32* v) {
    Ev& e=PushEv(1,5);e.v[0]=Tag(matrix);e.v[1]=v[0];e.v[2]=v[1];e.v[3]=v[2];e.v[4]=GunTime();
    const u32* r=g.c->res[g.mul++&1u];
    out[0]=r[0];out[1]=r[1];out[2]=r[2];
    After();
    return out;
}
inline u32 __cdecl FakeRand() {
    Ev& e=PushEv(2,1);e.v[0]=GunTime();
    const u32 r=g.c->rnd[g.rn<6?g.rn:5];++g.rn;
    After();
    return r;
}
inline void __cdecl FakeDoom(void* vehicle,void* owner,u32* start,u32* end) {
    Ev& e=PushEv(3,9);e.v[0]=Tag(vehicle);e.v[1]=Tag(owner);for(int i=0;i<3;++i){e.v[2+i]=start[i];e.v[5+i]=end[i];}e.v[8]=GunTime();
    const C& c=*g.c;
    if(c.doomMode&2u)for(int i=0;i<3;++i)start[i]=c.dt[i];
    if(c.doomMode&1u)for(int i=0;i<3;++i)end[i]=c.dt[3+i];
    After();
}
inline void __cdecl FakeFire(const u32* start,const u32* end,int intensity) {
    Ev& e=PushEv(4,8);for(int i=0;i<3;++i){e.v[i]=start[i];e.v[3+i]=end[i];}e.v[6]=u32(intensity);e.v[7]=GunTime();
    After();
}
inline void __fastcall FakeReport(void* self,void*,int event,int weapon,void* physical) {
    Ev& e=PushEv(5,5);e.v[0]=u32(uintptr_t(self));e.v[1]=u32(event);e.v[2]=u32(weapon);e.v[3]=Tag(physical);e.v[4]=GunTime();
    After();
}

inline void SetGlobals(const C& c) {G32(kTimeAddr)=c.now;}

inline void CaseFire(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);matA.Fill(c.seed*13u+2u);matB.Fill(c.seed*13u+3u);pedA.Fill(c.seed*13u+4u);pedB.Fill(c.seed*13u+5u);
        vehA.Put32(0x14,c.hasMat?u32(matA.Obj()):0u);vehA.Put32(0x4D4,c.gun);vehA.Put8(0x4AB,c.ammo);vehA.Put32(0x460,Ptr(c.driver));
        matA.Put32(0x10,c.topX);matA.Put32(0x14,c.topY);matA.Put32(0x18,c.topZ);
        vehA.Snap();matA.Snap();matB.Snap();pedA.Snap();pedB.Snap();
        TagClear();vehA.Tagged();matA.Tagged();matB.Tagged();pedA.Tagged();pedB.Tagged();
        SetGlobals(c);
        g=State{};g.c=&c;nev=0;
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["gun"]=vehA.Get32(0x4D4);
        obs["ammo"]=vehA.m[kGuard+0x4AB];
        obs["matrix"]=Tag(uintptr_t(vehA.Get32(0x14)));
        obs["driver"]=Tag(uintptr_t(vehA.Get32(0x460)));
        obs["changes"]=json::array({vehA.Changes({{0x14,4},{0x460,4}}),matA.Changes(),matB.Changes(),pedA.Changes(),pedB.Changes()});
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
    if(!std::filesystem::exists(dir/"veh80c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"veh80c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("veh80c");}
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
    const uintptr_t randBody=Addr(&CGeneral::GetRandomNumber);
    ready&=randBody!=0&&ModuleOf(randBody)==env.asi;
    const Pair pMul=MakeCollab(env,kMul,hooksFound),pDoom=MakeCollab(env,kDoom,hooksFound),pFire=MakeCollab(env,kFire,hooksFound),pReport=MakeCollab(env,kReport,hooksFound);
    const Pair pRand{kRand,randBody,false};
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pMul,reinterpret_cast<void*>(&FakeMul)),S(pDoom,reinterpret_cast<void*>(&FakeDoom)),S(pFire,reinterpret_cast<void*>(&FakeFire)),S(pReport,reinterpret_cast<void*>(&FakeReport)),
        S(pRand,reinterpret_cast<void*>(&FakeRand))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTimer::GetTimeInMS (lectura del reloj)","aritmetica y ayudantes x87 de X87Math.cpp (Veh80C*)"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
