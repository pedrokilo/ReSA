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
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Physical.h"
#include "Core/Vector.h"
#include "Core/Matrix.h"
#include "Pad.h"
#include "Camera.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "CullZones.h"
#include "Ropes.h"
#include "Weapon.h"
#include "Wanted.h"
#include "InterestingEvents.h"
#include "Audio/AudioEngine.h"
#include "Fx/FxSystem.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Heli83F dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::ProcessControl (0x6C7050, hook V: RH_ScopedVMTInstall; incluye el salto directo en la entrada del EXE). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el
// cuerpo de la DLL (sus hooks RH quedan apagados durante el banco):
//   CAutomobile::ProcessControl 0x6B1880 (virtual: el cuerpo de la DLL sale de la entrada de su vtabla), FxSystem_c::Kill 0x4AA3F0, CPad::HornJustDown 0x53FF30, CCullZones::PlayerNoRain 0x72DDC0 y NoPolice 0x72DD50,
//   FindPlayerVehicle 0x56E0D0, FindPlayerEntity 0x56E120, FindPlayerPed 0x56E210 y FindPlayerWanted 0x56E230, CAudioEngine::SayPedless 0x5073C0 y ReportWeaponEvent 0x506F40, CHeli::FindSwatPositionRelativeToHeli 0x6C4760
//   (el EXE no toca EDX: falso desnudo que lo conserva), operator*(CMatrix, CVector) 0x59C890, Multiply3x3 0x59C790 (en la DLL, AutoPreMultiply3x3Cpp: solo parche del cuerpo, otra firma), CWorld::GetIsLineOfSightClear 0x56A490,
//   FireOneInstantHitRound 0x73AF00, CRopes::RegisterRope 0x556B40 y SetSpeedOfTopNode 0x555DF0, CVehicle::UpdateWinch 0x6D3B80 y ProcessWeapons 0x6E3950, CInterestingEvents::Add 0x602590, CHeli::SendDownSwat 0x6C69C0
//   y CVector::Normalise 0x59C910 (thiscall, in situ).
//   Corren de verdad en ambos lados: CPad::GetPad (0x53FB70), rand() del CRT del EXE (0x821B1E, con 0x41BD90 del EXE: su hook se apaga), GetUp/GetForward de la matriz (0x50E420/0x41CCB0), CTimer, la matriz y los campos del
//   helicoptero y de las entidades (arenas). Los falsos registran sus argumentos EN ORDEN (punteros de arena por etiqueta, locales de pila por su marca); los de salida escriben los valores que da el caso.

