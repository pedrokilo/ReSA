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
#include "CarCtrl.h"
#include "PathFind.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84G dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::SteerAICarWithPhysics (0x437C20, hook S, cdecl con un argumento, void). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (sus hooks RH
// quedan apagados durante el banco): CCarCtrl::SwitchBetweenPhysicsAndGhost 0x4222A0 (cdecl, vehiculo), CPathFind::AreNodesLoadedForArea 0x44DD10 (thiscall, cuatro floats; bool),
// CCarCtrl::JoinCarWithRoadAccordingToMission 0x432CB0 (cdecl, vehiculo), CCarCtrl::FindMaxSteerAngle 0x427FE0 (cdecl, vehiculo; float en st0) y CCarCtrl::SteerAICarWithPhysics_OnlyMission 0x436A90
// (cdecl, vehiculo y cuatro punteros a salidas; en la DLL es la envoltura que llama al EXE). Corren de verdad, en ambos lados, el producto escalar (0x40FDB0), v * f (0x40FEC0), a + b (0x40FE30) y el
// CPlaceable::GetForward del EXE (0x41CCB0) frente a los ayudantes x87 y AutoPreRenderPlaceableForward de la DLL, CVehicleRecording::bUseCarAI, CTimer y las constantes de .rdata (se vigilan sus bytes).
// Los falsos registran el estado del vehiculo EN ORDEN y escriben los valores que da el caso. Tras el evento numero mutAt el falso muta el estado (mision, banderas, accion temporal, tiempos, objetivo,
// velocidad, matriz, volante, banderas, modelo, nodos, relojes) para comprobar que la funcion relee esos campos donde el EXE los relee.

namespace C84GLive {
using namespace C84GFix;
inline constexpr uintptr_t kEntry = 0x437C20;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CVehicle,m_matrix)==0x14&&offsetof(CEntity,m_nModelIndex)==0x22&&offsetof(CVehicle,m_autoPilot)==0x390&&offsetof(CVehicle,m_fSteerAngle)==0x494
              &&offsetof(CVehicle,m_GasPedal)==0x49C&&offsetof(CVehicle,m_BrakePedal)==0x4A0&&offsetof(CPhysical,m_vecMoveSpeed)==0x44&&offsetof(CVehicle,m_nVehicleUpperFlags)==0x428,"CVehicle: campos");
static_assert(offsetof(CAutoPilot,m_nTimeToStartMission)==0x1C&&offsetof(CAutoPilot,m_nTimeSwitchedToRealPhysics)==0x20&&offsetof(CAutoPilot,m_nCarMission)==0x2A&&offsetof(CAutoPilot,m_nTempAction)==0x2B
              &&offsetof(CAutoPilot,m_nTempActionTime)==0x2C&&offsetof(CAutoPilot,m_nMovementFlags)==0x4C&&offsetof(CAutoPilot,m_TargetEntity)==0x8C&&offsetof(CAutoPilot,m_ObstructingEntity)==0x90
              &&offsetof(CAutoPilot,m_vehicleRecordingId)==0x94&&offsetof(CAutoPilot,m_startingRouteNode)==4&&offsetof(CAutoPilot,m_endingRouteNode)==8,"CAutoPilot: campos");

