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

// Banco par a par Veh81A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::UpdateTrailerLink (0x6DFC50, hook S). Colaboradores sustituidos por falsos identicos en el EXE y en el cuerpo de la DLL:
//   CPhysical::GetSpeed 0x542CE0 (thiscall, puntero de resultado oculto + vector por valor, ret 0x10) y CPhysical::ApplyForce 0x542B50 (thiscall, dos vectores por valor y un bool, ret 0x1C), ambos con hook RH (apagado);
//   los virtuales GetTowHitchPos (+0xEC), GetTowBarPos (+0xF0) y BreakTowLink (+0xF8) son entradas de una vtabla falsa compartida por el remolque (this) y los dos tractores (A y B) (el EXE y la DLL llaman por la vtabla).
// La aritmetica (CVector::operator-=, DotProduct, CrossProduct, Normalise, Multiply3x3, SquaredMagnitude, max...) NO se sustituye: el EXE ejecuta su codigo real y la DLL los ayudantes x87 de X87Math.cpp (Veh81A*).
// Para que el lado original ejecute de verdad el codigo del EXE se apagan los hooks RH de DotProduct 0x40FDB0, CrossProduct 0x59C730 y Normalise 0x59C910 durante el banco (se restauran al final).
// Los falsos registran sus argumentos EN ORDEN (contenido de los vectores de pila, etiquetas de arena) y pueden mutar el tractor, los remolques o los globales para comprobar que la funcion relee esos campos.

namespace V81Layout {
static_assert(offsetof(CVehicle, m_pTowingVehicle) == 0x4C4 && offsetof(CVehicle, m_nVehicleSubType) == 0x594, "CVehicle: tractor +0x4C4, subtipo +0x594");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) == 0x4, "CPlaceable: matriz en +0x14, posicion compacta en +4");
static_assert(offsetof(CPhysical, m_fMass) == 0x8C && offsetof(CPhysical, m_fTurnMass) == 0x90 && offsetof(CPhysical, m_vecCentreOfMass) == 0xA4, "CPhysical: masa +0x8C, masa de giro +0x90, centro de masa +0xA4");
static_assert(sizeof(CVehicle) <= V81Fix::kSelfWin, "la arena del tractor cubre el vehiculo");
}

