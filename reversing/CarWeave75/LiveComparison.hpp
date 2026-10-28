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
#include "World.h"
#include "RepeatSector.h"
#include "Entity/Ped/Ped.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarWeave75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::FindAngleToWeaveThroughTraffic (0x4325C0, hook S, cdecl `ret`, float en ST0). Colaboradores, todos con hook RH (apagado durante el banco), sustituidos por falsos identicos en la direccion del EXE y en el
// cuerpo de la DLL: WeaveThroughCarsSectorList 0x42D680, WeaveThroughPedsSectorList 0x42D7E0, WeaveThroughObjectsSectorList 0x42D950 (registran lista, vehiculo, fisico, limites y los dos valores de salida, y aplican las
// escrituras programadas) y CWorld::ClearScanCodes 0x563470. Mundo falso: vehiculo (+4, +0x14, +0x44, +0x48, +0x3B9), matriz (+0x30), fisico opaco y el codigo de escaneo (0xB7CD78, region restaurada tras cada lado).

namespace Cw75Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == 0x44, "CPhysical: velocidad en +0x44");
static_assert(sizeof(CRepeatSector) == 12, "CRepeatSector: 12 bytes");
static_assert(Cw75Fix::kVehWin >= 0x3BA, "la ventana del vehiculo cubre +0x3B9");
inline bool WorldLayoutOK() {
    const uintptr_t style = reinterpret_cast<uintptr_t>(&reinterpret_cast<CVehicle*>(0)->m_autoPilot.m_nCarDrivingStyle);
    const uintptr_t peds = reinterpret_cast<uintptr_t>(&reinterpret_cast<CRepeatSector*>(0)->Peds), objects = reinterpret_cast<uintptr_t>(&reinterpret_cast<CRepeatSector*>(0)->Objects);
    return style == 0x3B9 && peds == 4 && objects == 8 && uintptr_t(&CWorld::ms_aRepeatSectors[0][0]) == Cw75Fix::kRepeatSectorsAddr && uintptr_t(&CWorld::ms_nCurrentScanCode) == Cw75Fix::kScanCodeAddr;
}
}

