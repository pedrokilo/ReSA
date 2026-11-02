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
#include "Entity/Ped/Ped.h"
#include "CarCtrl.h"
#include "CarAI.h"
#include "PathFind.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84I dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::SteerAICarWithPhysics_OnlyMission (0x436A90, hook S, cdecl con cinco argumentos (vehiculo y cuatro punteros a salidas), void). Colaboradores sustituidos por falsos identicos en la
// direccion del EXE y en el cuerpo de la DLL (sus hooks RH quedan apagados durante el banco): las 24 funciones de CCarCtrl a las que despacha (SteerAICarWithPhysicsFollowPath 0x434900, _Racing 0x435830,
// FollowPreRecordedPath 0x432DD0, HeadingForTarget 0x433280, TryingToBlockTarget 0x4335E0 y _Stop 0x428990, TowardsPointInEscort 0x4336D0, Boat* 0x428BE0 / 0x428DE0 / 0x429090, BlockingPlayerForwardAndBack 0x422B20,
// ParkPerpendicular 0x433EA0 y ParkParallel 0x433BA0 y 16 de aire con solo el vehiculo: 0x42A630, 0x423790, 0x42F3C0, 0x429780, 0x423000, 0x4237F0, 0x42A730, 0x42A750, 0x42AAD0, 0x42ACB0, 0x42AD30, 0x42AEB0,
// 0x423880, 0x4238E0, 0x429890, 0x42F370), FindPlayerCoors 0x56E010, FindPlayerVehicle 0x56E0D0, FindPlayerSpeed 0x56E090, FindPlayerEntity 0x56E120, CVehicle::PickRandomPassenger 0x6D2A10, CPed::Say 0x5EFFE0 y
// CVector::Normalise 0x59C910; CMatrix * CVector (0x59C890) lo llama el C++ por direccion, asi que el falso va solo en el EXE. Corren de verdad la resta 0x40FE60, el modulo 0x4082C0 y el producto escalar 0x40FDB0 del EXE
// frente a los ayudantes x87 de la DLL. Los falsos registran sus argumentos y el estado del vehiculo EN ORDEN y escriben los valores que da el caso. Tras el evento numero mutAt el falso muta el estado
// para comprobar que la funcion relee esos campos donde el EXE los relee.

namespace C84ILive {
using namespace C84IFix;
inline constexpr uintptr_t kEntry = 0x436A90;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CVehicle,m_matrix)==0x14&&offsetof(CVehicle,m_autoPilot)==0x390&&offsetof(CPhysical,m_vecMoveSpeed)==0x44&&offsetof(CEntity,m_nRandomSeed)==0x20&&offsetof(CEntity,m_nModelIndex)==0x22,"CVehicle: campos");
static_assert(offsetof(CAutoPilot,m_nCarMission)==0x2A&&offsetof(CAutoPilot,m_nTempAction)==0x2B&&offsetof(CAutoPilot,m_nTempActionTime)==0x2C&&offsetof(CAutoPilot,m_ucCarFollowDist)==0x4E
              &&offsetof(CAutoPilot,m_vecDestinationCoors)==0x5C&&offsetof(CAutoPilot,m_TargetEntity)==0x8C,"CAutoPilot: campos");