namespace V81Live {
using namespace V81Fix;
inline constexpr uintptr_t kEntry=0x6DFC50;
inline constexpr uintptr_t kSpeed=0x542CE0,kForce=0x542B50,kDot=0x40FDB0,kCross=0x59C730,kNormalise=0x59C910;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uint16& G16(uintptr_t a) {return *reinterpret_cast<uint16*>(a);}
inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+kGuard+off,&w,2);}
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
// Etiquetas (orden de TagAdd): 0 remolque (this), 1 tractor A, 2 tractor B, 3 matriz del remolque, 4 matriz alternativa del remolque, 5 matriz del tractor A, 6 matriz U (alternativa de A y matriz de B)
inline Arena<kSelfArena> selfA;
inline Arena<kTowArena> towA,towB;
inline Arena<kMatArena> matS,matSAlt,matT,matU;
inline std::array<uintptr_t,80> vtbl{};   // vtabla falsa: 0xEC/4 = 59, 0xF0/4 = 60, 0xF8/4 = 62

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["step"]=G32(kStepAddr);
    obs["hoist"]=uint32(G16(kHoistAddr));
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ UpdateTrailerLink (0x6DFC50)
struct State {const C* c{};unsigned ev{},spd{};};
inline State g;
// Tras el evento numero mutAt, el falso muta el tractor, un remolque o un global (ver Fixtures.hpp: mutKind)
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    switch(c.mutKind) {
    case 1: selfA.Put32(0x4C4,u32(towB.Obj()));break;
    case 2: selfA.Put32(0x14,u32(matSAlt.Obj()));break;
    case 3: towA.Put32(0x14,u32(matU.Obj()));break;
    case 4: G32(kStepAddr)=c.mutVal;break;
    case 5: selfA.Put32(0x8C,c.mutVal);break;
    case 6: selfA.Put32(0x90,c.mutVal);break;
    case 7: selfA.Put32(0xA4,c.mutVal);break;
    case 8: selfA.Put32(0x594,c.mutVal);break;
    case 9: selfA.Put32(0x9EC,c.mutVal);break;
    case 10: towA.Put16(0x86C,c.mutVal);break;
    case 11: towA.Put16(0x22,c.mutVal);break;
    case 12: G16(kHoistAddr)=uint16(c.mutVal);break;
    default: break;
    }
}
// Eventos: 1 GetTowHitchPos(esto, salida [contenido], comprobar, vehiculo), 2 GetTowBarPos (igual), 3 BreakTowLink(esto), 4 GetSpeed(esto, punto xyz), 5 ApplyForce(esto, fuerza xyz, punto xyz, bool), 9 virtual inesperado
inline u32 __fastcall FakeHitch(void* self,void*,u32* out,int check,void* vehicle) {
    Ev& e=PushEv(1,6);e.v[0]=Tag(self);e.v[1]=out[0];e.v[2]=out[1];e.v[3]=out[2];e.v[4]=u32(check);e.v[5]=Tag(vehicle);
    const C& c=*g.c;
    for(int i=0;i<3;++i)out[i]=c.hitch[i];
    After();
    return c.hitchRet;
}
inline u32 __fastcall FakeBar(void* self,void*,u32* out,int check,void* vehicle) {
    Ev& e=PushEv(2,6);e.v[0]=Tag(self);e.v[1]=out[0];e.v[2]=out[1];e.v[3]=out[2];e.v[4]=u32(check);e.v[5]=Tag(vehicle);
    const C& c=*g.c;
    for(int i=0;i<3;++i)out[i]=c.bar[i];
    After();
    return c.barRet;
}
inline u32 __fastcall FakeBreak(void* self,void*) {
    Ev& e=PushEv(3,1);e.v[0]=Tag(self);
    After();
    return 0xCAFEBA00u;
}
inline u32 __fastcall FakeTrap(void* self,void*) {
    Ev& e=PushEv(9,1);e.v[0]=Tag(self);
    return 0u;
}
inline u32* __fastcall FakeSpeed(void* self,void*,u32* ret,u32 x,u32 y,u32 z) {
    Ev& e=PushEv(4,4);e.v[0]=Tag(self);e.v[1]=x;e.v[2]=y;e.v[3]=z;
    const C& c=*g.c;
    const u32* r=c.spd[g.spd++&1u];
    ret[0]=r[0];ret[1]=r[1];ret[2]=r[2];
    After();
    return ret;
}
inline void __fastcall FakeForce(void* self,void*,u32 fx,u32 fy,u32 fz,u32 px,u32 py,u32 pz,int flag) {
    Ev& e=PushEv(5,8);e.v[0]=Tag(self);e.v[1]=fx;e.v[2]=fy;e.v[3]=fz;e.v[4]=px;e.v[5]=py;e.v[6]=pz;e.v[7]=u32(flag);
    After();
}

inline void FillMatrix(auto& a,const u32* m,const u32* pos) {
    for(int i=0;i<12;++i)a.Put32(size_t(i)*4,m[i]);
    for(int i=0;i<3;++i)a.Put32(0x30+size_t(i)*4,pos[i]);
}

