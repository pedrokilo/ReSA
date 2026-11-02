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
#include "CarCtrl.h"
#include "PathFind.h"
#include "General.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84E dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::GenerateCarCreationCoors2 (0x424210, hook S, cdecl con 15 dwords de pila, devuelve bool en AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL
// (con su hook RH apagado durante el banco): CPathFind::Find2NodesForCarCreation 0x452090 (thiscall, 6 dwords, `ret 0x18`; sin hook en la DLL, que lo llama por direccion), CPathFind::FindNodeClosestToCoors
// 0x44F460 (thiscall, puntero de retorno + 10 dwords, `ret 0x2C`), CCamera::IsSphereVisible 0x420D40 (thiscall, `ret 8`) y CPathFind::ThisNodeWillLeadIntoADeadEnd 0x44D310 (thiscall, `ret 8`). Registran sus argumentos
// EN ORDEN (con el estado de las salidas en ese instante), devuelven lo que dice el caso y pueden mutar un nodo, una tabla del mundo de caminos, una estatica, el reloj o el punto de salida para comprobar que la funcion
// relee esos campos donde el EXE los relee. El mundo de caminos son arenas del banco enlazadas desde ThePaths (cuatro tablas por area, restauradas). Con el hook apagado corren de verdad CPathNode::GetPosition
// (0x420A10, bytes del EXE frente al C++ de la DLL), CCarPathLink::GetNodeCoors (0x420A60), srand/rand del CRT del EXE (semilla fijada por lado), sprintf del CRT del EXE (0x821BB5) y las constantes de .rdata
// (se vigilan sus bytes).

namespace C84ELayout {
static_assert(sizeof(CPathNode) == 0x1C, "CPathNode: 0x1C bytes");
static_assert(offsetof(CPathNode, m_wBaseLinkId) == 0x10 && offsetof(CPathNode, m_wAreaId) == 0x12, "CPathNode: campos");
static_assert(offsetof(CPathFind, m_pPathNodes) == 0x804 && offsetof(CPathFind, m_pNaviNodes) == 0x924 && offsetof(CPathFind, m_pNodeLinks) == 0xA44 && offsetof(CPathFind, m_pNaviLinks) == 0xDA4, "CPathFind: tablas");
}

