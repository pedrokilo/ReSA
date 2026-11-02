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
#include "CarAI.h"
#include "TrafficLights.h"
#include "PathFind.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84H dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::SteerAICarWithPhysicsFollowPath (0x434900, hook S, cdecl con cinco argumentos (vehiculo y cuatro punteros a salidas), void). Colaboradores sustituidos por falsos identicos en la
// direccion del EXE y en el cuerpo de la DLL (sus hooks RH quedan apagados durante el banco): CCarCtrl::StopCarIfNodesAreInvalid 0x422590, PickNextNodeAccordingStrategy 0x432B10,
// SteerAICarWithPhysicsHeadingForTarget 0x433280, FindAngleToWeaveThroughTraffic 0x4325C0, FindMaxSteerAngle 0x427FE0, FindMaximumSpeedForThisCarInTraffic 0x434400, FindSpeedMultiplier 0x4224E0 (cuerpo
// Cc63SpeedMultiplierCpp), CTrafficLights::ShouldCarStopForLight 0x49D610 y ShouldCarStopForBridge 0x49D420, CCarAI::CarHasReasonToStop 0x41C050 y FindPlayerCoors 0x56E010. Corren de verdad, con el hook
// apagado (bytes del EXE frente al C++ de la DLL): CGeneral::GetATanOfXY 0x53CC70 y CCarPathLink::OneWayLaneOffset 0x44DB00, y en el EXE el resto aritmetico (0x40FE60 resta, 0x4082C0 modulo, 0x40FDB0 producto
// escalar) frente a los ayudantes x87 de la DLL. Los falsos registran sus argumentos y el estado del vehiculo EN ORDEN y escriben los valores que da el caso. Tras el evento numero mutAt el falso muta el
// estado para comprobar que la funcion relee esos campos donde el EXE los relee.

