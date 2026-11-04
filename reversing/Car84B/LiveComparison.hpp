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

// Banco par a par Car84B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::PickNextNodeToChaseCar (0x426EF0, hook S, cdecl con cuatro argumentos, devuelve bool en AL). Unico colaborador sustituido por un falso identico en la direccion del EXE y en el
// cuerpo de la DLL (con su hook RH apagado durante el banco): CPathFind::DoPathSearch 0x4515D0 (thiscall, 19 dwords, `ret 0x4C`). Registra sus argumentos (con el estado del vehiculo y la zona en ese instante),
// escribe la ruta del caso (cuenta, nodos, distancia) y puede mutar el vehiculo, la matriz, un nodo o la zona del clima para comprobar que la funcion relee esos campos donde el EXE los relee. El mundo de
// caminos son arenas del banco enlazadas desde ThePaths (cuatro tablas por area, restauradas). Con el hook apagado corren de verdad StopCarIfNodesAreInvalid (0x422590) y CGeneral::GetATanOfXY (0x53CC70),
// y en ambos lados srand/rand del CRT del EXE (semilla fijada por lado), 0x44DB00 y las constantes de .rdata (se vigilan sus bytes).

namespace C84BLayout {
static_assert(sizeof(CVehicle) == 0x5A0 && C84BFix::kVehWin == sizeof(CVehicle), "la arena del vehiculo es un CVehicle completo");
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390, "CAutoPilot: +0x390");
static_assert(offsetof(CAutoPilot, m_nSpeedScaleFactor) == 0x10 && offsetof(CAutoPilot, m_nNextLane) == 0x28 && offsetof(CAutoPilot, field_50) == 0x50, "CAutoPilot: campos");
static_assert(offsetof(CVehicle, m_matrix) == 0x14 && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CEntity, m_nModelIndex) == 0x22, "CVehicle: campos");
}

