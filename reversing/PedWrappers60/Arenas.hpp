#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <vector>
#include "Fixtures.hpp"

// Arenas, etiquetas, traza de eventos y falsos del banco PedWrappers60 (solo C++ estandar + Fixtures.hpp): los usan Core.hpp (banco en el juego) y selftest_source.cpp (autoprueba del fuente
// compilado contra los tipos reales del juego y los mismos falsos, sin el juego). Los falsos son funciones libres __fastcall/__cdecl con la firma del EXE; el banco las pone como parches de
// entrada y la autoprueba las llama desde definiciones de los colaboradores.
namespace PedWrappers60Live {
using namespace PedWrappers60Fix;
using u8 = std::uint8_t;
using uint32 = std::uint32_t;
using int32 = std::int32_t;
using int16 = std::int16_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using i16 = std::int16_t;
// ------------------------------------------------------------------------------------------------ memoria de los casos
template<size_t N> struct Ar { alignas(16) std::array<u8,N> now{},ref{}; };
inline Ar<kPedArena> aPed;inline Ar<kPdataArena> aPdata,aPdata2;inline Ar<kPpedArena> aPped;inline Ar<kEntArena> aEnt;inline Ar<kVehArena> aVeh;
inline Ar<kMatArena> aMatPed,aMatOther,aMatCam;inline Ar<kMinfoArena> aMi0,aMi1,aObjinfo;
inline Ar<kTokArena> aFx,aHier,aClump,aIntel,aWobj,aCm0,aCm1,aTex;inline Ar<kMatArrArena> aMatArr;inline Ar<kBoolsArena> aBools;inline Ar<kVtblArena> aVtbl;

struct Slot {u8* now;u8* ref;u32 bytes,off,arena,label;};
inline std::array<Slot,L_COUNT> slots{};
template<size_t N> inline Slot Mk(Ar<N>& a,u32 bytes,u32 off,u32 label){return Slot{a.now.data(),a.ref.data(),bytes,off,u32(N),label};}
inline void InitSlots() {
    slots={};
    auto set=[](Slot s){slots[s.label]=s;};
    set(Mk(aPed,kPedBytes,kPedOff,L_PED));set(Mk(aPdata,kPdataBytes,kPdataOff,L_PDATA));set(Mk(aPdata2,kPdataBytes,kPdataOff,L_PDATA2));set(Mk(aPped,kPpedBytes,kPpedOff,L_PPED));
    set(Mk(aEnt,kEntBytes,kEntOff,L_ENT));set(Mk(aVeh,kVehBytes,kVehOff,L_VEH));set(Mk(aMatPed,kMatBytes,kMatOff,L_MATPED));set(Mk(aMatOther,kMatBytes,kMatOff,L_MATOTHER));
    set(Mk(aMatCam,kMatBytes,kMatOff,L_MATCAM));set(Mk(aMi0,kMinfoBytes,kMinfoOff,L_MI0));set(Mk(aMi1,kMinfoBytes,kMinfoOff,L_MI1));set(Mk(aObjinfo,kMinfoBytes,kMinfoOff,L_OBJINFO));
    set(Mk(aFx,kTokBytes,kTokOff,L_FX));set(Mk(aHier,kTokBytes,kTokOff,L_HIER));set(Mk(aClump,kTokBytes,kTokOff,L_CLUMP));set(Mk(aIntel,kTokBytes,kTokOff,L_INTEL));set(Mk(aWobj,kTokBytes,kTokOff,L_WOBJ));
    set(Mk(aCm0,kTokBytes,kTokOff,L_CM0));set(Mk(aCm1,kTokBytes,kTokOff,L_CM1));set(Mk(aTex,kTokBytes,kTokOff,L_TEX));set(Mk(aMatArr,kMatArrBytes,kMatArrOff,L_MATARR));
    set(Mk(aBools,kBoolsBytes,kBoolsOff,L_BOOLS));set(Mk(aVtbl,kVtblBytes,kVtblOff,L_VTBL));
}
// L_MATOTHER (matriz del vehiculo en ProcessControl / de la entidad en PreCollision); L_ENT = entidad bajo los pies (PC) / colPhysical (PS).
inline u32 AddrOf(u32 label){return label>0&&label<L_COUNT?u32(uintptr_t(slots[label].now+slots[label].off)):0u;}
inline u32 ResolvePtr(u32 v){return IsTok(v)?AddrOf(v&0xFFFFFFu):v;}
// etiqueta de un puntero: la del objeto cuya ventana empieza ahi; 0x100 + desplazamiento para direcciones dentro del ped; 0 nulo; 0xBAD otro
inline u32 Label(const void* p){
    if(!p)return 0u;const u8* q=static_cast<const u8*>(p);
    const u8* ped=slots[L_PED].now+kPedOff;
    if(q>=ped&&q<ped+kPedBytes)return q==ped?u32(L_PED):0x100u+u32(q-ped);
    for(u32 l=1;l<L_COUNT;++l)if(l!=L_PED&&q==slots[l].now+slots[l].off)return l;
    return 0xBADu;
}
inline u32 LabelU(u32 v){return v?Label(reinterpret_cast<const void*>(uintptr_t(v))):0u;}
inline u32 Bits(const void* p){u32 v;std::memcpy(&v,p,4);return v;}
inline u32 Fnv32(const char* s){u64 h=0xCBF29CE484222325ull;for(unsigned i=0;i<64&&s[i];++i){h=(h^u8(s[i]))*0x100000001B3ull;}return u32(h);}

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
constexpr u32 kEvWidth=20;
struct Ev {u32 w[kEvWidth];};
inline std::array<Ev,96> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0,u32 f=0,u32 g=0,u32 h=0,u32 i=0,u32 j=0,u32 k2=0,u32 l=0,u32 m=0,u32 n=0,u32 o=0,u32 p=0){
    if(nev<evs.size()){evs[nev]=Ev{{k,a,b,c,d,e,f,g,h,i,j,k2,l,m,n,o,p,0,0,0}};++nev;}
}
// tipos de evento (mismo orden que fixture_oracle.py)
enum : u32 {K_AE_SERVICE=1,K_SWIM,K_GETHIER,K_BONEIDX,K_MATARR,K_CREATEFX,K_FXLOCAL,K_FXCOPY,K_FXPLAY,K_FXDESTROY,K_COORS,K_PRUNE,K_GETALPHA,K_SETALPHA,K_COLMODELS,K_FINDPLAYER,K_ISMEMBER,
            K_BUOY,K_PFIRST,K_PPROC,K_SKIP,K_PHYSPC,K_DELW,K_FOOT,K_NEWVEL,K_UPDPOS,K_MOVEANIM,K_BLOOD,K_SHADOW,K_PEDCOUNT,K_SAY,K_WAE_ADD,K_WAE_SERVICE,K_BADVIRT,K_CLIMB};

