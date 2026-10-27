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
#include "PlayerInfo.h"
#include "WeaponEffects.h"
#include "Sprite.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CrosshairRender74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CWeaponEffects::RenderPlayerCrosshair (0x56EF90, thiscall con this = &m_nCrosshairActivated, `ret 4`: indice de jugador; sin valor de retorno que lea nadie, CWeaponEffects::Render 0x742E66).
// Colaboradores sustituidos por falsos identicos en ambos lados:
//   RwRenderStateSet: la entrada fpRenderStateSet de la tabla de RenderWare (RwEngineInstance + 0x20; cdecl, dos argumentos) se sustituye durante la llamada: el EXE la llama por la tabla y la DLL tambien (macro de RenderWare).
//   CSprite::RenderOneXLUSprite_Rotate_Aspect 0x70D490 (cdecl, 12 dwords): falso en la entrada del EXE y en el cuerpo de la DLL (hook S apagado durante el banco).
// El resto (historiales 0xB9B8F8/0xB9B920/0xB9B948, indice y tabla de texturas, reloj, RsGlobal) son datos del juego fijados por el banco por caso y comparados despues.

namespace Cr74Layout {
struct InfoProbe : CPlayerInfo {
    static constexpr size_t OffCross() { return offsetof(InfoProbe, m_nCrosshairActivated); }
    static constexpr size_t OffTarget() { return offsetof(InfoProbe, m_vecCrosshairTarget); }
};
static_assert(InfoProbe::OffCross() == Cr74Fix::kCrossOff, "CPlayerInfo: m_nCrosshairActivated en +0x158");
static_assert(InfoProbe::OffTarget() == Cr74Fix::kCrossOff + 4, "CPlayerInfo: m_vecCrosshairTarget en +0x15C");
static_assert(sizeof(CPlayerInfo) == Cr74Fix::kInfoWin, "CPlayerInfo: tamano 0x190");
static_assert(offsetof(RwTexture, raster) == 0, "RwTexture: raster es el primer campo");
}

namespace Cr74Live {
using namespace Cr74Fix;
inline constexpr uintptr_t kEntry=0x56EF90;
inline constexpr uintptr_t kSprite=0x70D490;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

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
inline Arena<kInfoArena> infoA;
inline Arena<kTexArena> texA;

inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["history"]=HexBytes(reinterpret_cast<const void*>(kHistT),0x78);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// Eventos: 1 RwRenderStateSet(estado, valor); 2 RenderOneXLUSprite_Rotate_Aspect (doce dwords en el orden de la pila).
inline u32 __cdecl FakeState(u32 state,u32 value) {Ev& e=PushEv(1,2);e.v[0]=state;e.v[1]=value;return 1;}
inline void __cdecl FakeSprite(u32 x,u32 y,u32 z,u32 w,u32 h,u32 r,u32 g,u32 b,u32 intensity,u32 rz,u32 rotation,u32 alpha) {
    Ev& e=PushEv(2,12);e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=w;e.v[4]=h;e.v[5]=r;e.v[6]=g;e.v[7]=b;e.v[8]=intensity;e.v[9]=rz;e.v[10]=rotation;e.v[11]=alpha;
}
inline uintptr_t StateSlot() {return *reinterpret_cast<const uintptr_t*>(0xC97B24)+0x20;}

inline void CaseRender(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        infoA.Fill(c.seed*13u+1u);texA.Fill(c.seed*13u+2u);
        infoA.Put32(kCrossOff,c.active);infoA.Put32(kCrossOff+4,c.tx);infoA.Put32(kCrossOff+8,c.ty);
        texA.Put32(0,c.raster);
        infoA.Snap();texA.Snap();
        TagClear();infoA.Tagged();texA.Tagged();
        G32(kTimeAddr)=c.time;G32(kWidthAddr)=c.width;G32(kHeightAddr)=c.height;G32(kTexIdxAddr)=c.texIdx;
        G32(kTexTable+4*c.texIdx)=u32(texA.Obj());
        for(unsigned i=0;i<kHist;++i) {G32(kHistT+4*i)=c.ht[i];G32(kHistY+4*i)=c.hy[i];G32(kHistX+4*i)=c.hx[i];}
        nev=0;
        const u32 args[1]{c.player};
        const u32 self=u32(infoA.Obj())+u32(kCrossOff);
        const uintptr_t slot=StateSlot();
        const uintptr_t savedSlot=*reinterpret_cast<const uintptr_t*>(slot);
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){
            *reinterpret_cast<uintptr_t*>(slot)=uintptr_t(&FakeState);
            const Reg r=Gate(entry,args,1,c.eax,self,c.edx,c.stack,0);
            *reinterpret_cast<uintptr_t*>(slot)=savedSlot;
            return r;
        });
        json obs;
        obs["changes"]=json::array({infoA.Changes(),texA.Changes()});
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
            for(unsigned c=0;c<kCases;++c)CaseRender(env,totals,c);
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
    if(!std::filesystem::exists(dir/"crosshairrender74-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"crosshairrender74-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("crosshairrender74");}
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
    const Pair pSprite=MakeCollab(env,kSprite,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pSprite,reinterpret_cast<void*>(&FakeSprite))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"tables",json::array({"RwEngineInstance + 0x20 (fpRenderStateSet)"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
