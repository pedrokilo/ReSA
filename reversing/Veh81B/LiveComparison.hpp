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

// Banco par a par Veh81B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::UpdateTractorLink (0x6E0050, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado durante el banco):
//   CPhysical::GetSpeed 0x542CE0 (thiscall con puntero de resultado oculto y el vector por valor, ret 0x10), CVector::Normalise 0x59C910 (thiscall, ret), CrossProduct(out,a,b) 0x59C730 (cdecl, devuelve out)
//   y CPhysical::ApplyForce 0x542B50 (thiscall, ret 0x1C). Los dos virtuales (CVehicle::GetTowHitchPos +0xEC del remolque y GetTowBarPos +0xF0 del tractor) los atienden los falsos de la vtabla de las arenas.
//   Sin hook y SIN falso (se ejecutan tal cual en ambos lados, validados con la funcion): 0x40FE90 (v*s), 0x4119D0 (v/s), 0x420800 (max), 0x406D70 (-=) y 0x59C790 (matriz*vector); en la DLL son ayudantes x87 (X87Math).
// Los falsos registran sus argumentos EN ORDEN (punteros de pila por su contenido, punteros de arena por etiqueta) y tras cada uno puede mutar el estado (remolque, matrices, masas, paso de tiempo, modelo, gancho,
// campos de equipaje) para comprobar las relecturas. Tractor y remolques son objetos de 0x9F8 bytes en arenas (el EXE lee hasta +0x9EC); las matrices, de 0x48.

namespace V81Live {
using namespace V81Fix;
inline constexpr uintptr_t kEntry=0x6E0050;
inline constexpr uintptr_t kSpeed=0x542CE0,kNormalise=0x59C910,kCross=0x59C730,kForce=0x542B50;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

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
inline Arena<kVehArena> vehA,trA,tr2A;
inline Arena<kMatArena> mV,mV2,mT,mT2;

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

// ================================================================================================ UpdateTractorLink (0x6E0050)
struct State {const C* c{};unsigned ev{},spd{};std::array<u32,3> crossBuf{};unsigned unexpected{};};
inline State g;
inline u32 PtrTag(const void* p) {const u32 t=Tag(p);return t==u32(uintptr_t(p))?0xFFFF0001u:t;}   // fuera de las arenas = pila
// Tras el evento numero mutAt, el falso muta el estado (ver kMutKinds en Fixtures.hpp)
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    switch(c.mutKind) {
    case 1: vehA.Put32(0x4C8,u32(tr2A.Obj()));break;
    case 2: vehA.Put32(0x14,(c.mutVal&1u)?u32(mV2.Obj()):u32(mV.Obj()));break;
    case 3: trA.Put32(0x14,(c.mutVal&1u)?u32(mT2.Obj()):u32(mT.Obj()));break;
    case 4: vehA.Put32(0x8C,c.mutVal2);break;
    case 5: vehA.Put32(0x90,c.mutVal2);break;
    case 6: trA.Put32(0x8C,c.mutVal2);break;
    case 7: G32(kTimeAddr)=c.mutVal2;break;
    case 8: vehA.Put16(0x22,c.mutVal);break;
    case 9: vehA.Put16(0x86C,c.mutVal);break;
    case 10: trA.Put32(0x594,kLuggageSub);trA.Put32(0x9EC,kLuggageSentinel);break;
    case 11: trA.Put32(0x594,c.mutVal);break;
    case 12: mV.Put32(kMatOff[c.mutVal%12u],c.mutVal2);break;
    case 13: tr2A.Put32(0x8C,c.mutVal2);break;
    case 14: mT.Put32(kMatOff[c.mutVal%12u],c.mutVal2);break;
    default: break;
    }
}
// Eventos: 1 GetTowHitchPos(remolque): [this, salida, salida xyz antes, bandera, vehiculo]; 2 GetTowBarPos(tractor): idem; 3 y 4 GetSpeed (remolque, tractor): [this, salida, punto xyz];
// 5 Normalise: [vector xyz antes]; 6 CrossProduct: [a xyz, b xyz]; 7 ApplyForce: [this, fuerza xyz, punto xyz, bandera]
inline u32 __fastcall FakeHitch(void* self,void*,u32* out,u32 flag,void* vehicle) {
    Ev& e=PushEv(1,7);e.v[0]=Tag(self);e.v[1]=PtrTag(out);e.v[2]=out[0];e.v[3]=out[1];e.v[4]=out[2];e.v[5]=flag;e.v[6]=Tag(vehicle);
    for(int i=0;i<3;++i)out[i]=g.c->hitch[i];
    After();
    return g.c->retHitch;
}
inline u32 __fastcall FakeBar(void* self,void*,u32* out,u32 flag,void* vehicle) {
    Ev& e=PushEv(2,7);e.v[0]=Tag(self);e.v[1]=PtrTag(out);e.v[2]=out[0];e.v[3]=out[1];e.v[4]=out[2];e.v[5]=flag;e.v[6]=Tag(vehicle);
    for(int i=0;i<3;++i)out[i]=g.c->bar[i];
    After();
    return g.c->retBar;
}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;PushEv(99,1).v[0]=Tag(self);return 0;}
inline u32* __fastcall FakeSpeed(void* self,void*,u32* out,u32 x,u32 y,u32 z) {
    const unsigned idx=g.spd++&1u;
    Ev& e=PushEv(3+idx,5);e.v[0]=Tag(self);e.v[1]=PtrTag(out);e.v[2]=x;e.v[3]=y;e.v[4]=z;
    for(int i=0;i<3;++i)out[i]=g.c->spd[idx][i];
    After();
    return out;
}
inline void __fastcall FakeNormalise(u32* v,void*) {
    Ev& e=PushEv(5,4);e.v[0]=PtrTag(v);e.v[1]=v[0];e.v[2]=v[1];e.v[3]=v[2];
    for(int i=0;i<3;++i)v[i]=g.c->norm[i];
    After();
}
inline u32* __cdecl FakeCross(u32* out,const u32* a,const u32* b) {
    Ev& e=PushEv(6,9);e.v[0]=PtrTag(out);for(int i=0;i<3;++i){e.v[1+i]=a[i];e.v[4+i]=b[i];}e.v[7]=PtrTag(a);e.v[8]=PtrTag(b);
    const C& c=*g.c;
    u32* ret=out;
    if(c.crossMode) {   // basura en la salida y el resultado en otro buffer: el EXE lee el resultado por el puntero devuelto
        for(int i=0;i<3;++i){out[i]=c.decoy[i];g.crossBuf[i]=c.cross[i];}
        ret=g.crossBuf.data();
    } else for(int i=0;i<3;++i)out[i]=c.cross[i];
    After();
    return ret;
}
inline void __fastcall FakeForce(void* self,void*,u32 fx,u32 fy,u32 fz,u32 px,u32 py,u32 pz,u32 flag) {
    Ev& e=PushEv(7,8);e.v[0]=Tag(self);e.v[1]=fx;e.v[2]=fy;e.v[3]=fz;e.v[4]=px;e.v[5]=py;e.v[6]=pz;e.v[7]=flag;
    After();
}
// vtabla falsa compartida por las arenas: solo +0xEC (59) y +0xF0 (60) son validas
inline std::array<u32,96> vtbl{};
inline void InitVtbl() {
    for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));
    vtbl[59]=u32(reinterpret_cast<uintptr_t>(&FakeHitch));
    vtbl[60]=u32(reinterpret_cast<uintptr_t>(&FakeBar));
}

