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
#include "PathFind.h"
#include "NodeAddress.h"
#include "Collision/Collision.h"
#include "Entity/Ped/Ped.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarNodes75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::FindNodesThisCarIsNearestTo (0x42BD20, hook S, cdecl `ret`, sin retorno). Colaboradores, todos con hook RH (apagado durante el banco), sustituidos por falsos identicos en la direccion del EXE y en
// el cuerpo de la DLL: CPathFind::FindXRegionForCoors 0x44D890, FindYRegionForCoors 0x44D8C0, FindXCoorsForRegion 0x44D8F0, FindYCoorsForRegion 0x44D910 (thiscall), CCollision::DistToLine 0x417610 (cdecl) y
// CVector::Normalise 0x59C910 (thiscall), que devuelven/escriben lo que programa el caso. Mundo falso de rutas: hasta 4 casillas (ThePaths.m_pPathNodes / m_pNodeLinks / m_anNumVehicleNodes de las areas del caso,
// regiones restauradas tras cada lado), vehiculo (+4, +0x14), matriz (+0x10 adelante, +0x30 posicion) y las dos salidas.

namespace Cn75Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CNodeAddress) == 4 && offsetof(CNodeAddress, m_wAreaId) == 0 && offsetof(CNodeAddress, m_wNodeId) == 2, "CNodeAddress: area en los 16 bits bajos y nodo en los altos");
inline bool WorldLayoutOK() {
    return uintptr_t(&ThePaths) == Cn75Fix::kThePaths && uintptr_t(&ThePaths.m_pPathNodes[0]) == Cn75Fix::kPathNodesAddr && uintptr_t(&ThePaths.m_pNodeLinks[0]) == Cn75Fix::kNodeLinksAddr
        && uintptr_t(&ThePaths.m_anNumVehicleNodes[0]) == Cn75Fix::kNumVehNodesAddr && sizeof(ThePaths.m_pPathNodes) / sizeof(ThePaths.m_pPathNodes[0]) == Cn75Fix::kPathSlots
        && reinterpret_cast<uintptr_t>(&reinterpret_cast<CMatrix*>(0)->GetForward()) == 0x10 && reinterpret_cast<uintptr_t>(&reinterpret_cast<CMatrix*>(0)->GetPosition()) == 0x30;
}
}

namespace Cn75Live {
using namespace Cn75Fix;
inline constexpr uintptr_t kEntry=0x42BD20;
inline constexpr uintptr_t kXRegion=0x44D890,kYRegion=0x44D8C0,kXCoors=0x44D8F0,kYCoors=0x44D910,kDist=0x417610,kNormalise=0x59C910;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+kGuard+off,&w,2);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matV;
inline Arena<kOutArena> out1,out2;
inline std::array<Arena<kNodesArena>,kSlots> nodeA;
inline std::array<Arena<kTableArena>,kSlots> tableA;

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

// ================================================================================================ FindNodesThisCarIsNearestTo (0x42BD20)
struct State {const C* c{};unsigned dists{};unsigned norms{};};
inline State g;
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL. Regiones y coordenadas de region: thiscall (ecx = ThePaths, un argumento, `ret 4`) -> __fastcall con EDX sin usar.
// Evento 7/8: FindX/YRegionForCoors(self, bits del float); 9/10: FindX/YCoorsForRegion(self, region); 11: DistToLine(inicio xyz, fin xyz, punto xyz); 12: Normalise (entrada xyz; escribe la salida del guion).
inline u32 __fastcall FakeXRegion(void* self,void*,u32 bits) {
    Ev& e=PushEv(7,2);e.v[0]=u32(uintptr_t(self));e.v[1]=bits;SealEv(e,0x2u);return g.c->regionX;
}
inline u32 __fastcall FakeYRegion(void* self,void*,u32 bits) {
    Ev& e=PushEv(8,2);e.v[0]=u32(uintptr_t(self));e.v[1]=bits;SealEv(e,0x2u);return g.c->regionY;
}
inline float __fastcall FakeXCoors(void* self,void*,u32 region) {
    Ev& e=PushEv(9,2);e.v[0]=u32(uintptr_t(self));e.v[1]=region;SealEv(e,0u);return F(g.c->xCoord);
}
inline float __fastcall FakeYCoors(void* self,void*,u32 region) {
    Ev& e=PushEv(10,2);e.v[0]=u32(uintptr_t(self));e.v[1]=region;SealEv(e,0u);return F(g.c->yCoord);
}
inline float __cdecl FakeDist(const u32* a,const u32* b,const u32* p) {
    Ev& e=PushEv(11,10);e.v[0]=g.dists;for(int i=0;i<3;++i){e.v[1+i]=a[i];e.v[4+i]=b[i];e.v[7+i]=p[i];}
    SealEv(e,0x3FEu);
    return F(g.c->dist[g.dists++%kScripts]);
}
inline void __fastcall FakeNormalise(u32* v,void*) {
    Ev& e=PushEv(12,4);e.v[0]=g.norms;for(int i=0;i<3;++i)e.v[1+i]=v[i];
    SealEv(e,0xEu);
    for(int i=0;i<3;++i)v[i]=g.c->norm[g.norms%kScripts][i];
    ++g.norms;
}