inline void CaseTrail(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        selfA.Fill(c.seed*13u+1u);towA.Fill(c.seed*13u+2u);towB.Fill(c.seed*13u+3u);
        matS.Fill(c.seed*13u+4u);matSAlt.Fill(c.seed*13u+5u);matT.Fill(c.seed*13u+6u);matU.Fill(c.seed*13u+7u);
        selfA.Put32(0,u32(uintptr_t(vtbl.data())));towA.Put32(0,u32(uintptr_t(vtbl.data())));towB.Put32(0,u32(uintptr_t(vtbl.data())));
        selfA.Put32(0x14,u32(matS.Obj()));selfA.Put8(0x36,c.statusByte);selfA.Put32(0x4C4,c.hasTrailer?u32(towA.Obj()):0u);
        selfA.Put32(0x8C,c.mass);selfA.Put32(0x90,c.turn);for(int i=0;i<3;++i)selfA.Put32(0xA4+size_t(i)*4,c.centre[i]);
        selfA.Put32(0x594,c.subtype);selfA.Put32(0x9EC,c.w9ec);
        towA.Put32(0x14,u32(matT.Obj()));towA.Put16(0x22,c.model);towA.Put16(0x86C,c.angle);
        towB.Put32(0x14,u32(matU.Obj()));towB.Put16(0x22,c.modelB);towB.Put16(0x86C,c.angleB);
        FillMatrix(matS,c.selfM,c.selfPos);FillMatrix(matSAlt,c.altSelfM,c.selfPos);FillMatrix(matT,c.towM,c.towPos);FillMatrix(matU,c.altTowM,c.altTowPos);
        selfA.Snap();towA.Snap();towB.Snap();matS.Snap();matSAlt.Snap();matT.Snap();matU.Snap();
        TagClear();selfA.Tagged();towA.Tagged();towB.Tagged();matS.Tagged();matSAlt.Tagged();matT.Tagged();matU.Tagged();
        G32(kStepAddr)=c.ts;G16(kHoistAddr)=uint16(c.hoist);
        g=State{};g.c=&c;nev=0;
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){
            const u32 args[2]{c.arg0,c.arg1};
            return Gate(entry,args,2,c.eax,u32(selfA.Obj()),c.edx,c.stack,0);
        });
        json obs;
        obs["trailer"]=Tag(uintptr_t(selfA.Get32(0x4C4)));
        obs["self_matrix"]=Tag(uintptr_t(selfA.Get32(0x14)));
        obs["tow_matrix"]=json::array({Tag(uintptr_t(towA.Get32(0x14))),Tag(uintptr_t(towB.Get32(0x14)))});
        obs["changes"]=json::array({selfA.Changes({{0x14,4},{0x4C4,4}}),towA.Changes({{0x14,4}}),towB.Changes({{0x14,4}}),matS.Changes(),matSAlt.Changes(),matT.Changes(),matU.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseTrail(env,totals,c);
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
    if(!std::filesystem::exists(dir/"veh81a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"veh81a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("veh81a");}
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
    // vtabla falsa compartida
    vtbl.fill(uintptr_t(&FakeTrap));
    vtbl[0xEC/4]=uintptr_t(&FakeHitch);vtbl[0xF0/4]=uintptr_t(&FakeBar);vtbl[0xF8/4]=uintptr_t(&FakeBreak);
    const Pair pSpeed=MakeCollab(env,kSpeed,hooksFound),pForce=MakeCollab(env,kForce,hooksFound);
    // Hooks RH de la aritmetica que el EXE ejecuta en el lado original: apagados (sin falso) para que el original sea el codigo del EXE
    json hooksOff=json::array();
    for(const uintptr_t a:{kDot,kCross,kNormalise}) {
        std::string cat,name;
        if(FindHook(a,cat,name)) {
            env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),a,HookRoute::RegisteredTarget{}));
            hooksOff.push_back({{"exe",a},{"class",cat},{"name",name}});
        }
        env.watches.emplace_back(a,16);
    }
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pSpeed,reinterpret_cast<void*>(&FakeSpeed)),S(pForce,reinterpret_cast<void*>(&FakeForce))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"aritmetica y ayudantes x87 de X87Math.cpp (Veh81A*)","CTimer::ms_fTimeStep y 0x8D313C (lecturas)","virtuales +0xEC/+0xF0/+0xF8 por la vtabla falsa"})},
            {"stubbed",stubbed},{"hooks_found",hooksFound},{"hooks_off",hooksOff},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
