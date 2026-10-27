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
#include "Entity/Ped/Ped.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarLink74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::ClipTargetOrientationToLink (0x422760, hook S, cdecl `ret`). Unico colaborador: CGeneral::GetATanOfXY (0x53CC70, cdecl, resultado en ST0), con hook RH (apagado durante el banco):
// falso identico en la direccion del EXE y en el cuerpo de la DLL (registra x, y de cada llamada y devuelve el valor del caso); las familias 14 y 15 NO lo sustituyen (atan real en ambos lados:
// valida el fpatan que el EXE deja en ST0 en la tercera llamada). Mundo falso: vehiculo (solo +4 y +0x14), matriz (+0x30), destino de arg4, enlace (14 bytes) y las entradas
// ThePaths.m_pPathNodes[area] / m_pNaviNodes[area] (regiones restauradas tras cada lado).

namespace Cl74Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CCarPathLink) == 14, "CCarPathLink: 14 bytes");
static_assert(Cl74Fix::kVehWin >= 0x18, "la ventana del vehiculo cubre +0x14");
inline bool PathsLayoutOK() {
    return uintptr_t(&ThePaths.m_pPathNodes[0]) == Cl74Fix::kPathNodesAddr && uintptr_t(&ThePaths.m_pNaviNodes[0]) == Cl74Fix::kNaviNodesAddr
        && sizeof(ThePaths.m_pPathNodes) / sizeof(ThePaths.m_pPathNodes[0]) >= Cl74Fix::kPathSlots && sizeof(ThePaths.m_pNaviNodes) / sizeof(ThePaths.m_pNaviNodes[0]) >= Cl74Fix::kPathSlots;
}
}

namespace Cl74Live {
using namespace Cl74Fix;
inline constexpr uintptr_t kEntry=0x422760;
inline constexpr uintptr_t kAtan=0x53CC70;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matV;
inline Arena<kTgtArena> tgtA;
inline Arena<kLinkArena> linkA;

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

// ================================================================================================ ClipTargetOrientationToLink (0x422760)
struct State {const C* c{};unsigned calls{};};
inline State g;
// Evento 1: GetATanOfXY(llamada, x, y). El valor devuelto es el del caso (float en ST0).
inline float __cdecl FakeAtan(u32 x,u32 y) {
    Ev& e=PushEv(1,3);e.v[0]=g.calls;e.v[1]=x;e.v[2]=y;
    const u32 bits=g.calls<3?g.c->ret[g.calls]:0u;
    ++g.calls;
    return F(bits);
}

inline void CaseClip(Env& env,Totals& t,unsigned n,const std::vector<StubSpec>& fakeStubs) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    env.stubs[0]=c.real?std::vector<StubSpec>{}:fakeStubs;
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);matV.Fill(c.seed*13u+2u);tgtA.Fill(c.seed*13u+3u);linkA.Fill(c.seed*13u+4u);
        vehA.Put32(0x14,c.hasMat?u32(matV.Obj()):0u);vehA.Put32(4,c.ppos[0]);vehA.Put32(8,c.ppos[1]);
        matV.Put32(0x30,c.mpos[0]);matV.Put32(0x34,c.mpos[1]);
        tgtA.Put32(0,c.old);
        linkA.Put8(8,c.dirx);linkA.Put8(9,c.diry);linkA.Put8(0xA,c.width);linkA.Put8(0xB,c.flags);
        vehA.Snap();matV.Snap();tgtA.Snap();linkA.Snap();
        TagClear();vehA.Tagged();matV.Tagged();tgtA.Tagged();linkA.Tagged();
        // ThePaths: area cargada (entrada no nula) o no; la base de enlaces apunta de modo que el enlace del indice caiga en la arena
        reinterpret_cast<u32*>(kPathNodesAddr)[c.area]=c.nullNode?0u:u32(linkA.Obj());
        reinterpret_cast<u32*>(kNaviNodesAddr)[c.area]=u32(linkA.Obj())-c.idx*14u;
        g.c=&c;g.calls=0;nev=0;
        const u32 arg2=(c.hi16<<16)|(c.area<<10)|c.idx;
        const u32 args[6]{u32(vehA.Obj()),arg2,c.arg3,u32(tgtA.Obj()),c.arg5,c.arg6};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,c.seed,c.edx,c.stack,24);});
        json obs;
        obs["result"]=tgtA.Get32(0);
        obs["atan_calls"]=g.calls;
        obs["atan_exe_original"]=c.real?!ReversingProbe::JumpsToDll(kAtan):true;
        obs["changes"]=json::array({vehA.Changes(),matV.Changes(),tgtA.Changes(),linkA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators,const std::vector<StubSpec>& fakeStubs) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    const bool layout=Cl74Layout::PathsLayoutOK();
    ready&=layout;
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},{"paths_layout_ok",layout},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseClip(env,totals,c,fakeStubs);
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
    if(!std::filesystem::exists(dir/"carlink74-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carlink74-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carlink74");}
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
    if(!ready){Run(env,false,registered,bodies,collaborators,fakeStubs);return;}
    const Pair pAtan=MakeCollab(env,kAtan,hooksFound);
    fakeStubs={StubSpec{pAtan,reinterpret_cast<void*>(&FakeAtan)}};
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
    Run(env,ready,registered,bodies,collaborators,fakeStubs);
}
}
