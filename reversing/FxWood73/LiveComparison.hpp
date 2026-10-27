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
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxWood73 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: Fx_c::AddWood (0x49EE10, hook S). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL:
//   FxSystem_c::AddParticle(v3d) 0x4AA440 (thiscall, con hook RH apagado durante el banco) y el rand() del EXE 0x821B1E (sin hook: la DLL llama a la direccion del EXE).
// El constructor FxPrtMult_c 0x4AB290 corre de verdad en cada lado (el EXE el suyo, la DLL el de FxPrtMult.cpp).

namespace Fw73Live {
using namespace Fw73Fix;
inline constexpr uintptr_t kEntry=0x49EE10;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kVecArena> posA,dirA;
inline Arena<kMatArena> matCam;
inline Arena<kSysArena> sysA,sysB;
inline Arena<kEntArena> entA;

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
// La camara: con matriz, la posicion usada esta en +0x30 de la matriz y la compacta (+4) es ruido; sin matriz (puntero nulo), al reves.
inline void SetCamera(bool hasMat,const u32* used,const u32* other) {
    for(unsigned i=0;i<3;++i) {G32(kCamPlacement+4*i)=hasMat?other[i]:used[i];matCam.Put32(0x30+4*i,hasMat?used[i]:other[i]);}
    G32(kCamMatrixPtr)=hasMat?u32(matCam.Obj()):0u;
}

// ================================================================================================ Fx_c::AddWood (0x49EE10)
struct State {const C* c{};u32 evHash{};u32 particleCalls{};};
inline State g;
// rand() NO se sustituye: ambos lados llaman al CRT real del EXE (0x821B1E) con el mismo estado inicial (RandStatePtr), que se compara al final.
// 0x4AA440: FxSystem_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& mult, float rotZ, float light, float limit, bool createLocal), thiscall, `ret 20h`.
// Registra el sistema (ECX), la direccion del vector de posicion (etiquetada) y TODOS los valores; con swap_slot cambia g_fx.m_Blood entre dos sistemas (el EXE lo relee en cada vuelta).
inline void __fastcall FakeAddParticle(void* self,void*,const void* pos,const void* vel,u32 timeSince,const void* mult,u32 rotZ,u32 light,u32 limit,u32 createLocal) {
    Ev& e=PushEv(31,20);
    e.v[0]=Tag(self);e.v[1]=Tag(pos);
    for(unsigned i=0;i<3;++i) {e.v[2+i]=BitsOf(static_cast<const u8*>(pos)+4*i);e.v[5+i]=BitsOf(static_cast<const u8*>(vel)+4*i);}
    e.v[8]=timeSince;
    for(unsigned i=0;i<7;++i) e.v[9+i]=BitsOf(static_cast<const u8*>(mult)+4*i);
    e.v[16]=rotZ;e.v[17]=light;e.v[18]=limit;e.v[19]=createLocal;
    for(unsigned i=0;i<20;++i)for(unsigned b=0;b<4;++b){g.evHash^=(e.v[i]>>(8*b))&0xFFu;g.evHash*=16777619u;}   // hash de TODOS los eventos (la traza guardada se limita a 256)
    ++g.particleCalls;
    if(g.c->swapSlot)G32(kBloodSlot)=G32(kBloodSlot)==u32(sysA.Obj())?u32(sysB.Obj()):u32(sysA.Obj());
}

inline void CaseWood(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        posA.Fill(c.seed*5u+1u);dirA.Fill(c.seed*5u+2u);matCam.Fill(c.seed*5u+3u);sysA.Fill(c.seed*5u+4u);sysB.Fill(c.seed*5u+5u);entA.Fill(c.seed*5u+6u);
        for(unsigned i=0;i<3;++i) {posA.Put32(4*i,c.pos[i]);dirA.Put32(4*i,c.dir[i]);}
        SetCamera(c.hasMat,c.cam,c.other);
        G32(kBloodSlot)=u32(sysA.Obj());
        posA.Snap();dirA.Snap();matCam.Snap();sysA.Snap();sysB.Snap();entA.Snap();
        TagClear();posA.Tagged();dirA.Tagged();matCam.Tagged();sysA.Tagged();sysB.Tagged();entA.Tagged();
        g.c=&c;g.evHash=2166136261u;g.particleCalls=0;nev=0;
        *RandStatePtr()=c.randSeed;   // mismo estado inicial del rand() real del EXE en ambos lados
        const u32 thisPtr=c.thisKind==0?u32(entA.Obj()):c.thisKind==1?0xA9AE00u:c.thisKind==2?(0xDEAD0000u|(Rand(n,77)&0xFFFFu)):0u;
        const u32 args[4]{u32(posA.Obj()),u32(dirA.Obj()),c.amount,c.light};
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,thisPtr,c.edx,c.stack,0);});
        json obs;   // void: se observan la traza, los sorteos consumidos y los bytes cambiados
        obs["rand_state"]=*RandStatePtr();
        obs["ev_hash"]=g.evHash;
        obs["particle_calls"]=g.particleCalls;
        obs["blood_slot"]=Tag(uintptr_t(G32(kBloodSlot)));
        obs["changes"]=json::array({posA.Changes(),dirA.Changes(),matCam.Changes(),sysA.Changes(),sysB.Changes(),entA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    ready&=NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D);   // rand real del EXE en ambos lados
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
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseWood(env,totals,c);
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
    if(!std::filesystem::exists(dir/"fxwood73-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxwood73-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxwood73");}
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
    const Pair pAdd=MakeCollab(env,0x4AA440,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pAdd,reinterpret_cast<void*>(&FakeAddParticle))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    env.watches.emplace_back(0x827B3D,16);   // getptd
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
