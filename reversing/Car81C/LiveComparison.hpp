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
#include "General.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car81C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::PossiblyRemoveVehicle (0x424F80, hook S, cdecl con un argumento). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado
// durante el banco): CVehicle::CanBeDeleted 0x6D1180 (thiscall), CCranes::IsThisCarBeingTargettedByAnyCrane 0x6F4000, CVisibilityPlugins::GetClumpAlpha 0x732B20, FindPlayerCentreOfWorld 0x56E250,
// CEntity::GetIsOnScreen 0x534540 (thiscall), CCamera::GetLookDirection 0x50AE90 (thiscall), CGarages::IsPointWithinHideOutGarage 0x448900, CCarCtrl::IsThisVehicleInteresting 0x423EA0,
// CTrafficLights::ShouldCarStopForLight 0x49D610 / ShouldCarStopForBridge 0x49D420 y CWorld::Remove 0x563280. El destructor virtual (flag 1) va a una tabla virtual falsa (ranura 0).
// Los falsos registran sus argumentos EN ORDEN y pueden mutar el vehiculo, las matrices, el centro, el reloj o la camara para comprobar que la funcion relee esos campos donde el EXE los relee.
// Los helpers x87 del EXE (0x420800, 0x40FE60, 0x406DA0) y los limites de .rdata corren de verdad en el lado EXE (se vigilan sus bytes).

namespace C81Layout {
static_assert(sizeof(CVehicle) == 0x5A0 && C81Fix::kVehWin == sizeof(CVehicle), "la arena del vehiculo es un CVehicle completo");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: matriz en +0x14");
}

