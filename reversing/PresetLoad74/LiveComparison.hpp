#pragma once
#include <bit>
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "platform.h"
#include "MemoryMgr.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PresetLoad74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: RsLoadPresetViews (0x619D60, cdecl sin argumentos, devuelve NumPresetViews != 0). Colaboradores sustituidos por falsos identicos (entrada del EXE y cuerpo de la DLL):
//   con hook RH (apagado durante el banco): psPathnameCreate 0x745470, psPathnameDestroy 0x7454E0, RsErrorMessage 0x619B00, CMemoryMgr::Malloc 0x72F420;
//   sin hook (DLL = funcion reconstruida): RsDestroyPresetViews 0x6198C0, CMemoryMgr::Free 0x72F430;
//   sin cuerpo propio en la DLL (la DLL llama a la direccion del EXE): isprint del CRT 0x8228B5.
// Ademas se sustituyen durante la llamada las entradas de la tabla de RenderWare (RwEngineInstance + 0xC4 / + 0xF0): fopen, fclose, fgets, sprintf, strcpy, strlen y sscanf. RwOsGetFileInterface 0x804130 corre de verdad en el EXE.

namespace Pl74Layout {
static_assert(offsetof(RwGlobals, stringFuncs.vecSprintf) == 0xF0 && offsetof(RwGlobals, stringFuncs.vecStrcpy) == 0xF8 && offsetof(RwGlobals, stringFuncs.vecStrlen) == 0x120 && offsetof(RwGlobals, stringFuncs.vecSscanf) == 0x130, "RwGlobals: tabla de cadenas");
static_assert(offsetof(RwGlobals, fileFuncs) == 0xC4, "RwGlobals: tabla de ficheros");
}

namespace Pl74Live {
using namespace Pl74Fix;
inline constexpr uintptr_t kEntry=0x619D60;
inline constexpr uintptr_t kPathCreate=0x745470,kPathDestroy=0x7454E0,kErr=0x619B00,kMalloc=0x72F420,kDestroy=0x6198C0,kFree=0x72F430,kIsPrint=0x8228B5;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}

template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kHeapArena> heapA;
inline Arena<kPathArena> pathA;
inline Arena<kFileArena> fileA;

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

struct State {const C* c{};unsigned fgets{},scanf{},malloc{};size_t heapOff{};u32 ispCount{},ispHash{};};
inline State g;
// Eventos: 1 RsDestroyPresetViews (NumPresetViews, PresetViews); 2 psPathnameCreate (hash de la ruta); 3 fopen (puntero de la ruta, hash de la ruta, hash del modo); 4 psPathnameDestroy (puntero);
// 5 fgets (tamano, fichero); 6 strlen (hash); 7 sscanf (hash del buffer, hash del formato); 8 sprintf (hash del formato, hash del texto, entero); 9 Malloc (tamano, pista);
// 10 strcpy (destino, hash del origen); 11 Free (puntero); 12 fclose (fichero); 13 RsErrorMessage (hash del mensaje).
inline void __cdecl FakeDestroy() {Ev& e=PushEv(1,2);e.v[0]=G32(kNumViews);e.v[1]=Tag(uintptr_t(G32(kPresetViews)));G32(kNumViews)=0;G32(kPresetViews)=0;}
inline u32 __cdecl FakePathCreate(const char* s) {Ev& e=PushEv(2,1);e.v[0]=FnvStr(s);return g.c->pathNull?0u:u32(pathA.Obj());}
inline u32 __cdecl FakeFopen(const char* path,const char* mode) {Ev& e=PushEv(3,3);e.v[0]=Tag(path);e.v[1]=FnvStr(path);e.v[2]=FnvStr(mode);return g.c->fopenOk?u32(fileA.Obj()):0u;}
inline void __cdecl FakePathDestroy(char* p) {Ev& e=PushEv(4,1);e.v[0]=Tag(p);}
inline u32 __cdecl FakeFgets(char* buf,int size,void* file) {
    Ev& e=PushEv(5,2);e.v[0]=u32(size);e.v[1]=Tag(file);
    const unsigned k=g.fgets++;
    if(k>=g.c->nLines)return 0u;
    const Line& l=g.c->line[k];
    memcpy(buf,l.b.data(),l.len+1);
    return u32(uintptr_t(buf));
}
inline u32 __cdecl FakeStrlen(const char* s) {Ev& e=PushEv(6,1);e.v[0]=FnvStr(s);return u32(strlen(s));}
inline int __cdecl FakeScanf(const char* buf,const char* fmt,...) {
    va_list ap;va_start(ap,fmt);
    float* f[7];for(unsigned i=0;i<7;++i)f[i]=va_arg(ap,float*);
    char* name=va_arg(ap,char*);va_end(ap);
    Ev& e=PushEv(7,2);e.v[0]=FnvStr(buf);e.v[1]=FnvStr(fmt);
    const Line& l=g.c->line[g.fgets-1];
    const int ret=l.ret;
    for(int i=0;i<7&&i<ret;++i)memcpy(f[i],&l.f[i],4);
    if(ret>=8)memcpy(name,l.name.data(),l.nameLen+1);
    return ret;
}
inline int __cdecl FakeSprintf(char* dest,const char* fmt,...) {
    va_list ap;va_start(ap,fmt);
    const char* s=va_arg(ap,const char*);const int num=va_arg(ap,int);va_end(ap);
    Ev& e=PushEv(8,3);e.v[0]=FnvStr(fmt);e.v[1]=FnvStr(s);e.v[2]=u32(num);
    return sprintf_s(dest,0x40,"%s%d",s,num);
}
inline u32 __cdecl FakeMalloc(u32 size,u32 hint) {
    Ev& e=PushEv(9,2);e.v[0]=size;e.v[1]=hint;
    if(++g.malloc==g.c->failAt)return 0u;
    const size_t off=g.heapOff;g.heapOff=(off+size+15)&~size_t(15);
    return u32(heapA.Obj(kGuard+off));
}
inline u32 __cdecl FakeStrcpy(char* dest,const char* src) {Ev& e=PushEv(10,2);e.v[0]=Tag(dest);e.v[1]=FnvStr(src);strcpy(dest,src);return u32(uintptr_t(dest));}
inline void __cdecl FakeFree(void* p) {Ev& e=PushEv(11,1);e.v[0]=Tag(p);}
inline u32 __cdecl FakeFclose(void* f) {Ev& e=PushEv(12,1);e.v[0]=Tag(f);return 0u;}
inline void __cdecl FakeErr(const char* msg) {Ev& e=PushEv(13,1);e.v[0]=FnvStr(msg);}
inline int __cdecl FakeIsPrint(int c) {++g.ispCount;g.ispHash=(g.ispHash^u32(c))*16777619u;return c>=0x20&&c<=0x7E;}

