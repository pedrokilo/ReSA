#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Core/Matrix.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par MatrixOps76 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos funciones: CMatrix::Attach (0x59BD10, hook S) y CMatrix::operator= (0x59BBC0, hook S), instalados por CMatrix::InjectHooksAttachAssign.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE; son codigo de RenderWare sin hook, la DLL los llama por su direccion del EXE):
// RwMatrixDestroy 0x7F2A20 (Attach: solo se llama si hay matriz previa y el dword de propiedad es distinto de cero) y RwMatrixUpdate 0x7F18A0 (operator=: tras escribir la RwMatrix asociada).
// Corren de verdad en ambos lados: en Attach la copia de los 12 dwords de la RwMatrix (inline en el EXE; CMatrix::Update/UpdateMatrix de la DLL) y en operator= la copia de 0x40 bytes y UpdateRwMatrix
// (0x59AD70 en el EXE, CMatrix::UpdateRwMatrix de la DLL; sin hook en ambos casos, vigiladas).

namespace Mx76Layout {
static_assert(sizeof(CMatrix) == 0x48 && sizeof(CMatrix) == Mx76Fix::kMatWin, "CMatrix mide 0x48");
static_assert(offsetof(CMatrix, m_pAttachMatrix) == 0x40 && offsetof(CMatrix, m_bOwnsAttachedMatrix) == 0x44, "CMatrix: RwMatrix asociada en +0x40 y bandera de propiedad en +0x44");
static_assert(sizeof(RwMatrix) == Mx76Fix::kRwWin, "RwMatrix mide 0x40");
}