namespace C84ELive {
using namespace C84EFix;
inline constexpr uintptr_t kEntry=0x424210;
inline constexpr uintptr_t kFind2=0x452090,kClosest=0x44F460,kVis=0x420D40,kDead=0x44D310,kGetPos=0x420A10;
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
inline Arena<kNodeArena> nodeA;
inline Arena<kNlArena> nlA;
inline Arena<kNaviArena> naviA;
inline Arena<kNnArena> nnA;
inline Arena<kOutArena> outA;

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    obs["time"]=G32(kTimeAddr);
    obs["rand_state"]=*RandStatePtr();
    obs["statics"]=json::array({G32(0x969070),G32(0x969074),G32(0x969078)});
    obs["statics2"]=json::array({G32(0x969108),G32(0x9690E8),G32(0x9690EC),G32(0x9690F0),G32(0x9690F4),G32(0x969104),G32(0x969100),G32(0x9690FC),G32(0x9690F8)});
    obs["busy"]=G8(0xC0FCB5);
    obs["gstring"]=HexBytes(reinterpret_cast<void*>(0xB71670),kGStringLen);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ GenerateCarCreationCoors2 (0x424210)
struct State {const C* c{};unsigned ev{};unsigned find2{},vis{},dead{};};
inline State g;
inline u32 Slot0Area(const C& c) {return c.areas[0];}
// Tras el evento numero mutEv, el falso muta un nodo, una tabla del mundo de caminos, una estatica, el reloj o el punto de salida
inline u32 PickAddr(const C& c,u32 v) {return c.areas[v%kSlots]|(((v>>8)%kNodesPer)<<16);}
inline void ApplyMut(u32 kind,u32 v) {
    const C& c=*g.c;
    switch(kind) {
    case 1: for(u32 i=0;i<kSlots*kNodesPer;++i) {const size_t o=size_t(i)*kNodeBytes;nodeA.Put16(o+8,v+i*37u);nodeA.Put16(o+0xA,(v>>8)-i*53u);nodeA.Put16(o+0xC,(v>>16)+i*11u);}break;
    case 2: for(u32 i=0;i<kSlots*kNodesPer;++i) {const size_t o=size_t(i)*kNodeBytes;nodeA.Put8(o+0x18,(nodeA.Get8(o+0x18)&0x0Fu)|(v&0xF0u));}break;
    case 3: outA.Put32(0,MutFloat(v));break;
    case 4: G32(0x969104)=PickAddr(c,v);break;
    case 5: G32(kTimeAddr)=v;break;
    case 6: for(u32 i=0;i<kSlots*kNodesPer;++i) nodeA.Put16(size_t(i)*kNodeBytes+0x10,(v+i)%8u);break;
    case 7: for(u32 s=0;s<kSlots;++s)for(u32 k=0;k<kLinksPer;++k) nlA.Put32((size_t(s)*kLinksPer+k)*4,PickAddr(c,v+s*16u+k));break;
    default: break;
    }
}
inline void After() {
    const C& c=*g.c;
    if(g.ev++!=c.mutEv)return;
    ApplyMut(c.mutKind,c.mutVal);
}
// Evento 1 = CPathFind::Find2NodesForCarCreation [esta ThePaths, posicion x, y, z, salida 1, salida 2, bajo trafico, reloj, numero de llamada]. thiscall, 6 dwords (ret 0x18)
inline void __fastcall FakeFind2(void* self,void*,u32 x,u32 y,u32 z,u32* o1,u32* o2,u32 low) {
    Ev& e=PushEv(1,9);
    e.v[0]=uintptr_t(self)==0x96F050u?1u:0u;e.v[1]=x;e.v[2]=y;e.v[3]=z;e.v[4]=u32(uintptr_t(o1));e.v[5]=u32(uintptr_t(o2));e.v[6]=low&0xFFu;e.v[7]=G32(kTimeAddr);e.v[8]=g.find2;
    const C& c=*g.c;
    *o1=c.f2[2*(g.find2&1)];*o2=c.f2[2*(g.find2&1)+1];
    ++g.find2;
    After();
}
// Evento 2 = CPathFind::FindNodeClosestToCoors [esta ThePaths, posicion x, y, z, tipo, alcance, unk2, unk3, unk4, barcos, unk6]. thiscall, puntero de retorno + 10 dwords (ret 0x2C); devuelve el puntero
inline u32* __fastcall FakeClosest(void* self,void*,u32* ret,u32 x,u32 y,u32 z,u32 type,u32 maxd,u32 u2,u32 u3,u32 u4,u32 boats,u32 u6) {
    Ev& e=PushEv(2,11);
    e.v[0]=uintptr_t(self)==0x96F050u?1u:0u;e.v[1]=x;e.v[2]=y;e.v[3]=z;e.v[4]=type&0xFFu;e.v[5]=maxd;e.v[6]=u2&0xFFFFu;e.v[7]=u3;e.v[8]=u4&0xFFFFu;e.v[9]=boats&0xFFFFu;e.v[10]=u6;
    *ret=g.c->closest;
    After();
    return ret;
}
// Evento 3 = CCamera::IsSphereVisible [esta TheCamera, x, y, z del punto, radio, el punto es el de salida]. thiscall (ret 8)
inline bool __fastcall FakeVis(void* self,void*,const u32* v,u32 radius) {
    Ev& e=PushEv(3,6);
    e.v[0]=uintptr_t(self)==0xB6F028u?1u:0u;e.v[1]=v[0];e.v[2]=v[1];e.v[3]=v[2];e.v[4]=radius;e.v[5]=uintptr_t(v)==outA.Obj()?1u:0u;
    const bool r=(g.c->vis[g.vis&3]&1u)!=0;
    ++g.vis;
    After();
    return r;
}
// Evento 4 = CPathFind::ThisNodeWillLeadIntoADeadEnd [esta ThePaths, nodo inicial, nodo final (etiquetas de la arena), nodo 1 y 2 de la salida]. thiscall (ret 8)
inline bool __fastcall FakeDead(void* self,void*,const void* start,const void* end) {
    Ev& e=PushEv(4,5);
    e.v[0]=uintptr_t(self)==0x96F050u?1u:0u;e.v[1]=Tag(start);e.v[2]=Tag(end);e.v[3]=outA.Get32(0x10);e.v[4]=outA.Get32(0x14);
    const bool r=(g.c->dead[g.dead&1]&1u)!=0;
    ++g.dead;
    After();
    return r;
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
        nodeA.Put16(o+8,c.node[i][0]&0xFFFFu);nodeA.Put16(o+0xA,c.node[i][0]>>16);nodeA.Put16(o+0xC,c.node[i][1]&0xFFFFu);nodeA.Put16(o+0x10,c.node[i][1]>>16);
        nodeA.Put16(o+0x12,c.node[i][2]&0xFFFFu);nodeA.Put8(o+0x18,(c.node[i][2]>>16)&0xFFu);nodeA.Put8(o+0x19,c.node[i][2]>>24);
    }
    for(u32 s=0;s<kSlots;++s)for(u32 k=0;k<kLinksPer;++k)nlA.Put32((size_t(s)*kLinksPer+k)*4,c.nlink[s][k]);
    for(u32 s=0;s<kSlots;++s)for(u32 k=0;k<kNaviPer;++k)naviA.Put16((size_t(s)*kNaviPer+k)*2,c.navi[s][k]);
    for(u32 i=0;i<kSlots*kNavNodesPer;++i) {
        const size_t o=size_t(i)*kLinkBytes;
        nnA.Put16(o,c.lk[i][0]&0xFFFFu);nnA.Put16(o+2,c.lk[i][0]>>16);nnA.Put8(o+0xB,c.lk[i][1]);
    }
}

