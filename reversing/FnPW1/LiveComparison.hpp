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
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Physical.h"
#include "Core/Vector.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FnPW1 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CVehicle::ProcessWheel (0x6D6C00, hook S, thiscall con 12 dwords, `ret 0x30`). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (sus hooks RH quedan apagados durante el banco):
//   CPhysical::ApplyMoveForce(CVector) 0x5429F0 y CPhysical::ApplyTurnForce 0x542A50 (thiscall, vectores por valor, callee limpia). El producto vectorial corre de verdad en ambos lados (0x59C730 del EXE; el de la DLL, sin hook propio) y el
//   oraculo lo calcula aparte: asi tambien se valida la fuerza de giro que la DLL obtiene con CVector::Cross (SSE) frente a la x87 del EXE.
//   Corren de verdad en ambos lados: rand() del CRT del EXE (0x821B1E), CTimer::ms_fTimeStep, gHandlingDataMgr.fWheelFriction, el CHandlingData (+4 masa, +0xA4 perdida de traccion), la matriz del vehiculo
//   y las estaticas 0xC1CDAC..0xC1CDAE (regiones restauradas tras cada lado).
// Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, locales de pila por su marca). La fuerza y el torque aplicados son los argumentos de ApplyMoveForce / ApplyTurnForce.

namespace PW1Live {
using namespace PW1Fix;
inline constexpr uintptr_t kEntry = 0x6D6C00;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CVehicle,m_nVehicleType)==0x590&&offsetof(CVehicle,m_nVehicleSubType)==0x594&&offsetof(CVehicle,m_GasPedal)==0x49C&&offsetof(CVehicle,m_pHandlingData)==0x384);
static_assert(offsetof(CPhysical,m_fMass)==0x8C&&offsetof(CPhysical,m_fTurnMass)==0x90);
static_assert(offsetof(CPlaceable,m_matrix)==0x14&&offsetof(CEntity,m_nModelIndex)==0x22);
static_assert(sizeof(CAutomobile)<=kVehWin);
static_assert(offsetof(tHandlingData,m_fMass)==0x4&&offsetof(tHandlingData,m_fTractionLoss)==0xA4&&offsetof(tHandlingData,m_fSuspensionAntiDiveMultiplier)==0xC4);
static_assert(sizeof(CEntity::CEntityInfo)==1);   // el byte 0x36 (tipo + estado) lo comprueba el propio banco: el EXE lo lee en 0x36 y la DLL por m_info

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
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kHandArena> handA;
inline Arena<kArgArena> argA;       // wheelFwd +0, wheelRight +0x10, wheelContactSpeed +0x20, wheelContactPoint +0x30
inline Arena<kStateArena> stateA;   // *wheelState (+0)
inline Arena<kSpeedArena> speedA;   // wheelSpeed (no se usa)

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

// ================================================================================================ falsos de los colaboradores
struct State {const C* c{};u32 evHash{};u32 unexpected{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
// Las palabras de float de los eventos se registran con cualquier NaN canonizado a 0x7FC00000 (se compara "NaN con NaN", no la carga ni el signo); el resto, bit a bit.
inline u32 Canon(u32 w) {return (w&0x7F800000u)==0x7F800000u&&(w&0x7FFFFFu)!=0?0x7FC00000u:w;}
inline u32 Raw(const void* p,unsigned i) {return Canon(BitsOf(static_cast<const u8*>(p)+4*i));}
// Eventos: 3 ApplyMoveForce [this, fuerza(3)], 4 ApplyTurnForce [this, fuerza(3), punto(3)], 99 virtual inesperado
inline void __fastcall FakeMove(void* self,void*,u32 x,u32 y,u32 z) {Ev& e=PushEv(3,4);e.v[0]=Tag(self);e.v[1]=Canon(x);e.v[2]=Canon(y);e.v[3]=Canon(z);HashEv(e);}
inline void __fastcall FakeTurn(void* self,void*,u32 fx,u32 fy,u32 fz,u32 px,u32 py,u32 pz) {
    Ev& e=PushEv(4,7);e.v[0]=Tag(self);e.v[1]=Canon(fx);e.v[2]=Canon(fy);e.v[3]=Canon(fz);e.v[4]=Canon(px);e.v[5]=Canon(py);e.v[6]=Canon(pz);HashEv(e);
}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;Ev& e=PushEv(99,1);e.v[0]=Tag(self);HashEv(e);return 0;}
// vtabla falsa del vehiculo: ninguna entrada es valida (la funcion no llama a virtuales)
inline std::array<u32,96> vtbl{};
inline void InitVtbl() {for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));}