inline void SetGlobals(const C& c) {G32(kTimeAddr)=c.ts;}
inline void PutMat(Arena<kMatArena>& a,const u32* v) {for(unsigned i=0;i<12;++i)a.Put32(kMatOff[i],v[i]);}

inline void CaseUpdate(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);trA.Fill(c.seed*13u+2u);tr2A.Fill(c.seed*13u+3u);mV.Fill(c.seed*13u+4u);mV2.Fill(c.seed*13u+5u);mT.Fill(c.seed*13u+6u);mT2.Fill(c.seed*13u+7u);
        PutMat(mV,c.vm);PutMat(mV2,c.vm2);PutMat(mT,c.tm);PutMat(mT2,c.tm2);
        // tractor
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        vehA.Put32(0x14,c.hasVMat?u32(mV.Obj()):0u);
        for(unsigned i=0;i<3;++i)vehA.Put32(4+4*i,c.vfb[i]);
        vehA.Put16(0x20,c.model>>16);vehA.Put16(0x22,c.model);
        vehA.Put32(0x8C,c.vmass);vehA.Put32(0x90,c.vturn);
        for(unsigned i=0;i<3;++i)vehA.Put32(0xA4+4*i,c.com[i]);
        vehA.Put32(0x4C8,c.hasTrailer?u32(trA.Obj()):0u);
        vehA.Put16(0x86C,c.hoist);vehA.Put16(0x86E,c.hoist>>16);
        vehA.Put8(0xB8,0xA5);   // la funcion lo pone a cero al terminar
        // remolques
        trA.Put32(0,u32(uintptr_t(vtbl.data())));tr2A.Put32(0,u32(uintptr_t(vtbl.data())));
        trA.Put32(0x14,c.hasTMat?u32(mT.Obj()):0u);tr2A.Put32(0x14,u32(mT2.Obj()));
        for(unsigned i=0;i<3;++i){trA.Put32(4+4*i,c.tfb[0][i]);tr2A.Put32(4+4*i,c.tfb[1][i]);}
        trA.Put32(0x8C,c.tmass[0]);tr2A.Put32(0x8C,c.tmass[1]);
        trA.Put32(0x594,c.tsub[0]);tr2A.Put32(0x594,c.tsub[1]);
        trA.Put32(0x9EC,c.tsent[0]);tr2A.Put32(0x9EC,c.tsent[1]);
        vehA.Snap();trA.Snap();tr2A.Snap();mV.Snap();mV2.Snap();mT.Snap();mT2.Snap();
        TagClear();vehA.Tagged();trA.Tagged();tr2A.Tagged();mV.Tagged();mV2.Tagged();mT.Tagged();mT2.Tagged();
        SetGlobals(c);
        g=State{};g.c=&c;nev=0;
        const u32 args[2]{c.arg0,c.arg1};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["ptrs"]=json::array({Tag(uintptr_t(vehA.Get32(0x14))),Tag(uintptr_t(vehA.Get32(0x4C8))),Tag(uintptr_t(trA.Get32(0x14))),Tag(uintptr_t(tr2A.Get32(0x14)))});
        obs["changes"]=json::array({vehA.Changes({{0x14,4},{0x4C8,4}}),trA.Changes({{0x14,4}}),tr2A.Changes({{0x14,4}}),mV.Changes(),mV2.Changes(),mT.Changes(),mT2.Changes()});
        obs["events"]=EvsJson();
        obs["unexpected_virtuals"]=g.unexpected;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ microtests de la x87 (hardware): `fld a; fcomp b` frente al modelo del oraculo (el maximo de 0x420800)
