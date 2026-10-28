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
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Veh80A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion de CVehicle (hook S): DoDriveByShootings (0x741FD0). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL:
// CPlayerPed::GetPlayerInfoForThisPlayerPed 0x609FF0, CPlayerPed::GetPadFromPlayer 0x609560, CWeaponInfo::GetWeaponInfo 0x743C60, CPad::GetCarGunFired 0x53FF90,
// CPad::GetLookLeft 0x53FDD0, CPad::GetLookRight 0x53FE10, CWeapon::Update 0x73DB40, RpAnimBlendClumpGetAssociation 0x4D68B0, CAnimManager::BlendAnimation 0x4D4610,
// CWeapon::FireFromCar 0x73FA20 y CPed::DoGunFlash 0x5DF340. GetRideAnimData (virtual, ranura 47 = +0xBC) es un falso en la vtabla del vehiculo de la arena.

namespace Ve80Live {
using namespace Ve80Fix;
inline constexpr uintptr_t kEntry[kFunctions]{0x741FD0};
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uint16& G16(uintptr_t a) {return *reinterpret_cast<uint16*>(a);}
inline uint8& G8(uintptr_t a) {return *reinterpret_cast<uint8*>(a);}

// ================================================================================================ arenas
template<size_t N, size_t OBJ> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj() const {return uintptr_t(m.data())+OBJ;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+OBJ+off,&v,4);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+OBJ+off,&w,2);}
    void Put8(size_t off,u32 v) {m[OBJ+off]=uint8(v);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,OBJ);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kThisArena,kGuard> thisA;
inline Arena<kDrvArena,kDrvObj> drvA;
inline Arena<kInfoArena,kGuard> infoA,infoB;
inline Arena<kPInfoArena,kGuard> pinfoA;
inline Arena<kRideArena,kGuard> rideA,rideB;
inline std::array<Arena<kAssocArena,kGuard>,4> assocA;
inline Arena<kPadArena,kGuard> padA;

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
inline void SaveRegions() {
    regions.clear();
    regions.emplace_back(kCamRegion1,kCamRegion1Bytes);   // TheCamera: bandera 0xB6F057 y camara activa 0xB6F081
    regions.emplace_back(kCamArray,kCamRegion2Bytes);     // las tres camaras (modo +0xC, mirar a izquierda/derecha +8/+9)
    regions.emplace_back(kTimer,4);                        // CTimer::m_snTimeInMilliseconds
}