namespace Mx76Live {
using namespace Mx76Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Words(const u32* w,size_t count) {memcpy(m.data()+kGuard,w,count*4);}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    void Snap() {r=m;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kMatArena> thisA,otherA;
inline Arena<kRwArena> rwNewA,rwOldA;
// indices de etiqueta: 0 this, 1 other, 2 rwNew, 3 rwOld (en este orden se registran)

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

// Falsos (cdecl, un argumento en pila, el llamador limpia). Los dos devuelven basura reconocible en EAX.
inline u32 __cdecl FakeDestroy(void* p) {Ev& e=PushEv(1,1);e.v[0]=Tag(p);return 0xA5A50001u;}                                              // 0x7F2A20 RwMatrixDestroy
inline void* __cdecl FakeUpdate(void* p) {                                                                                                // 0x7F18A0 RwMatrixUpdate
    Ev& e=PushEv(2,17);e.v[0]=Tag(p);
    if(p)for(int i=0;i<16;++i)e.v[1+i]=static_cast<const u32*>(p)[i];
    return p;
}

inline void Prepare(const C& c) {
    thisA.Fill(c.seed*23u+1u);otherA.Fill(c.seed*23u+2u);rwNewA.Fill(c.seed*23u+3u);rwOldA.Fill(c.seed*23u+4u);
    thisA.Words(c.thisW,18);otherA.Words(c.otherW,18);rwNewA.Words(c.rwNewW,16);rwOldA.Words(c.rwOldW,16);
}
inline void Seal() {
    thisA.Snap();otherA.Snap();rwNewA.Snap();rwOldA.Snap();
    TagClear();thisA.Tagged();otherA.Tagged();rwNewA.Tagged();rwOldA.Tagged();
    nev=0;
}
inline json Observe(const json& extra=json::object()) {
    json obs=extra;
    obs["changes"]=json::array({thisA.Changes(),otherA.Changes(),rwNewA.Changes(),rwOldA.Changes()});
    obs["events"]=EvsJson();
    obs["attach_ptr"]=Tag(uintptr_t(thisA.Get32(0x40)));   // el puntero de +0x40 despues de la llamada, como etiqueta (los bytes crudos de +0x40..0x43 son direcciones de ejecucion)
    obs["owns_word"]=thisA.Get32(0x44);
    return obs;
}

// ================================================================================================ CMatrix::Attach (0x59BD10)
inline void CaseAttach(Env& env,Totals& t,unsigned n) {
    const C c=MakeAttach(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        Prepare(c);
        const uintptr_t newPtr=c.newKind==0?rwNewA.Obj():c.newKind==1?thisA.Obj():rwOldA.Obj();
        uintptr_t oldPtr=0;
        switch(c.oldKind) {
        case 0: oldPtr=0;break;
        case 1: oldPtr=rwOldA.Obj();break;
        case 2: oldPtr=c.oldJunk;break;
        case 3: oldPtr=newPtr;break;
        default: oldPtr=thisA.Obj();break;
        }
        thisA.Put32(0x40,u32(oldPtr));thisA.Put32(0x44,c.owns);
        Seal();
        const u32 args[2]{u32(newPtr),c.boolArg};
        sides[s]=Execute(env,0,0x59BD10,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(thisA.Obj()),c.edx,c.stack,0);});
        rows[s]=CloseSide(env,sides[s],Observe());
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ CMatrix::operator= (0x59BBC0)
inline void CaseAssign(Env& env,Totals& t,unsigned n) {
    const C c=MakeAssign(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        Prepare(c);
        uintptr_t dst=0;
        switch(c.dstKind) {
        case 0: dst=0;break;
        case 1: dst=rwOldA.Obj();break;
        case 2: dst=otherA.Obj();break;
        case 3: dst=thisA.Obj();break;
        default: dst=rwNewA.Obj();break;
        }
        thisA.Put32(0x40,u32(dst));
        Seal();
        const u32 args[1]{u32(c.srcKind?thisA.Obj():otherA.Obj())};
        sides[s]=Execute(env,1,0x59BBC0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(thisA.Obj()),c.edx,c.stack,0);});
        rows[s]=CloseSide(env,sides[s],Observe());
    }
    Finish(t,kFn[1],n,input,rows,sides);
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
            for(unsigned c=0;c<kCases;++c) {
                if(f==0)CaseAttach(env,totals,c);else CaseAssign(env,totals,c);
            }
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
    if(!std::filesystem::exists(dir/"matrixops76-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"matrixops76-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("matrixops76");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    env.route[0].reset(new HookRoute("CMatrix","Attach",0x59BD10,HookRoute::RegisteredTarget{}));
    env.route[1].reset(new HookRoute("CMatrix","operator=",0x59BBC0,HookRoute::RegisteredTarget{}));
    for(unsigned f=0;f<kFunctions;++f) {
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Funciones del EXE que las DLL llaman por su direccion (sin hook): sus entradas siguen siendo codigo del EXE
    env.stubs[0]={{Pair{0x7F2A20,0,false},reinterpret_cast<void*>(&FakeDestroy)}};
    env.stubs[1]={{Pair{0x7F18A0,0,false},reinterpret_cast<void*>(&FakeUpdate)}};
    // Funciones internas SIN hook que corren de verdad en ambos lados (en el EXE 0x59AD70; en la DLL CMatrix::UpdateRwMatrix): si alguien las enganchase, el lado original saltaria a la DLL
    constexpr uintptr_t kInternals[]{0x59AD70,0x59AD20,0x59BB60,0x59BBB0,0x59ACF0};
    for(uintptr_t a:kInternals)ready&=!ReversingProbe::JumpsToDll(a);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",f==0?json::array({"CMatrix::Update (copia de los 12 dwords de la RwMatrix)"}):json::array({"CMatrix::CopyOnlyMatrix (0x40 bytes)","CMatrix::UpdateRW","CMatrix::UpdateRwMatrix (EXE: 0x59AD70)"})},{"stubbed",stubbed}};
    }
    for(uintptr_t a:kInternals)env.watches.emplace_back(a,16);
    env.watches.emplace_back(0x59BD10,16);
    env.watches.emplace_back(0x59BBC0,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
