#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Camera.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CamSphere76 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCamera::IsSphereVisible(const CVector&, float) (0x420D40, hook S "IsSphereVisible-mirror").
// Colaborador sustituido por un falso identico en ambos lados (parche de entrada en la direccion del EXE y en el cuerpo de la DLL): CCamera::IsSphereVisible(origen, radio, RwMatrix*) 0x420C40
// (con hook "IsSphereVisible-matrix"; validado por el banco CameraFinal4 y anteriores). La funcion probada solo lee el byte m_bMirrorActive (+0x970) y calcula las direcciones de las dos matrices (+0xA4C, +0xA94).

namespace Cs76Layout {
static_assert(offsetof(CCamera, m_bMirrorActive) == 0x970, "CCamera: m_bMirrorActive en +0x970");
static_assert(offsetof(CCamera, m_mMatInverse) == 0xA4C && offsetof(CCamera, m_mMatMirrorInverse) == 0xA94, "CCamera: matrices inversas en +0xA4C y +0xA94");
static_assert(0xA94 + 0x48 <= Cs76Fix::kCamWin, "la ventana cubre las dos matrices");
}

namespace Cs76Live {
using namespace Cs76Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Words(const u32* w,size_t count) {memcpy(m.data()+kGuard,w,count*4);}
    void Put8(ptrdiff_t off,u8 v) {m[kGuard+off]=v;}
    void Snap() {r=m;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kCamArena> camA;
inline Arena<kOrgArena> orgA;
// indices de etiqueta: 0 camara, 1 origen

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

// ================================================================================================ CCamera::IsSphereVisible(origen, radio) (0x420D40)
struct State {const C* c{};unsigned calls{};};
inline State g;
// Falso de 0x420C40: thiscall con 3 argumentos en pila (CVector*, float como dword, RwMatrix*), `ret 0xC`. Devuelve en AL res1 la primera vez y res2 la segunda, con basura en el resto de EAX.
// En la primera llamada puede cambiar el byte m_bMirrorActive (el original lo relee DESPUES).
inline u32 __fastcall FakeSphereMatrix(void* self,void*,const u32* origin,u32 radius,void* matrix) {
    Ev& e=PushEv(1,7);
    e.v[0]=Tag(self);e.v[1]=Tag(origin);
    for(int i=0;i<3;++i)e.v[2+i]=origin[i];
    e.v[5]=radius;e.v[6]=Tag(matrix);
    const bool first=g.calls++==0;
    if(first&&g.c->mutate)camA.Put8(0x970,u8(g.c->mutVal));
    return first?((g.c->hi1&0xFFFFFF00u)|(g.c->res1&0xFFu)):((g.c->hi2&0xFFFFFF00u)|(g.c->res2&0xFFu));
}

inline void CaseSphere(Env& env,Totals& t,unsigned n) {
    const C c=Make(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        camA.Fill(c.seed*29u+1u);orgA.Fill(c.seed*29u+2u);
        orgA.Words(c.origin,3);
        camA.Put8(0x970,u8(c.mirror));
        camA.Snap();orgA.Snap();
        TagClear();camA.Tagged();orgA.Tagged();
        g=State{};g.c=&c;nev=0;
        uintptr_t originPtr=orgA.Obj();
        switch(c.originKind) {
        case 1: originPtr=camA.Obj(kGuard+0x0);break;
        case 2: originPtr=camA.Obj(kGuard+0x970);break;
        case 3: originPtr=camA.Obj(kGuard+0xA4C);break;
        case 4: originPtr=camA.Obj(kGuard+0xA94);break;
        default: break;
        }
        const u32 args[2]{u32(originPtr),c.radius};
        sides[s]=Execute(env,0,0x420D40,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(camA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;
        obs["changes"]=json::array({camA.Changes(),orgA.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseSphere(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"camsphere76-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"camsphere76-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camsphere76");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpo de la DLL del colaborador: se resuelve ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t matrixBody=Body(std::bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible)));
    env.route[0].reset(new HookRoute("CCamera","IsSphereVisible-mirror",0x420D40,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=matrixBody!=0;
    env.stubs[0]={{Pair{0x420C40,matrixBody,true},reinterpret_cast<void*>(&FakeSphereMatrix)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x420D40,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
