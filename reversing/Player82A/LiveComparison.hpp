#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Player82A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::DrawTriangleForMouseRecruitPed (0x60BA80, hook S). Colaboradores sustituidos por falsos identicos en ambos lados: RwIm3DTransform 0x7EF450, RwIm3DRenderIndexedPrimitive 0x7EF550 y
// RwIm3DEnd 0x7EF520 (funciones de la DLL sin hook que llaman a la direccion del EXE: se parchea la entrada del EXE) y RwRenderStateSet (entrada fpRenderStateSet de RwEngineInstance, +0x20, que usan por igual
// el EXE y la DLL). CVector::Normalise (0x59C910) y la conversion _ftol2 (0x821B40) del EXE se ejecutan de verdad; la DLL las calcula en X87Math.cpp (Player82ATriangleCpp).
// Datos del caso: jugador y objetivo (peatones falsos en arenas, con posicion en m_placement y en la matriz), matriz de la camara (en una arena; 0xB6F03C la apunta o es nula), posicion y rumbo de la camara
// (0xB6F02C, 0xB6F038) y los tres primeros vertices Im3D (0xC4D958) y primeros indices (0xC4B958) del bufer temporal, que se rellenan al azar para ver que solo cambian posicion, color e indices.
// NaN: la x87 y SSE propagan cargas utiles de NaN distintas; todo float observado se canonicaliza (cualquier NaN = 0x7FC00000); ver README.md.

namespace Pm82Layout {
struct PmPlayer : CPlayerPed {
    static constexpr size_t OffTarget() { return offsetof(PmPlayer, m_p3rdPersonMouseTarget); }
    static constexpr size_t OffMatrix() { return offsetof(PmPlayer, m_matrix); }
    static constexpr size_t OffPlacement() { return offsetof(PmPlayer, m_placement); }
    static constexpr size_t OffHealth() { return offsetof(PmPlayer, m_fHealth); }
    static constexpr size_t OffMaxHealth() { return offsetof(PmPlayer, m_fMaxHealth); }
    static constexpr size_t OffPedType() { return offsetof(PmPlayer, m_nPedType); }
};
static_assert(PmPlayer::OffTarget() == 0x79C && PmPlayer::OffMatrix() == 0x14 && PmPlayer::OffPlacement() == 4 && PmPlayer::OffHealth() == 0x540 && PmPlayer::OffMaxHealth() == 0x544 && PmPlayer::OffPedType() == 0x598,
              "CPlayerPed: objetivo del raton +0x79C, matriz +0x14, posicion +4, vida +0x540, vida maxima +0x544, tipo +0x598");
static_assert(sizeof(RwIm3DVertex) == 0x24 && offsetof(RwIm3DVertex, color) == 0x18, "vertice Im3D de 0x24 con color +0x18");
}

namespace Pm82Live {
using namespace Pm82Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPedArena> playerA,targetA;
inline Arena<kMatArena> pmatA,tmatA,cmatA;

inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}
inline constexpr uintptr_t kVerts=0xC4D958,kIndices=0xC4B958,kCamPos=0xB6F02C,kCamHeading=0xB6F038,kCamMatrix=0xB6F03C;

inline u32 Canon(u32 w) {return (w&0x7F800000u)==0x7F800000u&&(w&0x007FFFFFu)?0x7FC00000u:w;}
// Los 27 dwords de los tres vertices (los x, y, z se canonicalizan) y los dos de los indices
inline json VertexWords() {json out=json::array();for(unsigned i=0;i<27;++i){const u32 w=*reinterpret_cast<const u32*>(kVerts+4*i);out.push_back(i%9<3?Canon(w):w);}return out;}
inline json IndexWords() {json out=json::array();for(unsigned i=0;i<2;++i)out.push_back(*reinterpret_cast<const u32*>(kIndices+4*i));return out;}

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

// ================================================================================================ DrawTriangleForMouseRecruitPed (0x60BA80)
struct State {const C* c{};};
inline State g;

// Todos __cdecl con la limpieza del original. Retornos completos (EAX) con basura en los bits altos.
inline u32 __cdecl FakeRenderStateSet(u32 state,u32 value) {Ev& e=PushEv(1,2);e.v[0]=state;e.v[1]=value;return Rand(g.c->rsRet,nev);}                          // fpRenderStateSet
inline u32 __cdecl FakeTransform(void* verts,u32 num,void* ltm,u32 flags) {                                                                                      // 0x7EF450
    Ev& e=PushEv(2,19);e.v[0]=u32(uintptr_t(verts));e.v[1]=num;e.v[2]=u32(uintptr_t(ltm));e.v[3]=flags;
    for(unsigned i=0;i<3;++i) {
        const u8* v=reinterpret_cast<const u8*>(kVerts)+0x24*i;
        for(unsigned k=0;k<3;++k)e.v[4+4*i+k]=Canon(*reinterpret_cast<const u32*>(v+4*k));
        e.v[4+4*i+3]=*reinterpret_cast<const u32*>(v+0x18);
    }
    for(unsigned i=0;i<3;++i)e.v[16+i]=*reinterpret_cast<const u16*>(kIndices+2*i);
    return g.c->xform;
}
inline u32 __cdecl FakeRenderIndexed(u32 prim,void* indices,u32 num) {Ev& e=PushEv(3,3);e.v[0]=prim;e.v[1]=u32(uintptr_t(indices));e.v[2]=num;return Rand(g.c->rsRet,nev+500);}   // 0x7EF550
inline u32 __cdecl FakeEnd() {PushEv(4,0);return Rand(g.c->rsRet,nev+900);}                                                                                       // 0x7EF520

