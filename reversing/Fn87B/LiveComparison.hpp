#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "General.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "GameLogic.h"
#include "Camera.h"
#include "Entity/Object/Object.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Pools/PedPool.h"
#include "Pools/ObjectPool.h"
#include "Pools/VehiclePool.h"
#include "Pools/Pools.h"
#include "Events/Event.h"
#include "Events/EventGroup.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventGunAimedAt.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn87B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CPlayerPed::FindNextWeaponLockOnTarget 0x60E530 (thiscall, 2 argumentos en pila, `ret 8`, bool en AL). `this` es una arena propia; los pools de peatones, vehiculos y objetos
// (0xB74490 / 0xB74494 / 0xB7449C) apuntan a pools falsos con cabecera, banderas y entidades en arenas; la camara (TheCamera, 0xB6F02C..0xB6F040) y dos floats de la camara (0xB6F9AC/B0) se fijan por caso.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y, si tienen hook, en el cuerpo de la DLL): CPed::GetWeaponSkill 0x5E6580, CWeaponInfo::GetWeaponInfo 0x743C60,
// CGeneral::GetATanOfXY 0x53CC70, CWeapon::TargetWeaponRangeMultiplier 0x73B380, CPlayerPed::CanIKReachThisTarget 0x609F80, LOSBlockedBetweenPeds 0x60B550, CPedGroups::AreInSameGroup 0x5F7F40,
// CObject::CanBeTargetted 0x59F320, CGameLogic::IsCoopGameGoingOn 0x441390, CPlayerPed::EvaluateNeighbouringTarget 0x60D1C0, CPedGroups::GetPedsGroup 0x5F7E80, CEvent::operator new 0x4B5620 (ranura real del pool de eventos)
// (sin hook), CPedGroupIntelligence::AddEvent 0x5F7470, CEventGroup::Add 0x4AB420, CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00 (todos con hook salvo los marcados). Corren de verdad en ambos
// lados: los constructores y destructores de CEventGunAimedAt y CEventGroupEvent (sobre la arena del evento o la pila) y la aritmetica en linea de la funcion.

namespace Fn87BLayout {
struct PpPed : CPed {
    static constexpr size_t OffPlayerData() { return offsetof(PpPed, m_pPlayerData); }
    static constexpr size_t OffIntelligence() { return offsetof(PpPed, m_pIntelligence); }
};
static_assert(PpPed::OffPlayerData() == 0x480 && PpPed::OffIntelligence() == 0x47C, "CPed: datos +0x480, inteligencia +0x47C");
static_assert(offsetof(CPed, m_nPedType) == 0x598 && offsetof(CPed, m_pVehicle) == 0x58C, "CPed: tipo +0x598, vehiculo +0x58C");
static_assert(sizeof(CPlayerPed) <= Fn87BFix::kPlayerWin, "tamanos");
static_assert(sizeof(CEventGunAimedAt) == 0x18, "CEventGunAimedAt");
}