namespace H83FLive {
using namespace H83FFix;
inline constexpr uintptr_t kEntry = 0x6C7050;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

static_assert(offsetof(CHeli,m_nHeliFlags)==0x988&&offsetof(CHeli,m_nNumSwatOccupants)==0x9B9&&offsetof(CHeli,m_aSwatState)==0x9BA&&offsetof(CHeli,m_fSearchLightIntensity)==0xA00&&offsetof(CHeli,m_vecSearchLightTarget)==0x9F4);
static_assert(offsetof(CVehicle,m_pDustParticle)==0x580,"dust");
static_assert(offsetof(CVehicle,m_pDriver)==0x460,"driver");
static_assert(offsetof(CVehicle,m_nVehicleSubType)==0x594,"subtype");
static_assert(offsetof(CAutomobile,m_heliDustFxTimeConst)==0x984);
static_assert(offsetof(CPlaceable,m_matrix)==0x14&&offsetof(CPlaceable,m_placement)==0x4&&offsetof(CPhysical,m_vecMoveSpeed)==0x44);
static_assert(offsetof(CWanted,m_Flags)==0x1E&&offsetof(CWanted,m_WantedLevel)==0x2C);
static_assert(sizeof(CHeli)<=kVehWin);

// Disposicion de CMatrix (miembros privados: se mide con los accesores): derecha +0, adelante +0x10, arriba +0x20, posicion +0x30
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
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;        // 0 helicoptero
inline Arena<kMatArena> matA;        // 1 su matriz
inline Arena<kDrvArena> drvA;        // 2 conductor (+0x598 tipo de ped)
inline Arena<kPvArena> pvA;          // 3 vehiculo del jugador (FindPlayerVehicle; +0x594 subtipo) y posible objetivo
inline Arena<kEntArena> entA;        // 4 entidad (FindPlayerEntity / piloto automatico)
inline Arena<kMatArena> tmatA;       // 5 matriz del objetivo
inline Arena<kPlayerArena> playerA;  // 6 CPlayerPed de FindPlayerPed (+0x480 datos del jugador)
inline Arena<kDataArena> dataA;      // 7 CPlayerPedData (+0 CWanted*)
inline Arena<kWantedArena> wantedA;  // 8 CWanted (+0x1E banderas, +0x2C nivel)
inline Arena<kFxArena> fxA;          // 9 FxSystem_c de +0x580 (m_pDustParticle; solo se etiqueta)

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
struct State {const C* c{};u32 evHash{};u32 unexpected{};u32 swat{};u32 mp{};u32 m3{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline u32 Raw(const void* p,unsigned i) {return BitsOf(static_cast<const u8*>(p)+4*i);}
inline Ev& Begin(u32 kind,u32 len) {return PushEv(kind,len);}
// Eventos: 1 CVector::Normalise [&v, x, y, z], 2 FindPlayerVehicle [id, bool], 3 FindPlayerEntity [id], 4 FindPlayerPed [id], 5 FindPlayerWanted [id], 6 CAutomobile::ProcessControl [this], 7 FxSystem_c::Kill [fx],
// 8 CPad::HornJustDown [pad], 9 PlayerNoRain [], 10 NoPolice [], 11 SayPedless [motor, 8 argumentos], 12 FindSwatPositionRelativeToHeli [this, out, n], 13 operator*(matriz, vector) [out, m, v, v(3)],
// 14 Multiply3x3 [out, m, v, v(3)], 15 RegisterRope [id, tipo, x, y, z, expira, segmentos, byte, holder, tiempo], 16 SetSpeedOfTopNode [id, x, y, z], 17 GetIsLineOfSightClear [origen, destino, origen(3), destino(3), 7 booleanos],
// 18 FireOneInstantHitRound [inicio, fin, intensidad, inicio(3), fin(3)], 19 ReportWeaponEvent [motor, evento, arma, entidad], 20 UpdateWinch [this], 21 ProcessWeapons [this], 22 SendDownSwat [this],
// 23 CInterestingEvents::Add [objeto, tipo, entidad], 99 virtual inesperado
inline void __fastcall FakeNormalise(void* v,void*) {
    Ev& e=Begin(1,4);e.v[0]=Tag(v);for(unsigned i=0;i<3;++i)e.v[1+i]=Raw(v,i);HashEv(e);
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(v)+4*i,&g.c->nrm[i],4);
}
inline void* EntityOf(u32 kind) {return kind==1?reinterpret_cast<void*>(entA.Obj()):kind==2?reinterpret_cast<void*>(playerA.Obj()):kind==3?reinterpret_cast<void*>(pvA.Obj()):nullptr;}
inline void* __cdecl FakeFindVeh(u32 id,u32 b) {Ev& e=Begin(2,2);e.v[0]=id;e.v[1]=b;HashEv(e);return g.c->pvKind?reinterpret_cast<void*>(pvA.Obj()):nullptr;}
inline void* __cdecl FakeFindEntity(u32 id) {Ev& e=Begin(3,1);e.v[0]=id;HashEv(e);return EntityOf(g.c->peKind);}
inline void* __cdecl FakeFindPed(u32 id) {Ev& e=Begin(4,1);e.v[0]=id;HashEv(e);return reinterpret_cast<void*>(playerA.Obj());}
inline void* __cdecl FakeFindWanted(u32 id) {Ev& e=Begin(5,1);e.v[0]=id;HashEv(e);return reinterpret_cast<void*>(wantedA.Obj());}
inline void __fastcall FakeAutoControl(void* self,void*) {Ev& e=Begin(6,1);e.v[0]=Tag(self);HashEv(e);}
inline void __fastcall FakeKill(void* self,void*) {Ev& e=Begin(7,1);e.v[0]=Tag(self);HashEv(e);}
inline u32 __fastcall FakeHorn(void* self,void*) {Ev& e=Begin(8,1);e.v[0]=Tag(self);HashEv(e);return g.c->horn;}
inline u32 __cdecl FakeNoRain() {Ev& e=Begin(9,0);HashEv(e);return g.c->norain;}
inline u32 __cdecl FakeNoPolice() {Ev& e=Begin(10,0);HashEv(e);return g.c->nopolice;}
inline void __fastcall FakeSay(void* self,void*,u32 a0,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7) {
    Ev& e=Begin(11,9);e.v[0]=Tag(self);e.v[1]=a0;e.v[2]=a1;e.v[3]=Tag(uintptr_t(a2));e.v[4]=a3;e.v[5]=a4;e.v[6]=a5;e.v[7]=a6;e.v[8]=a7;HashEv(e);
}
inline void* __fastcall FakeFindSwatImpl(void* self,void*,void* out,u32 n) {
    Ev& e=Begin(12,3);e.v[0]=Tag(self);e.v[1]=Tag(out);e.v[2]=n;HashEv(e);
    const unsigned k=g.swat<7?g.swat:7;++g.swat;
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(out)+4*i,&g.c->sw[k][i],4);
    return out;
}
// El EXE no toca EDX en 0x6C4760 (el llamador de 0x6C7844 lo conserva): el falso lo guarda y lo devuelve
__declspec(naked) inline void FakeFindSwat() {
    __asm {
        push edx
        push dword ptr [esp+0Ch]
        push dword ptr [esp+0Ch]
        call FakeFindSwatImpl
        pop edx
        ret 8
    }
}
inline void* __cdecl FakeMatPoint(void* out,void* m,void* v) {
    Ev& e=Begin(13,6);e.v[0]=Tag(out);e.v[1]=Tag(m);e.v[2]=Tag(v);for(unsigned i=0;i<3;++i)e.v[3+i]=Raw(v,i);HashEv(e);
    const unsigned k=g.mp<4?g.mp:4;++g.mp;
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(out)+4*i,&g.c->mp[k][i],4);
    return out;
}
inline void Mul3x3Common(void* out,void* m,void* v) {
    Ev& e=Begin(14,6);e.v[0]=Tag(out);e.v[1]=Tag(m);e.v[2]=Tag(v);for(unsigned i=0;i<3;++i)e.v[3+i]=Raw(v,i);HashEv(e);
    const unsigned k=g.m3<3?g.m3:3;++g.m3;
    for(unsigned i=0;i<3;++i)memcpy(static_cast<u8*>(out)+4*i,&g.c->m3o[k][i],4);
}
inline void* __cdecl FakeMul3x3Exe(void* out,void* m,void* v) {Mul3x3Common(out,m,v);return out;}            // EXE: (salida, matriz, vector)
inline void __cdecl FakeMul3x3Dll(const float* m,const float* v,float* out) {Mul3x3Common(out,const_cast<float*>(m),const_cast<float*>(v));}   // AutoPreMultiply3x3Cpp: (matriz, vector, salida)
inline u32 __cdecl FakeRope(u32 id,u32 type,u32 x,u32 y,u32 z,u32 exp,u32 seg,u32 b,u32 holder,u32 time) {
    Ev& e=Begin(15,10);e.v[0]=Tag(uintptr_t(id));e.v[1]=type;e.v[2]=x;e.v[3]=y;e.v[4]=z;e.v[5]=exp;e.v[6]=seg;e.v[7]=b;e.v[8]=Tag(uintptr_t(holder));e.v[9]=time;HashEv(e);
    return 1;
}
inline void __cdecl FakeRopeSpeed(u32 id,u32 x,u32 y,u32 z) {Ev& e=Begin(16,4);e.v[0]=Tag(uintptr_t(id));e.v[1]=x;e.v[2]=y;e.v[3]=z;HashEv(e);}
inline u32 __cdecl FakeLos(void* origin,void* target,u32 a2,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8) {
    Ev& e=Begin(17,15);e.v[0]=Tag(origin);e.v[1]=Tag(target);for(unsigned i=0;i<3;++i) {e.v[2+i]=Raw(origin,i);e.v[5+i]=Raw(target,i);}
    e.v[8]=a2;e.v[9]=a3;e.v[10]=a4;e.v[11]=a5;e.v[12]=a6;e.v[13]=a7;e.v[14]=a8;HashEv(e);
    return g.c->los;
}
inline void __cdecl FakeFire(void* start,void* end,u32 intensity) {
    Ev& e=Begin(18,9);e.v[0]=Tag(start);e.v[1]=Tag(end);e.v[2]=intensity;for(unsigned i=0;i<3;++i) {e.v[3+i]=Raw(start,i);e.v[6+i]=Raw(end,i);}HashEv(e);
}
inline void __fastcall FakeWeaponEvent(void* self,void*,u32 ev,u32 weapon,void* entity) {Ev& e=Begin(19,4);e.v[0]=Tag(self);e.v[1]=ev;e.v[2]=weapon;e.v[3]=Tag(entity);HashEv(e);}
inline void __fastcall FakeWinch(void* self,void*) {Ev& e=Begin(20,1);e.v[0]=Tag(self);HashEv(e);}
inline void __fastcall FakeWeapons(void* self,void*) {Ev& e=Begin(21,1);e.v[0]=Tag(self);HashEv(e);}
inline u32 __fastcall FakeSendDown(void* self,void*) {Ev& e=Begin(22,1);e.v[0]=Tag(self);HashEv(e);return 0;}
inline void __fastcall FakeInteresting(void* self,void*,u32 type,void* entity) {Ev& e=Begin(23,3);e.v[0]=Tag(self);e.v[1]=type;e.v[2]=Tag(entity);HashEv(e);}
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {++g.unexpected;Ev& e=Begin(99,1);e.v[0]=Tag(self);HashEv(e);return 0;}
// vtabla falsa del helicoptero: ninguna entrada es valida (la funcion no llama a virtuales)
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
inline uintptr_t PadAddr(unsigned index) {return kPadsBase+kPadSize*index;}

