#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <intrin.h>
#include <type_traits>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CarCtrl.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84L dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::GenerateOneRandomCar (0x430050, hook S, cdecl sin argumentos, void). La funcion de la DLL es una transcripcion mecanica del EXE (reversing/Car84L/tools/lift.py) que llama a los
// colaboradores por su direccion del EXE: basta con parchear la entrada del EXE (los hooks RH de esos colaboradores quedan apagados durante el banco). Falsos (registran sus argumentos y el estado
// del vehiculo EN ORDEN y devuelven lo que dice el caso): ver kind en cada falso. Corren de verdad (bytes del EXE en los dos lados, con su hook apagado): srand/rand/_ftol2, GetRandomNumberInRange (int y float),
// aritmetica de vectores (0x40FE30/0x40FE60/0x40FEC0/0x4119D0), 0x4082C0, 0x59C910, 0x41CCB0, 0x420800, 0x420920/0x420950/0x420980, posicion de nodos y tramos (0x420A10/0x420A60), 0x421120, 0x424130, 0x43C710, 0x43C900 y 0x44DB00.
// Tras el evento numero mutAt el falso muta el estado del vehiculo para comprobar que la funcion relee esos campos donde el EXE los relee.

namespace C84LLive {
using namespace C84LFix;
inline constexpr uintptr_t kEntry = 0x430050;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Zero() {m.fill(0);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    u32 Get16(size_t off) const {uint16 v;memcpy(&v,m.data()+kGuard+off,2);return v;}
    u32 Get8(size_t off) const {return m[kGuard+off];}
    json Rest() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA;
inline Arena<kVecArena> centreA, coorsA;
inline Arena<kWantedArena> wantedA;
inline Arena<kPvArena> pvA;
inline Arena<kZoneArena> zoneA;
inline Arena<kTgtArena> tgtA;
inline Arena<kPedArena> pedA;
inline Arena<kVtArena> vtA;
inline std::array<Arena<kModelArena>,kVariants> miA;
inline std::array<Arena<kColArena>,kVariants> colA;
inline std::array<Arena<kLinkArena>,kSlots> linkA;
inline Arena<kNodeArena> nodeA;
inline Arena<kNlArena> nlA;
inline Arena<kNaviArena> naviA;

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
struct State {const C* c{};u32 evHash{};u32 ev{};u32 vl{};int side{};};
inline State g;
inline std::array<u32,512> sites{};inline unsigned nsites{};   // direcciones de retorno de los falsos del lado original (cobertura por sitio de llamada del EXE)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline void Site(const void* ret) {if(g.side==0&&nsites<sites.size())sites[nsites++]=u32(uintptr_t(ret));}
// Mutaciones: 1 contador de seguidos (+0x590), 2 subtipo (+0x594), 3 banderas (+0x36), 4 nodo siguiente (+0x394), 5 tramo (+0x3A6), 6 modelo (+0x22), 7 conductor (+0x460), 8 crucero (+0x3D0)
inline void ApplyMut(u32 kind,u32 v) {
    switch(kind) {
    case 1: vehA.Put32(0x590,v&3u);break;
    case 2: vehA.Put32(0x594,v%20u);break;
    case 3: vehA.Put8(0x36,v);break;
    case 4: vehA.Put32(0x394,g.c->pn[0]);break;
    case 5: vehA.Put16(0x3A6,g.c->pn[2]);break;
    case 6: vehA.Put16(0x22,kModels[v%kModels.size()]);break;
    case 7: vehA.Put32(0x460,vehA.Get32(0x460)?0u:u32(pedA.Obj()));break;
    case 8: vehA.Put8(0x3D0,v);break;
    default: break;
    }
}
inline void After() {
    if(g.ev++!=g.c->mutAt)return;
    ApplyMut(g.c->mutKind,g.c->mutVal);
}
inline u32 VehFp() {return Fnv(vehA.m.data()+kGuard,kVehWin);}
// Registra un evento: etiqueta + argumentos + huella del vehiculo
inline void Log(u32 kind,std::initializer_list<u32> args) {
    Ev& e=PushEv(kind,u32(args.size())+1);
    u32 i=0;
    for(u32 a:args)e.v[i++]=a;
    e.v[i]=VehFp();
    HashEv(e);
}
inline u32 Rd(const void* p) {u32 v;memcpy(&v,p,4);return v;}
inline u32 Model() {return vehA.Get16(0x22);}

// Eventos (kind): 1 FindPlayerCentreOfWorld [id]  2 FindPlayerCoors [id]  3 FewerCars  4 FindCarMultiplierMotorway  5 FindPlayerWanted [id]  6 GangWarFightingGoingOn  7 ChoosePoliceCarModel [a]  8 ChooseModel [&tipo]
// 9 LaRiotsActiveHere  10 FindPlayerVehicle [id, bandera]  11 GenerateCarCreationCoors2 [15 argumentos]  12 RequestModel [modelo, bandera]  13 PickLeastUsedModel [esto, arg]  14 FindObjectsKindaColliding [11]
// 15 GetZoneInfo [posicion x, y, z, bandera]  16 GetNewVehicleDependingOnCarModel [modelo, bandera]  17..20 FindPolice*  21 GetVehicleAppearance [esto]  22 ChooseVehicleColour [esto, 5 argumentos]  23 CWorld::Add [vehiculo]
// 24 ProcessVerticalLine [x, y, z, distancia, &golpe, &entidad, 6 banderas]  25 GetWaterLevel [x, y, z, &nivel, bandera, &normales]  26 IsBoatModel [modelo]  27 destructor virtual [esto, bandera]  28 GetHeightAboveRoad virtual [esto]
// 29 SetPosn [esto, x, y, z]  30 SetClumpAlpha [clump, alfa]  31 AddVehicleUpgrade [esto, modelo]  32 GetIsOnScreen [esto]  33 PossiblyStreamCarOutAfterCreation [modelo]  34 CreatePoliceChase [vehiculo, a, nodo]
// 35 CreateConvoy [vehiculo, a]  36 SetUpDriverAndPassengersForVehicle [6]  37 AddPoliceCarOccupants [vehiculo, bandera]  38 ChangeLawEnforcerState [esto, bandera]  39 0x6A2530 [esto, arg]  40 GetWantedLevel [esto]  41 FindPlayerPed [id]
// 42 PickNextNodeRandomly [vehiculo]
#define FAKE_NOINLINE __declspec(noinline)
FAKE_NOINLINE inline u32 __cdecl FakeCentre(u32 id) {Site(_ReturnAddress());Log(1,{id});After();return u32(centreA.Obj());}
FAKE_NOINLINE inline u32 __cdecl FakeCoors(u32 id) {Site(_ReturnAddress());Log(2,{id});After();return u32(coorsA.Obj());}
FAKE_NOINLINE inline uint8 __cdecl FakeFewer() {Site(_ReturnAddress());Log(3,{});const uint8 r=uint8(g.c->fewer);After();return r;}
FAKE_NOINLINE inline float __cdecl FakeMotorway() {Site(_ReturnAddress());Log(4,{});const float r=F(g.c->motor);After();return r;}
FAKE_NOINLINE inline u32 __cdecl FakeWanted(u32 id) {Site(_ReturnAddress());Log(5,{id});After();return u32(wantedA.Obj());}
FAKE_NOINLINE inline uint8 __cdecl FakeGang() {Site(_ReturnAddress());Log(6,{});const uint8 r=uint8(g.c->gang);After();return r;}
FAKE_NOINLINE inline u32 __cdecl FakePoliceModel(u32 a) {Site(_ReturnAddress());Log(7,{a});const u32 r=g.c->policeModel;After();return r;}
FAKE_NOINLINE inline u32 __cdecl FakeChooseModel(u32* out) {Site(_ReturnAddress());Log(8,{Tag(out)});*out=g.c->chosenPed;const u32 r=g.c->chooseModel;After();return r;}
FAKE_NOINLINE inline uint8 __cdecl FakeRiots() {Site(_ReturnAddress());Log(9,{});const uint8 r=uint8(g.c->riots);After();return r;}
FAKE_NOINLINE inline u32 __cdecl FakePlayerVehicle(u32 id,u32 flag) {Site(_ReturnAddress());Log(10,{id,flag});After();return g.c->pvPresent?u32(pvA.Obj()):0u;}
FAKE_NOINLINE inline uint8 __cdecl FakeCreationCoors(u32 a0,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32* origin,u16* na1,u16* na2,u32* f11,u32 a13,u32 a14) {
    Site(_ReturnAddress());
    Log(11,{a0,a1,a2,a3,a4,a5,a6,a7,a8,Tag(origin),Tag(na1),Tag(na2),Tag(f11),a13,a14});
    const C& c=*g.c;
    for(unsigned k=0;k<3;++k)origin[k]=c.ccOrigin[k];
    na1[0]=uint16(c.ccNa1[0]);na1[1]=uint16(c.ccNa1[1]);na2[0]=uint16(c.ccNa2[0]);na2[1]=uint16(c.ccNa2[1]);
    *f11=c.ccF11;
    const uint8 r=uint8(c.ccOk);
    After();
    return r;
}
FAKE_NOINLINE inline void __cdecl FakeRequestModel(u32 model,u32 flags) {Site(_ReturnAddress());Log(12,{model,flags});After();}
FAKE_NOINLINE inline u32 __fastcall FakePickLeast(void* self,void*,u32 arg) {Site(_ReturnAddress());Log(13,{Tag(self),arg});const u32 r=g.c->pickModel;After();return r;}
FAKE_NOINLINE inline void __cdecl FakeColliding(u32* point,u32 radius,u32 b2d,int16_t* count,u32 maxCount,u32 entities,u32 b6,u32 b7,u32 b8,u32 b9,u32 b10) {
    Site(_ReturnAddress());
    Log(14,{Tag(point),radius,b2d,Tag(count),maxCount,Tag(uintptr_t(entities)),b6&0xFFu,b7&0xFFu,b8&0xFFu,b9&0xFFu,b10&0xFFu});
    *count=int16_t(g.c->colCount);
    After();
}
FAKE_NOINLINE inline u32 __cdecl FakeZone(u32* pos,u32 flag) {Site(_ReturnAddress());Log(15,{Rd(pos),Rd(pos+1),Rd(pos+2),flag});After();return u32(zoneA.Obj());}
FAKE_NOINLINE inline u32 __cdecl FakeNewVehicle(u32 model,u32 flag) {
    Site(_ReturnAddress());
    Log(16,{model,flag});
    vehA.Put16(0x22,g.c->vehModelOverride?g.c->vehModelOverride:model);
    After();
    return u32(vehA.Obj());
}
FAKE_NOINLINE inline uint8 __cdecl FakeSpeed0() {Site(_ReturnAddress());Log(17,{});const uint8 r=uint8(g.c->spd[0]);After();return r;}
FAKE_NOINLINE inline uint8 __cdecl FakeSpeed1(u32 veh) {Site(_ReturnAddress());Log(18,{Tag(uintptr_t(veh))});const uint8 r=uint8(g.c->spd[1]);After();return r;}
FAKE_NOINLINE inline uint8 __cdecl FakeSpeed2() {Site(_ReturnAddress());Log(19,{});const uint8 r=uint8(g.c->spd[2]);After();return r;}
FAKE_NOINLINE inline uint8 __cdecl FakeSpeed3() {Site(_ReturnAddress());Log(20,{});const uint8 r=uint8(g.c->spd[3]);After();return r;}
FAKE_NOINLINE inline u32 __fastcall FakeAppearance(void* self,void*) {Site(_ReturnAddress());Log(21,{Tag(self)});const u32 r=g.c->appear;After();return r;}
FAKE_NOINLINE inline void __fastcall FakeColour(void* self,void*,uint8* a0,uint8* a1,uint8* a2,uint8* a3,u32 a4) {
    Site(_ReturnAddress());
    Log(22,{Tag(self),Tag(a0),Tag(a1),Tag(a2),Tag(a3),a4});
    *a0=uint8(g.c->colours[0]);*a1=uint8(g.c->colours[1]);*a2=uint8(g.c->colours[2]);*a3=uint8(g.c->colours[3]);
    After();
}
FAKE_NOINLINE inline void __cdecl FakeWorldAdd(u32 veh) {Site(_ReturnAddress());Log(23,{Tag(uintptr_t(veh))});After();}
FAKE_NOINLINE inline uint8 __cdecl FakeVertical(u32* origin,u32 dist,u32* colPoint,u32 entity,u32 b4,u32 b5,u32 b6,u32 b7,u32 b8,u32 b9,u32 poly) {
    Site(_ReturnAddress());
    Log(24,{Rd(origin),Rd(origin+1),Rd(origin+2),dist,Tag(colPoint),Tag(uintptr_t(entity)),b4&0xFFu,b5&0xFFu,b6&0xFFu,b7&0xFFu,b8&0xFFu,b9&0xFFu,Tag(uintptr_t(poly))});
    const bool second=g.vl++!=0;
    const u32* v=second?g.c->vl2:g.c->vl1;
    colPoint[2]=v[1];
    const uint8 r=uint8(v[0]);
    After();
    return r;
}
FAKE_NOINLINE inline uint8 __cdecl FakeWater(u32 x,u32 y,u32 z,u32* out,u32 touch,u32 normals) {
    Site(_ReturnAddress());
    Log(25,{x,y,z,Tag(out),touch&0xFFu,Tag(uintptr_t(normals))});
    *out=g.c->water[1];
    const uint8 r=uint8(g.c->water[0]);
    After();
    return r;
}
FAKE_NOINLINE inline uint8 __cdecl FakeIsBoat(u32 model) {Site(_ReturnAddress());Log(26,{model});const uint8 r=uint8(g.c->isBoat);After();return r;}
FAKE_NOINLINE inline void __fastcall FakeDtor(void* self,void*,u32 flags) {Site(_ReturnAddress());Log(27,{Tag(self),flags});After();}
FAKE_NOINLINE inline float __fastcall FakeHeight(void* self,void*) {Site(_ReturnAddress());Log(28,{Tag(self)});const float r=F(g.c->height);After();return r;}
FAKE_NOINLINE inline void __fastcall FakeSetPosn(void* self,void*,u32* pos) {
    Site(_ReturnAddress());
    Log(29,{Tag(self),Rd(pos),Rd(pos+1),Rd(pos+2)});
    if(vehA.Get32(0x14)) {matA.Put32(0x30,pos[0]);matA.Put32(0x34,pos[1]);matA.Put32(0x38,pos[2]);}
    else {vehA.Put32(4,pos[0]);vehA.Put32(8,pos[1]);vehA.Put32(0xC,pos[2]);}
    After();
}
FAKE_NOINLINE inline void __cdecl FakeClumpAlpha(u32 clump,u32 alpha) {Site(_ReturnAddress());Log(30,{clump,alpha});After();}
FAKE_NOINLINE inline void __fastcall FakeUpgrade(void* self,void*,u32 model) {Site(_ReturnAddress());Log(31,{Tag(self),model});After();}
FAKE_NOINLINE inline uint8 __fastcall FakeOnScreen(void* self,void*) {Site(_ReturnAddress());Log(32,{Tag(self)});const uint8 r=uint8(g.c->onScreen);After();return r;}
FAKE_NOINLINE inline void __cdecl FakeStreamOut(u32 model) {Site(_ReturnAddress());Log(33,{model});After();}
FAKE_NOINLINE inline uint8 __cdecl FakeChase(u32 veh,u32 a,u32 node) {Site(_ReturnAddress());Log(34,{Tag(uintptr_t(veh)),a,node});const uint8 r=uint8(g.c->chase);After();return r;}
FAKE_NOINLINE inline uint8 __cdecl FakeConvoy(u32 veh,u32 a) {Site(_ReturnAddress());Log(35,{Tag(uintptr_t(veh)),a});const uint8 r=uint8(g.c->convoy);After();return r;}
FAKE_NOINLINE inline void __cdecl FakeSetUpDriver(u32 veh,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5) {Site(_ReturnAddress());Log(36,{Tag(uintptr_t(veh)),a1,a2,a3&0xFFu,a4&0xFFu,a5&0xFFu});After();}
FAKE_NOINLINE inline void __cdecl FakePoliceOccupants(u32 veh,u32 flag) {Site(_ReturnAddress());Log(37,{Tag(uintptr_t(veh)),flag&0xFFu});After();}
FAKE_NOINLINE inline void __fastcall FakeLawState(void* self,void*,u32 flag) {Site(_ReturnAddress());Log(38,{Tag(self),flag&0xFFu});After();}
FAKE_NOINLINE inline void __fastcall FakeFn6A2530(void* self,void*,u32 arg) {Site(_ReturnAddress());Log(39,{Tag(self),arg&0xFFu});After();}
FAKE_NOINLINE inline u32 __fastcall FakeWantedLevel(void* self,void*) {Site(_ReturnAddress());Log(40,{Tag(self)});const u32 r=g.c->wantedLevel;After();return r;}
FAKE_NOINLINE inline u32 __cdecl FakePlayerPed(u32 id) {Site(_ReturnAddress());Log(41,{id});After();return u32(pedA.Obj());}
FAKE_NOINLINE inline void __cdecl FakePickNext(u32 veh) {
    Site(_ReturnAddress());
    Log(42,{Tag(uintptr_t(veh))});
    const C& c=*g.c;
    vehA.Put32(0x394,c.pn[0]);vehA.Put16(0x3A4,c.pn[1]);vehA.Put16(0x3A6,c.pn[2]);
    vehA.Put8(0x3B5,c.pn[3]&1u?1u:0xFFu);vehA.Put8(0x3B6,c.pn[4]&1u?1u:0xFFu);vehA.Put8(0x3B8,c.pn[5]&3u);
    vehA.Put32(0x39C,c.pn[6]);vehA.Put32(0x3A0,c.pn[7]);
    After();
}

// ================================================================================================ el caso
inline json ChangesOf(Arena<kVehArena>& a) {return a.Rest();}
inline void ApplyGlobals(const C& c) {
    for(size_t i=0;i<kGlobals.size();++i) {
        const auto& d=kGlobals[i];
        u32 v=c.g[i];
        if(i==gCamTarget)v=v?u32(tgtA.Obj()):0u;
        memcpy(reinterpret_cast<void*>(d.addr),&v,d.size);
    }
}
inline void CaseRandomCar(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    json sitesJson=json::array();
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.vehSeed);matA.Zero();centreA.Zero();coorsA.Zero();wantedA.Zero();pvA.Zero();zoneA.Zero();tgtA.Zero();pedA.Zero();vtA.Zero();
        for(unsigned i=0;i<12;++i)matA.Put32(4*i,c.mat[i]);
        for(unsigned i=0;i<3;++i) {centreA.Put32(4*i,c.centre[i]);coorsA.Put32(4*i,c.coors[i]);tgtA.Put32(0x30+4*i,c.camPos[i]);}
        wantedA.Put8(0x18,c.wB18);wantedA.Put8(0x19,c.wB19);wantedA.Put8(0x1A,c.wB1A);wantedA.Put32(0x2C,c.wLevel);
        pvA.Put32(0x44,c.pvSpeed[0]);pvA.Put32(0x48,c.pvSpeed[1]);
        zoneA.Put8(0xF,c.zoneFlags);
        pedA.Fill(c.seed*7u+9u);
        // el vehiculo: matriz, tabla virtual (destructor y altura sobre la carretera) y los campos que fija el caso
        vtA.Put32(0,u32(uintptr_t(&FakeDtor)));vtA.Put32(0xD4,u32(uintptr_t(&FakeHeight)));
        vehA.Put32(0,u32(vtA.Obj()));vehA.Put32(0x14,u32(matA.Obj()));
        vehA.Put8(0x36,c.veh[0]);vehA.Put8(0x42C,c.veh[1]);vehA.Put8(0x42D,c.veh[2]);vehA.Put8(0x42E,c.veh[3]);
        vehA.Put32(0x590,c.veh[4]);vehA.Put32(0x594,c.veh[5]);vehA.Put32(0x460,c.veh[6]?u32(pedA.Obj()):0u);
        vehA.Put16(0x4A6,c.veh[7]);vehA.Put32(0x4C0,c.veh[8]);vehA.Put32(0x18,c.veh[9]);
        // informacion de modelo: cuatro variantes con su modelo de colision
        for(unsigned v=0;v<kVariants;++v) {
            miA[v].Zero();colA[v].Zero();
            miA[v].Put32(0x14,u32(colA[v].Obj()));miA[v].Put32(0x3C,c.mi[v][0]);miA[v].Put8(0x50,c.mi[v][1]);
            colA[v].Put32(4,c.mi[v][2]);colA[v].Put32(0x10,c.mi[v][3]);colA[v].Put32(0x24,c.miR[v]);
        }
        // mundo de caminos
        for(auto id:{kRPathNodes,kRNaviNodes,kRNodeLinks,kRNaviLinks})memset(Mem(id),0,regions[id].bytes);
        nodeA.Zero();nlA.Zero();naviA.Zero();
        for(u32 k=0;k<kSlots;++k) {
            linkA[k].Zero();
            memcpy(linkA[k].m.data()+kGuard,c.links[k],kLinks*kLinkBytes);
            for(u32 i=0;i<kNodes;++i) {
                const size_t o=(size_t(k)*kNodes+i)*kNodeBytes;
                nodeA.Put16(o+8,c.nodeW[k][i][0]);nodeA.Put16(o+0xA,c.nodeW[k][i][1]);nodeA.Put16(o+0xC,c.nodeW[k][i][2]);nodeA.Put16(o+0x10,c.nodeW[k][i][3]);
                nodeA.Put8(o+0x18,c.nodeW[k][i][4]);nodeA.Put8(o+0x19,c.nodeW[k][i][5]);nodeA.Put8(o+0x1A,c.nodeW[k][i][6]);
            }
            for(u32 j=0;j<kNL;++j) {nlA.Put32((size_t(k)*kNL+j)*4,c.nl[k][j]);naviA.Put16((size_t(k)*kNL+j)*2,c.navi[k][j]);}
            SetPtrR(kRPathNodes,c.areas[k],nodeA.Obj()+size_t(k)*kNodes*kNodeBytes);SetPtrR(kRNaviNodes,c.areas[k],linkA[k].Obj());
            SetPtrR(kRNodeLinks,c.areas[k],nlA.Obj()+size_t(k)*kNL*4);SetPtrR(kRNaviLinks,c.areas[k],naviA.Obj()+size_t(k)*kNL*2);
        }
        // tablas por modelo: informacion de modelo (puntero a una de las cuatro variantes) y primer byte de la informacion de streaming
        for(size_t i=0;i<kModels.size();++i) {
            const u32 m=kModels[i];
            G32(0xA9B0C8+4*m)=u32(miA[i%kVariants].Obj());
            *reinterpret_cast<uint8*>(0x8E4CD0+20*m)=uint8(c.tbl[i]);
        }
        ApplyGlobals(c);
        vehA.Snap();matA.Snap();pedA.Snap();for(auto& a:miA)a.Snap();for(auto& a:colA)a.Snap();
        TagClear();vehA.Tagged();matA.Tagged();centreA.Tagged();coorsA.Tagged();wantedA.Tagged();pvA.Tagged();zoneA.Tagged();tgtA.Tagged();pedA.Tagged();vtA.Tagged();
        for(auto& a:miA)a.Tagged();for(auto& a:colA)a.Tagged();for(auto& a:linkA)a.Tagged();nodeA.Tagged();nlA.Tagged();naviA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g=State{};g.c=&c;g.evHash=2166136261u;g.side=s;nev=0;nsites=0;
        reinterpret_cast<void(__cdecl*)(unsigned)>(0x821B11)(c.seed);   // srand del CRT del EXE
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,c.ecx,c.edx,c.stack,0);});
        json obs;
        obs["veh"]=vehA.Rest();
        obs["mat"]=matA.Rest();
        obs["ped"]=pedA.Rest();
        {json mi=json::array();for(auto& a:miA)mi.push_back(a.Rest());obs["mi"]=mi;}
        obs["ev_hash"]=g.evHash;
        obs["ev_count"]=nev;
        obs["vl_calls"]=g.vl;
        obs["events"]=EvsJson();
        if(s==0){for(unsigned i=0;i<nsites;++i)sitesJson.push_back(sites[i]);}
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides,json{{"sites",sitesJson}});
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
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseRandomCar(env,totals,c);
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
// Colaborador falseado: si la DLL tiene hook RH sobre esa direccion se apaga (la entrada del EXE es codigo original); la DLL llama a la direccion del EXE, asi que solo se parchea el EXE (dll = 0)
inline Pair MakeCollab(Env& env,uintptr_t exe,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr}});
    return p;
}
// Funcion que corre de verdad: con hook RH en la DLL se apaga (EXE y DLL ejecutan los bytes del EXE); sin hook, solo se anota
inline void MakeReal(Env& env,uintptr_t exe,json& hooksFound) {
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name},{"real",true}});
    }
    else hooksFound.push_back({{"exe",exe},{"class",nullptr},{"name",nullptr},{"real",true}});
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"car84l-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84l-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84l");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
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
    struct Fk {uintptr_t addr;void* fn;};
    const Fk fakes[]={
        {0x56E250,Void(&FakeCentre)},{0x56E090,Void(&FakeCoors)},{0x72DD80,Void(&FakeFewer)},{0x611B60,Void(&FakeMotorway)},{0x56E230,Void(&FakeWanted)},{0x443AC0,Void(&FakeGang)},
        {0x421980,Void(&FakePoliceModel)},{0x424CE0,Void(&FakeChooseModel)},{0x441C10,Void(&FakeRiots)},{0x56E0D0,Void(&FakePlayerVehicle)},{0x424210,Void(&FakeCreationCoors)},
        {0x4087E0,Void(&FakeRequestModel)},{0x611E90,Void(&FakePickLeast)},{0x568B80,Void(&FakeColliding)},{0x572400,Void(&FakeZone)},{0x421440,Void(&FakeNewVehicle)},
        {0x41CA50,Void(&FakeSpeed0)},{0x41CAA0,Void(&FakeSpeed1)},{0x41CA40,Void(&FakeSpeed2)},{0x41C9D0,Void(&FakeSpeed3)},{0x6D1080,Void(&FakeAppearance)},{0x4C8500,Void(&FakeColour)},
        {0x563220,Void(&FakeWorldAdd)},{0x5674E0,Void(&FakeVertical)},{0x6EB690,Void(&FakeWater)},{0x4C5A70,Void(&FakeIsBoat)},{0x4241C0,Void(&FakeSetPosn)},{0x732B00,Void(&FakeClumpAlpha)},
        {0x6E3290,Void(&FakeUpgrade)},{0x534540,Void(&FakeOnScreen)},{0x40BA70,Void(&FakeStreamOut)},{0x42C2B0,Void(&FakeChase)},{0x42C740,Void(&FakeConvoy)},{0x4217C0,Void(&FakeSetUpDriver)},
        {0x41C070,Void(&FakePoliceOccupants)},{0x6D2330,Void(&FakeLawState)},{0x6A2530,Void(&FakeFn6A2530)},{0x41BE60,Void(&FakeWantedLevel)},{0x56E210,Void(&FakePlayerPed)},{0x42DE80,Void(&FakePickNext)}};
    for(const auto& fk:fakes)env.stubs[0].push_back(S(MakeCollab(env,fk.addr,hooksFound),fk.fn));
    // Con el hook apagado y sin falso: los bytes del EXE corren en los dos lados (la DLL llama a la direccion del EXE)
    static constexpr uintptr_t reals[]{0x407180,0x41BD90,0x4082C0,0x40FE30,0x40FE60,0x40FEC0,0x4119D0,0x59C910,0x41CCB0,0x420800,0x420920,0x420950,0x420980,0x420A10,0x420A60,0x421120,0x424130,0x43C710,0x43C900,0x44DB00};
    for(const uintptr_t a:reals)MakeReal(env,a,hooksFound);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"srand/rand/_ftol2 del CRT del EXE","GetRandomNumberInRange int y float (0x407180, 0x41BD90), vectores 0x40FE30/0x40FE60/0x40FEC0/0x4119D0, 0x4082C0, 0x59C910, 0x41CCB0, 0x420800, 0x420920/0x420950/0x420980, 0x420A10/0x420A60, 0x421120, 0x424130, 0x43C710, 0x43C900, 0x44DB00 (bytes del EXE en los dos lados)","mundo de caminos y modelos: arenas del banco enlazadas desde ThePaths y la tabla de modelos"})},
            {"virtual_fakes",json::array({"destructor virtual y GetHeightAboveRoad (+0xD4) del vehiculo"})},{"real_entries",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(const uintptr_t a:reals)env.watches.emplace_back(a,16);
    for(const uintptr_t a:{uintptr_t(0x821B1E),uintptr_t(0x821B40),uintptr_t(0x821B11)})env.watches.emplace_back(a,16);
    // constantes de .rdata que lee el EXE (float): se vigilan sus bytes
    for(const uintptr_t a:{uintptr_t(0x858C48),uintptr_t(0x858624),uintptr_t(0x858B14),uintptr_t(0x858C7C),uintptr_t(0x858B8C),uintptr_t(0x858B50),uintptr_t(0x858C50),uintptr_t(0x858C58),uintptr_t(0x859010)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