inline void CaseRun(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        outA.Fill(c.seed*13u+6u);
        SetupWorld(c);
        outA.Snap();nodeA.Snap();nlA.Snap();naviA.Snap();nnA.Snap();
        G32(kTimeAddr)=c.now;*RandStatePtr()=c.rseed;
        G32(0x969070)=c.seq[0];G32(0x969074)=c.seq[1];G32(0x969078)=c.seq[2];
        G32(0x969108)=c.guard;G32(0x9690E8)=c.nextTime;G32(0x9690EC)=c.last[0];G32(0x9690F0)=c.last[1];G32(0x9690F4)=c.last[2];
        G32(0x969104)=c.sA;G32(0x969100)=c.sB;G32(0x9690FC)=c.sC;G32(0x9690F8)=c.sD;
        G8(0xC0FCB5)=uint8(c.busy);
        {std::array<uint8,kGStringLen> gs{};FillWords(gs.data(),gs.size(),c.seed*13u+7u);memcpy(reinterpret_cast<void*>(0xB71670),gs.data(),gs.size());}
        TagClear();TagAdd(nodeA.m.data(),nodeA.m.size());
        g=State{};g.c=&c;nev=0;
        const u32 args[15]{c.pos[0],c.pos[1],c.pos[2],c.rad,c.a3,c.a4,c.f5,c.a6,c.a7,u32(outA.Obj()),u32(outA.Obj(kGuard+0x10)),u32(outA.Obj(kGuard+0x14)),u32(outA.Obj(kGuard+0x18)),c.a12,c.a13};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,15,c.eax,c.ecx,c.edx,c.stack,60);});
        json obs;
        obs["ret"]=sides[s].reg.eax&0xFFu;
        obs["out"]=json::array({outA.Get32(0),outA.Get32(4),outA.Get32(8),outA.Get32(0x10),outA.Get32(0x14),outA.Get32(0x18)});
        obs["out_changes"]=outA.Changes({{0,12},{0x10,12}});
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
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)&&!ReversingProbe::JumpsToDll(0x821B11)&&!ReversingProbe::JumpsToDll(0x821BB5);
    // GetPosition y GetNodeCoors: con el hook apagado, la entrada del EXE es codigo original
    ready&=!ReversingProbe::JumpsToDll(kGetPos)&&!ReversingProbe::JumpsToDll(0x420A60);
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
    if(!std::filesystem::exists(dir/"car84e-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"car84e-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car84e");}
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
    const Pair pFind2=MakeCollab(env,kFind2,hooksFound);
    const Pair pClosest=MakeCollab(env,kClosest,hooksFound);
    const Pair pVis=MakeCollab(env,kVis,hooksFound);
    const Pair pDead=MakeCollab(env,kDead,hooksFound);
    // Con el hook apagado y sin falso: CPathNode::GetPosition (la DLL llama a su propio C++, el EXE a los bytes originales)
    (void)MakeCollab(env,kGetPos,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pFind2,reinterpret_cast<void*>(&FakeFind2)),S(pClosest,reinterpret_cast<void*>(&FakeClosest)),S(pVis,reinterpret_cast<void*>(&FakeVis)),S(pDead,reinterpret_cast<void*>(&FakeDead))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CPathNode::GetPosition (con el hook apagado: bytes del EXE frente al C++ de la DLL) y CCarPathLink::GetNodeCoors 0x420A60 (en el EXE; la DLL lleva el calculo en linea)",
            "srand/rand y sprintf del CRT del EXE (la DLL llama a los bytes del EXE para rand; sprintf es el de su CRT)","ayudantes x87 de X87Math.cpp (Veh83DMul, Veh83DAdd, Veh83DSub, Veh83DDiv, Veh83DSqrt, Veh83DStore)",
            "mundo de caminos: arenas del banco enlazadas desde ThePaths"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    // funciones reales del EXE que corren de verdad (GetPosition 0x420A10 con el hook apagado, 0x420A60, srand 0x821B11, rand 0x821B1E, getptd 0x827B3D, sprintf 0x821BB5) y constantes de .rdata
    // (0.125f, 10.0f, 40.0f, 230.0f, 1.5f, 0.5f, 1.0f, 0.0f y la cadena de formato): se vigilan sus bytes
    for(const uintptr_t a:{kGetPos,uintptr_t(0x420A60),uintptr_t(0x821B11),uintptr_t(0x821B1E),uintptr_t(0x827B3D),uintptr_t(0x821BB5)})env.watches.emplace_back(a,16);
    for(const uintptr_t a:{uintptr_t(0x858C48),uintptr_t(0x85862C),uintptr_t(0x858A10),uintptr_t(0x858F80),uintptr_t(0x858CE8),uintptr_t(0x858B8C),uintptr_t(0x858624),uintptr_t(0x858B50)})env.watches.emplace_back(a,4);
    env.watches.emplace_back(0x858F58,40);
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