namespace C84HLive {
using namespace C84HFix;
inline constexpr uintptr_t kEntry = 0x434900;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CVehicle,m_matrix)==0x14&&offsetof(CVehicle,m_autoPilot)==0x390&&offsetof(CPhysical,m_vecMoveSpeed)==0x44,"CVehicle: campos");
static_assert(offsetof(CAutoPilot,m_nCurrentPathNodeInfo)==0x14&&offsetof(CAutoPilot,m_nNextPathNodeInfo)==0x16&&offsetof(CAutoPilot,_smthCurr)==0x25&&offsetof(CAutoPilot,_smthNext)==0x26
              &&offsetof(CAutoPilot,m_nCurrentLane)==0x27&&offsetof(CAutoPilot,m_nNextLane)==0x28&&offsetof(CAutoPilot,m_nCarDrivingStyle)==0x29&&offsetof(CAutoPilot,m_nCarMission)==0x2A
              &&offsetof(CAutoPilot,m_nTempAction)==0x2B&&offsetof(CAutoPilot,m_nTempActionTime)==0x2C&&offsetof(CAutoPilot,m_nCruiseSpeed)==0x40&&offsetof(CAutoPilot,m_vecDestinationCoors)==0x5C
              &&offsetof(CAutoPilot,m_TargetEntity)==0x8C,"CAutoPilot: campos");

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
inline Arena<kOutArena> outA;
inline Arena<kTgtArena> tgtA;
inline Arena<kMatArena> tmatA;
inline std::array<Arena<kLinkArena>,kSlots> linkA;
inline Arena<kPathArena> pathA;

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
struct State {const C* c{};u32 evHash{};u32 ev{};u32 multCount{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline void PutAt(uint8* p,u32 v) {memcpy(p,&v,4);}
// Mutaciones: 1 mision, 2 accion temporal, 3 estilo, 4 crucero, 5 CTimer::m_snTimeInMilliseconds, 6 velocidad.x, 7 adelante.x, 8 tramo actual, 9 tramo siguiente, 10 carril actual, 11 subtipo, 12 posicion.x,
// 13 destino.x, 14 multiplicador actual, 15 temporizador
inline void ApplyMut(u32 kind,u32 v) {
    switch(kind) {
    case 1: vehA.Put8(0x3BA,v);break;
    case 2: vehA.Put8(0x3BB,v);break;
    case 3: vehA.Put8(0x3B9,v);break;
    case 4: vehA.Put8(0x3D0,v);break;
    case 5: G32(0xB7CB84)=v;break;
    case 6: vehA.Put32(0x44,v);break;
    case 7: matA.Put32(0x10,v);break;
    case 8: vehA.Put16(0x3A4,v);break;
    case 9: vehA.Put16(0x3A6,v);break;
    case 10: vehA.Put8(0x3B7,v);break;
    case 11: vehA.Put32(0x594,v);break;
    case 12: matA.Put32(0x30,v);break;
    case 13: vehA.Put32(0x3EC,v);break;
    case 14: vehA.Put8(0x3B5,v);break;
    case 15: vehA.Put32(0x818,v);break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
inline u32 Cur() {return vehA.Get16(0x3A4);}
inline u32 Nxt() {return vehA.Get16(0x3A6);}
// Eventos: 1 StopCarIfNodesAreInvalid [&vehiculo, tramo actual, siguiente, mision, accion]
// 2 PickNextNodeAccordingStrategy [&vehiculo, actual, siguiente, mision, +0x3B5, +0x3B6, +0x3B7, +0x3B8, estilo, accion, reloj]
// 3 SteerAICarWithPhysicsHeadingForTarget [&vehiculo, objetivo, x, y, &giro, &acelerador, &freno, &freno de mano, mision, actual, siguiente]
// 4 FindPlayerCoors [&resultado, jugador]   5 FindAngleToWeaveThroughTraffic [&vehiculo, objetivo, rumbo al punto, rumbo del vehiculo, escala]   6 FindMaxSteerAngle [&vehiculo]
// 7 FindMaximumSpeedForThisCarInTraffic [&vehiculo, estilo]   8 ShouldCarStopForLight [&vehiculo, argumento]   9 ShouldCarStopForBridge [&vehiculo]   10 CarHasReasonToStop [&vehiculo]
// 11 FindSpeedMultiplier [a, b, c, d]
inline bool __cdecl FakeStop(void* veh) {
    Ev& e=PushEv(1,5);e.v[0]=Tag(veh);e.v[1]=Cur();e.v[2]=Nxt();e.v[3]=vehA.Get8(0x3BA);e.v[4]=vehA.Get8(0x3BB);HashEv(e);
    const bool ret=g.c->stopRet!=0;
    After();
    return ret;
}
inline bool __cdecl FakePick(void* veh) {
    Ev& e=PushEv(2,11);e.v[0]=Tag(veh);e.v[1]=Cur();e.v[2]=Nxt();e.v[3]=vehA.Get8(0x3BA);e.v[4]=vehA.Get8(0x3B5);e.v[5]=vehA.Get8(0x3B6);e.v[6]=vehA.Get8(0x3B7);e.v[7]=vehA.Get8(0x3B8);
    e.v[8]=vehA.Get8(0x3B9);e.v[9]=vehA.Get8(0x3BB);e.v[10]=G32(0xB7CB84);HashEv(e);
    const C& c=*g.c;
    if(c.pickMut) {
        vehA.Put16(0x3A4,c.pickCur);vehA.Put16(0x3A6,c.pickNext);vehA.Put8(0x3B5,c.pickS5);vehA.Put8(0x3B6,c.pickS6);vehA.Put8(0x3B7,c.pickL7);vehA.Put8(0x3B8,c.pickL8);vehA.Put8(0x3BA,c.pickMission);
    }
    const bool ret=c.pickRet!=0;
    After();
    return ret;
}
inline void __cdecl FakeHeading(void* veh,void* tgt,u32 x,u32 y,void* o1,void* o2,void* o3,void* o4) {
    Ev& e=PushEv(3,11);e.v[0]=Tag(veh);e.v[1]=Tag(tgt);e.v[2]=x;e.v[3]=y;e.v[4]=Tag(o1);e.v[5]=Tag(o2);e.v[6]=Tag(o3);e.v[7]=Tag(o4);e.v[8]=vehA.Get8(0x3BA);e.v[9]=Cur();e.v[10]=Nxt();HashEv(e);
    const C& c=*g.c;
    memcpy(o1,&c.hf[0],4);memcpy(o2,&c.hf[1],4);memcpy(o3,&c.hf[2],4);
    *static_cast<uint8*>(o4)=uint8(c.hfHb);
    After();
}
inline float* __cdecl FakePlayer(float* out,int id) {
    Ev& e=PushEv(4,2);e.v[0]=Tag(out);e.v[1]=u32(id);HashEv(e);
    const C& c=*g.c;
    memcpy(out,c.player,12);
    After();
    return out;
}
inline float __cdecl FakeWeave(void* veh,void* tgt,u32 a3,u32 a4,u32 a5) {
    Ev& e=PushEv(5,5);e.v[0]=Tag(veh);e.v[1]=Tag(tgt);e.v[2]=a3;e.v[3]=a4;e.v[4]=a5;HashEv(e);
    const float v=std::bit_cast<float>(g.c->weave);
    After();
    return v;
}
inline float __cdecl FakeMaxSteer(void* veh) {
    Ev& e=PushEv(6,1);e.v[0]=Tag(veh);HashEv(e);
    const float v=std::bit_cast<float>(g.c->maxSteer);
    After();
    return v;
}
inline float __cdecl FakeMaxTraffic(void* veh) {
    Ev& e=PushEv(7,2);e.v[0]=Tag(veh);e.v[1]=vehA.Get8(0x3B9);HashEv(e);
    const float v=std::bit_cast<float>(g.c->maxTraffic);
    After();
    return v;
}
inline bool __cdecl FakeLight(void* veh,u32 arg) {
    Ev& e=PushEv(8,2);e.v[0]=Tag(veh);e.v[1]=arg&0xFFu;HashEv(e);
    const bool ret=g.c->light!=0;
    After();
    return ret;
}
inline bool __cdecl FakeBridge(void* veh) {
    Ev& e=PushEv(9,1);e.v[0]=Tag(veh);HashEv(e);
    const bool ret=g.c->bridge!=0;
    After();
    return ret;
}
inline void __cdecl FakeReason(void* veh) {
    Ev& e=PushEv(10,1);e.v[0]=Tag(veh);HashEv(e);
    After();
}
inline float __cdecl FakeMult(u32 a,u32 b,u32 c2,u32 d) {
    Ev& e=PushEv(11,4);e.v[0]=a;e.v[1]=b;e.v[2]=c2;e.v[3]=d;HashEv(e);
    const float v=std::bit_cast<float>(g.c->mult[g.multCount++&1u]);
    After();
    return v;
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
inline void SetPtr(uintptr_t table,u32 index,uintptr_t value) {G32(table+4*index)=uint32(value);}
inline void CaseFollow(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);outA.Fill(c.seed*17u+3u);tgtA.Fill(c.seed*17u+4u);tmatA.Fill(c.seed*17u+5u);pathA.Fill(c.seed*17u+9u);
        for(unsigned i=0;i<3;++i) {
            matA.Put32(4*i,c.mr[i]);matA.Put32(0x10+4*i,c.mf[i]);matA.Put32(0x20+4*i,c.mu[i]);matA.Put32(0x30+4*i,c.mp[i]);
            vehA.Put32(0x44+4*i,c.speed[i]);vehA.Put32(0x3EC+4*i,c.dest[i]);
            tgtA.Put32(4+4*i,c.tpos[i]);tmatA.Put32(0x30+4*i,c.tmp[i]);
        }
        outA.Put32(0,c.o0[0]);outA.Put32(4,c.o0[1]);outA.Put32(8,c.o0[2]);outA.Put8(0xC,c.o0[3]);
        vehA.Put32(0x14,u32(matA.Obj()));
        tgtA.Put32(0x14,c.tmat?u32(tmatA.Obj()):0u);
        vehA.Put32(0x41C,u32(tgtA.Obj()));
        vehA.Put16(0x3A4,c.cur);vehA.Put16(0x3A6,c.next);
        vehA.Put8(0x3B5,c.s5);vehA.Put8(0x3B6,c.s6);vehA.Put8(0x3B7,c.l7);vehA.Put8(0x3B8,c.l8);
        vehA.Put8(0x3B9,c.style);vehA.Put8(0x3BA,c.mission);vehA.Put8(0x3BB,c.tact);vehA.Put32(0x3BC,c.ttime);
        vehA.Put8(0x3D0,c.cruise);vehA.Put32(0x594,c.subtype);vehA.Put32(0x818,c.timer);
        // mundo de caminos: tres areas cargadas con sus tramos; el resto de la tabla, a cero
        memset(regions[0].Ptr(),0,regions[0].bytes);memset(regions[1].Ptr(),0,regions[1].bytes);
        for(u32 k=0;k<kSlots;++k) {
            linkA[k].Fill(c.seed*17u+20u+k);
            memcpy(linkA[k].m.data()+kGuard,c.links[k],kLinks*kLinkBytes);
            SetPtr(0x96F854,c.areas[k],pathA.Obj());SetPtr(0x96F974,c.areas[k],linkA[k].Obj());
        }
        vehA.Snap();matA.Snap();outA.Snap();tgtA.Snap();tmatA.Snap();pathA.Snap();for(auto& a:linkA)a.Snap();
        TagClear();vehA.Tagged();matA.Tagged();outA.Tagged();tgtA.Tagged();tmatA.Tagged();for(auto& a:linkA)a.Tagged();pathA.Tagged();
        G32(0xB7CB84)=c.now;
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        const u32 args[5]{u32(vehA.Obj()),u32(outA.Obj()),u32(outA.Obj())+4,u32(outA.Obj())+8,u32(outA.Obj())+12};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,5,c.eax,c.ecx,c.edx,c.stack,20);});
        json obs;
        obs["v"]=json::array({vehA.Get8(0x3BA),vehA.Get8(0x3BB),vehA.Get32(0x3BC),vehA.Get16(0x3A4),vehA.Get16(0x3A6),vehA.Get8(0x3B5),vehA.Get8(0x3B6),vehA.Get8(0x3B7),vehA.Get8(0x3B8),vehA.Get8(0x3B9),
            vehA.Get8(0x3D0),vehA.Get32(0x818),vehA.Get32(0x44),vehA.Get32(0x594),vehA.Get32(0x3EC),G32(0xB7CB84)});
        obs["mat"]=WordsOf(matA);
        obs["out"]=json::array({outA.Get32(0),outA.Get32(4),outA.Get32(8),outA.Get8(0xC)});
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        {   // escrituras esperadas: el vehiculo (tramos, multiplicadores, carriles, estilo, mision, accion y su tiempo, crucero, destino.x, velocidad.x, subtipo, temporizador), adelante.x y posicion.x de la matriz y las salidas; nada mas
            obs["changes"]=json::array({vehA.Rest({{0x44,0x48},{0x3A4,0x3A8},{0x3B5,0x3C0},{0x3D0,0x3D1},{0x3EC,0x3F0},{0x594,0x598},{0x818,0x81C}}),matA.Rest({{0x10,0x14},{0x30,0x34}}),outA.Rest({{0,0x10}}),
                tgtA.Rest({}),tmatA.Rest({}),pathA.Rest({}),linkA[0].Rest({}),linkA[1].Rest({}),linkA[2].Rest({})});
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
            for(unsigned c=0;c<kCases;++c)CaseFollow(env,totals,c);
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
// Funcion que corre de verdad: con hook RH en la DLL se apaga (el EXE ejecuta sus bytes, la DLL su C++); sin hook, solo se anota
inline void MakeReal(Env& env,uintptr_t exe,json& hooksFound) {
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name},{"real",true}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr},{"real",true}});
}
template<class T> inline uintptr_t FnAddr(T f) {
    if constexpr (std::is_member_function_pointer_v<T>) return std::bit_cast<uintptr_t>(f); else return reinterpret_cast<uintptr_t>(f);
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"car84h-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84h-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84h");}
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
    const auto findPlayer=static_cast<CVector(*)(int32)>(&FindPlayerCoors);
    env.stubs[0]={
        S(MakeCollab(env,0x422590,FnAddr(&CCarCtrl::StopCarIfNodesAreInvalid),hooksFound),Void(&FakeStop)),
        S(MakeCollab(env,0x432B10,FnAddr(&CCarCtrl::PickNextNodeAccordingStrategy),hooksFound),Void(&FakePick)),
        S(MakeCollab(env,0x433280,FnAddr(&CCarCtrl::SteerAICarWithPhysicsHeadingForTarget),hooksFound),Void(&FakeHeading)),
        S(MakeCollab(env,0x56E010,FnAddr(findPlayer),hooksFound),Void(&FakePlayer)),
        S(MakeCollab(env,0x4325C0,FnAddr(&CCarCtrl::FindAngleToWeaveThroughTraffic),hooksFound),Void(&FakeWeave)),
        S(MakeCollab(env,0x427FE0,FnAddr(&CCarCtrl::FindMaxSteerAngle),hooksFound),Void(&FakeMaxSteer)),
        S(MakeCollab(env,0x434400,FnAddr(&CCarCtrl::FindMaximumSpeedForThisCarInTraffic),hooksFound),Void(&FakeMaxTraffic)),
        S(MakeCollab(env,0x49D610,FnAddr(&CTrafficLights::ShouldCarStopForLight),hooksFound),Void(&FakeLight)),
        S(MakeCollab(env,0x49D420,FnAddr(&CTrafficLights::ShouldCarStopForBridge),hooksFound),Void(&FakeBridge)),
        S(MakeCollab(env,0x41C050,FnAddr(&CCarAI::CarHasReasonToStop),hooksFound),Void(&FakeReason)),
        S(MakeCollab(env,0x4224E0,FnAddr(&Cc63SpeedMultiplierCpp),hooksFound),Void(&FakeMult))};
    // Con el hook apagado y sin falso: arcotangente, desplazamiento de carril y la aritmetica de vectores del EXE (la DLL llama a su propio C++ o a sus ayudantes x87)
    for(const uintptr_t a:{uintptr_t(0x53CC70),uintptr_t(0x44DB00),uintptr_t(0x4082C0),uintptr_t(0x40FE60),uintptr_t(0x40FDB0)})MakeReal(env,a,hooksFound);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CGeneral::GetATanOfXY 0x53CC70 y CCarPathLink::OneWayLaneOffset 0x44DB00 (con el hook apagado: bytes del EXE frente al C++ de la DLL)","resta 0x40FE60, modulo 0x4082C0 y producto escalar 0x40FDB0 del EXE frente a los ayudantes x87 de la DLL","ayudantes x87 de X87Math.cpp (Veh83D*, Veh83C*, CarCtrlSteer63*)","mundo de caminos: arenas del banco enlazadas desde ThePaths, CTimer"})},
            {"virtual_fakes",json::array()},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(const uintptr_t a:{uintptr_t(0x53CC70),uintptr_t(0x44DB00),uintptr_t(0x4082C0),uintptr_t(0x40FE60),uintptr_t(0x40FDB0)})env.watches.emplace_back(a,16);
    // constantes de .rdata que lee el EXE (float): se vigilan sus bytes; y las tablas de salto del conmutador de la mision (0x43579C + 0x4357C0) y de los estilos (0x435804 + 0x43580C, 0x435814 + 0x43581C)
    for(const uintptr_t a:{uintptr_t(0x858B50),uintptr_t(0x858624),uintptr_t(0x858C48),uintptr_t(0x858C50),uintptr_t(0x858C58),uintptr_t(0x859010),uintptr_t(0x858C80),uintptr_t(0x859000),uintptr_t(0x858CB0),
            uintptr_t(0x858A10),uintptr_t(0x858CA0),uintptr_t(0x858C84),uintptr_t(0x859038),uintptr_t(0x858F9C),uintptr_t(0x858C28),uintptr_t(0x858B10),uintptr_t(0x858B3C),uintptr_t(0x858B34),uintptr_t(0x859034),
            uintptr_t(0x858B8C),uintptr_t(0x858CC0),uintptr_t(0x858CBC),uintptr_t(0x858CB8),uintptr_t(0x858CC4)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(0x43579C,36);env.watches.emplace_back(0x4357C0,67);env.watches.emplace_back(0x435804,8);env.watches.emplace_back(0x43580C,7);env.watches.emplace_back(0x435814,8);env.watches.emplace_back(0x43581C,7);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