namespace C84BLive {
using namespace C84BFix;
inline constexpr uintptr_t kEntry=0x426EF0;
inline constexpr uintptr_t kPath=0x4515D0,kStop=0x422590,kAtan=0x53CC70;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
inline uint16& G16(uintptr_t a) {return *reinterpret_cast<uint16*>(a);}
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
inline Arena<kMatArena> matA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["time"]=G32(kTimeAddr);
    obs["rand_state"]=*RandStatePtr();
    obs["statics"]=json::array({G32(0x969070),G32(0x969074),G32(0x969078)});
    obs["zone"]=G16(kZoneAddr);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ PickNextNodeToChaseCar (0x426EF0)
struct State {const C* c{};unsigned ev{};};
inline State g;
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
// Tras la llamada a DoPathSearch el falso muta el vehiculo, la matriz, el nodo siguiente o la zona del clima
inline void ApplyMut(u32 kind,u32 v) {
    const C& c=*g.c;
    u32 slot=0;for(u32 s=0;s<kSlots;++s)if(c.areas[s]==(c.start&0xFFFFu))slot=s;
    const size_t startOff=(size_t(slot)*kNodesPer+(c.start>>16))*kNodeBytes;
    switch(kind) {
    case 1: vehA.Put32(0x390,PickAddr(v));break;
    case 2: vehA.Put32(0x394,PickAddr(v));break;
    case 3: vehA.Put16(0x3A6,PickLink(v));break;
    case 4: vehA.Put16(0x3A4,PickLink(v));break;
    case 5: vehA.Put8(0x3B8,v);break;
    case 6: vehA.Put8(0x3B7,v);break;
    case 7: vehA.Put8(0x3B5,v);vehA.Put8(0x3B6,v>>8);break;
    case 8: vehA.Put8(0x3BA,v);break;
    case 9: vehA.Put8(0x3E0,v);break;
    case 10: vehA.Put8(0x3DB,v);break;
    case 11: vehA.Put32(0x594,(v&3)==0?10u:v);break;
    case 12: vehA.Put32(0x39C,v);break;
    case 13: vehA.Put32(0x3A0,v);break;
    case 14: G16(kZoneAddr)=uint16((v&3)==0?0u:(v&3)==1?4u:(v>>2)&0xFFu);break;
    case 15: vehA.Put32(0x14,0);break;
    case 16: vehA.Put32(4,MutFloat(v));vehA.Put32(8,MutFloat(v*5u+1u));break;
    case 17: matA.Put32(0x30,MutFloat(v));matA.Put32(0x34,MutFloat(v+7u));break;
    case 18: nodeA.Put16(startOff+0x10,v%(17u-(nodeA.Get8(startOff+0x18)&0xFu)));break;
    case 19: nodeA.Put8(startOff+0x18,(nodeA.Get8(startOff+0x18)&0xF0u)|(v%(17u-(nodeA.Get16(startOff+0x10)&0xFFFFu))));break;
    case 20: nodeA.Put16(startOff+8,v);nodeA.Put16(startOff+0xA,v>>16);break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutAt)return;
    ApplyMut(c.mutKind,c.mutVal);
}
// Evento 1 = CPathFind::DoPathSearch [esta ThePaths, tipo, origen x, y, z, nodo origen, destino x, y, z, hay ruta, hay cuenta, maximo de nodos, hay distancia, alcance, nodo de destino, limite, un solo carril,
// nodo prohibido == [0x8A5F44], con nodos sin enlaces, agua, zona del clima, actual, siguiente]. Thiscall con 19 dwords de pila (ret 0x4C): __fastcall con los dos primeros argumentos en ECX y EDX.
inline void __fastcall FakePath(void* self,void*,u32 type,u32 ox,u32 oy,u32 oz,u32 oaddr,u32 tx,u32 ty,u32 tz,u32* nodes,int16_t* count,u32 maxNodes,u32* dist,u32 maxSearch,u32 hint,u32 maxUnk,
                                u32 oneSide,u32 forbidden,u32 incl,u32 water) {
    Ev& e=PushEv(1,23);
    e.v[0]=uintptr_t(self)==0x96F050u?1u:0u;e.v[1]=type&0xFFu;e.v[2]=ox;e.v[3]=oy;e.v[4]=oz;e.v[5]=oaddr;e.v[6]=tx;e.v[7]=ty;e.v[8]=tz;e.v[9]=nodes?1u:0u;e.v[10]=count?1u:0u;e.v[11]=maxNodes;e.v[12]=dist?1u:0u;
    e.v[13]=maxSearch;e.v[14]=hint;e.v[15]=maxUnk;e.v[16]=oneSide&0xFFu;e.v[17]=forbidden==G32(kForbiddenAddr)?1u:0u;e.v[18]=incl&0xFFu;e.v[19]=water&0xFFu;e.v[20]=G16(kZoneAddr);
    e.v[21]=vehA.Get32(0x390);e.v[22]=vehA.Get32(0x394);
    const C& c=*g.c;
    const int16_t n=int16_t(c.cnt);
    if(count)*count=n;
    if(dist)*dist=c.psDist;
    if(nodes&&n>=1)nodes[0]=c.ps0;
    if(nodes&&n>=2)nodes[1]=c.ps1;
    After();
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
        matA.Fill(c.seed*13u+6u);
        matA.Put32(0x30,c.mat[0]);matA.Put32(0x34,c.mat[1]);matA.Put32(0x38,c.mat[2]);
        vehA.Put32(4,c.place[0]);vehA.Put32(8,c.place[1]);vehA.Put32(0xC,c.place[2]);
        vehA.Put32(0x14,c.hasMat?u32(matA.Obj()):0u);
        vehA.Put16(0x22,c.model);
        vehA.Put16(0x45E,c.seed16);
        vehA.Put32(0x390,c.cur);vehA.Put32(0x394,c.start);vehA.Put32(0x398,c.end0);vehA.Put32(0x39C,c.fieldC);vehA.Put32(0x3A0,c.scale);
        vehA.Put16(0x3A4,c.curLink);vehA.Put16(0x3A6,c.nextLink);vehA.Put16(0x3A8,c.prevLink);
        vehA.Put8(0x3B4,c.sm4);vehA.Put8(0x3B5,c.sm5);vehA.Put8(0x3B6,c.sm6);vehA.Put8(0x3B7,c.lane7);vehA.Put8(0x3B8,c.lane8);
        vehA.Put8(0x3BA,c.mission);vehA.Put8(0x3DB,c.f3db);vehA.Put8(0x3E0,c.f50);vehA.Put32(0x594,c.subtype);
        SetupWorld(c);
        vehA.Snap();nodeA.Snap();nlA.Snap();naviA.Snap();nnA.Snap();matA.Snap();
        G32(kTimeAddr)=c.now;*RandStatePtr()=c.rseed;G16(kZoneAddr)=uint16(c.zone);
        G32(0x969070)=c.st[0];G32(0x969074)=c.st[1];G32(0x969078)=c.st[2];
        g=State{};g.c=&c;nev=0;
        const u32 args[4]{u32(vehA.Obj()),c.dest[0],c.dest[1],c.dest[2]};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,c.ecx,c.edx,c.stack,16);});
        json obs;
        json v=json::object();
        v["m04"]=vehA.Get32(4);v["m08"]=vehA.Get32(8);v["m0c"]=vehA.Get32(0xC);v["m14nz"]=vehA.Get32(0x14)!=0;v["m22"]=vehA.Get16(0x22);v["m45e"]=vehA.Get16(0x45E);
        v["m390"]=vehA.Get32(0x390);
        {const u32 w=vehA.Get32(0x394);v["m394"]=(w&0xFFFFu)==0xFFFFu?(w&0xFFFFu):w;}   // el EXE deja basura de la pila en la palabra alta si ningun enlace sirve
        v["m398"]=vehA.Get32(0x398);v["m39c"]=vehA.Get32(0x39C);v["m3a0"]=vehA.Get32(0x3A0);
        v["m3a4"]=vehA.Get16(0x3A4);v["m3a6"]=vehA.Get16(0x3A6);v["m3a8"]=vehA.Get16(0x3A8);
        v["m3b4"]=vehA.Get8(0x3B4);v["m3b5"]=vehA.Get8(0x3B5);v["m3b6"]=vehA.Get8(0x3B6);v["m3b7"]=vehA.Get8(0x3B7);v["m3b8"]=vehA.Get8(0x3B8);
        v["m3ba"]=vehA.Get8(0x3BA);v["m3db"]=vehA.Get8(0x3DB);v["m3e0"]=vehA.Get8(0x3E0);v["m594"]=vehA.Get32(0x594);
        obs["v"]=v;
        obs["ret"]=sides[s].reg.eax&0xFFu;
        obs["mat_pos"]=json::array({matA.Get32(0x30),matA.Get32(0x34),matA.Get32(0x38)});
        obs["changes"]=vehA.Changes({{4,12},{0x14,4},{0x22,2},{0x390,12},{0x39C,4},{0x3A0,4},{0x3A4,6},{0x3B4,5},{0x3BA,1},{0x3DB,1},{0x3E0,1},{0x594,4}});
        obs["node_changes"]=json::array({nodeA.Changes(),nlA.Changes(),naviA.Changes(),nnA.Changes(),matA.Changes({{0x30,12}})});
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
    // StopCarIfNodesAreInvalid y GetATanOfXY: con el hook apagado, la entrada del EXE es codigo original
    ready&=!ReversingProbe::JumpsToDll(kStop)&&!ReversingProbe::JumpsToDll(kAtan);
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
    if(!std::filesystem::exists(dir/"car84b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84b");}
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
    const Pair pPath=MakeCollab(env,kPath,hooksFound);
    // Con el hook apagado y sin falso: StopCarIfNodesAreInvalid y CGeneral::GetATanOfXY (la DLL llama a su propio C++, el EXE a los bytes originales)
    (void)MakeCollab(env,kStop,hooksFound);(void)MakeCollab(env,kAtan,hooksFound); (void)MakeCollab(env,0x44DB00,hooksFound); // 0x44DB00 (CCarPathLink::OneWayLaneOffset) ya tiene hook RH (lote 93A): se apaga para que el EXE ejecute sus bytes reales
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pPath,reinterpret_cast<void*>(&FakePath))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"StopCarIfNodesAreInvalid y CGeneral::GetATanOfXY (con el hook apagado: bytes del EXE frente al C++ de la DLL)","srand/rand del CRT del EXE y 0x44DB00 (en el EXE; la DLL lleva CCarPathLink::OneWayLaneOffset)",
            "ayudantes x87 de X87Math.cpp (Veh83CSubStore, Car84B*)","mundo de caminos: arenas del banco enlazadas desde ThePaths"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    // funciones reales del EXE que corren de verdad (0x44DB00 desplazamiento del carril, srand 0x821B11, rand 0x821B1E, getptd 0x827B3D, _ftol2 0x821B40, 0x422590 y 0x53CC70 con el hook apagado) y constantes de .rdata:
    // se vigilan sus bytes
    for(const uintptr_t a:{uintptr_t(0x44DB00),uintptr_t(0x821B11),uintptr_t(0x821B1E),uintptr_t(0x827B3D),uintptr_t(0x821B40),kStop,kAtan})env.watches.emplace_back(a,16);
    for(const uintptr_t a:{uintptr_t(0x858B3C),uintptr_t(0x858C48),uintptr_t(0x858CB8),uintptr_t(0x858CBC),uintptr_t(0x858CC0),uintptr_t(0x858B50),uintptr_t(0x858FB4)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