namespace Cw75Live {
using namespace Cw75Fix;
inline constexpr uintptr_t kEntry=0x4325C0;
inline constexpr uintptr_t kCars=0x42D680,kPeds=0x42D7E0,kObjects=0x42D950,kClear=0x563470;

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
inline Arena<kPhysArena> physA;

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

// ================================================================================================ FindAngleToWeaveThroughTraffic (0x4325C0)
struct State {const C* c{};unsigned calls{};unsigned clears{};};
inline State g;
// Escrituras programadas: en la llamada g.calls se escriben los valores que el caso indica en *p50 (arg8/arg7) y *p40 (arg9/arg8)
inline void ApplyWrites(u32* p50,u32* p40) {
    for(u32 i=0;i<g.c->nWrites;++i) {
        const Wr& w=g.c->w[i];
        if(w.call!=g.calls)continue;
        if(w.mode&1)*p50=w.v50;
        if(w.mode&2)*p40=w.v40;
    }
}
// Evento 1/2: coches / peatones (lista, vehiculo, fisico, xmin, ymin, xmax, ymax, &slot50, &slot40); evento 3: objetos (lista, vehiculo, xmin, ymin, xmax, ymax, &slot50, &slot40). Los valores de salida se leen ANTES de escribir.
inline void __cdecl FakeCars(u32 list,u32 vehicle,u32 phys,u32 a4,u32 a5,u32 a6,u32 a7,u32* p50,u32* p40) {
    Ev& e=PushEv(1,11);e.v[0]=g.calls;e.v[1]=list;e.v[2]=Tag(vehicle);e.v[3]=Tag(phys);e.v[4]=a4;e.v[5]=a5;e.v[6]=a6;e.v[7]=a7;e.v[8]=*p50;e.v[9]=*p40;e.v[10]=p50!=p40;
    SealEv(e,0x3F0u);ApplyWrites(p50,p40);++g.calls;
}
inline void __cdecl FakePeds(u32 list,u32 vehicle,u32 phys,u32 a4,u32 a5,u32 a6,u32 a7,u32* p50,u32* p40) {
    Ev& e=PushEv(2,11);e.v[0]=g.calls;e.v[1]=list;e.v[2]=Tag(vehicle);e.v[3]=Tag(phys);e.v[4]=a4;e.v[5]=a5;e.v[6]=a6;e.v[7]=a7;e.v[8]=*p50;e.v[9]=*p40;e.v[10]=p50!=p40;
    SealEv(e,0x3F0u);ApplyWrites(p50,p40);++g.calls;
}
inline void __cdecl FakeObjects(u32 list,u32 vehicle,u32 a3,u32 a4,u32 a5,u32 a6,u32* p50,u32* p40) {
    Ev& e=PushEv(3,10);e.v[0]=g.calls;e.v[1]=list;e.v[2]=Tag(vehicle);e.v[3]=a3;e.v[4]=a4;e.v[5]=a5;e.v[6]=a6;e.v[7]=*p50;e.v[8]=*p40;e.v[9]=p50!=p40;
    SealEv(e,0x1F8u);ApplyWrites(p50,p40);++g.calls;
}
inline void __cdecl FakeClear() {
    Ev& e=PushEv(4,1);e.v[0]=g.clears;SealEv(e,0u);++g.clears;
}

inline void CaseWeave(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vehA.Fill(c.seed*13u+1u);matV.Fill(c.seed*13u+2u);physA.Fill(c.seed*13u+3u);
        vehA.Put32(0x14,c.hasMat?u32(matV.Obj()):0u);vehA.Put32(4,c.ppos[0]);vehA.Put32(8,c.ppos[1]);vehA.Put32(0x44,c.vx);vehA.Put32(0x48,c.vy);vehA.Put8(0x3B9,c.style);
        matV.Put32(0x30,c.mpos[0]);matV.Put32(0x34,c.mpos[1]);
        vehA.Snap();matV.Snap();physA.Snap();
        TagClear();vehA.Tagged();matV.Tagged();physA.Tagged();
        *reinterpret_cast<uint16*>(kScanCodeAddr)=uint16(c.scan);
        g.c=&c;g.calls=0;g.clears=0;EvReset();
        const u32 phys=c.phys==0?0u:c.phys==1?u32(physA.Obj()):0xDEAD1234u;
        const u32 args[5]{u32(vehA.Obj()),phys,c.arg3,c.arg4,c.scale};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,5,c.eax,c.seed,c.edx,c.stack,20);});
        json obs;
        obs["ret_st0"]=HexBytes(sides[s].fp.data()+28,10);
        obs["scan_after"]=*reinterpret_cast<uint16*>(kScanCodeAddr);
        obs["calls"]=g.calls;obs["clears"]=g.clears;obs["ev_count"]=nev;obs["ev_hash_raw"]=evHashRaw;obs["ev_hash"]=evHashCanon;
        obs["changes"]=json::array({vehA.Changes(),matV.Changes(),physA.Changes()});
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
    const bool layout=Cw75Layout::WorldLayoutOK();
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
            for(unsigned c=0;c<kCases;++c)CaseWeave(env,totals,c);
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
    if(!std::filesystem::exists(dir/"carweave75-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carweave75-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carweave75");}
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
    const Pair pCars=MakeCollab(env,kCars,hooksFound),pPeds=MakeCollab(env,kPeds,hooksFound),pObjs=MakeCollab(env,kObjects,hooksFound),pClear=MakeCollab(env,kClear,hooksFound);
    fakeStubs={StubSpec{pCars,reinterpret_cast<void*>(&FakeCars)},StubSpec{pPeds,reinterpret_cast<void*>(&FakePeds)},StubSpec{pObjs,reinterpret_cast<void*>(&FakeObjects)},StubSpec{pClear,reinterpret_cast<void*>(&FakeClear)}};
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
