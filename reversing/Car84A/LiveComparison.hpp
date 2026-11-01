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
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::PickNextNodeRandomly (0x42DE80, hook S, cdecl con un argumento). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado
// durante el banco): CCarCtrl::InitSequence 0x421740, ThisVehicleShouldTryNotToTurn 0x421FE0, FindPathDirection 0x422090, IsThisAnAppropriateNode 0x42DAB0, CAutoPilot::ModifySpeed 0x41B980 (thiscall) y
// CCurves::CalcSpeedScaleFactor 0x43C710. Los falsos registran sus argumentos EN ORDEN (con el estado del piloto en ese instante) y pueden mutar el vehiculo, un nodo o el reloj para comprobar que la funcion
// relee esos campos donde el EXE los relee. El mundo de caminos son arenas del banco enlazadas desde ThePaths (cuatro tablas por area, restauradas). Corren de verdad en ambos lados: srand/rand/_ftol2 del CRT del EXE
// (semilla fijada por lado), 0x44DB00 (desplazamiento del carril) y las constantes de .rdata (se vigilan sus bytes).

namespace C84ALayout {
static_assert(sizeof(CVehicle) == 0x5A0 && C84AFix::kVehWin == sizeof(CVehicle), "la arena del vehiculo es un CVehicle completo");
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390, "CAutoPilot: +0x390");
static_assert(offsetof(CAutoPilot, m_nSpeedScaleFactor) == 0x10 && offsetof(CAutoPilot, m_nNextLane) == 0x28 && offsetof(CAutoPilot, field_50) == 0x50 && offsetof(CAutoPilot, m_speed) == 0x38, "CAutoPilot: campos");
}