// FSW & 0x4500 tras `fld a; fcomp b`: 0x0000 si a > b, 0x0100 si a < b, 0x4000 si a == b, 0x4500 si no son ordenables (NaN)
inline u32 X87Cmp(u32 a,u32 b) {
    uint16 sw{};
    __asm {fld dword ptr a} __asm {fcomp dword ptr b} __asm {fnstsw word ptr sw}
    return u32(sw)&0x4500u;
}
inline json MicroTests() {
    json out=json::array();
    PrepareFloatingState();
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0x7F800001u,0xFFA00003u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x3F7FFFFFu,0x3F800001u,0xBF7FFFFFu,
        0xC1000000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x7F7FFFFFu,0xFF7FFFFFu,0x3F000000u,0x42C80000u,0xFFC00000u};
    for(u32 a:vals)for(u32 b:vals)out.push_back({0,a,b,X87Cmp(a,b)});
    RestoreFloatingState();
    return out;
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
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseUpdate(env,totals,c);
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
    if(!std::filesystem::exists(dir/"veh81b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"veh81b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("veh81b");}
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
    InitVtbl();
    // CVector::Normalise y CrossProduct(out) no los encuentra FindHook (categoria "Core"): se resuelve su cuerpo en la DLL directamente (como Helpers66D)
    void (CVector::*normalise)()=&CVector::Normalise;
    CVector* (*crossOut)(CVector*,CVector*,CVector*)=&CrossProduct;
    const Pair pSpeed=MakeCollab(env,kSpeed,hooksFound),pForce=MakeCollab(env,kForce,hooksFound);
    const Pair pNorm{kNormalise,Body(std::bit_cast<uintptr_t>(normalise)),false},pCross{kCross,Body(reinterpret_cast<uintptr_t>(crossOut)),false};
    hooksFound.push_back({{"exe",kNormalise},{"class","CVector"},{"name","Normalise"},{"dll_body",pNorm.dll}});
    hooksFound.push_back({{"exe",kCross},{"class","(global)"},{"name","CrossProduct-out"},{"dll_body",pCross.dll}});
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pSpeed,reinterpret_cast<void*>(&FakeSpeed)),S(pNorm,reinterpret_cast<void*>(&FakeNormalise)),S(pCross,reinterpret_cast<void*>(&FakeCross)),S(pForce,reinterpret_cast<void*>(&FakeForce))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTimer::ms_fTimeStep (lectura)","aritmetica y ayudantes x87 de X87Math.cpp (Veh81B*) en lugar de 0x40FE90, 0x4119D0, 0x420800, 0x406D70 y 0x59C790 del EXE"})},
            {"virtual_fakes",json::array({"GetTowHitchPos +0xEC (remolque)","GetTowBarPos +0xF0 (tractor)"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