// contenido del objetivo (posicion, puntero de matriz y velocidad) que leen la funcion y la comparacion con el jugador
template<size_t N> inline void PutTarget(Arena<N>& a,const C& c) {
    for(unsigned i=0;i<3;++i) {a.Put32(4+4*i,c.tpos[i]);a.Put32(0x44+4*i,c.tspd[i]);}
    a.Put32(0x14,c.tmatNull?0u:u32(tmatA.Obj()));
}

inline void CaseHeli(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*17u+1u);matA.Fill(c.seed*17u+2u);drvA.Fill(c.seed*17u+3u);pvA.Fill(c.seed*17u+4u);entA.Fill(c.seed*17u+5u);tmatA.Fill(c.seed*17u+6u);
        playerA.Fill(c.seed*17u+7u);dataA.Fill(c.seed*17u+8u);wantedA.Fill(c.seed*17u+9u);fxA.Fill(c.seed*17u+10u);
        // matriz del helicoptero: derecha (+0), adelante (+0x10), arriba (+0x20), posicion (+0x30)
        for(unsigned i=0;i<3;++i) {matA.Put32(4*i,c.mr[i]);matA.Put32(0x10+4*i,c.mf[i]);matA.Put32(0x20+4*i,c.mu[i]);matA.Put32(0x30+4*i,c.mpos[i]);}
        for(unsigned i=0;i<3;++i)tmatA.Put32(0x30+4*i,c.tmpos[i]);
        // helicoptero
        vehA.Put32(0,u32(uintptr_t(vtbl.data())));
        for(unsigned i=0;i<3;++i) {vehA.Put32(4+4*i,c.hpos[i]);vehA.Put32(0x44+4*i,c.ms[i]);}
        vehA.Put32(0x10,c.heading);
        vehA.Put32(0x14,c.matNull?0u:u32(matA.Obj()));
        vehA.Put16(0x22,c.model);vehA.Put8(0x36,c.status);vehA.Put32(0x40,c.fl40);
        vehA.Put8(0x3BA,c.mission);vehA.Put32(0x41C,u32(uintptr_t(EntityOf(c.apKind))));
        vehA.Put8(0x428,c.fl428);vehA.Put32(0x460,c.drvKind?u32(drvA.Obj()):0u);
        vehA.Put32(0x504,c.iw[0]);vehA.Put32(0x580,c.fire?u32(fxA.Obj()):0u);vehA.Put32(0x984,c.iw[2]);vehA.Put8(0x988,c.hf988);
        vehA.Put8(0x9B9,c.nsw);for(unsigned i=0;i<4;++i)vehA.Put8(0x9BA+i,c.swt[i]);
        for(unsigned i=3;i<22;++i)vehA.Put32(kOffs[i],c.iw[i]);
        vehA.Put8(0xA11,c.a11);
        drvA.Put32(0x598,c.drvType);
        pvA.Put32(0x594,c.pvSub);
        PutTarget(entA,c);PutTarget(pvA,c);PutTarget(playerA,c);
        playerA.Put32(0x480,u32(dataA.Obj()));dataA.Put32(0,u32(wantedA.Obj()));
        wantedA.Put8(0x1E,c.wflags);wantedA.Put32(0x2C,c.wlevel);
        // globales
        G32(kNowMs)=c.now;G32(kPrevMs)=c.prev;G32(kTimeStep)=c.ts;*reinterpret_cast<uint8*>(kEventsFlag)=uint8(c.ev184);
        *RandStatePtr()=c.rseed;   // mismo estado inicial del rand() real del EXE en ambos lados
        vehA.Snap();matA.Snap();drvA.Snap();pvA.Snap();entA.Snap();tmatA.Snap();playerA.Snap();dataA.Snap();wantedA.Snap();fxA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();drvA.Tagged();pvA.Tagged();entA.Tagged();tmatA.Tagged();playerA.Tagged();dataA.Tagged();wantedA.Tagged();fxA.Tagged();
        TagAdd(reinterpret_cast<void*>(kPadsBase),kPadsBytes);
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;nev=0;
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(vehA.Obj()),c.edx,c.stack,0);});
        json obs;
        {
            json w=json::array();
            for(const u32 off:kOffs)w.push_back(off==0x580?Tag(uintptr_t(vehA.Get32(0x580))):vehA.Get32(off));
            obs["w"]=w;
        }
        obs["sw"]=json::array({vehA.m[kGuard+0x9BA],vehA.m[kGuard+0x9BB],vehA.m[kGuard+0x9BC],vehA.m[kGuard+0x9BD]});
        obs["a11"]=vehA.m[kGuard+0xA11];
        obs["rand_state"]=*RandStatePtr();
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["unexpected_virtuals"]=g.unexpected;
        {   // escrituras esperadas en el helicoptero: las palabras de kOffs, 0x9BA..0x9BD y 0xA11; todo lo demas y las otras arenas debe quedar igual
            auto changes=vehA.Changes();
            json rest=json::array();
            for(const auto& ch:changes) {
                const int64 o=ch[0].get<int64>();
                bool known=(o>=0x9BA&&o<0x9BE)||o==0xA11;
                for(const u32 off:kOffs)known|=o>=int64(off)&&o<int64(off)+4;
                if(!known)rest.push_back(ch);
            }
            obs["changes"]=json::array({rest,matA.Changes(),drvA.Changes(),pvA.Changes(),entA.Changes(),tmatA.Changes(),playerA.Changes(),dataA.Changes(),wantedA.Changes(),fxA.Changes()});
        }
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators,const json& realEntries) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    for(const auto& s:env.dllOnly)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D);   // rand real del EXE en ambos lados
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(unsigned f=0;f<kFunctions;++f)cases[kFn[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)},
        {"collaborators",collaborators},{"real_entries",realEntries},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseHeli(env,totals,c);
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