namespace C84ALive {
using namespace C84AFix;
inline constexpr uintptr_t kEntry=0x42DE80;
inline constexpr uintptr_t kInit=0x421740,kTurn=0x421FE0,kDir=0x422090,kIan=0x42DAB0,kModify=0x41B980,kCsf=0x43C710;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uint8& G8(uintptr_t a) {return *reinterpret_cast<uint8*>(a);}

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
};
inline Arena<kVehArena> vehA;
inline Arena<kNodeArena> nodeA;
inline Arena<kNlArena> nlA;
inline Arena<kNaviArena> naviA;
inline Arena<kNnArena> nnA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["time"]=G32(kTimeAddr);
    obs["rand_state"]=*RandStatePtr();
    obs["statics"]=json::array({G32(0x969070),G32(0x969074),G32(0x969078)});
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ PickNextNodeRandomly (0x42DE80)
struct State {const C* c{};unsigned ev{},fpd{},ian{};};
inline State g;
inline u32 Now() {return G32(kTimeAddr);}
inline u32 Rel(const void* p) {return u32(uintptr_t(p)-vehA.Obj());}
// Direcciones de nodo y de enlace validas (area cargada) de las que tira la mutacion
inline u32 PickAddr(u32 v) {
    const C& c=*g.c;std::vector<u32> l;
    for(u32 s=0;s<kSlots;++s)if(c.loaded[s])for(u32 j=0;j<kNodesPer;++j)l.push_back(c.areas[s]|(j<<16));
    return l.empty()?0u:l[v%l.size()];
}
inline u32 PickLink(u32 v) {
    const C& c=*g.c;std::vector<u32> l;
    for(u32 s=0;s<kSlots;++s)if(c.loaded[s])for(u32 j=0;j<kNavNodesPer;++j)l.push_back(j|(c.areas[s]<<10));
    return l.empty()?0u:l[v%l.size()];
}
// Tras el evento numero mutAt, el falso muta el vehiculo, un nodo o el reloj
inline void ApplyMut(u32 kind,u32 v) {
    const C& c=*g.c;
    auto nodeOfStart=[&](u32& index)->bool {
        const u32 a=vehA.Get32(0x394);
        for(u32 s=0;s<kSlots;++s)if(c.loaded[s]&&c.areas[s]==(a&0xFFFFu)&&(a>>16)<kNodesPer){index=s*kNodesPer+(a>>16);return true;}
        return false;
    };
    u32 idx{};
    switch(kind) {
    case 1: vehA.Put8(0x36,v);break;
    case 2: vehA.Put32(0x390,PickAddr(v));break;
    case 3: vehA.Put16(0x3A6,PickLink(v));break;
    case 4: vehA.Put16(0x3A4,PickLink(v));break;
    case 5: vehA.Put8(0x3B8,v);break;
    case 6: vehA.Put8(0x3B7,v);break;
    case 7: vehA.Put8(0x3B5,v);vehA.Put8(0x3B6,v>>8);break;
    case 8: vehA.Put32(0x394,PickAddr(v));break;
    case 9: vehA.Put32(0x3C8,MutFloat(v));break;
    case 10: vehA.Put8(0x3E0,v);break;
    case 11: vehA.Put32(0x39C,v);break;
    case 12: vehA.Put32(0x3A0,v);break;
    case 13: vehA.Put8(0x3DB,v);break;
    case 14: vehA.Put8(0x4A4,v);break;
    case 15: G32(kTimeAddr)=v;break;
    case 16: if(nodeOfStart(idx))nodeA.Put8(size_t(idx)*kNodeBytes+0x1A,v);break;
    case 17: if(nodeOfStart(idx))nodeA.Put8(size_t(idx)*kNodeBytes+0x18,v);break;
    case 18: vehA.Put32(0x398,v);break;
    case 19: vehA.Put8(0x3B9,v);vehA.Put8(0x3BA,v>>8);vehA.Put8(0x3BB,v>>16);break;
    case 20: vehA.Put16(0x3A8,v);vehA.Put32(0x3AC,v);break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
// Eventos: 1 InitSequence [n, hora]; 2 ThisVehicleShouldTryNotToTurn [vehiculo, actual, siguiente, anterior, estado, hora];
// 3 FindPathDirection [nodo1, nodo2, nodo3, hay puntero, siguiente del piloto, actual del piloto, estado, hora]; 4 IsThisAnAppropriateNode [vehiculo, nodo1, nodo2, nodo3, banda1, siguiente del piloto, hora];
// 5 ModifySpeed [piloto, bits, hora]; 6 CalcSpeedScaleFactor [salida x, y, z, llegada x, y, z, dx1, dy1, dx2, dy2, hora]
inline void __cdecl FakeInit(int n) {
    Ev& e=PushEv(1,2);e.v[0]=u32(n);e.v[1]=Now();
    G32(0x969078)=u32(n);G32(0x969074)=g.c->seqOff;G8(0x969070)=uint8(g.c->seqOther);
    After();
}
inline u32 __cdecl FakeTurn(void* veh) {
    Ev& e=PushEv(2,6);e.v[0]=Rel(veh);e.v[1]=vehA.Get32(0x390);e.v[2]=vehA.Get32(0x394);e.v[3]=vehA.Get32(0x398);e.v[4]=vehA.Get8(0x36);e.v[5]=Now();
    const u32 r=g.c->rtnt;After();return r;
}
inline u32 __cdecl FakeDir(u32 a1,u32 a2,u32 a3,uint8* out) {
    const u32 k=g.fpd++;const u32 idx=k<kSeq?k:kSeq-1;
    Ev& e=PushEv(3,8);e.v[0]=a1;e.v[1]=a2;e.v[2]=a3;e.v[3]=out?1u:0u;e.v[4]=vehA.Get32(0x394);e.v[5]=vehA.Get32(0x390);e.v[6]=vehA.Get8(0x36);e.v[7]=Now();
    if(out)*out=uint8(g.c->fpdOut[idx]);
    const u32 r=g.c->fpd[idx];After();return r;
}
inline u32 __cdecl FakeIan(void* veh,u32 a1,u32 a2,u32 a3,u32 band) {
    const u32 k=g.ian++;const u32 idx=k<kSeq?k:kSeq-1;
    Ev& e=PushEv(4,7);e.v[0]=Rel(veh);e.v[1]=a1;e.v[2]=a2;e.v[3]=a3;e.v[4]=band&0xFFu;e.v[5]=vehA.Get32(0x394);e.v[6]=Now();
    const u32 r=g.c->ian[idx];After();return r;
}
inline void __fastcall FakeModify(void* self,void*,u32 bits) {
    Ev& e=PushEv(5,3);e.v[0]=Rel(self);e.v[1]=bits;e.v[2]=Now();After();
}
inline float __cdecl FakeCsf(const u32* from,const u32* to,u32 dx1,u32 dy1,u32 dx2,u32 dy2) {
    Ev& e=PushEv(6,11);for(u32 i=0;i<3;++i){e.v[i]=from[i];e.v[3+i]=to[i];}e.v[6]=dx1;e.v[7]=dy1;e.v[8]=dx2;e.v[9]=dy2;e.v[10]=Now();
    const u32 r=g.c->csf;After();return std::bit_cast<float>(r);
}

// Reparto del mundo de caminos de la ranura s en las arenas
inline uintptr_t NodePtr(u32 s) {return nodeA.Obj()+size_t(s)*kNodesPer*kNodeBytes;}
inline uintptr_t NlPtr(u32 s) {return nlA.Obj()+size_t(s)*kLinksPer*4;}
inline uintptr_t NaviPtr(u32 s) {return naviA.Obj()+size_t(s)*kNaviPer*2;}
inline uintptr_t NnPtr(u32 s) {return nnA.Obj()+size_t(s)*kNavNodesPer*kLinkBytes;}

inline void SetupWorld(const C& c) {
    nodeA.Fill(c.seed*13u+2u);nlA.Fill(c.seed*13u+3u);naviA.Fill(c.seed*13u+4u);nnA.Fill(c.seed*13u+5u);
    for(u32 i=0;i<72;++i){SetPtr(kRPathNodes,i,0);SetPtr(kRNaviNodes,i,0);SetPtr(kRNodeLinks,i,0);}
    for(u32 i=0;i<64;++i)SetPtr(kRNaviLinks,i,0);
    for(u32 s=0;s<kSlots;++s) {
        if(!c.loaded[s])continue;
        SetPtr(kRPathNodes,c.areas[s],NodePtr(s));SetPtr(kRNaviNodes,c.areas[s],NnPtr(s));SetPtr(kRNodeLinks,c.areas[s],NlPtr(s));SetPtr(kRNaviLinks,c.areas[s],NaviPtr(s));
    }
    for(u32 i=0;i<kSlots*kNodesPer;++i) {
        const size_t o=size_t(i)*kNodeBytes;
        nodeA.Put16(o+8,c.node[i][0]&0xFFFFu);nodeA.Put16(o+0xA,c.node[i][0]>>16);
        nodeA.Put16(o+0x10,c.node[i][1]&0xFFFFu);nodeA.Put8(o+0x18,(c.node[i][1]>>16)&0xFFu);nodeA.Put8(o+0x19,c.node[i][1]>>24);nodeA.Put8(o+0x1A,c.node[i][2]);
    }
    for(u32 s=0;s<kSlots;++s)for(u32 k=0;k<kLinksPer;++k)nlA.Put32((size_t(s)*kLinksPer+k)*4,c.nlink[s][k]);
    for(u32 s=0;s<kSlots;++s)for(u32 k=0;k<kNaviPer;++k)naviA.Put16((size_t(s)*kNaviPer+k)*2,c.navi[s][k]);
    for(u32 i=0;i<kSlots*kNavNodesPer;++i) {
        const size_t o=size_t(i)*kLinkBytes;
        nnA.Put16(o,c.lk[i][0]&0xFFFFu);nnA.Put16(o+2,c.lk[i][0]>>16);nnA.Put16(o+4,c.lk[i][1]&0xFFFFu);nnA.Put16(o+6,c.lk[i][1]>>16);
        nnA.Put8(o+8,c.lk[i][2]);nnA.Put8(o+9,c.lk[i][2]>>8);nnA.Put8(o+0xA,c.lk[i][2]>>16);nnA.Put8(o+0xB,c.lk[i][2]>>24);
    }
}

inline void CaseRun(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);
        vehA.Put32(0,0);
        vehA.Put16(0x45E,c.seed16);vehA.Put8(0x36,c.s36);vehA.Put8(0xB8,c.b8);vehA.Put8(0x4A4,c.created);
        vehA.Put32(0x390,c.cur);vehA.Put32(0x394,c.start);vehA.Put32(0x398,c.end0);vehA.Put32(0x39C,c.fieldC);vehA.Put32(0x3A0,c.scale);
        vehA.Put16(0x3A4,c.curLink);vehA.Put16(0x3A6,c.nextLink);vehA.Put16(0x3A8,c.prevLink);vehA.Put32(0x3AC,c.t3ac);vehA.Put32(0x3B0,c.t3b0);
        vehA.Put8(0x3B4,c.sm4);vehA.Put8(0x3B5,c.sm5);vehA.Put8(0x3B6,c.sm6);vehA.Put8(0x3B7,c.lane7);vehA.Put8(0x3B8,c.lane8);
        vehA.Put8(0x3B9,c.style);vehA.Put8(0x3BA,c.mission);vehA.Put8(0x3BB,c.tempact);vehA.Put32(0x3BC,c.tempTime);vehA.Put32(0x3C8,c.speed);
        vehA.Put8(0x3DB,c.f3db);vehA.Put8(0x3E0,c.f50);
        SetupWorld(c);
        vehA.Snap();nodeA.Snap();nlA.Snap();naviA.Snap();nnA.Snap();
        G32(kTimeAddr)=c.now;*RandStatePtr()=c.rseed;
        G32(0x969070)=c.st[0];G32(0x969074)=c.st[1];G32(0x969078)=c.st[2];
        g=State{};g.c=&c;nev=0;
        const u32 arg=u32(vehA.Obj());
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,&arg,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        json v=json::object();
        v["m36"]=vehA.Get8(0x36);v["mb8"]=vehA.Get8(0xB8);v["m45e"]=vehA.Get16(0x45E);v["m4a4"]=vehA.Get8(0x4A4);
        v["m390"]=vehA.Get32(0x390);v["m394"]=vehA.Get32(0x394);v["m398"]=vehA.Get32(0x398);v["m39c"]=vehA.Get32(0x39C);v["m3a0"]=vehA.Get32(0x3A0);
        v["m3a4"]=vehA.Get16(0x3A4);v["m3a6"]=vehA.Get16(0x3A6);v["m3a8"]=vehA.Get16(0x3A8);v["m3ac"]=vehA.Get32(0x3AC);v["m3b0"]=vehA.Get32(0x3B0);
        v["m3b4"]=vehA.Get8(0x3B4);v["m3b5"]=vehA.Get8(0x3B5);v["m3b6"]=vehA.Get8(0x3B6);v["m3b7"]=vehA.Get8(0x3B7);v["m3b8"]=vehA.Get8(0x3B8);
        v["m3b9"]=vehA.Get8(0x3B9);v["m3ba"]=vehA.Get8(0x3BA);v["m3bb"]=vehA.Get8(0x3BB);v["m3bc"]=vehA.Get32(0x3BC);v["m3c8"]=vehA.Get32(0x3C8);
        v["m3db"]=vehA.Get8(0x3DB);v["m3e0"]=vehA.Get8(0x3E0);
        obs["v"]=v;
        obs["changes"]=vehA.Changes({{0x36,1},{0xB8,1},{0x45E,2},{0x4A4,1},{0x390,12},{0x39C,4},{0x3A0,4},{0x3A4,6},{0x3AC,8},{0x3B4,5},{0x3B9,3},{0x3BC,4},{0x3C8,4},{0x3DB,1},{0x3E0,1}});
        obs["node_changes"]=json::array({nodeA.Changes(),nlA.Changes(),naviA.Changes(),nnA.Changes()});
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
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)&&!ReversingProbe::JumpsToDll(0x821B11)&&!ReversingProbe::JumpsToDll(0x44DB00)&&!ReversingProbe::JumpsToDll(0x821B40);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseRun(env,totals,c);
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
    if(!std::filesystem::exists(dir/"car84a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84a");}
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
    const Pair pInit=MakeCollab(env,kInit,hooksFound),pTurn=MakeCollab(env,kTurn,hooksFound),pDir=MakeCollab(env,kDir,hooksFound),pIan=MakeCollab(env,kIan,hooksFound),
        pModify=MakeCollab(env,kModify,hooksFound),pCsf=MakeCollab(env,kCsf,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pInit,reinterpret_cast<void*>(&FakeInit)),S(pTurn,reinterpret_cast<void*>(&FakeTurn)),S(pDir,reinterpret_cast<void*>(&FakeDir)),S(pIan,reinterpret_cast<void*>(&FakeIan)),
        S(pModify,reinterpret_cast<void*>(&FakeModify)),S(pCsf,reinterpret_cast<void*>(&FakeCsf))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTimer (lectura del reloj)","FindSequenceElement (aritmetica sobre las estaticas de la secuencia)","srand/rand/_ftol2 del CRT del EXE y 0x44DB00 (en el EXE; la DLL lleva CCarPathLink::OneWayLaneOffset)",
            "ayudantes x87 de X87Math.cpp (Veh83D*, Heli83G*)","mundo de caminos: arenas del banco enlazadas desde ThePaths"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    // funciones reales del EXE que corren de verdad (0x44DB00 desplazamiento del carril, srand 0x821B11, rand 0x821B1E, getptd 0x827B3D, _ftol2 0x821B40) y constantes de .rdata: se vigilan sus bytes
    for(const uintptr_t a:{uintptr_t(0x44DB00),uintptr_t(0x821B11),uintptr_t(0x821B1E),uintptr_t(0x827B3D),uintptr_t(0x821B40)})env.watches.emplace_back(a,16);
    for(const uintptr_t a:{uintptr_t(0x858C58),uintptr_t(0x858C48),uintptr_t(0x858C50),uintptr_t(0x858FB4),uintptr_t(0x858C4C)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