namespace C81Live {
using namespace C81Fix;
inline constexpr uintptr_t kEntry=0x424F80;
inline constexpr uintptr_t kCbd=0x6D1180,kCrane=0x6F4000,kAlpha=0x732B20,kCentre=0x56E250,kOnScreen=0x534540,kLook=0x50AE90,kGarage=0x448900,kInteresting=0x423EA0,kLight=0x49D610,kBridge=0x49D420,kRemove=0x563280;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uint8& G8(uintptr_t a) {return *reinterpret_cast<uint8*>(a);}
inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}

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
    // Cambios respecto a la imagen de referencia, sin los campos que se comparan por valor (mask = [[desplazamiento del objeto, bytes]])
    json Changes(std::initializer_list<std::pair<size_t,size_t>> mask={}) const {
        auto a=m,b=r;
        for(const auto& [off,len]:mask)for(size_t i=0;i<len;++i){a[kGuard+off+i]=0;b[kGuard+off+i]=0;}
        return ArenaChanges(a.data(),b.data(),N,kGuard);
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA,matB;
inline Arena<kCenArena> cenA;
inline Arena<kClumpArena> clumpA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["time"]=G32(kTimeAddr);
    obs["cam_mult"]=G32(kCamMultAddr);obs["cam_low"]=G32(kCamLowAddr);obs["active_cam"]=G8(kCamActiveAddr);
    json cams=json::array();for(unsigned i=0;i<3;++i)for(unsigned k=0;k<3;++k)cams.push_back(G8(kCamsBase+i*kCamStride+7+k));
    obs["cams"]=cams;obs["focus"]=G8(kFocusAddr);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ PossiblyRemoveVehicle (0x424F80)
struct State {const C* c{};unsigned ev{},cbd{},crane{},alpha{},on{},look{},gar{},inter{},light{},bridge{};};
inline State g;
inline u32 ClumpPtr(u32 code) {
    switch(code) {
    case 0: return 0u;
    case 1: return u32(clumpA.Obj());
    default: return kGarbageBase+0x20u;
    }
}
inline u32 Now() {return G32(kTimeAddr);}
// Derivados de mutVal para los tres floats de una posicion (el oraculo repite la formula)
inline u32 Dv(u32 v,u32 i) {return v^(i*0x01234567u);}
// Tras el evento numero mutAt, el falso muta el vehiculo, una matriz, el centro, el reloj o la camara
inline void ApplyMut(u32 kind,u32 v) {
    static constexpr u32 vtypes[8]{0u,3u,4u,5u,9u,0xFFFFFFFFu,0x80000003u,1u};
    switch(kind) {
    case 1: vehA.Put32(0x14,u32(matB.Obj()));break;
    case 2: G32(kTimeAddr)=v;break;
    case 3: vehA.Put8(0x428,v);break;
    case 4: vehA.Put8(0x42A,v);break;
    case 5: vehA.Put8(0x42B,v);break;
    case 6: vehA.Put8(0x42C,v);break;
    case 7: vehA.Put8(0x36,v);break;
    case 8: vehA.Put32(0x594,vtypes[v%8u]);break;
    case 9: vehA.Put8(0x3B9,v%8u);break;
    case 10: vehA.Put8(0x3BB,v);break;
    case 11: vehA.Put8(0x3BA,v);break;
    case 12: vehA.Put32(0x3B0,v);break;
    case 13: vehA.Put32(0x4D0,v);break;
    case 14: vehA.Put32(0x4D8,v);break;
    case 15: for(u32 i=0;i<3;++i){matA.Put32(0x30+4*i,Dv(v,i));matB.Put32(0x30+4*i,Dv(v,i+3));}break;
    case 16: for(u32 i=0;i<3;++i)vehA.Put32(4+4*i,Dv(v,i));break;
    case 17: for(u32 i=0;i<3;++i)cenA.Put32(4*i,Dv(v,i));break;
    case 18: matA.Put32(0x28,v);matB.Put32(0x28,Dv(v,1));break;
    case 19: vehA.Put8(0x4A4,v);break;
    case 20: vehA.Put16(0x4A6,v);break;
    case 21: vehA.Put16(0x22,v);break;
    case 22: vehA.Put32(0x1C,v);break;
    case 23: G32(kCamMultAddr)=v;break;
    case 24: G32(kCamLowAddr)=v;break;
    case 25: G8(kCamActiveAddr)=uint8(v%3u);break;
    case 26: G8(kCamsBase+uintptr_t(G8(kCamActiveAddr))*kCamStride+7+((v>>8)%3u))=uint8(v);break;
    case 27: vehA.Put32(0x18,ClumpPtr(v%3u));break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
inline u32 Seq(const u32* a,unsigned n,unsigned& i) {const u32 r=a[i<n?i:n-1];++i;return r;}
// Eventos: 1 CanBeDeleted [vehiculo, hora]; 2 grua [vehiculo, hora]; 3 GetClumpAlpha [clump, hora]; 4 FindPlayerCentreOfWorld [jugador, hora]; 5 GetIsOnScreen [vehiculo, hora]; 6 GetLookDirection [camara, hora];
// 7 IsPointWithinHideOutGarage [x, y, z, hora]; 8 IsThisVehicleInteresting [vehiculo, hora]; 9 ShouldCarStopForLight [vehiculo, bandera, hora]; 10 ShouldCarStopForBridge [vehiculo, hora];
// 11 CWorld::Remove [entidad, hora]; 12 destructor virtual [vehiculo, flag, hora]
inline u32 __fastcall FakeCbd(void* self,void*) {Ev& e=PushEv(1,2);e.v[0]=Tag(self);e.v[1]=Now();const u32 r=Seq(g.c->rcbd,2,g.cbd);After();return r;}
inline u32 __cdecl FakeCrane(void* veh) {Ev& e=PushEv(2,2);e.v[0]=Tag(veh);e.v[1]=Now();const u32 r=Seq(&g.c->rcrane,1,g.crane);After();return r;}
inline u32 __cdecl FakeAlpha(void* clump) {Ev& e=PushEv(3,2);e.v[0]=Tag(clump);e.v[1]=Now();const u32 r=Seq(&g.c->ralpha,1,g.alpha);After();return r;}
inline void* __cdecl FakeCentre(int player) {Ev& e=PushEv(4,2);e.v[0]=u32(player);e.v[1]=Now();After();return reinterpret_cast<void*>(cenA.Obj());}
inline u32 __fastcall FakeOnScreen(void* self,void*) {Ev& e=PushEv(5,2);e.v[0]=Tag(self);e.v[1]=Now();const u32 r=Seq(g.c->ron,4,g.on);After();return r;}
inline u32 __fastcall FakeLook(void* self,void*) {Ev& e=PushEv(6,2);e.v[0]=u32(uintptr_t(self));e.v[1]=Now();const u32 r=Seq(&g.c->rlook,1,g.look);After();return r;}
inline u32 __cdecl FakeGarage(const u32* p) {Ev& e=PushEv(7,4);e.v[0]=p[0];e.v[1]=p[1];e.v[2]=p[2];e.v[3]=Now();const u32 r=Seq(g.c->rgar,3,g.gar);After();return r;}
inline u32 __cdecl FakeInteresting(void* veh) {Ev& e=PushEv(8,2);e.v[0]=Tag(veh);e.v[1]=Now();const u32 r=Seq(g.c->rint,2,g.inter);After();return r;}
inline u32 __cdecl FakeLight(void* veh,int flag) {Ev& e=PushEv(9,3);e.v[0]=Tag(veh);e.v[1]=u32(flag);e.v[2]=Now();const u32 r=Seq(&g.c->rlight,1,g.light);After();return r;}
inline u32 __cdecl FakeBridge(void* veh) {Ev& e=PushEv(10,2);e.v[0]=Tag(veh);e.v[1]=Now();const u32 r=Seq(&g.c->rbridge,1,g.bridge);After();return r;}
inline void __cdecl FakeRemove(void* entity) {Ev& e=PushEv(11,2);e.v[0]=Tag(entity);e.v[1]=Now();After();}
inline void __fastcall FakeDtor(void* self,void*,int flag) {Ev& e=PushEv(12,3);e.v[0]=Tag(self);e.v[1]=u32(flag);e.v[2]=Now();After();}
inline void __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,2);e.v[0]=Tag(self);e.v[1]=Now();}   // cualquier otra ranura virtual: error visible
inline std::array<void*,48> fakeVtbl{};
inline void InitVtbl() {fakeVtbl.fill(reinterpret_cast<void*>(&FakeTrap));fakeVtbl[0]=reinterpret_cast<void*>(&FakeDtor);}

inline void SetGlobals(const C& c) {
    G32(kTimeAddr)=c.now;G32(kCamMultAddr)=c.camMult;G32(kCamLowAddr)=c.camLow;G8(kCamActiveAddr)=uint8(c.activeCam);G8(kFocusAddr)=uint8(c.focus);
    for(unsigned i=0;i<3;++i)for(unsigned k=0;k<3;++k)G8(kCamsBase+i*kCamStride+7+k)=uint8(c.cam[i*3+k]);
}

inline void CaseRemove(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);matA.Fill(c.seed*13u+2u);matB.Fill(c.seed*13u+3u);cenA.Fill(c.seed*13u+4u);clumpA.Fill(c.seed*13u+5u);
        vehA.Put32(0,u32(uintptr_t(fakeVtbl.data())));
        for(u32 i=0;i<3;++i)vehA.Put32(4+4*i,c.vp[i]);
        vehA.Put32(0x14,c.hasMat?u32(matA.Obj()):0u);vehA.Put32(0x18,ClumpPtr(c.clump));vehA.Put32(0x1C,c.entFlags);vehA.Put16(0x22,c.model);vehA.Put8(0x36,c.s36);vehA.Put8(0xB8,c.fakePhys);
        vehA.Put32(0x3B0,c.switched);vehA.Put8(0x3B9,c.style);vehA.Put8(0x3BA,c.mission);vehA.Put8(0x3BB,c.tempact);vehA.Put32(0x41C,c.target);
        vehA.Put8(0x428,c.f428);vehA.Put8(0x42A,c.f42A);vehA.Put8(0x42B,c.f42B);vehA.Put8(0x42C,c.f42C);
        vehA.Put8(0x485,c.b485);vehA.Put8(0x4A4,c.created);vehA.Put16(0x4A6,c.range);vehA.Put32(0x4D0,c.till);vehA.Put32(0x4D8,c.blown);vehA.Put32(0x594,c.vtype);
        for(u32 i=0;i<3;++i){matA.Put32(0x30+4*i,c.mp[i]);matB.Put32(0x30+4*i,c.bp[i]);cenA.Put32(4*i,c.cen[i]);}
        matA.Put32(0x28,c.upz);matB.Put32(0x28,c.bupz);
        vehA.Snap();matA.Snap();matB.Snap();cenA.Snap();clumpA.Snap();
        TagClear();vehA.Tagged();matA.Tagged();matB.Tagged();cenA.Tagged();clumpA.Tagged();
        SetGlobals(c);
        g=State{};g.c=&c;nev=0;
        const u32 arg=u32(vehA.Obj());
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,&arg,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        json v=json::object();
        v["m14"]=Tag(uintptr_t(vehA.Get32(0x14)));v["m18"]=Tag(uintptr_t(vehA.Get32(0x18)));v["m1c"]=vehA.Get32(0x1C);v["m22"]=vehA.Get16(0x22);v["m36"]=vehA.Get8(0x36);
        v["m04"]=json::array({vehA.Get32(4),vehA.Get32(8),vehA.Get32(12)});v["mb8"]=vehA.Get8(0xB8);v["m3b0"]=vehA.Get32(0x3B0);
        v["m3b9"]=vehA.Get8(0x3B9);v["m3ba"]=vehA.Get8(0x3BA);v["m3bb"]=vehA.Get8(0x3BB);v["m41c"]=Tag(uintptr_t(vehA.Get32(0x41C)));
        v["m428"]=vehA.Get8(0x428);v["m42a"]=vehA.Get8(0x42A);v["m42b"]=vehA.Get8(0x42B);v["m42c"]=vehA.Get8(0x42C);v["m485"]=vehA.Get8(0x485);v["m4a4"]=vehA.Get8(0x4A4);v["m4a6"]=vehA.Get16(0x4A6);
        v["m4d0"]=vehA.Get32(0x4D0);v["m4d8"]=vehA.Get32(0x4D8);v["m594"]=vehA.Get32(0x594);v["vptr"]=Tag(uintptr_t(vehA.Get32(0)));
        obs["v"]=v;
        obs["ma"]=json::array({matA.Get32(0x30),matA.Get32(0x34),matA.Get32(0x38),matA.Get32(0x28)});
        obs["mb"]=json::array({matB.Get32(0x30),matB.Get32(0x34),matB.Get32(0x38),matB.Get32(0x28)});
        obs["cen"]=json::array({cenA.Get32(0),cenA.Get32(4),cenA.Get32(8)});
        obs["changes"]=json::array({vehA.Changes({{0,4},{4,12},{0x14,4},{0x18,4},{0x1C,4},{0x22,2},{0x36,1},{0xB8,1},{0x3B0,4},{0x3B9,3},{0x41C,4},{0x428,8},{0x485,1},{0x4A4,1},{0x4A6,2},{0x4D0,4},{0x4D8,4},{0x594,4}}),
            matA.Changes({{0x28,4},{0x30,12}}),matB.Changes({{0x28,4},{0x30,12}}),cenA.Changes({{0,12}}),clumpA.Changes()});
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
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseRemove(env,totals,c);
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
    if(!std::filesystem::exists(dir/"car81c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car81c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car81c");}
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
    const Pair pCbd=MakeCollab(env,kCbd,hooksFound),pCrane=MakeCollab(env,kCrane,hooksFound),pAlpha=MakeCollab(env,kAlpha,hooksFound),pCentre=MakeCollab(env,kCentre,hooksFound),pOn=MakeCollab(env,kOnScreen,hooksFound),
        pLook=MakeCollab(env,kLook,hooksFound),pGarage=MakeCollab(env,kGarage,hooksFound),pInter=MakeCollab(env,kInteresting,hooksFound),pLight=MakeCollab(env,kLight,hooksFound),pBridge=MakeCollab(env,kBridge,hooksFound),
        pRemove=MakeCollab(env,kRemove,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pCbd,reinterpret_cast<void*>(&FakeCbd)),S(pCrane,reinterpret_cast<void*>(&FakeCrane)),S(pAlpha,reinterpret_cast<void*>(&FakeAlpha)),S(pCentre,reinterpret_cast<void*>(&FakeCentre)),
        S(pOn,reinterpret_cast<void*>(&FakeOnScreen)),S(pLook,reinterpret_cast<void*>(&FakeLook)),S(pGarage,reinterpret_cast<void*>(&FakeGarage)),S(pInter,reinterpret_cast<void*>(&FakeInteresting)),
        S(pLight,reinterpret_cast<void*>(&FakeLight)),S(pBridge,reinterpret_cast<void*>(&FakeBridge)),S(pRemove,reinterpret_cast<void*>(&FakeRemove))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTimer (lectura del reloj)","aritmetica y ayudantes x87 de X87Math.cpp (Car81*)"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    // helpers reales del EXE (0x420800 max, 0x40FE60 resta de vectores, 0x406DA0 magnitud al cuadrado) y limites de .rdata: no se sustituyen y deben seguir iguales
    for(const uintptr_t a:{uintptr_t(0x420800),uintptr_t(0x40FE60),uintptr_t(0x406DA0)})env.watches.emplace_back(a,16);
    for(const uintptr_t a:{uintptr_t(0x858F98),uintptr_t(0x858F94),uintptr_t(0x858F90),uintptr_t(0x858F8C),uintptr_t(0x858F88),uintptr_t(0x858CAC),uintptr_t(0x858B50)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