// caso actual (los falsos lo consultan)
inline const PCase* gP{};inline const SCase* gS{};
inline u32 gRet{};      // EAX que devuelven los falsos cuyo valor no se lee

inline void Mutate(u32 stub){
    if(!gP)return;
    for(const auto& m:gP->muts)if(m.stub==stub){u32 v;u8* at=slots[L_PED].now+kPedOff+m.off;std::memcpy(&v,at,4);v^=m.mask;std::memcpy(at,&v,4);}
}
// ---- falsos (thiscall = __fastcall (this, edx, args...) con el mismo `ret N`; cdecl = __cdecl)
inline u32 __fastcall FakeAeService(void* self,void*){Push(K_AE_SERVICE,Label(self));return gRet;}
inline u32 __fastcall FakeSwim(void* intel,void*){Push(K_SWIM,Label(intel));return gP?ResolvePtr(gP->rSwim):gRet;}
inline u32 __fastcall FakeClimb(void* intel,void*){Push(K_CLIMB,Label(intel));return gS?ResolvePtr(gS->rClimb):gRet;}
inline u32 __cdecl FakeGetHier(u32 clump){Push(K_GETHIER,LabelU(clump));return AddrOf(L_HIER);}
inline u32 __cdecl FakeBoneIdx(u32 hier,u32 id){Push(K_BONEIDX,LabelU(hier),id);return gP->rBone;}
inline u32 __cdecl FakeMatArr(u32 hier){Push(K_MATARR,LabelU(hier));return AddrOf(L_MATARR);}
inline u32 __fastcall FakeCreateFx(void* mgr,void*,const char* name,const u8* pos,u32 matrix,u32 flag){
    Push(K_CREATEFX,u32(uintptr_t(mgr)),Fnv32(name),Bits(pos),Bits(pos+4),Bits(pos+8),matrix-AddrOf(L_MATARR),flag&0xFFu);return gP->rFx?AddrOf(L_FX):0u;}