// ================================================================================================ falsos de los colaboradores
struct State {const C* c{};u32 evHash{};u32 getCalls{};u32 infoCalls{};u32 rideCalls{};uintptr_t weapon{};bool unexpectedVirtual{};};
inline State g;
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
inline uintptr_t CamBase(u32 idx) {return kCamArray+uintptr_t(idx)*kCamSize;}
// retKind: 0 nada, 1 CWeaponInfo, 2 CRideAnimData, 3 CAnimBlendAssociation (los objetivos 16..18 modifican el objeto que el falso devuelve)
inline void ApplyMut(const Mut& m,uintptr_t ret,int retKind) {
    const u32 v=m.value;
    auto wput=[&](size_t off){memcpy(reinterpret_cast<void*>(g.weapon+off),&v,4);};
    switch(m.target) {
    case 0: wput(0x0);break;
    case 1: wput(0xC);break;
    case 2: wput(0x10);break;
    case 15: wput(0x8);break;
    case 3: G32(kTimer)=v;break;
    case 4: drvA.Put32(0x18,v);break;
    case 5: drvA.Put8(0x718,v);break;
    case 6: thisA.Put32(0x460,v);break;
    case 7: thisA.Put32(0x594,v);break;
    case 8: thisA.Put8(0x429,v);break;
    case 9: G8(kCamActive)=uint8(v%3);break;
    case 10: {const u32 a=G8(kCamActive);if(a<=2)G32(CamBase(a)+0xC)=v;break;}
    case 11: G8(kCamFlag)=uint8(v);break;
    case 12: {const u32 a=G8(kCamActive);if(a<=2)G8(CamBase(a)+8)=uint8(v);break;}
    case 13: {const u32 a=G8(kCamActive);if(a<=2)G8(CamBase(a)+9)=uint8(v);break;}
    case 14: pinfoA.Put8(0x153,v);break;
    case 16: if(ret) {if(retKind==1)G32(ret+0x14)=v;else if(retKind==2)G32(ret)=v;else if(retKind==3)G16(ret+0x2E)=uint16(v);}break;
    case 17: if(ret) {if(retKind==1)G16(ret+0x20)=uint16(v);else if(retKind==3)G32(ret+0x18)=v;}break;
    case 18: if(ret) {if(retKind==3)G32(ret+0x1C)=v;}break;
    default: break;
    }
}
// Cierra un evento: lo incorpora al hash y aplica las mutaciones programadas tras el (antes de que el falso devuelva el control)
inline void Done(Ev& e,uintptr_t ret=0,int retKind=0) {
    const unsigned idx=nev-1;
    HashEv(e);
    for(u32 i=0;i<g.c->nmut;++i)if(g.c->mut[i].after==idx)ApplyMut(g.c->mut[i],ret,retKind);
}
inline uintptr_t AssocOf(u32 sel) {return sel>=1&&sel<=4?assocA[sel-1].Obj():0;}
inline uintptr_t InfoOf(u32 callNo) {
    const u32 map=g.c->info_map;
    const bool second=callNo>=1;
    const bool useA=second?!(map==0||map==3):(map<=1);
    return useA?infoA.Obj():infoB.Obj();
}

