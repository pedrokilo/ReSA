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
#include "Localisation.h"
#include "Shadows.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxBlood79 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion de Fx_c (hook S): AddBlood (0x49EB00). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL:
// CLocalisation::Blood 0x56D230, FxSystem_c::AddParticle(v3d) 0x4AA440 y CShadows::AddPermanentShadow 0x706F60. rand() del EXE (0x821B1E) sin falso.
// Los constructores FxPrtMult_c 0x4AB270 / 0x4AB290 corren de verdad en cada lado (el EXE los suyos, la DLL los de FxPrtMult.cpp).

namespace Fb79Live {
using namespace Fb79Fix;
inline constexpr uintptr_t kEntry[kFunctions]{0x49EB00};
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
inline Arena<kVecArena> posA;       // origen / posicion
inline Arena<kVecArena> dirA;       // direccion
inline Arena<kMatArena> matCam;
inline Arena<kSysArena> sysA,sysB;
inline Arena<kThisArena> thisA;

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

// ================================================================================================ falsos de los colaboradores
struct State {const C* c{};u32 evHash{};u32 particleCalls{};};
inline State g;
// Cada evento se incorpora a un FNV-1a de TODOS los eventos (la traza guardada se limita a 256)
inline void HashEv(const Ev& e) {
    auto feed=[&](u32 v){for(unsigned b=0;b<4;++b){g.evHash^=(v>>(8*b))&0xFFu;g.evHash*=16777619u;}};
    feed(e.kind);feed(e.len);
    for(u32 i=0;i<e.len;++i)feed(e.v[i]);
}
// 0x4AA440: FxSystem_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& mult, float rotZ, float light, float limit, bool createLocal), thiscall, `ret 20h`.
// Registra el sistema (ECX), la direccion del vector de posicion (etiquetada) y TODOS los valores; con swap_slot cambia el sistema de los cuatro huecos de g_fx entre dos sistemas (el EXE los relee en cada vuelta).
inline void __fastcall FakeAddParticle(void* self,void*,const void* pos,const void* vel,u32 timeSince,const void* mult,u32 rotZ,u32 light,u32 limit,u32 createLocal) {
    Ev& e=PushEv(31,20);
    e.v[0]=Tag(self);e.v[1]=Tag(pos);
    for(unsigned i=0;i<3;++i) {e.v[2+i]=BitsOf(static_cast<const u8*>(pos)+4*i);e.v[5+i]=BitsOf(static_cast<const u8*>(vel)+4*i);}
    e.v[8]=timeSince;
    for(unsigned i=0;i<7;++i) e.v[9+i]=BitsOf(static_cast<const u8*>(mult)+4*i);
    e.v[16]=rotZ;e.v[17]=light;e.v[18]=limit;e.v[19]=createLocal;
    HashEv(e);
    ++g.particleCalls;
    if(g.c->swapSlot)G32(kSlotBlood)=G32(kSlotBlood)==u32(sysA.Obj())?u32(sysB.Obj()):u32(sysA.Obj());
}
// 0x56D230: CLocalisation::Blood(), cdecl sin argumentos; devuelve en AL lo que fija el caso.
inline bool __cdecl FakeBlood() {
    Ev& e=PushEv(51,0);HashEv(e);
    return g.c->bloodOn!=0;
}
// 0x706F60: CShadows::AddPermanentShadow(type, tex, pos, topX, topY, rightX, rightY, intensity, r, g, b, drawDistance, time, upDistance), cdecl, catorce dwords de pila (los de 8/16 bits se registran enteros).
inline void __cdecl FakeAddPermShadow(u32 type,u32 tex,const void* pos,u32 topX,u32 topY,u32 rightX,u32 rightY,u32 intensity,u32 r,u32 gr,u32 b,u32 drawDistance,u32 time,u32 up) {
    Ev& e=PushEv(61,17);
    e.v[0]=type;e.v[1]=tex;e.v[2]=Tag(pos);
    for(unsigned i=0;i<3;++i)e.v[3+i]=BitsOf(static_cast<const u8*>(pos)+4*i);
    e.v[6]=topX;e.v[7]=topY;e.v[8]=rightX;e.v[9]=rightY;e.v[10]=intensity;e.v[11]=r;e.v[12]=gr;e.v[13]=b;e.v[14]=drawDistance;e.v[15]=time;e.v[16]=up;
    HashEv(e);
}

// ------------------------------------------------------------------------------------------------ microtests de la x87 (hardware): las reglas que supone el oraculo
// op: 0 fld a; fadd b | 1 fld a; fsub b | 2 fld a; fmul b | 3 fld a; fdiv b | 4 fld a; fsqrt | 5 fld a; fistp qword (trunca) -> EAX | 6 fld a; fabs. El resultado de 0..4 y 6 es el fstp dword.
inline u32 X87Micro(u32 op,u32 a,u32 b) {
    u32 r{};
    switch(op) {
    case 0: __asm {fld dword ptr a} __asm {fadd dword ptr b} __asm {fstp dword ptr r} break;
    case 1: __asm {fld dword ptr a} __asm {fsub dword ptr b} __asm {fstp dword ptr r} break;
    case 2: __asm {fld dword ptr a} __asm {fmul dword ptr b} __asm {fstp dword ptr r} break;
    case 3: __asm {fld dword ptr a} __asm {fdiv dword ptr b} __asm {fstp dword ptr r} break;
    case 4: __asm {fld dword ptr a} __asm {fsqrt} __asm {fstp dword ptr r} break;
    case 5: {
        uint16 cw{},cwTrunc{};uint64 wide{};
        __asm {fnstcw word ptr cw}
        cwTrunc=uint16(cw|0x0C00);
        __asm {fldcw word ptr cwTrunc}
        __asm {fld dword ptr a} __asm {fistp qword ptr wide}
        __asm {fldcw word ptr cw}
        r=u32(wide&0xFFFFFFFFu);
        break;
    }
    default: __asm {fld dword ptr a} __asm {fabs} __asm {fstp dword ptr r} break;
    }
    return r;
}
inline json MicroTests() {
    json out=json::array();
    PrepareFloatingState();
    // NaN silenciosos y de senal con cargas utiles distintas (la de mayor mantisa gana; con iguales y distinto signo gana el positivo), infinitos, ceros, normales y desnormales
    static constexpr u32 vals[]{0x7FC00001u,0xFFC00002u,0x7FC12345u,0xFFC54321u,0x7F800001u,0xFFA00003u,0x7FA00000u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40A00000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,
        0xFFC00001u,0x7FC00002u,0x7FC00000u,0xFFC00000u,0xFF800001u,0x7FA00003u,0xFF800002u,0x7F800002u};   // incluye parejas de igual mantisa y signo distinto
    for(u32 op=0;op<4;++op)for(u32 a:vals)for(u32 b:vals)out.push_back({op,a,b,X87Micro(op,a,b)});
    static constexpr u32 unary[]{0x7FC00001u,0x7F800001u,0xFFC12345u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x40000000u,0x40800000u,0x00000001u,0x007FFFFFu,0x7F7FFFFFu,0x41F00000u,0x4F000000u,0x4F800000u,0x5F000000u,0x5F7FFFFFu,0xCF000000u,0x4EFFFFFFu,0x2F800000u,0x3F7FFFFFu,0xBF7FFFFFu,0x4B000001u,0x5E800000u};
    for(u32 op:{4u,5u,6u})for(u32 a:unary)out.push_back({op,a,0u,X87Micro(op,a,0u)});
    RestoreFloatingState();
    return out;
}

// ================================================================================================ el caso
inline void CaseFx(Env& env,Totals& t,unsigned fnIdx,unsigned n) {
    const C c=MakeC(fnIdx,n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        posA.Fill(c.seed*11u+1u);dirA.Fill(c.seed*11u+2u);matCam.Fill(c.seed*11u+3u);sysA.Fill(c.seed*11u+4u);sysB.Fill(c.seed*11u+5u);thisA.Fill(c.seed*11u+6u);
        for(unsigned i=0;i<3;++i) {posA.Put32(4*i,c.pos[i]);dirA.Put32(4*i,c.dir[i]);}
        thisA.Put32(kCounterOffset,c.counter0);
        SetCamera(c.hasMat,c.cam,c.other);
        G32(kSlotBlood)=u32(sysA.Obj());
        G32(kFxBase+kCounterOffset)=c.counter0;   // el g_fx real (this de thisKind 1): se restaura con la region
        G32(kBloodTex)=c.tex;
        posA.Snap();dirA.Snap();matCam.Snap();sysA.Snap();sysB.Snap();thisA.Snap();
        TagClear();posA.Tagged();dirA.Tagged();matCam.Tagged();sysA.Tagged();sysB.Tagged();thisA.Tagged();
        {volatile uint8 marker{};stackHigh=uintptr_t(&marker)+0x100;stackLow=uintptr_t(&marker)-0x20000;}
        g.c=&c;g.evHash=2166136261u;g.particleCalls=0;nev=0;
        *RandStatePtr()=c.randSeed;   // mismo estado inicial del rand() real del EXE en ambos lados
        const u32 thisPtr=c.thisKind==0?u32(thisA.Obj()):u32(kFxBase);
        const u32 args[4]{u32(posA.Obj()),u32(dirA.Obj()),c.amount,c.light};
        sides[s]=Execute(env,fnIdx,kEntry[fnIdx],candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,thisPtr,c.edx,c.stack,0);});
        json obs;   // void: se observan la traza, los sorteos consumidos, el contador y los bytes cambiados
        obs["rand_state"]=*RandStatePtr();
        obs["ev_hash"]=g.evHash;
        obs["particle_calls"]=g.particleCalls;
        obs["counter"]=G32(thisPtr+kCounterOffset);
        obs["slots"]=json::array({Tag(uintptr_t(G32(kSlotBlood)))});
        obs["changes"]=json::array({posA.Changes(),dirA.Changes(),matCam.Changes(),sysA.Changes(),sysB.Changes(),thisA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[fnIdx],n,input,rows,sides);
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
    json cases=json::object();for(unsigned f=0;f<kFunctions;++f)cases[kFn[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},{"rng_native",NativeRandom()&&!ReversingProbe::JumpsToDll(0x821B1E)&&!ReversingProbe::JumpsToDll(0x827B3D)},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"x87_microtests",MicroTests()}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseFx(env,totals,f,c);
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
    if(!std::filesystem::exists(dir/"fxblood79-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxblood79-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxblood79");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    for(unsigned f=0;f<kFunctions;++f) {
        std::string cat,name;
        const bool found=FindHook(kEntry[f],cat,name);
        if(found)env.route[f].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=found&&env.route[f]->Registered();bodies[kFn[f]]=found&&env.route[f]->Body()!=0;
        ready&=found&&env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[f]]=found?json({{"class",cat},{"name",name},{"address",kEntry[f]}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    const Pair pAdd=MakeCollab(env,0x4AA440,hooksFound);
    const Pair pBlood=MakeCollab(env,0x56D230,hooksFound);
    const Pair pShadow=MakeCollab(env,0x706F60,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[kBlood]={S(pBlood,reinterpret_cast<void*>(&FakeBlood)),S(pAdd,reinterpret_cast<void*>(&FakeAddParticle)),S(pShadow,reinterpret_cast<void*>(&FakeAddPermShadow))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    for(unsigned f=0;f<kFunctions;++f)env.watches.emplace_back(kEntry[f],16);
    env.watches.emplace_back(0x821B1E,16);   // rand real del EXE (sin falso)
    env.watches.emplace_back(0x827B3D,16);   // getptd
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