// Busca el hook registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada). Hooks Simple (direccion del EXE) y Virtual (direccion de la entrada en la vtabla del EXE);
// de un Virtual devuelve ademas el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato.
inline bool FindHook(uintptr_t exe,std::string& cat,std::string& name,uintptr_t* virtualBody=nullptr) {
    bool found=false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for(auto& h:c.Items()) {
            if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
            }
            else if(virtualBody&&h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
                *virtualBody=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
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
    p.dll=dllFunction?Body(dllFunction):0;   // 0: la DLL no llama a esta direccion (la funcion equivalente lleva su propio parche en dllOnly)
    return p;
}
// Colaborador virtual (hook V): el cuerpo de la DLL es la entrada de la vtabla del hook (un puntero a miembro virtual apuntaria a una ranura, no a la funcion)
inline Pair MakeCollabVirtual(Env& env,uintptr_t exe,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;uintptr_t body=0;
    if(FindHook(exe,cat,name,&body)&&body) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,body));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
        p.dll=Body(body);
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr}});
    return p;
}
template<class T> inline uintptr_t FnAddr(T f) {
    if constexpr (std::is_member_function_pointer_v<T>) return std::bit_cast<uintptr_t>(f); else return reinterpret_cast<uintptr_t>(f);
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"heli83f-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"heli83f-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("heli83f");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=MatrixLayoutOk();json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado (el Virtual apaga tambien el salto de la entrada del EXE). El cuerpo de la DLL (entrada de la vtabla de la DLL)
    // se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat,name;uintptr_t body=0;
        const bool found=FindHook(kEntry,cat,name,&body);
        if(found&&body)env.route[0].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry,body));
        registered[kFn[0]]=found&&env.route[0]&&env.route[0]->Registered();bodies[kFn[0]]=found&&env.route[0]&&env.route[0]->Body()!=0;
        ready&=found&&body&&env.route[0]&&env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[0]]=found?json({{"class",cat},{"name",name},{"address",kEntry}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators,json::array());return;}
    InitVtbl();
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    using Void=void*;
#pragma warning(push)
#pragma warning(disable: 4996)
    const auto matPoint=static_cast<CVector(*)(const CMatrix&,const CVector&)>(&operator*);   // [[deprecated]] pero es el destino del hook 0x59C890