// ------------------------------------------------------------------------------------------------ microtests de la x87 (hardware): las reglas que supone el oraculo
// op: 0 fld a; fadd b | 1 fld a; fsub b | 2 fld a; fmul b | 3 fld a; fdiv b | 4 fld a; fsqrt | 5 fld a; fistp qword (trunca) -> EAX | 6 fld a; fabs. El resultado de 0..4 y 6 es el fstp dword.
inline u32 X87Micro(u32 op,u32 a,u32 b) {
    u32 r{};
    switch(op) {
    case 0: __asm {fld dword ptr a} __asm {fadd dword ptr b} __asm {fstp dword ptr r} break;
    case 1: __asm {fld dword ptr a} __asm {fsub dword ptr b} __asm {fstp dword ptr r} break;
    case 2: __asm {fld dword ptr a} __asm {fmul dword ptr b} __asm {fstp dword ptr r} break;
    case 3: __asm {fld dword ptr a} __asm {fdiv dword ptr b} __asm {fstp dword ptr r} break;
    case 4: __asm {fld dword ptr a} __asm {fsqrt} __asm {fstp dword ptr r} break;
    case 5: {
        uint16 cw{},cwTrunc{};uint64 wide{};
        __asm {fnstcw word ptr cw}
        cwTrunc=uint16(cw|0x0C00);
        __asm {fldcw word ptr cwTrunc}
        __asm {fld dword ptr a} __asm {fistp qword ptr wide}
        __asm {fldcw word ptr cw}
        r=u32(wide&0xFFFFFFFFu);
        break;
    }
    default: __asm {fld dword ptr a} __asm {fabs} __asm {fstp dword ptr r} break;
    }
    return r;
}
inline json MicroTests() {
    json out=json::array();
    PrepareFloatingState();
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0xFFC54321u,0x7F800001u,0xFFA00003u,0x7FA00000u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40A00000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,
        0xFFC00001u,0x7FC00002u,0x7FC00000u,0xFFC00000u,0xFF800001u,0x7FA00003u,0xFF800002u,0x7F800002u};
    for(u32 op=0;op<4;++op)for(u32 a:vals)for(u32 b:vals)out.push_back({op,a,b,X87Micro(op,a,b)});
    static constexpr u32 unary[]{0x7FC00001u,0x7F800001u,0xFFC12345u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40000000u,0x40800000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,0x41F00000u,0x4F000000u,0x4F800000u,0x5F000000u,0x5F7FFFFFu,0xCF000000u,0x4EFFFFFFu,0x2F800000u,0x3F7FFFFFu,0xBF7FFFFFu,0x4B000001u,0x5E800000u};
    for(u32 op:{4u,5u,6u})for(u32 a:unary)out.push_back({op,a,0u,X87Micro(op,a,0u)});
    RestoreFloatingState();
    return out;
}