inline void CaseDraw(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        playerA.Fill(c.seed*31u+1u);targetA.Fill(c.seed*31u+2u);pmatA.Fill(c.seed*31u+3u);tmatA.Fill(c.seed*31u+4u);cmatA.Fill(c.seed*31u+5u);
        // jugador: objetivo del raton (+0x79C), matriz (+0x14), posicion de m_placement (+4) y de la matriz (+0x30)
        playerA.Put32(0x79C,c.hasTarget?u32(targetA.Obj()):0u);
        playerA.Put32(0x14,c.pHasM?u32(pmatA.Obj()):0u);
        targetA.Put32(0x14,c.tHasM?u32(tmatA.Obj()):0u);
        for(unsigned i=0;i<3;++i) {
            playerA.Put32(4+4*i,c.pPl[i]);pmatA.Put32(0x30+4*i,c.pM[i]);targetA.Put32(4+4*i,c.tPl[i]);tmatA.Put32(0x30+4*i,c.tM[i]);cmatA.Put32(0x30+4*i,c.cM[i]);cmatA.Put32(4*i,c.cRight[i]);
        }
        targetA.Put32(0x540,c.health);targetA.Put32(0x544,c.maxHealth);targetA.Put32(0x598,c.pedType);
        // camara: posicion, rumbo y puntero a la matriz; los tres primeros vertices y los primeros indices del bufer temporal
        for(unsigned i=0;i<3;++i)*reinterpret_cast<u32*>(kCamPos+4*i)=c.cPl[i];
        *reinterpret_cast<u32*>(kCamHeading)=c.cHeading;*reinterpret_cast<u32*>(kCamMatrix)=c.cHasM?u32(cmatA.Obj()):0u;
        FillWords(reinterpret_cast<u8*>(kVerts),0x6C,c.seed*31u+6u);FillWords(reinterpret_cast<u8*>(kIndices),8,c.seed*31u+7u);
        playerA.Snap();targetA.Snap();pmatA.Snap();tmatA.Snap();cmatA.Snap();
        TagClear();playerA.Tagged();targetA.Tagged();pmatA.Tagged();tmatA.Tagged();cmatA.Tagged();
        g=State{};g.c=&c;nev=0;
        const json vbuf0=VertexWords(),ibuf0=IndexWords();
        sides[s]=Execute(env,0,0x60BA80,candidate,[&](uintptr_t entry){
            void*& fp=*reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(0xC97B24)+0x20);void* const saved=fp;fp=reinterpret_cast<void*>(&FakeRenderStateSet);
            const Reg r=Gate(entry,nullptr,0,c.eax,u32(playerA.Obj()),c.edx,c.stack,0);
            fp=saved;return r;});
        json obs;
        obs["changes"]=json::array({playerA.Changes(),targetA.Changes(),pmatA.Changes(),tmatA.Changes(),cmatA.Changes()});
        obs["events"]=EvsJson();
        obs["vbuf0"]=vbuf0;obs["vbuf1"]=VertexWords();obs["ibuf0"]=ibuf0;obs["ibuf1"]=IndexWords();
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
            for(unsigned c=0;c<kCases;++c)CaseDraw(env,totals,c);
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
    if(!std::filesystem::exists(dir/"player82a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"player82a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("player82a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    env.route[0].reset(new HookRoute("CPlayerPed","DrawTriangleForMouseRecruitPed",0x60BA80,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    // RwIm3D*: funciones de la DLL sin hook (dll = 0: llaman a la direccion del EXE); su entrada del EXE debe seguir siendo codigo original
    env.stubs[0]={{Pair{0x7EF450,0,false},reinterpret_cast<void*>(&FakeTransform)},
                  {Pair{0x7EF550,0,false},reinterpret_cast<void*>(&FakeRenderIndexed)},
                  {Pair{0x7EF520,0,false},reinterpret_cast<void*>(&FakeEnd)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"calculo del color, tamano y vertices (X87Math.cpp Player82ATriangleCpp)","RwRenderStateSet (entrada fpRenderStateSet falseada en ambos lados)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60BA80,16);
    for(const uintptr_t k:{0x858624u,0x858B50u,0x85862Cu,0x858B38u,0x86D1DCu,0x85A5C8u,0x859AACu})env.watches.emplace_back(k,4);   // 1.0, 0.0, 10.0, 0.02, 0.825, 0.175, 255.0
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