// Disposicion de CMatrix (miembros privados en algunos sitios: se mide con los accesores): derecha +0, adelante +0x10, arriba +0x20, posicion +0x30
inline bool MatrixLayoutOk() {
    CMatrix m;
    const auto base=uintptr_t(&m);
    return uintptr_t(&m.GetRight())-base==0x00&&uintptr_t(&m.GetForward())-base==0x10&&uintptr_t(&m.GetUp())-base==0x20&&uintptr_t(&m.GetPosition())-base==0x30;
}

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
    u32 Get16(size_t off) const {uint16 v;memcpy(&v,m.data()+kGuard+off,2);return v;}
    u32 Get8(size_t off) const {return m[kGuard+off];}
    // Cambios fuera de los campos que el banco compara por valor: known = [[inicio, fin)] respecto al objeto
    json Rest(std::initializer_list<std::pair<int64,int64>> known) const {
        json out=json::array();
        for(const auto& ch:ArenaChanges(m.data(),r.data(),N,kGuard)) {
            const int64 o=ch[0].get<int64>();
            bool isKnown=false;
            for(const auto& [lo,hi]:known)isKnown|=o>=lo&&o<hi;
            if(!isKnown)out.push_back(ch);
        }
        return out;
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA;

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
struct State {const C* c{};u32 evHash{};u32 ev{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline uint8* CurMat() {return reinterpret_cast<uint8*>(uintptr_t(vehA.Get32(0x14)));}
inline void PutAt(uint8* p,u32 v) {memcpy(p,&v,4);}
// Mutaciones: 1 mision, 2 banderas de movimiento, 3 accion temporal, 4 tiempo de la accion, 5 objetivo, 6 CTimer::m_snTimeInMilliseconds, 7 velocidad.x, 8 adelante.x de la matriz, 9 volante,
// 10 banderas del vehiculo, 11 indice de modelo, 12 nodo actual, 13 nodo inicial, 14 CTimer::ms_fTimeStep
inline void ApplyMut(u32 kind,u32 v) {
    switch(kind) {
    case 1: vehA.Put8(0x3BA,v);break;
    case 2: vehA.Put8(0x3DC,v);break;
    case 3: vehA.Put8(0x3BB,v);break;
    case 4: vehA.Put32(0x3BC,v);break;
    case 5: vehA.Put32(0x41C,v);break;
    case 6: G32(0xB7CB84)=v;break;
    case 7: vehA.Put32(0x44,v);break;
    case 8: if(CurMat())PutAt(CurMat()+0x10,v);break;
    case 9: vehA.Put32(0x494,v);break;
    case 10: vehA.Put32(0x428,v);break;
    case 11: vehA.Put16(0x22,v);break;
    case 12: vehA.Put32(0x390,v);break;
    case 13: vehA.Put32(0x394,v);break;
    case 14: G32(0xB7CB5C)=v;break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
// Eventos: 1 SwitchBetweenPhysicsAndGhost [&vehiculo, mision, accion, banderas de movimiento, objetivo, id de grabacion]
// 2 AreNodesLoadedForArea [ECX, minX, maxX, minY, maxY, banderas de movimiento, mision]   3 JoinCarWithRoadAccordingToMission [&vehiculo, banderas de movimiento, mision]
// 4 FindMaxSteerAngle [&vehiculo, accion, tiempo de la accion]   5 SteerAICarWithPhysics_OnlyMission [&vehiculo, &giro, &acelerador, &freno, &freno de mano, mision, accion, volante, reloj]
inline void __cdecl FakeSwitch(void* veh) {
    Ev& e=PushEv(1,6);e.v[0]=Tag(veh);e.v[1]=vehA.Get8(0x3BA);e.v[2]=vehA.Get8(0x3BB);e.v[3]=vehA.Get8(0x3DC);e.v[4]=vehA.Get32(0x41C);e.v[5]=vehA.Get8(0x424);HashEv(e);
    After();
}
inline bool __fastcall FakeAreNodes(void* self,void*,u32 minX,u32 maxX,u32 minY,u32 maxY) {
    Ev& e=PushEv(2,7);e.v[0]=u32(uintptr_t(self));e.v[1]=minX;e.v[2]=maxX;e.v[3]=minY;e.v[4]=maxY;e.v[5]=vehA.Get8(0x3DC);e.v[6]=vehA.Get8(0x3BA);HashEv(e);
    const bool ret=g.c->loaded!=0;
    After();
    return ret;
}
inline void __cdecl FakeJoin(void* veh) {
    Ev& e=PushEv(3,3);e.v[0]=Tag(veh);e.v[1]=vehA.Get8(0x3DC);e.v[2]=vehA.Get8(0x3BA);HashEv(e);
    After();
}
inline float __cdecl FakeFindMax(void* veh) {
    Ev& e=PushEv(4,3);e.v[0]=Tag(veh);e.v[1]=vehA.Get8(0x3BB);e.v[2]=vehA.Get32(0x3BC);HashEv(e);
    const float v=std::bit_cast<float>(g.c->findMax);
    After();
    return v;
}
inline void __cdecl FakeOnlyMission(void* veh,void* steer,void* gas,void* brake,void* handbrake) {
    Ev& e=PushEv(5,9);e.v[0]=Tag(veh);e.v[1]=Tag(steer);e.v[2]=Tag(gas);e.v[3]=Tag(brake);e.v[4]=Tag(handbrake);e.v[5]=vehA.Get8(0x3BA);e.v[6]=vehA.Get8(0x3BB);e.v[7]=vehA.Get32(0x494);e.v[8]=G32(0xB7CB84);HashEv(e);
    const C& c=*g.c;
    memcpy(steer,&c.om[0],4);memcpy(gas,&c.om[1],4);memcpy(brake,&c.om[2],4);
    *static_cast<uint8*>(handbrake)=uint8(c.omHb);
    After();
}

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
inline json WordsOf(const Arena<kMatArena>& a) {
    json words=json::array();
    for(unsigned o:{0u,4u,8u,0x10u,0x14u,0x18u,0x20u,0x24u,0x28u,0x30u,0x34u,0x38u})words.push_back(a.Get32(o));
    return words;
}
inline void CaseSteer(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);
        for(unsigned i=0;i<3;++i) {
            matA.Put32(4*i,c.mr[i]);matA.Put32(0x10+4*i,c.mf[i]);matA.Put32(0x20+4*i,c.mu[i]);matA.Put32(0x30+4*i,c.mp[i]);
            vehA.Put32(4+4*i,c.place[i]);vehA.Put32(0x44+4*i,c.speed[i]);vehA.Put32(0x390+4*i,c.node[i]);
        }
        vehA.Put32(0x10,c.heading);
        vehA.Put32(0x988,0);vehA.Put32(0x994,0);
        vehA.Put32(0x14,c.matNull?0u:u32(matA.Obj()));
        vehA.Put16(0x22,c.model);
        vehA.Put32(0x3AC,c.tstart);vehA.Put32(0x3B0,c.tswitch);
        vehA.Put8(0x3BA,c.mission);vehA.Put8(0x3BB,c.tact);vehA.Put32(0x3BC,c.ttime);vehA.Put8(0x3DC,c.mov);
        vehA.Put32(0x41C,c.target);vehA.Put32(0x420,c.obst);vehA.Put8(0x424,c.rec);vehA.Put32(0x428,c.vflags);
        vehA.Put32(0x494,c.steer0);vehA.Put32(0x49C,c.gas0);vehA.Put32(0x4A0,c.brk0);
        vehA.Snap();matA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();
        // regiones del juego: bUseCarAI (solo la casilla del id, si es >= 0), reloj y paso
        memset(regions[0].Ptr(),0,128);
        if(c.rec<0x80)regions[0].Ptr()[c.rec]=uint8(c.useAi);
        G32(0xB7CB84)=c.now;G32(0xB7CB5C)=c.ts;
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        const u32 args[1]{u32(vehA.Obj())};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["v"]=json::array({vehA.Get8(0x3BA),vehA.Get8(0x3BB),vehA.Get32(0x3BC),vehA.Get32(0x3AC),vehA.Get32(0x3B0),vehA.Get32(0x390),vehA.Get32(0x394),vehA.Get32(0x398),vehA.Get8(0x3DC),
            vehA.Get32(0x41C),vehA.Get32(0x420),vehA.Get32(0x428),vehA.Get32(0x44),vehA.Get32(0x48),vehA.Get32(0x4C),vehA.Get32(0x494),vehA.Get32(0x49C),vehA.Get32(0x4A0),vehA.Get16(0x22),
            vehA.Get32(0x988),vehA.Get32(0x994),G32(0xB7CB84),G32(0xB7CB5C)});
        obs["mat"]=WordsOf(matA);
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        {   // escrituras esperadas: el vehiculo (indice de modelo, velocidad, nodos, tiempos, accion, banderas, objetivo, volante y pedales, salidas del Vortex) y adelante.x de la matriz; nada mas
            obs["changes"]=json::array({vehA.Rest({{0x22,0x24},{0x44,0x50},{0x390,0x39C},{0x3AC,0x3B4},{0x3BA,0x3C0},{0x3DC,0x3DD},{0x41C,0x424},{0x428,0x42C},{0x494,0x4A4},{0x988,0x98C},{0x994,0x998}}),
                matA.Rest({{0x10,0x14}})});
        }
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
    json cases=json::object();for(unsigned f=0;f<kFunctions;++f)cases[kFn[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseSteer(env,totals,c);
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
    if(!std::filesystem::exists(dir/"car84g-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84g-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84g");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=MatrixLayoutOk();json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
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
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
    env.stubs[0]={
        S(MakeCollab(env,0x4222A0,FnAddr(&CCarCtrl::SwitchBetweenPhysicsAndGhost),hooksFound),Void(&FakeSwitch)),
        S(MakeCollab(env,0x44DD10,FnAddr(&CPathFind::AreNodesLoadedForArea),hooksFound),Void(&FakeAreNodes)),
        S(MakeCollab(env,0x432CB0,FnAddr(&CCarCtrl::JoinCarWithRoadAccordingToMission),hooksFound),Void(&FakeJoin)),
        S(MakeCollab(env,0x427FE0,FnAddr(&CCarCtrl::FindMaxSteerAngle),hooksFound),Void(&FakeFindMax)),
        S(MakeCollab(env,0x436A90,FnAddr(&CCarCtrl::SteerAICarWithPhysics_OnlyMission),hooksFound),Void(&FakeOnlyMission))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"producto escalar 0x40FDB0, v * f 0x40FEC0, a + b 0x40FE30 y CPlaceable::GetForward 0x41CCB0 del EXE frente a los ayudantes x87 y AutoPreRenderPlaceableForward de la DLL","CVehicleRecording::bUseCarAI y CTimer (regiones restauradas)","constantes de .rdata (se vigilan sus bytes)","ayudantes x87 de X87Math.cpp (Veh83D*, Veh83C*)"})},
            {"virtual_fakes",json::array()},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(const uintptr_t a:{uintptr_t(0x858CEC),uintptr_t(0x858B8C),uintptr_t(0x858B50),uintptr_t(0x858CC4),uintptr_t(0x858624),uintptr_t(0x859070),uintptr_t(0x858CDC),uintptr_t(0x859054),uintptr_t(0x858B1C)})env.watches.emplace_back(a,4);
    for(const uintptr_t a:{uintptr_t(0x859068),uintptr_t(0x859060),uintptr_t(0x859058)})env.watches.emplace_back(a,8);
    env.watches.emplace_back(0x4382E4,41);   // tabla de clases de la mision (mision - 15)
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