inline void CaseLoad(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        heapA.Fill(c.seed*13u+1u);pathA.Fill(c.seed*13u+2u);fileA.Fill(c.seed*13u+3u);
        memcpy(pathA.m.data()+kGuard,"./views.txt",12);
        heapA.Snap();pathA.Snap();fileA.Snap();
        TagClear();heapA.Tagged();pathA.Tagged();fileA.Tagged();
        G32(kNumViews)=c.numBefore;G32(kPresetViews)=c.listBefore;G32(kCurView)=c.curBefore;
        g=State{};g.c=&c;nev=0;g.ispHash=2166136261u;
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){
            TablePatches tp;   // tablas de RenderWare: se restauran justo al volver (antes de comprobar las regiones)
            tp.Set(engineBase+kFileFopen,u32(reinterpret_cast<uintptr_t>(&FakeFopen)));
            tp.Set(engineBase+kFileFclose,u32(reinterpret_cast<uintptr_t>(&FakeFclose)));
            tp.Set(engineBase+kFileFgets,u32(reinterpret_cast<uintptr_t>(&FakeFgets)));
            tp.Set(engineBase+kStrSprintf,u32(reinterpret_cast<uintptr_t>(&FakeSprintf)));
            tp.Set(engineBase+kStrStrcpy,u32(reinterpret_cast<uintptr_t>(&FakeStrcpy)));
            tp.Set(engineBase+kStrStrlen,u32(reinterpret_cast<uintptr_t>(&FakeStrlen)));
            tp.Set(engineBase+kStrSscanf,u32(reinterpret_cast<uintptr_t>(&FakeScanf)));
            return Gate(entry,nullptr,0,c.eax,c.ecx,c.edx,c.stack,0);
        });
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;   // EAX completo solo se registra (residuo): el unico llamador, RsSavePresetView, lo lee como bool
        obs["num"]=G32(kNumViews);obs["cur"]=G32(kCurView);obs["list"]=Tag(uintptr_t(G32(kPresetViews)));
        obs["isprint"]=json::array({g.ispCount,g.ispHash});
        obs["changes"]=json::array({heapA.Changes(),pathA.Changes(),fileA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

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
            for(unsigned c=0;c<kCases;++c)CaseLoad(env,totals,c);
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
    if(!std::filesystem::exists(dir/"presetload74-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"presetload74-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("presetload74");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
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
    const Pair pCreate=MakeCollab(env,kPathCreate,hooksFound),pDestroyPath=MakeCollab(env,kPathDestroy,hooksFound),pErr=MakeCollab(env,kErr,hooksFound),pMalloc=MakeCollab(env,kMalloc,hooksFound);
    const Pair pRsDestroy{kDestroy,DllBody(&RsDestroyPresetViews),false},pFree{kFree,DllBody(&CMemoryMgr::Free),false},pIsPrint{kIsPrint,0,false};
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pRsDestroy,reinterpret_cast<void*>(&FakeDestroy)),S(pCreate,reinterpret_cast<void*>(&FakePathCreate)),S(pDestroyPath,reinterpret_cast<void*>(&FakePathDestroy)),
                  S(pIsPrint,reinterpret_cast<void*>(&FakeIsPrint)),S(pMalloc,reinterpret_cast<void*>(&FakeMalloc)),S(pFree,reinterpret_cast<void*>(&FakeFree)),S(pErr,reinterpret_cast<void*>(&FakeErr))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"RwOsGetFileInterface 0x804130 (solo el EXE; la DLL usa la tabla directamente)"})},
            {"tables",json::array({"RwFileFunctions.fopen +0xC8","RwFileFunctions.fclose +0xCC","RwFileFunctions.fgets +0xD8","RwStringFunctions.sprintf +0xF0","RwStringFunctions.strcpy +0xF8","RwStringFunctions.strlen +0x120","RwStringFunctions.sscanf +0x130"})},
            {"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