// ================================================================================================ el caso
inline void CaseWheel(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);handA.Fill(c.seed*17u+3u);argA.Fill(c.seed*17u+4u);stateA.Fill(c.seed*17u+5u);speedA.Fill(c.seed*17u+6u);
        // vehiculo
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        vehA.Put16(0x22,c.model);
        vehA.Put8(0x36,c.vstat);
        vehA.Put32(0x8C,c.mass);
        vehA.Put32(0x90,c.turn);
        vehA.Put32(0x384,u32(handA.Obj()));
        vehA.Put32(0x49C,c.gas);
        vehA.Put32(0x590,c.vtype);
        vehA.Put32(0x594,c.subtype);
        handA.Put32(4,c.hmass);
        handA.Put32(0xA4,c.tl);
        handA.Put32(0xC4,c.sadm);
        // argumentos
        for(unsigned i=0;i<3;++i) {argA.Put32(4*i,c.fwd[i]);argA.Put32(0x10+4*i,c.right[i]);argA.Put32(0x20+4*i,c.cs[i]);argA.Put32(0x30+4*i,c.cp[i]);}
        stateA.Put32(0,c.wstate);
        // globales
        G32(kTimeStep)=c.ts;
        G32(kWheelFriction)=c.fric;
        {const uint8 flags[4]{uint8(c.af),uint8(c.b1),uint8(c.b2),0};memcpy(reinterpret_cast<void*>(kFlagAlready),flags,4);}
        vehA.Snap();handA.Snap();argA.Snap();stateA.Snap();speedA.Snap();
        TagClear();vehA.Tagged();handA.Tagged();argA.Tagged();stateA.Tagged();speedA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        *RandStatePtr()=c.randSeed;   // mismo estado inicial del rand() real del EXE en ambos lados
        const u32 args[12]{u32(argA.Obj(kGuard+0x00)),u32(argA.Obj(kGuard+0x10)),u32(argA.Obj(kGuard+0x20)),u32(argA.Obj(kGuard+0x30)),c.wheels,c.thrust,c.brake,c.adh,c.wheelId,u32(speedA.Obj()),u32(stateA.Obj()),c.status};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,12,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["rand_state"]=*RandStatePtr();
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["wheel_state"]=stateA.Get32(0);
        obs["flags"]=json::array({G32(kFlagAlready)&0xFFu,(G32(kFlagAlready)>>8)&0xFFu,(G32(kFlagAlready)>>16)&0xFFu,(G32(kFlagAlready)>>24)&0xFFu});
        obs["unexpected_virtuals"]=g.unexpected;
        {   // la unica escritura esperada en las arenas es *wheelState (+0); se registra a parte
            auto changes=stateA.Changes();
            json rest=json::array();for(const auto& ch:changes)if(ch[0].get<int64>()>=4||ch[0].get<int64>()<0)rest.push_back(ch);
            obs["changes"]=json::array({vehA.Changes(),handA.Changes(),argA.Changes(),rest,speedA.Changes()});
        }
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D);   // rand real del EXE en ambos lados
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
    json cases=json::object();for(unsigned f=0;f<kFunctions;++f)cases[kFn[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseWheel(env,totals,c);
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
// Colaborador: si la DLL tiene hook RH (se apaga al crear su ruta) se anota; el cuerpo de la DLL se toma siempre de la funcion C++ (puntero a funcion), de modo que tambien las funciones sin hook
// (llamadas directamente por la DLL) llevan el mismo falso.
inline Pair MakeCollab(Env& env,uintptr_t exe,uintptr_t dllFunction,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr}});
    p.dll=Body(dllFunction);
    return p;
}
template<class T> inline uintptr_t FnAddr(T f) {
    if constexpr (std::is_member_function_pointer_v<T>) return std::bit_cast<uintptr_t>(f); else return reinterpret_cast<uintptr_t>(f);
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fnpw1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fnpw1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fnpw1");}
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
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    env.stubs[0]={
        S(MakeCollab(env,0x5429F0,FnAddr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),hooksFound),Void(&FakeMove)),
        S(MakeCollab(env,0x542A50,FnAddr(&CPhysical::ApplyTurnForce),hooksFound),Void(&FakeTurn))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"rand() del CRT del EXE (0x821B1E)","CTimer::ms_fTimeStep, gHandlingDataMgr.fWheelFriction, estaticas 0xC1CDAC..0xC1CDAF (regiones restauradas)","CHandlingData y vehiculo (arenas)"})},
            {"virtual_fakes",json::array()},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    env.watches.emplace_back(0x827B3D,16);   // getptd
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