// 0x609FF0: CPlayerPed::GetPlayerInfoForThisPlayerPed(), thiscall sin argumentos; devuelve el puntero (o nulo).
inline u32 __fastcall FakePlayerInfo(void* ped,void*) {
    Ev& e=PushEv(1,1);e.v[0]=Tag(ped);
    const u32 r=g.c->pinfo_null?0u:u32(pinfoA.Obj());
    Done(e);
    return r;
}
// 0x609560: CPlayerPed::GetPadFromPlayer(), thiscall sin argumentos.
inline u32 __fastcall FakePad(void* ped,void*) {
    Ev& e=PushEv(2,1);e.v[0]=Tag(ped);
    const u32 r=g.c->pad_null?0u:u32(padA.Obj());
    Done(e);
    return r;
}
// 0x743C60: CWeaponInfo::GetWeaponInfo(type, skill), cdecl; la primera llamada y la segunda pueden devolver informaciones distintas.
inline u32 __cdecl FakeWeaponInfo(u32 type,u32 skill) {
    Ev& e=PushEv(3,3);e.v[0]=type;e.v[1]=skill;e.v[2]=g.infoCalls;
    const uintptr_t obj=InfoOf(g.infoCalls);
    ++g.infoCalls;
    Done(e,obj,1);
    return u32(obj);
}
// 0x53FF90 / 0x53FDD0 / 0x53FE10: CPad::GetCarGunFired (devuelve int16 en AX), GetLookLeft y GetLookRight (bool en AL); el resto de EAX es basura del caso.
inline u32 __fastcall FakeGunFired(void* pad,void*) {Ev& e=PushEv(4,1);e.v[0]=Tag(pad);const u32 r=g.c->gun_ret;Done(e);return r;}
inline u32 __fastcall FakeLookLeft(void* pad,void*) {Ev& e=PushEv(5,1);e.v[0]=Tag(pad);const u32 r=g.c->ll_ret;Done(e);return r;}
inline u32 __fastcall FakeLookRight(void* pad,void*) {Ev& e=PushEv(6,1);e.v[0]=Tag(pad);const u32 r=g.c->lr_ret;Done(e);return r;}
// 0x73DB40: CWeapon::Update(CPed* owner), thiscall con un argumento (`ret 4`).
inline void __fastcall FakeUpdate(void* weapon,void*,u32 owner) {Ev& e=PushEv(7,2);e.v[0]=Tag(weapon);e.v[1]=owner;Done(e);}
// 0x4D68B0: RpAnimBlendClumpGetAssociation(clump, animId), cdecl.
inline u32 __cdecl FakeGetAssoc(u32 clump,u32 animId) {
    Ev& e=PushEv(8,3);e.v[0]=clump;e.v[1]=animId;e.v[2]=g.getCalls;
    const uintptr_t obj=AssocOf(g.c->get_sel[g.getCalls<2?g.getCalls:2]);
    ++g.getCalls;
    Done(e,obj,3);
    return u32(obj);
}
// 0x4D4610: CAnimManager::BlendAnimation(clump, groupId, animId, blend), cdecl.
inline u32 __cdecl FakeBlend(u32 clump,u32 group,u32 anim,u32 blend) {
    Ev& e=PushEv(9,4);e.v[0]=clump;e.v[1]=group;e.v[2]=anim;e.v[3]=blend;
    const uintptr_t obj=AssocOf(g.c->blend_sel);
    Done(e,obj,3);
    return u32(obj);
}
// 0x73FA20: CWeapon::FireFromCar(vehicle, left, right), thiscall con tres argumentos (`ret 0Ch`); el EXE empuja dwords con basura en los bytes altos de las banderas: solo se registra el byte bajo.
inline u32 __fastcall FakeFire(void* weapon,void*,u32 vehicle,u32 left,u32 right) {
    Ev& e=PushEv(10,4);e.v[0]=Tag(weapon);e.v[1]=Tag(reinterpret_cast<const void*>(uintptr_t(vehicle)));e.v[2]=left&0xFFu;e.v[3]=right&0xFFu;
    Done(e);
    return 0xA5A5A501u;
}
// 0x5DF340: CPed::DoGunFlash(duration, bRightHand), thiscall con dos argumentos (`ret 8`).
inline u32 __fastcall FakeFlash(void* ped,void*,u32 duration,u32 hand) {
    Ev& e=PushEv(11,3);e.v[0]=Tag(ped);e.v[1]=duration;e.v[2]=hand&0xFFu;
    Done(e);
    return 0x5A5A5A00u;
}
// CVehicle::GetRideAnimData (virtual, ranura 47 = +0xBC de la vtabla), thiscall sin argumentos. La primera llamada decide si hay datos; la segunda da el grupo.
inline u32 __fastcall FakeRide(void* vehicle,void*) {
    Ev& e=PushEv(12,2);e.v[0]=Tag(vehicle);e.v[1]=g.rideCalls;
    const u32 sel=g.rideCalls==0?g.c->ride1:g.c->ride2;
    ++g.rideCalls;
    const uintptr_t obj=sel==1?rideA.Obj():sel==2?rideB.Obj():0;
    Done(e,obj,2);
    return u32(obj);
}
// Cualquier otra ranura virtual del vehiculo: no deberia llamarse (se registra y se devuelve 0)
inline u32 __fastcall FakeUnexpectedVirtual(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);g.unexpectedVirtual=true;HashEv(e);return 0;}
inline std::array<u32,64> vtbl{};
inline void InitVtbl() {
    for(auto& v:vtbl)v=u32(reinterpret_cast<uintptr_t>(&FakeUnexpectedVirtual));
    vtbl[47]=u32(reinterpret_cast<uintptr_t>(&FakeRide));
}

// ------------------------------------------------------------------------------------------------ microtests de la x87 (hardware): FCOM/FCOMP frente al modelo del oraculo
// FSW & 0x4500 tras `fld a; fcomp b`: 0x0000 si a > b, 0x0100 si a < b, 0x4000 si a == b, 0x4500 si no son ordenables (NaN)
inline u32 X87Cmp(u32 a,u32 b) {
    uint16 sw{};
    __asm {fld dword ptr a} __asm {fcomp dword ptr b} __asm {fnstsw word ptr sw}
    return u32(sw)&0x4500u;
}
inline json MicroTests() {
    json out=json::array();
    PrepareFloatingState();
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0x7F800001u,0xFFA00003u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x3F7D70A4u,0x3F7D70A3u,0x3F7D70A5u,
        0xC1000000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x7F7FFFFFu,0xFF7FFFFFu,0x3F000000u};
    for(u32 a:vals)for(u32 b:vals)out.push_back({0,a,b,X87Cmp(a,b)});
    RestoreFloatingState();
    return out;
}