inline u32 __fastcall FakeFxLocal(void* fx,void*,u32 flag){Push(K_FXLOCAL,Label(fx),flag&0xFFu);return gRet;}
inline u32 __fastcall FakeFxCopy(void* fx,void*){Push(K_FXCOPY,Label(fx));return gRet;}
inline u32 __fastcall FakeFxPlay(void* fx,void*){Push(K_FXPLAY,Label(fx));return gRet;}
inline u32 __fastcall FakeFxDestroy(void* mgr,void*,void* fx){Push(K_FXDESTROY,u32(uintptr_t(mgr)),Label(fx));return gRet;}
inline u32 __cdecl FakeCoors(u32* out,i32 id){Push(K_COORS,u32(id));out[0]=gP->plCoors[0];out[1]=gP->plCoors[1];out[2]=gP->plCoors[2];return u32(uintptr_t(out));}
inline u32 __fastcall FakePrune(void* self,void*){Push(K_PRUNE,Label(self));Mutate(S_PRUNE);return gRet;}
inline u32 __cdecl FakeGetAlpha(u32 clump){Push(K_GETALPHA,LabelU(clump));return gP->rAlpha;}
inline u32 __cdecl FakeSetAlpha(u32 clump,u32 alpha){Push(K_SETALPHA,LabelU(clump),alpha);return gRet;}
inline u32 __cdecl FakeColModels(u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h){Push(K_COLMODELS,LabelU(a),LabelU(b),LabelU(c),LabelU(d),e,f,g,h&0xFFu);return gP->rCol;}
inline u32 __cdecl FakeFindPlayer(i32 id){Push(K_FINDPLAYER,u32(id));return gP->rFindPlayer==0?0u:AddrOf(gP->rFindPlayer==1?L_PED:L_PPED);}
inline u32 __fastcall FakeIsMember(void* group,void*,void* ped){Push(K_ISMEMBER,u32(uintptr_t(group))-kGroups,Label(ped));return (gP->rJunk&~0xFFu)|(gP->rIsMember?1u:0u);}
inline u32 __fastcall FakeBuoy(void* self,void*){Push(K_BUOY,Label(self));Mutate(S_BUOY);return gRet;}
inline u32 __fastcall FakePFirst(void* intel,void*){Push(K_PFIRST,Label(intel));Mutate(S_PFIRST);return gRet;}
inline u32 __fastcall FakePProc(void* intel,void*){Push(K_PPROC,Label(intel));Mutate(S_PPROC);return gRet;}
inline u32 __fastcall FakeSkip(void* self,void*){Push(K_SKIP,Label(self));Mutate(S_SKIP);return gRet;}
inline u32 __fastcall FakePhysPc(void* self,void*){Push(K_PHYSPC,Label(self));Mutate(S_PHYSPC);return gRet;}
inline u32 __fastcall FakeDelW(void* self,void*){Push(K_DELW,Label(self));Mutate(S_DELW);return gRet;}
inline u32 __fastcall FakeFoot(void* self,void*){Push(K_FOOT,Label(self));Mutate(S_FOOT);return gRet;}
inline u32 __fastcall FakeNewVel(void* self,void*){Push(K_NEWVEL,Label(self));Mutate(S_NEWVEL);return gRet;}
inline u32 __fastcall FakeUpdPos(void* self,void*){Push(K_UPDPOS,Label(self));Mutate(S_UPDPOS);return gRet;}
inline u32 __fastcall FakeMoveAnim(void* self,void*){Push(K_MOVEANIM,Label(self));Mutate(S_MOVEANIM);return gRet;}
inline u32 __fastcall FakeBadVirt(void* self,void*){Push(K_BADVIRT,Label(self));return gRet;}
inline u32 __cdecl FakeBlood(){Push(K_BLOOD);return (gP->rJunk&~0xFFu)|(gP->rBlood&0xFFu);}
inline u32 __cdecl FakeShadow(u32 type,u32 tex,const u8* pos,u32 topX,u32 topY,u32 rightX,u32 rightY,u32 intensity,u32 red,u32 green,u32 blue,u32 zdist,u32 time,u32 up){
    Push(K_SHADOW,type&0xFFu,LabelU(tex),Bits(pos),Bits(pos+4),Bits(pos+8),topX,topY,rightX,rightY,intensity&0xFFFFu,red&0xFFu,green&0xFFu,blue&0xFFu,zdist,time,up);return gRet;}
inline u32 __cdecl FakePedCount(void* ped,u32 flag){Push(K_PEDCOUNT,Label(ped),flag&0xFFu);return gRet;}
inline u32 __fastcall FakeSay(void* self,void*,u32 ctx,u32 delay,u32 prob,u32 b1,u32 b2,u32 b3){Push(K_SAY,Label(self),ctx&0xFFFFu,delay,prob,b1&0xFFu,b2&0xFFu,b3&0xFFu);return gRet;}
inline u32 __fastcall FakeWaeAdd(void* self,void*,u32 event){Push(K_WAE_ADD,Label(self),event);return gRet;}
inline u32 __fastcall FakeWaeService(void* self,void*){Push(K_WAE_SERVICE,Label(self));return gRet;}


// aplica los pokes de un objeto sobre su imagen previa (ya rellena con basura) y la copia a `now`
inline void ApplyPokes(Slot& s,const Obj& o,u32 seed){
    FillWords(s.ref,s.arena,seed);
    for(const auto& q:o.p){
        u32 v=q.ptr?ResolvePtr(q.value):q.value;
        std::memcpy(s.ref+s.off+q.off,&v,q.size);
    }
}
// cambios de una arena respecto a su imagen previa: [arena, desplazamiento respecto al objeto (negativo = guarda delantera), byte nuevo]; se omiten los desplazamientos [skip0, skip1)
template<class Out> inline void Diff(Out& out,unsigned arena,const Slot& s,u32 skip0=0,u32 skip1=0){
    for(u32 i=0;i<s.arena;++i)if(s.now[i]!=s.ref[i]){const int rel=int(i)-int(s.off);if(rel>=int(skip0)&&rel<int(skip1))continue;out.push_back(std::array<int,3>{int(arena),rel,int(s.now[i])});}
}
inline bool SameArena(const Slot& s){return std::memcmp(s.now,s.ref,s.arena)==0;}

}