static_assert(offsetof(CVehicle,vehicleFlags)==0x428,"CVehicle: banderas");

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
inline Arena<kVehArena> vehA;                       // 0 vehiculo
inline Arena<kMatArena> matA;                       // 1 matriz del vehiculo
inline Arena<kOutArena> outA;                       // 2 salidas
inline Arena<kTgtArena> tgtA;                       // 3 objetivo
inline Arena<kMatArena> tmatA;                      // 4 matriz del objetivo
inline std::array<Arena<kPvArena>,2> pvA;           // 5, 6 vehiculos del jugador
inline std::array<Arena<kMatArena>,2> pvmA;         // 7, 8 sus matrices
inline Arena<kModelArena> modelA;                   // 9 modelos y cajas de colision
inline Arena<kPedArena> pedA;                       // 10 pasajero
inline Arena<kMiscArena> miscA;                     // 11 velocidades del jugador y entidad ficticia

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
struct State {const C* c{};u32 evHash{};u32 ev{};u32 fpv{};u32 fpc{};u32 fps{};u32 mtc{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline void PutAt(uint8* p,u32 v) {memcpy(p,&v,4);}
// Mutaciones: 1 mision, 2 accion temporal, 3 tiempo de la accion, 4 CTimer::m_snTimeInMilliseconds, 5 velocidad.x propia, 6 posicion.x propia, 7 destino.x, 8 distancia de seguimiento, 9 semilla,
// 10 banderas, 11 posicion.x del objetivo, 12 velocidad.x del objetivo, 13 velocidad.x del vehiculo 0 del jugador, 14 adelante.x propia, 15 derecha.x del vehiculo 0 del jugador, 16 adelante.x del vehiculo 1 del jugador
inline void ApplyMut(u32 kind,u32 v) {
    const C& c=*g.c;
    switch(kind) {
    case 1: vehA.Put8(0x3BA,v);break;
    case 2: vehA.Put8(0x3BB,v);break;
    case 3: vehA.Put32(0x3BC,v);break;
    case 4: G32(0xB7CB84)=v;break;
    case 5: vehA.Put32(0x44,v);break;
    case 6: if(c.hasMat)matA.Put32(0x30,v);else vehA.Put32(4,v);break;
    case 7: vehA.Put32(0x3EC,v);break;
    case 8: vehA.Put8(0x3DE,v);break;
    case 9: vehA.Put16(0x20,v);break;
    case 10: vehA.Put32(0x428,v);break;
    case 11: if(c.tmat)tmatA.Put32(0x30,v);else tgtA.Put32(4,v);break;
    case 12: tgtA.Put32(0x44,v);break;
    case 13: pvA[0].Put32(0x44,v);break;
    case 14: matA.Put32(0x10,v);break;
    case 15: pvmA[0].Put32(0,v);break;
    case 16: pvmA[1].Put32(0x10,v);break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
inline u32 Mis() {return vehA.Get8(0x3BA);}
inline u32 Tact() {return vehA.Get8(0x3BB);}
inline u32 Time() {return vehA.Get32(0x3BC);}
// Escribe las salidas (giro, acelerador, freno, byte de freno de mano) que da el caso
inline void PutOut(void* o1,void* o2,void* o3,void* o4) {
    const C& c=*g.c;
    memcpy(o1,&c.ov[0],4);memcpy(o2,&c.ov[1],4);memcpy(o3,&c.ov[2],4);
    *static_cast<uint8*>(o4)=uint8(c.ov[3]&1u);
}
// Eventos: 1 FindPlayerCoors [&out, id]  2 FindPlayerVehicle [id, bool]  3 FindPlayerSpeed [id]  4 FindPlayerEntity [id]  5 PickRandomPassenger [this]  6 CPed::Say [this, contexto, retardo, probabilidad, 3 bool]
// 7 CVector::Normalise [&v, x, y, z]  8 CMatrix * CVector [&out, &matriz, x, y, z]  9 HeadingForTarget [veh, objetivo, x, y, 4 salidas, mision, accion, tiempo]
// 10 TryingToBlockTarget [veh, entidad, x, y, vx, vy, 4 salidas, ...]  11 TryingToBlockTarget_Stop [veh, x, y, vx, vy, 4 salidas, ...]  12 SteerAIBoatWithPhysicsHeadingForTarget [veh, x, y, 3 salidas, ...]
// 13 SteerAICarTowardsPointInEscort [veh1, veh2, a3, a4, 4 salidas, ...]  14..21 las de cinco punteros [veh, 4 salidas, mision, accion, tiempo]  30..45 las de aire [veh, mision, accion, tiempo]
inline float* __cdecl FakeCoors(float* out,int id) {
    Ev& e=PushEv(1,2);e.v[0]=Tag(out);e.v[1]=u32(id);HashEv(e);
    memcpy(out,g.c->pc[g.fpc++&1u],12);
    After();
    return out;
}
inline void* __cdecl FakeVehicle(int id,u32 remote) {
    Ev& e=PushEv(2,2);e.v[0]=u32(id);e.v[1]=remote&0xFFu;HashEv(e);
    void* ret=g.c->pvNull?nullptr:reinterpret_cast<void*>(pvA[g.fpv&1u].Obj());
    ++g.fpv;
    After();
    return ret;
}
inline void* __cdecl FakeSpeed(int id) {
    Ev& e=PushEv(3,1);e.v[0]=u32(id);HashEv(e);
    void* ret=reinterpret_cast<void*>(miscA.Obj(kGuard+((g.fps++&1u)?kSpeedBuf1:kSpeedBuf0)));
    After();
    return ret;
}
inline void* __cdecl FakeEntity(int id) {
    Ev& e=PushEv(4,1);e.v[0]=u32(id);HashEv(e);
    void* ret=reinterpret_cast<void*>(miscA.Obj(kGuard+kEntityDummy));
    After();
    return ret;
}
inline void* __fastcall FakePassenger(void* self,void*) {
    Ev& e=PushEv(5,1);e.v[0]=Tag(self);HashEv(e);
    void* ret=g.c->passenger?reinterpret_cast<void*>(pedA.Obj()):nullptr;
    After();
    return ret;
}
inline short __fastcall FakeSay(void* self,void*,u32 ctx,u32 delay,u32 prob,u32 b1,u32 b2,u32 b3) {
    Ev& e=PushEv(6,7);e.v[0]=Tag(self);e.v[1]=ctx;e.v[2]=delay;e.v[3]=prob;e.v[4]=b1&0xFFu;e.v[5]=b2&0xFFu;e.v[6]=b3&0xFFu;HashEv(e);
    After();
    return 0;
}
inline void __fastcall FakeNormalise(void* v,void*) {
    Ev& e=PushEv(7,4);e.v[0]=Tag(v);memcpy(&e.v[1],v,12);HashEv(e);
    memcpy(v,g.c->nv,12);
    After();
}
inline float* __cdecl FakeMatTimes(float* out,void* m,float* p) {
    Ev& e=PushEv(8,5);e.v[0]=Tag(out);e.v[1]=Tag(m);memcpy(&e.v[2],p,12);HashEv(e);
    memcpy(out,g.c->mt[g.mtc++&1u],12);
    After();
    return out;
}
inline void __cdecl FakeHeading(void* veh,void* tgt,u32 x,u32 y,void* o1,void* o2,void* o3,void* o4) {
    Ev& e=PushEv(9,12);e.v[0]=Tag(veh);e.v[1]=Tag(tgt);e.v[2]=x;e.v[3]=y;e.v[4]=Tag(o1);e.v[5]=Tag(o2);e.v[6]=Tag(o3);e.v[7]=Tag(o4);e.v[8]=Mis();e.v[9]=Tact();e.v[10]=Time();e.v[11]=G32(0xB7CB84);HashEv(e);
    PutOut(o1,o2,o3,o4);
    After();
}
inline void __cdecl FakeBlock(void* veh,void* ent,u32 x,u32 y,u32 vx,u32 vy,void* o1,void* o2,void* o3,void* o4) {
    Ev& e=PushEv(10,14);e.v[0]=Tag(veh);e.v[1]=Tag(ent);e.v[2]=x;e.v[3]=y;e.v[4]=vx;e.v[5]=vy;e.v[6]=Tag(o1);e.v[7]=Tag(o2);e.v[8]=Tag(o3);e.v[9]=Tag(o4);e.v[10]=Mis();e.v[11]=Tact();e.v[12]=Time();e.v[13]=G32(0xB7CB84);HashEv(e);
    PutOut(o1,o2,o3,o4);
    After();
}
inline void __cdecl FakeBlockStop(void* veh,u32 x,u32 y,u32 a4,u32 a5,void* o1,void* o2,void* o3,void* o4) {
    Ev& e=PushEv(11,13);e.v[0]=Tag(veh);e.v[1]=x;e.v[2]=y;e.v[3]=a4;e.v[4]=a5;e.v[5]=Tag(o1);e.v[6]=Tag(o2);e.v[7]=Tag(o3);e.v[8]=Tag(o4);e.v[9]=Mis();e.v[10]=Tact();e.v[11]=Time();e.v[12]=G32(0xB7CB84);HashEv(e);
    PutOut(o1,o2,o3,o4);
    After();
}
inline void __cdecl FakeBoat(void* veh,u32 x,u32 y,void* o1,void* o2,void* o3) {
    Ev& e=PushEv(12,10);e.v[0]=Tag(veh);e.v[1]=x;e.v[2]=y;e.v[3]=Tag(o1);e.v[4]=Tag(o2);e.v[5]=Tag(o3);e.v[6]=Mis();e.v[7]=Tact();e.v[8]=Time();e.v[9]=G32(0xB7CB84);HashEv(e);
    memcpy(o1,&g.c->ov[0],4);memcpy(o2,&g.c->ov[1],4);memcpy(o3,&g.c->ov[2],4);
    After();
}
inline void __cdecl FakeEscort(void* veh1,void* veh2,u32 a3,u32 a4,void* o1,void* o2,void* o3,void* o4) {
    Ev& e=PushEv(13,12);e.v[0]=Tag(veh1);e.v[1]=Tag(veh2);e.v[2]=a3;e.v[3]=a4;e.v[4]=Tag(o1);e.v[5]=Tag(o2);e.v[6]=Tag(o3);e.v[7]=Tag(o4);e.v[8]=Mis();e.v[9]=Tact();e.v[10]=Time();e.v[11]=G32(0xB7CB84);HashEv(e);
    PutOut(o1,o2,o3,o4);
    After();
}
template<u32 K> void __cdecl Fake5(void* veh,void* o1,void* o2,void* o3,void* o4) {
    Ev& e=PushEv(K,9);e.v[0]=Tag(veh);e.v[1]=Tag(o1);e.v[2]=Tag(o2);e.v[3]=Tag(o3);e.v[4]=Tag(o4);e.v[5]=Mis();e.v[6]=Tact();e.v[7]=Time();e.v[8]=G32(0xB7CB84);HashEv(e);
    PutOut(o1,o2,o3,o4);
    After();
}
template<u32 K> void __cdecl Fake1(void* veh) {
    Ev& e=PushEv(30+K,5);e.v[0]=Tag(veh);e.v[1]=Mis();e.v[2]=Tact();e.v[3]=Time();e.v[4]=G32(0xB7CB84);HashEv(e);
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
inline void SetPtr(uintptr_t table,u32 index,uintptr_t value) {G32(table+4*index)=uint32(value);}
inline void PutMat(Arena<kMatArena>& a,const u32* r,const u32* f,const u32* u,const u32* p) {
    for(unsigned i=0;i<3;++i) {a.Put32(4*i,r[i]);a.Put32(0x10+4*i,f[i]);a.Put32(0x20+4*i,u[i]);a.Put32(0x30+4*i,p[i]);}
}
inline void CaseMission(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);outA.Fill(c.seed*17u+3u);tgtA.Fill(c.seed*17u+4u);tmatA.Fill(c.seed*17u+5u);
        pvA[0].Fill(c.seed*17u+6u);pvA[1].Fill(c.seed*17u+7u);pvmA[0].Fill(c.seed*17u+8u);pvmA[1].Fill(c.seed*17u+9u);modelA.Fill(c.seed*17u+10u);pedA.Fill(c.seed*17u+11u);miscA.Fill(c.seed*17u+12u);
        // vehiculo
        for(unsigned i=0;i<3;++i) {vehA.Put32(4+4*i,c.pp[i]);vehA.Put32(0x44+4*i,c.speed[i]);vehA.Put32(0x3EC+4*i,c.dest[i]);}
        PutMat(matA,c.mr,c.mf,c.mu,c.mp);
        vehA.Put32(0x14,c.hasMat?u32(matA.Obj()):0u);
        vehA.Put16(0x20,c.rseed);vehA.Put16(0x22,c.oi);
        vehA.Put8(0x3BA,c.mission);vehA.Put8(0x3BB,c.tact);vehA.Put32(0x3BC,c.ttime);vehA.Put8(0x3DE,c.follow);
        vehA.Put32(0x41C,c.tgtNull?0u:u32(tgtA.Obj()));
        vehA.Put32(0x428,c.flags);
        // objetivo
        for(unsigned i=0;i<3;++i) {tgtA.Put32(4+4*i,c.tpp[i]);tgtA.Put32(0x44+4*i,c.tspeed[i]);tmatA.Put32(0x30+4*i,c.tmp[i]);}
        tgtA.Put32(0x14,c.tmat?u32(tmatA.Obj()):0u);tgtA.Put16(0x22,c.ti);
        // vehiculos del jugador
        for(unsigned j=0;j<2;++j) {
            PutMat(pvmA[j],c.pr[j],c.pf[j],c.pu[j],c.pp2[j]);
            pvA[j].Put32(0x14,u32(pvmA[j].Obj()));pvA[j].Put16(0x22,c.pi[j]);
            for(unsigned i=0;i<3;++i)pvA[j].Put32(0x44+4*i,c.pspeed[j][i]);
        }
        // modelos 1..6 y sus cajas de colision; velocidades devueltas por FindPlayerSpeed
        for(unsigned i=0;i<kModels;++i) {
            modelA.Put32(ModelInfoAt(i)+0x14,u32(modelA.Obj(kGuard+ColModelAt(i))));
            modelA.Put32(ColModelAt(i)+0xC,c.cmx[i]);modelA.Put32(ColModelAt(i)+0x10,c.cmy[i]);
        }
        for(unsigned k=0;k<2;++k)for(unsigned i=0;i<3;++i)miscA.Put32((k?kSpeedBuf1:kSpeedBuf0)+4*i,c.psp[k][i]);
        outA.Put32(0,c.o0[0]);outA.Put32(4,c.o0[1]);outA.Put32(8,c.o0[2]);outA.Put8(0xC,c.o0[3]);
        for(unsigned i=0;i<kModels;++i)SetPtr(0xA9B0C8,i+1,modelA.Obj(kGuard+ModelInfoAt(i)));
        vehA.Snap();matA.Snap();outA.Snap();tgtA.Snap();tmatA.Snap();pvA[0].Snap();pvA[1].Snap();pvmA[0].Snap();pvmA[1].Snap();modelA.Snap();pedA.Snap();miscA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();outA.Tagged();tgtA.Tagged();tmatA.Tagged();pvA[0].Tagged();pvA[1].Tagged();pvmA[0].Tagged();pvmA[1].Tagged();modelA.Tagged();pedA.Tagged();miscA.Tagged();
        G32(0xB7CB84)=c.now;
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        const u32 args[5]{u32(vehA.Obj()),u32(outA.Obj()),u32(outA.Obj())+4,u32(outA.Obj())+8,u32(outA.Obj())+12};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,5,c.eax,c.ecx,c.edx,c.stack,20);});
        json obs;
        obs["v"]=json::array({vehA.Get8(0x3BA),vehA.Get8(0x3BB),vehA.Get32(0x3BC),G32(0xB7CB84),vehA.Get32(0x44),vehA.Get16(0x20),vehA.Get32(0x428),vehA.Get8(0x3DE),vehA.Get32(0x3EC),vehA.Get32(4)});
        obs["mat"]=WordsOf(matA);
        obs["pvmat"]=json::array({WordsOf(pvmA[0]),WordsOf(pvmA[1])});
        obs["tgt"]=json::array({tgtA.Get32(4),tgtA.Get32(0x44),tmatA.Get32(0x30)});
        obs["pv"]=json::array({pvA[0].Get32(0x44),pvA[1].Get32(0x44)});
        obs["out"]=json::array({outA.Get32(0),outA.Get32(4),outA.Get32(8),outA.Get8(0xC)});
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        {   // escrituras esperadas: el vehiculo (mision, accion y su tiempo, velocidad.x, semilla, banderas, distancia de seguimiento, destino.x, posicion.x), la matriz (adelante.x, posicion.x), el objetivo, los
            // vehiculos del jugador y las salidas; nada mas
            obs["changes"]=json::array({vehA.Rest({{0x4,0x8},{0x20,0x22},{0x44,0x48},{0x3BA,0x3C0},{0x3DE,0x3DF},{0x3EC,0x3F0},{0x428,0x42C}}),matA.Rest({{0x10,0x14},{0x30,0x34}}),outA.Rest({{0,0x10}}),
                tgtA.Rest({{4,8},{0x44,0x48}}),tmatA.Rest({{0x30,0x34}}),pvA[0].Rest({{0x44,0x48}}),pvA[1].Rest({}),pvmA[0].Rest({{0,4}}),pvmA[1].Rest({{0x10,0x14}}),modelA.Rest({}),pedA.Rest({}),miscA.Rest({})});
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
            for(unsigned c=0;c<kCases;++c)CaseMission(env,totals,c);
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
// (llamadas directamente por la DLL) llevan el mismo falso. dllFunction == 0: solo en el EXE (la DLL lo llama por la direccion del EXE).
inline Pair MakeCollab(Env& env,uintptr_t exe,uintptr_t dllFunction,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr}});
    p.dll=dllFunction?Body(dllFunction):0;
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
    if(!std::filesystem::exists(dir/"car84i-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84i-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84i");}
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
    using SteerFn=void(*)(CVehicle*,float*,float*,float*,bool*);
    using AirFn=void(*)(CAutomobile*);
    struct Five {uintptr_t exe;SteerFn fn;Void fake;};
    struct One {uintptr_t exe;AirFn fn;Void fake;};
    const Five fives[]{
        {0x434900,&CCarCtrl::SteerAICarWithPhysicsFollowPath,Void(&Fake5<14>)},{0x435830,&CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing,Void(&Fake5<15>)},
        {0x432DD0,&CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath,Void(&Fake5<16>)},{0x428DE0,&CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer,Void(&Fake5<17>)},
        {0x429090,&CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer,Void(&Fake5<18>)},{0x422B20,&CCarCtrl::SteerAICarBlockingPlayerForwardAndBack,Void(&Fake5<19>)},
        {0x433EA0,&CCarCtrl::SteerAICarParkPerpendicular,Void(&Fake5<20>)},{0x433BA0,&CCarCtrl::SteerAICarParkParallel,Void(&Fake5<21>)}};
    const One ones[]{
        {0x42A630,&CCarCtrl::SteerAIHeliTowardsTargetCoors,Void(&Fake1<0>)},{0x423790,&CCarCtrl::SteerAIPlaneTowardsTargetCoors,Void(&Fake1<1>)},{0x42F3C0,&CCarCtrl::GetAIHeliToAttackPlayer,Void(&Fake1<2>)},
        {0x429780,&CCarCtrl::GetAIPlaneToAttackPlayer,Void(&Fake1<3>)},{0x4237F0,&CCarCtrl::SteerAIPlaneToFollowEntity,Void(&Fake1<5>)},{0x42A730,&CCarCtrl::GetAIHeliToFlyInDirection,Void(&Fake1<6>)},
        {0x42A750,&CCarCtrl::SteerAIHeliToFollowEntity,Void(&Fake1<7>)},{0x42AAD0,&CCarCtrl::SteerAIHeliAsPoliceHeli,Void(&Fake1<8>)},{0x42ACB0,&CCarCtrl::SteerAIHeliFlyingAwayFromPlayer,Void(&Fake1<9>)},
        {0x42AD30,&CCarCtrl::SteerAIHeliToLand,Void(&Fake1<10>)},{0x42AEB0,&CCarCtrl::SteerAIHeliToKeepEntityInView,Void(&Fake1<11>)},{0x423880,&CCarCtrl::SteerAIPlaneToCrashAndBurn,Void(&Fake1<12>)},
        {0x4238E0,&CCarCtrl::SteerAIHeliToCrashAndBurn,Void(&Fake1<13>)},{0x429890,&CCarCtrl::GetAIPlaneToDoDogFight,Void(&Fake1<14>)},{0x42F370,&CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer,Void(&Fake1<15>)}};
    const auto findCoors=static_cast<CVector(*)(int32)>(&FindPlayerCoors);
    const auto findVehicle=static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle);
    const auto findSpeed=static_cast<CVector&(*)(int32)>(&FindPlayerSpeed);
    const auto findEntity=static_cast<CEntity*(*)(int32)>(&FindPlayerEntity);
    const auto flyPlane=static_cast<void(*)(CPlane*)>(&CCarCtrl::FlyAIPlaneInCertainDirection);
    std::vector<StubSpec> specs;
    for(const auto& f:fives)specs.push_back(S(MakeCollab(env,f.exe,FnAddr(f.fn),hooksFound),f.fake));
    for(const auto& o:ones)specs.push_back(S(MakeCollab(env,o.exe,FnAddr(o.fn),hooksFound),o.fake));
    specs.push_back(S(MakeCollab(env,0x423000,FnAddr(flyPlane),hooksFound),Void(&Fake1<4>)));
    specs.push_back(S(MakeCollab(env,0x433280,FnAddr(&CCarCtrl::SteerAICarWithPhysicsHeadingForTarget),hooksFound),Void(&FakeHeading)));
    specs.push_back(S(MakeCollab(env,0x4335E0,FnAddr(&CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget),hooksFound),Void(&FakeBlock)));
    specs.push_back(S(MakeCollab(env,0x428990,FnAddr(&CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop),hooksFound),Void(&FakeBlockStop)));
    specs.push_back(S(MakeCollab(env,0x428BE0,FnAddr(&CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget),hooksFound),Void(&FakeBoat)));
    specs.push_back(S(MakeCollab(env,0x4336D0,FnAddr(&CCarCtrl::SteerAICarTowardsPointInEscort),hooksFound),Void(&FakeEscort)));
    specs.push_back(S(MakeCollab(env,0x56E010,FnAddr(findCoors),hooksFound),Void(&FakeCoors)));
    specs.push_back(S(MakeCollab(env,0x56E0D0,FnAddr(findVehicle),hooksFound),Void(&FakeVehicle)));
    specs.push_back(S(MakeCollab(env,0x56E090,FnAddr(findSpeed),hooksFound),Void(&FakeSpeed)));
    specs.push_back(S(MakeCollab(env,0x56E120,FnAddr(findEntity),hooksFound),Void(&FakeEntity)));
    specs.push_back(S(MakeCollab(env,0x6D2A10,FnAddr(&CVehicle::PickRandomPassenger),hooksFound),Void(&FakePassenger)));
    specs.push_back(S(MakeCollab(env,0x5EFFE0,FnAddr(&CPed::Say),hooksFound),Void(&FakeSay)));
    specs.push_back(S(MakeCollab(env,0x59C910,FnAddr(&CVector::Normalise),hooksFound),Void(&FakeNormalise)));
    specs.push_back(S(MakeCollab(env,0x59C890,0,hooksFound),Void(&FakeMatTimes)));
    env.stubs[0]=specs;
    // Con el hook apagado y sin falso: la aritmetica de vectores del EXE (la DLL llama a sus ayudantes x87)
    for(const uintptr_t a:{uintptr_t(0x40FE60),uintptr_t(0x4082C0),uintptr_t(0x40FDB0)})MakeReal(env,a,hooksFound);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"resta 0x40FE60, modulo 0x4082C0 y producto escalar 0x40FDB0 del EXE frente a los ayudantes x87 de la DLL (con el hook apagado)","ayudantes x87 de X87Math.cpp (Veh83D*, Veh83C*)","modelos 1..6 y sus cajas de colision (arenas del banco), CTimer"})},
            {"virtual_fakes",json::array()},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(const uintptr_t a:{uintptr_t(0x40FE60),uintptr_t(0x4082C0),uintptr_t(0x40FDB0)})env.watches.emplace_back(a,16);
    // constantes de .rdata que lee el EXE (float): se vigilan sus bytes; y la tabla de saltos de la mision (0x437B24, 44 destinos) y su tabla de indices (0x437BD4, 0x45 bytes)
    for(const uintptr_t a:{uintptr_t(0x858B8C),uintptr_t(0x858CC4),uintptr_t(0x858CCC),uintptr_t(0x858CA0),uintptr_t(0x859050),uintptr_t(0x85904C),uintptr_t(0x858EE8),uintptr_t(0x85862C),uintptr_t(0x858B34),
            uintptr_t(0x858C80),uintptr_t(0x858EF4),uintptr_t(0x858624),uintptr_t(0x858C28),uintptr_t(0x858F48),uintptr_t(0x858B50)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(0x437B24,44*4);env.watches.emplace_back(0x437BD4,0x45);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