// ================================================================================================ el caso
inline void CaseDriveBy(Env& env,Totals& t,unsigned fnIdx,unsigned n) {
    const C c=MakeC(fnIdx,n);
    json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        thisA.Fill(c.seed*13u+1u);drvA.Fill(c.seed*13u+2u);infoA.Fill(c.seed*13u+3u);infoB.Fill(c.seed*13u+4u);pinfoA.Fill(c.seed*13u+5u);rideA.Fill(c.seed*13u+6u);rideB.Fill(c.seed*13u+7u);
        for(unsigned i=0;i<4;++i)assocA[i].Fill(c.seed*13u+8u+i);
        padA.Fill(c.seed*13u+12u);
        // vehiculo
        thisA.Put32(0,u32(uintptr_t(vtbl.data())));
        thisA.Put32(0x460,c.has_driver?u32(drvA.Obj()):0u);
        thisA.Put32(0x594,c.vsub);
        thisA.Put8(0x429,c.vflags);
        // conductor y arma (la ranura se extiende con signo)
        drvA.Put32(0x18,c.clump);
        drvA.Put8(0x718,c.slot);
        g.weapon=drvA.Obj()+kWeaponBase+size_t(int64(int8(c.slot))*int64(kWeaponSize));
        {const u32 w[5]{c.wtype,c.wstate,c.clip0,c.total,c.nextshot};memcpy(reinterpret_cast<void*>(g.weapon),w,sizeof(w));}
        pinfoA.Put8(0x153,c.can_drive_by);
        // informaciones del arma: la que devuelve la primera llamada trae el slot; la de la segunda, el tamano del cargador; los demas campos conservan valores del caso
        {
            const bool firstA=c.info_map<=1,secondA=!(c.info_map==0||c.info_map==3);
            Arena<kInfoArena,kGuard>& first=firstA?infoA:infoB;
            Arena<kInfoArena,kGuard>& second=secondA?infoA:infoB;
            for(auto* a:{&infoA,&infoB}) {a->Put32(0x14,c.infob_slot);a->Put16(0x20,c.infoa_clip);}
            first.Put32(0x14,c.info1_slot);
            second.Put16(0x20,c.info2_clip);
        }
        rideA.Put32(0,c.ride_a_group);rideB.Put32(0,c.ride_b_group);
        for(unsigned i=0;i<4;++i) {assocA[i].Put16(0x2E,c.assoc[i].flags);assocA[i].Put32(0x18,c.assoc[i].amount);assocA[i].Put32(0x1C,c.assoc[i].delta);}
        // globales: camara, bandera 0xB6F057, reloj
        G8(kCamActive)=uint8(c.cam_active);G8(kCamFlag)=uint8(c.cam_flag);
        for(unsigned i=0;i<3;++i) {G32(CamBase(i)+0xC)=c.cam_mode[i];G8(CamBase(i)+8)=uint8(c.cam_ll[i]);G8(CamBase(i)+9)=uint8(c.cam_lr[i]);}
        G32(kTimer)=c.now;
        thisA.Snap();drvA.Snap();infoA.Snap();infoB.Snap();pinfoA.Snap();rideA.Snap();rideB.Snap();for(auto& a:assocA)a.Snap();padA.Snap();
        TagClear();thisA.Tagged();drvA.Tagged();infoA.Tagged();infoB.Tagged();pinfoA.Tagged();rideA.Tagged();rideB.Tagged();for(auto& a:assocA)a.Tagged();padA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g.c=&c;g.evHash=2166136261u;g.getCalls=0;g.infoCalls=0;g.rideCalls=0;g.unexpectedVirtual=false;nev=0;
        if(s==0) {   // lo que la funcion lee de la camara activa (indice sin comprobar): tambien fuera de las tres camaras
            const u32 a=G8(kCamActive);
            input["cam_read"]=json::array({G16(CamBase(a)+0xC),G8(CamBase(a)+8),G8(CamBase(a)+9)});
        }
        const u32 dummy[1]{};
        sides[s]=Execute(env,fnIdx,kEntry[fnIdx],candidate,[&](uintptr_t entry){return Gate(entry,dummy,0,c.eax,u32(thisA.Obj()),c.edx,c.stack,0);});
        json obs;   // void: se observan la traza, los bytes cambiados de las arenas, el reloj y la camara
        obs["ev_hash"]=g.evHash;
        obs["nev"]=nev;
        obs["unexpected_virtual"]=g.unexpectedVirtual;
        obs["events"]=EvsJson();
        obs["timer"]=G32(kTimer);
        {json cam=json::array({G8(kCamActive),G8(kCamFlag)});for(unsigned i=0;i<3;++i){cam.push_back(G32(CamBase(i)+0xC));cam.push_back(G8(CamBase(i)+8));cam.push_back(G8(CamBase(i)+9));}obs["cam"]=cam;}
        obs["changes"]=json::array({thisA.Changes(),drvA.Changes(),infoA.Changes(),infoB.Changes(),pinfoA.Changes(),rideA.Changes(),rideB.Changes(),
                                    assocA[0].Changes(),assocA[1].Changes(),assocA[2].Changes(),assocA[3].Changes(),padA.Changes()});
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[fnIdx],n,input,rows,sides);
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
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"vtable_slot",47},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseDriveBy(env,totals,f,c);
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
    if(!std::filesystem::exists(dir/"veh80a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"veh80a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("veh80a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    InitVtbl();
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    for(unsigned f=0;f<kFunctions;++f) {
        std::string cat,name;
        const bool found=FindHook(kEntry[f],cat,name);
        if(found)env.route[f].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=found&&env.route[f]->Registered();bodies[kFn[f]]=found&&env.route[f]->Body()!=0;
        ready&=found&&env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[f]]=found?json({{"class",cat},{"name",name},{"address",kEntry[f]}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    const Pair pInfo=MakeCollab(env,0x609FF0,hooksFound);
    const Pair pPad=MakeCollab(env,0x609560,hooksFound);
    const Pair pWI=MakeCollab(env,0x743C60,hooksFound);
    const Pair pGun=MakeCollab(env,0x53FF90,hooksFound);
    const Pair pLL=MakeCollab(env,0x53FDD0,hooksFound);
    const Pair pLR=MakeCollab(env,0x53FE10,hooksFound);
    const Pair pUpd=MakeCollab(env,0x73DB40,hooksFound);
    const Pair pGet=MakeCollab(env,0x4D68B0,hooksFound);
    const Pair pBlend=MakeCollab(env,0x4D4610,hooksFound);
    const Pair pFire=MakeCollab(env,0x73FA20,hooksFound);
    const Pair pFlash=MakeCollab(env,0x5DF340,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[kDriveBy]={S(pInfo,reinterpret_cast<void*>(&FakePlayerInfo)),S(pPad,reinterpret_cast<void*>(&FakePad)),S(pWI,reinterpret_cast<void*>(&FakeWeaponInfo)),
        S(pGun,reinterpret_cast<void*>(&FakeGunFired)),S(pLL,reinterpret_cast<void*>(&FakeLookLeft)),S(pLR,reinterpret_cast<void*>(&FakeLookRight)),
        S(pUpd,reinterpret_cast<void*>(&FakeUpdate)),S(pGet,reinterpret_cast<void*>(&FakeGetAssoc)),S(pBlend,reinterpret_cast<void*>(&FakeBlend)),
        S(pFire,reinterpret_cast<void*>(&FakeFire)),S(pFlash,reinterpret_cast<void*>(&FakeFlash))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(unsigned f=0;f<kFunctions;++f)env.watches.emplace_back(kEntry[f],16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