inline void CaseNodes(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);matV.Fill(c.seed*13u+2u);out1.Fill(c.seed*13u+3u);out2.Fill(c.seed*13u+4u);
        for(unsigned i=0;i<kSlots;++i){nodeA[i].Fill(c.seed*13u+10u+i);tableA[i].Fill(c.seed*13u+20u+i);}
        vehA.Put32(0x14,c.hasMat?u32(matV.Obj()):0u);vehA.Put32(4,c.ppos[0]);vehA.Put32(8,c.ppos[1]);vehA.Put32(0xC,c.ppos[2]);
        matV.Put32(0x30,c.mpos[0]);matV.Put32(0x34,c.mpos[1]);matV.Put32(0x38,c.mpos[2]);
        matV.Put32(0x10,c.fwd[0]);matV.Put32(0x14,c.fwd[1]);matV.Put32(0x18,c.fwd[2]);
        // mundo de rutas: se anulan las tres tablas y se rellenan las casillas del caso
        memset(reinterpret_cast<void*>(kPathNodesAddr),0,kPathSlots*4);memset(reinterpret_cast<void*>(kNodeLinksAddr),0,kPathSlots*4);memset(reinterpret_cast<void*>(kNumVehNodesAddr),0,kPathSlots*4);
        for(unsigned i=0;i<c.nSlots;++i) {
            const Slot& sl=c.s[i];
            for(u32 k=0;k<sl.np;++k) {
                const Node& nd=sl.nd[k];
                nodeA[i].Put16(k*0x1C+0x8,u32(nd.x));nodeA[i].Put16(k*0x1C+0xA,u32(nd.y));nodeA[i].Put16(k*0x1C+0xC,u32(nd.z));nodeA[i].Put16(k*0x1C+0x10,u32(nd.base));nodeA[i].Put8(k*0x1C+0x18,nd.flags);
            }
            for(u32 k=0;k<sl.nt;++k)tableA[i].Put32((size_t(kTableBias)+size_t(sl.tIdx[k]))*4,sl.tVal[k]);
            reinterpret_cast<u32*>(kPathNodesAddr)[sl.area]=sl.loaded?u32(nodeA[i].Obj()):0u;
            reinterpret_cast<u32*>(kNodeLinksAddr)[sl.area]=u32(tableA[i].Obj(kGuard+size_t(kTableBias)*4));
            reinterpret_cast<u32*>(kNumVehNodesAddr)[sl.area]=sl.count;
        }
        vehA.Snap();matV.Snap();out1.Snap();out2.Snap();
        for(unsigned i=0;i<kSlots;++i){nodeA[i].Snap();tableA[i].Snap();}
        TagClear();vehA.Tagged();matV.Tagged();out1.Tagged();out2.Tagged();
        for(unsigned i=0;i<kSlots;++i){nodeA[i].Tagged();tableA[i].Tagged();}
        g.c=&c;g.dists=0;g.norms=0;EvReset();
        const u32 args[3]{u32(vehA.Obj()),u32(out1.Obj()),u32(out2.Obj())};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,c.seed,c.edx,c.stack,12);});
        json obs;
        // salidas: (area, nodo); con area 0xFFFF el EXE deja en el nodo un resto de pila (ningun llamador lo lee): se registra como nulo
        auto outJson=[&](const Arena<kOutArena>& a) {
            const u32 v=a.Get32(0);const u32 area=v&0xFFFFu,node=v>>16;
            json changes=json::array();
            for(const auto& ch:a.Changes())if(ch[0].get<int64>()<0||ch[0].get<int64>()>=4)changes.push_back(ch);
            return json::array({area,area==0xFFFFu?json(nullptr):json(node),changes});
        };
        obs["out1"]=outJson(out1);obs["out2"]=outJson(out2);
        obs["dist_calls"]=g.dists;obs["norm_calls"]=g.norms;obs["ev_count"]=nev;obs["ev_hash_raw"]=evHashRaw;obs["ev_hash"]=evHashCanon;
        json others=json::array({vehA.Changes(),matV.Changes()});
        for(unsigned i=0;i<kSlots;++i){others.push_back(nodeA[i].Changes());others.push_back(tableA[i].Changes());}
        obs["changes"]=others;
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
    const bool layout=Cn75Layout::WorldLayoutOK();
    ready&=layout;
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},{"world_layout_ok",layout},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseNodes(env,totals,c);
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
    if(!std::filesystem::exists(dir/"carnodes75-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carnodes75-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carnodes75");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    std::vector<StubSpec> fakeStubs;
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
    const Pair pXR=MakeCollab(env,kXRegion,hooksFound),pYR=MakeCollab(env,kYRegion,hooksFound),pXC=MakeCollab(env,kXCoors,hooksFound),pYC=MakeCollab(env,kYCoors,hooksFound),pDist=MakeCollab(env,kDist,hooksFound);
    // CVector::Normalise: CVector::InjectHooks esta desactivado (sin hook RH); el cuerpo de la DLL es la funcion propia y la entrada del EXE sigue siendo codigo original: se parchean ambos.
    const Pair pNorm{kNormalise,Body(std::bit_cast<uintptr_t>(&CVector::Normalise)),false};
    hooksFound.push_back({{"exe",kNormalise},{"class","CVector"},{"name","Normalise"},{"hook",false}});
    fakeStubs={StubSpec{pXR,reinterpret_cast<void*>(&FakeXRegion)},StubSpec{pYR,reinterpret_cast<void*>(&FakeYRegion)},StubSpec{pXC,reinterpret_cast<void*>(&FakeXCoors)},StubSpec{pYC,reinterpret_cast<void*>(&FakeYCoors)},
        StubSpec{pDist,reinterpret_cast<void*>(&FakeDist)},StubSpec{pNorm,reinterpret_cast<void*>(&FakeNormalise)}};
    env.stubs[0]=fakeStubs;
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