namespace Fn87BLive {
using namespace Fn87BFix;

template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void FillByte(u8 b) {m.fill(b);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(ptrdiff_t off,u8 v) {m[kGuard+off]=v;}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}

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
// Puntero de argumento: dentro de una arena se etiqueta; si no es un local de la pila del llamador (cambia de una pasada a otra).
inline u32 TagP(const void* p) {
    const uintptr_t v=uintptr_t(p);
    for(const auto& e:tagTable)if(v>=e.base&&v<e.base+e.size)return Tag(v);
    return p?0xDEADC0DEu:0u;
}

// Indices de arena para las etiquetas (el orden de TagAdd es el de este enum)
enum Tg : unsigned { T_PL, T_DATA, T_PLMAT, T_TGT, T_CUR, T_CURMAT, T_INFO, T_INFO2, T_PEDHDR, T_PEDFLAGS, T_PEDS, T_PEDMAT0, T_VEHHDR = T_PEDMAT0 + kPeds, T_VEHFLAGS, T_VEHS, T_VEHMAT0,
                     T_OBJHDR = T_VEHMAT0 + kVehs, T_OBJFLAGS, T_OBJS, T_OBJMAT0, T_INTEL = T_OBJMAT0 + kObjs, T_GRP, T_EV, T_CAMMAT };

inline Arena<Fn87BFix::Arena(kPlayerWin)> pl;
inline Arena<Fn87BFix::Arena(kDataWin)> data;
inline Arena<Fn87BFix::Arena(kMatWin)> plMat,curMat,camMat;
inline Arena<Fn87BFix::Arena(kEntWin)> tgt,cur;
inline Arena<Fn87BFix::Arena(kInfoWin)> info,info2;
inline Arena<Fn87BFix::Arena(kHdrWin)> pedHdr,vehHdr,objHdr;
inline Arena<Fn87BFix::Arena(kFlagsWin)> pedFlags,vehFlags,objFlags;
inline Arena<Fn87BFix::Arena(kPedsWin)> peds;
inline Arena<Fn87BFix::Arena(kVehsWin)> vehs;
inline Arena<Fn87BFix::Arena(kObjsWin)> objs;
inline std::array<Arena<Fn87BFix::Arena(kMatWin)>,kPeds> pedMat;
inline std::array<Arena<Fn87BFix::Arena(kMatWin)>,kVehs> vehMat;
inline std::array<Arena<Fn87BFix::Arena(kMatWin)>,kObjs> objMat;
inline Arena<Fn87BFix::Arena(kIntelWin)> intel;
inline Arena<Fn87BFix::Arena(kGrpWin)> grp;
inline Arena<Fn87BFix::Arena(kEvWin)> ev;

constexpr size_t kPedStride=0x7C4,kVehStride=0xA18,kObjStride=0x19C;

struct State {
    const C* c{};
    unsigned atanN{},multN{},evalN{},reachN{},winfoN{};
    bool afterGetGroup{};
    std::array<bool,kSites+1> mutDone{};
};
inline State g;
inline void Mutate(u32 site) {
    if(g.c->mutSite!=site||g.mutDone[site])return;
    g.mutDone[site]=true;
    const u32 k=g.c->mutKind;
    const auto slotOf=[]{return i32(int8(pl.m[kGuard+0x718]));};
    if(k&1)pl.Put32(0x71C,pl.Get32(0x71C)?0u:u32(tgt.Obj()));
    if(k&2){pl.Put8(0x718,u8(g.c->mutSlot));pl.Put32(0x5A0+i32(int8(u8(g.c->mutSlot)))*0x1C,g.c->mutWtype);}
    if(k&4)info.Put32(4,g.c->mutRange);
    if(k&8)pl.Put32(0x5A0+slotOf()*0x1C,g.c->mutWtype);
}
inline unsigned IndexIn(const void* p,uintptr_t base,size_t stride,unsigned count) {   // indice de la entidad si el puntero apunta a una de la arena
    const uintptr_t v=uintptr_t(p);
    if(v<base+kGuard)return count;
    const uintptr_t off=v-base-kGuard;
    return off%stride==0&&off/stride<count?unsigned(off/stride):count;
}

// ---- falsos (float: se pasan y se devuelven como patrones de 32 bits; los que devuelven float lo hacen por ST0 con un fld de dword). Tipos de evento: ver fixture_oracle.py
inline u32 __fastcall FkSkill(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=Tag(self);Mutate(1);return g.c->skill;}                                                  // 0x5E6580
inline void* __cdecl FkWInfo(u32 type,u32 skill) {Ev& e=PushEv(2,2);e.v[0]=type;e.v[1]=skill;const unsigned k=g.winfoN++;Mutate(2);return reinterpret_cast<void*>(k==0?info.Obj():info2.Obj());}   // 0x743C60
inline float __cdecl FkAtan(u32 x,u32 y) {Ev& e=PushEv(3,2);e.v[0]=x;e.v[1]=y;Mutate(3);return F(g.c->atan[g.atanN++%12]);}                                         // 0x53CC70
inline float __cdecl FkMult(void* target,void* owner) {Ev& e=PushEv(4,2);e.v[0]=Tag(target);e.v[1]=Tag(owner);Mutate(4);return F(g.c->mult[g.multN++%12]);}            // 0x73B380
inline u32 __fastcall FkReach(void* self,void*,u32 x,u32 y,u32 z,void* weapon,u32 flag) {                                                                             // 0x609F80, `ret 14h`
    Ev& e=PushEv(5,5);e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=Tag(weapon);e.v[4]=flag&0xFFu;Mutate(5);
    return (g.c->reach[g.reachN++%12]&1u)|0x5A5A5A00u;
}
inline u32 __cdecl FkLos(void* a,void* b) {                                                                                                                         // 0x60B550
    Ev& e=PushEv(6,2);e.v[0]=Tag(a);e.v[1]=Tag(b);Mutate(6);
    const unsigned i=IndexIn(b,peds.Obj(0),kPedStride,kPeds);
    return (i<kPeds?g.c->ped[i].g&1u:0u)|0xA5A5A500u;
}
inline u32 __cdecl FkGroup(void* a,void* b) {                                                                                                                       // 0x5F7F40
    Ev& e=PushEv(7,2);e.v[0]=Tag(a);e.v[1]=Tag(b);Mutate(7);
    if(g.afterGetGroup)return (g.c->sameAfter&1u)|0x5A5A5A00u;   // la consulta del aviso (tras GetPedsGroup) tiene su propia respuesta
    const unsigned i=IndexIn(a,peds.Obj(0),kPedStride,kPeds);
    return (i<kPeds?g.c->ped[i].f&1u:0u)|0x5A5A5A00u;
}
inline u32 __fastcall FkTargetted(void* obj,void*) {                                                                                                                // 0x59F320
    Ev& e=PushEv(8,1);e.v[0]=Tag(obj);Mutate(8);
    const unsigned i=IndexIn(obj,objs.Obj(0),kObjStride,kObjs);
    return (i<kObjs?g.c->obj[i].a&1u:0u)|0x31000000u;
}
inline u32 __cdecl FkCoop() {PushEv(9,0);Mutate(9);return (g.c->coop&1u)|0x1234AB00u;}                                                                              // 0x441390
inline void __fastcall FkEval(void* self,void*,void* target,u32* outTarget,u32* outPrio,u32 maxDist,u32 heading,u32 arg5) {                                           // 0x60D1C0, `ret 18h`
    Ev& e=PushEv(10,7);e.v[0]=Tag(self);e.v[1]=Tag(target);e.v[2]=Tag(uintptr_t(*outTarget));e.v[3]=*outPrio;e.v[4]=maxDist;e.v[5]=heading;e.v[6]=arg5&0xFFu;
    const unsigned k=g.evalN++%12;
    if(g.c->evalSet[k]){*outTarget=u32(uintptr_t(target));*outPrio=g.c->evalPrio[k];}
    Mutate(10);
}
inline void* __cdecl FkGetGroup(void* ped) {Ev& e=PushEv(11,1);e.v[0]=Tag(ped);Mutate(11);g.afterGetGroup=true;return g.c->hasGroup?reinterpret_cast<void*>(grp.Obj()):nullptr;}                  // 0x5F7E80
inline void* __cdecl FkEventNew(u32 size) {   // ranura real del pool de eventos: el destructor virtual del EXE inlinea el borrado del pool, que no admite memoria ajena; el EXE y la DLL la liberan de verdad
    Ev& e=PushEv(12,1);e.v[0]=size;Mutate(12);
    void* const slot=GetEventPool()->New();
    if(slot&&tagTable.size()>30)tagTable[30]={uintptr_t(slot)-kGuard,kGuard+Fn87BFix::kEvWin};   // misma etiqueta que la arena del evento (indice 30, desplazamiento 0x40+off)
    return slot;
}                                      // 0x4B5620 (sin hook)
inline u32 __fastcall FkAddEvent(void* self,void*,void* event) {                                                                                                    // 0x5F7470, `ret 4`
    const auto* ge=static_cast<const CEventGroupEvent*>(event);
    Ev& e=PushEv(13,10);
    e.v[0]=Tag(self);e.v[1]=u32(static_cast<CEvent*>(event)->GetEventType());e.v[2]=Tag(uintptr_t(ge->m_ped));e.v[3]=Tag(uintptr_t(ge->m_event));
    if(ge->m_event) {const uint8* b=reinterpret_cast<const uint8*>(ge->m_event);e.v[4]=u32(ge->m_event->GetEventType());u16 w{},w10{};u32 d4{},p14{};memcpy(&w,b+0xE,2);memcpy(&w10,b+0x10,2);memcpy(&d4,b+4,4);memcpy(&p14,b+0x14,4);
        e.v[5]=w;e.v[6]=d4;e.v[7]=u32(b[8])|(u32(b[0xC])<<8);e.v[8]=w10;e.v[9]=Tag(uintptr_t(p14));} else {e.v[4]=0xFFFFFFFFu;e.v[5]=0;e.v[6]=0;e.v[7]=0;e.v[8]=0;e.v[9]=0;}
    Mutate(13);
    return 0xA5A5A500u;
}
inline u32 __fastcall FkGroupAdd(void* self,void*,void* event,u32 flag) {                                                                                           // 0x4AB420, `ret 8`
    const auto* b=static_cast<const uint8*>(event);
    Ev& e=PushEv(14,8);
    u32 d4{};u16 w0E{},w10{};u32 p14{};memcpy(&d4,b+4,4);memcpy(&w0E,b+0xE,2);memcpy(&w10,b+0x10,2);memcpy(&p14,b+0x14,4);
    e.v[0]=Tag(self);e.v[1]=u32(static_cast<CEvent*>(event)->GetEventType());e.v[2]=flag&0xFFu;e.v[3]=d4;e.v[4]=u32(b[8])|(u32(b[0xC])<<8);e.v[5]=w0E;e.v[6]=w10;e.v[7]=Tag(uintptr_t(p14));
    Mutate(14);
    return 0xA5A5A500u;
}
inline void __fastcall FkRegister(void* self,void*,void** ref) {Ev& e=PushEv(30,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}      // 0x571B70, `ret 4`
inline void __fastcall FkCleanUp(void* self,void*,void** ref) {Ev& e=PushEv(31,3);e.v[0]=Tag(self);e.v[1]=TagP(ref);e.v[2]=Tag(uintptr_t(ref?*ref:nullptr));}       // 0x571A00, `ret 4`

template<size_t N> inline void PutPos(Arena<N>& a,size_t base,const EntC& e,Arena<Fn87BFix::Arena(kMatWin)>& mat) {   // posicion en la matriz (+0x30) o en m_placement (+4): la otra queda con basura
    a.Put32(base+0x14,e.mt?u32(mat.Obj()):0u);
    if(e.mt){mat.Put32(0x30,e.x);mat.Put32(0x34,e.y);mat.Put32(0x38,e.z);}
    else{a.Put32(base+4,e.x);a.Put32(base+8,e.y);a.Put32(base+0xC,e.z);}
}

inline void CaseNext(Env& env,Totals& t,unsigned n) {
    const C c=MakeCase(n);
    const json input=json::parse(JsonCase(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        const u32 sd=c.seed*31u;
        pl.Fill(sd+1);data.Fill(sd+2);plMat.Fill(sd+3);curMat.Fill(sd+4);camMat.Fill(sd+5);tgt.Fill(sd+6);cur.Fill(sd+7);info.Fill(sd+8);info2.Fill(sd+9);
        pedHdr.Fill(sd+10);vehHdr.Fill(sd+11);objHdr.Fill(sd+12);pedFlags.Fill(sd+13);vehFlags.Fill(sd+14);objFlags.Fill(sd+15);peds.Fill(sd+16);vehs.Fill(sd+17);objs.Fill(sd+18);
        intel.Fill(sd+19);grp.Fill(sd+20);ev.FillByte(0xCC);
        for(unsigned i=0;i<kPeds;++i)pedMat[i].Fill(sd+30+i);
        for(unsigned i=0;i<kVehs;++i)vehMat[i].Fill(sd+40+i);
        for(unsigned i=0;i<kObjs;++i)objMat[i].Fill(sd+50+i);
        // jugador: matriz (vector hacia delante en +0x10/+0x14), datos, ranura, arma e informacion del arma
        pl.Put32(0x14,u32(plMat.Obj()));plMat.Put32(0x10,c.fx);plMat.Put32(0x14,c.fy);
        pl.Put32(0x480,u32(data.Obj()));pl.Put8(0x718,u8(c.slot));pl.Put32(0x71C,c.tgt?u32(tgt.Obj()):0u);
        pl.Put32(0x5A0+i32(int8(u8(c.slot)))*0x1C,c.wtype);
        info.Put32(4,c.rng);
        info2.Put32(0,c.fire);
        // objetivo actual (argumento): entidad propia o un peaton del pool
        {EntC ce{};ce.mt=c.cmt;ce.x=c.cx;ce.y=c.cy;ce.z=c.cz;PutPos(cur,0,ce,curMat);}
        // pools
        for(unsigned i=0;i<kPeds;++i) {
            const EntC& e=c.ped[i];const size_t b=i*kPedStride;
            PutPos(peds,b,e,pedMat[i]);
            peds.Put8(b+0x36,u8(e.t));
            peds.Put32(b+0x530,e.a);
            peds.Put32(b+0x46C,(peds.Get32(b+0x46C)&~0x100u)|(e.b?0x100u:0u));
            peds.Put32(b+0x58C,e.c>=1&&e.c<=3?u32(vehs.Obj(kGuard+(e.c-1)*kVehStride)):0u);
            peds.Put32(b+0x470,(peds.Get32(b+0x470)&~0x10000000u)|(e.d?0x10000000u:0u));
            peds.Put32(b+0x598,e.e);
            peds.Put32(b+0x47C,u32(intel.Obj()));
            pedFlags.Put8(i,e.pr?u8(Rand(c.seed,900+i)&0x7Fu):u8(0x80u|(Rand(c.seed,910+i)&0x7Fu)));
        }
        for(unsigned i=0;i<kVehs;++i) {
            const EntC& e=c.veh[i];const size_t b=i*kVehStride;
            PutPos(vehs,b,e,vehMat[i]);
            vehs.Put8(b+0x36,u8(e.t));
            vehs.Put32(b+0x47C,u32(intel.Obj()));vehs.Put32(b+0x40,e.a);vehs.Put32(b+0x594,e.b);vehs.Put32(b+0x590,e.c);vehs.Put8(b+0x42D,u8(e.d));
            vehFlags.Put8(i,e.pr?u8(Rand(c.seed,920+i)&0x7Fu):u8(0x80u|(Rand(c.seed,930+i)&0x7Fu)));
        }
        for(unsigned i=0;i<kObjs;++i) {
            const EntC& e=c.obj[i];const size_t b=i*kObjStride;
            PutPos(objs,b,e,objMat[i]);
            objs.Put8(b+0x36,u8(e.t));
            objs.Put32(b+0x18,e.c?0x00ABCD00u+i*0x10u:0u);objs.Put8(b+0x140,u8(e.b));
            objFlags.Put8(i,e.pr?u8(Rand(c.seed,940+i)&0x7Fu):u8(0x80u|(Rand(c.seed,950+i)&0x7Fu)));
        }
        pedHdr.Put32(0,u32(peds.Obj()));pedHdr.Put32(4,u32(pedFlags.Obj()));pedHdr.Put32(8,c.pedSize);
        vehHdr.Put32(0,u32(vehs.Obj()));vehHdr.Put32(4,u32(vehFlags.Obj()));vehHdr.Put32(8,c.vehSize);
        objHdr.Put32(0,u32(objs.Obj()));objHdr.Put32(4,u32(objFlags.Obj()));objHdr.Put32(8,c.objSize);
        *reinterpret_cast<uint32*>(0xB74490)=u32(pedHdr.Obj());*reinterpret_cast<uint32*>(0xB74494)=u32(vehHdr.Obj());*reinterpret_cast<uint32*>(0xB7449C)=u32(objHdr.Obj());
        // camara: posicion en la matriz (+0x30) si existe y en m_placement (0xB6F02C) si no; la otra queda con basura
        *reinterpret_cast<uint32*>(0xB6F03C)=c.camMt?u32(camMat.Obj()):0u;
        if(c.camMt){camMat.Put32(0x30,c.camX);camMat.Put32(0x34,c.camY);camMat.Put32(0x38,c.camZ);*reinterpret_cast<uint32*>(0xB6F02C)=sd+60;*reinterpret_cast<uint32*>(0xB6F030)=sd+61;*reinterpret_cast<uint32*>(0xB6F034)=sd+62;}
        else {*reinterpret_cast<uint32*>(0xB6F02C)=c.camX;*reinterpret_cast<uint32*>(0xB6F030)=c.camY;*reinterpret_cast<uint32*>(0xB6F034)=c.camZ;}
        *reinterpret_cast<uint32*>(0xB6F9AC)=c.cam9AC;*reinterpret_cast<uint32*>(0xB6F9B0)=c.cam9B0;
        *reinterpret_cast<uint8*>(0x96A8B2)=u8(c.twoPlayer);
        const u32 curPtr=c.cur==0?0u:(c.cur==1?u32(cur.Obj()):u32(peds.Obj(kGuard+(c.cur-2)*kPedStride)));
        pl.Snap();data.Snap();plMat.Snap();curMat.Snap();camMat.Snap();tgt.Snap();cur.Snap();info.Snap();info2.Snap();pedHdr.Snap();vehHdr.Snap();objHdr.Snap();pedFlags.Snap();vehFlags.Snap();objFlags.Snap();
        peds.Snap();vehs.Snap();objs.Snap();intel.Snap();grp.Snap();ev.Snap();
        for(auto& a:pedMat)a.Snap();for(auto& a:vehMat)a.Snap();for(auto& a:objMat)a.Snap();
        TagClear();   // el orden sigue a Tg
        pl.Tagged();data.Tagged();plMat.Tagged();tgt.Tagged();cur.Tagged();curMat.Tagged();info.Tagged();info2.Tagged();pedHdr.Tagged();pedFlags.Tagged();peds.Tagged();for(auto& a:pedMat)a.Tagged();
        vehHdr.Tagged();vehFlags.Tagged();vehs.Tagged();for(auto& a:vehMat)a.Tagged();objHdr.Tagged();objFlags.Tagged();objs.Tagged();for(auto& a:objMat)a.Tagged();intel.Tagged();grp.Tagged();ev.Tagged();camMat.Tagged();
        g=State{};g.c=&c;nev=0;
        const u32 args[2]{curPtr,c.arg1};
        sides[s]=Execute(env,0,0x60E530,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(pl.Obj()),c.edx,c.stack,0);});
        memset(ev.m.data()+kGuard,0xCC,4);   // vptr del evento (la vtable del EXE o de la DLL segun el lado, no comparable)
        json obs;
        obs["changes"]=json::array({pl.Changes(),data.Changes(),tgt.Changes(),cur.Changes(),info.Changes(),info2.Changes(),intel.Changes(),grp.Changes(),ev.Changes(),peds.Changes(),vehs.Changes(),objs.Changes()});
        obs["events"]=EvsJson();
        obs["ret_al"]=sides[s].reg.eax&0xFFu;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

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
    json cases=json::object();cases[kFn[0]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        unsigned before=totals.failures;
        for(unsigned c=0;c<kCases;++c)CaseNext(env,totals,c);
        Write({{"event","function_summary"},{"function",kFn[0]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fn87b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fn87b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fn87b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t skillBody=Addr(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill));
    const uintptr_t winfoBody=Addr(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo));
    const uintptr_t atanBody=Addr(&CGeneral::GetATanOfXY);
    const uintptr_t multBody=Addr(&CWeapon::TargetWeaponRangeMultiplier);
    const uintptr_t reachBody=Addr(&CPlayerPed::CanIKReachThisTarget);
    const uintptr_t losBody=Addr(&LOSBlockedBetweenPeds);
    const uintptr_t groupBody=Addr(&CPedGroups::AreInSameGroup);
    const uintptr_t targettedBody=Addr(&CObject::CanBeTargetted);
    const uintptr_t coopBody=Addr(&CGameLogic::IsCoopGameGoingOn);
    const uintptr_t evalBody=Addr(&CPlayerPed::EvaluateNeighbouringTarget);
    const uintptr_t getGroupBody=Addr(&CPedGroups::GetPedsGroup);
    const uintptr_t eventNewBody=Addr(static_cast<void*(*)(unsigned)>(&CEvent::operator new));
    const uintptr_t eventDeleteBody=Addr(static_cast<void(*)(void*)>(&CEvent::operator delete));
    const uintptr_t addEventBody=Addr(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent));
    const uintptr_t groupAddBody=Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add));
    const uintptr_t registerBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t cleanBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    env.route[0].reset(new HookRoute("CPlayerPed","FindNextWeaponLockOnTarget",0x60E530,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=skillBody&&winfoBody&&atanBody&&multBody&&reachBody&&losBody&&groupBody&&targettedBody&&coopBody&&evalBody&&getGroupBody&&eventNewBody&&eventDeleteBody&&addEventBody&&groupAddBody&&registerBody&&cleanBody;
    env.stubs[0]={{Pair{0x5E6580,skillBody,true},reinterpret_cast<void*>(&FkSkill)},
                  {Pair{0x743C60,winfoBody,true},reinterpret_cast<void*>(&FkWInfo)},
                  {Pair{0x53CC70,atanBody,true},reinterpret_cast<void*>(&FkAtan)},
                  {Pair{0x73B380,multBody,true},reinterpret_cast<void*>(&FkMult)},
                  {Pair{0x609F80,reachBody,true},reinterpret_cast<void*>(&FkReach)},
                  {Pair{0x60B550,losBody,true},reinterpret_cast<void*>(&FkLos)},
                  {Pair{0x5F7F40,groupBody,true},reinterpret_cast<void*>(&FkGroup)},
                  {Pair{0x59F320,targettedBody,true},reinterpret_cast<void*>(&FkTargetted)},
                  {Pair{0x441390,coopBody,true},reinterpret_cast<void*>(&FkCoop)},
                  {Pair{0x60D1C0,evalBody,true},reinterpret_cast<void*>(&FkEval)},
                  {Pair{0x5F7E80,getGroupBody,true},reinterpret_cast<void*>(&FkGetGroup)},
                  {Pair{0x4B5620,eventNewBody,true},reinterpret_cast<void*>(&FkEventNew)},
                  {Pair{0x5F7470,addEventBody,true},reinterpret_cast<void*>(&FkAddEvent)},
                  {Pair{0x4AB420,groupAddBody,true},reinterpret_cast<void*>(&FkGroupAdd)},
                  {Pair{0x571B70,registerBody,true},reinterpret_cast<void*>(&FkRegister)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&FkCleanUp)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked},{"state_jumps",ReversingProbe::JumpsToDll(s.pair.exe)},{"state_module_ok",s.pair.exe?ModuleOf(Resolve(s.pair.exe))==env.asi:false}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"constructores y destructores de CEventGunAimedAt y CEventGroupEvent (el evento reservado es una ranura real del pool; CEvent::operator delete corre real) y la pila","aritmetica en linea: restas, productos, comparaciones","X87TimesFloatCpp (X87Math.cpp)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60E530,16);
    SaveRegions();
    {   // pool de eventos: cabecera (+0 objetos, +4 estados, +8 capacidad, +0xC ultima ranura libre) y bytes de estado (se restauran tras cada lado)
        const uintptr_t pool=*reinterpret_cast<uintptr_t*>(0xB744AC);
        regions.emplace_back(pool,0x14);
        regions.emplace_back(*reinterpret_cast<uintptr_t*>(pool+4),size_t(*reinterpret_cast<int32*>(pool+8)));
        ready&=uintptr_t(GetEventPool())==pool;
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