#pragma warning(pop)
    env.stubs[0]={
        S(MakeCollabVirtual(env,0x6B1880,hooksFound),Void(&FakeAutoControl)),
        S(MakeCollab(env,0x4AA3F0,FnAddr(&FxSystem_c::Kill),hooksFound),Void(&FakeKill)),
        S(MakeCollab(env,0x53FF30,FnAddr(&CPad::HornJustDown),hooksFound),Void(&FakeHorn)),
        S(MakeCollab(env,0x72DDC0,FnAddr(&CCullZones::PlayerNoRain),hooksFound),Void(&FakeNoRain)),
        S(MakeCollab(env,0x72DD50,FnAddr(&CCullZones::NoPolice),hooksFound),Void(&FakeNoPolice)),
        S(MakeCollab(env,0x56E0D0,FnAddr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),hooksFound),Void(&FakeFindVeh)),
        S(MakeCollab(env,0x56E120,FnAddr(static_cast<CEntity*(*)(int32)>(&FindPlayerEntity)),hooksFound),Void(&FakeFindEntity)),
        S(MakeCollab(env,0x56E210,FnAddr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),hooksFound),Void(&FakeFindPed)),
        S(MakeCollab(env,0x56E230,FnAddr(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),hooksFound),Void(&FakeFindWanted)),
        S(MakeCollab(env,0x5073C0,FnAddr(&CAudioEngine::SayPedless),hooksFound),Void(&FakeSay)),
        S(MakeCollab(env,0x506F40,FnAddr(&CAudioEngine::ReportWeaponEvent),hooksFound),Void(&FakeWeaponEvent)),
        S(MakeCollab(env,0x6C4760,FnAddr(&CHeli::FindSwatPositionRelativeToHeli),hooksFound),Void(&FakeFindSwat)),
        S(MakeCollab(env,0x59C890,FnAddr(matPoint),hooksFound),Void(&FakeMatPoint)),
        S(MakeCollab(env,0x59C790,0,hooksFound),Void(&FakeMul3x3Exe)),
        S(MakeCollab(env,0x56A490,FnAddr(&CWorld::GetIsLineOfSightClear),hooksFound),Void(&FakeLos)),
        S(MakeCollab(env,0x73AF00,FnAddr(&FireOneInstantHitRound),hooksFound),Void(&FakeFire)),
        S(MakeCollab(env,0x556B40,FnAddr(&CRopes::RegisterRope),hooksFound),Void(&FakeRope)),
        S(MakeCollab(env,0x555DF0,FnAddr(&CRopes::SetSpeedOfTopNode),hooksFound),Void(&FakeRopeSpeed)),
        S(MakeCollab(env,0x6D3B80,FnAddr(&CVehicle::UpdateWinch),hooksFound),Void(&FakeWinch)),
        S(MakeCollab(env,0x6E3950,FnAddr(&CVehicle::ProcessWeapons),hooksFound),Void(&FakeWeapons)),
        S(MakeCollab(env,0x602590,FnAddr(&CInterestingEvents::Add),hooksFound),Void(&FakeInteresting)),
        S(MakeCollab(env,0x6C69C0,FnAddr(&CHeli::SendDownSwat),hooksFound),Void(&FakeSendDown)),
        S(MakeCollab(env,0x59C910,FnAddr(&CVector::Normalise),hooksFound),Void(&FakeNormalise))};
    // Multiply3x3 (0x59C790) no tiene hook y la DLL llama a AutoPreMultiply3x3Cpp (otra firma): solo se parchea el cuerpo de la DLL con su propio falso
    env.dllOnly.push_back(S(Pair{0,Body(FnAddr(&AutoPreMultiply3x3Cpp)),false},Void(&FakeMul3x3Dll)));
    // Entradas del EXE que la DLL tiene enganchadas y que corren de verdad en el lado original: sus hooks se apagan (0x41BD90 llama al rand() del EXE; la DLL no la llama)
    json realEntries=json::array();
    for(const uintptr_t a:{uintptr_t(0x41BD90)}) {
        std::string cat,name;
        const bool hooked=FindHook(a,cat,name);
        if(hooked)env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),a,HookRoute::RegisteredTarget{}));
        env.watches.emplace_back(a,16);
        ready&=!ReversingProbe::JumpsToDll(a);
        realEntries.push_back({{"exe",a},{"was_hooked",hooked}});
    }
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        json dllOnly=json::array();
        for(const auto& s:env.dllOnly)dllOnly.push_back({{"dll",s.pair.dll!=0}});
        collaborators[kFn[f]]={{"dll_real",json::array({"CPad::GetPad (0x53FB70, aritmetica sobre CPad::Pads)","rand() del CRT del EXE (0x821B1E) y 0x41BD90 del EXE (hook apagado)","GetUp/GetForward de la matriz (0x50E420/0x41CCB0)","CTimer y 0xC0B184 (regiones restauradas)","matriz, campos del helicoptero y entidades (arenas)"})},
            {"virtual_fakes",json::array()},{"real_entries",realEntries},{"stubbed",stubbed},{"dll_only",dllOnly},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    env.watches.emplace_back(0x827B3D,16);   // getptd
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators,realEntries);
}
}
